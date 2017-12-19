#undef TRACE_SYSTEM
#define TRACE_SYSTEM i2c

#if !defined(LTTNG_TRACE_I2C_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_I2C_H

#include <probes/lttng-tracepoint-event.h>

/*
 * __i2c_transfer() write request
 */
LTTNG_TRACEPOINT_EVENT_CODE(i2c_write,

	TP_PROTO(const struct i2c_adapter *adap, const struct i2c_msg *msg,
		int num),

	TP_ARGS(adap, msg, num),

	TP_locvar(
		int extract_sensitive_payload;
	),

	TP_code_pre(
		tp_locvar->extract_sensitive_payload =
			READ_ONCE(extract_sensitive_payload);
	),

	TP_FIELDS(
		ctf_integer(int, adapter_nr, adap->nr)
		ctf_integer(__u16, msg_nr, num)
		ctf_integer(__u16, addr, msg->addr)
		ctf_integer(__u16, flags, msg->flags)
		ctf_integer(__u16, len, msg->len)
		ctf_sequence_hex(__u8, buf,
				tp_locvar->extract_sensitive_payload ?
				msg->buf : NULL,
				__u16,
				tp_locvar->extract_sensitive_payload ?
				msg->len : 0)
	),

	TP_code_post()
)

/*
 * __i2c_transfer() read request
 */
LTTNG_TRACEPOINT_EVENT(i2c_read,

	TP_PROTO(const struct i2c_adapter *adap, const struct i2c_msg *msg,
		int num),

	TP_ARGS(adap, msg, num),

	TP_FIELDS(
		ctf_integer(int, adapter_nr, adap->nr)
		ctf_integer(__u16, msg_nr, num)
		ctf_integer(__u16, addr, msg->addr)
		ctf_integer(__u16, flags, msg->flags)
		ctf_integer(__u16, len, msg->len)
	)
)

/*
 * __i2c_transfer() read reply
 */
LTTNG_TRACEPOINT_EVENT_CODE(i2c_reply,

	TP_PROTO(const struct i2c_adapter *adap, const struct i2c_msg *msg,
		int num),

	TP_ARGS(adap, msg, num),

	TP_locvar(
		int extract_sensitive_payload;
	),

	TP_code_pre(
		tp_locvar->extract_sensitive_payload =
			READ_ONCE(extract_sensitive_payload);
	),

	TP_FIELDS(
		ctf_integer(int, adapter_nr, adap->nr)
		ctf_integer(__u16, msg_nr, num)
		ctf_integer(__u16, addr, msg->addr)
		ctf_integer(__u16, flags, msg->flags)
		ctf_integer(__u16, len, msg->len)
		ctf_sequence_hex(__u8, buf,
				tp_locvar->extract_sensitive_payload ?
				msg->buf : NULL,
				__u16,
				tp_locvar->extract_sensitive_payload ?
				msg->len : 0)
	),

	TP_code_post()
)

/*
 * __i2c_transfer() result
 */
LTTNG_TRACEPOINT_EVENT(i2c_result,

	TP_PROTO(const struct i2c_adapter *adap, int num, int ret),

	TP_ARGS(adap, num, ret),

	TP_FIELDS(
		ctf_integer(int, adapter_nr, adap->nr)
		ctf_integer(__u16, nr_msgs, num)
		ctf_integer(__s16, ret, ret)
	)
)

#endif /*  LTTNG_TRACE_I2C_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
