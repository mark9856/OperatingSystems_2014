#include <stdio.h>
#include <unistd.h>

int main()
{
  printf( "ONE\n" );
  fprintf( stderr, "ERROR: ONE\n" );
  int rc = close( 1 );

  printf( "TWO\n" );
  fprintf( stderr, "ERROR: TWO\n" );
  rc = dup2( 2, 1 );

  printf( "THREE\n" );
  fprintf( stderr, "ERROR: THREE\n" );
  rc = close( 2 );

  printf( "FOUR\n" );
  fprintf( stderr, "ERROR: FOUR\n" );
  return 0;
}

