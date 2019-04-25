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


  fileptr = fopen("fat32.img", "rb");
  fseek(fileptr, 0, SEEK_END);
  filelen = ftell(fileptr);
  rewind(fileptr);

  buffer = (uint8_t *)malloc( (filelen+1) * sizeof(char));
  fread(buffer, filelen, 1, fileptr);
  rewind(fileptr);
  load_fat_bpb(&bpb, fileptr);
  fclose(fileptr);

  for (i = 0; i < 50; ++i) {
    printf("0x%02x  ", buffer[i]);
    if (i % 5 == 0) printf("\n");
  }
  printf("\n\n");

  for (i = 0; i < 50; ++i) {
    printf("0x%02x  ", b_ptr[i]);
    if (i % 5 == 0) printf("\n");
  }
printf("Size of FAT32_BPB:\t%lu\nSize of FAT32_DIR:\t%lu\n", sizeof(struct fat_bpb), sizeof(struct fat_dir));
printf("\n\n");
dump_fat_bpb(&bpb);

  return 0;
}
