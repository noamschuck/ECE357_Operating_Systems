#ifndef SEM_H
#define SEM_H

#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include "spinlock.h"

struct sem {
    int count;
    int max_count;
    pid_t sleeping[6];
    char lock;
    int id; // TODO: delte this
    int num_sleeping;
    int sstats[6];
    int astats[6];
};

void sem_init(struct sem *s, int count);
int sem_try(struct sem *s);
void sem_wait(struct sem *s);
void sem_inc(struct sem *s); 

#endif
