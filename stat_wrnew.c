/*
 Это та прога, которая пишет в mmap.dat то, что выведет "ls -al"
 */

#include "librdwr.h"

#define FILE_SIZE 1024*1024

int main(int argc, char** argv)
{   
	//kill(1, SIGUSR1);
    DIR *dirfd;
    dirfd = opendir(argv[1]);	// она получает в качестве аргумента путь к директории,
	// в которой сделает ls -al. Она открывает её (заходит туда)
    if(dirfd == NULL) //если не открывается
    {
        printf("Can't open directory\n"); //то всё плохо
        exit(-1);
    }
    struct dirent* opened; // создаём структуру данных dirnet
    opened = readdir(dirfd); // и вызываем readdir, который записывает в эту структуру много
	// разного барахла, но и кое-что, что нам нужно
	
    int fd = open("mmap.dat", O_CREAT | O_RDWR | O_TRUNC, 0666); //открываем/создаём mmap.dat
    if(fd < 0) // если что-то не так
    {
        printf("can't create file\n"); //то кричим об этом
        exit(-1);
    }
    ftruncate(fd, FILE_SIZE);
	/*
	 ftruncate устанавливает длину обычного файла с файловым дескриптором fd в FILE_SIZE байт.
	 Если файл до этой операции был длиннее, то отсеченные данные теряются. Если файл был короче, 
	 то он увеличивается, а добавленная часть заполняется нулевыми байтами.
	 С уважением, ваш man
	 */
    char* send = (char*)calloc(FILE_SIZE, sizeof(char)); //выделяем в оперативке строку размером под размер файла
    send = (char*)mmap(NULL, FILE_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
	/*
	 и торжественно отображаем на оперативку наш файл.
	 По сути, мы просто говорим, что теперь мы ассоциируем раздел оперативной 
	 памяти с файлом. При вызове специальной функции всё, что есть в этом разделе
	 оперативки, будет скопировано в файл. С разделом обращаемся как обычно (в данном
	 случае — как с char'ом)
	 */
    if(send == MAP_FAILED) //если что-то не так
    {
        printf("mistake in mmap\n"); //обижаемся на юзера
    }
    while(opened != NULL) //пока не закончатся файлы в директории, проходимся по ним
    {
        char* name_to_stat = (char*)calloc(200, sizeof(char));
        struct stat* inf = (struct stat*)calloc(1, sizeof(struct stat));
        strcat(name_to_stat, argv[1]);
        strcat(name_to_stat, opened->d_name);
        int res = stat(name_to_stat, inf); //получаем инфу о файле, которая печатается
		//при "ls -al". Вызываем stat, который возвращает
		//структуру, в которой всё это есть
        if(res < 0)
        {  
			perror("can't get information\n");
            exit(-1);
        }
        struct passwd* name = (struct passwd*)calloc(1, sizeof(struct passwd));
        name = getpwuid(inf->st_uid); //но не всё
        struct group* grname = (struct group*)calloc(1, sizeof(struct group));
        grname = getgrgid(inf->st_gid); //нам надо ещё и это
		/*
		 Дальше идёт магия. Нам нужно как-то преобразовать данные из структуры в то, что печатает
		 "ls -al", потому что они в структуре записаны через ж.. не очень удобно для чтения.
		 Для этого лезем в ман, разбироаемся в том, что там стоит, думаем как же нам 
		 "обустроить Рассею" и собираем ужасную конструкцию. Это и вправду жесть.
		 По ходу её выполнения пишем в оперативку (строку send) то, что должно быть в "ls -al".
		 */
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
        opened = readdir(dirfd); //переходим у следующему файлу
    }
    closedir(dirfd); //всё, ура, шлём директорию в сад
	
	/*
	 Теперь надо сделать "мама, роди меня обратно", то есть записать то, что есть в
	 оперативке, назад в файл, то есть "отобразить наоборот". Для этого есть 
	 специальная функция, которую мы и вызовем.
	 */
    if(munmap((void*)send, FILE_SIZE) < 0)
    {
        printf("mistake in munmap\n"); //ругаемся, если не можем
        exit(-1);
    }
	/*
	 Допустим, у нас всё хорошо. Мы всё закрываем и посылаем сигнал второй проге о том, что
	 мы закончили и можно писать в mmap.dat "Work done".
	 */
    close(fd);
    free(send);
	kill(1,SIGUSR1);
}
