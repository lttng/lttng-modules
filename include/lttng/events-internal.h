/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng/events-internal.h
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_EVENTS_INTERNAL_H
#define _LTTNG_EVENTS_INTERNAL_H

#include <lttng/events.h>

struct lttng_kernel_event_common_private {
	struct lttng_kernel_event_common *pub;		/* Public event interface */

	const struct lttng_kernel_event_desc *desc;
	/* Backward references: list of lttng_enabler_ref (ref to enablers) */
	struct list_head enablers_ref_head;
	int registered;					/* has reg'd tracepoint probe */
	uint64_t user_token;

	int has_enablers_without_filter_bytecode;
	/* list of struct lttng_kernel_bytecode_runtime, sorted by seqnum */
	struct list_head filter_bytecode_runtime_head;
	enum lttng_kernel_abi_instrumentation instrumentation;
	/* Selected by instrumentation */
	union {
		struct lttng_kprobe kprobe;
		struct lttng_uprobe uprobe;
		struct {
			struct lttng_krp *lttng_krp;
			char *symbol_name;
		} kretprobe;
		struct {
			enum lttng_syscall_entryexit entryexit;
			enum lttng_syscall_abi abi;
			struct hlist_node node;			/* chain registered syscall event_notifier */
			unsigned int syscall_id;
		} syscall;
	} u;
};

struct lttng_kernel_event_recorder_private {
	struct lttng_kernel_event_common_private parent;

	struct lttng_kernel_event_recorder *pub;	/* Public event interface */
	struct list_head node;				/* Event recorder list */
	struct hlist_node hlist;			/* Hash table of event recorders */
	struct lttng_kernel_ctx *ctx;
	unsigned int id;
	unsigned int metadata_dumped:1;
};

struct lttng_kernel_event_notifier_private {
	struct lttng_kernel_event_common_private parent;

	struct lttng_kernel_event_notifier *pub;	/* Public event notifier interface */
	struct lttng_event_notifier_group *group;	/* weak ref */
	size_t num_captures;				/* Needed to allocate the msgpack array. */
	uint64_t error_counter_index;
	struct list_head node;				/* Event notifier list */
	struct hlist_node hlist;			/* Hash table of event notifiers */
	struct list_head capture_bytecode_runtime_head;

};

enum lttng_kernel_bytecode_interpreter_ret {
	LTTNG_KERNEL_BYTECODE_INTERPRETER_ERROR = -1,
	LTTNG_KERNEL_BYTECODE_INTERPRETER_OK = 0,
};

enum lttng_kernel_bytecode_filter_result {
	LTTNG_KERNEL_BYTECODE_FILTER_ACCEPT = 0,
	LTTNG_KERNEL_BYTECODE_FILTER_REJECT = 1,
};

struct lttng_kernel_bytecode_filter_ctx {
	enum lttng_kernel_bytecode_filter_result result;
};

struct lttng_interpreter_output;

enum lttng_kernel_bytecode_type {
	LTTNG_KERNEL_BYTECODE_TYPE_FILTER,
	LTTNG_KERNEL_BYTECODE_TYPE_CAPTURE,
};

struct lttng_kernel_bytecode_node {
	enum lttng_kernel_bytecode_type type;
	struct list_head node;
	struct lttng_enabler *enabler;
	struct {
		uint32_t len;
		uint32_t reloc_offset;
		uint64_t seqnum;
		char data[];
	} bc;
};

struct lttng_kernel_bytecode_runtime {
	/* Associated bytecode */
	enum lttng_kernel_bytecode_type type;
	struct lttng_kernel_bytecode_node *bc;
	int (*interpreter_func)(struct lttng_kernel_bytecode_runtime *kernel_bytecode,
				const char *interpreter_stack_data,
				struct lttng_kernel_probe_ctx *lttng_probe_ctx,
				void *caller_ctx);
	int link_failed;
	struct list_head node;	/* list of bytecode runtime in event */
	struct lttng_kernel_ctx *ctx;
};

/*
 * Enabler field, within whatever object is enabling an event. Target of
 * backward reference.
 */
struct lttng_enabler {
	enum lttng_enabler_format_type format_type;

	/* head list of struct lttng_kernel_bytecode_node */
	struct list_head filter_bytecode_head;

	struct lttng_kernel_abi_event event_param;
	unsigned int enabled:1;

	uint64_t user_token;		/* User-provided token. */
};

struct lttng_event_enabler {
	struct lttng_enabler base;
	struct list_head node;	/* per-session list of enablers */
	struct lttng_channel *chan;
};

struct lttng_event_notifier_enabler {
	struct lttng_enabler base;
	uint64_t error_counter_index;
	struct list_head node;	/* List of event_notifier enablers */
	struct lttng_event_notifier_group *group;

	/* head list of struct lttng_kernel_bytecode_node */
	struct list_head capture_bytecode_head;
	uint64_t num_captures;
};

struct lttng_ctx_value {
	union {
		int64_t s64;
		const char *str;
		double d;
	} u;
};

/*
 * We need to keep this perf counter field separately from struct
 * lttng_kernel_ctx_field because cpu hotplug needs fixed-location addresses.
 */
struct lttng_perf_counter_field {
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0))
	struct lttng_cpuhp_node cpuhp_prepare;
	struct lttng_cpuhp_node cpuhp_online;
#else
	struct notifier_block nb;
	int hp_enable;
#endif
	struct perf_event_attr *attr;
	struct perf_event **e;	/* per-cpu array */
	char *name;
	struct lttng_kernel_event_field *event_field;
};

struct lttng_kernel_ctx_field {
	const struct lttng_kernel_event_field *event_field;
	size_t (*get_size)(void *priv, struct lttng_kernel_probe_ctx *probe_ctx,
			size_t offset);
	void (*record)(void *priv, struct lttng_kernel_probe_ctx *probe_ctx,
			struct lib_ring_buffer_ctx *ctx,
			struct lttng_channel *chan);
	void (*get_value)(void *priv, struct lttng_kernel_probe_ctx *probe_ctx,
			struct lttng_ctx_value *value);
	void (*destroy)(void *priv);
	void *priv;
};

struct lttng_kernel_ctx {
	struct lttng_kernel_ctx_field *fields;
	unsigned int nr_fields;
	unsigned int allocated_fields;
	size_t largest_align;	/* in bytes */
};

extern struct lttng_kernel_ctx *lttng_static_ctx;

static inline
const struct lttng_kernel_type_integer *lttng_kernel_get_type_integer(const struct lttng_kernel_type_common *type)
{
	if (type->type != lttng_kernel_type_integer)
		return NULL;
	return container_of(type, const struct lttng_kernel_type_integer, parent);
}

static inline
const struct lttng_kernel_type_string *lttng_kernel_get_type_string(const struct lttng_kernel_type_common *type)
{
	if (type->type != lttng_kernel_type_string)
		return NULL;
	return container_of(type, const struct lttng_kernel_type_string, parent);
}

static inline
const struct lttng_kernel_type_enum *lttng_kernel_get_type_enum(const struct lttng_kernel_type_common *type)
{
	if (type->type != lttng_kernel_type_enum)
		return NULL;
	return container_of(type, const struct lttng_kernel_type_enum, parent);
}

static inline
const struct lttng_kernel_type_array *lttng_kernel_get_type_array(const struct lttng_kernel_type_common *type)
{
	if (type->type != lttng_kernel_type_array)
		return NULL;
	return container_of(type, const struct lttng_kernel_type_array, parent);
}

static inline
const struct lttng_kernel_type_sequence *lttng_kernel_get_type_sequence(const struct lttng_kernel_type_common *type)
{
	if (type->type != lttng_kernel_type_sequence)
		return NULL;
	return container_of(type, const struct lttng_kernel_type_sequence, parent);
}

static inline
const struct lttng_kernel_type_struct *lttng_kernel_get_type_struct(const struct lttng_kernel_type_common *type)
{
	if (type->type != lttng_kernel_type_struct)
		return NULL;
	return container_of(type, const struct lttng_kernel_type_struct, parent);
}

static inline
const struct lttng_kernel_type_variant *lttng_kernel_get_type_variant(const struct lttng_kernel_type_common *type)
{
	if (type->type != lttng_kernel_type_variant)
		return NULL;
	return container_of(type, const struct lttng_kernel_type_variant, parent);
}

static inline bool lttng_kernel_type_is_bytewise_integer(const struct lttng_kernel_type_common *type)
{
	const struct lttng_kernel_type_integer *type_integer = lttng_kernel_get_type_integer(type);

	if (!type_integer)
		return false;
	switch (type_integer->size) {
	case 8:		/* Fall-through. */
	case 16:	/* Fall-through. */
	case 32:	/* Fall-through. */
	case 64:
		break;
	default:
		return false;
	}
	return true;
}

int lttng_kernel_interpret_event_filter(const struct lttng_kernel_event_common *event,
		const char *interpreter_stack_data,
		struct lttng_kernel_probe_ctx *probe_ctx,
		void *event_filter_ctx);

static inline
struct lttng_enabler *lttng_event_enabler_as_enabler(
		struct lttng_event_enabler *event_enabler)
{
	return &event_enabler->base;
}

static inline
struct lttng_enabler *lttng_event_notifier_enabler_as_enabler(
		struct lttng_event_notifier_enabler *event_notifier_enabler)
{
	return &event_notifier_enabler->base;
}

int lttng_context_init(void);
void lttng_context_exit(void);
int lttng_kernel_context_append(struct lttng_kernel_ctx **ctx_p,
		const struct lttng_kernel_ctx_field *f);
void lttng_kernel_context_remove_last(struct lttng_kernel_ctx **ctx_p);
struct lttng_kernel_ctx_field *lttng_kernel_get_context_field_from_index(struct lttng_kernel_ctx *ctx,
		size_t index);
int lttng_kernel_find_context(struct lttng_kernel_ctx *ctx, const char *name);
int lttng_kernel_get_context_index(struct lttng_kernel_ctx *ctx, const char *name);
void lttng_kernel_destroy_context(struct lttng_kernel_ctx *ctx);
int lttng_add_pid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_cpu_id_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_procname_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_prio_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_nice_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_vpid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_tid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_vtid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_ppid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_vppid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_hostname_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_interruptible_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_need_reschedule_to_ctx(struct lttng_kernel_ctx **ctx);
#if defined(CONFIG_PREEMPT_RT_FULL) || defined(CONFIG_PREEMPT)
int lttng_add_preemptible_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_preemptible_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif
#ifdef CONFIG_PREEMPT_RT_FULL
int lttng_add_migratable_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_migratable_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif

int lttng_add_callstack_to_ctx(struct lttng_kernel_ctx **ctx, int type);

#if defined(CONFIG_CGROUPS) && \
	((LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,6,0)) || \
	 LTTNG_UBUNTU_KERNEL_RANGE(4,4,0,0, 4,5,0,0))
int lttng_add_cgroup_ns_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_cgroup_ns_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif

#if defined(CONFIG_IPC_NS) && \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,8,0))
int lttng_add_ipc_ns_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_ipc_ns_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif

#if !defined(LTTNG_MNT_NS_MISSING_HEADER) && \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,8,0))
int lttng_add_mnt_ns_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_mnt_ns_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif

#if defined(CONFIG_NET_NS) && \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,8,0))
int lttng_add_net_ns_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_net_ns_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif

#if defined(CONFIG_PID_NS) && \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,8,0))
int lttng_add_pid_ns_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_pid_ns_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif

#if defined(CONFIG_USER_NS) && \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,8,0))
int lttng_add_user_ns_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_user_ns_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif

#if defined(CONFIG_UTS_NS) && \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,8,0))
int lttng_add_uts_ns_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_uts_ns_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif

#if defined(CONFIG_TIME_NS) && \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,6,0))
int lttng_add_time_ns_to_ctx(struct lttng_kernel_ctx **ctx);
#else
static inline
int lttng_add_time_ns_to_ctx(struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
#endif

int lttng_add_uid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_euid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_suid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_gid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_egid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_sgid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_vuid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_veuid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_vsuid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_vgid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_vegid_to_ctx(struct lttng_kernel_ctx **ctx);
int lttng_add_vsgid_to_ctx(struct lttng_kernel_ctx **ctx);

#if defined(CONFIG_PERF_EVENTS)
int lttng_add_perf_counter_to_ctx(uint32_t type,
				  uint64_t config,
				  const char *name,
				  struct lttng_kernel_ctx **ctx);
int lttng_cpuhp_perf_counter_online(unsigned int cpu,
		struct lttng_cpuhp_node *node);
int lttng_cpuhp_perf_counter_dead(unsigned int cpu,
		struct lttng_cpuhp_node *node);
#else
static inline
int lttng_add_perf_counter_to_ctx(uint32_t type,
				  uint64_t config,
				  const char *name,
				  struct lttng_kernel_ctx **ctx)
{
	return -ENOSYS;
}
static inline
int lttng_cpuhp_perf_counter_online(unsigned int cpu,
		struct lttng_cpuhp_node *node)
{
	return 0;
}
static inline
int lttng_cpuhp_perf_counter_dead(unsigned int cpu,
		struct lttng_cpuhp_node *node)
{
	return 0;
}
#endif



#define lttng_kernel_static_ctx_field(_event_field, _get_size, _record, _get_value, _destroy, _priv)	\
	__LTTNG_COMPOUND_LITERAL(const struct lttng_kernel_ctx_field, {					\
		.event_field = (_event_field),								\
		.get_size = (_get_size),								\
		.record = (_record),									\
		.get_value = (_get_value),								\
		.destroy = (_destroy),									\
		.priv = (_priv),									\
	})

#endif /* _LTTNG_EVENTS_INTERNAL_H */
