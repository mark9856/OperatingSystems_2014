/* pipe.c */

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

  /* fd table:

    0: stdin
    1: stdout
    2: stderr
    3: p[0] <=====read========
    4: p[1] ======write=======>
   */

  int bytes_written = write( p[1], "ABCDEFGHIJKLMNOPQRSTUV", 12 );

  printf( "Wrote %d bytes\n", bytes_written );

  char buffer[10];
  int bytes_read = read( p[0], buffer, 6 );  /* BLOCKING */
  /*
   * read and wait are all blocking
   */
  buffer[bytes_read] = '\0';
  printf( "Read %d bytes: %s\n", bytes_read, buffer );

  bytes_read = read( p[0], buffer, 6 );  /* BLOCKING */
  buffer[bytes_read] = '\0';
  printf( "Read %d bytes: %s\n", bytes_read, buffer );

  return EXIT_SUCCESS;
}
