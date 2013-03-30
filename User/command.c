/************************�����Ǻӹ��********************************/

#include "LPC11xx.h"
#include "uart.h"
#include "timer32.h"
#include "24c02.h"
#include <stdio.h>
#include "string.h"
#include "Application.h"
#include "CRC16_1.h"
#include "wdt.h"
#include "math.h"
#include "gpio.h"
uint8_t Data_Buf[Data_Len];
uint8_t field[10];
uint8_t value[10]= {0X64,0X64,0X64,0X64,0X64,0X64,0X64,0X64,0X64,0X64};
uint32_t pwmtimer=10;
uint8_t zigbeeflag=0;
extern volatile  uint32_t timer32_0_counter;
void App_Command(void)//��������ֽ�
{
    uint32_t value=0;
    uint8_t Command;
    uint32_t CRC16_value=0;
    uint8_t Light_Status=0x30;
    uint8_t temp_buf[20];
    Command=Data_Buf[7];
    Command=Command&0x7F;
    if(Command==0x07)//��������
    {

        Tune_Light();
        Frame_OK(0x07);
    }
    else if(Command==0x33)//���ѹ
    {
        value=Measuring_220V();
        memset(temp_buf,0,20);
        temp_buf[0]=0x7e;
        temp_buf[1]=0x02;
        memcpy(&temp_buf[2],Terminal_ID,4);
        temp_buf[6]=group_number;
        temp_buf[7]=0x00;      //Ӧ����
        temp_buf[8]=0xF3;     //������
        temp_buf[9]=(0xc0+((value&0xFF00)>>8));   //��λΪ1V      ����
        temp_buf[10]=value&0x00FF;
        CRC16_value=CRC16_1(temp_buf,9);
        temp_buf[11]=((CRC16_value&0xff00)>>8);
        temp_buf[12]=CRC16_value&0x00ff;
        temp_buf[13]=0x0d;
        temp_buf[14]=0x0a;
        UARTSend( (uint8_t *)temp_buf, 15 );
    }
    else if(Command==0x34)//�����
    {

        value=Measuring_AC();
        memset(temp_buf,0,20);
        temp_buf[0]=0x7e;
        temp_buf[1]=0x02;
        memcpy(&temp_buf[2],Terminal_ID,4);
        temp_buf[6]=group_number;
        temp_buf[7]=0x00;
        temp_buf[8]=0xF4;
        temp_buf[9]=(value&0xFF00)>>8;   //��λΪ1MA
        temp_buf[10]=value&0x00FF;
        CRC16_value=CRC16_1(temp_buf,9);
        temp_buf[11]=((CRC16_value&0xff00)>>8);
        temp_buf[12]=CRC16_value&0x00ff;
        temp_buf[13]=0x0d;
        temp_buf[14]=0x0a;
        UARTSend( (uint8_t *)temp_buf, 15);
    }
    else if(Command==0x35)//���й�����
    {
        value= Measuring_Pactive();
        memset(temp_buf,0,20);
        temp_buf[0]=0x7e;
        temp_buf[1]=0x02;
        memcpy(&temp_buf[2],Terminal_ID,4);
        temp_buf[6]=group_number;
        temp_buf[7]=0x00;              //Ӧ����
        temp_buf[8]=0xF5;
        temp_buf[9]=(0x80+((value&0xFF00)>>8));//��λΪ100MW
        temp_buf[10]=value&0x00FF;
        CRC16_value=CRC16_1(temp_buf,9);
        temp_buf[11]=((CRC16_value&0xff00)>>8);
        temp_buf[12]=CRC16_value&0x00ff;
        temp_buf[13]=0x0d;
        temp_buf[14]=0x0a;
        UARTSend( (uint8_t *)temp_buf, 15 );

    }
    else if(Command==0x01)//zigbeeģ������
    {
        //         Frame_OK(0x01);
        ZigbeeSetup();
        Frame_OK(0x01);

    }
    else if(Command==0x02)//����
    {

        if(Command==0x02)
        {
            Frame_OK(0x02);
        }
// 		Delay1_MS(100000);
        //	Falg_WDT=TRUE;
    }
    else if(Command==0x12)//�޸��豸ID
    {
        Modify_ID();
        Frame_OK(0x12);
    }
    else if(Command==0x31)//��ѯID
    {
        Inquiry_ID();
    }
    else if(Command==0x0B)//����Ĭ�ϵ���ֵ
    {

        SetupPwm();
        Frame_OK(0x0B);
    }
    else if(Command==0x0C)//�������
    {

        SetupGroupNumber();
        Frame_OK(0x0C);
    }
    else if(Command==0x0D)//����ǰLED������ɾ��
    {

        deleteSetupGroupNumber();
        Frame_OK(0x0D);
    }
    else if(Command==0x39)//��ѯ���
    {

        InquiryGroupNumber();
    }
    else if(Command==0x36)//��ѯ��ǰ����ֵ
    {

        InquiryLightValue();
    }
    else if(Command==0x37)//��ѯĬ�ϵ���ֵ
    {

        InquiryAcquiesceLightValue();
    }
    else if(Command==0x3C)//��ѯ�̼��汾��
    {

        Inquiry_Version();
    }
    else if(Command==0x3B)//��ѯ��״̬
    {

        //uint32_t Light_Measuring_220AV();
        if(value<20)//�ն�״̬
        {
            Light_Status=0x31;
        }
        else if(value<10)
        {
            Light_Status=0x32;
        }
        else
        {
            Light_Status=0x30;
        }

    }
    else if(Command==0x0A)//���õ���ʱ��
    {

        Setup_lighttime();
        Frame_OK(0x0A);
    }
    else  if(Command==0x08)//��˸
    {

        flicker();
        Frame_OK(0x08);
    }
    else  if(Command==0x0E)//����ĳ�����ĵ���ֵ
    {

        Setup_fieldlight();
        Frame_OK(0x0E);
    }
    else   if(Command==0x0F)//��ĳ����ֵ�Ƴ�
    {

        remove_fieldlight();
        Frame_OK(0x0F);
    }
    else   if(Command==0x11)//����ĳ�ֳ���
    {

        in_field();
        Frame_OK(0x11);
    }
    else   if(Command==0x3A)//��ѯ��ĳ�����ĵ���ֵ
    {

        Inquiry_fieldlightvalue();
    }
    else   if(Command==0x32)//��ѯ��������
    {

        Inquiry_alldata();
    }
    else   if(Command==0x38)//��ѯ����ʱ��
    {

        Inquiry_lighttime();
    }
    else   if(Command==0x06)   //��д������Ϣ��EEPROM
    {

        configuration();
    }
    else   if(Command==0x10)//����ͨѶ������
    {

        Baud_Rate ();
        Frame_OK(0x10);
    }
    else   if(Command==0x09)//������
    {

        heartbeat ();
    }
    else   if(Command==0x03)//�̼�������������
    {

        upgrade();
    }
    else   if(Command==0x04)//�̼��������ݰ�
    {

        upgradedata();
    }
    else if(Command==0x05)//�̼�����У���
    {

        upgradecrc();
    }
}

/*************************���⺯��**********************************************/
void Tune_Light(void)
{

    uint32_t timer=2,pwmtimer1=0,pwmtimer0=0,pwmtimer2=0;
    uint32_t lightvalue=0;

    Duty_Time_Temp=Data_Buf[8];
    lightvalue=Duty_Time;
    if(Duty_Time_Temp!=lightvalue)
    {
        timer = (uint32_t)pow(2,pwmtimer);
        pwmtimer2=(timer*20);
        if(Duty_Time_Temp>lightvalue)
        {
            pwmtimer1 = pwmtimer2/(Duty_Time_Temp-lightvalue);
            if(lightvalue<5)
            {
                lightvalue=5;
                Duty_Time=lightvalue;
                init_timer32PWM(1,TIME_INTERVAL,0x01);
            }
            while((Duty_Time_Temp>lightvalue)&&(lightvalue>=5))
            {

                Delay1_MS(pwmtimer1);
                lightvalue++;
                Duty_Time=lightvalue;
                init_timer32PWM(1,TIME_INTERVAL,0x01);
            }
        }


        if(Duty_Time_Temp<lightvalue)
        {
            pwmtimer0=pwmtimer2/(lightvalue-Duty_Time_Temp);
            while(Duty_Time_Temp<lightvalue)
            {
                if(lightvalue>5)
                {
                    Delay1_MS(pwmtimer0);
                    lightvalue--;
                    Duty_Time=lightvalue;
                    init_timer32PWM(1,TIME_INTERVAL,0x01);
                }
                else
                {
                    lightvalue=0;
                    Duty_Time=lightvalue;
                    init_timer32PWM(1,TIME_INTERVAL,0x01);
                }
                Delay1_MS(1);
            }
        }
    }
    else
    {
        lightvalue=Duty_Time_Temp;
        Duty_Time=lightvalue;
        init_timer32PWM(1,TIME_INTERVAL,0x01);
    }

}
void SetupPwm(void)//����Ĭ�ϵ���ֵ
{
    Duty_Time=Data_Buf[8];

    memset(Data_Buf,0,Data_Len);
    Data_Buf[0]=Duty_Time;
    m24xx_write(EEPROM_24XX02,15,0,Data_Buf,1);	  //���ַ0x0f����ʼд��1������
    memset(Data_Buf,0,Data_Len);
}

void Setup_fieldlight(void)  //����ĳ�����ĵ���ֵ
{
    uint8_t num=0;

    num=Data_Buf[8];
    field[num]=num;         //������
    value[num]=Data_Buf[9];   //����ֵ
    m24xx_write(EEPROM_24XX02,50+num,0,&field[num],1);
    m24xx_write(EEPROM_24XX02,60+num,0,&value[num],1);

}
void remove_fieldlight(void)  //��ĳ����ֵ�Ƴ�
{
    uint8_t num=0;
    num=Data_Buf[8];
    field[num]=0x00;
    value[num]=0x64;
    m24xx_write(EEPROM_24XX02,50+num,0,&field[num],1);
    m24xx_write(EEPROM_24XX02,60+num,0,&value[num],1);

}
void in_field(void)  //����ĳ�ֳ���
{
    uint8_t num=0;
    num=Data_Buf[8];     //������
    field[num]=num;
    value[num]=value[num];
    Duty_Time=value[num];
    init_timer32PWM(1,TIME_INTERVAL,0x01);
    memset(Data_Buf,0,Data_Len);
    Data_Buf[0]=Duty_Time;
    m24xx_write(EEPROM_24XX02,15,0,Data_Buf,1);	  //���ַ0x0f����ʼд��1������
    memset(Data_Buf,0,Data_Len);
    m24xx_write(EEPROM_24XX02,50+num,0,&field[num],1);

}
void Inquiry_fieldlightvalue(void)  //��ѯ��ĳ�����ĵ���ֵ
{
    uint8_t num=0;
    uint8_t temp_buf[20];
    uint32_t CRC16_value=0;
    num=Data_Buf[8];
    value[num]=value[num];
    temp_buf[0]=0x7e;
    temp_buf[1]=0x02;
    memcpy(&temp_buf[2],Terminal_ID,4);
    temp_buf[6]=group_number;
    temp_buf[7]=0x00;
    temp_buf[8]=0xFA;    //�ظ�������0XC0+0X3A
    temp_buf[9]=num;
    temp_buf[10]=value[num];

    CRC16_value=CRC16_1(temp_buf,11);
    temp_buf[11]=((CRC16_value&0xff00)>>8);
    temp_buf[12]=CRC16_value&0x00ff;
    temp_buf[13]=0x0d;
    temp_buf[14]=0x0a;
    UARTSend( (uint8_t *)temp_buf, 15 );
}
void flicker(void)  //��˸
{
    uint32_t timer=100;
    uint8_t  a=0,num;
    a=Data_Buf[1];         //��Ϣ����
    if(a>0x00)
    {
        for(num=0; num<2; num++)
        {
            timer+=Data_Buf[8+num];
        }
    }

    while(1)
    {
        Duty_Time=100;
        init_timer32PWM(1,TIME_INTERVAL,0x01);
        Delay1_MS(timer);
        Duty_Time=00;
        init_timer32PWM(1,TIME_INTERVAL,0x01);
        Delay1_MS(timer);
    }
}

void SetupGroupNumber(void)//�������
{
    group_number=Data_Buf[8];
    memset(Data_Buf,0,Data_Len);
    Data_Buf[0]=group_number;
    m24xx_write(EEPROM_24XX02,17,0,Data_Buf,1); //���ַ0x11����ʼд��1������
    memset(Data_Buf,0,Data_Len);
}

void InquiryGroupNumber(void)//��ѯ���
{
    uint8_t temp_buf[20];

    uint32_t CRC16_value=0;
    memset(temp_buf,0,20);
    temp_buf[0]=0x7e;
    temp_buf[1]=0x01;
    memcpy(&temp_buf[2],Terminal_ID,4);
    temp_buf[6]=group_number;
    temp_buf[7]=0x00;
    temp_buf[8]=0xF9;
    temp_buf[9]=group_number;

    CRC16_value=CRC16_1(temp_buf,11);
    temp_buf[10]=((CRC16_value&0xff00)>>8);
    temp_buf[11]=CRC16_value&0x00ff;
    temp_buf[12]=0x0d;
    temp_buf[13]=0x0a;
    UARTSend( (uint8_t *)temp_buf, 14 );
}
void deleteSetupGroupNumber(void)//����ǰLED������ɾ��
{
    group_number = 0XFF;
    m24xx_write(EEPROM_24XX02,17,0,&group_number,1); //���ַ0x11����ʼд��1������
}

void Inquiry_ID(void)//��ѯID��
{
    uint8_t temp_buf[20];

    uint32_t CRC16_value=0;
    memset(temp_buf,0,20);
    temp_buf[0]=0x7e;
    temp_buf[1]=0x04;
    memcpy(&temp_buf[2],Terminal_ID,4);
    temp_buf[6]=group_number;
    temp_buf[7]=0x00;
    temp_buf[8]=0xF1;
    memcpy(&temp_buf[9],Terminal_ID,4);
    CRC16_value=CRC16_1(temp_buf,12);
    temp_buf[13]=((CRC16_value&0xff00)>>8);
    temp_buf[14]=CRC16_value&0x00ff;
    temp_buf[15]=0x0d;
    temp_buf[16]=0x0a;
    UARTSend( (uint8_t *)temp_buf, 17 );
}
void Modify_ID(void)    //�޸��豸ID��
{
    uint8_t  DataL = 0;
    uint8_t  num = 0;
    DataL = Data_Buf[1];      //���ݳ���
    for(num = 0 ; num < DataL ; num++)
        Terminal_ID[num] = Data_Buf[8+num];
    m24xx_write(EEPROM_24XX02, 10, 0, Terminal_ID, 4);
}
void Setup_lighttime(void)//���õ���ʱ��
{
    pwmtimer=Data_Buf[8];
    Data_Buf[0]=pwmtimer;
    m24xx_write(EEPROM_24XX02,70,0,Data_Buf,1); //���ַ0x11����ʼд��1������
    memset(Data_Buf,0,Data_Len);

}
void Inquiry_Version(void)//��ѯ�̼��汾��
{
    uint8_t temp_buf[20];

    uint32_t CRC16_value=0;
    memset(temp_buf,0,20);
    temp_buf[0]=0x7e;
    temp_buf[1]=0x03;
    memcpy(&temp_buf[2],Terminal_ID,4);
    temp_buf[6]=group_number;
    temp_buf[7]=0x00;
    temp_buf[8]=0xFC;
    memcpy(&temp_buf[9],Version,3);
    CRC16_value=CRC16_1(temp_buf,11);
    temp_buf[12]=((CRC16_value&0xff00)>>8);
    temp_buf[13]=CRC16_value&0x00ff;
    temp_buf[14]=0x0d;
    temp_buf[15]=0x0a;
    UARTSend( (uint8_t *)temp_buf, 16 );
}



void Inquiry_alldata(void)   //��ѯ��������
{
    uint8_t temp_buf[25];
    uint32_t ALLdata[11];
    uint8_t  data[1]= {0};
    uint32_t CRC16_value=0;
    ALLdata[0] = Measuring_220V();
    ALLdata[1] = Measuring_AC();
    ALLdata[2] = Measuring_Pactive();
    ALLdata[3] = Duty_Time;
    m24xx_read(EEPROM_24XX02,15,0,data,1);	//Ĭ�ϵ���ֵ
    ALLdata[4] = *data;
    ALLdata[5] = pwmtimer;
    ALLdata[6] = group_number;
    ALLdata[7] = Terminal_ID[0];
    ALLdata[8] = Terminal_ID[1];
    ALLdata[9] = Terminal_ID[2];
    ALLdata[10] = Terminal_ID[3];

    temp_buf[0]=0x7e;
    temp_buf[1]=0x0b;
    memcpy(&temp_buf[2],Terminal_ID,4);
    temp_buf[6]=group_number;
    temp_buf[7]=0x00;
    temp_buf[8]=0xF2;
    memcpy(&temp_buf[9],ALLdata,11);
    CRC16_value=CRC16_1(temp_buf,11);
    temp_buf[20]=((CRC16_value&0xff00)>>8);
    temp_buf[21]=CRC16_value&0x00ff;
    temp_buf[22]=0x0d;
    temp_buf[23]=0x0a;
    UARTSend( (uint8_t *)temp_buf, 24 );


}


void Inquiry_lighttime(void)  //��ѯ����ʱ��
{
    uint8_t temp_buf[20];

    uint32_t CRC16_value=0;
    memset(temp_buf,0,20);
    temp_buf[0]=0x7e;
    temp_buf[1]=0x01;
    memcpy(&temp_buf[2],Terminal_ID,4);
    temp_buf[6]=group_number;
    temp_buf[7]=0x00;
    temp_buf[8]=0xF8;
    temp_buf[9]=pwmtimer;
    CRC16_value=CRC16_1(temp_buf,11);
    temp_buf[10]=((CRC16_value&0xff00)>>8);
    temp_buf[11]=CRC16_value&0x00ff;
    temp_buf[12]=0x0d;
    temp_buf[13]=0x0a;
    UARTSend( (uint8_t *)temp_buf, 14 );
}
void InquiryLightValue(void)//��ѯ��ǰ����ֵ
{
    uint8_t temp_buf[20];

    uint32_t CRC16_value=0;
    memset(temp_buf,0,20);
    temp_buf[0]=0x7e;
    temp_buf[1]=0x01;
    memcpy(&temp_buf[2],Terminal_ID,4);
    temp_buf[6]=group_number;
    temp_buf[7]=0x00;
    temp_buf[8]=0xF6;
    temp_buf[9]=Duty_Time;

    CRC16_value=CRC16_1(temp_buf,11);
    temp_buf[10]=((CRC16_value&0xff00)>>8);
    temp_buf[11]=CRC16_value&0x00ff;
    temp_buf[12]=0x0d;
    temp_buf[13]=0x0a;
    UARTSend( (uint8_t *)temp_buf, 14 );
}

void InquiryAcquiesceLightValue(void)   //��ѯĬ�ϵ���ֵ
{

    uint8_t temp_buf[20];

    uint32_t CRC16_value=0;
    memset(temp_buf,0,20);
    temp_buf[0]=0x7e;
    temp_buf[1]=0x01;
    memcpy(&temp_buf[2],Terminal_ID,4);
    temp_buf[6]=group_number;
    temp_buf[7]=0x00;
    temp_buf[8]=0xF7;
    //     temp_buf[9]=Duty_Time;
    m24xx_read(EEPROM_24XX02,15,0,&temp_buf[9],1);
    CRC16_value=CRC16_1(temp_buf,11);
    temp_buf[10]=((CRC16_value&0xff00)>>8);
    temp_buf[11]=CRC16_value&0x00ff;
    temp_buf[12]=0x0d;
    temp_buf[13]=0x0a;
    UARTSend( (uint8_t *)temp_buf, 14 );
}


/*void ZigbeeSetup(void)           //ZIGBEE����
{

  uint8_t temp_buf[20]={0};
   Frame_OK(0x01);

   UARTInit(38400);

   GPIOSetValue(1,8,0);

   Delay1_MS(3000);
  GPIOSetValue(1,8,1);

   temp_buf[0]=0x23;
   temp_buf[1]=0xa0;
   UARTSend(temp_buf,2);
   Delay1_MS(50);
   temp_buf[0]=0x23;
   temp_buf[1]=0xfe;
   memcpy(&temp_buf[2],&Data_Buf[8],14);

   UARTSend(temp_buf,16);
   Delay1_MS(50);
   temp_buf[0]=0x23;
   temp_buf[1]=0x23;
   UARTSend(temp_buf,2);
 //  GPIOSetValue(1,8,1);
   Delay1_MS(50);
   UARTInit(19200);
   Delay1_MS(100);

} */
void ZigbeeSetup(void)           // ��ZIGBEE������Ϣ
{

    uint8_t num=0;
    uint8_t temp_buf[20]= {0};
    memset(temp_buf,0,20);
    memset(Data_Buf,0,Data_Len);
    UARTInit(38400);
    NVIC_DisableIRQ(UART_IRQn);
    Delay1_MS(20);
    GPIOSetValue(1,8,0);
    Delay1_MS(3000);
    GPIOSetValue(1,8,1);
    while ((LPC_UART->LSR & 0x01) == 0x01)
    {
        num = LPC_UART->RBR;

    }
    num=0;
    Data_Buf[0]=0x23;
    Data_Buf[1]=0xa0;
    UARTSend((uint8_t *)Data_Buf,2);

    while ((LPC_UART->LSR & 0x01) == 0x01)
    {


        Data_Buf[num] = LPC_UART->RBR;
        num++;

    }
    num=0;

    memcpy(temp_buf,Data_Buf,15);
    Data_Buf[0]=0x23;
    Data_Buf[1]=0x23;
    UARTSend((uint8_t *)Data_Buf,2);
//  Delay1_MS(200);
// UARTInit(19200);
// Delay1_MS(200);


    UARTSend( (uint8_t *)temp_buf, 15 );
    Delay1_MS(20);


}
void configuration(void)        //д������Ϣ
{
    uint8_t buf[50];
    memset(buf,0,100);

    m24xx_write(EEPROM_24XX02, 7, 0, Version, 3);
    m24xx_write(EEPROM_24XX02, 10, 0, Terminal_ID, 4);

    buf[0]=Duty_Time;
    m24xx_write(EEPROM_24XX02,15,0,buf,1);

// buf[10]=Brate;
//  m24xx_write(EEPROM_24XX02,21,0,&buf[10],1);
// memset(buf,0,100);


    memcpy(buf,field,10);
    memcpy(&buf[11],value,10);
    buf[20]=pwmtimer;
    m24xx_write(EEPROM_24XX02, 50, 0, buf, 21);
    memset(buf,0,100);
    Frame_OK(0x06);
}
void  Baud_Rate(void)      //����ͨѶ������
{
    uint8_t b_rate;
    uint32_t rate;
    uint8_t temp[10];
    b_rate=Data_Buf[8];
    switch(b_rate)
    {
    case 0:
        rate=2400;
        break;
    case 1:
        rate=4800;
        break;
    case 2:
        rate=9600;
        break;
    case 3:
        rate=19200;
        break;
    case 4:
        rate=38400;
        break;
    case 5:
        rate=43000;
        break;
    case 6:
        rate=56000;
        break;
    case 7:
        rate=57600;
        break;
    default :
        rate=19200;
        break;
    }
    UARTInit(rate);

    temp[0]=b_rate;
    m24xx_write(EEPROM_24XX02, 21, 0, temp, 1);

}


void Delay1_MS(uint32_t ulTime)
{
    uint32_t i = 0;
    while (ulTime--)
    {
        for (i = 0; i < 5000; i++);
    }
}
void Delay1_US(uint32_t ulTime)
{
    uint32_t i = 0;
    while (ulTime--)
    {
        for (i = 0; i < 5; i++);
    }
}
void Frame_OK(uint8_t command) //�ظ�Э��
{
    uint8_t temp_buf[20];

    uint32_t CRC16_value=0;
    memset(temp_buf,0,20);
    temp_buf[0]=0x7e;                         //֡ͷ
    temp_buf[1]=00;                           //��Ϣ����
    memcpy(&temp_buf[2],Terminal_ID,4);        //��ַ
    temp_buf[6]=group_number;                 //���
    temp_buf[7]=0x00;                          //Ӧ����
    temp_buf[8]=(command+0XC0);                       //������
    CRC16_value=CRC16_1(temp_buf,8);
    temp_buf[9]=((CRC16_value&0xff00)>>8);    //У����
    temp_buf[10]=CRC16_value&0x00ff;
    temp_buf[11]=0x0d;
    temp_buf[12]=0x0a;                        //֡β
    UARTSend( (uint8_t *)temp_buf, 13 );
    Delay1_MS(10);
}
void heartbeat(void)    //������
{
    ;
}
void upgrade(void) // �̼�������������
{
    ;
}
void upgradedata(void)  //�̼��������ݰ�
{
    ;
}
void upgradecrc(void)   //�̼�����У���
{
    ;
}
