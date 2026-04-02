#!/bin/bash
# SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
# SPDX-FileCopyrightText: 2026 EfficiOS Inc.

set -eu

CURRENT_KVER="7.2"
SUPPORTED_ARCHS=(arm powerpc riscv x86)

for arch in "${SUPPORTED_ARCHS[@]}"; do

	./lttng-syscalls-generate-headers.sh pointers $CURRENT_KVER "$arch" 32 ../../include/instrumentation/syscalls/
	./lttng-syscalls-generate-headers.sh integers $CURRENT_KVER "$arch" 32 ../../include/instrumentation/syscalls/

	./lttng-syscalls-generate-headers.sh pointers $CURRENT_KVER "$arch" 64 ../../include/instrumentation/syscalls/
	./lttng-syscalls-generate-headers.sh integers $CURRENT_KVER "$arch" 64 ../../include/instrumentation/syscalls/

done
