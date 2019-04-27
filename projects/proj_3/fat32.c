#include "fat32.h"
#include "path.h"
#include <glib.h>

void load_fat_bpb(struct fat32_info *f)
{
  fread(&f->b, sizeof(struct fat_bpb), 1, f->fp);
}

void load_fat_dir(const struct fat32_info *f, struct fat_dir *d, unsigned cluster, unsigned offset)
{
    offset *= 32;
    fseek(f->fp, cluster_to_byte(f, cluster) + offset, SEEK_SET);
    fread(d, sizeof(struct fat_dir), 1, f->fp);
}

// void load_entry(struct fat_bpb*b, struct fat_dir *d, FILE *fp, unsigned cluster, unsigned offset)
// {
//   offset = offset *32 + 32;
//   fseek(fp, cluster_to_byte(f, cluster), SEEK_SET);
//   fread(d, sizeof(struct fat_dir), 1, fp);
// }

void dump_fat_bpb(const struct fat32_info *f)
{
  printf("BS_JMPBoot: 0x%02x 0x%02x 0x%02x\n", f->b.BS_jmpBoot[0], f->b.BS_jmpBoot[1], f->b.BS_jmpBoot[2]);
  printf("BS_OEMName: %.8s\n",f->b.BS_OEMName);
  printf("BPB_BytesPerSec: %u\n",f->b.BPB_BytsPerSec);
  printf("BPB_SecPerClus: %u\n",f->b.BPB_SecPerClus);
  printf("BPB_RsvdSecCnt: %u\n",f->b.BPB_RsvdSecCnt);
  printf("BPB_NumFATs: %u\n",f->b.BPB_NumFATs);
  printf("BPB_RootEntCnt: %u\n",f->b.BPB_RootEntCnt);
  printf("BPB_TotSec16: %u\n",f->b.BPB_TotSec16);
  printf("BPB_Media: 0x%02x\n",f->b.BPB_Media);
  printf("BPB_FATSz16: 0x%x\n",f->b.BPB_FATSz16);
  printf("BPB_SecPerTrk: %u\n",f->b.BPB_SecPerTrk);
  printf("BPB_NumHeads: %u\n",f->b.BPB_NumHeads);
  printf("BPB_HiddSec: 0x%x\n",f->b.BPB_HiddSec);
  printf("BPB_TotSec32: %u\n",f->b.BPB_TotSec32);
  printf("BPB_FATSz32: %#x\n",f->b.BPB_FATSz32);
  printf("BPB_ExtFlags: 0x%x\n",f->b.BPB_ExtFlags);
  printf("BPB_FSVer: 0x%x\n",f->b.BPB_FSVer);
  printf("BPB_RootClus: %u\n",f->b.BPB_RootClus);
  printf("BPB_FSInfo: 0x%x\n",f->b.BPB_FSInfo);
  printf("BPB_BkBootSec: %u\n",f->b.BPB_BkBootSec);
  printf("BPB_Reserved: %.12s\n",f->b.BPB_Reserved);
  printf("BS_DrvNum: %u\n",f->b.BS_DrvNum);
  printf("BS_Reserved1: 0x%x\n",f->b.BS_Reserved1);
  printf("BS_BootSig: 0x%x\n",f->b.BS_BootSig);
  printf("BS_VolID: %u\n",f->b.BS_VolID);
  printf("BS_VolLab: %.11s\n",f->b.BS_VolLab);
  printf("BS_FilSysType: %.8s\n",f->b.BS_FilSysType);
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


void fat32_ls(const struct fat32_info *f, const char* dir)
{
  uint32_t i, clus = 2; // clus = get_dir_cluster
  if (strcmp(dir, "") == 0 || strcmp(dir, ".") == 0) {
    clus = f->current_cluster;
  }

  struct fat_dir d;
  while ( clus < 0xFFFFFF8) {
    for (i = 0; i < 16; ++i) {
      load_fat_dir(f, &d, clus, i);

      if ( ((d.DIR_Attr & ATTR_LONG_NAME_MASK) != ATTR_LONG_NAME) && (d.DIR_Name[0] != 0x00)) {
        if ( (d.DIR_Attr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == 0x00) {
          // found a file
          printf("%.8s", d.DIR_Name);
          if (d.DIR_Name[8] != 0x20) {
            printf(".%.3s", d.DIR_Name+8);
          }
          printf("\n");
        } else if ( (d.DIR_Attr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == ATTR_DIRECTORY) {
          printf("%.8s\n", d.DIR_Name);
        } else if ( (d.DIR_Attr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == ATTR_VOLUME_ID) {
          // found a volume
        }
      }
    }
    clus = fat_get_next_clus(f,  clus);
  }
}

void fat32_cd(const struct fat32_info *f, const char* dir)
{
  // returns string
  char *full_path = get_full_path(dir);
  GQueue *q = g_queue_new(); //
  GList *walk = NULL;
  uint32_t clus = f->b.BPB_RootClus;
  unsigned i = 0;
  struct fat_dir d;
  tokenize_path(full_path, q);

  // q stores the tokenized version of full_path
  walk = g_queue_peek_head_link(q);
  while (walk) {

    // walk through the current cluster and see if walk->data is found
    while ( clus < 0xFFFFFF8) {
      for (i = 0; i < 16; ++i) {
        load_fat_dir(f, &d, clus, i);

        if ( ((d.DIR_Attr & ATTR_LONG_NAME_MASK) != ATTR_LONG_NAME) && (d.DIR_Name[0] != 0x00)) {
          if ( (d.DIR_Attr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == 0x00) {
            return;
          }
          if ( (d.DIR_Attr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == ATTR_DIRECTORY) {
            if (strncmp(d.DIR_Name, walk->data, 8) == 0) {
              walk = walk->next;
              clus = (d.DIR_FstClusHI << 16) | d.DIR_FstClusLO;
            }
          }
        }
      } // end for
      clus = fat_get_next_clus(f,  clus);
    } // end while (clus)
  } // end while (walk)
}


/*
 * dir must be in current directory
 */
uint32_t fat32_get_dir_clus(const struct fat32_info *f, const char *dir, uint32_t curr_clus)
{
  uint32_t i,strsz;
  struct fat_dir d;

  strsz = strlen(dir);
  if (strsz > 11) return 0;
  while ( curr_clus < 0xFFFFFF8 ) {
    for (i = 0; i < 16; ++i) {
      load_fat_dir(f, &d, curr_clus, i);
      if ( ((d.DIR_Attr & ATTR_LONG_NAME_MASK) != ATTR_LONG_NAME) && (d.DIR_Name[0] != 0x00)) {
        if ( (d.DIR_Attr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == ATTR_DIRECTORY) {
          // this is a directory
          if (strncmp(d.DIR_Name, dir, strsz) == 0) {
            return (d.DIR_FstClusHI << 16) | d.DIR_FstClusLO;
          }
        }
      }
    } // end for
    curr_clus = fat_get_next_clus(f,  curr_clus);
  }
  // return 0 if folder not found in current directory
  return 0;
}

 void fat32_print_dir(struct fat32_info *f, const struct fat_dir *d)
{

}


unsigned first_data_sector(const struct fat32_info *f)
{
  return f->b.BPB_RsvdSecCnt + (f->b.BPB_NumFATs * f->b.BPB_FATSz32) + root_dir_sectors(f);
}


unsigned root_dir_sector(const struct fat32_info *f)
{
  return ((f->b.BPB_RootEntCnt * 32) + (f->b.BPB_BytsPerSec -1 )) /f->b.BPB_BytsPerSec;
}

unsigned root_dir_sectors(const struct fat32_info *f)
{
   return (( f->b.BPB_RootEntCnt * 32) + (f->b.BPB_BytsPerSec -1 )) / f->b.BPB_BytsPerSec;
}

unsigned first_sect_of_clus(const struct fat32_info *f, unsigned clust_num)
{
  return ((clust_num - 2) * (f->b.BPB_SecPerClus)) + first_data_sector(f);
}

unsigned sector_to_byte(const struct fat32_info *f, unsigned sect_num)
{
  return sect_num * f->b.BPB_SecPerClus;
}

unsigned cluster_to_byte(const struct fat32_info *f, unsigned clust_num)
{
  return first_sect_of_clus(f, clust_num) * f->b.BPB_BytsPerSec;
}

uint32_t fat_entry(const struct fat32_info *f  ,unsigned clust_num)
{
  uint32_t entry;
  fseek(f->fp, fat_address(f, clust_num), SEEK_SET);
  fread(&entry, 4, 1, f->fp);
  return entry;
}

/*
  Returns the byte address for the img file for the cluster number
*/
uint32_t fat_address(const struct fat32_info *f, uint32_t cluster)
{
  uint32_t fat_offset = cluster * 4;
  uint32_t fat_sec_num =f->b.BPB_RsvdSecCnt + (fat_offset /f->b.BPB_BytsPerSec);
  uint32_t fat_ent_offset = fat_offset %f->b.BPB_BytsPerSec;
  return (fat_sec_num *f->b.BPB_BytsPerSec) + fat_ent_offset;
}

/*
  Returns the next fat value from the fat table.
*/
uint32_t fat_get_next_clus(const struct fat32_info *f, uint32_t curr_clus)
{
  uint32_t fat_ent;
  fseek(f->fp, fat_address(f, curr_clus), SEEK_SET);
  fread(&fat_ent, sizeof(uint32_t), 1, f->fp);
  return fat_ent;
}
