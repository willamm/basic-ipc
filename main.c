#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "child_functions.h"

int main()
{	
	// Enable raw input, disable normal keyboard input.
	system("stty raw igncr -echo");

	//Get termios struct

	//Buffer for keyboard input
	const size_t LENGTH = 1024;
	char buffer[LENGTH];
	char c;
	//buffer[strcspn(buffer, "\0") - 1] = 0;

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
		while (1) 
		{
			read(output_fd[0], &c, 1);
			if (c == 'X') 
			{
				break;
			}
			else
			{
				write(1, &c, 1);
			}
		}

	}
	else 
	{
		// in parent process
		while (read(0, buffer, 1) > 0) {
			write(output_fd[1], buffer, 1);
		}
		wait(NULL);
		exit(EXIT_SUCCESS);
	
	}
	
	// Start translate process as child of the input process

	if (close(kb) == -1) 
	{
		error_handler("Error while closing /dev/tty");
	}

	// Reenable normal keyboard input.
	system("stty sane");
	system("stty -raw -igncr echo");
	return 0;
}

