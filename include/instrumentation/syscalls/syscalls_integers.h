// SPDX-FileCopyrightText: 2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#ifdef CONFIG_X86_64
#include "x86-64-syscalls_integers.h"
#endif

#ifdef CONFIG_X86_32
#include "x86-32-syscalls_integers.h"
#endif

#ifdef CONFIG_ARM
#include "arm-32-syscalls_integers.h"
#endif

#ifdef CONFIG_PPC
#include "powerpc-32-syscalls_integers.h"
#endif

#ifdef CONFIG_CPU_MIPS32
#include "mips-32-syscalls_integers.h"
#endif

#ifdef CONFIG_ARM64
#include "arm-64-syscalls_integers.h"
#endif
