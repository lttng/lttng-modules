/* SPDX-License-Identifier: (GPL-2.0-only)
 *
 * wrapper/ibt.h
 *
 * Copyright (C) 2024 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_IBT_H
#define _LTTNG_WRAPPER_IBT_H

struct irq_ibt_state {
	u64 msr;
	unsigned long flags;
};

/*
 * Save (disable) and restore interrupts around MSR bit change and indirect
 * function call to make sure this thread is not migrated to another CPU which
 * would not have the MSR bit cleared.
 */

#ifdef CONFIG_X86_KERNEL_IBT
# include <asm/cpufeature.h>
# include <asm/msr.h>
static inline __attribute__((always_inline))
struct irq_ibt_state wrapper_irq_ibt_save(void)
{
	struct irq_ibt_state state = { 0, 0 };
	u64 msr;

	if (!cpu_feature_enabled(X86_FEATURE_IBT))
		goto end;
	local_irq_save(state.flags);
	rdmsrl(MSR_IA32_S_CET, msr);
	wrmsrl(MSR_IA32_S_CET, msr & ~CET_ENDBR_EN);
	state.msr = msr;
end:
	return state;
}

static inline __attribute__((always_inline))
void wrapper_irq_ibt_restore(struct irq_ibt_state state)
{
	u64 msr;

	if (!cpu_feature_enabled(X86_FEATURE_IBT))
		return;
	rdmsrl(MSR_IA32_S_CET, msr);
	msr &= ~CET_ENDBR_EN;
	msr |= (state.msr & CET_ENDBR_EN);
	wrmsrl(MSR_IA32_S_CET, msr);
	local_irq_restore(state.flags);
}
#else
static inline struct irq_ibt_state wrapper_irq_ibt_save(void) { struct irq_ibt_state state = { 0, 0 }; return state; }
static inline void wrapper_irq_ibt_restore(struct irq_ibt_state state) { }
#endif

#endif /* _LTTNG_WRAPPER_IBT_H */
