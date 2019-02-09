
#include <stdio.h>
#include <stdlib.h>
#include <utility/path.h>
#include "echo.h"

/*
  The method for expanding is bad, but it was designed before echo
  was changed to have an array of strings.
*/
void echo(int argc, char **argv)
{
  int i;
  char *exp;
  for (i = 1; i < argc; ++i) {
    exp = expand_env(argv[i]);
    if (exp) {
      free(exp);
    } else {
      printf("Environmental variable does not exist.\n");
    }
  }
}
