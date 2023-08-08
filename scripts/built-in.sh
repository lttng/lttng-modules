#!/bin/bash
# SPDX-License-Identifier: (GPL-2.0-only OR LGPL-2.1-only)

set -e

usage() {
	echo "usage: $0 <kernel source tree>" >&2
	exit 1
}

[ "$#" -eq 1 ] || usage
KERNEL_DIR="$(readlink --canonicalize-existing "$1")"

# Symlink the lttng-modules directory in the kernel source
ln -sf "$(pwd)" "${KERNEL_DIR}/lttng"

# Get kernel version from Makefile
version=$(grep -m 1 VERSION ${KERNEL_DIR}/Makefile | sed 's/^.*= //g')
patchlevel=$(grep -m 1 PATCHLEVEL ${KERNEL_DIR}/Makefile | sed 's/^.*= //g')
kernel_version=${version}.${patchlevel}

# Graft ourself to the kernel build system
echo 'source "lttng/src/Kconfig"' >> "${KERNEL_DIR}/Kconfig"

if awk "BEGIN {exit !(${kernel_version} >= 6.1)}"; then
	echo 'obj-y += lttng/' >> "${KERNEL_DIR}/Kbuild"
else
	sed -i 's#+= kernel/#+= kernel/ lttng/#' "${KERNEL_DIR}/Makefile"
fi

echo >&2
echo "    $0: done." >&2
echo "    $0: now you can build the kernel with LTTng support." >&2
echo "    $0: make sure you enable it (CONFIG_LTTNG) before building." >&2
echo >&2
