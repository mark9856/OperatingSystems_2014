/* server.c */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h> // for mkdir

#include <dirent.h> // for stat

#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

/* use -lpthread or -pthread on the gcc line */
#include <pthread.h>
#define BUFFER_SIZE 1024

void ToCMD(char *cmd[], char * command);

int main(int argc, char *argv[]){
	char buffer[1024];
	strcpy(buffer, "test Hello You\0");
	int n = strcmp(buffer, "test\0");
	printf("%lu\n", sizeof(buffer));
	// printf("%s\n", buffer);

	char *command = (char *)malloc(sizeof(char)*100);
	for(int i = 0; i < sizeof(buffer) - 1; i ++){
		strncat(command, &buffer[i], 1);
	}
	printf("%s\n", command);
	// printf("%s\n", command);
	// char * cmd[3];
	// ToCMD(cmd, command);
	// printf("%s\n", command);


}


// Convert command into cmd
void ToCMD(char *cmd[], char * command){
    int i = 0;
    char tmp[100];
    bzero(tmp, 100);
    while(*command != '\0'){
        if(*command == ' '){
            if(cmd[i] == NULL){
                cmd[i] = (char *)malloc(sizeof(char)*strlen(tmp) + 1);
            }
            strcpy(cmd[i], tmp);
            bzero(tmp, 100);
            i ++;
        }
        else{
            strncat(tmp, command, 1);
        }
        command ++;
    }
    cmd[i] = (char *)malloc(sizeof(char)*strlen(tmp) + 1);
    strcpy(cmd[i], tmp);
    // cmd[i+1] = 0;
    // argc = i;
}