#ifndef MACROS_H
#define MACROS_H

#include <dirent.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h> 

#define REPORT(a, b, c) (fprintf(stderr, "ERROR %d: An error occured attmpting \"%s%s%s\""". %s.\n", errno, a, b, c, strerror(errno)))
#define BUFF_SIZE 255

#endif