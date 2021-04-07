/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
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
#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <lttng/tracer.h>

#define LTTNG_HOSTNAME_CTX_LEN	(__NEW_UTS_LEN + 1)

static
size_t hostname_get_size(size_t offset)
{
	size_t size = 0;

	size += LTTNG_HOSTNAME_CTX_LEN;
	return size;
}

static
void hostname_record(struct lttng_kernel_ctx_field *field,
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
void hostname_get_value(struct lttng_kernel_ctx_field *field,
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

static const struct lttng_kernel_ctx_field *ctx_field = lttng_kernel_static_ctx_field(
	lttng_kernel_static_event_field("hostname",
		lttng_kernel_static_type_array_text(LTTNG_HOSTNAME_CTX_LEN),
		false, false, false),
	hostname_get_size,
	NULL,
	hostname_record,
	hostname_get_value,
	NULL, NULL);

int lttng_add_hostname_to_ctx(struct lttng_kernel_ctx **ctx)
{
	int ret;

	if (lttng_kernel_find_context(*ctx, ctx_field->event_field->name))
		return -EEXIST;
	ret = lttng_kernel_context_append(ctx, ctx_field);
	wrapper_vmalloc_sync_mappings();
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_add_hostname_to_ctx);
