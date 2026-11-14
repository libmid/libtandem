#ifndef TD_QUEUE_H
#define TD_QUEUE_H

#include <stdbool.h>
#include <stddef.h>

struct td_coro;

typedef struct td_queue {
  struct td_coro *head;
  struct td_coro *tail;
} td_queue;

void td_queue_push(td_queue *q, struct td_coro *c);
struct td_coro *td_queue_pop(td_queue *q);
void td_queue_remove(td_queue *q, struct td_coro *c);

#endif

