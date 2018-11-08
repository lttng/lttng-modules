/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-events.c
 *
 * Holds LTTng per-session event registry.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

/*
 * This page_alloc.h wrapper needs to be included before gfpflags.h because it
 * overrides a function with a define.
 */
#include "wrapper/page_alloc.h"

#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/utsname.h>
#include <linux/err.h>
#include <linux/seq_file.h>
#include <linux/file.h>
#include <linux/anon_inodes.h>
#include <wrapper/file.h>
#include <linux/jhash.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/uuid.h>

#include <wrapper/vmalloc.h>	/* for wrapper_vmalloc_sync_all() */
#include <wrapper/random.h>
#include <wrapper/tracepoint.h>
#include <wrapper/list.h>
#include <wrapper/types.h>
#include <lttng-kernel-version.h>
#include <lttng-events.h>
#include <lttng-tracer.h>
#include <lttng-abi-old.h>
#include <lttng-endian.h>
#include <lttng-string-utils.h>
#include <wrapper/ringbuffer/backend.h>
#include <wrapper/ringbuffer/frontend.h>

#define METADATA_CACHE_DEFAULT_SIZE 4096

static LIST_HEAD(sessions);
static LIST_HEAD(lttng_transport_list);
/*
 * Protect the sessions and metadata caches.
 */
static DEFINE_MUTEX(sessions_mutex);
static struct kmem_cache *event_cache;

static void lttng_session_lazy_sync_enablers(struct lttng_session *session);
static void lttng_session_sync_enablers(struct lttng_session *session);
static void lttng_enabler_destroy(struct lttng_enabler *enabler);

static void _lttng_event_destroy(struct lttng_event *event);
static void _lttng_channel_destroy(struct lttng_channel *chan);
static int _lttng_event_unregister(struct lttng_event *event);
static
int _lttng_event_metadata_statedump(struct lttng_session *session,
				  struct lttng_channel *chan,
				  struct lttng_event *event);
static
int _lttng_session_metadata_statedump(struct lttng_session *session);
static
void _lttng_metadata_channel_hangup(struct lttng_metadata_stream *stream);
static
int _lttng_field_statedump(struct lttng_session *session,
		const struct lttng_event_field *field,
		size_t nesting);

void synchronize_trace(void)
{
	synchronize_sched();
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
#ifdef CONFIG_PREEMPT_RT_FULL
	synchronize_rcu();
#endif
#else /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)) */
#ifdef CONFIG_PREEMPT_RT
	synchronize_rcu();
#endif
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)) */
}

void lttng_lock_sessions(void)
{
	mutex_lock(&sessions_mutex);
}

void lttng_unlock_sessions(void)
{
	mutex_unlock(&sessions_mutex);
}

/*
 * Called with sessions lock held.
 */
int lttng_session_active(void)
{
	struct lttng_session *iter;

	list_for_each_entry(iter, &sessions, list) {
		if (iter->active)
			return 1;
	}
	return 0;
}

struct lttng_session *lttng_session_create(void)
{
	struct lttng_session *session;
	struct lttng_metadata_cache *metadata_cache;
	int i;

	mutex_lock(&sessions_mutex);
	session = lttng_kvzalloc(sizeof(struct lttng_session), GFP_KERNEL);
	if (!session)
		goto err;
	INIT_LIST_HEAD(&session->chan);
	INIT_LIST_HEAD(&session->events);
	uuid_le_gen(&session->uuid);

	metadata_cache = kzalloc(sizeof(struct lttng_metadata_cache),
			GFP_KERNEL);
	if (!metadata_cache)
		goto err_free_session;
	metadata_cache->data = vzalloc(METADATA_CACHE_DEFAULT_SIZE);
	if (!metadata_cache->data)
		goto err_free_cache;
	metadata_cache->cache_alloc = METADATA_CACHE_DEFAULT_SIZE;
	kref_init(&metadata_cache->refcount);
	mutex_init(&metadata_cache->lock);
	session->metadata_cache = metadata_cache;
	INIT_LIST_HEAD(&metadata_cache->metadata_stream);
	memcpy(&metadata_cache->uuid, &session->uuid,
		sizeof(metadata_cache->uuid));
	INIT_LIST_HEAD(&session->enablers_head);
	for (i = 0; i < LTTNG_EVENT_HT_SIZE; i++)
		INIT_HLIST_HEAD(&session->events_ht.table[i]);
	list_add(&session->list, &sessions);
	mutex_unlock(&sessions_mutex);
	return session;

err_free_cache:
	kfree(metadata_cache);
err_free_session:
	lttng_kvfree(session);
err:
	mutex_unlock(&sessions_mutex);
	return NULL;
}

void metadata_cache_destroy(struct kref *kref)
{
	struct lttng_metadata_cache *cache =
		container_of(kref, struct lttng_metadata_cache, refcount);
	vfree(cache->data);
	kfree(cache);
}

void lttng_session_destroy(struct lttng_session *session)
{
	struct lttng_channel *chan, *tmpchan;
	struct lttng_event *event, *tmpevent;
	struct lttng_metadata_stream *metadata_stream;
	struct lttng_enabler *enabler, *tmpenabler;
	int ret;

	mutex_lock(&sessions_mutex);
	WRITE_ONCE(session->active, 0);
	list_for_each_entry(chan, &session->chan, list) {
		ret = lttng_syscalls_unregister(chan);
		WARN_ON(ret);
	}
	list_for_each_entry(event, &session->events, list) {
		ret = _lttng_event_unregister(event);
		WARN_ON(ret);
	}
	synchronize_trace();	/* Wait for in-flight events to complete */
	list_for_each_entry_safe(enabler, tmpenabler,
			&session->enablers_head, node)
		lttng_enabler_destroy(enabler);
	list_for_each_entry_safe(event, tmpevent, &session->events, list)
		_lttng_event_destroy(event);
	list_for_each_entry_safe(chan, tmpchan, &session->chan, list) {
		BUG_ON(chan->channel_type == METADATA_CHANNEL);
		_lttng_channel_destroy(chan);
	}
	list_for_each_entry(metadata_stream, &session->metadata_cache->metadata_stream, list)
		_lttng_metadata_channel_hangup(metadata_stream);
	if (session->pid_tracker)
		lttng_pid_tracker_destroy(session->pid_tracker);
	kref_put(&session->metadata_cache->refcount, metadata_cache_destroy);
	list_del(&session->list);
	mutex_unlock(&sessions_mutex);
	lttng_kvfree(session);
}

int lttng_session_statedump(struct lttng_session *session)
{
	int ret;

	mutex_lock(&sessions_mutex);
	ret = lttng_statedump_start(session);
	mutex_unlock(&sessions_mutex);
	return ret;
}

int lttng_session_enable(struct lttng_session *session)
{
	int ret = 0;
	struct lttng_channel *chan;

	mutex_lock(&sessions_mutex);
	if (session->active) {
		ret = -EBUSY;
		goto end;
	}

	/* Set transient enabler state to "enabled" */
	session->tstate = 1;

	/* We need to sync enablers with session before activation. */
	lttng_session_sync_enablers(session);

	/*
	 * Snapshot the number of events per channel to know the type of header
	 * we need to use.
	 */
	list_for_each_entry(chan, &session->chan, list) {
		if (chan->header_type)
			continue;		/* don't change it if session stop/restart */
		if (chan->free_event_id < 31)
			chan->header_type = 1;	/* compact */
		else
			chan->header_type = 2;	/* large */
	}

	/* Clear each stream's quiescent state. */
	list_for_each_entry(chan, &session->chan, list) {
		if (chan->channel_type != METADATA_CHANNEL)
			lib_ring_buffer_clear_quiescent_channel(chan->chan);
	}

	WRITE_ONCE(session->active, 1);
	WRITE_ONCE(session->been_active, 1);
	ret = _lttng_session_metadata_statedump(session);
	if (ret) {
		WRITE_ONCE(session->active, 0);
		goto end;
	}
	ret = lttng_statedump_start(session);
	if (ret)
		WRITE_ONCE(session->active, 0);
end:
	mutex_unlock(&sessions_mutex);
	return ret;
}

int lttng_session_disable(struct lttng_session *session)
{
	int ret = 0;
	struct lttng_channel *chan;

	mutex_lock(&sessions_mutex);
	if (!session->active) {
		ret = -EBUSY;
		goto end;
	}
	WRITE_ONCE(session->active, 0);

	/* Set transient enabler state to "disabled" */
	session->tstate = 0;
	lttng_session_sync_enablers(session);

	/* Set each stream's quiescent state. */
	list_for_each_entry(chan, &session->chan, list) {
		if (chan->channel_type != METADATA_CHANNEL)
			lib_ring_buffer_set_quiescent_channel(chan->chan);
	}
end:
	mutex_unlock(&sessions_mutex);
	return ret;
}

int lttng_session_metadata_regenerate(struct lttng_session *session)
{
	int ret = 0;
	struct lttng_channel *chan;
	struct lttng_event *event;
	struct lttng_metadata_cache *cache = session->metadata_cache;
	struct lttng_metadata_stream *stream;

	mutex_lock(&sessions_mutex);
	if (!session->active) {
		ret = -EBUSY;
		goto end;
	}

	mutex_lock(&cache->lock);
	memset(cache->data, 0, cache->cache_alloc);
	cache->metadata_written = 0;
	cache->version++;
	list_for_each_entry(stream, &session->metadata_cache->metadata_stream, list) {
		stream->metadata_out = 0;
		stream->metadata_in = 0;
	}
	mutex_unlock(&cache->lock);

	session->metadata_dumped = 0;
	list_for_each_entry(chan, &session->chan, list) {
		chan->metadata_dumped = 0;
	}

	list_for_each_entry(event, &session->events, list) {
		event->metadata_dumped = 0;
	}

	ret = _lttng_session_metadata_statedump(session);

end:
	mutex_unlock(&sessions_mutex);
	return ret;
}

int lttng_channel_enable(struct lttng_channel *channel)
{
	int ret = 0;

	mutex_lock(&sessions_mutex);
	if (channel->channel_type == METADATA_CHANNEL) {
		ret = -EPERM;
		goto end;
	}
	if (channel->enabled) {
		ret = -EEXIST;
		goto end;
	}
	/* Set transient enabler state to "enabled" */
	channel->tstate = 1;
	lttng_session_sync_enablers(channel->session);
	/* Set atomically the state to "enabled" */
	WRITE_ONCE(channel->enabled, 1);
end:
	mutex_unlock(&sessions_mutex);
	return ret;
}

int lttng_channel_disable(struct lttng_channel *channel)
{
	int ret = 0;

	mutex_lock(&sessions_mutex);
	if (channel->channel_type == METADATA_CHANNEL) {
		ret = -EPERM;
		goto end;
	}
	if (!channel->enabled) {
		ret = -EEXIST;
		goto end;
	}
	/* Set atomically the state to "disabled" */
	WRITE_ONCE(channel->enabled, 0);
	/* Set transient enabler state to "enabled" */
	channel->tstate = 0;
	lttng_session_sync_enablers(channel->session);
end:
	mutex_unlock(&sessions_mutex);
	return ret;
}

int lttng_event_enable(struct lttng_event *event)
{
	int ret = 0;

	mutex_lock(&sessions_mutex);
	if (event->chan->channel_type == METADATA_CHANNEL) {
		ret = -EPERM;
		goto end;
	}
	if (event->enabled) {
		ret = -EEXIST;
		goto end;
	}
	switch (event->instrumentation) {
	case LTTNG_KERNEL_TRACEPOINT:
	case LTTNG_KERNEL_SYSCALL:
		ret = -EINVAL;
		break;
	case LTTNG_KERNEL_KPROBE:
	case LTTNG_KERNEL_FUNCTION:
	case LTTNG_KERNEL_UPROBE:
	case LTTNG_KERNEL_NOOP:
		WRITE_ONCE(event->enabled, 1);
		break;
	case LTTNG_KERNEL_KRETPROBE:
		ret = lttng_kretprobes_event_enable_state(event, 1);
		break;
	default:
		WARN_ON_ONCE(1);
		ret = -EINVAL;
	}
end:
	mutex_unlock(&sessions_mutex);
	return ret;
}

int lttng_event_disable(struct lttng_event *event)
{
	int ret = 0;

	mutex_lock(&sessions_mutex);
	if (event->chan->channel_type == METADATA_CHANNEL) {
		ret = -EPERM;
		goto end;
	}
	if (!event->enabled) {
		ret = -EEXIST;
		goto end;
	}
	switch (event->instrumentation) {
	case LTTNG_KERNEL_TRACEPOINT:
	case LTTNG_KERNEL_SYSCALL:
		ret = -EINVAL;
		break;
	case LTTNG_KERNEL_KPROBE:
	case LTTNG_KERNEL_FUNCTION:
	case LTTNG_KERNEL_UPROBE:
	case LTTNG_KERNEL_NOOP:
		WRITE_ONCE(event->enabled, 0);
		break;
	case LTTNG_KERNEL_KRETPROBE:
		ret = lttng_kretprobes_event_enable_state(event, 0);
		break;
	default:
		WARN_ON_ONCE(1);
		ret = -EINVAL;
	}
end:
	mutex_unlock(&sessions_mutex);
	return ret;
}

static struct lttng_transport *lttng_transport_find(const char *name)
{
	struct lttng_transport *transport;

	list_for_each_entry(transport, &lttng_transport_list, node) {
		if (!strcmp(transport->name, name))
			return transport;
	}
	return NULL;
}

struct lttng_channel *lttng_channel_create(struct lttng_session *session,
				       const char *transport_name,
				       void *buf_addr,
				       size_t subbuf_size, size_t num_subbuf,
				       unsigned int switch_timer_interval,
				       unsigned int read_timer_interval,
				       enum channel_type channel_type)
{
	struct lttng_channel *chan;
	struct lttng_transport *transport = NULL;

	mutex_lock(&sessions_mutex);
	if (session->been_active && channel_type != METADATA_CHANNEL)
		goto active;	/* Refuse to add channel to active session */
	transport = lttng_transport_find(transport_name);
	if (!transport) {
		printk(KERN_WARNING "LTTng transport %s not found\n",
		       transport_name);
		goto notransport;
	}
	if (!try_module_get(transport->owner)) {
		printk(KERN_WARNING "LTT : Can't lock transport module.\n");
		goto notransport;
	}
	chan = kzalloc(sizeof(struct lttng_channel), GFP_KERNEL);
	if (!chan)
		goto nomem;
	chan->session = session;
	chan->id = session->free_chan_id++;
	chan->ops = &transport->ops;
	/*
	 * Note: the channel creation op already writes into the packet
	 * headers. Therefore the "chan" information used as input
	 * should be already accessible.
	 */
	chan->chan = transport->ops.channel_create(transport_name,
			chan, buf_addr, subbuf_size, num_subbuf,
			switch_timer_interval, read_timer_interval);
	if (!chan->chan)
		goto create_error;
	chan->tstate = 1;
	chan->enabled = 1;
	chan->transport = transport;
	chan->channel_type = channel_type;
	list_add(&chan->list, &session->chan);
	mutex_unlock(&sessions_mutex);
	return chan;

create_error:
	kfree(chan);
nomem:
	if (transport)
		module_put(transport->owner);
notransport:
active:
	mutex_unlock(&sessions_mutex);
	return NULL;
}

/*
 * Only used internally at session destruction for per-cpu channels, and
 * when metadata channel is released.
 * Needs to be called with sessions mutex held.
 */
static
void _lttng_channel_destroy(struct lttng_channel *chan)
{
	chan->ops->channel_destroy(chan->chan);
	module_put(chan->transport->owner);
	list_del(&chan->list);
	lttng_destroy_context(chan->ctx);
	kfree(chan);
}

void lttng_metadata_channel_destroy(struct lttng_channel *chan)
{
	BUG_ON(chan->channel_type != METADATA_CHANNEL);

	/* Protect the metadata cache with the sessions_mutex. */
	mutex_lock(&sessions_mutex);
	_lttng_channel_destroy(chan);
	mutex_unlock(&sessions_mutex);
}
EXPORT_SYMBOL_GPL(lttng_metadata_channel_destroy);

static
void _lttng_metadata_channel_hangup(struct lttng_metadata_stream *stream)
{
	stream->finalized = 1;
	wake_up_interruptible(&stream->read_wait);
}

/*
 * Supports event creation while tracing session is active.
 * Needs to be called with sessions mutex held.
 */
struct lttng_event *_lttng_event_create(struct lttng_channel *chan,
				struct lttng_kernel_event *event_param,
				void *filter,
				const struct lttng_event_desc *event_desc,
				enum lttng_kernel_instrumentation itype)
{
	struct lttng_session *session = chan->session;
	struct lttng_event *event;
	const char *event_name;
	struct hlist_head *head;
	size_t name_len;
	uint32_t hash;
	int ret;

	if (chan->free_event_id == -1U) {
		ret = -EMFILE;
		goto full;
	}

	switch (itype) {
	case LTTNG_KERNEL_TRACEPOINT:
		event_name = event_desc->name;
		break;
	case LTTNG_KERNEL_KPROBE:
	case LTTNG_KERNEL_UPROBE:
	case LTTNG_KERNEL_KRETPROBE:
	case LTTNG_KERNEL_FUNCTION:
	case LTTNG_KERNEL_NOOP:
	case LTTNG_KERNEL_SYSCALL:
		event_name = event_param->name;
		break;
	default:
		WARN_ON_ONCE(1);
		ret = -EINVAL;
		goto type_error;
	}
	name_len = strlen(event_name);
	hash = jhash(event_name, name_len, 0);
	head = &session->events_ht.table[hash & (LTTNG_EVENT_HT_SIZE - 1)];
	lttng_hlist_for_each_entry(event, head, hlist) {
		WARN_ON_ONCE(!event->desc);
		if (!strncmp(event->desc->name, event_name,
					LTTNG_KERNEL_SYM_NAME_LEN - 1)
				&& chan == event->chan) {
			ret = -EEXIST;
			goto exist;
		}
	}

	event = kmem_cache_zalloc(event_cache, GFP_KERNEL);
	if (!event) {
		ret = -ENOMEM;
		goto cache_error;
	}
	event->chan = chan;
	event->filter = filter;
	event->id = chan->free_event_id++;
	event->instrumentation = itype;
	event->evtype = LTTNG_TYPE_EVENT;
	INIT_LIST_HEAD(&event->bytecode_runtime_head);
	INIT_LIST_HEAD(&event->enablers_ref_head);

	switch (itype) {
	case LTTNG_KERNEL_TRACEPOINT:
		/* Event will be enabled by enabler sync. */
		event->enabled = 0;
		event->registered = 0;
		event->desc = lttng_event_get(event_name);
		if (!event->desc) {
			ret = -ENOENT;
			goto register_error;
		}
		/* Populate lttng_event structure before event registration. */
		smp_wmb();
		break;
	case LTTNG_KERNEL_KPROBE:
		/*
		 * Needs to be explicitly enabled after creation, since
		 * we may want to apply filters.
		 */
		event->enabled = 0;
		event->registered = 1;
		/*
		 * Populate lttng_event structure before event
		 * registration.
		 */
		smp_wmb();
		ret = lttng_kprobes_register(event_name,
				event_param->u.kprobe.symbol_name,
				event_param->u.kprobe.offset,
				event_param->u.kprobe.addr,
				event);
		if (ret) {
			ret = -EINVAL;
			goto register_error;
		}
		ret = try_module_get(event->desc->owner);
		WARN_ON_ONCE(!ret);
		break;
	case LTTNG_KERNEL_KRETPROBE:
	{
		struct lttng_event *event_return;

		/* kretprobe defines 2 events */
		/*
		 * Needs to be explicitly enabled after creation, since
		 * we may want to apply filters.
		 */
		event->enabled = 0;
		event->registered = 1;
		event_return =
			kmem_cache_zalloc(event_cache, GFP_KERNEL);
		if (!event_return) {
			ret = -ENOMEM;
			goto register_error;
		}
		event_return->chan = chan;
		event_return->filter = filter;
		event_return->id = chan->free_event_id++;
		event_return->enabled = 0;
		event_return->registered = 1;
		event_return->instrumentation = itype;
		/*
		 * Populate lttng_event structure before kretprobe registration.
		 */
		smp_wmb();
		ret = lttng_kretprobes_register(event_name,
				event_param->u.kretprobe.symbol_name,
				event_param->u.kretprobe.offset,
				event_param->u.kretprobe.addr,
				event, event_return);
		if (ret) {
			kmem_cache_free(event_cache, event_return);
			ret = -EINVAL;
			goto register_error;
		}
		/* Take 2 refs on the module: one per event. */
		ret = try_module_get(event->desc->owner);
		WARN_ON_ONCE(!ret);
		ret = try_module_get(event->desc->owner);
		WARN_ON_ONCE(!ret);
		ret = _lttng_event_metadata_statedump(chan->session, chan,
						    event_return);
		WARN_ON_ONCE(ret > 0);
		if (ret) {
			kmem_cache_free(event_cache, event_return);
			module_put(event->desc->owner);
			module_put(event->desc->owner);
			goto statedump_error;
		}
		list_add(&event_return->list, &chan->session->events);
		break;
	}
	case LTTNG_KERNEL_FUNCTION:
		/*
		 * Needs to be explicitly enabled after creation, since
		 * we may want to apply filters.
		 */
		event->enabled = 0;
		event->registered = 1;
		/*
		 * Populate lttng_event structure before event
		 * registration.
		 */
		smp_wmb();
		ret = lttng_ftrace_register(event_name,
				event_param->u.ftrace.symbol_name,
				event);
		if (ret) {
			goto register_error;
		}
		ret = try_module_get(event->desc->owner);
		WARN_ON_ONCE(!ret);
		break;
	case LTTNG_KERNEL_NOOP:
	case LTTNG_KERNEL_SYSCALL:
		/*
		 * Needs to be explicitly enabled after creation, since
		 * we may want to apply filters.
		 */
		event->enabled = 0;
		event->registered = 0;
		event->desc = event_desc;
		if (!event->desc) {
			ret = -EINVAL;
			goto register_error;
		}
		break;
	case LTTNG_KERNEL_UPROBE:
		/*
		 * Needs to be explicitly enabled after creation, since
		 * we may want to apply filters.
		 */
		event->enabled = 0;
		event->registered = 1;

		/*
		 * Populate lttng_event structure before event
		 * registration.
		 */
		smp_wmb();

		ret = lttng_uprobes_register(event_param->name,
				event_param->u.uprobe.fd,
				event);
		if (ret)
			goto register_error;
		ret = try_module_get(event->desc->owner);
		WARN_ON_ONCE(!ret);
		break;
	default:
		WARN_ON_ONCE(1);
		ret = -EINVAL;
		goto register_error;
	}
	ret = _lttng_event_metadata_statedump(chan->session, chan, event);
	WARN_ON_ONCE(ret > 0);
	if (ret) {
		goto statedump_error;
	}
	hlist_add_head(&event->hlist, head);
	list_add(&event->list, &chan->session->events);
	return event;

statedump_error:
	/* If a statedump error occurs, events will not be readable. */
register_error:
	kmem_cache_free(event_cache, event);
cache_error:
exist:
type_error:
full:
	return ERR_PTR(ret);
}

struct lttng_event *lttng_event_create(struct lttng_channel *chan,
				struct lttng_kernel_event *event_param,
				void *filter,
				const struct lttng_event_desc *event_desc,
				enum lttng_kernel_instrumentation itype)
{
	struct lttng_event *event;

	mutex_lock(&sessions_mutex);
	event = _lttng_event_create(chan, event_param, filter, event_desc,
				itype);
	mutex_unlock(&sessions_mutex);
	return event;
}

/* Only used for tracepoints for now. */
static
void register_event(struct lttng_event *event)
{
	const struct lttng_event_desc *desc;
	int ret = -EINVAL;

	if (event->registered)
		return;

	desc = event->desc;
	switch (event->instrumentation) {
	case LTTNG_KERNEL_TRACEPOINT:
		ret = lttng_wrapper_tracepoint_probe_register(desc->kname,
						  desc->probe_callback,
						  event);
		break;
	case LTTNG_KERNEL_SYSCALL:
		ret = lttng_syscall_filter_enable(event->chan,
			desc->name);
		break;
	case LTTNG_KERNEL_KPROBE:
	case LTTNG_KERNEL_UPROBE:
	case LTTNG_KERNEL_KRETPROBE:
	case LTTNG_KERNEL_FUNCTION:
	case LTTNG_KERNEL_NOOP:
		ret = 0;
		break;
	default:
		WARN_ON_ONCE(1);
	}
	if (!ret)
		event->registered = 1;
}

/*
 * Only used internally at session destruction.
 */
int _lttng_event_unregister(struct lttng_event *event)
{
	const struct lttng_event_desc *desc;
	int ret = -EINVAL;

	if (!event->registered)
		return 0;

	desc = event->desc;
	switch (event->instrumentation) {
	case LTTNG_KERNEL_TRACEPOINT:
		ret = lttng_wrapper_tracepoint_probe_unregister(event->desc->kname,
						  event->desc->probe_callback,
						  event);
		break;
	case LTTNG_KERNEL_KPROBE:
		lttng_kprobes_unregister(event);
		ret = 0;
		break;
	case LTTNG_KERNEL_KRETPROBE:
		lttng_kretprobes_unregister(event);
		ret = 0;
		break;
	case LTTNG_KERNEL_FUNCTION:
		lttng_ftrace_unregister(event);
		ret = 0;
		break;
	case LTTNG_KERNEL_SYSCALL:
		ret = lttng_syscall_filter_disable(event->chan,
			desc->name);
		break;
	case LTTNG_KERNEL_NOOP:
		ret = 0;
		break;
	case LTTNG_KERNEL_UPROBE:
		lttng_uprobes_unregister(event);
		ret = 0;
		break;
	default:
		WARN_ON_ONCE(1);
	}
	if (!ret)
		event->registered = 0;
	return ret;
}

/*
 * Only used internally at session destruction.
 */
static
void _lttng_event_destroy(struct lttng_event *event)
{
	switch (event->instrumentation) {
	case LTTNG_KERNEL_TRACEPOINT:
		lttng_event_put(event->desc);
		break;
	case LTTNG_KERNEL_KPROBE:
		module_put(event->desc->owner);
		lttng_kprobes_destroy_private(event);
		break;
	case LTTNG_KERNEL_KRETPROBE:
		module_put(event->desc->owner);
		lttng_kretprobes_destroy_private(event);
		break;
	case LTTNG_KERNEL_FUNCTION:
		module_put(event->desc->owner);
		lttng_ftrace_destroy_private(event);
		break;
	case LTTNG_KERNEL_NOOP:
	case LTTNG_KERNEL_SYSCALL:
		break;
	case LTTNG_KERNEL_UPROBE:
		module_put(event->desc->owner);
		lttng_uprobes_destroy_private(event);
		break;
	default:
		WARN_ON_ONCE(1);
	}
	list_del(&event->list);
	lttng_destroy_context(event->ctx);
	kmem_cache_free(event_cache, event);
}

int lttng_session_track_pid(struct lttng_session *session, int pid)
{
	int ret;

	if (pid < -1)
		return -EINVAL;
	mutex_lock(&sessions_mutex);
	if (pid == -1) {
		/* track all pids: destroy tracker. */
		if (session->pid_tracker) {
			struct lttng_pid_tracker *lpf;

			lpf = session->pid_tracker;
			rcu_assign_pointer(session->pid_tracker, NULL);
			synchronize_trace();
			lttng_pid_tracker_destroy(lpf);
		}
		ret = 0;
	} else {
		if (!session->pid_tracker) {
			struct lttng_pid_tracker *lpf;

			lpf = lttng_pid_tracker_create();
			if (!lpf) {
				ret = -ENOMEM;
				goto unlock;
			}
			ret = lttng_pid_tracker_add(lpf, pid);
			rcu_assign_pointer(session->pid_tracker, lpf);
		} else {
			ret = lttng_pid_tracker_add(session->pid_tracker, pid);
		}
	}
unlock:
	mutex_unlock(&sessions_mutex);
	return ret;
}

int lttng_session_untrack_pid(struct lttng_session *session, int pid)
{
	int ret;

	if (pid < -1)
		return -EINVAL;
	mutex_lock(&sessions_mutex);
	if (pid == -1) {
		/* untrack all pids: replace by empty tracker. */
		struct lttng_pid_tracker *old_lpf = session->pid_tracker;
		struct lttng_pid_tracker *lpf;

		lpf = lttng_pid_tracker_create();
		if (!lpf) {
			ret = -ENOMEM;
			goto unlock;
		}
		rcu_assign_pointer(session->pid_tracker, lpf);
		synchronize_trace();
		if (old_lpf)
			lttng_pid_tracker_destroy(old_lpf);
		ret = 0;
	} else {
		if (!session->pid_tracker) {
			ret = -ENOENT;
			goto unlock;
		}
		ret = lttng_pid_tracker_del(session->pid_tracker, pid);
	}
unlock:
	mutex_unlock(&sessions_mutex);
	return ret;
}

static
void *pid_list_start(struct seq_file *m, loff_t *pos)
{
	struct lttng_session *session = m->private;
	struct lttng_pid_tracker *lpf;
	struct lttng_pid_hash_node *e;
	int iter = 0, i;

	mutex_lock(&sessions_mutex);
	lpf = session->pid_tracker;
	if (lpf) {
		for (i = 0; i < LTTNG_PID_TABLE_SIZE; i++) {
			struct hlist_head *head = &lpf->pid_hash[i];

			lttng_hlist_for_each_entry(e, head, hlist) {
				if (iter++ >= *pos)
					return e;
			}
		}
	} else {
		/* PID tracker disabled. */
		if (iter >= *pos && iter == 0) {
			return session;	/* empty tracker */
		}
		iter++;
	}
	/* End of list */
	return NULL;
}

/* Called with sessions_mutex held. */
static
void *pid_list_next(struct seq_file *m, void *p, loff_t *ppos)
{
	struct lttng_session *session = m->private;
	struct lttng_pid_tracker *lpf;
	struct lttng_pid_hash_node *e;
	int iter = 0, i;

	(*ppos)++;
	lpf = session->pid_tracker;
	if (lpf) {
		for (i = 0; i < LTTNG_PID_TABLE_SIZE; i++) {
			struct hlist_head *head = &lpf->pid_hash[i];

			lttng_hlist_for_each_entry(e, head, hlist) {
				if (iter++ >= *ppos)
					return e;
			}
		}
	} else {
		/* PID tracker disabled. */
		if (iter >= *ppos && iter == 0)
			return session;	/* empty tracker */
		iter++;
	}

	/* End of list */
	return NULL;
}

static
void pid_list_stop(struct seq_file *m, void *p)
{
	mutex_unlock(&sessions_mutex);
}

static
int pid_list_show(struct seq_file *m, void *p)
{
	int pid;

	if (p == m->private) {
		/* Tracker disabled. */
		pid = -1;
	} else {
		const struct lttng_pid_hash_node *e = p;

		pid = lttng_pid_tracker_get_node_pid(e);
	}
	seq_printf(m,	"process { pid = %d; };\n", pid);
	return 0;
}

static
const struct seq_operations lttng_tracker_pids_list_seq_ops = {
	.start = pid_list_start,
	.next = pid_list_next,
	.stop = pid_list_stop,
	.show = pid_list_show,
};

static
int lttng_tracker_pids_list_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &lttng_tracker_pids_list_seq_ops);
}

static
int lttng_tracker_pids_list_release(struct inode *inode, struct file *file)
{
	struct seq_file *m = file->private_data;
	struct lttng_session *session = m->private;
	int ret;

	WARN_ON_ONCE(!session);
	ret = seq_release(inode, file);
	if (!ret && session)
		fput(session->file);
	return ret;
}

const struct file_operations lttng_tracker_pids_list_fops = {
	.owner = THIS_MODULE,
	.open = lttng_tracker_pids_list_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = lttng_tracker_pids_list_release,
};

int lttng_session_list_tracker_pids(struct lttng_session *session)
{
	struct file *tracker_pids_list_file;
	struct seq_file *m;
	int file_fd, ret;

	file_fd = lttng_get_unused_fd();
	if (file_fd < 0) {
		ret = file_fd;
		goto fd_error;
	}

	tracker_pids_list_file = anon_inode_getfile("[lttng_tracker_pids_list]",
					  &lttng_tracker_pids_list_fops,
					  NULL, O_RDWR);
	if (IS_ERR(tracker_pids_list_file)) {
		ret = PTR_ERR(tracker_pids_list_file);
		goto file_error;
	}
	if (atomic_long_add_unless(&session->file->f_count,
		1, INT_MAX) == INT_MAX) {
		goto refcount_error;
	}
	ret = lttng_tracker_pids_list_fops.open(NULL, tracker_pids_list_file);
	if (ret < 0)
		goto open_error;
	m = tracker_pids_list_file->private_data;
	m->private = session;
	fd_install(file_fd, tracker_pids_list_file);

	return file_fd;

open_error:
	atomic_long_dec(&session->file->f_count);
refcount_error:
	fput(tracker_pids_list_file);
file_error:
	put_unused_fd(file_fd);
fd_error:
	return ret;
}

/*
 * Enabler management.
 */
static
int lttng_match_enabler_star_glob(const char *desc_name,
		const char *pattern)
{
	if (!strutils_star_glob_match(pattern, LTTNG_SIZE_MAX,
			desc_name, LTTNG_SIZE_MAX))
		return 0;
	return 1;
}

static
int lttng_match_enabler_name(const char *desc_name,
		const char *name)
{
	if (strcmp(desc_name, name))
		return 0;
	return 1;
}

static
int lttng_desc_match_enabler(const struct lttng_event_desc *desc,
		struct lttng_enabler *enabler)
{
	const char *desc_name, *enabler_name;

	enabler_name = enabler->event_param.name;
	switch (enabler->event_param.instrumentation) {
	case LTTNG_KERNEL_TRACEPOINT:
		desc_name = desc->name;
		break;
	case LTTNG_KERNEL_SYSCALL:
		desc_name = desc->name;
		if (!strncmp(desc_name, "compat_", strlen("compat_")))
			desc_name += strlen("compat_");
		if (!strncmp(desc_name, "syscall_exit_",
				strlen("syscall_exit_"))) {
			desc_name += strlen("syscall_exit_");
		} else if (!strncmp(desc_name, "syscall_entry_",
				strlen("syscall_entry_"))) {
			desc_name += strlen("syscall_entry_");
		} else {
			WARN_ON_ONCE(1);
			return -EINVAL;
		}
		break;
	default:
		WARN_ON_ONCE(1);
		return -EINVAL;
	}
	switch (enabler->type) {
	case LTTNG_ENABLER_STAR_GLOB:
		return lttng_match_enabler_star_glob(desc_name, enabler_name);
	case LTTNG_ENABLER_NAME:
		return lttng_match_enabler_name(desc_name, enabler_name);
	default:
		return -EINVAL;
	}
}

static
int lttng_event_match_enabler(struct lttng_event *event,
		struct lttng_enabler *enabler)
{
	if (enabler->event_param.instrumentation != event->instrumentation)
		return 0;
	if (lttng_desc_match_enabler(event->desc, enabler)
			&& event->chan == enabler->chan)
		return 1;
	else
		return 0;
}

static
struct lttng_enabler_ref *lttng_event_enabler_ref(struct lttng_event *event,
		struct lttng_enabler *enabler)
{
	struct lttng_enabler_ref *enabler_ref;

	list_for_each_entry(enabler_ref,
			&event->enablers_ref_head, node) {
		if (enabler_ref->ref == enabler)
			return enabler_ref;
	}
	return NULL;
}

static
void lttng_create_tracepoint_if_missing(struct lttng_enabler *enabler)
{
	struct lttng_session *session = enabler->chan->session;
	struct lttng_probe_desc *probe_desc;
	const struct lttng_event_desc *desc;
	int i;
	struct list_head *probe_list;

	probe_list = lttng_get_probe_list_head();
	/*
	 * For each probe event, if we find that a probe event matches
	 * our enabler, create an associated lttng_event if not
	 * already present.
	 */
	list_for_each_entry(probe_desc, probe_list, head) {
		for (i = 0; i < probe_desc->nr_events; i++) {
			int found = 0;
			struct hlist_head *head;
			const char *event_name;
			size_t name_len;
			uint32_t hash;
			struct lttng_event *event;

			desc = probe_desc->event_desc[i];
			if (!lttng_desc_match_enabler(desc, enabler))
				continue;
			event_name = desc->name;
			name_len = strlen(event_name);

			/*
			 * Check if already created.
			 */
			hash = jhash(event_name, name_len, 0);
			head = &session->events_ht.table[hash & (LTTNG_EVENT_HT_SIZE - 1)];
			lttng_hlist_for_each_entry(event, head, hlist) {
				if (event->desc == desc
						&& event->chan == enabler->chan)
					found = 1;
			}
			if (found)
				continue;

			/*
			 * We need to create an event for this
			 * event probe.
			 */
			event = _lttng_event_create(enabler->chan,
					NULL, NULL, desc,
					LTTNG_KERNEL_TRACEPOINT);
			if (!event) {
				printk(KERN_INFO "Unable to create event %s\n",
					probe_desc->event_desc[i]->name);
			}
		}
	}
}

static
void lttng_create_syscall_if_missing(struct lttng_enabler *enabler)
{
	int ret;

	ret = lttng_syscalls_register(enabler->chan, NULL);
	WARN_ON_ONCE(ret);
}

/*
 * Create struct lttng_event if it is missing and present in the list of
 * tracepoint probes.
 * Should be called with sessions mutex held.
 */
static
void lttng_create_event_if_missing(struct lttng_enabler *enabler)
{
	switch (enabler->event_param.instrumentation) {
	case LTTNG_KERNEL_TRACEPOINT:
		lttng_create_tracepoint_if_missing(enabler);
		break;
	case LTTNG_KERNEL_SYSCALL:
		lttng_create_syscall_if_missing(enabler);
		break;
	default:
		WARN_ON_ONCE(1);
		break;
	}
}

/*
 * Create events associated with an enabler (if not already present),
 * and add backward reference from the event to the enabler.
 * Should be called with sessions mutex held.
 */
static
int lttng_enabler_ref_events(struct lttng_enabler *enabler)
{
	struct lttng_session *session = enabler->chan->session;
	struct lttng_event *event;

	/* First ensure that probe events are created for this enabler. */
	lttng_create_event_if_missing(enabler);

	/* For each event matching enabler in session event list. */
	list_for_each_entry(event, &session->events, list) {
		struct lttng_enabler_ref *enabler_ref;

		if (!lttng_event_match_enabler(event, enabler))
			continue;
		enabler_ref = lttng_event_enabler_ref(event, enabler);
		if (!enabler_ref) {
			/*
			 * If no backward ref, create it.
			 * Add backward ref from event to enabler.
			 */
			enabler_ref = kzalloc(sizeof(*enabler_ref), GFP_KERNEL);
			if (!enabler_ref)
				return -ENOMEM;
			enabler_ref->ref = enabler;
			list_add(&enabler_ref->node,
				&event->enablers_ref_head);
		}

		/*
		 * Link filter bytecodes if not linked yet.
		 */
		lttng_enabler_event_link_bytecode(event, enabler);

		/* TODO: merge event context. */
	}
	return 0;
}

/*
 * Called at module load: connect the probe on all enablers matching
 * this event.
 * Called with sessions lock held.
 */
int lttng_fix_pending_events(void)
{
	struct lttng_session *session;

	list_for_each_entry(session, &sessions, list)
		lttng_session_lazy_sync_enablers(session);
	return 0;
}

struct lttng_enabler *lttng_enabler_create(enum lttng_enabler_type type,
		struct lttng_kernel_event *event_param,
		struct lttng_channel *chan)
{
	struct lttng_enabler *enabler;

	enabler = kzalloc(sizeof(*enabler), GFP_KERNEL);
	if (!enabler)
		return NULL;
	enabler->type = type;
	INIT_LIST_HEAD(&enabler->filter_bytecode_head);
	memcpy(&enabler->event_param, event_param,
		sizeof(enabler->event_param));
	enabler->chan = chan;
	/* ctx left NULL */
	enabler->enabled = 0;
	enabler->evtype = LTTNG_TYPE_ENABLER;
	mutex_lock(&sessions_mutex);
	list_add(&enabler->node, &enabler->chan->session->enablers_head);
	lttng_session_lazy_sync_enablers(enabler->chan->session);
	mutex_unlock(&sessions_mutex);
	return enabler;
}

int lttng_enabler_enable(struct lttng_enabler *enabler)
{
	mutex_lock(&sessions_mutex);
	enabler->enabled = 1;
	lttng_session_lazy_sync_enablers(enabler->chan->session);
	mutex_unlock(&sessions_mutex);
	return 0;
}

int lttng_enabler_disable(struct lttng_enabler *enabler)
{
	mutex_lock(&sessions_mutex);
	enabler->enabled = 0;
	lttng_session_lazy_sync_enablers(enabler->chan->session);
	mutex_unlock(&sessions_mutex);
	return 0;
}

int lttng_enabler_attach_bytecode(struct lttng_enabler *enabler,
		struct lttng_kernel_filter_bytecode __user *bytecode)
{
	struct lttng_filter_bytecode_node *bytecode_node;
	uint32_t bytecode_len;
	int ret;

	ret = get_user(bytecode_len, &bytecode->len);
	if (ret)
		return ret;
	bytecode_node = kzalloc(sizeof(*bytecode_node) + bytecode_len,
			GFP_KERNEL);
	if (!bytecode_node)
		return -ENOMEM;
	ret = copy_from_user(&bytecode_node->bc, bytecode,
		sizeof(*bytecode) + bytecode_len);
	if (ret)
		goto error_free;
	bytecode_node->enabler = enabler;
	/* Enforce length based on allocated size */
	bytecode_node->bc.len = bytecode_len;
	list_add_tail(&bytecode_node->node, &enabler->filter_bytecode_head);
	lttng_session_lazy_sync_enablers(enabler->chan->session);
	return 0;

error_free:
	kfree(bytecode_node);
	return ret;
}

int lttng_event_add_callsite(struct lttng_event *event,
		struct lttng_kernel_event_callsite __user *callsite)
{

	switch (event->instrumentation) {
	case LTTNG_KERNEL_UPROBE:
		return lttng_uprobes_add_callsite(event, callsite);
	default:
		return -EINVAL;
	}
}

int lttng_enabler_attach_context(struct lttng_enabler *enabler,
		struct lttng_kernel_context *context_param)
{
	return -ENOSYS;
}

static
void lttng_enabler_destroy(struct lttng_enabler *enabler)
{
	struct lttng_filter_bytecode_node *filter_node, *tmp_filter_node;

	/* Destroy filter bytecode */
	list_for_each_entry_safe(filter_node, tmp_filter_node,
			&enabler->filter_bytecode_head, node) {
		kfree(filter_node);
	}

	/* Destroy contexts */
	lttng_destroy_context(enabler->ctx);

	list_del(&enabler->node);
	kfree(enabler);
}

/*
 * lttng_session_sync_enablers should be called just before starting a
 * session.
 * Should be called with sessions mutex held.
 */
static
void lttng_session_sync_enablers(struct lttng_session *session)
{
	struct lttng_enabler *enabler;
	struct lttng_event *event;

	list_for_each_entry(enabler, &session->enablers_head, node)
		lttng_enabler_ref_events(enabler);
	/*
	 * For each event, if at least one of its enablers is enabled,
	 * and its channel and session transient states are enabled, we
	 * enable the event, else we disable it.
	 */
	list_for_each_entry(event, &session->events, list) {
		struct lttng_enabler_ref *enabler_ref;
		struct lttng_bytecode_runtime *runtime;
		int enabled = 0, has_enablers_without_bytecode = 0;

		switch (event->instrumentation) {
		case LTTNG_KERNEL_TRACEPOINT:
		case LTTNG_KERNEL_SYSCALL:
			/* Enable events */
			list_for_each_entry(enabler_ref,
					&event->enablers_ref_head, node) {
				if (enabler_ref->ref->enabled) {
					enabled = 1;
					break;
				}
			}
			break;
		default:
			/* Not handled with lazy sync. */
			continue;
		}
		/*
		 * Enabled state is based on union of enablers, with
		 * intesection of session and channel transient enable
		 * states.
		 */
		enabled = enabled && session->tstate && event->chan->tstate;

		WRITE_ONCE(event->enabled, enabled);
		/*
		 * Sync tracepoint registration with event enabled
		 * state.
		 */
		if (enabled) {
			register_event(event);
		} else {
			_lttng_event_unregister(event);
		}

		/* Check if has enablers without bytecode enabled */
		list_for_each_entry(enabler_ref,
				&event->enablers_ref_head, node) {
			if (enabler_ref->ref->enabled
					&& list_empty(&enabler_ref->ref->filter_bytecode_head)) {
				has_enablers_without_bytecode = 1;
				break;
			}
		}
		event->has_enablers_without_bytecode =
			has_enablers_without_bytecode;

		/* Enable filters */
		list_for_each_entry(runtime,
				&event->bytecode_runtime_head, node)
			lttng_filter_sync_state(runtime);
	}
}

/*
 * Apply enablers to session events, adding events to session if need
 * be. It is required after each modification applied to an active
 * session, and right before session "start".
 * "lazy" sync means we only sync if required.
 * Should be called with sessions mutex held.
 */
static
void lttng_session_lazy_sync_enablers(struct lttng_session *session)
{
	/* We can skip if session is not active */
	if (!session->active)
		return;
	lttng_session_sync_enablers(session);
}

/*
 * Serialize at most one packet worth of metadata into a metadata
 * channel.
 * We grab the metadata cache mutex to get exclusive access to our metadata
 * buffer and to the metadata cache. Exclusive access to the metadata buffer
 * allows us to do racy operations such as looking for remaining space left in
 * packet and write, since mutual exclusion protects us from concurrent writes.
 * Mutual exclusion on the metadata cache allow us to read the cache content
 * without racing against reallocation of the cache by updates.
 * Returns the number of bytes written in the channel, 0 if no data
 * was written and a negative value on error.
 */
int lttng_metadata_output_channel(struct lttng_metadata_stream *stream,
		struct channel *chan)
{
	struct lib_ring_buffer_ctx ctx;
	int ret = 0;
	size_t len, reserve_len;

	/*
	 * Ensure we support mutiple get_next / put sequences followed by
	 * put_next. The metadata cache lock protects reading the metadata
	 * cache. It can indeed be read concurrently by "get_next_subbuf" and
	 * "flush" operations on the buffer invoked by different processes.
	 * Moreover, since the metadata cache memory can be reallocated, we
	 * need to have exclusive access against updates even though we only
	 * read it.
	 */
	mutex_lock(&stream->metadata_cache->lock);
	WARN_ON(stream->metadata_in < stream->metadata_out);
	if (stream->metadata_in != stream->metadata_out)
		goto end;

	/* Metadata regenerated, change the version. */
	if (stream->metadata_cache->version != stream->version)
		stream->version = stream->metadata_cache->version;

	len = stream->metadata_cache->metadata_written -
		stream->metadata_in;
	if (!len)
		goto end;
	reserve_len = min_t(size_t,
			stream->transport->ops.packet_avail_size(chan),
			len);
	lib_ring_buffer_ctx_init(&ctx, chan, NULL, reserve_len,
			sizeof(char), -1);
	/*
	 * If reservation failed, return an error to the caller.
	 */
	ret = stream->transport->ops.event_reserve(&ctx, 0);
	if (ret != 0) {
		printk(KERN_WARNING "LTTng: Metadata event reservation failed\n");
		goto end;
	}
	stream->transport->ops.event_write(&ctx,
			stream->metadata_cache->data + stream->metadata_in,
			reserve_len);
	stream->transport->ops.event_commit(&ctx);
	stream->metadata_in += reserve_len;
	ret = reserve_len;

end:
	mutex_unlock(&stream->metadata_cache->lock);
	return ret;
}

/*
 * Write the metadata to the metadata cache.
 * Must be called with sessions_mutex held.
 * The metadata cache lock protects us from concurrent read access from
 * thread outputting metadata content to ring buffer.
 */
int lttng_metadata_printf(struct lttng_session *session,
			  const char *fmt, ...)
{
	char *str;
	size_t len;
	va_list ap;
	struct lttng_metadata_stream *stream;

	WARN_ON_ONCE(!READ_ONCE(session->active));

	va_start(ap, fmt);
	str = kvasprintf(GFP_KERNEL, fmt, ap);
	va_end(ap);
	if (!str)
		return -ENOMEM;

	len = strlen(str);
	mutex_lock(&session->metadata_cache->lock);
	if (session->metadata_cache->metadata_written + len >
			session->metadata_cache->cache_alloc) {
		char *tmp_cache_realloc;
		unsigned int tmp_cache_alloc_size;

		tmp_cache_alloc_size = max_t(unsigned int,
				session->metadata_cache->cache_alloc + len,
				session->metadata_cache->cache_alloc << 1);
		tmp_cache_realloc = vzalloc(tmp_cache_alloc_size);
		if (!tmp_cache_realloc)
			goto err;
		if (session->metadata_cache->data) {
			memcpy(tmp_cache_realloc,
				session->metadata_cache->data,
				session->metadata_cache->cache_alloc);
			vfree(session->metadata_cache->data);
		}

		session->metadata_cache->cache_alloc = tmp_cache_alloc_size;
		session->metadata_cache->data = tmp_cache_realloc;
	}
	memcpy(session->metadata_cache->data +
			session->metadata_cache->metadata_written,
			str, len);
	session->metadata_cache->metadata_written += len;
	mutex_unlock(&session->metadata_cache->lock);
	kfree(str);

	list_for_each_entry(stream, &session->metadata_cache->metadata_stream, list)
		wake_up_interruptible(&stream->read_wait);

	return 0;

err:
	mutex_unlock(&session->metadata_cache->lock);
	kfree(str);
	return -ENOMEM;
}

static
int print_tabs(struct lttng_session *session, size_t nesting)
{
	size_t i;

	for (i = 0; i < nesting; i++) {
		int ret;

		ret = lttng_metadata_printf(session, "	");
		if (ret) {
			return ret;
		}
	}
	return 0;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_struct_type_statedump(struct lttng_session *session,
		const struct lttng_type *type,
		size_t nesting)
{
	int ret;
	uint32_t i, nr_fields;

	ret = print_tabs(session, nesting);
	if (ret)
		return ret;
	ret = lttng_metadata_printf(session,
		"struct {\n");
	if (ret)
		return ret;
	nr_fields = type->u._struct.nr_fields;
	for (i = 0; i < nr_fields; i++) {
		const struct lttng_event_field *iter_field;

		iter_field = &type->u._struct.fields[i];
		ret = _lttng_field_statedump(session, iter_field, nesting + 1);
		if (ret)
			return ret;
	}
	ret = print_tabs(session, nesting);
	if (ret)
		return ret;
	ret = lttng_metadata_printf(session,
		"}");
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_struct_statedump(struct lttng_session *session,
		const struct lttng_event_field *field,
		size_t nesting)
{
	int ret;

	ret = _lttng_struct_type_statedump(session,
			&field->type, nesting);
	if (ret)
		return ret;
	ret = lttng_metadata_printf(session,
		"_%s;\n",
		field->name);
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_variant_type_statedump(struct lttng_session *session,
		const struct lttng_type *type,
		size_t nesting)
{
	int ret;
	uint32_t i, nr_choices;

	ret = print_tabs(session, nesting);
	if (ret)
		return ret;
	ret = lttng_metadata_printf(session,
		"variant <_%s> {\n",
		type->u.variant.tag_name);
	if (ret)
		return ret;
	nr_choices = type->u.variant.nr_choices;
	for (i = 0; i < nr_choices; i++) {
		const struct lttng_event_field *iter_field;

		iter_field = &type->u.variant.choices[i];
		ret = _lttng_field_statedump(session, iter_field, nesting + 1);
		if (ret)
			return ret;
	}
	ret = print_tabs(session, nesting);
	if (ret)
		return ret;
	ret = lttng_metadata_printf(session,
		"}");
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_variant_statedump(struct lttng_session *session,
		const struct lttng_event_field *field,
		size_t nesting)
{
	int ret;

	ret = _lttng_variant_type_statedump(session,
			&field->type, nesting);
	if (ret)
		return ret;
	ret = lttng_metadata_printf(session,
		"_%s;\n",
		field->name);
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_array_compound_statedump(struct lttng_session *session,
		const struct lttng_event_field *field,
		size_t nesting)
{
	int ret;
	const struct lttng_type *elem_type;

	/* Only array of structures and variants are currently supported. */
	elem_type = field->type.u.array_compound.elem_type;
	switch (elem_type->atype) {
	case atype_struct:
		ret = _lttng_struct_type_statedump(session, elem_type, nesting);
		if (ret)
			return ret;
		break;
	case atype_variant:
		ret = _lttng_variant_type_statedump(session, elem_type, nesting);
		if (ret)
			return ret;
		break;
	default:
		return -EINVAL;
	}
	ret = lttng_metadata_printf(session,
		" _%s[%u];\n",
		field->name,
		field->type.u.array_compound.length);
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_sequence_compound_statedump(struct lttng_session *session,
		const struct lttng_event_field *field,
		size_t nesting)
{
	int ret;
	const char *length_name;
	const struct lttng_type *elem_type;

	length_name = field->type.u.sequence_compound.length_name;

	/* Only array of structures and variants are currently supported. */
	elem_type = field->type.u.sequence_compound.elem_type;
	switch (elem_type->atype) {
	case atype_struct:
		ret = _lttng_struct_type_statedump(session, elem_type, nesting);
		if (ret)
			return ret;
		break;
	case atype_variant:
		ret = _lttng_variant_type_statedump(session, elem_type, nesting);
		if (ret)
			return ret;
		break;
	default:
		return -EINVAL;
	}
	ret = lttng_metadata_printf(session,
		" _%s[ _%s ];\n",
		field->name,
		length_name);
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_enum_statedump(struct lttng_session *session,
		const struct lttng_event_field *field,
		size_t nesting)
{
	const struct lttng_enum_desc *enum_desc;
	const struct lttng_integer_type *container_type;
	int ret;
	unsigned int i, nr_entries;

	enum_desc = field->type.u.basic.enumeration.desc;
	container_type = &field->type.u.basic.enumeration.container_type;
	nr_entries = enum_desc->nr_entries;

	ret = print_tabs(session, nesting);
	if (ret)
		goto end;
	ret = lttng_metadata_printf(session,
		"enum : integer { size = %u; align = %u; signed = %u; encoding = %s; base = %u;%s } {\n",
		container_type->size,
		container_type->alignment,
		container_type->signedness,
		(container_type->encoding == lttng_encode_none)
			? "none"
			: (container_type->encoding == lttng_encode_UTF8)
				? "UTF8"
				: "ASCII",
		container_type->base,
#if __BYTE_ORDER == __BIG_ENDIAN
		container_type->reverse_byte_order ? " byte_order = le;" : ""
#else
		container_type->reverse_byte_order ? " byte_order = be;" : ""
#endif
		);
	if (ret)
	        goto end;
	/* Dump all entries */
	for (i = 0; i < nr_entries; i++) {
		const struct lttng_enum_entry *entry = &enum_desc->entries[i];
		int j, len;

		ret = print_tabs(session, nesting + 1);
		if (ret)
			goto end;
		ret = lttng_metadata_printf(session,
				"\"");
		if (ret)
			goto end;
		len = strlen(entry->string);
		/* Escape the character '"' */
		for (j = 0; j < len; j++) {
			char c = entry->string[j];

			switch (c) {
			case '"':
				ret = lttng_metadata_printf(session,
						"\\\"");
				break;
			case '\\':
				ret = lttng_metadata_printf(session,
						"\\\\");
				break;
			default:
				ret = lttng_metadata_printf(session,
						"%c", c);
				break;
			}
			if (ret)
				goto end;
		}
		ret = lttng_metadata_printf(session, "\"");
		if (ret)
			goto end;

		if (entry->options.is_auto) {
			ret = lttng_metadata_printf(session, ",\n");
			if (ret)
				goto end;
		} else {
			ret = lttng_metadata_printf(session,
					" = ");
			if (ret)
				goto end;
			if (entry->start.signedness)
				ret = lttng_metadata_printf(session,
					"%lld", (long long) entry->start.value);
			else
				ret = lttng_metadata_printf(session,
					"%llu", entry->start.value);
			if (ret)
				goto end;
			if (entry->start.signedness == entry->end.signedness &&
					entry->start.value
						== entry->end.value) {
				ret = lttng_metadata_printf(session,
					",\n");
			} else {
				if (entry->end.signedness) {
					ret = lttng_metadata_printf(session,
						" ... %lld,\n",
						(long long) entry->end.value);
				} else {
					ret = lttng_metadata_printf(session,
						" ... %llu,\n",
						entry->end.value);
				}
			}
			if (ret)
				goto end;
		}
	}
	ret = print_tabs(session, nesting);
	if (ret)
		goto end;
	ret = lttng_metadata_printf(session, "} _%s;\n",
			field->name);
end:
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_field_statedump(struct lttng_session *session,
		const struct lttng_event_field *field,
		size_t nesting)
{
	int ret = 0;

	switch (field->type.atype) {
	case atype_integer:
		ret = print_tabs(session, nesting);
		if (ret)
			return ret;
		ret = lttng_metadata_printf(session,
			"integer { size = %u; align = %u; signed = %u; encoding = %s; base = %u;%s } _%s;\n",
			field->type.u.basic.integer.size,
			field->type.u.basic.integer.alignment,
			field->type.u.basic.integer.signedness,
			(field->type.u.basic.integer.encoding == lttng_encode_none)
				? "none"
				: (field->type.u.basic.integer.encoding == lttng_encode_UTF8)
					? "UTF8"
					: "ASCII",
			field->type.u.basic.integer.base,
#if __BYTE_ORDER == __BIG_ENDIAN
			field->type.u.basic.integer.reverse_byte_order ? " byte_order = le;" : "",
#else
			field->type.u.basic.integer.reverse_byte_order ? " byte_order = be;" : "",
#endif
			field->name);
		break;
	case atype_enum:
		ret = _lttng_enum_statedump(session, field, nesting);
		break;
	case atype_array:
	case atype_array_bitfield:
	{
		const struct lttng_basic_type *elem_type;

		elem_type = &field->type.u.array.elem_type;
		if (field->type.u.array.elem_alignment) {
			ret = print_tabs(session, nesting);
			if (ret)
				return ret;
			ret = lttng_metadata_printf(session,
			"struct { } align(%u) _%s_padding;\n",
					field->type.u.array.elem_alignment * CHAR_BIT,
					field->name);
			if (ret)
				return ret;
		}
		ret = print_tabs(session, nesting);
		if (ret)
			return ret;
		ret = lttng_metadata_printf(session,
			"integer { size = %u; align = %u; signed = %u; encoding = %s; base = %u;%s } _%s[%u];\n",
			elem_type->u.basic.integer.size,
			elem_type->u.basic.integer.alignment,
			elem_type->u.basic.integer.signedness,
			(elem_type->u.basic.integer.encoding == lttng_encode_none)
				? "none"
				: (elem_type->u.basic.integer.encoding == lttng_encode_UTF8)
					? "UTF8"
					: "ASCII",
			elem_type->u.basic.integer.base,
#if __BYTE_ORDER == __BIG_ENDIAN
			elem_type->u.basic.integer.reverse_byte_order ? " byte_order = le;" : "",
#else
			elem_type->u.basic.integer.reverse_byte_order ? " byte_order = be;" : "",
#endif
			field->name, field->type.u.array.length);
		break;
	}
	case atype_sequence:
	case atype_sequence_bitfield:
	{
		const struct lttng_basic_type *elem_type;
		const struct lttng_basic_type *length_type;

		elem_type = &field->type.u.sequence.elem_type;
		length_type = &field->type.u.sequence.length_type;
		ret = print_tabs(session, nesting);
		if (ret)
			return ret;
		ret = lttng_metadata_printf(session,
			"integer { size = %u; align = %u; signed = %u; encoding = %s; base = %u;%s } __%s_length;\n",
			length_type->u.basic.integer.size,
			(unsigned int) length_type->u.basic.integer.alignment,
			length_type->u.basic.integer.signedness,
			(length_type->u.basic.integer.encoding == lttng_encode_none)
				? "none"
				: ((length_type->u.basic.integer.encoding == lttng_encode_UTF8)
					? "UTF8"
					: "ASCII"),
			length_type->u.basic.integer.base,
#if __BYTE_ORDER == __BIG_ENDIAN
			length_type->u.basic.integer.reverse_byte_order ? " byte_order = le;" : "",
#else
			length_type->u.basic.integer.reverse_byte_order ? " byte_order = be;" : "",
#endif
			field->name);
		if (ret)
			return ret;

		if (field->type.u.sequence.elem_alignment) {
			ret = print_tabs(session, nesting);
			if (ret)
				return ret;
			ret = lttng_metadata_printf(session,
			"struct { } align(%u) _%s_padding;\n",
					field->type.u.sequence.elem_alignment * CHAR_BIT,
					field->name);
			if (ret)
				return ret;
		}
		ret = print_tabs(session, nesting);
		if (ret)
			return ret;
		ret = lttng_metadata_printf(session,
			"integer { size = %u; align = %u; signed = %u; encoding = %s; base = %u;%s } _%s[ __%s_length ];\n",
			elem_type->u.basic.integer.size,
			(unsigned int) elem_type->u.basic.integer.alignment,
			elem_type->u.basic.integer.signedness,
			(elem_type->u.basic.integer.encoding == lttng_encode_none)
				? "none"
				: ((elem_type->u.basic.integer.encoding == lttng_encode_UTF8)
					? "UTF8"
					: "ASCII"),
			elem_type->u.basic.integer.base,
#if __BYTE_ORDER == __BIG_ENDIAN
			elem_type->u.basic.integer.reverse_byte_order ? " byte_order = le;" : "",
#else
			elem_type->u.basic.integer.reverse_byte_order ? " byte_order = be;" : "",
#endif
			field->name,
			field->name);
		break;
	}

	case atype_string:
		/* Default encoding is UTF8 */
		ret = print_tabs(session, nesting);
		if (ret)
			return ret;
		ret = lttng_metadata_printf(session,
			"string%s _%s;\n",
			field->type.u.basic.string.encoding == lttng_encode_ASCII ?
				" { encoding = ASCII; }" : "",
			field->name);
		break;
	case atype_struct:
		ret = _lttng_struct_statedump(session, field, nesting);
		break;
	case atype_array_compound:
		ret = _lttng_array_compound_statedump(session, field, nesting);
		break;
	case atype_sequence_compound:
		ret = _lttng_sequence_compound_statedump(session, field, nesting);
		break;
	case atype_variant:
		ret = _lttng_variant_statedump(session, field, nesting);
		break;

	default:
		WARN_ON_ONCE(1);
		return -EINVAL;
	}
	return ret;
}

static
int _lttng_context_metadata_statedump(struct lttng_session *session,
				    struct lttng_ctx *ctx)
{
	int ret = 0;
	int i;

	if (!ctx)
		return 0;
	for (i = 0; i < ctx->nr_fields; i++) {
		const struct lttng_ctx_field *field = &ctx->fields[i];

		ret = _lttng_field_statedump(session, &field->event_field, 2);
		if (ret)
			return ret;
	}
	return ret;
}

static
int _lttng_fields_metadata_statedump(struct lttng_session *session,
				   struct lttng_event *event)
{
	const struct lttng_event_desc *desc = event->desc;
	int ret = 0;
	int i;

	for (i = 0; i < desc->nr_fields; i++) {
		const struct lttng_event_field *field = &desc->fields[i];

		ret = _lttng_field_statedump(session, field, 2);
		if (ret)
			return ret;
	}
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_event_metadata_statedump(struct lttng_session *session,
				  struct lttng_channel *chan,
				  struct lttng_event *event)
{
	int ret = 0;

	if (event->metadata_dumped || !READ_ONCE(session->active))
		return 0;
	if (chan->channel_type == METADATA_CHANNEL)
		return 0;

	ret = lttng_metadata_printf(session,
		"event {\n"
		"	name = \"%s\";\n"
		"	id = %u;\n"
		"	stream_id = %u;\n",
		event->desc->name,
		event->id,
		event->chan->id);
	if (ret)
		goto end;

	if (event->ctx) {
		ret = lttng_metadata_printf(session,
			"	context := struct {\n");
		if (ret)
			goto end;
	}
	ret = _lttng_context_metadata_statedump(session, event->ctx);
	if (ret)
		goto end;
	if (event->ctx) {
		ret = lttng_metadata_printf(session,
			"	};\n");
		if (ret)
			goto end;
	}

	ret = lttng_metadata_printf(session,
		"	fields := struct {\n"
		);
	if (ret)
		goto end;

	ret = _lttng_fields_metadata_statedump(session, event);
	if (ret)
		goto end;

	/*
	 * LTTng space reservation can only reserve multiples of the
	 * byte size.
	 */
	ret = lttng_metadata_printf(session,
		"	};\n"
		"};\n\n");
	if (ret)
		goto end;

	event->metadata_dumped = 1;
end:
	return ret;

}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_channel_metadata_statedump(struct lttng_session *session,
				    struct lttng_channel *chan)
{
	int ret = 0;

	if (chan->metadata_dumped || !READ_ONCE(session->active))
		return 0;

	if (chan->channel_type == METADATA_CHANNEL)
		return 0;

	WARN_ON_ONCE(!chan->header_type);
	ret = lttng_metadata_printf(session,
		"stream {\n"
		"	id = %u;\n"
		"	event.header := %s;\n"
		"	packet.context := struct packet_context;\n",
		chan->id,
		chan->header_type == 1 ? "struct event_header_compact" :
			"struct event_header_large");
	if (ret)
		goto end;

	if (chan->ctx) {
		ret = lttng_metadata_printf(session,
			"	event.context := struct {\n");
		if (ret)
			goto end;
	}
	ret = _lttng_context_metadata_statedump(session, chan->ctx);
	if (ret)
		goto end;
	if (chan->ctx) {
		ret = lttng_metadata_printf(session,
			"	};\n");
		if (ret)
			goto end;
	}

	ret = lttng_metadata_printf(session,
		"};\n\n");

	chan->metadata_dumped = 1;
end:
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_stream_packet_context_declare(struct lttng_session *session)
{
	return lttng_metadata_printf(session,
		"struct packet_context {\n"
		"	uint64_clock_monotonic_t timestamp_begin;\n"
		"	uint64_clock_monotonic_t timestamp_end;\n"
		"	uint64_t content_size;\n"
		"	uint64_t packet_size;\n"
		"	uint64_t packet_seq_num;\n"
		"	unsigned long events_discarded;\n"
		"	uint32_t cpu_id;\n"
		"};\n\n"
		);
}

/*
 * Compact header:
 * id: range: 0 - 30.
 * id 31 is reserved to indicate an extended header.
 *
 * Large header:
 * id: range: 0 - 65534.
 * id 65535 is reserved to indicate an extended header.
 *
 * Must be called with sessions_mutex held.
 */
static
int _lttng_event_header_declare(struct lttng_session *session)
{
	return lttng_metadata_printf(session,
	"struct event_header_compact {\n"
	"	enum : uint5_t { compact = 0 ... 30, extended = 31 } id;\n"
	"	variant <id> {\n"
	"		struct {\n"
	"			uint27_clock_monotonic_t timestamp;\n"
	"		} compact;\n"
	"		struct {\n"
	"			uint32_t id;\n"
	"			uint64_clock_monotonic_t timestamp;\n"
	"		} extended;\n"
	"	} v;\n"
	"} align(%u);\n"
	"\n"
	"struct event_header_large {\n"
	"	enum : uint16_t { compact = 0 ... 65534, extended = 65535 } id;\n"
	"	variant <id> {\n"
	"		struct {\n"
	"			uint32_clock_monotonic_t timestamp;\n"
	"		} compact;\n"
	"		struct {\n"
	"			uint32_t id;\n"
	"			uint64_clock_monotonic_t timestamp;\n"
	"		} extended;\n"
	"	} v;\n"
	"} align(%u);\n\n",
	lttng_alignof(uint32_t) * CHAR_BIT,
	lttng_alignof(uint16_t) * CHAR_BIT
	);
}

 /*
 * Approximation of NTP time of day to clock monotonic correlation,
 * taken at start of trace.
 * Yes, this is only an approximation. Yes, we can (and will) do better
 * in future versions.
 * This function may return a negative offset. It may happen if the
 * system sets the REALTIME clock to 0 after boot.
 */
static
int64_t measure_clock_offset(void)
{
	uint64_t monotonic_avg, monotonic[2], realtime;
	uint64_t tcf = trace_clock_freq();
	int64_t offset;
	struct timespec rts = { 0, 0 };
	unsigned long flags;

	/* Disable interrupts to increase correlation precision. */
	local_irq_save(flags);
	monotonic[0] = trace_clock_read64();
	getnstimeofday(&rts);
	monotonic[1] = trace_clock_read64();
	local_irq_restore(flags);

	monotonic_avg = (monotonic[0] + monotonic[1]) >> 1;
	realtime = (uint64_t) rts.tv_sec * tcf;
	if (tcf == NSEC_PER_SEC) {
		realtime += rts.tv_nsec;
	} else {
		uint64_t n = rts.tv_nsec * tcf;

		do_div(n, NSEC_PER_SEC);
		realtime += n;
	}
	offset = (int64_t) realtime - monotonic_avg;
	return offset;
}

/*
 * Output metadata into this session's metadata buffers.
 * Must be called with sessions_mutex held.
 */
static
int _lttng_session_metadata_statedump(struct lttng_session *session)
{
	unsigned char *uuid_c = session->uuid.b;
	unsigned char uuid_s[37], clock_uuid_s[BOOT_ID_LEN];
	struct lttng_channel *chan;
	struct lttng_event *event;
	int ret = 0;

	if (!READ_ONCE(session->active))
		return 0;
	if (session->metadata_dumped)
		goto skip_session;

	snprintf(uuid_s, sizeof(uuid_s),
		"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		uuid_c[0], uuid_c[1], uuid_c[2], uuid_c[3],
		uuid_c[4], uuid_c[5], uuid_c[6], uuid_c[7],
		uuid_c[8], uuid_c[9], uuid_c[10], uuid_c[11],
		uuid_c[12], uuid_c[13], uuid_c[14], uuid_c[15]);

	ret = lttng_metadata_printf(session,
		"typealias integer { size = 8; align = %u; signed = false; } := uint8_t;\n"
		"typealias integer { size = 16; align = %u; signed = false; } := uint16_t;\n"
		"typealias integer { size = 32; align = %u; signed = false; } := uint32_t;\n"
		"typealias integer { size = 64; align = %u; signed = false; } := uint64_t;\n"
		"typealias integer { size = %u; align = %u; signed = false; } := unsigned long;\n"
		"typealias integer { size = 5; align = 1; signed = false; } := uint5_t;\n"
		"typealias integer { size = 27; align = 1; signed = false; } := uint27_t;\n"
		"\n"
		"trace {\n"
		"	major = %u;\n"
		"	minor = %u;\n"
		"	uuid = \"%s\";\n"
		"	byte_order = %s;\n"
		"	packet.header := struct {\n"
		"		uint32_t magic;\n"
		"		uint8_t  uuid[16];\n"
		"		uint32_t stream_id;\n"
		"		uint64_t stream_instance_id;\n"
		"	};\n"
		"};\n\n",
		lttng_alignof(uint8_t) * CHAR_BIT,
		lttng_alignof(uint16_t) * CHAR_BIT,
		lttng_alignof(uint32_t) * CHAR_BIT,
		lttng_alignof(uint64_t) * CHAR_BIT,
		sizeof(unsigned long) * CHAR_BIT,
		lttng_alignof(unsigned long) * CHAR_BIT,
		CTF_SPEC_MAJOR,
		CTF_SPEC_MINOR,
		uuid_s,
#if __BYTE_ORDER == __BIG_ENDIAN
		"be"
#else
		"le"
#endif
		);
	if (ret)
		goto end;

	ret = lttng_metadata_printf(session,
		"env {\n"
		"	hostname = \"%s\";\n"
		"	domain = \"kernel\";\n"
		"	sysname = \"%s\";\n"
		"	kernel_release = \"%s\";\n"
		"	kernel_version = \"%s\";\n"
		"	tracer_name = \"lttng-modules\";\n"
		"	tracer_major = %d;\n"
		"	tracer_minor = %d;\n"
		"	tracer_patchlevel = %d;\n"
		"};\n\n",
		current->nsproxy->uts_ns->name.nodename,
		utsname()->sysname,
		utsname()->release,
		utsname()->version,
		LTTNG_MODULES_MAJOR_VERSION,
		LTTNG_MODULES_MINOR_VERSION,
		LTTNG_MODULES_PATCHLEVEL_VERSION
		);
	if (ret)
		goto end;

	ret = lttng_metadata_printf(session,
		"clock {\n"
		"	name = \"%s\";\n",
		trace_clock_name()
		);
	if (ret)
		goto end;

	if (!trace_clock_uuid(clock_uuid_s)) {
		ret = lttng_metadata_printf(session,
			"	uuid = \"%s\";\n",
			clock_uuid_s
			);
		if (ret)
			goto end;
	}

	ret = lttng_metadata_printf(session,
		"	description = \"%s\";\n"
		"	freq = %llu; /* Frequency, in Hz */\n"
		"	/* clock value offset from Epoch is: offset * (1/freq) */\n"
		"	offset = %lld;\n"
		"};\n\n",
		trace_clock_description(),
		(unsigned long long) trace_clock_freq(),
		(long long) measure_clock_offset()
		);
	if (ret)
		goto end;

	ret = lttng_metadata_printf(session,
		"typealias integer {\n"
		"	size = 27; align = 1; signed = false;\n"
		"	map = clock.%s.value;\n"
		"} := uint27_clock_monotonic_t;\n"
		"\n"
		"typealias integer {\n"
		"	size = 32; align = %u; signed = false;\n"
		"	map = clock.%s.value;\n"
		"} := uint32_clock_monotonic_t;\n"
		"\n"
		"typealias integer {\n"
		"	size = 64; align = %u; signed = false;\n"
		"	map = clock.%s.value;\n"
		"} := uint64_clock_monotonic_t;\n\n",
		trace_clock_name(),
		lttng_alignof(uint32_t) * CHAR_BIT,
		trace_clock_name(),
		lttng_alignof(uint64_t) * CHAR_BIT,
		trace_clock_name()
		);
	if (ret)
		goto end;

	ret = _lttng_stream_packet_context_declare(session);
	if (ret)
		goto end;

	ret = _lttng_event_header_declare(session);
	if (ret)
		goto end;

skip_session:
	list_for_each_entry(chan, &session->chan, list) {
		ret = _lttng_channel_metadata_statedump(session, chan);
		if (ret)
			goto end;
	}

	list_for_each_entry(event, &session->events, list) {
		ret = _lttng_event_metadata_statedump(session, event->chan, event);
		if (ret)
			goto end;
	}
	session->metadata_dumped = 1;
end:
	return ret;
}

/**
 * lttng_transport_register - LTT transport registration
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
void lttng_transport_register(struct lttng_transport *transport)
{
	/*
	 * Make sure no page fault can be triggered by the module about to be
	 * registered. We deal with this here so we don't have to call
	 * vmalloc_sync_all() in each module's init.
	 */
	wrapper_vmalloc_sync_all();

	mutex_lock(&sessions_mutex);
	list_add_tail(&transport->node, &lttng_transport_list);
	mutex_unlock(&sessions_mutex);
}
EXPORT_SYMBOL_GPL(lttng_transport_register);

/**
 * lttng_transport_unregister - LTT transport unregistration
 * @transport: transport structure
 */
void lttng_transport_unregister(struct lttng_transport *transport)
{
	mutex_lock(&sessions_mutex);
	list_del(&transport->node);
	mutex_unlock(&sessions_mutex);
}
EXPORT_SYMBOL_GPL(lttng_transport_unregister);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0))

enum cpuhp_state lttng_hp_prepare;
enum cpuhp_state lttng_hp_online;

static int lttng_hotplug_prepare(unsigned int cpu, struct hlist_node *node)
{
	struct lttng_cpuhp_node *lttng_node;

	lttng_node = container_of(node, struct lttng_cpuhp_node, node);
	switch (lttng_node->component) {
	case LTTNG_RING_BUFFER_FRONTEND:
		return 0;
	case LTTNG_RING_BUFFER_BACKEND:
		return lttng_cpuhp_rb_backend_prepare(cpu, lttng_node);
	case LTTNG_RING_BUFFER_ITER:
		return 0;
	case LTTNG_CONTEXT_PERF_COUNTERS:
		return 0;
	default:
		return -EINVAL;
	}
}

static int lttng_hotplug_dead(unsigned int cpu, struct hlist_node *node)
{
	struct lttng_cpuhp_node *lttng_node;

	lttng_node = container_of(node, struct lttng_cpuhp_node, node);
	switch (lttng_node->component) {
	case LTTNG_RING_BUFFER_FRONTEND:
		return lttng_cpuhp_rb_frontend_dead(cpu, lttng_node);
	case LTTNG_RING_BUFFER_BACKEND:
		return 0;
	case LTTNG_RING_BUFFER_ITER:
		return 0;
	case LTTNG_CONTEXT_PERF_COUNTERS:
		return lttng_cpuhp_perf_counter_dead(cpu, lttng_node);
	default:
		return -EINVAL;
	}
}

static int lttng_hotplug_online(unsigned int cpu, struct hlist_node *node)
{
	struct lttng_cpuhp_node *lttng_node;

	lttng_node = container_of(node, struct lttng_cpuhp_node, node);
	switch (lttng_node->component) {
	case LTTNG_RING_BUFFER_FRONTEND:
		return lttng_cpuhp_rb_frontend_online(cpu, lttng_node);
	case LTTNG_RING_BUFFER_BACKEND:
		return 0;
	case LTTNG_RING_BUFFER_ITER:
		return lttng_cpuhp_rb_iter_online(cpu, lttng_node);
	case LTTNG_CONTEXT_PERF_COUNTERS:
		return lttng_cpuhp_perf_counter_online(cpu, lttng_node);
	default:
		return -EINVAL;
	}
}

static int lttng_hotplug_offline(unsigned int cpu, struct hlist_node *node)
{
	struct lttng_cpuhp_node *lttng_node;

	lttng_node = container_of(node, struct lttng_cpuhp_node, node);
	switch (lttng_node->component) {
	case LTTNG_RING_BUFFER_FRONTEND:
		return lttng_cpuhp_rb_frontend_offline(cpu, lttng_node);
	case LTTNG_RING_BUFFER_BACKEND:
		return 0;
	case LTTNG_RING_BUFFER_ITER:
		return 0;
	case LTTNG_CONTEXT_PERF_COUNTERS:
		return 0;
	default:
		return -EINVAL;
	}
}

static int __init lttng_init_cpu_hotplug(void)
{
	int ret;

	ret = cpuhp_setup_state_multi(CPUHP_BP_PREPARE_DYN, "lttng:prepare",
			lttng_hotplug_prepare,
			lttng_hotplug_dead);
	if (ret < 0) {
		return ret;
	}
	lttng_hp_prepare = ret;
	lttng_rb_set_hp_prepare(ret);

	ret = cpuhp_setup_state_multi(CPUHP_AP_ONLINE_DYN, "lttng:online",
			lttng_hotplug_online,
			lttng_hotplug_offline);
	if (ret < 0) {
		cpuhp_remove_multi_state(lttng_hp_prepare);
		lttng_hp_prepare = 0;
		return ret;
	}
	lttng_hp_online = ret;
	lttng_rb_set_hp_online(ret);

	return 0;
}

static void __exit lttng_exit_cpu_hotplug(void)
{
	lttng_rb_set_hp_online(0);
	cpuhp_remove_multi_state(lttng_hp_online);
	lttng_rb_set_hp_prepare(0);
	cpuhp_remove_multi_state(lttng_hp_prepare);
}

#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0)) */
static int lttng_init_cpu_hotplug(void)
{
	return 0;
}
static void lttng_exit_cpu_hotplug(void)
{
}
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0)) */


static int __init lttng_events_init(void)
{
	int ret;

	ret = wrapper_lttng_fixup_sig(THIS_MODULE);
	if (ret)
		return ret;
	ret = wrapper_get_pfnblock_flags_mask_init();
	if (ret)
		return ret;
	ret = wrapper_get_pageblock_flags_mask_init();
	if (ret)
		return ret;
	ret = lttng_probes_init();
	if (ret)
		return ret;
	ret = lttng_context_init();
	if (ret)
		return ret;
	ret = lttng_tracepoint_init();
	if (ret)
		goto error_tp;
	event_cache = KMEM_CACHE(lttng_event, 0);
	if (!event_cache) {
		ret = -ENOMEM;
		goto error_kmem;
	}
	ret = lttng_abi_init();
	if (ret)
		goto error_abi;
	ret = lttng_logger_init();
	if (ret)
		goto error_logger;
	ret = lttng_init_cpu_hotplug();
	if (ret)
		goto error_hotplug;
	printk(KERN_NOTICE "LTTng: Loaded modules v%s.%s.%s%s (%s)%s%s\n",
		__stringify(LTTNG_MODULES_MAJOR_VERSION),
		__stringify(LTTNG_MODULES_MINOR_VERSION),
		__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION),
		LTTNG_MODULES_EXTRAVERSION,
		LTTNG_VERSION_NAME,
#ifdef LTTNG_EXTRA_VERSION_GIT
		LTTNG_EXTRA_VERSION_GIT[0] == '\0' ? "" : " - " LTTNG_EXTRA_VERSION_GIT,
#else
		"",
#endif
#ifdef LTTNG_EXTRA_VERSION_NAME
		LTTNG_EXTRA_VERSION_NAME[0] == '\0' ? "" : " - " LTTNG_EXTRA_VERSION_NAME);
#else
		"");
#endif
	return 0;

error_hotplug:
	lttng_logger_exit();
error_logger:
	lttng_abi_exit();
error_abi:
	kmem_cache_destroy(event_cache);
error_kmem:
	lttng_tracepoint_exit();
error_tp:
	lttng_context_exit();
	printk(KERN_NOTICE "LTTng: Failed to load modules v%s.%s.%s%s (%s)%s%s\n",
		__stringify(LTTNG_MODULES_MAJOR_VERSION),
		__stringify(LTTNG_MODULES_MINOR_VERSION),
		__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION),
		LTTNG_MODULES_EXTRAVERSION,
		LTTNG_VERSION_NAME,
#ifdef LTTNG_EXTRA_VERSION_GIT
		LTTNG_EXTRA_VERSION_GIT[0] == '\0' ? "" : " - " LTTNG_EXTRA_VERSION_GIT,
#else
		"",
#endif
#ifdef LTTNG_EXTRA_VERSION_NAME
		LTTNG_EXTRA_VERSION_NAME[0] == '\0' ? "" : " - " LTTNG_EXTRA_VERSION_NAME);
#else
		"");
#endif
	return ret;
}

module_init(lttng_events_init);

static void __exit lttng_events_exit(void)
{
	struct lttng_session *session, *tmpsession;

	lttng_exit_cpu_hotplug();
	lttng_logger_exit();
	lttng_abi_exit();
	list_for_each_entry_safe(session, tmpsession, &sessions, list)
		lttng_session_destroy(session);
	kmem_cache_destroy(event_cache);
	lttng_tracepoint_exit();
	lttng_context_exit();
	printk(KERN_NOTICE "LTTng: Unloaded modules v%s.%s.%s%s (%s)%s%s\n",
		__stringify(LTTNG_MODULES_MAJOR_VERSION),
		__stringify(LTTNG_MODULES_MINOR_VERSION),
		__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION),
		LTTNG_MODULES_EXTRAVERSION,
		LTTNG_VERSION_NAME,
#ifdef LTTNG_EXTRA_VERSION_GIT
		LTTNG_EXTRA_VERSION_GIT[0] == '\0' ? "" : " - " LTTNG_EXTRA_VERSION_GIT,
#else
		"",
#endif
#ifdef LTTNG_EXTRA_VERSION_NAME
		LTTNG_EXTRA_VERSION_NAME[0] == '\0' ? "" : " - " LTTNG_EXTRA_VERSION_NAME);
#else
		"");
#endif
}

module_exit(lttng_events_exit);

#include "extra_version/patches.i"
#ifdef LTTNG_EXTRA_VERSION_GIT
MODULE_INFO(extra_version_git, LTTNG_EXTRA_VERSION_GIT);
#endif
#ifdef LTTNG_EXTRA_VERSION_NAME
MODULE_INFO(extra_version_name, LTTNG_EXTRA_VERSION_NAME);
#endif
MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng tracer");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
