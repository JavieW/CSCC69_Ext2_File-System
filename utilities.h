#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "ext2.h"

#define SUPER_BLOCK(disk) (struct ext2_super_block *)(disk + EXT2_BLOCK_SIZE);
#define GROUP_DESCRIPTOR(disk) (struct ext2_group_desc *)(disk + 2*EXT2_BLOCK_SIZE);
#define BLOCK(disk, i) (disk+i*EXT2_BLOCK_SIZE);

int getBit(unsigned char *, int, int, int);

int testprint();