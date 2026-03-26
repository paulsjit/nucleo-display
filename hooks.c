#include <stdint.h>
#include "stm32f7xx_hal.h"
#include "driver_ssd1309_interface.h"

uint8_t ssd1309_interface_spi_cmd_data_gpio_write(uint8_t value) { return 0; }
uint8_t ssd1309_interface_reset_gpio_init(void) { return 0; }
uint8_t ssd1309_interface_reset_gpio_deinit(void) { return 0; }
uint8_t ssd1309_interface_reset_gpio_write(uint8_t value) { return 0; }
uint8_t ssd1309_interface_spi_cmd_data_gpio_init(void) { return 0; }
uint8_t ssd1309_interface_spi_cmd_data_gpio_deinit(void) { return 0; }
uint8_t ssd1309_interface_spi_write_cmd(uint8_t *buf, uint16_t len) { return 0; }
uint8_t ssd1309_interface_spi_deinit(void) { return 0; }
uint8_t ssd1309_interface_spi_init(void) { return 0; }
void ssd1309_interface_debug_print(const char *const fmt, ...) {}
void ssd1309_interface_delay_ms(uint32_t ms) { HAL_Delay(ms); }
