#include "TinyFifo.h"


#define MAX_RECV_SIZE 64
        
//������ֻ�ر��˴����жϣ���Ϊֻ�д����ж��е�����putc����������кܶ��ж��е��ã���ô��Ҫ�����ж�
#define ENTER_CRITICAL()   
#define LEAVE_CRITICAL() 


static  u8 in = 0;
static  u8 out = 0;

static  u8 rxBuff[MAX_RECV_SIZE];


void tinyFifoPutc(u8 c)
{
    *(rxBuff + (in & (MAX_RECV_SIZE - 1))) = c;
	in++;

	if (in - out > MAX_RECV_SIZE)
		out++;

}
s8   tinyFifoGetc(u8* c)
{
    ENTER_CRITICAL();
    if (in == out)
    {
        LEAVE_CRITICAL();
		return (s8)-1;
    }
	*c = *(rxBuff + (out & (MAX_RECV_SIZE - 1)));
    out++;
    LEAVE_CRITICAL();
	return 0;
}

u8   tinyFifoLength()
{
	return (in - out);
}

u8   tinyFifoEmpty()
{
    return  tinyFifoLength()?0:1;//uart1RxFlag?0:1;
}

