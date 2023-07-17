/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-07-10     xqyjlj       The first version.
 */

#include "gtimer.h"
#include "ktime.h"

unsigned long rt_ktime_cputimer_getres(void)
{
    return ((1000UL * 1000 * 1000) * RT_KTIME_RESMUL) / rt_hw_get_gtimer_frq();
}

unsigned long rt_ktime_cputimer_getfrq(void)
{
    return rt_hw_get_gtimer_frq();
}

unsigned long rt_ktime_cputimer_getcnt(void)
{
    return rt_hw_get_cntpct_val();
}

unsigned long rt_ktime_cputimer_getstep(void)
{
    return rt_ktime_cputimer_getfrq() / RT_TICK_PER_SECOND;
}
