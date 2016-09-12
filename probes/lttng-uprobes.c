/*
 * probes/lttng-uprobes.c
 *
 * LTTng uprobes integration module.
 *
 * Copyright (C) 2013 Yannick Brosseau <yannick.brosseau@gmail.com>
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
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

#include <linux/fdtable.h>
#include <linux/module.h>
#include <linux/namei.h>
#include <linux/slab.h>
#include <lttng-events.h>
#include <lttng-tracer.h>
#include <wrapper/irqflags.h>
#include <wrapper/ringbuffer/frontend_types.h>
#include <wrapper/uprobes.h>
#include <wrapper/vmalloc.h>

static
int lttng_uprobes_handler_pre(struct uprobe_consumer *uc, struct pt_regs *regs)
{
	struct lttng_event *event =
		container_of(uc, struct lttng_event, u.uprobe.up_consumer);
	struct lttng_probe_ctx lttng_probe_ctx = {
		.event = event,
		.interruptible = !lttng_regs_irqs_disabled(regs),
	};
	struct lttng_channel *chan = event->chan;
	struct lib_ring_buffer_ctx ctx;
	int ret;

	struct {
		unsigned long ip;
	 } payload;

	if (unlikely(!ACCESS_ONCE(chan->session->active)))
		return 0;
	if (unlikely(!ACCESS_ONCE(chan->enabled)))
		return 0;
	if (unlikely(!ACCESS_ONCE(event->enabled)))
		return 0;

	lib_ring_buffer_ctx_init(&ctx, chan->chan, &lttng_probe_ctx,
		sizeof(payload), lttng_alignof(payload), -1);

	ret = chan->ops->event_reserve(&ctx, event->id);
	if (ret < 0)
		return 0;

	/* Event payload. */
	payload.ip = regs->ip;
	lib_ring_buffer_align_ctx(&ctx, lttng_alignof(payload));
	chan->ops->event_write(&ctx, &payload, sizeof(payload));
	chan->ops->event_commit(&ctx);
	return 0;
}

/*
 * Create event description.
 */
static
int lttng_create_uprobe_event(const char *name, struct lttng_event *event)
{
	struct lttng_event_desc *desc;
	struct lttng_event_field *fields;
	int ret;

	desc = kzalloc(sizeof(*event->desc), GFP_KERNEL);
	if (!desc)
		return -ENOMEM;
	desc->name = kstrdup(name, GFP_KERNEL);
	if (!desc->name) {
		ret = -ENOMEM;
		goto error_str;
	}

	desc->nr_fields = 1;
	desc->fields = fields =
		kzalloc(1 * sizeof(struct lttng_event_field), GFP_KERNEL);

	if (!desc->fields) {
		ret = -ENOMEM;
		goto error_fields;
	}
	fields[0].name = "ip";
	fields[0].type.atype = atype_integer;
	fields[0].type.u.basic.integer.size = sizeof(unsigned long) * CHAR_BIT;
	fields[0].type.u.basic.integer.alignment = lttng_alignof(unsigned long) * CHAR_BIT;
	fields[0].type.u.basic.integer.signedness = lttng_is_signed_type(unsigned long);
	fields[0].type.u.basic.integer.reverse_byte_order = 0;
	fields[0].type.u.basic.integer.base = 16;
	fields[0].type.u.basic.integer.encoding = lttng_encode_none;

	desc->owner = THIS_MODULE;
	event->desc = desc;

	return 0;

error_fields:
	kfree(desc->name);
error_str:
	kfree(desc);
	return ret;
}

int lttng_uprobes_register(const char *name,
			   const char *path_name,
			   uint64_t offset,
			   struct lttng_event *event)
{
	int ret;

	/* Shoudl we fail if the path is empty, it should be checked before */
	if (path_name[0] == '\0')
		path_name = NULL;

	ret = lttng_create_uprobe_event(name, event);
	if (ret)
		goto error;

	memset(&event->u.uprobe.up_consumer, 0,
	       sizeof(event->u.uprobe.up_consumer));

	event->u.uprobe.up_consumer.handler = lttng_uprobes_handler_pre;
	if (path_name) {
		struct path path;
		ret = kern_path(path_name, LOOKUP_FOLLOW, &path);
		if (ret)
			goto path_error;

		event->u.uprobe.inode = igrab(path.dentry->d_inode);
	}
	event->u.uprobe.offset = offset;

	 /* Ensure the memory we just allocated don't trigger page faults. */
	wrapper_vmalloc_sync_all();
	printk(KERN_WARNING "Registering probe on inode %lu and offset %llu\n", event->u.uprobe.inode->i_ino, event->u.uprobe.offset);
	ret = wrapper_uprobe_register(event->u.uprobe.inode,
			event->u.uprobe.offset,
			&event->u.uprobe.up_consumer);
	if (ret) {
		printk(KERN_WARNING "Error registering probe on inode %lu "
		       "and offset %llu\n", event->u.uprobe.inode->i_ino,
		       event->u.uprobe.offset);
		goto register_error;
	}
	return 0;

register_error:
	iput(event->u.uprobe.inode);
path_error:
	kfree(event->desc->name);
	kfree(event->desc);
error:
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_uprobes_register);

void lttng_uprobes_unregister(struct lttng_event *event)
{
	wrapper_uprobe_unregister(event->u.uprobe.inode,
			event->u.uprobe.offset,
			&event->u.uprobe.up_consumer);
}
EXPORT_SYMBOL_GPL(lttng_uprobes_unregister);

void lttng_uprobes_destroy_private(struct lttng_event *event)
{
	iput(event->u.uprobe.inode);
	kfree(event->desc->name);
	kfree(event->desc);
}
EXPORT_SYMBOL_GPL(lttng_uprobes_destroy_private);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Yannick Brosseau");
MODULE_DESCRIPTION("Linux Trace Toolkit Uprobes Support");
