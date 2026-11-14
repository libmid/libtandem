#ifndef TD_SCHEDULER_H
#define TD_SCHEDULER_H

#include "common.h"
#include "io.h"

void td_sch_init(td_rt *rt);
void td_sch_free(td_rt *rt);
void td_sch_run(td_rt *rt);

struct io_uring_sqe *td_sch_get_sqe(td_rt *rt);
void td_sch_wait(td_rt *rt);

#endif
