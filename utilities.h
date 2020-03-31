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

#define TRUE 1
#define FALSE 0

#define INODE_BITMAP 1
#define BLOCK_BITMAP 0

extern char unsigned *disk;

// sb, gp
struct ext2_super_block *getSuperblock(void);

struct ext2_group_desc *getGroupDesc(void);

// bitmap
char unsigned *getBlockBitmap(void);

char unsigned *getInodeBitmap(void);

int getBit(char unsigned * bitmap, int index);

int getFirstEmptyBitIndex(int bitmap);

void changeBitmap(char unsigned *bitmap, int idx, char mode);

// inode
struct ext2_inode *getInodeTable(void);

int initInode(unsigned short mode);

void deleteInode(int index);

void printInode(struct ext2_inode *inode);

// block
char unsigned *getBlock(int blockNum);

int allocateNewBlock(void);

// dir_entry
int searchFileInDir(struct ext2_inode *inode, char *fileName);

int calculateActuralSize(struct ext2_dir_entry_2 *dirent);

struct ext2_dir_entry_2 *allocateNewDirent(struct ext2_inode *parent_inode, int size);

struct ext2_dir_entry_2 *allocateDirentHelper(int blockNum, int size);

struct ext2_dir_entry_2 *initNewDirent(struct ext2_inode *parentInode, int childInodeNum, int type, char *fileName);

unsigned int *initSingleIndirect(int blockNum);

// path handling
int getInodeFromPath(char *path);

void getFileNameFromPath(char *fileName, char *path);

void getParentDirPath(char *path);
