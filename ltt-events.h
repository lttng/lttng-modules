/*
 * ltt-events.h
 *
 * Copyright 2010 (c) - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * Holds LTTng per-session event registry.
 */

#include <linux/list.h>

struct ltt_channel;
struct ltt_session;

enum instrum_type itype {
	INSTRUM_TRACEPOINTS,
};

/*
 * ltt_event structure is referred to by the tracing fast path. It must be
 * kept small.
 */
struct ltt_event {
	unsigned int id;
	struct ltt_channel *chan;
	void *probe;
	void *filter;
	char *name;
	enum instrum_type itype;
	struct list_head list;		/* Event list */
};

struct ltt_channel {
	struct channel *chan;		/* Channel buffers */
	/* Event ID management */
	struct ltt_session *session;
	struct file *file;		/* File associated to channel */
	unsigned int free_event_id;	/* Next event ID to allocate */
	struct list_head list;		/* Channel list */
	wait_queue_head_t notify_wait;	/* Channel addition notif. waitqueue */
};

struct ltt_session {
	int active;			/* Is trace session active ? */
	struct file *file;		/* File associated to session */
	struct list_head chan;		/* Channel list head */
	struct list_head events;	/* Event list head */
	struct list_head list;		/* Session list */
};

struct ltt_trace_ops {
	struct channel *(*channel_create)(const char *name,
				struct ltt_trace *trace,
				void *buf_addr,
				size_t subbuf_size, size_t num_subbuf,
				unsigned int switch_timer_interval,
				unsigned int read_timer_interval);
	void (*channel_destroy)(struct channel *chan);
	struct lib_ring_buffer *(*buffer_read_open)(struct channel *chan);
	struct lib_ring_buffer *(*buffer_read_close)(struct lib_ring_buffer *buf);
};

struct ltt_transport {
	char *name;
	struct module *owner;
	struct list_head node;
	struct ltt_trace_ops ops;
};

struct ltt_session *ltt_session_create(void);
int ltt_session_start(struct ltt_session *session);
int ltt_session_stop(struct ltt_session *session);
int ltt_session_destroy(struct ltt_session *session);

struct ltt_channel *ltt_channel_create(struct ltt_session *session,
				       int overwrite, void *buf_addr,
				       size_t subbuf_size, size_t num_subbuf,
				       unsigned int switch_timer_interval,
				       unsigned int read_timer_interval);
int _ltt_channel_destroy(struct ltt_channel *chan);

struct ltt_event *ltt_event_create(struct ltt_channel *chan,
				   enum instrum_type itype,
				   char *name,
				   void *filter);
int _ltt_event_destroy(struct ltt_event *event);

void ltt_transport_register(struct ltt_transport *transport);
void ltt_transport_unregister(struct ltt_transport *transport);
