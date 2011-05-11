#
# Makefile for the LTT objects.
#

ifneq ($(KERNELRELEASE),)
ifneq ($(CONFIG_TRACEPOINTS),)

obj-m += ltt-ring-buffer-client-discard.o
obj-m += ltt-ring-buffer-client-overwrite.o

obj-m += ltt-relay.o
ltt-relay-objs :=  ltt-events.o ltt-event-header.o ltt-debugfs-abi.o \
			ltt-probes.o ltt-core.o

obj-m += probes/

# Use externally packaged lib ring buffer if missing in kernel
ifneq ($(CONFIG_LIB_RING_BUFFER),)
else
obj-m += lib/
endif

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
