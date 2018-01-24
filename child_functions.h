#ifndef CHILD_FUNC_H
#define CHILD_FUNC_H

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

void error_handler(const char*);
void output_process(const int pipe_fd, const int write);
void translate_process(const int* pipe_fd);

#endif // CHILD_FUNC_H
