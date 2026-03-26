#include <stdint.h>
#include "stm32f7xx_hal.h"
#include "driver_ssd1309_interface.h"
#include "hook_i2c_hw_common.h"
  
static DMA_HandleTypeDef hdma_tx;
static DMA_HandleTypeDef hdma_rx;

uint8_t ssd1309_interface_iic_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    I2cBuffer[0] = reg;
    memcpy(&I2cBuffer[1], buf, len);

    if(HAL_I2C_Master_Transmit_DMA(&I2cHandle, (uint16_t)addr, I2cBuffer, len + 1)!= HAL_OK)
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

void DMA1_Stream3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(I2cHandle.hdmarx);
}

void DMA1_Stream7_IRQHandler(void)
{
  HAL_DMA_IRQHandler(I2cHandle.hdmatx);
}

void HAL_I2C_MspInit_Extra(I2C_HandleTypeDef *hi2c)
{
  __HAL_RCC_DMA1_CLK_ENABLE();

  hdma_tx.Instance                 = DMA1_Stream7;
  hdma_tx.Init.Channel             = DMA_CHANNEL_7;                     
  hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_tx.Init.Mode                = DMA_NORMAL;
  hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
  hdma_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;           /* FIFO mode disabled               */
  hdma_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  hdma_tx.Init.MemBurst = DMA_MBURST_SINGLE;              /* Memory burst                     */
  hdma_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;           /* Peripheral burst                 */

  HAL_DMA_Init(&hdma_tx);   
  
  __HAL_LINKDMA(hi2c, hdmatx, hdma_tx);
    
  hdma_rx.Instance                 = DMA1_Stream3;
  hdma_rx.Init.Channel             = DMA_CHANNEL_7;                     
  hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_rx.Init.Mode                = DMA_NORMAL;
  hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
  hdma_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;           /* FIFO mode disabled               */
  hdma_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  hdma_rx.Init.MemBurst = DMA_MBURST_SINGLE;              /* Memory burst                     */
  hdma_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;           /* Peripheral burst                 */

  HAL_DMA_Init(&hdma_rx);
    
  __HAL_LINKDMA(hi2c, hdmarx, hdma_rx);
    
  HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);   
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  HAL_NVIC_SetPriority(I2C2_ER_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);
  HAL_NVIC_SetPriority(I2C2_EV_IRQn, 0, 2);
  HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
}

void HAL_I2C_MspDeInit_Extra(I2C_HandleTypeDef *hi2c)
{
  HAL_DMA_DeInit(&hdma_tx); 
  HAL_DMA_DeInit(&hdma_rx);
  
  HAL_NVIC_DisableIRQ(DMA1_Stream7_IRQn);
  HAL_NVIC_DisableIRQ(DMA1_Stream3_IRQn);
  HAL_NVIC_DisableIRQ(I2C2_ER_IRQn);
  HAL_NVIC_DisableIRQ(I2C2_EV_IRQn);
}
