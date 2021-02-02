#include<unistd.h>
#include<iostream>

int main () 
{
  char hostName[1024];
  gethostname(hostName,1024);
  std::cout<<hostName<<std::endl;
  return 0;
}
