/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-tracker-pid.c
 *
 * LTTng Process ID tracking.
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/seq_file.h>
#include <linux/stringify.h>
#include <linux/hash.h>
#include <linux/rcupdate.h>

#include <wrapper/tracepoint.h>
#include <wrapper/rcu.h>
#include <wrapper/list.h>
#include <lttng-events.h>

/*
 * Hash table is allocated and freed when there are no possible
 * concurrent lookups (ensured by the alloc/free caller). However,
 * there can be concurrent RCU lookups vs add/del operations.
 *
 * Concurrent updates of the PID hash table are forbidden: the caller
 * must ensure mutual exclusion. This is currently done by holding the
 * sessions_mutex across calls to create, destroy, add, and del
 * functions of this API.
 */
int lttng_id_tracker_get_node_id(const struct lttng_id_hash_node *node)
{
	return node->id;
}

/*
 * Lookup performed from RCU read-side critical section (RCU sched),
 * protected by preemption off at the tracepoint call site.
 * Return true if found, false if not found.
 */
bool lttng_id_tracker_lookup(struct lttng_id_tracker_rcu *p, int id)
{
	struct hlist_head *head;
	struct lttng_id_hash_node *e;
	uint32_t hash = hash_32(id, 32);

	head = &p->id_hash[hash & (LTTNG_ID_TABLE_SIZE - 1)];
	lttng_hlist_for_each_entry_rcu(e, head, hlist) {
		if (id == e->id)
			return true;	/* Found */
	}
	return false;
}
EXPORT_SYMBOL_GPL(lttng_id_tracker_lookup);

static struct lttng_id_tracker_rcu *lttng_id_tracker_rcu_create(void)
{
	struct lttng_id_tracker_rcu *tracker;

	tracker = kzalloc(sizeof(struct lttng_id_tracker_rcu), GFP_KERNEL);
	if (!tracker)
		return NULL;
	return tracker;
}

/*
 * Tracker add and del operations support concurrent RCU lookups.
 */
int lttng_id_tracker_add(struct lttng_id_tracker *lf, int id)
{
	struct hlist_head *head;
	struct lttng_id_hash_node *e;
	struct lttng_id_tracker_rcu *p = lf->p;
	uint32_t hash = hash_32(id, 32);
	bool allocated = false;

	if (!p) {
		p = lttng_id_tracker_rcu_create();
		if (!p)
			return -ENOMEM;
		allocated = true;
	}
	head = &p->id_hash[hash & (LTTNG_ID_TABLE_SIZE - 1)];
	lttng_hlist_for_each_entry(e, head, hlist) {
		if (id == e->id)
			return -EEXIST;
	}
	e = kmalloc(sizeof(struct lttng_id_hash_node), GFP_KERNEL);
	if (!e)
		return -ENOMEM;
	e->id = id;
	hlist_add_head_rcu(&e->hlist, head);
	if (allocated) {
		rcu_assign_pointer(lf->p, p);
	}
	return 0;
}

static
void id_tracker_del_node_rcu(struct lttng_id_hash_node *e)
{
	hlist_del_rcu(&e->hlist);
	/*
	 * We choose to use a heavyweight synchronize on removal here,
	 * since removal of an ID from the tracker mask is a rare
	 * operation, and we don't want to use more cache lines than
	 * what we really need when doing the ID lookups, so we don't
	 * want to afford adding a rcu_head field to those pid hash
	 * node.
	 */
	synchronize_trace();
	kfree(e);
}

/*
 * This removal is only used on destroy, so it does not need to support
 * concurrent RCU lookups.
 */
static
void id_tracker_del_node(struct lttng_id_hash_node *e)
{
	hlist_del(&e->hlist);
	kfree(e);
}

int lttng_id_tracker_del(struct lttng_id_tracker *lf, int id)
{
	struct hlist_head *head;
	struct lttng_id_hash_node *e;
	struct lttng_id_tracker_rcu *p = lf->p;
	uint32_t hash = hash_32(id, 32);

	if (!p)
		return -ENOENT;
	head = &p->id_hash[hash & (LTTNG_ID_TABLE_SIZE - 1)];
	/*
	 * No need of _safe iteration, because we stop traversal as soon
	 * as we remove the entry.
	 */
	lttng_hlist_for_each_entry(e, head, hlist) {
		if (id == e->id) {
			id_tracker_del_node_rcu(e);
			return 0;
		}
	}
	return -ENOENT;	/* Not found */
}

static void lttng_id_tracker_rcu_destroy(struct lttng_id_tracker_rcu *p)
{
	int i;

	if (!p)
		return;
	for (i = 0; i < LTTNG_ID_TABLE_SIZE; i++) {
		struct hlist_head *head = &p->id_hash[i];
		struct lttng_id_hash_node *e;
		struct hlist_node *tmp;

		lttng_hlist_for_each_entry_safe(e, tmp, head, hlist)
			id_tracker_del_node(e);
	}
	kfree(p);
}

int lttng_id_tracker_empty_set(struct lttng_id_tracker *lf)
{
	struct lttng_id_tracker_rcu *p, *oldp;

	p = lttng_id_tracker_rcu_create();
	if (!p)
		return -ENOMEM;
	oldp = lf->p;
	rcu_assign_pointer(lf->p, p);
	synchronize_trace();
	lttng_id_tracker_rcu_destroy(oldp);
	return 0;
}

void lttng_id_tracker_destroy(struct lttng_id_tracker *lf, bool rcu)
{
	struct lttng_id_tracker_rcu *p = lf->p;

	if (!lf->p)
		return;
	rcu_assign_pointer(lf->p, NULL);
	if (rcu)
		synchronize_trace();
	lttng_id_tracker_rcu_destroy(p);
}
