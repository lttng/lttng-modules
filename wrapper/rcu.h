/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/rcu.h
 *
 * wrapper around linux/rcupdate.h and linux/rculist.h.
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_RCU_H
#define _LTTNG_WRAPPER_RCU_H

#include <linux/version.h>
#include <linux/rculist.h>
#include <linux/rcupdate.h>
#include <wrapper/list.h>

#ifndef rcu_dereference_raw_notrace
#define rcu_dereference_raw_notrace(p)	rcu_dereference_raw(p)
#endif

#define lttng_rcu_dereference(p) rcu_dereference_raw_notrace(p)

/**
 * lttng_list_entry_rcu - get the struct for this entry
 * @ptr:        the &struct list_head pointer.
 * @type:       the type of the struct this is embedded in.
 * @member:     the name of the list_head within the struct.
 *
 * This primitive may safely run concurrently with the _rcu list-mutation
 * primitives such as list_add_rcu() as long as it's guarded by
 * rcu_read_lock_sched().
 * Can be used while tracing RCU.
 */
#define lttng_list_entry_rcu(ptr, type, member) \
({ \
	typeof(*ptr) __rcu *__ptr = (typeof(*ptr) __rcu __force *)ptr; \
	container_of((typeof(ptr))lttng_rcu_dereference(__ptr), type, member); \
})

/**
 * lttng_list_for_each_entry_rcu - iterate over rcu list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * This list-traversal primitive may safely run concurrently with
 * the _rcu list-mutation primitives such as list_add_rcu()
 * as long as the traversal is guarded by rcu_read_lock_sched().
 * Can be used while tracing RCU.
 */
#define lttng_list_for_each_entry_rcu(pos, head, member) \
	for (pos = lttng_list_entry_rcu((head)->next, typeof(*pos), member); \
		&pos->member != (head); \
		pos = lttng_list_entry_rcu(pos->member.next, typeof(*pos), member))

/**
 * lttng_hlist_for_each_entry_rcu - iterate over rcu list of given type (for tracing)
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the hlist_node within the struct.
 *
 * This list-traversal primitive may safely run concurrently with
 * the _rcu list-mutation primitives such as hlist_add_head_rcu()
 * as long as the traversal is guarded by rcu_read_lock().
 *
 * This is the same as hlist_for_each_entry_rcu() except that it does
 * not do any RCU debugging or tracing.
 */
#define lttng_hlist_for_each_entry_rcu(pos, head, member)	\
	for (pos = lttng_hlist_entry_safe (lttng_rcu_dereference(lttng_hlist_first_rcu(head)), \
			typeof(*(pos)), member);		\
		pos;						\
		pos = lttng_hlist_entry_safe(lttng_rcu_dereference(lttng_hlist_next_rcu( \
			&(pos)->member)), typeof(*(pos)), member))

#endif /* _LTTNG_WRAPPER_RCU_H */
