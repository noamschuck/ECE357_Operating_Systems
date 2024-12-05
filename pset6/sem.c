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
    sigset_t newmask, sigusr_mask;
    
    spin_lock(&(s->lock));
    if(!sem_try(s)) {
        
        // Make a new  maske that blocks everything (later removes SIGUSR1)
        if(sigfillset(&newmask) == -1) {
            perror("ERROR: sigfillset(newmask) resulted error");
            return; // TODO: Should I just be returning if they dont work?
        }
        
        // Remove SIGUSR1 from the new mask
        if(sigdelset(&newmask, SIGUSR1) == -1) { // TODO: Is SIGUSR1 the only one i should unmask?
            perror("ERROR: sigdelset(newmask, SIGUSR1) resulted error");
            return;
        }

        // Make a new mask that blocks SIGUSR1
        if(sigaddset(&sigusr_mask, SIGUSR1) == -1) { // TODO: Is SIGUSR1 the only one i should unmask?
            perror("ERROR: sigdelset(sigusr_mask, SIGUSR1) resulted error");
            return;
        }

        // Block SIGUSR1
        if(sigprocmask(SIG_BLOCK, &sigusr_mask, NULL) == -1) {
            perror("ERROR: sigprocmask(SIG_SETBLOCK, sigusr_mask, NULL) resulted in error");
            return;
        }

        // Add current process to wait queue (CRITICAL REGION)
        num_elements = s->max_count - s->count;
        (s->sleeping)[num_elements - 1] = getpid();

        spin_unlock(&(s->lock));
        // Wait
        if(sigsuspend(&newmask) == -1 && errno != EINTR) {
            perror("ERROR: sigsuspend(newmask)");
            return;
        }

        errno = 0; // sigsuspend (if it returns) always sets errno

        // Unblock SIGUSR1
        if(sigprocmask(SIG_UNBLOCK, &sigusr_mask, NULL) == -1) {
            perror("ERROR: sigprocmask(SIG_SETBLOCK, sigusr_mask, NULL) resulted in error");
            return;
        }
    } else {
        spin_unlock(&(s->lock)); // Unlock the semaphore if we successfully got the rock!
    }
}

void sem_inc(struct sem *s) {
    int num_elements;
    
    spin_lock(&(s->lock));
    if(s->count < 1) { // If there is stuff waiting
        // Increment count and shift pids left
        num_elements = s->count + s->max_count;
        
        for(int i = 0; i < num_elements; i++) {
            // wake up the sleeping process
            if(kill((s->sleeping)[i], SIGUSR1) == -1) {
                perror("ERROR: kill((s->sleeping)[i], SIGUSR1) resulted in error");
                return; // TODO: should we return here?
            }
            (s->sleeping)[i] = 0;
        }

        s->count = s->max_count; // Reset the count
    }

    // Unlock the semaphore
    spin_unlock(&(s->lock));
}
