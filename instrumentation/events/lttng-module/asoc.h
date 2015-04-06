#undef TRACE_SYSTEM
#define TRACE_SYSTEM asoc

#if !defined(LTTNG_TRACE_ASOC_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_ASOC_H

#include "../../../probes/lttng-tracepoint-event.h"
#include <linux/ktime.h>
#include <linux/version.h>

#define DAPM_DIRECT "(direct)"

#ifndef _TRACE_ASOC_DEF
#define _TRACE_ASOC_DEF
struct snd_soc_jack;
struct snd_soc_codec;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0))
struct snd_soc_platform;
#endif
struct snd_soc_card;
struct snd_soc_dapm_widget;
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0))
#define CODEC_NAME_FIELD component.name
#define CODEC_ID_FIELD component.id
#else
#define CODEC_NAME_FIELD name
#define CODEC_ID_FIELD id
#endif

/*
 * Log register events
 */
LTTNG_TRACEPOINT_EVENT_CLASS(asoc_snd_soc_reg,

	TP_PROTO(struct snd_soc_codec *codec, unsigned int reg,
		 unsigned int val),

	TP_ARGS(codec, reg, val),

	TP_FIELDS(
		ctf_string(name, codec->CODEC_NAME_FIELD)
		ctf_integer(int, id, codec->CODEC_ID_FIELD)
		ctf_integer(unsigned int, reg, reg)
		ctf_integer(unsigned int, val, val)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(asoc_snd_soc_reg, snd_soc_reg_write,

	asoc_snd_soc_reg_write,

	TP_PROTO(struct snd_soc_codec *codec, unsigned int reg,
		 unsigned int val),

	TP_ARGS(codec, reg, val)

)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(asoc_snd_soc_reg, snd_soc_reg_read,

	asoc_snd_soc_reg_read,

	TP_PROTO(struct snd_soc_codec *codec, unsigned int reg,
		 unsigned int val),

	TP_ARGS(codec, reg, val)

)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0))
LTTNG_TRACEPOINT_EVENT_CLASS(asoc_snd_soc_preg,

	TP_PROTO(struct snd_soc_platform *platform, unsigned int reg,
		 unsigned int val),

	TP_ARGS(platform, reg, val),

	TP_FIELDS(
		ctf_string(name, platform->CODEC_NAME_FIELD)
		ctf_integer(int, id, platform->CODEC_ID_FIELD)
		ctf_integer(unsigned int, reg, reg)
		ctf_integer(unsigned int, val, val)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(asoc_snd_soc_preg, snd_soc_preg_write,

	asoc_snd_soc_preg_write,

	TP_PROTO(struct snd_soc_platform *platform, unsigned int reg,
		 unsigned int val),

	TP_ARGS(platform, reg, val)

)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(asoc_snd_soc_preg, snd_soc_preg_read,

	asoc_snd_soc_preg_read,

	TP_PROTO(struct snd_soc_platform *platform, unsigned int reg,
		 unsigned int val),

	TP_ARGS(platform, reg, val)

)
#endif

LTTNG_TRACEPOINT_EVENT_CLASS(asoc_snd_soc_card,

	TP_PROTO(struct snd_soc_card *card, int val),

	TP_ARGS(card, val),

	TP_FIELDS(
		ctf_string(name, card->name)
		ctf_integer(int, val, val)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(asoc_snd_soc_card, snd_soc_bias_level_start,

	asoc_snd_soc_bias_level_start,

	TP_PROTO(struct snd_soc_card *card, int val),

	TP_ARGS(card, val)

)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(asoc_snd_soc_card, snd_soc_bias_level_done,

	asoc_snd_soc_bias_level_done,

	TP_PROTO(struct snd_soc_card *card, int val),

	TP_ARGS(card, val)

)

LTTNG_TRACEPOINT_EVENT_CLASS(asoc_snd_soc_dapm_basic,

	TP_PROTO(struct snd_soc_card *card),

	TP_ARGS(card),

	TP_FIELDS(
		ctf_string(name, card->name)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(asoc_snd_soc_dapm_basic, snd_soc_dapm_start,

	asoc_snd_soc_dapm_start,

	TP_PROTO(struct snd_soc_card *card),

	TP_ARGS(card)

)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(asoc_snd_soc_dapm_basic, snd_soc_dapm_done,

	asoc_snd_soc_dapm_done,

	TP_PROTO(struct snd_soc_card *card),

	TP_ARGS(card)

)

LTTNG_TRACEPOINT_EVENT_CLASS(asoc_snd_soc_dapm_widget,

	TP_PROTO(struct snd_soc_dapm_widget *w, int val),

	TP_ARGS(w, val),

	TP_FIELDS(
		ctf_string(name, w->name)
		ctf_integer(int, val, val)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(asoc_snd_soc_dapm_widget, snd_soc_dapm_widget_power,

	asoc_snd_soc_dapm_widget_power,

	TP_PROTO(struct snd_soc_dapm_widget *w, int val),

	TP_ARGS(w, val)

)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(asoc_snd_soc_dapm_widget, snd_soc_dapm_widget_event_start,

	asoc_snd_soc_dapm_widget_event_start,

	TP_PROTO(struct snd_soc_dapm_widget *w, int val),

	TP_ARGS(w, val)

)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(asoc_snd_soc_dapm_widget, snd_soc_dapm_widget_event_done,

	asoc_snd_soc_dapm_widget_event_done,

	TP_PROTO(struct snd_soc_dapm_widget *w, int val),

	TP_ARGS(w, val)

)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0))
LTTNG_TRACEPOINT_EVENT_MAP(snd_soc_dapm_walk_done,

	asoc_snd_soc_dapm_walk_done,

	TP_PROTO(struct snd_soc_card *card),

	TP_ARGS(card),

	TP_FIELDS(
		ctf_string(name, card->name)
		ctf_integer(int, power_checks, card->dapm_stats.power_checks)
		ctf_integer(int, path_checks, card->dapm_stats.path_checks)
		ctf_integer(int, neighbour_checks, card->dapm_stats.neighbour_checks)
	)
)
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,5,0))
LTTNG_TRACEPOINT_EVENT_MAP(snd_soc_dapm_output_path,

	asoc_snd_soc_dapm_output_path,

	TP_PROTO(struct snd_soc_dapm_widget *widget,
		struct snd_soc_dapm_path *path),

	TP_ARGS(widget, path),

	TP_FIELDS(
		ctf_string(wname, widget->name)
		ctf_string(pname, path->name ? path->name : DAPM_DIRECT)
		ctf_string(psname, path->sink->name)
		ctf_integer(int, path_sink, (long) path->sink)
		ctf_integer(int, path_connect, path->connect)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(snd_soc_dapm_input_path,

	asoc_snd_soc_dapm_input_path,

	TP_PROTO(struct snd_soc_dapm_widget *widget,
		struct snd_soc_dapm_path *path),

	TP_ARGS(widget, path),

	TP_FIELDS(
		ctf_string(wname, widget->name)
		ctf_string(pname,path->name ? path->name : DAPM_DIRECT)
		ctf_string(psname, path->source->name)
		ctf_integer(int, path_source, (long) path->source)
		ctf_integer(int, path_connect, path->connect)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(snd_soc_dapm_connected,

	asoc_snd_soc_dapm_connected,

	TP_PROTO(int paths, int stream),

	TP_ARGS(paths, stream),

	TP_FIELDS(
		ctf_integer(int, paths, paths)
		ctf_integer(int, stream, stream)
	)
)
#endif

LTTNG_TRACEPOINT_EVENT_MAP(snd_soc_jack_irq,

	asoc_snd_soc_jack_irq,

	TP_PROTO(const char *name),

	TP_ARGS(name),

	TP_FIELDS(
		ctf_string(name, name)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(snd_soc_jack_report,

	asoc_snd_soc_jack_report,

	TP_PROTO(struct snd_soc_jack *jack, int mask, int val),

	TP_ARGS(jack, mask, val),

	TP_FIELDS(
		ctf_string(name, jack->jack->name)
		ctf_integer(int, mask, mask)
		ctf_integer(int, val, val)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(snd_soc_jack_notify,

	asoc_snd_soc_jack_notify,

	TP_PROTO(struct snd_soc_jack *jack, int val),

	TP_ARGS(jack, val),

	TP_FIELDS(
		ctf_string(name, jack->jack->name)
		ctf_integer(int, val, val)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(snd_soc_cache_sync,

	asoc_snd_soc_cache_sync,

	TP_PROTO(struct snd_soc_codec *codec, const char *type,
		 const char *status),

	TP_ARGS(codec, type, status),

	TP_FIELDS(
		ctf_string(name, codec->CODEC_NAME_FIELD)
		ctf_string(status, status)
		ctf_string(type, type)
		ctf_integer(int, id, codec->CODEC_ID_FIELD)
	)
)

#endif /* LTTNG_TRACE_ASOC_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
