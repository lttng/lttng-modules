/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-clock-plugin-test.c
 *
 * Copyright (C) 2014, 2016 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <linux/ktime.h>
#include <linux/hrtimer.h>
#include <linux/time.h>

#include <lttng-tracer.h>
#include <lttng-clock.h>	/* From lttng-modules */

static u64 trace_clock_read64_example(void)
{
	/* Freeze time. */
	return 0;
}

static u64 trace_clock_freq_example(void)
{
	return 1000;	/* 1KHz */
}

static int trace_clock_uuid_example(char *uuid)
{
	const char myuuid[] = "83c63deb-7aa4-48fb-abda-946f400d76e6";
	memcpy(uuid, myuuid, LTTNG_MODULES_UUID_STR_LEN);
	return 0;
}

static const char *trace_clock_name_example(void)
{
	return "lttng_test_clock_override";
}

static const char *trace_clock_description_example(void)
{
	return "Freeze time with 1KHz for regression test";
}

static
struct lttng_trace_clock ltc = {
	.read64 = trace_clock_read64_example,
	.freq = trace_clock_freq_example,
	.uuid = trace_clock_uuid_example,
	.name = trace_clock_name_example,
	.description = trace_clock_description_example,
};

static __init
int lttng_clock_plugin_init(void)
{
	return lttng_clock_register_plugin(&ltc, THIS_MODULE);
}
fs_initcall(lttng_clock_plugin_init);

static __exit
void lttng_clock_plugin_exit(void)
{
	lttng_clock_unregister_plugin(&ltc, THIS_MODULE);
}
module_exit(lttng_clock_plugin_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng Clock Plugin Example");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
