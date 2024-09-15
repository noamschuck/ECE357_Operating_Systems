#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ERROR_MESSAGE(a, b, c) (printf("ERROR: System call %s%s%s failed because \"%s\" (Error #%d).\n\n", a, b, c, strerror(errno), errno))

int main(int argc, char *argv[]) {
    int opt, i, fd_output = -1, fd_temp, fd_input, r_out, w_out, buf_len = 4096, o = 0, b = 0; //o indicates how many times the o flag was used and b indicates buf flag
    char *fname;

    // Terminate if no arguments were entered
    if(argc <= 1) {
        printf("ERROR: No arguments entered.\n\n");
        return -1;
    }

    // Parses through all of the arguments and adds them to arguments
    char **arguments[argc-1]; // Array that holds all of the arguments
    
    for(i = 1; i < argc; i++ ) {
        arguments[i-1] = (argv+i);
        if(!strcmp(*arguments[i-1], "-o")) {
            if(o++ > 0) {
                printf("ERROR: Two output files cannot be designated.");
                return -1;
            } else {
                fname = *(argv+i+1);
            }
            continue;
        } else if(!strcmp(*arguments[i-1], "-b")) {
            if(b++ > 0) {
                printf("ERROR: Two buffer sizes cannot be designated.");
                return -1;
            }
            continue;
        } else if(b > 0) {
            b--;
            buf_len = atoi(*arguments[i-1]);
            continue;
        } else if(o && !strcmp(*arguments[i-2], "-o")) {
            fd_output = open(*arguments[i-1], O_WRONLY | O_CREAT | O_TRUNC, 0667);
            if(errno) {
                ERROR_MESSAGE("open(\"", *arguments[i-1],"\", O_WRONLY | O_CREAT | O_TRUNC, 0666)");
                return -1;
            }
            continue;
        } else {
            if(fd_output == -1) {
                fd_output = 1; // If parsing the first argument, then stdout must be the output
            }
            
            // Open file to read
            fd_input = !strcmp(*arguments[i-1], "-") ? 0 : open(*arguments[i-1], O_RDONLY);
            if(errno) {
                ERROR_MESSAGE("open(\"", *arguments[i-1], "\", O_RDONLY)");
                return -1;
            }
            
            char buf[buf_len];
            
            // Read file and print to output
            while((r_out = read(fd_input, buf, sizeof buf)) != 0) {
                if(errno) {
                    ERROR_MESSAGE("read() on file \"", *arguments[i-1], "\"");
                    return -1;
                } else {
                    w_out = write(fd_output, buf, r_out);
                    if(errno) {
                        ERROR_MESSAGE("writing file \"", *arguments[i-1], "\" to output");
                        return -1;
                    } else if(w_out < r_out) 
                        printf("ERROR: Partial write occured when writing file \"%s\" to temperary output.\n\n", *arguments[i-1]);
                }
            }
        }
    }
    return 0;
}
