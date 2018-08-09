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
