#include "sem.h"

void sem_init(struct sem *s, int count) {
    s->count = count;
    s->max_count = count;
    s->lock = 0;
}

int sem_try(struct sem *s) {
    if(s->count > 0) { // If we have more resources availible
        s->count--;
        return 1;
    } else return 0; // We dont have resources availible
}

void sem_wait(struct sem *s) { 
    int num_elements;
    sigset_t sleep_mask, sigusr_mask;

try_again:

    spin_lock(&(s->lock));
    //printf("  %d accessing shell %d to take (count %d)\n", tid, s->id, s->count);
    if(!sem_try(s)) {
        
        // Make a new  maske that blocks everything (later removes SIGUSR1)
        if(sigfillset(&sleep_mask) == -1) {
            perror("ERROR: sigfillset(sleep_mask) resulted error");
            return; // TODO: Should I just be returning if they dont work?
        }
        
        // Remove SIGUSR1 from the new mask
        if(sigdelset(&sleep_mask, SIGUSR1) == -1) { // TODO: Is SIGUSR1 the only one i should unmask?
            perror("ERROR: sigdelset(sleep_mask, SIGUSR1) resulted error");
            return;
        }

        // Make a new mask that blocks SIGUSR1
        if(sigemptyset(&sigusr_mask) == -1) { 
            perror("ERROR: sigemptyset(&sigusr_mask, SIGUSR1) resulted error");
            return;
        }

        // Make a new mask that blocks SIGUSR1
        if(sigaddset(&sigusr_mask, SIGUSR1) == -1) { 
            perror("ERROR: sigaddset(&sigusr_mask, SIGUSR1) resulted error");
            return;
        }

        // Block SIGUSR1
        if(sigprocmask(SIG_BLOCK, &sigusr_mask, NULL) == -1) {
            perror("ERROR: sigprocmask(SIG_SETBLOCK, sigusr_mask, NULL) resulted in error");
            return;
        }

        // Add current process to wait queue (CRITICAL REGION)
        (s->sleeping)[s->num_sleeping++] = getpid();
        (s->sstats[getpid()%6])++;

        spin_unlock(&(s->lock));

        // Wait
        //printf("  %d sleep\n", tid);
        if(sigsuspend(&sleep_mask) == -1 && errno != EINTR) {
            perror("ERROR: sigsuspend(sleep_mask)");
            return;
        }

        errno = 0; // sigsuspend (if it returns) always sets errno

        // Unblock SIGUSR1
        if(sigprocmask(SIG_UNBLOCK, &sigusr_mask, NULL) == -1) {
            perror("ERROR: sigprocmask(SIG_UNBLOCK, ^sigusr_mask, NULL) resulted in error");
            return;
        }
        
        goto try_again;

    } else {
        spin_unlock(&(s->lock)); // Unlock the semaphore if we successfully got the rock!
    }
}

void sem_inc(struct sem *s) {
    spin_lock(&(s->lock));
    
    if(s->num_sleeping > 0) { // If there is stuff waiting
        for(int i = 0; i < s->num_sleeping; i++) {
            // wake up the sleeping process
            if(kill((s->sleeping)[i], SIGUSR1) == -1) {
                perror("ERROR: kill((s->sleeping)[i], SIGUSR1) resulted in error");
                return; 
            }
            (s->sleeping)[i] = 0;
        }
        s->num_sleeping = 0;
        (s->astats[getpid()%6])++;
    } 
    s->count++;
    // Unlock the semaphore
    spin_unlock(&(s->lock));
}
