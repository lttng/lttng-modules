/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/kallsyms.h
 *
 * wrapper around kallsyms_lookup_name. Implements arch-dependent code for
 * arches where the address of the start of the function body is different
 * from the pointer which can be used to call the function, e.g. ARM THUMB2.
 *
 * Copyright (C) 2011 Avik Sil (avik.sil@linaro.org)
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_KALLSYMS_H
#define _LTTNG_WRAPPER_KALLSYMS_H

#include <linux/kallsyms.h>
#include <linux/version.h>

/*
 * PowerPC ABIv1 needs KALLSYMS_ALL to get the function descriptor,
 * which is needed to perform the function call.
 */
#if defined(CONFIG_PPC64) && (!defined(_CALL_ELF) || _CALL_ELF < 2)
# ifndef CONFIG_KALLSYMS_ALL
#  error "LTTng-modules requires CONFIG_KALLSYMS_ALL on PowerPC ABIv1"
# endif
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0))

unsigned long wrapper_kallsyms_lookup_name(const char *name);

#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)) */

static inline
unsigned long wrapper_kallsyms_lookup_name(const char *name)
{
	return kallsyms_lookup_name(name);
}

#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)) */

static inline
unsigned long kallsyms_lookup_funcptr(const char *name)
{
	unsigned long addr;

	addr = wrapper_kallsyms_lookup_name(name);
#ifdef CONFIG_ARM
#ifdef CONFIG_THUMB2_KERNEL
	if (addr)
		addr |= 1; /* set bit 0 in address for thumb mode */
#endif
#endif
	return addr;
}

static inline
unsigned long kallsyms_lookup_dataptr(const char *name)
{
	return wrapper_kallsyms_lookup_name(name);
}

#endif /* _LTTNG_WRAPPER_KALLSYMS_H */
