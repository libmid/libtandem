#include <tandem/common.h>
#include <tandem/queue.h>

void td_queue_push(td_queue *q, td_coro *c) {
  c->next = NULL;
  c->prev = q->tail;
  if (q->tail) {
    q->tail->next = c;
  } else {
    q->head = c;
  }
  q->tail = c;
}

td_coro *td_queue_pop(td_queue *q) {
  td_coro *c = q->head;
  if (!c) {
    return NULL;
  }
  q->head = c->next;
  if (q->head) {
    q->head->prev = NULL;
  } else {
    q->tail = NULL;
  }
  c->next = NULL;
  c->prev = NULL;
  return c;
}

void td_queue_remove(td_queue *q, td_coro *c) {
  if (c->prev) {
    c->prev->next = c->next;
  } else {
    q->head = c->next;
  }
  if (c->next) {
    c->next->prev = c->prev;
  } else {
    q->tail = c->prev;
  }
  c->next = NULL;
  c->prev = NULL;
}

