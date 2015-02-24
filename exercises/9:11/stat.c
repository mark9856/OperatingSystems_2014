/* stat.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

 /* check out "man 2 stat" for details */

int main()
{
  struct stat buf;

  /* user enters:  ls

     MYPATH has /bin

     string:  "/bin/ls" this goes into lstat() */

  int rc = lstat( "/bin/ls", &buf );
/*  int rc = lstat( "/cs/goldsd/OS-Fall2014/stat.c", &buf ); */

  printf( "%d\n", rc );

  printf( "st_mode is %d\n", (int)buf.st_mode );
  printf( "st_mode in octal is %o\n", (int)buf.st_mode );

                 /* 100755 */
                 /*    111101101 */
                 /*    rwxr-xr-x */

  if ( S_ISREG( buf.st_mode ) )
  {
    printf( "This is a regular file\n" );

    if ( buf.st_mode & ( S_IXUSR | S_IXGRP | S_IXOTH ) )
    {
      printf( "It's executable!\n" );
    }
  }

  return EXIT_SUCCESS;
}
