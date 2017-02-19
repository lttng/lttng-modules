#ifndef _LTTNG_STRING_UTILS_H
#define _LTTNG_STRING_UTILS_H

/*
 * Copyright (C) 2017 Philippe Proulx <pproulx@efficios.com>
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
