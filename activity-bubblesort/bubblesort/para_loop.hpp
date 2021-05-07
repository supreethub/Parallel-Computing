#ifndef __PARA_LOOP_H
#define __PARA_LOOP_H

#include <vector>
#include <thread>
#include <functional>

using namespace std;

// class ParaLoop {
// public:
  /// @brief execute the function f multiple times with different
  /// parameters possibly in parallel
  ///
  /// f will be executed multiple times with parameters starting at
  /// beg, no greater than end, in inc increment. These execution may
  /// be in parallel
  void parafor (size_t beg, size_t end, size_t inc,
	       std::function<void(int)> f) {
    for (size_t i=beg; i<end; i+= inc) {
      f(i);
    }
  }

  /// @brief execute the function f multiple times with different
  /// parameters possibly in parallel
  ///
  /// f will be executed multiple times with parameters starting at
  /// beg, no greater than end, in inc increment. These execution may
  /// be in parallel.
  ///
  /// Each thread that participate in the calculation will have its
  /// own TLS object.
  ///
  /// Each thread will execute function before prior to any
  /// calculation on a TLS object.
  ///
  /// Each thread will be passed the TLS object it executed before on.
  ///
  /// Once the iterations are complete, each thread will execute after
  /// on the TLS object. No two thread can execute after at the same time.
  
  

// void simpleFor(size_t beg, size_t end, size_t inc,
//               std::function<void(int)> f)
// {
//   for (size_t i = beg; i < end; i += inc)
//   {
//     f(i);
//   }
// }

void staticFor(size_t beg, size_t end, size_t inc, size_t nthreads,
               std::function<void(int)> func)
{
  vector<thread> thread_stack;
  for (int t = 1, start = beg; t <= nthreads; t++, start += inc)
  {
    thread_stack.push_back(thread(parafor, start, end, inc * nthreads, func));
  }
  for (auto &itr : thread_stack)
  {
    itr.join();
  }
}
  
  template<typename TLS> 
   
  void parfor (size_t beg, size_t end, size_t increment, size_t numthreads,
	       std::function<void(TLS&)> before,
	       std::function<void(int, TLS &)> f,
	       std::function<void(TLS&)> after
	       )
    {

    vector<thread> threads;
    vector<TLS> thrTLS(numthreads);


    for (int t = 0; t<numthreads; t+=1)
    {
      before(thrTLS[t]);
      threads.push_back(thread(parafor, t, end, numthreads, [&, t](int j) -> void {
        f(j, thrTLS[t]);
      }));
    }
    

      for (auto &t : threads)
      {
        t.join();
      }

      for (auto &t : thrTLS)
      {
        after(t);
      }
  }
  
// };

#endif
