#ifndef LTTNG_SYSCALLS_H
#define LTTNG_SYSCALLS_H

#include <stddef.h>

#include <lttng/events.h>
struct trace_syscall_entry {
	void *event_func;
	const struct lttng_kernel_event_desc *desc;
	const struct lttng_kernel_event_field **fields;
	unsigned int nrargs;
};

struct trace_syscall_table {
	const struct trace_syscall_entry *table;
	const size_t len;
};
#endif /* LTTNG_SYSCALLS_H */
