/*
  Andrue Peters
  2/1/19
*/

#include <builtins/cd.h>
#include <builtins/cd.c>
#include <shared.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_add_alias();
void test_alias();

int main()
{
  struct shell_data sd;
  sd.pwd = NULL;
  char * fp = strdup(getenv("HOME"));
  printf("Current env: %s\n", getenv("PWD"));
  cd(fp, &sd);
  printf("New env: %s\n", getenv("PWD"));
  printf("New env: %s\n", fp);
  return 0;
}
