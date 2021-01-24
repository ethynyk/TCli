/* type.h
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
#ifndef __TYPE_H__
#define __TYPE_H__
 

#ifndef SEND_LENGTH
#define SEND_LENGTH    (1024)
#endif

#ifndef RECV_LENGTH
#define RECV_LENGTH    (1)
#endif

#ifndef SYS_ERROR
#define SYS_ERROR      (-1)
#endif

#ifndef SYS_OK
#define SYS_OK         (0)
#endif

#ifndef SYS_TRUE
#define SYS_TRUE       (1)
#endif

#ifndef SYS_FALSE
#define SYS_FALSE       (0)
#endif

#ifndef CLI_NULL
#define CLI_NULL (0)
#endif

//用于查看函数参数的输入输出
#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif


#ifndef true
#define true (1)
#endif

#ifndef false
#define false (0)
#endif

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef char INT8;
typedef short INT16;
typedef int INT32;
typedef unsigned long long UINT64;
typedef unsigned long  ULONG;
typedef long long INT64;

typedef int BOOL;




#endif
