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
    unsigned char *singleIndirect;

    int fd = open("largefile.img", O_RDWR);
    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    inodeTable = getInodeTable();

    
}