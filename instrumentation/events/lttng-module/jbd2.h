#undef TRACE_SYSTEM
#define TRACE_SYSTEM jbd2

#if !defined(LTTNG_TRACE_JBD2_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_JBD2_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/jbd2.h>
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
#ifndef _TRACE_JBD2_DEF
#define _TRACE_JBD2_DEF
struct transaction_chp_stats_s;
struct transaction_run_stats_s;
#endif
#endif

LTTNG_TRACEPOINT_EVENT(jbd2_checkpoint,

	TP_PROTO(journal_t *journal, int result),

	TP_ARGS(journal, result),

	TP_FIELDS(
		ctf_integer(dev_t, dev, journal->j_fs_dev->bd_dev)
		ctf_integer(int, result, result)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(jbd2_commit,

	TP_PROTO(journal_t *journal, transaction_t *commit_transaction),

	TP_ARGS(journal, commit_transaction),

	TP_FIELDS(
		ctf_integer(dev_t, dev, journal->j_fs_dev->bd_dev)
		ctf_integer(char, sync_commit, commit_transaction->t_synchronous_commit)
		ctf_integer(int, transaction, commit_transaction->t_tid)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(jbd2_commit, jbd2_start_commit,

	TP_PROTO(journal_t *journal, transaction_t *commit_transaction),

	TP_ARGS(journal, commit_transaction)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(jbd2_commit, jbd2_commit_locking,

	TP_PROTO(journal_t *journal, transaction_t *commit_transaction),

	TP_ARGS(journal, commit_transaction)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(jbd2_commit, jbd2_commit_flushing,

	TP_PROTO(journal_t *journal, transaction_t *commit_transaction),

	TP_ARGS(journal, commit_transaction)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(jbd2_commit, jbd2_commit_logging,

	TP_PROTO(journal_t *journal, transaction_t *commit_transaction),

	TP_ARGS(journal, commit_transaction)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
LTTNG_TRACEPOINT_EVENT_INSTANCE(jbd2_commit, jbd2_drop_transaction,

	TP_PROTO(journal_t *journal, transaction_t *commit_transaction),

	TP_ARGS(journal, commit_transaction)
)
#endif

LTTNG_TRACEPOINT_EVENT(jbd2_end_commit,
	TP_PROTO(journal_t *journal, transaction_t *commit_transaction),

	TP_ARGS(journal, commit_transaction),

	TP_FIELDS(
		ctf_integer(dev_t, dev, journal->j_fs_dev->bd_dev)
		ctf_integer(char, sync_commit, commit_transaction->t_synchronous_commit)
		ctf_integer(int, transaction, commit_transaction->t_tid)
		ctf_integer(int, head, journal->j_tail_sequence)
	)
)

LTTNG_TRACEPOINT_EVENT(jbd2_submit_inode_data,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
	)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
LTTNG_TRACEPOINT_EVENT(jbd2_run_stats,
	TP_PROTO(dev_t dev, unsigned long tid,
		 struct transaction_run_stats_s *stats),

	TP_ARGS(dev, tid, stats),

	TP_FIELDS(
		ctf_integer(dev_t, dev, dev)
		ctf_integer(unsigned long, tid, tid)
		ctf_integer(unsigned long, wait, stats->rs_wait)
		ctf_integer(unsigned long, running, stats->rs_running)
		ctf_integer(unsigned long, locked, stats->rs_locked)
		ctf_integer(unsigned long, flushing, stats->rs_flushing)
		ctf_integer(unsigned long, logging, stats->rs_logging)
		ctf_integer(__u32, handle_count, stats->rs_handle_count)
		ctf_integer(__u32, blocks, stats->rs_blocks)
		ctf_integer(__u32, blocks_logged, stats->rs_blocks_logged)
	)
)

LTTNG_TRACEPOINT_EVENT(jbd2_checkpoint_stats,
	TP_PROTO(dev_t dev, unsigned long tid,
		 struct transaction_chp_stats_s *stats),

	TP_ARGS(dev, tid, stats),

	TP_FIELDS(
		ctf_integer(dev_t, dev, dev)
		ctf_integer(unsigned long, tid, tid)
		ctf_integer(unsigned long, chp_time, stats->cs_chp_time)
		ctf_integer(__u32, forced_to_close, stats->cs_forced_to_close)
		ctf_integer(__u32, written, stats->cs_written)
		ctf_integer(__u32, dropped, stats->cs_dropped)
	)
)
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34))
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
LTTNG_TRACEPOINT_EVENT(jbd2_update_log_tail,
#else
LTTNG_TRACEPOINT_EVENT(jbd2_cleanup_journal_tail,
#endif

	TP_PROTO(journal_t *journal, tid_t first_tid,
		 unsigned long block_nr, unsigned long freed),

	TP_ARGS(journal, first_tid, block_nr, freed),

	TP_FIELDS(
		ctf_integer(dev_t, dev, journal->j_fs_dev->bd_dev)
		ctf_integer(tid_t, tail_sequence, journal->j_tail_sequence)
		ctf_integer(tid_t, first_tid, first_tid)
		ctf_integer(unsigned long, block_nr, block_nr)
		ctf_integer(unsigned long, freed, freed)
	)
)
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
LTTNG_TRACEPOINT_EVENT(jbd2_write_superblock,

	TP_PROTO(journal_t *journal, int write_op),

	TP_ARGS(journal, write_op),

	TP_FIELDS(
		ctf_integer(dev_t, dev, journal->j_fs_dev->bd_dev)
		ctf_integer(int, write_op, write_op)
	)
)
#endif

#endif /* LTTNG_TRACE_JBD2_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
