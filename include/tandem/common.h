#ifndef TD_COMMON_H
#define TD_COMMON_H

#include <liburing.h>
#include <stdbool.h>
#include <stdlib.h>

#include "queue.h"

typedef enum td_coro_status {
  TD_CORO_FREE,
  TD_CORO_RUNNING,
  TD_CORO_WAITING_ON_IO,
  TD_CORO_FINISHED,
  TD_CORO_DEAD,
} td_coro_status;

typedef struct td_ctx {
  void *rip, *rsp, *rbp, *rbx, *r12, *r13, *r14, *r15;
} td_ctx;

typedef struct td_sch {
  struct io_uring ring;
} td_sch;

typedef struct td_rt {
  size_t id_counter;
  td_queue free;
  td_queue running;
  td_queue finished;
  td_queue dead;
  struct td_sch *sch;
  struct td_coro *current;
  struct td_coro *main_coro;
} td_rt;

typedef struct td_coro {
  size_t id;
  td_ctx ctx;
  void *stack;
  size_t stack_size;
  td_coro_status status;
  void (*fn)(td_rt *);

  size_t argc;
  void *argv;
  void *result;
  int io_res;

  struct td_coro *prev;
  struct td_coro *next;
} td_coro;

#endif
