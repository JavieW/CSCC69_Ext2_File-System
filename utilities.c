#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "ext2.h"
#include "utilities.h"

// get superblock, group descriptor
struct ext2_super_block *getSuperblock() {
    return (struct ext2_super_block *)(disk+EXT2_BLOCK_SIZE);
}

struct ext2_group_desc *getGroupDesc() {
    return (struct ext2_group_desc *)(disk+2*EXT2_BLOCK_SIZE);
}

// get bitmaps, bit
char unsigned *getBlockBitmap() {
    struct ext2_group_desc *gd = getGroupDesc();
    return (char unsigned *)(disk+gd->bg_block_bitmap*EXT2_BLOCK_SIZE);
}

char unsigned *getInodeBitmap() {
    struct ext2_group_desc *gd = getGroupDesc();
    return (char unsigned *)(disk+gd->bg_inode_bitmap*EXT2_BLOCK_SIZE);
}

int getBit(char unsigned * bitmap, int index) {
    return (bitmap[index/8]>>index%8)&1;
}

// get inode, block
struct ext2_inode *getInodeTable() {
    struct ext2_group_desc *gd = getGroupDesc();
    return (struct ext2_inode *)(disk+gd->bg_inode_table*EXT2_BLOCK_SIZE);
}

struct ext2_inode *getInode(int index) {
    struct ext2_group_desc *gd = getGroupDesc();
    return (struct ext2_inode *)(disk+(gd->bg_inode_table+index)*EXT2_BLOCK_SIZE);
}

struct ext2_inode *getInodeFromPath(char * path) {
    return NULL;
}

char unsigned *getBlock(int blockNum) {
    return (char unsigned*)(disk+blockNum*EXT2_BLOCK_SIZE);
}

// path handling

