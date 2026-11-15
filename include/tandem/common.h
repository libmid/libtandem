#ifndef TD_COMMON_H
#define TD_COMMON_H

#if defined(__GNUC__) && __GNUC__ >= 4
  #define TD_API __attribute__((visibility("default")))
#else
  #define TD_API
#endif

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

#if defined(__x86_64__)
typedef struct td_ctx {
  void *rip, *rsp, *rbp, *rbx, *r12, *r13, *r14, *r15;
} td_ctx;
#elif defined(__aarch64__)
typedef struct td_ctx {
  void *sp;
  void *x19, *x20, *x21, *x22, *x23, *x24, *x25, *x26, *x27, *x28;
  void *fp, *lr;
  void *d8, *d9, *d10, *d11, *d12, *d13, *d14, *d15;
} td_ctx;
#elif defined(__arm__)
typedef struct td_ctx {
  void *sp;
  void *r4, *r5, *r6, *r7, *r8, *r9, *r10, *r11, *lr;
  void *d8, *d9, *d10, *d11, *d12, *d13, *d14, *d15;
} td_ctx;
#else
#error "Unsupported architecture"
#endif

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
