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
#ifndef	__LC_MOD_H
#define __LC_MOD_H
    
#ifdef __cplusplus
        extern "C" {
#endif

/* G-����ģ��; M-ģ�����ɼ�ģ�� X-���������޲������� 
   ������Դ����������ģ�鼼���淶��1 ģ���������ͱ�ʶ����*/
#define MOD_LC_TYPE_PRFIX       "GRX"   

/*����ģ����豸������ǰ׺ ������=uart_acm[ģ�����/��λ��]_[����ͨ����]
  ��HAL�ṩ��LIB���壬�μ�framework/include/hal/uart.h*/
#define MOD_DEV_DESC_PRFIX        "uart_acm"

#define MOD_SAMPLE_INTERV  (5)  //ģ����Ϣ��ȡ���
#define MOD_SAMPLE_PERIOD  (60) //ģ��״̬��������
#define MOD_SAMPLE_LIMITS ((MOD_SAMPLE_PERIOD/MOD_SAMPLE_INTERV)/2) //������ģ��״̬��ֵ

#define LCM_DI_WORKSTATUS       0x07000001  //������������Ϣ
#define LCM_DI_CTVERSION        0x07000002  //�������汾��Ϣ
#define LCM_DI_COREVERSION      0x07000003  //�㷨��汾��Ϣ
#define LCM_DI_FILETRANSMIT     0x0F000001  //�������ļ�����

/** @brief ģ���·״̬ö��   */
typedef enum tag_LC_MOD_STATUS
{
    LC_MOD_STATUS_NR  = 0, //����
    LC_MOD_STATUS_SC =  1, //��·
    LC_MOD_STATUS_OC  = 2, //��·
    LC_MOD_STATUS_SE =  6, //���Ӱ뵼��    
    LC_MOD_STATUS_ED,
}LC_MOD_STATUS_E;

/** @brief �¼���·״̬ö��   */
typedef enum tag_LC_EVT_STATUS
{
    LC_EVT_STATUS_NR  = 0, //����
    LC_EVT_STATUS_SC =  4, //��·
    LC_EVT_STATUS_OC  = 6, //��·
    LC_EVT_STATUS_SE =  1, //���Ӱ뵼��
}LC_EVT_STATUS_E;

/** @brief ��Լ��·״̬ö��   */
typedef enum tag_LC_OOP_STATUS
{
    LC_OOP_STATUS_NR  = 0, //����
    LC_OOP_STATUS_SC =  1, //��·
    LC_OOP_STATUS_OC  = 2, //��·
    LC_OOP_STATUS_SE =  3, //���Ӱ뵼��
}LC_OOP_STATUS_E;

/*ģ�鹤����Ϣ�ṹ��*/
typedef struct taglcWorkStatus
{
    uint8  loopStatus[3];        //������·״̬
    uint32 envTemp[3];           //�����¶�
    uint32 freqMax[3];           //Ƶ�����ֵ
    uint32 freqMin[3];           //Ƶ����Сֵ
    uint32 curValue[3];          //��Ƶ������Чֵ
    uint32 impeFreq1[3];         //��һ���迹Ƶ��
    uint32 impeValue1[3];        //��һ���迹ģֵ
    uint32 impeAnge1[3];         //��һ���迹�Ƕ�
    uint32 impeFreq2[3];         //�ڶ����迹Ƶ��
    uint32 impeValue2[3];        //�ڶ����迹ģֵ
    uint32 impeAnge2[3];         //�ڶ����迹�Ƕ�
    uint32 impeFreq3[3];         //�������迹Ƶ��
    uint32 impeValue3[3];        //�������迹ģֵ
    uint32 impeAnge3[3];         //�������迹�Ƕ�                   
}LC_WORK_STATUS_T;

/*ģ�鹤����Ϣͳ�ƽṹ��*/
typedef struct taglcStatusStat
{
    uint32 count[3]; //3�����
    LC_WORK_STATUS_T status;
}LC_STATUS_STAT_T;

/** @brief ��·Ѳ���ȡ״̬   */
typedef enum tag_LC_MONITOR_COL
{
    LCM_INIT  = 0, //��ʼ̬
    LCM_SEND =  1, //����̬
    LCM_OPEN  = 2, //��̬
    LCM_RECV =  3, //����̬
    LCM_FUPG  = 4, //����̬
    LCM_TRAN  = 5, //͸��̬
    LCM_GETM  = 6, //��ȡ̬    
    LCM_IDLE  = 7, //����̬
}LC_MONITOR_COL_E;

#define LCM_SEND_RETRY (0)
#define LCM_RECV_RETRY ((MOD_SAMPLE_PERIOD/MOD_SAMPLE_INTERV)*(60.0/MOD_SAMPLE_PERIOD))

#define VF_BEGIN       0x68  //֡��ʼ�ַ�-
#define VF_END         0x16  //֡�����ַ�-

#define DL645_ADDR                 1                //��ַ��
#define DL645_CTRL                 8                //������
#define DL645_LEGH                 9                //������
#define DL645_DATA                 10               //������
    
#define DL645_AFN_CTEXT            0x03     //��չ�����룺����������
#define DL645_FRAME_MIN      12   //������С���ȣ�68(1)+addr(6)+68(1)+C(1)+L(1)+cs(1)+16(1)��
#define DL645_DATA_MAX       256  //��������󳤶�

typedef int (*MODGET)(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata);

typedef struct tag_LC_MODPARSE
{
    uint32 di;
    MODGET apduGet;
}LC_MODPARSE_T;

/*���ڲ���*/
typedef struct tag_LC_UART_PARAM_T
{
    uint32        baudRate;      /** < ������ */ 
    uint8         dataBits;      /** < ����λ */
    uint8         stopBits;      /** < ֹͣλ */
    uint8         parity;        /** < У��λ */    
    uint8         flowCtrl;      /** < ����λ */    
}LC_UART_PARAM_T;

/*���ڲ������*/
typedef struct tag_LC_UART_VDEV_T
{
    char             halname[32];   /** < HAL�豸������   */    
    char            *desc;          /** < ��ʵ�豸������   */    
    LC_UART_PARAM_T  param;         /** < ���ڲ���   */
    uint16           frametimeout;  /** < ֡��ʱ   */
    uint16           bytetimeout;   /** < �ֽڳ�ʱ   */    
    int              fd;            /** < ���ھ��(Ҫ��ʼ��Ϊ-1�����ڴ򿪺�����) */
    uint32           sendCnt;       /** < ���ڷ��ͼ��� */
    uint32           recvCnt;       /** < ���ڽ��ռ��� */ 
    struct timespec  sendTm;        /** < ���ڷ���ʱ�� */
    struct timespec  recvTm;        /** < ���ڽ���ʱ�� */  
}LC_UART_VDEV_T;

#define DL645_ADDR_LEN       6    //��ַ�򳤶�

typedef union DL645_CMD
{
    struct
    {
        unsigned char   afn:5;  //������   
        unsigned char   next:1; //����֡��� 0-�޺���֡ 1-�к���֡
        unsigned char   res:1;  //��վӦ������ 0-��վ��ȷӦ�� 1-��վ���쳣��Ϣ��Ӧ��
        unsigned char   dir:1;  //���䷽�� 0-����վ����������֡ 1-�ɴ�վ������Ӧ��֡
    };
    unsigned char value;
} DL645_CMD_U; //-�������ֽ�-

//֡��Ϣ
typedef struct
{
    uint8 *pData;     //������    
    uint8 lengh;      //������
    DL645_CMD_U ctrl; //������
    uint8 addr[DL645_ADDR_LEN];    //��ַ��
}FRAME_INFO_T;

void lc_monitor_collect(void *pPublic, void *pPrivate);
void lc_monitor_deal(void *pPublic, void *pPrivate);
void lc_workstate_analyze(DB_CLIENT clientid, int sockfd);
int lc_version_get(void *pDev, DB_CLIENT clientid, int socketfd);
int lc_workstate_get(void *pDev, DB_CLIENT clientid, int socketfd);
int lcm_version_get(void *pDev, DB_CLIENT clientid, int socketfd);
int lcm_workstate_get(void *pDev, DB_CLIENT clientid, int socketfd);
void lc_monitor_read(void *pDev, uint16 dt, DB_CLIENT clientid, int socketfd);
int uart_open(LC_UART_VDEV_T *pUart);

int lc_get_map_slot(int sockfd, const char *type);

void lc_mod_restart_send(uint32 slot, uint32 time);
void lc_uart_mode_check(void *pPrivate);

int lc_ctworkstate_get(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata);
int lc_versioninfo_get(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata);
int lcm_ctworkstate_parse(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata);
int lcm_ctversion_parse(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata);
int lcm_coreversion_parse(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata);
int lcm_filetrans_parse(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata);

#ifdef __cplusplus
}
#endif

#endif


