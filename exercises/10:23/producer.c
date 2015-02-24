/* producer.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "shared.h"

int main()
{
  int item = 1;   /* next item to be produced */

  int shmid;
  key_t key = 6789;

  /* also see "man shmget" and check out IPC_EXCL */
  shmid = shmget( key, sizeof( struct shared_data ), IPC_CREAT | 0666 );
  printf( "shmget() returned %d\n", shmid );
  if ( shmid < 0 )
  {
    perror( "shmget() failed" );
    return EXIT_FAILURE;
  }

  struct shared_data * shm;
  shm = shmat( shmid, NULL, 0 );   /* attach to shm */
  if ( shm == (struct shared_data *)-1 )
  {
    perror( "shmat() failed" );
    return EXIT_FAILURE;
  }

  shm->in = 0;
  shm->out = 0;
  shm->count = 0;

  while ( item < 10000 )
  {
    while ( shm->count == BUFFER_SIZE )
    {
      printf( "PRODUCER: buffer full...\n" );
      sleep( 1 );
    }

    shm->buffer[ shm->in ] = item;
    item++;
    shm->in = ( shm->in + 1 ) % BUFFER_SIZE;   /* circular array */
    shm->count++;
  }

  /* TO DO: wait until shm->count goes down to zero */


  /* detach from shared memory */
  shmdt( shm );

  /* remove shared memory segment altogether */
  if ( shmctl( shmid, IPC_RMID, 0 ) < 0 )
  {
    perror( "shmctl() failed" );
    return EXIT_FAILURE;
  }

  return 0;
}

