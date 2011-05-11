#ifndef _LINUX_RING_BUFFER_NOHZ_H
#define _LINUX_RING_BUFFER_NOHZ_H

#ifdef CONFIG_LIB_RING_BUFFER
void lib_ring_buffer_tick_nohz_flush(void);
void lib_ring_buffer_tick_nohz_stop(void);
void lib_ring_buffer_tick_nohz_restart(void);
#else
static inline void lib_ring_buffer_tick_nohz_flush(void)
{
}

static inline void lib_ring_buffer_tick_nohz_stop(void)
{
}

static inline void lib_ring_buffer_tick_nohz_restart(void)
{
}
#endif

#endif /* _LINUX_RING_BUFFER_NOHZ_H */
