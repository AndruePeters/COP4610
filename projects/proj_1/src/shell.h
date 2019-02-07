/*
  Rules for evaluating arguments
  1) Check to see if an alias exists, of so then expand
  2) Check to see if path exists
*/


#ifndef _GROUP12_SHELL_H_
#define _GROUP12_SHELL_H_

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

  bool background;
};

struct cmd_queue {
  GQueue* cq;
}

char* get_line();
void display_prompt(struct shell_data *sd);
void process_input(const char* input);






#endif
