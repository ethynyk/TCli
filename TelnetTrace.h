/* 
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
