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

// sb, gp
struct ext2_super_block *getSuperblock(void);

struct ext2_group_desc *getGroupDesc(void);

// bitmap
char unsigned *getBlockBitmap(void);

char unsigned *getInodeBitmap(void);

int getBit(char unsigned * , int);

int getFirstEmptyBitIndex(char unsigned *, int);

void changeBitmap(char unsigned *, int, char);

// inode
struct ext2_inode *getInodeTable(void);

int initInode(char, int);

void deleteInode(int);

// block, dir_entry
char unsigned *getBlock(int);

int searchFileInDir(struct ext2_inode *, char *);

int calculateActuralSize(struct ext2_dir_entry_2 *);

struct ext2_dir_entry_2 *initDirent(struct ext2_inode *, int);

struct ext2_dir_entry_2 *initDirentDDB(int, int);

// path handling
int getInodeFromPath(char *);

void getFileNameFromPath(char *, char *);

void getParentDirPath(char *path);
