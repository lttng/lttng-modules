/* SPDX-License-Identifier: (GPL-2.0 OR LGPL-2.1)
 *
 * lib/ringbuffer/api.h
 *
 * Ring Buffer API.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LIB_RING_BUFFER_API_H
#define _LIB_RING_BUFFER_API_H

#include <wrapper/ringbuffer/backend.h>
#include <wrapper/ringbuffer/frontend.h>
#include <wrapper/ringbuffer/vfs.h>

/*
 * ring_buffer_frontend_api.h contains static inline functions that depend on
 * client static inlines. Hence the inclusion of this >api> header only
 * within the client.
 */
#include <wrapper/ringbuffer/frontend_api.h>

#endif /* _LIB_RING_BUFFER_API_H */
