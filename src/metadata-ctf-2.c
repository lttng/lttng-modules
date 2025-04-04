/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * metadata-ctf-2.c
 *
 * CTF 2 metadata stream serialization.
 *
 * Copyright (C) 2025 Philippe Proulx <pproulx@efficios.com>
 */

#include <linux/types.h>
#include <linux/utsname.h>
#include <linux/dmi.h>
#include <linux/math64.h>

#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <lttng/lttng-kernel-mj-gen-internal.h>

#include "lttng-metadata-print.h"
#include "metadata-ctf-2.h"
#include "metadata-ctf-common.h"

#define CTF_2_BE	"big-endian"
#define CTF_2_LE	"little-endian"

#if __BYTE_ORDER == __BIG_ENDIAN
# define CTF_2_QUOTED_BYTE_ORDER	"\"" CTF_2_BE "\""
# define CTF_2_BYTE_ORDER		CTF_2_BE
# define CTF_2_BYTE_ORDER_REV		CTF_2_LE
#else
# define CTF_2_QUOTED_BYTE_ORDER	"\"" CTF_2_LE "\""
# define CTF_2_BYTE_ORDER		CTF_2_LE
# define CTF_2_BYTE_ORDER_REV		CTF_2_BE
#endif

/* Field class serialization context */
struct fc_ctx {
	/* Fields of the current structure type (can be `NULL`) */
	const struct lttng_kernel_event_field * const *struct_fields;

	/* Number of fields in `struct_fields` */
	unsigned int nr_fields;

	/* Previous field (can be `NULL`) */
	const struct lttng_kernel_event_field *prev_field;
};

static
int write_fc(struct lttng_kernel_session *session, const char *key,
		const struct fc_ctx *ctx,
		const struct lttng_kernel_type_common *type);

/*
 * Writes the common part of a new fragment of type `type`, leaving the
 * JSON object opened to add more properties.
 *
 * Close with close_fragment().
 */
static
int open_fragment(struct lttng_kernel_session * const session,
		const char * const type)
{
	int ret;

	/* Initial RS (see <https://datatracker.ietf.org/doc/html/rfc7464>) */
	ret = lttng_metadata_print(session, "\x1e");
	if (ret)
		return ret;

	/* Reinitialize the JSON metadata system for the new object */
	lttng_kernel_mj_gen_init(session);

	/* Open fragment object */
	ret = lttng_kernel_mj_gen_open_obj(session, NULL);
	if (ret)
		return ret;

	/* Write `type` property */
	return lttng_kernel_mj_gen_str(session, "type", type);
}

/*
 * Writes what's needed to close the current fragment.
 */
static
int close_fragment(struct lttng_kernel_session * const session)
{
	int ret;

	/* Close fragment object */
	ret = lttng_kernel_mj_gen_close_obj(session);
	if (ret)
		return ret;

	/* Write terminating newline (recommended by RFC 7464) */
	return lttng_metadata_print(session, "\n");
}

/*
 * Writes the common part of a field class object having the key `key`.
 *
 * This function doesn't close the JSON object so that you may add
 * specific properties.
 */
static
int open_fc(struct lttng_kernel_session * const session,
		const char * const key, const char * const type)
{
	int ret;

	ret = lttng_kernel_mj_gen_open_obj(session, key);
	if (ret)
		return ret;

	/* Write `type` property */
	return lttng_kernel_mj_gen_str(session, "type", type);
}

/*
 * Writes the common part of a fixed-length bit array field class object
 * having the key `key`.
 *
 * This function doesn't close the JSON object so that you may add
 * specific properties if needed.
 */
static
int open_bit_array_fc(struct lttng_kernel_session * const session,
		const char * const key, const char * const type,
		const unsigned int len, const bool reverse_byte_order,
		const unsigned int align)
{
	int ret;

	ret = open_fc(session, key, type);
	if (ret)
		return ret;

	/* Write `length` property */
	ret = lttng_kernel_mj_gen_uint(session, "length", len);
	if (ret)
		return ret;

	/* Write `byte-order` property */
	ret = lttng_kernel_mj_gen_str(session, "byte-order",
		reverse_byte_order ? CTF_2_BYTE_ORDER_REV : CTF_2_BYTE_ORDER);
	if (ret)
		return ret;

	/* Write `alignment` property if needed */
	if (align > 1) {
		ret = lttng_kernel_mj_gen_uint(session, "alignment", align);
		if (ret)
			return ret;
	}

	return 0;
}

/*
 * Writes the common part of a fixed-length integer field class object
 * having the key `key` for `type`.
 *
 * This function doesn't close the JSON object so that you may add
 * specific properties if needed.
 */
static
int open_int_fc(struct lttng_kernel_session * const session,
		const struct lttng_kernel_type_integer * const type,
		const char * const key)
{
	int ret;

	ret = open_bit_array_fc(session, key, type->signedness ?
		"fixed-length-signed-integer" : "fixed-length-unsigned-integer",
		type->size, type->reverse_byte_order, type->alignment);
	if (ret)
		return ret;

	/* Write `preferred-display-base` property if needed */
	if (type->base != 10) {
		ret = lttng_kernel_mj_gen_uint(session,
			"preferred-display-base", type->base);
		if (ret)
			return ret;
	}

	return 0;
}

/*
 * Writes the fixed-length integer field class object having the key
 * `key` for `type`.
 */
static
int write_int_fc_from_int_type(struct lttng_kernel_session * const session,
		const char * const key,
		const struct lttng_kernel_type_integer * const type)
{
	int ret;

	ret = open_int_fc(session, type, key);
	if (ret)
		return ret;

	return lttng_kernel_mj_gen_close_obj(session);
}

/*
 * Writes an integer range value for `enum_val`.
 */
static
int write_int_range_val(struct lttng_kernel_session * const session,
		const struct lttng_kernel_enum_value * const enum_val)
{
	if (enum_val->signedness) {
		return lttng_kernel_mj_gen_sint(session, NULL,
			(int64_t) enum_val->value);
	} else {
		return lttng_kernel_mj_gen_uint(session, NULL,
			enum_val->value);
	}
}

/*
 * Writes an array of integer ranges having the key `key` from the
 * entries found in `type`, from the index `begin_index` to `end_index`
 * (excluded).
 */
static
int write_int_ranges(struct lttng_kernel_session * const session,
		const struct lttng_kernel_type_enum * const type,
		const char * const key,
		const unsigned int begin_index, const unsigned int end_index)
{
	int ret;
	unsigned int i;

	/* Open array of ranges */
	ret = lttng_kernel_mj_gen_open_array(session, key);
	if (ret)
		return ret;

	for (i = begin_index; i < end_index; i++) {
		const struct lttng_kernel_enum_entry * const entry =
			&type->desc->sorted_entries[i];

		/* Append range pair to current array of ranges */
		{
			/* Open pair */
			ret = lttng_kernel_mj_gen_open_array(session, NULL);
			if (ret)
				return ret;

			/* Beginning of range */
			ret = write_int_range_val(session, &entry->start);
			if (ret)
				return ret;

			/* End of range */
			ret = write_int_range_val(session, &entry->end);
			if (ret)
				return ret;

			/* Close pair */
			ret = lttng_kernel_mj_gen_close_array(session);
			if (ret)
				return ret;
		}
	}

	/* Close array of ranges */
	return lttng_kernel_mj_gen_close_array(session);
}

/*
 * Writes the mappings of an integer field class for `type`.
 */
static
int write_int_fc_mappings(struct lttng_kernel_session * const session,
		const struct lttng_kernel_type_enum * const type)
{
	int ret;
	unsigned int begin_index, end_index, i;

	ret = lttng_kernel_mj_gen_open_obj(session, "mappings");
	if (ret)
		return ret;

	while (i < type->desc->nr_entries) {
		const struct lttng_kernel_enum_entry *entry =
			&type->desc->sorted_entries[i];

		if (!entry->options.is_first_in_group) {
			i++;
			continue;
		}

		/* Found the first entry of the group */
		begin_index = i;
		end_index = type->desc->nr_entries;
		i++;

		/* Find the last entry of the group */
		while (i < type->desc->nr_entries) {
			entry = &type->desc->sorted_entries[i];

			if (entry->options.is_first_in_group) {
				/* Found the next name */
				end_index = i;
				break;
			}

			i++;
		}

		/* Write the mapping for this name */
		ret = write_int_ranges(session, type,
			type->desc->sorted_entries[begin_index].string,
			begin_index, end_index);
		if (ret)
			return ret;
	}

	return lttng_kernel_mj_gen_close_obj(session);
}

/*
 * Writes the fixed-length integer field class object having the key
 * `key` for `type`.
 */
static
int write_int_fc_from_enum_type(struct lttng_kernel_session * const session,
		const char * const key,
		const struct lttng_kernel_type_enum * const type)
{
	int ret;

	ret = open_int_fc(session,
		lttng_kernel_get_type_integer(type->container_type), key);
	if (ret)
		return ret;

	ret = write_int_fc_mappings(session, type);
	if (ret)
		return ret;

	return lttng_kernel_mj_gen_close_obj(session);
}

/*
 * Writes the null-terminated string field class object having the
 * key `key`.
 */
static
int write_null_terminated_str_fc(struct lttng_kernel_session * const session,
		const char * const key)
{
	int ret;

	ret = open_fc(session, key, "null-terminated-string");
	if (ret)
		return ret;

	return lttng_kernel_mj_gen_close_obj(session);
}

/*
 * Writes the common part of an array field class object having the
 * key `key`.
 *
 * This function doesn't close the JSON object so that you may add
 * specific properties.
 */
static
int open_array_fc(struct lttng_kernel_session * const session,
		const char * const key, const struct fc_ctx * const ctx,
		const struct lttng_kernel_type_common * const elem_type,
		const unsigned int min_align,
		const char * const type_str)
{
	int ret;

	ret = open_fc(session, key, type_str);
	if (ret)
		return ret;

	/* Write `minimum-alignment` property if needed */
	if (min_align > 1) {
		ret = lttng_kernel_mj_gen_uint(session,
			"minimum-alignment", min_align);
		if (ret)
			return ret;
	}

	/* Write `element-field-class` property if needed */
	if (elem_type) {
		ret = write_fc(session, "element-field-class", ctx, elem_type);
		if (ret)
			return ret;
	}

	return 0;
}

/*
 * Writes the static-length array field class object having the
 * key `key` for `type`.
 */
static
int write_static_len_array_fc(struct lttng_kernel_session * const session,
		const char * const key, const struct fc_ctx * const ctx,
		const struct lttng_kernel_type_array * const type)
{
	int ret;

	ret = open_array_fc(session, key, ctx, type->elem_type,
		type->alignment, "static-length-array");
	if (ret)
		return ret;

	/* Write `length` property */
	ret = lttng_kernel_mj_gen_uint(session, "length", type->length);
	if (ret)
		return ret;

	return lttng_kernel_mj_gen_close_obj(session);
}

/*
 * Returns the field named `name` within `ctx->struct_fields`, or `NULL`
 * if not found.
 */
static
const struct lttng_kernel_event_field *find_field(
	const struct fc_ctx * const ctx, const char *name)
{
	unsigned int i;

	if (!name) {
		if (!ctx->prev_field) {
			WARN_ONCE(1, "LTTng: Expecting a previous field, but none found\n");
			return NULL;
		}

		name = ctx->prev_field->name;
	}

	for (i = 0; i < ctx->nr_fields; i++) {
		if (strcmp(ctx->struct_fields[i]->name, name) == 0) {
			return ctx->struct_fields[i];
		}
	}

	return NULL;
}

/*
 * Writes a field location object having the key `key.
 *
 * The field location path is always relative and the target is within
 * the same immediate structure fields.
 *
 * If `name` is `NULL`, then use the name of `ctx->prev_field`.
 *
 * This function validates that a field having the target name exists
 * within `ctx->struct_fields`, returning -EINVAL otherwise.
 */
static
int write_field_loc(struct lttng_kernel_session * const session,
		const char * const key, const struct fc_ctx * const ctx,
		const char *name)
{
	int ret;

	if (!name) {
		if (!ctx->prev_field) {
			WARN_ONCE(1, "LTTng: Expecting a previous field, but none found\n");
			return -EINVAL;
		}

		name = ctx->prev_field->name;
	}

	if (!find_field(ctx, name)) {
		WARN_ONCE(1, "LTTng: Cannot find target field named `%s`\n",
			name);
		return -EINVAL;
	}

	ret = lttng_kernel_mj_gen_open_obj(session, key);
	if (ret)
		return ret;

	/* Write `path` property */
	{
		ret = lttng_kernel_mj_gen_open_array(session, "path");
		if (ret)
			return ret;

		ret = lttng_kernel_mj_gen_str(session, NULL, name);
		if (ret)
			return ret;

		ret = lttng_kernel_mj_gen_close_array(session);
		if (ret)
			return ret;
	}

	return lttng_kernel_mj_gen_close_obj(session);
}

/*
 * Writes the dynamic-length array field class object having the
 * key `key` for `type`.
 */
static
int write_dyn_len_array_fc(struct lttng_kernel_session * const session,
		const char * const key, const struct fc_ctx * const ctx,
		const struct lttng_kernel_type_sequence * const type)
{
	int ret;

	ret = open_array_fc(session, key, ctx, type->elem_type,
		type->alignment, "dynamic-length-array");
	if (ret)
		return ret;

	/* Write `length-field-location` property */
	ret = write_field_loc(session, "length-field-location", ctx,
		type->length_name);
	if (ret)
		return ret;

	return lttng_kernel_mj_gen_close_obj(session);
}

/*
 * Writes the static-length string field class object having the
 * key `key` for `type`.
 */
static
int write_static_len_str_fc(struct lttng_kernel_session * const session,
		const char * const key,
		const struct lttng_kernel_type_array * const type)
{
	int ret;

	ret = open_fc(session, key, "static-length-string");
	if (ret)
		return ret;

	/* Write `length` property */
	ret = lttng_kernel_mj_gen_uint(session, "length", type->length);
	if (ret)
		return ret;

	return lttng_kernel_mj_gen_close_obj(session);
}

/*
 * Writes the dynamic-length string field class object having the
 * key `key` for `type`.
 */
static
int write_dyn_len_str_fc(struct lttng_kernel_session * const session,
		const char * const key, const struct fc_ctx * const ctx,
		const struct lttng_kernel_type_sequence * const type)
{
	int ret;

	ret = open_fc(session, key, "dynamic-length-string");
	if (ret)
		return ret;

	/* Write `length-field-location` property */
	ret = write_field_loc(session, "length-field-location", ctx,
		type->length_name);
	if (ret)
		return ret;

	return lttng_kernel_mj_gen_close_obj(session);
}

/*
 * Writes the structure field class object having the
 * key `key` for `type`.
 */
static
int write_struct_fc(struct lttng_kernel_session * const session,
		const char * const key, const struct fc_ctx * const ctx,
		const struct lttng_kernel_type_struct * const type)
{
	int ret;

	ret = open_fc(session, key, "structure");
	if (ret)
		return ret;

	/* Write `minimum-alignment` property if needed */
	if (type->alignment > 1) {
		ret = lttng_kernel_mj_gen_uint(session,
			"minimum-alignment", type->alignment);
		if (ret)
			return ret;
	}

	/* Write `member-classes` property */
	{
		unsigned int i;

		/* New context */
		struct fc_ctx new_fc_ctx = {
			.struct_fields = type->fields,
			.nr_fields = type->nr_fields,
			.prev_field = NULL,
		};

		/* Open array of member classes */
		ret = lttng_kernel_mj_gen_open_array(session, "member-classes");
		if (ret)
			return ret;

		for (i = 0; i < type->nr_fields; i++) {
			const struct lttng_kernel_event_field * const field =
				type->fields[i];

			/* Open member class object */
			ret = lttng_kernel_mj_gen_open_obj(session, NULL);
			if (ret)
				return ret;

			/* Write `name` property */
			ret = lttng_kernel_mj_gen_str(session, "name",
				field->name);
			if (ret)
				return ret;

			/* Write `field-class` property */
			ret = write_fc(session, "field-class", &new_fc_ctx,
				field->type);
			if (ret)
				return ret;

			/* Close member class object */
			ret = lttng_kernel_mj_gen_close_obj(session);
			if (ret)
				return ret;

			/* Set previous field of context */
			new_fc_ctx.prev_field = field;
		}

		/* Close array of member classes */
		ret = lttng_kernel_mj_gen_close_array(session);
		if (ret)
			return ret;
	}

	return lttng_kernel_mj_gen_close_obj(session);
}

/*
 * Sets `*begin_index` and `*end_index` to the indexes of the
 * beginning and past-the-end enumeration entries having the name
 * `name` within `type`.
 *
 * For example, looking for `hope`:
 *
 * ╔═══════╦═══════════╦══════════╦═════════════════════════╗
 * ║ Index ║   Name    ║  Range   ║       Note              ║
 * ╠═══════╬═══════════╬══════════╬═════════════════════════╣
 * ║     0 ║ ignite    ║ [ 5,  9] ║                         ║
 * ║     1 ║ ignite    ║ [15, 15] ║                         ║
 * ║     2 ║ shake     ║ [ 2,  4] ║                         ║
 * ║     3 ║ hope      ║ [ 6, 19] ║ `*begin_index` set to 3 ║
 * ║     4 ║ hope      ║ [21, 21] ║                         ║
 * ║     5 ║ hope      ║ [35, 45] ║                         ║
 * ║     6 ║ butterfly ║ [12, 12] ║ `*end_index` set to 6   ║
 * ╚═══════╩═══════════╩══════════╩═════════════════════════╝
 *
 * Returns -EINVAL if not found.
 */
static
int enum_entry_indexes_for_name(
		const struct lttng_kernel_type_enum * const type,
		const char * const name,
		unsigned int * const begin_index,
		unsigned int * const end_index)
{
	unsigned int i = 0;
	int ret = -EINVAL;

	while (i < type->desc->nr_entries) {
		const struct lttng_kernel_enum_entry *entry =
			&type->desc->sorted_entries[i];

		if (!entry->options.is_first_in_group ||
				strcmp(entry->string, name) != 0) {
			i++;
			continue;
		}

		/* Found the name */
		ret = 0;
		*end_index = type->desc->nr_entries;
		*begin_index = i;
		i++;

		while (i < type->desc->nr_entries) {
			entry = &type->desc->sorted_entries[i];

			if (entry->options.is_first_in_group) {
				/* Found the next name */
				*end_index = i;
				return 0;
			}

			i++;
		}
	}

	return ret;
}

/*
 * Writes the selector field ranges (array of ranges) of the options
 * named `opt_name` within `type`.
 */
static
int write_sel_field_ranges(struct lttng_kernel_session * const session,
		const struct fc_ctx * const ctx,
		const struct lttng_kernel_type_variant * const type,
		const struct lttng_kernel_event_field * const tag_field,
		const char * const opt_name)
{
	const struct lttng_kernel_type_enum *tag_type;
	unsigned int begin_index, end;
	int ret;

	tag_type = lttng_kernel_get_type_enum(tag_field->type);

	/* Get the range of indexes for the entries named `opt_name` */
	ret = enum_entry_indexes_for_name(tag_type, opt_name,
		&begin_index, &end);
	if (ret)
		return ret;

	/* Write the array of ranges */
	return write_int_ranges(session, tag_type,
		"selector-field-ranges", begin_index, end);
}

/*
 * Writes the variant field class object having the
 * key `key` for `type`.
 */
static
int write_var_fc(struct lttng_kernel_session * const session,
		const char * const key, const struct fc_ctx * const ctx,
		const struct lttng_kernel_type_variant * const type)
{
	int ret;
	const struct lttng_kernel_event_field *tag_field;

	if (type->alignment != 0) {
		WARN_ONCE(1, "LTTng: Unexpected variant type alignment %u\n",
			type->alignment);
		return -EINVAL;
	}

	ret = open_fc(session, key, "variant");
	if (ret)
		return ret;

	/* Write `selector-field-location` property */
	ret = write_field_loc(session, "selector-field-location", ctx,
		type->tag_name);
	if (ret)
		return ret;

	/* Find tag field within current structure type */
	tag_field = find_field(ctx, type->tag_name);
	if (!tag_field) {
		WARN_ONCE(1, "LTTng: Cannot find tag field named `%s`\n",
			type->tag_name);
		return -EINVAL;
	}

	/* Write `options` property */
	{
		unsigned int i;

		/* Open array of options */
		ret = lttng_kernel_mj_gen_open_array(session, "options");
		if (ret)
			return ret;

		for (i = 0; i < type->nr_choices; i++) {
			const struct lttng_kernel_event_field * const field =
				type->choices[i];

			/* Open option object */
			ret = lttng_kernel_mj_gen_open_obj(session, NULL);
			if (ret)
				return ret;

			/* Write `name` property */
			ret = lttng_kernel_mj_gen_str(session, "name",
				field->name);
			if (ret)
				return ret;

			/* Write `selector-field-ranges` property */
			ret = write_sel_field_ranges(session, ctx,
				type, tag_field, field->name);
			if (ret)
				return ret;

			/* Write `field-class` property */
			ret = write_fc(session, "field-class", ctx,
				field->type);
			if (ret)
				return ret;

			/* Close option object */
			ret = lttng_kernel_mj_gen_close_obj(session);
			if (ret)
				return ret;
		}

		/* Close array of options */
		ret = lttng_kernel_mj_gen_close_array(session);
		if (ret)
			return ret;
	}

	return lttng_kernel_mj_gen_close_obj(session);
}

/*
 * Writes the field class object having the key `key` for `type`.
 */
static
int write_fc(struct lttng_kernel_session * const session,
		const char * const key, const struct fc_ctx * const ctx,
		const struct lttng_kernel_type_common * const type)
{
	switch (type->type) {
	case lttng_kernel_type_integer:
		return write_int_fc_from_int_type(session, key,
			lttng_kernel_get_type_integer(type));
	case lttng_kernel_type_enum:
		return write_int_fc_from_enum_type(session, key,
			lttng_kernel_get_type_enum(type));
	case lttng_kernel_type_string:
		return write_null_terminated_str_fc(session, key);
	case lttng_kernel_type_array:
	{
		const struct lttng_kernel_type_array * const array_type =
			lttng_kernel_get_type_array(type);

		if (array_type->encoding == lttng_kernel_string_encoding_none)
			return write_static_len_array_fc(session, key,
				ctx, array_type);
		else
			return write_static_len_str_fc(session, key,
				array_type);
	}
	case lttng_kernel_type_sequence:
	{
		const struct lttng_kernel_type_sequence * const seq_type =
			lttng_kernel_get_type_sequence(type);

		if (seq_type->encoding == lttng_kernel_string_encoding_none)
			return write_dyn_len_array_fc(session, key,
				ctx, seq_type);
		else
			return write_dyn_len_str_fc(session, key,
				ctx, seq_type);
	}
	case lttng_kernel_type_struct:
		return write_struct_fc(session, key, ctx,
			lttng_kernel_get_type_struct(type));
	case lttng_kernel_type_variant:
		return write_var_fc(session, key, ctx,
			lttng_kernel_get_type_variant(type));
	default:
		WARN_ONCE(1, "LTTng: Unknown type ID %d\n", type->type);
		return -EINVAL;
	}
}

/*
 * Writes the scope field class object having the key `key` for
 * the fields `fields`.
 */
static
int write_scope_fc(struct lttng_kernel_session * const session,
		const char * const key,
		const struct lttng_kernel_event_field * const * const fields,
		const unsigned int nr_fields)
{
	/* Create anonymous structure type containing the fields */
	const struct lttng_kernel_type_struct struct_type = {
		.parent = {
			.type = lttng_kernel_type_struct,
		},
		.nr_fields = nr_fields,
		.fields = fields,
		.alignment = 0,
	};

	/* Initial context: this is the root, therefore no structure fields */
	struct fc_ctx ctx = {
		.struct_fields = NULL,
		.nr_fields = 0,
		.prev_field = NULL,
	};

	/* Write structure field class from anonymous structure type */
	return write_struct_fc(session, key, &ctx, &struct_type);
}

/*
 * Writes an event record class fragment for `event`.
 */
static
int write_event_record_cls_fragment(struct lttng_kernel_session * const session,
		const struct lttng_kernel_event_recorder * const event_recorder)
{
	int ret;

	ret = open_fragment(session, "event-record-class");
	if (ret)
		return ret;

	/* Write `data-stream-class-id` property */
	ret = lttng_kernel_mj_gen_uint(session, "data-stream-class-id",
		event_recorder->chan->priv->id);
	if (ret)
		return ret;

	/* Write `id` property */
	ret = lttng_kernel_mj_gen_uint(session, "id",
		event_recorder->priv->parent.id);
	if (ret)
		return ret;

	/* Write `name` property */
	ret = lttng_kernel_mj_gen_str(session, "name",
		event_recorder->priv->parent.parent.desc->event_name);
	if (ret)
		return ret;

	/* Write `payload-field-class` property */
	{
		const struct lttng_kernel_event_desc * const desc =
			event_recorder->priv->parent.parent.desc;

		ret = write_scope_fc(session, "payload-field-class",
			desc->tp_class->fields, desc->tp_class->nr_fields);
		if (ret)
			return ret;
	}

	return close_fragment(session);
}

int lttng_event_recorder_metadata_statedump_ctf_2(
		struct lttng_kernel_session * const session,
		struct lttng_kernel_event_recorder * const event_recorder)
{
	return write_event_record_cls_fragment(session, event_recorder);
}

/*
 * Writes a clock offset object having the key `key` based on the
 * current trace clock.
 */
static
int write_clk_offset(struct lttng_kernel_session * const session,
		const char * const key)
{
	int ret;

	ret = lttng_kernel_mj_gen_open_obj(session, key);
	if (ret)
		return ret;

	/*
	 * Write the `seconds` and `cycles` properties, adjusting what
	 * measure_clock_offset() returns depending on the clock
	 * frequency.
	 *
	 * CTF 2 has a requirement that the value of the `cycles`
	 * property is positive and less than the clock frequency.
	 *
	 * For example, with a clock frequency of 1000 Hz:
	 *
	 * • 4758 cycles is equivalent to 4 seconds + 758 cycles.
	 * • -7385 cycles is equivalent to -8 seconds + 615 cycles.
	 */
	{
		const u64 freq = trace_clock_freq();
		const s64 total_cycles = measure_clock_offset();
		s64 offset_secs = div64_s64(total_cycles, (s64) freq);
		s64 offset_cycles = total_cycles - offset_secs * freq;

		if (offset_cycles < 0) {
			offset_secs -= 1;
			offset_cycles += freq;
		}

		/* Write resulting `seconds` property */
		ret = lttng_kernel_mj_gen_sint(session, "seconds", offset_secs);
		if (ret)
			return ret;

		/* Write resulting `cycles` property */
		ret = lttng_kernel_mj_gen_uint(session, "cycles",
			(unsigned long long) offset_cycles);
		if (ret)
			return ret;
	}

	return lttng_kernel_mj_gen_close_obj(session);
}

/*
 * Writes a clock class fragment for the current trace clock.
 */
static
int write_clk_cls_fragment(struct lttng_kernel_session * const session)
{
	unsigned char clock_uuid_s[BOOT_ID_LEN];
	int ret;

	/* Open fragment */
	ret = open_fragment(session, "clock-class");
	if (ret)
		return ret;

	/*
	 * Write `id` property.
	 *
	 * This is a unique ID within the metadata stream to set
	 * the default clock class of a data stream class.
	 *
	 * Using the name is fine as there's only one clock class
	 * anyway.
	 */
	ret = lttng_kernel_mj_gen_str(session, "id", trace_clock_name());
	if (ret)
		return ret;

	/* Write `name` property */
	ret = lttng_kernel_mj_gen_str(session, "name", trace_clock_name());
	if (ret)
		return ret;

	/* Write `uid` property if the UUID is available */
	if (!trace_clock_uuid(clock_uuid_s)) {
		ret = lttng_kernel_mj_gen_str(session, "uid", clock_uuid_s);
		if (ret)
			return ret;
	}

	/* Write `description` property */
	ret = lttng_kernel_mj_gen_str(session, "description",
		trace_clock_description());
	if (ret)
		return ret;

	/* Write `frequency` property */
	ret = lttng_kernel_mj_gen_uint(session, "frequency",
		(unsigned long long) trace_clock_freq());
	if (ret)
		return ret;

	/* Write `origin` property: always Unix epoch */
	ret = lttng_kernel_mj_gen_str(session, "origin", "unix-epoch");
	if (ret)
		return ret;

	/* Write `offset-from-origin` property */
	ret = write_clk_offset(session, "offset-from-origin");
	if (ret)
		return ret;

	return close_fragment(session);
}

/*
 * Writes the `event-record-common-context-field-class` of a data stream
 * class fragment for `chan`.
 */
static
int write_event_record_common_ctx_fc(struct lttng_kernel_session * const session,
		const struct lttng_kernel_ctx * const lttng_ctx)
{
	int ret;
	unsigned int i;
	const struct lttng_kernel_event_field **fields;

	/*
	 * Allocate an array of `struct lttng_kernel_event_field`
	 * pointers because this is what write_scope_fc(), whereas
	 * `lttng_ctx->fields` is an array of
	 * `struct lttng_kernel_ctx_field` instances.
	 */
	fields = kmalloc(sizeof(void *) * lttng_ctx->nr_fields, GFP_KERNEL);
	if (!fields)
		return -ENOMEM;

	/* Fill `fields` with corresponding kernel event fields */
	for (i = 0; i < lttng_ctx->nr_fields; i++) {
		fields[i] = lttng_ctx->fields[i].event_field;
	}

	/* Write the scope field class and return */
	ret = write_scope_fc(session, "event-record-common-context-field-class",
		fields, lttng_ctx->nr_fields);
	kfree(fields);
	return ret;
}

/*
 * Writes a data stream class fragment for `chan`.
 */
static
int write_data_stream_cls_fragment(struct lttng_kernel_session * const session,
		const struct lttng_kernel_channel_buffer * const chan)
{
	int ret;

	ret = open_fragment(session, "data-stream-class");
	if (ret)
		return ret;

	/* Write `id` property */
	ret = lttng_kernel_mj_gen_uint(session, "id", chan->priv->id);
	if (ret)
		return ret;

	/* Write `default-clock-class-id` property */
	ret = lttng_kernel_mj_gen_str(session, "default-clock-class-id",
		trace_clock_name());
	if (ret)
		return ret;

	/* Write `packet-context-field-class` property */
	ret = lttng_kernel_mj_gen_str(session, "packet-context-field-class",
		"pkt-ctx");
	if (ret)
		return ret;

	/* Write `event-record-header-field-class` property */
	ret = lttng_kernel_mj_gen_str(session,
		"event-record-header-field-class",
		chan->priv->header_type == 1 ?
			"er-header-compact" : "er-header-large");
	if (ret)
		return ret;

	/* Write `event-record-common-context-field-class` property */
	if (chan->priv->ctx && chan->priv->ctx->nr_fields > 0) {
		ret = write_event_record_common_ctx_fc(session,
			chan->priv->ctx);
		if (ret)
			return ret;
	}

	return close_fragment(session);
}

int lttng_channel_metadata_statedump_ctf_2(
		struct lttng_kernel_session * const session,
		struct lttng_kernel_channel_buffer * const chan)
{
	return write_data_stream_cls_fragment(session, chan);
}

/*
 * Writes a trace environment object having the key `key` based on the
 * properties of the running kernel.
 */
static
int write_trace_env(struct lttng_kernel_session * const session,
		const char * const key)
{
	int ret;

	ret = lttng_kernel_mj_gen_open_obj(session, key);
	if (ret)
		return ret;

	ret = lttng_kernel_mj_gen_str(session, "hostname",
		current->nsproxy->uts_ns->name.nodename);
	if (ret)
		return ret;

	ret = lttng_kernel_mj_gen_str(session, "domain", "kernel");
	if (ret)
		return ret;

	ret = lttng_kernel_mj_gen_str(session, "sysname", utsname()->sysname);
	if (ret)
		return ret;

	ret = lttng_kernel_mj_gen_str(session, "kernel_release",
		utsname()->release);
	if (ret)
		return ret;

	ret = lttng_kernel_mj_gen_str(session, "kernel_version",
		utsname()->version);
	if (ret)
		return ret;

	ret = lttng_kernel_mj_gen_str(session, "tracer_name", "lttng-modules");
	if (ret)
		return ret;

	ret = lttng_kernel_mj_gen_sint(session, "tracer_major",
		LTTNG_MODULES_MAJOR_VERSION);
	if (ret)
		return ret;

	ret = lttng_kernel_mj_gen_sint(session, "tracer_minor",
		LTTNG_MODULES_MINOR_VERSION);
	if (ret)
		return ret;

	ret = lttng_kernel_mj_gen_sint(session, "tracer_patchlevel",
		LTTNG_MODULES_PATCHLEVEL_VERSION);
	if (ret)
		return ret;

	ret = lttng_kernel_mj_gen_str(session, "trace_buffering_scheme",
		"global");
	if (ret)
		return ret;

	ret = lttng_kernel_mj_gen_str(session, "trace_name",
		session->priv->name);
	if (ret)
		return ret;

	ret = lttng_kernel_mj_gen_str(session, "trace_creation_datetime",
		session->priv->creation_time);
	if (ret)
		return ret;

	{
		const char * const product_uuid =
			dmi_get_system_info(DMI_PRODUCT_UUID);

		if (product_uuid) {
			ret = lttng_kernel_mj_gen_str(session, "product_uuid",
				product_uuid);
			if (ret)
				return ret;
		}
	}

	return lttng_kernel_mj_gen_close_obj(session);
}

/*
 * Writes a trace class fragment for `session`.
 */
static
int write_trace_cls_fragment(struct lttng_kernel_session * const session)
{
	int ret;

	ret = open_fragment(session, "trace-class");
	if (ret)
		return ret;

	/* Write `namespace` property */
	ret = lttng_kernel_mj_gen_str(session, "namespace", "lttng.org,2009");
	if (ret)
		return ret;

	/* Write `name` property */
	ret = lttng_kernel_mj_gen_str(session, "name", "");
	if (ret)
		return ret;

	/* Write `uid` property */
	{
		const unsigned char * const uuid = session->priv->uuid.b;

		ret = lttng_kernel_mj_gen_raw(session, "uid",
			"\"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\"",
			uuid[0], uuid[1], uuid[2], uuid[3],
			uuid[4], uuid[5], uuid[6], uuid[7],
			uuid[8], uuid[9], uuid[10], uuid[11],
			uuid[12], uuid[13], uuid[14], uuid[15]);
		if (ret)
			return ret;
	}

	/* Write `environment` property */
	ret = write_trace_env(session, "environment");
	if (ret)
		return ret;

	/* Write `packet-header-field-class` property */
	ret = lttng_kernel_mj_gen_str(session, "packet-header-field-class",
		"pkt-header");
	if (ret)
		return ret;

	return close_fragment(session);
}

/*
 * Writes a field class alias fragment, assigning the alias `name` to
 * the field class of which the JSON is `...` formatted with `fc_fmt`.
 */
static
int write_fc_alias_fragment(struct lttng_kernel_session * const session,
		const char * const name, const char * const fc_fmt, ...)
{
	int ret;
	va_list ap;

	ret = open_fragment(session, "field-class-alias");
	if (ret)
		return ret;

	/* Write `name` property */
	ret = lttng_kernel_mj_gen_str(session, "name", name);
	if (ret)
		return ret;

	/* Write `field-class` property */
	va_start(ap, fc_fmt);
	ret = lttng_kernel_mj_gen_raw_va(session, "field-class", fc_fmt, ap);
	va_end(ap);
	if (ret)
		return ret;

	return close_fragment(session);
}

/*
 * Writes all the field class aliases.
 */
static
int write_fc_alias_fragments(struct lttng_kernel_session * const session)
{
	int ret;

	/* 32-bit unsigned integer FC with "event record class ID" role */
	ret = write_fc_alias_fragment(session, "u32-erc-id",
		"{"
		  "\"type\": \"fixed-length-unsigned-integer\","
		  "\"length\": 32,"
		  "\"alignment\": %u,"
		  "\"byte-order\": " CTF_2_QUOTED_BYTE_ORDER ","
		  "\"roles\": [\"event-record-class-id\"]"
		"}",
		lttng_alignof(uint32_t) * CHAR_BIT);
	if (ret)
		return ret;

	/* 64-bit unsigned integer FC with "default clock timestamp" role */
	ret = write_fc_alias_fragment(session, "u64-ts",
		"{"
		  "\"type\": \"fixed-length-unsigned-integer\","
		  "\"length\": 64,"
		  "\"alignment\": %u,"
		  "\"byte-order\": " CTF_2_QUOTED_BYTE_ORDER ","
		  "\"roles\": [\"default-clock-timestamp\"]"
		"}",
		lttng_alignof(uint64_t) * CHAR_BIT);
	if (ret)
		return ret;

	/* Compact event record header FC */
	ret = write_fc_alias_fragment(session, "er-header-compact",
		"{"
		  "\"type\": \"structure\","
		  "\"minimum-alignment\": %u,"
		  "\"member-classes\": ["
		    "{"
		      "\"name\": \"id\","
		      "\"field-class\": {"
		        "\"type\": \"fixed-length-unsigned-integer\","
		        "\"length\": 5,"
		        "\"byte-order\": " CTF_2_QUOTED_BYTE_ORDER ","
		        "\"mappings\": {"
		          "\"compact\": [[0, 30]],"
		          "\"extended\": [[31, 31]]"
		        "},"
		        "\"roles\": [\"event-record-class-id\"]"
		      "}"
		    "},"
		    "{"
		      "\"name\": \"v\","
		      "\"field-class\": {"
		        "\"type\": \"variant\","
		        "\"selector-field-location\": {"
		          "\"path\": [\"id\"]"
		        "},"
		        "\"options\": ["
		          "{"
		            "\"name\": \"compact\","
		            "\"selector-field-ranges\": [[0, 30]],"
		            "\"field-class\": {"
		              "\"type\": \"structure\","
		              "\"member-classes\": ["
		                "{"
		                  "\"name\": \"timestamp\","
		                  "\"field-class\": {"
		                    "\"type\": \"fixed-length-unsigned-integer\","
		                    "\"length\": 27,"
		                    "\"byte-order\": " CTF_2_QUOTED_BYTE_ORDER ","
		                    "\"roles\": [\"default-clock-timestamp\"]"
		                  "}"
		                "}"
		              "]"
		            "}"
		          "},"
		          "{"
		            "\"name\": \"extended\","
		            "\"selector-field-ranges\": [[31, 31]],"
		            "\"field-class\": {"
		              "\"type\": \"structure\","
		              "\"member-classes\": ["
		                "{"
		                  "\"name\": \"id\","
		                  "\"field-class\": \"u32-erc-id\""
		                "},"
		                "{"
		                  "\"name\": \"timestamp\","
		                  "\"field-class\": \"u64-ts\""
		                "}"
		              "]"
		            "}"
		          "}"
		        "]"
		      "}"
		    "}"
		  "]"
		"}",
		lttng_alignof(uint32_t) * CHAR_BIT);
	if (ret)
		return ret;

	/* Large event record header FC */
	ret = write_fc_alias_fragment(session, "er-header-large",
		"{"
		  "\"type\": \"structure\","
		  "\"minimum-alignment\": %u,"
		  "\"member-classes\": ["
		    "{"
		      "\"name\": \"id\","
		      "\"field-class\": {"
		        "\"type\": \"fixed-length-unsigned-integer\","
		        "\"length\": 16,"
		        "\"alignment\": %u,"
		        "\"byte-order\": " CTF_2_QUOTED_BYTE_ORDER ","
		        "\"mappings\": {"
		          "\"compact\": [[0, 65534]],"
		          "\"extended\": [[65535, 65535]]"
		        "},"
		        "\"roles\": [\"event-record-class-id\"]"
		      "}"
		    "},"
		    "{"
		      "\"name\": \"v\","
		      "\"field-class\": {"
		        "\"type\": \"variant\","
		        "\"selector-field-location\": {"
		          "\"path\": [\"id\"]"
		        "},"
		        "\"options\": ["
		          "{"
		            "\"name\": \"compact\","
		            "\"selector-field-ranges\": [[0, 65534]],"
		            "\"field-class\": {"
		              "\"type\": \"structure\","
		              "\"member-classes\": ["
		                "{"
		                  "\"name\": \"timestamp\","
		                  "\"field-class\": {"
		                    "\"type\": \"fixed-length-unsigned-integer\","
		                    "\"length\": 32,"
		                    "\"alignment\": %u,"
		                    "\"byte-order\": " CTF_2_QUOTED_BYTE_ORDER ","
		                    "\"roles\": [\"default-clock-timestamp\"]"
		                  "}"
		                "}"
		              "]"
		            "}"
		          "},"
		          "{"
		            "\"name\": \"extended\","
		            "\"selector-field-ranges\": [[65535, 65535]],"
		            "\"field-class\": {"
		              "\"type\": \"structure\","
		              "\"member-classes\": ["
		                "{"
		                  "\"name\": \"id\","
		                  "\"field-class\": \"u32-erc-id\""
		                "},"
		                "{"
		                  "\"name\": \"timestamp\","
		                  "\"field-class\": \"u64-ts\""
		                "}"
		              "]"
		            "}"
		          "}"
		        "]"
		      "}"
		    "}"
		  "]"
		"}",
		lttng_alignof(uint16_t) * CHAR_BIT,
		lttng_alignof(uint16_t) * CHAR_BIT,
		lttng_alignof(uint32_t) * CHAR_BIT);
	if (ret)
		return ret;

	/* Packet context FC */
	ret = write_fc_alias_fragment(session, "pkt-ctx",
		"{"
		  "\"type\": \"structure\","
		  "\"member-classes\": ["
		    "{"
		      "\"name\": \"timestamp_begin\","
		      "\"field-class\": \"u64-ts\""
		    "},"
		    "{"
		      "\"name\": \"timestamp_end\","
		      "\"field-class\": {"
		        "\"type\": \"fixed-length-unsigned-integer\","
		        "\"length\": 64,"
		        "\"alignment\": %u,"
		        "\"byte-order\": " CTF_2_QUOTED_BYTE_ORDER ","
		        "\"roles\": [\"packet-end-default-clock-timestamp\"]"
		      "}"
		    "},"
		    "{"
		      "\"name\": \"content_size\","
		      "\"field-class\": {"
		        "\"type\": \"fixed-length-unsigned-integer\","
		        "\"length\": 64,"
		        "\"alignment\": %u,"
		        "\"byte-order\": " CTF_2_QUOTED_BYTE_ORDER ","
		        "\"roles\": [\"packet-content-length\"]"
		      "}"
		    "},"
		    "{"
		      "\"name\": \"packet_size\","
		      "\"field-class\": {"
		        "\"type\": \"fixed-length-unsigned-integer\","
		        "\"length\": 64,"
		        "\"alignment\": %u,"
		        "\"byte-order\": " CTF_2_QUOTED_BYTE_ORDER ","
		        "\"roles\": [\"packet-total-length\"]"
		      "}"
		    "},"
		    "{"
		      "\"name\": \"packet_seq_num\","
		      "\"field-class\": {"
		        "\"type\": \"fixed-length-unsigned-integer\","
		        "\"length\": 64,"
		        "\"alignment\": %u,"
		        "\"byte-order\": " CTF_2_QUOTED_BYTE_ORDER ","
		        "\"roles\": [\"packet-sequence-number\"]"
		      "}"
		    "},"
		    "{"
		      "\"name\": \"events_discarded\","
		      "\"field-class\": {"
		        "\"type\": \"fixed-length-unsigned-integer\","
		        "\"length\": %u,"
		        "\"alignment\": %u,"
		        "\"byte-order\": " CTF_2_QUOTED_BYTE_ORDER ","
		        "\"roles\": [\"discarded-event-record-counter-snapshot\"]"
		      "}"
		    "},"
		    "{"
		      "\"name\": \"cpu_id\","
		      "\"field-class\": {"
		        "\"type\": \"fixed-length-unsigned-integer\","
		        "\"length\": 32,"
		        "\"alignment\": %u,"
		        "\"byte-order\": " CTF_2_QUOTED_BYTE_ORDER
		      "}"
		    "}"
		  "]"
		"}",
		lttng_alignof(uint64_t) * CHAR_BIT,
		lttng_alignof(uint64_t) * CHAR_BIT,
		lttng_alignof(uint64_t) * CHAR_BIT,
		lttng_alignof(uint64_t) * CHAR_BIT,
		sizeof(unsigned long) * CHAR_BIT,
		lttng_alignof(unsigned long) * CHAR_BIT,
		lttng_alignof(uint32_t) * CHAR_BIT);
	if (ret)
		return ret;

	/* Packet header FC */
	return write_fc_alias_fragment(session, "pkt-header",
		"{"
		  "\"type\": \"structure\","
		  "\"member-classes\": ["
		    "{"
		      "\"name\": \"magic\","
		      "\"field-class\": {"
		        "\"type\": \"fixed-length-unsigned-integer\","
		        "\"length\": 32,"
		        "\"alignment\": %u,"
		        "\"byte-order\": " CTF_2_QUOTED_BYTE_ORDER ","
		        "\"roles\": [\"packet-magic-number\"]"
		      "}"
		    "},"
		    "{"
		      "\"name\": \"uuid\","
		      "\"field-class\": {"
		        "\"type\": \"static-length-blob\","
		        "\"length\": 16,"
		        "\"roles\": [\"metadata-stream-uuid\"]"
		      "}"
		    "},"
		    "{"
		      "\"name\": \"stream_id\","
		      "\"field-class\": {"
		        "\"type\": \"fixed-length-unsigned-integer\","
		        "\"length\": 32,"
		        "\"alignment\": %u,"
		        "\"byte-order\": " CTF_2_QUOTED_BYTE_ORDER ","
		        "\"roles\": [\"data-stream-class-id\"]"
		      "}"
		    "},"
		    "{"
		      "\"name\": \"stream_instance_id\","
		      "\"field-class\": {"
		        "\"type\": \"fixed-length-unsigned-integer\","
		        "\"length\": 64,"
		        "\"alignment\": %u,"
		        "\"byte-order\": " CTF_2_QUOTED_BYTE_ORDER ","
		        "\"roles\": [\"data-stream-id\"]"
		      "}"
		    "}"
		  "]"
		"}",
		lttng_alignof(uint32_t) * CHAR_BIT,
		lttng_alignof(uint32_t) * CHAR_BIT,
		lttng_alignof(uint64_t) * CHAR_BIT);
}

/*
 * Writes a preamble fragment for `session`.
 */
static
int write_preamble_fragment(struct lttng_kernel_session * const session)
{
	int ret;

	ret = open_fragment(session, "preamble");
	if (ret)
		return ret;

	/* Write `version` property */
	ret = lttng_kernel_mj_gen_uint(session, "version", 2);
	if (ret)
		return ret;

	/* Write `uuid` property */
	{
		unsigned int i;

		ret = lttng_kernel_mj_gen_open_array(session, "uuid");
		if (ret)
			return ret;

		for (i = 0; i < 16; i++) {
			ret = lttng_kernel_mj_gen_uint(session, NULL,
				session->priv->uuid.b[i]);
			if (ret)
				return ret;
		}

		ret = lttng_kernel_mj_gen_close_array(session);
		if (ret)
			return ret;
	}

	return close_fragment(session);
}

int lttng_session_metadata_statedump_ctf_2(
		struct lttng_kernel_session * const session)
{
	int ret;

	/* Write preamble fragment */
	ret = write_preamble_fragment(session);
	if (ret)
		return ret;

	/* Write all field class alias fragments */
	ret = write_fc_alias_fragments(session);
	if (ret)
		return ret;

	/* Write trace class fragment */
	ret = write_trace_cls_fragment(session);
	if (ret)
		return ret;

	/* Write single clock class fragment */
	return write_clk_cls_fragment(session);
}
