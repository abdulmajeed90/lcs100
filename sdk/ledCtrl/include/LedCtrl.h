#ifndef LEDCTRL_H
#define LEDCTRL_H
#include "ledProto.h"
#include "LedMsgQueue.h"
#include <iostream>
#include <vector>
#include <map>
#ifdef _WIN32
    #ifdef LEDCTRL_EXPORTS
        #define LEDCTRL_API __declspec(dllexport)
    #else
        #define LEDCTRL_API __declspec(dllimport)
    #endif
#else
    #define  LEDCTRL_API
#endif

#define MAX_SCENE_NUM 4
typedef struct
{
    int voltage; //��ѹ
    int current; //����
    int kw; //����
    int brightness; //��ǰʵʱ����ֵ
    int defBright;  //Ĭ�ϵĵ���ֵ
    int group; //�豸���ڵ���
    int adjustTime; //����ʱ��
    int ver;    //���Ƶİ汾��
    int devId; //�豸���
    int sceneNum; //��������
    int sceneLight[MAX_SCENE_NUM]; //ÿ�������ĵ���ֵ
    unsigned char zigbeeCfg[15];
}StreetLight;

#define MAX_SENCE_NUM 10
typedef struct{
    unsigned int  id;
    unsigned char group;
    unsigned int  duty_time;
    unsigned char curSenceId; //��ǰ���� [0-MAX_SENCE_NUM)
    unsigned char sence[MAX_SENCE_NUM];
    unsigned int  pwmtimer; //????
}LedParam;
enum LedEvent{
    EV_REQ,
    EV_DATA,
    EV_VERIFY,
    EV_OK
};
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
struct TEventParam{
    TEventParam(unsigned int _id,unsigned char _group,LedEvent _event,LedError _err=ERR_OK,unsigned int _arg=0)
    {
        id = _id;
        group = _group;
        event = _event;
        err = _err;
        arg = _arg;
    }
    unsigned int id;
    unsigned char group;
    LedEvent event;
    LedError err;
    unsigned int arg;
};
#pragma push(1)
typedef struct{
    unsigned short address;
    unsigned char  netID;
    unsigned char  netType;
    unsigned char  nodeType;
    unsigned char  sendMode;
    unsigned char  baudRate;
    unsigned char  parity;
    unsigned char  dataBit;
    unsigned char  dataMode;
    unsigned char  timeOut;
    unsigned char  channal;
    unsigned char  kw;
    unsigned char  addrMode;
}TZigbeeCfg;
#pragma pop()
typedef std::vector<unsigned int> DeviceList;

typedef std::map<unsigned int,StreetLight> TStreeLightList;

typedef void (*LedCallBackProc)(TEventParam* param,void* arg);

struct ILedEventNofityer
{
	virtual int notify(TEventParam* arg) = 0 ;
};

class LEDCTRL_API LedCtrl
{
public:
    LedCtrl();
    static LedCtrl& get();
/*!
    \brief ���ûص����������κ��첽�¼�������ʱ�򣬾ͻ�ص�֪ͨ�ӿ�
    \param[in] arg :���Ӳ���
*/
	void addObserver(ILedEventNofityer* obs=NULL);
    void setCallBack(LedCallBackProc _callback,void* _arg);
    /*!
    \brief �򿪴��ڣ���ʼ��ģ��
    \return true: �ɹ� false:ʧ��
*/
    bool open(std::string comPath,unsigned int bps=19200);
    /*!
    \brief ���������豸.
    \param[in] file :�����ļ�ȫ·��.
    \param[in] devlist: ���������豸�б�.
*/
    bool upload(std::string file,DeviceList devlist);
 /*!
   ��ȡ�����ļ��ܵ��ļ�����
*/
    int  getUploadFilePacketNum(void);

    /*!
      �ж����������Ƿ��Ѿ�����
*/
    bool hasUploadComplete(void);
    /*!
    �ڲ�ʹ��
*/
    void notify(TEventParam* par);
/*!
  \brief �����豸id�������豸�ĵ�ǰ����ʵʱ����
  \return NULL �޷���ȡ��ID�Ĳ���
*/
    StreetLight*  getLightParam(unsigned int id);
    /*!
       \brief �������,�޸ĵ�ǰ���Ƶ����ΪnewGroup,�������0Ϊ��Ч��ţ�������
       ĳ�����ƴ�ĳ����ɾ�����ͽ����������Ϊ0.
    */
    int setGroup(unsigned int id,unsigned char newGroup,unsigned int waitMs=1000);

    /*!
       \brief �޸ĵ��Ƶ�ID
    */
    int setID(unsigned int id,unsigned int newId,unsigned int waitMs=1000);

/*!
   \brief ֪ͨ�豸[afterMs]ms��λ
*/
    bool setDeviceReset(unsigned int id,unsigned char group,unsigned int afterMs=1000);
/*!
    \brief ��ȡ���Ƹ�λ����
    \param[in] id ���Ƶ�id
    \param[in] waitMs �ȴ�����ms�������û�л�ȡ�����ݾͷ���
    \return ���ظ�λ����
    \retval -1 ��ʱʧ�� >=0 ��λ����
*/
    int  getDeviceResetCount(unsigned int id,long waitMs);

    /*!
    \brief ����ָ��
    \param[in] id ���Ƶ�id
    \param[in] group ���Ƶ����
    \param[in] value ����ֵ0-100
    \param[in] waitMs �ȴ�����ms�������û�л�ȡ�����ݾͷ���
    \return ���ص�ǰ����
    \retval -1 ��ʱʧ�� >=0 ��ǰ����
*/
    int  setBrigtness(unsigned int id,unsigned char group,unsigned char value,long waitMs=1000);

    /*!
    \brief ��˸LED
    \param[in] id ���Ƶ�id
    \param[in] group ���Ƶ����
    \param[in] value ��˸���ʱ��(ms)
    \param[in] waitMs �ȴ�����ms�������û�л�ȡ�����ݾͷ���
    \return ����
    \retval -1 ��ʱʧ�� >=0 ��ǰ����ʱ��
*/
    int  setShakeLed(unsigned int id,unsigned char group,unsigned char value,long waitMs=1000);

    /*!
    \brief д��洢������eeprom��
    \param[in] id ���Ƶ�id
    \param[in] group ���Ƶ����
    \param[in] pPara �洢����
    \param[in] waitMs �ȴ�����ms�������û�л�ȡ�����ݾͷ���,��Ϊeeprom�Ĳ�������������ʱ����΢����
    \return ���ؽ��
    \retval <0 ʧ�� =0 �ɹ�
*/
    int  writeE2prom(unsigned int id,unsigned char group,LedParam* pPara,long waitMs=2000);
    /*!
        \brief ��ȡ����ʵʱ����
        \param[in] id ���Ƶ�id
        \param[in] waitMs �ȴ�����ms�������û�л�ȡ�����ݾͷ���
        \return ����ʵʱ����
        \retval <0 ʧ�� >0 ����ֵ
    */
    int  getCureent(unsigned int id,long waitMs=1000);
    /*!
        \brief ��ȡ����ʵʱ��ѹ
        \param[in] id ���Ƶ�id
        \param[in] waitMs �ȴ�����ms�������û�л�ȡ�����ݾͷ���
        \return ����ʵʱ��ѹ
        \retval <0 ʧ�� >0 ��ѹֵ
    */
    int  getVoltage(unsigned int id,long waitMs=1000);
    /*!
        \brief ��ȡ����ʵʱ����
        \param[in] id ���Ƶ�id
        \param[in] waitMs �ȴ�����ms�������û�л�ȡ�����ݾͷ���
        \return ����ʵʱ����
        \retval <0 ʧ�� >0 ����ֵ
    */
    int  getKw(unsigned int id,long waitMs=1000);
    /*!
        \brief ��ȡ����zigbee����
        \param[in] id ���Ƶ�id
        \param[in] waitMs �ȴ�����ms�������û�л�ȡ�����ݾͷ���
        \return ����zigbee����
        \retval NULL ʧ�� >0 zigbee����
    */
    TZigbeeCfg*  getZigbeeCfg(unsigned int id,long waitMs=1000);
    /*!
        \brief ���õ���zigbee����
        \param[in] id ���Ƶ�id
        \param[in] pCfg zigbee����
        \param[in] waitMs �ȴ�����ms�������û�л�ȡ�����ݾͷ���
        \return ����zigbee����
        \retval NULL ʧ�� >0 zigbee����
    */
    int  setZigbeeCfg(unsigned int id,TZigbeeCfg* pCfg,long waitMs=1000);


    /*!
        \brief ��ȡ���ƹ̼��汾��
        \param[in] id ���Ƶ�id
        \param[in] waitMs �ȴ�����ms�������û�л�ȡ�����ݾͷ���
        \return ���ع̼��汾��(1���ֽ�) (����0x31 ��ʾ V3.1�汾)
        \retval <=0 ʧ�� >0 �̼��汾��
    */
    int  getVersion(unsigned int id,long waitMs=1000);
    /*!
        \brief ���õ��Ƶ���ʱ��
        \param[in] id ���Ƶ�id
        \param[in] time ���Ƶ���ʱ��(0-100)
        \param[in] waitMs �ȴ�����ms�������û�л�ȡ�����ݾͷ���
        \return ����ʱ��
        \retval <0 ʧ�� =0 �ɹ�
    */
    int  setAdjustTime(unsigned int id,unsigned char group,int time,long waitMs=1000);
    /*!
        \brief ��ȡ���Ƶ���ʱ��
        \param[in] id ���Ƶ�id
        \param[in] waitMs �ȴ�����ms�������û�л�ȡ�����ݾͷ���
        \return ����ʱ��
        \retval <0 ʧ�� >=0 ����ʱ��
    */
    int  getAdjustTime(unsigned int id,long waitMs=1000);

    int  getAllData(unsigned int id,unsigned char group,StreetLight* pLight,long waitMs=1000);
    /*!
        \brief ��ȡ�������ڵ����
        \param[in] id ���Ƶ�id
        \param[in] waitMs �ȴ�����ms�������û�л�ȡ�����ݾͷ���
        \return ��� (���Ϊ0��Ϊ��Ч���)
        \retval <=0 ʧ�� >0 ���
    */
    int getGroup(unsigned int id,long waitMs=1000);
    /*!
        \brief ��ȡ����Ĭ�ϵ���ֵ
        \param[in] id ���Ƶ�id
        \param[in] waitMs �ȴ�����ms�������û�л�ȡ�����ݾͷ���
        \return Ĭ�ϵ���ֵ
        \retval <0 ʧ�� >=0 Ĭ�ϵ���ֵ
    */
    int getDefaultAdjValue(unsigned int id,long waitMs=1000);
private:
    int  getIntResp(unsigned int id,LedCmdType type,int size,long waitMs=1000);
    int  setIntResp(unsigned int id,unsigned char group,LedCmdType type,int value,int size,long waitMs=1000);
    
    int  sendMessage(LedMessage* pMsg);
    bool waitRespMessage(LedMessage* pReqMsg,LedMessage* pRespMsg);
    bool checkPacketValid(LedMessage* pReqMsg,LedMessage* pRespMsg);
    unsigned char checkSum(unsigned char* buff, int size);
};

#endif // LEDCTRL_H
