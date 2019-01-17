/*
  Andrue Peters
  1/15/18

  COP4610 - Group 12 - Proj 1

  This is a test file for the tokenize.c.
*/

#include <utility/tokenize.h>
#include <utility/tokenize.c>

int main()
{
  char * instructions[] = { "cd path1/path2/path3/path4", "echo this is my argument", "ls -al", NULL};
  struct instruction instr;

  int i;
  for (i = 0; instructions[i] != NULL; ++i) {
    printf("%s\n", instructions[i]);
    add_tokens(&instr, instructions[i]);
  }
  clear_instruction(&instr);


  //clear_instruction(&instr);

  return 0;
}
