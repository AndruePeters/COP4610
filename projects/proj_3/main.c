#include <stdio.h>
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
  fileptr = fopen("fat32.img", "rb");
  fseek(fileptr, 0, SEEK_END);
  filelen = ftell(fileptr);
  rewind(fileptr);

  buffer = (uint8_t *)malloc( (filelen+1) * sizeof(char));
  fread(buffer, filelen, 1, fileptr);
  fclose(fileptr);

  for (i = 0; i < 50; ++i) {
    printf("0x%02x  ", buffer[i]);
    if (i % 5 == 0) printf("\n");
  }

printf("Size of FAT32_BPB:\t%lu\nSize of FAT32_DIR:\t%lu\n", sizeof(struct fat_bpb), sizeof(struct fat_dir));
  return 0;
}
