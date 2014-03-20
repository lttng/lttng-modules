#ifndef _LTTNG_TRACEPOINT_H
#define _LTTNG_TRACEPOINT_H

/*
 * lttng-tracepoint.h
 *
 * LTTng adaptation layer for Linux kernel 3.15+ tracepoints.
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
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

int lttng_tracepoint_probe_register(const char *name, void *probe, void *data);
int lttng_tracepoint_probe_unregister(const char *name, void *probe, void *data);
int lttng_tracepoint_init(void);
void lttng_tracepoint_exit(void);

#endif /* _LTTNG_TRACEPOINT_H */
