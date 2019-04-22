#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> //For uint32_
#include <unistd.h>
#include <ctype.h>  //For toupper
#include "cmds.h"


int main(int argc, char* argv[])
{
    //Variables
    char inputBuffer[200];
    char successFilename[200];
    char failFilename[200];
    char input[MSIZE];
    int argcount, pathargcount = 0;
    unsigned int FATSz = 0 ;

    fp = fopen(argv[1], "rb+");

    updateBPB(&b);

    //Set FATSz

    if(b.BPB_FATSz16 != 0 )
    {
        FATSz = b.BPB_FATSz16;
    }
    else
    {
        FATSz = b.BPB_FATSz32;
    }


    //Initial enviroment variables
    initEnvironment(argv[1], &b);

    //Messing around with data regions
    //Get RootDirSectors and FirstDataSector
    RootDirSectors = ((b.BPB_RootEntCnt * 32)+ (b.BPB_BytsPerSec - 1)) / b.BPB_BytsPerSec; //Note that on a FAT32 volumeRootEntCnt value is always 0 -- so RootDirSectors is always 0
    FirstDataSector = b.BPB_RsvdSecCnt + (b.BPB_NumFATs * FATSz) + RootDirSectors;

    //End messing around with regions

    workingdir[0] = '/';
    workingdir[1] = '\0';
    current_cluster = b.BPB_RootClus;

    while(strcmp(input, "exit"))
    {
        printf("%s> ", workingdir);

        input[0] = '\0';
        fgets(input, 255, stdin);
        strcpy(inputBuffer, input);

        argcount = toArgs(input);

        if(!strcmp(input, "info"))
        {
            printBPB(&b);
        }
        else if(argcount == 1 && !strcmp(args[0], "ls"))
        {
            ls_size(workingdir, current_cluster, NULL, 0);
        }

        else if (!strcmp(args[0], "create"))
        {
            create(args[1], current_cluster);
        }
        else if (!strcmp(args[0], "mkdir"))
        {
            mkdir(args[1], current_cluster);
        }

        else if(argcount == 2 && !strcmp(args[0], "ls"))
        {
            //Simulate walking to path with CD and then calling ls locally
            //Handle pathing
            if(isPath(args[1]))
            {
                char tempdir[MSIZE];
                strcpy(tempdir, workingdir);
                unsigned int origin_cluster = current_cluster;
                if(resolvePath())
                {
                    ls_size(workingdir, current_cluster, NULL, 0);
                    //Reset it back to origin location
                    strcpy(workingdir, tempdir);
                    current_cluster = origin_cluster;
                }
                else
                {
                    printf("Couldn't resolve dir\n");
                }
            }
            else
            {
                char tempdir[MSIZE];
                strcpy(tempdir, workingdir);
                unsigned int origin_cluster = current_cluster;
                current_cluster = cd(current_cluster, args[1]);
                ls_size(workingdir, current_cluster, NULL, 0);
                //Reset it back to origin location
                strcpy(workingdir, tempdir);
                current_cluster = origin_cluster;
            }
        }
        else if(argcount == 2 && !strcmp(args[0], "size"))
            ls_size(workingdir, current_cluster, str_to_fat(args[1]), 1);
        else if(argcount == 2 && !strcmp(args[0], "cd"))
        {
            //Handle pathing
            if(isPath(args[1]))
            {
                resolvePath();
            }
            else
            {
                current_cluster = cd(current_cluster, args[1]);
            }
        }
        else if(!strcmp(args[0], "open"))
        {
            char modeName[21];
            fOpen(&b, argcount, args[1], args[2], modeName);
        }
        else if(!strcmp(args[0], "close"))
        {
            fClose(&b, argcount, args[1]);
        }
        else if(!strcmp(args[0], "read"))
        {
            uint32_t actualBytesRead;
            int	numBytes;
            if(argcount < 4)
            {
                printf("Usage: read <filename> <offset> <size>\n");
                continue;
            }
            else if(fRead(&b, argcount, args[1], args[2], args[3], &actualBytesRead))
            {
                numBytes = atoi(args[3]);
                if(numBytes >= actualBytesRead)
                {
                    numBytes = actualBytesRead;
                }
                printf("\nRead success, read %d bytes from \"%s\"\n", numBytes, args[1]);
            }
        }
        else if(!strcmp(args[0], "write"))
        {
            if(argcount < 4)
            {
                printf("Usage: write <filename> <offset> <string>\n");
                continue;
            }
            else
            {
                int firstByte;
                int lastByte;
                int startData = strlen(args[0]) + strlen(args[1]) + strlen(args[2]);
                startData += 3;
                int totalLen = strlen(inputBuffer)-1;
                int dataSize = totalLen - startData;
                int x;
                char data[dataSize + 1];
                for(x = 0; x < dataSize; x++)
                {
                    data[x] = inputBuffer[startData + x];
                }
                data[dataSize] = '\0';
                if(fWrite(&b,argcount,args[1], args[2], data))
                {
                    firstByte = atoi(args[2]);
                    lastByte = firstByte + strlen(data);
                    printf("Write success, wrote \"%s\" to bytes %d-%d of %s \n", data, firstByte, lastByte, args[1]);
                }
            }
        }
        else if(!strcmp(args[0], "rm"))
        {
            if(argcount != 2)
            {
                printf("Usage: rm <filename | path>\n");
                continue;
            }
            else
            {
                int returnCode;
                uint32_t targetCluster = environment.pwd_cluster;
                if(isPath(args[1]) &&
                        handleAbsolutePaths(&b, argcount, &targetCluster, args[1], successFilename, failFilename, FALSE, TRUE, FALSE))
                {
                    returnCode = rm(&b, successFilename, targetCluster);
                    strcpy(args[1], successFilename);
                }
                else
                {
                    returnCode = rm(&b, args[1], environment.pwd_cluster);
                }

                switch(returnCode)
                {
                case 0:
                    printf("rm Success: \"%s\" has been removed\n", args[1]);
                    break;
                case 1:
                    printf("ERROR: %s failed, \"%s\" is currently open\n", "rm", args[1]);
                    break;
                case 2:
                    printf("ERROR: %s failed, \"%s\" is a directory\n", "rm", args[1]);
                    break;
                case 3:
                    printf("ERROR: %s failed, \"%s\" not found\n", "rm", args[1]);
                    break;
                }
            }

        }
        else if(!strcmp(args[0], "rmdir"))
        {
            if(argcount != 2)
            {
                printf("Usage: rmdir <filename | path>\n");
                continue;
            }
            else
            {
                int returnCode;
                uint32_t targetCluster = environment.pwd_cluster;
                if(isPath(args[1]) &&
                        handleAbsolutePaths(&b, argcount, &targetCluster, args[1], successFilename, failFilename, FALSE, FALSE, TRUE))
                {
                    returnCode = rmDir(&b, successFilename, targetCluster);
                    strcpy(args[1], successFilename);
                }
                else
                {
                    returnCode = rmDir(&b, args[1], environment.pwd_cluster);
                }

                switch(returnCode)
                {
                case 0:
                    printf("rm Success: \"%s\" has been removed\n", args[1]);
                    break;
                case 1:
                    printf("ERROR: %s failed, \"%s\" not empty\n", "rmdir", args[1]);
                    break;
                case 2:
                    printf("ERROR: %s failed, \"%s\" is not a directory\n", "rmdir", args[1]);
                    break;
                case 3:
                    printf("ERROR: %s failed, \"%s\" not found\n", "rmdir", args[1]);
                    break;
                }
            }
        }

        //else if... command

        //Lastly, free args from memory
        freeArgs(args, argcount);
    }



    fclose(fp);
    return 0;
}


