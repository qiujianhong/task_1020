#ifndef _MDL_MGR_H_
#define _MDL_MGR_H_



#include "common.h"
#include "commfun.h"
#include "OopAll.h"
#include "power.h"
#include "commdrv.h"

#ifdef __cplusplus
extern "C"{
#endif

#define CONFIG_FILE                 "/tmp/mapManager.ini"

#define MDL_PRTL_VER0               0
#define MDL_PRTL_VER1               1

#define TEMP_DINGXIN_MDL            1                   //��ʱ����ģ��
#ifdef  TEMP_DINGXIN_MDL            
    #define    DINGXING_TYPE_NAME   "GBH32-DX001"       //��ʱ������                   
#endif

#define MDL_LOG_DIR                 "/data/app/mapManager/log"      //��־·��

#define MAX_QUEUE_CNT               64
#define MAX_INFO_QUEUE_CNT          64

#define MAX_QUEUE_BUFF_LEN          5000        //ͨѶ����buff����
#define MAX_MSG_PAYLOAD_LEN         2040        //��ϢMSG��payload��󳤶�

#define mdlnum                      5

/* ֡�ṹƫ�� */
#define H_SITE_M                    0           // ��һ��0x7e
#define L_LOW_M                     1           // ���ݳ��ȵ��ֽ�
#define L_HIGH_M                    2           // ���ݳ��ȸ��ֽ�
#define C_SITE_M                    3           // ������
#define D_SITE_M                    5           // ������

/* ģ������� */
#define ACK_F                       0           // ȷ��֡
#define NAK_F                       1           // ����֡
#define NGT_F                       2           // ��·Э��֡
#define INF_F                       2           // ��Ϣ֡

/*   ������   */
#define GET_N                       0           // ��ѯ����
#define CTL_N                       1           // ��������
#define RPT_N                       2           // ״̬֪ͨ
#define ASK_N                       3           // ���󳭶�


/* �������� */
//#define RET_OK                      0
#define RET_ERR                    -1

#define MSG_MQTT  1    

#define  MQTT_INIT           0               //��ʼ��mqtt����
#define  MQTT_RECV           1               //mqtt���ݽ���
#define  MQTT_SEND           2               //mqtt���ݷ���
#define  MQTT_CLOSE          3               //�ر�mqtt����
#define  MQTT_LOGIN          4               //��¼��ϵͳ������
#define  MQTT_LOGIN_WAIT     5               //��¼�ȴ��ظ�

#define  MAX_BUF_LEN       3000

#define     MAX_CHAR_LEN        256
#define     MAX_VIRTURL_CNT     10

typedef struct
{
    uint8   sDeviceName[50];    // �豸������"/dev/USB0"
    uint32  baud;               // ������
    uint32  databits;           // ����λ, 4-8 
    uint32  stopbits;           // У�� 'n'  'e'  'o'  's' = None,Odd,Even,Space
    uint32  parity;             // ֹͣλ 0,1,2 = 1, 1.5, 2
    int32   fdUart;             
} COMM_PARA_T;

typedef enum
{
    IDLE_STATE = 0,             // ����
    INIT_STATE,                 // ��ʼ��
    NGTT_SEND_STATE,            // Э��
    NGTT_RECV_STATE,            // Э�̽���
    SEND_STATE,                 // ���� 
    RECV_STATE,                 // ����   
    WAIT_STATE,                 // ���� 
    CLOSE_STATE,                // �ر�
    OTHER_STATE,                // ����״̬
} MODULE_STATE_E;

typedef enum
{
    UNKNOW_STATE,               // δ֪״̬
    ON_STATE,                   // �����״̬
    OFF_STATE,                  // δ���״̬ 
} USB_STATE_E;


//typedef struct
//{
//    uint8   seq;
//    uint8   cmd_h;                  // ������
//    uint8   afn;
//    uint16  fn;
//    uint8   pDataBuf[2048];
//    int16   dataLen;
//}FRAME_INFO_T;


typedef struct
{
    uint16  prtlVersion;                //Э��汾��
    uint8   tmnModeLen;                 //�ն��豸�ͺų���
    char    tmnMode[MAX_CHAR_LEN];      //�ն��豸�ͺ�
    uint8   tmnIDLen;                   //�ն��豸ID����
    char    tmnID[MAX_CHAR_LEN];        //�ն��豸ID
    uint16  sendBufLen;                 //����ͻ���������
    uint16  recvBufLen;                 //�����ջ���������
    uint8   conCurrWDs;                 //�������䴰����
}LINKREQUEST_T;                         //��·����

typedef enum
{
    CDC_ACM = 1,        
    CDC_ECM,
    HID
}INTERFACE_TYPE_T;

typedef struct
{
    INTERFACE_TYPE_T    interf;
    uint8               funcConfig;
    char                driveName[MAX_CHAR_LEN];
}VIRTURL_PIPE_MODE_T;


typedef struct
{
    uint16  prtlVersion;                //Э��汾��
    uint8   modModeLen;                 //ģ���豸�ͺų���
    char    modMode[MAX_CHAR_LEN];      //ģ���豸�ͺ�
    uint8   modIDLen;                   //ģ���豸ID����
    char    modID[MAX_CHAR_LEN];        //ģ���豸ID
    uint16  sendBufLen;                 //����ͻ���������
    uint16  recvBufLen;                 //�����ջ���������
    uint8   conCurrWDs;                 //�������䴰����
    uint8   virPipeCnt;                 //����ͨ����
    VIRTURL_PIPE_MODE_T     virturMode[MAX_VIRTURL_CNT];    //����ͨ������ģʽ
}LINKRESPONSE_T;            //��·��Ӧ��Ϣ


typedef struct
{
    uint16  year;
    uint8   month;
    uint8   dayofmonth;
    uint8   dayofweek;
}DATE_T;

typedef struct
{
    uint8   DevTypeLen;             //�豸�ͺų���
    char    DevType[MAX_CHAR_LEN];  //�豸�ͺ�
    uint8   DevIDLen;               //�豸ID����
    char    DevID[MAX_CHAR_LEN];    //�豸ID
    uint32  softVersion;            //����汾
    DATE_T  date_soft;              //�����������
    uint32  hardwVersion;           //Ӳ���汾
    DATE_T  date_hard;              //Ӳ����������
    uint8   VendorCodeLen;          //���̱��볤��
    char    VendorCode[MAX_CHAR_LEN];   //���̱���

    //��Э��
    uint8   type;                   //Э����
    char    softVer[MAX_CHAR_LEN];  //����汾
    char    softDate[MAX_CHAR_LEN]; //����汾
    char    hardVer[MAX_CHAR_LEN];  //Ӳ���汾
    char    hardDate[MAX_CHAR_LEN]; //Ӳ���汾
    char    VendorExt[MAX_CHAR_LEN];//������չ
}MODULE_INF_T;

typedef struct
{
    uint32  packCnt;            //�ܰ���
    uint16  packState;          //������״̬
}FILE_TRANS_STATE_T;

typedef enum
{   
    powerOn,                //�ϵ�
    powerOff                //�ϵ�
}POWER_STATE_E;             //��Դ״̬

//typedef struct
//{
//  uint8   fileID;             //�ļ���ʶ
//  uint32  segNo;              //�κ�
//  uint32  segCnt;             //�ܶ���
//  uint8   fileData[1024];     //�ļ�����
//}FILE_TRANS_SET_T;                //�ļ�����

//typedef struct
//{
//  uint8           validFlag;      //��Ч�Ա�־
//  MODULE_INF_T    mod_Info;       //����λģ����Ϣ
//}MODULE_INF_F_T;


typedef struct
{
    uint8               modInfoFlag;    //ģ����Ϣ��־(0x55:��Ч;0xAA:����δʶ��;)
    MODULE_INF_T        moduleInfo;     //ģ����Ϣ
    LINKRESPONSE_T      linkInfo;       //��·��Ϣ
    uint16              protocolVer;    //Э��汾
    POWER_STATE_E       powerSta;       //��Դ״̬
    //FILE_TRANS_STATE_T    fileState;      //�ļ�����״̬
    Date_Time_t         clock;          //ʱ��
    //FILE_TRANS_SET_T  fileTrans;      //�ļ�����
}MODE_ALL_INFO_T;               //ģ��ͨ����Ϣ��

typedef struct
{
    uint8           YX_state;           //ң��״̬
    uint8           YX_change;          //ң�ű�λ
    Date_Time_t     date_time[4];       //ң�ű�λʱ��            //Ŀǰ��4·ң��
}REMOTE_STATE_T;    

typedef struct
{
    uint32      pulseCnt[4];        //������
    uint32      time[4];            //ͳ��ʱ��
    
}PULSE_CNT_T;               //�������


typedef struct
{
    REMOTE_STATE_T      remote_state;       //ң��״̬
    PULSE_CNT_T         pulse_cnt_last;     //��һ���������
    PULSE_CNT_T         pulse_cnt;          //�������
    uint8               YX_FuncSet;         //ң�Ż����幦�����ã�ѡ��
    uint16              YX_AntiShakeTime;   //ң�ŷ���ʱ��
    
}REMOTE_PULSE_T;    

typedef enum
{
    PULSEMODE = 1,
    LEVELMODE,
}RELAY_OUTMODE_E;               //�̵������ģʽ
    
typedef enum
{
    /* LINK-APDU */
    LINKREQUEST = 1,       /* Ԥ�������� */
    GETREQUEST,          /* ��ȡ���� */
    SETREQUEST,            /* �������� */
    REPORTREQUEST,         /* �ϱ�֪ͨ */

    /* LINK-APDU  */
    LINKRESPONSE = 129,    /* Ԥ������Ӧ */
    GETRESPONSE,           /* ��ȡ��Ӧ */
    SETRESPONSE,           /* ������Ӧ */
    REPORTRESPONSE,     /* �ϱ�Ӧ�� */
}ENUM_FRAME_FUNC_T;

typedef enum                //Data Access Result
{
    ACTSUCCESS,
    REFUSED,
    NODEFINITION,
    
    OTHERS = 255
}DAR_T;

typedef struct
{
    uint8           plugEvent;      //�Ƿ��������¼�
    int32           usbNo;          //ģ�����
    USB_STATE_E     plugStatus;     //ģ����״̬
}PLUG_MODE_T;               //ģ�����¼�

typedef struct
{
    USB_STATE_E     plugStatus;         //ģ�����״̬
    uint8           changeStatus;       //ģ��״̬��λ
    time_t          time;               //��λʱ��
}PLUG_STATUS_T;             //ģ����״̬

typedef struct
{
    uint8   modeTypeLen;                    //ģ���ͺų���
    char    modeType[MAX_CHAR_LEN];         //ģ���ͺ�
    int32   usbNo;                          //��λ���
}SLOT_INFO_T;

typedef struct 
{
    char channelName[256];                  //ģ��ͨ����Ϣ
    char channelPath[128];                  //ģ��ͨ����Ϣ·��
    char channelInfo[64];                   //ģ��ͨ���˵�����
}MDL_CHANNEL_CFG_T;

typedef union MDL_CNTL
{
    struct
    {
        uint8   ctlcode:6;
        uint8   prm:1;
        uint8   dir:1;
    }bit;
    uint8   all;
}MDL_CNTL_U;                    // ������ 

typedef struct
{
    ENUM_FRAME_FUNC_T frameFunc;
    uint16 dataLen;                         //����֡����
    MDL_CNTL_U cntl;
    uint8   seq;
    uint8  pDataBuf[2048];                  //����֡����
    uint16 fcs;
}MDL_FRAME_INFO_T;

//typedef struct
//{
//  ENUM_FRAME_FUNC_T   frameFunc;  //��Ϣ֡������
//  uint16              dataLen;    //�����򳤶�
//  uint8               crtlCode;   //������
//  uint8               prm;        //������־
//  uint8               dir;        //���䷽��
//  uint8               fid;        //֡���
//  char                apdu[2048]; //������
//  uint16              fcs;        //У����
//}MDL_FRAME_INFO_T;

typedef union SFP_CMD_L
{
    struct
    {
        uint8   seq:6;
        uint8   prm:1;
        uint8   dir:1;
    }bit;
    uint8   all;
}SFP_CMD_L_U;                   // ������ 

#define VM_COMM_NUM_MAX       8 // ����ͨ�������
typedef struct
{
    uint16  ver_id;                             // Э��汾��
    uint8   tmn_tp[32];                         // �ն��豸�ͺ�
    uint8   tmn_id[32];                         // �ն��豸�ͺ�
    uint8   mdl_tp[32];                         // ģ���豸�ͺ�
    uint8   mdl_id[32];                         // ģ���豸ID
    uint16  mx_sd_len;                          // ����ͻ��������ȣ��ֽڣ�
    uint16  mx_rv_len;                          // �����ջ��������ȣ��ֽڣ�
    uint8   tx_wid_nm;                          // �������䴰����
    uint8   vm_comm_nm;                         // �������ͨ����
    uint8   vm_comm_tp[VM_COMM_NUM_MAX];        // ����ͨ���ӿ�ģʽ
}FRAME_NGT_T;

/* �����ֽڶ���, û���ϸ��ձ��ĸ�ʽ, �յ�����Ϣ���������¸�ʽ */
typedef struct tagInfoData
{
    uint32 IP;        //Ŀ�Ķ�ip
    uint16 port;
    uint8  res[2];
    
    uint8  MsgRPM;         /* ������־λ, 1-����, 0-������ */
    uint8  MsgPRIORITY;    /* ���ȼ� */
    uint16 MsgIndex;       /* ��Ϣ��� */
    uint16 MsgLabel;       /* ��Ϣ��ǩ */

    uint8  SourLen;
    uint8  DestLen;
    char   SourAddr[128];
    char   DestAddr[128];

    uint16 IID_ID;
    uint16 IOP_ID;

    uint32 nPayloadLength; 
    uint8  aPayloadData[MAX_MSG_PAYLOAD_LEN];
}INFO_DATA_T;

typedef struct tagMsgInfo
{
    uint32 IP;        //Ŀ�Ķ�ip
    uint16 port;
    uint8  res[2];
    
    uint8  MsgRPM;         /* ������־λ, 1-����, 0-������ */
    uint8  MsgPRIORITY;    /* ���ȼ� */
    uint16 MsgIndex;       /* ��Ϣ��� */
    uint16 MsgLabel;       /* ��Ϣ��ǩ */

    uint8  SourLen;
    uint8  DestLen;
    char   SourAddr[128];
    char   DestAddr[128];

    uint16 IID;
    uint16 IOP;

    uint32 MsgLen; 
    uint8  MsgData[0];
}MSG_INFO_T;


/** @brief �����ʼ������    */
//typedef struct
//{
//  int8            name[MQT_NAME_LEN];     /**< ����                     */
//  uint32          eventMask;              /**< �����¼�                   */
//  uint32          msgQueLen;              /**< ��Ϣ���г���(0Ϊȱʡ)     */
//  uint32          msgReadLen;             /**< ��Ϣ��ȡ���泤��(0Ϊȱʡ)   */
//  VERSION_T       version;                /**< �汾��                      */
//} MQT_PROFILE_T;


//����
typedef struct
{
    int32  QueueLen;                            //���г���
    uint8   QueueBuf[MAX_QUEUE_BUFF_LEN];       //���д洢������
}Queue_T;

//�����������ṹ
typedef struct
{
    pthread_mutex_t QueueLock;                  //���ջ�����������
    int16  QueueHead;                           //����ͷ
    int16  QueueTail;                           //����β
    Queue_T Queue[MAX_QUEUE_CNT];               //ͨѶ���ݶ���
}LockQueue_T;

//�̲߳����ṹ
typedef struct
{
    uint8        nArrIndex;                     //�����±�
    LockQueue_T *RecvBuf;                       //���ջ������ṹָ��
    LockQueue_T *SendBuf;                       //���ͻ������ṹָ��
}ThreadArg_T;


typedef struct
{
    pthread_mutex_t FrameLock;                      //������
    uint8           flag[MAX_INFO_QUEUE_CNT];       //��Ч�Ա�־
    INFO_DATA_T     InfoData[MAX_INFO_QUEUE_CNT];   //mqtt��Ϣ֡
}InfoDataQueue_T;                               //�����Ѵ����mqtt֡��Ϣ
/********************************************************************************
 * @brief   �߳�ά���Ĵ������߳�ID�� 
 * @note    ntid:�߳�id��
 *          cnt_thread_msg:�߳�ά������
 *******************************************************************************/
typedef struct
{
    pthread_t ntid; 
    int16 cnt_thread_msg;       
} MDL_THREAD_DEFEND_T;

/********************************************************************************
 * @brief   �߳�ά���Ĵ����ͱ�־��Ϣ    
 * @note    ntid:�߳�id��
 *          port:ͨ����
 *          cnt_thread_msg:�߳�ά������
 *******************************************************************************/
typedef struct
{
    int32 port;
    int16 flag_thread_msg;
} MDL_THREAD_INFO_T;

/**
 ******************************************************************************
 * @brief       ע��ģ���������ز���
 * @param[in]   pProfile    :   ������ò���
 * @return      �ɹ�����TRUE, ʧ�ܷ���FALSE
 * @note                    
 ******************************************************************************
 */
//BOOLEAN mdl_regist_init(const MQT_PROFILE_T *pProfile);


/* ������ */
typedef union _CMT_FIELD_U
{
    struct
    {
        uint8 fid :6;   //֡���
        uint8 frm :1;   //������ʶ
        uint8 dir :1;   //���䷽��
        uint16 cmd :8;  //������
    };
    uint16 wcmd;
}CMT_FIELD_U;

typedef struct
{
    uint16 len;
    CMT_FIELD_U cmd_field;
    uint8 afn;
    uint8 dt1;
    uint8 dt2;
    int16 dataLen;
    uint8 *pDataBuf;
}__attribute__ ((packed)) FRAMEOFMODULE_INFO;

typedef struct _MDL_MSG_T
{
    char mdlType[32];
    char mdlId[32];
    char softVer[4];
    char softDate[10];
    char hardVer[4];
    char hardDate[10];
    char facVer[4];
}__attribute__ ((packed)) MDL_MSG_T;

typedef enum
{
    MDL_UPDATE_IDLE = 0,                               //����
    MDL_UPDATE_INIT,
    MDL_UPDATE_DELAY,                              //��ʱ
    MDL_UPDATE_READ_FILE_DELAY,
    MDL_UPDATE_OPEN_FILE,                          //�������ļ�
    MDL_UPDATE_READ_FILE,                          //��ȡ�����ļ�
    MDL_UPDATE_SEND,                               //����
    MDL_UPDATE_RECV,                               //����
    MDL_UPDATE_RETRY,                              //����
    MDL_UPDATE_AGAIN,                          //��������
    MDL_UPDATE_GET_RUN_STATE,                      //��ȡ·������״̬
    MDL_UPDATE_REBOOT_DEV,
    MDL_UPDATE_GET_VER,                            //�汾��Ϣ
    MDL_UPDATE_END,                                //��������
}MDL_UPDATE_DEAL_T;                      //�ز�ģ������״̬��

typedef struct
{
   uint32  TotalPackageNum;
   uint32  CurrentPackageNo;
   uint16  PackageLen;
}MdlUpdateInfoState;     //ģ��ģ�鱾���������ܼ�¼�ṹ��

typedef enum
{
    MDL_COMM_IDLE = 0,                          //����
    MDL_COMM_SEND,                              //����
    MDL_COMM_RECV,                              //����
    MDL_COMM_RETRY,                             //����
    MDL_COMM_DELAY,                             //��ʱ
}MDL_COMM_STATE_T;                           //����ͨѶ״̬��

typedef enum _MDL_UPDATE_STEP_E
{
    UPDATE_IDLE = -1,
    UPDATE_START = 0,
    UPDATE_RUN,
    UPDATE_END,
}MDL_UPDATE_STEP_E;

#define L_SIZE_M                    1           //���ݳ���-
#define AFN_SITE_M                  5           //afn
#define DT1_SITE_M                  6           //dt1
#define DT2_SITE_M                  7           //dt2
#define DATA_SITE_M                 8           //���ݿ�ʼλ��

// ������Ϣ��
#define COMM_ERROR                  -1          //һ���Դ���
#define MODULE_TYPE_ERROR           -2          //ģ������������е�ģ�����Ͳ�һ�´���
#define MODULE_CHANGE               -3          //ģ����

#define COMM_OK                     1           //һ������ȷ

// ģ�鹦����
#define GET_M                       0           //ģ�鳭������
#define CTRL_M                      1           //ģ���������
#define NOTIFY_M                    2           //״̬֪ͨ
#define READ_M                      3           //���󳭶�   

#define MOD_CTR_OK   0       // ȷ��
#define MOD_CTR_NO   1       // ����
#define MOD_CTR_LIK  2       // ��·Э��
#define MOD_CTR_MSG  3       // ��Ϣ֡

#define UPDATE_PACKAGE_LEN 256
#define MAX_UPDATE_DELAY_TIME 6
#define MAX_MDL_COMM_DELAY_TIME 6

#define MDL_UPDATE_BUF_LOG               0    //ģ��������־

#define     MAX_INFO_DATA_NUM   500                         //��ϢMSG�����б�

//��Ϣ�б�����
typedef  struct
{
    int16 InfoDataNum;  //��Ϣ�б�����������
    int16 InfoDataReadIndex;  //��ȡ�����������±꣬��0��ʼ��
    INFO_DATA_T InfoDataList[MAX_INFO_DATA_NUM];  //��ϢMSG�б�����
} INFO_DATA_LIST_T;  //��Ϣ�б�����

typedef struct
{
    uint8   DTA;
    uint8   DTB;
    uint8   seq;                            //���к�
    uint8   clkSetFlag;                     //�Ƿ�������
    uint16  dataLen;                        //���ݳ���
    uint8   setData[1024];                  //��Ҫ���õ�����
}TRANS_INFO_T;              //��ģ��֡��Ϣ��


typedef struct
{
    uint8       fileID;                 //�ļ���ʶ
    uint16      packDataLen;            //���ݰ�����
    uint32      packNo;                 //��ǰ��
    uint32      packCnt;                //�ܰ���
    uint8       packData[512];          //�ļ�����
} __attribute__((packed)) UPDATE_INFO_T;

typedef struct
{
    MODULE_INF_T mdl_up_info[mdlnum];                       
}UPDATE_INFO_ALL_T;         //��������Ϣ�ṹ��

typedef struct
{
    uint8   offpowerflag;   //�ϵ��־
    time_t  offpowertime;   //�ϵ�����
    INFO_DATA_T infotemp;
}MOD_OFF_POWER_T;

extern  LockQueue_T LockSendBuf[mdlnum];

//extern char       *MDL_MGR_RECD_LOG;

extern int  g_MqttThreadFlag;            //���ڼ��mqtt�߳��Ƿ�����

extern uint8            Event_Flag[mdlnum] ;        //���ڼ���Ƿ�����¼�

extern PLUG_STATUS_T    plug_status[mdlnum];        //ģ����״̬

extern SLOT_INFO_T      slotInfo[mdlnum];           //ģ���λ��Ϣ

extern MODE_ALL_INFO_T  moduleInfo_all[mdlnum];     //ģ��ͨ����Ϣ

extern REMOTE_PULSE_T   YX_pulse_info;              //ң������ģ����Ϣ��

extern RELAY_OUTMODE_E  relayMode;                  //�̵������ģʽ

extern uint16           pulseWidth;                 //�̵������������

extern UPDATE_INFO_ALL_T    updateAllInfo;          //�������ݽṹ��

extern uint8            globleSeq;                  //ȫ�����к�

extern MOD_OFF_POWER_T  modPowerCtl[mdlnum];        //ģ��ϵ������Ϣ

extern POWER_DEVICE_T*  dev_power;

extern BOOL             gBuzzerEn;

extern  int32   makeInfoData(uint8 usbNo,MODULE_INF_T   *pModuleInfo, INFO_DATA_T *pRecvInfo,      uint8 seq, INFO_DATA_T *pSendInfo);

extern  int32   makeInfoData_new(uint8 usbNo,MODULE_INF_T   *pModuleInfo, INFO_DATA_T *pRecvInfo,      uint8 seq, INFO_DATA_T *pSendInfo);
    
extern int8     AddInfoDataIntoList(INFO_DATA_T RecvInfoData);
    
extern int32    MDLWriteInfo(InfoDataQueue_T *pInfoDataQueue, INFO_DATA_T *pInfoData);
    
extern int32    MDLReadInfo(InfoDataQueue_T *pInfoDataQueue, uint8 frameIndex, INFO_DATA_T *pInfoData);
    
extern int32    MakeModeFrame(MDL_FRAME_INFO_T* frameInfo, uint8* pSendBuf);
    
extern int32    MakeFrame(int32 usbNo, TRANS_INFO_T* pTransInfo, uint8* pSendBuf);
    
extern int32    PlugEventInform(uint8 usbNo);
    
extern int      GetHeadInfo(INFO_DATA_T InfoMsgData, MSG_INFO_T *ptMsgInfo);

extern  void    ClearNgtMsg(int32 usbNo);

extern  int32   AnalysisFrameOfModule(int32 usbNo, MDL_FRAME_INFO_T *desFrameInfo, uint8 *pOutBuf,BOOL isupdate);

extern  int32 AskLocalInfo(int32 usbNo, uint8 *czBufIn, int16 len,uint8 seq);

extern  int32 GetmoduleClock(int32 usbNo);

extern char *get_logname_by_pipe(char* filepath,int8 pipe);

extern char mdlLog[100];

extern char *gDownRecordPath[mdlnum + 1];

#define MDL_LOG           ((mdlLog[0]!=0)?mdlLog:"/data/app/mapManager/log/mdlmgr.log")    //��־����

//��ӡ��ʽ����Ϣ��TRACE�ǲ���¼��־��,DEBUG�Ǳؼ���־��,LOGCC���ܿؼ���־��
#define MDLMGR_FMT_TRACE(pipe,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 28, gDownRecordPath[pipe], 0, NULL, 0, fmt, ##__VA_ARGS__)
#define MDLMGR_FMT_DEBUG(pipe,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 28, gDownRecordPath[pipe], 1, NULL, 0, fmt, ##__VA_ARGS__)
//��ӡ��������buffer����,��ʽΪÿ2λ16��������һ��
#define MDLMGR_BUF_TRACE(pipe,buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 28, gDownRecordPath[pipe], 0, buf, len, fmt, ##__VA_ARGS__)
#define MDLMGR_BUF_DEBUG(pipe,buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 28, gDownRecordPath[pipe], 1, buf, len, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
