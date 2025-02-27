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

#include <linux/mutex.h>
#include <linux/string.h>
#include <linux/err.h>
#include <linux/vmalloc.h>

#include <wrapper/stdarg.h>
#include <lttng/events.h>
#include <lttng/events-internal.h>

static inline
void lttng_metadata_begin(struct lttng_kernel_session *session)
{
	if (atomic_inc_return(&session->priv->metadata_cache->producing) == 1)
		mutex_lock(&session->priv->metadata_cache->lock);
}

static inline
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
static inline
int lttng_metadata_printf(struct lttng_kernel_session *session,
			  const char *fmt, ...)
{
	char *str;
	size_t len;
	va_list ap;

	WARN_ON_ONCE(!LTTNG_READ_ONCE(session->active));

	va_start(ap, fmt);
	str = kvasprintf(GFP_KERNEL, fmt, ap);
	va_end(ap);
	if (!str)
		return -ENOMEM;

	len = strlen(str);
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
	kfree(str);

	return 0;

err:
	kfree(str);
	return -ENOMEM;
}

#endif /* _LTTNG_METADATA_PRINT_H */
