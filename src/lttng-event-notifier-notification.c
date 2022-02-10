/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-event-notifier-notification.c
 *
 * Copyright (C) 2020 Francis Deslauriers <francis.deslauriers@efficios.com>
 */

#include <asm/barrier.h>
#include <linux/bug.h>

#include <lttng/lttng-bytecode.h>
#include <lttng/events.h>
#include <lttng/msgpack.h>
#include <lttng/event-notifier-notification.h>
#include <lttng/events-internal.h>
#include <lttng/probe-user.h>

/*
 * The capture buffer size needs to be below 1024 bytes to avoid the
 * frame to be larger than the 1024 limit enforced by the kernel. If we
 * ever need to increase it, we will need to use a memory allocation
 * scheme which allows allocating temporary memory chunks from the
 * instrumentation sites. This could be done by adapting lttng
 * tp-mempool to become nmi-safe and lock-free.
 */
#define CAPTURE_BUFFER_SIZE 512

#define MSG_WRITE_NIL_LEN 1

struct lttng_event_notifier_notification {
	int notification_fd;
	uint64_t event_notifier_token;
	uint8_t capture_buf[CAPTURE_BUFFER_SIZE];
	struct lttng_msgpack_writer writer;
	bool has_captures;
};

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
		goto end;
	}

	ret = lttng_msgpack_write_str(writer, "type");
	if (ret) {
		goto end;
	}

	ret = lttng_msgpack_write_str(writer, "enum");
	if (ret) {
		goto end;
	}

	ret = lttng_msgpack_write_str(writer, "value");
	if (ret) {
		goto end;
	}

	switch (output->type) {
	case LTTNG_INTERPRETER_TYPE_SIGNED_ENUM:
		ret = lttng_msgpack_write_signed_integer(writer, output->u.s);
		if (ret) {
			goto end;
		}
		break;
	case LTTNG_INTERPRETER_TYPE_UNSIGNED_ENUM:
		ret = lttng_msgpack_write_signed_integer(writer, output->u.u);
		if (ret) {
			goto end;
		}
		break;
	default:
		WARN_ON_ONCE(1);
		ret = -1;
		goto end;
	}

	ret = lttng_msgpack_end_map(writer);
end:
	return ret;
}

static
int capture_sequence_element_signed(uint8_t *ptr,
		const struct lttng_kernel_type_integer *type,
		int64_t *_value)
{
	int64_t value = 0;
	unsigned int size = type->size;
	bool user = type->user;
	bool byte_order_reversed = type->reverse_byte_order;

	switch (size) {
	case 8:
	{
		int8_t tmp;

		if (user) {
			if (lttng_copy_from_user_check_nofault(&tmp, ptr, sizeof(int8_t)))
				return -1;
		} else {
			tmp = *ptr;
		}
		value = tmp;
		break;
	}
	case 16:
	{
		int16_t tmp;

		if (user) {
			if (lttng_copy_from_user_check_nofault(&tmp, ptr, sizeof(int16_t)))
				return -1;
		} else {
			tmp = *(int16_t *) ptr;
		}
		if (byte_order_reversed)
			__swab16s(&tmp);
		value = tmp;
		break;
	}
	case 32:
	{
		int32_t tmp;

		if (user) {
			if (lttng_copy_from_user_check_nofault(&tmp, ptr, sizeof(int32_t)))
				return -1;
		} else {
			tmp = *(int32_t *) ptr;
		}
		if (byte_order_reversed)
			__swab32s(&tmp);
		value = tmp;
		break;
	}
	case 64:
	{
		int64_t tmp;

		if (user) {
			if (lttng_copy_from_user_check_nofault(&tmp, ptr, sizeof(int64_t)))
				return -1;
		} else {
			tmp = *(int64_t *) ptr;
		}
		if (byte_order_reversed)
			__swab64s(&tmp);
		value = tmp;
		break;
	}
	default:
		WARN_ON_ONCE(1);
		return -1;
	}

	*_value = value;
	return 0;
}

static
int capture_sequence_element_unsigned(uint8_t *ptr,
		const struct lttng_kernel_type_integer *type,
		uint64_t *_value)
{
	uint64_t value = 0;
	unsigned int size = type->size;
	bool user = type->user;
	bool byte_order_reversed = type->reverse_byte_order;

	switch (size) {
	case 8:
	{
		uint8_t tmp;

		if (user) {
			if (lttng_copy_from_user_check_nofault(&tmp, ptr, sizeof(uint8_t)))
				return -1;
		} else {
			tmp = *ptr;
		}
		value = tmp;
		break;
	}
	case 16:
	{
		uint16_t tmp;

		if (user) {
			if (lttng_copy_from_user_check_nofault(&tmp, ptr, sizeof(uint16_t)))
				return -1;
		} else {
			tmp = *(uint16_t *) ptr;
		}
		if (byte_order_reversed)
			__swab16s(&tmp);
		value = tmp;
		break;
	}
	case 32:
	{
		uint32_t tmp;

		if (user) {
			if (lttng_copy_from_user_check_nofault(&tmp, ptr, sizeof(uint32_t)))
				return -1;
		} else {
			tmp = *(uint32_t *) ptr;
		}
		if (byte_order_reversed)
			__swab32s(&tmp);
		value = tmp;
		break;
	}
	case 64:
	{
		uint64_t tmp;

		if (user) {
			if (lttng_copy_from_user_check_nofault(&tmp, ptr, sizeof(uint64_t)))
				return -1;
		} else {
			tmp = *(uint64_t *) ptr;
		}
		if (byte_order_reversed)
			__swab64s(&tmp);
		value = tmp;
		break;
	}
	default:
		WARN_ON_ONCE(1);
		return -1;
	}

	*_value = value;
	return 0;
}

static
int capture_sequence(struct lttng_msgpack_writer *writer,
		struct lttng_interpreter_output *output)
{
	const struct lttng_kernel_type_integer *integer_type = NULL;
	const struct lttng_kernel_type_common *nested_type;
	uint8_t *ptr;
	bool signedness;
	int ret, i;

	ret = lttng_msgpack_begin_array(writer, output->u.sequence.nr_elem);
	if (ret) {
		goto end;
	}

	ptr = (uint8_t *) output->u.sequence.ptr;
	nested_type = output->u.sequence.nested_type;
	switch (nested_type->type) {
	case lttng_kernel_type_integer:
		integer_type = lttng_kernel_get_type_integer(nested_type);
		break;
	case lttng_kernel_type_enum:
		/* Treat enumeration as an integer. */
		integer_type = lttng_kernel_get_type_integer(lttng_kernel_get_type_enum(nested_type)->container_type);
		break;
	default:
		/* Capture of array of non-integer are not supported. */
		WARN_ON_ONCE(1);
		ret = -1;
		goto end;
	}
	signedness = integer_type->signedness;
	for (i = 0; i < output->u.sequence.nr_elem; i++) {
		if (signedness) {
			int64_t v;

			ret = capture_sequence_element_signed(ptr, integer_type, &v);
			if (ret) {
				goto end;
			}
			ret = lttng_msgpack_write_signed_integer(writer, v);
			if (ret) {
				goto end;
			}
		} else {
			uint64_t v;

			ret = capture_sequence_element_unsigned(ptr, integer_type, &v);
			if (ret) {
				goto end;
			}
			ret = lttng_msgpack_write_unsigned_integer(writer, v);
			if (ret) {
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
		WARN_ON_ONCE(integer_type->alignment > integer_type->size);

		/* Size is in number of bits. */
		ptr += (integer_type->size / CHAR_BIT) ;
	}

	ret = lttng_msgpack_end_array(writer);
end:
	return ret;
}

static
int notification_append_capture(
		struct lttng_event_notifier_notification *notif,
		struct lttng_interpreter_output *output)
{
	struct lttng_msgpack_writer *writer = &notif->writer;
	int ret = 0;

	switch (output->type) {
	case LTTNG_INTERPRETER_TYPE_S64:
		ret = lttng_msgpack_write_signed_integer(writer, output->u.s);
		break;
	case LTTNG_INTERPRETER_TYPE_U64:
		ret = lttng_msgpack_write_unsigned_integer(writer, output->u.u);
		break;
	case LTTNG_INTERPRETER_TYPE_STRING:
		if (output->u.str.user) {
			ret = lttng_msgpack_write_user_str(writer, output->u.str.user_str);
		} else {
			ret = lttng_msgpack_write_str(writer, output->u.str.str);
		}
		break;
	case LTTNG_INTERPRETER_TYPE_SEQUENCE:
		ret = capture_sequence(writer, output);
		break;
	case LTTNG_INTERPRETER_TYPE_SIGNED_ENUM:
	case LTTNG_INTERPRETER_TYPE_UNSIGNED_ENUM:
		ret = capture_enum(writer, output);
		break;
	default:
		ret = -1;
		WARN_ON_ONCE(1);
	}
	return ret;
}

static
int notification_append_empty_capture(
		struct lttng_event_notifier_notification *notif)
{
	return lttng_msgpack_write_nil(&notif->writer);
}

static
int notification_init(struct lttng_event_notifier_notification *notif,
		struct lttng_kernel_event_notifier *event_notifier)
{
	struct lttng_msgpack_writer *writer = &notif->writer;
	int ret = 0;

	notif->has_captures = false;

	if (event_notifier->priv->num_captures > 0) {
		lttng_msgpack_writer_init(writer, notif->capture_buf,
				CAPTURE_BUFFER_SIZE);

		ret = lttng_msgpack_begin_array(writer, event_notifier->priv->num_captures);
		if (ret) {
			goto end;
		}

		notif->has_captures = true;
	}

end:
	return ret;
}

static
void record_error(struct lttng_kernel_event_notifier *event_notifier)
{

	struct lttng_event_notifier_group *event_notifier_group = event_notifier->priv->group;
	struct lttng_kernel_channel_counter *error_counter;
	size_t dimension_index[1];
	int ret;

	/*
	 * smp_load_acquire paired with smp_store_release orders creation of
	 * the error counter and setting error_counter_len before the
	 * error_counter is used.
	 */
	error_counter = smp_load_acquire(&event_notifier_group->error_counter);
	/* This group may not have an error counter attached to it. */
	if (!error_counter)
		return;

	dimension_index[0] = event_notifier->priv->error_counter_index;

	ret = error_counter->ops->priv->counter_add(error_counter, dimension_index, 1);
	if (ret)
		WARN_ON_ONCE(1);
}

static
void notification_send(struct lttng_event_notifier_notification *notif,
		struct lttng_kernel_event_notifier *event_notifier)
{
	struct lttng_event_notifier_group *event_notifier_group = event_notifier->priv->group;
	struct lttng_kernel_ring_buffer_ctx ctx;
	struct lttng_kernel_abi_event_notifier_notification kernel_notif;
	size_t capture_buffer_content_len, reserve_size;
	int ret;

	reserve_size = sizeof(kernel_notif);
	kernel_notif.token = event_notifier->priv->parent.user_token;

	if (notif->has_captures) {
		capture_buffer_content_len = notif->writer.write_pos - notif->writer.buffer;
	} else {
		capture_buffer_content_len = 0;
	}

	WARN_ON_ONCE(capture_buffer_content_len > CAPTURE_BUFFER_SIZE);

	reserve_size += capture_buffer_content_len;
	kernel_notif.capture_buf_size = capture_buffer_content_len;

	lib_ring_buffer_ctx_init(&ctx, event_notifier_group->chan, reserve_size,
			lttng_alignof(kernel_notif), NULL);
	ret = event_notifier_group->ops->event_reserve(&ctx);
	if (ret < 0) {
		record_error(event_notifier);
		return;
	}

	/* Write the notif structure. */
	event_notifier_group->ops->event_write(&ctx, &kernel_notif,
			sizeof(kernel_notif), lttng_alignof(kernel_notif));

	/*
	 * Write the capture buffer. No need to realigned as the below is a raw
	 * char* buffer.
	 */
	event_notifier_group->ops->event_write(&ctx, &notif->capture_buf,
			capture_buffer_content_len, 1);

	event_notifier_group->ops->event_commit(&ctx);
	irq_work_queue(&event_notifier_group->wakeup_pending);
}

/*
 * Validate that the buffer has enough room to hold empty capture fields.
 */
static
bool validate_buffer_len(struct lttng_event_notifier_notification *notif, size_t captures_left)
{
	if (notif->writer.end_write_pos - notif->writer.write_pos < MSG_WRITE_NIL_LEN * captures_left)
		return false;
	return true;
}

void lttng_event_notifier_notification_send(struct lttng_kernel_event_notifier *event_notifier,
		const char *stack_data,
		struct lttng_kernel_probe_ctx *probe_ctx,
		struct lttng_kernel_notification_ctx *notif_ctx)
{
	struct lttng_event_notifier_notification notif = { 0 };
	size_t captures_left;

	if (notification_init(&notif, event_notifier))
		goto error;

	captures_left = event_notifier->priv->num_captures;
	if (!validate_buffer_len(&notif, captures_left))
		goto error;

	if (unlikely(notif_ctx->eval_capture)) {
		struct lttng_kernel_bytecode_runtime *capture_bc_runtime;

		/*
		 * Iterate over all the capture bytecodes. If the interpreter
		 * functions returns successfully, append the value of the
		 * `output` parameter to the capture buffer. If the interpreter
		 * fails, append an empty capture to the buffer.
		 */
		list_for_each_entry_rcu(capture_bc_runtime,
				&event_notifier->priv->capture_bytecode_runtime_head, node) {
			struct lttng_interpreter_output output;
			uint8_t *save_pos;
			int ret = -1;

			lttng_msgpack_save_writer_pos(&notif.writer, &save_pos);
			captures_left--;
			if (capture_bc_runtime->interpreter_func(capture_bc_runtime,
					stack_data, probe_ctx, &output) == LTTNG_KERNEL_BYTECODE_INTERPRETER_OK)
				ret = notification_append_capture(&notif, &output);
			if (ret || !validate_buffer_len(&notif, captures_left)) {
				/*
				 * On append capture error or if the generated
				 * buffer data would not leave enough room to
				 * write empty capture fields for the remaining
				 * fields, skip the field capture by restoring
				 * the msgpack writer position and writing an
				 * empty capture field.
				 */
				lttng_msgpack_restore_writer_pos(&notif.writer, save_pos);
				ret = notification_append_empty_capture(&notif);
				WARN_ON_ONCE(ret);
			}
		}
	}

	if (notif.has_captures && lttng_msgpack_end_array(&notif.writer))
		goto error;

	/*
	 * Send the notification (including the capture buffer) to the
	 * sessiond.
	 */
	notification_send(&notif, event_notifier);
	return;

error:
	record_error(event_notifier);
	return;
}
