#include <arpa/inet.h>

void establishConnection(int argc, char *argv[], int *clientSock, struct sockaddr_in *serverAddr);
void sendToServer(int clientSock, char *data);
int recvFromServer(int clientSock, char *data);
void input(char *buff);
