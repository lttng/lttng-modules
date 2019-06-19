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
DEFINE_TRACE(lttng_kallsyms_new_module_symbol);
DEFINE_TRACE(lttng_kallsyms_module_unloaded);

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

#ifdef CONFIG_MODULES

/*
 * Trace the symbols coming from a specific module
 *
 * data: The new module for which we want the symbols
 * symbol_name: The function name this symbol resolves to
 * module: The module containing this symbol. Can be NULL
 * symbol_addr: The symbol address
 */
static
int _lttng_trace_module_symbol(void * data, const char * symbol_name,
		struct module * module, unsigned long symbol_addr)
{
	struct module *mod = data;

	// Trace the symbols from the new module
	if (mod == module) {
		trace_lttng_kallsyms_new_module_symbol(symbol_addr, symbol_name, mod->name);
	}
	return 0;
}

static
int lttng_kallsyms_module_coming(struct module *mod)
{
	int ret = 0;

	ret = kallsyms_on_each_symbol(_lttng_trace_module_symbol, (void *) mod);

	return ret;
}

static
int lttng_kallsyms_module_going(struct module *mod)
{
	trace_lttng_kallsyms_module_unloaded(mod->name);
	return 0;
}

static
int lttng_kallsyms_module_notify(struct notifier_block *self,
		unsigned long val, void *data)
{
	struct module *mod = data;
	int ret = 0;

	switch (val) {
	case MODULE_STATE_COMING:
		ret = lttng_kallsyms_module_coming(mod);
		break;
	case MODULE_STATE_GOING:
		ret = lttng_kallsyms_module_going(mod);
		break;
	default:
		break;
	}
	return ret;
}

static
struct notifier_block lttng_kallsyms_module_notifier = {
	.notifier_call = lttng_kallsyms_module_notify,
	.priority = 0,
};

static
int lttng_kallsyms_module_init(void)
{
	return register_module_notifier(&lttng_kallsyms_module_notifier);
}

static
void lttng_kallsyms_module_exit(void)
{
	WARN_ON(unregister_module_notifier(&lttng_kallsyms_module_notifier));
}

#else /* #ifdef CONFIG_MODULES */

static
int lttng_kallsyms_module_init(void)
{
	return 0;
}

static
void lttng_kallsyms_module_exit(void)
{

}

#endif /* #else #ifdef CONFIG_MODULES */

module_init(lttng_kallsyms_module_init);

module_exit(lttng_kallsyms_module_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Geneviève Bastien <gbastien@versatic.net");
MODULE_DESCRIPTION("LTTng kallsyms");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
