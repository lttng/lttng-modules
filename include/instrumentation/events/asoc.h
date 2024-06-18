// SPDX-FileCopyrightText: 2012 Paul Woegerer <paul_woegerer@mentor.com>
//
// SPDX-License-Identifier: GPL-2.0-only

#undef TRACE_SYSTEM
#define TRACE_SYSTEM asoc

#if !defined(LTTNG_TRACE_ASOC_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_ASOC_H

#include <lttng/tracepoint-event.h>
#include <linux/ktime.h>
#include <lttng/kernel-version.h>

#define DAPM_DIRECT "(direct)"
#define DAPM_COMPONENT_NONE "(none)"

#ifndef _TRACE_ASOC_DEF
#define _TRACE_ASOC_DEF
struct snd_soc_jack;
struct snd_soc_card;
struct snd_soc_dapm_widget;
struct snd_soc_dapm_path;
#endif

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,9,0))
LTTNG_TRACEPOINT_EVENT_CLASS(asoc_snd_soc_dapm_context,

	TP_PROTO(struct snd_soc_dapm_context *dapm, int val),

	TP_ARGS(dapm, val),

	TP_FIELDS(
		ctf_string(name, dapm->card->name)
		ctf_string(component, dapm->component ? dapm->component->name : DAPM_COMPONENT_NONE)
		ctf_integer(int, val, val)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(asoc_snd_soc_dapm_context, snd_soc_bias_level_start,

	asoc_snd_soc_bias_level_start,

	TP_PROTO(struct snd_soc_dapm_context *dapm, int val),

	TP_ARGS(dapm, val)

)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(asoc_snd_soc_dapm_context, snd_soc_bias_level_done,

	asoc_snd_soc_bias_level_done,

	TP_PROTO(struct snd_soc_dapm_context *dapm, int val),

	TP_ARGS(dapm, val)

)
#else
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
#endif

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,9,0))
LTTNG_TRACEPOINT_EVENT_CLASS(asoc_snd_soc_dapm_basic,

	TP_PROTO(struct snd_soc_card *card, int event),

	TP_ARGS(card, event),

	TP_FIELDS(
		ctf_string(name, card->name)
		ctf_integer(int, event, event)
	)
)
LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(asoc_snd_soc_dapm_basic, snd_soc_dapm_start,

	asoc_snd_soc_dapm_start,

	TP_PROTO(struct snd_soc_card *card, int event),

	TP_ARGS(card, event)

)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(asoc_snd_soc_dapm_basic, snd_soc_dapm_done,

	asoc_snd_soc_dapm_done,

	TP_PROTO(struct snd_soc_card *card, int event),

	TP_ARGS(card, event)

)
#else
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
#endif

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

LTTNG_TRACEPOINT_EVENT_MAP(snd_soc_dapm_path,

	asoc_snd_soc_dapm_path,

	TP_PROTO(struct snd_soc_dapm_widget *widget,
		enum snd_soc_dapm_direction dir,
		struct snd_soc_dapm_path *path),

	TP_ARGS(widget, dir, path),

	TP_FIELDS(
		ctf_string(wname, widget->name)
		ctf_string(pname, path->name ? path->name : DAPM_DIRECT)
		ctf_string(pnname, path->node[dir]->name)
		ctf_integer(int, path_node, (long) path->node[dir])
		ctf_integer(int, path_connect, path->connect)
		ctf_integer(int, path_dir, dir)
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
		ctf_string(name, jack->jack->id)
		ctf_integer(int, mask, mask)
		ctf_integer(int, val, val)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(snd_soc_jack_notify,

	asoc_snd_soc_jack_notify,

	TP_PROTO(struct snd_soc_jack *jack, int val),

	TP_ARGS(jack, val),

	TP_FIELDS(
		ctf_string(name, jack->jack->id)
		ctf_integer(int, val, val)
	)
)

#endif /* LTTNG_TRACE_ASOC_H */

/* This part must be outside protection */
#include <lttng/define_trace.h>
