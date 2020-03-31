#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "ext2.h"
#include "utilities.h"

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

    struct ext2_super_block *sb = getSuperblock();
    printf("Inodes: %d\n", sb->s_inodes_count);
    printf("Blocks: %d\n", sb->s_blocks_count);

    struct ext2_group_desc *gd = getGroupDesc();
    printf("Block group:\n");
    printf("\tblock bitmap: %d\n", gd->bg_block_bitmap);
    printf("\tinode bitmap: %d\n", gd->bg_inode_bitmap);
    printf("\tinode table: %d\n", gd->bg_inode_table);
    printf("\tfree blocks: %d\n", gd->bg_free_blocks_count);
    printf("\tfree inodes: %d\n", gd->bg_free_inodes_count);
    printf("\tused_dirs: %d\n", gd->bg_used_dirs_count);

    // task1, print bitmap
    int i,j;
    char unsigned *block_bitmap = getBlockBitmap();
    char unsigned *inode_bitmap = getInodeBitmap();
    printf("Block bitmap:");
    for (i=0; i<sb->s_blocks_count/8; i++) {
        printf(" ");
        for (j=0; j<8; j++) {
            printf("%d", ((block_bitmap[i]>>j)&1));
        }
    }
    printf("\nInode bitmap:");
    for (i=0; i<sb->s_inodes_count/8; i++) {
        printf(" ");
        for (j=0; j<8; j++) {
            printf("%d", ((inode_bitmap[i]>>j)&1));
        }
    }
    printf("\n\n");

    // task2, print inode
    struct ext2_inode *inodeTable = getInodeTable();
    char mode='\0';
    printf("Inodes:\n");
    for(i = 0; i < sb->s_inodes_count; i++) 
    {
        // print root inode and used unreserved inodes
        if ((getBit(inode_bitmap, i)==1 && i>=EXT2_GOOD_OLD_FIRST_INO) || (i==EXT2_ROOT_INO-1))
        {
            // skip the inodes that don't reserve blocks
            if (inodeTable[i].i_block[0] == 0)
                continue;

            // print out the following info about the inode 
            if (inodeTable[i].i_mode & EXT2_S_IFDIR)
                mode = 'd';
            else if (inodeTable[i].i_mode & EXT2_S_IFREG)
                mode = 'f';
            else if (inodeTable[i].i_mode & EXT2_S_IFLNK)
                mode = 'l';
            printf("[%d] type: %c size: %d links: %d blocks: %d\n", i+1, mode, inodeTable[i].i_size, inodeTable[i].i_links_count, inodeTable[i].i_blocks);
            printf("[%d] Blocks: %d\n", i+1, inodeTable[i].i_block[0]);
        }
    }
    printf("\n");

    // task3, print directory enties
    char type='\0';
    struct ext2_dir_entry_2 *dir_entries;
    printf("Directory Blocks:\n");
    for(i = 0; i < sb->s_inodes_count; i++) 
    {
        // print root inode and used unreserved inodes
        if ((getBit(inode_bitmap, i)==1 && i>=EXT2_GOOD_OLD_FIRST_INO) || (i==EXT2_ROOT_INO-1))
        {
            // skip the inodes that not belong to a directory
            if (!(inodeTable[i].i_mode & EXT2_S_IFDIR))
                continue;
            
            for (int j=0; j<12; j++) {
                if (inodeTable[i].i_block[j] == 0) continue;
                printf("\tDIR BLOCK NUM: %d (for inode %d)\n", inodeTable[i].i_block[j], i+1);
                dir_entries = (struct ext2_dir_entry_2 *)getBlock(inodeTable[i].i_block[j]);
                // while not hit the end og the block
                while ((int)dir_entries < (int)(disk+(inodeTable[i].i_block[0]+1)*EXT2_BLOCK_SIZE)) {
                    if (dir_entries->file_type == EXT2_FT_UNKNOWN)
                        type = 'u';
                    else if (dir_entries->file_type == EXT2_FT_REG_FILE)
                        type = 'f';
                    else if (dir_entries->file_type == EXT2_FT_DIR)
                        type = 'd';
                    else if (dir_entries->file_type == EXT2_FT_SYMLINK)
                        type = 'l';
                    printf("Inode: %d rec_len: %d name_len: %d type= %c name=%s\n", dir_entries->inode, dir_entries->rec_len, dir_entries->name_len, type, dir_entries->name);
                    dir_entries = (void *) dir_entries + dir_entries->rec_len;
                }
            }   
        }
    }

    

    return 0;
}
