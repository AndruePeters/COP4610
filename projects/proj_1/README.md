# Project 1 - Group 12

Members: Andrue Peters, Oscar Flores

# Description
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
