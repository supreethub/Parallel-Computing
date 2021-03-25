#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>

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

  if (argc < 6) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }

  auto begin = std::chrono::high_resolution_clock::now();

  int functionID = atoi(argv[1]);
  float a = strtof(argv[2], NULL);
  float b = strtof(argv[3], NULL);;
  float n = strtof(argv[4], NULL);;
  int intensity = atoi(argv[5]);
  
  float term1 = (b-a)/n; //Consider (b-a)/n as term1
  float sum = 0;
  float functionTerm,funcVal; //Consider functionTerm as (a+(i+0.5)*term1)
  for(int i=0;i<n;i++)
  {
    functionTerm = a + ((i + 0.5) * term1);
      switch (functionID)
      {
        case 1:
        {
          funcVal = f1(functionTerm, intensity); 
          break;
        }
        case 2:
        {
          funcVal = f2(functionTerm, intensity);
          break;
        }
        case 3:
        {
          funcVal = f3(functionTerm, intensity);
          break;
        }
        case 4:
        {
          funcVal = f4(functionTerm, intensity);
          break;
        }
        default:
          break;
      }
    sum = sum + funcVal;
  }
  auto end = std::chrono::high_resolution_clock::now();

  std::cout<<sum * term1<<std::endl;

  //Consider timeTaken as the total time taken for execution
  std::chrono::duration<double> timeTaken = std::chrono::duration_cast<std::chrono::duration<double>>(end - begin);

  std::cerr << timeTaken.count() << std::endl;
  return 0;
}
