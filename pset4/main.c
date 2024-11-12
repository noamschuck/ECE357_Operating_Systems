#include "macros.h"

int total_bytes = 0, total_files = 0, done_running = 0, pid[] = {0, 0, 0};
jmp_buf jmp_point;
struct sigaction sa1, sa2;

int redirect(int in, int out);
void handler_1(int s);
void handler_2(int s);

int main(int argc, char *argv[]) {
    char *pattern, buf[4096];
    int fd, fds1[2], fds2[2], process = PARENT;
    
    // Terminate if no arguments were entered
    if(argc <= 1) {
        printf("ERROR: No arguments entered.\n\n");
        return -1;
    }
    
    // Defining signals
    sa2.sa_handler = handler_2;
    sa1.sa_handler = handler_1;
    sa1.sa_flags = 0;
    sa2.sa_flags = SA_NODEFER;
    sigemptyset(&sa1.sa_mask);
    sigemptyset(&sa2.sa_mask);
    if(sigaction(SIGUSR1, &sa1, 0)) REPORT("sigaction(SIGUSR1, sa1, 0)", "", "");
    if(sigaction(SIGUSR2, &sa2, 0)) REPORT("sigaction(SIGUSR2, sa2, 0)", "", "");

    pattern = argv[1]; // The grep pattern
    pid[0] = getpid(); //KOALA: remove this?                  
    printf("Parent PID is: %d\n\n", pid[0]);
    // Loop through the files
    for(int i = 2; i < argc; i++) {
        
        total_files++;
        if(setjmp(jmp_point)) goto loop_end;

        // Open the file, if we can't skip to the next one
        if((fd = open(argv[i], O_RDONLY)) < 0) {
            REPORT("open(", argv[i], "O_RDONLY");
            continue;
        }

        // Create the pipes
        if(pipe(fds1) < 0 | pipe(fds2) < 0) {
            REPORT("pipe(fds1) or pipe(fds2)", "", "");
            continue; // KOALA: Do i continue or exit?
        }
        
        // Do the forking for both more and grep
        if((pid[1] = fork()) < 0) { 
            REPORT("fork()", "", "");
            continue; // KOALA: Do i continue or exit?
        } else if(!pid[1]) {
            process = GREP;
        } else {
            if((pid[2] = fork()) < 0) {
                REPORT("fork()", "", "");
                continue; // KOALA: Do i continue or exit?
            } else if(!pid[2]) process = MORE;
        }

        switch(process) {
            case PARENT:
                int bytes_read, bytes_written = 0, prev_write;
                errno = 0;
                while((bytes_read = read(fd, buf, sizeof buf)) > 0) {
                    prev_write = 0;
                    if(bytes_read < 0) { 
                        REPORT("read(fd, buf, 4096)", "", "");
                        continue; // KOALA: should i continue?
                    } 
    
do_write:           bytes_written = write(fds1[1], buf + prev_write, bytes_read - prev_write);
                    total_bytes += bytes_written;
                    
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
                
loop_end:       close(fds1[1]); // KOALA: add error checking for the close.
                close(fds2[1]);
                close(fd);
                close(fds1[0]);
                close(fds2[0]);

                while(wait(NULL) > 0 || errno == EINTR);
                break;

            case GREP:
                close(fds1[1]);
                close(fds2[0]);
                close(fds2[0]);
                errno = 0;
                if(redirect(fds1[0], fds2[1]) < 0) break; // KOALA: what do i do if dup2 fails
                if(!errno) execlp("grep", "grep", pattern, NULL);
                else REPORT("execlp(grep, ...)", "", "");
                close(fds2[1]);
                break;

            case MORE:
                close(fds1[1]);
                close(fds2[1]);
                close(fds1[0]);
                errno = 0;
                if(redirect(fds2[0], 1) < 0) {
                    break;   // KOALA: what do i do if dup2 fails?
                }
                if(!errno) execlp("more", "more", NULL);
                else REPORT("execlp(more, ...)", "", "");
                break;

            default:
                perror("ERROR: Variable 'process' has an unexpected value in the switch area!\n");
                break;
        }
    }
        if(fork() == 0) execlp("stty", "stty", "sane", NULL);
        else wait(NULL);

        return 0;
}

void handler_1(int s) {
    if(s == SIGUSR1) {
        fprintf(stderr, "\n-----------\nCurrent Statistics:\n  Files Parsed: %d\n  Bytes Written: %d\n----------\n", 
                total_files, total_bytes);
    }
}

void handler_2(int s) {
    if(s == SIGUSR2) {
        fprintf(stderr, "\n* * * * SUGSUR2 recieved, moving onto file #%d (or ending if that was the last file) * * * *\n", total_files+1);
        kill(pid[1], SIGTERM);
        kill(pid[2], SIGTERM);
        longjmp(jmp_point, 1);
    }
}

int redirect(int in, int out) {
    errno = 0;
    if(dup2(in, 0) < 0) REPORT("dup2(in, 0)", "", "");
    if(dup2(out, 1) < 0) REPORT("dup2(out, 1)", "", "");
    if(errno) return -1;
    return 0;
}
