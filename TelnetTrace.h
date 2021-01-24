#ifndef __TELNET_TRACE_H__
#define __TELNET_TRACE_H__

#include "common.h"
#include "TelnetServer.h"
#include "tcli.h"


typedef struct TELNET_S_TRACE_IDtag
{
    BOOL bUsed;
    INT32 sockId;
    pthread_t tid;
}TELNET_S_TRACE_ID;

//extern INT32 TelnetTrace(const char *pcString);
extern CLI_EN_RET TelnetSetTraceSocked(INT32 i32SocketId,pthread_t tid);
extern void TelnetReSetSocketId(const pthread_t tid);
extern void TelnetTracePrint(const DEBUG_EN_LEVEL enLevel,char *pcpathname,
                            const UINT32 u32Line,const char * fmt, ... );
extern void TelnetTraceLevelSet(DEBUG_EN_LEVEL eLevel);
extern DEBUG_EN_LEVEL TelnetTraceGetLevel();

#ifndef TELNET_TRACE 
#define TELNET_TRACE(fmt...) TelnetTracePrint(DEBUG_EN_TRACE_NONE,__FILE__,__LINE__,fmt)
#endif




#endif
