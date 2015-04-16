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

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,4))
/*
 * Log register events
 */
LTTNG_TRACEPOINT_EVENT_CLASS(regmap_reg,

	TP_PROTO(struct regmap *map, unsigned int reg,
		 unsigned int val),

	TP_ARGS(map, reg, val),

	TP_FIELDS(
		ctf_string(name, regmap_name(map))
		ctf_integer(unsigned int, reg, reg)
		ctf_integer(unsigned int, val, val)
	)
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
#else
/*
 * Log register events
 */
LTTNG_TRACEPOINT_EVENT_CLASS(regmap_reg,

	TP_PROTO(struct device *dev, unsigned int reg,
		 unsigned int val),

	TP_ARGS(dev, reg, val),

	TP_FIELDS(
		ctf_string(name, dev_name(dev))
		ctf_integer(unsigned int, reg, reg)
		ctf_integer(unsigned int, val, val)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_reg, regmap_reg_write,

	TP_PROTO(struct device *dev, unsigned int reg,
		 unsigned int val),

	TP_ARGS(dev, reg, val)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_reg, regmap_reg_read,

	TP_PROTO(struct device *dev, unsigned int reg,
		 unsigned int val),

	TP_ARGS(dev, reg, val)
)
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,4))
LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_reg, regmap_reg_read_cache,

	TP_PROTO(struct regmap *map, unsigned int reg,
		 unsigned int val),

	TP_ARGS(map, reg, val)
)
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_reg, regmap_reg_read_cache,

	TP_PROTO(struct device *dev, unsigned int reg,
		 unsigned int val),

	TP_ARGS(dev, reg, val)
)
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,4))
LTTNG_TRACEPOINT_EVENT_CLASS(regmap_block,

	TP_PROTO(struct regmap *map, unsigned int reg, int count),

	TP_ARGS(map, reg, count),

	TP_FIELDS(
		ctf_string(name, regmap_name(map))
		ctf_integer(unsigned int, reg, reg)
		ctf_integer(int, count, count)
	)
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
#else
LTTNG_TRACEPOINT_EVENT_CLASS(regmap_block,

	TP_PROTO(struct device *dev, unsigned int reg, int count),

	TP_ARGS(dev, reg, count),

	TP_FIELDS(
		ctf_string(name, dev_name(dev))
		ctf_integer(unsigned int, reg, reg)
		ctf_integer(int, count, count)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_block, regmap_hw_read_start,

	TP_PROTO(struct device *dev, unsigned int reg, int count),

	TP_ARGS(dev, reg, count)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_block, regmap_hw_read_done,

	TP_PROTO(struct device *dev, unsigned int reg, int count),

	TP_ARGS(dev, reg, count)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_block, regmap_hw_write_start,

	TP_PROTO(struct device *dev, unsigned int reg, int count),

	TP_ARGS(dev, reg, count)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_block, regmap_hw_write_done,

	TP_PROTO(struct device *dev, unsigned int reg, int count),

	TP_ARGS(dev, reg, count)
)
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,4))
LTTNG_TRACEPOINT_EVENT_MAP(regcache_sync,

	regmap_regcache_sync,

	TP_PROTO(struct regmap *map, const char *type,
		 const char *status),

	TP_ARGS(map, type, status),

	TP_FIELDS(
		ctf_string(name, regmap_name(map))
		ctf_string(status, status)
		ctf_string(type, type)
	)
)
#else
LTTNG_TRACEPOINT_EVENT_MAP(regcache_sync,

	regmap_regcache_sync,

	TP_PROTO(struct device *dev, const char *type,
		 const char *status),

	TP_ARGS(dev, type, status),

	TP_FIELDS(
		ctf_string(name, dev_name(dev))
		ctf_string(status, status)
		ctf_string(type, type)
	)
)
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,4))
LTTNG_TRACEPOINT_EVENT_CLASS(regmap_bool,

	TP_PROTO(struct regmap *map, bool flag),

	TP_ARGS(map, flag),

	TP_FIELDS(
		ctf_string(name, regmap_name(map))
		ctf_integer(int, flag, flag)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_bool, regmap_cache_only,

	TP_PROTO(struct regmap *map, bool flag),

	TP_ARGS(map, flag)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_bool, regmap_cache_bypass,

	TP_PROTO(struct regmap *map, bool flag),

	TP_ARGS(map, flag)

)
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
LTTNG_TRACEPOINT_EVENT_CLASS(regmap_bool,

	TP_PROTO(struct device *dev, bool flag),

	TP_ARGS(dev, flag),

	TP_FIELDS(
		ctf_string(name, dev_name(dev))
		ctf_integer(int, flag, flag)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_bool, regmap_cache_only,

	TP_PROTO(struct device *dev, bool flag),

	TP_ARGS(dev, flag)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regmap_bool, regmap_cache_bypass,

	TP_PROTO(struct device *dev, bool flag),

	TP_ARGS(dev, flag)
)
#endif

#endif /* LTTNG_TRACE_REGMAP_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
