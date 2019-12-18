/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1) */
#ifndef _LTTNG_UTILS_H
#define _LTTNG_UTILS_H

/*
 * Copyright (C) 2020 Francis Deslauriers <francis.deslauriers@efficios.com>
 */

#include <linux/jhash.h>
#include <linux/string.h>
#include <linux/types.h>

static inline
struct hlist_head *utils_borrow_hash_table_bucket(
		struct hlist_head *hash_table,
		unsigned int hash_table_size,
		const char *event_name)
{
	size_t name_len;
	uint32_t hash;

	name_len = strlen(event_name);

	hash = jhash(event_name, name_len, 0);
	return &hash_table[hash & (hash_table_size - 1)];
}
#endif /* _LTTNG_UTILS_H */
