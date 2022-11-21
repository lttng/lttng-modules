/* SPDX-License-Identifier: GPL-2.0-only */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM random

#if !defined(LTTNG_TRACE_RANDOM_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_RANDOM_H

#include <lttng/tracepoint-event.h>
#include <linux/writeback.h>

LTTNG_TRACEPOINT_EVENT_MAP(add_device_randomness,

	random_add_device_randomness,

	TP_PROTO(int bytes, unsigned long IP),

	TP_ARGS(bytes, IP),

	TP_FIELDS(
		ctf_integer(int, bytes, bytes)
		ctf_integer_hex(unsigned long, IP, IP)
	)
)

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,17,0))
LTTNG_TRACEPOINT_EVENT_CLASS(random__mix_pool_bytes,
	TP_PROTO(int bytes, unsigned long IP),

	TP_ARGS(bytes, IP),

	TP_FIELDS(
		ctf_integer(int, bytes, bytes)
		ctf_integer_hex(unsigned long, IP, IP)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(random__mix_pool_bytes, mix_pool_bytes,

	random_mix_pool_bytes,

	TP_PROTO(int bytes, unsigned long IP),

	TP_ARGS(bytes, IP)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(random__mix_pool_bytes, mix_pool_bytes_nolock,

	random_mix_pool_bytes_nolock,

	TP_PROTO(int bytes, unsigned long IP),

	TP_ARGS(bytes, IP)
)
#else
LTTNG_TRACEPOINT_EVENT_CLASS(random__mix_pool_bytes,
	TP_PROTO(const char *pool_name, int bytes, unsigned long IP),

	TP_ARGS(pool_name, bytes, IP),

	TP_FIELDS(
		ctf_string(pool_name, pool_name)
		ctf_integer(int, bytes, bytes)
		ctf_integer_hex(unsigned long, IP, IP)
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
#endif

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,17,0))
LTTNG_TRACEPOINT_EVENT_MAP(credit_entropy_bits,

	random_credit_entropy_bits,

	TP_PROTO(int bits, int entropy_count, unsigned long IP),

	TP_ARGS(bits, entropy_count, IP),

	TP_FIELDS(
		ctf_integer(int, bits, bits)
		ctf_integer(int, entropy_count, entropy_count)
		ctf_integer_hex(unsigned long, IP, IP)
	)
)
#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,2,0))
LTTNG_TRACEPOINT_EVENT_MAP(credit_entropy_bits,

	random_credit_entropy_bits,

	TP_PROTO(const char *pool_name, int bits, int entropy_count,
		 unsigned long IP),

	TP_ARGS(pool_name, bits, entropy_count, IP),

	TP_FIELDS(
		ctf_string(pool_name, pool_name)
		ctf_integer(int, bits, bits)
		ctf_integer(int, entropy_count, entropy_count)
		ctf_integer_hex(unsigned long, IP, IP)
	)
)
#else
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
		ctf_integer_hex(unsigned long, IP, IP)
	)
)
#endif

LTTNG_TRACEPOINT_EVENT_MAP(push_to_pool,

	random_push_to_pool,

	TP_PROTO(const char *pool_name, int pool_bits, int input_bits),

	TP_ARGS(pool_name, pool_bits, input_bits),

	TP_FIELDS(
		ctf_string(pool_name, pool_name)
		ctf_integer(int, pool_bits, pool_bits)
		ctf_integer(int, input_bits, input_bits)
	)
)

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,17,0))
LTTNG_TRACEPOINT_EVENT_MAP(debit_entropy,

	random_debit_entropy,

	TP_PROTO(int debit_bits),

	TP_ARGS(debit_bits),

	TP_FIELDS(
		ctf_integer(int, debit_bits, debit_bits)
	)
)
#else
LTTNG_TRACEPOINT_EVENT_MAP(debit_entropy,

	random_debit_entropy,

	TP_PROTO(const char *pool_name, int debit_bits),

	TP_ARGS(pool_name, debit_bits),

	TP_FIELDS(
		ctf_string(pool_name, pool_name)
		ctf_integer(int, debit_bits, debit_bits)
	)
)
#endif

LTTNG_TRACEPOINT_EVENT_MAP(add_input_randomness,

	random_add_input_randomness,

	TP_PROTO(int input_bits),

	TP_ARGS(input_bits),

	TP_FIELDS(
		ctf_integer(int, input_bits, input_bits)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(add_disk_randomness,

	random_add_disk_randomness,

	TP_PROTO(dev_t dev, int input_bits),

	TP_ARGS(dev, input_bits),

	TP_FIELDS(
		ctf_integer(dev_t, dev, dev)
		ctf_integer(int, input_bits, input_bits)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(xfer_secondary_pool,

	random_xfer_secondary_pool,

	TP_PROTO(const char *pool_name, int xfer_bits, int request_bits,
		int pool_entropy, int input_entropy),

	TP_ARGS(pool_name, xfer_bits, request_bits, pool_entropy,
		input_entropy),

	TP_FIELDS(
		ctf_string(pool_name, pool_name)
		ctf_integer(int, xfer_bits, xfer_bits)
		ctf_integer(int, request_bits, request_bits)
		ctf_integer(int, pool_entropy, pool_entropy)
		ctf_integer(int, input_entropy, input_entropy)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(random__get_random_bytes,

	TP_PROTO(int nbytes, unsigned long IP),

	TP_ARGS(nbytes, IP),

	TP_FIELDS(
		ctf_integer(int, nbytes, nbytes)
		ctf_integer_hex(unsigned long, IP, IP)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(random__get_random_bytes, get_random_bytes,

	random_get_random_bytes,

	TP_PROTO(int nbytes, unsigned long IP),

	TP_ARGS(nbytes, IP)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(random__get_random_bytes, get_random_bytes_arch,

	random_get_random_bytes_arch,

	TP_PROTO(int nbytes, unsigned long IP),

	TP_ARGS(nbytes, IP)
)

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,17,0))
LTTNG_TRACEPOINT_EVENT_CLASS(random__extract_entropy,
	TP_PROTO(int nbytes, int entropy_count, unsigned long IP),

	TP_ARGS(nbytes, entropy_count, IP),

	TP_FIELDS(
		ctf_integer(int, nbytes, nbytes)
		ctf_integer(int, entropy_count, entropy_count)
		ctf_integer_hex(unsigned long, IP, IP)
	)
)


LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(random__extract_entropy, extract_entropy,

	random_extract_entropy,

	TP_PROTO(int nbytes, int entropy_count, unsigned long IP),

	TP_ARGS(nbytes, entropy_count, IP)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(random__extract_entropy, extract_entropy_user,

	random_extract_entropy_user,

	TP_PROTO(int nbytes, int entropy_count, unsigned long IP),

	TP_ARGS(nbytes, entropy_count, IP)
)
#else
LTTNG_TRACEPOINT_EVENT_CLASS(random__extract_entropy,
	TP_PROTO(const char *pool_name, int nbytes, int entropy_count,
		 unsigned long IP),

	TP_ARGS(pool_name, nbytes, entropy_count, IP),

	TP_FIELDS(
		ctf_string(pool_name, pool_name)
		ctf_integer(int, nbytes, nbytes)
		ctf_integer(int, entropy_count, entropy_count)
		ctf_integer_hex(unsigned long, IP, IP)
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
#endif

LTTNG_TRACEPOINT_EVENT_MAP(random_read,

	random_random_read,

	TP_PROTO(int got_bits, int need_bits, int pool_left, int input_left),

	TP_ARGS(got_bits, need_bits, pool_left, input_left),

	TP_FIELDS(
		ctf_integer(int, got_bits, got_bits)
		ctf_integer(int, need_bits, need_bits)
		ctf_integer(int, pool_left, pool_left)
		ctf_integer(int, input_left, input_left)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(urandom_read,

	random_urandom_read,

	TP_PROTO(int got_bits, int pool_left, int input_left),

	TP_ARGS(got_bits, pool_left, input_left),

	TP_FIELDS(
		ctf_integer(int, got_bits, got_bits)
		ctf_integer(int, pool_left, pool_left)
		ctf_integer(int, input_left, input_left)
	)
)

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,9,0))
LTTNG_TRACEPOINT_EVENT_MAP(prandom_u32,

	random_prandom_u32,

	TP_PROTO(unsigned int ret),

	TP_ARGS(ret),

	TP_FIELDS(
		ctf_integer(unsigned int, ret, ret)
	)
)
#endif


#endif /* LTTNG_TRACE_RANDOM_H */

/* This part must be outside protection */
#include <lttng/define_trace.h>
