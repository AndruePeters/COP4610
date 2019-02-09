/*
  Rules for evaluating arguments
  1) Check to see if an alias exists, of so then expand
  2) Check to see if path exists
*/


#ifndef _GROUP12_SHELL_H_
#define _GROUP12_SHELL_H_

#include <glib.h>
#include <stdbool.h>
#include <builtins/shell_data.h>

#define RED_OUT_NONE (0)
#define RED_OUT_FILE (1)
#define RED_OUT_PIPE (2)

#define RED_IN_NONE (0)
#define RED_IN_FILE (1)
#define RED_IN_PIPE (2)

struct cmd {
  char **cmd;
  int num_cmd;

  char* red_in;
  char* red_out;

  int red_out_type;
  int red_in_type;

  bool built_in;
  bool background;
};

struct cmd_queue {
  GQueue* cq;
};

char* get_line();
void display_prompt(struct shell_data *sd);
void process_input(const char* input);

void init_cmd_queue(struct cmd_queue *cq);
void push_cmd(struct cmd_queue *cq, struct cmd *c);
void print_cmd_queue(struct cmd_queue *q);
void form_cmds(struct instruction **instr, struct cmd_queue *cq);
void proc_redirect_cmd(struct instruction *instr, struct cmd_queue *cq);
void free_cmd_queue_data(struct cmd_queue* q);
void print_args_in_cmd(const struct cmd *cmd);
void init_cmd(struct cmd* q);
void add_null_arg(struct cmd *cmd);
struct cmd* cmd_queue_get_last_cmd(struct cmd_queue* q);
struct cmd* cmd_queue_get_first_cmd(struct cmd_queue* q);
#endif
