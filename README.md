LTTng-modules
=============

_by [Mathieu Desnoyers](mailto:mathieu.desnoyers@efficios.com)_


LTTng kernel modules are Linux kernel modules which make
[LTTng](http://lttng.org/) kernel tracing possible. They include
essential control modules and many probes which instrument numerous
interesting parts of Linux. LTTng-modules builds against a vanilla or
distribution kernel, with no need for additional patches.

Other notable features:

  - Produces [CTF](http://www.efficios.com/ctf)
    (Common Trace Format) natively.
  - Tracepoints, function tracer, CPU Performance Monitoring Unit (PMU)
    counters, kprobes, and kretprobes support.
  - Have the ability to attach _context_ information to events in the
    trace (e.g., any PMU counter, PID, PPID, TID, command name, etc).
    All the extra information fields to be collected with events are
    optional, specified on a per-tracing-session basis (except for
    timestamp and event ID, which are mandatory).


Building
--------

To build and install LTTng-modules, you will need to have your kernel
headers available (or access to your full kernel source tree), and do:

    make
    sudo make modules_install
    sudo depmod -a

The above commands will build LTTng-modules against your
current kernel. If you need to build LTTng-modules against a custom
kernel, do:

    make KERNELDIR=/path/to/custom/kernel
    sudo make KERNELDIR=/path/to/custom/kernel modules_install
    sudo depmod -a kernel_version


### Kernel built-in support

It is also possible to build these modules as part of a kernel image. Simply
run the [`built-in.sh`](built-in.sh) script with the path to your kernel
source directory as an argument.  It will symlink the lttng-modules directory
in the kernel sources and add an include in the kernel Makefile.

Then configure your kernel as usual and enable the `CONFIG_LTTNG` option.


### Required kernel config options

Make sure your target kernel has the following config options enabled:

  - `CONFIG_MODULES`: loadable module support (not strictly required
     when built into the kernel)
  - `CONFIG_KALLSYMS`: see files in [`wrapper`](wrapper); this is
     necessary until the few required missing symbols are exported to GPL
     modules from mainline
  - `CONFIG_HIGH_RES_TIMERS`: needed for LTTng 2.x clock source
  - `CONFIG_TRACEPOINTS`: kernel tracepoint instrumentation
     (enabled as a side-effect of any of the perf/ftrace/blktrace
     instrumentation features)


### Supported (optional) kernel config options

The following kernel configuration options will affect the features
available from LTTng:

  - `CONFIG_HAVE_SYSCALL_TRACEPOINTS`: system call tracing:

        lttng enable-event -k --syscall
        lttng enable-event -k -a

  - `CONFIG_PERF_EVENTS`: performance counters:

        lttng add-context -t perf:*

  - `CONFIG_EVENT_TRACING`: needed to allow block layer tracing
  - `CONFIG_KPROBES`: dynamic probes:

        lttng enable-event -k --probe ...

  - `CONFIG_KRETPROBES`: dynamic function entry/return probes:

        lttng enable-event -k --function ...

  - `CONFIG_KALLSYMS_ALL`: state dump of mapping between block device
    number and name


Usage
-----

Use [LTTng-tools](https://lttng.org/download) to control the tracer.
The session daemon of LTTng-tools should automatically load the LTTng
kernel modules when needed. Use [Babeltrace](https://lttng.org/babeltrace)
to print traces as a human-readable text log.


Support
-------

Linux kernels >= 2.6.36 are supported.


Notes
-----

### About perf PMU counters support

Each PMU counter has its zero value set when it is attached to a context with
add-context. Therefore, it is normal that the same counters attached to both the
stream context and event context show different values for a given event; what
matters is that they increment at the same rate.
