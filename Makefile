# SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)

ifneq ($(KERNELRELEASE),)

  # This part of the Makefile is used when called by the kernel build system
  # and defines the modules to be built.

  ifdef CONFIG_LOCALVERSION	# Check if dot-config is included.
    ifeq ($(CONFIG_TRACEPOINTS),)
      $(error The option CONFIG_TRACEPOINTS needs to be enabled in your kernel configuration)
    endif # CONFIG_TRACEPOINTS
  endif # ifdef CONFIG_LOCALVERSION

  TOP_LTTNG_MODULES_DIR := $(shell dirname $(lastword $(MAKEFILE_LIST)))

  lttng_check_linux_version = $(shell pwd)/include/linux/version.h
  lttng_check_generated_linux_version = $(shell pwd)/include/generated/uapi/linux/version.h

  #
  # Check for stale version.h, which can be a leftover from an old Linux
  # kernel tree moved to a newer kernel version, only pruned by make
  # distclean.
  #
  ifneq ($(wildcard $(lttng_check_linux_version)),)
    ifneq ($(wildcard $(lttng_check_generated_linux_version)),)
      $(error Duplicate version.h files found in $(lttng_check_linux_version) and $(lttng_check_generated_linux_version). Consider running make distclean on your kernel, or removing the stale $(lttng_check_linux_version) file)
    endif
  endif

  include $(TOP_LTTNG_MODULES_DIR)/Kbuild.common

  ccflags-y += -I$(TOP_LTTNG_MODULES_DIR)

  obj-$(CONFIG_LTTNG) += lttng-ring-buffer-client-discard.o
  obj-$(CONFIG_LTTNG) += lttng-ring-buffer-client-overwrite.o
  obj-$(CONFIG_LTTNG) += lttng-ring-buffer-metadata-client.o
  obj-$(CONFIG_LTTNG) += lttng-ring-buffer-client-mmap-discard.o
  obj-$(CONFIG_LTTNG) += lttng-ring-buffer-client-mmap-overwrite.o
  obj-$(CONFIG_LTTNG) += lttng-ring-buffer-metadata-mmap-client.o
  obj-$(CONFIG_LTTNG) += lttng-clock.o

  obj-$(CONFIG_LTTNG) += lttng-tracer.o

  lttng-tracer-objs := lttng-events.o lttng-abi.o lttng-string-utils.o \
                       lttng-probes.o lttng-context.o \
                       lttng-context-pid.o lttng-context-procname.o \
                       lttng-context-prio.o lttng-context-nice.o \
                       lttng-context-vpid.o lttng-context-tid.o \
                       lttng-context-vtid.o lttng-context-ppid.o \
                       lttng-context-vppid.o lttng-context-cpu-id.o \
                       lttng-context-interruptible.o \
                       lttng-context-need-reschedule.o \
                       lttng-context-callstack.o lttng-calibrate.o \
                       lttng-context-hostname.o wrapper/random.o \
                       probes/lttng.o wrapper/trace-clock.o \
                       wrapper/page_alloc.o \
                       lttng-tracker-pid.o \
                       lttng-filter.o lttng-filter-interpreter.o \
                       lttng-filter-specialize.o \
                       lttng-filter-validator.o \
                       probes/lttng-probe-user.o \
                       lttng-tp-mempool.o

  ifneq ($(CONFIG_HAVE_SYSCALL_TRACEPOINTS),)
    lttng-tracer-objs += lttng-syscalls.o
  endif # CONFIG_HAVE_SYSCALL_TRACEPOINTS

  ifneq ($(CONFIG_PERF_EVENTS),)
    lttng-tracer-objs += lttng-context-perf-counters.o
  endif # CONFIG_PERF_EVENTS

  ifneq ($(CONFIG_PREEMPT_RT_FULL),)
    lttng-tracer-objs += lttng-context-migratable.o
    lttng-tracer-objs += lttng-context-preemptible.o
  endif # CONFIG_PREEMPT_RT_FULL

  ifneq ($(CONFIG_PREEMPT),)
    lttng-tracer-objs += lttng-context-preemptible.o
  endif

  lttng-tracer-objs += $(shell \
    if [ $(VERSION) -ge 4 \
      -o \( $(VERSION) -eq 3 -a $(PATCHLEVEL) -ge 15 \) ] ; then \
      echo "lttng-tracepoint.o" ; fi;)

  ifneq ($(CONFIG_KALLSYMS),)
    obj-$(CONFIG_LTTNG) += lttng-kallsyms.o
  endif

  obj-$(CONFIG_LTTNG) += lttng-statedump.o
  lttng-statedump-objs := lttng-statedump-impl.o wrapper/irqdesc.o \
                          wrapper/fdtable.o

  obj-$(CONFIG_LTTNG) += probes/
  obj-$(CONFIG_LTTNG) += lib/
  obj-$(CONFIG_LTTNG) += tests/

else # KERNELRELEASE

# This part of the Makefile is used when the 'make' command is runned in the
# base directory of the lttng-modules sources. It sets some environment and
# calls the kernel build system to build the actual modules.

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
CFLAGS = $(EXTCFLAGS)

default: modules

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) CONFIG_LTTNG=m CONFIG_LTTNG_CLOCK_PLUGIN_TEST=m modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) CONFIG_LTTNG=m CONFIG_LTTNG_CLOCK_PLUGIN_TEST=m modules_install

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

%.i: %.c
	$(MAKE) -C $(KERNELDIR) M=$(PWD) CONFIG_LTTNG=m CONFIG_LTTNG_CLOCK_PLUGIN_TEST=m $@

endif # KERNELRELEASE
