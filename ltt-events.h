#ifndef _LTT_EVENTS_H
#define _LTT_EVENTS_H

/*
 * ltt-events.h
 *
 * Copyright 2010 (c) - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * Holds LTTng per-session event registry.
 */

#include <linux/list.h>
#include <linux/uuid.h>
#include "ltt-debugfs-abi.h"

struct ltt_channel;
struct ltt_session;
struct lib_ring_buffer_ctx;

/* Type description */

/* Update the astract_types name table in lttng-types.c along with this enum */
enum abstract_types {
	atype_integer,
	atype_enum,
	atype_array,
	atype_sequence,
	atype_string,
	NR_ABSTRACT_TYPES,
};

/* Update the string_encodings name table in lttng-types.c along with this enum */
enum lttng_string_encodings {
	lttng_encode_UTF8 = 0,
	lttng_encode_ASCII = 1,
	NR_STRING_ENCODINGS,
};

struct lttng_enum_entry {
	unsigned long long start, end;	/* start and end are inclusive */
	const char *string;
};

#define __type_integer(_type, _byte_order)			\
	{							\
	    .atype = atype_integer,				\
	    .u.basic.integer =					\
		{						\
		  .size = sizeof(_type),			\
		  .alignment = ltt_alignof(_type) * CHAR_BIT,	\
		  .signedness = is_signed_type(_type),		\
		  .reverse_byte_order = _byte_order != __BYTE_ORDER,	\
		},						\
	}							\

struct lttng_integer_type {
	unsigned int size;		/* in bits */
	unsigned short alignment;	/* in bits */
	unsigned int signedness:1;
	unsigned int reverse_byte_order:1;
};

union _lttng_basic_type {
	struct lttng_integer_type integer;
	struct {
		const char *name;
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
		} array;
		struct {
			struct lttng_basic_type length_type;
			struct lttng_basic_type elem_type;
		} sequence;
	} u;
};

struct lttng_enum {
	const char *name;
	struct lttng_type container_type;
	const struct lttng_enum_entry *entries;
	unsigned int len;
};

/* Event field description */

struct lttng_event_field {
	const char *name;
	const struct lttng_type type;
};

struct lttng_event_desc {
	const struct lttng_event_field *fields;
	const char *name;
	void *probe_callback;
	unsigned int nr_fields;
};

struct lttng_probe_desc {
	const struct lttng_event_desc *event_desc;
	unsigned int nr_events;
	struct list_head head;			/* chain registered probes */
};

/*
 * ltt_event structure is referred to by the tracing fast path. It must be
 * kept small.
 */
struct ltt_event {
	unsigned int id;
	struct ltt_channel *chan;
	const struct lttng_event_desc *desc;
	void *filter;
	enum instrum_type itype;
	struct list_head list;		/* Event list */
	int metadata_dumped:1;
};

struct ltt_channel_ops {
	struct channel *(*channel_create)(const char *name,
				struct ltt_session *session,
				void *buf_addr,
				size_t subbuf_size, size_t num_subbuf,
				unsigned int switch_timer_interval,
				unsigned int read_timer_interval);
	void (*channel_destroy)(struct channel *chan);
	struct lib_ring_buffer *(*buffer_read_open)(struct channel *chan);
	void (*buffer_read_close)(struct lib_ring_buffer *buf);
	int (*event_reserve)(struct lib_ring_buffer_ctx *ctx);
	void (*event_commit)(struct lib_ring_buffer_ctx *ctx);
	void (*event_write)(struct lib_ring_buffer_ctx *ctx, const void *src,
			    size_t len);
	/*
	 * packet_avail_size returns the available size in the current
	 * packet. Note that the size returned is only a hint, since it
	 * may change due to concurrent writes.
	 */
	size_t (*packet_avail_size)(struct channel *chan);
	wait_queue_head_t *(*get_reader_wait_queue)(struct ltt_channel *chan);
};

struct ltt_channel {
	unsigned int id;
	struct channel *chan;		/* Channel buffers */
	/* Event ID management */
	struct ltt_session *session;
	struct file *file;		/* File associated to channel */
	unsigned int free_event_id;	/* Next event ID to allocate */
	struct list_head list;		/* Channel list */
	wait_queue_head_t notify_wait;	/* Channel addition notif. waitqueue */
	struct ltt_channel_ops *ops;
	int metadata_dumped:1;
	int header_type:2;		/* 0: unset, 1: compact, 2: large */
};

struct ltt_session {
	int active;			/* Is trace session active ? */
	struct file *file;		/* File associated to session */
	struct ltt_channel *metadata;	/* Metadata channel */
	struct list_head chan;		/* Channel list head */
	struct list_head events;	/* Event list head */
	struct list_head list;		/* Session list */
	unsigned int free_chan_id;	/* Next chan ID to allocate */
	uuid_le uuid;			/* Trace session unique ID */
	int metadata_dumped:1;
};

struct ltt_transport {
	char *name;
	struct module *owner;
	struct list_head node;
	struct ltt_channel_ops ops;
};

struct ltt_session *ltt_session_create(void);
int ltt_session_start(struct ltt_session *session);
int ltt_session_stop(struct ltt_session *session);
void ltt_session_destroy(struct ltt_session *session);

struct ltt_channel *ltt_channel_create(struct ltt_session *session,
				       const char *transport_name,
				       void *buf_addr,
				       size_t subbuf_size, size_t num_subbuf,
				       unsigned int switch_timer_interval,
				       unsigned int read_timer_interval);
struct ltt_channel *ltt_global_channel_create(struct ltt_session *session,
				       int overwrite, void *buf_addr,
				       size_t subbuf_size, size_t num_subbuf,
				       unsigned int switch_timer_interval,
				       unsigned int read_timer_interval);
void _ltt_channel_destroy(struct ltt_channel *chan);

struct ltt_event *ltt_event_create(struct ltt_channel *chan,
				   char *name,
				   enum instrum_type itype,
				   const struct lttng_event_desc *event_desc,
				   void *filter);
int ltt_event_unregister(struct ltt_event *event);

void ltt_transport_register(struct ltt_transport *transport);
void ltt_transport_unregister(struct ltt_transport *transport);

int ltt_debugfs_abi_init(void);
void ltt_debugfs_abi_exit(void);

int ltt_probe_register(struct lttng_probe_desc *desc);
void ltt_probe_unregister(struct lttng_probe_desc *desc);
const struct lttng_event_desc *ltt_event_get(const char *name);
void ltt_event_put(const struct lttng_event_desc *desc);
int ltt_probes_init(void);
void ltt_probes_exit(void);

#endif /* _LTT_EVENTS_H */
