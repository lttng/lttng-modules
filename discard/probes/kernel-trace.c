/*
 * ltt/probes/kernel-trace.c
 *
 * kernel tracepoint probes.
 *
 * (C) Copyright 2009 - Mathieu Desnoyers <mathieu.desnoyers@polymtl.ca>
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <linux/irq.h>
#include <trace/events/signal.h>
#include <trace/irq.h>
#include <trace/sched.h>
#include <trace/timer.h>
#include <trace/kernel.h>
#include <trace/fault.h>
#include <trace/events/sched.h>

#include "../ltt-tracer.h"
#include "../ltt-type-serializer.h"

/*
 * This should probably be added to s390.
 */
#ifdef CONFIG_S390
static struct pt_regs *get_irq_regs(void)
{
	return task_pt_regs(current);
}
#endif

/*
 * FIXME :
 * currently, the specialized tracepoint probes cannot call into other marker
 * probes, such as ftrace enable/disable. Given we want them to be as fast as
 * possible, it might not be so bad to lose this flexibility. But that means
 * such probes would have to connect to tracepoints on their own.
 */

/* kernel_irq_entry specialized tracepoint probe */

void probe_irq_entry(void *_data, unsigned int id, struct pt_regs *regs,
	struct irqaction *action);

DEFINE_MARKER_TP(kernel, irq_entry, irq_entry, probe_irq_entry,
	"ip %lu handler %p irq_id #2u%u kernel_mode #1u%u");

notrace void probe_irq_entry(void *_data, unsigned int id, struct pt_regs *regs,
	struct irqaction *action)
{
	struct marker *marker;
	struct serialize_long_long_short_char data;

	if (unlikely(!regs))
		regs = get_irq_regs();
	if (likely(regs)) {
		data.f1 = instruction_pointer(regs);
		data.f4 = !user_mode(regs);
	} else {
		data.f1 = 0UL;
		data.f4 = 1;
	}
	data.f2 = (unsigned long) (action ? action->handler : NULL);
	data.f3 = id;

	marker = &GET_MARKER(kernel, irq_entry);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, serialize_sizeof(data), sizeof(long));
}

void probe_irq_next_handler(void *_data, unsigned int id, struct irqaction *action,
		irqreturn_t prev_ret);

DEFINE_MARKER_TP(kernel, irq_next_handler, irq_next_handler,
	probe_irq_next_handler,
	"handler %p prev_ret #1u%u");

notrace void probe_irq_next_handler(void *_data, unsigned int id, struct irqaction *action,
		irqreturn_t prev_ret)
{
	struct marker *marker;
	struct serialize_long_char data;

	data.f1 = (unsigned long) (action ? action->handler : NULL);
	data.f2 = prev_ret;

	marker = &GET_MARKER(kernel, irq_next_handler);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, serialize_sizeof(data), sizeof(long));
}

/* kernel_irq_exit specialized tracepoint probe */

void probe_irq_exit(void *_data, irqreturn_t retval);

DEFINE_MARKER_TP(kernel, irq_exit, irq_exit, probe_irq_exit,
	"handled #1u%u");

notrace void probe_irq_exit(void *_data, irqreturn_t retval)
{
	struct marker *marker;
	unsigned char data;

	data = IRQ_RETVAL(retval);

	marker = &GET_MARKER(kernel, irq_exit);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, sizeof(data), sizeof(data));
}

/* kernel_softirq_entry specialized tracepoint probe */

void probe_softirq_entry(void *_data, struct softirq_action *h,
	struct softirq_action *softirq_vec);

DEFINE_MARKER_TP(kernel, softirq_entry, softirq_entry,
	probe_softirq_entry, "softirq_id #1u%lu");

notrace void probe_softirq_entry(void *_data, struct softirq_action *h,
	struct softirq_action *softirq_vec)
{
	struct marker *marker;
	unsigned char data;

	data = ((unsigned long)h - (unsigned long)softirq_vec) / sizeof(*h);

	marker = &GET_MARKER(kernel, softirq_entry);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, sizeof(data), sizeof(data));
}

/* kernel_softirq_exit specialized tracepoint probe */

void probe_softirq_exit(void *_data, struct softirq_action *h,
	struct softirq_action *softirq_vec);

DEFINE_MARKER_TP(kernel, softirq_exit, softirq_exit,
	probe_softirq_exit, "softirq_id #1u%lu");

notrace void probe_softirq_exit(void *_data, struct softirq_action *h,
	struct softirq_action *softirq_vec)
{
	struct marker *marker;
	unsigned char data;

	data = ((unsigned long)h - (unsigned long)softirq_vec) / sizeof(*h);

	marker = &GET_MARKER(kernel, softirq_exit);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, sizeof(data), sizeof(data));
}

/* kernel_softirq_raise specialized tracepoint probe */

void probe_softirq_raise(void *_data, unsigned int nr);

DEFINE_MARKER_TP(kernel, softirq_raise, softirq_raise,
	probe_softirq_raise, "softirq_id #1u%u");

notrace void probe_softirq_raise(void *_data, unsigned int nr)
{
	struct marker *marker;
	unsigned char data;

	data = nr;

	marker = &GET_MARKER(kernel, softirq_raise);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, sizeof(data), sizeof(data));
}

/* Standard probes */
void probe_irq_tasklet_low_entry(void *_data, struct tasklet_struct *t)
{
	trace_mark_tp(kernel, tasklet_low_entry, irq_tasklet_low_entry,
		probe_irq_tasklet_low_entry, "func %p data %lu",
		t->func, t->data);
}

void probe_irq_tasklet_low_exit(void *_data, struct tasklet_struct *t)
{
	trace_mark_tp(kernel, tasklet_low_exit, irq_tasklet_low_exit,
		probe_irq_tasklet_low_exit, "func %p data %lu",
		t->func, t->data);
}

void probe_irq_tasklet_high_entry(void *_data, struct tasklet_struct *t)
{
	trace_mark_tp(kernel, tasklet_high_entry, irq_tasklet_high_entry,
		probe_irq_tasklet_high_entry, "func %p data %lu",
		t->func, t->data);
}

void probe_irq_tasklet_high_exit(void *_data, struct tasklet_struct *t)
{
	trace_mark_tp(kernel, tasklet_high_exit, irq_tasklet_high_exit,
		probe_irq_tasklet_high_exit, "func %p data %lu",
		t->func, t->data);
}

void probe_sched_kthread_stop(void *_data, struct task_struct *t)
{
	trace_mark_tp(kernel, kthread_stop, sched_kthread_stop,
		probe_sched_kthread_stop, "pid %d", t->pid);
}

void probe_sched_kthread_stop_ret(void *_data, int ret)
{
	trace_mark_tp(kernel, kthread_stop_ret, sched_kthread_stop_ret,
		probe_sched_kthread_stop_ret, "ret %d", ret);
}

void probe_sched_wait_task(void *_data, struct task_struct *p)
{
	trace_mark_tp(kernel, sched_wait_task, sched_wait_task,
		probe_sched_wait_task, "pid %d state #2d%ld",
		p->pid, p->state);
}

/* kernel_sched_try_wakeup specialized tracepoint probe */

void probe_sched_wakeup(void *_data, struct task_struct *p, int success);

DEFINE_MARKER_TP(kernel, sched_try_wakeup, sched_wakeup,
	probe_sched_wakeup, "pid %d cpu_id %u state #2d%ld");

notrace void probe_sched_wakeup(void *_data, struct task_struct *p, int success)
{
	struct marker *marker;
	struct serialize_int_int_short data;

	data.f1 = p->pid;
	data.f2 = task_cpu(p);
	data.f3 = p->state;

	marker = &GET_MARKER(kernel, sched_try_wakeup);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, serialize_sizeof(data), sizeof(int));
}

void probe_sched_wakeup_new(void *_data, struct task_struct *p, int success)
{
	trace_mark_tp(kernel, sched_wakeup_new_task, sched_wakeup_new,
		probe_sched_wakeup_new, "pid %d state #2d%ld cpu_id %u",
		p->pid, p->state, task_cpu(p));
}

/* kernel_sched_schedule specialized tracepoint probe */

void probe_sched_switch(void *_data, struct task_struct *prev,
		struct task_struct *next);

DEFINE_MARKER_TP(kernel, sched_schedule, sched_switch, probe_sched_switch,
	"prev_pid %d next_pid %d prev_state #2d%ld");

notrace void probe_sched_switch(void *_data, struct task_struct *prev,
		struct task_struct *next)
{
	struct marker *marker;
	struct serialize_int_int_short data;

	data.f1 = prev->pid;
	data.f2 = next->pid;
	data.f3 = prev->state;

	marker = &GET_MARKER(kernel, sched_schedule);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, serialize_sizeof(data), sizeof(int));
}

void probe_sched_migrate_task(void *_data, struct task_struct *p, int dest_cpu)
{
	trace_mark_tp(kernel, sched_migrate_task, sched_migrate_task,
		probe_sched_migrate_task, "pid %d state #2d%ld dest_cpu %d",
		p->pid, p->state, dest_cpu);
}

void probe_sched_signal_send(void *_data, int sig, struct siginfo *info, struct task_struct *t)
{
	trace_mark_tp(kernel, send_signal, signal_generate,
		probe_sched_signal_send, "pid %d signal %d", t->pid, sig);
}

void probe_sched_process_free(void *_data, struct task_struct *p)
{
	trace_mark_tp(kernel, process_free, sched_process_free,
		probe_sched_process_free, "pid %d", p->pid);
}

void probe_sched_process_exit(void *_data, struct task_struct *p)
{
	trace_mark_tp(kernel, process_exit, sched_process_exit,
		probe_sched_process_exit, "pid %d", p->pid);
}

void probe_sched_process_wait(void *_data, struct pid *pid)
{
	trace_mark_tp(kernel, process_wait, sched_process_wait,
		probe_sched_process_wait, "pid %d", pid_nr(pid));
}

void probe_sched_process_fork(void *_data, struct task_struct *parent,
		struct task_struct *child)
{
	trace_mark_tp(kernel, process_fork, sched_process_fork,
		probe_sched_process_fork,
		"parent_pid %d child_pid %d child_tgid %d",
		parent->pid, child->pid, child->tgid);
}

void probe_sched_kthread_create(void *_data, void *fn, int pid)
{
	trace_mark_tp(kernel, kthread_create, sched_kthread_create,
		probe_sched_kthread_create,
		"fn %p pid %d", fn, pid);
}

void probe_timer_itimer_expired(void *_data, struct signal_struct *sig)
{
	trace_mark_tp(kernel, timer_itimer_expired, timer_itimer_expired,
		probe_timer_itimer_expired, "pid %d",
		pid_nr(sig->leader_pid));
}

void probe_timer_itimer_set(void *_data, int which, struct itimerval *value)
{
	trace_mark_tp(kernel, timer_itimer_set,
		timer_itimer_set, probe_timer_itimer_set,
		"which %d interval_sec %ld interval_usec %ld "
		"value_sec %ld value_usec %ld",
		which,
		value->it_interval.tv_sec,
		value->it_interval.tv_usec,
		value->it_value.tv_sec,
		value->it_value.tv_usec);
}

/* kernel_timer_set specialized tracepoint probe */

void probe_timer_set(void *_data, struct timer_list *timer);

DEFINE_MARKER_TP(kernel, timer_set, timer_set, probe_timer_set,
	"expires %lu function %p data %lu");

notrace void probe_timer_set(void *_data, struct timer_list *timer)
{
	struct marker *marker;
	struct serialize_long_long_long data;

	data.f1 = timer->expires;
	data.f2 = (unsigned long)timer->function;
	data.f3 = timer->data;

	marker = &GET_MARKER(kernel, timer_set);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, serialize_sizeof(data), sizeof(long));
}

void probe_timer_update_time(void *_data, struct timespec *_xtime,
		struct timespec *_wall_to_monotonic)
{
	trace_mark_tp(kernel, timer_update_time, timer_update_time,
		probe_timer_update_time,
		"jiffies #8u%llu xtime_sec %ld xtime_nsec %ld "
		"walltomonotonic_sec %ld walltomonotonic_nsec %ld",
		(unsigned long long)jiffies_64, _xtime->tv_sec, _xtime->tv_nsec,
		_wall_to_monotonic->tv_sec, _wall_to_monotonic->tv_nsec);
}

void probe_timer_timeout(void *_data, struct task_struct *p)
{
	trace_mark_tp(kernel, timer_timeout, timer_timeout,
		probe_timer_timeout, "pid %d", p->pid);
}

void probe_kernel_printk(void *_data, unsigned long retaddr)
{
	trace_mark_tp(kernel, printk, kernel_printk,
		probe_kernel_printk, "ip 0x%lX", retaddr);
}

void probe_kernel_vprintk(void *_data, unsigned long retaddr, char *buf, int len)
{
	if (len > 0) {
		unsigned int loglevel;
		int mark_len;
		char *mark_buf;
		char saved_char;

		if (buf[0] == '<' && buf[1] >= '0' &&
		   buf[1] <= '7' && buf[2] == '>') {
			loglevel = buf[1] - '0';
			mark_buf = &buf[3];
			mark_len = len - 3;
		} else {
			loglevel = default_message_loglevel;
			mark_buf = buf;
			mark_len = len;
		}
		if (mark_buf[mark_len - 1] == '\n')
			mark_len--;
		saved_char = mark_buf[mark_len];
		mark_buf[mark_len] = '\0';
		trace_mark_tp(kernel, vprintk, kernel_vprintk,
			probe_kernel_vprintk,
			"loglevel #1u%u string %s ip 0x%lX",
			loglevel, mark_buf, retaddr);
		mark_buf[mark_len] = saved_char;
	}
}

#ifdef CONFIG_MODULES
void probe_kernel_module_free(void *_data, struct module *mod)
{
	trace_mark_tp(kernel, module_free, kernel_module_free,
		probe_kernel_module_free, "name %s", mod->name);
}

void probe_kernel_module_load(void *_data, struct module *mod)
{
	trace_mark_tp(kernel, module_load, kernel_module_load,
		probe_kernel_module_load, "name %s", mod->name);
}
#endif

void probe_kernel_panic(void *_data, const char *fmt, va_list args)
{
	char info[64];
	vsnprintf(info, sizeof(info), fmt, args);
	trace_mark_tp(kernel, panic, kernel_panic, probe_kernel_panic,
		"info %s", info);
}

void probe_kernel_kernel_kexec(void *_data, struct kimage *image)
{
	trace_mark_tp(kernel, kernel_kexec, kernel_kernel_kexec,
		probe_kernel_kernel_kexec, "image %p", image);
}

void probe_kernel_crash_kexec(void *_data, struct kimage *image, struct pt_regs *regs)
{
	trace_mark_tp(kernel, crash_kexec, kernel_crash_kexec,
		probe_kernel_crash_kexec, "image %p ip %p", image,
		regs ? (void *)instruction_pointer(regs) : NULL);
}

/* kernel_page_fault_entry specialized tracepoint probe */

void probe_kernel_page_fault_entry(void *_data, struct pt_regs *regs, int trapnr,
	struct mm_struct *mm, struct vm_area_struct *vma,
	unsigned long address, int write_access);

DEFINE_MARKER_TP(kernel, page_fault_entry, page_fault_entry,
	probe_kernel_page_fault_entry,
	"ip #p%lu address #p%lu trap_id #2u%u write_access #1u%u");

notrace void probe_kernel_page_fault_entry(void *_data, struct pt_regs *regs, int trapnr,
	struct mm_struct *mm, struct vm_area_struct *vma,
	unsigned long address, int write_access)
{
	struct marker *marker;
	struct serialize_long_long_short_char data;

	if (likely(regs))
		data.f1 = instruction_pointer(regs);
	else
		data.f1 = 0UL;
	data.f2 = address;
	data.f3 = (unsigned short)trapnr;
	data.f4 = (unsigned char)!!write_access;

	marker = &GET_MARKER(kernel, page_fault_entry);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, serialize_sizeof(data), sizeof(long));
}

/* kernel_page_fault_exit specialized tracepoint probe */

void probe_kernel_page_fault_exit(void *_data, int res);

DEFINE_MARKER_TP(kernel, page_fault_exit, page_fault_exit,
	probe_kernel_page_fault_exit,
	"res %d");

notrace void probe_kernel_page_fault_exit(void *_data, int res)
{
	struct marker *marker;

	marker = &GET_MARKER(kernel, page_fault_exit);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&res, sizeof(res), sizeof(res));
}

/* kernel_page_fault_nosem_entry specialized tracepoint probe */

void probe_kernel_page_fault_nosem_entry(void *_data, struct pt_regs *regs,
	int trapnr, unsigned long address);

DEFINE_MARKER_TP(kernel, page_fault_nosem_entry, page_fault_nosem_entry,
	probe_kernel_page_fault_nosem_entry,
	"ip #p%lu address #p%lu trap_id #2u%u");

notrace void probe_kernel_page_fault_nosem_entry(void *_data, struct pt_regs *regs,
	int trapnr, unsigned long address)
{
	struct marker *marker;
	struct serialize_long_long_short data;

	if (likely(regs))
		data.f1 = instruction_pointer(regs);
	else
		data.f1 = 0UL;
	data.f2 = address;
	data.f3 = (unsigned short)trapnr;

	marker = &GET_MARKER(kernel, page_fault_nosem_entry);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, serialize_sizeof(data), sizeof(long));
}

/* kernel_page_fault_nosem_exit specialized tracepoint probe */

void probe_kernel_page_fault_nosem_exit(void *_data, int res);

DEFINE_MARKER_TP(kernel, page_fault_nosem_exit, page_fault_nosem_exit,
	probe_kernel_page_fault_nosem_exit,
	MARK_NOARGS);

notrace void probe_kernel_page_fault_nosem_exit(void *_data, int res)
{
	struct marker *marker;

	marker = &GET_MARKER(kernel, page_fault_nosem_exit);
	ltt_specialized_trace(marker, marker->single.probe_private,
		NULL, 0, 0);
}

/* kernel_page_fault_get_user_entry specialized tracepoint probe */

void probe_kernel_page_fault_get_user_entry(void *_data, struct mm_struct *mm,
	struct vm_area_struct *vma, unsigned long address, int write_access);

DEFINE_MARKER_TP(kernel, page_fault_get_user_entry, page_fault_get_user_entry,
	probe_kernel_page_fault_get_user_entry,
	"address #p%lu write_access #1u%u");

notrace void probe_kernel_page_fault_get_user_entry(void *_data, struct mm_struct *mm,
	struct vm_area_struct *vma, unsigned long address, int write_access)
{
	struct marker *marker;
	struct serialize_long_char data;

	data.f1 = address;
	data.f2 = (unsigned char)!!write_access;

	marker = &GET_MARKER(kernel, page_fault_get_user_entry);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, serialize_sizeof(data), sizeof(long));
}

/* kernel_page_fault_get_user_exit specialized tracepoint probe */

void probe_kernel_page_fault_get_user_exit(void *_data, int res);

DEFINE_MARKER_TP(kernel, page_fault_get_user_exit, page_fault_get_user_exit,
	probe_kernel_page_fault_get_user_exit,
	"res %d");

notrace void probe_kernel_page_fault_get_user_exit(void *_data, int res)
{
	struct marker *marker;

	marker = &GET_MARKER(kernel, page_fault_get_user_exit);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&res, sizeof(res), sizeof(res));
}

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("kernel Tracepoint Probes");
