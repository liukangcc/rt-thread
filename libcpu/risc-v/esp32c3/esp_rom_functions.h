#ifndef __ESP_ROM_FUNCTIONS_H
#define __ESP_ROM_FUNCTIONS_H

#include <stdbool.h>
#include <stdint.h>

extern void esp_rom_intr_matrix_set(int cpu_no, uint32_t model_num, uint32_t intr_num);
extern void esp_rom_uart_attach(void);
extern void esp_rom_uart_tx_wait_idle(uint8_t uart_no);
extern int esp_rom_uart_tx_one_char(uint8_t chr);
extern int esp_rom_uart_rx_one_char(uint8_t *chr);
extern int esp_rom_gpio_matrix_in(uint32_t gpio, uint32_t signal_index, bool inverted);
extern int esp_rom_gpio_matrix_out(uint32_t gpio, uint32_t signal_index,
				bool out_invrted, bool out_enabled_inverted);
extern void esp_rom_ets_set_user_start(uint32_t start);
extern void esprv_intc_int_set_threshold(int priority_threshold);
extern void esp_rom_Cache_Resume_ICache(uint32_t autoload);
extern int esp_rom_Cache_Invalidate_Addr(uint32_t addr, uint32_t size);
extern int esp_rom_gpio_matrix_in(uint32_t gpio, uint32_t signal_index,
				    bool inverted);
extern int esp_rom_gpio_matrix_out(uint32_t gpio, uint32_t signal_index,
				     bool out_inverted,
				     bool out_enabled_inverted);

#endif