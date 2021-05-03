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
	const char *length_name;	/* Length field name. If NULL, use previous field. */
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
	const char *tag_name;		/* Tag field name. If NULL, use previous field. */
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

struct lttng_kernel_probe_ctx {
	struct lttng_kernel_event_common *event;
	uint8_t interruptible;
};

struct lttng_kernel_event_desc {
	const char *event_name;		/* lttng-modules name */
	const char *event_kname;	/* Linux kernel name (tracepoints) */
	const struct lttng_kernel_probe_desc *probe_desc;
	void (*probe_callback)(void);
	const struct lttng_kernel_event_field **fields;	/* event payload */
	unsigned int nr_fields;
	struct module *owner;
};

struct lttng_kernel_probe_desc {
	const char *provider_name;
	const struct lttng_kernel_event_desc **event_desc;
	unsigned int nr_events;
	struct list_head head;			/* chain registered probes */
	struct list_head lazy_init_head;
	int lazy;				/* lazy registration */
};

/*
 * Result of the run_filter() callback.
 */
enum lttng_kernel_event_filter_result {
	LTTNG_KERNEL_EVENT_FILTER_ACCEPT = 0,
	LTTNG_KERNEL_EVENT_FILTER_REJECT = 1,
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
	int (*run_filter)(const struct lttng_kernel_event_common *event,
		const char *stack_data,
		struct lttng_kernel_probe_ctx *probe_ctx,
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
		const char *stack_data,
		struct lttng_kernel_probe_ctx *probe_ctx,
		struct lttng_kernel_notification_ctx *notif_ctx);
};

struct lttng_kernel_channel_buffer_ops {
	struct lttng_kernel_channel_buffer_ops_private *priv;	/* Private channel buffer ops interface. */

	int (*event_reserve)(struct lib_ring_buffer_ctx *ctx);
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
	struct lttng_kernel_channel_buffer_ops ops;
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
	struct lttng_kernel_channel_buffer_ops *ops;
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
	struct lttng_kernel_channel_buffer_ops *ops;
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

int lttng_kernel_probe_register(struct lttng_kernel_probe_desc *desc);
void lttng_kernel_probe_unregister(struct lttng_kernel_probe_desc *desc);

bool lttng_id_tracker_lookup(struct lttng_id_tracker_rcu *p, int id);

#endif /* _LTTNG_EVENTS_H */
