#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#include "../general/general.h"
#include "server.h"

int main(int argc, char const *argv[])
{
	int listenfd, *connfd;
	struct sockaddr_in serverAddr;	
	struct sockaddr_in *clientAddr; 
	int sin_size;
	pthread_t tid;


	startServer(argc, argv, &listenfd, &serverAddr);


	makeNullList(&clientList);
	makeNullList(&accountList);
	readDataFile(DATA_FILE);
	sin_size = sizeof(struct sockaddr_in);
	clientAddr = malloc(sin_size);

	while (1)
	{

		clientAddr = acceptNewConnection(&connfd, &sin_size, &listenfd);


		tHandlerArgs *args = malloc(sizeof(tHandlerArgs));
		args->connfd = *connfd;
		args->clientAddr = clientAddr;
		pthread_create(&tid, NULL, &clientHandler, args);
	}

	close(listenfd);
	return 0;
}
