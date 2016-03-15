#undef TRACE_SYSTEM
#define TRACE_SYSTEM v4l2

#if !defined(LTTNG_TRACE_V4L2_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_V4L2_H

#include <probes/lttng-tracepoint-event.h>

LTTNG_TRACEPOINT_EVENT_CLASS(v4l2_class,

	TP_PROTO(int minor, struct v4l2_buffer *buf),

	TP_ARGS(minor, buf),

	TP_FIELDS(
		ctf_integer(int, minor, minor)
		ctf_integer(u32, index, buf->index)
		ctf_integer(u32, type, buf->type)
		ctf_integer(u32, bytesused, buf->bytesused)
		ctf_integer(u32, flags, buf->flags)
		ctf_integer(u32, field, buf->field)
		ctf_integer(s64, timestamp, timeval_to_ns(&buf->timestamp))
		ctf_integer(u32, timecode_type, buf->timecode.type)
		ctf_integer(u32, timecode_flags, buf->timecode.flags)
		ctf_integer(u8, timecode_frames, buf->timecode.frames)
		ctf_integer(u8, timecode_seconds, buf->timecode.seconds)
		ctf_integer(u8, timecode_minutes, buf->timecode.minutes)
		ctf_integer(u8, timecode_hours, buf->timecode.hours)
		ctf_array(u8, timecode_userbits, buf->timecode.userbits, 4)
		ctf_integer(u32, sequence, buf->sequence)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(v4l2_class,
	v4l2_dqbuf,

	TP_PROTO(int minor, struct v4l2_buffer *buf),

	TP_ARGS(minor, buf)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(v4l2_class,

	v4l2_qbuf,

	TP_PROTO(int minor, struct v4l2_buffer *buf),

	TP_ARGS(minor, buf)
)

#endif /* if !defined(LTTNG_TRACE_V4L2_H) || defined(TRACE_HEADER_MULTI_READ) */

/* This part must be outside protection */
#include <probes/define_trace.h>
