/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-metadata-print.c
 *
 * Metadata cache printing utilities.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/mutex.h>
#include <linux/string.h>
#include <linux/err.h>
#include <linux/vmalloc.h>

#include <wrapper/stdarg.h>
#include <lttng/events.h>
#include <lttng/events-internal.h>
#include "lttng-metadata-print.h"

void lttng_metadata_begin(struct lttng_kernel_session *session)
{
	if (atomic_inc_return(&session->priv->metadata_cache->producing) == 1)
		mutex_lock(&session->priv->metadata_cache->lock);
}

void lttng_metadata_end(struct lttng_kernel_session *session)
{
	WARN_ON_ONCE(!atomic_read(&session->priv->metadata_cache->producing));
	if (atomic_dec_return(&session->priv->metadata_cache->producing) == 0) {
		struct lttng_metadata_stream *stream;

		list_for_each_entry(stream, &session->priv->metadata_cache->metadata_stream, node)
			wake_up_interruptible(&stream->read_wait);
		mutex_unlock(&session->priv->metadata_cache->lock);
	}
}

/*
 * Write the metadata to the metadata cache.
 * Must be called with sessions_mutex held.
 * The metadata cache lock protects us from concurrent read access from
 * thread outputting metadata content to ring buffer.
 * The content of the printf is printed as a single atomic metadata
 * transaction.
 */
int lttng_metadata_print_len(struct lttng_kernel_session *session,
			  const char *str, size_t len)
{
	WARN_ON_ONCE(!LTTNG_READ_ONCE(session->active));
	WARN_ON_ONCE(!atomic_read(&session->priv->metadata_cache->producing));
	if (session->priv->metadata_cache->metadata_written + len >
			session->priv->metadata_cache->cache_alloc) {
		char *tmp_cache_realloc;
		unsigned int tmp_cache_alloc_size;

		tmp_cache_alloc_size = max_t(unsigned int,
				session->priv->metadata_cache->cache_alloc + len,
				session->priv->metadata_cache->cache_alloc << 1);
		tmp_cache_realloc = vzalloc(tmp_cache_alloc_size);
		if (!tmp_cache_realloc)
			goto err;
		if (session->priv->metadata_cache->data) {
			memcpy(tmp_cache_realloc,
				session->priv->metadata_cache->data,
				session->priv->metadata_cache->cache_alloc);
			vfree(session->priv->metadata_cache->data);
		}

		session->priv->metadata_cache->cache_alloc = tmp_cache_alloc_size;
		session->priv->metadata_cache->data = tmp_cache_realloc;
	}
	memcpy(session->priv->metadata_cache->data +
			session->priv->metadata_cache->metadata_written,
			str, len);
	session->priv->metadata_cache->metadata_written += len;

	return 0;

err:
	return -ENOMEM;
}

/*
 * Like lttng_metadata_print_len(), but `str` is null-terminated and
 * this function computes its length.
 */
int lttng_metadata_print(struct lttng_kernel_session *session,
			  const char *str)
{
	return lttng_metadata_print_len(session, str, strlen(str));
}

/*
 * Like lttng_metadata_print(), but also formats.
 */
int lttng_metadata_vprintf(struct lttng_kernel_session *session,
			  const char *fmt, va_list ap)
{
	char *str;
	int ret;

	str = kvasprintf(GFP_KERNEL, fmt, ap);
	if (!str) {
		ret = -ENOMEM;
		goto end;
	}

	ret = lttng_metadata_print(session, str);

end:
	kfree(str);
	return ret;
}

/*
 * lttng_metadata_vprintf() with a variable number of arguments.
 */
int lttng_metadata_printf(struct lttng_kernel_session *session,
			  const char *fmt, ...)
{
	int ret;
	va_list ap;

	va_start(ap, fmt);
	ret = lttng_metadata_vprintf(session, fmt, ap);
	va_end(ap);
	return ret;
}
