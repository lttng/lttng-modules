/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng/abi.h
 *
 * LTTng ABI header
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_ABI_H
#define _LTTNG_ABI_H

#include <linux/fs.h>
#include <linux/types.h>

/*
 * Major/minor version of ABI exposed to lttng tools. Major number
 * should be increased when an incompatible ABI change is done.
 */
#define LTTNG_KERNEL_ABI_MAJOR_VERSION		2
#define LTTNG_KERNEL_ABI_MINOR_VERSION		6

#define LTTNG_KERNEL_ABI_SYM_NAME_LEN		256
#define LTTNG_KERNEL_ABI_SESSION_NAME_LEN	256

/*
 * The expected iso8601 time formats are either:
 *
 * - YYYYmmddTHHMMSS+HHMM           (20 characters + \0)
 * - YYYY-mm-ddTHH:MM:SS+HH:MM      (25 characters + \0)
 */
#define LTTNG_KERNEL_ABI_SESSION_CREATION_TIME_ISO8601_LEN	26

enum lttng_kernel_abi_instrumentation {
	LTTNG_KERNEL_ABI_TRACEPOINT	= 0,
	LTTNG_KERNEL_ABI_KPROBE	= 1,
	LTTNG_KERNEL_ABI_FUNCTION	= 2,
	LTTNG_KERNEL_ABI_KRETPROBE	= 3,
	LTTNG_KERNEL_ABI_NOOP	= 4,	/* not hooked */
	LTTNG_KERNEL_ABI_SYSCALL	= 5,
	LTTNG_KERNEL_ABI_UPROBE	= 6,
};

/*
 * LTTng consumer mode
 */
enum lttng_kernel_abi_output {
	LTTNG_KERNEL_ABI_SPLICE	= 0,
	LTTNG_KERNEL_ABI_MMAP	= 1,
};

/*
 * LTTng DebugFS ABI structures.
 */
#define LTTNG_KERNEL_ABI_CHANNEL_PADDING	LTTNG_KERNEL_ABI_SYM_NAME_LEN + 32
struct lttng_kernel_abi_channel {
	uint64_t subbuf_size;			/* in bytes */
	uint64_t num_subbuf;
	unsigned int switch_timer_interval;	/* usecs */
	unsigned int read_timer_interval;	/* usecs */
	uint32_t output;			/* enum lttng_kernel_abi_output (splice, mmap) */
	int overwrite;				/* 1: overwrite, 0: discard */
	char padding[LTTNG_KERNEL_ABI_CHANNEL_PADDING];
} __attribute__((packed));

enum lttng_kernel_abi_kretprobe_entryexit {
	LTTNG_KERNEL_ABI_KRETPROBE_ENTRYEXIT = 0,
	LTTNG_KERNEL_ABI_KRETPROBE_ENTRY = 1,
	LTTNG_KERNEL_ABI_KRETPROBE_EXIT = 2,
};

struct lttng_kernel_abi_kretprobe {
	uint64_t addr;

	uint64_t offset;
	char symbol_name[LTTNG_KERNEL_ABI_SYM_NAME_LEN];
	uint8_t entryexit;	/* enum lttng_kernel_abi_kretprobe_entryexit */
} __attribute__((packed));

/*
 * Either addr is used, or symbol_name and offset.
 */
struct lttng_kernel_abi_kprobe {
	uint64_t addr;

	uint64_t offset;
	char symbol_name[LTTNG_KERNEL_ABI_SYM_NAME_LEN];
} __attribute__((packed));

struct lttng_kernel_abi_function_tracer {
	char symbol_name[LTTNG_KERNEL_ABI_SYM_NAME_LEN];
} __attribute__((packed));

struct lttng_kernel_abi_uprobe {
	int fd;
} __attribute__((packed));

struct lttng_kernel_abi_event_callsite_uprobe {
	uint64_t offset;
} __attribute__((packed));

struct lttng_kernel_abi_event_callsite {
	union {
		struct lttng_kernel_abi_event_callsite_uprobe uprobe;
	} u;
} __attribute__((packed));

enum lttng_kernel_abi_syscall_entryexit {
	LTTNG_KERNEL_ABI_SYSCALL_ENTRYEXIT	= 0,
	LTTNG_KERNEL_ABI_SYSCALL_ENTRY	= 1,
	LTTNG_KERNEL_ABI_SYSCALL_EXIT	= 2,
};

enum lttng_kernel_abi_syscall_abi {
	LTTNG_KERNEL_ABI_SYSCALL_ABI_ALL = 0,
	LTTNG_KERNEL_ABI_SYSCALL_ABI_NATIVE = 1,
	LTTNG_KERNEL_ABI_SYSCALL_ABI_COMPAT = 2,
};

enum lttng_kernel_abi_syscall_match {
	LTTNG_KERNEL_ABI_SYSCALL_MATCH_NAME = 0,
	LTTNG_KERNEL_ABI_SYSCALL_MATCH_NR = 1,		/* Not implemented. */
};

struct lttng_kernel_abi_syscall {
	uint8_t entryexit;	/* enum lttng_kernel_abi_syscall_entryexit */
	uint8_t abi;		/* enum lttng_kernel_abi_syscall_abi */
	uint8_t match;		/* enum lttng_kernel_abi_syscall_match */
	uint8_t padding;
	uint32_t nr;		/* For LTTNG_SYSCALL_MATCH_NR */
} __attribute__((packed));

/*
 * An immediate match requires that the requested event matches existing
 * instrumentation when the ioctl is issued, whereas a lazy match does not
 * have this constraint.
 *
 * Instrumentation                  default
 * ---------------------------------------------------------------------------
 * LTTNG_KERNEL_ABI_TRACEPOINT:     lazy
 * LTTNG_KERNEL_ABI_SYSCALL:        lazy
 * LTTNG_KERNEL_ABI_KPROBE:         immediate
 * LTTNG_KERNEL_ABI_KRETPROBE:      immediate
 * LTTNG_KERNEL_ABI_UPROBE:         immediate
 */
enum lttng_kernel_abi_match_check {
	LTTNG_KERNEL_ABI_MATCH_DEFAULT = 0,
	LTTNG_KERNEL_ABI_MATCH_IMMEDIATE = 1,
	LTTNG_KERNEL_ABI_MATCH_LAZY = 2,
};

/*
 * Extended event parameters.
 */
struct lttng_kernel_abi_event_ext {
	uint32_t len;				/* length of this structure */

	uint8_t match_check;			/* enum lttng_kernel_abi_match_check */
}  __attribute__((packed));

/*
 * For syscall tracing, name = "*" means "enable all".
 */
#define LTTNG_KERNEL_ABI_EVENT_PADDING2	LTTNG_KERNEL_ABI_SYM_NAME_LEN + 32
struct lttng_kernel_abi_event {
	char name[LTTNG_KERNEL_ABI_SYM_NAME_LEN];	/* event name */
	uint32_t instrumentation;		/* enum lttng_kernel_abi_instrumentation */
	uint64_t token;				/* User-provided token */
	uint64_t event_ext;			/* struct lttng_kernel_abi_event_ext */

	/* Per instrumentation type configuration */
	union {
		struct lttng_kernel_abi_kretprobe kretprobe;
		struct lttng_kernel_abi_kprobe kprobe;
		struct lttng_kernel_abi_function_tracer ftrace;
		struct lttng_kernel_abi_uprobe uprobe;
		struct lttng_kernel_abi_syscall syscall;
		char padding[LTTNG_KERNEL_ABI_EVENT_PADDING2];
	} u;
} __attribute__((packed));

#define LTTNG_KERNEL_ABI_EVENT_NOTIFIER_PADDING	32
struct lttng_kernel_abi_event_notifier {
	struct lttng_kernel_abi_event event;
	uint64_t error_counter_index;

	char padding[LTTNG_KERNEL_ABI_EVENT_NOTIFIER_PADDING];
} __attribute__((packed));

#define LTTNG_KERNEL_ABI_EVENT_NOTIFIER_NOTIFICATION_PADDING 32
struct lttng_kernel_abi_event_notifier_notification {
	uint64_t token;
	uint16_t capture_buf_size;
	char padding[LTTNG_KERNEL_ABI_EVENT_NOTIFIER_NOTIFICATION_PADDING];
} __attribute__((packed));

enum lttng_kernel_abi_key_token_type {
	LTTNG_KERNEL_ABI_KEY_TOKEN_STRING = 0,		/* arg: string_ptr. */
	LTTNG_KERNEL_ABI_KEY_TOKEN_EVENT_NAME = 1,	/* no arg. */
	LTTNG_KERNEL_ABI_KEY_TOKEN_PROVIDER_NAME = 2,	/* no arg. */
};

enum lttng_kernel_abi_counter_arithmetic {
	LTTNG_KERNEL_ABI_COUNTER_ARITHMETIC_MODULAR = 0,
};

enum lttng_kernel_abi_counter_bitness {
	LTTNG_KERNEL_ABI_COUNTER_BITNESS_32 = 0,
	LTTNG_KERNEL_ABI_COUNTER_BITNESS_64 = 1,
};

struct lttng_kernel_abi_key_token {
	uint32_t len;				/* length of child structure. */
	uint32_t type;				/* enum lttng_kernel_abi_key_token_type */
	/*
	 * The size of this structure is fixed because it is embedded into
	 * children structures.
	 */
} __attribute__((packed));

/* Length of this structure excludes the following string. */
struct lttng_kernel_abi_key_token_string {
	struct lttng_kernel_abi_key_token parent;
	uint32_t string_len;		/* string length (includes \0) */

	/* Null-terminated string of length @string_len follows this structure. */
} __attribute__((packed));

/*
 * token types event_name and provider_name don't have specific fields,
 * so they do not need to derive their own specific child structure.
 */

struct lttng_kernel_abi_counter_map_descriptor {
	uint32_t len;			/* length of this structure. */

	uint64_t descriptor_index;	/* Descriptor index (input: [ 0 .. nr_descriptors - 1 ]) */

	uint32_t dimension;		/* Dimension indexed (output) */
	uint64_t user_token;		/* User-provided 64-bit token (output) */
	uint64_t key_string;		/*
					 * Pointer (input) to key string associated with this index
					 * (output). If key_string_len is smaller than the required
					 * space, the ioctl fails with -ENOSPC, storing the required
					 * string space into @key_string_len.
					 */
	uint32_t key_string_len;	/* Key string length (input/output, includes \0) */
	uint64_t array_indexes;		/*
					 * Pointer (input) to array of indexes within each dimension
					 * (output). There are @dimension values populated.  Each
					 * element is of type uint64_t.  If arrays_indexes_len is
					 * smaller than @dimension, the ioctl fails with -ENOSPC,
					 * storing the required array index length into
					 * @array_index_len.
					 */
	uint32_t array_indexes_len;	/* Array indexes length (input/output). */
} __attribute__((packed));

/*
 * Dimension indexing: All events should use the same key type to index
 * a given map dimension.
 */
enum lttng_kernel_abi_key_type {
	LTTNG_KERNEL_ABI_KEY_TYPE_TOKENS = 0,		/* Dimension key is a set of tokens. */
	LTTNG_KERNEL_ABI_KEY_TYPE_INTEGER = 1,		/* Dimension key is an integer value. */
};

struct lttng_kernel_abi_counter_key_dimension {
	uint32_t len;			/* length of child structure */
	uint32_t key_type;		/* enum lttng_kernel_abi_key_type */
	/*
	 * The size of this structure is fixed because it is embedded
	 * into children structures.
	 */
} __attribute__((packed));

struct lttng_kernel_abi_counter_key_dimension_tokens {
	struct lttng_kernel_abi_counter_key_dimension parent;
	uint32_t nr_key_tokens;

	/* Followed by an array of nr_key_tokens struct lttng_kernel_abi_key_token elements. */
} __attribute__((packed));

/*
 * The "integer" key type is not implemented yet, but when it will be
 * introduced in the future, its specific key dimension will allow
 * defining the function to apply over input argument, bytecode to run
 * and so on.
 */

enum lttng_kernel_abi_counter_action {
	LTTNG_KERNEL_ABI_COUNTER_ACTION_INCREMENT = 0,

	/*
	 * Can be extended with additional actions, such as decrement,
	 * set value, run bytecode, and so on.
	 */
};

struct lttng_kernel_abi_counter_event {
	uint32_t len;			/* length of this structure */
	uint32_t action;		/* enum lttng_kernel_abi_counter_action */

	struct lttng_kernel_abi_event event;
	uint32_t number_key_dimensions;		/* array of dimensions is an array of var. len. elements. */

	/*
	 * Followed by additional data specific to the action, and by a
	 * variable-length array of key dimensions.
	 */
} __attribute__((packed));

enum lttng_kernel_abi_counter_dimension_flags {
	LTTNG_KERNEL_ABI_COUNTER_DIMENSION_FLAG_UNDERFLOW = (1 << 0),
	LTTNG_KERNEL_ABI_COUNTER_DIMENSION_FLAG_OVERFLOW = (1 << 1),
};

struct lttng_kernel_abi_counter_dimension {
	uint32_t key_type;		/* enum lttng_kernel_abi_key_type */
	uint32_t flags;			/* enum lttng_kernel_abi_counter_dimension_flags */
	uint64_t size;			/* dimension size */
	uint64_t underflow_index;
	uint64_t overflow_index;
} __attribute__((packed));

struct lttng_kernel_abi_counter_dimension_array {
	uint32_t number_dimensions;
	uint32_t elem_len;		/* array stride (size of struct lttng_kernel_abi_counter_dimension) */
	uint64_t ptr;			/* pointer to array of struct lttng_kernel_abi_counter_dimension */
} __attribute__((packed));

enum lttng_kernel_abi_counter_conf_flags {
	LTTNG_KERNEL_ABI_COUNTER_CONF_FLAG_COALESCE_HITS = (1 << 0),
};

struct lttng_kernel_abi_counter_conf {
	uint32_t len;			/* length of this structure */
	uint32_t flags;			/* enum lttng_kernel_abi_counter_conf_flags */
	uint32_t arithmetic;		/* enum lttng_kernel_abi_counter_arithmetic */
	uint32_t bitness;		/* enum lttng_kernel_abi_counter_bitness */
	int64_t global_sum_step;
	struct lttng_kernel_abi_counter_dimension_array dimension_array;
} __attribute__((packed));

struct lttng_kernel_abi_counter_index {
	uint32_t number_dimensions;
	uint64_t ptr;			/* pointer to dimension indexes (array of uint64_t) */
} __attribute__((packed));

enum lttng_kernel_abi_counter_value_flags {
	LTTNG_KERNEL_ABI_COUNTER_VALUE_FLAG_UNDERFLOW = (1 << 0),
	LTTNG_KERNEL_ABI_COUNTER_VALUE_FLAG_OVERFLOW = (1 << 1),
};

struct lttng_kernel_abi_counter_value {
	int64_t value;
	uint32_t flags;			/* enum lttng_kernel_abi_counter_value_flags */
} __attribute__((packed));

struct lttng_kernel_abi_counter_read {
	uint32_t len;			/* length of this structure */
	struct lttng_kernel_abi_counter_index index;
	int32_t cpu;	/* -1 for global counter, >= 0 for specific cpu. */
	struct lttng_kernel_abi_counter_value value;	/* output */
} __attribute__((packed));

struct lttng_kernel_abi_counter_aggregate {
	uint32_t len;			/* length of this structure */
	struct lttng_kernel_abi_counter_index index;
	struct lttng_kernel_abi_counter_value value;	/* output */
} __attribute__((packed));

struct lttng_kernel_abi_counter_clear {
	uint32_t len;			/* length of this structure */
	struct lttng_kernel_abi_counter_index index;
} __attribute__((packed));

struct lttng_kernel_abi_tracer_version {
	uint32_t major;
	uint32_t minor;
	uint32_t patchlevel;
} __attribute__((packed));

struct lttng_kernel_abi_tracer_abi_version {
	uint32_t major;
	uint32_t minor;
} __attribute__((packed));

struct lttng_kernel_abi_session_name {
	char name[LTTNG_KERNEL_ABI_SESSION_NAME_LEN];
} __attribute__((packed));

struct lttng_kernel_abi_session_creation_time {
	char iso8601[LTTNG_KERNEL_ABI_SESSION_CREATION_TIME_ISO8601_LEN];
} __attribute__((packed));

enum lttng_kernel_abi_calibrate_type {
	LTTNG_KERNEL_ABI_CALIBRATE_KRETPROBE,
};

struct lttng_kernel_abi_calibrate {
	uint32_t type;	/* enum lttng_kernel_abi_calibrate_type (input) */
} __attribute__((packed));

struct lttng_kernel_abi_syscall_mask {
	uint32_t len;	/* in bits */
	char mask[];
} __attribute__((packed));

enum lttng_kernel_abi_context_type {
	LTTNG_KERNEL_ABI_CONTEXT_PID		= 0,
	LTTNG_KERNEL_ABI_CONTEXT_PERF_COUNTER	= 1,
	LTTNG_KERNEL_ABI_CONTEXT_PROCNAME	= 2,
	LTTNG_KERNEL_ABI_CONTEXT_PRIO		= 3,
	LTTNG_KERNEL_ABI_CONTEXT_NICE		= 4,
	LTTNG_KERNEL_ABI_CONTEXT_VPID		= 5,
	LTTNG_KERNEL_ABI_CONTEXT_TID		= 6,
	LTTNG_KERNEL_ABI_CONTEXT_VTID		= 7,
	LTTNG_KERNEL_ABI_CONTEXT_PPID		= 8,
	LTTNG_KERNEL_ABI_CONTEXT_VPPID		= 9,
	LTTNG_KERNEL_ABI_CONTEXT_HOSTNAME	= 10,
	LTTNG_KERNEL_ABI_CONTEXT_CPU_ID		= 11,
	LTTNG_KERNEL_ABI_CONTEXT_INTERRUPTIBLE	= 12,
	LTTNG_KERNEL_ABI_CONTEXT_PREEMPTIBLE	= 13,
	LTTNG_KERNEL_ABI_CONTEXT_NEED_RESCHEDULE = 14,
	LTTNG_KERNEL_ABI_CONTEXT_MIGRATABLE	= 15,
	LTTNG_KERNEL_ABI_CONTEXT_CALLSTACK_KERNEL = 16,
	LTTNG_KERNEL_ABI_CONTEXT_CALLSTACK_USER	= 17,
	LTTNG_KERNEL_ABI_CONTEXT_CGROUP_NS	= 18,
	LTTNG_KERNEL_ABI_CONTEXT_IPC_NS		= 19,
	LTTNG_KERNEL_ABI_CONTEXT_MNT_NS		= 20,
	LTTNG_KERNEL_ABI_CONTEXT_NET_NS		= 21,
	LTTNG_KERNEL_ABI_CONTEXT_PID_NS		= 22,
	LTTNG_KERNEL_ABI_CONTEXT_USER_NS	= 23,
	LTTNG_KERNEL_ABI_CONTEXT_UTS_NS		= 24,
	LTTNG_KERNEL_ABI_CONTEXT_UID		= 25,
	LTTNG_KERNEL_ABI_CONTEXT_EUID		= 26,
	LTTNG_KERNEL_ABI_CONTEXT_SUID		= 27,
	LTTNG_KERNEL_ABI_CONTEXT_GID		= 28,
	LTTNG_KERNEL_ABI_CONTEXT_EGID		= 29,
	LTTNG_KERNEL_ABI_CONTEXT_SGID		= 30,
	LTTNG_KERNEL_ABI_CONTEXT_VUID		= 31,
	LTTNG_KERNEL_ABI_CONTEXT_VEUID		= 32,
	LTTNG_KERNEL_ABI_CONTEXT_VSUID		= 33,
	LTTNG_KERNEL_ABI_CONTEXT_VGID		= 34,
	LTTNG_KERNEL_ABI_CONTEXT_VEGID		= 35,
	LTTNG_KERNEL_ABI_CONTEXT_VSGID		= 36,
	LTTNG_KERNEL_ABI_CONTEXT_TIME_NS	= 37,
};

struct lttng_kernel_abi_perf_counter_ctx {
	uint32_t type;
	uint64_t config;
	char name[LTTNG_KERNEL_ABI_SYM_NAME_LEN];
} __attribute__((packed));

#define LTTNG_KERNEL_ABI_CONTEXT_PADDING1	16
#define LTTNG_KERNEL_ABI_CONTEXT_PADDING2	LTTNG_KERNEL_ABI_SYM_NAME_LEN + 32
struct lttng_kernel_abi_context {
	uint32_t ctx;	/*enum lttng_kernel_abi_context_type */
	char padding[LTTNG_KERNEL_ABI_CONTEXT_PADDING1];

	union {
		struct lttng_kernel_abi_perf_counter_ctx perf_counter;
		char padding[LTTNG_KERNEL_ABI_CONTEXT_PADDING2];
	} u;
} __attribute__((packed));

#define LTTNG_KERNEL_ABI_FILTER_BYTECODE_MAX_LEN		65536
struct lttng_kernel_abi_filter_bytecode {
	uint32_t len;
	uint32_t reloc_offset;
	uint64_t seqnum;
	char data[];
} __attribute__((packed));

#define LTTNG_KERNEL_ABI_CAPTURE_BYTECODE_MAX_LEN		65536
struct lttng_kernel_abi_capture_bytecode {
	uint32_t len;
	uint32_t reloc_offset;
	uint64_t seqnum;
	char data[];
} __attribute__((packed));

enum lttng_kernel_abi_tracker_type {
	LTTNG_KERNEL_ABI_TRACKER_UNKNOWN	= -1,

	LTTNG_KERNEL_ABI_TRACKER_PID		= 0,
	LTTNG_KERNEL_ABI_TRACKER_VPID		= 1,
	LTTNG_KERNEL_ABI_TRACKER_UID		= 2,
	LTTNG_KERNEL_ABI_TRACKER_VUID		= 3,
	LTTNG_KERNEL_ABI_TRACKER_GID		= 4,
	LTTNG_KERNEL_ABI_TRACKER_VGID		= 5,
};

struct lttng_kernel_abi_tracker_args {
	uint32_t type;	/* enum lttng_kernel_abi_tracker_type */
	int32_t id;
};

/* LTTng file descriptor ioctl */
/* lttng/abi-old.h reserve 0x40, 0x41, 0x42, 0x43, and 0x44. */
#define LTTNG_KERNEL_ABI_SESSION			_IO(0xF6, 0x45)
#define LTTNG_KERNEL_ABI_TRACER_VERSION		\
	_IOR(0xF6, 0x46, struct lttng_kernel_abi_tracer_version)
#define LTTNG_KERNEL_ABI_TRACEPOINT_LIST		_IO(0xF6, 0x47)
#define LTTNG_KERNEL_ABI_WAIT_QUIESCENT		_IO(0xF6, 0x48)
#define LTTNG_KERNEL_ABI_CALIBRATE			\
	_IOWR(0xF6, 0x49, struct lttng_kernel_abi_calibrate)
#define LTTNG_KERNEL_ABI_SYSCALL_LIST		_IO(0xF6, 0x4A)
#define LTTNG_KERNEL_ABI_TRACER_ABI_VERSION		\
	_IOR(0xF6, 0x4B, struct lttng_kernel_abi_tracer_abi_version)
#define LTTNG_KERNEL_ABI_EVENT_NOTIFIER_GROUP_CREATE    _IO(0xF6, 0x4C)

/* Session FD ioctl */
/* lttng/abi-old.h reserve 0x50, 0x51, 0x52, and 0x53. */
#define LTTNG_KERNEL_ABI_METADATA			\
	_IOW(0xF6, 0x54, struct lttng_kernel_abi_channel)
#define LTTNG_KERNEL_ABI_CHANNEL			\
	_IOW(0xF6, 0x55, struct lttng_kernel_abi_channel)
#define LTTNG_KERNEL_ABI_SESSION_START		_IO(0xF6, 0x56)
#define LTTNG_KERNEL_ABI_SESSION_STOP		_IO(0xF6, 0x57)
#define LTTNG_KERNEL_ABI_SESSION_TRACK_PID		\
	_IOW(0xF6, 0x58, int32_t)
#define LTTNG_KERNEL_ABI_SESSION_UNTRACK_PID	\
	_IOW(0xF6, 0x59, int32_t)

/*
 * ioctl 0x58 and 0x59 are duplicated here. It works, since _IOR vs _IO
 * are generating two different ioctl numbers, but this was not done on
 * purpose. We should generally try to avoid those duplications.
 */
#define LTTNG_KERNEL_ABI_SESSION_LIST_TRACKER_PIDS	_IO(0xF6, 0x58)
#define LTTNG_KERNEL_ABI_SESSION_METADATA_REGEN	_IO(0xF6, 0x59)

/* lttng/abi-old.h reserve 0x5A and 0x5B. */
#define LTTNG_KERNEL_ABI_SESSION_STATEDUMP		_IO(0xF6, 0x5C)
#define LTTNG_KERNEL_ABI_SESSION_SET_NAME		\
	_IOW(0xF6, 0x5D, struct lttng_kernel_abi_session_name)
#define LTTNG_KERNEL_ABI_SESSION_SET_CREATION_TIME		\
	_IOW(0xF6, 0x5E, struct lttng_kernel_abi_session_creation_time)

/* Channel FD ioctl */
/* lttng/abi-old.h reserve 0x60 and 0x61. */
#define LTTNG_KERNEL_ABI_STREAM			_IO(0xF6, 0x62)
#define LTTNG_KERNEL_ABI_EVENT			\
	_IOW(0xF6, 0x63, struct lttng_kernel_abi_event)
/* LTTNG_KERNEL_ABI_SYSCALL_MASK applies to both channel and counter fds. */
#define LTTNG_KERNEL_ABI_SYSCALL_MASK		\
	_IOWR(0xF6, 0x64, struct lttng_kernel_abi_syscall_mask)

/* Event and Channel FD ioctl */
/* lttng/abi-old.h reserve 0x70. */
#define LTTNG_KERNEL_ABI_CONTEXT			\
	_IOW(0xF6, 0x71, struct lttng_kernel_abi_context)

/* Event, Event notifier, Channel, Counter and Session ioctl */
/* lttng/abi-old.h reserve 0x80 and 0x81. */
#define LTTNG_KERNEL_ABI_ENABLE			_IO(0xF6, 0x82)
#define LTTNG_KERNEL_ABI_DISABLE			_IO(0xF6, 0x83)

/* Event notifier group and session ioctl */

/* (0xF6, 0x84) is reserved for old ABI. */

#define LTTNG_KERNEL_ABI_COUNTER \
	_IOW(0xF6, 0x85, struct lttng_kernel_abi_counter_conf)

/* Event and Event notifier FD ioctl */
#define LTTNG_KERNEL_ABI_FILTER			_IO(0xF6, 0x90)
#define LTTNG_KERNEL_ABI_ADD_CALLSITE		_IO(0xF6, 0x91)

/* Session FD ioctl (continued) */
#define LTTNG_KERNEL_ABI_SESSION_LIST_TRACKER_IDS	\
	_IOW(0xF6, 0xA0, struct lttng_kernel_abi_tracker_args)
#define LTTNG_KERNEL_ABI_SESSION_TRACK_ID		\
	_IOW(0xF6, 0xA1, struct lttng_kernel_abi_tracker_args)
#define LTTNG_KERNEL_ABI_SESSION_UNTRACK_ID		\
	_IOW(0xF6, 0xA2, struct lttng_kernel_abi_tracker_args)

/* Event notifier group file descriptor ioctl */
#define LTTNG_KERNEL_ABI_EVENT_NOTIFIER_CREATE \
	_IOW(0xF6, 0xB0, struct lttng_kernel_abi_event_notifier)
#define LTTNG_KERNEL_ABI_EVENT_NOTIFIER_GROUP_NOTIFICATION_FD \
	_IO(0xF6, 0xB1)

/* Event notifier file descriptor ioctl */
#define LTTNG_KERNEL_ABI_CAPTURE			_IO(0xF6, 0xB8)

/* (0xF6, {0xC0, 0xC1, 0xC2}) are reserved for old ABI. */

/* Counter file descriptor ioctl */
#define LTTNG_KERNEL_ABI_COUNTER_MAP_NR_DESCRIPTORS \
	_IOR(0xF6, 0xC3, uint64_t)
#define LTTNG_KERNEL_ABI_COUNTER_MAP_DESCRIPTOR \
	_IOWR(0xF6, 0xC4, struct lttng_kernel_abi_counter_map_descriptor)
#define LTTNG_KERNEL_ABI_COUNTER_EVENT \
	_IOW(0xF6, 0xC5, struct lttng_kernel_abi_counter_event)
#define LTTNG_KERNEL_ABI_COUNTER_READ \
	_IOWR(0xF6, 0xC6, struct lttng_kernel_abi_counter_read)
#define LTTNG_KERNEL_ABI_COUNTER_AGGREGATE \
	_IOWR(0xF6, 0xC7, struct lttng_kernel_abi_counter_aggregate)
#define LTTNG_KERNEL_ABI_COUNTER_CLEAR \
	_IOW(0xF6, 0xC8, struct lttng_kernel_abi_counter_clear)

/*
 * LTTng-specific ioctls for the lib ringbuffer.
 *
 * Operations applying to the current sub-buffer need to occur between
 * a get/put or get_next/put_next ioctl pair.
 */

/* returns the timestamp begin of the current sub-buffer */
#define LTTNG_KERNEL_ABI_RING_BUFFER_GET_TIMESTAMP_BEGIN	_IOR(0xF6, 0x20, uint64_t)
/* returns the timestamp end of the current sub-buffer */
#define LTTNG_KERNEL_ABI_RING_BUFFER_GET_TIMESTAMP_END		_IOR(0xF6, 0x21, uint64_t)
/* returns the number of events discarded of the current sub-buffer */
#define LTTNG_KERNEL_ABI_RING_BUFFER_GET_EVENTS_DISCARDED	_IOR(0xF6, 0x22, uint64_t)
/* returns the packet payload size of the current sub-buffer */
#define LTTNG_KERNEL_ABI_RING_BUFFER_GET_CONTENT_SIZE		_IOR(0xF6, 0x23, uint64_t)
/* returns the packet size of the current sub-buffer*/
#define LTTNG_KERNEL_ABI_RING_BUFFER_GET_PACKET_SIZE		_IOR(0xF6, 0x24, uint64_t)
/* returns the stream id (invariant for the stream) */
#define LTTNG_KERNEL_ABI_RING_BUFFER_GET_STREAM_ID		_IOR(0xF6, 0x25, uint64_t)
/* returns the current timestamp as perceived from the tracer */
#define LTTNG_KERNEL_ABI_RING_BUFFER_GET_CURRENT_TIMESTAMP	_IOR(0xF6, 0x26, uint64_t)
/* returns the packet sequence number of the current sub-buffer */
#define LTTNG_KERNEL_ABI_RING_BUFFER_GET_SEQ_NUM		_IOR(0xF6, 0x27, uint64_t)
/* returns the stream instance id (invariant for the stream) */
#define LTTNG_KERNEL_ABI_RING_BUFFER_INSTANCE_ID		_IOR(0xF6, 0x28, uint64_t)

/*
 * Those ioctl numbers use the wrong direction, but are kept for ABI backward
 * compatibility.
 */
#define LTTNG_KERNEL_ABI_OLD_SESSION_SET_NAME		\
	_IOR(0xF6, 0x5D, struct lttng_kernel_abi_session_name)
#define LTTNG_KERNEL_ABI_OLD_SESSION_SET_CREATION_TIME	\
	_IOR(0xF6, 0x5E, struct lttng_kernel_abi_session_creation_time)
#define LTTNG_KERNEL_ABI_OLD_SESSION_TRACK_PID		\
	_IOR(0xF6, 0x58, int32_t)
#define LTTNG_KERNEL_ABI_OLD_SESSION_UNTRACK_PID	\
	_IOR(0xF6, 0x59, int32_t)
#define LTTNG_KERNEL_ABI_OLD_SESSION_LIST_TRACKER_IDS	\
	_IOR(0xF6, 0xA0, struct lttng_kernel_abi_tracker_args)
#define LTTNG_KERNEL_ABI_OLD_SESSION_TRACK_ID		\
	_IOR(0xF6, 0xA1, struct lttng_kernel_abi_tracker_args)
#define LTTNG_KERNEL_ABI_OLD_SESSION_UNTRACK_ID		\
	_IOR(0xF6, 0xA2, struct lttng_kernel_abi_tracker_args)

#ifdef CONFIG_COMPAT
/* returns the timestamp begin of the current sub-buffer */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_TIMESTAMP_BEGIN \
	LTTNG_KERNEL_ABI_RING_BUFFER_GET_TIMESTAMP_BEGIN
/* returns the timestamp end of the current sub-buffer */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_TIMESTAMP_END \
	LTTNG_KERNEL_ABI_RING_BUFFER_GET_TIMESTAMP_END
/* returns the number of events discarded of the current sub-buffer */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_EVENTS_DISCARDED \
	LTTNG_KERNEL_ABI_RING_BUFFER_GET_EVENTS_DISCARDED
/* returns the packet payload size of the current sub-buffer */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_CONTENT_SIZE \
	LTTNG_KERNEL_ABI_RING_BUFFER_GET_CONTENT_SIZE
/* returns the packet size of the current sub-buffer */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_PACKET_SIZE \
	LTTNG_KERNEL_ABI_RING_BUFFER_GET_PACKET_SIZE
/* returns the stream id (invariant for the stream) */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_STREAM_ID \
	LTTNG_KERNEL_ABI_RING_BUFFER_GET_STREAM_ID
/* returns the current timestamp as perceived from the tracer */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_CURRENT_TIMESTAMP \
	LTTNG_KERNEL_ABI_RING_BUFFER_GET_CURRENT_TIMESTAMP
/* returns the packet sequence number of the current sub-buffer */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_SEQ_NUM	\
	LTTNG_KERNEL_ABI_RING_BUFFER_GET_SEQ_NUM
/* returns the stream instance id (invariant for the stream) */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_INSTANCE_ID	\
	LTTNG_KERNEL_ABI_RING_BUFFER_INSTANCE_ID
#endif /* CONFIG_COMPAT */

#endif /* _LTTNG_ABI_H */
