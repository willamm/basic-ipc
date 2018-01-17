#include "child_functions.h"

void error_handler(const char* err_msg)
{
	fprintf(stderr, "%s: %s\n", err_msg, strerror(errno));
	system("stty sane");
	exit(EXIT_FAILURE);
}

void output_process(const int pipe_fd, const int write)
{	

}

void translate_process(const int* pipe_fd);
