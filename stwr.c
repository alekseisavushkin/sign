
#include "librdwr.h"

#define FILE_SIZE 1024*1024

int main(int argc, char** argv)
{
    DIR *dirfd;
    dirfd = opendir(argv[1]);	
    if(dirfd == NULL) 
    {
        printf("Can't open directory\n"); 
        exit(-1);
    }
    struct dirent* opened; 
    opened = readdir(dirfd); //wr
	
    int fd = open("mmap.dat", O_CREAT | O_RDWR | O_TRUNC, 0666); //mmap.dat
    if(fd < 0) // если что-то не так
    {
        printf("can't create file\n"); 
        exit(-1);
    }
    ftruncate(fd, FILE_SIZE);//truncate, extend a file to a specified length

	
    char* send = (char*)calloc(FILE_SIZE, sizeof(char)); //memory
    send = (char*)mmap(NULL, FILE_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);//allocate memory, or map files or devices into memory
	
    if(send == MAP_FAILED)
    {
        printf("mistake in mmap\n"); 
    }
    while(opened != NULL) //all files
    {
        char* name_to_stat = (char*)calloc(200, sizeof(char));
        struct stat* inf = (struct stat*)calloc(1, sizeof(struct stat));
        strcat(name_to_stat, argv[1]);
        strcat(name_to_stat, opened->d_name);
        int res = stat(name_to_stat, inf); //display file status
	
        if(res < 0)
        {
            printf("can't get information\n");
            exit(-1);
        }
        struct passwd* name = (struct passwd*)calloc(1, sizeof(struct passwd));//memory
        name = getpwuid(inf->st_uid); //return information about user from user id
        struct group* grname = (struct group*)calloc(1, sizeof(struct group));//memory
        grname = getgrgid(inf->st_gid); //group inform
		
        int plus = 18 + strlen(intochar(inf->st_nlink)) + 
		strlen(name->pw_name) + strlen(grname->gr_name) + 
		strlen(intochar(inf->st_size)) + 
		strlen(intochar(inf->st_mtime)) + strlen(opened->d_name);
		
        if((inf->st_mode / 16384) % 2)
            strcat(send, "d");
        if((inf->st_mode / 4096) % 2)
            strcat(send, "f");
        if((inf->st_mode / 32768))
            strcat(send, "-");
        if(((inf->st_mode) / 256) % 2)
            strcat(send, "r");
        else
            strcat(send, "-");
        if(((inf->st_mode) / 128) % 2)
            strcat(send, "w");
        else
            strcat(send, "-");
        if(((inf->st_mode) / 64) % 2)
            strcat(send, "x");
        else
            strcat(send, "-");
        if((inf->st_mode / 32) % 2)
            strcat(send, "r");
        else
            strcat(send, "-");
        if(((inf->st_mode) / 16) % 2)
            strcat(send, "w");
        else
            strcat(send, "-");
        if(((inf->st_mode) / 8) % 2)
            strcat(send, "x");
        else
            strcat(send, "-");
        if(((inf->st_mode) / 4) % 2)
            strcat(send, "r");
        else
            strcat(send, "-");
        if(((inf->st_mode) / 2) % 2)
            strcat(send, "w");
        else
            strcat(send, "-");
        if((inf->st_mode) % 2)
            strcat(send, "x");
        else
            strcat(send, "-");
        strcat(send, " ");
		
        strcat(send, intochar(inf->st_nlink));
        strcat(send, " ");
        strcat(send, name->pw_name);
        strcat(send, " ");
        strcat(send, grname->gr_name);
        strcat(send, " ");
        strcat(send, intochar(inf->st_size));
        strcat(send, intochar(inf->st_mtime));
        strcat(send, " ");
        strcat(send, opened->d_name);
        strcat(send, "\n");
        opened = readdir(dirfd); //next file
    }
    closedir(dirfd); 
	
	
    if(munmap((void*)send, FILE_SIZE) < 0)//remove a mapping

    {
        printf("mistake in munmap\n"); //err
        exit(-1);
    }
	
    close(fd);
    kill(0, SIGUSR1);
    free(send);
}
