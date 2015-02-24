/* copy.c */

#include <stdio.h>
#include <stdlib.h>

int main()
{
  int c;

/* to do: write code to implement the word count (wc) program */

  for ( ; ( c = getchar() ) != EOF ; putchar( c ) )
    ;

#if 0
  while ( ( c = getchar() ) != EOF )
  {
    putchar( c );
  }
#endif

#if 0
  c = getchar();
  while ( c != EOF )
  {
    putchar( c );
    c = getchar();
  }
#endif

  return EXIT_SUCCESS;
}
