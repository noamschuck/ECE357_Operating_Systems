#ifndef SEM_H
#define SEM_H

#include <unstid.h>
#include <sys/types.h>
#include <signal.h>

void sem_init(struct sem *s, int count);
int sem_try(struct sem *s);
void sem_wait(struct sem *s);
void sem_inc(struct sem *s);

struct sem {
    int count;
    int max_count;
    pid_t *sleeping;
};
#endif
