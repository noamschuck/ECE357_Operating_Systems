#include "macros.h"

int redirect(int in, int out);

int main(int argc, char *argv[]) {
    char *pattern, buf[4096];
    int fd, process = PARENT, pid;

    // Terminate if no arguments were entered
    if(argc <= 1) {
        printf("ERROR: No arguments entered.\n\n");
        return -1;
    }

    pattern = argv[1]; // The grep pattern

    int DELETE = open("./out", O_WRONLY | O_TRUNC | O_CREAT); // KOALA: delete this

    // Loop through the files
    for(int i = 2; i < argc; i++ ) {
        int fds1[2], fds2[2];

        // Open the file, if we can't skip to the next one
        if((fd = open(argv[i], O_RDONLY)) < 0) {
            REPORT("open(", argv[i], "O_RDONLY");
            continue;
        }

        // Create the pipes
        errno = 0;
        pipe(fds1);
        pipe(fds2);
        if(pipe(fds1) < 0 | pipe(fds2) < 0) {
            REPORT("pipe(fds1) or pipe(fds2)", "", "");
            continue; // KOALA: Do i continue or exit?
        }

        // Do the forking for both more and grep
        if((pid = fork()) < 0) { 
            REPORT("fork()", "", "");
            continue; // KOALA: Do i continue or exit?
        } else if(!pid) {
            process = GREP;
        } else {
            /*if((pid = fork()) < 0) {
                REPORT("fork()", "", "");
                continue; // KOALA: Do i continue or exit?
            } else if(!pid) process = MORE;*/
        }

        switch(process) {
            case PARENT:
                int bytes_read, bytes_written = 0, prev_write, wstatus;
                errno = 0;
                while((bytes_read = read(fd, buf, sizeof buf)) > 0) {
                    prev_write = 0;
                    if(bytes_read < 0) { 
                        REPORT("read(fd, buf, 4096)", "", "");
                        continue; // KOALA: should i continue?
                    } 
                    fprintf(stderr, "bytes_read = %d\n", bytes_read);
                    
do_write:
                    bytes_written = write(fds1[1], buf + prev_write, bytes_read - prev_write);
                    //bytes_written = write(fds1[1], buf, bytes_read);
                           
                    // Error while writing
                    if(bytes_written < 0 && errno != EINTR) {
                        REPORT("write(fds1[0], buf, sizeof buf)", "", "");
                        continue; // KOALA: should i continue?
                    // Partial Write
                    } else if(bytes_read - prev_write < bytes_written && errno == EINTR) {
                        fprintf(stderr, "\tPARTIAL\n");
                        prev_write += bytes_written;
                        errno = 0;
                        goto do_write;
                    }

                }
                fprintf(stderr, "CLOSING\n");
                close(fds1[1]);
                while(wait(&wstatus) > 0 || errno == EINTR);
                //close(fds2[1]);
                //while(wait(&wstatus) > 0 || errno == EINTR);
                break;
            case GREP:
                // KOALA: What do i do if the dupping fails?
                close(fds1[1]);
                errno = 0;
                redirect(fds1[0], 1);
                if(!errno) execlp("grep", "grep", pattern, NULL);
                //else REPORT("dup2(in, 0)", "", "");
                break;
            case MORE:
                errno = 0;
                dup2(fds2[0], 0);
                if(!errno) execlp("more", "more", NULL);
                else REPORT("dup2(in, 0)", "", "");
                
                break;
            default:
                perror("ERROR: Variable 'process' has an unexpected value in the switch area!\n");
                break;
        }
        close(fd);
        close(fds1[0]);
        close(fds1[1]);
        close(fds2[0]);
        close(fds2[1]);
    }

    return 0;
}

int redirect(int in, int out) {
    errno = 0;
    if(dup2(in, 0) < 0) REPORT("dup2(in, 0)", "", "");
    if(dup2(out, 1) < 0) REPORT("dup2(out, 1)", "", "");
    if(errno) return -1;
    return 0;
}
