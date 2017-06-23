#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include "serverUtil.h"

void sig_handler(int signo);
void sig_handler2(int signo);

void sig_handler(int signo)
{
    fprintf(stdout,"\nWe recieved a Sig Int Killing the Process\n");
    if (signo == SIGINT)
    {
        exit(0);
    }
}

/* sig_handler2() -- signal handler for SIGTSTP */
void sig_handler2(int signo)
{
    if (signo == SIGTSTP)
    {
        fprintf(stdout, "\nReceievd a SIGSTP, now killing the process\n");
        exit(0);
    }
}


/* MAIN ENTRY POINT */
int main(int argc, char * argv[])
{
    int clientSocket;
    pid_t clientID;
    //Signal Handlers
    if (signal(SIGINT, sig_handler) == SIG_ERR)
    {
        perror("Signal Error:");
        exit(-1);
    }
    if (signal(SIGTSTP, sig_handler2) == SIG_ERR)
    {
        perror("Error killing via signal:");
    }

    if(argc != 2)
    {
        fprintf(stderr, "Missing PortNum or Too many inputs\n");
        exit(1);
    }
    int portNum = atoi(argv[1]); //Retrieve the portNum.
    int serverSocket = createSocket(portNum);   //inherit from serverUtil.h


    for(;;)
    {
        if((clientSocket = receiveConnection(serverSocket)) < 0)
        {
            perror("bad connection");
            exit(1);
        }
        if(clientSocket)
        {
            if((clientID = fork()) < 0)
            {
                perror("Fork Error");
                exit(-1);
            }
            else if(clientID == 0) //We are in the child.
            {
                close(serverSocket);
                clientRelationship(clientSocket);
                close(clientSocket);
                exit(0);
            }
            else
            {
                close(clientSocket);
                clientSocket = -1;
                continue;
            }

        }
        else
        {
            perror("Recieved a bad connection");
            exit(-1);
        }
         close(clientSocket);
    }


}