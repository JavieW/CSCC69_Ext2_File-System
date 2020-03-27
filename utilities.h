#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "ext2.h"

#define SUPER_BLOCK (disk + EXT2_BLOCK_SIZE);
#define GROUP_DESCRIPTOR (disk + 2*EXT2_BLOCK_SIZE);

extern char unsigned *disk;

struct ext2_super_block *getSuperblock(void);

struct ext2_group_desc *getGroupDesc(void);

char unsigned *getBlockBitmap(void);

char unsigned *getInodeBitmap(void);

struct ext2_inode *getInodeTable(void);

struct ext2_dir_entry_2 *getDirEntries(int);

char unsigned *getBlock(int);

int getBit(char unsigned * , int);