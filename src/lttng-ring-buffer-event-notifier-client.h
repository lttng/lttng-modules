/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-ring-buffer-event-notifier-client.h
 *
 * LTTng lib ring buffer event notifier client template.
 *
 * Copyright (C) 2010-2020 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/limits.h>
#include <linux/module.h>
#include <linux/types.h>
#include <wrapper/vmalloc.h>	/* for wrapper_vmalloc_sync_mappings() */
#include <lttng/abi.h>
#include <lttng/events.h>
#include <lttng/tracer.h>

static struct lttng_transport lttng_relay_transport;

struct event_notifier_packet_header {
	uint8_t  header_end[0];
};

struct event_notifier_record_header {
	uint32_t payload_len;		/* in bytes */
	uint8_t header_end[0];		/* End of header */
};

static const struct lib_ring_buffer_config client_config;

static inline
u64 lib_ring_buffer_clock_read(struct channel *chan)
{
	return 0;
}

static inline
size_t record_header_size(const struct lib_ring_buffer_config *config,
				 struct channel *chan, size_t offset,
				 size_t *pre_header_padding,
				 struct lib_ring_buffer_ctx *ctx,
				 void *client_ctx)
{
	size_t orig_offset = offset;
	size_t padding;

	padding = lib_ring_buffer_align(offset, lttng_alignof(uint32_t));
	offset += padding;

	offset += sizeof(uint32_t);

	*pre_header_padding = padding;

	return offset - orig_offset;
}

#include <ringbuffer/api.h>

static u64 client_ring_buffer_clock_read(struct channel *chan)
{
	return 0;
}

static
size_t client_record_header_size(const struct lib_ring_buffer_config *config,
				 struct channel *chan, size_t offset,
				 size_t *pre_header_padding,
				 struct lib_ring_buffer_ctx *ctx,
				 void *client_ctx)
{
	return record_header_size(config, chan, offset,
				  pre_header_padding, ctx, client_ctx);
}

/**
 * client_packet_header_size - called on buffer-switch to a new sub-buffer
 *
 * Return header size without padding after the structure. Don't use packed
 * structure because gcc generates inefficient code on some architectures
 * (powerpc, mips..)
 */
static size_t client_packet_header_size(void)
{
	return offsetof(struct event_notifier_packet_header, header_end);
}

static void client_buffer_begin(struct lib_ring_buffer *buf, u64 tsc,
				unsigned int subbuf_idx)
{
}

/*
 * offset is assumed to never be 0 here : never deliver a completely empty
 * subbuffer. data_size is between 1 and subbuf_size.
 */
static void client_buffer_end(struct lib_ring_buffer *buf, u64 tsc,
			      unsigned int subbuf_idx, unsigned long data_size)
{
}

static int client_buffer_create(struct lib_ring_buffer *buf, void *priv,
				int cpu, const char *name)
{
	return 0;
}

static void client_buffer_finalize(struct lib_ring_buffer *buf, void *priv, int cpu)
{
}

static int client_timestamp_begin(const struct lib_ring_buffer_config *config,
		struct lib_ring_buffer *buf, uint64_t *timestamp_begin)
{
	return -ENOSYS;
}

static int client_timestamp_end(const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *timestamp_end)
{
	return -ENOSYS;
}

static int client_events_discarded(const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *events_discarded)
{
	return -ENOSYS;
}

static int client_current_timestamp(const struct lib_ring_buffer_config *config,
		struct lib_ring_buffer *bufb,
		uint64_t *ts)
{
	return -ENOSYS;
}

static int client_content_size(const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *content_size)
{
	return -ENOSYS;
}

static int client_packet_size(const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *packet_size)
{
	return -ENOSYS;
}

static int client_stream_id(const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *stream_id)
{
	return -ENOSYS;
}

static int client_sequence_number(const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *seq)
{
	return -ENOSYS;
}

static
int client_instance_id(const struct lib_ring_buffer_config *config,
		struct lib_ring_buffer *bufb,
		uint64_t *id)
{
	return -ENOSYS;
}

static void client_record_get(const struct lib_ring_buffer_config *config,
	struct channel *chan, struct lib_ring_buffer *buf,
	size_t offset, size_t *header_len,
	size_t *payload_len, u64 *timestamp)
{
	struct event_notifier_record_header header;
	int ret;

	ret = lib_ring_buffer_read(&buf->backend, offset, &header,
			offsetof(struct event_notifier_record_header, header_end));
	CHAN_WARN_ON(chan, ret != offsetof(struct event_notifier_record_header, header_end));
	*header_len = offsetof(struct event_notifier_record_header, header_end);
	*payload_len = header.payload_len;
	*timestamp = 0;
}

static const struct lib_ring_buffer_config client_config = {
	.cb.ring_buffer_clock_read = client_ring_buffer_clock_read,
	.cb.record_header_size = client_record_header_size,
	.cb.subbuffer_header_size = client_packet_header_size,
	.cb.buffer_begin = client_buffer_begin,
	.cb.buffer_end = client_buffer_end,
	.cb.buffer_create = client_buffer_create,
	.cb.buffer_finalize = client_buffer_finalize,
	.cb.record_get = client_record_get,

	.tsc_bits = 0,
	.alloc = RING_BUFFER_ALLOC_GLOBAL,
	.sync = RING_BUFFER_SYNC_GLOBAL,
	.mode = RING_BUFFER_MODE_TEMPLATE,
	.backend = RING_BUFFER_PAGE,
	.output = RING_BUFFER_OUTPUT_TEMPLATE,
	.oops = RING_BUFFER_OOPS_CONSISTENCY,
	.ipi = RING_BUFFER_NO_IPI_BARRIER,
	.wakeup = RING_BUFFER_WAKEUP_BY_WRITER,
};

static
void release_priv_ops(void *priv_ops)
{
	module_put(THIS_MODULE);
}

static
void lttng_channel_destroy(struct channel *chan)
{
	channel_destroy(chan);
}

static
struct channel *_channel_create(const char *name,
				void *priv, void *buf_addr,
				size_t subbuf_size, size_t num_subbuf,
				unsigned int switch_timer_interval,
				unsigned int read_timer_interval)
{
	struct lttng_event_notifier_group *event_notifier_group = priv;
	struct channel *chan;

	chan = channel_create(&client_config, name,
			      event_notifier_group, buf_addr,
			      subbuf_size, num_subbuf, switch_timer_interval,
			      read_timer_interval);
	if (chan) {
		/*
		 * Ensure this module is not unloaded before we finish
		 * using lttng_relay_transport.ops.
		 */
		if (!try_module_get(THIS_MODULE)) {
			printk(KERN_WARNING "LTTng: Can't lock event_notifier transport module.\n");
			goto error;
		}
		chan->backend.priv_ops = &lttng_relay_transport.ops;
		chan->backend.release_priv_ops = release_priv_ops;
	}
	return chan;

error:
	lttng_channel_destroy(chan);
	return NULL;
}

static
struct lib_ring_buffer *lttng_buffer_read_open(struct channel *chan)
{
	struct lib_ring_buffer *buf;

	buf = channel_get_ring_buffer(&client_config, chan, 0);
	if (!lib_ring_buffer_open_read(buf))
		return buf;
	return NULL;
}

static
int lttng_buffer_has_read_closed_stream(struct channel *chan)
{
	struct lib_ring_buffer *buf;
	int cpu;

	for_each_channel_cpu(cpu, chan) {
		buf = channel_get_ring_buffer(&client_config, chan, cpu);
		if (!atomic_long_read(&buf->active_readers))
			return 1;
	}
	return 0;
}

static
void lttng_buffer_read_close(struct lib_ring_buffer *buf)
{
	lib_ring_buffer_release_read(buf);
}

static
void lttng_write_event_notifier_header(const struct lib_ring_buffer_config *config,
			    struct lib_ring_buffer_ctx *ctx)
{
	uint32_t data_size;

	WARN_ON_ONCE(ctx->data_size > U32_MAX);

	data_size = (uint32_t) ctx->data_size;

	lib_ring_buffer_write(config, ctx, &data_size, sizeof(data_size));

	lib_ring_buffer_align_ctx(ctx, ctx->largest_align);
}

static
int lttng_event_reserve(struct lib_ring_buffer_ctx *ctx, uint32_t event_id)
{
	int ret;

	ret = lib_ring_buffer_reserve(&client_config, ctx, NULL);
	if (ret)
		return ret;
	lib_ring_buffer_backend_get_pages(&client_config, ctx,
			&ctx->backend_pages);

	lttng_write_event_notifier_header(&client_config, ctx);
	return 0;
}

static
void lttng_event_commit(struct lib_ring_buffer_ctx *ctx)
{
	lib_ring_buffer_commit(&client_config, ctx);
}

static
void lttng_event_write(struct lib_ring_buffer_ctx *ctx, const void *src,
		     size_t len)
{
	lib_ring_buffer_write(&client_config, ctx, src, len);
}

static
void lttng_event_write_from_user(struct lib_ring_buffer_ctx *ctx,
			       const void __user *src, size_t len)
{
	lib_ring_buffer_copy_from_user_inatomic(&client_config, ctx, src, len);
}

static
void lttng_event_memset(struct lib_ring_buffer_ctx *ctx,
		int c, size_t len)
{
	lib_ring_buffer_memset(&client_config, ctx, c, len);
}

static
void lttng_event_strcpy(struct lib_ring_buffer_ctx *ctx, const char *src,
		size_t len)
{
	lib_ring_buffer_strcpy(&client_config, ctx, src, len, '#');
}

static
size_t lttng_packet_avail_size(struct channel *chan)
{
	unsigned long o_begin;
	struct lib_ring_buffer *buf;

	buf = chan->backend.buf;	/* Only for global buffer ! */
	o_begin = v_read(&client_config, &buf->offset);
	if (subbuf_offset(o_begin, chan) != 0) {
		return chan->backend.subbuf_size - subbuf_offset(o_begin, chan);
	} else {
		return chan->backend.subbuf_size - subbuf_offset(o_begin, chan)
			- sizeof(struct event_notifier_packet_header);
	}
}

static
wait_queue_head_t *lttng_get_writer_buf_wait_queue(struct channel *chan, int cpu)
{
	struct lib_ring_buffer *buf = channel_get_ring_buffer(&client_config,
					chan, cpu);
	return &buf->write_wait;
}

static
wait_queue_head_t *lttng_get_hp_wait_queue(struct channel *chan)
{
	return &chan->hp_wait;
}

static
int lttng_is_finalized(struct channel *chan)
{
	return lib_ring_buffer_channel_is_finalized(chan);
}

static
int lttng_is_disabled(struct channel *chan)
{
	return lib_ring_buffer_channel_is_disabled(chan);
}

static struct lttng_transport lttng_relay_transport = {
	.name = "relay-" RING_BUFFER_MODE_TEMPLATE_STRING,
	.owner = THIS_MODULE,
	.ops = {
		.channel_create = _channel_create,
		.channel_destroy = lttng_channel_destroy,
		.buffer_read_open = lttng_buffer_read_open,
		.buffer_has_read_closed_stream =
			lttng_buffer_has_read_closed_stream,
		.buffer_read_close = lttng_buffer_read_close,
		.event_reserve = lttng_event_reserve,
		.event_commit = lttng_event_commit,
		.event_write_from_user = lttng_event_write_from_user,
		.event_memset = lttng_event_memset,
		.event_write = lttng_event_write,
		.event_strcpy = lttng_event_strcpy,
		.packet_avail_size = lttng_packet_avail_size,
		.get_writer_buf_wait_queue = lttng_get_writer_buf_wait_queue,
		.get_hp_wait_queue = lttng_get_hp_wait_queue,
		.is_finalized = lttng_is_finalized,
		.is_disabled = lttng_is_disabled,
		.timestamp_begin = client_timestamp_begin,
		.timestamp_end = client_timestamp_end,
		.events_discarded = client_events_discarded,
		.content_size = client_content_size,
		.packet_size = client_packet_size,
		.stream_id = client_stream_id,
		.current_timestamp = client_current_timestamp,
		.sequence_number = client_sequence_number,
		.instance_id = client_instance_id,
	},
};

static int __init lttng_ring_buffer_event_notifier_client_init(void)
{
	/*
	 * This vmalloc sync all also takes care of the lib ring buffer
	 * vmalloc'd module pages when it is built as a module into LTTng.
	 */
	wrapper_vmalloc_sync_mappings();
	lttng_transport_register(&lttng_relay_transport);
	return 0;
}

module_init(lttng_ring_buffer_event_notifier_client_init);

static void __exit lttng_ring_buffer_event_notifier_client_exit(void)
{
	lttng_transport_unregister(&lttng_relay_transport);
}

module_exit(lttng_ring_buffer_event_notifier_client_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng ring buffer " RING_BUFFER_MODE_TEMPLATE_STRING
		   " client");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
