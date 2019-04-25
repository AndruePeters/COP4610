#include "fat32.h"

void load_fat_bpb(struct fat_bpb* b, FILE *fp)
{
  fread(b, sizeof(struct fat_bpb), 1, fp);
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
  printf("BPB_FATSz32: %u\n", b->BPB_FATSz32);
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
  printf("DIR_FstClusHI: 0x%04x\n", d->DIR_FstClusHI);
  printf("DIR_WrtTime: %u\n", d->DIR_WrtTime);
  printf("DIR_WrtDate: %u\n", d->DIR_WrtDate);
  printf("DIR_FstClusLO: %0x04x\n", d->DIR_FstClusLO);
  printf("DIR_FileSize: %u\n", d->DIR_FileSize);
}
