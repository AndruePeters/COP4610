#ifndef CMDS_H
#define CMDS_H

//Prototypes, structs, and or utilities here
#define ENTRIES_PER_SECTOR 16
#define OFFSET_CONST 32
#define LAST_ENTRY 0x00
#define EMPTY_ENTRY 0xE5
#define ATTR_LONG_NAME 0x0F
#define FAT_ENTRY_SIZE 4
#define MSIZE 99999	//Const size for char strings workingdir,input

//clusters
#define EMPTY_CLUSTER 0x00000000
#define EOC 0x0FFFFFF8
#define MASK_IGNORE_MSB 0x0FFFFFFF

//file table
#define TBL_OPEN_FILES 75
#define TBL_DIR_STATS  75

//boolean support
#define TRUE 1
#define FALSE 0
typedef int bool;

//open file codes
#define MODE_READ 0
#define MODE_WRITE 1
#define MODE_BOTH 2
#define MODE_UNKNOWN 3
#define ATTR_DIRECTORY 0x10

#define MAX_FILENAME_SIZE 8
#define MAX_EXTENTION_SIZE 3

#include <stdint.h>

struct DIR
{

    unsigned char DIR_Name[11];
    unsigned char DIR_Attr;
    unsigned char DIR_NTRes;
    unsigned char DIR_CrtTimeTenth;

    unsigned short DIR_CrtTime;
    unsigned short DIR_CrtDate;
    unsigned short DIR_LstAccDate;
    unsigned short DIR_FstClusHI;
    unsigned short DIR_WrtTime;
    unsigned short DIR_WrtDate;
    unsigned short DIR_FstClusLO;
    unsigned int DIR_FileSize;
} __attribute__((packed));

struct BPB
{
    unsigned char BS_jmpBoot[3];
    unsigned char BS_OEMName[8];
    unsigned short BPB_BytsPerSec;
    unsigned char BPB_SecPerClus;
    unsigned short BPB_RsvdSecCnt;

    unsigned char BPB_NumFATs;
    unsigned short BPB_RootEntCnt;
    unsigned short BPB_TotSec16;
    unsigned char BPB_Media;
    unsigned short BPB_FATSz16;
    unsigned short BPB_SecPerTrk;
    unsigned short BPB_NumHeads;
    unsigned int BPB_HiddSec;
    unsigned int BPB_TotSec32;
    //FAT32 36 offset point
    unsigned int BPB_FATSz32;
    unsigned short BPB_ExtFlags;
    unsigned short BPB_FSVer;
    unsigned int BPB_RootClus;
    unsigned short BPB_FSInfo;
    unsigned short BPB_BkBootSec;
    unsigned char BPB_Reserved[12];
    unsigned char BS_DrvNum;
    unsigned char BS_Reserved1;

    unsigned char BS_BootSig;
    unsigned int BS_VolID;
    unsigned char BS_VolLab[11];
    unsigned char BS_FilSysType[8];

} __attribute__((packed));

struct FILEDESCRIPTOR
{
    uint8_t filename[9];
    uint8_t extention[4];
    char parent[100];
    uint32_t firstCluster;
    int mode;
    uint32_t size;
    bool dir;
    bool isOpen;
    uint8_t fullFilename[13];
} __attribute__((packed));


struct ENVIRONMENT
{
    char pwd[100];
    char imageName[100];
    int pwd_cluster;
    uint32_t io_writeCluster;
    int tbl_dirStatsIndex;
    int tbl_openFilesIndex;
    int tbl_dirStatsSize;
    int tbl_openFilesSize;
    char last_pwd[100];
    struct FILEDESCRIPTOR * openFilesTbl;
    struct FILEDESCRIPTOR * dirStatsTbl;

} environment;

//Global variables
struct BPB b;
struct DIR d;
FILE* fp;

unsigned int FirstDataSector;
unsigned int RootDirSectors;
unsigned int current_cluster;
char workingdir[MSIZE], workingdir2[MSIZE];

char ROOT[5];
char PARENT[5];
char *args[100];
char *pargs[100];
char inputBuffer2[200];

//This will update the BPB with the latest from our fat32img file
void updateBPB(struct BPB*);
void updateDIR(struct DIR*, uint32_t, unsigned int);
void updateEntry(struct DIR*, uint32_t, unsigned int);
int FWRITE_writeData(struct BPB * bs, uint32_t firstCluster, uint32_t pos, const char * data, uint32_t dataSize);
int FREAD_readData(struct BPB * bs, uint32_t firstCluster, uint32_t pos, uint32_t dataSize, uint32_t fileSize);

void ls_size(const char*, uint32_t, char*, int); //Does both ls and size
uint32_t cd(int, char*);
int fOpen(struct BPB * bs, int argC, const char * filename, const char * option, char * modeName);
int fClose(struct BPB * bs, int argC, const char * filename);
int fRead(struct BPB * bs, int argC, const char * filename, const char * position, const char * numberOfBytes, uint32_t * actualBytesRead);
int fWrite(struct BPB * bs, int argC, const char * filename, const char * position, const char * data) ;
int multifun(struct BPB * bs, uint32_t clusterNum, bool cd, const char * directoryName, bool goingUp, struct FILEDESCRIPTOR * searchFile, bool useAsSearch, bool searchForDir);
int rm(struct BPB * bs, const char * searchName, uint32_t searchDirectoryCluster);
int rmDir(struct BPB * bs, const char * searchName, uint32_t searchDirectoryCluster) ;
void create (char *, int);
void mkdir (char *, int);
//==================Initialization========================
bool handleAbsolutePaths(struct BPB * bs, int argcount,uint32_t * targetCluster, char * path, char * successFilename, char * failFilename, bool isCd, bool searchForFile, bool isMkdir);
void allocateFileTable();
int initEnvironment(const char * imageName, struct BPB * boot_sector);

//==================Args========================
int toArgs(char* input);
int PathtoArgs(char* input);
void freeArgs(char** input, int argcount);

//==================Path========================
int dissolvePath(char* input);
int resolvePath(void);
int isPath(char* str);
bool isEntryEmpty(struct FILEDESCRIPTOR * fd);

//==================Utilities========================
int checkForFileError(FILE * f);
long sector_to_bytes(long);
char* str_to_fat(char*);
int countOfClusters(struct BPB * bs);
int sectorsInDataRegion(struct BPB * bs);
int rootDirSectorCount(struct BPB * bs);
void writeNewFatEntry(uint32_t, uint32_t *);
int showEntry(struct DIR * entry);
int createEntry(struct DIR * entry, const char * filename, const char * ext, int isDir, uint32_t firstCluster, uint32_t filesize, bool emptyAfterThis, bool emptyDirectory);
struct DIR * readEntry(struct BPB * bs, struct DIR * entry, uint32_t clusterNum, int offset);
struct DIR * writeEntry(struct BPB * bs, struct DIR * entry, uint32_t clusterNum, int offset);
struct FILEDESCRIPTOR * makeFileDecriptor(struct DIR * entry, struct FILEDESCRIPTOR * fd);
struct FILEDESCRIPTOR * TBL_createFile(const char * filename, const char * extention, const char * parent, uint32_t firstCluster, int mode, uint32_t size, int dir,int isOpen);

unsigned int FirstSectorOfCluster(unsigned int);
uint32_t byteOffsetOfCluster(uint32_t);
int getFileSizeInClusters(uint32_t);
int getFirstEmptyCluster();
int getEntryOffset(struct BPB * bs, const char * searchName) ;
uint32_t getFatAddressByCluster(uint32_t);
uint32_t getFatEntry(uint32_t);
uint32_t getLastClusterInChain(struct BPB * bs, uint32_t firstClusterVal);
int FAT_writeFatEntry(struct BPB * bs, uint32_t destinationCluster, uint32_t * newFatVal);
int FAT_findFirstFreeCluster(struct BPB * bs);
uint32_t FAT_extendClusterChain(struct BPB * bs,  uint32_t clusterChainMember);
int FAT_allocateClusterChain(struct BPB * bs,  uint32_t clusterNum) ;
int FAT_freeClusterChain(struct BPB * bs,  uint32_t firstClusterOfChain);
uint32_t buildClusterAddress(struct DIR * entry);
int deconstructClusterAddress(struct DIR * entry, uint32_t cluster);
const char * TBL_getParent(const char * dirName);
bool TBL_getFileDescriptor(int * index, const char * filename, bool isDir);
int TBL_addFileToTbl(struct FILEDESCRIPTOR * file, int isDir);
int FAT_setIoWriteCluster(struct BPB * bs, uint32_t clusterChainMember);

bool searchOrPrintFileSize(struct BPB * bs, const char * fileName, bool useAsFileSearch, bool searchForDirectory, struct FILEDESCRIPTOR * searchFile);
bool searchForFile(struct BPB * bs, const char * fileName, bool searchForDirectory, struct FILEDESCRIPTOR * searchFile) ;
//Print functions
void printBPB(struct BPB*);
void printDIR(struct DIR*); //For Debugging
void printLS(struct DIR*);
void printSZ(struct DIR*);


#endif
