/*
 * lttng-filter-validator.c
 *
 * LTTng modules filter bytecode validator.
 *
 * Copyright (C) 2010-2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
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

#include <linux/types.h>
#include <linux/jhash.h>
#include <linux/slab.h>

#include <wrapper/list.h>
#include <lttng-filter.h>

#define MERGE_POINT_TABLE_BITS		7
#define MERGE_POINT_TABLE_SIZE		(1U << MERGE_POINT_TABLE_BITS)

/* merge point table node */
struct mp_node {
	struct hlist_node node;

	/* Context at merge point */
	struct vstack stack;
	unsigned long target_pc;
};

struct mp_table {
	struct hlist_head mp_head[MERGE_POINT_TABLE_SIZE];
};

static
int lttng_hash_match(struct mp_node *mp_node, unsigned long key_pc)
{
	if (mp_node->target_pc == key_pc)
		return 1;
	else
		return 0;
}

static
int merge_points_compare(const struct vstack *stacka,
			const struct vstack *stackb)
{
	int i, len;

	if (stacka->top != stackb->top)
		return 1;
	len = stacka->top + 1;
	WARN_ON_ONCE(len < 0);
	for (i = 0; i < len; i++) {
		if (stacka->e[i].type != stackb->e[i].type)
			return 1;
	}
	return 0;
}

static
int merge_point_add_check(struct mp_table *mp_table, unsigned long target_pc,
		const struct vstack *stack)
{
	struct mp_node *mp_node;
	unsigned long hash = jhash_1word(target_pc, 0);
	struct hlist_head *head;
	struct mp_node *lookup_node;
	int found = 0;

	dbg_printk("Filter: adding merge point at offset %lu, hash %lu\n",
			target_pc, hash);
	mp_node = kzalloc(sizeof(struct mp_node), GFP_KERNEL);
	if (!mp_node)
		return -ENOMEM;
	mp_node->target_pc = target_pc;
	memcpy(&mp_node->stack, stack, sizeof(mp_node->stack));

	head = &mp_table->mp_head[hash & (MERGE_POINT_TABLE_SIZE - 1)];
	lttng_hlist_for_each_entry(lookup_node, head, node) {
		if (lttng_hash_match(lookup_node, target_pc)) {
			found = 1;
			break;
		}
	}
	if (found) {
		/* Key already present */
		dbg_printk("Filter: compare merge points for offset %lu, hash %lu\n",
				target_pc, hash);
		kfree(mp_node);
		if (merge_points_compare(stack, &lookup_node->stack)) {
			printk(KERN_WARNING "Merge points differ for offset %lu\n",
				target_pc);
			return -EINVAL;
		}
	} else {
		hlist_add_head(&mp_node->node, head);
	}
	return 0;
}

/*
 * Binary comparators use top of stack and top of stack -1.
 */
static
int bin_op_compare_check(struct vstack *stack, const char *str)
{
	if (unlikely(!vstack_ax(stack) || !vstack_bx(stack)))
		goto error_unknown;

	switch (vstack_ax(stack)->type) {
	default:
	case REG_DOUBLE:
		goto error_unknown;

	case REG_STRING:
		switch (vstack_bx(stack)->type) {
		default:
		case REG_DOUBLE:
			goto error_unknown;

		case REG_STRING:
			break;
		case REG_S64:
			goto error_mismatch;
		}
		break;
	case REG_S64:
		switch (vstack_bx(stack)->type) {
		default:
		case REG_DOUBLE:
			goto error_unknown;

		case REG_STRING:
			goto error_mismatch;

		case REG_S64:
			break;
		}
		break;
	}
	return 0;

error_unknown:
	return -EINVAL;

error_mismatch:
	printk(KERN_WARNING "type mismatch for '%s' binary operator\n", str);
	return -EINVAL;
}

/*
 * Validate bytecode range overflow within the validation pass.
 * Called for each instruction encountered.
 */
static
int bytecode_validate_overflow(struct bytecode_runtime *bytecode,
		void *start_pc, void *pc)
{
	int ret = 0;

	switch (*(filter_opcode_t *) pc) {
	case FILTER_OP_UNKNOWN:
	default:
	{
		printk(KERN_WARNING "unknown bytecode op %u\n",
			(unsigned int) *(filter_opcode_t *) pc);
		ret = -EINVAL;
		break;
	}

	case FILTER_OP_RETURN:
	{
		if (unlikely(pc + sizeof(struct return_op)
				> start_pc + bytecode->len)) {
			ret = -ERANGE;
		}
		break;
	}

	/* binary */
	case FILTER_OP_MUL:
	case FILTER_OP_DIV:
	case FILTER_OP_MOD:
	case FILTER_OP_PLUS:
	case FILTER_OP_MINUS:
	case FILTER_OP_RSHIFT:
	case FILTER_OP_LSHIFT:
	case FILTER_OP_BIN_AND:
	case FILTER_OP_BIN_OR:
	case FILTER_OP_BIN_XOR:
	case FILTER_OP_EQ_DOUBLE:
	case FILTER_OP_NE_DOUBLE:
	case FILTER_OP_GT_DOUBLE:
	case FILTER_OP_LT_DOUBLE:
	case FILTER_OP_GE_DOUBLE:
	case FILTER_OP_LE_DOUBLE:
	/* Floating point */
	case FILTER_OP_EQ_DOUBLE_S64:
	case FILTER_OP_NE_DOUBLE_S64:
	case FILTER_OP_GT_DOUBLE_S64:
	case FILTER_OP_LT_DOUBLE_S64:
	case FILTER_OP_GE_DOUBLE_S64:
	case FILTER_OP_LE_DOUBLE_S64:
	case FILTER_OP_EQ_S64_DOUBLE:
	case FILTER_OP_NE_S64_DOUBLE:
	case FILTER_OP_GT_S64_DOUBLE:
	case FILTER_OP_LT_S64_DOUBLE:
	case FILTER_OP_GE_S64_DOUBLE:
	case FILTER_OP_LE_S64_DOUBLE:
	case FILTER_OP_LOAD_FIELD_REF_DOUBLE:
	case FILTER_OP_GET_CONTEXT_REF_DOUBLE:
	case FILTER_OP_LOAD_DOUBLE:
	case FILTER_OP_CAST_DOUBLE_TO_S64:
	case FILTER_OP_UNARY_PLUS_DOUBLE:
	case FILTER_OP_UNARY_MINUS_DOUBLE:
	case FILTER_OP_UNARY_NOT_DOUBLE:
	{
		printk(KERN_WARNING "unsupported bytecode op %u\n",
			(unsigned int) *(filter_opcode_t *) pc);
		ret = -EINVAL;
		break;
	}

	case FILTER_OP_EQ:
	case FILTER_OP_NE:
	case FILTER_OP_GT:
	case FILTER_OP_LT:
	case FILTER_OP_GE:
	case FILTER_OP_LE:
	case FILTER_OP_EQ_STRING:
	case FILTER_OP_NE_STRING:
	case FILTER_OP_GT_STRING:
	case FILTER_OP_LT_STRING:
	case FILTER_OP_GE_STRING:
	case FILTER_OP_LE_STRING:
	case FILTER_OP_EQ_S64:
	case FILTER_OP_NE_S64:
	case FILTER_OP_GT_S64:
	case FILTER_OP_LT_S64:
	case FILTER_OP_GE_S64:
	case FILTER_OP_LE_S64:
	{
		if (unlikely(pc + sizeof(struct binary_op)
				> start_pc + bytecode->len)) {
			ret = -ERANGE;
		}
		break;
	}

	/* unary */
	case FILTER_OP_UNARY_PLUS:
	case FILTER_OP_UNARY_MINUS:
	case FILTER_OP_UNARY_NOT:
	case FILTER_OP_UNARY_PLUS_S64:
	case FILTER_OP_UNARY_MINUS_S64:
	case FILTER_OP_UNARY_NOT_S64:
	{
		if (unlikely(pc + sizeof(struct unary_op)
				> start_pc + bytecode->len)) {
			ret = -ERANGE;
		}
		break;
	}

	/* logical */
	case FILTER_OP_AND:
	case FILTER_OP_OR:
	{
		if (unlikely(pc + sizeof(struct logical_op)
				> start_pc + bytecode->len)) {
			ret = -ERANGE;
		}
		break;
	}

	/* load field ref */
	case FILTER_OP_LOAD_FIELD_REF:
	{
		printk(KERN_WARNING "Unknown field ref type\n");
		ret = -EINVAL;
		break;
	}
	/* get context ref */
	case FILTER_OP_GET_CONTEXT_REF:
	{
		printk(KERN_WARNING "Unknown field ref type\n");
		ret = -EINVAL;
		break;
	}
	case FILTER_OP_LOAD_FIELD_REF_STRING:
	case FILTER_OP_LOAD_FIELD_REF_SEQUENCE:
	case FILTER_OP_LOAD_FIELD_REF_USER_STRING:
	case FILTER_OP_LOAD_FIELD_REF_USER_SEQUENCE:
	case FILTER_OP_LOAD_FIELD_REF_S64:
	case FILTER_OP_GET_CONTEXT_REF_STRING:
	case FILTER_OP_GET_CONTEXT_REF_S64:
	{
		if (unlikely(pc + sizeof(struct load_op) + sizeof(struct field_ref)
				> start_pc + bytecode->len)) {
			ret = -ERANGE;
		}
		break;
	}

	/* load from immediate operand */
	case FILTER_OP_LOAD_STRING:
	{
		struct load_op *insn = (struct load_op *) pc;
		uint32_t str_len, maxlen;

		if (unlikely(pc + sizeof(struct load_op)
				> start_pc + bytecode->len)) {
			ret = -ERANGE;
			break;
		}

		maxlen = start_pc + bytecode->len - pc - sizeof(struct load_op);
		str_len = strnlen(insn->data, maxlen);
		if (unlikely(str_len >= maxlen)) {
			/* Final '\0' not found within range */
			ret = -ERANGE;
		}
		break;
	}

	case FILTER_OP_LOAD_S64:
	{
		if (unlikely(pc + sizeof(struct load_op) + sizeof(struct literal_numeric)
				> start_pc + bytecode->len)) {
			ret = -ERANGE;
		}
		break;
	}

	case FILTER_OP_CAST_TO_S64:
	case FILTER_OP_CAST_NOP:
	{
		if (unlikely(pc + sizeof(struct cast_op)
				> start_pc + bytecode->len)) {
			ret = -ERANGE;
		}
		break;
	}

	}

	return ret;
}

static
unsigned long delete_all_nodes(struct mp_table *mp_table)
{
	struct mp_node *mp_node;
	struct hlist_node *tmp;
	unsigned long nr_nodes = 0;
	int i;

	for (i = 0; i < MERGE_POINT_TABLE_SIZE; i++) {
		struct hlist_head *head;

		head = &mp_table->mp_head[i];
		lttng_hlist_for_each_entry_safe(mp_node, tmp, head, node) {
			kfree(mp_node);
			nr_nodes++;
		}
	}
	return nr_nodes;
}

/*
 * Return value:
 * 0: success
 * <0: error
 */
static
int validate_instruction_context(struct bytecode_runtime *bytecode,
		struct vstack *stack,
		void *start_pc,
		void *pc)
{
	int ret = 0;

	switch (*(filter_opcode_t *) pc) {
	case FILTER_OP_UNKNOWN:
	default:
	{
		printk(KERN_WARNING "unknown bytecode op %u\n",
			(unsigned int) *(filter_opcode_t *) pc);
		ret = -EINVAL;
		goto end;
	}

	case FILTER_OP_RETURN:
	{
		goto end;
	}

	/* binary */
	case FILTER_OP_MUL:
	case FILTER_OP_DIV:
	case FILTER_OP_MOD:
	case FILTER_OP_PLUS:
	case FILTER_OP_MINUS:
	case FILTER_OP_RSHIFT:
	case FILTER_OP_LSHIFT:
	case FILTER_OP_BIN_AND:
	case FILTER_OP_BIN_OR:
	case FILTER_OP_BIN_XOR:
	/* Floating point */
	case FILTER_OP_EQ_DOUBLE:
	case FILTER_OP_NE_DOUBLE:
	case FILTER_OP_GT_DOUBLE:
	case FILTER_OP_LT_DOUBLE:
	case FILTER_OP_GE_DOUBLE:
	case FILTER_OP_LE_DOUBLE:
	case FILTER_OP_EQ_DOUBLE_S64:
	case FILTER_OP_NE_DOUBLE_S64:
	case FILTER_OP_GT_DOUBLE_S64:
	case FILTER_OP_LT_DOUBLE_S64:
	case FILTER_OP_GE_DOUBLE_S64:
	case FILTER_OP_LE_DOUBLE_S64:
	case FILTER_OP_EQ_S64_DOUBLE:
	case FILTER_OP_NE_S64_DOUBLE:
	case FILTER_OP_GT_S64_DOUBLE:
	case FILTER_OP_LT_S64_DOUBLE:
	case FILTER_OP_GE_S64_DOUBLE:
	case FILTER_OP_LE_S64_DOUBLE:
	case FILTER_OP_UNARY_PLUS_DOUBLE:
	case FILTER_OP_UNARY_MINUS_DOUBLE:
	case FILTER_OP_UNARY_NOT_DOUBLE:
	case FILTER_OP_LOAD_FIELD_REF_DOUBLE:
	case FILTER_OP_LOAD_DOUBLE:
	case FILTER_OP_CAST_DOUBLE_TO_S64:
	case FILTER_OP_GET_CONTEXT_REF_DOUBLE:
	{
		printk(KERN_WARNING "unsupported bytecode op %u\n",
			(unsigned int) *(filter_opcode_t *) pc);
		ret = -EINVAL;
		goto end;
	}

	case FILTER_OP_EQ:
	{
		ret = bin_op_compare_check(stack, "==");
		if (ret)
			goto end;
		break;
	}
	case FILTER_OP_NE:
	{
		ret = bin_op_compare_check(stack, "!=");
		if (ret)
			goto end;
		break;
	}
	case FILTER_OP_GT:
	{
		ret = bin_op_compare_check(stack, ">");
		if (ret)
			goto end;
		break;
	}
	case FILTER_OP_LT:
	{
		ret = bin_op_compare_check(stack, "<");
		if (ret)
			goto end;
		break;
	}
	case FILTER_OP_GE:
	{
		ret = bin_op_compare_check(stack, ">=");
		if (ret)
			goto end;
		break;
	}
	case FILTER_OP_LE:
	{
		ret = bin_op_compare_check(stack, "<=");
		if (ret)
			goto end;
		break;
	}

	case FILTER_OP_EQ_STRING:
	case FILTER_OP_NE_STRING:
	case FILTER_OP_GT_STRING:
	case FILTER_OP_LT_STRING:
	case FILTER_OP_GE_STRING:
	case FILTER_OP_LE_STRING:
	{
		if (!vstack_ax(stack) || !vstack_bx(stack)) {
			printk(KERN_WARNING "Empty stack\n");
			ret = -EINVAL;
			goto end;
		}
		if (vstack_ax(stack)->type != REG_STRING
				|| vstack_bx(stack)->type != REG_STRING) {
			printk(KERN_WARNING "Unexpected register type for string comparator\n");
			ret = -EINVAL;
			goto end;
		}
		break;
	}

	case FILTER_OP_EQ_S64:
	case FILTER_OP_NE_S64:
	case FILTER_OP_GT_S64:
	case FILTER_OP_LT_S64:
	case FILTER_OP_GE_S64:
	case FILTER_OP_LE_S64:
	{
		if (!vstack_ax(stack) || !vstack_bx(stack)) {
			printk(KERN_WARNING "Empty stack\n");
			ret = -EINVAL;
			goto end;
		}
		if (vstack_ax(stack)->type != REG_S64
				|| vstack_bx(stack)->type != REG_S64) {
			printk(KERN_WARNING "Unexpected register type for s64 comparator\n");
			ret = -EINVAL;
			goto end;
		}
		break;
	}

	/* unary */
	case FILTER_OP_UNARY_PLUS:
	case FILTER_OP_UNARY_MINUS:
	case FILTER_OP_UNARY_NOT:
	{
		if (!vstack_ax(stack)) {
			printk(KERN_WARNING "Empty stack\n");
			ret = -EINVAL;
			goto end;
		}
		switch (vstack_ax(stack)->type) {
		default:
		case REG_DOUBLE:
			printk(KERN_WARNING "unknown register type\n");
			ret = -EINVAL;
			goto end;

		case REG_STRING:
			printk(KERN_WARNING "Unary op can only be applied to numeric or floating point registers\n");
			ret = -EINVAL;
			goto end;
		case REG_S64:
			break;
		}
		break;
	}

	case FILTER_OP_UNARY_PLUS_S64:
	case FILTER_OP_UNARY_MINUS_S64:
	case FILTER_OP_UNARY_NOT_S64:
	{
		if (!vstack_ax(stack)) {
			printk(KERN_WARNING "Empty stack\n");
			ret = -EINVAL;
			goto end;
		}
		if (vstack_ax(stack)->type != REG_S64) {
			printk(KERN_WARNING "Invalid register type\n");
			ret = -EINVAL;
			goto end;
		}
		break;
	}

	/* logical */
	case FILTER_OP_AND:
	case FILTER_OP_OR:
	{
		struct logical_op *insn = (struct logical_op *) pc;

		if (!vstack_ax(stack)) {
			printk(KERN_WARNING "Empty stack\n");
			ret = -EINVAL;
			goto end;
		}
		if (vstack_ax(stack)->type != REG_S64) {
			printk(KERN_WARNING "Logical comparator expects S64 register\n");
			ret = -EINVAL;
			goto end;
		}

		dbg_printk("Validate jumping to bytecode offset %u\n",
			(unsigned int) insn->skip_offset);
		if (unlikely(start_pc + insn->skip_offset <= pc)) {
			printk(KERN_WARNING "Loops are not allowed in bytecode\n");
			ret = -EINVAL;
			goto end;
		}
		break;
	}

	/* load field ref */
	case FILTER_OP_LOAD_FIELD_REF:
	{
		printk(KERN_WARNING "Unknown field ref type\n");
		ret = -EINVAL;
		goto end;
	}
	case FILTER_OP_LOAD_FIELD_REF_STRING:
	case FILTER_OP_LOAD_FIELD_REF_SEQUENCE:
	case FILTER_OP_LOAD_FIELD_REF_USER_STRING:
	case FILTER_OP_LOAD_FIELD_REF_USER_SEQUENCE:
	{
		struct load_op *insn = (struct load_op *) pc;
		struct field_ref *ref = (struct field_ref *) insn->data;

		dbg_printk("Validate load field ref offset %u type string\n",
			ref->offset);
		break;
	}
	case FILTER_OP_LOAD_FIELD_REF_S64:
	{
		struct load_op *insn = (struct load_op *) pc;
		struct field_ref *ref = (struct field_ref *) insn->data;

		dbg_printk("Validate load field ref offset %u type s64\n",
			ref->offset);
		break;
	}

	/* load from immediate operand */
	case FILTER_OP_LOAD_STRING:
	{
		break;
	}

	case FILTER_OP_LOAD_S64:
	{
		break;
	}

	case FILTER_OP_CAST_TO_S64:
	{
		struct cast_op *insn = (struct cast_op *) pc;

		if (!vstack_ax(stack)) {
			printk(KERN_WARNING "Empty stack\n");
			ret = -EINVAL;
			goto end;
		}
		switch (vstack_ax(stack)->type) {
		default:
		case REG_DOUBLE:
			printk(KERN_WARNING "unknown register type\n");
			ret = -EINVAL;
			goto end;

		case REG_STRING:
			printk(KERN_WARNING "Cast op can only be applied to numeric or floating point registers\n");
			ret = -EINVAL;
			goto end;
		case REG_S64:
			break;
		}
		if (insn->op == FILTER_OP_CAST_DOUBLE_TO_S64) {
			if (vstack_ax(stack)->type != REG_DOUBLE) {
				printk(KERN_WARNING "Cast expects double\n");
				ret = -EINVAL;
				goto end;
			}
		}
		break;
	}
	case FILTER_OP_CAST_NOP:
	{
		break;
	}

	/* get context ref */
	case FILTER_OP_GET_CONTEXT_REF:
	{
		printk(KERN_WARNING "Unknown get context ref type\n");
		ret = -EINVAL;
		goto end;
	}
	case FILTER_OP_GET_CONTEXT_REF_STRING:
	{
		struct load_op *insn = (struct load_op *) pc;
		struct field_ref *ref = (struct field_ref *) insn->data;

		dbg_printk("Validate get context ref offset %u type string\n",
			ref->offset);
		break;
	}
	case FILTER_OP_GET_CONTEXT_REF_S64:
	{
		struct load_op *insn = (struct load_op *) pc;
		struct field_ref *ref = (struct field_ref *) insn->data;

		dbg_printk("Validate get context ref offset %u type s64\n",
			ref->offset);
		break;
	}

	}
end:
	return ret;
}

/*
 * Return value:
 * 0: success
 * <0: error
 */
static
int validate_instruction_all_contexts(struct bytecode_runtime *bytecode,
		struct mp_table *mp_table,
		struct vstack *stack,
		void *start_pc,
		void *pc)
{
	int ret, found = 0;
	unsigned long target_pc = pc - start_pc;
	unsigned long hash;
	struct hlist_head *head;
	struct mp_node *mp_node;

	/* Validate the context resulting from the previous instruction */
	ret = validate_instruction_context(bytecode, stack, start_pc, pc);
	if (ret)
		return ret;

	/* Validate merge points */
	hash = jhash_1word(target_pc, 0);
	head = &mp_table->mp_head[hash & (MERGE_POINT_TABLE_SIZE - 1)];
	lttng_hlist_for_each_entry(mp_node, head, node) {
		if (lttng_hash_match(mp_node, target_pc)) {
			found = 1;
			break;
		}
	}
	if (found) {
		dbg_printk("Filter: validate merge point at offset %lu\n",
				target_pc);
		if (merge_points_compare(stack, &mp_node->stack)) {
			printk(KERN_WARNING "Merge points differ for offset %lu\n",
				target_pc);
			return -EINVAL;
		}
		/* Once validated, we can remove the merge point */
		dbg_printk("Filter: remove merge point at offset %lu\n",
				target_pc);
		hlist_del(&mp_node->node);
	}
	return 0;
}

/*
 * Return value:
 * >0: going to next insn.
 * 0: success, stop iteration.
 * <0: error
 */
static
int exec_insn(struct bytecode_runtime *bytecode,
		struct mp_table *mp_table,
		struct vstack *stack,
		void **_next_pc,
		void *pc)
{
	int ret = 1;
	void *next_pc = *_next_pc;

	switch (*(filter_opcode_t *) pc) {
	case FILTER_OP_UNKNOWN:
	default:
	{
		printk(KERN_WARNING "unknown bytecode op %u\n",
			(unsigned int) *(filter_opcode_t *) pc);
		ret = -EINVAL;
		goto end;
	}

	case FILTER_OP_RETURN:
	{
		if (!vstack_ax(stack)) {
			printk(KERN_WARNING "Empty stack\n");
			ret = -EINVAL;
			goto end;
		}
		ret = 0;
		goto end;
	}

	/* binary */
	case FILTER_OP_MUL:
	case FILTER_OP_DIV:
	case FILTER_OP_MOD:
	case FILTER_OP_PLUS:
	case FILTER_OP_MINUS:
	case FILTER_OP_RSHIFT:
	case FILTER_OP_LSHIFT:
	case FILTER_OP_BIN_AND:
	case FILTER_OP_BIN_OR:
	case FILTER_OP_BIN_XOR:
	/* Floating point */
	case FILTER_OP_EQ_DOUBLE:
	case FILTER_OP_NE_DOUBLE:
	case FILTER_OP_GT_DOUBLE:
	case FILTER_OP_LT_DOUBLE:
	case FILTER_OP_GE_DOUBLE:
	case FILTER_OP_LE_DOUBLE:
	case FILTER_OP_EQ_DOUBLE_S64:
	case FILTER_OP_NE_DOUBLE_S64:
	case FILTER_OP_GT_DOUBLE_S64:
	case FILTER_OP_LT_DOUBLE_S64:
	case FILTER_OP_GE_DOUBLE_S64:
	case FILTER_OP_LE_DOUBLE_S64:
	case FILTER_OP_EQ_S64_DOUBLE:
	case FILTER_OP_NE_S64_DOUBLE:
	case FILTER_OP_GT_S64_DOUBLE:
	case FILTER_OP_LT_S64_DOUBLE:
	case FILTER_OP_GE_S64_DOUBLE:
	case FILTER_OP_LE_S64_DOUBLE:
	case FILTER_OP_UNARY_PLUS_DOUBLE:
	case FILTER_OP_UNARY_MINUS_DOUBLE:
	case FILTER_OP_UNARY_NOT_DOUBLE:
	case FILTER_OP_LOAD_FIELD_REF_DOUBLE:
	case FILTER_OP_GET_CONTEXT_REF_DOUBLE:
	case FILTER_OP_LOAD_DOUBLE:
	case FILTER_OP_CAST_DOUBLE_TO_S64:
	{
		printk(KERN_WARNING "unsupported bytecode op %u\n",
			(unsigned int) *(filter_opcode_t *) pc);
		ret = -EINVAL;
		goto end;
	}

	case FILTER_OP_EQ:
	case FILTER_OP_NE:
	case FILTER_OP_GT:
	case FILTER_OP_LT:
	case FILTER_OP_GE:
	case FILTER_OP_LE:
	case FILTER_OP_EQ_STRING:
	case FILTER_OP_NE_STRING:
	case FILTER_OP_GT_STRING:
	case FILTER_OP_LT_STRING:
	case FILTER_OP_GE_STRING:
	case FILTER_OP_LE_STRING:
	case FILTER_OP_EQ_S64:
	case FILTER_OP_NE_S64:
	case FILTER_OP_GT_S64:
	case FILTER_OP_LT_S64:
	case FILTER_OP_GE_S64:
	case FILTER_OP_LE_S64:
	{
		/* Pop 2, push 1 */
		if (vstack_pop(stack)) {
			ret = -EINVAL;
			goto end;
		}
		if (!vstack_ax(stack)) {
			printk(KERN_WARNING "Empty stack\n");
			ret = -EINVAL;
			goto end;
		}
		vstack_ax(stack)->type = REG_S64;
		next_pc += sizeof(struct binary_op);
		break;
	}

	/* unary */
	case FILTER_OP_UNARY_PLUS:
	case FILTER_OP_UNARY_MINUS:
	case FILTER_OP_UNARY_NOT:
	case FILTER_OP_UNARY_PLUS_S64:
	case FILTER_OP_UNARY_MINUS_S64:
	case FILTER_OP_UNARY_NOT_S64:
	{
		/* Pop 1, push 1 */
		if (!vstack_ax(stack)) {
			printk(KERN_WARNING "Empty stack\n");
			ret = -EINVAL;
			goto end;
		}
		vstack_ax(stack)->type = REG_S64;
		next_pc += sizeof(struct unary_op);
		break;
	}

	/* logical */
	case FILTER_OP_AND:
	case FILTER_OP_OR:
	{
		struct logical_op *insn = (struct logical_op *) pc;
		int merge_ret;

		/* Add merge point to table */
		merge_ret = merge_point_add_check(mp_table,
					insn->skip_offset, stack);
		if (merge_ret) {
			ret = merge_ret;
			goto end;
		}
		/* Continue to next instruction */
		/* Pop 1 when jump not taken */
		if (vstack_pop(stack)) {
			ret = -EINVAL;
			goto end;
		}
		next_pc += sizeof(struct logical_op);
		break;
	}

	/* load field ref */
	case FILTER_OP_LOAD_FIELD_REF:
	{
		printk(KERN_WARNING "Unknown field ref type\n");
		ret = -EINVAL;
		goto end;
	}
	/* get context ref */
	case FILTER_OP_GET_CONTEXT_REF:
	{
		printk(KERN_WARNING "Unknown get context ref type\n");
		ret = -EINVAL;
		goto end;
	}
	case FILTER_OP_LOAD_FIELD_REF_STRING:
	case FILTER_OP_LOAD_FIELD_REF_SEQUENCE:
	case FILTER_OP_GET_CONTEXT_REF_STRING:
	case FILTER_OP_LOAD_FIELD_REF_USER_STRING:
	case FILTER_OP_LOAD_FIELD_REF_USER_SEQUENCE:
	{
		if (vstack_push(stack)) {
			ret = -EINVAL;
			goto end;
		}
		vstack_ax(stack)->type = REG_STRING;
		next_pc += sizeof(struct load_op) + sizeof(struct field_ref);
		break;
	}
	case FILTER_OP_LOAD_FIELD_REF_S64:
	case FILTER_OP_GET_CONTEXT_REF_S64:
	{
		if (vstack_push(stack)) {
			ret = -EINVAL;
			goto end;
		}
		vstack_ax(stack)->type = REG_S64;
		next_pc += sizeof(struct load_op) + sizeof(struct field_ref);
		break;
	}

	/* load from immediate operand */
	case FILTER_OP_LOAD_STRING:
	{
		struct load_op *insn = (struct load_op *) pc;

		if (vstack_push(stack)) {
			ret = -EINVAL;
			goto end;
		}
		vstack_ax(stack)->type = REG_STRING;
		next_pc += sizeof(struct load_op) + strlen(insn->data) + 1;
		break;
	}

	case FILTER_OP_LOAD_S64:
	{
		if (vstack_push(stack)) {
			ret = -EINVAL;
			goto end;
		}
		vstack_ax(stack)->type = REG_S64;
		next_pc += sizeof(struct load_op)
				+ sizeof(struct literal_numeric);
		break;
	}

	case FILTER_OP_CAST_TO_S64:
	{
		/* Pop 1, push 1 */
		if (!vstack_ax(stack)) {
			printk(KERN_WARNING "Empty stack\n");
			ret = -EINVAL;
			goto end;
		}
		vstack_ax(stack)->type = REG_S64;
		next_pc += sizeof(struct cast_op);
		break;
	}
	case FILTER_OP_CAST_NOP:
	{
		next_pc += sizeof(struct cast_op);
		break;
	}

	}
end:
	*_next_pc = next_pc;
	return ret;
}

/*
 * Never called concurrently (hash seed is shared).
 */
int lttng_filter_validate_bytecode(struct bytecode_runtime *bytecode)
{
	struct mp_table *mp_table;
	void *pc, *next_pc, *start_pc;
	int ret = -EINVAL;
	struct vstack stack;

	vstack_init(&stack);

	mp_table = kzalloc(sizeof(*mp_table), GFP_KERNEL);
	if (!mp_table) {
		printk(KERN_WARNING "Error allocating hash table for bytecode validation\n");
		return -ENOMEM;
	}
	start_pc = &bytecode->data[0];
	for (pc = next_pc = start_pc; pc - start_pc < bytecode->len;
			pc = next_pc) {
		ret = bytecode_validate_overflow(bytecode, start_pc, pc);
		if (ret != 0) {
			if (ret == -ERANGE)
				printk(KERN_WARNING "filter bytecode overflow\n");
			goto end;
		}
		dbg_printk("Validating op %s (%u)\n",
			lttng_filter_print_op((unsigned int) *(filter_opcode_t *) pc),
			(unsigned int) *(filter_opcode_t *) pc);

		/*
		 * For each instruction, validate the current context
		 * (traversal of entire execution flow), and validate
		 * all 	merge points targeting this instruction.
		 */
		ret = validate_instruction_all_contexts(bytecode, mp_table,
					&stack, start_pc, pc);
		if (ret)
			goto end;
		ret = exec_insn(bytecode, mp_table, &stack, &next_pc, pc);
		if (ret <= 0)
			goto end;
	}
end:
	if (delete_all_nodes(mp_table)) {
		if (!ret) {
			printk(KERN_WARNING "Unexpected merge points\n");
			ret = -EINVAL;
		}
	}
	kfree(mp_table);
	return ret;
}
