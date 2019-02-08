/*
  Andrue Peters

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
void add_arg_to_cmd(struct cmd *cm, const char *c);


GHashTable* builtins_table;
struct cmd_queue cmdQ;



int main()
{
  init_builtin_function_pointer();
  init_cmd_queue(&cmdQ);

  char* line = NULL;
  struct shell_data sd;
  struct instruction instr;

  shell_data_init(&sd);
  instr.tokens = NULL;
  instr.num_tokens = 0;

  init_alias();


  while (1) {
    init_cmd_queue(&cmdQ);
    display_prompt(&sd);

    if (!(line = get_line())) {
      continue;
    }

    add_tokens(&instr, line);
    form_cmds(&instr, &cmdQ);
    print_cmd_queue(&cmdQ);
    free(line);
    free_cmd_queue_data(&cmdQ);
    clear_instruction(&instr);
  }

  return 0;
}

/*
  Reads a complete line from stdin and sends returns it.
*/
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

/*
  Processes tokenization to determine cmd properties.
*/
void form_cmds(struct instruction *instr, struct cmd_queue *cq)
{
  int i;

  struct cmd *c = calloc(1, sizeof(struct cmd));
  init_cmd(c);
  char *t;
  /*
    Pass 1: Store text and only form commands
  */
  for(i = 0; i < instr->num_tokens; ++i) {
    t = (instr->tokens)[i];

    /* skip the next input because its either input or output redirect */
    if (strcmp(t, "<") == 0 || strcmp(t, ">") == 0)  {
      ++i;
      continue;
    }

    /* skip this token, but dont' skip the next because it's a new command */
    if (strcmp(t, "|") == 0 && i > 0) {
      add_null_arg(c);
      push_cmd(&cmdQ, c);
      c = calloc(1, sizeof(struct cmd));
      init_cmd(c);
      continue;
    }

    add_arg_to_cmd(c, t);
  }
  add_null_arg(c);
  push_cmd(&cmdQ, c);
}

/*
  Adds c to cmd->cmd
*/
void add_arg_to_cmd(struct cmd *cmd, const char *c)
{
  if (!c) return;

  if (cmd->num_cmd == 0) {
    cmd->cmd = (char **)calloc(1, sizeof(char*));
  } else {
    cmd->cmd = realloc(cmd->cmd, (cmd->num_cmd + 1) * sizeof(char*));
  }

  cmd->cmd[cmd->num_cmd] = calloc(strlen(c) + 1, sizeof(char));
  cmd->cmd[cmd->num_cmd] = strdup(c);
  ++(cmd->num_cmd);
}

/*
  Forms a null terminated array that can be passed to excecv and other functions
*/
void add_null_arg(struct cmd *cmd)
{
  cmd->cmd = realloc(cmd->cmd, (cmd->num_cmd + 1) * sizeof(char*));
  cmd->cmd[cmd->num_cmd] = '\0';
}

/*
  Prints all arguments.
*/
void print_args_in_cmd(const struct cmd *cmd)
{
  if(!cmd) return;
  if(!cmd->cmd) return;

  int i;
  for(i = 0; i < cmd->num_cmd; ++i) {
    printf("%s\t", (cmd->cmd)[i]);
  }
  printf("\n");
}

/*
  Initializes GQueue inside of struct cmd_queue
*/
void init_cmd_queue(struct cmd_queue *cq)
{
  cq->cq = g_queue_new();
}

/*
  Addsa a struct cmd* to the command queue *cq
*/
void push_cmd(struct cmd_queue *cq, struct cmd *c)
{
  g_queue_push_tail(cq->cq, c);
}


void print_cmd_queue(struct cmd_queue *q)
{
  g_queue_foreach(q->cq, (GFunc)print_args_in_cmd, NULL);
}

//void free_cmd(struct cmd *c)
void free_cmd(gpointer gp)
{
  struct cmd *c = gp;
  int i;
  for (i = 0; i < c->num_cmd; ++i) {
    free((c->cmd)[i]);
    (c->cmd)[i] = NULL;
  }
  free(c->cmd);
  c->cmd = NULL;

  if (c->red_out_type) {
    free(c->red_out);
    c->red_out = NULL;
  }

  if (c->red_in_type) {
    free(c->red_in);
    c->red_in = NULL;
  }

  c->num_cmd = 0;
  free(c);
  c = NULL;
}

/*
  Frees all memory accociated with q->cq
*/
void free_cmd_queue_data(struct cmd_queue * q)
{
  g_queue_free_full(q->cq, free_cmd);
}

void init_cmd(struct cmd* q)
{
  if (!q)
  q->cmd = NULL;
  q->num_cmd = 0;
  q->red_in = NULL;
  q->red_out = NULL;
  q->red_out_type = 0;
  q->red_in_type = 0;
  q->background = NULL;
}
