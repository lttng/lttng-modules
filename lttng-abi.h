/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-abi.h
 *
 * LTTng ABI header
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_ABI_H
#define _LTTNG_ABI_H

#include <linux/fs.h>

/*
 * Major/minor version of ABI exposed to lttng tools. Major number
 * should be increased when an incompatible ABI change is done.
 */
#define LTTNG_MODULES_ABI_MAJOR_VERSION		2
#define LTTNG_MODULES_ABI_MINOR_VERSION		4

#define LTTNG_KERNEL_SYM_NAME_LEN	256
#define LTTNG_KERNEL_SESSION_NAME_LEN	256
#define LTTNG_KERNEL_SESSION_CREATION_TIME_ISO8601_LEN	26

enum lttng_kernel_instrumentation {
	LTTNG_KERNEL_TRACEPOINT	= 0,
	LTTNG_KERNEL_KPROBE	= 1,
	LTTNG_KERNEL_FUNCTION	= 2,
	LTTNG_KERNEL_KRETPROBE	= 3,
	LTTNG_KERNEL_NOOP	= 4,	/* not hooked */
	LTTNG_KERNEL_SYSCALL	= 5,
	LTTNG_KERNEL_UPROBE	= 6,
};

/*
 * LTTng consumer mode
 */
enum lttng_kernel_output {
	LTTNG_KERNEL_SPLICE	= 0,
	LTTNG_KERNEL_MMAP	= 1,
};

/*
 * LTTng DebugFS ABI structures.
 */
#define LTTNG_KERNEL_CHANNEL_PADDING	LTTNG_KERNEL_SYM_NAME_LEN + 32
struct lttng_kernel_channel {
	uint64_t subbuf_size;			/* in bytes */
	uint64_t num_subbuf;
	unsigned int switch_timer_interval;	/* usecs */
	unsigned int read_timer_interval;	/* usecs */
	enum lttng_kernel_output output;	/* splice, mmap */
	int overwrite;				/* 1: overwrite, 0: discard */
	char padding[LTTNG_KERNEL_CHANNEL_PADDING];
} __attribute__((packed));

struct lttng_kernel_kretprobe {
	uint64_t addr;

	uint64_t offset;
	char symbol_name[LTTNG_KERNEL_SYM_NAME_LEN];
} __attribute__((packed));

/*
 * Either addr is used, or symbol_name and offset.
 */
struct lttng_kernel_kprobe {
	uint64_t addr;

	uint64_t offset;
	char symbol_name[LTTNG_KERNEL_SYM_NAME_LEN];
} __attribute__((packed));

struct lttng_kernel_function_tracer {
	char symbol_name[LTTNG_KERNEL_SYM_NAME_LEN];
} __attribute__((packed));

struct lttng_kernel_uprobe {
	int fd;
} __attribute__((packed));

struct lttng_kernel_event_callsite_uprobe {
	uint64_t offset;
} __attribute__((packed));

struct lttng_kernel_event_callsite {
	union {
		struct lttng_kernel_event_callsite_uprobe uprobe;
	} u;
} __attribute__((packed));

enum lttng_kernel_syscall_entryexit {
	LTTNG_KERNEL_SYSCALL_ENTRYEXIT = 0,
	LTTNG_KERNEL_SYSCALL_ENTRY = 1,		/* Not implemented. */
	LTTNG_KERNEL_SYSCALL_EXIT = 2,		/* Not implemented. */
};

enum lttng_kernel_syscall_abi {
	LTTNG_KERNEL_SYSCALL_ABI_ALL = 0,
	LTTNG_KERNEL_SYSCALL_ABI_NATIVE = 1,	/* Not implemented. */
	LTTNG_KERNEL_SYSCALL_ABI_COMPAT = 2,	/* Not implemented. */
};

enum lttng_kernel_syscall_match {
	LTTNG_SYSCALL_MATCH_NAME = 0,
	LTTNG_SYSCALL_MATCH_NR = 1,		/* Not implemented. */
};

struct lttng_kernel_syscall {
	uint8_t entryexit;	/* enum lttng_kernel_syscall_entryexit */
	uint8_t abi;		/* enum lttng_kernel_syscall_abi */
	uint8_t match;		/* enum lttng_kernel_syscall_match */
	uint8_t padding;
	uint32_t nr;		/* For LTTNG_SYSCALL_MATCH_NR */
} __attribute__((packed));

/*
 * For syscall tracing, name = "*" means "enable all".
 */
#define LTTNG_KERNEL_EVENT_PADDING1	16
#define LTTNG_KERNEL_EVENT_PADDING2	LTTNG_KERNEL_SYM_NAME_LEN + 32
struct lttng_kernel_event {
	char name[LTTNG_KERNEL_SYM_NAME_LEN];	/* event name */
	enum lttng_kernel_instrumentation instrumentation;
	char padding[LTTNG_KERNEL_EVENT_PADDING1];

	/* Per instrumentation type configuration */
	union {
		struct lttng_kernel_kretprobe kretprobe;
		struct lttng_kernel_kprobe kprobe;
		struct lttng_kernel_function_tracer ftrace;
		struct lttng_kernel_uprobe uprobe;
		struct lttng_kernel_syscall syscall;
		char padding[LTTNG_KERNEL_EVENT_PADDING2];
	} u;
} __attribute__((packed));

struct lttng_kernel_tracer_version {
	uint32_t major;
	uint32_t minor;
	uint32_t patchlevel;
} __attribute__((packed));

struct lttng_kernel_tracer_abi_version {
	uint32_t major;
	uint32_t minor;
} __attribute__((packed));

struct lttng_kernel_session_name {
	char name[LTTNG_KERNEL_SESSION_NAME_LEN];
} __attribute__((packed));

struct lttng_kernel_session_creation_time {
	char iso8601[LTTNG_KERNEL_SESSION_CREATION_TIME_ISO8601_LEN];
} __attribute__((packed));

enum lttng_kernel_calibrate_type {
	LTTNG_KERNEL_CALIBRATE_KRETPROBE,
};

struct lttng_kernel_calibrate {
	enum lttng_kernel_calibrate_type type;	/* type (input) */
} __attribute__((packed));

struct lttng_kernel_syscall_mask {
	uint32_t len;	/* in bits */
	char mask[];
} __attribute__((packed));

enum lttng_kernel_context_type {
	LTTNG_KERNEL_CONTEXT_PID		= 0,
	LTTNG_KERNEL_CONTEXT_PERF_COUNTER	= 1,
	LTTNG_KERNEL_CONTEXT_PROCNAME		= 2,
	LTTNG_KERNEL_CONTEXT_PRIO		= 3,
	LTTNG_KERNEL_CONTEXT_NICE		= 4,
	LTTNG_KERNEL_CONTEXT_VPID		= 5,
	LTTNG_KERNEL_CONTEXT_TID		= 6,
	LTTNG_KERNEL_CONTEXT_VTID		= 7,
	LTTNG_KERNEL_CONTEXT_PPID		= 8,
	LTTNG_KERNEL_CONTEXT_VPPID		= 9,
	LTTNG_KERNEL_CONTEXT_HOSTNAME		= 10,
	LTTNG_KERNEL_CONTEXT_CPU_ID		= 11,
	LTTNG_KERNEL_CONTEXT_INTERRUPTIBLE	= 12,
	LTTNG_KERNEL_CONTEXT_PREEMPTIBLE	= 13,
	LTTNG_KERNEL_CONTEXT_NEED_RESCHEDULE	= 14,
	LTTNG_KERNEL_CONTEXT_MIGRATABLE		= 15,
	LTTNG_KERNEL_CONTEXT_CALLSTACK_KERNEL	= 16,
	LTTNG_KERNEL_CONTEXT_CALLSTACK_USER	= 17,
};

struct lttng_kernel_perf_counter_ctx {
	uint32_t type;
	uint64_t config;
	char name[LTTNG_KERNEL_SYM_NAME_LEN];
} __attribute__((packed));

#define LTTNG_KERNEL_CONTEXT_PADDING1	16
#define LTTNG_KERNEL_CONTEXT_PADDING2	LTTNG_KERNEL_SYM_NAME_LEN + 32
struct lttng_kernel_context {
	enum lttng_kernel_context_type ctx;
	char padding[LTTNG_KERNEL_CONTEXT_PADDING1];

	union {
		struct lttng_kernel_perf_counter_ctx perf_counter;
		char padding[LTTNG_KERNEL_CONTEXT_PADDING2];
	} u;
} __attribute__((packed));

#define LTTNG_KERNEL_FILTER_BYTECODE_MAX_LEN		65536
struct lttng_kernel_filter_bytecode {
	uint32_t len;
	uint32_t reloc_offset;
	uint64_t seqnum;
	char data[0];
} __attribute__((packed));

/* LTTng file descriptor ioctl */
#define LTTNG_KERNEL_SESSION			_IO(0xF6, 0x45)
#define LTTNG_KERNEL_TRACER_VERSION		\
	_IOR(0xF6, 0x46, struct lttng_kernel_tracer_version)
#define LTTNG_KERNEL_TRACEPOINT_LIST		_IO(0xF6, 0x47)
#define LTTNG_KERNEL_WAIT_QUIESCENT		_IO(0xF6, 0x48)
#define LTTNG_KERNEL_CALIBRATE			\
	_IOWR(0xF6, 0x49, struct lttng_kernel_calibrate)
#define LTTNG_KERNEL_SYSCALL_LIST		_IO(0xF6, 0x4A)
#define LTTNG_KERNEL_TRACER_ABI_VERSION		\
	_IOR(0xF6, 0x4B, struct lttng_kernel_tracer_abi_version)

/* Session FD ioctl */
#define LTTNG_KERNEL_METADATA			\
	_IOW(0xF6, 0x54, struct lttng_kernel_channel)
#define LTTNG_KERNEL_CHANNEL			\
	_IOW(0xF6, 0x55, struct lttng_kernel_channel)
#define LTTNG_KERNEL_SESSION_START		_IO(0xF6, 0x56)
#define LTTNG_KERNEL_SESSION_STOP		_IO(0xF6, 0x57)
#define LTTNG_KERNEL_SESSION_TRACK_PID		\
	_IOR(0xF6, 0x58, int32_t)
#define LTTNG_KERNEL_SESSION_UNTRACK_PID	\
	_IOR(0xF6, 0x59, int32_t)
/*
 * ioctl 0x58 and 0x59 are duplicated here. It works, since _IOR vs _IO
 * are generating two different ioctl numbers, but this was not done on
 * purpose. We should generally try to avoid those duplications.
 */
#define LTTNG_KERNEL_SESSION_LIST_TRACKER_PIDS	_IO(0xF6, 0x58)
#define LTTNG_KERNEL_SESSION_METADATA_REGEN	_IO(0xF6, 0x59)

/* 0x5A and 0x5B are reserved for a future ABI-breaking cleanup. */
#define LTTNG_KERNEL_SESSION_STATEDUMP		_IO(0xF6, 0x5C)
#define LTTNG_KERNEL_SESSION_SET_NAME		\
	_IOR(0xF6, 0x5D, struct lttng_kernel_session_name)
#define LTTNG_KERNEL_SESSION_SET_CREATION_TIME		\
	_IOR(0xF6, 0x5E, struct lttng_kernel_session_creation_time)

/* Channel FD ioctl */
#define LTTNG_KERNEL_STREAM			_IO(0xF6, 0x62)
#define LTTNG_KERNEL_EVENT			\
	_IOW(0xF6, 0x63, struct lttng_kernel_event)
#define LTTNG_KERNEL_SYSCALL_MASK		\
	_IOWR(0xF6, 0x64, struct lttng_kernel_syscall_mask)

/* Event and Channel FD ioctl */
#define LTTNG_KERNEL_CONTEXT			\
	_IOW(0xF6, 0x71, struct lttng_kernel_context)

/* Event, Channel and Session ioctl */
#define LTTNG_KERNEL_ENABLE			_IO(0xF6, 0x82)
#define LTTNG_KERNEL_DISABLE			_IO(0xF6, 0x83)

/* Event FD ioctl */
#define LTTNG_KERNEL_FILTER			_IO(0xF6, 0x90)
#define LTTNG_KERNEL_ADD_CALLSITE		_IO(0xF6, 0x91)

/*
 * LTTng-specific ioctls for the lib ringbuffer.
 *
 * Operations applying to the current sub-buffer need to occur between
 * a get/put or get_next/put_next ioctl pair.
 */

/* returns the timestamp begin of the current sub-buffer */
#define LTTNG_RING_BUFFER_GET_TIMESTAMP_BEGIN	_IOR(0xF6, 0x20, uint64_t)
/* returns the timestamp end of the current sub-buffer */
#define LTTNG_RING_BUFFER_GET_TIMESTAMP_END	_IOR(0xF6, 0x21, uint64_t)
/* returns the number of events discarded of the current sub-buffer */
#define LTTNG_RING_BUFFER_GET_EVENTS_DISCARDED	_IOR(0xF6, 0x22, uint64_t)
/* returns the packet payload size of the current sub-buffer */
#define LTTNG_RING_BUFFER_GET_CONTENT_SIZE	_IOR(0xF6, 0x23, uint64_t)
/* returns the packet size of the current sub-buffer*/
#define LTTNG_RING_BUFFER_GET_PACKET_SIZE	_IOR(0xF6, 0x24, uint64_t)
/* returns the stream id (invariant for the stream) */
#define LTTNG_RING_BUFFER_GET_STREAM_ID		_IOR(0xF6, 0x25, uint64_t)
/* returns the current timestamp as perceived from the tracer */
#define LTTNG_RING_BUFFER_GET_CURRENT_TIMESTAMP	_IOR(0xF6, 0x26, uint64_t)
/* returns the packet sequence number of the current sub-buffer */
#define LTTNG_RING_BUFFER_GET_SEQ_NUM		_IOR(0xF6, 0x27, uint64_t)
/* returns the stream instance id (invariant for the stream) */
#define LTTNG_RING_BUFFER_INSTANCE_ID		_IOR(0xF6, 0x28, uint64_t)

#ifdef CONFIG_COMPAT
/* returns the timestamp begin of the current sub-buffer */
#define LTTNG_RING_BUFFER_COMPAT_GET_TIMESTAMP_BEGIN \
	LTTNG_RING_BUFFER_GET_TIMESTAMP_BEGIN
/* returns the timestamp end of the current sub-buffer */
#define LTTNG_RING_BUFFER_COMPAT_GET_TIMESTAMP_END \
	LTTNG_RING_BUFFER_GET_TIMESTAMP_END
/* returns the number of events discarded of the current sub-buffer */
#define LTTNG_RING_BUFFER_COMPAT_GET_EVENTS_DISCARDED \
	LTTNG_RING_BUFFER_GET_EVENTS_DISCARDED
/* returns the packet payload size of the current sub-buffer */
#define LTTNG_RING_BUFFER_COMPAT_GET_CONTENT_SIZE \
	LTTNG_RING_BUFFER_GET_CONTENT_SIZE
/* returns the packet size of the current sub-buffer */
#define LTTNG_RING_BUFFER_COMPAT_GET_PACKET_SIZE \
	LTTNG_RING_BUFFER_GET_PACKET_SIZE
/* returns the stream id (invariant for the stream) */
#define LTTNG_RING_BUFFER_COMPAT_GET_STREAM_ID \
	LTTNG_RING_BUFFER_GET_STREAM_ID
/* returns the current timestamp as perceived from the tracer */
#define LTTNG_RING_BUFFER_COMPAT_GET_CURRENT_TIMESTAMP \
	LTTNG_RING_BUFFER_GET_CURRENT_TIMESTAMP
/* returns the packet sequence number of the current sub-buffer */
#define LTTNG_RING_BUFFER_COMPAT_GET_SEQ_NUM	\
	LTTNG_RING_BUFFER_GET_SEQ_NUM
/* returns the stream instance id (invariant for the stream) */
#define LTTNG_RING_BUFFER_COMPAT_INSTANCE_ID	\
	LTTNG_RING_BUFFER_INSTANCE_ID
#endif /* CONFIG_COMPAT */

#endif /* _LTTNG_ABI_H */
