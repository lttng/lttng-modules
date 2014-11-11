/*
 * Because linux/module.h has tracepoints in the header, and ftrace.h
 * eventually includes this file, define_trace.h includes linux/module.h
 * But we do not want the module.h to override the TRACE_SYSTEM macro
 * variable that define_trace.h is processing, so we only set it
 * when module events are being processed, which would happen when
 * CREATE_TRACE_POINTS is defined.
 */
#ifdef CREATE_TRACE_POINTS
#undef TRACE_SYSTEM
#define TRACE_SYSTEM module
#endif

#if !defined(LTTNG_TRACE_MODULE_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_MODULE_H

#include "../../../probes/lttng-tracepoint-event.h"
#include <linux/version.h>

#ifdef CONFIG_MODULES

#ifndef _TRACE_MODULE_DEF
#define _TRACE_MODULE_DEF
struct module;

#endif

LTTNG_TRACEPOINT_EVENT(module_load,

	TP_PROTO(struct module *mod),

	TP_ARGS(mod),

	TP_FIELDS(
		ctf_integer(unsigned int, taints, mod->taints)
		ctf_string(name, mod->name)
	)
)

LTTNG_TRACEPOINT_EVENT(module_free,

	TP_PROTO(struct module *mod),

	TP_ARGS(mod),

	TP_FIELDS(
		ctf_string(name, mod->name)
	)
)

#ifdef CONFIG_MODULE_UNLOAD
/* trace_module_get/put are only used if CONFIG_MODULE_UNLOAD is defined */

LTTNG_TRACEPOINT_EVENT_CLASS(module_refcnt,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35))
	TP_PROTO(struct module *mod, unsigned long ip),

	TP_ARGS(mod, ip),
#else
	TP_PROTO(struct module *mod, unsigned long ip, int refcnt),

	TP_ARGS(mod, ip, refcnt),
#endif

	TP_FIELDS(
		ctf_integer(unsigned long, ip, ip)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0))
		ctf_integer(int, refcnt, atomic_read(&mod->refcnt))
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35))
		ctf_integer(int, refcnt, __this_cpu_read(mod->refptr->incs) + __this_cpu_read(mod->refptr->decs))
#else
		ctf_integer(int, refcnt, refcnt)
#endif
		ctf_string(name, mod->name)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(module_refcnt, module_get,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35))
	TP_PROTO(struct module *mod, unsigned long ip),

	TP_ARGS(mod, ip)
#else
	TP_PROTO(struct module *mod, unsigned long ip, int refcnt),

	TP_ARGS(mod, ip, refcnt)
#endif
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(module_refcnt, module_put,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35))
	TP_PROTO(struct module *mod, unsigned long ip),

	TP_ARGS(mod, ip)
#else
	TP_PROTO(struct module *mod, unsigned long ip, int refcnt),

	TP_ARGS(mod, ip, refcnt)
#endif
)
#endif /* CONFIG_MODULE_UNLOAD */

LTTNG_TRACEPOINT_EVENT(module_request,

	TP_PROTO(char *name, bool wait, unsigned long ip),

	TP_ARGS(name, wait, ip),

	TP_FIELDS(
		ctf_integer(unsigned long, ip, ip)
		ctf_integer(bool, wait, wait)
		ctf_string(name, name)
	)
)

#endif /* CONFIG_MODULES */

#endif /* LTTNG_TRACE_MODULE_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
