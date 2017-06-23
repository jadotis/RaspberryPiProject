#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>


/* GLOBAL VARIABLES */
int regular = 0;
int directory = 0;
int links = 0;
int fifo = 0;
int socket = 0;
int block = 0;
int character = 0;

void getVals(char * pathname);
void printVals();

int main(int argc, char * argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Not enough args... exiting.\n");
        exit(-1);
    }
    else
    {
        char * pathname = argv[1];
        getVals(pathname);
        printVals();
    }
}

void getVals(char * pathname)
{
    DIR *dptr;
    struct dirent *dir;
    struct stat statbuf;
    char * filePointer = (char *)malloc(4096);
    
    if ((dptr = opendir(pathname)) == NULL)
    {
        perror(NULL);
        fprintf(stderr, "Please run as Sudo to open this folder\n");
        exit(-1);
    }
    while ((dir = readdir(dptr)) != NULL)
    {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
        {
            continue;
        }
        if(filePointer)
        {
            memset(filePointer, 0, 4096);
        }
        sprintf(filePointer, "%s%s%s", pathname, "/", dir->d_name);
        if ((stat(filePointer, &statbuf)) < 0)
        {
            break;
        }
        else if (S_ISREG(statbuf.st_mode))
        {
            regular++;
        }
        else if (S_ISDIR(statbuf.st_mode))
        {
            directory++;
            getVals(filePointer);
        }
        else if (S_ISLNK(statbuf.st_mode))
        {
            links++;
        }
        else if (S_ISBLK(statbuf.st_mode))
        {
            block++;
        }
        else if (S_ISCHR(statbuf.st_mode))
        {
            character++;
        }
        else if (S_ISSOCK(statbuf.st_mode))
        {
            socket++;
        }
        else
        {
            continue;
        }
    }
    closedir(dptr);
}


void printVals()
{
    fprintf(stdout, "Regular\t%d\n", regular);
    fprintf(stdout, "Directories\t%d\n", directory);
    fprintf(stdout, "Links\t%d\n", links);
    fprintf(stdout, "Block\t%d\n", block);
    fprintf(stdout, "Character\t%d\n", character);
    fprintf(stdout, "Sockets\t%d\n", socket);
}