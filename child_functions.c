#include "child_functions.h"

void error_handler(const char* err_msg)
{
	fprintf(stderr, "%s: %s\n", err_msg, strerror(errno));
	system("stty sane");
	exit(EXIT_FAILURE);
}


void translate(char* buffer, size_t bufsize)
{
	size_t i = 0;
	for (i = 0; i < bufsize; i++)
	{
		if (buffer[i] == 'a')
		{
			buffer[i] = 'z';
		}
	}
	buffer[i] = '\0';
}

bool child_cleanup(const int* pipe, int signum, int num_children, ...)
{
	system("stty sane");
	write(*pipe, "\n", 1);
	va_list pid_list;

	va_start(pid_list, num_children);

	pid_t pid;
	for (size_t i = 0; i < num_children; i++)
	{
		pid = va_arg(pid_list, pid_t);
		kill(pid, signum);
	}
	kill(getpid(), signum);
	return true;
}
