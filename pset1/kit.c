#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ERROR_MESSAGE(a, b, c) (printf("ERROR: System call %s%s%s failed because \"%s\" (Error #%d).\n\n", a, b, c, strerror(errno), errno))

// can we assume the flag will be at the start?

int report_error(char *syscall);

int main(int argc, char *argv[]) {
    int opt, i, fd_output, fd_temp, fd_input, r_out, w_out;
    char buf[4096];
    
    // Terminate if no arguments were entered
    if(argc <= 1) {
        printf("ERROR: No arguments entered.\n\n");
        return -1;
    }

    // Parses through all of the arguments and adds them to arguments
    char **arguments[argc-1]; // Array that holds all of the arguments
    
    fd_temp = open(".temp_output_stream_noam_schuck.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(errno) {
        ERROR_MESSAGE("open(\".temp_output_stream_noam_schuck.txt\"", "", ", O_WRONLY | O_CREAT | O_TRUNC, 0666)");
        return -1;
    }

    for(i = 1; i < argc; i++ ) {
        arguments[i-1] = (argv+i);
        //printf("Current argument (%d): %s\n", i-1, *arguments[i-1]);
        
        if(i == 1 && !strcmp(*arguments[i-1], "-o")) continue;                               // If first argument is 'o' then continue
        else if(i != 1 && !strcmp(*arguments[i-2], "-o")) {   // If specifing output then
            if(i != 2) { // The -o flag is not the first option
                printf("ERROR: Two output files cannot be designated.");
                return -1;
            } else {
                // Open file for writing
                fd_output = open(*arguments[i-1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if(errno) {
                    ERROR_MESSAGE("open(\"", *arguments[i-1],"\", O_WRONLY | O_CREAT | O_TRUNC, 0666)");
                    return -1;
                }
            }
        } else {
            if(i == 1) fd_output = 1; // If parsing the first argument, then stdout must be the output

            // Open file to read
            fd_input = !strcmp(*arguments[i-1], "-") ? 0 : open(*arguments[i-1], O_RDONLY);
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
                    w_out = write(fd_temp, buf, r_out);
                    if(errno) {
                        ERROR_MESSAGE("writing file \"", *arguments[i-1], "\" to output");
                        return -1;
                    } else if(w_out < r_out) 
                        printf("ERROR: Partial write occured when writing file \"%s\" to temperary output.\n\n", *arguments[i-1]);
                }
            }
        }
    }

    // Open the temperary file for reading
    fd_temp = open(".temp_output_stream_noam_schuck.txt", O_RDONLY);
    if(errno) {
        ERROR_MESSAGE("open(\".temp_output_stream_noam_schuck.txt\"", "", ", O_RDONLY)");
        return -1;
    }

    // Copy what was written in the temperary file to the designated output
    while((r_out = read(fd_temp, buf, sizeof buf)) != 0) {
        if(errno) {
            ERROR_MESSAGE("read() on file \"", *arguments[i-1], "\"");
            return -1;
        } else {
            w_out = write(fd_output, buf, r_out);
            if(errno) {
                ERROR_MESSAGE("writing file \"", *arguments[i-1], "\" to output");
                return -1;
            } else if(w_out < r_out) 
                printf("ERROR: Partial write occured when writing file \"%s\" to output.\n\n", *arguments[i-1]);
        }
    }

    // Delete the temperary file
    unlink("./.temp_output_stream_noam_schuck.txt");
    if(errno) {
        ERROR_MESSAGE("unlink() on \".temp_output_stream_noam_schuck.txt\"", "", "");
        return -1;
    }

    return 0;
}
