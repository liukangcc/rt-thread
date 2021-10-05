/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <rthw.h>
#include <rtthread.h>


static volatile unsigned long tick_cycles = 0;
int tick_isr(void)
{
    rt_tick_increase();
    return 0;
}

/* Sets and enable the timer interrupt */
int rt_hw_tick_init(void)
{
    return 0;
}
