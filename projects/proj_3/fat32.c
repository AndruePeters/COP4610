#include "fat32.h"


void load_fat_bpb(struct fat_bpb* b, FILE *fp)
{
  fread(b, sizeof(struct fat_bpb), 1, fp);
}

void load_fat_dir(struct fat_bpb* b, struct fat_dir *d, FILE *fp, unsigned cluster, unsigned offset)
{
    offset *= 32;
    fseek(fp, cluster_to_byte(b, cluster) + offset, SEEK_SET);
    fread(d, sizeof(struct fat_dir), 1, fp);
}

void load_entry(struct fat_bpb*b, struct fat_dir *d, FILE *fp, unsigned cluster, unsigned offset)
{
  offset = offset *32 + 32;
  fseek(fp, cluster_to_byte(b, cluster), SEEK_SET);
  fread(d, sizeof(struct fat_dir), 1, fp);
}

void dump_fat_bpb(const struct fat_bpb*b)
{
  printf("BS_JMPBoot: 0x%02x 0x%02x 0x%02x\n", b->BS_jmpBoot[0], b->BS_jmpBoot[1], b->BS_jmpBoot[2]);
  printf("BS_OEMName: %.8s\n", b->BS_OEMName);
  printf("BPB_BytesPerSec: %u\n", b->BPB_BytsPerSec);
  printf("BPB_SecPerClus: %u\n", b->BPB_SecPerClus);
  printf("BPB_RsvdSecCnt: %u\n", b->BPB_RsvdSecCnt);
  printf("BPB_NumFATs: %u\n", b->BPB_NumFATs);
  printf("BPB_RootEntCnt: %u\n", b->BPB_RootEntCnt);
  printf("BPB_TotSec16: %u\n", b->BPB_TotSec16);
  printf("BPB_Media: 0x%02x\n", b->BPB_Media);
  printf("BPB_FATSz16: 0x%x\n", b->BPB_FATSz16);
  printf("BPB_SecPerTrk: %u\n", b->BPB_SecPerTrk);
  printf("BPB_NumHeads: %u\n", b->BPB_NumHeads);
  printf("BPB_HiddSec: 0x%x\n", b->BPB_HiddSec);
  printf("BPB_TotSec32: %u\n", b->BPB_TotSec32);
  printf("BPB_FATSz32: %#x\n", b->BPB_FATSz32);
  printf("BPB_ExtFlags: 0x%x\n", b->BPB_ExtFlags);
  printf("BPB_FSVer: 0x%x\n", b->BPB_FSVer);
  printf("BPB_RootClus: %u\n", b->BPB_RootClus);
  printf("BPB_FSInfo: 0x%x\n", b->BPB_FSInfo);
  printf("BPB_BkBootSec: %u\n", b->BPB_BkBootSec);
  printf("BPB_Reserved: %.12s\n", b->BPB_Reserved);
  printf("BS_DrvNum: %u\n", b->BS_DrvNum);
  printf("BS_Reserved1: 0x%x\n", b->BS_Reserved1);
  printf("BS_BootSig: 0x%x\n", b->BS_BootSig);
  printf("BS_VolID: %u\n", b->BS_VolID);
  printf("BS_VolLab: %.11s\n", b->BS_VolLab);
  printf("BS_FilSysType: %.8s\n", b->BS_FilSysType);
}


void dump_fat_dir(const struct fat_dir *d)
{
  printf("DIR_Name: %.11s\n", d->DIR_Name);
  printf("DIR_Attr: 0x%02x\n", d->DIR_Attr);
  printf("DIR_NTRes: 0x%02x\n", d->DIR_NTRes);
  printf("DIR_CrtTimeTenth: %u\n", d->DIR_CrtTimeTenth);
  printf("DIR_CrtTime: %u\n", d->DIR_CrtTime);
  printf("DIR_CrtDate: %u\n", d->DIR_CrtDate);
  printf("DIR_LstAccDate: %u\n", d->DIR_LstAccDate);
  printf("DIR_FstClusHI: %#x\n", d->DIR_FstClusHI);
  printf("DIR_WrtTime: %u\n", d->DIR_WrtTime);
  printf("DIR_WrtDate: %u\n", d->DIR_WrtDate);
  printf("DIR_FstClusLO: %#x\n", d->DIR_FstClusLO);
  printf("DIR_FileSize: %u\n", d->DIR_FileSize);
}


void fat32_ls(const struct fat_bpb *b, const char* dir)
{
  // this prints the root directory
  /*for( i = 0; i < 16; ++i) {
    load_fat_dir(&bpb, &d, fileptr, 2, i);
    if (d.DIR_Attr & ATTR_DIRECTORY)
      printf("Dir_name: %.11s\n", d.DIR_Name);
  }*/

}


unsigned first_data_sector(const struct fat_bpb*b)
{
  return b->BPB_RsvdSecCnt + (b->BPB_NumFATs * b->BPB_FATSz32) + root_dir_sectors(b);
}


unsigned root_dir_sectors(const struct fat_bpb *b)
{
  return (( b->BPB_RootEntCnt * 32) + (b->BPB_BytsPerSec -1 )) / b->BPB_BytsPerSec;
}

unsigned first_sect_of_clus(const struct fat_bpb *b, unsigned clust_num)
{
  return ((clust_num - 2) * b->BPB_SecPerClus) + first_data_sector(b);
}

unsigned sector_to_byte(const struct fat_bpb *b, unsigned sect_num)
{
  return sect_num * b->BPB_SecPerClus;
}

unsigned cluster_to_byte(const struct fat_bpb *b, unsigned clust_num)
{
  return first_sect_of_clus(b, clust_num) * b->BPB_BytsPerSec;
}

uint32_t fat_entry(const struct fat_bpb *b, FILE *fp, unsigned clust_num)
{
  uint32_t entry;
  fseek(fp, fat_address(b, clust_num), SEEK_SET);
  fread(&entry, 4, 1, fp);
  return entry;
}

/*
  Returns the byte address for the img file for the cluster number
*/
uint32_t fat_address(const struct fat_bpb *b, uint32_t cluster)
{
  uint32_t fat_offset = cluster * 4;
  uint32_t fat_sec_num = b->BPB_RsvdSecCnt + (fat_offset / b->BPB_BytsPerSec);
  uint32_t fat_ent_offset = fat_offset % b->BPB_BytsPerSec;
  return (fat_sec_num * b->BPB_BytsPerSec) + fat_ent_offset;
}

/*
  Returns the next fat value from the fat table.
*/
uint32_t fat_get_next_clus(const struct fat_bpb*b, FILE *fp, uint32_t curr_clus)
{
  uint32_t fat_ent;
  uint32_t offset = curr_clus * 4; // increment by 4 bytes
  fseek(fp, fat_address(b, curr_clus), SEEK_SET);
  fread(&fat_ent, sizeof(uint32_t), 1, fp);
  return fat_ent;
}
