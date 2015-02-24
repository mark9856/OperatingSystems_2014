/* getenv.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
  char * home = getenv( "HOME" );

  if ( home == NULL )
  {
    fprintf( stderr, "$HOME is not set!\n" );
    return EXIT_FAILURE;
  }

  printf( "$HOME is %s\n", home );

  char copy_of_home[100];  /* BIG assumption that len < 100 */
  strcpy( copy_of_home, home );
  printf( "$HOME copy is %s\n", copy_of_home );

  return EXIT_SUCCESS;
}
