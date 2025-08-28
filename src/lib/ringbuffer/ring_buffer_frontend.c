/* SPDX-License-Identifier: (GPL-2.0-only OR LGPL-2.1-only)
 *
 * ring_buffer_frontend.c
 *
 * Copyright (C) 2005-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * Ring buffer wait-free buffer synchronization. Producer-consumer and flight
 * recorder (overwrite) modes. See thesis:
 *
 * Desnoyers, Mathieu (2009), "Low-Impact Operating System Tracing", Ph.D.
 * dissertation, Ecole Polytechnique de Montreal.
 * http://www.lttng.org/pub/thesis/desnoyers-dissertation-2009-12.pdf
 *
 * - Algorithm presentation in Chapter 5:
 *     "Lockless Multi-Core High-Throughput Buffering".
 * - Algorithm formal verification in Section 8.6:
 *     "Formal verification of LTTng"
 *
 * Author:
 *	Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * Inspired from LTT and RelayFS:
 *  Karim Yaghmour <karim@opersys.com>
 *  Tom Zanussi <zanussi@us.ibm.com>
 *  Bob Wisniewski <bob@watson.ibm.com>
 * And from K42 :
 *  Bob Wisniewski <bob@watson.ibm.com>
 *
 * Buffer reader semantic :
 *
 * - get_subbuf_size
 * while buffer is not finalized and empty
 *   - get_subbuf
 *     - if return value != 0, continue
 *   - splice one subbuffer worth of data to a pipe
 *   - splice the data from pipe to disk/network
 *   - put_subbuf
 */

#include <linux/delay.h>
#include <linux/module.h>
#include <linux/percpu.h>
#include <asm/cacheflush.h>

#include <ringbuffer/config.h>
#include <ringbuffer/backend.h>
#include <ringbuffer/frontend.h>
#include <ringbuffer/iterator.h>
#include <ringbuffer/nohz.h>
#include <wrapper/atomic.h>
#include <wrapper/cpu.h>
#include <wrapper/kref.h>
#include <wrapper/percpu-defs.h>
#include <wrapper/timer.h>
#include <wrapper/vmalloc.h>

/*
 * Internal structure representing offsets to use at a sub-buffer switch.
 */
struct switch_offsets {
	unsigned long begin, end, old;
	size_t pre_header_padding, size;
	unsigned int switch_new_start:1, switch_new_end:1, switch_old_start:1,
		     switch_old_end:1;
};

#ifdef CONFIG_NO_HZ
enum tick_nohz_val {
	TICK_NOHZ_STOP,
	TICK_NOHZ_FLUSH,
	TICK_NOHZ_RESTART,
};

static ATOMIC_NOTIFIER_HEAD(tick_nohz_notifier);
#endif /* CONFIG_NO_HZ */

static DEFINE_PER_CPU(spinlock_t, ring_buffer_nohz_lock);

DEFINE_PER_CPU(unsigned int, lib_ring_buffer_nesting);
EXPORT_PER_CPU_SYMBOL(lib_ring_buffer_nesting);

static
void lib_ring_buffer_print_errors(struct lttng_kernel_ring_buffer_channel *chan,
				  struct lttng_kernel_ring_buffer *buf, int cpu);
static
void _lib_ring_buffer_switch_remote(struct lttng_kernel_ring_buffer *buf,
		enum switch_mode mode);

static
int lib_ring_buffer_poll_deliver(const struct lttng_kernel_ring_buffer_config *config,
				 struct lttng_kernel_ring_buffer *buf,
			         struct lttng_kernel_ring_buffer_channel *chan)
{
	unsigned long consumed_old, consumed_idx, commit_count, write_offset;

	consumed_old = atomic_long_read(&buf->consumed);
	consumed_idx = subbuf_index(consumed_old, chan);
	commit_count = v_read(config, &buf->commit_cold[consumed_idx].cc_sb);
	/*
	 * No memory barrier here, since we are only interested
	 * in a statistically correct polling result. The next poll will
	 * get the data is we are racing. The mb() that ensures correct
	 * memory order is in get_subbuf.
	 */
	write_offset = v_read(config, &buf->offset);

	/*
	 * Check that the subbuffer we are trying to consume has been
	 * already fully committed.
	 */

	if (((commit_count - chan->backend.subbuf_size)
	     & chan->commit_count_mask)
	    - (buf_trunc(consumed_old, chan)
	       >> chan->backend.num_subbuf_order)
	    != 0)
		return 0;

	/*
	 * Check that we are not about to read the same subbuffer in
	 * which the writer head is.
	 */
	if (subbuf_trunc(write_offset, chan) - subbuf_trunc(consumed_old, chan)
	    == 0)
		return 0;

	return 1;
}

/*
 * Must be called under cpu hotplug protection.
 */
void lib_ring_buffer_free(struct lttng_kernel_ring_buffer *buf)
{
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;

	irq_work_sync(&buf->wakeup_pending);

	lib_ring_buffer_print_errors(chan, buf, buf->backend.cpu);
	lttng_kvfree(buf->commit_hot);
	lttng_kvfree(buf->commit_cold);
	lttng_kvfree(buf->ts_end);

	lib_ring_buffer_backend_free(&buf->backend);
}

/**
 * lib_ring_buffer_reset - Reset ring buffer to initial values.
 * @buf: Ring buffer.
 *
 * Effectively empty the ring buffer. Should be called when the buffer is not
 * used for writing. The ring buffer can be opened for reading, but the reader
 * should not be using the iterator concurrently with reset. The previous
 * current iterator record is reset.
 */
void lib_ring_buffer_reset(struct lttng_kernel_ring_buffer *buf)
{
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	unsigned int i;

	/*
	 * Reset iterator first. It will put the subbuffer if it currently holds
	 * it.
	 */
	lib_ring_buffer_iterator_reset(buf);
	v_set(config, &buf->offset, 0);
	for (i = 0; i < chan->backend.num_subbuf; i++) {
		v_set(config, &buf->commit_hot[i].cc, 0);
		v_set(config, &buf->commit_hot[i].seq, 0);
		v_set(config, &buf->commit_cold[i].cc_sb, 0);
		buf->ts_end[i] = 0;
	}
	atomic_long_set(&buf->consumed, 0);
	atomic_set(&buf->record_disabled, 0);
	v_set(config, &buf->last_tsc, 0);
	lib_ring_buffer_backend_reset(&buf->backend);
	/* Don't reset number of active readers */
	v_set(config, &buf->records_lost_full, 0);
	v_set(config, &buf->records_lost_wrap, 0);
	v_set(config, &buf->records_lost_big, 0);
	v_set(config, &buf->records_count, 0);
	v_set(config, &buf->records_overrun, 0);
	buf->finalized = 0;
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_reset);

/**
 * channel_reset - Reset channel to initial values.
 * @chan: Channel.
 *
 * Effectively empty the channel. Should be called when the channel is not used
 * for writing. The channel can be opened for reading, but the reader should not
 * be using the iterator concurrently with reset. The previous current iterator
 * record is reset.
 */
void channel_reset(struct lttng_kernel_ring_buffer_channel *chan)
{
	/*
	 * Reset iterators first. Will put the subbuffer if held for reading.
	 */
	channel_iterator_reset(chan);
	atomic_set(&chan->record_disabled, 0);
	/* Don't reset commit_count_mask, still valid */
	channel_backend_reset(&chan->backend);
	/* Don't reset switch/read timer interval */
	/* Don't reset notifiers and notifier enable bits */
	/* Don't reset reader reference count */
}
EXPORT_SYMBOL_GPL(channel_reset);

static void lib_ring_buffer_pending_wakeup_buf(struct irq_work *entry)
{
	struct lttng_kernel_ring_buffer *buf = container_of(entry, struct lttng_kernel_ring_buffer,
						   wakeup_pending);
	wake_up_interruptible(&buf->read_wait);
}

static void lib_ring_buffer_pending_wakeup_chan(struct irq_work *entry)
{
	struct lttng_kernel_ring_buffer_channel *chan = container_of(entry, struct lttng_kernel_ring_buffer_channel, wakeup_pending);
	wake_up_interruptible(&chan->read_wait);
}

/*
 * Must be called under cpu hotplug protection.
 */
int lib_ring_buffer_create(struct lttng_kernel_ring_buffer *buf,
			   struct channel_backend *chanb, int cpu)
{
	const struct lttng_kernel_ring_buffer_config *config = &chanb->config;
	struct lttng_kernel_ring_buffer_channel *chan = container_of(chanb, struct lttng_kernel_ring_buffer_channel, backend);
	void *priv = chanb->priv;
	size_t subbuf_header_size;
	u64 tsc;
	int ret;

	/* Test for cpu hotplug */
	if (buf->backend.allocated)
		return 0;

	/*
	 * Paranoia: per cpu dynamic allocation is not officially documented as
	 * zeroing the memory, so let's do it here too, just in case.
	 */
	memset(buf, 0, sizeof(*buf));

	ret = lib_ring_buffer_backend_create(&buf->backend, &chan->backend, cpu);
	if (ret)
		return ret;

	buf->commit_hot =
		lttng_kvzalloc_node(ALIGN(sizeof(*buf->commit_hot)
				   * chan->backend.num_subbuf,
				   1 << INTERNODE_CACHE_SHIFT),
			GFP_KERNEL | __GFP_NOWARN,
			cpu_to_node(max(cpu, 0)));
	if (!buf->commit_hot) {
		ret = -ENOMEM;
		goto free_chanbuf;
	}

	buf->commit_cold =
		lttng_kvzalloc_node(ALIGN(sizeof(*buf->commit_cold)
				   * chan->backend.num_subbuf,
				   1 << INTERNODE_CACHE_SHIFT),
			GFP_KERNEL | __GFP_NOWARN,
			cpu_to_node(max(cpu, 0)));
	if (!buf->commit_cold) {
		ret = -ENOMEM;
		goto free_commit;
	}

	buf->ts_end =
		lttng_kvzalloc_node(ALIGN(sizeof(*buf->ts_end)
				   * chan->backend.num_subbuf,
				   1 << INTERNODE_CACHE_SHIFT),
			GFP_KERNEL | __GFP_NOWARN,
			cpu_to_node(max(cpu, 0)));
	if (!buf->ts_end) {
		ret = -ENOMEM;
		goto free_commit_cold;
	}

	init_waitqueue_head(&buf->read_wait);
	init_waitqueue_head(&buf->write_wait);
	init_irq_work(&buf->wakeup_pending, lib_ring_buffer_pending_wakeup_buf);
	raw_spin_lock_init(&buf->raw_tick_nohz_spinlock);

	/*
	 * Write the subbuffer header for first subbuffer so we know the total
	 * duration of data gathering.
	 */
	subbuf_header_size = config->cb.subbuffer_header_size();
	v_set(config, &buf->offset, subbuf_header_size);
	subbuffer_id_clear_noref(config, &buf->backend.buf_wsb[0].id);
	tsc = config->cb.ring_buffer_clock_read(buf->backend.chan);
	config->cb.buffer_begin(buf, tsc, 0);
	v_add(config, subbuf_header_size, &buf->commit_hot[0].cc);

	if (config->cb.buffer_create) {
		ret = config->cb.buffer_create(buf, priv, cpu, chanb->name);
		if (ret)
			goto free_init;
	}

	/*
	 * Ensure the buffer is ready before setting it to allocated and setting
	 * the cpumask.
	 * Used for cpu hotplug vs cpumask iteration.
	 */
	smp_wmb();
	buf->backend.allocated = 1;

	if (config->alloc == RING_BUFFER_ALLOC_PER_CPU) {
		CHAN_WARN_ON(chan, cpumask_test_cpu(cpu,
			     chan->backend.cpumask));
		cpumask_set_cpu(cpu, chan->backend.cpumask);
	}

	return 0;

	/* Error handling */
free_init:
	lttng_kvfree(buf->ts_end);
free_commit_cold:
	lttng_kvfree(buf->commit_cold);
free_commit:
	lttng_kvfree(buf->commit_hot);
free_chanbuf:
	lib_ring_buffer_backend_free(&buf->backend);
	return ret;
}

static void switch_buffer_timer(LTTNG_TIMER_FUNC_ARG_TYPE t)
{
	struct lttng_kernel_ring_buffer *buf = lttng_timer_container_of(buf, t, switch_timer);
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;

	/*
	 * Only flush buffers periodically if readers are active.
	 */
	if (atomic_long_read(&buf->active_readers))
		lib_ring_buffer_switch_slow(buf, SWITCH_ACTIVE);

	if (config->alloc == RING_BUFFER_ALLOC_PER_CPU)
		lttng_mod_timer_pinned(&buf->switch_timer,
				 jiffies + chan->switch_timer_interval);
	else
		mod_timer(&buf->switch_timer,
			  jiffies + chan->switch_timer_interval);
}

/*
 * Called with ring_buffer_nohz_lock held for per-cpu buffers.
 */
static void lib_ring_buffer_start_switch_timer(struct lttng_kernel_ring_buffer *buf)
{
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	unsigned int flags = 0;

	if (!chan->switch_timer_interval || buf->switch_timer_enabled)
		return;

	if (config->alloc == RING_BUFFER_ALLOC_PER_CPU)
		flags = LTTNG_TIMER_PINNED;

	lttng_timer_setup(&buf->switch_timer, switch_buffer_timer, flags, buf);
	buf->switch_timer.expires = jiffies + chan->switch_timer_interval;

	if (config->alloc == RING_BUFFER_ALLOC_PER_CPU)
		add_timer_on(&buf->switch_timer, buf->backend.cpu);
	else
		add_timer(&buf->switch_timer);

	buf->switch_timer_enabled = 1;
}

/*
 * Called with ring_buffer_nohz_lock held for per-cpu buffers.
 */
static void lib_ring_buffer_stop_switch_timer(struct lttng_kernel_ring_buffer *buf)
{
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;

	if (!chan->switch_timer_interval || !buf->switch_timer_enabled)
		return;

	lttng_timer_delete_sync(&buf->switch_timer);
	buf->switch_timer_enabled = 0;
}

/*
 * Polling timer to check the channels for data.
 */
static void read_buffer_timer(LTTNG_TIMER_FUNC_ARG_TYPE t)
{
	struct lttng_kernel_ring_buffer *buf = lttng_timer_container_of(buf, t, read_timer);
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;

	CHAN_WARN_ON(chan, !buf->backend.allocated);

	if (atomic_long_read(&buf->active_readers)
	    && lib_ring_buffer_poll_deliver(config, buf, chan)) {
		wake_up_interruptible(&buf->read_wait);
		wake_up_interruptible(&chan->read_wait);
	}

	if (config->alloc == RING_BUFFER_ALLOC_PER_CPU)
		lttng_mod_timer_pinned(&buf->read_timer,
				 jiffies + chan->read_timer_interval);
	else
		mod_timer(&buf->read_timer,
			  jiffies + chan->read_timer_interval);
}

/*
 * Called with ring_buffer_nohz_lock held for per-cpu buffers.
 */
static void lib_ring_buffer_start_read_timer(struct lttng_kernel_ring_buffer *buf)
{
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	unsigned int flags = 0;

	if (config->wakeup != RING_BUFFER_WAKEUP_BY_TIMER
	    || !chan->read_timer_interval
	    || buf->read_timer_enabled)
		return;

	if (config->alloc == RING_BUFFER_ALLOC_PER_CPU)
		flags = LTTNG_TIMER_PINNED;

	lttng_timer_setup(&buf->read_timer, read_buffer_timer, flags, buf);
	buf->read_timer.expires = jiffies + chan->read_timer_interval;

	if (config->alloc == RING_BUFFER_ALLOC_PER_CPU)
		add_timer_on(&buf->read_timer, buf->backend.cpu);
	else
		add_timer(&buf->read_timer);

	buf->read_timer_enabled = 1;
}

/*
 * Called with ring_buffer_nohz_lock held for per-cpu buffers.
 */
static void lib_ring_buffer_stop_read_timer(struct lttng_kernel_ring_buffer *buf)
{
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;

	if (config->wakeup != RING_BUFFER_WAKEUP_BY_TIMER
	    || !chan->read_timer_interval
	    || !buf->read_timer_enabled)
		return;

	lttng_timer_delete_sync(&buf->read_timer);
	/*
	 * do one more check to catch data that has been written in the last
	 * timer period.
	 */
	if (lib_ring_buffer_poll_deliver(config, buf, chan)) {
		wake_up_interruptible(&buf->read_wait);
		wake_up_interruptible(&chan->read_wait);
	}
	buf->read_timer_enabled = 0;
}

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0))

enum cpuhp_state lttng_rb_hp_prepare;
enum cpuhp_state lttng_rb_hp_online;

void lttng_rb_set_hp_prepare(enum cpuhp_state val)
{
	lttng_rb_hp_prepare = val;
}
EXPORT_SYMBOL_GPL(lttng_rb_set_hp_prepare);

void lttng_rb_set_hp_online(enum cpuhp_state val)
{
	lttng_rb_hp_online = val;
}
EXPORT_SYMBOL_GPL(lttng_rb_set_hp_online);

int lttng_cpuhp_rb_frontend_dead(unsigned int cpu,
		struct lttng_cpuhp_node *node)
{
	struct lttng_kernel_ring_buffer_channel *chan = container_of(node, struct lttng_kernel_ring_buffer_channel,
					    cpuhp_prepare);
	struct lttng_kernel_ring_buffer *buf = per_cpu_ptr(chan->backend.percpu_buf, cpu);
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;

	CHAN_WARN_ON(chan, config->alloc == RING_BUFFER_ALLOC_GLOBAL);

	/*
	 * Performing a buffer switch on a remote CPU. Performed by
	 * the CPU responsible for doing the hotunplug after the target
	 * CPU stopped running completely. Ensures that all data
	 * from that remote CPU is flushed.
	 */
	lib_ring_buffer_switch_slow(buf, SWITCH_ACTIVE);
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_cpuhp_rb_frontend_dead);

int lttng_cpuhp_rb_frontend_online(unsigned int cpu,
		struct lttng_cpuhp_node *node)
{
	struct lttng_kernel_ring_buffer_channel *chan = container_of(node, struct lttng_kernel_ring_buffer_channel,
					    cpuhp_online);
	struct lttng_kernel_ring_buffer *buf = per_cpu_ptr(chan->backend.percpu_buf, cpu);
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;

	CHAN_WARN_ON(chan, config->alloc == RING_BUFFER_ALLOC_GLOBAL);

	wake_up_interruptible(&chan->hp_wait);
	lib_ring_buffer_start_switch_timer(buf);
	lib_ring_buffer_start_read_timer(buf);
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_cpuhp_rb_frontend_online);

int lttng_cpuhp_rb_frontend_offline(unsigned int cpu,
		struct lttng_cpuhp_node *node)
{
	struct lttng_kernel_ring_buffer_channel *chan = container_of(node, struct lttng_kernel_ring_buffer_channel,
					    cpuhp_online);
	struct lttng_kernel_ring_buffer *buf = per_cpu_ptr(chan->backend.percpu_buf, cpu);
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;

	CHAN_WARN_ON(chan, config->alloc == RING_BUFFER_ALLOC_GLOBAL);

	lib_ring_buffer_stop_switch_timer(buf);
	lib_ring_buffer_stop_read_timer(buf);
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_cpuhp_rb_frontend_offline);

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
	struct lttng_kernel_ring_buffer_channel *chan = container_of(nb, struct lttng_kernel_ring_buffer_channel,
					    cpu_hp_notifier);
	struct lttng_kernel_ring_buffer *buf = per_cpu_ptr(chan->backend.percpu_buf, cpu);
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;

	if (!chan->cpu_hp_enable)
		return NOTIFY_DONE;

	CHAN_WARN_ON(chan, config->alloc == RING_BUFFER_ALLOC_GLOBAL);

	switch (action) {
	case CPU_DOWN_FAILED:
	case CPU_DOWN_FAILED_FROZEN:
	case CPU_ONLINE:
	case CPU_ONLINE_FROZEN:
		wake_up_interruptible(&chan->hp_wait);
		lib_ring_buffer_start_switch_timer(buf);
		lib_ring_buffer_start_read_timer(buf);
		return NOTIFY_OK;

	case CPU_DOWN_PREPARE:
	case CPU_DOWN_PREPARE_FROZEN:
		lib_ring_buffer_stop_switch_timer(buf);
		lib_ring_buffer_stop_read_timer(buf);
		return NOTIFY_OK;

	case CPU_DEAD:
	case CPU_DEAD_FROZEN:
		/*
		 * Performing a buffer switch on a remote CPU. Performed by
		 * the CPU responsible for doing the hotunplug after the target
		 * CPU stopped running completely. Ensures that all data
		 * from that remote CPU is flushed.
		 */
		lib_ring_buffer_switch_slow(buf, SWITCH_ACTIVE);
		return NOTIFY_OK;

	default:
		return NOTIFY_DONE;
	}
}

#endif

#endif /* #else #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0)) */

#if defined(CONFIG_NO_HZ) && defined(CONFIG_LIB_RING_BUFFER)
/*
 * For per-cpu buffers, call the reader wakeups before switching the buffer, so
 * that wake-up-tracing generated events are flushed before going idle (in
 * tick_nohz). We test if the spinlock is locked to deal with the race where
 * readers try to sample the ring buffer before we perform the switch. We let
 * the readers retry in that case. If there is data in the buffer, the wake up
 * is going to forbid the CPU running the reader thread from going idle.
 */
static int notrace ring_buffer_tick_nohz_callback(struct notifier_block *nb,
						  unsigned long val,
						  void *data)
{
	struct lttng_kernel_ring_buffer_channel *chan = container_of(nb, struct lttng_kernel_ring_buffer_channel,
					    tick_nohz_notifier);
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	struct lttng_kernel_ring_buffer *buf;
	int cpu = smp_processor_id();

	if (config->alloc != RING_BUFFER_ALLOC_PER_CPU) {
		/*
		 * We don't support keeping the system idle with global buffers
		 * and streaming active. In order to do so, we would need to
		 * sample a non-nohz-cpumask racelessly with the nohz updates
		 * without adding synchronization overhead to nohz. Leave this
		 * use-case out for now.
		 */
		return 0;
	}

	buf = channel_get_ring_buffer(config, chan, cpu);
	switch (val) {
	case TICK_NOHZ_FLUSH:
		raw_spin_lock(&buf->raw_tick_nohz_spinlock);
		if (config->wakeup == RING_BUFFER_WAKEUP_BY_TIMER
		    && chan->read_timer_interval
		    && atomic_long_read(&buf->active_readers)
		    && (lib_ring_buffer_poll_deliver(config, buf, chan)
			|| lib_ring_buffer_pending_data(config, buf, chan))) {
			wake_up_interruptible(&buf->read_wait);
			wake_up_interruptible(&chan->read_wait);
		}
		if (chan->switch_timer_interval)
			lib_ring_buffer_switch_slow(buf, SWITCH_ACTIVE);
		raw_spin_unlock(&buf->raw_tick_nohz_spinlock);
		break;
	case TICK_NOHZ_STOP:
		spin_lock(lttng_this_cpu_ptr(&ring_buffer_nohz_lock));
		lib_ring_buffer_stop_switch_timer(buf);
		lib_ring_buffer_stop_read_timer(buf);
		spin_unlock(lttng_this_cpu_ptr(&ring_buffer_nohz_lock));
		break;
	case TICK_NOHZ_RESTART:
		spin_lock(lttng_this_cpu_ptr(&ring_buffer_nohz_lock));
		lib_ring_buffer_start_read_timer(buf);
		lib_ring_buffer_start_switch_timer(buf);
		spin_unlock(lttng_this_cpu_ptr(&ring_buffer_nohz_lock));
		break;
	}

	return 0;
}

void notrace lib_ring_buffer_tick_nohz_flush(void)
{
	atomic_notifier_call_chain(&tick_nohz_notifier, TICK_NOHZ_FLUSH,
				   NULL);
}

void notrace lib_ring_buffer_tick_nohz_stop(void)
{
	atomic_notifier_call_chain(&tick_nohz_notifier, TICK_NOHZ_STOP,
				   NULL);
}

void notrace lib_ring_buffer_tick_nohz_restart(void)
{
	atomic_notifier_call_chain(&tick_nohz_notifier, TICK_NOHZ_RESTART,
				   NULL);
}
#endif /* defined(CONFIG_NO_HZ) && defined(CONFIG_LIB_RING_BUFFER) */

/*
 * Holds CPU hotplug.
 */
static void channel_unregister_notifiers(struct lttng_kernel_ring_buffer_channel *chan)
{
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;

	channel_iterator_unregister_notifiers(chan);
	if (config->alloc == RING_BUFFER_ALLOC_PER_CPU) {
#ifdef CONFIG_NO_HZ
		/*
		 * Remove the nohz notifier first, so we are certain we stop
		 * the timers.
		 */
		atomic_notifier_chain_unregister(&tick_nohz_notifier,
						 &chan->tick_nohz_notifier);
		/*
		 * ring_buffer_nohz_lock will not be needed below, because
		 * we just removed the notifiers, which were the only source of
		 * concurrency.
		 */
#endif /* CONFIG_NO_HZ */
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0))
		{
			int ret;

			ret = cpuhp_state_remove_instance(lttng_rb_hp_online,
				&chan->cpuhp_online.node);
			WARN_ON(ret);
			ret = cpuhp_state_remove_instance_nocalls(lttng_rb_hp_prepare,
				&chan->cpuhp_prepare.node);
			WARN_ON(ret);
		}
#else /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0)) */
		{
			int cpu;

#ifdef CONFIG_HOTPLUG_CPU
			lttng_cpus_read_lock();
			chan->cpu_hp_enable = 0;
			for_each_online_cpu(cpu) {
				struct lttng_kernel_ring_buffer *buf = per_cpu_ptr(chan->backend.percpu_buf,
								      cpu);
				lib_ring_buffer_stop_switch_timer(buf);
				lib_ring_buffer_stop_read_timer(buf);
			}
			lttng_cpus_read_unlock();
			unregister_cpu_notifier(&chan->cpu_hp_notifier);
#else
			for_each_possible_cpu(cpu) {
				struct lttng_kernel_ring_buffer *buf = per_cpu_ptr(chan->backend.percpu_buf,
								      cpu);
				lib_ring_buffer_stop_switch_timer(buf);
				lib_ring_buffer_stop_read_timer(buf);
			}
#endif
		}
#endif /* #else #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0)) */
	} else {
		struct lttng_kernel_ring_buffer *buf = chan->backend.global_buf;

		lib_ring_buffer_stop_switch_timer(buf);
		lib_ring_buffer_stop_read_timer(buf);
	}
	channel_backend_unregister_notifiers(&chan->backend);
}

static void lib_ring_buffer_set_quiescent(struct lttng_kernel_ring_buffer *buf)
{
	if (!buf->quiescent) {
		buf->quiescent = true;
		_lib_ring_buffer_switch_remote(buf, SWITCH_FLUSH);
	}
}

static void lib_ring_buffer_clear_quiescent(struct lttng_kernel_ring_buffer *buf)
{
	buf->quiescent = false;
}

void lib_ring_buffer_set_quiescent_channel(struct lttng_kernel_ring_buffer_channel *chan)
{
	int cpu;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;

	if (config->alloc == RING_BUFFER_ALLOC_PER_CPU) {
		lttng_cpus_read_lock();
		for_each_channel_cpu(cpu, chan) {
			struct lttng_kernel_ring_buffer *buf = per_cpu_ptr(chan->backend.percpu_buf,
							      cpu);

			lib_ring_buffer_set_quiescent(buf);
		}
		lttng_cpus_read_unlock();
	} else {
		struct lttng_kernel_ring_buffer *buf = chan->backend.global_buf;

		lib_ring_buffer_set_quiescent(buf);
	}
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_set_quiescent_channel);

void lib_ring_buffer_clear_quiescent_channel(struct lttng_kernel_ring_buffer_channel *chan)
{
	int cpu;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;

	if (config->alloc == RING_BUFFER_ALLOC_PER_CPU) {
		lttng_cpus_read_lock();
		for_each_channel_cpu(cpu, chan) {
			struct lttng_kernel_ring_buffer *buf = per_cpu_ptr(chan->backend.percpu_buf,
							      cpu);

			lib_ring_buffer_clear_quiescent(buf);
		}
		lttng_cpus_read_unlock();
	} else {
		struct lttng_kernel_ring_buffer *buf = chan->backend.global_buf;

		lib_ring_buffer_clear_quiescent(buf);
	}
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_clear_quiescent_channel);

static void channel_free(struct lttng_kernel_ring_buffer_channel *chan)
{
	if (chan->backend.release_priv_ops) {
		chan->backend.release_priv_ops(chan->backend.priv_ops);
	}
	channel_iterator_free(chan);
	channel_backend_free(&chan->backend);
	kfree(chan);
}

/**
 * channel_create - Create channel.
 * @config: ring buffer instance configuration
 * @name: name of the channel
 * @priv: ring buffer client private data
 * @buf_addr: pointer the the beginning of the preallocated buffer contiguous
 *            address mapping. It is used only by RING_BUFFER_STATIC
 *            configuration. It can be set to NULL for other backends.
 * @subbuf_size: subbuffer size
 * @num_subbuf: number of subbuffers
 * @switch_timer_interval: Time interval (in us) to fill sub-buffers with
 *                         padding to let readers get those sub-buffers.
 *                         Used for live streaming.
 * @read_timer_interval: Time interval (in us) to wake up pending readers.
 *
 * Holds cpu hotplug.
 * Returns NULL on failure.
 */
struct lttng_kernel_ring_buffer_channel *channel_create(const struct lttng_kernel_ring_buffer_config *config,
		   const char *name, void *priv, void *buf_addr,
		   size_t subbuf_size,
		   size_t num_subbuf, unsigned int switch_timer_interval,
		   unsigned int read_timer_interval)
{
	int ret;
	struct lttng_kernel_ring_buffer_channel *chan;

	if (lib_ring_buffer_check_config(config, switch_timer_interval,
					 read_timer_interval))
		return NULL;

	chan = kzalloc(sizeof(struct lttng_kernel_ring_buffer_channel), GFP_KERNEL);
	if (!chan)
		return NULL;

	ret = channel_backend_init(&chan->backend, name, config, priv,
				   subbuf_size, num_subbuf);
	if (ret)
		goto error;

	ret = channel_iterator_init(chan);
	if (ret)
		goto error_free_backend;

	chan->commit_count_mask = (~0UL >> chan->backend.num_subbuf_order);
	chan->switch_timer_interval = usecs_to_jiffies(switch_timer_interval);
	chan->read_timer_interval = usecs_to_jiffies(read_timer_interval);
	kref_init(&chan->ref);
	init_waitqueue_head(&chan->read_wait);
	init_waitqueue_head(&chan->hp_wait);
	init_irq_work(&chan->wakeup_pending, lib_ring_buffer_pending_wakeup_chan);

	if (config->alloc == RING_BUFFER_ALLOC_PER_CPU) {
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0))
		chan->cpuhp_prepare.component = LTTNG_RING_BUFFER_FRONTEND;
		ret = cpuhp_state_add_instance_nocalls(lttng_rb_hp_prepare,
			&chan->cpuhp_prepare.node);
		if (ret)
			goto cpuhp_prepare_error;

		chan->cpuhp_online.component = LTTNG_RING_BUFFER_FRONTEND;
		ret = cpuhp_state_add_instance(lttng_rb_hp_online,
			&chan->cpuhp_online.node);
		if (ret)
			goto cpuhp_online_error;
#else /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0)) */
		{
			int cpu;
			/*
			 * In case of non-hotplug cpu, if the ring-buffer is allocated
			 * in early initcall, it will not be notified of secondary cpus.
			 * In that off case, we need to allocate for all possible cpus.
			 */
#ifdef CONFIG_HOTPLUG_CPU
			chan->cpu_hp_notifier.notifier_call =
					lib_ring_buffer_cpu_hp_callback;
			chan->cpu_hp_notifier.priority = 6;
			register_cpu_notifier(&chan->cpu_hp_notifier);

			lttng_cpus_read_lock();
			for_each_online_cpu(cpu) {
				struct lttng_kernel_ring_buffer *buf = per_cpu_ptr(chan->backend.percpu_buf,
								       cpu);
				spin_lock(&per_cpu(ring_buffer_nohz_lock, cpu));
				lib_ring_buffer_start_switch_timer(buf);
				lib_ring_buffer_start_read_timer(buf);
				spin_unlock(&per_cpu(ring_buffer_nohz_lock, cpu));
			}
			chan->cpu_hp_enable = 1;
			lttng_cpus_read_unlock();
#else
			for_each_possible_cpu(cpu) {
				struct lttng_kernel_ring_buffer *buf = per_cpu_ptr(chan->backend.percpu_buf,
								      cpu);
				spin_lock(&per_cpu(ring_buffer_nohz_lock, cpu));
				lib_ring_buffer_start_switch_timer(buf);
				lib_ring_buffer_start_read_timer(buf);
				spin_unlock(&per_cpu(ring_buffer_nohz_lock, cpu));
			}
#endif
		}
#endif /* #else #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0)) */

#if defined(CONFIG_NO_HZ) && defined(CONFIG_LIB_RING_BUFFER)
		/* Only benefit from NO_HZ idle with per-cpu buffers for now. */
		chan->tick_nohz_notifier.notifier_call =
			ring_buffer_tick_nohz_callback;
		chan->tick_nohz_notifier.priority = ~0U;
		atomic_notifier_chain_register(&tick_nohz_notifier,
				       &chan->tick_nohz_notifier);
#endif /* defined(CONFIG_NO_HZ) && defined(CONFIG_LIB_RING_BUFFER) */

	} else {
		struct lttng_kernel_ring_buffer *buf = chan->backend.global_buf;

		lib_ring_buffer_start_switch_timer(buf);
		lib_ring_buffer_start_read_timer(buf);
	}

	return chan;

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0))
cpuhp_online_error:
	ret = cpuhp_state_remove_instance_nocalls(lttng_rb_hp_prepare,
			&chan->cpuhp_prepare.node);
	WARN_ON(ret);
cpuhp_prepare_error:
#endif /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0)) */
error_free_backend:
	channel_backend_free(&chan->backend);
error:
	kfree(chan);
	return NULL;
}
EXPORT_SYMBOL_GPL(channel_create);

static
void channel_release(struct kref *kref)
{
	struct lttng_kernel_ring_buffer_channel *chan = container_of(kref, struct lttng_kernel_ring_buffer_channel, ref);
	channel_free(chan);
}

/**
 * channel_destroy - Finalize, wait for q.s. and destroy channel.
 * @chan: channel to destroy
 *
 * Holds cpu hotplug.
 * Call "destroy" callback, finalize channels, and then decrement the
 * channel reference count.  Note that when readers have completed data
 * consumption of finalized channels, get_subbuf() will return -ENODATA.
 * They should release their handle at that point.  Returns the private
 * data pointer.
 */
void *channel_destroy(struct lttng_kernel_ring_buffer_channel *chan)
{
	int cpu;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	void *priv;

	irq_work_sync(&chan->wakeup_pending);

	channel_unregister_notifiers(chan);

	if (config->alloc == RING_BUFFER_ALLOC_PER_CPU) {
		/*
		 * No need to hold cpu hotplug, because all notifiers have been
		 * unregistered.
		 */
		for_each_channel_cpu(cpu, chan) {
			struct lttng_kernel_ring_buffer *buf = per_cpu_ptr(chan->backend.percpu_buf,
							      cpu);

			if (config->cb.buffer_finalize)
				config->cb.buffer_finalize(buf,
							   chan->backend.priv,
							   cpu);
			/*
			 * Perform flush before writing to finalized.
			 */
			smp_wmb();
			WRITE_ONCE(buf->finalized, 1);
			wake_up_interruptible(&buf->read_wait);
		}
	} else {
		struct lttng_kernel_ring_buffer *buf = chan->backend.global_buf;

		if (config->cb.buffer_finalize)
			config->cb.buffer_finalize(buf, chan->backend.priv, -1);
		/*
		 * Perform flush before writing to finalized.
		 */
		smp_wmb();
		WRITE_ONCE(buf->finalized, 1);
		wake_up_interruptible(&buf->read_wait);
	}
	WRITE_ONCE(chan->finalized, 1);
	wake_up_interruptible(&chan->hp_wait);
	wake_up_interruptible(&chan->read_wait);
	priv = chan->backend.priv;
	kref_put(&chan->ref, channel_release);
	return priv;
}
EXPORT_SYMBOL_GPL(channel_destroy);

struct lttng_kernel_ring_buffer *channel_get_ring_buffer(
					const struct lttng_kernel_ring_buffer_config *config,
					struct lttng_kernel_ring_buffer_channel *chan, int cpu)
{
	if (config->alloc == RING_BUFFER_ALLOC_GLOBAL)
		return chan->backend.global_buf;
	else
		return per_cpu_ptr(chan->backend.percpu_buf, cpu);
}
EXPORT_SYMBOL_GPL(channel_get_ring_buffer);

int lib_ring_buffer_open_read(struct lttng_kernel_ring_buffer *buf)
{
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;

	if (!atomic_long_add_unless(&buf->active_readers, 1, 1))
		return -EBUSY;
	if (!lttng_kref_get(&chan->ref)) {
		atomic_long_dec(&buf->active_readers);
		return -EOVERFLOW;
	}
	lttng_smp_mb__after_atomic();
	return 0;
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_open_read);

void lib_ring_buffer_release_read(struct lttng_kernel_ring_buffer *buf)
{
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;

	CHAN_WARN_ON(chan, atomic_long_read(&buf->active_readers) != 1);
	lttng_smp_mb__before_atomic();
	atomic_long_dec(&buf->active_readers);
	kref_put(&chan->ref, channel_release);
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_release_read);

/*
 * Promote compiler barrier to a smp_mb().
 * For the specific ring buffer case, this IPI call should be removed if the
 * architecture does not reorder writes.  This should eventually be provided by
 * a separate architecture-specific infrastructure.
 */
static void remote_mb(void *info)
{
	smp_mb();
}

/**
 * lib_ring_buffer_snapshot - save subbuffer position snapshot (for read)
 * @buf: ring buffer
 * @consumed: consumed count indicating the position where to read
 * @produced: produced count, indicates position when to stop reading
 *
 * Returns -ENODATA if buffer is finalized, -EAGAIN if there is currently no
 * data to read at consumed position, or 0 if the get operation succeeds.
 * Busy-loop trying to get data if the tick_nohz sequence lock is held.
 */

int lib_ring_buffer_snapshot(struct lttng_kernel_ring_buffer *buf,
			     unsigned long *consumed, unsigned long *produced)
{
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	unsigned long consumed_cur, write_offset;
	int finalized;

retry:
	finalized = LTTNG_READ_ONCE(buf->finalized);
	/*
	 * Read finalized before counters.
	 */
	smp_rmb();
	consumed_cur = atomic_long_read(&buf->consumed);
	/*
	 * No need to issue a memory barrier between consumed count read and
	 * write offset read, because consumed count can only change
	 * concurrently in overwrite mode, and we keep a sequence counter
	 * identifier derived from the write offset to check we are getting
	 * the same sub-buffer we are expecting (the sub-buffers are atomically
	 * "tagged" upon writes, tags are checked upon read).
	 */
	write_offset = v_read(config, &buf->offset);

	/*
	 * Check that we are not about to read the same subbuffer in
	 * which the writer head is.
	 */
	if (subbuf_trunc(write_offset, chan) - subbuf_trunc(consumed_cur, chan)
	    == 0)
		goto nodata;

	*consumed = consumed_cur;
	*produced = subbuf_trunc(write_offset, chan);

	return 0;

nodata:
	/*
	 * The memory barriers __wait_event()/wake_up_interruptible() take care
	 * of "raw_spin_is_locked" memory ordering.
	 */
	if (finalized)
		return -ENODATA;
	else if (raw_spin_is_locked(&buf->raw_tick_nohz_spinlock))
		goto retry;
	else
		return -EAGAIN;
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_snapshot);

/**
 * Performs the same function as lib_ring_buffer_snapshot(), but the positions
 * are saved regardless of whether the consumed and produced positions are
 * in the same subbuffer.
 * @buf: ring buffer
 * @consumed: consumed byte count indicating the last position read
 * @produced: produced byte count indicating the last position written
 *
 * This function is meant to provide information on the exact producer and
 * consumer positions without regard for the "snapshot" feature.
 */
int lib_ring_buffer_snapshot_sample_positions(struct lttng_kernel_ring_buffer *buf,
		unsigned long *consumed, unsigned long *produced)
{
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;

	smp_rmb();
	*consumed = atomic_long_read(&buf->consumed);
	/*
	 * No need to issue a memory barrier between consumed count read and
	 * write offset read, because consumed count can only change
	 * concurrently in overwrite mode, and we keep a sequence counter
	 * identifier derived from the write offset to check we are getting
	 * the same sub-buffer we are expecting (the sub-buffers are atomically
	 * "tagged" upon writes, tags are checked upon read).
	 */
	*produced = v_read(config, &buf->offset);
	return 0;
}

/**
 * lib_ring_buffer_put_snapshot - move consumed counter forward
 *
 * Should only be called from consumer context.
 * @buf: ring buffer
 * @consumed_new: new consumed count value
 */
void lib_ring_buffer_move_consumer(struct lttng_kernel_ring_buffer *buf,
				   unsigned long consumed_new)
{
	struct lttng_kernel_ring_buffer_backend *bufb = &buf->backend;
	struct lttng_kernel_ring_buffer_channel *chan = bufb->chan;
	unsigned long consumed;

	CHAN_WARN_ON(chan, atomic_long_read(&buf->active_readers) != 1);

	/*
	 * Only push the consumed value forward.
	 * If the consumed cmpxchg fails, this is because we have been pushed by
	 * the writer in flight recorder mode.
	 */
	consumed = atomic_long_read(&buf->consumed);
	while ((long) consumed - (long) consumed_new < 0)
		consumed = atomic_long_cmpxchg(&buf->consumed, consumed,
					       consumed_new);
	/* Wake-up the metadata producer */
	wake_up_interruptible(&buf->write_wait);
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_move_consumer);

#if ARCH_IMPLEMENTS_FLUSH_DCACHE_PAGE
static void lib_ring_buffer_flush_read_subbuf_dcache(
		const struct lttng_kernel_ring_buffer_config *config,
		struct lttng_kernel_ring_buffer_channel *chan,
		struct lttng_kernel_ring_buffer *buf)
{
	struct lttng_kernel_ring_buffer_backend_pages *pages;
	unsigned long sb_bindex, id, i, nr_pages;

	if (config->output != RING_BUFFER_MMAP)
		return;

	/*
	 * Architectures with caches aliased on virtual addresses may
	 * use different cache lines for the linear mapping vs
	 * user-space memory mapping. Given that the ring buffer is
	 * based on the kernel linear mapping, aligning it with the
	 * user-space mapping is not straightforward, and would require
	 * extra TLB entries. Therefore, simply flush the dcache for the
	 * entire sub-buffer before reading it.
	 */
	id = buf->backend.buf_rsb.id;
	sb_bindex = subbuffer_id_get_index(config, id);
	pages = buf->backend.array[sb_bindex];
	nr_pages = buf->backend.num_pages_per_subbuf;
	for (i = 0; i < nr_pages; i++) {
		struct lttng_kernel_ring_buffer_backend_page *backend_page;

		backend_page = &pages->p[i];
		flush_dcache_page(pfn_to_page(backend_page->pfn));
	}
}
#else
static void lib_ring_buffer_flush_read_subbuf_dcache(
		const struct lttng_kernel_ring_buffer_config *config,
		struct lttng_kernel_ring_buffer_channel *chan,
		struct lttng_kernel_ring_buffer *buf)
{
}
#endif

/**
 * lib_ring_buffer_get_subbuf - get exclusive access to subbuffer for reading
 * @buf: ring buffer
 * @consumed: consumed count indicating the position where to read
 *
 * Returns -ENODATA if buffer is finalized, -EAGAIN if there is currently no
 * data to read at consumed position, or 0 if the get operation succeeds.
 * Busy-loop trying to get data if the tick_nohz sequence lock is held.
 */
int lib_ring_buffer_get_subbuf(struct lttng_kernel_ring_buffer *buf,
			       unsigned long consumed)
{
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	unsigned long consumed_cur, consumed_idx, commit_count, write_offset;
	int ret;
	int finalized;

	if (buf->get_subbuf) {
		/*
		 * Reader is trying to get a subbuffer twice.
		 */
		CHAN_WARN_ON(chan, 1);
		return -EBUSY;
	}
retry:
	finalized = LTTNG_READ_ONCE(buf->finalized);
	/*
	 * Read finalized before counters.
	 */
	smp_rmb();
	consumed_cur = atomic_long_read(&buf->consumed);
	consumed_idx = subbuf_index(consumed, chan);
	commit_count = v_read(config, &buf->commit_cold[consumed_idx].cc_sb);
	/*
	 * Make sure we read the commit count before reading the buffer
	 * data and the write offset. Correct consumed offset ordering
	 * wrt commit count is insured by the use of cmpxchg to update
	 * the consumed offset.
	 * smp_call_function_single can fail if the remote CPU is offline,
	 * this is OK because then there is no wmb to execute there.
	 * If our thread is executing on the same CPU as the on the buffers
	 * belongs to, we don't have to synchronize it at all. If we are
	 * migrated, the scheduler will take care of the memory barriers.
	 * Normally, smp_call_function_single() should ensure program order when
	 * executing the remote function, which implies that it surrounds the
	 * function execution with :
	 * smp_mb()
	 * send IPI
	 * csd_lock_wait
	 *                recv IPI
	 *                smp_mb()
	 *                exec. function
	 *                smp_mb()
	 *                csd unlock
	 * smp_mb()
	 *
	 * However, smp_call_function_single() does not seem to clearly execute
	 * such barriers. It depends on spinlock semantic to provide the barrier
	 * before executing the IPI and, when busy-looping, csd_lock_wait only
	 * executes smp_mb() when it has to wait for the other CPU.
	 *
	 * I don't trust this code. Therefore, let's add the smp_mb() sequence
	 * required ourself, even if duplicated. It has no performance impact
	 * anyway.
	 *
	 * smp_mb() is needed because smp_rmb() and smp_wmb() only order read vs
	 * read and write vs write. They do not ensure core synchronization. We
	 * really have to ensure total order between the 3 barriers running on
	 * the 2 CPUs.
	 */
	if (config->ipi == RING_BUFFER_IPI_BARRIER) {
		if (config->sync == RING_BUFFER_SYNC_PER_CPU
		    && config->alloc == RING_BUFFER_ALLOC_PER_CPU) {
			if (raw_smp_processor_id() != buf->backend.cpu) {
				/* Total order with IPI handler smp_mb() */
				smp_mb();
				smp_call_function_single(buf->backend.cpu,
							 remote_mb, NULL, 1);
				/* Total order with IPI handler smp_mb() */
				smp_mb();
			}
		} else {
			/* Total order with IPI handler smp_mb() */
			smp_mb();
			smp_call_function(remote_mb, NULL, 1);
			/* Total order with IPI handler smp_mb() */
			smp_mb();
		}
	} else {
		/*
		 * Local rmb to match the remote wmb to read the commit count
		 * before the buffer data and the write offset.
		 */
		smp_rmb();
	}

	write_offset = v_read(config, &buf->offset);

	/*
	 * Check that the buffer we are getting is after or at consumed_cur
	 * position.
	 */
	if ((long) subbuf_trunc(consumed, chan)
	    - (long) subbuf_trunc(consumed_cur, chan) < 0)
		goto nodata;

	/*
	 * Check that the subbuffer we are trying to consume has been
	 * already fully committed.
	 */
	if (((commit_count - chan->backend.subbuf_size)
	     & chan->commit_count_mask)
	    - (buf_trunc(consumed, chan)
	       >> chan->backend.num_subbuf_order)
	    != 0)
		goto nodata;

	/*
	 * Check that we are not about to read the same subbuffer in
	 * which the writer head is.
	 */
	if (subbuf_trunc(write_offset, chan) - subbuf_trunc(consumed, chan)
	    == 0)
		goto nodata;

	/*
	 * Failure to get the subbuffer causes a busy-loop retry without going
	 * to a wait queue. These are caused by short-lived race windows where
	 * the writer is getting access to a subbuffer we were trying to get
	 * access to. Also checks that the "consumed" buffer count we are
	 * looking for matches the one contained in the subbuffer id.
	 */
	ret = update_read_sb_index(config, &buf->backend, &chan->backend,
				   consumed_idx, buf_trunc_val(consumed, chan));
	if (ret)
		goto retry;
	subbuffer_id_clear_noref(config, &buf->backend.buf_rsb.id);

	buf->get_subbuf_consumed = consumed;
	buf->get_subbuf = 1;

	lib_ring_buffer_flush_read_subbuf_dcache(config, chan, buf);

	return 0;

nodata:
	/*
	 * The memory barriers __wait_event()/wake_up_interruptible() take care
	 * of "raw_spin_is_locked" memory ordering.
	 */
	if (finalized)
		return -ENODATA;
	else if (raw_spin_is_locked(&buf->raw_tick_nohz_spinlock))
		goto retry;
	else
		return -EAGAIN;
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_get_subbuf);

/**
 * lib_ring_buffer_put_subbuf - release exclusive subbuffer access
 * @buf: ring buffer
 */
void lib_ring_buffer_put_subbuf(struct lttng_kernel_ring_buffer *buf)
{
	struct lttng_kernel_ring_buffer_backend *bufb = &buf->backend;
	struct lttng_kernel_ring_buffer_channel *chan = bufb->chan;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	unsigned long read_sb_bindex, consumed_idx, consumed;

	CHAN_WARN_ON(chan, atomic_long_read(&buf->active_readers) != 1);

	if (!buf->get_subbuf) {
		/*
		 * Reader puts a subbuffer it did not get.
		 */
		CHAN_WARN_ON(chan, 1);
		return;
	}
	consumed = buf->get_subbuf_consumed;
	buf->get_subbuf = 0;

	/*
	 * Clear the records_unread counter. (overruns counter)
	 * Can still be non-zero if a file reader simply grabbed the data
	 * without using iterators.
	 * Can be below zero if an iterator is used on a snapshot more than
	 * once.
	 */
	read_sb_bindex = subbuffer_id_get_index(config, bufb->buf_rsb.id);
	v_add(config, v_read(config,
			     &bufb->array[read_sb_bindex]->records_unread),
	      &bufb->records_read);
	v_set(config, &bufb->array[read_sb_bindex]->records_unread, 0);
	CHAN_WARN_ON(chan, config->mode == RING_BUFFER_OVERWRITE
		     && subbuffer_id_is_noref(config, bufb->buf_rsb.id));
	subbuffer_id_set_noref(config, &bufb->buf_rsb.id);

	/*
	 * Exchange the reader subbuffer with the one we put in its place in the
	 * writer subbuffer table. Expect the original consumed count. If
	 * update_read_sb_index fails, this is because the writer updated the
	 * subbuffer concurrently. We should therefore keep the subbuffer we
	 * currently have: it has become invalid to try reading this sub-buffer
	 * consumed count value anyway.
	 */
	consumed_idx = subbuf_index(consumed, chan);
	update_read_sb_index(config, &buf->backend, &chan->backend,
			     consumed_idx, buf_trunc_val(consumed, chan));
	/*
	 * update_read_sb_index return value ignored. Don't exchange sub-buffer
	 * if the writer concurrently updated it.
	 */
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_put_subbuf);

/*
 * cons_offset is an iterator on all subbuffer offsets between the reader
 * position and the writer position. (inclusive)
 */
static
void lib_ring_buffer_print_subbuffer_errors(struct lttng_kernel_ring_buffer *buf,
					    struct lttng_kernel_ring_buffer_channel *chan,
					    unsigned long cons_offset,
					    int cpu)
{
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	unsigned long cons_idx, commit_count, commit_count_sb;

	cons_idx = subbuf_index(cons_offset, chan);
	commit_count = v_read(config, &buf->commit_hot[cons_idx].cc);
	commit_count_sb = v_read(config, &buf->commit_cold[cons_idx].cc_sb);

	if (subbuf_offset(commit_count, chan) != 0)
		printk(KERN_WARNING
		       "LTTng: ring buffer %s, cpu %d: "
		       "commit count in subbuffer %lu,\n"
		       "expecting multiples of %lu bytes\n"
		       "  [ %lu bytes committed, %lu bytes reader-visible ]\n",
		       chan->backend.name, cpu, cons_idx,
		       chan->backend.subbuf_size,
		       commit_count, commit_count_sb);

	printk(KERN_DEBUG "LTTng: ring buffer: %s, cpu %d: %lu bytes committed\n",
	       chan->backend.name, cpu, commit_count);
}

static
void lib_ring_buffer_print_buffer_errors(struct lttng_kernel_ring_buffer *buf,
					 struct lttng_kernel_ring_buffer_channel *chan,
					 void *priv, int cpu)
{
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	unsigned long write_offset, cons_offset;

	/*
	 * No need to order commit_count, write_offset and cons_offset reads
	 * because we execute at teardown when no more writer nor reader
	 * references are left.
	 */
	write_offset = v_read(config, &buf->offset);
	cons_offset = atomic_long_read(&buf->consumed);

	/*
	 * Only print errors if the session associated with the buffer has been
	 * active, otherwise destroying an un-started session results in errors
	 * because of the un-consumed packet header present in the buffer.
	 */
	if (chan->been_active && write_offset != cons_offset)
		printk(KERN_DEBUG
		       "LTTng: ring buffer %s, cpu %d: "
		       "non-consumed data\n"
		       "  [ %lu bytes written, %lu bytes read ]\n",
		       chan->backend.name, cpu, write_offset, cons_offset);

	for (cons_offset = atomic_long_read(&buf->consumed);
	     (long) (subbuf_trunc((unsigned long) v_read(config, &buf->offset),
				  chan)
		     - cons_offset) > 0;
	     cons_offset = subbuf_align(cons_offset, chan))
		lib_ring_buffer_print_subbuffer_errors(buf, chan, cons_offset,
						       cpu);
}

#ifdef LTTNG_RING_BUFFER_COUNT_EVENTS
static
void lib_ring_buffer_print_records_count(struct lttng_kernel_ring_buffer_channel *chan,
					 struct lttng_kernel_ring_buffer *buf,
					 int cpu)
{
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;

	if (!strcmp(chan->backend.name, "relay-metadata")) {
		printk(KERN_DEBUG "LTTng: ring buffer %s: %lu records written, "
			"%lu records overrun\n",
			chan->backend.name,
			v_read(config, &buf->records_count),
			v_read(config, &buf->records_overrun));
	} else {
		printk(KERN_DEBUG "LTTng: ring buffer %s, cpu %d: %lu records written, "
			"%lu records overrun\n",
			chan->backend.name, cpu,
			v_read(config, &buf->records_count),
			v_read(config, &buf->records_overrun));
	}
}
#else
static
void lib_ring_buffer_print_records_count(struct lttng_kernel_ring_buffer_channel *chan,
					 struct lttng_kernel_ring_buffer *buf,
					 int cpu)
{
}
#endif

static
void lib_ring_buffer_print_errors(struct lttng_kernel_ring_buffer_channel *chan,
				  struct lttng_kernel_ring_buffer *buf, int cpu)
{
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	void *priv = chan->backend.priv;

	lib_ring_buffer_print_records_count(chan, buf, cpu);
	if (strcmp(chan->backend.name, "relay-metadata")) {
		if (v_read(config, &buf->records_lost_full)
		    || v_read(config, &buf->records_lost_wrap)
		    || v_read(config, &buf->records_lost_big))
			printk(KERN_WARNING
				"LTTng: ring buffer %s, cpu %d: records were lost. Caused by:\n"
				"  [ %lu buffer full, %lu nest buffer wrap-around, "
				"%lu event too big ]\n",
				chan->backend.name, cpu,
				v_read(config, &buf->records_lost_full),
				v_read(config, &buf->records_lost_wrap),
				v_read(config, &buf->records_lost_big));
	}
	lib_ring_buffer_print_buffer_errors(buf, chan, priv, cpu);
}

/*
 * lib_ring_buffer_switch_old_start: Populate old subbuffer header.
 *
 * Only executed when the buffer is finalized, in SWITCH_FLUSH.
 */
static
void lib_ring_buffer_switch_old_start(struct lttng_kernel_ring_buffer *buf,
				      struct lttng_kernel_ring_buffer_channel *chan,
				      struct switch_offsets *offsets,
				      const struct lttng_kernel_ring_buffer_ctx *ctx)
{
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	unsigned long oldidx = subbuf_index(offsets->old, chan);
	unsigned long commit_count;
	struct commit_counters_hot *cc_hot;

	config->cb.buffer_begin(buf, ctx->priv.tsc, oldidx);

	/*
	 * Order all writes to buffer before the commit count update that will
	 * determine that the subbuffer is full.
	 */
	if (config->ipi == RING_BUFFER_IPI_BARRIER) {
		/*
		 * Must write slot data before incrementing commit count.  This
		 * compiler barrier is upgraded into a smp_mb() by the IPI sent
		 * by get_subbuf().
		 */
		barrier();
	} else
		smp_wmb();
	cc_hot = &buf->commit_hot[oldidx];
	v_add(config, config->cb.subbuffer_header_size(), &cc_hot->cc);
	commit_count = v_read(config, &cc_hot->cc);
	/* Check if the written buffer has to be delivered */
	lib_ring_buffer_check_deliver(config, buf, chan, offsets->old,
				      commit_count, oldidx, ctx);
	lib_ring_buffer_write_commit_counter(config, buf, chan,
			offsets->old + config->cb.subbuffer_header_size(),
			commit_count, cc_hot);
}

/*
 * lib_ring_buffer_switch_old_end: switch old subbuffer
 *
 * Note : offset_old should never be 0 here. It is ok, because we never perform
 * buffer switch on an empty subbuffer in SWITCH_ACTIVE mode. The caller
 * increments the offset_old value when doing a SWITCH_FLUSH on an empty
 * subbuffer.
 */
static
void lib_ring_buffer_switch_old_end(struct lttng_kernel_ring_buffer *buf,
				    struct lttng_kernel_ring_buffer_channel *chan,
				    struct switch_offsets *offsets,
				    const struct lttng_kernel_ring_buffer_ctx *ctx)
{
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	unsigned long oldidx = subbuf_index(offsets->old - 1, chan);
	unsigned long commit_count, padding_size, data_size;
	struct commit_counters_hot *cc_hot;
	u64 *ts_end;

	data_size = subbuf_offset(offsets->old - 1, chan) + 1;
	padding_size = chan->backend.subbuf_size - data_size;
	subbuffer_set_data_size(config, &buf->backend, oldidx, data_size);

	ts_end = &buf->ts_end[oldidx];
	/*
	 * This is the last space reservation in that sub-buffer before
	 * it gets delivered. This provides exclusive access to write to
	 * this sub-buffer's ts_end. There are also no concurrent
	 * readers of that ts_end because delivery of that sub-buffer is
	 * postponed until the commit counter is incremented for the
	 * current space reservation.
	 */
	*ts_end = ctx->priv.tsc;

	/*
	 * Order all writes to buffer and store to ts_end before the commit
	 * count update that will determine that the subbuffer is full.
	 */
	if (config->ipi == RING_BUFFER_IPI_BARRIER) {
		/*
		 * Must write slot data before incrementing commit count.  This
		 * compiler barrier is upgraded into a smp_mb() by the IPI sent
		 * by get_subbuf().
		 */
		barrier();
	} else
		smp_wmb();
	cc_hot = &buf->commit_hot[oldidx];
	v_add(config, padding_size, &cc_hot->cc);
	commit_count = v_read(config, &cc_hot->cc);
	lib_ring_buffer_check_deliver(config, buf, chan, offsets->old - 1,
				      commit_count, oldidx, ctx);
	lib_ring_buffer_write_commit_counter(config, buf, chan,
			offsets->old + padding_size, commit_count,
			cc_hot);
}

/*
 * lib_ring_buffer_switch_new_start: Populate new subbuffer.
 *
 * This code can be executed unordered : writers may already have written to the
 * sub-buffer before this code gets executed, caution.  The commit makes sure
 * that this code is executed before the deliver of this sub-buffer.
 */
static
void lib_ring_buffer_switch_new_start(struct lttng_kernel_ring_buffer *buf,
				      struct lttng_kernel_ring_buffer_channel *chan,
				      struct switch_offsets *offsets,
				      const struct lttng_kernel_ring_buffer_ctx *ctx)
{
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	unsigned long beginidx = subbuf_index(offsets->begin, chan);
	unsigned long commit_count;
	struct commit_counters_hot *cc_hot;

	config->cb.buffer_begin(buf, ctx->priv.tsc, beginidx);

	/*
	 * Order all writes to buffer before the commit count update that will
	 * determine that the subbuffer is full.
	 */
	if (config->ipi == RING_BUFFER_IPI_BARRIER) {
		/*
		 * Must write slot data before incrementing commit count.  This
		 * compiler barrier is upgraded into a smp_mb() by the IPI sent
		 * by get_subbuf().
		 */
		barrier();
	} else
		smp_wmb();
	cc_hot = &buf->commit_hot[beginidx];
	v_add(config, config->cb.subbuffer_header_size(), &cc_hot->cc);
	commit_count = v_read(config, &cc_hot->cc);
	/* Check if the written buffer has to be delivered */
	lib_ring_buffer_check_deliver(config, buf, chan, offsets->begin,
				      commit_count, beginidx, ctx);
	lib_ring_buffer_write_commit_counter(config, buf, chan,
			offsets->begin + config->cb.subbuffer_header_size(),
			commit_count, cc_hot);
}

/*
 * lib_ring_buffer_switch_new_end: finish switching current subbuffer
 *
 * Calls subbuffer_set_data_size() to set the data size of the current
 * sub-buffer. We do not need to perform check_deliver nor commit here,
 * since this task will be done by the "commit" of the event for which
 * we are currently doing the space reservation.
 */
static
void lib_ring_buffer_switch_new_end(struct lttng_kernel_ring_buffer *buf,
					    struct lttng_kernel_ring_buffer_channel *chan,
					    struct switch_offsets *offsets,
					    const struct lttng_kernel_ring_buffer_ctx *ctx)
{
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	unsigned long endidx, data_size;
	u64 *ts_end;

	endidx = subbuf_index(offsets->end - 1, chan);
	data_size = subbuf_offset(offsets->end - 1, chan) + 1;
	subbuffer_set_data_size(config, &buf->backend, endidx, data_size);
	ts_end = &buf->ts_end[endidx];
	/*
	 * This is the last space reservation in that sub-buffer before
	 * it gets delivered. This provides exclusive access to write to
	 * this sub-buffer's ts_end. There are also no concurrent
	 * readers of that ts_end because delivery of that sub-buffer is
	 * postponed until the commit counter is incremented for the
	 * current space reservation.
	 */
	*ts_end = ctx->priv.tsc;
}

/*
 * Returns :
 * 0 if ok
 * !0 if execution must be aborted.
 */
static
int lib_ring_buffer_try_switch_slow(enum switch_mode mode,
				    struct lttng_kernel_ring_buffer *buf,
				    struct lttng_kernel_ring_buffer_channel *chan,
				    struct switch_offsets *offsets,
				    struct lttng_kernel_ring_buffer_ctx *ctx)
{
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	unsigned long off, reserve_commit_diff;

	offsets->begin = v_read(config, &buf->offset);
	offsets->old = offsets->begin;
	offsets->switch_old_start = 0;
	off = subbuf_offset(offsets->begin, chan);

	ctx->priv.tsc = config->cb.ring_buffer_clock_read(chan);

	/*
	 * Ensure we flush the header of an empty subbuffer when doing the
	 * finalize (SWITCH_FLUSH). This ensures that we end up knowing the
	 * total data gathering duration even if there were no records saved
	 * after the last buffer switch.
	 * In SWITCH_ACTIVE mode, switch the buffer when it contains events.
	 * SWITCH_ACTIVE only flushes the current subbuffer, dealing with end of
	 * subbuffer header as appropriate.
	 * The next record that reserves space will be responsible for
	 * populating the following subbuffer header. We choose not to populate
	 * the next subbuffer header here because we want to be able to use
	 * SWITCH_ACTIVE for periodical buffer flush and CPU tick_nohz stop
	 * buffer flush, which must guarantee that all the buffer content
	 * (records and header timestamps) are visible to the reader. This is
	 * required for quiescence guarantees for the fusion merge.
	 */
	if (mode != SWITCH_FLUSH && !off)
		return -1;	/* we do not have to switch : buffer is empty */

	if (unlikely(off == 0)) {
		unsigned long sb_index, commit_count;

		/*
		 * We are performing a SWITCH_FLUSH. At this stage, there are no
		 * concurrent writes into the buffer.
		 *
		 * The client does not save any header information.  Don't
		 * switch empty subbuffer on finalize, because it is invalid to
		 * deliver a completely empty subbuffer.
		 */
		if (!config->cb.subbuffer_header_size())
			return -1;

		/* Test new buffer integrity */
		sb_index = subbuf_index(offsets->begin, chan);
		commit_count = v_read(config,
				&buf->commit_cold[sb_index].cc_sb);
		reserve_commit_diff =
		  (buf_trunc(offsets->begin, chan)
		   >> chan->backend.num_subbuf_order)
		  - (commit_count & chan->commit_count_mask);
		if (likely(reserve_commit_diff == 0)) {
			/* Next subbuffer not being written to. */
			if (unlikely(config->mode != RING_BUFFER_OVERWRITE &&
				subbuf_trunc(offsets->begin, chan)
				 - subbuf_trunc((unsigned long)
				     atomic_long_read(&buf->consumed), chan)
				>= chan->backend.buf_size)) {
				/*
				 * We do not overwrite non consumed buffers
				 * and we are full : don't switch.
				 */
				return -1;
			} else {
				/*
				 * Next subbuffer not being written to, and we
				 * are either in overwrite mode or the buffer is
				 * not full. It's safe to write in this new
				 * subbuffer.
				 */
			}
		} else {
			/*
			 * Next subbuffer reserve offset does not match the
			 * commit offset. Don't perform switch in
			 * producer-consumer and overwrite mode.  Caused by
			 * either a writer OOPS or too many nested writes over a
			 * reserve/commit pair.
			 */
			return -1;
		}

		/*
		 * Need to write the subbuffer start header on finalize.
		 */
		offsets->switch_old_start = 1;
	}
	offsets->begin = subbuf_align(offsets->begin, chan);
	/* Note: old points to the next subbuf at offset 0 */
	offsets->end = offsets->begin;
	/*
	 * Populate the records lost counters prior to performing a
	 * sub-buffer switch.
	 */
	ctx->priv.records_lost_full = v_read(config, &buf->records_lost_full);
	ctx->priv.records_lost_wrap = v_read(config, &buf->records_lost_wrap);
	ctx->priv.records_lost_big = v_read(config, &buf->records_lost_big);
	return 0;
}

/*
 * Force a sub-buffer switch. This operation is completely reentrant : can be
 * called while tracing is active with absolutely no lock held.
 *
 * Note, however, that as a v_cmpxchg is used for some atomic
 * operations, this function must be called from the CPU which owns the buffer
 * for a ACTIVE flush.
 */
void lib_ring_buffer_switch_slow(struct lttng_kernel_ring_buffer *buf, enum switch_mode mode)
{
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	struct lttng_kernel_ring_buffer_ctx ctx;
	struct switch_offsets offsets;
	unsigned long oldidx;

	offsets.size = 0;

	/*
	 * Perform retryable operations.
	 */
	do {
		if (lib_ring_buffer_try_switch_slow(mode, buf, chan, &offsets,
						    &ctx))
			return;	/* Switch not needed */
	} while (v_cmpxchg(config, &buf->offset, offsets.old, offsets.end)
		 != offsets.old);

	/*
	 * Atomically update last_tsc. This update races against concurrent
	 * atomic updates, but the race will always cause supplementary full TSC
	 * records, never the opposite (missing a full TSC record when it would
	 * be needed).
	 */
	save_last_tsc(config, buf, ctx.priv.tsc);

	/*
	 * Push the reader if necessary
	 */
	lib_ring_buffer_reserve_push_reader(buf, chan, offsets.old);

	oldidx = subbuf_index(offsets.old, chan);
	lib_ring_buffer_clear_noref(config, &buf->backend, oldidx);

	/*
	 * May need to populate header start on SWITCH_FLUSH.
	 */
	if (offsets.switch_old_start) {
		lib_ring_buffer_switch_old_start(buf, chan, &offsets, &ctx);
		offsets.old += config->cb.subbuffer_header_size();
	}

	/*
	 * Switch old subbuffer.
	 */
	lib_ring_buffer_switch_old_end(buf, chan, &offsets, &ctx);
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_switch_slow);

struct switch_param {
	struct lttng_kernel_ring_buffer *buf;
	enum switch_mode mode;
};

static void remote_switch(void *info)
{
	struct switch_param *param = info;
	struct lttng_kernel_ring_buffer *buf = param->buf;

	lib_ring_buffer_switch_slow(buf, param->mode);
}

static void _lib_ring_buffer_switch_remote(struct lttng_kernel_ring_buffer *buf,
		enum switch_mode mode)
{
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	int ret;
	struct switch_param param;

	/*
	 * With global synchronization we don't need to use the IPI scheme.
	 */
	if (config->sync == RING_BUFFER_SYNC_GLOBAL) {
		lib_ring_buffer_switch_slow(buf, mode);
		return;
	}

	/*
	 * Disabling preemption ensures two things: first, that the
	 * target cpu is not taken concurrently offline while we are within
	 * smp_call_function_single(). Secondly, if it happens that the
	 * CPU is not online, our own call to lib_ring_buffer_switch_slow()
	 * needs to be protected from CPU hotplug handlers, which can
	 * also perform a remote subbuffer switch.
	 */
	preempt_disable();
	param.buf = buf;
	param.mode = mode;
	ret = smp_call_function_single(buf->backend.cpu,
				 remote_switch, &param, 1);
	if (ret) {
		/* Remote CPU is offline, do it ourself. */
		lib_ring_buffer_switch_slow(buf, mode);
	}
	preempt_enable();
}

/* Switch sub-buffer if current sub-buffer is non-empty. */
void lib_ring_buffer_switch_remote(struct lttng_kernel_ring_buffer *buf)
{
	_lib_ring_buffer_switch_remote(buf, SWITCH_ACTIVE);
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_switch_remote);

/* Switch sub-buffer even if current sub-buffer is empty. */
void lib_ring_buffer_switch_remote_empty(struct lttng_kernel_ring_buffer *buf)
{
	_lib_ring_buffer_switch_remote(buf, SWITCH_FLUSH);
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_switch_remote_empty);

void lib_ring_buffer_clear(struct lttng_kernel_ring_buffer *buf)
{
	struct lttng_kernel_ring_buffer_backend *bufb = &buf->backend;
	struct lttng_kernel_ring_buffer_channel *chan = bufb->chan;

	lib_ring_buffer_switch_remote(buf);
	lib_ring_buffer_clear_reader(buf, chan);
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_clear);

/*
 * Returns :
 * 0 if ok
 * -ENOSPC if event size is too large for packet.
 * -ENOBUFS if there is currently not enough space in buffer for the event.
 * -EIO if data cannot be written into the buffer for any other reason.
 */
static
int lib_ring_buffer_try_reserve_slow(struct lttng_kernel_ring_buffer *buf,
				     struct lttng_kernel_ring_buffer_channel *chan,
				     struct switch_offsets *offsets,
				     struct lttng_kernel_ring_buffer_ctx *ctx,
				     void *client_ctx)
{
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	unsigned long reserve_commit_diff, offset_cmp;

retry:
	offsets->begin = offset_cmp = v_read(config, &buf->offset);
	offsets->old = offsets->begin;
	offsets->switch_new_start = 0;
	offsets->switch_new_end = 0;
	offsets->switch_old_end = 0;
	offsets->pre_header_padding = 0;

	ctx->priv.tsc = config->cb.ring_buffer_clock_read(chan);
	if ((int64_t) ctx->priv.tsc == -EIO)
		return -EIO;

	if (last_tsc_overflow(config, buf, ctx->priv.tsc))
		ctx->priv.rflags |= RING_BUFFER_RFLAG_FULL_TSC;

	if (unlikely(subbuf_offset(offsets->begin, ctx->priv.chan) == 0)) {
		offsets->switch_new_start = 1;		/* For offsets->begin */
	} else {
		offsets->size = config->cb.record_header_size(config, chan,
						offsets->begin,
						&offsets->pre_header_padding,
						ctx, client_ctx);
		offsets->size +=
			lib_ring_buffer_align(offsets->begin + offsets->size,
					      ctx->largest_align)
			+ ctx->data_size;
		if (unlikely(subbuf_offset(offsets->begin, chan) +
			     offsets->size > chan->backend.subbuf_size)) {
			offsets->switch_old_end = 1;	/* For offsets->old */
			offsets->switch_new_start = 1;	/* For offsets->begin */
		}
	}
	if (unlikely(offsets->switch_new_start)) {
		unsigned long sb_index, commit_count;

		/*
		 * We are typically not filling the previous buffer completely.
		 */
		if (likely(offsets->switch_old_end))
			offsets->begin = subbuf_align(offsets->begin, chan);
		offsets->begin = offsets->begin
				 + config->cb.subbuffer_header_size();
		/* Test new buffer integrity */
		sb_index = subbuf_index(offsets->begin, chan);
		/*
		 * Read buf->offset before buf->commit_cold[sb_index].cc_sb.
		 * lib_ring_buffer_check_deliver() has the matching
		 * memory barriers required around commit_cold cc_sb
		 * updates to ensure reserve and commit counter updates
		 * are not seen reordered when updated by another CPU.
		 */
		smp_rmb();
		commit_count = v_read(config,
				&buf->commit_cold[sb_index].cc_sb);
		/* Read buf->commit_cold[sb_index].cc_sb before buf->offset. */
		smp_rmb();
		if (unlikely(offset_cmp != v_read(config, &buf->offset))) {
			/*
			 * The reserve counter have been concurrently updated
			 * while we read the commit counter. This means the
			 * commit counter we read might not match buf->offset
			 * due to concurrent update. We therefore need to retry.
			 */
			goto retry;
		}
		reserve_commit_diff =
		  (buf_trunc(offsets->begin, chan)
		   >> chan->backend.num_subbuf_order)
		  - (commit_count & chan->commit_count_mask);
		if (likely(reserve_commit_diff == 0)) {
			/* Next subbuffer not being written to. */
			if (unlikely(config->mode != RING_BUFFER_OVERWRITE &&
				subbuf_trunc(offsets->begin, chan)
				 - subbuf_trunc((unsigned long)
				     atomic_long_read(&buf->consumed), chan)
				>= chan->backend.buf_size)) {
				/*
				 * We do not overwrite non consumed buffers
				 * and we are full : record is lost.
				 */
				v_inc(config, &buf->records_lost_full);
				return -ENOBUFS;
			} else {
				/*
				 * Next subbuffer not being written to, and we
				 * are either in overwrite mode or the buffer is
				 * not full. It's safe to write in this new
				 * subbuffer.
				 */
			}
		} else {
			/*
			 * Next subbuffer reserve offset does not match the
			 * commit offset, and this did not involve update to the
			 * reserve counter. Drop record in producer-consumer and
			 * overwrite mode.  Caused by either a writer OOPS or
			 * too many nested writes over a reserve/commit pair.
			 */
			v_inc(config, &buf->records_lost_wrap);
			return -EIO;
		}
		offsets->size =
			config->cb.record_header_size(config, chan,
						offsets->begin,
						&offsets->pre_header_padding,
						ctx, client_ctx);
		offsets->size +=
			lib_ring_buffer_align(offsets->begin + offsets->size,
					      ctx->largest_align)
			+ ctx->data_size;
		if (unlikely(subbuf_offset(offsets->begin, chan)
			     + offsets->size > chan->backend.subbuf_size)) {
			/*
			 * Record too big for subbuffers, report error, don't
			 * complete the sub-buffer switch.
			 */
			v_inc(config, &buf->records_lost_big);
			return -ENOSPC;
		} else {
			/*
			 * We just made a successful buffer switch and the
			 * record fits in the new subbuffer. Let's write.
			 */
		}
	} else {
		/*
		 * Record fits in the current buffer and we are not on a switch
		 * boundary. It's safe to write.
		 */
	}
	offsets->end = offsets->begin + offsets->size;

	if (unlikely(subbuf_offset(offsets->end, chan) == 0)) {
		/*
		 * The offset_end will fall at the very beginning of the next
		 * subbuffer.
		 */
		offsets->switch_new_end = 1;	/* For offsets->begin */
	}
	/*
	 * Populate the records lost counters when the space reservation
	 * may cause a sub-buffer switch.
	 */
	if (offsets->switch_new_end || offsets->switch_old_end) {
		ctx->priv.records_lost_full = v_read(config, &buf->records_lost_full);
		ctx->priv.records_lost_wrap = v_read(config, &buf->records_lost_wrap);
		ctx->priv.records_lost_big = v_read(config, &buf->records_lost_big);
	}
	return 0;
}

static struct lttng_kernel_ring_buffer *get_current_buf(struct lttng_kernel_ring_buffer_channel *chan, int cpu)
{
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;

	if (config->alloc == RING_BUFFER_ALLOC_PER_CPU)
		return per_cpu_ptr(chan->backend.percpu_buf, cpu);
	else
		return chan->backend.global_buf;
}

void lib_ring_buffer_lost_event_too_big(struct lttng_kernel_ring_buffer_channel *chan)
{
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	struct lttng_kernel_ring_buffer *buf = get_current_buf(chan, smp_processor_id());

	v_inc(config, &buf->records_lost_big);
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_lost_event_too_big);

/**
 * lib_ring_buffer_reserve_slow - Atomic slot reservation in a buffer.
 * @ctx: ring buffer context.
 *
 * Return : -NOBUFS if not enough space, -ENOSPC if event size too large,
 * -EIO for other errors, else returns 0.
 * It will take care of sub-buffer switching.
 */
int lib_ring_buffer_reserve_slow(struct lttng_kernel_ring_buffer_ctx *ctx,
		void *client_ctx)
{
	struct lttng_kernel_ring_buffer_channel *chan = ctx->priv.chan;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	struct lttng_kernel_ring_buffer *buf;
	struct switch_offsets offsets;
	int ret;

	ctx->priv.buf = buf = get_current_buf(chan, ctx->priv.reserve_cpu);
	offsets.size = 0;

	do {
		ret = lib_ring_buffer_try_reserve_slow(buf, chan, &offsets,
						       ctx, client_ctx);
		if (unlikely(ret))
			return ret;
	} while (unlikely(v_cmpxchg(config, &buf->offset, offsets.old,
				    offsets.end)
			  != offsets.old));

	/*
	 * Atomically update last_tsc. This update races against concurrent
	 * atomic updates, but the race will always cause supplementary full TSC
	 * records, never the opposite (missing a full TSC record when it would
	 * be needed).
	 */
	save_last_tsc(config, buf, ctx->priv.tsc);

	/*
	 * Push the reader if necessary
	 */
	lib_ring_buffer_reserve_push_reader(buf, chan, offsets.end - 1);

	/*
	 * Clear noref flag for this subbuffer.
	 */
	lib_ring_buffer_clear_noref(config, &buf->backend,
				    subbuf_index(offsets.end - 1, chan));

	/*
	 * Switch old subbuffer if needed.
	 */
	if (unlikely(offsets.switch_old_end)) {
		lib_ring_buffer_clear_noref(config, &buf->backend,
					    subbuf_index(offsets.old - 1, chan));
		lib_ring_buffer_switch_old_end(buf, chan, &offsets, ctx);
	}

	/*
	 * Populate new subbuffer.
	 */
	if (unlikely(offsets.switch_new_start))
		lib_ring_buffer_switch_new_start(buf, chan, &offsets, ctx);

	if (unlikely(offsets.switch_new_end))
		lib_ring_buffer_switch_new_end(buf, chan, &offsets, ctx);

	ctx->priv.slot_size = offsets.size;
	ctx->priv.pre_offset = offsets.begin;
	ctx->priv.buf_offset = offsets.begin + offsets.pre_header_padding;
	return 0;
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_reserve_slow);

static
void lib_ring_buffer_vmcore_check_deliver(const struct lttng_kernel_ring_buffer_config *config,
					  struct lttng_kernel_ring_buffer *buf,
				          unsigned long commit_count,
				          unsigned long idx)
{
	if (config->oops == RING_BUFFER_OOPS_CONSISTENCY)
		v_set(config, &buf->commit_hot[idx].seq, commit_count);
}

/*
 * The ring buffer can count events recorded and overwritten per buffer,
 * but it is disabled by default due to its performance overhead.
 */
#ifdef LTTNG_RING_BUFFER_COUNT_EVENTS
static
void deliver_count_events(const struct lttng_kernel_ring_buffer_config *config,
		struct lttng_kernel_ring_buffer *buf,
		unsigned long idx)
{
	v_add(config, subbuffer_get_records_count(config,
			&buf->backend, idx),
		&buf->records_count);
	v_add(config, subbuffer_count_records_overrun(config,
			&buf->backend, idx),
		&buf->records_overrun);
}
#else /* LTTNG_RING_BUFFER_COUNT_EVENTS */
static
void deliver_count_events(const struct lttng_kernel_ring_buffer_config *config,
		struct lttng_kernel_ring_buffer *buf,
		unsigned long idx)
{
}
#endif /* #else LTTNG_RING_BUFFER_COUNT_EVENTS */


void lib_ring_buffer_check_deliver_slow(const struct lttng_kernel_ring_buffer_config *config,
				   struct lttng_kernel_ring_buffer *buf,
			           struct lttng_kernel_ring_buffer_channel *chan,
			           unsigned long offset,
				   unsigned long commit_count,
			           unsigned long idx,
				   const struct lttng_kernel_ring_buffer_ctx *ctx)
{
	unsigned long old_commit_count = commit_count
					 - chan->backend.subbuf_size;

	/*
	 * If we succeeded at updating cc_sb below, we are the subbuffer
	 * writer delivering the subbuffer. Deals with concurrent
	 * updates of the "cc" value without adding a add_return atomic
	 * operation to the fast path.
	 *
	 * We are doing the delivery in two steps:
	 * - First, we cmpxchg() cc_sb to the new value
	 *   old_commit_count + 1. This ensures that we are the only
	 *   subbuffer user successfully filling the subbuffer, but we
	 *   do _not_ set the cc_sb value to "commit_count" yet.
	 *   Therefore, other writers that would wrap around the ring
	 *   buffer and try to start writing to our subbuffer would
	 *   have to drop records, because it would appear as
	 *   non-filled.
	 *   We therefore have exclusive access to the subbuffer control
	 *   structures.  This mutual exclusion with other writers is
	 *   crucially important to perform record overruns count in
	 *   flight recorder mode locklessly.
	 * - When we are ready to release the subbuffer (either for
	 *   reading or for overrun by other writers), we simply set the
	 *   cc_sb value to "commit_count" and perform delivery.
	 *
	 * The subbuffer size is least 2 bytes (minimum size: 1 page).
	 * This guarantees that old_commit_count + 1 != commit_count.
	 */

	/*
	 * Order prior updates to reserve count prior to the
	 * commit_cold cc_sb update.
	 */
	smp_wmb();
	if (likely(v_cmpxchg(config, &buf->commit_cold[idx].cc_sb,
				 old_commit_count, old_commit_count + 1)
		   == old_commit_count)) {
		u64 *ts_end;

		/*
		 * Start of exclusive subbuffer access. We are
		 * guaranteed to be the last writer in this subbuffer
		 * and any other writer trying to access this subbuffer
		 * in this state is required to drop records.
		 *
		 * We can read the ts_end for the current sub-buffer
		 * which has been saved by the very last space
		 * reservation for the current sub-buffer.
		 *
		 * Order increment of commit counter before reading ts_end.
		 */
		smp_mb();
		ts_end = &buf->ts_end[idx];
		deliver_count_events(config, buf, idx);
		config->cb.buffer_end(buf, *ts_end, idx,
				      lib_ring_buffer_get_data_size(config,
								buf,
								idx), ctx);

		/*
		 * Increment the packet counter while we have exclusive
		 * access.
		 */
		subbuffer_inc_packet_count(config, &buf->backend, idx);

		/*
		 * Set noref flag and offset for this subbuffer id.
		 * Contains a memory barrier that ensures counter stores
		 * are ordered before set noref and offset.
		 */
		lib_ring_buffer_set_noref_offset(config, &buf->backend, idx,
						 buf_trunc_val(offset, chan));

		/*
		 * Order set_noref and record counter updates before the
		 * end of subbuffer exclusive access. Orders with
		 * respect to writers coming into the subbuffer after
		 * wrap around, and also order wrt concurrent readers.
		 */
		smp_mb();
		/* End of exclusive subbuffer access */
		v_set(config, &buf->commit_cold[idx].cc_sb,
		      commit_count);
		/*
		 * Order later updates to reserve count after
		 * the commit_cold cc_sb update.
		 */
		smp_wmb();
		lib_ring_buffer_vmcore_check_deliver(config, buf,
						 commit_count, idx);

		/*
		 * RING_BUFFER_WAKEUP_BY_WRITER uses an irq_work to issue
		 * the wakeups.
		 */
		if (config->wakeup == RING_BUFFER_WAKEUP_BY_WRITER
		    && atomic_long_read(&buf->active_readers)
		    && lib_ring_buffer_poll_deliver(config, buf, chan)) {
			irq_work_queue(&buf->wakeup_pending);
			irq_work_queue(&chan->wakeup_pending);
		}

	}
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_check_deliver_slow);

static
int __init init_lib_ring_buffer_frontend(void)
{
	int cpu;

	for_each_possible_cpu(cpu)
		spin_lock_init(&per_cpu(ring_buffer_nohz_lock, cpu));
	return 0;
}

module_init(init_lib_ring_buffer_frontend);

static
void __exit exit_lib_ring_buffer_frontend(void)
{
}

module_exit(exit_lib_ring_buffer_frontend);
