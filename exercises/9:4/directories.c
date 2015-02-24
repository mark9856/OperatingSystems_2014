/* directories.c */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main()
{
  /* open the current working directory "." */
  DIR * dir = opendir( "." );

  if ( dir == NULL )
  {
    perror( "opendir() failed" );
    return EXIT_FAILURE;
  }

   struct dirent * file;
/* ^^^^^^^^^^^^^
    |
  struct dirent is the data type, even tho it's two words */

  while ( ( file = readdir( dir ) ) != NULL )
  {
    printf( "found %s", file->d_name );
    printf( " (d_type is %d) ", file->d_type );
    
    struct stat buf;
    int rc = lstat( file->d_name, &buf );

    if ( rc < 0 )
    {
      perror( "lstat() failed" );
      return EXIT_FAILURE;
    }

    printf( " (%d bytes)", buf.st_size );

    if ( S_ISREG( buf.st_mode ) )
    {
      printf( " --> regular file\n" );
    }
    else if ( S_ISDIR( buf.st_mode ) )
    {
      printf( " --> directory\n" );
    }
    else
    {
      printf( " --> ????\n" );
    }
  }

  closedir( dir );

  return EXIT_SUCCESS;
}
