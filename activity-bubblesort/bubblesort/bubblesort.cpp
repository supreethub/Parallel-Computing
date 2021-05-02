#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <cmath>
#include "para_loop.hpp"


using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (const int* arr, size_t n);

#ifdef __cplusplus
}
#endif

void swap(int *arr, int i, int j)
{
  int temp = arr[i];
  arr[i] = arr[j];
  arr[j] = temp;
}

void printArr(int *arr, int n)
{
  for (int i = 0; i < n; i++)
  {
    cout << arr[i] << "\t";
  }
  cout << endl;
}

int main (int argc, char* argv[]) {
  if (argc < 3) { std::cerr<<"usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int nbthreads = atoi(argv[2]);
  // ParaLoop pl;
  // get arr data
  int * arr = new int [n];
  generateMergeSortData (arr, n);

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  //test code
  // printArr(arr, n)

  //insert sorting code here.
  bool flag = true;
  while(flag)
  {
    flag = false;
    staticFor(1, n-1, 2, nbthreads,
    [&](int i) -> void {
      if (arr[i] > arr[i+1])
      {
        swap(arr, i, i+1);
        flag = true;
      }
    });

  staticFor( 0, n-1, 2, nbthreads,
    [&](int i) -> void {
      if (arr[i] > arr[i+1])
      {
        swap(arr, i, i+1);
        flag = true;
      }
    });
    
  }

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elpased_seconds = end - start;
  
  checkMergeSortResult (arr, n);

  std::cerr << elpased_seconds.count() << std::endl;
  
  delete[] arr;

  return 0;
}
