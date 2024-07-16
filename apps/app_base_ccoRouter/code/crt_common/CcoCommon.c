/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2019-10-25
*********************************************************************
*/


#include "CcoCommon.h"

#include "CcoDebug.h"
#include "CcoFiles.h"
#include "CcoLib.h"
#include "CcoPrtl.h"
#include "CcoTTL.h"
#include "CcoMain.h"

#include "Mqtt.h"

uint8 gNullArr[100] = {0};//������, ���ڱȶ�

/*******************************************************************************
* ���ض���ͨ�� UDPͨ�Ų��� *********************------<�����޸�>------**********
*******************************************************************************/

//��������------<�����޸�>------
char       *gCcoRouterAppName = "M-ccoRouter";

//���ض˿ں�------<�����޸�>------
uint16      gCcoRouterAppPort = 9012;

//����IP��ַ------<�����޸�>------
char       *gCcoRouterAppIP   = "192.168.9.188";


/*******************************************************************************
* ���ȹ���APP UDPͨ�Ų��� *********************------<�����޸�>------**********
*******************************************************************************/

//���ȹ���APP CTS_ROUTER_APP_NAME------<�����޸�>------
char       *gCtsRouterAppName[MAX_ALLOWED_CTS_THREAD_NUM] = {"M-taskManager", "M-taskManager"};  //���ȹ���APP CTS_ROUTER_APP_NAME

//���ȹ���APP CTS_ROUTER_APP_PORT------<�����޸�>------
uint16      gCtsRouterAppPort[MAX_ALLOWED_CTS_THREAD_NUM] = {9020, 9021};  //���ȹ���APP CTS_ROUTER_APP_PORT

//���ȹ���APP CTS_ROUTER_APP_IP------<�����޸�>------
char       *gCtsRouterAppIP[MAX_ALLOWED_CTS_THREAD_NUM] = {"192.168.9.188", "192.168.9.188"};  //���ȹ���APP CTS_ROUTER_APP_IP


/*******************************************************************************
* ���������ز�ģ�����               *********************------<�����޸�>------**********
*******************************************************************************/

//�˿ں�------<�����޸�>------
//uint32		gPortID[MAX_ALLOWED_DOWN_THREAD_NUM] = {0xf2090201, 0xf2090202};  //�����˿ں�
uint32		gPortID[MAX_ALLOWED_DOWN_THREAD_NUM] = {0, 0};  //�����˿ں�
BOOL        bSetPortIDFlag[MAX_ALLOWED_DOWN_THREAD_NUM] = {0, 0};  //�Ƿ��յ��˿ں����õı�־ 0:ɾ��, 1:����


//ͨ��ģ���豸����------<�����޸�>------
char       *gDevName[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/dev/hidraw0", "/dev/hidraw1"};  //����ͨ���豸(��о)
char       *gDingxinDevName[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/tmp/dev/usb_exm2_1", "/tmp/dev/usb_exm1_0"};  //����ͨ���豸(����)
DEV_HAL_INFO gDevHalInfo[MAX_ALLOWED_DOWN_THREAD_NUM];


/*******************************************************************************
* ˽���ļ�·��
*******************************************************************************/

//����˽���ļ���
char       *gLocalFilesName[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/LocalFilesName_1", "/data/app/ccoRouter/LocalFilesName_2"};  //����˽���ļ�

//ͨ��ģ���ַ˽���ļ���
char       *gModuleManuName[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/ModuleManuName_1", "/data/app/ccoRouter/ModuleManuName_2"};  //����˽���ļ�

//�˿ں���Ϣ˽���ļ���
char       *gPortIDName[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/PortIDName_1", "/data/app/ccoRouter/PortIDName_2"};  //����˽���ļ�

//��ʾ�����Ϣ˽���ļ���
char       *gGuiInfoName[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/GuiInfoName_1", "/data/app/ccoRouter/GuiInfoName_2"};  //����˽���ļ�

/*******************************************************************************
* �ն˵�ַ
*******************************************************************************/
uint8       gTerminalAddr[MAX_ALLOWED_DOWN_THREAD_NUM][6] = {{0}, {0}};  //�ն˵�ַ
BOOL        gUpdateTerminalAddrFlag[MAX_ALLOWED_DOWN_THREAD_NUM] = {FALSE};  //�����ն˵�ַ��־

/*******************************************************************************
* ��������
*******************************************************************************/

PastTimeFlag_T             gPastTimeFlag[MAX_DOWN_THREAD_NUM];                           //��ʱ���־
DateTime_T                 gCurBinTime[MAX_DOWN_THREAD_NUM];                             //��ǰBINʱ��
DateTime_T                 gCurBcdTime[MAX_DOWN_THREAD_NUM];                             //��ǰBCDʱ��
DateTime_T                 gPreBinTime[MAX_DOWN_THREAD_NUM];                             //�ϴ�BINʱ��
DateTime_T                 gCurBcdTimeUp;  //��ǰBCDʱ��(��¼������Ϣ)
DateTime_T                 gCurBcdTimeDown[MAX_DOWN_THREAD_NUM];  //��ǰBCDʱ��(��¼������Ϣ)


//�յ�����Ϣ������
INFO_DATA_LIST_T gInfoDataList = {0};                      //�յ�����Ϣ�������б�

//�����ϱ���ϢINDEX
uint16  ActiveInfoIndex = 0;                                      //�����ϱ���ϢINDEX
extern  pthread_mutex_t    ActiveInfoIndexLock;

//�ϵ��־
int                        gPowerOnFlag[MAX_DOWN_THREAD_NUM];                            //�ϵ��־

//�ϵ��ȡģ����Ϣ
ModuleManuID_T             gModuleManuID[MAX_DOWN_THREAD_NUM];               //ģ�鳧�̱�ʶ
LOCAL_COMM_MODULE_RUNNING_PATTERN_INFO  gLocalCommModuleInfo[MAX_DOWN_THREAD_NUM];         //ģ����Ϣ��ȫ�ֱ���

//�����б�ȫ�ֱ���
AMR_METER_FILES_LIST_T gAmrMeterFilesListTemp[MAX_DOWN_THREAD_NUM];  //�����б���ʱ����(��Ϣ������ʱ����)
AMR_METER_FILES_LIST_T gAmrMeterFilesList[MAX_DOWN_THREAD_NUM];  //�����б�ȫ�ֱ���
AMR_METER_FILES_LIST_T gToAddFilesList[MAX_DOWN_THREAD_NUM];  //��Ҫ��ӵĵ����б�ȫ�ֱ���
AMR_METER_FILES_LIST_T gToDeleteFilesList[MAX_DOWN_THREAD_NUM];  //��Ҫɾ���ĵ����б�ȫ�ֱ���

uint8                     gMeterFileAddDelFlagTemp[MAX_DOWN_THREAD_NUM] = {0};       //��������ɾ����־(��Ϣ������ʱ����)
uint8                     gMeterFileClearFlagTemp[MAX_DOWN_THREAD_NUM] = {0};        //���������־(��Ϣ������ʱ����)

uint8                     gMeterFileAddDelFlag[MAX_DOWN_THREAD_NUM] = {0};       //��������ɾ����־
uint8                     gMeterFileClearFlag[MAX_DOWN_THREAD_NUM] = {0};        //���������־
uint8                     gMeterFileCompareFlag[MAX_DOWN_THREAD_NUM] = {0};      //�����ȶԱ�־
uint8                     gMeterFileModifyFlag[MAX_DOWN_THREAD_NUM] = {0};       //�����޸ı�־
uint8                     gMeterFileModifyState[MAX_DOWN_THREAD_NUM] = {0};      //�����޸�״̬(����cmp��modify��������)��0�������꣬1��������
METER_FILES_MODEFY_TYPE_E gMeterFilesModifyType[MAX_DOWN_THREAD_NUM] = {0};      //�����޸ķ�ʽ

//��ų�����Ϣ�Ļ�����
AMR_ACTIVE_INFO_LIST_T gActiveAmrInfoList[MAX_DOWN_THREAD_NUM];  //��ų�����Ϣ�Ļ�����

//�ز��������ʹ�õ�ȫ�ֱ���
PLCTASK_QUEUE              PlcTaskQueue[MAX_DOWN_THREAD_NUM];  //�ز��������
PLCTASK_DETAIL             gPlcTaskDetail[MAX_DOWN_THREAD_NUM];  //��ǰ�ز���������ϸ��
PLCTASK_DETAIL             gPlcTaskDetailSearchMeter[MAX_DOWN_THREAD_NUM];  //��ǰ�ز���������ϸ��(�ѱ�)

//�����������ڲ�ʹ�õ�ȫ�ֱ���
TOTAL_STATE_T              gTotalState[MAX_DOWN_THREAD_NUM];                             //��״̬��

//���������Ϣ
AmInfo_T    gAmrInfo[MAX_DOWN_THREAD_NUM];			                //���������Ϣ

//plc��λ���ź�ģ��������ż���ļ�������
int fd_plc[MAX_DOWN_THREAD_NUM];//plc��λ���ź�ģ��������ż���ļ�������

//����ճ��������
GLOBAL_SEQ_INFO_T gGlobalSeqInfo[MAX_DOWN_THREAD_NUM];

//·�ɿ��Ʋ���
ROUTER_CTRL_STATE_T   gRoutrtCtrl[MAX_DOWN_THREAD_NUM] = {RCS_NULL};           //·�ɿ��Ʋ���

//·�ɿ��Ʋ���
ROUTER_OPER_DETAIL_T  gRouterOper[MAX_DOWN_THREAD_NUM];           //·�ɿ��Ʋ���

//�����ѱ��־
WHETHER_START_SEARCH_METER_T gWhetherStartSearchMeter[MAX_DOWN_THREAD_NUM];       //�����ѱ��־

//��ͣ�ѱ��־
BOOL gPauseSearchMeterFlag[MAX_DOWN_THREAD_NUM] = {FALSE};       //��ͣ�ѱ��־

//�ѱ������(��0��ʼ)
uint16 gSearchMeterFilesIndex[MAX_DOWN_THREAD_NUM] = {0};

//��͸�������
AMR_DATA_TRANSMIT_LIST_T gDataTransmitQueue[MAX_DOWN_THREAD_NUM];

//��ȡ�ӽڵ�����ʱ�������
AMR_SLAVE_MONITOR_TIME_LIST_T gSlaveMonitorTimeQueue[MAX_DOWN_THREAD_NUM];

//�ӽڵ����������
AMR_SLAVE_MONITOR_LIST_T gSlaveMonitorQueue[MAX_DOWN_THREAD_NUM];

//�Ƿ��ȡ�㲥��ʱ
WHETHER_GET_BROADCAST_DELAY_T gWhetherGetBroadcastDelay[MAX_DOWN_THREAD_NUM];

//�Ƿ�㲥
WHETHER_DO_BROADCAST_T gWhetherDoBroadcast[MAX_DOWN_THREAD_NUM];

//��ѯHPLC��Ϣ ��Ϣ����
HPLC_TYPE_QUERY_UN gQueryTypeHPLC[MAX_DOWN_THREAD_NUM];  //ÿ��bit��Ӧ��ͬ��HPLC��ѯ����

//5��ģ�����Ϣ
BOOL gQueryAllModuleFlag = TRUE;  //���������ʱ, ��ȡ����ģ����Ϣ
MODULE_INFO_T gModuleInfo[5];  //5��ģ�����Ϣ
uint8 gReloadModuleFlag[6] = {0};  //��λ��0��ʾ�ޱ仯, 1��ʾ�仯, ����5��0/1��ʾģ��仯״̬

//�������һ���Ƿ�CCOģ����
uint8 gCcoFlag[5] = {0};  //�������һ���Ƿ�CCOģ����

//HPLCƵ�����ñ�־
uint8 gFrSetFlag[MAX_DOWN_THREAD_NUM] = {0};  //HPLCƵ�����ñ�־
//HPLCƵ������ֵ
uint8 gFrSetValue[MAX_DOWN_THREAD_NUM] = {0};  //HPLCƵ������ֵ

//HPLCƵ���ѯ��־
uint8 gFrQueryFlag[MAX_DOWN_THREAD_NUM] = {0};  //HPLCƵ���ѯ��־
INFO_DATA_HEAD_T gFrQueryInfoDataHead[MAX_DOWN_THREAD_NUM];  //HPLCƵ���ѯ��ϢԭʼHEAD
uint8 gInitFrQueryFlag[MAX_DOWN_THREAD_NUM] = {0};  //HPLCƵ���ѯ��־(�ϵ��ʼ����ѯ)

//��ѯ����״̬�ṹ��
AMR_QUERY_READING_STATE_T gAmrStateQuery[MAX_DOWN_THREAD_NUM];  //��ѯ����״̬�ṹ��

//��ѯ����ģ����Ϣ�ṹ��
AMR_QUERY_MODULE_INFO_T gAmrModuleQuery[MAX_DOWN_THREAD_NUM];  //��ѯ����ģ����Ϣ�ṹ��

//HPLC̨��ʶ���־
uint8 gAreaIdentifyFlag[MAX_DOWN_THREAD_NUM] = {0};  //HPLC̨��ʶ���־
//HPLC̨��ʶ��ֵ
uint8 gAreaIdentifyValue[MAX_DOWN_THREAD_NUM] = {0};  //HPLC̨��ʶ��ֵ

//HPLC������֪��־
uint8 gOfflineFeelFlag[MAX_DOWN_THREAD_NUM] = {0};  //HPLC������֪��־
//HPLC������ֵ֪
uint8 gOfflineFeelValue[MAX_DOWN_THREAD_NUM] = {0};  //HPLC������ֵ֪

//HPLC�ܾ��ڵ��ϱ���־
uint8 gRefuseNodeReportFlag[MAX_DOWN_THREAD_NUM] = {0};  //HPLC�ܾ��ڵ��ϱ���־
//HPLC�ܾ��ڵ��ϱ�����
uint8 gRefuseNodeReportValue[MAX_DOWN_THREAD_NUM] = {0};  //HPLC�ܾ��ڵ��ϱ�����

//ģ�������»�ȡ�豸
uint8 gReloadDevFlag[MAX_DOWN_THREAD_NUM] = {0};       //ģ�������»�ȡ�豸

//guiչʾ����
GUI_SHOW_MESSAGE_T gGuiShowMessage[MAX_DOWN_THREAD_NUM];  //guiչʾ����

//�����ļ���ʶȫ�ֱ��
uint8 gPlcUpdateFileMark[MAX_DOWN_THREAD_NUM] = {0};  //�����ļ���ʶȫ�ֱ��. 0x03:���ڵ������ļ�; 0x07:���ڵ�+ȫ���ӽڵ�;0x08:ȫ���ӽڵ�

//CCO���������ļ�·��
char *gRouterUpdateFile[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/router_update_file_1", "/data/app/ccoRouter/router_update_file_2"};  //���ڵ������� _1 ��һ��ccoģ��, _2 �ڶ���ccoģ��
char *gRouterUpdateFileBack[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/router_update_file_back_1", "/data/app/ccoRouter/router_update_file_back_2"};  //���ڵ�������
char *gAllRouterUpdateFile[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/all_router_update_file_1", "/data/app/ccoRouter/all_router_update_file_2"};  //���ڵ�+ȫ���ӽڵ����� _1 ��һ��ccoģ��, _2 �ڶ���ccoģ��
char *gAllRouterUpdateFileBack[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/all_router_update_file_back_1", "/data/app/ccoRouter/all_router_update_file_back_2"};  //���ڵ�+ȫ���ӽڵ�����
char *gSubRouterUpdateFile[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/sub_router_update_file_1", "/data/app/ccoRouter/sub_router_update_file_2"};  //ȫ���ӽڵ����� _1 ��һ��ccoģ��, _2 �ڶ���ccoģ��
char *gSubRouterUpdateFileBack[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/sub_router_update_file_back_1", "/data/app/ccoRouter/sub_router_update_file_back_2"};  //ȫ���ӽڵ�����

//��ѯ����ز�ģ��ڵ���Ϣ���ݵ�Ԫ��ʽ
CCO_STA_INFO_T gAllCcoStaInfo[MAX_DOWN_THREAD_NUM];

//�ļ������ӿ����
uint8  gIsUpdating = 0;     //��ǰ�Ƿ���������
uint8  gIsNeedUpdating = 0;     //�Ƿ�����������
char gFilePath[200] ={0};       //�ļ�·��
uint8 gFileFlag;        //�ļ���ʶ
uint8 gUpdateState;  //����״̬ 0δ���� 1������ 2 ��������
uint32 gUpdateBeginTime;
uint32 gUpdateEndTime;
uint8 gUpdateResult;        //������� 0�������ɹ���1�������ļ��쳣��2����������ʧ�ܣ�255�������쳣

const PLCTASK_FUNINFO PlcTask_FunList[] =
{
    {1, MS_UPDATE_DEAL,      "CCO�����ļ�����",          0,      RCS_PAUSE,      10,       ModuleUpdateDeal},                //�ز�������������
    {1, MS_GET_MANU_ID,      "��ȡ���̱�ʶ",             1,      RCS_NULL,       0,        GetModuleManuID},                 //��ȡ���̱�ʶ
    {1, MS_ROUTER_DEAL,      "·�ɲ���",               2,      RCS_NULL,       10,       AppRouterDealFun},                //·�ɲ���
    {1, MS_FILE_CMP,         "�����ȶ�",               3,      RCS_PAUSE,      15,       MeterFileCmpDeal},                //������ȶ�
    {1, MS_FILE_MODIFY,      "�����޸�",               4,      RCS_PAUSE,      15,       MeterFileMofifyDeal},             //������޸Ĵ���
    {1, MS_SLAVE_MONITOR,    "�ӽڵ���",              5,      RCS_PAUSE,      3,        SlaveMonitorDeal},                //�ӽڵ���
    {1, MS_RELAY_TRANS,      "͸��ת��",               6,      RCS_PAUSE,      3,        RelayTransDeal},                  //���͸��ת��    
    {1, MS_ADDR_SEARCH,      "�������",               7,      RCS_PAUSE,      60,       AddrSearchDeal},                  //���������
    {1, MS_SLAVE_DELAY,      "��ȡ�ӽڵ�����ʱ", 8,               RCS_PAUSE,      3,        GetSlaveMonitorDelay},            //��ȡ�ӽڵ�����ʱ
    {1, MS_BROADCAST_DELAY,  "��ȡ�㲥��ʱ",             10,     RCS_PAUSE,      3,        GetBroadcastDelay},               //��ȡ�㲥��ʱ
    {1, MS_BROADCAST_DEAL,   "�㲥",                 11,     RCS_PAUSE,      3,        BroadcastDeal},                   //�㲥
    {1, MS_HPLC_REPORT,      "HPLC��Ϣ�ϱ�",           13,     RCS_NULL,        0,       HplcInfoReport},                  //HPLC��Ϣ�ϱ�
    {1, MS_HPLC_FR_SET,      "HPLCƵ������",           14,     RCS_NULL,        0,       HplcFrSet},                       //HPLCƵ������
    {1, MS_HPLC_FR_QUERY,    "HPLCƵ���ѯ",           15,     RCS_NULL,        0,       HplcFrQuery},                     //HPLCƵ���ѯ
    {1, MS_HPLC_AREA,        "HPLC̨��ʶ��",           16,     RCS_NULL,        0,       HplcAreaIdentify},                //HPLC̨��ʶ��
    {1, MS_HPLC_OFFLINE,     "HPLC������֪",           17,     RCS_NULL,        0,       HplcOfflineFeel},                 //HPLC������֪
    {1, MS_HPLC_REFUSE,      "HPLC�ܾ��ڵ��ϱ�",         18,     RCS_NULL,        0,       HplcRefuseNodeReport},            //HPLC�ܾ��ڵ��ϱ�
    {1, MS_NORMAL_AMR,       "��������",               20,     RCS_RESTART,    60,       NormalDataReading},               //��������
    {1, 0xFF,                "", 0xFF,   RCS_NULL,       60,       NULL},
};

extern uint32 g_CcoProduct;

/*******************************************************************************
* ��������: GetInfoIndex
* ��������: ��ȡ��ǰ�����ϱ���Ϣ��index
* �������: ��
* �������: ��
* �� �� ֵ: �����ϱ���Ϣ��index
*******************************************************************************/
uint16 GetInfoIndex()
{
    uint16 index =0 ;
    pthread_mutex_lock(&ActiveInfoIndexLock);
    index = ActiveInfoIndex++;
    pthread_mutex_unlock(&ActiveInfoIndexLock);
    return index;
}
#if 0
/*******************************************************************************
* ��������: IsExistPlcUpDateFile
* ��������: �����ж��Ƿ�����ز�·�������ļ�
* �������: ģ���߳�����(0:�����һ��CCOģ��; 1:����ڶ���CCOģ��)
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0               ʧ��
*******************************************************************************/
int IsExistPlcUpDateFile(uint8 nArrIndex)
{
    char TmpPath[256] = {0};
    sprintf(TmpPath,"rm -rf %s", gRouterUpdateFileBack[nArrIndex]);

    if(access(gRouterUpdateFile[nArrIndex],R_OK) == 0)    //���ж����ڵ�������
    {
        //������� ROUTER_UPDATE_FILE_BACK �ļ��Ƚ���ɾ��
        if(access(gRouterUpdateFileBack[nArrIndex],R_OK) == 0)
        {
            system(TmpPath);
            printf("\n%s\n",TmpPath);
        }

        if(rename(gRouterUpdateFile[nArrIndex], gRouterUpdateFileBack[nArrIndex]) < 0)
        {
            return -1;
        }
        else
        {
            gPlcUpdateFileMark[nArrIndex] = 0x03;
            return 1;
        }

        return 1;
    }
    else if(access(gAllRouterUpdateFile[nArrIndex],R_OK) == 0)    //���ж����ڵ�+�ӽڵ�������
    {
        if(access(gAllRouterUpdateFileBack[nArrIndex],R_OK) == 0)
        {
            sprintf(TmpPath,"rm -rf %s", gAllRouterUpdateFileBack[nArrIndex]);
            system(TmpPath);
            printf("\n%s\n",TmpPath);
        }

        if(rename(gAllRouterUpdateFile[nArrIndex], gAllRouterUpdateFileBack[nArrIndex]) < 0)
        {
            return -1;
        }
        else
        {
            gPlcUpdateFileMark[nArrIndex] = 0x07;
            return 1;
        }

        return 1;
    }
    else if(access(gSubRouterUpdateFile[nArrIndex],R_OK) == 0)   //���ж�ȫ���ӽڵ�������
    {
        if(access(gSubRouterUpdateFileBack[nArrIndex],R_OK) == 0)
        {
            sprintf(TmpPath,"rm -rf %s", gSubRouterUpdateFileBack[nArrIndex]);
            system(TmpPath);
            printf("\n%s\n",TmpPath);
        }

        if(rename(gSubRouterUpdateFile[nArrIndex], gSubRouterUpdateFileBack[nArrIndex]) < 0)
        {
            return -1;
        }
        else
        {
            gPlcUpdateFileMark[nArrIndex] = 0x08;
            return 1;
        }

        return 1;
    }
    
    return -1;
}

/*******************************************************************************
* ��������: GetUpdateFileName
* ��������: ���������ļ���ʶ��ȡ�������ļ���
* �������: ģ���߳�����(0:�����һ��CCOģ��; 1:����ڶ���CCOģ��)
* �������: �������ļ���
* �� �� ֵ: ��
*******************************************************************************/
void GetUpdateFileName(char *TmpPath, uint8 nArrIndex)
{
    if(gPlcUpdateFileMark[nArrIndex] == 0x03)
        sprintf(TmpPath, "%s", gRouterUpdateFileBack[nArrIndex]);
    else if(gPlcUpdateFileMark[nArrIndex] == 0x07)
        sprintf(TmpPath, "%s", gAllRouterUpdateFileBack[nArrIndex]);
    else if(gPlcUpdateFileMark[nArrIndex] == 0x08)
        sprintf(TmpPath, "%s", gSubRouterUpdateFileBack[nArrIndex]);
    return;
}
#endif
/*******************************************************************************
* ��������:	UpdateFileSpecialDeal
* ��������:	�����ļ��������
* �������:	ģ���߳�����(0:�����һ��CCOģ��; 1:����ڶ���CCOģ��)
* �������:	��
* �� �� ֵ:	UpdateSpecialProcess�ṹ������
*******************************************************************************/
UpdateSpecialProcess UpdateFileSpecialDeal(uint8 nArrIndex)
{
    UpdateSpecialProcess tmpUpdateSpecialProcess;
    tmpUpdateSpecialProcess.isEnableUpdate 	     = 0x01;
	if(MMID_NULL == gModuleManuID[nArrIndex])
		tmpUpdateSpecialProcess.packageLen       = 128;
	else
    {
        tmpUpdateSpecialProcess.packageLen = gLocalCommModuleInfo[nArrIndex].FileTransferMaxSolePackageLen;
        //����ز�ģ���ϱ����������ĵ��������Ǵ���512�ֽڵģ�Ĭ��Ϊ128
        if(gLocalCommModuleInfo[nArrIndex].FileTransferMaxSolePackageLen > 512)
            tmpUpdateSpecialProcess.packageLen 		 = 128;
    }

	tmpUpdateSpecialProcess.crcMode 		     = 0x00;
    tmpUpdateSpecialProcess.isNeedDeleteDownFile = 0x00;

    return tmpUpdateSpecialProcess;
}

/*******************************************************************************
* ��������: GetUpdateInfo
* ��������: �������ļ�����Ԥ����ȡ��UpdateInfoState�ṹ����Ϣ
* �������: const char *Path              �ļ�·��
            uint16 PackageLen         ������
            uint8 nArrIndex           ģ���߳�����(0:�����һ��CCOģ��; 1:����ڶ���CCOģ��)
* �������: UpdateInfoState *uifo     �����ļ��ṹ��
* �� �� ֵ: >0                  �ɹ�
*           <0               ʧ��
*******************************************************************************/
int GetUpdateInfo(const char *Path, UpdateInfoState *uifo, uint16 PackageLen, uint8 nArrIndex)
{
    int fd = -1;
    int size = 0;
    uint8 buffer[1024];
    uint16 package_len = PackageLen;
    uint32 package_no = 0;

    memset(buffer,0x00,sizeof(buffer));
    fd=open(Path,O_RDONLY);
    if(fd <= 0)
    {
        CCODOWN_FMT_DEBUG(nArrIndex, "GetUpdateInfo() open file error!\n");
        return -1;
    }

    while(size >= 0)
    {
        size = read(fd,buffer,package_len);

        if(0 == size)
        {
            break;
        }

        package_no++;
        memset(buffer,0x00,sizeof(buffer));

    }

    uifo->TotalPackageNum = package_no;//�ܵİ�����
    uifo->CurrentPackageNo = 0;//��ǰ�����
    uifo->PackageLen = package_len;//����������

    package_no=0;
    memset(buffer,0x00,sizeof(buffer));
    close(fd);

    return 1;
}

/*******************************************************************************
* ��������: PackUpdateFameAndSend
* ��������: �ز�ģ��������֡�����ҷ��͵�������
* �������: uint16 Len                ���ĳ�����ֵ
            UpdateInfoState Uis   �������̼�¼
            uint8 nArrIndex       ģ���߳�����(0:�����һ��CCOģ��; 1:����ڶ���CCOģ��)
* �������: uint8 *Buffer             ���ͱ���
* �� �� ֵ: >0                       ���ĳ���
*           <0                    ������֡ʧ��
*******************************************************************************/
uint16 PackUpdateFameAndSend(uint8 *Buffer, uint16 Len, UpdateInfoState Uis, uint8 nArrIndex)
{
    AppData_T   AppData;
    uint8 FileIdentifying = gFileFlag;       //0x03:���ڵ�;0x07:���ڵ�+�ӽڵ�;0x08:�ӽڵ�.
    uint8 FileAttribute = 0x00;      //0x00:��ʼ֡���м�֡, 0x01:����֡
    uint8 FileCommand = 0x00;                       //���ķ�ʽ��װ
    uint16 TotalPackageNum = Uis.TotalPackageNum;//�ܶ������ܰ���
    uint32 PackageNo = Uis.CurrentPackageNo;     //�κţ�����
    int PackageLen = Len;                       //�γ��ȣ�������

    UpdateSpecialProcess specialDeal = UpdateFileSpecialDeal(nArrIndex);//��ͬģ�����⴦��,Ӧ�ø�������ȥ����

    if(PackageNo == (TotalPackageNum-1))
    {
        FileAttribute = 0x01;//����֡
    }

    if(specialDeal.isNeedDeleteDownFile == 0x01)//����ǰ�����װ�ļ�
    {
        //��һ֡���ļ���ʶ����Ϊ0x00,·�ɽ����Լ��õ��������ļ������
    }
    if(gFileFlag ==0)
        FileIdentifying = 3;
    else if(gFileFlag ==1)
        FileIdentifying = 7;
    else if(gFileFlag==2)
        FileIdentifying = 8;
    AppData.AFN = 0x15;//AFN 0x15
    AppData.Fn = 1; //FN  0x01

    AppData.DataUnitLen = 11+Len;
    AppData.DataUnitBuff[0] = FileIdentifying;//�ļ���ʶ
    AppData.DataUnitBuff[1] = FileAttribute;//�ļ�����
    AppData.DataUnitBuff[2] = FileCommand;//�ļ�ָ��
    memcpy(&(AppData.DataUnitBuff[3]),(uint8 *)&TotalPackageNum, sizeof(TotalPackageNum));//�ܶ���
    memcpy(&(AppData.DataUnitBuff[5]),(uint8 *)&PackageNo, sizeof(PackageNo));//��i�α�ʶ

    if(specialDeal.crcMode == 0x01)//��Ҫcrc16У��
    {
        int crc = crc16(Buffer, PackageLen);    //����CRC
        CCODOWN_FMT_DEBUG(nArrIndex, "��ǰ����crc ==0x%04x\n", crc);
        int tmpLen = PackageLen+2;//ʵ�ʷ��͵ĳ���Ҫ��2�ֽڵ�CRC
        memcpy(&(AppData.DataUnitBuff[9]),(uint8 *)&tmpLen, sizeof(tmpLen));
        memcpy(&(AppData.DataUnitBuff[11]), Buffer, PackageLen);//����������
        AppData.DataUnitBuff[11+PackageLen] =(uint8) (crc&0x00ff);//����У��
        AppData.DataUnitBuff[11+PackageLen+1] = crc>>8;    //����У��
        AppData.DataUnitLen += 2;
    }
    else
    {
        memcpy(&(AppData.DataUnitBuff[9]),(uint8 *)&PackageLen, sizeof(PackageLen));//��i�γ���
        memcpy(&(AppData.DataUnitBuff[11]), Buffer, PackageLen);//����
        CCODOWN_FMT_DEBUG(nArrIndex, "PackUpdateFameAndSend() �ļ���ʶ=%02x(00�����װ�ļ�, 03���ڵ�����, 07���ӽڵ�����, 08�ӽڵ�����), �ļ�����=%02x(00��ʼ֡, 01����֡), �ļ�ָ��=%02x(00���ķ�ʽ), �ܶ���=%d, ��ǰ�α�ʶ=%u, ��ǰ�����ݳ���=%d \n", 
            FileIdentifying, FileAttribute, FileCommand, TotalPackageNum, PackageNo, PackageLen);
    }

    return PackFrameData(AppData, nArrIndex);
}


/*******************************************************************************
* ��������: ModuleUpdateDeal
* ��������: �ز�ģ�鱾����������
* �������: TASK_FUN_INPUT stTaskFunInput   �ز����������Ϣ
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0               ʧ��
*******************************************************************************/
int ModuleUpdateDeal(TASK_FUN_INPUT stTaskFunInput)
{
    static time_t       CommBeginTime[MAX_DOWN_THREAD_NUM] = {0};              //ͨѶ��ʼʱ��
    static int          CommRetryCnt[MAX_DOWN_THREAD_NUM] = {0};           //ͨѶ���Դ���
    int                 RecvLen;                            //�������ݳ���
    uint8               RecvBuf[MAX_QUEUE_BUFF_LEN] = {0};        //�������ݻ�����
    AppData_T           AppData;                        //Ӧ�ò�����
    int                 RecvResult = 1;                     //���ս��(1:�ɹ� / -1:ʧ��)
    static int          WaitTime[MAX_DOWN_THREAD_NUM] = {0};
    int                 ret = -1;
    uint32              Temp = 0;
    //static   int        PauseDelayCnt[MAX_DOWN_THREAD_NUM] = {0};     //��ͣ��ʱ������
    static int          Fd[MAX_DOWN_THREAD_NUM];               //�ļ���ʶ��
    static int          Size[MAX_DOWN_THREAD_NUM] = {0};              //ÿ�δ��ļ��ж����Ĵ�С
    static uint8        Buffer[MAX_DOWN_THREAD_NUM][1024];          //���ļ�����Ļ�����
    static uint16       PackageLen[MAX_DOWN_THREAD_NUM] = {0};     //ÿһ�Σ��γ�(�ְ�����)
    //static int          PackageNo[MAX_DOWN_THREAD_NUM] = {0};         //�α�ţ��ְ���
    static char         TmpPath[MAX_DOWN_THREAD_NUM][256];          //shell�������
    static int          TotalRetryCnt[MAX_DOWN_THREAD_NUM] = {0};     //�������Դ���ͳ��
    static int          UpdateSuccessFlag[MAX_DOWN_THREAD_NUM] = {0}; //��������ȫ����ɱ��
    //static uint8        UpdataStat[MAX_DOWN_THREAD_NUM] = {0};        //��֪GUI������״̬
    static UpdateInfoState Uis[MAX_DOWN_THREAD_NUM];                //�����ļ��ṹ��

    AppData_T           RecvAppData = {0};          //���ջ�����

    uint8           nTtlIndex = 0;                      //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;
    
    switch (gTotalState[nTtlIndex].ModuleUpdateDeal)
    {
    case MUD_IDLE:
        CCODOWN_FMT_DEBUG(nTtlIndex, "ModuleUpdateDeal()�ز�ģ�鱾����������...MUD_IDLE \n");
        gUpdateBeginTime = time(NULL);
        gUpdateEndTime = 0;
        gUpdateState = 1;
        //������ʼ��
        WaitTime[nTtlIndex] = 0;
        Fd[nTtlIndex] = -1;
        memset(Buffer[nTtlIndex], 0x00, sizeof(Buffer[nTtlIndex]));
        memset(TmpPath[nTtlIndex], 0x00, sizeof(TmpPath[nTtlIndex]));

        memset(&Uis[nTtlIndex], 0x00, sizeof(Uis[nTtlIndex]));

        ClearCommRecvBuff(nTtlIndex);
        
        UpdateSpecialProcess specialDeal = UpdateFileSpecialDeal(nTtlIndex);//��ͬģ�����⴦��,Ӧ�ø�������ȥ����

        PackageLen[nTtlIndex] = specialDeal.packageLen;//ÿһ���ĳ���
        gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_READ_FILE_DELAY;
        CCODOWN_FMT_DEBUG(nTtlIndex, "������������ز�ģ����������!\n");

        //UpdataStat[nTtlIndex] = 0x55;//��������
        break;
    case MUD_READ_FILE_DELAY:
        ret = GetUpdateInfo(gFilePath, &Uis[nTtlIndex], PackageLen[nTtlIndex], nTtlIndex);
        if(ret < 0)
        {
            gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_END_AGAIN;
            CCODOWN_FMT_DEBUG(nTtlIndex, "GetUpdateInfo()����, ֱ�ӽ���!\n");
            break;
        }

        gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_OPEN_FILE;
        CCODOWN_FMT_DEBUG(nTtlIndex, "��������=%d, �ܵİ�����=%d\n", PackageLen[nTtlIndex], Uis[nTtlIndex].TotalPackageNum);
        break;

    case MUD_OPEN_FILE:
        Fd[nTtlIndex] = open(gFilePath,O_RDONLY);
        CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_OPEN_FILE: Fd=%d \n", Fd[nTtlIndex]);
        if(Fd[nTtlIndex] <= 0)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_OPEN_FILE: MUD_END_AGAIN \n");
            gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_END_AGAIN;
            UpdateSuccessFlag[nTtlIndex] = 2;   //��ʶ�����ļ��쳣
            break;
        }

        CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_OPEN_FILE: MUD_READ_FILE \n");
        gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_READ_FILE;
        break;

    case MUD_READ_FILE:
        memset(Buffer[nTtlIndex], 0x00, sizeof(Buffer[nTtlIndex]));
        Size[nTtlIndex] = read(Fd[nTtlIndex], Buffer[nTtlIndex], PackageLen[nTtlIndex]);
        if(Size[nTtlIndex] < 0)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_READ_FILE: Size=%d \n", Size[nTtlIndex]);
            gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_END_AGAIN;
        }
        else
        {
            gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_SEND;
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_READ_FILE: Size=%d, package_no:%d, package_len:%d \n", Size[nTtlIndex], Uis[nTtlIndex].CurrentPackageNo, PackageLen[nTtlIndex]);
        }

        //���¶���һ֡ʱ�������Դ�����0
        CommRetryCnt[nTtlIndex] = 0;
        break;

    case MUD_SEND:
        PackUpdateFameAndSend(Buffer[nTtlIndex], Size[nTtlIndex], Uis[nTtlIndex], nTtlIndex);
        //ˢ�·���ʱ��
        CommBeginTime[nTtlIndex] = time(NULL);
        //���ݷ�����ɣ��ȴ�����
        gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_RECV;
        break;

    case MUD_RECV:                         //����
        RecvLen = PlcReadQueue(pAmrRecvDown[nTtlIndex], RecvBuf);
        if (RecvLen > 0)
        {
            if (UnPackFrameData(RecvLen, RecvBuf, &AppData) != CHECK_RESULT_SUCCEED)
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "if (UnPackFrameData() != CHECK_RESULT_SUCCEED) \n");
                RecvResult = -1;
            }

            //��15-01��֡�������ȴ���ʱ
            if((0x15 != AppData.AFN) || (0x01 != AppData.Fn))
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "if((0x15 != AppData.AFN) || (0x01 != AppData.Fn)) \n");
                break;
            }

            memcpy((uint8 *)&Temp,AppData.DataUnitBuff,AppData.DataUnitLen);
            if((Temp != Uis[nTtlIndex].CurrentPackageNo) || (0xffffffff == Temp))
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "if((Temp != Uis[nTtlIndex].CurrentPackageNo) || (0xffffffff == Temp)) \n");
                RecvResult = -1;
            }

            if(RecvResult != -1)
            {
                //UpdataStat[nTtlIndex] = 0xA5;
                Uis[nTtlIndex].CurrentPackageNo++;

                gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_READ_FILE;
                if(Uis[nTtlIndex].CurrentPackageNo >= Uis[nTtlIndex].TotalPackageNum)
                {
                    CCODOWN_FMT_DEBUG(nTtlIndex, "if( Uis.CurrentPackageNo(=%d) >= Uis.TotalPackageNum(=%d) ) \n", 
                        Uis[nTtlIndex].CurrentPackageNo, Uis[nTtlIndex].TotalPackageNum);
                    if(gFileFlag == 0x03)  //��ģ�������ֱ�ӽ���
                    {
                        gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_END;
                        UpdateSuccessFlag[nTtlIndex] = 1;
                        CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_RECV:��ģ������ֱ�ӽ���! \n");
                    }
                    else                            //���ڵ�+�ӽڵ�,�ӽڵ���������ѯ·������״̬
                    {
                        gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_GET_RUN_STATE;
                        CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_RECV:����/�ӽڵ�������Ҫ��ѯ·��״̬! \n");
                    }
                }

                break;
            }

        }
        else
        {
            if (abs(time(NULL) - CommBeginTime[nTtlIndex]) > MAX_COMM_DELAY_TIME)
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_RECV: ��ʱ! \n");
                RecvResult = -1;
            }
        }

        //����ʧ��
        if (RecvResult == -1)
        {
            //���Դ���+1
            CommRetryCnt[nTtlIndex]++;

            //�л�������״̬
            gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_RETRY;
        }
        break;

    case MUD_RETRY:                        //����
        //if (CommRetryCnt <= MAX_COMM_RETRY_CNT)
        if (CommRetryCnt[nTtlIndex] <= 4)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_RETRY: ����! CommRetryCnt[nTtlIndex]=%d \n", CommRetryCnt[nTtlIndex]);
            gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_SEND;
        }
        else
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_RETRY: �ﵽ������Դ���, ֱ�ӽ���! CommRetryCnt[nTtlIndex]=%d \n", CommRetryCnt[nTtlIndex]);
            gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_END_AGAIN;
        }

        break;

    case MUD_END_AGAIN:

        close(Fd[nTtlIndex]);
        Fd[nTtlIndex] = -1;

        //PauseDelayCnt[nTtlIndex] = 0;
        Size[nTtlIndex] = 0;
        memset(Buffer[nTtlIndex],0x00,sizeof(Buffer[nTtlIndex]));
        //PackageNo[nTtlIndex] = 0;
        memset((uint8 *)&Uis[nTtlIndex],0x00,sizeof(Uis[nTtlIndex]));
        memset(TmpPath[nTtlIndex],0x00,sizeof(TmpPath[nTtlIndex]));
        CommRetryCnt[nTtlIndex] = 0;

        TotalRetryCnt[nTtlIndex]++;
        if(TotalRetryCnt[nTtlIndex] <= 3)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_END_AGAIN: MUD_IDLE\n");
            gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_IDLE;
        }
        else
        {
            TotalRetryCnt[nTtlIndex] = 0;
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_END_AGAIN: MUD_END\n");
            gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_END;
        }

        break;
    case MUD_GET_RUN_STATE:
        //��ʱ20s,��ѯ·������״̬
        if(IsCloseInnerDelayState((gPastTimeFlag[nTtlIndex].BitSect.Second == TRUE),&WaitTime[nTtlIndex],20) <= 0)
            break;

        //���ѯ·��״̬֡
        AppData.AFN = 0x10;
        AppData.Fn = 4;
        AppData.DataUnitLen = 0;
        ret = CommFunDeal(AppData, &RecvAppData, nTtlIndex);
        if (ret > 0)
        {
            if (RecvAppData.DataUnitBuff[0] & 0x02)                     //�ϱ����
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_GET_RUN_STATE: ������δ�����������ȴ�\n");
                WaitTime[nTtlIndex] = 0;
            }
            else
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_GET_RUN_STATE: �л�������̬�����ٲ�ѯ����״̬\n");
                WaitTime[nTtlIndex] = 100;                 //�ȴ�ֵ��Ϊ���100s�����ٵ���ʱ
                UpdateSuccessFlag[nTtlIndex] = 1;          //�����ɹ�
                gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_END;
            }
        }
        else                //δ�յ���ģ��Ļظ������ж�Ϊ�쳣���п���ģ����������
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_GET_RUN_STATE: δ�յ���ģ��Ļظ�\n");
            WaitTime[nTtlIndex] = 0;   //�ȴ�ʱ����0,�´��ٲ�ѯ
        }
        break;
    case MUD_END:
        //������ɺ�100��������ز�����
        if(IsCloseInnerDelayState((gPastTimeFlag[nTtlIndex].BitSect.Second == TRUE),&WaitTime[nTtlIndex],100) <= 0)
        {
            //UpdataStat[nTtlIndex] = 0x5A;
            return -1;
        }

        CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_END: �ر��ز�ģ�������ļ�\n");
        close(Fd[nTtlIndex]);
        Fd[nTtlIndex] = -1;

        //PauseDelayCnt[nTtlIndex] = 0;
        Size[nTtlIndex] = 0;
        memset(Buffer[nTtlIndex],0x00,sizeof(Buffer[nTtlIndex]));
        //PackageNo[nTtlIndex] = 0;
        memset((uint8 *)&Uis[nTtlIndex],0x00,sizeof(Uis[nTtlIndex]));
        memset(TmpPath[nTtlIndex],0x00,sizeof(TmpPath[nTtlIndex]));
        TotalRetryCnt[nTtlIndex] = 0;

        CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_END: ɾ���ز������ļ�\n");
        sprintf(TmpPath[nTtlIndex],"rm -rf %s", gFilePath);
        system(TmpPath[nTtlIndex]);
        memset(TmpPath[nTtlIndex],0x00,sizeof(TmpPath[nTtlIndex]));

        if(1 == UpdateSuccessFlag[nTtlIndex])
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_END: �����ļ����з������\n");
            gUpdateResult = 0;
        }
        else if(2 == UpdateSuccessFlag[nTtlIndex])
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_END: �����ļ��쳣\n");
            gUpdateResult = 1;
        }else
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_END: �����ļ�����δ��ɣ�������\n");
            gUpdateResult = 2;
        }
        gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_IDLE;
        //UpdataStat[nTtlIndex] = 0xAA;

        gIsUpdating = 0;
        gUpdateEndTime =time(NULL);
        gUpdateState =  2;
        //CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_END: ����ʱ100�����, �������������߳�!\n");
        //gRebootAmrThread[nTtlIndex] = 1;
        //sleep(3);
        break;

    default:
        gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_IDLE;
        break;
    }

    return -1;
}

/*******************************************************************************
* ��������: GetUnitInfo
* ��������: ��ѯͨ�ŵ�Ԫ��Ϣ(�������ѯ�汾��)
* �������: uint8 nArrIndex       ģ���߳�����(0:�����һ��CCOģ��; 1:����ڶ���CCOģ��)
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void GetUnitInfo(uint8 nArrIndex)
{
    uint8           CurNum;                      //���λظ�����
    AppData_T       AppData;
    AppData_T       RecvAppData;
    int             i, j;
    int             ret = -1;
    uint16          TotalNum = 0;
    uint16          CalTotalNum = 0;//�ۼ�����
    int             index = 0;//ƫ��λ
    uint16          CurIndex = 0;//��ǰ����
    uint8           nTtlIndex = nArrIndex;//��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

    memset(&gAllCcoStaInfo[nArrIndex], 0x00, sizeof(gAllCcoStaInfo[nArrIndex]));
    memset((uint8 *)&AppData, 0x0, sizeof(AppData_T));

    if(gLocalCommModuleInfo[nArrIndex].LocalCommPatternWord.b1.BitSect.CommPattern != 2)
    {
        return;
    }

    //�����ϱ���Ϣ
    INFO_DATA_T InfoMsgData;
	memset((uint8 *)&InfoMsgData, 0x00, sizeof(InfoMsgData));

    //���Ͳ�ѯ����
    AppData.AFN = 0x10;
    AppData.Fn = 104;
    AppData.DataUnitLen = 3;
    CCODOWN_FMT_DEBUG(nArrIndex, "GetUnitInfo() ��ѯ·��ͨ�ŵ�Ԫ��Ϣ\n");
    for(i=0; (i*15)<MAX_CCO_STA_NUM; i++)   //ÿ֡��64����¼���ظ�ֱ����ȡ��ȫ����¼
    {
        *(uint16 *)AppData.DataUnitBuff = (i*15);       //��ʼ��Ŵ�0��ʼ
        AppData.DataUnitBuff[2] = 15;
        CCODOWN_FMT_DEBUG(nArrIndex, "��ѯ: ��ʼ���=%d (0:CCO, ����:STA), �ڵ�����=%d \n", i*15, AppData.DataUnitBuff[2]);
        ret = CommFunDeal(AppData, &RecvAppData, nArrIndex);
        if(ret == 2)
        {
            RecvAppData.DataUnitBuff[0] = 0xff;
            RecvAppData.DataUnitBuff[1] = 0xff;
            CCODOWN_FMT_DEBUG(nArrIndex, "GetUnitInfo() ·����Ϣ��ѯʧ��\n");
            break;
        }
        TotalNum = *(uint16 *)RecvAppData.DataUnitBuff;//�ڵ�������
        CurNum = RecvAppData.DataUnitBuff[2];//����Ӧ������
        index = 3;

        CCODOWN_FMT_DEBUG(nArrIndex, "GetUnitInfo() i=%d, �ڵ�������=%d, ����Ӧ������=%d \n", i, TotalNum, CurNum);

        //��װ�ϱ���Ϣ����
        memset((uint8 *)&InfoMsgData, 0x00, sizeof(InfoMsgData));
        CurIndex = 0;//��ǰ����

        InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
    	InfoMsgData.IP  = ntohl(InfoMsgData.IP);
    	InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
    	InfoMsgData.MsgPRIORITY = 0;
    	InfoMsgData.MsgRPM = 1;
    	InfoMsgData.MsgIndex = GetInfoIndex();
    	InfoMsgData.MsgLabel = nTtlIndex;  //mqtt��Ŷ˿����(0��1)
    	InfoMsgData.SourLen = strlen(gCcoRouterAppName);
    	memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
    	memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
    	InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
    	memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
    	memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
    				
    	InfoMsgData.IID_ID = 0x0003;
    	InfoMsgData.IOP_ID = 0xff01;

    	Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurIndex]);  //�˿ں�
    	CurIndex += 4;
        ChangeLenToBuff(CurNum, &CurIndex, &InfoMsgData.aPayloadData[CurIndex]);  //����Ӧ��ڵ����
        
        for(j=0; j<CurNum; j++)
        {
            if(CalTotalNum >= MAX_CCO_STA_NUM)
            {
                break;
            }
            memcpy_r(gAllCcoStaInfo[nArrIndex].aAddr[CalTotalNum], &RecvAppData.DataUnitBuff[index], 6);
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], &RecvAppData.DataUnitBuff[index], 6);
            index += 6;
            CurIndex += 6;
            memcpy_r(gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].aSoftVer, &RecvAppData.DataUnitBuff[index], 2);
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], &RecvAppData.DataUnitBuff[index], 2);
            index += 2;
            CurIndex += 2;
            gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].nVerDay = RecvAppData.DataUnitBuff[index];
            InfoMsgData.aPayloadData[CurIndex] = RecvAppData.DataUnitBuff[index];
            index += 1;
            CurIndex += 1;
            gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].nVerMonth = RecvAppData.DataUnitBuff[index];
            InfoMsgData.aPayloadData[CurIndex] = RecvAppData.DataUnitBuff[index];
            index += 1;
            CurIndex += 1;
            gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].nVerYear = RecvAppData.DataUnitBuff[index];
            InfoMsgData.aPayloadData[CurIndex] = RecvAppData.DataUnitBuff[index];
            index += 1;
            CurIndex += 1;
            memcpy_r((uint8*)&gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].aFacCode[0], &RecvAppData.DataUnitBuff[index], 2);
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], &RecvAppData.DataUnitBuff[index], 2);
            index += 2;
            CurIndex += 2;
            memcpy_r((uint8*)&gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].aChipCode[0], &RecvAppData.DataUnitBuff[index], 2);
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], &RecvAppData.DataUnitBuff[index], 2);
            index += 2;
            CurIndex += 2;

            if(0 == CalTotalNum)//���0��ʾ���ڵ�
            {
                gAllCcoStaInfo[nArrIndex].aType[CalTotalNum] = 0;//0���ڵ� 1����
                InfoMsgData.aPayloadData[CurIndex++] = 0;
            }
            else
            {
                gAllCcoStaInfo[nArrIndex].aType[CalTotalNum] = 1;//0���ڵ� 1����
                InfoMsgData.aPayloadData[CurIndex++] = 1;
            }

            CCODOWN_FMT_DEBUG(nArrIndex, "��%d���ڵ���Ϣ: ��ַ %02x %02x %02x %02x %02x %02x, ����汾�� %02x %02x, �汾���� %02x-%02x-%02x, ģ�鳧�̴��� %02x %02x, оƬ���� %02x %02x, ���� %d\n", 
                j, gAllCcoStaInfo[nArrIndex].aAddr[CalTotalNum][0], gAllCcoStaInfo[nArrIndex].aAddr[CalTotalNum][1], gAllCcoStaInfo[nArrIndex].aAddr[CalTotalNum][2], 
                gAllCcoStaInfo[nArrIndex].aAddr[CalTotalNum][3], gAllCcoStaInfo[nArrIndex].aAddr[CalTotalNum][4], gAllCcoStaInfo[nArrIndex].aAddr[CalTotalNum][5], 
                gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].aSoftVer[0], gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].aSoftVer[1], gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].nVerYear, 
                gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].nVerMonth, gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].nVerDay, gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].aFacCode[0], 
                gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].aFacCode[1], gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].aChipCode[0], gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].aChipCode[1], 
                gAllCcoStaInfo[nArrIndex].aType[CalTotalNum]);
            
            CalTotalNum ++;
            CCODOWN_FMT_DEBUG(nArrIndex, "��ǰ�ۼ�������CalTotalNum=%d \n", CalTotalNum);
        }

        InfoMsgData.nPayloadLength = CurIndex;
	    PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);

        if(CalTotalNum >= MAX_CCO_STA_NUM)
        {
            break;
        }

        if(CurNum < 15)
        {
            break;   //���ظ��ڵ���С�ڲ�ѯ�ڵ�������˵�����������һ���ڵ���ţ�����Ҫ�ټ�����ѯ
        }
        gAmrThreadFlag[nArrIndex] = 1;//ι�̹߳�
    }

    gAllCcoStaInfo[nArrIndex].nCcoStaNum = CalTotalNum;//������
    CCODOWN_FMT_DEBUG(nArrIndex, "GetUnitInfo() �ۼ�������nCcoStaNum=%d \n", gAllCcoStaInfo[nArrIndex].nCcoStaNum);

    return;
}

/*******************************************************************************
* ��������: ModuleID_Recv
* ��������: ������ȡ��ģ����Ϣ
* �������: AppData_T *AppData    ģ����Ϣ
* �������: ModuleManuID_T *ModuleManuID    ģ�鳧������
* �� �� ֵ: >0 �ɹ�; <0 ʧ��
*******************************************************************************/
uint8 ModuleID_Recv(AppData_T  *AppData,ModuleManuID_T *ModuleManuID)
{
	*ModuleManuID = MMID_NOT_NULL;
	return 1;
}

/*******************************************************************************
* ��������: UnPackModuleInfo
* ��������: ������ȡ��ģ����Ϣ
* �������: AppData_T RecvAppData    ģ����Ϣ
            uint8 nArrIndex          ģ���߳�����(0:�����һ��CCOģ��; 1:����ڶ���CCOģ��)
* �������: ��
* �� �� ֵ: >0 �ɹ�; <0 ʧ��
*******************************************************************************/
int UnPackModuleInfo(AppData_T RecvAppData, uint8 nArrIndex)
{
    AppData_T   TmpAppData;
    memset((uint8 *)&TmpAppData,0x00,sizeof(AppData_T));

	uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = nArrIndex;

    GetLocalCommModuleRunningModeInfo(&RecvAppData.DataUnitBuff[0], &gLocalCommModuleInfo[nTtlIndex], sizeof(LOCAL_COMM_MODULE_RUNNING_PATTERN_INFO));

    memcpy(&(TmpAppData.DataUnitBuff[0]), &(gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.InfoBuff[0]), 9);

    return ModuleID_Recv(&TmpAppData, &gModuleManuID[nTtlIndex]);

}

/*******************************************************************************
* ��������: GetMasterAddr_Recv
* ��������: ��ȡ���ڵ��ַ
* �������: AppData_T  *AppData          ģ����Ϣ
            uint8 nArrIndex          ģ���߳�����(0:�����һ��CCOģ��; 1:����ڶ���CCOģ��)
* �������: ��
* �� �� ֵ: 1 ���ڵ��ַ�뱾����ͬ; 0 ���ڵ��ַ�뱾�ز�ͬ
*******************************************************************************/
uint8 GetMasterAddr_Recv(AppData_T  *AppData, uint8 nArrIndex)
{
    uint8    TmnAddr[6];//�ն˵�ַ

    ReadTmnAddr(TmnAddr, nArrIndex);  //��ȡ�ն˵�ַ 

	CCODOWN_FMT_DEBUG(nArrIndex, "GetMasterAddr_Recv() �ն�ʵ�ʵ�ַ: \n");
	CCODOWN_BUF_DEBUG(nArrIndex, TmnAddr, 6, "��ַ");
	CCODOWN_FMT_DEBUG(nArrIndex, "GetMasterAddr_Recv() 03F4�ٻ���ַ: \n");
	CCODOWN_BUF_DEBUG(nArrIndex, AppData->DataUnitBuff, 6, "��ַ");

    if(0 ==  memcmp(TmnAddr,AppData->DataUnitBuff,6))
    {
        return 1;//���
    }
    else
        return 0;
}

/*******************************************************************************
* ��������: RegPlcModule
* ��������: ע���ز�ģ��
* �������: uint8 nArrIndex          ģ���߳�����(0:�����һ��CCOģ��; 1:����ڶ���CCOģ��)
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void RegPlcModule(uint8 nArrIndex)
{
    //��ģ����Ϣд��˽���ļ�
	//������
}

/*******************************************************************************
* ��������: GetModuleManuID
* ��������: ��ȡģ�鳧�̱�ʶ
* �������: TASK_FUN_INPUT stTaskFunInput   �ز����������Ϣ
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0               ʧ��
*******************************************************************************/
int GetModuleManuID(TASK_FUN_INPUT stTaskFunInput)
{
    //printx("��ȡģ�鳧�̱�ʶGetModuleManuID(TASK_FUN_INPUT stTaskFunInput)...begin \n");
    int             RecvLen;                            //�������ݳ���
    uint8           RecvBuf[MAX_QUEUE_BUFF_LEN];        //�������ݻ�����
    AppData_T       AppData;                            //Ӧ�ò�����
    AppData_T       RecvAppData;                        //Ӧ�ò�����
    int             ret;
    uint8           TmnAddr[6];                         //�ն˵�ַ
    uint8           ModuAdressId[8];
    static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM]; //ͨѶ��ʼʱ��
    uint8           isNeedWait = 0;
    uint8           WaitSeconds = 0;
    static  uint8   RecvActFlag[MAX_DOWN_THREAD_NUM];   //�յ�ģ�������ϱ���Ϣ��־
    
    uint8           nTtlIndex = 0;                      //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
    nTtlIndex = stTaskFunInput.TaskArrIndex;

    isNeedWait = stTaskFunInput.InfoMsgBuff[0]; //0:����Ҫ�ȴ��ϱ�; 1:��Ҫ�ȴ��ϱ�ģ����Ϣ

    //printx(" GetModuleManuID(TASK_FUN_INPUT stTaskFunInput)��ȡģ�鳧�̱�ʶ...nPortID:%d, nTtlIndex:%d\n", nPortID, nTtlIndex);
    
    switch (gTotalState[nTtlIndex].GetManuID)
    {

    case ACT_MODULE_IDLE:                                      //ģ���ʼ��״̬
        CCODOWN_FMT_DEBUG(nTtlIndex, "GetModuleManuID()ģ���ʼ��״̬...ACT_MODULE_IDLE \n");
        ClearCommRecvBuff(nTtlIndex);                              //���������
        CheckModuleSoftReset(1, nTtlIndex);                          //�ڴ�������·�ɸ�λ����������

        if (g_CcoProduct == CCO_PRODUCT_NO_MODULE)
        {
            PlcResetRouter(nTtlIndex);
        }

        gTotalState[nTtlIndex].GetManuID = ACT_WAIT_MODULEINFO;
        CommBeginTime[nTtlIndex] = time(NULL);
        RecvActFlag[nTtlIndex] = 0;
        if(0 == isNeedWait)
        {
            ClearCommQueueBuff(&gAmrRecvDownReportModule[nTtlIndex]);//��������ϱ�ģ����Ϣ
        }
        CCODOWN_FMT_DEBUG(nTtlIndex, "GetModuleManuID()�ȴ�·�������ϱ�ģ�鳧����Ϣ...60��(isNeedWait=%d)...ACT_WAIT_MODULEINFO \n", isNeedWait);
        break;

    case ACT_WAIT_MODULEINFO:  //�ȴ�·�������ϱ�ģ�鳧����Ϣ
        //CCODOWN_FMT_DEBUG(nTtlIndex, "GetModuleManuID()�ȴ�·�������ϱ�ģ�鳧����Ϣ...ACT_WAIT_MODULEINFO \n");
        RecvLen = PlcReadQueue(&gAmrRecvDownReportModule[nTtlIndex], RecvBuf);
        if(RecvLen > 0)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "�յ������ϱ����� RecvLen = PlcReadQueue(gAmrRecvDownReportModule[nTtlIndex], RecvBuf);...RecvLen:%d \n", RecvLen);
            if(UnPackFrameData(RecvLen, RecvBuf, &AppData) == CHECK_RESULT_SUCCEED)
            {
                if ((AppData.AFN == 0x03) && (AppData.Fn == 10))
                {
                    ret = UnPackModuleInfo(AppData, nTtlIndex);
                    if(ret > 0)
                    {
                        RecvActFlag[nTtlIndex] = 1;
                        CCODOWN_FMT_DEBUG(nTtlIndex, "�ɹ�! GetModuleManuID()�ȴ�·�������ϱ�ģ�鳧����Ϣ, �յ������ϱ�AFN03H-F10����, ���Ľ�����ȷ! ret:%d \n", ret);
                        CheckModuleSoftReset(0, nTtlIndex);
                    }
                }
            }
        }
        else
        {
            if(1 == RecvActFlag[nTtlIndex])
            {
                gTotalState[nTtlIndex].GetManuID = ACT_MODULE_SEND_GET_ADDRESS;
                break;
            }
            
            if(1 == isNeedWait)
            {
                WaitSeconds = 60;
            }
            else
            {
                WaitSeconds = 0;
            }

            if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) >= WaitSeconds)
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "δ�յ��ϱ����� if ((time(NULL) - CommBeginTime) >= %d ��ʱ...\n", WaitSeconds);
                gTotalState[nTtlIndex].GetManuID = ACT_READ_MODULEINFO;  //����������ȡ
            }
            else
            {
                gTotalState[nTtlIndex].GetManuID = ACT_WAIT_MODULEINFO;  //�����ȴ������ϱ�
            }
        }
        break;

    case ACT_READ_MODULEINFO:                                //������ȡ����ģ������ģʽ��Ϣ
        CCODOWN_FMT_DEBUG(nTtlIndex, "GetModuleManuID()������ȡ����ģ������ģʽ��Ϣ...ACT_READ_MODULEINFO \n");
        AppData.AFN = 0x03;
        AppData.Fn = 10;
        AppData.DataUnitLen = 0;
        CheckModuleSoftReset(1, nTtlIndex);
        ret = CommFunDeal(AppData,&RecvAppData, nTtlIndex);
        if(ret > 0)
        {
            if(ret == 1)
            {
                ret = UnPackModuleInfo(RecvAppData, nTtlIndex);
                if(ret > 0)
                {
                    gTotalState[nTtlIndex].GetManuID = ACT_MODULE_SEND_GET_ADDRESS;
                    CheckModuleSoftReset(0, nTtlIndex);
                    CCODOWN_FMT_DEBUG(nTtlIndex, "�ɹ�! GetModuleManuID()������ȡ����ģ������ģʽ��Ϣ, �յ�AFN03H-F10Ӧ������, ���Ľ�����ȷ! ret:%d \n", ret);
                }
            }
            else
            {
                gTotalState[nTtlIndex].GetManuID = ACT_MODULE_IDLE;//��ͷ��ʼ����ִ��
            }
        }
        break;

    case ACT_MODULE_SEND_GET_ADDRESS:                        //��ȡ���ڵ��ַ
        CCODOWN_FMT_DEBUG(nTtlIndex, "GetModuleManuID()��ȡ���ڵ��ַ...ACT_MODULE_SEND_GET_ADDRESS \n");
        AppData.AFN = 0x03;
        AppData.Fn = 4;
        AppData.DataUnitLen = 0;
        ret = CommFunDeal(AppData,&RecvAppData, nTtlIndex);
        if(ret > 0)
        {
            ret = GetMasterAddr_Recv(&RecvAppData, nTtlIndex);
            if(ret == 1)
            {
                gTotalState[nTtlIndex].GetManuID = ACT_MODULE_END;
            }
            else
            {
                gTotalState[nTtlIndex].GetManuID = ACT_MODULE_SEND_SET_ADDRESS;
            }

        }
        break;

    case ACT_MODULE_SEND_SET_ADDRESS:                      //�������ڵ��ַ
        CCODOWN_FMT_DEBUG(nTtlIndex, "GetModuleManuID()�������ڵ��ַ...ACT_MODULE_SEND_SET_ADDRESS \n");

        ReadTmnAddr(TmnAddr, nTtlIndex);
        memset(ModuAdressId, '0', 8);
        memcpy(ModuAdressId,TmnAddr,6);
        AppData.AFN = 0x05;
        AppData.Fn = 1;
        AppData.DataUnitLen = 6;
        memcpy(AppData.DataUnitBuff, TmnAddr, 6);

        ret = CommFunDeal(AppData, &RecvAppData, nTtlIndex);
        if(ret > 0)
        {
            memcpy(&gLocalCommModuleInfo[nTtlIndex].MasterAddr[0], gTerminalAddr[nTtlIndex], 6);
            gTotalState[nTtlIndex].GetManuID = ACT_MODULE_END;
        }
        
        break;

    case ACT_MODULE_END:
        CCODOWN_FMT_DEBUG(nTtlIndex, "GetModuleManuID()����...ACT_MODULE_END \n");

        RegPlcModule(nTtlIndex);    //ע���ز�ģ��
        
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]���ڳ���ģʽ:%02x(0:����,1:����,2:����,3:��֧��)\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b1.BitSect.AmrCycleMode);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]�ӽڵ���Ϣģʽ:%02x\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b1.BitSect.SlaverInfoMode);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]·�ɹ���ʽ:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b1.BitSect.RouterMangerPattern);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]ͨ�ŷ�ʽ:%d(1:խ��,2:���,3:С����)\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b1.BitSect.CommPattern);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]�㲥�����ŵ�ִ�з�ʽ:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b2.BitSect.BroadcastChannelExecPattern);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]�㲥����ȷ�Ϸ�ʽ:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b2.BitSect.BroadcastAffirmPattern);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]ʧ�ܽڵ��л�����ʽ:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b2.BitSect.FailureNodeSwitchLaunchPattern);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]������ʱ����֧��:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b2.BitSect.TransDelayParamSupport);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]��������:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b4.BitSect.SpeedNum);//<corrigendum-2013-09-18>
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]�ŵ�����:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b3.BitSect.ChannelNum);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]��ѹ����������Ϣ:0X%02x\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b3.BitSect.LowVoltageGridPowerDownInfo);//<corrigendum-2013-09-18>
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]�ӽڵ������ʱʱ��:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].SlaverMonitorMaxOverTime);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]�㲥�������ʱʱ��:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].BroadcastMaxOverTime);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]���֧�ֱ��ĳ���:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].MaxFrameLen);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]�ļ�����֧����󵥸����ݰ�����:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].FileTransferMaxSolePackageLen);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]���������ȴ�ʱ��:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].UpdateOperatorWaittingTime);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]���ڵ��ַ:%02x %02x %02x %02x %02x %02x\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].MasterAddr[0],
               gLocalCommModuleInfo[nTtlIndex].MasterAddr[1],gLocalCommModuleInfo[nTtlIndex].MasterAddr[2],
               gLocalCommModuleInfo[nTtlIndex].MasterAddr[3],gLocalCommModuleInfo[nTtlIndex].MasterAddr[4],
               gLocalCommModuleInfo[nTtlIndex].MasterAddr[5]);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]���֧�ִӽڵ�����:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].MaxSlaverNum);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]��ǰ�ӽڵ�����:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].CurrentSlaverNum);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]ͨ��ģ��ʹ�õ�Э�鷢������:%02x-%02x-%02x\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].ModuleProtocolReleaseDate[0],gLocalCommModuleInfo[nTtlIndex].ModuleProtocolReleaseDate[1],gLocalCommModuleInfo[nTtlIndex].ModuleProtocolReleaseDate[2]);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]ͨ��ģ��ʹ�õ�Э����󱸰�����:%02x-%02x-%02x\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].ModuleProtocolLastRecordDate[0],gLocalCommModuleInfo[nTtlIndex].ModuleProtocolLastRecordDate[1],gLocalCommModuleInfo[nTtlIndex].ModuleProtocolLastRecordDate[2]);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]���̴���:%02x %02x\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.ManufacturerCode[0],gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.ManufacturerCode[1]);     
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]оƬ����:%02x %02x\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.ChipCode[0],gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.ChipCode[1]);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]�汾������:%02x %02x %02x\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.EditionDate[0],gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.EditionDate[1],gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.EditionDate[2]);
        CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d]�汾��:%d(0x%04x)\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.EditionInfo,gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.EditionInfo);

        //�ز�ģ����Ϣ��ֵ���ṹ��, ����ѯʹ��
        memcpy_r(&gAmrModuleQuery[nTtlIndex].aLocalCommTypeModel[0], (uint8 *)&gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord, 6);//����ͨ��ģʽ��
        gAmrModuleQuery[nTtlIndex].nSlaveMonitorMaxOvertime = gLocalCommModuleInfo[nTtlIndex].SlaverMonitorMaxOverTime;//�ӽڵ������ʱʱ��(��λ:s)
        gAmrModuleQuery[nTtlIndex].nBroadcastMaxOvertime = gLocalCommModuleInfo[nTtlIndex].BroadcastMaxOverTime;//�㲥�������ʱʱ��(��λ:s)
        gAmrModuleQuery[nTtlIndex].nMaxAllowedFrameLen = gLocalCommModuleInfo[nTtlIndex].MaxFrameLen;//���֧�ֵı��ĳ���
        gAmrModuleQuery[nTtlIndex].nMaxUpgradeSingleFrameLen = gLocalCommModuleInfo[nTtlIndex].FileTransferMaxSolePackageLen;//�ļ�����֧�ֵ���󵥸����ݰ�����
        gAmrModuleQuery[nTtlIndex].nUpgradeWaitingPeriod = gLocalCommModuleInfo[nTtlIndex].UpdateOperatorWaittingTime;//���������ȴ�ʱ��
        //���� �㲥�ɹ���ʱʱ��
        memcpy_r(&gAmrModuleQuery[nTtlIndex].aModuleID[0], &gLocalCommModuleInfo[nTtlIndex].MasterAddr[0], 6);//ģ����
        gAmrModuleQuery[nTtlIndex].nMaxSupportSlaveNum = gLocalCommModuleInfo[nTtlIndex].MaxSlaverNum;//֧�ֵ����ӽڵ�����
        gAmrModuleQuery[nTtlIndex].nCurrentSlaveNum = gLocalCommModuleInfo[nTtlIndex].CurrentSlaverNum;//��ǰ�ӽڵ�����

        //ֱ�ӿ���(С��)
        memcpy(&gAmrModuleQuery[nTtlIndex].InfoBuff[0], &gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.InfoBuff[0], 9);//ͨ��ģ�鳧�̴��뼰�汾��Ϣ(ԭʼ9�ֽ�)
        memcpy(&gAmrModuleQuery[nTtlIndex].ManufacturerCode[0], &gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.ManufacturerCode[0], 2);//���̴���
        memcpy(&gAmrModuleQuery[nTtlIndex].ChipCode[0], &gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.ChipCode[0], 2);//оƬ����
        memcpy(&gAmrModuleQuery[nTtlIndex].EditionDate[0], &gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.EditionDate[0], 3);//�汾������
        gAmrModuleQuery[nTtlIndex].nModuleVer = gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.EditionInfo;//ģ��汾

        gTotalState[nTtlIndex].GetManuID = ACT_MODULE_IDLE;
        return 1;
        
        break;
    }

    return -1;
}


/*******************************************************************************
* ��������: CustomTaskDeal
* ��������: �Զ�����������
* �������: TASK_FUN_INPUT stTaskFunInput   �ز����������Ϣ
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0               ʧ��
*******************************************************************************/
int CustomTaskDeal(TASK_FUN_INPUT stTaskFunInput)
{
	return 1;

}


/*******************************************************************************
* ��������: CheckMeterFilesResult
* ��������: ���·�ɵ�����Ϣ
* �������: char *MeterInfo              ������Ϣ
            uint8 MeterNum           ��������
            uint8 nArrIndex          ģ���߳�����(0:�����һ��CCOģ��; 1:����ڶ���CCOģ��)
* �������: ��
* �� �� ֵ: >0 �ɹ�; <0 ʧ��
*******************************************************************************/
uint8 CheckMeterFilesResult(char *MeterInfo,uint8 MeterNum, uint8 nArrIndex)
{
	uint16		i = 0;
	uint8		MtrAddr[6] = {0};
	uint16		MtrIndex = 0;
	uint8		Result = 0;//�ȽϽ��(0:��ͬ 1:��ͬ)
	int         ret = 0;
	AMR_METER_FILES_INFO_T MeterFilesInfo;
	uint8	    nProtocolType;	//Э������
	

	uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = nArrIndex;
	
	if(MeterNum > 0)
	{
		for(i = 0; i < MeterNum; i++)
		{
			//����ַ�Ƿ�����ڵ����б�
			memcpy(MtrAddr, &MeterInfo[8*i], 6);
			nProtocolType = (MeterInfo[8*i+7] >> 3) & 0x07;  //���յ��ı����н���Э��
			
			ret = IsExistMtrAddrInAmrInfo(MtrAddr, &MtrIndex, nTtlIndex);  //��ַ�Ƿ����
			//printx(" CheckMeterFilesResult() nProtocolType:%d, ret:%d \n", nProtocolType, ret);
			
			if((ret >= 0) && (ret == nProtocolType))
			{
				//�����ر�־
				CCODOWN_FMT_DEBUG(nTtlIndex, "�˿��±�:%d, ������ַ:%02x %02x %02x %02x %02x %02x, Э������:%d, �����ڳ���ṹ��! \n", 
					nTtlIndex, MtrAddr[0], MtrAddr[1],MtrAddr[2],MtrAddr[3], MtrAddr[4],MtrAddr[5], nProtocolType);
				gAmrInfo[nTtlIndex].MtrAmrInfo[MtrIndex].SpecialFlag = 1;//һ�µı�־
			}
			else
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "�˿��±�:%d, ������ַ:%02x %02x %02x %02x %02x %02x, Э������:%d, ����ṹ��û�иõ���, ��Ҫɾ��! \n", 
					nTtlIndex, MtrAddr[0], MtrAddr[1],MtrAddr[2],MtrAddr[3], MtrAddr[4],MtrAddr[5], nProtocolType);
				//�����ɾ������
				MeterFilesInfo.nPortID = gPortID[nTtlIndex];				
				memcpy(MeterFilesInfo.stAmrMeterFiles.aMeterFilesAddr, MtrAddr, 6);
				MeterFilesInfo.stAmrMeterFiles.nProtocolType = nProtocolType;
				ListInsert_Files(&gToDeleteFilesList[nTtlIndex], gToDeleteFilesList[nTtlIndex].nMeterFilesNum+1, MeterFilesInfo);
				Result = 1;
			}
		}
	}
	return Result;//·�ɵ���һ��
}


/*******************************************************************************
* ��������: CheckAllMeterFileCmpResult
* ��������: ����ն˵����Ƿ���Ѿ��Ƚ����
* �������: uint8 nArrIndex          ģ���߳�����(0:�����һ��CCOģ��; 1:����ڶ���CCOģ��)
* �������: ��
* �� �� ֵ: >0 �ɹ�; <0 ʧ��
*******************************************************************************/
uint8 CheckAllMeterFileCmpResult(uint8 nArrIndex)
{
    uint16         i;
    uint8          Result = 0;//�ȽϽ��(0:��ͬ 1:��ͬ)
	AMR_METER_FILES_INFO_T MeterFilesInfo;

	uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = nArrIndex;

    //�鿴�Ƿ���δ���صĲ�����
    for (i = 0; i < gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt; i++)
    {
        if (gAmrInfo[nTtlIndex].MtrAmrInfo[i].SpecialFlag == 0)
        {
        	Result = 1;
            //ͨ�ŵ�ַ
            GetElem_Files(gAmrMeterFilesList[nTtlIndex], i+1, &MeterFilesInfo);
            CCODOWN_FMT_DEBUG(nTtlIndex, "�˿ں�:%08x, ������ַ:%02x %02x %02x %02x %02x %02x, ͨ��Э��:%d, δ�ѵ��ñ�������Ҫ���! \n", 
					gPortID[nTtlIndex], MeterFilesInfo.stAmrMeterFiles.aMeterFilesAddr[0], MeterFilesInfo.stAmrMeterFiles.aMeterFilesAddr[1],
					MeterFilesInfo.stAmrMeterFiles.aMeterFilesAddr[2],MeterFilesInfo.stAmrMeterFiles.aMeterFilesAddr[3],
					MeterFilesInfo.stAmrMeterFiles.aMeterFilesAddr[4],MeterFilesInfo.stAmrMeterFiles.aMeterFilesAddr[5],
					MeterFilesInfo.stAmrMeterFiles.nProtocolType);
			gAmrInfo[nTtlIndex].MtrAmrInfo[i].SpecialFlag = 1;
			
            ListInsert_Files(&gToAddFilesList[nTtlIndex], gToAddFilesList[nTtlIndex].nMeterFilesNum+1, MeterFilesInfo);
        }
    }
    return Result;
}


/*******************************************************************************
* ��������: MeterFileCmpDeal
* ��������: ������ȶԴ�������
* �������: TASK_FUN_INPUT stTaskFunInput   �ز����������Ϣ
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0               ʧ��

·�ɵ������޸��»���:

----> ·��ģ����������ϵĵ����޸Ļ���,��������ģ����ͨ������������

----> ÿ�ε����Ķ�ȡ�·�������ȡ������:MODULEDEAL_CNT

----> �����޸ķ�Ϊ�������:
      -->1)��ģ�鶨ʱ�ж�:
                                   1) ��ȡ·�ɵ�����Ϣ
                                   2) �ȽϺ�����ж������������·�ɵ���,��ô�ж�·�ɵ�����һ��
                                   3) ���е������޸Ĵ���
                                   4) ��·�ɽ��в�����ʼ��
                                   5) ����ն˵�����·����
      -->2)����վ�·�F10�޸�:
                                   1)��ȡ·�ɵ�����Ϣ
                                   2)�ȽϺ�����ж���ĵ���,��ӵ�ɾ���������У�����������ĵ�������ӵ�����������������
                                   3)���е������޸Ĵ���
                                   4)ɾ�������·�ɵ���
                                   5)���������·�ɵ���
                                   6)��ɺ���뵵����ͬ���Ƚ�
                                   7) ��ȡ·�ɵ�����Ϣ
                                   8) �ȽϺ�����ж������������·�ɵ���,��ô�ж�·�ɵ�����һ��
                                   9) ���е������޸Ĵ���
                                   10) ��·�ɽ��в�����ʼ��
                                   11) ����ն˵�����·����

*******************************************************************************/
int MeterFileCmpDeal(TASK_FUN_INPUT stTaskFunInput)
{
	static	uint8	CmpResult[MAX_DOWN_THREAD_NUM] = {0};					//�ȽϽ��(0:��ͬ 1:��ͬ)
	static	uint16	CurIndex[MAX_DOWN_THREAD_NUM] = {0};					//2013�浱ǰ������1��ʼ
	uint16			i;
	int 			ret;
	AppData_T		AppData;						//Ӧ�ò�����
	AppData_T		RecvAppData;					//Ӧ�ò��������
	char			MeterInfo[8*MODULEDEAL_CNT];    //�����Ϣ����ַ6�ֽڣ���Ϣ2�ֽ�

	uint8       nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;
	
	//printx(" int MeterFileCmpDeal() �����б�[%d]�ȶ� ...begin... \n", nTtlIndex);
	
	switch (gTotalState[nTtlIndex].MeterFileCmp)
	{
	case MFCS_IDLE: 							//����		
		CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileCmpDeal() �����б�[%d]�ȶ� ...MFCS_IDLE... \n", nTtlIndex);
		ReadAmrInfo(nTtlIndex);  //���ص�����Ϣ��������Ϣ

		//�����ر�־
        for (i = 0; i < gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt; i++)
        {
            gAmrInfo[nTtlIndex].MtrAmrInfo[i].SpecialFlag = 0;
        }
		if(gToAddFilesList[nTtlIndex].nMeterFilesNum > 0)//�״���Ҫȫ��ɾ��
		{
			ClearList_Files(&gToAddFilesList[nTtlIndex]);
		}
		if(gToDeleteFilesList[nTtlIndex].nMeterFilesNum > 0)//�״���Ҫȫ��ɾ��
		{
			ClearList_Files(&gToDeleteFilesList[nTtlIndex]);
		}
		CurIndex[nTtlIndex] = 1;//2013�浱ǰ������1��ʼ

		gTotalState[nTtlIndex].MeterFileCmp = MFCS_READ_AMOUNT_FILE;
		
		break;

	case MFCS_READ_AMOUNT_FILE: 				//��ȡ�ӽڵ�����
		CCODOWN_FMT_DEBUG(nTtlIndex, "int MeterFileCmpDeal() �����б�[%d]�ȶ� ...��ȡ�ӽڵ�����MFCS_READ_AMOUNT_FILE... \n", nTtlIndex);
		AppData.AFN = 0x10;
		AppData.Fn = 1;
		AppData.DataUnitLen = 0;
		ret = CommFunDeal(AppData,&RecvAppData, nTtlIndex);
		if (ret > 0)
		{
			memcpy(&gAmrInfo[nTtlIndex].TmnAmrInfo.SlaveCnt,RecvAppData.DataUnitBuff,2);  //�ӽڵ���������2�ֽ�
			memcpy(&gAmrInfo[nTtlIndex].TmnAmrInfo.MaxAllowSlaveCnt,&RecvAppData.DataUnitBuff[2],2);  //·��֧�����ӽڵ�������2�ֽ�
			//��·���ж�ȡ�Ĵӽڵ���������ϵͳ֧�ֵ��������ʱ-�ݴ�
			if(gAmrInfo[nTtlIndex].TmnAmrInfo.SlaveCnt > MAX_AMR_COLLECT_CNT)
			{
				gAmrInfo[nTtlIndex].TmnAmrInfo.SlaveCnt = gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt;
			}

			CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileCmpDeal()�˿�[%d]�ӽڵ�������:%d, ���֧������:%d \n"
				   , nTtlIndex, gAmrInfo[nTtlIndex].TmnAmrInfo.SlaveCnt, gAmrInfo[nTtlIndex].TmnAmrInfo.MaxAllowSlaveCnt);

			CmpResult[nTtlIndex] = 0;
			gTotalState[nTtlIndex].MeterFileCmp = MFCS_READ_INFO_FILE;
		}
		break;

	case MFCS_READ_INFO_FILE:					//��ȡ�ӽڵ���Ϣ
		CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileCmpDeal() �����б�[%d]�ȶ�  ...��ȡ�ӽڵ���ϢMFCS_READ_INFO_FILE...CurIndex:%d SlaveCnt:%d \n", nTtlIndex, CurIndex[nTtlIndex], gAmrInfo[nTtlIndex].TmnAmrInfo.SlaveCnt);
		if(CurIndex[nTtlIndex] > gAmrInfo[nTtlIndex].TmnAmrInfo.SlaveCnt)
		{
			gTotalState[nTtlIndex].MeterFileCmp = MFCS_RESULT_DEAL;
		}
		else
		{
			AppData.AFN = 0x10;
			AppData.Fn = 2;
			AppData.DataUnitLen = 3;
			memcpy(AppData.DataUnitBuff,&CurIndex[nTtlIndex],2);  //�ӽڵ���ʼ��ţ�2�ֽ�
			AppData.DataUnitBuff[2] = MODULEDEAL_CNT;  //�ӽڵ�������1�ֽ�
			ret = CommFunDeal(AppData,&RecvAppData, nTtlIndex);
			CCODOWN_FMT_DEBUG(nTtlIndex, "��ȡ�ӽڵ���Ϣ ret = CommFunDeal(AppData,&RecvAppData, nPortID); ret=[%d] \n", ret);
			if(ret > 0)
			{
				memcpy(MeterInfo,&RecvAppData.DataUnitBuff[3],RecvAppData.DataUnitBuff[2]*8);  //recv�ӽڵ�������2�ֽڣ�����Ӧ��ӽڵ�1�ֽڣ�������6�ֽڵ�ַ+2�ֽ���Ϣ
				if(CheckMeterFilesResult(MeterInfo, RecvAppData.DataUnitBuff[2], nTtlIndex) > 0)
				{
					CmpResult[nTtlIndex] = 1;
				}

				CurIndex[nTtlIndex] += RecvAppData.DataUnitBuff[2];

				if(CmpResult[nTtlIndex] > 0 && gMeterFilesModifyType[nTtlIndex] == FILES_MODEFY_CLEAR)
                {
                    gTotalState[nTtlIndex].MeterFileCmp = MFCS_RESULT_DEAL;
                    break;
                }
			}
		}
		break;

	case MFCS_RESULT_DEAL:						//�������
		CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileCmpDeal() �����б�[%d]�ȶ� ...�������MFCS_RESULT_DEAL... \n", nTtlIndex);

		if(CheckAllMeterFileCmpResult(nTtlIndex) > 0)
		{
			CmpResult[nTtlIndex] = 1;
		}

		//�ȽϽ����ͬ���õ����޸ı�־
		if (CmpResult[nTtlIndex] == 1)//��ͬ
		{
			if(gMeterFilesModifyType[nTtlIndex] == FILES_MODEFY_CLEAR)
			{
				//��cco��ɾ������ȫ�����
				ClearList_Files(&gToDeleteFilesList[nTtlIndex]);

				//����ǰ����ȫ�ֱ���ȫ����Ϊ����ӵ���
				memcpy(&gToAddFilesList[nTtlIndex], &gAmrMeterFilesList[nTtlIndex], sizeof(gToAddFilesList[nTtlIndex]));
			}
			gMeterFileModifyFlag[nTtlIndex] = 1; //ֱ�ӽ����޸�
			CCODOWN_FMT_DEBUG(nTtlIndex, "int MeterFileCmpDeal() �����б�[%d]�ȶԲ�һ��, ��Ҫͬ��... \n", nTtlIndex);
		}
		else//��ȫ��ͬ
		{
			gMeterFilesModifyType[nTtlIndex] = FILES_MODEFY_NULL;
			gMeterFileModifyState[nTtlIndex] = 0;
			CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileCmpDeal() �����б�[%d]�ȶ�һ�£�����Ҫ����ͬ������... \n", nTtlIndex);
		}

		CCODOWN_FMT_DEBUG(nTtlIndex, "�ȽϺ���Ҫ��ӵĴӽڵ���ϢgToAddFilesList[%d] :  \n", nTtlIndex);
		for(i=0; i<gToAddFilesList[nTtlIndex].nMeterFilesNum; i++)
		{
			PrintFilesDetail(gToAddFilesList[nTtlIndex].stMeterFilesData[i], nTtlIndex);
		}
		CCODOWN_FMT_DEBUG(nTtlIndex, "�ȽϺ���Ҫɾ���Ĵӽڵ���ϢgToDeleteFilesList[%d] :  \n", nTtlIndex);
		for(i=0; i<gToDeleteFilesList[nTtlIndex].nMeterFilesNum; i++)
		{
			PrintFilesDetail(gToDeleteFilesList[nTtlIndex].stMeterFilesData[i], nTtlIndex);
		}				

		gTotalState[nTtlIndex].MeterFileCmp = MFCS_IDLE;
		return 1;
		break;

	default:
		gTotalState[nTtlIndex].MeterFileCmp = MFCS_IDLE;
		break;
	}

	return -1;
}


/*******************************************************************************
* ��������: Comm_AddSlaveInfo_New
* ��������: ���Ӵӽڵ���Ϣ
* �������: AMR_METER_FILES_LIST_T *PModuleInfo     �����ӽڵ���Ϣ
            uint8 AddNum                            �����ӽڵ�����
            uint8 nArrIndex          ģ���߳�����(0:�����һ��CCOģ��; 1:����ڶ���CCOģ��)
* �������: ��
* �� �� ֵ: >0 �ɹ�; <0 ʧ��
*******************************************************************************/
int8 Comm_AddSlaveInfo_New(AMR_METER_FILES_LIST_T *PModuleInfo, uint8 AddNum, uint8 nArrIndex)
{
	CCODOWN_FMT_DEBUG(nArrIndex, "Comm_AddSlaveInfo_New()�˿�[%d]���Ӵӽڵ���Ϣ...begin...\n", nArrIndex);

    int                 i;
    AppData_T           AppData;
    AppData_T           RecvAppData;
    uint8               AddCnt = 0;
	int                 nMeterFilesNum = 0;

    AppData.AFN = 0x11;
    AppData.Fn = 1;

    for(i = 0; i < AddNum; i++)
    {
		nMeterFilesNum = PModuleInfo->nMeterFilesNum;
        if(nMeterFilesNum > 0)
        {
        	AddCnt++;
            memcpy(&AppData.DataUnitBuff[1+7*i], PModuleInfo->stMeterFilesData[nMeterFilesNum-1].stAmrMeterFiles.aMeterFilesAddr, 6);
            switch (PModuleInfo->stMeterFilesData[nMeterFilesNum-1].stAmrMeterFiles.nProtocolType)
			{
				//0x00Ϊ͸�����䣬0x01Ϊ97��0x02Ϊ07��0x03Ϊ��������
				case PRTL_97:
				{
					AppData.DataUnitBuff[1+7*i+6] = 0x01;
					break;
				}
				case PRTL_07:
				{
					AppData.DataUnitBuff[1+7*i+6] = 0x02;
					break;
				}
				case PRTL_OOP:
				{
					AppData.DataUnitBuff[1+7*i+6] = 0x03;
					break;
				}
				default:
					break;
			}
			
			CCODOWN_FMT_DEBUG(nArrIndex, "Comm_AddSlaveInfo_New()�˿�[%d]���ӵĴӽڵ���ϢΪ: \n", nArrIndex);
			PrintFilesDetail(PModuleInfo->stMeterFilesData[nMeterFilesNum-1], nArrIndex);
			ListDeleteByIndex_Files(PModuleInfo, nMeterFilesNum);
			
        }
        else
            break;

    }

    AppData.DataUnitLen = 1 + 7*AddCnt;
    AppData.DataUnitBuff[0] = AddCnt;

    return CommFunDeal(AppData, &RecvAppData, nArrIndex);

}


/*******************************************************************************
* ��������: Comm_DeleteRouterInfo_New
* ��������: ɾ���ӽڵ���Ϣ
* �������: AMR_METER_FILES_LIST_T *PModuleInfo     ɾ���ӽڵ���Ϣ
            uint8 DeleteNum                         ɾ���ӽڵ�����
            uint8 nArrIndex          ģ���߳�����(0:�����һ��CCOģ��; 1:����ڶ���CCOģ��)
* �������: ��
* �� �� ֵ: >0 �ɹ�; <0 ʧ��
*******************************************************************************/
int8 Comm_DeleteRouterInfo_New(AMR_METER_FILES_LIST_T *PModuleInfo, uint8 DeleteNum, uint8 nArrIndex)
{
	CCODOWN_FMT_DEBUG(nArrIndex, "Comm_DeleteRouterInfo_New()�˿�[%d]ɾ���ӽڵ���Ϣ...begin...\n", nArrIndex);
	
    AppData_T           AppData;
    AppData_T           RecvAppData;
    uint8               DeleteCnt = 0;
    int                 i;
	int                 nMeterFilesNum = 0;

    AppData.AFN = 0x11;
    AppData.Fn = 2;

	for(i = 0; i < DeleteNum; i++)
    {
		nMeterFilesNum = PModuleInfo->nMeterFilesNum;
        if(nMeterFilesNum > 0)
        {
        	DeleteCnt++;
            memcpy(&AppData.DataUnitBuff[1+6*i], PModuleInfo->stMeterFilesData[nMeterFilesNum-1].stAmrMeterFiles.aMeterFilesAddr, 6);

			CCODOWN_FMT_DEBUG(nArrIndex, "Comm_DeleteRouterInfo_New()�˿�[%d]ɾ���Ĵӽڵ���ϢΪ: \n", nArrIndex);
			PrintFilesDetail(PModuleInfo->stMeterFilesData[nMeterFilesNum-1], nArrIndex);
			ListDeleteByIndex_Files(PModuleInfo, nMeterFilesNum);
        }
        else
            break;

    }

    AppData.DataUnitLen = 1 + 6*DeleteCnt;
    AppData.DataUnitBuff[0] = DeleteCnt;

    return CommFunDeal(AppData, &RecvAppData, nArrIndex);
}


/*******************************************************************************
* ��������: MeterFileMofifyDeal
* ��������: ������޸Ĵ���
* �������: TASK_FUN_INPUT stTaskFunInput   �ز����������Ϣ
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0               ʧ��
*******************************************************************************/
int MeterFileMofifyDeal(TASK_FUN_INPUT stTaskFunInput)
{
	int 					ret;
    AppData_T				AppData;
    AppData_T				RecvAppData;

	uint8       nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	//printx(" MeterFileMofifyDeal(TASK_FUN_INPUT stTaskFunInput)�����б�[%d]�޸�...begin...\n", nTtlIndex);

    switch(gTotalState[nTtlIndex].MeterFileModify)
    {
    case MFMS_IDLE: 							//����
    	CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileMofifyDeal()�����б�[%d]�޸�...����MFMS_IDLE...\n", nTtlIndex);

		ReadAmrInfo(nTtlIndex);  //���ص�����Ϣ��������Ϣ
		
        if(gMeterFilesModifyType[nTtlIndex] == FILES_MODEFY_UPDATE)
        {
			CCODOWN_FMT_DEBUG(nTtlIndex, "==>  ����F10��ʽ�����޸Ĵ���FILES_MODEFY_UPDATE \n");
			gTotalState[nTtlIndex].MeterFileModify = MFMS_FILE_DELETE;
		}
        else
        {
			CCODOWN_FMT_DEBUG(nTtlIndex, "==>  ���������ʼ����ʽ�����޸Ĵ���FILES_MODEFY_CLEAR \n");
			gTotalState[nTtlIndex].MeterFileModify = MFMS_FILE_CLEAR;
		}
        break;

    case MFMS_FILE_CLEAR:						//�������
        CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileMofifyDeal()�����б�[%d]�޸�...�������MFMS_FILE_CLEAR...\n", nTtlIndex);
		AppData.AFN = 0x01;
        AppData.Fn = 2;
        AppData.DataUnitLen = 0;
        ret = CommFunDeal(AppData, &RecvAppData, nTtlIndex);
        if (ret > 0)							//��ɣ��л�����һ״̬
        {
            gTotalState[nTtlIndex].MeterFileModify = MFMS_FILE_ADD;
        }
        else
        {
            gTotalState[nTtlIndex].MeterFileModify  = MFMS_IDLE;
        }
        break;


    case MFMS_FILE_ADD: 						//��������
        CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileMofifyDeal()�����б�[%d]�޸�...��������MFMS_FILE_ADD...\n", nTtlIndex);
		if(gToAddFilesList[nTtlIndex].nMeterFilesNum > 0)
        {
            ret = Comm_AddSlaveInfo_New(&gToAddFilesList[nTtlIndex], MODULEDEAL_CNT, nTtlIndex);
			CCODOWN_FMT_DEBUG(nTtlIndex, "�����б�[%d]�޸� Comm_AddSlaveInfo_New()...ret=%d...\n", nTtlIndex, ret);
            if(ret > 0)
            {
                
            }
        }
        else
        {
        	gTotalState[nTtlIndex].MeterFileModify = MFMS_RESTART;
            
        }
        break;

    case MFMS_FILE_DELETE:							 //����ɾ��
        CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileMofifyDeal()�����б�[%d]�޸�...����ɾ��MFMS_FILE_DELETE...\n", nTtlIndex);
		if(gToDeleteFilesList[nTtlIndex].nMeterFilesNum > 0)
        {
			ret = Comm_DeleteRouterInfo_New(&gToDeleteFilesList[nTtlIndex], MODULEDEAL_CNT, nTtlIndex);
			CCODOWN_FMT_DEBUG(nTtlIndex, "�����б�[%d]�޸� Comm_DeleteRouterInfo_New()...ret=%d...\n", nTtlIndex, ret);
            if(ret > 0)
            {
                
            }
		}
        else
        {
        	gTotalState[nTtlIndex].MeterFileModify = MFMS_FILE_ADD;
            
        }
        break;

    case MFMS_RESTART:							//����
    	
		CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileMofifyDeal()�����б�[%d]�޸�...����MFMS_RESTART...\n", nTtlIndex);

		if((gToAddFilesList[nTtlIndex].nMeterFilesNum > 0) || (gToDeleteFilesList[nTtlIndex].nMeterFilesNum > 0))
        {
			CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileMofifyDeal()�����б�[%d]������δȫ��ͬ��, ��Ҫ�´μ���...MFMS_RESTART...\n", nTtlIndex);
			gMeterFileModifyFlag[nTtlIndex] = 1;
		}
		else
		{				
			gMeterFileModifyState[nTtlIndex] = 0;
            gMeterFileCompareFlag[nTtlIndex] = 1;
		}
        
		gTotalState[nTtlIndex].MeterFileModify = MFMS_IDLE;
		return 1;

    default:
        gTotalState[nTtlIndex].MeterFileModify = MFMS_IDLE;
        break;
    }

    return -1;
}

/*******************************************************************************
* ��������: ActReportNodeDeal
* ��������: �����ϱ��ڵ㴦��
* �������: ��
* �������: uint8 *ReportInfo                  �ϱ��ӽڵ���Ϣ
            uint8 *RealPhase               ʵ����λ
            uint8 *AddrSearchReportType    �ϱ��ӽڵ�ģʽ
            AppData_T *AppData             �ϱ��ӽڵ㱨��
            uint8 nArrIndex                ģ���߳�����(0:�����һ��CCOģ��; 1:����ڶ���CCOģ��)
* �� �� ֵ: >0                  �ϱ��ӽڵ�����
           =-1               ���ϱ�
           =-2               ע�����
*******************************************************************************/
int ReportInfoDeal(uint8 *ReportInfo, uint8 *RealPhase, uint8 *AddrSearchReportType, AppData_T *AppData, uint8 nArrIndex)
{
    if((AppData->AFN == 0x06) && (AppData->Fn == 1))
    {
        //����ϱ���Ϣ
        memcpy(ReportInfo, &AppData->DataUnitBuff[1], (AppData->DataUnitLen-1));

        *AddrSearchReportType  = 1;

        *RealPhase = (AppData->RInfo[2] & 0x0f); //��λ
        *RealPhase += (AppData->RInfo[3] & 0xf0); //�ź�Ʒ��

        //ȷ��֡Ӧ��
        Comm_ConfirmAckTakeFrameSeq(*AppData, nArrIndex);

        return AppData->DataUnitBuff[0];
    }

    if ((AppData->AFN == 0x06) && (AppData->Fn == 4))
    {
        //����ϱ���Ϣ
        memcpy(ReportInfo, &AppData->DataUnitBuff[1], (AppData->DataUnitLen-1));

        *AddrSearchReportType  = 4;

        *RealPhase = (AppData->RInfo[2] & 0x0f); //��λ
        *RealPhase += (AppData->RInfo[3] & 0xf0); //�ź�Ʒ��

        //ȷ��֡Ӧ��
        Comm_ConfirmAckTakeFrameSeq(*AppData, nArrIndex);

        return AppData->DataUnitBuff[0];
    }

    return -1;
}


/*******************************************************************************
* ��������: ActReportNodeDeal
* ��������: �����ϱ��ڵ㴦��
* �������: ��
* �������: ReportInfo          �ϱ��ӽڵ���Ϣ
*           RealPhase           ʵ����λ
            AddrSearchReportType �ϱ��ӽڵ�ģʽ

* �� �� ֵ: >0                  �ϱ��ӽڵ�����
*           =-1                 ���ϱ�
*           =-2                 ע�����
*******************************************************************************/
int ActReportNodeDeal(uint8 *ReportInfo, uint8 *RealPhase, uint8 *AddrSearchReportType, uint8 nArrIndex)
{
    int     RecvLen;                            //�������ݳ���
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN];        //�������ݻ�����
    AppData_T   AppData;                        //Ӧ�ò�����
    
    uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = nArrIndex;

    //��ȡ����
    RecvLen = PlcReadQueue(&gAmrRecvDownSearchMeter[nTtlIndex], RecvBuf);
	
    if (RecvLen <= 0)
    {
        return -1;
    }

    //���ݽ�֡
    if (UnPackFrameData(RecvLen, RecvBuf, &AppData) != CHECK_RESULT_SUCCEED)
    {
        return -1;
    }

    if((AppData.AFN == 0x06) && (AppData.Fn == 1 || AppData.Fn == 4))
    {
        return ReportInfoDeal(ReportInfo, RealPhase, AddrSearchReportType, &AppData, nArrIndex);
    }

    if ((AppData.AFN == 0x06) && (AppData.Fn == 3))//�ϱ�·�ɹ����䶯��Ϣ 1:�����������  2:�ѱ��������
    {
        if (AppData.DataUnitBuff[0] == 0x02)
        {
            //ȷ��֡Ӧ��
            Comm_ConfirmAckTakeFrameSeq(AppData, nTtlIndex);
            return -2;
        }
        //ȷ��֡Ӧ��
        Comm_ConfirmAckTakeFrameSeq(AppData, nTtlIndex);
    }

    return -1;
}

/*******************************************************************************
* ��������: SaveReportInfo
* ��������: �����ϱ���Ϣ
* �������: ReportCnt           �ϱ�����
*           ReportInfo          �ϱ���Ϣ
*           RealPhase           ʵ����λ
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void SaveReportInfo_F1(uint8 ReportCnt, uint8 *ReportInfo, uint8 RealPhase, uint8 nArrIndex)
{
    CCODOWN_FMT_DEBUG(nArrIndex, "SaveReportInfo_F1(ReportCnt[%d], *ReportInfo[%d], RealPhase[%d], nArrIndex[%d])...begin \n", ReportCnt, *ReportInfo, RealPhase, nArrIndex);
    ACT_REPORT_INFO_T ActInfo;
    uint16  MeterCnt = 0;                       //�����Ŀ
    uint16  CurIndex = 0;                       //��ǰ����
    //uint16  SearchFilesIndex = 0;               //�������(�����ϱ����,��ԭʼ�������)
    int     i;
    int     pn = 0;

    AMR_METER_SEARCH_REPORT_T AmrMeterSearchReport;
    memset((uint8 *)&AmrMeterSearchReport, 0x00, sizeof(AmrMeterSearchReport));

    INFO_DATA_T InfoMsgData;
    memset((uint8 *)&InfoMsgData, 0x00, sizeof(InfoMsgData));

    uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
    nTtlIndex = nArrIndex;

    //����ϱ���Ϣ
    for (i = 0; i < ReportCnt; i++)
    {
        //�ز����ַ
        memcpy(ActInfo.MeterAddr, &ReportInfo[CurIndex], 6);  //ע�⣺ReportInfo���������ֽڡ��ϱ��ӽڵ������n����ֱ�Ӵӵ�ַ��ʼ
        CurIndex += 6;

        ActInfo.PrtlMode = ReportInfo[CurIndex]; //��Լ����
        CurIndex += 1;

        CurIndex += 2; //�����ӽڵ����
        CCODOWN_FMT_DEBUG(nTtlIndex, "���豸Ϊ�ز��� \n");
        
        //��Ч�Լ��
        if(1)
        {
            //�ز����ַ
            memcpy(AmrMeterSearchReport.stAmrMeterFiles[MeterCnt].aMeterFilesAddr, ActInfo.MeterAddr, 6);  //ע�⣺ReportInfo���������ֽڡ��ϱ��ӽڵ������n����ֱ�Ӵӵ�ַ��ʼ
            CurIndex += 6;

            //��Լ����
            AmrMeterSearchReport.stAmrMeterFiles[MeterCnt].nProtocolType = ActInfo.PrtlMode; 
            CurIndex += 1;

            //�����ӽڵ����
            CurIndex += 2; 

            MeterCnt++;
        }

        CCODOWN_FMT_DEBUG(nTtlIndex, "�� %d ������ַ: %02x %02x %02x %02x %02x %02x, Э��:%02x \n", i, 
            ActInfo.MeterAddr[5], ActInfo.MeterAddr[4], ActInfo.MeterAddr[3], ActInfo.MeterAddr[2], ActInfo.MeterAddr[1], ActInfo.MeterAddr[0], ActInfo.PrtlMode);
    }

    CCODOWN_FMT_DEBUG(nTtlIndex, "SaveReportInfo_F1() ����֮�� MeterCnt=%d \n", MeterCnt);
    if(MeterCnt <= 0)
    {
        CCODOWN_FMT_DEBUG(nTtlIndex, "SaveReportInfo_F1() if(MeterCnt <= 0) ֱ�ӷ���! \n", MeterCnt);
        return;
    }

    //��װ��Ϣ����
    InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
    InfoMsgData.IP  = ntohl(InfoMsgData.IP);
    InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
    InfoMsgData.MsgPRIORITY = 0;
    InfoMsgData.MsgRPM = 1;
    InfoMsgData.MsgIndex = GetInfoIndex();
    InfoMsgData.MsgLabel = nTtlIndex;  //mqtt��Ŷ˿����(0��1)
    InfoMsgData.SourLen = strlen(gCcoRouterAppName);
    memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
    memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
    InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
    memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
    memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
                
    CurIndex = 0;
    InfoMsgData.IID_ID = 0x0003;
    InfoMsgData.IOP_ID = 0x0036;

    Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurIndex]);  //�˿ں�
    CurIndex += 4;
    InfoMsgData.aPayloadData[CurIndex] = 0x00;  //�Ƿ��г�����Ϣ��־, 0x00:��, 0x01:��
    CurIndex += 1;
    ChangeLenToBuff(MeterCnt, &CurIndex, &InfoMsgData.aPayloadData[CurIndex]);  //��Ƶ����ĸ���
    for (i = 0; i < MeterCnt; i++)
    {
        pn = GetPnByMeterAddr(AmrMeterSearchReport.stAmrMeterFiles[i].aMeterFilesAddr, nTtlIndex);
        if(pn <= 0) //����û�иõ���������յ��ı���Ϊ׼
        {
            //SearchFilesIndex = gSearchMeterFilesIndex[nTtlIndex] ++;
            //Uint16ToCharArr(SearchFilesIndex, &InfoMsgData.aPayloadData[CurIndex]);  //�������
            //CurIndex += 2;
            //CurIndex += 1;  //������ַ����
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], AmrMeterSearchReport.stAmrMeterFiles[i].aMeterFilesAddr, 6);  //������ַ
            CurIndex += 6;
            //CurIndex += 1;  //����mac����
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], AmrMeterSearchReport.stAmrMeterFiles[i].aMeterFilesAddr, 6);  //����mac
            CurIndex += 6; 
            InfoMsgData.aPayloadData[CurIndex] = AmrMeterSearchReport.stAmrMeterFiles[i].nProtocolType;  //Э������(�˴��Ե���Ϊ׼, �������յ��ı���Ϊ׼?)
            CurIndex += 1;
            //InfoMsgData.aPayloadData[CurIndex] = 0;  //ͨ�Ų���
            //CurIndex += 1; 
        }
        else
        {
            //SearchFilesIndex = gSearchMeterFilesIndex[nTtlIndex] ++;
            //Uint16ToCharArr(SearchFilesIndex, &InfoMsgData.aPayloadData[CurIndex]);  //�������
            //CurIndex += 2;
            //CurIndex += 1;  //������ַ����
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].aMeterFilesAddr, 6);  //������ַ
            CurIndex += 6;
            //CurIndex += 1;  //����mac����
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].aMeterFilesMac, 6);  //����mac
            CurIndex += 6; 
            InfoMsgData.aPayloadData[CurIndex] = gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].nProtocolType;  //Э������(�˴��Ե���Ϊ׼, �������յ��ı���Ϊ׼?)
            CurIndex += 1;
            //InfoMsgData.aPayloadData[CurIndex] = gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].nCommPara;  //ͨ�Ų���
            //CurIndex += 1;  
        }
        
    }
    InfoMsgData.nPayloadLength = CurIndex;
    PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
    return;
}

/*******************************************************************************
* ��������: SaveReportInfo
* ��������: �����ϱ���Ϣ
* �������: ReportCnt           �ϱ�����
*           ReportInfo          �ϱ���Ϣ
*           RealPhase           ʵ����λ
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void SaveReportInfo_F4_Collector(uint8 ReportCnt, uint8 *ReportInfo, uint8 RealPhase, uint8 nArrIndex)
{
    CCODOWN_FMT_DEBUG(nArrIndex, "SaveReportInfo_F4_Collector(ReportCnt[%d], *ReportInfo[%d], RealPhase[%d], nArrIndex[%d])...begin \n", ReportCnt, *ReportInfo, RealPhase, nArrIndex);
    
    uint16  MeterCnt = 0;                       //�����Ŀ
    int     SlaveCnt = 0;                       //�ӽڵ�����
    uint16  CurIndex = 0;                       //��ǰ����
    //uint16  SearchFilesIndex = 0;               //�������(�����ϱ����,��ԭʼ�������)
    int     i,j;
    int     pn = 0;

    ACT_REPORT_INFO_T ActInfo;
    memset((uint8 *)&ActInfo, 0x00, sizeof(ACT_REPORT_INFO_T));
    AMR_METER_SEARCH_REPORT_T AmrMeterSearchReport;
    memset((uint8 *)&AmrMeterSearchReport, 0x00, sizeof(AmrMeterSearchReport));
    INFO_DATA_T InfoMsgData;
    memset((uint8 *)&InfoMsgData, 0x00, sizeof(InfoMsgData));

    uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
    nTtlIndex = nArrIndex;

    //����ϱ���Ϣ
    for (i = 0; i < ReportCnt; i++)
    {
        //�ɼ�����ַ
        memcpy(ActInfo.AcqUnitId, &ReportInfo[CurIndex], 6);   //ע�⣺ReportInfo���������ֽڡ��ϱ��ӽڵ������n����ֱ�Ӵӵ�ַ��ʼ
        CurIndex += 6;
        CCODOWN_FMT_DEBUG(nTtlIndex, "�ɼ�����ַ: %02x %02x %02x %02x %02x %02x \n", 
                    ActInfo.AcqUnitId[5], ActInfo.AcqUnitId[4], ActInfo.AcqUnitId[3], ActInfo.AcqUnitId[2], ActInfo.AcqUnitId[1], ActInfo.AcqUnitId[0]);

        //�����ɼ�����Լ����š��豸���ͼ��½Ӵӽڵ�������
        CurIndex += 5;

        //�ϱ��ӽڵ�����
        SlaveCnt = ReportInfo[CurIndex];
        CurIndex += 1;
        CCODOWN_FMT_DEBUG(nTtlIndex, "���ɼ����²�������:%d \n", SlaveCnt);

        //�ǿղɼ���
        if (SlaveCnt > 0)
        {
            //ѭ������ϱ��Ĵӽڵ�
            for (j = 0; j < SlaveCnt; j++)
            {
                //�ӽڵ��ַ
                memcpy(ActInfo.MeterAddr, &ReportInfo[CurIndex], 6);
                CurIndex += 6;

                //��Լ����
                if (ReportInfo[CurIndex] == 0x01)
                {
                    ActInfo.PrtlMode = 0x01;
                }
                else if(ReportInfo[CurIndex] == 0x02)
                {
                    ActInfo.PrtlMode = 0x02;
                }
                else if(ReportInfo[CurIndex] == 0x03)
                {
                    ActInfo.PrtlMode = 0x03;
                }
                else
                {
                    ActInfo.PrtlMode = 0x00;
                }

                CurIndex += 1;

                //��Ч�Լ��
                if(TRUE)
                {
                    //�ӽڵ��ַ
                    memcpy(AmrMeterSearchReport.stAmrMeterFiles[MeterCnt].aMeterFilesAddr, ActInfo.MeterAddr, 6);  //ע�⣺ReportInfo���������ֽڡ��ϱ��ӽڵ������n����ֱ�Ӵӵ�ַ��ʼ
                    
                    //��Լ����
                    AmrMeterSearchReport.stAmrMeterFiles[MeterCnt].nProtocolType = ActInfo.PrtlMode;

                    MeterCnt ++;
                }          

                CCODOWN_FMT_DEBUG(nTtlIndex, "�� %d ������ַ: %02x %02x %02x %02x %02x %02x, Э��:%02x \n", j, 
                    ActInfo.MeterAddr[5], ActInfo.MeterAddr[4], ActInfo.MeterAddr[3], ActInfo.MeterAddr[2], ActInfo.MeterAddr[1], ActInfo.MeterAddr[0], ActInfo.PrtlMode);
            }
        }
    }

    CCODOWN_FMT_DEBUG(nTtlIndex, "SaveReportInfo_F4_Collector() ����֮�� MeterCnt=%d \n", MeterCnt);
    if(MeterCnt <= 0)
    {
        CCODOWN_FMT_DEBUG(nTtlIndex, "SaveReportInfo_F4_Collector() if(MeterCnt <= 0) ֱ�ӷ���! \n", MeterCnt);
        return;
    }

    //��װ��Ϣ����
    InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
    InfoMsgData.IP  = ntohl(InfoMsgData.IP);
    InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
    InfoMsgData.MsgPRIORITY = 0;
    InfoMsgData.MsgRPM = 1;
    InfoMsgData.MsgIndex = GetInfoIndex();
    InfoMsgData.MsgLabel = nTtlIndex;  //mqtt��Ŷ˿����(0��1)
    InfoMsgData.SourLen = strlen(gCcoRouterAppName);
    memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
    memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
    InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
    memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
    memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
    
    CurIndex = 0;
    InfoMsgData.IID_ID = 0x0003;
    InfoMsgData.IOP_ID = 0x0036;

    Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurIndex]);  //�˿ں�
    CurIndex += 4;
    InfoMsgData.aPayloadData[CurIndex] = 0x00;  //�Ƿ��г�����Ϣ��־, 0x00:��, 0x01:��
    CurIndex += 1;
    if(0 == AREA_ZHEJIANG)
    {
        ChangeLenToBuff(MeterCnt, &CurIndex, &InfoMsgData.aPayloadData[CurIndex]);  //��Ƶ����ĸ���
    }
    else
    {
        //�㽭������������: �ѱ����ĵ�һ��Ϊ�ɼ���, ͨ��Э��Ϊ����0xCC
        ChangeLenToBuff(1+MeterCnt, &CurIndex, &InfoMsgData.aPayloadData[CurIndex]);  //�ɼ�������+��Ƶ����ĸ���

        //���ɼ�����ַ
        //SearchFilesIndex = 0;
        //Uint16ToCharArr(SearchFilesIndex, &InfoMsgData.aPayloadData[CurIndex]);  //�������
        //CurIndex += 2;
        //CurIndex += 1;  //������ַ����
        memcpy_r(&InfoMsgData.aPayloadData[CurIndex], ActInfo.AcqUnitId, 6);  //������ַ
        CurIndex += 6;
        //CurIndex += 1;  //����mac����
        memcpy_r(&InfoMsgData.aPayloadData[CurIndex], ActInfo.AcqUnitId, 6);  //����mac
        CurIndex += 6; 
        InfoMsgData.aPayloadData[CurIndex] = 0xCC;  //Э������(��ʱ���ɼ���Э������Ϊ0xCC)
        CurIndex += 1;
        //InfoMsgData.aPayloadData[CurIndex] = 0;  //ͨ�Ų���
        //CurIndex += 1;  
    }

    //������ַ
    for (i = 0; i < MeterCnt; i++)
    {
        pn = GetPnByMeterAddr(AmrMeterSearchReport.stAmrMeterFiles[i].aMeterFilesAddr, nTtlIndex);
        if(pn <= 0) //����û�иõ���������յ��ı���Ϊ׼
        {
            //SearchFilesIndex = gSearchMeterFilesIndex[nTtlIndex] ++;
            //Uint16ToCharArr(SearchFilesIndex, &InfoMsgData.aPayloadData[CurIndex]);  //�������
            //CurIndex += 2;
            //CurIndex += 1;  //������ַ����
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], AmrMeterSearchReport.stAmrMeterFiles[i].aMeterFilesAddr, 6);  //������ַ
            CurIndex += 6;
            //CurIndex += 1;  //����mac����
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], AmrMeterSearchReport.stAmrMeterFiles[i].aMeterFilesAddr, 6);  //����mac
            CurIndex += 6; 
            InfoMsgData.aPayloadData[CurIndex] = AmrMeterSearchReport.stAmrMeterFiles[i].nProtocolType;  //Э������(�˴��Ե���Ϊ׼, �������յ��ı���Ϊ׼?)
            CurIndex += 1;
            //InfoMsgData.aPayloadData[CurIndex] = 0;  //ͨ�Ų���
            //CurIndex += 1; 
        }
        else
        {
            //SearchFilesIndex = gSearchMeterFilesIndex[nTtlIndex] ++;
            //Uint16ToCharArr(SearchFilesIndex, &InfoMsgData.aPayloadData[CurIndex]);  //�������
            //CurIndex += 2;
            //CurIndex += 1;  //������ַ����
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].aMeterFilesAddr, 6);  //������ַ
            CurIndex += 6;
            //CurIndex += 1;  //����mac����
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].aMeterFilesMac, 6);  //����mac
            CurIndex += 6; 
            InfoMsgData.aPayloadData[CurIndex] = gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].nProtocolType;  //Э������(�˴��Ե���Ϊ׼, �������յ��ı���Ϊ׼?)
            CurIndex += 1;
            //InfoMsgData.aPayloadData[CurIndex] = gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].nCommPara;  //ͨ�Ų���
            //CurIndex += 1;  
        }
        
    }
    InfoMsgData.nPayloadLength = CurIndex;
    PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
    return;
    
}

/*******************************************************************************
* ��������: SaveReportInfo
* ��������: �����ϱ���Ϣ
* �������: ReportCnt           �ϱ�����
*           ReportInfo          �ϱ���Ϣ
*           RealPhase           ʵ����λ
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void SaveReportInfo_F4_Meter(uint8 ReportCnt, uint8 *ReportInfo, uint8 RealPhase, uint8 nArrIndex)
{
    CCODOWN_FMT_DEBUG(nArrIndex, "SaveReportInfo_F4_Meter(ReportCnt[%d], *ReportInfo[%d], RealPhase[%d], nArrIndex[%d])...begin \n", ReportCnt, *ReportInfo, RealPhase, nArrIndex);
    
    uint16  MeterCnt = 0;                       //�����Ŀ
    int     SlaveCnt = 0;                       //�ӽڵ�����
    uint16  CurIndex = 0;                       //��ǰ����
    //uint16  SearchFilesIndex = 0;               //�������(�����ϱ����,��ԭʼ�������)
    int     i,j;
    int     pn = 0;

    ACT_REPORT_INFO_T ActInfo;
    memset((uint8 *)&ActInfo, 0x00, sizeof(ACT_REPORT_INFO_T));
    AMR_METER_SEARCH_REPORT_T AmrMeterSearchReport;
    memset((uint8 *)&AmrMeterSearchReport, 0x00, sizeof(AmrMeterSearchReport));
    INFO_DATA_T InfoMsgData;
    memset((uint8 *)&InfoMsgData, 0x00, sizeof(InfoMsgData));

    uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
    nTtlIndex = nArrIndex;

    //����ϱ���Ϣ
    for (i = 0; i < ReportCnt; i++)
    {
        //�ز����ַ
        memcpy(ActInfo.MeterAddr, &ReportInfo[CurIndex], 6);  //ע�⣺ReportInfo���������ֽڡ��ϱ��ӽڵ������n����ֱ�Ӵӵ�ַ��ʼ
        CurIndex += 6;

        //�ز���ͨ��Э��
        ActInfo.PrtlMode = ReportInfo[CurIndex];
        CurIndex += 1;
        
        //������š��豸���ͼ��½Ӵӽڵ�������
        CurIndex += 4;

        //�ϱ��ӽڵ�����
        SlaveCnt = ReportInfo[CurIndex];
        CurIndex += 1;
        CCODOWN_FMT_DEBUG(nTtlIndex, "���豸Ϊ�ز��� \n");
    
       //���ز���
        if (SlaveCnt == 0)
        {
            //�ز����ַ
            memcpy(AmrMeterSearchReport.stAmrMeterFiles[MeterCnt].aMeterFilesAddr, ActInfo.MeterAddr, 6);  //ע�⣺ReportInfo���������ֽڡ��ϱ��ӽڵ������n����ֱ�Ӵӵ�ַ��ʼ
            
            //�ز���ͨ��Э��
            AmrMeterSearchReport.stAmrMeterFiles[MeterCnt].nProtocolType = ActInfo.PrtlMode;

            MeterCnt ++;
        }
        //�ǿ��ز���
        else
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "�ϱ���Ϣ���� \n");
            for (j = 0; j < SlaveCnt; j++)
            {
                CurIndex += 7;
            }
        }

        CCODOWN_FMT_DEBUG(nTtlIndex, "�� %d ������ַ: %02x %02x %02x %02x %02x %02x, Э��:%02x \n", i, 
            ActInfo.MeterAddr[5], ActInfo.MeterAddr[4], ActInfo.MeterAddr[3], ActInfo.MeterAddr[2], ActInfo.MeterAddr[1], ActInfo.MeterAddr[0], ActInfo.PrtlMode);
    }

    CCODOWN_FMT_DEBUG(nTtlIndex, "SaveReportInfo_F4_Meter() ����֮�� MeterCnt=%d \n", MeterCnt);
    if(MeterCnt <= 0)
    {
        CCODOWN_FMT_DEBUG(nTtlIndex, "SaveReportInfo_F4_Meter() if(MeterCnt <= 0) ֱ�ӷ���! \n", MeterCnt);
        return;
    }

    //��װ��Ϣ����
    InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
    InfoMsgData.IP  = ntohl(InfoMsgData.IP);
    InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
    InfoMsgData.MsgPRIORITY = 0;
    InfoMsgData.MsgRPM = 1;
    InfoMsgData.MsgIndex = GetInfoIndex();
    InfoMsgData.MsgLabel = nTtlIndex;  //mqtt��Ŷ˿����(0��1)
    InfoMsgData.SourLen = strlen(gCcoRouterAppName);
    memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
    memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
    InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
    memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
    memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
    
    CurIndex = 0;
    InfoMsgData.IID_ID = 0x0003;
    InfoMsgData.IOP_ID = 0x0036;

    Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurIndex]);  //�˿ں�
    CurIndex += 4;
    InfoMsgData.aPayloadData[CurIndex] = 0x00;  //�Ƿ��г�����Ϣ��־, 0x00:��, 0x01:��
    CurIndex += 1;
    ChangeLenToBuff(MeterCnt, &CurIndex, &InfoMsgData.aPayloadData[CurIndex]);  //��Ƶ����ĸ���
    for (i = 0; i < MeterCnt; i++)
    {
        pn = GetPnByMeterAddr(AmrMeterSearchReport.stAmrMeterFiles[i].aMeterFilesAddr, nTtlIndex);
        if(pn <= 0) //����û�иõ���������յ��ı���Ϊ׼
        {
            //SearchFilesIndex = gSearchMeterFilesIndex[nTtlIndex] ++;
            //Uint16ToCharArr(SearchFilesIndex, &InfoMsgData.aPayloadData[CurIndex]);  //�������
            //CurIndex += 2;
            //CurIndex += 1;  //������ַ����
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], AmrMeterSearchReport.stAmrMeterFiles[i].aMeterFilesAddr, 6);  //������ַ
            CurIndex += 6;
            //CurIndex += 1;  //����mac����
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], AmrMeterSearchReport.stAmrMeterFiles[i].aMeterFilesAddr, 6);  //����mac
            CurIndex += 6; 
            InfoMsgData.aPayloadData[CurIndex] = AmrMeterSearchReport.stAmrMeterFiles[i].nProtocolType;  //Э������(�˴��Ե���Ϊ׼, �������յ��ı���Ϊ׼?)
            CurIndex += 1;
            //InfoMsgData.aPayloadData[CurIndex] = 0;  //ͨ�Ų���
            //CurIndex += 1; 
        }
        else
        {
            //SearchFilesIndex = gSearchMeterFilesIndex[nTtlIndex] ++;
            //Uint16ToCharArr(SearchFilesIndex, &InfoMsgData.aPayloadData[CurIndex]);  //�������
            //CurIndex += 2;
            //CurIndex += 1;  //������ַ����
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].aMeterFilesAddr, 6);  //������ַ
            CurIndex += 6;
            //CurIndex += 1;  //����mac����
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].aMeterFilesMac, 6);  //����mac
            CurIndex += 6; 
            InfoMsgData.aPayloadData[CurIndex] = gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].nProtocolType;  //Э������(�˴��Ե���Ϊ׼, �������յ��ı���Ϊ׼?)
            CurIndex += 1;
            //InfoMsgData.aPayloadData[CurIndex] = gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].nCommPara;  //ͨ�Ų���
            //CurIndex += 1;  
        }
        
    }
    InfoMsgData.nPayloadLength = CurIndex;
    PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
    return;
    
}


/*******************************************************************************
* ��������: SaveReportInfo
* ��������: �����ϱ���Ϣ
* �������: ReportCnt           �ϱ�����
*           ReportInfo          �ϱ���Ϣ
*           RealPhase           ʵ����λ
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void SaveReportInfo_F4(uint8 ReportCnt, uint8 *ReportInfo, uint8 RealPhase, uint8 nArrIndex)
{
	int 	CurIndex = 0;						//��ǰ����
	
	if(ReportInfo[CurIndex + 9] == DEV_COLLECTOR)  //�ɼ���
	{
		SaveReportInfo_F4_Collector(ReportCnt, ReportInfo, RealPhase, nArrIndex);
	}
	else if(ReportInfo[CurIndex + 9] == DEV_METER)
	{
		SaveReportInfo_F4_Meter(ReportCnt, ReportInfo, RealPhase, nArrIndex);
	}
	
}


/*******************************************************************************
* ��������: AddrSearchDeal
* ��������: �����������
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int AddrSearchDeal(TASK_FUN_INPUT stTaskFunInput)
{
	static int      StudyDelayCnt[MAX_DOWN_THREAD_NUM] = {0};        //ѧϰ��ʱ����
	static uint16	StudyTime[MAX_DOWN_THREAD_NUM] = {0}; 			 //ѧϰʱ��
	uint8			ReportInfo[500] = {0};			    //�ϱ���Ϣ
	int 			ReportCnt;							//�ϱ�����
	uint8			RealPhase;							//ʵ����λ
	int 			ret;
	DateTime_T      stDateTime;
	static int      AccumulateWaitTime[MAX_DOWN_THREAD_NUM] = {0};             //�ۼƵȴ�ʱ��
	//time_t CurTimeSec;

	AppData_T		SendAppData,RecvAppData;
	uint8			AddrSearchReportType = 0;
	
	uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;
	
	//printx("AddrSearchDeal(TASK_FUN_INPUT stTaskFunInput)�����������,�˿�[%d]...begin \n", nTtlIndex);
	
	AMR_METER_START_SEARCH_T stAmrMeterStartSearch;
	memcpy(&stAmrMeterStartSearch, stTaskFunInput.InfoMsgBuff, sizeof(stAmrMeterStartSearch));

	switch (gTotalState[nTtlIndex].AddrSearch)
	{
	case AS_IDLE:								//����
		CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal(TASK_FUN_INPUT stTaskFunInput)�����������,�˿�[%d]...AS_IDLE \n", nTtlIndex);
		//��ȡע�����ʱ��
		StudyTime[nTtlIndex] = 5;  //������ʱ����Ϊ5min
		StudyTime[nTtlIndex] = stAmrMeterStartSearch.nContinuedTime;  //ʵ����ʱ
		if(StudyTime[nTtlIndex] == 0)
		{
			StudyTime[nTtlIndex] = 180;
			CCODOWN_FMT_DEBUG(nTtlIndex, "�ݴ� if(StudyTime[nTtlIndex] == 0) StudyTime[nTtlIndex] = 180; \n");
		}
		else if(StudyTime[nTtlIndex] > 1440)
		{
			StudyTime[nTtlIndex] = 180;
			CCODOWN_FMT_DEBUG(nTtlIndex, "�ݴ� if(StudyTime[nTtlIndex] > 1440) StudyTime[nTtlIndex] = 180; \n");
		}
		gTotalState[nTtlIndex].AddrSearch = AS_ACT_REG;
		CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal()�����ѱ� ���ע�����ʱ��:%d \n", StudyTime[nTtlIndex]);
		ReadAmrInfo(nTtlIndex);  //���ص�����Ϣ��������Ϣ
		gSearchMeterFilesIndex[nTtlIndex] = 0;  //�ѱ�����ų�ʼ��Ϊ0
		//�����ѱ�״̬
		gAmrStateQuery[nTtlIndex].nModuleSearchState = 1;
		break;

	case AS_ACT_REG:							//����ע��
		CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal(TASK_FUN_INPUT stTaskFunInput)�����������,�˿�[%d]...����ע��AS_ACT_REG \n", nTtlIndex);
		SendAppData.AFN = 0x11;
		SendAppData.Fn = 5;
		SendAppData.DataUnitLen = 10;

		/*
		CurTimeSec = BcdTimeToSec((uint8*)&gCurBcdTime);
		printx("CurTimeSec:%d \n", (int)CurTimeSec);
		memcpy(SendAppData.DataUnitBuff, (char*)&gCurBcdTime, 6);  //����ʱ����ʱ��Ϊ��ǰʱ��
		memcpy(&SendAppData.DataUnitBuff[6],&StudyTime,2);
		SendAppData.DataUnitBuff[8] = 1;
		SendAppData.DataUnitBuff[9] = 1;
		*/
				
		CCODOWN_FMT_DEBUG(nTtlIndex, "stAmrMeterStartSearch.nStartTime:%d \n", stAmrMeterStartSearch.nStartTime);
		stDateTime = SecToDateTime_T((time_t)stAmrMeterStartSearch.nStartTime);
		CCODOWN_BUF_DEBUG(nTtlIndex, (uint8*)&stDateTime, 6, "ʱ��");
		stDateTime = DateTimeToBCDDateTime_T(stDateTime);
		CCODOWN_BUF_DEBUG(nTtlIndex, (uint8*)&stDateTime, 6, "ʱ��");
		
		memcpy(SendAppData.DataUnitBuff, (char*)&stDateTime, 6);
		memcpy(&SendAppData.DataUnitBuff[6],&StudyTime[nTtlIndex],2);
		SendAppData.DataUnitBuff[8] = stAmrMeterStartSearch.nSlaveNodeReSendCount;
		SendAppData.DataUnitBuff[9] = stAmrMeterStartSearch.nRandWaitTimeSlice;	
		
		ret = CommFunDealByType(SendAppData, &RecvAppData, nTtlIndex, stTaskFunInput.TaskName);
		if (ret > 0)							//��ɣ��л�����һ״̬
		{
			gTotalState[nTtlIndex].AddrSearch = AS_WAIT_REPORT;
			StudyDelayCnt[nTtlIndex] = 0;
			AccumulateWaitTime[nTtlIndex] = 0;
		}
		break;

	case AS_WAIT_REPORT:						//�ȴ��ϱ�
		//printx("AddrSearchDeal(TASK_FUN_INPUT stTaskFunInput)�����������,�˿�[%d]...AS_WAIT_REPORT \n", nTtlIndex);
		//��ȡ�ϱ���Ϣ����
		ReportCnt = ActReportNodeDeal(ReportInfo, &RealPhase, &AddrSearchReportType, nTtlIndex);
		if (ReportCnt > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal()����������� �����ϱ���Ϣ, ���������:%d \n", ReportCnt);
			if(AddrSearchReportType == 1)
			{
				SaveReportInfo_F1(ReportCnt, ReportInfo, RealPhase, nTtlIndex);
			}
			else if(AddrSearchReportType == 4)
			{
				SaveReportInfo_F4(ReportCnt, ReportInfo, RealPhase, nTtlIndex);
			}
			else
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal()����������� AddrSearchReportType�ȷ�1, Ҳ��4... \n");
			}

		}
		else if (ReportCnt == -2)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal()����������� ���ע���ϱ�ʱ�����! \n");
			gTotalState[nTtlIndex].AddrSearch = AS_SEARCH_END;
		}
		else
		{
			//printx("�ȷ�if (ReportCnt > 0), Ҳ��ReportCnt == -2, �������...\n", ReportCnt);
			if (gPastTimeFlag[nTtlIndex].BitSect.Minute == 1)
            {
                AccumulateWaitTime[nTtlIndex]++;
                StudyDelayCnt[nTtlIndex]++;

                CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal()����������� ѧϰʱ���ۼ�:%d, �ۼƵȴ�ʱ��:%d. \n", StudyDelayCnt[nTtlIndex], AccumulateWaitTime[nTtlIndex]);
                if (StudyDelayCnt[nTtlIndex] > StudyTime[nTtlIndex] && (StudyTime[nTtlIndex] != 0))
                {
                    CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal()����������� ���ע���ϱ�ʱ�����! \n");
                    gTotalState[nTtlIndex].AddrSearch = AS_SEARCH_END;
                }
                else
                {
                    //����ʱ�䳬��10����,��ѯһ��
                    if(AccumulateWaitTime[nTtlIndex] > 10)
                    {
                        gTotalState[nTtlIndex].AddrSearch = AS_GET_RUN_STATE;
                        CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal()����������� �ۼƵȴ�ʱ��:%d \n", AccumulateWaitTime[nTtlIndex]);

                        AccumulateWaitTime[nTtlIndex] = 0;
                    }

                }
            }
		}
		break;

	case AS_GET_RUN_STATE:						//��ȡ·������״̬
		CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal(TASK_FUN_INPUT stTaskFunInput)�����������,�˿�[%d]...��ȡ·������״̬AS_GET_RUN_STATE \n", nTtlIndex);
		SendAppData.AFN = 0x10;
		SendAppData.Fn = 4;
		SendAppData.DataUnitLen = 0;
		ret = CommFunDealByType(SendAppData, &RecvAppData, nTtlIndex, stTaskFunInput.TaskName);
		if (ret > 0)
		{
			if (RecvAppData.DataUnitBuff[0] & 0x02) 					//�ϱ����
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal()����������� �����ȴ�ע��! \n");
				gTotalState[nTtlIndex].AddrSearch = AS_WAIT_REPORT;
			}
			else
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal()����������� ע�����! \n");
				gTotalState[nTtlIndex].AddrSearch = AS_SEARCH_END;
			}
		}
		break;

	case AS_SEARCH_END: 						//��������
		CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal(TASK_FUN_INPUT stTaskFunInput)�����������,�˿�[%d]...��������AS_SEARCH_END \n", nTtlIndex);
		//����ж�״̬
		gTotalState[nTtlIndex].BreakState.AddrSearch = 0;
		gTotalState[nTtlIndex].AddrSearch = AS_IDLE;
		//�����ѱ�״̬
		gAmrStateQuery[nTtlIndex].nModuleSearchState = 0;
		return 1;

	default:
		gTotalState[nTtlIndex].AddrSearch = AS_IDLE;
		break;
	}

	return -1;
}

/*******************************************************************************
* ��������: DelSlaveInfoDeal
* ��������: ɾ���ӽڵ���Ϣ��������
* �������: 
* �������: 
* �� �� ֵ: 
*******************************************************************************/
int DelSlaveInfoDeal(TASK_FUN_INPUT stTaskFunInput)
{
	return 1;

}

/*******************************************************************************
* ��������:	CmdDataRelay
* ��������:	ȷ��֡Ӧ������
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
int CmdDataRelay(MeterPrtlFmt_T MeterData, uint8 *buf, uint32 buf_len, uint8 nArrIndex)
{
    uint8 tmpMeter[6] = {0};
    memset(tmpMeter,0x99,6);

    if(memcmp(MeterData.MtrAddr,tmpMeter,6) == 0)
    {
        Comm_DataRelay(MeterData, 2, buf, buf_len, nArrIndex);
    }
    else
    {
        Comm_DataRelay(MeterData, 1, buf, buf_len, nArrIndex);
    }
    return 1;
}



/*******************************************************************************
* ��������: RelayCmdMeterPrtlData
* ��������: ��������ٲ���֡����
* �������: 
* �������:
* �� �� ֵ: 
*******************************************************************************/
int RelayCmdMeterPrtlData(MeterPrtlFmt_T MeterPrtlData, uint8 *buf, uint32 buf_len, uint8 nArrIndex)
{   		
    uint16 Pn;	
	uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = nArrIndex;

	//��������Ϣ
	Pn = GetPnByMeterAddr(MeterPrtlData.MtrAddr, nTtlIndex);
	MeterPrtlData.DelayFlag = 0;//��ʱ��־
    ReadMtrPrtlMode(&MeterPrtlData.PrtlMode, Pn, nTtlIndex); //��Լ����,1:97,2:07, 3:oop
    ReadAcqUnitAddr(MeterPrtlData.AcqUnitAddr, Pn, nTtlIndex); //�ɼ�����ַ
    MeterPrtlData.MtrType = ReadMtrType(Pn, nTtlIndex);  //�������

	//��֡�·�
    CmdDataRelay(MeterPrtlData, buf, buf_len, nTtlIndex);

    return 1;
}


/*******************************************************************************
* ��������: RelayTransDeal
* ��������: ͸��ת��
* �������: 
* �������: 
* �� �� ֵ: 
*******************************************************************************/
int RelayTransDeal(TASK_FUN_INPUT stTaskFunInput)
{
	int 			Ret = -1;
	static time_t	CommBeginTime[MAX_DOWN_THREAD_NUM];						//ͨѶ��ʼʱ��
	int 			RecvLen;							//�������ݳ���
	uint8			RecvBuf[MAX_QUEUE_BUFF_LEN];		//�������ݻ�����
	AppData_T		SendAppData;							//Ӧ�ò�����
	AppData_T		AppData;							//Ӧ�ò�����
	int 			RecvResult = 1; 				    //���ս��(1:�ɹ� / -1:ʧ��)
	static uint32	OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};					//��ʱʱ��
	static	uint16	MaxRetrtCnt[MAX_DOWN_THREAD_NUM] = {0};					//������Դ���
	static	int 	CommRetryCnt[MAX_DOWN_THREAD_NUM] = {0};					//ͨѶ���Դ���
	static MeterPrtlFmt_T	SendMeterPrtlData[MAX_DOWN_THREAD_NUM];		//����Լ��ʽ���ݣ�����Ϣ�л�ȡ�������·���
	MeterPrtlFmt_T	RecvMeterPrtlData;		//����Լ��ʽ���ݣ��·�֮���յ��Ļظ���
	uint16          offset;
	static BOOL     bProtocol3762[MAX_DOWN_THREAD_NUM] = {0};
	static uint8    nFrameIndex3762[MAX_DOWN_THREAD_NUM] = {0};
	uint8           nZeroUint8 = 0;
	
	uint8           nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	static AMR_DATA_TRANSMIT_T AmrDataTransmit[MAX_DOWN_THREAD_NUM];  //����͸��
	static INFO_DATA_T InfoMsgData[MAX_DOWN_THREAD_NUM];  //�ظ���Ϣ
	static INFO_DATA_HEAD_T InfoDataHead[MAX_DOWN_THREAD_NUM];  //��ϢԭʼHEAD


	switch (gTotalState[nTtlIndex].RelayTrans)
	{
	case INNER_RTS_IDLE://����ִ��,��ʼ��
		CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()��͸���� INNER_RTS_IDLE...�˿�[%d] \n", nTtlIndex);
		ClearCommRecvBuff(nTtlIndex);  //��ͨѶ���ջ�����
		ReadAmrInfo(nTtlIndex);  //���ص�����Ϣ��������Ϣ
		CommBeginTime[nTtlIndex] = time(NULL);
		OverTimeCnt[nTtlIndex] = 60;
		MaxRetrtCnt[nTtlIndex] = 0;
		memset(&SendMeterPrtlData[nTtlIndex], 0, sizeof(MeterPrtlFmt_T));
		memset(&AmrDataTransmit[nTtlIndex], 0, sizeof(AMR_DATA_TRANSMIT_T));
		memset(&InfoMsgData[nTtlIndex], 0, sizeof(INFO_DATA_T));
		memset(&InfoDataHead[nTtlIndex], 0, sizeof(INFO_DATA_HEAD_T));

		//�Ӷ����ж�ȡ͸������
		Ret = GetDataTransmitFromList(nTtlIndex, &AmrDataTransmit[nTtlIndex], &InfoDataHead[nTtlIndex]);
		if(Ret > 0)
		{
			memcpy(&InfoMsgData[nTtlIndex], &InfoDataHead[nTtlIndex], sizeof(INFO_DATA_HEAD_T));  //��ȡԭʼHEAD
			InfoMsgData[nTtlIndex].MsgRPM = 0;   //������־λ, �ظ���������
		}
		else
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal() ͸��ת��δ�鵽����, ֱ�ӽ���! �˿�:%d \n", nTtlIndex);
			gTotalState[nTtlIndex].RelayTrans = INNER_RTS_END;//�������
			break;
		}

		//ģ���ϱ��Ĵӽڵ㳬ʱʱ���8��ʱ����2����Ϊ�������������̴���ʱ��(����Ӧ�ô��յ���Ϣ��ʼ��ʱ)��
		if(AmrDataTransmit[nTtlIndex].nFrameTimeover >= 8)
		{
			OverTimeCnt[nTtlIndex] = AmrDataTransmit[nTtlIndex].nFrameTimeover - 3;
		}
		else
		{
			OverTimeCnt[nTtlIndex] = AmrDataTransmit[nTtlIndex].nFrameTimeover;
		}
		
		//ģ���ϱ��Ĵӽڵ㳬ʱʱ��С��10sʱ������Ϊ60s��
        //OverTimeCnt[nTtlIndex] = (OverTimeCnt[nTtlIndex] < 10) ? 60 : OverTimeCnt[nTtlIndex];
        CCODOWN_FMT_DEBUG(nTtlIndex, "�ӽڵ����ʱʱ�� ����OverTimeCnt[%d]=%d, AmrDataTransmit[%d].nFrameTimeover=%d \n", 
        	nTtlIndex, OverTimeCnt[nTtlIndex], nTtlIndex, AmrDataTransmit[nTtlIndex].nFrameTimeover);		
		
		//�ж�͸�������Ƿ�376.2����
		CCODOWN_FMT_DEBUG(nTtlIndex, "�ж�͸�������Ƿ�376.2���� \n");
		Ret = UnPackFrameData(AmrDataTransmit[nTtlIndex].nTransmitDataLen, AmrDataTransmit[nTtlIndex].pTransmitDataBuf, &SendAppData);
		if(Ret != CHECK_RESULT_FAILUER)
		{
			//376.2����ֱ�ӷ������з��Ͷ���, �����ݷ��͵���������
			CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()͸����Լ��֡ ����Э��Ϊ376.2...\n");
			bProtocol3762[nTtlIndex] = TRUE;
			nFrameIndex3762[nTtlIndex] = SendAppData.RInfo[5];			
			gTotalState[nTtlIndex].RelayTrans = INNER_RTS_SEND;  //ֱ����������
		}
		else
		{
			//���ΰ���645���ĺ�698���Ľ���
			CCODOWN_FMT_DEBUG(nTtlIndex, "���ΰ���645���ĺ�698���Ľ��� \n");
			bProtocol3762[nTtlIndex] = FALSE;
			nFrameIndex3762[nTtlIndex] = g_InerSeq[nTtlIndex];
			Ret = CheckMeterProtocol(AmrDataTransmit[nTtlIndex].nTransmitDataLen, AmrDataTransmit[nTtlIndex].pTransmitDataBuf, &SendMeterPrtlData[nTtlIndex], &offset);
			if(Ret > 0)
			{
				if(Ret == METER_PROTOCOL_645)
				{
					CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()����Լ��֡ ����Э��Ϊ645...\n");
				}
				else if(Ret == METER_PROTOCOL_69845)
				{
					CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()����Լ��֡ ����Э��Ϊ698.45...\n");
				}
				gTotalState[nTtlIndex].RelayTrans = INNER_RTS_SEND;  //������֡����
			}
			else
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()����Լ��֡ ���ĸ�ʽ��������...\n");
				gTotalState[nTtlIndex].RelayTrans = INNER_RTS_END;  //���Ľ�֡ʧ��, ֱ�ӽ���
				
				//��װ��Ϣ����
				PackInfoMsgStruct(0x0003, 0x0033, 0, (uint8*)&nZeroUint8, &InfoMsgData[nTtlIndex]);
				PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData[nTtlIndex], sizeof(MSG_INFO_T)+InfoMsgData[nTtlIndex].nPayloadLength);
			}
			
		}		
        
		break;

	case INNER_RTS_SEND: //����
		CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()��͸���� INNER_RTS_SEND...�˿�[%d] \n", nTtlIndex);
		if(bProtocol3762[nTtlIndex])  //ԭʼ͸��������376.2����
		{
			PlcWriteQueue(pAmrSendDown[nTtlIndex], AmrDataTransmit[nTtlIndex].pTransmitDataBuf, AmrDataTransmit[nTtlIndex].nTransmitDataLen);
			CommBeginTime[nTtlIndex] = time(NULL);
			gTotalState[nTtlIndex].RelayTrans = INNER_RTS_RECV;
		}
		else
		{
			Ret = RelayCmdMeterPrtlData(SendMeterPrtlData[nTtlIndex], AmrDataTransmit[nTtlIndex].pTransmitDataBuf, AmrDataTransmit[nTtlIndex].nTransmitDataLen, nTtlIndex);
			if(Ret > 0)
			{
				CommBeginTime[nTtlIndex] = time(NULL);
				gTotalState[nTtlIndex].RelayTrans = INNER_RTS_RECV;
			}
			else
			{
				gTotalState[nTtlIndex].RelayTrans = INNER_RTS_RECV;
			}
		}
		
		break;

	case INNER_RTS_RECV: //����
		RecvResult = 1;  //���ս��(1:�ɹ� / -1:ʧ��)
		RecvLen = PlcReadQueue(pAmrRecvDown[nTtlIndex], RecvBuf);
		if (RecvLen > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()��͸���� INNER_RTS_RECV...if (RecvLen > 0)...�˿�[%d] \n", nTtlIndex);
			Ret = UnPackFrameData(RecvLen, RecvBuf, &AppData);

			switch(Ret)
			{
			case CHECK_RESULT_FAILUER:
			case CHECK_RESULT_CONFIRM:
			case CHECK_RESULT_DENY:
			case CHECK_RESULT_SUCCEED:
				CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()��͸���� INNER_RTS_RECV...check���=%d (FAILUER:-1, CONFIRM:1, DENY:2, SUCCEED:0) \n", Ret);
				if(bProtocol3762[nTtlIndex])  //ԭʼ͸��������376.2����
				{
					if(AppData.RInfo[5] == nFrameIndex3762[nTtlIndex])
					{
						gTotalState[nTtlIndex].RelayTrans = INNER_RTS_END;//�������

						//��װ��Ϣ����
						PackInfoMsgStruct(0x0003, 0x0033, RecvLen, RecvBuf, &InfoMsgData[nTtlIndex]);
						PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData[nTtlIndex], sizeof(MSG_INFO_T)+InfoMsgData[nTtlIndex].nPayloadLength);
					}
					else
					{
						CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal() INNER_RTS_RECV 376.2�շ�֡��Ų�ƥ��...\n");
					}
				}
				else  //ԭʼ͸�����ķ�376.2����
				{
					if(((AppData.AFN == 0x13) || (AppData.AFN == 0x02)) && (AppData.Fn == 1))
					{
						if(AppData.AFN == 0x13)
						{
							if (AppData.DataUnitBuff[3] <= 1)
							{
								RecvResult = -1;
								break;
							}
							Ret = CheckMeterProtocol(AppData.DataUnitBuff[3], &AppData.DataUnitBuff[4], &RecvMeterPrtlData, &offset);
						}
						else
						{
							if (AppData.DataUnitBuff[1] <= 1)
							{
								RecvResult = -1;
								break;
							}
							Ret = CheckMeterProtocol(AppData.DataUnitBuff[1], &AppData.DataUnitBuff[2], &RecvMeterPrtlData, &offset);
						}

						if(Ret > 0)
	                    {
	                    	if(Ret == METER_PROTOCOL_645)
							{
								CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()����Լ��֡ ����Э��Ϊ645...\n");
							}
							else if(Ret == METER_PROTOCOL_69845)
							{
								CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()����Լ��֡ ����Э��Ϊ698.45...\n");
							}
							
	                        Ret = memcmp(SendMeterPrtlData[nTtlIndex].MtrAddr, RecvMeterPrtlData.MtrAddr, 6);
	                        //if(Ret == 0)
	                        //{
	                            gTotalState[nTtlIndex].RelayTrans = INNER_RTS_END;//�������

								//��װ��Ϣ����
								if(AppData.AFN == 0x13)
								{
									PackInfoMsgStruct(0x0003, 0x0033, AppData.DataUnitBuff[3], (uint8*)&AppData.DataUnitBuff[4], &InfoMsgData[nTtlIndex]);
								}
								else
								{
									PackInfoMsgStruct(0x0003, 0x0033, AppData.DataUnitBuff[1], (uint8*)&AppData.DataUnitBuff[2], &InfoMsgData[nTtlIndex]);
								}
								PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData[nTtlIndex], sizeof(MSG_INFO_T)+InfoMsgData[nTtlIndex].nPayloadLength);
	                        //}
	                        //else//��֡ʧ�ܼ����ȴ�
	                       // {
	                       // 	CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal() INNER_RTS_RECV �շ���ַ��ƥ��...\n");
	                       //     RecvResult = -1;
	                      //  }
	                    }
						else
						{
							CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()����Լ��֡ ���ĸ�ʽ��������...\n");
							CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal() INNER_RTS_RECV UnPackMeterData() 3762��֡ʧ��...\n");
						}
						
					}
				}
				
				break;

			}
		}
		else//�ж��Ƿ��Ѿ���ʱ
		{
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				RecvResult = -1;
				CCODOWN_FMT_DEBUG(nTtlIndex, "��ʱ...\n");
			}
		}


		if (RecvResult == -1)
		{
			CommRetryCnt[nTtlIndex]++; //���Դ���+1
			gTotalState[nTtlIndex].RelayTrans = INNER_RTS_RETRY; //�л�������״̬	
		}
		break;

	case INNER_RTS_RETRY:
		CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()��͸���� INNER_RTS_RETRY...�˿�[%d] \n", nTtlIndex);
		CCODOWN_FMT_DEBUG(nTtlIndex, "�������³��Խ׶� ��ǰ���Դ���:%d, ����������Դ���MaxRetrtCnt:%d \n", CommRetryCnt[nTtlIndex], MaxRetrtCnt[nTtlIndex]);
		if(CommRetryCnt[nTtlIndex] > MaxRetrtCnt[nTtlIndex])
		{
			gTotalState[nTtlIndex].RelayTrans = INNER_RTS_END; //�ﵽ������Դ���, �������

			//��װ��Ϣ����
			PackInfoMsgStruct(0x0003, 0x0033, 0, (uint8*)&nZeroUint8, &InfoMsgData[nTtlIndex]);			
			PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData[nTtlIndex], sizeof(MSG_INFO_T)+InfoMsgData[nTtlIndex].nPayloadLength);
		}
		else
		{
			gTotalState[nTtlIndex].RelayTrans = INNER_RTS_SEND; //�л������͵�״̬
		}

		break;

	case INNER_RTS_END:
		CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()��͸���� INNER_RTS_END...�˿�[%d] \n", nTtlIndex);
		CommRetryCnt[nTtlIndex] = 0;
		bProtocol3762[nTtlIndex] = FALSE;
        gTotalState[nTtlIndex].RelayTrans = INNER_RTS_IDLE;
		return 1;
		break;

	default:
		break;
	}

	return -1;

}


/*******************************************************************************
* ��������: SlaveMonitorDeal
* ��������: �ӽڵ�������
* �������: 
* �������: 
* �� �� ֵ: 
*******************************************************************************/
int SlaveMonitorDeal(TASK_FUN_INPUT stTaskFunInput)
{
	int 			Ret = -1;
	static time_t	CommBeginTime[MAX_DOWN_THREAD_NUM];						//ͨѶ��ʼʱ��
	int 			RecvLen;							//�������ݳ���
	uint8			RecvBuf[MAX_QUEUE_BUFF_LEN];		//�������ݻ�����
	AppData_T		AppData;							//Ӧ�ò�����
	int 			RecvResult = 1; 				    //���ս��(1:�ɹ� / -1:ʧ��)
	static uint32	OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};					//��ʱʱ��
	static	uint16	MaxRetrtCnt[MAX_DOWN_THREAD_NUM] = {0};					//������Դ���
	static	int 	CommRetryCnt[MAX_DOWN_THREAD_NUM] = {0};					//ͨѶ���Դ���
	static MeterPrtlFmt_T	SendMeterPrtlData[MAX_DOWN_THREAD_NUM];		//����Լ��ʽ���ݣ�����Ϣ�л�ȡ�������·���
	MeterPrtlFmt_T	RecvMeterPrtlData;		//����Լ��ʽ���ݣ��·�֮���յ��Ļظ���
	uint16          offset;
	uint16  IndexID = 0;  //ƫ��λ
	
	uint8           nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	static AMR_SLAVE_NODE_MONITOR_T  AmrSlaveNodeMonitor[MAX_DOWN_THREAD_NUM];  //�ӽڵ���
	static INFO_DATA_T  InfoMsgData[MAX_DOWN_THREAD_NUM];  //�ظ���Ϣ
	static INFO_DATA_HEAD_T InfoDataHead[MAX_DOWN_THREAD_NUM];  //��ϢԭʼHEAD

	switch (gTotalState[nTtlIndex].SlaveMonitor)
	{
	case INNER_RTS_IDLE://����ִ��,��ʼ��
		CCODOWN_FMT_DEBUG(nTtlIndex, "SlaveMonitorDeal()�ӽڵ��� INNER_RTS_IDLE...�˿�[%d] \n", nTtlIndex);
		ClearCommRecvBuff(nTtlIndex);  //��ͨѶ���ջ�����
		ReadAmrInfo(nTtlIndex);  //���ص�����Ϣ��������Ϣ
		CommBeginTime[nTtlIndex] = time(NULL);
		OverTimeCnt[nTtlIndex] = 60;
		MaxRetrtCnt[nTtlIndex] = 0;
		memset(&SendMeterPrtlData[nTtlIndex], 0, sizeof(MeterPrtlFmt_T));
		memset(&AmrSlaveNodeMonitor[nTtlIndex], 0, sizeof(AMR_SLAVE_NODE_MONITOR_T));
		memset(&InfoMsgData[nTtlIndex], 0, sizeof(INFO_DATA_T));
		memset(&InfoDataHead[nTtlIndex], 0, sizeof(INFO_DATA_HEAD_T));

		//�Ӷ����ж�ȡ�ӽڵ��ر���
		Ret = GetSlaveMonitorFromList(nTtlIndex, &AmrSlaveNodeMonitor[nTtlIndex], &InfoDataHead[nTtlIndex]);
		if(Ret > 0)
		{
			memcpy(&InfoMsgData[nTtlIndex], &InfoDataHead[nTtlIndex], sizeof(INFO_DATA_HEAD_T));  //��ȡԭʼHEAD
			InfoMsgData[nTtlIndex].MsgRPM = 0;   //������־λ, �ظ���������
		}
		else
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "SlaveMonitorDeal() �ӽڵ���δ�鵽����, ֱ�ӽ���! �˿�:%d \n", nTtlIndex);
			gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_END;//�������
			break;
		}
		
		//ģ���ϱ��Ĵӽڵ㳬ʱʱ���8��ʱ����2����Ϊ�������������̴���ʱ��(����Ӧ�ô��յ���Ϣ��ʼ��ʱ)��
		if(AmrSlaveNodeMonitor[nTtlIndex].nFrameTimeover >= 8)
		{
			OverTimeCnt[nTtlIndex] = AmrSlaveNodeMonitor[nTtlIndex].nFrameTimeover - 3;
		}
		else
		{
			OverTimeCnt[nTtlIndex] = AmrSlaveNodeMonitor[nTtlIndex].nFrameTimeover;
		}
		
		//ģ���ϱ��Ĵӽڵ㳬ʱʱ��С��10sʱ������Ϊ60s��
        //OverTimeCnt[nTtlIndex] = (OverTimeCnt[nTtlIndex] < 10) ? 60 : OverTimeCnt[nTtlIndex];
        CCODOWN_FMT_DEBUG(nTtlIndex, "�ӽڵ����ʱʱ�� ����OverTimeCnt[%d]=%d, ԭʼAmrSlaveNodeMonitor[%d].nFrameTimeover=%d \n", 
        	nTtlIndex, OverTimeCnt[nTtlIndex], nTtlIndex, AmrSlaveNodeMonitor[nTtlIndex].nFrameTimeover);

		//���ý�֡, ֱ�Ӵ���Ϣ��ȡ��Ϣ
		CCODOWN_FMT_DEBUG(nTtlIndex, "���ý�֡, ֱ�Ӵ���Ϣ��ȡ��Ϣ \n");
		memcpy(SendMeterPrtlData[nTtlIndex].MtrAddr, AmrSlaveNodeMonitor[nTtlIndex].aSlaveNodeAddr, 6);//��ַ
		SendMeterPrtlData[nTtlIndex].DataLen = AmrSlaveNodeMonitor[nTtlIndex].nDataBufLen;//���ݳ���
		memcpy(SendMeterPrtlData[nTtlIndex].DataBuff, AmrSlaveNodeMonitor[nTtlIndex].aDataBuf, SendMeterPrtlData[nTtlIndex].DataLen);//��������

		gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_SEND;  //������֡����
        
		break;

	case INNER_RTS_SEND: //����
		CCODOWN_FMT_DEBUG(nTtlIndex, "SlaveMonitorDeal()�ӽڵ��� INNER_RTS_SEND...�˿�[%d] \n", nTtlIndex);
		Ret = RelayCmdMeterPrtlData(SendMeterPrtlData[nTtlIndex], AmrSlaveNodeMonitor[nTtlIndex].aDataBuf, AmrSlaveNodeMonitor[nTtlIndex].nDataBufLen, nTtlIndex);
		if(Ret > 0)
		{
			CommBeginTime[nTtlIndex] = time(NULL);
			gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_RECV;
		}
		else
		{
			gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_RECV;
		}
		break;

	case INNER_RTS_RECV: //����
		RecvResult = 1;  //���ս��(1:�ɹ� / -1:ʧ��)
		RecvLen = PlcReadQueue(pAmrRecvDown[nTtlIndex], RecvBuf);
		//PrintBuf(RecvBuf, RecvLen);
		if (RecvLen > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "SlaveMonitorDeal()�ӽڵ��� INNER_RTS_RECV...�˿�[%d]...if (RecvLen > 0) \n", nTtlIndex);
			Ret = UnPackFrameData(RecvLen, RecvBuf, &AppData);

			switch(Ret)
			{
			case CHECK_RESULT_FAILUER:
			case CHECK_RESULT_CONFIRM:
				CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()�ӽڵ��� INNER_RTS_RECV...CHECK_RESULT_FAILUER/CONFIRMʧ��...�˿�[%d] \n", nTtlIndex);
				break;

			case CHECK_RESULT_DENY:
				gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_END;  //����֡, �������
				CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()�ӽڵ��� INNER_RTS_RECV...CHECK_RESULT_DENY����...�˿�[%d] \n", nTtlIndex);
				
				//��װ��Ϣ����
				InfoMsgData[nTtlIndex].IID_ID = 0x0003;
				InfoMsgData[nTtlIndex].IOP_ID = 0x0038;
				InfoMsgData[nTtlIndex].nPayloadLength = 0;
				memset(InfoMsgData[nTtlIndex].aPayloadData, 0x00, InfoMsgData[nTtlIndex].nPayloadLength);  //payload
				
				PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData[nTtlIndex], sizeof(MSG_INFO_T)+InfoMsgData[nTtlIndex].nPayloadLength);
				break;

			case CHECK_RESULT_SUCCEED:
				CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()�ӽڵ��� INNER_RTS_RECV...CHECK_RESULT_SUCCEED�ɹ�...�˿�[%d] \n", nTtlIndex);
				if(((AppData.AFN == 0x13) || (AppData.AFN == 0x02)) && (AppData.Fn == 1))
				{
					if(AppData.AFN == 0x13)  //AppData.DataUnitBuff:����ʱ��2�ֽڣ�Э��1�ֽڣ�����1�ֽ�
					{
						if (AppData.DataUnitBuff[3] <= 1)
						{
							RecvResult = -1;
							break;
						}
						Ret = CheckMeterProtocol(AppData.DataUnitBuff[3], &AppData.DataUnitBuff[4], &RecvMeterPrtlData, &offset);
					}
					else  //AppData.DataUnitBuff:Э��1�ֽڣ�����1�ֽ�
					{
						if (AppData.DataUnitBuff[1] <= 1)
						{
							RecvResult = -1;
							break;
						}
						Ret = CheckMeterProtocol(AppData.DataUnitBuff[1], &AppData.DataUnitBuff[2], &RecvMeterPrtlData, &offset);
					}

					if(Ret > 0)
                    {
                    	if(Ret == METER_PROTOCOL_645)
						{
							CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()����Լ��֡ ����Э��Ϊ645...\n");
						}
						else if(Ret == METER_PROTOCOL_69845)
						{
							CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()����Լ��֡ ����Э��Ϊ698.45...\n");
						}
						
                        Ret = memcmp(SendMeterPrtlData[nTtlIndex].MtrAddr, RecvMeterPrtlData.MtrAddr, 6);
                        if(Ret == 0)
                        {
                            gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_END;//�������

							//��װ��Ϣ����
							IndexID = 0;

							InfoMsgData[nTtlIndex].IID_ID = 0x0003;
							InfoMsgData[nTtlIndex].IOP_ID = 0x0038;
							if(AppData.AFN == 0x13)
							{
								ChangeLenToBuff(AppData.DataUnitBuff[3], &IndexID, InfoMsgData[nTtlIndex].aPayloadData);  //octet-string�䳤																	
								memcpy(&InfoMsgData[nTtlIndex].aPayloadData[IndexID], &AppData.DataUnitBuff[4], AppData.DataUnitBuff[3]);  //payload
								IndexID += AppData.DataUnitBuff[3];
								InfoMsgData[nTtlIndex].nPayloadLength = IndexID;  
							}
							else
							{
								ChangeLenToBuff(AppData.DataUnitBuff[1], &IndexID, InfoMsgData[nTtlIndex].aPayloadData);  //octet-string�䳤																	
								memcpy(&InfoMsgData[nTtlIndex].aPayloadData[IndexID], &AppData.DataUnitBuff[2], AppData.DataUnitBuff[1]);  //payload
								IndexID += AppData.DataUnitBuff[1];
								InfoMsgData[nTtlIndex].nPayloadLength = IndexID;  						
							}														
							
							PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData[nTtlIndex], sizeof(MSG_INFO_T)+InfoMsgData[nTtlIndex].nPayloadLength);
                        }
                        else//��֡ʧ�ܼ����ȴ�
                        {
                        	CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal() INNER_RTS_RECV �շ���ַ��ƥ��...\n");
                            RecvResult = -1;
                        }
                    }
					else
					{
						CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()����Լ��֡ ���ĸ�ʽ��������...\n");
						CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal() INNER_RTS_RECV UnPackMeterData() 3762��֡ʧ��...\n");
					}
					
				}

				break;

			}
		}
		else//�ж��Ƿ��Ѿ���ʱ
		{
			//printx("if (RecvLen С�ڵ��� 0)...\n");
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				RecvResult = -1;
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) ��ʱ...\n");
			}
		}


		if (RecvResult == -1)
		{
			CommRetryCnt[nTtlIndex]++; //���Դ���+1
			gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_RETRY; //�л�������״̬	
		}
		break;

	case INNER_RTS_RETRY:
		CCODOWN_FMT_DEBUG(nTtlIndex, "SlaveMonitorDeal()�ӽڵ��� INNER_RTS_RETRY...�˿�[%d] \n", nTtlIndex);
		CCODOWN_FMT_DEBUG(nTtlIndex, "�������³��Խ׶� ��ǰ���Դ���:%d, ����������Դ���MaxRetrtCnt:%d \n", CommRetryCnt[nTtlIndex], MaxRetrtCnt[nTtlIndex]);
		if(CommRetryCnt[nTtlIndex] > MaxRetrtCnt[nTtlIndex])
		{
			gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_END; //�ﵽ������Դ���, �������

			//��װ��Ϣ����
			InfoMsgData[nTtlIndex].IID_ID = 0x0003;
			InfoMsgData[nTtlIndex].IOP_ID = 0x0038;
			InfoMsgData[nTtlIndex].nPayloadLength = 0;
			memset(InfoMsgData[nTtlIndex].aPayloadData, 0x00, InfoMsgData[nTtlIndex].nPayloadLength);  //payload
			
			PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData[nTtlIndex], sizeof(MSG_INFO_T)+InfoMsgData[nTtlIndex].nPayloadLength);
		}
		else
		{
			gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_SEND; //�л������͵�״̬
		}

		break;

	case INNER_RTS_END:
		CCODOWN_FMT_DEBUG(nTtlIndex, "SlaveMonitorDeal()�ӽڵ��� INNER_RTS_END...�˿�[%d] \n", nTtlIndex);
		CommRetryCnt[nTtlIndex] = 0;
        gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_IDLE;
		return 1;
		break;

	default:
		break;
	}

	return -1;

}

/*******************************************************************************
* ��������: GetSlaveMonitorDelay
* ��������: ��ȡ�ӽڵ�����ʱ
* �������: ��
* �������: ��
* �� �� ֵ: 
*******************************************************************************/
int GetSlaveMonitorDelay(TASK_FUN_INPUT stTaskFunInput)
{
	static time_t	CommBeginTime[MAX_DOWN_THREAD_NUM];						//ͨѶ��ʼʱ��
	int 			SendLen;							//�������ݳ���
	int 			RecvLen;							//�������ݳ���
	uint8			RecvBuf[MAX_QUEUE_BUFF_LEN];		//�������ݻ�����
	int 			RecvResult = 1; 				    //���ս��(1:�ɹ� / -1:ʧ��)
	static uint32	OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};					//��ʱʱ��
	static uint16	MaxRetrtCnt[MAX_DOWN_THREAD_NUM] = {0};					//������Դ���
	static int 	    CommRetryCnt[MAX_DOWN_THREAD_NUM] = {0};			    //ͨѶ���Դ���
	uint16          IndexID = 0;  //ƫ��λ
	uint16          DelaySecNum;  //�ӽڵ�����ʱ
	static uint8    SendType[MAX_DOWN_THREAD_NUM] = {0};  //0x13:����13F1, 0x14:�ظ�14F3
	
	uint8           nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	AppData_T       SendAppData,RecvAppData;
	memset(&SendAppData, 0, sizeof(AppData_T));
	memset(&RecvAppData, 0, sizeof(AppData_T));

	static AMR_SLAVE_NODE_DELAY_TIME_T  AmrSlaveNodeMonitorTime[MAX_DOWN_THREAD_NUM];  //��ȡ�ӽڵ�����ʱ
	static INFO_DATA_T     InfoMsgData[MAX_DOWN_THREAD_NUM];  //�ظ���Ϣ
	static INFO_DATA_HEAD_T InfoDataHead[MAX_DOWN_THREAD_NUM];  //��ϢԭʼHEAD
	static uint8 BakBuff14F3[MAX_DOWN_THREAD_NUM][500];  //��������14F3����������

	switch (gTotalState[nTtlIndex].SlaveMonitorTime)
	{
	case INNER_RTS_IDLE://����ִ��,��ʼ��
		CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay()��ȡ�ӽڵ�����ʱ INNER_RTS_IDLE...�˿�[%d] \n", nTtlIndex);
		ClearCommRecvBuff(nTtlIndex);  //��ͨѶ���ջ�����
		ReadAmrInfo(nTtlIndex);  //���ص�����Ϣ��������Ϣ
		CommBeginTime[nTtlIndex] = time(NULL);
		OverTimeCnt[nTtlIndex] = 60;
		MaxRetrtCnt[nTtlIndex] = 0;
		memset(&AmrSlaveNodeMonitorTime[nTtlIndex], 0, sizeof(AMR_SLAVE_NODE_DELAY_TIME_T));
		memset(&InfoMsgData[nTtlIndex], 0, sizeof(INFO_DATA_T));
		memset(&InfoDataHead[nTtlIndex], 0, sizeof(INFO_DATA_HEAD_T));
		memset(&BakBuff14F3[nTtlIndex][0], 0, sizeof(BakBuff14F3[nTtlIndex]));
		SendType[nTtlIndex] = 0x13;

		//�Ӷ����ж�ȡ�ӽڵ��ر���
		int Ret = GetSlaveMonitorTimeFromList(nTtlIndex, &AmrSlaveNodeMonitorTime[nTtlIndex], &InfoDataHead[nTtlIndex]);
		if(Ret > 0)
		{
			memcpy(&InfoMsgData[nTtlIndex], &InfoDataHead[nTtlIndex], sizeof(INFO_DATA_HEAD_T));	//��ȡԭʼHEAD
			InfoMsgData[nTtlIndex].MsgRPM = 0;   //������־λ, �ظ���������
			gTotalState[nTtlIndex].SlaveMonitorTime = INNER_RTS_SEND;  //������֡����
		}
		else
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay() ��ȡ�ӽڵ�����ʱ δ�鵽����, ֱ�ӽ���! �˿�:%d \n", nTtlIndex);
			gTotalState[nTtlIndex].SlaveMonitorTime = INNER_RTS_END;  //����
			break;
		}			
        
		break;

	case INNER_RTS_SEND: //����
		CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay()��ȡ�ӽڵ�����ʱ INNER_RTS_SEND...�˿�[%d], SendType=0x%02x \n", nTtlIndex, SendType[nTtlIndex]);

		if(SendType[nTtlIndex] == 0x13)
		{
			SendAppData.AFN = 0x13;
			SendAppData.Fn	= 1;
			memcpy(SendAppData.DesAddr, AmrSlaveNodeMonitorTime[nTtlIndex].aSlaveNodeAddr, 6);  //Ŀ�ĵ�ַ
			SendAppData.DataUnitBuff[0] = AmrSlaveNodeMonitorTime[nTtlIndex].nProtocolType;  //ͨ��Э������
			SendAppData.DataUnitBuff[1] = 0x01;  //ͨ����ʱ����Ա�־
			SendAppData.DataUnitBuff[2] = 0x00;  //�ӽڵ㸽���ڵ�����
			SendAppData.DataUnitBuff[3] = AmrSlaveNodeMonitorTime[nTtlIndex].nDataBufLen;	//����֡����
			memcpy(&SendAppData.DataUnitBuff[4], AmrSlaveNodeMonitorTime[nTtlIndex].aDataBuf, SendAppData.DataUnitBuff[3]);
			SendAppData.DataUnitLen = 4 + SendAppData.DataUnitBuff[3];
			SendLen = PackFrameData(SendAppData, nTtlIndex);
			CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay()��ȡ�ӽڵ�����ʱ 13F1 ���ͳ���SendLen=%d \n", SendLen);
		}
		else if(SendType[nTtlIndex] == 0x14)
		{
			SendAppData.AFN = 0x14;
			SendAppData.Fn	= 3;
			memcpy(SendAppData.DesAddr, AmrSlaveNodeMonitorTime[nTtlIndex].aSlaveNodeAddr, 6);  //Ŀ�ĵ�ַ
			SendAppData.DataUnitBuff[0] = AmrSlaveNodeMonitorTime[nTtlIndex].nDataBufLen;	//����֡����
			memcpy(&SendAppData.DataUnitBuff[1], &BakBuff14F3[nTtlIndex][0], SendAppData.DataUnitBuff[0]);
			SendAppData.DataUnitLen = 1 + SendAppData.DataUnitBuff[0];
			SendLen = PackFrameData(SendAppData, nTtlIndex);
			CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay()��ȡ�ӽڵ�����ʱ 14F3 ���ͳ���SendLen=%d \n", SendLen);
		}

		
		gTotalState[nTtlIndex].SlaveMonitorTime = INNER_RTS_RECV;
		break;

	case INNER_RTS_RECV: //����
		RecvResult = 1;  //���ս��(1:�ɹ� / -1:ʧ��)
		RecvLen = PlcReadQueue(pAmrRecvDown[nTtlIndex], RecvBuf);
		if (RecvLen > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay()��ȡ�ӽڵ�����ʱ INNER_RTS_RECV...�˿�[%d]...if (RecvLen > 0) \n", nTtlIndex);
			Ret = UnPackFrameData(RecvLen, RecvBuf, &RecvAppData);

			switch(Ret)
			{
			case CHECK_RESULT_FAILUER:
			case CHECK_RESULT_DENY:
			case CHECK_RESULT_CONFIRM:
				CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay()��ȡ�ӽڵ�����ʱ INNER_RTS_RECV...CHECK_RESULT_FAILUER/DENY/CONFIRMʧ��...�˿�[%d] \n", nTtlIndex);
				break;

			case CHECK_RESULT_SUCCEED:
				CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay()��ȡ�ӽڵ�����ʱ INNER_RTS_RECV...CHECK_RESULT_SUCCEED�ɹ�...�˿�[%d] \n", nTtlIndex);
				if(SendType[nTtlIndex] == 0x13)
				{
					CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay() INNER_RTS_RECV SendType=0x%02x \n", SendType[nTtlIndex]);
					if((RecvAppData.AFN == 0x14) && (RecvAppData.Fn == 3))
					{												
						memcpy(&DelaySecNum, &RecvAppData.DataUnitBuff[6], 2);  //�ӽڵ��ַ6�ֽڣ�Ԥ���ӳ�ʱ��2�ֽ�
						memcpy(&BakBuff14F3[nTtlIndex][0], &RecvAppData.DataUnitBuff[9], RecvAppData.DataUnitBuff[8]);  //����1�ֽ�
						CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay() if((RecvAppData.AFN == 0x14) && (RecvAppData.Fn == 3)) �����ʱ:%d \n", DelaySecNum);

						gTotalState[nTtlIndex].SlaveMonitorTime = INNER_RTS_SEND;//�������ͣ���14F3��Ӧ
						SendType[nTtlIndex] = 0x14;
					}
				}
				else if(SendType[nTtlIndex] == 0x14)
				{
					CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay() INNER_RTS_RECV SendType=0x%02x \n", SendType[nTtlIndex]);
					if((RecvAppData.AFN == 0x13) && (RecvAppData.Fn == 1))
					{
						gTotalState[nTtlIndex].SlaveMonitorTime = INNER_RTS_END;//�������
						
						memcpy(&DelaySecNum, &RecvAppData.DataUnitBuff[0], 2);  //��ǰ���ı���ͨ������ʱ��2�ֽ�
						CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay() if((RecvAppData.AFN == 0x13) && (RecvAppData.Fn == 1)) �����ʱ:%d \n", DelaySecNum);

						//��װ��Ϣ����
						InfoMsgData[nTtlIndex].IID_ID = 0x0003;
						InfoMsgData[nTtlIndex].IOP_ID = 0x0037;
						Uint16ToCharArr(DelaySecNum, InfoMsgData[nTtlIndex].aPayloadData);
						IndexID += 2;
						InfoMsgData[nTtlIndex].nPayloadLength = IndexID;

						PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData[nTtlIndex], sizeof(MSG_INFO_T)+InfoMsgData[nTtlIndex].nPayloadLength);

						break;
					}
				}				
				
			}
		}
		
		//�ж��Ƿ��Ѿ���ʱ
		{
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				RecvResult = -1;
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) ��ʱ...\n");
			}
		}

		if (RecvResult == -1)
		{
			CommRetryCnt[nTtlIndex]++; //���Դ���+1
			gTotalState[nTtlIndex].SlaveMonitorTime = INNER_RTS_RETRY; //�л�������״̬	
		}
		break;

	case INNER_RTS_RETRY:
		CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay()��ȡ�ӽڵ�����ʱ INNER_RTS_RETRY...�˿�[%d] \n", nTtlIndex);
		CCODOWN_FMT_DEBUG(nTtlIndex, "�������³��Խ׶� ��ǰ���Դ���:%d, ����������Դ���MaxRetrtCnt:%d \n", CommRetryCnt[nTtlIndex], MaxRetrtCnt[nTtlIndex]);
		if(CommRetryCnt[nTtlIndex] > MaxRetrtCnt[nTtlIndex])
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay() δ��ѯ���ӽڵ�����ʱ! \n");
			gTotalState[nTtlIndex].SlaveMonitorTime = INNER_RTS_END; //�ﵽ������Դ���, �������

			//��װ��Ϣ����
			InfoMsgData[nTtlIndex].IID_ID = 0x0003;
			InfoMsgData[nTtlIndex].IOP_ID = 0x0037;
			InfoMsgData[nTtlIndex].nPayloadLength = 0;
			memset(InfoMsgData[nTtlIndex].aPayloadData, 0x00, InfoMsgData[nTtlIndex].nPayloadLength);  //payload
			
			PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData[nTtlIndex], sizeof(MSG_INFO_T)+InfoMsgData[nTtlIndex].nPayloadLength);
		}
		else
		{
			gTotalState[nTtlIndex].SlaveMonitorTime = INNER_RTS_SEND; //�л������͵�״̬
		}

		break;

	case INNER_RTS_END:
		CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay()��ȡ�ӽڵ�����ʱ INNER_RTS_END...�˿�[%d] \n", nTtlIndex);
		CommRetryCnt[nTtlIndex] = 0;
        gTotalState[nTtlIndex].SlaveMonitorTime = INNER_RTS_IDLE;
		return 1;
		break;

	default:
		break;
	}

	return -1;
}

/*******************************************************************************
* ��������: GetBroadcastDelay
* ��������: ��ȡ�㲥��ʱ
* �������: ��
* �������: ��
* �� �� ֵ: 
*******************************************************************************/
int GetBroadcastDelay(TASK_FUN_INPUT stTaskFunInput)
{
	uint16  DelaySecNum = 0;  //�㲥��ʱʱ��(��)
	AppData_T       SendAppData,RecvAppData;
	uint16  IndexID = 0;  //ƫ��λ
	
	uint8           nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	AMR_GET_BROADCAST_DELAY_T stAmrGetBroadcastDelay;  //��ȡ�㲥��ʱ
	memcpy(&stAmrGetBroadcastDelay, stTaskFunInput.InfoMsgBuff, sizeof(AMR_GET_BROADCAST_DELAY_T));

	INFO_DATA_T     InfoMsgData;  //�ظ���Ϣ
	memset(&InfoMsgData, 0, sizeof(InfoMsgData));
	memcpy(&InfoMsgData, &stTaskFunInput.InfoDataHead, sizeof(INFO_DATA_HEAD_T));  //��ȡԭʼHEAD
	InfoMsgData.MsgRPM = 0;   //������־λ, �ظ���������

	ReadAmrInfo(nTtlIndex);  //���ص�����Ϣ��������Ϣ

	SendAppData.AFN = 0x03;
    SendAppData.Fn  = 9;
	SendAppData.DataUnitBuff[0] = stAmrGetBroadcastDelay.nProtocolType;  //ͨ��Э������
	SendAppData.DataUnitBuff[1] = (uint8)stAmrGetBroadcastDelay.nDataBufLen;  //���ĳ���(1�ֽ�)
	memcpy(&SendAppData.DataUnitBuff[2], stAmrGetBroadcastDelay.aDataBuf, SendAppData.DataUnitBuff[1]);
	SendAppData.DataUnitLen = 2 + SendAppData.DataUnitBuff[1];
	
	if(CommFunDeal(SendAppData, &RecvAppData, nTtlIndex) > 0)
    {
        memcpy(&DelaySecNum,RecvAppData.DataUnitBuff,2);
        CCODOWN_FMT_DEBUG(nTtlIndex, "GetBroadcastDelay() ��ѯ���㲥��ʱ:%d \n", DelaySecNum);

		//��װ��Ϣ����
		InfoMsgData.IID_ID = 0x0003;
		InfoMsgData.IOP_ID = 0x0039;
		Uint16ToCharArr(DelaySecNum, InfoMsgData.aPayloadData);
		IndexID += 2;
		InfoMsgData.nPayloadLength = IndexID;
		
		PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
		return 1;
		
    }
	else
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "GetBroadcastDelay() δ��ѯ���㲥��ʱ! \n");
		
		//��װ��Ϣ����
		InfoMsgData.IID_ID = 0x0003;
		InfoMsgData.IOP_ID = 0x0039;
		InfoMsgData.nPayloadLength = 0;
		
		PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
		return 1;
	}

}

/*******************************************************************************
* ��������:	CmdOopBroadcast
* ��������:	��������㲥Уʱ
* �������:	MeterData           ����Լ��ʽ����
* �������:	��
* �� �� ֵ: ��
*******************************************************************************/
void CmdOopBroadcast(MeterPrtlFmt_T MeterData, uint8 nArrIndex)
{
    AppData_T AppData;
    uint16    tmpCrc;

    AppData.AFN = 0x05;
    AppData.Fn = 3;
    AppData.DataUnitLen = MeterData.DataLen+2;
    AppData.DataUnitBuff[0] = MeterData.PrtlMode;
    AppData.DataUnitBuff[1] = MeterData.DataLen;
    //���¼���У��
    tmpCrc = CRC16_Get((uint8*)&MeterData.DataBuff[1] ,MeterData.DataLen-4);
    MeterData.DataBuff[MeterData.DataLen-3] =  tmpCrc & 0x00FF;
    MeterData.DataBuff[MeterData.DataLen-2] =  (tmpCrc & 0xFF00)>>8;
    memcpy(&AppData.DataUnitBuff[2], MeterData.DataBuff, MeterData.DataLen);

    PackFrameData(AppData, nArrIndex);
}


/*******************************************************************************
* ��������: BroadcastDeal
* ��������: �㲥
* �������: ��
* �������: ��
* �� �� ֵ: 
*******************************************************************************/
int BroadcastDeal(TASK_FUN_INPUT stTaskFunInput)
{
	uint8	nProtocolType;	//��Լ����
	MeterPrtlFmt_T MeterPrtlData;
	int Ret = -1;
	uint16  offset;  //����ͷ��ƫ��λ

	uint8           nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	AMR_DO_BROADCAST_T stAmrDoBroadcast;  //�㲥
	memcpy(&stAmrDoBroadcast, stTaskFunInput.InfoMsgBuff, sizeof(AMR_DO_BROADCAST_T));

	nProtocolType = stAmrDoBroadcast.nProtocolType;  //ʲô�ط�ʹ�����Э��?

	MeterPrtlData.MtrType = METER_PLC;
    MeterPrtlData.CtrlCode = 0x08;
    MeterPrtlData.DataLen = 6;
    MeterPrtlData.PrtlMode = 0x00;
    memset(MeterPrtlData.MtrAddr,0x99,6);

	ReadAmrInfo(nTtlIndex);  //���ص�����Ϣ��������Ϣ

	//���ΰ���645���ĺ�698���Ľ���
	Ret = CheckMeterProtocol(stAmrDoBroadcast.nDataBufLen, stAmrDoBroadcast.aDataBuf, &MeterPrtlData, &offset);
	if(Ret == METER_PROTOCOL_645)
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()����Լ��֡ ����Э��Ϊ645...\n");
		MeterPrtlData.PrtlMode = nProtocolType;
        CmdDataRelay(MeterPrtlData, stAmrDoBroadcast.aDataBuf, stAmrDoBroadcast.nDataBufLen, nTtlIndex);//������֡
	}
	else if(Ret == METER_PROTOCOL_69845)
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()����Լ��֡ ����Э��Ϊ698.45...\n");
		//�������㲥Уʱ֡����ԭʼ���ķ���
        if(Ret > 0 && MeterPrtlData.MtrAddr[0] == 0xAA && stAmrDoBroadcast.aDataBuf[offset+4] == 0xC0)
        {
            MeterPrtlData.DataLen = stAmrDoBroadcast.nDataBufLen;
            memcpy(MeterPrtlData.DataBuff, stAmrDoBroadcast.aDataBuf, MeterPrtlData.DataLen);

			MeterPrtlData.PrtlMode = nProtocolType;
            CmdOopBroadcast(MeterPrtlData, nTtlIndex);//�����������֡
        }
	}
	else
	{
		//���Ľ�֡ʧ��, ֱ�ӽ���
	}

	return 1;

}


/*******************************************************************************
* ��������: WirelessNetStudyDeal
* ��������: ΢��������������������
* �������: ��
* �������: ��
* �� �� ֵ: 
*******************************************************************************/
int WirelessNetStudyDeal(TASK_FUN_INPUT stTaskFunInput)
{
	return 1;

}

/*******************************************************************************
* ��������: SwitchChannelDeal
* ��������: �л��ŵ���������
* �������: ��
* �������: ��
* �� �� ֵ: 
*******************************************************************************/
int SwitchChannelDeal(TASK_FUN_INPUT stTaskFunInput)
{
	return 1;
}

/*******************************************************************************
* ��������: RouterStudyDeal
* ��������: ·��ѧϰ����
* �������: ��
* �������: ��
* �� �� ֵ: >0                  ���
*           <0                  δ���
*******************************************************************************/
int RouterStudyDeal(TASK_FUN_INPUT stTaskFunInput)
{
	return 1;
}

/*******************************************************************************
* ��������:	ClearCommRecvBuff
* ��������:	���ͨѶ��������
* �������:	��
* �������:	��
* �� �� ֵ:	��
*******************************************************************************/
void ClearCommRecvBuff(uint8 nArrIndex)
{
    int     RecvLen = 0xff;                     //�������ݳ���
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN];        //�������ݻ�����

	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = nArrIndex;

    while (RecvLen > 0)
    {
        RecvLen = PlcReadQueue(pAmrRecvDown[nTtlIndex], RecvBuf);
    }
}

/*******************************************************************************
* ��������:	ClearCommQueueBuff
* ��������:	���ͨѶ��������
* �������:	��
* �������:	��
* �� �� ֵ:	��
*******************************************************************************/
void ClearCommQueueBuff(LockQueue_T *pQueueBuff)
{
    int     RecvLen = 0xff;                     //�������ݳ���
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN] = {0};        //�������ݻ�����

    while (RecvLen > 0)
    {
        RecvLen = PlcReadQueue(pQueueBuff, RecvBuf);
    }
}


/*******************************************************************************
* ��������:			RecvFrameCheck
* ��������:			���з���������������յ������ݽ����ж�
* �������:			AppData_T AppData        //���з��͵���������
                AppData_T RecvAppData    //���з��͵���������
* �������:			NULL
* �� �� ֵ:	        > 0 ���յ���ȷ����
                    < 0 �������ݴ���

*******************************************************************************/
int RecvFrameCheck(AppData_T AppData,AppData_T RecvAppData, uint8 nArrIndex )
{
    static int DenyContinuationTimes[MAX_DOWN_THREAD_NUM] = {0};

	PLCTASK_DETAIL  stPlcTaskDetail;       //�ز����������Ϣ
	memset((uint8 *)&stPlcTaskDetail, 0x00, sizeof(stPlcTaskDetail));

	uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = nArrIndex;

    //һ�����ֲ��������ķ���֡�����÷�����������
    if(!(RecvAppData.AFN == 0x0 && RecvAppData.Fn == 2))
    {
        DenyContinuationTimes[nTtlIndex] = 0;
    }

    switch(AppData.AFN)//
    {
    case 0x0://ȷ�Ϸ���
       return 1;

    //ע��:0x01,0x04,0x05,0x11,0x12��1376.2��ֻ����ȷ�Ϸ���֡
    //<prohibition_modify>...</prohibition_modify>������ǩ֮������ݲ����޸�
    case 0x01://��ʼ��
    case 0x04://��·�ӿڼ��
    case 0x05://��������
    case 0x11://·������
        if(RecvAppData.AFN == 0x0 && RecvAppData.Fn == 1)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "�յ�ȷ��֡!  \n");
            return 1;
        }
		
        if(RecvAppData.AFN == 0x0 && RecvAppData.Fn == 2)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "�յ�����֡!  \n");
            DenyContinuationTimes[nTtlIndex]++;

            if(DenyContinuationTimes[nTtlIndex] > 5)
            {
                DenyContinuationTimes[nTtlIndex] = 0;
                CCODOWN_FMT_DEBUG(nTtlIndex, "�����յ�����֡, ����5��, ���������߳�! \n");
                gRebootAmrThread[nTtlIndex] = 1; //�����߳�����
	            sleep(3);             //�ȴ��߳��������̵�����
            }

            return 1;
        }
        break;

    case 0x12://·�ɿ���
        if(RecvAppData.AFN == 0x0 && RecvAppData.Fn == 1)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "·�ɿ�������(1:���� / 2:��ͣ / 3:�ָ�)ȷ��:%d \n", AppData.Fn);
            return 1;
        }

        if(RecvAppData.AFN == 0x0 && RecvAppData.Fn == 2)
        {
            //������ģ���������
            CCODOWN_FMT_DEBUG(nTtlIndex, "·�ɿ�������(1:���� / 2:��ͣ / 3:�ָ�)����:%d \n", AppData.Fn);
            return 1;
        }
        return 1;
        break;

    case 0x02://����ת��
    case 0x03://��ѯ����
    case 0x10://·�ɲ�ѯ
    case 0x13://·������ת��
    case 0x15://�ļ�����
        //�������AFN=0x03,F10,���������ѯ��־(ͳһ��GetModuleManuID�����м��)
        /*
        if(RecvAppData.AFN == 0x03 && RecvAppData.Fn == 10)
            CheckModuleSoftReset(0, nTtlIndex);
        */

        if(RecvAppData.AFN == AppData.AFN
                && RecvAppData.Fn == AppData.Fn)
            return 1;
        break;

    case 0x06://�����ϱ�
        return 1;

    case 0x14://·�����ݳ���
        if(RecvAppData.AFN == AppData.AFN)
            return 1;
        break;

    default:
        return 1;
    }

    return -1;

}


/*******************************************************************************
* ��������:			CommFunDeal
* ��������:			�����շ�����
* �������:			NULL
* �������:			NULL
* �� �� ֵ:	        > 0 ���ճɹ�
                    < 0 ����ʧ��

*******************************************************************************/
int CommFunDeal(AppData_T SendAppData,AppData_T *RecvAppData, uint8 nArrIndex )
{
	//printx("int CommFunDeal(AppData_T SendAppData,AppData_T *RecvAppData, uint32 nPortID)...begin \n");
	
    uint8 	             		RetryCnt = 0;                       //���ճ��Դ���
    int                  		RecvLen;                            //�������ݳ���
    uint8               		RecvBuf[MAX_QUEUE_BUFF_LEN];        //�������ݻ�����
    static time_t        		CommBeginTime[MAX_DOWN_THREAD_NUM];                      //ͨѶ��ʼʱ��
    uint16                      SendLen = 0;
    ACTIVE_COMM_STATE_T         CommStat = ACT_COMM_IDLE;
    int                         RecvResult = 1;                     //���ս��(1:�ɹ� / -1:ʧ��)

	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = nArrIndex;

    while(1)
    {
        switch(CommStat)
        {
        case ACT_COMM_IDLE:
			//printx("�����շ�����[%d]CommFunDeal()...ACT_COMM_IDLE \n", nTtlIndex);
            ClearCommRecvBuff(nTtlIndex);                    //���������
            CommBeginTime[nTtlIndex] = time(NULL);
            CommStat = ACT_COMM_SEND;               //�л�������
            break;

        case ACT_COMM_SEND:
			//printx("�����շ�����[%d]CommFunDeal()...ACT_COMM_SEND \n", nTtlIndex);
            SendLen = PackFrameData(SendAppData, nTtlIndex);
            if(SendLen > 0)
            {
               //����AFN=0x03,F10,��������ѯ��־(ͳһ��GetModuleManuID�����м��)
               /*
               if(SendAppData.AFN == 0x03 && SendAppData.Fn == 10)
                   CheckModuleSoftReset(1, nTtlIndex);
               */

                CommBeginTime[nTtlIndex] = time(NULL);
                CommStat = ACT_COMM_RECV;           //�л�������
            }
            break;

        case ACT_COMM_RECV:
			//printx("�����շ�����[%d]CommFunDeal()...ACT_COMM_RECV \n", nTtlIndex);
            RecvLen = PlcReadQueue(pAmrRecvDown[nTtlIndex], RecvBuf);
            if(RecvLen > 0)
            {
                UnPackFrameData(RecvLen, RecvBuf,RecvAppData);
                if(RecvFrameCheck(SendAppData,*RecvAppData, nTtlIndex) > 0)
                    return 1;//���ճɹ�
                else
                    RecvResult = -1;//����ʧ��

            }
            else
                RecvResult = -1;//����ʧ��

            if(RecvResult == -1)
            {
                if( abs(time(NULL) - CommBeginTime[nTtlIndex]) > MAX_COMM_DELAY_TIME)
                {
                    RetryCnt++;
                    CommStat = ACT_COMM_RETRY;
                }
            }

            break;

        case ACT_COMM_RETRY:
			//printx("�����շ�����[%d]CommFunDeal()...ACT_COMM_RETRY \n", nTtlIndex);
            if(RetryCnt > 3)
            {
                CommStat = ACT_COMM_IDLE;
                return 2;
            }

            CommStat = ACT_COMM_SEND;
            break;

        default:
            break;

        }

        usleep(20000);
    }

    return 1;
}


/*******************************************************************************
* ��������:			CommFunDeal
* ��������:			�����շ�����
* �������:			NULL
* �������:			NULL
* �� �� ֵ:	        > 0 ���ճɹ�
                    < 0 ����ʧ��

*******************************************************************************/
int CommFunDealByType(AppData_T SendAppData, AppData_T *RecvAppData, uint8 nArrIndex, int32 nFunType)
{
	//printx("int CommFunDeal(AppData_T SendAppData,AppData_T *RecvAppData, uint32 nPortID)...begin \n");
	
    uint8 	             		RetryCnt = 0;                       //���ճ��Դ���
    int                  		RecvLen;                            //�������ݳ���
    uint8               		RecvBuf[MAX_QUEUE_BUFF_LEN];        //�������ݻ�����
    static time_t        		CommBeginTime[MAX_DOWN_THREAD_NUM];                      //ͨѶ��ʼʱ��
    uint16                      SendLen = 0;
    ACTIVE_COMM_STATE_T         CommStat = ACT_COMM_IDLE;
    int                         RecvResult = 1;                     //���ս��(1:�ɹ� / -1:ʧ��)

	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = nArrIndex;

    while(1)
    {
        switch(CommStat)
        {
        case ACT_COMM_IDLE:
			//printx("�����շ�����[%d]CommFunDeal()...ACT_COMM_IDLE \n", nTtlIndex);
			if(MS_ADDR_SEARCH == nFunType)//�ѱ�
			{
				ClearCommQueueBuff(&gAmrRecvDownSearchMeter[nTtlIndex]);                    //���������
			}
			else
			{
				ClearCommRecvBuff(nTtlIndex);                    //���������
			}
            
            CommBeginTime[nTtlIndex] = time(NULL);
            CommStat = ACT_COMM_SEND;               //�л�������
            break;

        case ACT_COMM_SEND:
			//printx("�����շ�����[%d]CommFunDeal()...ACT_COMM_SEND \n", nTtlIndex);
            SendLen = PackFrameData(SendAppData, nTtlIndex);
            if(SendLen > 0)
            {
				//����AFN=0x03,F10,��������ѯ��־(ͳһ��GetModuleManuID�����м��)
				/*
				if(SendAppData.AFN == 0x03 && SendAppData.Fn == 10)
				{
					CheckModuleSoftReset(1, nTtlIndex);
				}
				*/

                CommBeginTime[nTtlIndex] = time(NULL);
                CommStat = ACT_COMM_RECV;           //�л�������
            }
            break;

        case ACT_COMM_RECV:
			//printx("�����շ�����[%d]CommFunDeal()...ACT_COMM_RECV \n", nTtlIndex);
			if(MS_ADDR_SEARCH == nFunType)//�ѱ�
			{
				RecvLen = PlcReadQueue(&gAmrRecvDownSearchMeter[nTtlIndex], RecvBuf);
			}
			else
			{
				RecvLen = PlcReadQueue(pAmrRecvDown[nTtlIndex], RecvBuf);
			}
			
            if(RecvLen > 0)
            {
                UnPackFrameData(RecvLen, RecvBuf, RecvAppData);
                if(RecvFrameCheck(SendAppData,*RecvAppData, nTtlIndex) > 0)
                    return 1;//���ճɹ�
                else
                    RecvResult = -1;//����ʧ��

            }
            else
                RecvResult = -1;//����ʧ��

            if(RecvResult == -1)
            {
                if( abs(time(NULL) - CommBeginTime[nTtlIndex]) > MAX_COMM_DELAY_TIME)
                {
                    RetryCnt++;
                    CommStat = ACT_COMM_RETRY;
                }
            }

            break;

        case ACT_COMM_RETRY:
			//printx("�����շ�����[%d]CommFunDeal()...ACT_COMM_RETRY \n", nTtlIndex);
            if(RetryCnt > 3)
            {
                CommStat = ACT_COMM_IDLE;
                return 2;
            }

            CommStat = ACT_COMM_SEND;
            break;

        default:
            break;

        }

        usleep(20000);
    }

    return 1;
}



/*******************************************************************************
* ��������: AppRouterDealFun
* ��������: �ⲿ·�ɲ���
* �������: ��
* �������: ��
* �ڲ�����:
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int AppRouterDealFun(TASK_FUN_INPUT stTaskFunInput)
{
	AppData_T	SendAppData;						//����
	AppData_T	RecvAppData;						//����
	memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));

	uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	SendAppData.AFN = 0x12;
	SendAppData.Fn = stTaskFunInput.RoutrtCtrl;
	SendAppData.DataUnitLen = 0;

	if(CommFunDeal(SendAppData, &RecvAppData, nTtlIndex))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}



/*******************************************************************************
* ��������: OftenAmrDeal
* ��������: ����Ƶ��ִ�е�����(ִ������С��һ��)
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int OftenAmrDeal(TASK_FUN_INPUT stTaskFunInput)
{
	return 1;
}

/*******************************************************************************
* ��������: PackFrameDataX
* ��������: ��3762֡,������ݲ����͵�����
* �������: AppData
* �������: Buff        3762֡
* �� �� ֵ: ����
*******************************************************************************/
uint16 PackFrameDataX(AppData_T AppData, uint8* Buff, uint8 nArrIndex)
{
	//printx("PackFrameDataX()��3762֡ ������ݲ����͵����� �˿�[%d]...begin \n", nArrIndex);
	
    uint8  	    FrameBuff[MAX_QUEUE_BUFF_LEN];      //֡���ݻ�����
    uint16      DataIndex = 0;
    int         i = 0;
    uint8       CheckSum = 0;                       //У���

    FrameBuff[0] = 0x68;

    if (gPrtlLinkLen == 1)
    {
        //����֡����
        DataIndex = 2;
    }
    else
    {
        //����֡����
        DataIndex = 3;
    }

    //������
    GetCtrlCode(&FrameBuff[DataIndex], AppData, nArrIndex);
    DataIndex++;

    //��Ϣ��
    GetInfoZoneData(&FrameBuff[DataIndex], AppData, nArrIndex);

    DataIndex += 6;
    //��ַ��
    if (GetAddrZoneData(&FrameBuff[DataIndex], AppData, nArrIndex) > 0)
    {
        DataIndex += 12;
    }

    //Ӧ��������
    FrameBuff[DataIndex] = AppData.AFN;
    GetDtInfo(&FrameBuff[DataIndex+1], AppData.Fn);
    DataIndex += 3;

    if (AppData.DataUnitLen > 0)
    {
        memcpy(&FrameBuff[DataIndex], AppData.DataUnitBuff, AppData.DataUnitLen);
        DataIndex += AppData.DataUnitLen;
    }

    //����͵ķ�ΧΪ������+�û�������
    for (i = (gPrtlLinkLen+1); i < DataIndex; i++)
    {
        CheckSum += FrameBuff[i];
    }
    FrameBuff[DataIndex] = CheckSum;
    DataIndex++;

    //֡β
    FrameBuff[DataIndex] = 0x16;
    DataIndex++;

    if (gPrtlLinkLen == 1)
    {
        //�����L
        FrameBuff[1] = (uint8)DataIndex;
    }
    else
    {
        //�����L
        *(uint16*)&FrameBuff[1] = DataIndex;
    }

    memcpy((void *)Buff, (void *)FrameBuff, DataIndex);
	//printx("PackFrameDataX()��3762֡ �˿�[%d] ����֮��ı���֡Ϊ...end \n", nArrIndex);
	//PrintBuf((uint8 *)Buff, DataIndex);
    return DataIndex;
}


/*******************************************************************************
* ��������: CmdMeterPrtlMakeFrame
* ��������: ��F1-01����֡
* �������: 
* �������: 
* �� �� ֵ: 
*******************************************************************************/
uint16 CmdMeterPrtlMakeFrame(AppData_T AppData, uint8 *Buff, uint8 nArrIndex)
{
	//printx("CmdMeterPrtlMakeFrame()��F1-01����֡ �˿ں�[%d]...begin \n", nPortID);

    AppData.AFN = 0xF1;
    AppData.Fn  = 1;
    AppData.ModuleID = 1;

    return PackFrameDataX(AppData, Buff, nArrIndex);
}

/*******************************************************************************
* ��������: CcoPackAFN03F4
* ��������: ��AFN03F4����֡
* �������: 
* �������: 
* �� �� ֵ: 
*******************************************************************************/
uint16 CcoPackAFN03F4(AppData_T AppData, uint8 *Buff, uint8 nArrIndex)
{
    AppData.AFN = 0x03;
    AppData.Fn  = 4;
    AppData.DataUnitLen = 0;
    return PackFrameDataX(AppData, Buff, nArrIndex);
}

/*******************************************************************************
* ��������: ParalAmrMakeFrame
* ��������: ���г�����֡
* �������: ��
* �������: ��
* �� �� ֵ: 1                  ȫ�����
*           0               ֡δ������ɣ��л�������
*******************************************************************************/
int ParalAmrMakeFrame(uint8 nArrIndex)
{
	//printx("int ParalAmrMakeFrame(uint32 nPortID)�˿ں�[%d]���г�����֡...begin \n", nPortID);
	
    int          i;
    uint16       FrameLen = 0;
    uint8        FrameBuff[MAX_QUEUE_BUFF_LEN];  //֡���ݻ�����
    AppData_T    AppData;
    //int          nActiveAmrInfoNum = 0;
	uint8	     nProtocolType;	//Э������
	uint16       IndexID = 0;  //ƫ��λ
	uint16       nDataBufLen = 0;  //��Ϣ���ĵĳ���ԭʼ���ĳ���
	AMR_MODE_ACTIVE_T AmrModeActive;  //������Ϣ
	int          nCmpResult = 0;
	int          ret = 0;

	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = nArrIndex;
	
    for(i = 0; i < MAX_NORMAL_AMR_INFO_CNT; i++)
    {
    	//�ӳ�����Ϣ��������ѯ����
		ret = QueryAmrFromList(&AmrModeActive, nTtlIndex);
		if(ret < 0)
		{
			break;  //û�ж���������
		}

		//���ҵ���Ҫ���͵Ŀ��ж���
        if(gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].ValidFlag == 1)
        {
        	nCmpResult = memcmp(gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr, AmrModeActive.nMeterAddr, 6);
			if(nCmpResult == 0)
			{
				//��־λ��Ч�ĵ�������յ������ģ������Ѿ���ʱ�����³���
				CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d] FrameSeqInfo[%d].ValidFlag==1, �������FrameSeqInfo[%d].MeterAddr:%02x %02x %02x %02x %02x %02x, �����ĵ���ַ:%02x %02x %02x %02x %02x %02x, �յ��µĳ����ģ����³���. \n", 
					nTtlIndex, i, i, gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[0], gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[1], gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[2], 
					gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[3], gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[4], gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[5], 
					AmrModeActive.nMeterAddr[0], AmrModeActive.nMeterAddr[1],AmrModeActive.nMeterAddr[2],AmrModeActive.nMeterAddr[3],
					AmrModeActive.nMeterAddr[4],AmrModeActive.nMeterAddr[5]);
			}
			else
			{
				//��־λ��Ч�ĵ��û�������յ������ģ������ȴ�������seq����һ��λ��
				/*
				CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d] FrameSeqInfo[%d].ValidFlag, �������FrameSeqInfo[%d].MeterAddr:%02x %02x %02x %02x %02x %02x, �����ĵ���ַ:%02x %02x %02x %02x %02x %02x, û���յ��µĳ����ģ�����RTS_RECV����������. \n", 
					nTtlIndex, i, i, gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[0], gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[1], gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[2], 
					gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[3], gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[4], gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[5], 
					AmrModeActive.nMeterAddr[0], AmrModeActive.nMeterAddr[1],AmrModeActive.nMeterAddr[2],AmrModeActive.nMeterAddr[3],
					AmrModeActive.nMeterAddr[4],AmrModeActive.nMeterAddr[5]);
				*/
				
				continue;
			}
		}
		DeleteAmrFromList(nTtlIndex);//ɾ������һ����Ϣ
		
		//printx("ParalAmrMakeFrame()���г�����֡ ѭ��i=[%d]ʱ��gActiveAmrInfoList[%d]ȡ���ĳ�����ϢΪ: \n", i, nTtlIndex);
		//PrintAmrActiveDetail(AmrModeActive);
		//printx("ParalAmrMakeFrame()���г�����֡ ѭ��i=[%d]ʱȡ��һ��������Ϣ֮��ʣ��[%d]����Ϣ \n", i, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);

		memcpy(AppData.DesAddr, AmrModeActive.nMeterAddr, 6);
		nProtocolType = GetProtocolByMeterAddr(AppData.DesAddr, nTtlIndex);

		IndexID = 0;
		AppData.DataUnitBuff[IndexID] = nProtocolType;  //��Լ���ͣ�1�ֽ�
		IndexID++;
		AppData.DataUnitBuff[IndexID] = 0x00;  //������1�ֽ�
		IndexID++;
		nDataBufLen = AmrModeActive.nDataBufLen;
  		Uint16ToCharArr_r(nDataBufLen, &AppData.DataUnitBuff[IndexID]);  //���ĳ��ȣ�2�ֽ�
		IndexID += 2;
		memcpy(&AppData.DataUnitBuff[IndexID], AmrModeActive.aDataBuf, nDataBufLen);
		IndexID += nDataBufLen;
		AppData.DataUnitLen = IndexID;

		//���浱ǰ֡���
		gGlobalSeqInfo[nTtlIndex].CurSeqCnt ++;
		gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].CommBeginTime = time(NULL);
		gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].ValidFlag = 1;
		gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].FrameSeq = g_InerSeq[nTtlIndex];
		gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].PrtlMode = nProtocolType;
		memcpy(gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr, AppData.DesAddr, 6);
		
		FrameLen += CmdMeterPrtlMakeFrame(AppData, &FrameBuff[FrameLen], nTtlIndex);
		//printx("ParalAmrMakeFrame()���г�����֡ ѭ��i=[%d]ʱFrameLen=[%d]...\n", i, FrameLen);
    	
	}
	
    if(FrameLen)
    {
    	PlcWriteQueue(pAmrSendDown[nTtlIndex], FrameBuff, FrameLen);
		
		CCODOWN_FMT_DEBUG(nTtlIndex, "ParalAmrMakeFrame()���г�����֡ FrameLen=[%d] 3762��������Ϊ: \n", FrameLen);
		CCODOWN_BUF_DEBUG(nTtlIndex, (uint8 *)FrameBuff, FrameLen, "����");
		return 1;
    }

    return 0;
}

/*******************************************************************************
* ��������: IsValidSeq
* ��������: �ж�����֡�Ƿ���Ч֡
* �������: InSeq     ���жϵ�֡���
* �������: OutSeq    �����Ӧ��֡����
* �� �� ֵ: 1         ��Ч
*           0      ��Ч
*******************************************************************************/
uint8 IsValidSeq(uint8* OutSeq, uint8 InSeq, uint8 nArrIndex)
{
	//printx("uint8 IsValidSeq()�ж�����֡�Ƿ���Ч֡ InSeq:%d...begin \n", InSeq);
    int i = 0;
    for(i = 0; i < MAX_NORMAL_AMR_INFO_CNT; i++)
    {
        CCODOWN_FMT_DEBUG(nArrIndex, "IsValidSeq() gGlobalSeqInfo.FrameSeqInfo[%d].FrameSeq=%02x, ValidFlag=%d, *OutSeq=%d \n", 
			i, gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].FrameSeq, gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].ValidFlag, *OutSeq);

		//����֡�����������Ч֮֡��
        if((gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].FrameSeq == InSeq)
                && (gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].ValidFlag == 1))
        {      	
            *OutSeq = i;
			CCODOWN_FMT_DEBUG(nArrIndex, "IsValidSeq() ƥ��! gGlobalSeqInfo.FrameSeqInfo[%d].FrameSeq=%02x, ValidFlag=%d, *OutSeq=%d \n", 
				i, gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].FrameSeq, gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].ValidFlag, *OutSeq);
			return 1;
        }
    }

    return 0;
}

/*******************************************************************************
* ��������: IsActiveAmrFinished
* ��������: �ж����������б��е����������Ƿ����
* �������: 
* �������: 
* �� �� ֵ: 1         ��Ч
*           0      ��Ч
*******************************************************************************/
uint8 IsActiveAmrFinished(uint8 nArrIndex)
{
    int i = 0;
	BOOL bFinished = TRUE;
	int         OverTimeCnt = 63;  //��ʱʱ��
	
    for(i = 0; i < MAX_NORMAL_AMR_INFO_CNT; i++)
    {
    	if(gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].ValidFlag == 1)
        {
        	bFinished = FALSE;
			if( abs(time(NULL) - gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].CommBeginTime) > OverTimeCnt)
            {
				CCODOWN_FMT_DEBUG(nArrIndex, "IsActiveAmrFinished() ��ʱOverTimeCnt=%d! if((time(NULL)-FrameSeqInfo[%d].CommBeginTime)>OverTimeCnt) \n", OverTimeCnt, i);
                RefreshPnState(i, nArrIndex);
				CCODOWN_FMT_DEBUG(nArrIndex, "IsActiveAmrFinished() RefreshPnState()ˢ��֮�� gGlobalSeqInfo.FrameSeqInfo[%d].FrameSeq=%02x, ValidFlag=%d \n", 
					i, gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].FrameSeq, gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].ValidFlag);
            }
        }
    }

    return bFinished;
}

/*******************************************************************************
* ��������: RefreshPnState
* ��������: ˢ��Pn��ʧ�������б�
* �������: 
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void RefreshPnState(uint8 SeqIndex, uint8 nArrIndex)
{
    //uint16 Pn = gGlobalSeqInfo[nArrIndex].FrameSeqInfo[SeqIndex].Pn;

    gGlobalSeqInfo[nArrIndex].FrameSeqInfo[SeqIndex].ValidFlag = 0;
    if(gGlobalSeqInfo[nArrIndex].CurSeqCnt > 0)
        gGlobalSeqInfo[nArrIndex].CurSeqCnt--;

    return;
}


/*******************************************************************************
* ��������: BroadBand_ParallelAmr
* ��������: ����ճ�����
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int BroadBand_ParallelAmr(TASK_FUN_INPUT stTaskFunInput)
{
    
    int             ret;
    uint8           SeqIndex = 0;
    int             RecvLen = 0;                              //�������ݳ���
    uint8           RecvBuf[MAX_QUEUE_BUFF_LEN] = {0};        //�������ݻ�����
    AppData_T       AppData = {0};                            //Ӧ�ò�����
    INFO_DATA_T InfoMsgData;
    uint16          IndexID = 0;  //ƫ��λ

    uint8       nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
    //uint16      nMeterFilesID = 0;  //�������
    uint16      nDataLenF101;  //F1-01����ԭʼ���ĳ���

    memset(&InfoMsgData, 0x00, sizeof(InfoMsgData));
    
    nTtlIndex = stTaskFunInput.TaskArrIndex;

    //printx("int BroadBand_ParallelAmr(TASK_FUN_INPUT stTaskFunInput)����ճ����� �˿�[%d]...begin \n", nTtlIndex);

    switch (gTotalState[nTtlIndex].NormalAmr)
    {
    case RTS_IDLE:                              //����
        //printx("int BroadBand_ParallelAmr(TASK_FUN_INPUT stTaskFunInput)����ճ����� �˿�[%d]...RTS_IDLE \n", nTtlIndex);
        ReadAmrInfo(nTtlIndex);  //���ص�����Ϣ��������Ϣ
        ClearCommRecvBuff(nTtlIndex);                   //������ջ�����
        memset((void *)&gGlobalSeqInfo[nTtlIndex], 0x0, sizeof(GLOBAL_SEQ_INFO_T));         //��֡��Ϣ
        if(gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum > 0)
        {
            gTotalState[nTtlIndex].NormalAmr = RTS_SEND;
        }
        else
        {
            //�����������û�����񣬿���ǰһ�γ����Ѿ�ȫ��ȡ��
            gTotalState[nTtlIndex].NormalAmr = RTS_END;
        }

        //���³���״̬
        gAmrStateQuery[nTtlIndex].nModuleAmrState = 1;

        break;

    case RTS_SEND:                              //����
        //printx("int BroadBand_ParallelAmr(TASK_FUN_INPUT stTaskFunInput)����ճ����� �˿�[%d]...RTS_SEND \n", nTtlIndex);
        if(ParalAmrMakeFrame(nTtlIndex))
        {
            //ȫ���������, ����RTS_RECV
            CCODOWN_FMT_DEBUG(nTtlIndex, "BroadBand_ParallelAmr()����ճ����� �˿�[%d]...RTS_SEND \n", nTtlIndex);
        }
        else
        {
            //��ǰ�����������Ϊ��
        }
        gTotalState[nTtlIndex].NormalAmr = RTS_RECV;
        break;

    case RTS_RECV:                              //����
        RecvLen = PlcReadQueue(pAmrRecvDown[nTtlIndex], RecvBuf);
        if (RecvLen > 0)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "BroadBand_ParallelAmr()����ճ����� �˿�[%d]...RTS_RECV \n", nTtlIndex);
            
            ret = UnPackFrameData(RecvLen, RecvBuf, &AppData);
            
            CCODOWN_FMT_DEBUG(nTtlIndex, "BroadBand_ParallelAmr()����֡��� ret:%d, AppData.RInfo[5] = %02x \n", ret, AppData.RInfo[5]);

            //�жϽ���֡����Ƿ�����ڷ��Ͷ��е�֡��Ϣ��
            if(IsValidSeq(&SeqIndex, AppData.RInfo[5], nTtlIndex) == 0)
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "BroadBand_ParallelAmr() SeqIndex:%d, AppData.RInfo[5]:%02x...����֡������֡, �������� \n", SeqIndex, AppData.RInfo[5]);
                break;
            }

            switch(ret)
            {
            case CHECK_RESULT_DENY:
                //���ڵ�æ��ò��������ڳ���������ȴ�
                if((AppData.DataUnitBuff[0] == 9) || (AppData.DataUnitBuff[0] == 111))
                {
                    break;
                }
                else
                {
                    RefreshPnState(SeqIndex, nTtlIndex);
                    gTotalState[nTtlIndex].NormalAmr = RTS_SEND; //�л�������״̬
                    break;
                }
            case CHECK_RESULT_FAILUER:
            case CHECK_RESULT_CONFIRM:
                RefreshPnState(SeqIndex, nTtlIndex);
                gTotalState[nTtlIndex].NormalAmr = RTS_SEND; //�л�������״̬
                break;

            case CHECK_RESULT_SUCCEED:
                if ((AppData.AFN == 0xF1) && (AppData.Fn == 1))
                {
                    CCODOWN_FMT_DEBUG(nTtlIndex, "BroadBand_ParallelAmr()�˿�[%d]...RTS_RECV...CHECK_RESULT_SUCCEED...if ((AppData.AFN == 0xF1) && (AppData.Fn == 1)) SeqIndex:%d \n", nTtlIndex, SeqIndex);
                    
                    //��װ��Ϣ����
                    InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
                    InfoMsgData.IP  = ntohl(InfoMsgData.IP);
                    InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
                    InfoMsgData.MsgPRIORITY = 0;
                    InfoMsgData.MsgRPM = 1;
                    InfoMsgData.MsgIndex = GetInfoIndex();
                    InfoMsgData.MsgLabel = nTtlIndex;  //mqtt��Ŷ˿����(0��1)
                    InfoMsgData.SourLen = strlen(gCcoRouterAppName);
                    memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
                    memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
                    InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
                    memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
                    memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
    
                    InfoMsgData.IID_ID = 0x0003;
                    InfoMsgData.IOP_ID = 0x0031;
                    IndexID = 0;
                    Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[IndexID]);  //�˿ںţ�4�ֽ�
                    IndexID += 4;
                    //nMeterFilesID = GetFilesIdByMeterAddr(gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[SeqIndex].MeterAddr, nTtlIndex);
                    //Uint16ToCharArr(nMeterFilesID, &InfoMsgData.aPayloadData[IndexID]);  //������ţ�2�ֽ�
                    //IndexID += 2;
                    memcpy_r(&InfoMsgData.aPayloadData[IndexID], gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[SeqIndex].MeterAddr, 6); //�ز��ӽڵ��ַ��6�ֽ�
                    IndexID += 6;
                    InfoMsgData.aPayloadData[IndexID] = gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[SeqIndex].PrtlMode;  //Э�����ͣ�1�ֽ�
                    IndexID += 1;
                    CharArrToUint16_r(&AppData.DataUnitBuff[1], &nDataLenF101);  //����֡���ȣ�F1-01��2�ֽڣ�С�ˣ�ǰ�ͺ�ߣ�
                    ChangeLenToBuff(nDataLenF101, &IndexID, &InfoMsgData.aPayloadData[IndexID]);  //����֡����
                    memcpy(&InfoMsgData.aPayloadData[IndexID], &AppData.DataUnitBuff[3], nDataLenF101);  //����֡
                    IndexID +=  nDataLenF101;
                    InfoMsgData.aPayloadData[IndexID] = 0x01;  //PLC_READING_INFO OPTIONAL �Ƿ��г�����Ϣ��־, 0x00:��, 0x01:��
                    IndexID += 1;
                    InfoMsgData.aPayloadData[IndexID] = (AppData.RInfo[0]>>4) & 0x0f;  //PLC_READING_INFO ��ʼ �м�·�ɼ��� 0-15
                    IndexID += 1;
                    InfoMsgData.aPayloadData[IndexID] = AppData.RInfo[1] & 0x0f;  //PLC_READING_INFO �ŵ���ʶ
                    IndexID += 1;
                    InfoMsgData.aPayloadData[IndexID] = AppData.RInfo[2] & 0x0f;  //PLC_READING_INFO �ز�������λ
                    IndexID += 1;
                    InfoMsgData.aPayloadData[IndexID] = (AppData.RInfo[3]>>4) & 0x0f;  //PLC_READING_INFO �ز��ź�Ʒ��(ĩ��Ӧ���ź�Ʒ��)
                    IndexID += 1;
                    InfoMsgData.aPayloadData[IndexID] = AppData.RInfo[0] & 0x01;  //PLC_READING_INFO �ز���־(·�ɱ�ʶ?)
                    IndexID += 1;
                    InfoMsgData.aPayloadData[IndexID++] = 0x00;  //PLC_READING_INFO ���� ���� 3�ֽ�
                    InfoMsgData.aPayloadData[IndexID++] = 0x00;  //PLC_READING_INFO ���� ���� 3�ֽ�
                    InfoMsgData.aPayloadData[IndexID++] = 0x00;  //PLC_READING_INFO ���� ���� 3�ֽ�
                    
                    InfoMsgData.nPayloadLength = IndexID;
                    
                    PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);

                    gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[SeqIndex].ValidFlag = 0;
                }
                else
                {
                    CCODOWN_FMT_DEBUG(nTtlIndex, "BroadBand_ParallelAmr()...AppData.AFN=%02x, AppData.Fn=%d...���ձ��ķ�AFN F1-F1����֡! \n", AppData.AFN, AppData.Fn);
                }

                break;
            }           
        }
        else
        {
            //printx("BroadBand_ParallelAmr()����ճ����� �˿�[%d]...RTS_RECV...RecvLenС��0 \n", nTtlIndex);
            //�ж�5�����������Ƿ�������
            if(IsActiveAmrFinished(nTtlIndex))
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "BroadBand_ParallelAmr()�˿�[%d]...RTS_RECV...IsActiveAmrFinished()5���������������� \n", nTtlIndex);
                gTotalState[nTtlIndex].NormalAmr = RTS_END;
            }
            else
            {
                gTotalState[nTtlIndex].NormalAmr = RTS_SEND; //�л�������״̬
            }
        }
        
        return -1;

    case RTS_END:                               //����
        {
            gTotalState[nTtlIndex].NormalAmr = RTS_IDLE;
            //���³���״̬
            gAmrStateQuery[nTtlIndex].nModuleAmrState = 0;
            return 1;
        }       
        break;

    default:
        gTotalState[nTtlIndex].NormalAmr = RTS_IDLE;
        break;
    }

    return -1;
}



/*******************************************************************************
* ��������: NormalDataReading
* ��������: �ճ����ݳ���
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int NormalDataReading(TASK_FUN_INPUT stTaskFunInput)
{
	//printx("int NormalDataReading(TASK_FUN_INPUT stTaskFunInput)�ճ����ݳ���...\n");

	return BroadBand_ParallelAmr(stTaskFunInput);
	
}
#if 0
/*******************************************************************************
* ��������: HplcInfoReport_NetNode
* ��������: HPLC��ѯ��Ϣ�ϱ� ����ڵ���Ϣ
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int HplcInfoReport_NetNode(TASK_FUN_INPUT stTaskFunInput)
{
	uint8           nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;
	CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_NetNode()��ѯ����ڵ���Ϣ HPLC_QUERY_END...�˿�[%d] \n", nTtlIndex);
	
	gQueryTypeHPLC[nTtlIndex].BitType.node = 0;
	return 1;

}
#endif
/*******************************************************************************
* ��������: HplcInfoReport_NodeVersion
* ��������: HPLC��ѯ��Ϣ�ϱ� ����ڵ�汾��Ϣ
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int HplcInfoReport_NodeVersion(TASK_FUN_INPUT stTaskFunInput)
{
    int i   = 0;
    int ret = 0;
    static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //ͨѶ��ʼʱ��
    static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //��ʱʱ��
    static  uint16  CurNodeIndex[MAX_DOWN_THREAD_NUM] = {0};  //2013�浱ǰ������1��ʼ
    uint16  CurInfoIndex = 0;  //��Ϣ��֡ƫ��λ
    uint16  TotalCount = 0;  //�ڵ�������
    uint8   ReplyCount = 0;  //����Ӧ��Ľڵ�����


    AppData_T   SendAppData;                        //����
    AppData_T   RecvAppData;                        //����
    memset(&SendAppData, 0x00, sizeof(AppData_T));
    memset(&RecvAppData, 0x00, sizeof(AppData_T));
    uint16 frameindex = 0;
    uint8           nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
    nTtlIndex = stTaskFunInput.TaskArrIndex;
    //MESSAGE_MQTT_T *msg = &gQueryTypeHPLC[nTtlIndex].msg;
    //uint8 payload[MAX_MSG_PAYLOAD_LEN]={0};
    INFO_DATA_T     InfoMsgData = {0};  //�ظ���Ϣ
    uint8 addr[6]={0};
    uint16 softversion = 0;
    OOP_DATE_T versiondate;
    memset(&versiondate,0x00,sizeof(OOP_DATE_T));
    uint8 factorycode[2]={0};
    uint8 icoode[2]={0};
    switch (gTotalState[nTtlIndex].HplcQuery)
    {
        case HPLC_QUERY_IDLE://����ִ��,��ʼ��
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()��ѯ����ڵ�汾��Ϣ HPLC_QUERY_IDLE...�˿�[%d] \n", nTtlIndex);
            ClearCommRecvBuff(nTtlIndex);  //��ͨѶ���ջ�����
            //ReadAmrInfo(nTtlIndex);  //���ص�����Ϣ��������Ϣ
            CurNodeIndex[nTtlIndex] = 1;//0 ��ʾ CCO�� 1 ��ʾ�� 1 ���ز��ӽڵ㣬 2 �Ժ�
            CommBeginTime[nTtlIndex] = time(NULL);
            OverTimeCnt[nTtlIndex] = 30;
            //memset(&gNodeVersionInfo,0x00,sizeof(HPLC_NODEVERSION_INFO_T));
            gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
            break;
        }
        case HPLC_QUERY_SEND: //����
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()��ѯ����ڵ�汾��Ϣ HPLC_QUERY_SEND...�˿�[%d] \n", nTtlIndex);

            //���ͱ�����֡
            SendAppData.AFN = 0x10;
            SendAppData.Fn = 104;
            SendAppData.DataUnitLen = 3;  //�ڵ���ʼ���2�ֽ�, �ڵ�����1�ֽ�
            memcpy(SendAppData.DataUnitBuff, &CurNodeIndex[nTtlIndex], 2);  //�ӽڵ���ʼ��ţ�2�ֽ�
            SendAppData.DataUnitBuff[2] = 15;  //ÿ�β�ѯ�ڵ�������1�ֽ�
            CCODOWN_FMT_DEBUG(nTtlIndex, "���ͱ���SendAppData �ڵ���ʼ���=%d, �ڵ�����=%d \n", CurNodeIndex[nTtlIndex], SendAppData.DataUnitBuff[2]);
            
            ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
            if(ret > 0)
            {

                //���ձ��Ľ�֡
                memcpy(&TotalCount, &RecvAppData.DataUnitBuff[frameindex], 2);  //�ڵ�������
                frameindex+=2;
                ReplyCount = RecvAppData.DataUnitBuff[frameindex++];  //����Ӧ�������
                CCODOWN_FMT_DEBUG(nTtlIndex, "���ձ��� �ڵ�������=%d,����Ӧ��ڵ�����=%d \n", 
                    TotalCount, ReplyCount);

                if(ReplyCount > 0)
                {
                    CurInfoIndex = 0;
                    //msg->tAXDR.index = GetInfoIndex();
                    Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurInfoIndex]);  //�˿ں�
                    CurInfoIndex += 4;
                    InfoMsgData.aPayloadData[CurInfoIndex] = 0x01;  //�Ƿ�����ʼ�����Ϣ, 0x00:��, 0x01:��
                    CurInfoIndex += 1;
                    memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], (uint8 *)&CurNodeIndex[nTtlIndex] , 2);  //��ʼ���2�ֽ�
                    CurInfoIndex += 2;
                    InfoMsgData.aPayloadData[CurInfoIndex] = 0x09;  //��ѯHPLC��Ϣchoice��־
                    CurInfoIndex += 1;
                    ChangeLenToBuff(ReplyCount, &CurInfoIndex, &InfoMsgData.aPayloadData[CurInfoIndex]);  //��Ϣ����seq of
                    CCODOWN_FMT_DEBUG(nTtlIndex, "��Ϣ����seq of: %d(ʮ����) \n", ReplyCount);
                    
                    for(i=0; i<ReplyCount; i++)
                    {
                        memcpy(addr,&RecvAppData.DataUnitBuff[frameindex],6);
                        frameindex+=6; 
                        memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex],addr,6);
                        CurInfoIndex += 6;
                        softversion = commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex+1])*100+commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex]);
                        frameindex+=2;
                        versiondate.mday = commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex++]);
                        versiondate.month = commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex++]);
                        versiondate.year = commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex++])+2000;
                        memcpy_r(factorycode,&RecvAppData.DataUnitBuff[frameindex],2);
                        frameindex+=2;
                        memcpy_r(icoode,&RecvAppData.DataUnitBuff[frameindex],2);
                        frameindex+=2;

                        memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex],&softversion,2);
                        CurInfoIndex+=2;
                        memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex],&versiondate.year,2);
                        CurInfoIndex+=2;
                        InfoMsgData.aPayloadData[CurInfoIndex++] = versiondate.month;
                        InfoMsgData.aPayloadData[CurInfoIndex++] = versiondate.mday;
                        InfoMsgData.aPayloadData[CurInfoIndex++] = 0xff;
                        memcpy(&InfoMsgData.aPayloadData[CurInfoIndex], factorycode, 2);  //���̴���
                        CurInfoIndex += 2;
                        memcpy(&InfoMsgData.aPayloadData[CurInfoIndex], icoode, 2);  //оƬ����
                        CurInfoIndex += 2;
                    }
                    //cco_axdr_reponse(&msg->tAXDR, payload, CurInfoIndex);
                    InfoMsgData.nPayloadLength = CurInfoIndex;
                    PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);

                }

                CurNodeIndex[nTtlIndex] += ReplyCount;  //��ǰ����ۼ�
                if(CurNodeIndex[nTtlIndex] >= TotalCount)
                {
                    gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//�������
                    break;
                }
            }

            gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;  //�ж��Ƿ�ʱ
            break;
        }
        
        case HPLC_QUERY_RECV:  //����
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()��ѯ����ڵ�汾��ϢHPLC_QUERY_RECV...�˿�[%d] \n", nTtlIndex);
            if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) ��ʱ...\n");
                gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ۼƳ�ʱ, �������
            }
            else
            {
                gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
            }
            
            break;
        }

        case HPLC_QUERY_RETRY:  //����
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()��ѯ����ڵ�汾��Ϣ HPLC_QUERY_RETRY...�˿�[%d] \n", nTtlIndex);
            gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ﵽ������Դ���, �������
            break;
        }

        case HPLC_QUERY_END:  //����
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()��ѯ����ڵ�汾��Ϣ HPLC_QUERY_END...�˿�[%d] \n", nTtlIndex);
            gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
            gQueryTypeHPLC[nTtlIndex].BitType.nodeversion = 0;
            return 1;
            break;
        }

        default:
            break;
    }

    return -1;
}

#if 0
/*******************************************************************************
* ��������: HplcInfoReport_NetBase
* ��������: HPLC��ѯ��Ϣ�ϱ� ���������Ϣ
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int HplcInfoReport_NetBase(TASK_FUN_INPUT stTaskFunInput)
{
	uint8           nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;
	CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_NetBase()��ѯ���������Ϣ HPLC_QUERY_END...�˿�[%d] \n", nTtlIndex);
	
	gQueryTypeHPLC[nTtlIndex].BitType.base = 0;
	return 1;

}
#endif
/*******************************************************************************
* ��������: HplcInfoReport_Module
* ��������: HPLC��ѯ��Ϣ�ϱ� ģ��汾��Ϣ
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int HplcInfoReport_Module(TASK_FUN_INPUT stTaskFunInput)
{
    int ret = 0;
    static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //ͨѶ��ʼʱ��
    static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //��ʱʱ��
    uint16  CurInfoIndex = 0;  //��Ϣ��֡ƫ��λ
    uint16  frameindex=0;
    uint8   num = 0;  

    AppData_T   SendAppData;                        //����
    AppData_T   RecvAppData;                        //����
    memset(&SendAppData, 0x00, sizeof(AppData_T));
    memset(&RecvAppData, 0x00, sizeof(AppData_T));
    
    uint8           nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
    nTtlIndex = stTaskFunInput.TaskArrIndex;
    uint16 softversion = 0;
    OOP_DATE_T versiondate;
    memset(&versiondate,0x00,sizeof(OOP_DATE_T));
    uint8 factorycode[2]={0};
    uint8 icoode[2]={0};
    //MESSAGE_MQTT_T *msg = &gQueryTypeHPLC[nTtlIndex].msg;
    //uint8 payload[MAX_MSG_PAYLOAD_LEN]={0};
    INFO_DATA_T     InfoMsgData = {0};  //�ظ���Ϣ

    switch (gTotalState[nTtlIndex].HplcQuery)
    {
        case HPLC_QUERY_IDLE://����ִ��,��ʼ��
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Module()��ѯģ��ڵ���Ϣ HPLC_QUERY_IDLE...�˿�[%d] \n", nTtlIndex);
            ClearCommRecvBuff(nTtlIndex);  //��ͨѶ���ջ�����
            //ReadAmrInfo(nTtlIndex);  //���ص�����Ϣ��������Ϣ
            CommBeginTime[nTtlIndex] = time(NULL);
            OverTimeCnt[nTtlIndex] = 30;

            gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
            break;
        }
        case HPLC_QUERY_SEND: //����
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Module()��ѯģ��ڵ���Ϣ HPLC_QUERY_SEND...�˿�[%d] \n", nTtlIndex);

            SendAppData.AFN = 0x10;
            SendAppData.Fn = 104;
            SendAppData.DataUnitLen = 3;  
            SendAppData.DataUnitBuff[2] = 1;    //ֻ��Ҫ��CCO�� �ڵ���ʼ���0 ��ѯ�ڵ�����1
            ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
            if(ret > 0)
            {
                frameindex+=2;
                num = RecvAppData.DataUnitBuff[frameindex++];
                if(num==0)
                {
                    CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Module()��ѯģ��ڵ���Ϣ �ظ��ڵ����Ϊ0\n");
                    gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//�������
                    break;
                }
                frameindex+=6; //��ַ����Ҫ
                softversion = commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex+1])*100+commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex]);
                frameindex+=2;
                versiondate.mday = commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex++]);
                versiondate.month = commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex++]);
                versiondate.year = commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex++])+2000;
                memcpy_r(factorycode,&RecvAppData.DataUnitBuff[frameindex],2);
                frameindex+=2;
                memcpy_r(icoode,&RecvAppData.DataUnitBuff[frameindex],2);
                frameindex+=2;

                CurInfoIndex = 0;

                //msg->tAXDR.index = GetInfoIndex();
                Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurInfoIndex]);  //�˿ں�
                CCODOWN_FMT_DEBUG(nTtlIndex, "�˿ں�: %02x %02x %02x %02x \n", 
                        InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
                        InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3]);
                
                CurInfoIndex += 4;
                InfoMsgData.aPayloadData[CurInfoIndex] = 0x00;  //�Ƿ�����ʼ�����Ϣ, 0x00:��, 0x01:��
                CCODOWN_FMT_DEBUG(nTtlIndex, "�Ƿ�����ʼ�����Ϣ: %02x (0x00:��, 0x01:��) \n", InfoMsgData.aPayloadData[CurInfoIndex]);
                
                CurInfoIndex += 1;
                InfoMsgData.aPayloadData[CurInfoIndex] = 0x08;  //��ѯHPLC��Ϣchoice��־
                CCODOWN_FMT_DEBUG(nTtlIndex, "��ѯHPLC��Ϣchoice��־: %02x \n", InfoMsgData.aPayloadData[CurInfoIndex]);
                
                CurInfoIndex += 1;

                memcpy(&InfoMsgData.aPayloadData[CurInfoIndex], factorycode, 2);  //���̴���
                CCODOWN_FMT_DEBUG(nTtlIndex, "���̴���: %02x %02x \n", 
                        InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1]);
                CurInfoIndex += 2;
                
                memcpy(&InfoMsgData.aPayloadData[CurInfoIndex], icoode, 2);  //оƬ����
                CCODOWN_FMT_DEBUG(nTtlIndex, "оƬ����: %02x %02x \n", 
                        InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1]);
                CurInfoIndex += 2;

                memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex],&versiondate.year,2);
                CurInfoIndex+=2;
                InfoMsgData.aPayloadData[CurInfoIndex++] = versiondate.month;
                InfoMsgData.aPayloadData[CurInfoIndex++] = versiondate.mday;
                InfoMsgData.aPayloadData[CurInfoIndex++] = 0xff;
                memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex],&softversion,2);
                CurInfoIndex+=2;
                //cco_axdr_reponse(&msg->tAXDR, payload, CurInfoIndex);
                InfoMsgData.nPayloadLength = CurInfoIndex;
                PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);

                gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//�������
                break;
            }

            gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;
            break;
        }
        
        case HPLC_QUERY_RECV:  //����
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Module()��ѯģ��ڵ���Ϣ HPLC_QUERY_RECV...�˿�[%d] \n", nTtlIndex);
            if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) ��ʱ...\n");
                gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ۼƳ�ʱ, �������
            }
            else
            {
                gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
            }
            
            break;
        }

        case HPLC_QUERY_RETRY:  //����
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Module()��ѯģ��ڵ���Ϣ HPLC_QUERY_RETRY...�˿�[%d] \n", nTtlIndex);
            gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ﵽ������Դ���, �������
            break;
        }

        case HPLC_QUERY_END:  //����
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Module()��ѯģ��ڵ���Ϣ HPLC_QUERY_END...�˿�[%d] \n", nTtlIndex);
            gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
            gQueryTypeHPLC[nTtlIndex].BitType.module = 0;
            return 1;
            break;
        }

        default:
            break;
    }

    return -1;
}

/*******************************************************************************
* ��������: HplcInfoReport_Slave
* ��������: HPLC��ѯ��Ϣ�ϱ� �ӽڵ�ID��Ϣ
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int HplcInfoReport_Slave(TASK_FUN_INPUT stTaskFunInput)
{
	int i   = 0;
	int ret = 0;
	static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //ͨѶ��ʼʱ��
	static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //��ʱʱ��
	static  uint16  CurNodeIndex[MAX_DOWN_THREAD_NUM] = {0};  //2013�浱ǰ������1��ʼ
	uint16  CurInfoIndex = 0;  //��Ϣ��֡ƫ��λ
	uint16  TotalCount = 0;  //�ڵ�������
	uint8   ReplyCount = 0;  //����Ӧ��Ľڵ�����
	//uint16  ReplyNodeIndex = 0;  //����Ӧ�����ʼ���
	uint16  NodeIndexTemp = 0;  //�ڵ����, ��ʱ����
	uint16  CurFrameIndex = 0;  //�յ��ı��Ľ���ƫ��λ
	uint8   LenTemp = 0;  //ģ��ID�ų���, ��ʱ����

	AppData_T   SendAppData;                        //����
    AppData_T   RecvAppData;                        //����
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));
	
	uint8           nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	INFO_DATA_T     InfoMsgData;  //�ظ���Ϣ
	memset(&InfoMsgData, 0, sizeof(InfoMsgData));

	switch (gTotalState[nTtlIndex].HplcQuery)
	{
		case HPLC_QUERY_IDLE://����ִ��,��ʼ��
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Slave()��ѯ�ӽڵ�ID��Ϣ HPLC_QUERY_IDLE...�˿�[%d] \n", nTtlIndex);
			ClearCommRecvBuff(nTtlIndex);  //��ͨѶ���ջ�����
			ReadAmrInfo(nTtlIndex);  //���ص�����Ϣ��������Ϣ
			CurNodeIndex[nTtlIndex] = 1;//2013�浱ǰ������1��ʼ
			CommBeginTime[nTtlIndex] = time(NULL);
			OverTimeCnt[nTtlIndex] = 30;
			
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
			break;
		}
		case HPLC_QUERY_SEND: //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Slave()��ѯ�ӽڵ�ID��Ϣ HPLC_QUERY_SEND...�˿�[%d] \n", nTtlIndex);

			//���ͱ�����֡
			SendAppData.AFN = 0x10;
		    SendAppData.Fn = 7;
		    SendAppData.DataUnitLen = 3;  //�ڵ���ʼ���2�ֽ�, �ڵ�����1�ֽ�
			memcpy(SendAppData.DataUnitBuff, &CurNodeIndex[nTtlIndex], 2);  //�ӽڵ���ʼ��ţ�2�ֽ�
			SendAppData.DataUnitBuff[2] = 10;  //ÿ�β�ѯ�ڵ�������1�ֽ�
			CCODOWN_FMT_DEBUG(nTtlIndex, "���ͱ���SendAppData �ڵ���ʼ���=%d, �ڵ�����=%d \n", CurNodeIndex[nTtlIndex], SendAppData.DataUnitBuff[2]);
			
			ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
			if(ret > 0)
			{
				//���ձ��Ľ�֡
				memcpy(&TotalCount, &RecvAppData.DataUnitBuff[0], 2);  //�ڵ�������
				ReplyCount = RecvAppData.DataUnitBuff[2];  //����Ӧ�������
				CCODOWN_FMT_DEBUG(nTtlIndex, "���ձ���RecvAppData �ڵ�������=%d, ����Ӧ��ڵ�����=%d \n", 
					TotalCount, ReplyCount);

				if(ReplyCount > 0)
				{
					//��װ��Ϣ����
					InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
					InfoMsgData.IP  = ntohl(InfoMsgData.IP);
					InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
					InfoMsgData.MsgPRIORITY = 0;
					InfoMsgData.MsgRPM = 1;
					InfoMsgData.MsgIndex = GetInfoIndex();
					InfoMsgData.MsgLabel = nTtlIndex;  //mqtt��Ŷ˿����(0��1)
					InfoMsgData.SourLen = strlen(gCcoRouterAppName);
					memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
					memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
					InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
					memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
					memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
					
					CurInfoIndex = 0;

					InfoMsgData.IID_ID = 0x0003;
					InfoMsgData.IOP_ID = 0x0051;
					
					Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurInfoIndex]);  //�˿ں�
					CCODOWN_FMT_DEBUG(nTtlIndex, "�˿ں�: %02x %02x %02x %02x \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
						InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3]);
					
					CurInfoIndex += 4;
					InfoMsgData.aPayloadData[CurInfoIndex] = 0x01;  //�Ƿ�����ʼ�����Ϣ, 0x00:��, 0x01:��
					CCODOWN_FMT_DEBUG(nTtlIndex, "�Ƿ�����ʼ�����Ϣ: %02x (0x00:��, 0x01:��) \n", InfoMsgData.aPayloadData[CurInfoIndex]);
					
					CurInfoIndex += 1;
					memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], (uint8 *)&CurNodeIndex[nTtlIndex], 2);  //��ʼ���2�ֽ�
					CCODOWN_FMT_DEBUG(nTtlIndex, "��ʼ���: %02x %02x (ʮ���� %d)\n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], CurNodeIndex[nTtlIndex]);
					
					CurInfoIndex += 2;
					InfoMsgData.aPayloadData[CurInfoIndex] = 0x07;  //��ѯHPLC��Ϣchoice��־
					CCODOWN_FMT_DEBUG(nTtlIndex, "��ѯHPLC��Ϣchoice��־: %02x \n", InfoMsgData.aPayloadData[CurInfoIndex]);
					
					CurInfoIndex += 1;
					ChangeLenToBuff(ReplyCount, &CurInfoIndex, &InfoMsgData.aPayloadData[CurInfoIndex]);  //��Ϣ����seq of
					CCODOWN_FMT_DEBUG(nTtlIndex, "��Ϣ����seq of: %d(ʮ����) \n", ReplyCount);
					
					CurFrameIndex = 3;  //�յ��Ļظ�����, �ӽڵ�������2�ֽ�, ����Ӧ������1�ֽ�
					for(i=0; i<ReplyCount; i++)
					{
  						NodeIndexTemp = i + CurNodeIndex[nTtlIndex];
  						Uint16ToCharArr(NodeIndexTemp, &InfoMsgData.aPayloadData[CurInfoIndex]);  //�ڵ����2�ֽ�
  						CCODOWN_FMT_DEBUG(nTtlIndex, "i=%d, �ӽڵ����: %02x %02x (ʮ���� %d) \n", i, 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], NodeIndexTemp);
						
						CurInfoIndex += 2;
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[CurFrameIndex], 6);  //�ڵ��ַ
						CCODOWN_FMT_DEBUG(nTtlIndex, "�ӽڵ��ַ: %02x %02x %02x %02x %02x %02x \n", 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
							InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3], 
							InfoMsgData.aPayloadData[CurInfoIndex+4], InfoMsgData.aPayloadData[CurInfoIndex+5]);
						
						CurInfoIndex += 6;
						CurFrameIndex += 6;
						InfoMsgData.aPayloadData[CurInfoIndex] = RecvAppData.DataUnitBuff[CurFrameIndex];  //�ڵ��豸����
						CCODOWN_FMT_DEBUG(nTtlIndex, "�ӽڵ��豸����: %02x (ʮ���� %d) \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex]);
						
						CurInfoIndex += 1;
						CurFrameIndex += 1;
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[CurFrameIndex], 2);  //ģ�鳧�̴���
						CCODOWN_FMT_DEBUG(nTtlIndex, "�ӽڵ�ģ�鳧�̴���: %02x %02x \n", 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1]);
						
						CurInfoIndex += 2;
						CurFrameIndex += 2;
						LenTemp = RecvAppData.DataUnitBuff[CurFrameIndex];
						InfoMsgData.aPayloadData[CurInfoIndex] = RecvAppData.DataUnitBuff[CurFrameIndex];  //ģ��ID����
						CCODOWN_FMT_DEBUG(nTtlIndex, "�ӽڵ�ģ��ID����: %02x (ʮ���� %d) \n", 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex]);
						
						CurInfoIndex += 1;
						CurFrameIndex += 1;
						InfoMsgData.aPayloadData[CurInfoIndex] = RecvAppData.DataUnitBuff[CurFrameIndex];  //ģ��ID��ʽ
						CCODOWN_FMT_DEBUG(nTtlIndex, "�ӽڵ�ģ��ID��ʽ: %02x (ʮ���� %d) \n", 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex]);
						
						CurInfoIndex += 1;
						CurFrameIndex += 1;
						ChangeLenToBuff(LenTemp, &CurInfoIndex, &InfoMsgData.aPayloadData[CurInfoIndex]);  //visible-string ����
						CCODOWN_FMT_DEBUG(nTtlIndex, "�ӽڵ�ģ��ID visible-string ����: %d(ʮ����) \n", LenTemp);
						
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[CurFrameIndex], LenTemp);  //ģ��ID
						CCODOWN_BUF_DEBUG(nTtlIndex, &InfoMsgData.aPayloadData[CurInfoIndex], LenTemp, "�ӽڵ�ģ��ID:");
						
						CurInfoIndex += LenTemp;
						CurFrameIndex += LenTemp;
					}
					
					InfoMsgData.nPayloadLength = CurInfoIndex;
					PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
				}

				CurNodeIndex[nTtlIndex] += ReplyCount;  //��ǰ����ۼ�
				if(CurNodeIndex[nTtlIndex] > TotalCount)
				{
					gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//�������
					break;
				}
			}

			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;  //�ж��Ƿ�ʱ
			break;
		}
		
		case HPLC_QUERY_RECV:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Slave()��ѯ�ӽڵ�ID��Ϣ HPLC_QUERY_RECV...�˿�[%d] \n", nTtlIndex);
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) ��ʱ...\n");
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ۼƳ�ʱ, �������
			}
			else
			{
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
			}
			
			break;
		}

		case HPLC_QUERY_RETRY:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Slave()��ѯ�ӽڵ�ID��Ϣ HPLC_QUERY_RETRY...�˿�[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ﵽ������Դ���, �������
			break;
		}

		case HPLC_QUERY_END:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Slave()��ѯ�ӽڵ�ID��Ϣ HPLC_QUERY_END...�˿�[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
			gQueryTypeHPLC[nTtlIndex].BitType.slave = 0;
			return 1;
			break;
		}

		default:
			break;
	}

	return -1;
}


/*******************************************************************************
* ��������: HplcInfoReport_Master
* ��������: HPLC��ѯ��Ϣ�ϱ� �������ڵ�ģ��ID��Ϣ
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int HplcInfoReport_Master(TASK_FUN_INPUT stTaskFunInput)
{
	int ret = 0;
	static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //ͨѶ��ʼʱ��
	static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //��ʱʱ��
	uint16  CurInfoIndex = 0;  //��Ϣ��֡ƫ��λ
	uint8   LenTemp = 0;  //ģ��ID�ų���, ��ʱ����

	AppData_T   SendAppData;                        //����
    AppData_T   RecvAppData;                        //����
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));
	
	uint8           nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	INFO_DATA_T     InfoMsgData;  //�ظ���Ϣ
	memset(&InfoMsgData, 0, sizeof(InfoMsgData));

	switch (gTotalState[nTtlIndex].HplcQuery)
	{
		case HPLC_QUERY_IDLE://����ִ��,��ʼ��
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Master()��ѯ�������ڵ� HPLC_QUERY_IDLE...�˿�[%d] \n", nTtlIndex);
			ClearCommRecvBuff(nTtlIndex);  //��ͨѶ���ջ�����
			ReadAmrInfo(nTtlIndex);  //���ص�����Ϣ��������Ϣ
			CommBeginTime[nTtlIndex] = time(NULL);
			OverTimeCnt[nTtlIndex] = 30;
			
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
			break;
		}
		case HPLC_QUERY_SEND: //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Master()��ѯ�������ڵ� HPLC_QUERY_SEND...�˿�[%d] \n", nTtlIndex);

			SendAppData.AFN = 0x03;
		    SendAppData.Fn = 12;
		    SendAppData.DataUnitLen = 0;  //�����ݵ�Ԫ
			
			ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
			if(ret > 0)
			{
				//��װ��Ϣ����
				InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
				InfoMsgData.IP  = ntohl(InfoMsgData.IP);
				InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
				InfoMsgData.MsgPRIORITY = 0;
				InfoMsgData.MsgRPM = 1;
				InfoMsgData.MsgIndex = GetInfoIndex();
				InfoMsgData.MsgLabel = nTtlIndex;  //mqtt��Ŷ˿����(0��1)
				InfoMsgData.SourLen = strlen(gCcoRouterAppName);
				memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
				memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
				InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
				memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
				memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
				
				CurInfoIndex = 0;

				InfoMsgData.IID_ID = 0x0003;
				InfoMsgData.IOP_ID = 0x0051;
				
				Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurInfoIndex]);  //�˿ں�
				CCODOWN_FMT_DEBUG(nTtlIndex, "�˿ں�: %02x %02x %02x %02x \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
						InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3]);
				
				CurInfoIndex += 4;
				InfoMsgData.aPayloadData[CurInfoIndex] = 0x00;  //�Ƿ�����ʼ�����Ϣ, 0x00:��, 0x01:��
				CCODOWN_FMT_DEBUG(nTtlIndex, "�Ƿ�����ʼ�����Ϣ: %02x (0x00:��, 0x01:��) \n", InfoMsgData.aPayloadData[CurInfoIndex]);
				
				CurInfoIndex += 1;
				InfoMsgData.aPayloadData[CurInfoIndex] = 0x06;  //��ѯHPLC��Ϣchoice��־
				CCODOWN_FMT_DEBUG(nTtlIndex, "��ѯHPLC��Ϣchoice��־: %02x \n", InfoMsgData.aPayloadData[CurInfoIndex]);
				
				CurInfoIndex += 1;
				//ChangeLenToBuff(1, &CurInfoIndex, &InfoMsgData.aPayloadData[CurInfoIndex]);  //��Ϣ����seq of
				memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[0], 2);  //ģ�鳧�̴���
				CCODOWN_FMT_DEBUG(nTtlIndex, "ģ�鳧�̴���: %02x %02x \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1]);
				
				CurInfoIndex += 2;
				InfoMsgData.aPayloadData[CurInfoIndex] = RecvAppData.DataUnitBuff[2];  //ģ��ID����
				LenTemp = RecvAppData.DataUnitBuff[2];
				CCODOWN_FMT_DEBUG(nTtlIndex, "ģ��ID����: %02x (ʮ���� %d) \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex]);
				
				CurInfoIndex += 1;
				InfoMsgData.aPayloadData[CurInfoIndex] = RecvAppData.DataUnitBuff[3];  //ģ��ID��ʽ
				CCODOWN_FMT_DEBUG(nTtlIndex, "ģ��ID��ʽ: %02x (ʮ���� %d) \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex]);
				
				CurInfoIndex += 1;
				ChangeLenToBuff(LenTemp, &CurInfoIndex, &InfoMsgData.aPayloadData[CurInfoIndex]);  //visible-string ����
				CCODOWN_FMT_DEBUG(nTtlIndex, "ģ��ID visible-string ����: %d(ʮ����) \n", LenTemp);
				
				memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[4], RecvAppData.DataUnitBuff[2]);  //ģ��ID
				CCODOWN_BUF_DEBUG(nTtlIndex, &InfoMsgData.aPayloadData[CurInfoIndex], RecvAppData.DataUnitBuff[2], "ģ��ID:");
				
				CurInfoIndex += RecvAppData.DataUnitBuff[2];

				InfoMsgData.nPayloadLength = CurInfoIndex;
				PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);

				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//�������
				break;
			}

			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;
			break;
		}
		
		case HPLC_QUERY_RECV:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Master()��ѯ�������ڵ� HPLC_QUERY_RECV...�˿�[%d] \n", nTtlIndex);
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) ��ʱ...\n");
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ۼƳ�ʱ, �������
			}
			else
			{
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
			}
			
			break;
		}

		case HPLC_QUERY_RETRY:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Master()��ѯ�������ڵ� HPLC_QUERY_RETRY...�˿�[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ﵽ������Դ���, �������
			break;
		}

		case HPLC_QUERY_END:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Master()��ѯ�������ڵ� HPLC_QUERY_END...�˿�[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
			gQueryTypeHPLC[nTtlIndex].BitType.master = 0;
			return 1;
			break;
		}

		default:
			break;
	}

	return -1;
}


/*******************************************************************************
* ��������: HplcInfoReport_Scale
* ��������: HPLC��ѯ��Ϣ�ϱ� �����ģ
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int HplcInfoReport_Scale(TASK_FUN_INPUT stTaskFunInput)
{
	//int i   = 0;
	int ret = 0;
	static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //ͨѶ��ʼʱ��
	static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //��ʱʱ��
	uint16  CurInfoIndex = 0;  //��Ϣ��֡ƫ��λ
	uint16  TotalCount = 0;  //�ڵ�������
	uint16   MaxAllowedCount = 0;  //·��֧�����ӽڵ�����

	AppData_T   SendAppData;                        //����
    AppData_T   RecvAppData;                        //����
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));
	
	uint8           nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Scale() &SendAppData=%p, &RecvAppData=%p, &nTtlIndex=%p \n", 
		&SendAppData, &RecvAppData, &nTtlIndex);
	CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Scale() stTaskFunInput.TaskArrIndex:%d, nTtlIndex:%d \n", 
		stTaskFunInput.TaskArrIndex, nTtlIndex);

	INFO_DATA_T     InfoMsgData;  //�ظ���Ϣ
	memset(&InfoMsgData, 0, sizeof(InfoMsgData));

	switch (gTotalState[nTtlIndex].HplcQuery)
	{
		case HPLC_QUERY_IDLE://����ִ��,��ʼ��
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Scale()��ѯ�����ģ HPLC_QUERY_IDLE...�˿�[%d] \n", nTtlIndex);
			ClearCommRecvBuff(nTtlIndex);  //��ͨѶ���ջ�����
			ReadAmrInfo(nTtlIndex);  //���ص�����Ϣ��������Ϣ
			CommBeginTime[nTtlIndex] = time(NULL);
			OverTimeCnt[nTtlIndex] = 30;
			
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
			break;
		}
		case HPLC_QUERY_SEND: //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Scale()��ѯ�����ģ HPLC_QUERY_SEND...�˿�[%d] \n", nTtlIndex);

			SendAppData.AFN = 0x10;
		    SendAppData.Fn = 1;
		    SendAppData.DataUnitLen = 0;  //�����ݵ�Ԫ
			ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
			if(ret > 0)
			{
				memcpy(&TotalCount, &RecvAppData.DataUnitBuff[0], 2);  //�ڵ�������
				memcpy(&MaxAllowedCount, &RecvAppData.DataUnitBuff[2], 2);  //·��֧�����ӽڵ�����
				CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Scale() �ڵ�������=%d, ·��֧�����ӽڵ�����=%d, ��󳭱�߽����=%d \n", TotalCount, MaxAllowedCount, MAX_AMR_COLLECT_CNT);
				if(TotalCount > MAX_AMR_COLLECT_CNT)
				{
					TotalCount = MAX_AMR_COLLECT_CNT;
				}

				CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Scale() stTaskFunInput.TaskArrIndex:%d, nTtlIndex:%d \n", 
					stTaskFunInput.TaskArrIndex, nTtlIndex);
				//��װ��Ϣ����
				InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
				InfoMsgData.IP  = ntohl(InfoMsgData.IP);
				InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
				InfoMsgData.MsgPRIORITY = 0;
				InfoMsgData.MsgRPM = 1;
				InfoMsgData.MsgIndex = GetInfoIndex();
				InfoMsgData.MsgLabel = nTtlIndex;  //mqtt��Ŷ˿����(0��1)
				InfoMsgData.SourLen = strlen(gCcoRouterAppName);
				memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
				memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
				InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
				memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
				memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
				
				CurInfoIndex = 0;

				InfoMsgData.IID_ID = 0x0003;
				InfoMsgData.IOP_ID = 0x0051;
				
				Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurInfoIndex]);  //�˿ں�
				CCODOWN_FMT_DEBUG(nTtlIndex, "�˿ں�: %02x %02x %02x %02x \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
						InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3]);
				
				CurInfoIndex += 4;
				InfoMsgData.aPayloadData[CurInfoIndex] = 0x00;  //�Ƿ�����ʼ�����Ϣ, 0x00:��, 0x01:��
				CCODOWN_FMT_DEBUG(nTtlIndex, "�Ƿ�����ʼ�����Ϣ: %02x (0x00:��, 0x01:��) \n", InfoMsgData.aPayloadData[CurInfoIndex]);
				
				CurInfoIndex += 1;
				InfoMsgData.aPayloadData[CurInfoIndex] = 0x05;  //��ѯHPLC��Ϣchoice��־
				CCODOWN_FMT_DEBUG(nTtlIndex, "��ѯHPLC��Ϣchoice��־: %02x \n", InfoMsgData.aPayloadData[CurInfoIndex]);
				
				CurInfoIndex += 1;
				Uint16ToCharArr(TotalCount, &InfoMsgData.aPayloadData[CurInfoIndex]);  //�ӽڵ�������
				CCODOWN_FMT_DEBUG(nTtlIndex, "�ӽڵ�������(�����ģ): %02x %02x (ʮ���� %d)\n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], TotalCount);
				
				CurInfoIndex += 2;

				InfoMsgData.nPayloadLength = CurInfoIndex;
				PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);

				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//�������
				break;
			}

			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;
			break;
		}
		
		case HPLC_QUERY_RECV:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Scale()��ѯ�����ģ HPLC_QUERY_RECV...�˿�[%d] \n", nTtlIndex);
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) ��ʱ...\n");
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ۼƳ�ʱ, �������
			}
			else
			{
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
			}
			
			break;
		}

		case HPLC_QUERY_RETRY:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Scale()��ѯ�����ģ HPLC_QUERY_RETRY...�˿�[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ﵽ������Դ���, �������
			break;
		}

		case HPLC_QUERY_END:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Scale()��ѯ�����ģ HPLC_QUERY_END...�˿�[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
			gQueryTypeHPLC[nTtlIndex].BitType.scale = 0;
			return 1;
			break;
		}

		default:
			break;
	}

	return -1;
}


/*******************************************************************************
* ��������: HplcInfoReport_Phase
* ��������: HPLC��ѯ��Ϣ�ϱ� �ڵ�������Ϣ
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int HplcInfoReport_Phase(TASK_FUN_INPUT stTaskFunInput)
{
	int i   = 0;
	int ret = 0;
	static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //ͨѶ��ʼʱ��
	static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //��ʱʱ��
	static  uint16  CurNodeIndex[MAX_DOWN_THREAD_NUM] = {0};  //2013�浱ǰ������1��ʼ
	uint16  CurInfoIndex = 0;  //��Ϣ��֡ƫ��λ
	uint16  TotalCount = 0;  //�ڵ�������
	uint8   ReplyCount = 0;  //����Ӧ��Ľڵ�����
	uint16  ReplyNodeIndex = 0;  //����Ӧ�����ʼ���
	uint16  NodeIndexTemp = 0;  //�ڵ����, ��ʱ����

	AppData_T   SendAppData;                        //����
    AppData_T   RecvAppData;                        //����
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));
	
	uint8           nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	INFO_DATA_T     InfoMsgData;  //�ظ���Ϣ
	memset(&InfoMsgData, 0, sizeof(InfoMsgData));

	switch (gTotalState[nTtlIndex].HplcQuery)
	{
		case HPLC_QUERY_IDLE://����ִ��,��ʼ��
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Phase()��ѯ�ڵ�������Ϣ HPLC_QUERY_IDLE...�˿�[%d] \n", nTtlIndex);
			ClearCommRecvBuff(nTtlIndex);  //��ͨѶ���ջ�����
			ReadAmrInfo(nTtlIndex);  //���ص�����Ϣ��������Ϣ
			CurNodeIndex[nTtlIndex] = 1;//2013�浱ǰ������1��ʼ
			CommBeginTime[nTtlIndex] = time(NULL);
			OverTimeCnt[nTtlIndex] = 30;
			
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
			break;
		}
		case HPLC_QUERY_SEND: //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Phase()��ѯ�ڵ�������Ϣ HPLC_QUERY_SEND...�˿�[%d] \n", nTtlIndex);

			//���ͱ�����֡
			SendAppData.AFN = 0x10;
		    SendAppData.Fn = 31;
		    SendAppData.DataUnitLen = 3;  //�ڵ���ʼ���2�ֽ�, �ڵ�����1�ֽ�
			memcpy(SendAppData.DataUnitBuff, &CurNodeIndex[nTtlIndex], 2);  //�ӽڵ���ʼ��ţ�2�ֽ�
			SendAppData.DataUnitBuff[2] = 10;  //ÿ�β�ѯ�ڵ�������1�ֽ�
			CCODOWN_FMT_DEBUG(nTtlIndex, "���ͱ���SendAppData �ڵ���ʼ���=%d, �ڵ�����=%d \n", CurNodeIndex[nTtlIndex], SendAppData.DataUnitBuff[2]);
			
			ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
			if(ret > 0)
			{
				//���ձ��Ľ�֡
				memcpy(&TotalCount, &RecvAppData.DataUnitBuff[0], 2);  //�ڵ�������
				memcpy(&ReplyNodeIndex, &RecvAppData.DataUnitBuff[2], 2);  //�ڵ���ʼ���
				ReplyCount = RecvAppData.DataUnitBuff[4];  //����Ӧ�������
				CCODOWN_FMT_DEBUG(nTtlIndex, "���ձ���RecvAppData �ڵ�������=%d, �ڵ���ʼ���=%d, ����Ӧ��ڵ�����=%d \n", 
					TotalCount, ReplyNodeIndex, ReplyCount);

				if(ReplyCount > 0)
				{
					//��װ��Ϣ����
					InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
					InfoMsgData.IP  = ntohl(InfoMsgData.IP);
					InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
					InfoMsgData.MsgPRIORITY = 0;
					InfoMsgData.MsgRPM = 1;
					InfoMsgData.MsgIndex = GetInfoIndex();
					InfoMsgData.MsgLabel = nTtlIndex;  //mqtt��Ŷ˿����(0��1)
					InfoMsgData.SourLen = strlen(gCcoRouterAppName);
					memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
					memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
					InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
					memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
					memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
					
					CurInfoIndex = 0;

					InfoMsgData.IID_ID = 0x0003;
					InfoMsgData.IOP_ID = 0x0051;
					
					Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurInfoIndex]);  //�˿ں�
					CCODOWN_FMT_DEBUG(nTtlIndex, "�˿ں�: %02x %02x %02x %02x \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
						InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3]);
					
					CurInfoIndex += 4;
					InfoMsgData.aPayloadData[CurInfoIndex] = 0x01;  //�Ƿ�����ʼ�����Ϣ, 0x00:��, 0x01:��
					CCODOWN_FMT_DEBUG(nTtlIndex, "�Ƿ�����ʼ�����Ϣ: %02x (0x00:��, 0x01:��) \n", InfoMsgData.aPayloadData[CurInfoIndex]);
					
					CurInfoIndex += 1;
					memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], (uint8 *)&ReplyNodeIndex, 2);  //��ʼ���2�ֽ�
					CCODOWN_FMT_DEBUG(nTtlIndex, "��ʼ���: %02x %02x (ʮ���� %d)\n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], ReplyNodeIndex);
					
					CurInfoIndex += 2;
					InfoMsgData.aPayloadData[CurInfoIndex] = 0x04;  //��ѯHPLC��Ϣchoice��־
					CCODOWN_FMT_DEBUG(nTtlIndex, "��ѯHPLC��Ϣchoice��־: %02x \n", InfoMsgData.aPayloadData[CurInfoIndex]);
					
					CurInfoIndex += 1;
					ChangeLenToBuff(ReplyCount, &CurInfoIndex, &InfoMsgData.aPayloadData[CurInfoIndex]);  //��Ϣ����seq of
					CCODOWN_FMT_DEBUG(nTtlIndex, "��Ϣ����seq of: %d(ʮ����) \n", ReplyCount);
					
					for(i=0; i<ReplyCount; i++)
					{
  						NodeIndexTemp = i + ReplyNodeIndex;
  						Uint16ToCharArr(NodeIndexTemp, &InfoMsgData.aPayloadData[CurInfoIndex]);  //�ڵ����2�ֽ�
  						CCODOWN_FMT_DEBUG(nTtlIndex, "i=%d, �ڵ����: %02x %02x (ʮ���� %d) \n", i, 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], NodeIndexTemp);
						
						CurInfoIndex += 2;
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[5+i*8], 6);  //�ڵ��ַ
						CCODOWN_FMT_DEBUG(nTtlIndex, "�ڵ��ַ: %02x %02x %02x %02x %02x %02x \n", 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
							InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3], 
							InfoMsgData.aPayloadData[CurInfoIndex+4], InfoMsgData.aPayloadData[CurInfoIndex+5]);
						
						CurInfoIndex += 6;
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[5+i*8+6], 2);  //�ڵ���λ��Ϣ
						CCODOWN_FMT_DEBUG(nTtlIndex, "�ڵ���λ��Ϣ: %02x %02x \n", 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1]);
						
						CurInfoIndex += 2;
					}
					InfoMsgData.nPayloadLength = CurInfoIndex;
					PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
				}

				CurNodeIndex[nTtlIndex] += ReplyCount;  //��ǰ����ۼ�
				if(CurNodeIndex[nTtlIndex] > TotalCount)
				{
					gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//�������
					break;
				}
			}

			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;  //�ж��Ƿ�ʱ
			break;
		}
		
		case HPLC_QUERY_RECV:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Phase()��ѯ�ڵ�������Ϣ HPLC_QUERY_RECV...�˿�[%d] \n", nTtlIndex);
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) ��ʱ...\n");
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ۼƳ�ʱ, �������
			}
			else
			{
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
			}
			
			break;
		}

		case HPLC_QUERY_RETRY:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Phase()��ѯ�ڵ�������Ϣ HPLC_QUERY_RETRY...�˿�[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ﵽ������Դ���, �������
			break;
		}

		case HPLC_QUERY_END:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Phase()��ѯ�ڵ�������Ϣ HPLC_QUERY_END...�˿�[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
			gQueryTypeHPLC[nTtlIndex].BitType.phase = 0;
			return 1;
			break;
		}

		default:
			break;
	}

	return -1;
}


/*******************************************************************************
* ��������: HplcInfoReport_Nets
* ��������: HPLC��ѯ��Ϣ�ϱ� ��������Ϣ
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int HplcInfoReport_Nets(TASK_FUN_INPUT stTaskFunInput)
{
	int i   = 0;
	int ret = 0;
	static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //ͨѶ��ʼʱ��
	static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //��ʱʱ��
	uint16  CurInfoIndex = 0;  //��Ϣ��֡ƫ��λ
	uint16  TotalCount = 0;  //�ڵ�������

	AppData_T   SendAppData;                        //����
    AppData_T   RecvAppData;                        //����
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));
	
	uint8           nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	INFO_DATA_T     InfoMsgData;  //�ظ���Ϣ
	memset(&InfoMsgData, 0, sizeof(InfoMsgData));

	switch (gTotalState[nTtlIndex].HplcQuery)
	{
		case HPLC_QUERY_IDLE://����ִ��,��ʼ��
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Nets()��ѯ��������Ϣ HPLC_QUERY_IDLE...�˿�[%d] \n", nTtlIndex);
			ClearCommRecvBuff(nTtlIndex);  //��ͨѶ���ջ�����
			ReadAmrInfo(nTtlIndex);  //���ص�����Ϣ��������Ϣ
			CommBeginTime[nTtlIndex] = time(NULL);
			OverTimeCnt[nTtlIndex] = 30;
			
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
			break;
		}
		case HPLC_QUERY_SEND: //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Nets()��ѯ��������Ϣ HPLC_QUERY_SEND...�˿�[%d] \n", nTtlIndex);

			//���ͱ�����֡
			SendAppData.AFN = 0x10;
		    SendAppData.Fn = 111;
		    SendAppData.DataUnitLen = 0;  //�����ݵ�Ԫ
			
			ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
			if(ret > 0)
			{
				//���ձ��Ľ�֡
				TotalCount = RecvAppData.DataUnitBuff[0];  //������ڵ�������
				CCODOWN_FMT_DEBUG(nTtlIndex, "���ձ���RecvAppData �ڵ�������=%d, ���ڵ������ʶ��=%02x%02x%02x, ���ڵ����ڵ��ַ=%02x%02x%02x%02x%02x%02x \n", 
							TotalCount, RecvAppData.DataUnitBuff[3], RecvAppData.DataUnitBuff[2], RecvAppData.DataUnitBuff[1], 
							RecvAppData.DataUnitBuff[9], RecvAppData.DataUnitBuff[8], RecvAppData.DataUnitBuff[7], 
							RecvAppData.DataUnitBuff[6], RecvAppData.DataUnitBuff[5], RecvAppData.DataUnitBuff[4]);

				//��װ��Ϣ����
				InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
				InfoMsgData.IP  = ntohl(InfoMsgData.IP);
				InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
				InfoMsgData.MsgPRIORITY = 0;
				InfoMsgData.MsgRPM = 1;
				InfoMsgData.MsgIndex = GetInfoIndex();
				InfoMsgData.MsgLabel = nTtlIndex;  //mqtt��Ŷ˿����(0��1)
				InfoMsgData.SourLen = strlen(gCcoRouterAppName);
				memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
				memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
				InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
				memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
				memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
				
				CurInfoIndex = 0;

				InfoMsgData.IID_ID = 0x0003;
				InfoMsgData.IOP_ID = 0x0051;
				
				Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurInfoIndex]);  //�˿ں�
				CCODOWN_FMT_DEBUG(nTtlIndex, "�˿ں�: %02x %02x %02x %02x \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
						InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3]);
				
				CurInfoIndex += 4;
				InfoMsgData.aPayloadData[CurInfoIndex] = 0x00;  //�Ƿ�����ʼ�����Ϣ, 0x00:��, 0x01:��
				CCODOWN_FMT_DEBUG(nTtlIndex, "�Ƿ�����ʼ�����Ϣ: %02x (0x00:��, 0x01:��) \n", InfoMsgData.aPayloadData[CurInfoIndex]);
				
				CurInfoIndex += 1;
				InfoMsgData.aPayloadData[CurInfoIndex] = 0x03;  //��ѯHPLC��Ϣchoice��־
				CCODOWN_FMT_DEBUG(nTtlIndex, "��ѯHPLC��Ϣchoice��־: %02x \n", InfoMsgData.aPayloadData[CurInfoIndex]);
				
				CurInfoIndex += 1;
				InfoMsgData.aPayloadData[CurInfoIndex] = RecvAppData.DataUnitBuff[0];  //�ھӽڵ����
				CCODOWN_FMT_DEBUG(nTtlIndex, "�ھӽڵ����: %02x (ʮ���� %d) \n", InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex]);
				
				CurInfoIndex += 1;
				memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[1], 3);  //���ڵ������ʶ��
				CCODOWN_FMT_DEBUG(nTtlIndex, "���ڵ������ʶ��: %02x %02x %02x \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
						InfoMsgData.aPayloadData[CurInfoIndex+2]);
				
				CurInfoIndex += 3;
				memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[4], 6);  //���ڵ����ڵ��ַ
				CCODOWN_FMT_DEBUG(nTtlIndex, "���ڵ����ڵ��ַ: %02x %02x %02x %02x %02x %02x \n", 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
							InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3], 
							InfoMsgData.aPayloadData[CurInfoIndex+4], InfoMsgData.aPayloadData[CurInfoIndex+5]);
				
				CurInfoIndex += 6;
				ChangeLenToBuff(TotalCount, &CurInfoIndex, &InfoMsgData.aPayloadData[CurInfoIndex]);  //�ھӽڵ������ʶ�� seq of
				CCODOWN_FMT_DEBUG(nTtlIndex, "�ھӽڵ������ʶ�� seq of: %d(ʮ����) \n", TotalCount);
				
				for(i=0; i<TotalCount; i++)
				{
					memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[10+i*3], 3);  //�ھӽڵ������ʶ��
					CCODOWN_FMT_DEBUG(nTtlIndex, "i=%d, �ھӽڵ������ʶ��: %02x %02x %02x \n", i, 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
						InfoMsgData.aPayloadData[CurInfoIndex+2]);
					
					CurInfoIndex += 3;
				}
				InfoMsgData.nPayloadLength = CurInfoIndex;
				PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);

				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//�������
				break;
			}

			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;  //�ж��Ƿ�ʱ
			break;
		}
		
		case HPLC_QUERY_RECV:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Nets()��ѯ��������Ϣ HPLC_QUERY_RECV...�˿�[%d] \n", nTtlIndex);
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) ��ʱ...\n");
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ۼƳ�ʱ, �������
			}
			else
			{
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
			}
			
			break;
		}

		case HPLC_QUERY_RETRY:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Nets()��ѯ��������Ϣ HPLC_QUERY_RETRY...�˿�[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ﵽ������Դ���, �������
			break;
		}

		case HPLC_QUERY_END:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Nets()��ѯ��������Ϣ HPLC_QUERY_END...�˿�[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
			gQueryTypeHPLC[nTtlIndex].BitType.nets = 0;
			return 1;
			break;
		}

		default:
			break;
	}

	return -1;
}


/*******************************************************************************
* ��������: HplcInfoReport_Chip
* ��������: HPLC��ѯ��Ϣ�ϱ� ����ز�оƬ��Ϣ
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int HplcInfoReport_Chip(TASK_FUN_INPUT stTaskFunInput)
{
	int i   = 0;
	int ret = 0;
	static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //ͨѶ��ʼʱ��
	static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //��ʱʱ��
	static  uint16  CurNodeIndex[MAX_DOWN_THREAD_NUM] = {0};  //2013�浱ǰ������1��ʼ
	uint16  CurInfoIndex = 0;  //��Ϣ��֡ƫ��λ
	uint16  TotalCount = 0;  //�ڵ�������
	uint8   ReplyCount = 0;  //����Ӧ��Ľڵ�����
	uint16  ReplyNodeIndex = 0;  //����Ӧ�����ʼ���
	uint16  NodeIndexTemp = 0;  //�ڵ����, ��ʱ����

	AppData_T   SendAppData;                        //����
    AppData_T   RecvAppData;                        //����
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));
	
	uint8           nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	INFO_DATA_T     InfoMsgData;  //�ظ���Ϣ
	memset(&InfoMsgData, 0, sizeof(InfoMsgData));

	switch (gTotalState[nTtlIndex].HplcQuery)
	{
		case HPLC_QUERY_IDLE://����ִ��,��ʼ��
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()��ѯ����ز�оƬ��Ϣ HPLC_QUERY_IDLE...�˿�[%d] \n", nTtlIndex);
			ClearCommRecvBuff(nTtlIndex);  //��ͨѶ���ջ�����
			ReadAmrInfo(nTtlIndex);  //���ص�����Ϣ��������Ϣ
			CurNodeIndex[nTtlIndex] = 1;//2013�浱ǰ������1��ʼ
			CommBeginTime[nTtlIndex] = time(NULL);
			OverTimeCnt[nTtlIndex] = 30;
			
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
			break;
		}
		case HPLC_QUERY_SEND: //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()��ѯ����ز�оƬ��Ϣ HPLC_QUERY_SEND...�˿�[%d] \n", nTtlIndex);

			//���ͱ�����֡
			SendAppData.AFN = 0x10;
		    SendAppData.Fn = 112;
		    SendAppData.DataUnitLen = 3;  //�ڵ���ʼ���2�ֽ�, �ڵ�����1�ֽ�
			memcpy(SendAppData.DataUnitBuff, &CurNodeIndex[nTtlIndex], 2);  //�ӽڵ���ʼ��ţ�2�ֽ�
			SendAppData.DataUnitBuff[2] = 10;  //ÿ�β�ѯ�ڵ�������1�ֽ�
			CCODOWN_FMT_DEBUG(nTtlIndex, "���ͱ���SendAppData �ڵ���ʼ���=%d, �ڵ�����=%d \n", CurNodeIndex[nTtlIndex], SendAppData.DataUnitBuff[2]);
			
			ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
			if(ret > 0)
			{
				//���ձ��Ľ�֡
				memcpy(&TotalCount, &RecvAppData.DataUnitBuff[0], 2);  //�ڵ�������
				memcpy(&ReplyNodeIndex, &RecvAppData.DataUnitBuff[2], 2);  //�ڵ���ʼ���
				ReplyCount = RecvAppData.DataUnitBuff[4];  //����Ӧ�������
				CCODOWN_FMT_DEBUG(nTtlIndex, "���ձ���RecvAppData �ڵ�������=%d, �ڵ���ʼ���=%d, ����Ӧ��ڵ�����=%d \n", 
					TotalCount, ReplyNodeIndex, ReplyCount);

				if(ReplyCount > 0)
				{
					//��װ��Ϣ����
					InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
					InfoMsgData.IP  = ntohl(InfoMsgData.IP);
					InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
					InfoMsgData.MsgPRIORITY = 0;
					InfoMsgData.MsgRPM = 1;
					InfoMsgData.MsgIndex = GetInfoIndex();
					InfoMsgData.MsgLabel = nTtlIndex;  //mqtt��Ŷ˿����(0��1)
					InfoMsgData.SourLen = strlen(gCcoRouterAppName);
					memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
					memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
					InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
					memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
					memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
					
					CurInfoIndex = 0;

					InfoMsgData.IID_ID = 0x0003;
					InfoMsgData.IOP_ID = 0x0051;
					
					Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurInfoIndex]);  //�˿ں�
					CCODOWN_FMT_DEBUG(nTtlIndex, "�˿ں�: %02x %02x %02x %02x \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
						InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3]);
					
					CurInfoIndex += 4;
					InfoMsgData.aPayloadData[CurInfoIndex] = 0x01;  //�Ƿ�����ʼ�����Ϣ, 0x00:��, 0x01:��
					CCODOWN_FMT_DEBUG(nTtlIndex, "�Ƿ�����ʼ�����Ϣ: %02x (0x00:��, 0x01:��) \n", InfoMsgData.aPayloadData[CurInfoIndex]);
					
					CurInfoIndex += 1;
					memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], (uint8 *)&ReplyNodeIndex, 2);  //��ʼ���2�ֽ�
					CCODOWN_FMT_DEBUG(nTtlIndex, "��ʼ���: %02x %02x (ʮ���� %d)\n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], ReplyNodeIndex);
					
					CurInfoIndex += 2;
					InfoMsgData.aPayloadData[CurInfoIndex] = 0x02;  //��ѯHPLC��Ϣchoice��־
					CCODOWN_FMT_DEBUG(nTtlIndex, "��ѯHPLC��Ϣchoice��־: %02x \n", InfoMsgData.aPayloadData[CurInfoIndex]);
					
					CurInfoIndex += 1;
					ChangeLenToBuff(ReplyCount, &CurInfoIndex, &InfoMsgData.aPayloadData[CurInfoIndex]);  //��Ϣ����seq of
					CCODOWN_FMT_DEBUG(nTtlIndex, "��Ϣ����seq of: %d(ʮ����) \n", ReplyCount);
					
					for(i=0; i<ReplyCount; i++)
					{
  						NodeIndexTemp = i + ReplyNodeIndex;
  						Uint16ToCharArr(NodeIndexTemp, &InfoMsgData.aPayloadData[CurInfoIndex]);  //�ڵ����2�ֽ�
  						CCODOWN_FMT_DEBUG(nTtlIndex, "i=%d, �ڵ����: %02x %02x (ʮ���� %d) \n", i, 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], NodeIndexTemp);
						
						CurInfoIndex += 2;
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[5+i*33], 6);  //�ڵ��ַ
						CCODOWN_FMT_DEBUG(nTtlIndex, "�ڵ��ַ: %02x %02x %02x %02x %02x %02x \n", 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
							InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3], 
							InfoMsgData.aPayloadData[CurInfoIndex+4], InfoMsgData.aPayloadData[CurInfoIndex+5]);
						
						CurInfoIndex += 6;
						InfoMsgData.aPayloadData[CurInfoIndex] = RecvAppData.DataUnitBuff[5+i*33+6];  //�ڵ��豸����
						CCODOWN_FMT_DEBUG(nTtlIndex, "�ڵ��豸����: %02x \n", 
							InfoMsgData.aPayloadData[CurInfoIndex]);
						
						CurInfoIndex += 1;
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[5+i*33+7], 24);  //�ڵ�оƬID��Ϣ
						CCODOWN_BUF_DEBUG(nTtlIndex, &InfoMsgData.aPayloadData[CurInfoIndex], 24, "�ڵ�оƬID��Ϣ:");
						
						CurInfoIndex += 24;
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[5+i*33+31], 2);  //�ڵ�оƬ����汾
						CCODOWN_FMT_DEBUG(nTtlIndex, "�ڵ�оƬ����汾: %02x %02x \n", 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1]);
						
						CurInfoIndex += 2;
					}
					InfoMsgData.nPayloadLength = CurInfoIndex;
					PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
				}

				CurNodeIndex[nTtlIndex] += ReplyCount;  //��ǰ����ۼ�
				if(CurNodeIndex[nTtlIndex] > TotalCount)
				{
					gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//�������
					break;
				}
			}

			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;  //�ж��Ƿ�ʱ
			break;
		}
		
		case HPLC_QUERY_RECV:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()��ѯ����ز�оƬ��Ϣ HPLC_QUERY_RECV...�˿�[%d] \n", nTtlIndex);
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) ��ʱ...\n");
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ۼƳ�ʱ, �������
			}
			else
			{
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
			}
			
			break;
		}

		case HPLC_QUERY_RETRY:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()��ѯ����ز�оƬ��Ϣ HPLC_QUERY_RETRY...�˿�[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ﵽ������Դ���, �������
			break;
		}

		case HPLC_QUERY_END:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()��ѯ����ز�оƬ��Ϣ HPLC_QUERY_END...�˿�[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
			gQueryTypeHPLC[nTtlIndex].BitType.chip = 0;
			return 1;
			break;
		}

		default:
			break;
	}

	return -1;
}


/*******************************************************************************
* ��������: HplcInfoReport_Topo
* ��������: HPLC��ѯ��Ϣ�ϱ� ����������Ϣ
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int HplcInfoReport_Topo(TASK_FUN_INPUT stTaskFunInput)
{
	int i   = 0;
	int ret = 0;
	static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //ͨѶ��ʼʱ��
	static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //��ʱʱ��
	static  uint16  CurNodeIndex[MAX_DOWN_THREAD_NUM] = {0};  //2013�浱ǰ������1��ʼ
	uint16  CurInfoIndex = 0;  //��Ϣ��֡ƫ��λ
	uint16  TotalCount = 0;  //�ڵ�������
	uint8   ReplyCount = 0;  //����Ӧ��Ľڵ�����
	uint16  ReplyNodeIndex = 0;  //����Ӧ�����ʼ���

	AppData_T   SendAppData;                        //����
    AppData_T   RecvAppData;                        //����
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));
	
	uint8           nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	INFO_DATA_T     InfoMsgData;  //�ظ���Ϣ
	memset(&InfoMsgData, 0, sizeof(InfoMsgData));

	switch (gTotalState[nTtlIndex].HplcQuery)
	{
		case HPLC_QUERY_IDLE://����ִ��,��ʼ��
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Topo()��ѯ����������Ϣ HPLC_QUERY_IDLE...�˿�[%d] \n", nTtlIndex);
			ClearCommRecvBuff(nTtlIndex);  //��ͨѶ���ջ�����
			ReadAmrInfo(nTtlIndex);  //���ص�����Ϣ��������Ϣ
			CurNodeIndex[nTtlIndex] = 1;//2013�浱ǰ������1��ʼ
			CommBeginTime[nTtlIndex] = time(NULL);
			OverTimeCnt[nTtlIndex] = 30;
			
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
			break;
		}
		case HPLC_QUERY_SEND: //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Topo()��ѯ����������Ϣ HPLC_QUERY_SEND...�˿�[%d] \n", nTtlIndex);

			//���ͱ�����֡
			SendAppData.AFN = 0x10;
		    SendAppData.Fn = 21;
		    SendAppData.DataUnitLen = 3;  //�ڵ���ʼ���2�ֽ�, �ڵ�����1�ֽ�
			memcpy(SendAppData.DataUnitBuff, &CurNodeIndex[nTtlIndex], 2);  //�ӽڵ���ʼ��ţ�2�ֽ�
			SendAppData.DataUnitBuff[2] = 10;  //ÿ�β�ѯ�ڵ�������1�ֽ�
			CCODOWN_FMT_DEBUG(nTtlIndex, "���ͱ���SendAppData �ڵ���ʼ���=%d, �ڵ�����=%d \n", CurNodeIndex[nTtlIndex], SendAppData.DataUnitBuff[2]);
			
			ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
			if(ret > 0)
			{
				//���ձ��Ľ�֡
				memcpy(&TotalCount, &RecvAppData.DataUnitBuff[0], 2);  //�ڵ�������
				memcpy(&ReplyNodeIndex, &RecvAppData.DataUnitBuff[2], 2);  //�ڵ���ʼ���
				ReplyCount = RecvAppData.DataUnitBuff[4];  //����Ӧ�������
				CCODOWN_FMT_DEBUG(nTtlIndex, "���ձ���RecvAppData �ڵ�������=%d, �ڵ���ʼ���=%d, ����Ӧ��ڵ�����=%d \n", 
					TotalCount, ReplyNodeIndex, ReplyCount);

				if(ReplyCount > 0)
				{
					//��װ��Ϣ����
					InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
					InfoMsgData.IP  = ntohl(InfoMsgData.IP);
					InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
					InfoMsgData.MsgPRIORITY = 0;
					InfoMsgData.MsgRPM = 1;
					InfoMsgData.MsgIndex = GetInfoIndex();
					InfoMsgData.MsgLabel = nTtlIndex;  //mqtt��Ŷ˿����(0��1)
					InfoMsgData.SourLen = strlen(gCcoRouterAppName);
					memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
					memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
					InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
					memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
					memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
					
					CurInfoIndex = 0;

					InfoMsgData.IID_ID = 0x0003;
					InfoMsgData.IOP_ID = 0x0051;
					
					Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurInfoIndex]);  //�˿ں�
					CCODOWN_FMT_DEBUG(nTtlIndex, "�˿ں�: %02x %02x %02x %02x \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
						InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3]);

					CurInfoIndex += 4;
					InfoMsgData.aPayloadData[CurInfoIndex] = 0x01;  //�Ƿ�����ʼ�����Ϣ, 0x00:��, 0x01:��
					CCODOWN_FMT_DEBUG(nTtlIndex, "�Ƿ�����ʼ�����Ϣ: %02x (0x00:��, 0x01:��) \n", InfoMsgData.aPayloadData[CurInfoIndex]);

					CurInfoIndex += 1;
					memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], (uint8 *)&ReplyNodeIndex, 2);  //��ʼ���2�ֽ�
					CCODOWN_FMT_DEBUG(nTtlIndex, "��ʼ���: %02x %02x (ʮ���� %d) \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], ReplyNodeIndex);

					CurInfoIndex += 2;
					InfoMsgData.aPayloadData[CurInfoIndex] = 0x01;  //��ѯHPLC��Ϣchoice��־
					CCODOWN_FMT_DEBUG(nTtlIndex, "��ѯHPLC��Ϣchoice��־: %02x \n", InfoMsgData.aPayloadData[CurInfoIndex]);

					CurInfoIndex += 1;
					ChangeLenToBuff(ReplyCount, &CurInfoIndex, &InfoMsgData.aPayloadData[CurInfoIndex]);  //��Ϣ����seq of
					CCODOWN_FMT_DEBUG(nTtlIndex, "��Ϣ����seq of: %d(ʮ����) \n", ReplyCount);
					
					for(i=0; i<ReplyCount; i++)
					{
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[5+i*11], 6);  //�ڵ��ַ
						CCODOWN_FMT_DEBUG(nTtlIndex, "i=%d, �ڵ��ַ: %02x %02x %02x %02x %02x %02x \n", i, 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
							InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3], 
							InfoMsgData.aPayloadData[CurInfoIndex+4], InfoMsgData.aPayloadData[CurInfoIndex+5]);
						
						CurInfoIndex += 6;

                        //�ڵ��ʶ 2�ֽ�
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[5+i*11+6], 2);  //�ڵ�������Ϣ
						CCODOWN_FMT_DEBUG(nTtlIndex, "i=%d, �ڵ��ʶ: %02x %02x \n", i, 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1]);
						
						CurInfoIndex += 2;

                        //����ڵ��ʶ 2�ֽ�
                        memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[5+i*11+6+2], 2);  //�ڵ�������Ϣ
						CCODOWN_FMT_DEBUG(nTtlIndex, "i=%d, ����ڵ��ʶ: %02x %02x \n", i, 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1]);
						
						CurInfoIndex += 2;

                        //�ڵ���Ϣ
                        memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[5+i*11+6+4], 1);  //�ڵ�������Ϣ
						CCODOWN_FMT_DEBUG(nTtlIndex, "i=%d, �ڵ���Ϣ: %02x \n", i, 
							InfoMsgData.aPayloadData[CurInfoIndex]);
						
						CurInfoIndex += 1;
					}
					InfoMsgData.nPayloadLength = CurInfoIndex;
					PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);

					CurNodeIndex[nTtlIndex] += ReplyCount;  //��ǰ����ۼ�
					if(CurNodeIndex[nTtlIndex] > TotalCount)
					{
						gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//�������
						break;
					}
				}
			}
			
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;  //�ж��Ƿ�ʱ
			break;
		}
		
		case HPLC_QUERY_RECV:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Topo()��ѯ����������Ϣ HPLC_QUERY_RECV...�˿�[%d] \n", nTtlIndex);
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) ��ʱ...\n");
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ۼƳ�ʱ, �������
			}
			else
			{
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
			}
			
			break;
		}

		case HPLC_QUERY_RETRY:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Topo()��ѯ����������Ϣ HPLC_QUERY_RETRY...�˿�[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ﵽ������Դ���, �������
			break;
		}

		case HPLC_QUERY_END:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Topo()��ѯ����������Ϣ HPLC_QUERY_END...�˿�[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
			gQueryTypeHPLC[nTtlIndex].BitType.topo = 0;
			return 1;
			break;
		}

		default:
			break;
	}

	return -1;
}


/*******************************************************************************
* ��������: HplcInfoReport
* ��������: HPLC��ѯ��Ϣ�ϱ�
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int HplcInfoReport(TASK_FUN_INPUT stTaskFunInput)
{
    int result = 1;
    
    uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
    nTtlIndex = stTaskFunInput.TaskArrIndex;
    
    CCODOWN_FMT_DEBUG(nTtlIndex, "int HplcInfoReport(TASK_FUN_INPUT stTaskFunInput)HPLC��ѯ��Ϣ�ϱ�...\n");

    if(gQueryTypeHPLC[nTtlIndex].BitType.topo == 1)
    {
        result = HplcInfoReport_Topo(stTaskFunInput);
    }
    else if(gQueryTypeHPLC[nTtlIndex].BitType.chip == 1)
    {
        result = HplcInfoReport_Chip(stTaskFunInput);
    }
    else if(gQueryTypeHPLC[nTtlIndex].BitType.nets == 1)
    {
        result = HplcInfoReport_Nets(stTaskFunInput);
    }
    else if(gQueryTypeHPLC[nTtlIndex].BitType.phase == 1)
    {
        result = HplcInfoReport_Phase(stTaskFunInput);
    }
    else if(gQueryTypeHPLC[nTtlIndex].BitType.scale == 1)
    {
        result = HplcInfoReport_Scale(stTaskFunInput);
    }
    else if(gQueryTypeHPLC[nTtlIndex].BitType.master == 1)
    {
        result = HplcInfoReport_Master(stTaskFunInput);
    }
    else if(gQueryTypeHPLC[nTtlIndex].BitType.slave == 1)
    {
        result = HplcInfoReport_Slave(stTaskFunInput);
    }
    else if(gQueryTypeHPLC[nTtlIndex].BitType.module == 1)
    {
        result = HplcInfoReport_Module(stTaskFunInput);
    }
    else if(gQueryTypeHPLC[nTtlIndex].BitType.nodeversion == 1)
    {
        result = HplcInfoReport_NodeVersion(stTaskFunInput);
    }
    else
    {
        result = 1;
    }

    return result;
    
}

/*******************************************************************************
* ��������: HplcFrSet
* ��������: HPLCƵ������
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int HplcFrSet(TASK_FUN_INPUT stTaskFunInput)
{
	AppData_T   SendAppData;                        //����
    AppData_T   RecvAppData;                        //����
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));

	uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	SendAppData.AFN = 0x05;
    SendAppData.Fn = 16;
    SendAppData.DataUnitLen = 1;
	SendAppData.DataUnitBuff[0] = gFrSetValue[nTtlIndex];

	if(CommFunDeal(SendAppData, &RecvAppData, nTtlIndex))
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}

/*******************************************************************************
* ��������: HplcFrQuery
* ��������: HPLCƵ���ѯ
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int HplcFrQuery(TASK_FUN_INPUT stTaskFunInput)
{
	int ret = 0;
	static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //ͨѶ��ʼʱ��
	static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //��ʱʱ��
	uint16  CurInfoIndex = 0;  //��Ϣ��֡ƫ��λ
	int     IsSendMqtt = 0;  //�Ƿ񽫲�ѯ����ϱ�mqtt

	AppData_T   SendAppData;                        //����
    AppData_T   RecvAppData;                        //����
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));
	
	uint8           nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	INFO_DATA_T     InfoMsgData;  //�ظ���Ϣ
	memset(&InfoMsgData, 0, sizeof(InfoMsgData));
	memcpy(&InfoMsgData, &stTaskFunInput.InfoDataHead, sizeof(INFO_DATA_HEAD_T));  //��ȡԭʼHEAD
	InfoMsgData.MsgRPM = 0;   //������־λ, �ظ���������

	IsSendMqtt = stTaskFunInput.InfoMsgBuff[0];  //0:����Ҫ�ϱ�; 1:��Ҫ�ϱ�mqtt

	switch (gTotalState[nTtlIndex].HplcQuery)
	{
		case HPLC_QUERY_IDLE://����ִ��,��ʼ��
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcFrQuery()��ѯHPLCƵ�� HPLC_QUERY_IDLE...�˿�[%d] \n", nTtlIndex);
			ClearCommRecvBuff(nTtlIndex);  //��ͨѶ���ջ�����
			ReadAmrInfo(nTtlIndex);  //���ص�����Ϣ��������Ϣ
			CommBeginTime[nTtlIndex] = time(NULL);
			OverTimeCnt[nTtlIndex] = 20;
			
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
			break;
		}
		case HPLC_QUERY_SEND: //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcFrQuery()��ѯHPLCƵ�� HPLC_QUERY_SEND...�˿�[%d] \n", nTtlIndex);

			SendAppData.AFN = 0x03;
		    SendAppData.Fn = 16;
		    SendAppData.DataUnitLen = 0;  //�����ݵ�Ԫ
			
			ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
			if(ret > 0)
			{
				//Ƶ�㸳ֵ
				gGuiShowMessage[nTtlIndex].nFrValue = RecvAppData.DataUnitBuff[0];
				CCODOWN_FMT_DEBUG(nTtlIndex, "HplcFrQuery()��ѯHPLCƵ�� nFrValue=%d \n", RecvAppData.DataUnitBuff[0]);
				
				if(IsSendMqtt > 0)
				{
					//��װ��Ϣ����
					CurInfoIndex = 0;

					InfoMsgData.IID_ID = 0x0003;
					InfoMsgData.IOP_ID = 0x0051;
					
					InfoMsgData.aPayloadData[CurInfoIndex] = RecvAppData.DataUnitBuff[0];  //HPLCƵ��ֵ
					CurInfoIndex += 1;
					InfoMsgData.nPayloadLength = CurInfoIndex;
					PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
				}
				else
				{
					ret = write_pdata_xram(gGuiInfoName[nTtlIndex], (char *)&gGuiShowMessage[nTtlIndex], 0, sizeof(gGuiShowMessage[nTtlIndex]));
					CCOUP_FMT_DEBUG("HplcFrQuery()��ѯHPLCƵ��(���ϱ�mqtt) ����ʾ�����Ϣд��˽���ļ�, ret=%d \n", ret);
					CCODOWN_FMT_DEBUG(nTtlIndex, "HplcFrQuery()��ѯHPLCƵ��(���ϱ�mqtt) ����...�˿�[%d] \n", nTtlIndex);
				}
				
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//�������
				break;
			}

			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;
			break;
		}
		
		case HPLC_QUERY_RECV:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcFrQuery()��ѯHPLCƵ�� HPLC_QUERY_RECV...�˿�[%d] \n", nTtlIndex);
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) ��ʱ...\n");
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ۼƳ�ʱ, �������
			}
			else
			{
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //������֡����
			}
			
			break;
		}

		case HPLC_QUERY_RETRY:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcFrQuery()��ѯHPLCƵ�� HPLC_QUERY_RETRY...�˿�[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //�ﵽ������Դ���, �������
			break;
		}

		case HPLC_QUERY_END:  //����
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcFrQuery()��ѯHPLCƵ�� HPLC_QUERY_END...�˿�[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
			return 1;
			break;
		}

		default:
			break;
	}

	return -1;
	
}


/*******************************************************************************
* ��������: HplcAreaIdentify
* ��������: HPLC̨��ʶ��
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int HplcAreaIdentify(TASK_FUN_INPUT stTaskFunInput)
{
	AppData_T   SendAppData;                        //����
    AppData_T   RecvAppData;                        //����
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));

	uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	SendAppData.AFN = 0x05;
    SendAppData.Fn = 6;
    SendAppData.DataUnitLen = 1;
	SendAppData.DataUnitBuff[0] = gAreaIdentifyValue[nTtlIndex];

	if(CommFunDeal(SendAppData, &RecvAppData, nTtlIndex))
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}

/*******************************************************************************
* ��������: HplcOfflineFeel
* ��������: HPLC������֪
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int HplcOfflineFeel(TASK_FUN_INPUT stTaskFunInput)
{
	return 1;
	
}

/*******************************************************************************
* ��������: HplcRefuseNodeReport
* ��������: HPLC�ܾ��ڵ��ϱ�
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int HplcRefuseNodeReport(TASK_FUN_INPUT stTaskFunInput)
{
	AppData_T   SendAppData;                        //����
    AppData_T   RecvAppData;                        //����
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));

	uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	SendAppData.AFN = 0x05;
    SendAppData.Fn = 200;
    SendAppData.DataUnitLen = 1;
	SendAppData.DataUnitBuff[0] = gRefuseNodeReportValue[nTtlIndex];

	if(CommFunDeal(SendAppData, &RecvAppData, nTtlIndex))
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}


/*******************************************************************************
* ��������:  CheckModuleSoftReset
* ��������:  ���ģ���Ƿ�λ
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
int8 CheckModuleSoftReset(int8 CheckRmt, uint8 nArrIndex)
{
    static int8 ModuleRmt[MAX_DOWN_THREAD_NUM] = {0};//0x01Ϊ������ѯ��ʽ 0x0Ϊ��λ��������ϱ�    

    //printx(" CheckModuleSoftReset(int8 CheckRmt)ģ�鳧����Ϣ�ϱ���ʽ : %d \n",ModuleRmt);
    //��ȡ�ϱ�AFN=0x03 F10��������ѯ���������ϱ���ʽ
    if(CheckRmt == -1)
        return ModuleRmt[nArrIndex];

    ModuleRmt[nArrIndex] = CheckRmt;
    return ModuleRmt[nArrIndex];
}

/*******************************************************************************
* ��������:	Comm_ConfirmAckTakeFrameSeq
* ��������:	ͨѶ:ȷ��֡Ӧ��
* �������:	��
* �������:	��
* �� �� ֵ: ��
*******************************************************************************/
void Comm_ConfirmAckTakeFrameSeq(AppData_T ModuleRequestAppData, uint8 nArrIndex)
{
    AppData_T AppData;
    memset(&AppData,0x0,sizeof(AppData));
    AppData.RInfo[5] = ModuleRequestAppData.RInfo[5]; //��ȡ֡����
    AppData.RInfo[1] = ModuleRequestAppData.RInfo[1]; //�ŵ���ʶ
    //common bug(2014-6-24-14-09-30)
    AppData.AFN = 0x00;
    AppData.Fn = 1;
    AppData.DataUnitLen = 6;
    AppData.DataUnitBuff[0] = 0xff;
    AppData.DataUnitBuff[1] = 0xff;
    AppData.DataUnitBuff[2] = 0xff;
    AppData.DataUnitBuff[3] = 0xff;

    AppData.DataUnitBuff[4] = 0x00;
    AppData.DataUnitBuff[5] = 0x00;


    PackFrameData(AppData, nArrIndex);
}


/*******************************************************************************
* ��������:	PackInfoMsgStruct
* ��������:	��װ��ϢMSG�ṹ��
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void PackInfoMsgStruct(uint16 IID_ID, uint16 IOP_ID, uint16 nBuffLen, uint8 *aBuff, INFO_DATA_T *InfoMsgData)
{
	uint16          IndexID = 0;  //ƫ��λ
	
	InfoMsgData->IID_ID = IID_ID;
	InfoMsgData->IOP_ID = IOP_ID;

	ChangeLenToBuff(nBuffLen, &IndexID, InfoMsgData->aPayloadData);  //octet-string�䳤																	
	memcpy(&InfoMsgData->aPayloadData[IndexID], aBuff, nBuffLen);  //payload
	IndexID += nBuffLen;
	InfoMsgData->nPayloadLength = IndexID; 

}

/*******************************************************************************
* ��������:	InfoSimpleAck
* ��������:	������Ϣ��BOOL�ظ�
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoSimpleAck(INFO_DATA_T InfoMsgData, BOOL bResult)
{
	memset(&InfoMsgData.aPayloadData[0], 0x00, InfoMsgData.nPayloadLength);
	InfoMsgData.MsgRPM = 0;
	InfoMsgData.nPayloadLength = 1;
	InfoMsgData.aPayloadData[0] = 0x00;
	if(bResult)
	{
		InfoMsgData.aPayloadData[0] = 0x01;
	}
	PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
}

/*******************************************************************************
* ��������:	InfoEmptyAck
* ��������:	������Ϣ�Ŀջظ�
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoEmptyAck(INFO_DATA_T InfoMsgData)
{
	memset(&InfoMsgData.aPayloadData[0], 0x00, InfoMsgData.nPayloadLength);
	InfoMsgData.MsgRPM = 0;
	InfoMsgData.nPayloadLength = 0;
	PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
}


/*******************************************************************************
* ��������:	InfoSetPortID
* ��������:	����˿�����
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoSetPortID(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoSetPortID()����˿�����...begin \n");
	
	uint32      nPortID = 0;  //�˿ں�
    uint16      IndexID = 0;  //ƫ��λ
    uint8       nOpType = 0;  //0:ɾ��, 1:����
    int i = 0;
	int ret = -1;
	int Ret = -1;

	//������ϢMSG
	IndexID = 0;
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);	//�˿ں�
	IndexID += 4;
	nOpType = InfoMsgData.aPayloadData[IndexID];  //0:ɾ��, 1:����

	CCOUP_FMT_DEBUG("InfoSetPortID()����˿����� �˿ں�nPortID=%u, ����nOpType=%d (0:ɾ��, 1:����) \n", nPortID, nOpType);

	for(i=0; i<MAX_ALLOWED_DOWN_THREAD_NUM; i++)
	{
		CCOUP_FMT_DEBUG("InfoSetPortID()����˿����� ����֮ǰ: i=%d, gPortID[i]=%u (0x%08x), bSetPortIDFlag[i]=%d \n", i, gPortID[i], gPortID[i], bSetPortIDFlag[i]);
	}

	if((nPortID != PORT_F2090201) && (nPortID != PORT_F2090202))
	{
		CCOUP_FMT_DEBUG("InfoSetPortID() if((nPortID != PORT_F2090201) && (nPortID != PORT_F2090202)) �˿ںŴ���! \n");

		//��Ϣ��֡������Ϣ����
		InfoSimpleAck(InfoMsgData, FALSE);

		return;
	}

	if(nOpType == 0) //ɾ��
	{
		for(i=0; i<MAX_ALLOWED_DOWN_THREAD_NUM; i++)
		{
			if( ((i==0) && (nPortID==PORT_F2090201))
				|| ((i==1) && (nPortID==PORT_F2090202)) )
			{
				ret = 1;
				gPortID[i] = 0;
				bSetPortIDFlag[i] = 0;
				CCOUP_FMT_DEBUG("InfoSetPortID()����˿����� ɾ��֮��: i=%d, gPortID[i]=%u (0x%08x), bSetPortIDFlag[i]=%d \n", i, gPortID[i], gPortID[i], bSetPortIDFlag[i]);

				Ret = write_pdata_xram(gPortIDName[i], (char *)&gPortID[i], 0, sizeof(gPortID[i]));
				CCOUP_FMT_DEBUG("InfoSetPortID()����˿����� ���˿ں�д��˽���ļ�, Ret=%d \n", Ret);
			}
		}
		if(ret < 0)
		{
			CCOUP_FMT_DEBUG("InfoSetPortID()����˿����� ɾ���˿ڲ���ʧ�� \n");
		}
	}
	else if(nOpType == 1) //����
	{
		for(i=0; i<MAX_ALLOWED_DOWN_THREAD_NUM; i++)
		{
			if( ((i==0) && (nPortID==PORT_F2090201))
				|| ((i==1) && (nPortID==PORT_F2090202)) )
			{
				ret = 1;
				gPortID[i] = nPortID;
				bSetPortIDFlag[i] = 1;
				CCOUP_FMT_DEBUG("InfoSetPortID()����˿����� ����֮��: i=%d, gPortID[i]=%u (0x%08x), bSetPortIDFlag[i]=%d \n", i, gPortID[i], gPortID[i], bSetPortIDFlag[i]);

				Ret = write_pdata_xram(gPortIDName[i], (char *)&gPortID[i], 0, sizeof(gPortID[i]));
				CCOUP_FMT_DEBUG("InfoSetPortID()����˿����� ���˿ں�д��˽���ļ�, Ret=%d \n", Ret);
			
				break;
			}
		}
		if(ret < 0)
		{
			CCOUP_FMT_DEBUG("InfoSetPortID()����˿����� ���Ӷ˿ڲ���ʧ��! \n");
		}
	}
	else //�쳣
	{
		CCOUP_FMT_DEBUG("InfoSetPortID()����˿����� �������ʹ���! \n");

		//��Ϣ��֡������Ϣ����
		InfoSimpleAck(InfoMsgData, FALSE);

		return;
	}

	//��Ϣ��֡������Ϣ����
	InfoSimpleAck(InfoMsgData, TRUE);
	
}

/*******************************************************************************
* ��������:	InfoAddMeterFiles
* ��������:	��ӱ���
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoAddMeterFiles(INFO_DATA_T InfoMsgData)
{
    CCOUP_FMT_DEBUG("InfoAddMeterFiles()��ӱ���...begin \n");
    
    uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
    int     i = 0;
    BOOL    bAddResult = FALSE;
    BOOL    bChanged[MAX_DOWN_THREAD_NUM] = {0};
    uint16  IndexID = 0;  //ƫ��λ
    int     ret = 0;

    //������ϢMSG
    uint16   nAddFilesNum = 0;  //��ӵĵ��ܱ�������
    BuffToChangeLen(&InfoMsgData.aPayloadData[IndexID], &IndexID, &nAddFilesNum);
    
    if(nAddFilesNum <= 0)
    {
        //��Ϣ��֡������Ϣ����
        InfoSimpleAck(InfoMsgData, FALSE);
        return;
    }
    AMR_METER_FILES_INFO_T aAddFilesData[nAddFilesNum];

    for(i=0; i<nAddFilesNum; i++)
    {
        CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &(aAddFilesData[i].nPortID));  //�˿ں�
        IndexID += 4;
        //CharArrToUint16(&InfoMsgData.aPayloadData[IndexID], &(aAddFilesData[i].stAmrMeterFiles.nMeterFilesID));  //�������
        //IndexID += 2;
        //IndexID += 1;  //TLV������������ַ��6�ֽ�<128����L��1�ֽڣ�
        memcpy_r(aAddFilesData[i].stAmrMeterFiles.aMeterFilesAddr, InfoMsgData.aPayloadData+IndexID, 6);  //������ַ
        IndexID += 6;
        //IndexID += 1;  //TLV����������Mac��6�ֽ�<128����L��1�ֽڣ�
        memcpy_r(aAddFilesData[i].stAmrMeterFiles.aMeterFilesMac, InfoMsgData.aPayloadData+IndexID, 6);  //����mac
        IndexID += 6;
        aAddFilesData[i].stAmrMeterFiles.nProtocolType = InfoMsgData.aPayloadData[IndexID];  //Э������
        IndexID += 1;
        //aAddFilesData[i].stAmrMeterFiles.nCommPara = InfoMsgData.aPayloadData[IndexID];  //ͨ�Ų���
        //IndexID += 1;       
    }

    //д���ڴ�
    for(i=0; i<nAddFilesNum; i++)
    {
        //��ӵ���
        CCOUP_FMT_DEBUG("InfoAddMeterFiles()gAmrMeterFilesListTemp[%d]����:%d...begin \n", nTtlIndex, ListLength_Files(gAmrMeterFilesListTemp[nTtlIndex]));  //���֮ǰ����Ŀ
        
        //�ж϶˿ں��Ƿ���Ч
        ret = PortIDToArrIndex(aAddFilesData[i].nPortID, &nTtlIndex);
        if(ret < 0)
        {
            break;
        }
        
        //�ж��Ƿ��Ѿ�������ͬ��ַ�ĵ���
        ret = LocateElemByPara_Files(gAmrMeterFilesListTemp[nTtlIndex], aAddFilesData[i], CMP_FILES_ADDR);
        if(ret > 0)  //����
        {
            bAddResult = UpdateByAddr_Files(gAmrMeterFilesListTemp[nTtlIndex], aAddFilesData[i]);
            //���µ���
            CCOUP_FMT_DEBUG("������ַ�Ѵ���, ֱ�Ӹ���. ���µĵ�����ϢΪ: \n");
            PrintFilesDetail(aAddFilesData[i], 0xaa);
            CCOUP_FMT_DEBUG("InfoAddMeterFiles()����֮�� gAmrMeterFilesListTemp[%d]����:%d...end \n", nTtlIndex, ListLength_Files(gAmrMeterFilesListTemp[nTtlIndex]));  //����֮�����Ŀ
        }
        else  //���
        {
            bAddResult = ListInsert_Files(&gAmrMeterFilesListTemp[nTtlIndex], gAmrMeterFilesListTemp[nTtlIndex].nMeterFilesNum+1, aAddFilesData[i]);
            //��ӵ���
            CCOUP_FMT_DEBUG("��ӵĵ�����ϢΪ: \n");
            PrintFilesDetail(aAddFilesData[i], 0xaa);
            CCOUP_FMT_DEBUG("InfoAddMeterFiles()���֮�� gAmrMeterFilesListTemp[%d]����:%d...end \n", nTtlIndex, ListLength_Files(gAmrMeterFilesListTemp[nTtlIndex]));  //���֮�����Ŀ
        }
        
        if(bAddResult)
        {
            bChanged[nTtlIndex] = TRUE;         
        }
        
    }

    //ˢ�±���
    for(nTtlIndex=0; nTtlIndex<MAX_DOWN_THREAD_NUM; nTtlIndex++)
    {
        if(bChanged[nTtlIndex])
        {
            gMeterFileAddDelFlagTemp[nTtlIndex] = 1;
        }       
    
        if(bChanged[nTtlIndex])
        {
            CCOUP_FMT_DEBUG("�˿�[%d] ȫ�ֵ����б� gAmrMeterFilesListTemp[%d]�仯 ˢ��һ��: \n", nTtlIndex, nTtlIndex);
            PrintFilesList(gAmrMeterFilesListTemp[nTtlIndex], 0xaa);

            //��������, ����˽���ļ�
            WriteFilesToPData(nTtlIndex);
        }       
    }

    //��Ϣ��֡������Ϣ����
    InfoSimpleAck(InfoMsgData, TRUE);

}

/*******************************************************************************
* ��������:	InfoDeleteMeterFiles
* ��������:	ɾ������
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
#define MAX_DELETE_NUM 200
void InfoDeleteMeterFiles(INFO_DATA_T InfoMsgData)
{
    CCOUP_FMT_DEBUG("InfoDeleteMeterFiles()ɾ������...begin \n");
    
    uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
    int     i = 0;
    BOOL    bDeleteResult = FALSE;
    BOOL    bChanged[MAX_DOWN_THREAD_NUM] = {0};
    uint8  aDeleteFilesData[MAX_DELETE_NUM][6];  //��ɾ���ĵ�����ַ
    uint16  IndexID = 0;  //ƫ��λ
    int     ret = 0;
    uint32  nPortID = 0;  //�˿ں�

    uint16  nFilesIndex = 0;//����λ�ã���1��ʼ

    //������ϢMSG
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //�˿ں�
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
    if(ret < 0)
    {
        //��Ϣ��֡������Ϣ����
        InfoSimpleAck(InfoMsgData, FALSE);
        return;
    }
    IndexID += 4;
    
    uint16   nDeleteFilesNum = 0;  //ɾ���ĵ��ܱ�������
    BuffToChangeLen(&InfoMsgData.aPayloadData[IndexID], &IndexID, &nDeleteFilesNum);
    if(nDeleteFilesNum > MAX_DELETE_NUM)
    {
        nDeleteFilesNum = MAX_DELETE_NUM;
    }
    CCOUP_FMT_DEBUG("InfoDeleteMeterFiles()ɾ������...ɾ������nDeleteFilesNum:%d \n", nDeleteFilesNum);
    
    for(i=0; i<nDeleteFilesNum; i++)
    {
        memcpy_r(aDeleteFilesData[i], &InfoMsgData.aPayloadData[IndexID], 6);
        IndexID += 2;
        CCOUP_FMT_DEBUG("InfoDeleteMeterFiles()ɾ������%d...ɾ���ĵ�����ַ%02X %02X %02X %02X %02X %02X\n",
                        i, aDeleteFilesData[i][5], aDeleteFilesData[i][4], aDeleteFilesData[i][3], aDeleteFilesData[i][2],
                        aDeleteFilesData[i][1], aDeleteFilesData[i][0]);
    }

    //д���ڴ�
    for(i=0; i<nDeleteFilesNum; i++)
    {
        nFilesIndex = GetIndexByAddr_Files(gAmrMeterFilesListTemp[nTtlIndex], aDeleteFilesData[i]);
        //printx(" i:%d, nTtlIndex:%d, nFilesIndex:%d \n", i, nTtlIndex, nFilesIndex);
        if(nFilesIndex > 0)
        {
            bDeleteResult = ListDeleteByIndex_Files(&gAmrMeterFilesListTemp[nTtlIndex], nFilesIndex);
            //printx(" i:%d, nTtlIndex:%d, nFilesIndex:%d bDeleteResult:%d \n", i, nTtlIndex, nFilesIndex, bDeleteResult);
            if(bDeleteResult)
            {
                bChanged[nTtlIndex] = TRUE;             

                CCOUP_FMT_DEBUG("InfoDeleteMeterFiles()ɾ������...ɾ���ĵ������ aDeleteFilesData[%d]:%d λ�ڶ˿�[%d] \n", i, aDeleteFilesData[i], nTtlIndex);
                CCOUP_FMT_DEBUG("InfoDeleteMeterFiles()ɾ������...ɾ��֮�� gAmrMeterFilesListTemp[%d]������nMeterFilesNum:%d \n", nTtlIndex, gAmrMeterFilesListTemp[nTtlIndex].nMeterFilesNum);
            }
        }
    }

    if(bChanged[nTtlIndex])
    {
        gMeterFileAddDelFlagTemp[nTtlIndex] = 1;
    }

    //ˢ�±���
    if(bChanged[nTtlIndex])
    {
        CCOUP_FMT_DEBUG("�˿�[%d] ȫ�ֵ����б� gAmrMeterFilesListTemp[%d]�仯 ˢ��һ��: \n", nTtlIndex, nTtlIndex);
        PrintFilesList(gAmrMeterFilesListTemp[nTtlIndex], 0xaa);

        //��������, ����˽���ļ�
        WriteFilesToPData(nTtlIndex);
    }
    
    //��Ϣ��֡������Ϣ����
    InfoSimpleAck(InfoMsgData, TRUE);
    
}

/*******************************************************************************
* ��������:	InfoClearMeterFiles
* ��������:	�������
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoClearMeterFiles(INFO_DATA_T InfoMsgData)
{
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	uint32  nPortID = 0;  //�˿ں�
	uint16  IndexID = 0;  //ƫ��λ
	int     ret = 0;
	BOOL    bChanged[MAX_DOWN_THREAD_NUM] = {0};

	//������ϢMSG
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //�˿ں�
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//��Ϣ��֡������Ϣ����
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}

	CCOUP_FMT_DEBUG("InfoClearMeterFiles()�������...�˿�:%d...begin \n", nTtlIndex);

	//д���ڴ�	
	ret = ClearList_Files(&gAmrMeterFilesListTemp[nTtlIndex]);
	if(ret)
	{
		bChanged[nTtlIndex] = TRUE;
	}

	if(bChanged[nTtlIndex])
	{
		gMeterFileClearFlagTemp[nTtlIndex] = 1;
	}

	//ˢ�±���
	if(bChanged[nTtlIndex])
	{
		CCOUP_FMT_DEBUG("�˿�[%d] ȫ�ֵ����б� gAmrMeterFilesListTemp[%d]�仯 ˢ��һ��: \n", nTtlIndex, nTtlIndex);
		PrintFilesList(gAmrMeterFilesListTemp[nTtlIndex], 0xaa);

		//��������, ����˽���ļ�
		WriteFilesToPData(nTtlIndex);
	}

	//��Ϣ��֡������Ϣ����
	InfoSimpleAck(InfoMsgData, TRUE);
	
}

/*******************************************************************************
* ��������:	InfoCurrentFilesNum
* ��������:	��ǰ��������������ĳ���˿ںţ�
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoCurrentFilesNum(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoCurrentFilesNum()��ǰ��������...begin \n");

	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	uint32  nPortID = 0;  //�˿ں�
	uint32  nFilesNum = 0;  //��������
	uint16  IndexID = 0;  //ƫ��λ
	int     ret = 0;

	INFO_DATA_T SendInfoMsgData;
	memset((uint8*)&SendInfoMsgData, 0x00, sizeof(SendInfoMsgData));

	//������ϢMSG
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //�˿ں�
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//��Ϣ��֡������Ϣ����
		InfoEmptyAck(InfoMsgData);
		return;
	}

	//���ڴ��л�ȡ��������
	nFilesNum = ListLength_Files(gAmrMeterFilesListTemp[nTtlIndex]);
	
	//��Ϣ��֡������Ϣ����
	IndexID = 0;
	memcpy(&SendInfoMsgData, &InfoMsgData, sizeof(MSG_INFO_T));
	SendInfoMsgData.MsgRPM = 0;
	
	Uint32ToCharArr(nFilesNum, &SendInfoMsgData.aPayloadData[IndexID]);
	IndexID = 4;
	SendInfoMsgData.nPayloadLength = IndexID;
	
    PlcWriteQueue(pAmrSendUp, (uint8 *)&SendInfoMsgData, sizeof(MSG_INFO_T)+SendInfoMsgData.nPayloadLength);
	
}

/*******************************************************************************
* ��������:	InfoQueryFilesDetail
* ��������:	��ѯ������Ϣ
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoQueryFilesDetail(INFO_DATA_T InfoMsgData)
{
    CCOUP_FMT_DEBUG("InfoQueryFilesDetail()��ѯ������Ϣ...begin \n");
    
    uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
    int     i = 0;
    uint32  nPortID = 0;  //�˿ں�
    uint16  nBeginFilesID = 0;  //���������ʼ����1��ʼ
    uint16  nFilesNum = 0;  //��ѯ������
    uint16  IndexID = 0;  //ƫ��λ
    uint16  nFactQueryNum = 0;
    int     ret = 0;
    
    AMR_METER_FILES_INFO_T stFilesInfo;
    memset((uint8 *)&stFilesInfo, 0x00, sizeof(stFilesInfo));

    INFO_DATA_T SendInfoMsgData;
    memset((uint8 *)&SendInfoMsgData, 0x00, sizeof(SendInfoMsgData));

    //������ϢMSG
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //�˿ں�
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
    if(ret < 0)
    {
        //��Ϣ��֡������Ϣ����
        InfoEmptyAck(InfoMsgData);
        return;
    }

    IndexID += 4;
    CharArrToUint16(&InfoMsgData.aPayloadData[IndexID], &nBeginFilesID);
    nBeginFilesID += 1; //���������ʼ����1��ʼ
    IndexID += 2;
    CharArrToUint16(&InfoMsgData.aPayloadData[IndexID], &nFilesNum);  //��ѯ������
    IndexID += 2;

    //��Ϣ��֡������Ϣ����
    IndexID = 0;
    memcpy(&SendInfoMsgData, &InfoMsgData, sizeof(MSG_INFO_T));
    SendInfoMsgData.MsgRPM = 0;

    //�жϲ�ѯ����ʼ��ź������Ƿ����
    if((nBeginFilesID <= 0) || (nBeginFilesID > gAmrMeterFilesListTemp[nTtlIndex].nMeterFilesNum))
    {
        nFactQueryNum = 0;
    }   
    else if((nBeginFilesID+nFilesNum-1) <= gAmrMeterFilesListTemp[nTtlIndex].nMeterFilesNum)
    {
        nFactQueryNum = nFilesNum;
    }
    else
    {
        nFactQueryNum = gAmrMeterFilesListTemp[nTtlIndex].nMeterFilesNum - nBeginFilesID + 1 ;
    }

    //ʵ���ܹ���ѯ�ĵ�������
    ChangeLenToBuff(nFactQueryNum, &IndexID, &SendInfoMsgData.aPayloadData[IndexID]);  //�����ĸ���
    for(i=0; i<nFactQueryNum; i++)
    {
        if(GetElem_Files(gAmrMeterFilesListTemp[nTtlIndex], nBeginFilesID+i, &stFilesInfo))
        {
            //Uint16ToCharArr(stFilesInfo.stAmrMeterFiles.nMeterFilesID, &SendInfoMsgData.aPayloadData[IndexID]);  //�������
            //IndexID += 2;
            //IndexID += 1;  //������ַ����
            memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], stFilesInfo.stAmrMeterFiles.aMeterFilesAddr, 6);  //������ַ
            IndexID += 6;
            //IndexID += 1;  //����mac����
            memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], stFilesInfo.stAmrMeterFiles.aMeterFilesMac, 6);  //����mac
            IndexID += 6;
            SendInfoMsgData.aPayloadData[IndexID] = stFilesInfo.stAmrMeterFiles.nProtocolType;  //Э������
            IndexID += 1;
            //SendInfoMsgData.aPayloadData[IndexID] = stFilesInfo.stAmrMeterFiles.nCommPara;  //ͨ�Ų���
            //IndexID += 1;
        }
        
    }
    SendInfoMsgData.nPayloadLength = IndexID;
    
    PlcWriteQueue(pAmrSendUp, (uint8 *)&SendInfoMsgData, sizeof(MSG_INFO_T)+SendInfoMsgData.nPayloadLength);
    
}

/*******************************************************************************
* ��������:	InfoRouterDeal
* ��������:	·�ɲ���
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoRouterDeal(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoRouterDeal()·�ɲ���...begin \n");
	
	uint32		nPortID = 0;  //�˿ں�
	uint16		IndexID = 0;  //ƫ��λ
	int 		ret 	= 0;
	uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

	//������ϢMSG
	IndexID = 0;
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);	//�˿ں�
	ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//��Ϣ��֡������Ϣ����
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}
	switch (InfoMsgData.IOP_ID)
	{
		case 0x0040://��ͣ����
		{
			gRoutrtCtrl[nTtlIndex] = RCS_PAUSE;
			break;
		}
		case 0x0041://�ָ�����
		{
			gRoutrtCtrl[nTtlIndex] = RCS_RESUME;
			break;
		}
		case 0x0042://��������
		{
			gRoutrtCtrl[nTtlIndex] = RCS_RESTART;
			break;
		}
		default:
			//��Ϣ��֡������Ϣ����
			InfoSimpleAck(InfoMsgData, FALSE);
			return;
			break;
	}

	//��Ϣ��֡������Ϣ����
	InfoSimpleAck(InfoMsgData, TRUE);

}



/*******************************************************************************
* ��������:	InfoActiveMeter
* ��������:	��������
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoActiveMeter(INFO_DATA_T InfoMsgData)
{   
    uint16  IndexID = 0;  //ƫ��λ
    uint32  nPortID = 0;  //�˿ں�
    uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
    int     ret = 0;
    AMR_MODE_ACTIVE_T AmrModeActive;
    memset(&AmrModeActive, 0x00, sizeof(AMR_MODE_ACTIVE_T));

    //������ϢMSG
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //�˿ں�
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
    if(ret < 0)
    {
        //��Ϣ��֡������Ϣ����
        InfoSimpleAck(InfoMsgData, FALSE);
        return;
    }

    IndexID = 0;
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &AmrModeActive.nPortID);  //�˿ں�
    IndexID += 4;
    //CharArrToUint16(&InfoMsgData.aPayloadData[IndexID], &AmrModeActive.nMeterFilesID);  //�������
    //IndexID += 2;
    AmrModeActive.comDcb.baud = InfoMsgData.aPayloadData[IndexID];  //������
    IndexID += 1;
    AmrModeActive.comDcb.ctrl = InfoMsgData.aPayloadData[IndexID];  //����λ
    IndexID += 1;
    //IndexID += 1;  //������ַ����
    memcpy_r(AmrModeActive.nMeterAddr, &InfoMsgData.aPayloadData[IndexID], 6);  //��Ƶ�ַ
    IndexID += 6;
    BuffToChangeLen(&InfoMsgData.aPayloadData[IndexID], &IndexID, &AmrModeActive.nDataBufLen);  //���Ŀɱ䳤��
    //�����ݴ�
    if(AmrModeActive.nDataBufLen > MAX_MSG_PAYLOAD_LEN)
    {
        CCOUP_FMT_DEBUG("InfoActiveMeter()�������� �˿�[%d] �����ĳ����쳣! AmrModeActive.nDataBufLen:%d, MAX_MSG_PAYLOAD_LEN=%d \n", 
            nTtlIndex, AmrModeActive.nDataBufLen, MAX_MSG_PAYLOAD_LEN);
        //��Ϣ��֡������Ϣ����
        InfoSimpleAck(InfoMsgData, FALSE);
        return;
    }
    memcpy(AmrModeActive.aDataBuf, &InfoMsgData.aPayloadData[IndexID], AmrModeActive.nDataBufLen);  //������
    
    ret = AddAmrIntoList(AmrModeActive, nTtlIndex);
    if(ret > 0)
    {
        CCOUP_FMT_DEBUG("InfoActiveMeter()�������� �˿�[%d] ������ϢΪ: \n", nTtlIndex);
        PrintAmrActiveDetail(AmrModeActive);
    }

    //��Ϣ��֡������Ϣ����
    InfoSimpleAck(InfoMsgData, TRUE);

    //���³���״̬�����һ�ν���ʱ��
    gAmrStateQuery[nTtlIndex].nLastRecvTime = time(NULL);
    
}

/*******************************************************************************
* ��������:	InfoSearchMeter
* ��������:	�����ѱ�
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoSearchMeter(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoSearchMeter()�����ѱ�...begin \n");
	
	uint16                    IndexID = 0;  //ƫ��λ
	uint32                    uint32_temp;
	int                       ret = 0;
	uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

	AMR_METER_START_SEARCH_T       AmrMeterStartSearch;            //�����ѱ�
	PLCTASK_DETAIL                 stPlcTaskDetail;                //�ز����������Ϣ
	memset(&AmrMeterStartSearch, 0, sizeof(AmrMeterStartSearch));
	memset(&stPlcTaskDetail, 0, sizeof(stPlcTaskDetail));

	//������ϢMSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &AmrMeterStartSearch.nPortID);
	ret = PortIDToArrIndex(AmrMeterStartSearch.nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//��Ϣ��֡������Ϣ����
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}
	IndexID += 4;
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &uint32_temp);
	AmrMeterStartSearch.nStartTime = (int32)uint32_temp;
    if(AmrMeterStartSearch.nStartTime >= (8*60*60))
    {
        AmrMeterStartSearch.nStartTime -= 8*60*60;//�ɼ�����ʱ��������ֶ���cst��0��, ͬ���޸�
    }
    else
    {
        AmrMeterStartSearch.nStartTime = 0;
    }
	IndexID += 4;
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &uint32_temp);
	AmrMeterStartSearch.nContinuedTime = (int32)uint32_temp;
	IndexID += 4;
	AmrMeterStartSearch.nSlaveNodeReSendCount = InfoMsgData.aPayloadData[IndexID];
	IndexID += 1;
	AmrMeterStartSearch.nRandWaitTimeSlice = InfoMsgData.aPayloadData[IndexID];
	IndexID += 1;

	PrintSearchMeterDetail(AmrMeterStartSearch);

	//��������
	gWhetherStartSearchMeter[nTtlIndex].bStartSearch = TRUE;
	gWhetherStartSearchMeter[nTtlIndex].stAmrMeterStartSearch = AmrMeterStartSearch;

	//��Ϣ��֡������Ϣ����
	InfoSimpleAck(InfoMsgData, TRUE);

}

/*******************************************************************************
* ��������:	InfoPauseSearchMeter
* ��������:	��ͣ�ѱ�
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoPauseSearchMeter(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoPauseSearchMeter()��ͣ�ѱ�...begin \n");
	
	int                       ret = 0;
	uint16                    IndexID = 0;  //ƫ��λ
	uint32 nPortID = 0;  //�˿ں�
	uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

	//������ϢMSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);
	ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//��Ϣ��֡������Ϣ����
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}
	IndexID += 4;

	//д���ڴ�
	gPauseSearchMeterFlag[nTtlIndex] = TRUE;

	//��Ϣ��֡������Ϣ����
	InfoSimpleAck(InfoMsgData, TRUE);
	
}


/*******************************************************************************
* ��������:	InfoDataTransmit
* ��������:	����͸��
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoDataTransmit(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoDataTransmit()����͸��...begin \n");
	
	uint16                    IndexID = 0;  //ƫ��λ
	AMR_DATA_TRANSMIT_T       AmrDataTransmit;            //����͸��
	uint8                     nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	int                       ret = 0;
	INFO_DATA_HEAD_T InfoDataHead;
	
	memset((uint8 *)&AmrDataTransmit, 0x00, sizeof(AmrDataTransmit));
	memcpy(&InfoDataHead, &InfoMsgData, sizeof(InfoDataHead));

	//������ϢMSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &AmrDataTransmit.nPortID);  //�˿ں�
	ret = PortIDToArrIndex(AmrDataTransmit.nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//��Ϣ��֡������Ϣ����
		InfoEmptyAck(InfoMsgData);
		return;
	}
	IndexID += 4;
	AmrDataTransmit.nCommSymbol = InfoMsgData.aPayloadData[IndexID];  //ͨѶ��
	IndexID += 2;
	CharArrToUint16(&InfoMsgData.aPayloadData[IndexID], &AmrDataTransmit.nFrameTimeover);  //֡��ʱʱ��
	IndexID += 2;
	CharArrToUint16(&InfoMsgData.aPayloadData[IndexID], &AmrDataTransmit.nCharTimeover);  //�ַ���ʱʱ��
	IndexID += 2;
	BuffToChangeLen(&InfoMsgData.aPayloadData[IndexID], &IndexID, &AmrDataTransmit.nTransmitDataLen);  //�ɱ䳤
	memcpy(AmrDataTransmit.pTransmitDataBuf, &InfoMsgData.aPayloadData[IndexID], AmrDataTransmit.nTransmitDataLen);  //����֡

	//��������
	AddDataTransmitList(nTtlIndex, AmrDataTransmit, InfoDataHead);
	
}

/*******************************************************************************
* ��������:	InfoGetSlaveMonitorDelayTime
* ��������:	��ȡ�ӽڵ�����ʱ
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoGetSlaveMonitorDelayTime(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoGetSlaveMonitorDelayTime()��ȡ�ӽڵ�����ʱ...begin \n");

	uint16                    IndexID = 0;  //ƫ��λ
	AMR_SLAVE_NODE_DELAY_TIME_T       AmrSlaveNodeMonitorTime;  //�ӽڵ���
	uint8                     nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	int                       ret = 0;
	INFO_DATA_HEAD_T InfoDataHead;
	
	memset((uint8 *)&AmrSlaveNodeMonitorTime, 0x00, sizeof(AMR_SLAVE_NODE_DELAY_TIME_T));
	memcpy(&InfoDataHead, &InfoMsgData, sizeof(INFO_DATA_HEAD_T));

	//������ϢMSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &AmrSlaveNodeMonitorTime.nPortID);  //�˿ں�
	ret = PortIDToArrIndex(AmrSlaveNodeMonitorTime.nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//��Ϣ��֡������Ϣ����
		InfoEmptyAck(InfoMsgData);
		return;
	}
	IndexID += 4;
	memcpy_r(AmrSlaveNodeMonitorTime.aSlaveNodeAddr, &InfoMsgData.aPayloadData[IndexID], 6);  //�ز��ӽڵ��ַ
	IndexID += 6;
	AmrSlaveNodeMonitorTime.nProtocolType = InfoMsgData.aPayloadData[IndexID];  //��Լ����
	IndexID += 1;
	BuffToChangeLen(&InfoMsgData.aPayloadData[IndexID], &IndexID, &AmrSlaveNodeMonitorTime.nDataBufLen);  //�ɱ䳤
	memcpy(AmrSlaveNodeMonitorTime.aDataBuf, &InfoMsgData.aPayloadData[IndexID], AmrSlaveNodeMonitorTime.nDataBufLen);  //����֡

	//��������
	AddSlaveMonitorTimeList(nTtlIndex, AmrSlaveNodeMonitorTime, InfoDataHead);
	
}


/*******************************************************************************
* ��������:	InfoSlaveMonitor
* ��������:	�ӽڵ���
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoSlaveMonitor(INFO_DATA_T InfoMsgData)
{
    CCOUP_FMT_DEBUG("InfoSlaveMonitor()�ӽڵ���...begin \n");
    
    uint16                    IndexID = 0;  //ƫ��λ
    AMR_SLAVE_NODE_MONITOR_T       AmrSlaveNodeMonitor;  //�ӽڵ���
    uint8                     nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
    int                       ret = 0;
    INFO_DATA_HEAD_T InfoDataHead;
    
    memset((uint8 *)&AmrSlaveNodeMonitor, 0x00, sizeof(AmrSlaveNodeMonitor));
    memcpy(&InfoDataHead, &InfoMsgData, sizeof(InfoDataHead));

    //������ϢMSG
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &AmrSlaveNodeMonitor.nPortID);  //�˿ں�
    ret = PortIDToArrIndex(AmrSlaveNodeMonitor.nPortID, &nTtlIndex);
    if(ret < 0)
    {
        //��Ϣ��֡������Ϣ����
        InfoEmptyAck(InfoMsgData);
        return;
    }
    IndexID += 4;
    AmrSlaveNodeMonitor.nProtocolType = InfoMsgData.aPayloadData[IndexID];  //��Լ����
    IndexID += 1;
    memcpy_r(AmrSlaveNodeMonitor.aSlaveNodeAddr, &InfoMsgData.aPayloadData[IndexID], 6);  //�ز��ӽڵ��ַ
    IndexID += 6;
    AmrSlaveNodeMonitor.comDcb.baud = InfoMsgData.aPayloadData[IndexID];  //������
    IndexID += 1;
    AmrSlaveNodeMonitor.comDcb.ctrl = InfoMsgData.aPayloadData[IndexID];  //����λ
    IndexID += 1;
    CharArrToUint16(&InfoMsgData.aPayloadData[IndexID], &AmrSlaveNodeMonitor.nFrameTimeover);  //֡��ʱʱ��
    IndexID += 2;
    CharArrToUint16(&InfoMsgData.aPayloadData[IndexID], &AmrSlaveNodeMonitor.nByteTimeover);  //�ַ���ʱʱ��
    IndexID += 2;
    BuffToChangeLen(&InfoMsgData.aPayloadData[IndexID], &IndexID, &AmrSlaveNodeMonitor.nDataBufLen);  //�ɱ䳤
    memcpy(AmrSlaveNodeMonitor.aDataBuf, &InfoMsgData.aPayloadData[IndexID], AmrSlaveNodeMonitor.nDataBufLen);  //����֡

    //��������
    AddSlaveMonitorList(nTtlIndex, AmrSlaveNodeMonitor, InfoDataHead);
    
}

/*******************************************************************************
* ��������:	InfoSetTmnAddr
* ��������:	ģ���ַ����
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoSetTmnAddr(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoSetTmnAddr()ģ���ַ����...begin \n");
	
	uint32      nPortID = 0;  //�˿ں�
    uint16      IndexID = 0;  //ƫ��λ
    int         ret     = 0;
	uint8       nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	uint8       aAddrTemp[6] = {0};  //���򱸷���Ϣ����ն˵�ַ

	//������ϢMSG
	IndexID = 0;
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);	//�˿ں�
	ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//��Ϣ��֡������Ϣ����
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}
	IndexID += 4;

	//������ϢMSG
	memcpy_r(aAddrTemp, &InfoMsgData.aPayloadData[IndexID], 6);  //���򱸷���Ϣ����ն˵�ַ
	ret = memcmp(&gTerminalAddr[nTtlIndex][0], aAddrTemp, 6);
	if(ret != 0)
	{
		gUpdateTerminalAddrFlag[nTtlIndex] = TRUE;
		memcpy(&gTerminalAddr[nTtlIndex][0], aAddrTemp, 6);  //ģ���ַ
		CCOUP_FMT_DEBUG("InfoSetTmnAddr()ģ���ַ����...�˿�[%d]ģ���ַ�仯! \n", nTtlIndex);

        //���ն˵�ַ������˽���ļ�
		ret = clear_pdata_xram(gModuleManuName[nTtlIndex]);
		CCODOWN_FMT_DEBUG(nTtlIndex, "gTerminalAddr() Ret = clear_pdata_xram() Ret:%d...\n", ret);
		
		ret = write_pdata_xram(gModuleManuName[nTtlIndex], (char *)&gTerminalAddr[nTtlIndex][0], 0, sizeof(gTerminalAddr[nTtlIndex]));
		CCODOWN_FMT_DEBUG(nTtlIndex, "gTerminalAddr() Ret = write_pdata_xram() Ret:%d...\n", ret);
	}
	IndexID += 6;

	//��Ϣ��֡������Ϣ����
	InfoSimpleAck(InfoMsgData, TRUE);
	
}


/*******************************************************************************
* ��������:	InfoGetBroadcastDelayTime
* ��������:	��ȡ�㲥��ʱ
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoGetBroadcastDelayTime(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoGetBroadcastDelayTime()��ȡ�㲥��ʱ...begin \n");
	
	uint16                    IndexID = 0;  //ƫ��λ
	//uint32                    uint32_temp;
	int                       ret = 0;
	uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

	AMR_GET_BROADCAST_DELAY_T       AmrGetBroadcastDelay;            //��ȡ�㲥��ʱ
	PLCTASK_DETAIL                  stPlcTaskDetail;                //�ز����������Ϣ
	memset(&AmrGetBroadcastDelay, 0, sizeof(AmrGetBroadcastDelay));
	memset(&stPlcTaskDetail, 0, sizeof(stPlcTaskDetail));

	//������ϢMSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &AmrGetBroadcastDelay.nPortID);  //�˿ں�
	ret = PortIDToArrIndex(AmrGetBroadcastDelay.nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//��Ϣ��֡������Ϣ����
		InfoEmptyAck(InfoMsgData);
		return;
	}
	IndexID += 4;
	AmrGetBroadcastDelay.nProtocolType = InfoMsgData.aPayloadData[IndexID];  //��Լ
	IndexID += 1;
	BuffToChangeLen(&InfoMsgData.aPayloadData[IndexID], &IndexID, &AmrGetBroadcastDelay.nDataBufLen);  //�ɱ䳤
	memcpy(AmrGetBroadcastDelay.aDataBuf, &InfoMsgData.aPayloadData[IndexID], AmrGetBroadcastDelay.nDataBufLen);  //����֡

	//��������
	gWhetherGetBroadcastDelay[nTtlIndex].bGetBroadcastDelay = TRUE;
	memcpy(&gWhetherGetBroadcastDelay[nTtlIndex].InfoDataHead, &InfoMsgData, sizeof(INFO_DATA_HEAD_T));
	gWhetherGetBroadcastDelay[nTtlIndex].stAmrGetBroadcastDelay = AmrGetBroadcastDelay;

	
}

/*******************************************************************************
* ��������:	InfoBroadcastDeal
* ��������:	�㲥
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoBroadcastDeal(INFO_DATA_T InfoMsgData)
{
    CCOUP_FMT_DEBUG("InfoBroadcastDeal()�㲥...begin \n");
    
    uint16                    IndexID = 0;  //ƫ��λ
    //uint32                    uint32_temp;
    int                       ret = 0;
    uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

    AMR_DO_BROADCAST_T       AmrDoBroadcast;            //�����㲥
    PLCTASK_DETAIL                  stPlcTaskDetail;                //�ز����������Ϣ
    memset(&AmrDoBroadcast, 0, sizeof(AmrDoBroadcast));
    memset(&stPlcTaskDetail, 0, sizeof(stPlcTaskDetail));

    //������ϢMSG
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &AmrDoBroadcast.nPortID);  //�˿ں�
    ret = PortIDToArrIndex(AmrDoBroadcast.nPortID, &nTtlIndex);
    if(ret < 0)
    {
        //��Ϣ��֡������Ϣ����
        InfoSimpleAck(InfoMsgData, FALSE);
        return;
    }
    IndexID += 4;
    AmrDoBroadcast.nProtocolType = InfoMsgData.aPayloadData[IndexID];  //��Լ
    IndexID += 1;
    AmrDoBroadcast.comDcb.baud = InfoMsgData.aPayloadData[IndexID];  //������
    IndexID += 1;
    AmrDoBroadcast.comDcb.ctrl = InfoMsgData.aPayloadData[IndexID];  //����λ
    IndexID += 1;
    BuffToChangeLen(&InfoMsgData.aPayloadData[IndexID], &IndexID, &AmrDoBroadcast.nDataBufLen);  //�ɱ䳤
    memcpy(AmrDoBroadcast.aDataBuf, &InfoMsgData.aPayloadData[IndexID], AmrDoBroadcast.nDataBufLen);  //����֡

    //��������
    gWhetherDoBroadcast[nTtlIndex].bDoBroadcast = TRUE;
    gWhetherDoBroadcast[nTtlIndex].stAmrDoBroadcast = AmrDoBroadcast;

    //��Ϣ��֡������Ϣ����
    InfoSimpleAck(InfoMsgData, TRUE);
    
}

/*******************************************************************************
* ��������:	InfoQueryHPLC
* ��������:	��ѯHPLC��Ϣ
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoQueryHPLC(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoQueryHPLC()��ѯHPLC��Ϣ...begin \n");
	
	uint16                    IndexID = 0;  //ƫ��λ
	int                       ret = 0;
	uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

	AMR_HPLC_INFO_QUERY_T       AmrHplcInfoQuery;            //��ѯHPLC��Ϣ
	memset(&AmrHplcInfoQuery, 0, sizeof(AMR_HPLC_INFO_QUERY_T));

	//������ϢMSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &AmrHplcInfoQuery.nPortID);  //�˿ں�
	ret = PortIDToArrIndex(AmrHplcInfoQuery.nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//��Ϣ��֡������Ϣ����
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}
	IndexID += 4;
	AmrHplcInfoQuery.nInfoType = InfoMsgData.aPayloadData[IndexID];  //��Ϣ����
	IndexID += 1;

	//�ж�hplc��ѯchoice�Ƿ�Ϸ�
	if((AmrHplcInfoQuery.nInfoType <= 0) ||	(AmrHplcInfoQuery.nInfoType >= HPLC_LAST))
	{
		CCOUP_FMT_DEBUG("AmrHplcInfoQuery.nInfoType error! AmrHplcInfoQuery.nInfoType=%d \n", AmrHplcInfoQuery.nInfoType);
		//��Ϣ��֡������Ϣ����
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}

	//��������
	switch(AmrHplcInfoQuery.nInfoType)
	{
		case HPLC_TOPOLOGYINFO:
		{
			gQueryTypeHPLC[nTtlIndex].BitType.topo = 1;
			break;
		}
		case HPLC_CHIPINFO:
		{
			gQueryTypeHPLC[nTtlIndex].BitType.chip = 1;
			break;
		}
		case HPLC_NETSINFO:
		{
			gQueryTypeHPLC[nTtlIndex].BitType.nets = 1;
			break;
		}
		case HPLC_PHASEINFO:
		{
			gQueryTypeHPLC[nTtlIndex].BitType.phase = 1;
			break;
		}
		case HPLC_SCALE:
		{
			gQueryTypeHPLC[nTtlIndex].BitType.scale = 1;
			break;
		}
		case HPLC_MASTERID:
		{
			gQueryTypeHPLC[nTtlIndex].BitType.master = 1;
			break;
		}
		case HPLC_SLAVEID:
		{
			gQueryTypeHPLC[nTtlIndex].BitType.slave = 1;
			break;
		}
		case HPLC_MODULEINFO:
		{
			gQueryTypeHPLC[nTtlIndex].BitType.module = 1;
			break;
		}
		case HPLC_NODEVERSION:
		{
			gQueryTypeHPLC[nTtlIndex].BitType.nodeversion = 1;
			break;
		}
		default:
		{
			CCOUP_FMT_DEBUG("AmrHplcInfoQuery.nInfoType error! AmrHplcInfoQuery.nInfoType=%d \n", AmrHplcInfoQuery.nInfoType);
			//��Ϣ��֡������Ϣ����
			InfoSimpleAck(InfoMsgData, FALSE);
			return;
		}
	}

	//��Ϣ��֡������Ϣ����
	InfoSimpleAck(InfoMsgData, TRUE);
	
}

/*******************************************************************************
* ��������:	InfoFrSet
* ��������:	HPLCƵ������
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoFrSet(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoFrSet()HPLCƵ������...begin \n");
	
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	uint32  nPortID = 0;  //�˿ں�
	uint16  IndexID = 0;  //ƫ��λ
	int     ret = 0;

	//������ϢMSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //�˿ں�
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//��Ϣ��֡������Ϣ����
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}

	IndexID += 4;
	gFrSetFlag[nTtlIndex] = 1;
	gFrSetValue[nTtlIndex] = InfoMsgData.aPayloadData[IndexID];

	//��Ϣ��֡������Ϣ����
	InfoSimpleAck(InfoMsgData, TRUE);
}

/*******************************************************************************
* ��������:	InfoFrQuery
* ��������:	HPLCƵ���ѯ
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoFrQuery(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoFrQuery()HPLCƵ���ѯ...begin \n");
	
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	uint32  nPortID = 0;  //�˿ں�
	uint16  IndexID = 0;  //ƫ��λ
	int     ret = 0;

	//������ϢMSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //�˿ں�
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//��Ϣ��֡������Ϣ����
		InfoEmptyAck(InfoMsgData);
		return;
	}

	gFrQueryFlag[nTtlIndex] = 1;  //���ò�ѯ��־
	memcpy(&gFrQueryInfoDataHead[nTtlIndex], &InfoMsgData, sizeof(INFO_DATA_HEAD_T));
}

/*******************************************************************************
* ��������:	InfoAreaIdentify
* ��������:	HPLC̨��ʶ��
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoAreaIdentify(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoAreaIdentify()HPLC̨��ʶ��...begin \n");
	
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	uint32  nPortID = 0;  //�˿ں�
	uint16  IndexID = 0;  //ƫ��λ
	int     ret = 0;

	//������ϢMSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //�˿ں�
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//��Ϣ��֡������Ϣ����
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}

	IndexID += 4;
	gAreaIdentifyFlag[nTtlIndex] = 1;
	gAreaIdentifyValue[nTtlIndex] = InfoMsgData.aPayloadData[IndexID];

	//��ʾ�ṹ�帳ֵ
	gGuiShowMessage[nTtlIndex].nAreaIdentifyState = InfoMsgData.aPayloadData[IndexID];

	ret = write_pdata_xram(gGuiInfoName[nTtlIndex], (char *)&gGuiShowMessage[nTtlIndex], 0, sizeof(gGuiShowMessage[nTtlIndex]));
	CCOUP_FMT_DEBUG("InfoAreaIdentify()HPLC̨��ʶ�� ����ʾ�����Ϣд��˽���ļ�, ret=%d \n", ret);

	//��Ϣ��֡������Ϣ����
	InfoSimpleAck(InfoMsgData, TRUE);
}

/*******************************************************************************
* ��������:	InfoOfflineFeel
* ��������:	HPLC������֪
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoOfflineFeel(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoOfflineFeel()HPLC������֪...begin \n");
	
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	uint32  nPortID = 0;  //�˿ں�
	uint16  IndexID = 0;  //ƫ��λ
	int     ret = 0;

	//������ϢMSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //�˿ں�
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//��Ϣ��֡������Ϣ����
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}

	IndexID += 4;
	gOfflineFeelFlag[nTtlIndex] = 1;
	gOfflineFeelValue[nTtlIndex] = InfoMsgData.aPayloadData[IndexID];

	//��ʾ�ṹ�帳ֵ
	gGuiShowMessage[nTtlIndex].nAreaIdentifyState = InfoMsgData.aPayloadData[IndexID];

	ret = write_pdata_xram(gGuiInfoName[nTtlIndex], (char *)&gGuiShowMessage[nTtlIndex], 0, sizeof(gGuiShowMessage[nTtlIndex]));
	CCOUP_FMT_DEBUG("InfoAreaIdentify()HPLC������֪ ����ʾ�����Ϣд��˽���ļ�, ret=%d \n", ret);

	//��Ϣ��֡������Ϣ����
	InfoSimpleAck(InfoMsgData, TRUE);
}

/*******************************************************************************
* ��������:	InfoRefuseNodeReport
* ��������:	HPLC�ܾ��ڵ��ϱ�
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoRefuseNodeReport(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoRefuseNodeReport()HPLC�ܾ��ڵ��ϱ�...begin \n");
	
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	uint32  nPortID = 0;  //�˿ں�
	uint16  IndexID = 0;  //ƫ��λ
	int     ret = 0;

	//������ϢMSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //�˿ں�
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//��Ϣ��֡������Ϣ����
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}

	IndexID += 4;
	gRefuseNodeReportFlag[nTtlIndex] = 1;
	gRefuseNodeReportValue[nTtlIndex] = InfoMsgData.aPayloadData[IndexID];

	//��ʾ�ṹ�帳ֵ
	gGuiShowMessage[nTtlIndex].nAreaIdentifyState = InfoMsgData.aPayloadData[IndexID];

	ret = write_pdata_xram(gGuiInfoName[nTtlIndex], (char *)&gGuiShowMessage[nTtlIndex], 0, sizeof(gGuiShowMessage[nTtlIndex]));
	CCOUP_FMT_DEBUG("InfoAreaIdentify()HPLC�ܾ��ڵ��ϱ� ����ʾ�����Ϣд��˽���ļ�, ret=%d \n", ret);

	//��Ϣ��֡������Ϣ����
	InfoSimpleAck(InfoMsgData, TRUE);
}


/*******************************************************************************
* ��������:	InfoAmrStateQuery
* ��������:	����״̬��ѯ
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoAmrStateQuery(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoAmrStateQuery()����״̬��ѯ...begin \n");

	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	uint32  nPortID = 0;  //�˿ں�
	uint16  IndexID = 0;  //ƫ��λ
	int     ret = 0;

	INFO_DATA_T SendInfoMsgData;
	memset((uint8*)&SendInfoMsgData, 0x00, sizeof(SendInfoMsgData));

	//������ϢMSG
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //�˿ں�
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//��Ϣ��֡������Ϣ����
		InfoEmptyAck(InfoMsgData);
		return;
	}

	//��Ϣ��֡������Ϣ����
	IndexID = 0;
	memcpy(&SendInfoMsgData, &InfoMsgData, sizeof(MSG_INFO_T));
	SendInfoMsgData.MsgRPM = 0;
	
	memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], (uint8 *)&gAmrStateQuery[nTtlIndex].nLastRecvTime, 4);//���һ�ν���ʱ��
	IndexID += 4;
	memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], (uint8 *)&gAmrStateQuery[nTtlIndex].nModuleAmrState, 4);//ģ�鳭��״̬
	IndexID += 4;
	memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], (uint8 *)&gAmrStateQuery[nTtlIndex].nModuleSearchState, 4);//ģ���ѱ�״̬
	IndexID += 4;
	memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], (uint8 *)&gAmrStateQuery[nTtlIndex].nAutoAmrState, 4);//�Զ�����״̬
	IndexID += 4;
	memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], (uint8 *)&gAmrStateQuery[nTtlIndex].nClearRouterTimes, 2);//���·�ɴ���
	IndexID += 2;
	memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], (uint8 *)&gAmrStateQuery[nTtlIndex].nDataInitTimes, 2);//���ݳ�ʼ������
	IndexID += 2;
	memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], (uint8 *)&gAmrStateQuery[nTtlIndex].nParaInitTimes, 2);//������ʼ������
	IndexID += 2;
	memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], (uint8 *)&gAmrStateQuery[nTtlIndex].nResetModuleTimes, 2);//��λģ�����
	IndexID += 2;
	SendInfoMsgData.nPayloadLength = IndexID;
	
    PlcWriteQueue(pAmrSendUp, (uint8 *)&SendInfoMsgData, sizeof(MSG_INFO_T)+SendInfoMsgData.nPayloadLength);

	
}

/*******************************************************************************
* ��������:	InfoAmrModuleQuery
* ��������:	��ѯ����ģ����Ϣ
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void InfoAmrModuleQuery(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoAmrModuleQuery()��ѯ����ģ����Ϣ...begin \n");

	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	uint32  nPortID = 0;  //�˿ں�
	uint16  IndexID = 0;  //ƫ��λ
	int     ret = 0;

	INFO_DATA_T SendInfoMsgData;
	memset((uint8*)&SendInfoMsgData, 0x00, sizeof(SendInfoMsgData));

	//������ϢMSG
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //�˿ں�
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//��Ϣ��֡������Ϣ����
		InfoEmptyAck(InfoMsgData);
		return;
	}

	//��Ϣ��֡������Ϣ����
	IndexID = 0;
	memcpy(&SendInfoMsgData, &InfoMsgData, sizeof(MSG_INFO_T));
	SendInfoMsgData.MsgRPM = 0;
	
	memcpy(&SendInfoMsgData.aPayloadData[IndexID], &gAmrModuleQuery[nTtlIndex].aLocalCommTypeModel[0], 6);//����ͨ��ģʽ��
	IndexID += 6;
	Uint16ToCharArr(gAmrModuleQuery[nTtlIndex].nSlaveMonitorMaxOvertime, &SendInfoMsgData.aPayloadData[IndexID]);//�ӽڵ������ʱʱ��(��λ:s)
	IndexID += 2;
	Uint16ToCharArr(gAmrModuleQuery[nTtlIndex].nBroadcastMaxOvertime, &SendInfoMsgData.aPayloadData[IndexID]);//�㲥�������ʱʱ��(��λ:s)
	IndexID += 2;
	Uint16ToCharArr(gAmrModuleQuery[nTtlIndex].nMaxAllowedFrameLen, &SendInfoMsgData.aPayloadData[IndexID]);//���֧�ֵı��ĳ���
	IndexID += 2;
	Uint16ToCharArr(gAmrModuleQuery[nTtlIndex].nMaxUpgradeSingleFrameLen, &SendInfoMsgData.aPayloadData[IndexID]);//�ļ�����֧�ֵ���󵥸����ݰ�����
	IndexID += 2;
	SendInfoMsgData.aPayloadData[IndexID] = gAmrModuleQuery[nTtlIndex].nUpgradeWaitingPeriod;//���������ȴ�ʱ��
	IndexID += 1;
	
	IndexID += 2;//���� �㲥�ɹ���ʱʱ��

	memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], &gTerminalAddr[nTtlIndex][0], 6);//�ն˵�ַ(ģ����)
	IndexID += 6;

	Uint16ToCharArr(gAmrModuleQuery[nTtlIndex].nMaxSupportSlaveNum, &SendInfoMsgData.aPayloadData[IndexID]);//֧�ֵ����ӽڵ�����
	IndexID += 2;

	Uint16ToCharArr(gAmrMeterFilesList[nTtlIndex].nMeterFilesNum, &SendInfoMsgData.aPayloadData[IndexID]);//��ǰ�ӽڵ�����
	IndexID += 2;

    //��װͨ��ģ�鳧�̴��뼰�汾��Ϣ(�ַ���)
    uint8 factLen = 16;
    char faccode[40] = {0};
    sprintf(faccode, "%C%C%C%C-%02X%02X%02X-%02X%02X", gAmrModuleQuery[nTtlIndex].InfoBuff[1], 
                                                   gAmrModuleQuery[nTtlIndex].InfoBuff[0], 
                                                   gAmrModuleQuery[nTtlIndex].InfoBuff[3], 
                                                   gAmrModuleQuery[nTtlIndex].InfoBuff[2], 
                                                   gAmrModuleQuery[nTtlIndex].InfoBuff[6], 
                                                   gAmrModuleQuery[nTtlIndex].InfoBuff[5], 
                                                   gAmrModuleQuery[nTtlIndex].InfoBuff[4], 
                                                   gAmrModuleQuery[nTtlIndex].InfoBuff[8], 
                                                   gAmrModuleQuery[nTtlIndex].InfoBuff[7]);

    CCOUP_FMT_DEBUG("InfoAmrModuleQuery() faccode=%s, strlen(faccode)=%d, factLen=%d \n", faccode, strlen(faccode), factLen);
    
    SendInfoMsgData.aPayloadData[IndexID] = factLen;//ͨ��ģ�鳧�̴��뼰�汾��Ϣ����(�ַ�����)
	IndexID += 1;

    memcpy(&SendInfoMsgData.aPayloadData[IndexID], (uint8 *)&faccode, factLen);//ͨ��ģ�鳧�̴��뼰�汾��Ϣ
	IndexID += factLen;
	
	SendInfoMsgData.nPayloadLength = IndexID;
	
    PlcWriteQueue(pAmrSendUp, (uint8 *)&SendInfoMsgData, sizeof(MSG_INFO_T)+SendInfoMsgData.nPayloadLength);
}

/*******************************************************************************
* ��������: cco_master_node_get
* ��������: ��ѯ���ڵ�
* �������: 
* �������: 
* �� �� ֵ: 
*******************************************************************************/
void cco_master_node_get(INFO_DATA_T *InfoMsgData)
{
    INFO_DATA_T SendInfoMsgData = {0};

    memcpy(&SendInfoMsgData, InfoMsgData, sizeof(MSG_INFO_T));
    SendInfoMsgData.MsgRPM = 0;
    memcpy_r(SendInfoMsgData.aPayloadData, gTerminalAddr[0], 6);
    SendInfoMsgData.nPayloadLength = 6;
    PlcWriteQueue(pAmrSendUp, (uint8 *)&SendInfoMsgData, sizeof(MSG_INFO_T) + SendInfoMsgData.nPayloadLength);
}

/*******************************************************************************
* ��������: cco_file_update_start
* ��������: ��������
* �������: 
* �������: 
* �� �� ֵ: 
*******************************************************************************/
void cco_file_update_start(INFO_DATA_T *InfoMsgData)
{
    CCOUP_FMT_DEBUG("cco_file_update_start��������...begin \n");
    
    uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

    uint32  nPortID = 0;  //�˿ں�

    uint16  IndexID = 0;  //ƫ��λ
    uint16 pathlen;
    int     ret = 0;

    if(gIsUpdating == 1)
    {
        CCOUP_FMT_DEBUG("cco_file_update_start ��ǰ���������� \n");
        InfoSimpleAck(*InfoMsgData, FALSE);
        return;
    }

    //������ϢMSG
    CharArrToUint32(&InfoMsgData->aPayloadData[IndexID], &nPortID);  //�˿ں�
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
    if(ret < 0)
    {
        //��Ϣ��֡������Ϣ����
        InfoSimpleAck(*InfoMsgData, FALSE);
        return;
    }
    IndexID += 4;
    //�ļ�·��
    BuffToChangeLen(&InfoMsgData->aPayloadData[IndexID], &IndexID, &pathlen);
    memcpy(gFilePath,&InfoMsgData->aPayloadData[IndexID],pathlen);
    IndexID+=pathlen;
    gFileFlag = InfoMsgData->aPayloadData[IndexID++];

    CCOUP_FMT_DEBUG("cco_file_update_start �ļ�·�� %s �ļ���ʶ %d   \n",gFilePath,gFileFlag);
    gIsUpdating = 1;
    gIsNeedUpdating =1;
    InfoSimpleAck(*InfoMsgData, TRUE);
}

/*******************************************************************************
* ��������: cco_update_state_get
* ��������: ��ѯ����״̬
* �������: 
* �������: 
* �� �� ֵ: 
*******************************************************************************/
void cco_update_state_get(INFO_DATA_T *InfoMsgData)
{
    CCOUP_FMT_DEBUG("cco_update_state_get()��ѯ����״̬ \n");

    uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
    uint32  nPortID = 0;  //�˿ں�
    uint16  IndexID = 0;  //ƫ��λ
    int     ret = 0;
    //uint8 payload[100]={0};
    INFO_DATA_T SendInfoMsgData = {0};
    uint16 index=0;

    //������ϢMSG
    CharArrToUint32(&InfoMsgData->aPayloadData[IndexID], &nPortID);  //�˿ں�
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
    if(ret < 0)
    {
        //��Ϣ��֡������Ϣ����
        InfoSimpleAck(*InfoMsgData, FALSE);
        return;
    }
    SendInfoMsgData.aPayloadData[index++] = gUpdateState;
    memcpy_r(&SendInfoMsgData.aPayloadData[index],&gUpdateBeginTime,4);
    index+=4;
    memcpy_r(&SendInfoMsgData.aPayloadData[index],&gUpdateEndTime,4);
    index+=4;
    if(gUpdateState !=2)
    {
        SendInfoMsgData.aPayloadData[index++] = 0;
    }else
    {
        SendInfoMsgData.aPayloadData[index++] = 1;
        SendInfoMsgData.aPayloadData[index++] = gUpdateResult;
    }
    //��Ϣ��֡������Ϣ����
    SendInfoMsgData.nPayloadLength = index;
    PlcWriteQueue(pAmrSendUp, (uint8 *)&SendInfoMsgData, sizeof(MSG_INFO_T) + SendInfoMsgData.nPayloadLength);
}


/*********************************
�������ܣ�    ȥ���ظ�������
����ԭ�ͣ�    Remove_RepeatPlcTask
����������
��������ֵ��
*********************************/
int8 Remove_RepeatPlcTask(PLCTASK_DETAIL      stPlcTaskDetail, uint8  nArrIndex)
{
	int IndexOfQueue = 0;

	//������������͸�����ӽڵ��ز��ж��ظ�����
	/*
	if(stPlcTaskDetail.TaskName == MS_NORMAL_AMR
		|| stPlcTaskDetail.TaskName == MS_RELAY_TRANS
		|| stPlcTaskDetail.TaskName == MS_SLAVE_MONITOR
		|| stPlcTaskDetail.TaskName == MS_GET_MANU_ID)
	{
		return IndexOfQueue;
	}
	*/

	//�����������Ƿ�����ظ�����
	IndexOfQueue = LocateElemSameNamePort_Task(PlcTaskQueue[nArrIndex], stPlcTaskDetail);
	return IndexOfQueue;
    
}

/*******************************************************************************
* ��������: PlcTask_AddQueue
* ��������: �����������
* �������:     
* �������:         
* �� �� ֵ:      
*******************************************************************************/
int PlcTask_AddQueue(PLCTASK_DETAIL      stPlcTaskDetail, uint8 nArrIndex)
{
	//printx(" PlcTask_AddQueue(PLCTASK_DETAIL stPlcTaskDetail)...begin \n");
	int i = 0;
	int IndexOfQueue;

	//����������Ϣ
	while(PlcTask_FunList[i].TaskName != 0xFF)
	{
		if(PlcTask_FunList[i].TaskName == stPlcTaskDetail.TaskName)
		{
			stPlcTaskDetail.TaskStat = PlcTask_FunList[i].TaskStat;
			stPlcTaskDetail.Priority = PlcTask_FunList[i].Priority;
			stPlcTaskDetail.DelaySec = PlcTask_FunList[i].DelaySec;
			stPlcTaskDetail.pPlcTaskFun = PlcTask_FunList[i].pPlcTaskFun;
			memset(stPlcTaskDetail.strTaskName, 0, sizeof(stPlcTaskDetail.strTaskName));
			strncpy(stPlcTaskDetail.strTaskName, (char *)PlcTask_FunList[i].strTaskName, strlen((char *)PlcTask_FunList[i].strTaskName));
			break;
		}
		i++;
	}

	//�����������Ƿ�����ظ�����
	IndexOfQueue = Remove_RepeatPlcTask(stPlcTaskDetail, nArrIndex);
	if(IndexOfQueue > 0)
	{
		return -1;
	}
	else
	{
		//�������
		//printx(" ******PlcTask_AddQueue()�������ظ�������, �����Ҫ��Ӹ�����****** \n");
		
		CCODOWN_FMT_DEBUG(nArrIndex, "PlcTask_AddQueue()���֮ǰ��������:%d...begin \n", ListLength_Task(PlcTaskQueue[nArrIndex]));  //���֮ǰ����Ŀ
		CCODOWN_FMT_DEBUG(nArrIndex, "��ӵ�������Ϣ: \n");
		PrintTaskDetail(stPlcTaskDetail, nArrIndex);
		
		int nNextPriorityID;  //�ȵ�ǰ��Ҫ��ӵ���������ȼ��ͣ�����Խ�����ȼ�Խ�ͣ�������ID��id��1��ʼ��
		nNextPriorityID = LocateElemPriorityLater_Task(PlcTaskQueue[nArrIndex], stPlcTaskDetail);
		ListInsert_Task(&PlcTaskQueue[nArrIndex], nNextPriorityID, stPlcTaskDetail);
		
		CCODOWN_FMT_DEBUG(nArrIndex, "PlcTask_AddQueue()���֮����������:%d...end \n", ListLength_Task(PlcTaskQueue[nArrIndex]));  //���֮�����Ŀ
		PrintTaskList(PlcTaskQueue[nArrIndex], nArrIndex);
	}

	//�ж��Ƿ���Ҫִ�и������ȼ�������
	if(stPlcTaskDetail.Priority < gPlcTaskDetail[nArrIndex].Priority)
	{
		switch (gPlcTaskDetail[nArrIndex].TaskName)
		{
			case MS_NORMAL_AMR:  //��ǰ��������ǳ���, �������ж�, ִ�и������ȼ�����
			{
				ClearAmrList(nArrIndex);  //��ճ������
				gTotalState[nArrIndex].NormalAmr = RTS_END;
			}
				break;

			default:
				break;
		}
	}

	return 1;
}

/*******************************************************************************
* ��������:	AddAmrIntoList
* ��������:	��ӳ�����Ϣ������
* �������:	��
* �������:	��
* �� �� ֵ: ��
*******************************************************************************/
int8 AddAmrIntoList(AMR_MODE_ACTIVE_T AmrModeActive, uint8 nArrIndex)
{
	int     result = 1;
	uint16  nAddIndex = 0;
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

	nTtlIndex = nArrIndex;
	
	//��������
    pthread_mutex_lock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);

	//��ӱ���
	if(gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum < MAX_ACTIVE_AMR_INFO_NUM)
	{
		CCOUP_FMT_DEBUG("AddAmrIntoList() ���֮ǰ...gActiveAmrInfoList[%d].nActiveAmrInfoNum = %d \n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);
		CCOUP_FMT_DEBUG("��ӵĳ�������Ϊ: \n");
		PrintAmrActiveDetail(AmrModeActive);
		
		nAddIndex = (gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoIndex + gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum) % MAX_ACTIVE_AMR_INFO_NUM;//ѭ��ȡ��ӵ�ַ�������±꣩				
		gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum ++;
		gActiveAmrInfoList[nTtlIndex].aActiveAmrInfoList[nAddIndex] = AmrModeActive;

		CCOUP_FMT_DEBUG("AddAmrIntoList() ���֮��...gActiveAmrInfoList[%d].nActiveAmrInfoNum = %d \n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);
	}
	else
	{
		CCOUP_FMT_DEBUG("AddAmrIntoList() gActiveAmrInfoList[%d].nActiveAmrInfoNum = %d ���ڵ��� MAX_ACTIVE_AMR_INFO_NUM ��ȴ�...\n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);
		result = -1;
	}

	//����
	pthread_mutex_unlock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);

	return result;
}

/*******************************************************************************
* ��������:	GetAmrFromList
* ��������:	�Ӷ���ȡ��������Ϣ
* �������:	��
* �������:	��
* �� �� ֵ: ��
*******************************************************************************/
int8 GetAmrFromList(AMR_MODE_ACTIVE_T *AmrModeActive, uint8 nArrIndex)
{
	int     result = 1;
	//uint16  nAddIndex = 0;
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

	nTtlIndex = nArrIndex;
	
	//��������
    pthread_mutex_lock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);
	
	//�ж���Ϣ�����Ƿ�Ϊ��
	if(gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum > 0)
	{	
		CCOUP_FMT_DEBUG("GetAmrFromList() ȡ��֮ǰ...gActiveAmrInfoList[%d].nActiveAmrInfoNum = %d \n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);
		CCOUP_FMT_DEBUG("ȡ���ĳ�������Ϊ: \n");
		PrintAmrActiveDetail(*AmrModeActive);
		
		//�����Ƚ��ȳ���˳��ȡ����Ϣ
		*AmrModeActive = gActiveAmrInfoList[nTtlIndex].aActiveAmrInfoList[gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoIndex];
		gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum --;
		gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoIndex = (gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoIndex+1) % MAX_ACTIVE_AMR_INFO_NUM;

		CCOUP_FMT_DEBUG("GetAmrFromList() ȡ��֮��...gActiveAmrInfoList[%d].nActiveAmrInfoNum = %d \n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);
	}
	else
	{
		CCOUP_FMT_DEBUG("GetAmrFromList() gActiveAmrInfoList[%d].nActiveAmrInfoNum = %d С�ڵ��� 0 ��ȴ�...\n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);
		result = -1;
	}

	//����
	pthread_mutex_unlock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);

	return result;
}

/*******************************************************************************
* ��������:	QueryAmrFromList
* ��������:	�Ӷ��в�ѯ������Ϣ��ֻ��ѯ����ɾ����
* �������:	��
* �������:	��
* �� �� ֵ: ��
*******************************************************************************/
int8 QueryAmrFromList(AMR_MODE_ACTIVE_T *AmrModeActive, uint8 nArrIndex)
{
	int     result = 1;
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

	nTtlIndex = nArrIndex;
	
	//��������
    pthread_mutex_lock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);
	
	//�ж���Ϣ�����Ƿ�Ϊ��
	if(gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum > 0)
	{	
		//�����Ƚ��ȳ���˳��ȡ����Ϣ
		*AmrModeActive = gActiveAmrInfoList[nTtlIndex].aActiveAmrInfoList[gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoIndex];

		//CCOUP_FMT_DEBUG("QueryAmrFromList() ��ѯ���ĳ�������Ϊ: \n");
		//PrintAmrActiveDetail(*AmrModeActive);
	}
	else
	{
		result = -1;
	}

	//����
	pthread_mutex_unlock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);

	return result;
}

/*******************************************************************************
* ��������:	DeleteAmrFromList
* ��������:	�Ӷ���ɾ����һ��������Ϣ
* �������:	��
* �������:	��
* �� �� ֵ: ��
*******************************************************************************/
int8 DeleteAmrFromList(uint8 nArrIndex)
{
	int     result = 1;
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

	nTtlIndex = nArrIndex;
	
	//��������
    pthread_mutex_lock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);
	
	//�ж���Ϣ�����Ƿ�Ϊ��
	if(gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum > 0)
	{	
		CCOUP_FMT_DEBUG("DeleteAmrFromList() ɾ��֮ǰ...gActiveAmrInfoList[%d].nActiveAmrInfoNum = %d \n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);		
		
		//�����Ƚ��ȳ���˳��ɾ����һ����Ϣ
		gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum --;
		gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoIndex = (gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoIndex+1) % MAX_ACTIVE_AMR_INFO_NUM;

		CCOUP_FMT_DEBUG("AddAmrIntoList() ɾ��֮��...gActiveAmrInfoList[%d].nActiveAmrInfoNum = %d \n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);
	}
	else
	{
		CCOUP_FMT_DEBUG("DeleteAmrFromList() gActiveAmrInfoList[%d].nActiveAmrInfoNum = %d С�ڵ��� 0 ��ȴ�...\n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);
		result = -1;
	}

	//����
	pthread_mutex_unlock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);

	return result;
}

/*******************************************************************************
* ��������:	ClearAmrList
* ��������:	��ճ������
* �������:	��
* �������:	��
* �� �� ֵ: ��
*******************************************************************************/
int8 ClearAmrList(uint8 nArrIndex)
{
	int     result = 1;
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

	nTtlIndex = nArrIndex;
	
	//��������
    pthread_mutex_lock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);

	//��ն���
	memset(&gActiveAmrInfoList[nTtlIndex], 0x00, sizeof(AMR_ACTIVE_INFO_LIST_T));
	CCOUP_FMT_DEBUG("ClearAmrList() ��ճ�����к�: gActiveAmrInfoList[%d].nActiveAmrInfoNum=%d...\n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);

	//����
	pthread_mutex_unlock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);

	return result;
}


/*******************************************************************************
* ��������:	GetAmrNumFromList
* ��������:	��ѯ������Ϣ����
* �������:	��
* �������:	��
* �� �� ֵ: ��
*******************************************************************************/
uint16 GetAmrNumFromList(uint8 nArrIndex)
{
	int     nAmrNum = 0;
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

	nTtlIndex = nArrIndex;
	
	//��������
    pthread_mutex_lock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);
	
	//��ֵ
	nAmrNum = gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum;

	//����
	pthread_mutex_unlock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);

	return nAmrNum;

}

/*******************************************************************************
* ��������:	AddDataTransmitList
* ��������:	���͸����Ϣ������
* �������:	��
* �������:	��
* �� �� ֵ: ��
*******************************************************************************/
int8 AddDataTransmitList(uint8 nArrIndex, AMR_DATA_TRANSMIT_T AmrDataTransmit, INFO_DATA_HEAD_T InfoDataHead)
{
	int     result = 1;
	uint16  nAddIndex = 0;
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

	nTtlIndex = nArrIndex;
	
	//��������
    pthread_mutex_lock(&gDataTransmitQueue[nTtlIndex].DataTransmitLock);

	//��ӱ���
	if(gDataTransmitQueue[nTtlIndex].nDataTransmitNum < MAX_DATA_TRANSMIT_INFO_NUM)
	{
		CCOUP_FMT_DEBUG("AddDataTransmitList() ���֮ǰ...gDataTransmitQueue[%d].nDataTransmitNum = %d \n", nTtlIndex, gDataTransmitQueue[nTtlIndex].nDataTransmitNum);
		
		nAddIndex = (gDataTransmitQueue[nTtlIndex].DataTransmitIndex + gDataTransmitQueue[nTtlIndex].nDataTransmitNum) % MAX_DATA_TRANSMIT_INFO_NUM;//ѭ��ȡ��ӵ�ַ�������±꣩				
		gDataTransmitQueue[nTtlIndex].nDataTransmitNum ++;
		gDataTransmitQueue[nTtlIndex].aDataTransmitList[nAddIndex] = AmrDataTransmit;
		gDataTransmitQueue[nTtlIndex].aInfoDataHead[nAddIndex] = InfoDataHead;

		CCOUP_FMT_DEBUG("AddDataTransmitList() ���֮��...gDataTransmitQueue[%d].nDataTransmitNum = %d \n", nTtlIndex, gDataTransmitQueue[nTtlIndex].nDataTransmitNum);
	}
	else
	{
		result = -1;
	}

	//����
	pthread_mutex_unlock(&gDataTransmitQueue[nTtlIndex].DataTransmitLock);

	return result;
}

/*******************************************************************************
* ��������:	GetDataTransmitFromList
* ��������:	�Ӷ���ȡ��͸����Ϣ
* �������:	��
* �������:	��
* �� �� ֵ: ��
*******************************************************************************/
int8 GetDataTransmitFromList(uint8 nArrIndex, AMR_DATA_TRANSMIT_T *AmrDataTransmit, INFO_DATA_HEAD_T *InfoDataHead)
{
	int     result = 1;
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

	nTtlIndex = nArrIndex;
	
	//��������
    pthread_mutex_lock(&gDataTransmitQueue[nTtlIndex].DataTransmitLock);
	
	//�ж���Ϣ�����Ƿ�Ϊ��
	if(gDataTransmitQueue[nTtlIndex].nDataTransmitNum > 0)
	{	
		CCOUP_FMT_DEBUG("GetDataTransmitFromList() ȡ��֮ǰ...gDataTransmitQueue[%d].nDataTransmitNum = %d \n", nTtlIndex, gDataTransmitQueue[nTtlIndex].nDataTransmitNum);
		
		//�����Ƚ��ȳ���˳��ȡ����Ϣ
		*AmrDataTransmit = gDataTransmitQueue[nTtlIndex].aDataTransmitList[gDataTransmitQueue[nTtlIndex].DataTransmitIndex];
		*InfoDataHead = gDataTransmitQueue[nTtlIndex].aInfoDataHead[gDataTransmitQueue[nTtlIndex].DataTransmitIndex];
		gDataTransmitQueue[nTtlIndex].nDataTransmitNum --;
		gDataTransmitQueue[nTtlIndex].DataTransmitIndex = (gDataTransmitQueue[nTtlIndex].DataTransmitIndex+1) % MAX_DATA_TRANSMIT_INFO_NUM;

		
		CCOUP_FMT_DEBUG("GetDataTransmitFromList() ȡ��֮ǰ...gDataTransmitQueue[%d].nDataTransmitNum = %d \n", nTtlIndex, gDataTransmitQueue[nTtlIndex].nDataTransmitNum);
	}
	else
	{
		result = -1;
	}

	//����
	pthread_mutex_unlock(&gDataTransmitQueue[nTtlIndex].DataTransmitLock);

	return result;
}


/*******************************************************************************
* ��������:	AddSlaveMonitorTimeList
* ��������:	��Ӵӽڵ�����ʱ��Ϣ������
* �������:	��
* �������:	��
* �� �� ֵ: ��
*******************************************************************************/
int8 AddSlaveMonitorTimeList(uint8 nArrIndex, AMR_SLAVE_NODE_DELAY_TIME_T AmrSlaveNodeMonitorTime, INFO_DATA_HEAD_T InfoDataHead)
{
	int     result = 1;
	uint16  nAddIndex = 0;
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

	nTtlIndex = nArrIndex;
	
	//��������
    pthread_mutex_lock(&gSlaveMonitorTimeQueue[nTtlIndex].SlaveMonitorTimeLock);

	//��ӱ���
	if(gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum < MAX_SLAVE_MONITOR_INFO_NUM)
	{
		CCOUP_FMT_DEBUG("AddSlaveMonitorTimeList() ���֮ǰ...gSlaveMonitorTimeQueue[%d].nSlaveMonitorTimeNum = %d \n", nTtlIndex, gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum);
		
		nAddIndex = (gSlaveMonitorTimeQueue[nTtlIndex].SlaveMonitorTimeIndex + gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum) % MAX_SLAVE_MONITOR_INFO_NUM;//ѭ��ȡ��ӵ�ַ�������±꣩
		gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum ++;
		gSlaveMonitorTimeQueue[nTtlIndex].aSlaveMonitorTimeList[nAddIndex] = AmrSlaveNodeMonitorTime;
		gSlaveMonitorTimeQueue[nTtlIndex].aInfoDataHead[nAddIndex] = InfoDataHead;

		CCOUP_FMT_DEBUG("AddSlaveMonitorTimeList() ���֮��...gSlaveMonitorTimeQueue[%d].nSlaveMonitorTimeNum = %d \n", nTtlIndex, gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum);
	}
	else
	{
		result = -1;
	}

	//����
	pthread_mutex_unlock(&gSlaveMonitorTimeQueue[nTtlIndex].SlaveMonitorTimeLock);

	return result;
}

/*******************************************************************************
* ��������:	GetSlaveMonitorTimeFromList
* ��������:	�Ӷ���ȡ���ӽڵ�����ʱ��Ϣ
* �������:	��
* �������:	��
* �� �� ֵ: ��
*******************************************************************************/
int8 GetSlaveMonitorTimeFromList(uint8 nArrIndex, AMR_SLAVE_NODE_DELAY_TIME_T *AmrSlaveNodeMonitor, INFO_DATA_HEAD_T *InfoDataHead)
{
	int     result = 1;
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

	nTtlIndex = nArrIndex;
	
	//��������
    pthread_mutex_lock(&gSlaveMonitorTimeQueue[nTtlIndex].SlaveMonitorTimeLock);
	
	//�ж���Ϣ�����Ƿ�Ϊ��
	if(gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum > 0)
	{	
		CCOUP_FMT_DEBUG("GetSlaveMonitorTimeFromList() ȡ��֮ǰ...gSlaveMonitorTimeQueue[%d].nSlaveMonitorTimeNum = %d \n", nTtlIndex, gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum);
		
		//�����Ƚ��ȳ���˳��ȡ����Ϣ
		*AmrSlaveNodeMonitor = gSlaveMonitorTimeQueue[nTtlIndex].aSlaveMonitorTimeList[gSlaveMonitorTimeQueue[nTtlIndex].SlaveMonitorTimeIndex];
		*InfoDataHead = gSlaveMonitorTimeQueue[nTtlIndex].aInfoDataHead[gSlaveMonitorTimeQueue[nTtlIndex].SlaveMonitorTimeIndex];
		gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum --;
		gSlaveMonitorTimeQueue[nTtlIndex].SlaveMonitorTimeIndex = (gSlaveMonitorTimeQueue[nTtlIndex].SlaveMonitorTimeIndex+1) % MAX_SLAVE_MONITOR_INFO_NUM;

		CCOUP_FMT_DEBUG("GetSlaveMonitorTimeFromList() ȡ��֮��...gSlaveMonitorTimeQueue[%d].nSlaveMonitorTimeNum = %d \n", nTtlIndex, gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum);
	}
	else
	{
		result = -1;
	}

	//����
	pthread_mutex_unlock(&gSlaveMonitorTimeQueue[nTtlIndex].SlaveMonitorTimeLock);

	return result;
}


/*******************************************************************************
* ��������:	AddSlaveMonitorList
* ��������:	��Ӵӽڵ�����Ϣ������
* �������:	��
* �������:	��
* �� �� ֵ: ��
*******************************************************************************/
int8 AddSlaveMonitorList(uint8 nArrIndex, AMR_SLAVE_NODE_MONITOR_T AmrSlaveNodeMonitor, INFO_DATA_HEAD_T InfoDataHead)
{
	int     result = 1;
	uint16  nAddIndex = 0;
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

	nTtlIndex = nArrIndex;
	
	//��������
    pthread_mutex_lock(&gSlaveMonitorQueue[nTtlIndex].SlaveMonitorLock);

	//��ӱ���
	if(gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum < MAX_SLAVE_MONITOR_INFO_NUM)
	{
		CCOUP_FMT_DEBUG("AddSlaveMonitorList() ���֮ǰ...gSlaveMonitorQueue[%d].nSlaveMonitorNum = %d \n", nTtlIndex, gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum);
		
		nAddIndex = (gSlaveMonitorQueue[nTtlIndex].SlaveMonitorIndex + gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum) % MAX_SLAVE_MONITOR_INFO_NUM;//ѭ��ȡ��ӵ�ַ�������±꣩
		gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum ++;
		gSlaveMonitorQueue[nTtlIndex].aSlaveMonitorList[nAddIndex] = AmrSlaveNodeMonitor;
		gSlaveMonitorQueue[nTtlIndex].aInfoDataHead[nAddIndex] = InfoDataHead;

		CCOUP_FMT_DEBUG("AddSlaveMonitorList() ���֮��...gSlaveMonitorQueue[%d].nSlaveMonitorNum = %d \n", nTtlIndex, gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum);
	}
	else
	{
		result = -1;
	}

	//����
	pthread_mutex_unlock(&gSlaveMonitorQueue[nTtlIndex].SlaveMonitorLock);

	return result;
}

/*******************************************************************************
* ��������:	GetSlaveMonitorFromList
* ��������:	�Ӷ���ȡ���ӽڵ�����Ϣ
* �������:	��
* �������:	��
* �� �� ֵ: ��
*******************************************************************************/
int8 GetSlaveMonitorFromList(uint8 nArrIndex, AMR_SLAVE_NODE_MONITOR_T *AmrSlaveNodeMonitor, INFO_DATA_HEAD_T *InfoDataHead)
{
	int     result = 1;
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

	nTtlIndex = nArrIndex;
	
	//��������
    pthread_mutex_lock(&gSlaveMonitorQueue[nTtlIndex].SlaveMonitorLock);
	
	//�ж���Ϣ�����Ƿ�Ϊ��
	if(gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum > 0)
	{	
		CCOUP_FMT_DEBUG("GetSlaveMonitorFromList() ȡ��֮ǰ...gSlaveMonitorQueue[%d].nSlaveMonitorNum = %d \n", nTtlIndex, gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum);
		
		//�����Ƚ��ȳ���˳��ȡ����Ϣ
		*AmrSlaveNodeMonitor = gSlaveMonitorQueue[nTtlIndex].aSlaveMonitorList[gSlaveMonitorQueue[nTtlIndex].SlaveMonitorIndex];
		*InfoDataHead = gSlaveMonitorQueue[nTtlIndex].aInfoDataHead[gSlaveMonitorQueue[nTtlIndex].SlaveMonitorIndex];
		gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum --;
		gSlaveMonitorQueue[nTtlIndex].SlaveMonitorIndex = (gSlaveMonitorQueue[nTtlIndex].SlaveMonitorIndex+1) % MAX_SLAVE_MONITOR_INFO_NUM;

		CCOUP_FMT_DEBUG("GetSlaveMonitorFromList() ȡ��֮��...gSlaveMonitorQueue[%d].nSlaveMonitorNum = %d \n", nTtlIndex, gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum);
	}
	else
	{
		result = -1;
	}

	//����
	pthread_mutex_unlock(&gSlaveMonitorQueue[nTtlIndex].SlaveMonitorLock);

	return result;
}



/*******************************************************************************
* ��������:	AddInfoDataIntoList
* ��������:	�����Ϣ������
* �������:	��Ϣmsg�ṹ��
* �������:	��
* �� �� ֵ: 
*******************************************************************************/
int8 AddInfoDataIntoList(INFO_DATA_T RecvInfoData)
{
	uint16 nAddIndex = 0;
	
	//���յ�������Ϣ�ݴ滺����
	if(gInfoDataList.InfoDataNum < MAX_INFO_DATA_NUM)
	{
		nAddIndex = (gInfoDataList.InfoDataReadIndex+gInfoDataList.InfoDataNum) % MAX_INFO_DATA_NUM;//ѭ��ȡ��ӵ�ַ�������±꣩
		CCOUP_FMT_DEBUG("AddMsgIntoList()���֮ǰ...stInfoMsgDataList.InfoMsgDataNum=[%d]...begin \n", gInfoDataList.InfoDataNum);
		
		gInfoDataList.InfoDataNum ++;
		gInfoDataList.InfoDataList[nAddIndex] = RecvInfoData;

		CCOUP_FMT_DEBUG("AddMsgIntoList()���֮��...stInfoMsgDataList.InfoMsgDataNum=[%d]...end \n", gInfoDataList.InfoDataNum);
	}
	else
	{
		CCOUP_FMT_DEBUG("CreateTaskByInfo()...stInfoMsgDataList.InfoMsgDataNum ���ڵ��� MAX_MSG_INFO_NUM����ȴ�...\n");
		return -1;
	}
	

	return 1;
}

/*******************************************************************************
* ��������:	GetInfoDataFromList
* ��������:	�Ӷ�����ȡ����Ϣ
* �������:	
* �������:	��Ϣmsg�ṹ��
* �� �� ֵ: 
*******************************************************************************/
int8 GetInfoDataFromList(INFO_DATA_T *SendInfoData)
{
	//�ж���Ϣ�����Ƿ�Ϊ��
	if(gInfoDataList.InfoDataNum > 0)
	{
		CCOUP_FMT_DEBUG("CreateTaskByInfo()ȡ��֮ǰ...�յ���Ϣ������=[%d]...begin \n", gInfoDataList.InfoDataNum);
		
		//�����Ƚ��ȳ���˳��ȡ����Ϣ
		*SendInfoData = gInfoDataList.InfoDataList[gInfoDataList.InfoDataReadIndex];
		gInfoDataList.InfoDataNum --;
		gInfoDataList.InfoDataReadIndex = (gInfoDataList.InfoDataReadIndex+1) % MAX_INFO_DATA_NUM;

		CCOUP_FMT_DEBUG("CreateTaskByInfo()ȡ��֮��...�յ���Ϣ������=[%d]...end \n", gInfoDataList.InfoDataNum);
	}
	else
	{
		return -1;
	}

	return 1;
}

/*******************************************************************************
* ��������:	CreateTaskByInfo
* ��������:	�����յ�����Ϣ��������
* �������:	��
* �������:	��
* �� �� ֵ: >0 �ɹ�
            =0
            <0 ʧ��
*******************************************************************************/
int8 CreateTaskByInfo(void)
{
    int nGetRet = 0;
    
    INFO_DATA_T stInfoData;  //��Ϣ
    memset(&stInfoData, 0x00, sizeof(stInfoData));
    
    nGetRet = GetInfoDataFromList(&stInfoData);
    if(nGetRet < 0)
    {
        return -1;
    }
        
    //ִ����Ϣ
    if(stInfoData.IID_ID != 0x0003)
    {
        return -1;
    }

    switch (stInfoData.IOP_ID)
    {
        case 0x0010://����˿�����
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()����˿����� \n");
            InfoSetPortID(stInfoData);
            break;
        }
        case 0x0020://��ӱ���
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()��ӱ��� \n");
            InfoAddMeterFiles(stInfoData);
            break;
        }
        case 0x0021://ɾ������
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()ɾ������ \n");
            InfoDeleteMeterFiles(stInfoData);
            break;
        }
        case 0x0022://�������
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()������� \n");
            InfoClearMeterFiles(stInfoData);
            break;
        }
        case 0x003D://��ǰ��������
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()��ǰ�������� \n");
            InfoCurrentFilesNum(stInfoData);
            break;
        }
        case 0x0043://��ѯ����
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()��ѯ���� \n");
            InfoQueryFilesDetail(stInfoData);
            break;
        }
        case 0x0040://��ͣ����
        case 0x0041://�ָ�����
        case 0x0042://��������
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()·�ɲ��� \n");
            InfoRouterDeal(stInfoData);
            break;
        }
        case 0x003E://��������
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()�������� \n");
            InfoActiveMeter(stInfoData);
            break;
        }
        case 0x0034://�����ѱ�
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()�����ѱ� \n");
            InfoSearchMeter(stInfoData);
            break;
        }
        case 0x0035://��ͣ�ѱ�
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()��ͣ�ѱ� \n");
            InfoPauseSearchMeter(stInfoData);
            break;
        }
        case 0x0033://����͸��
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()����͸�� \n");
            InfoDataTransmit(stInfoData);
            break;
        }
        case 0x0037://��ȡ�ӽڵ�����ʱ
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()��ȡ�ӽڵ�����ʱ \n");
            InfoGetSlaveMonitorDelayTime(stInfoData);
            break;
        }
        case 0x0038://�ӽڵ���
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()�ӽڵ��� \n");
            InfoSlaveMonitor(stInfoData);
            break;
        }
        case 0x003B://ģ���ַ����
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()ģ���ַ���� \n");
            InfoSetTmnAddr(stInfoData);
            break;
        }
        case 0x0039://��ȡ�㲥��ʱ
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()��ȡ�㲥��ʱ \n");
            InfoGetBroadcastDelayTime(stInfoData);
            break;
        }
        case 0x003A://�㲥
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()�㲥 \n");
            InfoBroadcastDeal(stInfoData);
            break;
        }
        case 0x0050://��ѯHPLC��Ϣ
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()��ѯHPLC��Ϣ \n");
            InfoQueryHPLC(stInfoData);
            break;
        }
        case 0x0052://HPLCƵ������
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()HPLCƵ������ \n");
            InfoFrSet(stInfoData);
            break;
        }
        case 0x0053://HPLCƵ���ѯ
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()HPLCƵ���ѯ \n");
            InfoFrQuery(stInfoData);
            break;
        }
        case 0x0054://HPLC̨��ʶ��
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()HPLC̨��ʶ�� \n");
            InfoAreaIdentify(stInfoData);
            break;
        }
        case 0x0055://HPLC������֪
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()HPLC������֪ \n");
            InfoOfflineFeel(stInfoData);
            break;
        }
        case 0x0056://HPLC�ܾ��ڵ��ϱ�
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()HPLC�ܾ��ڵ��ϱ� \n");
            InfoRefuseNodeReport(stInfoData);
            break;
        }
        case 0x0060://��ѯ����״̬
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()��ѯ����״̬ \n");
            InfoAmrStateQuery(stInfoData);
            break;
        }
        case 0x0061://��ѯ����ģ����Ϣ
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()��ѯ����ģ����Ϣ \n");
            InfoAmrModuleQuery(stInfoData);
            break;
        }
        case 0x0062://��ѯģ���ַ
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()��ѯ��ѯģ���ַ \n");
            cco_master_node_get(&stInfoData);
            break;
        }            
        case 0x0063://��������
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()�������� \n");
            cco_file_update_start(&stInfoData);
            break;
        }
        case 0x0064:
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()��ѯ����״̬ \n");
            cco_update_state_get(&stInfoData);
            break;
        }
        default:
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo() IOP���ʹ���! \n");
            return -1;
            break;
        } 
        
    }
    return 1;
}

/*******************************************************************************
* ��������: InstantReportTaskDeal
* ��������: ��ʱ�ϱ�������
* �������:
            uint8 *buf,
            int32 buf_len
* �������: ��
* �� �� ֵ: >0 �Ѵ���
            <0 �޶�Ӧ���
*******************************************************************************/
int InstantReportTaskDeal(uint8 *buf, int32 buf_len, uint8 nArrIndex)
{
    //CCODOWN_FMT_DEBUG(nArrIndex, "InstantReportTaskDeal()...start...buf_len=%d, nArrIndex=%d \n", buf_len, nArrIndex);
    AppData_T  AppData;
    AppData_T  SendAppData;
    uint16 IndexID = 0;  //ƫ��λ
    INFO_DATA_T SendInfoMsgData;
    

    memset(&AppData,0x00,sizeof(AppData));
    memset(&SendAppData,0x00,sizeof(SendAppData));
    memset(&SendInfoMsgData, 0x00, sizeof(INFO_DATA_T));

    uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
    nTtlIndex = nArrIndex;
    
    if(UnPackFrameData(buf_len, buf, &AppData) != CHECK_RESULT_SUCCEED)
    {
        CCODOWN_FMT_DEBUG(nTtlIndex, "InstantReportTaskDeal()...if(UnPackFrameData() != CHECK_RESULT_SUCCEED) \n");
        return -1;
    }

    if((AppData.AFN == 0x14) && (AppData.Fn == 1))//·����������, �ظ�·����ͣ
    {
        CCODOWN_FMT_DEBUG(nTtlIndex, "InstantReportTaskDeal()...·����������if((AppData.AFN == 0x14) && (AppData.Fn == 1))...\n");
        SendAppData.RInfo[5] =   AppData.RInfo[5];//֡����
        //printf("AppData.RInfo[5] = %02x\n", AppData.RInfo[5]);
        SendAppData.AFN = 0x12;
        SendAppData.Fn = 2;
        SendAppData.DataUnitLen = 0;
        PackFrameData(SendAppData, nArrIndex);
        return 1;
    }

    if((AppData.AFN == 0x14) && (AppData.Fn == 2))
    {
        CCODOWN_FMT_DEBUG(nTtlIndex, "InstantReportTaskDeal()...if((AppData.AFN == 0x14) && (AppData.Fn == 2))...\n");
        SendAppData.RInfo[5] =   AppData.RInfo[5];//֡����
        //printf("AppData.RInfo[5] = %02x\n", AppData.RInfo[5]);
        SendAppData.AFN = 0x14;
        SendAppData.Fn = 2;
        SendAppData.DataUnitLen = 6;
        SendAppData.DataUnitBuff[0] = gCurBcdTime[nArrIndex].Second;
        SendAppData.DataUnitBuff[1] = gCurBcdTime[nArrIndex].Minute;
        SendAppData.DataUnitBuff[2] = gCurBcdTime[nArrIndex].Hour;
        SendAppData.DataUnitBuff[3] = gCurBcdTime[nArrIndex].Day;
        SendAppData.DataUnitBuff[4] = gCurBcdTime[nArrIndex].Month;
        SendAppData.DataUnitBuff[5] = gCurBcdTime[nArrIndex].Year;
        PackFrameData(SendAppData, nArrIndex);
        return 2;
    }

    if((AppData.AFN == 0x03) && (AppData.Fn == 10))
    {
        CCODOWN_FMT_DEBUG(nTtlIndex, "InstantReportTaskDeal()...if((AppData.AFN == 0x03) && (AppData.Fn == 10))...\n");
        Comm_ConfirmAckTakeFrameSeq(AppData, nArrIndex);

        if(CheckModuleSoftReset(-1, nTtlIndex) == 0)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "CheckModuleSoftReset()==0, ·�������ϱ�ģ����Ϣ, ����֮ǰ�Ѿ��ϱ�, �˴��ж�ģ���쳣��λ, ������... \n");
            ClearCommQueueBuff(&gAmrRecvDownReportModule[nTtlIndex]);
            CCODOWN_FMT_DEBUG(nTtlIndex, "ģ���쳣��λִ�� �����ȶ����� �˿�:%d \n", nTtlIndex);
            gMeterFileCompareFlag[nTtlIndex] = 1;
            return 3;
        }
        else
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "CheckModuleSoftReset()==1, ��ǰ������ ������ȡ·��/�ȴ�·���ϱ� ״̬, ��Ҫ����... \n");

            if(0 == PlcWriteQueue(&gAmrRecvDownReportModule[nTtlIndex], buf, buf_len))
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "gAmrRecvDownReportModule����д��! ���! \n");
                ClearCommQueueBuff(&gAmrRecvDownReportModule[nTtlIndex]);
            }
        }
    }

    if((AppData.AFN == 0x06) && (AppData.Fn == 5))
    {
        CCODOWN_FMT_DEBUG(nTtlIndex, "InstantReportTaskDeal()...if((AppData.AFN == 0x06) && (AppData.Fn == 5))...\n");
        Comm_ConfirmAckTakeFrameSeq(AppData, nArrIndex);
        CCODOWN_FMT_DEBUG(nTtlIndex, "AFN=06H-F5���¼��ϱ�!\n");
        CCODOWN_FMT_DEBUG(nTtlIndex, "�ڵ��豸����:%02x(00H:�ɼ���, 01H:���ܱ�02��hplc)\n", AppData.DataUnitBuff[0]);
        CCODOWN_FMT_DEBUG(nTtlIndex, "ͨѶЭ������:%02x(00H:������01H:97��02:07��,03:698��04���ӽڵ�ͣ����)\n", AppData.DataUnitBuff[1]);
        CCODOWN_FMT_DEBUG(nTtlIndex, "����:%d \n", AppData.DataUnitBuff[2]);
        CCODOWN_FMT_DEBUG(nTtlIndex, "����: \n");
        CCODOWN_BUF_DEBUG(nTtlIndex, &(AppData.DataUnitBuff[3]), AppData.DataUnitBuff[2], "����");

        //�¼��ϱ�smiOS
        Uint32ToCharArr(gPortID[nArrIndex], &SendInfoMsgData.aPayloadData[IndexID]);  //�˿ں�
        IndexID += 4;
        SendInfoMsgData.aPayloadData[IndexID] = AppData.DataUnitBuff[0];  //�豸����
        IndexID += 1;
        SendInfoMsgData.aPayloadData[IndexID] = AppData.DataUnitBuff[1];  //Э�����ͣ�0��͸�����䣻1��DL/T 645-1997��2-DL/T 645-2007��3��DL/T 698.45�� 
        IndexID += 1;
        ChangeLenToBuff(AppData.DataUnitBuff[2], &IndexID, &SendInfoMsgData.aPayloadData[IndexID]);  //�ɱ䳤
        memcpy(&SendInfoMsgData.aPayloadData[IndexID], &AppData.DataUnitBuff[3], AppData.DataUnitBuff[2]);  //����֡
        IndexID += AppData.DataUnitBuff[2];
        SendInfoMsgData.aPayloadData[IndexID] = 0x00;  //PLC_READING_INFO OPTIONAL �Ƿ��г�����Ϣ��־, 0x00:��, 0x01:��
        IndexID += 1;       
        SendInfoMsgData.nPayloadLength = IndexID;  //payload����
        event_to_smiOS(SendInfoMsgData.aPayloadData, SendInfoMsgData.nPayloadLength);

        return 4;
    }

    if((AppData.AFN == 0x06) && (AppData.Fn == 6))
    {
        uint16 datalen = 0;
        CCODOWN_FMT_DEBUG(nTtlIndex, "InstantReportTaskDeal()...if((AppData.AFN == 0x06) && (AppData.Fn == 6))...\n");
        Comm_ConfirmAckTakeFrameSeq(AppData, nArrIndex);
        CCODOWN_FMT_DEBUG(nTtlIndex, "AFN=06H-F6���¼��ϱ�!\n");
        CCODOWN_FMT_DEBUG(nTtlIndex, "�ڵ��豸����:%02x(00H:˫������Դ, 01H:���ܱ�, 02H:hplc, 03H:խ���ز�, 04H:����ͨ��, 05H:˫ģͨ�ŵ�Ԫ, 06H��FFH:����)\n", AppData.DataUnitBuff[0]);
        CCODOWN_FMT_DEBUG(nTtlIndex, "ͨѶЭ������:%02x(00H:����, 01H:97��, 02:07��, 03:698��, 04H��FFH:����)\n", AppData.DataUnitBuff[1]);
        memcpy(&datalen, &AppData.DataUnitBuff[2], 2);
        CCODOWN_FMT_DEBUG(nTtlIndex, "����:%d \n", datalen);
        CCODOWN_FMT_DEBUG(nTtlIndex, "����: \n");
        CCODOWN_BUF_DEBUG(nTtlIndex, &(AppData.DataUnitBuff[4]), datalen, "����:");

        //�¼��ϱ�smiOS
        Uint32ToCharArr(gPortID[nArrIndex], &SendInfoMsgData.aPayloadData[IndexID]);  //�˿ں�
        IndexID += 4;
        SendInfoMsgData.aPayloadData[IndexID] = AppData.DataUnitBuff[0];  //�豸����
        IndexID += 1;
        SendInfoMsgData.aPayloadData[IndexID] = AppData.DataUnitBuff[1];  //Э�����ͣ�0��͸�����䣻1��DL/T 645-1997��2-DL/T 645-2007��3��DL/T 698.45�� 
        IndexID += 1;
        ChangeLenToBuff(datalen, &IndexID, &SendInfoMsgData.aPayloadData[IndexID]);  //�ɱ䳤
        memcpy(&SendInfoMsgData.aPayloadData[IndexID], &AppData.DataUnitBuff[4], datalen);  //����֡
        IndexID += datalen;
        SendInfoMsgData.aPayloadData[IndexID] = 0x00;  //PLC_READING_INFO OPTIONAL �Ƿ��г�����Ϣ��־, 0x00:��, 0x01:��
        IndexID += 1;       
        SendInfoMsgData.nPayloadLength = IndexID;  //payload����
        event_to_smiOS(SendInfoMsgData.aPayloadData, SendInfoMsgData.nPayloadLength);

        return 5;
    }

    if((AppData.AFN == 0x06) && (AppData.Fn == 10))
    {
        uint16 datalen = AppData.DataUnitLen;
        uint8  equiptype = 0x00;
        uint8  prototpye = 0x0a;
        CCODOWN_FMT_DEBUG(nTtlIndex, "InstantReportTaskDeal()...if((AppData.AFN == 0x06) && (AppData.Fn == 10))...\n");
        Comm_ConfirmAckTakeFrameSeq(AppData, nArrIndex);
        CCODOWN_FMT_DEBUG(nTtlIndex, "AFN=06H-F10���¼��ϱ�!\n");
        CCODOWN_FMT_DEBUG(nTtlIndex, "�ڵ��豸����:%02x(�Զ���)\n", equiptype);
        CCODOWN_FMT_DEBUG(nTtlIndex, "ͨѶЭ������:%02x(�Զ���)\n", prototpye);
        CCODOWN_FMT_DEBUG(nTtlIndex, "����:%d \n", datalen);
        CCODOWN_FMT_DEBUG(nTtlIndex, "����: \n");
        CCODOWN_BUF_DEBUG(nTtlIndex, &(AppData.DataUnitBuff[0]), datalen, "����:");

        //�¼��ϱ�smiOS
        Uint32ToCharArr(gPortID[nArrIndex], &SendInfoMsgData.aPayloadData[IndexID]);  //�˿ں�
        IndexID += 4;
        SendInfoMsgData.aPayloadData[IndexID] = equiptype;  //�豸����
        IndexID += 1;
        SendInfoMsgData.aPayloadData[IndexID] = prototpye;  //Э�����ͣ�0��͸�����䣻1��DL/T 645-1997��2-DL/T 645-2007��3��DL/T 698.45�� 
        IndexID += 1;
        ChangeLenToBuff(datalen, &IndexID, &SendInfoMsgData.aPayloadData[IndexID]);  //�ɱ䳤
        memcpy(&SendInfoMsgData.aPayloadData[IndexID], &AppData.DataUnitBuff[0], datalen);  //����֡
        IndexID += datalen;
        SendInfoMsgData.aPayloadData[IndexID] = 0x00;  //PLC_READING_INFO OPTIONAL �Ƿ��г�����Ϣ��־, 0x00:��, 0x01:��
        IndexID += 1;       
        SendInfoMsgData.nPayloadLength = IndexID;  //payload����
        event_to_smiOS(SendInfoMsgData.aPayloadData, SendInfoMsgData.nPayloadLength);

        return 6;
    }

    //�ѱ���ֱ���ϱ�; ��������ѱ�, �򽫴��ڱ��Ŀ���һ�����ѱ����
    if((AppData.AFN == 0x06) && (AppData.Fn == 1 || AppData.Fn == 4))
    {
        uint8           ReportInfo[500] = {0};              //�ϱ���Ϣ
        int             ReportCnt;                          //�ϱ�����
        uint8           RealPhase;                          //ʵ����λ
        uint8           AddrSearchReportType = 0;
        ReportCnt = ReportInfoDeal(ReportInfo, &RealPhase, &AddrSearchReportType, &AppData, nTtlIndex);
        if (ReportCnt > 0)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "InstantReportTaskDeal()�����ϱ��ѱ���, ���������:%d \n", ReportCnt);
            if(AddrSearchReportType == 1)
            {
                SaveReportInfo_F1(ReportCnt, ReportInfo, RealPhase, nTtlIndex);
            }
            else if(AddrSearchReportType == 4)
            {
                SaveReportInfo_F4(ReportCnt, ReportInfo, RealPhase, nTtlIndex);
            }
        }
        return 7;
    }
    else if(1 == gAmrStateQuery[nTtlIndex].nModuleSearchState)//��ǰ�������ѱ�״̬
    {
        if(0 == PlcWriteQueue(&gAmrRecvDownSearchMeter[nTtlIndex], buf, buf_len))
        {
            ClearCommQueueBuff(&gAmrRecvDownSearchMeter[nTtlIndex]);
        }
    }
    
    return -2;
}


/*******************************************************************************
* ��������:	AmrInfoInit
* ��������:	������Ϣ��ʼ��
* �������:	��
* �������:	��
* �� �� ֵ:	��
*******************************************************************************/
void AmrInfoInit(uint8 nArrIndex)
{
	CCODOWN_FMT_DEBUG(nArrIndex, "AmrInfoInit() begin...\n");
	
	int Ret = -1;

	//��˽���ļ����ݶ���������ֵ�������б�ȫ�ֱ���
	memset(&gAmrMeterFilesListTemp[nArrIndex], 0, sizeof(gAmrMeterFilesListTemp[nArrIndex]));
	ReadFilesFromPData(nArrIndex);

	//��ȫ�ֵ������������Ϣʹ�õ���ʱ����
	memset(&gAmrMeterFilesList[nArrIndex], 0, sizeof(gAmrMeterFilesList[nArrIndex]));
	memcpy(&gAmrMeterFilesList[nArrIndex], &gAmrMeterFilesListTemp[nArrIndex], sizeof(gAmrMeterFilesList[nArrIndex]));
	
	//�������б�ȫ����Ϣ��ֵ������ṹ��
	ReadAmrInfo(nArrIndex); 

	//��˽���ļ����ݶ���������ֵ���˿ں�ȫ������
	Ret = read_pdata_xram(gPortIDName[nArrIndex], (char *)&gPortID[nArrIndex], 0, sizeof(gPortID[nArrIndex]));
	CCODOWN_FMT_DEBUG(nArrIndex, "AmrInfoInit() ����˽���ļ��˿ں� Ret=%d...\n", Ret);
	if(Ret == 0)
	{	
		bSetPortIDFlag[nArrIndex] = 1;
		CCODOWN_FMT_DEBUG(nArrIndex, "AmrInfoInit()�˿ںż��سɹ�! i=%d, gPortID[i]=%d (0x%08x), bSetPortIDFlag[i]=%d \n", nArrIndex, gPortID[nArrIndex], gPortID[nArrIndex], bSetPortIDFlag[nArrIndex]);
	}

	//�ӱ���˽���ļ���ȡ�ն˵�ַ
	Ret = read_pdata_xram(gModuleManuName[nArrIndex], (char *)&gTerminalAddr[nArrIndex][0], 0, sizeof(gTerminalAddr[nArrIndex]));
	if(Ret < 0)
	{
		CCODOWN_FMT_DEBUG(nArrIndex, "AmrInfoInit() read_pdata_xram(gModuleManuName[%d] error! \n", nArrIndex);
	}
	else
	{
		CCODOWN_FMT_DEBUG(nArrIndex, "AmrInfoInit() ����˽���ļ���ȡ �ն˵�ַ: \n");
		CCODOWN_BUF_DEBUG(nArrIndex, &gTerminalAddr[nArrIndex][0], 6, "��ַ:");
	}

	//��˽���ļ����ݶ���������ֵ����ʾ���ȫ�ֱ���
	Ret = read_pdata_xram(gGuiInfoName[nArrIndex], (char *)&gGuiShowMessage[nArrIndex], 0, sizeof(gGuiShowMessage[nArrIndex]));
	CCODOWN_FMT_DEBUG(nArrIndex, "AmrInfoInit() ������ʾ�����Ϣ Ret=%d...\n", Ret);
	if(Ret == 0)
	{	
		CCODOWN_FMT_DEBUG(nArrIndex, "AmrInfoInit()��ʾ�����Ϣ���سɹ�! nFrValue=%d, nAreaIdentifyState=%d, nOfflineFeelState=%d, nRefuseReportState=%d \n", 
			gGuiShowMessage[nArrIndex].nFrValue, gGuiShowMessage[nArrIndex].nAreaIdentifyState, 
			gGuiShowMessage[nArrIndex].nOfflineFeelState, gGuiShowMessage[nArrIndex].nRefuseReportState);
	}

	CCODOWN_FMT_DEBUG(nArrIndex, "AmrInfoInit() end...\n");
}


/*******************************************************************************
* ��������: PastTimeDealInit
* ��������: ʱ��������ʼ��
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void PastTimeDealInit(uint8 nArrIndex)
{
    GetCurBinTime(&gCurBinTime[nArrIndex]);
    memcpy(&gPreBinTime[nArrIndex], &gCurBinTime[nArrIndex], sizeof(DateTime_T));
}

/*******************************************************************************
* ��������: AllStateInit
* ��������: ״̬�ֳ�ʼ��
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void AllStateInit(uint8 nArrIndex)
{
	CCODOWN_FMT_DEBUG(nArrIndex, "AllStateInit() begin...\n");

    memset(&gTotalState[nArrIndex].ModuleUpdateDeal, 0x00, sizeof(MODULE_UPDATE_DEAL_T));
	memset(&gTotalState[nArrIndex].MeterFileModify, 0x00, sizeof(METER_FILE_MODIFY_STATE_T));
    memset(&gTotalState[nArrIndex].MeterFileCmp, 0x00, sizeof(METER_FILE_CMP_STATE_T));
    memset(&gTotalState[nArrIndex].RelayTrans, 0x00, sizeof(RELAY_TRANS_STATE_T));
	memset(&gTotalState[nArrIndex].SlaveMonitor, 0x00, sizeof(RELAY_TRANS_STATE_T));
	memset(&gTotalState[nArrIndex].SlaveMonitorTime, 0x00, sizeof(RELAY_TRANS_STATE_T));
    memset(&gTotalState[nArrIndex].HourAmr, 0x00, sizeof(RELAY_TRANS_STATE_T));
    memset(&gTotalState[nArrIndex].NormalAmr, 0x00, sizeof(RELAY_TRANS_STATE_T));
	memset(&gTotalState[nArrIndex].HplcQuery, 0x00, sizeof(HPLC_QUERY_STATE_T));
    

	//ģ����Ϣ
	gModuleManuID[nArrIndex] = MMID_NULL;
	memset(&gLocalCommModuleInfo[nArrIndex], 0x00, sizeof(LOCAL_COMM_MODULE_RUNNING_PATTERN_INFO));
    
    //�ϵ�Ĭ��AFN=0x03,F10������ʽ�ϱ�Ϊ��ѯ
    CheckModuleSoftReset(1, nArrIndex);

	//������������
	InitList_Task(&PlcTaskQueue[nArrIndex]);  //�ز��������
	memset(&gPlcTaskDetail[nArrIndex], 0x00, sizeof(gPlcTaskDetail[nArrIndex]));  //��ǰ�ز���������ϸ��

	//����������г�ʼ��
	memset(&gActiveAmrInfoList[nArrIndex], 0, sizeof(gActiveAmrInfoList[nArrIndex]));
	if (pthread_mutex_init(&gActiveAmrInfoList[nArrIndex].ActiveAmrInfoLock, NULL) != 0)
    {
        CCODOWN_FMT_DEBUG(nArrIndex, "init gActiveAmrInfoList[%d].ActiveAmrInfoLock error! \n", nArrIndex);
    }

	//��͸������г�ʼ��
	memset(&gDataTransmitQueue[nArrIndex], 0, sizeof(gDataTransmitQueue[nArrIndex]));
	if (pthread_mutex_init(&gDataTransmitQueue[nArrIndex].DataTransmitLock, NULL) != 0)
    {
        CCODOWN_FMT_DEBUG(nArrIndex, "init gDataTransmitQueue[%d].DataTransmitLock error! \n", nArrIndex);
    }

    //�ӽڵ���������г�ʼ��
	memset(&gSlaveMonitorQueue[nArrIndex], 0, sizeof(gSlaveMonitorQueue[nArrIndex]));
	if (pthread_mutex_init(&gSlaveMonitorQueue[nArrIndex].SlaveMonitorLock, NULL) != 0)
    {
        CCODOWN_FMT_DEBUG(nArrIndex, "init gSlaveMonitorQueue[%d].SlaveMonitorLock error! \n", nArrIndex);
    }

	//��ǰ�ز���������ϸ��(�ѱ�)��ʼ��
	memset(&gPlcTaskDetailSearchMeter[nArrIndex], 0, sizeof(gPlcTaskDetailSearchMeter[nArrIndex]));

	//�ѱ��Ķ��г�ʼ��
	memset(&gAmrRecvDownSearchMeter[nArrIndex], 0, sizeof(gAmrRecvDownSearchMeter[nArrIndex]));
	if (pthread_mutex_init(&gAmrRecvDownSearchMeter[nArrIndex].QueueLock, NULL) != 0)
    {
        CCODOWN_FMT_DEBUG(nArrIndex, "init gAmrRecvDownSearchMeter[%d].QueueLock error! \n", nArrIndex);
    }

	//·�ɲ����ṹ���ʼ��
	memset(&gRouterOper[nArrIndex], 0, sizeof(gRouterOper[nArrIndex]));

	//�����ϱ�ģ����Ϣ���г�ʼ��
	memset(&gAmrRecvDownReportModule[nArrIndex], 0, sizeof(gAmrRecvDownReportModule[nArrIndex]));
	if (pthread_mutex_init(&gAmrRecvDownReportModule[nArrIndex].QueueLock, NULL) != 0)
    {
        CCODOWN_FMT_DEBUG(nArrIndex, "init gAmrRecvDownReportModule[%d].QueueLock error! \n", nArrIndex);
    }

	CCODOWN_FMT_DEBUG(nArrIndex, "AllStateInit() end...\n");
}


/*******************************************************************************
* ��������: AllFlagInit
* ��������: ��־��ʼ��
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void AllFlagInit(uint8 nArrIndex)
{
    gPowerOnFlag[nArrIndex] = TRUE;                        //���ϵ��־
}

/*******************************************************************************
* ��������:	ModuleCheck
* ��������:	�ز�ģ���Ƿ������
* �������:	
* �������:	
* �� �� ֵ:	
*******************************************************************************/
int ModuleCheck(uint8 index)
{
    int state = 0;

    if (gDevHalInfo[index].ctrlDev->local_cu_insert_state_get(gDevHalInfo[index].ctrlDev, &state) == 0)
    {
        return state;
    }

    return -1;
}

/*******************************************************************************
* ��������: PlcResetRouter
* ��������: ��λ�ز�ģ��������ѭ�����ȴ���ʱ���߳�����
* �������: fd                  �豸�ļ�������
* �������: ��
* �� �� ֵ:  >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int PlcResetRouter(uint8 index)
{
    if (gDevHalInfo[index].ctrlDev->local_cu_reset(gDevHalInfo[index].ctrlDev) == 0)
    {
        return 1;
    }

    return -1;
}

/*******************************************************************************
* ��������: TimeManage
* ��������: ʱ�������
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void TimeManage(uint8 nArrIndex)
{
    gPastTimeFlag[nArrIndex].Word = 0;                           //���ʱ���־

    GetCurBinTime(&gCurBinTime[nArrIndex]);                //ˢ�µ�ǰHEX��ʱ��
    GetCurBcdTime(&gCurBcdTime[nArrIndex]);              //ˢ�µ�ǰBCD��ʱ��


    //����
    if (gCurBinTime[nArrIndex].Month != gPreBinTime[nArrIndex].Month)
    {
        //ͬʱ�������¡����ա���ʱ�����֡������־
        gPastTimeFlag[nArrIndex].BitSect.Month = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Day = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Hour = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Minute = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Second = TRUE;
    }
    //����
    if (gCurBinTime[nArrIndex].Day != gPreBinTime[nArrIndex].Day)
    {
        //ͬʱ�������ա���ʱ�����ֱ�־
        gPastTimeFlag[nArrIndex].BitSect.Day = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Hour = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Minute = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Second = TRUE;
    }
    //��ʱ
    else if (gCurBinTime[nArrIndex].Hour != gPreBinTime[nArrIndex].Hour)
    {
        //ͬʱ������ʱ�����ֱ�־
        gPastTimeFlag[nArrIndex].BitSect.Hour = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Minute = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Second = TRUE;
    }
    //����
    else if (gCurBinTime[nArrIndex].Minute != gPreBinTime[nArrIndex].Minute)
    {
        //�������ֱ�־
        gPastTimeFlag[nArrIndex].BitSect.Minute = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Second = TRUE;
    }

    //����
    else if (gCurBinTime[nArrIndex].Second != gPreBinTime[nArrIndex].Second)
    {
        gPastTimeFlag[nArrIndex].BitSect.Second = TRUE;
    }
    else
    {
        usleep(10000);
    }

    memcpy(&gPreBinTime[nArrIndex], &gCurBinTime[nArrIndex], sizeof(DateTime_T));

}

/*******************************************************************************
* ��������: FillTaskRunInput
* ��������: ����������������
* �������: 
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int FillTaskRunInput(PLCTASK_DETAIL *pPlcTaskDetail)
{
	int ret = -1;
	int i = 0;
	
	//����������Ϣ
	while(PlcTask_FunList[i].TaskName != 0xFF)
	{
		if(PlcTask_FunList[i].TaskName == pPlcTaskDetail->TaskName)
		{
			pPlcTaskDetail->TaskStat = PlcTask_FunList[i].TaskStat;
			pPlcTaskDetail->Priority = PlcTask_FunList[i].Priority;
			pPlcTaskDetail->DelaySec = PlcTask_FunList[i].DelaySec;
			pPlcTaskDetail->pPlcTaskFun = PlcTask_FunList[i].pPlcTaskFun;
			memset(pPlcTaskDetail->strTaskName, 0, sizeof(pPlcTaskDetail->strTaskName));
			strncpy(pPlcTaskDetail->strTaskName, (char *)PlcTask_FunList[i].strTaskName, strlen((char *)PlcTask_FunList[i].strTaskName));
			ret = 1;
			break;
		}
		i++;
	}

	return ret;
}

/*******************************************************************************
* ��������: PlcTaskCreat
* ��������: ������в����ж�
* �������: 
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void PlcTaskCreat(uint8    nArrIndex)
{
	if((gDevHalInfo[nArrIndex].isvalid == 0) || (1 == gReloadDevFlag[nArrIndex]))
	{
		//CCODOWN_FMT_DEBUG(nArrIndex, "PlcTaskCreat() gDevHalInfo[nArrIndex].isvalid=%d, gReloadDevFlag[nArrIndex]=%d \n", gDevHalInfo[nArrIndex].isvalid, gReloadDevFlag[nArrIndex]);
		//sleep(5);
		return;  //ģ����Ϣ��Ч, ֱ���˳�
	}
	
	int     ret = 0;
	uint8   aEmptyAddr[6] = {0};  //�յ�ַ
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = nArrIndex;

	INFO_DATA_HEAD_T InfoDataHead;  //��ϢԭʼHEAD
	memset((uint8 *)&InfoDataHead, 0x00, sizeof(InfoDataHead));
	
	PLCTASK_DETAIL  stPlcTaskDetail;       //�ز����������Ϣ
	memset((uint8 *)&stPlcTaskDetail, 0x00, sizeof(stPlcTaskDetail));

	AMR_DATA_TRANSMIT_T       AmrDataTransmit;            //����͸��
	memset((uint8 *)&AmrDataTransmit, 0x00, sizeof(AmrDataTransmit));

	AMR_SLAVE_NODE_MONITOR_T  AmrSlaveNodeMonitor;            //�ӽڵ���
	memset((uint8 *)&AmrSlaveNodeMonitor, 0x00, sizeof(AmrSlaveNodeMonitor));

	//�жϵ��ȹ���app�Ƿ��·��ն˵�ַ
	ret = memcmp(aEmptyAddr, &gTerminalAddr[nTtlIndex][0], 6);
	if(!ret)
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()��ȡ�ն˵�ַʧ�� �˿�:%d \n", nTtlIndex);
		sleep(5);
		return;
	}

    //����ģ���ȡ�ɹ�����ִ����������
    if ((gModuleManuID[nTtlIndex]==MMID_NULL) && (gPlcTaskDetail[nTtlIndex].TaskName != MS_GET_MANU_ID))
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()�ϵ�/ģ���λ�ȡ���̱�ʶ �˿�:%d \n", nTtlIndex);

		//��������
		stPlcTaskDetail.TaskName = MS_GET_MANU_ID;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
        stPlcTaskDetail.InfoMsgBuff[0] = 1; //0:����Ҫ�ȴ��ϱ�; 1:��Ҫ�ȴ��ϱ�ģ����Ϣ
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex);		//�ϵ��ȡ���̱�ʶ       
	}

	if (gModuleManuID[nTtlIndex]==MMID_NULL)
	{
		return;
	}

	//�ز�����������ж�
    if(MUD_IDLE == gTotalState[nTtlIndex].ModuleUpdateDeal)
    {
        if(gIsNeedUpdating == 1)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()�����ز��������� �˿�:%d \n", nTtlIndex);
            gIsNeedUpdating = 0;
            stPlcTaskDetail.TaskName = MS_UPDATE_DEAL;
    	    stPlcTaskDetail.RoutrtCtrl = RCS_PAUSE;
    		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
    		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 
        }
    }

    //����ģ���ַ��������
	if(gUpdateTerminalAddrFlag[nTtlIndex])
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()�ն˵�ַ�仯, ���ɻ�ȡ���̱�ʶ �˿�:%d \n", nTtlIndex);
		gUpdateTerminalAddrFlag[nTtlIndex] = FALSE;

		//�������� ���»�ȡ���̱�ʶ
		stPlcTaskDetail.TaskName = MS_GET_MANU_ID;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
        stPlcTaskDetail.InfoMsgBuff[0] = 0; //0:����Ҫ�ȴ��ϱ�; 1:��Ҫ�ȴ��ϱ�ģ����Ϣ
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex);  //�ϵ��ȡ���̱�ʶ 
	}
	
    //�ϵ�������ж�
    if (gPowerOnFlag[nTtlIndex] == TRUE)
    {
    	gPowerOnFlag[nTtlIndex] = FALSE;
    	
		//���� �����ȶ�����
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()�ϵ�ִ�� �����ȶ����� �˿�:%d \n", nTtlIndex);
		gMeterFileCompareFlag[nTtlIndex] = 1;

		//���� Ƶ���ѯ����
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()�ϵ�ִ�� Ƶ���ѯ���� �˿�:%d \n", nTtlIndex);
		gInitFrQueryFlag[nTtlIndex] = 1;

        //�ز�������������̣߳����ȶ�ȡͨ�ŵ�Ԫ��Ϣ(�汾)
        GetUnitInfo(nTtlIndex);
    }

	/*
	//·�ɿ��Ƶ��ж�(ֱ��ִ��)
	if (gRoutrtCtrl[nTtlIndex] != RCS_NULL)
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()��Ϣ����·�ɿ������� �˿�:%d \n", nTtlIndex);
		
		//��������
        stPlcTaskDetail.TaskName = MS_ROUTER_DEAL;
	    stPlcTaskDetail.RoutrtCtrl = gRoutrtCtrl[nTtlIndex];
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 	

		gRoutrtCtrl[nTtlIndex] = RCS_NULL;
	}
	*/

	//�����ȶ�������ж�
	if((1 == gMeterFileCompareFlag[nTtlIndex]) && (0 == gMeterFileModifyState[nTtlIndex]))
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()��Ϣ���������ȶ����� �˿�:%d \n", nTtlIndex);
		gMeterFileCompareFlag[nTtlIndex] = 0;

		//��������
        stPlcTaskDetail.TaskName = MS_FILE_CMP;
	    stPlcTaskDetail.RoutrtCtrl = RCS_PAUSE;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 

		//�ж��Ƿ���Ҫ������ʼ��
		if(1 == gMeterFileClearFlag[nTtlIndex])
		{
			gMeterFilesModifyType[nTtlIndex] = FILES_MODEFY_CLEAR;
		}
		else
		{
			gMeterFilesModifyType[nTtlIndex] = FILES_MODEFY_UPDATE;
		}
		gMeterFileClearFlag[nTtlIndex] = 0;
		gMeterFileAddDelFlag[nTtlIndex] = 0;

		//����ͬ��״̬
		gMeterFileModifyState[nTtlIndex] = 1;
	}

	//�����޸�������ж�
	if (1 == gMeterFileModifyFlag[nTtlIndex])
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()�����ȶԲ����޸����� �˿�:%d \n", nTtlIndex);
		gMeterFileModifyFlag[nTtlIndex] = 0;
		
		//��������
		stPlcTaskDetail.TaskName = MS_FILE_MODIFY;
	    stPlcTaskDetail.RoutrtCtrl = RCS_PAUSE;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 
	}

	//��������������ж�
	if (gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum > 0)
	{
		//CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()���������������� �˿�:%d \n", nTtlIndex);

		//��������
		stPlcTaskDetail.TaskName = MS_NORMAL_AMR;
	    stPlcTaskDetail.RoutrtCtrl = RCS_RESTART;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex);
	}

	//����͸������
	if (gDataTransmitQueue[nTtlIndex].nDataTransmitNum > 0)
	{
		//CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()��Ϣ��������͸������ �˿�:%d \n", nTtlIndex);

		//��������
		stPlcTaskDetail.TaskName = MS_RELAY_TRANS;
		stPlcTaskDetail.RoutrtCtrl = RCS_PAUSE;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex);
		
	}
	
	//��ȡ�ӽڵ�����ʱ
	if (gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum > 0)
	{		
		//CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()��Ϣ������ȡ�ӽڵ�����ʱ �˿�:%d \n", nTtlIndex);

		//��������
		stPlcTaskDetail.TaskName = MS_SLAVE_DELAY;
		stPlcTaskDetail.RoutrtCtrl = RCS_PAUSE;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex);
		
	}

	//�ӽڵ���
	if (gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum > 0)
	{		
		//CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()��Ϣ�����ӽڵ������� �˿�:%d \n", nTtlIndex);

		//��������
		stPlcTaskDetail.TaskName = MS_SLAVE_MONITOR;
		stPlcTaskDetail.RoutrtCtrl = RCS_PAUSE;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex);
		
	}

	/*
	//�����ѱ�������ж�
	if (gWhetherStartSearchMeter[nTtlIndex].bStartSearch)
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()��Ϣ�����ѱ����� �˿�:%d \n", nTtlIndex);
		
		//��������
        stPlcTaskDetail.TaskName = MS_ADDR_SEARCH;
	    stPlcTaskDetail.RoutrtCtrl = RCS_PAUSE;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		memcpy(stPlcTaskDetail.InfoMsgBuff, &gWhetherStartSearchMeter[nTtlIndex].stAmrMeterStartSearch, sizeof(AMR_METER_START_SEARCH_T));
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 		

		gWhetherStartSearchMeter[nTtlIndex].bStartSearch = FALSE;
		memset(&gWhetherStartSearchMeter[nTtlIndex].stAmrMeterStartSearch, 0x00, sizeof(AMR_METER_START_SEARCH_T));
	}


	//��ͣ�ѱ�������ж�
	if (gPauseSearchMeterFlag[nTtlIndex])
	{
		gPauseSearchMeterFlag[nTtlIndex] = FALSE;
		if(gPlcTaskDetail[nTtlIndex].TaskName == MS_ADDR_SEARCH
			&& gPlcTaskDetail[nTtlIndex].TaskArrIndex == nTtlIndex)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()��Ϣ������ͣ�ѱ����� �˿�:%d \n", nTtlIndex);
		
			gTotalState[nTtlIndex].AddrSearch = AS_SEARCH_END;
		}
		
	}
	*/

	//��ȡ�㲥��ʱ������ж�
	if (gWhetherGetBroadcastDelay[nTtlIndex].bGetBroadcastDelay)
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()��Ϣ������ȡ�㲥��ʱ���� �˿�:%d \n", nTtlIndex);
		
		//��������
		stPlcTaskDetail.InfoDataHead = gWhetherGetBroadcastDelay[nTtlIndex].InfoDataHead;
        stPlcTaskDetail.TaskName = MS_BROADCAST_DELAY;
	    stPlcTaskDetail.RoutrtCtrl = RCS_PAUSE;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		memcpy(stPlcTaskDetail.InfoMsgBuff, &gWhetherGetBroadcastDelay[nTtlIndex].stAmrGetBroadcastDelay, sizeof(AMR_METER_START_SEARCH_T));
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 
		
		gWhetherGetBroadcastDelay[nTtlIndex].bGetBroadcastDelay = FALSE;
		memset(&gWhetherGetBroadcastDelay[nTtlIndex].stAmrGetBroadcastDelay, 0x00, sizeof(AMR_METER_START_SEARCH_T));
	}

	//�����㲥
	if (gWhetherDoBroadcast[nTtlIndex].bDoBroadcast)
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()��Ϣ���������㲥���� �˿�:%d \n", nTtlIndex);
		
		//��������
		stPlcTaskDetail.TaskName = MS_BROADCAST_DEAL;
	    stPlcTaskDetail.RoutrtCtrl = RCS_PAUSE;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		memcpy(stPlcTaskDetail.InfoMsgBuff, &gWhetherDoBroadcast[nTtlIndex].stAmrDoBroadcast, sizeof(AMR_DO_BROADCAST_T));
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 
		
		gWhetherDoBroadcast[nTtlIndex].bDoBroadcast = FALSE;
		memset(&gWhetherDoBroadcast[nTtlIndex].stAmrDoBroadcast, 0x00, sizeof(AMR_METER_START_SEARCH_T));
	}

	//�����ǰccoģ��Ϊխ���������²�ѯhplc��Ϣ
	if(gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b1.BitSect.CommPattern == 2)//ͨ�ŷ�ʽ(1:խ��,2:���,3:С����)
	{
		//HPLCƵ���ѯ(�ϵ�����)
		if(gInitFrQueryFlag[nTtlIndex] > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()�ϵ����HPLCƵ���ѯ���� �˿�:%d \n", nTtlIndex);
			
			//��������
			stPlcTaskDetail.TaskName = MS_HPLC_FR_QUERY;
		    stPlcTaskDetail.RoutrtCtrl = RCS_NULL;
			stPlcTaskDetail.TaskArrIndex = nTtlIndex;
			stPlcTaskDetail.InfoMsgBuff[0] = 0; //0:����Ҫ�ϱ�; 1:��Ҫ�ϱ�mqtt
			PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 

			gInitFrQueryFlag[nTtlIndex] = 0;
		}
		
		//HPLC��Ϣ�ϱ�
		if(gQueryTypeHPLC[nTtlIndex].Value > 0)
		{
			//��������
			stPlcTaskDetail.TaskName = MS_HPLC_REPORT;
		    stPlcTaskDetail.RoutrtCtrl = RCS_NULL;
			stPlcTaskDetail.TaskArrIndex = nTtlIndex;
			PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 
		}

		//HPLCƵ������
		if(gFrSetFlag[nTtlIndex] > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()��Ϣ����HPLCƵ���������� �˿�:%d \n", nTtlIndex);
			
			//��������
			stPlcTaskDetail.TaskName = MS_HPLC_FR_SET;
		    stPlcTaskDetail.RoutrtCtrl = RCS_NULL;
			stPlcTaskDetail.TaskArrIndex = nTtlIndex;
			PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 

			gFrSetFlag[nTtlIndex] = 0;
		}

		//HPLCƵ���ѯ
		if(gFrQueryFlag[nTtlIndex] > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()��Ϣ����HPLCƵ���ѯ���� �˿�:%d \n", nTtlIndex);
			
			//��������
			memcpy(&stPlcTaskDetail.InfoDataHead, &gFrQueryInfoDataHead[nTtlIndex], sizeof(INFO_DATA_HEAD_T));
			stPlcTaskDetail.TaskName = MS_HPLC_FR_QUERY;
		    stPlcTaskDetail.RoutrtCtrl = RCS_NULL;
			stPlcTaskDetail.TaskArrIndex = nTtlIndex;
			stPlcTaskDetail.InfoMsgBuff[0] = 1; //0:����Ҫ�ϱ�; 1:��Ҫ�ϱ�mqtt
			PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 

			gFrQueryFlag[nTtlIndex] = 0;
		}

		//HPLC̨��ʶ��
		if(gAreaIdentifyFlag[nTtlIndex] > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()��Ϣ����HPLC̨��ʶ������ �˿�:%d \n", nTtlIndex);
			
			//��������
			stPlcTaskDetail.TaskName = MS_HPLC_AREA;
		    stPlcTaskDetail.RoutrtCtrl = RCS_NULL;
			stPlcTaskDetail.TaskArrIndex = nTtlIndex;
			PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 

			gAreaIdentifyFlag[nTtlIndex] = 0;
		}

		//HPLC������֪
		if(gOfflineFeelFlag[nTtlIndex] > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()��Ϣ����HPLC������֪���� �˿�:%d \n", nTtlIndex);
			
			//��������
			stPlcTaskDetail.TaskName = MS_HPLC_OFFLINE;
		    stPlcTaskDetail.RoutrtCtrl = RCS_NULL;
			stPlcTaskDetail.TaskArrIndex = nTtlIndex;
			PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 

			gOfflineFeelFlag[nTtlIndex] = 0;
		}

		//HPLC�ܾ��ڵ��ϱ�
		if(gRefuseNodeReportFlag[nTtlIndex] > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()��Ϣ����HPLC�ܾ��ڵ��ϱ� �˿�:%d \n", nTtlIndex);
			
			//��������
			stPlcTaskDetail.TaskName = MS_HPLC_REFUSE;
		    stPlcTaskDetail.RoutrtCtrl = RCS_NULL;
			stPlcTaskDetail.TaskArrIndex = nTtlIndex;
			PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 

			gRefuseNodeReportFlag[nTtlIndex] = 0;
		}
	}

	//5����дһ�ε���˽���ļ�
	//RegularWriteFilesToPData(nArrIndex, 5);

}

/*******************************************************************************
* ��������: CombineTaskFunInput
* ��������: ������Ĳ���ת��Ϊ�����������
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int CombineTaskFunInput(const PLCTASK_DETAIL stPlcTaskDetail, TASK_FUN_INPUT *stTaskFunInput)
{
	stTaskFunInput->InfoDataHead = stPlcTaskDetail.InfoDataHead;
	
	stTaskFunInput->TaskStat = stPlcTaskDetail.TaskStat;
	stTaskFunInput->TaskName = stPlcTaskDetail.TaskName;
	memcpy(stTaskFunInput->strTaskName, stPlcTaskDetail.strTaskName, sizeof(stTaskFunInput->strTaskName));
	stTaskFunInput->Priority = stPlcTaskDetail.Priority;
	stTaskFunInput->RoutrtCtrl = stPlcTaskDetail.RoutrtCtrl;
	stTaskFunInput->DelaySec = stPlcTaskDetail.DelaySec;
	stTaskFunInput->PlcTaskDeal_Stat = stPlcTaskDetail.PlcTaskDeal_Stat;
	memcpy(stTaskFunInput->InfoMsgBuff, stPlcTaskDetail.InfoMsgBuff, sizeof(stTaskFunInput->InfoMsgBuff));
	stTaskFunInput->TaskArrIndex = stPlcTaskDetail.TaskArrIndex;
	return 1;
}

/*******************************************************************************
* ��������: PlcTaskRunRouterDeal
* ��������: ����ִ�д���(·�в���)
* �������: 
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void PlcTaskRunRouterDeal(uint8 nArrIndex)
{
	if((gDevHalInfo[nArrIndex].isvalid == 0) || (1 == gReloadDevFlag[nArrIndex]))
	{
		return;  //ģ����Ϣ��Ч, ֱ���˳�
	}

	if( gPlcTaskDetail[nArrIndex].PlcTaskDeal_Stat != TASK_INIT )
	{
		return;  //����ִ�о������������, ֱ���˳�
	}

	int Ret = 0;
	uint8	nTtlIndex = 0;	//��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = nArrIndex;

	PLCTASK_DETAIL	stPlcTaskDetail;	   //�ز����������Ϣ
	memset((uint8 *)&stPlcTaskDetail, 0x00, sizeof(stPlcTaskDetail));

	//·�ɿ��Ƶ��ж�
	if (gRoutrtCtrl[nTtlIndex] != RCS_NULL)
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskRunRouterDeal()��Ϣ����·�ɿ������� �˿�:%d \n", nTtlIndex);
		
		//��������
		stPlcTaskDetail.TaskName = MS_ROUTER_DEAL;
		stPlcTaskDetail.RoutrtCtrl = gRoutrtCtrl[nTtlIndex];
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;

		//��־����
		gRoutrtCtrl[nTtlIndex] = RCS_NULL;

		//����������Ϣ
		Ret = FillTaskRunInput(&stPlcTaskDetail);
		if(Ret < 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskRunRouterDeal() input����ʧ�� Ret:%d \n", Ret);
			return;
		}

		//Ȼ��ִ������
		TASK_FUN_INPUT TASK_FUN_INPUT;
		memset(&TASK_FUN_INPUT, 0, sizeof(TASK_FUN_INPUT));
		CombineTaskFunInput(stPlcTaskDetail, &TASK_FUN_INPUT);
		
		Ret = stPlcTaskDetail.pPlcTaskFun(TASK_FUN_INPUT);
		CCODOWN_FMT_DEBUG(nTtlIndex, "stPlcTaskDetail.pPlcTaskFun(TASK_FUN_INPUT) Ret:%d \n", Ret);
	}
}


/*******************************************************************************
* ��������: PlcTaskRun
* ��������: ����ִ�д���
* �������: 
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void PlcTaskRun(uint8    nArrIndex)
{
	if((gDevHalInfo[nArrIndex].isvalid == 0) || (1 == gReloadDevFlag[nArrIndex]))
	{
		return;  //ģ����Ϣ��Ч, ֱ���˳�
	}
	
	int Ret = 0;
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = nArrIndex;

	//��ǰ�����ִ�д���
    switch(gPlcTaskDetail[nTtlIndex].PlcTaskDeal_Stat)
    {
		case TASK_INIT:                         //����ĳ�ʼ������
			{
				Ret = ListEmpty_Task(PlcTaskQueue[nTtlIndex]);
				if(Ret > 0)
				{
					//printx(" void PlcTaskRun(void) ��ǰ�������Ϊ��...\n");
				}
				else
		        {
		        	CCODOWN_FMT_DEBUG(nTtlIndex, "  ######  �˿�[%d] PlcTaskRun(void)  ######  ������=%d ִ�� TASK_INIT  ######  \n", nTtlIndex, gPlcTaskDetail[nTtlIndex].TaskName);
					
					ListDelete_Task(&PlcTaskQueue[nTtlIndex], 1, &gPlcTaskDetail[nTtlIndex]);
					
					CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskRun() ��ȡ��������е���������... \n");    //�����ǰ�������Ϊ�գ���������еĵ�һ��������Ϊ��ǰ����
					PrintTaskDetail(gPlcTaskDetail[nTtlIndex], nTtlIndex);  //��ӡ������Ϣ
					CCODOWN_FMT_DEBUG(nTtlIndex, "ListLength_Task(PlcTaskQueue) �������ʣ����������:%d \n", ListLength_Task(PlcTaskQueue[nTtlIndex])); 
					PrintTaskList(PlcTaskQueue[nTtlIndex], nTtlIndex);

					gPlcTaskDetail[nTtlIndex].PlcTaskDeal_Stat = FUN_DEAL;
				}
			}
			break;
		/*
		case ROUTERCTRL_DEAL:                   //·�ɵĲ���
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, " ---->�˿�[%d] PlcTaskRun(void)    ������ : %d ִ�� ROUTERCTRL_DEAL   ------    \n", nTtlIndex, gPlcTaskDetail[nTtlIndex].TaskName);
				gPlcTaskDetail[nTtlIndex].PlcTaskDeal_Stat = ROUTERCTRL_DELAY;
			}
			break;
		
		case ROUTERCTRL_DELAY:                  //��ʱ����
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, " ---->�˿�[%d] PlcTaskRun(void)    ������ : %d ִ�� ROUTERCTRL_DELAY   ------    \n", nTtlIndex, gPlcTaskDetail[nTtlIndex].TaskName);
				gPlcTaskDetail[nTtlIndex].PlcTaskDeal_Stat = FUN_DEAL;
			}
			break;
		*/
		case FUN_DEAL:                          //��ǰ������
			{
				TASK_FUN_INPUT TASK_FUN_INPUT;
				memset(&TASK_FUN_INPUT, 0, sizeof(TASK_FUN_INPUT));
				CombineTaskFunInput(gPlcTaskDetail[nTtlIndex], &TASK_FUN_INPUT);
				
				Ret = gPlcTaskDetail[nTtlIndex].pPlcTaskFun(TASK_FUN_INPUT);
				if(Ret > 0)
				{
					gPlcTaskDetail[nTtlIndex].PlcTaskDeal_Stat = TASK_INIT;
				}
			}
			break;
		/*
		case ROUTERRECOVER_DELAY:                //·�ɻָ���ʱ����
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, " ---->�˿�[%d] PlcTaskRun(void)    ������ : %d ִ�� ROUTERRECOVER_DELAY   ------    \n", nTtlIndex, gPlcTaskDetail[nTtlIndex].TaskName);
				gPlcTaskDetail[nTtlIndex].PlcTaskDeal_Stat = ROUTERRECOVER_DEAL;
			}
			break;
		case ROUTERRECOVER_DEAL:                //·�ɻָ�����
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, " ---->�˿�[%d] PlcTaskRun(void)    ������ : %d ִ�� ROUTERRECOVER_DEAL   ------    \n", nTtlIndex, gPlcTaskDetail[nTtlIndex].TaskName);
				gPlcTaskDetail[nTtlIndex].PlcTaskDeal_Stat = TASK_INIT;
			}
			break;
		*/
		
		default:
			gPlcTaskDetail[nTtlIndex].PlcTaskDeal_Stat = TASK_INIT;
			break;
	}

}

/*******************************************************************************
* ��������: PlcTaskRunSearchMeter
* ��������: ����ִ�д���(�ѱ�)
* �������: 
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void PlcTaskRunSearchMeter(uint8 nArrIndex)
{
	if((gDevHalInfo[nArrIndex].isvalid == 0) || (1 == gReloadDevFlag[nArrIndex]))
	{
		return;  //ģ����Ϣ��Ч, ֱ���˳�
	}

	if(CheckModuleSoftReset(-1, nArrIndex) == 1)
	{
		return;  //δ��ȡ��ccoģ����Ϣ, ֱ���˳�
	}

	int Ret = 0;
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = nArrIndex;

    //����������ֹͣ�ѱ�
    if(MUD_IDLE != gTotalState[nTtlIndex].ModuleUpdateDeal)
    {
        if(1 == gAmrStateQuery[nTtlIndex].nModuleSearchState)//��ǰ�������ѱ�״̬
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskRunSearchMeter()ģ������������ͣ�ѱ����� �˿�:%d \n", nTtlIndex);
			gTotalState[nTtlIndex].AddrSearch = AS_SEARCH_END;
            return;
		}
    }

	//��ǰ�����ִ�д���
    switch(gPlcTaskDetailSearchMeter[nTtlIndex].PlcTaskDeal_Stat)
    {
		case TASK_INIT:                         //����ĳ�ʼ������
		{
			//�����ѱ�������ж�
			if (gWhetherStartSearchMeter[nTtlIndex].bStartSearch)
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskRunSearchMeter()��Ϣ�����ѱ����� �˿�:%d \n", nTtlIndex);
				
				//��������
				memset(&gPlcTaskDetailSearchMeter[nTtlIndex], 0x00, sizeof(gPlcTaskDetailSearchMeter[nTtlIndex]));
			    gPlcTaskDetailSearchMeter[nTtlIndex].TaskName = MS_ADDR_SEARCH;
			    gPlcTaskDetailSearchMeter[nTtlIndex].RoutrtCtrl = RCS_PAUSE;
				gPlcTaskDetailSearchMeter[nTtlIndex].TaskArrIndex = nTtlIndex;
				memcpy(gPlcTaskDetailSearchMeter[nTtlIndex].InfoMsgBuff, &gWhetherStartSearchMeter[nTtlIndex].stAmrMeterStartSearch, sizeof(AMR_METER_START_SEARCH_T));	

				//����������Ϣ
				Ret = FillTaskRunInput(&gPlcTaskDetailSearchMeter[nTtlIndex]);
				if(Ret < 0)
				{
					CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskRunSearchMeter() input����ʧ�� Ret:%d \n", Ret);
					break;
				}

				//��������
				gWhetherStartSearchMeter[nTtlIndex].bStartSearch = FALSE;
				memset(&gWhetherStartSearchMeter[nTtlIndex].stAmrMeterStartSearch, 0x00, sizeof(AMR_METER_START_SEARCH_T));

				//ת��������
				gPlcTaskDetailSearchMeter[nTtlIndex].PlcTaskDeal_Stat = FUN_DEAL;
			}
		}
			break;
		case FUN_DEAL:                          //��ǰ������
		{
			//�����ж��Ƿ��յ���ͣ�ѱ�ָ��
			if (gPauseSearchMeterFlag[nTtlIndex])
			{
				gPauseSearchMeterFlag[nTtlIndex] = FALSE;
				if(1 == gAmrStateQuery[nTtlIndex].nModuleSearchState)//��ǰ�������ѱ�״̬
				{
					CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskRunSearchMeter()��Ϣ������ͣ�ѱ����� �˿�:%d \n", nTtlIndex);
					gTotalState[nTtlIndex].AddrSearch = AS_SEARCH_END;
				}
			}

			//Ȼ��ִ���ѱ�����
			TASK_FUN_INPUT TASK_FUN_INPUT;
			memset(&TASK_FUN_INPUT, 0, sizeof(TASK_FUN_INPUT));
			CombineTaskFunInput(gPlcTaskDetailSearchMeter[nTtlIndex], &TASK_FUN_INPUT);
			
			Ret = gPlcTaskDetailSearchMeter[nTtlIndex].pPlcTaskFun(TASK_FUN_INPUT);
			if(Ret > 0)
			{
				gPlcTaskDetailSearchMeter[nTtlIndex].PlcTaskDeal_Stat = TASK_INIT;
			}
		}
			break;
		default:
			gPlcTaskDetailSearchMeter[nTtlIndex].PlcTaskDeal_Stat = TASK_INIT;
			break;
	}
}


/*******************************************************************************
* ��������: GetHeadInfo
* ��������: ��װ�����ظ���Ϣ
* �������: 
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int GetHeadInfo(INFO_DATA_T InfoMsgData, MSG_INFO_T *ptMsgInfo)
{	
	//head
	ptMsgInfo->IP = InfoMsgData.IP;
	ptMsgInfo->port = InfoMsgData.port;
	ptMsgInfo->MsgRPM = InfoMsgData.MsgRPM;
	ptMsgInfo->MsgPRIORITY = InfoMsgData.MsgPRIORITY;		
	ptMsgInfo->MsgIndex = InfoMsgData.MsgIndex;
	ptMsgInfo->MsgLabel = InfoMsgData.MsgLabel;
	if(InfoMsgData.MsgRPM == 0)  //��Ϣ�ظ�
	{
		ptMsgInfo->SourLen = InfoMsgData.DestLen;
		memcpy(ptMsgInfo->SourAddr, InfoMsgData.DestAddr, ptMsgInfo->SourLen);
		ptMsgInfo->DestLen = InfoMsgData.SourLen;
		memcpy(ptMsgInfo->DestAddr, InfoMsgData.SourAddr, ptMsgInfo->DestLen);
	}
	else if(InfoMsgData.MsgRPM == 1)  //�����ϱ�
	{
		ptMsgInfo->SourLen = InfoMsgData.SourLen;
		memcpy(ptMsgInfo->SourAddr, InfoMsgData.SourAddr, ptMsgInfo->SourLen);
		ptMsgInfo->DestLen = InfoMsgData.DestLen;
		memcpy(ptMsgInfo->DestAddr, InfoMsgData.DestAddr, ptMsgInfo->DestLen);
	}

	//msg
	ptMsgInfo->IID = InfoMsgData.IID_ID;
	ptMsgInfo->IOP = InfoMsgData.IOP_ID;
	ptMsgInfo->MsgLen = (uint32)InfoMsgData.nPayloadLength;
	memcpy(ptMsgInfo->MsgData, InfoMsgData.aPayloadData, ptMsgInfo->MsgLen);
	
	return 1;
}

/**********************************************************
* �������ƣ�
		XADR_GetEleNum
* ����������
		��ȡ����������Ԫ�س���
* ����������
		indata:����Buffer
* ����ֵ��
		bytes:������ֽڸ�����
**********************************************************/
uint16 XADR_GetEleNum(const uint8 *indata,uint8 *bytes)
{
    uint16 EleNum;
    uint8 nbytes = 1;
    EleNum = indata[0];
    if(EleNum==0x81)
    {
        EleNum = indata[1];
        nbytes = 2;
    }
    else if(EleNum==0x82)
    {
        EleNum = indata[1];
        EleNum <<= 8;
        EleNum += indata[2];
        nbytes = 3;
    }
    else if(EleNum==0x80 || EleNum>0x82)
        return 0;
    if(bytes!=NULL)
        *bytes = nbytes;
    return EleNum;
}

/*******************************************************************************
* ��������: ModuleInfoDeal
* ��������: ģ����Ϣ����
* �������: 
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void ModuleInfoDeal(uint8 ModuleIndex, uint8 *RecvBuf, uint32 RecvLen)
{
	CCOUP_FMT_DEBUG("ModuleInfoDeal() ModuleIndex=%d \n", ModuleIndex);
	
	int i = 0;
	uint16 index = 0;
	uint16 lentmp = 0;
	uint8 lenoffset = 0;
	
	pthread_rwlock_wrlock(&gModuleInfo[ModuleIndex-1].ModuleLock);

	if(RecvBuf[index++] !=0)
    {
        gModuleInfo[ModuleIndex-1].IsValid = 0;
        pthread_rwlock_unlock(&gModuleInfo[ModuleIndex-1].ModuleLock);
        return;
    }
	
    lentmp = XADR_GetEleNum(&RecvBuf[index],&lenoffset);
    index+=lenoffset;
    memcpy(gModuleInfo[ModuleIndex-1].DeviceType,&RecvBuf[index],lentmp);
    index+=lentmp;
    
    lentmp = XADR_GetEleNum(&RecvBuf[index],&lenoffset);
    index+=lenoffset;
    memcpy(gModuleInfo[ModuleIndex-1].DeviceID,&RecvBuf[index],lentmp);
    index+=lentmp;
    
    lentmp = XADR_GetEleNum(&RecvBuf[index],&lenoffset);
    index+=lenoffset;
    memcpy(gModuleInfo[ModuleIndex-1].SoftVersion,&RecvBuf[index],lentmp);
    index+=lentmp;

    
    lentmp = XADR_GetEleNum(&RecvBuf[index],&lenoffset);
    index+=lenoffset;
    memcpy(gModuleInfo[ModuleIndex-1].SoftDate,&RecvBuf[index],lentmp );
    index+=lentmp ;
    
    lentmp = XADR_GetEleNum(&RecvBuf[index],&lenoffset);
    index+=lenoffset;
    memcpy(gModuleInfo[ModuleIndex-1].HardVersion,&RecvBuf[index],lentmp );
    index+=lentmp ;

    lentmp = XADR_GetEleNum(&RecvBuf[index],&lenoffset);
    index+=lenoffset;
    memcpy(gModuleInfo[ModuleIndex-1].HardDate,&RecvBuf[index],lentmp);
    index+=lentmp;

    lentmp = XADR_GetEleNum(&RecvBuf[index],&lenoffset);
    index+=lenoffset;
    memcpy(gModuleInfo[ModuleIndex-1].ManufacturerCode,&RecvBuf[index],lentmp);
    index+=lentmp;
    gModuleInfo[ModuleIndex-1].ChannelNum = RecvBuf[index++];
    CCOUP_FMT_DEBUG("DeviceType %s DeviceID %s \n",gModuleInfo[ModuleIndex-1].DeviceType,gModuleInfo[ModuleIndex-1].DeviceID);
    CCOUP_FMT_DEBUG("SoftVersion %s SoftDate %s \n",gModuleInfo[ModuleIndex-1].SoftVersion,gModuleInfo[ModuleIndex-1].SoftDate);
    CCOUP_FMT_DEBUG("HardVersion %s HardDate %s \n",gModuleInfo[ModuleIndex-1].HardVersion,gModuleInfo[ModuleIndex-1].HardDate);
    CCOUP_FMT_DEBUG("ManufacturerCode %s ChannelNum %d\n",gModuleInfo[ModuleIndex-1].ManufacturerCode,gModuleInfo[ModuleIndex-1].ChannelNum);
    for(i=0;i<gModuleInfo[ModuleIndex-1].ChannelNum;i++)
    {
        gModuleInfo[ModuleIndex-1].ChannelInfo[i].ChannelType = RecvBuf[index++];
        gModuleInfo[ModuleIndex-1].ChannelInfo[i].ChannelFunc = RecvBuf[index++];
        lentmp = XADR_GetEleNum(&RecvBuf[index],&lenoffset);
        index+=lenoffset;
        memcpy(gModuleInfo[ModuleIndex-1].ChannelInfo[i].DriveName,&RecvBuf[index],lentmp);
        index+=lentmp;
        CCOUP_FMT_DEBUG("i %d type %d func %d drivename %s\n",i,gModuleInfo[ModuleIndex-1].ChannelInfo[i].ChannelType,gModuleInfo[ModuleIndex-1].ChannelInfo[i].ChannelFunc,
            gModuleInfo[ModuleIndex-1].ChannelInfo[i].DriveName);
    }
    gModuleInfo[ModuleIndex-1].IsValid = 1;

    pthread_rwlock_unlock(&gModuleInfo[ModuleIndex-1].ModuleLock);
}


/*******************************************************************************
* ��������: GetModuleInfo
* ��������: ��ȡģ����Ŷ�Ӧ��ģ����Ϣ
* �������: 
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int GetModuleInfo(uint8 ModuleIndex)
{
	CCOUP_FMT_DEBUG("GetModuleInfo() ��ѯģ����Ϣ ...ModuleIndex=%d \n", ModuleIndex);
    if(ModuleIndex<1 || ModuleIndex>MAX_MODULE_NUM)
    {
		return -1;
	}

	int8   SocketState = ACT_COMM_IDLE;        //����״̬
	time_t CommBeginTime;  //ͨѶ��ʼʱ��
	uint8  RecvBuf[MAX_QUEUE_BUFF_LEN] = {0};  //socket���ջ�����    
    int32  RecvLen = 0;                        //�������ݳ���
    int8   RecvResult = 0;
    uint8  RetryCnt = 0;
    MSG_INFO_T *ptMsgInfo = NULL;
	//int ret = 0;

	uint8  aPayload[200] = {0};
	uint16 nPayloadLen = 0;

	uint16 nIID = 0;
	uint16 nIOP = 0;
	uint16 nIndex = 0;
	
    nPayloadLen += 3;
    aPayload[nPayloadLen++] = ModuleIndex;

	while(1)
	{
		switch (SocketState)
        {
	        case ACT_COMM_IDLE:
	        {
				SocketState = ACT_COMM_SEND;
	            break;
			}
			case ACT_COMM_SEND:
	        {
				//���Ͳ�ѯ��Ϣ
				module_from_mapManager(aPayload, nPayloadLen, &nIID, &nIOP, &nIndex);

				//���¼�ʱ��
				CommBeginTime= time(NULL);

	            //ת����̼����ݽ��մ���
	            SocketState = ACT_COMM_RECV;

	            break;
			}
			case ACT_COMM_RECV:
	        {
				memset(RecvBuf,0x00,sizeof(RecvBuf));
		    	RecvLen = PlcReadQueue(pAmrRecvUp, RecvBuf);
				
				if(RecvLen > 0)
				{
					ptMsgInfo = (MSG_INFO_T *)RecvBuf;
					
					//��ӡ������Ϣ����
					//CCOUP_FMT_DEBUG("GetModuleInfo()...ACT_COMM_RECV...ͨ��Mqtt�յ�����ϢΪ: \n");
					//PrintInfoDetail(ptMsgInfo);

					//��Ϣ����
					if((ptMsgInfo->IID == nIID) && (ptMsgInfo->IOP == nIOP) && (ptMsgInfo->MsgIndex == nIndex))
					{	CCOUP_FMT_DEBUG("GetModuleInfo() ��ѯģ����Ϣ �յ�IID == %d, IOP == %d, index == %d ! \n", nIID, nIOP, nIndex);
						ModuleInfoDeal(ModuleIndex, ptMsgInfo->MsgData, ptMsgInfo->MsgLen);
						return 1;
					}
					else if(ptMsgInfo->IID == 0x0000 && ptMsgInfo->IOP == 0x0014)
					{
						CCOUP_FMT_DEBUG("GetModuleInfo() ��ѯģ����Ϣ �յ�IID == 0x0000, IOP == 0x0014 ���������Ϣ! \n");
						return -1;
					}
					else
					{
						RecvResult = -1;
					}
				}
				else
				{
					RecvResult = -1;
				}
				
				if(RecvResult == -1)
	            {
	                if( abs(time(NULL)-CommBeginTime) > MAX_MODULE_DELAY_TIME)
	                {
	                    RetryCnt++;
	                    SocketState = ACT_COMM_RETRY;
	                    CCOUP_FMT_DEBUG("GetModuleInfo() ��ѯģ����Ϣ ��ʱ����RetryCnt=%d \n", RetryCnt);
	                }
	            }
				            
	            break;
			}
			case ACT_COMM_RETRY:
			{
				if(RetryCnt>3)
	            {
	                SocketState = ACT_COMM_IDLE;
	                return -1;
	            }
	            SocketState = ACT_COMM_SEND;
				
				break;
			}
	        default:
	        {
				SocketState = ACT_COMM_IDLE;
	            break;
			}
        }
		
		usleep(20000);
	}

	return 1;
}


/*******************************************************************************
* ��������: GetAllModuleInfo
* ��������: ��ѯ����ģ����Ϣ
* �������: 
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int GetAllModuleInfo()
{
    uint8 i = 0;
	uint8 result[MAX_MODULE_NUM] = {0};  //ģ����Ϣ��ȡ���
    
    for(i=0; i<MAX_MODULE_NUM; i++)
    {
        memset(&gModuleInfo[i],0x00,sizeof(MODULE_INFO_T));
        if ( pthread_rwlock_init ( &gModuleInfo[i].ModuleLock, NULL ) != 0 )
        {
            CCOUP_FMT_DEBUG ("init gModuleInfo lock %i error! \n", i);
            continue;
        }
        CCOUP_FMT_DEBUG("��ʼ��ȡģ��%d��Ϣ��\n", i+1);
		
        if(GetModuleInfo(i+1) > 0)
        {
            CCOUP_FMT_DEBUG("��ȡģ��%d��Ϣ�ɹ���\n", i+1);
			result[i] = 1;
        }
		else
        {
            CCOUP_FMT_DEBUG("��ȡģ��%d��Ϣʧ�ܣ�\n", i+1);
			result[i] = 0;
        }
    }

	if((result[0]==0) && (result[1]==0))//û�б���ģ��
	{
		return -1;
	}

	return 1;
}

/*******************************************************************************
* ��������: UpdateModuleFiles
* ��������: ģ���ε��� ����ģ�鵵����Ϣ
* �������: 
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int UpdateModuleFiles()
{
	CCOUP_FMT_DEBUG("UpdateModuleFiles() ���ڲ�ε��µ�ģ�鵵������... \n");
	uint8 i = 0;

	for(i=0; i<MAX_ALLOWED_DOWN_THREAD_NUM; i++)
	{
		if(gDevHalInfo[i].isvalid == 1)
		{
			gMeterFileCompareFlag[i] = 1;//����ccoģ�鵵��
			gReloadDevFlag[i] = 1;//���´��ڲ������
			gModuleManuID[i] = MMID_NULL;//����ģ����Ϣ
			CheckModuleSoftReset(1, i);//������ѯģ����Ϣ��־��1
			ClearCommQueueBuff(&gAmrRecvDownReportModule[i]);//��������ϱ�ģ����Ϣ
			
			CCOUP_FMT_DEBUG("UpdateModuleFiles() i=%d, gDevHalInfo[i].isvalid=%d, gMeterFileCompareFlag[i]=%d, gReloadDevFlag[i]=%d, gModuleManuID[i]=%d \n", 
				i, gDevHalInfo[i].isvalid, gMeterFileCompareFlag[i], gReloadDevFlag[i], gModuleManuID[i]);
		}
	}

	return 1;
}

/*******************************************************************************
* ��������: UpdateModuleInfo
* ��������: ����ģ����Ϣ
* �������: 
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int UpdateModuleInfo()
{
	CCOUP_FMT_DEBUG("UpdateModuleInfo() ���ڳ�ʼ��/��ε��µ�ģ����Ϣ���� begin... \n");
	uint8 i = 0;
	uint8 j = 0;
	uint8 count = 0;
    int pipe = 0;
    int ret = 0;
    uint8 gCcoFlagTemp[5] = {0};//�Ƿ�CCOģ����
    BOOL bCCOModuleChanged = FALSE;//CCOģ���Ƿ����仯

    //��¼���µ�CCOģ���λ�±���Ϣ
    for(i=0; i<MAX_MODULE_NUM; i++)
	{
	    pthread_rwlock_rdlock(&gModuleInfo[i].ModuleLock);

		if(gModuleInfo[i].IsValid==1)
		{
		    /* �ж��Ƿ�CCOģ�� */
            if(0 == memcmp(gModuleInfo[i].DeviceType, "GBH", 3))
            {
                gCcoFlagTemp[i] = 1;
            }			
		}
		
		pthread_rwlock_unlock(&gModuleInfo[i].ModuleLock);
	}

    //�ж�CCOģ���Ƿ�����λ
    for(i=0; i<MAX_MODULE_NUM; i++)
	{
	    if(gCcoFlagTemp[i] != gCcoFlag[i])
        {
            bCCOModuleChanged = TRUE;
            CCOUP_FMT_DEBUG("CCOģ��gModuleInfo[%d]�����仯, ��Ҫ����! \n", i);
        }
	}

    if(FALSE == bCCOModuleChanged)
    {
        CCOUP_FMT_DEBUG("CCOģ��λ��û�з����仯, �����и��²���! \n");
        return -1;
    }

    //��ʼ��֮ǰ��Ҫ���ͷ�
    for(pipe=0; pipe<MAX_DOWN_THREAD_NUM; pipe++)
    {
        if(NULL != gDevHalInfo[pipe].dev)
        {
            gDevHalInfo[pipe].isvalid = 0;
            ret = hal_device_release((HW_DEVICE *)gDevHalInfo[pipe].dev);
            gDevHalInfo[pipe].dev = NULL;
            CCODOWN_FMT_DEBUG(pipe, "hal_device_release() ret=%d \n", ret);
        }
    }

	memset(gDevHalInfo, 0x00, sizeof(gDevHalInfo));
    memset(gCcoFlag, 0x00, sizeof(gCcoFlag));
    
	for(i=0; i<MAX_MODULE_NUM; i++)
	{
		pthread_rwlock_rdlock(&gModuleInfo[i].ModuleLock);

		if(gModuleInfo[i].IsValid==1)
		{
			for(j=0; j<gModuleInfo[i].ChannelNum; j++)
			{
				if(gModuleInfo[i].ChannelInfo[j].ChannelFunc==2)
                {
                    if(count >= MAX_ALLOWED_DOWN_THREAD_NUM)
                    {
                        pthread_rwlock_unlock(&gModuleInfo[i].ModuleLock);
                        break;
                    }
                    sprintf(gDevHalInfo[count].deviceName, "%s%d_%d", CCO_MODULE_DEVNAME, i+1, j);

                    gDevHalInfo[count].isvalid = 1;
					gDevHalInfo[count].moduleIndex = i+1;
                    count++;                    

					gModuleInfo[i].bCCO = TRUE;//���б���cco����
					gCcoFlag[i] = 1;//����ccoͨ��
                }
			}
		}
		
		pthread_rwlock_unlock(&gModuleInfo[i].ModuleLock);
	}

	CCOUP_FMT_DEBUG("UpdateModuleInfo() ���ڳ�ʼ��/��ε��µ�ģ����Ϣ���� end... \n");
	return 1;
}

/*******************************************************************************
* ��������: GlobalInit
* ��������: ��ʼ������
* �������:
* �������:
* �ڲ�����:
* �� �� ֵ: 
*******************************************************************************/
int GlobalInit()
{
	memset(gDevHalInfo, 0x00, sizeof(gDevHalInfo));
	memset(gModuleInfo, 0x00, sizeof(gModuleInfo));

	memset(gActiveAmrInfoList, 0x00, sizeof(gActiveAmrInfoList));
	memset(gGlobalSeqInfo, 0x00, sizeof(gGlobalSeqInfo));
	memset(gWhetherStartSearchMeter, 0x00, sizeof(gWhetherStartSearchMeter));
	memset(gDataTransmitQueue, 0x00, sizeof(gDataTransmitQueue));
	memset(gSlaveMonitorQueue, 0x00, sizeof(gSlaveMonitorQueue));
	memset(gWhetherGetBroadcastDelay, 0x00, sizeof(gWhetherGetBroadcastDelay));
	memset(gWhetherDoBroadcast, 0x00, sizeof(gWhetherDoBroadcast));
	memset(gQueryTypeHPLC, 0x00, sizeof(gQueryTypeHPLC));
	memset(gAmrStateQuery, 0x00, sizeof(gAmrStateQuery));
	memset(gFrQueryInfoDataHead, 0x00, sizeof(gFrQueryInfoDataHead));
	memset(gSlaveMonitorTimeQueue, 0x00, sizeof(gSlaveMonitorTimeQueue));
	memset(gAmrModuleQuery, 0x00, sizeof(gAmrModuleQuery));
	memset(gGuiShowMessage, 0x00, sizeof(gGuiShowMessage));
    memset(gAllCcoStaInfo, 0x00, sizeof(gAllCcoStaInfo));

	return 1;
}


/*******************************************************************************
* ��������: ModuleEventFun
* ��������: ����ģ�����¼�
* �������:
* �������:
* �ڲ�����:
* �� �� ֵ: 
*******************************************************************************/
void ModuleEventFun(uint8* buf, uint16 len)
{
    if(len < 5)
    {
		 return;
	}

    uint32 ModuleIndex = 0;
    uint16 index = 0;
    uint8 status = 0;
	BOOL bCCOModule = FALSE;

    //ģ�����
    memcpy_r((uint8*)&ModuleIndex, &buf[index], 4);
    index += 4;
    status = buf[index++];//���״̬ 0�� 1��
    
    CCOUP_FMT_DEBUG("�յ�ģ�����¼� ģ�����=%d, status=%d, 0�� 1��\n", ModuleIndex, status);
	
    if(ModuleIndex<1 || ModuleIndex>5)
    {
		return;
	}

    if(status == 0)
    {
        //֪ͨ���̶߳�ȡ�������Ϣ
        gReloadModuleFlag[ModuleIndex] = 1;
    }
    else if(status == 1)
    {
        //�ж��Ƿ���CCOģ��γ�
        if(1 != gCcoFlag[ModuleIndex-1])
        {
            CCOUP_FMT_DEBUG("��λ�±�ModuleIndex=%d��CCOģ��, ������! \n", ModuleIndex);
            return;
        }
        
        //ֱ��ɾ������Ŷ�Ӧ��Ϣ
        pthread_rwlock_wrlock(&gModuleInfo[ModuleIndex-1].ModuleLock);
        gModuleInfo[ModuleIndex-1].IsValid = 0;
        pthread_rwlock_unlock(&gModuleInfo[ModuleIndex-1].ModuleLock); 

		//(�γ���Ҫ�ڸ���֮ǰ�ж��Ƿ񱾵�ģ��)
		bCCOModule = gModuleInfo[ModuleIndex-1].bCCO;

		UpdateModuleInfo();

		//�жϰγ��Ƿ񱾵�ģ��  
		if( bCCOModule == TRUE )
		{
			//���±���ģ�鵵����Ϣ
			UpdateModuleFiles();
		}
    }
}


/*******************************************************************************
* ��������: ModuleEventDeal
* ��������: ����ģ�����¼�
* �������:
* �������:
* �ڲ�����:
* �� �� ֵ: 
*******************************************************************************/
int ModuleEventDeal()
{
	int ModuleIndex = 1;
	int num = sizeof(gReloadModuleFlag);
	int ret = -1;
	
	for(ModuleIndex=1; ModuleIndex<num; ModuleIndex++)
	{
		CCOUP_FMT_DEBUG("ModuleEventDeal()...gReloadModuleFlag[%d]=%d \n", ModuleIndex, gReloadModuleFlag[ModuleIndex]);
		if(gReloadModuleFlag[ModuleIndex] == 1)
		{
			if(GetModuleInfo(ModuleIndex) > 0)
			{
				gReloadModuleFlag[ModuleIndex] = 0;
				ret = ModuleIndex;
			}
			else
			{
				ret = -1;
			}

			break;
		}
	}

	return ret;
}


/*******************************************************************************
* ��������: gui_hplc_fr_query
* ��������: ��ʾGUI �鿴��ǰHPLCƵ��
* �������: uint8 ModuleID ����ģ�����(1����2)
* �������: uint8 *Fr Ƶ��ֵ
* �� �� ֵ: ����0����ȷ��С��0������
*******************************************************************************/
int gui_hplc_fr_query(uint8 ModuleID, uint8 *Fr)
{
	CCOGUI_FMT_DEBUG("gui_hplc_fr_query() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_hplc_fr_query() ModuleID=%d �Ƿ�! \n", ModuleID);
		return -1;
	}

	*Fr = gGuiShowMessage[ModuleID-1].nFrValue;
	return 1;
}


/*******************************************************************************
* ��������: gui_amr_port_query
* ��������: ��ʾGUI �鿴��ǰ����˿���Ϣ
* �������: uint8 ModuleID ����ģ�����(1����2)
* �������: uint32 *Port �˿ں�
* �� �� ֵ: ����0����ȷ��С��0������
*******************************************************************************/
int gui_amr_port_query(uint8 ModuleID, uint32 *Port)
{
	CCOGUI_FMT_DEBUG("gui_amr_port_query() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_amr_port_query() ModuleID=%d �Ƿ�! \n", ModuleID);
		return -1;
	}

	*Port = gPortID[ModuleID-1];
	return 1;
}

/*******************************************************************************
* ��������: gui_module_addr_query
* ��������: ��ʾGUI �鿴��ǰģ���ַ
* �������: uint8 ModuleID ����ģ�����(1����2)
			uint8 Len ��ַ����������(6)
* �������: uint8 *Addr ģ���ַ
* �� �� ֵ: ����0����ȷ��С��0������
*******************************************************************************/
int gui_module_addr_query(uint8 ModuleID, uint8 Len, uint8 *Addr)
{
	CCOGUI_FMT_DEBUG("gui_module_addr_query() ModuleID=%d, Len=%d \n", ModuleID, Len);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_module_addr_query() ModuleID=%d �Ƿ�! \n", ModuleID);
		return -1;
	}

	if(Len != 6)
	{
		CCOGUI_FMT_DEBUG("gui_module_addr_query() Len=%d �Ƿ�! \n", Len);
		return -1;
	}

	memcpy(Addr, &gTerminalAddr[ModuleID-1][0], Len);
	return 1;
}


/*******************************************************************************
* ��������: gui_module_info_query
* ��������: ��ʾGUI �鿴ģ���������Ϣ
* �������: uint8 ModuleID ����ģ�����(1����2)
* �������: AMR_QUERY_MODULE_INFO_T *ModuleInfo ģ����Ϣ
* �� �� ֵ: ����0����ȷ��С��0������
*******************************************************************************/
int gui_module_info_query(uint8 ModuleID, AMR_QUERY_MODULE_INFO_T *ModuleInfo)
{
	CCOGUI_FMT_DEBUG("gui_module_info_query() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_module_info_query() ModuleID=%d �Ƿ�! \n", ModuleID);
		return -1;
	}

	memcpy_r(&gAmrModuleQuery[ModuleID-1].aModuleID[0], &gTerminalAddr[ModuleID-1][0], 6);//�ն˵�ַ
	gAmrModuleQuery[ModuleID-1].nCurrentSlaveNum = gAmrMeterFilesList[ModuleID-1].nMeterFilesNum;//��ǰ�ӽڵ�����

	memcpy(ModuleInfo, &gAmrModuleQuery[ModuleID-1], sizeof(AMR_QUERY_MODULE_INFO_T));
	return 1;
}


/*******************************************************************************
* ��������: gui_amr_state_query
* ��������: ��ʾGUI ��ѯ��ǰ����״̬
* �������: uint8 ModuleID ����ģ�����(1����2)
* �������: AMR_QUERY_READING_STATE_T *AmrState ����״̬
* �� �� ֵ: ����0����ȷ��С��0������
*******************************************************************************/
int gui_amr_state_query(uint8 ModuleID, AMR_QUERY_READING_STATE_T *AmrState)
{
	CCOGUI_FMT_DEBUG("gui_amr_state_query() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_amr_state_query() ModuleID=%d �Ƿ�! \n", ModuleID);
		return -1;
	}

	memcpy(AmrState, &gAmrStateQuery[ModuleID-1], sizeof(AMR_QUERY_READING_STATE_T));
	return 1;
}


/*******************************************************************************
* ��������: gui_area_iden_query
* ��������: ��ʾGUI �鿴��ǰHPLC̨��ʶ����״̬
* �������: uint8 ModuleID ����ģ�����(1����2)
* �������: ��
* �� �� ֵ: >=0:����״̬�� -1:����
*******************************************************************************/
int gui_area_iden_query(uint8 ModuleID)
{
	CCOGUI_FMT_DEBUG("gui_area_iden_query() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_area_iden_query() ModuleID=%d �Ƿ�! \n", ModuleID);
		return -1;
	}

	return gGuiShowMessage[ModuleID-1].nAreaIdentifyState;
}

/*******************************************************************************
* ��������: gui_offline_feel_query
* ��������: ��ʾGUI �鿴��ǰHPLC������֪����״̬
* �������: uint8 ModuleID ����ģ�����(1����2)
* �������: ��
* �� �� ֵ: >=0:����״̬�� -1:����
*******************************************************************************/
int gui_offline_feel_query(uint8 ModuleID)
{
	CCOGUI_FMT_DEBUG("gui_offline_feel_query() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_offline_feel_query() ModuleID=%d �Ƿ�! \n", ModuleID);
		return -1;
	}

	return gGuiShowMessage[ModuleID-1].nOfflineFeelState;
}

/*******************************************************************************
* ��������: gui_refuse_report_query
* ��������: ��ʾGUI �鿴��ǰHPLC�ܾ��ڵ��ϱ�����״̬
* �������: uint8 ModuleID ����ģ�����(1����2)
* �������: ��
* �� �� ֵ: >=0:����״̬�� -1:����
*******************************************************************************/
int gui_refuse_report_query(uint8 ModuleID)
{
	CCOGUI_FMT_DEBUG("gui_refuse_report_query() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_refuse_report_query() ModuleID=%d �Ƿ�! \n", ModuleID);
		return -1;
	}

	return gGuiShowMessage[ModuleID-1].nRefuseReportState;
}

/*******************************************************************************
* ��������: gui_files_count_query
* ��������: ��ʾGUI �鿴��������
* �������: uint8 ModuleID ����ģ�����(1����2)
* �������: ��
* �� �� ֵ: >=0:���������� -1:����
*******************************************************************************/
int gui_files_count_query(uint8 ModuleID)
{
	CCOGUI_FMT_DEBUG("gui_files_count_query() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_files_count_query() ModuleID=%d �Ƿ�! \n", ModuleID);
		return -1;
	}

	return gAmrMeterFilesList[ModuleID-1].nMeterFilesNum;
}

/*******************************************************************************
* ��������: gui_single_files_query
* ��������: ��ʾGUI �鿴����������Ϣ
* �������: uint8 ModuleID ����ģ�����(1����2)
			uint16 index �����б������±�(��0��ʼ)
* �������: AMR_METER_FILES_INFO_T *FilesInfo ����������Ϣ
* �� �� ֵ: ����0����ȷ��С��0������
*******************************************************************************/
int gui_single_files_query(uint8 ModuleID, uint16 index, AMR_METER_FILES_INFO_T *FilesInfo)
{
	CCOGUI_FMT_DEBUG("gui_single_files_query() ModuleID=%d, index=%d \n", ModuleID, index);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_single_files_query() ModuleID=%d �Ƿ�! \n", ModuleID);
		return -1;
	}

	if(index >= gAmrMeterFilesList[ModuleID-1].nMeterFilesNum)
	{
		CCOGUI_FMT_DEBUG("gui_single_files_query() index=%d �Ƿ�! \n", index);
		return -1;
	}

	memcpy(FilesInfo, &gAmrMeterFilesList[ModuleID-1].stMeterFilesData[index], sizeof(AMR_METER_FILES_INFO_T));

	return 1;
}

/*******************************************************************************
* ��������: gui_restart_app
* ��������: ��ʾGUI �Ա�Ӧ���������
* �������: uint8 ModuleID ����ģ�����(1����2)
* �������: ��
* �� �� ֵ: ����0����ȷ��С��0������
*******************************************************************************/
int gui_restart_app(uint8 ModuleID)
{
	CCOGUI_FMT_DEBUG("gui_restart_app() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_restart_app() ModuleID=%d �Ƿ�! \n", ModuleID);
		return -1;
	}

	return 1;
}


/*******************************************************************************
* ��������: gui_hardware_init
* ��������: ��ʾGUI Ӳ����λ·��
* �������: uint8 ModuleID ����ģ�����(1����2)
* �������: ��
* �� �� ֵ: ����0����ȷ��С��0������
*******************************************************************************/
int gui_hardware_init(uint8 ModuleID)
{
	CCOGUI_FMT_DEBUG("gui_hardware_init() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_hardware_init() ModuleID=%d �Ƿ�! \n", ModuleID);
		return -1;
	}

	return 1;
}

/*******************************************************************************
* ��������: gui_param_init
* ��������: ��ʾGUI ��ʼ������
* �������: uint8 ModuleID ����ģ�����(1����2)
* �������: ��
* �� �� ֵ: ����0����ȷ��С��0������
*******************************************************************************/
int gui_param_init(uint8 ModuleID)
{
	CCOGUI_FMT_DEBUG("gui_param_init() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_param_init() ModuleID=%d �Ƿ�! \n", ModuleID);
		return -1;
	}

	return 1;
}

/*******************************************************************************
* ��������: gui_software_info_query
* ��������: ��ʾGUI ��ѯ����汾�ͷ�������
* �������: uint8 ModuleID ����ģ�����(1����2)
* �������: ��
* �� �� ֵ: ����0����ȷ��С��0������
*******************************************************************************/
int gui_software_info_query(uint8 ModuleID, MODULE_INFO_T *pModuleInfo)
{
	CCOGUI_FMT_DEBUG("gui_software_info_query() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_software_info_query() ModuleID=%d �Ƿ�! \n", ModuleID);
		return -1;
	}

	memcpy(pModuleInfo, &gModuleInfo[ModuleID-1], sizeof(MODULE_INFO_T));

	return 1;
}


