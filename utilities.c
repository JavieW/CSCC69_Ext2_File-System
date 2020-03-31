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

// bitmaps, bit
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

int getFirstEmptyBitIndex(int bitmapNum) {
    int index, maxLength;
    unsigned char *bitmap;
    if (bitmapNum == INODE_BITMAP) {
        index = EXT2_GOOD_OLD_FIRST_INO;
        maxLength = getSuperblock()->s_inodes_count;
        bitmap = getInodeBitmap();
    } else {
        index = 0;
        maxLength = getSuperblock()->s_blocks_count;
        bitmap = getBlockBitmap();
    }
    while (index < maxLength) {
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
}

// inode
struct ext2_inode *getInodeTable() {
    struct ext2_group_desc *gd = getGroupDesc();
    return (struct ext2_inode *)(disk+gd->bg_inode_table*EXT2_BLOCK_SIZE);
}


int initInode(unsigned short mode) {

    // find the first free inode
    int index = getFirstEmptyBitIndex(INODE_BITMAP);
    
    // change its bitmap and update field in gd
    char unsigned *bitmap = getInodeBitmap();
    changeBitmap(bitmap, index, 'a');
    getGroupDesc()->bg_free_inodes_count--;

    // initialize inode attribute
    struct ext2_inode *inode_table = getInodeTable();
    inode_table[index].i_mode = mode;
    inode_table[index].i_size = 1024;
    inode_table[index].i_links_count = 1;
    inode_table[index].i_blocks = 2;
    for(int i=0; i<15; i++) {
        inode_table[index].i_block[i] = 0;
    }
    return index;
}

void deleteInode(int index) {
    
    char unsigned *inode_bitmap = getInodeBitmap();
    char unsigned *block_bitmap = getBlockBitmap();
    
    // change inode bitmap
    changeBitmap(inode_bitmap, index, 'd');
    getGroupDesc()->bg_free_inodes_count++;
    
    struct ext2_inode *inode_table = getInodeTable();
    struct ext2_inode target = inode_table[index];
    
    // delete the block bitmap
    int i;
    for(i = 0; i<12;i++) {
        if (target.i_block[i] != 0) {
            changeBitmap(block_bitmap, target.i_block[i], 'd');
            getGroupDesc()->bg_free_blocks_count++;
        }
    }
    // delete single indirect
    int bp = target.i_block[12];
    if (bp != 0)
    {
        unsigned int *single = (unsigned int*)getBlock(bp);
        for (int i=0; i<EXT2_BLOCK_SIZE/4; i++) {
            if (single[i] != 0) {
                changeBitmap(block_bitmap, single[i], 'd');
                getGroupDesc()->bg_free_blocks_count++;
            }
        }
    }
}

void printInode(struct ext2_inode *inode)
{
    printf("i_mode: %d\n", inode->i_mode);
    printf("i_size: %d\n", inode->i_size);
    printf("i_links_count: %d\n", inode->i_links_count);
    printf("i_blocks: %d\n", inode->i_blocks);
    printf("i_block:\n\t");
    for(int i=0; i<15; i++) {
        printf("[%d]: %d ", i, inode->i_block[i]);
    }
    if(inode->i_block[12] != 0) {
        printf("\nfirst 15 single indirect:\n\t");
        unsigned int *singleIndirect = (unsigned int *)getBlock(inode->i_block[12]);
        for(int i=0; i<15; i++) {
            printf("[%d]: %d ", i, singleIndirect[i]);
        }
    }
    printf("\n\n");
}


// block
char unsigned *getBlock(int blockNum) {
    return (char unsigned*)(disk+blockNum*EXT2_BLOCK_SIZE);
}

int allocateNewBlock() {
    int index = getFirstEmptyBitIndex(BLOCK_BITMAP);
    changeBitmap(getBlockBitmap(), index, 'a');
    getGroupDesc()->bg_free_blocks_count--;
    return index+1;
}

// dir_entry
int searchFileInDir(struct ext2_inode *inode, char *fileName) {
    /*
    * return inode number of file if the file is found, o/w return 0
    */
    struct ext2_dir_entry_2 * dir_entry;
    int total_rec_len;
    unsigned int *singleIndirect;

    // first argument must be directory type
    assert(inode->i_mode & EXT2_S_IFDIR);

    // search in direct block
    for (int i=0; i<12; i++) {
        if (inode->i_block[i] == 0) {
            continue;
        } else {
            dir_entry = (struct ext2_dir_entry_2 *)getBlock(inode->i_block[i]);
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
    // search in single indirect block
    if (inode->i_block[12] != 0) {
        // for each block number in single indirect block
        singleIndirect = (unsigned int *)getBlock(inode->i_block[12]);
        for(int i = 0; i<EXT2_BLOCK_SIZE/4;i++) {
            if (singleIndirect[i] == 0) {
                continue;
            } else { 
                dir_entry = (struct ext2_dir_entry_2 *)getBlock(singleIndirect[i]);
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
    }
    return 0;
}

int calculateActuralSize(struct ext2_dir_entry_2 *dirent) {
    return sizeof(struct ext2_dir_entry_2) + ((dirent->name_len+4)/4)*4;
}

struct ext2_dir_entry_2 *initDirent(struct ext2_inode *parent_inode, int size) {
    unsigned int *singleIndirect;
    struct ext2_dir_entry_2 *new_dir_entry = NULL;
    // search in all used direct block
    for(int i = 0; i<12;i++) {
        if (parent_inode->i_block[i] != 0) {
            new_dir_entry = initDirentDDB(parent_inode->i_block[i], size);
            if (new_dir_entry!=NULL)
                return new_dir_entry;
        }
    }

    // search in single indirect block
    if (parent_inode->i_block[12] != 0)
    {
        // for each block number in single indirect block
        singleIndirect = (unsigned int *)getBlock(parent_inode->i_block[12]);
        for(int i = 0; i<EXT2_BLOCK_SIZE/4;i++) {
            if (singleIndirect[i] != 0) {
                new_dir_entry = initDirentDDB(singleIndirect[i], size);
                if (new_dir_entry!=NULL)
                    return new_dir_entry;
            }
        }
    }

    // if we cannot find a space, try to allocate a new direct block
    for(int i = 0; i<13+EXT2_BLOCK_SIZE/4;i++) {
        if (parent_inode->i_block[i] != 0) {
            continue;
        }
        
        int newBlockNum = allocateNewBlock();
        if (i<12) {
            parent_inode->i_block[i] = newBlockNum;
        } else if (i==12) {
            parent_inode->i_block[i] = newBlockNum;
            singleIndirect = initSingleIndirect(parent_inode->i_block[i]);
            continue;
        } else {
            singleIndirect[i-13] = newBlockNum;
        }

        new_dir_entry = (struct ext2_dir_entry_2 *)getBlock(newBlockNum);
        parent_inode->i_blocks+=EXT2_BLOCK_SIZE/512;
        parent_inode->i_size+=EXT2_BLOCK_SIZE;
        new_dir_entry->rec_len=EXT2_BLOCK_SIZE;
        return new_dir_entry;
    }
    return NULL;
}

struct ext2_dir_entry_2 *initDirentDDB(int blockNum, int size) {
    /*
    * Helper function for initDirent
    */
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

struct ext2_dir_entry_2 *allocateNewDirent(struct ext2_inode *parentInode, int childInodeNum, char type, char *fileName) {
    int name_len, size;
    struct ext2_dir_entry_2 *newDirent; 

    // calculate actual size required for new dir_entry
    name_len = strlen(fileName);
    size = sizeof(struct ext2_dir_entry_2) + ((name_len+4)/4)*4;

    // allocate new dir_entry in parent directory
    newDirent = initDirent(parentInode, size);
    
    // initialize new dir_entry
    newDirent->inode = childInodeNum;
    newDirent->file_type = type;
    newDirent->name_len = (unsigned char) name_len;
    strcpy(newDirent->name, fileName);
    return newDirent;
}

unsigned int *initSingleIndirect(int blockNum) {
    unsigned int *singleIndirect = (unsigned int *)getBlock(blockNum);
    for (int i=0; i<(EXT2_BLOCK_SIZE/4); i++) {
        singleIndirect[i] = 0;
    }
    return singleIndirect;
}

// path handling
int getInodeFromPath(char *path) {
    /*
    * return: inodeNum of the path, 0 if path is not found or invalid
    */
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

void getFileNameFromPath(char *fileName, char *path) {
    /*
    * return the last section of path into fileName
    */
    char *curr = strtok(path, "/");
    while (curr != NULL) {
        strcpy(fileName, curr);
        curr = strtok(NULL, "/");
    }
}

void getParentDirPath(char *path) {
    /*
    * modify input path
    */
    int len = strlen(path);
    if (path[len-1]=='/')
        path[len-1] = '\0';
    char *target = strrchr(path, '/');
    *(target+1) = '\0';
}
