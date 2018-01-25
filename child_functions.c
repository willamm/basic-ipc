#include "child_functions.h"

/**
 * FUNCTION: error_handler
 *
 * DATE: January 21, 2018
 *
 * DESIGNER: William Murphy
 *
 * PROGRAMMER: William Murphy
 *
 * INTERFACE: void error_handler(const char* err_msg)
 *
 * RETURNS: void
 *
 * NOTES:
 * Handles errors raised during the program's execution.
 */ 
void error_handler(const char* err_msg)
{
	fprintf(stderr, "%s: %s\n", err_msg, strerror(errno));
	system("stty sane");
	exit(EXIT_FAILURE);
}

/**
 * FUNCTION: translate
 *
 * DATE: January 21, 2018
 *
 * DESIGNER: William Murphy
 *
 * PROGRAMMER: William Murphy
 *
 * INTERFACE: void translate(char* buffer, size_t bufsize)
 *
 * RETURNS: void
 * 
 * NOTES:
 * Turns every 'a' in the buffer into a 'z'.
 */
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

/**
 * FUNCTION: child_cleanup
 *
 * DATE: January 21, 2018
 *
 * DESIGNER: William Murphy
 *
 * PROGRAMMER: William Murphy
 *
 * INTERFACE: bool child_cleanup(const int* pipe, int signum, int num_children, ...)
 *
 * RETURNS: bool
 * 
 * NOTES:
 * Cleans up after each child process. Can be used with a variable number
 * of child processes as long as you know how many processes you want to kill.
 */
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
