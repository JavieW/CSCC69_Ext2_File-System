#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "ext2.h"
#include <errno.h>
#include <string.h>
#include "utilities.h"

unsigned char *disk;

int main(int argc, char **argv) {
    char path[EXT2_NAME_LEN];
    char pathCopy[EXT2_NAME_LEN];
    char dirName[EXT2_NAME_LEN];
    struct ext2_inode *parent_inode, *inode_table, *target_inode;

    if(argc!=3) {
        fprintf(stderr, "Usage: ./ext2_mkdir <image file name> <absolute path>\n");
        exit(1);
    }
    int fd = open(argv[1], O_RDWR);

    // read disk and get inode table
    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    strcpy(path, argv[2]);
    strcpy(pathCopy, argv[2]);
    // if input is root, return EEXIST
    if (strcmp(pathCopy, "/") == 0) {
        fprintf(stderr, "Specified directory already exists\n");
        return EEXIST;
    }
    // get the parent directory of the specified directory
    getParentDirPath(pathCopy);
    int parent_inode_num = getInodeFromPath(pathCopy);
    // if any component on the path of parent directory does not exist,
    // return ENOENT
    if (parent_inode_num == 0) {
        fprintf(stderr, "One of the components on the path does not exist\n");
        return ENOENT;
    }

    // get the parent directory inode
    inode_table = getInodeTable();
    parent_inode = &inode_table[parent_inode_num-1];
    // get the directory name from input
    strcpy(pathCopy, path);
    getFileNameFromPath(dirName, pathCopy);
    int target_inode_num = searchFileInDir(parent_inode, dirName);
    // if specified dir already exists, return EEXIST
    if (target_inode_num != 0) {
        fprintf(stderr, "Specified directory already exists\n");
        return EEXIST;
    }

    // initialize an inode for the specified directory
    target_inode_num = initInode(EXT2_S_IFDIR);
    target_inode = &inode_table[target_inode_num-1];
    //create an directory entry for the specified directory
    initNewDirent(parent_inode, target_inode_num, EXT2_FT_DIR, dirName);

    // allocate a new block for the specified directory
    int newBlockNum = allocateNewBlock();
    target_inode->i_block[0] = newBlockNum;
    struct ext2_dir_entry_2 *firstDirent = (struct ext2_dir_entry_2 *)getBlock(newBlockNum);
    // initialize the data block information for the target directory
    firstDirent->file_type = EXT2_FT_DIR;
    firstDirent->inode = target_inode_num;
    // create an entry for .
    strcpy(firstDirent->name, ".");
    firstDirent->name_len = 1;
    firstDirent->rec_len = EXT2_BLOCK_SIZE;
    // create an entry for ..
    initNewDirent(target_inode, parent_inode_num, EXT2_FT_DIR, "..");

    // update the revelent information
    getGroupDesc()->bg_used_dirs_count++;
    parent_inode->i_links_count++;
    target_inode->i_links_count = 2;
    return 0;
}
