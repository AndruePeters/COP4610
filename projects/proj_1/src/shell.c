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
#include <sys/wait.h>

#include <builtins/shell_data.h>
#include <builtins/cd.h>
#include <builtins/alias.h>
#include <builtins/echo.h>



#include <utility/path.h>
#include <utility/tokenize.h>

int my_exec(struct shell_data *sd, struct instruction* instr, char **cmd);
void init_builtin_function_pointer();
void load_cmdQ(GQueue *cmdQ, struct instructions *instr);

static GHashTable* builtins_table;
static GQueue *cmdQ;

#include "shell.h"

int main()
{
  init_builtin_function_pointer();
  char* line = NULL;
  struct shell_data sd;
  struct instruction instr;
  char **cmd = NULL;

  shell_data_init(&sd);
  instr.tokens = NULL;
  instr.num_tokens = 0;

  init_alias();

  while (1) {
    display_prompt(&sd);
    line = get_line();
    if (!line) continue;

    add_tokens(&instr, line);
    cmd = form_cmd(instr);
  /*  if (my_exec(&sd, &instr, cmd)) {

  }*/

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

int my_exec(struct shell_data *sd, struct instruction* instr, char **cmd)
{
  void (*p)() = g_hash_table_lookup(builtins_table, "echo");
  p("print $USER");

  print_tokens(instr);
  char *alias_expansion = expand_alias(instr->tokens[0]);
  if (!alias_expansion) {

  }
  pid_t child_pid, temp_pid;
  int child_status;
  char * arg[0];
  child_pid = fork();
  if (child_pid == 0) {
    execv("/bin/ls", arg);

    printf("Unkown command\n");
    exit(0);
  } else {
    do {
      temp_pid = waitpid(child_pid, &child_status, 0);

    } while (temp_pid != child_pid);
  }
  return child_status;
}

void display_prompt(struct shell_data *sd)
{
  static char hostname[255];
  gethostname(hostname, 255);
  printf("%s@%s:%s$ ", getenv("USER"), hostname, sd->pwd);
}

void init_builtin_function_pointer()
{
  builtins_table = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, NULL);

//  g_hash_table_replace(builtins_table, "alias", alias(char*));
  //g_hash_table_replace(builtins_table, "unalias", unalias);
  //g_hash_table_replace(builtins_table, "cd", cd);
  g_hash_table_replace(builtins_table, "echo", echo);
  //g_hash_table_replace(builtins_table, "exit", exit);
  //g_hash_table_replace(builtins_table, "ls", ls);


}

void load_cmdQ(GQueue *cmdQ, struct instructions *instr)
{
  int i = 0;
  my_cmd = {NULL, NULL, RED_OUT_NONE, NULL};
  for (i = 0; i < instr->num_tokens; ++i) {

  }
}

void form_cmds(struct instruction *instr, struct cmd_queue *cq)
{
  int rin[50];
  int rout[50];
  int i;


  struct cmd *c = calloc(1, sizeof(cmd));

  for(i = 1; i < instr->num_tokens; ++i) {
    char *t = (instr->tokens)[i];
    /* skip if it's the last */
    if (strcmp(t, "<") == 0 || strcmp(t, ">" == 0 || strcmp(t, "|") == 0) {
      continue;
    }

    /* These are the possible redirect options available */
    if (strcmp((instr->tokens)[i-1], "<") == 0) {
        c->red_in = strdup((instr->tokens)[i-1]);
        c->red_in_type = RED_IN_FILE;
    } else if (strcmp((instr->tokens)[i], ">") == 0) {
        c->red_out = strdup((instr->tokens)[i-1]);
        c->red_out_type = RED_OUT_FILE;
    } else if (strcmp((instr->tokens)[i], "|")) {
      /* For the case of a pipe, set the current output to pipe
         then create new cmd and set its input to pipe */
      c->red_out = strdup((instr->tokens)[i-1]);
      c->red_out_type = RED_OUT_PIPE;

      c = calloc(1, sizeof(cmd));

      c->red_in = strdup((instr->tokens)[i-1]);
      c->red_in_type = RED_IN_PIPE;
    } else {
      /* add the argument to c->cmd */
      ++(c->num_cmd);
      c->cmd = realloc(c->cmd, c->num_cmd * sizeof(char *));
    }
  }

}
