#
# Makefile for the LTT objects.
#

ifneq ($(KERNELRELEASE),)
ifneq ($(CONFIG_TRACEPOINTS),)

obj-m += ltt-ring-buffer-client-discard.o
obj-m += ltt-ring-buffer-client-overwrite.o
obj-m += ltt-ring-buffer-metadata-client.o

obj-m += ltt-relay.o
ltt-relay-objs :=  ltt-events.o ltt-debugfs-abi.o \
			ltt-probes.o ltt-core.o ltt-context.o \
			lttng-context-pid.o lttng-context-comm.o \
			lttng-context-prio.o lttng-context-nice.o \
			lttng-context-vpid.o lttng-context-tid.o \
			lttng-context-vtid.o lttng-context-ppid.o \
			lttng-context-vppid.o

ifneq ($(CONFIG_PERF_EVENTS),)
ltt-relay-objs += $(shell \
	if [ $(VERSION) -ge 2 -a $(PATCHLEVEL) -ge 6 -a $(SUBLEVEL) -ge 33 ] ; then \
		echo "lttng-context-perf-counters.o" ; fi;)
endif

obj-m += probes/
obj-m += lib/

endif

else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
	CFLAGS = $(EXTCFLAGS)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install
	/sbin/depmod -a

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

endif
