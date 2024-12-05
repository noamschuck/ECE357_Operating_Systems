#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include "sem.h"

#define NUM_SEM 3

int create_sems(struct sem **sems, int rocks);

int main(int argc, char *argv[]) {
    int move_count, pid, tid;
    struct sem *sems;

    // Create the list of semaphores and initialize them
    if(create_sems(&sems, atoi(argv[1])) == -1) { 
        return -1;
    }
    
    printf("Semaphores rocks: %d %d %d\n", sems[0].count, sems[1].count, sems[2].count);
    printf("Semaphores max_count: %d %d %d\n", sems[0].max_count, sems[1].max_count, sems[2].max_count);
    printf("Semaphores locks: %d %d %d\n", sems[0].lock, sems[1].lock, sems[2].lock);
    //while(wait(NULL) > 0 && errno != ECHILD);

    return 0;
}

int create_sems(struct sem **sems, int rocks) {
    if((*sems = mmap(NULL, sizeof(struct sem)*NUM_SEM, PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
        perror("ERROR: mmap(NULL, sizeof(struct sem)*NUM_SEM, PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0) resulted in");
        return -1;
    }

    for(int i = 0; i < NUM_SEM; i++)
        sem_init(&((*sems)[i]), rocks);
    
    return 0;
}
