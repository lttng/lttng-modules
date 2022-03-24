/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
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
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/dmi.h>

#include <wrapper/compiler_attributes.h>
#include <wrapper/uuid.h>
#include <wrapper/vmalloc.h>	/* for wrapper_vmalloc_sync_mappings() */
#include <wrapper/random.h>
#include <wrapper/tracepoint.h>
#include <wrapper/list.h>
#include <wrapper/types.h>
#include <lttng/kernel-version.h>
#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <lttng/lttng-bytecode.h>
#include <lttng/tracer.h>
#include <lttng/event-notifier-notification.h>
#include <lttng/abi-old.h>
#include <lttng/endian.h>
#include <lttng/string-utils.h>
#include <lttng/utils.h>
#include <counter/counter.h>
#include <ringbuffer/backend.h>
#include <ringbuffer/frontend.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,16,0) \
	|| LTTNG_RHEL_KERNEL_RANGE(5,14,0,162,0,0, 5,15,0,0,0,0))
#include <linux/stdarg.h>
#else
#include <stdarg.h>
#endif

#define METADATA_CACHE_DEFAULT_SIZE 4096

static LIST_HEAD(sessions);
static LIST_HEAD(event_notifier_groups);
static LIST_HEAD(lttng_transport_list);
static LIST_HEAD(lttng_counter_transport_list);
/*
 * Protect the sessions and metadata caches.
 */
static DEFINE_MUTEX(sessions_mutex);
static struct kmem_cache *event_recorder_cache;
static struct kmem_cache *event_recorder_private_cache;
static struct kmem_cache *event_counter_cache;
static struct kmem_cache *event_counter_private_cache;
static struct kmem_cache *event_notifier_cache;
static struct kmem_cache *event_notifier_private_cache;

static void lttng_session_lazy_sync_event_enablers(struct lttng_kernel_session *session);
static void lttng_session_sync_event_enablers(struct lttng_kernel_session *session);
static void lttng_event_notifier_group_sync_enablers(struct lttng_event_notifier_group *event_notifier_group);
static void lttng_event_enabler_sync(struct lttng_event_enabler_common *event_enabler);

static void _lttng_event_destroy(struct lttng_kernel_event_common *event);
static void _lttng_channel_destroy(struct lttng_kernel_channel_common *chan);
static void _lttng_event_unregister(struct lttng_kernel_event_common *event);
static
int _lttng_event_recorder_metadata_statedump(struct lttng_kernel_event_common *event);
static
int _lttng_session_metadata_statedump(struct lttng_kernel_session *session);
static
void _lttng_metadata_channel_hangup(struct lttng_metadata_stream *stream);
static
int _lttng_type_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_type_common *type,
		enum lttng_kernel_string_encoding parent_encoding,
		size_t nesting);
static
int _lttng_field_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting, const char **prev_field_name_p);

void synchronize_trace(void)
{
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,1,0) || \
	LTTNG_RHEL_KERNEL_RANGE(4,18,0,193,0,0, 4,19,0,0,0,0))
	synchronize_rcu();
#else
	synchronize_sched();
#endif

#ifdef CONFIG_PREEMPT_RT_FULL
	synchronize_rcu();
#endif
}

void lttng_lock_sessions(void)
{
	mutex_lock(&sessions_mutex);
}

void lttng_unlock_sessions(void)
{
	mutex_unlock(&sessions_mutex);
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

/*
 * Called with sessions lock held.
 */
int lttng_session_active(void)
{
	struct lttng_kernel_session_private *iter;

	list_for_each_entry(iter, &sessions, node) {
		if (iter->pub->active)
			return 1;
	}
	return 0;
}

struct lttng_kernel_session *lttng_session_create(void)
{
	struct lttng_kernel_session *session;
	struct lttng_kernel_session_private *session_priv;
	struct lttng_metadata_cache *metadata_cache;
	int i;

	mutex_lock(&sessions_mutex);
	session = lttng_kvzalloc(sizeof(*session), GFP_KERNEL);
	if (!session)
		goto err;
	session_priv = lttng_kvzalloc(sizeof(*session_priv), GFP_KERNEL);
	if (!session_priv)
		goto err_free_session;
	session->priv = session_priv;
	session_priv->pub = session;

	INIT_LIST_HEAD(&session_priv->chan_head);
	INIT_LIST_HEAD(&session_priv->events_head);
	lttng_guid_gen(&session_priv->uuid);

	metadata_cache = kzalloc(sizeof(struct lttng_metadata_cache),
			GFP_KERNEL);
	if (!metadata_cache)
		goto err_free_session_private;
	metadata_cache->data = vzalloc(METADATA_CACHE_DEFAULT_SIZE);
	if (!metadata_cache->data)
		goto err_free_cache;
	metadata_cache->cache_alloc = METADATA_CACHE_DEFAULT_SIZE;
	kref_init(&metadata_cache->refcount);
	mutex_init(&metadata_cache->lock);
	session_priv->metadata_cache = metadata_cache;
	INIT_LIST_HEAD(&metadata_cache->metadata_stream);
	memcpy(&metadata_cache->uuid, &session_priv->uuid,
		sizeof(metadata_cache->uuid));
	INIT_LIST_HEAD(&session_priv->enablers_head);
	for (i = 0; i < LTTNG_EVENT_HT_SIZE; i++)
		INIT_HLIST_HEAD(&session_priv->events_name_ht.table[i]);
	for (i = 0; i < LTTNG_EVENT_HT_SIZE; i++)
		INIT_HLIST_HEAD(&session_priv->events_key_ht.table[i]);
	list_add(&session_priv->node, &sessions);

	if (lttng_id_tracker_init(&session->pid_tracker, session, TRACKER_PID))
		goto tracker_alloc_error;
	if (lttng_id_tracker_init(&session->vpid_tracker, session, TRACKER_VPID))
		goto tracker_alloc_error;
	if (lttng_id_tracker_init(&session->uid_tracker, session, TRACKER_UID))
		goto tracker_alloc_error;
	if (lttng_id_tracker_init(&session->vuid_tracker, session, TRACKER_VUID))
		goto tracker_alloc_error;
	if (lttng_id_tracker_init(&session->gid_tracker, session, TRACKER_GID))
		goto tracker_alloc_error;
	if (lttng_id_tracker_init(&session->vgid_tracker, session, TRACKER_VGID))
		goto tracker_alloc_error;

	mutex_unlock(&sessions_mutex);

	return session;

tracker_alloc_error:
	lttng_id_tracker_fini(&session->pid_tracker);
	lttng_id_tracker_fini(&session->vpid_tracker);
	lttng_id_tracker_fini(&session->uid_tracker);
	lttng_id_tracker_fini(&session->vuid_tracker);
	lttng_id_tracker_fini(&session->gid_tracker);
	lttng_id_tracker_fini(&session->vgid_tracker);
err_free_cache:
	kfree(metadata_cache);
err_free_session_private:
	lttng_kvfree(session_priv);
err_free_session:
	lttng_kvfree(session);
err:
	mutex_unlock(&sessions_mutex);
	return NULL;
}

static
struct lttng_counter_transport *lttng_counter_transport_find(const char *name)
{
	struct lttng_counter_transport *transport;

	list_for_each_entry(transport, &lttng_counter_transport_list, node) {
		if (!strcmp(transport->name, name))
			return transport;
	}
	return NULL;
}

struct lttng_kernel_channel_counter *lttng_kernel_counter_create(
		const char *counter_transport_name,
		size_t number_dimensions,
		const struct lttng_kernel_counter_dimension *dimensions,
		int64_t global_sum_step,
		bool coalesce_hits)
{
	struct lttng_counter_transport *counter_transport = NULL;
	struct lttng_kernel_channel_counter *counter = NULL;

	counter_transport = lttng_counter_transport_find(counter_transport_name);
	if (!counter_transport) {
		printk(KERN_WARNING "LTTng: counter transport %s not found.\n",
		       counter_transport_name);
		goto notransport;
	}
	if (!try_module_get(counter_transport->owner)) {
		printk(KERN_WARNING "LTTng: Can't lock counter transport module.\n");
		goto notransport;
	}

	counter = counter_transport->ops.priv->counter_create(number_dimensions, dimensions,
			global_sum_step);
	if (!counter)
		goto create_error;

	/* Create event notifier error counter. */
	counter->ops = &counter_transport->ops;
	counter->priv->parent.coalesce_hits = coalesce_hits;
	counter->priv->transport = counter_transport;
	mutex_init(&counter->priv->map.lock);

	return counter;

create_error:
	if (counter_transport)
		module_put(counter_transport->owner);
notransport:
	return NULL;
}

static
void lttng_kernel_counter_destroy(struct lttng_kernel_channel_counter *counter)
{
	struct lttng_counter_transport *counter_transport = counter->priv->transport;

	lttng_kvfree(counter->priv->map.descriptors);
	counter->ops->priv->counter_destroy(counter);
	module_put(counter_transport->owner);
}

struct lttng_event_notifier_group *lttng_event_notifier_group_create(void)
{
	struct lttng_transport *transport = NULL;
	struct lttng_event_notifier_group *event_notifier_group;
	const char *transport_name = "relay-event-notifier";
	size_t subbuf_size = 4096;	//TODO
	size_t num_subbuf = 16;		//TODO
	unsigned int switch_timer_interval = 0;
	unsigned int read_timer_interval = 0;
	int i;

	mutex_lock(&sessions_mutex);

	transport = lttng_transport_find(transport_name);
	if (!transport) {
		printk(KERN_WARNING "LTTng: transport %s not found\n",
		       transport_name);
		goto notransport;
	}
	if (!try_module_get(transport->owner)) {
		printk(KERN_WARNING "LTTng: Can't lock transport %s module.\n",
		       transport_name);
		goto notransport;
	}

	event_notifier_group = lttng_kvzalloc(sizeof(struct lttng_event_notifier_group),
				       GFP_KERNEL);
	if (!event_notifier_group)
		goto nomem;

	/*
	 * Initialize the ring buffer used to store event notifier
	 * notifications.
	 */
	event_notifier_group->ops = &transport->ops;
	event_notifier_group->chan = transport->ops.priv->channel_create(
			transport_name, event_notifier_group, NULL,
			subbuf_size, num_subbuf, switch_timer_interval,
			read_timer_interval);
	if (!event_notifier_group->chan)
		goto create_error;

	event_notifier_group->transport = transport;

	INIT_LIST_HEAD(&event_notifier_group->enablers_head);
	INIT_LIST_HEAD(&event_notifier_group->event_notifiers_head);
	for (i = 0; i < LTTNG_EVENT_HT_SIZE; i++)
		INIT_HLIST_HEAD(&event_notifier_group->events_name_ht.table[i]);

	list_add(&event_notifier_group->node, &event_notifier_groups);

	mutex_unlock(&sessions_mutex);

	return event_notifier_group;

create_error:
	lttng_kvfree(event_notifier_group);
nomem:
	if (transport)
		module_put(transport->owner);
notransport:
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

void lttng_session_destroy(struct lttng_kernel_session *session)
{
	struct lttng_kernel_channel_common_private *chan_priv, *tmpchan_priv;
	struct lttng_kernel_event_recorder_private *event_recorder_priv, *tmpevent_recorder_priv;
	struct lttng_metadata_stream *metadata_stream;
	struct lttng_event_enabler_common *event_enabler, *tmp_event_enabler;
	int ret;

	mutex_lock(&sessions_mutex);
	WRITE_ONCE(session->active, 0);
	list_for_each_entry(chan_priv, &session->priv->chan_head, node) {
		ret = lttng_syscalls_unregister_syscall_table(&chan_priv->syscall_table);
		WARN_ON(ret);
	}
	list_for_each_entry(event_recorder_priv, &session->priv->events_head, parent.parent.node)
		_lttng_event_unregister(&event_recorder_priv->pub->parent);
	synchronize_trace();	/* Wait for in-flight events to complete */
	list_for_each_entry(chan_priv, &session->priv->chan_head, node) {
		ret = lttng_syscalls_destroy_syscall_table(&chan_priv->syscall_table);
		WARN_ON(ret);
	}
	list_for_each_entry_safe(event_enabler, tmp_event_enabler, &session->priv->enablers_head, node)
		lttng_event_enabler_destroy(event_enabler);
	list_for_each_entry_safe(event_recorder_priv, tmpevent_recorder_priv, &session->priv->events_head, parent.parent.node)
		_lttng_event_destroy(&event_recorder_priv->pub->parent);
	list_for_each_entry_safe(chan_priv, tmpchan_priv, &session->priv->chan_head, node) {
		_lttng_channel_destroy(chan_priv->pub);
	}
	mutex_lock(&session->priv->metadata_cache->lock);
	list_for_each_entry(metadata_stream, &session->priv->metadata_cache->metadata_stream, node)
		_lttng_metadata_channel_hangup(metadata_stream);
	mutex_unlock(&session->priv->metadata_cache->lock);
	lttng_id_tracker_fini(&session->pid_tracker);
	lttng_id_tracker_fini(&session->vpid_tracker);
	lttng_id_tracker_fini(&session->uid_tracker);
	lttng_id_tracker_fini(&session->vuid_tracker);
	lttng_id_tracker_fini(&session->gid_tracker);
	lttng_id_tracker_fini(&session->vgid_tracker);
	kref_put(&session->priv->metadata_cache->refcount, metadata_cache_destroy);
	list_del(&session->priv->node);
	mutex_unlock(&sessions_mutex);
	lttng_kvfree(session->priv);
	lttng_kvfree(session);
}

void lttng_event_notifier_group_destroy(
		struct lttng_event_notifier_group *event_notifier_group)
{
	struct lttng_event_enabler_common *event_enabler, *tmp_event_enabler;
	struct lttng_kernel_event_notifier_private *event_notifier_priv, *tmpevent_notifier_priv;
	int ret;

	if (!event_notifier_group)
		return;

	mutex_lock(&sessions_mutex);

	ret = lttng_syscalls_unregister_syscall_table(&event_notifier_group->syscall_table);
	WARN_ON(ret);

	list_for_each_entry_safe(event_notifier_priv, tmpevent_notifier_priv,
			&event_notifier_group->event_notifiers_head, parent.node)
		_lttng_event_unregister(&event_notifier_priv->pub->parent);

	/* Wait for in-flight event notifier to complete */
	synchronize_trace();

	irq_work_sync(&event_notifier_group->wakeup_pending);

	ret = lttng_syscalls_destroy_syscall_table(&event_notifier_group->syscall_table);
	WARN_ON(ret);

	list_for_each_entry_safe(event_enabler, tmp_event_enabler,
			&event_notifier_group->enablers_head, node)
		lttng_event_enabler_destroy(event_enabler);

	list_for_each_entry_safe(event_notifier_priv, tmpevent_notifier_priv,
			&event_notifier_group->event_notifiers_head, parent.node)
		_lttng_event_destroy(&event_notifier_priv->pub->parent);

	if (event_notifier_group->error_counter)
		lttng_kernel_counter_destroy(event_notifier_group->error_counter);

	event_notifier_group->ops->priv->channel_destroy(event_notifier_group->chan);
	module_put(event_notifier_group->transport->owner);
	list_del(&event_notifier_group->node);

	mutex_unlock(&sessions_mutex);
	lttng_kvfree(event_notifier_group);
}

int lttng_session_statedump(struct lttng_kernel_session *session)
{
	int ret;

	mutex_lock(&sessions_mutex);
	ret = lttng_statedump_start(session);
	mutex_unlock(&sessions_mutex);
	return ret;
}

int lttng_session_enable(struct lttng_kernel_session *session)
{
	int ret = 0;
	struct lttng_kernel_channel_common_private *chan_priv;

	mutex_lock(&sessions_mutex);
	if (session->active) {
		ret = -EBUSY;
		goto end;
	}

	/* Set transient enabler state to "enabled" */
	session->priv->tstate = 1;

	/* We need to sync enablers with session before activation. */
	lttng_session_sync_event_enablers(session);

	/*
	 * Snapshot the number of events per channel to know the type of header
	 * we need to use.
	 */
	list_for_each_entry(chan_priv, &session->priv->chan_head, node) {
		struct lttng_kernel_channel_buffer_private *chan_buf_priv;

		if (chan_priv->pub->type != LTTNG_KERNEL_CHANNEL_TYPE_BUFFER)
			continue;
		chan_buf_priv = container_of(chan_priv, struct lttng_kernel_channel_buffer_private, parent);
		if (chan_buf_priv->header_type)
			continue;			/* don't change it if session stop/restart */
		if (chan_buf_priv->free_event_id < 31)
			chan_buf_priv->header_type = 1;	/* compact */
		else
			chan_buf_priv->header_type = 2;	/* large */
	}

	/* Clear each stream's quiescent state. */
	list_for_each_entry(chan_priv, &session->priv->chan_head, node) {
		struct lttng_kernel_channel_buffer_private *chan_buf_priv;

		if (chan_priv->pub->type != LTTNG_KERNEL_CHANNEL_TYPE_BUFFER)
			continue;
		chan_buf_priv = container_of(chan_priv, struct lttng_kernel_channel_buffer_private, parent);
		if (chan_buf_priv->channel_type != METADATA_CHANNEL)
			lib_ring_buffer_clear_quiescent_channel(chan_buf_priv->rb_chan);
	}

	WRITE_ONCE(session->active, 1);
	WRITE_ONCE(session->priv->been_active, 1);
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

int lttng_session_disable(struct lttng_kernel_session *session)
{
	int ret = 0;
	struct lttng_kernel_channel_common_private *chan_priv;

	mutex_lock(&sessions_mutex);
	if (!session->active) {
		ret = -EBUSY;
		goto end;
	}
	WRITE_ONCE(session->active, 0);

	/* Set transient enabler state to "disabled" */
	session->priv->tstate = 0;
	lttng_session_sync_event_enablers(session);

	/* Set each stream's quiescent state. */
	list_for_each_entry(chan_priv, &session->priv->chan_head, node) {
		struct lttng_kernel_channel_buffer_private *chan_buf_priv;

		if (chan_priv->pub->type != LTTNG_KERNEL_CHANNEL_TYPE_BUFFER)
			continue;
		chan_buf_priv = container_of(chan_priv, struct lttng_kernel_channel_buffer_private, parent);
		if (chan_buf_priv->channel_type != METADATA_CHANNEL)
			lib_ring_buffer_set_quiescent_channel(chan_buf_priv->rb_chan);
	}
end:
	mutex_unlock(&sessions_mutex);
	return ret;
}

int lttng_session_metadata_regenerate(struct lttng_kernel_session *session)
{
	int ret = 0;
	struct lttng_kernel_channel_common_private *chan_priv;
	struct lttng_kernel_event_recorder_private *event_recorder_priv;
	struct lttng_metadata_cache *cache = session->priv->metadata_cache;
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
	list_for_each_entry(stream, &session->priv->metadata_cache->metadata_stream, node) {
		stream->metadata_out = 0;
		stream->metadata_in = 0;
	}
	mutex_unlock(&cache->lock);

	session->priv->metadata_dumped = 0;
	list_for_each_entry(chan_priv, &session->priv->chan_head, node) {
		struct lttng_kernel_channel_buffer_private *chan_buf_priv;

		if (chan_priv->pub->type != LTTNG_KERNEL_CHANNEL_TYPE_BUFFER)
			continue;
		chan_buf_priv = container_of(chan_priv, struct lttng_kernel_channel_buffer_private, parent);
		chan_buf_priv->metadata_dumped = 0;
	}

	list_for_each_entry(event_recorder_priv, &session->priv->events_head, parent.parent.node) {
		event_recorder_priv->metadata_dumped = 0;
	}

	ret = _lttng_session_metadata_statedump(session);

end:
	mutex_unlock(&sessions_mutex);
	return ret;
}

static
bool is_channel_buffer_metadata(struct lttng_kernel_channel_common *channel)
{
	struct lttng_kernel_channel_buffer *chan_buf;

	if (channel->type != LTTNG_KERNEL_CHANNEL_TYPE_BUFFER)
		return false;
	chan_buf = container_of(channel, struct lttng_kernel_channel_buffer, parent);
	if (chan_buf->priv->channel_type == METADATA_CHANNEL)
		return true;
	return false;
}

int lttng_channel_enable(struct lttng_kernel_channel_common *channel)
{
	int ret = 0;

	mutex_lock(&sessions_mutex);
	if (is_channel_buffer_metadata(channel)) {
		ret = -EPERM;
		goto end;
	}
	if (channel->enabled) {
		ret = -EEXIST;
		goto end;
	}
	/* Set transient enabler state to "enabled" */
	channel->priv->tstate = 1;
	lttng_session_sync_event_enablers(channel->session);
	/* Set atomically the state to "enabled" */
	WRITE_ONCE(channel->enabled, 1);
end:
	mutex_unlock(&sessions_mutex);
	return ret;
}

int lttng_channel_disable(struct lttng_kernel_channel_common *channel)
{
	int ret = 0;

	mutex_lock(&sessions_mutex);
	if (is_channel_buffer_metadata(channel)) {
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
	channel->priv->tstate = 0;
	lttng_session_sync_event_enablers(channel->session);
end:
	mutex_unlock(&sessions_mutex);
	return ret;
}

int lttng_event_enable(struct lttng_kernel_event_common *event)
{
	int ret = 0;

	mutex_lock(&sessions_mutex);
	switch (event->type) {
	case LTTNG_KERNEL_EVENT_TYPE_RECORDER:
	{
		struct lttng_kernel_event_recorder *event_recorder =
			container_of(event, struct lttng_kernel_event_recorder, parent);

		if (event_recorder->chan->priv->channel_type == METADATA_CHANNEL) {
			ret = -EPERM;
			goto end;
		}
		break;
	}
	case LTTNG_KERNEL_EVENT_TYPE_NOTIFIER:
		lttng_fallthrough;
	case LTTNG_KERNEL_EVENT_TYPE_COUNTER:
		break;
	default:
		break;
	}

	if (event->enabled) {
		ret = -EEXIST;
		goto end;
	}
	switch (event->priv->instrumentation) {
	case LTTNG_KERNEL_ABI_TRACEPOINT:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_SYSCALL:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_KPROBE:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_KRETPROBE:
		ret = -EINVAL;
		break;

	case LTTNG_KERNEL_ABI_UPROBE:
		WRITE_ONCE(event->enabled, 1);
		break;

	case LTTNG_KERNEL_ABI_FUNCTION:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_NOOP:
		lttng_fallthrough;
	default:
		WARN_ON_ONCE(1);
		ret = -EINVAL;
	}
end:
	mutex_unlock(&sessions_mutex);
	return ret;
}

int lttng_event_disable(struct lttng_kernel_event_common *event)
{
	int ret = 0;

	mutex_lock(&sessions_mutex);
	switch (event->type) {
	case LTTNG_KERNEL_EVENT_TYPE_RECORDER:
	{
		struct lttng_kernel_event_recorder *event_recorder =
			container_of(event, struct lttng_kernel_event_recorder, parent);

		if (event_recorder->chan->priv->channel_type == METADATA_CHANNEL) {
			ret = -EPERM;
			goto end;
		}
		break;
	}
	case LTTNG_KERNEL_EVENT_TYPE_NOTIFIER:
		lttng_fallthrough;
	case LTTNG_KERNEL_EVENT_TYPE_COUNTER:
		break;
	default:
		break;
	}

	if (!event->enabled) {
		ret = -EEXIST;
		goto end;
	}
	switch (event->priv->instrumentation) {
	case LTTNG_KERNEL_ABI_TRACEPOINT:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_SYSCALL:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_KPROBE:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_KRETPROBE:
		ret = -EINVAL;
		break;

	case LTTNG_KERNEL_ABI_UPROBE:
		WRITE_ONCE(event->enabled, 0);
		break;

	case LTTNG_KERNEL_ABI_FUNCTION:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_NOOP:
		lttng_fallthrough;
	default:
		WARN_ON_ONCE(1);
		ret = -EINVAL;
	}
end:
	mutex_unlock(&sessions_mutex);
	return ret;
}

struct lttng_kernel_channel_buffer *lttng_channel_buffer_create(struct lttng_kernel_session *session,
				       const char *transport_name,
				       void *buf_addr,
				       size_t subbuf_size, size_t num_subbuf,
				       unsigned int switch_timer_interval,
				       unsigned int read_timer_interval,
				       enum channel_type channel_type)
{
	struct lttng_kernel_channel_buffer *chan;
	struct lttng_transport *transport = NULL;

	mutex_lock(&sessions_mutex);
	if (session->priv->been_active && channel_type != METADATA_CHANNEL)
		goto active;	/* Refuse to add channel to active session */
	transport = lttng_transport_find(transport_name);
	if (!transport) {
		printk(KERN_WARNING "LTTng: transport %s not found\n",
		       transport_name);
		goto notransport;
	}
	if (!try_module_get(transport->owner)) {
		printk(KERN_WARNING "LTTng: Can't lock transport module.\n");
		goto notransport;
	}
	chan = lttng_kernel_alloc_channel_buffer();
	if (!chan)
		goto nomem;
	chan->parent.session = session;
	chan->priv->id = session->priv->free_chan_id++;
	chan->ops = &transport->ops;
	/*
	 * Note: the channel creation op already writes into the packet
	 * headers. Therefore the "chan" information used as input
	 * should be already accessible.
	 */
	chan->priv->rb_chan = transport->ops.priv->channel_create(transport_name,
			chan, buf_addr, subbuf_size, num_subbuf,
			switch_timer_interval, read_timer_interval);
	if (!chan->priv->rb_chan)
		goto create_error;
	chan->priv->parent.tstate = 1;
	chan->parent.enabled = 1;
	chan->priv->transport = transport;
	chan->priv->channel_type = channel_type;
	list_add(&chan->priv->parent.node, &session->priv->chan_head);
	mutex_unlock(&sessions_mutex);
	return chan;

create_error:
	lttng_kernel_free_channel_common(&chan->parent);
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
void lttng_kernel_buffer_destroy(struct lttng_kernel_channel_buffer *chan)
{
	chan->ops->priv->channel_destroy(chan->priv->rb_chan);
	module_put(chan->priv->transport->owner);
	lttng_kernel_destroy_context(chan->priv->ctx);
	kfree(chan->priv);
	kfree(chan);
}

static
void _lttng_channel_destroy(struct lttng_kernel_channel_common *chan)
{
	list_del(&chan->priv->node);

	switch (chan->type) {
	case LTTNG_KERNEL_CHANNEL_TYPE_BUFFER:
	{
		struct lttng_kernel_channel_buffer *chan_buf =
			container_of(chan, struct lttng_kernel_channel_buffer, parent);
		lttng_kernel_buffer_destroy(chan_buf);
		break;
	}
	case LTTNG_KERNEL_CHANNEL_TYPE_COUNTER:
	{
		struct lttng_kernel_channel_counter *chan_counter =
			container_of(chan, struct lttng_kernel_channel_counter, parent);
		lttng_kernel_counter_destroy(chan_counter);
		break;
	}
	default:
		WARN_ON_ONCE(1);
	}
}

void lttng_metadata_channel_buffer_destroy(struct lttng_kernel_channel_buffer *chan)
{
	BUG_ON(chan->priv->channel_type != METADATA_CHANNEL);

	/* Protect the metadata cache with the sessions_mutex. */
	mutex_lock(&sessions_mutex);
	_lttng_channel_destroy(&chan->parent);
	mutex_unlock(&sessions_mutex);
}
EXPORT_SYMBOL_GPL(lttng_metadata_channel_buffer_destroy);

static
void _lttng_metadata_channel_hangup(struct lttng_metadata_stream *stream)
{
	stream->finalized = 1;
	wake_up_interruptible(&stream->read_wait);
}

static
bool lttng_kernel_event_id_available(struct lttng_event_enabler_common *event_enabler)
{
	struct lttng_kernel_abi_event *event_param = &event_enabler->event_param;
        enum lttng_kernel_abi_instrumentation itype = event_param->instrumentation;

	switch (event_enabler->enabler_type) {
	case LTTNG_EVENT_ENABLER_TYPE_RECORDER:
	{
		struct lttng_event_recorder_enabler *event_recorder_enabler =
			container_of(event_enabler, struct lttng_event_recorder_enabler, parent.parent);
		struct lttng_kernel_channel_buffer *chan = event_recorder_enabler->chan;

		switch (itype) {
		case LTTNG_KERNEL_ABI_TRACEPOINT:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_KPROBE:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_SYSCALL:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_UPROBE:
			if (chan->priv->free_event_id == -1U)
				return false;
			return true;
		case LTTNG_KERNEL_ABI_KRETPROBE:
			/* kretprobes require 2 event IDs. */
			if (chan->priv->free_event_id >= -2U)
				return false;
			return true;
		default:
			WARN_ON_ONCE(1);
			return false;
		}
	}
	case LTTNG_EVENT_ENABLER_TYPE_COUNTER:
	{
		struct lttng_event_counter_enabler *event_counter_enabler =
			container_of(event_enabler, struct lttng_event_counter_enabler, parent.parent);
		struct lttng_kernel_channel_counter *chan = event_counter_enabler->chan;
		size_t nr_dimensions, max_nr_elem;

		if (lttng_kernel_counter_get_nr_dimensions(chan, &nr_dimensions))
			return false;
		WARN_ON_ONCE(nr_dimensions != 1);
		if (nr_dimensions != 1)
			return false;
		if (lttng_kernel_counter_get_max_nr_elem(chan, &max_nr_elem))
			return false;
		switch (itype) {
		case LTTNG_KERNEL_ABI_TRACEPOINT:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_KPROBE:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_SYSCALL:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_UPROBE:
			if (chan->priv->free_index >= max_nr_elem)
				return false;
			return true;
		case LTTNG_KERNEL_ABI_KRETPROBE:
			/* kretprobes require 2 event IDs. */
			if (chan->priv->free_index + 1 >= max_nr_elem)
				return false;
			return true;
		default:
			WARN_ON_ONCE(1);
			return false;
		}
	}
	case LTTNG_EVENT_ENABLER_TYPE_NOTIFIER:
		return true;
	default:
		WARN_ON_ONCE(1);
		return false;
	}
}

static
struct lttng_kernel_event_common *lttng_kernel_event_alloc(struct lttng_event_enabler_common *event_enabler,
		struct hlist_head *key_head,
		const char *key_string)
{
	struct lttng_kernel_abi_event *event_param = &event_enabler->event_param;
        enum lttng_kernel_abi_instrumentation itype = event_param->instrumentation;

	switch (event_enabler->enabler_type) {
	case LTTNG_EVENT_ENABLER_TYPE_RECORDER:
	{
		struct lttng_event_recorder_enabler *event_recorder_enabler =
			container_of(event_enabler, struct lttng_event_recorder_enabler, parent.parent);
		struct lttng_kernel_event_recorder *event_recorder;
		struct lttng_kernel_event_recorder_private *event_recorder_priv;
		struct lttng_kernel_channel_buffer *chan = event_recorder_enabler->chan;

		WARN_ON_ONCE(key_head);	/* not implemented. */
		event_recorder = kmem_cache_zalloc(event_recorder_cache, GFP_KERNEL);
		if (!event_recorder)
			return NULL;
		event_recorder_priv = kmem_cache_zalloc(event_recorder_private_cache, GFP_KERNEL);
		if (!event_recorder_priv) {
			kmem_cache_free(event_recorder_private_cache, event_recorder);
			return NULL;
		}
		event_recorder_priv->pub = event_recorder;
		event_recorder_priv->parent.parent.pub = &event_recorder->parent;
		event_recorder->priv = event_recorder_priv;
		event_recorder->parent.priv = &event_recorder_priv->parent.parent;

		event_recorder->parent.type = LTTNG_KERNEL_EVENT_TYPE_RECORDER;
		event_recorder->parent.run_filter = lttng_kernel_interpret_event_filter;
		event_recorder->priv->parent.parent.instrumentation = itype;
		INIT_LIST_HEAD(&event_recorder->priv->parent.parent.filter_bytecode_runtime_head);
		INIT_LIST_HEAD(&event_recorder->priv->parent.parent.enablers_ref_head);

		event_recorder->chan = chan;
		event_recorder->priv->parent.chan = &chan->parent;
		event_recorder->priv->parent.id = chan->priv->free_event_id++;
		return &event_recorder->parent;
	}
	case LTTNG_EVENT_ENABLER_TYPE_NOTIFIER:
	{
		struct lttng_event_notifier_enabler *event_notifier_enabler =
			container_of(event_enabler, struct lttng_event_notifier_enabler, parent);
		struct lttng_kernel_event_notifier *event_notifier;
		struct lttng_kernel_event_notifier_private *event_notifier_priv;

		WARN_ON_ONCE(key_head);	/* not implemented. */
		event_notifier = kmem_cache_zalloc(event_notifier_cache, GFP_KERNEL);
		if (!event_notifier)
			return NULL;
		event_notifier_priv = kmem_cache_zalloc(event_notifier_private_cache, GFP_KERNEL);
		if (!event_notifier_priv) {
			kmem_cache_free(event_notifier_private_cache, event_notifier);
			return NULL;
		}
		event_notifier_priv->pub = event_notifier;
		event_notifier_priv->parent.pub = &event_notifier->parent;
		event_notifier->priv = event_notifier_priv;
		event_notifier->parent.priv = &event_notifier_priv->parent;

		event_notifier->parent.type = LTTNG_KERNEL_EVENT_TYPE_NOTIFIER;
		event_notifier->parent.run_filter = lttng_kernel_interpret_event_filter;
		event_notifier->priv->parent.instrumentation = itype;
		event_notifier->priv->parent.user_token = event_enabler->user_token;
		INIT_LIST_HEAD(&event_notifier->priv->parent.filter_bytecode_runtime_head);
		INIT_LIST_HEAD(&event_notifier->priv->parent.enablers_ref_head);

		event_notifier->priv->group = event_notifier_enabler->group;
		event_notifier->priv->error_counter_index = event_notifier_enabler->error_counter_index;
		event_notifier->priv->num_captures = 0;
		event_notifier->notification_send = lttng_event_notifier_notification_send;
		INIT_LIST_HEAD(&event_notifier->priv->capture_bytecode_runtime_head);
		return &event_notifier->parent;
	}
	case LTTNG_EVENT_ENABLER_TYPE_COUNTER:
	{
		struct lttng_event_counter_enabler *event_counter_enabler =
			container_of(event_enabler, struct lttng_event_counter_enabler, parent.parent);
		struct lttng_kernel_event_counter *event_counter;
		struct lttng_kernel_event_counter_private *event_counter_priv;
		struct lttng_kernel_channel_counter *chan = event_counter_enabler->chan;
		bool key_found = false;

		event_counter = kmem_cache_zalloc(event_counter_cache, GFP_KERNEL);
		if (!event_counter)
			return NULL;
		event_counter_priv = kmem_cache_zalloc(event_counter_private_cache, GFP_KERNEL);
		if (!event_counter_priv) {
			kmem_cache_free(event_counter_private_cache, event_counter);
			return NULL;
		}
		event_counter_priv->pub = event_counter;
		event_counter_priv->parent.parent.pub = &event_counter->parent;
		event_counter->priv = event_counter_priv;
		event_counter->parent.priv = &event_counter_priv->parent.parent;

		event_counter->parent.type = LTTNG_KERNEL_EVENT_TYPE_COUNTER;
		event_counter->parent.run_filter = lttng_kernel_interpret_event_filter;
		event_counter->priv->parent.parent.instrumentation = itype;
		INIT_LIST_HEAD(&event_counter->priv->parent.parent.filter_bytecode_runtime_head);
		INIT_LIST_HEAD(&event_counter->priv->parent.parent.enablers_ref_head);

		event_counter->chan = chan;
		event_counter->priv->parent.chan = &chan->parent;
		if (!chan->priv->parent.coalesce_hits)
			event_counter->priv->parent.parent.user_token = event_counter_enabler->parent.parent.user_token;
		strcpy(event_counter_priv->key, key_string);
		if (key_head) {
			struct lttng_kernel_event_counter_private *event_counter_priv_iter;

			lttng_hlist_for_each_entry(event_counter_priv_iter, key_head, hlist_key_node) {
				if (!strcmp(key_string, event_counter_priv_iter->key)) {
					/* Same key, use same id. */
					key_found = true;
					event_counter->priv->parent.id = event_counter_priv_iter->parent.id;
					break;
				}
			}
		}
		if (!key_found)
			event_counter->priv->parent.id = chan->priv->free_index++;
		return &event_counter->parent;
	}
	default:
		return NULL;
	}
}

static
void lttng_kernel_event_free(struct lttng_kernel_event_common *event)
{
	switch (event->type) {
	case LTTNG_KERNEL_EVENT_TYPE_RECORDER:
	{
		struct lttng_kernel_event_recorder *event_recorder =
			container_of(event, struct lttng_kernel_event_recorder, parent);

		kmem_cache_free(event_recorder_private_cache, event_recorder->priv);
		kmem_cache_free(event_recorder_cache, event_recorder);
		break;
	}
	case LTTNG_KERNEL_EVENT_TYPE_COUNTER:
	{
		struct lttng_kernel_event_counter *event_counter =
			container_of(event, struct lttng_kernel_event_counter, parent);

		kmem_cache_free(event_counter_private_cache, event_counter->priv);
		kmem_cache_free(event_counter_cache, event_counter);
		break;
	}
	case LTTNG_KERNEL_EVENT_TYPE_NOTIFIER:
	{
		struct lttng_kernel_event_notifier *event_notifier =
			container_of(event, struct lttng_kernel_event_notifier, parent);

		kmem_cache_free(event_notifier_private_cache, event_notifier->priv);
		kmem_cache_free(event_notifier_cache, event_notifier);
		break;
	}
	default:
		WARN_ON_ONCE(1);
	}
}

static
int lttng_kernel_event_notifier_clear_error_counter(struct lttng_kernel_event_common *event)
{
	switch (event->type) {
	case LTTNG_KERNEL_EVENT_TYPE_RECORDER:
		lttng_fallthrough;
	case LTTNG_KERNEL_EVENT_TYPE_COUNTER:
		return 0;

	case LTTNG_KERNEL_EVENT_TYPE_NOTIFIER:
	{
		struct lttng_kernel_event_notifier *event_notifier =
			container_of(event, struct lttng_kernel_event_notifier, parent);
		struct lttng_kernel_channel_counter *error_counter;
		struct lttng_event_notifier_group *event_notifier_group = event_notifier->priv->group;
		size_t dimension_index[1];
		int ret;

		/*
		 * Clear the error counter bucket. The sessiond keeps track of which
		 * bucket is currently in use. We trust it. The session lock
		 * synchronizes against concurrent creation of the error
		 * counter.
		 */
		error_counter = event_notifier_group->error_counter;
		if (!error_counter)
			return 0;
		/*
		 * Check that the index is within the boundary of the counter.
		 */
		if (event_notifier->priv->error_counter_index >= event_notifier_group->error_counter_len) {
			printk(KERN_INFO "LTTng: event_notifier: Error counter index out-of-bound: counter-len=%zu, index=%llu\n",
				event_notifier_group->error_counter_len, event_notifier->priv->error_counter_index);
			return -EINVAL;
		}

		dimension_index[0] = event_notifier->priv->error_counter_index;
		ret = error_counter->ops->priv->counter_clear(error_counter, dimension_index);
		if (ret) {
			printk(KERN_INFO "LTTng: event_notifier: Unable to clear error counter bucket %llu\n",
				event_notifier->priv->error_counter_index);
			return -EINVAL;
		}
		return 0;
	}
	default:
		return -EINVAL;
	}
}

int format_event_key(struct lttng_event_enabler_common *event_enabler, char *key_string,
		     const char *event_name)
{
	struct lttng_event_counter_enabler *event_counter_enabler;
	const struct lttng_kernel_counter_key_dimension *dim;
	size_t i, left = LTTNG_KERNEL_COUNTER_KEY_LEN;
	const struct lttng_kernel_counter_key *key;

	if (event_enabler->enabler_type != LTTNG_EVENT_ENABLER_TYPE_COUNTER)
		return 0;
	event_counter_enabler = container_of(event_enabler, struct lttng_event_counter_enabler, parent.parent);
	key = event_counter_enabler->key;
	if (!key->nr_dimensions)
		return 0;
	/* Currently event keys can only be specified on a single dimension. */
	if (key->nr_dimensions != 1)
		return -EINVAL;
	dim = &key->dimension_array[0];
	for (i = 0; i < dim->nr_key_tokens; i++) {
		const struct lttng_key_token *token = &dim->token_array[i];
		size_t token_len;
		const char *str;

		switch (token->type) {
		case LTTNG_KEY_TOKEN_STRING:
			str = token->str;
			break;
		case LTTNG_KEY_TOKEN_EVENT_NAME:
			str = event_name;
			break;
		default:
			return -EINVAL;
		}
		token_len = strlen(str);
		if (token_len >= left)
			return -EINVAL;
		strcat(key_string, str);
		left -= token_len;
	}
	return 0;
}

static
bool match_event_key(struct lttng_kernel_event_common *event, const char *key_string)
{
	switch (event->type) {
	case LTTNG_KERNEL_EVENT_TYPE_RECORDER:
		lttng_fallthrough;
	case LTTNG_KERNEL_EVENT_TYPE_NOTIFIER:
		return true;

	case LTTNG_KERNEL_EVENT_TYPE_COUNTER:
	{
		struct lttng_kernel_event_counter_private *event_counter_priv =
			container_of(event->priv, struct lttng_kernel_event_counter_private, parent.parent);

		if (key_string[0] == '\0')
			return true;
		return !strcmp(key_string, event_counter_priv->key);
	}

	default:
		WARN_ON_ONCE(1);
		return false;
	}
}

static
bool match_event_session_token(struct lttng_kernel_event_session_common_private *event_session_priv,
		uint64_t token)
{
	if (event_session_priv->chan->priv->coalesce_hits)
		return true;
	if (event_session_priv->parent.user_token == token)
		return true;
	return false;
}

bool lttng_event_enabler_event_name_key_match_event(struct lttng_event_enabler_common *event_enabler,
		const char *event_name, const char *key_string, struct lttng_kernel_event_common *event)
{
	switch (event_enabler->enabler_type) {
	case LTTNG_EVENT_ENABLER_TYPE_RECORDER:
		lttng_fallthrough;
	case LTTNG_EVENT_ENABLER_TYPE_COUNTER:
	{
		struct lttng_event_enabler_session_common *event_session_enabler =
			container_of(event_enabler, struct lttng_event_enabler_session_common, parent);
		struct lttng_kernel_event_session_common_private *event_session_priv =
			container_of(event->priv, struct lttng_kernel_event_session_common_private, parent);
		bool same_event = false, same_channel = false, same_key = false,
				same_token = false;

		WARN_ON_ONCE(!event->priv->desc);
		if (!strncmp(event->priv->desc->event_name, event_name, LTTNG_KERNEL_ABI_SYM_NAME_LEN - 1))
			same_event = true;
		if (event_session_enabler->chan == event_session_priv->chan) {
			same_channel = true;
			if (match_event_session_token(event_session_priv, event_enabler->user_token))
				same_token = true;
		}
		if (match_event_key(event, key_string))
			same_key = true;
		return same_event && same_channel && same_key && same_token;
	}

	case LTTNG_EVENT_ENABLER_TYPE_NOTIFIER:
	{
		/*
		 * Check if event_notifier already exists by checking
		 * if the event_notifier and enabler share the same
		 * description and id.
		 */
		if (!strncmp(event->priv->desc->event_name, event_name, LTTNG_KERNEL_ABI_SYM_NAME_LEN - 1)
				&& event->priv->user_token == event_enabler->user_token)
			return true;
		else
			return false;
	}
	default:
		WARN_ON_ONCE(1);
		return false;
	}
}

static
int lttng_counter_append_descriptor(struct lttng_kernel_channel_counter *counter,
		uint64_t user_token,
		size_t index,
		const char *key)
{
	struct lttng_counter_map *map = &counter->priv->map;
	struct lttng_counter_map_descriptor *last;
	int ret = 0;

	if (strlen(key) >= LTTNG_KERNEL_COUNTER_KEY_LEN) {
		WARN_ON_ONCE(1);
		return -EOVERFLOW;
	}
	mutex_lock(&map->lock);
	if (map->nr_descriptors == map->alloc_len) {
		struct lttng_counter_map_descriptor *new_table, *old_table;
		size_t old_len = map->nr_descriptors;
		size_t new_len = max_t(size_t, old_len + 1, map->alloc_len * 2);

		old_table = map->descriptors;
		new_table = lttng_kvzalloc(sizeof(struct lttng_counter_map_descriptor) * new_len,
				GFP_KERNEL);
		if (!new_table) {
			ret = -ENOMEM;
			goto unlock;
		}

		if (old_table)
			memcpy(new_table, old_table, old_len * sizeof(struct lttng_counter_map_descriptor));

		map->descriptors = new_table;
		map->alloc_len = new_len;
		lttng_kvfree(old_table);
	}
	last = &map->descriptors[map->nr_descriptors++];
	last->user_token = user_token;
	last->array_index = index;
	strcpy(last->key, key);
unlock:
	mutex_unlock(&map->lock);
	return ret;
}

static
int lttng_append_event_to_channel_map(struct lttng_event_enabler_common *event_enabler,
		struct lttng_kernel_event_common *event,
		const char *event_name)
{
	struct lttng_event_counter_enabler *event_counter_enabler;
	struct lttng_kernel_channel_counter *chan_counter;
	struct lttng_kernel_event_counter *event_counter;
	const char *name = "<UNKNOWN>";

	if (event_enabler->enabler_type != LTTNG_EVENT_ENABLER_TYPE_COUNTER)
		return 0;
	event_counter_enabler = container_of(event_enabler, struct lttng_event_counter_enabler, parent.parent);
	event_counter = container_of(event, struct lttng_kernel_event_counter, parent);
	chan_counter = event_counter_enabler->chan;
	if (event_counter->priv->key[0])
		name = event_counter->priv->key;
	else
		name = event_name;
	return lttng_counter_append_descriptor(chan_counter, event_enabler->user_token,
			event_counter->priv->parent.id, name);
}

/*
 * Supports event creation while tracing session is active.
 * Needs to be called with sessions mutex held.
 */
struct lttng_kernel_event_common *_lttng_kernel_event_create(struct lttng_event_enabler_common *event_enabler,
				const struct lttng_kernel_event_desc *event_desc,
				struct lttng_kernel_event_pair *event_pair)
{
	char key_string[LTTNG_KERNEL_COUNTER_KEY_LEN] = { 0 };
	struct lttng_event_ht *events_name_ht = lttng_get_events_name_ht_from_enabler(event_enabler);
	struct lttng_event_ht *events_key_ht = lttng_get_events_key_ht_from_enabler(event_enabler);
	struct list_head *event_list_head = lttng_get_event_list_head_from_enabler(event_enabler);
	struct lttng_kernel_abi_event *event_param = &event_enabler->event_param;
	enum lttng_kernel_abi_instrumentation itype = event_param->instrumentation;
	struct lttng_kernel_event_common_private *event_priv_iter;
	struct lttng_kernel_event_common *event;
	struct hlist_head *name_head, *key_head = NULL;
	const char *event_name;
	int ret;

	if (event_pair == NULL || event_pair->check_ids) {
		if (!lttng_kernel_event_id_available(event_enabler)) {
			ret = -EMFILE;
			goto full;
		}
	}

	switch (itype) {
	case LTTNG_KERNEL_ABI_TRACEPOINT:
		event_name = event_desc->event_name;
		break;

	case LTTNG_KERNEL_ABI_KPROBE:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_UPROBE:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_SYSCALL:
		event_name = event_param->name;
		break;

	case LTTNG_KERNEL_ABI_KRETPROBE:
		event_name = event_pair->name;
		break;

	case LTTNG_KERNEL_ABI_FUNCTION:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_NOOP:
		lttng_fallthrough;
	default:
		WARN_ON_ONCE(1);
		ret = -EINVAL;
		goto type_error;
	}

	if (format_event_key(event_enabler, key_string, event_name)) {
		ret = -EINVAL;
		goto type_error;
	}

	name_head = utils_borrow_hash_table_bucket(events_name_ht->table, LTTNG_EVENT_HT_SIZE, event_name);
	lttng_hlist_for_each_entry(event_priv_iter, name_head, hlist_name_node) {
		if (lttng_event_enabler_event_name_key_match_event(event_enabler,
				event_name, key_string, event_priv_iter->pub)) {
			ret = -EEXIST;
			goto exist;
		}
	}

	if (key_string[0] != '\0')
		key_head = utils_borrow_hash_table_bucket(events_key_ht->table, LTTNG_EVENT_HT_SIZE, key_string);

	event = lttng_kernel_event_alloc(event_enabler, key_head, key_string);
	if (!event) {
		ret = -ENOMEM;
		goto alloc_error;
	}

	switch (itype) {
	case LTTNG_KERNEL_ABI_TRACEPOINT:
		/* Event will be enabled by enabler sync. */
		event->enabled = 0;
		event->priv->registered = 0;
		event->priv->desc = lttng_event_desc_get(event_name);
		if (!event->priv->desc) {
			ret = -ENOENT;
			goto register_error;
		}
		/* Populate lttng_event structure before event registration. */
		smp_wmb();
		break;

	case LTTNG_KERNEL_ABI_KPROBE:
		/* Event will be enabled by enabler sync. */
		event->enabled = 0;
		event->priv->registered = 0;
		ret = lttng_kprobes_init_event(event_name,
				event_param->u.kprobe.symbol_name,
				event_param->u.kprobe.offset,
				event_param->u.kprobe.addr,
				event);
		if (ret) {
			ret = -EINVAL;
			goto register_error;
		}
		/*
		 * Populate lttng_event structure before event
		 * registration.
		 */
		smp_wmb();
		ret = try_module_get(event->priv->desc->owner);
		WARN_ON_ONCE(!ret);
		break;

	case LTTNG_KERNEL_ABI_KRETPROBE:
	{
		/* Event will be enabled by enabler sync. */
		event->enabled = 0;
		event->priv->registered = 0;
		ret = lttng_kretprobes_init_event(event_name,
				event_pair->entryexit,
				event, event_pair->krp);
		if (ret) {
			ret = -EINVAL;
			goto register_error;
		}
		ret = try_module_get(event->priv->desc->owner);
		WARN_ON_ONCE(!ret);
		break;
	}

	case LTTNG_KERNEL_ABI_SYSCALL:
		/* Event will be enabled by enabler sync. */
		event->enabled = 0;
		event->priv->registered = 0;
		event->priv->desc = event_desc;
		switch (event_param->u.syscall.entryexit) {
		case LTTNG_KERNEL_ABI_SYSCALL_ENTRYEXIT:
			ret = -EINVAL;
			goto register_error;
		case LTTNG_KERNEL_ABI_SYSCALL_ENTRY:
			event->priv->u.syscall.entryexit = LTTNG_SYSCALL_ENTRY;
			break;
		case LTTNG_KERNEL_ABI_SYSCALL_EXIT:
			event->priv->u.syscall.entryexit = LTTNG_SYSCALL_EXIT;
			break;
		}
		switch (event_param->u.syscall.abi) {
		case LTTNG_KERNEL_ABI_SYSCALL_ABI_ALL:
			ret = -EINVAL;
			goto register_error;
		case LTTNG_KERNEL_ABI_SYSCALL_ABI_NATIVE:
			event->priv->u.syscall.abi = LTTNG_SYSCALL_ABI_NATIVE;
			break;
		case LTTNG_KERNEL_ABI_SYSCALL_ABI_COMPAT:
			event->priv->u.syscall.abi = LTTNG_SYSCALL_ABI_COMPAT;
			break;
		}
		if (!event->priv->desc) {
			ret = -EINVAL;
			goto register_error;
		}
		break;

	case LTTNG_KERNEL_ABI_UPROBE:
		/*
		 * Needs to be explicitly enabled after creation, since
		 * we may want to apply filters.
		 */
		event->enabled = 0;
		event->priv->registered = 1;

		/*
		 * Populate lttng_event structure before event
		 * registration.
		 */
		smp_wmb();

		ret = lttng_uprobes_register_event(event_param->name,
				event_param->u.uprobe.fd,
				event);
		if (ret)
			goto register_error;
		ret = try_module_get(event->priv->desc->owner);
		WARN_ON_ONCE(!ret);
		ret = lttng_append_event_to_channel_map(event_enabler, event, event_name);
		WARN_ON_ONCE(ret);
		break;

	default:
		WARN_ON_ONCE(1);
		ret = -EINVAL;
		goto register_error;
	}

	ret = _lttng_event_recorder_metadata_statedump(event);
	WARN_ON_ONCE(ret > 0);
	if (ret) {
		goto statedump_error;
	}

	ret = lttng_kernel_event_notifier_clear_error_counter(event);
	if (ret) {
		goto register_error;
	}

	hlist_add_head(&event->priv->hlist_name_node, name_head);
	if (key_head) {
		struct lttng_kernel_event_counter_private *event_counter_priv =
			container_of(event->priv, struct lttng_kernel_event_counter_private, parent.parent);
		hlist_add_head(&event_counter_priv->hlist_key_node, key_head);
	}
	list_add(&event->priv->node, event_list_head);

	return event;

statedump_error:
	/* If a statedump error occurs, events will not be readable. */
register_error:
	lttng_kernel_event_free(event);
alloc_error:
exist:
type_error:
full:
	return ERR_PTR(ret);
}

struct lttng_kernel_event_common *lttng_kernel_event_create(struct lttng_event_enabler_common *event_enabler,
				const struct lttng_kernel_event_desc *event_desc,
				struct lttng_kernel_event_pair *event_pair)
{
	struct lttng_kernel_event_common *event;

	mutex_lock(&sessions_mutex);
	event = _lttng_kernel_event_create(event_enabler, event_desc, event_pair);
	mutex_unlock(&sessions_mutex);
	return event;
}

int lttng_kernel_counter_read(struct lttng_kernel_channel_counter *counter,
		const size_t *dim_indexes, int32_t cpu,
		int64_t *val, bool *overflow, bool *underflow)
{
	return counter->ops->priv->counter_read(counter, dim_indexes,
			cpu, val, overflow, underflow);
}

int lttng_kernel_counter_aggregate(struct lttng_kernel_channel_counter *counter,
		const size_t *dim_indexes, int64_t *val,
		bool *overflow, bool *underflow)
{
	return counter->ops->priv->counter_aggregate(counter, dim_indexes,
			val, overflow, underflow);
}

int lttng_kernel_counter_clear(struct lttng_kernel_channel_counter *counter,
		const size_t *dim_indexes)
{
	return counter->ops->priv->counter_clear(counter, dim_indexes);
}

int lttng_kernel_counter_get_nr_dimensions(struct lttng_kernel_channel_counter *counter,
		size_t *nr_dimensions)
{
	return counter->ops->priv->counter_get_nr_dimensions(counter, nr_dimensions);
}

int lttng_kernel_counter_get_max_nr_elem(struct lttng_kernel_channel_counter *counter,
		size_t *max_nr_elem)
{
	return counter->ops->priv->counter_get_max_nr_elem(counter, max_nr_elem);
}

/* Used for tracepoints, system calls, and kprobe. */
static
void register_event(struct lttng_kernel_event_common *event)
{
	const struct lttng_kernel_event_desc *desc;
	int ret = -EINVAL;

	WARN_ON_ONCE(event->priv->registered);

	desc = event->priv->desc;
	switch (event->priv->instrumentation) {
	case LTTNG_KERNEL_ABI_TRACEPOINT:
		ret = lttng_tracepoint_probe_register(desc->event_kname,
						  desc->tp_class->probe_callback,
						  event);
		break;

	case LTTNG_KERNEL_ABI_SYSCALL:
		ret = lttng_syscall_filter_enable_event(event);
		break;

	case LTTNG_KERNEL_ABI_KPROBE:
		ret = lttng_kprobes_register_event(event);
		break;

	case LTTNG_KERNEL_ABI_UPROBE:
		ret = 0;
		break;

	case LTTNG_KERNEL_ABI_KRETPROBE:
		ret = lttng_kretprobes_register_event(event);
		break;

	case LTTNG_KERNEL_ABI_FUNCTION:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_NOOP:
		lttng_fallthrough;
	default:
		WARN_ON_ONCE(1);
	}
	WARN_ON_ONCE(ret);
	if (!ret)
		event->priv->registered = 1;
}

static
void unregister_event(struct lttng_kernel_event_common *event)
{
	struct lttng_kernel_event_common_private *event_priv = event->priv;
	const struct lttng_kernel_event_desc *desc;
	int ret = -EINVAL;

	WARN_ON_ONCE(!event->priv->registered);

	desc = event_priv->desc;
	switch (event_priv->instrumentation) {
	case LTTNG_KERNEL_ABI_TRACEPOINT:
		ret = lttng_tracepoint_probe_unregister(event_priv->desc->event_kname,
						  event_priv->desc->tp_class->probe_callback,
						  event);
		break;

	case LTTNG_KERNEL_ABI_KPROBE:
		lttng_kprobes_unregister_event(event);
		ret = 0;
		break;

	case LTTNG_KERNEL_ABI_KRETPROBE:
		lttng_kretprobes_unregister_event(event);
		ret = 0;
		break;

	case LTTNG_KERNEL_ABI_SYSCALL:
		ret = lttng_syscall_filter_disable_event(event);
		break;

	case LTTNG_KERNEL_ABI_NOOP:
		switch (event->type) {
		case LTTNG_KERNEL_EVENT_TYPE_RECORDER:
			lttng_fallthrough;
		case LTTNG_KERNEL_EVENT_TYPE_COUNTER:
			ret = 0;
			break;
		case LTTNG_KERNEL_EVENT_TYPE_NOTIFIER:
			WARN_ON_ONCE(1);
			break;
		}
		break;

	case LTTNG_KERNEL_ABI_UPROBE:
		lttng_uprobes_unregister_event(event);
		ret = 0;
		break;

	case LTTNG_KERNEL_ABI_FUNCTION:
		lttng_fallthrough;
	default:
		WARN_ON_ONCE(1);
	}
	WARN_ON_ONCE(ret);
	if (!ret)
		event_priv->registered = 0;
}

static
void _lttng_event_unregister(struct lttng_kernel_event_common *event)
{
	if (event->priv->registered)
		unregister_event(event);
}

/*
 * Only used internally at session destruction.
 */
static
void _lttng_event_destroy(struct lttng_kernel_event_common *event)
{
	struct lttng_enabler_ref *enabler_ref, *tmp_enabler_ref;

	lttng_free_event_filter_runtime(event);
	/* Free event enabler refs */
	list_for_each_entry_safe(enabler_ref, tmp_enabler_ref,
				 &event->priv->enablers_ref_head, node)
		kfree(enabler_ref);

	/* Remove from event list. */
	list_del(&event->priv->node);
	/* Remove from event hash table. */
	hlist_del(&event->priv->hlist_name_node);

	switch (event->type) {
	case LTTNG_KERNEL_EVENT_TYPE_COUNTER:
	{
		struct lttng_kernel_event_counter_private *event_counter_priv =
			container_of(event->priv, struct lttng_kernel_event_counter_private, parent.parent);
		if (event_counter_priv->key[0] != '\0')
			hlist_del(&event_counter_priv->hlist_key_node);
		break;
	}
	case LTTNG_KERNEL_EVENT_TYPE_RECORDER:
		lttng_fallthrough;
	case LTTNG_KERNEL_EVENT_TYPE_NOTIFIER:
		break;
	default:
		WARN_ON_ONCE(1);
	}

	switch (event->priv->instrumentation) {
	case LTTNG_KERNEL_ABI_TRACEPOINT:
		lttng_event_desc_put(event->priv->desc);
		break;

	case LTTNG_KERNEL_ABI_KPROBE:
		module_put(event->priv->desc->owner);
		lttng_kprobes_destroy_event_private(event);
		break;

	case LTTNG_KERNEL_ABI_KRETPROBE:
		module_put(event->priv->desc->owner);
		lttng_kretprobes_destroy_event_private(event);
		break;

	case LTTNG_KERNEL_ABI_SYSCALL:
		break;

	case LTTNG_KERNEL_ABI_UPROBE:
		module_put(event->priv->desc->owner);
		lttng_uprobes_destroy_event_private(event);
		break;

	case LTTNG_KERNEL_ABI_FUNCTION:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_NOOP:
		lttng_fallthrough;
	default:
		WARN_ON_ONCE(1);
	}

	lttng_kernel_event_free(event);
}

static
struct lttng_kernel_id_tracker *get_tracker(struct lttng_kernel_session *session,
		enum tracker_type tracker_type)
{
	switch (tracker_type) {
	case TRACKER_PID:
		return &session->pid_tracker;
	case TRACKER_VPID:
		return &session->vpid_tracker;
	case TRACKER_UID:
		return &session->uid_tracker;
	case TRACKER_VUID:
		return &session->vuid_tracker;
	case TRACKER_GID:
		return &session->gid_tracker;
	case TRACKER_VGID:
		return &session->vgid_tracker;
	default:
		WARN_ON_ONCE(1);
		return NULL;
	}
}

int lttng_session_track_id(struct lttng_kernel_session *session,
		enum tracker_type tracker_type, int id)
{
	struct lttng_kernel_id_tracker *tracker;
	int ret;

	tracker = get_tracker(session, tracker_type);
	if (!tracker)
		return -EINVAL;
	if (id < -1)
		return -EINVAL;
	mutex_lock(&sessions_mutex);
	if (id == -1) {
		/* track all ids: destroy tracker. */
		lttng_id_tracker_destroy(tracker, true);
		ret = 0;
	} else {
		ret = lttng_id_tracker_add(tracker, id);
	}
	mutex_unlock(&sessions_mutex);
	return ret;
}

int lttng_session_untrack_id(struct lttng_kernel_session *session,
		enum tracker_type tracker_type, int id)
{
	struct lttng_kernel_id_tracker *tracker;
	int ret;

	tracker = get_tracker(session, tracker_type);
	if (!tracker)
		return -EINVAL;
	if (id < -1)
		return -EINVAL;
	mutex_lock(&sessions_mutex);
	if (id == -1) {
		/* untrack all ids: replace by empty tracker. */
		ret = lttng_id_tracker_empty_set(tracker);
	} else {
		ret = lttng_id_tracker_del(tracker, id);
	}
	mutex_unlock(&sessions_mutex);
	return ret;
}

static
void *id_list_start(struct seq_file *m, loff_t *pos)
{
	struct lttng_kernel_id_tracker *id_tracker = m->private;
	struct lttng_kernel_id_tracker_rcu *id_tracker_p = id_tracker->p;
	struct lttng_id_hash_node *e;
	int iter = 0, i;

	mutex_lock(&sessions_mutex);
	if (id_tracker_p) {
		for (i = 0; i < LTTNG_ID_TABLE_SIZE; i++) {
			struct hlist_head *head = &id_tracker_p->id_hash[i];

			lttng_hlist_for_each_entry(e, head, hlist) {
				if (iter++ >= *pos)
					return e;
			}
		}
	} else {
		/* ID tracker disabled. */
		if (iter >= *pos && iter == 0) {
			return id_tracker_p;	/* empty tracker */
		}
		iter++;
	}
	/* End of list */
	return NULL;
}

/* Called with sessions_mutex held. */
static
void *id_list_next(struct seq_file *m, void *p, loff_t *ppos)
{
	struct lttng_kernel_id_tracker *id_tracker = m->private;
	struct lttng_kernel_id_tracker_rcu *id_tracker_p = id_tracker->p;
	struct lttng_id_hash_node *e;
	int iter = 0, i;

	(*ppos)++;
	if (id_tracker_p) {
		for (i = 0; i < LTTNG_ID_TABLE_SIZE; i++) {
			struct hlist_head *head = &id_tracker_p->id_hash[i];

			lttng_hlist_for_each_entry(e, head, hlist) {
				if (iter++ >= *ppos)
					return e;
			}
		}
	} else {
		/* ID tracker disabled. */
		if (iter >= *ppos && iter == 0)
			return p;	/* empty tracker */
		iter++;
	}

	/* End of list */
	return NULL;
}

static
void id_list_stop(struct seq_file *m, void *p)
{
	mutex_unlock(&sessions_mutex);
}

static
int id_list_show(struct seq_file *m, void *p)
{
	struct lttng_kernel_id_tracker *id_tracker = m->private;
	struct lttng_kernel_id_tracker_rcu *id_tracker_p = id_tracker->p;
	int id;

	if (p == id_tracker_p) {
		/* Tracker disabled. */
		id = -1;
	} else {
		const struct lttng_id_hash_node *e = p;

		id = lttng_id_tracker_get_node_id(e);
	}
	switch (id_tracker->priv->tracker_type) {
	case TRACKER_PID:
		seq_printf(m,	"process { pid = %d; };\n", id);
		break;
	case TRACKER_VPID:
		seq_printf(m,	"process { vpid = %d; };\n", id);
		break;
	case TRACKER_UID:
		seq_printf(m,	"user { uid = %d; };\n", id);
		break;
	case TRACKER_VUID:
		seq_printf(m,	"user { vuid = %d; };\n", id);
		break;
	case TRACKER_GID:
		seq_printf(m,	"group { gid = %d; };\n", id);
		break;
	case TRACKER_VGID:
		seq_printf(m,	"group { vgid = %d; };\n", id);
		break;
	default:
		seq_printf(m,	"UNKNOWN { field = %d };\n", id);
	}
	return 0;
}

static
const struct seq_operations lttng_tracker_ids_list_seq_ops = {
	.start = id_list_start,
	.next = id_list_next,
	.stop = id_list_stop,
	.show = id_list_show,
};

static
int lttng_tracker_ids_list_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &lttng_tracker_ids_list_seq_ops);
}

static
int lttng_tracker_ids_list_release(struct inode *inode, struct file *file)
{
	struct seq_file *m = file->private_data;
	struct lttng_kernel_id_tracker *id_tracker = m->private;
	int ret;

	WARN_ON_ONCE(!id_tracker);
	ret = seq_release(inode, file);
	if (!ret)
		fput(id_tracker->priv->session->priv->file);
	return ret;
}

const struct file_operations lttng_tracker_ids_list_fops = {
	.owner = THIS_MODULE,
	.open = lttng_tracker_ids_list_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = lttng_tracker_ids_list_release,
};

int lttng_session_list_tracker_ids(struct lttng_kernel_session *session,
		enum tracker_type tracker_type)
{
	struct file *tracker_ids_list_file;
	struct seq_file *m;
	int file_fd, ret;

	file_fd = get_unused_fd_flags(0);
	if (file_fd < 0) {
		ret = file_fd;
		goto fd_error;
	}

	tracker_ids_list_file = anon_inode_getfile("[lttng_tracker_ids_list]",
					  &lttng_tracker_ids_list_fops,
					  NULL, O_RDWR);
	if (IS_ERR(tracker_ids_list_file)) {
		ret = PTR_ERR(tracker_ids_list_file);
		goto file_error;
	}
	if (!atomic_long_add_unless(&session->priv->file->f_count, 1, LONG_MAX)) {
		ret = -EOVERFLOW;
		goto refcount_error;
	}
	ret = lttng_tracker_ids_list_fops.open(NULL, tracker_ids_list_file);
	if (ret < 0)
		goto open_error;
	m = tracker_ids_list_file->private_data;

	m->private = get_tracker(session, tracker_type);
	BUG_ON(!m->private);
	fd_install(file_fd, tracker_ids_list_file);

	return file_fd;

open_error:
	atomic_long_dec(&session->priv->file->f_count);
refcount_error:
	fput(tracker_ids_list_file);
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
int lttng_desc_match_enabler_check(const struct lttng_kernel_event_desc *desc,
		struct lttng_event_enabler_common *enabler)
{
	const char *desc_name, *enabler_name;
	bool compat = false, entry = false;

	enabler_name = enabler->event_param.name;
	switch (enabler->event_param.instrumentation) {
	case LTTNG_KERNEL_ABI_TRACEPOINT:
		desc_name = desc->event_name;
		switch (enabler->format_type) {
		case LTTNG_ENABLER_FORMAT_STAR_GLOB:
			return lttng_match_enabler_star_glob(desc_name, enabler_name);
		case LTTNG_ENABLER_FORMAT_NAME:
			return lttng_match_enabler_name(desc_name, enabler_name);
		default:
			return -EINVAL;
		}
		break;

	case LTTNG_KERNEL_ABI_SYSCALL:
		desc_name = desc->event_name;
		if (!strncmp(desc_name, "compat_", strlen("compat_"))) {
			desc_name += strlen("compat_");
			compat = true;
		}
		if (!strncmp(desc_name, "syscall_exit_",
				strlen("syscall_exit_"))) {
			desc_name += strlen("syscall_exit_");
		} else if (!strncmp(desc_name, "syscall_entry_",
				strlen("syscall_entry_"))) {
			desc_name += strlen("syscall_entry_");
			entry = true;
		} else {
			WARN_ON_ONCE(1);
			return -EINVAL;
		}
		switch (enabler->event_param.u.syscall.entryexit) {
		case LTTNG_KERNEL_ABI_SYSCALL_ENTRYEXIT:
			break;
		case LTTNG_KERNEL_ABI_SYSCALL_ENTRY:
			if (!entry)
				return 0;
			break;
		case LTTNG_KERNEL_ABI_SYSCALL_EXIT:
			if (entry)
				return 0;
			break;
		default:
			return -EINVAL;
		}
		switch (enabler->event_param.u.syscall.abi) {
		case LTTNG_KERNEL_ABI_SYSCALL_ABI_ALL:
			break;
		case LTTNG_KERNEL_ABI_SYSCALL_ABI_NATIVE:
			if (compat)
				return 0;
			break;
		case LTTNG_KERNEL_ABI_SYSCALL_ABI_COMPAT:
			if (!compat)
				return 0;
			break;
		default:
			return -EINVAL;
		}
		switch (enabler->event_param.u.syscall.match) {
		case LTTNG_KERNEL_ABI_SYSCALL_MATCH_NAME:
			switch (enabler->format_type) {
			case LTTNG_ENABLER_FORMAT_STAR_GLOB:
				return lttng_match_enabler_star_glob(desc_name, enabler_name);
			case LTTNG_ENABLER_FORMAT_NAME:
				return lttng_match_enabler_name(desc_name, enabler_name);
			default:
				return -EINVAL;
			}
			break;
		case LTTNG_KERNEL_ABI_SYSCALL_MATCH_NR:
			return -EINVAL;	/* Not implemented. */
		default:
			return -EINVAL;
		}
		break;

	case LTTNG_KERNEL_ABI_KPROBE:
		desc_name = desc->event_name;
		switch (enabler->format_type) {
		case LTTNG_ENABLER_FORMAT_STAR_GLOB:
			return -EINVAL;
		case LTTNG_ENABLER_FORMAT_NAME:
			return lttng_match_enabler_name(desc_name, enabler_name);
		default:
			return -EINVAL;
		}
		break;

	case LTTNG_KERNEL_ABI_KRETPROBE:
	{
		char base_name[LTTNG_KERNEL_ABI_SYM_NAME_LEN];
		size_t base_name_len;	/* includes \0 */
		char *last_separator, *entryexit;

		desc_name = desc->event_name;
		last_separator = strrchr(desc_name, '_');
		base_name_len = last_separator - desc_name + 1;
		memcpy(base_name, desc_name, base_name_len);
		base_name[base_name_len - 1] = '\0';	/* Replace '_' by '\0' */
		entryexit = last_separator + 1;

		if (!strcmp(entryexit, "entry")) {
			entry = true;
		} else if (!strcmp(entryexit, "exit")) {
			/* Nothing to do. */
		} else {
			WARN_ON_ONCE(1);
			return -EINVAL;
		}

		switch (enabler->event_param.u.kretprobe.entryexit) {
		case LTTNG_KERNEL_ABI_KRETPROBE_ENTRYEXIT:
			break;
		case LTTNG_KERNEL_ABI_KRETPROBE_ENTRY:
			if (!entry)
				return 0;
			break;
		case LTTNG_KERNEL_ABI_KRETPROBE_EXIT:
			if (entry)
				return 0;
			break;
		default:
			return -EINVAL;
		}

		switch (enabler->format_type) {
		case LTTNG_ENABLER_FORMAT_STAR_GLOB:
			return -EINVAL;
		case LTTNG_ENABLER_FORMAT_NAME:
			return lttng_match_enabler_name(base_name, enabler_name);
		default:
			return -EINVAL;
		}
		break;
	}

	default:
		WARN_ON_ONCE(1);
		return -EINVAL;
	}
}

bool lttng_desc_match_enabler(const struct lttng_kernel_event_desc *desc,
		struct lttng_event_enabler_common *enabler)
{
	int ret;

	ret = lttng_desc_match_enabler_check(desc, enabler);
	if (ret < 0) {
		WARN_ON_ONCE(1);
		return false;
	}
	return ret;
}

static
bool lttng_event_session_enabler_match_event_session(struct lttng_event_enabler_session_common *event_enabler_session,
		struct lttng_kernel_event_session_common_private *event_session_priv)
{
	if (lttng_desc_match_enabler(event_session_priv->parent.desc, &event_enabler_session->parent)
			&& event_session_priv->chan == event_enabler_session->chan
			&& match_event_session_token(event_session_priv, event_enabler_session->parent.user_token))
		return true;
	else
		return false;
}

static
int lttng_event_notifier_enabler_match_event_notifier(
		struct lttng_event_notifier_enabler *event_notifier_enabler,
		struct lttng_kernel_event_notifier_private *event_notifier_priv)
{
	int desc_matches = lttng_desc_match_enabler(event_notifier_priv->parent.desc,
		lttng_event_notifier_enabler_as_enabler(event_notifier_enabler));

	if (desc_matches && event_notifier_priv->group == event_notifier_enabler->group &&
			event_notifier_priv->parent.user_token == event_notifier_enabler->parent.user_token)
		return 1;
	else
		return 0;
}

bool lttng_event_enabler_match_event(struct lttng_event_enabler_common *event_enabler,
		struct lttng_kernel_event_common *event)
{
	if (event_enabler->event_param.instrumentation != event->priv->instrumentation)
		return false;

	switch (event_enabler->enabler_type) {
	case LTTNG_EVENT_ENABLER_TYPE_RECORDER:
		lttng_fallthrough;
	case LTTNG_EVENT_ENABLER_TYPE_COUNTER:
	{
		struct lttng_event_enabler_session_common *event_enabler_session =
			container_of(event_enabler, struct lttng_event_enabler_session_common, parent);
		struct lttng_kernel_event_session_common_private *event_session_priv =
			container_of(event->priv, struct lttng_kernel_event_session_common_private, parent);
		return lttng_event_session_enabler_match_event_session(event_enabler_session, event_session_priv);
	}
	case LTTNG_EVENT_ENABLER_TYPE_NOTIFIER:
	{
		struct lttng_event_notifier_enabler *event_notifier_enabler =
			container_of(event_enabler, struct lttng_event_notifier_enabler, parent);
		struct lttng_kernel_event_notifier_private *event_notifier_priv =
			container_of(event->priv, struct lttng_kernel_event_notifier_private, parent);
		return lttng_event_notifier_enabler_match_event_notifier(event_notifier_enabler, event_notifier_priv);
	}
	default:
		WARN_ON_ONCE(1);
		return false;
	}
}

static
struct lttng_enabler_ref *lttng_enabler_ref(
		struct list_head *enablers_ref_list,
		struct lttng_event_enabler_common *enabler)
{
	struct lttng_enabler_ref *enabler_ref;

	list_for_each_entry(enabler_ref, enablers_ref_list, node) {
		if (enabler_ref->ref == enabler)
			return enabler_ref;
	}
	return NULL;
}

static
void lttng_event_enabler_create_tracepoint_events_if_missing(struct lttng_event_enabler_common *event_enabler)
{
	struct lttng_kernel_probe_desc *probe_desc;
	const struct lttng_kernel_event_desc *desc;
	struct list_head *probe_list;
	int i;

	probe_list = lttng_get_probe_list_head();
	/*
	 * For each probe event, if we find that a probe event matches
	 * our enabler, create an associated lttng_event if not
	 * already present.
	 */
	list_for_each_entry(probe_desc, probe_list, head) {
		for (i = 0; i < probe_desc->nr_events; i++) {
			struct lttng_kernel_event_common *event;

			desc = probe_desc->event_desc[i];
			if (!lttng_desc_match_enabler(desc, event_enabler))
				continue;
			/*
			 * We need to create an event for this event probe.
			 */
			event = _lttng_kernel_event_create(event_enabler, desc, NULL);
			if (IS_ERR(event)) {
				/* Skip if already found. */
				if (PTR_ERR(event) == -EEXIST)
					continue;
				printk(KERN_INFO "LTTng: Unable to create event %s\n",
					probe_desc->event_desc[i]->event_name);
			}
		}
	}
}

/* Try to create the event associated with this kprobe enabler. */
static
void lttng_event_enabler_create_kprobe_event_if_missing(struct lttng_event_enabler_common *event_enabler)
{
	struct lttng_kernel_event_common *event;

	event = _lttng_kernel_event_create(event_enabler, NULL, NULL);
	if (IS_ERR(event)) {
		if (PTR_ERR(event) != -EEXIST) {
			printk(KERN_INFO "LTTng: Unable to create kprobe event %s\n",
				event_enabler->event_param.name);
		}
	}
}

/* Try to create the event associated with this kretprobe enabler. */
static
void lttng_event_enabler_create_kretprobe_event_if_missing(struct lttng_event_enabler_common *event_enabler)
{
	struct lttng_kernel_abi_event *event_param = &event_enabler->event_param;
	struct lttng_kernel_event_pair event_pair;
	struct lttng_kernel_event_common *event;

	if (strlen(event_param->name) + strlen("_entry") >= LTTNG_KERNEL_ABI_SYM_NAME_LEN) {
		WARN_ON_ONCE(1);
		return;
	}

	memset(&event_pair, 0, sizeof(event_pair));
	event_pair.krp = lttng_kretprobes_create_krp(event_param->u.kretprobe.symbol_name,
			event_param->u.kretprobe.offset, event_param->u.kretprobe.addr);
	if (!event_pair.krp) {
		WARN_ON_ONCE(1);
		return;
	}
	strcpy(event_pair.name, event_enabler->event_param.name);
	strcat(event_pair.name, "_entry");
	event_pair.check_ids = true;
	event_pair.entryexit = LTTNG_KRETPROBE_ENTRY;
	event = _lttng_kernel_event_create(event_enabler, NULL, &event_pair);
	if (IS_ERR(event)) {
		if (PTR_ERR(event) != -EEXIST) {
			printk(KERN_INFO "LTTng: Unable to create kretprobe event %s\n",
				event_enabler->event_param.name);
		}
	}

	strcpy(event_pair.name, event_enabler->event_param.name);
	strcat(event_pair.name, "_exit");
	event_pair.check_ids = false;
	event_pair.entryexit = LTTNG_KRETPROBE_EXIT;
	event = _lttng_kernel_event_create(event_enabler, NULL, &event_pair);
	if (IS_ERR(event)) {
		if (PTR_ERR(event) != -EEXIST) {
			printk(KERN_INFO "LTTng: Unable to create kretprobe event %s\n",
				event_enabler->event_param.name);
		}
	}

	lttng_kretprobes_put_krp(event_pair.krp);
}

/*
 * Create event if it is missing and present in the list of tracepoint probes.
 * Should be called with sessions mutex held.
 */
static
void lttng_event_enabler_create_events_if_missing(struct lttng_event_enabler_common *event_enabler)
{
	int ret;

	switch (event_enabler->event_param.instrumentation) {
	case LTTNG_KERNEL_ABI_TRACEPOINT:
		lttng_event_enabler_create_tracepoint_events_if_missing(event_enabler);
		break;

	case LTTNG_KERNEL_ABI_SYSCALL:
		ret = lttng_event_enabler_create_syscall_events_if_missing(event_enabler);
		WARN_ON_ONCE(ret);
		break;

	case LTTNG_KERNEL_ABI_KPROBE:
		lttng_event_enabler_create_kprobe_event_if_missing(event_enabler);
		break;

	case LTTNG_KERNEL_ABI_KRETPROBE:
		lttng_event_enabler_create_kretprobe_event_if_missing(event_enabler);
		break;

	default:
		WARN_ON_ONCE(1);
		break;
	}
}

static
void lttng_event_enabler_init_event_filter(struct lttng_event_enabler_common *event_enabler,
		struct lttng_kernel_event_common *event)
{
	/* Link filter bytecodes if not linked yet. */
	lttng_enabler_link_bytecode(event->priv->desc, lttng_static_ctx,
		&event->priv->filter_bytecode_runtime_head, &event_enabler->filter_bytecode_head);
}

static
void lttng_event_enabler_init_event_capture(struct lttng_event_enabler_common *event_enabler,
		struct lttng_kernel_event_common *event)
{
	switch (event_enabler->enabler_type) {
	case LTTNG_EVENT_ENABLER_TYPE_RECORDER:
		lttng_fallthrough;
	case LTTNG_EVENT_ENABLER_TYPE_COUNTER:
		break;
	case LTTNG_EVENT_ENABLER_TYPE_NOTIFIER:
	{
		struct lttng_event_notifier_enabler *event_notifier_enabler =
			container_of(event_enabler, struct lttng_event_notifier_enabler, parent);
		struct lttng_kernel_event_notifier *event_notifier =
			container_of(event, struct lttng_kernel_event_notifier, parent);

		/* Link capture bytecodes if not linked yet. */
		lttng_enabler_link_bytecode(event->priv->desc,
			lttng_static_ctx, &event_notifier->priv->capture_bytecode_runtime_head,
			&event_notifier_enabler->capture_bytecode_head);
		event_notifier->priv->num_captures = event_notifier_enabler->num_captures;
		break;
	}
	default:
		WARN_ON_ONCE(1);
	}
}

/*
 * Create events associated with an event_enabler (if not already present),
 * and add backward reference from the event to the enabler.
 * Should be called with sessions mutex held.
 */
static
int lttng_event_enabler_ref_events(struct lttng_event_enabler_common *event_enabler)
{
	struct list_head *event_list_head = lttng_get_event_list_head_from_enabler(event_enabler);
	struct lttng_kernel_event_common_private *event_priv;

	 /*
	  * Only try to create events for enablers that are enabled, the user
	  * might still be attaching filter or exclusion to the event enabler.
	  */
	if (!event_enabler->enabled)
		goto end;

	lttng_syscall_table_set_wildcard_all(event_enabler);

	/* First ensure that probe events are created for this enabler. */
	lttng_event_enabler_create_events_if_missing(event_enabler);

	/* Link the created event with its associated enabler. */
	list_for_each_entry(event_priv, event_list_head, node) {
		struct lttng_kernel_event_common *event = event_priv->pub;
		struct lttng_enabler_ref *enabler_ref;

		if (!lttng_event_enabler_match_event(event_enabler, event))
			continue;

		enabler_ref = lttng_enabler_ref(&event_priv->enablers_ref_head, event_enabler);
		if (!enabler_ref) {
			int ret;

			/*
			 * If no backward ref, create it.
			 * Add backward ref from event_notifier to enabler.
			 */
			enabler_ref = kzalloc(sizeof(*enabler_ref), GFP_KERNEL);
			if (!enabler_ref)
				return -ENOMEM;

			enabler_ref->ref = event_enabler;
			list_add(&enabler_ref->node, &event_priv->enablers_ref_head);

			ret = lttng_append_event_to_channel_map(event_enabler, event,
					event->priv->desc->event_name);
			WARN_ON_ONCE(ret);
		}

		lttng_event_enabler_init_event_filter(event_enabler, event);
		lttng_event_enabler_init_event_capture(event_enabler, event);
	}
end:
	return 0;
}

/*
 * Called at module load: connect the probe on all enablers matching
 * this event.
 * Called with sessions lock held.
 */
int lttng_fix_pending_events(void)
{
	struct lttng_kernel_session_private *session_priv;

	list_for_each_entry(session_priv, &sessions, node)
		lttng_session_lazy_sync_event_enablers(session_priv->pub);
	return 0;
}

static bool lttng_event_notifier_group_has_active_event_notifiers(
		struct lttng_event_notifier_group *event_notifier_group)
{
	struct lttng_event_enabler_common *event_enabler;

	list_for_each_entry(event_enabler, &event_notifier_group->enablers_head, node) {
		if (event_enabler->enabled)
			return true;
	}
	return false;
}

bool lttng_event_notifier_active(void)
{
	struct lttng_event_notifier_group *event_notifier_group;

	list_for_each_entry(event_notifier_group, &event_notifier_groups, node) {
		if (lttng_event_notifier_group_has_active_event_notifiers(event_notifier_group))
			return true;
	}
	return false;
}

int lttng_fix_pending_event_notifiers(void)
{
	struct lttng_event_notifier_group *event_notifier_group;

	list_for_each_entry(event_notifier_group, &event_notifier_groups, node)
		lttng_event_notifier_group_sync_enablers(event_notifier_group);
	return 0;
}

struct lttng_event_recorder_enabler *lttng_event_recorder_enabler_create(
		enum lttng_enabler_format_type format_type,
		struct lttng_kernel_abi_event *event_param,
		struct lttng_kernel_channel_buffer *chan)
{
	struct lttng_event_recorder_enabler *event_enabler;

	event_enabler = kzalloc(sizeof(*event_enabler), GFP_KERNEL);
	if (!event_enabler)
		return NULL;
	event_enabler->parent.parent.enabler_type = LTTNG_EVENT_ENABLER_TYPE_RECORDER;
	event_enabler->parent.parent.format_type = format_type;
	INIT_LIST_HEAD(&event_enabler->parent.parent.filter_bytecode_head);
	memcpy(&event_enabler->parent.parent.event_param, event_param,
		sizeof(event_enabler->parent.parent.event_param));
	event_enabler->chan = chan;
	event_enabler->parent.chan = &chan->parent;

	/* ctx left NULL */
	event_enabler->parent.parent.enabled = 0;
	return event_enabler;
}

struct lttng_event_counter_enabler *lttng_event_counter_enabler_create(
		enum lttng_enabler_format_type format_type,
		struct lttng_kernel_abi_event *event_param,
		struct lttng_kernel_counter_key *counter_key,
		struct lttng_kernel_channel_counter *chan)
{
	struct lttng_event_counter_enabler *event_enabler;

	event_enabler = kzalloc(sizeof(*event_enabler), GFP_KERNEL);
	if (!event_enabler)
		return NULL;
	event_enabler->parent.parent.enabler_type = LTTNG_EVENT_ENABLER_TYPE_COUNTER;
	event_enabler->parent.parent.format_type = format_type;
	INIT_LIST_HEAD(&event_enabler->parent.parent.filter_bytecode_head);
	memcpy(&event_enabler->parent.parent.event_param, event_param,
		sizeof(event_enabler->parent.parent.event_param));
	event_enabler->chan = chan;
	event_enabler->parent.chan = &chan->parent;
	if (create_counter_key_from_kernel(&event_enabler->key, counter_key)) {
		kfree(event_enabler);
		return NULL;
	}

	/* ctx left NULL */
	event_enabler->parent.parent.enabled = 0;
	event_enabler->parent.parent.user_token = event_param->token;
	return event_enabler;
}

void lttng_event_enabler_session_add(struct lttng_kernel_session *session,
		struct lttng_event_enabler_session_common *event_enabler)
{
	mutex_lock(&sessions_mutex);
	list_add(&event_enabler->parent.node, &session->priv->enablers_head);
	event_enabler->parent.published = true;
	lttng_session_lazy_sync_event_enablers(session);
	mutex_unlock(&sessions_mutex);
}

int lttng_event_enabler_enable(struct lttng_event_enabler_common *event_enabler)
{
	mutex_lock(&sessions_mutex);
	event_enabler->enabled = 1;
	lttng_event_enabler_sync(event_enabler);
	mutex_unlock(&sessions_mutex);
	return 0;
}

int lttng_event_enabler_disable(struct lttng_event_enabler_common *event_enabler)
{
	mutex_lock(&sessions_mutex);
	event_enabler->enabled = 0;
	lttng_event_enabler_sync(event_enabler);
	mutex_unlock(&sessions_mutex);
	return 0;
}

static
int lttng_enabler_attach_filter_bytecode(struct lttng_event_enabler_common *enabler,
		struct lttng_kernel_abi_filter_bytecode __user *bytecode)
{
	struct lttng_kernel_bytecode_node *bytecode_node;
	uint32_t bytecode_len;
	int ret;

	ret = get_user(bytecode_len, &bytecode->len);
	if (ret)
		return ret;
	bytecode_node = lttng_kvzalloc(sizeof(*bytecode_node) + bytecode_len,
			GFP_KERNEL);
	if (!bytecode_node)
		return -ENOMEM;
	ret = copy_from_user(&bytecode_node->bc, bytecode,
		sizeof(*bytecode) + bytecode_len);
	if (ret)
		goto error_free;

	bytecode_node->type = LTTNG_KERNEL_BYTECODE_TYPE_FILTER;
	bytecode_node->enabler = enabler;
	/* Enforce length based on allocated size */
	bytecode_node->bc.len = bytecode_len;
	list_add_tail(&bytecode_node->node, &enabler->filter_bytecode_head);

	return 0;

error_free:
	lttng_kvfree(bytecode_node);
	return ret;
}

int lttng_event_enabler_attach_filter_bytecode(struct lttng_event_enabler_common *event_enabler,
		struct lttng_kernel_abi_filter_bytecode __user *bytecode)
{
	int ret;
	ret = lttng_enabler_attach_filter_bytecode(event_enabler, bytecode);
	if (ret)
		goto error;
	lttng_event_enabler_sync(event_enabler);
	return 0;

error:
	return ret;
}

int lttng_event_add_callsite(struct lttng_kernel_event_common *event,
		struct lttng_kernel_abi_event_callsite __user *callsite)
{

	switch (event->priv->instrumentation) {
	case LTTNG_KERNEL_ABI_UPROBE:
		return lttng_uprobes_event_add_callsite(event, callsite);
	default:
		return -EINVAL;
	}
}

static
void lttng_enabler_destroy(struct lttng_event_enabler_common *enabler)
{
	struct lttng_kernel_bytecode_node *filter_node, *tmp_filter_node;

	/* Destroy filter bytecode */
	list_for_each_entry_safe(filter_node, tmp_filter_node,
			&enabler->filter_bytecode_head, node) {
		lttng_kvfree(filter_node);
	}
}

void lttng_event_enabler_destroy(struct lttng_event_enabler_common *event_enabler)
{
	lttng_enabler_destroy(event_enabler);
	if (event_enabler->published)
		list_del(&event_enabler->node);

	switch (event_enabler->enabler_type) {
	case LTTNG_EVENT_ENABLER_TYPE_RECORDER:
	{
		struct lttng_event_recorder_enabler *event_recorder_enabler =
			container_of(event_enabler, struct lttng_event_recorder_enabler, parent.parent);

		kfree(event_recorder_enabler);
		break;
	}
	case LTTNG_EVENT_ENABLER_TYPE_NOTIFIER:
	{
		struct lttng_event_notifier_enabler *event_notifier_enabler =
			container_of(event_enabler, struct lttng_event_notifier_enabler, parent);

		kfree(event_notifier_enabler);
		break;
	}
	case LTTNG_EVENT_ENABLER_TYPE_COUNTER:
	{
		struct lttng_event_counter_enabler *event_counter_enabler =
			container_of(event_enabler, struct lttng_event_counter_enabler, parent.parent);

		destroy_counter_key(event_counter_enabler->key);
		kfree(event_counter_enabler);
		break;
	}
	default:
		WARN_ON_ONCE(1);
	}
}

struct lttng_event_notifier_enabler *lttng_event_notifier_enabler_create(
		enum lttng_enabler_format_type format_type,
		struct lttng_kernel_abi_event_notifier *event_notifier_param,
		struct lttng_event_notifier_group *event_notifier_group)
{
	struct lttng_event_notifier_enabler *event_notifier_enabler;

	event_notifier_enabler = kzalloc(sizeof(*event_notifier_enabler), GFP_KERNEL);
	if (!event_notifier_enabler)
		return NULL;

	event_notifier_enabler->parent.enabler_type = LTTNG_EVENT_ENABLER_TYPE_NOTIFIER;
	event_notifier_enabler->parent.format_type = format_type;
	INIT_LIST_HEAD(&event_notifier_enabler->parent.filter_bytecode_head);
	INIT_LIST_HEAD(&event_notifier_enabler->capture_bytecode_head);

	event_notifier_enabler->error_counter_index = event_notifier_param->error_counter_index;
	event_notifier_enabler->num_captures = 0;

	memcpy(&event_notifier_enabler->parent.event_param, &event_notifier_param->event,
		sizeof(event_notifier_enabler->parent.event_param));

	event_notifier_enabler->parent.enabled = 0;
	event_notifier_enabler->parent.user_token = event_notifier_param->event.token;
	event_notifier_enabler->group = event_notifier_group;
	return event_notifier_enabler;
}

void lttng_event_notifier_enabler_group_add(struct lttng_event_notifier_group *event_notifier_group,
		struct lttng_event_notifier_enabler *event_notifier_enabler)
{
	mutex_lock(&sessions_mutex);
	list_add(&event_notifier_enabler->parent.node, &event_notifier_enabler->group->enablers_head);
	event_notifier_enabler->parent.published = true;
	lttng_event_notifier_group_sync_enablers(event_notifier_enabler->group);
	mutex_unlock(&sessions_mutex);
}

int lttng_event_notifier_enabler_attach_capture_bytecode(
		struct lttng_event_notifier_enabler *event_notifier_enabler,
		struct lttng_kernel_abi_capture_bytecode __user *bytecode)
{
	struct lttng_kernel_bytecode_node *bytecode_node;
	struct lttng_event_enabler_common *enabler =
			lttng_event_notifier_enabler_as_enabler(event_notifier_enabler);
	uint32_t bytecode_len;
	int ret;

	ret = get_user(bytecode_len, &bytecode->len);
	if (ret)
		return ret;

	bytecode_node = lttng_kvzalloc(sizeof(*bytecode_node) + bytecode_len,
			GFP_KERNEL);
	if (!bytecode_node)
		return -ENOMEM;

	ret = copy_from_user(&bytecode_node->bc, bytecode,
		sizeof(*bytecode) + bytecode_len);
	if (ret)
		goto error_free;

	bytecode_node->type = LTTNG_KERNEL_BYTECODE_TYPE_CAPTURE;
	bytecode_node->enabler = enabler;

	/* Enforce length based on allocated size */
	bytecode_node->bc.len = bytecode_len;
	list_add_tail(&bytecode_node->node, &event_notifier_enabler->capture_bytecode_head);

	event_notifier_enabler->num_captures++;

	lttng_event_notifier_group_sync_enablers(event_notifier_enabler->group);
	goto end;

error_free:
	lttng_kvfree(bytecode_node);
end:
	return ret;
}

static
void lttng_event_sync_filter_state(struct lttng_kernel_event_common *event)
{
	int has_enablers_without_filter_bytecode = 0, nr_filters = 0;
	struct lttng_kernel_bytecode_runtime *runtime;
	struct lttng_enabler_ref *enabler_ref;

	/* Check if has enablers without bytecode enabled */
	list_for_each_entry(enabler_ref, &event->priv->enablers_ref_head, node) {
		if (enabler_ref->ref->enabled
				&& list_empty(&enabler_ref->ref->filter_bytecode_head)) {
			has_enablers_without_filter_bytecode = 1;
			break;
		}
	}
	event->priv->has_enablers_without_filter_bytecode = has_enablers_without_filter_bytecode;

	/* Enable filters */
	list_for_each_entry(runtime, &event->priv->filter_bytecode_runtime_head, node) {
		lttng_bytecode_sync_state(runtime);
		nr_filters++;
	}
	WRITE_ONCE(event->eval_filter, !(has_enablers_without_filter_bytecode || !nr_filters));
}

static
void lttng_event_sync_capture_state(struct lttng_kernel_event_common *event)
{
	switch (event->type) {
	case LTTNG_KERNEL_EVENT_TYPE_RECORDER:
		lttng_fallthrough;
	case LTTNG_KERNEL_EVENT_TYPE_COUNTER:
		break;
	case LTTNG_KERNEL_EVENT_TYPE_NOTIFIER:
	{
		struct lttng_kernel_event_notifier *event_notifier =
			container_of(event, struct lttng_kernel_event_notifier, parent);
		struct lttng_kernel_bytecode_runtime *runtime;
		int nr_captures = 0;

		/* Enable captures */
		list_for_each_entry(runtime, &event_notifier->priv->capture_bytecode_runtime_head, node) {
			lttng_bytecode_sync_state(runtime);
			nr_captures++;
		}
		WRITE_ONCE(event_notifier->eval_capture, !!nr_captures);
		break;
	}
	default:
		WARN_ON_ONCE(1);
	}
}

static
bool lttng_get_event_enabled_state(struct lttng_kernel_event_common *event)
{
	struct lttng_enabler_ref *enabler_ref;
	bool enabled = false;

	switch (event->priv->instrumentation) {
	case LTTNG_KERNEL_ABI_TRACEPOINT:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_SYSCALL:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_KPROBE:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_KRETPROBE:
		/* Enable events */
		list_for_each_entry(enabler_ref, &event->priv->enablers_ref_head, node) {
			if (enabler_ref->ref->enabled) {
				enabled = true;
				break;
			}
		}
		break;
	default:
		WARN_ON_ONCE(1);
		return false;
	}

	switch (event->type) {
	case LTTNG_KERNEL_EVENT_TYPE_RECORDER:
		lttng_fallthrough;
	case LTTNG_KERNEL_EVENT_TYPE_COUNTER:
	{
		struct lttng_kernel_event_session_common_private *event_session_common_priv =
			container_of(event->priv, struct lttng_kernel_event_session_common_private, parent);

		/*
		 * Enabled state is based on union of enablers, with
		 * intersection of session and channel transient enable
		 * states.
		 */
		return enabled && event_session_common_priv->chan->session->priv->tstate && event_session_common_priv->chan->priv->tstate;
	}
	case LTTNG_KERNEL_EVENT_TYPE_NOTIFIER:
		return enabled;
	default:
		WARN_ON_ONCE(1);
		return false;
	}
}

static
bool lttng_event_is_lazy_sync(struct lttng_kernel_event_common *event)
{
	switch (event->priv->instrumentation) {
	case LTTNG_KERNEL_ABI_TRACEPOINT:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_SYSCALL:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_KPROBE:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_KRETPROBE:
		return true;

	default:
		/* Not handled with lazy sync. */
		return false;
	}
}

/*
 * Should be called with sessions mutex held.
 */
static
void lttng_sync_event_list(struct list_head *event_enabler_list,
		struct list_head *event_list)
{
	struct lttng_kernel_event_common_private *event_priv;
	struct lttng_event_enabler_common *event_enabler;

	list_for_each_entry(event_enabler, event_enabler_list, node)
		lttng_event_enabler_ref_events(event_enabler);

	/*
	 * For each event, if at least one of its enablers is enabled,
	 * and its channel and session transient states are enabled, we
	 * enable the event, else we disable it.
	 */
	list_for_each_entry(event_priv, event_list, node) {
		struct lttng_kernel_event_common *event = event_priv->pub;
		bool enabled;

		if (!lttng_event_is_lazy_sync(event))
			continue;

		enabled = lttng_get_event_enabled_state(event);
		WRITE_ONCE(event->enabled, enabled);
		/*
		 * Sync tracepoint registration with event enabled state.
		 */
		if (enabled) {
			if (!event_priv->registered)
				register_event(event);
		} else {
			if (event_priv->registered)
				unregister_event(event);
		}

		lttng_event_sync_filter_state(event);
		lttng_event_sync_capture_state(event);
	}
}

/*
 * lttng_session_sync_event_enablers should be called just before starting a
 * session.
 */
static
void lttng_session_sync_event_enablers(struct lttng_kernel_session *session)
{
	lttng_sync_event_list(&session->priv->enablers_head, &session->priv->events_head);
}

/*
 * Apply enablers to session events, adding events to session if need
 * be. It is required after each modification applied to an active
 * session, and right before session "start".
 * "lazy" sync means we only sync if required.
 * Should be called with sessions mutex held.
 */
static
void lttng_session_lazy_sync_event_enablers(struct lttng_kernel_session *session)
{
	/* We can skip if session is not active */
	if (!session->active)
		return;
	lttng_session_sync_event_enablers(session);
}

static
void lttng_event_notifier_group_sync_enablers(struct lttng_event_notifier_group *event_notifier_group)
{
	lttng_sync_event_list(&event_notifier_group->enablers_head, &event_notifier_group->event_notifiers_head);
}

static
void lttng_event_enabler_sync(struct lttng_event_enabler_common *event_enabler)
{
	switch (event_enabler->enabler_type) {
	case LTTNG_EVENT_ENABLER_TYPE_RECORDER:
		lttng_fallthrough;
	case LTTNG_EVENT_ENABLER_TYPE_COUNTER:
	{
		struct lttng_event_enabler_session_common *event_enabler_session =
			container_of(event_enabler, struct lttng_event_enabler_session_common, parent);
		lttng_session_lazy_sync_event_enablers(event_enabler_session->chan->session);
		break;
	}
	case LTTNG_EVENT_ENABLER_TYPE_NOTIFIER:
	{
		struct lttng_event_notifier_enabler *event_notifier_enabler =
			container_of(event_enabler, struct lttng_event_notifier_enabler, parent);
		lttng_event_notifier_group_sync_enablers(event_notifier_enabler->group);
		break;
	}
	default:
		WARN_ON_ONCE(1);
	}
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
		struct lttng_kernel_ring_buffer_channel *chan, bool *coherent)
{
	struct lttng_kernel_ring_buffer_ctx ctx;
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
			stream->transport->ops.priv->packet_avail_size(chan),
			len);
	lib_ring_buffer_ctx_init(&ctx, chan, reserve_len,
			sizeof(char), NULL);
	/*
	 * If reservation failed, return an error to the caller.
	 */
	ret = stream->transport->ops.event_reserve(&ctx);
	if (ret != 0) {
		printk(KERN_WARNING "LTTng: Metadata event reservation failed\n");
		stream->coherent = false;
		goto end;
	}
	stream->transport->ops.event_write(&ctx,
			stream->metadata_cache->data + stream->metadata_in,
			reserve_len, 1);
	stream->transport->ops.event_commit(&ctx);
	stream->metadata_in += reserve_len;
	if (reserve_len < len)
		stream->coherent = false;
	else
		stream->coherent = true;
	ret = reserve_len;

end:
	if (coherent)
		*coherent = stream->coherent;
	mutex_unlock(&stream->metadata_cache->lock);
	return ret;
}

static
void lttng_metadata_begin(struct lttng_kernel_session *session)
{
	if (atomic_inc_return(&session->priv->metadata_cache->producing) == 1)
		mutex_lock(&session->priv->metadata_cache->lock);
}

static
void lttng_metadata_end(struct lttng_kernel_session *session)
{
	WARN_ON_ONCE(!atomic_read(&session->priv->metadata_cache->producing));
	if (atomic_dec_return(&session->priv->metadata_cache->producing) == 0) {
		struct lttng_metadata_stream *stream;

		list_for_each_entry(stream, &session->priv->metadata_cache->metadata_stream, node)
			wake_up_interruptible(&stream->read_wait);
		mutex_unlock(&session->priv->metadata_cache->lock);
	}
}

/*
 * Write the metadata to the metadata cache.
 * Must be called with sessions_mutex held.
 * The metadata cache lock protects us from concurrent read access from
 * thread outputting metadata content to ring buffer.
 * The content of the printf is printed as a single atomic metadata
 * transaction.
 */
static
int lttng_metadata_printf(struct lttng_kernel_session *session,
			  const char *fmt, ...)
{
	char *str;
	size_t len;
	va_list ap;

	WARN_ON_ONCE(!LTTNG_READ_ONCE(session->active));

	va_start(ap, fmt);
	str = kvasprintf(GFP_KERNEL, fmt, ap);
	va_end(ap);
	if (!str)
		return -ENOMEM;

	len = strlen(str);
	WARN_ON_ONCE(!atomic_read(&session->priv->metadata_cache->producing));
	if (session->priv->metadata_cache->metadata_written + len >
			session->priv->metadata_cache->cache_alloc) {
		char *tmp_cache_realloc;
		unsigned int tmp_cache_alloc_size;

		tmp_cache_alloc_size = max_t(unsigned int,
				session->priv->metadata_cache->cache_alloc + len,
				session->priv->metadata_cache->cache_alloc << 1);
		tmp_cache_realloc = vzalloc(tmp_cache_alloc_size);
		if (!tmp_cache_realloc)
			goto err;
		if (session->priv->metadata_cache->data) {
			memcpy(tmp_cache_realloc,
				session->priv->metadata_cache->data,
				session->priv->metadata_cache->cache_alloc);
			vfree(session->priv->metadata_cache->data);
		}

		session->priv->metadata_cache->cache_alloc = tmp_cache_alloc_size;
		session->priv->metadata_cache->data = tmp_cache_realloc;
	}
	memcpy(session->priv->metadata_cache->data +
			session->priv->metadata_cache->metadata_written,
			str, len);
	session->priv->metadata_cache->metadata_written += len;
	kfree(str);

	return 0;

err:
	kfree(str);
	return -ENOMEM;
}

static
int print_tabs(struct lttng_kernel_session *session, size_t nesting)
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

static
int lttng_field_name_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting)
{
	return lttng_metadata_printf(session, " _%s;\n", field->name);
}

static
int _lttng_integer_type_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_type_integer *type,
		enum lttng_kernel_string_encoding parent_encoding,
		size_t nesting)
{
	int ret;

	ret = print_tabs(session, nesting);
	if (ret)
		return ret;
	ret = lttng_metadata_printf(session,
		"integer { size = %u; align = %u; signed = %u; encoding = %s; base = %u;%s }",
		type->size,
		type->alignment,
		type->signedness,
		(parent_encoding == lttng_kernel_string_encoding_none)
			? "none"
			: (parent_encoding == lttng_kernel_string_encoding_UTF8)
				? "UTF8"
				: "ASCII",
		type->base,
#if __BYTE_ORDER == __BIG_ENDIAN
		type->reverse_byte_order ? " byte_order = le;" : ""
#else
		type->reverse_byte_order ? " byte_order = be;" : ""
#endif
	);
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_struct_type_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_type_struct *type,
		size_t nesting)
{
	const char *prev_field_name = NULL;
	int ret;
	uint32_t i, nr_fields;
	unsigned int alignment;

	ret = print_tabs(session, nesting);
	if (ret)
		return ret;
	ret = lttng_metadata_printf(session,
		"struct {\n");
	if (ret)
		return ret;
	nr_fields = type->nr_fields;
	for (i = 0; i < nr_fields; i++) {
		const struct lttng_kernel_event_field *iter_field;

		iter_field = type->fields[i];
		ret = _lttng_field_statedump(session, iter_field, nesting + 1, &prev_field_name);
		if (ret)
			return ret;
	}
	ret = print_tabs(session, nesting);
	if (ret)
		return ret;
	alignment = type->alignment;
	if (alignment) {
		ret = lttng_metadata_printf(session,
			"} align(%u)",
			alignment);
	} else {
		ret = lttng_metadata_printf(session,
			"}");
	}
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_struct_field_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting)
{
	int ret;

	ret = _lttng_struct_type_statedump(session,
			lttng_kernel_get_type_struct(field->type), nesting);
	if (ret)
		return ret;
	return lttng_field_name_statedump(session, field, nesting);
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_variant_type_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_type_variant *type,
		size_t nesting,
		const char *prev_field_name)
{
	const char *tag_name;
	int ret;
	uint32_t i, nr_choices;

	tag_name = type->tag_name;
	if (!tag_name)
		tag_name = prev_field_name;
	if (!tag_name)
		return -EINVAL;
	/*
	 * CTF 1.8 does not allow expressing nonzero variant alignment in a nestable way.
	 */
	if (type->alignment != 0)
		return -EINVAL;
	ret = print_tabs(session, nesting);
	if (ret)
		return ret;
	ret = lttng_metadata_printf(session,
		"variant <_%s> {\n",
		tag_name);
	if (ret)
		return ret;
	nr_choices = type->nr_choices;
	for (i = 0; i < nr_choices; i++) {
		const struct lttng_kernel_event_field *iter_field;

		iter_field = type->choices[i];
		ret = _lttng_field_statedump(session, iter_field, nesting + 1, NULL);
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
int _lttng_variant_field_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting,
		const char *prev_field_name)
{
	int ret;

	ret = _lttng_variant_type_statedump(session,
			lttng_kernel_get_type_variant(field->type), nesting,
			prev_field_name);
	if (ret)
		return ret;
	return lttng_field_name_statedump(session, field, nesting);
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_array_field_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting)
{
	int ret;
	const struct lttng_kernel_type_array *array_type;
	const struct lttng_kernel_type_common *elem_type;

	array_type = lttng_kernel_get_type_array(field->type);
	WARN_ON_ONCE(!array_type);

	if (array_type->alignment) {
		ret = print_tabs(session, nesting);
		if (ret)
			return ret;
		ret = lttng_metadata_printf(session,
		"struct { } align(%u) _%s_padding;\n",
				array_type->alignment * CHAR_BIT,
				field->name);
		if (ret)
			return ret;
	}
	/*
	 * Nested compound types: Only array of structures and variants are
	 * currently supported.
	 */
	elem_type = array_type->elem_type;
	switch (elem_type->type) {
	case lttng_kernel_type_integer:
	case lttng_kernel_type_struct:
	case lttng_kernel_type_variant:
		ret = _lttng_type_statedump(session, elem_type,
				array_type->encoding, nesting);
		if (ret)
			return ret;
		break;

	default:
		return -EINVAL;
	}
	ret = lttng_metadata_printf(session,
		" _%s[%u];\n",
		field->name,
		array_type->length);
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_sequence_field_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting,
		const char *prev_field_name)
{
	int ret;
	const char *length_name;
	const struct lttng_kernel_type_sequence *sequence_type;
	const struct lttng_kernel_type_common *elem_type;

	sequence_type = lttng_kernel_get_type_sequence(field->type);
	WARN_ON_ONCE(!sequence_type);

	length_name = sequence_type->length_name;
	if (!length_name)
		length_name = prev_field_name;
	if (!length_name)
		return -EINVAL;

	if (sequence_type->alignment) {
		ret = print_tabs(session, nesting);
		if (ret)
			return ret;
		ret = lttng_metadata_printf(session,
		"struct { } align(%u) _%s_padding;\n",
				sequence_type->alignment * CHAR_BIT,
				field->name);
		if (ret)
			return ret;
	}

	/*
	 * Nested compound types: Only array of structures and variants are
	 * currently supported.
	 */
	elem_type = sequence_type->elem_type;
	switch (elem_type->type) {
	case lttng_kernel_type_integer:
	case lttng_kernel_type_struct:
	case lttng_kernel_type_variant:
		ret = _lttng_type_statedump(session, elem_type,
				sequence_type->encoding, nesting);
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
int _lttng_enum_type_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_type_enum *type,
		size_t nesting)
{
	const struct lttng_kernel_enum_desc *enum_desc;
	const struct lttng_kernel_type_common *container_type;
	int ret;
	unsigned int i, nr_entries;

	container_type = type->container_type;
	if (container_type->type != lttng_kernel_type_integer) {
		ret = -EINVAL;
		goto end;
	}
	enum_desc = type->desc;
	nr_entries = enum_desc->nr_entries;

	ret = print_tabs(session, nesting);
	if (ret)
		goto end;
	ret = lttng_metadata_printf(session, "enum : ");
	if (ret)
		goto end;
	ret = _lttng_integer_type_statedump(session, lttng_kernel_get_type_integer(container_type),
			lttng_kernel_string_encoding_none, 0);
	if (ret)
		goto end;
	ret = lttng_metadata_printf(session, " {\n");
	if (ret)
		goto end;
	/* Dump all entries */
	for (i = 0; i < nr_entries; i++) {
		const struct lttng_kernel_enum_entry *entry = enum_desc->entries[i];
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
	ret = lttng_metadata_printf(session, "}");
end:
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_enum_field_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting)
{
	int ret;
	const struct lttng_kernel_type_enum *enum_type;

	enum_type = lttng_kernel_get_type_enum(field->type);
	WARN_ON_ONCE(!enum_type);
	ret = _lttng_enum_type_statedump(session, enum_type, nesting);
	if (ret)
		return ret;
	return lttng_field_name_statedump(session, field, nesting);
}

static
int _lttng_integer_field_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting)
{
	int ret;

	ret = _lttng_integer_type_statedump(session, lttng_kernel_get_type_integer(field->type),
			lttng_kernel_string_encoding_none, nesting);
	if (ret)
		return ret;
	return lttng_field_name_statedump(session, field, nesting);
}

static
int _lttng_string_type_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_type_string *type,
		size_t nesting)
{
	int ret;

	/* Default encoding is UTF8 */
	ret = print_tabs(session, nesting);
	if (ret)
		return ret;
	ret = lttng_metadata_printf(session,
		"string%s",
		type->encoding == lttng_kernel_string_encoding_ASCII ?
			" { encoding = ASCII; }" : "");
	return ret;
}

static
int _lttng_string_field_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting)
{
	const struct lttng_kernel_type_string *string_type;
	int ret;

	string_type = lttng_kernel_get_type_string(field->type);
	WARN_ON_ONCE(!string_type);
	ret = _lttng_string_type_statedump(session, string_type, nesting);
	if (ret)
		return ret;
	return lttng_field_name_statedump(session, field, nesting);
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_type_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_type_common *type,
		enum lttng_kernel_string_encoding parent_encoding,
		size_t nesting)
{
	int ret = 0;

	switch (type->type) {
	case lttng_kernel_type_integer:
		ret = _lttng_integer_type_statedump(session,
				lttng_kernel_get_type_integer(type),
				parent_encoding, nesting);
		break;
	case lttng_kernel_type_enum:
		ret = _lttng_enum_type_statedump(session,
				lttng_kernel_get_type_enum(type),
				nesting);
		break;
	case lttng_kernel_type_string:
		ret = _lttng_string_type_statedump(session,
				lttng_kernel_get_type_string(type),
				nesting);
		break;
	case lttng_kernel_type_struct:
		ret = _lttng_struct_type_statedump(session,
				lttng_kernel_get_type_struct(type),
				nesting);
		break;
	case lttng_kernel_type_variant:
		ret = _lttng_variant_type_statedump(session,
				lttng_kernel_get_type_variant(type),
				nesting, NULL);
		break;

	/* Nested arrays and sequences are not supported yet. */
	case lttng_kernel_type_array:
	case lttng_kernel_type_sequence:
	default:
		WARN_ON_ONCE(1);
		return -EINVAL;
	}
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_field_statedump(struct lttng_kernel_session *session,
		const struct lttng_kernel_event_field *field,
		size_t nesting,
		const char **prev_field_name_p)
{
	const char *prev_field_name = NULL;
	int ret = 0;

	if (prev_field_name_p)
		prev_field_name = *prev_field_name_p;
	switch (field->type->type) {
	case lttng_kernel_type_integer:
		ret = _lttng_integer_field_statedump(session, field, nesting);
		break;
	case lttng_kernel_type_enum:
		ret = _lttng_enum_field_statedump(session, field, nesting);
		break;
	case lttng_kernel_type_string:
		ret = _lttng_string_field_statedump(session, field, nesting);
		break;
	case lttng_kernel_type_struct:
		ret = _lttng_struct_field_statedump(session, field, nesting);
		break;
	case lttng_kernel_type_array:
		ret = _lttng_array_field_statedump(session, field, nesting);
		break;
	case lttng_kernel_type_sequence:
		ret = _lttng_sequence_field_statedump(session, field, nesting, prev_field_name);
		break;
	case lttng_kernel_type_variant:
		ret = _lttng_variant_field_statedump(session, field, nesting, prev_field_name);
		break;

	default:
		WARN_ON_ONCE(1);
		return -EINVAL;
	}
	if (prev_field_name_p)
		*prev_field_name_p = field->name;
	return ret;
}

static
int _lttng_context_metadata_statedump(struct lttng_kernel_session *session,
				    struct lttng_kernel_ctx *ctx)
{
	const char *prev_field_name = NULL;
	int ret = 0;
	int i;

	if (!ctx)
		return 0;
	for (i = 0; i < ctx->nr_fields; i++) {
		const struct lttng_kernel_ctx_field *field = &ctx->fields[i];

		ret = _lttng_field_statedump(session, field->event_field, 2, &prev_field_name);
		if (ret)
			return ret;
	}
	return ret;
}

static
int _lttng_fields_metadata_statedump(struct lttng_kernel_session *session,
				   struct lttng_kernel_event_recorder *event_recorder)
{
	const char *prev_field_name = NULL;
	const struct lttng_kernel_event_desc *desc = event_recorder->priv->parent.parent.desc;
	int ret = 0;
	int i;

	for (i = 0; i < desc->tp_class->nr_fields; i++) {
		const struct lttng_kernel_event_field *field = desc->tp_class->fields[i];

		ret = _lttng_field_statedump(session, field, 2, &prev_field_name);
		if (ret)
			return ret;
	}
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 * The entire event metadata is printed as a single atomic metadata
 * transaction.
 */
static
int _lttng_event_recorder_metadata_statedump(struct lttng_kernel_event_common *event)
{
	struct lttng_kernel_event_recorder *event_recorder;
	struct lttng_kernel_channel_buffer *chan;
	struct lttng_kernel_session *session;
	int ret = 0;

	if (event->type != LTTNG_KERNEL_EVENT_TYPE_RECORDER)
		return 0;
	event_recorder = container_of(event, struct lttng_kernel_event_recorder, parent);
	chan = event_recorder->chan;
	session = chan->parent.session;

	if (event_recorder->priv->metadata_dumped || !LTTNG_READ_ONCE(session->active))
		return 0;
	if (chan->priv->channel_type == METADATA_CHANNEL)
		return 0;

	lttng_metadata_begin(session);

	ret = lttng_metadata_printf(session,
		"event {\n"
		"	name = \"%s\";\n"
		"	id = %llu;\n"
		"	stream_id = %u;\n",
		event_recorder->priv->parent.parent.desc->event_name,
		event_recorder->priv->parent.id,
		event_recorder->chan->priv->id);
	if (ret)
		goto end;

	ret = lttng_metadata_printf(session,
		"	fields := struct {\n"
		);
	if (ret)
		goto end;

	ret = _lttng_fields_metadata_statedump(session, event_recorder);
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

	event_recorder->priv->metadata_dumped = 1;
end:
	lttng_metadata_end(session);
	return ret;

}

/*
 * Must be called with sessions_mutex held.
 * The entire channel metadata is printed as a single atomic metadata
 * transaction.
 */
static
int _lttng_channel_metadata_statedump(struct lttng_kernel_session *session,
				    struct lttng_kernel_channel_buffer *chan)
{
	int ret = 0;

	if (chan->priv->metadata_dumped || !LTTNG_READ_ONCE(session->active))
		return 0;

	if (chan->priv->channel_type == METADATA_CHANNEL)
		return 0;

	lttng_metadata_begin(session);

	WARN_ON_ONCE(!chan->priv->header_type);
	ret = lttng_metadata_printf(session,
		"stream {\n"
		"	id = %u;\n"
		"	event.header := %s;\n"
		"	packet.context := struct packet_context;\n",
		chan->priv->id,
		chan->priv->header_type == 1 ? "struct event_header_compact" :
			"struct event_header_large");
	if (ret)
		goto end;

	if (chan->priv->ctx) {
		ret = lttng_metadata_printf(session,
			"	event.context := struct {\n");
		if (ret)
			goto end;
	}
	ret = _lttng_context_metadata_statedump(session, chan->priv->ctx);
	if (ret)
		goto end;
	if (chan->priv->ctx) {
		ret = lttng_metadata_printf(session,
			"	};\n");
		if (ret)
			goto end;
	}

	ret = lttng_metadata_printf(session,
		"};\n\n");

	chan->priv->metadata_dumped = 1;
end:
	lttng_metadata_end(session);
	return ret;
}

/*
 * Must be called with sessions_mutex held.
 */
static
int _lttng_stream_packet_context_declare(struct lttng_kernel_session *session)
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
int _lttng_event_header_declare(struct lttng_kernel_session *session)
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
 *
 * Use 64bit timespec on kernels that have it, this makes 32bit arch
 * y2038 compliant.
 */
static
int64_t measure_clock_offset(void)
{
	uint64_t monotonic_avg, monotonic[2], realtime;
	uint64_t tcf = trace_clock_freq();
	int64_t offset;
	unsigned long flags;
	struct timespec64 rts = { 0, 0 };

	/* Disable interrupts to increase correlation precision. */
	local_irq_save(flags);
	monotonic[0] = trace_clock_read64();
	ktime_get_real_ts64(&rts);
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

static
int print_escaped_ctf_string(struct lttng_kernel_session *session, const char *string)
{
	int ret = 0;
	size_t i;
	char cur;

	i = 0;
	cur = string[i];
	while (cur != '\0') {
		switch (cur) {
		case '\n':
			ret = lttng_metadata_printf(session, "%s", "\\n");
			break;
		case '\\':
		case '"':
			ret = lttng_metadata_printf(session, "%c", '\\');
			if (ret)
				goto error;
			/* We still print the current char */
			lttng_fallthrough;
		default:
			ret = lttng_metadata_printf(session, "%c", cur);
			break;
		}

		if (ret)
			goto error;

		cur = string[++i];
	}
error:
	return ret;
}

static
int print_metadata_escaped_field(struct lttng_kernel_session *session, const char *field,
		const char *field_value)
{
	int ret;

	ret = lttng_metadata_printf(session, "	%s = \"", field);
	if (ret)
		goto error;

	ret = print_escaped_ctf_string(session, field_value);
	if (ret)
		goto error;

	ret = lttng_metadata_printf(session, "\";\n");

error:
	return ret;
}

/*
 * Output metadata into this session's metadata buffers.
 * Must be called with sessions_mutex held.
 */
static
int _lttng_session_metadata_statedump(struct lttng_kernel_session *session)
{
	unsigned char *uuid_c = session->priv->uuid.b;
	unsigned char uuid_s[37], clock_uuid_s[BOOT_ID_LEN];
	const char *product_uuid;
	struct lttng_kernel_channel_common_private *chan_priv;
	struct lttng_kernel_event_recorder_private *event_recorder_priv;
	int ret = 0;

	if (!LTTNG_READ_ONCE(session->active))
		return 0;

	lttng_metadata_begin(session);

	if (session->priv->metadata_dumped)
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
		"	trace_buffering_scheme = \"global\";\n",
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

	ret = print_metadata_escaped_field(session, "trace_name", session->priv->name);
	if (ret)
		goto end;
	ret = print_metadata_escaped_field(session, "trace_creation_datetime",
			session->priv->creation_time);
	if (ret)
		goto end;

	/* Add the product UUID to the 'env' section */
	product_uuid = dmi_get_system_info(DMI_PRODUCT_UUID);
	if (product_uuid) {
		ret = lttng_metadata_printf(session,
				"	product_uuid = \"%s\";\n",
				product_uuid
				);
		if (ret)
			goto end;
	}

	/* Close the 'env' section */
	ret = lttng_metadata_printf(session, "};\n\n");
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
	list_for_each_entry(chan_priv, &session->priv->chan_head, node) {
		struct lttng_kernel_channel_buffer_private *chan_buf_priv;

		if (chan_priv->pub->type != LTTNG_KERNEL_CHANNEL_TYPE_BUFFER)
			continue;
		chan_buf_priv = container_of(chan_priv, struct lttng_kernel_channel_buffer_private, parent);
		ret = _lttng_channel_metadata_statedump(session, chan_buf_priv->pub);
		if (ret)
			goto end;
	}

	list_for_each_entry(event_recorder_priv, &session->priv->events_head, parent.parent.node) {
		ret = _lttng_event_recorder_metadata_statedump(&event_recorder_priv->pub->parent);
		if (ret)
			goto end;
	}
	session->priv->metadata_dumped = 1;
end:
	lttng_metadata_end(session);
	return ret;
}

/**
 * lttng_transport_register - LTT transport registration
 * @transport: transport structure
 *
 * Registers a transport which can be used as output to extract the data out of
 * LTTng. The module calling this registration function must ensure that no
 * trap-inducing code will be executed by the transport functions. E.g.
 * vmalloc_sync_mappings() must be called between a vmalloc and the moment the memory
 * is made visible to the transport function. This registration acts as a
 * vmalloc_sync_mappings. Therefore, only if the module allocates virtual memory
 * after its registration must it synchronize the TLBs.
 */
void lttng_transport_register(struct lttng_transport *transport)
{
	/*
	 * Make sure no page fault can be triggered by the module about to be
	 * registered. We deal with this here so we don't have to call
	 * vmalloc_sync_mappings() in each module's init.
	 */
	wrapper_vmalloc_sync_mappings();

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

void lttng_counter_transport_register(struct lttng_counter_transport *transport)
{
	/*
	 * Make sure no page fault can be triggered by the module about to be
	 * registered. We deal with this here so we don't have to call
	 * vmalloc_sync_mappings() in each module's init.
	 */
	wrapper_vmalloc_sync_mappings();

	mutex_lock(&sessions_mutex);
	list_add_tail(&transport->node, &lttng_counter_transport_list);
	mutex_unlock(&sessions_mutex);
}
EXPORT_SYMBOL_GPL(lttng_counter_transport_register);

void lttng_counter_transport_unregister(struct lttng_counter_transport *transport)
{
	mutex_lock(&sessions_mutex);
	list_del(&transport->node);
	mutex_unlock(&sessions_mutex);
}
EXPORT_SYMBOL_GPL(lttng_counter_transport_unregister);

struct lttng_kernel_channel_buffer *lttng_kernel_alloc_channel_buffer(void)
{
	struct lttng_kernel_channel_buffer *lttng_chan_buf;
	struct lttng_kernel_channel_common *lttng_chan_common;
	struct lttng_kernel_channel_buffer_private *lttng_chan_buf_priv;

	lttng_chan_buf = kzalloc(sizeof(struct lttng_kernel_channel_buffer), GFP_KERNEL);
	if (!lttng_chan_buf)
		goto nomem;
	lttng_chan_buf_priv = kzalloc(sizeof(struct lttng_kernel_channel_buffer_private), GFP_KERNEL);
	if (!lttng_chan_buf_priv)
		goto nomem_priv;
	lttng_chan_common = &lttng_chan_buf->parent;
	lttng_chan_common->type = LTTNG_KERNEL_CHANNEL_TYPE_BUFFER;
	lttng_chan_buf->priv = lttng_chan_buf_priv;
	lttng_chan_common->priv = &lttng_chan_buf_priv->parent;
	lttng_chan_buf_priv->pub = lttng_chan_buf;
	lttng_chan_buf_priv->parent.pub = lttng_chan_common;
	return lttng_chan_buf;

nomem_priv:
	kfree(lttng_chan_buf);
nomem:
	return NULL;
}
EXPORT_SYMBOL_GPL(lttng_kernel_alloc_channel_buffer);

struct lttng_kernel_channel_counter *lttng_kernel_alloc_channel_counter(void)
{
	struct lttng_kernel_channel_counter *lttng_chan_counter;
	struct lttng_kernel_channel_common *lttng_chan_common;
	struct lttng_kernel_channel_counter_private *lttng_chan_counter_priv;

	lttng_chan_counter = kzalloc(sizeof(struct lttng_kernel_channel_counter), GFP_KERNEL);
	if (!lttng_chan_counter)
		goto nomem;
	lttng_chan_counter_priv = kzalloc(sizeof(struct lttng_kernel_channel_counter_private), GFP_KERNEL);
	if (!lttng_chan_counter_priv)
		goto nomem_priv;
	lttng_chan_common = &lttng_chan_counter->parent;
	lttng_chan_common->type = LTTNG_KERNEL_CHANNEL_TYPE_COUNTER;
	lttng_chan_counter->priv = lttng_chan_counter_priv;
	lttng_chan_common->priv = &lttng_chan_counter_priv->parent;
	lttng_chan_counter_priv->pub = lttng_chan_counter;
	lttng_chan_counter_priv->parent.pub = lttng_chan_common;
	return lttng_chan_counter;

nomem_priv:
	kfree(lttng_chan_counter);
nomem:
	return NULL;
}
EXPORT_SYMBOL_GPL(lttng_kernel_alloc_channel_counter);

void lttng_kernel_free_channel_common(struct lttng_kernel_channel_common *chan)
{
	switch (chan->type) {
	case LTTNG_KERNEL_CHANNEL_TYPE_BUFFER:
	{
		struct lttng_kernel_channel_buffer *chan_buf = container_of(chan,
				struct lttng_kernel_channel_buffer, parent);

		kfree(chan_buf->priv);
		kfree(chan_buf);
		break;
	}
	case LTTNG_KERNEL_CHANNEL_TYPE_COUNTER:
	{
		struct lttng_kernel_channel_counter *chan_counter = container_of(chan,
				struct lttng_kernel_channel_counter, parent);

		kfree(chan_counter->priv);
		kfree(chan_counter);
		break;
	}
	default:
		WARN_ON_ONCE(1);
	}
}
EXPORT_SYMBOL_GPL(lttng_kernel_free_channel_common);

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0))

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

#else /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0)) */
static int lttng_init_cpu_hotplug(void)
{
	return 0;
}
static void lttng_exit_cpu_hotplug(void)
{
}
#endif /* #else #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0)) */

static int __init lttng_events_init(void)
{
	int ret;

	ret = wrapper_get_pfnblock_flags_mask_init();
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
	event_recorder_cache = KMEM_CACHE(lttng_kernel_event_recorder, 0);
	if (!event_recorder_cache) {
		ret = -ENOMEM;
		goto error_kmem_event_recorder;
	}
	event_recorder_private_cache = KMEM_CACHE(lttng_kernel_event_recorder_private, 0);
	if (!event_recorder_private_cache) {
		ret = -ENOMEM;
		goto error_kmem_event_recorder_private;
	}
	event_counter_cache = KMEM_CACHE(lttng_kernel_event_counter, 0);
	if (!event_counter_cache) {
		ret = -ENOMEM;
		goto error_kmem_event_counter;
	}
	event_counter_private_cache = KMEM_CACHE(lttng_kernel_event_counter_private, 0);
	if (!event_counter_private_cache) {
		ret = -ENOMEM;
		goto error_kmem_event_counter_private;
	}
	event_notifier_cache = KMEM_CACHE(lttng_kernel_event_notifier, 0);
	if (!event_notifier_cache) {
		ret = -ENOMEM;
		goto error_kmem_event_notifier;
	}
	event_notifier_private_cache = KMEM_CACHE(lttng_kernel_event_notifier_private, 0);
	if (!event_notifier_private_cache) {
		ret = -ENOMEM;
		goto error_kmem_event_notifier_private;
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
#ifdef CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM
	printk(KERN_NOTICE "LTTng: Experimental bitwise enum enabled.\n");
#endif /* CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM */
	return 0;

error_hotplug:
	lttng_logger_exit();
error_logger:
	lttng_abi_exit();
error_abi:
	kmem_cache_destroy(event_notifier_private_cache);
error_kmem_event_notifier_private:
	kmem_cache_destroy(event_notifier_cache);
error_kmem_event_notifier:
	kmem_cache_destroy(event_counter_private_cache);
error_kmem_event_counter_private:
	kmem_cache_destroy(event_counter_cache);
error_kmem_event_counter:
	kmem_cache_destroy(event_recorder_private_cache);
error_kmem_event_recorder_private:
	kmem_cache_destroy(event_recorder_cache);
error_kmem_event_recorder:
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
	struct lttng_kernel_session_private *session_priv, *tmpsession_priv;

	lttng_exit_cpu_hotplug();
	lttng_logger_exit();
	lttng_abi_exit();
	list_for_each_entry_safe(session_priv, tmpsession_priv, &sessions, node)
		lttng_session_destroy(session_priv->pub);
	kmem_cache_destroy(event_recorder_cache);
	kmem_cache_destroy(event_recorder_private_cache);
	kmem_cache_destroy(event_counter_cache);
	kmem_cache_destroy(event_counter_private_cache);
	kmem_cache_destroy(event_notifier_cache);
	kmem_cache_destroy(event_notifier_private_cache);
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

#include <generated/patches.h>
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
