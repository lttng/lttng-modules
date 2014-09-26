#ifndef LTTNG_TRACEPOINT_EVENT_H
#define LTTNG_TRACEPOINT_EVENT_H

/*
 * lttng-tracepoint-event.h
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
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

#include <linux/tracepoint.h>

/*
 * If code defines LTTNG_INSTRUMENTATION before including the instrumentation
 * header, generate the instrumentation static inlines. Else, it means
 * we are a probe for the Linux kernel, and it is the probe responsibility
 * to have already included the Linux kernel instrumentation header.
 */
#ifdef LTTNG_INSTRUMENTATION
#define _LTTNG_INSTRUMENTATION(...)	__VA_ARGS__
#else
#define _LTTNG_INSTRUMENTATION(...)
#endif

#define LTTNG_TRACEPOINT_EVENT(name, proto, args, struct, assign, print) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE(name, PARAMS(proto), PARAMS(args)))
#define LTTNG_TRACEPOINT_EVENT_CODE(name, proto, args, _locvar, _code, tstruct, assign, print) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE(name, PARAMS(proto), PARAMS(args)))
#define LTTNG_TRACEPOINT_EVENT_MAP(name, map, proto, args, tstruct, assign, print) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE(name, PARAMS(proto), PARAMS(args)))
#define LTTNG_TRACEPOINT_EVENT_FN(name, proto, args, struct, assign, print, reg, unreg) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE(name, PARAMS(proto), PARAMS(args)))
#define LTTNG_TRACEPOINT_EVENT_FN_MAP(name, map, proto, args, tstruct, assign, print, reg, unreg) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE(name, PARAMS(proto), PARAMS(args)))
#define LTTNG_TRACEPOINT_EVENT_CONDITION(name, proto, args, cond, struct, assign, print) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE(name, PARAMS(proto), PARAMS(args)))
#define LTTNG_TRACEPOINT_EVENT_CONDITION_MAP(name, map, proto, args, cond, struct, assign, print) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE(name, PARAMS(proto), PARAMS(args)))
#define LTTNG_TRACEPOINT_EVENT_MAP_NOARGS(name, map, struct, assign, print) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE_NOARGS(name))

#define LTTNG_TRACEPOINT_EVENT_CLASS(name, proto, args, tstruct, assign, print)
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE(_name, _proto, _args, _locvar, _code, _tstruct, _assign, _print)
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS(_name, _locvar, _code, _tstruct, _assign, _print)

#define LTTNG_TRACEPOINT_EVENT_INSTANCE(template, name, proto, args) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE(name, PARAMS(proto), PARAMS(args)))
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(template, name) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE_NOARGS(name))
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_PRINT(template, name, proto, args, print) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE(name, PARAMS(proto), PARAMS(args)))
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_PRINT_MAP(template, name, map, proto, args, print) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE(name, PARAMS(proto), PARAMS(args)))
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_CONDITION(template, name, proto, args, cond) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE(name, PARAMS(proto), PARAMS(args)))
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_CONDITION_MAP(template, name, map, proto, args, cond) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE(name, PARAMS(proto), PARAMS(args)))
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(_template, _name, _map, _proto, _args) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE(name, PARAMS(proto), PARAMS(args)))
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS(_template, _name, _map) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE_NOARGS(name))

#endif /* LTTNG_TRACEPOINT_EVENT_H */
