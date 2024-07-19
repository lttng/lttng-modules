<!--
SPDX-FileCopyrightText: 2020 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>

SPDX-License-Identifier: CC-BY-SA-4.0
-->

# LTTng modules design

by Mathieu Desnoyers
June 30, 2020

This document covers the high level design of lttng-modules.

LTTng modules is a kernel tracer for the Linux kernel. It can be either
loaded as a set of kernel modules, or built into a Linux kernel.

Here are its key components:

## LTTng modules ABI

  Files:
  - `src/lttng-abi.c`
  - `include/lttng/abi.h`

  This ABI consists of ioctls with code 0xF6. It extensively uses
  anonymous file descriptors to represent the tracer "objects". Only
  root is allowed to interact with those ioctls.


## LTTng session, channels, contexts and events management

  Files:
  - `src/lttng-events.c`
  - `include/lttng/lttng-events.h`

  Current state about configured tracing sessions, channels, contexts
  and events. The session, channel, context and event state is
  manipulated through the LTTng modules ABI. A session contains 0 or
  more channels, through which data is traced. A channel is associated
  with an instance of a lib ring buffer client. Channels have 0 or more
  events, which are associated to kernel instrumentation as event
  sources.


## lib ring buffer

  Generic ring buffer library (kernel implementation). Note, there is
  a very similar copy of this implementation within the lttng-ust
  user-space tracer. The overall goal of this library is to support
  both kernel and user-space tracing.

  Files:
  - `src/lib/ringbuffer/*`
  - `include/ringbuffer/*`

  Those include ring buffer ABI meant for consuming the buffer data
  from user-space. It is implemented in:

  - `src/lib/ringbuffer/ring_buffer_vfs.c` (open, release, poll, ioctl)
  - `src/lib/ringbuffer/ring_buffer_mmap.c` (mmap)
  - `src/lib/ringbuffer/ring_buffer_splice.c` (splice)
  - `include/ringbuffer/vfs.h`: lib ring buffer ioctl commands (code 0xF6).

  The ring buffer library can be configured to be used in various
  use-cases by creating a specialized ring buffer "client" (template).
  `include/ringbuffer/config.h` details the various configuration
  parameters which are supported.


## LTTng modules ring buffer clients

  Files:
  - `src/lttng-ring-buffer-client-discard.c`
  - `src/lttng-ring-buffer-client-mmap-discard.c`
  - `src/lttng-ring-buffer-client-mmap-overwrite.c`
  - `src/lttng-ring-buffer-client-overwrite.c`
  - `src/lttng-ring-buffer-metadata-client.c`
  - `src/lttng-ring-buffer-metadata-mmap-client.c`
  - `src/lttng-ring-buffer-client.h`
  - `src/lttng-ring-buffer-metadata-client.h`

  Those are the users of lib ring buffer, with specialized instances of
  the ring buffer for each use-case supported by LTTng. Those are
  hand-crafted templates in C. The fast-paths are inlined within each
  client, and the slow paths are kept in the common library to minimize
  code memory usage.


## LTTng filter

  The filter in lttng-modules is meant to quickly discard events which
  do not match an expression. The expression parsing is all done in
  userspace within lttng-tools. The filter is received by lttng-modules
  as a bytecode. The frequent case for which a filter is optimized is to
  discard most of the events. The filter operates on input arguments
  received on the stack, before the ring buffer is touched.

  Files:
  - `include/lttng/filter-bytecode.h`: LTTng filter bytecode.
  - `src/lttng-filter-validator.c`: Validation pass on bytecode reception
  - `src/lttng-filter.c`: Filter linker code: link a bytecode onto a given
                        event (knowing its fields offsets).
  - `src/lttng-filter-specialize.c`: Specialize the bytecode, transforming
                                   generic instructions into
                                   type-specific (faster) instructions.
  - `src/lttng-filter-interpreter.c`: Bytecode interpreter, called by
                                    instrumentation to filter events.

## LTTng contexts

  LTTng-modules supports the notion of "contexts" which can be attached either
  to specific events or to all events in a channel. Those are additional
  data which can be saved prior to the event payload, e.g. current
  thread ID, process name, performance counters, and more.

  Files:
  - `src/lttng-context.c`: Context state associated to a channel or event,
                         and helpers.
  - `src/lttng-context-*.c`: Implementation of all supported contexts:
    callstack, cgroup-ns, cpu-id, egid, euid, gid, hostname,
    interruptible, ipc-ns, migratable, mnt-ns, need-reschedule, net-ns,
    nice, perf-counters, pid, pis-ns, ppid, preemptible, prio, procname,
    sgid, suid, tid, uid, user-ns, uts-ns, vegid, veuid, vgid, vpid, vppid,
    vsgid, vtid, vuid.


## LTTng tracepoint instrumentation

  The LTTng tracer attaches "probes" to kernel subsystems. A probe is a
  set of tracepoint callbacks matching the tracepoint instrumentation
  for a kernel subsystem. Each probe can be loaded separately.

  Due to limitations in the kernel `TRACE_EVENT` macros, LTTng
  implements its own `LTTNG_TRACEPOINT_EVENT` macros. It uses the
  upstream kernel `TRACE_EVENT` macros only to validate the prototype
  of its callbacks. Also, LTTng exposes an event field semantic which
  matches what is exposed to user-space through /proc in the traces,
  which requires different field layout implementation than what the
  upstream kernel exposes to user-space.

  Files:
  - `src/lttng-tracepoint.c`: Mapping between tracepoint instrumentation and LTTng
                          events.
  - `src/lttng-probes.c`: LTTng probes registry.
  - `include/instrumentation/events/*`: LTTng tracepoint instrumentation
                                    headers for all kernel subsystems.


## LTTng system call instrumentation

  The LTTng tracer gathers both input and output arguments from each
  system call, for all supported architectures. This means the system
  call probe callbacks read from user-space memory when needed.

  Files:
  - `src/lttng-syscalls.c`: LTTng system call instrumentation callbacks and
                          tables.
  - `include/instrumentation/syscall/*`: generated and override system
                                       call instrumentation headers.


## LTTng statedump

  Dump kernel state at trace start or when an explicit "statedump" is
  requested. Useful to reconstruct the entire kernel state at
  post-processing. Dumps: threads scheduling state, file
  descriptor tables, interrupt handlers, network interfaces, block
  devices, cpu topology. Also performs a "fence" on all CPUs to reach
  a quiescent state on all CPUs before start and end of statedump.

  Files:
  - `src/lttng-statedump-impl.c`


## LTTng tracker

  User ID and Process ID trackers, for filtering of entire sessions
  based on UID, GID, and PID.

  Files:
  - `src/lttng-tracker-id.c`


## LTTng clock

  Clock plugin registration. The clock used by the LTTng modules kernel
  tracer can be overridden by a plugin module.

  Files:
  - `src/lttng-clock.c`
  - `include/lttng/clock.h`
