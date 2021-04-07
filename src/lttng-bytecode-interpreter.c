/* SPDX-License-Identifier: MIT
 *
 * lttng-bytecode-interpreter.c
 *
 * LTTng modules bytecode interpreter.
 *
 * Copyright (C) 2010-2016 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <wrapper/uaccess.h>
#include <wrapper/objtool.h>
#include <wrapper/types.h>
#include <linux/swab.h>

#include <lttng/lttng-bytecode.h>
#include <lttng/string-utils.h>
#include <lttng/events-internal.h>

/*
 * get_char should be called with page fault handler disabled if it is expected
 * to handle user-space read.
 */
static
char get_char(const struct estack_entry *reg, size_t offset)
{
	if (unlikely(offset >= reg->u.s.seq_len))
		return '\0';
	if (reg->u.s.user) {
		char c;

		/* Handle invalid access as end of string. */
		if (unlikely(!lttng_access_ok(VERIFY_READ,
				reg->u.s.user_str + offset,
				sizeof(c))))
			return '\0';
		/* Handle fault (nonzero return value) as end of string. */
		if (unlikely(__copy_from_user_inatomic(&c,
				reg->u.s.user_str + offset,
				sizeof(c))))
			return '\0';
		return c;
	} else {
		return reg->u.s.str[offset];
	}
}

/*
 * -1: wildcard found.
 * -2: unknown escape char.
 * 0: normal char.
 */
static
int parse_char(struct estack_entry *reg, char *c, size_t *offset)
{
	switch (*c) {
	case '\\':
		(*offset)++;
		*c = get_char(reg, *offset);
		switch (*c) {
		case '\\':
		case '*':
			return 0;
		default:
			return -2;
		}
	case '*':
		return -1;
	default:
		return 0;
	}
}

static
char get_char_at_cb(size_t at, void *data)
{
	return get_char(data, at);
}

static
int stack_star_glob_match(struct estack *stack, int top, const char *cmp_type)
{
	bool has_user = false;
	int result;
	struct estack_entry *pattern_reg;
	struct estack_entry *candidate_reg;

	/* Disable the page fault handler when reading from userspace. */
	if (estack_bx(stack, top)->u.s.user
			|| estack_ax(stack, top)->u.s.user) {
		has_user = true;
		pagefault_disable();
	}

	/* Find out which side is the pattern vs. the candidate. */
	if (estack_ax(stack, top)->u.s.literal_type == ESTACK_STRING_LITERAL_TYPE_STAR_GLOB) {
		pattern_reg = estack_ax(stack, top);
		candidate_reg = estack_bx(stack, top);
	} else {
		pattern_reg = estack_bx(stack, top);
		candidate_reg = estack_ax(stack, top);
	}

	/* Perform the match operation. */
	result = !strutils_star_glob_match_char_cb(get_char_at_cb,
		pattern_reg, get_char_at_cb, candidate_reg);
	if (has_user)
		pagefault_enable();

	return result;
}

static
int stack_strcmp(struct estack *stack, int top, const char *cmp_type)
{
	size_t offset_bx = 0, offset_ax = 0;
	int diff, has_user = 0;

	if (estack_bx(stack, top)->u.s.user
			|| estack_ax(stack, top)->u.s.user) {
		has_user = 1;
		pagefault_disable();
	}

	for (;;) {
		int ret;
		int escaped_r0 = 0;
		char char_bx, char_ax;

		char_bx = get_char(estack_bx(stack, top), offset_bx);
		char_ax = get_char(estack_ax(stack, top), offset_ax);

		if (unlikely(char_bx == '\0')) {
			if (char_ax == '\0') {
				diff = 0;
				break;
			} else {
				if (estack_ax(stack, top)->u.s.literal_type ==
						ESTACK_STRING_LITERAL_TYPE_PLAIN) {
					ret = parse_char(estack_ax(stack, top),
						&char_ax, &offset_ax);
					if (ret == -1) {
						diff = 0;
						break;
					}
				}
				diff = -1;
				break;
			}
		}
		if (unlikely(char_ax == '\0')) {
			if (estack_bx(stack, top)->u.s.literal_type ==
					ESTACK_STRING_LITERAL_TYPE_PLAIN) {
				ret = parse_char(estack_bx(stack, top),
					&char_bx, &offset_bx);
				if (ret == -1) {
					diff = 0;
					break;
				}
			}
			diff = 1;
			break;
		}
		if (estack_bx(stack, top)->u.s.literal_type ==
				ESTACK_STRING_LITERAL_TYPE_PLAIN) {
			ret = parse_char(estack_bx(stack, top),
				&char_bx, &offset_bx);
			if (ret == -1) {
				diff = 0;
				break;
			} else if (ret == -2) {
				escaped_r0 = 1;
			}
			/* else compare both char */
		}
		if (estack_ax(stack, top)->u.s.literal_type ==
				ESTACK_STRING_LITERAL_TYPE_PLAIN) {
			ret = parse_char(estack_ax(stack, top),
				&char_ax, &offset_ax);
			if (ret == -1) {
				diff = 0;
				break;
			} else if (ret == -2) {
				if (!escaped_r0) {
					diff = -1;
					break;
				}
			} else {
				if (escaped_r0) {
					diff = 1;
					break;
				}
			}
		} else {
			if (escaped_r0) {
				diff = 1;
				break;
			}
		}
		diff = char_bx - char_ax;
		if (diff != 0)
			break;
		offset_bx++;
		offset_ax++;
	}
	if (has_user)
		pagefault_enable();

	return diff;
}

uint64_t lttng_bytecode_filter_interpret_false(void *filter_data,
		struct lttng_probe_ctx *lttng_probe_ctx,
		const char *filter_stack_data)
{
	return LTTNG_INTERPRETER_DISCARD;
}

uint64_t lttng_bytecode_capture_interpret_false(void *filter_data,
		struct lttng_probe_ctx *lttng_probe_ctx,
		const char *capture_stack_data,
		struct lttng_interpreter_output *output)
{
	return LTTNG_INTERPRETER_DISCARD;
}

#ifdef INTERPRETER_USE_SWITCH

/*
 * Fallback for compilers that do not support taking address of labels.
 */

#define START_OP							\
	start_pc = &bytecode->data[0];					\
	for (pc = next_pc = start_pc; pc - start_pc < bytecode->len;	\
			pc = next_pc) {					\
		dbg_printk("LTTng: Executing op %s (%u)\n",		\
			lttng_bytecode_print_op((unsigned int) *(bytecode_opcode_t *) pc), \
			(unsigned int) *(bytecode_opcode_t *) pc); 	\
		switch (*(bytecode_opcode_t *) pc)	{

#define OP(name)	case name

#define PO		break

#define END_OP		}						\
	}

#else

/*
 * Dispatch-table based interpreter.
 */

#define START_OP							\
	start_pc = &bytecode->code[0];					\
	pc = next_pc = start_pc;					\
	if (unlikely(pc - start_pc >= bytecode->len))			\
		goto end;						\
	goto *dispatch[*(bytecode_opcode_t *) pc];

#define OP(name)							\
LABEL_##name

#define PO								\
		pc = next_pc;						\
		goto *dispatch[*(bytecode_opcode_t *) pc];

#define END_OP

#endif

#define IS_INTEGER_REGISTER(reg_type) \
		(reg_type == REG_S64 || reg_type == REG_U64)

static int context_get_index(struct lttng_probe_ctx *lttng_probe_ctx,
		struct load_ptr *ptr,
		uint32_t idx)
{

	struct lttng_kernel_ctx_field *ctx_field;
	const struct lttng_kernel_event_field *field;
	union lttng_ctx_value v;

	ctx_field = &lttng_static_ctx->fields[idx];
	field = ctx_field->event_field;
	ptr->type = LOAD_OBJECT;
	/* field is only used for types nested within variants. */
	ptr->field = NULL;

	switch (field->type->type) {
	case lttng_kernel_type_integer:
		ctx_field->get_value(ctx_field, lttng_probe_ctx, &v);
		if (lttng_kernel_get_type_integer(field->type)->signedness) {
			ptr->object_type = OBJECT_TYPE_S64;
			ptr->u.s64 = v.s64;
			ptr->ptr = &ptr->u.s64;
		} else {
			ptr->object_type = OBJECT_TYPE_U64;
			ptr->u.u64 = v.s64;	/* Cast. */
			ptr->ptr = &ptr->u.u64;
		}
		break;
	case lttng_kernel_type_enum:
	{
		const struct lttng_kernel_type_enum *enum_type = lttng_kernel_get_type_enum(field->type);
		const struct lttng_kernel_type_integer *integer_type = lttng_kernel_get_type_integer(enum_type->container_type);

		ctx_field->get_value(ctx_field, lttng_probe_ctx, &v);
		if (integer_type->signedness) {
			ptr->object_type = OBJECT_TYPE_SIGNED_ENUM;
			ptr->u.s64 = v.s64;
			ptr->ptr = &ptr->u.s64;
		} else {
			ptr->object_type = OBJECT_TYPE_UNSIGNED_ENUM;
			ptr->u.u64 = v.s64;	/* Cast. */
			ptr->ptr = &ptr->u.u64;
		}
		break;
	}
	case lttng_kernel_type_array:
	{
		const struct lttng_kernel_type_array *array_type = lttng_kernel_get_type_array(field->type);

		if (!lttng_kernel_type_is_bytewise_integer(array_type->elem_type)) {
			printk(KERN_WARNING "LTTng: bytecode: Array nesting only supports integer types.\n");
			return -EINVAL;
		}
		if (array_type->encoding == lttng_kernel_string_encoding_none) {
			printk(KERN_WARNING "LTTng: bytecode: Only string arrays are supported for contexts.\n");
			return -EINVAL;
		}
		ptr->object_type = OBJECT_TYPE_STRING;
		ctx_field->get_value(ctx_field, lttng_probe_ctx, &v);
		ptr->ptr = v.str;
		break;
	}
	case lttng_kernel_type_sequence:
	{
		const struct lttng_kernel_type_sequence *sequence_type = lttng_kernel_get_type_sequence(field->type);

		if (!lttng_kernel_type_is_bytewise_integer(sequence_type->elem_type)) {
			printk(KERN_WARNING "LTTng: bytecode: Sequence nesting only supports integer types.\n");
			return -EINVAL;
		}
		if (sequence_type->encoding == lttng_kernel_string_encoding_none) {
			printk(KERN_WARNING "LTTng: bytecode: Only string sequences are supported for contexts.\n");
			return -EINVAL;
		}
		ptr->object_type = OBJECT_TYPE_STRING;
		ctx_field->get_value(ctx_field, lttng_probe_ctx, &v);
		ptr->ptr = v.str;
		break;
	}
	case lttng_kernel_type_string:
		ptr->object_type = OBJECT_TYPE_STRING;
		ctx_field->get_value(ctx_field, lttng_probe_ctx, &v);
		ptr->ptr = v.str;
		break;
	case lttng_kernel_type_struct:
		printk(KERN_WARNING "LTTng: bytecode: Structure type cannot be loaded.\n");
		return -EINVAL;
	case lttng_kernel_type_variant:
		printk(KERN_WARNING "LTTng: bytecode: Variant type cannot be loaded.\n");
		return -EINVAL;
	default:
		printk(KERN_WARNING "LTTng: bytecode: Unknown type: %d", (int) field->type->type);
		return -EINVAL;
	}
	return 0;
}

static int dynamic_get_index(struct lttng_probe_ctx *lttng_probe_ctx,
		struct bytecode_runtime *runtime,
		uint64_t index, struct estack_entry *stack_top)
{
	int ret;
	const struct bytecode_get_index_data *gid;

	gid = (const struct bytecode_get_index_data *) &runtime->data[index];
	switch (stack_top->u.ptr.type) {
	case LOAD_OBJECT:
		switch (stack_top->u.ptr.object_type) {
		case OBJECT_TYPE_ARRAY:
		{
			const char *ptr;

			WARN_ON_ONCE(gid->offset >= gid->array_len);
			/* Skip count (unsigned long) */
			ptr = *(const char **) (stack_top->u.ptr.ptr + sizeof(unsigned long));
			ptr = ptr + gid->offset;
			stack_top->u.ptr.ptr = ptr;
			stack_top->u.ptr.object_type = gid->elem.type;
			stack_top->u.ptr.rev_bo = gid->elem.rev_bo;
			BUG_ON(stack_top->u.ptr.field->type->type != lttng_kernel_type_array);
			stack_top->u.ptr.field = NULL;
			break;
		}
		case OBJECT_TYPE_SEQUENCE:
		{
			const char *ptr;
			size_t ptr_seq_len;

			ptr = *(const char **) (stack_top->u.ptr.ptr + sizeof(unsigned long));
			ptr_seq_len = *(unsigned long *) stack_top->u.ptr.ptr;
			if (gid->offset >= gid->elem.len * ptr_seq_len) {
				ret = -EINVAL;
				goto end;
			}
			ptr = ptr + gid->offset;
			stack_top->u.ptr.ptr = ptr;
			stack_top->u.ptr.object_type = gid->elem.type;
			stack_top->u.ptr.rev_bo = gid->elem.rev_bo;
			BUG_ON(stack_top->u.ptr.field->type->type != lttng_kernel_type_sequence);
			stack_top->u.ptr.field = NULL;
			break;
		}
		case OBJECT_TYPE_STRUCT:
			printk(KERN_WARNING "LTTng: bytecode: Nested structures are not supported yet.\n");
			ret = -EINVAL;
			goto end;
		case OBJECT_TYPE_VARIANT:
		default:
			printk(KERN_WARNING "LTTng: bytecode: Unexpected get index type %d",
				(int) stack_top->u.ptr.object_type);
			ret = -EINVAL;
			goto end;
		}
		break;
	case LOAD_ROOT_CONTEXT:
	case LOAD_ROOT_APP_CONTEXT:	/* Fall-through */
	{
		ret = context_get_index(lttng_probe_ctx,
				&stack_top->u.ptr,
				gid->ctx_index);
		if (ret) {
			goto end;
		}
		break;
	}
	case LOAD_ROOT_PAYLOAD:
		stack_top->u.ptr.ptr += gid->offset;
		if (gid->elem.type == OBJECT_TYPE_STRING)
			stack_top->u.ptr.ptr = *(const char * const *) stack_top->u.ptr.ptr;
		stack_top->u.ptr.object_type = gid->elem.type;
		stack_top->u.ptr.type = LOAD_OBJECT;
		stack_top->u.ptr.field = gid->field;
		stack_top->u.ptr.rev_bo = gid->elem.rev_bo;
		break;
	}

	stack_top->type = REG_PTR;

	return 0;

end:
	return ret;
}

static int dynamic_load_field(struct estack_entry *stack_top)
{
	int ret;

	switch (stack_top->u.ptr.type) {
	case LOAD_OBJECT:
		break;
	case LOAD_ROOT_CONTEXT:
	case LOAD_ROOT_APP_CONTEXT:
	case LOAD_ROOT_PAYLOAD:
	default:
		dbg_printk("Bytecode warning: cannot load root, missing field name.\n");
		ret = -EINVAL;
		goto end;
	}
	switch (stack_top->u.ptr.object_type) {
	case OBJECT_TYPE_S8:
		dbg_printk("op load field s8\n");
		stack_top->u.v = *(int8_t *) stack_top->u.ptr.ptr;
		stack_top->type = REG_S64;
		break;
	case OBJECT_TYPE_S16:
	{
		int16_t tmp;

		dbg_printk("op load field s16\n");
		tmp = *(int16_t *) stack_top->u.ptr.ptr;
		if (stack_top->u.ptr.rev_bo)
			__swab16s(&tmp);
		stack_top->u.v = tmp;
		stack_top->type = REG_S64;
		break;
	}
	case OBJECT_TYPE_S32:
	{
		int32_t tmp;

		dbg_printk("op load field s32\n");
		tmp = *(int32_t *) stack_top->u.ptr.ptr;
		if (stack_top->u.ptr.rev_bo)
			__swab32s(&tmp);
		stack_top->u.v = tmp;
		stack_top->type = REG_S64;
		break;
	}
	case OBJECT_TYPE_S64:
	{
		int64_t tmp;

		dbg_printk("op load field s64\n");
		tmp = *(int64_t *) stack_top->u.ptr.ptr;
		if (stack_top->u.ptr.rev_bo)
			__swab64s(&tmp);
		stack_top->u.v = tmp;
		stack_top->type = REG_S64;
		break;
	}
	case OBJECT_TYPE_SIGNED_ENUM:
	{
		int64_t tmp;

		dbg_printk("op load field signed enumeration\n");
		tmp = *(int64_t *) stack_top->u.ptr.ptr;
		if (stack_top->u.ptr.rev_bo)
			__swab64s(&tmp);
		stack_top->u.v = tmp;
		stack_top->type = REG_S64;
		break;
	}
	case OBJECT_TYPE_U8:
		dbg_printk("op load field u8\n");
		stack_top->u.v = *(uint8_t *) stack_top->u.ptr.ptr;
		stack_top->type = REG_U64;
		break;
	case OBJECT_TYPE_U16:
	{
		uint16_t tmp;

		dbg_printk("op load field u16\n");
		tmp = *(uint16_t *) stack_top->u.ptr.ptr;
		if (stack_top->u.ptr.rev_bo)
			__swab16s(&tmp);
		stack_top->u.v = tmp;
		stack_top->type = REG_U64;
		break;
	}
	case OBJECT_TYPE_U32:
	{
		uint32_t tmp;

		dbg_printk("op load field u32\n");
		tmp = *(uint32_t *) stack_top->u.ptr.ptr;
		if (stack_top->u.ptr.rev_bo)
			__swab32s(&tmp);
		stack_top->u.v = tmp;
		stack_top->type = REG_U64;
		break;
	}
	case OBJECT_TYPE_U64:
	{
		uint64_t tmp;

		dbg_printk("op load field u64\n");
		tmp = *(uint64_t *) stack_top->u.ptr.ptr;
		if (stack_top->u.ptr.rev_bo)
			__swab64s(&tmp);
		stack_top->u.v = tmp;
		stack_top->type = REG_U64;
		break;
	}
	case OBJECT_TYPE_UNSIGNED_ENUM:
	{
		uint64_t tmp;

		dbg_printk("op load field unsigned enumeration\n");
		tmp = *(uint64_t *) stack_top->u.ptr.ptr;
		if (stack_top->u.ptr.rev_bo)
			__swab64s(&tmp);
		stack_top->u.v = tmp;
		stack_top->type = REG_U64;
		break;
	}
	case OBJECT_TYPE_STRING:
	{
		const char *str;

		dbg_printk("op load field string\n");
		str = (const char *) stack_top->u.ptr.ptr;
		stack_top->u.s.str = str;
		if (unlikely(!stack_top->u.s.str)) {
			dbg_printk("Bytecode warning: loading a NULL string.\n");
			ret = -EINVAL;
			goto end;
		}
		stack_top->u.s.seq_len = LTTNG_SIZE_MAX;
		stack_top->u.s.literal_type =
			ESTACK_STRING_LITERAL_TYPE_NONE;
		stack_top->type = REG_STRING;
		break;
	}
	case OBJECT_TYPE_STRING_SEQUENCE:
	{
		const char *ptr;

		dbg_printk("op load field string sequence\n");
		ptr = stack_top->u.ptr.ptr;
		stack_top->u.s.seq_len = *(unsigned long *) ptr;
		stack_top->u.s.str = *(const char **) (ptr + sizeof(unsigned long));
		if (unlikely(!stack_top->u.s.str)) {
			dbg_printk("Bytecode warning: loading a NULL sequence.\n");
			ret = -EINVAL;
			goto end;
		}
		stack_top->u.s.literal_type =
			ESTACK_STRING_LITERAL_TYPE_NONE;
		stack_top->type = REG_STRING;
		break;
	}
	case OBJECT_TYPE_DYNAMIC:
		/*
		 * Dynamic types in context are looked up
		 * by context get index.
		 */
		ret = -EINVAL;
		goto end;
	case OBJECT_TYPE_DOUBLE:
		ret = -EINVAL;
		goto end;
	case OBJECT_TYPE_SEQUENCE:
	case OBJECT_TYPE_ARRAY:
	case OBJECT_TYPE_STRUCT:
	case OBJECT_TYPE_VARIANT:
		printk(KERN_WARNING "LTTng: bytecode: Sequences, arrays, struct and variant cannot be loaded (nested types).\n");
		ret = -EINVAL;
		goto end;
	}
	return 0;

end:
	return ret;
}

static
int lttng_bytecode_interpret_format_output(struct estack_entry *ax,
		struct lttng_interpreter_output *output)
{
	int ret;

again:
	switch (ax->type) {
	case REG_S64:
		output->type = LTTNG_INTERPRETER_TYPE_S64;
		output->u.s = ax->u.v;
		break;
	case REG_U64:
		output->type = LTTNG_INTERPRETER_TYPE_U64;
		output->u.u = (uint64_t) ax->u.v;
		break;
	case REG_STRING:
		output->type = LTTNG_INTERPRETER_TYPE_STRING;
		output->u.str.str = ax->u.s.str;
		output->u.str.len = ax->u.s.seq_len;
		break;
	case REG_PTR:
		switch (ax->u.ptr.object_type) {
		case OBJECT_TYPE_S8:
		case OBJECT_TYPE_S16:
		case OBJECT_TYPE_S32:
		case OBJECT_TYPE_S64:
		case OBJECT_TYPE_U8:
		case OBJECT_TYPE_U16:
		case OBJECT_TYPE_U32:
		case OBJECT_TYPE_U64:
		case OBJECT_TYPE_DOUBLE:
		case OBJECT_TYPE_STRING:
		case OBJECT_TYPE_STRING_SEQUENCE:
			ret = dynamic_load_field(ax);
			if (ret)
				return ret;
			/* Retry after loading ptr into stack top. */
			goto again;
		case OBJECT_TYPE_SEQUENCE:
			output->type = LTTNG_INTERPRETER_TYPE_SEQUENCE;
			output->u.sequence.ptr = *(const char **) (ax->u.ptr.ptr + sizeof(unsigned long));
			output->u.sequence.nr_elem = *(unsigned long *) ax->u.ptr.ptr;
			output->u.sequence.nested_type = lttng_kernel_get_type_sequence(ax->u.ptr.field->type)->elem_type;
			break;
		case OBJECT_TYPE_ARRAY:
			/* Skip count (unsigned long) */
			output->type = LTTNG_INTERPRETER_TYPE_SEQUENCE;
			output->u.sequence.ptr = *(const char **) (ax->u.ptr.ptr + sizeof(unsigned long));
			output->u.sequence.nr_elem = lttng_kernel_get_type_array(ax->u.ptr.field->type)->length;
			output->u.sequence.nested_type = lttng_kernel_get_type_array(ax->u.ptr.field->type)->elem_type;
			break;
		case OBJECT_TYPE_SIGNED_ENUM:
			ret = dynamic_load_field(ax);
			if (ret)
				return ret;
			output->type = LTTNG_INTERPRETER_TYPE_SIGNED_ENUM;
			output->u.s = ax->u.v;
			break;
		case OBJECT_TYPE_UNSIGNED_ENUM:
			ret = dynamic_load_field(ax);
			if (ret)
				return ret;
			output->type = LTTNG_INTERPRETER_TYPE_UNSIGNED_ENUM;
			output->u.u = ax->u.v;
			break;
		case OBJECT_TYPE_STRUCT:
		case OBJECT_TYPE_VARIANT:
		default:
			return -EINVAL;
		}

		break;
	case REG_STAR_GLOB_STRING:
	case REG_TYPE_UNKNOWN:
	default:
		return -EINVAL;
	}

	return LTTNG_INTERPRETER_RECORD_FLAG;
}

#ifdef DEBUG

#define DBG_USER_STR_CUTOFF 32

/*
 * In debug mode, print user string (truncated, if necessary).
 */
static inline
void dbg_load_ref_user_str_printk(const struct estack_entry *user_str_reg)
{
	size_t pos = 0;
	char last_char;
	char user_str[DBG_USER_STR_CUTOFF];

	pagefault_disable();
	do {
		last_char = get_char(user_str_reg, pos);
		user_str[pos] = last_char;
		pos++;
	} while (last_char != '\0' && pos < sizeof(user_str));
	pagefault_enable();

	user_str[sizeof(user_str) - 1] = '\0';
	dbg_printk("load field ref user string: '%s%s'\n", user_str,
		last_char != '\0' ? "[...]" : "");
}
#else
static inline
void dbg_load_ref_user_str_printk(const struct estack_entry *user_str_reg)
{
}
#endif

/*
 * Return 0 (discard), or raise the 0x1 flag (log event).
 * Currently, other flags are kept for future extensions and have no
 * effect.
 */
static
uint64_t bytecode_interpret(void *interpreter_data,
		struct lttng_probe_ctx *lttng_probe_ctx,
		const char *interpreter_stack_data,
		struct lttng_interpreter_output *output)
{
	struct bytecode_runtime *bytecode = interpreter_data;
	void *pc, *next_pc, *start_pc;
	int ret = -EINVAL;
	uint64_t retval = 0;
	struct estack _stack;
	struct estack *stack = &_stack;
	register int64_t ax = 0, bx = 0;
	register enum entry_type ax_t = REG_TYPE_UNKNOWN, bx_t = REG_TYPE_UNKNOWN;
	register int top = INTERPRETER_STACK_EMPTY;
#ifndef INTERPRETER_USE_SWITCH
	static void *dispatch[NR_BYTECODE_OPS] = {
		[ BYTECODE_OP_UNKNOWN ] = &&LABEL_BYTECODE_OP_UNKNOWN,

		[ BYTECODE_OP_RETURN ] = &&LABEL_BYTECODE_OP_RETURN,

		/* binary */
		[ BYTECODE_OP_MUL ] = &&LABEL_BYTECODE_OP_MUL,
		[ BYTECODE_OP_DIV ] = &&LABEL_BYTECODE_OP_DIV,
		[ BYTECODE_OP_MOD ] = &&LABEL_BYTECODE_OP_MOD,
		[ BYTECODE_OP_PLUS ] = &&LABEL_BYTECODE_OP_PLUS,
		[ BYTECODE_OP_MINUS ] = &&LABEL_BYTECODE_OP_MINUS,
		[ BYTECODE_OP_BIT_RSHIFT ] = &&LABEL_BYTECODE_OP_BIT_RSHIFT,
		[ BYTECODE_OP_BIT_LSHIFT ] = &&LABEL_BYTECODE_OP_BIT_LSHIFT,
		[ BYTECODE_OP_BIT_AND ] = &&LABEL_BYTECODE_OP_BIT_AND,
		[ BYTECODE_OP_BIT_OR ] = &&LABEL_BYTECODE_OP_BIT_OR,
		[ BYTECODE_OP_BIT_XOR ] = &&LABEL_BYTECODE_OP_BIT_XOR,

		/* binary comparators */
		[ BYTECODE_OP_EQ ] = &&LABEL_BYTECODE_OP_EQ,
		[ BYTECODE_OP_NE ] = &&LABEL_BYTECODE_OP_NE,
		[ BYTECODE_OP_GT ] = &&LABEL_BYTECODE_OP_GT,
		[ BYTECODE_OP_LT ] = &&LABEL_BYTECODE_OP_LT,
		[ BYTECODE_OP_GE ] = &&LABEL_BYTECODE_OP_GE,
		[ BYTECODE_OP_LE ] = &&LABEL_BYTECODE_OP_LE,

		/* string binary comparator */
		[ BYTECODE_OP_EQ_STRING ] = &&LABEL_BYTECODE_OP_EQ_STRING,
		[ BYTECODE_OP_NE_STRING ] = &&LABEL_BYTECODE_OP_NE_STRING,
		[ BYTECODE_OP_GT_STRING ] = &&LABEL_BYTECODE_OP_GT_STRING,
		[ BYTECODE_OP_LT_STRING ] = &&LABEL_BYTECODE_OP_LT_STRING,
		[ BYTECODE_OP_GE_STRING ] = &&LABEL_BYTECODE_OP_GE_STRING,
		[ BYTECODE_OP_LE_STRING ] = &&LABEL_BYTECODE_OP_LE_STRING,

		/* globbing pattern binary comparator */
		[ BYTECODE_OP_EQ_STAR_GLOB_STRING ] = &&LABEL_BYTECODE_OP_EQ_STAR_GLOB_STRING,
		[ BYTECODE_OP_NE_STAR_GLOB_STRING ] = &&LABEL_BYTECODE_OP_NE_STAR_GLOB_STRING,

		/* s64 binary comparator */
		[ BYTECODE_OP_EQ_S64 ] = &&LABEL_BYTECODE_OP_EQ_S64,
		[ BYTECODE_OP_NE_S64 ] = &&LABEL_BYTECODE_OP_NE_S64,
		[ BYTECODE_OP_GT_S64 ] = &&LABEL_BYTECODE_OP_GT_S64,
		[ BYTECODE_OP_LT_S64 ] = &&LABEL_BYTECODE_OP_LT_S64,
		[ BYTECODE_OP_GE_S64 ] = &&LABEL_BYTECODE_OP_GE_S64,
		[ BYTECODE_OP_LE_S64 ] = &&LABEL_BYTECODE_OP_LE_S64,

		/* double binary comparator */
		[ BYTECODE_OP_EQ_DOUBLE ] = &&LABEL_BYTECODE_OP_EQ_DOUBLE,
		[ BYTECODE_OP_NE_DOUBLE ] = &&LABEL_BYTECODE_OP_NE_DOUBLE,
		[ BYTECODE_OP_GT_DOUBLE ] = &&LABEL_BYTECODE_OP_GT_DOUBLE,
		[ BYTECODE_OP_LT_DOUBLE ] = &&LABEL_BYTECODE_OP_LT_DOUBLE,
		[ BYTECODE_OP_GE_DOUBLE ] = &&LABEL_BYTECODE_OP_GE_DOUBLE,
		[ BYTECODE_OP_LE_DOUBLE ] = &&LABEL_BYTECODE_OP_LE_DOUBLE,

		/* Mixed S64-double binary comparators */
		[ BYTECODE_OP_EQ_DOUBLE_S64 ] = &&LABEL_BYTECODE_OP_EQ_DOUBLE_S64,
		[ BYTECODE_OP_NE_DOUBLE_S64 ] = &&LABEL_BYTECODE_OP_NE_DOUBLE_S64,
		[ BYTECODE_OP_GT_DOUBLE_S64 ] = &&LABEL_BYTECODE_OP_GT_DOUBLE_S64,
		[ BYTECODE_OP_LT_DOUBLE_S64 ] = &&LABEL_BYTECODE_OP_LT_DOUBLE_S64,
		[ BYTECODE_OP_GE_DOUBLE_S64 ] = &&LABEL_BYTECODE_OP_GE_DOUBLE_S64,
		[ BYTECODE_OP_LE_DOUBLE_S64 ] = &&LABEL_BYTECODE_OP_LE_DOUBLE_S64,

		[ BYTECODE_OP_EQ_S64_DOUBLE ] = &&LABEL_BYTECODE_OP_EQ_S64_DOUBLE,
		[ BYTECODE_OP_NE_S64_DOUBLE ] = &&LABEL_BYTECODE_OP_NE_S64_DOUBLE,
		[ BYTECODE_OP_GT_S64_DOUBLE ] = &&LABEL_BYTECODE_OP_GT_S64_DOUBLE,
		[ BYTECODE_OP_LT_S64_DOUBLE ] = &&LABEL_BYTECODE_OP_LT_S64_DOUBLE,
		[ BYTECODE_OP_GE_S64_DOUBLE ] = &&LABEL_BYTECODE_OP_GE_S64_DOUBLE,
		[ BYTECODE_OP_LE_S64_DOUBLE ] = &&LABEL_BYTECODE_OP_LE_S64_DOUBLE,

		/* unary */
		[ BYTECODE_OP_UNARY_PLUS ] = &&LABEL_BYTECODE_OP_UNARY_PLUS,
		[ BYTECODE_OP_UNARY_MINUS ] = &&LABEL_BYTECODE_OP_UNARY_MINUS,
		[ BYTECODE_OP_UNARY_NOT ] = &&LABEL_BYTECODE_OP_UNARY_NOT,
		[ BYTECODE_OP_UNARY_PLUS_S64 ] = &&LABEL_BYTECODE_OP_UNARY_PLUS_S64,
		[ BYTECODE_OP_UNARY_MINUS_S64 ] = &&LABEL_BYTECODE_OP_UNARY_MINUS_S64,
		[ BYTECODE_OP_UNARY_NOT_S64 ] = &&LABEL_BYTECODE_OP_UNARY_NOT_S64,
		[ BYTECODE_OP_UNARY_PLUS_DOUBLE ] = &&LABEL_BYTECODE_OP_UNARY_PLUS_DOUBLE,
		[ BYTECODE_OP_UNARY_MINUS_DOUBLE ] = &&LABEL_BYTECODE_OP_UNARY_MINUS_DOUBLE,
		[ BYTECODE_OP_UNARY_NOT_DOUBLE ] = &&LABEL_BYTECODE_OP_UNARY_NOT_DOUBLE,

		/* logical */
		[ BYTECODE_OP_AND ] = &&LABEL_BYTECODE_OP_AND,
		[ BYTECODE_OP_OR ] = &&LABEL_BYTECODE_OP_OR,

		/* load field ref */
		[ BYTECODE_OP_LOAD_FIELD_REF ] = &&LABEL_BYTECODE_OP_LOAD_FIELD_REF,
		[ BYTECODE_OP_LOAD_FIELD_REF_STRING ] = &&LABEL_BYTECODE_OP_LOAD_FIELD_REF_STRING,
		[ BYTECODE_OP_LOAD_FIELD_REF_SEQUENCE ] = &&LABEL_BYTECODE_OP_LOAD_FIELD_REF_SEQUENCE,
		[ BYTECODE_OP_LOAD_FIELD_REF_S64 ] = &&LABEL_BYTECODE_OP_LOAD_FIELD_REF_S64,
		[ BYTECODE_OP_LOAD_FIELD_REF_DOUBLE ] = &&LABEL_BYTECODE_OP_LOAD_FIELD_REF_DOUBLE,

		/* load from immediate operand */
		[ BYTECODE_OP_LOAD_STRING ] = &&LABEL_BYTECODE_OP_LOAD_STRING,
		[ BYTECODE_OP_LOAD_STAR_GLOB_STRING ] = &&LABEL_BYTECODE_OP_LOAD_STAR_GLOB_STRING,
		[ BYTECODE_OP_LOAD_S64 ] = &&LABEL_BYTECODE_OP_LOAD_S64,
		[ BYTECODE_OP_LOAD_DOUBLE ] = &&LABEL_BYTECODE_OP_LOAD_DOUBLE,

		/* cast */
		[ BYTECODE_OP_CAST_TO_S64 ] = &&LABEL_BYTECODE_OP_CAST_TO_S64,
		[ BYTECODE_OP_CAST_DOUBLE_TO_S64 ] = &&LABEL_BYTECODE_OP_CAST_DOUBLE_TO_S64,
		[ BYTECODE_OP_CAST_NOP ] = &&LABEL_BYTECODE_OP_CAST_NOP,

		/* get context ref */
		[ BYTECODE_OP_GET_CONTEXT_REF ] = &&LABEL_BYTECODE_OP_GET_CONTEXT_REF,
		[ BYTECODE_OP_GET_CONTEXT_REF_STRING ] = &&LABEL_BYTECODE_OP_GET_CONTEXT_REF_STRING,
		[ BYTECODE_OP_GET_CONTEXT_REF_S64 ] = &&LABEL_BYTECODE_OP_GET_CONTEXT_REF_S64,
		[ BYTECODE_OP_GET_CONTEXT_REF_DOUBLE ] = &&LABEL_BYTECODE_OP_GET_CONTEXT_REF_DOUBLE,

		/* load userspace field ref */
		[ BYTECODE_OP_LOAD_FIELD_REF_USER_STRING ] = &&LABEL_BYTECODE_OP_LOAD_FIELD_REF_USER_STRING,
		[ BYTECODE_OP_LOAD_FIELD_REF_USER_SEQUENCE ] = &&LABEL_BYTECODE_OP_LOAD_FIELD_REF_USER_SEQUENCE,

		/* Instructions for recursive traversal through composed types. */
		[ BYTECODE_OP_GET_CONTEXT_ROOT ] = &&LABEL_BYTECODE_OP_GET_CONTEXT_ROOT,
		[ BYTECODE_OP_GET_APP_CONTEXT_ROOT ] = &&LABEL_BYTECODE_OP_GET_APP_CONTEXT_ROOT,
		[ BYTECODE_OP_GET_PAYLOAD_ROOT ] = &&LABEL_BYTECODE_OP_GET_PAYLOAD_ROOT,

		[ BYTECODE_OP_GET_SYMBOL ] = &&LABEL_BYTECODE_OP_GET_SYMBOL,
		[ BYTECODE_OP_GET_SYMBOL_FIELD ] = &&LABEL_BYTECODE_OP_GET_SYMBOL_FIELD,
		[ BYTECODE_OP_GET_INDEX_U16 ] = &&LABEL_BYTECODE_OP_GET_INDEX_U16,
		[ BYTECODE_OP_GET_INDEX_U64 ] = &&LABEL_BYTECODE_OP_GET_INDEX_U64,

		[ BYTECODE_OP_LOAD_FIELD ] = &&LABEL_BYTECODE_OP_LOAD_FIELD,
		[ BYTECODE_OP_LOAD_FIELD_S8	 ] = &&LABEL_BYTECODE_OP_LOAD_FIELD_S8,
		[ BYTECODE_OP_LOAD_FIELD_S16 ] = &&LABEL_BYTECODE_OP_LOAD_FIELD_S16,
		[ BYTECODE_OP_LOAD_FIELD_S32 ] = &&LABEL_BYTECODE_OP_LOAD_FIELD_S32,
		[ BYTECODE_OP_LOAD_FIELD_S64 ] = &&LABEL_BYTECODE_OP_LOAD_FIELD_S64,
		[ BYTECODE_OP_LOAD_FIELD_U8 ] = &&LABEL_BYTECODE_OP_LOAD_FIELD_U8,
		[ BYTECODE_OP_LOAD_FIELD_U16 ] = &&LABEL_BYTECODE_OP_LOAD_FIELD_U16,
		[ BYTECODE_OP_LOAD_FIELD_U32 ] = &&LABEL_BYTECODE_OP_LOAD_FIELD_U32,
		[ BYTECODE_OP_LOAD_FIELD_U64 ] = &&LABEL_BYTECODE_OP_LOAD_FIELD_U64,
		[ BYTECODE_OP_LOAD_FIELD_STRING ] = &&LABEL_BYTECODE_OP_LOAD_FIELD_STRING,
		[ BYTECODE_OP_LOAD_FIELD_SEQUENCE ] = &&LABEL_BYTECODE_OP_LOAD_FIELD_SEQUENCE,
		[ BYTECODE_OP_LOAD_FIELD_DOUBLE ] = &&LABEL_BYTECODE_OP_LOAD_FIELD_DOUBLE,

		[ BYTECODE_OP_UNARY_BIT_NOT ] = &&LABEL_BYTECODE_OP_UNARY_BIT_NOT,

		[ BYTECODE_OP_RETURN_S64 ] = &&LABEL_BYTECODE_OP_RETURN_S64,
	};
#endif /* #ifndef INTERPRETER_USE_SWITCH */

	START_OP

		OP(BYTECODE_OP_UNKNOWN):
		OP(BYTECODE_OP_LOAD_FIELD_REF):
		OP(BYTECODE_OP_GET_CONTEXT_REF):
#ifdef INTERPRETER_USE_SWITCH
		default:
#endif /* INTERPRETER_USE_SWITCH */
			printk(KERN_WARNING "LTTng: bytecode: unknown bytecode op %u\n",
				(unsigned int) *(bytecode_opcode_t *) pc);
			ret = -EINVAL;
			goto end;

		OP(BYTECODE_OP_RETURN):
		OP(BYTECODE_OP_RETURN_S64):
			/* LTTNG_INTERPRETER_DISCARD or LTTNG_INTERPRETER_RECORD_FLAG */
			switch (estack_ax_t) {
			case REG_S64:
			case REG_U64:
				retval = !!estack_ax_v;
				break;
			case REG_DOUBLE:
			case REG_STRING:
			case REG_PTR:
				if (!output) {
					ret = -EINVAL;
					goto end;
				}
				retval = 0;
				break;
			case REG_STAR_GLOB_STRING:
			case REG_TYPE_UNKNOWN:
				ret = -EINVAL;
				goto end;
			}
			ret = 0;
			goto end;

		/* binary */
		OP(BYTECODE_OP_MUL):
		OP(BYTECODE_OP_DIV):
		OP(BYTECODE_OP_MOD):
		OP(BYTECODE_OP_PLUS):
		OP(BYTECODE_OP_MINUS):
			printk(KERN_WARNING "LTTng: bytecode: unsupported bytecode op %u\n",
				(unsigned int) *(bytecode_opcode_t *) pc);
			ret = -EINVAL;
			goto end;

		OP(BYTECODE_OP_EQ):
		OP(BYTECODE_OP_NE):
		OP(BYTECODE_OP_GT):
		OP(BYTECODE_OP_LT):
		OP(BYTECODE_OP_GE):
		OP(BYTECODE_OP_LE):
			printk(KERN_WARNING "LTTng: bytecode: unsupported non-specialized bytecode op %u\n",
				(unsigned int) *(bytecode_opcode_t *) pc);
			ret = -EINVAL;
			goto end;

		OP(BYTECODE_OP_EQ_STRING):
		{
			int res;

			res = (stack_strcmp(stack, top, "==") == 0);
			estack_pop(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = res;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct binary_op);
			PO;
		}
		OP(BYTECODE_OP_NE_STRING):
		{
			int res;

			res = (stack_strcmp(stack, top, "!=") != 0);
			estack_pop(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = res;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct binary_op);
			PO;
		}
		OP(BYTECODE_OP_GT_STRING):
		{
			int res;

			res = (stack_strcmp(stack, top, ">") > 0);
			estack_pop(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = res;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct binary_op);
			PO;
		}
		OP(BYTECODE_OP_LT_STRING):
		{
			int res;

			res = (stack_strcmp(stack, top, "<") < 0);
			estack_pop(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = res;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct binary_op);
			PO;
		}
		OP(BYTECODE_OP_GE_STRING):
		{
			int res;

			res = (stack_strcmp(stack, top, ">=") >= 0);
			estack_pop(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = res;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct binary_op);
			PO;
		}
		OP(BYTECODE_OP_LE_STRING):
		{
			int res;

			res = (stack_strcmp(stack, top, "<=") <= 0);
			estack_pop(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = res;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct binary_op);
			PO;
		}

		OP(BYTECODE_OP_EQ_STAR_GLOB_STRING):
		{
			int res;

			res = (stack_star_glob_match(stack, top, "==") == 0);
			estack_pop(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = res;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct binary_op);
			PO;
		}
		OP(BYTECODE_OP_NE_STAR_GLOB_STRING):
		{
			int res;

			res = (stack_star_glob_match(stack, top, "!=") != 0);
			estack_pop(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = res;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct binary_op);
			PO;
		}

		OP(BYTECODE_OP_EQ_S64):
		{
			int res;

			res = (estack_bx_v == estack_ax_v);
			estack_pop(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = res;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct binary_op);
			PO;
		}
		OP(BYTECODE_OP_NE_S64):
		{
			int res;

			res = (estack_bx_v != estack_ax_v);
			estack_pop(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = res;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct binary_op);
			PO;
		}
		OP(BYTECODE_OP_GT_S64):
		{
			int res;

			res = (estack_bx_v > estack_ax_v);
			estack_pop(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = res;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct binary_op);
			PO;
		}
		OP(BYTECODE_OP_LT_S64):
		{
			int res;

			res = (estack_bx_v < estack_ax_v);
			estack_pop(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = res;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct binary_op);
			PO;
		}
		OP(BYTECODE_OP_GE_S64):
		{
			int res;

			res = (estack_bx_v >= estack_ax_v);
			estack_pop(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = res;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct binary_op);
			PO;
		}
		OP(BYTECODE_OP_LE_S64):
		{
			int res;

			res = (estack_bx_v <= estack_ax_v);
			estack_pop(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = res;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct binary_op);
			PO;
		}

		OP(BYTECODE_OP_EQ_DOUBLE):
		OP(BYTECODE_OP_NE_DOUBLE):
		OP(BYTECODE_OP_GT_DOUBLE):
		OP(BYTECODE_OP_LT_DOUBLE):
		OP(BYTECODE_OP_GE_DOUBLE):
		OP(BYTECODE_OP_LE_DOUBLE):
		{
			BUG_ON(1);
			PO;
		}

		/* Mixed S64-double binary comparators */
		OP(BYTECODE_OP_EQ_DOUBLE_S64):
		OP(BYTECODE_OP_NE_DOUBLE_S64):
		OP(BYTECODE_OP_GT_DOUBLE_S64):
		OP(BYTECODE_OP_LT_DOUBLE_S64):
		OP(BYTECODE_OP_GE_DOUBLE_S64):
		OP(BYTECODE_OP_LE_DOUBLE_S64):
		OP(BYTECODE_OP_EQ_S64_DOUBLE):
		OP(BYTECODE_OP_NE_S64_DOUBLE):
		OP(BYTECODE_OP_GT_S64_DOUBLE):
		OP(BYTECODE_OP_LT_S64_DOUBLE):
		OP(BYTECODE_OP_GE_S64_DOUBLE):
		OP(BYTECODE_OP_LE_S64_DOUBLE):
		{
			BUG_ON(1);
			PO;
		}
		OP(BYTECODE_OP_BIT_RSHIFT):
		{
			int64_t res;

			if (!IS_INTEGER_REGISTER(estack_ax_t) || !IS_INTEGER_REGISTER(estack_bx_t)) {
				ret = -EINVAL;
				goto end;
			}

			/* Catch undefined behavior. */
			if (unlikely(estack_ax_v < 0 || estack_ax_v >= 64)) {
				ret = -EINVAL;
				goto end;
			}
			res = ((uint64_t) estack_bx_v >> (uint32_t) estack_ax_v);
			estack_pop(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = res;
			estack_ax_t = REG_U64;
			next_pc += sizeof(struct binary_op);
			PO;
		}
		OP(BYTECODE_OP_BIT_LSHIFT):
		{
			int64_t res;

			if (!IS_INTEGER_REGISTER(estack_ax_t) || !IS_INTEGER_REGISTER(estack_bx_t)) {
				ret = -EINVAL;
				goto end;
			}

			/* Catch undefined behavior. */
			if (unlikely(estack_ax_v < 0 || estack_ax_v >= 64)) {
				ret = -EINVAL;
				goto end;
			}
			res = ((uint64_t) estack_bx_v << (uint32_t) estack_ax_v);
			estack_pop(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = res;
			estack_ax_t = REG_U64;
			next_pc += sizeof(struct binary_op);
			PO;
		}
		OP(BYTECODE_OP_BIT_AND):
		{
			int64_t res;

			if (!IS_INTEGER_REGISTER(estack_ax_t) || !IS_INTEGER_REGISTER(estack_bx_t)) {
				ret = -EINVAL;
				goto end;
			}

			res = ((uint64_t) estack_bx_v & (uint64_t) estack_ax_v);
			estack_pop(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = res;
			estack_ax_t = REG_U64;
			next_pc += sizeof(struct binary_op);
			PO;
		}
		OP(BYTECODE_OP_BIT_OR):
		{
			int64_t res;

			if (!IS_INTEGER_REGISTER(estack_ax_t) || !IS_INTEGER_REGISTER(estack_bx_t)) {
				ret = -EINVAL;
				goto end;
			}

			res = ((uint64_t) estack_bx_v | (uint64_t) estack_ax_v);
			estack_pop(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = res;
			estack_ax_t = REG_U64;
			next_pc += sizeof(struct binary_op);
			PO;
		}
		OP(BYTECODE_OP_BIT_XOR):
		{
			int64_t res;

			if (!IS_INTEGER_REGISTER(estack_ax_t) || !IS_INTEGER_REGISTER(estack_bx_t)) {
				ret = -EINVAL;
				goto end;
			}

			res = ((uint64_t) estack_bx_v ^ (uint64_t) estack_ax_v);
			estack_pop(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = res;
			estack_ax_t = REG_U64;
			next_pc += sizeof(struct binary_op);
			PO;
		}

		/* unary */
		OP(BYTECODE_OP_UNARY_PLUS):
		OP(BYTECODE_OP_UNARY_MINUS):
		OP(BYTECODE_OP_UNARY_NOT):
			printk(KERN_WARNING "LTTng: bytecode: unsupported non-specialized bytecode op %u\n",
				(unsigned int) *(bytecode_opcode_t *) pc);
			ret = -EINVAL;
			goto end;


		OP(BYTECODE_OP_UNARY_BIT_NOT):
		{
			estack_ax_v = ~(uint64_t) estack_ax_v;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct unary_op);
			PO;
		}

		OP(BYTECODE_OP_UNARY_PLUS_S64):
		{
			next_pc += sizeof(struct unary_op);
			PO;
		}
		OP(BYTECODE_OP_UNARY_MINUS_S64):
		{
			estack_ax_v = -estack_ax_v;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct unary_op);
			PO;
		}
		OP(BYTECODE_OP_UNARY_PLUS_DOUBLE):
		OP(BYTECODE_OP_UNARY_MINUS_DOUBLE):
		{
			BUG_ON(1);
			PO;
		}
		OP(BYTECODE_OP_UNARY_NOT_S64):
		{
			estack_ax_v = !estack_ax_v;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct unary_op);
			PO;
		}
		OP(BYTECODE_OP_UNARY_NOT_DOUBLE):
		{
			BUG_ON(1);
			PO;
		}

		/* logical */
		OP(BYTECODE_OP_AND):
		{
			struct logical_op *insn = (struct logical_op *) pc;

			/* If AX is 0, skip and evaluate to 0 */
			if (unlikely(estack_ax_v == 0)) {
				dbg_printk("Jumping to bytecode offset %u\n",
					(unsigned int) insn->skip_offset);
				next_pc = start_pc + insn->skip_offset;
			} else {
				/* Pop 1 when jump not taken */
				estack_pop(stack, top, ax, bx, ax_t, bx_t);
				next_pc += sizeof(struct logical_op);
			}
			PO;
		}
		OP(BYTECODE_OP_OR):
		{
			struct logical_op *insn = (struct logical_op *) pc;

			/* If AX is nonzero, skip and evaluate to 1 */

			if (unlikely(estack_ax_v != 0)) {
				estack_ax_v = 1;
				dbg_printk("Jumping to bytecode offset %u\n",
					(unsigned int) insn->skip_offset);
				next_pc = start_pc + insn->skip_offset;
			} else {
				/* Pop 1 when jump not taken */
				estack_pop(stack, top, ax, bx, ax_t, bx_t);
				next_pc += sizeof(struct logical_op);
			}
			PO;
		}


		/* load field ref */
		OP(BYTECODE_OP_LOAD_FIELD_REF_STRING):
		{
			struct load_op *insn = (struct load_op *) pc;
			struct field_ref *ref = (struct field_ref *) insn->data;

			dbg_printk("load field ref offset %u type string\n",
				ref->offset);
			estack_push(stack, top, ax, bx, ax_t, bx_t);
			estack_ax(stack, top)->u.s.str =
				*(const char * const *) &interpreter_stack_data[ref->offset];
			if (unlikely(!estack_ax(stack, top)->u.s.str)) {
				dbg_printk("Bytecode warning: loading a NULL string.\n");
				ret = -EINVAL;
				goto end;
			}
			estack_ax(stack, top)->u.s.seq_len = LTTNG_SIZE_MAX;
			estack_ax(stack, top)->u.s.literal_type =
				ESTACK_STRING_LITERAL_TYPE_NONE;
			estack_ax(stack, top)->u.s.user = 0;
			estack_ax(stack, top)->type = REG_STRING;
			dbg_printk("ref load string %s\n", estack_ax(stack, top)->u.s.str);
			next_pc += sizeof(struct load_op) + sizeof(struct field_ref);
			PO;
		}

		OP(BYTECODE_OP_LOAD_FIELD_REF_SEQUENCE):
		{
			struct load_op *insn = (struct load_op *) pc;
			struct field_ref *ref = (struct field_ref *) insn->data;

			dbg_printk("load field ref offset %u type sequence\n",
				ref->offset);
			estack_push(stack, top, ax, bx, ax_t, bx_t);
			estack_ax(stack, top)->u.s.seq_len =
				*(unsigned long *) &interpreter_stack_data[ref->offset];
			estack_ax(stack, top)->u.s.str =
				*(const char **) (&interpreter_stack_data[ref->offset
								+ sizeof(unsigned long)]);
			if (unlikely(!estack_ax(stack, top)->u.s.str)) {
				dbg_printk("Bytecode warning: loading a NULL sequence.\n");
				ret = -EINVAL;
				goto end;
			}
			estack_ax(stack, top)->u.s.literal_type =
				ESTACK_STRING_LITERAL_TYPE_NONE;
			estack_ax(stack, top)->u.s.user = 0;
			next_pc += sizeof(struct load_op) + sizeof(struct field_ref);
			PO;
		}

		OP(BYTECODE_OP_LOAD_FIELD_REF_S64):
		{
			struct load_op *insn = (struct load_op *) pc;
			struct field_ref *ref = (struct field_ref *) insn->data;

			dbg_printk("load field ref offset %u type s64\n",
				ref->offset);
			estack_push(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v =
				((struct literal_numeric *) &interpreter_stack_data[ref->offset])->v;
			estack_ax_t = REG_S64;
			dbg_printk("ref load s64 %lld\n",
				(long long) estack_ax_v);
			next_pc += sizeof(struct load_op) + sizeof(struct field_ref);
			PO;
		}

		OP(BYTECODE_OP_LOAD_FIELD_REF_DOUBLE):
		{
			BUG_ON(1);
			PO;
		}

		/* load from immediate operand */
		OP(BYTECODE_OP_LOAD_STRING):
		{
			struct load_op *insn = (struct load_op *) pc;

			dbg_printk("load string %s\n", insn->data);
			estack_push(stack, top, ax, bx, ax_t, bx_t);
			estack_ax(stack, top)->u.s.str = insn->data;
			estack_ax(stack, top)->u.s.seq_len = LTTNG_SIZE_MAX;
			estack_ax(stack, top)->u.s.literal_type =
				ESTACK_STRING_LITERAL_TYPE_PLAIN;
			estack_ax(stack, top)->u.s.user = 0;
			next_pc += sizeof(struct load_op) + strlen(insn->data) + 1;
			PO;
		}

		OP(BYTECODE_OP_LOAD_STAR_GLOB_STRING):
		{
			struct load_op *insn = (struct load_op *) pc;

			dbg_printk("load globbing pattern %s\n", insn->data);
			estack_push(stack, top, ax, bx, ax_t, bx_t);
			estack_ax(stack, top)->u.s.str = insn->data;
			estack_ax(stack, top)->u.s.seq_len = LTTNG_SIZE_MAX;
			estack_ax(stack, top)->u.s.literal_type =
				ESTACK_STRING_LITERAL_TYPE_STAR_GLOB;
			estack_ax(stack, top)->u.s.user = 0;
			next_pc += sizeof(struct load_op) + strlen(insn->data) + 1;
			PO;
		}

		OP(BYTECODE_OP_LOAD_S64):
		{
			struct load_op *insn = (struct load_op *) pc;

			estack_push(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = ((struct literal_numeric *) insn->data)->v;
			estack_ax_t = REG_S64;
			dbg_printk("load s64 %lld\n",
				(long long) estack_ax_v);
			next_pc += sizeof(struct load_op)
					+ sizeof(struct literal_numeric);
			PO;
		}

		OP(BYTECODE_OP_LOAD_DOUBLE):
		{
			BUG_ON(1);
			PO;
		}

		/* cast */
		OP(BYTECODE_OP_CAST_TO_S64):
			printk(KERN_WARNING "LTTng: bytecode: unsupported non-specialized bytecode op %u\n",
				(unsigned int) *(bytecode_opcode_t *) pc);
			ret = -EINVAL;
			goto end;

		OP(BYTECODE_OP_CAST_DOUBLE_TO_S64):
		{
			BUG_ON(1);
			PO;
		}

		OP(BYTECODE_OP_CAST_NOP):
		{
			next_pc += sizeof(struct cast_op);
			PO;
		}

		/* get context ref */
		OP(BYTECODE_OP_GET_CONTEXT_REF_STRING):
		{
			struct load_op *insn = (struct load_op *) pc;
			struct field_ref *ref = (struct field_ref *) insn->data;
			struct lttng_kernel_ctx_field *ctx_field;
			union lttng_ctx_value v;

			dbg_printk("get context ref offset %u type string\n",
				ref->offset);
			ctx_field = &lttng_static_ctx->fields[ref->offset];
			ctx_field->get_value(ctx_field, lttng_probe_ctx, &v);
			estack_push(stack, top, ax, bx, ax_t, bx_t);
			estack_ax(stack, top)->u.s.str = v.str;
			if (unlikely(!estack_ax(stack, top)->u.s.str)) {
				dbg_printk("Bytecode warning: loading a NULL string.\n");
				ret = -EINVAL;
				goto end;
			}
			estack_ax(stack, top)->u.s.seq_len = LTTNG_SIZE_MAX;
			estack_ax(stack, top)->u.s.literal_type =
				ESTACK_STRING_LITERAL_TYPE_NONE;
			estack_ax(stack, top)->u.s.user = 0;
			estack_ax(stack, top)->type = REG_STRING;
			dbg_printk("ref get context string %s\n", estack_ax(stack, top)->u.s.str);
			next_pc += sizeof(struct load_op) + sizeof(struct field_ref);
			PO;
		}

		OP(BYTECODE_OP_GET_CONTEXT_REF_S64):
		{
			struct load_op *insn = (struct load_op *) pc;
			struct field_ref *ref = (struct field_ref *) insn->data;
			struct lttng_kernel_ctx_field *ctx_field;
			union lttng_ctx_value v;

			dbg_printk("get context ref offset %u type s64\n",
				ref->offset);
			ctx_field = &lttng_static_ctx->fields[ref->offset];
			ctx_field->get_value(ctx_field, lttng_probe_ctx, &v);
			estack_push(stack, top, ax, bx, ax_t, bx_t);
			estack_ax_v = v.s64;
			estack_ax_t = REG_S64;
			dbg_printk("ref get context s64 %lld\n",
				(long long) estack_ax_v);
			next_pc += sizeof(struct load_op) + sizeof(struct field_ref);
			PO;
		}

		OP(BYTECODE_OP_GET_CONTEXT_REF_DOUBLE):
		{
			BUG_ON(1);
			PO;
		}

		/* load userspace field ref */
		OP(BYTECODE_OP_LOAD_FIELD_REF_USER_STRING):
		{
			struct load_op *insn = (struct load_op *) pc;
			struct field_ref *ref = (struct field_ref *) insn->data;

			dbg_printk("load field ref offset %u type user string\n",
				ref->offset);
			estack_push(stack, top, ax, bx, ax_t, bx_t);
			estack_ax(stack, top)->u.s.user_str =
				*(const char * const *) &interpreter_stack_data[ref->offset];
			if (unlikely(!estack_ax(stack, top)->u.s.user_str)) {
				dbg_printk("Bytecode warning: loading a NULL string.\n");
				ret = -EINVAL;
				goto end;
			}
			estack_ax(stack, top)->u.s.seq_len = LTTNG_SIZE_MAX;
			estack_ax(stack, top)->u.s.literal_type =
				ESTACK_STRING_LITERAL_TYPE_NONE;
			estack_ax(stack, top)->u.s.user = 1;
			estack_ax(stack, top)->type = REG_STRING;
			dbg_load_ref_user_str_printk(estack_ax(stack, top));
			next_pc += sizeof(struct load_op) + sizeof(struct field_ref);
			PO;
		}

		OP(BYTECODE_OP_LOAD_FIELD_REF_USER_SEQUENCE):
		{
			struct load_op *insn = (struct load_op *) pc;
			struct field_ref *ref = (struct field_ref *) insn->data;

			dbg_printk("load field ref offset %u type user sequence\n",
				ref->offset);
			estack_push(stack, top, ax, bx, ax_t, bx_t);
			estack_ax(stack, top)->u.s.seq_len =
				*(unsigned long *) &interpreter_stack_data[ref->offset];
			estack_ax(stack, top)->u.s.user_str =
				*(const char **) (&interpreter_stack_data[ref->offset
								+ sizeof(unsigned long)]);
			if (unlikely(!estack_ax(stack, top)->u.s.user_str)) {
				dbg_printk("Bytecode warning: loading a NULL sequence.\n");
				ret = -EINVAL;
				goto end;
			}
			estack_ax(stack, top)->u.s.literal_type =
				ESTACK_STRING_LITERAL_TYPE_NONE;
			estack_ax(stack, top)->u.s.user = 1;
			next_pc += sizeof(struct load_op) + sizeof(struct field_ref);
			PO;
		}

		OP(BYTECODE_OP_GET_CONTEXT_ROOT):
		{
			dbg_printk("op get context root\n");
			estack_push(stack, top, ax, bx, ax_t, bx_t);
			estack_ax(stack, top)->u.ptr.type = LOAD_ROOT_CONTEXT;
			/* "field" only needed for variants. */
			estack_ax(stack, top)->u.ptr.field = NULL;
			estack_ax(stack, top)->type = REG_PTR;
			next_pc += sizeof(struct load_op);
			PO;
		}

		OP(BYTECODE_OP_GET_APP_CONTEXT_ROOT):
		{
			BUG_ON(1);
			PO;
		}

		OP(BYTECODE_OP_GET_PAYLOAD_ROOT):
		{
			dbg_printk("op get app payload root\n");
			estack_push(stack, top, ax, bx, ax_t, bx_t);
			estack_ax(stack, top)->u.ptr.type = LOAD_ROOT_PAYLOAD;
			estack_ax(stack, top)->u.ptr.ptr = interpreter_stack_data;
			/* "field" only needed for variants. */
			estack_ax(stack, top)->u.ptr.field = NULL;
			estack_ax(stack, top)->type = REG_PTR;
			next_pc += sizeof(struct load_op);
			PO;
		}

		OP(BYTECODE_OP_GET_SYMBOL):
		{
			dbg_printk("op get symbol\n");
			switch (estack_ax(stack, top)->u.ptr.type) {
			case LOAD_OBJECT:
				printk(KERN_WARNING "LTTng: bytecode: Nested fields not implemented yet.\n");
				ret = -EINVAL;
				goto end;
			case LOAD_ROOT_CONTEXT:
			case LOAD_ROOT_APP_CONTEXT:
			case LOAD_ROOT_PAYLOAD:
				/*
				 * symbol lookup is performed by
				 * specialization.
				 */
				ret = -EINVAL;
				goto end;
			}
			next_pc += sizeof(struct load_op) + sizeof(struct get_symbol);
			PO;
		}

		OP(BYTECODE_OP_GET_SYMBOL_FIELD):
		{
			/*
			 * Used for first variant encountered in a
			 * traversal. Variants are not implemented yet.
			 */
			ret = -EINVAL;
			goto end;
		}

		OP(BYTECODE_OP_GET_INDEX_U16):
		{
			struct load_op *insn = (struct load_op *) pc;
			struct get_index_u16 *index = (struct get_index_u16 *) insn->data;

			dbg_printk("op get index u16\n");
			ret = dynamic_get_index(lttng_probe_ctx, bytecode, index->index, estack_ax(stack, top));
			if (ret)
				goto end;
			estack_ax_v = estack_ax(stack, top)->u.v;
			estack_ax_t = estack_ax(stack, top)->type;
			next_pc += sizeof(struct load_op) + sizeof(struct get_index_u16);
			PO;
		}

		OP(BYTECODE_OP_GET_INDEX_U64):
		{
			struct load_op *insn = (struct load_op *) pc;
			struct get_index_u64 *index = (struct get_index_u64 *) insn->data;

			dbg_printk("op get index u64\n");
			ret = dynamic_get_index(lttng_probe_ctx, bytecode, index->index, estack_ax(stack, top));
			if (ret)
				goto end;
			estack_ax_v = estack_ax(stack, top)->u.v;
			estack_ax_t = estack_ax(stack, top)->type;
			next_pc += sizeof(struct load_op) + sizeof(struct get_index_u64);
			PO;
		}

		OP(BYTECODE_OP_LOAD_FIELD):
		{
			dbg_printk("op load field\n");
			ret = dynamic_load_field(estack_ax(stack, top));
			if (ret)
				goto end;
			estack_ax_v = estack_ax(stack, top)->u.v;
			estack_ax_t = estack_ax(stack, top)->type;
			next_pc += sizeof(struct load_op);
			PO;
		}

		OP(BYTECODE_OP_LOAD_FIELD_S8):
		{
			dbg_printk("op load field s8\n");

			estack_ax_v = *(int8_t *) estack_ax(stack, top)->u.ptr.ptr;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct load_op);
			PO;
		}
		OP(BYTECODE_OP_LOAD_FIELD_S16):
		{
			dbg_printk("op load field s16\n");

			estack_ax_v = *(int16_t *) estack_ax(stack, top)->u.ptr.ptr;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct load_op);
			PO;
		}
		OP(BYTECODE_OP_LOAD_FIELD_S32):
		{
			dbg_printk("op load field s32\n");

			estack_ax_v = *(int32_t *) estack_ax(stack, top)->u.ptr.ptr;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct load_op);
			PO;
		}
		OP(BYTECODE_OP_LOAD_FIELD_S64):
		{
			dbg_printk("op load field s64\n");

			estack_ax_v = *(int64_t *) estack_ax(stack, top)->u.ptr.ptr;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct load_op);
			PO;
		}
		OP(BYTECODE_OP_LOAD_FIELD_U8):
		{
			dbg_printk("op load field u8\n");

			estack_ax_v = *(uint8_t *) estack_ax(stack, top)->u.ptr.ptr;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct load_op);
			PO;
		}
		OP(BYTECODE_OP_LOAD_FIELD_U16):
		{
			dbg_printk("op load field u16\n");

			estack_ax_v = *(uint16_t *) estack_ax(stack, top)->u.ptr.ptr;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct load_op);
			PO;
		}
		OP(BYTECODE_OP_LOAD_FIELD_U32):
		{
			dbg_printk("op load field u32\n");

			estack_ax_v = *(uint32_t *) estack_ax(stack, top)->u.ptr.ptr;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct load_op);
			PO;
		}
		OP(BYTECODE_OP_LOAD_FIELD_U64):
		{
			dbg_printk("op load field u64\n");

			estack_ax_v = *(uint64_t *) estack_ax(stack, top)->u.ptr.ptr;
			estack_ax_t = REG_S64;
			next_pc += sizeof(struct load_op);
			PO;
		}
		OP(BYTECODE_OP_LOAD_FIELD_DOUBLE):
		{
			ret = -EINVAL;
			goto end;
		}

		OP(BYTECODE_OP_LOAD_FIELD_STRING):
		{
			const char *str;

			dbg_printk("op load field string\n");
			str = (const char *) estack_ax(stack, top)->u.ptr.ptr;
			estack_ax(stack, top)->u.s.str = str;
			if (unlikely(!estack_ax(stack, top)->u.s.str)) {
				dbg_printk("Bytecode warning: loading a NULL string.\n");
				ret = -EINVAL;
				goto end;
			}
			estack_ax(stack, top)->u.s.seq_len = LTTNG_SIZE_MAX;
			estack_ax(stack, top)->u.s.literal_type =
				ESTACK_STRING_LITERAL_TYPE_NONE;
			estack_ax(stack, top)->type = REG_STRING;
			next_pc += sizeof(struct load_op);
			PO;
		}

		OP(BYTECODE_OP_LOAD_FIELD_SEQUENCE):
		{
			const char *ptr;

			dbg_printk("op load field string sequence\n");
			ptr = estack_ax(stack, top)->u.ptr.ptr;
			estack_ax(stack, top)->u.s.seq_len = *(unsigned long *) ptr;
			estack_ax(stack, top)->u.s.str = *(const char **) (ptr + sizeof(unsigned long));
			if (unlikely(!estack_ax(stack, top)->u.s.str)) {
				dbg_printk("Bytecode warning: loading a NULL sequence.\n");
				ret = -EINVAL;
				goto end;
			}
			estack_ax(stack, top)->u.s.literal_type =
				ESTACK_STRING_LITERAL_TYPE_NONE;
			estack_ax(stack, top)->type = REG_STRING;
			next_pc += sizeof(struct load_op);
			PO;
		}

	END_OP
end:
	/* Return _DISCARD on error. */
	if (ret)
		return LTTNG_INTERPRETER_DISCARD;

	if (output) {
		return lttng_bytecode_interpret_format_output(
				estack_ax(stack, top), output);
	}

	return retval;
}
LTTNG_STACK_FRAME_NON_STANDARD(bytecode_interpret);

uint64_t lttng_bytecode_filter_interpret(void *filter_data,
		struct lttng_probe_ctx *lttng_probe_ctx,
		const char *filter_stack_data)
{
	return bytecode_interpret(filter_data, lttng_probe_ctx,
			filter_stack_data, NULL);
}

uint64_t lttng_bytecode_capture_interpret(void *capture_data,
		struct lttng_probe_ctx *lttng_probe_ctx,
		const char *capture_stack_data,
		struct lttng_interpreter_output *output)
{
	return bytecode_interpret(capture_data, lttng_probe_ctx,
			capture_stack_data, output);
}

#undef START_OP
#undef OP
#undef PO
#undef END_OP
