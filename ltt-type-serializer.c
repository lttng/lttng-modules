/**
 * ltt-type-serializer.c
 *
 * LTTng specialized type serializer.
 *
 * Copyright Mathieu Desnoyers, 2008.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */
#include <linux/module.h>

#include "ltt-type-serializer.h"
#include "ltt-relay-lockless.h"

notrace
void _ltt_specialized_trace(const struct marker *mdata, void *probe_data,
		void *serialize_private, unsigned int data_size,
		unsigned int largest_align)
{
	int ret;
	uint16_t eID;
	size_t slot_size;
	unsigned int chan_index;
	struct ltt_chanbuf *buf;
	struct ltt_chan *chan;
	struct ltt_trace *trace;
	uint64_t tsc;
	long buf_offset;
	int cpu;
	unsigned int rflags;

	/*
	 * If we get here, it's probably because we have useful work to do.
	 */
	if (unlikely(ltt_traces.num_active_traces == 0))
		return;

	rcu_read_lock_sched_notrace();
	cpu = smp_processor_id();
	__get_cpu_var(ltt_nesting)++;
	/*
	 * asm volatile and "memory" clobber prevent the compiler from moving
	 * instructions out of the ltt nesting count. This is required to ensure
	 * that probe side-effects which can cause recursion (e.g. unforeseen
	 * traps, divisions by 0, ...) are triggered within the incremented
	 * nesting count section.
	 */
	barrier();
	eID = mdata->event_id;
	chan_index = mdata->channel_id;

	/*
	 * Iterate on each trace, typically small number of active traces,
	 * list iteration with prefetch is usually slower.
	 */
	__list_for_each_entry_rcu(trace, &ltt_traces.head, list) {
		if (unlikely(!trace->active))
			continue;
		if (unlikely(!ltt_run_filter(trace, eID)))
			continue;
#ifdef LTT_DEBUG_EVENT_SIZE
		rflags = LTT_RFLAG_ID_SIZE;
#else
		if (unlikely(eID >= LTT_FREE_EVENTS))
			rflags = LTT_RFLAG_ID;
		else
			rflags = 0;
#endif
		/*
		 * Skip channels added after trace creation.
		 */
		if (unlikely(chan_index >= trace->nr_channels))
			continue;
		chan = &trace->channels[chan_index];
		if (!chan->active)
			continue;

		/* reserve space : header and data */
		ret = ltt_reserve_slot(chan, trace, data_size, largest_align,
				       cpu, &buf, &slot_size, &buf_offset, &tsc,
				       &rflags);
		if (unlikely(ret < 0))
			continue; /* buffer full */

		/* Out-of-order write : header and data */
		buf_offset = ltt_write_event_header(&buf->a, &chan->a,
						    buf_offset, eID, data_size,
						    tsc, rflags);
		if (data_size) {
			buf_offset += ltt_align(buf_offset, largest_align);
			ltt_relay_write(&buf->a, &chan->a, buf_offset,
					serialize_private, data_size);
			buf_offset += data_size;
		}
		/* Out-of-order commit */
		ltt_commit_slot(buf, chan, buf_offset, data_size, slot_size);
	}
	/*
	 * asm volatile and "memory" clobber prevent the compiler from moving
	 * instructions out of the ltt nesting count. This is required to ensure
	 * that probe side-effects which can cause recursion (e.g. unforeseen
	 * traps, divisions by 0, ...) are triggered within the incremented
	 * nesting count section.
	 */
	barrier();
	__get_cpu_var(ltt_nesting)--;
	rcu_read_unlock_sched_notrace();
}
EXPORT_SYMBOL_GPL(_ltt_specialized_trace);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("LTT type serializer");
