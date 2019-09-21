#ifndef __BOARD_H_
#define __BOARD_H_

#include <rtthread.h>

#ifdef __CC_ARM
extern int Image$$RW_IRAM1$$ZI$$Limit;
#define AM_SRAM_BEGIN    (&Image$$RW_IRAM1$$ZI$$Limit)
#elif __ICCARM__
#pragma section="HEAP"
#define AM_SRAM_BEGIN    (__segment_end("HEAP"))
#else
extern int __bss_end__;
#define AM_SRAM_BEGIN   (&__bss_end__)
#endif

// <o> Internal SRAM memory size[Kbytes] <8-384>
// <i>Default: 256
#define AM_SRAM_SIZE         384
#define AM_SRAM_END          (0x10000000 + AM_SRAM_SIZE * 1024)

/**@brief Macro for converting milliseconds to ticks.  performing rounded integer division (as opposed to truncating the result)
 *
 * @param[in] TIME          Number of milliseconds to convert.
 */
#define MSEC_TO_OS_TICK(TIME) (((TIME) * (RT_TIMER_TICK_PER_SECOND) + 500)/1000)

#define TICK_TO_OS_MSEC(TICK) (((TICK) * 1000 + (RT_TIMER_TICK_PER_SECOND >>1))/RT_TIMER_TICK_PER_SECOND)

#ifndef ABS
#define ABS(a) 			  (((a) < (0)) ? (-(a)) : (a))
#endif

void rt_hw_board_init(void);

#endif /* __BOARD_H__ */
