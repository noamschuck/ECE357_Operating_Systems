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
int create_tasks(int moves);

int move_count, tid;
pid_t pid;

int main(int argc, char *argv[]) {
    struct sem *sems;

    // Create the list of semaphores and initialize them
    if(create_sems(&sems, atoi(argv[1])) == -1) { 
        return -1;
    }
    
    // Create tasks
    if(create_tasks(atoi(argv[2])) == -1) {
        return -1;
    }

    switch(tid) {
        case 7:
            while(wait(NULL) > 0 && errno != ECHILD);
            printf("All done!\n");
            break;
        default:
            printf("VCPU %d (pid %d) has count %d left.\n", tid, pid, move_count);
            break;
    }

    return 0;
}

/* Function: create_sems()
 * Description: Creates a list of three semaphores initialized to 3 and puts the address to the list in the contents of sems.
 * Return Valie: Returns 0 on success and -1 on failure.
*/
int create_sems(struct sem **sems, int rocks) {

    // Allocate space for the semaphores
    if((*sems = mmap(NULL, sizeof(struct sem)*NUM_SEM, PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
        perror("ERROR: mmap(NULL, sizeof(struct sem)*NUM_SEM, PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0) resulted in");
        return -1;
    }

    // Initialize the semaphores
    for(int i = 0; i < NUM_SEM; i++)
        sem_init(&((*sems)[i]), rocks);
    
    return 0;
}

/* Function: create_tasks()
 * Description: Spawns 6 child processes and assigns them a task ID.
 * Return Value: Returns 0 on success and -1 on failure.
*/
int create_tasks(int moves) {
    for(int i = 0; i < 6; i++) {
        if((pid = fork()) == -1) {
            perror("ERROR: fork() resulted in");
            return -1;
        }

        // Initializing values for the children
        if(pid == 0) {
            tid = i;
            pid = getpid();
            move_count = moves;
            return 0;
        }
    }

    // Initializing values for the parent
    tid = 7;
    pid = getpid();
    return 0;
}
