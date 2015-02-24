/* alarm-v3.c */

/* multiple threads */

/* gcc -Wall alarm-v3.c -pthread */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>

#define MAX_LINE 80

/* define a structure, because the thread
   function only takes 1 argument */
typedef struct alarm_info
{
  int seconds;
  char msg[MAX_LINE];
} alarm_t;

/* function executed by each thread */
void * alarm_thread( void * arg )
{
  alarm_t * alarm = (alarm_t *)arg;

  int rc = pthread_detach( pthread_self() );

  if ( rc != 0 )
  {
    /* pthreads functions do NOT use errno or perror() */
    fprintf( stderr, "pthread_detach() failed (%d): %s",
             rc, strerror( rc ) );

    /* end the entire process? */
    exit( EXIT_FAILURE );
  }

  printf( "<alarm set for %d seconds>\n", alarm->seconds );
  sleep( alarm->seconds );
  printf( "ALARM (%d): %s\n", alarm->seconds, alarm->msg );

  free( alarm );

  return NULL;
}

int main()
{
  char line[MAX_LINE];

  while ( 1 )
  {
    printf( "Set alarm (<sec> <msg>): " );

    if ( fgets( line, MAX_LINE, stdin ) == NULL )
    {
      return EXIT_FAILURE;
    }

    /* skip blank lines */
    if ( strlen( line ) <= 1 )  continue;

    alarm_t * alarm = (alarm_t *)malloc( sizeof( alarm_t ) );
      /* TO DO: make sure malloc() succeeded */

    if ( sscanf( line, "%d %[^\n]", &alarm->seconds, alarm->msg ) < 2
          || alarm->seconds < 0 )
    {
      fprintf( stderr, "ERROR: invalid alarm request\n" );
      free( alarm );
    }
    else
    {
      pthread_t thread;  /* similar to: pid_t pid; */

      int rc = pthread_create( &thread, NULL, alarm_thread, alarm );

      if ( rc != 0 )
      {
        /* pthreads functions do NOT use errno or perror() */
        fprintf( stderr, "pthread_create() failed (%d): %s",
                 rc, strerror( rc ) );
        return EXIT_FAILURE;
      }

      usleep( 10000 );
    }
  }

  return EXIT_SUCCESS;
}

