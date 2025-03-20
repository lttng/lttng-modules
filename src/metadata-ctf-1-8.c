/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * metadata-ctf-1-8.c
 *
 * CTF 1.8 metadata stream serialization.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/types.h>
#include <linux/utsname.h>
#include <linux/dmi.h>

#include <lttng/events.h>
#include <lttng/events-internal.h>

#include "lttng-metadata-print.h"
#include "metadata-ctf-1-8.h"
#include "metadata-ctf-common.h"

static
int _lttng_type_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_type_common *type,
		enum lttng_kernel_string_encoding parent_encoding,
		size_t nesting);
static
int _lttng_field_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting, const char **prev_field_name_p);

static
int print_tabs(struct lttng_kernel_session *session, size_t nesting)
{
	size_t i;

	for (i = 0; i < nesting; i++) {
		int ret;

		ret = lttng_metadata_printf(session, "	");
		if (ret) {
			return ret;
		}
	}
	return 0;
}

static
int lttng_field_name_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting)
{
	return lttng_metadata_printf(session, " _%s;\n", field->name);
}

static
int _lttng_integer_type_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_type_integer *type,
		enum lttng_kernel_string_encoding parent_encoding,
		size_t nesting)
{
	int ret;

	ret = print_tabs(session, nesting);
	if (ret)
		return ret;
	ret = lttng_metadata_printf(session,
		"integer { size = %u; align = %u; signed = %u; encoding = %s; base = %u;%s }",
		type->size,
		type->alignment,
		type->signedness,
		(parent_encoding == lttng_kernel_string_encoding_none)
			? "none"
			: (parent_encoding == lttng_kernel_string_encoding_UTF8)
				? "UTF8"
				: "ASCII",
		type->base,
#if __BYTE_ORDER == __BIG_ENDIAN
		type->reverse_byte_order ? " byte_order = le;" : ""
#else
		type->reverse_byte_order ? " byte_order = be;" : ""
#endif
	);
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_struct_type_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_type_struct *type,
		size_t nesting)
{
	const char *prev_field_name = NULL;
	int ret;
	uint32_t i, nr_fields;
	unsigned int alignment;

	ret = print_tabs(session, nesting);
	if (ret)
		return ret;
	ret = lttng_metadata_printf(session,
		"struct {\n");
	if (ret)
		return ret;
	nr_fields = type->nr_fields;
	for (i = 0; i < nr_fields; i++) {
		const struct lttng_kernel_event_field *iter_field;

		iter_field = type->fields[i];
		ret = _lttng_field_statedump(session, iter_field, nesting + 1, &prev_field_name);
		if (ret)
			return ret;
	}
	ret = print_tabs(session, nesting);
	if (ret)
		return ret;
	alignment = type->alignment;
	if (alignment) {
		ret = lttng_metadata_printf(session,
			"} align(%u)",
			alignment);
	} else {
		ret = lttng_metadata_printf(session,
			"}");
	}
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_struct_field_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting)
{
	int ret;

	ret = _lttng_struct_type_statedump(session,
			lttng_kernel_get_type_struct(field->type), nesting);
	if (ret)
		return ret;
	return lttng_field_name_statedump(session, field, nesting);
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_variant_type_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_type_variant *type,
		size_t nesting,
		const char *prev_field_name)
{
	const char *tag_name;
	int ret;
	uint32_t i, nr_choices;

	tag_name = type->tag_name;
	if (!tag_name)
		tag_name = prev_field_name;
	if (!tag_name)
		return -EINVAL;
	/*
	 * CTF 1.8 does not allow expressing nonzero variant alignment in a nestable way.
	 */
	if (type->alignment != 0)
		return -EINVAL;
	ret = print_tabs(session, nesting);
	if (ret)
		return ret;
	ret = lttng_metadata_printf(session,
		"variant <_%s> {\n",
		tag_name);
	if (ret)
		return ret;
	nr_choices = type->nr_choices;
	for (i = 0; i < nr_choices; i++) {
		const struct lttng_kernel_event_field *iter_field;

		iter_field = type->choices[i];
		ret = _lttng_field_statedump(session, iter_field, nesting + 1, NULL);
		if (ret)
			return ret;
	}
	ret = print_tabs(session, nesting);
	if (ret)
		return ret;
	ret = lttng_metadata_printf(session,
		"}");
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_variant_field_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting,
		const char *prev_field_name)
{
	int ret;

	ret = _lttng_variant_type_statedump(session,
			lttng_kernel_get_type_variant(field->type), nesting,
			prev_field_name);
	if (ret)
		return ret;
	return lttng_field_name_statedump(session, field, nesting);
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_array_field_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting)
{
	int ret;
	const struct lttng_kernel_type_array *array_type;
	const struct lttng_kernel_type_common *elem_type;

	array_type = lttng_kernel_get_type_array(field->type);
	WARN_ON_ONCE(!array_type);

	if (array_type->alignment) {
		ret = print_tabs(session, nesting);
		if (ret)
			return ret;
		ret = lttng_metadata_printf(session,
		"struct { } align(%u) _%s_padding;\n",
				array_type->alignment * CHAR_BIT,
				field->name);
		if (ret)
			return ret;
	}
	/*
	 * Nested compound types: Only array of structures and variants are
	 * currently supported.
	 */
	elem_type = array_type->elem_type;
	switch (elem_type->type) {
	case lttng_kernel_type_integer:
	case lttng_kernel_type_struct:
	case lttng_kernel_type_variant:
		ret = _lttng_type_statedump(session, elem_type,
				array_type->encoding, nesting);
		if (ret)
			return ret;
		break;

	default:
		return -EINVAL;
	}
	ret = lttng_metadata_printf(session,
		" _%s[%u];\n",
		field->name,
		array_type->length);
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_sequence_field_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting,
		const char *prev_field_name)
{
	int ret;
	const char *length_name;
	const struct lttng_kernel_type_sequence *sequence_type;
	const struct lttng_kernel_type_common *elem_type;

	sequence_type = lttng_kernel_get_type_sequence(field->type);
	WARN_ON_ONCE(!sequence_type);

	length_name = sequence_type->length_name;
	if (!length_name)
		length_name = prev_field_name;
	if (!length_name)
		return -EINVAL;

	if (sequence_type->alignment) {
		ret = print_tabs(session, nesting);
		if (ret)
			return ret;
		ret = lttng_metadata_printf(session,
		"struct { } align(%u) _%s_padding;\n",
				sequence_type->alignment * CHAR_BIT,
				field->name);
		if (ret)
			return ret;
	}

	/*
	 * Nested compound types: Only array of structures and variants are
	 * currently supported.
	 */
	elem_type = sequence_type->elem_type;
	switch (elem_type->type) {
	case lttng_kernel_type_integer:
	case lttng_kernel_type_struct:
	case lttng_kernel_type_variant:
		ret = _lttng_type_statedump(session, elem_type,
				sequence_type->encoding, nesting);
		if (ret)
			return ret;
		break;

	default:
		return -EINVAL;
	}
	ret = lttng_metadata_printf(session,
		" _%s[ _%s ];\n",
		field->name,
		length_name);
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_enum_type_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_type_enum *type,
		size_t nesting)
{
	const struct lttng_kernel_enum_desc *enum_desc;
	const struct lttng_kernel_type_common *container_type;
	int ret;
	unsigned int i, nr_entries;

	container_type = type->container_type;
	if (container_type->type != lttng_kernel_type_integer) {
		ret = -EINVAL;
		goto end;
	}
	enum_desc = type->desc;
	nr_entries = enum_desc->nr_entries;

	ret = print_tabs(session, nesting);
	if (ret)
		goto end;
	ret = lttng_metadata_printf(session, "enum : ");
	if (ret)
		goto end;
	ret = _lttng_integer_type_statedump(session, lttng_kernel_get_type_integer(container_type),
			lttng_kernel_string_encoding_none, 0);
	if (ret)
		goto end;
	ret = lttng_metadata_printf(session, " {\n");
	if (ret)
		goto end;
	/* Dump all entries */
	for (i = 0; i < nr_entries; i++) {
		const struct lttng_kernel_enum_entry *entry = enum_desc->entries[i];
		int j, len;

		ret = print_tabs(session, nesting + 1);
		if (ret)
			goto end;
		ret = lttng_metadata_printf(session,
				"\"");
		if (ret)
			goto end;

		if (enum_desc->is_tag) {
			ret = lttng_metadata_printf(session, "_");
			if (ret)
				goto end;
		}

		len = strlen(entry->string);
		/* Escape the character '"' */
		for (j = 0; j < len; j++) {
			char c = entry->string[j];

			switch (c) {
			case '"':
				ret = lttng_metadata_printf(session,
						"\\\"");
				break;
			case '\\':
				ret = lttng_metadata_printf(session,
						"\\\\");
				break;
			default:
				ret = lttng_metadata_printf(session,
						"%c", c);
				break;
			}
			if (ret)
				goto end;
		}
		ret = lttng_metadata_printf(session, "\"");
		if (ret)
			goto end;

		if (entry->options.is_auto) {
			ret = lttng_metadata_printf(session, ",\n");
			if (ret)
				goto end;
		} else {
			ret = lttng_metadata_printf(session,
					" = ");
			if (ret)
				goto end;
			if (entry->start.signedness)
				ret = lttng_metadata_printf(session,
					"%lld", (long long) entry->start.value);
			else
				ret = lttng_metadata_printf(session,
					"%llu", entry->start.value);
			if (ret)
				goto end;
			if (entry->start.signedness == entry->end.signedness &&
					entry->start.value
						== entry->end.value) {
				ret = lttng_metadata_printf(session,
					",\n");
			} else {
				if (entry->end.signedness) {
					ret = lttng_metadata_printf(session,
						" ... %lld,\n",
						(long long) entry->end.value);
				} else {
					ret = lttng_metadata_printf(session,
						" ... %llu,\n",
						entry->end.value);
				}
			}
			if (ret)
				goto end;
		}
	}
	ret = print_tabs(session, nesting);
	if (ret)
		goto end;
	ret = lttng_metadata_printf(session, "}");
end:
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_enum_field_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting)
{
	int ret;
	const struct lttng_kernel_type_enum *enum_type;

	enum_type = lttng_kernel_get_type_enum(field->type);
	WARN_ON_ONCE(!enum_type);
	ret = _lttng_enum_type_statedump(session, enum_type, nesting);
	if (ret)
		return ret;
	return lttng_field_name_statedump(session, field, nesting);
}

static
int _lttng_integer_field_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting)
{
	int ret;

	ret = _lttng_integer_type_statedump(session, lttng_kernel_get_type_integer(field->type),
			lttng_kernel_string_encoding_none, nesting);
	if (ret)
		return ret;
	return lttng_field_name_statedump(session, field, nesting);
}

static
int _lttng_string_type_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_type_string *type,
		size_t nesting)
{
	int ret;

	/* Default encoding is UTF8 */
	ret = print_tabs(session, nesting);
	if (ret)
		return ret;
	ret = lttng_metadata_printf(session,
		"string%s",
		type->encoding == lttng_kernel_string_encoding_ASCII ?
			" { encoding = ASCII; }" : "");
	return ret;
}

static
int _lttng_string_field_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting)
{
	const struct lttng_kernel_type_string *string_type;
	int ret;

	string_type = lttng_kernel_get_type_string(field->type);
	WARN_ON_ONCE(!string_type);
	ret = _lttng_string_type_statedump(session, string_type, nesting);
	if (ret)
		return ret;
	return lttng_field_name_statedump(session, field, nesting);
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_type_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_type_common *type,
		enum lttng_kernel_string_encoding parent_encoding,
		size_t nesting)
{
	int ret = 0;

	switch (type->type) {
	case lttng_kernel_type_integer:
		ret = _lttng_integer_type_statedump(session,
				lttng_kernel_get_type_integer(type),
				parent_encoding, nesting);
		break;
	case lttng_kernel_type_enum:
		ret = _lttng_enum_type_statedump(session,
				lttng_kernel_get_type_enum(type),
				nesting);
		break;
	case lttng_kernel_type_string:
		ret = _lttng_string_type_statedump(session,
				lttng_kernel_get_type_string(type),
				nesting);
		break;
	case lttng_kernel_type_struct:
		ret = _lttng_struct_type_statedump(session,
				lttng_kernel_get_type_struct(type),
				nesting);
		break;
	case lttng_kernel_type_variant:
		ret = _lttng_variant_type_statedump(session,
				lttng_kernel_get_type_variant(type),
				nesting, NULL);
		break;

	/* Nested arrays and sequences are not supported yet. */
	case lttng_kernel_type_array:
	case lttng_kernel_type_sequence:
	default:
		WARN_ON_ONCE(1);
		return -EINVAL;
	}
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_field_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting,
		const char **prev_field_name_p)
{
	const char *prev_field_name = NULL;
	int ret = 0;

	if (prev_field_name_p)
		prev_field_name = *prev_field_name_p;
	switch (field->type->type) {
	case lttng_kernel_type_integer:
		ret = _lttng_integer_field_statedump(session, field, nesting);
		break;
	case lttng_kernel_type_enum:
		ret = _lttng_enum_field_statedump(session, field, nesting);
		break;
	case lttng_kernel_type_string:
		ret = _lttng_string_field_statedump(session, field, nesting);
		break;
	case lttng_kernel_type_struct:
		ret = _lttng_struct_field_statedump(session, field, nesting);
		break;
	case lttng_kernel_type_array:
		ret = _lttng_array_field_statedump(session, field, nesting);
		break;
	case lttng_kernel_type_sequence:
		ret = _lttng_sequence_field_statedump(session, field, nesting, prev_field_name);
		break;
	case lttng_kernel_type_variant:
		ret = _lttng_variant_field_statedump(session, field, nesting, prev_field_name);
		break;

	default:
		WARN_ON_ONCE(1);
		return -EINVAL;
	}
	if (prev_field_name_p)
		*prev_field_name_p = field->name;
	return ret;
}

static
int _lttng_context_metadata_statedump(struct lttng_kernel_session *session,
				    struct lttng_kernel_ctx *ctx)
{
	const char *prev_field_name = NULL;
	int ret = 0;
	int i;

	if (!ctx)
		return 0;
	for (i = 0; i < ctx->nr_fields; i++) {
		const struct lttng_kernel_ctx_field *field = &ctx->fields[i];

		ret = _lttng_field_statedump(session, field->event_field, 2, &prev_field_name);
		if (ret)
			return ret;
	}
	return ret;
}

static
int _lttng_fields_metadata_statedump(struct lttng_kernel_session *session,
				   struct lttng_kernel_event_recorder *event_recorder)
{
	const char *prev_field_name = NULL;
	const struct lttng_kernel_event_desc *desc = event_recorder->priv->parent.parent.desc;
	int ret = 0;
	int i;

	for (i = 0; i < desc->tp_class->nr_fields; i++) {
		const struct lttng_kernel_event_field *field = desc->tp_class->fields[i];

		ret = _lttng_field_statedump(session, field, 2, &prev_field_name);
		if (ret)
			return ret;
	}
	return ret;
}

int lttng_event_recorder_metadata_statedump_ctf_1_8(struct lttng_kernel_session *session,
		struct lttng_kernel_event_recorder *event_recorder)
{
	int ret;

	ret = lttng_metadata_printf(session,
		"event {\n"
		"	name = \"%s\";\n"
		"	id = %llu;\n"
		"	stream_id = %u;\n",
		event_recorder->priv->parent.parent.desc->event_name,
		event_recorder->priv->parent.id,
		event_recorder->chan->priv->id);
	if (ret)
		goto end;

	ret = lttng_metadata_printf(session,
		"	fields := struct {\n"
		);
	if (ret)
		goto end;

	ret = _lttng_fields_metadata_statedump(session, event_recorder);
	if (ret)
		goto end;

	/*
	 * LTTng space reservation can only reserve multiples of the
	 * byte size.
	 */
	ret = lttng_metadata_printf(session,
		"	};\n"
		"};\n\n");

end:
	return ret;

}

/*
 * Must be called with sessions_mutex held.
 * The entire channel metadata is printed as a single atomic metadata
 * transaction.
 */
int lttng_channel_metadata_statedump_ctf_1_8(struct lttng_kernel_session *session,
				    struct lttng_kernel_channel_buffer *chan)
{
	int ret = 0;

	ret = lttng_metadata_printf(session,
		"stream {\n"
		"	id = %u;\n"
		"	event.header := %s;\n"
		"	packet.context := struct packet_context;\n",
		chan->priv->id,
		chan->priv->header_type == 1 ? "struct event_header_compact" :
			"struct event_header_large");
	if (ret)
		goto end;

	if (chan->priv->ctx) {
		ret = lttng_metadata_printf(session,
			"	event.context := struct {\n");
		if (ret)
			goto end;
	}
	ret = _lttng_context_metadata_statedump(session, chan->priv->ctx);
	if (ret)
		goto end;
	if (chan->priv->ctx) {
		ret = lttng_metadata_printf(session,
			"	};\n");
		if (ret)
			goto end;
	}

	ret = lttng_metadata_printf(session,
		"};\n\n");

end:
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_stream_packet_context_declare(struct lttng_kernel_session *session)
{
	return lttng_metadata_printf(session,
		"struct packet_context {\n"
		"	uint64_clock_monotonic_t timestamp_begin;\n"
		"	uint64_clock_monotonic_t timestamp_end;\n"
		"	uint64_t content_size;\n"
		"	uint64_t packet_size;\n"
		"	uint64_t packet_seq_num;\n"
		"	unsigned long events_discarded;\n"
		"	uint32_t cpu_id;\n"
		"};\n\n"
		);
}

/*
 * Compact header:
 * id: range: 0 - 30.
 * id 31 is reserved to indicate an extended header.
 *
 * Large header:
 * id: range: 0 - 65534.
 * id 65535 is reserved to indicate an extended header.
 *
 * Must be called with sessions_mutex held.
 */
static
int _lttng_event_header_declare(struct lttng_kernel_session *session)
{
	return lttng_metadata_printf(session,
	"struct event_header_compact {\n"
	"	enum : uint5_t { compact = 0 ... 30, extended = 31 } id;\n"
	"	variant <id> {\n"
	"		struct {\n"
	"			uint27_clock_monotonic_t timestamp;\n"
	"		} compact;\n"
	"		struct {\n"
	"			uint32_t id;\n"
	"			uint64_clock_monotonic_t timestamp;\n"
	"		} extended;\n"
	"	} v;\n"
	"} align(%u);\n"
	"\n"
	"struct event_header_large {\n"
	"	enum : uint16_t { compact = 0 ... 65534, extended = 65535 } id;\n"
	"	variant <id> {\n"
	"		struct {\n"
	"			uint32_clock_monotonic_t timestamp;\n"
	"		} compact;\n"
	"		struct {\n"
	"			uint32_t id;\n"
	"			uint64_clock_monotonic_t timestamp;\n"
	"		} extended;\n"
	"	} v;\n"
	"} align(%u);\n\n",
	lttng_alignof(uint32_t) * CHAR_BIT,
	lttng_alignof(uint16_t) * CHAR_BIT
	);
}

static
int print_escaped_ctf_string(struct lttng_kernel_session *session, const char *string)
{
	int ret = 0;
	size_t i;
	char cur;

	i = 0;
	cur = string[i];
	while (cur != '\0') {
		switch (cur) {
		case '\n':
			ret = lttng_metadata_printf(session, "%s", "\\n");
			break;
		case '\\':
		case '"':
			ret = lttng_metadata_printf(session, "%c", '\\');
			if (ret)
				goto error;
			/* We still print the current char */
			lttng_fallthrough;
		default:
			ret = lttng_metadata_printf(session, "%c", cur);
			break;
		}

		if (ret)
			goto error;

		cur = string[++i];
	}
error:
	return ret;
}

static
int print_metadata_escaped_field(struct lttng_kernel_session *session, const char *field,
		const char *field_value)
{
	int ret;

	ret = lttng_metadata_printf(session, "	%s = \"", field);
	if (ret)
		goto error;

	ret = print_escaped_ctf_string(session, field_value);
	if (ret)
		goto error;

	ret = lttng_metadata_printf(session, "\";\n");

error:
	return ret;
}

int lttng_session_metadata_statedump_ctf_1_8(struct lttng_kernel_session *session)
{
	unsigned char *uuid_c = session->priv->uuid.b;
	unsigned char uuid_s[37], clock_uuid_s[BOOT_ID_LEN];
	const char *product_uuid;
	int ret = 0;

	snprintf(uuid_s, sizeof(uuid_s),
		"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		uuid_c[0], uuid_c[1], uuid_c[2], uuid_c[3],
		uuid_c[4], uuid_c[5], uuid_c[6], uuid_c[7],
		uuid_c[8], uuid_c[9], uuid_c[10], uuid_c[11],
		uuid_c[12], uuid_c[13], uuid_c[14], uuid_c[15]);

	ret = lttng_metadata_printf(session,
		"typealias integer { size = 8; align = %u; signed = false; } := uint8_t;\n"
		"typealias integer { size = 16; align = %u; signed = false; } := uint16_t;\n"
		"typealias integer { size = 32; align = %u; signed = false; } := uint32_t;\n"
		"typealias integer { size = 64; align = %u; signed = false; } := uint64_t;\n"
		"typealias integer { size = %u; align = %u; signed = false; } := unsigned long;\n"
		"typealias integer { size = 5; align = 1; signed = false; } := uint5_t;\n"
		"typealias integer { size = 27; align = 1; signed = false; } := uint27_t;\n"
		"\n"
		"trace {\n"
		"	major = %u;\n"
		"	minor = %u;\n"
		"	uuid = \"%s\";\n"
		"	byte_order = %s;\n"
		"	packet.header := struct {\n"
		"		uint32_t magic;\n"
		"		uint8_t  uuid[16];\n"
		"		uint32_t stream_id;\n"
		"		uint64_t stream_instance_id;\n"
		"	};\n"
		"};\n\n",
		lttng_alignof(uint8_t) * CHAR_BIT,
		lttng_alignof(uint16_t) * CHAR_BIT,
		lttng_alignof(uint32_t) * CHAR_BIT,
		lttng_alignof(uint64_t) * CHAR_BIT,
		sizeof(unsigned long) * CHAR_BIT,
		lttng_alignof(unsigned long) * CHAR_BIT,
		CTF_SPEC_MAJOR,
		CTF_SPEC_MINOR,
		uuid_s,
#if __BYTE_ORDER == __BIG_ENDIAN
		"be"
#else
		"le"
#endif
		);
	if (ret)
		goto end;

	ret = lttng_metadata_printf(session,
		"env {\n"
		"	hostname = \"%s\";\n"
		"	domain = \"kernel\";\n"
		"	sysname = \"%s\";\n"
		"	kernel_release = \"%s\";\n"
		"	kernel_version = \"%s\";\n"
		"	tracer_name = \"lttng-modules\";\n"
		"	tracer_major = %d;\n"
		"	tracer_minor = %d;\n"
		"	tracer_patchlevel = %d;\n"
		"	trace_buffering_scheme = \"global\";\n",
		current->nsproxy->uts_ns->name.nodename,
		utsname()->sysname,
		utsname()->release,
		utsname()->version,
		LTTNG_MODULES_MAJOR_VERSION,
		LTTNG_MODULES_MINOR_VERSION,
		LTTNG_MODULES_PATCHLEVEL_VERSION
		);
	if (ret)
		goto end;

	ret = print_metadata_escaped_field(session, "trace_name", session->priv->name);
	if (ret)
		goto end;
	ret = print_metadata_escaped_field(session, "trace_creation_datetime",
			session->priv->creation_time);
	if (ret)
		goto end;

	/* Add the product UUID to the 'env' section */
	product_uuid = dmi_get_system_info(DMI_PRODUCT_UUID);
	if (product_uuid) {
		ret = lttng_metadata_printf(session,
				"	product_uuid = \"%s\";\n",
				product_uuid
				);
		if (ret)
			goto end;
	}

	/* Close the 'env' section */
	ret = lttng_metadata_printf(session, "};\n\n");
	if (ret)
		goto end;

	ret = lttng_metadata_printf(session,
		"clock {\n"
		"	name = \"%s\";\n",
		trace_clock_name()
		);
	if (ret)
		goto end;

	if (!trace_clock_uuid(clock_uuid_s)) {
		ret = lttng_metadata_printf(session,
			"	uuid = \"%s\";\n",
			clock_uuid_s
			);
		if (ret)
			goto end;
	}

	ret = lttng_metadata_printf(session,
		"	description = \"%s\";\n"
		"	freq = %llu; /* Frequency, in Hz */\n"
		"	/* clock value offset from Epoch is: offset * (1/freq) */\n"
		"	offset = %lld;\n"
		"};\n\n",
		trace_clock_description(),
		(unsigned long long) trace_clock_freq(),
		(long long) measure_clock_offset()
		);
	if (ret)
		goto end;

	ret = lttng_metadata_printf(session,
		"typealias integer {\n"
		"	size = 27; align = 1; signed = false;\n"
		"	map = clock.%s.value;\n"
		"} := uint27_clock_monotonic_t;\n"
		"\n"
		"typealias integer {\n"
		"	size = 32; align = %u; signed = false;\n"
		"	map = clock.%s.value;\n"
		"} := uint32_clock_monotonic_t;\n"
		"\n"
		"typealias integer {\n"
		"	size = 64; align = %u; signed = false;\n"
		"	map = clock.%s.value;\n"
		"} := uint64_clock_monotonic_t;\n\n",
		trace_clock_name(),
		lttng_alignof(uint32_t) * CHAR_BIT,
		trace_clock_name(),
		lttng_alignof(uint64_t) * CHAR_BIT,
		trace_clock_name()
		);
	if (ret)
		goto end;

	ret = _lttng_stream_packet_context_declare(session);
	if (ret)
		goto end;

	ret = _lttng_event_header_declare(session);
	if (ret)
		goto end;

end:
	return ret;
}
