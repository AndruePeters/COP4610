#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> //For toupper
#include "cmds.h"

const uint8_t ATTR_ARCHIVE = 0x20;
uint32_t FAT_EOC = 0x0FFFFFF8;
uint32_t FAT_FREE_CLUSTER = 0x00000000;

//This function will update the BPB struct with
//the boot sector info on the fat32.img (used for info cmd)
void updateBPB(struct BPB* bbb)
{
    struct BPB whatever;
    fread(&whatever, sizeof(whatever), 1, fp);
    memcpy(bbb, &whatever, sizeof(whatever));
    //Reset the file ptr to beginning
    fseek(fp, 0, SEEK_SET);
}

void updateDIR(struct DIR* ddd, uint32_t cluster, unsigned int offset)
{
    offset *= 32;
    uint32_t byte_addr = byteOffsetOfCluster(cluster);
    fseek(fp, byte_addr + offset, 0);
    fread(ddd, sizeof(struct DIR), 1, fp);
    //Reset the file ptr to the beginning
    fseek(fp, 0, SEEK_SET);
}

void updateEntry(struct DIR* ddd, uint32_t cluster, unsigned int offset)
{
    offset *= 32;
    offset += 32;
    uint32_t addr =  FirstSectorOfCluster(cluster) * b.BPB_BytsPerSec;
    fseek(fp, addr + offset, 0);
    fread(ddd, sizeof(struct DIR), 1, fp);
    fseek(fp, 0, SEEK_SET);
}

//When give /example/whatever it will turn path into an array of
//args[0] = example args[1] = whatever
//Will also return the number of args
int dissolvePath(char* input)
{
    int argcount = 0;
    //Remove '/' and replace with space then shift string left 1
    int i;
    for(i = 0; i < strlen(input); i++)
        if(input[i] == '/')
            input[i] = ' ';
    //Shift left 1
    for(i = 0; i < strlen(input)-1; i++)
        input[i] = input[i+1];
    //Now "example whatever" can be put onto args array
    argcount = PathtoArgs(input);
    return argcount;
}

//1 yes path is resolved 0 path not resolved
int resolvePath(void)
{
    int pathargcount = 0;
    //Store args[1] into series of pargs
    pathargcount = dissolvePath(args[1]);
    //Pargs now holds all the names of directories
    //Iterate over all the pargs checking each for success
    int k = 0, success_cd = 1;
    unsigned int origin_cluster = current_cluster;
    unsigned int lastclusternum = current_cluster;
    while(k < pathargcount)
    {
        lastclusternum = current_cluster;
        current_cluster = cd(current_cluster, pargs[k]);
        //If they have changed then successful cd -- if not...
        if(lastclusternum != current_cluster)
            ++k;
        else
        {
            success_cd = 0;
            break;
        }
    }
    if(!success_cd)
        current_cluster = origin_cluster;
    //Free pargs
    freeArgs(pargs, pathargcount);
    if(success_cd)
        return 1;
    return 0;
}

//1 yes 0 no
int isPath(char* str)
{
    int i;
    for(i = 0; i < strlen(str); i++)
        if(str[i] == '/')
            return 1;
    return 0;
}

bool isEntryEmpty(struct FILEDESCRIPTOR * fd)
{
    if((fd->filename[0] != 0x00) && (fd->filename[0] != 0xE5) )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


void ls_size(const char* directoryName, uint32_t clusterNum, char* targ_name, int is_size)
{
    struct DIR dir;
    int found_targ = 0;
    int dirSizeInCluster = getFileSizeInClusters(clusterNum);
    int offset, increment, clusterCount;
    for(clusterCount = 0; clusterCount < dirSizeInCluster; clusterCount++)
    {
        if(strcmp(directoryName, "/") == 0)
        {
            offset = 1;
            increment = 2;
        }
        else
        {
            offset = 0;
            increment = 1;
        }
        for(; offset < ENTRIES_PER_SECTOR; offset += increment)
        {
            if(strcmp(directoryName, "/") != 0 && offset == 2)
            {
                increment = 2;
                offset -= 1;
                continue;
            }
            updateDIR(&dir, clusterNum, offset);
            //Local ls
            if(!is_size)
                printLS(&dir);
            else
            {
                //Local File size
                int ismatch = 1, j;
                for(j = 0; j < 11; j++)
                {
                    if(j >= strlen(targ_name))
                        break;
                    if(dir.DIR_Name[j] != targ_name[j])
                    {
                        ismatch = 0;
                        break;
                    }
                }

                if(ismatch)
                {
                    printSZ(&dir);
                    found_targ = 1;
                }
            }
        }
        clusterNum = getFatEntry(clusterNum);
    }

    if(!found_targ && is_size)
        printf("ERR: Couldn't find file\n");
}

uint32_t cd(int cdirnum, char* DIRNAME)
{

    if(!strcmp(DIRNAME, ".."))
        if(cdirnum == 2)
        {
            printf("ERR: Cannot go up further\n");
            return cdirnum;
        }
    struct DIR dd;
    char *token;
    int counter = 0;
    int offset = 0;
    int parent = cdirnum;

    if(!strcmp(DIRNAME, "."))
        return cdirnum;
    while(counter*64 <= b.BPB_BytsPerSec)
    {
        if(counter*64 == b.BPB_BytsPerSec)
        {
            uint32_t entry = getFatEntry(cdirnum);
            if(entry == 0x0FFFFFF8 || entry == 0xFFFFFFF || entry == 0x00000000)
            {
                puts("reached end of directory\n");
                break;
            }
            else
            {
                cdirnum = entry;
                offset = 0;
                counter = 0;
            }
        }
        updateEntry(&dd, cdirnum, offset);
        token = strtok((char*)dd.DIR_Name, " ");
        if(strcmp(DIRNAME, "..") && strcmp(DIRNAME, "."))
            DIRNAME = str_to_fat(DIRNAME);

        if(token != NULL)
        {
            if(!strcmp(DIRNAME, token) && dd.DIR_Attr == 0x10)
            {
                if(!strcmp(DIRNAME, ".."))
                {
                    //This means we need to get rid of the rightmost /whatever/path
                    int h;
                    for(h = strlen(workingdir); h >= 0; h--)
                        if(workingdir[h] == '/')
                        {
                            workingdir[h] = '\0';
                            break;
                        }
                    if(strlen(workingdir) == 0)
                    {
                        workingdir[0] = '/';
                        workingdir[1] = '\0';
                    }
                }
                else
                {
                    //This means we need to append to the rightmost
                    //Append args[1] onto workingdir
                    if(strlen(workingdir) != 1)
                        strcat(workingdir, "/");
                    strcat(workingdir, DIRNAME);

                }

                uint32_t cluster_ret = dd.DIR_FstClusHI*0x100 + dd.DIR_FstClusLO;
                if((cluster_ret) == 0)
                    return 2;
                return cluster_ret;
            }
            else if(!strcmp(DIRNAME, token) && dd.DIR_Attr != 0x10)
            {
                printf("ERR: %s is not a directory\n", DIRNAME);
                return parent;
            }
        }
        offset += 2;
        counter++;
    }
    printf("ERR: %s does not exist\n", DIRNAME);
    return parent;
}


// Takes in a file name and creates a DIR struct and inserts it into the Fat Image
void create(char * DIRNAME, int current_cluster_number)
{

    struct DIR * dir1;
    dir1 = malloc(sizeof(struct DIR));

    int empty_cluster = getFirstEmptyCluster();
    getFirstEmptyCluster();
    int counter = 0;
    int offset = 0;
    dir1->DIR_Attr = 0;
    dir1->DIR_CrtTime = 0;
    dir1->DIR_CrtDate = 0;
    dir1->DIR_WrtTime = 0;
    dir1->DIR_WrtDate = 0;
    dir1->DIR_LstAccDate = 0;
    dir1->DIR_FileSize = 0;
    dir1->DIR_FstClusHI = 0x00;
    dir1->DIR_FstClusLO = 0x00;
    strcpy((char *)dir1->DIR_Name, DIRNAME);
    while(counter*64 <= b.BPB_BytsPerSec)
    {
        if(counter*64 == b.BPB_BytsPerSec)
        {
            uint32_t entry = getFatEntry(current_cluster_number);
            if (entry == 0x0FFFFFF8)
            {
                if (empty_cluster > -1)
                {
                    uint32_t eoc = 0x0FFFFFF8;
                    writeNewFatEntry(current_cluster_number, &empty_cluster);
                    writeNewFatEntry(empty_cluster, &eoc);
                    uint32_t data_addy = FirstSectorOfCluster(empty_cluster);
                    fseek(fp, data_addy, 0);
                    fwrite(dir1, sizeof(struct DIR), 1, fp);
                }
                else
                    printf("Error, there are no free clusters available\n");

                free(dir1);
                return;
            }

            else
            {
                current_cluster_number = entry;
                counter = 0;
            }
        }
        counter++;
    }


}


// Takes in a directory name and inserts a directory into the Fat Image
void mkdir(char * DIRNAME, int current_cluster_number)
{

    struct DIR * dir1;
    dir1 = malloc(sizeof(struct DIR));

    int empty_cluster = getFirstEmptyCluster();
    getFirstEmptyCluster();
    int counter = 0;
    int offset = 0;
    dir1->DIR_Attr = 0x10;
    dir1->DIR_CrtTime = 0;
    dir1->DIR_CrtDate = 0;
    dir1->DIR_WrtTime = 0;
    dir1->DIR_WrtDate = 0;
    dir1->DIR_LstAccDate = 0;
    dir1->DIR_FileSize = 0;
    dir1->DIR_FstClusHI = 0x00;
    dir1->DIR_FstClusLO = 0x00;
    strcpy((char *)dir1->DIR_Name, DIRNAME);
    while(counter*64 <= b.BPB_BytsPerSec)
    {
        if(counter*64 == b.BPB_BytsPerSec)
        {
            uint32_t entry = getFatEntry(current_cluster_number);
            if (entry == 0x0FFFFFF8)
            {
                if (empty_cluster > -1)
                {
                    uint32_t eoc = 0x0FFFFFF8;
                    writeNewFatEntry(current_cluster_number, &empty_cluster);
                    writeNewFatEntry(empty_cluster, &eoc);
                    uint32_t data_addy = FirstSectorOfCluster(empty_cluster);
                    fseek(fp, data_addy, 0);
                    fwrite(dir1, sizeof(struct DIR), 1, fp);
                }
                else
                    printf("Error, there are no empty clusters available\n");

                free(dir1);
                return;
            }

            else
            {
                current_cluster_number = entry;
                counter = 0;
            }
        }
        counter++;
    }


}


int FWRITE_writeData(struct BPB * bs, uint32_t firstCluster, uint32_t pos, const char * data, uint32_t dataSize)
{

    uint32_t currentCluster = firstCluster;
    uint32_t writeClusterOffset = (pos / bs->BPB_BytsPerSec);
    uint32_t posRelativeToCluster = pos %  bs->BPB_BytsPerSec;
    uint32_t fileSizeInClusters = getFileSizeInClusters(firstCluster);
    uint32_t remainingClustersToWalk = fileSizeInClusters - writeClusterOffset;

    int x;
    int dataIndex;

    for(x = 0; x < writeClusterOffset; x++)
    {
        currentCluster = getFatEntry(currentCluster);
    }

    dataIndex = 0;
    uint32_t dataWriteLength = dataSize;
    uint32_t dataRemaining = dataSize;
    uint32_t fileWriteOffset;
    uint32_t startWritePos;
    FILE* f = fopen(environment.imageName, "r+");
    checkForFileError(f);
    for(x = 0; x < remainingClustersToWalk; x++)
    {

        startWritePos = byteOffsetOfCluster(currentCluster);

        if(x == 0)
            startWritePos += posRelativeToCluster;

        if(dataRemaining > bs->BPB_BytsPerSec)
            dataWriteLength = bs->BPB_BytsPerSec;
        else
            dataWriteLength = dataRemaining;


        for(fileWriteOffset = 0; fileWriteOffset < dataWriteLength; fileWriteOffset++)
        {
            fseek(f, startWritePos + fileWriteOffset, 0);

            uint8_t dataChar[1] = {data[dataIndex++]};
            fwrite(dataChar, 1, 1, f);

        }

        dataRemaining -= dataWriteLength;
        if(dataRemaining == 0)
        {
            break;
        }

        currentCluster = getFatEntry(currentCluster);
    }
    fclose(f);
    return 0;
}

int FREAD_readData(struct BPB * bs, uint32_t firstCluster, uint32_t pos, uint32_t dataSize, uint32_t fileSize)
{

    uint32_t currentCluster = firstCluster;

    uint32_t readClusterOffset = (pos / bs->BPB_BytsPerSec);
    uint32_t posRelativeToCluster = pos %  bs->BPB_BytsPerSec;
    uint32_t fileSizeInClusters = getFileSizeInClusters(firstCluster);
    uint32_t remainingClustersToWalk = fileSizeInClusters - readClusterOffset;

    int x;

    for(x = 0; x < readClusterOffset; x++)
    {
        currentCluster = getFatEntry(currentCluster);
    }

    uint32_t dataReadLength = dataSize;
    uint32_t dataRemaining = dataSize;
    uint32_t fileReadOffset;
    uint32_t startReadPos;
    FILE* f = fopen(environment.imageName, "r");
    checkForFileError(f);
    for(x = 0; x < remainingClustersToWalk; x++)
    {

        startReadPos = byteOffsetOfCluster(currentCluster);

        if(x == 0)
            startReadPos += posRelativeToCluster;

        if(dataRemaining > bs->BPB_BytsPerSec)
            dataReadLength = bs->BPB_BytsPerSec;
        else
            dataReadLength = dataRemaining;


        for(fileReadOffset = 0; fileReadOffset < dataReadLength && (pos + fileReadOffset) < fileSize; fileReadOffset++)
        {
            fseek(f, startReadPos + fileReadOffset, 0);
            uint8_t dataChar[1];
            fread(dataChar, 1, 1, f);
            printf("%c", dataChar[0]);
        }

        dataRemaining -= dataReadLength;
        if(dataRemaining == 0)
        {
            break;
        }

        currentCluster = getFatEntry(currentCluster);
    }
    fclose(f);
    return 0;
}
//==================Utilities========================


int getFileSizeInClusters(uint32_t firstClusterVal)
{
    int size = 1;
    firstClusterVal = (int) getFatEntry(firstClusterVal);
    while((firstClusterVal = (firstClusterVal & 0x0FFFFFFF)) < EOC)
    {
        size++;
        firstClusterVal = getFatEntry(firstClusterVal);
    }
    return size;
}

unsigned int FirstSectorOfCluster(unsigned int N)
{
    return (((N - 2) * b.BPB_SecPerClus) + FirstDataSector);
}

long sector_to_bytes(long sec)
{
    return (b.BPB_BytsPerSec * sec);
}

void writeNewFatEntry(uint32_t clusterNum, uint32_t * FatVal)
{

    fseek(fp, getFatAddressByCluster(clusterNum), 0);
    fwrite(FatVal, 4, 1, fp);
}

uint32_t byteOffsetOfCluster(uint32_t cluster)
{
    return FirstSectorOfCluster(cluster) * b.BPB_BytsPerSec;
}

char* str_to_fat(char* str)
{
    int i;
    for(i = 0; i < strlen(str); i++)
    {
        if(str[i] == '.')
            str[i] = ' ';
        else
            str[i] = toupper(str[i]);
    }
    return str;
}

int rootDirSectorCount(struct BPB * bs)
{
    return (bs->BPB_RootEntCnt * 32) + (bs->BPB_BytsPerSec - 1) / bs->BPB_BytsPerSec ;

}

int sectorsInDataRegion(struct BPB * bs)
{
    int FATSz;
    int TotSec;
    if(bs->BPB_FATSz16 != 0)
        FATSz = bs->BPB_FATSz16;
    else
        FATSz = bs->BPB_FATSz32;
    if(bs->BPB_TotSec16 != 0)
        TotSec = bs->BPB_TotSec16;
    else
        TotSec = bs->BPB_TotSec32;
    return TotSec - (bs->BPB_RsvdSecCnt + (bs->BPB_NumFATs * FATSz) + rootDirSectorCount(bs));

}

int countOfClusters(struct BPB * bs)
{
    return sectorsInDataRegion(bs) / bs->BPB_SecPerClus;
}

int getFirstEmptyCluster()
{

    int clusterNum = b.BPB_RootClus;
    int fileSize = getFileSizeInClusters(clusterNum);

    while (getFatEntry(clusterNum) != EMPTY_CLUSTER)
    {
        clusterNum++;
    }
    if (clusterNum == fileSize)
    {
        printf ("Error, no empty cluster");
        return -1;
    }

    else
    {
        return clusterNum;
    }
}

uint32_t getFatAddressByCluster(uint32_t clusterNum)
{
    uint32_t FATOffset = clusterNum * 4;
    uint32_t ThisFATSecNum = b.BPB_RsvdSecCnt + (FATOffset / b.BPB_BytsPerSec);
    uint32_t ThisFATEntOffset = FATOffset % b.BPB_BytsPerSec;
    return (ThisFATSecNum * b.BPB_BytsPerSec + ThisFATEntOffset);
}

uint32_t getFatEntry(uint32_t clusterNum)
{


    uint8_t tempFatEntry[4];
    uint32_t offset = clusterNum * 4;
    fseek(fp, getFatAddressByCluster(clusterNum), SEEK_SET);
    fread(tempFatEntry, 1, FAT_ENTRY_SIZE, fp);

    uint32_t fatEntry = 0x00000000;
    for (int x = 0; x < 4; x++)
    {
        fatEntry |= tempFatEntry[(offset % FAT_ENTRY_SIZE) + x] << 8 * x;
    }

    return fatEntry;
}


int FAT_writeFatEntry(struct BPB * bs, uint32_t destinationCluster, uint32_t * newFatVal)
{
    FILE* f = fopen(environment.imageName, "r+");
    checkForFileError(f);
    fseek(f, getFatAddressByCluster(destinationCluster), 0);
    fwrite(newFatVal, 4, 1, f);
    fclose(f);
    return 0;
}

void printBPB(struct BPB* b)
{
    int i;
    unsigned char prtBoot[4];
    for(i = 0; i < 3; i++)
        prtBoot[i] = b->BS_jmpBoot[i];
    prtBoot[3] = '\0';
    printf("jmpBoot: %s\n", prtBoot);
    printf("OEMName: %s\n", b->BS_OEMName);
    printf("BytsPerSec: %hu\n", b->BPB_BytsPerSec);
    printf("SecPerClus: %u\n", b->BPB_SecPerClus);
    printf("RsvdSecCnt: %hu\n", b->BPB_RsvdSecCnt);
    printf("NumFATs: %uc\n", b->BPB_NumFATs);
    printf("RootEntCnt: %hu\n", b->BPB_RootEntCnt);
    printf("TotSec16: %hu\n", b->BPB_TotSec16);
    printf("Media: %uc\n", b->BPB_Media);
    printf("FATSz16: %hu\n", b->BPB_FATSz16);
    printf("SecPerTrk: %hu\n", b->BPB_SecPerTrk);
    printf("NumHeads: %hu\n", b->BPB_NumHeads);
    printf("HiddSec: %u\n", b->BPB_HiddSec);
    printf("TotSec32: %u\n\n", b->BPB_TotSec32);
    //FAT32 Now at offset 36
    printf("FATSz32: %u\n", b->BPB_FATSz32);
    printf("ExtFlags: %hu\n", b->BPB_ExtFlags);
    printf("FSVer: %hu\n", b->BPB_FSVer);
    printf("RootClus: %u\n", b->BPB_RootClus);
    printf("FSInfo: %hu\n", b->BPB_FSInfo);
    printf("BkBootSec: %hu\n", b->BPB_BkBootSec);
    printf("Reserved: %s\n", b->BPB_Reserved);
    printf("DrvNum: %uc\n", b->BS_DrvNum);
    printf("Reserved1: %uc\n", b->BS_Reserved1);
    printf("BootSig: %uc\n", b->BS_BootSig);
    printf("VolID: %u\n", b->BS_VolID);

    unsigned char prtLab[12];
    for(i = 0; i < 11; i++)
        prtLab[i] = b->BS_VolLab[i];
    prtLab[11] = '\0';
    printf("VolLab: %s\n", prtLab);
    printf("FilSysType: %s\n\n", b->BS_FilSysType);

    printf("RootDirSectors:%u\n", RootDirSectors);
    printf("FirstDataSector:%u\n", FirstDataSector);
}

void printDIR(struct DIR* d)
{
    int i;
    unsigned char tDirName[12];
    for(i = 0; i < 11; i++)
        tDirName[i] = d->DIR_Name[i];
    tDirName[11] = '\0';
    printf("Name: %s\n", tDirName);
    printf("Attr: %#08x\n", d->DIR_Attr);
    printf("NTRes: %uc\n", d->DIR_NTRes);
    printf("CrtTimeTenth: %uc\n", d->DIR_CrtTimeTenth);
    printf("CrtTime: %hu\n", d->DIR_CrtTime);
    printf("CrtDate: %hu\n", d->DIR_CrtDate);
    printf("LstAccDate: %hu\n", d->DIR_LstAccDate);
    printf("FstClusHI: %hu\n", d->DIR_FstClusHI);
    printf("WrtTime: %hu\n", d->DIR_WrtTime);
    printf("WrtDate: %hu\n", d->DIR_WrtDate);
    printf("FstClusLO: %hu\n", d->DIR_FstClusLO);
    printf("FileSize: %u\n", d->DIR_FileSize);

}

void printLS(struct DIR* d)
{
    int i;
    unsigned char tDirName[12];
    for(i = 0; i < 11; i++)
        tDirName[i] = d->DIR_Name[i];
    tDirName[11] = '\0';
    if(d->DIR_Name[0]!= 0xE5)
    {
        if(d->DIR_Attr == 0x10)
            printf("\tDIR %s\n", tDirName);
        else
            printf("\t%s\n", tDirName);
    }
}

void printSZ(struct DIR* d)
{
    printf("Filesize: %d bytes\n", d->DIR_FileSize);
}

int checkForFileError(FILE * f)
{
    if(f == NULL)
    {
        printf("FATAL ERROR: Cannot open .img\n");
        exit(EXIT_FAILURE);
    }
}

int showEntry(struct DIR * entry)
{
    puts("First Cluster\n");
    printf("DIR_FstClusLO%02x, DIR_FstClusHI%02x \n", entry->DIR_FstClusLO, entry->DIR_FstClusHI);

    int x;
    for(x = 0; x < 11; x++)
    {
        if(entry->DIR_Name[x] == ' ')
            printf("filename[%d]->%s\n", x, "SPACE");
        else
            printf("filename[%d]->%c\n", x, entry->DIR_Name[x]);
    }

    printf("\nattr->0x%x, size->0x%x ", entry->DIR_Attr, entry->DIR_FileSize);


}

int createEntry(struct DIR * entry,
                const char * filename,
                const char * ext,
                int isDir,
                uint32_t firstCluster,
                uint32_t filesize,
                bool emptyAfterThis,
                bool emptyDirectory)
{
    //set the same no matter the entry
    //   entry->r1 = 0;
    entry->DIR_NTRes = 0;
    entry->DIR_CrtTimeTenth = 0;
    entry->DIR_CrtTime = 0;
    entry->DIR_CrtDate = 0;
    entry->DIR_LstAccDate = 0;
    entry->DIR_WrtTime = 0;
    entry->DIR_WrtDate = 0;

    if(!emptyAfterThis && !emptyDirectory)   //if both are false
    {
        int x;
        for(x = 0; x < MAX_FILENAME_SIZE; x++)
        {
            if(x < strlen(filename))
                entry->DIR_Name[x] = filename[x];
            else
                entry->DIR_Name[x] = ' ';
        }

        for(x = 0; x < MAX_EXTENTION_SIZE; x++)
        {
            if(x < strlen(ext))
                entry->DIR_Name[MAX_FILENAME_SIZE + x] = ext[x];
            else
                entry->DIR_Name[MAX_FILENAME_SIZE + x] = ' ';
        }

        deconstructClusterAddress(entry, firstCluster);

        if(isDir)
        {
            entry->DIR_FileSize = 0;
            entry->DIR_Attr = ATTR_DIRECTORY;
        }
        else
        {
            entry->DIR_FileSize = filesize;
            entry->DIR_Attr = ATTR_ARCHIVE;
        }
        return 0; //stops execution so we don't flow out into empty entry config code below

    }
    else if(emptyAfterThis)     //if this isn't true, then the other must be
    {
        entry->DIR_Name[0] = 0xE5;
        entry->DIR_Attr = 0x00;
    }
    else                                //hence no condition check here
    {
        entry->DIR_Name[0] = 0x00;
        entry->DIR_Attr = 0x00;
    }

    //if i made it here we're creating an empty entry and both conditions
    //require this setup
    int x;
    for(x = 1; x < 11; x++)
        entry->DIR_Name[x] = 0x00;

    entry->DIR_FstClusLO = 0x00;
    entry->DIR_FstClusHI = 0x00;
    entry->DIR_Attr = 0x00;
    entry->DIR_FileSize = 0;

    return 0;
}


//Takes a cluster number where bunch of directories are and the offst of the directory you want read and it will store that directory info into the variable dir
struct DIR * readEntry(struct BPB * bs, struct DIR * entry, uint32_t clusterNum, int offset)
{
    offset *= 32;

    uint32_t dataAddress = byteOffsetOfCluster(clusterNum);
    FILE* f = fopen(environment.imageName, "r");
    checkForFileError(f);
    fseek(f, dataAddress + offset, 0);
    fread(entry, sizeof(struct DIR), 1, f);
    //printf("entry.DIR_FileSize%d\n", entry->DIR_FileSize);
    fclose(f);
    return entry;
}

struct DIR * writeEntry(struct BPB * bs, struct DIR * entry, uint32_t clusterNum, int offset)
{
    offset *= 32;
    uint32_t dataAddress = byteOffsetOfCluster(clusterNum);
    FILE* f = fopen(environment.imageName, "r+");
    checkForFileError(f);

    fseek(f, dataAddress + offset, 0);
    fwrite(entry, 1, sizeof(struct DIR), f);
    fclose(f);
    return entry;
}

//Searches the open directory table for entry if it finds it, it returns the name of the parent directory, else it returns an empty string
const char * TBL_getParent(const char * dirName)
{
    if(environment.tbl_dirStatsSize > 0)
    {
        int x;
        for(x = 0; x < environment.tbl_dirStatsSize; x++)
        {
            if(strcmp(environment.dirStatsTbl[x % TBL_DIR_STATS].filename, dirName) == 0)
                return environment.dirStatsTbl[x % TBL_DIR_STATS].parent;
        }
        return "";
    }
    else
    {
        return "";
    }
}

//Pass in a entry and this properly formats the "firstCluster" from the 2 byte segments in the file structure
uint32_t buildClusterAddress(struct DIR * entry)
{
    uint32_t firstCluster = 0x00000000;
    firstCluster |=  entry->DIR_FstClusHI << 16;
    firstCluster |=  entry->DIR_FstClusLO;
    return firstCluster;
}


int deconstructClusterAddress(struct DIR * entry, uint32_t cluster)
{
    entry->DIR_FstClusLO = cluster;
    entry->DIR_FstClusHI = cluster >> 16;
    return 0;
}

struct FILEDESCRIPTOR * makeFileDecriptor(struct DIR * entry, struct FILEDESCRIPTOR * fd)
{
    char newFilename[12];
    bzero(fd->filename, 9);
    bzero(fd->extention, 4);

    memcpy(newFilename, entry->DIR_Name, 11);
    newFilename[11] = '\0';
    int x;
    for(x = 0; x < 8; x++)
    {
        if(newFilename[x] == ' ')
            break;
        fd->filename[x] = newFilename[x];
    }
    fd->filename[++x] = '\0';
    for(x = 8; x < 11; x++)
    {
        if(newFilename[x] == ' ')
            break;
        fd->extention[x - 8] = newFilename[x];
    }
    fd->extention[++x - 8] = '\0';
    if(strlen(fd->extention) > 0)
    {
        strcpy(fd->fullFilename, fd->filename);
        strcat(fd->fullFilename, ".");
        strcat(fd->fullFilename, fd->extention);
    }
    else
    {
        strcpy(fd->fullFilename, fd->filename);
    }
    fd->firstCluster = buildClusterAddress(entry);
    fd->size = entry->DIR_FileSize;
    fd->mode = MODE_UNKNOWN;
    if((entry->DIR_Attr & ATTR_DIRECTORY) == ATTR_DIRECTORY)
        fd->dir = TRUE;
    else
        fd->dir = FALSE;
    return fd;
}

struct FILEDESCRIPTOR * TBL_createFile(
    const char * filename,
    const char * extention,
    const char * parent,
    uint32_t firstCluster,
    int mode,
    uint32_t size,
    int dir,
    int isOpen)
{
    struct FILEDESCRIPTOR * newFile = (struct FILEDESCRIPTOR *) malloc(sizeof(struct FILEDESCRIPTOR));
    strcpy(newFile->filename, filename);
    strcpy(newFile->extention, extention);
    strcpy(newFile->parent, parent);

    if(strlen(newFile->extention) > 0)
    {
        strcpy(newFile->fullFilename, newFile->filename);
        strcat(newFile->fullFilename, ".");
        strcat(newFile->fullFilename, newFile->extention);
    }
    else
    {
        strcpy(newFile->fullFilename, newFile->filename);
    }

    newFile->firstCluster = firstCluster;
    newFile->mode = mode;
    newFile->size = size;
    newFile->dir = dir;
    newFile->isOpen = isOpen;
    return newFile;
}

uint32_t getLastClusterInChain(struct BPB * bs, uint32_t firstClusterVal)
{
    int size = 1;
    uint32_t lastCluster = firstClusterVal;
    firstClusterVal = (int) getFatEntry(firstClusterVal);
    if((((firstClusterVal & MASK_IGNORE_MSB) | EMPTY_CLUSTER) == EMPTY_CLUSTER) )
        return lastCluster;
    while((firstClusterVal = (firstClusterVal & MASK_IGNORE_MSB)) < EOC)
    {
        lastCluster = firstClusterVal;
        firstClusterVal = getFatEntry(firstClusterVal);
    }
    return lastCluster;

}

int FAT_findFirstFreeCluster(struct BPB * bs)
{
    int i = 0;
    int totalClusters = countOfClusters(bs);
    while(i < totalClusters)
    {
        if(((getFatEntry(i) & MASK_IGNORE_MSB) | EMPTY_CLUSTER) == EMPTY_CLUSTER)
            break;
        i++;
    }
    return i;
}

uint32_t FAT_extendClusterChain(struct BPB * bs,  uint32_t clusterChainMember)
{
    uint32_t firstFreeCluster = FAT_findFirstFreeCluster(bs);
    uint32_t lastClusterinChain = getLastClusterInChain(bs, clusterChainMember);

    FAT_writeFatEntry(bs, lastClusterinChain, &firstFreeCluster);
    FAT_writeFatEntry(bs, firstFreeCluster, &FAT_EOC);
    return firstFreeCluster;
}

int FAT_allocateClusterChain(struct BPB * bs,  uint32_t clusterNum)
{
    FAT_writeFatEntry(bs, clusterNum, &FAT_EOC);
    return 0;
}

int FAT_freeClusterChain(struct BPB * bs,  uint32_t firstClusterOfChain)
{
    int dirSizeInCluster = getFileSizeInClusters(firstClusterOfChain);
    int currentCluster= firstClusterOfChain;
    int nextCluster;
    int clusterCount;

    for(clusterCount = 0; clusterCount < dirSizeInCluster; clusterCount++)
    {
        nextCluster = getFatEntry( currentCluster);
        FAT_writeFatEntry(bs, currentCluster, &FAT_FREE_CLUSTER);
        currentCluster = nextCluster;
    }
    return 0;

}

int FAT_setIoWriteCluster(struct BPB * bs, uint32_t clusterChainMember)
{
    environment.io_writeCluster = getLastClusterInChain(bs, clusterChainMember);
    return 0;
}

int TBL_addFileToTbl(struct FILEDESCRIPTOR * file, int isDir)
{
    if(isDir == TRUE)
    {
        if(environment.tbl_dirStatsSize < TBL_DIR_STATS)
        {
            environment.dirStatsTbl[environment.tbl_dirStatsIndex % TBL_DIR_STATS] = *file;
            environment.tbl_dirStatsSize++;
            environment.tbl_dirStatsIndex = ++environment.tbl_dirStatsIndex % TBL_DIR_STATS;
            return 0;
        }
        else
        {
            environment.dirStatsTbl[environment.tbl_dirStatsIndex % TBL_DIR_STATS] = *file;
            environment.tbl_dirStatsIndex = ++environment.tbl_dirStatsIndex % TBL_DIR_STATS;
            return 0;
        }


    }
    else
    {
        if(environment.tbl_openFilesSize < TBL_OPEN_FILES)
        {
            environment.openFilesTbl[environment.tbl_openFilesIndex % TBL_OPEN_FILES] = *file;
            environment.tbl_openFilesSize++;
            environment.tbl_openFilesIndex = ++environment.tbl_openFilesIndex % TBL_OPEN_FILES;
            return 0;
        }
        else
        {
            environment.openFilesTbl[environment.tbl_openFilesIndex % TBL_OPEN_FILES] = *file;
            environment.tbl_openFilesIndex = ++environment.tbl_openFilesIndex % TBL_OPEN_FILES;
            return 0;
        }

    }
}

bool searchOrPrintFileSize(struct BPB * bs, const char * fileName, bool useAsFileSearch, bool searchForDirectory, struct FILEDESCRIPTOR * searchFile)
{
    if(multifun(bs, environment.pwd_cluster, TRUE, fileName, FALSE, searchFile, TRUE, searchForDirectory) == TRUE)
    {
        if(useAsFileSearch == FALSE)
            printf("File: %s is %d byte(s) in size", fileName,searchFile->size);
        return TRUE;
    }
    else
    {
        if(useAsFileSearch == FALSE)
            printf("ERROR: File: %s was not found", fileName);
        return FALSE;
    }
}

bool searchForFile(struct BPB * bs, const char * fileName, bool searchForDirectory, struct FILEDESCRIPTOR * searchFile)
{
    return searchOrPrintFileSize(bs, fileName, TRUE, searchForDirectory, searchFile);
}

bool TBL_getFileDescriptor(int * index, const char * filename, bool isDir)
{
    struct FILEDESCRIPTOR  * b;

    if(isDir == TRUE)
    {
        if(environment.tbl_dirStatsSize > 0)
        {

            int x;
            for(x = 0; x < environment.tbl_dirStatsSize; x++)
            {
                if(strcmp(environment.dirStatsTbl[x % TBL_DIR_STATS].fullFilename, filename) == 0)
                {
                    *index = x;
                    return TRUE;
                }
            }

        }
        else
            return FALSE;
    }
    else
    {
        if(environment.tbl_openFilesSize > 0)
        {

            int x;
            for(x = 0; x < environment.tbl_openFilesSize; x++)
            {
                if(strcmp(environment.openFilesTbl[x % TBL_OPEN_FILES].fullFilename, filename) == 0)
                {
                    *index = x;
                    return TRUE;
                }
            }

        }
        else
            return FALSE;
    }

    return FALSE;
}

int fOpen(struct BPB * bs, int argC, const char * filename, const char * mode, char * modeName)
{

    struct FILEDESCRIPTOR searchFileInflator;
    struct FILEDESCRIPTOR * searchFile = &searchFileInflator;
    struct FILEDESCRIPTOR * fileTblEntry;
    int fileMode;
    int fileTblIndex;

    if(argC != 3)
    {
        printf("Usage: open <filename> <r | w | rw | wr>\n");
        return 0;
    }

    if(searchForFile(bs, filename, TRUE, searchFile))
    {
        printf("\"%s\" is a directory\n", filename);
        return 0;
    }

    if(!searchForFile(bs, filename, FALSE, searchFile))
    {
        printf("File \"%s\" does not exsit\n", filename);
        return 0;
    }

    if(TBL_getFileDescriptor(&fileTblIndex, filename, FALSE)&& environment.openFilesTbl[fileTblIndex].isOpen)
    {
        printf("File \"%s\" is already open\n", filename);
        return 0;
    }

    if(strcmp(mode, "w") == 0)
    {
        fileMode = MODE_WRITE;
        strcpy(modeName, "Writing");
    }
    else if(strcmp(mode, "r") == 0)
    {
        fileMode = MODE_READ;
        strcpy(modeName, "Reading");

    }
    else if(strcmp(mode, "rw") == 0)
    {
        fileMode = MODE_BOTH;
        strcpy(modeName, "Reading and Writing");
    }
    else if(strcmp(mode, "wr") == 0)
    {
        fileMode = MODE_BOTH;
        strcpy(modeName, "Writing and Reading");
    }
    else
    {
        printf("\"%s\" is an invalid mode\n", mode);
        printf("Usage: open <filename> <r | w | rw | wr>\n");
        return 0;
    }

    if(!TBL_getFileDescriptor(&fileTblIndex, filename, FALSE))
    {
        TBL_addFileToTbl(TBL_createFile(searchFile->filename, searchFile->extention, environment.pwd, searchFile->firstCluster, fileMode, searchFile->size, FALSE, TRUE), FALSE);
    }
    else
    {

        environment.openFilesTbl[fileTblIndex].isOpen = TRUE;
        uint32_t offset;
        struct DIR entry;
        if( ( offset = getEntryOffset(bs, filename) ) == -1)
        {
            printf("Open success, but the filetable could not be updated\n");
            return 0;
        }

        readEntry(bs, &entry, environment.pwd_cluster, offset);
        environment.openFilesTbl[fileTblIndex].size = entry.DIR_FileSize;
        environment.openFilesTbl[fileTblIndex].mode = fileMode;

    }

    printf("Open success, file \"%s\" opened for %s\n", filename, modeName);
    return 1;
}

int fClose(struct BPB * bs, int argC, const char * filename)
{
    int fIndex;
    struct FILEDESCRIPTOR searchFileInflator;
    struct FILEDESCRIPTOR * searchFile = &searchFileInflator;
    if(argC != 2)
    {
        printf("Usage: close <filename>\n");
        return 0;
    }

    if(!searchForFile(bs, filename, FALSE, searchFile))
    {
        printf("File \"%s\" does not exsit\n", filename);
        return 0;
    }

    if(!TBL_getFileDescriptor(&fIndex, filename, FALSE))
    {
        printf("File \"%s\" has never been opened\n", filename);
        return 0;
    }

    if(TBL_getFileDescriptor(&fIndex,filename, FALSE)&& !environment.openFilesTbl[fIndex].isOpen )
    {
        printf("File \"%s\" is currently closed\n", filename);
        return 0;
    }

    environment.openFilesTbl[fIndex].isOpen = FALSE;
    printf("Close success, file \"%s\" has been closed\n", filename);
    return 1;
}

//File read
int fRead(struct BPB * bs, int argC, const char * filename, const char * position, const char * numberOfBytes, uint32_t * actualBytesRead)
{
    int fileTblIndex;
    struct FILEDESCRIPTOR searchFileInflator;
    struct FILEDESCRIPTOR * searchFile = &searchFileInflator;
    struct DIR entry;
    uint32_t offset;
    uint32_t pos = atoi(position);
    uint32_t dataSize = atoi(numberOfBytes);
    uint32_t firstCluster;
    uint32_t currentCluster;
    uint32_t fileSize;

    if(argC != 4)
    {
        printf("Usage: read <filename> <offset> <size>\n");
        return 0;
    }
    if(searchForFile(bs, filename, TRUE, searchFile))
    {
        printf("\"%s\" is a directory\n", filename);
        return 0;
    }
    if(!searchForFile(bs, filename, FALSE, searchFile))
    {
        printf("File \"%s\" does not exist.\n", filename);
        return 0;
    }
    if(!TBL_getFileDescriptor(&fileTblIndex, filename, FALSE))
    {
        printf("File \"%s\" has never been opened\n", filename);
        return 0;
    }
    if(TBL_getFileDescriptor(&fileTblIndex, filename, FALSE) && !environment.openFilesTbl[fileTblIndex].isOpen)
    {
        printf("File \"%s\" is not open\n", filename);
        return 0;
    }
    if(pos > environment.openFilesTbl[fileTblIndex].size)
    {
        printf("Read failed on \"%s\" offset > filesize\n", filename);
        return 0;
    }
    if(environment.openFilesTbl[fileTblIndex].mode != MODE_READ && environment.openFilesTbl[fileTblIndex].mode != MODE_BOTH)
    {
        printf("Read failed, \"%s\" is not open for reading \n", filename);
        return 0;
    }

    FILE* f = fopen(environment.imageName, "r");
    checkForFileError(f);
    fileSize = environment.openFilesTbl[fileTblIndex].size;
    *actualBytesRead = fileSize;
    firstCluster = currentCluster = environment.openFilesTbl[fileTblIndex].firstCluster ;
    FREAD_readData(bs, firstCluster, pos, dataSize, fileSize);

    fclose(f);
    return 1;
}

int fWrite(struct BPB * bs, int argC, const char * filename, const char * position, const char * data)
{

    int fileTblIndex;
    struct FILEDESCRIPTOR searchFileInflator;
    struct FILEDESCRIPTOR * searchFile = &searchFileInflator;
    struct DIR entry;
    uint32_t offset;

    uint32_t pos = atoi(position);
    uint32_t firstCluster;
    uint32_t currentCluster;
    uint32_t dataSize = strlen(data);
    uint32_t fileSize;
    uint32_t newSize;

    uint32_t totalSectors;
    uint32_t additionalsectors;
    uint32_t paddingClusterOffset;

    uint32_t remainingClustersToWalk;

    uint32_t paddingLength = 0;
    uint32_t paddingRemaining;

    uint32_t usedBytesInLastSector = 0;
    uint32_t openBytesInLastSector = 0;
    uint32_t startWritePos;

    uint32_t paddingWriteLength;

    uint32_t fileWriteOffset;

    char * dataChar;
    int x;
    if(searchForFile(bs, filename, TRUE, searchFile))
    {
        printf("\"%s\" is a directory\n", filename);
        return 0;
    }

    if(!searchForFile(bs, filename, FALSE, searchFile))
    {
        printf("File \"%s\" does not exist \n", filename);
        return 0;
    }

    if(!TBL_getFileDescriptor(&fileTblIndex, filename, FALSE))
    {
        printf("File \"%s\" has never been opened\n", filename);
        return 0;
    }

    if(TBL_getFileDescriptor(&fileTblIndex, filename, FALSE) && !environment.openFilesTbl[fileTblIndex].isOpen)
    {
        printf("File \"%s\" is not open\n", filename);
        return 0;
    }

    if(dataSize < 1)
    {
        printf("Write failed on \"%s\" no data to be written\n", filename);
        return 0;
    }

    if(environment.openFilesTbl[fileTblIndex].mode != MODE_WRITE && environment.openFilesTbl[fileTblIndex].mode != MODE_BOTH)
    {
        printf("File \"%s\" is not open for writing \n", filename);
        return 0;
    }

    FILE* f = fopen(environment.imageName, "r+");
    checkForFileError(f);

    if(searchFile->firstCluster == 0)
    {
        if( ( offset = getEntryOffset(bs, filename) ) == -1)
        {
            printf("\nFile \"%s\" could not be allocated space \n", filename);
            return 0;
        }
        readEntry(bs,  &entry, environment.pwd_cluster, offset);

        firstCluster = FAT_findFirstFreeCluster(bs);
        FAT_allocateClusterChain(bs, firstCluster);

        deconstructClusterAddress(&entry, firstCluster);

        environment.openFilesTbl[fileTblIndex].firstCluster = firstCluster;
        environment.openFilesTbl[fileTblIndex].size = 0;
        writeEntry(bs,  &entry, environment.pwd_cluster, offset);
    }
    else
    {

        fileSize = environment.openFilesTbl[fileTblIndex].size;
        firstCluster = currentCluster = environment.openFilesTbl[fileTblIndex].firstCluster;
    }

    uint32_t fileSizeInClusters = getFileSizeInClusters(firstCluster);


    if((pos + dataSize) > fileSize)
    {
        newSize = pos + dataSize;

        totalSectors = newSize / bs->BPB_BytsPerSec;
        if(newSize > 0 && (newSize % bs->BPB_BytsPerSec != 0))
        {
            totalSectors++;
        }

        additionalsectors = totalSectors - fileSizeInClusters;

        for(x = 0; x < additionalsectors; x++)
        {
            FAT_extendClusterChain(bs, firstCluster);
        }

        fileSizeInClusters = getFileSizeInClusters(firstCluster);

        if(pos > fileSize)
            paddingLength = pos - fileSize;
        else
            paddingLength = 0;

        paddingRemaining = paddingLength;
        if(fileSize > 0)
        {
            if((fileSize % bs->BPB_BytsPerSec) == 0)
                usedBytesInLastSector = bs->BPB_BytsPerSec;
            else
                usedBytesInLastSector = fileSize % bs->BPB_BytsPerSec;
        }

        openBytesInLastSector = bs->BPB_BytsPerSec - usedBytesInLastSector;
        if(paddingLength > 0)
        {
            paddingClusterOffset = (fileSize / bs->BPB_BytsPerSec);
            remainingClustersToWalk = fileSizeInClusters - paddingClusterOffset;
            for(x = 0; x < paddingClusterOffset; x++)
                currentCluster = getFatEntry(currentCluster);

            uint8_t padding[1] = {0x20};
            for(x = 0; x < remainingClustersToWalk; x++)
            {
                if(x == 0)
                {
                    startWritePos = byteOffsetOfCluster(currentCluster) + usedBytesInLastSector;
                    if((usedBytesInLastSector + paddingRemaining) > bs->BPB_BytsPerSec)
                        paddingWriteLength = openBytesInLastSector;
                    else
                        paddingWriteLength = paddingRemaining;

                }
                else
                {
                    startWritePos = byteOffsetOfCluster(currentCluster);
                    if(paddingRemaining > bs->BPB_BytsPerSec)
                        paddingWriteLength = bs->BPB_BytsPerSec;
                    else
                        paddingWriteLength = paddingRemaining;


                }
                for(fileWriteOffset = 0; fileWriteOffset < paddingWriteLength ; fileWriteOffset++)
                {
                    fseek(f, startWritePos + fileWriteOffset, 0);
                    fwrite(padding, 1, 1, f);
                }

                paddingRemaining -= paddingWriteLength;
                if(paddingRemaining == 0)
                {
                    break;
                }
                currentCluster = getFatEntry(currentCluster);
            }
        }

        FWRITE_writeData(bs, firstCluster, pos, data, dataSize);

    }
    else
    {
        newSize = fileSize;
        FWRITE_writeData(bs, firstCluster, pos, data, dataSize);
    }

    if(fileSize != newSize)
    {
        if( ( offset = getEntryOffset(bs, filename) ) == -1)
        {
            printf("Write success, but the file size of \"%s\" was not updated \n", filename);
            return 0;
        }
        readEntry(bs,  &entry, environment.pwd_cluster, offset);
        entry.DIR_FileSize = newSize;
        environment.openFilesTbl[fileTblIndex].size = newSize;
        writeEntry(bs,  &entry, environment.pwd_cluster, offset);
    }
    fclose(f);
    return 1;

}

bool handleAbsolutePaths(struct BPB * bs, int argcount, uint32_t * targetCluster, char * path, char * successFilename, char * failFilename, bool isCd, bool searchForFile, bool isMkdir)
{
    int x;
    char * paths[400];
    int pathsArgC;
    bool isValidPath = TRUE;
    struct FILEDESCRIPTOR searchFileInflator;
    struct FILEDESCRIPTOR * searchFile = &searchFileInflator;
    uint32_t pwdCluster = *targetCluster;
    uint32_t previousCluster;

    if(strcmp(path, ".") == 0)
    {
        *targetCluster = environment.pwd_cluster;
        strcpy(successFilename, environment.pwd);
        return TRUE;
    }
    if(path[0] == ROOT[0])
        pwdCluster = *targetCluster = bs->BPB_RootEntCnt;
    pathsArgC = argcount;
    if(searchForFile == TRUE)
        pathsArgC -= 1;

    for(x = 0; x < pathsArgC; x++)
    {
        if( multifun(bs, *targetCluster, TRUE, paths[x], FALSE, searchFile, TRUE, TRUE) == TRUE )
        {
            previousCluster = *targetCluster;
            *targetCluster = searchFile->firstCluster;
        }
        else
        {
            strcpy(failFilename, paths[x]);
            if(isCd == TRUE)
                isValidPath = FALSE;
            else
                return FALSE;
            break;
        }
    }
    if(isCd == TRUE)
    {

        if(isValidPath == FALSE)
            return FALSE;
        else
        {
            *targetCluster = pwdCluster;
            for(x = 0; x < pathsArgC; x++)
            {
                multifun(bs, *targetCluster, TRUE, paths[x], FALSE, searchFile, FALSE, FALSE);
                *targetCluster = searchFile->firstCluster;
                strcpy(environment.pwd, paths[x]);
            }
            return TRUE;
        }

    }
    else
    {
        if(searchForFile == TRUE)
            strcpy(successFilename, paths[pathsArgC]);
        else
            strcpy(successFilename, paths[pathsArgC - 1]);

        if(isMkdir == TRUE)
            *targetCluster = previousCluster;

        return TRUE;

    }
}

void allocateFileTable()
{
    environment.dirStatsTbl =  malloc(TBL_DIR_STATS * sizeof(struct FILEDESCRIPTOR));
    int i;
    for (i = 0; i < TBL_DIR_STATS; i++)
        environment.dirStatsTbl[i] = *( (struct FILEDESCRIPTOR *) malloc( sizeof(struct FILEDESCRIPTOR) ));
    environment.openFilesTbl =  malloc(TBL_OPEN_FILES * sizeof(struct FILEDESCRIPTOR));
    for (i = 0; i < TBL_OPEN_FILES; i++)
        environment.openFilesTbl[i] = *( (struct FILEDESCRIPTOR *) malloc( sizeof(struct FILEDESCRIPTOR) ));
}

int initEnvironment(const char * imageName, struct BPB * boot_sector)
{
    strcpy(ROOT, "/");
    strcpy(PARENT, "..");
    strcpy(environment.imageName, imageName);
    FILE* f = fopen(imageName, "r");
    if(f == NULL)
    {
        return 1;
    }
    strcpy(environment.imageName, imageName);
    fread(boot_sector, sizeof(struct BPB), 1,f);
    fclose(f);
    strcpy(environment.pwd, ROOT);
    strcpy(environment.last_pwd, ROOT);

    environment.pwd_cluster = boot_sector->BPB_RootClus;
    environment.tbl_dirStatsIndex = 0;
    environment.tbl_dirStatsSize = 0;
    environment.tbl_openFilesIndex = 0;
    environment.tbl_openFilesSize = 0;

    FAT_setIoWriteCluster(boot_sector, environment.pwd_cluster);
    allocateFileTable();

    return 0;
}

int getEntryOffset(struct BPB * bs, const char * searchName)
{
    struct DIR entry;
    struct FILEDESCRIPTOR fd;
    uint32_t currentCluster = environment.pwd_cluster;
    int dirSizeInCluster = getFileSizeInClusters(currentCluster);
    int clusterCount;
    int offset;
    int increment;

    for(clusterCount = 0; clusterCount < dirSizeInCluster; clusterCount++)
    {
        if(strcmp(environment.pwd, ROOT) == 0)
        {
            offset = 1;
            increment = 2;
        }
        else
        {
            offset = 0;
            increment = 1;
        }

        for(; offset < ENTRIES_PER_SECTOR; offset += increment)
        {
            if(strcmp(environment.pwd, ROOT) != 0 && offset == 2)
            {
                increment = 2;
                offset -= 1;
                continue;
            }

            readEntry(bs, &entry, currentCluster, offset);
            makeFileDecriptor(&entry, &fd);

            if(strcmp(searchName, fd.fullFilename) == 0)
            {

                return offset;
            }
        }
        currentCluster = getFatEntry(currentCluster);
    }
    return -1;
}


int multifun(struct BPB * bs, uint32_t clusterNum, bool cd, const char * directoryName, bool goingUp, struct FILEDESCRIPTOR * searchFile, bool useAsSearch, bool searchForDir)
{
    struct DIR dir;
    int dirSizeInCluster = getFileSizeInClusters(clusterNum);
    int clusterCount;
    char fileName[13];
    int offset;
    int increment;
    for(clusterCount = 0; clusterCount < dirSizeInCluster; clusterCount++)
    {
        if(strcmp(directoryName, ROOT) == 0)
        {
            offset = 1;
            increment = 2;
        }
        else
        {
            offset = 0;
            increment = 1;
        }
        for(; offset < ENTRIES_PER_SECTOR; offset += increment)
        {

            if(strcmp(directoryName, ROOT) != 0 && offset == 2)
            {
                increment = 2;
                offset -= 1;
                continue;
            }
            readEntry(bs, &dir, clusterNum, offset);
            makeFileDecriptor(&dir, searchFile);

            if( cd == FALSE )
            {
                if(searchFile->filename[0] != 0xE5)
                {
                    if(searchFile->dir == TRUE)
                        printf("dir->%s   ", searchFile->fullFilename);
                    else
                        printf("%s     ", searchFile->fullFilename);
                }
            }
            else
            {
                if(useAsSearch == TRUE)
                {
                    if(strcmp(searchFile->fullFilename, directoryName) == 0 && searchFile->dir == searchForDir )
                    {
                        return TRUE;
                    }
                }
                else
                {

                    if(searchFile->dir == TRUE && strcmp(searchFile->fullFilename, directoryName) == 0 && goingUp == FALSE)
                    {
                        environment.pwd_cluster = searchFile->firstCluster;
                        if(strcmp(TBL_getParent(directoryName), "") == 0)
                            TBL_addFileToTbl(TBL_createFile(searchFile->filename, "", environment.pwd, searchFile->firstCluster, searchFile->mode, searchFile->size, TRUE, FALSE), TRUE);
                        FAT_setIoWriteCluster(bs, environment.pwd_cluster);
                        return TRUE;
                    }
                    if(searchFile->dir == TRUE && strcmp(directoryName, PARENT) == 0 && goingUp == TRUE)
                    {
                        const char * parent = TBL_getParent(environment.pwd);
                        if(strcmp(parent, "") != 0)
                        {

                            if(strcmp(parent, "/") == 0)
                                environment.pwd_cluster = 2;
                            else
                                environment.pwd_cluster = searchFile->firstCluster;

                            strcpy(environment.last_pwd, environment.pwd);
                            strcpy(environment.pwd, TBL_getParent(environment.pwd));
                            FAT_setIoWriteCluster(bs, environment.pwd_cluster);

                            return TRUE;
                        }
                        else
                            return FALSE;
                    }

                }
            }
        }
        clusterNum = getFatEntry(clusterNum);
    }
    return FALSE;
}

int rm(struct BPB * bs, const char * searchName, uint32_t searchDirectoryCluster)
{
    struct DIR entry;
    struct FILEDESCRIPTOR fd;
    struct FILEDESCRIPTOR fileTableFd;
    uint32_t currentCluster = searchDirectoryCluster;
    int dirSizeInCluster = getFileSizeInClusters(currentCluster);
    int fileTblIndex;
    int clusterCount;
    char fileName[12];
    int offset;
    int increment;

    uint32_t hitCluster;
    uint32_t hitFileFirstCluster;
    int entriesAfterHit = 0;
    int searchHitOffset = 0;
    bool hasSearchHit = FALSE;


    for(clusterCount = 0; clusterCount < dirSizeInCluster; clusterCount++)
    {
        if(strcmp(environment.pwd, ROOT) == 0)
        {
            offset = 1;
            increment = 2;
        }
        else
        {
            offset = 0;
            increment = 1;
        }

        for(; offset < ENTRIES_PER_SECTOR; offset += increment)
        {
            if(strcmp(environment.pwd, ROOT) != 0 && offset == 2)
            {
                increment = 2;
                offset -= 1;
                continue;
            }
            readEntry(bs, &entry, currentCluster, offset);

            makeFileDecriptor(&entry, &fd);


            if(strcmp(searchName, fd.fullFilename) == 0)
            {

                if(entry.DIR_Attr == ATTR_DIRECTORY)
                    return 2;

                if( TBL_getFileDescriptor(&fileTblIndex, fd.fullFilename, FALSE) == TRUE )
                {
                    if(environment.openFilesTbl[fileTblIndex].isOpen == TRUE)
                        return 1;
                }

                searchHitOffset = offset;
                hitCluster = currentCluster;
                hasSearchHit = TRUE;
                hitFileFirstCluster = fd.firstCluster;
                continue;
            }

            if(hasSearchHit)
            {
                if( isEntryEmpty(&fd) == FALSE )
                {
                    entriesAfterHit++;
                }
            }

        }

        currentCluster = getFatEntry(currentCluster);

    }
    struct DIR entryToWrite;
    if(hasSearchHit == TRUE)
    {
        if(entriesAfterHit == 0)
        {

            createEntry(&entryToWrite, "", "", FALSE, 0, 0, FALSE, TRUE);
            writeEntry(bs, &entryToWrite, hitCluster, searchHitOffset);

            if(hitFileFirstCluster != 0)
                FAT_freeClusterChain(bs, hitFileFirstCluster);
            return 0;
        }
        else
        {

            createEntry(&entryToWrite, "", "", FALSE, 0, 0, TRUE, FALSE);
            writeEntry(bs, &entryToWrite, hitCluster, searchHitOffset);
            if(hitFileFirstCluster != 0)
                FAT_freeClusterChain(bs, hitFileFirstCluster);
            return 0;
        }
    }
    else
    {
        return 3;
    }
}

int rmDir(struct BPB * bs, const char * searchName, uint32_t searchDirectoryCluster)
{
    struct DIR entry;
    struct FILEDESCRIPTOR fd;
    uint32_t currentCluster = searchDirectoryCluster;
    int dirSizeInCluster = getFileSizeInClusters(currentCluster);

    int clusterCount;
    char fileName[12];
    int offset;
    int increment;
    uint32_t hitCluster;
    uint32_t hitFileFirstCluster;
    int entriesAfterHit = 0;
    int searchHitOffset = 0;
    bool hasSearchHit = FALSE;

    for(clusterCount = 0; clusterCount < dirSizeInCluster; clusterCount++)
    {
        if(strcmp(searchName, ROOT) == 0)
        {
            offset = 1;
            increment = 2;
        }
        else
        {
            offset = 0;
            increment = 1;
        }

        for(; offset < ENTRIES_PER_SECTOR; offset += increment)
        {
            if(strcmp(searchName, ROOT) != 0 && offset == 2)
            {
                increment = 2;
                offset -= 1;
                continue;
            }

            readEntry(bs, &entry, currentCluster, offset);
            makeFileDecriptor(&entry, &fd);

            if(strcmp(searchName, fd.fullFilename) == 0)
            {

                if( entry.DIR_Attr != ATTR_DIRECTORY )
                    return 2;

                searchHitOffset = offset;
                hitCluster = currentCluster;
                hasSearchHit = TRUE;
                hitFileFirstCluster = fd.firstCluster;
                continue;
            }

            if(hasSearchHit)
            {
                if(!isEntryEmpty(&fd))
                {
                    entriesAfterHit++;
                }
            }

        }

        currentCluster = getFatEntry(currentCluster);

    }

    if(hasSearchHit)
    {
        offset = 0;
        increment = 1;
        for(; offset < ENTRIES_PER_SECTOR; offset += increment)
        {
            if(strcmp(searchName, ROOT) != 0 && offset == 2)
            {
                increment = 2;
                offset -= 1;
                continue;
            }
            readEntry(bs, &entry, hitFileFirstCluster, offset);
            makeFileDecriptor(&entry, &fd);

            if(isEntryEmpty(&fd) == FALSE)
            {
                if( (strcmp(".", fd.filename) != 0 ) && ( strcmp("..", fd.filename) != 0 ) )
                {
                    return 1;
                }
            }
        }
    }

    if(hasSearchHit)
    {
        struct DIR entryToWrite;
        if(entriesAfterHit == 0)
        {
            createEntry(&entryToWrite, "", "", FALSE, 0, 0, TRUE, FALSE);
            writeEntry(bs, &entryToWrite, hitCluster, searchHitOffset);
            FAT_freeClusterChain(bs, hitFileFirstCluster);
            return 0;
        }
        else
        {
            createEntry(&entryToWrite, "", "", FALSE, 0, 0, FALSE, TRUE);
            writeEntry(bs, &entryToWrite, hitCluster, searchHitOffset);
            FAT_freeClusterChain(bs, hitFileFirstCluster);
            return 0;
        }
    }


    return 3;

}

int toArgs(char* input)
{
    char *const space = " ";
    char *saveptr;
    int i = 0;
    input[strlen(input)-1] = '\0';
    if(!strcmp(input, ""))
        return 0;
    for(char *token = strtok_r(input, space, &saveptr); token != NULL; token = strtok_r(NULL, space, &saveptr))
    {
        args[i] = (char *) malloc(strlen(token) + 1);
        strcpy(args[i], token);
        i++;
    }

    return i;
}

int PathtoArgs(char* input)
{
    char *const space = " ";
    char *saveptr;
    int i = 0;
    input[strlen(input)-1] = '\0';
    if(!strcmp(input, ""))
        return 0;
    for(char *token = strtok_r(input, space, &saveptr); token != NULL; token = strtok_r(NULL, space, &saveptr))
    {
        pargs[i] = (char *) malloc(strlen(token) + 1);
        strcpy(pargs[i], token);
        i++;
    }

    return i;
}

void freeArgs(char** input, int argcount)
{
    int i;
    for(i = 0; i < argcount; i++)
        free(input[i]);
}
