#include <assert.h>
#include <liburing.h>
#include <stdlib.h>

#include <tandem/common.h>
#include <tandem/coro.h>
#include <tandem/queue.h>
#include <tandem/scheduler.h>

char *td_status_to_str(td_coro_status status) {
  switch (status) {
  case TD_CORO_FREE:
    return "TD_CORO_FREE";
  case TD_CORO_RUNNING:
    return "TD_CORO_RUNNING";
  case TD_CORO_WAITING_ON_IO:
    return "TD_CORO_WAITING_ON_IO";
  case TD_CORO_FINISHED:
    return "TD_CORO_FINISHED";
  case TD_CORO_DEAD:
    return "TD_CORO_DEAD";
  }
  return "TD_CORO_UNKNOWN";
}

struct io_uring_sqe *td_sch_get_sqe(td_rt *rt) {
  assert(rt->sch != NULL);
  struct io_uring_sqe *sqe = io_uring_get_sqe(&rt->sch->ring);
  if (!sqe) {
    io_uring_submit(&rt->sch->ring);
    sqe = io_uring_get_sqe(&rt->sch->ring);
    assert(sqe != NULL);
  }
  return sqe;
}

void td_sch_wait(td_rt *rt) {
  rt->current->status = TD_CORO_WAITING_ON_IO;
  td_suspend(rt);
  rt->current->status = TD_CORO_RUNNING;
}

void td_sch_run(td_rt *rt) {
  bool all_waiting;
  while (rt->running.head && rt->running.head->next) {
    do {
      all_waiting = true;
      td_coro *curr = rt->running.head->next;
      while (curr) {
        td_coro *next = curr->next;
        if (curr->status == TD_CORO_RUNNING) {
          td_resume(rt, curr);
          if (curr->status == TD_CORO_RUNNING) {
            all_waiting = false;
          }
        }
        curr = next;
      }
    } while (!all_waiting && rt->running.head && rt->running.head->next);

    if (rt->running.head && rt->running.head->next) {
      io_uring_submit_and_wait(&rt->sch->ring, 1);
      unsigned count = io_uring_peek_batch_cqe(&rt->sch->ring, rt->sch->cqes, rt->sch->ring_size);
      for (unsigned i = 0; i < count; i++) {
        rt->sch->ready[i] = (td_coro *)io_uring_cqe_get_data(rt->sch->cqes[i]);
        rt->sch->ready[i]->io_res = rt->sch->cqes[i]->res;
      }
      io_uring_cq_advance(&rt->sch->ring, count);
      for (unsigned i = 0; i < count; i++) {
        td_resume(rt, rt->sch->ready[i]);
      }
    }
  }
}

void td_sch_init_opts(td_rt *rt, unsigned int ring_size, unsigned int flags) {
  if (ring_size == 0) {
    ring_size = TD_DEFAULT_RING_SIZE;
  }
  td_sch *sch = malloc(sizeof(td_sch));
  struct io_uring ring;

  io_uring_queue_init(ring_size, &ring, flags);

  sch->ring = ring;
  sch->ring_size = ring_size;
  sch->cqes = malloc(ring_size * sizeof(struct io_uring_cqe *));
  sch->ready = malloc(ring_size * sizeof(td_coro *));
  rt->sch = sch;
}

void td_sch_init(td_rt *rt) {
  td_sch_init_opts(rt, TD_DEFAULT_RING_SIZE, 0);
}

void td_sch_free(td_rt *rt) {
  if (rt->sch) {
    io_uring_queue_exit(&rt->sch->ring);
    free(rt->sch->cqes);
    free(rt->sch->ready);
    free(rt->sch);
    rt->sch = NULL;
  }
}
