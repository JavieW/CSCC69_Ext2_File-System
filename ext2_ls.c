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
    char path[EXT2_NAME_LEN];
    char pathCopy[EXT2_NAME_LEN];
    char fileName[EXT2_NAME_LEN];
    int flagged = 0;
    struct ext2_inode inode;

    if(argc!=3 && argc!=4) {
        fprintf(stderr, "Usage: ext2_ls <image file name> <optional flag -a> <absolute path>\n");
        exit(1);
    }
    int fd = open(argv[1], O_RDWR);

    // read disk and get inode table
    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    inodeTable = getInodeTable();

    // read other arguments
    if(argc == 3) {
        strcpy(path, argv[2]);
    } else {
        if (strcmp(argv[2], "-a")!=0) {
            fprintf(stderr, "Invalid Flag\n");
            exit(1);
        }
        flagged = 1;
        strcpy(path, argv[3]);
    }

    // get the inode from path
    strcpy(pathCopy, path);
    int inodeNum = getInodeFromPath(pathCopy);
    if (inodeNum == 0) {
        fprintf(stderr, "No such file or directory\n");
        return ENOENT;
    }
    inode = inodeTable[inodeNum-1];
    
    // print all file nemes in directory data block
    if (inode.i_mode & EXT2_S_IFDIR) {
        struct ext2_dir_entry_2 *dir_entry = NULL;
        unsigned int *singleIndirect = NULL;
        int total_rec_len = 0;

        // print file names in direct blocks
        for (int i=0; i<13+EXT2_BLOCK_SIZE/4; i++) {

            if (i<12) {
                if (inode.i_block[i] == 0) continue;
                dir_entry = (struct ext2_dir_entry_2 *)getBlock(inode.i_block[i]);
            } else if (i==12) {
                if (inode.i_block[i] == 0) break;
                singleIndirect = (unsigned int *)getBlock(inode.i_block[12]);
                continue;
            } else {
                if (singleIndirect[i-13] == 0) continue;
                dir_entry = (struct ext2_dir_entry_2 *)getBlock(singleIndirect[i-13]);
            }
                
            // for each dir entry in the block
            total_rec_len = 0;
            while (total_rec_len < EXT2_BLOCK_SIZE) {
                if ((dir_entry->name[0]!='.' || flagged) && (dir_entry->name_len!=0)) {
                    printf("%s\n", dir_entry->name);
                }
                total_rec_len = total_rec_len + dir_entry->rec_len;
                dir_entry = (void *) dir_entry + dir_entry->rec_len;
            }
        }

    // print file name
    } else if (inode.i_mode&EXT2_S_IFREG || inode.i_mode&EXT2_S_IFLNK) {
        getFileNameFromPath(fileName, path);
        printf("%s\n", fileName);
    }
    return 0;
}