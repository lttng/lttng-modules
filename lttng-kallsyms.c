/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-kallsyms.c
 *
 * Copyright (C) 2019 Geneviève Bastien <gbastien@versatic.net>
 */

#include <linux/module.h>
#include <linux/kallsyms.h>
#include <lttng-events.h>
#include <lttng-kallsyms.h>

/* Define the tracepoints, but do not build the probes */
#define CREATE_TRACE_POINTS
#define TRACE_INCLUDE_PATH instrumentation/events/lttng-module
#define TRACE_INCLUDE_FILE lttng-kallsyms
#define LTTNG_INSTRUMENTATION
#include <instrumentation/events/lttng-module/lttng-kallsyms.h>

DEFINE_TRACE(lttng_kallsyms_kernel_symbol);

/*
 * Trace the kernel symbols from a given module
 *
 * data: The lttng_session instance
 * symbol_name: The function name this symbol resolves to
 * module: The module containing this symbol. Can be NULL
 * symbol_addr: The symbol address
 */
static
int _lttng_one_symbol_received(void * data, const char * symbol_name,
		struct module * module, unsigned long symbol_addr)
{
	struct lttng_session *session = data;

	if (module) {
		trace_lttng_kallsyms_kernel_symbol(session, symbol_addr,
				symbol_name, module->name);
	} else {
		trace_lttng_kallsyms_kernel_symbol(session, symbol_addr,
				symbol_name, "");
	}

	return 0;
}

int lttng_enumerate_kernel_symbols(struct lttng_session *session)
{
	int ret = 0;

	ret = kallsyms_on_each_symbol(_lttng_one_symbol_received, (void *) session);

	return ret;
}
EXPORT_SYMBOL_GPL(lttng_enumerate_kernel_symbols);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Geneviève Bastien <gbastien@versatic.net");
MODULE_DESCRIPTION("LTTng kallsyms");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
