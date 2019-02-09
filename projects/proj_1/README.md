# Project 1 - Group 15

Members:
Andrue Peters
Parsing, prompt, execution, path resolution, built-ins, tokenize

Oscar Flores
Piping, Built-Ins, general debugging

# How to Compile the executable using a makefile
Extract all the files from the tar into your directory on linprog. In the primary directory where the makefile
is located, type in 'make'. The code will compile and create an executable name my_shell. Type in './my_shell'
to run the executable. There are multiple folders in the tar fil that hold the builtins and utility files separately.
This was done to organize the development process. The Builtin folder hold the builtin functions in files, such as
alias.c, alias.h, echo.c, echo.h, and exit.c. The utility folder holds the path resolution and tokenize files. The
primary directory after extracting the files holds the shell.c, shell.h, makefile, pipe.h, and pipe.c, along with
the builtins and utility folder. The makefile is set so that the files in each older are compiled.

# Tar Contents and File Description
1. shell.h: Holds function declarations, cmd struct, and queue use in shell.c
2. shell.c: This is the main routine for the shell. It contains the functions used for parsing. The cmd
  struct declared in shell.c is used to hold the commands input into the command line after they have been
  tokenized. They are pushed to the cmd_queue, which is a queue that holds the command line inputs. The
  cmd_queue is cleared after each command is processed.
3. makefile: compiles all files and links them into executable called my_shell.
4. pipe.h: holds the function declarations for pipe.c file
5. pipe.c: contains the functions for executing processes with a single pipe, double pipe, and triple pipe.
  The double pipe function calls the single pipe function for the first two commands. After the first pipe is
  completed, the single pipe command is called to perform the last pipe. The triple pipe function calls the
  double pipe function, which calls the single pipe command twice. Once this is completed, the single pipe
  command is called to perform the last pipe.
6. path.h: Held in the utility folder. Contain the function declarations for path resolution
7. path.c: Held in the utility folder. Contains the function defintions for path resolution and shortcut
  handling.
8. tokenize.h: Held in the Utility folder. contians the function declarations needed for tokenize.c file
9. tokenize.c: Held in the utility folder. tokenizes using a c-string. The delimeter we used is whitespace.
10. alias.h: Held in the builtins folder. Contains the function declarations for the alias.c file
11. alias.c: Held in the builtins folder. Contains the function declarations to handle aliasing. Aliases are
  stored in a hash table called alias_table
12. cd.h: Held in the Builtins folder. Conains the function declarations used to change directories
13. cd.c: Held in the Builtins Folder. Handles directory changes
14. echo.h: Held in the Builtins Folder. Contains the function declaration used by echo.c file
15. echo.c: Held in the Builtins folder. Prints results of command line input to the screen. Outputs to terminal
  an error message if the environmental variable doesn't exist
16. exit.h: Held in the Builitins folder. Holds the function declarations for exit.c file
17. exit.c: Uses shell_data struct to check for process statuses. Uses loop to check the status and exits when successful.
18. shell_data.h: Held in the builtin folder. Contains the function declaration
19. shell_data.c: Held in Builtin folder. shell_data struct holds all of the PID in GArray. The GArray is used in exit.c
   to check the status of each process to exit.

# Known Bugs and Unfinished Portion


# Special Considerations
The linprog servers were not able to connect online on 02/07/2019. We were unable to pull from the github repository
we had been using for development. We had been told the using github or a repository like it would be ideal to simplify
collaboration on this project. We could not pull in the updated files that each of us had be working on separately to
do general debugging. We sent an email to the TAs regarding this problem. We pinged google.com and got an error message
saying the host was unknown. The next day, Friday February 8, linprog was able to connect online again and we were
able to pull from our github repository.
## Extra Credit
Multiple Pipes: We attempted to support multiple pipes, but we had issues with our command execution, which did not allow us to pipe
correctly. As explained in pipe.c above, a single pipe function was built. The idea was that the first command would be
pipe into the second command and return a 0 if successful and a 1 if failed. The double pipe function calls on the
single pipe function to pipe command 1 into command 2 and returns once completed. The single pipe function was called
again to pipe command 2, which had just been piped, into command 3. The double pipe function also return a 0 if successful
and a 1 if failed. The triple pipe function called on the double pipe function. The double pipe function called on
the single pipe function. If the double and single pipe functions returned successful, then the single pipe function
was called to pipe command 3, which has already been piped in the double pipe function, and command 4. This was our
general line of thinking.
Novel Utility: We implemented a clear function to clear the screen. This keeps your terminal clutter free.

# ------------------ Project 1 General Information for our Own Use --------------------------------------------------
# Project Description
Design and implement a basic shell interface that supports input/output redirection,
pipes, background processing, and a series of built-in functions. The shell needs
to be robust (e.g. it should not crash under any circumstance beyond machine failure).
Unless otherwise specified, the required features should adhere to the operational
semantics of the bash shell.

# Project Tasks
## Overview
* Implement a basic shell.
* Implementation is distributed into 10 parts.
  1. Parsing
  2. Environmental Variables
  3. Prompt
  4. Shortcut Resolution
  5. $PATH Resolution
  6. Execution
  7. I/O Redirection
  8. Pipes
  9. Background Processing
  10. Built-ins

## Restrictions
* Must be implemented in C
* Only for() and execv() can be used to spawn new processes
* Can not use system() or any of the other exec family of system calls
* Output must match bash except for:
* Built-ins must be created from scratch. (No use of execv())
* No limit on number of characters per instruction

## Allowed Assumptions
* Error messaged do not need to be worded the same as bash, but should convey meaning
* strtok() is permitted but strongly discouraged
* No more than two pipes will appear in a single line
* Do not need to handle globs, regular expressions, special characters, quotes, etc
* Do not need to worry about expanding environmental variables that are arguments to non-built in commands
* There will be no more than 10 aliases
* Pipes and IO redirection will not occur together
* Multiple redirections of the same time will not appear
* No need to implement auto-complete
* No need to handle zombie processes
* No need to support unspecified built-in commands

## Extra Credit
* Support multiple pipes []
* Unique built-ins with proper documentation []
* Shell-ception: can execute your shell from within a running shell process repeatedly

# Bugs Found
1. First bug was in the tok_test.c. Issue was that uninitialized struct instruction can cause the program to crash at realloc. I assumed the compiler would set each value to NULL and 0. Soemtimes it worked, sometimes it didn't. Program fixed when I initialzied the values. 1/17/19
