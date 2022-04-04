/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * src/wrapper/kprobes.h
 *
 * Copyright (C) 2021 Michael Jeanson <mjeanson@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_KPROBES_H
#define _LTTNG_WRAPPER_KPROBES_H

#include <linux/kprobes.h>
#include <lttng/kernel-version.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,11,0))

static inline
struct kretprobe *lttng_get_kretprobe(struct kretprobe_instance *ri)
{
	return get_kretprobe(ri);
}

#else /* LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,11,0) */

static inline
struct kretprobe *lttng_get_kretprobe(struct kretprobe_instance *ri)
{
	return ri->rp;
}

#endif /* LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,11,0) */


#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,18,0))
static inline
unsigned long lttng_get_kretprobe_retaddr(struct kretprobe_instance *ri)
{
	return get_kretprobe_retaddr(ri);
}

#else

static inline
unsigned long lttng_get_kretprobe_retaddr(struct kretprobe_instance *ri)
{
	return (unsigned long) ri->ret_addr;
}
#endif

#endif /* _LTTNG_WRAPPER_KPROBES_H */
