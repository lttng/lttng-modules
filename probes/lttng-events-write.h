/*
 * lttng-events-write.h
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
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

#undef ctf_integer
#define ctf_integer(_type, _item, _src)				\
	_ctf_integer_ext(_type, _item, _src, __BYTE_ORDER, 10, 0, 0)

#undef ctf_integer_hex
#define ctf_integer_hex(_type, _item, _src)			\
	_ctf_integer_ext(_type, _item, _src, __BYTE_ORDER, 16, 0, 0)

#undef ctf_integer_oct
#define ctf_integer_oct(_type, _item, _src)			\
	_ctf_integer_ext(_type, _item, _src, __BYTE_ORDER, 8, 0, 0)

#undef ctf_integer_network
#define ctf_integer_network(_type, _item, _src)			\
	_ctf_integer_ext(_type, _item, _src, __BIG_ENDIAN, 10, 0, 0)

#undef ctf_integer_network_hex
#define ctf_integer_network_hex(_type, _item, _src)		\
	_ctf_integer_ext(_type, _item, _src, __BIG_ENDIAN, 16, 0, 0)

#undef ctf_array
#define ctf_array(_type, _item, _src, _length)			\
	_ctf_array_encoded(_type, _item, _src, _length, none, 0, 0)

#undef ctf_array_text
#define ctf_array_text(_type, _item, _src, _length)		\
	_ctf_array_encoded(_type, _item, _src, _length, UTF8, 0, 0)

#undef ctf_sequence
#define ctf_sequence(_type, _item, _src, _length_type, _src_length) \
	_ctf_sequence_encoded(_type, _item, _src,		\
			_length_type, _src_length, none, __BYTE_ORDER, 10, 0, 0)

#undef ctf_sequence_hex
#define ctf_sequence_hex(_type, _item, _src, _length_type, _src_length) \
	_ctf_sequence_encoded(_type, _item, _src,		\
			_length_type, _src_length, none, __BYTE_ORDER, 16, 0, 0)

#undef ctf_sequence_network
#define ctf_sequence_network(_type, _item, _src, _length_type, _src_length) \
	_ctf_sequence_encoded(_type, _item, _src,		\
			_length_type, _src_length, none, __BIG_ENDIAN, 10, 0, 0)

#undef ctf_sequence_text
#define ctf_sequence_text(_type, _item, _src, _length_type, _src_length) \
	_ctf_sequence_encoded(_type, _item, _src,		\
			_length_type, _src_length, UTF8, __BYTE_ORDER, 0, 0, 0)

#undef ctf_string
#define ctf_string(_item, _src)					\
	_ctf_string(_item, _src, 0, 0)

/* user src */
#undef ctf_user_integer
#define ctf_user_integer(_type, _item, _src)				\
	_ctf_integer_ext(_type, _item, _src, __BYTE_ORDER, 10, 1, 0)

#undef ctf_user_integer_hex
#define ctf_user_integer_hex(_type, _item, _src)			\
	_ctf_integer_ext(_type, _item, _src, __BYTE_ORDER, 16, 1, 0)

#undef ctf_user_integer_network
#define ctf_user_integer_network(_type, _item, _src)			\
	_ctf_integer_ext(_type, _item, _src, __BIG_ENDIAN, 10, 1, 0)

#undef ctf_user_integer_network_hex
#define ctf_user_integer_network_hex(_type, _item, _src)		\
	_ctf_integer_ext(_type, _item, _src, __BIG_ENDIAN, 16, 1, 0)

#undef ctf_user_array
#define ctf_user_array(_type, _item, _src, _length)			\
	_ctf_array_encoded(_type, _item, _src, _length, none, 1, 0)

#undef ctf_user_array_text
#define ctf_user_array_text(_type, _item, _src, _length)		\
	_ctf_array_encoded(_type, _item, _src, _length, UTF8, 1, 0)

#undef ctf_user_sequence
#define ctf_user_sequence(_type, _item, _src, _length_type, _src_length) \
	_ctf_sequence_encoded(_type, _item, _src,		\
			_length_type, _src_length, none, __BYTE_ORDER, 10, 1, 0)

#undef ctf_user_sequence_hex
#define ctf_user_sequence_hex(_type, _item, _src, _length_type, _src_length) \
	_ctf_sequence_encoded(_type, _item, _src,		\
			_length_type, _src_length, none, __BYTE_ORDER, 16, 1, 0)

#undef ctf_user_sequence_text
#define ctf_user_sequence_text(_type, _item, _src, _length_type, _src_length) \
	_ctf_sequence_encoded(_type, _item, _src,		\
			_length_type, _src_length, UTF8, __BYTE_ORDER, 0, 1, 0)

#undef ctf_user_string
#define ctf_user_string(_item, _src)					\
	_ctf_string(_item, _src, 1, 0)
