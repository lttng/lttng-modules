<!--
SPDX-FileCopyrightText: 2011-2017 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>

SPDX-License-Identifier: CC-BY-SA-4.0
-->

# Workflow for updating patches from newer kernel:

Pull the new headers from mainline kernel to this subdirectory.
Update those headers to fix the lttng-modules instrumentation style.


# Workflow to add new Tracepoint instrumentation to newer kernel, and add support for it into LTTng:

1. Instrument the kernel with new trace events headers. If you want that
   instrumentation distributed, you will have to push those changes into
   the upstream Linux kernel first,
2. Copy those headers into this subdirectory,
3. Look at a diff from other headers between mainline kernel version and 
   those in this subdirectory, and use that as a recipe to alter the new
   headers copy.
4. Create a new file in `src/probes/` for the new trace event header you added,
5. Add it to `src/probes/Makefile`,
6. Build, `make modules_install`,
7. Don't forget to load that new module too.

Currently, LTTng policy is to only accept headers derived from trace
event headers accepted into the Linux kernel upstream for tracepoints
related to upstream kernel instrumentation.
