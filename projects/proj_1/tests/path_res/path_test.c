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
  char* path;
  path = calloc(50, sizeof(char));
  snprintf(path, 50, "User/druepeters/./../more_stuff/stuff/../filec/");
  printf("Path: %s\n", path);
  char* p = get_path(path);


  free(path);
  free(p);
  path = NULL;


  return 0;
}
