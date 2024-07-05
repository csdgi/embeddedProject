#include "gn1640.h"


uint8_t SEG1_CODE[]=
{		//共阴数码管0~9字型码
	0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0xFF,0x6F
};

void delay_us(uint32_t us) {
    us *= 24; // 将微秒转换为时钟周期数

    while(us--) {
        __asm("nop"); // 空指令，占用一个时钟周期
    }
}
void TM1640_Init(void)
{	
	GPIO_InitTypeDef  GPIO_InitStruct;	
	__HAL_RCC_GPIOF_CLK_ENABLE();
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
	GPIO_InitStruct.Pull = GPIO_NOPULL;                  
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;          /* GPIO速度 */
	GPIO_InitStruct.Pin = SCLK_GPIO_PIN|DIN_GPIO_PIN;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct); 
	
	TM1640_Cfg_Display(BRIGHTNESS);
	
	AIP1820_printf(0xc0,0x00);
	AIP1820_printf(0xc1,0x00);
	AIP1820_printf(0xc2,0x00);
	AIP1820_printf(0xc3,0x00);
	AIP1820_printf(0xc4,0x00);
	AIP1820_printf(0xc5,0x00);
	AIP1820_printf(0xc6,0x00);
	
}

/****************************************  
起始函数
***************************************/  
void TM1640Start(void)  
{   
        TM1640SLK_HING;
		delay_us(2);
		TM1640DAT_HING;    
        delay_us(2) ;  
        TM1640DAT_LOW;
		delay_us(2);  
} 
/*************************************  
结束函数
***************************************/  
void TM1640Stop(void)  
{       
		TM1640SLK_LOW;  
        delay_us(2);
		TM1640DAT_LOW;  
        delay_us(2); 
        TM1640SLK_HING;  
        delay_us(2);
        TM1640DAT_HING;            
}  
/*************************************  
TM1640WriteByte  
写一字节数据  date 为所要写的数据
***************************************/  
void TM1640WriteByte(u8 date)  
{  
	u8 i;     
	for(i=0;i<8;i++)  
	{  
			TM1640SLK_LOW;  
			delay_us(2);  
			if(date & 0x01)  //先低位后高位
			{  
				TM1640DAT_HING;  
				delay_us(3);  
			}  
			else  
			{  
				TM1640DAT_LOW;  
				delay_us(3);  
			} 
			date = date>>1;		//数据右移一位						
			TM1640SLK_HING;
			delay_us(3);                
	}				
} 


/*****************************************************
*函数名称：void TM1640_Cfg_Display(INT8U param)
*函数功能：TM1640亮度调节
*入口参数：INT8U param
*出口参数：void
*****************************************************/
void TM1640_Cfg_Display(uint8_t param)
{
	TM1640Start();
	TM1640WriteByte(0x48);
	TM1640WriteByte(param);
	TM1640Stop();
}


/***************************************
发送数组
Addr1640：起始地址值
*a : 所要发送的数组
ValueLong:想要发送的数据长度
适用于地址自加1模式
**************************************/
void TM1640_SendData(u8 Addr1640,u8 *a,u8 ValueLong)  
{  
        u8 i;       
        TM1640Start();  
        TM1640WriteByte(Addr1640);   
          
        for(i=0;i<ValueLong;i++)  
        {  
              TM1640WriteByte(a[i]);   
        }        
        TM1640Stop();   
}




void AIP1820_printf(uint8_t addr,uint8_t data)
{	 
	TM1640Start();
	TM1640WriteByte(0x44);
	TM1640Stop();
	TM1640Start();
	TM1640WriteByte(addr);
	TM1640WriteByte(data);
	TM1640Stop();
	TM1640Start();
	TM1640WriteByte(0x8C);
	TM1640Stop();

}






