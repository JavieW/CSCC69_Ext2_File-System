#include "utilities.h"

struct ext2_super_block *getSuperblock() {
    return (struct ext2_super_block *)SUPER_BLOCK;
}

struct ext2_group_desc *getGroupDesc() {
    return (struct ext2_group_desc *)GROUP_DESCRIPTOR;
}

char unsigned *getBlockBitmap() {
    struct ext2_group_desc *gd = getGroupDesc();
    return (char unsigned *)(disk+gd->bg_block_bitmap*EXT2_BLOCK_SIZE);
}

char unsigned *getInodeBitmap() {
    struct ext2_group_desc *gd = getGroupDesc();
    return (char unsigned *)(disk+gd->bg_inode_bitmap*EXT2_BLOCK_SIZE);
}

struct ext2_inode *getInodeTable() {
    struct ext2_group_desc *gd = getGroupDesc();
    return (struct ext2_inode *)(disk+gd->bg_inode_table*EXT2_BLOCK_SIZE);
}

struct ext2_dir_entry_2 *getDirEntries(int blockNum) {
    return (struct ext2_dir_entry_2 *)(disk+blockNum*EXT2_BLOCK_SIZE);
}

char unsigned *getBlock(blockNum) {
    return (char unsigned*)(disk+blockNum*EXT2_BLOCK_SIZE);
}

int getBit(char unsigned * bitmap, int index) {
    return (bitmap[index/8]>>index%8)&1;
}
