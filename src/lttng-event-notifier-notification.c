/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-event-notifier-notification.c
 *
 * Copyright (C) 2020 Francis Deslauriers <francis.deslauriers@efficios.com>
 */

#include <lttng/events.h>
#include <lttng/event-notifier-notification.h>

void lttng_event_notifier_notification_send(struct lttng_event_notifier *event_notifier)
{
	struct lttng_event_notifier_group *event_notifier_group = event_notifier->group;
	struct lib_ring_buffer_ctx ctx;
	int ret;

	if (unlikely(!READ_ONCE(event_notifier->enabled)))
		return;

	lib_ring_buffer_ctx_init(&ctx, event_notifier_group->chan, NULL,
			sizeof(event_notifier->user_token),
			lttng_alignof(event_notifier->user_token), -1);
	ret = event_notifier_group->ops->event_reserve(&ctx, 0);
	if (ret < 0) {
		//TODO: error handling with counter maps
		//silently drop for now.
		WARN_ON_ONCE(1);
		return;
	}
	lib_ring_buffer_align_ctx(&ctx, lttng_alignof(event_notifier->user_token));
	event_notifier_group->ops->event_write(&ctx, &event_notifier->user_token,
			sizeof(event_notifier->user_token));
	event_notifier_group->ops->event_commit(&ctx);
	irq_work_queue(&event_notifier_group->wakeup_pending);
}
