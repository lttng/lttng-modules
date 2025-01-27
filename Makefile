# SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
# SPDX-FileCopyrightText: 2010-2024 EfficiOS Inc.

ifneq ($(KERNELRELEASE),)

# This part of the Makefile is used when called by the kernel build system
# and defines the modules to be built.

obj-$(CONFIG_LTTNG) += src/

else # KERNELRELEASE

# This part of the Makefile is used when the 'make' command is runned in the
# base directory of the lttng-modules sources. It sets some environment and
# calls the kernel build system to build the actual modules.

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# Experimental bitwise enum defaults to disabled.
CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM ?= n

# Emulate Kconfig behavior of setting defines for config options.
LKCPPFLAGS = $(KCPPFLAGS)
ifeq ($(CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM),y)
LKCPPFLAGS += -DCONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM=y
endif

# Work-around for crtsavres.o missing $(objtree) prefix on powerpc
ifeq ($(ARCH),powerpc)
MO ?= $(KERNELDIR)
MO_OPT = MO=$(MO)
endif

default: modules

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)/src \
		CONFIG_LTTNG=m CONFIG_LTTNG_CLOCK_PLUGIN_TEST=m \
		KCPPFLAGS='$(LKCPPFLAGS)' $(MO_OPT) \
		modules

syscalls_extractor:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)/src \
		CONFIG_LTTNG_SYSCALLS_EXTRACTOR=m \
		CONFIG_LTTNG=m CONFIG_LTTNG_CLOCK_PLUGIN_TEST=m \
		KCPPFLAGS='$(LKCPPFLAGS)' \
		modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)/src \
		CONFIG_LTTNG=m CONFIG_LTTNG_CLOCK_PLUGIN_TEST=m \
		KCPPFLAGS='$(LKCPPFLAGS)' \
		modules_install

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)/src clean

# The following targets are used for development and debugging. They are not
# part of the build system.
%.i: %.c
	$(MAKE) -C $(KERNELDIR) M=$(PWD) \
		CONFIG_LTTNG=m CONFIG_LTTNG_CLOCK_PLUGIN_TEST=m \
		KCPPFLAGS='$(LKCPPFLAGS)' \
		$@

%.o: %.c
	$(MAKE) -C $(KERNELDIR) M=$(PWD) \
		CONFIG_LTTNG=m CONFIG_LTTNG_CLOCK_PLUGIN_TEST=m \
		KCPPFLAGS='$(LKCPPFLAGS)' \
		$@

endif # KERNELRELEASE
