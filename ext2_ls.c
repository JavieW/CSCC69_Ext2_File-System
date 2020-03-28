#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include "ext2.h"
#include "utilities.h"

unsigned char *disk;

int main(int argc, char **argv) {
    char *path = NULL;
    int flagged = FALSE;
    struct ext2_inode *inode;

    if(argc!=3 || argc!=4) {
        fprintf(stderr, "Usage: ext2_ls <image file name> <optional flag -a> <absolute path>\n");
        exit(1);
    }
    int fd = open(argv[1], O_RDWR);

    // read disk
    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // read other arguments
    if(argc == 3) {
        path = argv[2];
    } else {
        if (strcmp(argv[2], "-a")!=0) {
            fprintf(stderr, "Invalid Flag\n");
            exit(1);
        }
        flagged = TRUE;
        path = argv[3];
    }

    // get the inode from
    inode = getInodeFromPath(path);

    return ENOENT;
    return 0;
}