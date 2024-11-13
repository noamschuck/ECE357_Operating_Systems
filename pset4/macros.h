#ifndef MACROS_H
#define MACROS_H

#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h> 
#include <setjmp.h>

#define REPORT(a, b, c) (fprintf(stderr, "ERROR %d: An error occured attmpting \"%s%s%s\""". %s.\n", errno, a, b, c, strerror(errno)))
#define GREP 1
#define MORE 2
#define PARENT 0

#endif
