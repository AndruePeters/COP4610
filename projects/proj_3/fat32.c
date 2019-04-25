#include "fat32.h"

void load_fat_bpb(struct fat_bpb* b, FILE *fp)
{
  fread(b, sizeof(struct fat_bpb), 1, fp);
}

void dump_fat_bpb(struct fat_bpb*b)
{
  printf("BS_JMPBoot: %x %x %x\n", b->BS_jmpBoot[0], b->BS_jmpBoot[1], b->BS_jmpBoot[2]);
  printf("BS_OEMName: %.8s\n", b->BS_OEMName);
  printf("BPB_BytesPerSec: %u\n", b->BPB_BytsPerSec);
  printf("BPB_SecPerClus: %u\n", b->BPB_SecPerClus);
  printf("BPB_RsvdSecCnt: %u\n", b->BPB_RsvdSecCnt);
  printf("BPB_NumFATs: %u\n", b->BPB_NumFATs);
  printf("BPB_RootEntCnt: %u\n", b->BPB_RootEntCnt);
  printf("BPB_TotSec16: %u\n", b->BPB_TotSec16);
  printf("BPB_Media: %x\n", b->BPB_Media);
  printf("BPB_FATSz16: %x\n", b->BPB_FATSz16);
  printf("BPB_SecPerTrk: %u\n", b->BPB_SecPerTrk);
  printf("BPB_NumHeads: %u\n", b->BPB_NumHeads);
  printf("BPB_HiddSec: %x\n", b->BPB_HiddSec);
  printf("BPB_TotSec32: %u\n", b->BPB_TotSec32);
  printf("BPB_FATSz32: %u\n", b->BPB_FATSz32);
  printf("BPB_ExtFlags: %x\n", b->BPB_ExtFlags);
  printf("BPB_FSVer: %x\n", b->BPB_FSVer);
  printf("BPB_RootClus: %u\n", b->BPB_RootClus);
  printf("BPB_FSInfo: %x\n", b->BPB_FSInfo);
  printf("BPB_BkBootSec: %u\n", b->BPB_BkBootSec);
  printf("BPB_Reserved: %.12s\n", b->BPB_Reserved);
  printf("BS_DrvNum: %u\n", b->BS_DrvNum);
  printf("BS_Reserved1: %x\n", b->BS_Reserved1);
  printf("BS_BootSig: %x\n", b->BS_BootSig);
  printf("BS_VolID: %u\n", b->BS_VolID);
  printf("BS_VolLab: %.11s\n", b->BS_VolLab);
  printf("BS_FilSysType: %.8s\n", b->BS_FilSysType);
}
