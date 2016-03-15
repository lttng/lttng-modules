/*
 * define_trace.h
 *
 * Copyright (C) 2009 Steven Rostedt <rostedt@goodmis.org>
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; only
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * Trace files that want to automate creationg of all tracepoints defined
 * in their file should include this file. The following are macros that the
 * trace file may define:
 *
 * TRACE_SYSTEM defines the system the tracepoint is for
 *
 * TRACE_INCLUDE_FILE if the file name is something other than TRACE_SYSTEM.h
 *     This macro may be defined to tell define_trace.h what file to include.
 *     Note, leave off the ".h".
 *
 * TRACE_INCLUDE_PATH if the path is something other than core kernel include/trace
 *     then this macro can define the path to use. Note, the path is relative to
 *     define_trace.h, not the file including it. Full path names for out of tree
 *     modules must be used.
 */

#ifdef CREATE_TRACE_POINTS

/* Prevent recursion */
#undef CREATE_TRACE_POINTS

#include <linux/stringify.h>

#undef LTTNG_TRACEPOINT_EVENT_MAP
#define LTTNG_TRACEPOINT_EVENT_MAP(name, map, proto, args, fields)

#undef LTTNG_TRACEPOINT_EVENT_MAP_NOARGS
#define LTTNG_TRACEPOINT_EVENT_MAP_NOARGS(name, map, fields)

#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(template, name, map, proto, args)

#undef LTTNG_TRACEPOINT_EVENT
#define LTTNG_TRACEPOINT_EVENT(name, proto, args, fields)

#undef LTTNG_TRACEPOINT_EVENT_CODE
#define LTTNG_TRACEPOINT_EVENT_CODE(name, proto, args, _locvar, _code_pre, fields, _code_post)

#undef LTTNG_TRACEPOINT_EVENT_CODE_MAP
#define LTTNG_TRACEPOINT_EVENT_CODE_MAP(name, map, proto, args, _locvar, _code_pre, fields, _code_post)

#undef LTTNG_TRACEPOINT_EVENT_INSTANCE
#define LTTNG_TRACEPOINT_EVENT_INSTANCE(template, name, proto, args)

#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(template, name)

#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS(template, name, map)

#undef TRACE_INCLUDE
#undef __TRACE_INCLUDE

#ifndef TRACE_INCLUDE_FILE
# define TRACE_INCLUDE_FILE TRACE_SYSTEM
# define UNDEF_TRACE_INCLUDE_FILE
#endif

#ifndef TRACE_INCLUDE_PATH
# define __TRACE_INCLUDE(system) <trace/events/system.h>
# define UNDEF_TRACE_INCLUDE_PATH
#else
# define __TRACE_INCLUDE(system) __stringify(TRACE_INCLUDE_PATH/system.h)
#endif

# define TRACE_INCLUDE(system) __TRACE_INCLUDE(system)

/* Let the trace headers be reread */
#define TRACE_HEADER_MULTI_READ

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)

/* Make all open coded DECLARE_TRACE nops */
#undef DECLARE_TRACE
#define DECLARE_TRACE(name, proto, args)

#ifdef LTTNG_PACKAGE_BUILD
#include <probes/lttng-tracepoint-event-impl.h>
#endif

#undef LTTNG_TRACEPOINT_EVENT
#undef LTTNG_TRACEPOINT_EVENT_INSTANCE
#undef LTTNG_TRACEPOINT_EVENT_MAP
#undef LTTNG_TRACEPOINT_EVENT_CLASS
#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP
#undef LTTNG_TRACEPOINT_EVENT_CODE_MAP
#undef LTTNG_TRACEPOINT_EVENT_CODE
#undef LTTNG_TRACEPOINT_EVENT_MAP_NOARGS
#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS
#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS
#undef TRACE_HEADER_MULTI_READ

/* Only undef what we defined in this file */
#ifdef UNDEF_TRACE_INCLUDE_FILE
# undef TRACE_INCLUDE_FILE
# undef UNDEF_TRACE_INCLUDE_FILE
#endif

#ifdef UNDEF_TRACE_INCLUDE_PATH
# undef TRACE_INCLUDE_PATH
# undef UNDEF_TRACE_INCLUDE_PATH
#endif

/*
 * We want to re-include lttng-tracepoint-event.h for a following probe.
 */
#undef LTTNG_TRACEPOINT_EVENT_H

/* We may be processing more files */
#define CREATE_TRACE_POINTS

#endif /* CREATE_TRACE_POINTS */
