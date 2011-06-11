/*
 * Copyright (C) 2011 Mathieu Desnoyers (mathieu.desnoyers@efficios.com)
 *
 * wrapper around poll __pollwait and poll_get_entry. Using KALLSYMS to get its
 * address when available, else we need to have a kernel that exports this
 * function to GPL modules.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#ifdef CONFIG_KALLSYMS

#include <linux/kallsyms.h>
#include <linux/poll.h>

struct poll_table_entry;
struct splice_pipe_desc;

static
void (*__pollwait_sym)(struct file *filp, wait_queue_head_t *wait_address,
		poll_table *p);
static
struct poll_table_entry *(*poll_get_entry_sym)(struct poll_wqueues *p);

void wrapper_pollwait_exclusive(struct file *filp,
			 wait_queue_head_t *wait_address,
			 poll_table *p)

{
	struct poll_wqueues *pwq = container_of(p, struct poll_wqueues, pt);
	struct poll_table_entry *entry;

	if (!poll_get_entry_sym)
		poll_get_entry_sym = (void *) kallsyms_lookup_name("poll_get_entry");
	if (!poll_get_entry_sym) {
		printk(KERN_WARNING "LTTng: poll_get_entry_sym symbol lookup failed.\n");
		return;
	}
	entry = poll_get_entry_sym(pwq);

	if (!__pollwait_sym)
		__pollwait_sym = (void *) kallsyms_lookup_name("__pollwait");
	if (!__pollwait_sym) {
		printk(KERN_WARNING "LTTng: __pollwait symbol lookup failed.\n");
		return;
	}
	return __pollwait_sym(filp, wait_address, p);
}

#else

#include <linux/poll.h>

ssize_t wrapper_pollwait_exclusive(struct file *filp,
			wait_queue_head_t *wait_address,
			poll_table *p)
{
	return pollwait_exclusive(filp, wait_address, p);
}

#endif
