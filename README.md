> *Tandem* - Two people or pieces of equipment that work together to achieve a result

Libtandem is a stack based (no callback) coroutine library for c using context switching and `io_uring`. Naturally, only linux kernel >= 5.1 is supported. The code is self documenting and public api is marked with TD_API so checkout `include/` for docs.

## Build & Usage

### Requirements
- liburing

Build shared and static library using `make`. Build artifacts can be found in `build/`.

Look into `examples/` for more examples.

## Basic Coroutine Example

``` c
#include <stdio.h>
#include <tandem/coro.h>

#define STACK_SIZE 64 * 1024

void foo(td_rt *rt) {
  for (int i = 1; i <= 10; i++) {
    printf("FOO: %d\n", i);
    td_suspend(rt);
  }
}

void bar(td_rt *rt) {
  for (int i = 2; i <= 10; i+=2) {
    printf("BAR: %d\n", i);
    td_suspend(rt);
  }
}

int main() {
  td_rt *rt = td_init();

  td_coro* coro1 = td_spawn(rt, &foo, 0, NULL, STACK_SIZE);
  td_coro* coro2 = td_spawn(rt, &bar, 0, NULL, STACK_SIZE);

  while (coro1->status == TD_CORO_RUNNING || coro2->status == TD_CORO_RUNNING) {
    printf("MAIN\n");
    td_resume(rt, coro1);
    td_resume(rt, coro2);
  }

  td_free(rt);

  return 0;
}
```

## I/O Scheduler Example

```c
#include <stdio.h>
#include <tandem/tandem.h>

#define MSEC 500
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
    td_sleep(rt, MSEC);
  }
}

int main() {
  td_rt *rt = td_init();
  td_sch_init(rt);

  td_spawn(rt, &input, 0, NULL, STACK_SIZE);
  td_spawn(rt, &counter_loop, 0, NULL, STACK_SIZE);

  td_sch_run(rt);

  td_free(rt);
  return 0;
}
```
## Benchmarks

Here's some benchmark results on my ryzen 7 8840HS.

```
Total iterations: 10000000
Total switches:   20000000
Total time:       0.257 s
Time per switch:  12.87 ns
Switches / sec:   77.68 M/s
```