/* SPDX-License-Identifier: (GPL-2.0-only OR LGPL-2.1-only)
 *
 * ring_buffer_backend.c
 *
 * Copyright (C) 2005-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/stddef.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>

#include <wrapper/cpu.h>
#include <wrapper/mm.h>
#include <wrapper/vmalloc.h>	/* for wrapper_vmalloc_sync_mappings() */
#include <ringbuffer/config.h>
#include <ringbuffer/backend.h>
#include <ringbuffer/frontend.h>

/**
 * lib_ring_buffer_backend_allocate - allocate a channel buffer
 * @config: ring buffer instance configuration
 * @buf: the buffer struct
 * @size: total size of the buffer
 * @num_subbuf: number of subbuffers
 * @extra_reader_sb: need extra subbuffer for reader
 */
static
int lib_ring_buffer_backend_allocate(const struct lttng_kernel_ring_buffer_config *config,
				     struct lttng_kernel_ring_buffer_backend *bufb,
				     size_t size, size_t num_subbuf,
				     int extra_reader_sb)
{
	struct channel_backend *chanb = &bufb->chan->backend;
	unsigned long j, num_pages, num_pages_per_subbuf, page_idx = 0;
	unsigned long subbuf_size, mmap_offset = 0;
	unsigned long num_subbuf_alloc;
	struct page **pages;
	unsigned long i;

	num_pages = size >> PAGE_SHIFT;

	/*
	 * Verify that there is enough free pages available on the system for
	 * the current allocation request.
	 * wrapper_check_enough_free_pages uses si_mem_available() if available
	 * and returns if there should be enough free pages based on the
	 * current estimate.
	 */
	if (!wrapper_check_enough_free_pages(num_pages))
		goto not_enough_pages;

	/*
	 * Set the current user thread as the first target of the OOM killer.
	 * If the estimate received by si_mem_available() was off, and we do
	 * end up running out of memory because of this buffer allocation, we
	 * want to kill the offending app first.
	 */
	set_current_oom_origin();

	num_pages_per_subbuf = num_pages >> get_count_order(num_subbuf);
	subbuf_size = chanb->subbuf_size;
	num_subbuf_alloc = num_subbuf;

	if (extra_reader_sb) {
		num_pages += num_pages_per_subbuf; /* Add pages for reader */
		num_subbuf_alloc++;
	}

	pages = vmalloc_node(ALIGN(sizeof(*pages) * num_pages,
				   1 << INTERNODE_CACHE_SHIFT),
			cpu_to_node(max(bufb->cpu, 0)));
	if (unlikely(!pages))
		goto pages_error;

	bufb->array = lttng_kvmalloc_node(ALIGN(sizeof(*bufb->array)
					 * num_subbuf_alloc,
				  1 << INTERNODE_CACHE_SHIFT),
			GFP_KERNEL | __GFP_NOWARN,
			cpu_to_node(max(bufb->cpu, 0)));
	if (unlikely(!bufb->array))
		goto array_error;

	for (i = 0; i < num_pages; i++) {
		pages[i] = alloc_pages_node(cpu_to_node(max(bufb->cpu, 0)),
				GFP_KERNEL | __GFP_NOWARN | __GFP_ZERO, 0);
		if (unlikely(!pages[i]))
			goto depopulate;
	}
	bufb->num_pages_per_subbuf = num_pages_per_subbuf;

	/* Allocate backend pages array elements */
	for (i = 0; i < num_subbuf_alloc; i++) {
		bufb->array[i] =
			lttng_kvzalloc_node(ALIGN(
				sizeof(struct lttng_kernel_ring_buffer_backend_pages) +
				sizeof(struct lttng_kernel_ring_buffer_backend_page)
				* num_pages_per_subbuf,
				1 << INTERNODE_CACHE_SHIFT),
				GFP_KERNEL | __GFP_NOWARN,
				cpu_to_node(max(bufb->cpu, 0)));
		if (!bufb->array[i])
			goto free_array;
	}

	/* Allocate write-side subbuffer table */
	bufb->buf_wsb = lttng_kvzalloc_node(ALIGN(
				sizeof(struct lttng_kernel_ring_buffer_backend_subbuffer)
				* num_subbuf,
				1 << INTERNODE_CACHE_SHIFT),
				GFP_KERNEL | __GFP_NOWARN,
				cpu_to_node(max(bufb->cpu, 0)));
	if (unlikely(!bufb->buf_wsb))
		goto free_array;

	for (i = 0; i < num_subbuf; i++)
		bufb->buf_wsb[i].id = subbuffer_id(config, 0, 1, i);

	/* Assign read-side subbuffer table */
	if (extra_reader_sb)
		bufb->buf_rsb.id = subbuffer_id(config, 0, 1,
						num_subbuf_alloc - 1);
	else
		bufb->buf_rsb.id = subbuffer_id(config, 0, 1, 0);

	/* Allocate subbuffer packet counter table */
	bufb->buf_cnt = lttng_kvzalloc_node(ALIGN(
				sizeof(struct lttng_kernel_ring_buffer_backend_counts)
				* num_subbuf,
				1 << INTERNODE_CACHE_SHIFT),
			GFP_KERNEL | __GFP_NOWARN,
			cpu_to_node(max(bufb->cpu, 0)));
	if (unlikely(!bufb->buf_cnt))
		goto free_wsb;

	/* Assign pages to page index */
	for (i = 0; i < num_subbuf_alloc; i++) {
		for (j = 0; j < num_pages_per_subbuf; j++) {
			CHAN_WARN_ON(chanb, page_idx > num_pages);
			bufb->array[i]->p[j].virt = page_address(pages[page_idx]);
			bufb->array[i]->p[j].pfn = page_to_pfn(pages[page_idx]);
			page_idx++;
		}
		if (config->output == RING_BUFFER_MMAP) {
			bufb->array[i]->mmap_offset = mmap_offset;
			mmap_offset += subbuf_size;
		}
	}

	/*
	 * If kmalloc ever uses vmalloc underneath, make sure the buffer pages
	 * will not fault.
	 */
	wrapper_vmalloc_sync_mappings();
	clear_current_oom_origin();
	vfree(pages);
	return 0;

free_wsb:
	lttng_kvfree(bufb->buf_wsb);
free_array:
	for (i = 0; (i < num_subbuf_alloc && bufb->array[i]); i++)
		lttng_kvfree(bufb->array[i]);
depopulate:
	/* Free all allocated pages */
	for (i = 0; (i < num_pages && pages[i]); i++)
		__free_page(pages[i]);
	lttng_kvfree(bufb->array);
array_error:
	vfree(pages);
pages_error:
	clear_current_oom_origin();
not_enough_pages:
	return -ENOMEM;
}

int lib_ring_buffer_backend_create(struct lttng_kernel_ring_buffer_backend *bufb,
				   struct channel_backend *chanb, int cpu)
{
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;

	bufb->chan = container_of(chanb, struct lttng_kernel_ring_buffer_channel, backend);
	bufb->cpu = cpu;

	return lib_ring_buffer_backend_allocate(config, bufb, chanb->buf_size,
						chanb->num_subbuf,
						chanb->extra_reader_sb);
}

void lib_ring_buffer_backend_free(struct lttng_kernel_ring_buffer_backend *bufb)
{
	struct channel_backend *chanb = &bufb->chan->backend;
	unsigned long i, j, num_subbuf_alloc;

	num_subbuf_alloc = chanb->num_subbuf;
	if (chanb->extra_reader_sb)
		num_subbuf_alloc++;

	lttng_kvfree(bufb->buf_wsb);
	lttng_kvfree(bufb->buf_cnt);
	for (i = 0; i < num_subbuf_alloc; i++) {
		for (j = 0; j < bufb->num_pages_per_subbuf; j++)
			__free_page(pfn_to_page(bufb->array[i]->p[j].pfn));
		lttng_kvfree(bufb->array[i]);
	}
	lttng_kvfree(bufb->array);
	bufb->allocated = 0;
}

void lib_ring_buffer_backend_reset(struct lttng_kernel_ring_buffer_backend *bufb)
{
	struct channel_backend *chanb = &bufb->chan->backend;
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;
	unsigned long num_subbuf_alloc;
	unsigned int i;

	num_subbuf_alloc = chanb->num_subbuf;
	if (chanb->extra_reader_sb)
		num_subbuf_alloc++;

	for (i = 0; i < chanb->num_subbuf; i++)
		bufb->buf_wsb[i].id = subbuffer_id(config, 0, 1, i);
	if (chanb->extra_reader_sb)
		bufb->buf_rsb.id = subbuffer_id(config, 0, 1,
						num_subbuf_alloc - 1);
	else
		bufb->buf_rsb.id = subbuffer_id(config, 0, 1, 0);

	for (i = 0; i < num_subbuf_alloc; i++) {
		/* Don't reset mmap_offset */
		v_set(config, &bufb->array[i]->records_commit, 0);
		v_set(config, &bufb->array[i]->records_unread, 0);
		bufb->array[i]->data_size = 0;
		/* Don't reset backend page and virt addresses */
	}
	/* Don't reset num_pages_per_subbuf, cpu, allocated */
	v_set(config, &bufb->records_read, 0);
}

/*
 * The frontend is responsible for also calling ring_buffer_backend_reset for
 * each buffer when calling channel_backend_reset.
 */
void channel_backend_reset(struct channel_backend *chanb)
{
	struct lttng_kernel_ring_buffer_channel *chan = container_of(chanb, struct lttng_kernel_ring_buffer_channel, backend);
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;

	/*
	 * Don't reset buf_size, subbuf_size, subbuf_size_order,
	 * num_subbuf_order, buf_size_order, extra_reader_sb, num_subbuf,
	 * priv, notifiers, config, cpumask and name.
	 */
	chanb->start_timestamp = config->cb.ring_buffer_clock_read(chan);
}

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0))

/*
 * No need to implement a "dead" callback to do a buffer switch here,
 * because it will happen when tracing is stopped, or will be done by
 * switch timer CPU DEAD callback.
 * We don't free buffers when CPU go away, because it would make trace
 * data vanish, which is unwanted.
 */
int lttng_cpuhp_rb_backend_prepare(unsigned int cpu,
		struct lttng_cpuhp_node *node)
{
	struct channel_backend *chanb = container_of(node,
			struct channel_backend, cpuhp_prepare);
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;
	struct lttng_kernel_ring_buffer *buf;
	int ret;

	CHAN_WARN_ON(chanb, config->alloc == RING_BUFFER_ALLOC_PER_CHANNEL);

	buf = per_cpu_ptr(chanb->buf, cpu);
	ret = lib_ring_buffer_create(buf, chanb, cpu);
	if (ret) {
		printk(KERN_ERR
		  "LTTng: ring_buffer_cpu_hp_callback: cpu %d "
		  "buffer creation failed\n", cpu);
		return ret;
	}
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_cpuhp_rb_backend_prepare);

#else /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0)) */

#ifdef CONFIG_HOTPLUG_CPU

/**
 *	lib_ring_buffer_cpu_hp_callback - CPU hotplug callback
 *	@nb: notifier block
 *	@action: hotplug action to take
 *	@hcpu: CPU number
 *
 *	Returns the success/failure of the operation. (%NOTIFY_OK, %NOTIFY_BAD)
 */
static
int lib_ring_buffer_cpu_hp_callback(struct notifier_block *nb,
					      unsigned long action,
					      void *hcpu)
{
	unsigned int cpu = (unsigned long)hcpu;
	struct channel_backend *chanb = container_of(nb, struct channel_backend,
						     cpu_hp_notifier);
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;
	struct lttng_kernel_ring_buffer *buf;
	int ret;

	CHAN_WARN_ON(chanb, config->alloc == RING_BUFFER_ALLOC_PER_CHANNEL);

	switch (action) {
	case CPU_UP_PREPARE:
	case CPU_UP_PREPARE_FROZEN:
		buf = per_cpu_ptr(chanb->buf, cpu);
		ret = lib_ring_buffer_create(buf, chanb, cpu);
		if (ret) {
			printk(KERN_ERR
			  "LTTng: ring_buffer_cpu_hp_callback: cpu %d "
			  "buffer creation failed\n", cpu);
			return NOTIFY_BAD;
		}
		break;
	case CPU_DEAD:
	case CPU_DEAD_FROZEN:
		/* No need to do a buffer switch here, because it will happen
		 * when tracing is stopped, or will be done by switch timer CPU
		 * DEAD callback. */
		break;
	}
	return NOTIFY_OK;
}

#endif

#endif /* #else #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0)) */

/**
 * channel_backend_init - initialize a channel backend
 * @chanb: channel backend
 * @name: channel name
 * @config: client ring buffer configuration
 * @priv: client private data
 * @parent: dentry of parent directory, %NULL for root directory
 * @subbuf_size: size of sub-buffers (> PAGE_SIZE, power of 2)
 * @num_subbuf: number of sub-buffers (power of 2)
 *
 * Returns channel pointer if successful, %NULL otherwise.
 *
 * Creates per-cpu channel buffers using the sizes and attributes
 * specified.  The created channel buffer files will be named
 * name_0...name_N-1.  File permissions will be %S_IRUSR.
 *
 * Called with CPU hotplug disabled.
 */
int channel_backend_init(struct channel_backend *chanb,
			 const char *name,
			 const struct lttng_kernel_ring_buffer_config *config,
			 void *priv, size_t subbuf_size, size_t num_subbuf)
{
	struct lttng_kernel_ring_buffer_channel *chan = container_of(chanb, struct lttng_kernel_ring_buffer_channel, backend);
	unsigned int i;
	int ret;

	if (!name)
		return -EPERM;

	/* Check that the subbuffer size is larger than a page. */
	if (subbuf_size < PAGE_SIZE)
		return -EINVAL;

	/*
	 * Make sure the number of subbuffers and subbuffer size are
	 * power of 2 and nonzero.
	 */
	if (!subbuf_size || (subbuf_size & (subbuf_size - 1)))
		return -EINVAL;
	if (!num_subbuf || (num_subbuf & (num_subbuf - 1)))
		return -EINVAL;
	/*
	 * Overwrite mode buffers require at least 2 subbuffers per
	 * buffer.
	 */
	if (config->mode == RING_BUFFER_OVERWRITE && num_subbuf < 2)
		return -EINVAL;

	ret = subbuffer_id_check_index(config, num_subbuf);
	if (ret)
		return ret;

	chanb->priv = priv;
	chanb->buf_size = num_subbuf * subbuf_size;
	chanb->subbuf_size = subbuf_size;
	chanb->buf_size_order = get_count_order(chanb->buf_size);
	chanb->subbuf_size_order = get_count_order(subbuf_size);
	chanb->num_subbuf_order = get_count_order(num_subbuf);
	chanb->extra_reader_sb =
			(config->mode == RING_BUFFER_OVERWRITE) ? 1 : 0;
	chanb->num_subbuf = num_subbuf;
	strscpy(chanb->name, name, NAME_MAX);
	memcpy(&chanb->config, config, sizeof(chanb->config));

	if (config->alloc == RING_BUFFER_ALLOC_PER_CPU) {
		if (!zalloc_cpumask_var(&chanb->cpumask, GFP_KERNEL))
			return -ENOMEM;
	}

	if (config->alloc == RING_BUFFER_ALLOC_PER_CPU) {
		/* Allocating the buffer per-cpu structures */
		chanb->buf = alloc_percpu(struct lttng_kernel_ring_buffer);
		if (!chanb->buf)
			goto free_cpumask;

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0))
		chanb->cpuhp_prepare.component = LTTNG_RING_BUFFER_BACKEND;
		ret = cpuhp_state_add_instance(lttng_rb_hp_prepare,
			&chanb->cpuhp_prepare.node);
		if (ret)
			goto free_bufs;
#else /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0)) */

		{
			/*
			 * In case of non-hotplug cpu, if the ring-buffer is allocated
			 * in early initcall, it will not be notified of secondary cpus.
			 * In that off case, we need to allocate for all possible cpus.
			 */
#ifdef CONFIG_HOTPLUG_CPU
			/*
			 * buf->backend.allocated test takes care of concurrent CPU
			 * hotplug.
			 * Priority higher than frontend, so we create the ring buffer
			 * before we start the timer.
			 */
			chanb->cpu_hp_notifier.notifier_call =
					lib_ring_buffer_cpu_hp_callback;
			chanb->cpu_hp_notifier.priority = 5;
			register_hotcpu_notifier(&chanb->cpu_hp_notifier);

			lttng_cpus_read_lock();
			for_each_online_cpu(i) {
				ret = lib_ring_buffer_create(per_cpu_ptr(chanb->buf, i),
							 chanb, i);
				if (ret)
					goto free_bufs;	/* cpu hotplug locked */
			}
			lttng_cpus_read_unlock();
#else
			for_each_possible_cpu(i) {
				ret = lib_ring_buffer_create(per_cpu_ptr(chanb->buf, i),
							 chanb, i);
				if (ret)
					goto free_bufs;
			}
#endif
		}
#endif /* #else #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0)) */
	} else {
		chanb->buf = kzalloc(sizeof(struct lttng_kernel_ring_buffer), GFP_KERNEL);
		if (!chanb->buf)
			goto free_cpumask;
		ret = lib_ring_buffer_create(chanb->buf, chanb, -1);
		if (ret)
			goto free_bufs;
	}
	chanb->start_timestamp = config->cb.ring_buffer_clock_read(chan);

	return 0;

free_bufs:
	if (config->alloc == RING_BUFFER_ALLOC_PER_CPU) {
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0))
		/*
		 * Teardown of lttng_rb_hp_prepare instance
		 * on "add" error is handled within cpu hotplug,
		 * no teardown to do from the caller.
		 */
#else /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0)) */
#ifdef CONFIG_HOTPLUG_CPU
		lttng_cpus_read_unlock();
		unregister_hotcpu_notifier(&chanb->cpu_hp_notifier);
#endif
#endif /* #else #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0)) */
		for_each_possible_cpu(i) {
			struct lttng_kernel_ring_buffer *buf =
				per_cpu_ptr(chanb->buf, i);

			if (!buf->backend.allocated)
				continue;
			lib_ring_buffer_free(buf);
		}
		free_percpu(chanb->buf);
	} else
		kfree(chanb->buf);
free_cpumask:
	if (config->alloc == RING_BUFFER_ALLOC_PER_CPU)
		free_cpumask_var(chanb->cpumask);
	return -ENOMEM;
}

/**
 * channel_backend_unregister_notifiers - unregister notifiers
 * @chan: the channel
 *
 * Holds CPU hotplug.
 */
void channel_backend_unregister_notifiers(struct channel_backend *chanb)
{
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;

	if (config->alloc == RING_BUFFER_ALLOC_PER_CPU) {
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0))
		int ret;

		ret = cpuhp_state_remove_instance(lttng_rb_hp_prepare,
				&chanb->cpuhp_prepare.node);
		WARN_ON(ret);
#else /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0)) */
		unregister_hotcpu_notifier(&chanb->cpu_hp_notifier);
#endif /* #else #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0)) */
	}
}

/**
 * channel_backend_free - destroy the channel
 * @chan: the channel
 *
 * Destroy all channel buffers and frees the channel.
 */
void channel_backend_free(struct channel_backend *chanb)
{
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;
	unsigned int i;

	if (config->alloc == RING_BUFFER_ALLOC_PER_CPU) {
		for_each_possible_cpu(i) {
			struct lttng_kernel_ring_buffer *buf = per_cpu_ptr(chanb->buf, i);

			if (!buf->backend.allocated)
				continue;
			lib_ring_buffer_free(buf);
		}
		free_cpumask_var(chanb->cpumask);
		free_percpu(chanb->buf);
	} else {
		struct lttng_kernel_ring_buffer *buf = chanb->buf;

		CHAN_WARN_ON(chanb, !buf->backend.allocated);
		lib_ring_buffer_free(buf);
		kfree(buf);
	}
}

/**
 * lib_ring_buffer_write - write data to a ring_buffer buffer.
 * @bufb : buffer backend
 * @offset : offset within the buffer
 * @src : source address
 * @len : length to write
 */
void _lib_ring_buffer_write(struct lttng_kernel_ring_buffer_backend *bufb, size_t offset,
			    const void *src, size_t len)
{
	struct channel_backend *chanb = &bufb->chan->backend;
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;
	size_t sbidx, index, bytes_left_in_page;
	struct lttng_kernel_ring_buffer_backend_pages *rpages;
	unsigned long sb_bindex, id;

	do {
		sbidx = offset >> chanb->subbuf_size_order;
		index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;

		/*
		 * Underlying layer should never ask for writes across
		 * subbuffers.
		 */
		CHAN_WARN_ON(chanb, offset >= chanb->buf_size);

		bytes_left_in_page = min_t(size_t, len, PAGE_SIZE - (offset & ~PAGE_MASK));
		id = bufb->buf_wsb[sbidx].id;
		sb_bindex = subbuffer_id_get_index(config, id);
		rpages = bufb->array[sb_bindex];
		CHAN_WARN_ON(chanb, config->mode == RING_BUFFER_OVERWRITE
			     && subbuffer_id_is_noref(config, id));
		lib_ring_buffer_do_copy(config,
					rpages->p[index].virt
						+ (offset & ~PAGE_MASK),
					src, bytes_left_in_page);
		len -= bytes_left_in_page;
		src += bytes_left_in_page;
		offset += bytes_left_in_page;
	} while (unlikely(len));
}
EXPORT_SYMBOL_GPL(_lib_ring_buffer_write);

/**
 * lib_ring_buffer_memset - write len bytes of c to a ring_buffer buffer.
 * @bufb : buffer backend
 * @offset : offset within the buffer
 * @c : the byte to write
 * @len : length to write
 */
void _lib_ring_buffer_memset(struct lttng_kernel_ring_buffer_backend *bufb,
			     size_t offset, int c, size_t len)
{
	struct channel_backend *chanb = &bufb->chan->backend;
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;
	size_t sbidx, index, bytes_left_in_page;
	struct lttng_kernel_ring_buffer_backend_pages *rpages;
	unsigned long sb_bindex, id;

	do {
		sbidx = offset >> chanb->subbuf_size_order;
		index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;

		/*
		 * Underlying layer should never ask for writes across
		 * subbuffers.
		 */
		CHAN_WARN_ON(chanb, offset >= chanb->buf_size);

		bytes_left_in_page = min_t(size_t, len, PAGE_SIZE - (offset & ~PAGE_MASK));
		id = bufb->buf_wsb[sbidx].id;
		sb_bindex = subbuffer_id_get_index(config, id);
		rpages = bufb->array[sb_bindex];
		CHAN_WARN_ON(chanb, config->mode == RING_BUFFER_OVERWRITE
			     && subbuffer_id_is_noref(config, id));
		lib_ring_buffer_do_memset(rpages->p[index].virt
					  + (offset & ~PAGE_MASK),
					  c, bytes_left_in_page);
		len -= bytes_left_in_page;
		offset += bytes_left_in_page;
	} while (unlikely(len));
}
EXPORT_SYMBOL_GPL(_lib_ring_buffer_memset);

/**
 * lib_ring_buffer_strcpy - write string data to a ring_buffer buffer.
 * @bufb : buffer backend
 * @offset : offset within the buffer
 * @src : source address
 * @len : length to write
 * @pad : character to use for padding
 */
void _lib_ring_buffer_strcpy(struct lttng_kernel_ring_buffer_backend *bufb,
			size_t offset, const char *src, size_t len, int pad)
{
	struct channel_backend *chanb = &bufb->chan->backend;
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;
	size_t sbidx, index, bytes_left_in_page;
	struct lttng_kernel_ring_buffer_backend_pages *rpages;
	unsigned long sb_bindex, id;
	bool src_terminated = false;

	CHAN_WARN_ON(chanb, !len);
	do {
		sbidx = offset >> chanb->subbuf_size_order;
		index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;

		/*
		 * Underlying layer should never ask for writes across
		 * subbuffers.
		 */
		CHAN_WARN_ON(chanb, offset >= chanb->buf_size);

		bytes_left_in_page = min_t(size_t, len, PAGE_SIZE - (offset & ~PAGE_MASK));
		id = bufb->buf_wsb[sbidx].id;
		sb_bindex = subbuffer_id_get_index(config, id);
		rpages = bufb->array[sb_bindex];
		CHAN_WARN_ON(chanb, config->mode == RING_BUFFER_OVERWRITE
			     && subbuffer_id_is_noref(config, id));

		if (likely(!src_terminated)) {
			size_t count, to_copy;

			to_copy = bytes_left_in_page;
			if (bytes_left_in_page == len)
				to_copy--;	/* Final '\0' */
			count = lib_ring_buffer_do_strcpy(config,
					rpages->p[index].virt
						+ (offset & ~PAGE_MASK),
					src, to_copy);
			offset += count;
			/* Padding */
			if (unlikely(count < to_copy)) {
				size_t pad_len = to_copy - count;

				/* Next pages will have padding */
				src_terminated = true;
				lib_ring_buffer_do_memset(rpages->p[index].virt
						+ (offset & ~PAGE_MASK),
					pad, pad_len);
				offset += pad_len;
			}
		} else {
			size_t pad_len;

			pad_len = bytes_left_in_page;
			if (bytes_left_in_page == len)
				pad_len--;	/* Final '\0' */
			lib_ring_buffer_do_memset(rpages->p[index].virt
					+ (offset & ~PAGE_MASK),
				pad, pad_len);
			offset += pad_len;
		}
		len -= bytes_left_in_page;
		if (!src_terminated)
			src += bytes_left_in_page;
	} while (unlikely(len));

	/* Ending '\0' */
	lib_ring_buffer_do_memset(rpages->p[index].virt + (offset & ~PAGE_MASK),
			'\0', 1);
}
EXPORT_SYMBOL_GPL(_lib_ring_buffer_strcpy);

/**
 * _lib_ring_buffer_pstrcpy - write to a buffer backend P-string
 * @bufb : buffer backend
 * @src : source pointer to copy from
 * @len : length of data to copy
 * @pad : character to use for padding
 *
 * This function copies up to @len bytes of data from a source pointer
 * to a Pascal String into the buffer backend. If a terminating '\0'
 * character is found in @src before @len characters are copied, pad the
 * buffer with @pad characters (e.g.  '\0').
 *
 * The length of the pascal strings in the ring buffer is explicit: it
 * is either the array or sequence length.
 */
void _lib_ring_buffer_pstrcpy(struct lttng_kernel_ring_buffer_backend *bufb,
			size_t offset, const char *src, size_t len, int pad)
{
	struct channel_backend *chanb = &bufb->chan->backend;
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;
	size_t sbidx, index, bytes_left_in_page;
	struct lttng_kernel_ring_buffer_backend_pages *rpages;
	unsigned long sb_bindex, id;
	bool src_terminated = false;

	CHAN_WARN_ON(chanb, !len);
	do {
		sbidx = offset >> chanb->subbuf_size_order;
		index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;

		/*
		 * Underlying layer should never ask for writes across
		 * subbuffers.
		 */
		CHAN_WARN_ON(chanb, offset >= chanb->buf_size);

		bytes_left_in_page = min_t(size_t, len, PAGE_SIZE - (offset & ~PAGE_MASK));
		id = bufb->buf_wsb[sbidx].id;
		sb_bindex = subbuffer_id_get_index(config, id);
		rpages = bufb->array[sb_bindex];
		CHAN_WARN_ON(chanb, config->mode == RING_BUFFER_OVERWRITE
			     && subbuffer_id_is_noref(config, id));

		if (likely(!src_terminated)) {
			size_t count, to_copy;

			to_copy = bytes_left_in_page;
			count = lib_ring_buffer_do_strcpy(config,
					rpages->p[index].virt
						+ (offset & ~PAGE_MASK),
					src, to_copy);
			offset += count;
			/* Padding */
			if (unlikely(count < to_copy)) {
				size_t pad_len = to_copy - count;

				/* Next pages will have padding */
				src_terminated = true;
				lib_ring_buffer_do_memset(rpages->p[index].virt
						+ (offset & ~PAGE_MASK),
					pad, pad_len);
				offset += pad_len;
			}
		} else {
			size_t pad_len;

			pad_len = bytes_left_in_page;
			lib_ring_buffer_do_memset(rpages->p[index].virt
					+ (offset & ~PAGE_MASK),
				pad, pad_len);
			offset += pad_len;
		}
		len -= bytes_left_in_page;
		if (!src_terminated)
			src += bytes_left_in_page;
	} while (unlikely(len));
}
EXPORT_SYMBOL_GPL(_lib_ring_buffer_pstrcpy);

/**
 * lib_ring_buffer_copy_from_user_inatomic - write user data to a ring_buffer buffer.
 * @bufb : buffer backend
 * @offset : offset within the buffer
 * @src : source address
 * @len : length to write
 *
 * This function deals with userspace pointers, it should never be called
 * directly without having the src pointer checked with access_ok()
 * previously.
 */
void _lib_ring_buffer_copy_from_user_inatomic(struct lttng_kernel_ring_buffer_backend *bufb,
				      size_t offset, const void __user *src, size_t len)
{
	struct channel_backend *chanb = &bufb->chan->backend;
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;
	size_t sbidx, index, bytes_left_in_page;
	struct lttng_kernel_ring_buffer_backend_pages *rpages;
	unsigned long sb_bindex, id;
	int ret;

	do {
		sbidx = offset >> chanb->subbuf_size_order;
		index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;

		/*
		 * Underlying layer should never ask for writes across
		 * subbuffers.
		 */
		CHAN_WARN_ON(chanb, offset >= chanb->buf_size);

		bytes_left_in_page = min_t(size_t, len, PAGE_SIZE - (offset & ~PAGE_MASK));
		id = bufb->buf_wsb[sbidx].id;
		sb_bindex = subbuffer_id_get_index(config, id);
		rpages = bufb->array[sb_bindex];
		CHAN_WARN_ON(chanb, config->mode == RING_BUFFER_OVERWRITE
				&& subbuffer_id_is_noref(config, id));
		ret = lib_ring_buffer_do_copy_from_user_inatomic(rpages->p[index].virt
							+ (offset & ~PAGE_MASK),
							src, bytes_left_in_page) != 0;
		if (ret > 0) {
			/* Copy failed. */
			_lib_ring_buffer_memset(bufb, offset, 0, len);
			break; /* stop copy */
		}
		len -= bytes_left_in_page;
		src += bytes_left_in_page;
		offset += bytes_left_in_page;
	} while (unlikely(len));
}
EXPORT_SYMBOL_GPL(_lib_ring_buffer_copy_from_user_inatomic);

/**
 * lib_ring_buffer_strcpy_from_user_inatomic - write userspace string data to a ring_buffer buffer.
 * @bufb : buffer backend
 * @offset : offset within the buffer
 * @src : source address
 * @len : length to write
 * @pad : character to use for padding
 *
 * This function deals with userspace pointers, it should never be called
 * directly without having the src pointer checked with access_ok()
 * previously.
 */
void _lib_ring_buffer_strcpy_from_user_inatomic(struct lttng_kernel_ring_buffer_backend *bufb,
		size_t offset, const char __user *src, size_t len, int pad)
{
	struct channel_backend *chanb = &bufb->chan->backend;
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;
	size_t sbidx, index, bytes_left_in_page;
	struct lttng_kernel_ring_buffer_backend_pages *rpages;
	unsigned long sb_bindex, id;
	bool src_terminated = false;

	do {
		sbidx = offset >> chanb->subbuf_size_order;
		index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;

		/*
		 * Underlying layer should never ask for writes across
		 * subbuffers.
		 */
		CHAN_WARN_ON(chanb, offset >= chanb->buf_size);

		bytes_left_in_page = min_t(size_t, len, PAGE_SIZE - (offset & ~PAGE_MASK));
		id = bufb->buf_wsb[sbidx].id;
		sb_bindex = subbuffer_id_get_index(config, id);
		rpages = bufb->array[sb_bindex];
		CHAN_WARN_ON(chanb, config->mode == RING_BUFFER_OVERWRITE
				&& subbuffer_id_is_noref(config, id));

		if (likely(!src_terminated)) {
			size_t count, to_copy;

			to_copy = bytes_left_in_page;
			if (bytes_left_in_page == len)
				to_copy--;	/* Final '\0' */
			count = lib_ring_buffer_do_strcpy_from_user_inatomic(config,
					rpages->p[index].virt
						+ (offset & ~PAGE_MASK),
					src, to_copy);
			offset += count;
			/* Padding */
			if (unlikely(count < to_copy)) {
				size_t pad_len = to_copy - count;

				/* Next pages will have padding */
				src_terminated = true;
				lib_ring_buffer_do_memset(rpages->p[index].virt
						+ (offset & ~PAGE_MASK),
					pad, pad_len);
				offset += pad_len;
			}
		} else {
			size_t pad_len;

			pad_len = bytes_left_in_page;
			if (bytes_left_in_page == len)
				pad_len--;	/* Final '\0' */
			lib_ring_buffer_do_memset(rpages->p[index].virt
					+ (offset & ~PAGE_MASK),
				pad, pad_len);
			offset += pad_len;
		}
		len -= bytes_left_in_page;
		if (!src_terminated)
			src += bytes_left_in_page;
	} while (unlikely(len));

	/* Ending '\0' */
	lib_ring_buffer_do_memset(rpages->p[index].virt + (offset & ~PAGE_MASK),
			'\0', 1);
}
EXPORT_SYMBOL_GPL(_lib_ring_buffer_strcpy_from_user_inatomic);

/**
 * _lib_ring_buffer_pstrcpy_from_user_inatomic - write userspace string to a buffer backend P-string
 * @bufb : buffer backend
 * @src : source pointer to copy from
 * @len : length of data to copy
 * @pad : character to use for padding
 *
 * This function copies up to @len bytes of data from a source pointer
 * to a Pascal String into the buffer backend. If a terminating '\0'
 * character is found in @src before @len characters are copied, pad the
 * buffer with @pad characters (e.g.  '\0').
 *
 * The length of the pascal strings in the ring buffer is explicit: it
 * is either the array or sequence length.
 *
 * This function deals with userspace pointers, it should never be called
 * directly without having the src pointer checked with access_ok()
 * previously.
 */
void _lib_ring_buffer_pstrcpy_from_user_inatomic(struct lttng_kernel_ring_buffer_backend *bufb,
			size_t offset, const char __user *src, size_t len, int pad)
{
	struct channel_backend *chanb = &bufb->chan->backend;
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;
	size_t sbidx, index, bytes_left_in_page;
	struct lttng_kernel_ring_buffer_backend_pages *rpages;
	unsigned long sb_bindex, id;
	bool src_terminated = false;

	CHAN_WARN_ON(chanb, !len);
	do {
		sbidx = offset >> chanb->subbuf_size_order;
		index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;

		/*
		 * Underlying layer should never ask for writes across
		 * subbuffers.
		 */
		CHAN_WARN_ON(chanb, offset >= chanb->buf_size);

		bytes_left_in_page = min_t(size_t, len, PAGE_SIZE - (offset & ~PAGE_MASK));
		id = bufb->buf_wsb[sbidx].id;
		sb_bindex = subbuffer_id_get_index(config, id);
		rpages = bufb->array[sb_bindex];
		CHAN_WARN_ON(chanb, config->mode == RING_BUFFER_OVERWRITE
			     && subbuffer_id_is_noref(config, id));

		if (likely(!src_terminated)) {
			size_t count, to_copy;

			to_copy = bytes_left_in_page;
			count = lib_ring_buffer_do_strcpy_from_user_inatomic(config,
					rpages->p[index].virt
						+ (offset & ~PAGE_MASK),
					src, to_copy);
			offset += count;
			/* Padding */
			if (unlikely(count < to_copy)) {
				size_t pad_len = to_copy - count;

				/* Next pages will have padding */
				src_terminated = true;
				lib_ring_buffer_do_memset(rpages->p[index].virt
						+ (offset & ~PAGE_MASK),
					pad, pad_len);
				offset += pad_len;
			}
		} else {
			size_t pad_len;

			pad_len = bytes_left_in_page;
			lib_ring_buffer_do_memset(rpages->p[index].virt
					+ (offset & ~PAGE_MASK),
				pad, pad_len);
			offset += pad_len;
		}
		len -= bytes_left_in_page;
		if (!src_terminated)
			src += bytes_left_in_page;
	} while (unlikely(len));
}
EXPORT_SYMBOL_GPL(_lib_ring_buffer_pstrcpy_from_user_inatomic);

/**
 * lib_ring_buffer_read - read data from ring_buffer_buffer.
 * @bufb : buffer backend
 * @offset : offset within the buffer
 * @dest : destination address
 * @len : length to copy to destination
 *
 * Should be protected by get_subbuf/put_subbuf.
 * Returns the length copied.
 */
size_t lib_ring_buffer_read(struct lttng_kernel_ring_buffer_backend *bufb, size_t offset,
			    void *dest, size_t len)
{
	struct channel_backend *chanb = &bufb->chan->backend;
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;
	size_t index, bytes_left_in_page, orig_len;
	struct lttng_kernel_ring_buffer_backend_pages *rpages;
	unsigned long sb_bindex, id;

	orig_len = len;
	offset &= chanb->buf_size - 1;
	index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
	if (unlikely(!len))
		return 0;
	for (;;) {
		bytes_left_in_page = min_t(size_t, len, PAGE_SIZE - (offset & ~PAGE_MASK));
		id = bufb->buf_rsb.id;
		sb_bindex = subbuffer_id_get_index(config, id);
		rpages = bufb->array[sb_bindex];
		CHAN_WARN_ON(chanb, config->mode == RING_BUFFER_OVERWRITE
			     && subbuffer_id_is_noref(config, id));
		memcpy(dest, rpages->p[index].virt + (offset & ~PAGE_MASK),
		       bytes_left_in_page);
		len -= bytes_left_in_page;
		if (likely(!len))
			break;
		dest += bytes_left_in_page;
		offset += bytes_left_in_page;
		index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
		/*
		 * Underlying layer should never ask for reads across
		 * subbuffers.
		 */
		CHAN_WARN_ON(chanb, offset >= chanb->buf_size);
	}
	return orig_len;
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_read);

/**
 * __lib_ring_buffer_copy_to_user - read data from ring_buffer to userspace
 * @bufb : buffer backend
 * @offset : offset within the buffer
 * @dest : destination userspace address
 * @len : length to copy to destination
 *
 * Should be protected by get_subbuf/put_subbuf.
 * access_ok() must have been performed on dest addresses prior to call this
 * function.
 * Returns -EFAULT on error, 0 if ok.
 */
int __lib_ring_buffer_copy_to_user(struct lttng_kernel_ring_buffer_backend *bufb,
				   size_t offset, void __user *dest, size_t len)
{
	struct channel_backend *chanb = &bufb->chan->backend;
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;
	size_t index;
	ssize_t bytes_left_in_page;
	struct lttng_kernel_ring_buffer_backend_pages *rpages;
	unsigned long sb_bindex, id;

	offset &= chanb->buf_size - 1;
	index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
	if (unlikely(!len))
		return 0;
	for (;;) {
		bytes_left_in_page = min_t(size_t, len, PAGE_SIZE - (offset & ~PAGE_MASK));
		id = bufb->buf_rsb.id;
		sb_bindex = subbuffer_id_get_index(config, id);
		rpages = bufb->array[sb_bindex];
		CHAN_WARN_ON(chanb, config->mode == RING_BUFFER_OVERWRITE
			     && subbuffer_id_is_noref(config, id));
		if (__copy_to_user(dest,
			       rpages->p[index].virt + (offset & ~PAGE_MASK),
			       bytes_left_in_page))
			return -EFAULT;
		len -= bytes_left_in_page;
		if (likely(!len))
			break;
		dest += bytes_left_in_page;
		offset += bytes_left_in_page;
		index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
		/*
		 * Underlying layer should never ask for reads across
		 * subbuffers.
		 */
		CHAN_WARN_ON(chanb, offset >= chanb->buf_size);
	}
	return 0;
}
EXPORT_SYMBOL_GPL(__lib_ring_buffer_copy_to_user);

/**
 * lib_ring_buffer_read_cstr - read a C-style string from ring_buffer.
 * @bufb : buffer backend
 * @offset : offset within the buffer
 * @dest : destination address
 * @len : destination's length
 *
 * Return string's length, or -EINVAL on error.
 * Should be protected by get_subbuf/put_subbuf.
 * Destination length should be at least 1 to hold '\0'.
 */
int lib_ring_buffer_read_cstr(struct lttng_kernel_ring_buffer_backend *bufb, size_t offset,
			      void *dest, size_t len)
{
	struct channel_backend *chanb = &bufb->chan->backend;
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;
	size_t index;
	ssize_t bytes_left_in_page, pagelen, strpagelen, orig_offset;
	char *str;
	struct lttng_kernel_ring_buffer_backend_pages *rpages;
	unsigned long sb_bindex, id;

	offset &= chanb->buf_size - 1;
	index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
	orig_offset = offset;
	if (unlikely(!len))
		return -EINVAL;
	for (;;) {
		id = bufb->buf_rsb.id;
		sb_bindex = subbuffer_id_get_index(config, id);
		rpages = bufb->array[sb_bindex];
		CHAN_WARN_ON(chanb, config->mode == RING_BUFFER_OVERWRITE
			     && subbuffer_id_is_noref(config, id));
		str = (char *)rpages->p[index].virt + (offset & ~PAGE_MASK);
		pagelen = PAGE_SIZE - (offset & ~PAGE_MASK);
		strpagelen = strnlen(str, pagelen);
		if (len) {
			bytes_left_in_page = min_t(size_t, len, strpagelen);
			if (dest) {
				memcpy(dest, str, bytes_left_in_page);
				dest += bytes_left_in_page;
			}
			len -= bytes_left_in_page;
		}
		offset += strpagelen;
		index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
		if (strpagelen < pagelen)
			break;
		/*
		 * Underlying layer should never ask for reads across
		 * subbuffers.
		 */
		CHAN_WARN_ON(chanb, offset >= chanb->buf_size);
	}
	if (dest && len)
		((char *)dest)[0] = 0;
	return offset - orig_offset;
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_read_cstr);

/**
 * lib_ring_buffer_read_get_pfn - Get a page frame number to read from
 * @bufb : buffer backend
 * @offset : offset within the buffer
 * @virt : pointer to page address (output)
 *
 * Should be protected by get_subbuf/put_subbuf.
 * Returns the pointer to the page frame number unsigned long.
 */
unsigned long *lib_ring_buffer_read_get_pfn(struct lttng_kernel_ring_buffer_backend *bufb,
					    size_t offset, void ***virt)
{
	size_t index;
	struct lttng_kernel_ring_buffer_backend_pages *rpages;
	struct channel_backend *chanb = &bufb->chan->backend;
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;
	unsigned long sb_bindex, id;

	offset &= chanb->buf_size - 1;
	index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
	id = bufb->buf_rsb.id;
	sb_bindex = subbuffer_id_get_index(config, id);
	rpages = bufb->array[sb_bindex];
	CHAN_WARN_ON(chanb, config->mode == RING_BUFFER_OVERWRITE
		     && subbuffer_id_is_noref(config, id));
	*virt = &rpages->p[index].virt;
	return &rpages->p[index].pfn;
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_read_get_pfn);

/**
 * lib_ring_buffer_read_offset_address - get address of a buffer location
 * @bufb : buffer backend
 * @offset : offset within the buffer.
 *
 * Return the address where a given offset is located (for read).
 * Should be used to get the current subbuffer header pointer. Given we know
 * it's never on a page boundary, it's safe to read/write directly
 * from/to this address, as long as the read/write is never bigger than a
 * page size.
 */
void *lib_ring_buffer_read_offset_address(struct lttng_kernel_ring_buffer_backend *bufb,
					  size_t offset)
{
	size_t index;
	struct lttng_kernel_ring_buffer_backend_pages *rpages;
	struct channel_backend *chanb = &bufb->chan->backend;
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;
	unsigned long sb_bindex, id;

	offset &= chanb->buf_size - 1;
	index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
	id = bufb->buf_rsb.id;
	sb_bindex = subbuffer_id_get_index(config, id);
	rpages = bufb->array[sb_bindex];
	CHAN_WARN_ON(chanb, config->mode == RING_BUFFER_OVERWRITE
		     && subbuffer_id_is_noref(config, id));
	return rpages->p[index].virt + (offset & ~PAGE_MASK);
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_read_offset_address);

/**
 * lib_ring_buffer_offset_address - get address of a location within the buffer
 * @bufb : buffer backend
 * @offset : offset within the buffer.
 *
 * Return the address where a given offset is located.
 * Should be used to get the current subbuffer header pointer. Given we know
 * it's always at the beginning of a page, it's safe to write directly to this
 * address, as long as the write is never bigger than a page size.
 */
void *lib_ring_buffer_offset_address(struct lttng_kernel_ring_buffer_backend *bufb,
				     size_t offset)
{
	size_t sbidx, index;
	struct lttng_kernel_ring_buffer_backend_pages *rpages;
	struct channel_backend *chanb = &bufb->chan->backend;
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;
	unsigned long sb_bindex, id;

	offset &= chanb->buf_size - 1;
	sbidx = offset >> chanb->subbuf_size_order;
	index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
	id = bufb->buf_wsb[sbidx].id;
	sb_bindex = subbuffer_id_get_index(config, id);
	rpages = bufb->array[sb_bindex];
	CHAN_WARN_ON(chanb, config->mode == RING_BUFFER_OVERWRITE
		     && subbuffer_id_is_noref(config, id));
	return rpages->p[index].virt + (offset & ~PAGE_MASK);
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_offset_address);
