#include "TelnetCliApi.h"
#include "TelnetCmd.h"
#include <sys/prctl.h>  

/*
                   _ooOoo_
                  o8888888o
                  88" . "88
                  (| -_- |)
                  O\  =  /O
               ____/`---'\____
             .'  \\|     |//  `.
            /  \\|||  :  |||//  \
           /  _||||| -:- |||||-  \
           |   | \\\  -  /// |   |
           | \_|  ''\---/''  |   |
           \  .-\__  `-`  ___/-. /
         ___`. .'  /--.--\  `. . __
      ."" '<  `.___\_<|>_/___.'  >'"".
     | | :  `- \`.;`\ _ /`;.`/ - ` : | |
     \  \ `-.   \_ __\ /__ _/   .-` /  /
======`-.____`-.___\_____/___.-`____.-'======
                   `=---='
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
         佛祖保佑       永无BUG

*********************************************
*            Made by Ethyn                  *
*********************************************
*/

TELNET_S_LOGINWORD stTelnetLoginWord;




static TELNET_CLI_S_COMMAND cli_cmd_list[]={
    {"help",    CLI_NULL,    CLI_NULL,"get help,show cmd list",0,CLI_NULL},
    {"ls",    CLI_NULL,    CLI_NULL,  "show cmd list",0,CLI_NULL},
    {"history", CLI_NULL,       CLI_NULL,       "get history cmd",0,CLI_NULL},
    {CLI_NULL, CLI_NULL,       CLI_NULL,       CLI_NULL,0,CLI_NULL}
};


TELNET_CLI_S_CMD_LIST_PTR g_pstCliCmdList=CLI_NULL;

int ShowCmd(TELNET_CLI_S_COMMAND *pstCliCmd,FILE *fp)
{
    while(pstCliCmd)
    {
        if(pstCliCmd->pcName)
        {
            fprintf(fp, "%-20s%s\r\n",pstCliCmd->pcName,pstCliCmd->pcCmdHelp);
        }
        #if 0
        if(pstCliCmd->pChildren)
        {
            fprintf(fp, "---sub-->");
            ShowCmd(pstCliCmd->pChildren,fp);
        }
        #endif
        pstCliCmd = pstCliCmd->next;
    }
}

int cli_Gethelp(TELNET_CLI_S_COMMAND *pstCliCmdCur,FILE *client)
{
    fprintf(client, "Commands available:\r\n");

    ShowCmd(pstCliCmdCur,client);

    
   
   return 0;
}


INT32 ProcessRegisterTelnetSubCmd(TELNET_CLI_S_COMMAND *pstTelnetCmd,TELNET_CLI_S_COMMAND *parent)
{
    int i;
    TELNET_CLI_S_COMMAND *pstTelnetCmdTmp = pstTelnetCmd;
    if(CLI_NULL == pstTelnetCmdTmp)
    {
        return -1;
    }

    if(CLI_NULL == pstTelnetCmdTmp->pcName)
    {
        return -1;
    }
#if 0
    if(pstTelnetCmdTmp->pChildren)
    {
        ProcessRegisterTelnetSubCmd(pstTelnetCmdTmp->pChildren,pstTelnetCmdTmp);
    }
#endif
    for(i = 0; pstTelnetCmdTmp[i+1].pcName; i++)
    {
        pstTelnetCmdTmp[i].next = &pstTelnetCmdTmp[i+1];
        pstTelnetCmdTmp[i].parent = parent;
        if(i != 0)
        {
             pstTelnetCmdTmp[i].prev = &pstTelnetCmdTmp[i-1];
        }
        if(pstTelnetCmdTmp[i].pChildren)
        {
            ProcessRegisterTelnetSubCmd(pstTelnetCmdTmp[i].pChildren,&pstTelnetCmdTmp[i]);
        }
    }

    pstTelnetCmdTmp[i].next = CLI_NULL;
    if(i != 0)
    {
        pstTelnetCmdTmp[i].prev = &pstTelnetCmdTmp[i-1];
    }
    pstTelnetCmdTmp[i].parent = parent;
    if(pstTelnetCmdTmp[i].pChildren)
    {
        ProcessRegisterTelnetSubCmd(pstTelnetCmdTmp[i].pChildren,&pstTelnetCmdTmp[i]);
    }
    
    return CLI_OK;
}


INT32 RegisterCliCommand(/*const*/ void *pstCliCmd)
{
    int i = 0;
    TELNET_CLI_S_COMMAND *pstChildCliCmd;
    TELNET_CLI_S_CMD_LIST_PTR pstCliCmdlist;
    TELNET_CLI_S_CMD_LIST_PTR pstCliCmdEmptyPoint;
    TELNET_CLI_S_COMMAND *pstCliCmdTmp;
    CHECK_PTR_RET(pstCliCmd,CLI_EN_PARAM_INVALID);
    if(CLI_NULL == g_pstCliCmdList)
    {
        g_pstCliCmdList =(TELNET_CLI_S_CMD_LIST_PTR)malloc(sizeof(TELNET_CLI_S_CMD_LIST));
        if(CLI_NULL == g_pstCliCmdList)
        {
            return CLI_EN_MALLOC_FAILD;
        }
        
        g_pstCliCmdList->pstCliCmd = cli_cmd_list;
        ProcessRegisterTelnetSubCmd(cli_cmd_list,CLI_NULL);
         
    }
    pstCliCmdTmp = (TELNET_CLI_S_COMMAND *)pstCliCmd;
    
    pstChildCliCmd = g_pstCliCmdList->pstCliCmd;

    while(pstChildCliCmd->next)
    {
        pstChildCliCmd = pstChildCliCmd->next;
    }
    
    if(CLI_OK == ProcessRegisterTelnetSubCmd(pstCliCmdTmp,CLI_NULL))
    {
        pstChildCliCmd->next = pstCliCmdTmp;
        pstCliCmdTmp->prev = pstChildCliCmd;
    }
    

    return CLI_OK;
        
}


CLI_EN_RET TelnetCliChangeDirectoryName(CLI_S_CMD_DEF *pstCliCmdDefParam,TELNET_CLI_S_COMMAND *pstCliCmdCur,DIR_EN_CHANGE_MODE eMode)
{
    //参数检测外面要做好
    TELNET_CLI_S_COMMAND *pCur = pstCliCmdCur;
    INT8 *pPrompt = pstCliCmdDefParam->acPrompt;
    INT8 *pPromptTmp = CLI_NULL;
    if(eMode >= DIR_E_BUTT)
    {
        printf("eMode is invalid,eMode=%d\r\n",eMode);
        return CLI_EN_PARAM_INVALID;
    }
    if(DIR_E_IN == eMode)
    {
        if((strlen(pPrompt)+strlen(pCur->pcName)+1) > MAX_PROMPT_LEN)
        {
            printf("Directory is too long,please don't add too many sub directory\r\n");
            return CLI_EN_NOT_SUPPORT;
        }
        pPromptTmp = &pPrompt[strlen(pPrompt) -1];
        pPromptTmp[strlen(pCur->pcName)+1] ='>';
        pPromptTmp[0] ='.';
        memcpy(&pPromptTmp[1],pCur->pcName,strlen(pCur->pcName));
        
    }
    else
    {
        if(CLI_NULL == pCur->parent)
        {
            memset(pPrompt,0,sizeof(strlen(pPrompt)));
            memcpy(pPrompt,"cli>",strlen("cli>"));
            return CLI_OK;
        }
        
        pPromptTmp = &pPrompt[strlen(pPrompt) -1-strlen(pCur->parent->pcName)-1];
        pPromptTmp[0] = '>';
        memset(&pPromptTmp[1],0,strlen(&pPromptTmp[1]));
    }
    

    return CLI_OK;
}


void TelnetCmdFindCmdNode(const char *acString, TELNET_CLI_S_COMMAND *pstCurCmdlistNode,TELNET_CLI_S_COMMAND **ppstCliCmdToFindNode)
{
    CHECK_PTR_VOID(acString);
    CHECK_PTR_VOID(pstCurCmdlistNode);
    unsigned int len =0;
    const char *pacStrTmp = acString;
    BOOL bFound = FALSE;
    DEBUG(printf("<%s,%d>IN0:%s\r\n",__FUNCTION__,__LINE__,acString));
    while((*pacStrTmp != KEY_DOT)&&(*pacStrTmp != CLI_NULL))
    {
        pacStrTmp++;
        len++;
    }
    if(*pacStrTmp == KEY_DOT)
    {
        pacStrTmp++;
    }
    
    if(len)
    {
        while(pstCurCmdlistNode)
        {
            DEBUG(printf("check name:%s,cur[%d]:%s\r\n",pArgv[0],i++,pstCurCmdlistNode->pcName));
            if(!strncmp(acString,pstCurCmdlistNode->pcName,len))   
            {
                bFound = TRUE;
                break;
            }
            pstCurCmdlistNode = pstCurCmdlistNode->next;
        }

    }

    if(bFound)
    {
        *ppstCliCmdToFindNode = pstCurCmdlistNode;
        if((CLI_NULL != pstCurCmdlistNode->pChildren) && (CLI_NULL != pstCurCmdlistNode))
        {   
            TelnetCmdFindCmdNode(pacStrTmp,pstCurCmdlistNode->pChildren,ppstCliCmdToFindNode);
        }
        
    }

    return;
}

/*命令行分析任务*/
CLI_EN_RET TelnetCliAnalyzeAndProcessCmd(FILE *client,
                                           TELNET_INPUT_DEF *pstTelnetInputDef,
                                           char *pcCmdString,
                                           UINT32 rec_num,
                                           CLI_S_CMD_DEF *pstCliCmdDef)
{
    UINT32 i;
    CLI_EN_RET enRet = CLI_EN_NOT_SUPPORT;
    UINT32 u32Argc = 0;
    struct TELNET_CLI_S_COMMANDtag *pstCliCmd = pstCliCmdDef->cur;
    struct TELNET_CLI_S_COMMANDtag **ppstCliCmdCur = &pstCliCmdDef->cur;
    struct TELNET_CLI_S_COMMANDtag *pstCliCmdFindNode = CLI_NULL;
    char *pArgv[CMD_LEN]={0};
    char *save_ptr;
    memset(pArgv,CLI_NULL,sizeof(char *) * CMD_LEN);
    
    while(pArgv[u32Argc] = strtok_r(pcCmdString," ",&save_ptr))
    {
        u32Argc++;
        if(u32Argc >= CMD_LEN) 
        {
            break;
        }
        pcCmdString = CLI_NULL;  //important
    }

    if(!u32Argc)
    {
        return CLI_OK;//no cmd
    }

    if(!strcmp(pArgv[0],"cd") && (2 == u32Argc))
    {

        if(!strcmp(pArgv[1],".."))
        {
            if(CLI_NULL != pstCliCmd->parent)
            {
                if(CLI_OK != TelnetCliChangeDirectoryName(pstCliCmdDef,pstCliCmd,DIR_E_OUT))
                {
                    return CLI_EN_NOT_SUPPORT;
                }
                pstCliCmd = pstCliCmd->parent;
                while(pstCliCmd->prev)
                {
                    pstCliCmd = pstCliCmd->prev;
                }
                *ppstCliCmdCur = pstCliCmd;
            }
            return CLI_OK;
        }
        while(pstCliCmd)
        {
            DEBUG(printf("cd check name:%s,cur[%d]:%s\r\n",pArgv[1],i++,pstCliCmd->pcName));
            if(!strcmp(pArgv[1],pstCliCmd->pcName)
                &&(CLI_NULL != pstCliCmd->pChildren)) 
            {
                if(CLI_OK != TelnetCliChangeDirectoryName(pstCliCmdDef,pstCliCmd,DIR_E_IN))
                {
                    return CLI_EN_NOT_SUPPORT;
                }
                *ppstCliCmdCur = pstCliCmd->pChildren;
                return CLI_OK;
            }
            pstCliCmd = pstCliCmd->next;
        }
        fprintf(client, "no directory:%s\r\n",pArgv[1]);
        return CLI_EN_NOT_SUPPORT;
    }

    
    DEBUG(printf("pArgv:%s\r\n",pArgv[0]));

    if(!strcmp(pArgv[0],"history"))
    {
        fprintf(client, "\r\nCommand history:\r\n");
        for (i = 0; i < MAX_COUNT_HISTORY; i++)
        {
            if (pstCliCmdDef->stCliCmdHistory.pacHistory[i])
                fprintf(client, "%3d. %s\r\n", i, pstCliCmdDef->stCliCmdHistory.pacHistory[i]);
        }
        return CLI_OK;
    }

    if(!strcmp(pArgv[0],"quit"))
    {
        
        return CLI_EN_CLI_QUIT;
    }

    if((!strcmp(pArgv[0],"help"))|| (!strcmp(pArgv[0],"ls"))
        || (!strcmp(pArgv[0],"?")))       
    {
        cli_Gethelp(pstCliCmd,client);
        return CLI_OK;
    }


    TelnetCmdFindCmdNode(pArgv[0],pstCliCmd,&pstCliCmdFindNode);
    if(CLI_NULL != pstCliCmdFindNode)
    {
        if(CLI_NULL != pstCliCmdFindNode->pCallback)
        {
            pstCliCmdFindNode->pCallback(u32Argc,pArgv,(void *)client);
            return CLI_OK;
        }
    }

    #if 0
    i = 0;
    //遍历所有命令节点
    while(pstCliCmd)
    {
        DEBUG(printf("check name:%s,cur[%d]:%s\r\n",pArgv[0],i++,pstCliCmd->pcName));
        if((!strcmp(pArgv[0],pstCliCmd->pcName))
            &&(CLI_NULL != pstCliCmd->pCallback))       
        {
            DEBUG(printf("find_name:%s\r\n",pstCliCmd->pcName));
            pstCliCmd->pCallback(u32Argc,pArgv,(void *)client);
            return CLI_OK;
            
        }
        pstCliCmd = pstCliCmd->next;
    }
    #endif
    fprintf(client, "no cmd:%s\r\n",pArgv[0]);
   return enRet;
}


void TelnetCliFree(CLI_S_CMD_DEF **p)
{
    int i;
    for (i = 0; i < MAX_COUNT_HISTORY; i++)
    {
        if((*p)->stCliCmdHistory.pacHistory[i])
        {
            FREE_MEM((*p)->stCliCmdHistory.pacHistory[i]);
        }
        else
        {
            break;
        }
    }
    FREE_MEM(*p);
}


INT32 TelnetCliAddHistory(CLI_S_CMD_DEF *pstCliCmdDef, char *cmd)
{
    int i;
    DEBUG(printf("Add history cmd:%s\r\n",cmd));
    for (i = 0; i < MAX_COUNT_HISTORY; i++)
    {
        if (!pstCliCmdDef->stCliCmdHistory.pacHistory[i])
        {
            pstCliCmdDef->stCliCmdHistory.pacHistory[i] = strdup(cmd);
            pstCliCmdDef->stCliCmdHistory.pcCur = pstCliCmdDef->stCliCmdHistory.pacHistory[i];
            pstCliCmdDef->stCliCmdHistory.u32CurIndex = i;
            return CLI_OK;
        }
    }
    FREE_MEM(pstCliCmdDef->stCliCmdHistory.pacHistory[0]);
    for (i = 0; i < MAX_COUNT_HISTORY; i++)
    {
        pstCliCmdDef->stCliCmdHistory.pacHistory[i] = pstCliCmdDef->stCliCmdHistory.pacHistory[i+1];
    }
    pstCliCmdDef->stCliCmdHistory.pacHistory[MAX_COUNT_HISTORY - 1] = strdup(cmd);
    pstCliCmdDef->stCliCmdHistory.pcCur = pstCliCmdDef->stCliCmdHistory.pacHistory[MAX_COUNT_HISTORY - 1];
    pstCliCmdDef->stCliCmdHistory.u32CurIndex = MAX_COUNT_HISTORY - 1;
    return CLI_OK;
}


/***********************************************************************************************
*函数名 ： TelnetServerInit
*函数功能描述 ： TelnetServer初始化
*函数参数 ： 无
*函数返回值 ： 无
*作者 ：易科
*函数创建日期 ： 2016.08.10
*函数修改日期 ： 
*修改人 ：
*修改原因 ： 
*版本 ： 
*历史版本 ： 
***********************************************************************************************/
void TelnetServerInit(void)
{
    memset(&stTelnetLoginWord,0,sizeof(stTelnetLoginWord));

    sprintf(stTelnetLoginWord.acWelcome,
    "**********************************************\r\n"
    "*                   Welcome                  *\r\n"
    "*               Author: Yi   Ke              *\r\n"
    "*               Q Q: 1029928586              *\r\n"
    "*               TEL:13828735950              *\r\n"
    "*        UserName:yike  Password:ethyn       *\r\n"
    "**********************************************\r\n");

    sprintf(stTelnetLoginWord.acUsername,"                UserName:");
    sprintf(stTelnetLoginWord.acPassword,"                Password:");
    sprintf(stTelnetLoginWord.acLoginSuc,"check the User Name and Password... success.\r\n");
    sprintf(stTelnetLoginWord.acLoginFail,"check the User Name and Password ... failed.\r\n");

}

/***********************************************************************************************
*函数名 ： TelnetShowCmdInfo
*函数功能描述 ： 显示telnetServer端对接收命令的关键信息，方便调试
*函数参数 ： TELNET_INPUT_DEF *pstTelnetCmd 
*函数返回值 ： 无
*作者 ：易科
*函数创建日期 ： 2016.08.10
*函数修改日期 ： 
*修改人 ：
*修改原因 ： 
*版本 ： 
*历史版本 ： 
***********************************************************************************************/
void TelnetShowCmdInfo(TELNET_INPUT_DEF *pstTelnetCmd)
{
    CHECK_PTR_VOID(pstTelnetCmd);
    pstTelnetCmd = pstTelnetCmd;

    printf("pstTelnetCmd->acEditWord:%s\r\n",pstTelnetCmd->acEditWord);
    printf("pstTelnetCmd->ulEditLen:%d\r\n",pstTelnetCmd->ulEditLen);

    printf("pstTelnetCmd->acSendWord:%s\r\n",pstTelnetCmd->acSendWord);
    printf("pstTelnetCmd->ulSendLen:%d\r\n",pstTelnetCmd->ulSendLen);

    printf("pstTelnetCmd->ulCursorPos:%d\r\n",pstTelnetCmd->ulCursorPos);
    printf("pstTelnetCmd->enCurrentStatus:%d\r\n",pstTelnetCmd->enCurrentStatus);
    printf("pstTelnetCmd->enLastCurrentStatus:%d\r\n",pstTelnetCmd->enLastCurrentStatus);

    return ;
}

/***********************************************************************************************
*函数名 ： TelnetServerSendToClient
*函数功能描述 ： 发送给客户端的数据
*函数参数 ： iSocketId 与客户端连接的socketID
             pcBuffer 要发送的数据
             ulSendLen 要发送的长度
*函数返回值 ： 无
*作者 ：易科
*函数创建日期 ： 2016.08.10
*函数修改日期 ： 
*修改人 ：
*修改原因 ： 
*版本 ： 
*历史版本 ： 
***********************************************************************************************/
int TelnetServerSendToClient(int iSocketId,char *pcBuffer,unsigned int ulSendLen)
{

    unsigned int ulLength = 0;
    int i,iRet;
    fd_set   wfds; 
    struct timeval tv;
    INT32 i32Retval;
    CHECK_PTR_RET(pcBuffer,SYS_ERROR);

    FD_ZERO(&wfds);
    FD_SET(iSocketId, &wfds);
    tv.tv_sec = 0;
    tv.tv_usec = 50000;

    i32Retval = select(iSocketId+1, NULL, &wfds, NULL, &tv);
    if (i32Retval == -1)
    {
        perror("select()");
    }
    else if (i32Retval) /* FD_ISSET(0, &rfds) will be true. */
    {
        DEBUG(printf("Data can be send now.i32RetVal = %d\r\n",i32Retval));
    }
    else
    {
        DEBUG(printf("No data can write within 50ms.i32RetVal = %d\r\n",i32Retval));
        return  CLI_EN_ERROR_TIME_OUT;
    }
   

    ulLength =  send(iSocketId, pcBuffer, ulSendLen, MSG_DONTWAIT);
    if(ulLength >= 0)
    {
        iRet = (int)ulLength;
    }


    return iRet;
}


/***********************************************************************************************
*函数名 ： TelnetServerMoveClientCursor
*函数功能描述 ： 移动客户端的光标
*函数参数 ： TELNET_INPUT_DEF *pstTelnetCmd
			 CURSOR_E_SHIFT enShift
*函数返回值 ： 无
*作者 ：易科
*函数创建日期 ： 2016.08.10
*函数修改日期 ： 
*修改人 ：
*修改原因 ： 
*版本 ： 
*历史版本 ： 
***********************************************************************************************/
void TelnetServerMoveClientCursor(TELNET_INPUT_DEF *pstTelnetCmd,CURSOR_E_SHIFT enShift)
{
    unsigned int ulDeltLen;
    unsigned int ulCurrentPos;
    unsigned int ulDelt;
    CHECK_PTR_VOID(pstTelnetCmd);
    char cMoveLeftPos[]={'\x1b','\x5b','\x30','\x44'};

    if(enShift >= CURSOR_EN_BUTT)
    {
        DEBUG(printf("enShift  error\r\n"));
        return ;
    }

    ulDeltLen = pstTelnetCmd->ulEditLen - pstTelnetCmd->ulCursorPos;

    if(CURSOR_EN_NONE == enShift)
    {
        if(pstTelnetCmd->ulCursorPos == pstTelnetCmd->ulEditLen)
        {
            strncat(pstTelnetCmd->acSendWord,"\x1b\x5b\x30\x40",4); //相等则往后移一位，在最后添加 ESC [ 0 C
            pstTelnetCmd->ulSendLen += 4;
        }
        else if(pstTelnetCmd->ulEditLen > pstTelnetCmd->ulCursorPos)
        {
            ulCurrentPos = pstTelnetCmd->ulCursorPos;

            strncpy(pstTelnetCmd->acSendWord+strlen(pstTelnetCmd->acSendWord),
            &pstTelnetCmd->acEditWord[ulCurrentPos],
            pstTelnetCmd->ulEditLen - ulCurrentPos);
            pstTelnetCmd->ulSendLen +=  pstTelnetCmd->ulEditLen - ulCurrentPos;
            cMoveLeftPos[2] = '0'+ pstTelnetCmd->ulEditLen - ulCurrentPos;
            strncat(pstTelnetCmd->acSendWord,cMoveLeftPos,4); //相等则往前移位，在最后添加 ESC [ 0 C
            pstTelnetCmd->ulSendLen += 4;
        }
    }
    else if(CURSOR_EN_BACKSPACE == enShift)
    {
        ulCurrentPos = pstTelnetCmd->ulCursorPos;
        if(ulDeltLen)
        {
            strncpy(pstTelnetCmd->acSendWord+strlen(pstTelnetCmd->acSendWord),
                        &pstTelnetCmd->acEditWord[ulCurrentPos],
                        pstTelnetCmd->ulEditLen - ulCurrentPos);
            pstTelnetCmd->ulSendLen +=  pstTelnetCmd->ulEditLen - ulCurrentPos;

        }

        pstTelnetCmd->acSendWord[pstTelnetCmd->ulSendLen] = 0x20;//空字符

        cMoveLeftPos[2] = '0'+ pstTelnetCmd->ulEditLen - ulCurrentPos +1;
        strncpy(pstTelnetCmd->acSendWord+strlen(pstTelnetCmd->acSendWord),cMoveLeftPos,4); 
            pstTelnetCmd->ulSendLen = strlen(pstTelnetCmd->acSendWord);
    }
    else
    {
        switch(enShift)
        {
            case CURSOR_EN_LEFT:
            {
                strncat(pstTelnetCmd->acSendWord,"\x1b\x5b\x44",3); //相等则往后移一位，在最后添加 ESC [ 0 C
                pstTelnetCmd->ulSendLen += 3;
                break;
            }
            case CURSOR_EN_RIGHT:
            {
                strncat(pstTelnetCmd->acSendWord,"\x1b\x5b\x43",3); //相等则往后移一位，在最后添加 ESC [ 0 C
                pstTelnetCmd->ulSendLen += 3;
                break;
            }
            case CURSOR_EN_UP:
            {
                strncat(pstTelnetCmd->acSendWord,"\x1b\x5b\x41",3); //相等则往后移一位，在最后添加 ESC [ 0 C
                pstTelnetCmd->ulSendLen += 3;
                break;
            }
            case CURSOR_EN_DOWN:
            {
                strncat(pstTelnetCmd->acSendWord,"\x1b\x5b\x42",3); //相等则往后移一位，在最后添加 ESC [ 0 C
                pstTelnetCmd->ulSendLen += 3;
                break;
            }

            default:
                break;

        }
    }

    return ;
}

/***********************************************************************************************
*函数名 ： TelnetServerAddaWord
*函数功能描述 ： 将输入的字符添加到pstTelnetCmd
*函数参数 ： char cWord 待处理的字符
            TELNET_INPUT_DEF *pstTelnetCmd
*函数返回值 ： 1或0
*作者 ：易科
*函数创建日期 ： 2017.03.17
*函数修改日期 ： 
*修改人 ：
*修改原因 ： 
*版本 ： 
*历史版本 ： 
***********************************************************************************************/
void TelnetServerAddaWord(char cWord,TELNET_INPUT_DEF *pstTelnetCmd)
{
    unsigned int ulDelt = 0;
    
    pstTelnetCmd->ulEditLen = strlen(pstTelnetCmd->acEditWord);

        if(pstTelnetCmd->ulEditLen > pstTelnetCmd->ulCursorPos)
        {
            ulDelt = pstTelnetCmd->ulEditLen - pstTelnetCmd->ulCursorPos;
            while(ulDelt--)
            {
                pstTelnetCmd->acEditWord[pstTelnetCmd->ulCursorPos+ ulDelt + 1] = pstTelnetCmd->acEditWord[pstTelnetCmd->ulCursorPos+ ulDelt];
            }
        }

        DEBUG(printf("<%s><%d>:pstTelnetCmd->ulCursorPos[%d] is going to add\r\n",
               __FUNCTION__,__LINE__,pstTelnetCmd->ulCursorPos));
        pstTelnetCmd->acEditWord[pstTelnetCmd->ulCursorPos++] = cWord;

        pstTelnetCmd->ulEditLen = strlen(pstTelnetCmd->acEditWord);

        pstTelnetCmd->acSendWord[pstTelnetCmd->ulSendLen++] = cWord;

        return;
}

void TelnetServerHistoryCmd(TELNET_INPUT_DEF *pstTelnetCmd,CLI_CMD_S_HISTORY_PTR pstCliCmdHistory)
{
    unsigned int ulTmp = 0;
    unsigned int i;
    char *pTmp;
    char cCleanWord[]={'\x1b','\x5b','\x30','\x44'};

    if(CLI_NULL != pstCliCmdHistory->pcCur)
    {
        memset(pstTelnetCmd->acSendWord,0,sizeof(SEND_LENGTH * sizeof(INT8)));
        if(pstTelnetCmd->ulCursorPos)//位置为0时不能添加，否则仍会移动向前移动一位
        {
            cCleanWord[2] = '0'+pstTelnetCmd->ulCursorPos;
            memcpy(pstTelnetCmd->acSendWord,cCleanWord,4);
        }
        memcpy(pstTelnetCmd->acSendWord+ strlen(pstTelnetCmd->acSendWord),pstCliCmdHistory->pcCur,strlen(pstCliCmdHistory->pcCur));
        pstTelnetCmd->ulSendLen = strlen(pstTelnetCmd->acSendWord);
        memset(pstTelnetCmd->acEditWord,0,sizeof(EDIT_LENGTH * sizeof(INT8)));
        memcpy(pstTelnetCmd->acEditWord,pstCliCmdHistory->pcCur,strlen(pstCliCmdHistory->pcCur));
        if(pstTelnetCmd->ulEditLen > strlen(pstCliCmdHistory->pcCur))
        {
            ulTmp = pstTelnetCmd->ulEditLen - strlen(pstCliCmdHistory->pcCur);
            pTmp = pstTelnetCmd->acSendWord+ strlen(pstTelnetCmd->acSendWord);
            for(i = 0; i < ulTmp;i++)
            {
                pTmp[i] = 0x20;
            }
            
            cCleanWord[2] = '0'+ulTmp;
            memcpy(pstTelnetCmd->acSendWord+ strlen(pstTelnetCmd->acSendWord),cCleanWord,4);
        }   
        pstTelnetCmd->ulEditLen = strlen(pstTelnetCmd->acEditWord);
        pstTelnetCmd->ulSendLen= strlen(pstTelnetCmd->acSendWord);
        pstTelnetCmd->ulCursorPos =  strlen(pstCliCmdHistory->pcCur);
    }
}

/***********************************************************************************************
*函数名 ： TelnetServerProcessSingleChar
*函数功能描述 ： 处理单个字符，返回1表示命令还没有处理完，返回0表示输入了回车键，命令处理完毕
*函数参数 ： char cWord 待处理的字符
            TELNET_INPUT_DEF *pstTelnetCmd
*函数返回值 ： 1或0
*作者 ：易科
*函数创建日期 ： 2016.08.10
*函数修改日期 ： 
*修改人 ：
*修改原因 ： 
*版本 ： 
*历史版本 ： 
***********************************************************************************************/
int TelnetServerProcessSingleChar(char cWord,TELNET_INPUT_DEF *pstTelnetCmd,CLI_S_CMD_DEF *pstCliCmdCurDef)
{

    unsigned int ulEditLen = 0;
    unsigned int ulDelt = 0;
    int i = 0;    
    char acEditWords[EDIT_LENGTH];
    char *pname = CLI_NULL;
    char *pArgv[2]={0};
    char *save_ptr;
    UINT32 u32Argc = 0;
    char cCleanWord[]={'\x1b','\x5b','\x30','\x44'};
    CHECK_PTR_RET(pstTelnetCmd, SYS_ERROR);
    DEBUG(printf("<%s><%d>:input char is %x\r\n",__FUNCTION__,__LINE__,cWord));
    INT8 *pcHistory = CLI_NULL;
    CLI_CMD_S_HISTORY_PTR pstCliCmdHistory = CLI_NULL;
    TELNET_CLI_S_COMMAND *pstCliCurCmdNode = CLI_NULL;

    if(CLI_NULL != pstCliCmdCurDef)
    {
        pstCliCmdHistory = &pstCliCmdCurDef->stCliCmdHistory;
        pstCliCurCmdNode = pstCliCmdCurDef->cur;
    }

    if('\n' == cWord)
    {
        pstTelnetCmd->enLastCurrentStatus = KEY_INPUT_NORMAL;
        pstTelnetCmd->enCurrentStatus = KEY_INPUT_NORMAL;
        return 2;
    }

    if('\r' == cWord || '\n' == cWord)
    {
        pstTelnetCmd->enLastCurrentStatus = KEY_INPUT_NORMAL;
        pstTelnetCmd->enCurrentStatus = KEY_INPUT_NORMAL;
        return 0;
    }

    if(KEY_TAB == cWord) //tab 实现命令自动补全
    {
        //fix me,拷贝太多，另申请的变量太大，建议添加替代strtok_r的函数
        if(0 == pstTelnetCmd->ulEditLen)
        {
            return 1;
        }
        memset(acEditWords,0,sizeof(acEditWords));
        memcpy(acEditWords,pstTelnetCmd->acEditWord,strlen(pstTelnetCmd->acEditWord));
        char *pcCurEdit = acEditWords;

        pArgv[u32Argc] = strtok_r(pcCurEdit," ",&save_ptr);
        
        if((CLI_NULL != save_ptr) && strlen(save_ptr))
        {   
            u32Argc++;
            pArgv[u32Argc] = save_ptr;
        }
        while(CLI_NULL != pArgv[u32Argc]) 
        {
            if(' ' == *pArgv[u32Argc])
            {
                pArgv[u32Argc] = pArgv[u32Argc]+1;
            }
            else
            {
                break;
            }
        }
        
        if(pArgv[u32Argc])
        {
            while(pstCliCurCmdNode)
            {
                if(!strncmp(pstCliCurCmdNode->pcName,pArgv[u32Argc],strlen(pArgv[u32Argc])))
                {
                    DEBUG(printf("TAB_KEY find cmd:%s ,pArgv[%d]:%s\r\n",pstCliCurCmdNode->pcName,u32Argc,pArgv[u32Argc]));
                    pname = &pstCliCurCmdNode->pcName[strlen(pArgv[u32Argc])];
                    for(i = 0; i < strlen(pname);i++)
                    {
                        TelnetServerAddaWord(pname[i],pstTelnetCmd);
                        TelnetServerMoveClientCursor(pstTelnetCmd,CURSOR_EN_NONE);
                    }
                    DEBUG(printf("cmd:%s\r\n",pstTelnetCmd->acEditWord));
                    return 1;
                }
                pstCliCurCmdNode = pstCliCurCmdNode->next;
            }
        }
    }
    if((cWord >= 0x20) && (cWord < 0x7f) && (KEY_INPUT_NORMAL == pstTelnetCmd->enCurrentStatus))
    {
        if(strlen(pstTelnetCmd->acEditWord +1) > 100) //字串最大长度
        {
            return 1;
        }

        TelnetServerAddaWord(cWord,pstTelnetCmd);

        DEBUG(printf("%s,%d,Edit[%s],Send[%s]\r\n",__FUNCTION__,__LINE__,
            pstTelnetCmd->acEditWord,pstTelnetCmd->acSendWord));

        TelnetServerMoveClientCursor(pstTelnetCmd,CURSOR_EN_NONE);

    }
    else 
    {
        if((KEY_INPUT_NORMAL == pstTelnetCmd->enCurrentStatus)&& ISBACKSPACE(cWord))
        {
            DEBUG(printf("backspace ulCursorPos:%d\r\n",pstTelnetCmd->ulCursorPos));
            if(pstTelnetCmd->ulCursorPos > 0)
            {
                ulDelt = strlen(pstTelnetCmd->acEditWord) - pstTelnetCmd->ulCursorPos;
                for(i = 0;i < ulDelt ;i++)
                {
                    pstTelnetCmd->acEditWord[i+pstTelnetCmd->ulCursorPos -1] 
                        = pstTelnetCmd->acEditWord[i+pstTelnetCmd->ulCursorPos];
                }

                pstTelnetCmd->ulCursorPos--;
                pstTelnetCmd->acEditWord[pstTelnetCmd->ulCursorPos+i] = '\0';
                pstTelnetCmd->ulEditLen = strlen(pstTelnetCmd->acEditWord);

                pstTelnetCmd->ulSendLen = strlen(pstTelnetCmd->acSendWord);
                strncpy(pstTelnetCmd->acSendWord+pstTelnetCmd->ulSendLen,
                            "\x1b\x5b\x44",strlen("\x1b\x5b\x44"));
                pstTelnetCmd->ulSendLen = strlen(pstTelnetCmd->acSendWord);

                TelnetServerMoveClientCursor(pstTelnetCmd,CURSOR_EN_BACKSPACE);
            }

        }
        else
        {
            if(KEY_ESC == cWord && pstTelnetCmd->enCurrentStatus == KEY_INPUT_NORMAL) //ESC
            {
                pstTelnetCmd->enLastCurrentStatus = pstTelnetCmd->enCurrentStatus;
                pstTelnetCmd->enCurrentStatus = KEY_INPUT_DIRECTION;
            }
            else if((KEY_INPUT_NORMAL == pstTelnetCmd->enLastCurrentStatus)
            &&(KEY_INPUT_DIRECTION == pstTelnetCmd->enCurrentStatus && '[' == cWord))
            {
                pstTelnetCmd->enLastCurrentStatus = pstTelnetCmd->enCurrentStatus;
                pstTelnetCmd->enCurrentStatus = KEY_INPUT_DIRECTION_FINAL; 
            }
            else if((KEY_INPUT_DIRECTION == pstTelnetCmd->enLastCurrentStatus)
            &&(KEY_INPUT_DIRECTION_FINAL == pstTelnetCmd->enCurrentStatus))
            {
                if('D' == cWord) //向前1位
                {
                    if(pstTelnetCmd->ulCursorPos)
                    {
                     
                        pstTelnetCmd->ulCursorPos--;
                        TelnetServerMoveClientCursor(pstTelnetCmd,CURSOR_EN_LEFT);
                    }
                }
                else if('C' == cWord) //向后
                {
                    if(pstTelnetCmd->ulCursorPos < strlen(pstTelnetCmd->acEditWord))
                    {
                        pstTelnetCmd->ulCursorPos++;
                        TelnetServerMoveClientCursor(pstTelnetCmd,CURSOR_EN_RIGHT);
                    }
                }
                else if('A' == cWord) //up
                {
                    if(CLI_NULL != pstCliCmdHistory)
                    {
                        TelnetServerHistoryCmd(pstTelnetCmd,pstCliCmdHistory);
                        if(pstCliCmdHistory->u32CurIndex >= 1)
                        {
                            pstCliCmdHistory->u32CurIndex--;
                            pstCliCmdHistory->pcCur = pstCliCmdHistory->pacHistory[pstCliCmdHistory->u32CurIndex];
                        }
                        
                    }
                }
                else if('B' == cWord)
                {
                    if(CLI_NULL != pstCliCmdHistory)
                    {
                        pstCliCmdHistory->u32CurIndex++;
                        pstCliCmdHistory->pcCur = pstCliCmdHistory->pacHistory[pstCliCmdHistory->u32CurIndex];
                        if(CLI_NULL == pstCliCmdHistory->pcCur)
                        {
                            pstCliCmdHistory->pcCur = pstCliCmdHistory->pacHistory[0];
                            pstCliCmdHistory->u32CurIndex = 0;
                        }
                        
                        TelnetServerHistoryCmd(pstTelnetCmd,pstCliCmdHistory);
                        
                    }
                }
                pstTelnetCmd->enCurrentStatus = KEY_INPUT_NORMAL;
            }
        }

    }


    return 1;
}


/***********************************************************************************************
*函数名 ： TelnetServerProcessSingleChar
*函数功能描述 ： 处理一个按键，返回1表示命令还没有处理完，返回0表示输入了回车键，命令处理完毕
*函数参数 ： char cWord 待处理的字符
             TELNET_INPUT_DEF *pstTelnetCmd
*函数返回值 ： 1或0
*作者 ：易科
*函数创建日期 ： 2016.08.10
*函数修改日期 ： 
*修改人 ：
*修改原因 ： 
*版本 ： 
*历史版本 ： 
***********************************************************************************************/
int TelnetServerProcessOneKey(char *pcUserInput,
                                        unsigned int ulInputLen,
                                        IN OUT TELNET_INPUT_DEF *pstTelnetCmd,
                                        CLI_S_CMD_DEF *pstCliCmdCurDef)
{
    int i = 0;
    char cTmp;
    CHECK_PTR_RET(pcUserInput, SYS_ERROR);
    CHECK_PTR_RET(pstTelnetCmd, SYS_ERROR);

    cTmp = *pcUserInput;
    return TelnetServerProcessSingleChar(cTmp,pstTelnetCmd,pstCliCmdCurDef);
}


/*
*参数0表示socketID
* 参数1用来备份用户输入的命令行字符串
* 参数2输入时表示参数1的最大长度，防止拷贝越界，输出用作命令行的长度 
*/
/***********************************************************************************************
*函数名 ： TelnetServerReadUserCommand
*函数功能描述 ：读取用户输入的一行字符串
*函数参数 ： int iSocketId 套接字ID
			 OUT char *pcUserInput 用于存储用户输入的字串
			 unsigned int *pulInputLen 用户输入的长度,注意，此参数也作允许的最大长度值
*函数返回值 ： 1或0
*作者 ：易科
*函数创建日期 ： 2016.08.10
*函数修改日期 ： 
*修改人 ：
*修改原因 ： 
*版本 ： 
*历史版本 ： 
***********************************************************************************************/
CLI_EN_RET TelnetServerReadUserCommand(int iSocketId,
                                                    OUT char *pcUserInput,
                                                    TELNET_INPUT_DEF_PTR pstTelnetCmd,
                                                    OUT unsigned int *pulInputLen,
                                                    INPUT_EN_MODE enMode,
                                                    CLI_S_CMD_DEF *pstCliCmdCurDef)
{
    unsigned int ulLen;
    char cKey;
    
    CLI_EN_RET enRet = CLI_EN_RETURN_BUTT;
    int iRet = 0;

    CHECK_PTR_RET(pcUserInput, SYS_ERROR);
    CHECK_PTR_RET(pulInputLen, SYS_ERROR);

    if(enMode >= INPUT_E_BUTT)
    {
        DEBUG(printf("error input mode[%d]\r\n",enMode));
        return CLI_EN_PARAM_INVALID;
    }

    memset(pstTelnetCmd,0,sizeof(TELNET_INPUT_DEF));

    pstTelnetCmd->acEditWord[0] = '\0';
    pstTelnetCmd->acSendWord[0] = '\0';
    pstTelnetCmd->enCurrentStatus = KEY_INPUT_NORMAL;
    pstTelnetCmd->enLastCurrentStatus = KEY_INPUT_NORMAL;



    while(1)
    {
        enRet =  KTSocketRecv(iSocketId,&cKey,1,NULL,&ulLen);
        if(CLI_OK != enRet)
        {
            //FREE_MEM(pstTelnetCmd); //注:这里释放，外面的pstTelnetCmd的地址仍然非0，除非用双指针
            return enRet;
        }
        
        if(ulLen <= 0)
        {
            DEBUG(printf("recv error in %s:%d\r\n",__FUNCTION__,__LINE__));
            continue;
        }

        DEBUG(printf("ulLen=%d,ckey=%c,ckey_ascii=%#x\r\n",ulLen,cKey,cKey));

        iRet = TelnetServerProcessOneKey(&cKey,ulLen,pstTelnetCmd,pstCliCmdCurDef);

        if(2 == iRet)
        {
            continue;
        }


        if(pstTelnetCmd->ulSendLen && INPUT_E_NORMAL == enMode)
        {
            DEBUG(printf("onekey_to_client[%d]:%s\r\n",pstTelnetCmd->ulSendLen,pstTelnetCmd->acSendWord));
            //发送的长度大于0需要发送给客户端进行回显
            (void)TelnetServerSendToClient(iSocketId,pstTelnetCmd->acSendWord,pstTelnetCmd->ulSendLen);
        }

        //TelnetShowCmdInfo(pstTelnetCmd);

        if(KEY_INPUT_NORMAL == pstTelnetCmd->enCurrentStatus)
        {
            pstTelnetCmd->enCurrentStatus = KEY_INPUT_NORMAL;
            pstTelnetCmd->enLastCurrentStatus = KEY_INPUT_NORMAL;
            pstTelnetCmd->ulSendLen = 0;
            memset(pstTelnetCmd->acSendWord,0,SEND_LENGTH);
        }


        if(0 == iRet) //命令处理完毕
        {
            //将输入的命令行字符串拷贝给参数1，将长度附给参数2
            if(CMD_BUF_LEN < pstTelnetCmd->ulEditLen)
            {
                pstTelnetCmd->ulEditLen = CMD_BUF_LEN;
            }

            memcpy(pcUserInput,pstTelnetCmd->acEditWord,pstTelnetCmd->ulEditLen);
            memset(pstTelnetCmd->acEditWord,0,EDIT_LENGTH);
            *pulInputLen = pstTelnetCmd->ulEditLen;
            break;
        }

    }

    return CLI_OK;
}


CLI_EN_RET TelnetServerCheckLoginFunc(int iNewSocketId)
{
    CLI_EN_RET enRet;
    unsigned int ulTryTimes = 3;
    unsigned int ulUserNameLen =0;
    char acUserName[100]={0};
    char acPassWord[100]={0};
    unsigned int ulNameLen =100;
    unsigned int ulPasswdLen =100;

    TELNET_INPUT_DEF_PTR pstTelnetCmd = (TELNET_INPUT_DEF_PTR)malloc(sizeof(TELNET_INPUT_DEF));
    if(CLI_NULL == pstTelnetCmd)
    {
        DEBUG(printf("malloc for pstTelnetCmd failed in %s:%d\r\n",__FUNCTION__,__LINE__));
        return CLI_EN_MALLOC_FAILD;
    }

    while(ulTryTimes--)
    {
        TelnetServerSendToClient(iNewSocketId,stTelnetLoginWord.acUsername,strlen(stTelnetLoginWord.acUsername));

        //读取用户名
        enRet = TelnetServerReadUserCommand(iNewSocketId,acUserName,pstTelnetCmd,&ulNameLen,INPUT_E_NORMAL,CLI_NULL);
        if(CLI_OK != enRet)
        {
            DEBUG(printf("%s:%d,read user Command error,iRet = %d\r\n",__FUNCTION__,__LINE__,enRet));
            
            FREE_MEM(pstTelnetCmd);
            return enRet;
        }

        DEBUG(printf("****Read[%d]:%s\r\n",ulNameLen,acUserName));
        TelnetServerSendToClient(iNewSocketId,NEW_LINE,strlen(NEW_LINE));
        TelnetServerSendToClient(iNewSocketId,stTelnetLoginWord.acPassword,strlen(stTelnetLoginWord.acPassword));

        //读取密码
        enRet = TelnetServerReadUserCommand(iNewSocketId,acPassWord,pstTelnetCmd,&ulPasswdLen,INPUT_E_HIDE,CLI_NULL);
        if(CLI_OK != enRet)
        {
            DEBUG(printf("%s:%d,read user Command error,iRet = %d\r\n",__FUNCTION__,__LINE__,enRet));
            FREE_MEM(pstTelnetCmd);
            return enRet;
        }

        DEBUG(printf("****Read[%d]:%s\r\n",ulPasswdLen,acPassWord));
        TelnetServerSendToClient(iNewSocketId,NEW_LINE,strlen(NEW_LINE));

        //验证用户名和密码

        ulUserNameLen = strlen(SYS_USERNAME);
        ulPasswdLen = strlen(SYS_PASSWORD);
        
        DEBUG(printf("user[%ld]:%s,passwd[%ld]:%s\r\n",strlen(acUserName),acUserName,strlen(acPassWord),acPassWord));
        if((strlen(acUserName) != ulUserNameLen)
                ||strncmp(acUserName,SYS_USERNAME,ulUserNameLen)
                ||(strlen(acPassWord) != ulPasswdLen)
                ||strncmp(acPassWord,SYS_PASSWORD,ulPasswdLen))
        {
            TelnetServerSendToClient(iNewSocketId,stTelnetLoginWord.acLoginFail,strlen(stTelnetLoginWord.acLoginFail));
            memset(acUserName,0,sizeof(acUserName));
            memset(acPassWord,0,sizeof(acPassWord));
            continue;
        }
        else
        {
            FREE_MEM(pstTelnetCmd);
            return CLI_OK;
        }
        
    }
    FREE_MEM(pstTelnetCmd);
    return CLI_EN_LOGIN_ERROR;

}

CLI_EN_RET KTSocketRecv(INT32 i32SocketId,INT8 *pcStr,IN UINT32 u32Size,void *vReserve,OUT UINT32 *u32Len)
{
    fd_set   rfds; 
    INT32 i;
    struct timeval tv;
    INT32 i32Retval;

    FD_ZERO(&rfds);
    FD_SET(i32SocketId, &rfds);
    tv.tv_sec = TELNET_NODATA_TIMEOUT_MAX;
    tv.tv_usec = 0;
    *u32Len = -1;

    i32Retval = select(i32SocketId+1, &rfds, NULL, NULL, &tv);
    memset(pcStr,0,u32Size);
    if (i32Retval == -1)
    {
        perror("select()");
    }
    else if (i32Retval) /* FD_ISSET(0, &rfds) will be true. */
    {
        DEBUG(printf("Data is available now.i32RetVal = %d\r\n",i32Retval));
    }
    else
    {
        printf("No data within %d seconds.disconnected!!!\r\n",TELNET_NODATA_TIMEOUT_MAX);
        return  CLI_EN_ERROR_TIME_OUT;
    }
    
    if(FD_ISSET(i32SocketId,&rfds))
    {
        *u32Len = recv(i32SocketId,pcStr,u32Size,O_NONBLOCK);
    }

    if(*u32Len <= 0)
    {
        printf("%s,%d:select_ret=%d\r\n",__FUNCTION__,__LINE__,i32Retval);
        return CLI_EN_NO_DATA;
    }

    
    return CLI_OK;
}


void *TelnetServerThread(void *pArg)
{
    int iNewSocketId = *(int *)pArg;
    CLI_EN_RET enRet = 0;
    char ThreadName[12]={0};
    char acBuffer[RECV_LENGTH]={0};
    char acUserInput[RECV_LENGTH];
    char processed_buf[CMD_BUF_LEN];
    TELNET_CLI_S_COMMAND *pstCurCliCmdNode;
    int i;
    TELNET_INPUT_DEF_PTR pstTelnetCmd;
    unsigned int ulInputLen;
    unsigned int ulLen;
    DEBUG(printf("new socket_id:%d\r\n",iNewSocketId));

    sprintf(ThreadName,"TelnetCli%d",iNewSocketId);
    prctl(PR_SET_NAME, ThreadName);

    FILE *clientfp = fdopen(iNewSocketId,"w+");
    setbuf(clientfp,NULL);

    if(CLI_NULL == g_pstCliCmdList)
    {
        printf("g_pstCliCmdList is NULL,please check it\r\n");
        return CLI_NULL;
    }

    CLI_S_CMD_DEF *pstCliCmdDefMain = (CLI_S_CMD_DEF *)malloc(sizeof(CLI_S_CMD_DEF));
    if(CLI_NULL == pstCliCmdDefMain)
    {
        printf("pstCliCmdDefMain malloc failed\r\n");
        return CLI_NULL;
    }
    memset(pstCliCmdDefMain,0,sizeof(CLI_S_CMD_DEF));

    pstCliCmdDefMain->pstCliCmdHead=g_pstCliCmdList->pstCliCmd;
    pstCliCmdDefMain->i32SocketId = iNewSocketId;

    if(CLI_NULL ==pstCliCmdDefMain->pstCliCmdHead)
    {
        printf("no cmd registered,please check it\r\n");
        FREE_MEM(pstCliCmdDefMain);
        return CLI_NULL;
    }

    pstCliCmdDefMain->cur = pstCliCmdDefMain->pstCliCmdHead;
    memcpy(pstCliCmdDefMain->acPrompt,SYS_CMD_LINE,strlen(SYS_CMD_LINE));

    pstTelnetCmd = (TELNET_INPUT_DEF_PTR)malloc(sizeof(TELNET_INPUT_DEF));
    if(CLI_NULL == pstTelnetCmd)
    {
        DEBUG(printf("malloc for pstTelnetCmd failed in %s:%d\r\n",__FUNCTION__,__LINE__));
        FREE_MEM(pstCliCmdDefMain);
        return CLI_NULL;
    }
    memset(pstTelnetCmd,0,sizeof(TELNET_INPUT_DEF));
    

    TelnetServerSendToClient(iNewSocketId,TELNET_SHOW_COMMAND,sizeof(TELNET_SHOW_COMMAND));

    ulLen = recv(iNewSocketId,acBuffer,RECV_LENGTH,0);
    DEBUG(printf("Read cmd [%d]:%s\r\n",ulLen,acBuffer));
    TelnetServerSendToClient(iNewSocketId,stTelnetLoginWord.acWelcome,strlen(stTelnetLoginWord.acWelcome));

    enRet = TelnetServerCheckLoginFunc(iNewSocketId);
    if(CLI_OK != enRet)
    {
        TelnetServerSendToClient(iNewSocketId,stTelnetLoginWord.acLoginFail,strlen(stTelnetLoginWord.acLoginFail));
        TelnetCliFree(&pstCliCmdDefMain);
        FREE_MEM(pstTelnetCmd);
        close(iNewSocketId);
        DEBUG(printf("%s,%d,error=%#x\r\n",__FUNCTION__,__LINE__, enRet));
        return CLI_NULL;
    }
    
    TelnetServerSendToClient(iNewSocketId,stTelnetLoginWord.acLoginSuc,strlen(stTelnetLoginWord.acLoginSuc));
    TelnetServerSendToClient(iNewSocketId,pstCliCmdDefMain->acPrompt,strlen(pstCliCmdDefMain->acPrompt));
    TelnetSetTraceSocked(iNewSocketId,pthread_self());     
    
    while(1)
    {
        enRet = TelnetServerReadUserCommand(iNewSocketId,processed_buf,pstTelnetCmd,&ulLen,INPUT_E_NORMAL,pstCliCmdDefMain);
        if(CLI_OK != enRet)
        {
            DEBUG(printf("%s:%d,read user Command error,enRet = %#x\r\n",__FUNCTION__,__LINE__,enRet));
            TelnetCliFree(&pstCliCmdDefMain);
            FREE_MEM(pstTelnetCmd);
            TelnetReSetSocketId(pthread_self());
            close(iNewSocketId);
            return CLI_NULL;
        }

        DEBUG(printf("socketID[%d] userinput[%ld]:%s\r\n",iNewSocketId,strlen(processed_buf),processed_buf));
        
        if((strlen(processed_buf)>0) && (processed_buf[0] >= 0x20))
        {
            TelnetCliAddHistory(pstCliCmdDefMain,processed_buf);
        }
        
        TelnetServerSendToClient(iNewSocketId,NEW_LINE,strlen(NEW_LINE));
        enRet = TelnetCliAnalyzeAndProcessCmd(clientfp,pstTelnetCmd,processed_buf,ulLen,pstCliCmdDefMain);
        if(CLI_EN_CLI_QUIT == enRet)
        {
            break;
        }
        memset(processed_buf,0,sizeof(processed_buf));        
        TelnetServerSendToClient(iNewSocketId,pstCliCmdDefMain->acPrompt,strlen(pstCliCmdDefMain->acPrompt));
        
    }
    
    TelnetCliFree(&pstCliCmdDefMain);
    TelnetReSetSocketId(pthread_self());
    FREE_MEM(pstTelnetCmd);
    close(iNewSocketId);
    return CLI_NULL;
}


void SystemCreateThread(int stack_size,PTHREAD_FUNC func,void *arg)
{
    int iRet;
    pthread_attr_t attr;
    pthread_t thread_id;

    iRet = pthread_attr_init(&attr);
    if(0 != iRet)
    {
        DEBUG(printf("pthread_attr_init error\r\n"));
        return ;
    }

    if (stack_size > 0) 
    {
        iRet = pthread_attr_setstacksize(&attr, stack_size);
        if(0 != iRet)
        {
            DEBUG(printf("pthread_attr_setstacksize error\r\n"));
            goto ERROR;
            return ;
        }
    }

    iRet = pthread_create(&thread_id, &attr,func,arg);
    if(0 != iRet)
    {
        DEBUG(printf("pthread_create error\r\n"));
        return ;
    }

    pthread_detach(pthread_self());
ERROR:
    pthread_attr_destroy(&attr);
    //pthread_detach(thread_id);
     return ;
}




void *connect_loop(void *ptr)
{
    TelnetServerInit();
    //设置一个socket地址结构server_addr,代表服务器internet地址, 端口
    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr)); //把一段内存区的内容全部设置为0
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(TELNET_SERVER_PORT);
    //创建用于internet的流协议(TCP)socket,用server_socket代表服务器socket
    int server_socket = socket(PF_INET,SOCK_STREAM,0);
    if( server_socket < 0)
    {
        DEBUG(printf("Create Socket Failed!"));
        exit(1);
    }
   
   int opt =1;
   setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
     
    //把socket和socket地址结构联系起来
    if( bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr)))
    {
        DEBUG(printf("Server Bind Port : 23 Failed!"));
        exit(1);
    }
    printf("waiting for connect...\r\n");
    //server_socket用于监听
    if ( listen(server_socket, 5) )
    {
        DEBUG(printf("Server Listen Failed!"));
        exit(1);
    }
    
    while (1) //服务器端要一直运行
    {
        //定义客户端的socket地址结构client_addr
        struct sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);
        //接受一个到server_socket代表的socket的一个连接
        //如果没有连接请求,就等待到有连接请求--这是accept函数的特性
        //accept函数返回一个新的socket,这个socket(new_server_socket)用于同连接到的客户的通信
        //new_server_socket代表了服务器和客户端之间的一个通信通道
        //accept函数把连接到的客户端信息填写到客户端的socket地址结构client_addr中
        int new_server_socket = accept(server_socket,(struct sockaddr*)&client_addr,&length);
        if ( new_server_socket < 0)
        {
            DEBUG(printf("Server Accept Failed!\n"));
            break;
        }
        
        SystemCreateThread(THREAD_STACK_SIZE,TelnetServerThread,(void *)&new_server_socket);

        DEBUG(printf("new socket:%d\r\n",new_server_socket));
    }
    return CLI_NULL;
}

