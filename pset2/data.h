#ifndef DATA_H
#define DATA_H

#include <stdio.h>

typedef struct {
    int num_inodes[7]; // Directory, Simlink, File, Block Device, Socket, Character Device, Named Pipe
    int total_size; // Total size of regular files
    int block_count; // Total number of blocks allocated to regular files
    int link_count; // Number of inodes which are hard linked
    int bad_symlink_count; // Number of symlinks that don't resolve
    int bad_filenames_count; // Number of problematic filenames
} metadata;


int print_metadata(metadata data) {
    printf("Files Size: %d\n", data.total_size);
    printf("File Blocks: %d\n", data.block_count);
    printf("Hard-Link: %d\n", data.link_count);
    printf("Bad Symlinks: %d\n", data.bad_symlink_count);
    printf("Bad Filenames: %d\n\n", data.bad_filenames_count);

    printf("Directory iNode: %d\n", data.num_inodes[0]);
    printf("Simlink iNode: %d\n", data.num_inodes[1]);
    printf("File iNode: %d\n", data.num_inodes[2]);
    printf("Block Device iNode: %d\n", data.num_inodes[3]);
    printf("Socket iNode: %d\n", data.num_inodes[4]);
    printf("Character Device iNode: %d\n", data.num_inodes[5]);
    printf("Named Pipe iNode: %d\n", data.num_inodes[6]);

    return 0;
}
#endif
