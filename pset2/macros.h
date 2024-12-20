#ifndef MACROS_H
#define MACROS_H

// Includes and stuff that isn't mine
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h> 
#include <math.h>
#define _GNU_SOURCE

//My stuff
#include "data.h"
#include "macros.h"

//Macro variables
#define BUF_SIZE 4096
#define BEEN_THERE_DONE_THAT 1
#define ETA_0 2
#define ITS_A_FOLDER 3
#define YOU_CANT_CALL_ME_THAT 1

// Macro functions
#define REPORT(a, b, c) (fprintf(stderr, "ERROR %d: An error occured attmpting syscall \"%s%s%s\""". %s.\n", errno, a, b, c, strerror(errno)))

#endif
