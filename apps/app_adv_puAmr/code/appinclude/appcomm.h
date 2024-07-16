/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��Ӧ�õ��Ƚ���ͨ�����ݺͲ���
* @date��    2019-12-3
* @history�� 
*********************************************************************
*/
#ifndef __APPCOMM_H
#define __APPCOMM_H

#include "commuprot.h"
#include "OopAll.h"
#include "db_api.h"

#ifdef __cplusplus
        extern "C" {
#endif

#define APP_TRHEAD_FLAG_MUX       120

#define APP_698_MESSAGE_MIN_LEN   13    //������С����
#define APP_698_HEAD_MIN_LEN      8     //����֡ͷ��С����

#define APP_698_LEN_REGIN_SIZE    2     //���ĳ����򳤶�
#define APP_698_CTL_REGIN_SIZE    1     //���Ŀ����򳤶�
#define APP_698_SA_REGIN_MAXSIZE  17    //����SA��󳤶�
#define APP_698_CA_REGIN_SIZE     1     //����CA����
#define APP_698_CS_REGIN_SIZE     2     //����У�鳤��
#define APP_698_SEPARA_REGIN_SIZE 2     //����APDU�ֶα��

#define APP_698_A_REGIN_OFFSET    4     //��ַ��ƫ��

#define APP_698_HEAD_BASE_SIZE    \
    (APP_698_LEN_REGIN_SIZE + APP_698_CTL_REGIN_SIZE +  1 + APP_698_CA_REGIN_SIZE + APP_698_CS_REGIN_SIZE)

#define CHECK_RESULT_SUCCEED      0
#define CHECK_RESULT_FAILUER      -1
#define CHECK_RESULT_CONFIRM      1
#define CHECK_RESULT_DENY         2

typedef struct FrameHeadInfo_t_
{
    uint8 pipe;//����ͨ����
	LinkLayerCtrl_t ctrl;//������
	OOP_TSA_T tsa;
	OOP_OAD_U oad;
    uint8 cAddr;
}FrameHeadInfo_t;

typedef struct ProxyHeadInfo_t_
{
    FrameHeadInfo_t FrameHeadInfo;
    SecurityMode_t   SecMod;
    uint8            Useid;
    uint16           label;
    uint8            PIID;
}ProxyHeadInfo_t;


typedef struct 
{
	uint8 PIID;
	uint8 ServerID;
	uint8 nChioce;
	SecurityMode_t SecMod;
	FrameHeadInfo_t FrameHeadInfo;		//֡ͷ��Ϣ
//	PROXY_STATE state;					//��͸������״̬
//	time_t RelayStartTime;					//�м̿�ʼʱ��
//	time_t SendTime;						//���͵���ʼʱ��
//	uint16 TimeOut;							//����ʱʱ��
	uint16 RequestApduLen;					//���������APDU��
	uint8  pRequestApdu[1400];					//������������
}Proxyresquest;

typedef struct 
{
	uint8 PIID;
	SecurityMode_t SecMod;
	FrameHeadInfo_t FrameHeadInfo;		//֡ͷ��Ϣ
//	PROXY_STATE state;					//��͸������״̬
//	time_t RelayStartTime;					//�м̿�ʼʱ��
//	time_t SendTime;						//���͵���ʼʱ��
//	uint16 TimeOut;							//����ʱʱ��
	uint16 ResponseApduLen;					//���������APDU��
	uint8  ResponseApdu[2100];					//������������
	uint8 ServerID;
    uint8 UseID;
}ProxyResponse;

typedef struct
{
    uint16  Queue_Len;                     //����β
    uint8   Queue_Data[2200];               //ͨѶ���ݶ���
}Proxy_Queue; 

typedef struct
{
	pthread_mutex_t QueueLock;                  //���ջ�����������
    int16  QueueHead;                          //����ͷ
    int16  QueueTail;                          //����β
    Proxy_Queue Queue[10];               //ͨѶ���ݶ���
}ProxyLockQueue; 

typedef struct tag_APPURTENANT_METER_INFO    
{
   uint8 pipeaddr[6];//����ͨ����ַ
   uint8 meteraddr[6];//���ܱ��ַ
   uint8 phaseinfo;
   uint16 pn;
   

}OOP_APPURTENANT_METER_INFO_T;

typedef struct tag_APPURTENANT_METER_LIST    
{
   uint8 num;//�������
   OOP_APPURTENANT_METER_INFO_T meterinfo[12]; 

}OOP_APPURTENANT_METER_LIST_T;

typedef struct tag_PERCEPTION_INFO    
{
   uint8 terminaladdr[6];//��֪�ն˵�ַ
   OOP_APPURTENANT_METER_LIST_T meterlist;  

}OOP_PERCEPTION_TERMINAL_INFO_T;

typedef struct tag_PERCEPTION_TERMINAL    
{
   //pthread_mutex_t lock;
   uint8 num;//��֪�ն�����
   OOP_PERCEPTION_TERMINAL_INFO_T terminalinfo[96];  
   uint16 totalpn;

}OOP_PERCEPTION_TERMINAL_LIST_T;

typedef  struct
{
    uint8 	AFN;                                //Ӧ�ù�����
    uint16 	Fn;                                 //��Ϣ���ʶ
    uint8   ModuleID;                           //ͨ��ģ���ʶ(0:������ / 1:�ز���)
    uint8   DesAddr[6];                         //Ŀ�ĵ�ַ
    uint8   RInfo[6];                           //��Ϣ��
    uint16 	DataUnitLen;                        //Ӧ�����ݳ���
    uint8 	DataUnitBuff[500];                  //Ӧ�����ݻ�����
} AppData_T; 

typedef struct
{
    uint16  Queue_Len;                          //����β
    uint8  Queue_Data[256];               //ͨѶ���ݶ���
}Trans_Queue; 

typedef struct
{
	pthread_mutex_t QueueLock;                  //���ջ�����������
    int16  QueueHead;                          //����ͷ
    int16  QueueTail;                          //����β
    Proxy_Queue Queue[10];               //ͨѶ���ݶ���
}TransLockQueue; 


typedef void *(*EntryFun)(void *);
typedef struct tagApp698Thread
{
    pthread_t          nThreadId;   /* �߳�ID */
    EntryFun           pEntryFunc;  /* �߳���ں��� */
    void              *pArg;        /* ������� */
    uint8             *pRunFlag;    /* �߳����б��, �߳�ִ������Ϊ0 */
}APP698_TRHEAD_T;

#define LOCAL_RECV_FRAM_MAXLEN     16384  //�ն������֡���� �ͻ�������������
#define LOCAL_SEND_FRAM_MAXLEN     2048  //�ն�������֡���� �ͻ������������
#define LOACL_PROC_FRAM_MAXLEN     16384 //�ն������֡����
#define LOCAL_SEND_FRAM_MINLEN     1024  //�ն�������֡���� �ͻ�����С��������
#define RS485_SEND_FRAM_MINLEN     512   //�ն�������֡���� �ͻ�����С��������
#define SMS_FRAM_MAXLEN            140   //���ŷ�֡����
#define RADIO_FRAM_MAXLEN          255   //��̨��֡



typedef struct TimeTagInfo_t_
{
    BOOL   isTimeOut;//�Ƿ�ʱ
    uint8  tagFlag;  //ʱ���ǩֵ
    uint8  tagBuff[10];//ʱ��+���
}TimeTagInfo_t;

typedef struct tagAppConectInfo
{
    uint16                  servSendMaxLen;      // ����������ͳ���
    uint16                  servRecvMaxLen;      // �����������ճ���
    uint8                   servRecvMaxNum;      // ���������ɴ����APDU��Ԫ���ڳߴ�
    uint16                  servMaxApduLen;      // ���������ɴ����APDU��Ԫ

    uint16                  clientSendMaxLen;    // �ͻ�������ͳ���
    uint16                  clientRecvMaxLen;    // �ͻ��������ճ���
    uint8                   clientRecvMaxNum;    // �ͻ������ɴ����APDU��Ԫ���ڳߴ�
    uint16                  clientMaxApduLen;    // �ͻ������ɴ����APDU��Ԫ
    uint8                      SecurityLevel;       // Ӧ������Э�̳�������Ȩ��
    SecurityMode_t          curSecurityMode;     // ��֡����ģʽ
    LinkLayerAddr_t         addrInfo;            // ����֡�ĵ�ַ����
    BOOL                    SecurityServer;         //��֡�Ƿ��ǰ�ȫ���� TRUE:��ȫ�������FALSE:��ͨ����
    TimeTagInfo_t           timeTag;             //ʱ���ǩ��Ϣ
}APP_CPNNECT_INFO_T;

typedef struct{
    uint8  id;//����ID
    uint8  piid;
    uint16 version;      //Э��汾��
    uint8  prtlConf[8];  //Э��һ����
    uint8  funcConf[16]; //����һ����
    uint16 clientSFML;   //�ͻ�������֡����
    uint16 clientRFML;   //�ͻ�������֡����
    uint8  clientWS;     //�ͻ������մ��ڴ�С
    uint16 clientPFML;   //�ͻ������ɴ���֡�ߴ�
    uint32 timeOut;      //Ӧ�����ӳ�ʱʱ��
    uint8  mechains[0];  //Ӧ������������֤�Ļ�����Ϣ
}__attribute__ ((packed)) ConnectRequest_t;


typedef struct{
    uint8   factory[4];  /**< ���̴���           */
    uint8   softVer[4];  /**< ����汾��         */
    uint8   softDate[6]; /**< ����汾����       */
    uint8   hardVer[4];  /**< Ӳ���汾��         */
    uint8   hardDate[6]; /**< Ӳ���汾����       */
    uint8   extend[8];   /**< ������չ��Ϣ       */
}__attribute__ ((packed)) Factory_Version_t;

typedef struct{
    uint8  result;    //Ӧ������������֤�Ľ��
    uint8  data[0];   //��֤������Ϣ
}__attribute__ ((packed)) ConnectResponseInfo_t;

typedef enum AppConnectResult
{
    CONNECT_PERMISSION    = 0, //������Ӧ������
    CONNECT_ERR_PASSWORD  = 1, //�������
    CONNECT_ERR_SYMMETRY  = 2, //�Գƽ��ܴ���
    CONNECT_ERR_NSYMMETRY = 3, //�ǶԳƽ��ܴ���
    CONNECT_ERR_SIGNATURE = 4, //����ǩ������
    CONNECT_ERR_VERSION   = 5, //Э��汾��ƥ��
    CONNECT_ERR_OTHER     = 255, //��������
}CONNECT_RESULT_E;

typedef struct{
    uint8  id;//����ID
    uint8  piid_acd;
    Factory_Version_t factoryVersion;
    uint16 version;      //Э��汾��
    uint8  prtlConf[8];  //Э��һ����
    uint8  funcConf[16]; //����һ����
    uint16 serverSFML;   //����������֡���ߴ�
    uint16 serverRFML;   //����������֡����
    uint8  serverWS;     //���������մ��ڴ�С
    uint16 serverPFML;   //���������ɴ���֡�ߴ�
    uint32 timeOut;      //Ӧ�����ӳ�ʱʱ��
    ConnectResponseInfo_t  response;  //Ӧ�������������֤��Ӧ��Ϣ
}__attribute__ ((packed)) ConnectResponse_t;

/* �������� (curSecurityMode securityServer timeTag ������Ӧ������ ��Ҫɾ�� ������FRAME_HEAD_Tβ�����) */
typedef struct tagAPPContex
{
    uint16 prtlVersion;              //Э��汾��
    uint16 recvFrameLen;             //����֡���� �ն˽�������
    uint16 sendFrameLen;             //����֡���� �ۺ��˿ͻ����ͷ��������� �ô���Ϊ��֡��׼
    uint16 recvApduLen;              //Apdu����
    uint8  prtlConformance[8];       //Э��һ����  �ۺ��˿ͻ����ͷ���������
    uint8  funcConformance[16];      //����һ����  �ۺ��˿ͻ����ͷ���������
    uint8  recvWindowSize;           //���մ��ڴ�С
    uint8  securityMode;             //����ģʽ SECURITY_MODE_E Ӧ������Я��
    uint16 ApplSplitLen;             //Ӧ�÷�֡��С������ֵ����
    uint32 timeOut;                  //��̬��ʱʱ��

    time_t estalishTime;             //Ӧ�����ӽ���ʱ��
    SecurityMode_t curSecurityMode;  // ��֡����ģʽ 
    BOOL securityServer;		     //��֡�Ƿ��ǰ�ȫ���� TRUE:��ȫ�������FALSE:��ͨ����
    TimeTagInfo_t           timeTag; //ʱ���ǩ��Ϣ
}APP_CONTEXT_t;

typedef enum AppSecurityMode
{
    NULL_SECURITY_MODE  = 0, //��������
    PWD_SECURITY_MODE   = 1, //һ������
    SYM_SECURITY_MODE   = 2, //�ԳƼ���
    SIG_SECURITY_MODE   = 3, //����ǩ��
}SECURITY_MODE_E;

#define APP_RECV_BIGMSG_MAXLEN  300000  //�������β2���ֽ� 
#define APP_RECV_MSG_MAXLEN     16388   //�������β2���ֽ� 
#define APP_RECV_IPC_MAXLEN     65024   //����������ˣ�����������
#define APP_SEND_MSG_MAXLEN     16388   //�������β2���ֽ� 
#define APP_645PRTL_MINLEN      12      //645��Լ��С����68(1)+addr(6)+68(1)+C(1)+L(1)+CS(1)+16(1)

#define LINK_NO         0x00    //�޲���
#define LINK_LOGIN      0x01    //��¼
#define LINK_HEART      0x02    //����
#define LINK_QUIT       0x03    //�˳���¼
#define LINK_CHANGE		0x04	//״̬�л�

#define LOGIN_INTERVAL  30      //���Ե�½�ļ��ʱ�䣬10~20s
#define HEART_AHEAD     5       //����ʱ��ﵽ֮ǰ����ǰ����һ������

typedef enum AppLockUser
{
    APP698_NO_LOCK   = 0,
    APP698_PROC_LOCK,
    APP698_RIPC_LOCK,
    APP698_SIPC_LOCK,
    APP698_AUTO_LOCK,
    APP698_DEBUG_LOCK,
}APP_LOCKUSER_E;

/* APDU�ϵ�ɾ���ص����� */
typedef void (*APDUBreakFree)(void **);
/* APDU�ϵ�����ص����� */
typedef void (*APDUBreakZero)(void *);
/* ͨ���ṹ�� */
typedef struct
{
    uint8            id;         //ͨ��id 0~255
    uint8            bOpen;      //ͨ��������� 1-����/0-�ر�
    uint8            linkState;  //Ԥ����״̬ ��¼-����
    uint8            bConnect;   //�Ƿ���Ӧ������
    CHANNEL_INFO_T   tChannel;   //ͨ������
    APP_CONTEXT_t    tContext;   //Ӧ����������
    pthread_mutex_t  fWork;      //�������, �̳߳���

    uint16           leftHB;         //ʣ������ʱ��
    uint8            lostHBCount;    //��ʧ������/��������
    uint8            heartbeatflag;  //������ʱʱ�� ������������

    uint32           startTime;      //��ʼ����ʱ��ʱ�� struct timespec��s
    uint32           endTime;        //��������ʱ��ʱ�� struct timespec��s
    uint8            bNeedClose;     //��Ҫִ�йرղ���, ͨ�������ǹر�ʧ�ܣ���Ҫ���ô˱��
    uint8            workFlag;       //�������
    uint8            longFlag;       //�Ƿ�ʱ�䴦����

    uint8            bmsgExtLen;     //��չ���ȵ���Ϣ��ǩ
    uint8            bmsgExtOver;    //��չ���ȵĽ������
    uint8            bMsgExtProc;    //������
    uint16           lostHBtime;     //����������ʧʱ��
    uint32           recvExtLen;     //�յ��ĳ���
    uint32           extMsgLen;      //��������Ϣ���� ������β
    uint8           *extMsg;         //������Ϣ����

    uint32           extOutMsglen;   //���͵���չ������Ϣ����
    uint8            extIndex;       //��Ϣ����
    uint8            extSendFlag;    //���ͳɹ����
    uint8            res1[1];

    uint8            bSafe;          //���н����Ƿ�ȫģʽ
    pthread_mutex_t  fRecv;          //���ջ�����
    uint8            recvLockUser;   //ͨ���û�   APP_LOCKUSER_E
    uint8            msgnum;         //���汨����
    uint16           totalLen;       //���նӻ�����Ϣ�ܳ���
    uint8            recv[APP_RECV_IPC_MAXLEN];  //����IPC����
    time_t           checkTimeOK;    //���ջ�����ɹ��ж�ʱ�䳬��10s���ɹ���Ҫ���õ�һ���ֽ�Ϊ0

    uint16           procLen;        //������Ϣ����
    uint16           blocknum;       //��֡���
    uint8            ucMess[APP_RECV_MSG_MAXLEN]; //��װ��������������

    pthread_mutex_t  fSend;           //���ͻ�����
    uint8            sendLockUser;    //ͨ���û�  APP_LOCKUSER_E
    uint8            res2[1];
    uint16           sendMessLen;     //������Ϣ����
    uint16           curblock;        //��ǰ������֡���
    uint8            permSend;        //�����ͱ��
    uint8            protocol;        //˽��Э��-����
    uint8            sendMess[APP_SEND_MSG_MAXLEN]; //���ͻ��� �ɴ���APDU�ֶη���

    DB_CLIENT        clientid;        //�������ľ��

    uint8            bSendApduBreak;  //����Ӧ�÷�֡��� 1-���÷�֡��0-����֡(������֡)
    uint8            res3[3];
    uint32           BreakMsgLen;
    uint8           *pBreakBakMsg;    //Ӧ�÷�֡������Ϣ
    void            *pApduBreak;      //APDU�ϵ�
    APDUBreakFree    pFreeFunc;       //�ϵ�free�ص�����

    uint8            private[12];      //˽������, ͨ����ά��
                                      //0-�߼��豸 1-pApdu[1] 2-�Ƿ���Ҫ��¼����¼���� 
                                      //3-�Ƿ�ֻ�����ݲ����� 4-����ģ���������� 
                                      //5-�Ƿ�ʱΪ�ϱ���֯�����ⱨ��[1��, 0��]
                                      //6-�ϱ�����ֵ��ɽ�����У�
                                      //7-�ϱ����ڵ�λ��ɽ�����У�
                                      //8-��ʾ�Ƿ����ϱ������Ĺ�Լ�ٲ⣻1����ʾ���ϱ��ٲⴥ���ģ�0��ʾ�������Ĺ�Լ�ٲ�
                                      //9-�µ�������Ϣ�Ƿ�Ӧ�÷�֡ 1-�� 0-��
                                      //10 1��ʾ���鰲ȫģʽ ��߷����������ݵ�ʱ��
}CHANNEL_CACHE_T;

#define SERVER_ADDR_MAXLEN         16
#define RTU_ADDRESS_SINGLE           0            //����ַ-
#define RTU_ADDRESS_WILD             1             //ͨ���ַ-
#define RTU_ADDRESS_TEAM             2             //���ַ-
#define RTU_ADDRESS_BROADCAST        3             //�㲥��ַ-
typedef struct tagServerCLIENTAddr
{
    uint8   len;       //����˵�ַ����
    uint8   logicAddr; //�߼���ַ��չ 0-255
    uint8   type;      //0-���� 1-ͨ�� 2-�鲥 3-�㲥
    uint8   res[1];
    uint8   addr[SERVER_ADDR_MAXLEN];
}SERVER_ADDR_T;

#define  LINK_SEPARATE_MAX_BLOCK    4095
#define  LINK_SEPARATE_START        0
#define  LINK_SEPARATE_END          1
#define  LINK_SEPARATE_ACK          2
#define  LINK_SEPARATE_MIDDLE       3
typedef struct tagLinkSeparate
{
    uint16     block;  //���
    uint8      type;   //��֡����
    uint8      res;
}LINK_SEPARATE_T;

#define MESSAGE_PRM_FROM_SERVER   0
#define MESSAGE_PRM_FROM_CLIENT   1
#define MESSAGE_DIR_FROM_CLIENT   0
#define MESSAGE_DIR_FROM_SERVER   1
typedef struct tagFrameHead
{
    uint32          msglen;  //֧�ֳ�����չ��Э�� ��������ʼ���� �ַ�
    uint8           bExtmsg; //�Ƿ񳤶���չ �ñ�����ú� ��Ҫʹ��

    uint8           headlen; //֡ͷ���� ��������ʼ�ַ� ����HCSУ��
    uint8           funCode; //������
    uint8           sc;      //����
    uint8           separate;//��֡��־
    uint8           prm;     //������־
    uint8           dir;     //����
    uint8           cAddr;   //�ͻ�����ַ
    SERVER_ADDR_T   sAddr;   //��������ַ
    
    /* ��ȫ������Ϣ, ��֯���Ĳ���, ��֯apdu�������� */
    union{
        struct
        {
            uint8   verifyType:3; //0-������֤�� 1-����� 2-�����+MAC 3-��ȫ��ʶ
            uint8   dataType  :1; //��ַ���� 0-���� 1-����
            uint8   res       :2; //����
            uint8   bProxy    :1; //1-���� 
            uint8   bSecurity :1; //1-��ȫ����
        };
        uint8       security; //��ȫ����ʹ��
    };
    uint8           NoEsamMac;//������esam��mac����ȷ��
    uint8           res1[2];
    uint32          apduLen;   //������չ
    uint8          *pApdu;
    LINK_SEPARATE_T tSepara;   //��·��֡����
}FRAME_HEAD_T;

typedef int (*DBUpdateFun)(int, uint8, uint16, uint32);

typedef enum AppErrCode
{
    APP698_SECCESS      = 0,
    APP698_ERR_BUSY,
    APP698_ERR_INPUT,
    APP698_ERR_NOT_OPEN,
    APP698_ERR_NOT_CONNECT,
    APP698_ERR_ID_NOTEXIST,
    APP698_ERR_MSG_HEAD_TAIL,
    APP698_ERR_HCS,
    APP698_ERR_FCS,
    APP698_ERR_CACHE_NOT_ENOUGH,
    APP698_ERR_MSG_TOO_LONG,
    APP698_ERR_IPC_RECONNECT,
    APP698_ERR_SEND_ERRORMSG,
    APP698_ERR_ADDR_NOTMATCH,
    APP698_ERR_NO_PROC_MESS,
    APP698_ERR_ADDR_BROADCAST,
    APP698_ERR_NOT_EXT_MSG,
    APP698_ERR_EXT_MSG_FIRST,
    APP698_ERR_EXT_MSG_OVER,
}APP_ERRORCODE_E;

/* ���ͨ���Ƿ�û��׼���� */
#define CHANNEL_CHECK_NOTOK(pchannel)  \
    (NULL == (pchannel) || FALSE == (pchannel)->bOpen || FALSE == (pchannel)->bConnect)

/* ��ȡ���ͽ��նԴ�����֡���� */
#define CHANNEL_GET_BREAK_FRAME_SIZE(pchannel)  \
    (0 == (pchannel)->tContext.sendFrameLen) ? LOCAL_SEND_FRAM_MINLEN : MIN((pchannel)->tContext.sendFrameLen, LOCAL_SEND_FRAM_MAXLEN)

/* �ж��Ƿ������� */
#define CHANNEL_NOT_NEED_SEND(pchannel) (0 == (pchannel)->permSend || 0 == (pchannel)->sendMessLen)

#define CAHNNEL_LOCK_RECV(pchannel, lockuser)   \
    {\
        pthread_mutex_lock(&((pchannel)->fRecv));\
        (pchannel)->sendLockUser = lockuser;\
    }

#define CAHNNEL_UNLOCK_RECV(pchannel)  \
    {\
        pthread_mutex_unlock(&((pchannel)->fRecv));\
        (pchannel)->sendLockUser = APP698_NO_LOCK;\
    }

/* ��մ����Ķ��� */
#define CHANNEL_EMPTY_PROC_MESS(pchannel)   \
    do{\
        if(pchannel->procLen > 0)\
        {\
            memset(pchannel->ucMess, 0, APP_RECV_MSG_MAXLEN);\
            pchannel->procLen = 0;\
        }\
        pchannel->blocknum = 0;\
    }while(0);


#define CAHNNEL_LOCK_SEND(pchannel, lockuser)   \
    {\
        pthread_mutex_lock(&((pchannel)->fSend));\
        (pchannel)->sendLockUser = lockuser;\
    }

#define CAHNNEL_UNLOCK_SEND(pchannel)  \
    {\
        pthread_mutex_unlock(&((pchannel)->fSend));\
        (pchannel)->sendLockUser = APP698_NO_LOCK;\
    }

#define CHANNEL_EMPTY_SEND_MESS_NOLOCK(pchannel) \
    do{\
        memset(pchannel->sendMess, 0, APP_SEND_MSG_MAXLEN);\
        pchannel->sendMessLen = 0;\
        pchannel->curblock = 0;\
        pchannel->permSend = 0;\
        pchannel->protocol = 0;\
    }while(0);

/* ��д���ͻ����� */
#define CHANNEL_PUT_SEND_MESS_NOLOCK(pchannel, m, l, p) \
    do{\
        memcpy(pchannel->sendMess, m, l);\
        pchannel->sendMessLen = l;\
        pchannel->curblock = 0;\
        pchannel->permSend = 1;\
        pchannel->protocol = p;\
    }while(0);

/* ���÷��ͻ��������� */
#define CHANNEL_SET_SEND_MESS_NOLOCK(pchannel, l, s, p) \
    do{\
        pchannel->sendMessLen = l;\
        pchannel->curblock = 0;\
        pchannel->permSend = s;\
        pchannel->protocol = p;\
    }while(0);

/* ��ȡ��̬���� */
#define CHANNEL_GET_HEART(pchannel)   \
    ((pchannel->tChannel.heart < CHANNEL_MIN_HEART) ? CHANNEL_MIN_HEART : pchannel->tChannel.heart)

/* ��ȡӦ�÷�֡��С */
#define APDU_SPLIT_DEFAULT_MAX           app_apdu_default_len()

/* ����֡ͷ���� */
#define APUD_COUNT_FRAME_LEN(alen, logica) (APP_698_HEAD_BASE_SIZE + alen + (logica <= 1 ? 0 : 1))

void app_get_initlock();
void app_set_localaddr(uint8 len, uint8 *pAddr);
uint8 app_get_localaddr(uint8 **ppAddr);
void app_set_clientNO(uint8 len, uint8 *pAddr);
uint8 app_get_clientNO(uint8 **ppAddr);
void app_set_groupaddr(OOP_GRPADDR_T *pAddr);
void app_get_groupaddr(OOP_GRPADDR_T *pAddr);
void app_set_password(OOP_VISIBLEVAR32_T *password);
uint8 app_get_password(uint8 **ppPwd);
BOOLEAN app_check_password(uint8 *pPwd, uint8 len);
void app_set_context(OOP_ASSOCIATION_CONTEXT_T *pContext);
void *app_get_context();
void app_set_version(Factory_Version_t *pVersion);
void *app_get_version();

void app_init_channel(uint32 ip, uint16 port);
void app_set_preconnect(uint8 id, uint8 linkState, uint16 heartbeat);
void app_open_channel(uint8 id, CHANNEL_INFO_T *pChannelInfo);
void app_set_channel(uint8 id, CHANNEL_INFO_T *pChannelInfo);
void app_close_channel(uint8 id);
void app_unlock_channel(uint8 id, uint8 lockuser);
int app_set_channelworklock(uint8 id, uint8 bwork);
void app_update_leftHB(uint8 id, uint16 heartbeat);
uint16 app_get_leftHB(uint8 id);
void app_set_connect(uint8 id, uint8 bConncet, APP_CONTEXT_t *pAppContext);
BOOLEAN app_get_connect(uint8 id, APP_CONTEXT_t *pAppContext);
CHANNEL_CACHE_T *app_get_channel(uint8 id);
int app_externmsg_reproc(uint8 id, uint8 *pMsg, uint16 msgLen, uint8 lockuser);
int app_insert_recvmsg(uint8 id, uint8 *pMsg, uint16 msgLen, uint8 lockuser);
uint16 app_get_recvmsg(uint8 id, uint8 *pbuf, uint16 bufLen, uint8 lockuser);
uint16 app_get_645recv(uint8 id, uint8 *pbuf, uint16 bufLen, uint8 lockuser);
uint16 app_get_recvmsg_s(uint8 id, uint8 *pbuf, uint16 bufLen, uint8 lockuser);
void app_remove_firstrecvmsg(uint8 id, uint8 lockuser);
BOOLEAN app_cut_message(uint8 *pMsg, uint16 msgLen, uint16 *offset, uint16 *newlen);
int app_check_message(uint8 *pMsg, uint32 msgLen);
int app_check_message_head(uint8 *pMsg, uint32 msgLen);
int app_get_frame(uint8 *pMsg, uint32 msgLen, FRAME_HEAD_T *pFrame, BOOLEAN bSC);
uint16 app_frame_to_package(FRAME_HEAD_T *pFrame, uint8 *pBuf, uint16 bufLen, BOOLEAN bSC);
int app_set_sendblock(CHANNEL_CACHE_T *ptChannel, uint16 blocknum);
BOOLEAN app_bluetooth_mac(uint8 id, uint8 *pMsg, uint16 *pLen);
int app_bluetooth_mac_reply(uint8 id);
int app_insert_procmesstail(CHANNEL_CACHE_T *ptChannel, uint8 *pBuf, uint16 bufLen, uint16 blockid);
void app_update_procmess_frame(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *pFrame);
uint16 app_linkrequest_package(uint8 type, uint16 beatPeriod, uint8 *pBuf, uint16 bufLen);
void app_connect_init(uint8 id);
void rpt_proxyresponse_data();
uint16 app_apdu_default_len();
uint8 app_get_opencha_num();
uint8 app_get_opencha_id(uint8 no);
void app_log_channel_recvbuf(uint8 id, uint8 lockuser);
void app_channel_proctime(uint8 id, uint8 bStart);
uint32 app_frame_to_bigpackage(FRAME_HEAD_T *pFrame, uint8 **ppBigMsg, uint32 tarMsgLen, uint32 *pApduLeft, 
                               BOOLEAN bSC);
uint32 app_frame_to_adaptpackage(FRAME_HEAD_T *pFrame, uint8 *pBuf, uint32 bufLen, BOOLEAN bSC);
void app_quitlogin_proc();


int32 ProxyWriteQueue(ProxyLockQueue *ProxyQueue, uint8 *buf, int32 buf_len);
extern int send_emergency_ipcmsg(uint8 id, uint8 protocol, uint8 *pMsg, uint16 msgLen);
extern int send_bigpackage_master(uint8 id, uint8 *pMsg, uint32 msgLen);
extern int protocol_698_addpool(uint8 channel);
extern void *app_apdubreak_init();
extern void app_apdubreak_free(void **pBreak);
extern void app_apdubreak_zero(void *pBreak);
extern int appmain_send_msg(uint32 msgid, uint16 label, uint8 *pMsg, uint16 msglen);
extern int appmain_send_syncmsg(uint32 msgid, uint8 *sendbuf, uint16 sendlen, uint8 *recvbuf, uint16 buflen, uint16 *recvLen);
extern int appmain_send_syncmsg_ccon(uint32 msgid, uint8 *sendbuf, uint16 sendlen, uint8 *recvbuf, uint16 buflen, uint16 *recvLen);
extern int appmain_send_syncmsg_time(uint32 msgid, uint8 *sendbuf, uint16 sendlen, uint8 *recvbuf, uint16 buflen, uint16 *recvLen, uint8 outtime);
extern int reboot_sent_to_smiOS(void);
extern int param_init_sent_to_acmeter(void);
extern int data_init_sent_to_acmeter(void);
extern int demand_init_sent_to_acmeter(void);
extern int system_cmd_sent_to_acmeter(const char* cmd);
extern int rate_set_sent_to_acmeter(uint8 time);
extern uint16 appmain_get_ppp_mode(uint8 no, uint8* recvbuf, uint16 buflen);
extern uint16 appmain_get_ppp_diag(uint8 no, uint8* recvbuf, uint16 buflen);
extern uint16 appmain_get_ppp_gps(uint8 no, uint8* recvbuf, uint16 buflen);
extern uint16 appmain_trans_AT(uint8 no, char *sendAT, uint8* recvbuf, uint16 buflen);
void appmain_set_gps_tempposition(OOP_GEOGADD_A_T *ptGps);
int appmain_get_gps_tempposition(OOP_GEOGADD_A_T *ptGps);
extern uint16 appmain_get_map_slot(const char *type, uint8* recvbuf, uint16 buflen);
extern uint16 appmain_get_map_info(uint32 no, uint8* recvbuf, uint16 buflen);
extern uint16 appmain_get_map_all(uint8* recvbuf, uint16 buflen);
extern uint16 appmain_get_deviceinfo(OOP_DEVICERUNSTAT_T *deviceinfo);
extern int appmain_manager_app(uint8 type, char *appname);
extern int appmain_act_ble_checkmeter(OOPM_BLUETOOTHERRCHK_T *check);
extern int appmain_get_bluetooth_cfg(uint8* recvbuf, uint16 buflen);
extern int appmain_get_bluetooth_sn(uint8* recvbuf, uint16 buflen);
extern int appmain_get_bluetooth_connect(uint8* recvbuf, uint16 buflen);
extern int appmain_set_bluetooth_cfg(uint8 *sendbuf, uint16 sendlen, uint8* recvbuf, uint16 buflen, uint16 *recvlen);
extern int appmain_set_bluetooth_sn(uint8 *sendbuf, uint16 sendlen, uint8* recvbuf, uint16 buflen, uint16 *recvlen);
extern int appmain_set_bluetooth_check(uint8 *sendbuf, uint16 sendlen);
extern int appmain_set_flow_param(uint32 monLimit, uint32 flowLimit, uint32 time);
extern int appmain_get_flow_param(uint32 *monLimit, uint32 *flowLimit, uint32 *time);
extern int appmain_send_proxy_to_taskManager(uint8 *sendbuf, uint16 sendlen, uint8* recvbuf, uint16 buflen, 
    uint16 *recvlen);
extern int ipconfig_send_to_smiOS(char* Name, uint8 type, uint8* IP, uint8* NetMask, uint8* NetGw);
extern int seek_meter_to_taskManager(uint16 time);
extern int trans_buf_to_taskManager(uint8* buf, uint16 len, uint32 port, uint8 baud, uint32 overtime, uint8 *recvbuf, uint32 recLenMax, uint16 *recLen);
extern int trans_buf_send_to_taskManager(uint16 label, uint8* buf, uint16 len, uint32 port, uint8 baud, uint32 overtime);
extern int appmain_refresh_status_bar(uint32 time, char *str);
extern	void ProxyRequest_ClientApdu(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, uint8 *pBreakFlag);
extern	int Proc_Proxy(uint16* label,ProxyHeadInfo_t *ProxyHead );

void app_comm_proxy_set(BOOLEAN isUse);
BOOLEAN app_comm_proxy_isuse();
void app_secure_defend_set(BOOLEAN isUse);
BOOLEAN app_secure_defend_isuse();
void proxy_apply_ack_set(uint8 id);
BOOLEAN proxy_apply_ack_get(uint8 id);
void proxy_apply_retry_sub(uint8 id);
uint8 proxy_apply_retry_get(uint8 id);
void proxy_coninfo_notify(uint8 id);

int32 ProxyReadQueue(ProxyLockQueue *ProxyQueue, uint8 *buf);
BOOLEAN ProxyCheckQueue(ProxyLockQueue *ProxyQueue);
int8 UnPackRouterFrameData(uint16 FrameLen, uint8 *FrameBuff, AppData_T *AppData);

extern uint16 channel_get_break_frame_size(CHANNEL_CACHE_T  *ptChannel);

#ifdef __cplusplus
}
#endif

#endif


