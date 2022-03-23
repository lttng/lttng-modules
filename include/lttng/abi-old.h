/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng/abi-old.h
 *
 * LTTng old ABI header (without support for compat 32/64 bits)
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_ABI_OLD_H
#define _LTTNG_ABI_OLD_H

#include <linux/fs.h>
#include <lttng/abi.h>

/*
 * LTTng DebugFS ABI structures.
 */
#define LTTNG_KERNEL_ABI_OLD_CHANNEL_PADDING	LTTNG_KERNEL_ABI_SYM_NAME_LEN + 32
struct lttng_kernel_abi_old_channel {
	int overwrite;				/* 1: overwrite, 0: discard */
	uint64_t subbuf_size;			/* in bytes */
	uint64_t num_subbuf;
	unsigned int switch_timer_interval;	/* usecs */
	unsigned int read_timer_interval;	/* usecs */
	enum lttng_kernel_abi_output output;	/* splice, mmap */
	char padding[LTTNG_KERNEL_ABI_OLD_CHANNEL_PADDING];
};

struct lttng_kernel_abi_old_kretprobe {
	uint64_t addr;

	uint64_t offset;
	char symbol_name[LTTNG_KERNEL_ABI_SYM_NAME_LEN];
};

/*
 * Either addr is used, or symbol_name and offset.
 */
struct lttng_kernel_abi_old_kprobe {
	uint64_t addr;

	uint64_t offset;
	char symbol_name[LTTNG_KERNEL_ABI_SYM_NAME_LEN];
};

struct lttng_kernel_abi_old_function_tracer {
	char symbol_name[LTTNG_KERNEL_ABI_SYM_NAME_LEN];
};

/*
 * For syscall tracing, name = '\0' means "enable all".
 */
#define LTTNG_KERNEL_ABI_OLD_EVENT_PADDING1	16
#define LTTNG_KERNEL_ABI_OLD_EVENT_PADDING2	LTTNG_KERNEL_ABI_SYM_NAME_LEN + 32
struct lttng_kernel_abi_old_event {
	char name[LTTNG_KERNEL_ABI_SYM_NAME_LEN];	/* event name */
	enum lttng_kernel_abi_instrumentation instrumentation;
	char padding[LTTNG_KERNEL_ABI_OLD_EVENT_PADDING1];

	/* Per instrumentation type configuration */
	union {
		struct lttng_kernel_abi_old_kretprobe kretprobe;
		struct lttng_kernel_abi_old_kprobe kprobe;
		struct lttng_kernel_abi_old_function_tracer ftrace;
		char padding[LTTNG_KERNEL_ABI_OLD_EVENT_PADDING2];
	} u;
};

struct lttng_kernel_abi_old_tracer_version {
	uint32_t major;
	uint32_t minor;
	uint32_t patchlevel;
};

struct lttng_kernel_abi_old_calibrate {
	enum lttng_kernel_abi_calibrate_type type;	/* type (input) */
};

struct lttng_kernel_abi_old_perf_counter_ctx {
	uint32_t type;
	uint64_t config;
	char name[LTTNG_KERNEL_ABI_SYM_NAME_LEN];
};

#define LTTNG_KERNEL_ABI_OLD_CONTEXT_PADDING1	16
#define LTTNG_KERNEL_ABI_OLD_CONTEXT_PADDING2	LTTNG_KERNEL_ABI_SYM_NAME_LEN + 32
struct lttng_kernel_abi_old_context {
	enum lttng_kernel_abi_context_type ctx;
	char padding[LTTNG_KERNEL_ABI_OLD_CONTEXT_PADDING1];

	union {
		struct lttng_kernel_abi_old_perf_counter_ctx perf_counter;
		char padding[LTTNG_KERNEL_ABI_OLD_CONTEXT_PADDING2];
	} u;
};

#define LTTNG_KERNEL_ABI_OLD_COUNTER_DIMENSION_MAX	4

struct lttng_kernel_abi_old_counter_dimension {
	uint64_t size;
	uint64_t underflow_index;
	uint64_t overflow_index;
	uint8_t has_underflow;
	uint8_t has_overflow;
} __attribute__((packed));

#define LTTNG_KERNEL_ABI_OLD_COUNTER_CONF_PADDING1	67
struct lttng_kernel_abi_old_counter_conf {
	uint32_t arithmetic;	/* enum lttng_kernel_abi_counter_arithmetic */
	uint32_t bitness;	/* enum lttng_kernel_abi_counter_bitness */
	uint32_t number_dimensions;
	int64_t global_sum_step;
	struct lttng_kernel_abi_old_counter_dimension dimensions[LTTNG_KERNEL_ABI_OLD_COUNTER_DIMENSION_MAX];
	uint8_t coalesce_hits;
	char padding[LTTNG_KERNEL_ABI_OLD_COUNTER_CONF_PADDING1];
} __attribute__((packed));

struct lttng_kernel_abi_old_counter_index {
	uint32_t number_dimensions;
	uint64_t dimension_indexes[LTTNG_KERNEL_ABI_OLD_COUNTER_DIMENSION_MAX];
} __attribute__((packed));

struct lttng_kernel_abi_old_counter_value {
	int64_t value;
	uint8_t underflow;
	uint8_t overflow;
} __attribute__((packed));

#define LTTNG_KERNEL_ABI_OLD_COUNTER_READ_PADDING 32
struct lttng_kernel_abi_old_counter_read {
	struct lttng_kernel_abi_old_counter_index index;
	int32_t cpu;	/* -1 for global counter, >= 0 for specific cpu. */
	struct lttng_kernel_abi_old_counter_value value;	/* output */
	char padding[LTTNG_KERNEL_ABI_OLD_COUNTER_READ_PADDING];
} __attribute__((packed));

#define LTTNG_KERNEL_ABI_OLD_COUNTER_AGGREGATE_PADDING 32
struct lttng_kernel_abi_old_counter_aggregate {
	struct lttng_kernel_abi_old_counter_index index;
	struct lttng_kernel_abi_old_counter_value value;	/* output */
	char padding[LTTNG_KERNEL_ABI_OLD_COUNTER_AGGREGATE_PADDING];
} __attribute__((packed));

#define LTTNG_KERNEL_ABI_OLD_COUNTER_CLEAR_PADDING 32
struct lttng_kernel_abi_old_counter_clear {
	struct lttng_kernel_abi_old_counter_index index;
	char padding[LTTNG_KERNEL_ABI_OLD_COUNTER_CLEAR_PADDING];
} __attribute__((packed));

/* LTTng file descriptor ioctl */
#define LTTNG_KERNEL_ABI_OLD_SESSION		_IO(0xF6, 0x40)
#define LTTNG_KERNEL_ABI_OLD_TRACER_VERSION		\
	_IOR(0xF6, 0x41, struct lttng_kernel_abi_old_tracer_version)
#define LTTNG_KERNEL_ABI_OLD_TRACEPOINT_LIST	_IO(0xF6, 0x42)
#define LTTNG_KERNEL_ABI_OLD_WAIT_QUIESCENT		_IO(0xF6, 0x43)
#define LTTNG_KERNEL_ABI_OLD_CALIBRATE		\
	_IOWR(0xF6, 0x44, struct lttng_kernel_abi_old_calibrate)

/* Session FD ioctl */
#define LTTNG_KERNEL_ABI_OLD_METADATA		\
	_IOW(0xF6, 0x50, struct lttng_kernel_abi_old_channel)
#define LTTNG_KERNEL_ABI_OLD_CHANNEL		\
	_IOW(0xF6, 0x51, struct lttng_kernel_abi_old_channel)
#define LTTNG_KERNEL_ABI_OLD_SESSION_START		_IO(0xF6, 0x52)
#define LTTNG_KERNEL_ABI_OLD_SESSION_STOP		_IO(0xF6, 0x53)

/* Channel FD ioctl */
#define LTTNG_KERNEL_ABI_OLD_STREAM			_IO(0xF6, 0x60)
#define LTTNG_KERNEL_ABI_OLD_EVENT			\
	_IOW(0xF6, 0x61, struct lttng_kernel_abi_old_event)

/* Event and Channel FD ioctl */
#define LTTNG_KERNEL_ABI_OLD_CONTEXT		\
	_IOW(0xF6, 0x70, struct lttng_kernel_abi_old_context)

/* Event, Channel and Session ioctl */
#define LTTNG_KERNEL_ABI_OLD_ENABLE			_IO(0xF6, 0x80)
#define LTTNG_KERNEL_ABI_OLD_DISABLE		_IO(0xF6, 0x81)

#define LTTNG_KERNEL_ABI_OLD_COUNTER \
	_IOW(0xF6, 0x84, struct lttng_kernel_abi_old_counter_conf)

/* Counter file descriptor ioctl ("old" 2.13 ABI) */
#define LTTNG_KERNEL_ABI_OLD_COUNTER_READ \
	_IOWR(0xF6, 0xC0, struct lttng_kernel_abi_old_counter_read)
#define LTTNG_KERNEL_ABI_OLD_COUNTER_AGGREGATE \
	_IOWR(0xF6, 0xC1, struct lttng_kernel_abi_old_counter_aggregate)
#define LTTNG_KERNEL_ABI_OLD_COUNTER_CLEAR \
	_IOW(0xF6, 0xC2, struct lttng_kernel_abi_old_counter_clear)

#endif /* _LTTNG_ABI_OLD_H */
