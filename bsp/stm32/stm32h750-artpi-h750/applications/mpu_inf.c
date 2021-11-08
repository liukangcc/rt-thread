/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-01     liukang      first version
 */

/*
        1. 关键内存区域保护
        2. 线程堆栈保护
*/

#include <mpu.h>
#include <board.h>

//#define DRV_DEBUG
#define LOG_TAG             "drv.mpu"
#include <drv_log.h>

rt_uint32_t rt_mpu_regions_number;

/* ---- core-------     regions --------
        CORTEX-M3       8
        CORTEX-M4       16
        CORTEX-M7       16
        CORTEX-H7       16
*/

//static rt_uint32_t mpu_align_min(rt_uint32_t map_size)
//{
//    int ffs = 0;

//     /* 32 is the smallest region size, 31 is the largest valid value for
//     * ulReturnValue. */
//    for (ffs = 31; !(map_size & (1 << ffs)) && ffs > ARM_MPU_REGION_SIZE_32B + 1; ffs--);
//    if (map_size > (1 << ffs))
//    {
//        ffs++;
//    }
//    ffs -= 3;
//    if (ffs < 0)
//    {
//        ffs = 0;
//    }
//    return 1 << ffs;
//}

static rt_uint32_t mpu_align_min(rt_uint32_t ulActualSizeInBytes )
{
    rt_uint32_t ulRegionSize, ulReturnValue = 4;

    /* 32 is the smallest region size, 31 is the largest valid value for
     * ulReturnValue. */
    for( ulRegionSize = 32UL; ulReturnValue < 31UL; ( ulRegionSize <<= 1UL ) )
    {
        if( ulActualSizeInBytes <= ulRegionSize )
        {
            break;
        }
        else
        {
            ulReturnValue++;
        }
    }

    /* Shift the code by one before returning so it can be written directly
     * into the the correct bit position of the attribute register. */
    return( ulReturnValue << 1UL );
}

static void rt_mpu_enable(void)
{
    ARM_MPU_Enable(4); /* enable mpu */
}

static void rt_mpu_disable()
{
    ARM_MPU_Disable(); /* disable mpu */
}

static int rt_mpu_init(void)
{
    
    rt_mpu_disable();
    
    rt_uint8_t regions_number = 0;

    /* The only permitted number of regions are 8 or 16. */
    RT_ASSERT((MPU_REGIONS_NUMBER == 8 ) || MPU_REGIONS_NUMBER == 16);
    regions_number = (MPUTR >> MPUTR_DREGION_Pos) & 0x1F;
    if (regions_number != 0)
    {
        rt_mpu_regions_number = regions_number;
        LOG_D("cpu support mpu, regions number is: %d.", regions_number);
    }
    else
    {
        LOG_E("cpu not support mpu.");
        return -RT_ERROR;
    }

    MPURBAR = MPU_PROTECT_FLASH_START_ADDR | MPU_REGION_VALID | FLASH_REGION;
    MPURASR = (MPU_REGION_READ_ONLY) |
              ((configTEX_S_C_B_FLASH & MPURASR_TEX_S_C_B_Msk) << MPURASR_TEX_S_C_B_Pos) |
              (mpu_align_min(MPU_PROTECT_FLASH_SIZE)) |
              (MPU_REGION_ENABLE);

    MPURBAR = MPU_PROTECT_RAM_START_ADDR | MPU_REGION_VALID | KERNEL_RAM_REGION;
    MPURASR = (MPU_REGION_READ_WRITE) | 
              ((configTEX_S_C_B_SRAM & MPURASR_TEX_S_C_B_Msk ) << MPURASR_TEX_S_C_B_Pos ) |
              (mpu_align_min(MPU_PROTECT_RAM_SIZE)) |
              (MPU_REGION_ENABLE );
    
    /* Enable the memory fault exception. */
    NVIC_INT_CTRL |= NVIC_MEM_FAULT_Msk;
    
    /* Enable the MPU with the background region configured. */
    MPUCR |= MPU_BACKGROUND_ENABLE;

    rt_mpu_enable();

    return 0;
}
INIT_BOARD_EXPORT(rt_mpu_init);

void rt_mpu_deinit(void)
{
    return;
}

/**
 * @brief   This function will initialize a thread. It's used to initialize a
 *          static thread object.
 *
 * @param   thread is the thread object.
 *
 * @param   name is the name of thread, which shall be unique.
 *
 * @param   addr is the protected area.
 *
 * @param   size is the protected area length.
 *
 * @param   attribute is the read and write permission of the thread to the protected area.
 *
 * @return  Return the operation status. If the return value is RT_EOK, the function is successfully executed.
 *          If the return value is any other values, it means this operation failed.
 */
rt_err_t rt_thread_mpu_attach(rt_thread_t thread, rt_uint8_t* addr, size_t size, rt_uint32_t attribute)
{
    rt_uint32_t stack_top, stack_bottom;
    RT_ASSERT(thread->stack_size > 0)

    /* 获取栈顶地址 */
    stack_top = RT_ALIGN_DOWN((rt_uint32_t)addr + size, 32);
    stack_bottom = RT_ALIGN_DOWN((rt_uint32_t)addr - (1 << (ARM_MPU_REGION_SIZE_32B + 1)), 32);

    /* Only certain memory regions can be accessed */
    thread->setting.table[thread->setting.num].address   = stack_top | MPU_REGION_VALID | PROTECT_RAM_REGION + thread->setting.num;
    thread->setting.table[thread->setting.num].attribute = MPU_REGION_READ_ONLY | 
                                                           ((configTEX_S_C_B_SRAM & MPURASR_TEX_S_C_B_Msk ) << MPURASR_TEX_S_C_B_Pos) |
                                                           (ARM_MPU_REGION_SIZE_32B <<1) |
                                                           (MPU_REGION_ENABLE);
    thread->setting.num += 1;

    /* set current thread stack access, read and write */
    thread->setting.table[thread->setting.num].address   = stack_bottom | MPU_REGION_VALID | PROTECT_RAM_REGION + thread->setting.num;
    thread->setting.table[thread->setting.num].attribute = MPU_REGION_READ_ONLY | 
                                                           ((configTEX_S_C_B_SRAM & MPURASR_TEX_S_C_B_Msk) << MPURASR_TEX_S_C_B_Pos) |
                                                           (ARM_MPU_REGION_SIZE_32B <<1) |
                                                           (MPU_REGION_ENABLE);
    thread->setting.num += 1;

    return RT_EOK;
}

/**
 * @brief Allocate a block of memory with a minimum of 'size' bytes.
 *
 * @param size is the minimum size of the requested block in bytes.
 *
 * @return the pointer to allocated memory or NULL if no free memory was found.
 */
void *rt_mpu_malloc(rt_size_t len, rt_uint32_t type)
{

    rt_thread_t thread = RT_NULL;
    void *ptr = RT_NULL;

    ptr = rt_malloc(len);
    if (ptr == RT_NULL)
    {
        LOG_E("no memory.");
        return RT_NULL;
    }
    
    thread = rt_thread_self();
    
    if (thread->setting.num < NUM_CONFIGURABLE_REGION)
    {
        /* config the region. */
        thread->setting.table[thread->setting.num].address = ((rt_uint32_t)ptr) | 
                                                             (MPU_REGION_VALID) | 
                                                             (THREAD_STACK_REGION + thread->setting.num ); /* Region number. */

        thread->setting.table[thread->setting.num].attribute = (MPU_REGION_READ_WRITE ) |
                                                               (mpu_align_min((rt_uint32_t )len)) |
                                                               ((type & MPURASR_TEX_S_C_B_Msk ) << MPURASR_TEX_S_C_B_Pos ) |
                                                               (MPU_REGION_ENABLE );
        thread->setting.num++;

        return ptr;
    }

    /* no configurable region */
    if (ptr)
        rt_free(ptr);

    return RT_NULL;
}

void rt_mpu_free(void *ptr)
{
    rt_thread_t thread = RT_NULL;

    thread = rt_thread_self();
    /* Invalidate the region. */
    thread->setting.table[thread->setting.num].address = (THREAD_STACK_REGION + thread->setting.num) | MPU_REGION_VALID;
    thread->setting.table[thread->setting.num].attribute = 0UL;
    thread->setting.num--;

    rt_free(ptr);

}

void rt_mpu_table_switch(struct rt_thread* thread)
{
    rt_mpu_disable(); /* disable mpu */
    
    if (thread->setting.num == 0)
    {
        for (int i = 0; i < NUM_CONFIGURABLE_REGION; i++)
        {
            /* Invalidate the region. */
            MPURBAR = (PROTECT_RAM_REGION + i) | MPU_REGION_VALID;
            MPURASR = 0UL;
        }
    }
    else 
    {
        for (int i = 0; i < 2; i++)
        {
            MPURBAR = thread->setting.table[i].address;
            MPURASR = thread->setting.table[i].attribute;
        }
    }

    rt_mpu_enable();
}
