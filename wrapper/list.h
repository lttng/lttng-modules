#ifndef _LTTNG_WRAPPER_LIST_H
#define _LTTNG_WRAPPER_LIST_H

/*
 * wrapper/list.h
 *
 * wrapper around linux/list.h.
 *
 * Copyright (C) 2015 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; only version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * This wrapper code is derived from Linux 3.19.2 include/linux/list.h
 * and include/linux/rculist.h, hence the GPLv2 license applied to this
 * file.
 */

#include <linux/list.h>
#include <linux/rculist.h>

/*
 * return the first or the next element in an RCU protected hlist
 */
#define lttng_hlist_first_rcu(head)	(*((struct hlist_node __rcu **)(&(head)->first)))
#define lttng_hlist_next_rcu(node)	(*((struct hlist_node __rcu **)(&(node)->next)))
#define lttng_hlist_pprev_rcu(node)	(*((struct hlist_node __rcu **)((node)->pprev)))

#define lttng_hlist_entry_safe(ptr, type, member) \
	({ typeof(ptr) ____ptr = (ptr); \
	   ____ptr ? hlist_entry(____ptr, type, member) : NULL; \
	})

/**
 * lttng_hlist_for_each_entry	- iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the hlist_node within the struct.
 */
#define lttng_hlist_for_each_entry(pos, head, member)			\
	for (pos = lttng_hlist_entry_safe((head)->first, typeof(*(pos)), member);\
	     pos;							\
	     pos = lttng_hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

/**
 * lttng_hlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another &struct hlist_node to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the hlist_node within the struct.
 */
#define lttng_hlist_for_each_entry_safe(pos, n, head, member) 		\
	for (pos = lttng_hlist_entry_safe((head)->first, typeof(*pos), member);\
	     pos && ({ n = pos->member.next; 1; });			\
	     pos = lttng_hlist_entry_safe(n, typeof(*pos), member))

#endif /* _LTTNG_WRAPPER_LIST_H */
