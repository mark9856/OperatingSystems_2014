/* struct.c */

/* a struct is simply a definition of a "data type"; in other
   words, essentially a class definition with no member functions
   and all attributes are public */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct course
{
  char name[80];   /* Operating Systems */
  char * number;   /* CSCI-4210/6140 */
  int crn;         /* 12345 */
  int enrollment;  /* 140 */
};

/*   typedef struct { .... } course;  */

int main()
{
  struct course c1; /* how much memory is allocated here? */

  printf( "sizeof struct course is %d\n", (int)sizeof( struct course ) );
  printf( "sizeof struct course * is %d\n", (int)sizeof( struct course * ) );

  strcpy( c1.name, "Operating Systems" );
  c1.number = (char *)malloc( 30 );
  //sprintf( c1.number, "CSCI-4210/6140" );
/*  strncpy( c1.number, "CSCI-4210/6140", 9 ); */
  strcpy(c1.number, "CSCI-4210/6140");
  c1.crn = 12345;
  c1.enrollment = 140;

  printf( "course 1 is %s\n", c1.number );


  struct course * c2;   /* how many bytes are allocated? */

  c2 = (struct course *)malloc( sizeof( struct course ) );
/*     ^^^^^^^^^^^^^^^^^        ^^^^^^^^^^^^^^^^^^^^^^^
        cast void * to            number of bytes allocated
        struct course *
*/

  strcpy( (*c2).name, "Operating Systems" );

  c2->number = (char *)malloc( 30 );
  int section = 1;
  sprintf( c2->number, "CSCI-4210-%02d", section );

  (*c2).crn = 12345;
  c2->crn = 12345;

  c2->enrollment = 140;

  free( c2->number );
  free( c2 );

  return EXIT_SUCCESS;
}
