#include <stdint.h>
#include "stm32f7xx_hal.h"
#include "hook_i2c_hw_common.h"

I2C_HandleTypeDef I2cHandle;
uint8_t I2cBuffer[2048];

void Error_Handler(void) {
    while(1);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  RCC_PeriphCLKInitTypeDef  RCC_PeriphCLKInitStruct;
  
  /*##-1- Configure the I2C clock source. The clock is derived from the SYSCLK #*/
  RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
  RCC_PeriphCLKInitStruct.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);

  /*##-2- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  /* Enable I2C2 clock */
  __HAL_RCC_I2C2_CLK_ENABLE(); 

  /*##-3- Configure peripheral GPIO ##########################################*/  
  /* I2C TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = GPIO_PIN_4;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
    
  /* I2C RX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = GPIO_PIN_5;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  HAL_I2C_MspInit_Extra(hi2c);
}

/**
  * @brief I2C MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO, DMA and NVIC configuration to their default state
  * @param hi2c: I2C handle pointer
  * @retval None
  */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
  
  /*##-1- Reset peripherals ##################################################*/
  __HAL_RCC_I2C2_FORCE_RESET();
  __HAL_RCC_I2C2_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure I2C Tx as alternate function  */
  HAL_GPIO_DeInit(GPIOH, GPIO_PIN_4);
  /* Configure I2C Rx as alternate function  */
  HAL_GPIO_DeInit(GPIOH, GPIO_PIN_5);

  HAL_I2C_MspDeInit_Extra(hi2c);
}

uint8_t ssd1309_interface_iic_init(void)
{
  /*##-1- Configure the I2C peripheral ######################################*/
  I2cHandle.Instance             = I2C2;
  I2cHandle.Init.Timing          = I2C_TIMING;
  I2cHandle.Init.OwnAddress1     = I2C_ADDRESS;
  I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  I2cHandle.Init.OwnAddress2     = 0xFF;
  I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
  
  if(HAL_I2C_Init(&I2cHandle) != HAL_OK)
  {
      while(1);
  }
  return 0;
}

uint8_t ssd1309_interface_iic_deinit(void)
{
    if(HAL_I2C_DeInit(&I2cHandle) != HAL_OK)
    {
      Error_Handler();
    }
    return 0;
}

