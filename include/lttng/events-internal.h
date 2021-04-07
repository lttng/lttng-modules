/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng/events-internal.h
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_EVENTS_INTERNAL_H
#define _LTTNG_EVENTS_INTERNAL_H

#include <lttng/events.h>

static inline
const struct lttng_kernel_type_integer *lttng_kernel_get_type_integer(const struct lttng_kernel_type_common *type)
{
	if (type->type != lttng_kernel_type_integer)
		return NULL;
	return container_of(type, const struct lttng_kernel_type_integer, parent);
}

static inline
const struct lttng_kernel_type_string *lttng_kernel_get_type_string(const struct lttng_kernel_type_common *type)
{
	if (type->type != lttng_kernel_type_string)
		return NULL;
	return container_of(type, const struct lttng_kernel_type_string, parent);
}

static inline
const struct lttng_kernel_type_enum *lttng_kernel_get_type_enum(const struct lttng_kernel_type_common *type)
{
	if (type->type != lttng_kernel_type_enum)
		return NULL;
	return container_of(type, const struct lttng_kernel_type_enum, parent);
}

static inline
const struct lttng_kernel_type_array *lttng_kernel_get_type_array(const struct lttng_kernel_type_common *type)
{
	if (type->type != lttng_kernel_type_array)
		return NULL;
	return container_of(type, const struct lttng_kernel_type_array, parent);
}

static inline
const struct lttng_kernel_type_sequence *lttng_kernel_get_type_sequence(const struct lttng_kernel_type_common *type)
{
	if (type->type != lttng_kernel_type_sequence)
		return NULL;
	return container_of(type, const struct lttng_kernel_type_sequence, parent);
}

static inline
const struct lttng_kernel_type_struct *lttng_kernel_get_type_struct(const struct lttng_kernel_type_common *type)
{
	if (type->type != lttng_kernel_type_struct)
		return NULL;
	return container_of(type, const struct lttng_kernel_type_struct, parent);
}

static inline
const struct lttng_kernel_type_variant *lttng_kernel_get_type_variant(const struct lttng_kernel_type_common *type)
{
	if (type->type != lttng_kernel_type_variant)
		return NULL;
	return container_of(type, const struct lttng_kernel_type_variant, parent);
}

static inline bool lttng_kernel_type_is_bytewise_integer(const struct lttng_kernel_type_common *type)
{
	const struct lttng_kernel_type_integer *type_integer = lttng_kernel_get_type_integer(type);

	if (!type_integer)
		return false;
	switch (type_integer->size) {
	case 8:		/* Fall-through. */
	case 16:	/* Fall-through. */
	case 32:	/* Fall-through. */
	case 64:
		break;
	default:
		return false;
	}
	return true;
}

#endif /* _LTTNG_EVENTS_INTERNAL_H */
