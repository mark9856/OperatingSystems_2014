/* fork.c */

/* create a child process */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
  int x = 5;
  pid_t pid;    /* process id (int) */

  pid = fork();   /* create a child process */

  /* this fork() system call, if successful, will
     return twice.....once in the parent process,
      once in the child process */

  if ( pid < 0 )
  {
    perror( "fork() failed!" );
    return EXIT_FAILURE;
  }

/*  int x = 5; */

  /* when a child process is created, the child is an exact */
  /*  duplicate of the parent's memory.... */

  if ( pid == 0 )   /* child process */
  {
    printf( "CHILD: happy birthday!\n" );
    x += 10;
    printf( "CHILD: x is %d\n", x );
    printf( "CHILD: bye\n" );
    sleep( 5 ); 
    return 12;
  }
  else /* pid > 0 so we're in the parent (calling) process */
  {
    printf( "PARENT: my child's pid is %d\n", pid );
    x += 30;
    printf( "PARENT: x is %d\n", x );
//#if 0
    /* wait for the child to terminate */
    int status;
    pid_t child_pid = wait( &status );
                   /* wait() blocks indefinitely ... */

    printf( "PARENT: child %d terminated...", (int)child_pid );

    if ( WIFSIGNALED( status ) )  // core dump or kill or kill -9 
    {
      printf( "abnormally\n" );
    }
    else if ( WIFEXITED( status ) ) // child called return or exit() 
    {
      int rc = WEXITSTATUS( status );
      printf( "successfully with exit status %d\n", rc );
    }

    printf( "PARENT: bye\n" );
//#endif
  }


  /* things that get duplicated:
     -- program counter (PC)
     -- variables and their values (anything in memory)
     -- file descriptors
     -- environment (env variables)
     -- process priority
     -- controlling terminal
     -- current working directory
     -- signal handlers ???
     -- etc.

     things that do NOT get duplicated:
     -- process id (pid)
     -- parent process id
     -- list of child processes (NULL)
     -- data on resource allocation (CPU usage, total runtime)
     -- memory location / locks
     -- pending signals
   */
  

  return 0;
}
