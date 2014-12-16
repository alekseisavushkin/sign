#pragma once 	
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <grp.h>
#include <signal.h>
#include <pwd.h>
#include <errno.h>
#include <dirent.h>

char* intochar(int num);
