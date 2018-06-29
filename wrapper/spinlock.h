/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/spinlock.h
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_SPINLOCK_H
#define _LTTNG_WRAPPER_SPINLOCK_H

#include <linux/version.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33))

#include <linux/string.h>

#define raw_spin_lock_init(lock)					\
	do {								\
		raw_spinlock_t __lock = __RAW_SPIN_LOCK_UNLOCKED;	\
		memcpy(lock, &__lock, sizeof(lock));			\
	} while (0)

#define raw_spin_is_locked(lock)	__raw_spin_is_locked(lock)

#define wrapper_desc_spin_lock(lock)	spin_lock(lock)
#define wrapper_desc_spin_unlock(lock)	spin_unlock(lock)

#else

#define wrapper_desc_spin_lock(lock)	raw_spin_lock(lock)
#define wrapper_desc_spin_unlock(lock)	raw_spin_unlock(lock)

#endif
#endif /* _LTTNG_WRAPPER_SPINLOCK_H */
