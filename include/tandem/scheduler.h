#ifndef TD_SCHEDULER_H
#define TD_SCHEDULER_H

#include "common.h"
#include "io.h"

#ifndef TD_DEFAULT_RING_SIZE
#define TD_DEFAULT_RING_SIZE 256
#endif

// Initialze the runtime with a scheduler right after creating it.
TD_API void td_sch_init(td_rt *rt);
TD_API void td_sch_init_opts(td_rt *rt, unsigned int ring_size, unsigned int flags);

// After you create a bunch of coroutines using `td_spawm`,
// call this to run them in the default scheduler.
TD_API void td_sch_run(td_rt *rt);

TD_API void td_sch_free(td_rt *rt);

struct io_uring_sqe *td_sch_get_sqe(td_rt *rt);
void td_sch_wait(td_rt *rt);

#endif
