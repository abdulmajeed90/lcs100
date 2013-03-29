#ifndef __CS5463_H 
#define __CS5463_H

/* 
 * ��,���ڶ���SPI_CS��Ƭѡ����
 */
#define SPI0_CS		   (1<< 2)											/* P0.2��ΪCS5463��Ƭѡ�� 	*/.
#define CS5463_Reset_Pin  (1<< 2)//��λ��
#define CS5463CS_Low()	 GPIOSetValue(PORT0, 2,0)
#define CS5463CS_High()	 GPIOSetValue(PORT0, 2,1)

#define CS5463_Reset_H()	LPC_GPIO1->DATA |= CS5463_Reset_Pin
#define CS5463_Reset_L()	LPC_GPIO1->DATA &=~CS5463_Reset_Pin
/*------------------------------------��ȡ��������-----------------------------------------*/
#define WrConfig	0x40	//	д����������

#define WrIgain 0x44	//	д��������ָ��
#define RdIgain 0x04	//	����������ָ��

#define WrVgain 0x48	//	д��ѹ����ָ��
#define RdVgain 0x08	//	����ѹ����ָ��

#define WrCounter	0x4a	//	дһ�����ڲ�������ָ��
#define RdCounter	0x0a	//	��һ�����ڲ�������ָ��

#define Prms	0x14	//	���й�������Чֵָ��
#define Irms	0x16	//	�ȵ�����Чֵָ��
#define Urms	0x18	//	����ѹ��Чֵָ��

#define WrPgain 0x5c	//	д����ƫ����
#define RdPgain 0x1c	//	������ƫ����

#define WrStatus	0x5e	//	д״ָ̬��
#define RdStatus	0x1e	//	��״ָ̬��

#define WrIacoff	0x60	//	д����ƫ��ָ��
#define RdIacoff	0x20	//	������ƫ��ָ��

#define Qavg	0x28		//	��ƽ���޹�����ָ��

#define PFrms	0x32		//	����������ָ��
#define Srms	0x36		//	�����ڹ���ָ��
#define Frms	0x1a

#define WrVacoff	0x62
#define RdVacoff	0x22

#define WrMode	0x64	//	д����ģʽ

#define WrMask	0x74	//	д�ж�����ָ��

#define Halt	0xA0	//	����CS5463��

extern void  SSP_Init (void);
extern uint8_t SPI0_ReceiveByte (void);
extern void SPI0_SendByte (uint8_t data);
extern uint32_t CS5463_ReadData(uint8_t RegAdd);
extern void CS5463_Init(void);
extern void Delay_MS(uint32_t ulTime);
extern void Delay_US(uint32_t ulTime);



#endif

