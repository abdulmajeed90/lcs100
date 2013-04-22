#ifndef LED_PROTO_INCLUDED
#define LED_PROTO_INCLUDED

#define PROTO_HEAD 0xA3

enum LedError{
    ERR_OK=0,
    ERR_TOO_LARGE,
    ERR_SESSION,
    ERR_ERASE,
    ERR_WRITE_DATA,
    ERR_CRC,
    ERR_WRITE_CRC,
    ERR_PACKET_NUM
};

enum LedMode
{
	MODE_BOOT=0,
	MODE_APP =1
};

enum   LedCmdType
{
    CMD_NONE=0,     //��Ч��������
    CMD_UPLOAD_REQ, //��������
    CMD_UPLOAD_DATA,    //�������ݰ�
    CMD_UPLOAD_VERIFY,  //��������У��
    CMD_BOOTMODE,   //��������ģʽ
    CMD_RESET,  //�豸��λ
    CMD_ADJUST_BRIGHTNESS, //����
    CMD_TWINKLE, //��˸
    CMD_HEART, //������
    CMD_SET_ADJ_TIME, //���õ���ʱ��
    CMD_SET_DEFAULT_BRIGHTNESS, //����Ĭ�ϵ���ֵ
    CMD_SET_GROUP, //�������
    CMD_REMOVE_GRUOP, //�����豸�Ӹ���ɾ��
    CMD_SET_SCENE_BRIGHTNESS, //����ĳ������������
    CMD_REMOVE_SCENE, //ɾ��ĳ������
    CMD_SET_BAUD,//����ͨѶ������
    CMD_ENTER_SCENE, //����ĳ������
    CMD_WRITE_EEPROM, //��дEEPROM
    CMD_MODIFY_DEVID, //�޸��ն�ID
    CMD_QUERY_VOLTAGE,  //��ѯ��ѹ
    CMD_QUERY_CURRENT, //��ѯ����
    CMD_QUERY_KW,   //��ѯ�й�����
    CMD_QUERY_BRIGHTNESS, //��ѯֱ�ӵ���ֵ
    CMD_QUERY_DEFAULT_BRIGHTNESS,//��ѯĬ�ϵ���ֵ
    CMD_QUERY_ADJ_TIME, //��ѯ����ʱ��
    CMD_QUERY_GROUP,    //��ѯ���
    CMD_QUERY_SCENE_BRIGHTNESS, //��ѯĳ������������
    CMD_QUERY_DEVICE_STATUS, //��ѯ�豸����״̬���޹���֮��
    CMD_QUERY_VERSION,  //��ѯ���Ƶİ汾��
    CMD_QUERY_ALL, //��ѯ���е��ƵĹ�����������ԼͨѶ����
    CMD_BROADCAST_DEVID, //�㲥��ѯ�����豸ID
    CMD_SET_ZIGBEE_CFG,  //�Ե��Ƶ�zigbee�豸��������
    CMD_GET_RESET_CNT, //��ȡ���Ƹ�λ����
    CMD_QUERY_ZIGBEE_CFG,
	CMD_QUERY_MODE, //��ѯ��ǰģʽ [0: bootģʽ��1: appģʽ]
};


#endif
