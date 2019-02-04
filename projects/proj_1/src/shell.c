/*
  Andrue Peters
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include <glib.h>
#include "shell.h"

int main()
{
  char* line = NULL;
  while (1) {
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
  char* exp;
  expand_env(&exp, line);
  printf("exp: %s\n", exp);
  return line;
}

bool expand_env(char **dest, const char *src)
{
  int i = 0, j=0,
      env_start = 0,
      env_end = 0;
  bool is_env = false;
  char *exp = calloc(200, sizeof(char)),
       *env_tok = calloc(200, sizeof(char));

  while (src[i] != '\0') {
    if (src[i] == '$') {
      is_env = true;
      env_start = i+1;
    }

    if (is_env) {
      if (isspace(src[i]) || src[i] == '\0' || src[i] == '$') {
        env_end = i;
        strncpy(env_tok, src+env_start, env_end - env_start);
        printf("env_tok: %s\n", env_tok);
        printf("env expand: %s\n", getenv(env_tok));
        is_env = false;

      }
    } else {
      exp[j] = src[i];
      ++j;
    }
    ++i;
  }
  *dest = exp;
  printf("expanded: %s\n", exp);
}
