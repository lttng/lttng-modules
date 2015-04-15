#undef TRACE_SYSTEM
#define TRACE_SYSTEM lttng_statedump

#if !defined(LTTNG_TRACE_LTTNG_STATEDUMP_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_LTTNG_STATEDUMP_H

#include "../../../probes/lttng-tracepoint-event.h"
#include <linux/nsproxy.h>
#include <linux/pid_namespace.h>
#include <linux/types.h>
#include <linux/version.h>


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0))
#define lttng_proc_inum ns.inum
#else
#define lttng_proc_inum proc_inum
#endif

LTTNG_TRACEPOINT_EVENT(lttng_statedump_start,
	TP_PROTO(struct lttng_session *session),
	TP_ARGS(session),
	TP_FIELDS()
)

LTTNG_TRACEPOINT_EVENT(lttng_statedump_end,
	TP_PROTO(struct lttng_session *session),
	TP_ARGS(session),
	TP_FIELDS()
)

LTTNG_TRACEPOINT_EVENT(lttng_statedump_process_state,
	TP_PROTO(struct lttng_session *session,
		struct task_struct *p,
		int type, int mode, int submode, int status,
		struct pid_namespace *pid_ns),
	TP_ARGS(session, p, type, mode, submode, status, pid_ns),
	TP_FIELDS(
		ctf_integer(pid_t, tid, p->pid)
		ctf_integer(pid_t, vtid, pid_ns ? task_pid_nr_ns(p, pid_ns) : 0)
		ctf_integer(pid_t, pid, p->tgid)
		ctf_integer(pid_t, vpid, pid_ns ? task_tgid_nr_ns(p, pid_ns) : 0)
		ctf_integer(pid_t, ppid,
			({
				pid_t ret;

				rcu_read_lock();
				ret = task_tgid_nr(p->real_parent);
				rcu_read_unlock();
				ret;
			}))
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
		ctf_array_text(char, name, p->comm, TASK_COMM_LEN)
		ctf_integer(int, type, type)
		ctf_integer(int, mode, mode)
		ctf_integer(int, submode, submode)
		ctf_integer(int, status, status)
		ctf_integer(int, ns_level, pid_ns ? pid_ns->level : 0)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0))
		ctf_integer(unsigned int, ns_inum, pid_ns ? pid_ns->lttng_proc_inum : 0)
#endif
	)
)

LTTNG_TRACEPOINT_EVENT(lttng_statedump_file_descriptor,
	TP_PROTO(struct lttng_session *session,
		struct task_struct *p, int fd, const char *filename,
		unsigned int flags, fmode_t fmode),
	TP_ARGS(session, p, fd, filename, flags, fmode),
	TP_FIELDS(
		ctf_integer(pid_t, pid, p->tgid)
		ctf_integer(int, fd, fd)
		ctf_integer_oct(unsigned int, flags, flags)
		ctf_integer_hex(fmode_t, fmode, fmode)
		ctf_string(filename, filename)
	)
)

LTTNG_TRACEPOINT_EVENT(lttng_statedump_vm_map,
	TP_PROTO(struct lttng_session *session,
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
	TP_PROTO(struct lttng_session *session,
		struct net_device *dev, struct in_ifaddr *ifa),
	TP_ARGS(session, dev, ifa),
	TP_FIELDS(
		ctf_string(name, dev->name)
		ctf_integer_network_hex(uint32_t, address_ipv4,
			ifa ? ifa->ifa_address : 0U)
	)
)

LTTNG_TRACEPOINT_EVENT(lttng_statedump_block_device,
	TP_PROTO(struct lttng_session *session,
		dev_t dev, const char *diskname),
	TP_ARGS(session, dev, diskname),
	TP_FIELDS(
		ctf_integer(dev_t, dev, dev)
		ctf_string(diskname, diskname)
	)
)

/* Called with desc->lock held */
LTTNG_TRACEPOINT_EVENT(lttng_statedump_interrupt,
	TP_PROTO(struct lttng_session *session,
		unsigned int irq, const char *chip_name,
		struct irqaction *action),
	TP_ARGS(session, irq, chip_name, action),
	TP_FIELDS(
		ctf_integer(unsigned int, irq, irq)
		ctf_string(name, chip_name)
		ctf_string(action, action->name ? : "")
	)
)

#endif /*  LTTNG_TRACE_LTTNG_STATEDUMP_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
