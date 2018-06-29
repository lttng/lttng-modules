#!/bin/sh
# SPDX-License-Identifier: (GPL-2.0 OR LGPL-2.1)

# First argument is the path to the lttng modules sources.
TOP_LTTNG_MODULES_DIR="$1"

if ! test -d "${TOP_LTTNG_MODULES_DIR}"; then
	exit 1
fi

TMPFILE="$(mktemp)"

find "${TOP_LTTNG_MODULES_DIR}/extra_version/patches/" -maxdepth 1 ! -name 'README' -type f -printf '%f\n' | sort -r \
	| sed -E 's/[^a-zA-Z0-9 \.]/-/g ; s/(.*)/MODULE_INFO(extra_version_patch, "\1");/g' >"${TMPFILE}" 2>/dev/null

if test ! -f "${TOP_LTTNG_MODULES_DIR}/extra_version/patches.i" || \
	test x"$(cat "${TMPFILE}")" != x"$(cat "${TOP_LTTNG_MODULES_DIR}/extra_version/patches.i")"; then
	mv "${TMPFILE}" "${TOP_LTTNG_MODULES_DIR}/extra_version/patches.i"
fi

rm -f "${TMPFILE}"
