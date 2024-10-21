#include <dirent.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
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
            // Check if the argument is one of the built in commands              DONT FORGET THIS!!!!!!
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
                _exit(EXIT_FAILURE);
            case 0: // In child
                // Set IO redirection
                char *path = calloc(255, sizeof(char));
                int fd, flags = 0, offset = 0, fd_next;
                for(int i = 0; i < num_io; i++) {

                    if((io[i])[0] == '<') { offset = 1; flags = O_RDONLY; fd_next = 0; }
                    else if((io[i])[0] == '>') {
                        if((io[i])[1] == '>') { offset = 2; flags = O_WRONLY | O_TRUNC | O_CREAT; fd_next = 1; }
                        else { offset = 1; flags = O_WRONLY | O_APPEND | O_CREAT; fd_next = 1;}
                    } else if((io[i])[0] == '2') {
                        if((io[i])[2] == '>') { offset = 3; flags = O_WRONLY | O_APPEND | O_CREAT; fd_next = 2; }
                        else { offset = 2; flags = O_WRONLY | O_TRUNC | O_CREAT; fd_next = 2;}
                    } 

                    if((fd = open(strncpy(path, io[i]+offset, strlen(io[i])), flags)) < 0) {
                        REPORT("open(", path+offset, ", some flags)");
                        exit(EXIT_FAILURE);
                    } else if(dup2(fd, fd_next) < 0) {
                        REPORT("dup2(fd of ", path+offset, ", 0)");
                        exit(EXIT_FAILURE);
                    }
            

                }
                // Execute the command
                _exit(EXIT_SUCCESS);
            default: // In parent
                struct rusage ru;
                int options, status;
                if(wait3(&status, 0, &ru) == -1) REPORT("wait3(&status, *ru, 0)", "", "");
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
