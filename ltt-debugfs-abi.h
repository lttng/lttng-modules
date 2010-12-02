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

enum instrum_type {
	INSTRUM_TRACEPOINTS,
};

/*
 * LTTng DebugFS ABI structures.
 */

struct lttng_channel {
	int overwrite;			/* 1: overwrite, 0: discard */
	u64 subbuf_size;
	u64 num_subbuf;
	unsigned int switch_timer_interval;
	unsigned int read_timer_interval;
};

struct lttng_event {
	enum instrum_type itype;
	char name[];
};

#define LTTNG_SESSION		_IO(0xF6, 0x40)
#define LTTNG_SESSION_START	_IO(0xF6, 0x41)
#define LTTNG_SESSION_STOP	_IO(0xF6, 0x42)
#define LTTNG_CHANNEL		_IOW(0xF6, 0x43, struct lttng_channel)
#define LTTNG_STREAM		_IO(0xF6, 0x44)
#define LTTNG_EVENT		_IOW(0xF6, 0x45, struct lttng_event)

#endif /* _LTT_DEBUGFS_ABI_H */
