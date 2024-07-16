/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2019-10-25
*********************************************************************
*/


#ifndef __CCO_REDEF_H__
#define __CCO_REDEF_H__

#include "CcoAll.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

/*******************************************************************************
* �������APP�߳��������� *********************------<�����޸�>------**********
*******************************************************************************/

#define MAX_ALLOWED_CTS_THREAD_NUM    2               //�������CTS������������߳�����
#define MAX_CTS_THREAD_NUM    2               //�������CTS����ʵ�������߳�����

/*******************************************************************************
* ���س���ģ���߳��������� *********************------<�����޸�>------**********
*******************************************************************************/

#define MAX_ALLOWED_DOWN_THREAD_NUM     2               //CCO������������߳�����
#define MAX_DOWN_THREAD_NUM             1               //CCO����ʵ�������߳�����




/*******************************************************************************
* ͨ�Ų�������
*******************************************************************************/
#define MAX_QUEUE_CNT          20              //��������

#define MAX_HID_SEND_LEN        65              //�ز�HID����ͳ���
#define MAX_HID_RECV_LEN        64              //�ز�HID�����ճ���

#define MAX_COMM_DELAY_TIME     6               //��������ʱ�ȴ�ʱ��
#define MAX_NORMAL_AMR_INFO_CNT     10            //������󳭱����

#define MAX_INFO_DATA_NUM      500             //��ϢMSG�����б�

#define MAX_MODULE_NUM     5       //ģ����������
#define MAX_CHANNEL_NUM     5      //ÿ��ģ�������5��ͨ�� 0Ĭ�Ϲ���ͨ��
#define MAX_MODULE_DELAY_TIME  6   //��ѯģ����Ϣ���ʱʱ��
#define CCO_MODULE_DEVNAME  "uart_acm"

/*
 ********************************************************************
 *                                                                  *
 * CCO������Ϣ�������Ͷ���                                          *
 *                                                                  *
 ********************************************************************
*/

#define MAX_QUEUE_BUFF_LEN                 5120                      //���С����л���������
#define MAX_MSG_PAYLOAD_LEN                2040                      //��ϢMSG��payload��󳤶�
#define MAX_DEAL_BUFF_LEN                  1500                      //����͸�����ӽڵ��ص��ں�ԭʼ���ĳ���
#define MAX_UINT8_LEN                      255                       //uint8��Ӧ��󳤶�

#define MAX_SEARCH_METER_FILES_NUM         1000                      //�ϱ��ѱ����ݱ�Ƶ��������
#define MAX_NEIGHBOR_NODE_NET_NUM          1000                      //�ھӽڵ������ʶ��������
#define MAX_NET_LEVEL_NUM                  1000                      //����㼶������
#define MAX_NEXT_NODE_NUM                  1000                      //�½ӽڵ�������
#define MAX_AMR_COLLECT_CNT                2040                      //��󳭱���ܸ���
#define MAX_FILES_SUMMARY_NUM              2040                      //��������������

#define MAX_ACTIVE_AMR_INFO_NUM            1000                      //����������������
#define MAX_DATA_TRANSMIT_INFO_NUM         1000                      //����͸������������
#define MAX_SLAVE_MONITOR_INFO_NUM         1000                      //�ӽڵ��ر���������

/*******************************************************************************
* ÿ�ζ�ȡ ɾ�� ��� ·�ɵ���Ŀ
*******************************************************************************/
#define MODULEDEAL_CNT                            10

/*******************************************************************************
* �ѱ���豸��ʶ
*******************************************************************************/
#define DEV_COLLECTOR          0x00           //�ɼ���
#define DEV_METER              0x01           //���

/*******************************************************************************
* �˿ں�����
*******************************************************************************/
#define PORT_F2090201          0xf2090201           //��1������ģ��˿ں�
#define PORT_F2090202          0xf2090202           //��2������ģ��˿ں�

#define CCO_PRODUCT_MODULE          1       /* ��ģ������� */
#define CCO_PRODUCT_NO_MODULE       2       /* û��ģ������� */


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


//��ϢHEAD
typedef struct tagInfoDataHead
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
}INFO_DATA_HEAD_T;


//��Ϣ�б�����
typedef  struct
{
    int16 InfoDataNum;  //��Ϣ�б�����������
    int16 InfoDataReadIndex;  //��ȡ�����������±꣬��0��ʼ��
    INFO_DATA_T InfoDataList[MAX_INFO_DATA_NUM];  //��ϢMSG�б�����
} INFO_DATA_LIST_T;  //��Ϣ�б�����

//����˿��޸�����
typedef enum
{
    AMR_PORT_MODIFY_DELETE = 0, //ɾ��
    AMR_PORT_MODIFY_ADD,  //���
}AMR_PORT_MODIFY_E;  

//����˿�����
typedef struct
{
    uint32   nPortID;  //�˿ں�
    AMR_PORT_MODIFY_E emPortModify;  //����˿��޸�����
}AMR_PORT_CFG_T;   


//�����ȶ�ʱ�ľ������
typedef enum
{
    CMP_FILES_PORT_ID = 0, //�˿ں�
    //CMP_FILES_ID,  //�������
    CMP_FILES_ADDR,  //������ַ
    CMP_FILES_MAC,  //����MAC
    CMP_FILES_PROTOCAL,  //Э������
    //CMP_FILES_COMM,  //ͨ�Ų���
    CMP_FILES_ADDR_PROTOCAL,  //ͬʱ�ȶԵ�ַ�͹�Լ
}AMR_FILES_CMP_PARA_E;  

//���ܱ���
typedef struct
{
    //uint16   nMeterFilesID; //�������
    uint8   aMeterFilesAddr[6];  //������ַ
    uint8   aMeterFilesMac[6];  //����MAC
    uint8   nProtocolType;  //Э������
    //uint8   nCommPara;  //ͨ�Ų���
}AMR_METER_FILES_T;  

//������Ϣ
typedef struct
{
    uint32   nPortID;   //�˿ں�
    AMR_METER_FILES_T stAmrMeterFiles;  //���ܱ���
}AMR_METER_FILES_INFO_T;  

//������Ϣ���ܱ�
typedef struct
{
    uint16   nMeterFilesNum;  //��������
    AMR_METER_FILES_INFO_T stMeterFilesData[MAX_FILES_SUMMARY_NUM]; //���ܱ���
}AMR_METER_FILES_LIST_T; 

typedef struct tagCCO_COM_DCB_T
{
    uint8   baud; /* ������ */
    uint8   ctrl; /* ����λ */
}CCO_COM_DCB_T;

//���󳭱���������
typedef struct
{
    uint32   nPortID;  //�˿ں�
    uint16   nMeterFilesID; //�������
    CCO_COM_DCB_T comDcb;  //ͨ�ſ�����
    uint8   aPlcCommModelSymbol[6];  //�ز�ͨ��ģʽ��
    uint8   nMeterAddr[6];  //��Ƶ�ַ
}AMR_METER_REQUEST_READ_T;  

//������Ϣ
typedef struct
{
    uint8   nRelayRouterLevel;  //�м�·�ɼ���
    uint8   nChannelIdentifier;  //�ŵ���ʶ
    uint8   nCarrierReadPhase;  //�ز�������λ
    uint8   nCarrierSignalQuality;  //�ز��ź�Ʒ��
    uint8   nCarrierSymbol;  //�ز���־
    uint8   aReserveForOtherUse[3];  //����  
}AMR_PLC_READING_INFO_T;  

//�ϱ���������
typedef struct
{
    uint32   nPortID;  //�˿ں�
    uint16   nMeterFilesID; //�������
    uint8   nProtocolType;  //Э������
    uint16  nDataBufLen;    //����֡����
    uint8   pDataBuf[MAX_DEAL_BUFF_LEN];    //����֡
    uint8   nReadingInfoFlag;  //�Ƿ��г�����Ϣ��־, 0x00:��, 0x01:��
    AMR_PLC_READING_INFO_T stPlcReadingInfo;  //������Ϣ
}AMR_PLC_INFO_REPORT_T;  

//�ϱ��¼�����
typedef struct
{
    uint32   nPortID;  //�˿ں�
    uint16   nMeterFilesID; //�������
    uint8   nProtocolType;  //Э������
    uint16  nDataBufLen;    //����֡����
    uint8   pDataBuf[MAX_DEAL_BUFF_LEN];    //����֡
    uint8   nReadingInfoFlag;  //�Ƿ��г�����Ϣ��־, 0x00:��, 0x01:��
    AMR_PLC_READING_INFO_T stPlcReadingInfo;  //������Ϣ
}AMR_EVENT_DATA_REPORT_T;  


//����͸��
typedef struct
{
    uint32   nPortID;  //�˿ں�
    uint8   nCommSymbol;  //ͨѶ��
    uint16   nFrameTimeover; //֡��ʱʱ��
    uint16   nCharTimeover; //�ַ���ʱʱ��
    uint16  nTransmitDataLen;   //����֡����
    uint8   pTransmitDataBuf[MAX_DEAL_BUFF_LEN];    //������������
}AMR_DATA_TRANSMIT_T;  

//����͸����Ϣ�ļ���
typedef struct
{
    pthread_mutex_t DataTransmitLock;                   //����͸����Ϣ������������
    uint16   nDataTransmitNum;  //����͸����Ϣ����
    int16    DataTransmitIndex;  //��ȡ�����������±꣬��0��ʼ��
    AMR_DATA_TRANSMIT_T aDataTransmitList[MAX_DATA_TRANSMIT_INFO_NUM];  //����͸����Ϣinfo����
    INFO_DATA_HEAD_T    aInfoDataHead[MAX_DATA_TRANSMIT_INFO_NUM];  //����͸����ϢinfoԭʼHEAD
}AMR_DATA_TRANSMIT_LIST_T; 


//�����ѱ�
typedef struct
{
    uint32   nPortID;  //�˿ں�
    int32    nStartTime;  //��ʼʱ��
    int32    nContinuedTime;  //����ʱ��
    uint8   nSlaveNodeReSendCount;  //�ӽڵ��ط�����
    uint8   nRandWaitTimeSlice;  //����ȴ�ʱ��Ƭ����
}AMR_METER_START_SEARCH_T;  

//�Ƿ������ѱ�
typedef struct
{
    BOOL bStartSearch;  //�Ƿ���������
    AMR_METER_START_SEARCH_T stAmrMeterStartSearch;  //�ѱ���Ϣ
}WHETHER_START_SEARCH_METER_T;  


//�ϱ��ѱ�����
typedef struct
{
    uint32   nPortID;  //�˿ں�
    uint8   nReadingInfoFlag;  //�Ƿ��г�����Ϣ��־, 0x00:��, 0x01:��
    AMR_PLC_READING_INFO_T stPlcReadingInfo;  //�ز����丽����Ϣ
    uint16  nAmrMeterFilesNum;  //��Ƶ�������
    AMR_METER_FILES_T stAmrMeterFiles[MAX_SEARCH_METER_FILES_NUM];  //��Ƶ���
}AMR_METER_SEARCH_REPORT_T; 

//��ȡ�ӽڵ�����ʱ
typedef struct
{
    uint32   nPortID;  //�˿ں�
    uint8    aSlaveNodeAddr[6]; //�ز��ӽڵ��ַ
    uint8    nProtocolType; //��Լ����
    uint16   nDataBufLen;  //���ĳ���
    uint8    aDataBuf[MAX_DEAL_BUFF_LEN];   //��������
}AMR_SLAVE_NODE_DELAY_TIME_T; 

//��ȡ�ӽڵ�����ʱ�ļ���
typedef struct
{
    pthread_mutex_t SlaveMonitorTimeLock;  //�ӽڵ�����ʱ��Ϣ������������
    uint16   nSlaveMonitorTimeNum;  //�ӽڵ�����ʱ��Ϣ����
    int16    SlaveMonitorTimeIndex;  //��ȡ�����������±꣬��0��ʼ��
    AMR_SLAVE_NODE_DELAY_TIME_T aSlaveMonitorTimeList[MAX_SLAVE_MONITOR_INFO_NUM];  //�ӽڵ�����ʱ��Ϣinfo����
    INFO_DATA_HEAD_T    aInfoDataHead[MAX_SLAVE_MONITOR_INFO_NUM];  //�ӽڵ�����ʱ��ϢinfoԭʼHEAD
}AMR_SLAVE_MONITOR_TIME_LIST_T; 

//�ӽڵ���
typedef struct
{
    uint32   nPortID;  //�˿ں�
    uint8   nProtocolType;  //��Լ����
    uint8   aSlaveNodeAddr[6];  //�ز��ӽڵ��ַ
    CCO_COM_DCB_T comDcb;  //ͨ�ſ�����
    uint16   nFrameTimeover; //֡��ʱʱ��
    uint16   nByteTimeover; //�ֽڳ�ʱʱ��
    uint16   nDataBufLen;  //���ĳ���
    uint8    aDataBuf[MAX_DEAL_BUFF_LEN];   //��������
}AMR_SLAVE_NODE_MONITOR_T; 

//�ӽڵ�����Ϣ�ļ���
typedef struct
{
    pthread_mutex_t SlaveMonitorLock;                   //�ӽڵ�����Ϣ������������
    uint16   nSlaveMonitorNum;  //�ӽڵ�����Ϣ����
    int16    SlaveMonitorIndex;  //��ȡ�����������±꣬��0��ʼ��
    AMR_SLAVE_NODE_MONITOR_T aSlaveMonitorList[MAX_SLAVE_MONITOR_INFO_NUM];  //�ӽڵ�����Ϣinfo����
    INFO_DATA_HEAD_T    aInfoDataHead[MAX_SLAVE_MONITOR_INFO_NUM];  //�ӽڵ�����ϢinfoԭʼHEAD
}AMR_SLAVE_MONITOR_LIST_T; 


//��ȡ�㲥��ʱ
typedef struct
{
    uint32   nPortID;  //�˿ں�
    uint8   nProtocolType;  //��Լ����
    uint16  nDataBufLen;    //����֡����
    uint8   aDataBuf[MAX_UINT8_LEN];    //����֡
}AMR_GET_BROADCAST_DELAY_T; 

//�Ƿ��ȡ�㲥��ʱ
typedef struct
{
    BOOL bGetBroadcastDelay;  //�Ƿ��ȡ�㲥��ʱ
    INFO_DATA_HEAD_T InfoDataHead;  //��ϢԭʼHEAD
    AMR_GET_BROADCAST_DELAY_T stAmrGetBroadcastDelay;  //��ȡ�㲥��ʱ
}WHETHER_GET_BROADCAST_DELAY_T; 


//�㲥
typedef struct
{
    uint32   nPortID;  //�˿ں�
    uint8   nProtocolType;  //��Լ����
    CCO_COM_DCB_T comDcb;  //ͨ�ſ�����
    uint16  nDataBufLen;    //����֡����
    uint8   aDataBuf[MAX_DEAL_BUFF_LEN];    //����֡
}AMR_DO_BROADCAST_T; 

//�Ƿ�㲥
typedef struct
{
    BOOL bDoBroadcast;  //�Ƿ��ȡ�㲥��ʱ
    AMR_DO_BROADCAST_T stAmrDoBroadcast;  //�㲥
}WHETHER_DO_BROADCAST_T;


//��ǰ����ģʽ
typedef enum
{
    AMR_MODE_ACTIVE = 0, //��������
    AMR_MODE_PASSIVE,  //��������
}AMR_CURRENT_MODE_E;  

//��������
typedef struct
{
    uint32   nPortID;  //�˿ں�
    //uint16   nMeterFilesID; //�������
    CCO_COM_DCB_T comDcb;  //ͨ�ſ�����
    uint8    nMeterAddr[6]; //��Ƶ�ַ
    uint16   nDataBufLen;  //���ĳ���
    uint8    aDataBuf[MAX_DEAL_BUFF_LEN];   //��������
}AMR_MODE_ACTIVE_T; 

//������������ļ���
typedef struct
{
    pthread_mutex_t ActiveAmrInfoLock;                  //������Ϣ������������
    uint16   nActiveAmrInfoNum;  //����������
    int16    ActiveAmrInfoIndex;  //��ȡ�����������±꣬��0��ʼ��
    AMR_MODE_ACTIVE_T aActiveAmrInfoList[MAX_ACTIVE_AMR_INFO_NUM];  //��������info����
}AMR_ACTIVE_INFO_LIST_T; 


//��ѯ����
typedef struct
{
    uint32   nPortID;  //�˿ں�
    uint16   nFilesStartID; //���������ʼ
    uint16   nFilesCount; //��ѯ������
}AMR_FILES_QUERY_T; 

//��ѯHPLC��Ϣ
typedef struct
{
    uint32   nPortID;  //�˿ں�
    uint8   nInfoType;  //��Ϣ����
}AMR_HPLC_INFO_QUERY_T; 

//�ڵ�����������Ϣ
typedef struct
{
    uint8   nNodeAddr[6];  //�ڵ��ַ
    uint8   nNodeTopologyInfo[5];  //�ڵ�������Ϣ
}AMR_HPLC_TOPOGOGY_INFO_T; 

//����ز�оƬ��Ϣ
typedef struct
{
    uint16   nNodeID; //�ڵ����
    uint8   nNodeAddr[6];  //�ڵ��ַ
    uint8   nNodeEquipType;  //�ڵ��豸����
    uint8   nNodeChipInfo[24];  //�ڵ�оƬID��Ϣ
    uint8   nNodeChipSoftVer[2];  //�ڵ�оƬ����汾
}AMR_HPLC_CHIP_INFO_T; 

//��������Ϣ
typedef struct
{
    uint8   nNeighborNodeNum;  //�ھӽڵ����
    uint8   nNodeNetSymbol[3];  //���ڵ������ʶ��
    uint8   nNodeMasterNodeAddr[6];  //���ڵ����ڵ��ַ
    uint8   nNeighborNodeNetSymbol[MAX_NEIGHBOR_NODE_NET_NUM][3];  //�ھӽڵ������ʶ��
}AMR_HPLC_NETS_INFO_T; 

//�ڵ�������Ϣ
typedef struct
{
    uint16   nNodeID; //�ڵ����
    uint8   nNodeAddr[6];  //�ڵ��ַ
    uint8   nNodePhaseInfo[2];  //�ڵ���λ��Ϣ
}AMR_HPLC_PHASE_INFO_T; 

//���ڵ�ģ��ID��Ϣ
typedef struct
{
    uint8   nModuleFacSymbol[2];  //ģ�鳧�̴���
    uint8   nModuleIdLength;  //ģ��ID����
    uint8   nModuleIdFormat;  //ģ��ID��ʽ
    uint8   nModuleID;  //ģ��ID
}AMR_HPLC_MASTER_ID_INFO_T; 

//�ӽڵ�ģ��ID��Ϣ
typedef struct
{
    uint16   nNodeID; //�ӽڵ����
    uint8   nNodeAddr[6];  //�ӽڵ��ַ
    uint8   nNodeType;  //�ӽڵ�����
    uint8   nModuleFacSymbol[2];  //�ӽڵ�ģ�鳧�̴���
    uint8   nModuleIdLength;  //�ӽڵ�ģ��ID����
    uint8   nModuleIdFormat;  //�ӽڵ�ģ��ID��ʽ
    uint8   nModuleID;  //�ӽڵ�ģ��ID
}AMR_HPLC_SLAVE_ID_INFO_T; 

//�㼶��Ϣ
typedef struct
{
    uint16   nBelongLevel; //���ڲ㼶
    uint8   nNodeNumOfLevel[2];  //�˲㼶�ڵ����
}AMR_HPLC_LEVEL_INFO_T; 

//���������Ϣ
typedef struct
{
    uint8   nNetNodeCount[2];  //����ڵ�����
    uint8   nOnlineNodeCount[2];  //����վ�����
    uint8   nNetStartNetPeriod[2];  //������������ʱ��
    uint8   nNetWorkLinkPeriod[2];  //����ʱ��
    uint8   nBeaconCycle;  //�ű�����
    uint8   nRouterCycle[2];  //·������
    uint8   nTopoChangeTimes[2];  //���˱������
    AMR_HPLC_LEVEL_INFO_T  stLevelInfo[MAX_NET_LEVEL_NUM];  //�㼶��Ϣ
}AMR_HPLC_BASE_ID_INFO_T; 

//�½ӽڵ���Ϣ
typedef struct
{
    uint8   nNodeAddr[6];  //�½Ӵӽڵ�ͨ�ŵ�ַ
    uint8   nCommProtocolType;  //�½Ӵӽڵ�ͨ��Э������
    uint8   nNodeEquipType;  //�½Ӵӽڵ��豸����
}AMR_HPLC_NEXT_NODE_INFO_T; 

//����ڵ���Ϣ
typedef struct
{
    uint16   nNodeID; //���
    uint8   nNodeAddr[6];  //�ڵ��ַ
    uint8   nNodeTopologyInfo[5];  //�ڵ�������Ϣ
    uint8   nNetInfo;  //������Ϣ
    uint8   nNodeEquipType;  //�豸����
    uint8   nPhaseInfo;  //��λ��Ϣ
    uint8   nAgentChangeTimes[2];  //����������
    uint8   nStaOfflineTimes[2];  //վ�����ߴ���
    uint8   nStaOfflinePeriod[4];  //վ������ʱ��
    uint8   nStaOfflinePeriodMax[4];  //վ���������ʱ��
    uint8   nUpCommSuccessRate[4];  //����ͨ�ųɹ���
    uint8   nDownCommSuccessRate[4];  //����ͨ�ųɹ���
    uint8   nMainVerID[3];  //���汾��
    uint8   nSecondaryVerID[2];  //�ΰ汾��
    uint8   nNextTurnInfo[2];  //��һ����Ϣ
    uint8   nChannelType[2];  //�ŵ�����
    uint8   nProtocolType;  //��Լ����
    uint8   nTransformAreaState;  //̨��״̬
    uint8   nTransformAreaAddr[6];  //̨���ŵ�ַ
    AMR_HPLC_NEXT_NODE_INFO_T  stNextNodeInfo[MAX_NEXT_NODE_NUM];
}AMR_HPLC_NET_NODE_INFO_T; 

//�ϱ�HPLC��ѯ��Ϣ����
typedef enum
{
    AMR_HPLC_TOPOLOGY_INFO = 1,  //����������Ϣ   [1] SEQUENCE OF AMR_HPLC_TOPOLOGYINFO��
    AMR_HPLC_CHIP_INFO,  //����ز�оƬ��Ϣ [2] SEQUENCE OF AMR_HPLC_CHIPINFO��
    AMR_HPLC_NETS_INFO,  //��������Ϣ          [3] AMR_HPLC_NETSINFO��
    AMR_HPLC_PHASE_INFO,  //�ڵ�������Ϣ      [4] SEQUENCE OF AMR_HPLC_PHASEINFO��
    AMR_HPLC_NET_SCALE,  //�����ģ             [5] long-unsigned��
    AMR_HPLC_MASTER_ID,  //���ڵ�ģ��ID��Ϣ  [6] SEQUENCE OF AMR_HPLC_MASTERID��
    AMR_HPLC_SLAVE_ID,  //�ӽڵ�ID��Ϣ     [7] SEQUENCE OF AMR_HPLC_SLAVEID��
    AMR_HPLC_BASE_INFO,  //���������Ϣ   [8] AMR_HPLC_BASEINFO��
    AMR_HPLC_NET_NODE_INFO,  //����ڵ���Ϣ   [9] SEQUENCE OF AMR_HPLC_NETNODEINFO
}AMR_HPLC_QUERY_INFO_TYPE_E;  


//�ϱ�HPLC��ѯ��Ϣ
typedef struct
{
    uint32   nPortID;  //�˿ں�
    uint16   nStartID; //��ʼ���
    AMR_HPLC_QUERY_INFO_TYPE_E  emQueryInfoType;  //�ϱ�HPLC��ѯ��Ϣ����
    uint8   *pDataBuf;  //����
}AMR_HPLC_QUERY_INFO_REPORT_T; 

//HPLCƵ������
typedef struct
{
    uint32   nPortID;  //�˿ں�
    uint8   nFrequencyPot;  //Ƶ��
}AMR_HPLC_FREQUENCY_POT_SET_T; 

//HPLC̨��ʶ��
typedef struct
{
    uint32   nPortID;  //�˿ں�
    uint8   nSwitch;  //����
}AMR_HPLC_TRANSFORM_AREA_TELL_T; 

//HPLC������֪
typedef struct
{
    uint32   nPortID;  //�˿ں�
    uint8   nSwitch;  //����
}AMR_HPLC_OFFLINE_SENSE_T; 

//��ѯ����״̬
typedef struct
{
    uint32   nLastRecvTime;  //���һ�ν���ʱ��
    int32    nModuleAmrState; //ģ�鳭��״̬ 1�������У�0��δ����
    int32    nModuleSearchState; //ģ���ѱ�״̬ 1���ѱ��У�0��δ�ѱ�
    int32    nAutoAmrState; //�Զ�����״̬
    uint16   nClearRouterTimes; //���·�ɴ���
    uint16   nDataInitTimes; //���ݳ�ʼ������
    uint16   nParaInitTimes; //������ʼ������
    uint16   nResetModuleTimes; //��λģ�����
}AMR_QUERY_READING_STATE_T; 

//��ѯ����ģ����Ϣ
typedef struct
{
    uint8   aLocalCommTypeModel[6];  //����ͨ��ģʽ��
    uint16   nSlaveMonitorMaxOvertime; //�ӽڵ������ʱ
    uint16   nBroadcastMaxOvertime; //�㲥���ʱʱ��
    uint16   nMaxAllowedFrameLen; //���֧�ֱ��ĳ���
    uint16   nMaxUpgradeSingleFrameLen; //������󵥸�������
    uint8   nUpgradeWaitingPeriod;  //���������ȴ�ʱ��
    uint16   nBroadcastDelayPeriod;  //�㲥�ɹ���ʱʱ��
    uint8   aModuleID[6];  //ģ����
    uint16   nMaxSupportSlaveNum; //֧�ֵ����ӽڵ�����
    uint16   nCurrentSlaveNum; //��ǰ�ӽڵ�����
    uint8    InfoBuff[9];        //ԭʼ����(������)
    uint8   ManufacturerCode[2]; //���̴���
    uint8   ChipCode[2];         //оƬ����
    uint8   EditionDate[3];      //�汾������
    uint16  nModuleVer;         //�汾��
}AMR_QUERY_MODULE_INFO_T; 

//ģ����Ϣ�е�ͨ����Ϣ
typedef struct
{
    uint8 ChannelType;//�ӿ�����    enum{CDC-ACM(1), CDC-ECM(2)},
    uint8 ChannelFunc;//��������    enum{����ͨ��(0), Զ��ͨ��(1)��PLC/RFͨ��(2)��RS-485ͨ��(3)��ң������(4)��CANͨ��(5)��M-Busͨ��(6)������(7)��ģ�����ɼ�(8)��RS232ͨ��(9)}
    uint8 DriveName[32];
}CHANNEL_INFO_T;  //ģ����Ϣ�е�ͨ����Ϣ

//ģ����Ϣ
typedef struct
{
    pthread_rwlock_t   ModuleLock;  //��дģ����Ϣ���ö�д��
    uint8 IsValid;//�Ƿ���Ч 0��Ч 1��Ч
    uint8 DeviceType[32];//�豸�ͺ�
    uint8 DeviceID[32];//�豸id
    uint8 SoftVersion[32];//����汾
    uint8 SoftDate[10];//�����������
    uint8 HardVersion[32];//Ӳ���汾
    uint8 HardDate[10];//Ӳ����������
    uint8 ManufacturerCode[8];//���̱���
    uint8 ChannelNum;//ͨ������
    CHANNEL_INFO_T ChannelInfo[MAX_CHANNEL_NUM];//ͨ������
    BOOL bCCO;//�Ƿ񱾵�ͨ��ģ��
}MODULE_INFO_T;//ģ����Ϣ

//����ģ��HAL�ӿڲ���
typedef struct
{
    char  deviceName[30];  /*�豸�� */
    char  ctrlName[30]; /* ����ͨ������ */
    uint8 isvalid;
    uint8 moduleIndex;  /*ģ���� ��1��ʼ */
    UART_DEVICE_T* dev;
    LOCAL_CU_DEVICE_T *ctrlDev; /* ����ͨ����� */
}DEV_HAL_INFO;


/*******************************************************************************
* ö�ٺͽṹ�嶨��
*******************************************************************************/



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

//�Զ������̲߳����ṹ
typedef struct
{
    uint8        nArrIndex;                     //�����±�
    ThreadArg_T *UpIpcArg;                      //����IPCͨ�Ž������ṹָ��
    ThreadArg_T *DownCommArg;                   //����ͨ�Ž������ṹָ��
}AmrThreadArg_T;

typedef struct
{
    uint8   DeviceName[30];                     //�豸������"/dev/ttyS0"
    int32  BaudRate;                            //������
    int8   DataBits;                            //����λ, 4-8
    int8   ParityFlag;                          //У�� 'n'  'e'  'o'  's' = None,Odd,Even,Space
    int8   StopBits;                            //ֹͣλ 0,1,2 = 1, 1.5, 2
}COMM_PARA_T;

typedef  struct
{
    uint8   AFN;                                //Ӧ�ù�����
    uint16  Fn;                                 //��Ϣ���ʶ
    uint8   ModuleID;                           //ͨ��ģ���ʶ(0:������ / 1:�ز���)
    uint8   DesAddr[6];                         //Ŀ�ĵ�ַ
    uint8   RInfo[6];                           //��Ϣ��
    uint16  DataUnitLen;                        //Ӧ�����ݳ���
    uint8   DataUnitBuff[MAX_QUEUE_BUFF_LEN];   //Ӧ�����ݻ�����
} AppData_T; 

typedef struct
{
    uint8   Second;                             //BIN or BCD
    uint8   Minute;                             //BIN or BCD
    uint8   Hour;                               //BIN or BCD
    uint8   Day;                                //BIN or BCD
    uint8   Month;                              //BIN or BCD
    uint8   Year;                               //BIN or BCD
}DateTime_T;                                    //����ʱ��ṹ

typedef struct
{
    uint8   Second              :1;             //�����־
    uint8   Minute              :1;             //���ֱ�־
    uint8   Hour                :1;             //��ʱ��־
    uint8   Day                 :1;             //���ձ�־
    uint8   Month               :1;             //���±�־
    uint8   ReserveBit6         :1;             //����
    uint8   ReserveBit7         :1;             //����
    uint8   ReserveBit8         :1;             //����
}PastTimeFlagBit_T;

typedef union
{
    PastTimeFlagBit_T   BitSect;
    uint8               Word;
}PastTimeFlag_T;       

typedef struct
{
    uint8  COMM_TYPE        :6; //ͨ�ŷ�ʽ
    uint8  PRM              :1; //������־λ
    uint8  DIR              :1; //������
}CTRLCODE_BITS_T;//ң��λ

typedef struct
{
    uint8   CommPattern        :4;  //ͨ�ŷ�ʽ(1:խ��,2:���,3:С����)
    uint8   RouterMangerPattern    :1;  //·�ɹ���ʽ(0:��·�ɹ���,1:��·�ɹ���)
    uint8   SlaverInfoMode               :1;    //�ӽڵ���Ϣģʽ(0:�����·�,1:��Ҫ�·�)
    uint8   AmrCycleMode                 :2;    //���ڳ���ģʽ(01:����,10:����,11:��֧��,00:����)
} LOCAL_COMM_PATTERN_WORD_1;

typedef union
{
    LOCAL_COMM_PATTERN_WORD_1    BitSect;
    uint8                        Word;
} LOCAL_COMM_PATTERN_WORD_B1;

typedef struct
{
    uint8   TransDelayParamSupport      :3; //������ʱ����֧��
    uint8   FailureNodeSwitchLaunchPattern  :2; //ʧ�ܽڵ��л�����ʽ
    uint8   BroadcastAffirmPattern                     :1; //�㲥����ȷ�Ϸ�ʽ(0,1)
    uint8   BroadcastChannelExecPattern        :2; //�㲥�����ŵ�ִ�з�ʽ(0:ִ�й㲥���������������,1:��Ҫ)
} LOCAL_COMM_PATTERN_WORD_2;

typedef union
{
    LOCAL_COMM_PATTERN_WORD_2    BitSect;
    uint8            Word;
} LOCAL_COMM_PATTERN_WORD_B2;

typedef struct
{
    uint8   ChannelNum              :5;             //�ŵ����� <corrigendum-2013-09-18>
    uint8   LowVoltageGridPowerDownInfo :3;         //��ѹ����������Ϣ <corrigendum-2013-09-18>
} LOCAL_COMM_PATTERN_WORD_3;

typedef union
{
    LOCAL_COMM_PATTERN_WORD_3    BitSect;
    uint8            Word;
} LOCAL_COMM_PATTERN_WORD_B3;

typedef struct
{
    uint8   SpeedNum                        :4;             //��������n-l <corrigendum-2013-09-18>
    uint8   Reserved                        :4;             //������ h <corrigendum-2013-09-18>
} LOCAL_COMM_PATTERN_WORD_4;

typedef union
{
    LOCAL_COMM_PATTERN_WORD_4    BitSect;
    uint8            Word;
} LOCAL_COMM_PATTERN_WORD_B4;

typedef struct
{
    uint8   Reserved            :8;             //������
} LOCAL_COMM_PATTERN_WORD_5;

typedef union
{
    LOCAL_COMM_PATTERN_WORD_5    BitSect;
    uint8            Word;
} LOCAL_COMM_PATTERN_WORD_B5;

typedef struct
{
    uint8   Reserved            :8;             //������
} LOCAL_COMM_PATTERN_WORD_6;

typedef union
{
    LOCAL_COMM_PATTERN_WORD_6    BitSect;
    uint8            Word;
} LOCAL_COMM_PATTERN_WORD_B6;


typedef struct
{
    LOCAL_COMM_PATTERN_WORD_B1 b1;
    LOCAL_COMM_PATTERN_WORD_B2 b2;
    LOCAL_COMM_PATTERN_WORD_B3 b3;
    LOCAL_COMM_PATTERN_WORD_B4 b4;
    LOCAL_COMM_PATTERN_WORD_B5 b5;
    LOCAL_COMM_PATTERN_WORD_B6 b6;
} LOCAL_COMM_PATTERN_WORD;

typedef struct
{
    uint8   ManufacturerCode[2]; //���̴���
    uint8   ChipCode[2];         //оƬ����
    uint8   EditionDate[3];      //�汾������
    uint16  EditionInfo;         //�汾��
    uint8    InfoBuff[9];        //ԭʼ����(������)
} MANUFACTURE_INFO;

typedef struct
{
    uint16   CommSpeed                      :15;            //ͨ������
    uint16   SpeedUintIdentifying           :1;         //���ʵ�λ��ʶ
} COMM_SPEED_UNIT_INFO_T;

typedef union
{
    COMM_SPEED_UNIT_INFO_T       BitSect;
    uint16               Word;
} COMM_SPEED_UNIT_INFO;


typedef struct
{
    LOCAL_COMM_PATTERN_WORD LocalCommPatternWord;   //����ͨ��ģʽ��
    uint8  SlaverMonitorMaxOverTime;                //�ӽڵ������ʱʱ��(��λ:s)<corrigendum-2013-09-18>
    uint16 BroadcastMaxOverTime;                    //�㲥�������ʱʱ��(��λ:s)
    uint16 MaxFrameLen;                             //���֧�ֵı��ĳ���
    uint16 FileTransferMaxSolePackageLen;           //�ļ�����֧�ֵ���󵥸����ݰ�����
    uint8  UpdateOperatorWaittingTime;              //���������ȴ�ʱ��
    uint8  MasterAddr[6];                           //���ڵ��ַ
    uint16 MaxSlaverNum;                            //֧�ֵ����ӽڵ�����
    uint16 CurrentSlaverNum;                        //��ǰ�ӽڵ�����
    uint8  ModuleProtocolReleaseDate[3];            //ͨ��ģ��ʹ�õ�Э�鷢������(BCD)
    uint8  ModuleProtocolLastRecordDate[3];         //ͨ��ģ��ʹ�õ�Э����󱸰�����(BCD)
    MANUFACTURE_INFO  ManufacturerInfo;             //ͨ��ģ�鳧�̴��뼰�汾��Ϣ
    COMM_SPEED_UNIT_INFO CommSpeedUnitInfo[256];    //���ʵ�λ��ʶ��ͨ������
} LOCAL_COMM_MODULE_RUNNING_PATTERN_INFO;

typedef enum
{
    MS_IDLE = 0,                                //����
    MS_DELAY_DEAL,                              //1 ��ʱ����
    MS_GET_MANU_ID,                             //2 ��ȡ���̱�ʶ
    MS_POWER_ON,                                //3 �ϵ紦��
    MS_FILE_CMP,                                //4 ������ȶ�
    MS_FILE_MODIFY,                             //5 ������޸Ĵ���
    MS_DEL_SLAVE_DEAL,                          //6 ɾ���ӽڵ㴦��
    MS_CUS_TASK_DEAL,                           //7 �Զ��嵥��������
    MS_SWITCH_CHANNEL,                          //8 �л��ŵ�����
    MS_ROUTER_RESTART,                          //9 ����
    MS_ROUTER_PAUSE,                            //10 ��ͣ
    MS_ROUTER_RESUME,                           //11 �ָ�
    MS_UPDATE_DEAL,                             //12 �ز�ģ�鱾���ļ���������
    MS_WORK_MODE,                               //13 ����·�ɹ���ģʽ
    MS_NET_STUDY_DEAL,                          //14 ΢����������������
    MS_ADDR_SEARCH,                             //15 ���������
    MS_RELAY_TRANS,                             //16 ���͸��ת��
    MS_ROUTER_DEAL,                             //17 ·�ɲ���
    MS_HOUR_AMR,                                //18 Сʱ����
    MS_ROUTER_STUDY,                            //19 ·��ѧϰ
    MS_NORMAL_AMR,                              //20 �ճ�����
    MS_SLAVE_DELAY,                             //21 ��ȡ�ӽڵ�����ʱ
    MS_SLAVE_MONITOR,                           //22 �ӽڵ���
    MS_BROADCAST_DELAY,                         //23 ��ȡ�㲥��ʱ
    MS_BROADCAST_DEAL,                          //24 �㲥
    MS_HPLC_REPORT,                             //25 HPLC��ѯ��Ϣ�ϱ�
    MS_HPLC_FR_SET,                             //26 HPLCƵ������
    MS_HPLC_FR_QUERY,                           //27 HPLCƵ���ѯ
    MS_HPLC_AREA,                               //28 HPLC̨��ʶ��
    MS_HPLC_OFFLINE,                            //29 HPLC������֪
    MS_HPLC_REFUSE,                             //30 HPLC�ܾ��ڵ��ϱ�
    MS_AMR_STATE_QUERY,                         //31 ��ѯ����״̬
    MS_AMR_MODULE_QUERY,                        //32 ��ѯ����ģ����Ϣ
}MAIN_CYC_STATE_T;                              //��ѭ��״̬��

typedef enum
{
    RCS_NULL = 0,                               //��Ч
    RCS_RESTART,                                //����
    RCS_PAUSE,                                  //��ͣ
    RCS_RESUME,                                 //�ָ�
    RCS_RESUME_EXEC,                            //�ָ�ִ��
}ROUTER_CTRL_STATE_T;                           //·�ɱ����״̬

typedef enum
{
    TASK_INIT = 0,                      //�����ʼ��
    ROUTERCTRL_DEAL,                    //·�ɿ���
    ROUTERCTRL_DELAY,                   //��ʱִ��
    FUN_DEAL,                           //���ܺ���ִ��
    ROUTERRECOVER_DELAY,                //·�ɻָ���ʱ
    ROUTERRECOVER_DEAL,                 //·�ɻָ�ִ��
}TASKDEAL_STAT;

typedef struct
{
    uint8 isValidFlag;  //��ǰ·�ɲ����Ƿ���Ч(�Ƿ������δ��ɵ�·�ɲ���)
    ROUTER_CTRL_STATE_T eRouterCtrl;  //·�ɲ�������
    INFO_DATA_T stInfoMsgData;  //ԭʼ��������
}ROUTER_OPER_DETAIL_T;//·�ɲ�����������

typedef struct
{
    INFO_DATA_HEAD_T      InfoDataHead;  // ��ϢԭʼHEAD
    
    uint8                 TaskStat;              //������Ч��
    MAIN_CYC_STATE_T      TaskName;              //������
    char                  strTaskName[50];       //������(�ַ���)
    int8                  Priority;              //�������ȼ�
    ROUTER_CTRL_STATE_T   RoutrtCtrl;            //·�ɿ��Ʋ���
    uint8                 DelaySec;              //��ʱ
    TASKDEAL_STAT         PlcTaskDeal_Stat;      //�ز�����ִ��״̬
    uint8                 InfoMsgBuff[MAX_MSG_PAYLOAD_LEN];           //�����������Ϣ����
    uint32                TaskArrIndex;            //�������õĶ˿������±�(��0��ʼ)
}TASK_FUN_INPUT;//�ز����������Ϣ�������������ܺ��������


typedef struct
{
    INFO_DATA_HEAD_T      InfoDataHead;  // ��ϢԭʼHEAD

    uint8                 TaskStat;              //������Ч��
    MAIN_CYC_STATE_T      TaskName;              //������
    char                  strTaskName[50];       //������(�ַ���)
    int8                  Priority;              //�������ȼ�
    ROUTER_CTRL_STATE_T   RoutrtCtrl;            //·�ɿ��Ʋ���
    uint8                 DelaySec;              //��ʱ
    TASKDEAL_STAT         PlcTaskDeal_Stat;      //�ز�����ִ��״̬
    uint8                 InfoMsgBuff[MAX_MSG_PAYLOAD_LEN];           //�����������Ϣ����
    uint32                TaskArrIndex;            //�������õĶ˿������±�(��0��ʼ)
    int                   (*pPlcTaskFun)(TASK_FUN_INPUT);  //�����ܺ���
}PLCTASK_DETAIL;//�ز����������Ϣ

typedef struct node
{
    PLCTASK_DETAIL        stPlcTaskDetail;       //�ز����������Ϣ
    struct node           *next;                 //ָ����һ�������ָ��
}PLCTASK_NODE;//�ز��������
typedef struct node *PLCTASK_QUEUE; /* ����LinkList */

typedef struct
{
    uint8                   TaskStat;              //������Ч��
    MAIN_CYC_STATE_T        TaskName;              //������
    char*                   strTaskName;           //������(�ַ���)
    uint8                   Priority;              //�������ȼ�
    ROUTER_CTRL_STATE_T     RoutrtCtrl;            //����·�ɲ���
    uint8                   DelaySec;              //������ʱʱ��
    int                     (*pPlcTaskFun)(TASK_FUN_INPUT);  //�����ܺ���
}PLCTASK_FUNINFO;//�ز���������Ϣ


typedef enum
{
    ACT_COMM_IDLE = 0,                          //����
    ACT_COMM_SEND,                              //����
    ACT_COMM_RECV,                              //����
    ACT_COMM_RETRY,                             //����
    ACT_COMM_DELAY,                             //��ʱ
}ACTIVE_COMM_STATE_T;                           //����ͨѶ״̬��


typedef enum
{
    MFCS_IDLE = 0,                              //����
    MFCS_PAUSE,                                 //��ͣ
    MFCS_PAUSE_DELAY,                           //��ͣ��ʱ
    MFCS_SET_TIME,                              //����ʱ��
    MFCS_READ_AMOUNT_FILE,                      //��ȡ�ӽڵ�����
    MFCS_READ_INFO_FILE,                        //��ȡ�ӽڵ���Ϣ
    MFCS_RESULT_DEAL,                           //�������
}METER_FILE_CMP_STATE_T;                        //������ȶ�״̬��

typedef enum
{
    ACT_MODULE_IDLE = 0,                          //����
    ACT_WAIT_MODULEINFO,                          //�ȴ�ģ���ϱ���Ϣ�����ϱ�
    ACT_READ_MODULEINFO,                          //��ȡģ���ϱ���Ϣ
    ACT_MODULE_SEND_GET_ADDRESS,                  //��ȡ���ڵ��ַ
    ACT_MODULE_SEND_SET_ADDRESS,                  //�������ڵ��ַ
    ACT_MODULE_END,                               //ģ���ϱ���Ϣ����

}GET_MODULE_STATE_T;                           //����ͨѶ״̬��

typedef struct
{
    uint8   HourAmr;                            //���㳭��
    uint8   RelayTrans;                         //�м�
    uint8   AddrSearch;                         //�������
}BREAK_STATE_T;   

typedef enum
{
    MFMS_IDLE = 0,                              //����
    MFMS_INIT,                                  //��ʼ��
    MFMS_PAUSE,                                 //��ͣ
    MFMS_DELET_METERFILE_FLAG,    //���Ե����޸�//start Common Problem
    MFMS_FILE_CLEAR,                            //�������
    MFMS_FILE_ADD,                              //�������
    MFMS_FILE_DELETE,                           //����ɾ��
    MFMS_RESTART,                               //����
}METER_FILE_MODIFY_STATE_T;                     //������޸�״̬��

typedef enum
{
    AS_IDLE = 0,                                //����
    AS_PAUSE,                                   //��ͣ
    AS_PAUSE_DELAY,                             //��ͣ��ʱ
    AS_ACT_SEARCH_MTRID,                        //�����Զ��ѱ�
    AS_ACT_REG,                                 //����ע��
    AS_WAIT_REPORT,                             //�ȴ��ϱ�
    AS_GET_RUN_STATE,                           //��ȡ·������״̬
    AS_SEARCH_END,                              //��������
}ADDR_SEARCH_STATE_T;                           //�������״̬��

typedef enum
{
    INNER_RTS_IDLE = 0,                               //����
    INNER_RTS_BROASTDELAY,                            //�㲥��ʱ��ȡͨ������ʱʱ��
    INNER_RTS_SEND,                                   //����
    INNER_RTS_RECV,                                   //����
    INNER_RTS_RETRY,                                  //����
    INNER_RTS_END,                                    //����
}INNER_RELAY_TASK_STATE_T;                            //�ڲ�����͸���߼�

typedef enum
{
    RSS_IDLE = 0,                               //����
    RSS_SET_STUDY_MODE,                         //����·��ѧϰģʽ
    RSS_GET_RUN_STATE,                          //��ȡ����״̬
    RSS_DELAY,                                  //��ʱ״̬
    RSS_SET_AMR_MODE,                           //����·�ɳ���ģʽ
    RSS_STUDY_END,                              //ѧϰ����
}ROUTER_STUDY_STATE_T;                          //·��ѧϰ����״̬��

typedef enum
{
    RTS_IDLE = 0,                               //����
    RTS_PAUSE,                                  //��ͣ
    RTS_PAUSE_DELAY,                            //��ͣ��ʱ
    RTS_SET_TIME,                               //����ʱ��
    RTS_READ_MODULE_STATE,                      //��ѯģ��״̬
    RTS_READ_DELAY,                             //��ѯ��ʱ
    RTS_SEND,                                   //����
    RTS_RECV,                                   //����
    RTS_RETRY,                                  //����
    RTS_RESUME,                                 //�ָ�
    RTS_END,                                    //����
}RELAY_TRANS_STATE_T;

typedef enum
{
    MUD_IDLE = 0,                           //����
    MUD_DELAY,                              //��ʱ
    MUD_READ_FILE_DELAY,
    MUD_OPEN_FILE,                          //�������ļ�
    MUD_READ_FILE,                          //��ȡ�����ļ�
    MUD_SEND,                               //����
    MUD_RECV,                               //����
    MUD_RETRY,                              //����
    MUD_END_AGAIN,                          //��������
    MUD_END,                                //��������
    MUD_GET_RUN_STATE,                      //��ȡ·������״̬
}MODULE_UPDATE_DEAL_T;                      //�ز�ģ������״̬��

typedef enum
{
    NSD_IDLE = 0,                                //����
    NSD_ACT_START_NET_STUDY,                     //��������(AFN=11H-F102, AFN=11H-F101)
    NSD_WAIT_STATE,                              //�ȴ�״̬
    NSD_QUERY_NET_INFO,                          //��ѯ������Ϣ(AFN=10-F4)
    NSD_NET_STUDY_END,                           //��������
    NSD_QUERY_SLAVE_INFO,                        //��ѯ�ӽڵ���Ϣ(AFN=10H-F101)
    NSD_QUERY_SLAVE_INFO_RECV,                   //���մӽڵ���Ϣ״̬
    NSD_END,                                     //����
}NET_STUDY_DEAL_T;                               //΢������������״̬��

typedef enum
{
    DSD_IDLE = 0,                                //����
    DSD_SEND_STATE,                              //����ɾ��ָ��״̬
    DSD_END,                                     //����
}DEL_SLAVE_DEAL_T;                               //ɾ���ӽڵ���Ϣ

typedef enum
{
    SCD_IDLE = 0,                                //����
    SCD_SEND_STATE,                              //�����л��ŵ�״̬
    SCD_DELAY_STATE,                             //��ʱ״̬
    SCD_END,                                     //����
}SWITCH_CHANNEL_DEAL_T;                          //�л��ŵ�

typedef enum
{
    HPLC_QUERY_IDLE = 0,                               //����
    HPLC_QUERY_SEND,                                   //����
    HPLC_QUERY_RECV,                                   //����
    HPLC_QUERY_RETRY,                                  //����
    HPLC_QUERY_END,                                    //����
}HPLC_QUERY_STATE_T;                            //HPLC��Ϣ��ѯ������

typedef enum
{
    CTD_IDLE = 0,                                //����
    CTD_SEND_STATE,                              //����״̬
    CTD_END,                                     //����
}CUS_TASK_DEAL_T;                                //�Զ�������


typedef struct
{
    GET_MODULE_STATE_T          GetManuID;      //��ȡ����ģ����Ϣ
    BREAK_STATE_T               BreakState;     //�ж�״̬
    METER_FILE_MODIFY_STATE_T   MeterFileModify;//������޸�״̬��
    METER_FILE_CMP_STATE_T      MeterFileCmp;   //������ȶ�״̬��
    ADDR_SEARCH_STATE_T         AddrSearch;     //�������
    INNER_RELAY_TASK_STATE_T    RelayTrans;      // �м�����ת��
    INNER_RELAY_TASK_STATE_T    SlaveMonitor;      // �ӽڵ���
    INNER_RELAY_TASK_STATE_T    SlaveMonitorTime;  // ��ȡ�ӽڵ�����ʱ
    ROUTER_STUDY_STATE_T        RouterStudy;    //·��ѧϰ
    RELAY_TRANS_STATE_T         HourAmr;        //���㳭��
    RELAY_TRANS_STATE_T         NormalAmr;      //�ճ�����
    MODULE_UPDATE_DEAL_T        ModuleUpdateDeal; //������������
    NET_STUDY_DEAL_T            NetStudyDeal;   //΢����������������
    DEL_SLAVE_DEAL_T            DelSlaveDeal;   //ɾ���ӽڵ㴦��
    SWITCH_CHANNEL_DEAL_T       SwitchChannel;  //�л��ŵ�����
    HPLC_QUERY_STATE_T          HplcQuery;      //HPLC��Ϣ��ѯ
    CUS_TASK_DEAL_T             CustomTask;     //�Զ�������
}TOTAL_STATE_T;  

typedef struct
{
    uint8   AmrBusyFlag         :1;             //����æ/�б�־
    uint8   AmrFinishFlag       :1;             //��������ʱ����ɱ�־
    uint8   ReserveBit3         :1;             //����
    uint8   ReserveBit4         :1;             //����
    uint8   ReserveBit5         :1;             //����
    uint8   ReserveBit6         :1;             //����
    uint8   ReserveBit7         :1;             //����
    uint8   ReserveBit8         :1;             //����
}AMR_WORK_STATE_BIT_T;

typedef union
{
    AMR_WORK_STATE_BIT_T    BitSect;
    uint8                   Word;
}AMR_WORK_STATE_T;


typedef struct
{
    uint16        SlaveCnt;                     //�ӽڵ�����
    uint16        MaxAllowSlaveCnt;             //�������ӽڵ�����
    uint16        MtrCnt;                       //�������
    uint16        CurMtrIndex;                  //��ǰ�������
    uint8         AcqUnitFailureFlag;           //�ɼ���ʧ�ܱ�־
    uint16        AmrSucceedCnt;                //����ɹ�����
    uint8         OftenPriority;                //Ƶ������ִ��������ȼ�
    AMR_WORK_STATE_T    AmrWorkState;           //������״̬
    uint8   MachineReboot;                      //������������־
    uint8         SucRateState;                 //�ն���ɹ���ͳ��״̬ 0:ȫ����� 1: >=95% 2: <95%
} TmnAmrInfo_T;                                 //�ն˳�����Ϣ

typedef struct
{
    uint16  Pn;                                 //�������
    uint8   SpecialFlag;                        //�����־����2������:
                                                    //1:�·���·�ɰ��־(0:δ���� / 1:������)
                                                    //2:����ɹ���־(0:ʧ�� / 1:�ɹ�)
    uint8   AmrTurnFlag;                        //�����ִ���λ���
    uint8   AdaptFlag;                          //����Ӧ��־(0:����δ���� / 1:�����ѽ���)
    uint8   CurAdaptIndex;                      //����Ӧ��ǰ֡��
    uint8   AcqFlag;
    uint16  LastTaskId;                         //���һ�γ������������ķ�����
    BOOL    IsNeedGetSafeMode;                  //�Ƿ���Ҫ��ȡ�������ȫģʽ����
    uint8   BroadFailCnt;                       //����ʧ�ܼ���

    uint32  nPortID;   //�˿ں�
    //uint16  nMeterFilesID; //�������
    uint8   aMeterFilesAddr[6];  //������ַ
    uint8   aMeterFilesMac[6];  //����MAC
    uint8   nProtocolType;  //Э������
    //uint8   nCommPara;  //ͨ�Ų���
} MtrAmrInfo_T;                                 //�����㳭����Ϣ


typedef struct
{
    TmnAmrInfo_T TmnAmrInfo;                    //�ն˳�����Ϣ
    MtrAmrInfo_T MtrAmrInfo[MAX_AMR_COLLECT_CNT]; //�����㳭����Ϣ
} AmInfo_T;                                     //������Ϣ


typedef enum
{
    FILES_MODEFY_NULL = 0,                           //���������޸�
    FILES_MODEFY_CLEAR,                              //����ȫ��ķ�ʽ
    FILES_MODEFY_UPDATE,                                //�������/ɾ���ķ�ʽ
}METER_FILES_MODEFY_TYPE_E;                                  //�������������

typedef enum
{
    PRTL_NULL = 0,
    PRTL_97,                                    //DLT/645-1997��Լ����
    PRTL_07,                                    //DLT/645-2007��Լ����
    PRTL_OOP,                                   //��������Լ�������
    RRTL_188,                                   //��������Լ188Э��
}PRTL_MODE_T;                                   //��Լ����

typedef enum
{
    METER_NULL = 0,
    METER_PLC,                                  //PLC���
    METER_485,                                  //485���
}MTR_TYPE_T;                                    //�������


typedef struct
{
    uint8    DelayFlag;                         //��֡ʱ��ͨ����ʱ��־
    uint16   DelayCnt;                          //ͨ����ʱʱ��
    MTR_TYPE_T   MtrType;                       //�������(1:plc/2:485)
    uint8   PrtlMode;                           //��Լ����
    uint8   AcqUnitAddr[6];                     //�ɼ�����ַ
    uint8   MtrAddr[6];                         //����ַ
    uint8   CtrlCode;                           //������
    uint8   DataLen;                            //�����򳤶�
    uint8   DataBuff[2048];                      //����������
}MeterPrtlFmt_T;                                //����Լ��ʽ


typedef struct
{
    uint8   MeterAddr[6];                       //����ַ
    uint8   PrtlMode;                           //Э������(1:07/30:07)
    uint8   RealPhase;                          //�����λ(�Ͱ��ֽ�/(0:δ֪/1~3��ӦABC��))/�ź�Ʒ��(�߰��ֽ�)
    uint8   AcqUnitId[6];                       //�ɼ�����ַ
    uint8   ChgFlag;                            //�����־
    uint8   MasterAddr[6];                      //���ڵ��ַ
    uint8   AddrSearchTime[7];                  //����ʱ��
}ACT_REPORT_INFO_T;     //�����ϱ���Ϣ

/*******************************************************************************
* ģ������ö�ٶ���
*******************************************************************************/
typedef enum
{
    MMID_NULL = 0,
    MMID_NOT_NULL,
} ModuleManuID_T;                               //ģ�鳧������

#define AUAM_TWO_ADDR_PRE       0               //˫��ַ,����ַ����������ǰ
#define AUAM_TWO_ADDR_POST      1               //˫��ַ,����ַ�����������
#define AUAM_ONE_ADDR           2               //����ַ,�������ַ�滻376.2��Ŀ�ĵ�ַ


typedef struct
{
    uint8       ValidFlag;                 //1:��Ч; 0:��Ч
    uint8       FrameSeq;                  //F1-01֡���
    uint16      Pn;                        //��֡��Ӧ�Ĳ������
    int         MsIndex;                   //��֡��Ӧ�ķ���MS����
    uint8       PrtlMode;                  //��Ӧ�ı��Լ
    MTR_TYPE_T  Mettype;                   //�ڵ�����
    uint8       MeterAddr[6];              //����ַ
    uint8       AcqAddr[6];                //�ɼ�����ַ
    time_t      CommBeginTime;             //��֡ͨѶ��ʼʱ��
}FRAME_SEQ_INFO_T;                    //֡�����Ϣ

typedef struct
{
    uint8 CurSeqCnt;    //���5֡
    FRAME_SEQ_INFO_T FrameSeqInfo[MAX_NORMAL_AMR_INFO_CNT];
}GLOBAL_SEQ_INFO_T;


typedef struct
{
    uint16  PointNum;  //�㳭������
    uint8   SuccessLabel;  //�㳭�ɹ���־
}GUI_RELAY_PN_T;      //����㳭�Ĳ������


typedef struct
{
    uint8   ValidFlag;  //��1���ֽ�Ϊ�㳭������־0x55��Ч,0xBBΪ�㳭ȡ��̬,����Ϊ0xAA
    uint16  RelayNum;   //��2��3���ֽ�ΪҪ�㳭����Ŀ
    GUI_RELAY_PN_T GuiRelayPn[100];  //��4��5���ֽ���ΪҪ�㳭������,��6Ϊ�㳭�ɹ���־�������ظ���4��5��6���ֽڸ�ʽ
}GUI_RELAY_PN_LIST_T;  //����㳭�Ĳ������


typedef enum
{
    TYPE_NULL = 0,                           //����
    TYPE_SETTERMALADDRESS,                           //�����ն˵�ַ
    TYPE_INFOADDMETERFILES,                           //��ӱ���
    TYPE_INFODELETEMETERFILES,                           //ɾ������
    TYPE_INFOCLEARMETERFILES,                           //�������
    TYPE_INFOCURRENTFILESNUM,                           //��ǰ��������
    TYPE_INFOQUERYFILESDETAIL,                           //��ѯ����
    TYPE_INFOROUTERDEAL_PAUSE,                           //��ͣ����
    TYPE_INFOROUTERDEAL_RECO,                           //�ָ�����
    TYPE_INFOROUTERDEAL_REST,                           //��������
    TYPE_INFOACTIVEMETER,                           //��������
    TYPE_INFOSEARCHMETER,                           //�����ѱ�
    TYPE_INFODATATRANSMIT,                           //����͸��
    TYPE_INFOSLAVEMONITOR,                           //�ӽڵ���
    TYPE_INFOGETBROADCASTDELAY,                           //��ȡ�㲥��ʱ
    TYPE_INFOBROADCASTDEAL,                           //�㲥
}INFO_TASK_TPYE_T;                      //��Ϣ��������

typedef enum
{
    METER_PROTOCOL_NULL = 0,                      //�޷�����
    METER_PROTOCOL_645,                           //645���ĸ�ʽ
    METER_PROTOCOL_69845,                         //698.45���ĸ�ʽ
}METER_PROTOCOL_TPYE_T;                      //���ĸ�ʽ

typedef struct
{
    uint8 HidRead[2040];
    uint8 pinjie;
    uint8 HidWrite[2040];
}COMM_JOIN_MESSAGE_T;      //����ƴ�ӱ�����Ϣ

typedef enum
{
    HPLC_NONE = 0,
    HPLC_TOPOLOGYINFO,
    HPLC_CHIPINFO,
    HPLC_NETSINFO,
    HPLC_PHASEINFO,
    HPLC_SCALE,
    HPLC_MASTERID,
    HPLC_SLAVEID,
    //HPLC_BASEINFO,
    HPLC_MODULEINFO,
    //HPLC_NETNODEINFO,
    HPLC_NODEVERSION,
    HPLC_LAST,
}HPLC_TPYE_E;                               //ɾ���ӽڵ���Ϣ

typedef struct
{
    uint8   topo:1;  //����������Ϣ
    uint8   chip:1;  //����ز�оƬ��Ϣ
    uint8   nets:1;  //��������Ϣ
    uint8   phase:1;  //�ڵ�������Ϣ
    uint8   scale:1;  //�����ģ
    uint8   master:1;  //�������ڵ�ģ��ID��Ϣ
    uint8   slave:1;  //�ӽڵ�ID��Ϣ
    //uint8   base:1;  //���������Ϣ
    uint8   module:1;  //ģ��汾��Ϣ
    //uint8   node:1;  //����ڵ���Ϣ
    uint8   nodeversion:1;  //����ڵ�汾
    uint8   res:7;  //����
} HPLC_TYPE_BIT_T;  //ע��������ö�ٶ�Ӧ

typedef union
{
    HPLC_TYPE_BIT_T BitType;
    uint16 Value;
} HPLC_TYPE_QUERY_UN;


typedef struct
{
    uint8 nFrValue;  //��ǰHPLCƵ��
    uint8 nAreaIdentifyState;  //��ǰHPLC̨��ʶ����״̬
    uint8 nOfflineFeelState;  //��ǰHPLC������֪����״̬
    uint8 nRefuseReportState;  //��ǰHPLC�ܾ��ڵ��ϱ�����״̬
}GUI_SHOW_MESSAGE_T;      //GUI��ʾ���ݽṹ��

typedef struct
{
   uint16  TotalPackageNum;
   uint16  PackageLen;
   uint16  CurrentPackageNo;
}UpdateInfoState;     //�ز�ģ�鱾���������ܼ�¼�ṹ��

typedef struct
{
   uint8   isEnableUpdate; //ģ���Ƿ�֧���������� 00:��֧��  01:֧��
   uint16  packageLen;  //������ ÿ��ģ�����Ҫ��һ��
   uint8   crcMode;         //�Ƿ���ҪУ�� 00:����ҪУ��  01:CRC16У��  ����:����
   uint8   isNeedDeleteDownFile;//�Ƿ���Ҫ�����װ�ļ�  00:������� 01:��Ҫ���
}UpdateSpecialProcess;   //�ز�ģ�鱾������,ģ������⹦�ܴ���

#define MAX_CCO_STA_NUM    500             //CCO��ȫ��STA�������

typedef struct
{
   uint8 aSoftVer[2];//����汾��
   uint8 nVerDay;//�汾���� ��
   uint8 nVerMonth;//�汾���� ��
   uint8 nVerYear;//�汾���� ��
   char aFacCode[2];//ģ�鳧�̴���
   char aChipCode[2];//оƬ����
}SINGLE_CCO_STA_INFO_T;   //��ѯ����ز�ģ��ڵ���Ϣ���ݵ�Ԫ��ʽ


typedef struct
{
   uint16 nCcoStaNum;//�ڵ�������
   uint8 aAddr[MAX_CCO_STA_NUM][6];//�ڵ��ַ
   SINGLE_CCO_STA_INFO_T aInfo[MAX_CCO_STA_NUM];//�ڵ���Ϣ
   uint8 aType[MAX_CCO_STA_NUM];//�ڵ�����
}CCO_STA_INFO_T;   //��ѯ����ز�ģ��ڵ���Ϣ���ݵ�Ԫ��ʽ


#ifdef __cplusplus
}
#endif

#endif // __CCO_REDEF_H__
