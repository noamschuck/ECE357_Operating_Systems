#include "spinlock.h"
#include <stdio.h>

int spin_lock(char *lock) {
    while(tas(lock)) {
        sched_yield();
    }
    
    return 1;
}

int spin_unlock(char *lock) {
    *lock = 0;
    return 0;
}
