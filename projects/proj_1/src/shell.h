/*
  Rules for evaluating arguments
  1) Check to see if an alias exists, of so then expand
  2) Check to see if path exists
*/


#ifndef _GROUP12_SHELL_H_
#define _GROUP12_SHELL_H_

#include <stdbool.h>
#include <shared.h>

char* get_line();
void display_prompt();
void process_input(const char* input);

bool expand_env(char **dest, const char *src);





#endif
