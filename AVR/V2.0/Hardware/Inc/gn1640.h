#ifndef __TM1640_H
#define __TM1640_H   
#include "main.h"

#define u8	uint8_t


#define BRIGHTNESS	0x88

#define SEG1	0xC0
#define SEG2	0xC1
#define SEG3	0xC2

#define SEG4	0xC3
#define SEG5	0xC4
#define SEG6	0xC5

#define SCLK_GPIO_PORT      GPIOF		              	
#define SCLK_GPIO_PIN	    GPIO_PIN_0		        
 
#define DIN_GPIO_PORT       GPIOF		              	
#define DIN_GPIO_PIN	    GPIO_PIN_1

#define TM1640SLK_LOW      HAL_GPIO_WritePin( SCLK_GPIO_PORT, SCLK_GPIO_PIN,GPIO_PIN_RESET)
#define TM1640SLK_HING     HAL_GPIO_WritePin( SCLK_GPIO_PORT, SCLK_GPIO_PIN,GPIO_PIN_SET)
#define TM1640DAT_LOW      HAL_GPIO_WritePin( DIN_GPIO_PORT, DIN_GPIO_PIN,GPIO_PIN_RESET)
#define TM1640DAT_HING     HAL_GPIO_WritePin( DIN_GPIO_PORT, DIN_GPIO_PIN,GPIO_PIN_SET)
extern uint8_t SEG1_CODE[];

void TM1640_Init(void);
void TM1640Start(void);
void TM1640Stop(void);
void TM1640WriteByte(u8 date);
void TM1640_Config(u8 InValue);
void TM1640_SendData(u8 Addr1640,u8 *a,u8 DataLong);
void AIP1820_printf(uint8_t addr,uint8_t data);
void TM1640_Cfg_Display(uint8_t param);
                           
#endif
