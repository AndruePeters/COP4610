/*
  Andrue Peters
  Main file.
  Got messy at the end, so code can be factored out.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <glib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <builtins/shell_data.h>
#include <builtins/cd.h>
#include <builtins/alias.h>
#include <builtins/echo.h>
#include <builtins/exit.h>
#include <builtins/clear.h>



#include <utility/path.h>
#include <utility/tokenize.h>

#include "shell.h"

void my_exec(struct shell_data *sd, struct cmd_queue* cmdq);
void init_builtin_function_pointer();
void add_arg_to_cmd(struct cmd *cm, const char *c);

/* Hashtable for built in functions */
GHashTable* builtins_table;
struct cmd_queue cmdQ;
struct shell_data sd;

/*
  Main routine.
*/
int main(int argc, char **argv)
{
  init_builtin_function_pointer();
  init_cmd_queue(&cmdQ);

  char* line = NULL;

  struct instruction *instr;

  instruction_init(&instr);
  shell_data_init(&sd);


  init_alias();
  add_alias("omw", "on my | way");

  while (1) {
    init_cmd_queue(&cmdQ);
    display_prompt(&sd);

    if (!(line = get_line())) {
      continue;
    }

    add_tokens(instr, line);
    form_cmds(&instr, &cmdQ);
    proc_redirect_cmd(instr, &cmdQ);
    set_cmd_path_and_type(&cmdQ);
    my_exec(&sd, &cmdQ);
    free(line);
    free_cmd_queue_data(&cmdQ);
    clear_instruction(instr);
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


/*
  Displays the prompt
*/
void display_prompt(struct shell_data *sd)
{
  static char hostname[255];
  gethostname(hostname, 255);
  printf("%s@%s:%s$ ", getenv("USER"), hostname, getenv("PWD"));
}

/*
  Creates hash table with entries pointing text to function pointer
*/
void init_builtin_function_pointer()
{
  builtins_table = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, NULL);
  g_hash_table_replace(builtins_table, "alias", alias);
  g_hash_table_replace(builtins_table, "unalias", unalias);
  g_hash_table_replace(builtins_table, "cd", cd);
  g_hash_table_replace(builtins_table, "echo", echo);
  g_hash_table_replace(builtins_table, "exit", my_exit);
  g_hash_table_replace(builtins_table, "clear", clear);
}

/*
  Processes tokenization to determine cmd properties.
*/
void form_cmds(struct instruction **instr, struct cmd_queue *cq)
{
  int i;
  bool new_cmd = true;

  struct cmd *c = calloc(1, sizeof(struct cmd));
  init_cmd(c);
  char *t;

  for(i = 0; i < (*instr)->num_tokens; ++i) {
    t = ((*instr)->tokens)[i];

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
      new_cmd = true;
      continue;
    }
    if (new_cmd && exists_alias(t)) {
      t = (char*)expand_alias(t);
      add_tokens_pos(instr, t, &i);
      t = ((*instr)->tokens)[i];
    }
    new_cmd = false;
    add_arg_to_cmd(c, t);
  }

  add_null_arg(c);
  push_cmd(&cmdQ, c);
}

/*
  Sets information for each command for pipes and redirects
*/
void proc_redirect_cmd(struct instruction *instr, struct cmd_queue *cq)
{
  int i;
  char *t;
  struct cmd *c;

  GList* walk = g_queue_peek_head_link(cq->cq);
  c = walk->data;

  for (i = 0; i < instr->num_tokens; ++i) {
    t = instr->tokens[i];

    if (strcmp(t, "<") == 0) {
      c = cmd_queue_get_first_cmd(cq);
      c->red_in = strdup(instr->tokens[i+1]);
      c->red_in_type = RED_IN_FILE;
    } else if (strcmp(t, ">") == 0) {
      c = cmd_queue_get_last_cmd(cq);
      c->red_out = strdup(instr->tokens[i+1]);
      c->red_out_type = RED_OUT_FILE;
    } else if (strcmp(t, "|") == 0) {
      c->red_out_type = RED_OUT_PIPE;
      walk = walk->next;
      c = walk->data;
      c->red_in_type = RED_IN_PIPE;
    }
  }
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

/*
  Debug method to print info for all struct cmd in cmd_queue
*/
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

/*
  Initializes values in cmd
*/
void init_cmd(struct cmd* q)
{
  if (!q)
  q->cmd = NULL;
  q->num_cmd = 0;
  q->red_in = NULL;
  q->red_out = NULL;
  q->red_out_type = 0;
  q->red_in_type = 0;
  q->built_in = false;
  q->background = false;
}

/*
  Returns first struct cmd in cmd_queue
*/
struct cmd* cmd_queue_get_first_cmd(struct cmd_queue* q)
{
  return g_queue_peek_tail(q->cq);
}

/*
  Returns data for last struct cmd in q
*/
struct cmd* cmd_queue_get_last_cmd(struct cmd_queue* q)
{
  return g_queue_peek_tail(q->cq);
}

/*
  Finds path of file and sets cmd->built_in for all cmd in cmd_queue
*/
void set_cmd_path_and_type(struct cmd_queue* q)
{
  GList *walk = g_queue_peek_head_link(q->cq);
  struct cmd* c;
  char *path;

  while (walk) {
    c = walk->data;
    if (g_hash_table_contains(builtins_table, (c->cmd)[0])) {
      c->built_in = true;
    } else {
      /* not a built in */
      c->built_in = false;

      /* check to see if what they typed in is a file */
      path = get_full_path(c->cmd[0]);

      if(is_file(path)) {
        cmd_overwrite_filename(walk->data, path);
      } else {
        /* does not exist in current directory, so let's check $PATH */
        free(path);
        path = expand_path(c->cmd[0]);
        if(!is_file(path)) {
          printf("Invalid command, human. Prepare for the consequences because I am not programmed to stop.\n");
          free(path);
        } else {
          cmd_overwrite_filename(walk->data, path);
        }

    }

    }
    walk = walk->next;
  }
}

/*
  Overwrite cmd[0] with full file path
*/
void cmd_overwrite_filename(struct cmd* c, char* new_name)
{
  free(c->cmd[0]);
  c->cmd[0] = new_name;
}

/*
  Handles execution for all cmd in cmd_queue
*/
void my_exec(struct shell_data *sd, struct cmd_queue *cmdq)
{
  GList *walk;
  struct cmd *c;
  walk = g_queue_peek_head_link(cmdq->cq);
  void (*exec_func)(struct cmd *c);

  int fdi=0, fdo=0, fdi_save=0, fdo_save=0;

  while (walk) {
    c = walk->data;
    exec_func = (c->built_in == true) ? builtin_exec : ext_exec;

    exec_func(c);
    walk = walk->next;
  }
}

/*
  Runs built in commands
*/
void builtin_exec(struct cmd *c)
{
  int fdi_save=0, fdo_save=0;
  cmd_red_input_open(c, &fdi_save);
  cmd_red_output_open(c, &fdo_save);

  void (*fptr)();
  fptr = g_hash_table_lookup(builtins_table, (c->cmd)[0]);
  (*fptr)(c->num_cmd, (c->cmd));
  cmd_red_input_close(c, &fdi_save);
  cmd_red_output_close(c, &fdo_save);
}

/*
  runs external commands
*/
void ext_exec(struct cmd *c)
{
  int fdi_save=0, fdo_save=0;
  int status;

  pid_t pid = fork();
  if (pid == -1) {
    // error
    printf("What the fork Batman? Something went wrong with fork()\n");
    exit(1);
  } else if (pid == 0) {
    // child

    /* Redirect in child only, so parent doesn't have to do anything. */
    cmd_red_output_open(c, &fdo_save);
    cmd_red_input_open(c, &fdi_save);
    execv(c->cmd[0], c->cmd);


  } else {
    // parent
    waitpid(pid, &status, 0);
  }
}

/*
  Opens new stdin
*/
bool cmd_red_input_open(struct cmd *c, int *saved)
{
  bool ret = false;
  int fdi;
  if (c->red_in_type == RED_IN_FILE) {
    fdi = open(c->red_in, O_RDONLY);
    ret = (fdi == -1) ? false : true;
    if ( ret == true) {
      *saved = dup(STDIN_FILENO);
      dup2(fdi, STDIN_FILENO);
    }
  }

  return ret;
}

/*
  Opens new stdout
*/
bool cmd_red_output_open(struct cmd* c, int *saved)
{
  int fdo;
  bool ret = false;
  if (c->red_out_type == RED_OUT_FILE) {
    fdo = open(c->red_out, O_CREAT|O_WRONLY|O_TRUNC, 0644);
    ret = (fdo < 0) ? false: true;
    if (ret == true) {
      saved = dup(STDOUT_FILENO);
      dup2(fdo, STDOUT_FILENO);
    }
  }
  return ret;
}

/*
  Closes and changes stdin
*/
void cmd_red_input_close(struct cmd *c, int *saved)
{
  if (c->red_in_type != RED_IN_FILE) { return; }
  dup2(*saved, STDIN_FILENO);
}

/*
  Closes current stdout and switches to next
*/
void cmd_red_output_close(struct cmd* c, int *saved)
{
  if (c->red_out_type != RED_OUT_FILE) {return; }
  dup2(*saved, STDOUT_FILENO);
}
