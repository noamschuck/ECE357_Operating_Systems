#ifndef EXPLORE_DIRECTORY_H
#define EXPLORE_DIRECTORY_H
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h> 
#include "data.h"
#define _GNU_SOURCE

#define REPORT(a, b, c) (fprintf(stderr, "ERROR %d: An error occured attmpting syscall \"%s%s%s\""". %s.\n", errno, a, b, c, strerror(errno)))
#define BUF_SIZE 4096

int explore_directory(char *papa, metadata *data, ino_t *seen) {
    int bytes_read;
    struct dirent *entry;
    struct stat stat_info;
    char *fname = (char *)malloc(256+strlen(papa)+1);
    DIR *pops;
    // Opens the directory to explore
    pops = opendir(papa);
    
    if(!pops) REPORT("opendir(", papa, ")");
    while((entry = readdir(pops))) {
        if(stat(strcat(strcat(strcpy(fname, papa), "/"), entry->d_name), &stat_info)) REPORT("stat(\"", fname, "\", &stat_info)");
        switch(entry->d_type) {
            case DT_DIR:
                data->num_inodes[0]++;
                break;
            default:
                int end = sizeof(seen), index = sizeof(seen)/sizeof(ino_t *);

                for(int i = 0; i < index; i++) {
                     
                }

                if(stat_info->st_nlink > 1) {
                    realloc(seen, end+sizeof(ino_t *));
                    seen[index] = stat_info->st_ino;
                }
            case DT_LNK:
                data->num_inodes[1]++;
                break;
            case DT_REG:
                data->num_inodes[2]++;
                data->total_size += stat_info.st_size;
                break;
            case DT_BLK:
                data->num_inodes[3]++;
                break;
            case DT_SOCK:
                data->num_inodes[4]++;
                break;
            case DT_CHR:
                data->num_inodes[5]++;
                break;
            case DT_FIFO:
                data->num_inodes[6]++;
                break;
            case DT_UNKNOWN:
                data->num_inodes[7]++;
                break;
        }
        
    }

    if(errno) REPORT("readdir(", papa, " directory entry)"); // If readdir failed

    free(fname);

    return 0;
}

#endif
