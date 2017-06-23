void handleError(int clientSocket);
int readIntoBuffer(char * buffer, FILE * file);
void handleImg(char * filePath, int clientSocket);
void handleDir(char * pathname, int clientSocket);
void handleHTML(char * pathname, int clientSocket);
void handleCgi(char * pathname, int clientSocket, char * argv[]);
int countNumArgs(char * args[]);
