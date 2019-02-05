/*
  Andrue Peters
  https://www.geeksforgeeks.org/c-program-replace-word-text-another-given-word/
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <glib.h>

#include <builtins/shell_data.h>
#include <builtins/shell_data.c>

#include <utility/path.h>
#include <utility/path.c>

#include "shell.h"

int main()
{
  char* line = NULL;
  struct shell_data sd;

  shell_data_init(&sd);
  while (1) {
    display_prompt(&sd);
    line = get_line();
    printf("You entered: %s\n\n\n", line);
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
  char* exp = expand_env(line);
  if (exp) {
    free(line);
    line = exp;
  }
  return line;
}



void display_prompt(struct shell_data *sd)
{
  static char hostname[255];
  gethostname(hostname, 255);
  printf("%s@%s:%s$ ", getenv("USER"), hostname, sd->pwd);
}
