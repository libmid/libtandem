#include <stdio.h>
#include <time.h>
#include <tandem/tandem.h>

#define ITERS 10000000

static int counter = 0;

static void ping_pong(td_rt *rt) {
  while (counter < ITERS) {
    counter++;
    td_suspend(rt);
  }
}

int main() {
  td_rt *rt = td_init();
  td_coro *c = td_spawn(rt, ping_pong, 0, NULL, 32 * 1024);

  struct timespec t0, t1;
  clock_gettime(CLOCK_MONOTONIC, &t0);

  while (counter < ITERS) {
    td_resume(rt, c);
  }

  clock_gettime(CLOCK_MONOTONIC, &t1);

  double elapsed_sec = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
  double ns_per_switch = (elapsed_sec * 1e9) / (2.0 * ITERS);

  printf("Context Switch Benchmark:\n");
  printf("  Total iterations: %d\n", ITERS);
  printf("  Total switches:   %d\n", 2 * ITERS);
  printf("  Total time:       %.3f s\n", elapsed_sec);
  printf("  Time per switch:  %.2f ns\n", ns_per_switch);
  printf("  Switches / sec:   %.2f M/s\n", (2.0 * ITERS / elapsed_sec) / 1e6);

  td_free(rt);
  return 0;
}

