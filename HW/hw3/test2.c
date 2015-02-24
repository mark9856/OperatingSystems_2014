/* strtok.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>



#define CMDMAX 4
// Store single path
char * mypaths[10]; 
// A combination of mypath
char * mypath;
// current pwd
char * crnpwd;
// total cmd argc
char * newpwd;
int argc;
// cmd 
char * cmd[CMDMAX];
// second cmd for pipe use




int main(){

  crnpwd = getenv("PWD");
      // crnpwd = (char *)malloc(sizeof(char)*100);
  newpwd = (char *)malloc(sizeof(char)*100);
  // newpwd = getenv("PWD");
  strcpy(newpwd, crnpwd);
  char * dir = "/storage";
  strcat(newpwd, dir);

  chdir(newpwd);

  // printf("%s\n", crnpwd);
  // printf("%s\n", getenv("PWD"));
  // int fd1 = open("test.c", O_RDONLY);
  // if ( fd1 < 0 )
  // {
  //   fprintf( stderr, "ERROR: open() failed\n" );
  //   perror( "error opening file" );
  //   return 0;
  // }
  // char buff[80];
  // int rc = read( fd1, buff, 25 );
  //        read at most 25 bytes from fd into buffer 

  //       // printf("Hello\n");
  // if ( rc < 0 )
  // {
  //   perror( "read() failed" );
  //   return 0;
  // }

  // buff[rc] = '\0';

  // printf( "read() returned %d and read %s\n", rc, buff );

  // close( fd1 );
  // return 0;

   FILE *fp;
   int c;
  
   fp = fopen("test.c","r");
   int count = 0;

   fseek(fp, 0L, SEEK_END);
   count = ftell(fp);
   fseek(fp, 0L, SEEK_SET);
   printf("%d\n", count);
   while(1)
   {
      c = fgetc(fp);
      if( feof(fp) )
      { 
          break ;
      }
      printf("%c", c);
      count++;
   }
   printf("\n");
   fclose(fp);
   return(0);

}


