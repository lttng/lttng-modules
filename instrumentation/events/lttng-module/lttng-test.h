#undef TRACE_SYSTEM
#define TRACE_SYSTEM lttng_test

#if !defined(LTTNG_TRACE_LTTNG_TEST_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_LTTNG_TEST_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/types.h>
#include <linux/version.h>

LTTNG_TRACEPOINT_EVENT(lttng_test_filter_event,
	TP_PROTO(int anint, int netint, long *values,
		char *text, size_t textlen,
		char *etext),
	TP_ARGS(anint, netint, values, text, textlen, etext),
	TP_FIELDS(
		ctf_integer(int, intfield, anint)
		ctf_integer_hex(int, intfield2, anint)
		ctf_integer(long, longfield, anint)
		ctf_integer_network(int, netintfield, netint)
		ctf_integer_network_hex(int, netintfieldhex, netint)
		ctf_array(long, arrfield1, values, 3)
		ctf_array_text(char, arrfield2, text, 10)
		ctf_sequence(char, seqfield1, text, size_t, textlen)
		ctf_sequence_text(char, seqfield2, text, size_t, textlen)
		ctf_string(stringfield, text)
		ctf_string(stringfield2, etext)
		ctf_sequence_bitfield(long, bitfield_seq, values, uint8_t, 3)
		ctf_array_bitfield(long, bitfield_array, values, 3)
	)
)

#endif /*  LTTNG_TRACE_LTTNG_TEST_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
