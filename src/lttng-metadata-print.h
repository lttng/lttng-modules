/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-metadata-print.h
 *
 * Metadata cache printing utilities.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_METADATA_PRINT_H
#define _LTTNG_METADATA_PRINT_H

#include <wrapper/stdarg.h>
#include <lttng/events.h>
#include <lttng/events-internal.h>

void lttng_metadata_begin(struct lttng_kernel_session *session);
void lttng_metadata_end(struct lttng_kernel_session *session);
int lttng_metadata_print_len(struct lttng_kernel_session *session,
			  const char *str, size_t len);
int lttng_metadata_print(struct lttng_kernel_session *session,
			  const char *str);
int lttng_metadata_vprintf(struct lttng_kernel_session *session,
			  const char *fmt, va_list ap);
int lttng_metadata_printf(struct lttng_kernel_session *session,
			  const char *fmt, ...);

#endif /* _LTTNG_METADATA_PRINT_H */
