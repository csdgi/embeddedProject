#include "adc.h"

ADC_HandleTypeDef             	AdcHandle;
ADC_ChannelConfTypeDef        	sConfig;
TIM_HandleTypeDef    			TimHandle;
TIM_OC_InitTypeDef       		OCConfig;
TIM_MasterConfigTypeDef 		sMasterConfig;

void APP_ADCConfig(void);
void APP_Tim1Config(void);



/**
  * @brief  系统时钟配置函数
  * @param  无
  * @retval 无
  */
void APP_ADCConfig(void)
{
	__HAL_RCC_ADC_CLK_ENABLE();                                                     /* ADC时钟使能 */

	/* 先复位ADC值 */
	__HAL_RCC_ADC_FORCE_RESET();
	__HAL_RCC_ADC_RELEASE_RESET();
	__HAL_RCC_ADC_CLK_ENABLE();                                 /* 使能ADC时钟 */

	AdcHandle.Instance = ADC1;
	if (HAL_ADCEx_Calibration_Start(&AdcHandle) != HAL_OK)                          /* ADC校准 */
	{
		Error_Handler();
	}
	
	AdcHandle.Instance                   = ADC1;                                    /* ADC*/
	AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV1;                /* 模拟ADC时钟源为PCLK*/
	AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;                      /* 转换分辨率12bit*/
	AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;                     /* 数据右对齐 */
	AdcHandle.Init.ScanConvMode          = ADC_SCAN_DIRECTION_FORWARD;            	/* 扫描序列方向：向上*/
	AdcHandle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;                     /* 转换结束标志*/
	AdcHandle.Init.LowPowerAutoWait      = ENABLE;                                  /* 等待转换模式开启 */
	AdcHandle.Init.ContinuousConvMode    = DISABLE;                                 /* 单次转换模式 */
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;                                 /* 使能非连续模式 */
	AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_TRGO;            /* 外部触发转换启动事件为TIM1_TRG0 */
	AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_RISING;  		/* 上升沿进行外部驱动 */
	AdcHandle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;                /* 当过载发生时，覆盖上一个值*/
	AdcHandle.Init.SamplingTimeCommon    = ADC_SAMPLETIME_13CYCLES_5;              	/* 通道采样时间为13.5ADC时钟周期 */
	if (HAL_ADC_Init(&AdcHandle) != HAL_OK)                                         /* ADC初始化*/
	{
		Error_Handler();
	}

	sConfig.Rank         = ADC_RANK_NONE;                                		 	/* 单通道采集 */
	sConfig.Channel      = ADC_CHANNEL_1;                                           /* 设置采样通道 */
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)                      /* 配置ADC通道 */
	{
		Error_Handler();
	}
	if (HAL_ADC_Start_IT(&AdcHandle) != HAL_OK)                                      /*ADC开启，并且开启中断*/
	{
		Error_Handler();
	}                                     
}
/**
  * @brief  Timer1初始化
  * @param  无
  * @retval 无
  */
void APP_Tim1Config(void)
{
	__HAL_RCC_TIM1_CLK_ENABLE();                                        /*TIM1时钟使能*/
	TimHandle.Instance = TIM1;                                          /*TIM1*/
	TimHandle.Init.Period            = 120 - 1;                        	/*TIM1重装载值位8000-1*/
	TimHandle.Init.Prescaler         = 100 - 1;                         /*预分频为100-1*/
	TimHandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;          /*时钟不分配*/
	TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;              /*向上计数*/
	TimHandle.Init.RepetitionCounter = 0;                               /*不重复*/
	TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;  /*自动重装载寄存器没有缓冲*/
	if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)                        /*初始化TIM1*/
	{
		Error_Handler();  
	}
	/*配置TIM1为主机模式*/
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;                /*选择更新事件作为触发源*/
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;        /*主/从模式无作用*/
	HAL_TIMEx_MasterConfigSynchronization(&TimHandle, &sMasterConfig);  /*配置TIM1*/
	if (HAL_TIM_Base_Start(&TimHandle) != HAL_OK)                       /*TIM1时钟启动*/
	{
		Error_Handler();
	}
}

/**
  * @brief  ADC通道切换函数
  * @param  uint8_t channel	采样通道
  * @retval 无
  */
void adc_ch_change(uint8_t channel)
{
	HAL_ADC_Stop_IT(&AdcHandle);
	sConfig.Channel      = channel;  
	HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
	HAL_ADC_Start_IT(&AdcHandle);
}

/**
  * @brief  读取VCC电压
  * @param  无
  * @retval VCC电压
  */
float adc_vref(void)
{
	uint16_t ADC_Value[50] = {0};
	uint8_t  i, j, minIndex;
	uint32_t sum = 0;
	uint16_t temp;
	adc_ch_change(ADC_CHANNEL_VREFINT);			//切换到VREFINT通道
	
	
	
	for(i = 0;i<50;i++)
	{
		ADC_Value[i] = HAL_ADC_GetValue(&AdcHandle);	//读取ADC
	}
	
	for (i = 0; i < 50 - 1; i++)					//排序
	{
        minIndex = i;
        for (j = i + 1; j < 50; j++) {
            if (ADC_Value[j] < ADC_Value[minIndex]) {
                minIndex = j;
            }
        }
        
        temp = ADC_Value[i];
        ADC_Value[i] = ADC_Value[minIndex];
        ADC_Value[minIndex] = temp;
    }	
	
	for(i = 2;i < 50-2;i++)							//计算去除最2个大值2个最小值后的总和
	{
		sum += ADC_Value[i];
	
	}
    //返回采样平均值
    return (4095 * 1.2) /(sum / (50-4));
}


