/*
 * ltt-events.c
 *
 * Copyright 2010 (c) - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * Holds LTTng per-session event registry.
 */

#include <linux/module.h>
#include "ltt-events.h"

static LIST_HEAD(sessions);
static DEFINE_MUTEX(sessions_mutex);
static struct kmem_cache *event_cache;

static void synchronize_trace(void)
{
	synchronize_sched();
#ifdef CONFIG_PREEMPT_RT
	synchronize_rcu();
#endif
}

struct ltt_session *ltt_session_create(char *name)
{
	struct ltt_session *session;

	mutex_lock(&sessions_mutex);
	list_for_each_entry(session, &sessions, list)
		if (!strcmp(session->name, name))
			goto exist;
	session = kmalloc(sizeof(struct ltt_session) + strlen(name) + 1);
	if (!session)
		return NULL;
	strcpy(session->name, name);
	INIT_LIST_HEAD(&session->chan);
	list_add(&session->list, &sessions);
	mutex_unlock(&sessions_mutex);
	return session;

exist:
	mutex_unlock(&sessions_mutex);
	return NULL;
}

int ltt_session_destroy(struct ltt_session *session)
{
	struct ltt_channel *chan, *tmpchan;
	struct ltt_event *event, *tmpevent;

	mutex_lock(&sessions_mutex);
	session->active = 0;
	synchronize_trace();	/* Wait for in-flight events to complete */
	list_for_each_entry_safe(event, tmpevent, &session->events, list)
		_ltt_event_destroy(event);
	list_for_each_entry_safe(chan, tmpchan, &session->chan, list)
		_ltt_channel_destroy(chan);
	list_del(&session->list);
	mutex_unlock(&sessions_mutex);
	kfree(session);
}

struct ltt_channel *ltt_channel_create(struct ltt_session *session, char *name,
				       int overwrite, void *buf_addr,
				       size_t subbuf_size, size_t num_subbuf,
				       unsigned int switch_timer_interval,
				       unsigned int read_timer_interval)
{
	struct ltt_channel *chan;

	mutex_lock(&sessions_mutex);
	if (session->active)
		goto active;	/* Refuse to add channel to active session */
	list_for_each_entry(chan, &session->chan, list)
		if (!strcmp(chan->name, name))
			goto exist;
	chan = kmalloc(sizeof(struct ltt_channel) + strlen(name) + 1, GFP_KERNEL);
	if (!chan)
		return NULL;
	strcpy(chan->name, name);
	chan->session = session;

	/* TODO: create rb channel */
	list_add(&chan->list, &session->chan);
	mutex_unlock(&sessions_mutex);
	return chan;

exist:
active:
	mutex_unlock(&sessions_mutex);
	return NULL;
}

/*
 * Only used internally at session destruction.
 */
int _ltt_channel_destroy(struct ltt_channel *chan)
{
	/* TODO: destroy rb channel */
	list_del(&chan->list);
	kfree(chan);
}

/*
 * Supports event creation while tracing session is active.
 */
struct ltt_event *ltt_event_create(struct ltt_channel *chan, char *name,
				   void *filter)
{
	struct ltt_event *event;
	int ret;

	mutex_lock(&sessions_mutex);
	if (chan->free_event_id == -1UL)
		goto full;
	/*
	 * This is O(n^2) (for each event loop called at event creation).
	 * Might require a hash if we have lots of events.
	 */
	list_for_each_entry(event, &chan->session->events, list)
		if (!strcmp(event->name, name))
			goto exist;
	event = kmem_cache_zalloc(events_cache, GFP_KERNEL);
	if (!event)
		goto cache_error;
	event->name = kmalloc(strlen(name) + 1, GFP_KERNEL);
	if (!event->name)
		goto error;
	strcpy(event->name, name);
	event->chan = chan;
	event->filter = filter;
	event->id = chan->free_event_id++;
	mutex_unlock(&sessions_mutex);
	/* Populate ltt_event structure before tracepoint registration. */
	smp_wmb();
	return event;

error:
	kmem_cache_free(event);
cache_error:
exist:
full:
	mutex_unlock(&sessions_mutex);
	return NULL;
}

/*
 * Only used internally at session destruction.
 */
int _ltt_event_destroy(struct ltt_event *event)
{
	/* TODO unregister from tracepoint */
	kfree(event->name);
	kmem_cache_free(event);
}

static int __init ltt_events_init(void)
{
	int ret;

	events_cache = KMEM_CACHE(ltt_event, 0);
	if (!events_cache)
		return -ENOMEM;

	/* TODO: show ABI to userspace */

	return 0;
}

static void __exit ltt_events_exit(void)
{
	struct ltt_session *session, *tmpsession;

	/* TODO: hide ABI from userspace, wait for callers to release refs. */

	list_for_each_entry_safe(session, tmpsession, &sessions, list)
		ltt_session_destroy(session);
	kmem_cache_destroy(events_cache);
}

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng Events");
