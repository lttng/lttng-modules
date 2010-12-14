/* Type list, used to create metadata */

TRACE_EVENT_TYPE(long, integer)
TRACE_EVENT_TYPE(unsigned long, integer)
TRACE_EVENT_TYPE(int, integer)
TRACE_EVENT_TYPE(unsigned int, integer)
TRACE_EVENT_TYPE(short, integer)
TRACE_EVENT_TYPE(unsigned short, integer)
TRACE_EVENT_TYPE(char, integer)
TRACE_EVENT_TYPE(signed char, integer)
TRACE_EVENT_TYPE(unsigned char, integer)
TRACE_EVENT_TYPE(bool, integer)
TRACE_EVENT_TYPE(size_t, integer)
TRACE_EVENT_TYPE(ssize_t, integer)
TRACE_EVENT_TYPE(loff_t, integer)
TRACE_EVENT_TYPE(u64, integer)
TRACE_EVENT_TYPE(u32, integer)
TRACE_EVENT_TYPE(u16, integer)
TRACE_EVENT_TYPE(u8, integer)
TRACE_EVENT_TYPE(s64, integer)
TRACE_EVENT_TYPE(s32, integer)
TRACE_EVENT_TYPE(s16, integer)
TRACE_EVENT_TYPE(s8, integer)
TRACE_EVENT_TYPE(void *, integer)

/* Kernel-specific types */
TRACE_EVENT_TYPE(pid_t, integer)
TRACE_EVENT_TYPE(tid_t, integer)
TRACE_EVENT_TYPE(uid_t, integer)
TRACE_EVENT_TYPE(gid_t, integer)
TRACE_EVENT_TYPE(ino_t, integer)
TRACE_EVENT_TYPE(sector_t, integer)
TRACE_EVENT_TYPE(blkcnt_t, integer)
TRACE_EVENT_TYPE(pgoff_t, integer)
TRACE_EVENT_TYPE(gfp_t, integer)
TRACE_EVENT_TYPE(dev_t, integer)
TRACE_EVENT_TYPE(umode_t, integer)
TRACE_EVENT_TYPE(clockid_t, integer)
TRACE_EVENT_TYPE(cputime_t, integer)

/* Aliases needed by kernel instrumentation */
TRACE_EVENT_TYPE(struct page *, integer)
TRACE_EVENT_TYPE(unsigned, integer)
TRACE_EVENT_TYPE(__u32, integer)
TRACE_EVENT_TYPE(__u16, integer)

/* Arrays */
TRACE_EVENT_TYPE(task_comm, array, char, TASK_COMM_LEN)

#include <linux/hrtimer.h>
/* Enumerations */
TRACE_EVENT_ENUM(hrtimer_mode,
        V(HRTIMER_MODE_ABS),
        V(HRTIMER_MODE_REL),
        V(HRTIMER_MODE_PINNED),
        V(HRTIMER_MODE_ABS_PINNED),
        V(HRTIMER_MODE_REL_PINNED),
	R(HRTIMER_MODE_UNDEFINED, 0x04, 0x20),	/* Example (to remove) */
)

TRACE_EVENT_TYPE(hrtimer_mode, enum, unsigned char)

/* Tests */

TRACE_EVENT_TYPE(__be32, integer_ext, __BIG_ENDIAN)
TRACE_EVENT_TYPE(testseq, sequence, unsigned int, size_t)
TRACE_EVENT_TYPE(teststring, string, UTF8)
