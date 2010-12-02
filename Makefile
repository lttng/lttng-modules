#
# Makefile for the LTT objects.
#

ifneq ($(KERNELRELEASE),)
ifneq ($(CONFIG_TRACEPOINTS),)

obj-m += ltt-core.o
obj-m += ltt-debugfs-abi.o
obj-m += ltt-events.o
obj-m += ltt-ring-buffer-client-discard.o
obj-m += ltt-ring-buffer-client-overwrite.o

obj-m += ltt-relay.o
ltt-relay-objs :=  ltt-event-header.o ltt-serialize.o ltt-type-serializer.o

#obj-m += ltt-marker-control.o
#obj-m += ltt-trace-control.o
#ltt-ascii.o

#obj-m += ltt-statedump.o
#obj-m += ltt-userspace-event.o
#obj-m += ltt-filter.o
#obj-m += ltt-kprobes.o
#obj-m += probes/

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
