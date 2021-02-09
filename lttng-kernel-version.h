/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-kernel-version.h
 *
 * Contains helpers to check kernel version conditions.
 *
 * Copyright (C) 2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_KERNEL_VERSION_H
#define _LTTNG_KERNEL_VERSION_H

#include <linux/version.h>
#include <generated/utsrelease.h>

/*
 * The following defines are extracted from the toplevel Linux Makefile and
 * passed on the command line -with '-D'.
 */

#ifndef LTTNG_LINUX_MAJOR
#define LTTNG_LINUX_MAJOR 0
#endif

#ifndef LTTNG_LINUX_MINOR
#define LTTNG_LINUX_MINOR 0
#endif

#ifndef LTTNG_LINUX_PATCH
#define LTTNG_LINUX_PATCH 0
#endif

/*
 * Some stable releases have overflowed the 8bits allocated to the sublevel in
 * the version code. To determine if the current kernel is affected, use the
 * sublevel version from the Makefile. This is currently true for the 4.4.256
 * and 4.9.256 stable releases.
 *
 * When the sublevel has overflowed, use the values from the Makefile instead
 * of LINUX_VERSION_CODE from the kernel headers and allocate 16bits.
 * Otherwise, keep using the version code from the headers to minimise the
 * behavior change and avoid regressions.
 *
 * Cast the result to uint64_t to prevent overflowing when we append distro
 * specific version information.
 */
#if (LTTNG_LINUX_PATCH >= 256)

#define LTTNG_KERNEL_VERSION(a, b, c) \
	((((a) << 24) + ((b) << 16) + (c)) * 1ULL)

#define LTTNG_LINUX_VERSION_CODE \
	LTTNG_KERNEL_VERSION(LTTNG_LINUX_MAJOR, LTTNG_LINUX_MINOR, LTTNG_LINUX_PATCH)

#else

#define LTTNG_KERNEL_VERSION(a, b, c) (KERNEL_VERSION(a, b, c) * 1ULL)
#define LTTNG_LINUX_VERSION_CODE (LINUX_VERSION_CODE * 1ULL)

#endif

/*
 * This macro checks if the kernel version is between the two specified
 * versions (lower limit inclusive, upper limit exclusive).
 */
#define LTTNG_KERNEL_RANGE(a_low, b_low, c_low, a_high, b_high, c_high) \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(a_low, b_low, c_low) && \
	 LTTNG_LINUX_VERSION_CODE < LTTNG_KERNEL_VERSION(a_high, b_high, c_high))

/* Ubuntu */

#define LTTNG_UBUNTU_KERNEL_VERSION(a, b, c, d) \
	(((LTTNG_KERNEL_VERSION(a, b, c)) << 16) + (d))

#ifdef UTS_UBUNTU_RELEASE_ABI
#define LTTNG_UBUNTU_VERSION_CODE \
	((LTTNG_LINUX_VERSION_CODE << 16) + UTS_UBUNTU_RELEASE_ABI)
#else
#define LTTNG_UBUNTU_VERSION_CODE 	0
#endif

#define LTTNG_UBUNTU_KERNEL_RANGE(a_low, b_low, c_low, d_low, \
		a_high, b_high, c_high, d_high) \
	(LTTNG_UBUNTU_VERSION_CODE >= \
		LTTNG_UBUNTU_KERNEL_VERSION(a_low, b_low, c_low, d_low) && \
		LTTNG_UBUNTU_VERSION_CODE < \
		LTTNG_UBUNTU_KERNEL_VERSION(a_high, b_high, c_high, d_high))

/* Debian */

#define LTTNG_DEBIAN_KERNEL_VERSION(a, b, c, d, e, f) \
	(((LTTNG_KERNEL_VERSION(a, b, c)) * 1000000ULL) + ((d) * 10000) + ((e) * 100) + (f))

#ifdef DEBIAN_API_VERSION
#define LTTNG_DEBIAN_VERSION_CODE \
	((LTTNG_LINUX_VERSION_CODE * 1000000ULL) + DEBIAN_API_VERSION)
#else
#define LTTNG_DEBIAN_VERSION_CODE	0
#endif

#define LTTNG_DEBIAN_KERNEL_RANGE(a_low, b_low, c_low, d_low, e_low, f_low, \
		a_high, b_high, c_high, d_high, e_high, f_high) \
	(LTTNG_DEBIAN_VERSION_CODE >= \
		LTTNG_DEBIAN_KERNEL_VERSION(a_low, b_low, c_low, d_low, e_low, f_low) && \
		LTTNG_DEBIAN_VERSION_CODE < \
		LTTNG_DEBIAN_KERNEL_VERSION(a_high, b_high, c_high, d_high, e_high, f_high))

#define LTTNG_RHEL_KERNEL_VERSION(a, b, c, d, e, f) \
	(((LTTNG_KERNEL_VERSION(a, b, c)) * 100000000ULL) + ((d) * 10000) + ((e) * 100) + (f))

/* RHEL */

#ifdef RHEL_API_VERSION
#define LTTNG_RHEL_VERSION_CODE \
	((LTTNG_LINUX_VERSION_CODE * 100000000ULL) + RHEL_API_VERSION)
#else
#define LTTNG_RHEL_VERSION_CODE		0
#endif

#define LTTNG_RHEL_KERNEL_RANGE(a_low, b_low, c_low, d_low, e_low, f_low, \
		a_high, b_high, c_high, d_high, e_high, f_high) \
	(LTTNG_RHEL_VERSION_CODE >= \
		LTTNG_RHEL_KERNEL_VERSION(a_low, b_low, c_low, d_low, e_low, f_low) && \
		LTTNG_RHEL_VERSION_CODE < \
		LTTNG_RHEL_KERNEL_VERSION(a_high, b_high, c_high, d_high, e_high, f_high))

/* SUSE Linux enterprise */

#define LTTNG_SLE_KERNEL_VERSION(a, b, c, d, e, f) \
	(((LTTNG_KERNEL_VERSION(a, b, c)) * 100000000ULL) + ((d) * 100000) + ((e) * 100) + (f))

#ifdef SLE_API_VERSION
#define LTTNG_SLE_VERSION_CODE \
	((LTTNG_LINUX_VERSION_CODE * 100000000ULL) + SLE_API_VERSION)
#else
#define LTTNG_SLE_VERSION_CODE 	0
#endif

#define LTTNG_SLE_KERNEL_RANGE(a_low, b_low, c_low, d_low, e_low, f_low, \
		a_high, b_high, c_high, d_high, e_high, f_high) \
	(LTTNG_SLE_VERSION_CODE >= \
		LTTNG_SLE_KERNEL_VERSION(a_low, b_low, c_low, d_low, e_low, f_low) && \
		LTTNG_SLE_VERSION_CODE < \
		LTTNG_SLE_KERNEL_VERSION(a_high, b_high, c_high, d_high, e_high, f_high))

/* Fedora */

#define LTTNG_FEDORA_KERNEL_VERSION(a, b, c, d) \
        (((LTTNG_KERNEL_VERSION(a, b, c)) * 10000ULL) + (d))

#ifdef FEDORA_REVISION_VERSION
#define LTTNG_FEDORA_VERSION_CODE \
        ((LTTNG_LINUX_VERSION_CODE * 10000ULL) + FEDORA_REVISION_VERSION)
#else
#define LTTNG_FEDORA_VERSION_CODE         0
#endif

#define LTTNG_FEDORA_KERNEL_RANGE(a_low, b_low, c_low, d_low, \
                a_high, b_high, c_high, d_high) \
        (LTTNG_FEDORA_VERSION_CODE >= \
                LTTNG_FEDORA_KERNEL_VERSION(a_low, b_low, c_low, d_low) && \
                LTTNG_FEDORA_VERSION_CODE < \
                LTTNG_FEDORA_KERNEL_VERSION(a_high, b_high, c_high, d_high))

/* RT patch */

#define LTTNG_RT_KERNEL_VERSION(a, b, c, d) \
	(((LTTNG_KERNEL_VERSION(a, b, c)) << 16) + (d))

#ifdef RT_PATCH_VERSION
#define LTTNG_RT_VERSION_CODE \
	((LTTNG_LINUX_VERSION_CODE << 16) + RT_PATCH_VERSION)
#else
#define LTTNG_RT_VERSION_CODE 	0
#endif

#define LTTNG_RT_KERNEL_RANGE(a_low, b_low, c_low, d_low, \
		a_high, b_high, c_high, d_high) \
	(LTTNG_RT_VERSION_CODE >= \
		LTTNG_RT_KERNEL_VERSION(a_low, b_low, c_low, d_low) && \
		LTTNG_RT_VERSION_CODE < \
		LTTNG_RT_KERNEL_VERSION(a_high, b_high, c_high, d_high))

#endif /* _LTTNG_KERNEL_VERSION_H */
