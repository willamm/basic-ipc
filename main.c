#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

#include "child_functions.h"

int main()
{
	// Enable raw input
	system("stty raw igncr -echo");
	
	// Number of child processes
	const int NUM_CHILDREN = 2;
	// Buffer for keyboard input
	char c;
	
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
		const size_t BUFSIZE = 1024;
		char buffer[BUFSIZE];
		// to avoid compiler warnings
		(void)buffer;
		const char* bspace = "\b \b";

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
				translate(buffer, sizeof(buffer));
				write(output_fd[1], "\n\r", 2);
				write(output_fd[1], buffer, strlen(buffer));
				write(output_fd[1], "\r", 1);
			}
		}
		else
		{
			// in input process
			close(translate_pipe[0]);
			bool done = false;
			size_t bsLen = strlen(bspace);
			while (read(0, &c, 1) > 0 && !done) 
			{
				if (c == 'X') 
				{
					write(output_fd[1], bspace, bsLen);
				}
				else if (c == 'T')
				{
					done = child_cleanup(&output_fd[1], SIGTERM, NUM_CHILDREN, output_id, translate_id);
				}
				else if (c == 'E') 
				{
					buffer[bufLoc] = '\n';
					// Write buffer to translate process
					write(translate_pipe[1], buffer, sizeof(buffer));
					memset(buffer, '\0', sizeof(buffer));
					bufLoc = 0;
				}
				else if (c == 'K')
				{
					// line kill
					size_t i = 0;
					while (buffer[i++] != '\n' && i < BUFSIZE)
					{
						write(output_fd[1], bspace, bsLen);
					}
					memset(buffer, '\0', sizeof(buffer));
					bufLoc = 0;
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
	// Reenable normal keyboard input.
	system("stty -raw -igncr echo");
	return 0;
}

