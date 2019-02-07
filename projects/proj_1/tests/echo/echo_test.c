/*
  Andrue Peters
  2/1/19
*/

#include <builtins/echo.h>
#include <builtins/echo.c>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_line();
int main()
{
  char* c;
  while (1) {
    c = get_line();
    echo(c);
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
  char* exp = expand_env(line);
  if (exp) {
    free(line);
    line = exp;
  }
  return line;
}
