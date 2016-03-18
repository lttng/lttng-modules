/*
 * lttng-events-reset.h
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; only
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* Reset macros used within LTTNG_TRACEPOINT_EVENT to "nothing" */

#undef LTTNG_TRACEPOINT_EVENT_CLASS_CODE
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE(_name, _proto, _args, _locvar, _code_pre, _fields, _code_post)

#undef LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS
#define LTTNG_TRACEPOINT_EVENT_CLASS_CODE_NOARGS(_name, _locvar, _code_pre, _fields, _code_post)

#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(_template, _name, _map, _proto, _args)

#undef LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS
#define LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP_NOARGS(_template, _name, _map)

#undef LTTNG_TRACEPOINT_ENUM
#define LTTNG_TRACEPOINT_ENUM(_name, _values)

#undef TP_PROTO
#define TP_PROTO(args...)

#undef TP_ARGS
#define TP_ARGS(args...)

#undef TP_locvar
#define TP_locvar(...)

#undef TP_code_pre
#define TP_code_pre(...)

#undef TP_code_post
#define TP_code_post(...)

#undef TP_FIELDS
#define TP_FIELDS(args...)

#undef _ctf_integer_ext
#define _ctf_integer_ext(_type, _item, _src, _byte_order, _base, \
			_user, _nowrite)

#undef _ctf_integer_ext_isuser0
#define _ctf_integer_ext_isuser0(_type, _item, _src, _byte_order, _base, \
			_nowrite)

#undef _ctf_integer_ext_isuser1
#define _ctf_integer_ext_isuser1(_type, _item, _src, _byte_order, _base, \
			_nowrite)

#undef _ctf_integer_ext_fetched
#define _ctf_integer_ext_fetched(_type, _item, _src, _byte_order, _base, \
			_nowrite)

#undef _ctf_array_encoded
#define _ctf_array_encoded(_type, _item, _src, _length, _encoding, \
			_user, _nowrite)

#undef _ctf_array_bitfield
#define _ctf_array_bitfield(_type, _item, _src, _length, _user, _nowrite)

#undef _ctf_sequence_encoded
#define _ctf_sequence_encoded(_type, _item, _src, _length_type, \
			_src_length, _encoding, _byte_order, _base, _user, _nowrite)

#undef _ctf_sequence_bitfield
#define _ctf_sequence_bitfield(_type, _item, _src, _length_type, \
			_src_length, _byte_order, _user, _nowrite)

#undef _ctf_string
#define _ctf_string(_item, _src, _user, _nowrite)

#undef _ctf_enum
#define _ctf_enum(_name, _type, _item, _src, _nowrite)

/* "write" */
#undef ctf_integer
#define ctf_integer(_type, _item, _src)

#undef ctf_integer_hex
#define ctf_integer_hex(_type, _item, _src)

#undef ctf_integer_oct
#define ctf_integer_oct(_type, _item, _src)

#undef ctf_integer_network
#define ctf_integer_network(_type, _item, _src)

#undef ctf_integer_network_hex
#define ctf_integer_network_hex(_type, _item, _src)

#undef ctf_float
#define ctf_float(_type, _item, _src)

#undef ctf_array
#define ctf_array(_type, _item, _src, _length)

#undef ctf_array_text
#define ctf_array_text(_type, _item, _src, _length)

#undef ctf_array_bitfield
#define ctf_array_bitfield(_type, _item, _src, _length)

#undef ctf_sequence
#define ctf_sequence(_type, _item, _src, _length_type, _src_length)

#undef ctf_sequence_hex
#define ctf_sequence_hex(_type, _item, _src, _length_type, _src_length)

#undef ctf_sequence_network
#define ctf_sequence_network(_type, _item, _src, _length_type, _src_length)

#undef ctf_sequence_text
#define ctf_sequence_text(_type, _item, _src, _length_type, _src_length)

#undef ctf_sequence_bitfield
#define ctf_sequence_bitfield(_type, _item, _src, _length_type, _src_length)

#undef ctf_string
#define ctf_string(_item, _src)

#undef ctf_enum
#define ctf_enum(_name, _type, _item, _src)

#undef ctf_custom_field
#define ctf_custom_field(_type, _item, _code)

#undef ctf_custom_type
#define ctf_custom_type(...)

#undef ctf_custom_code
#define ctf_custom_code(...)

#undef ctf_align
#define ctf_align(_type)

/* "nowrite" */
#undef ctf_integer_nowrite
#define ctf_integer_nowrite(_type, _item, _src)

#undef ctf_float_nowrite
#define ctf_float_nowrite(_type, _item, _src)

#undef ctf_array_nowrite
#define ctf_array_nowrite(_type, _item, _src, _length)

#undef ctf_array_text_nowrite
#define ctf_array_text_nowrite(_type, _item, _src, _length)

#undef ctf_array_bitfield_nowrite
#define ctf_array_bitfield_nowrite(_type, _item, _src, _length)

#undef ctf_sequence_nowrite
#define ctf_sequence_nowrite(_type, _item, _src, _length_type, _src_length)

#undef ctf_sequence_text_nowrite
#define ctf_sequence_text_nowrite(_type, _item, _src, _length_type, _src_length)

#undef ctf_sequence_bitfield_nowrite
#define ctf_sequence_bitfield_nowrite(_type, _item, _src, _length_type, _src_length)

#undef ctf_string_nowrite
#define ctf_string_nowrite(_item, _src)

#undef ctf_enum_nowrite
#define ctf_enum_nowrite(_name, _type, _item, _src)

/* "user" - "write" */
#undef ctf_user_integer
#define ctf_user_integer(_type, _item, _user_src)

#undef ctf_user_integer_hex
#define ctf_user_integer_hex(_type, _item, _user_src)

#undef ctf_user_integer_network
#define ctf_user_integer_network(_type, _item, _user_src)

#undef ctf_user_integer_network_hex
#define ctf_user_integer_network_hex(_type, _item, _user_src)

#undef ctf_user_float
#define ctf_user_float(_type, _item, _user_src)

#undef ctf_user_array
#define ctf_user_array(_type, _item, _user_src, _length)

#undef ctf_user_array_text
#define ctf_user_array_text(_type, _item, _user_src, _length)

#undef ctf_user_array_bitfield
#define ctf_user_array_bitfield(_type, _item, _src, _length)

#undef ctf_user_sequence
#define ctf_user_sequence(_type, _item, _user_src, _length_type, _user_src_length)

#undef ctf_user_sequence_text
#define ctf_user_sequence_text(_type, _item, _user_src, _length_type, _user_src_length)

#undef ctf_user_sequence_bitfield
#define ctf_user_sequence_bitfield(_type, _item, _src, _length_type, _src_length)

#undef ctf_user_string
#define ctf_user_string(_item, _user_src)

#undef ctf_user_enum
#define ctf_user_enum(_name, _type, _item, _src)

/* "user" - "nowrite" */
#undef ctf_user_integer_nowrite
#define ctf_user_integer_nowrite(_type, _item, _user_src)

#undef ctf_user_float_nowrite
#define ctf_user_float_nowrite(_type, _item, _user_src)

#undef ctf_user_array_nowrite
#define ctf_user_array_nowrite(_type, _item, _user_src, _length)

#undef ctf_user_array_text_nowrite
#define ctf_user_array_text_nowrite(_type, _item, _user_src, _length)

#undef ctf_user_array_bitfield_nowrite
#define ctf_user_array_bitfield_nowrite(_type, _item, _src, _length)

#undef ctf_user_sequence_nowrite
#define ctf_user_sequence_nowrite(_type, _item, _user_src, _length_type, _user_src_length)

#undef ctf_user_sequence_text_nowrite
#define ctf_user_sequence_text_nowrite(_type, _item, _user_src, _length_type, _user_src_length)

#undef ctf_user_sequence_bitfield_nowrite
#define ctf_user_sequence_bitfield_nowrite(_type, _item, _src, _length_type, _src_length)

#undef ctf_user_string_nowrite
#define ctf_user_string_nowrite(_item, _user_src)

#undef ctf_user_enum_nowrite
#define ctf_user_enum_nowrite(_name, _type, _item, _src)
