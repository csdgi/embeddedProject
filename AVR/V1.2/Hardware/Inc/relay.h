
#ifndef __RELAY_H_
#define __RELAY_H_



#include "main.h"

#define OPEN 	1
#define CLOSE	0


#define RELAY_1_PIN		GPIO_PIN_3		//GPIOA
#define RELAY_2_PIN		GPIO_PIN_2		//GPIOA
#define RELAY_3_PIN		GPIO_PIN_2		//GPIOF
#define RELAY_4_PIN		GPIO_PIN_9		//GPIOA
#define RELAY_5_PIN		GPIO_PIN_1		//GPIOB
#define RELAY_6_PIN		GPIO_PIN_0		//GPIOB
#define RELAY_7_PIN		GPIO_PIN_7		//GPIOA

typedef enum 
{
	ALL = 0U,
	RELAY1,
	RELAY2,
	RELAY3,
	RELAY4,
	RELAY5,
	RELAY6,
	RELAY7
	
}Relay_Enum;

extern Relay_Enum Relay_Number;
void power_switch( GPIO_PinState PinState);
void relay_init(void);
void relay_control(uint8_t num);
void relay_close(void);


#endif
