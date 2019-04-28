/*
  Function prototypes and structs for fat32
  http://www.cs.uni.edu/~diesburg/courses/cop4610_fall10/week11/week11.pdf
*/

#ifndef _GROUP_15_FAT32_H_
#define _GROUP_15_FAT32_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "fat32_masks.h"
#include <ctype.h>


struct dir_pos {
  uint32_t cluster;
  unsigned offset;
};

struct fat_dir {
  char DIR_Name[11];
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
  char BS_OEMName[8];

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
  uint32_t BPB_HiddSec;
  uint32_t BPB_TotSec32;

  // this is where FAT32 is different
  // the following are from page 12 of the documentation
  uint32_t BPB_FATSz32;
  uint16_t BPB_ExtFlags;
  uint16_t BPB_FSVer;
  uint32_t BPB_RootClus;
  uint16_t BPB_FSInfo;
  uint16_t BPB_BkBootSec;
  uint8_t BPB_Reserved[12];
  uint8_t BS_DrvNum;
  uint8_t BS_Reserved1;
  uint8_t BS_BootSig;
  uint32_t BS_VolID;
  char BS_VolLab[11];
  uint8_t BS_FilSysType[8];
} __attribute__((packed));

struct fat_fsi {
  uint32_t FSI_LeadSig;
  uint8_t  FSI_Reserved1[480];
  uint32_t FSI_StrucSig;
  uint32_t FSI_Free_Count;
  uint32_t FSI_Nxt_Free;
  uint8_t  FSI_Reserved2[12];
  uint32_t FSI_TrailSig;
};

struct fat32_info {
  struct fat_bpb b;
  FILE *fp;
  struct dir_pos pos;
  char path[4000];
};

struct shell_env {
  char *pwd;
  char *img_name;
};





/*
 * Loads the bpb using fp.
 * Assumptions:
 *   fp is not null and is already opened.
 *   fp no longer points at the beginning of the file
*/
void load_fat_bpb(struct fat32_info *f);

void load_fat_dir(const struct fat32_info *f, struct fat_dir *d, unsigned cluster, unsigned offset);

/*
 * Dumps the information in b
 * Assumptions:
 *  b is not null
*/
void dump_fat_bpb(const struct fat32_info *f);

/*
 * Dumps the information in d
 * Assumptions:
 *  d is not null and valid dir object
*/
void dump_fat_dir(const struct fat_dir *d);


void fat32_ls(const struct fat32_info *f, const char* dir);
uint32_t fat32_cd(struct fat32_info *f, const char* dir);
void fat32_size(const struct fat32_info *f, const char* dir);
void fat32_info(const struct fat32_info *f);
void fat32_exit(struct fat32_info *f);
void fat32_get_dir_name(struct fat_dir *d, char *buffer);

/*
 * Prints the name of the directory.
 */
 void fat32_print_dir(struct fat32_info *f, const struct fat_dir *d);

/******************************************************************************/
/*          Utility functions                                                 */
/******************************************************************************/
unsigned first_data_sector(const struct fat32_info *f);
unsigned root_dir_sector(const struct fat32_info *f);
unsigned root_dir_sectors(const struct fat32_info *f);
unsigned first_sect_of_clus(const struct fat32_info *f, unsigned clust_num);
unsigned sector_to_byte(const struct fat32_info *f, unsigned sect_num);
unsigned cluster_to_byte(const struct fat32_info *f, unsigned clust_num);
uint32_t fat_entry(const struct fat32_info *f  ,unsigned clust_num);
uint32_t fat_address(const struct fat32_info *f, uint32_t cluster);
uint32_t fat_get_next_clus(const struct fat32_info *f, uint32_t curr_clus);
struct dir_pos fat32_get_curr_dir_pos(const struct fat32_info *f, const char *dir, uint32_t curr_clus);
struct dir_pos fat32_get_dir_pos(const struct fat32_info *f, const char *dir, uint32_t curr_clus);
bool fat32_is_dir(const struct fat_dir *d);


#endif
