#include "macros.h"

volatile int total_bytes = 0, total_files = 0, done_running = 0, pid[] = {0, 0, 0};
volatile int fd, fds1[2], fds2[2];
sigjmp_buf jmp_point;
struct sigaction sa1, sa2;

volatile int change = 0, delete = 1; // TODO: delete this

int redirect(int in, int out);
void handler_1(int s);
void handler_2(int s);
int close_fd(int *fd);

int main(int argc, char *argv[]) {
    int process = PARENT;   
    char *pattern, buf[4096];
    
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
    pid[0] = getpid();

    // Loop through the files
    for(int i = 2; i < argc; i++) {
        total_files++;
        
        // Open the file, if we can't skip to the next one
        if((fd = open(argv[i], O_RDONLY)) < 0) {
            REPORT("open(", argv[i], "O_RDONLY");
            continue;
        }

        // Create the pipes
        if(pipe(fds1) < 0 | pipe(fds2) < 0) {
            REPORT("pipe(fds1) or pipe(fds2)", "", "");
            return -1;
        }

        
        // Do the forking for both more and grep
        if((pid[1] = fork()) < 0) { 
            REPORT("fork()", "", "");
            return -1;
        } else if(!pid[1]) {
            process = GREP;
        } else {
            if((pid[2] = fork()) < 0) {
                REPORT("fork()", "", "");
                return -1;
            } else if(!pid[2]) process = MORE;
        }

        // Setting long jump point
        if(sigsetjmp(jmp_point, 1) && process == PARENT) {
            if(close_fd(&fds1[1]) == -1) REPORT("close(a)", "", "");
            if(close_fd(&fds2[1]) == -1) REPORT("close(b)", "", "");
            if(close_fd(&fd) == -1) REPORT("close(c)", "", "");
            if(close_fd(&fds1[0]) == -1) REPORT("close(d)", "", "");
            if(close_fd(&fds2[0]) == -1) REPORT("close(e)", "", "");
            
            // Wait for children to end
            while(wait(NULL) > 0 || errno == EINTR);
            continue;
        }

        switch(process) {
            case PARENT:
                printf(""); // You can't have a declaration after a label, it must be a statement
                int bytes_read, bytes_written = 0, prev_write;

                // Read loop and writes to pipe
                while((bytes_read = read(fd, buf, sizeof buf)) > 0) {
                    prev_write = 0;
                    if(bytes_read == -1) { 
                        REPORT("read(fd, buf, 4096)", "", "");
                        goto loop_end;
                    } 
                    
do_write:           bytes_written = write(fds1[1], buf + prev_write, bytes_read - prev_write); // Write
                    total_bytes += bytes_written; // Update total bytes written
                    
                    // Error while writing
                    if(bytes_written == -1 && errno != EINTR) { // If error writing not from a signal
                        REPORT("write(fds1[0], buf, sizeof buf)", "", "");
                        goto loop_end;
                    } else if(bytes_read - prev_write < bytes_written && errno == EINTR) { // Partial write
                        fprintf(stderr, "\tPARTIAL\n");
                        prev_write += bytes_written;
                        goto do_write;
                    }
                }
                
                // Close open file descrip
loop_end:       if(close_fd(&fds1[1]) == -1) REPORT("close(1)", "", "");
                if(close_fd(&fds2[1]) == -1) REPORT("close(2)", "", "");
                if(close_fd(&fd) == -1) REPORT("close(3)", "", "");
                if(close_fd(&fds1[0]) == -1) REPORT("close(4)", "", "");
                if(close_fd(&fds2[0]) == -1) REPORT("close(5)", "", "");
                
                // Wait for children to end
                while(wait(NULL) > 0 || errno == EINTR);
                break;

            case GREP:
                // CLose unecessary open file descriptors
                if(close_fd(&fds1[1]) == -1) REPORT("close(6)", "", "");
                if(close_fd(&fds2[0]) == -1) REPORT("close(7)", "", "");
                if(close_fd(&fd) == -1) REPORT("close(8)", "", "");

                if(redirect(fds1[0], fds2[1]) < 0) return -1; // Redirect IO
                
                // Execute grep
                if(execlp("grep", "grep", pattern, NULL) == -1) {
                    REPORT("execlp(grep, ...)", "", "");
                    if(close_fd(&fds2[1]) == -1) REPORT("close(9)", "", "");
                    exit(EXIT_FAILURE);
                }
                break;

            case MORE:
                // Close unecessary open file descriptors
                if(close_fd(&fds1[1]) == -1) REPORT("close(10)", "", "");
                if(close_fd(&fds2[1]) == -1) REPORT("close(11)", "", "");
                if(close_fd(&fds1[0]) == -1) REPORT("close(12)", "", "");
                if(close_fd(&fd) == -1) REPORT("close(15)", "", "");

                if(redirect(fds2[0], 1) < 0) return -1; // Redirect IO

                //Execute more
                if(execlp("more", "more", NULL) == -1) {
                    REPORT("execlp(more, ...)", "", "");
                    if(close_fd(&fds2[0]) == -1) REPORT("close(13)", "", "");
                    exit(EXIT_FAILURE);
                }
                break;

            default:
                perror("ERROR: Variable 'process' has an unexpected value in the switch area!\n");
                break;
        }
    }

    return 0;
}

void handler_1(int s) {
    if(s == SIGUSR1) {
        fprintf(stderr, "\n\n- - - - - - - - - - - - - - - - - - - - - -\n\nCurrent Statistics:\n  Files Parsed: %d\n  Bytes Written: %d\n\n- - - - - - - - - - - - - - - - - - - - - -\n\n", 
                total_files, total_bytes);
        delete = 0; // TODO: delete this
    }
}

void handler_2(int s) {
    if(s == SIGUSR2 && getpid() == pid[0]) {
        fprintf(stderr, "\n\n- - - - - - - - - - - - - - - - - - - - - -\n\n* * * * SUGSUR2 recieved, moving onto file #%d (or ending if that was the last file) * * * *\n\n- - - - - - - - - - - - - - - - - - - - - -\n\n", total_files+1);
        kill(pid[1], SIGINT);
        kill(pid[2], SIGINT);
        siglongjmp(jmp_point, 1);
    }
}

int redirect(int in, int out) {
    if(dup2(in, 0) == -1 || dup2(out, 1) == -1) REPORT("dup2()", "", "");
    return 0;
}

int close_fd(int *fd) {
    if(*fd == -1) return 1;
    int ret = close(*fd);
    *fd = -1;
    return ret;
}
