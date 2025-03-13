/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-kernel-mj-gen.c
 *
 * Internal metadata JSON generation API.
 *
 * Copyright (C) 2025 Philippe Proulx <pproulx@efficios.com>
 */

#include <linux/string.h>
#include <linux/types.h>

#include <wrapper/stdarg.h>
#include <lttng/events-internal.h>
#include <lttng/lttng-kernel-mj-gen-internal.h>

#include "lttng-metadata-print.h"

/* Initial count of ASCII control characters */
#define ASCII_INIT_CTRL_CH_COUNT	32

/* ASCII code of DEL control character */
#define ASCII_CTRL_CH_DEL		127

/* Integer string formatting buffer size (bytes) */
#define INT_STR_BUF_SIZE		32

/* State flags */
enum {
	/* Initial */
	STATE_FLAG_INIT =	1 << 0,

	/* In array */
	STATE_FLAG_IN_ARRAY =	1 << 1,

	/* In object */
	STATE_FLAG_IN_OBJ =	1 << 2,

	/* In empty (array or object) */
	STATE_FLAG_EMPTY =	1 << 3,
};

/* States */
enum {
	/* Initial */
	STATE_INIT = 			STATE_FLAG_INIT,

	/* In empty array */
	STATE_IN_ARRAY_EMPTY =		STATE_FLAG_IN_ARRAY | STATE_FLAG_EMPTY,

	/* In non-empty array */
	STATE_IN_ARRAY_NON_EMPTY =	STATE_FLAG_IN_ARRAY,

	/* In empty object */
	STATE_IN_OBJ_EMPTY =		STATE_FLAG_IN_OBJ | STATE_FLAG_EMPTY,

	/* In non-empty object */
	STATE_IN_OBJ_NON_EMPTY =	STATE_FLAG_IN_OBJ,
};

static
struct lttng_kernel_mj_gen_private *priv(
		struct lttng_kernel_session * const session)
{
	return &session->priv->mj_gen;
}

void lttng_kernel_mj_gen_init(struct lttng_kernel_session * const session)
{
	priv(session)->states[0] = STATE_INIT;
	priv(session)->state_index = 0;
}

static
int cur_state(const struct lttng_kernel_session * const session)
{
	return session->priv->mj_gen.states[session->priv->mj_gen.state_index];
}

static
void set_cur_state(struct lttng_kernel_session * const session, const int state)
{
	priv(session)->states[priv(session)->state_index] = state;
}

/*
 * Pushes `state` onto the JSON generation stack of `session`.
 *
 * Returns `-ENOSPC` when the stack is already full.
 */
static
int push_state(struct lttng_kernel_session * const session, const int state)
{
	if (priv(session)->state_index >= LTTNG_KERNEL_MJ_GEN_STACK_DEPTH - 1)
		return -ENOSPC;

	priv(session)->state_index++;
	set_cur_state(session, state);
	return 0;
}

/*
 * Pops the JSON generation stack of `session`.
 *
 * Returns `-EINVAL` when the stack is already empty.
 */
static
int pop_state(struct lttng_kernel_session * const session)
{
	if (priv(session)->state_index == 0)
		return -EINVAL;

	priv(session)->state_index--;
	return 0;
}

/*
 * Writes a comma if needed.
 *
 * Returns `-EINVAL` when the current JSON generation state
 * is unexpected.
 */
static
int try_gen_comma(struct lttng_kernel_session * const session)
{
	const int state = cur_state(session);

	if (state == STATE_INIT)
		/* Not relevant in this context */
		return 0;

	if (!(state & STATE_FLAG_EMPTY))
		return lttng_metadata_print(session, ",");

	return 0;
}

/*
 * Removes the `STATE_FLAG_EMPTY` flag from the current JSON generation
 * state of `session`.
 */
static
void remove_empty_state(struct lttng_kernel_session * const session)
{
	set_cur_state(session, cur_state(session) & ~STATE_FLAG_EMPTY);
}

/*
 * Writes `str` between double quotes (`"`), escaping
 * escapable characters.
 *
 * This function scans the first `len` bytes of `str` and calls
 * lttng_metadata_print*() as it goes, writing `\u00xy` escape sequences
 * for any escapable character.
 */
static
int gen_quoted_str(struct lttng_kernel_session * const session,
		const char *str, const size_t len)
{
	static const char * const hex_chs = "0123456789abcdef";
	const char * const str_end = str + len;
	const char *last_beg = str;
	char u_buf[] = "\\u0000";
	int ret;

	/* Initial `"` */
	ret = lttng_metadata_print(session, "\"");
	if (ret)
		return ret;

	/* Write all characters, escapable or not */
	for (; str < str_end; str++) {
		const unsigned char ch = *str;

		if (ch < ASCII_INIT_CTRL_CH_COUNT || ch == ASCII_CTRL_CH_DEL ||
				ch == '"' || ch == '\\') {
			/* Handle escapable character */
			if (str > last_beg) {
				/*
				 * Write current substring without
				 * escapable characters.
				 */
				ret = lttng_metadata_print_len(session,
					last_beg, str - last_beg);
				if (ret)
					return ret;
			}

			/* Write escapable character as `\u00xy` */
			u_buf[4] = hex_chs[ch >> 4];
			u_buf[5] = hex_chs[ch & 0xf];
			ret = lttng_metadata_print(session, u_buf);
			if (ret)
				return ret;

			/* Last beginning is the next character */
			last_beg = str + 1;
		}
	}

	/* Write current substring without escapable characters if needed */
	if (str_end > last_beg) {
		ret = lttng_metadata_print_len(session, last_beg,
			str_end - last_beg);
		if (ret)
			return ret;
	}

	/* Final `"` */
	return lttng_metadata_print(session, "\"");
}

/*
 * Tries to write the JSON object key `key`.
 *
 * Returns `-EINVAL` when the current state is unexpected.
 */
static
int try_gen_key(struct lttng_kernel_session * const session,
		const char * const key)
{
	int ret;

	if (!key) {
		if (cur_state(session) & STATE_FLAG_IN_OBJ)
			return -EINVAL;

		return 0;
	}

	if (!(cur_state(session) & STATE_FLAG_IN_OBJ))
		return -EINVAL;

	ret = gen_quoted_str(session, key, strlen(key));
	if (ret)
		return ret;

	return lttng_metadata_print(session, ":");
}

/*
 * Writes the plain, unquoted word `word` having the key `key` (if not
 * `NULL`) for the session `session`
 */
static
int gen_word(struct lttng_kernel_session * const session,
		const char * const key, const char * const word)
{
	int ret = try_gen_comma(session);

	if (ret)
		return ret;

	ret = try_gen_key(session, key);
	if (ret)
		return ret;

	ret = lttng_metadata_print(session, word);
	if (ret)
		return ret;

	remove_empty_state(session);
	return 0;
}

int lttng_kernel_mj_gen_null(struct lttng_kernel_session * const session,
		const char * const key)
{
	return gen_word(session, key, "null");
}

int lttng_kernel_mj_gen_bool(struct lttng_kernel_session * const session,
		const char * const key, const bool val)
{
	return gen_word(session, key, val ? "true" : "false");
}

int lttng_kernel_mj_gen_uint(struct lttng_kernel_session * const session,
		const char * const key, const uint64_t val)
{
	char buf[INT_STR_BUF_SIZE];
	int ret = try_gen_comma(session);

	if (ret)
		return ret;

	ret = try_gen_key(session, key);
	if (ret)
		return ret;

	ret = snprintf(&buf[0], INT_STR_BUF_SIZE, "%llu", val);
	if (ret < 0)
		return ret;

	ret = lttng_metadata_print(session, buf);
	if (ret)
		return ret;

	remove_empty_state(session);
	return 0;
}

int lttng_kernel_mj_gen_sint(struct lttng_kernel_session * const session,
		const char * const key, const int64_t val)
{
	char buf[INT_STR_BUF_SIZE];
	int ret = try_gen_comma(session);

	if (ret)
		return ret;

	ret = try_gen_key(session, key);
	if (ret)
		return ret;

	ret = snprintf(&buf[0], INT_STR_BUF_SIZE, "%lld", val);
	if (ret < 0)
		return ret;

	ret = lttng_metadata_print(session, buf);
	if (ret)
		return ret;

	remove_empty_state(session);
	return 0;
}

int lttng_kernel_mj_gen_str(struct lttng_kernel_session * const session,
		const char * const key, const char * const val)
{
	int ret = try_gen_comma(session);

	if (ret)
		return ret;

	ret = try_gen_key(session, key);
	if (ret)
		return ret;

	ret = gen_quoted_str(session, val, strlen(val));
	if (ret)
		return ret;

	remove_empty_state(session);
	return 0;
}

int lttng_kernel_mj_gen_raw_va(struct lttng_kernel_session * const session,
		const char * const key, const char * const fmt, va_list ap)
{
	int ret;

	ret = try_gen_comma(session);
	if (ret)
		return ret;

	ret = try_gen_key(session, key);
	if (ret)
		return ret;

	ret = lttng_metadata_vprintf(session, fmt, ap);
	if (ret)
		return ret;

	remove_empty_state(session);
	return 0;
}

int lttng_kernel_mj_gen_raw(struct lttng_kernel_session * const session,
		const char * const key, const char * const fmt, ...)
{
	va_list ap;
	int ret;

	va_start(ap, fmt);
	ret = lttng_kernel_mj_gen_raw_va(session, key, fmt, ap);
	va_end(ap);
	return ret;
}

int lttng_kernel_mj_gen_open_array(struct lttng_kernel_session * const session,
		const char * const key)
{
	int ret = gen_word(session, key, "[");

	if (ret)
		return ret;

	return push_state(session, STATE_IN_ARRAY_EMPTY);
}

int lttng_kernel_mj_gen_close_array(struct lttng_kernel_session * const session)
{
	int ret;

	if (!(cur_state(session) & STATE_FLAG_IN_ARRAY))
		return -EINVAL;

	ret = lttng_metadata_print(session, "]");
	if (ret)
		return ret;

	ret = pop_state(session);
	if (ret)
		return ret;

	remove_empty_state(session);
	return 0;
}

int lttng_kernel_mj_gen_open_obj(struct lttng_kernel_session * const session,
		const char * const key)
{
	int ret = gen_word(session, key, "{");

	if (ret)
		return ret;

	return push_state(session, STATE_IN_OBJ_EMPTY);
}

int lttng_kernel_mj_gen_close_obj(struct lttng_kernel_session * const session)
{
	int ret;

	if (!(cur_state(session) & STATE_FLAG_IN_OBJ))
		return -EINVAL;

	ret = lttng_metadata_print(session, "}");
	if (ret)
		return ret;

	ret = pop_state(session);
	if (ret)
		return ret;

	remove_empty_state(session);
	return 0;
}
