#ifndef CONFIG_LIB_RING_BUFFER
#include <linux/poll.h>

#warning "poll_wait_set_exclusive() is defined as no-op. Will increase LTTng overhead. Please consider using the LTTng kernel tree for better results."

/*
 * Will cause higher overhead when signalling all possible reader threads when a
 * buffer is ready to be consumed.
 */
#define poll_wait_set_exclusive(poll_table)

#endif
