#include "aip1820.h"
#include "stdio.h"

uint8_t SEG1_CODE[]=
{		//���������0~9������
        0xFA,        //0
        0x22,        //1
        0xB9,        //2
        0xAB,        //3
        0x63,        //4
        0xCB,        //5
        0xDB,        //6
        0xA2,        //7
        0xFB,        //8
        0xEB,        //9
		0xF3,		 //A
		0x5B,		 //B
		0xD8,		 //C
		0x3B,		 //D
		0xD9,		 //E
		0xD1,		 //F
		0x04 		 //LED
};
uint8_t SEG2_CODE[]={0x80,0x20,0x02,0x08,0x10,0x40,0x01,0xFF};
uint8_t SEG3_CODE[]={0x80,0x01,0x08,0x50,0x22,0x04};


static void sdelay_us(uint32_t us) {
    us *= 24; // ��΢��ת��Ϊʱ��������

    while(us--) {
        __asm("nop"); // ��ָ�ռ��һ��ʱ������
    }
}
//����IIC������ʼ�ź�
static void AIP1820_IIC_start(void)
{

	AIP1820_IIC_SDA_HIGH;   
	sdelay_us(3);
	AIP1820_IIC_SCL_HIGH; 
	sdelay_us(10);   
	AIP1820_IIC_SDA_LOW;	
	sdelay_us(10);
}
 
//����IIC���߽����ź�
static void AIP1820_IIC_stop(void)
{
	AIP1820_IIC_SDA_LOW;      //SCL=0

	sdelay_us(3);
	AIP1820_IIC_SCL_HIGH;
	sdelay_us(10);
	AIP1820_IIC_SDA_HIGH;      //SDA=0
	sdelay_us(10);

}
 
//ͨ��IIC���߷���һ���ֽ�
static void AIP1820_IIC_write_byte(uint8_t dat)
{
	uint8_t i;
	
	for(i=0;i<8;i++)
	{
		sdelay_us(3);
		AIP1820_IIC_SCL_LOW;
		AIP1820_IIC_SDA_WR(dat&0x01);		
		
		sdelay_us(3);
		AIP1820_IIC_SCL_HIGH;
		dat>>=1;
	}
	AIP1820_IIC_SCL_LOW;
	sdelay_us(3);
	AIP1820_IIC_SDA_LOW;
	sdelay_us(3);
	AIP1820_IIC_SCL_HIGH;
	sdelay_us(3);
	AIP1820_IIC_SCL_LOW;
	sdelay_us(3);
	AIP1820_IIC_SDA_HIGH;
}
 
//ͨ��IIC���߽��մӻ���Ӧ��ACK�ź�
//static uint8_t AIP1820_IIC_wait_ack(void)
//{
//	uint8_t ack_signal = 0;
//	
//	AIP1820_IIC_SDA_HIGH;    //SDA=1
//	delay_us(3);
//	AIP1820_IIC_SCL_HIGH;
//	delay_us(3);
//	if(AIP1820_IIC_SDA_RD()) ack_signal = 1;   //�����ȡ������NACK�ź�
//	AIP1820_IIC_SCL_LOW;
//	delay_us(3);
//	return ack_signal;
//}
 
 
//TM1650��ʼ��
void AIP1820_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	__HAL_RCC_GPIOF_CLK_ENABLE();
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;            /* ������� */
	GPIO_InitStruct.Pull = GPIO_NOPULL;                    /* ���������� */
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;          /* GPIO�ٶ� */

		/* GPIOA��ʼ�� */
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct); 
 
    AIP1820_IIC_SDA_HIGH;   								//�ͷ�SDA��
    AIP1820_IIC_SCL_HIGH;   								//�ͷ�SCL��
	



	AIP1820_printf(SEG1,0x00);
	AIP1820_printf(SEG2,0x00);
	AIP1820_printf(SEG3,0x00);
	AIP1820_printf(SEG4,0x00);
	AIP1820_printf(SEG5,0x00);
	AIP1820_printf(SEG6,0x00);
}


void AIP1820_printf(uint8_t addr,uint8_t data)
{
	AIP1820_IIC_start();
	AIP1820_IIC_write_byte(0x44);
	AIP1820_IIC_stop();
	
	AIP1820_IIC_start();																							
	AIP1820_IIC_write_byte(addr);
	AIP1820_IIC_write_byte(data);
	AIP1820_IIC_stop();
	
	AIP1820_IIC_start();
	AIP1820_IIC_write_byte(0x8C);
	AIP1820_IIC_stop();
}


















