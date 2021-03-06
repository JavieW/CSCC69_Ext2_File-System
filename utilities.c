#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
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
char unsigned *getBitmap(int bitmapNum) {
    struct ext2_group_desc *gd = getGroupDesc();
    if (bitmapNum == INODE_BITMAP)
        return (char unsigned *)(disk+gd->bg_inode_bitmap*EXT2_BLOCK_SIZE);
    else if (bitmapNum == BLOCK_BITMAP)
        return (char unsigned *)(disk+gd->bg_block_bitmap*EXT2_BLOCK_SIZE);
    return NULL;
}

int getBit(char unsigned *bitmap, int index) {
    return (bitmap[index/8]>>index%8)&1;
}

int getFirstEmptyBitIndex(int bitmapNum) {
    int index, maxLength;
    unsigned char *bitmap;
    if (bitmapNum == INODE_BITMAP) {
        index = EXT2_GOOD_OLD_FIRST_INO;
        maxLength = getSuperblock()->s_inodes_count;
        bitmap = getBitmap(INODE_BITMAP);
    } else {
        index = 0;
        maxLength = getSuperblock()->s_blocks_count;
        bitmap = getBitmap(BLOCK_BITMAP);
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

struct ext2_inode *getInode(int inodeNum) {
    struct ext2_inode *inodeTable = getInodeTable();
    return &inodeTable[inodeNum-1];
}

/**
 * return new intialized inode number
 */
int initInode(unsigned short mode) {

    // find the first free inode
    int index = getFirstEmptyBitIndex(INODE_BITMAP);
    
    // change its bitmap and update field in gd
    char unsigned *bitmap = getBitmap(INODE_BITMAP);
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

    // set creation time for this inode
    inode_table[index].i_ctime = time(NULL);
    return index+1;
}

void deleteInode(int inodeNum) {
    
    char unsigned *inode_bitmap = getBitmap(INODE_BITMAP);
    char unsigned *block_bitmap = getBitmap(BLOCK_BITMAP);
    
    // delete inode
    changeBitmap(inode_bitmap, inodeNum-1, 'd');
    getGroupDesc()->bg_free_inodes_count++;
    struct ext2_inode *inode_table = getInodeTable();
    struct ext2_inode *target = &inode_table[inodeNum-1];
    target->i_dtime = time(NULL);

    // delete block
    int i;
    for(i = 0; i<12;i++) {
        if (target->i_block[i] != 0) {
            changeBitmap(block_bitmap, target->i_block[i]-1, 'd');
            getGroupDesc()->bg_free_blocks_count++;
        }
    }

    // delete single indirect
    int bp = target->i_block[12];
    if (bp != 0)
    {
        // delete blocks in single
        unsigned int *single = (unsigned int*)getBlock(bp);
        for (int i=0; i<EXT2_BLOCK_SIZE/4; i++) {
            if (single[i] != 0) {
                changeBitmap(block_bitmap, single[i]-1, 'd');
                getGroupDesc()->bg_free_blocks_count++;
            }
        }   
        // delte single itself
        changeBitmap(block_bitmap, target->i_block[12]-1, 'd');
        getGroupDesc()->bg_free_blocks_count++;
    }
}

// block
char unsigned *getBlock(int blockNum) {
    // block index start at 1, so block Number == block Index
    // since "block[0]" is allocated for superblock
    return (char unsigned*)(disk+blockNum*EXT2_BLOCK_SIZE);
}

/**
 * return new allocated block number
 */
int allocateNewBlock() {
    int index = getFirstEmptyBitIndex(BLOCK_BITMAP);
    changeBitmap(getBitmap(BLOCK_BITMAP), index, 'a');
    getGroupDesc()->bg_free_blocks_count--;
    return index+1;
}

// dir_entry
int searchFileInDir(struct ext2_inode *parentInode, char *childFileName) {
    /**
     * return inode num if childFile is found, o/w 0
     */
    struct ext2_dir_entry_2 *cur_dir_entry = NULL;
    struct ext2_dir_entry_2 *pre_dir_entry = getPreDirent(parentInode, childFileName);
    if (strcmp(childFileName, ".") == 0) {
        return ((struct ext2_dir_entry_2 *)getBlock(parentInode->i_block[0]))->inode;
    }
    else if (pre_dir_entry != NULL) {
        cur_dir_entry = (void *)pre_dir_entry + pre_dir_entry->rec_len;
        return cur_dir_entry->inode;
    } 
    return 0;
}

int calculateActuralSize(struct ext2_dir_entry_2 *dirent) {
    return ((sizeof(struct ext2_dir_entry_2)+(dirent->name_len+4))/4)*4;
}

struct ext2_dir_entry_2 *allocateNewDirent(struct ext2_inode *parent_inode, int size) {
    struct ext2_dir_entry_2 *new_dir_entry = NULL;
    // search in all used direct block
    for(int i = 0; i<12;i++) {
        if (parent_inode->i_block[i] != 0) {
            new_dir_entry = allocateDirentHelper(parent_inode->i_block[i], size);
            if (new_dir_entry!=NULL)
                return new_dir_entry;
        }
    }

    // if we cannot find a space, try to allocate a new block
    int newBlockNum = 0;
    for(int i = 0; i<12;i++) {
        if (parent_inode->i_block[i] != 0) continue;
        newBlockNum = allocateNewBlock();
        parent_inode->i_block[i] = newBlockNum;

        // increse parentdir size
        parent_inode->i_blocks+=(EXT2_BLOCK_SIZE+511)/512;
        parent_inode->i_size+=EXT2_BLOCK_SIZE;

        // insert dummy head
        new_dir_entry = (struct ext2_dir_entry_2 *)getBlock(newBlockNum);
        new_dir_entry->file_type = EXT2_FT_UNKNOWN;
        new_dir_entry->inode = 0;
        new_dir_entry->name_len = 0;
        new_dir_entry->rec_len = ((sizeof(struct ext2_dir_entry_2)+3)/4)*4;

        // return new setted dir_entry
        new_dir_entry = (void *)new_dir_entry + new_dir_entry->rec_len;
        new_dir_entry->rec_len=EXT2_BLOCK_SIZE-((sizeof(struct ext2_dir_entry_2)+3)/4)*4;
        return new_dir_entry;
    }
    return NULL;
}

struct ext2_dir_entry_2 *allocateDirentHelper(int blockNum, int size) {
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

struct ext2_dir_entry_2 *initNewDirent(struct ext2_inode *parentInode, int childInodeNum, int type, char *fileName) {
    int name_len, size;
    struct ext2_dir_entry_2 *newDirent; 

    // calculate actual size required for new dir_entry
    name_len = strlen(fileName);
    size = sizeof(struct ext2_dir_entry_2) + ((name_len+4)/4)*4;

    // allocate new dir_entry in parent directory
    newDirent = allocateNewDirent(parentInode, size);
    
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
    struct ext2_inode *cur_inode = &inodeTable[inode_num-1];
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
        if (!(cur_inode->i_mode & EXT2_S_IFDIR)) {
            // Invalid path; non-dir type file inside path
            return 0;
        }

        // get next inode from current directory
        inode_num = searchFileInDir(cur_inode, next_file);
        // update inode to next file with next_file
        if (inode_num != 0) {
            cur_inode = &inodeTable[inode_num-1];
        } else {
            // invalid path: file name not found
            return 0;
        }

        next_file = strtok(NULL, "/");
    }

    if (!(cur_inode->i_mode & EXT2_S_IFDIR) && endWithDir) {
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
    assert (strcmp(path, "/")!=0);

    int len = strlen(path);
    if (path[len-1]=='/')
        path[len-1] = '\0';
    char *target = strrchr(path, '/');
    *(target+1) = '\0';
}

struct ext2_dir_entry_2 *getPreDirent(struct ext2_inode *parentInode, char *childFileName) {
    struct ext2_dir_entry_2 * pre_dir_entry;
    struct ext2_dir_entry_2 * cur_dir_entry;
    int total_rec_len;

    // search in direct block
    for (int i=0; i<12; i++) {
        if (parentInode->i_block[i] == 0) {
            continue;
        } else {
            pre_dir_entry = (struct ext2_dir_entry_2 *)getBlock(parentInode->i_block[i]);
            total_rec_len = pre_dir_entry->rec_len;
            cur_dir_entry = (void *) pre_dir_entry + pre_dir_entry->rec_len;
        }

        // for each dir entry in the block
        while (total_rec_len < EXT2_BLOCK_SIZE) {
            if(strcmp(cur_dir_entry->name, childFileName)==0) {
                return pre_dir_entry;
            }
            total_rec_len = total_rec_len + cur_dir_entry->rec_len;
            pre_dir_entry = cur_dir_entry;
            cur_dir_entry = (void *) cur_dir_entry + cur_dir_entry->rec_len;
        }
    }
    return NULL;
}

void rm(struct ext2_inode *parentInode, char *childFileName) {
    struct ext2_dir_entry_2 *pre_dir_entry = NULL;
    struct ext2_dir_entry_2 *cur_dir_entry = NULL;
    struct ext2_dir_entry_2 *child_dir_entry = NULL;
    struct ext2_inode *childInode = NULL;
    int total_rec_len;
    
    // delete childFile from parentDir and get childInode
    pre_dir_entry = getPreDirent(parentInode, childFileName);
    cur_dir_entry = (void *)pre_dir_entry + pre_dir_entry->rec_len;
    childInode = getInode(cur_dir_entry->inode);

    // base case1, if childInode is Symbolic link
    if (cur_dir_entry->file_type == EXT2_FT_SYMLINK)
    {
        if (childInode->i_size > 60) {
            changeBitmap(getBitmap(BLOCK_BITMAP), childInode->i_block[0]-1, 'd');
            getGroupDesc()->bg_free_blocks_count++;
        }
        changeBitmap(getBitmap(INODE_BITMAP), cur_dir_entry->inode-1, 'd');
        getGroupDesc()->bg_free_inodes_count++;
    }
    // base case2, if childInode is a file
    else if (cur_dir_entry->file_type == EXT2_FT_REG_FILE)
    {
        // if link count == 0 remove inode
        if (childInode->i_links_count == 1) {
            deleteInode(cur_dir_entry->inode);
        } else {
            childInode->i_links_count--;
        }
    }
    // recursive case, if childInode is a dir
    else if (cur_dir_entry->file_type == EXT2_FT_DIR)
    {
        // reduce link count for self and parent (. and ..)
        childInode->i_links_count--;
        parentInode->i_links_count--;

        // for each file name (other than . and ..) in child dir, call recursion
        for (int i=0; i<12; i++) {
            if (childInode->i_block[i] == 0)
                continue;
            child_dir_entry = (struct ext2_dir_entry_2 *)getBlock(childInode->i_block[i]);
            total_rec_len = 0;
            // for each dir entry in the block
            while (total_rec_len < EXT2_BLOCK_SIZE) {
                if (child_dir_entry->name_len != 0 &&
                    strcmp(child_dir_entry->name, ".")!=0 &&
                    strcmp(child_dir_entry->name, "..")!=0) {
                    rm(childInode, child_dir_entry->name);
                }
                total_rec_len = total_rec_len + child_dir_entry->rec_len;
                child_dir_entry = (void *) child_dir_entry + child_dir_entry->rec_len;
            }
        }
        
        // if child link count == 0 remove inode
        if (childInode->i_links_count == 1) {
            deleteInode(cur_dir_entry->inode);
            getGroupDesc()->bg_used_dirs_count--;
        }
    }
    pre_dir_entry->rec_len += cur_dir_entry->rec_len;
}