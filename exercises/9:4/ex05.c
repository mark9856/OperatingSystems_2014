/* ex05.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


/*
 *  fd            C++    Java           C
 *  0   stdin     cin    System.in      scanf(), read(), getchar()
 *  1   stdout    cout   System.out     printf(), write()
 *  2   stderr    cerr   System.err     fprintf( stderr, "..." ), write()
 *
 *   fd 1 stdout is buffered
 *   fd 2 stderr is NOT buffered
 *
 */

int main( int argc, char *argv[] )
{
  if ( argc != 2 )
  {
    fprintf( stderr, "ERROR: invalid usage\n" );
    fprintf( stderr, "USAGE: %s <filename>\n", argv[0] );
    return EXIT_FAILURE;
  }


  int fd = open( argv[1], O_WRONLY | O_CREAT | O_TRUNC,
                                    0660 );
                                /*  ^
                                  translate at octal

                                     110 110 000
                                     rwx rwx rwx
                                 */

  if ( fd < 0 )
  {
    fprintf( stderr, "ERROR: open() failed\n" );
    perror( "error opening file" );
    return EXIT_FAILURE;
  }

  /* fd table for this process:

     0 stdin
     1 stdout
     2 stderr
     3 argv[1]  (some file open for writing)

   */

  int rc = write( fd, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 20 );
     /* read at most 20 bytes from fd into buffer */

  printf( "fd is %d\n", fd );
  /* good idea to flush the buffer on occasion */
  //fflush( NULL );

  if ( rc < 0 )
  {
    perror( "write() failed" );
    return EXIT_FAILURE;
  }

  printf( "write() returned %d\n", rc );

  close( fd );  /* does this flush the output buffer?! */

  return EXIT_SUCCESS;
}
