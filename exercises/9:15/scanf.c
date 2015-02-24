/* scanf.c */

#include <stdio.h>
#include <stdlib.h>

int main()
{
  char name[1000];

  printf( "What's your name? " );
  scanf( "%s", name );
  printf( "name is %s\n", name );
  scanf( "%[^\n]", name ); /* read the entire line (up to but not
                              including the '\n' at then end) */
  getchar();
  printf( "name is %s\n", name );

  scanf( "%[^\n]", name ); /* read the entire line (up to but not
                              including the '\n' at then end) */
  printf( "name is %s\n", name );


  /* could also use fgets() as follows:
   *
   * char *fgets(char *s, int size, FILE *stream);
   *
   *    fgets( name, 1000, stdin );
   *
   */

  return EXIT_SUCCESS;
}
