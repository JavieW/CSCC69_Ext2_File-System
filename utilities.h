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

int getBit(char unsigned * bitmap, int index);

int getFirstEmptyBitIndex(char unsigned * bitmap, int length);

void changeBitmap(char unsigned *bitmap, int idx, char mode);

// inode
struct ext2_inode *getInodeTable(void);

int initInode(char mode);

void deleteInode(int index);

// block, dir_entry
char unsigned *getBlock(int blockNum);

int searchFileInDir(struct ext2_inode *inode, char *fileName);

int calculateActuralSize(struct ext2_dir_entry_2 *dirent);

struct ext2_dir_entry_2 *initDirent(struct ext2_inode *parent_inode, int size);

struct ext2_dir_entry_2 *initDirentDDB(int blockNum, int size);

// path handling
int getInodeFromPath(char *path);

void getFileNameFromPath(char *fileName, char *path);

void getParentDirPath(char *path);
