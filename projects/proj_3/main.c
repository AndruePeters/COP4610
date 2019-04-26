#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "fat32.h"
#include "fat32_masks.h"

int main()
{
  FILE *fileptr;
  uint8_t *buffer;
  long filelen;
  int i;
  struct fat_bpb bpb;
  uint8_t *b_ptr = (char *)&bpb;
  unsigned root_dir;
  struct fat_dir d, red;

  fileptr = fopen("fat32.img", "rb");
  fseek(fileptr, 0, SEEK_END);
  filelen = ftell(fileptr);
  rewind(fileptr);

  buffer = (uint8_t *)malloc( (filelen+1) * sizeof(char));
  fread(buffer, filelen, 1, fileptr);
  rewind(fileptr);
  load_fat_bpb(&bpb, fileptr);

  dump_fat_bpb(&bpb);
  root_dir = first_sect_of_clus(&bpb, 2);
  load_fat_dir(&bpb, &d, fileptr, 2, 2);
  dump_fat_dir(&d);

  for( i = 0; i < 16; ++i) {
    load_fat_dir(&bpb, &d, fileptr, 2, i);
    if (d.DIR_Attr & ATTR_DIRECTORY)
      printf("Dir_name: %.11s\n", d.DIR_Name);
  }


  printf("First data sector: 0x%x\n", first_data_sector(&bpb));
  printf("Root dir sector: %x\n", cluster_to_byte(&bpb, 3));


  return 0;
}
