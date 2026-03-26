#ifndef __HOOK_I2C_HW_COMMON_H__
#define __HOOK_I2C_HW_COMMON_H__

extern I2C_HandleTypeDef I2cHandle;
extern uint8_t I2cBuffer[2048];
void Error_Handler(void);
void HAL_I2C_MspInit_Extra(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MspDeInit_Extra(I2C_HandleTypeDef *hi2c);

#endif
