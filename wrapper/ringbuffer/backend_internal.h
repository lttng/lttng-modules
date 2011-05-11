#ifdef CONFIG_LIB_RING_BUFFER
#include <linux/ringbuffer/backend_internal.h>
#else
#include "../../wrapper/inline_memcpy.h"
#include "../../lib/ringbuffer/backend_internal.h"
#endif
