#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "../general/general.h"
#include "client.h"

int main(int argc, char *argv[])
{
	int clientSock;
	char data[BUFF_SIZE + 1];
	struct sockaddr_in serverAddr; /* server's address information */
	int msgType;

	establishConnection(argc, argv, &clientSock, &serverAddr);

	// Step 4: Communicate with server
	while (1)
	{
		// receive message
		msgType = recvFromServer(clientSock, data);
		if (msgType == MSG_TYPE_CLIENT_INPUT)
		{
			input(data);
			sendToServer(clientSock, data);
		}
		else if (msgType == MSG_TYPE_ERROR || msgType == MSG_TYPE_QUIT)
			break;
	}

	// Step 4: Close socket
	close(clientSock);
	return 0;
}

void establishConnection(int argc, char *argv[], int *clientSock, struct sockaddr_in *serverAddr)
{
	if (argc != 3)
	{
		printf("Usage: %s <server IP> <server port>\n", argv[0]);
		exit(1);
	}
	int serverPort = atoi(argv[2]);
	char *serverIP = argv[1];

	// Step 1: Construct socket
	(*clientSock) = socket(AF_INET, SOCK_STREAM, 0);

	// Step 2: Specify server address
	(*serverAddr).sin_family = AF_INET;
	(*serverAddr).sin_port = htons(serverPort);
	(*serverAddr).sin_addr.s_addr = inet_addr(serverIP);

	// Step 3: Request to connect server
	if (connect((*clientSock), (struct sockaddr *)serverAddr, sizeof(struct sockaddr)) < 0)
	{
		printf("\nCan not connect to sever!");
		exit(0);
	}
}

void sendToServer(int clientSock, char *data)
{
	int bytes_sent = send(clientSock, data, strlen(data), 0);
	if (bytes_sent < 0)
		perror("\nError: ");
}

int recvFromServer(int clientSock, char *data)
{
	messageHeader header;
	recv(clientSock, &header, sizeof(messageHeader), 0); // receive the length and code together

	int total_received = 0;
	while (total_received < header.msgLen)
	{
		int received = recv(clientSock, data + total_received, header.msgLen - total_received, 0);
		if (received == -1)
		{
			perror("Error: ");
			exit(0);
		}
		total_received += received;
	}

	data[total_received] = '\n';
	data[total_received + 1] = '\0';
	int i, msgType;
	for (i = 0; i < total_received; i++)
		if (data[i] == ';')
			break;
	msgType = atoi(data);
	printf("%s", &data[i + 1]);


	return msgType;
}

void input(char *buff)
{
	memset(buff, '\0', BUFF_SIZE);
	fgets(buff, BUFF_SIZE, stdin);
}
