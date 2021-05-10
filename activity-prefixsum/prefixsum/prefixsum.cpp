#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<chrono>
#include<cstring>



#ifdef __cplusplus
extern "C" {
#endif
  void generatePrefixSumData (int* arr, size_t n);
  void checkPrefixSumResult (int* arr, size_t n);
#ifdef __cplusplus
}
#endif


using namespace std;
void prefixsum_inplace(int *x, int *ans, unsigned long N,int nbthreads) {
    int *suma;
    omp_set_dynamic(0);
    omp_set_num_threads(nbthreads);
    #pragma omp parallel
    {
        #pragma omp single
        {
            const int nthreads = omp_get_num_threads();
            suma = new int[nthreads+1];
            suma[0] = 0;
        }
        const int ithread = omp_get_thread_num();
        float sum = 0;
        #pragma omp for schedule(static)
        for (int i=0; i<N; i++) {
            sum += x[i];
            ans[i] = sum;
        }
        suma[ithread+1] = sum;
        #pragma omp barrier
        float offset = 0;
        for(int i=0; i<(ithread+1); i++) {
            offset += suma[i];
        }
        #pragma omp for schedule(static)
        for (int i=0; i<N; i++) {
            ans[i] += offset;
        }
    }
    memmove(&(ans[1]), &(ans[0]), N * sizeof(int));
    delete[] suma;
}



int main (int argc, char* argv[]) {
  //forces openmp to create the threads beforehand
#pragma omp parallel
  {
    int fd = open (argv[0], O_RDONLY);
    if (fd != -1) {
      close (fd);
    }
    else {
      std::cerr<<"something is amiss"<<std::endl;
    }
  }
  
  if (argc < 3) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }



  
  int n = atoi(argv[1]);
  int nbthreads = atoi(argv[2]);

  int * arr = new int [n];
  generatePrefixSumData (arr, n);


  int * pr = new int [n+1];

  //insert prefix sum code here


  std::chrono::time_point<std::chrono::system_clock> start_clock, end_clock;
  start_clock = std::chrono::system_clock::now();
  
  prefixsum_inplace(arr,pr, n,nbthreads);

  end_clock = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end_clock-start_clock;
  cerr<<elapsed_seconds.count();
  checkPrefixSumResult(pr, n);

  delete[] arr;

  return 0;
}