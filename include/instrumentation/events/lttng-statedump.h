/* SPDX-License-Identifier: GPL-2.0-only */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM lttng_statedump

#if !defined(LTTNG_TRACE_LTTNG_STATEDUMP_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_LTTNG_STATEDUMP_H

#include <lttng/tracepoint-event.h>
#include <linux/nsproxy.h>
#include <linux/cgroup.h>
#include <linux/ipc_namespace.h>
#include <net/net_namespace.h>
#include <linux/pid_namespace.h>
#include <linux/user_namespace.h>
#include <linux/utsname.h>
#include <linux/types.h>
#include <linux/user_namespace.h>

#include <lttng/kernel-version.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,6,0) || \
	LTTNG_RHEL_KERNEL_RANGE(4,18,0,305,0,0, 4,19,0,0,0,0))
#include <linux/time_namespace.h>
#endif

#ifndef LTTNG_MNT_NS_MISSING_HEADER
# ifndef ONCE_LTTNG_FS_MOUNT_H
#  define ONCE_LTTNG_FS_MOUNT_H
#  include <../fs/mount.h>
# endif
#endif

LTTNG_TRACEPOINT_EVENT(lttng_statedump_start,
	TP_PROTO(struct lttng_kernel_session *session),
	TP_ARGS(session),
	TP_FIELDS()
)

LTTNG_TRACEPOINT_EVENT(lttng_statedump_end,
	TP_PROTO(struct lttng_kernel_session *session),
	TP_ARGS(session),
	TP_FIELDS()
)

LTTNG_TRACEPOINT_EVENT(lttng_statedump_process_state,
	TP_PROTO(struct lttng_kernel_session *session,
		struct task_struct *p,
		int type, int mode, int submode, int status,
		struct files_struct *files),
	TP_ARGS(session, p, type, mode, submode, status, files),
	TP_FIELDS(
		ctf_integer(pid_t, tid, p->pid)
		ctf_integer(pid_t, pid, p->tgid)
		ctf_integer(pid_t, ppid,
			({
				pid_t ret;

				rcu_read_lock();
				ret = task_tgid_nr(p->real_parent);
				rcu_read_unlock();
				ret;
			}))
		ctf_array_text(char, name, p->comm, TASK_COMM_LEN)
		ctf_integer(int, type, type)
		ctf_integer(int, mode, mode)
		ctf_integer(int, submode, submode)
		ctf_integer(int, status, status)
		ctf_integer(unsigned int, cpu, task_cpu(p))
		ctf_integer_hex(struct files_struct *, file_table_address, files)
	)
)

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,6,0))
LTTNG_TRACEPOINT_EVENT(lttng_statedump_process_cgroup_ns,
	TP_PROTO(struct lttng_kernel_session *session,
		struct task_struct *p,
		struct cgroup_namespace *cgroup_ns),
	TP_ARGS(session, p, cgroup_ns),
	TP_FIELDS(
		ctf_integer(pid_t, tid, p->pid)
		ctf_integer(unsigned int, ns_inum, cgroup_ns ? cgroup_ns->ns.inum : 0)
	)
)
#endif

LTTNG_TRACEPOINT_EVENT(lttng_statedump_process_ipc_ns,
	TP_PROTO(struct lttng_kernel_session *session,
		struct task_struct *p,
		struct ipc_namespace *ipc_ns),
	TP_ARGS(session, p, ipc_ns),
	TP_FIELDS(
		ctf_integer(pid_t, tid, p->pid)
		ctf_integer(unsigned int, ns_inum, ipc_ns ? ipc_ns->ns.inum : 0)
	)
)

#if !defined(LTTNG_MNT_NS_MISSING_HEADER)
LTTNG_TRACEPOINT_EVENT(lttng_statedump_process_mnt_ns,
	TP_PROTO(struct lttng_kernel_session *session,
		struct task_struct *p,
		struct mnt_namespace *mnt_ns),
	TP_ARGS(session, p, mnt_ns),
	TP_FIELDS(
		ctf_integer(pid_t, tid, p->pid)
		ctf_integer(unsigned int, ns_inum, mnt_ns ? mnt_ns->ns.inum : 0)
	)
)
#endif

LTTNG_TRACEPOINT_EVENT(lttng_statedump_process_net_ns,
	TP_PROTO(struct lttng_kernel_session *session,
		struct task_struct *p,
		struct net *net_ns),
	TP_ARGS(session, p, net_ns),
	TP_FIELDS(
		ctf_integer(pid_t, tid, p->pid)
		ctf_integer(unsigned int, ns_inum, net_ns ? net_ns->ns.inum : 0)
	)
)

LTTNG_TRACEPOINT_EVENT(lttng_statedump_process_pid_ns,
	TP_PROTO(struct lttng_kernel_session *session,
		struct task_struct *p,
		struct pid_namespace *pid_ns),
	TP_ARGS(session, p, pid_ns),
	TP_FIELDS(
		ctf_integer(pid_t, tid, p->pid)
		ctf_integer(pid_t, vtid, pid_ns ? task_pid_nr_ns(p, pid_ns) : 0)
		ctf_integer(pid_t, vpid, pid_ns ? task_tgid_nr_ns(p, pid_ns) : 0)
		ctf_integer(pid_t, vppid,
			({
				struct task_struct *parent;
				pid_t ret = 0;

				if (pid_ns) {
					rcu_read_lock();
					parent = rcu_dereference(p->real_parent);
					ret = task_tgid_nr_ns(parent, pid_ns);
					rcu_read_unlock();
				}
				ret;
			}))
		ctf_integer(int, ns_level, pid_ns ? pid_ns->level : 0)
		ctf_integer(unsigned int, ns_inum, pid_ns ? pid_ns->ns.inum : 0)
	)
)

LTTNG_TRACEPOINT_EVENT(lttng_statedump_process_user_ns,
	TP_PROTO(struct lttng_kernel_session *session,
		struct task_struct *p,
		struct user_namespace *user_ns),
	TP_ARGS(session, p, user_ns),
	TP_FIELDS(
		ctf_integer(pid_t, tid, p->pid)
		ctf_integer(uid_t, vuid, user_ns ? from_kuid_munged(user_ns, task_cred_xxx(p, uid)) : 0)
		ctf_integer(gid_t, vgid, user_ns ? from_kgid_munged(user_ns, task_cred_xxx(p, gid)) : 0)
		ctf_integer(int, ns_level, user_ns ? user_ns->level : 0)
		ctf_integer(unsigned int, ns_inum, user_ns ? user_ns->ns.inum : 0)
	)
)

LTTNG_TRACEPOINT_EVENT(lttng_statedump_process_uts_ns,
	TP_PROTO(struct lttng_kernel_session *session,
		struct task_struct *p,
		struct uts_namespace *uts_ns),
	TP_ARGS(session, p, uts_ns),
	TP_FIELDS(
		ctf_integer(pid_t, tid, p->pid)
		ctf_integer(unsigned int, ns_inum, uts_ns ? uts_ns->ns.inum : 0)
	)
)

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,6,0) || \
	LTTNG_RHEL_KERNEL_RANGE(4,18,0,305,0,0, 4,19,0,0,0,0))
LTTNG_TRACEPOINT_EVENT(lttng_statedump_process_time_ns,
	TP_PROTO(struct lttng_kernel_session *session,
		struct task_struct *p,
		struct time_namespace *time_ns),
	TP_ARGS(session, p, time_ns),
	TP_FIELDS(
		ctf_integer(pid_t, tid, p->pid)
		ctf_integer(unsigned int, ns_inum, time_ns ? time_ns->ns.inum : 0)
	)
)
#endif

LTTNG_TRACEPOINT_EVENT(lttng_statedump_file_descriptor,
	TP_PROTO(struct lttng_kernel_session *session,
		struct files_struct *files,
		int fd, const char *filename,
		unsigned int flags, fmode_t fmode),
	TP_ARGS(session, files, fd, filename, flags, fmode),
	TP_FIELDS(
		ctf_integer_hex(struct files_struct *, file_table_address, files)
		ctf_integer(int, fd, fd)
		ctf_integer_oct(unsigned int, flags, flags)
		ctf_integer_hex(fmode_t, fmode, fmode)
		ctf_string(filename, filename)
	)
)

LTTNG_TRACEPOINT_EVENT(lttng_statedump_vm_map,
	TP_PROTO(struct lttng_kernel_session *session,
		struct task_struct *p, struct vm_area_struct *map,
		unsigned long inode),
	TP_ARGS(session, p, map, inode),
	TP_FIELDS(
		ctf_integer(pid_t, pid, p->tgid)
		ctf_integer_hex(unsigned long, start, map->vm_start)
		ctf_integer_hex(unsigned long, end, map->vm_end)
		ctf_integer_hex(unsigned long, flags, map->vm_flags)
		ctf_integer(unsigned long, inode, inode)
		ctf_integer(unsigned long, pgoff, map->vm_pgoff << PAGE_SHIFT)
	)
)

LTTNG_TRACEPOINT_EVENT(lttng_statedump_network_interface,
	TP_PROTO(struct lttng_kernel_session *session,
		struct net_device *dev, struct in_ifaddr *ifa),
	TP_ARGS(session, dev, ifa),
	TP_FIELDS(
		ctf_string(name, dev->name)
		ctf_integer_network_hex(uint32_t, address_ipv4,
			ifa ? ifa->ifa_address : 0U)
	)
)

LTTNG_TRACEPOINT_EVENT(lttng_statedump_block_device,
	TP_PROTO(struct lttng_kernel_session *session,
		dev_t dev, const char *diskname),
	TP_ARGS(session, dev, diskname),
	TP_FIELDS(
		ctf_integer(dev_t, dev, dev)
		ctf_string(diskname, diskname)
	)
)

/* Called with desc->lock held */
LTTNG_TRACEPOINT_EVENT(lttng_statedump_interrupt,
	TP_PROTO(struct lttng_kernel_session *session,
		unsigned int irq, const char *chip_name,
		struct irqaction *action),
	TP_ARGS(session, irq, chip_name, action),
	TP_FIELDS(
		ctf_integer(unsigned int, irq, irq)
		ctf_string(name, chip_name)
		ctf_string(action, action->name ? : "")
		ctf_integer(pid_t, tid, action->thread ? action->thread->pid : 0)
	)
)

#if defined(CONFIG_X86_32) || defined(CONFIG_X86_64)

#define LTTNG_HAVE_STATEDUMP_CPU_TOPOLOGY

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,7,0) || \
	LTTNG_RHEL_KERNEL_RANGE(5,14,0,452,0,0, 5,15,0,0,0,0))
LTTNG_TRACEPOINT_EVENT(lttng_statedump_cpu_topology,
	TP_PROTO(struct lttng_kernel_session *session, struct cpuinfo_x86 *c),
	TP_ARGS(session, c),
	TP_FIELDS(
		ctf_string(architecture, "x86")
		ctf_integer(uint16_t, cpu_id, c->cpu_index)
		ctf_string(vendor, c->x86_vendor_id[0] ? c->x86_vendor_id : "unknown")
		ctf_integer(uint8_t, family, c->x86)
		ctf_integer(uint8_t, model, c->x86_model)
		ctf_string(model_name, c->x86_model_id[0] ? c->x86_model_id : "unknown")
		ctf_integer(uint16_t, physical_id, c->topo.pkg_id)
		ctf_integer(uint16_t, core_id, c->topo.core_id)
		ctf_integer(uint16_t, cores, c->booted_cores)
	)
)
#else
LTTNG_TRACEPOINT_EVENT(lttng_statedump_cpu_topology,
	TP_PROTO(struct lttng_kernel_session *session, struct cpuinfo_x86 *c),
	TP_ARGS(session, c),
	TP_FIELDS(
		ctf_string(architecture, "x86")
		ctf_integer(uint16_t, cpu_id, c->cpu_index)
		ctf_string(vendor, c->x86_vendor_id[0] ? c->x86_vendor_id : "unknown")
		ctf_integer(uint8_t, family, c->x86)
		ctf_integer(uint8_t, model, c->x86_model)
		ctf_string(model_name, c->x86_model_id[0] ? c->x86_model_id : "unknown")
		ctf_integer(uint16_t, physical_id, c->phys_proc_id)
		ctf_integer(uint16_t, core_id, c->cpu_core_id)
		ctf_integer(uint16_t, cores, c->booted_cores)
	)
)
#endif /* LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,7,0) */

#endif /* CONFIG_X86_32 || CONFIG_X86_64 */

#endif /*  LTTNG_TRACE_LTTNG_STATEDUMP_H */

/* This part must be outside protection */
#include <lttng/define_trace.h>
