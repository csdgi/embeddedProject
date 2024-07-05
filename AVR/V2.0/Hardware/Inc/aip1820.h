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

 
//========������IIC���ߵ��źŶ�д��ʱ��=======
//��������SCL
#define AIP1820_IIC_SCL_HIGH     HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET)
 
//��������SCL
#define AIP1820_IIC_SCL_LOW      HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET)
 
 
//��������SDA
#define AIP1820_IIC_SDA_HIGH     HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_SET)
 
//��������SDA
#define AIP1820_IIC_SDA_LOW      HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_RESET)
 
//����bΪ0ʱ��������SDA����0������SDA
#define AIP1820_IIC_SDA_WR(b)    do{                                       \
                               if(b) HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_SET);   \
                               else  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_RESET); \
                              }while(0)
 
 
//������ȡSDA�ߵ�ƽ״̬������ֵΪ0Ϊ�͵�ƽ����0��Ϊ�ߵ�ƽ
#define AIP1820_IIC_SDA_RD()    HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_0) 
 

//================================

extern uint8_t SEG1_CODE[];
extern uint8_t SEG2_CODE[];
extern uint8_t SEG3_CODE[];

void AIP1820_printf(uint8_t addr,uint8_t data);
void AIP1820_init(void);



#endif



