#include "macros.h"

int explore_directory(const char *papa);
int check_symlink(char *fname);
int check_inode(struct stat stat_info);
int check_name(char *name);

ino_t *been_there_done_that = NULL;
int num_inodes = 0;
metadata data = {{0, 0, 0}, 0, 0, 0, 0, 0};

int main(int argc, char *arg[]) {

    int return_val = explore_directory(arg[1]);
    
    if(!return_val) print_metadata(data);
    else return -1;

    return 0;
}

int explore_directory(const char *papa) {
    int hard_status = 0, name_status = 0;
    struct dirent *entry = NULL;
    struct stat stat_info;
    char *fname = (char *)malloc(256+strlen(papa)+1);
    if(fname < 0) {
        REPORT("malloc(", "", ")"); 
        return -1;
    }
    DIR *pops = NULL;
    
    // Opens the directory to explore
    pops = opendir(papa);
    if(!pops) {
        REPORT("opendir(", papa, ")");
        return -1;
    }

    while((entry = readdir(pops))) {
        
        // Continue if looking at . or ..
        if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;

        // Get full path name and stat info
        if(lstat(strcat(strcat(strcpy(fname, papa), "/"), entry->d_name), &stat_info)) {
            REPORT("lstat(\"", fname, "\", &stat_info)");
            continue;
        }

        // Check if bad pathname
        name_status = check_name(fname);
        if(name_status == YOU_CANT_CALL_ME_THAT) data.bad_filenames_count++;

        // Check if hard linked and was accounted for
        if((stat_info.st_mode & S_IFMT) != S_IFDIR) 
            hard_status = check_inode(stat_info);
        if(hard_status == BEEN_THERE_DONE_THAT) continue;
        else if(hard_status == ETA_0) {
            data.link_count++;
        }

                    
        // Deal with individual cases
        switch(stat_info.st_mode & S_IFMT) {
            case S_IFDIR:
                data.num_inodes[0]++;
                explore_directory(fname);
                break;
            case S_IFLNK:
                data.num_inodes[1]++;
                if(!check_symlink(fname)) data.bad_symlink_count++;
                break;
            case S_IFREG:
                data.num_inodes[2]++;
                data.block_count += stat_info.st_blocks/8;
                data.total_size += stat_info.st_size;
                break;
            case S_IFBLK:
                data.num_inodes[3]++;
                break;
            case S_IFSOCK:
                data.num_inodes[4]++;
                break;
            case S_IFCHR:
                data.num_inodes[5]++;
                break;
            case S_IFIFO:
                data.num_inodes[6]++;
                break;
            default:
                fprintf(stderr, "ERROR: Somehow the d_type for the dirent is none of the options.");
                return -1;
        }
    }

    if(errno) REPORT("readdir(", papa, ")"); // If readdir failed
    free(fname);

    return 0;
}

int check_inode(struct stat stat_info) {
    int end = (int)sizeof(ino_t) * num_inodes;
        
    // Check if we have seen the inode before
    for(int i = 0; i < num_inodes; i++) {
        if(been_there_done_that && been_there_done_that[i] == stat_info.st_ino) {
            return BEEN_THERE_DONE_THAT;
        }
    }
    
    // If we havent seen the inode then add it to the list
    if(stat_info.st_nlink > 1) {
        been_there_done_that = realloc(been_there_done_that, end + sizeof(ino_t)); // allocating space for another inode
        if(been_there_done_that < 0) {
            REPORT("realloc(been_there_done_that, end + sizeof(ino_t))","","");
            errno = 0;
            return -1;
        }
        been_there_done_that[num_inodes] = stat_info.st_ino;
        num_inodes++;
        return ETA_0;
    }
    return 0;
}

int check_symlink(char *fname) {
    
    struct stat s;
    int return_val = 0;
    if(stat(fname, &s) == -1) {
        if(errno == ENOENT) {
            return_val = 0;
        }
        else {
            REPORT("stat(", fname, ")");
            return_val = -1;
        }
    } else return_val = 1;

    errno = 0;
    return return_val;
}

int check_name(char *name) {
    // 33-46, 48-126
    for(int i = 0; i < strlen(name); i++) {
        if(name[i] < '!' | name[i] > '}' | name[i] == '/') { // im not includeing / because shouldnt be in a file name and not including ~ because its home
            return YOU_CANT_CALL_ME_THAT;
        }
    }

    return 0;
}
