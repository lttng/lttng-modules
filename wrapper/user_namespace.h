/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/user_namespace.h
 *
 * Copyright (C) 2019 Michael Jeanson <mjeanson@efficios.com>
 *
 */

#ifndef _LTTNG_WRAPPER_USER_NAMESPACE_H
#define _LTTNG_WRAPPER_USER_NAMESPACE_H

#include <linux/version.h>
#include <linux/user_namespace.h>


#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 5, 0)

#define lttng_current_xxuid(xxx)				\
	(from_kuid_munged(&init_user_ns, current_##xxx()))

#define lttng_current_vxxuid(xxx)				\
	(from_kuid_munged(current_user_ns(), current_##xxx()))

#define lttng_current_xxgid(xxx)				\
	(from_kgid_munged(&init_user_ns, current_##xxx()))

#define lttng_current_vxxgid(xxx)				\
	(from_kgid_munged(current_user_ns(), current_##xxx()))

#else

#define lttng_current_xxuid(xxx)	(current_##xxx())

#define lttng_current_vxxuid(xxx)					\
	(user_ns_map_uid(current_user_ns(), current_cred(), current_##xxx()))

#define lttng_current_xxgid(xxx)	(current_##xxx())

#define lttng_current_vxxgid(xxx)					\
	(user_ns_map_gid(current_user_ns(), current_cred(), current_##xxx()))
#endif

#define lttng_current_uid()	(lttng_current_xxuid(uid))
#define lttng_current_euid()	(lttng_current_xxuid(euid))
#define lttng_current_suid()	(lttng_current_xxuid(suid))
#define lttng_current_fsuid()	(lttng_current_xxuid(fsuid))
#define lttng_current_gid()	(lttng_current_xxgid(gid))
#define lttng_current_egid()	(lttng_current_xxgid(egid))
#define lttng_current_sgid()	(lttng_current_xxgid(sgid))
#define lttng_current_fsgid()	(lttng_current_xxgid(fsgid))

#define lttng_current_vuid()	(lttng_current_vxxuid(uid))
#define lttng_current_veuid()	(lttng_current_vxxuid(euid))
#define lttng_current_vsuid()	(lttng_current_vxxuid(suid))
#define lttng_current_vfsuid()	(lttng_current_vxxuid(fsuid))
#define lttng_current_vgid()	(lttng_current_vxxgid(gid))
#define lttng_current_vegid()	(lttng_current_vxxgid(egid))
#define lttng_current_vsgid()	(lttng_current_vxxgid(sgid))
#define lttng_current_vfsgid()	(lttng_current_vxxgid(fsgid))

#endif /* _LTTNG_WRAPPER_USER_NAMESPACE_H */
