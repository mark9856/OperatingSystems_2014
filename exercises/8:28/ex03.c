/* ex03.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

int main()
{
  char buffer[80];
  sprintf( buffer, "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
  write( 1, buffer, 10 );

  printf( "ABCD" );
  fprintf( stderr, "ERROR!" );
  printf( "EFGH\n" );

  return EXIT_SUCCESS;
}
