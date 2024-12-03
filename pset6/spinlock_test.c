#include "spinlock.h"
#include "tas.h"
#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

int action(long *money);

int main() {
    long *money1, *money2;
    int pid;
    char *lock;

    money1 = mmap(NULL, sizeof(int), PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    money2 = mmap(NULL, sizeof(int), PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    lock = mmap(NULL, sizeof(char), PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *lock = 0;
    
    for(int i = 0; i < 10; i++) {
        if((pid = fork()) == 0) {
            action(money1);
            spin_lock(lock);
            action(money2);
            spin_unlock(lock);
            break;
        }
    }
    if(pid == 0) exit(EXIT_SUCCESS);
    while(wait(NULL) > 0 && errno != ECHILD) {}

    printf("\nBalance w/out spinlock: %ld\n", *money1);
    printf("Balance w spinlock:     %ld\n", *money2);

    return 0;
}

int action(long *money) {
    int temp;
    temp = *money;
    sleep(1);
    *money = temp+1;
    return 0;
}
