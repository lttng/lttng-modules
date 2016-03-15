/*
 * lttng-context.c
 *
 * LTTng trace/channel/event context management.
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; only
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
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
		new_fields = kzalloc(ctx->allocated_fields * sizeof(struct lttng_ctx_field), GFP_KERNEL);
		if (!new_fields)
			return NULL;
		if (ctx->fields)
			memcpy(new_fields, ctx->fields, sizeof(*ctx->fields) * ctx->nr_fields);
		kfree(ctx->fields);
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
			field_align = type->u.basic.integer.alignment;
			break;
		case atype_array:
		{
			struct lttng_basic_type *btype;

			btype = &type->u.array.elem_type;
			switch (btype->atype) {
			case atype_integer:
				field_align = btype->u.basic.integer.alignment;
				break;
			case atype_string:
				break;

			case atype_array:
			case atype_sequence:
			default:
				WARN_ON_ONCE(1);
				break;
			}
			break;
		}
		case atype_sequence:
		{
			struct lttng_basic_type *btype;

			btype = &type->u.sequence.length_type;
			switch (btype->atype) {
			case atype_integer:
				field_align = btype->u.basic.integer.alignment;
				break;

			case atype_string:
			case atype_array:
			case atype_sequence:
			default:
				WARN_ON_ONCE(1);
				break;
			}

			btype = &type->u.sequence.elem_type;
			switch (btype->atype) {
			case atype_integer:
				field_align = max_t(size_t,
					field_align,
					btype->u.basic.integer.alignment);
				break;

			case atype_string:
				break;

			case atype_array:
			case atype_sequence:
			default:
				WARN_ON_ONCE(1);
				break;
			}
			break;
		}
		case atype_string:
			break;

		case atype_enum:
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
	kfree(ctx->fields);
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
#if defined(CONFIG_PREEMPT_RT_FULL) || defined(CONFIG_PREEMPT)
	ret = lttng_add_preemptible_to_ctx(&lttng_static_ctx);
	if (ret != -ENOSYS) {
		printk(KERN_WARNING "Cannot add context lttng_add_preemptible_to_ctx");
	}
#endif
#ifdef CONFIG_PREEMPT_RT_FULL
	ret = lttng_add_migratable_to_ctx(&lttng_static_ctx);
	if (ret != -ENOSYS) {
		printk(KERN_WARNING "Cannot add context lttng_add_migratable_to_ctx");
	}
#endif
	/* TODO: perf counters for filtering */
	return 0;
}

void lttng_context_exit(void)
{
	lttng_destroy_context(lttng_static_ctx);
	lttng_static_ctx = NULL;
}
