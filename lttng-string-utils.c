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

#include <linux/types.h>

#include <lttng-string-utils.h>

enum star_glob_pattern_type_flags {
	STAR_GLOB_PATTERN_TYPE_FLAG_NONE 	= 0,
	STAR_GLOB_PATTERN_TYPE_FLAG_PATTERN 	= (1U << 0),
	STAR_GLOB_PATTERN_TYPE_FLAG_END_ONLY 	= (1U << 1),
};

static
enum star_glob_pattern_type_flags strutils_test_glob_pattern(const char *pattern)
{
	enum star_glob_pattern_type_flags ret =
		STAR_GLOB_PATTERN_TYPE_FLAG_NONE;
	const char *p;

	for (p = pattern; *p != '\0'; p++) {
		switch (*p) {
		case '*':
			ret = STAR_GLOB_PATTERN_TYPE_FLAG_PATTERN;

			if (p[1] == '\0') {
				ret |= STAR_GLOB_PATTERN_TYPE_FLAG_END_ONLY;
			}
			goto end;
		case '\\':
			p++;

			if (*p == '\0') {
				goto end;
			}
			break;
		default:
			break;
		}
	}

end:
	return ret;
}

/*
 * Returns true if `pattern` is a star-only globbing pattern, that is,
 * it contains at least one non-escaped `*`.
 */
bool strutils_is_star_glob_pattern(const char *pattern)
{
	return strutils_test_glob_pattern(pattern) &
		STAR_GLOB_PATTERN_TYPE_FLAG_PATTERN;
}

/*
 * Returns true if `pattern` is a globbing pattern with a globbing,
 * non-escaped star only at its very end.
 */
bool strutils_is_star_at_the_end_only_glob_pattern(const char *pattern)
{
	return strutils_test_glob_pattern(pattern) &
		STAR_GLOB_PATTERN_TYPE_FLAG_END_ONLY;
}

struct string_with_len {
	const char *str;
	size_t len;
};

static
char string_get_char_at_cb(size_t at, void *data)
{
	struct string_with_len *string_with_len = data;

	if (at >= string_with_len->len) {
		return '\0';
	}

	return string_with_len->str[at];
}

/*
 * Globbing matching function with the star feature only (`?` and
 * character sets are not supported). This matches `candidate` (plain
 * string) against `pattern`. A literal star can be escaped with `\` in
 * `pattern`.
 *
 * `pattern_len` or `candidate_len` can be greater than the actual
 * string length of `pattern` or `candidate` if the string is
 * null-terminated.
 */
bool strutils_star_glob_match(const char *pattern, size_t pattern_len,
		const char *candidate, size_t candidate_len) {
	struct string_with_len pattern_with_len = {
		pattern, pattern_len
	};
	struct string_with_len candidate_with_len = {
		candidate, candidate_len
	};

	return strutils_star_glob_match_char_cb(string_get_char_at_cb,
		&pattern_with_len, string_get_char_at_cb,
		&candidate_with_len);
}

bool strutils_star_glob_match_char_cb(
		strutils_get_char_at_cb pattern_get_char_at_cb,
		void *pattern_get_char_at_cb_data,
		strutils_get_char_at_cb candidate_get_char_at_cb,
		void *candidate_get_char_at_cb_data)
{
	size_t retry_p_at = 0, retry_c_at = 0, c_at, p_at;
	char c, p, prev_p;
	bool got_a_star = false;

retry:
	c_at = retry_c_at;
	c = candidate_get_char_at_cb(c_at, candidate_get_char_at_cb_data);
	p_at = retry_p_at;
	p = pattern_get_char_at_cb(p_at, pattern_get_char_at_cb_data);

	/*
	 * The concept here is to retry a match in the specific case
	 * where we already got a star. The retry position for the
	 * pattern is just after the most recent star, and the retry
	 * position for the candidate is the character following the
	 * last try's first character.
	 *
	 * Example:
	 *
	 *     candidate: hi ev every onyx one
	 *                ^
	 *     pattern:   hi*every*one
	 *                ^
	 *
	 *     candidate: hi ev every onyx one
	 *                 ^
	 *     pattern:   hi*every*one
	 *                 ^
	 *
	 *     candidate: hi ev every onyx one
	 *                  ^
	 *     pattern:   hi*every*one
	 *                  ^
	 *
	 *     candidate: hi ev every onyx one
	 *                  ^
	 *     pattern:   hi*every*one
	 *                   ^ MISMATCH
	 *
	 *     candidate: hi ev every onyx one
	 *                   ^
	 *     pattern:   hi*every*one
	 *                   ^
	 *
	 *     candidate: hi ev every onyx one
	 *                   ^^
	 *     pattern:   hi*every*one
	 *                   ^^
	 *
	 *     candidate: hi ev every onyx one
	 *                   ^ ^
	 *     pattern:   hi*every*one
	 *                   ^ ^ MISMATCH
	 *
	 *     candidate: hi ev every onyx one
	 *                    ^
	 *     pattern:   hi*every*one
	 *                   ^ MISMATCH
	 *
	 *     candidate: hi ev every onyx one
	 *                     ^
	 *     pattern:   hi*every*one
	 *                   ^ MISMATCH
	 *
	 *     candidate: hi ev every onyx one
	 *                      ^
	 *     pattern:   hi*every*one
	 *                   ^
	 *
	 *     candidate: hi ev every onyx one
	 *                      ^^
	 *     pattern:   hi*every*one
	 *                   ^^
	 *
	 *     candidate: hi ev every onyx one
	 *                      ^ ^
	 *     pattern:   hi*every*one
	 *                   ^ ^
	 *
	 *     candidate: hi ev every onyx one
	 *                      ^  ^
	 *     pattern:   hi*every*one
	 *                   ^  ^
	 *
	 *     candidate: hi ev every onyx one
	 *                      ^   ^
	 *     pattern:   hi*every*one
	 *                   ^   ^
	 *
	 *     candidate: hi ev every onyx one
	 *                           ^
	 *     pattern:   hi*every*one
	 *                        ^
	 *
	 *     candidate: hi ev every onyx one
	 *                           ^
	 *     pattern:   hi*every*one
	 *                         ^ MISMATCH
	 *
	 *     candidate: hi ev every onyx one
	 *                            ^
	 *     pattern:   hi*every*one
	 *                         ^
	 *
	 *     candidate: hi ev every onyx one
	 *                            ^^
	 *     pattern:   hi*every*one
	 *                         ^^
	 *
	 *     candidate: hi ev every onyx one
	 *                            ^ ^
	 *     pattern:   hi*every*one
	 *                         ^ ^ MISMATCH
	 *
	 *     candidate: hi ev every onyx one
	 *                             ^
	 *     pattern:   hi*every*one
	 *                         ^ MISMATCH
	 *
	 *     candidate: hi ev every onyx one
	 *                              ^
	 *     pattern:   hi*every*one
	 *                         ^ MISMATCH
	 *
	 *     candidate: hi ev every onyx one
	 *                               ^
	 *     pattern:   hi*every*one
	 *                         ^ MISMATCH
	 *
	 *     candidate: hi ev every onyx one
	 *                                ^
	 *     pattern:   hi*every*one
	 *                         ^ MISMATCH
	 *
	 *     candidate: hi ev every onyx one
	 *                                 ^
	 *     pattern:   hi*every*one
	 *                         ^
	 *
	 *     candidate: hi ev every onyx one
	 *                                 ^^
	 *     pattern:   hi*every*one
	 *                         ^^
	 *
	 *     candidate: hi ev every onyx one
	 *                                 ^ ^
	 *     pattern:   hi*every*one
	 *                         ^ ^
	 *
	 *     candidate: hi ev every onyx one
	 *                                 ^  ^
	 *     pattern:   hi*every*one
	 *                         ^  ^ SUCCESS
	 */
	while (c != '\0') {
		if (p == '\0') {
			goto end_of_pattern;
		}

		switch (p) {
		case '*':
		{
			char retry_p;
			got_a_star = true;

			/*
			 * Our first try starts at the current candidate
			 * character and after the star in the pattern.
			 */
			retry_c_at = c_at;
			retry_p_at = p_at + 1;
			retry_p = pattern_get_char_at_cb(retry_p_at,
				pattern_get_char_at_cb_data);

			if (retry_p == '\0') {
				/*
				 * Star at the end of the pattern at
				 * this point: automatic match.
				 */
				return true;
			}

			goto retry;
		}
		case '\\':
			/* Go to escaped character. */
			p_at++;
			p = pattern_get_char_at_cb(p_at,
				pattern_get_char_at_cb_data);

			/*
			 * Fall through the default case which will
			 * compare the escaped character now.
			 */
		default:
			if (p == '\0' || c != p) {
end_of_pattern:
				/* Character mismatch OR end of pattern. */
				if (!got_a_star) {
					/*
					 * We didn't get any star yet,
					 * so this first mismatch
					 * automatically makes the whole
					 * test fail.
					 */
					return false;
				}

				/*
				 * Next try: next candidate character,
				 * original pattern character (following
				 * the most recent star).
				 */
				retry_c_at++;
				goto retry;
			}
			break;
		}

		/* Next pattern and candidate characters. */
		c_at++;
		c = candidate_get_char_at_cb(c_at,
			candidate_get_char_at_cb_data);
		p_at++;
		p = pattern_get_char_at_cb(p_at, pattern_get_char_at_cb_data);
	}

	/*
	 * We checked every candidate character and we're still in a
	 * success state: the only pattern character allowed to remain
	 * is a star.
	 */
	if (p == '\0') {
		return true;
	}

	prev_p = p;
	p_at++;
	p = pattern_get_char_at_cb(p_at, pattern_get_char_at_cb_data);
	return prev_p == '*' && p == '\0';
}
