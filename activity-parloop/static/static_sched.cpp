#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <cmath>
#include <vector>
#include "seq_loop.hpp"

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif


int main (int argc, char* argv[]) {

  if (argc < 7) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads>"<<std::endl;
    return -1;
  }
  
  int functionid = stoi(argv[1]);
  float a = stoi(argv[2]);
  float b = stoi(argv[3]);
  float n = stoi(argv[4]);
  float intensity = stoi(argv[5]);
  int nbthreads = stoi(argv[6]);
  float term = ((b - a) / n);
  float sum = 0.0;

  std::chrono::time_point<std::chrono::system_clock> begin = std::chrono::system_clock::now();

  SeqLoop s1;

  s1.parfor<float>(
      0, n, 1, nbthreads,
      [&](float &tls) -> void {
        tls = 0.0;
      },
      [&](int i, float &tls) -> void 
      {
        float x = (a + (i + 0.5) * term);
        switch (functionid)
        {
          case 1:
              tls += f1(x, intensity);
              break;
          case 2:
              tls += f2(x, intensity);
              break;
          case 3:
              tls += f3(x, intensity);
              break;
          case 4:
              tls += f4(x, intensity);
              break;
        }
      },
      [&](float &tls) -> void {
        sum += tls;
      });


  std::cout << (term * sum) << endl;

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();


  std::chrono::duration<double> elapsed_seconds = end - begin;

  std::cerr << elapsed_seconds.count() << std::endl;


  return 0;
}
