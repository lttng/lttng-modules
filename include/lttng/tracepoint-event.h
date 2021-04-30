/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng/tracepoint-event.h
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef LTTNG_TRACEPOINT_EVENT_H
#define LTTNG_TRACEPOINT_EVENT_H

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

#undef LTTNG_TRACEPOINT_EVENT
#define LTTNG_TRACEPOINT_EVENT(name, proto, args, fields) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE(name, PARAMS(proto), PARAMS(args)))
#undef LTTNG_TRACEPOINT_EVENT_CODE
#define LTTNG_TRACEPOINT_EVENT_CODE(name, proto, args, _locvar, _code_pre, fields, _code_post) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE(name, PARAMS(proto), PARAMS(args)))
#undef LTTNG_TRACEPOINT_EVENT_CODE_MAP
#define LTTNG_TRACEPOINT_EVENT_CODE_MAP(name, map, proto, args, _locvar, _code_pre, fields, _code_post) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE(name, PARAMS(proto), PARAMS(args)))
#undef LTTNG_TRACEPOINT_EVENT_MAP
#define LTTNG_TRACEPOINT_EVENT_MAP(name, map, proto, args, fields) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE(name, PARAMS(proto), PARAMS(args)))
#undef LTTNG_TRACEPOINT_EVENT_MAP_NOARGS
#define LTTNG_TRACEPOINT_EVENT_MAP_NOARGS(name, map, fields) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE_NOARGS(name))

#undef LTTNG_TRACEPOINT_EVENT_CLASS
#define LTTNG_TRACEPOINT_EVENT_CLASS(name, proto, args, fields)
#undef LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS
#define LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(name, fields)
#undef LTTNG_TRACEPOINT_EVENT_CLASS_CODE
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE(_name, _proto, _args, _locvar, _code_pre, _fields, _code_post)
#undef LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS(_name, _locvar, _code_pre, _fields, _code_post)

#undef LTTNG_TRACEPOINT_EVENT_INSTANCE
#define LTTNG_TRACEPOINT_EVENT_INSTANCE(template, name, proto, args) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE(name, PARAMS(proto), PARAMS(args)))
#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(template, name) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE_NOARGS(name))
#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(_template, _name, _map, _proto, _args) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE(name, PARAMS(proto), PARAMS(args)))
#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS(_template, _name, _map) \
	_LTTNG_INSTRUMENTATION(DECLARE_TRACE_NOARGS(name))

#undef LTTNG_TRACEPOINT_ENUM
#define LTTNG_TRACEPOINT_ENUM(_name, _values)

#undef LTTNG_TRACEPOINT_TYPE
#define LTTNG_TRACEPOINT_TYPE(_prototype, _init)

#endif /* LTTNG_TRACEPOINT_EVENT_H */
