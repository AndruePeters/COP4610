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

#include "shell.h"

int my_exec(struct shell_data *sd, struct instruction* instr, char **cmd);
void init_builtin_function_pointer();
void add_arg(struct cmd *cm, const char *c);
//void load_cmdQ(GQueue *cmdQ, struct instruction *instr);

static GHashTable* builtins_table;
static struct cmd_queue cmdQ;



int main()
{
  init_builtin_function_pointer();
  init_cmd_queue(&cmdQ);

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
    form_cmds(&instr, cmdQ);
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
  const char *alias_expansion = expand_alias(instr->tokens[0]);
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

/*void load_cmdQ(GQueue *cmdQ, struct instructions *instr)
{
  int i = 0;
  my_cmd = {NULL, NULL, RED_OUT_NONE, NULL};
  for (i = 0; i < instr->num_tokens; ++i) {

  }
}*/

void form_cmds(struct instruction *instr, struct cmd_queue *cq)
{
  int i;
  int num_rin = 0;
  int num_rout = 0;


  struct cmd *c = calloc(1, sizeof(struct cmd));
  char *t;
  /*
    Pass 1: Store text and only form commands
  */
  for(i = 0; i < instr->num_tokens; ++i) {
    t = strdup((instr->tokens)[i]);


    /* skip the next input because its either input or output redirect */
    if (strcmp(t, "<") == 0 || strcmp(t, ">") == 0)  {
      ++i;
      free(t);
      continue;
    }

    /* skip this token, but dont' skip the next because it's a new command */
    if (strcmp(t, "|") == 0) {
      free(t);
      continue;
    }

    /* If the previous command is a pipe then start a new cmd */
    if (i > 0 && strcmp((instr->tokens)[i-1], "|") == 0) {
      add_cmd(&cmdQ, c);
      c = calloc(1, sizeof(struct cmd));
    }

    printf("t:%s\n", t);

    add_arg(c, t);
    printf("t:%s\nc->cmd[num_cmd-1]:%s\n", t, c->cmd[c->num_cmd-1]);
  }

}

void add_arg(struct cmd *cmd, const char *c)
{
  if (!c) return;

  if (cmd->num_cmd == 0) {
    cmd->cmd = calloc(1, sizeof(char*));
  } else {
    cmd->cmd = realloc(cmd->cmd, (cmd->num_cmd + 1) * sizeof(char*));
  }

  cmd->cmd[cmd->num_cmd] = calloc(strlen(c) + 1, sizeof(char));
  strcpy(cmd->cmd[cmd->num_cmd], c);
  ++(cmd->num_cmd);
}

void print_arg(const struct cmd *cmd)
{
  if(!cmd->cmd) return;

  int i;
  printf("argv:\n");
  for (i = 0; i < cmd->num_cmd; ++i) {
    printf("#%s\n", (cmd->cmd)[i]);
  }
}

void init_cmd_queue(struct cmd_queue *cq)
{
  cq->cq = g_queue_new();
}

void add_cmd(struct cmd_queue *cq, struct cmd *c)
{
  g_queue_push_tail(cq->cq, c);
}
