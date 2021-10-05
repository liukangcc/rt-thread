/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018/10/01     Bernard      The first version
 * 2018/12/27     Jesven       Change irq enable/disable to cpu0
 */

#include <rthw.h>

#define ESP32C3_INTC_DEFAULT_PRIO			15

typedef void (*intr_handler_t)(void*);

typedef struct {
    intr_handler_t handler;
    void *arg;
} intr_handler_item_t;

static intr_handler_item_t s_intr_handlers[32];

static void intr_handler_set(int int_no, intr_handler_t fn, void *arg)
{
    s_intr_handlers[int_no] = (intr_handler_item_t) {
        .handler = fn,
        .arg = arg
    };
}

uintptr_t handle_trap(uintptr_t mcause, uintptr_t epc, uintptr_t * sp)
{
    intr_handler_item_t it = s_intr_handlers[mcause];
    if (it.handler) 
    {
        (*it.handler)(it.arg);
    }

    return epc;
}

/**
 * This function will initialize hardware interrupt
 */
void rt_hw_interrupt_init(void)
{
    set_csr(mie, MIP_MEIP);
}

void rt_hw_scondary_interrupt_init(void)
{
}

/**
 * This function will mask a interrupt.
 * @param vector the interrupt number
 */
void rt_hw_interrupt_mask(int vector)
{
    esprv_intc_int_disable(1 << vector);
}

/**
 * This function will un-mask a interrupt.
 * @param vector the interrupt number
 */
void rt_hw_interrupt_umask(int vector)
{
    esprv_intc_int_set_priority(vector, ESP32C3_INTC_DEFAULT_PRIO);
	esprv_intc_int_set_type(vector, 0);
	esprv_intc_int_enable(1 << vector);
}

/**
 * This function will install a interrupt service routine to a interrupt.
 * @param vector the interrupt number
 * @param new_handler the interrupt service routine to be installed
 * @param old_handler the old interrupt service routine
 */
rt_isr_handler_t rt_hw_interrupt_install(int vector, rt_isr_handler_t handler,
        void *param, const char *name)
{
    intr_handler_set(vector, (intr_handler_t)handler, param);
    rt_hw_interrupt_umask(vector);
    
    return RT_NULL;
}

