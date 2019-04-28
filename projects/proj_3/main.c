#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h> // used for function hash table

#include "path.h"
#include "tokenize.h"
#include "fat32.h"
#include "fat32_masks.h"


/*
 *  Returns what the user typed in.
 */
char *get_line();

int main()
{
  struct fat32_info fat;
  struct instruction *instr;
  instruction_init(&instr);
  fat.fp = fopen("fat32.img", "rb");
  load_fat_bpb(&fat);
  fat.pos.cluster = fat.b.BPB_RootClus;




  char *line;
  bool keep_running = true;
  while (keep_running) {
    printf("%s:$  ", "fat32.img");

    if (!(line = get_line())) {
      continue;
    }
    add_tokens(instr, line);

    if (strcmp(instr->tokens[0], "exit") == 0) {
      keep_running = false;
      printf("Thank you for using our fat32 system.\nHave a nice day.\n");
    } else if (strcmp(instr->tokens[0], "info") == 0) {
      printf("Here is the bpb information for you\n\n");
      dump_fat_bpb(&fat);
    } else if (strcmp(instr->tokens[0], "ls") == 0) {
      if (instr->num_tokens < 2)
        fat32_ls(&fat, ".");
      else
        fat32_ls(&fat, instr->tokens[1]);
    } else if (strcmp(instr->tokens[0], "cd") == 0) {
      if (instr->num_tokens < 2)
        fat32_cd(&fat, ".");
      else
        fat32_cd(&fat, instr->tokens[1]);
    } else if (strcmp(instr->tokens[0], "size") == 0) {

    } else if (strcmp(instr->tokens[0], "creat") == 0) {

    } else if (strcmp(instr->tokens[0], "mkdir") == 0) {

    } else if (strcmp(instr->tokens[0], "open") == 0) {

    } else if (strcmp(instr->tokens[0], "close") == 0) {

    } else if (strcmp(instr->tokens[0], "read") == 0) {

    } else if (strcmp(instr->tokens[0], "write") == 0) {

    } else if (strcmp(instr->tokens[0], "rm") == 0) {

    } else if (strcmp(instr->tokens[0], "rmdir" ) == 0) {

    }

    free(line);
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
