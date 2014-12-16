#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int main(int argc, char** argv)
{
    int res = fork(); 
    if(res == 0)
    {
        char* snd = (char*)calloc(strlen("./stat_wr ") + 50, sizeof(char)); //memory
        strcat(snd, "./stat_wr "); //конкатенация
        strcat(snd, argv[1]); 
        execlp("sh", "sh", "-c", snd, NULL);//first prog
        printf("Can't run first program\n"); //err
    }
    execlp("sh", "sh", "-c", "./stat_rd", NULL); //second prog
    printf("Can't run second program\n"); //err
}
