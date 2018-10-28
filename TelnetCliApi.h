#ifndef __TELNETCLIAPI_H__
#define __TELNETCLIAPI_H__


#include "TelnetTrace.h"

typedef void *(*PTHREAD_FUNC) (void *);

// for_register_cmd
typedef TELNET_CLI_S_COMMAND TELNET_S_CLICMD;



#define DECLARE_CLI_CMD_MACRO(name,child,Cbfunc,help,usermode) \
        DECLARE_TELNET_CLI_CMD_MACRO(name,child,Cbfunc,help,usermode)

#define DECLARE_CLI_CMD_MACRO_END()   DECLARE_TELNET_CLI_CMD_MACRO_END()
// end for_register_cmd

#if 0
typedef struct TELNET_S_CLICMDtag
{
    INT8 *pcName;
    struct TELNET_CLI_S_COMMANDtag *pChildren;
    INT32 (*pCallback)(INT32 argc,char *argv[],void *pvReserve);
    INT8 *pcCmdHelp;
    INT32 i32UserMode;
    void *reverse0;
    void *reverse1;
    void *reverse2;
}TELNET_S_CLICMD,*TELNET_S_CLICMD_PTR;
#endif
// for_print
// for_print
#ifndef TRACE
#define TRACE TELNET_TRACE
#endif//TRACE

#ifndef TRACE_INFO
#define TRACE_INFO TELNET_TRACE_INFO
#endif//TRACE_INFO


#ifndef TRACE_WARNING
#define TRACE_WARNING TELNET_TRACE_WARNING
#endif//TRACE_WARNING


#ifndef TRACE_ERROR
#define TRACE_ERROR TELNET_TRACE_ERROR
#endif//TRACE_ERROR
//end for_print

extern INT32 RegisterCliCommand( void *pstCliCmd);
extern void *connect_loop(void *ptr);
extern int SystemCreateThread(pthread_t *thread_handle,int stack_size,PTHREAD_FUNC func,
                            void *arg,const char *name,int sched_priority);

#endif