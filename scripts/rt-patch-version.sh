#!/bin/sh
# SPDX-License-Identifier: (GPL-2.0-only OR LGPL-2.1-only)
# SPDX-FileCopyrightText: 2016-2018 EfficiOS Inc.

# First argument is the path to the kernel headers.
KPATH="$1"

VERSIONFILE=""

if [ -f "${KPATH}/localversion-rt" ]; then
	VERSIONFILE="${KPATH}/localversion-rt"

elif [ -f "${KPATH}/source/localversion-rt" ]; then
	VERSIONFILE="${KPATH}/source/localversion-rt"
else
	echo 0
	exit 0
fi

RT_PATCH_VERSION=$(sed -rn 's/^-rt([0-9]+)$/\1/p' "${VERSIONFILE}")

if [ "x${RT_PATCH_VERSION}" = "x" ]; then
	echo 0
	exit 0
fi

echo "${RT_PATCH_VERSION}"
