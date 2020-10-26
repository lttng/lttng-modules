/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/tracepoint.h
 *
 * wrapper around DECLARE_EVENT_CLASS.
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_TRACEPOINT_H
#define _LTTNG_WRAPPER_TRACEPOINT_H

#include <linux/version.h>
#include <linux/tracepoint.h>
#include <linux/module.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))

#define DECLARE_EVENT_CLASS(name, proto, args, tstruct, assign, print)

#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0))
#define LTTNG_DEFINE_TRACE(name, proto, args)		\
	DEFINE_TRACE(name, PARAMS(proto), PARAMS(args))
#else
#define LTTNG_DEFINE_TRACE(name, proto, args)		\
	DEFINE_TRACE(name)
#endif

#ifndef HAVE_KABI_2635_TRACEPOINT

#define kabi_2635_tracepoint_probe_register tracepoint_probe_register
#define kabi_2635_tracepoint_probe_unregister tracepoint_probe_unregister

#endif /* HAVE_KABI_2635_TRACEPOINT */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0))

#include <lttng-tracepoint.h>

#define lttng_wrapper_tracepoint_probe_register lttng_tracepoint_probe_register
#define lttng_wrapper_tracepoint_probe_unregister lttng_tracepoint_probe_unregister

#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0)) */

#define lttng_wrapper_tracepoint_probe_register kabi_2635_tracepoint_probe_register
#define lttng_wrapper_tracepoint_probe_unregister kabi_2635_tracepoint_probe_unregister

static inline
int lttng_tracepoint_init(void)
{
	return 0;
}

static inline
void lttng_tracepoint_exit(void)
{
}

#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0)) */

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,15,0) && defined(CONFIG_MODULE_SIG))

#include <linux/kallsyms.h>
#include <wrapper/kallsyms.h>

static inline
int wrapper_tracepoint_module_notify(struct notifier_block *nb,
		unsigned long val, struct module *mod)
{
	int (*tracepoint_module_notify_sym)(struct notifier_block *nb,
			unsigned long val, struct module *mod);

	tracepoint_module_notify_sym =
		(void *) kallsyms_lookup_funcptr("tracepoint_module_notify");
	if (tracepoint_module_notify_sym) {
		return tracepoint_module_notify_sym(nb, val, mod);
	} else {
		printk_once(KERN_WARNING "LTTng: tracepoint_module_notify symbol lookup failed. It probably means you kernel don't need this work-around. Please consider upgrading LTTng modules to make this warning go away.\n");
		return -ENOSYS;
	}
}

/*
 * No canary for 'tracepoint_module_notify()', it's only defined in 'kernel/tracepoint.c'.
 *
 * static inline
 * int __canary__tracepoint_module_notify(struct notifier_block *nb,
 * 		unsigned long val, struct module *mod)
 * {
 * 	return tracepoint_module_notify(nb, val, mod);
 * }
 */

#endif /* #if (LINUX_VERSION_CODE < KERNEL_VERSION(3,15,0) && defined(CONFIG_MODULE_SIG)) */

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,15,0) && defined(CONFIG_MODULE_SIG) && defined(MODULE))

static inline
int wrapper_lttng_fixup_sig(struct module *mod)
{
	int ret = 0;

	/*
	 * This is for module.c confusing force loaded modules with
	 * unsigned modules.
	 */
	if (!THIS_MODULE->sig_ok &&
			THIS_MODULE->taints & (1U << TAINT_FORCED_MODULE)) {
		THIS_MODULE->taints &= ~(1U << TAINT_FORCED_MODULE);
		ret = wrapper_tracepoint_module_notify(NULL,
				MODULE_STATE_COMING, mod);
		THIS_MODULE->taints |= (1U << TAINT_FORCED_MODULE);
	}
	return ret;
}

#else /* #if (LINUX_VERSION_CODE < KERNEL_VERSION(3,15,0) && defined(CONFIG_MODULE_SIG) && defined(MODULE)) */

static inline
int wrapper_lttng_fixup_sig(struct module *mod)
{
	return 0;
}

#endif /* #else #if (LINUX_VERSION_CODE < KERNEL_VERSION(3,15,0) && defined(CONFIG_MODULE_SIG) && defined(MODULE)) */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,19,0))
static inline struct tracepoint *lttng_tracepoint_ptr_deref(tracepoint_ptr_t *p)
{
	return tracepoint_ptr_deref(p);
}
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,19,0)) */
static inline struct tracepoint *lttng_tracepoint_ptr_deref(struct tracepoint * const *p)
{
	return *p;
}
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,19,0)) */

#endif /* _LTTNG_WRAPPER_TRACEPOINT_H */
