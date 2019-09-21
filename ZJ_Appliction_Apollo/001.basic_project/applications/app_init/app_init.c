#include <stdint.h>
#include <string.h>
#include <rtthread.h>
#include "board.h"
#include "app_init.h"
#include "app_event.h"
#include "SEGGER_RTT.h"


void set_segger_rtt_mode_flag(void *agrm)
{  
    rt_kprintf("%d\r",agrm);
    set_send_mode_flag((uint32_t)agrm);
}
FINSH_FUNCTION_EXPORT_ALIAS(set_segger_rtt_mode_flag, mode, this is finsh c-tyle test(new name mode,mode() will work))
FINSH_FUNCTION_EXPORT_ALIAS(set_segger_rtt_mode_flag, __cmd_mode, this is finsh msh)

void test_event_task(void *agrm)
{  
    set_send_mode_flag(2);
    SEGGER_RTT_WriteString(0,RTT_CTRL_TEXT_BRIGHT_YELLOW);
    rt_kprintf("\r****************************\r");
    rt_kprintf("***[ZJ_RT_THREAD_APOLLO]****\r");
    rt_kprintf("***[GZH: Bluetooth-BLE]*****\r");
    rt_kprintf("***[BBS: bbs.codertown.cn]**\r");
    rt_kprintf("***[QQqun: 177341833]*******\r");
    rt_kprintf("****************************\r");
    SEGGER_RTT_WriteString(0,RTT_CTRL_TEXT_BRIGHT_WHITE);
//    set_send_mode_flag(1);
//    APP_EVENT_REMOVE_DELAY(test_event_task);
//    APP_EVENT_PUSH_DELAY(test_event_task,NULL,1000);
}
FINSH_FUNCTION_EXPORT_ALIAS(test_event_task, etest, this is finsh c-tyle test(new name etest,etest() will work))
FINSH_FUNCTION_EXPORT_ALIAS(test_event_task, __cmd_etest, this is finsh msh test)
    
int main(void)
{
    APP_EVENT_PUSH_DELAY(test_event_task,NULL,500);
    return RT_TRUE;
}


/**
 * @}
 */
