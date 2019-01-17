#include <string.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tokenize.h"

/*
  Tokenizes a c-string with whitespace as a delimiter.
  Tokens are added to instr_ptr->tokens.

  Input: instruction*
         char*
*/
void add_tokens(struct instruction* instr_ptr, char* line)
{
  char *token;
  char *saveptr = NULL;

  char temp[100];
  strcpy(temp, line);

  token = strtok_r(temp, " ", &saveptr);

  while (token != NULL) {
    //printf("token: %s\n", token);
    add_token(instr_ptr, token);
    token = strtok_r(NULL, " ", &saveptr);
  }
}

/*
  Adds a token to instr_ptr->token.
  Handles allocation and resizing.

  Input: instruction*
         char*
*/
void add_token(struct instruction* instr_ptr, char* tok)
{

  if (instr_ptr->num_tokens == 0) {
    instr_ptr->tokens = (char**)malloc(sizeof(char*));
    if (instr_ptr->tokens == NULL) {
      printf("MALLOC FAILED.\n");
    }
  }
  else {
    instr_ptr->tokens = (char**)realloc(instr_ptr->tokens, (instr_ptr->num_tokens+1) * sizeof(char*));
    if (instr_ptr->tokens == NULL) {
      printf("MALLOC FAILED.\n");
    }
  }


  instr_ptr->tokens[instr_ptr->num_tokens] = (char *)malloc( (strlen(tok)+1) * sizeof(char));
  strcpy(instr_ptr->tokens[instr_ptr->num_tokens], tok);
  ++instr_ptr->num_tokens;
}

/*
  Prints the tokens to terminal.
*/
void print_tokens(struct instruction* instr_ptr)
{
  if (instr_ptr->tokens == NULL)
    return;

  int i;
  printf("Tokens:\n");
  for (i = 0; i < instr_ptr->num_tokens; ++i)
    printf("#%s#\n", (instr_ptr->tokens)[i]);
}

/*
  Clears and frees memory for instruction*
*/
void clear_instruction(struct instruction* instr_ptr)
{
  /* Exit early if pointer is null */
  if (instr_ptr->tokens == NULL)
    return;

  int i;
  for (i = 0; i < instr_ptr->num_tokens; ++i)
    free(instr_ptr->tokens[i]);
  free(instr_ptr->tokens);

  instr_ptr->tokens = NULL;
  instr_ptr->num_tokens = 0;
}
