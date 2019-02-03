/*
  Andrue Peters
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"

int main()
{
  char* line = NULL;
  while (1) {
    line = get_line();
    printf("You entered: %s\n", line);
    free(line);
  }

  return 0;
}

char* get_line()
{
  unsigned lim_size = 4096;
  unsigned pos = 0;
  char* line = calloc(lim_size,  sizeof(char));
  int c; /* fgets returns 277 values, so char can't be used */


  while ( (c = fgetc(stdin)) != '\n' && !feof(stdin)) {
    line[pos] = c;

    if (++pos == lim_size) {
        line = realloc(line, (lim_size *= 2) * sizeof(char));
    }
  }

  line = realloc(line, (pos+1) * sizeof(char));
  line[pos] = '\0';
  return line;
}
