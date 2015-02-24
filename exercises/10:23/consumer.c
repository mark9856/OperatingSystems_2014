/* consumer.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "shared.h"


int main()
{
  int shmid;
  key_t key = 6789;

  /* also see "man shmget" and check out IPC_EXCL */
  shmid = shmget( key, sizeof( struct shared_data ), 0666 );
  if ( shmid < 0 )
  {
    perror( "shmget() failed" );
    return EXIT_FAILURE;
  }

  printf( "Found the shared memory segment with id %d\n", shmid );

  struct shared_data * shm;
  shm = shmat( shmid, NULL, 0 );
  if ( shm == (struct shared_data *)-1 )
  {
    perror( "shmat() failed" );
    return EXIT_FAILURE;
  }

  while ( 1 )
  {
    int next_consumed;
    while ( shm->count == 0 )
    {
      printf( "CONSUMER: buffer empty...\n" );
      sleep( 1 ); 
    }

 /*** CRITICAL SECTION ***/
    next_consumed = shm->buffer[ shm->out ];
    shm->out = ( shm->out + 1 ) % BUFFER_SIZE;  /* circular array */
    shm->count--;
 /*** CRITICAL SECTION ***/

    printf( "CONSUMER: consumed %d\n", next_consumed );
  }


  /* detach from shared memory */
  shmdt( shm );


  return 0;
}

