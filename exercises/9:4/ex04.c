/* ex04.c */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


/*
 *  fd            C++    Java           C
 *  0   stdin     cin    System.in      scanf(), read(), getchar()
 *  1   stdout    cout   System.out     printf(), write()
 *  2   stderr    cerr   System.err     fprintf( stderr, "..." ), write()
 *
 *   fd 1 stdout is buffered
 *   fd 2 stderr is NOT buffered
 *
 */

int main( int argc, char *argv[] )
{
/*
    if ( argc != 2 )
    {
        fprintf( stderr, "ERROR: invalid usage\n" );
        fprintf( stderr, "USAGE: %s <filename>\n", argv[0] );
        return EXIT_FAILURE;
    }
    */

    /* carry over from previous example.... */
    char buffer[80];
    sprintf( buffer, "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
    write( 1, buffer, 10 );

    printf( "ABCD" );
    fflush( NULL );
    fprintf( stderr, "ERROR!" );
    printf( "EFGH\n" );

//#if 0
    int fd = open( argv[1], O_RDONLY );

    if ( fd < 0 )
    {
        fprintf( stderr, "ERROR: open() failed\n" );
        perror( "error opening file" );
        return EXIT_FAILURE;
    }

    /* fd table for this process:

       0 stdin
       1 stdout
       2 stderr
       3 argv[1]  (some file open for reading)

*/
    /* try changing the 25 below to 300 */
    int rc = read( fd, buffer, 25 );
    /* read at most 25 bytes from fd into buffer */

    if ( rc < 0 )
    {
        perror( "read() failed" );
        return EXIT_FAILURE;
    }

    buffer[rc] = '\0';

    printf( "read() returned %d and read %s\n", rc, buffer );

    close( fd );

//#endif
    /* to do: add a loop to keep calling read() until the entire
       file contents have been read and displayed */

    /* to do: extend this program to write the data read from the
       file into a new file specified as argv[2]; further,
       remove all SPACE characters and '\n' characters */

    /* show line numbers when displaying the file */

    return EXIT_SUCCESS;
}
