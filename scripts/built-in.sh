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

# Graft ourself to the kernel build system
echo 'source "lttng/src/Kconfig"' >> "${KERNEL_DIR}/Kconfig"

# In kernel v6.1, subdirectories were moved to Kbuild
if grep -qE '^obj-y[[:space:]]+\+= kernel/' "${KERNEL_DIR}/Kbuild"; then
	echo 'obj-y += lttng/' >> "${KERNEL_DIR}/Kbuild"
else
	sed -i 's#+= kernel/#+= kernel/ lttng/#' "${KERNEL_DIR}/Makefile"
fi

echo >&2
echo "    $0: done." >&2
echo "    $0: now you can build the kernel with LTTng support." >&2
echo "    $0: make sure you enable it (CONFIG_LTTNG) before building." >&2
echo >&2
