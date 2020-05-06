# SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)

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

  obj-$(CONFIG_LTTNG) += src/
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
