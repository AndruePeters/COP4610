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

  //test_get_path();

  char first[] = "/ab/cd/ef/";
  char sec[] = "as";
  char* res;
  concat_path(first, sec, &res);
  printf("res: %s\n", res);
  free(res);

  char* path;
  path = calloc(49, sizeof(char));
  snprintf(path, 49, "User/druepeters/./../more_stuff/stuff/../filec/");
  char* p = get_path(path);


  free(path);
  free(p);
  path = NULL;


  return 0;
}
