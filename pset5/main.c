#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#define REPORT(a, b, c) (fprintf(stderr, "ERROR %d: An error occured attmpting \"%s%s%s\""". %s.\n", errno, a, b, c, strerror(errno)))

int fd;

int test1();
int test23(int test);
int test4();
void handler1(int s);

int main(int argc, char *argv[]) {
    if((fd = open("file", O_CREAT | O_TRUNC| O_RDWR, 0666)) == -1) {
        REPORT("open(\"file\", O_CREAT | O_TRUNC, S_IRWXU)", "", "");
        exit -1;
    }

    printf("Running Test #%d:\n", atoi(argv[1]));

    switch(atoi((argv[1]))) {
        case 1:
            struct sigaction sa;
            sa.sa_handler = handler1;
            sa.sa_flags = 0;
            if(sigemptyset(&sa.sa_mask) == -1) REPORT("sigemptyset(&sa.sa_mask)", "", "");

            for(int i = 1; i < 32; i++) {
                if(i == 9 || i == 19) continue;
                if(sigaction(i, &sa, 0) == -1) {
                    REPORT("sigaction()", "", "");
                    return -1;
                }
            }

            test1();
            break;
        case 2:
            test23(2);
            break;
        case 3:
            test23(3);
            break;
        case 4:
            test4();
            break;
        default:
            perror("ERROR: Invalid test case.\n");
            return -1;
    }

    close(fd);
    return 0;
}

void handler1(int s) {
    fprintf(stderr, "Signal %d received: %s.\n", s, strsignal(s));
    exit(s);
}

int test1() {
    char *ptr;

    if((ptr = mmap(NULL, 4096, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, fd, 0)) == MAP_FAILED) {
        REPORT("mmap(NULL, 4096, PROT_READ, 0, fd, 0)", "", "");
        return -1;
    }

    if(memcpy(ptr, "X", 1) == -1) { // TODO: How to check memcpy for errors?
        if(errno == EINTR) perror("Signal handler should have been called!!");
        
        perror("Writing to a mmap()ed region with PROT_READ results in an error");
        return 255;
    }

    return 0;
}

int test23(int test) {
    char buf[4096], *ptr;

    if((ptr = mmap(NULL, 4096, PROT_WRITE, (test == 2) ? MAP_SHARED : MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
        REPORT("mmap(NULL, 4096, PROT_READ, 0, fd, 0)", "", "");
        return -1;
    }
    
    if(ftruncate(fd, 4096) == -1) {
        REPORT("ftruncate(fd, 4096)", "", "");
        return -1;
    }

    ptr[0] = 'X';

    if(read(fd, buf, 1) == -1) {
        REPORT("read(fd, buf, 1)", "", "");
        return -1;
    }

    if(buf[0] == 'X') exit(0);
    else exit(1);
}


int test4() {
    char buf[4096];

    if((mmap(NULL, 100, PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        REPORT("mmap(NULL, 4096, PROT_READ, 0, fd, 0)", "", "");
        return -1;
    }

    if(read(fd, buf, 10) == -1) {
        REPORT("read(fd, buf, 1)", "", "");
        return -1;
    }

    if(write(fd, "X", 1) == -1) {
        REPORT("write(fd, \"X\", 1)", "", "");
        return -1;
    }

    if(lseek(fd, 10, SEEK_SET) == -1) {
        REPORT("lseek(fd, 10, SEEK_SET)", "", "");
        return -1;
    }

    if(write(fd, "Y", 1) == -1) {
        REPORT("write(fd, \"X\", 1)", "", "");
        return -1;
    }

    if(lseek(fd, 0, SEEK_SET) == -1) {
        REPORT("lseek(fd, 10, SEEK_SET)", "", "");
        return -1;
    }

    if(read(fd, buf, 15) == -1) {
        REPORT("read(fd, buf, 1)", "", "");
        return -1;
    }

    if(buf[10] == 'Y') exit(0);
    else exit(1);
}
