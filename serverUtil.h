typedef enum {HTML, IMG, CGI, DIR} fileType;
extern int createSocket(int portNum);
extern void clientRelationship(int clientSocket);
int receiveConnection(int serverSocket);
fileType getFileExtension(char * command);
char * getCommand(char * buffer, char * argv[]);


