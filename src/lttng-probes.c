/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-probes.c
 *
 * Holds LTTng probes registry.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/seq_file.h>

#include <lttng/events.h>

/*
 * probe list is protected by sessions lock.
 */
static LIST_HEAD(_probe_list);

/*
 * List of probes registered by not yet processed.
 */
static LIST_HEAD(lazy_probe_init);

/*
 * lazy_nesting counter ensures we don't notify lazy probe registration
 * fixup while we are performing the fixup. It is protected by the
 * sessions lock.
 */
static int lazy_nesting;

DEFINE_PER_CPU(struct lttng_dynamic_len_stack, lttng_dynamic_len_stack);

EXPORT_PER_CPU_SYMBOL_GPL(lttng_dynamic_len_stack);

/*
 * Called under sessions lock.
 */
static
int check_event_provider(struct lttng_kernel_probe_desc *desc)
{
	int i, mismatch = 0;
	size_t provider_name_len;

	provider_name_len = strnlen(desc->provider_name,
				LTTNG_KERNEL_ABI_SYM_NAME_LEN - 1);
	for (i = 0; i < desc->nr_events; i++) {
		/*
		 * The event name needs to start with provider name + _ +
		 * one or more letter.
		 */
		if (strncmp(desc->event_desc[i]->event_name, desc->provider_name, provider_name_len))
			mismatch = 1;
		else if (strlen(desc->event_desc[i]->event_name) <= provider_name_len + 1)
			mismatch = 1;
		else if (desc->event_desc[i]->event_name[provider_name_len] != '_')
			mismatch = 1;

		if (mismatch) {
			printk(KERN_WARNING "LTTng: event provider mismatch: "
				"The event name needs to start with provider "
				"name + _ + one or more letter, "
				"provider: %s, event name: %s\n",
				desc->provider_name, desc->event_desc[i]->event_name);
			return 0;
		}
	}
	return 1;
}

/*
 * Called under sessions lock.
 */
static
void lttng_lazy_probe_register(struct lttng_kernel_probe_desc *desc)
{
	struct lttng_kernel_probe_desc *iter;
	struct list_head *probe_list;

	/*
	 * Each provider enforce that every event name begins with the
	 * provider name. Check this in an assertion for extra
	 * carefulness. This ensures we cannot have duplicate event
	 * names across providers.
	 */
	WARN_ON_ONCE(!check_event_provider(desc));

	/*
	 * The provider ensures there are no duplicate event names.
	 * Duplicated TRACEPOINT_EVENT event names would generate a
	 * compile-time error due to duplicated symbol names.
	 */

	/*
	 * We sort the providers by struct lttng_kernel_probe_desc pointer
	 * address.
	 */
	probe_list = &_probe_list;
	list_for_each_entry_reverse(iter, probe_list, head) {
		BUG_ON(iter == desc); /* Should never be in the list twice */
		if (iter < desc) {
			/* We belong to the location right after iter. */
			list_add(&desc->head, &iter->head);
			goto desc_added;
		}
	}
	/* We should be added at the head of the list */
	list_add(&desc->head, probe_list);
desc_added:
	pr_debug("LTTng: just registered probe %s containing %u events\n",
		desc->provider_name, desc->nr_events);
}

/*
 * Called under sessions lock.
 */
static
void fixup_lazy_probes(void)
{
	struct lttng_kernel_probe_desc *iter, *tmp;
	int ret;

	lazy_nesting++;
	list_for_each_entry_safe(iter, tmp,
			&lazy_probe_init, lazy_init_head) {
		lttng_lazy_probe_register(iter);
		iter->lazy = 0;
		list_del(&iter->lazy_init_head);
	}
	ret = lttng_fix_pending_events();
	WARN_ON_ONCE(ret);
	ret = lttng_fix_pending_event_notifiers();
	WARN_ON_ONCE(ret);
	lazy_nesting--;
}

/*
 * Called under sessions lock.
 */
struct list_head *lttng_get_probe_list_head(void)
{
	if (!lazy_nesting && !list_empty(&lazy_probe_init))
		fixup_lazy_probes();
	return &_probe_list;
}

static
const struct lttng_kernel_probe_desc *find_provider(const char *provider)
{
	struct lttng_kernel_probe_desc *iter;
	struct list_head *probe_list;

	probe_list = lttng_get_probe_list_head();
	list_for_each_entry(iter, probe_list, head) {
		if (!strcmp(iter->provider_name, provider))
			return iter;
	}
	return NULL;
}

int lttng_probe_register(struct lttng_kernel_probe_desc *desc)
{
	int ret = 0;

	lttng_lock_sessions();

	/*
	 * Check if the provider has already been registered.
	 */
	if (find_provider(desc->provider_name)) {
		ret = -EEXIST;
		goto end;
	}
	list_add(&desc->lazy_init_head, &lazy_probe_init);
	desc->lazy = 1;
	pr_debug("LTTng: adding probe %s containing %u events to lazy registration list\n",
		desc->provider_name, desc->nr_events);
	/*
	 * If there is at least one active session, we need to register
	 * the probe immediately, since we cannot delay event
	 * registration because they are needed ASAP.
	 */
	if (lttng_session_active() || lttng_event_notifier_active())
		fixup_lazy_probes();
end:
	lttng_unlock_sessions();
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_probe_register);

void lttng_probe_unregister(struct lttng_kernel_probe_desc *desc)
{
	lttng_lock_sessions();
	if (!desc->lazy)
		list_del(&desc->head);
	else
		list_del(&desc->lazy_init_head);
	pr_debug("LTTng: just unregistered probe %s\n", desc->provider_name);
	lttng_unlock_sessions();
}
EXPORT_SYMBOL_GPL(lttng_probe_unregister);

/*
 * TODO: this is O(nr_probes * nb_events), could be faster.
 * Called with sessions lock held.
 */
static
const struct lttng_kernel_event_desc *find_event_desc(const char *name)
{
	struct lttng_kernel_probe_desc *probe_desc;
	int i;

	list_for_each_entry(probe_desc, &_probe_list, head) {
		for (i = 0; i < probe_desc->nr_events; i++) {
			if (!strcmp(probe_desc->event_desc[i]->event_name, name))
				return probe_desc->event_desc[i];
			}
	}
	return NULL;
}

/*
 * Called with sessions lock held.
 */
const struct lttng_kernel_event_desc *lttng_event_desc_get(const char *name)
{
	const struct lttng_kernel_event_desc *event_desc;
	int ret;

	event_desc = find_event_desc(name);
	if (!event_desc)
		return NULL;
	ret = try_module_get(event_desc->owner);
	WARN_ON_ONCE(!ret);
	return event_desc;
}
EXPORT_SYMBOL_GPL(lttng_event_desc_get);

/*
 * Called with sessions lock held.
 */
void lttng_event_desc_put(const struct lttng_kernel_event_desc *event_desc)
{
	module_put(event_desc->owner);
}
EXPORT_SYMBOL_GPL(lttng_event_desc_put);

static
void *tp_list_start(struct seq_file *m, loff_t *pos)
{
	struct lttng_kernel_probe_desc *probe_desc;
	struct list_head *probe_list;
	int iter = 0, i;

	lttng_lock_sessions();
	probe_list = lttng_get_probe_list_head();
	list_for_each_entry(probe_desc, probe_list, head) {
		for (i = 0; i < probe_desc->nr_events; i++) {
			if (iter++ >= *pos)
				return (void *) probe_desc->event_desc[i];
		}
	}
	/* End of list */
	return NULL;
}

static
void *tp_list_next(struct seq_file *m, void *p, loff_t *ppos)
{
	struct lttng_kernel_probe_desc *probe_desc;
	struct list_head *probe_list;
	int iter = 0, i;

	(*ppos)++;
	probe_list = lttng_get_probe_list_head();
	list_for_each_entry(probe_desc, probe_list, head) {
		for (i = 0; i < probe_desc->nr_events; i++) {
			if (iter++ >= *ppos)
				return (void *) probe_desc->event_desc[i];
		}
	}
	/* End of list */
	return NULL;
}

static
void tp_list_stop(struct seq_file *m, void *p)
{
	lttng_unlock_sessions();
}

static
int tp_list_show(struct seq_file *m, void *p)
{
	const struct lttng_kernel_event_desc *probe_desc = p;

	seq_printf(m,	"event { name = %s; };\n",
		   probe_desc->event_name);
	return 0;
}

static
const struct seq_operations lttng_tracepoint_list_seq_ops = {
	.start = tp_list_start,
	.next = tp_list_next,
	.stop = tp_list_stop,
	.show = tp_list_show,
};

static
int lttng_tracepoint_list_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &lttng_tracepoint_list_seq_ops);
}

const struct file_operations lttng_tracepoint_list_fops = {
	.owner = THIS_MODULE,
	.open = lttng_tracepoint_list_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

int lttng_probes_init(void)
{
	int cpu;

	for_each_possible_cpu(cpu)
		per_cpu_ptr(&lttng_dynamic_len_stack, cpu)->offset = 0;
	return 0;
}
