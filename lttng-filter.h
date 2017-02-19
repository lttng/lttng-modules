#ifndef _LTTNG_FILTER_H
#define _LTTNG_FILTER_H

/*
 * lttng-filter.h
 *
 * LTTng modules filter header.
 *
 * Copyright (C) 2010-2016 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
	REG_STAR_GLOB_STRING,
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
enum estack_string_literal_type {
	ESTACK_STRING_LITERAL_TYPE_NONE,
	ESTACK_STRING_LITERAL_TYPE_PLAIN,
	ESTACK_STRING_LITERAL_TYPE_STAR_GLOB,
};

struct estack_entry {
	union {
		int64_t v;

		struct {
			const char *str;
			const char __user *user_str;
			size_t seq_len;
			enum estack_string_literal_type literal_type;
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
