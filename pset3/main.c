#include "macros.h"

int main(int argc, char *arg[]) {
    size_t sz;
    char *curr_command, *arguments[20], *io[20], *str;
    FILE *in;
    int num_read = 0, num_args, num_io = 0, child, wexit = 0;

    sz = sizeof(char)*BUFF_SIZE;
    curr_command = malloc(sz);
    in = (argc > 1) ? fopen(arg[1], "r") : stdin; // Select where we are getting the input from

    while((num_read = getline(&curr_command, &sz, in)) > 0) {
        num_io = 0;

        // Getting rid of the newline character at the end
        if(curr_command[strlen(curr_command) - 1] == '\n' && strlen(curr_command) == 1) continue;
        else if(curr_command[strlen(curr_command) - 1] == '\n') curr_command[strlen(curr_command) - 1] = '\0';

        // If the line begins with an octothorpe, don't consider it
        if(curr_command[0] == '#') continue;

        // Parse arguments and IO specifications
        str = curr_command;
        num_args = -1;
        while((arguments[++num_args] = strtok(str, " "))) {
            // Check if the argument is actually an io change
            for(int i = 0; i < 3; i++) {
                if((arguments[num_args])[i] == '<' || (arguments[num_args])[i] == '>') {
                    io[num_io++] = arguments[num_args--];
                    break;
                }
            }
            str = NULL; // necessary for strtok to work properly
        }

        // Check if the argument is one of the built in commands 
        if(!strcmp(arguments[0],"cd")) {
            if(chdir((arguments[1]) ? arguments[1] : getenv("HOME")) < 0) {
                REPORT("chdir(", (arguments[1]) ? arguments[1] : "~", ")");
                errno = 0;
            }
            continue;
        } else if(!strcmp(arguments[0], "pwd")) {
            char path[1000];
            if(!getcwd(path, 1000)) {
                REPORT("getcwd(", arguments[1], ", 1000)");
                errno = 0;
            } else printf("%s\n", path);
            continue;
        } else if(!strcmp(arguments[0], "exit")) {
            if(arguments[1]) exit(atoi(arguments[1]));
            else exit(wexit);
            continue;
        }

        fflush(in);
        // Fork
        switch(child = fork()) {
            case -1: // An error occured
                REPORT("fork()", "", "");
                exit(EXIT_FAILURE);
                break;
            case 0: // In child
                // Set IO redirection
                char *path = calloc(255, sizeof(char));
                int fd, flags = 0, offset = 0, fd_next;
                for(int i = 0; i < num_io; i++) {
                    if((io[i])[0] == '<') { offset = 1; flags = O_RDONLY; fd_next = 0; }
                    else if((io[i])[0] == '>') {
                        if((io[i])[1] == '>') { offset = 2; flags = O_WRONLY | O_APPEND | O_CREAT; fd_next = 1; }
                        else { offset = 1; flags = O_WRONLY | O_TRUNC | O_CREAT; fd_next = 1;}
                    } else if((io[i])[0] == '2') {
                        if((io[i])[2] == '>') { offset = 3; flags = O_WRONLY | O_APPEND | O_CREAT; fd_next = 2; }
                        else { offset = 2; flags = O_WRONLY | O_TRUNC | O_CREAT; fd_next = 2;}
                    } 

                    if((fd = open(strncpy(path, io[i]+offset, strlen(io[i])), flags)) < 0) {
                        REPORT("open(", path, ", some flags)");
                        exit(EXIT_FAILURE);
                    } else if(dup2(fd, fd_next) < 0) {
                        REPORT("dup2(fd of ", path+offset, ", 0)");
                        exit(EXIT_FAILURE);
                    }
                }

                if(execvp(arguments[0], arguments) < 0) {
                    REPORT("execvp(", arguments[0], ", arguments)");
                    exit(EXIT_FAILURE); //KOALA should it be this or 127
                }
                exit(EXIT_SUCCESS);
                break; // Just in case (?)
            default: // In parent
                struct rusage ru;
                int options, status;
                if(wait3(&status, 0, &ru) == -1) {
                    REPORT("wait3(&status, *ru, 0)", "", "");
                    errno = 0;
                }
                if(!WIFEXITED(status) & WIFSTOPPED(status)) wexit = WSTOPSIG(status);
                else wexit = WEXITSTATUS(status);
                break;
        }
    }

    if(errno) REPORT("getline(curr_command, &sz, ", (argc > 1) ? arg[1] : "stdin", ")");

    return wexit;
}
