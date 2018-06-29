/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/splice.c
 *
 * wrapper around splice_to_pipe. Using KALLSYMS to get its address when
 * available, else we need to have a kernel that exports this function to GPL
 * modules.
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifdef CONFIG_KALLSYMS

#include <linux/kallsyms.h>
#include <linux/fs.h>
#include <linux/splice.h>
#include <wrapper/kallsyms.h>

static
ssize_t (*splice_to_pipe_sym)(struct pipe_inode_info *pipe,
			      struct splice_pipe_desc *spd);

ssize_t wrapper_splice_to_pipe(struct pipe_inode_info *pipe,
			       struct splice_pipe_desc *spd)
{
	if (!splice_to_pipe_sym)
		splice_to_pipe_sym = (void *) kallsyms_lookup_funcptr("splice_to_pipe"); 
	if (splice_to_pipe_sym) {
		return splice_to_pipe_sym(pipe, spd);
	} else {
		printk_once(KERN_WARNING "LTTng: splice_to_pipe symbol lookup failed.\n");
		return -ENOSYS;
	}
}

#else

#include <linux/fs.h>
#include <linux/splice.h>

ssize_t wrapper_splice_to_pipe(struct pipe_inode_info *pipe,
			       struct splice_pipe_desc *spd)
{
	return splice_to_pipe(pipe, spd);
}

#endif
