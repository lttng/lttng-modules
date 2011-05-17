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
#include <linux/types.h>
#include "wrapper/vmalloc.h"	/* for wrapper_vmalloc_sync_all() */
#include "wrapper/trace-clock.h"
#include "ltt-events.h"
#include "ltt-tracer.h"

/*
 * Keep the natural field alignment for _each field_ within this structure if
 * you ever add/remove a field from this header. Packed attribute is not used
 * because gcc generates poor code on at least powerpc and mips. Don't ever
 * let gcc add padding between the structure elements.
 */
struct packet_header {
	uint32_t magic;			/*
					 * Trace magic number.
					 * contains endianness information.
					 */
	uint8_t uuid[16];
	uint32_t stream_id;
	uint64_t timestamp_begin;	/* Cycle count at subbuffer start */
	uint64_t timestamp_end;	/* Cycle count at subbuffer end */
	uint32_t content_size;		/* Size of data in subbuffer */
	uint32_t packet_size;		/* Subbuffer size (include padding) */
	uint32_t events_lost;		/*
					 * Events lost in this subbuffer since
					 * the beginning of the trace.
					 * (may overflow)
					 */
#if 0
	uint64_t start_time_sec;	/* NTP-corrected start time */
	uint64_t start_time_usec;
	uint64_t start_freq;		/*
					 * Frequency at trace start,
				 * used all along the trace.
					 */
	uint32_t freq_scale;		/* Frequency scaling (divisor) */
#endif //0
	uint8_t header_end[0];		/* End of header */
};


static inline notrace u64 lib_ring_buffer_clock_read(struct channel *chan)
{
	return trace_clock_read64();
}

/*
 * record_header_size - Calculate the header size and padding necessary.
 * @config: ring buffer instance configuration
 * @chan: channel
 * @offset: offset in the write buffer
 * @data_size: size of the payload
 * @pre_header_padding: padding to add before the header (output)
 * @rflags: reservation flags
 * @ctx: reservation context
 *
 * Returns the event header size (including padding).
 *
 * Important note :
 * The event header must be 32-bits. The total offset calculated here :
 *
 * Alignment of header struct on 32 bits (min arch size, header size)
 * + sizeof(header struct)  (32-bits)
 * + (opt) u16 (ext. event id)
 * + (opt) u16 (event_size)
 *             (if event_size == LTT_MAX_SMALL_SIZE, has ext. event size)
 * + (opt) u32 (ext. event size)
 * + (opt) u64 full TSC (aligned on min(64-bits, arch size))
 *
 * The payload must itself determine its own alignment from the biggest type it
 * contains.
 */
static __inline__
unsigned char record_header_size(const struct lib_ring_buffer_config *config,
				 struct channel *chan, size_t offset,
				 size_t data_size, size_t *pre_header_padding,
				 unsigned int rflags,
				 struct lib_ring_buffer_ctx *ctx)
{
	size_t orig_offset = offset;
	size_t padding;

	BUILD_BUG_ON(sizeof(struct event_header) != sizeof(u32));

	padding = lib_ring_buffer_align(offset,
					sizeof(struct event_header));
	offset += padding;
	offset += sizeof(struct event_header);

	if (unlikely(rflags)) {
		switch (rflags) {
		case LTT_RFLAG_ID_SIZE_TSC:
			offset += sizeof(u16) + sizeof(u16);
			if (data_size >= LTT_MAX_SMALL_SIZE)
				offset += sizeof(u32);
			offset += lib_ring_buffer_align(offset, sizeof(u64));
			offset += sizeof(u64);
			break;
		case LTT_RFLAG_ID_SIZE:
			offset += sizeof(u16) + sizeof(u16);
			if (data_size >= LTT_MAX_SMALL_SIZE)
				offset += sizeof(u32);
			break;
		case LTT_RFLAG_ID:
			offset += sizeof(u16);
			break;
		}
	}

	*pre_header_padding = padding;
	return offset - orig_offset;
}

#include "wrapper/ringbuffer/api.h"

extern
void ltt_write_event_header_slow(const struct lib_ring_buffer_config *config,
				 struct lib_ring_buffer_ctx *ctx,
				 u16 eID, u32 event_size);

/*
 * ltt_write_event_header
 *
 * Writes the event header to the offset (already aligned on 32-bits).
 *
 * @config: ring buffer instance configuration
 * @ctx: reservation context
 * @eID : event ID
 * @event_size : size of the event, excluding the event header.
 */
static __inline__
void ltt_write_event_header(const struct lib_ring_buffer_config *config,
			    struct lib_ring_buffer_ctx *ctx,
			    u16 eID, u32 event_size)
{
	struct event_header header;

	if (unlikely(ctx->rflags))
		goto slow_path;

	header.id_time = eID << LTT_TSC_BITS;
	header.id_time |= (u32)ctx->tsc & LTT_TSC_MASK;
	lib_ring_buffer_write(config, ctx, &header, sizeof(header));

slow_path:
	ltt_write_event_header_slow(config, ctx, eID, event_size);
}

void ltt_write_event_header_slow(const struct lib_ring_buffer_config *config,
				   struct lib_ring_buffer_ctx *ctx,
				   u16 eID, u32 event_size)
{
	struct event_header header;
	u16 small_size;

	switch (ctx->rflags) {
	case LTT_RFLAG_ID_SIZE_TSC:
		header.id_time = 29 << LTT_TSC_BITS;
		break;
	case LTT_RFLAG_ID_SIZE:
		header.id_time = 30 << LTT_TSC_BITS;
		break;
	case LTT_RFLAG_ID:
		header.id_time = 31 << LTT_TSC_BITS;
		break;
	default:
		WARN_ON_ONCE(1);
		header.id_time = 0;
	}

	header.id_time |= (u32)ctx->tsc & LTT_TSC_MASK;
	lib_ring_buffer_write(config, ctx, &header, sizeof(header));

	switch (ctx->rflags) {
	case LTT_RFLAG_ID_SIZE_TSC:
		small_size = (u16)min_t(u32, event_size, LTT_MAX_SMALL_SIZE);
		lib_ring_buffer_write(config, ctx, &eID, sizeof(u16));
		lib_ring_buffer_write(config, ctx, &small_size, sizeof(u16));
		if (small_size == LTT_MAX_SMALL_SIZE)
			lib_ring_buffer_write(config, ctx, &event_size,
					      sizeof(u32));
		lib_ring_buffer_align_ctx(ctx, sizeof(u64));
		lib_ring_buffer_write(config, ctx, &ctx->tsc, sizeof(u64));
		break;
	case LTT_RFLAG_ID_SIZE:
		small_size = (u16)min_t(u32, event_size, LTT_MAX_SMALL_SIZE);
		lib_ring_buffer_write(config, ctx, &eID, sizeof(u16));
		lib_ring_buffer_write(config, ctx, &small_size, sizeof(u16));
		if (small_size == LTT_MAX_SMALL_SIZE)
			lib_ring_buffer_write(config, ctx, &event_size,
					      sizeof(u32));
		break;
	case LTT_RFLAG_ID:
		lib_ring_buffer_write(config, ctx, &eID, sizeof(u16));
		break;
	}
}

static const struct lib_ring_buffer_config client_config;

static u64 client_ring_buffer_clock_read(struct channel *chan)
{
	return lib_ring_buffer_clock_read(chan);
}

static
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
 * client_packet_header_size - called on buffer-switch to a new sub-buffer
 *
 * Return header size without padding after the structure. Don't use packed
 * structure because gcc generates inefficient code on some architectures
 * (powerpc, mips..)
 */
static size_t client_packet_header_size(void)
{
	return offsetof(struct packet_header, header_end);
}

static void client_buffer_begin(struct lib_ring_buffer *buf, u64 tsc,
				unsigned int subbuf_idx)
{
	struct channel *chan = buf->backend.chan;
	struct packet_header *header =
		(struct packet_header *)
			lib_ring_buffer_offset_address(&buf->backend,
				subbuf_idx * chan->backend.subbuf_size);
	struct ltt_session *session = channel_get_private(chan);

	header->magic = CTF_MAGIC_NUMBER;
	memcpy(header->uuid, session->uuid.b, sizeof(session->uuid));
	header->timestamp_begin = tsc;
	header->timestamp_end = 0;
	header->content_size = 0xFFFFFFFF; /* for debugging */
	header->packet_size = 0xFFFFFFFF;
	header->events_lost = 0;
#if 0
	header->start_time_sec = ltt_chan->session->start_time.tv_sec;
	header->start_time_usec = ltt_chan->session->start_time.tv_usec;
	header->start_freq = ltt_chan->session->start_freq;
	header->freq_scale = ltt_chan->session->freq_scale;
#endif //0
}

/*
 * offset is assumed to never be 0 here : never deliver a completely empty
 * subbuffer. data_size is between 1 and subbuf_size.
 */
static void client_buffer_end(struct lib_ring_buffer *buf, u64 tsc,
			      unsigned int subbuf_idx, unsigned long data_size)
{
	struct channel *chan = buf->backend.chan;
	struct packet_header *header =
		(struct packet_header *)
			lib_ring_buffer_offset_address(&buf->backend,
				subbuf_idx * chan->backend.subbuf_size);
	unsigned long records_lost = 0;

	header->timestamp_end = tsc;
	header->content_size = data_size;
	header->packet_size = PAGE_ALIGN(data_size);
	records_lost += lib_ring_buffer_get_records_lost_full(&client_config, buf);
	records_lost += lib_ring_buffer_get_records_lost_wrap(&client_config, buf);
	records_lost += lib_ring_buffer_get_records_lost_big(&client_config, buf);
	header->events_lost = records_lost;
}

static int client_buffer_create(struct lib_ring_buffer *buf, void *priv,
				int cpu, const char *name)
{
	return 0;
}

static void client_buffer_finalize(struct lib_ring_buffer *buf, void *priv, int cpu)
{
}

static const struct lib_ring_buffer_config client_config = {
	.cb.ring_buffer_clock_read = client_ring_buffer_clock_read,
	.cb.record_header_size = client_record_header_size,
	.cb.subbuffer_header_size = client_packet_header_size,
	.cb.buffer_begin = client_buffer_begin,
	.cb.buffer_end = client_buffer_end,
	.cb.buffer_create = client_buffer_create,
	.cb.buffer_finalize = client_buffer_finalize,

	.tsc_bits = 32,
	.alloc = RING_BUFFER_ALLOC_PER_CPU,
	.sync = RING_BUFFER_SYNC_PER_CPU,
	.mode = RING_BUFFER_MODE_TEMPLATE,
	.backend = RING_BUFFER_PAGE,
	.output = RING_BUFFER_SPLICE,
	.oops = RING_BUFFER_OOPS_CONSISTENCY,
	.ipi = RING_BUFFER_IPI_BARRIER,
	.wakeup = RING_BUFFER_WAKEUP_BY_TIMER,
};

static
struct channel *_channel_create(const char *name,
				struct ltt_session *session, void *buf_addr,
				size_t subbuf_size, size_t num_subbuf,
				unsigned int switch_timer_interval,
				unsigned int read_timer_interval)
{
	return channel_create(&client_config, name, session, buf_addr,
			      subbuf_size, num_subbuf, switch_timer_interval,
			      read_timer_interval);
}

static
void ltt_channel_destroy(struct channel *chan)
{
	channel_destroy(chan);
}

static
struct lib_ring_buffer *ltt_buffer_read_open(struct channel *chan)
{
	struct lib_ring_buffer *buf;
	int cpu;

	for_each_channel_cpu(cpu, chan) {
		buf = channel_get_ring_buffer(&client_config, chan, cpu);
		if (!lib_ring_buffer_open_read(buf))
			return buf;
	}
	return NULL;
}

static
void ltt_buffer_read_close(struct lib_ring_buffer *buf)
{
	lib_ring_buffer_release_read(buf);
	
}

static
int ltt_event_reserve(struct lib_ring_buffer_ctx *ctx)
{
	int ret, cpu;

	cpu = lib_ring_buffer_get_cpu(&client_config);
	if (cpu < 0)
		return -EPERM;
	ctx->cpu = cpu;

	ret = lib_ring_buffer_reserve(&client_config, ctx);
	if (ret)
		goto put;
	return ret;

put:
	lib_ring_buffer_put_cpu(&client_config);
	return ret;
}

static
void ltt_event_commit(struct lib_ring_buffer_ctx *ctx)
{
	lib_ring_buffer_commit(&client_config, ctx);
	lib_ring_buffer_put_cpu(&client_config);
}

static
void ltt_event_write(struct lib_ring_buffer_ctx *ctx, const void *src,
		     size_t len)
{
	lib_ring_buffer_write(&client_config, ctx, src, len);
}

static
wait_queue_head_t *ltt_get_reader_wait_queue(struct ltt_channel *chan)
{
	return &chan->chan->read_wait;
}

static struct ltt_transport ltt_relay_transport = {
	.name = "relay-" RING_BUFFER_MODE_TEMPLATE_STRING,
	.owner = THIS_MODULE,
	.ops = {
		.channel_create = _channel_create,
		.channel_destroy = ltt_channel_destroy,
		.buffer_read_open = ltt_buffer_read_open,
		.buffer_read_close = ltt_buffer_read_close,
		.event_reserve = ltt_event_reserve,
		.event_commit = ltt_event_commit,
		.event_write = ltt_event_write,
		.packet_avail_size = NULL,	/* Would be racy anyway */
		.get_reader_wait_queue = ltt_get_reader_wait_queue,
	},
};

static int __init ltt_ring_buffer_client_init(void)
{
	/*
	 * This vmalloc sync all also takes care of the lib ring buffer
	 * vmalloc'd module pages when it is built as a module into LTTng.
	 */
	wrapper_vmalloc_sync_all();
	printk(KERN_INFO "LTT : ltt ring buffer client init\n");
	ltt_transport_register(&ltt_relay_transport);
	return 0;
}

module_init(ltt_ring_buffer_client_init);

static void __exit ltt_ring_buffer_client_exit(void)
{
	printk(KERN_INFO "LTT : ltt ring buffer client exit\n");
	ltt_transport_unregister(&ltt_relay_transport);
}

module_exit(ltt_ring_buffer_client_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("LTTng ring buffer " RING_BUFFER_MODE_TEMPLATE_STRING
		   " client");
