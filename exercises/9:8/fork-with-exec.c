/* fork-with-exec.c */

/* create a child process */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
  pid_t pid;    /* process id (int) */

  pid = fork();   /* create a child process */

  if ( pid < 0 )
  {
    perror( "fork() failed!" );
    return EXIT_FAILURE;
  }

  /* when a child process is created, the child is an exact */
  /*  duplicate of the parent's memory.... */

  if ( pid == 0 )   /* child process */
  {
    printf( "CHILD: happy birthday!\n" );
    sleep( 5 );

                    /* path,   argv[0], argv[1], ... */
    int rc = execlp( "/bin/ls", "ls",    "-l",   NULL );

    printf( "We will never see this output (unless exec() failed)\n" );

    /* if the execlp() system call works, then we'll never
       get beyond this point in the code */

    perror( "execlp() failed!" );

    return EXIT_FAILURE;
  }
  else /* pid > 0 so we're in the parent (calling) process */
  {
    printf( "PARENT: my child's pid is %d\n", pid );

//#if 0
    /* wait for the child to terminate */
    int status;
    int child_pid = wait( &status );

    printf( "PARENT: child %d terminated...", child_pid );

    if ( WIFSIGNALED( status ) )  /* core dump or kill or kill -9 */
    {
      printf( "abnormally\n" );
    }
    else if ( WIFEXITED( status ) ) /* child called return or exit() */
    {
      int rc = WEXITSTATUS( status );
      printf( "successfully with exit status %d\n", rc );
    }
//#endif

    printf( "PARENT: bye\n" );
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
