#include"common.h"
#include"TelnetCliApi.h"
#include"TelnetCmd.h"



int main(int argc,char *argv[])
{
    
    my_register_cmd();
    
    SystemCreateThread(THREAD_STACK_SIZE,connect_loop,NULL);
    SystemCreateThread(THREAD_STACK_SIZE,ThreadTestMain,NULL);

    while(1)
    {
        sleep(1000);
    }
    
    return 0;
}
