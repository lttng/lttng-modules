#undef TRACE_SYSTEM
#define TRACE_SYSTEM random

#if !defined(LTTNG_TRACE_RANDOM_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_RANDOM_H

#include "../../../probes/lttng-tracepoint-event.h"
#include <linux/writeback.h>

LTTNG_TRACEPOINT_EVENT_CLASS(random__mix_pool_bytes,
	TP_PROTO(const char *pool_name, int bytes, unsigned long IP),

	TP_ARGS(pool_name, bytes, IP),

	TP_FIELDS(
		ctf_string(pool_name, pool_name)
		ctf_integer(int, bytes, bytes)
		ctf_integer(unsigned long, IP, IP)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(random__mix_pool_bytes, mix_pool_bytes,

	random_mix_pool_bytes,

	TP_PROTO(const char *pool_name, int bytes, unsigned long IP),

	TP_ARGS(pool_name, bytes, IP)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(random__mix_pool_bytes, mix_pool_bytes_nolock,

	random_mix_pool_bytes_nolock,

	TP_PROTO(const char *pool_name, int bytes, unsigned long IP),

	TP_ARGS(pool_name, bytes, IP)
)

LTTNG_TRACEPOINT_EVENT_MAP(credit_entropy_bits,

	random_credit_entropy_bits,

	TP_PROTO(const char *pool_name, int bits, int entropy_count,
		 int entropy_total, unsigned long IP),

	TP_ARGS(pool_name, bits, entropy_count, entropy_total, IP),

	TP_FIELDS(
		ctf_string(pool_name, pool_name)
		ctf_integer(int, bits, bits)
		ctf_integer(int, entropy_count, entropy_count)
		ctf_integer(int, entropy_total, entropy_total)
		ctf_integer(unsigned long, IP, IP)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(get_random_bytes,

	random_get_random_bytes,

	TP_PROTO(int nbytes, unsigned long IP),

	TP_ARGS(nbytes, IP),

	TP_FIELDS(
		ctf_integer(int, nbytes, nbytes)
		ctf_integer(unsigned long, IP, IP)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(random__extract_entropy,
	TP_PROTO(const char *pool_name, int nbytes, int entropy_count,
		 unsigned long IP),

	TP_ARGS(pool_name, nbytes, entropy_count, IP),

	TP_FIELDS(
		ctf_string(pool_name, pool_name)
		ctf_integer(int, nbytes, nbytes)
		ctf_integer(int, entropy_count, entropy_count)
		ctf_integer(unsigned long, IP, IP)
	)
)


LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(random__extract_entropy, extract_entropy,

	random_extract_entropy,

	TP_PROTO(const char *pool_name, int nbytes, int entropy_count,
		 unsigned long IP),

	TP_ARGS(pool_name, nbytes, entropy_count, IP)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(random__extract_entropy, extract_entropy_user,

	random_extract_entropy_user,

	TP_PROTO(const char *pool_name, int nbytes, int entropy_count,
		 unsigned long IP),

	TP_ARGS(pool_name, nbytes, entropy_count, IP)
)



#endif /* LTTNG_TRACE_RANDOM_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
