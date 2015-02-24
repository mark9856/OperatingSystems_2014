/* octuplets-dontcare.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <signal.h>

/* function prototypes */
int child();

/* in main(), use the signal() system call to set the
 * kill_zombies() function up as the signal handler for
 * the SIGCHLD signal */
void * kill_zombies( int n )  /* n here is the signal received */
{
  int status;
  printf( "PARENT: Caught SIGCHLD (%d).\n", n );
  waitpid( 0, &status, WNOHANG );
  return (void *)NULL;
}


int main()
{
  /* create all the child processes */

  int i, children = 8;  /* based on a true story.... */

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
  }

  /* signal( SIGINT, SIG_IGN );  ignore the SIGINT CTRL-C */
  /* signal( SIGCHLD, SIG_DFL );  restore default handler */

  signal( SIGCHLD, (void *)kill_zombies );
  printf( "PARENT: I don't care. I'm taking a nap, so go away!\n" );

  int t = 45;
  do
  {
    t = sleep( t ); /* the sleep() system call is not a re-entrant
                       function, so when it's interrupted
                       (by a signal),
                       we might want to call it again.... */
    printf( "PARENT: t is %d\n", t);
    printf( "PARENT: Argh! Don't interrupt me. Going back to sleep\n" );
  }
  while ( t > 0 );

  printf( "PARENT: Bye.\n" );

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
  return t;
}
