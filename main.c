#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <termios.h>
#include <signal.h>
#include <stdbool.h>
#include <stdarg.h>

#include "child_functions.h"

struct termios saved_attributes;

void termination_handler(int signum)
{
	if (signum == SIGTERM)
	{
		error_handler("sigterm");
	}
	if (signum == SIGINT)
	{
		error_handler("sigint");
	}

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
	// kill parent?
	kill(getpid(), signum);	
	return true;
}

int main()
{
	//Set SIGTERM signal handler
	signal(SIGTERM, termination_handler);
	signal(SIGINT, termination_handler);	
	// Enable raw input, disable normal keyboard input.
	system("stty raw igncr -echo");
	
	// Number of child processes
	const int NUM_CHILDREN = 2;
	// Buffer for keyboard input
	char c;

	// Open keyboard as input
	int kb;
	if ((kb = open("/dev/tty1", O_RDONLY | O_NDELAY)) == -1) 
	{
		error_handler("Error while opening /dev/tty for input");
	}
	
	int output_fd[2];
	if ((pipe(output_fd)) == -1)
	{
		error_handler("Error while opening pipe");
	}
	// Start output child process	
	pid_t output_id;
	if ((output_id = fork()) == -1)
	{
		error_handler("Error while forking output process");
	}

	if (output_id == 0) 
	{
		// in output process
		while (read(output_fd[0], &c, 1) > 0) 
		{
			write(1, &c, 1);	
		}
		
	}
	else 
	{
		// in parent process
		close(output_fd[0]);
		size_t bufLoc = 0;
		char buffer[1024];
		(void)buffer;

		// Open pipe to translate process
		int translate_pipe[2];
		if ((pipe(translate_pipe)) == -1) 
		{
			error_handler("Error while opening translate pipe");
		}
		pid_t translate_id;
		if ((translate_id = fork()) == -1) 
		{
			error_handler("Error while forking translate process");
		}

		if (translate_id == 0) 
		{
			// in translate process
			// Only runs once for some reason
			close(translate_pipe[1]);
			while (read(translate_pipe[0], buffer, sizeof(buffer)) > 0)
			{
				size_t i = 0;
				for (i = 0; i != strlen(buffer); i++)
				{
					if (buffer[i] == 'a')
						buffer[i] = 'z';
				}
				write(output_fd[1], buffer, strlen(buffer));
			}
		}
		else
		{
			// in input process
			close(translate_pipe[0]);
			bool done = false;
			while (read(0, &c, 1) > 0 && !done) 
			{
				if (c == 'X') 
				{
					const char* bspace = "\b \b";
					write(output_fd[1], bspace, strlen(bspace));
				}
				else if (c == 'T')
				{
					done = child_cleanup(&output_fd[1], SIGTERM, NUM_CHILDREN, output_id, translate_id);
				}
				else if (c == 'E') 
				{
					buffer[bufLoc] = '\0';
					// Write buffer to translate process
					write(translate_pipe[1], buffer, sizeof(buffer));
					memset(buffer, '\0', sizeof(buffer));
					bufLoc = 0;
				}
				else if (c == 'K')
				{
					// line kill
				}
				else if (c == 11)
				{
					done = child_cleanup(&output_fd[1], SIGINT, NUM_CHILDREN, output_id, translate_id);
				}
				else 
				{
					buffer[bufLoc++] = c;
					write(output_fd[1], &c, 1);
				}
			}
		}
	}
	if (close(kb) == -1) 
	{
		error_handler("Error while closing /dev/tty");
	}

	// Reenable normal keyboard input.
	// TODO: find out why each invocation of the program creates 3 stopped processes
	//system("stty sane");
	system("stty -raw -igncr echo");
	return 0;
}

