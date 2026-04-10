// SPDX-FileCopyrightText: 2026 Michael Jeanson <mjeanson@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#ifndef COMMON_SYSCALL_OFF_T_H
#define COMMON_SYSCALL_OFF_T_H

/*
 * On 32bit architecture, loff_t (64bit) parameters have to be split in a pair
 * of 32bit registers.
 */
#ifdef CONFIG_CPU_BIG_ENDIAN
/* Big-endian variant, high bits in the first register. */
#define tp_proto_loff_t(name)	uint32_t name##_high, uint32_t name##_low
#define tp_arg_loff_t(name)	name##_high, name##_low
#else
/* Little-endian variant, low bits in the first register. */
#define tp_proto_loff_t(name)	uint32_t name##_low, uint32_t name##_high
#define tp_arg_loff_t(name)	name##_low, name##_high
#endif

/*
 * Architectures were the calling convention requires an aligned pair of 32bit
 * registers to store a 64bit integer will have an unused padding register if
 * the 64bit argument is odd-numbered (zero-indexed).
 */
#if defined(CONFIG_PPC) || defined(CONFIG_ARM) || defined(CONFIG_ARM64)
#define tp_proto_loff_t_pad(name)	u32 unused_reg, tp_proto_loff_t(name)
#define tp_arg_loff_t_pad(name)		unused_reg, tp_arg_loff_t(name)
#else
#define tp_proto_loff_t_pad(name)	tp_proto_loff_t(name)
#define tp_arg_loff_t_pad(name)		tp_arg_loff_t(name)
#endif

#define tp_field_loff_t(name)	(((loff_t) name##_high << 32) | name##_low)

#endif /* COMMON_SYSCALL_OFF_T_H */
