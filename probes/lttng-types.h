/*
 * Protect against multiple inclusion of structure declarations, but run the
 * stages below each time.
 */
#ifndef _LTTNG_PROBES_LTTNG_TYPES_H
#define _LTTNG_PROBES_LTTNG_TYPES_H

#include <linux/seq_file.h>
#include <lttng.h>

#ifdef __KERNEL__
# include <asm/byteorder.h>
# ifdef __BIG_ENDIAN
#  define __BYTE_ORDER __BIG_ENDIAN
# elif defined(__LITTLE_ENDIAN)
#  define __BYTE_ORDER __LITTLE_ENDIAN
# else
#  error "unknown endianness"
# endif
#ifndef __BIG_ENDIAN
# define __BIG_ENDIAN 4321
#endif
#ifndef __LITTLE_ENDIAN
# define __LITTLE_ENDIAN 1234
#endif
#else
# include <endian.h>
#endif

/* Update the astract_types name table in lttng-types.c along with this enum */
enum abstract_types {
	atype_integer,
	atype_enum,
	atype_array,
	atype_sequence,
	atype_string,
	NR_ABSTRACT_TYPES,
};

/* Update the string_encodings name table in lttng-types.c along with this enum */
enum lttng_string_encodings {
	lttng_encode_UTF8 = 0,
	lttng_encode_ASCII = 1,
	NR_STRING_ENCODINGS,
};

struct lttng_enum_entry {
	unsigned long long start, end;	/* start and end are inclusive */
	const char *string;
};

struct lttng_enum {
	const struct lttng_enum_entry *entries;
	unsigned int len;
};

struct lttng_type {
	enum abstract_types atype;
	const char *name;
	union {
		struct {
			unsigned int size;		/* in bits */
			unsigned short alignment;	/* in bits */
			unsigned int signedness:1;
			unsigned int reverse_byte_order:1;
		} integer;
		struct {
			const char *parent_type;
			const struct lttng_enum def;
		} enumeration;
		struct {
			const char *elem_type;
			unsigned int length;		/* num. elems. */
		} array;
		struct {
			const char *elem_type;
			const char *length_type;
		} sequence;
		struct {
			enum lttng_string_encodings encoding;
		} string;
	} u;
} __attribute__((packed));

void lttng_print_event_type(struct seq_file *m, unsigned int indent,
			    const struct lttng_type *type);

#endif /* _LTTNG_PROBES_LTTNG_TYPES_H */


/* Export enumerations */

#ifdef STAGE_EXPORT_ENUMS

#undef TRACE_EVENT_TYPE
#define TRACE_EVENT_TYPE(_name, _abstract_type, args...)

#undef TRACE_EVENT_ENUM
#define TRACE_EVENT_ENUM(_name, _entries...)				\
	const struct lttng_enum_entry __trace_event_enum_##_name[] = {	\
		PARAMS(_entries)					\
	};

/* Enumeration entry (single value) */
#undef V
#define V(_string)		{ _string, _string, #_string}

/* Enumeration entry (range) */
#undef R
#define R(_string, _range_start, _range_end)				\
	{ _range_start, _range_end, #_string }

#endif /* STAGE_EXPORT_ENUMS */


/* Export named types */

#ifdef STAGE_EXPORT_TYPES

#undef TRACE_EVENT_TYPE___integer_ext
#define TRACE_EVENT_TYPE___integer_ext(_name, _byte_order)	\
		{						\
		  .atype = atype_integer,			\
		  .name = #_name,				\
		  .u.integer.size = sizeof(_name) * 8,		\
		  .u.integer.alignment = __alignof__(_name) * 8,\
		  .u.integer.signedness = is_signed_type(_name),\
		  .u.integer.reverse_byte_order = ((_byte_order) != __BYTE_ORDER),\
		},

#undef TRACE_EVENT_TYPE___integer
#define TRACE_EVENT_TYPE___integer(_name, _unused)		\
		TRACE_EVENT_TYPE___integer_ext(_name, __BYTE_ORDER)

#undef TRACE_EVENT_TYPE___enum
#define TRACE_EVENT_TYPE___enum(_name, _parent_type)		\
		{						\
		  .atype = atype_enum,				\
		  .name = #_name,				\
		  .u.enumeration.parent_type = #_parent_type,	\
		  .u.enumeration.def.entries = __trace_event_enum_##_name, \
		  .u.enumeration.def.len = ARRAY_SIZE(__trace_event_enum_##_name), \
		},

#undef TRACE_EVENT_TYPE___array
#define TRACE_EVENT_TYPE___array(_name, _elem_type, _length)	\
		{						\
		  .atype = atype_array,				\
		  .name = #_name,				\
		  .u.array.elem_type = #_elem_type,		\
		  .u.array.length = _length,			\
		},

#undef TRACE_EVENT_TYPE___sequence
#define TRACE_EVENT_TYPE___sequence(_name, _elem_type, _length_type)	\
		{						\
		  .atype = atype_sequence,			\
		  .name = #_name,				\
		  .u.sequence.elem_type = #_elem_type,		\
		  .u.sequence.length_type = #_length_type,	\
		},

#undef TRACE_EVENT_TYPE___string
#define TRACE_EVENT_TYPE___string(_name, _encoding)		\
		{						\
		  .atype = atype_string,			\
		  .name = #_name,				\
		  .u.string.encoding = lttng_encode_##_encoding,\
		},


/* Local declaration */
#undef TRACE_EVENT_TYPE
#define TRACE_EVENT_TYPE(_name, _abstract_type, args...)	\
		TRACE_EVENT_TYPE___##_abstract_type(_name, args)

#undef TRACE_EVENT_ENUM
#define TRACE_EVENT_ENUM(_name, _entries...)

#endif /* STAGE_EXPORT_TYPES */
