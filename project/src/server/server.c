#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#include "../general/general.h"
#include "server.h"

SingleList accountList;
SingleList clientList;
list_question list_quest_basic, list_quest_medium, list_quest_advanced;
int count[3] = {0};

void init(){
	list_quest_basic = (list_question) calloc (1000 ,sizeof (question));
	list_quest_medium = (list_question) calloc (1000 ,sizeof (question));
	list_quest_advanced = (list_question) calloc (1000 ,sizeof (question));
}

// Single linked list functions------------------------------------------------
Node *makeNewNode(void *e)
{
	Node *newNode = (Node *)malloc(sizeof(Node));

	// check if Node is allocated
	if (newNode == NULL)
	{
		printf("Failed allocating memory for new node!\n");
		return NULL;
	}
	if (e == NULL)
	{
		printf("Invalid element!\n");
		return NULL;
	}

	// check element type
	if (sizeof(*((Account *)e)) == sizeof(Account))
	{
		newNode->account = (Account *)e;
	}
	else if (sizeof(*((Client *)e)) == sizeof(Client))
	{
		newNode->client = (Client *)e;
	}
	else
	{
		printf("Invalid element!\n");
		free(newNode);
		return NULL;
	}

	newNode->next = NULL;
	return newNode;
}

void makeNullList(SingleList *root)
{
	(*root) = NULL;
}

void insertAtHead(void *e, SingleList *root)
{
	Node *newNode = makeNewNode(e);

	if (newNode == NULL)
	{
		printf("Make new node failed!\n");
		return;
	}

	// insert to empty list
	if ((*root) == NULL)
	{
		(*root) = newNode;
		return;
	}

	newNode->next = (*root);
	(*root) = newNode;
}

void deleteAtPosition(SingleList *root, Node *target)
{
	Node *p, *q;

	// delete head
	if (target == (*root))
	{
		(*root) = (*root)->next;
		free(target);
		return;
	}

	// delete other nodes
	for (p = (*root), q = NULL; p != NULL; q = p, p = p->next)
	{
		if (p == target)
		{
			q->next = p->next;
			free(p);
			return;
		}
	}
}

Node *searchAccount(char username[], SingleList root)
{
	// search Account by username
	Node *p;
	for (p = root; p != NULL; p = p->next)
	{
		if (strcmp(p->account->username, username) == 0)
		{
			return p;
		}
	}
	return NULL;
}

// debug function
void traverseList(SingleList root, char *type)
{
	Node *p;

	if (strcmp(type, "client") != 0 && strcmp(type, "account") != 0)
	{
		printf("Invalid type!\n");
		return;
	}

	for (p = root; p != NULL; p = p->next)
	{
		if (strcmp(type, "client") == 0)
		{
			if (p->client->clientAddr == NULL && p->client->account == NULL)
				printf("NULL - NULL\n");
			else if (p->client->clientAddr != NULL && p->client->account == NULL)
				printf("%s:%d - NULL\n", inet_ntoa(p->client->clientAddr->sin_addr), ntohs(p->client->clientAddr->sin_port));
			else if (p->client->clientAddr == NULL && p->client->account != NULL)
				printf("NULL - %s %s %d\n", p->client->account->username, p->client->account->password, p->client->account->status);
			else
				printf("%s:%d - %s %s %d\n", inet_ntoa(p->client->clientAddr->sin_addr), ntohs(p->client->clientAddr->sin_port), p->client->account->username, p->client->account->password, p->client->account->status);
		}
		else
			printf("%s %s %d\n", p->account->username, p->account->password, p->account->status);
	}
	printf("\n");
}

int challenge(Client *client, SingleList root, int check_pọint)
{
	Node *p;
	char string_point[10];
	int count = 0;
	for (p = root; p != NULL; p = p->next)
	{
		if(p->client->account != NULL){
			count++;
		}
		if (p->client->account != NULL && p->client->account != client->account)
		{ 
			printf("%s:%d - %s %s %d\n", inet_ntoa(p->client->clientAddr->sin_addr), ntohs(p->client->clientAddr->sin_port), p->client->account->username, p->client->account->password, p->client->account->status);
			sprintf(string_point,"%d", p->client->account->point);
			sendToClient(*client, string_point, MSG_TYPE_CHALLENGE_SUCESS);
			return 1;
		}
	}
	sendToClient(*client, string_point, 30);
	return 0;
	
}

// read account from file
void readDataFile(char filename[])
{
	char username[100], password[50];
	int status;
	int point;
	FILE *fptr;

	// read data from file
	makeNullList(&accountList);
	if ((fptr = fopen(filename, "r")) == NULL)
	{
		printf("Cannot open file %s\n", filename);
		exit(0);
	}

	while (fscanf(fptr, "%s %s %d %d", username, password, &status, &point) != EOF)
	{
		Account *acc = (Account *)malloc(sizeof(Account));

		strcpy(acc->username, username);
		strcpy(acc->password, password);
		acc->status = status;
		acc->loginAttempts = 0;
		acc->point = point;

		insertAtHead(acc, &accountList);
	}

	fclose(fptr);
}

void updateDataFile(char filename[])
{
	FILE *fptr;
	Node *p;

	// write data to file
	if ((fptr = fopen(filename, "w")) == NULL)
	{
		printf("Cannot open file %s\n", filename);
		exit(0);
	}

	for (p = accountList; p != NULL; p = p->next)
	{
		fprintf(fptr, "%s %s %d %d\n", p->account->username, p->account->password, p->account->status, p->account->point);
	}

	fclose(fptr);
}

void loadQuestion(list_question list, char filename[], int mark){
	count[mark] = 0;
    char buff[20];
    int temp = 0;
    int print = 0;
	int status = 1;
	FILE *fptr;

	// read data from file
	if ((fptr = fopen(filename, "r")) == NULL)
	{
		printf("Cannot open file \n");
		exit(0);
	}

	while (fscanf(fptr, "%s", buff) != EOF)
	{
        if(strcmp(buff, "-") == 0){
            temp++;
        } 
        else if(strcmp(buff, ":") == 0){
            temp = 0;
            print = 1;
			count[mark]++;
        }
        else{
            strcat(buff, " ");
            print = 0;
            switch (temp)
            {
            case 0:
                strcat(list[count[mark]].quest , buff);
                break;
            case 1:
                strcat(list[count[mark]].ans[0], buff);
                break;
            case 2:
                strcat(list[count[mark]].ans[1], buff);
                break;
            case 3:
                strcat(list[count[mark]].ans[2], buff);
                break;
            case 4:
                strcat(list[count[mark]].ans[3], buff);
                break;
            case 5:
                strcpy(list[count[mark]].answer, buff);
                break;
			default:
				break;
            }
        }
	}
	fclose(fptr);
}

int valueInArray(int val, int arr[], int mark, int level){
    int i;
    for(i = mark * 5; i < level; i++){
        if(arr[i] == val) return 1;
    }
    return 0;
}

int randomQuest(Client *client, int set_ques[], int mark, list_question list_quest, int level){
    time_t t;
	srand((unsigned) time(&t));
	char ques[1000] = "Cau ";
	char str_level[4];
	char str_ans[4][200];
	int random;
	strcpy(str_ans[0], "A. ");
	strcpy(str_ans[1], "B. ");
	strcpy(str_ans[2], "C. ");
	strcpy(str_ans[3], "D. ");
	do{
		random = rand() % count[mark];
	} while(valueInArray(random, set_ques, mark, level));
	printf("Random %d\n", random);
	printf("Count: %d\n", count[mark]);
	sprintf(str_level,"%d",level+1);
	strcat(ques, str_level);
	strcat(ques, ": ");
	strcat(ques, list_quest[random].quest);
	strcat(str_ans[0], list_quest[random].ans[0]);
	strcat(str_ans[1], list_quest[random].ans[1]);
	strcat(str_ans[2], list_quest[random].ans[2]);
	strcat(str_ans[3], list_quest[random].ans[3]);
	printf("%s\n", ques);
	printf("%s\n", str_ans[0]);
	printf("%s\n", str_ans[1]);
	printf("%s\n", str_ans[2]);
	printf("%s\n", str_ans[3]);
	printf("Send client %s:%d\n", inet_ntoa((*client).clientAddr->sin_addr), ntohs((*client).clientAddr->sin_port));

	

	//send 1 lan
	sendToClient(*client, ques, MSG_TYPE_QUESTION);
	sendToClient(*client, str_ans[0], MSG_TYPE_A_ANSWER);
	sendToClient(*client, str_ans[1], MSG_TYPE_B_ANSWER);
	sendToClient(*client, str_ans[2], MSG_TYPE_C_ANSWER);
	sendToClient(*client, str_ans[3], MSG_TYPE_D_ANSWER);

    return random;
}

question takeQuestion(int level, int set_ques[]){
	if(level < 5){
		return list_quest_basic[set_ques[level]];
	}
	else if(level < 10){
		return list_quest_medium[set_ques[level]];
	}
	else{
		return list_quest_advanced[set_ques[level]];
	}
}

int checkAnswer(Client *client ,int answer_client, int answer_right, int *level){
	char buff[BUFF_SIZE + 1]; 
	int reply_client;
	char str_level[5];
	if(answer_client == answer_right){
		char str_exact[30] = "So cau dung: ";
		sprintf(str_level,"%d", (*level) + 1);
		strcat(str_exact, str_level);
		sendToClient(*client, str_exact, MSG_TYPE_RIGHT);
		printf("Tra loi dung\n");
		(*level)++;
	}
	else{
		(*client).account->point += (*level);
		char str_exact[30] = "So cau dung: ";
		printf("ok1\n");
		strcat(str_exact, str_level);
		sendToClient(*client, str_exact, MSG_TYPE_MENU_ENDGAME);
		recvFromClient(*client, buff);
		printf("ok2\n");
		reply_client = atoi(buff);
		if(reply_client == Restart_game){
			return Restart_game;
		}
		else{
			return Return_menu;
		}
	}
}

int playGame(Client *client){
	printf("Start\n");
	init();
	loadQuestion(list_quest_basic, "question1.txt", Basic);
	loadQuestion(list_quest_medium, "question2.txt", Medium);
	loadQuestion(list_quest_advanced, "question3.txt", Advanced);
	char buff[BUFF_SIZE + 1];
	char notif_used[100] = "Quyen tro giup da duoc su dung\nVui long chon quyen tro giup khac hoac chon dap an";
	char notif_call[100] = "Dap an chuyen gia dua ra la:";
	char notif_5050[100] = "2 phuong an con lai la:";
	char notif_audience[100] = "Ti le lua chon cua khan gia la:";
	int level = 0;
	question quest;
	time_t t;
	srand((unsigned) time(&t));
	int set_ques[15];
	int answer_help = -1;
	int answer_help_5050_1 = -1;
	int answer_help_5050_2 = -1;
	int reply_client, answer_right;
	int help_call = Help_Available, help_fiftyfifty = Help_Available, help_audience = Help_Available;
	while (1)
	{
		char str_ans_help[4][200];
		char s[200] = "-----------------------------------------------------";
		char help[3][50];
		strcpy(help[0], "4. ");
		strcpy(help[1], "5. ");
		strcpy(help[2], "6. ");
		if(help_call == Help_Available){
			strcat(help[0], "Goi dien thoai cho chuyen gia.");
		}
		if(help_fiftyfifty == Help_Available){
			strcat(help[1], "50/50.");
		}
		if(help_audience == Help_Available){
			strcat(help[2], "Hoi y kien khan gia trong truong quay.");
		}
		// receive answer from client
		sendToClient(*client, s, MSG_TYPE_PROMPT);
		sendToClient(*client, help[0], MSG_TYPE_PROMPT);
		sendToClient(*client, help[1], MSG_TYPE_PROMPT);
		sendToClient(*client, help[2], MSG_TYPE_PROMPT);
		printf("Cau hoi %d:", level + 1);
		if(level < 5){
			set_ques[level] = randomQuest(&*client, set_ques, Basic, list_quest_basic, level);
			answer_right = atoi(list_quest_basic[set_ques[level]].answer);
		} 
		else if(level < 10){
			set_ques[level] = randomQuest(&*client, set_ques, Medium, list_quest_medium, level);
			answer_right = atoi(list_quest_medium[set_ques[level]].answer);
		}
		else{
			set_ques[level] = randomQuest(&*client, set_ques, Advanced, list_quest_advanced, level);
			answer_right = atoi(list_quest_advanced[set_ques[level]].answer);
		}
		RECEIVE_CLIENT: recvFromClient(*client, buff);
		reply_client = atoi(buff);
		strcpy(str_ans_help[0], "A. ");
		strcpy(str_ans_help[1], "B. ");
		strcpy(str_ans_help[2], "C. ");
		strcpy(str_ans_help[3], "D. ");
		if(reply_client < 4 || reply_client == 7){
			if(checkAnswer(&*client, reply_client, answer_right, &level) == Restart_game){
				return Restart_game;
			}
		}
		else
		{
			quest = takeQuestion(level, set_ques);
			switch (reply_client){
			case Help_Call_Professional:
				if(help_call == Help_Available){
					help_call = Help_Used;
					int percent;
					reply_client;
					srand((unsigned) time(&t));
					percent = rand() % 100;
					//80% exactly
					sendToClient(*client, notif_call, MSG_TYPE_PROMPT);
					if(percent > 20){	
						strcat(str_ans_help[answer_right], quest.ans[answer_right]);
						sendToClient(*client, str_ans_help[answer_right], MSG_TYPE_HELP_SUCCESS);
						goto RECEIVE_CLIENT;
					} 
					else{
						if(answer_help_5050_1 > -1){
							answer_help = answer_help_5050_1;
						}
						else{
							do
							{
								answer_help = rand() % 4;
							} while (answer_help == answer_right);
						}
		
						strcat(str_ans_help[answer_right], quest.ans[answer_right]);
						sendToClient(*client, str_ans_help[answer_right], MSG_TYPE_HELP_SUCCESS);
						goto RECEIVE_CLIENT;
					}
				}
				else{
					sendToClient(*client, "NOT", MSG_TYPE_HELP_UNAVAILABLE);
					goto RECEIVE_CLIENT;
				}
				break;
			case Help_Fifty_Fifty:
				if(help_fiftyfifty == Help_Available){
					help_fiftyfifty = Help_Used;
					do
					{
						answer_help_5050_1 = rand() % 4;
					} while (answer_help_5050_1 == answer_right);
					sendToClient(*client, "", MSG_TYPE_A_ANSWER + answer_help_5050_1);
					printf("tro giup 5050: %d\n", MSG_TYPE_A_ANSWER + answer_help_5050_1);
					do
					{
						answer_help_5050_2 = rand() % 4;
					} while (answer_help_5050_2 == answer_right || answer_help_5050_2 == answer_help_5050_1);
					sendToClient(*client, "", MSG_TYPE_A_ANSWER + answer_help_5050_2);
					printf("tro giup 5050: %d\n", MSG_TYPE_A_ANSWER + answer_help_5050_2);
					sendToClient(*client, "ok", MSG_TYPE_HELP_SUCCESS);
					goto RECEIVE_CLIENT;
				}
				else{
					sendToClient(*client, "NOT", MSG_TYPE_HELP_UNAVAILABLE);
					goto RECEIVE_CLIENT;
				}
				break;
			case Help_Ask_Audience:
				if(help_audience == Help_Available){					
					help_audience = Help_Used;
					int j, sum = 0;
					int percentage_audience[4], percentage_answer[4], random_percent;
					float proportion; 
					float level_float = (float)level + 1;
					float mark_float;
					if(level < 5){
						mark_float = (float)Basic + 5;
					} 
					else if(level < 10){
						mark_float = (float)Medium + 9;
					}
					else{
						mark_float = (float)Advanced + 13;
					}
					proportion = mark_float*10/(pow(level, 3)*log(mark_float))*100;
					percentage_audience[0] = (int)proportion;
					for(j = 0; j < 4; j++){
						if(j > 0){
							percentage_audience[j] = 2 * percentage_audience[j-1];
						}
						if((sum + percentage_audience[j]) > 100){
							percentage_audience[j] = 100 - sum;
						}
						sum += percentage_audience[j];
					}
					random_percent = rand() % 100;
					if(random_percent < percentage_audience[0]){
						percentage_answer[answer_right] = 75 + rand() % 25;
					} else if(random_percent < percentage_audience[1]){
						percentage_answer[answer_right] = 50 + rand() % 25;
					} else if(random_percent < percentage_audience[2]){
						percentage_answer[answer_right] = 25 + rand() % 25;
					}	else{
						percentage_answer[answer_right] = rand() % 25;
					}
					int percentage_remain = 100 - percentage_answer[answer_right];
					for(j = 0; j < 4; j++){
						if(j != answer_right){
							percentage_answer[j] = rand() % percentage_remain;
						}
						char str_help_audi[10];
						sprintf(str_help_audi,"%d",percentage_answer[j]);
						strcpy(str_ans_help[j], str_help_audi);
						percentage_remain -= percentage_answer[j];
						if(j < 3){
							sendToClient(*client, str_ans_help[j], MSG_TYPE_HELP_AUDIENCE_A + j);
							printf("client: %d - %s\n", MSG_TYPE_HELP_AUDIENCE_A + j, str_ans_help[j]);
						} else{
							sendToClient(*client, str_ans_help[j], MSG_TYPE_HELP_SUCCESS);
							goto RECEIVE_CLIENT;
						}
					}
					
				}
				else{
					sendToClient(*client, "NOT", MSG_TYPE_HELP_UNAVAILABLE);
					goto RECEIVE_CLIENT;
				}
				break;
			default:
				goto RECEIVE_CLIENT;
				break;
			}
		}

		// viet ham tra ve take_question(int level)
		// ham check dap an dung
				
	}	
}

char *getMenu()
{
	char *menu = (char *)malloc(BUFF_SIZE);

	strcpy(menu, "\nMENU\n");
	strcat(menu, "--------------------------------\n");
	strcat(menu, "1. Play now\n");
	strcat(menu, "2. Sign in\n");
	strcat(menu, "3. Sign up\n");
	strcat(menu, "4. Rank\n");
	strcat(menu, "5. History\n");
	strcat(menu, "Your choice (another to quit): ");

	return menu;
}

Node *signIn(Client *client)
{
	Node *p;
	int i = 0, length_username = 0, length_password = 0;
	char data[BUFF_SIZE + 1];
	char username[BUFF_SIZE];
	char password[BUFF_SIZE];
	// // Ask for username
	// sendToClient(*client, "\nLOGIN\n--------------------------\nUsername: ", MSG_TYPE_CLIENT_INPUT);
	recvFromClient(*client, data);
	while(data[i] != '-'){
		username[length_username] = data[i];
		length_username++;
		i++;
	}
	username[length_username] = '\0';
	i++;
	while(i < strlen(data)){
		password[length_password] = data[i];
		length_password++;
		i++;
	}
	password[length_password] = '\0';
	printf("username: %s\n", username);
	printf("password: %s\n", password);
	p = searchAccount(username, accountList);
	if (p == NULL)
	{
		sendToClient(*client, "Cannot find account\n", MSG_TYPE_ACCOUNT_UNAVAILABLE);
		return NULL;
	}
	else if (p->account->status == BLOCKED)
	{
		sendToClient(*client, "Account is blocked\n", MSG_TYPE_ACCOUNT_BLOCKED);
		return NULL;
	}

	if (strcmp(p->account->password, password) == 0)
	{
		p->account->status = ACTIVE;
		p->account->loginAttempts = 0;
		client->loginStatus = LOGGED_IN;
		client->account = p->account;

		printf("Hello %s, point = %d\n", p->account->username, p->account->point);
		sendToClient(*client, data, MSG_TYPE_LOGIN_SUCCESS);
		return p;
	}

	sendToClient(*client, "Password is incorrect\nIf you enter more than 3 times, account will be locked\n", MSG_TYPE_PASSWORD_INCORRECT);
	p->account->loginAttempts++;
	if (p->account->loginAttempts > 3)
	{
		sendToClient(*client, "Account is blocked\n", MSG_TYPE_WARNING);
		p->account->status = 0;
		p->account->loginAttempts = 0;
	}
	return NULL;
}

void signOut(Client *client)
{
	if (client->loginStatus == LOGGED_OUT)
	{
		sendToClient(*client, "You are not logged in\n", MSG_TYPE_PROMPT);
		return;
	}
	client->loginStatus = LOGGED_OUT;
	client->account = NULL;

	sendToClient(*client, "Logged out successfully\n", MSG_TYPE_PROMPT);
}

void *clientHandler(void *arg)
{
	char buff[BUFF_SIZE + 1];
	tHandlerArgs *tArgs = (tHandlerArgs *)arg;
	char data[BUFF_SIZE + 1];
	// add client to list
	Client *client = addClient(*tArgs);
	free(arg);
	pthread_detach(pthread_self());

	// print client list
	printf("client list:\n");
	traverseList(clientList, "client");
	int reply_client = 0;
	char *menu = getMenu();
	int running = 1;
	int check_game = Return_menu;
	int status_signin = Login_Fail;
	Node *p = NULL;
	while (running)
	{
		sendToClient(*client, menu, MSG_TYPE_CLIENT_INPUT);
		// receive choice from client
		recvFromClient(*client, buff);
		int choice = atoi(buff);

		switch (choice)
		{
		// case 1:
		// 	playGame(&*client);
		// 	break;	
		case 1:
			while(1){
				p = signIn(&*client);
				if(p != NULL){
					break;
				}
				recvFromClient(*client, data);
				if(strcmp(data, "back_menu") == 0){
					printf("back menu\n");
					break;
				}
			}
			updateDataFile(DATA_FILE);
			if(p != NULL){
				while (running)
				{
					sendToClient(*client, menu, MSG_TYPE_CLIENT_INPUT);
					// receive choice from client
					recvFromClient(*client, buff);
					int choice = atoi(buff);
					switch (choice)
					{
					case 1:
						do
						{
							check_game = playGame(&*client);
							printf("check\n");
						} while (check_game == Restart_game);
						
						break;	
					case 2:
						while(reply_client != 1){
							recvFromClient(*client, buff);
							reply_client = atoi(buff);
							if(challenge(&*client, clientList, p->account->point) == 1){
								recvFromClient(*client, buff);
								playGame(&*client);
								
							}
						}						
						break;
					case 3:
						break;
					default:
						running = 0;
						break;
					}
				}
			}
			break;
		case 2:
			
			break;
		case 3:
			break;
		default:
			running = 0;
			break;
		}
	}
	sendToClient(*client, "Goodbye!\n", MSG_TYPE_QUIT);

	// remove client from list
	close(client->connfd);
	removeClient(client);
}

// socket functions ------------------------------------------------------------
void startServer(int argc, char const *argv[], int *listenfd, struct sockaddr_in *serverAddr)
{
	// check arguments
	if (argc != 2)
	{
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	int port = atoi(argv[1]);

	/* Get the Socket file descriptor */
	if (((*listenfd) = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{ /* calls socket() */
		perror("\nError: ");
		exit(0);
	}
	bzero(serverAddr, sizeof(*serverAddr));
	(*serverAddr).sin_family = AF_INET;
	(*serverAddr).sin_port = htons(port);
	(*serverAddr).sin_addr.s_addr = htonl(INADDR_ANY); /* INADDR_ANY puts your IP address automatically */

	/* Information of my machine - the server */
	if (bind((*listenfd), (struct sockaddr *)serverAddr, sizeof(*serverAddr)) == -1)
	{
		perror("\nError: ");
		exit(0);
	}

	/* listen at listenfd with max connection is BACKLOG */
	if (listen((*listenfd), BACKLOG) == -1)
	{
		perror("\nError: ");
		exit(0);
	}
}

struct sockaddr_in *acceptNewConnection(int **connfd, int *sin_size, int *listenfd)
{
	struct sockaddr_in *clientAddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	// accept new connection
	(*connfd) = malloc(sizeof(int));
	if (((**connfd) = accept((*listenfd), (struct sockaddr *)clientAddr, sin_size)) == -1)
		perror("\nError: ");

	printf("You got a connection from %s:%d\n", inet_ntoa((*clientAddr).sin_addr), ntohs((*clientAddr).sin_port));

	return clientAddr;
}

void sendToClient(Client client, char *data, int msgType)
{
	int bytesSent;
	char buff[BUFF_SIZE + 1];

	sprintf(buff, "%d;%s", msgType, data);

	// Send message header
	messageHeader msg = {msgType, strlen(buff)};
	bytesSent = send(client.connfd, &msg, sizeof(messageHeader), 0);
	if (bytesSent < 0)
		perror("\nError: ");

	// Send message content
	bytesSent = send(client.connfd, buff, strlen(buff), 0);
	if (bytesSent < 0)
		perror("\nError: ");
}

void recvFromClient(Client client, char *buff)
{
	int bytes_received;
	bytes_received = recv(client.connfd, buff, BUFF_SIZE, 0);

	if (bytes_received < 0)
		perror("\nError: ");
	else
	{
		buff[bytes_received] = '\0';
		printf("Receive from client %s:%d: %s\n", inet_ntoa(client.clientAddr->sin_addr), ntohs(client.clientAddr->sin_port), buff);
	}
}

Client *addClient(tHandlerArgs tArgs)
{
	Client *client = (Client *)malloc(sizeof(Client));

	client->connfd = tArgs.connfd;
	client->clientAddr = tArgs.clientAddr;
	client->account = NULL;
	client->loginStatus = LOGGED_OUT;
	
	insertAtHead(client, &clientList);

	return client;
}

void removeClient(Client *client)
{
	Node *p;
	for (p = clientList; p != NULL; p = p->next)
	{
		if (p->client == client)
		{
			deleteAtPosition(&clientList, p);
			break;
		}
	}
}
