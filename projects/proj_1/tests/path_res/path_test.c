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

  char* path, *dest = NULL;
  path = calloc(5, sizeof(char));
  snprintf(path, 5, "../a");
  expand_pwd(&dest, path);
  printf("Original: %s\nExpanded: %s\n", path, dest);
  expand_prev(&dest);

  path = NULL;
  return 0;
}

void print_path_type(enum Path_Type pt)
{
  switch(pt) {
    case PATH_ABS:  printf("PATH_ABS\n"); break;
    case PATH_HOME: printf("PATH_HOME\n"); break;
    case PATH_CURR: printf("PATH_CURR\n"); break;
    case PATH_PREV: printf("PATH_PREV\n"); break;
    case PATH_PATH: printf("PATH_PATH\n"); break;
    case PATH_ERR:  printf("PATH_ERR\n"); break;
    default:        printf("DOESNT EXIST\n"); break;
  }
}

void test_get_path()
{
  char abso[] = "/asdfjasdfklj/asdfjkasdf";
  char hom[] = "~/asdfa/sdfasd";
  char curr1[] = "asdf/asf";
  char curr2[] = ".asdf";
  char curr3[] = "./asdf";
  char prev[] = "../asdf";
  char sing[] = "s";
  char blnk[] = "";
  char* null = NULL;

  printf("%s:\t", abso);
  print_path_type(get_path_type(abso));

  printf("%s:\t", hom);
  print_path_type(get_path_type(hom));

  printf("%s:\t", curr1);
  print_path_type(get_path_type(curr1));

  printf("%s:\t", curr2);
  print_path_type(get_path_type(curr2));

  printf("%s:\t", curr3);
  print_path_type(get_path_type(curr3));

  printf("%s:\t", prev);
  print_path_type(get_path_type(prev));

  printf("%s:\t", sing);
  print_path_type(get_path_type(sing));

  printf("%s:\t", blnk);
  print_path_type(get_path_type(blnk));

  printf("%s:\t", null);
  print_path_type(get_path_type(null));
}
