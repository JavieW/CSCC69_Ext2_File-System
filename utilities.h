#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include "ext2.h"

#define TRUE 1;
#define FALSE 0;

extern char unsigned *disk;

struct ext2_super_block *getSuperblock(void);

struct ext2_group_desc *getGroupDesc(void);

char unsigned *getBlockBitmap(void);

char unsigned *getInodeBitmap(void);

int getBit(char unsigned * , int);

struct ext2_inode *getInodeTable(void);

struct ext2_inode *getInode(int);

struct ext2_inode *getInodeFromPath(char *);

char unsigned *getBlock(int);


