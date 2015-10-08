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

#undef ctf_integer_nowrite
#define ctf_integer_nowrite(_type, _item, _user_src)		\
	_ctf_integer_ext(_type, _item, _user_src, __BYTE_ORDER, 10, 0, 1)

#undef ctf_integer_hex_nowrite
#define ctf_integer_hex_nowrite(_type, _item, _user_src)	\
	_ctf_integer_ext(_type, _item, _user_src, __BYTE_ORDER, 16, 0, 1)

#undef ctf_integer_oct_nowrite
#define ctf_integer_oct_nowrite(_type, _item, _user_src)	\
	_ctf_integer_ext(_type, _item, _user_src, __BYTE_ORDER, 8, 0, 1)

#undef ctf_integer_network_nowrite
#define ctf_integer_network_nowrite(_type, _item, _user_src)	\
	_ctf_integer_ext(_type, _item, _user_src, __BIG_ENDIAN, 10, 0, 1)

#undef ctf_integer_network_hex_nowrite
#define ctf_integer_network_hex_nowrite(_type, _item, _user_src) \
	_ctf_integer_ext(_type, _item, _user_src, __BIG_ENDIAN, 16, 0, 1)

#undef ctf_array_nowrite
#define ctf_array_nowrite(_type, _item, _user_src, _length)	\
	_ctf_array_encoded(_type, _item, _user_src, _length, none, 0, 1)

#undef ctf_array_text_nowrite
#define ctf_array_text_nowrite(_type, _item, _user_src, _length) \
	_ctf_array_encoded(_type, _item, _user_src, _length, UTF8, 0, 1)

#undef ctf_array_bitfield_nowrite
#define ctf_array_bitfield_nowrite(_type, _item, _src, _length)	\
	_ctf_array_bitfield(_type, _item, _src, _length, 0, 1)

#undef ctf_sequence_nowrite
#define ctf_sequence_nowrite(_type, _item, _user_src, _length_type, _user_src_length) \
	_ctf_sequence_encoded(_type, _item, _user_src,		\
			_length_type, _user_src_length, none, __BYTE_ORDER, 10, 0, 1)

#undef ctf_sequence_text_nowrite
#define ctf_sequence_text_nowrite(_type, _item, _user_src, _length_type, _user_src_length) \
	_ctf_sequence_encoded(_type, _item, _user_src,		\
			_length_type, _user_src_length, UTF8, __BYTE_ORDER, 10, 0, 1)

#undef ctf_sequence_bitfield_nowrite
#define ctf_sequence_bitfield_nowrite(_type, _item, _src, _length_type, _src_length) \
	_ctf_sequence_bitfield(_type, _item, _src,		\
			_length_type, _src_length, 0, 1)

#undef ctf_string_nowrite
#define ctf_string_nowrite(_item, _user_src)			\
	_ctf_string(_item, _user_src, 0, 1)

/* user src */
#undef ctf_user_integer_nowrite
#define ctf_user_integer_nowrite(_type, _item, _user_src)	\
	_ctf_integer_ext(_type, _item, _user_src, __BYTE_ORDER, 10, 1, 1)

#undef ctf_user_integer_hex_nowrite
#define ctf_user_integer_hex_nowrite(_type, _item, _user_src)	\
	_ctf_integer_ext(_type, _item, _user_src, __BYTE_ORDER, 16, 1, 1)

#undef ctf_user_integer_network_nowrite
#define ctf_user_integer_network_nowrite(_type, _item, _user_src) \
	_ctf_integer_ext(_type, _item, _user_src, __BIG_ENDIAN, 10, 1, 1)

#undef ctf_user_integer_network_hex_nowrite
#define ctf_user_integer_network_hex_nowrite(_type, _item, _user_src) \
	_ctf_integer_ext(_type, _item, _user_src, __BIG_ENDIAN, 16, 1, 1)

#undef ctf_user_array_nowrite
#define ctf_user_array_nowrite(_type, _item, _user_src, _length) \
	_ctf_array_encoded(_type, _item, _user_src, _length, none, 1, 1)

#undef ctf_user_array_text_nowrite
#define ctf_user_array_text_nowrite(_type, _item, _user_src, _length) \
	_ctf_array_encoded(_type, _item, _user_src, _length, UTF8, 1, 1)

#undef ctf_user_array_bitfield_nowrite
#define ctf_user_array_bitfield_nowrite(_type, _item, _src, _length)	\
	_ctf_array_bitfield(_type, _item, _src, _length, 1, 1)

#undef ctf_user_sequence_nowrite
#define ctf_user_sequence_nowrite(_type, _item, _user_src, _length_type, _user_src_length) \
	_ctf_sequence_encoded(_type, _item, _user_src,		\
			_length_type, _user_src_length, none, __BYTE_ORDER, 10, 1, 1)

#undef ctf_user_sequence_text_nowrite
#define ctf_user_sequence_text_nowrite(_type, _item, _user_src, _length_type, _user_src_length) \
	_ctf_sequence_encoded(_type, _item, _user_src,		\
			_length_type, _user_src_length, UTF8, __BYTE_ORDER, 10, 1, 1)

#undef ctf_user_sequence_bitfield_nowrite
#define ctf_user_sequence_bitfield_nowrite(_type, _item, _src, _length_type, _src_length) \
	_ctf_sequence_bitfield(_type, _item, _src,		\
			_length_type, _src_length, 1, 1)

#undef ctf_user_string_nowrite
#define ctf_user_string_nowrite(_item, _user_src)		\
	_ctf_string(_item, _user_src, 1, 1)
