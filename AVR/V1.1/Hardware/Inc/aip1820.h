#ifndef __AIP1820_H
#define __AIP1820_H


#include "main.h"
 
 
#define SEG1	0xC5
#define SEG2	0xC3
#define SEG3	0xC4

#define SEG4	0xC2
#define SEG5	0xC1
#define SEG6	0xC0
 
//=====================================

 
//========【配置IIC总线的信号读写和时序】=======
//主机拉高SCL
#define AIP1820_IIC_SCL_HIGH     HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET)
 
//主机拉低SCL
#define AIP1820_IIC_SCL_LOW      HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET)
 
 
//主机拉高SDA
#define AIP1820_IIC_SDA_HIGH     HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_SET)
 
//主机拉低SDA
#define AIP1820_IIC_SDA_LOW      HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_RESET)
 
//参数b为0时主机拉低SDA，非0则拉高SDA
#define AIP1820_IIC_SDA_WR(b)    do{                                       \
                               if(b) HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_SET);   \
                               else  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_RESET); \
                              }while(0)
 
 
//主机读取SDA线电平状态，返回值为0为低电平，非0则为高电平
#define AIP1820_IIC_SDA_RD()    HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_0) 
 

//================================

extern uint8_t SEG1_CODE[];
extern uint8_t SEG2_CODE[];
extern uint8_t SEG3_CODE[];

void AIP1820_printf(uint8_t addr,uint8_t data);
void AIP1820_init(void);



#endif



