/* ex01.c */

#include <stdio.h>
#include <stdlib.h>

#define INITIAL_X  12

/*  gcc -Wall -D DEBUG_MODE ex01.c  */

int main()
{
  int x = INITIAL_X;

  printf( "OpSys\n" );

#if 0
  printf( "OpSys\n" );
  printf( "OpSys\n" );
  printf( "OpSys\n" );
  printf( "OpSys\n" );
  printf( "OpSys\n" );
  printf( "OpSys\n" );
  printf( "OpSys\n" );
  printf( "OpSys\n" );
#endif

#ifdef DEBUG_MODE
  printf( "Debug code goes here\n" );
  printf( "x is %d\n", x );
#endif

  return EXIT_SUCCESS;
}
