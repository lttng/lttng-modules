/*
 * ltt-ring-buffer-client.h
 *
 * Copyright (C) 2010 - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * LTTng lib ring buffer client template.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include "ltt-tracer.h"

struct ring_buffer_priv {
	struct dentry *dentry;
}

struct channel_priv {
	struct ltt_trace *trace;
	struct ring_buffer_priv *buf;
};

static const struct lib_ring_buffer_config client_config;

static u64 client_ring_buffer_clock_read(struct channel *chan)
{
	return lib_ring_buffer_clock_read(chan);
}

size_t client_record_header_size(const struct lib_ring_buffer_config *config,
				 struct channel *chan, size_t offset,
				 size_t data_size,
				 size_t *pre_header_padding,
				 unsigned int rflags,
				 struct lib_ring_buffer_ctx *ctx)
{
	return record_header_size(config, chan, offset, data_size,
				  pre_header_padding, rflags, ctx);
}

/**
 * client_subbuffer_header_size - called on buffer-switch to a new sub-buffer
 *
 * Return header size without padding after the structure. Don't use packed
 * structure because gcc generates inefficient code on some architectures
 * (powerpc, mips..)
 */
static size_t client_subbuffer_header_size(void)
{
	return offsetof(struct subbuffer_header, header_end);
}

static void client_buffer_begin(struct lib_ring_buffer *buf, u64 tsc,
				unsigned int subbuf_idx)
{
	struct channel *chan = buf->backend.chan;
	struct subbuffer_header *header =
		(struct subbuffer_header *)
			lib_ring_buffer_offset_address(&buf->backend,
				subbuf_idx * chan->backend.subbuf_size);

	header->cycle_count_begin = tsc;
	header->data_size = 0xFFFFFFFF; /* for debugging */
	write_trace_header(chan->backend.priv, header);
}

/*
 * offset is assumed to never be 0 here : never deliver a completely empty
 * subbuffer. data_size is between 1 and subbuf_size.
 */
static void client_buffer_end(struct lib_ring_buffer *buf, u64 tsc,
			      unsigned int subbuf_idx, unsigned long data_size)
{
	struct channel *chan = buf->backend.chan;
	struct subbuffer_header *header =
		(struct subbuffer_header *)
			lib_ring_buffer_offset_address(&buf->backend,
				subbuf_idx * chan->backend.subbuf_size);
	unsigned long records_lost = 0;

	header->data_size = data_size;
	header->subbuf_size = PAGE_ALIGN(data_size);
	header->cycle_count_end = tsc;
	records_lost += lib_ring_buffer_get_records_lost_full(&client_config, buf);
	records_lost += lib_ring_buffer_get_records_lost_wrap(&client_config, buf);
	records_lost += lib_ring_buffer_get_records_lost_big(&client_config, buf);
	header->events_lost = records_lost;
	header->subbuf_corrupt = 0;	/* deprecated */
}

static int client_buffer_create(struct lib_ring_buffer *buf, void *priv,
				int cpu, const char *name)
{
	struct channel_priv *chan_priv = priv;
	struct ring_buffer_priv *buf_priv;
	struct dentry *trace_dentry;
	char *tmpname;
	int ret = 0;

	if (client_config.alloc == RING_BUFFER_ALLOC_PER_CPU)
		buf_priv = per_cpu_ptr(chan_priv->buf, cpu);
	else
		buf_priv = chan_priv->buf;

	tmpname = kzalloc(NAME_MAX + 1, GFP_KERNEL);
	if (!tmpname) {
		ret = -ENOMEM;
		goto end;
	}

	snprintf(tmpname, NAME_MAX, "%s%s_%d",
		 (client_config.mode == RING_BUFFER_OVERWRITE) ? "flight-" : "",
		 name, cpu);

	trace_dentry = chan_priv->trace->dentry.trace_root;
	buf_priv->dentry = debugfs_create_file(tmpname, S_IRUSR, trace_dentry,
					       buf,
					       &lib_ring_buffer_file_operations);
	if (!buf_priv->dentry) {
		ret = -ENOMEM;
		goto free_name;
	}
free_name:
	kfree(tmpname);
end:
	return ret;
}

static void client_buffer_finalize(struct lib_ring_buffer *buf, void *priv, int cpu)
{
	struct channel_priv *chan_priv = priv;
	struct lib_ring_buffer_priv *buf_priv;

	if (client_config.alloc == RING_BUFFER_ALLOC_PER_CPU)
		buf_priv = per_cpu_ptr(chan_priv->buf, cpu);
	else
		buf_priv = chan_priv->buf;

	debugfs_remove(buf_priv->dentry);
}

static const struct lib_ring_buffer_config client_config = {
	.cb.ring_buffer_clock_read = client_ring_buffer_clock_read,
	.cb.record_header_size = client_record_header_size,
	.cb.subbuffer_header_size = client_subbuffer_header_size,
	.cb.buffer_begin = client_buffer_begin,
	.cb.buffer_end = client_buffer_end,
	.cb.buffer_create = client_buffer_create,
	.cb.buffer_finalize = client_buffer_finalize,

	.tsc_bits = 32,
	.alloc = RING_BUFFER_ALLOC_PER_CPU,
	.sync = RING_BUFFER_SYNC_PER_CPU,
	.mode = RING_BUFFER_MODE_TEMPLATE,
#ifdef RING_BUFFER_ALIGN
	.align = RING_BUFFER_NATURAL,
#else
	.align = RING_BUFFER_PACKED,
#endif
	.backend = RING_BUFFER_PAGE,
	.output = RING_BUFFER_SPLICE,
	.oops = RING_BUFFER_OOPS_CONSISTENCY,
	.ipi = RING_BUFFER_IPI_BARRIER,
	.wakeup = RING_BUFFER_WAKEUP_BY_TIMER,
};

struct channel *ltt_channel_create(const char *name, struct ltt_trace *trace,
				   void *buf_addr,
				   size_t subbuf_size, size_t num_subbuf,
				   unsigned int switch_timer_interval,
				   unsigned int read_timer_interval)
{
	struct channel *chan;
	struct chan_priv *chan_priv;

	chan_priv = kzalloc(sizeof(struct chan_priv), GFP_KERNEL);
	if (!chan_priv)
		return NULL;
	if (client_config.alloc == RING_BUFFER_ALLOC_PER_CPU) {
		chan_priv->buf = alloc_percpu(struct lib_ring_buffer_priv);
		memset(chan_priv->buf, 0, sizeof(*chan_priv->buf));
	} else
		chan_priv->buf = kzalloc(sizeof(*chan_priv->buf), GFP_KERNEL)
	if (!channel_priv->buf)
		goto free_chan_priv;
	chan_priv->trace = trace;
	chan = channel_create(&client_config, name, chan_priv, buf_addr,
			      subbuf_size, num_subbuf, switch_timer_interval,
			      read_timer_interval);
	if (!chan)
		goto free_buf_priv;
	return chan;

free_buf_priv:
	if (client_config.alloc == RING_BUFFER_ALLOC_PER_CPU)
		free_percpu(chan_priv->buf);
	else
		kfree(chan_priv->buf);
free_chan_priv:
	kfree(chan_priv);
	return NULL;
}

void ltt_channel_destroy(struct channel *chan)
{
	struct chan_priv *chan_priv = channel_get_private(chan);

	channel_destroy(chan);
	if (client_config.alloc == RING_BUFFER_ALLOC_PER_CPU)
		free_percpu(chan_priv->buf);
	else
		kfree(chan_priv->buf);
	kfree(chan_priv);
}

static void ltt_relay_remove_dirs(struct ltt_trace *trace)
{
	debugfs_remove(trace->dentry.trace_root);
}

static int ltt_relay_create_dirs(struct ltt_trace *new_trace)
{
	struct dentry *ltt_root_dentry;
	int ret;

	ltt_root_dentry = get_ltt_root();
	if (!ltt_root_dentry)
		return ENOENT;

	new_trace->dentry.trace_root = debugfs_create_dir(new_trace->trace_name,
							  ltt_root_dentry);
	put_ltt_root();
	if (new_trace->dentry.trace_root == NULL) {
		printk(KERN_ERR "LTT : Trace directory name %s already taken\n",
		       new_trace->trace_name);
		return EEXIST;
	}
	return 0;
}
static struct ltt_transport ltt_relay_transport = {
	.name = "relay-" RING_BUFFER_MODE_TEMPLATE_STRING,
	.owner = THIS_MODULE,
	.ops = {
		.create_dirs = ltt_relay_create_dirs,
		.remove_dirs = ltt_relay_remove_dirs,
	},
};

static int __init ltt_ring_buffer_client_init(void)
{
	printk(KERN_INFO "LTT : ltt ring buffer client init\n");
	ltt_transport_register(&ltt_relay_transport);
	return 0;
}

static void __exit ltt_ring_buffer_client_exit(void)
{
	printk(KERN_INFO "LTT : ltt ring buffer client exit\n");
	ltt_transport_unregister(&ltt_relay_transport);
}

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("LTTng ring buffer " RING_BUFFER_MODE_TEMPLATE_STRING
		   " client");
