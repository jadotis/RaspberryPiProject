#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/uio.h>

#define MAXBUFF 50000000000
#define MAXPATH 1000

int countNumArgs(char * args[])
{
    int counter = 0;
    while(args[counter] != NULL)
    {
        counter++;
    }
    return counter;
}

void handleError(int clientSocket)
{
    char * outbuffer = (char*)malloc(MAXBUFF);
    strcat(outbuffer, "HTTP/1.1 501 Not Implemented\nContent-type: text/plain\n\n");
    strcat(outbuffer, "Viewing this file is not supported.");
    strcat(outbuffer, "\nWe have a highly trained team of monkeys dispatched to solve the issue.");
    write(clientSocket, outbuffer, strlen(outbuffer));
    exit(1);
}

int readIntoBuffer(char * buffer, FILE * file)
{
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);  
    fread(buffer, fsize, 1, file);
    fflush(stdout);
    fclose(file);
    return (int)fsize;
}

void handleImg(char * filePath, int clientSocket)
{
    char * outbuffer = (char *) malloc(MAXBUFF);

    strcat(outbuffer, "HTTP/1.1 200 OK\nContent-type: image/jpeg\n\n");
    FILE * file = fopen(filePath, "r");
    if(file == NULL)
    {
        perror("Dick Error");
        exit(1);
    }
    int size = readIntoBuffer(outbuffer, file);
    //fprintf(stdout, "%s", outbuffer);
    int j = write(clientSocket, outbuffer, size);
    fprintf(stdout, "Bytes Written: %d\n", size);
    fflush(stdout);
    exit(1);
}

void handleDir(char * pathname, int clientSocket)
{
    pid_t child;
    char * writeVal = (char *) malloc(200);
    sprintf(writeVal, "%s%s%s", "HTTP/1.1 200 OK\nContent-type: text/plain\n\nDirectory Listing for ", pathname, ":\n\n");
    write(clientSocket, writeVal, strlen(writeVal));

    child = fork();

    if (child == 0)
    {
        dup2(clientSocket, STDOUT_FILENO);
        char * argv[] = {"ls", pathname, (char *) NULL};
        if ((execvp(argv[0], argv)) < 0)
        {
            perror("Exec Error");
        }
    }

    else
    {
        if ((waitpid(child, NULL, 0)) < 0)
        {
            perror("Error Waiting");
        }
        exit(1);
    }

}

void handleHTML(char * pathname, int clientSocket)
{
    char * outbuffer = (char *) malloc(MAXBUFF);
    char * writeVal = "HTTP/1.1 200 OK\nContent-type: text/html\n\n";

    write(clientSocket, writeVal, strlen(writeVal));

    FILE * file = fopen(pathname, "r");
    if(file == NULL)
    {
        perror("Dick Error");
        exit(1);
    }
    int size = readIntoBuffer(outbuffer, file);
    //fprintf(stdout, "%s", outbuffer);
    int j = write(clientSocket, outbuffer, size);
    fprintf(stdout, "Bytes Written: %d\n", size);
    fflush(stdout);
    exit(1);
}

void handleClock(char * pathname, int clientSocket, char * argv[])
{
    pid_t child;
    char * writeVal = "HTTP/1.1 200 OK\nContent-type: text/html\n\n<head>Attempting to execute your command on the Pi Clock...</head><br>";
    write(clientSocket, writeVal, strlen(writeVal));
    int length = countNumArgs(argv);
    child = fork();
    if(child == 0)
    {   
        char * newPath = malloc(50);
        sprintf(newPath, "%s%s", "./", pathname);
        newPath[strlen(newPath)] = '\0';
        close(STDOUT_FILENO);
        fprintf(stdout, "Pathname: %s\n", newPath);
        fflush(stdout);
        close(clientSocket);
        char * mode = "0777";
        int i = strtol(mode, 0, 8);
        if (chmod(newPath, i) > 0) //Handles the issue if we have a file that cant be CHMOD'd'
        {
            perror("Chmod Error");
        }
        if (length == 1)
        {
            if ((execlp(newPath, newPath, NULL)) < 0)
            {
                perror("Exec Error");
                write(clientSocket, "<body>There was an Error Executing Your Command</body><br>", 58);
                exit(-1);
            }
        }
        else
        {
            if ((execvp(newPath, argv)) < 0)
            {
                perror("Exec Error");
                write(clientSocket, "<body>There was an Error Executing Your Command</body><br>", 58);
                exit(-1);
            }
        }      
    }
    else
    {
        waitpid(child, NULL, 0);
        int flag = 0;
        char * buffer = malloc(100000);
        FILE * errorfile = fopen("errorFile", "r");
        readIntoBuffer(buffer, errorfile);
        char * case1 = strstr(buffer, "Error");
        char * case2 = strstr(buffer, "Errno");
        /*
            Error Checking for Commands sent to the RaspBeryy Pi
        */
        if(strstr(buffer, "Error") == NULL && strstr(buffer,"Errno") == NULL)
        {
            flag = 1;
        }

        if (flag == 1)
        {
            write(clientSocket, "<body>Able to execute your command.</body>", 41);
            unlink("errorFile");
        }
        else
        {
           write(clientSocket, "<body>Unable to execute your command... Please try again.</body>", 64); 
        }
        exit(1);
    }


}

void handleCgi(char * pathname, int clientSocket, char * argv[])
{
    pid_t child;
    char * writeVal = "HTTP/1.1 200 OK\nContent-type: text/plain\n\n";
    write(clientSocket, writeVal, strlen(writeVal));
    int length = countNumArgs(argv);
    int stdError = open("cgi_error.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    child = fork();

    if (child == 0)
    {
        char * newPath = malloc(50);
        sprintf(newPath, "%s%s", "./", pathname);
        newPath[strlen(newPath)] = '\0';
        fprintf(stdout, "Pathname: %s\n", newPath);
        fflush(stdout);
        dup2(clientSocket, STDOUT_FILENO);
        dup2(stdError, STDERR_FILENO);
        char * mode = "0777";
        int i = strtol(mode, 0, 8);
        if (chmod(newPath, i) > 0)
        {
            perror("Chmod Error");
        }
        if (length == 1)
        {
            if ((execlp(newPath, newPath, NULL)) < 0)
            {
                perror("Exec Error");
            }
        }
        else
        {
            if ((execvp(newPath, argv)) < 0)
            {
                perror("Exec Error");
            }
        }
    }

    else
    {
        if ((waitpid(child, NULL, 0)) < 0)
        {
            perror("Error Waiting");
        }
        char * tempBuf = (char *) malloc(MAXBUFF);
        char * lineRead = (char *) malloc(MAXPATH);
        FILE * errorStream = fdopen(stdError, "r");
        int bytesRead = readIntoBuffer(tempBuf, errorStream);
        if (bytesRead != 0)
        {   
            int i = 0;
            while (tempBuf[i] != '\n')
            {
                lineRead[i] = tempBuf[i];
                i++;
            }
            lineRead[i] = '.';
            write(clientSocket, lineRead, strlen(lineRead));
        }
        unlink("cgi_error.txt");
        exit(1);
    }
}