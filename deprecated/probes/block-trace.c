/*
 * ltt/probes/block-trace.c
 *
 * block layer tracepoint probes.
 *
 * (C) Copyright 2009 - Mathieu Desnoyers <mathieu.desnoyers@polymtl.ca>
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>

#include <trace/events/block.h>

/*
 * Add rq cmd as a sequence. Needs new type. (size + binary blob)
 */

void probe_block_rq_abort(void *data, struct request_queue *q, struct request *rq)
{
	int rw = rq->cmd_flags & 0x03;

	if (blk_discard_rq(rq))
		rw |= (1 << BIO_RW_DISCARD);

	if (blk_pc_request(rq)) {
		trace_mark_tp(block, rq_abort_pc, block_rq_abort,
			probe_block_rq_abort,
			"data_len %u rw %d errors %d",
			blk_rq_bytes(rq), rw, rq->errors);
	} else {
		/*
		 * FIXME Using a simple trace_mark for the second event
		 * possibility because tracepoints do not support multiple
		 * connections to the same probe yet. They should have some
		 * refcounting. Need to enable both rq_abort_pc and rq_abort_fs
		 * markers to have the rq_abort_fs marker enabled.
		 */
		trace_mark(block, rq_abort_fs,
			"hard_sector %llu "
			"rw %d errors %d", (unsigned long long)blk_rq_pos(rq),
			rw, rq->errors);
	}
}

void probe_block_rq_insert(void *data, struct request_queue *q, struct request *rq)
{
	int rw = rq->cmd_flags & 0x03;

	if (blk_discard_rq(rq))
		rw |= (1 << BIO_RW_DISCARD);

	if (blk_pc_request(rq)) {
		trace_mark_tp(block, rq_insert_pc, block_rq_insert,
			probe_block_rq_insert,
			"data_len %u rw %d errors %d",
			blk_rq_bytes(rq), rw, rq->errors);
	} else {
		/*
		 * FIXME Using a simple trace_mark for the second event
		 * possibility because tracepoints do not support multiple
		 * connections to the same probe yet. They should have some
		 * refcounting. Need to enable both rq_insert_pc and
		 * rq_insert_fs markers to have the rq_insert_fs marker enabled.
		 */
		trace_mark(block, rq_insert_fs,
			"hard_sector %llu "
			"rw %d errors %d", (unsigned long long)blk_rq_pos(rq),
			rw, rq->errors);
	}
}

void probe_block_rq_issue(void *data, struct request_queue *q, struct request *rq)
{
	int rw = rq->cmd_flags & 0x03;

	if (blk_discard_rq(rq))
		rw |= (1 << BIO_RW_DISCARD);

	if (blk_pc_request(rq)) {
		trace_mark_tp(block, rq_issue_pc, block_rq_issue,
			probe_block_rq_issue,
			"data_len %u rw %d errors %d",
			blk_rq_bytes(rq), rw, rq->errors);
	} else {
		/*
		 * FIXME Using a simple trace_mark for the second event
		 * possibility because tracepoints do not support multiple
		 * connections to the same probe yet. They should have some
		 * refcounting. Need to enable both rq_issue_pc and rq_issue_fs
		 * markers to have the rq_issue_fs marker enabled.
		 */
		trace_mark(block, rq_issue_fs,
			"hard_sector %llu "
			"rw %d errors %d", (unsigned long long)blk_rq_pos(rq),
			rw, rq->errors);
	}
}

void probe_block_rq_requeue(void *data, struct request_queue *q, struct request *rq)
{
	int rw = rq->cmd_flags & 0x03;

	if (blk_discard_rq(rq))
		rw |= (1 << BIO_RW_DISCARD);

	if (blk_pc_request(rq)) {
		trace_mark_tp(block, rq_requeue_pc, block_rq_requeue,
			probe_block_rq_requeue,
			"data_len %u rw %d errors %d",
			blk_rq_bytes(rq), rw, rq->errors);
	} else {
		/*
		 * FIXME Using a simple trace_mark for the second event
		 * possibility because tracepoints do not support multiple
		 * connections to the same probe yet. They should have some
		 * refcounting. Need to enable both rq_requeue_pc and
		 * rq_requeue_fs markers to have the rq_requeue_fs marker
		 * enabled.
		 */
		trace_mark(block, rq_requeue_fs,
			"hard_sector %llu "
			"rw %d errors %d", (unsigned long long)blk_rq_pos(rq),
			rw, rq->errors);
	}
}

void probe_block_rq_complete(void *data, struct request_queue *q, struct request *rq)
{
	int rw = rq->cmd_flags & 0x03;

	if (blk_discard_rq(rq))
		rw |= (1 << BIO_RW_DISCARD);

	if (blk_pc_request(rq)) {
		trace_mark_tp(block, rq_complete_pc, block_rq_complete,
			probe_block_rq_complete,
			"data_len %u rw %d errors %d",
			blk_rq_bytes(rq), rw, rq->errors);
	} else {
		/*
		 * FIXME Using a simple trace_mark for the second event
		 * possibility because tracepoints do not support multiple
		 * connections to the same probe yet. They should have some
		 * refcounting. Need to enable both rq_complete_pc and
		 * rq_complete_fs markers to have the rq_complete_fs marker
		 * enabled.
		 */
		trace_mark(block, rq_complete_fs,
			"hard_sector %llu "
			"rw %d errors %d", (unsigned long long)blk_rq_pos(rq),
			rw, rq->errors);
	}
}

void probe_block_bio_bounce(void *data, struct request_queue *q, struct bio *bio)
{
	trace_mark_tp(block, bio_bounce, block_bio_bounce,
		probe_block_bio_bounce,
		"sector %llu size %u rw(FAILFAST_DRIVER,FAILFAST_TRANSPORT,"
		"FAILFAST_DEV,DISCARD,META,SYNC,BARRIER,AHEAD,RW) %lX "
		"not_uptodate #1u%d",
		(unsigned long long)bio->bi_sector, bio->bi_size,
		bio->bi_rw, !bio_flagged(bio, BIO_UPTODATE));
}

void probe_block_bio_complete(void *data, struct request_queue *q, struct bio *bio)
{
	trace_mark_tp(block, bio_complete, block_bio_complete,
		probe_block_bio_complete,
		"sector %llu size %u rw(FAILFAST_DRIVER,FAILFAST_TRANSPORT,"
		"FAILFAST_DEV,DISCARD,META,SYNC,BARRIER,AHEAD,RW) %lX "
		"not_uptodate #1u%d",
		(unsigned long long)bio->bi_sector, bio->bi_size,
		bio->bi_rw, !bio_flagged(bio, BIO_UPTODATE));
}

void probe_block_bio_backmerge(void *data, struct request_queue *q, struct bio *bio)
{
	trace_mark_tp(block, bio_backmerge, block_bio_backmerge,
		probe_block_bio_backmerge,
		"sector %llu size %u rw(FAILFAST_DRIVER,FAILFAST_TRANSPORT,"
		"FAILFAST_DEV,DISCARD,META,SYNC,BARRIER,AHEAD,RW) %lX "
		"not_uptodate #1u%d",
		(unsigned long long)bio->bi_sector, bio->bi_size,
		bio->bi_rw, !bio_flagged(bio, BIO_UPTODATE));
}

void probe_block_bio_frontmerge(void *data, struct request_queue *q, struct bio *bio)
{
	trace_mark_tp(block, bio_frontmerge, block_bio_frontmerge,
		probe_block_bio_frontmerge,
		"sector %llu size %u rw(FAILFAST_DRIVER,FAILFAST_TRANSPORT,"
		"FAILFAST_DEV,DISCARD,META,SYNC,BARRIER,AHEAD,RW) %lX "
		"not_uptodate #1u%d",
		(unsigned long long)bio->bi_sector, bio->bi_size,
		bio->bi_rw, !bio_flagged(bio, BIO_UPTODATE));
}

void probe_block_bio_queue(void *data, struct request_queue *q, struct bio *bio)
{
	trace_mark_tp(block, bio_queue, block_bio_queue,
		probe_block_bio_queue,
		"sector %llu size %u rw(FAILFAST_DRIVER,FAILFAST_TRANSPORT,"
		"FAILFAST_DEV,DISCARD,META,SYNC,BARRIER,AHEAD,RW) %lX "
		"not_uptodate #1u%d",
		(unsigned long long)bio->bi_sector, bio->bi_size,
		bio->bi_rw, !bio_flagged(bio, BIO_UPTODATE));
}

void probe_block_getrq(void *data, struct request_queue *q, struct bio *bio, int rw)
{
	if (bio) {
		trace_mark_tp(block, getrq_bio, block_getrq,
			probe_block_getrq,
			"sector %llu size %u "
			"rw(FAILFAST_DRIVER,FAILFAST_TRANSPORT,"
			"FAILFAST_DEV,DISCARD,META,SYNC,BARRIER,AHEAD,RW) %lX "
			"not_uptodate #1u%d",
			(unsigned long long)bio->bi_sector, bio->bi_size,
			bio->bi_rw, !bio_flagged(bio, BIO_UPTODATE));
	} else {
		/*
		 * FIXME Using a simple trace_mark for the second event
		 * possibility because tracepoints do not support multiple
		 * connections to the same probe yet. They should have some
		 * refcounting. Need to enable both getrq_bio and getrq markers
		 * to have the getrq marker enabled.
		 */
		trace_mark(block, getrq, "rw %d", rw);
	}
}

void probe_block_sleeprq(void *data, struct request_queue *q, struct bio *bio, int rw)
{
	if (bio) {
		trace_mark_tp(block, sleeprq_bio, block_sleeprq,
			probe_block_sleeprq,
			"sector %llu size %u "
			"rw(FAILFAST_DRIVER,FAILFAST_TRANSPORT,"
			"FAILFAST_DEV,DISCARD,META,SYNC,BARRIER,AHEAD,RW) %lX "
			"not_uptodate #1u%d",
			(unsigned long long)bio->bi_sector, bio->bi_size,
			bio->bi_rw, !bio_flagged(bio, BIO_UPTODATE));
	} else {
		/*
		 * FIXME Using a simple trace_mark for the second event
		 * possibility because tracepoints do not support multiple
		 * connections to the same probe yet. They should have some
		 * refcounting. Need to enable both sleeprq_bio and sleeprq
		 * markers to have the sleeprq marker enabled.
		 */
		trace_mark(block, sleeprq, "rw %d", rw);
	}
}

void probe_block_plug(void *data, struct request_queue *q)
{
	trace_mark_tp(block, plug, block_plug, probe_block_plug,
			 MARK_NOARGS);
}

void probe_block_unplug_io(void *data, struct request_queue *q)
{
	unsigned int pdu = q->rq.count[READ] + q->rq.count[WRITE];

	trace_mark_tp(block, unplug_io, block_unplug_io, probe_block_unplug_io,
			"pdu %u", pdu);
}

void probe_block_unplug_timer(void *data, struct request_queue *q)
{
	unsigned int pdu = q->rq.count[READ] + q->rq.count[WRITE];

	trace_mark_tp(block, unplug_timer, block_unplug_timer,
			probe_block_unplug_timer,
			"pdu %u", pdu);
}

void probe_block_split(void *data, struct request_queue *q, struct bio *bio,
		       unsigned int pdu)
{
	trace_mark_tp(block, split, block_split,
		probe_block_split,
		"sector %llu size %u rw(FAILFAST_DRIVER,FAILFAST_TRANSPORT,"
		"FAILFAST_DEV,DISCARD,META,SYNC,BARRIER,AHEAD,RW) %lX "
		"not_uptodate #1u%d pdu %u",
		(unsigned long long)bio->bi_sector, bio->bi_size,
		bio->bi_rw, !bio_flagged(bio, BIO_UPTODATE), pdu);
}

void probe_block_remap(void *data, struct request_queue *q, struct bio *bio,
		       dev_t dev, sector_t from)
{
	trace_mark_tp(block, remap, block_remap,
		probe_block_remap,
		"device_from %lu sector_from %llu device_to %lu "
		"size %u rw(FAILFAST_DRIVER,FAILFAST_TRANSPORT,"
		"FAILFAST_DEV,DISCARD,META,SYNC,BARRIER,AHEAD,RW) %lX "
		"not_uptodate #1u%d",
		(unsigned long)bio->bi_bdev->bd_dev,
		(unsigned long long)from,
		(unsigned long)dev,
		bio->bi_size, bio->bi_rw,
		!bio_flagged(bio, BIO_UPTODATE));
}

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("Block Tracepoint Probes");
