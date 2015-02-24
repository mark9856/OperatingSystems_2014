/* wc.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define IN_A_WORD  1 /* inside a word */
#define NOT_IN_A_WORD 0 /* outside a word */

/* count lines, words, and characters in input */

int main()
{
sleep( 15 );
  int c, nl, nw, nc, state;
  state = NOT_IN_A_WORD;
  nl = nw = nc = 0;

  while ( ( c = getchar() ) != EOF )
  {
    nc++;

    if ( c == '\n' ) nl++;

    if ( c == ' ' || c == '\n' || c == '\t' )
    {
      state = NOT_IN_A_WORD;
      printf( "DEBUG: blahblablah\n" );
    }
    else if ( state == NOT_IN_A_WORD )
    {
      state = IN_A_WORD;
      nw++;
    }
  }

  printf( "%d %d %d\n", nl, nw, nc );

  return EXIT_SUCCESS;
}

