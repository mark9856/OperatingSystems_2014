/* pipe-with-redirection.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  int p[2];  /* array to hold the two pipe file descriptors
                (p[0] is the read end; p[1] is the write end) */

  int rc = pipe( p );

  if ( rc < 0 )
  {
    perror( "pipe() failed" );
    return EXIT_FAILURE;
  }


  int pid = fork();  /* this will also copy fd table to child */

  /* fd tables (after parent closes p[1] and child closes p[0]):
                 ...and after dup2() call
       PARENT                     CHILD
    0: stdin                      stdin
    1: stdout      write========< p[1]
    2: stderr                     stderr
    3: p[0] <=======read
    4:             write========< p[1]
   */

  if ( pid == 0 ) /* child */
  {
    close( p[0] );  /* ensure no reads are done in the child */

    int bytes_written = write( p[1], "ABCDEFGHIJKLMNOPQRS", 10 );
/* -- context switch could occur here, causing PARENT
          output line to appear before the CHILD output line -- */
    printf( "CHILD: Wrote %d bytes\n", bytes_written );

    int rc = dup2( p[1], 1 );
     /* closes fd 1; redirect stdout (fd 1) to p[1] */
    printf( "this should go to the pipe now...\n" );
  }
  else /* parent */
  {
    close( p[1] );  /* ensure no writes are done in the parent */

    char buffer[100];
    int bytes_read = read( p[0], buffer, 100 );  /* BLOCKING */
    buffer[bytes_read] = '\0';
    printf( "PARENT: Read %d bytes: %s\n", bytes_read, buffer );

    bytes_read = read( p[0], buffer, 100 );  /* BLOCKING */
    buffer[bytes_read] = '\0';
    printf( "PARENT: Read %d bytes: %s\n", bytes_read, buffer );
  }

  return EXIT_SUCCESS;
}
