/* client.c */

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int main(int argc, char** args)
{
	if(argc<3)
	{
		fprintf(stderr, "Failed to start client\n");
		return 1;
	}

	int commandNum=atoi(args[2]);

	//Get info about the server
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if(getaddrinfo("127.0.0.1", args[1], &hints, &res)<0)
	{
		perror("getaddrinfo() failed: ");
		exit(EXIT_FAILURE);
	}
		/* create TCP client socket (endpoint) */
	int clientSocket = socket( res->ai_family,  res->ai_socktype, res->ai_protocol);
	if(clientSocket<0)
	{
		perror("socket() failed: ");
		exit( EXIT_FAILURE );
	}
	// connect!
	if(connect(clientSocket, res->ai_addr, res->ai_addrlen)<0)
	{
		perror("connect() failed: ");
		exit( EXIT_FAILURE );
	}

	//
	//printf( "server address is %s\n", inet_ntoa( ((struct sockaddr_in*)res->ai_addr)->sin_addr ));

	while(1)
	{


		int fd=open("./test_files/img.jpg", O_RDONLY);
		struct stat textBuf;
		fstat(fd, &textBuf);
		int fileSize=(int)textBuf.st_size;
		close(fd);
		char msg[BUFFER_SIZE+fileSize];
		sprintf(msg, "ADD img.jpg %d\n", fileSize);
		FILE* imgPtr=fopen("test_files/img.jpg", "r");
		char fileBuffer[fileSize];
		int i=0;
		while(!feof(imgPtr))
		{
			char byte;
			fread(&byte, 1, 1, imgPtr);
			fileBuffer[i]=byte;
			i++;
		}
		fclose(imgPtr);
		memmove(msg+strlen(msg), fileBuffer, fileSize);
		fprintf(stdout,"%s \t %d bytes\n", msg, i);
		fflush(NULL);
		int totalSize=BUFFER_SIZE+fileSize;
		int n = send( clientSocket, msg, totalSize, 0);

		char* msg;
		if(commandNum==0)
		{
			int fd=open("./test_files/text.txt", O_RDONLY);
			struct stat textBuf;
			fstat(fd, &textBuf);
			int fileSize=(int)textBuf.st_size;
			msg=malloc((fileSize+100)*sizeof(char));
			sprintf(msg, "ADD text.txt %d\n", fileSize);

			char fileBuffer[fileSize+100];
			int bytesRead=read(fd, fileBuffer, fileSize+100);
			close(fd);
			fileBuffer[bytesRead]='\0';
			strcat(msg, fileBuffer);
		}
		else if(commandNum==1)
		{
			msg="APPEND text.txt 24\nI'm Ki";
		}
		else if(commandNum==2)
		{
			msg="READ text.txt\n";
		}
		else if(commandNum==3)
		{
			msg="LIST\n";
		}
		else if(commandNum==4)
		{
			msg="DELETE text.txt\n";
		}

		int n = send( clientSocket, msg, strlen(msg), 0);
		fflush( NULL );
		if ( n < 0 )
		{
			perror( "send() failed" );
			exit( EXIT_FAILURE );
		}
		//
		printf("len of msg: %d\n", strlen( msg ));
		printf("Sent: %s | %d bytes\n", msg, n);
		fflush(NULL);

		char readBuffer[BUFFER_SIZE];
		n = read( clientSocket, readBuffer, BUFFER_SIZE );  // BLOCK
		if ( n < 0 )
		{
			perror( "read() failed" );
			exit( EXIT_FAILURE );
		}
		else
		{
			readBuffer[n] = '\0';
			printf( "Received message from server: %s\n", readBuffer );
		}
		printf("Sleeping...\n");
		fflush(NULL);
		sleep(2);

	}

	shutdown( clientSocket, 2);
	close(clientSocket);

	return EXIT_SUCCESS;
}
