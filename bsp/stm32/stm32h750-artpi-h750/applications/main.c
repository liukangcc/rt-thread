/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-03-17     supperthomas first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#ifdef RT_USING_MPU
#include <mpu.h>
#endif

/* defined the LED0 pin: PI8 */
#define LED0_PIN    GET_PIN(I, 8)
#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       1024
#define THREAD_TIMESLICE        5

#ifdef RT_USING_WIFI
    extern void wlan_autoconnect_init(void);
#endif

extern rt_err_t rt_thread_mpu_attach(rt_thread_t thread, rt_uint8_t* addr, size_t size, rt_uint32_t attribute);

static void thread1_entry(void *param)
{
   int temp = 0;
   while (1)
   {
       rt_thread_mdelay(1000);
   }
}

#define THREAD_MEMORY_SIZE 1024
uint8_t thread_stack[THREAD_MEMORY_SIZE] __attribute__((aligned(THREAD_MEMORY_SIZE)));

int main(void)
{
    struct rt_thread tid;
    rt_thread_t tid2 = RT_NULL;
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    
    rt_thread_init(&tid, "mpu", thread1_entry, RT_NULL, thread_stack, THREAD_MEMORY_SIZE, THREAD_PRIORITY, 20);
    {
        rt_thread_mpu_attach(&tid, thread_stack, THREAD_MEMORY_SIZE, 0);
        rt_thread_startup(&tid);
    }
 
    while (1)
    {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
}

#include "stm32h7xx.h"
static int vtor_config(void)
{
    /* Vector Table Relocation in Internal QSPI_FLASH */
    SCB->VTOR = QSPI_BASE;
    return 0;
}
INIT_BOARD_EXPORT(vtor_config);
