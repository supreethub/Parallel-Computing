#include<unistd.h>
#include<iostream>
#include<string>
int main () 
{
  char hostName[1024];
  gethostname(hostName,1024);
  std::cout<<hostName<<std::endl;
  return 0;
}
