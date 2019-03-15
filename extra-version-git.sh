#!/bin/sh
# SPDX-License-Identifier: (GPL-2.0 OR LGPL-2.1)

# First argument is the path to the lttng modules sources.
TOP_LTTNG_MODULES_DIR="$1"

GIT_VERSION="0"

if test -x "$(which git 2>&1;true)" && test -r "${TOP_LTTNG_MODULES_DIR}/.git"; then
	GIT_VERSION_STR="$(cd "${TOP_LTTNG_MODULES_DIR}" && git describe --tags --dirty 2> /dev/null)"
	GIT_CURRENT_TAG="$(cd "${TOP_LTTNG_MODULES_DIR}" && git describe --tags --exact-match --match="v[0-9]*" HEAD 2> /dev/null)"

	GIT_VERSION="${GIT_VERSION_STR}"

	if ! echo "${GIT_VERSION_STR}" | grep -- "-dirty" >/dev/null && test "x$GIT_CURRENT_TAG" != "x"; then
		GIT_VERSION="0"
	fi
fi

echo "${GIT_VERSION}"
