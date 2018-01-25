#ifndef CHILD_FUNC_H
#define CHILD_FUNC_H

#define _POSIX_SOURCE

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/types.h>
#include <signal.h>

void error_handler(const char*);
void translate(char* buffer, size_t bufsize);
bool child_cleanup(const int* pipe, int signum, int num_children, ...);

#endif // CHILD_FUNC_H
