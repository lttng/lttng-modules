#
# Makefile for the LTT probes.
# Only build from the package top-level directory. Never use with make directly.

ifneq ($(KERNELRELEASE),)
ifneq ($(CONFIG_TRACEPOINTS),)

ccflags-y += -I$(PWD)/probes
obj-m += lttng-types.o

obj-m += lttng-probe-sched.o
obj-m += lttng-probe-kvm.o
obj-m += lttng-probe-irq.o

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
