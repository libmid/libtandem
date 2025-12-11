#include <stdio.h>

#include "../include/tandem/coro.h"
#include "../include/tandem/scheduler.h"

#define MSEC 2000
#define STACK_SIZE 32 * 1024

int flag = 0;

void input(td_rt *rt) {
  char msg[1024];

  printf("[Task READ] Waiting for user input\n");
  size_t len = td_read(rt, 0, msg, 1024);
  printf("[Task READ] READ: %zu bytes\n", len);

  flag = 1;
  printf("[Task READ] Set flag = 1\n");
}

void counter_loop(td_rt *rt) {
  int c = 1;
  while (!flag) {
    printf("[COUNTER] counter = %d\n", c++);
    td_sleep(rt, 1500);
  }
}

int main() {
  td_rt *rt = td_init();
  td_sch_init(rt);

  td_spawn(rt, &input, 0, NULL, STACK_SIZE);
  td_spawn(rt, &counter_loop, 0, NULL, STACK_SIZE);

  td_sch_loop(rt);

  td_free(rt);
  return 0;
}
