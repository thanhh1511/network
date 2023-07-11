#include <arpa/inet.h>

#define BACKLOG 20 /* Number of allowed connections */
#define DATA_FILE "account.txt"

enum AccountStatus
{
    ACTIVE = 1,
    BLOCKED = 0
};
enum LoginStatus
{
    LOGGED_IN = 1,
    LOGGED_OUT = 0
};



typedef struct Account_t
{
    char username[100];
    char password[50];
    enum AccountStatus status; // account status: active/blocked
    int loginAttempts;
    int point;
} Account;

typedef struct Client_t
{
    int connfd;
    struct sockaddr_in *clientAddr;
    Account *account;
    enum LoginStatus loginStatus;
} Client;

typedef struct Node_t
{
    union
    {
        Account *account;
        Client *client;
    };
    struct Node_t *next;
} Node;

typedef Node *SingleList;

typedef struct question_t
{
    char quest[1000];
    char ans[4][200];
    char answer[10];
} question;

typedef question *list_question;

typedef struct tHandlerArgs_t {
    int connfd;
    struct sockaddr_in *clientAddr;    
} tHandlerArgs;

// Global variables-------------------------------------------------------------
extern SingleList accountList;
extern SingleList clientList;

// Application functions--------------------------------------------------------
// SLL functions
void init();
Node *makeNewNode(void *e);
void makeNullList(SingleList *root);
void insertAtHead(void *e, SingleList *root);
Node *searchAccount(char username[], SingleList root);
void traverseList(SingleList root, char *type);
int challenge(Client *client, SingleList root, int check_pọint);

// Application functions
void *clientHandler(void *arg);
void readDataFile(char filename[]);
void updateDataFile(char filename[]);
void loadQuestion(list_question list_quest, char filename[], int level);
int valueInArray(int val, int arr[], int mark, int level);
char *getMenu();
Node *signIn(Client *client);
void signOut(Client *client);



// Socket functions-------------------------------------------------------------
void startServer(int argc, char const *argv[], int *listenfd, struct sockaddr_in *serverAddr);
struct sockaddr_in * acceptNewConnection(int **connfd, int *sin_size, int *listenfd);
void sendToClient(Client client, char *data, int msgType);
void recvFromClient(Client client, char *buff);
Client *addClient(tHandlerArgs tArgs);
void removeClient(Client *client);






