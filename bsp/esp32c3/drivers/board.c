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

#include "board.h"
#include "drv_uart.h"
#include "drv_gpio.h"
#include "tick.h"

void rt_hw_board_init(void)
{

    /* initialize hardware interrupt */
    rt_hw_uart_init();
    rt_hw_pin_init();
    rt_hw_tick_init();

#ifdef RT_USING_CONSOLE
    /* set console device */
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif /* RT_USING_CONSOLE */

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
}
