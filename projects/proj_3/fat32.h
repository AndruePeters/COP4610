#include <stdint.h>

struct fat_dir {
  uint8_t DIR_Name[11];
  uint8_t DIR_Attr;
  uint8_t DIR_NTRes;

  uint8_t DIR_CrtTimeTenth;
  uint16_t DIR_CrtTime;
  uint16_t DIR_CrtDate;

  uint16_t DIR_LstAccDate;
  uint16_t DIR_FstClusHI;
  uint16_t DIR_WrtTime;
  uint16_t DIR_WrtDate;
  uint16_t DIR_FstClusLO;
  uint32_t DIR_FileSize;
} __attribute__((packed));

/*
  The following is found on page 9 of the provided fatspec.pdf
*/
struct fat_bpb {
  uint8_t BS_jmpBoot[3];
  uint8_t BS_OEMName[8];

  uint16_t BPB_BytsPerSec;
  uint8_t BPB_SecPerClus;
  uint16_t BPB_RsvdSecCnt;
  uint8_t BPB_NumFATs;
  uint16_t BPB_RootEntCnt;
  uint16_t BPB_TotSec16;
  uint8_t BPB_Media;
  uint16_t BPB_FATSz16; // must be 0 on fat32 systems
  uint16_t BPB_SecPerTrk;
  uint16_t BPB_NumHeads;
  uint64_t BPB_HiddSec;
  uint64_t BPB_TotSec32;

  // this is where FAT32 is different
  // the following are from page 12 of the documentation
  uint64_t BPB_FATSz32;
  uint16_t BPB_ExtFlags;
  uint16_t BPB_FSVer;
  uint64_t BPB_RootClus;
  uint16_t BPB_FSInfo;
  uint16_t BPB_BkBootSec;
  uint8_t BPB_Reserved[12];
  uint8_t BS_DrvNum;
  uint8_t BS_Reserved1;
  uint8_t BS_BootSig;
  uint64_t BS_VolID;
  uint8_t BS_VolLab[11];
  uint8_t BS_FilSysType[8];
} __attribute__((packed));

struct shell_data {
  char *pwd;
  char *img_name;
}
