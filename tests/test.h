#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>

static int test_total = 0;
static int test_passed = 0;

#define TEST_ASSERT(cond) do { \
  if (!(cond)) { \
    printf("FAILED: %s:%d: %s\n", __FILE__, __LINE__, #cond); \
    exit(1); \
  } \
} while (0)

#define TEST_RUN(fn) do { \
  test_total++; \
  printf("  RUN   %s\n", #fn); \
  fn(); \
  test_passed++; \
  printf("  PASS  %s\n", #fn); \
} while (0)

#define TEST_REPORT() do { \
  printf("%d/%d tests passed\n", test_passed, test_total); \
} while (0)

#endif

