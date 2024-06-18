#!/bin/sh
# SPDX-License-Identifier: (GPL-2.0-only OR LGPL-2.1-only)
# SPDX-FileCopyrightText: 2018-2020 EfficiOS Inc.

# First argument is the path to the lttng modules sources.
TOP_LTTNG_MODULES_DIR="$1"

INCLUDE_DIR="include/generated"

if ! test -d "${TOP_LTTNG_MODULES_DIR}"; then
	exit 1
fi

TMPFILE="$(mktemp)"

find "${TOP_LTTNG_MODULES_DIR}/extra_version/patches/" -maxdepth 1 ! -name 'README.md' -type f -printf '%f\n' | sort -r \
	| sed -E 's/[^a-zA-Z0-9 \.]/-/g ; s/(.*)/MODULE_INFO(extra_version_patch, "\1");/g' >"${TMPFILE}" 2>/dev/null

if test ! -d "${TOP_LTTNG_MODULES_DIR}/${INCLUDE_DIR}"; then
	mkdir "${TOP_LTTNG_MODULES_DIR}/${INCLUDE_DIR}"
fi

if test ! -f "${TOP_LTTNG_MODULES_DIR}/${INCLUDE_DIR}/patches.h" || \
	test x"$(cat "${TMPFILE}")" != x"$(cat "${TOP_LTTNG_MODULES_DIR}/${INCLUDE_DIR}/patches.h")"; then
	mv "${TMPFILE}" "${TOP_LTTNG_MODULES_DIR}/${INCLUDE_DIR}/patches.h"
fi

rm -f "${TMPFILE}"
