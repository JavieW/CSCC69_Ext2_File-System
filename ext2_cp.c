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
    FILE *src_fd;
    char parentDirPath[EXT2_NAME_LEN];
    char fileName[EXT2_NAME_LEN];
    int parentInodeNum, childInodeNum;
    struct ext2_inode parentInode, childInode;

    if(argc!=4) {
        fprintf(stderr, "Usage: ext2_cp <image file name> <native path> <absolute path on the disk>\n");
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

    // open native file for read
    if((src_fd = fopen(argv[2], "r")) == NULL) {
        perror("src_fd");
		fprintf(stderr, "No such file or directory\n");
        return ENOENT;
	}

    // get the parent directory inode
    strcpy(parentDirPath, argv[3]);
    if (parentDirPath[0]!='/') {
        perror("Invalid parentDirPath");
        fprintf(stderr, "No such file or directory\n");
        return ENOENT;
    } else {
        getParentDirPath(parentDirPath);
    }
    parentInodeNum = getInodeFromPath(parentDirPath);
    if (parentInodeNum == 0) {
        perror("parentDirPath not exist");
        fprintf(stderr, "No such file or directory\n");
        return ENOENT;
    }
    parentInode = inodeTable[parentInodeNum-1];

    // check file exist
    getFileNameFromPath(fileName, argv[3]);
    childInodeNum = searchFileInDir(&parentInode, fileName);
    if (childInodeNum != 0) {
        fprintf(stderr, "File or directory already exist\n");
        return EEXIST;
    }

    // create file and cp
    childInodeNum = initInode(EXT2_S_IFREG)+1;
    childInode = inodeTable[childInodeNum-1];
    unsigned int *singleIndirect;
    int nextBlockNum, byteRead;
    int fileSize = 0;
    int i = 0;
    while (!feof(src_fd)) {
        nextBlockNum = allocateNewBlock();
printf("Block %d is allocated for cp, content is:\n", nextBlockNum);
        if (i<12) {
            childInode.i_block[i] = nextBlockNum;
        } else if (i==12) {
            childInode.i_block[i] = nextBlockNum;
            singleIndirect = initSingleIndirect(nextBlockNum);
            continue;
        } else {
            singleIndirect[i-13] = nextBlockNum;
        }

        byteRead = fread(getBlock(nextBlockNum), 1, 1024, src_fd);
printf("%s\n", getBlock(nextBlockNum));
printf("total %d bytes in this block\n", byteRead);
        fileSize += byteRead;
        i++;
    }
    fclose(src_fd);
    // uptate inode filed
printf("file size: %d\n", fileSize);
    childInode.i_size = fileSize;
    childInode.i_blocks = (fileSize+511)/512;
printInode(&childInode);
    // add dir_entry fot this file into parent dir
    allocateNewDirent(&parentInode, childInodeNum, 'f', fileName);
}