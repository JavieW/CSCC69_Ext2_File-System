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

char unsigned *getBlock(int);

// path handling
int getInodeFromPath(char *);

int searchFileInDir(struct ext2_inode *, char *);

void getFileNameFromPath(char *, char *);

void getParentDirPath(char *path);

void changeBitmap(char unsigned *, int, char);

int initInode(char, int);

int getFirstEmptyBitIndex(char unsigned *, int);

void deleteInode(int);

struct ext2_dir_entry_2 *initDirentDDB(int, int);