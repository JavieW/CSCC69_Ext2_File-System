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

    if(argc != 2) {
        fprintf(stderr, "Usage: readimg <image file name>\n");
        exit(1);
    }
    int fd = open(argv[1], O_RDWR);

    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
	perror("mmap");
	exit(1);
    }

    struct ext2_super_block *sb = (struct ext2_super_block *)(disk + EXT2_BLOCK_SIZE);
    printf("Inodes: %d\n", sb->s_inodes_count);
    printf("Blocks: %d\n", sb->s_blocks_count);

    struct ext2_group_desc *gd = (struct ext2_group_desc *)(disk+2*EXT2_BLOCK_SIZE);
    printf("Block group:\n");
    printf("\tblock bitmap: %d\n", gd->bg_block_bitmap);
    printf("\tinode bitmap: %d\n", gd->bg_inode_bitmap);
    printf("\tinode table: %d\n", gd->bg_inode_table);
    printf("\tfree blocks: %d\n", gd->bg_free_blocks_count);
    printf("\tfree inodes: %d\n", gd->bg_free_inodes_count);
    printf("\tused_dirs: %d\n", gd->bg_used_dirs_count);

    // print bitmap
    int i,j;
    printf("Block bitmap:");
    for (i=0; i<sb->s_blocks_count/8; i++) {
        printf(" ");
        for (j=0; j<8; j++) {
            printf("%d", ((*(disk+gd->bg_block_bitmap*EXT2_BLOCK_SIZE+i)>>j)&1));
        }
    }
    printf("\nInode bitmap:");
    for (i=0; i<sb->s_inodes_count/8; i++) {
        printf(" ");
        for (j=0; j<8; j++) {
            printf("%d", ((*(disk+gd->bg_inode_bitmap*EXT2_BLOCK_SIZE+i)>>j)&1));
        }
    }
    printf("\n\n");

    // print inode
    struct ext2_inode *inodes = (struct ext2_inode *)(disk+gd->bg_inode_table*EXT2_BLOCK_SIZE);
    //struct ext2_inode inode;
    char mode;
    printf("Inodes:\n");
    for(i = 0; i < sb->s_inodes_count; i++) {
        if (((*(disk+gd->bg_inode_bitmap*EXT2_BLOCK_SIZE+i/8)>>i%8)&1 && i>=EXT2_GOOD_OLD_FIRST_INO) || (i==EXT2_ROOT_INO-1)) {
            if (inodes[i].i_mode == EXT2_S_IFDIR)
                mode = 'd';
            else if (inodes[i].i_mode == EXT2_S_IFREG)
                mode = 'f';
            else if (inodes[i].i_mode == EXT2_S_IFLNK)
                continue;
            printf("[%d] type: %c size: %d links: %d blocks: %d\n", i+1, mode, inodes[i].i_size, inodes[i].i_links_count, inodes[i].i_blocks);
            printf("Blocks: %d\n", inodes[i].i_block[0]);
        }
    }
    printf("\n");


    return 0;
}
