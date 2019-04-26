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

  dump_fat_bpb(&bpb);
  root_dir = first_sect_of_clus(&bpb, 2);
  load_fat_dir(&bpb, &d, fileptr, 2, 2);
  dump_fat_dir(&d);

  for( i = 0; i < 16; ++i) {
    load_fat_dir(&bpb, &d, fileptr, 2, i);
    if (d.DIR_Attr & ATTR_DIRECTORY) {
      if (strncmp(d.DIR_Name, "RED", 3) == 0) {
        printf("Dir_name: %.11s\n", d.DIR_Name);
        dump_fat_dir(&d);
        red = d;
      }
    }

  for (i = 0; i < 16; ++i) {

  }


  }
  red_clust = (red.DIR_FstClusHI << 16) | (red.DIR_FstClusLO);
  printf("Red cluster: %#x\n", red_clust);

  printf("getFatAddress(0x1bf): %#x\n", getFatAddressByCluster(&bpb, 0x21d));
  printf("getFatEntry(0x1bf): %#x\n", getFatEntry(&bpb, 0x21d, fileptr));
  printf("Byte entry : %#x\n", cluster_to_byte(&bpb, fat_get_next_clus(&bpb, fileptr, 0x1f9)));


  return 0;
}
