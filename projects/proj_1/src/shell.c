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
#include <builtins/cd.h>
#include <builtins/alias.h>
#include <builtins/echo.h>



#include <utility/path.h>
#include <utility/tokenize.h>

bool my_exec(struct shell_data *sd, struct instruction* instr);

#include "shell.h"

int main()
{
  char* line = NULL;
  struct shell_data sd;
  struct instruction instr;

  shell_data_init(&sd);
  instr.tokens = NULL;
  instr.num_tokens = 0;

  init_alias();

  while (1) {
    display_prompt(&sd);
    line = get_line();

    if (!line) continue;
    add_tokens(&instr, line);


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

bool my_exec(struct shell_data *sd, struct instruction* instr)
{

}

void display_prompt(struct shell_data *sd)
{
  static char hostname[255];
  gethostname(hostname, 255);
  printf("%s@%s:%s$ ", getenv("USER"), hostname, sd->pwd);
}
