#ifndef __MPU_INF_H__
#define __MPU_INF_H__

#include <rtthread.h>

#ifndef RT_MPU_ALIGN_SIZE
#define RT_MPU_ALIGN_SIZE     32
#endif

#ifndef MPU_REGIONS_NUMBER
#define MPU_REGIONS_NUMBER    8U
#endif

#define FLASH_REGION                    0UL            /* flash region */
#define KERNEL_RAM_REGION               1UL            /* kernel ram region */
#define PRIPHERALS_REGION               2UL            /* pripherals region */
#define PROTECT_RAM_REGION              3UL            /* protected ram region */
#define THREAD_STACK_REGION             4UL            /* thread region */
#define FIRST_CONFIGURABLE_REGION       5UL            /* thread region */
#define NUM_CONFIGURABLE_REGION         (MPU_REGIONS_NUMBER - PROTECT_RAM_REGION) /* number of regions that can be configured */
#define LAST_CONFIGURABLE_REGION        (MPU_REGIONS_NUMBER - 1)

struct rt_mpu_region
{
    void * address;
    unsigned int length;
    unsigned int parameter;
};
typedef struct rt_mpu_region *rt_mpu_region_t;

struct rt_mpu_region_register
{
    unsigned int address;
    unsigned int attribute;
};

struct rt_mpu_table
{
    struct rt_mpu_region_register table[NUM_CONFIGURABLE_REGION];
    unsigned char num;
};
typedef struct rt_mpu_table rt_mpu_tables;
    
#endif
