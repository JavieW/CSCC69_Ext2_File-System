#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "ext2.h"

unsigned char *disk;

int main(int argc, char **argv) {
    char path[EXT2_NAME_LEN];
    char pathCopy[EXT2_NAME_LEN];
    char dirName[EXT2_NAME_LEN];
    int flagged = FALSE;
    struct ext2_inode *parent_inode;
    struct ext2_dir_entry_2 *dir_entry = NULL;
    int total_rec_len;

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
    if (strlen(pathCopy) == 1) {
        fprintf(stderr, "Specified directory already exists\n");
        return EEXIST;
    }
    // get the parent directory of the specified directory
    getParentDirPath(pathCopy);
    int parent_inode_num = getInodeFromPath(pathCopy);
    // if any component on the path of parent directory does not exist,
    // return ENOENT
    if (parent_inode_num == 0) {
        fprintf(stderr, "One of the compoenets on the path does not exist\n");
        return ENOENT;
    }

    // get the parent directory inode
    inodeTable = getInodeTable();
    parent_inode = &inodeTable[parent_inode_num-1];
    // get the directory name from input
    strcpy(pathCopy, path);
    getFileNameFromPath(dirName, pathCopy);
    int target_inode = searchFileInDir(parent_inode, dirName);
    // if specified dir already exists, return EEXIST
    if (target_inode != 0) {
        fprintf(stderr, "Specified directory already exists\n");
        return EEXIST;
    }
    //create an directory entry for the specified directory
    createDirEntry(parent_inode, dirName, EXT2_FT_DIR);
    /*
    printf("%d\n", (int) ((struct ext2_dir_entry_2 *)getBlock(inodeTable[10].i_block[0])));    
    printf("%d\n", (int) (((struct ext2_dir_entry_2 *)getBlock(inodeTable[10].i_block[0]))->name));
    printf("%c\n", *(((struct ext2_dir_entry_2 *)getBlock(inodeTable[10].i_block[0]))->name));
    printf("%c\n", *(((char *)getBlock(inodeTable[10].i_block[0]))+ 8)); */
    
    return 0;
}
