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
struct ext2_inode *inodeTable;

int main(int argc, char **argv) {

    char pathTo[EXT2_NAME_LEN];
    char pathFrom[EXT2_NAME_LEN];
    char pathToCopy[EXT2_NAME_LEN];
    char pathFromCopy[EXT2_NAME_LEN];
    char parentOfPathFrom[EXT2_NAME_LEN];
    char linkName[EXT2_NAME_LEN];

    int flagged = FALSE;
    int inodeNum, parentInodeNum, childInodeNum;


    struct ext2_inode *targetInode, *parentInode, *childInode;

    if(argc!=4 && argc!=5) {
        fprintf(stderr, "Usage: ext2_ln <image file name> <optional flag -s> <absolute path> <absolute path>\n");
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
    if(argc == 4) {
        strcpy(pathTo, argv[2]);
        strcpy(pathFrom, argv[3]);
    } else {
        if (strcmp(argv[2], "-s") != 0) {
            fprintf(stderr, "Invalid Flag\n");
            exit(1);
        }
        flagged = TRUE;
        strcpy(pathTo, argv[3]);
        strcpy(pathFrom, argv[4]);
    }

    // get the inode from pathTo
    strcpy(pathToCopy, pathTo);
    if (pathToCopy[0]!='/') {
        perror("Invalid path argument");
        fprintf(stderr, "No such file or directory\n");
        return ENOENT;
    } else {
        inodeNum = getInodeFromPath(pathToCopy);
    }

    if (inodeNum == 0) {
        fprintf(stderr, "No such file or directory\n");
        return ENOENT;
    }
    targetInode = &inodeTable[inodeNum-1];
    if (targetInode->i_mode == EXT2_S_IFDIR){
        fprintf(stderr, "No link to a directory\n");
        return EISDIR;
    }

    // get the upper level inode from pathFrom
    strcpy(pathFromCopy, pathFrom);
    strcpy(parentOfPathFrom, pathFrom);

    if (parentOfPathFrom[0]!='/') {
        perror("Invalid parentDirPath");
        fprintf(stderr, "No such file or directory\n");
        return ENOENT;
    } else {
        getParentDirPath(parentOfPathFrom);
    }

    parentInodeNum = getInodeFromPath(parentOfPathFrom);
    if (parentInodeNum == 0) {
        perror("parentDirPath not exist");
        fprintf(stderr, "No such file or directory\n");
        return ENOENT;
    }
    parentInode = &inodeTable[parentInodeNum-1];

    // check whether the link has already existed
    getFileNameFromPath(linkName, pathFromCopy);
    childInodeNum = searchFileInDir(parentInode, linkName);
    if (childInodeNum != 0) {
        fprintf(stderr, "File or directory already exist\n");
        return EEXIST;
    }

    if (!flagged){

        // implementation for the hard link
        initNewDirent(parentInode, inodeNum, EXT2_FT_REG_FILE, linkName);
        // increment the link count of the target inode
        targetInode->i_links_count++;

    }else{

        // implementation for the symbolic link
        childInodeNum = initInode(EXT2_S_IFLNK)+1;
        childInode = &inodeTable[childInodeNum-1];
        childInode->i_size = strlen(pathFromCopy);

        initNewDirent(parentInode, childInodeNum, EXT2_FT_SYMLINK, linkName);

        if (childInode->i_size < 60){
            strcpy((char *)childInode->i_block, pathTo);
        }else{    
            // append path to the inode block
            int block_num = allocateNewBlock();
            strcpy((char *)getBlock(block_num), pathTo);
            childInode->i_block[0] = block_num;
        }
        
    }

    return 0;
}