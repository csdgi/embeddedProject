#include "adc.h"

ADC_HandleTypeDef             	AdcHandle;
ADC_ChannelConfTypeDef        	sConfig;
TIM_HandleTypeDef    			TimHandle;
TIM_OC_InitTypeDef       		OCConfig;
TIM_MasterConfigTypeDef 		sMasterConfig;

void APP_ADCConfig(void);
void APP_Tim1Config(void);



/**
  * @brief  ϵͳʱ�����ú���
  * @param  ��
  * @retval ��
  */
void APP_ADCConfig(void)
{
	__HAL_RCC_ADC_CLK_ENABLE();                                                     /* ADCʱ��ʹ�� */

	/* �ȸ�λADCֵ */
	__HAL_RCC_ADC_FORCE_RESET();
	__HAL_RCC_ADC_RELEASE_RESET();
	__HAL_RCC_ADC_CLK_ENABLE();                                 /* ʹ��ADCʱ�� */

	AdcHandle.Instance = ADC1;
	if (HAL_ADCEx_Calibration_Start(&AdcHandle) != HAL_OK)                          /* ADCУ׼ */
	{
		Error_Handler();
	}
	
	AdcHandle.Instance                   = ADC1;                                    /* ADC*/
	AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV1;                /* ģ��ADCʱ��ԴΪPCLK*/
	AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;                      /* ת���ֱ���12bit*/
	AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;                     /* �����Ҷ��� */
	AdcHandle.Init.ScanConvMode          = ADC_SCAN_DIRECTION_FORWARD;            	/* ɨ�����з�������*/
	AdcHandle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;                     /* ת��������־*/
	AdcHandle.Init.LowPowerAutoWait      = ENABLE;                                  /* �ȴ�ת��ģʽ���� */
	AdcHandle.Init.ContinuousConvMode    = DISABLE;                                 /* ����ת��ģʽ */
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;                                 /* ʹ�ܷ�����ģʽ */
	AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_TRGO;            /* �ⲿ����ת�������¼�ΪTIM1_TRG0 */
	AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_RISING;  		/* �����ؽ����ⲿ���� */
	AdcHandle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;                /* �����ط���ʱ��������һ��ֵ*/
	AdcHandle.Init.SamplingTimeCommon    = ADC_SAMPLETIME_13CYCLES_5;              	/* ͨ������ʱ��Ϊ13.5ADCʱ������ */
	if (HAL_ADC_Init(&AdcHandle) != HAL_OK)                                         /* ADC��ʼ��*/
	{
		Error_Handler();
	}

	sConfig.Rank         = ADC_RANK_NONE;                                		 	/* ��ͨ���ɼ� */
	sConfig.Channel      = ADC_CHANNEL_1;                                           /* ���ò���ͨ�� */
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)                      /* ����ADCͨ�� */
	{
		Error_Handler();
	}
	if (HAL_ADC_Start_IT(&AdcHandle) != HAL_OK)                                      /*ADC���������ҿ����ж�*/
	{
		Error_Handler();
	}                                     
}
/**
  * @brief  Timer1��ʼ��
  * @param  ��
  * @retval ��
  */
void APP_Tim1Config(void)
{
	__HAL_RCC_TIM1_CLK_ENABLE();                                        /*TIM1ʱ��ʹ��*/
	TimHandle.Instance = TIM1;                                          /*TIM1*/
	TimHandle.Init.Period            = 120 - 1;                        	/*TIM1��װ��ֵλ8000-1*/
	TimHandle.Init.Prescaler         = 100 - 1;                         /*Ԥ��ƵΪ100-1*/
	TimHandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;          /*ʱ�Ӳ�����*/
	TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;              /*���ϼ���*/
	TimHandle.Init.RepetitionCounter = 0;                               /*���ظ�*/
	TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;  /*�Զ���װ�ؼĴ���û�л���*/
	if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)                        /*��ʼ��TIM1*/
	{
		Error_Handler();  
	}
	/*����TIM1Ϊ����ģʽ*/
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;                /*ѡ������¼���Ϊ����Դ*/
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;        /*��/��ģʽ������*/
	HAL_TIMEx_MasterConfigSynchronization(&TimHandle, &sMasterConfig);  /*����TIM1*/
	if (HAL_TIM_Base_Start(&TimHandle) != HAL_OK)                       /*TIM1ʱ������*/
	{
		Error_Handler();
	}
}

/**
  * @brief  ADCͨ���л�����
  * @param  uint8_t channel	����ͨ��
  * @retval ��
  */
void adc_ch_change(uint8_t channel)
{
	HAL_ADC_Stop_IT(&AdcHandle);
	sConfig.Channel      = channel;  
	HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
	HAL_ADC_Start_IT(&AdcHandle);
}

/**
  * @brief  ��ȡVCC��ѹ
  * @param  ��
  * @retval VCC��ѹ
  */
float adc_vref(void)
{
	uint16_t ADC_Value[50] = {0};
	uint8_t  i, j, minIndex;
	uint32_t sum = 0;
	uint16_t temp;
	adc_ch_change(ADC_CHANNEL_VREFINT);			//�л���VREFINTͨ��
	
	
	
	for(i = 0;i<50;i++)
	{
		ADC_Value[i] = HAL_ADC_GetValue(&AdcHandle);	//��ȡADC
	}
	
	for (i = 0; i < 50 - 1; i++)					//����
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
	
	for(i = 2;i < 50-2;i++)							//����ȥ����2����ֵ2����Сֵ����ܺ�
	{
		sum += ADC_Value[i];
	
	}
    //���ز���ƽ��ֵ
    return (4095 * 1.2) /(sum / (50-4));
}


