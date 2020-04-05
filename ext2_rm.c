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
    char fileName[EXT2_NAME_LEN];
    int parentInodeNum, childInodeNum;
    struct ext2_inode *inodeTable;
    struct ext2_inode *parentInode, *childInode;

    if(argc!=3) {
        fprintf(stderr, "Usage: ext2_rm <image file name> <absolute path>\n");
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

    // get the parent directory inode
    strcpy(parentDirPath, argv[2]);
    // if it's not a absolute path
    if (parentDirPath[0]!='/') {
        fprintf(stderr, "Must be an absolute path\n");
        return ENOENT;
    // if it's a root directory
    } else if (parentDirPath[1]=='\0'){
        fprintf(stderr, "Cannot remove root directory\n");
        return ENOENT;
    // general case
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
    getFileNameFromPath(fileName, argv[2]);
    if (strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0){
        fprintf(stderr, "Cannot remove . or ..\n");
        return ENOENT;
    }
    childInodeNum = searchFileInDir(parentInode, fileName);
    if (childInodeNum == 0) {
        fprintf(stderr, "No such file or directory\n");
        return ENOENT;
    }
    childInode = &inodeTable[childInodeNum-1];

    // if the file is a directory
    if (childInode->i_mode & EXT2_S_IFDIR) {
        fprintf(stderr, "Cannot remove a directory\n");
        return ENOENT;
    }

    rm(parentInode, fileName);
}