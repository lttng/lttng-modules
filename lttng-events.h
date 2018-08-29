/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-events.h
 *
 * Holds LTTng per-session event registry.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_EVENTS_H
#define _LTTNG_EVENTS_H

#include <linux/version.h>
#include <linux/list.h>
#include <linux/kprobes.h>
#include <linux/kref.h>
#include <lttng-cpuhotplug.h>
#include <wrapper/uuid.h>
#include <wrapper/uprobes.h>
#include <lttng-tracer.h>
#include <lttng-abi.h>
#include <lttng-abi-old.h>

#define lttng_is_signed_type(type)	(((type)(-1)) < 0)

struct lttng_channel;
struct lttng_session;
struct lttng_metadata_cache;
struct lib_ring_buffer_ctx;
struct perf_event;
struct perf_event_attr;
struct lib_ring_buffer_config;

/* Type description */

enum abstract_types {
	atype_integer,
	atype_enum,
	atype_array,
	atype_sequence,
	atype_string,
	atype_struct,
	atype_array_compound,		/* Array of compound types. */
	atype_sequence_compound,	/* Sequence of compound types. */
	atype_variant,
	atype_array_bitfield,
	atype_sequence_bitfield,
	NR_ABSTRACT_TYPES,
};

enum lttng_string_encodings {
	lttng_encode_none = 0,
	lttng_encode_UTF8 = 1,
	lttng_encode_ASCII = 2,
	NR_STRING_ENCODINGS,
};

enum channel_type {
	PER_CPU_CHANNEL,
	METADATA_CHANNEL,
};

struct lttng_enum_value {
	unsigned long long value;
	unsigned int signedness:1;
};

struct lttng_enum_entry {
	struct lttng_enum_value start, end;	/* start and end are inclusive */
	const char *string;
	struct {
		unsigned int is_auto:1;
	} options;
};

#define __type_integer(_type, _size, _alignment, _signedness,	\
		_byte_order, _base, _encoding)	\
	{							\
	    .atype = atype_integer,				\
	    .u.basic.integer =					\
		{						\
		  .size = (_size) ? : sizeof(_type) * CHAR_BIT,	\
		  .alignment = (_alignment) ? : lttng_alignof(_type) * CHAR_BIT, \
		  .signedness = (_signedness) >= 0 ? (_signedness) : lttng_is_signed_type(_type), \
		  .reverse_byte_order = _byte_order != __BYTE_ORDER, \
		  .base = _base,				\
		  .encoding = lttng_encode_##_encoding,		\
		},						\
	}							\

struct lttng_integer_type {
	unsigned int size;		/* in bits */
	unsigned short alignment;	/* in bits */
	unsigned int signedness:1,
		reverse_byte_order:1;
	unsigned int base;		/* 2, 8, 10, 16, for pretty print */
	enum lttng_string_encodings encoding;
};

union _lttng_basic_type {
	struct lttng_integer_type integer;
	struct {
		const struct lttng_enum_desc *desc;	/* Enumeration mapping */
		struct lttng_integer_type container_type;
	} enumeration;
	struct {
		enum lttng_string_encodings encoding;
	} string;
};

struct lttng_basic_type {
	enum abstract_types atype;
	union {
		union _lttng_basic_type basic;
	} u;
};

struct lttng_type {
	enum abstract_types atype;
	union {
		union _lttng_basic_type basic;
		struct {
			struct lttng_basic_type elem_type;
			unsigned int length;		/* num. elems. */
			unsigned int elem_alignment;	/* alignment override */
		} array;
		struct {
			struct lttng_basic_type length_type;
			struct lttng_basic_type elem_type;
			unsigned int elem_alignment;	/* alignment override */
		} sequence;
		struct {
			uint32_t nr_fields;
			struct lttng_event_field *fields; /* Array of fields. */
		} _struct;
		struct {
			struct lttng_type *elem_type;
			unsigned int length;		/* num. elems. */
		} array_compound;
		struct {
			struct lttng_type *elem_type;
			const char *length_name;
		} sequence_compound;
		struct {
			const char *tag_name;
			struct lttng_event_field *choices; /* Array of fields. */
			uint32_t nr_choices;
		} variant;
	} u;
};

struct lttng_enum_desc {
	const char *name;
	const struct lttng_enum_entry *entries;
	unsigned int nr_entries;
};

/* Event field description */

struct lttng_event_field {
	const char *name;
	struct lttng_type type;
	unsigned int nowrite:1,		/* do not write into trace */
			user:1;		/* fetch from user-space */
};

union lttng_ctx_value {
	int64_t s64;
	const char *str;
	double d;
};

/*
 * We need to keep this perf counter field separately from struct
 * lttng_ctx_field because cpu hotplug needs fixed-location addresses.
 */
struct lttng_perf_counter_field {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0))
	struct lttng_cpuhp_node cpuhp_prepare;
	struct lttng_cpuhp_node cpuhp_online;
#else
	struct notifier_block nb;
	int hp_enable;
#endif
	struct perf_event_attr *attr;
	struct perf_event **e;	/* per-cpu array */
};

struct lttng_probe_ctx {
	struct lttng_event *event;
	uint8_t interruptible;
};

struct lttng_ctx_field {
	struct lttng_event_field event_field;
	size_t (*get_size)(size_t offset);
	size_t (*get_size_arg)(size_t offset, struct lttng_ctx_field *field,
	                       struct lib_ring_buffer_ctx *ctx,
	                       struct lttng_channel *chan);
	void (*record)(struct lttng_ctx_field *field,
		       struct lib_ring_buffer_ctx *ctx,
		       struct lttng_channel *chan);
	void (*get_value)(struct lttng_ctx_field *field,
			 struct lttng_probe_ctx *lttng_probe_ctx,
			 union lttng_ctx_value *value);
	union {
		struct lttng_perf_counter_field *perf_counter;
	} u;
	void (*destroy)(struct lttng_ctx_field *field);
	/*
	 * Private data to keep state between get_size and record.
	 * User must perform its own synchronization to protect against
	 * concurrent and reentrant contexts.
	 */
	void *priv;
};

struct lttng_ctx {
	struct lttng_ctx_field *fields;
	unsigned int nr_fields;
	unsigned int allocated_fields;
	size_t largest_align;	/* in bytes */
};

struct lttng_event_desc {
	const char *name;		/* lttng-modules name */
	const char *kname;		/* Linux kernel name (tracepoints) */
	void *probe_callback;
	const struct lttng_event_ctx *ctx;	/* context */
	const struct lttng_event_field *fields;	/* event payload */
	unsigned int nr_fields;
	struct module *owner;
};

struct lttng_probe_desc {
	const char *provider;
	const struct lttng_event_desc **event_desc;
	unsigned int nr_events;
	struct list_head head;			/* chain registered probes */
	struct list_head lazy_init_head;
	int lazy;				/* lazy registration */
};

struct lttng_krp;				/* Kretprobe handling */

enum lttng_event_type {
	LTTNG_TYPE_EVENT = 0,
	LTTNG_TYPE_ENABLER = 1,
};

struct lttng_filter_bytecode_node {
	struct list_head node;
	struct lttng_enabler *enabler;
	/*
	 * struct lttng_kernel_filter_bytecode has var. sized array, must be
	 * last field.
	 */
	struct lttng_kernel_filter_bytecode bc;
};

/*
 * Filter return value masks.
 */
enum lttng_filter_ret {
	LTTNG_FILTER_DISCARD = 0,
	LTTNG_FILTER_RECORD_FLAG = (1ULL << 0),
	/* Other bits are kept for future use. */
};

struct lttng_bytecode_runtime {
	/* Associated bytecode */
	struct lttng_filter_bytecode_node *bc;
	uint64_t (*filter)(void *filter_data, struct lttng_probe_ctx *lttng_probe_ctx,
			const char *filter_stack_data);
	int link_failed;
	struct list_head node;	/* list of bytecode runtime in event */
	struct lttng_event *event;
};

/*
 * Objects in a linked-list of enablers, owned by an event.
 */
struct lttng_enabler_ref {
	struct list_head node;			/* enabler ref list */
	struct lttng_enabler *ref;		/* backward ref */
};

struct lttng_uprobe_handler {
	struct lttng_event *event;
	loff_t offset;
	struct uprobe_consumer up_consumer;
	struct list_head node;
};

/*
 * lttng_event structure is referred to by the tracing fast path. It must be
 * kept small.
 */
struct lttng_event {
	enum lttng_event_type evtype;	/* First field. */
	unsigned int id;
	struct lttng_channel *chan;
	int enabled;
	const struct lttng_event_desc *desc;
	void *filter;
	struct lttng_ctx *ctx;
	enum lttng_kernel_instrumentation instrumentation;
	union {
		struct {
			struct kprobe kp;
			char *symbol_name;
		} kprobe;
		struct {
			struct lttng_krp *lttng_krp;
			char *symbol_name;
		} kretprobe;
		struct {
			char *symbol_name;
		} ftrace;
		struct {
			struct inode *inode;
			struct list_head head;
		} uprobe;
	} u;
	struct list_head list;		/* Event list in session */
	unsigned int metadata_dumped:1;

	/* Backward references: list of lttng_enabler_ref (ref to enablers) */
	struct list_head enablers_ref_head;
	struct hlist_node hlist;	/* session ht of events */
	int registered;			/* has reg'd tracepoint probe */
	/* list of struct lttng_bytecode_runtime, sorted by seqnum */
	struct list_head bytecode_runtime_head;
	int has_enablers_without_bytecode;
};

enum lttng_enabler_type {
	LTTNG_ENABLER_STAR_GLOB,
	LTTNG_ENABLER_NAME,
};

/*
 * Enabler field, within whatever object is enabling an event. Target of
 * backward reference.
 */
struct lttng_enabler {
	enum lttng_event_type evtype;	/* First field. */

	enum lttng_enabler_type type;

	struct list_head node;	/* per-session list of enablers */
	/* head list of struct lttng_ust_filter_bytecode_node */
	struct list_head filter_bytecode_head;

	struct lttng_kernel_event event_param;
	struct lttng_channel *chan;
	struct lttng_ctx *ctx;
	unsigned int enabled:1;
};

struct lttng_channel_ops {
	struct channel *(*channel_create)(const char *name,
				struct lttng_channel *lttng_chan,
				void *buf_addr,
				size_t subbuf_size, size_t num_subbuf,
				unsigned int switch_timer_interval,
				unsigned int read_timer_interval);
	void (*channel_destroy)(struct channel *chan);
	struct lib_ring_buffer *(*buffer_read_open)(struct channel *chan);
	int (*buffer_has_read_closed_stream)(struct channel *chan);
	void (*buffer_read_close)(struct lib_ring_buffer *buf);
	int (*event_reserve)(struct lib_ring_buffer_ctx *ctx,
			     uint32_t event_id);
	void (*event_commit)(struct lib_ring_buffer_ctx *ctx);
	void (*event_write)(struct lib_ring_buffer_ctx *ctx, const void *src,
			    size_t len);
	void (*event_write_from_user)(struct lib_ring_buffer_ctx *ctx,
				      const void *src, size_t len);
	void (*event_memset)(struct lib_ring_buffer_ctx *ctx,
			     int c, size_t len);
	void (*event_strcpy)(struct lib_ring_buffer_ctx *ctx, const char *src,
			     size_t len);
	void (*event_strcpy_from_user)(struct lib_ring_buffer_ctx *ctx,
				       const char __user *src, size_t len);
	/*
	 * packet_avail_size returns the available size in the current
	 * packet. Note that the size returned is only a hint, since it
	 * may change due to concurrent writes.
	 */
	size_t (*packet_avail_size)(struct channel *chan);
	wait_queue_head_t *(*get_writer_buf_wait_queue)(struct channel *chan, int cpu);
	wait_queue_head_t *(*get_hp_wait_queue)(struct channel *chan);
	int (*is_finalized)(struct channel *chan);
	int (*is_disabled)(struct channel *chan);
	int (*timestamp_begin) (const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *timestamp_begin);
	int (*timestamp_end) (const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *timestamp_end);
	int (*events_discarded) (const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *events_discarded);
	int (*content_size) (const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *content_size);
	int (*packet_size) (const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *packet_size);
	int (*stream_id) (const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *stream_id);
	int (*current_timestamp) (const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *ts);
	int (*sequence_number) (const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *seq);
	int (*instance_id) (const struct lib_ring_buffer_config *config,
			struct lib_ring_buffer *bufb,
			uint64_t *id);
};

struct lttng_transport {
	char *name;
	struct module *owner;
	struct list_head node;
	struct lttng_channel_ops ops;
};

struct lttng_syscall_filter;

#define LTTNG_EVENT_HT_BITS		12
#define LTTNG_EVENT_HT_SIZE		(1U << LTTNG_EVENT_HT_BITS)

struct lttng_event_ht {
	struct hlist_head table[LTTNG_EVENT_HT_SIZE];
};

struct lttng_channel {
	unsigned int id;
	struct channel *chan;		/* Channel buffers */
	int enabled;
	struct lttng_ctx *ctx;
	/* Event ID management */
	struct lttng_session *session;
	struct file *file;		/* File associated to channel */
	unsigned int free_event_id;	/* Next event ID to allocate */
	struct list_head list;		/* Channel list */
	struct lttng_channel_ops *ops;
	struct lttng_transport *transport;
	struct lttng_event **sc_table;	/* for syscall tracing */
	struct lttng_event **compat_sc_table;
	struct lttng_event **sc_exit_table;	/* for syscall exit tracing */
	struct lttng_event **compat_sc_exit_table;
	struct lttng_event *sc_unknown;	/* for unknown syscalls */
	struct lttng_event *sc_compat_unknown;
	struct lttng_event *sc_exit_unknown;
	struct lttng_event *compat_sc_exit_unknown;
	struct lttng_syscall_filter *sc_filter;
	int header_type;		/* 0: unset, 1: compact, 2: large */
	enum channel_type channel_type;
	unsigned int metadata_dumped:1,
		sys_enter_registered:1,
		sys_exit_registered:1,
		syscall_all:1,
		tstate:1;		/* Transient enable state */
};

struct lttng_metadata_stream {
	void *priv;			/* Ring buffer private data */
	struct lttng_metadata_cache *metadata_cache;
	unsigned int metadata_in;	/* Bytes read from the cache */
	unsigned int metadata_out;	/* Bytes consumed from stream */
	int finalized;			/* Has channel been finalized */
	wait_queue_head_t read_wait;	/* Reader buffer-level wait queue */
	struct list_head list;		/* Stream list */
	struct lttng_transport *transport;
	uint64_t version;		/* Current version of the metadata cache */
};

#define LTTNG_DYNAMIC_LEN_STACK_SIZE	128

struct lttng_dynamic_len_stack {
	size_t stack[LTTNG_DYNAMIC_LEN_STACK_SIZE];
	size_t offset;
};

DECLARE_PER_CPU(struct lttng_dynamic_len_stack, lttng_dynamic_len_stack);

/*
 * struct lttng_pid_tracker declared in header due to deferencing of *v
 * in RCU_INITIALIZER(v).
 */
#define LTTNG_PID_HASH_BITS	6
#define LTTNG_PID_TABLE_SIZE	(1 << LTTNG_PID_HASH_BITS)

struct lttng_pid_tracker {
	struct hlist_head pid_hash[LTTNG_PID_TABLE_SIZE];
};

struct lttng_pid_hash_node {
	struct hlist_node hlist;
	int pid;
};

struct lttng_session {
	int active;			/* Is trace session active ? */
	int been_active;		/* Has trace session been active ? */
	struct file *file;		/* File associated to session */
	struct list_head chan;		/* Channel list head */
	struct list_head events;	/* Event list head */
	struct list_head list;		/* Session list */
	unsigned int free_chan_id;	/* Next chan ID to allocate */
	uuid_le uuid;			/* Trace session unique ID */
	struct lttng_metadata_cache *metadata_cache;
	struct lttng_pid_tracker *pid_tracker;
	unsigned int metadata_dumped:1,
		tstate:1;		/* Transient enable state */
	/* List of enablers */
	struct list_head enablers_head;
	/* Hash table of events */
	struct lttng_event_ht events_ht;
};

struct lttng_metadata_cache {
	char *data;			/* Metadata cache */
	unsigned int cache_alloc;	/* Metadata allocated size (bytes) */
	unsigned int metadata_written;	/* Number of bytes written in metadata cache */
	struct kref refcount;		/* Metadata cache usage */
	struct list_head metadata_stream;	/* Metadata stream list */
	uuid_le uuid;			/* Trace session unique ID (copy) */
	struct mutex lock;		/* Produce/consume lock */
	uint64_t version;		/* Current version of the metadata */
};

void lttng_lock_sessions(void);
void lttng_unlock_sessions(void);

struct list_head *lttng_get_probe_list_head(void);

struct lttng_enabler *lttng_enabler_create(enum lttng_enabler_type type,
		struct lttng_kernel_event *event_param,
		struct lttng_channel *chan);

int lttng_enabler_enable(struct lttng_enabler *enabler);
int lttng_enabler_disable(struct lttng_enabler *enabler);
int lttng_fix_pending_events(void);
int lttng_session_active(void);

struct lttng_session *lttng_session_create(void);
int lttng_session_enable(struct lttng_session *session);
int lttng_session_disable(struct lttng_session *session);
void lttng_session_destroy(struct lttng_session *session);
int lttng_session_metadata_regenerate(struct lttng_session *session);
int lttng_session_statedump(struct lttng_session *session);
void metadata_cache_destroy(struct kref *kref);

struct lttng_channel *lttng_channel_create(struct lttng_session *session,
				       const char *transport_name,
				       void *buf_addr,
				       size_t subbuf_size, size_t num_subbuf,
				       unsigned int switch_timer_interval,
				       unsigned int read_timer_interval,
				       enum channel_type channel_type);
struct lttng_channel *lttng_global_channel_create(struct lttng_session *session,
				       int overwrite, void *buf_addr,
				       size_t subbuf_size, size_t num_subbuf,
				       unsigned int switch_timer_interval,
				       unsigned int read_timer_interval);

void lttng_metadata_channel_destroy(struct lttng_channel *chan);
struct lttng_event *lttng_event_create(struct lttng_channel *chan,
				struct lttng_kernel_event *event_param,
				void *filter,
				const struct lttng_event_desc *event_desc,
				enum lttng_kernel_instrumentation itype);
struct lttng_event *_lttng_event_create(struct lttng_channel *chan,
				struct lttng_kernel_event *event_param,
				void *filter,
				const struct lttng_event_desc *event_desc,
				enum lttng_kernel_instrumentation itype);
struct lttng_event *lttng_event_compat_old_create(struct lttng_channel *chan,
		struct lttng_kernel_old_event *old_event_param,
		void *filter,
		const struct lttng_event_desc *internal_desc);

int lttng_channel_enable(struct lttng_channel *channel);
int lttng_channel_disable(struct lttng_channel *channel);
int lttng_event_enable(struct lttng_event *event);
int lttng_event_disable(struct lttng_event *event);

void lttng_transport_register(struct lttng_transport *transport);
void lttng_transport_unregister(struct lttng_transport *transport);

void synchronize_trace(void);
int lttng_abi_init(void);
int lttng_abi_compat_old_init(void);
void lttng_abi_exit(void);
void lttng_abi_compat_old_exit(void);

int lttng_probe_register(struct lttng_probe_desc *desc);
void lttng_probe_unregister(struct lttng_probe_desc *desc);
const struct lttng_event_desc *lttng_event_get(const char *name);
void lttng_event_put(const struct lttng_event_desc *desc);
int lttng_probes_init(void);
void lttng_probes_exit(void);

int lttng_metadata_output_channel(struct lttng_metadata_stream *stream,
		struct channel *chan);

int lttng_pid_tracker_get_node_pid(const struct lttng_pid_hash_node *node);
struct lttng_pid_tracker *lttng_pid_tracker_create(void);
void lttng_pid_tracker_destroy(struct lttng_pid_tracker *lpf);
bool lttng_pid_tracker_lookup(struct lttng_pid_tracker *lpf, int pid);
int lttng_pid_tracker_add(struct lttng_pid_tracker *lpf, int pid);
int lttng_pid_tracker_del(struct lttng_pid_tracker *lpf, int pid);

int lttng_session_track_pid(struct lttng_session *session, int pid);
int lttng_session_untrack_pid(struct lttng_session *session, int pid);

int lttng_session_list_tracker_pids(struct lttng_session *session);

void lttng_clock_ref(void);
void lttng_clock_unref(void);

#if defined(CONFIG_HAVE_SYSCALL_TRACEPOINTS)
int lttng_syscalls_register(struct lttng_channel *chan, void *filter);
int lttng_syscalls_unregister(struct lttng_channel *chan);
int lttng_syscall_filter_enable(struct lttng_channel *chan,
		const char *name);
int lttng_syscall_filter_disable(struct lttng_channel *chan,
		const char *name);
long lttng_channel_syscall_mask(struct lttng_channel *channel,
		struct lttng_kernel_syscall_mask __user *usyscall_mask);
#else
static inline int lttng_syscalls_register(struct lttng_channel *chan, void *filter)
{
	return -ENOSYS;
}

static inline int lttng_syscalls_unregister(struct lttng_channel *chan)
{
	return 0;
}

static inline int lttng_syscall_filter_enable(struct lttng_channel *chan,
		const char *name)
{
	return -ENOSYS;
}

static inline int lttng_syscall_filter_disable(struct lttng_channel *chan,
		const char *name)
{
	return -ENOSYS;
}

static inline long lttng_channel_syscall_mask(struct lttng_channel *channel,
		struct lttng_kernel_syscall_mask __user *usyscall_mask)
{
	return -ENOSYS;
}
#endif

void lttng_filter_sync_state(struct lttng_bytecode_runtime *runtime);
int lttng_enabler_attach_bytecode(struct lttng_enabler *enabler,
		struct lttng_kernel_filter_bytecode __user *bytecode);
void lttng_enabler_event_link_bytecode(struct lttng_event *event,
		struct lttng_enabler *enabler);

int lttng_probes_init(void);

extern struct lttng_ctx *lttng_static_ctx;

int lttng_context_init(void);
void lttng_context_exit(void);
struct lttng_ctx_field *lttng_append_context(struct lttng_ctx **ctx);
void lttng_context_update(struct lttng_ctx *ctx);
int lttng_find_context(struct lttng_ctx *ctx, const char *name);
int lttng_get_context_index(struct lttng_ctx *ctx, const char *name);
void lttng_remove_context_field(struct lttng_ctx **ctx,
				struct lttng_ctx_field *field);
void lttng_destroy_context(struct lttng_ctx *ctx);
int lttng_add_pid_to_ctx(struct lttng_ctx **ctx);
int lttng_add_cpu_id_to_ctx(struct lttng_ctx **ctx);
int lttng_add_procname_to_ctx(struct lttng_ctx **ctx);
int lttng_add_prio_to_ctx(struct lttng_ctx **ctx);
int lttng_add_nice_to_ctx(struct lttng_ctx **ctx);
int lttng_add_vpid_to_ctx(struct lttng_ctx **ctx);
int lttng_add_tid_to_ctx(struct lttng_ctx **ctx);
int lttng_add_vtid_to_ctx(struct lttng_ctx **ctx);
int lttng_add_ppid_to_ctx(struct lttng_ctx **ctx);
int lttng_add_vppid_to_ctx(struct lttng_ctx **ctx);
int lttng_add_hostname_to_ctx(struct lttng_ctx **ctx);
int lttng_add_interruptible_to_ctx(struct lttng_ctx **ctx);
int lttng_add_need_reschedule_to_ctx(struct lttng_ctx **ctx);
#if defined(CONFIG_PREEMPT_RT_FULL) || defined(CONFIG_PREEMPT)
int lttng_add_preemptible_to_ctx(struct lttng_ctx **ctx);
#else
static inline
int lttng_add_preemptible_to_ctx(struct lttng_ctx **ctx)
{
	return -ENOSYS;
}
#endif
#ifdef CONFIG_PREEMPT_RT_FULL
int lttng_add_migratable_to_ctx(struct lttng_ctx **ctx);
#else
static inline
int lttng_add_migratable_to_ctx(struct lttng_ctx **ctx)
{
	return -ENOSYS;
}
#endif

int lttng_add_callstack_to_ctx(struct lttng_ctx **ctx, int type);

#if defined(CONFIG_PERF_EVENTS) && (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33))
int lttng_add_perf_counter_to_ctx(uint32_t type,
				  uint64_t config,
				  const char *name,
				  struct lttng_ctx **ctx);
int lttng_cpuhp_perf_counter_online(unsigned int cpu,
		struct lttng_cpuhp_node *node);
int lttng_cpuhp_perf_counter_dead(unsigned int cpu,
		struct lttng_cpuhp_node *node);
#else
static inline
int lttng_add_perf_counter_to_ctx(uint32_t type,
				  uint64_t config,
				  const char *name,
				  struct lttng_ctx **ctx)
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

int lttng_logger_init(void);
void lttng_logger_exit(void);

extern int lttng_statedump_start(struct lttng_session *session);

#ifdef CONFIG_KPROBES
int lttng_kprobes_register(const char *name,
		const char *symbol_name,
		uint64_t offset,
		uint64_t addr,
		struct lttng_event *event);
void lttng_kprobes_unregister(struct lttng_event *event);
void lttng_kprobes_destroy_private(struct lttng_event *event);
#else
static inline
int lttng_kprobes_register(const char *name,
		const char *symbol_name,
		uint64_t offset,
		uint64_t addr,
		struct lttng_event *event)
{
	return -ENOSYS;
}

static inline
void lttng_kprobes_unregister(struct lttng_event *event)
{
}

static inline
void lttng_kprobes_destroy_private(struct lttng_event *event)
{
}
#endif

int lttng_event_add_callsite(struct lttng_event *event,
	struct lttng_kernel_event_callsite *callsite);

#ifdef CONFIG_UPROBES
int lttng_uprobes_register(const char *name,
	int fd, struct lttng_event *event);
int lttng_uprobes_add_callsite(struct lttng_event *event,
	struct lttng_kernel_event_callsite *callsite);
void lttng_uprobes_unregister(struct lttng_event *event);
void lttng_uprobes_destroy_private(struct lttng_event *event);
#else
static inline
int lttng_uprobes_register(const char *name,
	int fd, struct lttng_event *event)
{
	return -ENOSYS;
}

static inline
int lttng_uprobes_add_callsite(struct lttng_event *event,
	struct lttng_kernel_event_callsite *callsite)
{
	return -ENOSYS;
}

static inline
void lttng_uprobes_unregister(struct lttng_event *event)
{
}

static inline
void lttng_uprobes_destroy_private(struct lttng_event *event)
{
}
#endif

#ifdef CONFIG_KRETPROBES
int lttng_kretprobes_register(const char *name,
		const char *symbol_name,
		uint64_t offset,
		uint64_t addr,
		struct lttng_event *event_entry,
		struct lttng_event *event_exit);
void lttng_kretprobes_unregister(struct lttng_event *event);
void lttng_kretprobes_destroy_private(struct lttng_event *event);
int lttng_kretprobes_event_enable_state(struct lttng_event *event,
	int enable);
#else
static inline
int lttng_kretprobes_register(const char *name,
		const char *symbol_name,
		uint64_t offset,
		uint64_t addr,
		struct lttng_event *event_entry,
		struct lttng_event *event_exit)
{
	return -ENOSYS;
}

static inline
void lttng_kretprobes_unregister(struct lttng_event *event)
{
}

static inline
void lttng_kretprobes_destroy_private(struct lttng_event *event)
{
}

static inline
int lttng_kretprobes_event_enable_state(struct lttng_event *event,
	int enable)
{
	return -ENOSYS;
}
#endif

#if defined(CONFIG_DYNAMIC_FTRACE) && !defined(LTTNG_FTRACE_MISSING_HEADER)
int lttng_ftrace_register(const char *name,
			  const char *symbol_name,
			  struct lttng_event *event);
void lttng_ftrace_unregister(struct lttng_event *event);
void lttng_ftrace_destroy_private(struct lttng_event *event);
#else
static inline
int lttng_ftrace_register(const char *name,
			  const char *symbol_name,
			  struct lttng_event *event)
{
	return -ENOSYS;
}

static inline
void lttng_ftrace_unregister(struct lttng_event *event)
{
}

static inline
void lttng_ftrace_destroy_private(struct lttng_event *event)
{
}
#endif

int lttng_calibrate(struct lttng_kernel_calibrate *calibrate);

extern const struct file_operations lttng_tracepoint_list_fops;
extern const struct file_operations lttng_syscall_list_fops;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35))
#define TRACEPOINT_HAS_DATA_ARG
#endif

#endif /* _LTTNG_EVENTS_H */
