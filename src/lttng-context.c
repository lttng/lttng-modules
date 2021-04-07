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
#include <wrapper/vmalloc.h>	/* for wrapper_vmalloc_sync_mappings() */
#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <lttng/tracer.h>

/*
 * The filter implementation requires that two consecutive "get" for the
 * same context performed by the same thread return the same result.
 */

/*
 * Static array of contexts, for $ctx filters.
 */
struct lttng_kernel_ctx *lttng_static_ctx;

int lttng_kernel_find_context(struct lttng_kernel_ctx *ctx, const char *name)
{
	unsigned int i;
	const char *subname;

	if (!ctx)
		return 0;
	if (strncmp(name, "$ctx.", strlen("$ctx.")) == 0) {
		subname = name + strlen("$ctx.");
	} else {
		subname = name;
	}
	for (i = 0; i < ctx->nr_fields; i++) {
		/* Skip allocated (but non-initialized) contexts */
		if (!ctx->fields[i].event_field->name)
			continue;
		if (!strcmp(ctx->fields[i].event_field->name, subname))
			return 1;
	}
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_kernel_find_context);

int lttng_kernel_get_context_index(struct lttng_kernel_ctx *ctx, const char *name)
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
		if (!ctx->fields[i].event_field->name)
			continue;
		if (!strcmp(ctx->fields[i].event_field->name, subname))
			return i;
	}
	return -1;
}
EXPORT_SYMBOL_GPL(lttng_kernel_get_context_index);

struct lttng_kernel_ctx_field *lttng_kernel_get_context_field_from_index(struct lttng_kernel_ctx *ctx,
		size_t index)
{
	if (index >= ctx->nr_fields)
		return NULL;
	return &ctx->fields[index];
}
EXPORT_SYMBOL_GPL(lttng_kernel_get_context_field_from_index);

/*
 * Note: as we append context information, the pointer location may change.
 * lttng_kernel_context_add_field leaves the new last context initialized to NULL.
 */
static
int lttng_kernel_context_add_field(struct lttng_kernel_ctx **ctx_p)
{
	struct lttng_kernel_ctx *ctx;

	if (!*ctx_p) {
		*ctx_p = kzalloc(sizeof(struct lttng_kernel_ctx), GFP_KERNEL);
		if (!*ctx_p)
			return -ENOMEM;
		(*ctx_p)->largest_align = 1;
	}
	ctx = *ctx_p;
	if (ctx->nr_fields + 1 > ctx->allocated_fields) {
		struct lttng_kernel_ctx_field *new_fields;

		ctx->allocated_fields = max_t(size_t, 1, 2 * ctx->allocated_fields);
		new_fields = lttng_kvzalloc(ctx->allocated_fields * sizeof(*new_fields), GFP_KERNEL);
		if (!new_fields)
			return -ENOMEM;
		if (ctx->fields)
			memcpy(new_fields, ctx->fields, sizeof(*ctx->fields) * ctx->nr_fields);
		lttng_kvfree(ctx->fields);
		ctx->fields = new_fields;
	}
	ctx->nr_fields++;
	return 0;
}

static size_t get_type_max_align(const struct lttng_kernel_type_common *type)
{
	switch (type->type) {
	case lttng_kernel_type_integer:
		return lttng_kernel_get_type_integer(type)->alignment;
	case lttng_kernel_type_string:
		return CHAR_BIT;
	case lttng_kernel_type_enum:
		return get_type_max_align(lttng_kernel_get_type_enum(type)->container_type);
	case lttng_kernel_type_array:
		return max_t(size_t, get_type_max_align(lttng_kernel_get_type_array(type)->elem_type),
				lttng_kernel_get_type_array(type)->alignment);
	case lttng_kernel_type_sequence:
		return max_t(size_t, get_type_max_align(lttng_kernel_get_type_sequence(type)->elem_type),
				lttng_kernel_get_type_sequence(type)->alignment);
	case lttng_kernel_type_struct:
	{
		unsigned int i;
		size_t field_align = 0;
		const struct lttng_kernel_type_struct *struct_type = lttng_kernel_get_type_struct(type);

		for (i = 0; i < struct_type->nr_fields; i++) {
			field_align = max_t(size_t,
				get_type_max_align(struct_type->fields[i]->type),
				field_align);
		}
		return field_align;
	}
	case lttng_kernel_type_variant:
		/* Variants are not accounted in the overall alignment of the type they are embedded in. */
		return 0;
	default:
		WARN_ON_ONCE(1);
		return 0;
	}
}

/*
 * lttng_context_update() should be called at least once between context
 * modification and trace start.
 */
static
void lttng_context_update(struct lttng_kernel_ctx *ctx)
{
	int i;
	size_t largest_align = 8;	/* in bits */

	for (i = 0; i < ctx->nr_fields; i++) {
		size_t field_align = 8;

		field_align = get_type_max_align(ctx->fields[i].event_field->type);
		largest_align = max_t(size_t, largest_align, field_align);
	}
	ctx->largest_align = largest_align >> 3;	/* bits to bytes */
}

int lttng_kernel_context_append(struct lttng_kernel_ctx **ctx_p,
		const struct lttng_kernel_ctx_field *f)
{
	int ret;

	ret = lttng_kernel_context_add_field(ctx_p);
	if (ret)
		return ret;
	(*ctx_p)->fields[(*ctx_p)->nr_fields - 1] = *f;
	lttng_context_update(*ctx_p);
	return 0;
}

void lttng_kernel_context_remove_last(struct lttng_kernel_ctx **ctx_p)
{
	struct lttng_kernel_ctx *ctx = *ctx_p;

	if (!ctx->nr_fields)
		return;
	memset(&ctx->fields[ctx->nr_fields - 1], 0, sizeof(struct lttng_kernel_ctx_field));
	ctx->nr_fields--;
	lttng_context_update(ctx);
}

void lttng_kernel_destroy_context(struct lttng_kernel_ctx *ctx)
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
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_hostname_to_ctx");
	}
	ret = lttng_add_nice_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_nice_to_ctx");
	}
	ret = lttng_add_pid_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_pid_to_ctx");
	}
	ret = lttng_add_ppid_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_ppid_to_ctx");
	}
	ret = lttng_add_prio_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_prio_to_ctx");
	}
	ret = lttng_add_procname_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_procname_to_ctx");
	}
	ret = lttng_add_tid_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_tid_to_ctx");
	}
	ret = lttng_add_vppid_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_vppid_to_ctx");
	}
	ret = lttng_add_vtid_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_vtid_to_ctx");
	}
	ret = lttng_add_vpid_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_vpid_to_ctx");
	}
	ret = lttng_add_cpu_id_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_cpu_id_to_ctx");
	}
	ret = lttng_add_interruptible_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_interruptible_to_ctx");
	}
	ret = lttng_add_need_reschedule_to_ctx(&lttng_static_ctx);
	if (ret) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_need_reschedule_to_ctx");
	}
	ret = lttng_add_preemptible_to_ctx(&lttng_static_ctx);
	if (ret && ret != -ENOSYS) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_preemptible_to_ctx");
	}
	ret = lttng_add_migratable_to_ctx(&lttng_static_ctx);
	if (ret && ret != -ENOSYS) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_migratable_to_ctx");
	}
	ret = lttng_add_cgroup_ns_to_ctx(&lttng_static_ctx);
	if (ret && ret != -ENOSYS) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_cgroup_ns_to_ctx");
	}
	ret = lttng_add_ipc_ns_to_ctx(&lttng_static_ctx);
	if (ret && ret != -ENOSYS) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_ipc_ns_to_ctx");
	}
	ret = lttng_add_mnt_ns_to_ctx(&lttng_static_ctx);
	if (ret && ret != -ENOSYS) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_mnt_ns_to_ctx");
	}
	ret = lttng_add_net_ns_to_ctx(&lttng_static_ctx);
	if (ret && ret != -ENOSYS) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_net_ns_to_ctx");
	}
	ret = lttng_add_pid_ns_to_ctx(&lttng_static_ctx);
	if (ret && ret != -ENOSYS) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_pid_ns_to_ctx");
	}
	ret = lttng_add_user_ns_to_ctx(&lttng_static_ctx);
	if (ret && ret != -ENOSYS) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_user_ns_to_ctx");
	}
	ret = lttng_add_uts_ns_to_ctx(&lttng_static_ctx);
	if (ret && ret != -ENOSYS) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_uts_ns_to_ctx");
	}
	ret = lttng_add_time_ns_to_ctx(&lttng_static_ctx);
	if (ret && ret != -ENOSYS) {
		printk(KERN_WARNING "LTTng: Cannot add context lttng_add_time_ns_to_ctx");
	}
	/* TODO: perf counters for filtering */
	return 0;
}

void lttng_context_exit(void)
{
	lttng_kernel_destroy_context(lttng_static_ctx);
	lttng_static_ctx = NULL;
}
