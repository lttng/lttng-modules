/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-ring-buffer-client.h
 *
 * LTTng lib ring buffer client template.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <linux/types.h>
#include <wrapper/vmalloc.h>	/* for wrapper_vmalloc_sync_mappings() */
#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <lttng/tracer.h>

static struct lttng_transport lttng_relay_transport;

struct metadata_packet_header {
	uint32_t magic;			/* 0x75D11D57 */
	uint8_t  uuid[16];		/* Unique Universal Identifier */
	uint32_t checksum;		/* 0 if unused */
	uint32_t content_size;		/* in bits */
	uint32_t packet_size;		/* in bits */
	uint8_t  compression_scheme;	/* 0 if unused */
	uint8_t  encryption_scheme;	/* 0 if unused */
	uint8_t  checksum_scheme;	/* 0 if unused */
	uint8_t  major;			/* CTF spec major version number */
	uint8_t  minor;			/* CTF spec minor version number */
	uint8_t  header_end[0];
};

struct metadata_record_header {
	uint8_t header_end[0];		/* End of header */
};

static const struct lttng_kernel_ring_buffer_config client_config;

static inline
u64 lib_ring_buffer_clock_read(struct lttng_kernel_ring_buffer_channel *chan)
{
	return 0;
}

static inline
size_t record_header_size(const struct lttng_kernel_ring_buffer_config *config,
				 struct lttng_kernel_ring_buffer_channel *chan, size_t offset,
				 size_t *pre_header_padding,
				 struct lttng_kernel_ring_buffer_ctx *ctx,
				 void *client_ctx)
{
	return 0;
}

#include <ringbuffer/api.h>

static u64 client_ring_buffer_clock_read(struct lttng_kernel_ring_buffer_channel *chan)
{
	return 0;
}

static
size_t client_record_header_size(const struct lttng_kernel_ring_buffer_config *config,
				 struct lttng_kernel_ring_buffer_channel *chan, size_t offset,
				 size_t *pre_header_padding,
				 struct lttng_kernel_ring_buffer_ctx *ctx,
				 void *client_ctx)
{
	return 0;
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
	return offsetof(struct metadata_packet_header, header_end);
}

static void client_buffer_begin(struct lttng_kernel_ring_buffer *buf, u64 timestamp,
				unsigned int subbuf_idx)
{
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	struct metadata_packet_header *header =
		(struct metadata_packet_header *)
			lib_ring_buffer_offset_address(&buf->backend,
				subbuf_idx * chan->backend.subbuf_size);
	struct lttng_metadata_cache *metadata_cache =
		channel_get_private(chan);

	header->magic = TSDL_MAGIC_NUMBER;
	memcpy(header->uuid, metadata_cache->uuid.b,
		sizeof(metadata_cache->uuid));
	header->checksum = 0;		/* 0 if unused */
	header->content_size = 0xFFFFFFFF; /* in bits, for debugging */
	header->packet_size = 0xFFFFFFFF;  /* in bits, for debugging */
	header->compression_scheme = 0;	/* 0 if unused */
	header->encryption_scheme = 0;	/* 0 if unused */
	header->checksum_scheme = 0;	/* 0 if unused */

	switch (metadata_cache->session->priv->output_format) {
	case LTTNG_KERNEL_ABI_SESSION_OUTPUT_FORMAT_CTF_1_8:
		header->major = CTF_SPEC_MAJOR;
		header->minor = CTF_SPEC_MINOR;
		break;

	case LTTNG_KERNEL_ABI_SESSION_OUTPUT_FORMAT_CTF_2:
		header->major = CTF2_SPEC_MAJOR;
		header->minor = CTF2_SPEC_MINOR;
		break;

	default:
		printk(KERN_WARNING "LTTng: Unknown session output format");
		break;
	};
}

/*
 * offset is assumed to never be 0 here : never deliver a completely empty
 * subbuffer. data_size is between 1 and subbuf_size.
 */
static void client_buffer_end(struct lttng_kernel_ring_buffer *buf, u64 timestamp,
			      unsigned int subbuf_idx, unsigned long data_size,
			      const struct lttng_kernel_ring_buffer_ctx *ctx)
{
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	struct metadata_packet_header *header =
		(struct metadata_packet_header *)
			lib_ring_buffer_offset_address(&buf->backend,
				subbuf_idx * chan->backend.subbuf_size);
	unsigned long records_lost = 0;

	header->content_size = data_size * CHAR_BIT;		/* in bits */
	header->packet_size = PAGE_ALIGN(data_size) * CHAR_BIT; /* in bits */
	/*
	 * We do not care about the records lost count, because the metadata
	 * channel waits and retry.
	 */
	(void) lib_ring_buffer_get_records_lost_full(&client_config, ctx);
	records_lost += lib_ring_buffer_get_records_lost_wrap(&client_config, ctx);
	records_lost += lib_ring_buffer_get_records_lost_big(&client_config, ctx);
	WARN_ON_ONCE(records_lost != 0);
}

static int client_buffer_create(struct lttng_kernel_ring_buffer *buf, void *priv,
				int cpu, const char *name)
{
	return 0;
}

static void client_buffer_finalize(struct lttng_kernel_ring_buffer *buf, void *priv, int cpu)
{
}

static int client_timestamp_begin(const struct lttng_kernel_ring_buffer_config *config,
		struct lttng_kernel_ring_buffer *buf, uint64_t *timestamp_begin)
{
	return -ENOSYS;
}

static int client_timestamp_end(const struct lttng_kernel_ring_buffer_config *config,
			struct lttng_kernel_ring_buffer *bufb,
			uint64_t *timestamp_end)
{
	return -ENOSYS;
}

static int client_events_discarded(const struct lttng_kernel_ring_buffer_config *config,
			struct lttng_kernel_ring_buffer *bufb,
			uint64_t *events_discarded)
{
	return -ENOSYS;
}

static int client_current_timestamp(const struct lttng_kernel_ring_buffer_config *config,
		struct lttng_kernel_ring_buffer *bufb,
		uint64_t *ts)
{
	return -ENOSYS;
}

static int client_content_size(const struct lttng_kernel_ring_buffer_config *config,
			struct lttng_kernel_ring_buffer *bufb,
			uint64_t *content_size)
{
	return -ENOSYS;
}

static int client_packet_size(const struct lttng_kernel_ring_buffer_config *config,
			struct lttng_kernel_ring_buffer *bufb,
			uint64_t *packet_size)
{
	return -ENOSYS;
}

static int client_stream_id(const struct lttng_kernel_ring_buffer_config *config,
			struct lttng_kernel_ring_buffer *bufb,
			uint64_t *stream_id)
{
	return -ENOSYS;
}

static int client_sequence_number(const struct lttng_kernel_ring_buffer_config *config,
			struct lttng_kernel_ring_buffer *bufb,
			uint64_t *seq)
{
	return -ENOSYS;
}

static
int client_instance_id(const struct lttng_kernel_ring_buffer_config *config,
		struct lttng_kernel_ring_buffer *bufb,
		uint64_t *id)
{
	return -ENOSYS;
}

static const struct lttng_kernel_ring_buffer_config client_config = {
	.cb.ring_buffer_clock_read = client_ring_buffer_clock_read,
	.cb.record_header_size = client_record_header_size,
	.cb.subbuffer_header_size = client_packet_header_size,
	.cb.buffer_begin = client_buffer_begin,
	.cb.buffer_end = client_buffer_end,
	.cb.buffer_create = client_buffer_create,
	.cb.buffer_finalize = client_buffer_finalize,

	.timestamp_bits = 0,
	.alloc = RING_BUFFER_ALLOC_PER_CHANNEL,
	.sync = RING_BUFFER_SYNC_PER_CHANNEL,
	.mode = RING_BUFFER_MODE_TEMPLATE,
	.backend = RING_BUFFER_PAGE,
	.output = RING_BUFFER_OUTPUT_TEMPLATE,
	.oops = RING_BUFFER_OOPS_CONSISTENCY,
	.ipi = RING_BUFFER_IPI_BARRIER,
	.wakeup = RING_BUFFER_WAKEUP_BY_TIMER,
};

static
void release_priv_ops(void *priv_ops)
{
	module_put(THIS_MODULE);
}

static
void lttng_channel_destroy(struct lttng_kernel_ring_buffer_channel *chan)
{
	channel_destroy(chan);
}

static
struct lttng_kernel_ring_buffer_channel *_channel_create(const char *name,
				void *priv, void *buf_addr,
				size_t subbuf_size, size_t num_subbuf,
				unsigned int switch_timer_interval,
				unsigned int read_timer_interval)
{
	struct lttng_kernel_channel_buffer *lttng_chan = priv;
	struct lttng_kernel_ring_buffer_channel *chan;

	chan = channel_create(&client_config, name,
			      lttng_chan->parent.session->priv->metadata_cache, buf_addr,
			      subbuf_size, num_subbuf, switch_timer_interval,
			      read_timer_interval);
	if (chan) {
		/*
		 * Ensure this module is not unloaded before we finish
		 * using lttng_relay_transport.ops.
		 */
		if (!try_module_get(THIS_MODULE)) {
			printk(KERN_WARNING "LTTng: Can't lock transport module.\n");
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
struct lttng_kernel_ring_buffer *lttng_buffer_read_open(struct lttng_kernel_ring_buffer_channel *chan)
{
	struct lttng_kernel_ring_buffer *buf;

	buf = channel_get_ring_buffer(&client_config, chan, 0);
	if (!lib_ring_buffer_open_read(buf))
		return buf;
	return NULL;
}

static
int lttng_buffer_has_read_closed_stream(struct lttng_kernel_ring_buffer_channel *chan)
{
	struct lttng_kernel_ring_buffer *buf;
	int cpu;

	for_each_channel_cpu(cpu, chan) {
		buf = channel_get_ring_buffer(&client_config, chan, cpu);
		if (!atomic_long_read(&buf->active_readers))
			return 1;
	}
	return 0;
}

static
void lttng_buffer_read_close(struct lttng_kernel_ring_buffer *buf)
{
	lib_ring_buffer_release_read(buf);
}

static
int lttng_event_reserve(struct lttng_kernel_ring_buffer_ctx *ctx)
{
	struct lttng_kernel_ring_buffer_channel *chan = ctx->client_priv;
	int ret;

	memset(&ctx->priv, 0, sizeof(ctx->priv));
	ctx->priv.chan = chan;

	ret = lib_ring_buffer_reserve(&client_config, ctx, NULL);
	if (ret)
		return ret;
	lib_ring_buffer_backend_get_pages(&client_config, ctx,
			&ctx->priv.backend_pages);
	return 0;

}

static
void lttng_event_commit(struct lttng_kernel_ring_buffer_ctx *ctx)
{
	lib_ring_buffer_commit(&client_config, ctx);
}

static
void lttng_event_write(struct lttng_kernel_ring_buffer_ctx *ctx, const void *src,
		     size_t len, size_t alignment)
{
	lib_ring_buffer_align_ctx(ctx, alignment);
	lib_ring_buffer_write(&client_config, ctx, src, len);
}

static
void lttng_event_write_from_user(struct lttng_kernel_ring_buffer_ctx *ctx,
			       const void __user *src, size_t len, size_t alignment)
{
	lib_ring_buffer_align_ctx(ctx, alignment);
	lib_ring_buffer_copy_from_user_inatomic(&client_config, ctx, src, len);
}

static
void lttng_event_memset(struct lttng_kernel_ring_buffer_ctx *ctx,
		int c, size_t len)
{
	lib_ring_buffer_memset(&client_config, ctx, c, len);
}

static
void lttng_event_strcpy(struct lttng_kernel_ring_buffer_ctx *ctx, const char *src,
		size_t len)
{
	lib_ring_buffer_strcpy(&client_config, ctx, src, len, '#');
}

static
void lttng_event_pstrcpy_pad(struct lttng_kernel_ring_buffer_ctx *ctx, const char *src,
		size_t len)
{
	lib_ring_buffer_pstrcpy(&client_config, ctx, src, len, '\0');
}

static
size_t lttng_packet_avail_size(struct lttng_kernel_ring_buffer_channel *chan)
{
	unsigned long o_begin;
	struct lttng_kernel_ring_buffer *buf;

	buf = chan->backend.global_buf;	/* Only for global buffer ! */
	o_begin = v_read(&client_config, &buf->offset);
	if (subbuf_offset(o_begin, chan) != 0) {
		return chan->backend.subbuf_size - subbuf_offset(o_begin, chan);
	} else {
		return chan->backend.subbuf_size - subbuf_offset(o_begin, chan)
			- sizeof(struct metadata_packet_header);
	}
}

static
wait_queue_head_t *lttng_get_writer_buf_wait_queue(struct lttng_kernel_ring_buffer_channel *chan, int cpu)
{
	struct lttng_kernel_ring_buffer *buf = channel_get_ring_buffer(&client_config,
					chan, cpu);
	return &buf->write_wait;
}

static
wait_queue_head_t *lttng_get_hp_wait_queue(struct lttng_kernel_ring_buffer_channel *chan)
{
	return &chan->hp_wait;
}

static
int lttng_is_finalized(struct lttng_kernel_ring_buffer_channel *chan)
{
	return lib_ring_buffer_channel_is_finalized(chan);
}

static
int lttng_is_disabled(struct lttng_kernel_ring_buffer_channel *chan)
{
	return lib_ring_buffer_channel_is_disabled(chan);
}

static struct lttng_transport lttng_relay_transport = {
	.name = "relay-" RING_BUFFER_MODE_TEMPLATE_STRING,
	.owner = THIS_MODULE,
	.ops = {
		.priv = __LTTNG_COMPOUND_LITERAL(struct lttng_kernel_channel_buffer_ops_private, {
			.pub = &lttng_relay_transport.ops,
			.channel_create = _channel_create,
			.channel_destroy = lttng_channel_destroy,
			.buffer_read_open = lttng_buffer_read_open,
			.buffer_has_read_closed_stream =
				lttng_buffer_has_read_closed_stream,
			.buffer_read_close = lttng_buffer_read_close,
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
		}),
		.event_reserve = lttng_event_reserve,
		.event_commit = lttng_event_commit,
		.event_write_from_user = lttng_event_write_from_user,
		.event_memset = lttng_event_memset,
		.event_write = lttng_event_write,
		.event_strcpy = lttng_event_strcpy,
		.event_pstrcpy_pad = lttng_event_pstrcpy_pad,
	},
};

static int __init lttng_ring_buffer_client_init(void)
{
	/*
	 * This vmalloc sync all also takes care of the lib ring buffer
	 * vmalloc'd module pages when it is built as a module into LTTng.
	 */
	wrapper_vmalloc_sync_mappings();
	lttng_transport_register(&lttng_relay_transport);
	return 0;
}

module_init(lttng_ring_buffer_client_init);

static void __exit lttng_ring_buffer_client_exit(void)
{
	lttng_transport_unregister(&lttng_relay_transport);
}

module_exit(lttng_ring_buffer_client_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng ring buffer " RING_BUFFER_MODE_TEMPLATE_STRING
		   " client");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
