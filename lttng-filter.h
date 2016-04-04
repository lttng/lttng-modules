#ifndef _LTTNG_FILTER_H
#define _LTTNG_FILTER_H

/*
 * lttng-filter.h
 *
 * LTTng modules filter header.
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

#include <linux/kernel.h>

#include <lttng-events.h>
#include <filter-bytecode.h>

/* Filter stack length, in number of entries */
#define FILTER_STACK_LEN	10	/* includes 2 dummy */
#define FILTER_STACK_EMPTY	1

#ifdef DEBUG
#define dbg_printk(fmt, args...)				\
	printk(KERN_DEBUG "[debug bytecode in %s:%s@%u] " fmt,		\
		__FILE__, __func__, __LINE__, ## args)
#else
#define dbg_printk(fmt, args...)				\
do {								\
	/* do nothing but check printf format */		\
	if (0)							\
		printk(KERN_DEBUG "[debug bytecode in %s:%s@%u] " fmt,	\
			__FILE__, __func__, __LINE__, ## args);	\
} while (0)
#endif

/* Linked bytecode. Child of struct lttng_bytecode_runtime. */
struct bytecode_runtime {
	struct lttng_bytecode_runtime p;
	uint16_t len;
	char data[0];
};

enum entry_type {
	REG_S64,
	REG_DOUBLE,
	REG_STRING,
	REG_TYPE_UNKNOWN,
};

/* Validation stack */
struct vstack_entry {
	enum entry_type type;
};

struct vstack {
	int top;	/* top of stack */
	struct vstack_entry e[FILTER_STACK_LEN];
};

static inline
void vstack_init(struct vstack *stack)
{
	stack->top = -1;
}

static inline
struct vstack_entry *vstack_ax(struct vstack *stack)
{
	if (unlikely(stack->top < 0))
		return NULL;
	return &stack->e[stack->top];
}

static inline
struct vstack_entry *vstack_bx(struct vstack *stack)
{
	if (unlikely(stack->top < 1))
		return NULL;
	return &stack->e[stack->top - 1];
}

static inline
int vstack_push(struct vstack *stack)
{
	if (stack->top >= FILTER_STACK_LEN - 1) {
		printk(KERN_WARNING "Stack full\n");
		return -EINVAL;
	}
	++stack->top;
	return 0;
}

static inline
int vstack_pop(struct vstack *stack)
{
	if (unlikely(stack->top < 0)) {
		printk(KERN_WARNING "Stack empty\n");
		return -EINVAL;
	}
	stack->top--;
	return 0;
}

/* Execution stack */
struct estack_entry {
	union {
		int64_t v;

		struct {
			const char *str;
			const char __user *user_str;
			size_t seq_len;
			int literal;		/* is string literal ? */
			int user;		/* is string from userspace ? */
		} s;
	} u;
};

struct estack {
	int top;	/* top of stack */
	struct estack_entry e[FILTER_STACK_LEN];
};

#define estack_ax_v	ax
#define estack_bx_v	bx

#define estack_ax(stack, top)					\
	({							\
		BUG_ON((top) <= FILTER_STACK_EMPTY);		\
		&(stack)->e[top];				\
	})

#define estack_bx(stack, top)					\
	({							\
		BUG_ON((top) <= FILTER_STACK_EMPTY + 1);	\
		&(stack)->e[(top) - 1];				\
	})

#define estack_push(stack, top, ax, bx)				\
	do {							\
		BUG_ON((top) >= FILTER_STACK_LEN - 1);		\
		(stack)->e[(top) - 1].u.v = (bx);		\
		(bx) = (ax);					\
		++(top);					\
	} while (0)

#define estack_pop(stack, top, ax, bx)				\
	do {							\
		BUG_ON((top) <= FILTER_STACK_EMPTY);		\
		(ax) = (bx);					\
		(bx) = (stack)->e[(top) - 2].u.v;		\
		(top)--;					\
	} while (0)

const char *lttng_filter_print_op(enum filter_op op);

int lttng_filter_validate_bytecode(struct bytecode_runtime *bytecode);
int lttng_filter_specialize_bytecode(struct bytecode_runtime *bytecode);

uint64_t lttng_filter_false(void *filter_data,
		struct lttng_probe_ctx *lttng_probe_ctx,
		const char *filter_stack_data);
uint64_t lttng_filter_interpret_bytecode(void *filter_data,
		struct lttng_probe_ctx *lttng_probe_ctx,
		const char *filter_stack_data);

#endif /* _LTTNG_FILTER_H */
