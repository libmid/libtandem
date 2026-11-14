#include <assert.h>
#include <liburing.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>

#include <tandem/common.h>
#include <tandem/coro.h>
#include <tandem/queue.h>
#include <tandem/scheduler.h>

#ifndef TD_RING_SIZE
#define TD_RING_SIZE 32
#endif

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

static struct io_uring_sqe *get_sqe(td_rt *rt) {
  assert(rt->sch != NULL);
  struct io_uring_sqe *sqe = io_uring_get_sqe(&rt->sch->ring);
  if (!sqe) {
    io_uring_submit(&rt->sch->ring);
    sqe = io_uring_get_sqe(&rt->sch->ring);
    assert(sqe != NULL);
  }
  return sqe;
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
      struct io_uring_cqe *cqes[TD_RING_SIZE];
      io_uring_submit_and_wait(&rt->sch->ring, 1);
      unsigned count = io_uring_peek_batch_cqe(&rt->sch->ring, cqes, TD_RING_SIZE);
      td_coro *ready[TD_RING_SIZE];
      for (unsigned i = 0; i < count; i++) {
        ready[i] = (td_coro *)io_uring_cqe_get_data(cqes[i]);
        ready[i]->io_res = cqes[i]->res;
      }
      io_uring_cq_advance(&rt->sch->ring, count);
      for (unsigned i = 0; i < count; i++) {
        td_resume(rt, ready[i]);
      }
    }
  }
}

void td_sch_init(td_rt *rt) {
  td_sch *sch = malloc(sizeof(td_sch));
  struct io_uring ring;

  io_uring_queue_init(TD_RING_SIZE, &ring, 0);

  sch->ring = ring;
  rt->sch = sch;
}

void td_sch_free(td_rt *rt) {
  if (rt->sch) {
    io_uring_queue_exit(&rt->sch->ring);
    free(rt->sch);
    rt->sch = NULL;
  }
}

void sch_wait_on_io(td_rt *rt) {
  rt->current->status = TD_CORO_WAITING_ON_IO;
  td_suspend(rt);
  rt->current->status = TD_CORO_RUNNING;
}

void td_sleep(td_rt *rt, size_t millis) {
  struct __kernel_timespec ts = {
    .tv_sec = millis / 1000,
    .tv_nsec = (millis % 1000) * 1000000ULL
  };
  struct io_uring_sqe *sqe = get_sqe(rt);

  io_uring_prep_timeout(sqe, &ts, 0, 0);
  io_uring_sqe_set_data(sqe, rt->current);

  sch_wait_on_io(rt);
}

ssize_t td_read(td_rt *rt, int fd, void *buf, size_t count) {
  struct io_uring_sqe *sqe = get_sqe(rt);

  io_uring_prep_read(sqe, fd, buf, count, 0);
  io_uring_sqe_set_data(sqe, rt->current);

  sch_wait_on_io(rt);

  return rt->current->io_res;
}

ssize_t td_write(td_rt *rt, int fd, void *buf, size_t count) {
  struct io_uring_sqe *sqe = get_sqe(rt);

  io_uring_prep_write(sqe, fd, buf, count, 0);
  io_uring_sqe_set_data(sqe, rt->current);

  sch_wait_on_io(rt);

  return rt->current->io_res;
}

int td_socket(td_rt *rt, int domain, int type, int protocol) {
  struct io_uring_sqe *sqe = get_sqe(rt);

  io_uring_prep_socket(sqe, domain, type, protocol, 0);
  io_uring_sqe_set_data(sqe, rt->current);

  sch_wait_on_io(rt);

  return rt->current->io_res;
}

int td_bind(td_rt *rt, int sockfd, struct sockaddr *addr, socklen_t addrlen) {
  struct io_uring_sqe *sqe = get_sqe(rt);

  io_uring_prep_bind(sqe, sockfd, addr, addrlen);
  io_uring_sqe_set_data(sqe, rt->current);

  sch_wait_on_io(rt);

  return rt->current->io_res;
}

int td_listen(td_rt *rt, int sockfd, int backlog) {
  struct io_uring_sqe *sqe = get_sqe(rt);

  io_uring_prep_listen(sqe, sockfd, backlog);
  io_uring_sqe_set_data(sqe, rt->current);

  sch_wait_on_io(rt);

  return rt->current->io_res;
}

int td_accept(td_rt *rt, int sockfd, struct sockaddr *addr,
              socklen_t *addrlen) {
  struct io_uring_sqe *sqe = get_sqe(rt);

  io_uring_prep_accept(sqe, sockfd, addr, addrlen, 0);
  io_uring_sqe_set_data(sqe, rt->current);

  sch_wait_on_io(rt);

  return rt->current->io_res;
}

int td_close(td_rt *rt, int fd) {
  struct io_uring_sqe *sqe = get_sqe(rt);

  io_uring_prep_close(sqe, fd);
  io_uring_sqe_set_data(sqe, rt->current);

  sch_wait_on_io(rt);

  return rt->current->io_res;
}
