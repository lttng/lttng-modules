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
#include <linux/uprobes.h>

#include <lttng/cpuhotplug.h>
#include <lttng/tracer.h>
#include <lttng/abi.h>
#include <lttng/abi-old.h>
#include <lttng/endian.h>

#define lttng_is_signed_type(type)	(((type) -1) < (type) 1)

struct lttng_kernel_channel_buffer;
struct lttng_kernel_channel_counter;
struct lttng_kernel_session;
struct lttng_kernel_ring_buffer_ctx;

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
		reverse_byte_order:1,
		user:1;			/* fetch from user-space */
	unsigned int base;		/* 2, 8, 10, 16, for pretty print */
};

struct lttng_kernel_type_string {
	struct lttng_kernel_type_common parent;
	enum lttng_kernel_string_encoding encoding;
	unsigned int user:1;		/* fetch from user-space */
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
	unsigned int alignment;		/* Alignment in bytes before elements. */
	enum lttng_kernel_string_encoding encoding;
};

struct lttng_kernel_type_sequence {
	struct lttng_kernel_type_common parent;
	const char *length_name;	/* Length field name. If NULL, use previous field. */
	const struct lttng_kernel_type_common *elem_type;
	unsigned int alignment;		/* Alignment in bytes before elements. */
	enum lttng_kernel_string_encoding encoding;
};

struct lttng_kernel_type_struct {
	struct lttng_kernel_type_common parent;
	unsigned int nr_fields;
	const struct lttng_kernel_event_field * const *fields;	/* Array of pointers to fields. */
	unsigned int alignment;		/* Alignment in bits */
};

struct lttng_kernel_type_variant {
	struct lttng_kernel_type_common parent;
	const char *tag_name;		/* Tag field name. If NULL, use previous field. */
	const struct lttng_kernel_event_field * const *choices; /* Array of pointers to fields. */
	unsigned int nr_choices;
	unsigned int alignment;		/* Alignment in bytes */
};

struct lttng_kernel_enum_desc {
	const char *name;
	const struct lttng_kernel_enum_entry * const *entries;
	unsigned int nr_entries;
	const struct lttng_kernel_probe_desc *probe_desc;
};

/* Event field description */

struct lttng_kernel_event_field {
	const char *name;
	const struct lttng_kernel_type_common *type;
	unsigned int nowrite:1,		/* do not write into trace */
			nofilter:1;	/* do not consider for filter */
};

#ifndef PARAMS
#define PARAMS(args...)	args
#endif

#define _lttng_kernel_static_type_integer(_size, _alignment, _signedness, _byte_order, _user, _base)	\
	((const struct lttng_kernel_type_common *) __LTTNG_COMPOUND_LITERAL(const struct lttng_kernel_type_integer, { \
		.parent = {										\
			.type = lttng_kernel_type_integer,							\
		},											\
		.size = (_size),									\
		.alignment = (_alignment),								\
		.signedness = (_signedness),								\
		.reverse_byte_order = (_byte_order) != __BYTE_ORDER,					\
		.user = (_user),									\
		.base = (_base),									\
	}))

#define lttng_kernel_static_type_integer(_size, _alignment, _signedness, _byte_order, _base)		\
	_lttng_kernel_static_type_integer(_size, _alignment, _signedness, _byte_order, 0, _base)

#define lttng_kernel_static_type_user_integer(_size, _alignment, _signedness, _byte_order, _base)	\
	_lttng_kernel_static_type_integer(_size, _alignment, _signedness, _byte_order, 1, _base)

#define _lttng_kernel_static_type_integer_from_type(_type, _byte_order, _user, _base)			\
	_lttng_kernel_static_type_integer(sizeof(_type) * CHAR_BIT,					\
			lttng_alignof(_type) * CHAR_BIT,						\
			lttng_is_signed_type(_type),							\
			_byte_order, _user, _base)

#define lttng_kernel_static_type_integer_from_type(_type, _byte_order, _base)				\
	_lttng_kernel_static_type_integer_from_type(_type, _byte_order, 0, _base)

#define lttng_kernel_static_type_user_integer_from_type(_type, _byte_order, _base)			\
	_lttng_kernel_static_type_integer_from_type(_type, _byte_order, 1, _base)

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

#define lttng_kernel_static_type_string(_encoding, _user)						\
	((const struct lttng_kernel_type_common *) __LTTNG_COMPOUND_LITERAL(const struct lttng_kernel_type_string, { \
		.parent = {										\
			.type = lttng_kernel_type_string,						\
		},											\
		.encoding = lttng_kernel_string_encoding_##_encoding,					\
		.user = (_user),									\
	}))

#define lttng_kernel_static_type_struct_init(_nr_fields, _fields, _alignment)				\
	{												\
		.parent = {										\
			.type = lttng_kernel_type_struct,						\
		},											\
		.nr_fields = (_nr_fields),								\
		.fields = _fields,									\
		.alignment = (_alignment),								\
	}

#define lttng_kernel_static_type_struct(_nr_fields, _fields, _alignment)				\
	((const struct lttng_kernel_type_common *) __LTTNG_COMPOUND_LITERAL(const struct lttng_kernel_type_struct, \
		lttng_kernel_static_type_struct_init(_nr_fields, PARAMS(_fields), _alignment)			\
	))

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

#define lttng_kernel_static_event_field(_name, _type, _nowrite, _nofilter)				\
	__LTTNG_COMPOUND_LITERAL(const struct lttng_kernel_event_field, {				\
		.name = (_name),									\
		.type = (_type),									\
		.nowrite = (_nowrite),									\
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

struct lttng_kernel_tracepoint_class {
	void (*probe_callback)(void);
	const struct lttng_kernel_event_field * const *fields;	/* event payload */
	unsigned int nr_fields;
	const struct lttng_kernel_probe_desc *probe_desc;
};

struct lttng_kernel_event_desc {
	const char *event_name;		/* lttng-modules name */
	const char *event_kname;	/* Linux kernel name (tracepoints) */
	const struct lttng_kernel_probe_desc *probe_desc;
	const struct lttng_kernel_tracepoint_class *tp_class;
	struct module *owner;
};

struct lttng_kernel_probe_desc {
	const char *provider_name;
	const struct lttng_kernel_event_desc * const *event_desc;
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
	LTTNG_KERNEL_EVENT_TYPE_COUNTER = 2
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

	struct lttng_kernel_channel_buffer *chan;
};

struct lttng_kernel_event_counter_ctx {
	int args_available;
};

struct lttng_kernel_event_counter_private;

struct lttng_kernel_event_counter {
	struct lttng_kernel_event_common parent;
	struct lttng_kernel_event_counter_private *priv;	/* Private event counter interface */

	struct lttng_kernel_channel_counter *chan;

	int use_args;						/* Use input arguments. */
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

	int (*event_reserve)(struct lttng_kernel_ring_buffer_ctx *ctx);
	void (*event_commit)(struct lttng_kernel_ring_buffer_ctx *ctx);
	void (*event_write)(struct lttng_kernel_ring_buffer_ctx *ctx, const void *src,
			    size_t len, size_t alignment);
	void (*event_write_from_user)(struct lttng_kernel_ring_buffer_ctx *ctx,
				      const void *src, size_t len, size_t alignment);
	void (*event_memset)(struct lttng_kernel_ring_buffer_ctx *ctx,
			     int c, size_t len);
	void (*event_strcpy)(struct lttng_kernel_ring_buffer_ctx *ctx, const char *src,
			     size_t len);
	void (*event_strcpy_from_user)(struct lttng_kernel_ring_buffer_ctx *ctx,
				       const char __user *src, size_t len);
	void (*event_pstrcpy_pad)(struct lttng_kernel_ring_buffer_ctx *ctx,
				  const char *src, size_t len);
	void (*event_pstrcpy_pad_from_user)(struct lttng_kernel_ring_buffer_ctx *ctx,
				  const char __user *src, size_t len);
	void (*lost_event_too_big)(struct lttng_kernel_channel_buffer *lttng_channel);
};

enum lttng_kernel_channel_type {
	LTTNG_KERNEL_CHANNEL_TYPE_BUFFER = 0,
	LTTNG_KERNEL_CHANNEL_TYPE_COUNTER = 1,
};

struct lttng_kernel_channel_common_private;

/* Use container_of() to get child. */
struct lttng_kernel_channel_common {
	struct lttng_kernel_channel_common_private *priv;	/* Private channel interface. */

	enum lttng_kernel_channel_type type;

	int enabled;
	struct lttng_kernel_session *session;
};

struct lttng_kernel_channel_buffer_private;

struct lttng_kernel_channel_buffer {
	struct lttng_kernel_channel_common parent;
	struct lttng_kernel_channel_buffer_private *priv;

	struct lttng_kernel_channel_buffer_ops *ops;
};

struct lttng_kernel_channel_counter;
struct lttng_kernel_channel_counter_ops_private;

struct lttng_kernel_channel_counter_ops {
	struct lttng_kernel_channel_counter_ops_private *priv;	/* Private channel counter ops interface */

	int (*counter_hit)(struct lttng_kernel_event_counter *event_counter,
		const char *stack_data,
		struct lttng_kernel_probe_ctx *probe_ctx,
		struct lttng_kernel_event_counter_ctx *event_counter_ctx);
};

struct lttng_kernel_channel_counter {
	struct lttng_kernel_channel_common parent;
	struct lttng_kernel_channel_counter_private *priv;	/* Private channel counter interface */

	struct lttng_kernel_channel_counter_ops *ops;
};

#define LTTNG_DYNAMIC_LEN_STACK_SIZE	128

struct lttng_dynamic_len_stack {
	size_t stack[LTTNG_DYNAMIC_LEN_STACK_SIZE];
	size_t offset;
};

DECLARE_PER_CPU(struct lttng_dynamic_len_stack, lttng_dynamic_len_stack);

/*
 * struct lttng_kernel_id_tracker declared in header due to deferencing of *v
 * in RCU_INITIALIZER(v).
 */
#define LTTNG_ID_HASH_BITS	6
#define LTTNG_ID_TABLE_SIZE	(1 << LTTNG_ID_HASH_BITS)

struct lttng_kernel_id_tracker_rcu {
	struct hlist_head id_hash[LTTNG_ID_TABLE_SIZE];
};

struct lttng_kernel_id_tracker {
	struct lttng_kernel_id_tracker_private *priv;	/* Private API */

	struct lttng_kernel_id_tracker_rcu *p;	/* RCU dereferenced. */
};

struct lttng_kernel_session_private;

struct lttng_kernel_session {
	struct lttng_kernel_session_private *priv;	/* Private session interface */

	int active;			/* Is trace session active ? */

	struct lttng_kernel_id_tracker pid_tracker;
	struct lttng_kernel_id_tracker vpid_tracker;
	struct lttng_kernel_id_tracker uid_tracker;
	struct lttng_kernel_id_tracker vuid_tracker;
	struct lttng_kernel_id_tracker gid_tracker;
	struct lttng_kernel_id_tracker vgid_tracker;
};

int lttng_kernel_probe_register(struct lttng_kernel_probe_desc *desc);
void lttng_kernel_probe_unregister(struct lttng_kernel_probe_desc *desc);

bool lttng_id_tracker_lookup(struct lttng_kernel_id_tracker_rcu *p, int id);

static inline
struct lttng_kernel_channel_common *lttng_kernel_get_chan_common_from_event_common(
		struct lttng_kernel_event_common *event)
{
	switch (event->type) {
	case LTTNG_KERNEL_EVENT_TYPE_RECORDER:
	{
		struct lttng_kernel_event_recorder *event_recorder =
			container_of(event, struct lttng_kernel_event_recorder, parent);
		struct lttng_kernel_channel_buffer *chan_buf = event_recorder->chan;

		return &chan_buf->parent;
	}
	case LTTNG_KERNEL_EVENT_TYPE_COUNTER:
	{
		struct lttng_kernel_event_counter *event_counter =
			container_of(event, struct lttng_kernel_event_counter, parent);
		struct lttng_kernel_channel_counter *chan_counter = event_counter->chan;

		return &chan_counter->parent;
	}
	default:
		return NULL;
	}
}

#endif /* _LTTNG_EVENTS_H */
