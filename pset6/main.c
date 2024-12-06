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
void handler(int sig);

int move_count, tid, signal_count;
pid_t pid; 

int main(int argc, char *argv[]) {
    struct sigaction sa;
    struct sem *sems;

    // Create the list of semaphores and initialize them
    if(create_sems(&sems, atoi(argv[1])) == -1) { 
        return -1;
    }
    
    // Set signal handler
    sa.sa_handler = handler;
    sa.sa_flags = 0;
    if(sigemptyset(&sa.sa_mask) == -1) {
        perror("ERROR: sigemptyset(&sa.sa_mask) resulted in error");
    }

    if(sigaction(SIGUSR1, &sa, 0) == -1) {
        perror("ERROR: sigaction(SIGUSR1, &sa, 0) resulted in error");
        return -1;
    }

    // Create tasks
    if(create_tasks(atoi(argv[2])) == -1) {
        return -1;
    }

    switch(tid) {
        case 7:
            //sigaction(SIGUSR1, &sa, 0); // TODO: delete
            while(wait(NULL) > 0 || errno == EINTR);

            printf("\n\n- - - - - - - FINAL REPORT - - - - - - -\n");
            printf("\nSem 1:\n  Count =  %d\n  Lock = %d\n", sems[0].count, sems[0].lock);
            printf("\nSem 2:\n  Count =  %d\n  Lock = %d\n", sems[1].count, sems[1].lock);
            printf("\nSem 3:\n  Count =  %d\n  Lock = %d\n", sems[2].count, sems[2].lock);
            printf("- - - - - - - - - - - - - - - - - - - - - -\n\n");

            break;
        default:
            printf("**VCPU %d (pid %d) is starting.\n", tid, pid);

            while(move_count) {
                sem_wait(&(sems[tid/2]));
                sem_inc(&(sems[2-(tid+3)%3]));
                move_count--;
            }

            printf("--VCPU %d (pid %d) finished. Signal handler invoked %d times.\n", tid, pid, signal_count);
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
    for(int i = 0; i < NUM_SEM; i++) {
        sem_init(&((*sems)[i]), rocks);
        (*sems)[i].id = i; //delete
        (*sems)[i].num_sleeping = 0; 
    }
    
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

/* Function: handler()
 * Description: Handles signal SIGUSR1
*/
void handler(int sig) {
    if(sig != SIGUSR1) fprintf(stderr, "ERROR: Ended up in the signal handler, but signal was not SIGUSR1.");  
    else signal_count++;
}

