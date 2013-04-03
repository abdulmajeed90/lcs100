#include "LPC11xx.h"
#include <stdio.h>
#include "CS5463.h"
#include "gpio.h"

/*******************************************************************************
������	 �� myDelay
����ԭ�� �� void myDelay(uint32_t)
�������� �� ���뼶��ʱ
������� �� ��
������� �� ��
����ֵ	 �� ��
*******************************************************************************/
void Delay_MS(uint32_t ulTime)
{
    uint32_t i = 0;
    while (ulTime--)
    {
        for (i = 0; i < 5000; i++);
    }
}
void Delay_US(uint32_t ulTime)
{
    uint32_t i = 0;
    while (ulTime--)
    {
        for (i = 0; i < 5; i++);
    }
}



/*******************************************************************************
������	 �� CS5463_Reset
����ԭ�� �� void CS5463_Reset(void)
�������� �� CS5463Ӳ����λ
˵����PDF�ϵĸ�λ˵����
	  1��ʹCS��Ϊ��(��CS��Ϊ��,ʹCSΪ��,��Ϊ��)
		 ���������Ӳ��SPI����CSֻ����SPI�������õ�����(0xFF)���ɡ�
	  2��Ӳ����λ(ʹRESET��Ϊ��,��������10uS)
	  3���򴮿ڷ���ʼ������,�����а���3��(������)ʱ�����ڵ�SYNC1������(0xFF ),
		 ������һ��ʱ�����ڵ�SYNC0������(0xFE)��
������� �� ��
������� �� ��
����ֵ	 �� ��
*******************************************************************************/
void CS5463_Reset(void)
{
    uint8_t i=0;
    CS5463CS_Low();
    CS5463_Reset_H();
    Delay_US(100);
    CS5463_Reset_L();
    Delay_US(500);
    CS5463_Reset_H();
    CS5463CS_High();
    Delay_US(100);
    CS5463CS_Low();
    Delay_US(100);
    //��SPI���߷���������������CS�ϲ����½��ء�
    Delay_MS(1);
    for(i=0; i<5; i++)
    {
        SPI0_SendByte(0xFF);
    }
    SPI0_SendByte(0xFE);
}
/*******************************************************************************
������	 �� CS5463_Init
����ԭ�� �� void CS5463_Init(void)
�������� �� ��ʼ��CS5463,������CS5463��һЩ�����Ĵ���
˵����		CS5463�ϵ�󣬵�һ���Ǹ���λ���嵽RESET�ţ�Ȼ���ڳ�ʼ��CS5463��
��ʼ�����裺
  1���ڴ����Ϸ�0x5e����,�ٷ�0x800000����(����0x800000д��״̬�Ĵ���,��;�����״̬�Ĵ���DRDYλ�ȹ���)��
  2���ڴ����Ϸ�0x40����,�ٷ�0x000001����(����0x000001д�����üĴ���,��;���趨K=1�ȹ���)��
  3���ڴ����Ϸ�0x4a����,�ٷ�0x000fa0����(����0x000fa0д�����ڼ����Ĵ���,��;���趨N=4000)��
  4���ڴ����Ϸ�0x74����,�ٷ�0x000000����(����0x000000д�����μ����Ĵ���,��;�ǲ�����INT�ŵȹ���)��
  5���ڴ����Ϸ�0x64����,�ٷ�0x000001����(����0x000001д������ģʽ�Ĵ���,��;�Ǽ����Զ�����Ƶ�ʲ����ȹ���)��
  6����У׼�õ���DCoff��ACgain��ֵ��E2PROMд����Ӧ�ļĴ�����
  7���ڴ����Ϸ�0xe8����(���Ѳ���/����ģʽ��Ϊ�����������ڣ���
�������Ϳ���ȥ����Ӧ�ļĴ��������õ�����ֵ��
������� �� ��
������� �� ��
����ֵ	 �� ��
*******************************************************************************/
void CS5463_Init(void)
{
    CS5463_Reset();
#if 0
    //д���üĴ������趨K=1�ȹ��ܡ�
    SPI0_SendByte(0x40);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x01);
    Delay_MS(1);
    //д����ģʽ�Ĵ����������Զ�����Ƶ�ʲ����ȹ��ܡ�
    SPI0_SendByte(0x64);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x61);
    Delay_MS(1);
    //=========================
    SPI0_SendByte(0x42);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    Delay_MS(1);
    SPI0_SendByte(0x46);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    Delay_MS(1);
    SPI0_SendByte(0x4C);
    SPI0_SendByte(0x80);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    Delay_MS(1);
    SPI0_SendByte(0x44);
    SPI0_SendByte(0x20);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    Delay_MS(1);
    SPI0_SendByte(0x48);
    SPI0_SendByte(0x10);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    Delay_MS(1);
    SPI0_SendByte(0x60);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    Delay_MS(1);
    SPI0_SendByte(0x62);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    Delay_MS(1);
    SPI0_SendByte(0x5c);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    Delay_MS(1);
    //д״̬�Ĵ���������״̬�Ĵ���DRDYλ�ȹ��ܡ�
    SPI0_SendByte(0x5e);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x01);
    Delay_MS(1);
    //д���μ����Ĵ�����������INT��
    SPI0_SendByte(0x74);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    Delay_MS(1);
    //д���ڼ����Ĵ������趨N=4000.
    SPI0_SendByte(0x4a);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x01);
    SPI0_SendByte(0x90);
    Delay_MS(1);
#endif
    //д״̬�Ĵ���������״̬�Ĵ���DRDYλ�ȹ��ܡ�
    SPI0_SendByte(0x5e);
    SPI0_SendByte(0x80);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    Delay_MS(10);
    //д���üĴ������趨K=1�ȹ��ܡ�
    SPI0_SendByte(0x40);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x01);
    Delay_MS(10);
    //д���ڼ����Ĵ������趨N=4000.
    SPI0_SendByte(0x4a);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x0F);
    SPI0_SendByte(0xA0);
    Delay_MS(10);
    //д���μ����Ĵ�����������INT��
    SPI0_SendByte(0x74);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x00);
    Delay_MS(10);
    //д����ģʽ�Ĵ����������Զ�����Ƶ�ʲ����ȹ��ܡ�
    SPI0_SendByte(0x64);
    SPI0_SendByte(0x80);
    SPI0_SendByte(0x00);
    SPI0_SendByte(0x01);
    Delay_MS(10);
    //У��
    //SPI0_SendByte(0xa0);
    //SPI0_SendByte(0xd6);
    //����ת��
    SPI0_SendByte(0xe8);
    Delay_MS(100);
}
/*******************************************************************************
������	 �� CS5463_ReadData
����ԭ�� �� uint32_t CS5463_ReadData(uint8_t)
�������� �� ��ָ���ļĴ�����ַ��ȡ�����ݡ�
������� �� RegAdd���Ĵ�����ַ��
������� �� ��
����ֵ	 �� ��
*******************************************************************************/
uint32_t CS5463_ReadData(uint8_t RegAdd)
{
    uint8_t data1,data2,data3;
    uint32_t value;
    SPI0_SendByte(RegAdd);//Delay_US(1);
    data1=SPI0_ReceiveByte();//Delay_US(1);
    data2=SPI0_ReceiveByte();//Delay_US(1);
    data3=SPI0_ReceiveByte();//Delay_US(1);
    value=(data1<<16)+(data2<<8)+data3;
    return(value);
}

uint32_t Measuring_220V(void)//������ѹ
{
    uint32_t Temp_220V=0;
//	uint32_t CRC16_value=0;
    uint32_t value=0;
    uint32_t AC220V_value=0;
    uint8_t i=0;
    CS5463_ReadData(0x18);
    Delay_MS(10);
    CS5463_ReadData(0x18);
    Delay_MS(10);
    for(i=0; i<10; i++)
    {
        Temp_220V=CS5463_ReadData(0x18);
        if(Temp_220V<6000000)
        {
            AC220V_value=(Temp_220V*150)/6000000;
        }
        else
        {
            AC220V_value=(Temp_220V*200)/7600000;
        }
        value+=AC220V_value;
        Delay_MS(10);
    }
    value=value/10;
    return(value);
}
uint32_t Measuring_AC(void)//��������
{
    uint32_t Temp_220V=0;
//		uint32_t CRC16_value=0;
    uint32_t AC220V_value=0;
    uint32_t value=0;
    uint8_t i=0;

    CS5463_ReadData(0x16);
    Delay_MS(10);
    CS5463_ReadData(0x16);
    Delay_MS(10);
    for(i=0; i<10; i++)
    {
        Temp_220V=CS5463_ReadData(0x16);

        if(Temp_220V<270000)
        {
            AC220V_value=(Temp_220V*100)/300000;
        }
        else if(Temp_220V<=290000&&Temp_220V>=270000)
        {
            AC220V_value=(Temp_220V*200)/300000;
        }
        else if(Temp_220V<=330000&&Temp_220V>290000)
        {
            AC220V_value=(Temp_220V*300)/330000;
        }
        else if(Temp_220V<=360000&&Temp_220V>330000)
        {
            AC220V_value=(Temp_220V*400)/380000;
        }
        else
        {
            AC220V_value=(Temp_220V*500)/430000;
        }
        value+=AC220V_value;
        Delay_MS(10);
    }
    value=value/10;
    return(value);

}
uint32_t Measuring_Pactive(void) //�����й�����
{
    uint32_t Temp_220V=0;
//	uint32_t CRC16_value=0;
    uint32_t value=0;
    uint32_t AC220V_value=0;
    uint8_t i=0;
    CS5463_ReadData(0x14);
    Delay_MS(10);
    CS5463_ReadData(0x14);
    Delay_MS(10);
    for(i=0; i<10; i++)
    {
        Temp_220V=CS5463_ReadData(0x14);
        Temp_220V=(~Temp_220V&0xFFFFFF)+1;
        if(Temp_220V<6000)
        {
            AC220V_value=(Temp_220V*100)/47000;
        }
        else
        {
            AC220V_value=((Temp_220V-6000)*1000)/60000+30;
        }
        value+=AC220V_value;
        Delay_MS(10);
    }
    value=value/10;
    return(value);
}

uint32_t  Light_Measuring_220AV(void)//��ѯ��״̬
{
    uint32_t Temp_220V=0;
//	uint32_t CRC16_value=0;
    uint32_t value=0;
    uint32_t AC220V_value=0;
    uint8_t i=0;
    CS5463_ReadData(0x14);
    Delay_MS(10);
    CS5463_ReadData(0x14);
    Delay_MS(10);
    for(i=0; i<10; i++)
    {
        Temp_220V=CS5463_ReadData(0x14);
        Temp_220V=(~Temp_220V&0xFFFFFF)+1;
        if(Temp_220V<6000)
        {
            AC220V_value=(Temp_220V*100)/47000;
        }
        else
        {
            AC220V_value=((Temp_220V-6000)*1000)/60000+30;
        }
        value+=AC220V_value;
        Delay_MS(10);
    }
    value=value/10;
    return(value);
}



