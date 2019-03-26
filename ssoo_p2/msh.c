/*-
 * msh.c
 *
 * Minishell C source
 * Show how to use "obtain_order" input interface function
 *
 * THIS FILE IS TO BE MODIFIED
 */

/* TO DO:
* - Errors
*
*/

#include <stddef.h>			/* NULL */
#include <stdio.h>			/* setbuf, printf */
#include <unistd.h>
#include <sys/wait.h>

extern int obtain_order();		/* See parser.y for description */

int main(void)
{
	// Concatenated pointers directing to the argument vector.
	char ***argvv;
	// Two counters that will iterate through commands and arguments.
	int command_counter;
	int args_counter;
	// Total of introduced commands.
	int num_commands;
	// Vector of files employed in redirections.
	char *filev[3];
	// Flag that signals if a command should be executed in background.
	int bg;
	// Variable that will store the output of the obtain_order function.
	// This output will be the number of commands input by the user.
	int ret;

	// what's this done for?
	setbuf(stdout, NULL);			/* Unbuffered */
	setbuf(stdin, NULL);

	while (1)
	{
		fprintf(stderr, "%s", "msh> ");	/* Prompt */
		ret = obtain_order(&argvv, filev, &bg);
		if (ret == 0) break;		/* EOF */
		if (ret == -1) continue;	/* Syntax error */
		num_commands = ret - 1;
		if (num_commands == 0) continue;	/* Empty line */

		// Create an array of process id's that host each process (each command)
		// This is done in order to track the process id's -> In preparation for
		// BACKGROUND commands implementation.
		pid_t pid[num_commands];

		// Iterates over the introduced commands in the argv structure.
		for (command_counter = 0; command_counter < num_commands; command_counter++){
			for (args_counter = 0; (argvv[command_counter][args_counter] != NULL); args_counter++){
				//Prints a prompt of the command to execute and its arguments:
				printf("%s ", argvv[command_counter][args_counter]);
			}
			printf("\n");

			// We call a fork() and save the id of the child in our array of process ids.
			// Then we procceed to morph the child into the desired process and wait for it
			// to finish on the parent process.
			if ((pid[command_counter] = fork()) == 0){
				execvp(argvv[command_counter][0], argvv[command_counter]);
			} else {
				wait(NULL);
			}

			printf("\n");
		}

		// No idea what's the functionality of this:

		if (filev[0] != NULL) printf("< %s\n", filev[0]);/* IN */

		if (filev[1] != NULL) printf("> %s\n", filev[1]);/* OUT */

		if (filev[2] != NULL) printf(">& %s\n", filev[2]);/* ERR */

		if (bg) printf("&\n");

	} //fin while

	return 0;

} //end main
