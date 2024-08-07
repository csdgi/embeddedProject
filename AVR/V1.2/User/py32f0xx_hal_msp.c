/**
  ******************************************************************************
  * @file    py32f0xx_hal_msp.c
  * @author  MCU Application Team
  * @brief   This file provides code for the MSP Initialization
  *          and de-Initialization codes.
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

/**
  * @brief 初始化全局MSP
  */
void HAL_MspInit(void)
{
//	GPIO_InitTypeDef          GPIO_InitStruct;				//设置PA13按键输入
  //BSP_LED_Init(LED_GREEN);
//	__HAL_RCC_GPIOA_CLK_ENABLE();
//	GPIO_InitStruct.Pin = GPIO_PIN_13;
//	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//	GPIO_InitStruct.Pull = GPIO_PULLUP;
//	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
  * @brief 初始化ADC相关MSP
  */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef          GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO clock ****************************************/
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /* ADC1 Periph clock enable */
  __HAL_RCC_ADC_CLK_ENABLE();
  __HAL_RCC_ADC_FORCE_RESET();
  __HAL_RCC_ADC_RELEASE_RESET();
  /*##- 2- Configure peripheral GPIO #########################################*/
  /* ADC Channel GPIO pin configuration */
  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_0 | GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(ADC_COMP_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(ADC_COMP_IRQn);
}

/**
  * @brief 初始化全局MSP
  */


/************************ (C) COPYRIGHT Puya *****END OF FILE****/
