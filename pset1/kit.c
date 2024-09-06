#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int opt, i;

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
    }
    
    // Terminate if the last argument was the -o flag and no output file was specified
    if(!strcmp(*arguments[i-2], "-o")) {
        printf("ERROR: No output file provided.\n\n");
        return -1;
    }
}
