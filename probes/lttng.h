#ifndef _LTTNG_PROBES_LTTNG_H
#define _LTTNG_PROBES_LTTNG_H

#undef is_signed_type
#define is_signed_type(type)		(((type)(-1)) < 0)

#undef PARAMS
#define PARAMS(args...)		args

#endif /* _LTTNG_PROBES_LTTNG_H */
