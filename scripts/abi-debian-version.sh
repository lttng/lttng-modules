#!/bin/sh
# SPDX-License-Identifier: (GPL-2.0-only OR LGPL-2.1-only)
# SPDX-FileCopyrightText: 2015-2024 EfficiOS Inc.

# First argument is the path to the kernel headers.
KPATH="$1"

if [ ! -f "${KPATH}/include/generated/package.h" ]; then
	echo 0
	exit 0
fi

# Debian snippet courtesy of Ben Hutchings

# Assuming KPATH is the target kernel headers directory
DEB_PACKAGE_VERSION=$(sed -rn 's/^#define LINUX_PACKAGE_ID " Debian (.*)"/\1/p' "${KPATH}/include/generated/package.h")

# Ignore backports part
DEB_PACKAGE_VERSION=$(echo "${DEB_PACKAGE_VERSION}" | sed -r 's/~(bpo|deb).*//')

# Get package revision
DEB_PACKAGE_REVISION=$(echo "${DEB_PACKAGE_VERSION}" | sed -r 's/.*-([^-]+)$/\1/')
# Get non-sec update number
DEB_PACKAGE_REVISION_BASE=$(echo "${DEB_PACKAGE_REVISION}" | sed -r 's/^([0-9]+).*/\1/')
# Get security update number, if present
DEB_PACKAGE_REVISION_SECURITY=$(echo "${DEB_PACKAGE_REVISION}" | sed -rn 's/.*\+(squeeze|deb[0-9]+)+u([0-9]+)$/\2/p')
test -n "${DEB_PACKAGE_REVISION_SECURITY}" || DEB_PACKAGE_REVISION_SECURITY=0
# Combine all update numbers into one
DEB_API_VERSION=$((DEB_PACKAGE_REVISION_BASE * 100 + DEB_PACKAGE_REVISION_SECURITY))

echo ${DEB_API_VERSION}
