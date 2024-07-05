#include "app.h"
#include <math.h>
#include "adc.h"
#include "gn1640.h"
#include "relay.h"

#define GAIN_FACTOR 	272.963		//电压增益系数
#define VDDREF 			5.00		//ADC参考电压	
#define DC_ZERO	    	2.500		//直流零基准点
#define AC_ZERO 		2047		//交流零点
#define ELE_ZERO		3.0			//电流零点
#define ADC_BUFF_SIZE	40			//ADC采样点数
#define ELE_RES			47.0			//电流采样电阻

#define AC_IN_CHANNEL	ADC_CHANNEL_1	//输入电压检测通道
#define AC_OUT_CHANNEL	ADC_CHANNEL_0	//输出电压检测通道
#define AC_I_CHANNEL	ADC_CHANNEL_6	//电流检测通道
#define UVP_DELAY  		200				//欠压判断时间 2S
#define RELAY_DELAY		1				//10ms
#define MAX_ELE			7				//电流上限7A
#define LOW_POWER		65				//电压下限65V
#define HIGH_POWER		280				//电压上限280V


extern ADC_HandleTypeDef             AdcHandle;

__IO uint8_t ch1ConversionFlag = 0;			//通道1传输完成
__IO uint8_t ch0ConversionFlag = 0;			//通道0传输完成
__IO uint8_t ch6ConversionFlag = 0;			//通道6传输完成
uint16_t adValue_buffer[ADC_BUFF_SIZE];		//AD值

__IO uint8_t adjustState = 0;				//调节器状态
uint8_t led2_twink = 0;

uint8_t power_p;							
uint8_t warning_twink = 1;
uint8_t Init_State = 0;					
uint16_t init_delay = 7;
					

uint16_t calculate_rms(uint16_t adc_values[], uint8_t num_samples,float gain_factor); 
float electric_read(uint16_t adc_values[]);
void Adjust_Task(void);
void Rms_Task(void);
void Display_Task(void);
void Key_Task(void);
void Protection_Task(void);

struct {
	uint16_t 	in_rms;				
	uint16_t 	out_rms;			
	uint8_t 	v_level; 
	uint8_t		ele_protect;
	uint16_t    in_show_rms;
	float 	electric;	
} AVR_Handle;

static _TASK_InfoType sg_tTaskInfo[TASKS_MAX] = {
	{0,1,1,Rms_Task},
	{0,100,100,Display_Task},
	{0,1000,1000,Key_Task},
	{0,10,10,Protection_Task}
};

/*****************************************************
*函数名称：void TASK_Process(void)
*函数功能：主循环任务执行
*入口参数：void
*出口参数：void
*****************************************************/
void TASK_Process(void) {
	
	uint8_t i;
	
	for (i = 0; i < TASKS_MAX; i++) {
		if (sg_tTaskInfo[i].m_runFlag) {
			sg_tTaskInfo[i].m_pTaskHook();
			sg_tTaskInfo[i].m_runFlag = 0;
		}
	}
}

/*****************************************************
*函数名称：void TASK_Remarks(void)
*函数功能：定时器任务调度
*入口参数：void
*出口参数：void
*****************************************************/
void TASK_Remarks(void) {
	uint8_t i;
	for (i = 0; i < TASKS_MAX; i++) {
		if (sg_tTaskInfo[i].m_timer) {
			sg_tTaskInfo[i].m_timer--;
			if (sg_tTaskInfo[i].m_timer == 0) {
				sg_tTaskInfo[i].m_timer = sg_tTaskInfo[i].m_itvTime;
				sg_tTaskInfo[i].m_runFlag = 1;
			}
		}
	}
}

uint8_t adc_indx = 0;
/**
  * @brief 获取当前的ADC值
  * @param  无
  * @retval 无
  */
void ADC_ConvCplt(ADC_HandleTypeDef *hadc)
{	
	uint16_t adc_value = 0;

	adc_value = hadc->Instance->DR;
	if(hadc->Instance->CHSELR == ADC_CHSELR_CHANNEL(AC_IN_CHANNEL))					//通道1
	{
		if( (adc_value > (AC_ZERO-10) && adc_value < (AC_ZERO+10)) && adjustState == 2)	//判断零点
		{
			relay_control(AVR_Handle.v_level);	
			adjustState = 0;
			return;
		}
		
		if(ch1ConversionFlag == 0 && adjustState == 0)
		{
			adValue_buffer[adc_indx] = adc_value;
			adc_indx++;
			if(adc_indx>=ADC_BUFF_SIZE)
			{
				adc_indx = 0;
				ch1ConversionFlag = 1;
			}
		}
	}
	else if(hadc->Instance->CHSELR == ADC_CHSELR_CHANNEL(AC_OUT_CHANNEL))	//通道0
	{
		if(ch0ConversionFlag == 0)
		{
			adValue_buffer[adc_indx] = adc_value;
			adc_indx++;
			if(adc_indx>=ADC_BUFF_SIZE)
			{
				adc_indx = 0;
				ch0ConversionFlag = 1;
			}
		}
	}
	else if(hadc->Instance->CHSELR == ADC_CHSELR_CHANNEL(AC_I_CHANNEL))		//电流检测通道
	{
		if(ch6ConversionFlag == 0)
		{
			adValue_buffer[adc_indx] = adc_value;
			adc_indx++;
			if(adc_indx>=ADC_BUFF_SIZE)
			{
				adc_indx = 0;
				ch6ConversionFlag = 1;							
			}
		}
	}
}  

/**
  * @brief ADC中断处理函数，2KHZ频率进入
  * @param  无
  * @retval 无
  */
void ADC_COMP_IRQHandler(void)
{
	ADC_ConvCplt(&AdcHandle);
	__HAL_ADC_CLEAR_FLAG(&AdcHandle, (ADC_FLAG_EOC | ADC_FLAG_EOS) );
}

/**
  * @brief  根据AD值计算交流有效值
  * @param  adc_values[]，AD值；num_samples，采样点数
  * @retval uint16_t rms，交流有效值,四舍五入取整
  */
uint16_t calculate_rms(uint16_t adc_values[], uint8_t num_samples,float gain_factor) 
{
    float sum_of_squares = 0;
    
    for (uint8_t i = 0; i < num_samples; i++) 						
	{
        sum_of_squares += pow((adc_values[i]*VDDREF/4095.0 - DC_ZERO)*gain_factor, 2);	
    }   
    float rms = sqrt(sum_of_squares / num_samples);				
    
    return (uint16_t) (rms + 0.5);								
}

#define N 	5	//消抖滤波系数

/**
  * @brief  消抖滤波算法
  * @param  uint16_t new_value 输入数据
  * @retval uint16_t 滤波后数据
  */
uint16_t debouncefilter(uint16_t new_value)
{
	static uint8_t i = 0;
	static uint16_t last_value = 0;
	
	if(new_value != last_value)		
	{
		i++;
		if(i > N)
		{	
			i = 0;
			last_value = new_value;
			return	new_value;
		}
	}
	else
		i = 0;
	return last_value;
}

uint16_t debouncefilter2(uint16_t new_value)
{
	static uint8_t i = 0;
	static uint16_t last_value = 0;
	
	if(new_value != last_value)		
	{
		i++;
		if(i > N)
		{	
			i = 0;
			last_value = new_value;
			return	new_value;
		}
	}
	else
		i = 0;
	return last_value;
}




/**
  * @brief  RMS计算任务
  * @param  无
  * @retval 无
  */
void Rms_Task(void)
{
	static uint8_t run_count = 0;
	//static uint8_t run_count2 = 0;
	//static uint8_t mutuallyLOCK = 0;
	
	if(ch1ConversionFlag == 1)									
	{	
		run_count++;
		
		AVR_Handle.in_rms = calculate_rms(adValue_buffer,ADC_BUFF_SIZE,GAIN_FACTOR);	//计算交流有效值
		if(AVR_Handle.in_rms >= 249)
		{
			AVR_Handle.in_rms =(uint16_t) (AVR_Handle.in_rms*1.3573 - 88.287 +0.5);	//大于249V误差修正
		}
		Adjust_Task();		//调节电压
		AVR_Handle.in_show_rms = debouncefilter(AVR_Handle.in_rms);
		if(AVR_Handle.in_show_rms < 50)						//输入电压低于50V，只显示50V
		{
			AVR_Handle.in_show_rms = 50;
		}
		
		if(adjustState == 0 && run_count >= 6)		
		{
			run_count = 0;	
			adc_ch_change(AC_OUT_CHANNEL);					//ADC切换到输出电压检测通道
		}
		else
		{
			if(run_count == 3 && adjustState == 0)				
				{adc_ch_change(AC_I_CHANNEL);}				//ADC切换到电流检测通道
		}
		ch1ConversionFlag = 0;		
	}
	if(ch0ConversionFlag == 1)									
	{
		//mutuallyLOCK = 0;
		adc_indx = 0;
		adc_ch_change(AC_IN_CHANNEL);
		AVR_Handle.out_rms = calculate_rms(adValue_buffer,ADC_BUFF_SIZE,GAIN_FACTOR);
		AVR_Handle.out_rms = debouncefilter2(AVR_Handle.out_rms);
		ch0ConversionFlag = 0;
	}	
		//电流判断

	if(ch6ConversionFlag == 1)
	{
		//mutuallyLOCK = 0;
		adc_indx = 0;
		AVR_Handle.electric = electric_read(adValue_buffer);
		//AVR_Handle.electric = debouncefilter2(AVR_Handle.electric);
		adc_ch_change(AC_IN_CHANNEL);	
		ch6ConversionFlag = 0;
	}	
}

/**
  * @brief  电流计算
  * @param  AD值
  * @retval 电流A
  */
float electric_read(uint16_t adc_values[])
{

	float sum = 0;
	uint8_t i = 0;
	float electric = 0.0;
	
	for(i=0;i<ADC_BUFF_SIZE;i++)
	{
		sum += pow((adc_values[i]*VDDREF/4095.0 - ELE_ZERO),2);
	}
	electric = sqrt(sum/ADC_BUFF_SIZE)/ELE_RES;
	return electric*1000.0;
}

/**
  * @brief  异常保护任务
  * @param  无
  * @retval 无
  */
void Protection_Task(void)
{
	static uint16_t lv_time = 0;
	
		//电压判断
	if(AVR_Handle.in_rms < LOW_POWER || AVR_Handle.in_rms > HIGH_POWER)
	{
		if(power_p == 0 || power_p == 3)
			power_p = 1;		
		if(power_p == 1)
		{
			if(AVR_Handle.in_rms > HIGH_POWER)	//过电压
			{
				power_switch(GPIO_PIN_RESET);	//切断电源
				power_p = 2;
			}
			
			if(AVR_Handle.in_rms < LOW_POWER)	//欠电压
			{
				if(++lv_time >= UVP_DELAY)		//延时2秒
				{
					power_switch(GPIO_PIN_RESET);	//切断电源
					lv_time = 0;
					power_p = 2;
				}
			}
		}	
	}
	else if(AVR_Handle.in_rms > LOW_POWER + 3 && AVR_Handle.in_rms < HIGH_POWER - 2)
	{
		if(power_p == 1)
		{
			power_p = 0;
			warning_twink = 1;
		}
		else if(power_p == 2)
		{
			warning_twink = 1;
			power_p = 3;
		}
	}
	if(power_p == 0 && Init_State == 1)		//电压恢复正常
	{
		power_switch(GPIO_PIN_SET);				//打开输出
	}
	
	if(AVR_Handle.electric > MAX_ELE)			//过电流判断
	{
		power_p = 4;
	}
	if(power_p == 4 && AVR_Handle.electric < MAX_ELE && AVR_Handle.ele_protect < 2)	//恢复
	{
		warning_twink = 1;
		power_p = 0;
	}
}
uint16_t count = 0;
/**
  * @brief  倒计时显示
  * @param  时间 秒
  * @retval 完成：0；未完成：1
  */
uint8_t count_down(uint16_t time)
{
	uint16_t num = 0;
	count ++;
	if(count%5 == 0)
		led2_twink = !led2_twink;

	if(count >= (time-1)*10)
	{
		count = 0;
		led2_twink = 0;
		return 0;
	}
	else
	{
		num = (time*10 - count)/10;
		AIP1820_printf(SEG4,SEG1_CODE[num/100]);					
		AIP1820_printf(SEG5,SEG1_CODE[num/10%10]);
		AIP1820_printf(SEG6,SEG1_CODE[num%10]);
	}
	return 1;
}



/**
  * @brief  数码管显示任务
  * @param  无
  * @retval 无
  */
void Display_Task(void)
{
	static uint16_t time_count = 0;
	static uint16_t time_count2 = 0;
	static uint8_t	time_count3 = 0;
	static uint8_t 	init_flag = 0;
	if(power_p != 4)
	{
		time_count2 = 0;
		if(power_p == 1 || power_p == 2)
		{
			time_count++;
			if(time_count%5 == 1)
			{
				warning_twink = !warning_twink;
			}
		}
		if(warning_twink == 1)
		{
			if(power_p == 1 || power_p == 2 )
			{		
				AIP1820_printf(0xC6,0x05);
			}
			else
			{		
				if(led2_twink == 1)
				{	
					AIP1820_printf(0xC6,0x03);
				}
				else
				{
					AIP1820_printf(0xC6,0x01);
				}
			}
			AIP1820_printf(SEG1,SEG1_CODE[AVR_Handle.in_show_rms/100]);
			AIP1820_printf(SEG2,SEG1_CODE[AVR_Handle.in_show_rms/10%10]);
			AIP1820_printf(SEG3,SEG1_CODE[AVR_Handle.in_show_rms%10]); 

			if(power_p == 0 )
			{
				if(Init_State == 1)
				{
					if(AVR_Handle.out_rms >198 && AVR_Handle.out_rms < 244)
					{
						AIP1820_printf(SEG4,SEG1_CODE[2]);					
						AIP1820_printf(SEG5,SEG1_CODE[2]);
						AIP1820_printf(SEG6,SEG1_CODE[0]); 
					}
					else
					{
						AIP1820_printf(SEG4,SEG1_CODE[AVR_Handle.out_rms/100]);					
						AIP1820_printf(SEG5,SEG1_CODE[AVR_Handle.out_rms/10%10]);
						AIP1820_printf(SEG6,SEG1_CODE[AVR_Handle.out_rms%10]);	
					}
				}
				else
				{
					if(count_down(init_delay) == 0 || init_flag == 1)
					{
						Init_State = 1;
						if(AVR_Handle.in_rms > LOW_POWER && AVR_Handle.in_rms < HIGH_POWER)
						{	power_p = 0;}
					}
				}
			}
			else if(power_p >= 1 && power_p < 3)		
			{
					AIP1820_printf(SEG4,0x00);	
					if(AVR_Handle.in_rms < LOW_POWER)
						AIP1820_printf(SEG5,0x38);
					else if(AVR_Handle.in_rms > HIGH_POWER)
						AIP1820_printf(SEG5,0x76);
					AIP1820_printf(SEG6,0x00); 
					count = 0;
			}
			else if(power_p == 3)
			{
				if(count_down(init_delay) == 0)
				{	power_p = 0;  init_flag = 1;}
			}
		}
		else
		{	
			AIP1820_printf(SEG1,0x00);
			AIP1820_printf(SEG2,0x00);
			AIP1820_printf(SEG3,0x00);
			AIP1820_printf(SEG4,0x00);
			AIP1820_printf(SEG5,0x00);
			AIP1820_printf(SEG6,0x00);
			AIP1820_printf(0xC6,0x01);
		}
	}
	else
	{
		time_count2++;
		time_count3++;
		if(time_count3 ==5 ) 
		{
			warning_twink = !warning_twink;
			time_count3 = 0;
		}
		if(!warning_twink)
		{
			AIP1820_printf(SEG1,0x00);
			AIP1820_printf(SEG2,0x00);
			AIP1820_printf(0xC6,0x05);
			AIP1820_printf(SEG3,0x00); 
			if(time_count2 >= 300)
			{
				AIP1820_printf(SEG4,0x00);
				AIP1820_printf(SEG5,0x00);
				AIP1820_printf(SEG6,0x00); 
			}
		}
		else
		{
			AIP1820_printf(SEG1,SEG1_CODE[AVR_Handle.in_show_rms/100]);
			AIP1820_printf(SEG2,SEG1_CODE[AVR_Handle.in_show_rms/10%10]);
			AIP1820_printf(SEG3,SEG1_CODE[AVR_Handle.in_show_rms%10]); 
			AIP1820_printf(0xC6,0x01);
			
			if(time_count2 >= 300)
			{
				AIP1820_printf(SEG4,0x73);
				AIP1820_printf(SEG5,0x40);						//显示P-H
				AIP1820_printf(SEG6,0x76);
			}			
		}
		if(time_count2 <= 300)
		{
			AIP1820_printf(SEG4,0x73);					
			AIP1820_printf(SEG5,SEG1_CODE[time_count2/100%10]);
			AIP1820_printf(SEG6,SEG1_CODE[time_count2/10%10]);	
			if(time_count2 == 300)
				AVR_Handle.ele_protect++;
		}
		else 
		{
			if(AVR_Handle.ele_protect < 2)					//倒计时30秒关闭输出继电器
			{	
				power_switch(GPIO_PIN_RESET);			
				if(time_count2 > 320)
					time_count2 = 0;
			}
		}
		if(AVR_Handle.ele_protect == 1||AVR_Handle.ele_protect == 2)
		{	
			if(time_count2 == 60)							//六秒后恢复继电器输出
			{
				power_switch(GPIO_PIN_SET);
			}
			if(AVR_Handle.ele_protect == 2)
				AVR_Handle.ele_protect = 3;
		}
		if(AVR_Handle.ele_protect > 2)						//循环超过两次
		{
			power_switch(GPIO_PIN_RESET);
			time_count2 = 310;
			AVR_Handle.ele_protect = 3;					//进入锁定状态，关闭输出继电器，重启才能解锁
		}
	}
}

uint8_t VPTH[12] = {0,87,97,107,121,129,144,155,174,190,212,237};
uint8_t VPLH[11] = {84,93,104,110,121,134,150,161,181,203,227};
/**
  * @brief  电压等级判断任务
  * @param  无
  * @retval 无
  */
void Adjust_Task(void)
{
	static uint8_t level_state = LEVEL6;
//	static uint8_t adjFlag = 0;
	static uint16_t time1 = 0;
	//static uint16_t time2 = 0;
	/***判断电压等级***/
	uint8_t i = 0;
	
	for(i=0;i<11;i++)
	{
		if(AVR_Handle.in_rms > VPTH[i] && AVR_Handle.in_rms <= VPTH[i+1])
		{
			if((AVR_Handle.v_level - i) == 1)							
			{
				if(AVR_Handle.in_rms < VPLH[i])
				{
					AVR_Handle.v_level = i;
					break;
				}
			}
			else											
			{
				AVR_Handle.v_level = i;
				break;
			}
		}
	}
	if(AVR_Handle.in_rms > VPTH[11])	
	{
		AVR_Handle.v_level = LEVEL11;
	}
	
	if(level_state != AVR_Handle.v_level)	//判断电压等级发生改变
	{			
		if(level_state ==10 && AVR_Handle.v_level == 9)	//从10档掉到9档时先关闭继电器3，延时一段时间
		{
			GPIOF->BRR = (uint32_t)RELAY_3_PIN;
			if(++time1 >= RELAY_DELAY)
			{	
				level_state = AVR_Handle.v_level;
				adjustState = 2;
				time1 = 0;
			}
			else
				return;
		}
		level_state = AVR_Handle.v_level;
		adjustState = 2;					//打开调节，在电压过零点切换继电器
		adc_indx = 0;
		ch1ConversionFlag = 0;
		adc_ch_change(ADC_CHANNEL_1);	
	}
} 

/**
  * @brief  按键检测任务
  * @param  无
  * @retval 无
  */
void Key_Task(void)
{	
	static uint16_t state = 0;
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_13) == 0)		//按下保压时间为300秒
	{
		init_delay = 301;
	}
	else
		init_delay = 7;
		
	if(state !=	init_delay)
	{
		state = init_delay;
		count = 0;
	}	
}


/**
  * @brief  初始化任务
  * @param  无
  * @retval 无
  */
void Init_Task(void)
{
	
	uint8_t i = 0;
	uint8_t level = 10;
	uint8_t level_buff[5] = {10,6,4,2,0};

		//等待adc传输完成
	do{
		if(ch1ConversionFlag == 1)				//传输完成开始初始化继电器							
		{	
			AVR_Handle.in_rms = calculate_rms(adValue_buffer,ADC_BUFF_SIZE,GAIN_FACTOR);
			for(i=0;i<11;i++)
			{
				if(AVR_Handle.in_rms > VPTH[i] && AVR_Handle.in_rms <= VPTH[i+1])
				{
					level = i;
					i = 0;
					break;
				}
			}
			if(AVR_Handle.in_rms > VPTH[11])	
			{
				level = LEVEL11;
			}

			for(i = 0;level < level_buff[i];i++)
			{
				AVR_Handle.v_level = level_buff[i];
				adjustState = 2;
				HAL_Delay(200);
			}
			return;
		}
		else	
		{
			HAL_Delay(1000);			//超时后退出初始化程序
			return;
		}
		
	}while(1);
}


/**
  * @brief  计算运算放大器增益系数
  * @param  无
  * @retval 无
  */
float calculate_gain_factor(float rms, float adc_values[], uint8_t num_samples, float vddref) 
{
    float sum_of_squares = 0;
    for (uint8_t i = 0; i < num_samples; i++) 
	{
        sum_of_squares += pow((adc_values[i] * vddref / 4095.0 - 2.5), 2);
    }
    float gain_factor = sqrt(sum_of_squares / (num_samples * rms * rms));
    return gain_factor;
}





