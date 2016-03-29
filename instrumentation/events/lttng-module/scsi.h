#undef TRACE_SYSTEM
#define TRACE_SYSTEM scsi

#if !defined(LTTNG_TRACE_SCSI_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_SCSI_H

#include <probes/lttng-tracepoint-event.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_host.h>
#include <linux/trace_seq.h>
#include <linux/version.h>

#ifndef _TRACE_SCSI_DEF
#define _TRACE_SCSI_DEF

#define scsi_opcode_name(opcode)	{ opcode, #opcode }

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0) \
	|| LTTNG_RHEL_KERNEL_RANGE(3,10,0,327,0,0, 3,11,0,0,0,0))

#define show_opcode_name(val)					\
	__print_symbolic(val,					\
		scsi_opcode_name(TEST_UNIT_READY),		\
		scsi_opcode_name(REZERO_UNIT),			\
		scsi_opcode_name(REQUEST_SENSE),		\
		scsi_opcode_name(FORMAT_UNIT),			\
		scsi_opcode_name(READ_BLOCK_LIMITS),		\
		scsi_opcode_name(REASSIGN_BLOCKS),		\
		scsi_opcode_name(INITIALIZE_ELEMENT_STATUS),	\
		scsi_opcode_name(READ_6),			\
		scsi_opcode_name(WRITE_6),			\
		scsi_opcode_name(SEEK_6),			\
		scsi_opcode_name(READ_REVERSE),			\
		scsi_opcode_name(WRITE_FILEMARKS),		\
		scsi_opcode_name(SPACE),			\
		scsi_opcode_name(INQUIRY),			\
		scsi_opcode_name(RECOVER_BUFFERED_DATA),	\
		scsi_opcode_name(MODE_SELECT),			\
		scsi_opcode_name(RESERVE),			\
		scsi_opcode_name(RELEASE),			\
		scsi_opcode_name(COPY),				\
		scsi_opcode_name(ERASE),			\
		scsi_opcode_name(MODE_SENSE),			\
		scsi_opcode_name(START_STOP),			\
		scsi_opcode_name(RECEIVE_DIAGNOSTIC),		\
		scsi_opcode_name(SEND_DIAGNOSTIC),		\
		scsi_opcode_name(ALLOW_MEDIUM_REMOVAL),		\
		scsi_opcode_name(SET_WINDOW),			\
		scsi_opcode_name(READ_CAPACITY),		\
		scsi_opcode_name(READ_10),			\
		scsi_opcode_name(WRITE_10),			\
		scsi_opcode_name(SEEK_10),			\
		scsi_opcode_name(POSITION_TO_ELEMENT),		\
		scsi_opcode_name(WRITE_VERIFY),			\
		scsi_opcode_name(VERIFY),			\
		scsi_opcode_name(SEARCH_HIGH),			\
		scsi_opcode_name(SEARCH_EQUAL),			\
		scsi_opcode_name(SEARCH_LOW),			\
		scsi_opcode_name(SET_LIMITS),			\
		scsi_opcode_name(PRE_FETCH),			\
		scsi_opcode_name(READ_POSITION),		\
		scsi_opcode_name(SYNCHRONIZE_CACHE),		\
		scsi_opcode_name(LOCK_UNLOCK_CACHE),		\
		scsi_opcode_name(READ_DEFECT_DATA),		\
		scsi_opcode_name(MEDIUM_SCAN),			\
		scsi_opcode_name(COMPARE),			\
		scsi_opcode_name(COPY_VERIFY),			\
		scsi_opcode_name(WRITE_BUFFER),			\
		scsi_opcode_name(READ_BUFFER),			\
		scsi_opcode_name(UPDATE_BLOCK),			\
		scsi_opcode_name(READ_LONG),			\
		scsi_opcode_name(WRITE_LONG),			\
		scsi_opcode_name(CHANGE_DEFINITION),		\
		scsi_opcode_name(WRITE_SAME),			\
		scsi_opcode_name(UNMAP),			\
		scsi_opcode_name(READ_TOC),			\
		scsi_opcode_name(LOG_SELECT),			\
		scsi_opcode_name(LOG_SENSE),			\
		scsi_opcode_name(XDWRITEREAD_10),		\
		scsi_opcode_name(MODE_SELECT_10),		\
		scsi_opcode_name(RESERVE_10),			\
		scsi_opcode_name(RELEASE_10),			\
		scsi_opcode_name(MODE_SENSE_10),		\
		scsi_opcode_name(PERSISTENT_RESERVE_IN),	\
		scsi_opcode_name(PERSISTENT_RESERVE_OUT),	\
		scsi_opcode_name(VARIABLE_LENGTH_CMD),		\
		scsi_opcode_name(REPORT_LUNS),			\
		scsi_opcode_name(MAINTENANCE_IN),		\
		scsi_opcode_name(MAINTENANCE_OUT),		\
		scsi_opcode_name(MOVE_MEDIUM),			\
		scsi_opcode_name(EXCHANGE_MEDIUM),		\
		scsi_opcode_name(READ_12),			\
		scsi_opcode_name(WRITE_12),			\
		scsi_opcode_name(WRITE_VERIFY_12),		\
		scsi_opcode_name(SEARCH_HIGH_12),		\
		scsi_opcode_name(SEARCH_EQUAL_12),		\
		scsi_opcode_name(SEARCH_LOW_12),		\
		scsi_opcode_name(READ_ELEMENT_STATUS),		\
		scsi_opcode_name(SEND_VOLUME_TAG),		\
		scsi_opcode_name(WRITE_LONG_2),			\
		scsi_opcode_name(READ_16),			\
		scsi_opcode_name(WRITE_16),			\
		scsi_opcode_name(VERIFY_16),			\
		scsi_opcode_name(WRITE_SAME_16),		\
		scsi_opcode_name(SERVICE_ACTION_IN_16),		\
		scsi_opcode_name(SAI_READ_CAPACITY_16),		\
		scsi_opcode_name(SAI_GET_LBA_STATUS),		\
		scsi_opcode_name(MI_REPORT_TARGET_PGS),		\
		scsi_opcode_name(MO_SET_TARGET_PGS),		\
		scsi_opcode_name(READ_32),			\
		scsi_opcode_name(WRITE_32),			\
		scsi_opcode_name(WRITE_SAME_32),		\
		scsi_opcode_name(ATA_16),			\
		scsi_opcode_name(ATA_12))

#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0)) */

#define show_opcode_name(val)					\
	__print_symbolic(val,					\
		scsi_opcode_name(TEST_UNIT_READY),		\
		scsi_opcode_name(REZERO_UNIT),			\
		scsi_opcode_name(REQUEST_SENSE),		\
		scsi_opcode_name(FORMAT_UNIT),			\
		scsi_opcode_name(READ_BLOCK_LIMITS),		\
		scsi_opcode_name(REASSIGN_BLOCKS),		\
		scsi_opcode_name(INITIALIZE_ELEMENT_STATUS),	\
		scsi_opcode_name(READ_6),			\
		scsi_opcode_name(WRITE_6),			\
		scsi_opcode_name(SEEK_6),			\
		scsi_opcode_name(READ_REVERSE),			\
		scsi_opcode_name(WRITE_FILEMARKS),		\
		scsi_opcode_name(SPACE),			\
		scsi_opcode_name(INQUIRY),			\
		scsi_opcode_name(RECOVER_BUFFERED_DATA),	\
		scsi_opcode_name(MODE_SELECT),			\
		scsi_opcode_name(RESERVE),			\
		scsi_opcode_name(RELEASE),			\
		scsi_opcode_name(COPY),				\
		scsi_opcode_name(ERASE),			\
		scsi_opcode_name(MODE_SENSE),			\
		scsi_opcode_name(START_STOP),			\
		scsi_opcode_name(RECEIVE_DIAGNOSTIC),		\
		scsi_opcode_name(SEND_DIAGNOSTIC),		\
		scsi_opcode_name(ALLOW_MEDIUM_REMOVAL),		\
		scsi_opcode_name(SET_WINDOW),			\
		scsi_opcode_name(READ_CAPACITY),		\
		scsi_opcode_name(READ_10),			\
		scsi_opcode_name(WRITE_10),			\
		scsi_opcode_name(SEEK_10),			\
		scsi_opcode_name(POSITION_TO_ELEMENT),		\
		scsi_opcode_name(WRITE_VERIFY),			\
		scsi_opcode_name(VERIFY),			\
		scsi_opcode_name(SEARCH_HIGH),			\
		scsi_opcode_name(SEARCH_EQUAL),			\
		scsi_opcode_name(SEARCH_LOW),			\
		scsi_opcode_name(SET_LIMITS),			\
		scsi_opcode_name(PRE_FETCH),			\
		scsi_opcode_name(READ_POSITION),		\
		scsi_opcode_name(SYNCHRONIZE_CACHE),		\
		scsi_opcode_name(LOCK_UNLOCK_CACHE),		\
		scsi_opcode_name(READ_DEFECT_DATA),		\
		scsi_opcode_name(MEDIUM_SCAN),			\
		scsi_opcode_name(COMPARE),			\
		scsi_opcode_name(COPY_VERIFY),			\
		scsi_opcode_name(WRITE_BUFFER),			\
		scsi_opcode_name(READ_BUFFER),			\
		scsi_opcode_name(UPDATE_BLOCK),			\
		scsi_opcode_name(READ_LONG),			\
		scsi_opcode_name(WRITE_LONG),			\
		scsi_opcode_name(CHANGE_DEFINITION),		\
		scsi_opcode_name(WRITE_SAME),			\
		scsi_opcode_name(UNMAP),			\
		scsi_opcode_name(READ_TOC),			\
		scsi_opcode_name(LOG_SELECT),			\
		scsi_opcode_name(LOG_SENSE),			\
		scsi_opcode_name(XDWRITEREAD_10),		\
		scsi_opcode_name(MODE_SELECT_10),		\
		scsi_opcode_name(RESERVE_10),			\
		scsi_opcode_name(RELEASE_10),			\
		scsi_opcode_name(MODE_SENSE_10),		\
		scsi_opcode_name(PERSISTENT_RESERVE_IN),	\
		scsi_opcode_name(PERSISTENT_RESERVE_OUT),	\
		scsi_opcode_name(VARIABLE_LENGTH_CMD),		\
		scsi_opcode_name(REPORT_LUNS),			\
		scsi_opcode_name(MAINTENANCE_IN),		\
		scsi_opcode_name(MAINTENANCE_OUT),		\
		scsi_opcode_name(MOVE_MEDIUM),			\
		scsi_opcode_name(EXCHANGE_MEDIUM),		\
		scsi_opcode_name(READ_12),			\
		scsi_opcode_name(WRITE_12),			\
		scsi_opcode_name(WRITE_VERIFY_12),		\
		scsi_opcode_name(SEARCH_HIGH_12),		\
		scsi_opcode_name(SEARCH_EQUAL_12),		\
		scsi_opcode_name(SEARCH_LOW_12),		\
		scsi_opcode_name(READ_ELEMENT_STATUS),		\
		scsi_opcode_name(SEND_VOLUME_TAG),		\
		scsi_opcode_name(WRITE_LONG_2),			\
		scsi_opcode_name(READ_16),			\
		scsi_opcode_name(WRITE_16),			\
		scsi_opcode_name(VERIFY_16),			\
		scsi_opcode_name(WRITE_SAME_16),		\
		scsi_opcode_name(SERVICE_ACTION_IN),		\
		scsi_opcode_name(SAI_READ_CAPACITY_16),		\
		scsi_opcode_name(SAI_GET_LBA_STATUS),		\
		scsi_opcode_name(MI_REPORT_TARGET_PGS),		\
		scsi_opcode_name(MO_SET_TARGET_PGS),		\
		scsi_opcode_name(READ_32),			\
		scsi_opcode_name(WRITE_32),			\
		scsi_opcode_name(WRITE_SAME_32),		\
		scsi_opcode_name(ATA_16),			\
		scsi_opcode_name(ATA_12))

#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0)) */

#define scsi_hostbyte_name(result)	{ result, #result }
#define show_hostbyte_name(val)					\
	__print_symbolic(val,					\
		scsi_hostbyte_name(DID_OK),			\
		scsi_hostbyte_name(DID_NO_CONNECT),		\
		scsi_hostbyte_name(DID_BUS_BUSY),		\
		scsi_hostbyte_name(DID_TIME_OUT),		\
		scsi_hostbyte_name(DID_BAD_TARGET),		\
		scsi_hostbyte_name(DID_ABORT),			\
		scsi_hostbyte_name(DID_PARITY),			\
		scsi_hostbyte_name(DID_ERROR),			\
		scsi_hostbyte_name(DID_RESET),			\
		scsi_hostbyte_name(DID_BAD_INTR),		\
		scsi_hostbyte_name(DID_PASSTHROUGH),		\
		scsi_hostbyte_name(DID_SOFT_ERROR),		\
		scsi_hostbyte_name(DID_IMM_RETRY),		\
		scsi_hostbyte_name(DID_REQUEUE),		\
		scsi_hostbyte_name(DID_TRANSPORT_DISRUPTED),	\
		scsi_hostbyte_name(DID_TRANSPORT_FAILFAST))

#define scsi_driverbyte_name(result)	{ result, #result }
#define show_driverbyte_name(val)				\
	__print_symbolic(val,					\
		scsi_driverbyte_name(DRIVER_OK),		\
		scsi_driverbyte_name(DRIVER_BUSY),		\
		scsi_driverbyte_name(DRIVER_SOFT),		\
		scsi_driverbyte_name(DRIVER_MEDIA),		\
		scsi_driverbyte_name(DRIVER_ERROR),		\
		scsi_driverbyte_name(DRIVER_INVALID),		\
		scsi_driverbyte_name(DRIVER_TIMEOUT),		\
		scsi_driverbyte_name(DRIVER_HARD),		\
		scsi_driverbyte_name(DRIVER_SENSE))

#define scsi_msgbyte_name(result)	{ result, #result }
#define show_msgbyte_name(val)					\
	__print_symbolic(val,					\
		scsi_msgbyte_name(COMMAND_COMPLETE),		\
		scsi_msgbyte_name(EXTENDED_MESSAGE),		\
		scsi_msgbyte_name(SAVE_POINTERS),		\
		scsi_msgbyte_name(RESTORE_POINTERS),		\
		scsi_msgbyte_name(DISCONNECT),			\
		scsi_msgbyte_name(INITIATOR_ERROR),		\
		scsi_msgbyte_name(ABORT_TASK_SET),		\
		scsi_msgbyte_name(MESSAGE_REJECT),		\
		scsi_msgbyte_name(NOP),				\
		scsi_msgbyte_name(MSG_PARITY_ERROR),		\
		scsi_msgbyte_name(LINKED_CMD_COMPLETE),		\
		scsi_msgbyte_name(LINKED_FLG_CMD_COMPLETE),	\
		scsi_msgbyte_name(TARGET_RESET),		\
		scsi_msgbyte_name(ABORT_TASK),			\
		scsi_msgbyte_name(CLEAR_TASK_SET),		\
		scsi_msgbyte_name(INITIATE_RECOVERY),		\
		scsi_msgbyte_name(RELEASE_RECOVERY),		\
		scsi_msgbyte_name(CLEAR_ACA),			\
		scsi_msgbyte_name(LOGICAL_UNIT_RESET),		\
		scsi_msgbyte_name(SIMPLE_QUEUE_TAG),		\
		scsi_msgbyte_name(HEAD_OF_QUEUE_TAG),		\
		scsi_msgbyte_name(ORDERED_QUEUE_TAG),		\
		scsi_msgbyte_name(IGNORE_WIDE_RESIDUE),		\
		scsi_msgbyte_name(ACA),				\
		scsi_msgbyte_name(QAS_REQUEST),			\
		scsi_msgbyte_name(BUS_DEVICE_RESET),		\
		scsi_msgbyte_name(ABORT))

#define scsi_statusbyte_name(result)	{ result, #result }
#define show_statusbyte_name(val)				\
	__print_symbolic(val,					\
		scsi_statusbyte_name(SAM_STAT_GOOD),		\
		scsi_statusbyte_name(SAM_STAT_CHECK_CONDITION),	\
		scsi_statusbyte_name(SAM_STAT_CONDITION_MET),	\
		scsi_statusbyte_name(SAM_STAT_BUSY),		\
		scsi_statusbyte_name(SAM_STAT_INTERMEDIATE),	\
		scsi_statusbyte_name(SAM_STAT_INTERMEDIATE_CONDITION_MET), \
		scsi_statusbyte_name(SAM_STAT_RESERVATION_CONFLICT),	\
		scsi_statusbyte_name(SAM_STAT_COMMAND_TERMINATED),	\
		scsi_statusbyte_name(SAM_STAT_TASK_SET_FULL),	\
		scsi_statusbyte_name(SAM_STAT_ACA_ACTIVE),	\
		scsi_statusbyte_name(SAM_STAT_TASK_ABORTED))

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39))
#define scsi_prot_op_name(result)	{ result, #result }
#define show_prot_op_name(val)					\
	__print_symbolic(val,					\
		scsi_prot_op_name(SCSI_PROT_NORMAL),		\
		scsi_prot_op_name(SCSI_PROT_READ_INSERT),	\
		scsi_prot_op_name(SCSI_PROT_WRITE_STRIP),	\
		scsi_prot_op_name(SCSI_PROT_READ_STRIP),	\
		scsi_prot_op_name(SCSI_PROT_WRITE_INSERT),	\
		scsi_prot_op_name(SCSI_PROT_READ_PASS),		\
		scsi_prot_op_name(SCSI_PROT_WRITE_PASS))
#endif

const char *scsi_trace_parse_cdb(struct trace_seq*, unsigned char*, int);
#define __parse_cdb(cdb, len) scsi_trace_parse_cdb(p, cdb, len)
#endif

LTTNG_TRACEPOINT_EVENT(scsi_dispatch_cmd_start,

	TP_PROTO(struct scsi_cmnd *cmd),

	TP_ARGS(cmd),

	TP_FIELDS(
		ctf_integer(unsigned int, host_no, cmd->device->host->host_no)
		ctf_integer(unsigned int, channel, cmd->device->channel)
		ctf_integer(unsigned int, id, cmd->device->id)
		ctf_integer(unsigned int, lun, cmd->device->lun)
		ctf_integer(unsigned int, opcode, cmd->cmnd[0])
		ctf_integer(unsigned int, cmd_len, cmd->cmd_len)
		ctf_integer(unsigned int, data_sglen, scsi_sg_count(cmd))
		ctf_integer(unsigned int, prot_sglen, scsi_prot_sg_count(cmd))
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39))
		ctf_integer(unsigned char, prot_op, scsi_get_prot_op(cmd))
#endif
		ctf_sequence_hex(unsigned char, cmnd, cmd->cmnd, u32, cmd->cmd_len)
	)
)

LTTNG_TRACEPOINT_EVENT(scsi_dispatch_cmd_error,

	TP_PROTO(struct scsi_cmnd *cmd, int rtn),

	TP_ARGS(cmd, rtn),

	TP_FIELDS(
		ctf_integer(unsigned int, host_no, cmd->device->host->host_no)
		ctf_integer(unsigned int, channel, cmd->device->channel)
		ctf_integer(unsigned int, id, cmd->device->id)
		ctf_integer(unsigned int, lun, cmd->device->lun)
		ctf_integer(int, rtn, rtn)
		ctf_integer(unsigned int, opcode, cmd->cmnd[0])
		ctf_integer(unsigned int, cmd_len, cmd->cmd_len)
		ctf_integer(unsigned int, data_sglen, scsi_sg_count(cmd))
		ctf_integer(unsigned int, prot_sglen, scsi_prot_sg_count(cmd))
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39))
		ctf_integer(unsigned char, prot_op, scsi_get_prot_op(cmd))
#endif
		ctf_sequence_hex(unsigned char, cmnd, cmd->cmnd, u32, cmd->cmd_len)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(scsi_cmd_done_timeout_template,

	TP_PROTO(struct scsi_cmnd *cmd),

	TP_ARGS(cmd),

	TP_FIELDS(
		ctf_integer(unsigned int, host_no, cmd->device->host->host_no)
		ctf_integer(unsigned int, channel, cmd->device->channel)
		ctf_integer(unsigned int, id, cmd->device->id)
		ctf_integer(unsigned int, lun, cmd->device->lun)
		ctf_integer(int, result, cmd->result)
		ctf_integer(unsigned int, opcode, cmd->cmnd[0])
		ctf_integer(unsigned int, cmd_len, cmd->cmd_len)
		ctf_integer(unsigned int, data_sglen, scsi_sg_count(cmd))
		ctf_integer(unsigned int, prot_sglen, scsi_prot_sg_count(cmd))
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39))
		ctf_integer(unsigned char, prot_op, scsi_get_prot_op(cmd))
#endif
		ctf_sequence_hex(unsigned char, cmnd, cmd->cmnd, u32, cmd->cmd_len)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(scsi_cmd_done_timeout_template, scsi_dispatch_cmd_done,
	     TP_PROTO(struct scsi_cmnd *cmd),
	     TP_ARGS(cmd))

LTTNG_TRACEPOINT_EVENT_INSTANCE(scsi_cmd_done_timeout_template, scsi_dispatch_cmd_timeout,
	     TP_PROTO(struct scsi_cmnd *cmd),
	     TP_ARGS(cmd))

LTTNG_TRACEPOINT_EVENT(scsi_eh_wakeup,

	TP_PROTO(struct Scsi_Host *shost),

	TP_ARGS(shost),

	TP_FIELDS(
		ctf_integer(unsigned int, host_no, shost->host_no)
	)
)

#endif /*  LTTNG_TRACE_SCSI_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
