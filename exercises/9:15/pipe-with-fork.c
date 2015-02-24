/* pipe-with-fork.c */

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

       PARENT                     CHILD
    0: stdin                      stdin
    1: stdout                     stdout
    2: stderr                     stderr
    3: p[0] <=======read
    4:             write========< p[1]
   */

  if ( pid == 0 ) /* child */
  {
    close( p[0] );  /* ensure no reads are done in the child */

    printf("Yeah\n");
    int bytes_written = write( p[1], "ABCDEFGHIJKLMNOPQRS", 10 );
/* -- context switch could occur here, causing PARENT
          output line to appear before the CHILD output line -- */
    printf( "CHILD: Wrote %d bytes\n", bytes_written );

#if 0
    char buffer[10];
    int bytes_read = read( p[0], buffer, 6 );  /* BLOCKING */
    buffer[bytes_read] = '\0';
    printf( "CHILD: Read %d bytes: %s\n", bytes_read, buffer );
#endif
  }
  else /* parent */
  {
    close( p[1] );  /* ensure no writes are done in the parent */

    printf("Hello\n");
    char buffer[10];
    int bytes_read = read( p[0], buffer, 6 );  /* BLOCKING */
    /* -Ma
    Blocking means waiting until this line is excuted
    NoBlocking means don't wait
     */
    buffer[bytes_read] = '\0';
    printf( "PARENT: Read %d bytes: %s\n", bytes_read, buffer );
  }

  return EXIT_SUCCESS;
}
