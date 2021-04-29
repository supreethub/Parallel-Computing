#ifndef __PAR_LOOP_H
#define __PAR_LOOP_H

#include <functional>

class ParLoop {
public:
  /// @brief execute the function f multiple times with different
  /// parameters possibly in parallel
  ///
  /// f will be executed multiple times with parameters starting at
  /// beg, no greater than end, in inc increment. These execution may
  /// be in parallel
  void parfor (size_t beg, size_t end, size_t inc,
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
  template<typename TLS>
  void parfor (size_t beg, size_t end, size_t increment, size_t nbthread,
	       size_t granularity,
	       std::function<void(TLS&)> before,
	       std::function<float(TLS&, int)> f,
	       std::function<void(TLS&)> after
	       ) {
    TLS tls;
    before(tls);    
    float sum = 0;
    if((end-beg)/nbthread<=granularity)
    {

    
//      for( size_t t =beg; t<nbthread; ++t){
//        tls = 0;
//        for(size_t j =t; j<end;j+=nbthread){	 
//     	  tls=f(tls,j);
//      }
//     	sum+=tls;
//      }
      
      for( size_t t = beg; t<end; t+=nbthread)
      {
        tls = 0;
        for( size_t j = t; j<t+nbthread && j<end;++j)
        {
          tls=f(tls,j);
        }
        sum+=tls;
      }
    }

     else
     {
        for(size_t t = beg;t<end; t+=granularity)
        {
          tls = 0;
          for( size_t j = t;j<t+granularity; ++j)
          {
            if(j==end)
            { 
              break;
            }
      	    tls=f(tls, j);
          }
          sum+=tls; 
        }
     }

    after(sum);
  }  
};

#endif
