#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <assert.h>
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

// get inode
struct ext2_inode *getInodeTable() {
    struct ext2_group_desc *gd = getGroupDesc();
    return (struct ext2_inode *)(disk+gd->bg_inode_table*EXT2_BLOCK_SIZE);
}

// get block, dir_entry
char unsigned *getBlock(int blockNum) {
    return (char unsigned*)(disk+blockNum*EXT2_BLOCK_SIZE);
}

// path handling
/*
 * return inode number of file if the file is found, o/w return 0
 */
int searchFileInDir(struct ext2_inode *cur_inode, char *fileName) {
    struct ext2_dir_entry_2 * dir_entry;
    int total_rec_len;

    // first argument must be directory type
    assert(cur_inode->i_mode & EXT2_S_IFDIR);
    dir_entry = (struct ext2_dir_entry_2 *)getBlock(cur_inode->i_block[0]);

    total_rec_len = 0;
    while (total_rec_len < cur_inode->i_size) {
        if(strcmp(dir_entry->name, fileName)==0) {
            return dir_entry->inode;
        }
        total_rec_len = total_rec_len + dir_entry->rec_len;
        dir_entry = (void *) dir_entry + dir_entry->rec_len;
    }
    return 0;
}

/*
 * return: inodeNum of the path, NULL if path is not found or invalid
 */
int getInodeFromPath(char *path) {
    struct ext2_inode *inodeTable = getInodeTable();
    int inode_num = EXT2_ROOT_INO;
    struct ext2_inode cur_inode = inodeTable[inode_num-1];
    char *next_file;
    int endWithDir = path[strlen(path)-1] == '/'; // is path endwith '/' ?

    if (path[0] != '/') {
        // invalid path: absolute path startwith 
        return 0;
    }

    // travel through path
    next_file = strtok(path, "/");
    while(next_file != NULL) {
        // cannot have a non-directory type file in the middle of path
        if (!(cur_inode.i_mode & EXT2_S_IFDIR)) {
            // Invalid path; non-dir type file inside path
            return 0;
        }

        // get next inode from current directory
        inode_num = searchFileInDir(&cur_inode, next_file);
        // update inode to next file with next_file
        if (inode_num != 0) {
            cur_inode = inodeTable[inode_num-1];
        } else {
            // invalid path: file name not found
            return 0;
        }

        next_file = strtok(NULL, "/");
    }

    if (!(cur_inode.i_mode & EXT2_S_IFDIR) && endWithDir) {
        // invalid path: path endwith '/' but have a non-dir type file at the end
        return 0;
    }

    return inode_num;
}

/*
 * return the last section of path into fileName
 */
void getFileNameFromPath(char *fileName, char *path) {
    char *curr = strtok(path, "/");
    while (curr != NULL) {
        strcpy(fileName, curr);
        curr = strtok(NULL, "/");
    }
}
