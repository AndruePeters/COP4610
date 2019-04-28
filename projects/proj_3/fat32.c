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
  struct dir_pos pos;
  if (strcmp(dir, "") == 0 || strcmp(dir, ".") == 0) {
    pos.cluster = f->pos.cluster;
  } else {
    pos = fat32_get_curr_dir_pos(f, dir, f->pos.cluster);
    if (pos.cluster == 0 && pos.offset == 0) {
      printf("Invalid directory.\n");
      return ;
    }
  }


  struct fat_dir d;
  while ( pos.cluster < 0xFFFFFF8) {
    for (i = 0; i < 16; ++i) {
      load_fat_dir(f, &d, pos.cluster, i);

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
    pos.cluster = fat_get_next_clus(f,  pos.cluster);
  }
}


uint32_t fat32_cd(struct fat32_info *f, const char* dir)
{
  // returns string
//  char *full_path = get_full_path(dir);
  GQueue *q = g_queue_new(); //
  GList *walk = NULL;
  struct fat_dir d;
  struct dir_pos pos;
  uint32_t prev_clus, prev_offset;
  tokenize_path(dir, q);
  pos.cluster = f->pos.cluster;
  pos.offset = 0;
  // q stores the tokenized version of full_path
  walk = g_queue_peek_head_link(q);
  while (walk->next) {
    pos = fat32_get_curr_dir_pos(f, walk->data, pos.cluster);
    if (!pos.cluster) {
      printf("Directory not found.\n");
      return 0;
    }
    walk = walk->next;
  } // end while (clus)

  // now do last case
  // clus cannot be null at this point
  pos = fat32_get_curr_dir_pos(f, walk->data, pos.cluster);

  // need the backups for the previous case
  prev_clus = pos.cluster;
  prev_offset = pos.offset;
  f->pos.cluster = prev_clus;
  f->pos.offset = prev_offset;
  // make sure it is correct
  if (pos.cluster != 0 && pos.offset != 0) {
    load_fat_dir(f, &d, pos.cluster, pos.offset);
     if (fat32_is_dir(&d)) {
       printf("made it\n");

     }
   }


}

bool fat32_is_dir(const struct fat_dir *d)
{
  if ( ((d->DIR_Attr & ATTR_LONG_NAME_MASK) != ATTR_LONG_NAME) && (d->DIR_Name[0] != 0x00)) {
    if ( (d->DIR_Attr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == ATTR_DIRECTORY) {
      return true;
    }
  }
  return false;
}

void fat32_get_parent_clus(const struct fat32_info *f, uint32_t clus)
{

}

/*
  Finds the directory at the full path dir
*/
struct dir_pos fat32_get_dir_pos(const struct fat32_info *f, const char *dir, uint32_t curr_clus)
{
  // returns string
  //  char *full_path = get_full_path(dir);
  GQueue *q = g_queue_new(); //
  GList *walk = NULL;
  uint32_t clus , next_clus;
  unsigned i = 0;
  struct fat_dir d;
  struct dir_pos pos;
  tokenize_path(dir, q);
  pos.cluster = f->b.BPB_RootClus;

  // q stores the tokenized version of full_path
  // all dirs in loop should be folders
  walk = g_queue_peek_head_link(q);
  while (walk && walk->next) {
    pos = fat32_get_curr_dir_pos(f, walk->data, pos.cluster);
    load_fat_dir(f, &d, pos.cluster, pos.offset);
    if (!pos.cluster) {
      printf("Directory not found.\n");
      pos.cluster = 0;
      pos.offset = 0;
      return pos;
    }
    walk = walk->next;
  } // end while (clus)

  // now do last case
  // clus cannot be null at this point
  pos = fat32_get_curr_dir_pos(f, walk->data, pos.cluster);
  load_fat_dir(f, &d, pos.cluster, pos.offset);
  return pos;
}


/*
 * Returns the starting cluster of "dir" if dir is found at dir curr_clus
 * If dir is not found in current directory, then returns 0
 */
struct dir_pos fat32_get_curr_dir_pos(const struct fat32_info *f, const char *dir, uint32_t curr_clus)
{
  uint32_t i,strsz;
  struct fat_dir d;
  struct dir_pos pos = {.cluster = curr_clus, .offset = 0};

  strsz = strlen(dir);
  if (strsz > 11) return pos;
  while ( pos.cluster < 0xFFFFFF8 ) {
    for (i = 0; i < 16; ++i) {
      load_fat_dir(f, &d, pos.cluster, i);
      if ( ((d.DIR_Attr & ATTR_LONG_NAME_MASK) != ATTR_LONG_NAME) && (d.DIR_Name[0] != 0x00)) {
        if ( (d.DIR_Attr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == ATTR_DIRECTORY) {
          // this is a directory
          if (strncmp(d.DIR_Name, dir, strsz) == 0) {
            pos.cluster = (d.DIR_FstClusHI << 16) | d.DIR_FstClusLO;
            pos.offset = i;
            return pos;
          }
        }
      }
    } // end for
    pos.cluster = fat_get_next_clus(f,  pos.cluster);
  }
  // return 0 if folder not found in current directory
  return pos;
}

void fat32_get_dir_name(struct fat_dir *d, char *buffer)
{
  int i;
  for (i = 0; i < 8 && !isspace(d->DIR_Name[i]); ++i) {
    buffer[i] = d->DIR_Name[i];
  }

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
