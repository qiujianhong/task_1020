/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��·���ҵ��
* @date��    2020-6-30
* @history�� 
* @User deatil:

*********************************************************************
*/
#ifndef __SEC_SERVICE_H
#define __SEC_SERVICE_H
    
#ifdef __cplusplus
        extern "C" {
#endif

#define KEY_MAX_FILE_NUM    128 //�ؼ�Ŀ¼ɨ�������ļ�����-->libĿ¼��ǰ�ļ�����91->����70->128
#define KEY_MAX_FILE_LEN    30  //�ؼ�Ŀ¼ɨ����ļ�����󳤶�
#define KEY_FILE_ATTR_LEN   36  //�ؼ�Ŀ¼ɨ����ļ�������󳤶�

#define STATION_COMM_PARA_CNT   4                   //��վͨ�Ų�������,����eth  ������GPRS����
#define TMN_SECURITY_DETECT     "security_manager_record"
#define TcpRemoteSrvPortLen     sizeof(SEC_PORTOPEN_T)      //����¼10����̫��Զ�̷���˿�,�ϱ����(1) + �˿�(2)
#define TcpExtConnInfoLen       sizeof(SEC_CONNECT_T)     //����¼10����̫����������Ϣ,�ϱ����(1) +(IP + ":"+PORT  30),    192.168.10.146:2145
#define UsbDevAccessInfoLen     2                   //USB�豸������Ϣ,      �ϱ����(1)+ ��Ϣ(1)
#define LocalMaintenanceInfoLen 2+2+2               //����ά���˿���Ϣ,�ϱ����(1)+ ��Ϣ(1)(232 485 �������ʹ�ø��Եı��)
#define KeyDirChangeInfoLen     sizeof(KEY_DIR_FILE_VECTOR)     //�ؼ�Ŀ¼�����Ϣ,�ϱ����(1)+ Ŀ¼·��(50)
#define TcpLoginTtyInfoLen      4                   //��̫����¼����̨��Ϣ,  ����
#define SerialLoginTtyInfoLen   4                   //���ڵ�¼����̨��Ϣ,  ����
#define FlowLimitParamLen       sizeof(SEC_SESSION_LIMIT_T)            //�������޲��� ���ۼ�������ֵ(4) ������ֵ(4)
#define SverLimitParamLen       sizeof(SEC_SRVTIME_LIMIT_T)            //����ʱ��δ�رղ���

#define RecordSartAddr              0
#define TcpRemoteSrvPortAddr        RecordSartAddr                                      //��̫��Զ�̷���˿���ʼ��ַ
#define TcpExtConnInfoAddr          TcpRemoteSrvPortAddr+TcpRemoteSrvPortLen            //��̫����������Ϣ��ʼ��ַ
#define KeyDirChangeInfoAddr        TcpExtConnInfoAddr+TcpExtConnInfoLen                //�ؼ�Ŀ¼�����Ϣ��ʼ��ַ
#define TcpLoginTtyInfoAddr         KeyDirChangeInfoAddr+KeyDirChangeInfoLen            //��̫����¼����̨��Ϣ��ʼ��ַ
#define SerialLoginTtyInfoAddr      TcpLoginTtyInfoAddr+TcpLoginTtyInfoLen              //���ڵ�¼����̨��Ϣ��ʼ��ַ
#define FlowLimitParamAddr          SerialLoginTtyInfoAddr+SerialLoginTtyInfoLen        //������ֵ
#define SverLimitParamAddr          FlowLimitParamAddr+FlowLimitParamLen                //����ʱ��δ�رղ���

#define SEC_DEFAULT_TIME    (0)  //Ĭ���ж�ʱ��
#define SEC_DEFAULT_FLOW    (0)  //Ĭ������
#define SEC_DEFAULT_BAND    (0)  //Ĭ�ϴ���

typedef struct
{
    char ipAddr[20];       /*IP��ַ������TCP/IP�ͻ���ͨ��*/
    uint16 port;                /*�˿ں�*/
}LINK_ADDR;

typedef struct
{
    char    state[20];
    char    local_ip[20];
    char    foreign_ip[20];
    uint16  local_port;
    uint16  foreign_port;
}POINT_INFO;

//�û���¼����
typedef enum
{
    LOGIN_ETH = 0, //��̫��
    LOGIN_UART,     //����
}SEC_LOGINTYPE_E;

//�û���¼��Ϣ�ṹ��
typedef struct
{
    char    user[20];     //��¼���û���
    char    tty[20];      //��¼���ն�
    char    time[20];     //��¼��ʱ��
    char    host[32];     //��¼������
}SEC_LOGININFO_T;

typedef struct
{
    char    usrname[64];  //ʧ�ܵ��û���
    char    loginip[20];  //��¼ʧ�ܵ�IP
    int     logincnt;     //��¼����
    time_t  failtime;     //ʧ�ܼ�¼ʱ��
}SEC_FAILLOGIN_T;

typedef struct
{
    uint8 status;
    uint32 conTime; //SSH�������ʱ�� ��λ����
}SEC_SSHINFO_T;

/* ���ڷ���״̬ */
typedef enum tag_SEC_WORKSTATE_E
{
    SEC_CONF = 0,  //����״̬
    SEC_PARA = 1,  //����״̬ 
    SEC_WORK = 2,  //����״̬ 
}SEC_WORKSTATE_E;

typedef struct
{
    char    local_ip[20];
    char    foreign_ip[20];
    uint16  local_port;
    uint16  foreign_port;
    char    protocol[4];   //Э������
    char    process[32];   //��������
    uint8   procType;      //�������� @ref SEC_PROCTYPE_E
    uint32  monthFlow;     //���ۻ����� KB
    uint32  flowSpeed;     //���� Kbp/s
    uint32  calcFlow;      //���ڼ������ٵ��ۻ�����
    uint8   calcUpdate;    //�Ƿ��ڼ������٣���������ʱץ���̲߳�ˢ��calcFlow    
    uint8   monUpdate;     //�Ƿ����������������ʱץ���̲߳�ˢ��monthFlow
    int     updateTime;    //�����һ��ˢ��ʱ��
}SEC_SESSIONAPP_T;

typedef struct
{
    uint8 nNum;
    SEC_SESSIONAPP_T info[SEC_RECORD_MAX];
}SEC_SESSIONAPPS_T;

typedef struct
{
    uint8 nNum;
    int   procinfo[65535];
}SEC_PROCINFOS_T;

/* ���ڷ����߳����� */
typedef struct tag_SEC_MGRDATA_T
{
    uint8          status;    //״̬
    uint8          res[2];
    uint8          port;
    DB_CLIENT      clientid;
    int            tasksocket;
    time_t         tasktimeout; //����ʱʱ��
    uint8          tcpInfoNum;  //ɨ���TCP��Ϣ
    POINT_INFO     tcpInfo[SEC_RECORD_MAX];    
    uint8          connWhiteListNum; //TCP������Ϣ������
    LINK_ADDR      connWhiteList[SEC_RECORD_MAX];
    time_t         curtime;       //��ǰʱ��    
    time_t         lsttime;       //�ϴ�ʱ��
}SEC_MGRDATA_T;


typedef struct
{
    uint16  mode;                   /*�ļ����ͺ�Ȩ��*/
    time_t  mtime;                  /*�ļ��޸�ʱ��*/
    char    name[KEY_MAX_FILE_LEN]; /*�ļ���*/
}__attribute__ ((packed)) FILE_ATTR;//�ļ�����

typedef struct
{
    uint8        filenum;   /*ɨ�赽���ļ�����*/
    FILE_ATTR    file_vector[KEY_MAX_FILE_NUM];
}__attribute__ ((packed)) KEY_DIR_FILE_VECTOR;


/* ��ں������� */
typedef int (*SEC_AXDRMSG_FUN)(MESSAGE_INFO_T*, SEC_APPDATA_T*, SEC_MGRDATA_T*);

typedef struct tag_SEC_AXDR_FUN_T
{
    uint32               IOP;      /** < ��ʶ */
    SEC_AXDRMSG_FUN     pProcMsg; /** < ������ */
}SEC_AXDR_FUN_T;

/* ��ں������� */
typedef int (*SEC_JSONMSG_FUN)(MESSAGE_JSON_T*, SEC_APPDATA_T*, SEC_MGRDATA_T*);

typedef struct tag_SEC_JSON_FUN_T
{
    char               *infopath; /** < ��ʶ */
    SEC_JSONMSG_FUN    pProcMsg; /** < ������ */
}SEC_JSON_FUN_T;

int sec_manager_init(void *pPublic, void **ppPrivate);
int sec_manager_proc(void *pArg, void *pPublic, void *pPrivate);
int sec_session_update(void);

#ifdef __cplusplus
}
#endif

#endif /* __SEC_SERVICE_H */


