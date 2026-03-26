#include <stdint.h>
#include "stm32f7xx_hal.h"
#include "driver_ssd1309_interface.h"
#include "hook_i2c_hw_common.h"

uint8_t ssd1309_interface_iic_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    I2cBuffer[0] = reg;
    memcpy(&I2cBuffer[1], buf, len);

    if(HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)addr, I2cBuffer, len + 1, HAL_MAX_DELAY)!= HAL_OK)
    {
        Error_Handler();
    }

    return 0;
}

void HAL_I2C_MspInit_Extra(I2C_HandleTypeDef *hi2c)
{
}

void HAL_I2C_MspDeInit_Extra(I2C_HandleTypeDef *hi2c)
{
}
