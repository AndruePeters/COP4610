/*
pipes declaration
used TA pipe example as basic format
attempting to have upto 3 pipes
*/

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <utility/path.h>
#include <sys/wait.h>

//simple single pipe
int pipe2cmd(char ** cmd1, char ** cmd2)
{
  int fd[2];
  int state;

  pid_t p = fork();
  pipe(fd);

  //checking piping
  if(pipe(fd) < 0)
  {
    fprintf(stderr, "\nFailed Pipe\n");
    return 1; //fail
  }

  //checking forking
  if(p < 0)
  {
    fprintf(stderr, "\nFailed Fork\n");
    return 1; //fail}
  }
  else if (p > 0)
  { //parent process
    pid_t p1 = fork();
    //assuming that if the piping and forking for p worked, then will
    //work for p1
    if(p1 != 0)
    {
      close(fd[0]);
      close(fd[1]);
      waitpid(p, &state, 0);
      waitpid(p, &state, 0);
    }
    else if(p1 == 0) //child processes of 2nd forking
    { //error
      close(fd[0]);
      close(fd[1]);
      printf("\nError with Second Command\n");

      //used to see details of the Error
      execv(cmd2[0], cmd2);
      perror(cmd2[0]);
      return 1;   //fail
    }
  }
  else if(p == 0) //child process of the first forking
  {
      close(fd[0]);
      close(fd[1]);
      printf("\nError with First Command\n");

      //used to see details of Error
      execv(cmd1[0], cmd1);
      perror(cmd1[0]);

      return 1; //fail
  }

  return 0; //no fail
}

int pipe3cmd(char ** cmd1, char ** cmd2, char ** cmd3)
{
  int x = pipe2cmd(cmd1, cmd2);
//if pipe2cmd returns 1, then failure so return 1.
//otherwise piping was success so return 0
  if(x==0) //piping successful
  {
    int y = pipe2cmd(cmd2, cmd3);

    if(y==0)  //pipe2cmd ran successfully
    {
      return 0;
    }
    else
    {
      printf("\nError with Second Piping of cmd2 to cmd3\n");
      return 1;
    }
  }
  else
  {
    printf("\nError with first Piping from cmd1 to cmd2\n");
    return 1;
  }
}

/*
int pipe4cmd(char ** cmd1, char ** cmd2, char ** cmd3, char ** cmd4)
{
  int x = pipe3cmd(cmd1, cmd2, cmd3);

  if(x==0) //pipe3cmd ran successful
  {
    int y = pipe2cmd(cmd3, cmd4);

    if(y==0)  //pipe2cmd ran successfully
    {
      return 0;
    }
    else
    {
      printf("\nError with pipe2cmd\n");
      return 1;
    }
  }
  else
  {
    printf("\nError with pipe3cmd\n");
    return 1;
  }
}
*/
