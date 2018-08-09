#ifndef __TELNET_SERVER_H__
#define __TELNET_SERVER_H__

#include "common.h"

#define TELNET_SERVER_PORT   (8888)  //非默认23号的端口需要使用telnet ip port 访问 eg: telnet 192.168.1.1 1000

#define TELNET_SHOW_COMMAND   "\xff\xfb\x01"
#define NEW_LINE    "\r\n"

#define ISBACKSPACE(x)    ('\b' == x)

#define EDIT_LENGTH     (512)

#define SYS_USERNAME       "yike"
#define SYS_PASSWORD       "ethyn"

#define SYS_CMD_LINE    "cli>"

#define MAX_COUNT_HISTORY  (64)
#define MAX_PROMPT_LEN     (512)

#define TELNET_NODATA_TIMEOUT_MAX  (300)//s

#define ARG_NUM     12          //命令中允许的参数个数
#define CMD_LEN     32         //命令名占用的最大字符长度
#define CMD_BUF_LEN 256         //命令缓存的最大长度
typedef void (*pCallbackFunc)(void* ptr);


#define DECLARE_TELNET_CLI_CMD_MACRO(name,child,Cbfunc,help,usermode)  \
{                                                               \
    #name,                                                      \
    child,                                                      \
    Cbfunc,                                                     \
    #help,                                                      \
    usermode,                                                   \
    CLI_NULL,                                                    \
    CLI_NULL,                                                   \
    CLI_NULL                                                    \
}

#define DECLARE_TELNET_CLI_CMD_MACRO_END()                             \
{                                                               \
    CLI_NULL,                                                   \
    CLI_NULL,                                                   \
    CLI_NULL,                                                   \
    CLI_NULL,                                                   \
    CLI_NULL,                                                   \
    CLI_NULL,                                                    \
    CLI_NULL,                                                   \
    CLI_NULL                                                    \
}


typedef enum DIR_E_CHANGE_MODEtag
{
    DIR_E_IN = 0,
    DIR_E_OUT,
    DIR_E_BUTT
}DIR_EN_CHANGE_MODE;

typedef enum KEY_E_STATUStag
{
    KEY_INPUT_NORMAL,
    KEY_INPUT_DIRECTION,
    KEY_INPUT_DIRECTION_FINAL,
    KEY_INPUT_BUTT
}KEY_E_STATUS;



typedef enum INPUT_EN_MODEtag
{
    INPUT_E_NORMAL=0,
    INPUT_E_HIDE,
    INPUT_E_BUTT
}INPUT_EN_MODE;


typedef enum CURSOR_E_SHIFTtag
{
    CURSOR_EN_NONE = 0,
    CURSOR_EN_LEFT, //左
    CURSOR_EN_RIGHT,  //右
    CURSOR_EN_UP,
    CURSOR_EN_DOWN,
    CURSOR_EN_BACKSPACE,
    CURSOR_EN_BUTT
}CURSOR_E_SHIFT;


typedef struct TELNET_INPUT_DEFtag
{
    UINT32 ulCursorPos;
    KEY_E_STATUS enCurrentStatus;
    KEY_E_STATUS enLastCurrentStatus;
    UINT32 ulEditLen;
    UINT32 ulSendLen;
    INT8 acEditWord[EDIT_LENGTH];
    INT8 acSendWord[SEND_LENGTH];
}TELNET_INPUT_DEF,*TELNET_INPUT_DEF_PTR;

typedef struct TELNET_S_LOGINWORDtag
{
    INT8 acWelcome[1024];
    INT8 acUsername[100];
    INT8 acPassword[100];
    INT8 acLoginSuc[100];
    INT8 acLoginFail[100];
}TELNET_S_LOGINWORD,*TELNET_S_LOGINWORD_PTR;


typedef struct TELNET_CLI_S_COMMANDtag
{
    INT8 *pcName;
    struct TELNET_CLI_S_COMMANDtag *pChildren;
    INT32 (*pCallback)(INT32 argc,char *argv[],void *pvReserve);
    INT8 *pcCmdHelp;
    INT32 i32UserMode;
    struct TELNET_CLI_S_COMMANDtag *next;
    struct TELNET_CLI_S_COMMANDtag *parent;
    struct TELNET_CLI_S_COMMANDtag *prev;
}TELNET_CLI_S_COMMAND,*TELNET_CLI_S_COMMAND_PTR;

typedef struct TELNET_CLI_S_CMD_LISTtag
{
    struct TELNET_CLI_S_COMMANDtag *pstCliCmd;
}TELNET_CLI_S_CMD_LIST,*TELNET_CLI_S_CMD_LIST_PTR;

typedef struct CLI_CMD_S_HISTORYtag
{
    UINT32 u32CurIndex;
    INT8 *pcCur;
    INT8 *pacHistory[MAX_COUNT_HISTORY];
}CLI_CMD_S_HISTORY,*CLI_CMD_S_HISTORY_PTR;

typedef struct CLI_S_CMD_DEFtag
{
    TELNET_CLI_S_COMMAND_PTR pstCliCmdHead;
    INT32 i32SocketId;
    struct TELNET_CLI_S_COMMANDtag *cur;
    INT8 acPrompt[MAX_PROMPT_LEN];
    CLI_CMD_S_HISTORY stCliCmdHistory;
}CLI_S_CMD_DEF,*CLI_S_CMD_DEF_PTR;

extern  int cli_Gethelp(TELNET_CLI_S_COMMAND *pstCliCmdCur,FILE *client);
extern void *TelnetServerThread(void *pArg);
extern void TelnetServerInit(void);
extern CLI_EN_RET KTSocketRecv(INT32 i32SocketId,INT8 *pcStr,IN UINT32 u32Size,void *vReserve,OUT UINT32 *u32Len);
extern int TelnetServerSendToClient(int iSocketId,char *pcBuffer,unsigned int ulSendLen);
extern CLI_EN_RET TelnetCliAnalyzeAndProcessCmd(FILE *client,
                                           TELNET_INPUT_DEF *pstTelnetInputDef,
                                           char *pcCmdString,
                                           UINT32 rec_num,
                                           CLI_S_CMD_DEF *pstCliCmdDef);



#endif //__TELNET_SERVER_H__
