/* tcli.h
**BSD 3-Clause License
**
**Copyright (c) 2020, yike
**All rights reserved.
**
**Redistribution and use in source and binary forms, with or without
**modification, are permitted provided that the following conditions are met:
**
**1. Redistributions of source code must retain the above copyright notice, this
**   list of conditions and the following disclaimer.
**
**2. Redistributions in binary form must reproduce the above copyright notice,
**   this list of conditions and the following disclaimer in the documentation
**   and/or other materials provided with the distribution.
**
**3. Neither the name of the copyright holder nor the names of its
**   contributors may be used to endorse or promote products derived from
**   this software without specific prior written permission.
**
**THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
**AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
**IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
**FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
**DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
**SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
**CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
**OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
**OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/
#ifndef __TCLIAPI_H__
#define __TCLIAPI_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#define DECLARE_TELNET_CLI_CMD_MACRO(name,child,Cbfunc,help,usermode)  \
{                                                               \
    #name,                                                      \
    child,                                                      \
    Cbfunc,                                                     \
    #help,                                                      \
    usermode,                                                   \
    0,                                                    \
    0,                                                   \
    0                                                    \
}

#define DECLARE_TELNET_CLI_CMD_MACRO_END()                             \
{                                                               \
    0,                                                   \
    0,                                                   \
    0,                                                   \
    0,                                                   \
    0,                                                   \
    0,                                                    \
    0,                                                   \
    0                                                    \
}

typedef enum{
    DEBUG_EN_TRACE_NONE = 0, //always print,can not be control
    DEBUG_EN_TRACE_INFO,
    DEBUG_EN_TRACE_WARNING,
    DEBUG_EN_TRACE_ERROR,
    DEBUG_EN_TRACE_BUTT
}DEBUG_EN_LEVEL;

typedef struct TELNET_CLI_S_COMMANDtag
{
    char *pcName;
    struct TELNET_CLI_S_COMMANDtag *pChildren;
    int (*pCallback)(int argc,char *argv[],void *pvReserve);
    char *pcCmdHelp;
    int i32UserMode;
    struct TELNET_CLI_S_COMMANDtag *next;
    struct TELNET_CLI_S_COMMANDtag *parent;
    struct TELNET_CLI_S_COMMANDtag *prev;
}TELNET_CLI_S_COMMAND,*TELNET_CLI_S_COMMAND_PTR;

// for_register_cmd
typedef TELNET_CLI_S_COMMAND TELNET_S_CLICMD;

#define DECLARE_CLI_CMD_MACRO(name,child,Cbfunc,help,usermode) \
        DECLARE_TELNET_CLI_CMD_MACRO(name,child,Cbfunc,help,usermode)

#define DECLARE_CLI_CMD_MACRO_END()   DECLARE_TELNET_CLI_CMD_MACRO_END()
// end for_register_cmd

extern void TelnetTracePrint(const DEBUG_EN_LEVEL enLevel,char *pcpathname,
                            const unsigned int u32Line,const char * fmt, ... );
extern int RegisterCliCommand( void *pstCliCmd);
extern void TcliInit(void);


#ifndef tcli_info 
#define tcli_info(fmt...) TelnetTracePrint(DEBUG_EN_TRACE_INFO,__FILE__,__LINE__,fmt)
#endif

#ifndef tcli_warning 
#define tcli_warning(fmt...) TelnetTracePrint(DEBUG_EN_TRACE_WARNING,__FILE__,__LINE__,fmt)
#endif

#ifndef tcli_error 
#define tcli_error(fmt...) TelnetTracePrint(DEBUG_EN_TRACE_ERROR,__FILE__,__LINE__,fmt)
#endif


#ifndef tcli_print 
#define tcli_print(fmt...) TelnetTracePrint(DEBUG_EN_TRACE_NONE,__FILE__,__LINE__,fmt)
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif
