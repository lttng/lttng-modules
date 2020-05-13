/* SPDX-License-Identifier: MIT
 *
 * lttng/lttng-bytecode.h
 *
 * LTTng modules bytecode header.
 *
 * Copyright (C) 2010-2016 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_BYTECODE_H
#define _LTTNG_BYTECODE_H

#include <linux/kernel.h>

#include <lttng/events.h>
#include <lttng/bytecode.h>

/* Interpreter stack length, in number of entries */
#define INTERPRETER_STACK_LEN	10	/* includes 2 dummy */
#define INTERPRETER_STACK_EMPTY	1
#define INTERPRETER_MAX_DATA_LEN	65536

#ifdef DEBUG
#define dbg_printk(fmt, args...)				\
	printk(KERN_DEBUG "LTTng: [debug bytecode in %s:%s@%u] " fmt,		\
		__FILE__, __func__, __LINE__, ## args)
#else
#define dbg_printk(fmt, args...)				\
do {								\
	/* do nothing but check printf format */		\
	if (0)							\
		printk(KERN_DEBUG "LTTng: [debug bytecode in %s:%s@%u] " fmt,	\
			__FILE__, __func__, __LINE__, ## args);	\
} while (0)
#endif

/* Linked bytecode. Child of struct lttng_bytecode_runtime. */
struct bytecode_runtime {
	struct lttng_bytecode_runtime p;
	size_t data_len;
	size_t data_alloc_len;
	char *data;
	uint16_t len;
	char code[0];
};

enum entry_type {
	REG_S64,
	REG_U64,
	REG_DOUBLE,
	REG_STRING,
	REG_STAR_GLOB_STRING,
	REG_TYPE_UNKNOWN,
	REG_PTR,
};

enum load_type {
	LOAD_ROOT_CONTEXT,
	LOAD_ROOT_APP_CONTEXT,
	LOAD_ROOT_PAYLOAD,
	LOAD_OBJECT,
};

enum object_type {
	OBJECT_TYPE_S8,
	OBJECT_TYPE_S16,
	OBJECT_TYPE_S32,
	OBJECT_TYPE_S64,
	OBJECT_TYPE_U8,
	OBJECT_TYPE_U16,
	OBJECT_TYPE_U32,
	OBJECT_TYPE_U64,

	OBJECT_TYPE_SIGNED_ENUM,
	OBJECT_TYPE_UNSIGNED_ENUM,

	OBJECT_TYPE_DOUBLE,
	OBJECT_TYPE_STRING,
	OBJECT_TYPE_STRING_SEQUENCE,

	OBJECT_TYPE_SEQUENCE,
	OBJECT_TYPE_ARRAY,
	OBJECT_TYPE_STRUCT,
	OBJECT_TYPE_VARIANT,

	OBJECT_TYPE_DYNAMIC,
};

struct bytecode_get_index_data {
	uint64_t offset;	/* in bytes */
	size_t ctx_index;
	size_t array_len;
	/*
	 * Field is only populated for LOAD_ROOT_CONTEXT, LOAD_ROOT_APP_CONTEXT
	 * and LOAD_ROOT_PAYLOAD. Left NULL for LOAD_OBJECT, considering that the
	 * interpreter needs to find it from the event fields and types to
	 * support variants.
	 */
	const struct lttng_event_field *field;
	struct {
		size_t len;
		enum object_type type;
		bool rev_bo;	/* reverse byte order */
	} elem;
};

/* Validation stack */
struct vstack_load {
	enum load_type type;
	enum object_type object_type;
	const struct lttng_event_field *field;
	bool rev_bo;	/* reverse byte order */
};

struct vstack_entry {
	enum entry_type type;
	struct vstack_load load;
};

struct vstack {
	int top;	/* top of stack */
	struct vstack_entry e[INTERPRETER_STACK_LEN];
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
	if (stack->top >= INTERPRETER_STACK_LEN - 1) {
		printk(KERN_WARNING "LTTng: filter: Stack full\n");
		return -EINVAL;
	}
	++stack->top;
	return 0;
}

static inline
int vstack_pop(struct vstack *stack)
{
	if (unlikely(stack->top < 0)) {
		printk(KERN_WARNING "LTTng: filter: Stack empty\n");
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

struct load_ptr {
	enum load_type type;
	enum object_type object_type;
	const void *ptr;
	size_t nr_elem;
	bool rev_bo;
	/* Temporary place-holders for contexts. */
	union {
		int64_t s64;
		uint64_t u64;
		double d;
	} u;
	const struct lttng_event_field *field;
};

struct estack_entry {
	enum entry_type type;
	union {
		int64_t v;

		struct {
			const char *str;
			const char __user *user_str;
			size_t seq_len;
			enum estack_string_literal_type literal_type;
			int user;		/* is string from userspace ? */
		} s;
		struct load_ptr ptr;
	} u;
};

struct estack {
	int top;	/* top of stack */
	struct estack_entry e[INTERPRETER_STACK_LEN];
};

#define estack_ax_v	ax
#define estack_bx_v	bx

#define estack_ax_t	ax_t
#define estack_bx_t	bx_t

#define estack_ax(stack, top)					\
	({							\
		BUG_ON((top) <= INTERPRETER_STACK_EMPTY);	\
		&(stack)->e[top];				\
	})

#define estack_bx(stack, top)					\
	({							\
		BUG_ON((top) <= INTERPRETER_STACK_EMPTY + 1);	\
		&(stack)->e[(top) - 1];				\
	})

#define estack_push(stack, top, ax, bx, ax_t, bx_t)		\
	do {							\
		BUG_ON((top) >= INTERPRETER_STACK_LEN - 1);	\
		(stack)->e[(top) - 1].u.v = (bx);		\
		(stack)->e[(top) - 1].type = (bx_t);		\
		(bx) = (ax);					\
		(bx_t) = (ax_t);				\
		++(top);					\
	} while (0)

#define estack_pop(stack, top, ax, bx, ax_t, bx_t)		\
	do {							\
		BUG_ON((top) <= INTERPRETER_STACK_EMPTY);	\
		(ax) = (bx);					\
		(ax_t) = (bx_t);				\
		(bx) = (stack)->e[(top) - 2].u.v;		\
		(bx_t) = (stack)->e[(top) - 2].type;		\
		(top)--;					\
	} while (0)

enum lttng_interpreter_type {
	LTTNG_INTERPRETER_TYPE_S64,
	LTTNG_INTERPRETER_TYPE_U64,
	LTTNG_INTERPRETER_TYPE_SIGNED_ENUM,
	LTTNG_INTERPRETER_TYPE_UNSIGNED_ENUM,
	LTTNG_INTERPRETER_TYPE_DOUBLE,
	LTTNG_INTERPRETER_TYPE_STRING,
	LTTNG_INTERPRETER_TYPE_SEQUENCE,
};

/*
 * Represents the output parameter of the lttng interpreter.
 * Currently capturable field classes are integer, double, string and sequence
 * of integer.
 */
struct lttng_interpreter_output {
	enum lttng_interpreter_type type;
	union {
		int64_t s;
		uint64_t u;

		struct {
			const char *str;
			size_t len;
		} str;
		struct {
			const void *ptr;
			size_t nr_elem;

			/* Inner type. */
			const struct lttng_type *nested_type;
		} sequence;
	} u;
};

const char *lttng_bytecode_print_op(enum bytecode_op op);

void lttng_bytecode_filter_sync_state(struct lttng_bytecode_runtime *runtime);
int lttng_bytecode_validate(struct bytecode_runtime *bytecode);
int lttng_bytecode_specialize(const struct lttng_event_desc *event_desc,
		struct bytecode_runtime *bytecode);

uint64_t lttng_bytecode_filter_interpret_false(void *filter_data,
		struct lttng_probe_ctx *lttng_probe_ctx,
		const char *filter_stack_data);
uint64_t lttng_bytecode_filter_interpret(void *filter_data,
		struct lttng_probe_ctx *lttng_probe_ctx,
		const char *filter_stack_data);

#endif /* _LTTNG_FILTER_H */
