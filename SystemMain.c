#include"common.h"
#include"TelnetCliApi.h"
#include"TelnetCmd.h"

static pthread_t sConnetLoopHandle;
static pthread_t sTestCmdHandle;

int main(int argc,char *argv[])
{
    
    my_register_cmd();
    
    SystemCreateThread(&sConnetLoopHandle,THREAD_STACK_SIZE,connect_loop,NULL,"connectLoop",95);
    SystemCreateThread(&sTestCmdHandle,THREAD_STACK_SIZE,ThreadTestMain,NULL,"testCmdMain",95);

    while(1)
    {
        sleep(1000);
    }
    
    return 0;
}
