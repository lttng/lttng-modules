/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-events-write.h
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
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
#define ctf_array_nowrite(_type, _item, _src, _length)		\
	_ctf_array_encoded(_type, _item, _src,			\
			_length, none, __BYTE_ORDER, 10, 0, 1)

#undef ctf_array_network_nowrite
#define ctf_array_network_nowrite(_type, _item, _src, _length)	\
	_ctf_array_encoded(_type, _item, _src,			\
			_length, none, __BIG_ENDIAN, 10, 0, 1)

#undef ctf_array_text_nowrite
#define ctf_array_text_nowrite(_type, _item, _src, _length)	\
	_ctf_array_encoded(_type, _item, _src,			\
			_length, UTF8, __BYTE_ORDER, 10, 0, 1)

#undef ctf_array_bitfield_nowrite
#define ctf_array_bitfield_nowrite(_type, _item, _src, _length)	\
	_ctf_array_bitfield(_type, _item, _src, _length, 0, 1)

#undef ctf_sequence_nowrite
#define ctf_sequence_nowrite(_type, _item, _user_src, _length_type, _user_src_length) \
	_ctf_sequence_encoded(_type, _item, _user_src,		\
			_length_type, _user_src_length, none, __BYTE_ORDER, 10, 0, 1)

#undef ctf_sequence_network_nowrite
#define ctf_sequence_network_nowrite(_type, _item, _user_src, _length_type, _user_src_length) \
	_ctf_sequence_encoded(_type, _item, _user_src,		\
			_length_type, _user_src_length, none, __BIG_ENDIAN, 10, 0, 1)

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

#undef ctf_enum_nowrite
#define ctf_enum_nowrite(_name, _type, _item, _src)		\
	_ctf_enum(_name, _type, _item, _src, 0, 1)

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
#define ctf_user_array_nowrite(_type, _item, _src, _length)	\
	_ctf_array_encoded(_type, _item, _src,			\
			_length, none, __BYTE_ORDER, 10, 1, 1)

#undef ctf_user_array_network_nowrite
#define ctf_user_array_network_nowrite(_type, _item, _src, _length) \
	_ctf_array_encoded(_type, _item, _src,			\
			_length, none, __BIG_ENDIAN, 10, 1, 1)

#undef ctf_user_array_text_nowrite
#define ctf_user_array_text_nowrite(_type, _item, _src, _length) \
	_ctf_array_encoded(_type, _item, _src,			\
			_length, UTF8, __BYTE_ORDER, 10, 1, 1)

#undef ctf_user_array_bitfield_nowrite
#define ctf_user_array_bitfield_nowrite(_type, _item, _src, _length)	\
	_ctf_array_bitfield(_type, _item, _src, _length, 1, 1)

#undef ctf_user_sequence_nowrite
#define ctf_user_sequence_nowrite(_type, _item, _user_src, _length_type, _user_src_length) \
	_ctf_sequence_encoded(_type, _item, _user_src,		\
			_length_type, _user_src_length, none, __BYTE_ORDER, 10, 1, 1)

#undef ctf_user_sequence_network_nowrite
#define ctf_user_sequence_network_nowrite(_type, _item, _user_src, _length_type, _user_src_length) \
	_ctf_sequence_encoded(_type, _item, _user_src,		\
			_length_type, _user_src_length, none, __BIG_ENDIAN, 10, 1, 1)

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

#undef ctf_user_enum_nowrite
#define ctf_user_enum_nowrite(_name, _type, _item, _src)	\
	_ctf_enum(_name, _type, _item, _src, 1, 1)
