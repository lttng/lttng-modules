// SPDX-FileCopyrightText: 2022 Rouven Czerwinski <r.czerwinski@pengutronix.de>
//
// SPDX-License-Identifier: GPL-2.0-only

#undef TRACE_SYSTEM
#define TRACE_SYSTEM dma_fence

#if !defined(LTTNG_TRACE_DMA_FENCE_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_DMA_FENCE_H

#include <lttng/tracepoint-event.h>

LTTNG_TRACEPOINT_EVENT_CLASS(dma_fence_class,

	TP_PROTO(struct dma_fence *fence),

	TP_ARGS(fence),

	TP_FIELDS(
		ctf_string(driver, fence->ops->get_driver_name(fence))
		ctf_string(timeline, fence->ops->get_timeline_name(fence))
		ctf_integer(unsigned int, context, fence->context)
		ctf_integer(unsigned int, seqno, fence->seqno)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(dma_fence_class,
	dma_fence_emit,

	TP_PROTO(struct dma_fence *fence),

	TP_ARGS(fence)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(dma_fence_class,
	dma_fence_init,

	TP_PROTO(struct dma_fence *fence),

	TP_ARGS(fence)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(dma_fence_class,
	dma_fence_destroy,

	TP_PROTO(struct dma_fence *fence),

	TP_ARGS(fence)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(dma_fence_class,
	dma_fence_enable_signal,

	TP_PROTO(struct dma_fence *fence),

	TP_ARGS(fence)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(dma_fence_class,
	dma_fence_signaled,

	TP_PROTO(struct dma_fence *fence),

	TP_ARGS(fence)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(dma_fence_class,
	dma_fence_wait_start,

	TP_PROTO(struct dma_fence *fence),

	TP_ARGS(fence)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(dma_fence_class,
	dma_fence_wait_end,

	TP_PROTO(struct dma_fence *fence),

	TP_ARGS(fence)
)

#endif /*  LTTNG_TRACE_DMA_FENCE_H */

/* This part must be outside protection */
#include <lttng/define_trace.h>
