#ifndef _LTT_DEBUGFS_ABI_H
#define _LTT_DEBUGFS_ABI_H

/*
 * ltt-debugfs-abi.h
 *
 * Copyright 2010 (c) - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * LTTng debugfs ABI header
 */

#include <linux/fs.h>

#define LTTNG_KPROBE_SYM_NAME_LEN	128

enum lttng_kernel_instrumentation {
	LTTNG_KERNEL_TRACEPOINTS,
	LTTNG_KERNEL_KPROBES,
	LTTNG_KERNEL_FUNCTION_TRACER,
};

/*
 * LTTng DebugFS ABI structures.
 */

struct lttng_kernel_channel {
	int overwrite;			/* 1: overwrite, 0: discard */
	uint64_t subbuf_size;
	uint64_t num_subbuf;
	unsigned int switch_timer_interval;
	unsigned int read_timer_interval;
};

/*
 * Either addr is used, or symbol_name and offset.
 */
struct lttng_kernel_kprobe {
	uint64_t addr;

	uint64_t offset;
	char symbol_name[LTTNG_KPROBE_SYM_NAME_LEN];
};

struct lttng_kernel_function_tracer {
	char symbol_name[LTTNG_KPROBE_SYM_NAME_LEN];
};

struct lttng_kernel_event {
	enum lttng_kernel_instrumentation instrumentation;
	/* Per instrumentation type configuration */
	union {
		struct lttng_kernel_kprobe kprobe;
		struct lttng_kernel_function_tracer ftrace;
	} u;
	char name[];	/* event name */
};

struct lttng_kernel_tracer_version {
	uint32_t version;
	uint32_t patchlevel;
	uint32_t sublevel;
};

#define LTTNG_KERNEL_SESSION		_IO(0xF6, 0x40)
#define LTTNG_KERNEL_SESSION_START	_IO(0xF6, 0x41)
#define LTTNG_KERNEL_SESSION_STOP	_IO(0xF6, 0x42)
#define LTTNG_KERNEL_CHANNEL		\
	_IOW(0xF6, 0x43, struct lttng_kernel_channel)
#define LTTNG_KERNEL_STREAM		_IO(0xF6, 0x44)
#define LTTNG_KERNEL_EVENT		\
	_IOW(0xF6, 0x45, struct lttng_kernel_event)
#define LTTNG_KERNEL_METADATA		\
	_IOW(0xF6, 0x46, struct lttng_kernel_channel)
#define LTTNG_KERNEL_KPROBE		\
	_IOW(0xF6, 0x47, struct lttng_kernel_kprobe)
#define LTTNG_KERNEL_TRACER_VERSION	\
	_IOR(0xF6, 0x48, struct lttng_kernel_tracer_version)

#endif /* _LTT_DEBUGFS_ABI_H */
