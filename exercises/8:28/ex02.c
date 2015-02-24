/* ex02.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char *argv[] )
{
  printf( "argc is %d\n", argc );
  printf( "argv[0] is %s\n", argv[0] );
  printf( "argv[1] is %s\n", argv[1] );
  printf( "argv[2] is %s\n", argv[2] );
  /*
      %d   decimal int (int)
      %lf  long float  (double)
      %s   string      (char * or char[])
      %f   float       (float)
      %c   char        (char)
   */

  printf( "sizeof( int * ) is %d\n", (int)sizeof( int * ) );

  char name[80];  /* static memory allocation --> stack */
  sprintf( name, "ABCD %s", argv[1] );
  printf( "name is %s\n", name );

  strcpy( name, "XYZ" );
  printf( "name is %s\n", name );

  /* strncpy()  strlen()  strtok()  */

  int x;   /* carves out 4 bytes in the main() activation record */

  printf( "sizeof( int ) is %d\n", (int)sizeof( int ) );


  int * y;  /* carves out 8 bytes */
  y = &x;   /* y points to x */

  y = malloc( sizeof( int ) );  /* dynamic mem allocation --> heap */
     /*       ^^^^^^^^^^^^^
               allocate 4 bytes of memory from the heap */

  *y = 100;  /* the * operator is the dereference operator */

 // printf("sizeof y is %d\n", (int)sizeof(y));
 // printf("sizeof * y is %d\n", (int)sizeof(*y));
  free( y );


  int q = 47;

  /* allocate array of int of size 47 */
  y = (int *)malloc( q * sizeof( int ) );

  //printf("sizeof y is %d\n", (int)sizeof(y));
  //printf("sizeof * y is %d\n", (int)sizeof(*y));
  if ( y == NULL )
  {
    fprintf( stderr, "ERROR: could not allocate memory\n" );
    return EXIT_FAILURE;
  }

  free( y );


  y = (int *)calloc( 47, sizeof( int ) );  /* <== preferred way */
  //printf("sizeof y is %d\n", (int)sizeof(y));
  //printf("sizeof * y is %d\n", (int)sizeof(*y));

  *(y+5) = 12;   /* y[5] = 12 */


/*  *( y + (5*sizeof(int)) ) = 12 */

  free( y );

  return EXIT_SUCCESS;
}
