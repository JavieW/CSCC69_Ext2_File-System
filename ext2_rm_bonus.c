#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include "ext2.h"
#include "utilities.h"

unsigned char *disk;

int main(int argc, char **argv) {
    char parentDirPath[EXT2_NAME_LEN];
    char parentDirPathCopy[EXT2_NAME_LEN];
    char fileName[EXT2_NAME_LEN];
    int parentInodeNum, childInodeNum;
    struct ext2_inode *inodeTable;
    struct ext2_inode *parentInode, *childInode;
    int flagged = 0;

    if(argc!=3 && argc!=4) {
        fprintf(stderr, "Usage: ext2_rm <image file name> <optional flag -r> <absolute path>\n");
        exit(1);
    }

    // read disk and get inode table
    int fd = open(argv[1], O_RDWR);
    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    inodeTable = getInodeTable();

    // read other arguments
    if(argc == 3) {
        strcpy(parentDirPath, argv[2]);
    } else {
        if (strcmp(argv[2], "-r")!=0) {
            fprintf(stderr, "Invalid Flag\n");
            exit(1);
        }
        flagged = 1;
        strcpy(parentDirPath, argv[3]);
    }
    strcpy(parentDirPathCopy, parentDirPath);

    // get the parent directory inode
    if (parentDirPath[0]!='/') {
        fprintf(stderr, "No such file or directory\n");
        return ENOENT;
    } else {
        getParentDirPath(parentDirPath);
    }
    parentInodeNum = getInodeFromPath(parentDirPath);
    if (parentInodeNum == 0) {
        fprintf(stderr, "No such file or directory\n");
        return ENOENT;
    }
    parentInode = &inodeTable[parentInodeNum-1];

    // check file exist
    getFileNameFromPath(fileName, parentDirPathCopy);
    childInodeNum = searchFileInDir(parentInode, fileName);
    if (childInodeNum == 0) {
        fprintf(stderr, "No such file or directory\n");
        return ENOENT;
    }
    childInode = &inodeTable[childInodeNum-1];

    // if the file is a directory
    if ((childInode->i_mode & EXT2_S_IFDIR) && !flagged){
        fprintf(stderr, "Cannot remove a directory without -r flag\n");
        return ENOENT;
    }

    rm(parentInode, fileName);
}