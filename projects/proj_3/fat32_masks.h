/*
  File created just to separate masks for in development commenting.
*/

#ifndef _GROUP_15_FAT32_MASKS_H_
#define _GROUP_15_FAT32_MASKS_H_

#define FAT32_BAD_CLUSTER (0x0FFFFFF7)


/*
  If set, then volume is clean.
  If cleared, then volume is "dirty". This means that the file system driver
  did not dismount the volume properly, so some disk repair program would be a
  good idea.

  Page 18.
*/
#define FAT32_ClnShutBitMask (0x08000000)

/*
  If bit is 1, then no disk read/write errors were encountered.
  If bit is 0, the file system driver encountered a disk IO error on the
  volume the last time it was mounted which means some sectors may have gone bad.
  Some disk repair utility might be a good idea to look for new bad sectors.
*/


/*
  ATTR_READ_ONLY              writes to file should fail
  ATTR_HIDDEN                 normal directly listings should not show file
  ATTR_SYSTEM                 operating system file
  ATTR_VOLUME_ID              only a single file with this attritube. Actual name for the volume
  ATTR_DIRECTORY              this file is a container for other files
  ATTR_ARCHIVE                supports backup utilities. Set by FAT driver when a file is
                              created, renamed, or written to.

  Page 18
*/
enum fat32_dir_attr_e {
  ATTR_READ_ONLY = 0x01,
  ATTR_HIDDEN = 0x02,
  ATTR_SYSTEM = 0x04,
  ATTR_VOLUME_ID = 0x08,
  ATTR_DIRECTORY = 0x10,
  ATTR_ARCHIVE = 0x20,
  ATTR_LONG_NAME = ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID,
  ATTR_LONG_NAME_MASK = ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID | ATTR_DIRECTORY | ATTR_ARCHIVE
};

enum fat32_dir_name_e {
  DIR_NAME_FREE = 0xE5,
  DIR_NAME_ALL_FREE = 0x00,
  DIR_NAME_KANJI = 0x05 // then character is actualy 0xE5
};



/*
  Invalid values for DIR_Name[0]

  Page 24
*/




#endif
