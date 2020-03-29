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
    char path[EXT2_NAME_LEN];
    char pathCopy[EXT2_NAME_LEN];
    char fileName[EXT2_NAME_LEN];
    int flagged = FALSE;
    struct ext2_inode inode;
    struct ext2_dir_entry_2 *dir_entry = NULL;
    int total_rec_len;

    if(argc!=4) {
        fprintf(stderr, "Usage: ext2_cp <image file name> <native path> <absolute path on the disk>\n");
        exit(1);
    }
    int fd = open(argv[1], O_RDWR);

    // read disk and get inode table
    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    inodeTable = getInodeTable();

    // read other arguments
    if(argc == 3) {
        strcpy(path, argv[2]);
    } else {
        if (strcmp(argv[2], "-a")!=0) {
            fprintf(stderr, "Invalid Flag\n");
            exit(1);
        }
        flagged = TRUE;
        strcpy(path, argv[3]);
    }

    // get the inode from path
    strcpy(pathCopy, path);
    int inodeNum = getInodeFromPath(pathCopy);
    if (inodeNum == 0) {
        fprintf(stderr, "No such file or directory\n");
        return ENOENT;
    }
    inode = inodeTable[inodeNum-1];


    // create inode base on path

}