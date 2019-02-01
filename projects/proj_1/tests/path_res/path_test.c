/*
  Andrue Peters
  1/19/18

  COP4610 - Group 15 - Proj 1

*/

#include <utility/path.h>
#include <utility/path.c>

#include <stdio.h>
#include <stdlib.h>

void print_path_type(enum Path_Type pt);
void test_get_path();
int main()
{
  char f[] = "User/druepeters/./../more_stuff/stuff/../filec/";
  char rel[] = "/project/tests/test.c";
  char* path = strdup(f);
  printf("Path: %s\n", path);
  char* p = get_path(path);
  printf("Final: \n\t%s\n", p);



  free(path);
  free(p);
  path = NULL;
  p = NULL;


  return 0;
}
