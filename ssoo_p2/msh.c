	#include <stdlib.h>
	#include <stddef.h>			/* NULL */
	#include <stdio.h>			/* setbuf, printf, perror */
	#include <unistd.h>			/* pipes */
	#include <sys/wait.h>		/* child reaping*/
	#include <sys/stat.h>		//open
	#include <sys/types.h>	//open
	#include <fcntl.h> 			//open
	#include <signal.h>			/* signal handling */
	#include <errno.h>			/* system error number */

	#include <time.h>

	#define TRUE 1
	#define FALSE 0

	extern int obtain_order();		/* See parser.y for description */

	/* Explain what this does and hone it */
	int reapChildren(){
		/*pid_t pid =*/ waitpid(-1, 0, WNOHANG);
		//if (pid > 0) printf("BG process has been reaped. [PID: %d]\n", pid);
		return 1;
	}

	int checkRedirections(char *filev[]){
		// Redirect the INPUT.
		if (filev[0] != NULL){
			int fd = open(filev[0], O_RDONLY);
			close(STDIN_FILENO);
			dup(fd);
			close(fd);
		}
		// Redirect the OUTPUT.
		if (filev[1] != NULL){
			int fd = open(filev[0], O_WRONLY);
			close(STDOUT_FILENO);
			dup(fd);
			close(fd);
		}
		// Redirect the ERROR.
		if (filev[2] != NULL){
			int fd = open(filev[0], O_RDWR);
			close(STDERR_FILENO);
			dup(fd);
			close(fd);
		}
		return 1;
	}

	int main(void)
	{
		///////// Variables to handle the Input /////////
		char ***argvv;
		int command_counter;
		int args_counter;
		int num_commands;
		char *filev[3];
		int bg;
		int ret;
		/////////////////////////////////////////////////

		// what's this done for?
		setbuf(stdout, NULL);			/* Unbuffered */
		setbuf(stdin, NULL);

		while (1)
		{

			reapChildren();

			////// Prompt and Basic input cases ////////
			fprintf(stderr, "%s", "msh> ");	/* Prompt */
			ret = obtain_order(&argvv, filev, &bg);
			if (ret == 0) break;		/* EOF */
			if (ret == -1) continue;	/* Syntax error */
			num_commands = ret - 1;
			if (num_commands == 0) continue;	/* Empty line */
			////////////////////////////////////////////

			//////// Command Handling //////////////////
			for (command_counter = 0; command_counter < num_commands; command_counter++){
				//Prints a prompt of the command to execute and its arguments:
				for (args_counter = 0; (argvv[command_counter][args_counter] != NULL); args_counter++){
					printf("%s ", argvv[command_counter][args_counter]);
				}

				// -·-·-·- Print Symbols for Redirection or Background execution -·-·-·-//
				if (bg == TRUE) printf("&\n");
				if (filev[0] != NULL) printf("< %s\n", filev[0]);/* IN */
				if (filev[1] != NULL) printf("> %s\n", filev[1]);/* OUT */
				if (filev[2] != NULL) printf(">& %s\n", filev[2]);/* ERR */
				printf("\n");
				// -·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·- //

				// A child is created and morphed into the desired process. If foreground,
				// parent process is blocked until child is reaped.
				pid_t pid;
				printf("Startime: %ld\n", start);
				if ((pid = fork()) == 0){
					checkRedirections(filev);
					execvp(argvv[command_counter][0], argvv[command_counter]);
				} else {
					if (bg == FALSE){
						wait(NULL);
					} else {
						printf("[%d]\n", getpid());
					}
				}
			} // End of Command Handling.

		} // End of Shell Input Prompt.
		return 0;
	} // End of Main.
