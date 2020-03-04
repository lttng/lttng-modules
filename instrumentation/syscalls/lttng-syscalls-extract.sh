#!/bin/bash
# SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)

set -eu

outfile="${1:-}"

if [ "x$outfile" = "x" ]; then
	echo "Specify an output file as first argument, it will be overwritten."
	exit 1
fi

cd lttng-syscalls-extractor || exit 1
make
cd - || exit 1

# Generate a random string to use as an identifier
ident=$(tr -dc 'a-zA-Z0-9' < /dev/urandom | fold -w 8 | head -n1)

sudo insmod ./lttng-syscalls-extractor/lttng-syscalls-extractor.ko ident="$ident" || true

sudo dmesg | sed -n -e 's/\(\[.*\] \)\?'"$ident"'//p' > "$outfile"

# Make sure we have both the start and end markers
if grep -q -- '---START---' "$outfile"; then
	sed -i '/^---START---/d' "$outfile"
else
	echo "Error: Start marker missing from dmesg output, your kernel log buffer is probably too small, set CONFIG_LOG_BUF_SHIFT to a bigger value."
	exit 1
fi

if grep -q -- '---END---' "$outfile"; then
	sed -i '/^---END---/d' "$outfile"
else
	echo "Error: End marker missing from dmesg output, something went wrong."
	exit 1
fi
