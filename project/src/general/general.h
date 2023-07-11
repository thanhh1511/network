#define BUFF_SIZE 1024
#define QUIT_QUEUE_ENEMY 1

typedef struct messageHeader_t
{
    int msgType;
    int msgLen;
} messageHeader;

enum MessageType
{
    MSG_TYPE_PROMPT,  
    MSG_TYPE_A_ANSWER,
    MSG_TYPE_B_ANSWER,
    MSG_TYPE_C_ANSWER,
    MSG_TYPE_D_ANSWER,
    MSG_TYPE_QUESTION,
    MSG_TYPE_RIGHT,
    MSG_TYPE_WIN,
    MSG_TYPE_LOSE,
    MSG_TYPE_HELP_SUCCESS,
    MSG_TYPE_HELP_UNAVAILABLE,
    MSG_TYPE_HELP_AUDIENCE_A,
    MSG_TYPE_HELP_AUDIENCE_B,
    MSG_TYPE_HELP_AUDIENCE_C,
    MSG_TYPE_CHALLENGE_SUCESS,
    MSG_TYPE_ACCOUNT_BLOCKED,
    MSG_TYPE_ACCOUNT_UNAVAILABLE,
    MSG_TYPE_PASSWORD_INCORRECT,
    MSG_TYPE_LOGIN_SUCCESS,
    MSG_TYPE_MENU_ENDGAME,
    MSG_TYPE_WARNING,
    MSG_TYPE_CLIENT_INPUT,
    MSG_TYPE_ERROR,
    MSG_TYPE_QUIT
};

enum Login{
    Login_Success,
    Login_Fail,
    Back_Menu
};

enum Help
{
    Help_Call_Professional = 4,
    Help_Fifty_Fifty = 5,
    Help_Ask_Audience = 6
};



enum Check_Help
{
    Help_Used,
    Help_Available

};

enum Mark_Question
{
    Basic,
    Medium, 
    Advanced
};

enum Answer_e
{
    A_answer,
    B_answer, 
    C_answer,
    D_answer
};

enum option_after_playgame
{
    Return_menu,
    Restart_game,
    Time_out
};
