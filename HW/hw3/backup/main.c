/* server.c */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h> // for mkdir
#include <sys/wait.h>

#include <dirent.h> // for stat

#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

/* use -lpthread or -pthread on the gcc line */
#include <pthread.h>
#define BUFFER_SIZE 1024
#define CMDMAX 4


int sock;
char buffer[ BUFFER_SIZE ];
struct sockaddr_in client;
/* socket structures */
struct sockaddr_in server;            
char * cmd[5];
// current pwd
char * crnpwd;
char * newpwd;





/* this function is called by each thread */
void * whattodo( void * arg );
void createDir();
void ToCMD(char * command);
void list(int ns);
void DEL(int ns, char * file_name, unsigned int threadNo);
int RD(int ns, char * file_name, unsigned int threadNo);
// void ADD(int ns, char * command);
int ADD(int ns, char * file_name, int size, char buf[]);
int APPEND(int ns, char * file_name, int size, char buf[]);
int findFile(char * file_name);





int main(int argc, char *argv[]){
    /* Create the listener socket as TCP socket */
    sock = socket( PF_INET, SOCK_STREAM, 0 );

    if ( sock < 0 )
    {
        perror( "socket() failed" );
        exit( EXIT_FAILURE );
    }

    server.sin_family = PF_INET;
    server.sin_addr.s_addr = INADDR_ANY;


    if(argc != 2){
        printf("Please type in the port\n");
        exit( EXIT_FAILURE );
    }
    unsigned short p = atoi(argv[1]);
    if(p > 9000 || p < 8000){
        printf("%d\n", p);
        printf("Please type in a port number bigger than 8000 and lower than 9000\n");
        exit( EXIT_FAILURE);
    }

    createDir();
    unsigned short port = p;

    /* htons() is host-to-network-short for marshalling */
    /* Internet is "big endian"; Intel is "little endian" */
    server.sin_port = htons( port );
    int len = sizeof( server );

    if ( bind( sock, (struct sockaddr *)&server, len ) < 0 )
    {
        perror( "bind() failed" );
        exit( EXIT_FAILURE );
    }

    listen( sock, 1024 );   /* 5 is the max number of waiting clients */
    printf("Started file-server\n");
    printf( "Listening on port %d\n", port );

    pthread_t tid;
    int rc;
    int * newsock;
    // int *t;

    int fromlen = sizeof( client );

    while ( 1 )
    {
        // printf( "PARENT: Blocked on accept()\n" );
        newsock = (int *)malloc( sizeof( int ) );
        *newsock = accept( sock, (struct sockaddr *)&client,
                (socklen_t*)&fromlen );
        // printf( "PARENT: Accepted client connection\n" );
        printf("Received incoming connection from %s\n", 
                inet_ntoa( (struct in_addr)client.sin_addr));

        /* handle socket in child process */
        rc = pthread_create(&tid, NULL, whattodo, newsock);

        if ( rc != 0 )
        {
            perror( "MAIN: Could not create child thread" );
        }        
    }

    close( *newsock );
    close( sock );
    return EXIT_SUCCESS;
}

void * whattodo( void * arg ){
    /*  sleep( 1 );  */
    int newSock = *(int *)arg;
    free( arg );

    int n;

    do{
        // printf( "[THREAD %d]: Blocked on recv()\n", (unsigned int)pthread_self() );

        /* can also use read() and write()..... */
        n = recv( newSock, buffer, BUFFER_SIZE, 0 );


        for (int i = 0; i < 5; ++i){
            /* code */
            cmd[i] = NULL;
        }
        if ( n < 0 ){
            perror( "recv() failed" );
        }
        else if ( n == 0 ){
            // printf( "[THREAD %d]: Rcvd 0 from recv(); closing socket\n",
                    // (unsigned int)pthread_self() );
            printf( "[THREAD %d] Client closed its socket....terminating\n",
                    (unsigned int)pthread_self() );
        }
        else{
            buffer[n] = '\0';  /* assuming text.... */
            // printf( "[THREAD %d]: Rcvd message from %s: %s\n",
            //         (unsigned int)pthread_self(),
            //         inet_ntoa( (struct in_addr)client.sin_addr ),
            //         buffer );
            char *command = (char *)malloc(sizeof(char)*100);
            for(int i = 0; i < n - 2; i ++){
                strncat(command, &buffer[i], 1);
            }
            // char * cmd[CMDMAX];
            // if the command is LIST

            ToCMD(command);
            
            if(strcmp(cmd[0], "LIST") == 0){
                // printf("crnpwd %s\n", crnpwd);
                // printf("newpwd %s\n", newpwd);
                if(cmd[1] == NULL){
                    printf( "[THREAD %d] Rcvd: %s\n",
                       (unsigned int)pthread_self(), "LIST");
                    list(newSock);
                    printf( "[THREAD %d] Sent: ACK\n",
                       (unsigned int)pthread_self());
                }
                else{
                    printf("Don't type in other argument\n");
                }
            }
            else if(strcmp(cmd[0], "DELETE") == 0){
                // n = send(newSock, "DELETE", 6, 0);
                if(cmd[1] != NULL){
                    char * file_name = (char *)malloc(sizeof(char)*100);
                    strcpy(file_name, cmd[1]);
                    printf( "[THREAD %d] Rcvd: %s %s\n",
                       (unsigned int)pthread_self(), "DELETE", file_name);
                    unsigned int threadNo = (unsigned int)pthread_self();
                    DEL(newSock, file_name, threadNo);
                }
                else{
                    printf("Please type the file name\n");
                }
            }
            else if(strcmp(cmd[0], "READ") == 0){
                if(cmd[1] != NULL){
                    // printf("%s\n", cmd[1]);
                    char * file_name = (char *)malloc(sizeof(char)*100);
                    strcpy(file_name, cmd[1]);
                    printf( "[THREAD %d] Rcvd: %s %s\n",
                       (unsigned int)pthread_self(), "READ", file_name);
                    unsigned int threadNo = (unsigned int)pthread_self();
                    RD(newSock, file_name, threadNo);
                }
                else{
                    printf("Please type the file name\n");
                    n = send( newSock, "ACK", 3, 0 );
                }
                strcpy(cmd[0], "READ");
            }
            else if(strcmp(cmd[0], "ADD") == 0){
                if(cmd[1] != NULL){
                    if(cmd[2] != NULL){
                        // printf("%s %s\n", cmd[0], cmd[1]);
                        // n = send( newSock, "\n", 1, 0 );
                        int si = atoi(cmd[2]);
                        char buf[si + 2];
                        char * file_name = (char *)malloc(sizeof(char)*100);
                        strcpy(file_name, cmd[1]);
                        n = recv( newSock, buf, si + 2, 0 );
                        if ( n < 0 ){
                            perror( "recv() failed" );
                        }
                        buf[n] = '\0';
                        printf( "[THREAD %d] Rcvd: %s %s %d\n",
                             (unsigned int)pthread_self(), "ADD", file_name, si);

                        if(ADD(newSock, file_name, si, buf) == 1){
                            printf( "[THREAD %d] Transferred file ( %d bytes)\n",
                             (unsigned int)pthread_self(), si);
                            printf( "[THREAD %d] Sent: ACK\n",
                             (unsigned int)pthread_self());
                        }
                        else{
                            printf( "[THREAD %d] Sent: ERROR FILE EXIST\n",
                             (unsigned int)pthread_self());
                        }
                    }
                    else{
                        printf("Please type in the file size\n");
                    }
                    // ADD(newSock, cmd[1], cmd[2]);                    
                }
                else
                    printf("Please type in the file name\n");
            }
            else if(strcmp(cmd[0], "APPEND") == 0){
                if(cmd[1] != NULL){
                    if(cmd[2] != NULL){
                        // printf("%s %s\n", cmd[0], cmd[1]);
                        // n = send( newSock, "\n", 1, 0 );
                        int si = atoi(cmd[2]);
                        char buf[si + 2];
                        char * file_name = (char *)malloc(sizeof(char)*100);
                        strcpy(file_name, cmd[1]);
                        n = recv( newSock, buf, si + 2, 0 );
                        if ( n < 0 ){
                            perror( "recv() failed" );
                        }
                        buf[n] = '\0';
                        printf( "[THREAD %d] Rcvd: %s %s %d\n",
                             (unsigned int)pthread_self(), "APPEND", file_name, si);

                        if(APPEND(newSock, file_name, si, buf) == 1){
                            printf( "[THREAD %d] Transferred file ( %d bytes)\n",
                             (unsigned int)pthread_self(), si);
                            printf( "[THREAD %d] Sent: ACK\n",
                             (unsigned int)pthread_self());
                        }
                        else{
                            printf( "[THREAD %d] Sent: ERROR NO SUCH FILE\n",
                             (unsigned int)pthread_self());
                        }
                    }
                    else{
                        printf("Please type in the file size\n");
                    }
                }
                else
                    printf("Please type in the file name\n");
            }
            else{
                n = send(newSock, "Please type in correct command\n", 31, 0);
            }


            /* send ack message back to the client */
            if(strcmp(cmd[0], "READ") != 0){
                n = send( newSock, "ACK", 3, 0 );
            }
            fflush( NULL );
            // if ( n != 3 ){
            //     perror( "send() failed" );
            // }
        }
    }
    while ( n > 0 );
    /* this do..while loop exits when the recv() call
       returns 0, indicating the remote/client side has
       closed its socket */

    // printf( "[THREAD %d]: Bye!\n", (unsigned int)pthread_self() );
    close( newSock );
    exit( EXIT_SUCCESS );  /* child terminates here! */

    /* TO DO: add code to handle zombies! */
    /* dynamically allocate space to hold a return value */
    unsigned int * x = (unsigned int *)malloc( sizeof( unsigned int ) );


    *x = *(unsigned int *)(pthread_self());
    pthread_exit( x ); 
    /* terminate the [thread, returning
       x to a pthread_join() call */

    return NULL;
}

void createDir(){
    // crnpwd = (char *)malloc(sizeof(char)*100);
    newpwd = (char *)malloc(sizeof(char)*100);
    crnpwd = getenv("PWD");
    strcpy(newpwd, crnpwd);
    char * dr = (char *)malloc(sizeof(char)*100);
    strcpy(dr, "/storage");
    strcat(newpwd, dr);



    DIR * dir = opendir(".");

    if(dir == NULL){
        perror("opendir() failed");
        return;
    }

    struct dirent * file;
    while((file = readdir(dir)) != NULL){
        // if(file -> d_name == "storage"){
        if(strcmp(file -> d_name, "storage") == 0){
            struct stat buf;
            int rc = lstat(file -> d_name, &buf);
            if(rc < 0){
                perror("lstat() failed");
                return;
            }
            if(S_ISDIR(buf.st_mode)){
                closedir(dir);
                return;
            }
        }
    }
    closedir(dir);

    int status;
    status = mkdir("./storage", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

void ToCMD(char * command){
    char * word;
    char * unused;
    char temp[1024];
    strcpy( temp, command);

    word = strtok_r( temp, " \t", &unused );
            /* after: "ls\0-la &" */
    cmd[0] = word;
    int i = 1;
    while ( word != NULL )
    {
        word = strtok_r( NULL, " \t", &unused );
            /* after: "ls\0-la\0&" */
            /*           ^    ^    */
        cmd[i] = word;
        i ++;
    }
    // for (int i = 0; i < 5; ++i){
    //     printf("%s\n", cmd[i]);
    // }
}

void list(int ns){
    chdir(newpwd);
    DIR * dir = opendir( "." );
    // DIR * dir = opendir( "." );

    int n;

    if ( dir == NULL ){
        perror( "opendir() failed" );
        return;
    }

    struct dirent * file;
    /* ^^^^^^^^^^^^^
       |
       struct dirent is the data type, even tho it's two words */

    int count = 0;

    while( (file = readdir(dir)) != NULL){
        struct stat buf;
        int rc = lstat(file -> d_name, &buf);
        if(rc < 0){
            perror("lstat() failed1");
            return;
        }
        if(S_ISREG( buf.st_mode ) ){
            // if(file -> d_name != ".DS_Store"){
            if(strcmp(file -> d_name, ".DS_Store")){
                count ++;
            }
        }
    }
    char con[20];
    sprintf(con, "%d", count);

    // n = send(ns, con, 20, 0);

    // chdir(crnpwd);
    chdir(newpwd);
    DIR * dir2 = opendir( "." );

    if ( dir2 == NULL ){
        perror( "opendir() failed" );
        return;
    }

    struct dirent * file2;

    n = send(ns, con, strlen(con), 0);
    n = send(ns, "\n", 1, 0);

    // printf("%d\n", count);
    while ( ( file2 = readdir( dir2 ) ) != NULL ){
        // n = send(ns, file2 -> d_name, strlen(file2 -> d_name), 0);
        // n = send(ns, "\n", 2, 0);
        struct stat buf2;
        int rc2 = lstat(file2 -> d_name, &buf2);
        if(rc2 < 0){
            perror("lstat() failed2");
            return;
        }
        if(S_ISREG( buf2.st_mode ) ){
            if(strcmp(file2 -> d_name, ".DS_Store") != 0){
                int n = send(ns, file2 -> d_name, strlen(file2 -> d_name), 0);
                n = send(ns, "\n", 1, 0);
            }
            // printf("%s\n", file2 -> d_name);
        }
    }
    chdir(crnpwd);

    closedir( dir );
    closedir( dir2 );
}

void DEL(int ns, char * file_name, unsigned int threadNo){
    char * temp = (char *)malloc(sizeof(char)*100);
    strcpy(temp, file_name);
    if(findFile(file_name) == 1){
        chdir(newpwd);
        remove(temp);
        printf( "[THREAD %d] Delete %s file\n",
            threadNo, file_name);
        printf( "[THREAD %d] Sent: ACK\n",
            threadNo);
    }
    else {
        // fprintf( stderr, "ERROR: NO SUCH FILE\n" );
        printf( "[THREAD %d] Sent: ERROR NO SUCH FILE\n",
            threadNo);
        int n = send(ns, "ERROR: NO SUCH FILE\n", 20, 0);
    }
}

int RD(int ns, char * file_name, unsigned int threadNo){
    char * temp = (char *)malloc(sizeof(char)*100);
    strcpy(temp, file_name);
    if(findFile(file_name) == 1){
        chdir(newpwd);
        FILE *fp;
        int c;
        fp = fopen(temp,"r");
        int count = 0;

        fseek(fp, 0L, SEEK_END);
        count = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        // printf("%d\n", count);
        char con[20];
        sprintf(con, "%d", count);

        while(1)
        {
          c = fgetc(fp);
          if( feof(fp) )
          { 
            break ;
          }
          // printf("%c", c);
          int n = send(ns, &c, 1, 0);
        }
        // printf("\n");
        int n = send(ns, "\n", 1, 0);
        fclose(fp);
        printf( "[THREAD %d] Sent: ACK %d\n",
            threadNo, count);
        n = send(ns, "ACK ", 4, 0);
        n = send(ns, con, strlen(con), 0);
        printf( "[THREAD %d] Transferred file ( %d bytes)\n",
            threadNo, count);
        return 1;
    }
    else {
        // fprintf( stderr, "ERROR: NO SUCH FILE\n" );
        printf( "[THREAD %d] Sent: ERROR NO SUCH FILE\n",
            threadNo);
        int n = send(ns, "ERROR: NO SUCH FILE\n", 20, 0);
        return 0;
    }
}

// void ADD(int ns, char * file_name){
int ADD(int ns, char * file_name, int size, char buf[]){

    char * temp = (char *)malloc(sizeof(char)*100);
    strcpy(temp, file_name);

    if(findFile(file_name) == 1){
        // fprintf( stderr, "ERROR: FILE EXIST\n" );
        int n = send( ns, "ERROR: FILE EXIST\n", 19, 0 );
        return 0;
    }
    else {
        chdir(newpwd);
        FILE * fp;

        fp = fopen (temp, "w+");
        // fprintf(fp, "%s %s %s %d", "We", "are", "in", 2012);
        // fprintf(fp, "%s\n", "Hello World How are you");
        fprintf(fp, "%s", buf);
   
        fclose(fp);
        return 1;
    }
}

int APPEND(int ns, char * file_name, int size, char buf[]){

    char * temp = (char *)malloc(sizeof(char)*100);
    strcpy(temp, file_name);

    if(findFile(file_name) == 1){
        chdir(newpwd);
        FILE * fp;

        fp = fopen (temp, "a");
        // fprintf(fp, "%s %s %s %d", "We", "are", "in", 2012);
        // fprintf(fp, "%s\n", "Hello World How are you");
        fprintf(fp, "%s", buf);
   
        fclose(fp);
        return 1;

    }
    else {
        // fprintf( stderr, "ERROR: NO SUCH FILE\n" );
        int n = send( ns, "ERROR: NO SUCH FILE\n", 21, 0 );
        return 0;
    }
}

int findFile(char * file_name){
    // printf("%s\n", file_name);
    char * temp = (char *)malloc(sizeof(char)*100);
    strcpy(temp, file_name);
    chdir(newpwd);
    DIR * dir = opendir( "." );

    // printf("%s\n", temp);

    int n;

    if ( dir == NULL ){
        perror( "opendir() failed" );
        return 0;
    }

    struct dirent * file;
    /* ^^^^^^^^^^^^^
       |
       struct dirent is the data type, even tho it's two words */

    while( (file = readdir(dir)) != NULL){
        // printf("Hello\n");

        struct stat buf;
        int rc = lstat(file -> d_name, &buf);
        if(rc < 0){
            perror("lstat() failed");
            return 0;
        }
        // if(file_name == file -> d_name){
        if(strcmp(temp, file -> d_name) == 0){
            return 1;
        }
    }
    return 0;
}


