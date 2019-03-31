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

	#include <string.h>
	#include <time.h>
	#include <limits.h>

	#define TRUE 1
	#define FALSE 0
	#define BILLION 1E9

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
			close(STDIN_FILENO);
			open(filev[0], O_RDONLY);
		}
		// Redirect the OUTPUT.
		if (filev[1] != NULL){
			close(STDOUT_FILENO);
			int fd = open(filev[1], O_CREAT | O_TRUNC | O_RDWR, 0666);
		}
		// Redirect the ERROR.
		if (filev[2] != NULL){
			close(STDERR_FILENO);
			open(filev[2], O_CREAT | O_TRUNC | O_WRONLY, 0666);
		}
		return 1;
	}

	int shiftArgArray(char ***argvv){
		int ii;
		for(ii = 0; argvv[0][ii + 1] != NULL; ++ii){
			argvv[0][ii] = argvv[0][ii + 1];
		}
		argvv[0][ii] = NULL;
		return 1;
	}

	int normalExec(char ***argvv, char *filev[], int command_counter, int bg){
		pid_t pid;
		if ((pid = fork()) == 0){
			checkRedirections(filev);
			if ((execvp(argvv[command_counter][0], argvv[command_counter])) == -1){
				fprintf(stderr, "Unknown command: %s\n", argvv[command_counter][0]);
			}
		} else {
			if (bg == FALSE){
				wait(NULL);
			} else {
				printf("[%d]\n", getpid());
			}
		}
		return 1;
	}

	int redirExec(char ***argvv, char *filev, int command_counter, int num_commands, int bg, int pipe1[], int pipe2[]){

		pid_t pid;

		if ((pid = fork()) == 0){

			if (command_counter == 0){
				close(STDOUT_FILENO);
				// Output of the process goes to the input of the pipe.
				dup(pipe1[1]);
			} else if (command_counter == (num_commands - 1)){
				close(STDIN_FILENO);
				// Input of the process is read from the output of the pipe.
				dup(pipe2[0]);
			} else {
				close(STDIN_FILENO);
				close (STDOUT_FILENO);
				// Input of the process is read from the output of the previous pipe.
				dup(pipe1[0]);
				// Output of the process goes into the input of the "current" pipe.
				dup(pipe2[1]);
			}

			checkRedirections(filev);
			execvp(argvv[command_counter][0], argvv[command_counter]);
		} else {
			if (bg == FALSE){
				wait(NULL);
			} else {
				printf("[%d]\n", getpid());
			}
		}

		return 1;
	}


	int myTimeExec(char ***argvv, char *filev, int command_counter, int bg){
		shiftArgArray(argvv);

		struct timespec timeStart;
		clock_gettime(CLOCK_MONOTONIC, &timeStart);

		normalExec(argvv, filev, command_counter, bg);

		struct timespec timeEnd;
		clock_gettime(CLOCK_MONOTONIC, &timeEnd);
		double timeTaken = (double)(timeEnd.tv_sec-timeStart.tv_sec) + (timeEnd.tv_nsec-timeStart.tv_nsec) / BILLION;
		if (timeTaken < 0){
			fprintf(stderr, "Usage: mytime <command<args>>\n");
		} else {
		printf("Time spent: %f secs.\n", timeTaken);
	}
		return 1;
	}

	int mygetcwd(){
		char cwd[PATH_MAX];
		if(getcwd(cwd, sizeof(cwd)) != NULL) {
			printf ("Current working dir: %s\n", cwd);
		} else {
			printf ("Mypwd error");
			return 0;
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


			int pipe1[2];
			int pipe2[2];
			if (num_commands > 1) {
				pipe(pipe1);
				pipe(pipe2);
			}


			//////// Command Handling //////////////////
			for (command_counter = 0; command_counter < num_commands; command_counter++){
				// Counts the number of arguments for the current command.
				for (args_counter = 0; (argvv[command_counter][args_counter] != NULL); args_counter++);

				// -·-·-·- Print Symbols for Redirection or Background execution -·-·-·-//
				if (bg == TRUE) printf("&\n");
				if (filev[0] != NULL) printf("< %s\n", filev[0]);/* IN */
				if (filev[1] != NULL) printf("> %s\n", filev[1]);/* OUT */
				if (filev[2] != NULL) printf(">& %s\n", filev[2]);/* ERR */
				printf("\n");
				// -·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·- //


				// -·--·-·-·-·-·-·- Child Creation and Transformation -·-·-·-·-·-·-·- //
				if (strcmp(argvv[command_counter][0], "mypwd") == 0) {
					mygetcwd();
				}
				else if (strcmp(argvv[command_counter][0], "mytime") == 0) {
					myTimeExec(argvv, filev, command_counter, bg);
				} else {
					if (num_commands > 1){
						redirExec(argvv, filev, command_counter, num_commands, bg, pipe1, pipe2);
					} else {
						normalExec(argvv, filev, command_counter, bg);
					}
				}
				// -·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·- //

			} // End of Command Handling.

		} // End of Shell Input Prompt.
		return 0;
	} // End of Main.
