#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "fat32.h"
#include "fat32_masks.h"

int main()
{
  FILE *fileptr;
  int i;
  struct fat_bpb bpb;
  uint8_t *b_ptr = (char *)&bpb;
  unsigned root_dir;
  struct fat_dir d, red;
  uint32_t red_clust;

  fileptr = fopen("fat32.img", "rb");
  fseek(fileptr, 0, SEEK_END);
  rewind(fileptr);


  rewind(fileptr);
  load_fat_bpb(&bpb, fileptr);

  root_dir = first_sect_of_clus(&bpb, 2);
  load_fat_dir(&bpb, &d, fileptr, 2, 2);


  for( i = 0; i < 16; ++i) {
    load_fat_dir(&bpb, &d, fileptr, 2, i);
    if (d.DIR_Attr & ATTR_DIRECTORY) {
      if (strncmp(d.DIR_Name, "RED", 3) == 0) {
        printf("Dir_name: %.11s\n", d.DIR_Name);
        dump_fat_dir(&d);
        red = d;
      }
    }
  }

  uint32_t addr = 0x1b4;
  printf("addr: %#x\n", fat_get_next_clus(&bpb, fileptr, addr));
  while ( addr != 0xFFFFFFF) {
    for (i = 0; i < 16; ++i) {
      load_fat_dir(&bpb, &d, fileptr, addr, i);
      if (d.DIR_Attr & ATTR_DIRECTORY)
        printf("Dir_name: %.11s\n", d.DIR_Name);
    }
    addr = fat_get_next_clus(&bpb, fileptr, addr);
  }



  return 0;
}
