/* SPDX-License-Identifier: GPL-2.0-only */
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

#include <lttng/tracepoint-event.h>
#include <linux/version.h>
#include <linux/kernel.h>

#ifdef CONFIG_MODULES

#ifndef _TRACE_MODULE_DEF
#define _TRACE_MODULE_DEF
struct module;

#endif

LTTNG_TRACEPOINT_ENUM(taint,
	TP_ENUM_VALUES(
		ctf_enum_value("PROPRIETARY_MODULE", 1UL << TAINT_PROPRIETARY_MODULE)
		ctf_enum_value("FORCED_MODULE", 1UL << TAINT_FORCED_MODULE)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0))
		ctf_enum_value("CPU_OUT_OF_SPEC", 1UL << TAINT_CPU_OUT_OF_SPEC)
#else
		ctf_enum_value("UNSAFE_SMP", 1UL << TAINT_UNSAFE_SMP)
#endif
		ctf_enum_value("FORCED_RMMOD", 1UL << TAINT_FORCED_RMMOD)
		ctf_enum_value("MACHINE_CHECK", 1UL << TAINT_MACHINE_CHECK)
		ctf_enum_value("BAD_PAGE", 1UL << TAINT_BAD_PAGE)
		ctf_enum_value("USER", 1UL << TAINT_USER)
		ctf_enum_value("DIE", 1UL << TAINT_DIE)
		ctf_enum_value("OVERRIDDEN_ACPI_TABLE", 1UL << TAINT_OVERRIDDEN_ACPI_TABLE)
		ctf_enum_value("WARN", 1UL << TAINT_WARN)
		ctf_enum_value("CRAP", 1UL << TAINT_CRAP)
		ctf_enum_value("FIRMWARE_WORKAROUND", 1UL << TAINT_FIRMWARE_WORKAROUND)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0))
		ctf_enum_value("OOT_MODULE", 1UL << TAINT_OOT_MODULE)
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0))
		ctf_enum_value("UNSIGNED_MODULE", 1UL << TAINT_UNSIGNED_MODULE)
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0))
		ctf_enum_value("SOFTLOCKUP", 1UL << TAINT_SOFTLOCKUP)
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0))
		ctf_enum_value("LIVEPATCH", 1UL << TAINT_LIVEPATCH)
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0))
		ctf_enum_value("AUX", 1UL << TAINT_AUX)
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0))
		ctf_enum_value("RANDSTRUCT", 1UL << TAINT_RANDSTRUCT)
#endif
	)
)

LTTNG_TRACEPOINT_EVENT(module_load,

	TP_PROTO(struct module *mod),

	TP_ARGS(mod),

	TP_FIELDS(
		ctf_enum(taint, unsigned int, taints, mod->taints)
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

	TP_PROTO(struct module *mod, unsigned long ip),

	TP_ARGS(mod, ip),

	TP_FIELDS(
		ctf_integer_hex(unsigned long, ip, ip)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0))
		ctf_integer(int, refcnt, atomic_read(&mod->refcnt))
#else
		ctf_integer(int, refcnt, __this_cpu_read(mod->refptr->incs) + __this_cpu_read(mod->refptr->decs))
#endif
		ctf_string(name, mod->name)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(module_refcnt, module_get,

	TP_PROTO(struct module *mod, unsigned long ip),

	TP_ARGS(mod, ip)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(module_refcnt, module_put,

	TP_PROTO(struct module *mod, unsigned long ip),

	TP_ARGS(mod, ip)
)
#endif /* CONFIG_MODULE_UNLOAD */

LTTNG_TRACEPOINT_EVENT(module_request,

	TP_PROTO(char *name, bool wait, unsigned long ip),

	TP_ARGS(name, wait, ip),

	TP_FIELDS(
		ctf_integer_hex(unsigned long, ip, ip)
		ctf_integer(bool, wait, wait)
		ctf_string(name, name)
	)
)

#endif /* CONFIG_MODULES */

#endif /* LTTNG_TRACE_MODULE_H */

/* This part must be outside protection */
#include <lttng/define_trace.h>
