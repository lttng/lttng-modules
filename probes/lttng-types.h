/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * probes/lttng-types.h
 *
 * LTTng types.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

/*
 * Protect against multiple inclusion of structure declarations, but run the
 * stages below each time.
 */
#ifndef _LTTNG_PROBES_LTTNG_TYPES_H
#define _LTTNG_PROBES_LTTNG_TYPES_H

#include <linux/seq_file.h>
#include <probes/lttng.h>
#include <lttng-events.h>
#include <lttng-tracer.h>
#include <lttng-endian.h>

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

#undef TRACE_EVENT_TYPE___enum
#define TRACE_EVENT_TYPE___enum(_name, _container_type)			\
		{							\
		  .name = #_name,					\
		  .container_type = __type_integer(_container_type, 0, 0, -1, __BYTE_ORDER, 10, none), \
		  .entries = __trace_event_enum_##_name, \
		  .len = ARRAY_SIZE(__trace_event_enum_##_name), \
		},

/* Local declaration */
#undef TRACE_EVENT_TYPE
#define TRACE_EVENT_TYPE(_name, _abstract_type, args...)	\
		TRACE_EVENT_TYPE___##_abstract_type(_name, args)

#undef TRACE_EVENT_ENUM
#define TRACE_EVENT_ENUM(_name, _entries...)

#endif /* STAGE_EXPORT_TYPES */
