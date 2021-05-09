#ifndef __SEQ_LOOP_H
#define __SEQ_LOOP_H

#include <functional>
#include <vector>
#include <thread>
#include <queue>
#include <iostream>
#include <condition_variable>

using namespace std;

void simpleFor(size_t beg, size_t end, size_t inc,
               std::function<void(int)> f)
{
  for (size_t i = beg; i < end; i += inc) {
    f(i);
  }
}

void parFor(size_t beg, size_t end, size_t inc, int nthreads,
               std::function<void(int)> f) {
    thread threads[nthreads];
  for (long i = beg; i < end; i+=inc)
  {
    threads[i] = thread(f, i);
  }

  for (long i = beg; i < end; i+=inc)
  {
    threads[i].join();
  }
}

#endif