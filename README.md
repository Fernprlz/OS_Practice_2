# OS - Practice 2
## To Do:
* Modify the code inside the while loop of the shell code so that it interprets the commands and performs their operations.
* Implement the functionalities of the commands available for the minishell.
  * [OK!] Simple commands.
  * [···] Simple commands in background.
  * [···] Simple commands with redirection.
  * [OK?] Sequences of commands connected through pipes (**MAX 3 but if implemented for an unlimited amount, extra points**).
  * [···] Combination of all cases above.
  * [···] Internal commands. These are commands that map directly to a system call or a command internally implemented inside the shell. They are:
    * **mytime**
    * **mypwd**: Internal commands are not part of the command sequences and have no file redirections nor are executed in the background.

### Things To Know:
* We are given a parser for commands. To obtain the parsed command line there's a function:

    `int obtain_order(char**** argv, char** filev, int* bg);`

  This return 0 if the user types CTRL+D(End Of Line) or -1 in case of error. **If succesful it returns the number of commands in the line**.
* Background mode:
  * When a simple command is executed in background the *pid* is the one from the process executing that command.
  * If it's a command sequence, de *pid* is the one from the process that executes the last command of the sequence.
  * The minishell may show the prompt mixed with the ouput of the process child. **This is correct behaviour**.
* Foreground mode:
  * Minishell cannot have zombie processes of previous background commands. *Which means...?*

#### Class' Comments
* Consider the use of data structures and state it on the report.
* Demonstrate that you know what you're doing.
* Be careful with comment coverage
* Think of the potential values inside the files, and its features that could make the program subject of errors: size of the file, permissions, values, etc.
* Be careful with the child process retrieval to avoid zombies. Don't only wait for one because you'll end up having zombies.
* Care about redirections because they are hard to debug.
