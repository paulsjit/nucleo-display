#include <stdint.h>
#include "stm32f7xx_hal.h"
#include "driver_ssd1309_interface.h"
#include "hook_i2c_hw_common.h"

uint8_t ssd1309_interface_iic_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    I2cBuffer[0] = reg;
    memcpy(&I2cBuffer[1], buf, len);

    if(HAL_I2C_Master_Transmit_IT(&I2cHandle, (uint16_t)addr, I2cBuffer, len + 1)!= HAL_OK)
    {
        Error_Handler();
    }
    while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
    {
    } 

    return 0;
}

void I2C2_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&I2cHandle);
}

void I2C2_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&I2cHandle);
}

void HAL_I2C_MspInit_Extra(I2C_HandleTypeDef *hi2c)
{
  HAL_NVIC_SetPriority(I2C2_ER_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);
  HAL_NVIC_SetPriority(I2C2_EV_IRQn, 0, 2);
  HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
}

void HAL_I2C_MspDeInit_Extra(I2C_HandleTypeDef *hi2c)
{
  HAL_NVIC_DisableIRQ(I2C2_ER_IRQn);
  HAL_NVIC_DisableIRQ(I2C2_EV_IRQn);
}
