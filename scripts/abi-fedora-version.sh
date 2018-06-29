#!/bin/sh
# SPDX-License-Identifier: (GPL-2.0 OR LGPL-2.1)

# First argument is the path to the kernel headers.
KPATH="$1"

if [ ! -f "${KPATH}/include/generated/utsrelease.h" ]; then
	echo 0
	exit 0
fi

# Assuming KPATH is the target kernel headers directory
UTS_RELEASE=$(sed -rn 's/^#define UTS_RELEASE "(.*)"/\1/p' "${KPATH}/include/generated/utsrelease.h")
FEDORA_VERSION=$(echo "${UTS_RELEASE}" | sed -n 's/.*\.fc\([0-9]*\)\..*/\1/p')

if [ "x${FEDORA_VERSION}" = "x" ]; then
        echo 0
        exit 0
fi

FEDORA_REVISION_VERSION=$(echo "${UTS_RELEASE}" | sed -n 's/\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\)\-\([0-9]*\).*/\4/p')
echo "${FEDORA_REVISION_VERSION}"
