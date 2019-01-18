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
void add_tokens(struct instruction* instr_ptr, const char* line)
{
  char *token, *cpy;
  char *saveptr = NULL;

  cpy = (char *)malloc( (strlen(line)+1) * sizeof(char));
  strcpy(cpy, line);

  token = strtok_r(cpy, " ", &saveptr);

  while (token != NULL) {
    add_token(instr_ptr, token);
    token = strtok_r(NULL, " ", &saveptr);
  }
  free(cpy);
}

/*
  Adds a token to instr_ptr->token.
  Handles allocation and resizing.

  Input: instruction*
         char*
*/
void add_token(struct instruction* instr_ptr, char* tok)
{
  char **old_tok = instr_ptr->tokens;

  if (instr_ptr->num_tokens == 0) {
    if (! (instr_ptr->tokens = (char**) malloc(sizeof(char*)))) {
      fprintf(stderr, "Initial allocation in add_token failed for %s.\n", tok);
      instr_ptr->tokens = old_tok;
      return;
    }
    instr_ptr->tokens = (char**)malloc(sizeof(char*));
  } else {
    if (! (instr_ptr->tokens = (char**)realloc(instr_ptr->tokens, (instr_ptr->num_tokens+1) * sizeof(char*)))) {
      fprintf(stderr, "Realloc failed in add_token for %s. \n", tok);
      instr_ptr->tokens = old_tok;
    }
  }


  instr_ptr->tokens[instr_ptr->num_tokens] = (char *)malloc( (strlen(tok)+1) * sizeof(char));
  strcpy(instr_ptr->tokens[instr_ptr->num_tokens], tok);
  instr_ptr->num_tokens++;
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
