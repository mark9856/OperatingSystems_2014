#include <stdio.h>
#include <unistd.h>

int main()
{
  int rc;
  printf( "ONE\n" );
  rc = fork();
  printf( "TWO\n" );
  if ( rc == 0 ) { printf( "THREE\n" ); }
  if ( rc > 0 ) { printf( "FOUR\n" ); }
  return 0;
}
