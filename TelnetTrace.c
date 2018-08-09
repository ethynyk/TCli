#include "TelnetTrace.h"
#include <stdarg.h>

static INT32 g_i32SocketId = -1;

static TELNET_S_TRACE_ID astTraceId[CONNECT_NUM_MAX];


CLI_EN_RET TelnetSetTraceSocked(INT32 i32SocketId,pthread_t tid)
{
    unsigned int u32i;
    //g_i32SocketId = i32SocketId;

    for(u32i = 0;u32i < CONNECT_NUM_MAX; u32i++)
    {
        if(false == astTraceId[u32i].bUsed)
        {
            astTraceId[u32i].bUsed = true;
            astTraceId[u32i].tid = tid;
            astTraceId[u32i].sockId = i32SocketId;
            return CLI_OK;
        }
    }
    
    return CLI_EN_NOT_SUPPORT;
}

void TelnetReSetSocketId(const pthread_t tid)
{
    unsigned int u32i;

    for(u32i = 0;u32i < CONNECT_NUM_MAX; u32i++)
    {
        if((true == astTraceId[u32i].bUsed) &&(tid == astTraceId[u32i].tid))
        {
            astTraceId[u32i].bUsed = false;
        }
    }
}

INT32 GetCurThreadSocketId(const pthread_t tid)
{
    unsigned int u32i;
    for(u32i = 0;u32i < CONNECT_NUM_MAX; u32i++)
    {
        if((true == astTraceId[u32i].bUsed) &&(tid == astTraceId[u32i].tid))
        {
            return astTraceId[u32i].sockId;
        }
    }

    return -1; //should not go here
}

INT32 GetValidSocketId(const UINT32 u32Idx)
{

    if((true == astTraceId[u32Idx].bUsed))
    {
        return astTraceId[u32Idx].sockId;
    }
    return -1;   
}



INT32 TelnetTrace(const char *pcString,const INT32 i32SocketId)
{
    fd_set   wfds; 
    INT32 i;
    struct timeval tv;
    INT32 i32Retval;
    UINT32 u32Length = strlen(pcString);
    //ESC[nD:光标左移n个字符。
    char cResetCursor[]= {'\r','\x1b','\x5b','\x30','\x44'};// 重置光标

    //i32SocketId =g_i32SocketId;

    FD_ZERO(&wfds);
    FD_SET(i32SocketId, &wfds);
    tv.tv_sec = 0;
    tv.tv_usec = 10000; //10ms
   
    i32Retval = select(i32SocketId+1, NULL, &wfds, NULL, &tv);
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

    sendto(i32SocketId,pcString,u32Length, MSG_DONTWAIT,NULL,0);
    sendto(i32SocketId,cResetCursor,sizeof(cResetCursor), MSG_DONTWAIT,NULL,0);
    
    return 0;
}


void TelnetTracePrint(const DEBUG_EN_LEVEL enLevel, char *pcpathname,
                            const UINT32 u32Line,const char * fmt,...)
{
    int n;
    UINT32 u32i;
    int size = 1024;     /* Guess we need no more than 256 bytes. */
    char *pbuffer, *np;
    char *pfilename;
    INT32 i32SocketId;
    va_list ap;
    
    CHECK_PTR_VOID(pcpathname);
    CHECK_PTR_VOID(fmt);

    if ((pbuffer = malloc(size)) == NULL)
    {
       return ;
    }

    pfilename = basename(pcpathname);
    if(NULL == pcpathname)
    {
        printf("pcpathname is NULL\r\n");
        FREE_MEM(pbuffer);
        return ;
    }

    switch(enLevel)
    {
        case DEBUG_EN_TRACE_INFO:
            snprintf(pbuffer,size,"[INFO][%s]<%d>:",pfilename,u32Line);
            break;
        case DEBUG_EN_TRACE_WARNING:
            snprintf(pbuffer,size,"[WARN][%s]<%d>:",pfilename,u32Line);
            break;
        case DEBUG_EN_TRACE_ERROR:
            snprintf(pbuffer,size,"[EROR][%s]<%d>:",pfilename,u32Line);
            break;
        case DEBUG_EN_TRACE_NONE:
            memset(pbuffer,0,size);
            break;
        default:
            FREE_MEM(pbuffer);
            return ;
    }
    
    while (1) 
    {
       va_start(ap, fmt);
       n = vsnprintf(pbuffer+strlen(pbuffer), size, fmt, ap);
       va_end(ap);

       if (n > -1 && n < size)
           break;

       if (n > -1)    
           size = n+1; 
       else           
           size *= 2; 

       if ((np = realloc (pbuffer, size)) == NULL) 
       {
           FREE_MEM(pbuffer);
           return ;
       } 
       else 
       {
           pbuffer = np;
       }
    }

    
    if(DEBUG_EN_TRACE_NONE == enLevel)
    {
        i32SocketId = GetCurThreadSocketId(pthread_self());
        if(-1 == i32SocketId)
        {
            printf("GetSocketId Error_sockId:%d\r\n",i32SocketId);
            FREE_MEM(pbuffer);
            return ;
        }
        TelnetTrace(pbuffer,i32SocketId);
    }
    else
    {
        for(u32i = 0; u32i < CONNECT_NUM_MAX; u32i++)
        {
            i32SocketId = GetValidSocketId(u32i);
            if(-1 != i32SocketId)
            {
                TelnetTrace(pbuffer,i32SocketId);
            }
        }
    }
    FREE_MEM(pbuffer);

    return;
}

