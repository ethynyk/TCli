#include "tcli.h"
#include "TelnetCmd.h"
#include "TelnetTrace.h"




#define MONTH_PER_YEAR   12   // 一年12月
#define YEAR_MONTH_DAY   20   // 年月日缓存大小
#define HOUR_MINUTES_SEC 20   // 时分秒缓存大小


BOOL g_bThreadRun = false;

extern int cli_GetVersion(INT32 argc,char *argv[],void *pvReserve);
extern int setTrace(int32_t argc,char *argv[]);


static TELNET_CLI_S_COMMAND cli_cmd_list[]={
    {"help",    CLI_NULL,    CLI_NULL,"get help,show cmd list",0,CLI_NULL},
    {"ls",    CLI_NULL,    CLI_NULL,  "show cmd list",0,CLI_NULL},
    {"history", CLI_NULL,       CLI_NULL,       "get history cmd",0,CLI_NULL},
    DECLARE_CLI_CMD_MACRO(trace,CLI_NULL,setTrace,trace control,0),
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

    return 0;
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
    //CHECK_PTR_RET(pstCliCmd,CLI_EN_PARAM_INVALID);
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

    CHECK_PTR_RET(pstCliCmd,CLI_EN_PARAM_INVALID);
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




void GetCompileTime(void)
{
  const char year_month[MONTH_PER_YEAR][4] =
  { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  char compile_date[YEAR_MONTH_DAY] = {0}, compile_time[HOUR_MINUTES_SEC] = {0}, i;
  char str_month[4] = {0};
  int year, month, day, hour, minutes, seconds;

  sprintf(compile_date, "%s", __DATE__); // "Aug 23 2016"
  sprintf(compile_time, "%s", __TIME__); // "10:59:19"

  sscanf(compile_date, "%s %d %d", str_month, &day, &year);
  sscanf(compile_time, "%d:%d:%d", &hour, &minutes, &seconds);

  for(i = 0; i < MONTH_PER_YEAR; ++i)
  {
    if(strncmp(str_month, year_month[i], 3) == 0)
    {
      month = i + 1;
      break;
    }
  }

  TELNET_TRACE("Ver:%s,Compile time is = %d-%d-%d %d:%d:%d\n",CLI_VERSION, year, month, day, hour, minutes, seconds);
}


/*打印字符串:Hello world!*/
void GetVersion(int32_t argc,char *argv[])
{
   GetCompileTime();
   return ;
}

int setTrace(int32_t argc,char *argv[])
{
   int ilevel = 0;
   if(argc >= 2)
   {
    ilevel = atoi(argv[1]);
    TelnetTraceLevelSet((DEBUG_EN_LEVEL)ilevel);     
   }
   else
   {
      TELNET_TRACE("trace [0/1/2/3]\n");
      TELNET_TRACE("0 : off\n");
      TELNET_TRACE("1 : info\n");
      TELNET_TRACE("2 : warning\n");
      TELNET_TRACE("3 : error\n");
      TELNET_TRACE("current level is %d .\n",TelnetTraceGetLevel());  
   }
   return 0;
}

/*打印字符串:Hello world!*/
int cli_GetVersion(INT32 argc,char *argv[],void *pvReserve)
{
   
    FILE *client = (FILE *)pvReserve;
    GetCompileTime();
   return  0;
}


static TELNET_S_CLICMD my_cli_cmd_list_subSecond[]={
    DECLARE_CLI_CMD_MACRO(audio,CLI_NULL,cli_GetVersion,"macro test2",0),
    DECLARE_CLI_CMD_MACRO(hdmi,CLI_NULL,cli_GetVersion,"macro test2",0),
    DECLARE_CLI_CMD_MACRO(video,CLI_NULL,cli_GetVersion,"macro sub tes2t",0),
    DECLARE_CLI_CMD_MACRO(arm,CLI_NULL,cli_GetVersion,"macro test2",0),
    DECLARE_CLI_CMD_MACRO_END()
};

static TELNET_S_CLICMD my_cli_cmd_list_Second[]={
    DECLARE_CLI_CMD_MACRO(dp,CLI_NULL,cli_GetVersion,"cli_GetVersion",0),
    DECLARE_CLI_CMD_MACRO(hal,CLI_NULL,cli_GetVersion,"cli_GetVersion",0),
    DECLARE_CLI_CMD_MACRO(bt,CLI_NULL,cli_GetVersion,"macro test",0),
    DECLARE_CLI_CMD_MACRO_END()
};


static TELNET_S_CLICMD my_cli_cmd_list_sub_hello[]={
    DECLARE_CLI_CMD_MACRO(second_directory,my_cli_cmd_list_subSecond,CLI_NULL,"second directory",0),
    DECLARE_CLI_CMD_MACRO(verSion,CLI_NULL,cli_GetVersion,"get the version",0),   
    DECLARE_CLI_CMD_MACRO_END()
};



/*命令表*/
static TELNET_S_CLICMD my_cli_cmd_list[]={

    DECLARE_CLI_CMD_MACRO(sub_dir,my_cli_cmd_list_sub_hello,CLI_NULL,"sub directory",0),
    DECLARE_CLI_CMD_MACRO(version,CLI_NULL,cli_GetVersion,"show version",0),   
    DECLARE_CLI_CMD_MACRO_END()
};

void my_register_cmd(void)
{
    CLI_EN_RET enRet = CLI_OK;
    enRet = RegisterCliCommand((void *)my_cli_cmd_list);
    enRet += RegisterCliCommand((void *)my_cli_cmd_list_Second);
    if(CLI_OK != enRet)
    {
        printf("<%s,%d>:cmd register failed,enRet=%#x\r\n",__FUNCTION__,__LINE__,enRet);
        return ;
    }
    
    return ;
}




