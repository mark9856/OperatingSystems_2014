#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include <semaphore.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <math.h>

#define BUFFER_SIZE 15
#define MAX_CLIENT 1024
#define MAX_FILE_NUM 1024

struct file_mutex {
    char* filename;
    pthread_mutex_t mutex_add;
    pthread_mutex_t mutex_append;
    pthread_mutex_t mutex_read;
};

struct in_connection {
    char buffer[BUFFER_SIZE];
    int newsock;
};

//global variables
char* home_dir = ".storage";
struct file_mutex fm[MAX_FILE_NUM];
//mutex for the current_file_number global variable
pthread_mutex_t mutex_fn = PTHREAD_MUTEX_INITIALIZER;
int current_file_number = 0;

void * whattodo(void * arg);
char* extract_cmd(char buffer[BUFFER_SIZE]);
int extract_file_len(char buffer[BUFFER_SIZE]);
char* extract_file_name(char buffer[BUFFER_SIZE]);
int handle_command(char* cmd, char* file_name, void* arg, int* received_len, int index);
int isdigit(char c);
int findfile(char* filename);
void mutex_init(struct file_mutex* one_fm);



int main(int argc, char* argv[]) {
    if (argc != 2) {
        perror("ERROR:\n  Input: port number\n");
        return EXIT_FAILURE;
    }
    int portNO = atoi(argv[1]);
    if (portNO < 8000 || portNO > 9000) {
        perror("Invalid port number\n");
        return EXIT_FAILURE;
    }

    /*** create directry .storage***/
    int dir_exist = 0;
    DIR * dir = opendir(".");
    struct dirent *file;
    while((file = readdir( dir ))!=NULL) {
        if(strcmp(file->d_name, home_dir)==0) {
            dir_exist = 1;
            break;
        }
    }
    if(dir_exist==0) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork() failed!\n");
            return EXIT_FAILURE;
        }
        if(pid == 0) {
            int rc=execv("/bin/mkdir", (char* []){"mkdir",home_dir, NULL});
            if(rc<0) {
                perror("ERROR: command cannot be executed!\n");
                return EXIT_FAILURE;
            }
            return 0;
        }
        if(pid > 0) {
            int status;
            wait(&status);
            closedir(dir);
        }
    }
    
    /**** file mutex array initialization ****/
    int i = 0;
    for (; i < MAX_FILE_NUM; i++) {
        struct file_mutex one_fm;
        fm[i] = one_fm;
    }
    i = 0;
    dir = opendir(home_dir);
    while((file = readdir( dir ))!=NULL) {
        if((file->d_name)[0]=='.') {
            continue;
        }
        fm[i].filename = (char*)malloc(sizeof(char)*100);
        mutex_init(&fm[i]);
        strcpy(fm[i].filename,file->d_name);
        pthread_mutex_lock(&mutex_fn);
        current_file_number++;
        pthread_mutex_unlock(&mutex_fn);
        i++;
    }
    closedir(dir);
    
    /**** set up port ****/
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("Socket() failed!\n");
        return EXIT_FAILURE;
    }
    struct sockaddr_in server;
    server.sin_family = PF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(portNO);
    int len = sizeof(server);
    if (bind(sock, (struct sockaddr *)&server, len) < 0) {
        perror("bind() failed!\n");
        return EXIT_FAILURE;
    }
    listen(sock, MAX_CLIENT);
    printf("Started file-server\n");
    printf("Listening on port %d\n", portNO);
    /**** declare client ****/
    struct sockaddr_in client;
    int fromlen = sizeof(client);
    
    /*** accept connections ***/
    while (1) {
        int newsock = accept(sock, (struct sockaddr *)&client, (socklen_t*)&fromlen);
        printf("Received incoming connection from %s\n",
               inet_ntoa((struct in_addr)client.sin_addr));
        fflush(NULL);
        pthread_t one_thread;
        struct in_connection* in_c = (struct in_connection*)malloc(sizeof(struct in_connection));
        in_c->newsock = newsock;
        int rc = pthread_create(&one_thread, NULL, whattodo, in_c);
        if (rc!=0) {
            perror("MAIN: Could not create child thread\n");
        }
    }
    return 0;
}

//what to do when creating a new thread
//after receiving an incomming connection
void * whattodo(void * arg) {
    struct in_connection* in_c = (struct in_connection* )arg;
    int rc = pthread_detach(pthread_self());//detach to avoid zombie threads
    if(rc!=0) {
        fprintf(stderr, "pthread_detach() failed (%d): %s\n", rc, strerror(rc));
        exit(EXIT_FAILURE);
    }
    int n;
    do {
        //reset buffer after each receiving
        memset(&(in_c->buffer)[0], '\0', BUFFER_SIZE);
        n = recv(in_c->newsock, in_c->buffer, BUFFER_SIZE, 0);
        if(n<=0) {//client closed connection
            break;
        }
        in_c->buffer[n] = '\0';
        char* cmd  = extract_cmd(in_c->buffer);
        if (strcmp(cmd,"")==0) continue;//client typed in return key
        printf("[thread %u] Rcvd: %s",
               (unsigned int)pthread_self(), in_c->buffer);
        
        int success = 0;
        int index;//corresponds to the file in the file_mutex array
        if (strcmp(cmd,"ADD")==0 || strcmp(cmd,"APPEND")==0) {
            int expected_file_len = extract_file_len(in_c->buffer);
            char* file_name = extract_file_name(in_c->buffer);
            int received_len = 0;
            if (strcmp(cmd,"ADD")==0) {
                //set the file_mutex array accordingly
                pthread_mutex_lock(&mutex_fn);
                index = current_file_number++;
                pthread_mutex_unlock(&mutex_fn);
                fm[index].filename = (char*)malloc(sizeof(char)*100);
                strcpy(fm[index].filename,file_name);
                mutex_init(&fm[index]);
            } else {//Append
                pthread_mutex_lock(&mutex_fn);
                index = findfile(file_name);
                pthread_mutex_unlock(&mutex_fn);
            }
            //if expected file length is larger than the buffer size,
            //continue read in
            while (received_len < expected_file_len) {
                memset(&(in_c->buffer)[0], '\0', BUFFER_SIZE);
                n = recv(in_c->newsock, in_c->buffer, BUFFER_SIZE, 0);
                success = handle_command(cmd, file_name, in_c, &received_len, index);
                if (success==-1) break;
                strcpy(cmd, "APPEND");
            }
            //send messages accordingly
            if(success==1 && strcmp(cmd,"APPEND")==0) {
                printf("[thread %u] Transferred file (%d bytes)\n",
                       (unsigned int)pthread_self(), received_len);
                int n = send(in_c->newsock, "ACK\n", 4, 0);
                if (n!=4) {
                    printf("[thread %u] send() failed!\n",
                           (unsigned int)pthread_self());
                } else {
                    printf("[thread %u] Sent: ACK\n", (unsigned int)pthread_self());
                }
            }
        } else if (strcmp(cmd,"READ")==0||strcmp(cmd,"DELETE")==0){
            char* file_name = extract_file_name(in_c->buffer);
            pthread_mutex_lock(&mutex_fn);
            index = findfile(file_name);
            pthread_mutex_unlock(&mutex_fn);
            success = handle_command(cmd, file_name, in_c, 0, index);
        } else if(strcmp(cmd,"LIST")==0) {
            success = handle_command(cmd, "", in_c, 0, 0);
        } else {
            int n = send(in_c->newsock, "ERROR: NO SUCH COMMAND\n", 23, 0);
            if (n!=23) {
                printf("[thread %u] send() failed!\n",
                       (unsigned int)pthread_self());
            } else {
                printf("[thread %u] Sent: ERROR NO SUCH COMMAND\n", (unsigned int)pthread_self());
            }
            
        }
        
    } while (n > 0);
    printf("[thread %u] Client closed its socket....terminating\n",
           (unsigned int)pthread_self());
    close(in_c->newsock);
    free(in_c);
    return NULL;
}

char* extract_cmd(char buffer[BUFFER_SIZE]) {
    char* cmd = (char*)malloc(sizeof(char)*10);
    int i = 0;
    while (buffer[i]!=' '&&
           buffer[i]!='\n'&&
           buffer[i]!='\r'&&
           buffer[i]!='\t') {
        cmd[i] = buffer[i];
        i++;
    }
    return cmd;
}


char* extract_file_name(char buffer[BUFFER_SIZE]) {
    char* cmd_name = (char*)malloc(sizeof(char)*100);
    int i = 0;
    while (buffer[i]!=' ') {
        i++;
    }
    i++;
    int j = 0;
    while (buffer[i]!=' '&&
           buffer[i]!='\n'&&
           buffer[i]!='\r'&&
           buffer[i]!='\t') {
        cmd_name[j] = buffer[i];
        i++;
        j++;
    }
    return cmd_name;
}

int extract_file_len(char buffer[BUFFER_SIZE]) {
    char* cmd_len = (char*)malloc(sizeof(char)*10);
    int i = 0;
    while (buffer[i]!=' ') {
        i++;
    }
    i++;
    while (buffer[i]!=' ') {
        i++;
    }
    i++;
    int j=0;
    while (isdigit(buffer[i])==1) {
        cmd_len[j] = buffer[i];
        i++;
        j++;
    }
    return atoi(cmd_len);

}

int isdigit(char c) {
    if (c=='0'||c=='1'||c=='2'||c=='3'||c=='4'||c=='5'||c=='6'||c=='7'||c=='8'||c=='9')
        return 1;
    else return 0;
}

int handle_command(char* cmd, char* file_name, void* arg, int* received_len, int index) {
    struct in_connection* in_c = (struct in_connection* )arg;
    if (strcmp(cmd,"ADD")==0) {
        //////critical section for adding file//////
        pthread_mutex_lock(&fm[index].mutex_add);
        int dir_exist = 0;
        DIR * dir = opendir(home_dir);
        struct dirent *file;
        while((file = readdir( dir ))!=NULL) {
            if(strcmp(file->d_name, file_name)==0) {
                dir_exist = 1;
                break;
            }
        }
        if (dir_exist == 1) {
            int n = send(in_c->newsock, "ERROR: FILE EXISTS\n", 19, 0);
            if (n!=19) {
                printf("[thread %u] send() failed!\n",
                       (unsigned int)pthread_self());
            } else {
                printf("[thread %u] Sent: ERROR FILE EXISTS\n",
                   (unsigned int)pthread_self());
            }
            fflush(NULL);
            return -1;
        } else {
            char* path = (char*)malloc(sizeof(char)*1000);
            strcpy(path, home_dir);
            strcat(path,"/");
            strcat(path,file_name);
            FILE* fp = fopen(path,"wb");
            if (fp == NULL) {
                int n = send(in_c->newsock, "ERROR: CANNOT CREATE FILE\n",
                             26, 0);
                if (n!=26) {
                    printf("[thread %u] send() failed!\n",
                           (unsigned int)pthread_self());
                } else {
                    printf("[thread %u] Sent: ERROR CANNOT CREATE FILE\n",
                       (unsigned int)pthread_self());
                }
                fflush(NULL);
                return -1;
            } else {
                int len = 0;
                char* tmp_buffer = (char*)malloc(sizeof(char)*BUFFER_SIZE);
                for(; in_c->buffer[len]!='\0' &&
                    in_c->buffer[len]!='\n'&&
                    in_c->buffer[len]!='\r'&&
                    in_c->buffer[len]!='\t'; len++) {
                    tmp_buffer[len] = in_c->buffer[len];
                }
                *received_len += len;
                fprintf(fp, "%s",tmp_buffer);
                fflush(NULL);
                //sleep(8);
                pthread_mutex_unlock(&fm[index].mutex_add);
                //////critical section ends//////
                return 1;
            }
        }
    } else if (strcmp(cmd,"APPEND")==0) {
        if (index==-1) {
            int n = send(in_c->newsock, "ERROR: NO SUCH FILE\n", 20, 0);
            if (n!=20) {
                printf("[thread %u] send() failed!\n",
                       (unsigned int)pthread_self());
            } else {
                printf("[thread %u] Sent: ERROR NO SUCH FILE\n",
                   (unsigned int)pthread_self());
            }
            fflush(NULL);
            return -1;
        }
        int dir_exist = 0;
        //////critical section: avoid other thread to append or add//////
        pthread_mutex_lock(&fm[index].mutex_add);
        pthread_mutex_lock(&fm[index].mutex_append);
        DIR * dir = opendir(home_dir);
        struct dirent *file;
        while((file = readdir( dir ))!=NULL) {
            if(strcmp(file->d_name, file_name)==0) {
                dir_exist = 1;
                break;
            }
        }
        if (dir_exist == 0) {
            int n = send(in_c->newsock, "ERROR: NO SUCH FILE\n", 20, 0);
            if (n!=20) {
                printf("[thread %u] send() failed!\n",
                       (unsigned int)pthread_self());
            } else {
                printf("[thread %u] Sent: ERROR NO SUCH FILE\n",
                   (unsigned int)pthread_self());
            }
            fflush(NULL);
            return -1;
        } else {
            char* path = (char*)malloc(sizeof(char)*1000);
            strcpy(path, home_dir);
            strcat(path,"/");
            strcat(path,file_name);
            //wait before all reads are done
            pthread_mutex_lock(&fm[index].mutex_read);
            FILE* fp = fopen(path,"ab");
            
            if (fp == NULL) {
                int n = send(in_c->newsock, "ERROR: CANNOT APPEND TO SUCH FILE\n", 34, 0);
                if (n!=34) {
                    printf("[thread %u] send() failed!\n",
                           (unsigned int)pthread_self());
                } else {
                    printf("[thread %u] Sent: ERROR CANNPT APPEND TO SUCH FILE\n",
                       (unsigned int)pthread_self());
                }
                fflush(NULL);
                pthread_mutex_unlock(&fm[index].mutex_read);
                return -1;
            } else {
                int len = 0;
                char* tmp_buffer = (char*)malloc(sizeof(char)*BUFFER_SIZE);
                for(; in_c->buffer[len]!='\0' &&
                    in_c->buffer[len]!='\n'&&
                    in_c->buffer[len]!='\r'&&
                    in_c->buffer[len]!='\t'; len++) {
                    tmp_buffer[len] = in_c->buffer[len];
                }
                *received_len += len;
                fprintf(fp, "%s", tmp_buffer);
                fflush(NULL);
                //sleep(8);
                pthread_mutex_unlock(&fm[index].mutex_read);
                pthread_mutex_unlock(&fm[index].mutex_append);
                pthread_mutex_unlock(&fm[index].mutex_add);
                //////critical section ends//////
                return 1;
            }
        }

    } else if (strcmp(cmd,"READ")==0) {
        if (index==-1) {
            int n = send(in_c->newsock, "ERROR: NO SUCH FILE\n", 20, 0);
            if (n!=20) {
                printf("[thread %u] send() failed!\n",
                       (unsigned int)pthread_self());
            } else {
                printf("[thread %u] Sent: ERROR NO SUCH FILE\n",
                   (unsigned int)pthread_self());
            }
            fflush(NULL);
            return -1;
        }
        //wait till done with adding and appending
        pthread_mutex_lock(&fm[index].mutex_add);
        pthread_mutex_unlock(&fm[index].mutex_add);
        pthread_mutex_lock(&fm[index].mutex_append);
        pthread_mutex_unlock(&fm[index].mutex_append);
        //unlock read mutex first and then claim the lock
        //so that any other "READ"'s can read the file, but
        //"DELETE" cannot execute before all "READ"'s
        //finish, since "DELETE" doesn't unlock read mutex
        //before it claims the read mutex
        pthread_mutex_unlock(&fm[index].mutex_read);
        pthread_mutex_lock(&fm[index].mutex_read);
        int file_exist = 0;
        DIR * dir = opendir(home_dir);
        struct dirent *file;
        while((file = readdir( dir ))!=NULL) {
            fflush(NULL);
            if(strcmp(file->d_name, file_name)==0) {
                file_exist = 1;
                break;
            }
        }
        if (file_exist == 0) {
            int n = send(in_c->newsock, "ERROR: NO SUCH FILE\n", 20, 0);
            if (n!=20) {
                printf("[thread %u] send() failed!\n",
                       (unsigned int)pthread_self());
            } else {
                printf("[thread %u] Sent: ERROR NO SUCH FILE\n",
                   (unsigned int)pthread_self());
            }
            fflush(NULL);
            return -1;
        } else {
            char* path = (char*)malloc(sizeof(char)*1000);
            strcpy(path, home_dir);
            strcat(path, "/");
            strcat(path,file_name);
            FILE* fp = fopen(path,"rb");
            if (fp == NULL) {
                int n = send(in_c->newsock, "ERROR: CANNOT READ FROM SUCH FILE\n", 34, 0);
                if (n!=34) {
                    printf("[thread %u] send() failed!\n",
                           (unsigned int)pthread_self());
                } else {
                    printf("[thread %u] Sent: ERROR CANNOT READ FROM SUCH FILE\n",
                       (unsigned int)pthread_self());
                }
                fflush(NULL);
                pthread_mutex_unlock(&fm[index].mutex_read);
                return -1;
            } else {
                char *file_content;
                unsigned long file_len;
                //Get file length
                fseek(fp, 0, SEEK_END);
                file_len=ftell(fp);
                fseek(fp, 0, SEEK_SET);
                
                //Allocate memory
                file_content=(char *)malloc(file_len+1);
                
                //Read file contents into buffer
                fread(file_content, file_len, 1, fp);
                fclose(fp);
                
                //sleep(8);
                pthread_mutex_unlock(&fm[index].mutex_read);
                //////critical section ends//////
                int digit_len = 0;
                if (file_len==0) digit_len += 1;
                else  digit_len += floor(log10(abs(file_len)))+1;
                char* l = (char*)malloc(sizeof(char)*digit_len);
                sprintf(l, "%lu", file_len);
                int n = send(in_c->newsock, "ACK ", 4, 0);
                n += send(in_c->newsock, l, digit_len, 0);
                n += send(in_c->newsock, "\n", 1, 0);
                n += send(in_c->newsock, file_content, file_len, 0);
                n += send(in_c->newsock, "\n", 1, 0);
                fflush(NULL);
                int real_len = 4 + digit_len + 1 + file_len + 1;
                if (n!=real_len) {
                    printf("[thread %u] sent() failed!\n",
                           (unsigned int)pthread_self());
                } else {
                    printf("[thread %u] Sent: ACK %lu\n", (unsigned int)pthread_self(), file_len);
                }
                printf("[thread %u] Transferred file (%lu bytes)\n",
                       (unsigned int)pthread_self(), file_len);
                return 1;
            }
        }
        
    } else if (strcmp(cmd,"LIST")==0) {
        char* list = (char*)malloc(sizeof(char)*1000);
        char* final_list = (char*)malloc(sizeof(char)*1000);
        strcpy(list,"\n");
        int i = 0;
        int l = 2;
        DIR * dir = opendir(home_dir);
        struct dirent *file;
        while((file = readdir( dir ))!=NULL) {
            if((file->d_name)[0]=='.') {
                continue;
            }
            strcat(list, file->d_name);
            l += strlen(file->d_name);
            strcat(list, "\n");
            l += 2;
            i++;
        }
        char* num_file = (char*)malloc(sizeof(char)*100);
        sprintf(num_file, "%d", i);
        strcpy(final_list, num_file);
        if (i == 0) l += 1;
        else  l += floor(log10(abs(i)))+1;
        strcat(final_list, list);
        int n = 0;
        fflush(NULL);
        n = send(in_c->newsock, final_list, l, 0);
        if(n!=l) {
            printf("[thread %u] ERROR: Sending %s failed\n",
                   (unsigned int)pthread_self(), final_list);
            fflush(NULL);
            return -1;
        }
        return 1;
        
    } else if (strcmp(cmd,"DELETE")==0) {
        if (index==-1) {
            int n = send(in_c->newsock, "ERROR: NO SUCH FILE\n", 20, 0);
            if (n!=20) {
                printf("[thread %u] send() failed!\n",
                       (unsigned int)pthread_self());
            } else {
                printf("[thread %u] Sent: ERROR NO SUCH FILE\n",
                   (unsigned int)pthread_self());
            }
            return -1;
        }
        int file_exist = 0;
        //////critical section: wait before deleting a file
        pthread_mutex_lock(&fm[index].mutex_add);
        pthread_mutex_lock(&fm[index].mutex_append);
        pthread_mutex_lock(&fm[index].mutex_read);
        
        DIR * dir = opendir(home_dir);
        struct dirent *file;
        while((file = readdir( dir ))!=NULL) {
            if(strcmp(file->d_name, file_name)==0) {
                file_exist = 1;
                break;
            }
        }
        if(file_exist==1) {
            pid_t pid = fork();
            if (pid < 0) {
                printf("[thread %u] fork() failed!\n",
                       (unsigned int)pthread_self());
                return EXIT_FAILURE;
            }
            if(pid == 0) {
                char* rm_file = (char*)malloc(sizeof(char)*100);
                strcpy(rm_file,"");
                strcat(rm_file,home_dir);
                strcat(rm_file,"/");
                strcat(rm_file,file_name);
                int rc=execv("/bin/rm", (char* []){"rm",rm_file, NULL});
                if(rc<0) {
                    int n = send(in_c->newsock, "ERROR: COMMAND CANNOT BE EXECUTED\n", 34, 0);
                    if (n!=34) {
                        printf("[thread %u] send() failed!\n",
                               (unsigned int)pthread_self());
                    } else {
                        printf("[thread %u] Sent: ERROR COMMAND CANNOT BE EXECUTED\n",
                           (unsigned int)pthread_self());
                    }
                    exit(EXIT_FAILURE);
                }
                return 0;
            }
            if(pid > 0) {
                int status;
                wait(&status);
                //remove it from fm
                int i = index;
                for (; i < current_file_number-1; i++) {
                    fm[i] = fm[i+1];
                }
                strcpy(fm[i].filename,"");
                mutex_init(&fm[i]);
                pthread_mutex_lock(&mutex_fn);
                current_file_number--;
                pthread_mutex_unlock(&mutex_fn);
                pthread_mutex_unlock(&fm[index].mutex_read);
                pthread_mutex_unlock(&fm[index].mutex_append);
                pthread_mutex_unlock(&fm[index].mutex_add);
                //////critical section ends//////
                printf("[thread %u] Delete \"%s\" file\n",
                       (unsigned int)pthread_self(), file_name);
                int n = send(in_c->newsock, "ACK\n", 4, 0);
                if (n!=4) {
                    printf("[thread %u] send() failed!\n",
                           (unsigned int)pthread_self());
                } else {
                    printf("[thread %u] Sent: ACK\n",
                       (unsigned int)pthread_self());
                }
                return 1;
            }
            
        } else {
            int n = send(in_c->newsock, "ERROR: NO SUCH FILE\n", 20, 0);
            if (n!=20) {
                printf("[thread %u] send() failed!\n",
                       (unsigned int)pthread_self());
            } else {
                printf("[thread %u] Sent: ERROR NO SUCH FILE\n",
                   (unsigned int)pthread_self());
            }
            return -1;
        }
        
    }
    return -1;
}

int findfile(char* filename) {
    int i = 0;
    for(; i < current_file_number; i++) {
        if (strcmp(fm[i].filename,filename)==0) {
            return i;
        }
    }
    return -1;
}

void mutex_init(struct file_mutex* one_fm) {
    pthread_mutex_init(&(one_fm->mutex_add),NULL);
    pthread_mutex_init(&(one_fm->mutex_append),NULL);
    pthread_mutex_init(&(one_fm->mutex_read),NULL);
}