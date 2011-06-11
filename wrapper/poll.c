/*
 * Copyright (C) 2011 Mathieu Desnoyers (mathieu.desnoyers@efficios.com)
 *
 * wrapper around poll_get_entry, implementing __pollwait with exclusive
 * wakeups. Using KALLSYMS to get poll_get_entry address when available,
 * else we need to have a kernel that exports this function to GPL
 * modules.
 *
 * GPL v2 license.
 */

#ifdef CONFIG_KALLSYMS

#include <linux/kallsyms.h>
#include <linux/poll.h>
#include <linux/wait.h>

struct poll_table_entry;
struct splice_pipe_desc;

static
int (*pollwake_sym)(wait_queue_t *wait, unsigned mode, int sync, void *key);
static
struct poll_table_entry *(*poll_get_entry_sym)(struct poll_wqueues *p);

/* Add a new entry */
static void __pollwait_exclusive(struct file *filp,
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

	if (!pollwake_sym)
		pollwake_sym = (void *) kallsyms_lookup_name("pollwake");
	if (!pollwake_sym) {
		printk(KERN_WARNING "LTTng: pollwake_sym symbol lookup failed.\n");
		return;
	}

	entry = poll_get_entry_sym(pwq);

	if (!entry)
		return;
	get_file(filp);
	entry->filp = filp;
	entry->wait_address = wait_address;
	entry->key = p->key;
	init_waitqueue_func_entry(&entry->wait, pollwake_sym);
	entry->wait.private = pwq;
	add_wait_queue_exclusive(wait_address, &entry->wait);
}

void wrapper_pollwait_exclusive(struct file *filp,
			 wait_queue_head_t *wait_address,
			 poll_table *p)

{
	__pollwait_exclusive(filp, wait_address, p);
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
