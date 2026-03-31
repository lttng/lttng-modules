
<!--
SPDX-FileCopyrightText: 2011-2022 EfficiOS Inc.

SPDX-License-Identifier: CC-BY-SA-4.0
-->

# LTTng system call tracing

## lttng-syscall-extractor

You need to build a kernel with `CONFIG_FTRACE_SYSCALLS=y`,
`CONFIG_KALLSYMS_ALL=y` and `CONFIG_DEBUG_FS=y` for extraction. Apply the
linker patch to get your kernel to keep the system call metadata after boot.
Then build and load the LTTng syscall extractor module. If your distribution
doesn't mount the debug filesystem by default you can do it manually:

    mount -t debugfs none /sys/kernel/debug

Then read the content of `/sys/kernel/debug/lttng/syscalls-extractor`.

## Generate system call TRACE_EVENT().

Take the data and feed it to `lttng-syscalls-generate-headers.sh` from the
tools/syscalls directory. See the script header for usage example. It should be
run for both the integers and pointers types.

After these are created, you just need to follow the new system call additions,
no need to regenerate the whole thing, since system calls are only appended to.

## Override headers

You need to provide override headers (even if they don't contain
overrides) for each architecture. Those are meant to contain manual
overrides for specific system calls for cases where the script do not
generate the correct typing for specific system calls.

## Integrating the new architecture, updating architecture header

Edit `instrumentation/syscalls/syscalls_integers.h` and
`instrumentation/syscalls/syscalls_pointers.h`. Add the new architecture (e.g.
CONFIG_X86_64) to the newly-generated headers (e.g. changing the current
`x86-64-syscalls_<CLASS>.h` into `x86-64-syscalls_<CLASS>.h`), then rebuild
lttng-modules.
