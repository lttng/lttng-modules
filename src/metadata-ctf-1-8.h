/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * metadata-ctf-1-8.h
 *
 * CTF 1.8 metadata stream serialization.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _METADATA_CTF_1_8_H
#define _METADATA_CTF_1_8_H

#include <lttng/events.h>
#include <lttng/events-internal.h>

/*
 * Output metadata into this session's metadata buffers.
 * Must be called with sessions_mutex held.
 */
int lttng_session_metadata_statedump(struct lttng_kernel_session *session);

/*
 * Must be called with sessions_mutex held.
 * The entire event metadata is printed as a single atomic metadata
 * transaction.
 */
int lttng_event_recorder_metadata_statedump(struct lttng_kernel_event_common *event);

#endif /* _METADATA_CTF_1_8_H */
