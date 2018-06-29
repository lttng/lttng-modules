/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1) */
#ifndef _LTTNG_STRING_UTILS_H
#define _LTTNG_STRING_UTILS_H

/*
 * Copyright (C) 2017 Philippe Proulx <pproulx@efficios.com>
 */

#include <stdbool.h>

typedef char (*strutils_get_char_at_cb)(size_t, void *);

bool strutils_is_star_glob_pattern(const char *pattern);
bool strutils_is_star_at_the_end_only_glob_pattern(const char *pattern);
bool strutils_star_glob_match(const char *pattern, size_t pattern_len,
		const char *candidate, size_t candidate_len);
bool strutils_star_glob_match_char_cb(
		strutils_get_char_at_cb pattern_get_char_at_cb,
		void *pattern_get_char_at_cb_data,
		strutils_get_char_at_cb candidate_get_char_at_cb,
		void *candidate_get_char_at_cb_data);

#endif /* _LTTNG_STRING_UTILS_H */
