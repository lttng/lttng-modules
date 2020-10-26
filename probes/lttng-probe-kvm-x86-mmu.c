/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * probes/lttng-probe-kvm.c
 *
 * LTTng kvm probes.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <linux/kvm_host.h>
#include <lttng-tracer.h>
#include <lttng-kernel-version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,1,0))
#include <kvm/iodev.h>
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,1,0)) */
#include <../../virt/kvm/iodev.h>
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,1,0)) */

/*
 * Create the tracepoint static inlines from the kernel to validate that our
 * trace event macros match the kernel we run on.
 */
#include <wrapper/tracepoint.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,9,0))
#include <../../arch/x86/kvm/mmu/mmu_internal.h>
#include <../../arch/x86/kvm/mmu/mmutrace.h>
#else
#include <../../arch/x86/kvm/mmutrace.h>
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0))
#include <../arch/x86/kvm/mmu.h>
#include <../arch/x86/kvm/mmu/spte.h>
#endif

#undef TRACE_INCLUDE_PATH
#undef TRACE_INCLUDE_FILE

/*
 * Create LTTng tracepoint probes.
 */
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS

#define TRACE_INCLUDE_PATH instrumentation/events/lttng-module/arch/x86/kvm
#include <instrumentation/events/lttng-module/arch/x86/kvm/mmutrace.h>

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng kvm mmu probes");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
