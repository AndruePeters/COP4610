#include <stdlib.h>
#include <stdio.h>

void clear(int argc, char **argv)
{
  int num_lines = 100;
  int i;
  for (i = 0; i < num_lines; ++i) {
    printf("\n");
  }
}
