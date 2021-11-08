
#ifndef __MPU_H__
#define __MPU_H__

#include <rtthread.h>
#include <rthw.h>

#ifndef configTEX_S_C_B_FLASH
    /* Default to TEX=000, S=1, C=1, B=1 for backward compatibility. */
    #define configTEX_S_C_B_FLASH    ( 0x07UL )
#endif

/* TEX, Shareable (S), Cacheable (C) and Bufferable (B) bits for RAM
 * region. */
#ifndef configTEX_S_C_B_SRAM
    /* Default to TEX=000, S=1, C=1, B=1 for backward compatibility. */
    #define configTEX_S_C_B_SRAM          ( 0x07UL )
#endif

/* Constants required to access and manipulate the MPU. */
#define MPUTR                                 HWREG32(0xE000ED90)       /*!< mpu type register, RO */
#define MPUCR                                 HWREG32(0xE000ED94)       /*!< mpu control register, RW */
#define MPURNR                                HWREG32(0xE000ED98)       /*!< mpu region number register, RW */
#define MPURBAR                               HWREG32(0xE000ED9C)       /*!< mpu region base address register, RW */
#define MPURASR                               HWREG32(0xE000EDA0)       /*!< mpu region attribute register, RW */

#ifndef NVIC_INT_CTRL 
#define NVIC_INT_CTRL                         HWREG32(0xE000ED04)  
#endif

/* MPUTR Register Definitions */
#define MPUTR_IREGION_Pos                                      16U
#define MPUTR_DREGION_Pos                                      8U   /* mpu support number */
#define MPUTR_SEPARATE_Pos                                     0U   /* always is zero */

/* MPUCR Register Definitions */
#define MPUCR_PRIVDEFENA_Pos                                   2UL /* background region */
#define MPUCR_HFNMIENA_Pos                                     1UL
#define MPUCR_ENABLE_Pos                                       0UL /* enable mpu */

/* MPURNR Register Definitions */
#define MPURNR_REGION                                     0x0UL

/* MPURASR Register Definitions */
#define MPURASR_XN_Pos                                     28U                            /*!< MPURBAR: XN Position */                                                                 
#define MPURASR_XN_Msk                                     ((1UL << MPURASR_XN_Pos))      /*!< MPURBAR: XN Mask */

#define MPURASR_AP_Pos                                     24U                             /*!< MPURBAR: AP Position */                                                                 
#define MPURASR_AP_Msk                                     ((3UL << MPURASR_XN_Pos))      /*!< MPURBAR: AP Mask */

#define MPURASR_TEX_Pos                                    19U                             /*!< MPURBAR: TEX Position */                                                                 
#define MPURASR_TEX_Msk                                    ((3UL << MPURASR_XN_Pos))      /*!< MPURBAR: TEX Mask */

#define MPURASR_S_Pos                                      18U                             /*!< MPURBAR: S Position */                                                                 
#define MPURASR_S_Msk                                      ((1UL << MPURASR_XN_Pos))      /*!< MPURBAR: S Mask */

#define MPURASR_C_Pos                                      17U                             /*!< MPURBAR: C Position */                                                                 
#define MPURASR_C_Msk                                      ((1UL << MPURASR_XN_Pos))      /*!< MPURBAR: C Mask */

#define MPURASR_B_Pos                                      16U                             /*!< MPURBAR: B Position */                                                                 
#define MPURASR_B_Msk                                      ((1UL << MPURASR_XN_Pos))      /*!< MPURBAR: B Mask */

#define MPURASR_SRD_Pos                                    8U                              /*!< MPURBAR: SRD Position */                                                                 
#define MPURASR_SRD_Msk                                    ((0xFUL << MPURASR_XN_Pos))    /*!< MPURBAR: SRD Mask */

#define MPURASR_SIZE_Pos                                   1U                              /*!< MPURBAR: RBAR Position */                                                                 
#define MPURASR_SIZE_Msk                                   ((0xFUL << MPURASR_XN_Pos))    /*!< MPURBAR: RBAR Mask */

#define MPURASR_ENABLE_Pos                                 0U                              /*!< MPURBAR: SZENABLE Position */                                                                 
#define MPURASR_ENABLE_Msk                                 ((1UL << MPURASR_XN_Pos))      /*!< MPURBAR: SZENABLE Mask */

#define MPURASR_TEX_S_C_B_Pos                         (16UL)                             /* MPURBAR: TEX,S,C,B bits Position */
#define MPURASR_TEX_S_C_B_Msk                         (0x3FUL)

/* MPURBAR Register Definitions */
// #define MPURBAR_ADDR  
#define MPURBAR_VALID_Pos                                       4UL
#define MPURBAR_REGION                                          0UL

/* access to  the MPU regions, only privilege level. */
#define MPU_REGION_READ_ONLY                      ( 0x05UL << MPURASR_AP_Pos )
#define MPU_REGION_READ_WRITE                     ( 0x01UL << MPURASR_AP_Pos )
#define MPU_REGION_CACHEABLE_BUFFERABLE           ( 0x07UL << MPURASR_B_Pos )

#define MPU_REGION_EXECUTE_DISABLE                           ( 0x01UL << MPURASR_XN_Pos ) /* 此区域禁止取指 */
#define MPU_REGION_EXECUTE_ENABLE                            ( 0x00UL << MPURASR_XN_Pos ) /* 此区域允许取指 */

#define MPU_ENABLE                            ( 0x01UL << MPUCR_ENABLE_Pos)
#define MPU_DISABLE                           ( 0x00UL << MPUCR_ENABLE_Pos) 
#define MPU_BACKGROUND_ENABLE                 ( 0x01UL << MPUCR_PRIVDEFENA_Pos)
#define MPU_REGION_VALID                      ( 0x01UL << MPURBAR_VALID_Pos)

#ifndef PERIPHERALS_START_ADDRESS
#define PERIPHERALS_START_ADDRESS             0x40000000UL
#endif

#ifndef PERIPHERALS_END_ADDRESS
#define PERIPHERALS_END_ADDRESS               0x5FFFFFFFUL
#endif

#ifndef NVIC_MEM_FAULT_Pos
#define NVIC_MEM_FAULT_Pos                16UL
#endif

#define NVIC_MEM_FAULT_Msk               ( 1UL << NVIC_MEM_FAULT_Pos )

/* ROM */
#ifndef MPU_PROTECT_FLASH_START_ADDR
#define MPU_PROTECT_FLASH_START_ADDR        0x90000000UL
#endif

#ifndef MPU_PROTECT_FLASH_SIZE
#define MPU_PROTECT_FLASH_SIZE            (16384UL * 1024)
#endif

/* RAM */
#ifndef MPU_PROTECT_RAM_START_ADDR
#define MPU_PROTECT_RAM_START_ADDR       0x24000000
#endif

#ifndef MPU_PROTECT_RAM_SIZE
#define MPU_PROTECT_RAM_SIZE             (512UL * 1024)
#endif

#define MPU_PROTECT_RAM_END_ADDR         ((uint32_t)(MPU_PROTECT_RAM_START_ADDR + MPU_PROTECT_RAM_SIZE))

#endif /* __MPU_H__ */
