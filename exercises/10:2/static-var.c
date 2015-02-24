/* static-var.c */

#include <stdio.h>
#include <stdlib.h>

void function()
{
  /* the state variable below is initialized to 1
     on the FIRST call to this function; thereafter,
     its value will persist */
  static int state = 1;
  printf( "state is %d\n", state );
  state *= 2;
}

int main()
{
  int i;
  for ( i = 0 ; i < 20 ; i++ )
  {
    function();
  }

  return EXIT_SUCCESS;
}
