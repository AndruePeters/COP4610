/*
  Andrue Peters
  2/18/19
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


int main()
{
  fork();
  fork();
  printf("Hello world!\n");

  return 0;
}
