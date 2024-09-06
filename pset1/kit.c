#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

// can we assume the flag will be at the start?

int report_error(char *syscall);

int main(int argc, char *argv[]) {
    int opt, i, fd_output;
    
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
        
        // Skip to next argument if the current argument 
        if(!strcmp(*arguments[i-1], "-o")) continue;
        else fd_output = 1; // 1 is the file descripter for stdout

        // If last argument was -o then set the output stream to be this file
        if(i > 1 && !strcmp(*arguments[i-2], "-o")) {
            fd_output = open(*arguments[i-1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if(errno) {
                printf("ERROR: System call %s%s%s failed with error message \"%s (#%d)\"\n\n", "open(\"", *arguments[i-1],"\", O_WRONLY | O_CREAT | O_TRUNC, 0666)", strerror(errno), errno);
                return -1;
            }

            continue;
        }

        
            
    }
    
    // Terminate if the last argument was the -o flag and no output file was specified
    if(!strcmp(*arguments[i-2], "-o")) {
        printf("ERROR: No output file provided.\n\n");
        return -1;
    }
}

