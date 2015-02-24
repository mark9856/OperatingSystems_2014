
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

/* use -lpthread or -pthread on the gcc line */
#include <pthread.h>
#define BUFFER_SIZE 1024

void * whattodo( void * arg );

int main(){
    pthread_t tid;
    int rc;

    while ( 1 )
    {
    	int * t;
    	t = (int *)malloc( sizeof( int ) );
    	*t = 3;
    	sleep(1);
        /* handle socket in child process */
        rc = pthread_create(&tid, NULL, whattodo, t);

        if ( rc != 0 )
        {
            perror( "MAIN: Could not create child thread" );
        }        
    }
    return 0;
}

void * whattodo( void * arg ){
    /*  sleep( 1 );  */
    int t = *(int *)arg;
    free( arg );

	printf( "THREAD %u: I'm gonna nap %d for seconds.\n",
		(unsigned int)pthread_self(), t);
	sleep(t);
	printf( "THREAD %u: I'm awake now.\n",
          (unsigned int)pthread_self() );

/* dynamically allocate space to hold a return value */
	unsigned int * x = (unsigned int *)malloc( sizeof( unsigned int ) );

  /* return value is the thread ID */
	*x = pthread_self();
  	pthread_exit( x ); /* terminate the thread, returning
                          x to a pthread_join() call */
  	return NULL;
}