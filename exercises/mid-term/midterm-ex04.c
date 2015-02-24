#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main()
{
  int fd;
  close( 2 );
#if 0
  close( 1 );  /* <== add this line later (part b) */
#endif
  printf( "HI\n" );
  fflush( stdout );
  fd = open( "newfile.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600 );

  printf( "%d\n", fd );
  printf( "WHAT?\n" );
  fprintf( stderr, "ERROR\n" );

  fflush( stdout );

  int rc = fork();
  if ( rc == 0 )
  {
    printf( "AGAIN?\n" );
    fprintf( stderr, "ERROR ERROR\n" );
  }
  else
  {
    wait( NULL );
  }

  printf( "BYE\n" );
  fprintf( stderr, "HELLO\n" );
  return 0;
}
