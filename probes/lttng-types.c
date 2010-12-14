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
#include "lttng-types.h"

struct dentry *lttng_types_dentry;

#define ATYPE_ENTRY(name)	[atype_##name] = #name

const char * const astract_types[NR_ABSTRACT_TYPES] = {
	ATYPE_ENTRY(integer),
	ATYPE_ENTRY(enum),
	ATYPE_ENTRY(array),
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

static void print_enum(struct seq_file *m, const struct lttng_enum *lttng_enum)
{
	int i;

	for (i = 0; i < lttng_enum->len; i++) {
		if (lttng_enum->entries[i].start == lttng_enum->entries[i].end)
			seq_printf(m,	"\t\t{ %llu, %s },\n",
					lttng_enum->entries[i].start,
					lttng_enum->entries[i].string);
		else
			seq_printf(m,	"\t\t{ { %llu, %llu }, %s },\n",
					lttng_enum->entries[i].start,
					lttng_enum->entries[i].end,
					lttng_enum->entries[i].string);
	}
}

static void print_event_type(struct seq_file *m, const struct lttng_type *type)
{
	switch(type->atype) {
	case atype_integer:
		seq_printf(m,	"type %s {\n"
				"\tparent = %s;\n"
				"\tsize = %u;\n"
				"\tsigned = %u;\n"
				"\talign = %u;\n"
				"};\n", type->name,
				astract_types[type->atype],
				type->u.integer.size,
				type->u.integer.signedness,
				type->u.integer.alignment);
		break;
	case atype_enum:
		seq_printf(m,	"type %s {\n"
				"\tparent = %s;\n"
				"\tparent.parent = %s;\n"
				"\tmap = {\n",
				type->name,
				astract_types[type->atype],
				type->u.enumeration.parent_type);
		print_enum(m, &type->u.enumeration.def);
		seq_printf(m,	"\t};\n"
				"};\n");
		break;
	case atype_array:
		seq_printf(m,	"type %s {\n"
				"\tparent = %s;\n"
				"\telem_type = %s;\n"
				"\tlength = %u;\n"
				"};\n", type->name,
				astract_types[type->atype],
				type->u.array.elem_type,
				type->u.array.length);
		break;
	default:
		seq_printf(m,	"<<< unknown abstract type %s for type %s >>>\n",
				astract_types[type->atype],
				type->name);
	}
}

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

	print_event_type(m, type);
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
