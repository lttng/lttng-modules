/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * src/wrapper/kprobes.h
 *
 * Copyright (C) 2021 Michael Jeanson <mjeanson@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_KPROBES_H
#define _LTTNG_WRAPPER_KPROBES_H

#include <linux/kprobes.h>
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,11,0))

static inline
struct kretprobe *lttng_get_kretprobe(struct kretprobe_instance *ri)
{
	return get_kretprobe(ri);
}

#else /* LINUX_VERSION_CODE >= KERNEL_VERSION(5,11,0) */

static inline
struct kretprobe *lttng_get_kretprobe(struct kretprobe_instance *ri)
{
	return ri->rp;
}

#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(5,11,0) */

#endif /* _LTTNG_WRAPPER_KPROBES_H */
