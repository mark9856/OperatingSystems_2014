/* octuplets-impatient.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

/* function prototypes */
int child();
void parent( int children, int childpids[] );

int main()
{
  /* create all the child processes */

  int i, children = 8;  /* based on a true story.... */
  int childpids[8];

  for ( i = 0 ; i < children ; i++ )
  {
    pid_t pid = fork();

    if ( pid < 0 )
    {
      perror( "fork() failed" );
      return EXIT_FAILURE;
    }

    if ( pid == 0 )
    {
      int rc = child();
      exit( rc );   /* return rc; */
    }

    childpids[i] = pid;  /* store each child pid in an array */
  }

  parent( children, childpids );

  return EXIT_SUCCESS;
}



/* each child process sleeps for t seconds and returns t
  when it wakes up */
int child()
{
  int t;
  printf( "CHILD: I'm new. My pid is %d.\n", getpid() );

  /* seed the pseudo-random number generator */
  srand( time( NULL ) * getpid() * getpid() );   /* use the current timestamp */

  t = 10 + ( rand() % 21 );   /* [10,30] */
  printf( "CHILD: I'm gonna nap for %d seconds.\n", t );
  sleep( t );
  //printf( "CHILD: I'm Back\n");
  return t;
}


void parent( int children, int childpids[] )
{
  int status;  /* return status from each child process */
  pid_t child_pid;

#if 0
  printf( "PARENT: I'm gonna nap for 20 seconds.\n" );
  sleep( 20 );
#endif

  printf( "PARENT: I'm waiting for my children to wake up.\n" );

  while ( children > 0 )
  {
    /* check to see if a child process has terminated
       (without BLOCKING!) */
    child_pid = waitpid( 0, &status, WNOHANG );

    if ( child_pid == 0 )
    {
      if ( children > 4 )
      {
        printf( "PARENT: No children woke up.\n" );
        sleep( 3 );
      }
      else
      {
        printf( "PARENT: I'm tired of waiting!\n" );
        int j;
        for ( j = 0 ; j < 8 ; j++ )
        {
          /* send a SIGINT signal to a process via kill() */
          //kill( childpids[j], SIGINT );  /* see man 7 signal */
          kill( childpids[j], SIGKILL );  /* see man 7 signal */
                          /* try SIGKILL here */

          /* we could try using 0 (or -1) as the first arg
              and avoid keeping track of childpids[] */

          /* TO DO: capture the return value of kill() */

          /* TO DO: manage child processes better by keeping track
                     of which ones are terminated etc. */
        }

        /* give child processes a chance to terminate */
        sleep( 1 );
      }
    }
    else  /* child_pid > 0 */
    {
      children--;

      printf( "PARENT: child %d terminated...", child_pid );
      if ( WIFSIGNALED( status ) )
      {
        printf( "abnormally\n" );  /* core dump or kill or kill -9 */
      }
      else if ( WIFEXITED( status ) )
      {
        int rc = WEXITSTATUS( status );
        printf( "successfully with exit status %d\n", rc );
      }
    }

    printf( "PARENT: %d children to go....\n", children );
  }

  printf( "PARENT: All of my children are awake!\n" );

}
