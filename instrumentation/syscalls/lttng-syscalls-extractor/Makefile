# SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)

ifneq ($(KERNELRELEASE),)

  # This part of the Makefile is used when called by the kernel build system
  # and defines the modules to be built.

  obj-m += lttng-syscalls-extractor.o

else # KERNELRELEASE

# This part of the Makefile is used when the 'make' command is runned in the
# base directory of the lttng-modules sources. It sets some environment and
# calls the kernel build system to build the actual modules.

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
CFLAGS = $(EXTCFLAGS)

default: modules

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

%.i: %.c
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $@

endif # KERNELRELEASE
