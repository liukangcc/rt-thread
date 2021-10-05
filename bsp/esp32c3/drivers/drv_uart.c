/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "board.h"
#include "drv_uart.h"
#include <esp_rom_functions.h>

#include <stdio.h>
#include <sysctl.h>

#include "uart.h"

static rt_err_t  rt_uart_configure(struct rt_serial_device *serial, struct serial_configure *cfg);
static rt_err_t  uart_control(struct rt_serial_device *serial, int cmd, void *arg);
static int       drv_uart_putc(struct rt_serial_device *serial, char c);
static int       drv_uart_getc(struct rt_serial_device *serial);


struct device_uart  
{
    int number; 
};

const struct rt_uart_ops _uart_ops =
{
    rt_uart_configure,
    RT_NULL,
    drv_uart_putc,
    drv_uart_getc,
    //TODO: add DMA support
    RT_NULL
};

/*
 * UART Initiation
 */
int rt_hw_uart_init(void)
{
    struct rt_serial_device *serial;
    struct device_uart      *uart;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    {
        static struct rt_serial_device  serial1;
        static struct device_uart       uart1;

        serial  = &serial1;
        uart    = &uart1;

        serial->ops              = &_uart_ops;
        serial->config           = config;
        serial->config.baud_rate = 115200;

        uart->number = 0;

        rt_hw_serial_register(serial,
                              "uart0",
                              RT_DEVICE_FLAG_RDWR,
                              uart);
    }

    return 0;
}

/*
 * UART interface
 */
static rt_err_t rt_uart_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct device_uart *uart;
    RT_ASSERT(serial != RT_NULL);
    serial->config = *cfg;

    uart = serial->parent.user_data;
    RT_ASSERT(uart != RT_NULL);

    (void) serial;
    (void) cfg;

    return (RT_EOK);
}

static rt_err_t uart_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct device_uart *uart;

    uart = serial->parent.user_data;
    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        break;

    case RT_DEVICE_CTRL_SET_INT:
        break;
    }

    return (RT_EOK);
}

static int drv_uart_putc(struct rt_serial_device *serial, char c)
{
    struct device_uart *uart = serial->parent.user_data;
    esp_rom_uart_tx_one_char(c);
    return (1);
}

static int drv_uart_getc(struct rt_serial_device *serial)
{
    struct device_uart *uart = serial->parent.user_data;
    char c;
    esp_rom_uart_rx_one_char(&c);
    return ((int )c);
}

RT_WEAK void uart_debug_init(uart_device_number_t uart_channel)
{

}

