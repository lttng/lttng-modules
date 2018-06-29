/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/splice.h
 *
 * wrapper around splice_to_pipe. Using KALLSYMS to get its address when
 * available, else we need to have a kernel that exports this function to GPL
 * modules.
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_SPLICE_H
#define _LTTNG_WRAPPER_SPLICE_H

#include <linux/splice.h>

ssize_t wrapper_splice_to_pipe(struct pipe_inode_info *pipe,
			       struct splice_pipe_desc *spd);

#ifndef PIPE_DEF_BUFFERS
#define PIPE_DEF_BUFFERS 16
#endif

#endif /* _LTTNG_WRAPPER_SPLICE_H */
