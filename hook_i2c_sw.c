#include <stdint.h>
#include <stdarg.h>
#include "stm32f7xx_hal.h"
#include "driver_ssd1309_interface.h"

#define PBout(n, x)       do { \
    if (x) \
       GPIOH->ODR |= (1 << (n)); \
    else \
       GPIOH->ODR &= (~(1 << (n))); \
} while(0)
#define PBin(n)           ((GPIOH->IDR & (1 << (n))) >> (n))
#define SDA_IN()          {GPIOH->MODER &= ~(3 << (9 * 2)); GPIOH->MODER |= 0 << 9 * 2;}
#define SDA_OUT()         {GPIOH->MODER &= ~(3 << (9 * 2)); GPIOH->MODER |= 1 << 9 * 2;}
#define IIC_SCL(x)        PBout(4, (x))
#define IIC_SDA(x)        PBout(5, (x))
#define READ_SDA          PBin(5)

static uint8_t delay_us_init(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  TIM2->CR1 |= TIM_CR1_CEN;
    
  return 0;
}

static void delay_us(uint32_t us)
{
    uint32_t ticks = us * 216;
    uint32_t told = TIM2->CNT;
    uint32_t tnow;
    
    /* delay */
    while (1)
    {
        tnow = TIM2->CNT;
        uint32_t diff = (uint32_t)((int32_t)tnow - (int32_t)told);
        if(diff >= ticks)
            break;
    }
}

static void a_iic_start(void)
{
    SDA_OUT();
    IIC_SDA(1);
    IIC_SCL(1);
    delay_us(4);
    IIC_SDA(0);
    delay_us(4);
    IIC_SCL(0);
}

static void a_iic_stop(void)
{
    SDA_OUT();
    IIC_SCL(0);
    IIC_SDA(0);
    delay_us(4);
    IIC_SCL(1);
    delay_us(4);
    IIC_SDA(1);
    delay_us(4);
}

static uint8_t a_iic_wait_ack(void)
{
    uint16_t uc_err_time = 0;
    
    SDA_IN();
    IIC_SDA(1); 
    delay_us(1);
    IIC_SCL(1); 
    delay_us(1);
    while (READ_SDA != 0)
    {
        uc_err_time++;
        if (uc_err_time > 250)
        {
            a_iic_stop();
            
            return 1;
        }
    }
    IIC_SCL(0);
    
    return 0;
}

static void a_iic_send_byte(uint8_t txd)
{
    uint8_t t;
    
    SDA_OUT();
    IIC_SCL(0);
    for (t = 0; t < 8; t++)
    {
        IIC_SDA((txd & 0x80) >> 7);
        txd <<= 1;
        delay_us(2);
        IIC_SCL(1);
        delay_us(2);
        IIC_SCL(0);
        delay_us(2);
    }
}

uint8_t ssd1309_interface_iic_init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    /* enable iic gpio clock */
    __HAL_RCC_GPIOH_CLK_ENABLE();
    
    /* iic gpio init */
    GPIO_Initure.Pin = GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;   
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);
    
    /* set sda high */
    IIC_SDA(1);
    
    /* set scl high */
    IIC_SCL(1);

    delay_us_init();
    
    return 0;
}

uint8_t ssd1309_interface_iic_deinit(void)
{
    /* iic gpio deinit */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_4 | GPIO_PIN_5);
    
    return 0;
}

uint8_t ssd1309_interface_iic_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    uint16_t i; 
    
    /* send a start */
    a_iic_start();
    
    /* send the write addr */
    a_iic_send_byte(addr);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();
        
        return 1;
    }
    
    /* send the reg */
    a_iic_send_byte(reg);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();
        
        return 1;
    }
    
    /* write the data */
    for (i = 0; i < len; i++)
    {
        /* send one byte */
        a_iic_send_byte(buf[i]);
        if (a_iic_wait_ack() != 0)
        {
            a_iic_stop(); 
            
            return 1;
        }
    }
    
    /* send a stop */
    a_iic_stop();
    
    return 0;
}
