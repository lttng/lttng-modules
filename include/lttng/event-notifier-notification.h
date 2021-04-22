/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng/event-notifier-notification.h
 *
 * Copyright (C) 2020 Francis Deslauriers <francis.deslauriers@efficios.com>
 */

#ifndef _LTTNG_EVENT_NOTIFIER_NOTIFICATION_H
#define _LTTNG_EVENT_NOTIFIER_NOTIFICATION_H

#include <lttng/events.h>

void lttng_event_notifier_notification_send(struct lttng_kernel_event_notifier *event_notifier,
		const char *stack_data,
		struct lttng_probe_ctx *probe_ctx,
		struct lttng_kernel_notification_ctx *notif_ctx);

#endif /* _LTTNG_EVENT_NOTIFIER_NOTIFICATION_H */
