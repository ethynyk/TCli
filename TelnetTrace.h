#ifndef __TELNET_TRACE_H__
#define __TELNET_TRACE_H__

#include "common.h"
#include "TelnetServer.h"

typedef enum{
    DEBUG_EN_TRACE_NONE = 0,
    DEBUG_EN_TRACE_INFO,
    DEBUG_EN_TRACE_WARNING,
    DEBUG_EN_TRACE_ERROR,
    DEBUG_EN_TRACE_BUTT
}DEBUG_EN_LEVEL;


typedef struct TELNET_S_TRACE_IDtag
{
    BOOL bUsed;
    INT32 sockId;
    pthread_t tid;
}TELNET_S_TRACE_ID;

//extern INT32 TelnetTrace(const char *pcString);
extern CLI_EN_RET TelnetSetTraceSocked(INT32 i32SocketId,pthread_t tid);
extern void TelnetReSetSocketId(const pthread_t tid);
void TelnetTracePrint(const DEBUG_EN_LEVEL enLevel,char *pcpathname,
                            const UINT32 u32Line,const char * fmt, ... );

#ifndef TELNET_TRACE_INFO 
#define TELNET_TRACE_INFO(fmt...) TelnetTracePrint(DEBUG_EN_TRACE_INFO,__FILE__,__LINE__,fmt)
#endif

#ifndef TELNET_TRACE_WARNING 
#define TELNET_TRACE_WARNING(fmt...) TelnetTracePrint(DEBUG_EN_TRACE_WARNING,__FILE__,__LINE__,fmt)
#endif

#ifndef TELNET_TRACE_ERROR 
#define TELNET_TRACE_ERROR(fmt...) TelnetTracePrint(DEBUG_EN_TRACE_ERROR,__FILE__,__LINE__,fmt)
#endif

#ifndef TELNET_TRACE
#define TELNET_TRACE(fmt...) TelnetTracePrint(DEBUG_EN_TRACE_NONE,__FILE__,__LINE__,fmt)
#endif


#endif
