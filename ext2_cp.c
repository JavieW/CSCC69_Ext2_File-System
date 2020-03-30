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
    /*
    int src_fd;
    char parentDirPath[EXT2_NAME_LEN];
    char fileName[EXT2_NAME_LEN];
    int fileSize;
    int parentInodeNum, childInodeNum;
    struct ext2_inode parentInode, childInode;
    struct ext2_dir_entry_2 *dir_entry = NULL;
    int total_rec_len;

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
		fprintf(stderr, "No such file or directory\n");
        return ENOENT;
	}

    // get the parent directory inode
    strcpy(parentDirPath, argv[3]);
    if (parentDirPath[0]!='/') {
        fprintf(stderr, "No such file or directory\n");
        return ENOENT;
    } else {
        getParentDirPath(parentDirPath);
    }
    parentInodeNum = getInodeFromPath(getParentDirPath);
    if (parentInodeNum == 0) {
        fprintf(stderr, "No such file or directory\n");
        return ENOENT;
    }
    parentInode = inodeTable[parentInodeNum-1];

    // check file exist
    getFileNameFromPath(fileName, argv[3]);
    childInodeNum = searchFileInDir(&parentInode, fileName);
    if (parentInodeNum != 0) {
        fprintf(stderr, "File or directory already exist\n");
        return EEXIST;
    }

    // create file and cp
    childInodeNum = initInode('f');
    unsigned int *singleIndirect;
    int nextBlockNum, byteRead;
    int i = 0;
    while (feof(src_fd)) {
        nextBlockNum = allocateBlock();
        if (i<12) {
            inodeTable[childInodeNum].i_block[i] = nextBlockNum;
        } else if (i==12) {
            inodeTable[childInodeNum].i_block[i] = nextBlockNum;
            singleIndirect = getBlock(nextBlockNum);
            nextBlockNum = allocateBlock();
        } else {
            singleIndirect[i-13] = nextBlockNum;
        }

        byteRead = fread(getBlock(nextBlockNum), 1024, 1, src_fd);
        fileSize += byteRead;
        i++;
    }
    // uptate inode filed
    inodeTable[childInodeNum].i_size = fileSize;
    inodeTable[childInodeNum].i_blocks = (fileSize+511)/512;

    // add dir_entry fot this file into parent dir
    allocateNewDirent(&parentInode, childInodeNum, 'f', fileName); */
}