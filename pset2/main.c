#include "macros.h"

int explore_directory(char *papa, metadata *data, ino_t *seen);

int main(int argc, char *arg[]) {
    metadata data = {{0, 0, 0}, 0, 1, 0, 0, 0};
    ino_t *iNodes = NULL;

    explore_directory("./test_folder", &data, iNodes);
    print_metadata(data);
}

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

        if(entry->d_type != DT_DIR) {
            int end = sizeof(seen), index = sizeof(seen)/sizeof(ino_t); 
                
            // Check if we have seen the inode before
            int leave = 0;
            for(int i = 0; i < index; i++) {
                if(seen && (seen[i] == stat_info.st_ino)) {
                    leave = 1;
                    break;
                }
            }
            if(leave) continue;
            
            // If we havent seen the inode then add it to the list
            if(stat_info.st_nlink > 1) {
                data->link_count++;
                seen = realloc(seen, end + sizeof(ino_t));
                seen[index-1] = stat_info.st_ino;
            }
        }

        switch(entry->d_type) {
            
            case DT_DIR:
                data->num_inodes[0]++;
                break;
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
            default:
                fprintf(stderr, "ERROR: Somehow the d_type for the dirent is none of the options.");
                return -1;
        }
        
    }

    if(errno) REPORT("readdir(", papa, " directory entry)"); // If readdir failed

    free(fname);

    return 0;
}
