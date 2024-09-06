#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define ERROR_MESSAGE(a, b, c) (printf("ERROR: System call \"%s%s%s\" failed because \"%s\" (Error #%d).\n\n", a, b, c, strerror(errno), errno))

// can we assume the flag will be at the start?

int report_error(char *syscall);

int main(int argc, char *argv[]) {
    int opt, i, fd_output, fd_input, r_out, w_out;
    char buf[4096];
    char *err_msg;
    
    // Terminate if no arguments were entered
    if(argc <= 1) {
        printf("ERROR: No arguments entered.\n\n");
        return -1;
    }

    // Parses through all of the arguments and adds them to arguments
    char **arguments[argc-1]; // Array that holds all of the arguments

    for(i = 1; i < argc; i++ ) {
        arguments[i-1] = (argv+i);
        //printf("Current argument (%d): %s\n", i-1, *arguments[i-1]);
        
        if(i == 1 && !strcmp(*arguments[i-1], "-o")) {
            continue;                               // If first argument is 'o' then continue
        } else if(i == 1) { 
            fd_output = 1;                          // 1 is the file descripter for stdout
        } else if(!strcmp(*arguments[i-2], "-o")) {   // If specifing output then
            if(i != 2) { // The -o flag is not the first option
                printf("ERROR: Two output files cannot be designated.");
                return -1;
            } else {
                // open the file and assign fd_output the fd of the file
                fd_output = open(*arguments[i-1], O_WRONLY | O_CREAT | O_TRUNC, 0666);

                // Error checking
                if(errno) {
                    ERROR_MESSAGE("open(\"", *arguments[i-1],"\", O_WRONLY | O_CREAT | O_TRUNC, 0666)");
                    return -1;
                }
            }
        } else {
            // Open file to read
            fd_input = open(*arguments[i-1], O_RDONLY);
            if(errno) {
                ERROR_MESSAGE("open(\"", *arguments[i-1], "\", O_RDONLY)");
                return -1;
            }

            // Read file and print to output
            while((r_out = read(fd_input, buf, sizeof buf)) != 0) {
                if(errno) {
                    ERROR_MESSAGE("read() on file \"", *arguments[i-1], "\"");
                    return -1;
                } else {
                    w_out = write(fd_output, buf, sizeof buf);
                    if(errno) {
                        ERROR_MESSAGE("writing file \"", *arguments[i-1], "\" to output");
                        return -1;
                    } else if(w_out < sizeof buf) {
                        printf("ERROR: Partial write occured when reading file \"%s\".", *arguments[i-1]);
                    }
                }
            }


            
        }
    }
}
