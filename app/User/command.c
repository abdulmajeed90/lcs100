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
#include "LedProto.h"
#include "param.h"

#define MAX_RESP_BUFF_SIZE  128


uint8_t Data_Buf[Data_Len];
//uint8_t field[10];
//uint8_t value[10]= {0X64,0X64,0X64,0X64,0X64,0X64,0X64,0X64,0X64,0X64};

//uint8_t zigbeeflag=0;
uint8_t LedVesion = 100;  //1.00�汾 0.01 - 2.53
uint8_t Command;
uint8_t Mode;

static uint8_t respBuf[MAX_RESP_BUFF_SIZE];

static toShort(uint8_t* buf)
{
	return (buf[0]<<8) + buf[1];
}
extern volatile  uint32_t timer32_0_counter;

//�ظ�Э��
static void ResponseMsg(uint8_t command,uint8_t ack,uint8_t* context, uint8_t len);
static void RespNoPara(uint8_t command,uint8_t ack);
static void RespCharPara(uint8_t command,uint8_t ack,uint8_t value);


/**
 * Convert an u32_t from host- to network byte order.
 *
 * @param n u32_t in host byte order
 * @return n in network byte order
 */
unsigned int
_htonl(unsigned int n)
{
  return ((n & 0xff) << 24) |
    ((n & 0xff00) << 8) |
    ((n & 0xff0000UL) >> 8) |
    ((n & 0xff000000UL) >> 24);
}

/**
 * Convert an u32_t from network- to host byte order.
 *
 * @param n u32_t in network byte order
 * @return n in host byte order
 */
unsigned int
_ntohl(unsigned int n)
{
  return _htonl(n);
}

/**
 * Convert an u16_t from host- to network byte order.
 *
 * @param n u16_t in host byte order
 * @return n in network byte order
 */
unsigned short
_htons(unsigned short n)
{
  return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}

/**
 * Convert an u16_t from network- to host byte order.
 *
 * @param n u16_t in network byte order
 * @return n in host byte order
 */
unsigned short
_ntohs(unsigned short n)
{
  return _htons(n);
}


static void resetCtrl(uint16_t afterMs)
{
	RespNoPara(CMD_RESET,ERR_OK);
	Delay1_MS(afterMs);
	NVIC_SystemReset();
}

//FIXME ������Ҫ��˸��ã���һֱ����������ֹͣ������ָ��ʱ����ߴ���
static void setTwinkle(uint16_t periodMs)
{
	
	uint16_t cnt = 10;
	RespNoPara(CMD_TWINKLE,ERR_OK);

	
    while(cnt--)
    {
        Duty_Time=100;
        init_timer32PWM(1,TIME_INTERVAL,0x01);
        Delay1_MS(periodMs);
        Duty_Time=00;
        init_timer32PWM(1,TIME_INTERVAL,0x01);
        Delay1_MS(periodMs);
    }

}

void SetupAdjustTime(uint8_t waitS)//���õ���ʱ��
{
    adj_timeS=waitS;

	RespNoPara(CMD_SET_ADJ_TIME,ERR_OK);

	paramSetU8(PARAM_ADJ_TIME,waitS);


	
}

void SetupDefaultBrightness(uint8_t brightness)//����Ĭ�ϵ���ֵ
{
    default_brightness=brightness;

	RespNoPara(CMD_SET_DEFAULT_BRIGHTNESS,ERR_OK);

	paramSetU8(PARAM_DEF_BRIGHTNESS,brightness);

}
void SetupGroupNumber(uint8_t group)//�������
{
    group_number=group;

	
	RespNoPara(CMD_SET_GROUP,ERR_OK);
	
	paramSetU8(PARAM_GROUP,group_number);
}
static void queryVoltage()
{
	
	uint16_t voltage =Measuring_220V();
	voltage = _htons(voltage);

	ResponseMsg(CMD_QUERY_VOLTAGE,ERR_OK,(uint8_t*)&voltage,sizeof(uint16_t));
	
}
static void queryCurrent()
{
	
	
	uint16_t currentMA =Measuring_AC(); //��MAΪ��λ
	currentMA = _htons(currentMA);

	ResponseMsg(CMD_QUERY_VOLTAGE,ERR_OK,(uint8_t*)&currentMA,sizeof(uint16_t));
	
}
static void queryPactive()
{

	uint16_t value =Measuring_Pactive(); //��λΪ100MW
	value = _htons(value);
	
	ResponseMsg(CMD_QUERY_VOLTAGE,ERR_OK,(uint8_t*)&value,sizeof(uint16_t));
	
}
void Modify_ID(unsigned char* pId)    //�޸��豸ID��
{

   	memcpy(Terminal_ID,pId,4);
	
	paramSetBuff(PARAM_ID,Terminal_ID,4);
}
void InquiryGroupNumber(void)//��ѯ���
{
    RespCharPara(CMD_QUERY_GROUP,ERR_OK,group_number);
}
void InquiryLightValue(void)//��ѯ��ǰ����ֵ
{
    RespCharPara(CMD_QUERY_BRIGHTNESS,ERR_OK,Duty_Time);
}
void InquiryAcquiesceLightValue(void)   //��ѯĬ�ϵ���ֵ
{
	RespCharPara(CMD_QUERY_DEFAULT_BRIGHTNESS,ERR_OK,default_brightness);
}
void Inquiry_Version(void)//��ѯ�̼��汾��
{
	RespCharPara(CMD_QUERY_DEFAULT_BRIGHTNESS,ERR_OK,LedVesion);
}


void Inquiry_lighttime(void)  //��ѯ����ʱ��
{
	RespCharPara(CMD_QUERY_ADJ_TIME,ERR_OK,adj_timeS);
}

void Inquiry_ZigbeeCfg()
{

}

/*************************���⺯��**********************************************/
static void adjustBrightness(uint8_t brightness)
{

    uint32_t timer=2,pwmtimer1=0,pwmtimer0=0,pwmtimer2=0;
    uint32_t lightvalue=0;

    Duty_Time_Temp= brightness;
	
    lightvalue=Duty_Time;
    if(Duty_Time_Temp!=lightvalue)
    {
        timer = (uint32_t)pow(2,adj_timeS);
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

	
	RespNoPara(CMD_ADJUST_BRIGHTNESS,ERR_OK);

}

void Inquiry_alldata(void)   //��ѯ��������
{

    uint8_t ALLdata[32];
	uint32_t temp = Measuring_220V();
	
    ALLdata[0] = temp>>24;
    ALLdata[1] = temp>>16;
    ALLdata[2] = temp>>8;
    ALLdata[3] = temp>>0;


    temp = Measuring_AC();
	
    ALLdata[4] = temp>>24;
    ALLdata[5] = temp>>16;
    ALLdata[6] = temp>>8;
    ALLdata[7] = temp>>0;


	temp = Measuring_Pactive();
	
    ALLdata[8]  = temp>>24;
    ALLdata[9]  = temp>>16;
    ALLdata[10] = temp>>8;
    ALLdata[11] = temp>>0;

	
    ALLdata[12] = brightness;
    ALLdata[13] = default_brightness;
    ALLdata[14] = adj_timeS;
    ALLdata[15] = group_number;
    ALLdata[16] = Terminal_ID[0];
    ALLdata[17] = Terminal_ID[1];
    ALLdata[18] = Terminal_ID[2];
    ALLdata[19] = Terminal_ID[3];

	ResponseMsg(CMD_QUERY_ALL,ERR_OK,ALLdata,20);

}

void Write2EEPROM(unsigned char* buff, int len)
{

}
void App_Command()//��������ֽ�
{
	
    Command=Data_Buf[7];
    Mode=Command&0x3F;
		
	switch(Command)
	{
		case CMD_RESET: //��λ
			resetCtrl(toShort(Data_Buf+8));
			break;
		case CMD_ADJUST_BRIGHTNESS: //��������
			adjustBrightness(Data_Buf[8]);
			break;
		case CMD_TWINKLE: //��˸
			setTwinkle(toShort(Data_Buf+8));
			break;
		case CMD_SET_ADJ_TIME: //���õ���ʱ��
			SetupAdjustTime(Data_Buf[8]);
			break;
		case CMD_SET_DEFAULT_BRIGHTNESS:
			SetupDefaultBrightness(Data_Buf[8]);
			break;
		case CMD_SET_GROUP:
			SetupGroupNumber(Data_Buf[8]);
			break;
		case CMD_MODIFY_DEVID:
			Modify_ID(Data_Buf+8);
			break;
		case CMD_QUERY_VOLTAGE:
			queryVoltage();
			break;
		case CMD_QUERY_CURRENT:
			queryCurrent();
			break;
		case CMD_QUERY_KW:
			queryPactive();
			break;
		case CMD_QUERY_BRIGHTNESS:
			InquiryLightValue();
			break;
		case CMD_QUERY_DEFAULT_BRIGHTNESS:
			InquiryAcquiesceLightValue();
			break;
		case CMD_QUERY_GROUP:
			InquiryGroupNumber();
			break;
		case CMD_QUERY_ALL:
			Inquiry_alldata();
			break;
		case CMD_QUERY_ADJ_TIME:
			Inquiry_lighttime();
			break;
		case CMD_QUERY_ZIGBEE_CFG:
			Inquiry_ZigbeeCfg();
			break;
		case CMD_WRITE_EEPROM:
			Write2EEPROM(Data_Buf+8,10);
			
	}

}

void ZigbeeSetup(void)          // ��ZIGBEE������Ϣ
{

    uint8_t num=0;
    uint8_t temp_buf[20]= {0};
    memset(temp_buf,0,20);
    //memset(Data_Buf,0,Data_Len);
    UARTInit(38400);
    NVIC_DisableIRQ(UART_IRQn);
    Delay1_MS(20);
    GPIOSetValue(1,8,0);
    Delay1_MS(3000);
    GPIOSetValue(1,8,1);
	Delay1_MS(500);
    while ((LPC_UART->LSR & 0x01) == 0x01)
    {
        num = LPC_UART->RBR;

    }
    num=0;
#if 1
    temp_buf[0]=0x23;
    temp_buf[1]=0xa0;
    UARTSend((uint8_t *)temp_buf,2);
	Delay1_MS(500);
    while ((LPC_UART->LSR & 0x01) == 0x01)
    {

        temp_buf[num] = LPC_UART->RBR;
        num++;

    }
	//��ⷵ�ص������Ƿ���15���ֽ�
	if(num != 15) return; 
#endif
//���������������zigbee�����᷵������
	temp_buf[0] = 0x23;
	temp_buf[1] = 0xfe;
  memcpy(temp_buf+2,Data_Buf+8,14);
	
	UARTSend((uint8_t *)temp_buf,16);
	Delay1_MS(500);
	
    temp_buf[0]=0x23;
    temp_buf[1]=0x23;
    UARTSend((uint8_t *)temp_buf,2);
    Delay1_MS(200);
	
	UARTInit(19200); //open interrput in init
	
// Delay1_MS(200);



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
uint8_t checkSum(uint8_t* buff, uint8_t len)
{
	uint8_t sum = 0;
	uint8_t i = 0;
	for(; i < len; i++)
		sum+=buff[i];
	return sum;
}

void RespCharPara(uint8_t command,uint8_t ack,uint8_t value)
{
	ResponseMsg(command,ack,&value,1);
}

void RespNoPara(uint8_t command,uint8_t ack)
{
	ResponseMsg(command,ack,0,0);
}
void ResponseMsg(uint8_t command,uint8_t ack,uint8_t* context, uint8_t len) //�ظ�Э��
{

	if( (10 + len) > MAX_RESP_BUFF_SIZE ) return;
	
	
    respBuf[0]=PROTO_HEAD;                         //֡ͷ
    respBuf[1]=10 + len;                           //��Ϣ����

	
    memcpy(&respBuf[2],Terminal_ID,4);        //��ַ
    respBuf[6]=group_number;                 //���

    respBuf[7]=(command);                       //������
    respBuf[8]= ack;                          //Ӧ����

	if( (context != NULL) && (len > 0 ))	
    	memcpy(respBuf+9,context,len);

    respBuf[9+len]=checkSum(respBuf,9+len);	

    UARTSend( (uint8_t *)respBuf, respBuf[1] );
    Delay1_MS(10);
	
}