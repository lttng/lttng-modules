/* Reset macros used within TRACE_EVENT to "nothing" */

#undef __field
#define __field(_type, _item)

#undef __field_ext
#define __field_ext(_type, _item, _filter_type)

#undef __field_network
#define __field_network(_type, _item)

#undef __array
#define __array(_type, _item, _length)

#undef __dynamic_array
#define __dynamic_array(_type, _item, _length)

#undef __dynamic_array
#define __dynamic_array_len(_type, _item, _length)

#undef __string
#define __string(_item, _src)

#undef tp_assign
#define tp_assign(dest, src)

#undef tp_memcpy
#define tp_memcpy(dest, src, len)

#undef tp_memcpy_dyn
#define tp_memcpy_dyn(dest, src, len)

#undef tp_strcpy
#define tp_strcpy(dest, src)

#undef __get_str
#define __get_str(field)

#undef __get_dynamic_array
#define __get_dynamic_array(field)

#undef __get_dynamic_array_len
#define __get_dynamic_array_len(field)

#undef TP_PROTO
#define TP_PROTO(args...)

#undef TP_ARGS
#define TP_ARGS(args...)

#undef TP_STRUCT__entry
#define TP_STRUCT__entry(args...)

#undef TP_fast_assign
#define TP_fast_assign(args...)

#undef __perf_count
#define __perf_count(args...)

#undef __perf_addr
#define __perf_addr(args...)

#undef TP_perf_assign
#define TP_perf_assign(args...)

#undef TP_printk
#define TP_printk(args...)

#undef DECLARE_EVENT_CLASS
#define DECLARE_EVENT_CLASS(_name, _proto, _args, _tstruct, _assign, _print)

#undef DEFINE_EVENT
#define DEFINE_EVENT(_template, _name, _proto, _args)

#undef TRACE_EVENT_FLAGS
#define TRACE_EVENT_FLAGS(name, value)
