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

    // for each block
    for (int i=0; i<15; i++) {
        if (cur_inode->i_block[i] == 0) {
            break;
        } else {
            dir_entry = (struct ext2_dir_entry_2 *)getBlock(cur_inode->i_block[i]);
        }

        // for each dir entry in the block
        total_rec_len = 0;
        while (total_rec_len < EXT2_BLOCK_SIZE) {
            if(strcmp(dir_entry->name, fileName)==0) {
                return dir_entry->inode;
            }
            total_rec_len = total_rec_len + dir_entry->rec_len;
            dir_entry = (void *) dir_entry + dir_entry->rec_len;
        }
    }
    return 0;
}

/*
 * return: inodeNum of the path, 0 if path is not found or invalid
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

/*
 * modify input path
 */
void getParentDirPath(char *path) {
    int len = strlen(path);
    if (path[len-1]=='/')
        path[len-1] = '\0';
    char *target = strrchr(path, '/');
    *(target+1) = '\0';
}


int getFirstEmptyBitIndex(char unsigned * bitmap, int length){
    int index = 0;
    while (index < length) {
        if (getBit(bitmap, index) == 0) {
            return index;
        }
        index++;
    }
    return -1;
}


void changeBitmap(char unsigned *bitmap, int idx, char mode) {

    int turn_on = 00000001;
    int turn_off = 00000001;

    turn_on = turn_on << idx%8;
    turn_off = ~(turn_off << idx%8);

    
    if (mode == 'a'){
        // turning on the bit
        bitmap[idx/8] = bitmap[idx/8] | turn_on;
    }else{
        // turning off the bit
        bitmap[idx/8] = bitmap[idx/8] & turn_off;
    }
    
    return ;
}




int initInode(char mode, int size){

    // find the first free inode
    int index = getFirstEmptyBitIndex(getInodeBitmap(), getSuperblock()->s_inodes_count);
    
    // change its bitmap
    struct ext2_group_desc *gd = getGroupDesc();
    char unsigned *bitmap = disk+gd->bg_inode_bitmap*EXT2_BLOCK_SIZE;
    changeBitmap(bitmap, index, 'a');

    // initialize inode attribute
    struct ext2_inode *inode_table = getInodeTable();
    inode_table[index].i_mode = mode;
    inode_table[index].i_size = size;
    inode_table[index].i_links_count = 1;
    inode_table[index].i_blocks = 0;
    return index;

}

void deleteInode(int index){
    
    char unsigned *inode_bitmap = getInodeBitmap();
    char unsigned *block_bitmap = getBlockBitmap();
    
    // change inode bitmap
    changeBitmap(inode_bitmap, index, 'd');
    
    struct ext2_inode *inode_table = getInodeTable();
    struct ext2_inode target = inode_table[index];
    
    // delete the block bitmap
    int i;
    int block_num;
    for(i = 0; i<12;i++) {
        block_num = target.i_block[i];
        changeBitmap(block_bitmap, block_num, 'd');
    }
    // delete single indirect
    int bp = target.i_block[12];
    if (bp != 0)
    {
        unsigned char *single = getBlock(bp);
        i=0;
        while(single[i] != 0) {
            changeBitmap(block_bitmap, single[i], 'd');
            i++;
        }
    }
}

int calculateActuralSize(struct ext2_dir_entry_2 *dirent){
    return sizeof(struct ext2_dir_entry_2) + ((dirent->name_len + 4)/4) * 4;
}

struct ext2_dir_entry_2 *initDirent(struct ext2_inode *parent_inode, int size){
    int total_rec_len;
    int residue_len, actural_len;
    struct ext2_dir_entry_2 *dir_entry;
    struct ext2_dir_entry_2 *new_dir_entry;
    unsigned char *singleIndirect;

    // search in direct block
    for(int i = 0; i<12;i++) {
        if (parent_inode->i_block[i] == 0)
            break;
        new_dir_entry = initDirentDDB(parent_inode->i_block[i], size);
        if (new_dir_entry!=NULL)
            return new_dir_entry;
    }
    // search in single indirect block
    if (parent_inode->i_block[12] != 0);
    {
        singleIndirect = getBlock(parent_inode->i_block[12]);
        for(int i = 0; i<EXT2_BLOCK_SIZE/4;i++) {
            if (parent_inode->i_block[i] == 0)
                break;
            new_dir_entry = initDirentDDB(parent_inode->i_block[i], size);
            if (new_dir_entry!=NULL)
                return new_dir_entry;
        }
    }
    return NULL;
}
/*
* Helper function for initDirent
*/
struct ext2_dir_entry_2 *initDirentDDB(int blockNum, int size) {
    struct ext2_dir_entry_2 *dir_entry, *new_dir_entry;
    int total_rec_len, actural_len, residue_len;

    dir_entry = (struct ext2_dir_entry_2 *)getBlock(blockNum);
    total_rec_len = 0;
    while (total_rec_len < EXT2_BLOCK_SIZE) {
        // calculate residue
        actural_len = calculateActuralSize(dir_entry);
        residue_len = dir_entry->rec_len - actural_len;
        if (residue_len >= size) {
            new_dir_entry = (void *)dir_entry+actural_len;
            dir_entry->rec_len=actural_len;
            new_dir_entry->rec_len=residue_len;
            return new_dir_entry;
        }
        total_rec_len = total_rec_len + dir_entry->rec_len;
        dir_entry = (void *) dir_entry + dir_entry->rec_len;
    }
    return NULL;
}
