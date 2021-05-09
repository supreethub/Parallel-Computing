#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <iostream>
#include "par_loop.hpp"
using namespace std;
#ifdef __cplusplus
extern "C"
{
#endif
  void generateMergeSortData(int *arr, size_t n);
  void checkMergeSortResult(int *arr, size_t n);
#ifdef __cplusplus
}
#endif

// conventional merge function
void merge(int arr[], int leftIndex, int middleIndex, int rightIndex)
{
  int i = 0;
  int j = 0;
  int k = 0;
  int leftIndex_length = middleIndex - leftIndex + 1;
  int rightIndex_length = rightIndex - middleIndex;

  //creating dummy arrays to hold temp values
  int *leftIndex_array = new int[leftIndex_length];
  int *rightIndex_array = new int[rightIndex_length];

  //copy value to left array
  for (int i = 0; i < leftIndex_length; i++)
  {
    leftIndex_array[i] = arr[leftIndex + i];
  }

  //copy values to right array
  for (int j = 0; j < rightIndex_length; j++)
  {
    rightIndex_array[j] = arr[middleIndex + 1 + j];
  }

  i = 0;
  j = 0;
  // select and copy to original array
  while (i < leftIndex_length && j < rightIndex_length)
  {
    if (leftIndex_array[i] <= rightIndex_array[j])
    {
      arr[leftIndex + k] = leftIndex_array[i];
      i++;
    }
    else
    {
      arr[leftIndex + k] = rightIndex_array[j];
      j++;
    }
    k++;
  }

  // copy rest
  while (i < leftIndex_length)
  {
    arr[leftIndex + k] = leftIndex_array[i];
    k++;
    i++;
  }
  while (j < rightIndex_length)
  {
    arr[leftIndex + k] = rightIndex_array[j];
    k++;
    j++;
  }
  //deallocate array
  delete[] leftIndex_array;
  delete[] rightIndex_array;
}

// conventional mergeSort function
void merge_sort(int arr[], int leftIndex, int rightIndex)
{
  if (leftIndex < rightIndex)
  {
    int middleIndex = leftIndex + (rightIndex - leftIndex) / 2;
    merge_sort(arr, leftIndex, middleIndex);
    merge_sort(arr, middleIndex + 1, rightIndex);
    merge(arr, leftIndex, middleIndex, rightIndex);
  }
}

// since sections of the array are sorted by threads, now the sorted arrays needs to be sorted
//starting from 2 sections and all the way to complete sections
void merge_thread_sections(int arr[], int nthreads, int selection, int thread_section, int n)
{
  parFor(0,nthreads,2,nthreads, [&](int i)->void{
    {
      int leftIndex = i * (thread_section * selection);
      int rightIndex = ((i + 2) * thread_section * selection) - 1;
      int middleIndex = leftIndex + (thread_section * selection) - 1;
      //if rightIndex is out of bound adjust it to higher bound
      if (rightIndex >= n)
      {
        rightIndex = n - 1;
      }
      // perform a traditional merge
      merge(arr, leftIndex, middleIndex, rightIndex);
    }
  });
  
  if (nthreads > 1)
  {
    // if the threads more than 1 that means there are sections of merge that are yet to be merged.
    merge_thread_sections(arr, nthreads / 2, selection * 2, thread_section, n);
  }
}

int main(int argc, char *argv[])
{

  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0] << " <n> <nbthreads>" << std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int nthreads = atoi(argv[2]);
  // get arr data
  int *arr = new int[n];
  const int thread_section = n / nthreads;
  const int offset = n % nthreads;
  generateMergeSortData(arr, n);

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  auto thread_level_sort = [&](int thread_id) -> void {
    int leftIndex = thread_id * (thread_section);
    int rightIndex = (thread_id + 1) * (thread_section)-1;
    // adjust the remaining values in last section
    if (thread_id == nthreads - 1)
    {
      rightIndex += offset;
    }
    int middleIndex = leftIndex + (rightIndex - leftIndex) / 2;
    //perform a normal merge per thread on a section of original array
    if (leftIndex < rightIndex)
    {
      merge_sort(arr, leftIndex, rightIndex);
      merge_sort(arr, leftIndex + 1, rightIndex);
      merge(arr, leftIndex, middleIndex, rightIndex);
    }
  };

  parFor(0, nthreads,1,nthreads, thread_level_sort);

  merge_thread_sections(arr, nthreads, 1, thread_section, n);
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elpased_seconds = end - start;
  std::cerr << elpased_seconds.count() << std::endl;
  checkMergeSortResult(arr, n);

  delete[] arr;

  return 0;
}