/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context.c
 *
 * LTTng trace/channel/event context management.
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <wrapper/vmalloc.h>	/* for wrapper_vmalloc_sync_all() */
#include <lttng-events.h>
#include <lttng-tracer.h>

/*
 * The filter implementation requires that two consecutive "get" for the
 * same context performed by the same thread return the same result.
 */

/*
 * Static array of contexts, for $ctx filters.
 */
struct lttng_ctx *lttng_static_ctx;

int lttng_find_context(struct lttng_ctx *ctx, const char *name)
{
	unsigned int i;

	for (i = 0; i < ctx->nr_fields; i++) {
		/* Skip allocated (but non-initialized) contexts */
		if (!ctx->fields[i].event_field.name)
			continue;
		if (!strcmp(ctx->fields[i].event_field.name, name))
			return 1;
	}
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_find_context);

int lttng_get_context_index(struct lttng_ctx *ctx, const char *name)
{
	unsigned int i;
	const char *subname;

	if (!ctx)
		return -1;
	if (strncmp(name, "$ctx.", strlen("$ctx.")) == 0) {
		subname = name + strlen("$ctx.");
	} else {
		subname = name;
	}
	for (i = 0; i < ctx->nr_fields; i++) {
		/* Skip allocated (but non-initialized) contexts */
		if (!ctx->fields[i].event_field.name)
			continue;
		if (!strcmp(ctx->fields[i].event_field.name, subname))
			return i;
	}
	return -1;
}
EXPORT_SYMBOL_GPL(lttng_get_context_index);

/*
 * Note: as we append context information, the pointer location may change.
 */
struct lttng_ctx_field *lttng_append_context(struct lttng_ctx **ctx_p)
{
	struct lttng_ctx_field *field;
	struct lttng_ctx *ctx;

	if (!*ctx_p) {
		*ctx_p = kzalloc(sizeof(struct lttng_ctx), GFP_KERNEL);
		if (!*ctx_p)
			return NULL;
		(*ctx_p)->largest_align = 1;
	}
	ctx = *ctx_p;
	if (ctx->nr_fields + 1 > ctx->allocated_fields) {
		struct lttng_ctx_field *new_fields;

		ctx->allocated_fields = max_t(size_t, 1, 2 * ctx->allocated_fields);
		new_fields = lttng_kvzalloc(ctx->allocated_fields * sizeof(struct lttng_ctx_field), GFP_KERNEL);
		if (!new_fields)
			return NULL;
		if (ctx->fields)
			memcpy(new_fields, ctx->fields, sizeof(*ctx->fields) * ctx->nr_fields);
		lttng_kvfree(ctx->fields);
		ctx->fields = new_fields;
	}
	field = &ctx->fields[ctx->nr_fields];
	ctx->nr_fields++;
	return field;
}
EXPORT_SYMBOL_GPL(lttng_append_context);

/*
 * lttng_context_update() should be called at least once between context
 * modification and trace start.
 */
void lttng_context_update(struct lttng_ctx *ctx)
{
	int i;
	size_t largest_align = 8;	/* in bits */

	for (i = 0; i < ctx->nr_fields; i++) {
		struct lttng_type *type;
		size_t field_align = 8;

		type = &ctx->fields[i].event_field.type;
		switch (type->atype) {
		case atype_integer:
			field_align = type->u.integer.alignment;
			break;
		case atype_array_nestable:
		{
			const struct lttng_type *nested_type;

			nested_type = type->u.array_nestable.elem_type;
			switch (nested_type->atype) {
			case atype_integer:
				field_align = nested_type->u.integer.alignment;
				break;
			case atype_string:
				break;

			case atype_array_nestable:
			case atype_sequence_nestable:
			case atype_struct_nestable:
			case atype_variant_nestable:
			default:
				WARN_ON_ONCE(1);
				break;
			}
			field_align = max_t(size_t, field_align,
					type->u.array_nestable.alignment);
			break;
		}
		case atype_sequence_nestable:
		{
			const struct lttng_type *nested_type;

			nested_type = type->u.sequence_nestable.elem_type;
			switch (nested_type->atype) {
			case atype_integer:
				field_align = nested_type->u.integer.alignment;
				break;

			case atype_string:
				break;

			case atype_array_nestable:
			case atype_sequence_nestable:
			case atype_struct_nestable:
			case atype_variant_nestable:
			default:
				WARN_ON_ONCE(1);
				break;
			}
			field_align = max_t(size_t, field_align,
					type->u.sequence_nestable.alignment);
			break;
		}
		case atype_string:
			break;

		case atype_struct_nestable:
		case atype_variant_nestable:
			break;

		case atype_enum_nestable:
		default:
			WARN_ON_ONCE(1);
			break;
		}
		largest_align = max_t(size_t, largest_align, field_align);
	}
	ctx->largest_align = largest_align >> 3;	/* bits to bytes */
}

/*
 * Remove last context field.
 */
void lttng_remove_context_field(struct lttng_ctx **ctx_p,
				struct lttng_ctx_field *field)
{
	struct lttng_ctx *ctx;

	ctx = *ctx_p;
	ctx->nr_fields--;
	WARN_ON_ONCE(&ctx->fields[ctx->nr_fields] != field);
	memset(&ctx->fields[ctx->nr_fields], 0, sizeof(struct lttng_ctx_field));
}
EXPORT_SYMBOL_GPL(lttng_remove_context_field);

void lttng_destroy_context(struct lttng_ctx *ctx)
{
	int i;

	if (!ctx)
		return;
	for (i = 0; i < ctx->nr_fields; i++) {
		if (ctx->fields[i].destroy)
			ctx->fields[i].destroy(&ctx->fields[i]);
	}
	lttng_kvfree(ctx->fields);
	kfree(ctx);
}

int lttng_context_init(void)
{
	int ret;

	ret = lttng_add_hostname_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "Cannot add context lttng_add_hostname_to_ctx");
	}
	ret = lttng_add_nice_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "Cannot add context lttng_add_nice_to_ctx");
	}
	ret = lttng_add_pid_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "Cannot add context lttng_add_pid_to_ctx");
	}
	ret = lttng_add_ppid_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "Cannot add context lttng_add_ppid_to_ctx");
	}
	ret = lttng_add_prio_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "Cannot add context lttng_add_prio_to_ctx");
	}
	ret = lttng_add_procname_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "Cannot add context lttng_add_procname_to_ctx");
	}
	ret = lttng_add_tid_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "Cannot add context lttng_add_tid_to_ctx");
	}
	ret = lttng_add_vppid_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "Cannot add context lttng_add_vppid_to_ctx");
	}
	ret = lttng_add_vtid_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "Cannot add context lttng_add_vtid_to_ctx");
	}
	ret = lttng_add_vpid_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "Cannot add context lttng_add_vpid_to_ctx");
	}
	ret = lttng_add_cpu_id_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "Cannot add context lttng_add_cpu_id_to_ctx");
	}
	ret = lttng_add_interruptible_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "Cannot add context lttng_add_interruptible_to_ctx");
	}
	ret = lttng_add_need_reschedule_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "Cannot add context lttng_add_need_reschedule_to_ctx");
	}
	ret = lttng_add_preemptible_to_ctx(&lttng_static_ctx);
	if (ret && ret != -ENOSYS) {
		printk(KERN_WARNING "Cannot add context lttng_add_preemptible_to_ctx");
	}
	ret = lttng_add_migratable_to_ctx(&lttng_static_ctx);
	if (ret && ret != -ENOSYS) {
		printk(KERN_WARNING "Cannot add context lttng_add_migratable_to_ctx");
	}
	ret = lttng_add_cgroup_ns_to_ctx(&lttng_static_ctx);
	if (ret && ret != -ENOSYS) {
		printk(KERN_WARNING "Cannot add context lttng_add_cgroup_ns_to_ctx");
	}
	ret = lttng_add_ipc_ns_to_ctx(&lttng_static_ctx);
	if (ret && ret != -ENOSYS) {
		printk(KERN_WARNING "Cannot add context lttng_add_ipc_ns_to_ctx");
	}
	ret = lttng_add_mnt_ns_to_ctx(&lttng_static_ctx);
	if (ret && ret != -ENOSYS) {
		printk(KERN_WARNING "Cannot add context lttng_add_mnt_ns_to_ctx");
	}
	ret = lttng_add_net_ns_to_ctx(&lttng_static_ctx);
	if (ret && ret != -ENOSYS) {
		printk(KERN_WARNING "Cannot add context lttng_add_net_ns_to_ctx");
	}
	ret = lttng_add_pid_ns_to_ctx(&lttng_static_ctx);
	if (ret && ret != -ENOSYS) {
		printk(KERN_WARNING "Cannot add context lttng_add_pid_ns_to_ctx");
	}
	ret = lttng_add_user_ns_to_ctx(&lttng_static_ctx);
	if (ret && ret != -ENOSYS) {
		printk(KERN_WARNING "Cannot add context lttng_add_user_ns_to_ctx");
	}
	ret = lttng_add_uts_ns_to_ctx(&lttng_static_ctx);
	if (ret && ret != -ENOSYS) {
		printk(KERN_WARNING "Cannot add context lttng_add_uts_ns_to_ctx");
	}
	/* TODO: perf counters for filtering */
	return 0;
}

void lttng_context_exit(void)
{
	lttng_destroy_context(lttng_static_ctx);
	lttng_static_ctx = NULL;
}
