/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-event-notifier-notification.c
 *
 * Copyright (C) 2020 Francis Deslauriers <francis.deslauriers@efficios.com>
 */

#include <linux/bug.h>

#include <lttng/lttng-bytecode.h>
#include <lttng/events.h>
#include <lttng/msgpack.h>
#include <lttng/event-notifier-notification.h>

static
int capture_enum(struct lttng_msgpack_writer *writer,
		struct lttng_interpreter_output *output) __attribute__ ((unused));
static
int capture_enum(struct lttng_msgpack_writer *writer,
		struct lttng_interpreter_output *output)
{
	int ret;

	/*
	 * Enums are captured as a map containing 2 key-value pairs. Such as:
	 * - type: enum
	 *   value: 177
	 */
	ret = lttng_msgpack_begin_map(writer, 2);
	if (ret) {
		WARN_ON_ONCE(1);
		goto end;
	}

	ret = lttng_msgpack_write_str(writer, "type");
	if (ret) {
		WARN_ON_ONCE(1);
		goto end;
	}

	ret = lttng_msgpack_write_str(writer, "enum");
	if (ret) {
		WARN_ON_ONCE(1);
		goto end;
	}

	ret = lttng_msgpack_write_str(writer, "value");
	if (ret) {
		WARN_ON_ONCE(1);
		goto end;
	}

	switch (output->type) {
	case LTTNG_INTERPRETER_TYPE_SIGNED_ENUM:
		ret = lttng_msgpack_write_signed_integer(writer, output->u.s);
		if (ret) {
			WARN_ON_ONCE(1);
			goto end;
		}
		break;
	case LTTNG_INTERPRETER_TYPE_UNSIGNED_ENUM:
		ret = lttng_msgpack_write_signed_integer(writer, output->u.u);
		if (ret) {
			WARN_ON_ONCE(1);
			goto end;
		}
		break;
	default:
		WARN_ON(1);
	}

	ret = lttng_msgpack_end_map(writer);
	if (ret)
		WARN_ON_ONCE(1);

end:
	return ret;
}

static
int64_t capture_sequence_element_signed(uint8_t *ptr,
		const struct lttng_integer_type *type)
{
	int64_t value = 0;
	unsigned int size = type->size;
	bool byte_order_reversed = type->reverse_byte_order;

	switch (size) {
	case 8:
		value = *ptr;
		break;
	case 16:
	{
		int16_t tmp;
		tmp = *(int16_t *) ptr;
		if (byte_order_reversed)
			__swab16s(&tmp);

		value = tmp;
		break;
	}
	case 32:
	{
		int32_t tmp;
		tmp = *(int32_t *) ptr;
		if (byte_order_reversed)
			__swab32s(&tmp);

		value = tmp;
		break;
	}
	case 64:
	{
		int64_t tmp;
		tmp = *(int64_t *) ptr;
		if (byte_order_reversed)
			__swab64s(&tmp);

		value = tmp;
		break;
	}
	default:
		WARN_ON(1);
	}

	return value;
}

static
uint64_t capture_sequence_element_unsigned(uint8_t *ptr,
		const struct lttng_integer_type *type)
{
	uint64_t value = 0;
	unsigned int size = type->size;
	bool byte_order_reversed = type->reverse_byte_order;

	switch (size) {
	case 8:
		value = *ptr;
		break;
	case 16:
	{
		uint16_t tmp;
		tmp = *(uint16_t *) ptr;
		if (byte_order_reversed)
			__swab16s(&tmp);

		value = tmp;
		break;
	}
	case 32:
	{
		uint32_t tmp;
		tmp = *(uint32_t *) ptr;
		if (byte_order_reversed)
			__swab32s(&tmp);

		value = tmp;
		break;
	}
	case 64:
	{
		uint64_t tmp;
		tmp = *(uint64_t *) ptr;
		if (byte_order_reversed)
			__swab64s(&tmp);

		value = tmp;
		break;
	}
	default:
		WARN_ON(1);
	}

	return value;
}

static
int capture_sequence(struct lttng_msgpack_writer *writer,
		struct lttng_interpreter_output *output) __attribute__ ((unused));
int capture_sequence(struct lttng_msgpack_writer *writer,
		struct lttng_interpreter_output *output)
{
	const struct lttng_integer_type *integer_type = NULL;
	const struct lttng_type *nested_type;
	uint8_t *ptr;
	bool signedness;
	int ret, i;

	ret = lttng_msgpack_begin_array(writer, output->u.sequence.nr_elem);
	if (ret) {
		WARN_ON_ONCE(1);
		goto end;
	}

	ptr = (uint8_t *) output->u.sequence.ptr;
	nested_type = output->u.sequence.nested_type;
	switch (nested_type->atype) {
	case atype_integer:
		integer_type = &nested_type->u.integer;
		break;
	case atype_enum_nestable:
		/* Treat enumeration as an integer. */
		integer_type = &nested_type->u.enum_nestable.container_type->u.integer;
		break;
	default:
		/* Capture of array of non-integer are not supported. */
		WARN_ON(1);
	}
	signedness = integer_type->signedness;
	for (i = 0; i < output->u.sequence.nr_elem; i++) {
		if (signedness) {
			ret = lttng_msgpack_write_signed_integer(writer,
				capture_sequence_element_signed(ptr, integer_type));
			if (ret) {
				WARN_ON_ONCE(1);
				goto end;
			}
		} else {
			ret = lttng_msgpack_write_unsigned_integer(writer,
				capture_sequence_element_unsigned(ptr, integer_type));
			if (ret) {
				WARN_ON_ONCE(1);
				goto end;
			}
		}

		/*
		 * We assume that alignment is smaller or equal to the size.
		 * This currently holds true but if it changes in the future,
		 * we will want to change the pointer arithmetics below to
		 * take into account that the next element might be further
		 * away.
		 */
		WARN_ON(integer_type->alignment > integer_type->size);

		/* Size is in number of bits. */
		ptr += (integer_type->size / CHAR_BIT) ;
	}

	ret = lttng_msgpack_end_array(writer);
	if (ret)
		WARN_ON_ONCE(1);
end:
	return ret;
}

void lttng_event_notifier_notification_send(struct lttng_event_notifier *event_notifier)
{
	struct lttng_event_notifier_group *event_notifier_group = event_notifier->group;
	struct lib_ring_buffer_ctx ctx;
	int ret;

	if (unlikely(!READ_ONCE(event_notifier->enabled)))
		return;

	lib_ring_buffer_ctx_init(&ctx, event_notifier_group->chan, NULL,
			sizeof(event_notifier->user_token),
			lttng_alignof(event_notifier->user_token), -1);
	ret = event_notifier_group->ops->event_reserve(&ctx, 0);
	if (ret < 0) {
		//TODO: error handling with counter maps
		//silently drop for now.
		WARN_ON_ONCE(1);
		return;
	}
	lib_ring_buffer_align_ctx(&ctx, lttng_alignof(event_notifier->user_token));
	event_notifier_group->ops->event_write(&ctx, &event_notifier->user_token,
			sizeof(event_notifier->user_token));
	event_notifier_group->ops->event_commit(&ctx);
	irq_work_queue(&event_notifier_group->wakeup_pending);
}
