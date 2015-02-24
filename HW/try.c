#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>


int main(int argc, char* argv[]){
    char ** try;
    //*try[0] = "HelloWorld";
    /*
    try = (char **) malloc(10 * sizeof(char *));
    for(int i = 0; i < 10; i++){
        try[i] = (char *) malloc(10 * sizeof(char));
    }
    strcpy( try[0], "HelloWorld");
    strcpy( try[1], "elloWorld");
    printf("*try[0] is %s\n", try[0]);
    printf("*try[1] is %s\n", try[1]);
    */

    /*
    char test[80];
    char *sep = ":";
    char *word, *pharse, *brkt, *brkb;

    strcpy(test, "This:is:a:test");
        printf("%s\n", word);
    }
    */

    /*
    
    char * path = getenv( "PATH" );
    for(word = strtok(path, sep); word; word = strtok(NULL, sep)){
        printf("%s\n", word);
    }
    */

    /*
    char * pathFound;
    char * path = getenv( "PATH" );
    for(pathFound = strtok(path, ":"); pathFound; pathFound = strtok(NULL, ":")){
        //printf("%s\n", pathFound);
        if(strcmp("s", pathFound)){
            printf("%s\n", pathFound);
        }
    }
    */

    /*
    char * pathFound;
    char * path = getenv( "PATH" );
    struct dirent * file;
    for(pathFound = strtok(path, ":"); pathFound; pathFound = strtok(NULL, ":")){
        DIR * dir = opendir(pathFound);
        while((file = readdir(dir)) != NULL){
            if(strcmp(file -> d_name, "ls") == 0){
                //return pathFound;
                printf("%s\n", pathFound);
                break;
            }
        }
    }
    */
    char * cm = (char*)malloc(sizeof(char)*10);
    //scanf("%s",cm);
    cm = "ls";
    int rc = execv("/bin/ls", &cm);

    /*
    word = strtok(test, sep);
    printf("%s\n", word);
    */

    return EXIT_SUCCESS;
}
