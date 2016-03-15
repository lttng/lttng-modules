#undef TRACE_SYSTEM
#define TRACE_SYSTEM jbd

#if !defined(LTTNG_TRACE_JBD_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_JBD_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/jbd.h>
#include <linux/version.h>

LTTNG_TRACEPOINT_EVENT(jbd_checkpoint,

	TP_PROTO(journal_t *journal, int result),

	TP_ARGS(journal, result),

	TP_FIELDS(
		ctf_integer(dev_t, dev, journal->j_fs_dev->bd_dev)
		ctf_integer(int, result, result)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(jbd_commit,

	TP_PROTO(journal_t *journal, transaction_t *commit_transaction),

	TP_ARGS(journal, commit_transaction),

	TP_FIELDS(
		ctf_integer(dev_t, dev, journal->j_fs_dev->bd_dev)
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0))
		ctf_integer(char, sync_commit, commit_transaction->t_synchronous_commit)
#endif
		ctf_integer(int, transaction, commit_transaction->t_tid)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(jbd_commit, jbd_start_commit,

	TP_PROTO(journal_t *journal, transaction_t *commit_transaction),

	TP_ARGS(journal, commit_transaction)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(jbd_commit, jbd_commit_locking,

	TP_PROTO(journal_t *journal, transaction_t *commit_transaction),

	TP_ARGS(journal, commit_transaction)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(jbd_commit, jbd_commit_flushing,

	TP_PROTO(journal_t *journal, transaction_t *commit_transaction),

	TP_ARGS(journal, commit_transaction)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(jbd_commit, jbd_commit_logging,

	TP_PROTO(journal_t *journal, transaction_t *commit_transaction),

	TP_ARGS(journal, commit_transaction)
)

LTTNG_TRACEPOINT_EVENT(jbd_drop_transaction,

	TP_PROTO(journal_t *journal, transaction_t *commit_transaction),

	TP_ARGS(journal, commit_transaction),

	TP_FIELDS(
		ctf_integer(dev_t, dev, journal->j_fs_dev->bd_dev)
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0))
		ctf_integer(char, sync_commit, commit_transaction->t_synchronous_commit)
#endif
		ctf_integer(int, transaction, commit_transaction->t_tid)
	)
)

LTTNG_TRACEPOINT_EVENT(jbd_end_commit,
	TP_PROTO(journal_t *journal, transaction_t *commit_transaction),

	TP_ARGS(journal, commit_transaction),

	TP_FIELDS(
		ctf_integer(dev_t, dev, journal->j_fs_dev->bd_dev)
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0))
		ctf_integer(char, sync_commit, commit_transaction->t_synchronous_commit)
#endif
		ctf_integer(int, transaction, commit_transaction->t_tid)
		ctf_integer(int, head, journal->j_tail_sequence)
	)
)

LTTNG_TRACEPOINT_EVENT(jbd_do_submit_data,
	TP_PROTO(journal_t *journal, transaction_t *commit_transaction),

	TP_ARGS(journal, commit_transaction),

	TP_FIELDS(
		ctf_integer(dev_t, dev, journal->j_fs_dev->bd_dev)
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0))
		ctf_integer(char, sync_commit, commit_transaction->t_synchronous_commit)
#endif
		ctf_integer(int, transaction, commit_transaction->t_tid)
	)
)

LTTNG_TRACEPOINT_EVENT(jbd_cleanup_journal_tail,

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

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,5,0))
LTTNG_TRACEPOINT_EVENT_MAP(journal_write_superblock,

	jbd_journal_write_superblock,

	TP_PROTO(journal_t *journal, int write_op),

	TP_ARGS(journal, write_op),

	TP_FIELDS(
		ctf_integer(dev_t, dev, journal->j_fs_dev->bd_dev)
		ctf_integer(int, write_op, write_op)
	)
)
#else
LTTNG_TRACEPOINT_EVENT(jbd_update_superblock_end,
	TP_PROTO(journal_t *journal, int wait),

	TP_ARGS(journal, wait),

	TP_FIELDS(
		ctf_integer(dev_t, dev, journal->j_fs_dev->bd_dev)
		ctf_integer(int, wait, wait)
	)
)
#endif

#endif /* LTTNG_TRACE_JBD_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
