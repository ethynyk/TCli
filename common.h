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
#ifndef __COMMON_H__
#define __COMMON_H__

#include <netinet/in.h>    // for sockaddr_in
#include <sys/types.h>    // for socket
#include <sys/socket.h>    // for socket
#include <stdio.h>        // for printf
#include <stdlib.h>        // for exit
#include <string.h>        // for bzero
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <math.h>
#include "type.h"
#include "ascii.h"

#ifndef THREAD_STACK_SIZE
#define THREAD_STACK_SIZE (2*1024*1024)
#endif

#define CLI_VERSION "2.00"

#define DEBUG(x)     //x

#define CONNECT_NUM_MAX  (3)

typedef enum
{
    CLI_OK = 0,
    CLI_EN_PARAM_INVALID = 0xFFFFFFE0,
    CLI_EN_LOGIN_ERROR,
    CLI_EN_NO_DATA,
    CLI_EN_MALLOC_FAILD,
    CLI_EN_ERROR_TIME_OUT,
    CLI_EN_SOCKET_ERROR,
    CLI_EN_NOT_SUPPORT,
    CLI_EN_CLI_QUIT,
    CLI_EN_RETURN_BUTT
}CLI_EN_RET;






#define CHECK_PTR_VOID(x)               \
{                                       \
    if(0 == x)                      \
    {                               \
        printf("x is NULL\r\n");    \
        return;                     \
    }                               \
}


#define CHECK_PTR_RET(x,ret)            \
{                                       \
    if(0 == x)                          \
    {                                   \
        printf("x is NULL\r\n");        \
        return ret;                     \
    }                                   \
}

#define FREE_MEM(x)                     \
{                                       \
    if(CLI_NULL != x) {free(x);x=CLI_NULL;}    \
}                                       

#endif
