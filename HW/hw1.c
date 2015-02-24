#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>



// Set up MYPATH
void setPath(char mypath[]);
char ** readIn();
char * findPath(char mypath[], char * command);
void clearCmd(char ** cmd);
void terminate();


int main(){
    // Define MYPATH
    char * mypath = (char *)malloc(sizeof(char)* 1000);

    char ** cmd;
    

    // Set up MYPATH
    setPath(mypath);
    while(1){
        cmd = readIn();
        if(strlen(cmd[0]) != 0){
            if(strcmp(cmd[0], "quit") == 0 || strcmp(cmd[0], "exit") == 0){
                terminate();
                break;
            }
            char * pathFound = (char*)malloc(sizeof(char)*1000);
            pathFound = findPath(mypath, cmd[0]);
            printf("%s\n", pathFound);
            if(strcmp(pathFound, "Error") == 0){
                printf("ERROR: command '%s' not found\n", cmd[0]);
            }
            else{
                //execvp(pathFound+cmd[0], cmd);
                //char * cm = (char*)malloc(sizeof(char)*10);
                //cm = "ls";
                strcat(pathFound,"/");
                strcat(pathFound,cmd[0]);
                printf("%s %s\n",pathFound,cmd[0]);
                int rc = execv(pathFound, cmd);
                printf("%d Done\n",rc);


            }
        }
        for(int i = 0; i < 10; i ++){
            printf("Command %d is %s\n", i, cmd[i]);
        }
    }

    


    return EXIT_SUCCESS;
}



void setPath(char mypath[]){
    char * path = getenv( "PATH" );

    if(path == NULL){
        strcpy(path, "/Users/huiyuma/");
    }

    strcpy( mypath, path);
}

char ** readIn(){
    char ** cmd;
    cmd = (char **) malloc(10 * sizeof(char *));
    for(int i = 0; i < 10; i++){
        cmd[i] = (char *) malloc(10 * sizeof(char));
    }
    int i = 0;
    while(1){
        char c = getchar();
        if(c != '\n'){
            if(c == ' '){
                if(strlen(cmd[i]) != 0){
                    char * end = "\0";
                    strncat(cmd[i], end, 1);
                    i ++;
                }
            }
            else{
                strncat(cmd[i], &c, 1);
            }
        }
        else{
            if(strlen(cmd[i]) == 0){
                char * end = "\0";
                strncat(cmd[i], end, 1);
            }
            return cmd;
        }
    }
    return cmd;
}

char * findPath(char mypath[], char * command){
    char * pathFound = (char*)malloc(sizeof(char)*1000);
    //char * path = getenv( "PATH" );
    char * path = (char*)malloc(sizeof(char)*1000);
    strcpy(path,mypath);
    struct dirent * file;
    /*
    for(pathFound = strtok(path, ":"); pathFound; pathFound = strtok(NULL, ":")){
        DIR * dir = opendir(pathFound);
        while((file = readdir(dir)) != NULL){
            if(strcmp(file -> d_name, command) == 0){
                return pathFound;
            }
        }
    }
    */
    pathFound = strtok(path, ":");
    while (pathFound != NULL) {
        DIR* dir =opendir(pathFound);
        while((file = readdir(dir)) != NULL){
            if(strcmp(file -> d_name, command) == 0){
                return pathFound;
            }
        }
        pathFound = strtok(NULL, ":");
    }

    return "Error";
}

void clearCmd(char ** cmd){
    for(int i = 0; i < 10; i ++){
        bzero(cmd[i], 10);
    }
    return;
}

void terminate(){
    printf("bye\n");
    fflush(stdout);
}
