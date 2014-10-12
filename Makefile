#
# Makefile for the LTT objects.
#

ifneq ($(KERNELRELEASE),)
ifneq ($(CONFIG_TRACEPOINTS),)

KERNELDIR = ${LTTNG_KERNELDIR}
MAKEFILEDIR = $(shell dirname $(lastword $(MAKEFILE_LIST)))

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

include $(MAKEFILEDIR)/Makefile.ABI.workarounds

obj-m += lttng-ring-buffer-client-discard.o
obj-m += lttng-ring-buffer-client-overwrite.o
obj-m += lttng-ring-buffer-metadata-client.o
obj-m += lttng-ring-buffer-client-mmap-discard.o
obj-m += lttng-ring-buffer-client-mmap-overwrite.o
obj-m += lttng-ring-buffer-metadata-mmap-client.o
obj-m += lttng-clock.o

obj-m += lttng-tracer.o
lttng-tracer-objs :=  lttng-events.o lttng-abi.o \
			lttng-probes.o lttng-context.o \
			lttng-context-pid.o lttng-context-procname.o \
			lttng-context-prio.o lttng-context-nice.o \
			lttng-context-vpid.o lttng-context-tid.o \
			lttng-context-vtid.o lttng-context-ppid.o \
			lttng-context-vppid.o lttng-context-cpu-id.o \
			lttng-calibrate.o \
			lttng-context-hostname.o wrapper/random.o \
			probes/lttng.o wrapper/trace-clock.o \
			wrapper/page_alloc.o \
			lttng-tracker-pid.o \
			lttng-filter.o lttng-filter-interpreter.o \
			lttng-filter-specialize.o \
			lttng-filter-validator.o \
			probes/lttng-probe-user.o

obj-m += lttng-statedump.o
lttng-statedump-objs := lttng-statedump-impl.o wrapper/irqdesc.o \
			wrapper/fdtable.o

ifneq ($(CONFIG_HAVE_SYSCALL_TRACEPOINTS),)
lttng-tracer-objs += lttng-syscalls.o
endif # CONFIG_HAVE_SYSCALL_TRACEPOINTS

ifneq ($(CONFIG_PERF_EVENTS),)
lttng-tracer-objs += $(shell \
	if [ $(VERSION) -ge 3 \
		-o \( $(VERSION) -eq 2 -a $(PATCHLEVEL) -ge 6 -a $(SUBLEVEL) -ge 33 \) ] ; then \
		echo "lttng-context-perf-counters.o" ; fi;)
endif # CONFIG_PERF_EVENTS

lttng-tracer-objs += $(shell \
	if [ $(VERSION) -ge 4 \
		-o \( $(VERSION) -eq 3 -a $(PATCHLEVEL) -ge 15 -a $(SUBLEVEL) -ge 0 \) ] ; then \
		echo "lttng-tracepoint.o" ; fi;)

obj-m += probes/
obj-m += lib/

endif # CONFIG_TRACEPOINTS

else # KERNELRELEASE
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
	CFLAGS = $(EXTCFLAGS)

default:
	LTTNG_KERNELDIR=$(KERNELDIR) $(MAKE) -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	LTTNG_KERNELDIR=$(KERNELDIR) $(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

clean:
	LTTNG_KERNELDIR=$(KERNELDIR) $(MAKE) -C $(KERNELDIR) M=$(PWD) clean

%.i: %.c
	LTTNG_KERNELDIR=$(KERNELDIR) $(MAKE) -C $(KERNELDIR) M=$(PWD) $@
endif # KERNELRELEASE
