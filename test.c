#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include "ext2.h"
#include "utilities.h"

unsigned char *disk;
struct ext2_inode *inodeTable;

int main(int argc, char **argv) {
    struct ext2_inode *inode =  NULL;

    int fd = open(argv[1], O_RDWR);
    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    inodeTable = getInodeTable();

    int  inodeNum = getInodeFromPath(argv[2]);
    inode = &inodeTable[inodeNum-1]; 
    unsigned int * single;
    // print file content for this inode
    if(inode->i_mode & EXT2_S_IFDIR) {
        int total_len = 0;
        struct ext2_dir_entry_2 *dir_entry = NULL;
        for (int i=0; i<12; i++) {
            if (inode->i_block[i] != 0) {
                total_len = 0;
                dir_entry = (struct ext2_dir_entry_2 *)getBlock(inode->i_block[i]);
                printf("inode->i_block[%d]: %d\n", i, inode->i_block[i]);
                while (total_len<EXT2_BLOCK_SIZE) {
                    printf("Inode: %d rec_len: %d name_len: %d type= %d name=%s\n", dir_entry->inode, dir_entry->rec_len, dir_entry->name_len, dir_entry->file_type, dir_entry->name);
                    total_len+=dir_entry->rec_len;
                    dir_entry = (void*)dir_entry + dir_entry->rec_len;
                }
            }
        }
        if (inode->i_block[12] != 0) {
            single = (unsigned int *)getBlock(inode->i_block[12]);
            for (int j=0; j<EXT2_BLOCK_SIZE/4;j++) {
                if (single[j] != 0) {
                    total_len = 0;
                    dir_entry = (struct ext2_dir_entry_2 *)getBlock(single[j]);
                    printf("single[%d]: %d\n", j, single[j]);
                    while (total_len<EXT2_BLOCK_SIZE) {
                        printf("Inode: %d rec_len: %d name_len: %d type= %d name=%s\n", dir_entry->inode, dir_entry->rec_len, dir_entry->name_len, dir_entry->file_type, dir_entry->name);
                        total_len+=dir_entry->rec_len;
                        dir_entry = (void*)dir_entry + dir_entry->rec_len;
                    }
                }   
            }
        }
    
    // print dirent for this inode
    } else {
        for (int i=0; i<12; i++) {
            if (inode->i_block[i] != 0) {
                printf("inode->i_block[%d]: %d\n", i, inode->i_block[i]);
                printf("%s\n", getBlock(inode->i_block[i]));
            }
        }
        if (inode->i_block[12] != 0) {
            single = (unsigned int *)getBlock(inode->i_block[12]);
            for (int j=0; j<EXT2_BLOCK_SIZE/4;j++) {
                if(single[j] != 0) {
                    printf("single[%d]: %d\n", j, single[j]);
                    printf("%s\n", getBlock(single[j]));
                }
            }
        }
    }


}