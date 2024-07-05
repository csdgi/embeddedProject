#include "gn1640.h"


uint8_t SEG1_CODE[]=
{		//���������0~9������
	0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0xFF,0x6F
};

void delay_us(uint32_t us) {
    us *= 24; // ��΢��ת��Ϊʱ��������

    while(us--) {
        __asm("nop"); // ��ָ�ռ��һ��ʱ������
    }
}
void TM1640_Init(void)
{	
	GPIO_InitTypeDef  GPIO_InitStruct;	
	__HAL_RCC_GPIOF_CLK_ENABLE();
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;            /* ������� */
	GPIO_InitStruct.Pull = GPIO_NOPULL;                  
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;          /* GPIO�ٶ� */
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
��ʼ����
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
��������
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
дһ�ֽ�����  date Ϊ��Ҫд������
***************************************/  
void TM1640WriteByte(u8 date)  
{  
	u8 i;     
	for(i=0;i<8;i++)  
	{  
			TM1640SLK_LOW;  
			delay_us(2);  
			if(date & 0x01)  //�ȵ�λ���λ
			{  
				TM1640DAT_HING;  
				delay_us(3);  
			}  
			else  
			{  
				TM1640DAT_LOW;  
				delay_us(3);  
			} 
			date = date>>1;		//��������һλ						
			TM1640SLK_HING;
			delay_us(3);                
	}				
} 


/*****************************************************
*�������ƣ�void TM1640_Cfg_Display(INT8U param)
*�������ܣ�TM1640���ȵ���
*��ڲ�����INT8U param
*���ڲ�����void
*****************************************************/
void TM1640_Cfg_Display(uint8_t param)
{
	TM1640Start();
	TM1640WriteByte(0x48);
	TM1640WriteByte(param);
	TM1640Stop();
}


/***************************************
��������
Addr1640����ʼ��ֵַ
*a : ��Ҫ���͵�����
ValueLong:��Ҫ���͵����ݳ���
�����ڵ�ַ�Լ�1ģʽ
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






