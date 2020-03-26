#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "ext2.h"
#include "utilities.h"


int getBit(unsigned char *disk, int block, int byte, int bit) {
    if ((*(disk+block*EXT2_BLOCK_SIZE+byte)>>bit)&1)
        return 1;
    else
        return 0;
}

int testprint() {
    printf("utilities complied!\n");
}