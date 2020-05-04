/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-wrapper.c
 *
 * Copyright (C) 2020 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <lttng-tracer.h>

static int __init lttng_wrapper_init(void)
{
	return 0;
}

module_init(lttng_wrapper_init);

static void __exit lttng_exit(void)
{
}

module_exit(lttng_exit);

#include "extra_version/patches.i"
#ifdef LTTNG_EXTRA_VERSION_GIT
MODULE_INFO(extra_version_git, LTTNG_EXTRA_VERSION_GIT);
#endif
#ifdef LTTNG_EXTRA_VERSION_NAME
MODULE_INFO(extra_version_name, LTTNG_EXTRA_VERSION_NAME);
#endif
MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng wrapper");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
