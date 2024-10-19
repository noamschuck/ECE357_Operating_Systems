#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h> 

#define REPORT(a, b, c) (fprintf(errout, "ERROR %d: An error occured attmpting \"%s%s%s\""". %s.\n", errno, a, b, c, strerror(errno)))
#define BUFF_SIZE 255

int main(int argc, char *arg[]) {
    size_t sz = sizeof(char)*BUFF_SIZE;
    char **curr_command = malloc(sz);
    FILE *in = (argc > 1) ? fopen(arg[1], "r") : stdin; // Select where we are getting the input from
    int num_read;

    while((num_read = getline(curr_command, &sz, in)) > 0) {
        printf("Read: %d\n", num_read);
    }
}
