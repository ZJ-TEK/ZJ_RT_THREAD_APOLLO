/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2017, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-09-14     Haley        first implementation
 */
#include "board.h"

#include <rtthread.h>
#include <rthw.h>

#include "am_mcu_apollo.h"
#include "hal/am_hal_clkgen.h"
#include "hal/am_hal_cachectrl.h"

#define TICK_RATE_HZ  RT_TICK_PER_SECOND
#define SYSTICK_CLOCK_HZ  ( 32768UL )
#define WAKE_INTERVAL ( (uint32_t) ((SYSTICK_CLOCK_HZ / TICK_RATE_HZ)))

/**
 * This is the timer interrupt service routine.
 *
 */
void am_stimer_cmpr0_isr(void)
{
    /* Check the timer interrupt status */
    am_hal_stimer_int_clear(AM_HAL_STIMER_INT_COMPAREA);
    am_hal_stimer_compare_delta_set(0, WAKE_INTERVAL);

    if (rt_thread_self() != RT_NULL)
    {
        /* enter interrupt */
        rt_interrupt_enter();

        rt_tick_increase();

        /* leave interrupt */
        rt_interrupt_leave();
    }
}

/**
 * This is the SysTick Configure.
 *
 */
void SysTick_Configuration(void)
{
    /* Enable compare A interrupt in STIMER */
    am_hal_stimer_int_enable(AM_HAL_STIMER_INT_COMPAREA);

    //
    // Enable the timer interrupt in the NVIC, making sure to use the
    // appropriate priority level.
    //
#if AM_CMSIS_REGS
    #define NVIC_configKERNEL_INTERRUPT_PRIORITY        (0x7)    
    NVIC_SetPriority(STIMER_CMPR0_IRQn, NVIC_configKERNEL_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(STIMER_CMPR0_IRQn);
#else // AM_CMSIS_REGS
    am_hal_interrupt_priority_set(AM_HAL_INTERRUPT_STIMER_CMPR0, configKERNEL_INTERRUPT_PRIORITY);
    am_hal_interrupt_enable(AM_HAL_INTERRUPT_STIMER_CMPR0);
#endif // AM_CMSIS_REGS

    /* Configure the STIMER and run */
    am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
    am_hal_stimer_compare_delta_set(0, WAKE_INTERVAL);
    am_hal_stimer_config(AM_HAL_STIMER_XTAL_32KHZ |
                         AM_HAL_STIMER_CFG_COMPARE_A_ENABLE);
}

/**
 * This is the low power operation.
 * This function enables several power-saving features of the MCU, and
 * disables some of the less-frequently used peripherals. It also sets the
 * system clock to 24 MHz.
 */
void am_low_power_init(void)
{
    //
    // Initialize for low power in the power control block
    //
    am_hal_pwrctrl_low_power_init();

    #ifndef RT_USING_RTC
    //
    // Run the RTC off the XTAL.
    //
    am_hal_rtc_osc_select(AM_HAL_RTC_OSC_XT);

    //
    // Stop the XTAL.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_XTAL_STOP, 0);

    //
    // Disable the RTC.
    //
    am_hal_rtc_osc_disable();    
    #endif
}

/**
 * This is the deep power save.
 *
 */
void deep_power_save(void)
{
    am_hal_interrupt_master_disable();

    am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

    am_hal_interrupt_master_enable();
}

/**
 * This function will initial APOLLO2 board.
 */
void rt_hw_board_init(void)
{
    //
    // Set the clock frequency
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();
    
    /* Configure the board for low power operation */
    am_low_power_init();

    /* Config SysTick */
    SysTick_Configuration();

#ifdef RT_USING_IDLE_HOOK
    /* Set sleep deep mode */
    rt_thread_idle_sethook(deep_power_save);

#ifndef NO_FPU
    /* Enable the floating point module, and configure the core for lazy stacking */
    am_hal_sysctrl_fpu_enable();
    am_hal_sysctrl_fpu_stacking_enable(true);
#else
    am_hal_sysctrl_fpu_disable();
#endif

    /* Turn off unused Flash & SRAM */
                                 
    am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEM_FLASH_512K);
    //am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEMEN_SRAM32K);

#endif

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#ifdef RT_USING_CONSOLE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

#ifdef RT_USING_HEAP
    rt_system_heap_init((void*)AM_SRAM_BEGIN, (void*)AM_SRAM_END);
#endif
}

/*@}*/
