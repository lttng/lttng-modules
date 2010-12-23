/*
 * ltt-events.c
 *
 * Copyright 2010 (c) - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * Holds LTTng per-session event registry.
 */

#include <linux/module.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>	/* For vmalloc_sync_all */
#include "ltt-events.h"

static LIST_HEAD(sessions);
static LIST_HEAD(ltt_transport_list);
static DEFINE_MUTEX(sessions_mutex);
static struct kmem_cache *event_cache;

static void synchronize_trace(void)
{
	synchronize_sched();
#ifdef CONFIG_PREEMPT_RT
	synchronize_rcu();
#endif
}

struct ltt_session *ltt_session_create(void)
{
	struct ltt_session *session;

	mutex_lock(&sessions_mutex);
	session = kzalloc(sizeof(struct ltt_session), GFP_KERNEL);
	if (!session)
		return NULL;
	INIT_LIST_HEAD(&session->chan);
	INIT_LIST_HEAD(&session->events);
	list_add(&session->list, &sessions);
	mutex_unlock(&sessions_mutex);
	return session;
}

void ltt_session_destroy(struct ltt_session *session)
{
	struct ltt_channel *chan, *tmpchan;
	struct ltt_event *event, *tmpevent;
	int ret;

	mutex_lock(&sessions_mutex);
	ACCESS_ONCE(session->active) = 0;
	list_for_each_entry(event, &session->events, list) {
		ret = _ltt_event_unregister(event);
		WARN_ON(ret);
	}
	synchronize_trace();	/* Wait for in-flight events to complete */
	list_for_each_entry_safe(event, tmpevent, &session->events, list)
		_ltt_event_destroy(event);
	list_for_each_entry_safe(chan, tmpchan, &session->chan, list)
		_ltt_channel_destroy(chan);
	list_del(&session->list);
	mutex_unlock(&sessions_mutex);
	kfree(session);
}

int ltt_session_start(struct ltt_session *session)
{
	int ret = 0;

	mutex_lock(&sessions_mutex);
	if (session->active) {
		ret = -EBUSY;
		goto end;
	}
	ACCESS_ONCE(session->active) = 1;
	synchronize_trace();	/* Wait for in-flight events to complete */
end:
	mutex_unlock(&sessions_mutex);
	return ret;
}

int ltt_session_stop(struct ltt_session *session)
{
	int ret = 0;

	mutex_lock(&sessions_mutex);
	if (!session->active) {
		ret = -EBUSY;
		goto end;
	}
	ACCESS_ONCE(session->active) = 0;
	synchronize_trace();	/* Wait for in-flight events to complete */
end:
	mutex_unlock(&sessions_mutex);
	return ret;
}

static struct ltt_transport *ltt_transport_find(char *name)
{
	struct ltt_transport *transport;

	list_for_each_entry(transport, &ltt_transport_list, node) {
		if (!strcmp(transport->name, name))
			return transport;
	}
	return NULL;
}

struct ltt_channel *ltt_channel_create(struct ltt_session *session,
				       int overwrite, void *buf_addr,
				       size_t subbuf_size, size_t num_subbuf,
				       unsigned int switch_timer_interval,
				       unsigned int read_timer_interval)
{
	struct ltt_channel *chan;
	struct ltt_transport *transport;
	char *transport_name;

	mutex_lock(&sessions_mutex);
	if (session->active) {
		printk(KERN_WARNING "LTTng refusing to add channel to active session\n");
		goto active;	/* Refuse to add channel to active session */
	}
	transport_name = overwrite ? "relay-overwrite" : "relay-discard";
	transport = ltt_transport_find(transport_name);
	if (!transport) {
		printk(KERN_WARNING "LTTng transport %s not found\n",
		       transport_name);
		goto notransport;
	}
	chan = kzalloc(sizeof(struct ltt_channel), GFP_KERNEL);
	if (!chan)
		goto nomem;
	chan->session = session;
	init_waitqueue_head(&chan->notify_wait);
	chan->chan = transport->ops.channel_create("[lttng]", session, buf_addr,
			subbuf_size, num_subbuf, switch_timer_interval,
			read_timer_interval);
	if (!chan->chan)
		goto create_error;
	chan->ops = &transport->ops;
	list_add(&chan->list, &session->chan);
	mutex_unlock(&sessions_mutex);
	return chan;

create_error:
	kfree(chan);
nomem:
notransport:
active:
	mutex_unlock(&sessions_mutex);
	return NULL;
}

/*
 * Only used internally at session destruction.
 */
void _ltt_channel_destroy(struct ltt_channel *chan)
{
	chan->ops->channel_destroy(chan->chan);
	list_del(&chan->list);
	kfree(chan);
}

/*
 * Supports event creation while tracing session is active.
 */
struct ltt_event *ltt_event_create(struct ltt_channel *chan, char *name,
				   enum instrum_type itype,
				   void *probe, void *filter)
{
	struct ltt_event *event;
	int ret;

	mutex_lock(&sessions_mutex);
	if (chan->free_event_id == -1UL)
		goto full;
	/*
	 * This is O(n^2) (for each event, the loop is called at event
	 * creation). Might require a hash if we have lots of events.
	 */
	list_for_each_entry(event, &chan->session->events, list)
		if (!strcmp(event->name, name))
			goto exist;
	event = kmem_cache_zalloc(event_cache, GFP_KERNEL);
	if (!event)
		goto cache_error;
	event->name = kmalloc(strlen(name) + 1, GFP_KERNEL);
	if (!event->name)
		goto name_error;
	strcpy(event->name, name);
	event->chan = chan;
	event->probe = probe;
	event->filter = filter;
	event->id = chan->free_event_id++;
	event->itype = itype;
	/* Populate ltt_event structure before tracepoint registration. */
	smp_wmb();
	switch (itype) {
	case INSTRUM_TRACEPOINTS:
		ret = tracepoint_probe_register(name, probe, event);
		if (ret)
			goto register_error;
		break;
	default:
		WARN_ON_ONCE(1);
	}
	list_add(&event->list, &chan->session->events);
	mutex_unlock(&sessions_mutex);
	return event;

register_error:
	kfree(event->name);
name_error:
	kmem_cache_free(event_cache, event);
cache_error:
exist:
full:
	mutex_unlock(&sessions_mutex);
	return NULL;
}

/*
 * Only used internally at session destruction.
 */
int _ltt_event_unregister(struct ltt_event *event)
{
	int ret = -EINVAL;

	switch (event->itype) {
	case INSTRUM_TRACEPOINTS:
		ret = tracepoint_probe_unregister(event->name, event->probe,
						  event);
		if (ret)
			return ret;
		break;
	default:
		WARN_ON_ONCE(1);
	}
	return ret;
}

/*
 * Only used internally at session destruction.
 */
void _ltt_event_destroy(struct ltt_event *event)
{
	kfree(event->name);
	list_del(&event->list);
	kmem_cache_free(event_cache, event);
}

/**
 * ltt_transport_register - LTT transport registration
 * @transport: transport structure
 *
 * Registers a transport which can be used as output to extract the data out of
 * LTTng. The module calling this registration function must ensure that no
 * trap-inducing code will be executed by the transport functions. E.g.
 * vmalloc_sync_all() must be called between a vmalloc and the moment the memory
 * is made visible to the transport function. This registration acts as a
 * vmalloc_sync_all. Therefore, only if the module allocates virtual memory
 * after its registration must it synchronize the TLBs.
 */
void ltt_transport_register(struct ltt_transport *transport)
{
	/*
	 * Make sure no page fault can be triggered by the module about to be
	 * registered. We deal with this here so we don't have to call
	 * vmalloc_sync_all() in each module's init.
	 */
	vmalloc_sync_all();

	mutex_lock(&sessions_mutex);
	list_add_tail(&transport->node, &ltt_transport_list);
	mutex_unlock(&sessions_mutex);
}
EXPORT_SYMBOL_GPL(ltt_transport_register);

/**
 * ltt_transport_unregister - LTT transport unregistration
 * @transport: transport structure
 */
void ltt_transport_unregister(struct ltt_transport *transport)
{
	mutex_lock(&sessions_mutex);
	list_del(&transport->node);
	mutex_unlock(&sessions_mutex);
}
EXPORT_SYMBOL_GPL(ltt_transport_unregister);


static int __init ltt_events_init(void)
{
	int ret;

	event_cache = KMEM_CACHE(ltt_event, 0);
	if (!event_cache)
		return -ENOMEM;
	ret = ltt_probes_init();
	if (ret)
		goto error;
	ret = ltt_debugfs_abi_init();
	if (ret)
		goto error_abi;
	return 0;
error_abi:
	ltt_probes_exit();
error:
	kmem_cache_destroy(event_cache);
	return ret;
}

module_init(ltt_events_init);

static void __exit ltt_events_exit(void)
{
	struct ltt_session *session, *tmpsession;

	ltt_debugfs_abi_exit();
	ltt_probes_exit();
	list_for_each_entry_safe(session, tmpsession, &sessions, list)
		ltt_session_destroy(session);
	kmem_cache_destroy(event_cache);
}

module_exit(ltt_events_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng Events");
