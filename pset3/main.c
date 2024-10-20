#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h> 

#define REPORT(a, b, c) (fprintf(stderr, "ERROR %d: An error occured attmpting \"%s%s%s\""". %s.\n", errno, a, b, c, strerror(errno)))
#define BUFF_SIZE 255

int main(int argc, char *arg[]) {
    size_t sz;
    char *curr_command, *arguments[20], *io[20], *str;
    FILE *in;
    int num_read = 0, num_args, num_io = 0, child;

    sz = sizeof(char)*BUFF_SIZE;
    curr_command = malloc(sz);
    in = (argc > 1) ? fopen(arg[1], "r") : stdin; // Select where we are getting the input from


    while((num_read = getline(&curr_command, &sz, in)) > 0) {

        // Getting rid of the newline character at the end
        if(curr_command[strlen(curr_command) - 1] == '\n' && strlen(curr_command) == 1) continue;
        else if(curr_command[strlen(curr_command) - 1] == '\n') curr_command[strlen(curr_command) - 1] = '\0';
        printf("%s\n", curr_command);

        // If the line begins with an octothorpe, don't consider it
        if(curr_command[0] == '#') continue;

        // Parse arguments and IO specifications
        str = curr_command;
        num_args = -1;
        while((arguments[++num_args] = strtok(str, " "))) {
            
            // Check if the argument is one of the built in commands
            if(num_args == 0) {
                if(arguments[num_args] == "cd") {
                    
                    continue;
                } else if(arguments[num_args] == "pwd") {

                    continue;
                } else if(arguments[num_args] == "exit") {

                    continue;
                }
                
            }

            // Check if the argument is actually an io change
            for(int i = 0; (arguments[num_args])[i]; i++) {
                if((arguments[num_args])[i] == '<' || (arguments[num_args])[i] == '>') {
                    io[num_io++] = arguments[num_args--];
                    break;
                }
            }

            str = NULL; // necessary for strtok to work properly
        }

        // Fork
        switch(child = fork()) {
            case -1: // An error occured
                REPORT("fork()", "", "");
                exit(EXIT_FAILURE);
            case 0: // In child
                printf("Im born!\n\n");
                _exit(EXIT_SUCCESS);
            default: // In parent
                printf("Im with child.\n\n");
                break;
        }
        // NOTE TO SELF: Be careful using num_args because it will be the last index of arguments, not how many there are

        // Resetting shit for the next command
        num_io = 0;
        num_args = 0;
    }

    if(errno) REPORT("getline(curr_command, &sz, ", (argc > 1) ? arg[1] : "stdin", ")");

    return 0;
}
