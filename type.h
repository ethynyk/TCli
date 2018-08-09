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
