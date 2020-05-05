/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-context-hostname.c
 *
 * LTTng hostname context.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/utsname.h>
#include <lttng-events.h>
#include <wrapper/ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <lttng-tracer.h>

#define LTTNG_HOSTNAME_CTX_LEN	(__NEW_UTS_LEN + 1)

static
size_t hostname_get_size(size_t offset)
{
	size_t size = 0;

	size += LTTNG_HOSTNAME_CTX_LEN;
	return size;
}

static
void hostname_record(struct lttng_ctx_field *field,
		 struct lib_ring_buffer_ctx *ctx,
		 struct lttng_channel *chan)
{
	struct nsproxy *nsproxy;
	struct uts_namespace *ns;
	char *hostname;

	/*
	 * No need to take the RCU read-side lock to read current
	 * nsproxy. (documented in nsproxy.h)
	 */
	nsproxy = current->nsproxy;
	if (nsproxy) {
		ns = nsproxy->uts_ns;
		hostname = ns->name.nodename;
		chan->ops->event_write(ctx, hostname,
				LTTNG_HOSTNAME_CTX_LEN);
	} else {
		chan->ops->event_memset(ctx, 0,
				LTTNG_HOSTNAME_CTX_LEN);
	}
}

static
void hostname_get_value(struct lttng_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	struct nsproxy *nsproxy;
	struct uts_namespace *ns;
	char *hostname;

	/*
	 * No need to take the RCU read-side lock to read current
	 * nsproxy. (documented in nsproxy.h)
	 */
	nsproxy = current->nsproxy;
	if (nsproxy) {
		ns = nsproxy->uts_ns;
		hostname = ns->name.nodename;
	} else {
		hostname = "";
	}
	value->str = hostname;
}

int lttng_add_hostname_to_ctx(struct lttng_ctx **ctx)
{
	struct lttng_ctx_field *field;

	field = lttng_append_context(ctx);
	if (!field)
		return -ENOMEM;
	if (lttng_find_context(*ctx, "hostname")) {
		lttng_remove_context_field(ctx, field);
		return -EEXIST;
	}
	field->event_field.name = "hostname";
	field->event_field.type.atype = atype_array;
	field->event_field.type.u.array.elem_type.atype = atype_integer;
	field->event_field.type.u.array.elem_type.u.basic.integer.size = sizeof(char) * CHAR_BIT;
	field->event_field.type.u.array.elem_type.u.basic.integer.alignment = lttng_alignof(char) * CHAR_BIT;
	field->event_field.type.u.array.elem_type.u.basic.integer.signedness = lttng_is_signed_type(char);
	field->event_field.type.u.array.elem_type.u.basic.integer.reverse_byte_order = 0;
	field->event_field.type.u.array.elem_type.u.basic.integer.base = 10;
	field->event_field.type.u.array.elem_type.u.basic.integer.encoding = lttng_encode_UTF8;
	field->event_field.type.u.array.length = LTTNG_HOSTNAME_CTX_LEN;

	field->get_size = hostname_get_size;
	field->record = hostname_record;
	field->get_value = hostname_get_value;
	lttng_context_update(*ctx);
	wrapper_vmalloc_sync_mappings();
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_add_hostname_to_ctx);
