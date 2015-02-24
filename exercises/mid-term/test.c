#include <stdio.h>


int main(){
    int x = 2;
    int y = 2;
    int pid = fork();
    if(pid == 0) x++;
    printf("%d\n", x + y);
    return 0;
}
