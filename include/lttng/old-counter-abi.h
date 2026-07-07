/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 * Copyright (C) 2022 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * The counter ioctls exposed in 2.13 for notifications used a fixed-size array
 * for the number of dimensions. This was replaced by variable length arrays in
 * 2.14 for event counters. This old ABI is retained for backwards
 * compatibility.
 */

#ifndef _LTTNG_OLD_COUNTER_ABI_H
#define _LTTNG_OLD_COUNTER_ABI_H

#include <linux/ioctl.h>

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


#define LTTNG_KERNEL_ABI_OLD_COUNTER \
	_IOW(0xF6, 0x84, struct lttng_kernel_abi_old_counter_conf)

/* Counter file descriptor ioctl ("old" 2.13 ABI) */
#define LTTNG_KERNEL_ABI_OLD_COUNTER_READ \
	_IOWR(0xF6, 0xC0, struct lttng_kernel_abi_old_counter_read)
#define LTTNG_KERNEL_ABI_OLD_COUNTER_AGGREGATE \
	_IOWR(0xF6, 0xC1, struct lttng_kernel_abi_old_counter_aggregate)
#define LTTNG_KERNEL_ABI_OLD_COUNTER_CLEAR \
	_IOW(0xF6, 0xC2, struct lttng_kernel_abi_old_counter_clear)

#endif /* _LTTNG_OLD_COUNTER_ABI_H */
