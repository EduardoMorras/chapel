#include "argparsing.h"
#include <assert.h>
#include <qthread/qthread.h>
#include <qthread/qtimer.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#define NUM_THREADS 10
#define PER_THREAD_INCR 1000000

aligned_t counter = 0;

static aligned_t qincr(void *arg) {
  aligned_t *c = (aligned_t *)arg;
  size_t incrs;

  for (incrs = 0; incrs < PER_THREAD_INCR; incrs++) { qthread_incr(c, 1); }
  return 0;
}

int main(int argc, char *argv[]) {
  aligned_t rets[NUM_THREADS];
  qtimer_t timer = qtimer_create();
  double cumulative_time = 0.0;

  if (qthread_initialize() != QTHREAD_SUCCESS) {
    fprintf(stderr, "qthread library could not be initialized!\n");
    exit(EXIT_FAILURE);
  }
  CHECK_VERBOSE();

  for (int iteration = 0; iteration < 10; iteration++) {
    counter = 0;
    qtimer_start(timer);
    for (int i = 0; i < NUM_THREADS; i++) {
      qthread_fork(qincr, &counter, &(rets[i]));
    }
    for (int i = 0; i < NUM_THREADS; i++) { qthread_readFF(NULL, &(rets[i])); }
    qtimer_stop(timer);
    assert(counter == (NUM_THREADS * PER_THREAD_INCR));
    iprintf("\ttest iteration %i: %f secs\n", iteration, qtimer_secs(timer));
    cumulative_time += qtimer_secs(timer);
  }
  printf("qthread time: %f\n", cumulative_time / 10.0);

  return 0;
}

/* vim:set expandtab */
