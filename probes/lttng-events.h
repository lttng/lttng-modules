#include <lttng.h>
#include <lttng-types.h>
#include <linux/debugfs.h>
#include "../wrapper/vmalloc.h"	/* for wrapper_vmalloc_sync_all() */
#include "../wrapper/ringbuffer/frontend_types.h"
#include "../ltt-events.h"
#include "../ltt-tracer-core.h"

struct lttng_event_field {
	const char *name;
	const struct lttng_type type;
};

struct lttng_event_desc {
	const struct lttng_event_field *fields;
	const char *name;
	void *probe_callback;
	unsigned int nr_fields;
};

/*
 * Macro declarations used for all stages.
 */

/*
 * DECLARE_EVENT_CLASS can be used to add a generic function
 * handlers for events. That is, if all events have the same
 * parameters and just have distinct trace points.
 * Each tracepoint can be defined with DEFINE_EVENT and that
 * will map the DECLARE_EVENT_CLASS to the tracepoint.
 *
 * TRACE_EVENT is a one to one mapping between tracepoint and template.
 */

#undef TRACE_EVENT
#define TRACE_EVENT(name, proto, args, tstruct, assign, print) \
	DECLARE_EVENT_CLASS(name,			       \
			     PARAMS(proto),		       \
			     PARAMS(args),		       \
			     PARAMS(tstruct),		       \
			     PARAMS(assign),		       \
			     PARAMS(print))		       \
	DEFINE_EVENT(name, name, PARAMS(proto), PARAMS(args))

#undef DEFINE_EVENT_PRINT
#define DEFINE_EVENT_PRINT(template, name, proto, args, print)	\
	DEFINE_EVENT(template, name, PARAMS(proto), PARAMS(args))

/* Callbacks are meaningless to LTTng. */
#undef TRACE_EVENT_FN
#define TRACE_EVENT_FN(name, proto, args, tstruct,			\
		assign, print, reg, unreg)				\
	TRACE_EVENT(name, PARAMS(proto), PARAMS(args),			\
		PARAMS(tstruct), PARAMS(assign), PARAMS(print))		\

/*
 * Stage 1 of the trace events.
 *
 * Create event field type metadata section.
 * Each event produce an array of fields.
 */

#include "lttng-events-reset.h"	/* Reset all macros within TRACE_EVENT */

/* Named field types must be defined in lttng-types.h */

#undef __field
#define __field(_type, _item)					\
	{ .name = #_item, .type = { .atype = atype_integer, .name = #_type} },

#undef __field_ext
#define __field_ext(_type, _item, _filter_type)		__field(_type, _item)

#undef __array
#define __array(_type, _item, _length)				\
	{							\
		.name = #_item,					\
		.type = {					\
		  .atype = atype_array,				\
		  .name = NULL,					\
		  .u.array.elem_type = #_type,			\
		  .u.array.length = _length,			\
		},						\
	},

#undef __dynamic_array
#define __dynamic_array(_type, _item, _length)			\
	{							\
		.name = #_item,					\
		.type = {					\
		  .atype = atype_sequence,			\
		  .name = NULL,					\
		  .u.sequence.elem_type = #_type,		\
		  .u.sequence.length_type = "u32",		\
		},						\
	},

#undef __string
#define __string(_item, _src)					\
	{							\
		.name = #_item,					\
		.type = {					\
		  .atype = atype_string,			\
		  .name = NULL,					\
		  .u.string.encoding = lttng_encode_UTF8,	\
		},						\
	},

#undef TP_STRUCT__entry
#define TP_STRUCT__entry(args...) args	/* Only one used in this phase */

#undef DECLARE_EVENT_CLASS
#define DECLARE_EVENT_CLASS(_name, _proto, _args, _tstruct, _assign, _print) \
	static const struct lttng_event_field __event_fields___##_name[] = { \
		_tstruct						     \
	};

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)

/*
 * Stage 1.1 of the trace events.
 *
 * Create probe callback prototypes.
 */

#include "lttng-events-reset.h"	/* Reset all macros within TRACE_EVENT */

#undef TP_PROTO
#define TP_PROTO(args...) args

#undef DECLARE_EVENT_CLASS
#define DECLARE_EVENT_CLASS(_name, _proto, _args, _tstruct, _assign, _print)  \
static void __event_probe__##_name(void *__data, _proto);

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)

/*
 * Stage 2 of the trace events.
 *
 * Create an array of events.
 */

/* Named field types must be defined in lttng-types.h */

#include "lttng-events-reset.h"	/* Reset all macros within TRACE_EVENT */

#undef DEFINE_EVENT
#define DEFINE_EVENT(_template, _name, _proto, _args)			       \
		{							       \
			.fields = __event_fields___##_template,		       \
			.name = #_name,					       \
			.probe_callback = (void *) &__event_probe__##_template,\
			.nr_fields = ARRAY_SIZE(__event_fields___##_template), \
		},

#define TP_ID1(_token, _system)	_token##_system
#define TP_ID(_token, _system)	TP_ID1(_token, _system)

static const struct lttng_event_desc TP_ID(__event_desc___, TRACE_SYSTEM)[] = {
#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)
};

#undef TP_ID1
#undef TP_ID

/*
 * Stage 3 of the trace events.
 *
 * Create seq file metadata output.
 */

#define TP_ID1(_token, _system)	_token##_system
#define TP_ID(_token, _system)	TP_ID1(_token, _system)

static void *TP_ID(__lttng_seq_start__, TRACE_SYSTEM)(struct seq_file *m,
						      loff_t *pos)
{
	const struct lttng_event_desc *desc =
		&TP_ID(__event_desc___, TRACE_SYSTEM)[*pos];

	if (desc > &TP_ID(__event_desc___, TRACE_SYSTEM)
			[ARRAY_SIZE(TP_ID(__event_desc___, TRACE_SYSTEM)) - 1])
		return NULL;
	return (void *) desc;
}

static void *TP_ID(__lttng_seq_next__, TRACE_SYSTEM)(struct seq_file *m,
						     void *p, loff_t *ppos)
{
	const struct lttng_event_desc *desc =
		&TP_ID(__event_desc___, TRACE_SYSTEM)[++(*ppos)];

	if (desc > &TP_ID(__event_desc___, TRACE_SYSTEM)
			[ARRAY_SIZE(TP_ID(__event_desc___, TRACE_SYSTEM)) - 1])
		return NULL;
	return (void *) desc;
}

static void TP_ID(__lttng_seq_stop__, TRACE_SYSTEM)(struct seq_file *m,
						    void *p)
{
}

static int TP_ID(__lttng_seq_show__, TRACE_SYSTEM)(struct seq_file *m,
						   void *p)
{
	const struct lttng_event_desc *desc = p;
	int i;

	seq_printf(m,	"event {\n"
			"\tname = %s;\n"
			"\tid = UNKNOWN;\n"
			"\tstream = UNKNOWN;\n"
			"\tfields = {\n",
			desc->name);
	for (i = 0; i < desc->nr_fields; i++) {
		if (desc->fields[i].type.name)	/* Named type */
			seq_printf(m,	"\t\t%s",
					desc->fields[i].type.name);
		else				/* Nameless type */
			lttng_print_event_type(m, 2, &desc->fields[i].type);
		seq_printf(m,	" %s;\n", desc->fields[i].name);
	}
	seq_printf(m,	"\t};\n");
	seq_printf(m,	"};\n");
	return 0;
}

static const
struct seq_operations TP_ID(__lttng_types_seq_ops__, TRACE_SYSTEM) = {
	.start = TP_ID(__lttng_seq_start__, TRACE_SYSTEM),
	.next = TP_ID(__lttng_seq_next__, TRACE_SYSTEM),
	.stop = TP_ID(__lttng_seq_stop__, TRACE_SYSTEM),
	.show = TP_ID(__lttng_seq_show__, TRACE_SYSTEM),
};

static int
TP_ID(__lttng_types_open__, TRACE_SYSTEM)(struct inode *inode, struct file *file)
{
	return seq_open(file, &TP_ID(__lttng_types_seq_ops__, TRACE_SYSTEM));
}

static const
struct file_operations TP_ID(__lttng_types_fops__, TRACE_SYSTEM) = {
        .open = TP_ID(__lttng_types_open__, TRACE_SYSTEM),
        .read = seq_read,
        .llseek = seq_lseek,
        .release = seq_release_private,
};

static struct dentry *TP_ID(__lttng_types_dentry__, TRACE_SYSTEM);

static int TP_ID(__lttng_types_init__, TRACE_SYSTEM)(void)
{
	int ret = 0;

	TP_ID(__lttng_types_dentry__, TRACE_SYSTEM) =
		debugfs_create_file("lttng-events-" __stringify(TRACE_SYSTEM),
				    S_IWUSR, NULL, NULL,
				    &TP_ID(__lttng_types_fops__, TRACE_SYSTEM));
	if (IS_ERR(TP_ID(__lttng_types_dentry__, TRACE_SYSTEM))
	    || !TP_ID(__lttng_types_dentry__, TRACE_SYSTEM)) {
		printk(KERN_ERR "Error creating LTTng type export file\n");
		ret = -ENOMEM;
		goto error;
	}
error:
	return ret;
}

static void TP_ID(__lttng_types_exit__, TRACE_SYSTEM)(void)
{
	debugfs_remove(TP_ID(__lttng_types_dentry__, TRACE_SYSTEM));
}

#undef TP_ID1
#undef TP_ID

/*
 * Stage 4 of the trace events.
 *
 * Create static inline function that calculates event size.
 */

#include "lttng-events-reset.h"	/* Reset all macros within TRACE_EVENT */

/* Named field types must be defined in lttng-types.h */

#undef __field
#define __field(_type, _item)						       \
	__event_len += lib_ring_buffer_align(__event_len, __alignof__(_type)); \
	__event_len += sizeof(_type);

#undef __field_ext
#define __field_ext(_type, _item, _filter_type)	__field(_type, _item)

#undef __array
#define __array(_type, _item, _length)					       \
	__event_len += lib_ring_buffer_align(__event_len, __alignof__(_type)); \
	__event_len += sizeof(_type) * (_length);

#undef __dynamic_array
#define __dynamic_array(_type, _item, _length)				       \
	__event_len += lib_ring_buffer_align(__event_len, __alignof__(u32));   \
	__event_len += sizeof(u32);					       \
	__event_len += lib_ring_buffer_align(__event_len, __alignof__(_type)); \
	__event_len += sizeof(_type) * (_length);

#undef __string
#define __string(_item, _src)						       \
	__event_len += __dynamic_len[__dynamic_len_idx++] = strlen(_src) + 1;

#undef TP_PROTO
#define TP_PROTO(args...) args

#undef TP_STRUCT__entry
#define TP_STRUCT__entry(args...) args

#undef DECLARE_EVENT_CLASS
#define DECLARE_EVENT_CLASS(_name, _proto, _args, _tstruct, _assign, _print)  \
static inline size_t __event_get_size__##_name(size_t *__dynamic_len, _proto) \
{									      \
	size_t __event_len = 0;						      \
	unsigned int __dynamic_len_idx = 0;				      \
									      \
	if (0)								      \
		(void) __dynamic_len_idx;	/* don't warn if unused */    \
	_tstruct							      \
	return __event_len;						      \
}

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)



/*
 * Stage 5 of the trace events.
 *
 * Create static inline function that calculates event payload alignment.
 */

#include "lttng-events-reset.h"	/* Reset all macros within TRACE_EVENT */

/* Named field types must be defined in lttng-types.h */

#undef __field
#define __field(_type, _item)						  \
	__event_align = max_t(size_t, __event_align, __alignof__(_type));

#undef __field_ext
#define __field_ext(_type, _item, _filter_type)	__field(_type, _item)

#undef __array
#define __array(_type, _item, _length)					  \
	__event_align = max_t(size_t, __event_align, __alignof__(_type));

#undef __dynamic_array
#define __dynamic_array(_type, _item, _length)				  \
	__event_align = max_t(size_t, __event_align, __alignof__(u32));	  \
	__event_align = max_t(size_t, __event_align, __alignof__(_type));

#undef __string
#define __string(_item, _src)

#undef TP_PROTO
#define TP_PROTO(args...) args

#undef TP_STRUCT__entry
#define TP_STRUCT__entry(args...) args

#undef DECLARE_EVENT_CLASS
#define DECLARE_EVENT_CLASS(_name, _proto, _args, _tstruct, _assign, _print)  \
static inline size_t __event_get_align__##_name(_proto)			      \
{									      \
	size_t __event_align = 1;					      \
	_tstruct							      \
	return __event_align;						      \
}

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)


/*
 * Stage 6 of the trace events.
 *
 * Create structure declaration that allows the "assign" macros to access the
 * field types.
 */

#include "lttng-events-reset.h"	/* Reset all macros within TRACE_EVENT */

/* Named field types must be defined in lttng-types.h */

#undef __field
#define __field(_type, _item)	_type	_item;

#undef __field_ext
#define __field_ext(_type, _item, _filter_type)	__field(_type, _item)

#undef __array
#define __array(_type, _item, _length)	_type	_item;

#undef __dynamic_array
#define __dynamic_array(_type, _item, _length)	_type	_item;

#undef __string
#define __string(_item, _src)	char _item;

#undef TP_STRUCT__entry
#define TP_STRUCT__entry(args...) args

#undef DECLARE_EVENT_CLASS
#define DECLARE_EVENT_CLASS(_name, _proto, _args, _tstruct, _assign, _print)  \
struct __event_typemap__##_name {					      \
	_tstruct							      \
};

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)


/*
 * Stage 7 of the trace events.
 *
 * Create the probe function : call even size calculation and write event data
 * into the buffer.
 *
 * We use both the field and assignment macros to write the fields in the order
 * defined in the field declaration. The field declarations control the
 * execution order, jumping to the appropriate assignment block.
 */

#include "lttng-events-reset.h"	/* Reset all macros within TRACE_EVENT */

#undef __field
#define __field(_type, _item)						\
	goto __assign_##_item;						\
__end_field_##_item:

#undef __field_ext
#define __field_ext(_type, _item, _filter_type)	__field(_type, _item)

#undef __array
#define __array(_type, _item, _length)					\
	goto __assign_##_item;						\
__end_field_##_item:

#undef __dynamic_array
#define __dynamic_array(_type, _item, _length)				\
	goto __assign_##_item##_1;					\
__end_field_##_item##_1:						\
	goto __assign_##_item##_2;					\
__end_field_##_item##_2:

#undef __string
#define __string(_item, _src)						\
	goto __assign_##_item;						\
__end_field_##_item:

/*
 * Macros mapping tp_assign() to "=", tp_memcpy() to memcpy() and tp_strcpy() to
 * strcpy().
 */
#undef tp_assign
#define tp_assign(dest, src)						\
__assign_##dest:							\
	{								\
		__typeof__(__typemap.dest) __tmp = (src);		\
		lib_ring_buffer_align_ctx(&ctx, __alignof__(__tmp));	\
		__chan->ops->event_write(&ctx, &__tmp, sizeof(__tmp));	\
	}								\
	goto __end_field_##dest;

#undef tp_memcpy
#define tp_memcpy(dest, src, len)					\
__assign_##dest:							\
	lib_ring_buffer_align_ctx(&ctx, __alignof__(__typemap.dest));	\
	__chan->ops->event_write(&ctx, src, len);			\
	goto __end_field_##dest;

#undef tp_memcpy_dyn
#define tp_memcpy_dyn(dest, src, len)					\
__assign_##dest##_1:							\
	{								\
		u32 __tmpl = (len);					\
		lib_ring_buffer_align_ctx(&ctx, __alignof__(u32));	\
		__chan->ops->event_write(&ctx, &__tmpl, sizeof(u32));	\
	}								\
	goto __end_field_##dest##_1;					\
__assign_##dest##_2:							\
	lib_ring_buffer_align_ctx(&ctx, __alignof__(__typemap.dest));	\
	__chan->ops->event_write(&ctx, src, len);			\
	goto __end_field_##dest##_2;

#undef tp_strcpy
#define tp_strcpy(dest, src)						\
	tp_memcpy(dest, src, __get_dynamic_array_len(dest))

/* Named field types must be defined in lttng-types.h */

#undef __get_str
#define __get_str(field)		field

#undef __get_dynamic_array
#define __get_dynamic_array(field)	field

/* Beware: this get len actually consumes the len value */
#undef __get_dynamic_array_len
#define __get_dynamic_array_len(field)	__dynamic_len[__dynamic_len_idx++]

#undef TP_PROTO
#define TP_PROTO(args...) args

#undef TP_ARGS
#define TP_ARGS(args...) args

#undef TP_STRUCT__entry
#define TP_STRUCT__entry(args...) args

#undef TP_fast_assign
#define TP_fast_assign(args...) args

#undef DECLARE_EVENT_CLASS
#define DECLARE_EVENT_CLASS(_name, _proto, _args, _tstruct, _assign, _print)  \
static void __event_probe__##_name(void *__data, _proto)		      \
{									      \
	struct ltt_event *__event = __data;				      \
	struct ltt_channel *__chan = __event->chan;			      \
	struct lib_ring_buffer_ctx ctx;					      \
	size_t __event_len, __event_align;				      \
	size_t __dynamic_len_idx = 0;					      \
	size_t __dynamic_len[ARRAY_SIZE(__event_fields___##_name)];	      \
	struct __event_typemap__##_name __typemap;			      \
	int __ret;							      \
									      \
	if (0)								      \
		(void) __dynamic_len_idx;	/* don't warn if unused */    \
	if (!ACCESS_ONCE(__chan->session->active))			      \
		return;							      \
	__event_len = __event_get_size__##_name(__dynamic_len, _args);	      \
	__event_align = __event_get_align__##_name(_args);		      \
	lib_ring_buffer_ctx_init(&ctx, __chan->chan, NULL, __event_len,	      \
				 __event_align, -1);			      \
	__ret = __chan->ops->event_reserve(&ctx);			      \
	if (__ret < 0)							      \
		return;							      \
	/* Control code (field ordering) */				      \
	_tstruct							      \
	__chan->ops->event_commit(&ctx);				      \
	return;								      \
	/* Copy code, steered by control code */			      \
	_assign								      \
}

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)

/*
 * Stage 8 of the trace events.
 *
 * Register/unregister probes at module load/unload.
 */

#include "lttng-events-reset.h"	/* Reset all macros within TRACE_EVENT */

#define TP_ID1(_token, _system)	_token##_system
#define TP_ID(_token, _system)	TP_ID1(_token, _system)
#define module_init_eval1(_token, _system)	module_init(_token##_system)
#define module_init_eval(_token, _system)	module_init_eval1(_token, _system)
#define module_exit_eval1(_token, _system)	module_exit(_token##_system)
#define module_exit_eval(_token, _system)	module_exit_eval1(_token, _system)

static int TP_ID(__lttng_events_init__, TRACE_SYSTEM)(void)
{
	int ret;
	int i;

	wrapper_vmalloc_sync_all();
	ret = TP_ID(__lttng_types_init__, TRACE_SYSTEM)();
	if (ret)
		return ret;
	for (i = 0; i < ARRAY_SIZE(TP_ID(__event_desc___, TRACE_SYSTEM)); i++) {
		const struct lttng_event_desc *event_desc;

		event_desc = &TP_ID(__event_desc___, TRACE_SYSTEM)[i];
		ret = ltt_probe_register(event_desc->name,
					 event_desc->probe_callback);
		if (ret)
			goto error;
	}
	return 0;

error:
	for (i--; i >= 0; i--) {
		const struct lttng_event_desc *event_desc;

		event_desc = &TP_ID(__event_desc___, TRACE_SYSTEM)[i];
		ltt_probe_unregister(event_desc->name);
	}
	return ret;
}

module_init_eval(__lttng_events_init__, TRACE_SYSTEM);

static void TP_ID(__lttng_events_exit__, TRACE_SYSTEM)(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(TP_ID(__event_desc___, TRACE_SYSTEM)); i++) {
		const struct lttng_event_desc *event_desc;

		event_desc = &TP_ID(__event_desc___, TRACE_SYSTEM)[i];
		ltt_probe_unregister(event_desc->name);
	}
	TP_ID(__lttng_types_exit__, TRACE_SYSTEM)();
}

module_exit_eval(__lttng_events_exit__, TRACE_SYSTEM);

#undef module_init_eval
#undef module_exit_eval
#undef TP_ID1
#undef TP_ID
