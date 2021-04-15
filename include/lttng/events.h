/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng/events.h
 *
 * Holds LTTng per-session event registry.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_EVENTS_H
#define _LTTNG_EVENTS_H

#include <lttng/kernel-version.h>
#include <linux/list.h>
#include <linux/kprobes.h>
#include <linux/kref.h>
#include <linux/uuid.h>
#include <linux/irq_work.h>
#include <wrapper/uprobes.h>
#include <lttng/cpuhotplug.h>
#include <lttng/tracer.h>
#include <lttng/abi.h>
#include <lttng/abi-old.h>
#include <lttng/endian.h>

#define lttng_is_signed_type(type)	(((type) -1) < (type) 1)

struct lttng_channel;
struct lttng_session;
struct lttng_metadata_cache;
struct lib_ring_buffer_ctx;
struct perf_event;
struct perf_event_attr;
struct lib_ring_buffer_config;

/* Type description */

enum lttng_kernel_type {
	lttng_kernel_type_integer,
	lttng_kernel_type_string,
	lttng_kernel_type_enum,
	lttng_kernel_type_array,
	lttng_kernel_type_sequence,
	lttng_kernel_type_struct,
	lttng_kernel_type_variant,
	NR_LTTNG_KERNEL_TYPES,
};

enum lttng_kernel_string_encoding {
	lttng_kernel_string_encoding_none = 0,
	lttng_kernel_string_encoding_UTF8 = 1,
	lttng_kernel_string_encoding_ASCII = 2,
	NR_LTTNG_KERNEL_STRING_ENCODING,
};

enum channel_type {
	PER_CPU_CHANNEL,
	METADATA_CHANNEL,
};

struct lttng_kernel_enum_value {
	unsigned long long value;
	unsigned int signedness:1;
};

struct lttng_kernel_enum_entry {
	struct lttng_kernel_enum_value start, end;	/* start and end are inclusive */
	const char *string;
	struct {
		unsigned int is_auto:1;
	} options;
};

/*
 * struct lttng_kernel_type_common is fixed-size. Its children inherits
 * from it by embedding struct lttng_kernel_type_common as its first field.
 */
struct lttng_kernel_type_common {
	enum lttng_kernel_type type;
};

struct lttng_kernel_type_integer {
	struct lttng_kernel_type_common parent;
	unsigned int size;		/* in bits */
	unsigned short alignment;	/* in bits */
	unsigned int signedness:1,
		reverse_byte_order:1;
	unsigned int base;		/* 2, 8, 10, 16, for pretty print */
};

struct lttng_kernel_type_string {
	struct lttng_kernel_type_common parent;
	enum lttng_kernel_string_encoding encoding;
};

struct lttng_kernel_type_enum {
	struct lttng_kernel_type_common parent;
	const struct lttng_kernel_enum_desc *desc;	/* Enumeration mapping */
	const struct lttng_kernel_type_common *container_type;
};

struct lttng_kernel_type_array {
	struct lttng_kernel_type_common parent;
	const struct lttng_kernel_type_common *elem_type;
	unsigned int length;			/* Num. elems. */
	unsigned int alignment;
	enum lttng_kernel_string_encoding encoding;
};

struct lttng_kernel_type_sequence {
	struct lttng_kernel_type_common parent;
	const char *length_name;	/* Length field name. */
	const struct lttng_kernel_type_common *elem_type;
	unsigned int alignment;		/* Alignment before elements. */
	enum lttng_kernel_string_encoding encoding;
};

struct lttng_kernel_type_struct {
	struct lttng_kernel_type_common parent;
	unsigned int nr_fields;
	const struct lttng_kernel_event_field **fields;	/* Array of pointers to fields. */
	unsigned int alignment;
};

struct lttng_kernel_type_variant {
	struct lttng_kernel_type_common parent;
	const char *tag_name;
	const struct lttng_kernel_event_field **choices; /* Array of pointers to fields. */
	unsigned int nr_choices;
	unsigned int alignment;
};

struct lttng_kernel_enum_desc {
	const char *name;
	const struct lttng_kernel_enum_entry **entries;
	unsigned int nr_entries;
};

/* Event field description */

struct lttng_kernel_event_field {
	const char *name;
	const struct lttng_kernel_type_common *type;
	unsigned int nowrite:1,		/* do not write into trace */
			user:1,		/* fetch from user-space */
			nofilter:1;	/* do not consider for filter */
};

#define lttng_kernel_static_type_integer(_size, _alignment, _signedness, _byte_order, _base)		\
	((const struct lttng_kernel_type_common *) __LTTNG_COMPOUND_LITERAL(const struct lttng_kernel_type_integer, { \
		.parent = {										\
			.type = lttng_kernel_type_integer,							\
		},											\
		.size = (_size),									\
		.alignment = (_alignment),								\
		.signedness = (_signedness),								\
		.reverse_byte_order = (_byte_order) != __BYTE_ORDER,					\
		.base = (_base),									\
	}))

#define lttng_kernel_static_type_integer_from_type(_type, _byte_order, _base)				\
	lttng_kernel_static_type_integer(sizeof(_type) * CHAR_BIT,					\
			lttng_alignof(_type) * CHAR_BIT,						\
			lttng_is_signed_type(_type),							\
			_byte_order,									\
			_base)

#define lttng_kernel_static_type_enum(_desc, _container_type)						\
	((const struct lttng_kernel_type_common *) __LTTNG_COMPOUND_LITERAL(const struct lttng_kernel_type_enum, { \
		.parent = {										\
			.type = lttng_kernel_type_enum,							\
		},											\
		.desc = (_desc),									\
		.container_type = (_container_type),							\
	}))

#define lttng_kernel_static_type_array(_length, _elem_type, _alignment, _encoding)			\
	((const struct lttng_kernel_type_common *) __LTTNG_COMPOUND_LITERAL(const struct lttng_kernel_type_array, { \
		.parent = {										\
			.type = lttng_kernel_type_array,						\
		},											\
		.length = (_length),									\
		.alignment = (_alignment),								\
		.encoding = lttng_kernel_string_encoding_##_encoding,					\
		.elem_type = (_elem_type),								\
	}))

#define lttng_kernel_static_type_array_text(_length)							\
	lttng_kernel_static_type_array(_length,								\
		lttng_kernel_static_type_integer(sizeof(char) * CHAR_BIT,				\
				lttng_alignof(char) * CHAR_BIT, lttng_is_signed_type(char),		\
				__BYTE_ORDER, 10),							\
		0, UTF8)

#define lttng_kernel_static_type_sequence(_length_name, _elem_type, _alignment, _encoding)			\
	((const struct lttng_kernel_type_common *) __LTTNG_COMPOUND_LITERAL(const struct lttng_kernel_type_sequence, { \
		.parent = {										\
			.type = lttng_kernel_type_sequence,						\
		},											\
		.length_name = (_length_name),								\
		.alignment = (_alignment),								\
		.encoding = lttng_kernel_string_encoding_##_encoding,					\
		.elem_type = (_elem_type),								\
	}))

#define lttng_kernel_static_type_string(_encoding)							\
	((const struct lttng_kernel_type_common *) __LTTNG_COMPOUND_LITERAL(const struct lttng_kernel_type_string, { \
		.parent = {										\
			.type = lttng_kernel_type_string,						\
		},											\
		.encoding = lttng_kernel_string_encoding_##_encoding,					\
	}))

#define lttng_kernel_static_type_struct(_nr_fields, _fields, _alignment)				\
	((const struct lttng_kernel_type_common *) __LTTNG_COMPOUND_LITERAL(const struct lttng_kernel_type_struct, { \
		.parent = {										\
			.type = lttng_kernel_type_struct,						\
		},											\
		.nr_fields = (_nr_fields),								\
		.fields = _fields,									\
		.alignment = (_alignment),								\
	}))

#define lttng_kernel_static_type_variant(_nr_choices, _choices, _tag_name, _alignment)			\
	((const struct lttng_kernel_type_common *) __LTTNG_COMPOUND_LITERAL(const struct lttng_kernel_type_variant, { \
		.parent = {										\
			.type = lttng_kernel_type_variant,						\
		},											\
		.tag_name = (_tag_name),								\
		.choices = _choices,									\
		.nr_choices = (_nr_choices),								\
		.alignment = (_alignment),								\
	}))

#define lttng_kernel_static_event_field(_name, _type, _nowrite, _user, _nofilter)			\
	__LTTNG_COMPOUND_LITERAL(const struct lttng_kernel_event_field, {				\
		.name = (_name),									\
		.type = (_type),									\
		.nowrite = (_nowrite),									\
		.user = (_user),									\
		.nofilter = (_nofilter),								\
	})

#define lttng_kernel_static_event_field_array(_fields...)						\
	__LTTNG_COMPOUND_LITERAL(const struct lttng_kernel_event_field *,				\
		_fields											\
	)

#define lttng_kernel_static_ctx_field(_event_field, _get_size, _get_size_arg, _record, _get_value, _destroy, _priv) \
	__LTTNG_COMPOUND_LITERAL(const struct lttng_kernel_ctx_field, {					\
		.event_field = (_event_field),								\
		.get_size = (_get_size),								\
		.get_size_arg = (_get_size_arg),							\
		.record = (_record),									\
		.get_value = (_get_value),								\
		.destroy = (_destroy),									\
		.priv = (_priv),									\
	})

#define lttng_kernel_static_enum_entry_value(_string, _value)						\
	__LTTNG_COMPOUND_LITERAL(const struct lttng_kernel_enum_entry, {				\
		.start = {										\
			.signedness = lttng_is_signed_type(__typeof__(_value)),				\
			.value = lttng_is_signed_type(__typeof__(_value)) ?				\
				(long long) (_value) : (_value),					\
		},											\
		.end = {										\
			.signedness = lttng_is_signed_type(__typeof__(_value)),				\
			.value = lttng_is_signed_type(__typeof__(_value)) ?				\
				(long long) (_value) : (_value),					\
		},											\
		.string = (_string),									\
	}),

#define lttng_kernel_static_enum_entry_range(_string, _range_start, _range_end)				\
	__LTTNG_COMPOUND_LITERAL(const struct lttng_kernel_enum_entry, {				\
		.start = {										\
			.signedness = lttng_is_signed_type(__typeof__(_range_start)),			\
			.value = lttng_is_signed_type(__typeof__(_range_start)) ?			\
				(long long) (_range_start) : (_range_start),				\
		},											\
		.end = {										\
			.signedness = lttng_is_signed_type(__typeof__(_range_end)),			\
			.value = lttng_is_signed_type(__typeof__(_range_end)) ?				\
				(long long) (_range_end) : (_range_end),				\
		},											\
		.string = (_string),									\
	}),

#define lttng_kernel_static_enum_entry_auto(_string)							\
	__LTTNG_COMPOUND_LITERAL(const struct lttng_kernel_enum_entry, {				\
		.start = {										\
			.signedness = -1,								\
			.value = -1,									\
		},											\
		.end = {										\
			.signedness = -1, 								\
			.value = -1,									\
		},											\
		.string = (_string),									\
		.options = {										\
			.is_auto = 1,									\
		}											\
	}),

union lttng_ctx_value {
	int64_t s64;
	const char *str;
	double d;
};

/*
 * We need to keep this perf counter field separately from struct
 * lttng_kernel_ctx_field because cpu hotplug needs fixed-location addresses.
 */
struct lttng_perf_counter_field {
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0))
	struct lttng_cpuhp_node cpuhp_prepare;
	struct lttng_cpuhp_node cpuhp_online;
#else
	struct notifier_block nb;
	int hp_enable;
#endif
	struct perf_event_attr *attr;
	struct perf_event **e;	/* per-cpu array */
	char *name;
	struct lttng_kernel_event_field *event_field;
};

struct lttng_probe_ctx {
	struct lttng_kernel_event_recorder *event;
	struct lttng_kernel_event_notifier *event_notifier; // Not sure if we will ever need it.
	uint8_t interruptible;
};

struct lttng_kernel_ctx_field {
	const struct lttng_kernel_event_field *event_field;
	size_t (*get_size)(size_t offset);
	size_t (*get_size_arg)(size_t offset, struct lttng_kernel_ctx_field *field,
	                       struct lib_ring_buffer_ctx *ctx,
	                       struct lttng_channel *chan);
	void (*record)(struct lttng_kernel_ctx_field *field,
		       struct lib_ring_buffer_ctx *ctx,
		       struct lttng_channel *chan);
	void (*get_value)(struct lttng_kernel_ctx_field *field,
			 struct lttng_probe_ctx *lttng_probe_ctx,
			 union lttng_ctx_value *value);
	void (*destroy)(struct lttng_kernel_ctx_field *field);
	void *priv;
};

struct lttng_kernel_ctx {
	struct lttng_kernel_ctx_field *fields;
	unsigned int nr_fields;
	unsigned int allocated_fields;
	size_t largest_align;	/* in bytes */
};

struct lttng_kernel_event_desc {
	const char *event_name;		/* lttng-modules name */
	const char *event_kname;	/* Linux kernel name (tracepoints) */
	const struct lttng_kernel_probe_desc *probe_desc;
	void (*probe_callback)(void);
	const struct lttng_kernel_event_field **fields;	/* event payload */
	unsigned int nr_fields;
	struct module *owner;
	void *event_notifier_callback;
};

struct lttng_kernel_probe_desc {
	const char *provider_name;
	const struct lttng_kernel_event_desc **event_desc;
	unsigned int nr_events;
	struct list_head head;			/* chain registered probes */
	struct list_head lazy_init_head;
	int lazy;				/* lazy registration */
};

struct lttng_krp;				/* Kretprobe handling */

enum lttng_bytecode_node_type {
	LTTNG_BYTECODE_NODE_TYPE_FILTER,
	LTTNG_BYTECODE_NODE_TYPE_CAPTURE,
};

struct lttng_bytecode_node {
	enum lttng_bytecode_node_type type;
	struct list_head node;
	struct lttng_enabler *enabler;
	struct {
		uint32_t len;
		uint32_t reloc_offset;
		uint64_t seqnum;
		char data[];
	} bc;
};

/*
 * Bytecode interpreter return value masks.
 */
enum lttng_bytecode_interpreter_ret {
	LTTNG_INTERPRETER_DISCARD = 0,
	LTTNG_INTERPRETER_RECORD_FLAG = (1ULL << 0),
	/* Other bits are kept for future use. */
};

struct lttng_interpreter_output;

struct lttng_bytecode_runtime {
	/* Associated bytecode */
	struct lttng_bytecode_node *bc;
	union {
		uint64_t (*filter)(void *filter_data,
				struct lttng_probe_ctx *lttng_probe_ctx,
				const char *filter_stack_data);
		uint64_t (*capture)(void *filter_data,
				struct lttng_probe_ctx *lttng_probe_ctx,
				const char *capture_stack_data,
				struct lttng_interpreter_output *output);
	} interpreter_funcs;
	int link_failed;
	struct list_head node;	/* list of bytecode runtime in event */
	struct lttng_kernel_ctx *ctx;
};

/*
 * Objects in a linked-list of enablers, owned by an event.
 */
struct lttng_enabler_ref {
	struct list_head node;			/* enabler ref list */
	struct lttng_enabler *ref;		/* backward ref */
};

struct lttng_uprobe_handler {
	union {
		struct lttng_kernel_event_recorder *event;
		struct lttng_kernel_event_notifier *event_notifier;
	} u;
	loff_t offset;
	struct uprobe_consumer up_consumer;
	struct list_head node;
};

struct lttng_kprobe {
	struct kprobe kp;
	char *symbol_name;
};

struct lttng_uprobe {
	struct inode *inode;
	struct list_head head;
};

enum lttng_syscall_entryexit {
	LTTNG_SYSCALL_ENTRY,
	LTTNG_SYSCALL_EXIT,
};

enum lttng_syscall_abi {
	LTTNG_SYSCALL_ABI_NATIVE,
	LTTNG_SYSCALL_ABI_COMPAT,
};

struct lttng_kernel_event_common_private;

enum lttng_kernel_event_type {
	LTTNG_KERNEL_EVENT_TYPE_RECORDER = 0,
	LTTNG_KERNEL_EVENT_TYPE_NOTIFIER = 1,
};

struct lttng_kernel_event_common {
	struct lttng_kernel_event_common_private *priv;	/* Private event interface */

	enum lttng_kernel_event_type type;
	/* Get child with container_of(). */

	int enabled;
	int eval_filter;				/* Need to evaluate filters */
	int (*run_filter)(struct lttng_kernel_event_common *event,
		const char *stack_data,
		void *filter_ctx);
};

struct lttng_kernel_event_recorder_private;

struct lttng_kernel_event_recorder {
	struct lttng_kernel_event_common parent;
	struct lttng_kernel_event_recorder_private *priv;	/* Private event record interface */

	struct lttng_channel *chan;
};

struct lttng_kernel_notification_ctx {
	int eval_capture;					/* Capture evaluation available. */
};

struct lttng_kernel_event_notifier_private;

struct lttng_kernel_event_notifier {
	struct lttng_kernel_event_common parent;
	struct lttng_kernel_event_notifier_private *priv;	/* Private event notifier interface */

	int eval_capture;				/* Need to evaluate capture */
	void (*notification_send)(struct lttng_kernel_event_notifier *event_notifier,
		struct lttng_probe_ctx *probe_ctx,
		const char *stack_data,
		struct lttng_kernel_notification_ctx *notif_ctx);
};

enum lttng_enabler_format_type {
	LTTNG_ENABLER_FORMAT_STAR_GLOB,
	LTTNG_ENABLER_FORMAT_NAME,
};

/*
 * Enabler field, within whatever object is enabling an event. Target of
 * backward reference.
 */
struct lttng_enabler {
	enum lttng_enabler_format_type format_type;

	/* head list of struct lttng_bytecode_node */
	struct list_head filter_bytecode_head;

	struct lttng_kernel_abi_event event_param;
	unsigned int enabled:1;

	uint64_t user_token;		/* User-provided token. */
};

struct lttng_event_enabler {
	struct lttng_enabler base;
	struct list_head node;	/* per-session list of enablers */
	struct lttng_channel *chan;
};

struct lttng_event_notifier_enabler {
	struct lttng_enabler base;
	uint64_t error_counter_index;
	struct list_head node;	/* List of event_notifier enablers */
	struct lttng_event_notifier_group *group;

	/* head list of struct lttng_bytecode_node */
	struct list_head capture_bytecode_head;
	uint64_t num_captures;
};


static inline
struct lttng_enabler *lttng_event_enabler_as_enabler(
		struct lttng_event_enabler *event_enabler)
{
	return &event_enabler->base;
}

static inline
struct lttng_enabler *lttng_event_notifier_enabler_as_enabler(
		struct lttng_event_notifier_enabler *event_notifier_enabler)
{
	return &event_notifier_enabler->base;
}

struct lttng_channel_ops {
	struct channel *(*channel_create)(const char *name,
				void *priv,
				void *buf_addr,
				size_t subbuf_size, size_t num_subbuf,
				unsigned int switch_timer_interval,
				unsigned int read_timer_interval);
	void (*channel_destroy)(struct channel *chan);
	struct lib_ring_buffer *(*buffer_read_open)(struct channel *chan);
	int (*buffer_has_read_closed_stream)(struct channel *chan);
	void (*buffer_read_close)(struct lib_ring_buffer *buf);
	int (*event_reserve)(struct lib_ring_buffer_ctx *ctx,
			     uint32_t event_id);
	void (*event_commit)(struct lib_ring_buffer_ctx *ctx);
	void (*event_write)(struct lib_ring_buffer_ctx *ctx, const void *src,
			    size_t len);
	void (*event_write_from_user)(struct lib_ring_buffer_ctx *ctx,
				      const void *src, size_t len);
	void (*event_memset)(struct lib_ring_buffer_ctx *ctx,
			     int c, size_t len);
	void (*event_strcpy)(struct lib_ring_buffer_ctx *ctx, const char *src,
			     size_t len);
	void (*event_strcpy_from_user)(struct lib_ring_buffer_ctx *ctx,
				       const char __user *src, size_t len);
	/*
	 * packet_avail_size returns the available size in the current
	 * packet. Note that the size returned is only a hint, since it
	 * may change due to concurrent writes.
	 */
	size_t (*packet_avail_size)(struct channel *chan);
	wait_queue_head_t *(*get_writer_buf_wait_queue)(struct channel *chan, int cpu);
	wait_queue_head_t *(*get_hp_wait_queue)(struct channel *chan);
	int (*is_finalized)(struct channel *chan);
	int (*is_disabled)(struct channel *chan);
	int (*timestamp_begin) (const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *timestamp_begin);
	int (*timestamp_end) (const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *timestamp_end);
	int (*events_discarded) (const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *events_discarded);
	int (*content_size) (const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *content_size);
	int (*packet_size) (const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *packet_size);
	int (*stream_id) (const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *stream_id);
	int (*current_timestamp) (const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *ts);
	int (*sequence_number) (const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *seq);
	int (*instance_id) (const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *id);
};

struct lttng_counter_ops {
	struct lib_counter *(*counter_create)(size_t nr_dimensions,
			const size_t *max_nr_elem,	/* for each dimension */
			int64_t global_sum_step);
	void (*counter_destroy)(struct lib_counter *counter);
	int (*counter_add)(struct lib_counter *counter, const size_t *dimension_indexes,
			int64_t v);
	/*
	 * counter_read reads a specific cpu's counter if @cpu >= 0, or
	 * the global aggregation counter if @cpu == -1.
	 */
	int (*counter_read)(struct lib_counter *counter, const size_t *dimension_indexes, int cpu,
			 int64_t *value, bool *overflow, bool *underflow);
	/*
	 * counter_aggregate returns the total sum of all per-cpu counters and
	 * the global aggregation counter.
	 */
	int (*counter_aggregate)(struct lib_counter *counter, const size_t *dimension_indexes,
			int64_t *value, bool *overflow, bool *underflow);
	int (*counter_clear)(struct lib_counter *counter, const size_t *dimension_indexes);
};

struct lttng_transport {
	char *name;
	struct module *owner;
	struct list_head node;
	struct lttng_channel_ops ops;
};

struct lttng_counter_transport {
	char *name;
	struct module *owner;
	struct list_head node;
	struct lttng_counter_ops ops;
};

struct lttng_syscall_filter;

#define LTTNG_EVENT_HT_BITS		12
#define LTTNG_EVENT_HT_SIZE		(1U << LTTNG_EVENT_HT_BITS)

struct lttng_event_ht {
	struct hlist_head table[LTTNG_EVENT_HT_SIZE];
};

#define LTTNG_EVENT_NOTIFIER_HT_BITS		12
#define LTTNG_EVENT_NOTIFIER_HT_SIZE		(1U << LTTNG_EVENT_NOTIFIER_HT_BITS)

struct lttng_event_notifier_ht {
	struct hlist_head table[LTTNG_EVENT_NOTIFIER_HT_SIZE];
};

struct lttng_channel {
	unsigned int id;
	struct channel *chan;		/* Channel buffers */
	int enabled;
	struct lttng_kernel_ctx *ctx;
	/* Event ID management */
	struct lttng_session *session;
	struct file *file;		/* File associated to channel */
	unsigned int free_event_id;	/* Next event ID to allocate */
	struct list_head list;		/* Channel list */
	struct lttng_channel_ops *ops;
	struct lttng_transport *transport;
	struct hlist_head *sc_table;	/* for syscall tracing */
	struct hlist_head *compat_sc_table;
	struct hlist_head *sc_exit_table;	/* for syscall exit tracing */
	struct hlist_head *compat_sc_exit_table;
	struct hlist_head sc_unknown;	/* for unknown syscalls */
	struct hlist_head sc_compat_unknown;
	struct hlist_head sc_exit_unknown;
	struct hlist_head compat_sc_exit_unknown;
	struct lttng_syscall_filter *sc_filter;
	int header_type;		/* 0: unset, 1: compact, 2: large */
	enum channel_type channel_type;
	int syscall_all_entry;
	int syscall_all_exit;
	unsigned int metadata_dumped:1,
		sys_enter_registered:1,
		sys_exit_registered:1,
		tstate:1;		/* Transient enable state */
};

struct lttng_metadata_stream {
	void *priv;			/* Ring buffer private data */
	struct lttng_metadata_cache *metadata_cache;
	unsigned int metadata_in;	/* Bytes read from the cache */
	unsigned int metadata_out;	/* Bytes consumed from stream */
	int finalized;			/* Has channel been finalized */
	wait_queue_head_t read_wait;	/* Reader buffer-level wait queue */
	struct list_head list;		/* Stream list */
	struct lttng_transport *transport;
	uint64_t version;		/* Current version of the metadata cache */
	bool coherent;			/* Stream in a coherent state */
};

#define LTTNG_DYNAMIC_LEN_STACK_SIZE	128

struct lttng_dynamic_len_stack {
	size_t stack[LTTNG_DYNAMIC_LEN_STACK_SIZE];
	size_t offset;
};

DECLARE_PER_CPU(struct lttng_dynamic_len_stack, lttng_dynamic_len_stack);

/*
 * struct lttng_id_tracker declared in header due to deferencing of *v
 * in RCU_INITIALIZER(v).
 */
#define LTTNG_ID_HASH_BITS	6
#define LTTNG_ID_TABLE_SIZE	(1 << LTTNG_ID_HASH_BITS)

enum tracker_type {
	TRACKER_PID,
	TRACKER_VPID,
	TRACKER_UID,
	TRACKER_VUID,
	TRACKER_GID,
	TRACKER_VGID,

	TRACKER_UNKNOWN,
};

struct lttng_id_tracker_rcu {
	struct hlist_head id_hash[LTTNG_ID_TABLE_SIZE];
};

struct lttng_id_tracker {
	struct lttng_session *session;
	enum tracker_type tracker_type;
	struct lttng_id_tracker_rcu *p;	/* RCU dereferenced. */
};

struct lttng_id_hash_node {
	struct hlist_node hlist;
	int id;
};

struct lttng_session {
	int active;			/* Is trace session active ? */
	int been_active;		/* Has trace session been active ? */
	struct file *file;		/* File associated to session */
	struct list_head chan;		/* Channel list head */
	struct list_head events;	/* Event list head */
	struct list_head list;		/* Session list */
	unsigned int free_chan_id;	/* Next chan ID to allocate */
	uuid_le uuid;			/* Trace session unique ID */
	struct lttng_metadata_cache *metadata_cache;
	struct lttng_id_tracker pid_tracker;
	struct lttng_id_tracker vpid_tracker;
	struct lttng_id_tracker uid_tracker;
	struct lttng_id_tracker vuid_tracker;
	struct lttng_id_tracker gid_tracker;
	struct lttng_id_tracker vgid_tracker;
	unsigned int metadata_dumped:1,
		tstate:1;		/* Transient enable state */
	/* List of event enablers */
	struct list_head enablers_head;
	/* Hash table of events */
	struct lttng_event_ht events_ht;
	char name[LTTNG_KERNEL_ABI_SESSION_NAME_LEN];
	char creation_time[LTTNG_KERNEL_ABI_SESSION_CREATION_TIME_ISO8601_LEN];
};

struct lttng_counter {
	struct file *file;		/* File associated to counter. */
	struct file *owner;
	struct lttng_counter_transport *transport;
	struct lib_counter *counter;
	struct lttng_counter_ops *ops;
};

struct lttng_event_notifier_group {
	struct file *file;		/* File associated to event notifier group */
	struct file *notif_file;	/* File used to expose notifications to userspace. */
	struct list_head node;		/* event notifier group list */
	struct list_head enablers_head; /* List of enablers */
	struct list_head event_notifiers_head; /* List of event notifier */
	struct lttng_event_notifier_ht event_notifiers_ht; /* Hash table of event notifiers */
	struct lttng_channel_ops *ops;
	struct lttng_transport *transport;
	struct channel *chan;		/* Ring buffer channel for event notifier group. */
	struct lib_ring_buffer *buf;	/* Ring buffer for event notifier group. */
	wait_queue_head_t read_wait;
	struct irq_work wakeup_pending;	/* Pending wakeup irq work. */
	struct lttng_kernel_event_notifier *sc_unknown;	/* for unknown syscalls */
	struct lttng_kernel_event_notifier *sc_compat_unknown;

	struct lttng_syscall_filter *sc_filter;

	struct hlist_head *event_notifier_syscall_dispatch;
	struct hlist_head *event_notifier_compat_syscall_dispatch;
	struct hlist_head *event_notifier_exit_syscall_dispatch;
	struct hlist_head *event_notifier_exit_compat_syscall_dispatch;

	struct hlist_head event_notifier_unknown_syscall_dispatch;
	struct hlist_head event_notifier_compat_unknown_syscall_dispatch;
	struct hlist_head event_notifier_exit_unknown_syscall_dispatch;
	struct hlist_head event_notifier_exit_compat_unknown_syscall_dispatch;

	int syscall_all_entry;
	int syscall_all_exit;

	unsigned int sys_enter_registered:1, sys_exit_registered:1;

	struct lttng_counter *error_counter;
	size_t error_counter_len;
};

struct lttng_metadata_cache {
	char *data;			/* Metadata cache */
	unsigned int cache_alloc;	/* Metadata allocated size (bytes) */
	unsigned int metadata_written;	/* Number of bytes written in metadata cache */
	atomic_t producing;		/* Metadata being produced (incomplete) */
	struct kref refcount;		/* Metadata cache usage */
	struct list_head metadata_stream;	/* Metadata stream list */
	uuid_le uuid;			/* Trace session unique ID (copy) */
	struct mutex lock;		/* Produce/consume lock */
	uint64_t version;		/* Current version of the metadata */
};

void lttng_lock_sessions(void);
void lttng_unlock_sessions(void);

struct list_head *lttng_get_probe_list_head(void);

struct lttng_event_enabler *lttng_event_enabler_create(
		enum lttng_enabler_format_type format_type,
		struct lttng_kernel_abi_event *event_param,
		struct lttng_channel *chan);

int lttng_event_enabler_enable(struct lttng_event_enabler *event_enabler);
int lttng_event_enabler_disable(struct lttng_event_enabler *event_enabler);
struct lttng_event_notifier_enabler *lttng_event_notifier_enabler_create(
		struct lttng_event_notifier_group *event_notifier_group,
		enum lttng_enabler_format_type format_type,
		struct lttng_kernel_abi_event_notifier *event_notifier_param);

int lttng_event_notifier_enabler_enable(
		struct lttng_event_notifier_enabler *event_notifier_enabler);
int lttng_event_notifier_enabler_disable(
		struct lttng_event_notifier_enabler *event_notifier_enabler);
int lttng_fix_pending_events(void);
int lttng_fix_pending_event_notifiers(void);
int lttng_session_active(void);
bool lttng_event_notifier_active(void);

struct lttng_session *lttng_session_create(void);
int lttng_session_enable(struct lttng_session *session);
int lttng_session_disable(struct lttng_session *session);
void lttng_session_destroy(struct lttng_session *session);
int lttng_session_metadata_regenerate(struct lttng_session *session);
int lttng_session_statedump(struct lttng_session *session);
void metadata_cache_destroy(struct kref *kref);

struct lttng_counter *lttng_kernel_counter_create(
		const char *counter_transport_name, size_t number_dimensions,
		const size_t *dimensions_sizes);
int lttng_kernel_counter_read(struct lttng_counter *counter,
		const size_t *dimension_indexes, int32_t cpu,
		int64_t *val, bool *overflow, bool *underflow);
int lttng_kernel_counter_aggregate(struct lttng_counter *counter,
		const size_t *dimension_indexes, int64_t *val,
		bool *overflow, bool *underflow);
int lttng_kernel_counter_clear(struct lttng_counter *counter,
		const size_t *dimension_indexes);


struct lttng_event_notifier_group *lttng_event_notifier_group_create(void);
int lttng_event_notifier_group_create_error_counter(
		struct file *event_notifier_group_file,
		const struct lttng_kernel_abi_counter_conf *error_counter_conf);
void lttng_event_notifier_group_destroy(
		struct lttng_event_notifier_group *event_notifier_group);

struct lttng_channel *lttng_channel_create(struct lttng_session *session,
				       const char *transport_name,
				       void *buf_addr,
				       size_t subbuf_size, size_t num_subbuf,
				       unsigned int switch_timer_interval,
				       unsigned int read_timer_interval,
				       enum channel_type channel_type);
struct lttng_channel *lttng_global_channel_create(struct lttng_session *session,
				       int overwrite, void *buf_addr,
				       size_t subbuf_size, size_t num_subbuf,
				       unsigned int switch_timer_interval,
				       unsigned int read_timer_interval);

void lttng_metadata_channel_destroy(struct lttng_channel *chan);
struct lttng_kernel_event_recorder *lttng_kernel_event_recorder_create(struct lttng_channel *chan,
				struct lttng_kernel_abi_event *event_param,
				const struct lttng_kernel_event_desc *event_desc,
				enum lttng_kernel_abi_instrumentation itype);
struct lttng_kernel_event_recorder *_lttng_kernel_event_recorder_create(struct lttng_channel *chan,
				struct lttng_kernel_abi_event *event_param,
				const struct lttng_kernel_event_desc *event_desc,
				enum lttng_kernel_abi_instrumentation itype);
struct lttng_kernel_event_recorder *lttng_event_compat_old_create(struct lttng_channel *chan,
		struct lttng_kernel_abi_old_event *old_event_param,
		const struct lttng_kernel_event_desc *internal_desc);

struct lttng_kernel_event_notifier *lttng_event_notifier_create(
				const struct lttng_kernel_event_desc *event_notifier_desc,
				uint64_t id,
				uint64_t error_counter_idx,
				struct lttng_event_notifier_group *event_notifier_group,
				struct lttng_kernel_abi_event_notifier *event_notifier_param,
				enum lttng_kernel_abi_instrumentation itype);
struct lttng_kernel_event_notifier *_lttng_event_notifier_create(
				const struct lttng_kernel_event_desc *event_notifier_desc,
				uint64_t id,
				uint64_t error_counter_idx,
				struct lttng_event_notifier_group *event_notifier_group,
				struct lttng_kernel_abi_event_notifier *event_notifier_param,
				enum lttng_kernel_abi_instrumentation itype);

int lttng_channel_enable(struct lttng_channel *channel);
int lttng_channel_disable(struct lttng_channel *channel);
int lttng_event_enable(struct lttng_kernel_event_recorder *event);
int lttng_event_disable(struct lttng_kernel_event_recorder *event);

int lttng_event_notifier_enable(struct lttng_kernel_event_notifier *event_notifier);
int lttng_event_notifier_disable(struct lttng_kernel_event_notifier *event_notifier);

void lttng_transport_register(struct lttng_transport *transport);
void lttng_transport_unregister(struct lttng_transport *transport);

void lttng_counter_transport_register(struct lttng_counter_transport *transport);
void lttng_counter_transport_unregister(struct lttng_counter_transport *transport);

void synchronize_trace(void);
int lttng_abi_init(void);
int lttng_abi_compat_old_init(void);
void lttng_abi_exit(void);
void lttng_abi_compat_old_exit(void);

int lttng_probe_register(struct lttng_kernel_probe_desc *desc);
void lttng_probe_unregister(struct lttng_kernel_probe_desc *desc);
const struct lttng_kernel_event_desc *lttng_event_desc_get(const char *name);
void lttng_event_desc_put(const struct lttng_kernel_event_desc *desc);
int lttng_probes_init(void);
void lttng_probes_exit(void);

int lttng_metadata_output_channel(struct lttng_metadata_stream *stream,
		struct channel *chan, bool *coherent);

int lttng_id_tracker_get_node_id(const struct lttng_id_hash_node *node);
int lttng_id_tracker_empty_set(struct lttng_id_tracker *lf);
void lttng_id_tracker_destroy(struct lttng_id_tracker *lf, bool rcu);
bool lttng_id_tracker_lookup(struct lttng_id_tracker_rcu *p, int id);
int lttng_id_tracker_add(struct lttng_id_tracker *lf, int id);
int lttng_id_tracker_del(struct lttng_id_tracker *lf, int id);

int lttng_session_track_id(struct lttng_session *session,
		enum tracker_type tracker_type, int id);
int lttng_session_untrack_id(struct lttng_session *session,
		enum tracker_type tracker_type, int id);

int lttng_session_list_tracker_ids(struct lttng_session *session,
		enum tracker_type tracker_type);

void lttng_clock_ref(void);
void lttng_clock_unref(void);

int lttng_desc_match_enabler(const struct lttng_kernel_event_desc *desc,
		struct lttng_enabler *enabler);

#if defined(CONFIG_HAVE_SYSCALL_TRACEPOINTS)
int lttng_syscalls_register_event(struct lttng_event_enabler *event_enabler);
int lttng_syscalls_unregister_channel(struct lttng_channel *chan);
int lttng_syscalls_destroy_event(struct lttng_channel *chan);
int lttng_syscall_filter_enable_event(
		struct lttng_channel *chan,
		struct lttng_kernel_event_recorder *event);
int lttng_syscall_filter_disable_event(
		struct lttng_channel *chan,
		struct lttng_kernel_event_recorder *event);

long lttng_channel_syscall_mask(struct lttng_channel *channel,
		struct lttng_kernel_abi_syscall_mask __user *usyscall_mask);

int lttng_syscalls_register_event_notifier(
		struct lttng_event_notifier_enabler *event_notifier_enabler);
int lttng_syscalls_create_matching_event_notifiers(
		struct lttng_event_notifier_enabler *event_notifier_enabler);
int lttng_syscalls_unregister_event_notifier_group(struct lttng_event_notifier_group *group);
int lttng_syscall_filter_enable_event_notifier(struct lttng_kernel_event_notifier *event_notifier);
int lttng_syscall_filter_disable_event_notifier(struct lttng_kernel_event_notifier *event_notifier);
#else
static inline int lttng_syscalls_register_event(
		struct lttng_event_enabler *event_enabler)
{
	return -ENOSYS;
}

static inline int lttng_syscalls_unregister_channel(struct lttng_channel *chan)
{
	return 0;
}

static inline int lttng_syscalls_destroy(struct lttng_channel *chan)
{
	return 0;
}

static inline int lttng_syscall_filter_enable_event(struct lttng_channel *chan,
		struct lttng_kernel_event_recorder *event);
{
	return -ENOSYS;
}

static inline int lttng_syscall_filter_disable_event(struct lttng_channel *chan,
		struct lttng_kernel_event_recorder *event);
{
	return -ENOSYS;
}

static inline long lttng_channel_syscall_mask(struct lttng_channel *channel,
		struct lttng_kernel_syscall_mask __user *usyscall_mask)
{
	return -ENOSYS;
}

static inline int lttng_syscalls_register_event_notifier(
		struct lttng_event_notifier_group *group)
{
	return -ENOSYS;
}

static inline int lttng_syscalls_unregister_event_notifier_group(
		struct lttng_event_notifier_group *group)
{
	return 0;
}

static inline int lttng_syscall_filter_enable_event_notifier(
		struct lttng_event_notifier_group *group,
		const char *name)
{
	return -ENOSYS;
}

static inline int lttng_syscall_filter_disable_event_notifier(
		struct lttng_event_notifier_group *group,
		const char *name)
{
	return -ENOSYS;
}

#endif

int lttng_event_enabler_attach_filter_bytecode(struct lttng_event_enabler *event_enabler,
		struct lttng_kernel_abi_filter_bytecode __user *bytecode);
int lttng_event_notifier_enabler_attach_filter_bytecode(
		struct lttng_event_notifier_enabler *event_notifier_enabler,
		struct lttng_kernel_abi_filter_bytecode __user *bytecode);
int lttng_event_notifier_enabler_attach_capture_bytecode(
		struct lttng_event_notifier_enabler *event_notifier_enabler,
		struct lttng_kernel_abi_capture_bytecode __user *bytecode);

void lttng_enabler_link_bytecode(const struct lttng_kernel_event_desc *event_desc,
		struct lttng_kernel_ctx *ctx,
		struct list_head *instance_bytecode_runtime_head,
		struct list_head *enabler_bytecode_runtime_head);
void lttng_free_event_filter_runtime(struct lttng_kernel_event_recorder *event);
void lttng_free_event_notifier_filter_runtime(struct lttng_kernel_event_notifier *event_notifier);

int lttng_probes_init(void);

extern struct lttng_kernel_ctx *lttng_static_ctx;

int lttng_context_init(void);
void lttng_context_exit(void);
int lttng_kernel_context_append(struct lttng_kernel_ctx **ctx_p,
		const struct lttng_kernel_ctx_field *f);
void lttng_kernel_context_remove_last(struct lttng_kernel_ctx **ctx_p);
struct lttng_kernel_ctx_field *lttng_kernel_get_context_field_from_index(struct lttng_kernel_ctx *ctx,
		size_t index);
int lttng_kernel_find_context(struct lttng_kernel_ctx *ctx, const char *name);
int lttng_kernel_get_context_index(struct lttng_kernel_ctx *ctx, const char *name);
void lttng_kernel_destroy_context(struct lttng_kernel_ctx *ctx);
int lttng_add_pid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_cpu_id_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_procname_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_prio_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_nice_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_vpid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_tid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_vtid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_ppid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_vppid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_hostname_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_interruptible_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_need_reschedule_to_ctx(struct lttng_kernel_ctx **ctx);
#if defined(CONFIG_PREEMPT_RT_FULL) || defined(CONFIG_PREEMPT)
int lttng_add_preemptible_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_preemptible_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif
#ifdef CONFIG_PREEMPT_RT_FULL
int lttng_add_migratable_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_migratable_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif

int lttng_add_callstack_to_ctx(struct lttng_kernel_ctx **ctx, int type);

#if defined(CONFIG_CGROUPS) && \
	((LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,6,0)) || \
	 LTTNG_UBUNTU_KERNEL_RANGE(4,4,0,0, 4,5,0,0))
int lttng_add_cgroup_ns_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_cgroup_ns_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif

#if defined(CONFIG_IPC_NS) && \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,8,0))
int lttng_add_ipc_ns_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_ipc_ns_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif

#if !defined(LTTNG_MNT_NS_MISSING_HEADER) && \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,8,0))
int lttng_add_mnt_ns_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_mnt_ns_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif

#if defined(CONFIG_NET_NS) && \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,8,0))
int lttng_add_net_ns_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_net_ns_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif

#if defined(CONFIG_PID_NS) && \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,8,0))
int lttng_add_pid_ns_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_pid_ns_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif

#if defined(CONFIG_USER_NS) && \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,8,0))
int lttng_add_user_ns_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_user_ns_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif

#if defined(CONFIG_UTS_NS) && \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,8,0))
int lttng_add_uts_ns_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_uts_ns_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif

#if defined(CONFIG_TIME_NS) && \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,6,0))
int lttng_add_time_ns_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_time_ns_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif

int lttng_add_uid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_euid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_suid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_gid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_egid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_sgid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_vuid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_veuid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_vsuid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_vgid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_vegid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_vsgid_to_ctx(struct lttng_kernel_ctx **ctx);

#if defined(CONFIG_PERF_EVENTS)
int lttng_add_perf_counter_to_ctx(uint32_t type,
				  uint64_t config,
				  const char *name,
				  struct lttng_kernel_ctx **ctx);
int lttng_cpuhp_perf_counter_online(unsigned int cpu,
		struct lttng_cpuhp_node *node);
int lttng_cpuhp_perf_counter_dead(unsigned int cpu,
		struct lttng_cpuhp_node *node);
#else
static inline
int lttng_add_perf_counter_to_ctx(uint32_t type,
				  uint64_t config,
				  const char *name,
				  struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
static inline
int lttng_cpuhp_perf_counter_online(unsigned int cpu,
		struct lttng_cpuhp_node *node)
{
	return 0;
}
static inline
int lttng_cpuhp_perf_counter_dead(unsigned int cpu,
		struct lttng_cpuhp_node *node)
{
	return 0;
}
#endif

int lttng_logger_init(void);
void lttng_logger_exit(void);

extern int lttng_statedump_start(struct lttng_session *session);

#ifdef CONFIG_KPROBES
int lttng_kprobes_register_event(const char *name,
		const char *symbol_name,
		uint64_t offset,
		uint64_t addr,
		struct lttng_kernel_event_recorder *event);
void lttng_kprobes_unregister_event(struct lttng_kernel_event_recorder *event);
void lttng_kprobes_destroy_event_private(struct lttng_kernel_event_recorder *event);
int lttng_kprobes_register_event_notifier(const char *symbol_name,
		uint64_t offset,
		uint64_t addr,
		struct lttng_kernel_event_notifier *event_notifier);
void lttng_kprobes_unregister_event_notifier(struct lttng_kernel_event_notifier *event_notifier);
void lttng_kprobes_destroy_event_notifier_private(struct lttng_kernel_event_notifier *event_notifier);
#else
static inline
int lttng_kprobes_register_event(const char *name,
		const char *symbol_name,
		uint64_t offset,
		uint64_t addr,
		struct lttng_kernel_event_recorder *event)
{
	return -ENOSYS;
}

static inline
void lttng_kprobes_unregister_event(struct lttng_kernel_event_recorder *event)
{
}

static inline
void lttng_kprobes_destroy_event_private(struct lttng_kernel_event_recorder *event)
{
}

static inline
int lttng_kprobes_register_event_notifier(const char *symbol_name,
		uint64_t offset,
		uint64_t addr,
		struct lttng_kernel_event_notifier *event_notifier)
{
	return -ENOSYS;
}

static inline
void lttng_kprobes_unregister_event_notifier(struct lttng_kernel_event_notifier *event_notifier)
{
}

static inline
void lttng_kprobes_destroy_event_notifier_private(struct lttng_kernel_event_notifier *event_notifier)
{
}
#endif

int lttng_event_add_callsite(struct lttng_kernel_event_recorder *event,
	struct lttng_kernel_abi_event_callsite __user *callsite);

int lttng_event_notifier_add_callsite(struct lttng_kernel_event_notifier *event_notifier,
	struct lttng_kernel_abi_event_callsite __user *callsite);

#ifdef CONFIG_UPROBES
int lttng_uprobes_register_event(const char *name,
	int fd, struct lttng_kernel_event_recorder *event);
int lttng_uprobes_event_add_callsite(struct lttng_kernel_event_recorder *event,
	struct lttng_kernel_abi_event_callsite __user *callsite);
void lttng_uprobes_unregister_event(struct lttng_kernel_event_recorder *event);
void lttng_uprobes_destroy_event_private(struct lttng_kernel_event_recorder *event);
int lttng_uprobes_register_event_notifier(const char *name,
	int fd, struct lttng_kernel_event_notifier *event_notifier);
int lttng_uprobes_event_notifier_add_callsite(struct lttng_kernel_event_notifier *event_notifier,
	struct lttng_kernel_abi_event_callsite __user *callsite);
void lttng_uprobes_unregister_event_notifier(struct lttng_kernel_event_notifier *event_notifier);
void lttng_uprobes_destroy_event_notifier_private(struct lttng_kernel_event_notifier *event_notifier);
#else
static inline
int lttng_uprobes_register_event(const char *name,
	int fd, struct lttng_kernel_event_recorder *event)
{
	return -ENOSYS;
}

static inline
int lttng_uprobes_event_add_callsite(struct lttng_kernel_event_recorder *event,
	struct lttng_kernel_abi_event_callsite __user *callsite)
{
	return -ENOSYS;
}

static inline
void lttng_uprobes_unregister_event(struct lttng_kernel_event_recorder *event)
{
}

static inline
void lttng_uprobes_destroy_event_private(struct lttng_kernel_event_recorder *event)
{
}

static inline
int lttng_uprobes_register_event_notifier(const char *name,
	int fd, struct lttng_kernel_event_notifier *event_notifier)
{
	return -ENOSYS;
}

static inline
int lttng_uprobes_event_notifier_add_callsite(struct lttng_kernel_event_notifier *event_notifier,
	struct lttng_kernel_abi_event_callsite __user *callsite)
{
	return -ENOSYS;
}

static inline
void lttng_uprobes_unregister_event_notifier(struct lttng_kernel_event_notifier *event_notifier)
{
}

static inline
void lttng_uprobes_destroy_event_notifier_private(struct lttng_kernel_event_notifier *event_notifier)
{
}
#endif

#ifdef CONFIG_KRETPROBES
int lttng_kretprobes_register(const char *name,
		const char *symbol_name,
		uint64_t offset,
		uint64_t addr,
		struct lttng_kernel_event_recorder *event_entry,
		struct lttng_kernel_event_recorder *event_exit);
void lttng_kretprobes_unregister(struct lttng_kernel_event_recorder *event);
void lttng_kretprobes_destroy_private(struct lttng_kernel_event_recorder *event);
int lttng_kretprobes_event_enable_state(struct lttng_kernel_event_recorder *event,
	int enable);
#else
static inline
int lttng_kretprobes_register(const char *name,
		const char *symbol_name,
		uint64_t offset,
		uint64_t addr,
		struct lttng_kernel_event_recorder *event_entry,
		struct lttng_kernel_event_recorder *event_exit)
{
	return -ENOSYS;
}

static inline
void lttng_kretprobes_unregister(struct lttng_kernel_event_recorder *event)
{
}

static inline
void lttng_kretprobes_destroy_private(struct lttng_kernel_event_recorder *event)
{
}

static inline
int lttng_kretprobes_event_enable_state(struct lttng_kernel_event_recorder *event,
	int enable)
{
	return -ENOSYS;
}
#endif

int lttng_calibrate(struct lttng_kernel_abi_calibrate *calibrate);

extern const struct file_operations lttng_tracepoint_list_fops;
extern const struct file_operations lttng_syscall_list_fops;

#define TRACEPOINT_HAS_DATA_ARG

#endif /* _LTTNG_EVENTS_H */
