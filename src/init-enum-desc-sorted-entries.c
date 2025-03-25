/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * init-enum-desc-sorted-entries.c
 *
 * Initialize the `sorted_entries` field of enumeration descriptors.
 *
 * Copyright (C) 2025 Philippe Proulx <pproulx@efficios.com>
 */

#include <linux/string.h>
#include <linux/sort.h>

#include <lttng/events.h>
#include <lttng/events-internal.h>

#include "init-enum-desc-sorted-entries.h"

static
void init_fields_enum_desc_sorted_entries(
		const struct lttng_kernel_event_field * const *fields,
		unsigned int nr_fields);

static
int comp_enum_entries(const void * const a, const void * const b)
{
	return strcmp(
		((const struct lttng_kernel_enum_entry *) a)->string,
		((const struct lttng_kernel_enum_entry *) b)->string);
}

/*
 * Initializes the `lttng_kernel_enum_desc::sorted_entries` field
 * of `type`.
 *
 * See `struct lttng_kernel_enum_desc` to learn the role of the
 * `sorted_entries` field.
 *
 * The steps are:
 *
 * 1. Copy the entry instances of `entries` to `sorted_entries`.
 *
 * 2. Traverse `sorted_entries` to set `start` and `end` if
 *    `options.is_auto` is set.
 *
 * 3. Sort `sorted_entries` by `string`.
 *
 * 4. Traverse `sorted_entries` to set `options.is_first_in_group`.
 */
static
void init_enum_type_desc_sorted_entries(
		const struct lttng_kernel_type_enum * const type)
{
	unsigned int i;
	const unsigned int nr_entries = type->desc->nr_entries;

	if (nr_entries > 0 && type->desc->sorted_entries[0].string)
		/* Already done */
		return;

	/* Step 1 */
	for (i = 0; i < nr_entries; i++) {
		type->desc->sorted_entries[i] = *type->desc->entries[i];
	}

	/* Step 2 */
	{
		unsigned long long last_end_val = 0;

		for (i = 0; i < nr_entries; i++) {
			struct lttng_kernel_enum_entry * const entry =
				&type->desc->sorted_entries[i];

			if (entry->options.is_auto) {
				last_end_val++;
				entry->start.value = last_end_val;
				entry->end.value = last_end_val;
				entry->options.is_auto = 0;
			}

			last_end_val = entry->end.value;
		}
	}

	/* Step 3 */
	sort(type->desc->sorted_entries, nr_entries,
		sizeof(struct lttng_kernel_enum_entry),
		comp_enum_entries, NULL);

	/* Step 4 */
	{
		const char *last_name = NULL;

		for (i = 0; i < nr_entries; i++) {
			struct lttng_kernel_enum_entry * const entry =
				&type->desc->sorted_entries[i];

			if (!last_name || strcmp(entry->string, last_name) != 0) {
				entry->options.is_first_in_group = 1;
			}

			last_name = entry->string;
		}
	}
}

void init_type_enum_desc_sorted_entries(
		const struct lttng_kernel_type_common * const type)
{
	switch (type->type) {
	case lttng_kernel_type_enum:
		init_enum_type_desc_sorted_entries(
			lttng_kernel_get_type_enum(type));
		break;
	case lttng_kernel_type_array:
		init_type_enum_desc_sorted_entries(
			lttng_kernel_get_type_array(type)->elem_type);
		break;
	case lttng_kernel_type_sequence:
		init_type_enum_desc_sorted_entries(
			lttng_kernel_get_type_sequence(type)->elem_type);
		break;
	case lttng_kernel_type_struct:
		init_fields_enum_desc_sorted_entries(
			lttng_kernel_get_type_struct(type)->fields,
			lttng_kernel_get_type_struct(type)->nr_fields);
		break;
	case lttng_kernel_type_variant:
		init_fields_enum_desc_sorted_entries(
			lttng_kernel_get_type_variant(type)->choices,
			lttng_kernel_get_type_variant(type)->nr_choices);
		break;
	default:
		/* We don't care about other types */
		break;
	}
}

/*
 * Initializes the `lttng_kernel_enum_desc::sorted_entries` field for
 * each instance of `struct lttng_kernel_enum_desc` recursively found
 * in `fields`.
 *
 * See `struct lttng_kernel_enum_desc` to learn the role of
 * the `sorted_entries` field.
 */
static
void init_fields_enum_desc_sorted_entries(
		const struct lttng_kernel_event_field * const * const fields,
		const unsigned int nr_fields)
{
	unsigned int i;

	for (i = 0; i < nr_fields; i++) {
		init_type_enum_desc_sorted_entries(fields[i]->type);
	}
}

/*
 * Initializes the `lttng_kernel_enum_desc::sorted_entries` field for
 * each instance of `struct lttng_kernel_enum_desc` recursively found
 * in `desc`.
 *
 * See `struct lttng_kernel_enum_desc` to learn the role of
 * the `sorted_entries` field.
 */
static
void init_event_desc_enum_desc_sorted_entries(
		const struct lttng_kernel_event_desc * const desc)
{
	init_fields_enum_desc_sorted_entries(desc->tp_class->fields,
		desc->tp_class->nr_fields);
}

void init_probe_desc_enum_desc_sorted_entries(
		const struct lttng_kernel_probe_desc * const desc)
{
	unsigned int i;

	for (i = 0; i < desc->nr_events; i++) {
		init_event_desc_enum_desc_sorted_entries(desc->event_desc[i]);
	}
}
