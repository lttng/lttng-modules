#!/bin/sh
# SPDX-License-Identifier: (GPL-2.0 OR LGPL-2.1)

# First argument is the path to the lttng modules sources.
TOP_LTTNG_MODULES_DIR="$1"

EXTRA_VERSION_NAME="$(sed -n '1p' "${TOP_LTTNG_MODULES_DIR}/extra_version/name" 2> /dev/null)"

if [ "x${EXTRA_VERSION_NAME}" != "x" ]; then
	echo "${EXTRA_VERSION_NAME}"
else
	echo "0"
fi
