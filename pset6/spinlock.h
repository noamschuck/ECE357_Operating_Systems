#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "tas.h"
#include <sched.h>

int spin_lock(char *lock);
int spin_unlock(char *lock);

#endif
