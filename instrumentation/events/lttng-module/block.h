#undef TRACE_SYSTEM
#define TRACE_SYSTEM block

#if !defined(LTTNG_TRACE_BLOCK_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_BLOCK_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/blktrace_api.h>
#include <linux/blkdev.h>
#include <linux/trace_seq.h>
#include <linux/version.h>

#ifndef _TRACE_BLOCK_DEF_
#define _TRACE_BLOCK_DEF_

enum {
	RWBS_FLAG_WRITE		= (1 << 0),
	RWBS_FLAG_DISCARD	= (1 << 1),
	RWBS_FLAG_READ		= (1 << 2),
	RWBS_FLAG_RAHEAD	= (1 << 3),
	RWBS_FLAG_BARRIER	= (1 << 4),
	RWBS_FLAG_SYNC		= (1 << 5),
	RWBS_FLAG_META		= (1 << 6),
	RWBS_FLAG_SECURE	= (1 << 7),
	RWBS_FLAG_FLUSH		= (1 << 8),
	RWBS_FLAG_FUA		= (1 << 9),
	RWBS_FLAG_PREFLUSH	= (1 << 10),
};

#endif /* _TRACE_BLOCK_DEF_ */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,8,0))

#define lttng_req_op(rq)	req_op(rq)
#define lttng_req_rw(rq)	((rq)->cmd_flags)
#define lttng_bio_op(bio)	bio_op(bio)
#define lttng_bio_rw(bio)	((bio)->bi_opf)

#define blk_rwbs_ctf_integer(type, rwbs, op, rw, bytes)			      \
		ctf_integer(type, rwbs,					      \
			(((op) == REQ_OP_WRITE || (op) == REQ_OP_WRITE_SAME) ? RWBS_FLAG_WRITE : \
			( (op) == REQ_OP_DISCARD ? RWBS_FLAG_DISCARD :	      \
			( (op) == REQ_OP_SECURE_ERASE ? (RWBS_FLAG_DISCARD | RWBS_FLAG_SECURE) : \
			( (op) == REQ_OP_FLUSH ? RWBS_FLAG_FLUSH :	      \
			( (op) == REQ_OP_READ ? RWBS_FLAG_READ :	      \
			( 0 ))))))					      \
			| ((rw) & REQ_RAHEAD ? RWBS_FLAG_RAHEAD : 0)	      \
			| ((rw) & REQ_SYNC ? RWBS_FLAG_SYNC : 0)	      \
			| ((rw) & REQ_META ? RWBS_FLAG_META : 0)	      \
			| ((rw) & REQ_PREFLUSH ? RWBS_FLAG_PREFLUSH : 0)      \
			| ((rw) & REQ_FUA ? RWBS_FLAG_FUA : 0))

#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0))

#define lttng_req_op(rq)
#define lttng_req_rw(rq)	((rq)->cmd_flags)
#define lttng_bio_op(bio)
#define lttng_bio_rw(bio)	((bio)->bi_rw)

#define blk_rwbs_ctf_integer(type, rwbs, op, rw, bytes)			      \
		ctf_integer(type, rwbs, ((rw) & WRITE ? RWBS_FLAG_WRITE :     \
			( (rw) & REQ_DISCARD ? RWBS_FLAG_DISCARD :	      \
			( (bytes) ? RWBS_FLAG_READ :			      \
			( 0 ))))					      \
			| ((rw) & REQ_RAHEAD ? RWBS_FLAG_RAHEAD : 0)	      \
			| ((rw) & REQ_SYNC ? RWBS_FLAG_SYNC : 0)	      \
			| ((rw) & REQ_META ? RWBS_FLAG_META : 0)	      \
			| ((rw) & REQ_SECURE ? RWBS_FLAG_SECURE : 0)	      \
			| ((rw) & REQ_FLUSH ? RWBS_FLAG_FLUSH : 0)	      \
			| ((rw) & REQ_FUA ? RWBS_FLAG_FUA : 0))

#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))

#define lttng_req_op(rq)
#define lttng_req_rw(rq)	((rq)->cmd_flags)
#define lttng_bio_op(bio)
#define lttng_bio_rw(bio)	((bio)->bi_rw)

#define blk_rwbs_ctf_integer(type, rwbs, op, rw, bytes)			      \
		ctf_integer(type, rwbs, ((rw) & WRITE ? RWBS_FLAG_WRITE :     \
			( (rw) & REQ_DISCARD ? RWBS_FLAG_DISCARD :	      \
			( (bytes) ? RWBS_FLAG_READ :			      \
			( 0 ))))					      \
			| ((rw) & REQ_RAHEAD ? RWBS_FLAG_RAHEAD : 0)	      \
			| ((rw) & REQ_SYNC ? RWBS_FLAG_SYNC : 0)	      \
			| ((rw) & REQ_META ? RWBS_FLAG_META : 0)	      \
			| ((rw) & REQ_SECURE ? RWBS_FLAG_SECURE : 0))

#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))

#define lttng_req_op(rq)
#define lttng_req_rw(rq)	((rq)->cmd_flags)
#define lttng_bio_op(bio)
#define lttng_bio_rw(bio)	((bio)->bi_rw)

#define blk_rwbs_ctf_integer(type, rwbs, op, rw, bytes)			      \
		ctf_integer(type, rwbs, ((rw) & WRITE ? RWBS_FLAG_WRITE :     \
			( (rw) & REQ_DISCARD ? RWBS_FLAG_DISCARD :	      \
			( (bytes) ? RWBS_FLAG_READ :			      \
			( 0 ))))					      \
			| ((rw) & REQ_RAHEAD ? RWBS_FLAG_RAHEAD : 0)	      \
			| ((rw) & REQ_HARDBARRIER ? RWBS_FLAG_BARRIER : 0)    \
			| ((rw) & REQ_SYNC ? RWBS_FLAG_SYNC : 0)	      \
			| ((rw) & REQ_META ? RWBS_FLAG_META : 0)	      \
			| ((rw) & REQ_SECURE ? RWBS_FLAG_SECURE : 0))

#else

#define lttng_req_op(rq)
#define lttng_req_rw(rq)	((rq)->cmd_flags)
#define lttng_bio_op(bio)
#define lttng_bio_rw(bio)	((bio)->bi_rw)

#define blk_rwbs_ctf_integer(type, rwbs, op, rw, bytes)			      \
		ctf_integer(type, rwbs, ((rw) & WRITE ? RWBS_FLAG_WRITE :     \
			( (rw) & (1 << BIO_RW_DISCARD) ? RWBS_FLAG_DISCARD :  \
			( (bytes) ? RWBS_FLAG_READ :			      \
			( 0 ))))					      \
			| ((rw) & (1 << BIO_RW_AHEAD) ? RWBS_FLAG_RAHEAD : 0) \
			| ((rw) & (1 << BIO_RW_SYNCIO) ? RWBS_FLAG_SYNC : 0)  \
			| ((rw) & (1 << BIO_RW_META) ? RWBS_FLAG_META : 0)    \
			| ((rw) & (1 << BIO_RW_BARRIER) ? RWBS_FLAG_BARRIER : 0))

#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
LTTNG_TRACEPOINT_EVENT_CLASS(block_buffer,

	TP_PROTO(struct buffer_head *bh),

	TP_ARGS(bh),

	TP_FIELDS (
		ctf_integer(dev_t, dev, bh->b_bdev->bd_dev)
		ctf_integer(sector_t, sector, bh->b_blocknr)
		ctf_integer(size_t, size, bh->b_size)
	)
)

/**
 * block_touch_buffer - mark a buffer accessed
 * @bh: buffer_head being touched
 *
 * Called from touch_buffer().
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(block_buffer, block_touch_buffer,

	TP_PROTO(struct buffer_head *bh),

	TP_ARGS(bh)
)

/**
 * block_dirty_buffer - mark a buffer dirty
 * @bh: buffer_head being dirtied
 *
 * Called from mark_buffer_dirty().
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(block_buffer, block_dirty_buffer,

	TP_PROTO(struct buffer_head *bh),

	TP_ARGS(bh)
)
#endif

LTTNG_TRACEPOINT_EVENT_CLASS_CODE(block_rq_with_error,

	TP_PROTO(struct request_queue *q, struct request *rq),

	TP_ARGS(q, rq),

	TP_locvar(
		sector_t sector;
		unsigned int nr_sector;
		unsigned char *cmd;
		size_t cmd_len;
	),

	TP_code_pre(
		if (rq->cmd_type == REQ_TYPE_BLOCK_PC) {
			tp_locvar->sector = 0;
			tp_locvar->nr_sector = 0;
			tp_locvar->cmd = rq->cmd;
			tp_locvar->cmd_len = rq->cmd_len;
		} else {
			tp_locvar->sector = blk_rq_pos(rq);
			tp_locvar->nr_sector = blk_rq_sectors(rq);
			tp_locvar->cmd = NULL;
			tp_locvar->cmd_len = 0;
		}
	),

	TP_FIELDS(
		ctf_integer(dev_t, dev,
			rq->rq_disk ? disk_devt(rq->rq_disk) : 0)
		ctf_integer(sector_t, sector, tp_locvar->sector)
		ctf_integer(unsigned int, nr_sector, tp_locvar->nr_sector)
		ctf_integer(int, errors, rq->errors)
		blk_rwbs_ctf_integer(unsigned int, rwbs,
			lttng_req_op(rq), lttng_req_rw(rq), blk_rq_bytes(rq))
		ctf_sequence_hex(unsigned char, cmd,
			tp_locvar->cmd, size_t, tp_locvar->cmd_len)
	),

	TP_code_post()
)

/**
 * block_rq_abort - abort block operation request
 * @q: queue containing the block operation request
 * @rq: block IO operation request
 *
 * Called immediately after pending block IO operation request @rq in
 * queue @q is aborted. The fields in the operation request @rq
 * can be examined to determine which device and sectors the pending
 * operation would access.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(block_rq_with_error, block_rq_abort,

	TP_PROTO(struct request_queue *q, struct request *rq),

	TP_ARGS(q, rq)
)

/**
 * block_rq_requeue - place block IO request back on a queue
 * @q: queue holding operation
 * @rq: block IO operation request
 *
 * The block operation request @rq is being placed back into queue
 * @q.  For some reason the request was not completed and needs to be
 * put back in the queue.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(block_rq_with_error, block_rq_requeue,

	TP_PROTO(struct request_queue *q, struct request *rq),

	TP_ARGS(q, rq)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,5)	\
	|| LTTNG_KERNEL_RANGE(3,12,21, 3,13,0)		\
	|| LTTNG_KERNEL_RANGE(3,10,41, 3,11,0)		\
	|| LTTNG_KERNEL_RANGE(3,4,91, 3,5,0)		\
	|| LTTNG_KERNEL_RANGE(3,2,58, 3,3,0)		\
	|| LTTNG_UBUNTU_KERNEL_RANGE(3,13,11,28, 3,14,0,0)	\
	|| LTTNG_RHEL_KERNEL_RANGE(3,10,0,229,0,0, 3,11,0,0,0,0))

/**
 * block_rq_complete - block IO operation completed by device driver
 * @q: queue containing the block operation request
 * @rq: block operations request
 * @nr_bytes: number of completed bytes
 *
 * The block_rq_complete tracepoint event indicates that some portion
 * of operation request has been completed by the device driver.  If
 * the @rq->bio is %NULL, then there is absolutely no additional work to
 * do for the request. If @rq->bio is non-NULL then there is
 * additional work required to complete the request.
 */
LTTNG_TRACEPOINT_EVENT_CODE(block_rq_complete,

	TP_PROTO(struct request_queue *q, struct request *rq,
		 unsigned int nr_bytes),

	TP_ARGS(q, rq, nr_bytes),

	TP_locvar(
		unsigned char *cmd;
		size_t cmd_len;
	),

	TP_code_pre(
		if (rq->cmd_type == REQ_TYPE_BLOCK_PC) {
			tp_locvar->cmd = rq->cmd;
			tp_locvar->cmd_len = rq->cmd_len;
		} else {
			tp_locvar->cmd = NULL;
			tp_locvar->cmd_len = 0;
		}
	),

	TP_FIELDS(
		ctf_integer(dev_t, dev,
			rq->rq_disk ? disk_devt(rq->rq_disk) : 0)
		ctf_integer(sector_t, sector, blk_rq_pos(rq))
		ctf_integer(unsigned int, nr_sector, nr_bytes >> 9)
		ctf_integer(int, errors, rq->errors)
		blk_rwbs_ctf_integer(unsigned int, rwbs,
			lttng_req_op(rq), lttng_req_rw(rq), nr_bytes)
		ctf_sequence_hex(unsigned char, cmd,
			tp_locvar->cmd, size_t, tp_locvar->cmd_len)
	),

	TP_code_post()
)

#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0)) */

/**
 * block_rq_complete - block IO operation completed by device driver
 * @q: queue containing the block operation request
 * @rq: block operations request
 *
 * The block_rq_complete tracepoint event indicates that some portion
 * of operation request has been completed by the device driver.  If
 * the @rq->bio is %NULL, then there is absolutely no additional work to
 * do for the request. If @rq->bio is non-NULL then there is
 * additional work required to complete the request.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(block_rq_with_error, block_rq_complete,

	TP_PROTO(struct request_queue *q, struct request *rq),

	TP_ARGS(q, rq)
)

#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0)) */

LTTNG_TRACEPOINT_EVENT_CLASS_CODE(block_rq,

	TP_PROTO(struct request_queue *q, struct request *rq),

	TP_ARGS(q, rq),

	TP_locvar(
		sector_t sector;
		unsigned int nr_sector;
		unsigned int bytes;
		unsigned char *cmd;
		size_t cmd_len;
	),

	TP_code_pre(
		if (rq->cmd_type == REQ_TYPE_BLOCK_PC) {
			tp_locvar->sector = 0;
			tp_locvar->nr_sector = 0;
			tp_locvar->bytes = blk_rq_bytes(rq);
			tp_locvar->cmd = rq->cmd;
			tp_locvar->cmd_len = rq->cmd_len;
		} else {
			tp_locvar->sector = blk_rq_pos(rq);
			tp_locvar->nr_sector = blk_rq_sectors(rq);
			tp_locvar->bytes = 0;
			tp_locvar->cmd = NULL;
			tp_locvar->cmd_len = 0;
		}
	),

	TP_FIELDS(
		ctf_integer(dev_t, dev,
			rq->rq_disk ? disk_devt(rq->rq_disk) : 0)
		ctf_integer(sector_t, sector, tp_locvar->sector)
		ctf_integer(unsigned int, nr_sector, tp_locvar->nr_sector)
		ctf_integer(unsigned int, bytes, tp_locvar->bytes)
		ctf_integer(pid_t, tid, current->pid)
		blk_rwbs_ctf_integer(unsigned int, rwbs,
			lttng_req_op(rq), lttng_req_rw(rq), blk_rq_bytes(rq))
		ctf_sequence_hex(unsigned char, cmd,
			tp_locvar->cmd, size_t, tp_locvar->cmd_len)
		ctf_array_text(char, comm, current->comm, TASK_COMM_LEN)
	),

	TP_code_post()
)

/**
 * block_rq_insert - insert block operation request into queue
 * @q: target queue
 * @rq: block IO operation request
 *
 * Called immediately before block operation request @rq is inserted
 * into queue @q.  The fields in the operation request @rq struct can
 * be examined to determine which device and sectors the pending
 * operation would access.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(block_rq, block_rq_insert,

	TP_PROTO(struct request_queue *q, struct request *rq),

	TP_ARGS(q, rq)
)

/**
 * block_rq_issue - issue pending block IO request operation to device driver
 * @q: queue holding operation
 * @rq: block IO operation operation request
 *
 * Called when block operation request @rq from queue @q is sent to a
 * device driver for processing.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(block_rq, block_rq_issue,

	TP_PROTO(struct request_queue *q, struct request *rq),

	TP_ARGS(q, rq)
)

/**
 * block_bio_bounce - used bounce buffer when processing block operation
 * @q: queue holding the block operation
 * @bio: block operation
 *
 * A bounce buffer was used to handle the block operation @bio in @q.
 * This occurs when hardware limitations prevent a direct transfer of
 * data between the @bio data memory area and the IO device.  Use of a
 * bounce buffer requires extra copying of data and decreases
 * performance.
 */
LTTNG_TRACEPOINT_EVENT(block_bio_bounce,

	TP_PROTO(struct request_queue *q, struct bio *bio),

	TP_ARGS(q, bio),

	TP_FIELDS(
		ctf_integer(dev_t, dev, bio->bi_bdev ? bio->bi_bdev->bd_dev : 0)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
		ctf_integer(sector_t, sector, bio->bi_iter.bi_sector)
		ctf_integer(unsigned int, nr_sector, bio_sectors(bio))
		blk_rwbs_ctf_integer(unsigned int, rwbs,
			lttng_bio_op(bio), lttng_bio_rw(bio),
			bio->bi_iter.bi_size)
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0)) */
		ctf_integer(sector_t, sector, bio->bi_sector)
		ctf_integer(unsigned int, nr_sector, bio->bi_size >> 9)
		blk_rwbs_ctf_integer(unsigned int, rwbs,
			lttng_bio_op(bio), lttng_bio_rw(bio),
			bio->bi_size)
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0)) */
		ctf_integer(pid_t, tid, current->pid)
		ctf_array_text(char, comm, current->comm, TASK_COMM_LEN)
	)
)

/**
 * block_bio_complete - completed all work on the block operation
 * @q: queue holding the block operation
 * @bio: block operation completed
 * @error: io error value
 *
 * This tracepoint indicates there is no further work to do on this
 * block IO operation @bio.
 */
LTTNG_TRACEPOINT_EVENT(block_bio_complete,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
	TP_PROTO(struct request_queue *q, struct bio *bio, int error),

	TP_ARGS(q, bio, error),
#else
	TP_PROTO(struct request_queue *q, struct bio *bio),

	TP_ARGS(q, bio),
#endif

	TP_FIELDS(
		ctf_integer(dev_t, dev, bio->bi_bdev->bd_dev)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
		ctf_integer(sector_t, sector, bio->bi_iter.bi_sector)
		ctf_integer(unsigned int, nr_sector, bio_sectors(bio))
		ctf_integer(int, error, error)
		blk_rwbs_ctf_integer(unsigned int, rwbs,
			lttng_bio_op(bio), lttng_bio_rw(bio),
			bio->bi_iter.bi_size)
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0)) */
		ctf_integer(sector_t, sector, bio->bi_sector)
		ctf_integer(unsigned int, nr_sector, bio->bi_size >> 9)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
		ctf_integer(int, error, error)
#else
		ctf_integer(int, error, 0)
#endif
		blk_rwbs_ctf_integer(unsigned int, rwbs,
			lttng_bio_op(bio), lttng_bio_rw(bio), bio->bi_size)
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0)) */
	)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
LTTNG_TRACEPOINT_EVENT_CLASS(block_bio_merge,

	TP_PROTO(struct request_queue *q, struct request *rq, struct bio *bio),

	TP_ARGS(q, rq, bio),

	TP_FIELDS(
		ctf_integer(dev_t, dev, bio->bi_bdev->bd_dev)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
		ctf_integer(sector_t, sector, bio->bi_iter.bi_sector)
		ctf_integer(unsigned int, nr_sector, bio_sectors(bio))
		blk_rwbs_ctf_integer(unsigned int, rwbs,
			lttng_bio_op(bio), lttng_bio_rw(bio),
			bio->bi_iter.bi_size)
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0)) */
		ctf_integer(sector_t, sector, bio->bi_sector)
		ctf_integer(unsigned int, nr_sector, bio->bi_size >> 9)
		blk_rwbs_ctf_integer(unsigned int, rwbs,
			lttng_bio_op(bio), lttng_bio_rw(bio), bio->bi_size)
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0)) */
		ctf_integer(pid_t, tid, current->pid)
		ctf_array_text(char, comm, current->comm, TASK_COMM_LEN)
	)
)

/**
 * block_bio_backmerge - merging block operation to the end of an existing operation
 * @q: queue holding operation
 * @bio: new block operation to merge
 *
 * Merging block request @bio to the end of an existing block request
 * in queue @q.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(block_bio_merge, block_bio_backmerge,

	TP_PROTO(struct request_queue *q, struct request *rq, struct bio *bio),

	TP_ARGS(q, rq, bio)
)

/**
 * block_bio_frontmerge - merging block operation to the beginning of an existing operation
 * @q: queue holding operation
 * @bio: new block operation to merge
 *
 * Merging block IO operation @bio to the beginning of an existing block
 * operation in queue @q.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(block_bio_merge, block_bio_frontmerge,

	TP_PROTO(struct request_queue *q, struct request *rq, struct bio *bio),

	TP_ARGS(q, rq, bio)
)

/**
 * block_bio_queue - putting new block IO operation in queue
 * @q: queue holding operation
 * @bio: new block operation
 *
 * About to place the block IO operation @bio into queue @q.
 */
LTTNG_TRACEPOINT_EVENT(block_bio_queue,

	TP_PROTO(struct request_queue *q, struct bio *bio),

	TP_ARGS(q, bio),

	TP_FIELDS(
		ctf_integer(dev_t, dev, bio->bi_bdev->bd_dev)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
		ctf_integer(sector_t, sector, bio->bi_iter.bi_sector)
		ctf_integer(unsigned int, nr_sector, bio_sectors(bio))
		blk_rwbs_ctf_integer(unsigned int, rwbs,
			lttng_bio_op(bio), lttng_bio_rw(bio),
			bio->bi_iter.bi_size)
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0)) */
		ctf_integer(sector_t, sector, bio->bi_sector)
		ctf_integer(unsigned int, nr_sector, bio->bi_size >> 9)
		blk_rwbs_ctf_integer(unsigned int, rwbs,
			lttng_bio_op(bio), lttng_bio_rw(bio), bio->bi_size)
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0)) */
		ctf_integer(pid_t, tid, current->pid)
		ctf_array_text(char, comm, current->comm, TASK_COMM_LEN)
	)
)
#else /* if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0)) */
LTTNG_TRACEPOINT_EVENT_CLASS(block_bio,

	TP_PROTO(struct request_queue *q, struct bio *bio),

	TP_ARGS(q, bio),

	TP_FIELDS(
		ctf_integer(dev_t, dev, bio->bi_bdev ? bio->bi_bdev->bd_dev : 0)
		ctf_integer(sector_t, sector, bio->bi_sector)
		ctf_integer(unsigned int, nr_sector, bio->bi_size >> 9)
		blk_rwbs_ctf_integer(unsigned int, rwbs,
			lttng_bio_op(bio), lttng_bio_rw(bio), bio->bi_size)
		ctf_integer(pid_t, tid, current->pid)
		ctf_array_text(char, comm, current->comm, TASK_COMM_LEN)
	)
)

/**
 * block_bio_backmerge - merging block operation to the end of an existing operation
 * @q: queue holding operation
 * @bio: new block operation to merge
 *
 * Merging block request @bio to the end of an existing block request
 * in queue @q.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(block_bio, block_bio_backmerge,

	TP_PROTO(struct request_queue *q, struct bio *bio),

	TP_ARGS(q, bio)
)

/**
 * block_bio_frontmerge - merging block operation to the beginning of an existing operation
 * @q: queue holding operation
 * @bio: new block operation to merge
 *
 * Merging block IO operation @bio to the beginning of an existing block
 * operation in queue @q.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(block_bio, block_bio_frontmerge,

	TP_PROTO(struct request_queue *q, struct bio *bio),

	TP_ARGS(q, bio)
)

/**
 * block_bio_queue - putting new block IO operation in queue
 * @q: queue holding operation
 * @bio: new block operation
 *
 * About to place the block IO operation @bio into queue @q.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(block_bio, block_bio_queue,

	TP_PROTO(struct request_queue *q, struct bio *bio),

	TP_ARGS(q, bio)
)
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0)) */

LTTNG_TRACEPOINT_EVENT_CLASS(block_get_rq,

	TP_PROTO(struct request_queue *q, struct bio *bio, int rw),

	TP_ARGS(q, bio, rw),

	TP_FIELDS(
		ctf_integer(dev_t, dev, bio ? bio->bi_bdev->bd_dev : 0)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
		ctf_integer(sector_t, sector, bio ? bio->bi_iter.bi_sector : 0)
		ctf_integer(unsigned int, nr_sector,
			bio ? bio_sectors(bio) : 0)
		blk_rwbs_ctf_integer(unsigned int, rwbs,
			bio ? lttng_bio_op(bio) : 0,
			bio ? lttng_bio_rw(bio) : 0,
			bio ? bio->bi_iter.bi_size : 0)
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0)) */
		ctf_integer(sector_t, sector, bio ? bio->bi_sector : 0)
		ctf_integer(unsigned int, nr_sector,
			bio ? bio->bi_size >> 9 : 0)
		blk_rwbs_ctf_integer(unsigned int, rwbs,
			bio ? lttng_bio_op(bio) : 0,
			bio ? lttng_bio_rw(bio) : 0,
			bio ? bio->bi_size : 0)
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0)) */
		ctf_integer(pid_t, tid, current->pid)
		ctf_array_text(char, comm, current->comm, TASK_COMM_LEN)
        )
)

/**
 * block_getrq - get a free request entry in queue for block IO operations
 * @q: queue for operations
 * @bio: pending block IO operation
 * @rw: low bit indicates a read (%0) or a write (%1)
 *
 * A request struct for queue @q has been allocated to handle the
 * block IO operation @bio.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(block_get_rq, block_getrq,

	TP_PROTO(struct request_queue *q, struct bio *bio, int rw),

	TP_ARGS(q, bio, rw)
)

/**
 * block_sleeprq - waiting to get a free request entry in queue for block IO operation
 * @q: queue for operation
 * @bio: pending block IO operation
 * @rw: low bit indicates a read (%0) or a write (%1)
 *
 * In the case where a request struct cannot be provided for queue @q
 * the process needs to wait for an request struct to become
 * available.  This tracepoint event is generated each time the
 * process goes to sleep waiting for request struct become available.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(block_get_rq, block_sleeprq,

	TP_PROTO(struct request_queue *q, struct bio *bio, int rw),

	TP_ARGS(q, bio, rw)
)

/**
 * block_plug - keep operations requests in request queue
 * @q: request queue to plug
 *
 * Plug the request queue @q.  Do not allow block operation requests
 * to be sent to the device driver. Instead, accumulate requests in
 * the queue to improve throughput performance of the block device.
 */
LTTNG_TRACEPOINT_EVENT(block_plug,

	TP_PROTO(struct request_queue *q),

	TP_ARGS(q),

	TP_FIELDS(
		ctf_integer(pid_t, tid, current->pid)
		ctf_array_text(char, comm, current->comm, TASK_COMM_LEN)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(block_unplug,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39))
	TP_PROTO(struct request_queue *q, unsigned int depth, bool explicit),

	TP_ARGS(q, depth, explicit),
#else
	TP_PROTO(struct request_queue *q),

	TP_ARGS(q),
#endif

	TP_FIELDS(
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39))
		ctf_integer(int, nr_rq, depth)
#else
		ctf_integer(int, nr_rq, q->rq.count[READ] + q->rq.count[WRITE])
#endif
		ctf_integer(pid_t, tid, current->pid)
		ctf_array_text(char, comm, current->comm, TASK_COMM_LEN)
	)
)

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39))
/**
 * block_unplug_timer - timed release of operations requests in queue to device driver
 * @q: request queue to unplug
 *
 * Unplug the request queue @q because a timer expired and allow block
 * operation requests to be sent to the device driver.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(block_unplug, block_unplug_timer,

	TP_PROTO(struct request_queue *q),

	TP_ARGS(q)
)
#endif

/**
 * block_unplug - release of operations requests in request queue
 * @q: request queue to unplug
 * @depth: number of requests just added to the queue
 * @explicit: whether this was an explicit unplug, or one from schedule()
 *
 * Unplug request queue @q because device driver is scheduled to work
 * on elements in the request queue.
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39))
LTTNG_TRACEPOINT_EVENT_INSTANCE(block_unplug, block_unplug,
#else
LTTNG_TRACEPOINT_EVENT_INSTANCE(block_unplug, block_unplug_io,
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39))
	TP_PROTO(struct request_queue *q, unsigned int depth, bool explicit),

	TP_ARGS(q, depth, explicit)
#else
	TP_PROTO(struct request_queue *q),

	TP_ARGS(q)
#endif
)

/**
 * block_split - split a single bio struct into two bio structs
 * @q: queue containing the bio
 * @bio: block operation being split
 * @new_sector: The starting sector for the new bio
 *
 * The bio request @bio in request queue @q needs to be split into two
 * bio requests. The newly created @bio request starts at
 * @new_sector. This split may be required due to hardware limitation
 * such as operation crossing device boundaries in a RAID system.
 */
LTTNG_TRACEPOINT_EVENT(block_split,

	TP_PROTO(struct request_queue *q, struct bio *bio,
		 unsigned int new_sector),

	TP_ARGS(q, bio, new_sector),

	TP_FIELDS(
		ctf_integer(dev_t, dev, bio->bi_bdev->bd_dev)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
		ctf_integer(sector_t, sector, bio->bi_iter.bi_sector)
		blk_rwbs_ctf_integer(unsigned int, rwbs,
			lttng_bio_op(bio), lttng_bio_rw(bio),
			bio->bi_iter.bi_size)
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0)) */
		ctf_integer(sector_t, sector, bio->bi_sector)
		blk_rwbs_ctf_integer(unsigned int, rwbs,
			lttng_bio_op(bio), lttng_bio_rw(bio), bio->bi_size)
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0)) */
		ctf_integer(sector_t, new_sector, new_sector)
		ctf_integer(pid_t, tid, current->pid)
		ctf_array_text(char, comm, current->comm, TASK_COMM_LEN)
	)
)

/**
 * block_bio_remap - map request for a logical device to the raw device
 * @q: queue holding the operation
 * @bio: revised operation
 * @dev: device for the operation
 * @from: original sector for the operation
 *
 * An operation for a logical device has been mapped to the
 * raw block device.
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
LTTNG_TRACEPOINT_EVENT(block_bio_remap,
#else
LTTNG_TRACEPOINT_EVENT(block_remap,
#endif

	TP_PROTO(struct request_queue *q, struct bio *bio, dev_t dev,
		 sector_t from),

	TP_ARGS(q, bio, dev, from),

	TP_FIELDS(
		ctf_integer(dev_t, dev, bio->bi_bdev->bd_dev)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
		ctf_integer(sector_t, sector, bio->bi_iter.bi_sector)
		ctf_integer(unsigned int, nr_sector, bio_sectors(bio))
		blk_rwbs_ctf_integer(unsigned int, rwbs,
			lttng_bio_op(bio), lttng_bio_rw(bio),
			bio->bi_iter.bi_size)
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0)) */
		ctf_integer(sector_t, sector, bio->bi_sector)
		ctf_integer(unsigned int, nr_sector, bio->bi_size >> 9)
		blk_rwbs_ctf_integer(unsigned int, rwbs,
			lttng_bio_op(bio), lttng_bio_rw(bio), bio->bi_size)
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0)) */
		ctf_integer(dev_t, old_dev, dev)
		ctf_integer(sector_t, old_sector, from)
	)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
/**
 * block_rq_remap - map request for a block operation request
 * @q: queue holding the operation
 * @rq: block IO operation request
 * @dev: device for the operation
 * @from: original sector for the operation
 *
 * The block operation request @rq in @q has been remapped.  The block
 * operation request @rq holds the current information and @from hold
 * the original sector.
 */
LTTNG_TRACEPOINT_EVENT(block_rq_remap,

	TP_PROTO(struct request_queue *q, struct request *rq, dev_t dev,
		 sector_t from),

	TP_ARGS(q, rq, dev, from),

	TP_FIELDS(
		ctf_integer(dev_t, dev, disk_devt(rq->rq_disk))
		ctf_integer(sector_t, sector, blk_rq_pos(rq))
		ctf_integer(unsigned int, nr_sector, blk_rq_sectors(rq))
		ctf_integer(dev_t, old_dev, dev)
		ctf_integer(sector_t, old_sector, from)
		blk_rwbs_ctf_integer(unsigned int, rwbs,
			lttng_req_op(rq), lttng_req_rw(rq), blk_rq_bytes(rq))
	)
)
#endif

#undef __print_rwbs_flags
#undef blk_fill_rwbs

#endif /* LTTNG_TRACE_BLOCK_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
