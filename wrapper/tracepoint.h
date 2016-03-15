#ifndef _LTTNG_WRAPPER_TRACEPOINT_H
#define _LTTNG_WRAPPER_TRACEPOINT_H

/*
 * wrapper/tracepoint.h
 *
 * wrapper around DECLARE_EVENT_CLASS.
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
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

#include <linux/version.h>
#include <linux/tracepoint.h>
#include <linux/module.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))

#define DECLARE_EVENT_CLASS(name, proto, args, tstruct, assign, print)

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

#ifdef CONFIG_MODULE_SIG

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
		printk(KERN_WARNING "LTTng: tracepoint_module_notify symbol lookup failed. It probably means you kernel don't need this work-around. Please consider upgrading LTTng modules to make this warning go away.\n");
		return -ENOSYS;
	}
}

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

#else /* CONFIG_MODULE_SIG */

static inline
int wrapper_lttng_fixup_sig(struct module *mod)
{
	return 0;
}

#endif /* #else CONFIG_MODULE_SIG */

#endif /* _LTTNG_WRAPPER_TRACEPOINT_H */
