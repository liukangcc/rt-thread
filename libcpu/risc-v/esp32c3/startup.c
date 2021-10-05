/* Include esp-idf headers first to avoid redefining BIT() macro */
#include <soc/rtc_cntl_reg.h>
#include <soc/timer_group_reg.h>
#include <soc/gpio_reg.h>
#include <soc/syscon_reg.h>
#include <soc/system_reg.h>
#include <soc/cache_memory.h>
#include "hal/soc_ll.h"
#include "esp_spi_flash.h"
#include <soc/interrupt_reg.h>
#include "cpuport.h"
#include "esp_rom_functions.h"

extern int entry(void);

/*
 * This is written in C rather than assembly since, during the port bring up,
 * RTT is being booted by the Espressif bootloader.  With it, the C stack
 * is already set up.
 */
void __attribute__((section(".iram1"))) __start(void)
{
	volatile uint32_t *wdt_rtc_protect = (uint32_t *)RTC_CNTL_WDTWPROTECT_REG;
	volatile uint32_t *wdt_rtc_reg = (uint32_t *)RTC_CNTL_WDTCONFIG0_REG;

	/* Configure the global pointer register
	 * (This should be the first thing startup does, as any other piece of code could be
	 * relaxed by the linker to access something relative to __global_pointer$)
	 */
	__asm__ __volatile__(".option push\n"
						".option norelax\n"
						"la gp, __global_pointer$\n"
						".option pop");

	__asm__ __volatile__("la t0, _esp32c3_vector_table\n"
						"csrw mtvec, t0\n");

	/* Disable normal interrupts. */
	csr_read_clear(mstatus, MSTATUS_MIE);

	/* ESP-IDF 2nd stage bootloader enables RTC WDT to check on startup sequence
	 * related issues in application. Hence disable that as we are about to start
	 * Zephyr environment.
	 */
	*wdt_rtc_protect = RTC_CNTL_WDT_WKEY_VALUE;
	*wdt_rtc_reg &= ~RTC_CNTL_WDT_EN;
	*wdt_rtc_protect = 0;

	/* Configure the Cache MMU size for instruction and rodata in flash. */
	extern uint32_t esp_rom_cache_set_idrom_mmu_size(uint32_t irom_size,
			uint32_t drom_size);

	extern int _rodata_reserved_start;
	uint32_t rodata_reserved_start_align =
		(uint32_t)&_rodata_reserved_start & ~(MMU_PAGE_SIZE - 1);
	uint32_t cache_mmu_irom_size =
		((rodata_reserved_start_align - SOC_DROM_LOW) / MMU_PAGE_SIZE) *
			sizeof(uint32_t);

	esp_rom_cache_set_idrom_mmu_size(cache_mmu_irom_size,
		CACHE_DROM_MMU_MAX_END - cache_mmu_irom_size);

	/* Enable wireless phy subsystem clock,
	 * This needs to be done before the kernel starts
	 */
	REG_CLR_BIT(SYSTEM_WIFI_CLK_EN_REG, SYSTEM_WIFI_CLK_SDIOSLAVE_EN);
	SET_PERI_REG_MASK(SYSTEM_WIFI_CLK_EN_REG, SYSTEM_WIFI_CLK_EN);

    entry();
}