/*
 * ltt-probes.c
 *
 * Copyright 2010 (c) - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * Holds LTTng probes registry.
 */

#include <linux/module.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/slab.h>

struct ltt_probe {
	const char *name;
	void *cb;
	struct list_head list;
};

static LIST_HEAD(probe_list);
static DEFINE_MUTEX(probe_mutex);
static struct kmem_cache *probe_cache;

static void *find_probe(const char *name)
{
	struct ltt_probe *probe;

	list_for_each_entry(probe, &probe_list, list) {
		if (!strcmp(probe->name, name))
			return probe->cb;
	}
	return NULL;
}

int ltt_probe_register(const char *name, void *cb)
{
	struct ltt_probe *probe;
	int ret = 0;

	if (!cb)
		return -EPERM;

	mutex_lock(&probe_mutex);
	if (find_probe(name)) {
		ret = -EEXIST;
		goto end;
	}
	probe = kmem_cache_zalloc(probe_cache, GFP_KERNEL);
	if (!probe) {
		ret = -ENOMEM;
		goto end;
	}
	probe->name = name;
	probe->cb = cb;
	list_add(&probe->list, &probe_list);
end:
	mutex_unlock(&probe_mutex);
	return ret;
}
EXPORT_SYMBOL_GPL(ltt_probe_register);

void ltt_probe_unregister(const char *name)
{
	struct ltt_probe *probe;

	mutex_lock(&probe_mutex);
	probe = find_probe(name);
	WARN_ON_ONCE(!probe);
	list_del(&probe->list);
	mutex_unlock(&probe_mutex);
	kmem_cache_free(probe_cache, probe);
}
EXPORT_SYMBOL_GPL(ltt_probe_unregister);

void *ltt_probe_get(const char *name)
{
	struct ltt_probe *probe;
	void *cb = NULL;
	int ret;

	mutex_lock(&probe_mutex);
	probe = find_probe(name);
	if (!probe)
		goto end;
	cb = probe->cb;
	ret = try_module_get(__module_text_address((unsigned long) cb));
	WARN_ON_ONCE(!ret);
end:
	mutex_unlock(&probe_mutex);
	return cb;
}
EXPORT_SYMBOL_GPL(ltt_probe_get);

void ltt_probe_put(void *cb)
{
	module_put(__module_text_address((unsigned long) cb));
}
EXPORT_SYMBOL_GPL(ltt_probe_put);

int __init ltt_probes_init(void)
{
	probe_cache = KMEM_CACHE(ltt_probe, 0);
	if (!probe_cache)
		return -ENOMEM;
	return 0;
}

void __exit ltt_probes_exit(void)
{
	kmem_cache_destroy(probe_cache);
}
