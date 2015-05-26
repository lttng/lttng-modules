#undef TRACE_SYSTEM
#define TRACE_SYSTEM regmap

#if !defined(LTTNG_TRACE_REGMAP_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_REGMAP_H

#include "../../../probes/lttng-tracepoint-event.h"
#include <linux/ktime.h>
#include <linux/version.h>

#ifndef _TRACE_REGMAP_DEF_
#define _TRACE_REGMAP_DEF_
struct device;
struct regmap;
#endif

/*
 * Log register events
 */
LTTNG_TRACEPOINT_EVENT_CLASS(regmap_reg,

	TP_PROTO(struct regmap *map, unsigned int reg,
		 unsigned int val),

	TP_ARGS(map, reg, val),

	TP_STRUCT__entry(
		__string(	name,		regmap_name(map))
		__field(	unsigned int,	reg		)
		__field(	unsigned int,	val		)
	),

	TP_fast_assign(
		tp_strcpy(name, regmap_name(map))
		tp_assign(reg, reg)
		tp_assign(val, val)
	),

	TP_printk("%s reg=%x val=%x", __get_str(name),
		  (unsigned int)__entry->reg,
		  (unsigned int)__entry->val)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_reg, regmap_reg_write,

	TP_PROTO(struct regmap *map, unsigned int reg,
		 unsigned int val),

	TP_ARGS(map, reg, val)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_reg, regmap_reg_read,

	TP_PROTO(struct regmap *map, unsigned int reg,
		 unsigned int val),

	TP_ARGS(map, reg, val)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_reg, regmap_reg_read_cache,

	TP_PROTO(struct regmap *map, unsigned int reg,
		 unsigned int val),

	TP_ARGS(map, reg, val)
)

LTTNG_TRACEPOINT_EVENT_CLASS(regmap_block,

	TP_PROTO(struct regmap *map, unsigned int reg, int count),

	TP_ARGS(map, reg, count),

	TP_STRUCT__entry(
		__string(	name,		regmap_name(map))
		__field(	unsigned int,	reg		)
		__field(	int,		count		)
	),

	TP_fast_assign(
		tp_strcpy(name, regmap_name(map))
		tp_assign(reg, reg)
		tp_assign(count, count)
	),

	TP_printk("%s reg=%x count=%d", __get_str(name),
		  (unsigned int)__entry->reg,
		  (int)__entry->count)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_block, regmap_hw_read_start,

	TP_PROTO(struct regmap *map, unsigned int reg, int count),

	TP_ARGS(map, reg, count)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_block, regmap_hw_read_done,

	TP_PROTO(struct regmap *map, unsigned int reg, int count),

	TP_ARGS(map, reg, count)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_block, regmap_hw_write_start,

	TP_PROTO(struct regmap *map, unsigned int reg, int count),

	TP_ARGS(map, reg, count)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_block, regmap_hw_write_done,

	TP_PROTO(struct regmap *map, unsigned int reg, int count),

	TP_ARGS(map, reg, count)
)

LTTNG_TRACEPOINT_EVENT(regcache_sync,

	TP_PROTO(struct regmap *map, const char *type,
		 const char *status),

	TP_ARGS(map, type, status),

	TP_STRUCT__entry(
		__string(       name,           regmap_name(map))
		__string(	status,		status		)
		__string(	type,		type		)
	),

	TP_fast_assign(
		tp_strcpy(name, regmap_name(map))
		tp_strcpy(status, status)
		tp_strcpy(type, type)
	),

	TP_printk("%s type=%s status=%s", __get_str(name),
		  __get_str(type), __get_str(status))
)

LTTNG_TRACEPOINT_EVENT_CLASS(regmap_bool,

	TP_PROTO(struct regmap *map, bool flag),

	TP_ARGS(map, flag),

	TP_STRUCT__entry(
		__string(	name,		regmap_name(map))
		__field(	int,		flag		)
	),

	TP_fast_assign(
		tp_strcpy(name, regmap_name(map))
		tp_assign(flag, flag)
	),

	TP_printk("%s flag=%d", __get_str(name),
		  (int)__entry->flag)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_bool, regmap_cache_only,

	TP_PROTO(struct regmap *map, bool flag),

	TP_ARGS(map, flag)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_bool, regmap_cache_bypass,

	TP_PROTO(struct regmap *map, bool flag),

	TP_ARGS(map, flag)
)

#endif /* LTTNG_TRACE_REGMAP_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
