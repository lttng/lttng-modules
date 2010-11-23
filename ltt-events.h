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

/*
 * ltt_event structure is referred to by the tracing fast path. It must be
 * kept small.
 */
struct ltt_event {
	unsigned int id;
	struct ltt_channel *chan;
	void *filter;
	char *name;
	struct list_head list;		/* Event list */
};

struct ltt_channel {
	struct channel *chan;		/* Channel buffers */
	/* Event ID management */
	struct ltt_session *session;
	atomic_t free_event_id;		/* Next event ID to allocate */
	struct list_head list;		/* Channel list */
	char name[PATH_MAX];
};

struct ltt_session {
	struct list_head chan;		/* Channel list head */
	struct list_head events;	/* Event list head */
	struct list_head list;		/* Session list */
	char name[PATH_MAX];
};

struct ltt_session *ltt_session_create(char *name);
int ltt_session_destroy(struct ltt_session *session);

struct ltt_channel *ltt_channel_create(struct ltt_session *session, char *name,
				       int overwrite, void *buf_addr,
				       size_t subbuf_size, size_t num_subbuf,
				       unsigned int switch_timer_interval,
				       unsigned int read_timer_interval);
int _ltt_channel_destroy(struct ltt_channel *chan);

struct ltt_event *ltt_event_create(struct ltt_channel *chan, char *name,
				   void *filter);
int _ltt_event_destroy(struct ltt_event *event);
