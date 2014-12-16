#include "librdwr.h"

#define FILE_SIZE 1024*1024 


void p(int signo) {}

int main(int argc, char** argv)
{
    int fd = open("mmap.dat", O_CREAT | O_RDWR, 0666); //make mmap.dat
    if(fd < 0)
    {
        printf("Сan't create/open file\n");
        exit(-1);
    }
	
    char* recieve = mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); // allocate memory, or map files or devices into memory
    if(recieve == MAP_FAILED )
    {
        printf("Mapping failed!\n");//err
    }

    sigset_t set;
	//waiting sign
    struct sigaction sigact;
    sigact.sa_handler = p;
    sigfillset(&sigact.sa_mask);
    sigaction(SIGUSR1, &sigact, NULL);
    sigemptyset(&set);
    sigsuspend(&set);
    struct stat* buf = (struct stat*)calloc(1, sizeof(struct stat));//memory
	
    if(stat("mmap.dat"
            , buf) < 0)
    {
        printf("problem\n");
        exit(-1);
    }
    strcat(recieve, "Work done\n");
    if(munmap((void*)recieve, strlen(recieve)) < 0) //remove a mapping
    {
        printf("mistake in manmap\n");
        exit(-1);
    }
    close(fd);
}
