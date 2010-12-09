# LTTng tracing probes

ifdef CONFIG_FTRACE
CFLAGS_REMOVE_kernel-trace.o = -pg
CFLAGS_REMOVE_mm-trace.o = -pg
CFLAGS_REMOVE_fs-trace.o = -pg
CFLAGS_REMOVE_ipc-trace.o = -pg
CFLAGS_REMOVE_lockdep-trace.o = -pg
CFLAGS_REMOVE_rcu-trace.o = -pg
CFLAGS_REMOVE_syscall-trace.o = -pg
CFLAGS_REMOVE_trap-trace.o = -pg
CFLAGS_REMOVE_pm-trace.o = -pg
endif

obj-m += kernel-trace.o mm-trace.o fs-trace.o ipc-trace.o lockdep-trace.o \
	 rcu-trace.o syscall-trace.o trap-trace.o pm-trace.o

ifeq ($(CONFIG_NET),y)
ifdef CONFIG_FTRACE
CFLAGS_REMOVE_net-trace.o = -pg
CFLAGS_REMOVE_net-extended-trace.o = -pg
endif
obj-m += net-trace.o net-extended-trace.o
endif

ifdef CONFIG_JBD2
ifdef CONFIG_FTRACE
CFLAGS_REMOVE_jbd2-trace.o = -pg
endif
obj-m += jbd2-trace.o
endif

#ifdef CONFIG_EXT4_FS
#ifdef CONFIG_FTRACE
#CFLAGS_REMOVE_ext4-trace.o = -pg
#endif
#obj-$(CONFIG_LTT_TRACEPROBES)	+= ext4-trace.o
#endif

ifdef CONFIG_BLOCK
ifdef CONFIG_FTRACE
CFLAGS_REMOVE_block-trace.o = -pg
endif
obj-m += block-trace.o
endif


