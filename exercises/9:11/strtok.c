/* strtok.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
  char * word;
  char * unused;
  char * cmd = "ls -la    notes.txt abcd.txt etc.txt &";

  char temp[1024];
  strcpy( temp, cmd );   /* we'll make a copy, because strtok()
                            changes the input */

     /* before: "ls -la &" */
  word = strtok_r( temp, " \t", &unused );
     /* after: "ls\0-la &" */

  while ( word != NULL )
  {
    printf( "[%s] %s\n", temp, word );
    word = strtok_r( NULL, " \t", &unused );
     /* after: "ls\0-la\0&" */
     /*           ^    ^    */
  }

  printf( "[%s]\n", temp );

  return EXIT_SUCCESS;
}
