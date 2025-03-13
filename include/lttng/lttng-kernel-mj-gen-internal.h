/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-kernel-mj-gen-internal.h
 *
 * Internal metadata JSON generation API.
 *
 * Copyright (C) 2025 Philippe Proulx <pproulx@efficios.com>
 */

#ifndef _LTTNG_KERNEL_MJ_GEN_INTERNAL_H
#define _LTTNG_KERNEL_MJ_GEN_INTERNAL_H

#include <linux/types.h>
#include <linux/stdarg.h>

#define LTTNG_KERNEL_MJ_GEN_STACK_DEPTH	128

struct lttng_kernel_mj_gen_private {
	int states[LTTNG_KERNEL_MJ_GEN_STACK_DEPTH];
	unsigned int state_index;
};

/*
 * Initializes the JSON metadata generation system for the
 * session `session`.
 *
 * Finalization isn't necessary because `lttng_kernel_mj_gen_private`
 * has no dynamic allocation.
 */
void lttng_kernel_mj_gen_init(struct lttng_kernel_session *session);

/*
 * Generates a metadata JSON null having the key `key` (if not
 * `NULL`) for the session `session`.
 */
int lttng_kernel_mj_gen_null(struct lttng_kernel_session *session,
		const char *key);

/*
 * Generates a metadata JSON boolean for `val` having the key
 * `key` (if not `NULL`) for the session `session`.
 */
int lttng_kernel_mj_gen_bool(struct lttng_kernel_session *session,
		const char *key, bool val);

/*
 * Generates a metadata JSON number for `val` having the key `key` (if
 * not `NULL`) for the session `session`.
 */
int lttng_kernel_mj_gen_uint(struct lttng_kernel_session *session,
		const char *key, uint64_t val);

/*
 * Generates a metadata JSON number for `val` having the key `key` (if
 * not `NULL`) for the session `session`.
 */
int lttng_kernel_mj_gen_sint(struct lttng_kernel_session *session,
		const char *key, int64_t val);

/*
 * Generates a metadata JSON string for `val` having the key `key` (if
 * not `NULL`) for the session `session`.
 *
 * This function assumes `val` is a valid UTF-8 string and deals with
 * escape sequence generation.
 */
int lttng_kernel_mj_gen_str(struct lttng_kernel_session *session,
		const char *key, const char *val);

/*
 * Formats a string from `fmt` and `ap`, and then writes it as a raw
 * metadata JSON value, with the key `key` (if not `NULL`), for the
 * session `session`.
 *
 * This function doesn't sanitize or validate the formatted string in
 * any way: use with caution to avoid JSON corruption.
 */
int lttng_kernel_mj_gen_raw_va(struct lttng_kernel_session *session,
		const char *key, const char *fmt, va_list ap);

/*
 * Calls lttng_kernel_mj_gen_raw_va(), making variable arguments
 * with `fmt` and `...`.
 */
int lttng_kernel_mj_gen_raw(struct lttng_kernel_session *session,
		const char *key, const char *fmt, ...);

/*
 * Generates a metadata JSON array opening having the key `key` (if
 * not `NULL`) for the session `session`.
 */
int lttng_kernel_mj_gen_open_array(struct lttng_kernel_session *session,
		const char *key);

/*
 * Generates a metadata JSON array closing for the session `session`.
 */
int lttng_kernel_mj_gen_close_array(struct lttng_kernel_session *session);

/*
 * Generates a metadata JSON object opening having the key `key` (if
 * not `NULL`) for the session `session`.
 */
int lttng_kernel_mj_gen_open_obj(struct lttng_kernel_session *session,
		const char *key);

/*
 * Generates a metadata JSON array closing for the session `session`.
 */
int lttng_kernel_mj_gen_close_obj(struct lttng_kernel_session *session);

#endif /* _LTTNG_KERNEL_MJ_GEN_INTERNAL_H */
