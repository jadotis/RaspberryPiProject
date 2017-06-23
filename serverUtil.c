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
#include "caseUtil.h"

#define MAX_CONNECTIONS 10 
#define MAXPATH 10000000000
#define MAXARG 5000

typedef enum {HTML, IMG, CGI, DIR, ERR, CLOCK} fileType;

/* Creates and Establishes the Server Socket */
int createSocket(int portNum)
{
    int true = 1;
    struct sockaddr_in serverAddr;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int));
    bzero((char*) &serverAddr, sizeof(serverAddr));
    serverAddr.sin_family =  AF_INET;
    serverAddr.sin_port = htons(portNum);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if((bind(fd, (struct sockaddr *) &serverAddr, sizeof(serverAddr))) < 0)
    {
        perror("Bind error");
        close(fd);
        exit(1);
    }
    listen(fd, MAX_CONNECTIONS);
    return fd; // returns the server Socket.
}


int receiveConnection(int serverSocket)
{
    int connectionFlag;
    if((connectionFlag = accept(serverSocket, NULL, NULL)) < 0)
    {
        connectionFlag = -1;
        return connectionFlag;
    }
    return connectionFlag; //connectionFlag is now the sockfd of the new connection
}



char * getCommand(char * buffer, char * argv[]) //needs to support arguments and place subsequent args in a buffer.
{

    char * command = malloc(MAXPATH);
    int counter = 0;
    while(buffer[0] != '/')
    {
        buffer += 1;
    }
    buffer += 1; //Move past the slash
    while(buffer[0] != ' ')
    {
        if (buffer[0] == '%' && buffer[1] == '2' && buffer[2] == '0')
        {
            command[strlen(command)] = ' ';
            buffer += 3;
        }
        else
        {
            command[strlen(command)] = buffer[0];
            buffer += 1;
        }
    }
    
    char * tokbuf = strtok(command, " ");
    argv[0] = tokbuf;
    fprintf(stdout, "Argv[0]: %s\n", argv[0]);
    counter = 1;
    while ((tokbuf = strtok(NULL, " ")) != NULL)
    {
        argv[counter++] = tokbuf;
        fprintf(stdout, "argv[%d]: %s\n", counter - 1, argv[counter - 1]);
    }
    argv[counter] = (char *) NULL;
    fprintf(stdout, "argv[%d], %s\n", counter, argv[counter]);
    return argv[0];
}

fileType getFileExtension(char * command)
{
    char * indic = strstr(command, ".");
    if (indic == NULL)
    {
        return DIR;
    }
    else
    {
        while(*command != '.')
        {
            command++;
        }
        command++;
    }
    if (strcmp(command, "jpg") == 0 || strcmp(command, "jpeg") == 0 || strcmp(command, "gif") == 0)
    {
        return IMG;
    }
    else if (strcmp(command, "html") == 0)
    {
        return HTML;
    }
    else if (strcmp(command, "cgi") == 0)
    {
        return CGI;
    }
    else if(strcmp(command, "clock") == 0)
    {
        return CLOCK;
    }
    else
    {
        return ERR;
    }
}


//The main client communcation.
void clientRelationship(int clientSocket)
{   
    int bytesRead;
    char * buffer = malloc(MAXARG);
    char * command = malloc(MAXARG);
    char * outbuffer = malloc(MAXARG);
    char * argv[MAXARG]; // Initialize an array for the arguments.
    struct stat statbuf;  //used to stat each file.

    if((bytesRead = recv(clientSocket, buffer, MAXARG, 0)) < 0)
    {
        perror("No bytes were read");
        exit(-1);
    }
    else
    {
        command =  getCommand(buffer, argv);
        fprintf(stdout, "The value read in was: %s", buffer);  
        if(stat(command, &statbuf) < 0) // Case for not found
        {
            fprintf(stdout, "File could not be statted and the command was: %s\n", command);
            strcat(outbuffer, "HTTP/1.1 404 Not Found\nContent-type: text/html\n\n");
            strcat(outbuffer, "<head><b><em><font size=\"40\">404 NOT FOUND</font></em></b></head><br>");
            strcat(outbuffer, "<body><font size=\"5\" color=\"green\">Professor West probably knows where it is though...</font></body>");
            write(clientSocket, outbuffer, strlen(outbuffer));
            exit(1);
        }

        else
        {
            fileType type = getFileExtension(command);
            switch (type)
            {
                case DIR:
                    fprintf(stdout, "We have a directory\n");
                    handleDir(command, clientSocket);
                    break;
                case IMG:
                    handleImg(command, clientSocket);
                    break;
                case HTML:
                    handleHTML(command, clientSocket);
                    break;
                case CGI:
                    handleCgi(command, clientSocket, argv);
                    break;
                case CLOCK:
                    handleClock(command, clientSocket, argv);
                    break;
                case ERR:
                    handleError(clientSocket);
                    break;
            }
        }
    }
}