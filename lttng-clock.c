/*
 * lttng-clock.c
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
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

#include <linux/module.h>
#include <linux/kmod.h>
#include <linux/mutex.h>

#include "wrapper/trace-clock.h"
#include "lttng-events.h"
#include "lttng-tracer.h"

struct lttng_trace_clock *lttng_trace_clock;
EXPORT_SYMBOL_GPL(lttng_trace_clock);

static DEFINE_MUTEX(clock_mutex);
static struct module *lttng_trace_clock_mod;	/* plugin */
static int clock_used;				/* refcount */

int lttng_clock_register_plugin(struct lttng_trace_clock *ltc,
		struct module *mod)
{
	int ret = 0;

	mutex_lock(&clock_mutex);
	if (clock_used) {
		ret = -EBUSY;
		goto end;
	}
	if (lttng_trace_clock_mod) {
		ret = -EEXIST;
		goto end;
	}
	/* set clock */
	ACCESS_ONCE(lttng_trace_clock) = ltc;
	lttng_trace_clock_mod = mod;
end:
	mutex_unlock(&clock_mutex);
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_clock_register_plugin);

void lttng_clock_unregister_plugin(struct lttng_trace_clock *ltc,
		struct module *mod)
{
	mutex_lock(&clock_mutex);
	WARN_ON_ONCE(clock_used);
	if (!lttng_trace_clock_mod) {
		goto end;
	}
	WARN_ON_ONCE(lttng_trace_clock_mod != mod);

	ACCESS_ONCE(lttng_trace_clock) = NULL;
	lttng_trace_clock_mod = NULL;
end:
	mutex_unlock(&clock_mutex);
}
EXPORT_SYMBOL_GPL(lttng_clock_unregister_plugin);

void lttng_clock_ref(void)
{
	mutex_lock(&clock_mutex);
	clock_used++;
	if (lttng_trace_clock_mod) {
		int ret;

		ret = try_module_get(lttng_trace_clock_mod);
		if (!ret) {
			printk(KERN_ERR "LTTng-clock cannot get clock plugin module\n");
			ACCESS_ONCE(lttng_trace_clock) = NULL;
			lttng_trace_clock_mod = NULL;
		}
	}
	mutex_unlock(&clock_mutex);
}
EXPORT_SYMBOL_GPL(lttng_clock_ref);

void lttng_clock_unref(void)
{
	mutex_lock(&clock_mutex);
	clock_used--;
	if (lttng_trace_clock_mod)
		module_put(lttng_trace_clock_mod);
	mutex_unlock(&clock_mutex);
}
EXPORT_SYMBOL_GPL(lttng_clock_unref);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng Clock");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
