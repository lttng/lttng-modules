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
#include <lttng/bitfield.h>
#include <wrapper/vmalloc.h>	/* for wrapper_vmalloc_sync_mappings() */
#include <wrapper/trace-clock.h>
#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <lttng/tracer.h>
#include <ringbuffer/frontend_types.h>

#define LTTNG_COMPACT_EVENT_BITS	5
#define LTTNG_COMPACT_TIMESTAMP_BITS	27

static struct lttng_transport lttng_relay_transport;

/*
 * Keep the natural field alignment for _each field_ within this structure if
 * you ever add/remove a field from this header. Packed attribute is not used
 * because gcc generates poor code on at least powerpc and mips. Don't ever
 * let gcc add padding between the structure elements.
 *
 * The guarantee we have with timestamps is that all the events in a
 * packet are included (inclusive) within the begin/end timestamps of
 * the packet. Another guarantee we have is that the "timestamp begin",
 * as well as the event timestamps, are monotonically increasing (never
 * decrease) when moving forward in a stream (physically). But this
 * guarantee does not apply to "timestamp end", because it is sampled at
 * commit time, which is not ordered with respect to space reservation.
 */

struct packet_header {
	/* Trace packet header */
	uint32_t magic;			/*
					 * Trace magic number.
					 * contains endianness information.
					 */
	uint8_t uuid[16];
	uint32_t stream_id;
	uint64_t stream_instance_id;

	struct {
		/* Stream packet context */
		uint64_t timestamp_begin;	/* Cycle count at subbuffer start */
		uint64_t timestamp_end;		/* Cycle count at subbuffer end */
		uint64_t content_size;		/* Size of data in subbuffer */
		uint64_t packet_size;		/* Subbuffer size (include padding) */
		uint64_t packet_seq_num;	/* Packet sequence number */
		unsigned long events_discarded;	/*
						 * Events lost in this subbuffer since
						 * the beginning of the trace.
						 * (may overflow)
						 */
		uint32_t cpu_id;		/* CPU id associated with stream */
		uint8_t header_end;		/* End of header */
	} ctx;
};

struct lttng_client_ctx {
	size_t packet_context_len;
	size_t event_context_len;
};

static inline notrace u64 lib_ring_buffer_clock_read(struct lttng_kernel_ring_buffer_channel *chan)
{
	return trace_clock_read64();
}

static inline
size_t ctx_get_aligned_size(size_t offset, struct lttng_kernel_ctx *ctx,
		size_t ctx_len)
{
	size_t orig_offset = offset;

	if (likely(!ctx))
		return 0;
	offset += lib_ring_buffer_align(offset, ctx->largest_align);
	offset += ctx_len;
	return offset - orig_offset;
}

static inline
void ctx_get_struct_size(struct lttng_kernel_ctx *ctx, size_t *ctx_len,
		struct lttng_kernel_channel_buffer *lttng_chan,
		struct lttng_kernel_ring_buffer_ctx *bufctx)
{
	int i;
	size_t offset = 0;

	if (likely(!ctx)) {
		*ctx_len = 0;
		return;
	}
	for (i = 0; i < ctx->nr_fields; i++) {
		offset += ctx->fields[i].get_size(ctx->fields[i].priv,
				bufctx->probe_ctx, offset);
	}
	*ctx_len = offset;
}

static inline
void ctx_record(struct lttng_kernel_ring_buffer_ctx *bufctx,
		struct lttng_kernel_channel_buffer *lttng_chan,
		struct lttng_kernel_ctx *ctx)
{
	int i;

	if (likely(!ctx))
		return;
	lib_ring_buffer_align_ctx(bufctx, ctx->largest_align);
	for (i = 0; i < ctx->nr_fields; i++)
		ctx->fields[i].record(ctx->fields[i].priv, bufctx->probe_ctx,
				bufctx, lttng_chan);
}

/*
 * record_header_size - Calculate the header size and padding necessary.
 * @config: ring buffer instance configuration
 * @chan: channel
 * @offset: offset in the write buffer
 * @pre_header_padding: padding to add before the header (output)
 * @ctx: reservation context
 *
 * Returns the event header size (including padding).
 *
 * The payload must itself determine its own alignment from the biggest type it
 * contains.
 */
static __inline__
size_t record_header_size(const struct lttng_kernel_ring_buffer_config *config,
				 struct lttng_kernel_ring_buffer_channel *chan, size_t offset,
				 size_t *pre_header_padding,
				 struct lttng_kernel_ring_buffer_ctx *ctx,
				 struct lttng_client_ctx *client_ctx)
{
	struct lttng_kernel_channel_buffer *lttng_chan = channel_get_private(chan);
	size_t orig_offset = offset;
	size_t padding;

	switch (lttng_chan->priv->header_type) {
	case 1:	/* compact */
		padding = lib_ring_buffer_align(offset, lttng_alignof(uint32_t));
		offset += padding;
		if (!(ctx->priv.rflags & (RING_BUFFER_RFLAG_FULL_TIMESTAMP | LTTNG_RFLAG_EXTENDED))) {
			offset += sizeof(uint32_t);	/* id and timestamp */
		} else {
			/* Minimum space taken by LTTNG_COMPACT_EVENT_BITS id */
			offset += (LTTNG_COMPACT_EVENT_BITS + CHAR_BIT - 1) / CHAR_BIT;
			/* Align extended struct on largest member */
			offset += lib_ring_buffer_align(offset, lttng_alignof(uint64_t));
			offset += sizeof(uint32_t);	/* id */
			offset += lib_ring_buffer_align(offset, lttng_alignof(uint64_t));
			offset += sizeof(uint64_t);	/* timestamp */
		}
		break;
	case 2:	/* large */
		padding = lib_ring_buffer_align(offset, lttng_alignof(uint16_t));
		offset += padding;
		offset += sizeof(uint16_t);
		if (!(ctx->priv.rflags & (RING_BUFFER_RFLAG_FULL_TIMESTAMP | LTTNG_RFLAG_EXTENDED))) {
			offset += lib_ring_buffer_align(offset, lttng_alignof(uint32_t));
			offset += sizeof(uint32_t);	/* timestamp */
		} else {
			/* Align extended struct on largest member */
			offset += lib_ring_buffer_align(offset, lttng_alignof(uint64_t));
			offset += sizeof(uint32_t);	/* id */
			offset += lib_ring_buffer_align(offset, lttng_alignof(uint64_t));
			offset += sizeof(uint64_t);	/* timestamp */
		}
		break;
	default:
		padding = 0;
		WARN_ON_ONCE(1);
	}
	offset += ctx_get_aligned_size(offset, lttng_chan->priv->ctx,
			client_ctx->packet_context_len);
	*pre_header_padding = padding;
	return offset - orig_offset;
}

#include <ringbuffer/api.h>

static
void lttng_write_event_header_slow(const struct lttng_kernel_ring_buffer_config *config,
				 struct lttng_kernel_ring_buffer_ctx *ctx,
				 uint32_t event_id);

/*
 * lttng_write_event_header
 *
 * Writes the event header to the offset (already aligned on 32-bits).
 *
 * @config: ring buffer instance configuration
 * @ctx: reservation context
 * @event_id: event ID
 */
static __inline__
void lttng_write_event_header(const struct lttng_kernel_ring_buffer_config *config,
			    struct lttng_kernel_ring_buffer_ctx *ctx,
			    uint32_t event_id)
{
	struct lttng_kernel_channel_buffer *lttng_chan = channel_get_private(ctx->priv.chan);

	if (unlikely(ctx->priv.rflags))
		goto slow_path;

	switch (lttng_chan->priv->header_type) {
	case 1:	/* compact */
	{
		uint32_t id_time = 0;

		bt_bitfield_write(&id_time, uint32_t,
				0,
				LTTNG_COMPACT_EVENT_BITS,
				event_id);
		bt_bitfield_write(&id_time, uint32_t,
				LTTNG_COMPACT_EVENT_BITS,
				LTTNG_COMPACT_TIMESTAMP_BITS,
				ctx->priv.timestamp);
		lib_ring_buffer_write(config, ctx, &id_time, sizeof(id_time));
		break;
	}
	case 2:	/* large */
	{
		uint32_t timestamp = (uint32_t) ctx->priv.timestamp;
		uint16_t id = event_id;

		lib_ring_buffer_write(config, ctx, &id, sizeof(id));
		lib_ring_buffer_align_ctx(ctx, lttng_alignof(uint32_t));
		lib_ring_buffer_write(config, ctx, &timestamp, sizeof(timestamp));
		break;
	}
	default:
		WARN_ON_ONCE(1);
	}

	ctx_record(ctx, lttng_chan, lttng_chan->priv->ctx);
	lib_ring_buffer_align_ctx(ctx, ctx->largest_align);

	return;

slow_path:
	lttng_write_event_header_slow(config, ctx, event_id);
}

static
void lttng_write_event_header_slow(const struct lttng_kernel_ring_buffer_config *config,
				 struct lttng_kernel_ring_buffer_ctx *ctx,
				 uint32_t event_id)
{
	struct lttng_kernel_channel_buffer *lttng_chan = channel_get_private(ctx->priv.chan);

	switch (lttng_chan->priv->header_type) {
	case 1:	/* compact */
		if (!(ctx->priv.rflags & (RING_BUFFER_RFLAG_FULL_TIMESTAMP | LTTNG_RFLAG_EXTENDED))) {
			uint32_t id_time = 0;

			bt_bitfield_write(&id_time, uint32_t,
					0,
					LTTNG_COMPACT_EVENT_BITS,
					event_id);
			bt_bitfield_write(&id_time, uint32_t,
					LTTNG_COMPACT_EVENT_BITS,
					LTTNG_COMPACT_TIMESTAMP_BITS, ctx->priv.timestamp);
			lib_ring_buffer_write(config, ctx, &id_time, sizeof(id_time));
		} else {
			uint8_t id = 0;
			uint64_t timestamp = ctx->priv.timestamp;

			bt_bitfield_write(&id, uint8_t,
					0,
					LTTNG_COMPACT_EVENT_BITS,
					31);
			lib_ring_buffer_write(config, ctx, &id, sizeof(id));
			/* Align extended struct on largest member */
			lib_ring_buffer_align_ctx(ctx, lttng_alignof(uint64_t));
			lib_ring_buffer_write(config, ctx, &event_id, sizeof(event_id));
			lib_ring_buffer_align_ctx(ctx, lttng_alignof(uint64_t));
			lib_ring_buffer_write(config, ctx, &timestamp, sizeof(timestamp));
		}
		break;
	case 2:	/* large */
	{
		if (!(ctx->priv.rflags & (RING_BUFFER_RFLAG_FULL_TIMESTAMP | LTTNG_RFLAG_EXTENDED))) {
			uint32_t timestamp = (uint32_t) ctx->priv.timestamp;
			uint16_t id = event_id;

			lib_ring_buffer_write(config, ctx, &id, sizeof(id));
			lib_ring_buffer_align_ctx(ctx, lttng_alignof(uint32_t));
			lib_ring_buffer_write(config, ctx, &timestamp, sizeof(timestamp));
		} else {
			uint16_t id = 65535;
			uint64_t timestamp = ctx->priv.timestamp;

			lib_ring_buffer_write(config, ctx, &id, sizeof(id));
			/* Align extended struct on largest member */
			lib_ring_buffer_align_ctx(ctx, lttng_alignof(uint64_t));
			lib_ring_buffer_write(config, ctx, &event_id, sizeof(event_id));
			lib_ring_buffer_align_ctx(ctx, lttng_alignof(uint64_t));
			lib_ring_buffer_write(config, ctx, &timestamp, sizeof(timestamp));
		}
		break;
	}
	default:
		WARN_ON_ONCE(1);
	}
	ctx_record(ctx, lttng_chan, lttng_chan->priv->ctx);
	lib_ring_buffer_align_ctx(ctx, ctx->largest_align);
}

static const struct lttng_kernel_ring_buffer_config client_config;

static u64 client_ring_buffer_clock_read(struct lttng_kernel_ring_buffer_channel *chan)
{
	return lib_ring_buffer_clock_read(chan);
}

static
size_t client_record_header_size(const struct lttng_kernel_ring_buffer_config *config,
				 struct lttng_kernel_ring_buffer_channel *chan, size_t offset,
				 size_t *pre_header_padding,
				 struct lttng_kernel_ring_buffer_ctx *ctx,
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
	return offsetof(struct packet_header, ctx.header_end);
}

static void client_buffer_begin(struct lttng_kernel_ring_buffer *buf, u64 timestamp,
				unsigned int subbuf_idx)
{
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	struct packet_header *header =
		(struct packet_header *)
			lib_ring_buffer_offset_address(&buf->backend,
				subbuf_idx * chan->backend.subbuf_size);
	struct lttng_kernel_channel_buffer *lttng_chan = channel_get_private(chan);
	struct lttng_kernel_session *session = lttng_chan->parent.session;

	header->magic = CTF_MAGIC_NUMBER;
	memcpy(header->uuid, session->priv->uuid.b, sizeof(session->priv->uuid));
	header->stream_id = lttng_chan->priv->id;
	header->stream_instance_id = buf->backend.cpu;
	header->ctx.timestamp_begin = timestamp;
	header->ctx.timestamp_end = 0;
	header->ctx.content_size = ~0ULL; /* for debugging */
	header->ctx.packet_size = ~0ULL;
	header->ctx.packet_seq_num = chan->backend.num_subbuf * \
				     buf->backend.buf_cnt[subbuf_idx].seq_cnt + \
				     subbuf_idx;
	header->ctx.events_discarded = 0;
	header->ctx.cpu_id = buf->backend.cpu;
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
	struct packet_header *header =
		(struct packet_header *)
			lib_ring_buffer_offset_address(&buf->backend,
				subbuf_idx * chan->backend.subbuf_size);
	unsigned long records_lost = 0;

	header->ctx.timestamp_end = timestamp;
	header->ctx.content_size =
		(uint64_t) data_size * CHAR_BIT;		/* in bits */
	header->ctx.packet_size =
		(uint64_t) PAGE_ALIGN(data_size) * CHAR_BIT;	/* in bits */
	records_lost += lib_ring_buffer_get_records_lost_full(&client_config, ctx);
	records_lost += lib_ring_buffer_get_records_lost_wrap(&client_config, ctx);
	records_lost += lib_ring_buffer_get_records_lost_big(&client_config, ctx);
	header->ctx.events_discarded = records_lost;
}

static int client_buffer_create(struct lttng_kernel_ring_buffer *buf, void *priv,
				int cpu, const char *name)
{
	return 0;
}

static void client_buffer_finalize(struct lttng_kernel_ring_buffer *buf, void *priv, int cpu)
{
}

static struct packet_header *client_packet_header(
		const struct lttng_kernel_ring_buffer_config *config,
		struct lttng_kernel_ring_buffer *buf)
{
	return lib_ring_buffer_read_offset_address(&buf->backend, 0);
}

static int client_timestamp_begin(const struct lttng_kernel_ring_buffer_config *config,
		struct lttng_kernel_ring_buffer *buf,
		uint64_t *timestamp_begin)
{
	struct packet_header *header = client_packet_header(config, buf);
	*timestamp_begin = header->ctx.timestamp_begin;

	return 0;
}

static int client_timestamp_end(const struct lttng_kernel_ring_buffer_config *config,
			struct lttng_kernel_ring_buffer *buf,
			uint64_t *timestamp_end)
{
	struct packet_header *header = client_packet_header(config, buf);
	*timestamp_end = header->ctx.timestamp_end;

	return 0;
}

static int client_events_discarded(const struct lttng_kernel_ring_buffer_config *config,
			struct lttng_kernel_ring_buffer *buf,
			uint64_t *events_discarded)
{
	struct packet_header *header = client_packet_header(config, buf);
	*events_discarded = header->ctx.events_discarded;

	return 0;
}

static int client_content_size(const struct lttng_kernel_ring_buffer_config *config,
			struct lttng_kernel_ring_buffer *buf,
			uint64_t *content_size)
{
	struct packet_header *header = client_packet_header(config, buf);
	*content_size = header->ctx.content_size;

	return 0;
}

static int client_packet_size(const struct lttng_kernel_ring_buffer_config *config,
			struct lttng_kernel_ring_buffer *buf,
			uint64_t *packet_size)
{
	struct packet_header *header = client_packet_header(config, buf);
	*packet_size = header->ctx.packet_size;

	return 0;
}

static int client_stream_id(const struct lttng_kernel_ring_buffer_config *config,
			struct lttng_kernel_ring_buffer *buf,
			uint64_t *stream_id)
{
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	struct lttng_kernel_channel_buffer *lttng_chan = channel_get_private(chan);

	*stream_id = lttng_chan->priv->id;
	return 0;
}

static int client_current_timestamp(const struct lttng_kernel_ring_buffer_config *config,
		struct lttng_kernel_ring_buffer *bufb,
		uint64_t *ts)
{
	*ts = config->cb.ring_buffer_clock_read(bufb->backend.chan);

	return 0;
}

static int client_sequence_number(const struct lttng_kernel_ring_buffer_config *config,
			struct lttng_kernel_ring_buffer *buf,
			uint64_t *seq)
{
	struct packet_header *header = client_packet_header(config, buf);

	*seq = header->ctx.packet_seq_num;

	return 0;
}

static
int client_instance_id(const struct lttng_kernel_ring_buffer_config *config,
		struct lttng_kernel_ring_buffer *buf,
		uint64_t *id)
{
	*id = buf->backend.cpu;

	return 0;
}


static
int client_user_packet_initialize(const struct lttng_kernel_ring_buffer_config *config,
		struct lttng_kernel_ring_buffer *bufb,
		void __user *packet,
		u64 timestamp_begin,
		u64 timestamp_end,
		u64 sequence_number,
		u64 *packet_length,
		u64 *packet_length_padded)
{
	struct lttng_kernel_channel_buffer *lttng_chan = channel_get_private(bufb->backend.chan);
	struct lttng_kernel_ring_buffer_channel *chan = bufb->backend.chan;
	struct packet_header packet_header = {};
	int ret;

	if (!chan)
		return -EINVAL;

	/*
	 * See client_buffer_begin()
	 */
	packet_header.magic = CTF_MAGIC_NUMBER;
	packet_header.stream_id = lttng_chan->priv->id;
	packet_header.stream_instance_id = bufb->backend.cpu;

	packet_header.ctx.content_size = client_packet_header_size() * CHAR_BIT;
	packet_header.ctx.packet_size = client_packet_header_size() * CHAR_BIT;
	packet_header.ctx.cpu_id = bufb->backend.cpu;
	packet_header.ctx.timestamp_begin = timestamp_begin;
	packet_header.ctx.timestamp_end = timestamp_end;
	packet_header.ctx.packet_seq_num = sequence_number;

	ret = copy_to_user(packet, &packet_header, sizeof(struct packet_header));
	if (ret)
		return -EFAULT;

	*packet_length = client_packet_header_size();
	*packet_length_padded = client_packet_header_size();
	return ret;
}

static const struct lttng_kernel_ring_buffer_config client_config = {
	.cb.ring_buffer_clock_read = client_ring_buffer_clock_read,
	.cb.record_header_size = client_record_header_size,
	.cb.subbuffer_header_size = client_packet_header_size,
	.cb.buffer_begin = client_buffer_begin,
	.cb.buffer_end = client_buffer_end,
	.cb.buffer_create = client_buffer_create,
	.cb.buffer_finalize = client_buffer_finalize,

	.timestamp_bits = LTTNG_COMPACT_TIMESTAMP_BITS,
	.alloc = RING_BUFFER_ALLOC_PER_CPU,
	.sync = RING_BUFFER_SYNC_PER_CPU,
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

	chan = channel_create(&client_config, name, lttng_chan, buf_addr,
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
	int cpu;

	for_each_channel_cpu(cpu, chan) {
		buf = channel_get_ring_buffer(&client_config, chan, cpu);
		if (!lib_ring_buffer_open_read(buf))
			return buf;
	}
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
	struct lttng_kernel_event_recorder *event_recorder = ctx->client_priv;
	struct lttng_kernel_channel_buffer *lttng_chan = event_recorder->chan;
	struct lttng_client_ctx client_ctx;
	int ret, cpu;
	uint32_t event_id;

	cpu = lib_ring_buffer_get_cpu(&client_config);
	if (unlikely(cpu < 0))
		return -EPERM;
	event_id = (uint32_t)event_recorder->priv->parent.id;
	memset(&ctx->priv, 0, sizeof(ctx->priv));
	ctx->priv.chan = lttng_chan->priv->rb_chan;
	ctx->priv.reserve_cpu = cpu;

	/* Compute internal size of context structures. */
	ctx_get_struct_size(lttng_chan->priv->ctx, &client_ctx.packet_context_len, lttng_chan, ctx);

	switch (lttng_chan->priv->header_type) {
	case 1:	/* compact */
		if (event_id > 30)
			ctx->priv.rflags |= LTTNG_RFLAG_EXTENDED;
		break;
	case 2:	/* large */
		if (event_id > 65534)
			ctx->priv.rflags |= LTTNG_RFLAG_EXTENDED;
		break;
	default:
		WARN_ON_ONCE(1);
	}

	ret = lib_ring_buffer_reserve(&client_config, ctx, &client_ctx);
	if (unlikely(ret))
		goto put;
	lib_ring_buffer_backend_get_pages(&client_config, ctx,
			&ctx->priv.backend_pages);
	lttng_write_event_header(&client_config, ctx, event_id);
	return 0;
put:
	lib_ring_buffer_put_cpu(&client_config);
	return ret;
}

static
void lttng_event_commit(struct lttng_kernel_ring_buffer_ctx *ctx)
{
	lib_ring_buffer_commit(&client_config, ctx);
	lib_ring_buffer_put_cpu(&client_config);
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
void lttng_event_strcpy_from_user(struct lttng_kernel_ring_buffer_ctx *ctx,
		const char __user *src, size_t len)
{
	lib_ring_buffer_strcpy_from_user_inatomic(&client_config, ctx, src,
			len, '#');
}

static
void lttng_event_pstrcpy_pad(struct lttng_kernel_ring_buffer_ctx *ctx, const char *src,
		size_t len)
{
	lib_ring_buffer_pstrcpy(&client_config, ctx, src, len, '\0');
}

static
void lttng_event_pstrcpy_pad_from_user(struct lttng_kernel_ring_buffer_ctx *ctx, const char __user *src,
		size_t len)
{
	lib_ring_buffer_pstrcpy_from_user_inatomic(&client_config, ctx, src, len, '\0');
}

static
void lttng_channel_buffer_lost_event_too_big(struct lttng_kernel_channel_buffer *lttng_chan)
{
	lib_ring_buffer_lost_event_too_big(lttng_chan->priv->rb_chan);
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
			.packet_avail_size = NULL,	/* Would be racy anyway */
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
			.user_packet_initialize = client_user_packet_initialize,
		}),
		.event_reserve = lttng_event_reserve,
		.event_commit = lttng_event_commit,
		.event_write = lttng_event_write,
		.event_write_from_user = lttng_event_write_from_user,
		.event_memset = lttng_event_memset,
		.event_strcpy = lttng_event_strcpy,
		.event_strcpy_from_user = lttng_event_strcpy_from_user,
		.event_pstrcpy_pad = lttng_event_pstrcpy_pad,
		.event_pstrcpy_pad_from_user = lttng_event_pstrcpy_pad_from_user,
		.lost_event_too_big = lttng_channel_buffer_lost_event_too_big,
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
