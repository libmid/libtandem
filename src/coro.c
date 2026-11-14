#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <tandem/common.h>
#include <tandem/coro.h>
#include <tandem/queue.h>

void __td_switch(td_ctx *from, td_ctx *to);
void __td_entry();
void td_sch_free(td_rt *rt);

td_rt *td_init() {
  td_rt *rt = calloc(1, sizeof(td_rt));

  rt->main_coro = calloc(1, sizeof(td_coro));
  rt->main_coro->status = TD_CORO_RUNNING;
  rt->main_coro->id = 0;

  td_queue_push(&rt->running, rt->main_coro);
  rt->current = rt->main_coro;

  return rt;
}

void td_free(td_rt *rt) {
  td_coro *c;
  while ((c = td_queue_pop(&rt->running))) {
    if (c != rt->main_coro) {
      free(c->stack);
    }
    free(c);
  }
  while ((c = td_queue_pop(&rt->finished))) {
    if (c != rt->main_coro) {
      free(c->stack);
    }
    free(c);
  }
  while ((c = td_queue_pop(&rt->free))) {
    free(c->stack);
    free(c);
  }
  while ((c = td_queue_pop(&rt->dead))) {
    free(c->stack);
    free(c);
  }
  if (rt->sch != NULL) {
    td_sch_free(rt);
  }
  free(rt);
}

void td_suspend(td_rt *rt) {
  if (rt->current == NULL) {
    __td_switch(&rt->finished.tail->ctx, &rt->main_coro->ctx);
  } else {
    __td_switch(&rt->current->ctx, &rt->main_coro->ctx);
  }
}

void td_resume(td_rt *rt, td_coro *coro) {
  if (coro->status == TD_CORO_FINISHED || coro->status == TD_CORO_DEAD)
    return;

  td_coro *current = rt->current;
  coro->status = TD_CORO_RUNNING;
  rt->current = coro;
  __td_switch(&current->ctx, &coro->ctx);
  rt->current = current;
}

static void entry(td_rt *rt) {
  td_coro *coro = rt->current;
  coro->fn(rt);

  td_queue_remove(&rt->running, coro);
  coro->status = TD_CORO_FINISHED;
  td_queue_push(&rt->finished, coro);

  rt->current = NULL;
  td_suspend(rt);
}

size_t td_argc(td_rt *rt) { return rt->current->argc; }

void *td_argv(td_rt *rt) { return rt->current->argv; }

void td_return(td_rt *rt, void *result) { rt->current->result = result; }

void *td_consume(td_rt *rt, td_coro *coro) {
  coro->status = TD_CORO_DEAD;
  td_queue_remove(&rt->finished, coro);
  td_queue_push(&rt->dead, coro);
  return coro->result;
}

td_coro *td_spawn(td_rt *rt, void (*fn)(td_rt *), size_t argc, void *argv,
                  size_t stack_size) {
  td_coro *coro = td_queue_pop(&rt->free);
  if (!coro) {
    coro = calloc(1, sizeof(td_coro));
    coro->stack = malloc(stack_size);
    coro->stack_size = stack_size;
  } else if (stack_size > coro->stack_size) {
    coro->stack = realloc(coro->stack, stack_size);
    coro->stack_size = stack_size;
  }

  coro->id = ++rt->id_counter;
  coro->argc = argc;
  coro->argv = argv;
  coro->status = TD_CORO_RUNNING;
  coro->fn = fn;
  coro->result = NULL;
  coro->ctx.rip = (void *)__td_entry;
  coro->ctx.rsp = (void *)(coro->stack + stack_size);
  coro->ctx.r12 = (void *)rt;
  coro->ctx.r13 = (void *)entry;

  td_queue_push(&rt->running, coro);
  return coro;
}
