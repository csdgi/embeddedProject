#include "relay.h"



void relay_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	__HAL_RCC_GPIOA_CLK_ENABLE();                          /* GPIOA时钟使能 */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;                    /*下拉 */
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;          /* GPIO速度 */

		/* GPIOA初始化 */
	GPIO_InitStruct.Pin = RELAY_1_PIN|RELAY_2_PIN|RELAY_4_PIN|RELAY_7_PIN;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);                	
		
		/* GPIOB初始化 */
	GPIO_InitStruct.Pin = RELAY_5_PIN|RELAY_6_PIN;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);     

		/* GPIOF初始化 */
	GPIO_InitStruct.Pin = RELAY_3_PIN;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct); 
	
	//GPIOF->BSRR = (uint32_t)RELAY_3_PIN;
}


void power_switch( GPIO_PinState PinState)
{
	if (PinState != GPIO_PIN_RESET)
	{
		GPIOA->BSRR = (uint32_t)RELAY_1_PIN;
	}
	else
	{
		GPIOA->BRR = (uint32_t)RELAY_1_PIN;
	}
}


///****关闭所有继电器*****/
//void relay_close(void)
//{
//	GPIOA->BRR = (uint32_t)RELAY_1_PIN;	
//	GPIOF->BRR = (uint32_t)RELAY_3_PIN;							//关闭继电器
//	GPIOA->BRR = (uint32_t)RELAY_4_PIN;
//	GPIOB->BRR = (uint32_t)RELAY_5_PIN;
//	GPIOB->BRR = (uint32_t)RELAY_6_PIN;			
//	GPIOA->BRR = (uint32_t)RELAY_7_PIN;	
//}

void relay_control(uint8_t level)
{
	switch(level)
	{
		//<87
		case 0:
			GPIOA->BRR = (uint32_t)RELAY_2_PIN;	
			GPIOA->BSRR = (uint32_t)RELAY_7_PIN;		
			GPIOA->BRR = (uint32_t)RELAY_1_PIN;	
			GPIOF->BRR = (uint32_t)RELAY_3_PIN;							
			GPIOA->BRR = (uint32_t)RELAY_4_PIN;
			GPIOB->BRR = (uint32_t)RELAY_5_PIN;
			GPIOB->BRR = (uint32_t)RELAY_6_PIN;						
		break;
		//<97
		case 1:			
			GPIOA->BSRR = (uint32_t)RELAY_2_PIN;		
			GPIOA->BSRR = (uint32_t)RELAY_7_PIN;			
			GPIOA->BRR = (uint32_t)RELAY_1_PIN;	
			GPIOF->BRR = (uint32_t)RELAY_3_PIN;							
			GPIOA->BRR = (uint32_t)RELAY_4_PIN;
			GPIOB->BRR = (uint32_t)RELAY_5_PIN;
			GPIOB->BRR = (uint32_t)RELAY_6_PIN;						
		break;
		//<107
		case 2:	
			GPIOA->BRR = (uint32_t)RELAY_2_PIN;
			GPIOB->BSRR = (uint32_t)RELAY_6_PIN;
			GPIOA->BRR = (uint32_t)RELAY_1_PIN;		
			GPIOF->BRR = (uint32_t)RELAY_3_PIN;							
			GPIOA->BRR = (uint32_t)RELAY_4_PIN;
			GPIOB->BRR = (uint32_t)RELAY_5_PIN;
			GPIOA->BRR = (uint32_t)RELAY_7_PIN;
		break;
		//<120
		case 3:		
			GPIOA->BSRR = (uint32_t)RELAY_2_PIN;		
			GPIOB->BSRR = (uint32_t)RELAY_6_PIN;	
			GPIOA->BRR = (uint32_t)RELAY_1_PIN;	
			GPIOF->BRR = (uint32_t)RELAY_3_PIN;							
			GPIOA->BRR = (uint32_t)RELAY_4_PIN;
			GPIOB->BRR = (uint32_t)RELAY_5_PIN;
			GPIOA->BRR = (uint32_t)RELAY_7_PIN;						
		break;
		//<129
		case 4:
			GPIOA->BRR = (uint32_t)RELAY_2_PIN;
			GPIOB->BSRR = (uint32_t)RELAY_5_PIN;
			GPIOA->BRR = (uint32_t)RELAY_1_PIN;	
			GPIOF->BRR = (uint32_t)RELAY_3_PIN;							
			GPIOA->BRR = (uint32_t)RELAY_4_PIN;
			GPIOB->BRR = (uint32_t)RELAY_6_PIN;
			GPIOA->BRR = (uint32_t)RELAY_7_PIN;								
		break;
		//<145
		case 5:
			GPIOA->BSRR = (uint32_t)RELAY_2_PIN;
			GPIOB->BSRR = (uint32_t)RELAY_5_PIN;
			GPIOA->BRR = (uint32_t)RELAY_1_PIN;	
			GPIOF->BRR = (uint32_t)RELAY_3_PIN;							
			GPIOA->BRR = (uint32_t)RELAY_4_PIN;
			GPIOB->BRR = (uint32_t)RELAY_6_PIN;
			GPIOA->BRR = (uint32_t)RELAY_7_PIN;								
		break;
		//<157
		case 6:	
			GPIOA->BRR = (uint32_t)RELAY_2_PIN;
			GPIOA->BSRR = (uint32_t)RELAY_4_PIN;
			GPIOA->BRR = (uint32_t)RELAY_1_PIN;	
			GPIOF->BRR = (uint32_t)RELAY_3_PIN;							
			GPIOB->BRR = (uint32_t)RELAY_5_PIN;
			GPIOB->BRR = (uint32_t)RELAY_6_PIN;
			GPIOA->BRR = (uint32_t)RELAY_7_PIN;							
		break;
		//<175
		case 7:	
			GPIOA->BSRR = (uint32_t)RELAY_2_PIN;		
			GPIOA->BSRR = (uint32_t)RELAY_4_PIN;
			GPIOA->BRR = (uint32_t)RELAY_1_PIN;	
			GPIOF->BRR = (uint32_t)RELAY_3_PIN;							
			GPIOB->BRR = (uint32_t)RELAY_5_PIN;
			GPIOB->BRR = (uint32_t)RELAY_6_PIN;
			GPIOA->BRR = (uint32_t)RELAY_7_PIN;							
		break;
		//<190
		case 8:						
			GPIOA->BRR = (uint32_t)RELAY_2_PIN;
			GPIOA->BRR = (uint32_t)RELAY_4_PIN;
			GPIOA->BRR = (uint32_t)RELAY_1_PIN;	
			GPIOF->BRR = (uint32_t)RELAY_3_PIN;							
			GPIOB->BRR = (uint32_t)RELAY_5_PIN;
			GPIOB->BRR = (uint32_t)RELAY_6_PIN;
			GPIOA->BRR = (uint32_t)RELAY_7_PIN;							
		break;
		//<212
		case 9:						
			GPIOF->BRR = (uint32_t)RELAY_3_PIN;
			GPIOA->BSRR = (uint32_t)RELAY_2_PIN;
			GPIOA->BRR = (uint32_t)RELAY_1_PIN;							
			GPIOA->BRR = (uint32_t)RELAY_4_PIN;
			GPIOB->BRR = (uint32_t)RELAY_5_PIN;
			GPIOB->BRR = (uint32_t)RELAY_6_PIN;
			GPIOA->BRR = (uint32_t)RELAY_7_PIN;								
		break;
		//<237
		case 10:
			GPIOF->BSRR = (uint32_t)RELAY_3_PIN;						
			GPIOA->BRR = (uint32_t)RELAY_2_PIN;
			GPIOA->BRR = (uint32_t)RELAY_1_PIN;			
			GPIOA->BRR = (uint32_t)RELAY_4_PIN;
			GPIOB->BRR = (uint32_t)RELAY_5_PIN;
			GPIOB->BRR = (uint32_t)RELAY_6_PIN;
			GPIOA->BRR = (uint32_t)RELAY_7_PIN;								
		break;
		//>237
		case 11:						
			GPIOF->BSRR = (uint32_t)RELAY_3_PIN;						
			GPIOA->BSRR = (uint32_t)RELAY_2_PIN;
			GPIOA->BRR = (uint32_t)RELAY_1_PIN;			
			GPIOA->BRR = (uint32_t)RELAY_4_PIN;
			GPIOB->BRR = (uint32_t)RELAY_5_PIN;
			GPIOB->BRR = (uint32_t)RELAY_6_PIN;
			GPIOA->BRR = (uint32_t)RELAY_7_PIN;								
		break;
		default:
		break;
	}
	
}





