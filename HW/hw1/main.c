#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

typedef int bool;
#define true 1
#define false 0
#define HISTORYLINEMAX 999
#define CMDMAX 4

// Store single path
char * mypaths[10]; 
// A combination of mypath
char * mypath;
// current pwd
char * crnpwd;
// total cmd argc
int argc;
// cmd 
char * cmd[CMDMAX];
// second cmd for pipe use
char * cmd1[CMDMAX];
// History of command
char * History[HISTORYLINEMAX];
// Store current History total
int crtHistory;



// Set up MYPATH
void setPath();
// Set up pwd
void setPWD();
void readIn();
// Convert command into cmd
void ToCMD(char * command);
// Terminate all functions
void terminate();
// return the path for execute if can't find return ERROR
char * findPath(char mypath[], char * command);
// secearch the command from History
int secearch_history(char * command);
// execute_cmd without background
void execute_cmd(char * path_cmd, char * cmd[]);
// execute_cmd with background
void execute_cmd_back(char * path_cmd, char * cmd[]);
// execute_cmd with pipe
void execute_cmd_pipe(char * path_cmd, char * cmd[]);


int main(){
    // Define MYPATH
    mypath = (char *)malloc(sizeof(char)* 1000);
    char c = '\0';
    char *command = (char *)malloc(sizeof(char)*100);
    char firstArgv[100];
    int i = 0;

    //char *MYPATH = getenv("MYPATH");
    //char *HOME = getenv("HOME");

    // Set up MYPATH
    setPath();
    // Set up pwd
    setPWD();

    //printf("%s $", crnpwd);
    printf("%s $", getenv("PWD"));
    while(1){	
        c = getchar();
        if(c != '\n'){
            strncat(command, &c, 1);	
        }	
        else{
            if(strlen(command) >= 1){
                c = '\0';
                strncat(command, &c, 1);
                //printf("command is %s\n", command);
                History[crtHistory] = (char *)malloc(sizeof(char)*100);
                strcpy(History[crtHistory], command);
                //printf("History is %s\n", History[crtHistory]);
                crtHistory ++;
                // Convert command into cmd
                ToCMD(command);

                bool execute_need = 1;
                strcpy(firstArgv, cmd[0]);
                for(i = 0; i < CMDMAX  && cmd[i] != NULL; i ++){
                    if(cmd[i][0] == '~'){
                        char * home = getenv("HOME");
                        char * tmp1 = (char *)malloc(sizeof(char)*1000);
                        strcpy(tmp1, home);
                        strcat(tmp1, cmd[i] + 1);
                        strcpy(cmd[i], tmp1);
                        //printf("%s\n", cmd[i]);
                    }
                }
                // Pipe situation
                for(i = 0; i < CMDMAX  && cmd[i] != NULL; i ++){
                    if(!strcmp(cmd[i], "|")){
                        /*
                        int j = i;
                        int k = 0;
                        while(cmd[j] != NULL){
                            cmd1[k] = cmd[j];
                            k ++;
                            j ++;
                        }
                        printf("cmd1 %s\n", cmd[0]);
                        cmd[i] = NULL;
                        printf("cmd2 %s\n", cmd1[0]);
                        */

                        /*
                        char * path_cmd = (char*)malloc(sizeof(char)*1000);
                        // return the path for execute if can't find return ERROR
                        path_cmd = findPath(mypath, cmd[0]);
                        //printf("path is %s\n", pathFound);
                        //if(eligible_cmd(firstArgv)){

                        if(!strcmp(path_cmd, "Error")){
                            printf("ERROR: command '%s' not found\n", command);
                        }
                        else{
                        // execute_cmd with pipe
                            execute_cmd_pipe(path_cmd, cmd);
                        }
                        */
                        execute_need = 0;
                        break;
                    }
                }
                // Exit and quit situation
                if(strcmp(firstArgv, "exit") == 0 || strcmp(firstArgv, "quit") == 0){
                    // Terminate all functions
                    terminate();
                }
                // History
                if(strcmp(firstArgv, "history") == 0){
                    for(i = 0; i < crtHistory; i ++){
                        printf("%03d   %s\n", i, History[i]);
                    }
                    execute_need = 0;
                    //continue;
                }
                // cd
                else if(strcmp(firstArgv, "cd") == 0){
                    if(cmd[1] == NULL){
                        chdir(getenv("HOME"));
                    }
                    else{
                        chdir(cmd[1]);
                    }
                    execute_need = 0;
                }
                // ! situation
                else if(firstArgv[0] == '!'){
                    if(firstArgv[1] == '!'){
                        printf("Done");
                        char * nCommand = History[crtHistory - 2];
                        ToCMD(nCommand);
                        execute_need = 1;
                    }
                    else{
                        // secearch the command from History
                        int line = secearch_history(command);
                        if(line >= 0){
                            char * Ncommand = History[line];
                            // Convert command into cmd
                            ToCMD(Ncommand);
                            execute_need = 1;
                        }
                    }
                }

                // whether use background
                bool back = 0;
                int c_length = strlen(command);
                if(command[c_length - 1] == '&'){
                    back = 1;
                    cmd[argc] = NULL;
                    argc--;
                }
                if(execute_need == 1){
                    char * path_cmd = (char*)malloc(sizeof(char)*1000);
                    // return the path for execute if can't find return ERROR
                    path_cmd = findPath(mypath, cmd[0]);
                    //printf("path is %s\n", pathFound);
                    //if(eligible_cmd(firstArgv)){

                    if(!strcmp(path_cmd, "Error")){
                        printf("ERROR: command '%s' not found\n", command);
                    }
                    else{
                        if(back == 0)
                            // execute_cmd without background
                            execute_cmd(path_cmd, cmd);
                        else if (back > 0)
                            // execute_cmd with background
                            execute_cmd_back(path_cmd, cmd);
                    }
                }
                execute_need = 1;
            }
            //printf("%s $", crnpwd);
            printf("%s $", getenv("PWD"));
            argc = 0;
            bzero(command, 100);
            bzero(firstArgv, 100);

        }
    }
    return EXIT_SUCCESS;
}



void setPath(){
    /*
    char * path = getenv( "PATH" );

    if(path == NULL){
        strcpy(path, "/Users/huiyuma/");
    }

    strcpy(mypath, path);
    */
    mypath = getenv("MYPATH");
    char * tempath = (char*)malloc(sizeof(char)* 1000);
    strcpy(tempath, mypath);


    if(strcmp(tempath, "/bin:.")){
        char * temp = strtok(tempath, ":");
        int i = 0;
        while (temp != NULL) {
            if(mypaths[i] == NULL){
                mypaths[i] = (char *)malloc(sizeof(char) * 100);
            }
            strcpy(mypaths[i], temp);
            i ++;
            temp = strtok(NULL, ":");
        }
    }
    else{
        int i = 0;
        for(i = 0; i < 2; i ++){
            if(mypaths[i] == NULL){
                mypaths[i] = (char *)malloc(sizeof(char) * 100);
            }
        }
        mypaths[0] = "/bin";
        mypaths[1] = ".";
    }
}

// Set up pwd
void setPWD(){
    crnpwd = getenv("PWD");
    return;
}

// Convert command into cmd
void ToCMD(char * command){
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
    cmd[i+1] = 0;
    argc = i;
}

void ToCMD2(char * command){
    int i = 0;
    char tmp[100];
    bzero(tmp, 100);
    while(*command != '\0'){
        if(*command == ' '){
            if(cmd1[i] == NULL){
                cmd1[i] = (char *)malloc(sizeof(char)*strlen(tmp) + 1);
            }
            strcpy(cmd1[i], tmp);
            bzero(tmp, 100);
            i ++;
        }
        else{
            strncat(tmp, command, 1);
        }
        command ++;
    }
    cmd1[i] = (char *)malloc(sizeof(char)*strlen(tmp) + 1);
    strcpy(cmd1[i], tmp);
    cmd1[i+1] = 0;
    argc = i;
}

// Terminate all functions
void terminate(){
    printf("bye\n");
    exit(1);
}

void readIn(){
    int i;
    for(i = 0; i < 4; i++){
        cmd[i] = (char *) malloc(10 * sizeof(char));
    }
    i = 0;
    while(1){
        char c = getchar();
        if(c != '\n'){
            if(c == ' '){
                if(strlen(cmd[i]) != 0){
                    char * end = "\0";
                    strcat(cmd[i], end);
                    i ++;
                }
            }
            else{
                strcat(cmd[i], &c);
            }
        }
        else{
            if(strlen(cmd[i]) == 0){
                char * end = "\0";
                strcat(cmd[i], end);
                //cmd[i+1] = 0;
            }
            cmd[i + 1] = 0;
            return;
        }
    }
    return;
}


// return the path for execute if can't find return ERROR
char * findPath(char mypath[], char * command){
    char * pathFound = (char*)malloc(sizeof(char)*1000);
    char * tempath = (char*)malloc(sizeof(char)* 1000);
    strcpy(tempath, mypath);
    struct dirent * file;
    pathFound = strtok(tempath, ":");
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

// secearch the command from History
int secearch_history(char * command){
    int i;
    int count = strlen(command) - 1;
    char * com = (char*)malloc(sizeof(char) * count);
    for(i = 0; i < count; i ++){
        com[i] = command[i + 1];
    }

    char * temp=(char*)malloc(sizeof(char)*count);
    
    for(i = crtHistory - 2; i >= 0; i --){
        int j;
        for(j = 0; j < count; j ++){
            temp[j] = History[i][j];
        }
        //printf("%s\n", temp);
        if(!strcmp(temp, com)){
            printf(">>> %s\n", History[i]);
            return i;
        }
        bzero(temp, count);
    }
    printf("ERROR: no command with '%s' prefix in history\n", com);
    return -1;
}

// execute_cmd without background
void execute_cmd(char * path_cmd, char * cmd[]){
    pid_t pid;
    int status;
    pid = fork();
    if( pid < 0){
        perror("fork() failed!");
        //return EXIT_FAILURE;
        return;
    }
    else if( pid == 0){
        //printf("CHILD: Happy birthday!\n");
        int i;
        for(i = 0; i < CMDMAX - 1 && cmd[i] != NULL; i ++){
            if(!strcmp(cmd[i], ">") && cmd[i + 1] != NULL){ 
                freopen(cmd[i + 1], "w", stdout);
                cmd[i] = NULL;
                break;
            }
            else if(!strcmp(cmd[i], ">>") && cmd[i + 1] != NULL){
                if(access(cmd[i + 1], 0) == 0){
                    freopen(cmd[i + 1], "a", stdout);
                    cmd[i] = NULL;
                    break;
                }
                else{
                    printf("ERROR: no such file to append to (%s)\n", cmd[i + 1]);
                    exit(1);
                }
            }
            else if(!strcmp(cmd[i], "<") && cmd[i + 1] != NULL){
                freopen(cmd[i + 1], "r", stdin);
                cmd[i] = NULL;
                break;
            }
        }
        strcat(path_cmd, "/");
        strcat(path_cmd, cmd[0]);
        execvp(path_cmd, cmd);
        perror(path_cmd);
        exit(1);
    }
    //if(waitpid(pid, &status, 0) == -1){
    if(wait(&status) == -1){
        perror("Error in waitpid");
        exit(1);
    }
    return;
}

// execute_cmd with background
void execute_cmd_back(char * path_cmd, char * cmd[]){
    pid_t pid;
    int status;
    pid = fork();
    if( pid < 0){
        perror("fork() failed!");
        //return EXIT_FAILURE;
        return;
    }
    else if( pid == 0){
        int i;
        for(i = 0; i < CMDMAX - 1 && cmd[i] != NULL; i ++){
            if(!strcmp(cmd[i], ">") && cmd[i + 1] != NULL){ 
                freopen(cmd[i + 1], "w", stdout);
                cmd[i] = NULL;
                break;
            }
            else if(!strcmp(cmd[i], ">>") && cmd[i + 1] != NULL){
                if(access(cmd[i + 1], 0) == 0){
                    freopen(cmd[i + 1], "a", stdout);
                    cmd[i] = NULL;
                    break;
                }
                else{
                    printf("ERROR: no such file to append to (%s)\n", cmd[i + 1]);
                    exit(1);
                }
            }
            else if(!strcmp(cmd[i], "<") && cmd[i + 1] != NULL){
                freopen(cmd[i + 1], "r", stdin);
                cmd[i] = NULL;
                break;
            }
        }
        strcat(path_cmd, "/");
        strcat(path_cmd, cmd[0]);
        execvp(path_cmd, cmd);
        perror(path_cmd);
        exit(1);
    }
    else{
        printf("[process running in background with pid %d]\n", pid);
    }
    pid_t child_pid;
    child_pid = waitpid(0, &status, WNOHANG);
    if(child_pid > 0){
        printf("[process %d completed]\n", pid);
    }
    return;
}

/*
// execute_cmd with pipe
void execute_cmd_pipe(char * path_cmd, char * cmd[]){
    pid1 = fork();
    if (pid1 < 0)
    {
        perror("fork() failed!\n");
        exit(1);
    }
    else if (pid1 == 0)
    {
        //the child1
        pipe(pfds);
        pid2 = fork();
        if (pid2 < 0)
        {
            perror("fork() failed!\n");
            exit(1);
        }
        else if (pid2 == 0)
        {
            //the child2
            close(0);
            dup(pfds[0]);
            close(pfds[1]);
            execv(cmd2, arg2);
        }
        else
        {
            //still child 1, but now he is a parent
            close(1);
            dup(pfds[1]);
            close(pfds[0]);
            execv(cmd1, arg1);
        }


    }
    else
    {
        //the parent
        wait(&status);
    }
}
*/
