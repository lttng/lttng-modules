#!/bin/bash

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
echo 'source "lttng/Kconfig"' >> "${KERNEL_DIR}/Kconfig"
sed -i 's#+= kernel/#+= kernel/ lttng/#' "${KERNEL_DIR}/Makefile"

echo >&2
echo "    $0: done." >&2
echo "    $0: now you can build the kernel with LTTng support." >&2
echo "    $0: make sure you enable it (CONFIG_LTTNG) before building." >&2
echo >&2
