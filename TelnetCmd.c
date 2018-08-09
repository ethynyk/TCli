#include "TelnetCmd.h"
#include "TelnetCliApi.h"



#define MONTH_PER_YEAR   12   // 一年12月
#define YEAR_MONTH_DAY   20   // 年月日缓存大小
#define HOUR_MINUTES_SEC 20   // 时分秒缓存大小


BOOL g_bThreadRun = false;


/*打印每个参数*/
int cli_handle_arg(INT32 argc,char *argv[],void *pvReserve)
{
   UINT32 i;
   
    if(argc==0)
    {
       TRACE_ERROR("no argument\r\n");
    }
    else
    {
       for(i=0;i<argc;i++)
       {
           TRACE_INFO("[%d]:%s\r\n",i,argv[i]);
       }
    }
}



int cli_printf_hello(INT32 argc,char *argv[],void *pvReserve)
{
    
   g_bThreadRun = !g_bThreadRun;
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

  TRACE("Ver:%s,Compile time is = %d-%d-%d %d:%d:%d\n",CLI_VERSION, year, month, day, hour, minutes, seconds);
}


/*打印字符串:Hello world!*/
void GetVersion(int32_t argc,char *argv[])
{
   GetCompileTime();
   return ;
}

/*打印字符串:Hello world!*/
int cli_GetVersion(INT32 argc,char *argv[],void *pvReserve)
{
   
    FILE *client = (FILE *)pvReserve;
    GetCompileTime();
   return ;
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
    DECLARE_CLI_CMD_MACRO(network,CLI_NULL,cli_printf_hello,"cli_printf_hello",0),
    DECLARE_CLI_CMD_MACRO(hal,CLI_NULL,cli_GetVersion,"cli_GetVersion",0),
    DECLARE_CLI_CMD_MACRO(bt,CLI_NULL,cli_GetVersion,"macro test",0),
    DECLARE_CLI_CMD_MACRO_END()
};


static TELNET_S_CLICMD my_cli_cmd_list_sub_hello[]={
    DECLARE_CLI_CMD_MACRO(hello,CLI_NULL,cli_printf_hello,"show hello world info",0),
    DECLARE_CLI_CMD_MACRO(second_directory,my_cli_cmd_list_subSecond,CLI_NULL,"second directory",0),
    DECLARE_CLI_CMD_MACRO(verSion,CLI_NULL,cli_GetVersion,"get the version",0),   
    DECLARE_CLI_CMD_MACRO_END()
};



/*命令表*/
static TELNET_S_CLICMD my_cli_cmd_list[]={

    DECLARE_CLI_CMD_MACRO(hellotest,CLI_NULL,cli_printf_hello,"print hello world test",0),
    DECLARE_CLI_CMD_MACRO(sub_dir,my_cli_cmd_list_sub_hello,CLI_NULL,"sub directory",0),
    DECLARE_CLI_CMD_MACRO(arg,CLI_NULL,cli_handle_arg,"show your input cmd",0), 
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



UINT32 ulj =0;
void printf_helloworld(void)
{
    static INT32 sTesti =0;  
    static UINT32 uli = 0;
    TRACE_ERROR("Hello world!,sTesti=%#x,%#x,%#x\r\n",sTesti,ulj,uli);
    TRACE_INFO("Hello world!,sTesti=%#x,%#x,%#x\r\n",sTesti,ulj,uli);
    TRACE_WARNING("Hello world!,sTesti=%#x,%#x,%#x\r\n",sTesti,ulj,uli);
    ulj++;
    sTesti++;
    uli++;
    return ;
}


void *ThreadTestMain(void *pArg)
{

    while(1)
    {
        if(g_bThreadRun)
        {
            printf_helloworld();
        }
        sleep(2);
    }

    return NULL;
}




