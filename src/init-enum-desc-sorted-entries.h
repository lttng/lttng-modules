/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * init-enum-desc-sorted-entries.h
 *
 * Initialize the `sorted_entries` field of enumeration descriptors.
 *
 * Copyright (C) 2025 Philippe Proulx <pproulx@efficios.com>
 */

#ifndef _LTTNG_INIT_ENUM_DESC_SORTED_ENTRIES_H
#define _LTTNG_INIT_ENUM_DESC_SORTED_ENTRIES_H

#include <lttng/events.h>
#include <lttng/events-internal.h>

/*
 * Initializes the `lttng_kernel_enum_desc::sorted_entries` field for
 * each instance of `struct lttng_kernel_enum_desc` recursively found
 * in `type`.
 *
 * See `struct lttng_kernel_enum_desc` to learn the role of
 * the `sorted_entries` field.
 */
void init_type_enum_desc_sorted_entries(
		const struct lttng_kernel_type_common * type);

/*
 * Initializes the `lttng_kernel_enum_desc::sorted_entries` field for
 * each instance of `struct lttng_kernel_enum_desc` recursively found
 * in `desc`.
 *
 * See `struct lttng_kernel_enum_desc` to learn the role of
 * the `sorted_entries` field.
 */
void init_probe_desc_enum_desc_sorted_entries(
		const struct lttng_kernel_probe_desc *desc);

#endif /* _LTTNG_INIT_ENUM_DESC_SORTED_ENTRIES_H */
