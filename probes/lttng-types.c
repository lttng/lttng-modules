/*
 * probes/lttng-types.c
 *
 * Copyright 2010 (c) - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * LTTng types.
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/seq_file.h>
#include <linux/jbd.h>	/* tid_t */
#include <linux/debugfs.h>
#include "../wrapper/vmalloc.h"	/* for wrapper_vmalloc_sync_all() */
#include "lttng-types.h"

struct dentry *lttng_types_dentry;

#undef ENTRY
#define ENTRY(name)	[atype_##name] = #name

const char * const astract_types[NR_ABSTRACT_TYPES] = {
	ENTRY(integer),
	ENTRY(enum),
	ENTRY(array),
	ENTRY(sequence),
	ENTRY(string),
};

#undef ENTRY
#define ENTRY(name)	[lttng_encode_##name] = #name

const char * const string_encodings[NR_STRING_ENCODINGS] = {
	ENTRY(UTF8),
	ENTRY(ASCII),
};

#define STAGE_EXPORT_ENUMS
#include "lttng-types.h"
#include "lttng-type-list.h"
#undef STAGE_EXPORT_ENUMS

struct lttng_type lttng_types[] = {
#define STAGE_EXPORT_TYPES
#include "lttng-types.h"
#include "lttng-type-list.h"
#undef STAGE_EXPORT_TYPES
};

static void print_indent(struct seq_file *m, unsigned int indent)
{
	int i;

	for (i = 0; i < indent; i++)
		seq_printf(m, "\t");
}

static void print_enum(struct seq_file *m, unsigned int indent,
		       const struct lttng_enum *lttng_enum)
{
	int i;

	for (i = 0; i < lttng_enum->len; i++) {
		print_indent(m, indent);
		if (lttng_enum->entries[i].start == lttng_enum->entries[i].end)
			seq_printf(m,	"{ %llu, %s },\n",
					lttng_enum->entries[i].start,
					lttng_enum->entries[i].string);
		else
			seq_printf(m,	"{ { %llu, %llu }, %s },\n",
					lttng_enum->entries[i].start,
					lttng_enum->entries[i].end,
					lttng_enum->entries[i].string);
	}
}

void lttng_print_event_type(struct seq_file *m, unsigned int indent,
			    const struct lttng_type *type)
{
	print_indent(m, indent);
	switch(type->atype) {
	case atype_integer:
		seq_printf(m,	"type %s%s{ parent = %s; size = %u; signed = %u; align = %u;",
				type->name ? : "", type->name ? " " : "",
				astract_types[type->atype],
				type->u.integer.size,
				type->u.integer.signedness,
				type->u.integer.alignment);
		if (type->u.integer.reverse_byte_order)
			seq_printf(m, 	" byte_order = %s;",
					(__BYTE_ORDER == __LITTLE_ENDIAN) ?
					"be" : "le");
		seq_printf(m, 	" }");
		break;
	case atype_enum:
		seq_printf(m,	"type %s%s{ parent = %s; parent.parent = %s; map = {\n",
				type->name ? : "", type->name ? " " : "",
				astract_types[type->atype],
				type->u.enumeration.parent_type);
		print_enum(m, indent + 2, &type->u.enumeration.def);
		print_indent(m, indent + 1);
		seq_printf(m,	"};\n");
		print_indent(m, indent);
		seq_printf(m,	"}");
		break;
	case atype_array:
		seq_printf(m,	"type %s%s{ parent = %s; elem_type = %s; length = %u; }",
				type->name ? : "", type->name ? " " : "",
				astract_types[type->atype],
				type->u.array.elem_type,
				type->u.array.length);
		break;
	case atype_sequence:
		seq_printf(m,	"type %s%s{ parent = %s; elem_type = %s; length_type = %s; }",
				type->name ? : "", type->name ? " " : "",
				astract_types[type->atype],
				type->u.sequence.elem_type,
				type->u.sequence.length_type);
		break;
	case atype_string:
		seq_printf(m,	"type %s%s{ parent = %s; encoding = %s; }",
				type->name ? : "", type->name ? " " : "",
				astract_types[type->atype],
				string_encodings[type->u.string.encoding]);
		break;
	default:
		seq_printf(m,	"<<< unknown abstract type %s for type %s%s>>>",
				astract_types[type->atype],
				type->name ? : "", type->name ? " " : "");
	}
}
EXPORT_SYMBOL_GPL(lttng_print_event_type);

static void *lttng_seq_start(struct seq_file *m, loff_t *pos)
{
	struct lttng_type *type = &lttng_types[*pos];

	if (type > &lttng_types[ARRAY_SIZE(lttng_types) - 1])
		return NULL;
	return type;
}

static void *lttng_seq_next(struct seq_file *m, void *v, loff_t *ppos)
{
	struct lttng_type *type = &lttng_types[++(*ppos)];

	if (type > &lttng_types[ARRAY_SIZE(lttng_types) - 1])
		return NULL;
	return type;
}

static void lttng_seq_stop(struct seq_file *m, void *v)
{
}

static int lttng_seq_show(struct seq_file *m, void *v)
{
	struct lttng_type *type = v;

	lttng_print_event_type(m, 0, type);
	seq_printf(m, ";\n");
	return 0;
}

static const struct seq_operations lttng_types_seq_ops = {
	.start = lttng_seq_start,
	.next = lttng_seq_next,
	.stop = lttng_seq_stop,
	.show = lttng_seq_show,
};

static int
lttng_types_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &lttng_types_seq_ops);
}

static const struct file_operations lttng_types_fops = {
	.open = lttng_types_open,
        .read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release_private,
};

static int lttng_types_init(void)
{
	int ret = 0;

	wrapper_vmalloc_sync_all();
	lttng_types_dentry = debugfs_create_file("lttng-types", S_IWUSR,
					NULL, NULL, &lttng_types_fops);
	if (IS_ERR(lttng_types_dentry) || !lttng_types_dentry) {
		printk(KERN_ERR "Error creating LTTng type export file\n");
		ret = -ENOMEM;
		goto error;
	}
error:
	return ret;
}

module_init(lttng_types_init);

static void lttng_types_exit(void)
{
	debugfs_remove(lttng_types_dentry);
}

module_exit(lttng_types_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng types");
