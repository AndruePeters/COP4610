#include <string.h>
#include <string.h>
#include <stdlib.h>

#include "tokenize.h"

/*
  Tokenizes a c-string with whitespace as a delimiter.
  Tokens are added to instr_ptr->tokens.

  Input: instruction*
         char*
*/
void add_tokens(instruction* instr_ptr, char* line)
{
  char *token, *temp;
  char *saveptr = NULL; // used for strtok_r

  // get first token
  token = strtok_r(line, "%ms", &saveptr);

  while (token = strtok_r(line, "%ms", &saveptr)) {
    add_token(instr_ptr, token);
  }
}

/*
  Adds a token to instr_ptr->token.
  Handles allocation and resizing.

  Input: instruction*
         char*
*/
void add_token(instruction* instr_ptr, char* tok)
{
  // extend token array to accomodate an additional token
  if (instr_ptr->num_tokens == 0)
    instr_ptr->tokens = (char**)malloc(sizeof(char*));
  else
    instr_ptr->tokens = (char**)realloc(instr_ptr->tokens, (instr_ptr->num_tokens+1) * sizeof(char*));

  // allocate char array for new token in new slot
  instr_ptr->tokens[instr_ptr->num_tokens] = (char *)malloc( (strlen(tok)+1) * sizeof(char));
  strcpy(instr_ptr->tokens[instr_ptr->num_tokens], tok);
  ++instr_ptr->num_tokens;
}

/*
  Prints the tokens to terminal.
*/
void print_tokens(instruction* instr_ptr)
{
  int i;
  printf("Tokens:\n");
  for (i = 0; i < instr_ptr->num_tokens; ++i)
    printf("#%s#\n", (instr_ptr->tokens)[i]);
}

/*
  Clears and frees memory for instruction*
*/
void clear_instruction(instruction* instr_ptr)
{
  int i;
  for (i = 0; i < instr_ptr->num_tokens; ++i)
    free(instr_ptr->tokens[i]);
  free(instr_ptr->tokens);

  instr_ptr->tokens = NULL;
  instr_ptr->num_tokens = 0;
}
