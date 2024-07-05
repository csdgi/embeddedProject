/**
  ******************************************************************************
  * @file    main.c
  * @author  MCU Application Team
  * @brief   Main program body
  * @date
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) Puya Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "py32f0xx_hal_conf.h"
#include "relay.h"
#include "app.h"
#include "aip1820.h"	

static void APP_SystemClockConfig(void);
volatile uint8_t task_tick = 0; //任务心跳

int main(void)
{
	
	FLASH_OBProgramInitTypeDef OBInitCfg;
	HAL_Init();
	APP_SystemClockConfig();
	HAL_InitTick(TICK_INT_PRIORITY);
	/* 初始化所有外设，Flash接口*/
//	GPIO_InitTypeDef          GPIO_InitStruct;
//	
//	__HAL_RCC_GPIOA_CLK_ENABLE();
//	GPIO_InitStruct.Pin = GPIO_PIN_13;
//	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//	GPIO_InitStruct.Pull = GPIO_PULLUP;
//	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	if( READ_BIT(FLASH->OPTR, FLASH_OPTR_NRST_MODE) == 0)			
	{
		 HAL_FLASH_Unlock();                                 /* 解锁FLASH */
		 HAL_FLASH_OB_Unlock();                              /* 解锁FLASH OPTION  */

		 OBInitCfg.OptionType = OPTIONBYTE_USER;
		 OBInitCfg.USERType = OB_USER_BOR_EN | OB_USER_BOR_LEV | OB_USER_IWDG_SW | OB_USER_NRST_MODE | OB_USER_nBOOT1;
		 /* BOR不使能/BOR上升3.2,下降3.1/软件模式看门狗/RST改为GPIO/System memory作为启动区 */
		 OBInitCfg.USERConfig = OB_BOR_DISABLE | OB_BOR_LEVEL_3p1_3p2 | OB_IWDG_SW |  OB_RESET_MODE_GPIO | OB_BOOT1_SYSTEM;
		 /* 启动option byte编程 */
		 HAL_FLASH_OBProgram(&OBInitCfg);
		 HAL_FLASH_Lock();                                   /* 锁定FLASH */
		 HAL_FLASH_OB_Lock();                                /* 锁定FLASH OPTION */
		 /* 产生一个复位，option byte装载 */
		 HAL_FLASH_OB_Launch();
	}
	APP_Tim1Config();
	APP_ADCConfig();
	relay_init();
	//BSP_USART_Config();
	AIP1820_init();
	Init_Task();

	while (1)
	{
		if(task_tick == 0)
		{
			task_tick =1;
			TASK_Remarks();		//任务调度器
		}
		TASK_Process();			//执行任务
	}
}

 
static void APP_SystemClockConfig(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
																
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE				
									| RCC_OSCILLATORTYPE_HSI
									| RCC_OSCILLATORTYPE_LSI;					// 设置振荡源类型
	
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;									// 开启内部高速时钟
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_24MHz;			// 设置内部高速时钟频率为24MHz
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;									// 内部高速时钟不分频
	RCC_OscInitStruct.HSEState = RCC_HSE_OFF;									// 关闭其它时钟: 外置高速, 内置低速, 外置低速
	RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
	
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)						// 应用设置
	{
		Error_Handler();
	}
	
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK
								| RCC_CLOCKTYPE_SYSCLK
								| RCC_CLOCKTYPE_PCLK1;							// 修改时钟后, 重新初始化 AHB,APB 时钟
	
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;						// 设置 SYSCLK 时钟源为内部高速时钟
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;							// AHB 不分频
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;							// APB 不分频
	
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)		// 启用设置, flash等待时间为0
	{
		Error_Handler();
	}
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  输出产生断言错误的源文件名及行号
  * @param  file：源文件名指针
  * @param  line：发生断言错误的行号
  * @retval 无
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* 用户可以根据需要添加自己的打印信息,
     例如: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* 无限循环 */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT Puya *****END OF FILE****/
