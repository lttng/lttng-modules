/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng/event-notifier-notification.h
 *
 * Copyright (C) 2020 Francis Deslauriers <francis.deslauriers@efficios.com>
 */

#ifndef _LTTNG_EVENT_NOTIFIER_NOTIFICATION_H
#define _LTTNG_EVENT_NOTIFIER_NOTIFICATION_H

#include <lttng/events.h>

void lttng_event_notifier_notification_send(struct lttng_event_notifier *event_notifier,
		struct lttng_probe_ctx *lttng_probe_ctx,
		const char *stack_data);

#endif /* _LTTNG_EVENT_NOTIFIER_NOTIFICATION_H */
