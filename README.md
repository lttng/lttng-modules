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
    (Common Trace Format) natively,
  - Tracepoints, function tracer, CPU Performance Monitoring Unit (PMU)
    counters, kprobes, and kretprobes support,
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
run the [`scripts/built-in.sh`](scripts/built-in.sh) script with the path to
your kernel source directory as an argument.  It will symlink the
lttng-modules directory in the kernel sources and add an include in the kernel
Makefile.

Then configure your kernel as usual and enable the `CONFIG_LTTNG` option.


### Required kernel config options

Make sure your target kernel has the following config options enabled:

  - `CONFIG_MODULES`: loadable module support (not strictly required
     when built into the kernel),
  - `CONFIG_KALLSYMS`: see files in [`wrapper`](wrapper); this is
     necessary until the few required missing symbols are exported to GPL
     modules from mainline,
  - `CONFIG_HIGH_RES_TIMERS`: needed for LTTng 2.x clock source,
  - `CONFIG_TRACEPOINTS`: kernel tracepoint instrumentation
     (enabled as a side-effect of any of the perf/ftrace/blktrace
     instrumentation features).
  - `CONFIG_KPROBES` (5.7+): use kallsyms for kernel 5.7 and newer.


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

### LTTng specific kernel config options

The following kernel configuration options are provided by LTTng:

  - `CONFIG_LTTNG`: Build LTTng (Defaults to 'm').
  - `CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM`: Enable the experimental bitwise
    enumerations (Defaults to 'n'). This can be enabled by building with:

         make CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM=y

  - `CONFIG_LTTNG_CLOCK_PLUGIN_TEST`: Build the test clock plugin (Defaults to
    'm'). This plugin overrides the trace clock and should always be built as a
    module for testing.


Customization/Extension
-----------------------

The lttng-modules source includes definitions for the actual callback
functions that will be attached to the kernel tracepoints by lttng.
The lttng-modules project implements its own macros generating these
callbacks: the LTTNG_TRACEPOINT_EVENT macro family found in
instrumentation/events/lttng-module/. In order to show up in a
lttng-modules trace, a kernel tracepoint must be defined within the
kernel tree, and also defined within lttng-modules with the
LTTNG_TRACEPOINT_EVENT macro family. Customizations or extensions must
be done by modifying instances of these macros within the lttng-modules
source.

Usage
-----

Use [LTTng-tools](https://lttng.org/download) to control the tracer.
The session daemon of LTTng-tools should automatically load the LTTng
kernel modules when needed. Use [Babeltrace](https://lttng.org/babeltrace)
to print traces as a human-readable text log.


Support
-------

Linux kernels >= 3.0 are supported.


Notes
-----

### About perf PMU counters support

Each PMU counter has its zero value set when it is attached to a context with
add-context. Therefore, it is normal that the same counters attached to both the
stream context and event context show different values for a given event; what
matters is that they increment at the same rate.


Contacts
--------

You can contact the maintainers on the following mailing list:
`lttng-dev@lists.lttng.org`.

IRC channel: [#lttng](irc://irc.oftc.net/lttng) on the OFTC network

Bug tracker: [LTTng-modules bug tracker](https://bugs.lttng.org/projects/lttng-modules)

Code review: [_lttng-modules_ project](https://review.lttng.org/q/project:lttng-modules) on LTTng Review

Continuous integration: [LTTng-modules](https://ci.lttng.org/view/LTTng-modules/) on LTTng's CI

GitHub mirror: [lttng/lttng-modules](https://github.com/lttng/lttng-modules)

Patches are principally submitted and reviewed on [LTTng Review](https://review.lttng.org),
but may also be submitted to the [mailing list](mailto:lttng-dev@lists.lttng.org)
with the subject prefix `PATCH lttng-modules` or by pull request on the
[GitHub mirror](https://github.com/lttng/lttng-modules).
