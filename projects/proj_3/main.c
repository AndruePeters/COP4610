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
  int i;
  unsigned root_dir;
  struct fat_dir d;
  struct dir_pos pos;
  fat.fp = fopen("fat32.img", "rb");
  fat.pos.cluster = 2;
  fseek(fat.fp, 0, SEEK_END);
  rewind(fat.fp);
  load_fat_bpb(&fat);

  load_fat_dir(&fat, &d, 2, 2);



  fat32_cd(&fat, "BLUE");
  //printf("\n\n\n");

  printf("\n\n");
  //fat32_cd(&fat, "RED");

  pos = fat32_get_dir_pos(&fat, "RED", 2);
  //printf("Red Cluster: %x\n\n", fat32_get_dir_pos(&fat, "RED", 2).cluster);
  //printf("BLUE Parent Cluster: %x\n\n", fat32_get_dir_pos(&fat, "..", fat32_get_dir_pos(&fat, "BLUE", 2).cluster).cluster);


  // Everything below this is for the final shell
  char *line;
  while (1) {
    printf("%s:$  ", "fat32.img");

    if (!(line = get_line())) {
      continue;
    }

    //fat.current_cluster = fat32_cd(&fat, line);
    fat32_ls(&fat, line);
    printf("\n\n");
  }

  free(line);
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
