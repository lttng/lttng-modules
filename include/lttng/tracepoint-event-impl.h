/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng/tracepoint-event-impl.h
 *
 * Copyright (C) 2009 Steven Rostedt <rostedt@goodmis.org>
 * Copyright (C) 2009-2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/uaccess.h>
#include <linux/debugfs.h>
#include <linux/rculist.h>
#include <asm/byteorder.h>
#include <linux/swab.h>
#include <linux/user_namespace.h>

#include <wrapper/vmalloc.h>	/* for wrapper_vmalloc_sync_mappings() */
#include <ringbuffer/frontend_types.h>
#include <ringbuffer/backend.h>
#include <wrapper/rcu.h>
#include <lttng/types.h>
#include <lttng/probe-user.h>
#include <lttng/events.h>
#include <lttng/tracer-core.h>
#include <lttng/tp-mempool.h>

#define __LTTNG_NULL_STRING	"(null)"

#undef PARAMS
#define PARAMS(args...)		args

/*
 * Macro declarations used for all stages.
 */

/*
 * LTTng name mapping macros. LTTng remaps some of the kernel events to
 * enforce name-spacing.
 */
#undef LTTNG_TRACEPOINT_EVENT_MAP
#define LTTNG_TRACEPOINT_EVENT_MAP(name, map, proto, args, fields) \
	LTTNG_TRACEPOINT_EVENT_CLASS(map,				\
			     PARAMS(proto),				\
			     PARAMS(args),				\
			     PARAMS(fields))				\
	LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(map, name, map, PARAMS(proto), PARAMS(args))

#undef LTTNG_TRACEPOINT_EVENT_MAP_NOARGS
#define LTTNG_TRACEPOINT_EVENT_MAP_NOARGS(name, map, fields)		\
	LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(map,			\
			     PARAMS(fields))				\
	LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS(map, name, map)

#undef LTTNG_TRACEPOINT_EVENT_CODE_MAP
#define LTTNG_TRACEPOINT_EVENT_CODE_MAP(name, map, proto, args, _locvar, _code_pre, fields, _code_post) \
	LTTNG_TRACEPOINT_EVENT_CLASS_CODE(map,				\
			     PARAMS(proto),				\
			     PARAMS(args),				\
			     PARAMS(_locvar),				\
			     PARAMS(_code_pre),				\
			     PARAMS(fields),				\
			     PARAMS(_code_post))			\
	LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(map, name, map, PARAMS(proto), PARAMS(args))

#undef LTTNG_TRACEPOINT_EVENT_CODE
#define LTTNG_TRACEPOINT_EVENT_CODE(name, proto, args, _locvar, _code_pre, fields, _code_post) \
	LTTNG_TRACEPOINT_EVENT_CODE_MAP(name, name,			\
			     PARAMS(proto),				\
			     PARAMS(args),				\
			     PARAMS(_locvar),				\
			     PARAMS(_code_pre),				\
			     PARAMS(fields),				\
			     PARAMS(_code_post))

/*
 * LTTNG_TRACEPOINT_EVENT_CLASS can be used to add a generic function
 * handlers for events. That is, if all events have the same parameters
 * and just have distinct trace points.  Each tracepoint can be defined
 * with LTTNG_TRACEPOINT_EVENT_INSTANCE and that will map the
 * LTTNG_TRACEPOINT_EVENT_CLASS to the tracepoint.
 *
 * LTTNG_TRACEPOINT_EVENT is a one to one mapping between tracepoint and
 * template.
 */

#undef LTTNG_TRACEPOINT_EVENT
#define LTTNG_TRACEPOINT_EVENT(name, proto, args, fields)		\
	LTTNG_TRACEPOINT_EVENT_MAP(name, name,				\
			PARAMS(proto),					\
			PARAMS(args),					\
			PARAMS(fields))

#undef LTTNG_TRACEPOINT_EVENT_NOARGS
#define LTTNG_TRACEPOINT_EVENT_NOARGS(name, fields)			\
	LTTNG_TRACEPOINT_EVENT_MAP_NOARGS(name, name, PARAMS(fields))

#undef LTTNG_TRACEPOINT_EVENT_INSTANCE
#define LTTNG_TRACEPOINT_EVENT_INSTANCE(template, name, proto, args)	\
	LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(template, name, name, PARAMS(proto), PARAMS(args))

#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(template, name)	\
	LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS(template, name, name)

#undef LTTNG_TRACEPOINT_EVENT_CLASS
#define LTTNG_TRACEPOINT_EVENT_CLASS(_name, _proto, _args, _fields) \
	LTTNG_TRACEPOINT_EVENT_CLASS_CODE(_name, PARAMS(_proto), PARAMS(_args), , , \
		PARAMS(_fields), )

#undef LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS
#define LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(_name, _fields) \
	LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS(_name, , , PARAMS(_fields), )


/*
 * Stage 1 of the trace events.
 *
 * Create dummy trace calls for each events, verifying that the LTTng module
 * instrumentation headers match the kernel arguments. Will be optimized
 * out by the compiler.
 */

/* Reset all macros within TRACEPOINT_EVENT */
#include <lttng/events-reset.h>

#undef TP_PROTO
#define TP_PROTO(...)	__VA_ARGS__

#undef TP_ARGS
#define TP_ARGS(...)	__VA_ARGS__

#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(_template, _name, _map, _proto, _args) \
void trace_##_name(_proto);

#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS(_template, _name, _map) \
void trace_##_name(void);

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)

/*
 * Stage 1.1 of the trace events.
 *
 * Create dummy trace prototypes for each event class, and for each used
 * template. This will allow checking whether the prototypes from the
 * class and the instance using the class actually match.
 */

#include <lttng/events-reset.h>	/* Reset all macros within TRACE_EVENT */

#undef TP_PROTO
#define TP_PROTO(...)	__VA_ARGS__

#undef TP_ARGS
#define TP_ARGS(...)	__VA_ARGS__

#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(_template, _name, _map, _proto, _args) \
void __event_template_proto___##_template(_proto);

#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS(_template, _name, _map) \
void __event_template_proto___##_template(void);

#undef LTTNG_TRACEPOINT_EVENT_CLASS_CODE
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE(_name, _proto, _args, _locvar, _code_pre, _fields, _code_post) \
void __event_template_proto___##_name(_proto);

#undef LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS(_name, _locvar, _code_pre, _fields, _code_post) \
void __event_template_proto___##_name(void);

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)

/*
 * Stage 1.2 of tracepoint event generation
 *
 * Unfolding the enums
 */
#include <lttng/events-reset.h>	/* Reset all macros within TRACE_EVENT */

/* Enumeration entry (single value) */
#undef ctf_enum_value
#define ctf_enum_value(_string, _value)					\
	lttng_kernel_static_enum_entry_value(_string, _value)

/* Enumeration entry (range) */
#undef ctf_enum_range
#define ctf_enum_range(_string, _range_start, _range_end)		\
	lttng_kernel_static_enum_entry_range(_string, _range_start, _range_end)

/* Enumeration entry (automatic value; follows the rules of CTF) */
#undef ctf_enum_auto
#define ctf_enum_auto(_string)						\
	lttng_kernel_static_enum_entry_auto(_string)

#undef TP_ENUM_VALUES
#define TP_ENUM_VALUES(...)						\
	__VA_ARGS__

#ifndef LTTNG_TRACEPOINT_TYPE_EXTERN
# undef LTTNG_TRACEPOINT_ENUM
# define LTTNG_TRACEPOINT_ENUM(_name, _values)				\
	static const struct lttng_kernel_enum_entry * const __enum_values__##_name[] = { \
		_values							\
	};								\
	static struct lttng_kernel_enum_entry __enum_values_sorted__##_name[ARRAY_SIZE(__enum_values__##_name)];

# undef LTTNG_TRACEPOINT_ENUM_TAG
# define LTTNG_TRACEPOINT_ENUM_TAG(_name, _values)			\
	static const struct lttng_kernel_enum_entry * const __enum_values__##_name[] = { \
		_values							\
	};								\
	static struct lttng_kernel_enum_entry __enum_values_sorted__##_name[ARRAY_SIZE(__enum_values__##_name)];
#endif

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)

/*
 * Stage 1.3 of the trace events.
 *
 * Create probe callback prototypes.
 */

/* Reset all macros within TRACEPOINT_EVENT */
#include <lttng/events-reset.h>

#undef TP_PROTO
#define TP_PROTO(...)	__VA_ARGS__

#undef LTTNG_TRACEPOINT_EVENT_CLASS_CODE
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE(_name, _proto, _args, _locvar, _code_pre, _fields, _code_post) \
static void __event_probe__##_name(void *__data, _proto);

#undef LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS(_name, _locvar, _code_pre, _fields, _code_post) \
static void __event_probe__##_name(void *__data);

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)

/*
 * Stage 1.4 of tracepoint event generation.
 *
 * Declare toplevel descriptor for the whole probe.
 */

#define TP_ID1(_token, _system)	_token##_system
#define TP_ID(_token, _system)	TP_ID1(_token, _system)

static __used struct lttng_kernel_probe_desc TP_ID(__probe_desc___, TRACE_SYSTEM);

#undef TP_ID1
#undef TP_ID

/*
 * Stage 2 of the trace events.
 *
 * Create event field type metadata section.
 * Each event produce an array of fields.
 */

/* Reset all macros within TRACEPOINT_EVENT */
#include <lttng/events-reset.h>
#include <lttng/events-write.h>
#include <lttng/events-nowrite.h>

#undef _ctf_integer_ext
#define _ctf_integer_ext(_type, _item, _src, _byte_order, _base, _user, _nowrite)	\
	lttng_kernel_static_event_field(#_item,						\
		_lttng_kernel_static_type_integer_from_type(_type, _byte_order, _user, _base),	\
		_nowrite, 0),

#undef _ctf_array_encoded
#define _ctf_array_encoded(_type, _item, _src, _length,		\
		_encoding, _byte_order, _elem_type_base, _user, _nowrite) \
	lttng_kernel_static_event_field(#_item,			\
		lttng_kernel_static_type_array(_length,		\
			_lttng_kernel_static_type_integer_from_type(_type, _byte_order, _user, _elem_type_base), \
			0,					\
			_encoding),				\
		_nowrite, 0),

#undef _ctf_array_bitfield
#define _ctf_array_bitfield(_type, _item, _src, _length, _user, _nowrite) \
	lttng_kernel_static_event_field(#_item,			\
		lttng_kernel_static_type_array((_length) * sizeof(_type) * CHAR_BIT, \
			_lttng_kernel_static_type_integer(1, 1, 0, __LITTLE_ENDIAN, _user, 10), \
			lttng_alignof(_type),			\
			none),					\
		_nowrite, 0),

#undef _ctf_sequence_encoded
#define _ctf_sequence_encoded(_type, _item, _src,		\
			_length_type, _src_length, _encoding,	\
			_byte_order, _elem_type_base, _user, _nowrite) \
	lttng_kernel_static_event_field("_" #_item "_length",	\
		lttng_kernel_static_type_integer_from_type(_length_type, __BYTE_ORDER, 10), \
		_nowrite, 1),					\
	lttng_kernel_static_event_field(#_item,			\
		lttng_kernel_static_type_sequence(NULL,	/* Use previous field. */ \
			_lttng_kernel_static_type_integer_from_type(_type, _byte_order, _user, _elem_type_base), \
			0,					\
			_encoding),				\
		_nowrite, 0),

#undef _ctf_sequence_bitfield
#define _ctf_sequence_bitfield(_type, _item, _src,		\
			_length_type, _src_length,		\
			_user, _nowrite)			\
	lttng_kernel_static_event_field("_" #_item "_length",	\
		lttng_kernel_static_type_integer_from_type(_length_type, __BYTE_ORDER, 10), \
		_nowrite, 1),					\
	lttng_kernel_static_event_field(#_item,			\
		lttng_kernel_static_type_sequence(NULL,	/* Use previous field. */ \
			_lttng_kernel_static_type_integer(1, 1, 0, __LITTLE_ENDIAN, _user, 10), \
			lttng_alignof(_type),			\
			none),					\
		_nowrite, 0),

#undef _ctf_string
#define _ctf_string(_item, _src, _user, _nowrite)		\
	lttng_kernel_static_event_field(#_item,			\
		lttng_kernel_static_type_string(UTF8, _user),	\
		_nowrite, 0),

#undef _ctf_unused
#define _ctf_unused(_src)

#undef _ctf_enum
#define _ctf_enum(_name, _type, _item, _src, _user, _nowrite)	\
	lttng_kernel_static_event_field(#_item,			\
		lttng_kernel_static_type_enum(&__enum_##_name,	\
			_lttng_kernel_static_type_integer_from_type(_type, __BYTE_ORDER, _user, 10)), \
		_nowrite, 0),

#undef ctf_custom_field
#define ctf_custom_field(_type, _item, _code)			\
	lttng_kernel_static_event_field(#_item, PARAMS(_type), 0, 1),

#undef ctf_custom_type
#define ctf_custom_type(...)	__VA_ARGS__

#undef TP_FIELDS
#define TP_FIELDS(...)	__VA_ARGS__	/* Only one used in this phase */

#ifndef TP_PROBE_CB
#define TP_PROBE_CB(_template)	&__event_probe__##_template
#endif

#undef LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS(_name, _locvar, _code_pre, _fields, _code_post) \
	static const struct lttng_kernel_event_field * const __event_fields___##_name[] = { \
		_fields							     \
	};								     \
	static const struct lttng_kernel_tracepoint_class lttng_kernel__event_class___##_name = { \
		.fields = __event_fields___##_name,			     \
		.nr_fields = ARRAY_SIZE(__event_fields___##_name),	     \
		.probe_callback = (void (*)(void)) TP_PROBE_CB(_name),	     \
		.probe_desc = &TP_ID(__probe_desc___, TRACE_SYSTEM),	     \
	};

#undef LTTNG_TRACEPOINT_EVENT_CLASS_CODE
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE(_name, _proto, _args, _locvar, _code_pre, _fields, _code_post) \
	LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS(_name, _locvar, _code_pre, PARAMS(_fields), _code_post)

#ifdef LTTNG_TRACEPOINT_TYPE_EXTERN
# undef LTTNG_TRACEPOINT_TYPE
# define LTTNG_TRACEPOINT_TYPE(_prototype, _init)	\
	extern _prototype;
#elif defined (LTTNG_TRACEPOINT_TYPE_DEFINE_EXPORT)
# define LTTNG_TRACEPOINT_TYPE_VISIBILITY __attribute__((visibility("hidden")))
#else
# define LTTNG_TRACEPOINT_TYPE_VISIBILITY static
#endif

#ifdef LTTNG_TRACEPOINT_TYPE_VISIBILITY
# undef LTTNG_TRACEPOINT_TYPE
# define LTTNG_TRACEPOINT_TYPE(_prototype, _init)	\
	LTTNG_TRACEPOINT_TYPE_VISIBILITY _prototype = _init;
#endif

# undef LTTNG_TRACEPOINT_ENUM
# define LTTNG_TRACEPOINT_ENUM(_name, _values)						\
	LTTNG_TRACEPOINT_TYPE(PARAMS(const struct lttng_kernel_enum_desc __enum_##_name), \
	PARAMS({									\
                .name = #_name,                                                         \
                .entries = __enum_values__##_name,                                      \
                .sorted_entries = __enum_values_sorted__##_name,                        \
                .nr_entries = ARRAY_SIZE(__enum_values__##_name),                       \
		.probe_desc = &TP_ID(__probe_desc___, TRACE_SYSTEM),			\
		.is_tag = 0,								\
        }))

# undef LTTNG_TRACEPOINT_ENUM_TAG
# define LTTNG_TRACEPOINT_ENUM_TAG(_name, _values)					\
	LTTNG_TRACEPOINT_TYPE(PARAMS(const struct lttng_kernel_enum_desc __enum_##_name), \
	PARAMS({									\
                .name = #_name,                                                         \
                .entries = __enum_values__##_name,                                      \
                .sorted_entries = __enum_values_sorted__##_name,                        \
                .nr_entries = ARRAY_SIZE(__enum_values__##_name),                       \
                .probe_desc = &TP_ID(__probe_desc___, TRACE_SYSTEM),			\
                .is_tag = 1,								\
        }))

#define TP_ID1(_token, _system)	_token##_system
#define TP_ID(_token, _system)	TP_ID1(_token, _system)

#define LTTNG_CREATE_FIELD_METADATA
#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)
#undef LTTNG_CREATE_FIELD_METADATA

#undef LTTNG_TRACEPOINT_TYPE_VISIBILITY

#undef TP_ID1
#undef TP_ID

/*
 * Stage 4 of the trace events.
 *
 * Create static inline function that calculates event size.
 */

/* Reset all macros within TRACEPOINT_EVENT */
#include <lttng/events-reset.h>
#include <lttng/events-write.h>

#undef _ctf_integer_ext
#define _ctf_integer_ext(_type, _item, _src, _byte_order, _base, _user, _nowrite) \
	__event_len += lib_ring_buffer_align(__event_len, lttng_alignof(_type)); \
	__event_len += sizeof(_type);

#undef _ctf_array_encoded
#define _ctf_array_encoded(_type, _item, _src, _length, _encoding, _byte_order, _base, _user, _nowrite) \
	__event_len += lib_ring_buffer_align(__event_len, lttng_alignof(_type)); \
	__event_len += sizeof(_type) * (_length);

#undef _ctf_array_bitfield
#define _ctf_array_bitfield(_type, _item, _src, _length, _user, _nowrite) \
	_ctf_array_encoded(_type, _item, _src, _length, none, __LITTLE_ENDIAN, 0, _user, _nowrite)

#undef _ctf_sequence_encoded
#define _ctf_sequence_encoded(_type, _item, _src, _length_type,			\
			_src_length, _encoding, _byte_order, _base, _user, _nowrite) \
	__event_len += lib_ring_buffer_align(__event_len, lttng_alignof(_length_type)); \
	__event_len += sizeof(_length_type);				       \
	__event_len += lib_ring_buffer_align(__event_len, lttng_alignof(_type)); \
	{										\
		size_t __seqlen = (_src_length);					\
											\
		if (unlikely(++this_cpu_ptr(&lttng_dynamic_len_stack)->offset >= LTTNG_DYNAMIC_LEN_STACK_SIZE)) \
			goto error;							\
		barrier();	/* reserve before use. */				\
		this_cpu_ptr(&lttng_dynamic_len_stack)->stack[this_cpu_ptr(&lttng_dynamic_len_stack)->offset - 1] = __seqlen; \
		__event_len += sizeof(_type) * __seqlen;				\
	}

#undef _ctf_sequence_bitfield
#define _ctf_sequence_bitfield(_type, _item, _src,		\
			_length_type, _src_length,		\
			_user, _nowrite)			\
	_ctf_sequence_encoded(_type, _item, _src, _length_type, _src_length, \
		none, __LITTLE_ENDIAN, 10, _user, _nowrite)

/*
 * ctf_user_string includes \0. If returns 0, it faulted, so we set size to
 * 1 (\0 only).
 */
#undef _ctf_string
#define _ctf_string(_item, _src, _user, _nowrite)			       \
	if (unlikely(++this_cpu_ptr(&lttng_dynamic_len_stack)->offset >= LTTNG_DYNAMIC_LEN_STACK_SIZE)) \
		goto error;						       \
	barrier();	/* reserve before use. */			       \
	if (_user) {							       \
		__event_len += this_cpu_ptr(&lttng_dynamic_len_stack)->stack[this_cpu_ptr(&lttng_dynamic_len_stack)->offset - 1] = \
			max_t(size_t, lttng_strlen_user_inatomic(_src), 1);    \
	} else {							       \
		__event_len += this_cpu_ptr(&lttng_dynamic_len_stack)->stack[this_cpu_ptr(&lttng_dynamic_len_stack)->offset - 1] = \
			strlen((_src) ? (_src) : __LTTNG_NULL_STRING) + 1; \
	}

#undef _ctf_enum
#define _ctf_enum(_name, _type, _item, _src, _user, _nowrite)		       \
	_ctf_integer_ext(_type, _item, _src, __BYTE_ORDER, 10, _user, _nowrite)

#undef ctf_align
#define ctf_align(_type)						\
	__event_len += lib_ring_buffer_align(__event_len, lttng_alignof(_type));

#undef ctf_custom_field
#define ctf_custom_field(_type, _item, _code)				\
	{								\
		_code							\
	}

#undef ctf_custom_code
#define ctf_custom_code(...)		__VA_ARGS__

#undef TP_PROTO
#define TP_PROTO(...)	__VA_ARGS__

#undef TP_FIELDS
#define TP_FIELDS(...)	__VA_ARGS__

#undef TP_locvar
#define TP_locvar(...)	__VA_ARGS__

#undef LTTNG_TRACEPOINT_EVENT_CLASS_CODE
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE(_name, _proto, _args, _locvar, _code_pre, _fields, _code_post) \
static inline ssize_t __event_get_size__##_name(void *__tp_locvar, _proto)     \
{									      \
	size_t __event_len = 0;						      \
	unsigned int __dynamic_len_idx __attribute__((unused)) = 0;	      \
	struct { _locvar } *tp_locvar __attribute__((unused)) = __tp_locvar;  \
									      \
	_fields								      \
	return __event_len;						      \
									      \
error:									      \
	__attribute__((unused));					      \
	return -1;							      \
}

#undef LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS(_name, _locvar, _code_pre, _fields, _code_post) \
static inline ssize_t __event_get_size__##_name(void *__tp_locvar)	      \
{									      \
	size_t __event_len = 0;						      \
	unsigned int __dynamic_len_idx __attribute__((unused)) = 0;	      \
	struct { _locvar } *tp_locvar __attribute__((unused)) = __tp_locvar;  \
									      \
	_fields								      \
	return __event_len;						      \
									      \
error:									      \
	__attribute__((unused));					      \
	return -1;							      \
}

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)


/*
 * Stage 4.1 of tracepoint event generation.
 *
 * Create static inline function that layout the filter stack data.
 * We make both write and nowrite data available to the filter.
 */

/* Reset all macros within TRACEPOINT_EVENT */
#include <lttng/events-reset.h>
#include <lttng/events-write.h>
#include <lttng/events-nowrite.h>

#undef _ctf_integer_ext_fetched
#define _ctf_integer_ext_fetched(_type, _item, _src, _byte_order, _base, _nowrite) \
	if (lttng_is_signed_type(_type)) {				       \
		int64_t __ctf_tmp_int64;				       \
		switch (sizeof(_type)) {				       \
		case 1:							       \
		{							       \
			union { _type t; int8_t v; } __tmp = { (_type) (_src) }; \
			__ctf_tmp_int64 = (int64_t) __tmp.v;		       \
			break;						       \
		}							       \
		case 2:							       \
		{							       \
			union { _type t; int16_t v; } __tmp = { (_type) (_src) }; \
			if (_byte_order != __BYTE_ORDER)		       \
				__swab16s(&__tmp.v);			       \
			__ctf_tmp_int64 = (int64_t) __tmp.v;		       \
			break;						       \
		}							       \
		case 4:							       \
		{							       \
			union { _type t; int32_t v; } __tmp = { (_type) (_src) }; \
			if (_byte_order != __BYTE_ORDER)		       \
				__swab32s(&__tmp.v);			       \
			__ctf_tmp_int64 = (int64_t) __tmp.v;		       \
			break;						       \
		}							       \
		case 8:							       \
		{							       \
			union { _type t; int64_t v; } __tmp = { (_type) (_src) }; \
			if (_byte_order != __BYTE_ORDER)		       \
				__swab64s(&__tmp.v);			       \
			__ctf_tmp_int64 = (int64_t) __tmp.v;		       \
			break;						       \
		}							       \
		default:						       \
			BUG_ON(1);					       \
		};							       \
		memcpy(__stack_data, &__ctf_tmp_int64, sizeof(int64_t));       \
	} else {							       \
		uint64_t __ctf_tmp_uint64;				       \
		switch (sizeof(_type)) {				       \
		case 1:							       \
		{							       \
			union { _type t; uint8_t v; } __tmp = { (_type) (_src) }; \
			__ctf_tmp_uint64 = (uint64_t) __tmp.v;		       \
			break;						       \
		}							       \
		case 2:							       \
		{							       \
			union { _type t; uint16_t v; } __tmp = { (_type) (_src) }; \
			if (_byte_order != __BYTE_ORDER)		       \
				__swab16s(&__tmp.v);			       \
			__ctf_tmp_uint64 = (uint64_t) __tmp.v;		       \
			break;						       \
		}							       \
		case 4:							       \
		{							       \
			union { _type t; uint32_t v; } __tmp = { (_type) (_src) }; \
			if (_byte_order != __BYTE_ORDER)		       \
				__swab32s(&__tmp.v);			       \
			__ctf_tmp_uint64 = (uint64_t) __tmp.v;		       \
			break;						       \
		}							       \
		case 8:							       \
		{							       \
			union { _type t; uint64_t v; } __tmp = { (_type) (_src) }; \
			if (_byte_order != __BYTE_ORDER)		       \
				__swab64s(&__tmp.v);			       \
			__ctf_tmp_uint64 = (uint64_t) __tmp.v;		       \
			break;						       \
		}							       \
		default:						       \
			BUG_ON(1);					       \
		};							       \
		memcpy(__stack_data, &__ctf_tmp_uint64, sizeof(uint64_t));     \
	}								       \
	__stack_data += sizeof(int64_t);

#undef _ctf_integer_ext_isuser0
#define _ctf_integer_ext_isuser0(_type, _item, _src, _byte_order, _base, _nowrite) \
	_ctf_integer_ext_fetched(_type, _item, _src, _byte_order, _base, _nowrite)

#undef _ctf_integer_ext_isuser1
#define _ctf_integer_ext_isuser1(_type, _item, _user_src, _byte_order, _base, _nowrite) \
{											\
	union {										\
		char __array[sizeof(_user_src)];					\
		__typeof__(_user_src) __v;						\
	} __tmp_fetch;									\
	if (lib_ring_buffer_copy_from_user_check_nofault(__tmp_fetch.__array,		\
				&(_user_src), sizeof(_user_src))) 			\
		memset(__tmp_fetch.__array, 0, sizeof(__tmp_fetch.__array));		\
	_ctf_integer_ext_fetched(_type, _item, __tmp_fetch.__v, _byte_order, _base, _nowrite) \
}

#undef _ctf_integer_ext
#define _ctf_integer_ext(_type, _item, _user_src, _byte_order, _base, _user, _nowrite) \
	_ctf_integer_ext_isuser##_user(_type, _item, _user_src, _byte_order, _base, _nowrite)

#undef _ctf_array_encoded
#define _ctf_array_encoded(_type, _item, _src, _length, _encoding, _byte_order, _base, _user, _nowrite) \
	{								       \
		unsigned long __ctf_tmp_ulong = (unsigned long) (_length);     \
		const void *__ctf_tmp_ptr = (_src);			       \
		memcpy(__stack_data, &__ctf_tmp_ulong, sizeof(unsigned long)); \
		__stack_data += sizeof(unsigned long);			       \
		memcpy(__stack_data, &__ctf_tmp_ptr, sizeof(void *));	       \
		__stack_data += sizeof(void *);				       \
	}

#undef _ctf_array_bitfield
#define _ctf_array_bitfield(_type, _item, _src, _length, _user, _nowrite) \
	_ctf_array_encoded(_type, _item, _src, _length, none, __LITTLE_ENDIAN, 0, _user, _nowrite)

#undef _ctf_sequence_encoded
#define _ctf_sequence_encoded(_type, _item, _src, _length_type,		       \
			_src_length, _encoding, _byte_order, _base, _user, _nowrite) \
	{								       \
		unsigned long __ctf_tmp_ulong = (unsigned long) (_src_length); \
		const void *__ctf_tmp_ptr = (_src);			       \
		memcpy(__stack_data, &__ctf_tmp_ulong, sizeof(unsigned long)); \
		__stack_data += sizeof(unsigned long);			       \
		memcpy(__stack_data, &__ctf_tmp_ptr, sizeof(void *));	       \
		__stack_data += sizeof(void *);				       \
	}

#undef _ctf_sequence_bitfield
#define _ctf_sequence_bitfield(_type, _item, _src,		\
			_length_type, _src_length,		\
			_user, _nowrite)			\
	_ctf_sequence_encoded(_type, _item, _src, _length_type, _src_length, \
		none, __LITTLE_ENDIAN, 10, _user, _nowrite)

#undef _ctf_string
#define _ctf_string(_item, _src, _user, _nowrite)			       \
	{								       \
		const void *__ctf_tmp_ptr =				       \
			((_src) ? (_src) : __LTTNG_NULL_STRING);	       \
		memcpy(__stack_data, &__ctf_tmp_ptr, sizeof(void *));	       \
		__stack_data += sizeof(void *);				       \
	}

#undef _ctf_enum
#define _ctf_enum(_name, _type, _item, _src, _user, _nowrite)		       \
	_ctf_integer_ext(_type, _item, _src, __BYTE_ORDER, 10, _user, _nowrite)

#undef TP_PROTO
#define TP_PROTO(...) __VA_ARGS__

#undef TP_FIELDS
#define TP_FIELDS(...) __VA_ARGS__

#undef TP_locvar
#define TP_locvar(...)	__VA_ARGS__

#undef LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS(_name, _locvar, _code_pre, _fields, _code_post) \
static inline								      \
void __event_prepare_interpreter_stack__##_name(char *__stack_data,		      \
		void *__tp_locvar)					      \
{									      \
	struct { _locvar } *tp_locvar __attribute__((unused)) = __tp_locvar;  \
									      \
	_fields								      \
}

#undef LTTNG_TRACEPOINT_EVENT_CLASS_CODE
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE(_name, _proto, _args, _locvar, _code_pre, _fields, _code_post) \
static inline								      \
void __event_prepare_interpreter_stack__##_name(char *__stack_data,		      \
		void *__tp_locvar, _proto)				      \
{									      \
	struct { _locvar } *tp_locvar __attribute__((unused)) = __tp_locvar;  \
									      \
	_fields								      \
}

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)

/*
 * Stage 5 of the trace events.
 *
 * Create static inline function that calculates event payload alignment.
 */

/* Reset all macros within TRACEPOINT_EVENT */
#include <lttng/events-reset.h>
#include <lttng/events-write.h>

#undef _ctf_integer_ext
#define _ctf_integer_ext(_type, _item, _src, _byte_order, _base, _user, _nowrite) \
	__event_align = max_t(size_t, __event_align, lttng_alignof(_type));

#undef _ctf_array_encoded
#define _ctf_array_encoded(_type, _item, _src, _length, _encoding, _byte_order, _base, _user, _nowrite) \
	__event_align = max_t(size_t, __event_align, lttng_alignof(_type));

#undef _ctf_array_bitfield
#define _ctf_array_bitfield(_type, _item, _src, _length, _user, _nowrite) \
	_ctf_array_encoded(_type, _item, _src, _length, none, __LITTLE_ENDIAN, 0, _user, _nowrite)

#undef _ctf_sequence_encoded
#define _ctf_sequence_encoded(_type, _item, _src, _length_type,			\
			_src_length, _encoding, _byte_order, _base, _user, _nowrite) \
	__event_align = max_t(size_t, __event_align, lttng_alignof(_length_type)); \
	__event_align = max_t(size_t, __event_align, lttng_alignof(_type));

#undef _ctf_sequence_bitfield
#define _ctf_sequence_bitfield(_type, _item, _src,		\
			_length_type, _src_length,		\
			_user, _nowrite)			\
	_ctf_sequence_encoded(_type, _item, _src, _length_type, _src_length, \
		none, __LITTLE_ENDIAN, 10, _user, _nowrite)

#undef _ctf_string
#define _ctf_string(_item, _src, _user, _nowrite)

#undef _ctf_enum
#define _ctf_enum(_name, _type, _item, _src, _user, _nowrite)	\
	_ctf_integer_ext(_type, _item, _src, __BYTE_ORDER, 10, _user, _nowrite)

#undef ctf_align
#define ctf_align(_type)						\
	__event_align = max_t(size_t, __event_align, lttng_alignof(_type));

#undef TP_PROTO
#define TP_PROTO(...)	__VA_ARGS__

#undef TP_FIELDS
#define TP_FIELDS(...)	__VA_ARGS__

#undef TP_locvar
#define TP_locvar(...)	__VA_ARGS__

#undef ctf_custom_field
#define ctf_custom_field(_type, _item, _code)		_code

#undef ctf_custom_code
#define ctf_custom_code(...)						\
	{								\
		__VA_ARGS__						\
	}

#undef LTTNG_TRACEPOINT_EVENT_CLASS_CODE
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE(_name, _proto, _args, _locvar, _code_pre, _fields, _code_post) \
static inline size_t __event_get_align__##_name(void *__tp_locvar, _proto)    \
{									      \
	size_t __event_align = 1;					      \
	struct { _locvar } *tp_locvar __attribute__((unused)) = __tp_locvar;  \
									      \
	_fields								      \
	return __event_align;						      \
}

#undef LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS(_name, _locvar, _code_pre, _fields, _code_post) \
static inline size_t __event_get_align__##_name(void *__tp_locvar)	      \
{									      \
	size_t __event_align = 1;					      \
	struct { _locvar } *tp_locvar __attribute__((unused)) = __tp_locvar;  \
									      \
	_fields								      \
	return __event_align;						      \
}

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)

/*
 * Stage 6 of tracepoint event generation.
 *
 * Create the probe function. This function calls event size calculation
 * and writes event data into the buffer.
 */

/* Reset all macros within TRACEPOINT_EVENT */
#include <lttng/events-reset.h>
#include <lttng/events-write.h>

#undef _ctf_integer_ext_fetched
#define _ctf_integer_ext_fetched(_type, _item, _src, _byte_order, _base, _nowrite) \
	{								\
		_type __tmp = _src;					\
		__chan->ops->event_write(&__ctx, &__tmp, sizeof(__tmp), lttng_alignof(__tmp)); \
	}

#undef _ctf_integer_ext_isuser0
#define _ctf_integer_ext_isuser0(_type, _item, _src, _byte_order, _base, _nowrite) \
	_ctf_integer_ext_fetched(_type, _item, _src, _byte_order, _base, _nowrite)

#undef _ctf_integer_ext_isuser1
#define _ctf_integer_ext_isuser1(_type, _item, _user_src, _byte_order, _base, _nowrite) \
{									       \
	union {										\
		char __array[sizeof(_user_src)];					\
		__typeof__(_user_src) __v;						\
	} __tmp_fetch;									\
	if (lib_ring_buffer_copy_from_user_check_nofault(__tmp_fetch.__array,		\
				&(_user_src), sizeof(_user_src))) 			\
		memset(__tmp_fetch.__array, 0, sizeof(__tmp_fetch.__array));		\
	_ctf_integer_ext_fetched(_type, _item, __tmp_fetch.__v, _byte_order, _base, _nowrite) \
}

#undef _ctf_integer_ext
#define _ctf_integer_ext(_type, _item, _user_src, _byte_order, _base, _user, _nowrite) \
	_ctf_integer_ext_isuser##_user(_type, _item, _user_src, _byte_order, _base, _nowrite)

#undef _ctf_array_encoded
#define _ctf_array_encoded(_type, _item, _src, _length, _encoding, _byte_order, _base, _user, _nowrite) \
	if (lttng_kernel_string_encoding_##_encoding == lttng_kernel_string_encoding_none) { \
		if (_user) {								\
			__chan->ops->event_write_from_user(&__ctx, _src, sizeof(_type) * (_length), lttng_alignof(_type)); \
		} else {								\
			__chan->ops->event_write(&__ctx, _src, sizeof(_type) * (_length), lttng_alignof(_type)); \
		}									\
	} else {									\
		if (_user) {								\
			__chan->ops->event_pstrcpy_pad_from_user(&__ctx, (const char __user *) (_src), _length); \
		} else {								\
			__chan->ops->event_pstrcpy_pad(&__ctx, (const char *) (_src), _length); \
		}									\
	}

#if (__BYTE_ORDER == __LITTLE_ENDIAN)
#undef _ctf_array_bitfield
#define _ctf_array_bitfield(_type, _item, _src, _length, _user, _nowrite) \
	if (_user) {							\
		__chan->ops->event_write_from_user(&__ctx, _src, sizeof(_type) * (_length), lttng_alignof(_type)); \
	} else {							\
		__chan->ops->event_write(&__ctx, _src, sizeof(_type) * (_length), lttng_alignof(_type)); \
	}
#else /* #if (__BYTE_ORDER == __LITTLE_ENDIAN) */
/*
 * For big endian, we need to byteswap into little endian.
 */
#undef _ctf_array_bitfield
#define _ctf_array_bitfield(_type, _item, _src, _length, _user, _nowrite) \
	lib_ring_buffer_align_ctx(&__ctx, lttng_alignof(_type));	\
	{								\
		size_t _i;						\
									\
		for (_i = 0; _i < (_length); _i++) {			\
			_type _tmp;					\
									\
			if (_user) {					\
				if (get_user(_tmp, (_type *) _src + _i)) \
					_tmp = 0;			\
			} else {					\
				_tmp = ((_type *) _src)[_i];		\
			}						\
			switch (sizeof(_type)) {			\
			case 1:						\
				break;					\
			case 2:						\
				_tmp = cpu_to_le16(_tmp);		\
				break;					\
			case 4:						\
				_tmp = cpu_to_le32(_tmp);		\
				break;					\
			case 8:						\
				_tmp = cpu_to_le64(_tmp);		\
				break;					\
			default:					\
				BUG_ON(1);				\
			}						\
			__chan->ops->event_write(&__ctx, &_tmp, sizeof(_type), 1); \
		}							\
	}
#endif /* #else #if (__BYTE_ORDER == __LITTLE_ENDIAN) */

#undef _ctf_sequence_encoded
#define _ctf_sequence_encoded(_type, _item, _src, _length_type,			\
			_src_length, _encoding, _byte_order, _base, _user, _nowrite) \
	{									\
		_length_type __tmpl = this_cpu_ptr(&lttng_dynamic_len_stack)->stack[__dynamic_len_idx]; \
		__chan->ops->event_write(&__ctx, &__tmpl, sizeof(_length_type), lttng_alignof(_length_type));\
	}									\
	if (lttng_kernel_string_encoding_##_encoding == lttng_kernel_string_encoding_none) { \
		if (_user) {							\
			__chan->ops->event_write_from_user(&__ctx, _src,	\
				sizeof(_type) * __get_dynamic_len(dest), lttng_alignof(_type));	\
		} else {							\
			__chan->ops->event_write(&__ctx, _src,			\
				sizeof(_type) * __get_dynamic_len(dest), lttng_alignof(_type));	\
		}								\
	} else {								\
		if (_user) {							\
			__chan->ops->event_pstrcpy_pad_from_user(&__ctx, (const char __user *) (_src), \
				__get_dynamic_len(dest));			\
		} else {							\
			__chan->ops->event_pstrcpy_pad(&__ctx, (const char *) (_src), \
				__get_dynamic_len(dest));			\
		}								\
	}

#if (__BYTE_ORDER == __LITTLE_ENDIAN)
#undef _ctf_sequence_bitfield
#define _ctf_sequence_bitfield(_type, _item, _src,		\
			_length_type, _src_length,		\
			_user, _nowrite)			\
	{								\
		_length_type __tmpl = this_cpu_ptr(&lttng_dynamic_len_stack)->stack[__dynamic_len_idx] * sizeof(_type) * CHAR_BIT; \
		__chan->ops->event_write(&__ctx, &__tmpl, sizeof(_length_type), lttng_alignof(_length_type)); \
	}								\
	if (_user) {							\
		__chan->ops->event_write_from_user(&__ctx, _src,	\
			sizeof(_type) * __get_dynamic_len(dest), lttng_alignof(_type));	\
	} else {							\
		__chan->ops->event_write(&__ctx, _src,			\
			sizeof(_type) * __get_dynamic_len(dest), lttng_alignof(_type));	\
	}
#else /* #if (__BYTE_ORDER == __LITTLE_ENDIAN) */
/*
 * For big endian, we need to byteswap into little endian.
 */
#undef _ctf_sequence_bitfield
#define _ctf_sequence_bitfield(_type, _item, _src,		\
			_length_type, _src_length,		\
			_user, _nowrite)			\
	{							\
		_length_type __tmpl = this_cpu_ptr(&lttng_dynamic_len_stack)->stack[__dynamic_len_idx] * sizeof(_type) * CHAR_BIT; \
		__chan->ops->event_write(&__ctx, &__tmpl, sizeof(_length_type), lttng_alignof(_length_type)); \
	}								\
	lib_ring_buffer_align_ctx(&__ctx, lttng_alignof(_type));	\
	{								\
		size_t _i, _length;					\
									\
		_length = __get_dynamic_len(dest);			\
		for (_i = 0; _i < _length; _i++) {			\
			_type _tmp;					\
									\
			if (_user) {					\
				if (get_user(_tmp, (_type *) _src + _i)) \
					_tmp = 0;			\
			} else {					\
				_tmp = ((_type *) _src)[_i];		\
			}						\
			switch (sizeof(_type)) {			\
			case 1:						\
				break;					\
			case 2:						\
				_tmp = cpu_to_le16(_tmp);		\
				break;					\
			case 4:						\
				_tmp = cpu_to_le32(_tmp);		\
				break;					\
			case 8:						\
				_tmp = cpu_to_le64(_tmp);		\
				break;					\
			default:					\
				BUG_ON(1);				\
			}						\
			__chan->ops->event_write(&__ctx, &_tmp, sizeof(_type), 1); \
		}							\
	}
#endif /* #else #if (__BYTE_ORDER == __LITTLE_ENDIAN) */

#undef _ctf_string
#define _ctf_string(_item, _src, _user, _nowrite)		        \
	if (_user) {							\
		lib_ring_buffer_align_ctx(&__ctx, lttng_alignof(*(_src))); \
		__chan->ops->event_strcpy_from_user(&__ctx, _src,	\
			__get_dynamic_len(dest));			\
	} else {							\
		const char *__ctf_tmp_string =				\
			((_src) ? (_src) : __LTTNG_NULL_STRING);	\
		lib_ring_buffer_align_ctx(&__ctx,			\
			lttng_alignof(*__ctf_tmp_string));		\
		__chan->ops->event_strcpy(&__ctx, __ctf_tmp_string,	\
			__get_dynamic_len(dest));			\
	}

#undef _ctf_enum
#define _ctf_enum(_name, _type, _item, _src, _user, _nowrite)		\
	_ctf_integer_ext(_type, _item, _src, __BYTE_ORDER, 10, _user, _nowrite)

#undef ctf_align
#define ctf_align(_type)						\
	lib_ring_buffer_align_ctx(&__ctx, lttng_alignof(_type));

#undef ctf_custom_field
#define ctf_custom_field(_type, _item, _code)		_code

#undef ctf_custom_code
#define ctf_custom_code(...)						\
	{								\
		__VA_ARGS__						\
	}

/* Beware: this get len actually consumes the len value */
#undef __get_dynamic_len
#define __get_dynamic_len(field)	this_cpu_ptr(&lttng_dynamic_len_stack)->stack[__dynamic_len_idx++]

#undef TP_PROTO
#define TP_PROTO(...)	__VA_ARGS__

#undef TP_ARGS
#define TP_ARGS(...)	__VA_ARGS__

#undef TP_FIELDS
#define TP_FIELDS(...)	__VA_ARGS__

#undef TP_locvar
#define TP_locvar(...)	__VA_ARGS__

#undef TP_code_pre
#define TP_code_pre(...)	__VA_ARGS__

#undef TP_code_post
#define TP_code_post(...)	__VA_ARGS__

/*
 * For state dump, check that "session" argument (mandatory) matches the
 * session this event belongs to. Ensures that we write state dump data only
 * into the started session, not into all sessions.
 */
#ifdef TP_SESSION_CHECK
#define _TP_SESSION_CHECK(session, csession)	(session == csession)
#else /* TP_SESSION_CHECK */
#define _TP_SESSION_CHECK(session, csession)	1
#endif /* TP_SESSION_CHECK */

/*
 * Using twice size for filter stack data to hold size and pointer for
 * each field (worse case). For integers, max size required is 64-bit.
 * Same for double-precision floats. Those fit within
 * 2*sizeof(unsigned long) for all supported architectures.
 * Perform UNION (||) of filter runtime list.
 */
#undef _LTTNG_TRACEPOINT_EVENT_CLASS_CODE
#define _LTTNG_TRACEPOINT_EVENT_CLASS_CODE(_name, _data_proto, _locvar_args, _locvar, _code_pre, _fields, _code_post) \
static void __event_probe__##_name(_data_proto)						\
{											\
	struct probe_local_vars { _locvar };						\
	struct lttng_kernel_event_common *__event = __data;				\
	struct lttng_kernel_channel_common *__chan_common;				\
	struct lttng_kernel_probe_ctx __lttng_probe_ctx = {				\
		.event = __event,							\
		.interruptible = !irqs_disabled(),					\
	};										\
	union {										\
		size_t __dynamic_len_removed[ARRAY_SIZE(__event_fields___##_name)];	\
		char __interpreter_stack_data[2 * sizeof(unsigned long) * ARRAY_SIZE(__event_fields___##_name)]; \
	} __stackvar;									\
	size_t __orig_dynamic_len_offset, __dynamic_len_idx __attribute__((unused));	\
	struct probe_local_vars __tp_locvar;						\
	struct probe_local_vars *tp_locvar __attribute__((unused)) =			\
			&__tp_locvar;							\
	bool __interpreter_stack_prepared = false;					\
											\
	if (unlikely(!READ_ONCE(__event->enabled)))					\
		return;									\
	__chan_common = lttng_kernel_get_chan_common_from_event_common(__event);	\
	if (__chan_common) {								\
		struct lttng_kernel_session *__session = __chan_common->session;	\
		struct lttng_kernel_id_tracker_rcu *__lf;				\
											\
		if (!_TP_SESSION_CHECK(session, __session))				\
			return;								\
		if (unlikely(!LTTNG_READ_ONCE(__session->active)))			\
			return;								\
		if (unlikely(!LTTNG_READ_ONCE(__chan_common->enabled)))			\
			return;								\
		__lf = lttng_rcu_dereference(__session->pid_tracker.p);			\
		if (__lf && likely(!lttng_id_tracker_lookup(__lf, current->tgid)))	\
			return;								\
		__lf = lttng_rcu_dereference(__session->vpid_tracker.p);		\
		if (__lf && likely(!lttng_id_tracker_lookup(__lf, task_tgid_vnr(current)))) \
			return;								\
		__lf = lttng_rcu_dereference(__session->uid_tracker.p);			\
		if (__lf && likely(!lttng_id_tracker_lookup(__lf,			\
				from_kuid_munged(&init_user_ns, current_uid()))))	\
			return;								\
		__lf = lttng_rcu_dereference(__session->vuid_tracker.p);		\
		if (__lf && likely(!lttng_id_tracker_lookup(__lf,			\
				from_kuid_munged(current_user_ns(), current_uid()))))	\
			return;								\
		__lf = lttng_rcu_dereference(__session->gid_tracker.p);			\
		if (__lf && likely(!lttng_id_tracker_lookup(__lf,			\
				from_kgid_munged(&init_user_ns, current_gid()))))	\
			return;								\
		__lf = lttng_rcu_dereference(__session->vgid_tracker.p);		\
		if (__lf && likely(!lttng_id_tracker_lookup(__lf,			\
				from_kgid_munged(current_user_ns(), current_gid()))))	\
			return;								\
	}										\
	__orig_dynamic_len_offset = this_cpu_ptr(&lttng_dynamic_len_stack)->offset;	\
	__dynamic_len_idx = __orig_dynamic_len_offset;					\
	_code_pre									\
	if (unlikely(READ_ONCE(__event->eval_filter))) {				\
		__event_prepare_interpreter_stack__##_name(__stackvar.__interpreter_stack_data, \
				_locvar_args);						\
		__interpreter_stack_prepared = true;					\
		if (likely(__event->run_filter(__event,			      		\
				__stackvar.__interpreter_stack_data, &__lttng_probe_ctx, NULL) != LTTNG_KERNEL_EVENT_FILTER_ACCEPT)) \
			goto __post;							\
	}										\
	switch (__event->type) {							\
	case LTTNG_KERNEL_EVENT_TYPE_RECORDER:						\
	{										\
		struct lttng_kernel_event_recorder *__event_recorder =			\
			container_of(__event, struct lttng_kernel_event_recorder, parent); \
		struct lttng_kernel_channel_buffer *__chan = __event_recorder->chan;	\
		struct lttng_kernel_ring_buffer_ctx __ctx;				\
		ssize_t __event_len;							\
		size_t __event_align;							\
		int __ret;								\
											\
		__event_len = __event_get_size__##_name(_locvar_args);			\
		if (unlikely(__event_len < 0)) {					\
			__chan->ops->lost_event_too_big(__chan);			\
			goto __post;							\
		}									\
		__event_align = __event_get_align__##_name(_locvar_args);		\
		lib_ring_buffer_ctx_init(&__ctx, __event_recorder, __event_len,		\
					 __event_align, &__lttng_probe_ctx);		\
		__ret = __chan->ops->event_reserve(&__ctx);				\
		if (__ret < 0)								\
			goto __post;							\
		_fields									\
		__chan->ops->event_commit(&__ctx);					\
		break;									\
	}										\
	case LTTNG_KERNEL_EVENT_TYPE_NOTIFIER:						\
	{										\
		struct lttng_kernel_event_notifier *__event_notifier =			\
			container_of(__event, struct lttng_kernel_event_notifier, parent); \
		struct lttng_kernel_notification_ctx __notif_ctx;			\
											\
		__notif_ctx.eval_capture = LTTNG_READ_ONCE(__event_notifier->eval_capture); \
		if (unlikely(!__interpreter_stack_prepared && __notif_ctx.eval_capture)) \
			__event_prepare_interpreter_stack__##_name(			\
					__stackvar.__interpreter_stack_data,		\
					_locvar_args);					\
											\
		__event_notifier->notification_send(__event_notifier,			\
				__stackvar.__interpreter_stack_data,			\
				&__lttng_probe_ctx,					\
				&__notif_ctx);						\
		break;									\
	}										\
	case LTTNG_KERNEL_EVENT_TYPE_COUNTER:						\
	{										\
		struct lttng_kernel_event_counter *__event_counter =			\
			container_of(__event, struct lttng_kernel_event_counter, parent); \
		struct lttng_kernel_event_counter_ctx __event_counter_ctx;		\
											\
		__event_counter_ctx.args_available = LTTNG_READ_ONCE(__event_counter->use_args); \
		if (unlikely(!__interpreter_stack_prepared && __event_counter_ctx.args_available)) \
			__event_prepare_interpreter_stack__##_name(			\
					__stackvar.__interpreter_stack_data,		\
					_locvar_args);					\
											\
		(void) __event_counter->chan->ops->counter_hit(__event_counter,		\
				__stackvar.__interpreter_stack_data,			\
				&__lttng_probe_ctx,					\
				&__event_counter_ctx);					\
		break;									\
	}										\
	default:									\
		WARN_ON_ONCE(1);							\
	}										\
__post:											\
	_code_post									\
	barrier();	/* use before un-reserve. */					\
	this_cpu_ptr(&lttng_dynamic_len_stack)->offset = __orig_dynamic_len_offset;	\
	return;										\
}

#undef LTTNG_TRACEPOINT_EVENT_CLASS_CODE
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE(_name, _proto, _args, _locvar, _code_pre, _fields, _code_post) \
	_LTTNG_TRACEPOINT_EVENT_CLASS_CODE(_name, PARAMS(void *__data, _proto), PARAMS(tp_locvar, _args), \
			PARAMS(_locvar), PARAMS(_code_pre), PARAMS(_fields), PARAMS(_code_post))

#undef LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS(_name, _locvar, _code_pre, _fields, _code_post) \
	_LTTNG_TRACEPOINT_EVENT_CLASS_CODE(_name, PARAMS(void *__data), PARAMS(tp_locvar), PARAMS(_locvar), \
			PARAMS(_code_pre), PARAMS(_fields), PARAMS(_code_post))

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)

#undef __get_dynamic_len

/*
 * Stage 7.1 of the trace events.
 *
 * Create event descriptions.
 */

/* Named field types must be defined in lttng-types.h */

#include <lttng/events-reset.h>	/* Reset all macros within LTTNG_TRACEPOINT_EVENT */

#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS(_template, _name, _map)	\
static const struct lttng_kernel_event_desc __event_desc___##_map = {	\
	.event_name = #_map,				     		\
	.event_kname = #_name,				     		\
	.tp_class = &lttng_kernel__event_class___##_template,		\
	.probe_desc = &TP_ID(__probe_desc___, TRACE_SYSTEM),		\
	.owner = THIS_MODULE,				     		\
};

#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(_template, _name, _map, _proto, _args) \
	LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS(_template, _name, _map)

#define TP_ID1(_token, _system)	_token##_system
#define TP_ID(_token, _system)	TP_ID1(_token, _system)

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)

#undef TP_ID1
#undef TP_ID

/*
 * Stage 8 of the trace events.
 *
 * Create an array of event description pointers.
 */

#include <lttng/events-reset.h>	/* Reset all macros within LTTNG_TRACEPOINT_EVENT */

#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS(_template, _name, _map) \
		&__event_desc___##_map,

#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(_template, _name, _map, _proto, _args) \
	LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS(_template, _name, _map)

#define TP_ID1(_token, _system)	_token##_system
#define TP_ID(_token, _system)	TP_ID1(_token, _system)

static const struct lttng_kernel_event_desc * const TP_ID(__provider_event_desc___, TRACE_SYSTEM)[] = {
#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)
};

#undef TP_ID1
#undef TP_ID

/*
 * Stage 9 of the trace events.
 *
 * Create a toplevel descriptor for the whole probe.
 */

#define TP_ID1(_token, _system)	_token##_system
#define TP_ID(_token, _system)	TP_ID1(_token, _system)

/* non-const because list head will be modified when registered. */
static __used struct lttng_kernel_probe_desc TP_ID(__probe_desc___, TRACE_SYSTEM) = {
	.provider_name = __stringify(TRACE_SYSTEM),
	.event_desc = TP_ID(__provider_event_desc___, TRACE_SYSTEM),
	.nr_events = ARRAY_SIZE(TP_ID(__provider_event_desc___, TRACE_SYSTEM)),
	.head = { NULL, NULL },
	.lazy_init_head = { NULL, NULL },
	.lazy = 0,
};

#undef TP_ID1
#undef TP_ID

/*
 * Stage 10 of the trace events.
 *
 * Register/unregister probes at module load/unload.
 */

#include <lttng/events-reset.h>	/* Reset all macros within LTTNG_TRACEPOINT_EVENT */

#define TP_ID1(_token, _system)	_token##_system
#define TP_ID(_token, _system)	TP_ID1(_token, _system)
#define module_init_eval1(_token, _system)	module_init(_token##_system)
#define module_init_eval(_token, _system)	module_init_eval1(_token, _system)
#define module_exit_eval1(_token, _system)	module_exit(_token##_system)
#define module_exit_eval(_token, _system)	module_exit_eval1(_token, _system)

#ifndef TP_MODULE_NOINIT
static int TP_ID(__lttng_events_init__, TRACE_SYSTEM)(void)
{
	wrapper_vmalloc_sync_mappings();
	return lttng_kernel_probe_register(&TP_ID(__probe_desc___, TRACE_SYSTEM));
}

static void TP_ID(__lttng_events_exit__, TRACE_SYSTEM)(void)
{
	lttng_kernel_probe_unregister(&TP_ID(__probe_desc___, TRACE_SYSTEM));
}

#ifndef TP_MODULE_NOAUTOLOAD
module_init_eval(__lttng_events_init__, TRACE_SYSTEM);
module_exit_eval(__lttng_events_exit__, TRACE_SYSTEM);
#endif

#endif

#undef module_init_eval
#undef module_exit_eval
#undef TP_ID1
#undef TP_ID

#undef TP_PROTO
#undef TP_ARGS
