/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-5-30      Bernard      the first version
 */

#ifndef BOARD_H__
#define BOARD_H__

extern unsigned int __bss_start;
extern unsigned int __bss_end;

void rt_hw_board_init(void);

#endif
