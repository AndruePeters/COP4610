/*
  Tokenizes a string.
  Based on code provided by TA for project 1.
*/
#ifndef _GROUP12_UTILITY_TOKENIZE_H_
#define _GROUP12_UTILITY_TOKENIZE_H_



struct instruction {
  char** tokens;
  int num_tokens;
};

/*
  Tokenizes input from a char array into (*instr_ptr).
*/
void add_tokens(instruction* instr_ptr, char* line);

/*
  Simply handles adding a token to (*instr_ptr).
*/
void add_token(insruction* instr_ptr, char* tok);

/*
  Dumps tokens to stdout.
*/
void print_tokens(instruction* instr_ptr);

/*
  Clears instr_ptr and frees memory.
*/
void clear_instruction(instruction* instr_ptr);


#endif
