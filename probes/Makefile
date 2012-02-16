#
# Makefile for the LTT probes.
# Only build from the package top-level directory. Never use with make directly.

ifneq ($(KERNELRELEASE),)
ifneq ($(CONFIG_TRACEPOINTS),)

ccflags-y += -I$(PWD)/probes
obj-m += lttng-types.o

obj-m += lttng-probe-lttng.o

obj-m += lttng-probe-sched.o
obj-m += lttng-probe-irq.o

obj-m += lttng-probe-statedump.o

ifneq ($(CONFIG_KVM),)
obj-m += lttng-probe-kvm.o
endif

ifneq ($(CONFIG_BLOCK),)
ifneq ($(CONFIG_EVENT_TRACING),)	# need blk_cmd_buf_len
obj-m +=  $(shell \
	if [ $(VERSION) -ge 3 \
		-o \( $(VERSION) -eq 2 -a $(PATCHLEVEL) -ge 6 -a $(SUBLEVEL) -ge 38 \) ] ; then \
		echo "lttng-probe-block.o" ; fi;)
endif
endif

ifneq ($(CONFIG_KPROBES),)
obj-m += lttng-kprobes.o
endif


ifneq ($(CONFIG_KRETPROBES),)
obj-m += lttng-kretprobes.o
endif

ifneq ($(CONFIG_DYNAMIC_FTRACE),)
obj-m += lttng-ftrace.o
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
