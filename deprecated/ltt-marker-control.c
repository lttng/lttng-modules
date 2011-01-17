/*
 * Copyright (C) 2007 Mathieu Desnoyers
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <linux/stat.h>
#include <linux/vmalloc.h>
#include <linux/marker.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/seq_file.h>
#include <linux/slab.h>

#include "ltt-tracer.h"

#define DEFAULT_CHANNEL "cpu"
#define DEFAULT_PROBE "default"

LIST_HEAD(probes_list);

/*
 * Mutex protecting the probe slab cache.
 * Nests inside the traces mutex.
 */
DEFINE_MUTEX(probes_mutex);

struct ltt_available_probe default_probe = {
	.name = "default",
	.format = NULL,
	.probe_func = ltt_vtrace,
	.callbacks[0] = ltt_serialize_data,
};

static struct kmem_cache *markers_loaded_cachep;
static LIST_HEAD(markers_loaded_list);
/*
 * List sorted by name strcmp order.
 */
static LIST_HEAD(probes_registered_list);

static struct ltt_available_probe *get_probe_from_name(const char *pname)
{
	struct ltt_available_probe *iter;
	int comparison, found = 0;

	if (!pname)
		pname = DEFAULT_PROBE;
	list_for_each_entry(iter, &probes_registered_list, node) {
		comparison = strcmp(pname, iter->name);
		if (!comparison)
			found = 1;
		if (comparison <= 0)
			break;
	}
	if (found)
		return iter;
	else
		return NULL;
}

int ltt_probe_register(struct ltt_available_probe *pdata)
{
	int ret = 0;
	int comparison;
	struct ltt_available_probe *iter;

	mutex_lock(&probes_mutex);
	list_for_each_entry_reverse(iter, &probes_registered_list, node) {
		comparison = strcmp(pdata->name, iter->name);
		if (!comparison) {
			ret = -EBUSY;
			goto end;
		} else if (comparison > 0) {
			/* We belong to the location right after iter. */
			list_add(&pdata->node, &iter->node);
			goto end;
		}
	}
	/* Should be added at the head of the list */
	list_add(&pdata->node, &probes_registered_list);
end:
	mutex_unlock(&probes_mutex);
	return ret;
}
EXPORT_SYMBOL_GPL(ltt_probe_register);

/*
 * Called when a probe does not want to be called anymore.
 */
int ltt_probe_unregister(struct ltt_available_probe *pdata)
{
	int ret = 0;
	struct ltt_active_marker *amark, *tmp;

	mutex_lock(&probes_mutex);
	list_for_each_entry_safe(amark, tmp, &markers_loaded_list, node) {
		if (amark->probe == pdata) {
			ret = marker_probe_unregister_private_data(
				pdata->probe_func, amark);
			if (ret)
				goto end;
			list_del(&amark->node);
			kmem_cache_free(markers_loaded_cachep, amark);
		}
	}
	list_del(&pdata->node);
end:
	mutex_unlock(&probes_mutex);
	return ret;
}
EXPORT_SYMBOL_GPL(ltt_probe_unregister);

/*
 * Connect marker "mname" to probe "pname".
 * Only allow _only_ probe instance to be connected to a marker.
 */
int ltt_marker_connect(const char *channel, const char *mname,
		       const char *pname)
{
	int ret;
	struct ltt_active_marker *pdata;
	struct ltt_available_probe *probe;

	ltt_lock_traces();
	mutex_lock(&probes_mutex);
	probe = get_probe_from_name(pname);
	if (!probe) {
		ret = -ENOENT;
		goto end;
	}
	pdata = marker_get_private_data(channel, mname, probe->probe_func, 0);
	if (pdata && !IS_ERR(pdata)) {
		ret = -EEXIST;
		goto end;
	}
	pdata = kmem_cache_zalloc(markers_loaded_cachep, GFP_KERNEL);
	if (!pdata) {
		ret = -ENOMEM;
		goto end;
	}
	pdata->probe = probe;
	/*
	 * ID has priority over channel in case of conflict.
	 */
	ret = marker_probe_register(channel, mname, NULL,
		probe->probe_func, pdata);
	if (ret)
		kmem_cache_free(markers_loaded_cachep, pdata);
	else
		list_add(&pdata->node, &markers_loaded_list);
end:
	mutex_unlock(&probes_mutex);
	ltt_unlock_traces();
	return ret;
}
EXPORT_SYMBOL_GPL(ltt_marker_connect);

/*
 * Disconnect marker "mname", probe "pname".
 */
int ltt_marker_disconnect(const char *channel, const char *mname,
			  const char *pname)
{
	struct ltt_active_marker *pdata;
	struct ltt_available_probe *probe;
	int ret = 0;

	mutex_lock(&probes_mutex);
	probe = get_probe_from_name(pname);
	if (!probe) {
		ret = -ENOENT;
		goto end;
	}
	pdata = marker_get_private_data(channel, mname, probe->probe_func, 0);
	if (IS_ERR(pdata)) {
		ret = PTR_ERR(pdata);
		goto end;
	} else if (!pdata) {
		/*
		 * Not registered by us.
		 */
		ret = -EPERM;
		goto end;
	}
	ret = marker_probe_unregister(channel, mname, probe->probe_func, pdata);
	if (ret)
		goto end;
	else {
		list_del(&pdata->node);
		kmem_cache_free(markers_loaded_cachep, pdata);
	}
end:
	mutex_unlock(&probes_mutex);
	return ret;
}
EXPORT_SYMBOL_GPL(ltt_marker_disconnect);

static void disconnect_all_markers(void)
{
	struct ltt_active_marker *pdata, *tmp;

	list_for_each_entry_safe(pdata, tmp, &markers_loaded_list, node) {
		marker_probe_unregister_private_data(pdata->probe->probe_func,
			pdata);
		list_del(&pdata->node);
		kmem_cache_free(markers_loaded_cachep, pdata);
	}
}

static int __init marker_control_init(void)
{
	int ret;

	markers_loaded_cachep = KMEM_CACHE(ltt_active_marker, 0);

	ret = ltt_probe_register(&default_probe);
	BUG_ON(ret);
	ret = ltt_marker_connect("metadata", "core_marker_format",
				 DEFAULT_PROBE);
	BUG_ON(ret);
	ret = ltt_marker_connect("metadata", "core_marker_id", DEFAULT_PROBE);
	BUG_ON(ret);

	return 0;
}
module_init(marker_control_init);

static void __exit marker_control_exit(void)
{
	int ret;

	ret = ltt_marker_disconnect("metadata", "core_marker_format",
				    DEFAULT_PROBE);
	BUG_ON(ret);
	ret = ltt_marker_disconnect("metadata", "core_marker_id",
				    DEFAULT_PROBE);
	BUG_ON(ret);
	ret = ltt_probe_unregister(&default_probe);
	BUG_ON(ret);
	disconnect_all_markers();
	kmem_cache_destroy(markers_loaded_cachep);
	marker_synchronize_unregister();
}
module_exit(marker_control_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("Linux Trace Toolkit Marker Control");
