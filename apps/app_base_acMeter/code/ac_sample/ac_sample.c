/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2019-10-25
*********************************************************************
*/

#include "ac_sample.h"
#include "ac_sample698.h"
#include "ac_lib.h"
#include "ac_main.h"
#include "ac_commdrv.h"
#include "ac_mqtt.h"
#include "ac_save.h"
#include "ac_update.h"

/*******************************************************************************
* ȫ�ֱ���
*******************************************************************************/

FACTORY_VER_TMP_T g_tVer =
{
    "XXXX",      /**< ���̴���           */
    "XXXX",      /**< ����汾��         */
    "211022",    /**< ����汾����       */
    "DM01",      /**< Ӳ���汾��         */
    "200624",    /**< Ӳ���汾����       */
    "XXXX-18",   /**< ������չ��Ϣ       */
};

int32                    fd_ac = -1;              // ���

COMMON_DATA_TYPE         gCommonData;             // ������������
PRIVTE_MEASURE_TYPE      gPriMeasurePara;         // ˽�м�������
PRIVATE_DATA_TYPE        gPrivateData;            // ˽�����ݱ���
MEA_REAL_DATA_COM_TYPE   gMeaRealTimData;         // ����ʵʱ����

// ������ͳ�ƽ����Ĳ���������
TERM_SHARE_DATA_TYPE    TermSharedData;           // ���ɹ����ڴ����ݵ�����
SHARED_MEM_MARK_BYTE    SharedMemMark;            // ˢ�¹������ݱ��

//��ʷ����ͳ�Ʊ���
TERM_SHARE_DATA_TYPE    MidTermSharedData;          // (�м����)���ɹ����ڴ����ݵ�����
PRIVTE_MEASURE_TYPE     gMidPriMeasurePara;         // (�м����)˽�м�������
TERM_SHARE_DATA_TYPE    HisTermSharedData;          // (��ʷ)���ɹ����ڴ����ݵ�����
PRIVTE_MEASURE_TYPE     gHisPriMeasurePara;         // (��ʷ)˽�м�������

//��ʷ����ͳ�Ʊ���
UPDATE_FLAG_T           gUpdateFlag = {0};          //�ɼ������Ƿ�Խ���仯������־

//�л�������֮�������ٻ��汾��Ϣ��־
uint8                   gBaudRateChangedFlag = 0;  //�л������ʱ�־
uint8                   gCallVerAfterBaudRateChanged = 0;  //�л�������֮�������ٻ��汾��Ϣ��־

//���ɲ�����ʼ����־
uint8 g_nAcParamInitFlag = 0;  //���ɲ�����ʼ����־

//�������ݳ�ʼ����־
uint8 g_nAcDataInitFlag = 0;  //�������ݳ�ʼ����־

//����������ʼ����־
uint8 g_nAcDemandInitFlag = 0;  //����������ʼ����־

//����������ʼ����־
uint8 g_nPowerOffFlag = 0;     //�����־

//����ģ���ʱ��־
uint8 g_nAcSetDateFlag = 0;  //����ģ���ʱ��־(����)
uint8 g_nAcSetTimeFlag = 0;  //����ģ���ʱ��־(ʱ��)
uint8 g_nAcReadDemandPeriodFlag = 1;  //����ģ���ѯ����������ڱ�־
uint8 g_nAcReadSlipTimeFlag = 1;  //����ģ���ѯ����ʱ���־

uint8 g_nAcReadZoneNumFlag = 1;  //����ģ�鵱ǰ��/������ʱ����־ ����
uint8 g_nAcReadDayTableNumFlag = 1;  //����ģ�鵱ǰ��/��������ʱ�α��־ ����
uint8 g_nAcReadDayPeriNumFlag = 1;  //����ģ�鵱ǰ��/��������ʱ�α�־ ����
uint8 g_nAcReadPeriNumFlagArr[OOP_MAX_DAYPERIODS];//��ǰ��/��������ʱ�α�(���ý���ģ��)��־ ����
uint8 g_nAcReadCostRateNumFlag = 1;  //����ģ���������־ ����
uint8 g_nAcReadHolidayNumFlag = 1;  //����ģ�鹫����������־ ����

uint8 g_nAcReadCurrZoneFlag = 1;  //����ģ�鵱ǰ��ʱ����־
uint8 g_nAcReadAddiZoneFlag = 1;  //����ģ�鱸����ʱ����־
uint8 g_nAcReadCurrDayPeriFlag = 1;  //����ģ�鵱ǰ����ʱ�α�־
uint8 g_nAcReadAddiDayPeriFlag = 1;  //����ģ�鱸������ʱ�α�־
uint8 g_nAcReadCurrDayFlagArr[OOP_MAX_DAYPERIODS];//��ǰ����ʱ�α�(���ý���ģ��)��־
uint8 g_nAcReadAddiDayFlagArr[OOP_MAX_DAYPERIODS];//��������ʱ�α�(���ý���ģ��)��־

uint8 g_nRefreshCurrDayFlagArr[OOP_MAX_DAYPERIODS] = {0};//��ǰ����ʱ�α� ˢ�±�־
uint8 g_nRefreshAddiDayFlagArr[OOP_MAX_DAYPERIODS] = {0};//��������ʱ�α� ˢ�±�־

uint8 g_nAddiZoneChangeTimeFlag = 1;//������ʱ�����л�ʱ�� ��־ ��ѯ
uint8 g_nAddiPeriChangeTimeFlag = 1;//��������ʱ�α��л�ʱ�� ��־ ��ѯ
uint8 g_nActPowCombWordFlag = 1;//�й���Ϸ�ʽ������ ��־ ��ѯ
uint8 g_nReaPowComb1WordFlag = 1;//�޹���Ϸ�ʽ1������ ��־ ��ѯ
uint8 g_nReaPowComb2WordFlag = 1;//�޹���Ϸ�ʽ2������ ��־ ��ѯ

uint8 g_nReadCurrDayPeriLabelFlag = 1;//����ģ��Ŀǰ���е���ʱ�α��ǵڼ��� ��ѯ��־
uint8 g_nRefreshCurrDayPeriLabelFlag = 0;//����ģ��Ŀǰ���е���ʱ�α��ǵڼ��� ��ˢ�±�־

uint8 NO_DATA[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
extern uint8 gdelay;
/*******************************************************************************
* ����ģ��ʵʱ����
********************************************************************************/
const uint32 AMR_RT_TASK_ID_EX[] =
{
    /* ���� */
    0x0000ff00,             //(��ǰ)����й����ܿ�
    0x0001ff00,             //(��ǰ)�����й������ܼ��ƽ��ʾֵ
    0x00150000,             //(A��)�����й��ܵ���ʾֵ
    0x00290000,             //(B��)�����й��ܵ���ʾֵ
    0x003d0000,             //(C��)�����й��ܵ���ʾֵ
    0x0002ff00,             //(��ǰ)�����й������ܼ��ƽ��ʾֵ
    0x00160000,             //(A��)�����й��ܵ���ʾֵ
    0x002a0000,             //(B��)�����й��ܵ���ʾֵ
    0x003e0000,             //(C��)�����й��ܵ���ʾֵ
    0x0003ff00,             //(��ǰ)�����޹������ܼ��ƽ��ʾֵ
    0x00170000,             //(A��)�����޹��ܵ���ʾֵ
    0x002b0000,             //(B��)�����޹��ܵ���ʾֵ
    0x003f0000,             //(C��)�����޹��ܵ���ʾֵ
    0x0004ff00,             //(��ǰ)�����޹������ܼ��ƽ��ʾֵ
    0x00180000,             //(A��)�����޹��ܵ���ʾֵ
    0x002c0000,             //(B��)�����޹��ܵ���ʾֵ
    0x00400000,             //(C��)�����޹��ܵ���ʾֵ
    0x0005ff00,             //(��ǰ)��һ�����޹��ܼ��ƽ�ȵ���ʾֵ
    0x00190000,             //(A��)��һ�����޹�����
    0x002d0000,             //(B��)��һ�����޹�����
    0x00410000,             //(C��)��һ�����޹�����
    0x0006ff00,             //(��ǰ)�ڶ������޹��ܼ��ƽ�ȵ���ʾֵ
    0x001a0000,             //(A��)�ڶ������޹�����
    0x002e0000,             //(B��)�ڶ������޹�����
    0x00420000,             //(C��)�ڶ������޹�����
    0x0007ff00,             //(��ǰ)���������޹��ܼ��ƽ�ȵ���ʾֵ
    0x001b0000,             //(A��)���������޹�����
    0x002f0000,             //(B��)���������޹�����
    0x00430000,             //(C��)���������޹�����
    0x0008ff00,             //(��ǰ)���������޹��ܼ��ƽ�ȵ���ʾֵ
    0x001c0000,             //(A��)���������޹�����
    0x00300000,             //(B��)���������޹�����
    0x00440000,             //(C��)���������޹�����
    /*
    0x0009ff00,             //(��ǰ)�������ڵ������ݿ�(�ظ��쳣 33 32 3C 33 34 34 )
    0x001d0000,             //(A��)�������ڵ���(�ظ��쳣 33 33 50 33 34 34 )
    0x00310000,             //(B��)�������ڵ���(�ظ��쳣 33 33 64 33 34 34 )
    0x00450000,             //(C��)�������ڵ���(�ظ��쳣 33 33 78 33 34 34 )
    
    0x000aff00,             //(��ǰ)�������ڵ������ݿ�(�ظ��쳣 33 32 3D 33 35 33 33 )
    0x001e0000,             //(A��)�������ڵ���(�ظ��쳣 33 33 51 33 34 34 )
    0x00320000,             //(B��)�������ڵ���(�ظ��쳣 33 33 65 33 34 34 )
    0x00460000,             //(C��)�������ڵ���(�ظ��쳣 33 33 79 33 34 34 )
    */

    /* ����(�߾��� ��λС��) */
    0xf100ff00,             //(��ǰ)����й����ܿ�
    
    0xf101ff00,             //(��ǰ)�����й������ܼ��ƽ��ʾֵ
    0xf1150000,             //(A��)�����й��ܵ���ʾֵ
    0xf1290000,             //(B��)�����й��ܵ���ʾֵ
    0xf13d0000,             //(C��)�����й��ܵ���ʾֵ
    
    0xf102ff00,             //(��ǰ)�����й������ܼ��ƽ��ʾֵ
    0xf1160000,             //(A��)�����й��ܵ���ʾֵ
    0xf12a0000,             //(B��)�����й��ܵ���ʾֵ
    0xf13e0000,             //(C��)�����й��ܵ���ʾֵ
    
    0xf103ff00,             //(��ǰ)�����޹������ܼ��ƽ��ʾֵ
    0xf1170000,             //(A��)�����޹��ܵ���ʾֵ
    0xf12b0000,             //(B��)�����޹��ܵ���ʾֵ
    0xf13f0000,             //(C��)�����޹��ܵ���ʾֵ
    
    0xf104ff00,             //(��ǰ)�����޹������ܼ��ƽ��ʾֵ
    0xf1180000,             //(A��)�����޹��ܵ���ʾֵ
    0xf12c0000,             //(B��)�����޹��ܵ���ʾֵ
    0xf1400000,             //(C��)�����޹��ܵ���ʾֵ
    
    0xf105ff00,             //(��ǰ)��һ�����޹��ܼ��ƽ�ȵ���ʾֵ
    0xf1190000,             //(A��)��һ�����޹�����
    0xf12d0000,             //(B��)��һ�����޹�����
    0xf1410000,             //(C��)��һ�����޹�����
    
    0xf106ff00,             //(��ǰ)�ڶ������޹��ܼ��ƽ�ȵ���ʾֵ
    0xf11a0000,             //(A��)�ڶ������޹�����
    0xf12e0000,             //(B��)�ڶ������޹�����
    0xf1420000,             //(C��)�ڶ������޹�����
    
    0xf107ff00,             //(��ǰ)���������޹��ܼ��ƽ�ȵ���ʾֵ
    0xf11b0000,             //(A��)���������޹�����
    0xf12f0000,             //(B��)���������޹�����
    0xf1430000,             //(C��)���������޹�����
    
    0xf108ff00,             //(��ǰ)���������޹��ܼ��ƽ�ȵ���ʾֵ
    0xf11c0000,             //(A��)���������޹�����
    0xf1300000,             //(B��)���������޹�����
    0xf1440000,             //(C��)���������޹�����
    /*
    0xf109ff00,             //(��ǰ)�������ڵ������ݿ�(�ظ��쳣 33 32 3C 33 34 34 )
    0xf11d0000,             //(A��)�������ڵ���(�ظ��쳣 33 33 50 33 34 34 )
    0xf1310000,             //(B��)�������ڵ���(�ظ��쳣 33 33 64 33 34 34 )
    0xf1450000,             //(C��)�������ڵ���(�ظ��쳣 33 33 78 33 34 34 )
    
    0xf10aff00,             //(��ǰ)�������ڵ������ݿ�(�ظ��쳣 33 32 3D 33 35 33 33 )
    0xf11e0000,             //(A��)�������ڵ���(�ظ��쳣 33 33 51 33 34 34 )
    0xf1320000,             //(B��)�������ڵ���(�ظ��쳣 33 33 65 33 34 34 )
    0xf1460000,             //(C��)�������ڵ���(�ظ��쳣 33 33 79 33 34 34 )
    */
    
    /* ���� */
    0x0101ff00,             //(��ǰ)�����й��ܼ��ƽ���������
    0x01150000,             //(A��)�����й����������
    0x01290000,             //(B��)�����й����������
    0x013d0000,             //(C��)�����й����������
    0x0102ff00,             //(��ǰ)�����й��ܼ��ƽ���������
    0x01160000,             //(A��)�����й��������
    0x012a0000,             //(B��)�����й��������
    0x013e0000,             //(C��)�����й��������
    0x0103ff00,             //(��ǰ)�����޹��ܼ��ƽ���������
    0x01170000,             //(A��)����޹�1�������
    0x012b0000,             //(B��)����޹�1�������
    0x013f0000,             //(C��)����޹�1�������
    0x0104ff00,             //(��ǰ)�����޹��ܼ��ƽ���������
    0x01180000,             //(A��)����޹�2�������
    0x012c0000,             //(B��)����޹�2�������
    0x01400000,             //(C��)����޹�2�������
    0x0105FF00,             //(��ǰ)��һ������������ܼ��ƽ��
    0x01190000,             //(A��)��һ�����������
    0x012d0000,             //(B��)��һ�����������
    0x01410000,             //(C��)��һ�����������
    0x0106FF00,             //(��ǰ)�ڶ�������������ܼ��ƽ��
    0x011a0000,             //(A��)�ڶ������������
    0x012e0000,             //(B��)�ڶ������������
    0x01420000,             //(C��)�ڶ������������
    0x0107FF00,             //(��ǰ)����������������ܼ��ƽ��
    0x011b0000,             //(A��)���������������
    0x012f0000,             //(B��)���������������
    0x01430000,             //(C��)���������������
    0x0108FF00,             //(��ǰ)����������������ܼ��ƽ��
    0x011c0000,             //(A��)���������������
    0x01300000,             //(B��)���������������
    0x01440000,             //(C��)���������������
    0x0109ff00,             //(��ǰ)����������������ܼ��ƽ��
    0x011d0000,             //(A��)���������������
    0x01310000,             //(B��)���������������
    0x01450000,             //(C��)���������������
    0x010aff00,             //(��ǰ)����������������ܼ��ƽ��
    0x011e0000,             //(A��)���������������
    0x01320000,             //(B��)���������������
    0x01460000,             //(C��)���������������

    /* ʵʱ���� */
    0x0201ff00, 			 //(��ǰ)�����ѹ
    0x0202ff00, 			 //(��ǰ)�������
    0x02800001,              //�������
    
    0x028800FF,             //�����ѹ���������������
    0x0207FF00,              //��ѹ������� 
    0x0203ff00,              //(��ǰ)˲ʱ�����й�����
    0x0204ff00, 		     //(��ǰ)˲ʱ�����޹�����
    0x0205FF00,              //(��ǰ)˲ʱ�������ڹ���
    0x0206ff00,              //(��ǰ)˲ʱ�ܼ����๦������
    0x02800002,              //����Ƶ��
    0x04000501,             //�������״̬��1
    0x04000502,             //�������״̬��2
    0x04000503,             //�������״̬��3
    0x04000504,             //�������״̬��4
    0x04000505,             //�������״̬��5
    0x04000506,             //�������״̬��6
    0x04000507,             //�������״̬��7

    /* г�� */
    0x020A01FF,           	//A���ѹг�������ʣ��ܼ�2��n�Σ�
    0x020A02FF,           	//B���ѹг�������ʣ��ܼ�2��n�Σ�
    0x020A03FF,           	//C���ѹг�������ʣ��ܼ�2��n�Σ�
    0x020B01FF,           	//A�����г�������ʣ��ܼ�2��n�Σ�
    0x020B02FF,           	//B�����г�������ʣ��ܼ�2��n�Σ�
    0x020B03FF,           	//C�����г�������ʣ��ܼ�2��n�Σ�
    
    0x020C01FF,           	//A���ѹг���������ܼ�2��n�Σ�
    0x020C02FF,           	//B���ѹг���������ܼ�2��n�Σ�
    0x020C03FF,           	//C���ѹг���������ܼ�2��n�Σ�
    0x020D01FF,           	//A�����г���������ܼ�2��n�Σ�
    0x020D02FF,           	//B�����г���������ܼ�2��n�Σ�
    0x020D03FF,           	//C�����г���������ܼ�2��n�Σ�
};

uint16 gAmrRtTaskIdExNum = 0;  //AMR_RT_TASK_ID_EX��Ԫ�ظ���


/*******************************************************************************
* ����ģ��ʵʱ�ɼ�����(�޸ĺ�, ��Ӳɼ�����)
********************************************************************************/
const uint32 gSampleTaskEnergy[] =
{
    /* ���� */
    
    0x0000ff00,             //(��ǰ)����й����ܿ�
    
    0x0001ff00,             //(��ǰ)�����й������ܼ��ƽ��ʾֵ
    0x00150000,             //(A��)�����й��ܵ���ʾֵ
    0x00290000,             //(B��)�����й��ܵ���ʾֵ
    0x003d0000,             //(C��)�����й��ܵ���ʾֵ
    
    0x0002ff00,             //(��ǰ)�����й������ܼ��ƽ��ʾֵ
    0x00160000,             //(A��)�����й��ܵ���ʾֵ
    0x002a0000,             //(B��)�����й��ܵ���ʾֵ
    0x003e0000,             //(C��)�����й��ܵ���ʾֵ
    
    0x0003ff00,             //(��ǰ)�����޹������ܼ��ƽ��ʾֵ
    0x00170000,             //(A��)�����޹��ܵ���ʾֵ
    0x002b0000,             //(B��)�����޹��ܵ���ʾֵ
    0x003f0000,             //(C��)�����޹��ܵ���ʾֵ
    
    0x0004ff00,             //(��ǰ)�����޹������ܼ��ƽ��ʾֵ
    0x00180000,             //(A��)�����޹��ܵ���ʾֵ
    0x002c0000,             //(B��)�����޹��ܵ���ʾֵ
    0x00400000,             //(C��)�����޹��ܵ���ʾֵ
    
    0x0005ff00,             //(��ǰ)��һ�����޹��ܼ��ƽ�ȵ���ʾֵ
    0x00190000,             //(A��)��һ�����޹�����
    0x002d0000,             //(B��)��һ�����޹�����
    0x00410000,             //(C��)��һ�����޹�����
    
    0x0006ff00,             //(��ǰ)�ڶ������޹��ܼ��ƽ�ȵ���ʾֵ
    0x001a0000,             //(A��)�ڶ������޹�����
    0x002e0000,             //(B��)�ڶ������޹�����
    0x00420000,             //(C��)�ڶ������޹�����
    
    0x0007ff00,             //(��ǰ)���������޹��ܼ��ƽ�ȵ���ʾֵ
    0x001b0000,             //(A��)���������޹�����
    0x002f0000,             //(B��)���������޹�����
    0x00430000,             //(C��)���������޹�����
    
    0x0008ff00,             //(��ǰ)���������޹��ܼ��ƽ�ȵ���ʾֵ
    0x001c0000,             //(A��)���������޹�����
    0x00300000,             //(B��)���������޹�����
    0x00440000,             //(C��)���������޹�����
    
    /*
    0x0009ff00,             //(��ǰ)�������ڵ������ݿ�(�ظ��쳣 33 32 3C 33 34 34 )
    0x001d0000,             //(A��)�������ڵ���(�ظ��쳣 33 33 50 33 34 34 )
    0x00310000,             //(B��)�������ڵ���(�ظ��쳣 33 33 64 33 34 34 )
    0x00450000,             //(C��)�������ڵ���(�ظ��쳣 33 33 78 33 34 34 )
    
    0x000aff00,             //(��ǰ)�������ڵ������ݿ�(�ظ��쳣 33 32 3D 33 35 33 33 )
    0x001e0000,             //(A��)�������ڵ���(�ظ��쳣 33 33 51 33 34 34 )
    0x00320000,             //(B��)�������ڵ���(�ظ��쳣 33 33 65 33 34 34 )
    0x00460000,             //(C��)�������ڵ���(�ظ��쳣 33 33 79 33 34 34 )
    */

    /* ʵʱ���� */
    
    0x0201ff00, 			 //(��ǰ)�����ѹ
    0x0202ff00, 			 //(��ǰ)�������
    0x02800001,              //�������
    
0x028800FF,             //�����ѹ���������������
    0x0207FF00,              //��ѹ������� 
    0x0203ff00,              //(��ǰ)˲ʱ�����й�����
    0x0204ff00, 		     //(��ǰ)˲ʱ�����޹�����
    0x0205FF00,              //(��ǰ)˲ʱ�������ڹ���
    0x0206ff00,              //(��ǰ)˲ʱ�ܼ����๦������
    0x02800002,              //����Ƶ��
    0x04000501,             //�������״̬��1
    0x04000502,             //�������״̬��2
    0x04000503,             //�������״̬��3
    0x04000504,             //�������״̬��4
    0x04000505,             //�������״̬��5
    0x04000506,             //�������״̬��6
    0x04000507,             //�������״̬��7
    
#if HD_ENERGY_EN == 1
    /* ����(�߾��� ��λС��) */

    0xf100ff00,             //(��ǰ)����й����ܿ�
    
    0xf101ff00,             //(��ǰ)�����й������ܼ��ƽ��ʾֵ
    0xf1150000,             //(A��)�����й��ܵ���ʾֵ
    0xf1290000,             //(B��)�����й��ܵ���ʾֵ
    0xf13d0000,             //(C��)�����й��ܵ���ʾֵ
    
    0xf102ff00,             //(��ǰ)�����й������ܼ��ƽ��ʾֵ
    0xf1160000,             //(A��)�����й��ܵ���ʾֵ
    0xf12a0000,             //(B��)�����й��ܵ���ʾֵ
    0xf13e0000,             //(C��)�����й��ܵ���ʾֵ
    
    0xf103ff00,             //(��ǰ)�����޹������ܼ��ƽ��ʾֵ
    0xf1170000,             //(A��)�����޹��ܵ���ʾֵ
    0xf12b0000,             //(B��)�����޹��ܵ���ʾֵ
    0xf13f0000,             //(C��)�����޹��ܵ���ʾֵ
    
    0xf104ff00,             //(��ǰ)�����޹������ܼ��ƽ��ʾֵ
    0xf1180000,             //(A��)�����޹��ܵ���ʾֵ
    0xf12c0000,             //(B��)�����޹��ܵ���ʾֵ
    0xf1400000,             //(C��)�����޹��ܵ���ʾֵ
    
    0xf105ff00,             //(��ǰ)��һ�����޹��ܼ��ƽ�ȵ���ʾֵ
    0xf1190000,             //(A��)��һ�����޹�����
    0xf12d0000,             //(B��)��һ�����޹�����
    0xf1410000,             //(C��)��һ�����޹�����
    
    0xf106ff00,             //(��ǰ)�ڶ������޹��ܼ��ƽ�ȵ���ʾֵ
    0xf11a0000,             //(A��)�ڶ������޹�����
    0xf12e0000,             //(B��)�ڶ������޹�����
    0xf1420000,             //(C��)�ڶ������޹�����
    
    0xf107ff00,             //(��ǰ)���������޹��ܼ��ƽ�ȵ���ʾֵ
    0xf11b0000,             //(A��)���������޹�����
    0xf12f0000,             //(B��)���������޹�����
    0xf1430000,             //(C��)���������޹�����
    
    0xf108ff00,             //(��ǰ)���������޹��ܼ��ƽ�ȵ���ʾֵ
    0xf11c0000,             //(A��)���������޹�����
    0xf1300000,             //(B��)���������޹�����
    0xf1440000,             //(C��)���������޹�����

    /*
    0xf109ff00,             //(��ǰ)�������ڵ������ݿ�(�ظ��쳣 33 32 3C 33 34 34 )
    0xf11d0000,             //(A��)�������ڵ���(�ظ��쳣 33 33 50 33 34 34 )
    0xf1310000,             //(B��)�������ڵ���(�ظ��쳣 33 33 64 33 34 34 )
    0xf1450000,             //(C��)�������ڵ���(�ظ��쳣 33 33 78 33 34 34 )
    
    0xf10aff00,             //(��ǰ)�������ڵ������ݿ�(�ظ��쳣 33 32 3D 33 35 33 33 )
    0xf11e0000,             //(A��)�������ڵ���(�ظ��쳣 33 33 51 33 34 34 )
    0xf1320000,             //(B��)�������ڵ���(�ظ��쳣 33 33 65 33 34 34 )
    0xf1460000,             //(C��)�������ڵ���(�ظ��쳣 33 33 79 33 34 34 )
    */
#endif

};

uint16 gSampleTaskEnergyNum = 0;  //gSampleTaskEnergy��Ԫ�ظ���

/*******************************************************************************
* ����ģ��ʵʱ�ɼ�����(�޸ĺ�, ��Ӳɼ�����)
********************************************************************************/
const uint32 gSampleTaskDemand[] =
{
    /* ���� */
    0x0101ff00,             //(��ǰ)�����й��ܼ��ƽ���������
    0x01150000,             //(A��)�����й����������
    0x01290000,             //(B��)�����й����������
    0x013d0000,             //(C��)�����й����������
    
    0x0102ff00,             //(��ǰ)�����й��ܼ��ƽ���������
    0x01160000,             //(A��)�����й��������
    0x012a0000,             //(B��)�����й��������
    0x013e0000,             //(C��)�����й��������
    
    0x0103ff00,             //(��ǰ)�����޹��ܼ��ƽ���������
    0x01170000,             //(A��)����޹�1�������
    0x012b0000,             //(B��)����޹�1�������
    0x013f0000,             //(C��)����޹�1�������
    
    0x0104ff00,             //(��ǰ)�����޹��ܼ��ƽ���������
    0x01180000,             //(A��)����޹�2�������
    0x012c0000,             //(B��)����޹�2�������
    0x01400000,             //(C��)����޹�2�������
    
    0x0105FF00,             //(��ǰ)��һ������������ܼ��ƽ��
    0x01190000,             //(A��)��һ�����������
    0x012d0000,             //(B��)��һ�����������
    0x01410000,             //(C��)��һ�����������
    
    0x0106FF00,             //(��ǰ)�ڶ�������������ܼ��ƽ��
    0x011a0000,             //(A��)�ڶ������������
    0x012e0000,             //(B��)�ڶ������������
    0x01420000,             //(C��)�ڶ������������
    
    0x0107FF00,             //(��ǰ)����������������ܼ��ƽ��
    0x011b0000,             //(A��)���������������
    0x012f0000,             //(B��)���������������
    0x01430000,             //(C��)���������������
    
    0x0108FF00,             //(��ǰ)����������������ܼ��ƽ��
    0x011c0000,             //(A��)���������������
    0x01300000,             //(B��)���������������
    0x01440000,             //(C��)���������������
    /*
    0x0109ff00,             //(��ǰ)����������������ܼ��ƽ��
    0x011d0000,             //(A��)���������������(�ظ��쳣 33 33 50 34 34 34 )
    0x01310000,             //(B��)���������������(�ظ��쳣 33 33 64 34 34 34 )
    0x01450000,             //(C��)���������������(�ظ��쳣 33 33 78 34 34 34 )
    
    0x010aff00,             //(��ǰ)����������������ܼ��ƽ��(�ظ��쳣 33 32 3D 34 35 33 33 )
    0x011e0000,             //(A��)���������������(�ظ��쳣 33 33 51 34 34 34 )
    0x01320000,             //(B��)���������������(�ظ��쳣 33 33 65 34 34 34 )
    0x01460000,             //(C��)���������������(�ظ��쳣 33 33 79 34 34 34 )
    */

    /* ʵʱ���� */
    0x0201ff00, 			 //(��ǰ)�����ѹ
    0x0202ff00, 			 //(��ǰ)�������
    0x02800001,              //�������
    
0x028800FF,             //�����ѹ���������������
    0x0207FF00,              //��ѹ������� 
    0x0203ff00,              //(��ǰ)˲ʱ�����й�����
    0x0204ff00, 		     //(��ǰ)˲ʱ�����޹�����
    0x0205FF00,              //(��ǰ)˲ʱ�������ڹ���
    0x0206ff00,              //(��ǰ)˲ʱ�ܼ����๦������
    0x02800002,              //����Ƶ��
    0x04000501,             //�������״̬��1
    0x04000502,             //�������״̬��2
    0x04000503,             //�������״̬��3
    0x04000504,             //�������״̬��4
    0x04000505,             //�������״̬��5
    0x04000506,             //�������״̬��6
    0x04000507,             //�������״̬��7
};

uint16 gSampleTaskDemandNum = 0;  //gSampleTaskDemand��Ԫ�ظ���

/*******************************************************************************
* ����ģ��ʵʱ�ɼ�����(�޸ĺ�, ��Ӳɼ�����)
********************************************************************************/
const uint32 gSampleTaskRealtime[] =
{
    /* ʵʱ���� */
    0x0201ff00, 			 //(��ǰ)�����ѹ
    0x0202ff00, 			 //(��ǰ)�������
    0x02800001,              //�������
    
    0x028800FF,             //�����ѹ���������������
    0x0207FF00,              //��ѹ������� 
    0x0203ff00,              //(��ǰ)˲ʱ�����й�����
    0x0204ff00, 		     //(��ǰ)˲ʱ�����޹�����
    0x0205FF00,              //(��ǰ)˲ʱ�������ڹ���
    0x0206ff00,              //(��ǰ)˲ʱ�ܼ����๦������
    0x02800002,              //����Ƶ��
    0x04000501,             //�������״̬��1
    0x04000502,             //�������״̬��2
    0x04000503,             //�������״̬��3
    0x04000504,             //�������״̬��4
    0x04000505,             //�������״̬��5
    0x04000506,             //�������״̬��6
    0x04000507,             //�������״̬��7
};

uint16 gSampleTaskRealtimeNum = 0;  //gSampleTaskRealtime��Ԫ�ظ���

/*******************************************************************************
* ����ģ��ʵʱ�ɼ�����(�޸ĺ�, ��Ӳɼ�����)
********************************************************************************/
const uint32 gSampleTaskHarmonic[] =
{
    /* г�� */
    0x020A01FF,           	//A���ѹг�������ʣ��ܼ�2��n�Σ�
    0x020A02FF,           	//B���ѹг�������ʣ��ܼ�2��n�Σ�
    0x020A03FF,           	//C���ѹг�������ʣ��ܼ�2��n�Σ�
    0x020B01FF,           	//A�����г�������ʣ��ܼ�2��n�Σ�
    0x020B02FF,           	//B�����г�������ʣ��ܼ�2��n�Σ�
    0x020B03FF,           	//C�����г�������ʣ��ܼ�2��n�Σ�
    
    0x020C01FF,           	//A���ѹг���������ܼ�2��n�Σ�
    0x020C02FF,           	//B���ѹг���������ܼ�2��n�Σ�
    0x020C03FF,           	//C���ѹг���������ܼ�2��n�Σ�
    0x020D01FF,           	//A�����г���������ܼ�2��n�Σ�
    0x020D02FF,           	//B�����г���������ܼ�2��n�Σ�
    0x020D03FF,           	//C�����г���������ܼ�2��n�Σ�

    /* ʵʱ���� */
    0x0201ff00, 			 //(��ǰ)�����ѹ
    0x0202ff00, 			 //(��ǰ)�������
    0x02800001,              //�������
    
0x028800FF,             //�����ѹ���������������
    0x0207FF00,              //��ѹ������� 
    0x0203ff00,              //(��ǰ)˲ʱ�����й�����
    0x0204ff00, 		     //(��ǰ)˲ʱ�����޹�����
    0x0205FF00,              //(��ǰ)˲ʱ�������ڹ���
    0x0206ff00,              //(��ǰ)˲ʱ�ܼ����๦������
    0x02800002,              //����Ƶ��
    0x04000501,             //�������״̬��1
    0x04000502,             //�������״̬��2
    0x04000503,             //�������״̬��3
    0x04000504,             //�������״̬��4
    0x04000505,             //�������״̬��5
    0x04000506,             //�������״̬��6
    0x04000507,             //�������״̬��7
};

uint16 gSampleTaskHarmonicNum = 0;  //gSampleTaskHarmonic��Ԫ�ظ���


/*********************************************************************
����������
���������
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 MakeFrameReadACModule(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//У���
    uint16	CurIndex;							//����
    int 	i;
    int 	j;
    uint8	DI_Buf[4] = {0};
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x1C;
    PrtlData[9] = sizeof(AMR_RT_TASK_ID_EX)&0xFF;
    PrtlData[10] = (sizeof(AMR_RT_TASK_ID_EX)>>8)&0xFF;

    for(i = 0; i < (sizeof(AMR_RT_TASK_ID_EX)/4); i++)
    {
        memset(DI_Buf, 0, 4);
        memcpy(DI_Buf, (uint8*)&AMR_RT_TASK_ID_EX[i], 4);
        for(j = 0; j < 4; j++)
        {
            PrtlData[11 + i*4 + j] =  DI_Buf[j] + 0x33;
        }
    }

    CurIndex = PrtlData[9] + (PrtlData[10]<<8) + 11;

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
����������
���������
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 MakeFrameReadEnergy(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//У���
    uint16	CurIndex;							//����
    int 	i;
    int 	j;
    uint8	DI_Buf[4] = {0};
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x1C;
    PrtlData[9] = sizeof(gSampleTaskEnergy)&0xFF;
    PrtlData[10] = (sizeof(gSampleTaskEnergy)>>8)&0xFF;

    for(i = 0; i < (sizeof(gSampleTaskEnergy)/4); i++)
    {
        memset(DI_Buf, 0, 4);
        memcpy(DI_Buf, (uint8*)&gSampleTaskEnergy[i], 4);
        for(j = 0; j < 4; j++)
        {
            PrtlData[11 + i*4 + j] =  DI_Buf[j] + 0x33;
        }
    }

    CurIndex = PrtlData[9] + (PrtlData[10]<<8) + 11;

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
����������
���������
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 MakeFrameReadDemand(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//У���
    uint16	CurIndex;							//����
    int 	i;
    int 	j;
    uint8	DI_Buf[4] = {0};
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x1C;
    PrtlData[9] = sizeof(gSampleTaskDemand)&0xFF;
    PrtlData[10] = (sizeof(gSampleTaskDemand)>>8)&0xFF;

    for(i = 0; i < (sizeof(gSampleTaskDemand)/4); i++)
    {
        memset(DI_Buf, 0, 4);
        memcpy(DI_Buf, (uint8*)&gSampleTaskDemand[i], 4);
        for(j = 0; j < 4; j++)
        {
            PrtlData[11 + i*4 + j] =  DI_Buf[j] + 0x33;
        }
    }

    CurIndex = PrtlData[9] + (PrtlData[10]<<8) + 11;

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
����������
���������
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 MakeFrameReadRealtime(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//У���
    uint16	CurIndex;							//����
    int 	i;
    int 	j;
    uint8	DI_Buf[4] = {0};
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x1C;
    PrtlData[9] = sizeof(gSampleTaskRealtime)&0xFF;
    PrtlData[10] = (sizeof(gSampleTaskRealtime)>>8)&0xFF;

    for(i = 0; i < (sizeof(gSampleTaskRealtime)/4); i++)
    {
        memset(DI_Buf, 0, 4);
        memcpy(DI_Buf, (uint8*)&gSampleTaskRealtime[i], 4);
        for(j = 0; j < 4; j++)
        {
            PrtlData[11 + i*4 + j] =  DI_Buf[j] + 0x33;
        }
    }

    CurIndex = PrtlData[9] + (PrtlData[10]<<8) + 11;

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
����������
���������
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 MakeFrameReadHarmonic(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//У���
    uint16	CurIndex;							//����
    int 	i;
    int 	j;
    uint8	DI_Buf[4] = {0};
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x1C;
    PrtlData[9] = sizeof(gSampleTaskHarmonic)&0xFF;
    PrtlData[10] = (sizeof(gSampleTaskHarmonic)>>8)&0xFF;

    for(i = 0; i < (sizeof(gSampleTaskHarmonic)/4); i++)
    {
        memset(DI_Buf, 0, 4);
        memcpy(DI_Buf, (uint8*)&gSampleTaskHarmonic[i], 4);
        for(j = 0; j < 4; j++)
        {
            PrtlData[11 + i*4 + j] =  DI_Buf[j] + 0x33;
        }
    }

    CurIndex = PrtlData[9] + (PrtlData[10]<<8) + 11;

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}


int STDAC_CheckRspFrame(uint8 *pBuf, uint16 bufLen)
{
    uint8	frameCs;
    uint16	frameLen;
    uint16	DataLen = 0;


    if((pBuf[8] & 0xF0) != 0x90)
    {
        return -4;
    }

    DataLen = pBuf[9] + (pBuf[10]<<8);
    frameLen = 0x0d + DataLen;
    if (frameLen > bufLen)
    {
        ACCOMM_FMT_LOGCC("frameLen=%d, bufLen=%d \n", frameLen, bufLen);
        return -1;
    }
    
    frameCs = CalCs(pBuf, frameLen - 2);

    if((pBuf[0] == 0x68)
            && (pBuf[7] == pBuf[0]) /*-����ϵͳ����ַ��0��AA�ɱ�-*/
            && (pBuf[frameLen - 2] == frameCs)
            && (pBuf[frameLen - 1] == 0x16))
    {
        if(frameLen >= 0x0E)
        {
            return 1;
        }
    }
    else
    {
        return -2;
    }

    return -3;
}

int STDAC_CheckRspFrame_Single(uint8 *pBuf, uint16 bufLen)
{
    uint8	frameCs;
    uint16	frameLen;
    uint16	DataLen = 0;

    //��������ģ�������ϱ�����, �����жϿ�����
    /*
    if((pBuf[8] & 0xF0) != 0x90)
    {
        return 0;
    }
    */

    DataLen = pBuf[9];
    frameLen = 8+2+2 + DataLen;

    if (frameLen > bufLen)
    {
        ACCOMM_FMT_LOGCC("frameLen=%d, bufLen=%d \n", frameLen, bufLen);
        return -1;
    }
    
    frameCs = CalCs(pBuf, frameLen - 2);

    if((pBuf[0] == 0x68)
            && (pBuf[7] == pBuf[0])
            && (pBuf[frameLen - 2] == frameCs)
            && (pBuf[frameLen - 1] == 0x16))
    {
        if(frameLen >= 0x0D)
        {
            return 1;
        }
    }
    else
    {
        return -2;
    }

    return -3;
}

/*******************************************************************************
* ��������: ac_curtime_get
* ��������: ʱ������ʼ��
* �������: ��
* �������: ��   
* �� �� ֵ: ��
*******************************************************************************/
void ac_curtime_get(YYMMDDWWhhnnss_TYPE *curtime)
{
    time_t now;
    struct tm *systime = NULL;
    
    time(&now);
    systime = localtime(&now);          // �ṹԪ��tm_sec,tm_min,tm_hour,tm_mday,tm_mon,tm_year

    curtime->ss = Hex2Bcd((uint8)systime->tm_sec);
    curtime->nn = Hex2Bcd((uint8)systime->tm_min);
    curtime->hh = Hex2Bcd((uint8)systime->tm_hour);
    curtime->DD = Hex2Bcd((uint8)systime->tm_mday);            
    curtime->WW = Hex2Bcd((uint8)systime->tm_wday);         // ����
    curtime->MM = Hex2Bcd((uint8)systime->tm_mon + 1);      // �·�0����1��
    curtime->YY = Hex2Bcd((uint8)(systime->tm_year - 100)); // ȡ��ݵĺ���λ

    return;
}

/*********************************************************************
���������� make_frame_ac_date
�����������֡ ����ģ��Уʱ(����)
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_ac_date(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//У���
    uint16	CurIndex;							//����
    int 	i;
    uint8   MeterAddr[6];
    YYMMDDWWhhnnss_TYPE curtime;

    ac_curtime_get(&curtime);    
    //SaveTimeManageInit();
    //TimeManageInit();

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x10;

    PrtlData[10] = 0x01;
    PrtlData[11] = 0x01;
    PrtlData[12] = 0x00;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = curtime.WW;
    PrtlData[23] = curtime.DD;
    PrtlData[24] = curtime.MM;
    PrtlData[25] = curtime.YY;

    CurIndex = 26;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
���������� make_frame_ac_time
�����������֡ ����ģ��Уʱ(ʱ��)
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_ac_time(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//У���
    uint16	CurIndex;							//����
    int 	i;
    uint8   MeterAddr[6];
    YYMMDDWWhhnnss_TYPE curtime;

    ac_curtime_get(&curtime);
    //SaveTimeManageInit();
    //TimeManageInit();

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x0F;

    PrtlData[10] = 0x02;
    PrtlData[11] = 0x01;
    PrtlData[12] = 0x00;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = curtime.ss;
    PrtlData[23] = curtime.nn;
    PrtlData[24] = curtime.hh;

    CurIndex = 25;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
���������� make_frame_ac_date_time
�����������֡ ����ģ��Уʱ
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_ac_date_time(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//У���
    uint16	CurIndex;							//����
    int 	i;
    uint8   MeterAddr[6];
    YYMMDDWWhhnnss_TYPE curtime;

    ac_curtime_get(&curtime); 

//    SaveTimeManageInit();
//    TimeManageInit();

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x13;

    PrtlData[10] = 0x0C;
    PrtlData[11] = 0x01;
    PrtlData[12] = 0x00;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = curtime.ss;
    PrtlData[23] = curtime.nn;
    PrtlData[24] = curtime.hh;
    PrtlData[25] = curtime.WW;
    PrtlData[26] = curtime.DD;
    PrtlData[27] = curtime.MM;
    PrtlData[28] = curtime.YY;

    CurIndex = 29;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
���������� make_frame_master_date
�����������֡ ����ģ��Уʱ(��վ�����·�������)
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_master_date(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//У���
    uint16	CurIndex;							//����
    int 	i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x10;

    PrtlData[10] = 0x01;
    PrtlData[11] = 0x01;
    PrtlData[12] = 0x00;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = Hex2Bcd(TermSharedData.DateTimeSet.week);
    PrtlData[23] = Hex2Bcd(TermSharedData.DateTimeSet.day);
    PrtlData[24] = Hex2Bcd(TermSharedData.DateTimeSet.month);
    PrtlData[25] = Hex2Bcd((uint8)(TermSharedData.DateTimeSet.year - 2000));

    CurIndex = 26;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}


/*********************************************************************
���������� make_frame_master_time
�����������֡ ����ģ��Уʱ(��վ�·�Уʱʱ��)
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_master_time(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//У���
    uint16	CurIndex;							//����
    int 	i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x0F;

    PrtlData[10] = 0x02;
    PrtlData[11] = 0x01;
    PrtlData[12] = 0x00;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = Hex2Bcd(TermSharedData.DateTimeSet.second);
    PrtlData[23] = Hex2Bcd(TermSharedData.DateTimeSet.minute);
    PrtlData[24] = Hex2Bcd(TermSharedData.DateTimeSet.hour);

    CurIndex = 25;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
���������� make_frame_zone_table_change_time
�����������֡ ʱ�����л�ʱ��
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_zone_table_change_time(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//У���
    uint16	CurIndex;							//����
    int 	i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x11;

    PrtlData[10] = 0x06;
    PrtlData[11] = 0x01;
    PrtlData[12] = 0x00;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = Hex2Bcd(TermSharedData.AddiZoneChangeTimeSet.minute);
    PrtlData[23] = Hex2Bcd(TermSharedData.AddiZoneChangeTimeSet.hour);
    PrtlData[24] = Hex2Bcd(TermSharedData.AddiZoneChangeTimeSet.day);
    PrtlData[25] = Hex2Bcd(TermSharedData.AddiZoneChangeTimeSet.month);
    PrtlData[26] = Hex2Bcd((uint8)(TermSharedData.AddiZoneChangeTimeSet.year - 2000));

    CurIndex = 27;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
���������� make_frame_peri_table_change_time
�����������֡ ��ʱ�α��л�ʱ��
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_peri_table_change_time(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//У���
    uint16	CurIndex;							//����
    int 	i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x11;

    PrtlData[10] = 0x07;
    PrtlData[11] = 0x01;
    PrtlData[12] = 0x00;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = Hex2Bcd(TermSharedData.AddiPeriChangeTimeSet.minute);
    PrtlData[23] = Hex2Bcd(TermSharedData.AddiPeriChangeTimeSet.hour);
    PrtlData[24] = Hex2Bcd(TermSharedData.AddiPeriChangeTimeSet.day);
    PrtlData[25] = Hex2Bcd(TermSharedData.AddiPeriChangeTimeSet.month);
    PrtlData[26] = Hex2Bcd((uint8)(TermSharedData.AddiPeriChangeTimeSet.year - 2000));

    CurIndex = 27;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}


/*********************************************************************
���������� make_frame_init_demand
�����������֡ ��ʼ����������
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_init_demand(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//У���
    uint16	CurIndex;							//����
    int 	i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x19;
    PrtlData[9] = 0x08;

    PrtlData[10] = 0x00;
    PrtlData[11] = 0x56;
    PrtlData[12] = 0x34;
    PrtlData[13] = 0x12;
    PrtlData[14] = 0x99;
    PrtlData[15] = 0x66;
    PrtlData[16] = 0x88;
    PrtlData[17] = 0x00;

    CurIndex = 18;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}


/*********************************************************************
���������� make_frame_init_data
�����������֡ ��ʼ����������
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_init_data(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//У���
    uint16	CurIndex;							//����
    int 	i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x1A;
    PrtlData[9] = 0x08;

    PrtlData[10] = 0x00;
    PrtlData[11] = 0x56;
    PrtlData[12] = 0x34;
    PrtlData[13] = 0x12;
    PrtlData[14] = 0x99;
    PrtlData[15] = 0x66;
    PrtlData[16] = 0x88;
    PrtlData[17] = 0x00;

    CurIndex = 18;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}


/*********************************************************************
���������� make_frame_init_param
�����������֡ ��ʼ�����ɲ���
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_init_param(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//У���
    uint16	CurIndex;							//����
    int 	i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x18;
    PrtlData[9] = 0x05;

    PrtlData[10] = 0x05;
    PrtlData[11] = 0x70;
    PrtlData[12] = 0x86;
    PrtlData[13] = 0x61;
    PrtlData[14] = 0x47;

    CurIndex = 15;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
���������� make_frame_set_demand_period
�����������֡ ���������������
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_set_demand_period(uint8 *PrtlData)
{
    uint8   CheckSum = 0;                       //У���
    uint16  CurIndex;                           //����
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x0D;

    PrtlData[10] = 0x03;
    PrtlData[11] = 0x01;
    PrtlData[12] = 0x00;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = Hex2Bcd(TermSharedData.MaxDemandPeriodSet);

    CurIndex = 23;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
���������� make_frame_set_slip_time
�����������֡ ���û���ʱ��
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_set_slip_time(uint8 *PrtlData)
{
    uint8   CheckSum = 0;                       //У���
    uint16  CurIndex;                           //����
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x0D;

    PrtlData[10] = 0x04;
    PrtlData[11] = 0x01;
    PrtlData[12] = 0x00;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = Hex2Bcd(TermSharedData.SlipDiffTimeSet);

    CurIndex = 23;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
���������� make_frame_set_zone_peri_num
�����������֡ ���õ�ǰ��ʱ��/ʱ����
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_set_zone_peri_num(uint8 *PrtlData, uint32 DI, uint8 Num)
{
    uint8   CheckSum = 0;                       //У���
    uint16  CurIndex;                           //����
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x0D;

    memcpy(&PrtlData[10], &DI, 4);//���ݱ�ʶ

    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = Hex2Bcd(Num);
    
    CurIndex = 23;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
���������� make_frame_set_holiday_num
�����������֡ ���ù���������
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_set_holiday_num(uint8 *PrtlData, uint32 DI, uint8 Num)
{
    uint8   CheckSum = 0;                       //У���
    uint16  CurIndex;                           //����
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x0E;

    memcpy(&PrtlData[10], &DI, 4);//���ݱ�ʶ

    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = Hex2Bcd(Num);
    PrtlData[23] = 0x00;
    
    CurIndex = 24;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}



/*********************************************************************
���������� make_frame_set_curr_zone
�����������֡ ���õ�ǰ��ʱ����
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_set_curr_zone(uint8 *PrtlData)
{
    uint8   CheckSum = 0;                       //У���
    uint16  CurIndex;                           //����
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    //PrtlData[9] = 0x36;

    PrtlData[10] = 0x00;
    PrtlData[11] = 0x00;
    PrtlData[12] = 0x01;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    CurIndex = 22;
    for(i=0; i<TermSharedData.CurrZoneTableSet.nNum; i++)
    {
        if(i == OOP_MAX_TIMEZONE)
            break;
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.CurrZoneTableSet.period[i].meterNum);
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.CurrZoneTableSet.period[i].day);
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.CurrZoneTableSet.period[i].mon);
    }
    PrtlData[9] = CurIndex - 10;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
���������� make_frame_set_addi_zone
�����������֡ ���ñ�����ʱ����
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_set_addi_zone(uint8 *PrtlData)
{
    uint8   CheckSum = 0;                       //У���
    uint16  CurIndex;                           //����
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    //PrtlData[9] = 0x36;

    PrtlData[10] = 0x00;
    PrtlData[11] = 0x00;
    PrtlData[12] = 0x02;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    CurIndex = 22;
    for(i=0; i<TermSharedData.AddiZoneTableSet.nNum; i++)
    {
        if(i == OOP_MAX_TIMEZONE)
            break;
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.AddiZoneTableSet.period[i].meterNum);
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.AddiZoneTableSet.period[i].day);
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.AddiZoneTableSet.period[i].mon);
    }
    PrtlData[9] = CurIndex - 10;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
���������� make_frame_set_curr_day_period
�����������֡ ���õ�ǰ��ʱ�α�
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_set_curr_day_period(uint8 *PrtlData, uint8 DayIndex)
{
    uint8   CheckSum = 0;                       //У���
    uint16  CurIndex;                           //����
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    //PrtlData[9] = 0x36;

    PrtlData[10] = DayIndex + 1;
    PrtlData[11] = 0x00;
    PrtlData[12] = 0x01;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    CurIndex = 22;
    for(i=0; i<TermSharedData.CurrDayPeriTableSet.dPeriod[DayIndex].nNum; i++)
    {
        if(i == OOP_MAX_DAYPERIOD)
            break;
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.CurrDayPeriTableSet.dPeriod[DayIndex].period[i].rateNO);
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.CurrDayPeriTableSet.dPeriod[DayIndex].period[i].min);
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.CurrDayPeriTableSet.dPeriod[DayIndex].period[i].hour);
    }
    PrtlData[9] = CurIndex - 10;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
���������� make_frame_set_addi_day_period
�����������֡ ���ñ�����ʱ�α�
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_set_addi_day_period(uint8 *PrtlData, uint8 DayIndex)
{
    uint8   CheckSum = 0;                       //У���
    uint16  CurIndex;                           //����
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    //PrtlData[9] = 0x36;

    PrtlData[10] = DayIndex + 1;
    PrtlData[11] = 0x00;
    PrtlData[12] = 0x02;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    CurIndex = 22;
    for(i=0; i<TermSharedData.AddiDayPeriTableSet.dPeriod[DayIndex].nNum; i++)
    {
        if(i == OOP_MAX_DAYPERIOD)
            break;
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.AddiDayPeriTableSet.dPeriod[DayIndex].period[i].rateNO);
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.AddiDayPeriTableSet.dPeriod[DayIndex].period[i].min);
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.AddiDayPeriTableSet.dPeriod[DayIndex].period[i].hour);
    }
    PrtlData[9] = CurIndex - 10;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
���������� make_frame_set_p_q_word
�����������֡ �����й�/�޹�״̬��
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_set_p_q_word(uint8 *PrtlData, uint32 DI, uint8 Num)
{
    uint8   CheckSum = 0;                       //У���
    uint16  CurIndex;                           //����
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x0D;

    memcpy(&PrtlData[10], &DI, 4);//���ݱ�ʶ

    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = Num;//״̬�ֲ���Ҫתbcd��
    
    CurIndex = 23;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
���������� make_frame_query_para
�����������֡ ��ѯ���ɲ���
���������
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_query_para(uint8 *PrtlData, uint32 DataIdentify)
{
    uint8   CheckSum = 0;                       //У���
    uint16  CurIndex;                           //����
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x11;
    PrtlData[9] = 0x04;

    memcpy(&PrtlData[10], &DataIdentify, 4);//���ݱ�ʶ

    CurIndex = 14;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
���������� make_frame_query_para
�����������֡ ��ѯ���ɲ���
            uint8 nInLen DI֮�󳤶�
            uint8 *inData DI֮��У����֮ǰ������������������
���������  uint8 *outData  ������ȷ�ϱ���
��������ֵ��send_len
�ڲ��������̣�
********************************************************************/
int32 make_frame_confirm(uint8 *inData, uint8 nInLen, uint32	DataIdentify, uint8 *outData)
{
    uint8   CheckSum = 0;                       //У���
    uint16  CurIndex;                           //����
    int     i;
    uint8   MeterAddr[6];
    

    memset(MeterAddr, 0xAA, 6);

    outData[0] = 0x68;
    memcpy(&outData[1], MeterAddr, 6);
    outData[7] = 0x68;

    outData[8] = 0x91;
    outData[9] = 4 + nInLen;

    memcpy(&outData[10], &DataIdentify, 4);//���ݱ�ʶ
    memcpy(&outData[14], inData, nInLen);//��������

    CurIndex = 14 + nInLen;

    for(i=10; i<CurIndex; i++)
    {
        outData[i] += 0x33;
    }

    for(i=0; i<CurIndex; i++)
    {
        CheckSum += outData[i];
    }
    outData[CurIndex] = CheckSum;
    outData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*******************************************************************************
* ��������: ac_demand_initial
* ��������: ����������ʼ��
* �������:
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void ac_demand_initial(MSG_INFO_T *ptMsgInfo)
{
    uint16 nPayloadLen = 0;
    uint8  aPayloadBuff[100] = {0};

    //�ý��ɳ�ʼ����־
    g_nAcDemandInitFlag = 1;

    //��װ�϶�Ӧ��
    nPayloadLen = 1;
    aPayloadBuff[0] = 0x01;
    make_resp_info(ptMsgInfo, nPayloadLen, aPayloadBuff);
}

/*******************************************************************************
* ��������: ac_system_deal
* ��������: ִ��ϵͳָ��
* �������:
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void ac_system_deal(MSG_INFO_T *ptMsgInfo)
{
    uint16 nPayloadLen = 0;
    uint8  aPayloadBuff[100] = {0};
    uint16 offset = 0;//ƫ��λ
    uint8  nCount = 0;//ָ�����
    char  aCharArr[500] = {0};//ָ������
    uint16  nCharLen = 0;//ָ�����ݳ���
    int32 result = 0;
    int i = 0;

    //ָ�����
    nCount = ptMsgInfo->MsgData[offset];
    offset++;
    ACMQTT_FMT_DEBUG("ac_system_deal() ָ������nCount=%d, offset=%d \n", nCount, offset);

    //��װӦ��(seq of)
    aPayloadBuff[nPayloadLen] = nCount;
    nPayloadLen += 1;

    //����ָ������
    for(i=0; i<nCount; i++)
    {
        nCharLen = 0;
        BuffToChangeLen(&ptMsgInfo->MsgData[offset], &offset, &nCharLen);
        memset(aCharArr, 0x00, sizeof(aCharArr));
        strncpy(aCharArr,(char *)&ptMsgInfo->MsgData[offset], nCharLen > sizeof(aCharArr) ? sizeof(aCharArr) : nCharLen);
        result = system(aCharArr);
        offset += nCharLen;
        ACMQTT_FMT_DEBUG("ac_system_deal() i=%d, nCharLen=%d, aCharArr=%s, offset=%d result=%d\n", 
            i, nCharLen, aCharArr, offset, result);

        //��װӦ��
        memcpy_r(&aPayloadBuff[nPayloadLen], &result, 4);
        nPayloadLen += 4;	
    }

    //��װӦ��
    make_resp_info(ptMsgInfo, nPayloadLen, aPayloadBuff);
}


/*******************************************************************************
* ��������: ac_data_initial
* ��������: �������ݳ�ʼ��
* �������:
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void ac_data_initial(MSG_INFO_T *ptMsgInfo)
{
    uint16 nPayloadLen = 0;
    uint8  aPayloadBuff[100] = {0};

    //�ý��ɳ�ʼ����־
    g_nAcDataInitFlag = 1;

    //��װ�϶�Ӧ��
    nPayloadLen = 1;
    aPayloadBuff[0] = 0x01;
    make_resp_info(ptMsgInfo, nPayloadLen, aPayloadBuff);
}

/*******************************************************************************
* ��������: ac_param_initial
* ��������: ���ɲ�����ʼ��
* �������:
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void ac_param_initial(MSG_INFO_T *ptMsgInfo)
{
    uint16 nPayloadLen = 0;
    uint8  aPayloadBuff[100] = {0};

    //�ý��ɳ�ʼ����־
    g_nAcParamInitFlag = 1;

    //��װ�϶�Ӧ��
    nPayloadLen = 1;
    aPayloadBuff[0] = 0x01;
    make_resp_info(ptMsgInfo, nPayloadLen, aPayloadBuff);
}

/*******************************************************************************
* ��������: check_data_initial_result
* ��������: ��齻�����ݳ�ʼ�����
* �������:
* �������: ��
* �� �� ֵ: 1:�ɹ�; -1:ʧ��
*******************************************************************************/
int check_data_initial_result(uint8 *buff, int32 length)
{
    int ret = -1;
    int i = 0;
    int startIndex = 0;

    for(i=0; i<length; i++)
    {
        if(buff[i] == 0x68)
        {
            startIndex = i;
            break;
        }
    }

    if(startIndex+9 < length)
    {
        if(0x00 == buff[startIndex+9])
        {
            ret = 1;
        }
    }

    return ret;
}

/*******************************************************************************
* ��������: check_param_initial_result
* ��������: ��齻�����ݳ�ʼ�����
* �������:
* �������: ��
* �� �� ֵ: 1:�ɹ�; -1:ʧ��
*******************************************************************************/
int check_param_initial_result(uint8 *buff, int32 length)
{
    int ret = -1;
    int i = 0;
    int startIndex = 0;

    for(i=0; i<length; i++)
    {
        if(buff[i] == 0x68)
        {
            startIndex = i;
            break;
        }
    }

    if(startIndex+9 < length)
    {
        if(0x00 == buff[startIndex+9])
        {
            ret = 1;
        }
    }

    return ret;
}

/*******************************************************************************
* ��������: check_demand_initial_result
* ��������: ����������ݳ�ʼ�����
* �������:
* �������: ��
* �� �� ֵ: 1:�ɹ�; -1:ʧ��
*******************************************************************************/
int check_demand_initial_result(uint8 *buff, int32 length)
{
    int ret = -1;
    int i = 0;
    int startIndex = 0;

    for(i=0; i<length; i++)
    {
        if(buff[i] == 0x68)
        {
            startIndex = i;
            break;
        }
    }

    if(startIndex+9 < length)
    {
        if(0x00 == buff[startIndex+9])
        {
            ret = 1;
        }
    }

    return ret;
}

/*******************************************************************************
* ��������: check_timing_result
* ��������: ���Уʱ���
* �������:
* �������: ��
* �� �� ֵ: 1:�ɹ�; -1:ʧ��
*******************************************************************************/
int check_timing_result(uint8 *buff, int32 length)
{
    int ret = -1;
    int i = 0;
    int startIndex = 0;

    for(i=0; i<length; i++)
    {
        if(buff[i] == 0x68)
        {
            startIndex = i;
            break;
        }
    }

    if(startIndex+9 < length)
    {
        if(0x00 == buff[startIndex+9])
        {
            ret = 1;
        }
    }

    return ret;
}

/*******************************************************************************
* ��������: check_set_demand_period_result
* ��������: �����������������ڽ��
* �������:
* �������: ��
* �� �� ֵ: 1:�ɹ�; -1:ʧ��
*******************************************************************************/
int check_set_demand_period_result(uint8 *buff, int32 length)
{
    int ret = -1;
    int i = 0;
    int startIndex = 0;

    for(i=0; i<length; i++)
    {
        if(buff[i] == 0x68)
        {
            startIndex = i;
            break;
        }
    }

    if(startIndex+9 < length)
    {
        if(0x00 == buff[startIndex+9])
        {
            ret = 1;
        }
    }

    return ret;
}

/*******************************************************************************
* ��������: check_set_slip_time_result
* ��������: ������û���ʱ����
* �������:
* �������: ��
* �� �� ֵ: 1:�ɹ�; -1:ʧ��
*******************************************************************************/
int check_set_slip_time_result(uint8 *buff, int32 length)
{
    int ret = -1;
    int i = 0;
    int startIndex = 0;

    for(i=0; i<length; i++)
    {
        if(buff[i] == 0x68)
        {
            startIndex = i;
            break;
        }
    }

    if(startIndex+9 < length)
    {
        if(0x00 == buff[startIndex+9])
        {
            ret = 1;
        }
    }

    return ret;
}

/*******************************************************************************
* ��������: check_set_zone_result
* ��������: ���ʱ�����
* �������:
* �������: ��
* �� �� ֵ: 1:�ɹ�; -1:ʧ��
*******************************************************************************/
int check_set_zone_result(uint8 *buff, int32 length)
{
    int ret = -1;
    int i = 0;
    int startIndex = 0;

    for(i=0; i<length; i++)
    {
        if(buff[i] == 0x68)
        {
            startIndex = i;
            break;
        }
    }

    if(startIndex+9 < length)
    {
        if(0x00 == buff[startIndex+9])
        {
            ret = 1;
        }
    }

    return ret;
}

/*******************************************************************************
* ��������: check_set_day_period_result
* ��������: ���������ʱ�ν��
* �������:
* �������: ��
* �� �� ֵ: 1:�ɹ�; -1:ʧ��
*******************************************************************************/
int check_set_day_period_result(uint8 *buff, int32 length)
{
    int ret = -1;
    int i = 0;
    int startIndex = 0;

    for(i=0; i<length; i++)
    {
        if(buff[i] == 0x68)
        {
            startIndex = i;
            break;
        }
    }

    if(startIndex+9 < length)
    {
        if(0x00 == buff[startIndex+9])
        {
            ret = 1;
        }
    }

    return ret;
}

/*******************************************************************************
* ��������: check_set_para_result
* ��������: ������ò������
* �������:
* �������: ��
* �� �� ֵ: 1:�ɹ�; -1:ʧ��
*******************************************************************************/
int check_set_para_result(uint8 *buff, int32 length)
{
    int ret = -1;
    int i = 0;
    int startIndex = 0;

    for(i=0; i<length; i++)
    {
        if(buff[i] == 0x68)
        {
            startIndex = i;
            break;
        }
    }

    if(startIndex+9 < length)
    {
        if(0x00 == buff[startIndex+9])
        {
            ret = 1;
        }
    }

    return ret;
}


/*******************************************************************************
* ��������: SaveDemand07
* ��������: �洢�������������ʱ������
* �������:
*           DataLen             ���ݳ���
*           DataBuff            ��������
*           EnergySort          �������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void SaveDemand07(uint8 DataLen, uint8 *DataBuff, ENERGY_SORT_T EnergySort)
{
    uint8	SrcData[10] = {0};
    uint8	TempBuff[10] = {0};
    uint8	i = 0, index = 0;
    uint8   X;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    switch (EnergySort)
    {
    case ES_POS_ACT:						//�����й�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_ACT:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.CurrDemand[0][i].DemNum, 0xff, 3);
                memset(TermSharedData.CurrDemandTime[0][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.CurrDemand[0][i].DemNum, SrcData, 3);
                TermSharedData.CurrDemand[0][i].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.CurrDemandTime[0][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.CurrDemandTime[0][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.CurrDemand[0][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.CurrDemand[0][i].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.CurrDemandTime[0][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.CurrDemandTime[0][i].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_NEG_ACT:						//�����й�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_ACT:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.CurrDemand[1][i].DemNum, 0xff, 3);
                memset(TermSharedData.CurrDemandTime[1][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.CurrDemand[1][i].DemNum, SrcData, 3);
                TermSharedData.CurrDemand[1][i].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.CurrDemandTime[1][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.CurrDemandTime[1][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.CurrDemand[1][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.CurrDemand[1][i].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.CurrDemandTime[1][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.CurrDemandTime[1][i].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_POS_REA:						//�����޹�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_REA:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.CurrDemand[2][i].DemNum, 0xff, 3);
                memset(TermSharedData.CurrDemandTime[2][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.CurrDemand[2][i].DemNum, SrcData, 3);
                TermSharedData.CurrDemand[2][i].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.CurrDemandTime[2][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.CurrDemandTime[2][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.CurrDemand[2][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.CurrDemand[2][i].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.CurrDemandTime[2][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.CurrDemandTime[2][i].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_NEG_REA:						//�����޹�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_REA:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.CurrDemand[3][i].DemNum, 0xff, 3);
                memset(TermSharedData.CurrDemandTime[3][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.CurrDemand[3][i].DemNum, SrcData, 3);
                TermSharedData.CurrDemand[3][i].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.CurrDemandTime[3][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.CurrDemandTime[3][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.CurrDemand[3][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.CurrDemand[3][i].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.CurrDemandTime[3][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.CurrDemandTime[3][i].DemTime, 6, "����:");

            index += X;
        }

        break;




    case ES_1_REA:						//1�����޹�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_1_REA:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.QuaDemand[0][i].DemNum, 0xff, 3);
                memset(TermSharedData.QuaDemandTime[0][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.QuaDemand[0][i].DemNum, SrcData, 3);
                TermSharedData.QuaDemand[0][i].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.QuaDemandTime[0][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.QuaDemandTime[0][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemand[0][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemand[0][i].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemandTime[0][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemandTime[0][i].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_2_REA:						//2�����޹�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_2_REA:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.QuaDemand[1][i].DemNum, 0xff, 3);
                memset(TermSharedData.QuaDemandTime[1][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.QuaDemand[1][i].DemNum, SrcData, 3);
                TermSharedData.QuaDemand[1][i].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.QuaDemandTime[1][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.QuaDemandTime[1][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemand[1][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemand[1][i].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemandTime[1][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemandTime[1][i].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_3_REA:						//3�����޹�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_3_REA:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.QuaDemand[2][i].DemNum, 0xff, 3);
                memset(TermSharedData.QuaDemandTime[2][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.QuaDemand[2][i].DemNum, SrcData, 3);
                TermSharedData.QuaDemand[2][i].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.QuaDemandTime[2][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.QuaDemandTime[2][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemand[2][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemand[2][i].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemandTime[2][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemandTime[2][i].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_4_REA:						//4�����޹�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_4_REA:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.QuaDemand[3][i].DemNum, 0xff, 3);
                memset(TermSharedData.QuaDemandTime[3][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.QuaDemand[3][i].DemNum, SrcData, 3);
                TermSharedData.QuaDemand[3][i].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.QuaDemandTime[3][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.QuaDemandTime[3][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemand[3][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemand[3][i].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemandTime[3][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemandTime[3][i].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_POS_APP:						//�������ڵ���
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_APP:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.AppaDemand[0][i].DemNum, 0xff, 3);
                memset(TermSharedData.QuaDemandTime[0][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.AppaDemand[0][i].DemNum, SrcData, 3);
                TermSharedData.AppaDemand[0][i].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.QuaDemandTime[0][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.QuaDemandTime[0][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.AppaDemand[0][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.AppaDemand[0][i].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemandTime[0][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemandTime[0][i].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_NEG_APP:						//�������ڵ���
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_APP:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.AppaDemand[1][i].DemNum, 0xff, 3);
                memset(TermSharedData.QuaDemandTime[1][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.AppaDemand[1][i].DemNum, SrcData, 3);
                TermSharedData.AppaDemand[1][i].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.QuaDemandTime[1][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.QuaDemandTime[1][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.AppaDemand[1][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.AppaDemand[1][i].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemandTime[1][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemandTime[1][i].DemTime, 6, "����:");

            index += X;
        }

        break;



    case ES_POS_ACT_A:						//A�������й�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_ACT_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.PosActDemand[0].DemNum, 0xff, 3);
                memset(TermSharedData.PosActDemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.PosActDemand[0].DemNum, SrcData, 3);
                TermSharedData.PosActDemand[0].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.PosActDemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.PosActDemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.PosActDemand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosActDemand[0].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.PosActDemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosActDemandTime[0].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_POS_ACT_B:						//B�������й�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_ACT_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.PosActDemand[1].DemNum, 0xff, 3);
                memset(TermSharedData.PosActDemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.PosActDemand[1].DemNum, SrcData, 3);
                TermSharedData.PosActDemand[1].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.PosActDemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.PosActDemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.PosActDemand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosActDemand[1].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.PosActDemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosActDemandTime[1].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_POS_ACT_C:						//C�������й�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_ACT_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.PosActDemand[2].DemNum, 0xff, 3);
                memset(TermSharedData.PosActDemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.PosActDemand[2].DemNum, SrcData, 3);
                TermSharedData.PosActDemand[2].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.PosActDemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.PosActDemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.PosActDemand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosActDemand[2].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.PosActDemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosActDemandTime[2].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_NEG_ACT_A:						//A�෴���й�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_ACT_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.NegActDemand[0].DemNum, 0xff, 3);
                memset(TermSharedData.NegActDemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.NegActDemand[0].DemNum, SrcData, 3);
                TermSharedData.NegActDemand[0].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.NegActDemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.NegActDemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.NegActDemand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegActDemand[0].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.NegActDemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegActDemandTime[0].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_NEG_ACT_B:						//B�෴���й�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_ACT_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.NegActDemand[1].DemNum, 0xff, 3);
                memset(TermSharedData.NegActDemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.NegActDemand[1].DemNum, SrcData, 3);
                TermSharedData.NegActDemand[1].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.NegActDemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.NegActDemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.NegActDemand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegActDemand[1].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.NegActDemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegActDemandTime[1].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_NEG_ACT_C:						//C�෴���й�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_ACT_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.NegActDemand[2].DemNum, 0xff, 3);
                memset(TermSharedData.NegActDemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.NegActDemand[2].DemNum, SrcData, 3);
                TermSharedData.NegActDemand[2].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.NegActDemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.NegActDemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.NegActDemand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegActDemand[2].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.NegActDemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegActDemandTime[2].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_POS_REA_A:						//A�������޹�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_REA_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.PosReaDemand[0].DemNum, 0xff, 3);
                memset(TermSharedData.PosReaDemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.PosReaDemand[0].DemNum, SrcData, 3);
                TermSharedData.PosReaDemand[0].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.PosReaDemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.PosReaDemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.PosReaDemand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosReaDemand[0].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.PosReaDemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosReaDemandTime[0].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_POS_REA_B:						//B�������޹�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_REA_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.PosReaDemand[1].DemNum, 0xff, 3);
                memset(TermSharedData.PosReaDemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.PosReaDemand[1].DemNum, SrcData, 3);
                TermSharedData.PosReaDemand[1].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.PosReaDemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.PosReaDemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.PosReaDemand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosReaDemand[1].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.PosReaDemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosReaDemandTime[1].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_POS_REA_C:						//C�������޹�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_REA_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.PosReaDemand[2].DemNum, 0xff, 3);
                memset(TermSharedData.PosReaDemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.PosReaDemand[2].DemNum, SrcData, 3);
                TermSharedData.PosReaDemand[2].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.PosReaDemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.PosReaDemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.PosReaDemand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosReaDemand[2].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.PosReaDemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosReaDemandTime[2].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_NEG_REA_A:						//A�෴���޹�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_REA_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.NegReaDemand[0].DemNum, 0xff, 3);
                memset(TermSharedData.NegReaDemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.NegReaDemand[0].DemNum, SrcData, 3);
                TermSharedData.NegReaDemand[0].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.NegReaDemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.NegReaDemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.NegReaDemand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegReaDemand[0].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.NegReaDemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegReaDemandTime[0].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_NEG_REA_B:						//B�෴���޹�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_REA_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.NegReaDemand[1].DemNum, 0xff, 3);
                memset(TermSharedData.NegReaDemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.NegReaDemand[1].DemNum, SrcData, 3);
                TermSharedData.NegReaDemand[1].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.NegReaDemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.NegReaDemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.NegReaDemand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegReaDemand[1].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.NegReaDemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegReaDemandTime[1].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_NEG_REA_C:						//C�෴���޹�
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_REA_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.NegReaDemand[2].DemNum, 0xff, 3);
                memset(TermSharedData.NegReaDemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.NegReaDemand[2].DemNum, SrcData, 3);
                TermSharedData.NegReaDemand[2].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.NegReaDemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.NegReaDemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.NegReaDemand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegReaDemand[2].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.NegReaDemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegReaDemandTime[2].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_1_REA_A:						//A���һ�����޹�����
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_1_REA_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua1Demand[0].DemNum, 0xff, 3);
                memset(TermSharedData.Qua1DemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua1Demand[0].DemNum, SrcData, 3);
                TermSharedData.Qua1Demand[0].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.Qua1DemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua1DemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua1Demand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua1Demand[0].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua1DemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua1DemandTime[0].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_1_REA_B:						//B���һ�����޹�����
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_1_REA_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua1Demand[1].DemNum, 0xff, 3);
                memset(TermSharedData.Qua1DemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua1Demand[1].DemNum, SrcData, 3);
                TermSharedData.Qua1Demand[1].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.Qua1DemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua1DemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua1Demand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua1Demand[1].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua1DemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua1DemandTime[1].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_1_REA_C:						//C���һ�����޹�����
        ACCOMM_FMT_LOGCC("SaveDemand07()ES_1_REA_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua1Demand[2].DemNum, 0xff, 3);
                memset(TermSharedData.Qua1DemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua1Demand[2].DemNum, SrcData, 3);
                TermSharedData.Qua1Demand[2].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.Qua1DemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua1DemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua1Demand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua1Demand[2].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua1DemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua1DemandTime[2].DemTime, 6, "����:");

            index += X;
        }

        break;

        case ES_2_REA_A:						//A��ڶ������޹�����
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_2_REA_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua2Demand[0].DemNum, 0xff, 3);
                memset(TermSharedData.Qua2DemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua2Demand[0].DemNum, SrcData, 3);
                TermSharedData.Qua2Demand[0].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.Qua2DemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua2DemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua2Demand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua2Demand[0].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua2DemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua2DemandTime[0].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_2_REA_B:						//B��ڶ������޹�����
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_2_REA_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua2Demand[1].DemNum, 0xff, 3);
                memset(TermSharedData.Qua2DemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua2Demand[1].DemNum, SrcData, 3);
                TermSharedData.Qua2Demand[1].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.Qua2DemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua2DemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua2Demand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua2Demand[1].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua2DemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua2DemandTime[1].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_2_REA_C:						//C��ڶ������޹�����
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_2_REA_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua2Demand[2].DemNum, 0xff, 3);
                memset(TermSharedData.Qua2DemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua2Demand[2].DemNum, SrcData, 3);
                TermSharedData.Qua2Demand[2].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.Qua2DemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua2DemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua2Demand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua2Demand[2].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua2DemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua2DemandTime[2].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_3_REA_A:						//A����������޹�����
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_3_REA_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua3Demand[0].DemNum, 0xff, 3);
                memset(TermSharedData.Qua3DemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua3Demand[0].DemNum, SrcData, 3);
                TermSharedData.Qua3Demand[0].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.Qua3DemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua3DemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua3Demand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua3Demand[0].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua3DemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua3DemandTime[0].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_3_REA_B:						//B����������޹�����
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_3_REA_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua3Demand[1].DemNum, 0xff, 3);
                memset(TermSharedData.Qua3DemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua3Demand[1].DemNum, SrcData, 3);
                TermSharedData.Qua3Demand[1].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.Qua3DemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua3DemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua3Demand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua3Demand[1].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua3DemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua3DemandTime[1].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_3_REA_C:						//C����������޹�����
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_3_REA_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua3Demand[2].DemNum, 0xff, 3);
                memset(TermSharedData.Qua3DemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua3Demand[2].DemNum, SrcData, 3);
                TermSharedData.Qua3Demand[2].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.Qua3DemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua3DemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua3Demand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua3Demand[2].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua3DemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua3DemandTime[2].DemTime, 6, "����:");

            index += X;
        }

        break;

        case ES_4_REA_A:						//A����������޹�����
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_4_REA_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua4Demand[0].DemNum, 0xff, 3);
                memset(TermSharedData.Qua4DemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua4Demand[0].DemNum, SrcData, 3);
                TermSharedData.Qua4Demand[0].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.Qua4DemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua4DemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua4Demand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua4Demand[0].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua4DemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua4DemandTime[0].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_4_REA_B:						//B����������޹�����
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_4_REA_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua4Demand[1].DemNum, 0xff, 3);
                memset(TermSharedData.Qua4DemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua4Demand[1].DemNum, SrcData, 3);
                TermSharedData.Qua4Demand[1].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.Qua4DemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua4DemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua4Demand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua4Demand[1].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua4DemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua4DemandTime[1].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_4_REA_C:						//C����������޹�����
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_4_REA_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua4Demand[2].DemNum, 0xff, 3);
                memset(TermSharedData.Qua4DemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua4Demand[2].DemNum, SrcData, 3);
                TermSharedData.Qua4Demand[2].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.Qua4DemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua4DemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua4Demand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua4Demand[2].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua4DemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua4DemandTime[2].DemTime, 6, "����:");

            index += X;
        }

        break;

        case ES_POS_APP_A:						//A���������ڵ���
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_APP_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.ActAppDemand[0].DemNum, 0xff, 3);
                memset(TermSharedData.ActAppDemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.ActAppDemand[0].DemNum, SrcData, 3);
                TermSharedData.ActAppDemand[0].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.ActAppDemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.ActAppDemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.ActAppDemand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.ActAppDemand[0].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.ActAppDemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.ActAppDemandTime[0].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_POS_APP_B:						//B���������ڵ���
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_APP_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.ActAppDemand[1].DemNum, 0xff, 3);
                memset(TermSharedData.ActAppDemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.ActAppDemand[1].DemNum, SrcData, 3);
                TermSharedData.ActAppDemand[1].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.ActAppDemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.ActAppDemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.ActAppDemand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.ActAppDemand[1].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.ActAppDemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.ActAppDemandTime[1].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_POS_APP_C:						//C���������ڵ���
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_APP_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.ActAppDemand[2].DemNum, 0xff, 3);
                memset(TermSharedData.ActAppDemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.ActAppDemand[2].DemNum, SrcData, 3);
                TermSharedData.ActAppDemand[2].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.ActAppDemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.ActAppDemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.ActAppDemand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.ActAppDemand[2].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.ActAppDemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.ActAppDemandTime[2].DemTime, 6, "����:");

            index += X;
        }

        break;

        case ES_NEG_APP_A:						//A�෴�����ڵ���
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_APP_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.NegAppDemand[0].DemNum, 0xff, 3);
                memset(TermSharedData.NegAppDemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.NegAppDemand[0].DemNum, SrcData, 3);
                TermSharedData.NegAppDemand[0].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.NegAppDemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.NegAppDemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.NegAppDemand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegAppDemand[0].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.NegAppDemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegAppDemandTime[0].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_NEG_APP_B:						//B�෴�����ڵ���
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_APP_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.NegAppDemand[1].DemNum, 0xff, 3);
                memset(TermSharedData.NegAppDemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.NegAppDemand[1].DemNum, SrcData, 3);
                TermSharedData.NegAppDemand[1].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.NegAppDemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.NegAppDemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.NegAppDemand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegAppDemand[1].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.NegAppDemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegAppDemandTime[1].DemTime, 6, "����:");

            index += X;
        }

        break;

    case ES_NEG_APP_C:						//C�෴�����ڵ���
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_APP_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.NegAppDemand[2].DemNum, 0xff, 3);
                memset(TermSharedData.NegAppDemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.NegAppDemand[2].DemNum, SrcData, 3);
                TermSharedData.NegAppDemand[2].DemNum[2] &= 0x7f;                // ȥ������޹��ķ���λ
                memcpy(&TermSharedData.NegAppDemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.NegAppDemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.NegAppDemand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegAppDemand[2].DemNum, 3, "����:");
            ACCOMM_FMT_LOGCC("TermSharedData.NegAppDemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegAppDemandTime[2].DemTime, 6, "����:");

            index += X;
        }

        break;
        
    default:
        return;
    }
}


/*******************************************************************************
* ��������: SaveER07
* ��������: �洢����ʾ��
* �������: Pn                  �������
*           DataLen             ���ݳ���
*           DataBuff            ��������
*           EnergySort          �������
*           EnergyType          ������������ 0:�Ǹ߾��� ��λС��; 1 �߾��� ��λС��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void SaveER07(uint8 DataLen, uint8 *DataBuff, ENERGY_SORT_T EnergySort, uint8 EnergyType)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   SignFlag = 0;                       //���ű�־(0:�� / 1:��)
    uint8   X, Y;                               //BCD���ֽ�����BIN���ֽ���
    uint8   Len = 0;
    uint8   i = 0, index = 0;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    if(1 == EnergyType)
    {
        X = 5;
        Y = 8;
    }
    else
    {
        X = 4;
        Y = 4;
    }

    ACCOMM_FMT_LOGCC("SaveER07() BCD���ֽ���X=%d, BIN���ֽ���Y=%d, DataLen=%u...... \n", X, Y, DataLen);
    
    //�����������ݳ���5/4
    switch (EnergySort)
    {
    case ES_POS_ACT:                        //�����й�
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_ACT:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.PosActEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PosActEng[%d]:%d \n", i, TermSharedData.PosActEng[i].ULinteger);

            index += Len;
        }

        break;

    case ES_NEG_ACT:                        //�����й�
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_ACT:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.NegActEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.NegActEng[%d]:%d \n", i, TermSharedData.NegActEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_POS_REA:                        //�����޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_REA:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                if((SrcData[X - 1] & 0x80) != 0)   //��
                {
                    SrcData[X - 1] &= 0x7F;
                    SignFlag = 1;
                }
                else
                {
                    SignFlag = 0;
                }
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            if(SignFlag == 1)                   //��
            {
                gPriMeasurePara.PosReaEnergyType[i] = 1;
            }
            else
            {
                gPriMeasurePara.PosReaEnergyType[i] = 0;
            }
            
            memcpy((uint8*)&TermSharedData.PosReaEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PosReaEng[%d]:%d \n", i, TermSharedData.PosReaEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_NEG_REA:                        //�����޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_REA:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                if((SrcData[X - 1] & 0x80) != 0)   //��
                {
                    SrcData[X - 1] &= 0x7F;
                    SignFlag = 1;
                }
                else
                {
                    SignFlag = 0;
                }
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            if(SignFlag == 1)                   //��
            {
                gPriMeasurePara.NegReaEnergyType[i] = 1;
            }
            else
            {
                gPriMeasurePara.NegReaEnergyType[i] = 0;
            }
            
            memcpy((uint8*)&TermSharedData.NegReaEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.NegReaEng[%d]:%d \n", i, TermSharedData.NegReaEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_1_REA:                          //1�����޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_1_REA:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.Quad1ReaEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.Quad1ReaEng[%d]:%d \n", i, TermSharedData.Quad1ReaEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_2_REA:                          //2�����޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_2_REA:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.Quad2ReaEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.Quad2ReaEng[%d]:%d \n", i, TermSharedData.Quad2ReaEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_3_REA:                          //3�����޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_3_REA:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.Quad3ReaEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.Quad3ReaEng[%d]:%d \n", i, TermSharedData.Quad3ReaEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_4_REA:                          //4�����޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_4_REA:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.Quad4ReaEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.Quad4ReaEng[%d]:%d \n", i, TermSharedData.Quad4ReaEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_POS_ACT_A:                      //A�������й�
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_ACT_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaAPosActEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaAPosActEng:%d \n", TermSharedData.PhaAPosActEng[0].ULinteger);
        break;

    case ES_NEG_ACT_A:                      //A�෴���й�
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_ACT_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaANegActEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaANegActEng:%d \n", TermSharedData.PhaANegActEng[0].ULinteger);
        break;

    case ES_POS_REA_A:                      //A�������޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_REA_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //��
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //��
        {
            gPriMeasurePara.PhaAPosReaEngType[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaAPosReaEngType[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaAPosReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaAPosReaEng:%d \n", TermSharedData.PhaAPosReaEng[0].ULinteger);
        break;

    case ES_NEG_REA_A:                      //A�෴���޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_REA_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //��
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //��
        {
            gPriMeasurePara.PhaANegReaEngType[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaANegReaEngType[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaANegReaEng, TempBuff, Y);
        
        ACCOMM_FMT_LOGCC("TermSharedData.PhaANegReaEng:%d \n", TermSharedData.PhaANegReaEng[0].ULinteger);
        break;

    case ES_POS_ACT_B:                      //B�������й�
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_ACT_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaBPosActEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBPosActEng:%d \n", TermSharedData.PhaBPosActEng[0].ULinteger);
        break;

    case ES_NEG_ACT_B:                      //B�෴���й�
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_ACT_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaBNegActEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBNegActEng:%d \n", TermSharedData.PhaBNegActEng[0].ULinteger);
        break;

    case ES_POS_REA_B:                      //B�������޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_REA_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //��
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //��
        {
            gPriMeasurePara.PhaBPosReaEngType[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaBPosReaEngType[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaBPosReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBPosReaEng:%d \n", TermSharedData.PhaBPosReaEng[0].ULinteger);
        break;

    case ES_NEG_REA_B:                      //B�෴���޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_REA_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //��
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //��
        {
            gPriMeasurePara.PhaBNegReaEngType[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaBNegReaEngType[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaBNegReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBNegReaEng:%d \n", TermSharedData.PhaBNegReaEng[0].ULinteger);
        break;

    case ES_POS_ACT_C:                      //C�������й�
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_ACT_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaCPosActEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCPosActEng:%d \n", TermSharedData.PhaCPosActEng[0].ULinteger);
        break;

    case ES_NEG_ACT_C:                      //C�෴���й�
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_ACT_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaCNegActEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCNegActEng:%d \n", TermSharedData.PhaCNegActEng[0].ULinteger);
        break;

    case ES_POS_REA_C:                      //C�������޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_REA_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //��
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //��
        {
            gPriMeasurePara.PhaCPosReaEngType[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaCPosReaEngType[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaCPosReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCPosReaEng:%d \n", TermSharedData.PhaCPosReaEng[0].ULinteger);
        break;

    case ES_NEG_REA_C:                      //C�෴���޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_REA_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //��
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //��
        {
            gPriMeasurePara.PhaCNegReaEngType[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaCNegReaEngType[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaCNegReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCNegReaEng:%d \n", TermSharedData.PhaCNegReaEng[0].ULinteger);
        break;

    case ES_CONBIN_ACT:						//(��ǰ)����й����ܿ�
        ACCOMM_FMT_LOGCC("SaveER07() ES_CONBIN_ACT:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                if((SrcData[X - 1] & 0x80) != 0)   //��
                {
                    SrcData[X - 1] &= 0x7F;
                    SignFlag = 1;
                }
                else
                {
                    SignFlag = 0;
                }
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            if(SignFlag == 1)                   //��
            {
                gPriMeasurePara.CmbEnergyType[i] = 1;
            }
            else
            {
                gPriMeasurePara.CmbEnergyType[i] = 0;
            }
            
            memcpy((uint8*)&TermSharedData.CmbActEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.CmbActEng[%d]:%d \n", i, TermSharedData.CmbActEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_POS_APP:						//(��ǰ)�������ڵ���
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_APP:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.PosAppEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PosAppEng[%d]:%d \n", i, TermSharedData.PosAppEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_NEG_APP:						//(��ǰ�������ڵ���
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_APP:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.NegAppEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.NegAppEng[%d]:%d \n", i, TermSharedData.NegAppEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_1_REA_A:                      //A���һ�����޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_1_REA_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaA1ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaA1ReaEng:%d \n", TermSharedData.PhaA1ReaEng[0].ULinteger);
        break;

    case ES_1_REA_B:                      //B���һ�����޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_1_REA_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaB1ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaB1ReaEng:%d \n", TermSharedData.PhaB1ReaEng[0].ULinteger);
        break;

    case ES_1_REA_C:                      //C���һ�����޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_1_REA_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaC1ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaC1ReaEng:%d \n", TermSharedData.PhaC1ReaEng[0].ULinteger);
        break;

    case ES_2_REA_A:                      //A��ڶ������޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_2_REA_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaA2ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaA2ReaEng:%d \n", TermSharedData.PhaA2ReaEng[0].ULinteger);
        break;

    case ES_2_REA_B:                      //B��ڶ������޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_2_REA_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaB2ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaB2ReaEng:%d \n", TermSharedData.PhaB2ReaEng[0].ULinteger);
        break;

    case ES_2_REA_C:                      //C��ڶ������޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_2_REA_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaC2ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaC2ReaEng:%d \n", TermSharedData.PhaC2ReaEng[0].ULinteger);
        break;

    case ES_3_REA_A:                      //A����������޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_3_REA_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaA3ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaA3ReaEng:%d \n", TermSharedData.PhaA3ReaEng[0].ULinteger);
        break;

    case ES_3_REA_B:                      //B����������޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_3_REA_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaB3ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaB3ReaEng:%d \n", TermSharedData.PhaB3ReaEng[0].ULinteger);
        break;

    case ES_3_REA_C:                      //C����������޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_3_REA_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaC3ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaC3ReaEng:%d \n", TermSharedData.PhaC3ReaEng[0].ULinteger);
        break;

    case ES_4_REA_A:                      //A����������޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_4_REA_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaA4ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaA4ReaEng:%d \n", TermSharedData.PhaA4ReaEng[0].ULinteger);
        break;

    case ES_4_REA_B:                      //B����������޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_4_REA_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaB4ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaB4ReaEng:%d \n", TermSharedData.PhaB4ReaEng[0].ULinteger);
        break;

    case ES_4_REA_C:                      //C����������޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_4_REA_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaC4ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaC4ReaEng:%d \n", TermSharedData.PhaC4ReaEng[0].ULinteger);
        break;

    case ES_POS_APP_A:                      //A���������ڵ���
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_APP_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaAActAppEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaAActAppEng:%d \n", TermSharedData.PhaAActAppEng[0].ULinteger);
        break;

    case ES_POS_APP_B:                      //B���������ڵ���
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_APP_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaBActAppEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBActAppEng:%d \n", TermSharedData.PhaBActAppEng[0].ULinteger);
        break;

    case ES_POS_APP_C:                      //C���������ڵ���
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_APP_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaCActAppEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCActAppEng:%d \n", TermSharedData.PhaCActAppEng[0].ULinteger);
        break;

    case ES_NEG_APP_A:                      //A�෴�����ڵ���
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_APP_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaANegAppEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaANegAppEng:%d \n", TermSharedData.PhaANegAppEng[0].ULinteger);
        break;

    case ES_NEG_APP_B:                      //B�෴�����ڵ���
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_APP_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaBNegAppEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBNegAppEng:%d \n", TermSharedData.PhaBNegAppEng[0].ULinteger);
        break;

    case ES_NEG_APP_C:                      //C�෴�����ڵ���
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_APP_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaCNegAppEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCNegAppEng:%d \n", TermSharedData.PhaCNegAppEng[0].ULinteger);
        break;

    /***�߾��� ������ begin***/
    
    case ES_POS_ACT_HD:                        //�����й�
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_ACT_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.PosActEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PosActEngHD[%d]:%d \n", i, TermSharedData.PosActEngHD[i].ULinteger);

            index += Len;
        }

        break;

    case ES_NEG_ACT_HD:                        //�����й�
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_ACT_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.NegActEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.NegActEngHD[%d]:%d \n", i, TermSharedData.NegActEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_POS_REA_HD:                        //�����޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_REA_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                if((SrcData[X - 1] & 0x80) != 0)   //��
                {
                    SrcData[X - 1] &= 0x7F;
                    SignFlag = 1;
                }
                else
                {
                    SignFlag = 0;
                }
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            if(SignFlag == 1)                   //��
            {
                gPriMeasurePara.PosReaEnergyTypeHD[i] = 1;
            }
            else
            {
                gPriMeasurePara.PosReaEnergyTypeHD[i] = 0;
            }
            
            memcpy((uint8*)&TermSharedData.PosReaEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PosReaEngHD[%d]:%d \n", i, TermSharedData.PosReaEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_NEG_REA_HD:                        //�����޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_REA_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                if((SrcData[X - 1] & 0x80) != 0)   //��
                {
                    SrcData[X - 1] &= 0x7F;
                    SignFlag = 1;
                }
                else
                {
                    SignFlag = 0;
                }
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            if(SignFlag == 1)                   //��
            {
                gPriMeasurePara.NegReaEnergyTypeHD[i] = 1;
            }
            else
            {
                gPriMeasurePara.NegReaEnergyTypeHD[i] = 0;
            }
            
            memcpy((uint8*)&TermSharedData.NegReaEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.NegReaEngHD[%d]:%d \n", i, TermSharedData.NegReaEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_1_REA_HD:                          //1�����޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_1_REA_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.Quad1ReaEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.Quad1ReaEngHD[%d]:%d \n", i, TermSharedData.Quad1ReaEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_2_REA_HD:                          //2�����޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_2_REA_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.Quad2ReaEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.Quad2ReaEngHD[%d]:%d \n", i, TermSharedData.Quad2ReaEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_3_REA_HD:                          //3�����޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_3_REA_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.Quad3ReaEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.Quad3ReaEngHD[%d]:%d \n", i, TermSharedData.Quad3ReaEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_4_REA_HD:                          //4�����޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_4_REA_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.Quad4ReaEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.Quad4ReaEngHD[%d]:%d \n", i, TermSharedData.Quad4ReaEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_POS_ACT_A_HD:                      //A�������й�
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_ACT_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaAPosActEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaAPosActEngHD:%d \n", TermSharedData.PhaAPosActEngHD[0].ULinteger);
        break;

    case ES_NEG_ACT_A_HD:                      //A�෴���й�
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_ACT_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaANegActEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaANegActEngHD:%d \n", TermSharedData.PhaANegActEngHD[0].ULinteger);
        break;

    case ES_POS_REA_A_HD:                      //A�������޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_REA_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //��
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //��
        {
            gPriMeasurePara.PhaAPosReaEngTypeHD[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaAPosReaEngTypeHD[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaAPosReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaAPosReaEngHD:%d \n", TermSharedData.PhaAPosReaEngHD[0].ULinteger);
        break;

    case ES_NEG_REA_A_HD:                      //A�෴���޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_REA_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //��
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //��
        {
            gPriMeasurePara.PhaANegReaEngTypeHD[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaANegReaEngTypeHD[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaANegReaEngHD, TempBuff, Y);
        
        ACCOMM_FMT_LOGCC("TermSharedData.PhaANegReaEngHD:%d \n", TermSharedData.PhaANegReaEngHD[0].ULinteger);
        break;

    case ES_POS_ACT_B_HD:                      //B�������й�
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_ACT_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaBPosActEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBPosActEngHD:%d \n", TermSharedData.PhaBPosActEngHD[0].ULinteger);
        break;

    case ES_NEG_ACT_B_HD:                      //B�෴���й�
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_ACT_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaBNegActEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBNegActEngHD:%d \n", TermSharedData.PhaBNegActEngHD[0].ULinteger);
        break;

    case ES_POS_REA_B_HD:                      //B�������޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_REA_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //��
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //��
        {
            gPriMeasurePara.PhaBPosReaEngTypeHD[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaBPosReaEngTypeHD[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaBPosReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBPosReaEngHD:%d \n", TermSharedData.PhaBPosReaEngHD[0].ULinteger);
        break;

    case ES_NEG_REA_B_HD:                      //B�෴���޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_REA_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //��
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //��
        {
            gPriMeasurePara.PhaBNegReaEngTypeHD[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaBNegReaEngTypeHD[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaBNegReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBNegReaEngHD:%d \n", TermSharedData.PhaBNegReaEngHD[0].ULinteger);
        break;

    case ES_POS_ACT_C_HD:                      //C�������й�
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_ACT_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaCPosActEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCPosActEngHD:%d \n", TermSharedData.PhaCPosActEngHD[0].ULinteger);
        break;

    case ES_NEG_ACT_C_HD:                      //C�෴���й�
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_ACT_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaCNegActEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCNegActEngHD:%d \n", TermSharedData.PhaCNegActEngHD[0].ULinteger);
        break;

    case ES_POS_REA_C_HD:                      //C�������޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_REA_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //��
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //��
        {
            gPriMeasurePara.PhaCPosReaEngTypeHD[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaCPosReaEngTypeHD[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaCPosReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCPosReaEngHD:%d \n", TermSharedData.PhaCPosReaEngHD[0].ULinteger);
        break;

    case ES_NEG_REA_C_HD:                      //C�෴���޹�
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_REA_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //��
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //��
        {
            gPriMeasurePara.PhaCNegReaEngTypeHD[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaCNegReaEngTypeHD[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaCNegReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCNegReaEngHD:%d \n", TermSharedData.PhaCNegReaEngHD[0].ULinteger);
        break;

    case ES_CONBIN_ACT_HD:						//(��ǰ)����й����ܿ�
        ACCOMM_FMT_LOGCC("SaveER07() ES_CONBIN_ACT_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                if((SrcData[X - 1] & 0x80) != 0)   //��
                {
                    SrcData[X - 1] &= 0x7F;
                    SignFlag = 1;
                }
                else
                {
                    SignFlag = 0;
                }
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            if(SignFlag == 1)                   //��
            {
                gPriMeasurePara.CmbEnergyTypeHD[i] = 1;
            }
            else
            {
                gPriMeasurePara.CmbEnergyTypeHD[i] = 0;
            }
            
            memcpy((uint8*)&TermSharedData.CmbActEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.CmbActEngHD[%d]:%d \n", i, TermSharedData.CmbActEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_POS_APP_HD:						//(��ǰ)�������ڵ���
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_APP_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.PosAppEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PosAppEngHD[%d]:%d \n", i, TermSharedData.PosAppEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_NEG_APP_HD:						//(��ǰ�������ڵ���
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_APP_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.NegAppEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.NegAppEngHD[%d]:%d \n", i, TermSharedData.NegAppEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_1_REA_A_HD:                      //A���һ�����޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_1_REA_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaA1ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaA1ReaEngHD:%d \n", TermSharedData.PhaA1ReaEngHD[0].ULinteger);
        break;

    case ES_1_REA_B_HD:                      //B���һ�����޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_1_REA_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaB1ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaB1ReaEngHD:%d \n", TermSharedData.PhaB1ReaEngHD[0].ULinteger);
        break;

    case ES_1_REA_C_HD:                      //C���һ�����޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_1_REA_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaC1ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaC1ReaEngHD:%d \n", TermSharedData.PhaC1ReaEngHD[0].ULinteger);
        break;

    case ES_2_REA_A_HD:                      //A��ڶ������޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_2_REA_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaA2ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaA2ReaEngHD:%d \n", TermSharedData.PhaA2ReaEngHD[0].ULinteger);
        break;

    case ES_2_REA_B_HD:                      //B��ڶ������޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_2_REA_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaB2ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaB2ReaEngHD:%d \n", TermSharedData.PhaB2ReaEngHD[0].ULinteger);
        break;

    case ES_2_REA_C_HD:                      //C��ڶ������޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_2_REA_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaC2ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaC2ReaEngHD:%d \n", TermSharedData.PhaC2ReaEngHD[0].ULinteger);
        break;

    case ES_3_REA_A_HD:                      //A����������޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_3_REA_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaA3ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaA3ReaEngHD:%d \n", TermSharedData.PhaA3ReaEngHD[0].ULinteger);
        break;

    case ES_3_REA_B_HD:                      //B����������޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_3_REA_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaB3ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaB3ReaEngHD:%d \n", TermSharedData.PhaB3ReaEngHD[0].ULinteger);
        break;

    case ES_3_REA_C_HD:                      //C����������޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_3_REA_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaC3ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaC3ReaEngHD:%d \n", TermSharedData.PhaC3ReaEngHD[0].ULinteger);
        break;

    case ES_4_REA_A_HD:                      //A����������޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_4_REA_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaA4ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaA4ReaEngHD:%d \n", TermSharedData.PhaA4ReaEngHD[0].ULinteger);
        break;

    case ES_4_REA_B_HD:                      //B����������޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_4_REA_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaB4ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaB4ReaEngHD:%d \n", TermSharedData.PhaB4ReaEngHD[0].ULinteger);
        break;

    case ES_4_REA_C_HD:                      //C����������޹�����
        ACCOMM_FMT_LOGCC("SaveER07() ES_4_REA_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaC4ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaC4ReaEngHD:%d \n", TermSharedData.PhaC4ReaEngHD[0].ULinteger);
        break;

    case ES_POS_APP_A_HD:                      //A���������ڵ���
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_APP_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaAActAppEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaAActAppEngHD:%d \n", TermSharedData.PhaAActAppEngHD[0].ULinteger);
        break;

    case ES_POS_APP_B_HD:                      //B���������ڵ���
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_APP_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaBActAppEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBActAppEngHD:%d \n", TermSharedData.PhaBActAppEngHD[0].ULinteger);
        break;

    case ES_POS_APP_C_HD:                      //C���������ڵ���
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_APP_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaCActAppEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCActAppEngHD:%d \n", TermSharedData.PhaCActAppEngHD[0].ULinteger);
        break;

    case ES_NEG_APP_A_HD:                      //A�෴�����ڵ���
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_APP_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaANegAppEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaANegAppEngHD:%d \n", TermSharedData.PhaANegAppEngHD[0].ULinteger);
        break;

    case ES_NEG_APP_B_HD:                      //B�෴�����ڵ���
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_APP_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaBNegAppEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBNegAppEngHD:%d \n", TermSharedData.PhaBNegAppEngHD[0].ULinteger);
        break;

    case ES_NEG_APP_C_HD:                      //C�෴�����ڵ���
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_APP_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaCNegAppEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCNegAppEngHD:%d \n", TermSharedData.PhaCNegAppEngHD[0].ULinteger);
        break;

    /***�߾��� ������ end***/

    default:
        return;
    }
}

/*******************************************************************************
* ��������: SaveU07
* ��������: �洢��ѹ
* �������:
*           DataLen             ���ݳ���
*           DataBuff            ��������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void SaveU07(uint8 DataLen, uint8 *DataBuff)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   X, Y;                               //BCD���ֽ�����BIN���ֽ���
    uint8   i = 0, index = 0;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    X = 2;
    Y = 2;

    ACCOMM_FMT_LOGCC("SaveU07():...... \n");

    for(i = 0; i < 3; i++)
    {
        memcpy(SrcData, DataBuff + index, X);

        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint16*)&TermSharedData.PhaAVolt+i, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaAVolt+%d: %d \n", i, *((uint16*)&TermSharedData.PhaAVolt+i));

        index += X;
    }
}

/*******************************************************************************
* ��������: Savefreq07
* ��������: �洢����Ƶ��
* �������:
*           DataLen             ���ݳ���
*           DataBuff            ��������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void Savefreq07(uint8 DataLen, uint8 *DataBuff)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   X, Y;                               //BCD���ֽ�����BIN���ֽ���

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    X = 2;
    Y = 2;

    ACCOMM_FMT_LOGCC("Savefreq07():...... \n");

    memcpy(SrcData, DataBuff, X);

    if(memcmp(SrcData, NO_DATA, X) == 0)
    {
        memset(TempBuff, 0xff, Y);
    }
    else
    {
        BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
    }
    memcpy((uint8*)&TermSharedData.LineFrz, TempBuff, Y);

    ACCOMM_FMT_LOGCC("TermSharedData.LineFrz: %d \n", TermSharedData.LineFrz);
}

/*******************************************************************************
* ��������: SaveP07
* ��������: �洢����
* �������:
*           DataLen             ���ݳ���
*           DataBuff            ��������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void SaveP07(uint8 DataLen, uint8 *DataBuff)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   SignFlag = 0;                       //���ű�־(0:�� / 1:��)
    uint8   X, Y;                               //BCD���ֽ�����BIN���ֽ���
    uint8   i = 0, index = 0;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    X = 3;
    Y = 4;

    ACCOMM_FMT_LOGCC("SaveP07():...... \n");

    for(i = 0; i < 4; i++)
    {
        memcpy(SrcData, DataBuff + index, X);

        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //��
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        index += X;
        if(i == 0)                              //��
        {
            if(SignFlag == 1)
            {
                gPriMeasurePara.CurPowDir.BitSect.ActPowFlag = 1;
                memcpy((uint8*)&TermSharedData.NegActPower, TempBuff, Y);
                memset(TempBuff, 0x00, 4);
                memcpy((uint8*)&TermSharedData.PosActPower, TempBuff, Y);
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.ActPowFlag = 0;
                memcpy((uint8*)&TermSharedData.PosActPower, TempBuff, Y);
                memset(TempBuff, 0x00, 4);
                memcpy((uint8*)&TermSharedData.NegActPower, TempBuff, Y);
            }

            ACCOMM_FMT_LOGCC("TermSharedData.PosActPower: %d, SignFlag=%d \n", TermSharedData.PosActPower, SignFlag);
            ACCOMM_FMT_LOGCC("TermSharedData.NegActPower: %d, SignFlag=%d \n", TermSharedData.NegActPower, SignFlag);
        }
        else if(i == 1) /* A���й�����*/
        {
            if(SignFlag == 1)                   //��
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaAActPowFlag = 1;
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaAActPowFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaAActPower, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaAActPower: %d, SignFlag=%d \n", TermSharedData.PhaAActPower, SignFlag);
        }
        else if(i == 2) /* B���й�����*/
        {
            if(SignFlag == 1)                   //��
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaBActPowFlag = 1;
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaBActPowFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaBActPower, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaBActPower: %d, SignFlag=%d \n", TermSharedData.PhaBActPower, SignFlag);
        }
        else if(i == 3) /* C���й�����*/
        {
            if(SignFlag == 1)                   //��
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaCActPowFlag = 1;
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaCActPowFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaCActPower, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaCActPower: %d, SignFlag=%d \n", TermSharedData.PhaCActPower, SignFlag);
        }
    }
}

/*******************************************************************************
* ��������: SaveQ07
* ��������: �洢����
* �������:
*           DataLen             ���ݳ���
*           DataBuff            ��������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void SaveQ07(uint8 DataLen, uint8 *DataBuff)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   SignFlag = 0;                       //���ű�־(0:�� / 1:��)
    uint8   X, Y;                               //BCD���ֽ�����BIN���ֽ���
    uint8   i = 0, index = 0;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    X = 3;
    Y = 4;

    ACCOMM_FMT_LOGCC("SaveQ07():...... \n");

    for(i = 0; i < 4; i++)
    {
        memcpy(SrcData, DataBuff + index, X);

        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //��
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        index += X;
        if(i == 0)                              //��
        {
            if(SignFlag == 1)
            {
                gPriMeasurePara.CurPowDir.BitSect.ReaPowFlag = 1;
                memcpy((uint8*)&TermSharedData.NegReaPower, TempBuff, Y);
                memset(TempBuff, 0x00, 4);
                memcpy((uint8*)&TermSharedData.PosReaPower, TempBuff, Y);
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.ReaPowFlag = 0;
                memcpy((uint8*)&TermSharedData.PosReaPower, TempBuff, Y);
                memset(TempBuff, 0x00, 4);
                memcpy((uint8*)&TermSharedData.NegReaPower, TempBuff, Y);
            }

            ACCOMM_FMT_LOGCC("TermSharedData.PosReaPower: %d, SignFlag=%d \n", TermSharedData.PosReaPower, SignFlag);
            ACCOMM_FMT_LOGCC("TermSharedData.NegReaPower: %d, SignFlag=%d \n", TermSharedData.NegReaPower, SignFlag);
        }
        else if(i == 1) /* A���޹�����*/
        {
            if(SignFlag == 1)                   //��
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaAReaPowFlag = 1;
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaAReaPowFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaAReaPower, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaAReaPower: %d, SignFlag=%d \n", TermSharedData.PhaAReaPower, SignFlag);
        }
        else if(i == 2) /* B���޹�����*/
        {
            if(SignFlag == 1)                   //��
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaBReaPowFlag = 1;
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaBReaPowFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaBReaPower, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaBReaPower: %d, SignFlag=%d \n", TermSharedData.PhaBReaPower, SignFlag);
        }
        else if(i == 3) /* C���޹�����*/
        {
            if(SignFlag == 1)                   //��
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaCReaPowFlag = 1;
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaCReaPowFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaCReaPower, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaCReaPower: %d, SignFlag=%d \n", TermSharedData.PhaCReaPower, SignFlag);
        }
    }
}

/*******************************************************************************
* ��������: SaveS07
* ��������: �洢����
* �������:
*           DataLen             ���ݳ���
*           DataBuff            ��������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void SaveS07(uint8 DataLen, uint8 *DataBuff)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   SignFlag = 0;                       //���ű�־(0:�� / 1:��)
    uint8   X, Y;                               //BCD���ֽ�����BIN���ֽ���
    uint8   i = 0, index = 0;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    X = 3;
    Y = 4;

    ACCOMM_FMT_LOGCC("SaveS07():...... \n");

    for(i = 0; i < 4; i++)
    {
        memcpy(SrcData, DataBuff + index, X);

        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //��
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        index += X;
        if(i == 0)                              //��
        {
            if(SignFlag == 1)
            {
                gPriMeasurePara.CurPowDir.BitSect.AppPowFlag = 1;
                memcpy((uint8*)&TermSharedData.NegApparentPower, TempBuff, Y);
                memset(TempBuff, 0x00, 4);
                memcpy((uint8*)&TermSharedData.PosApparentPower, TempBuff, Y);
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.AppPowFlag = 0;
                memcpy((uint8*)&TermSharedData.PosApparentPower, TempBuff, Y);
                memset(TempBuff, 0x00, 4);
                memcpy((uint8*)&TermSharedData.NegApparentPower, TempBuff, Y);
            }

            ACCOMM_FMT_LOGCC("TermSharedData.PosApparentPower: %d, SignFlag=%d \n", TermSharedData.PosApparentPower, SignFlag);
            ACCOMM_FMT_LOGCC("TermSharedData.NegApparentPower: %d, SignFlag=%d \n", TermSharedData.NegApparentPower, SignFlag);
        }
        else if(i == 1) /* A�����ڹ���*/
        {
            if(SignFlag == 1)                   //��
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaAAppPowFlag = 1;
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaAAppPowFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaAAppPower, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaAAppPower: %d, SignFlag=%d \n", TermSharedData.PhaAAppPower, SignFlag);
        }
        else if(i == 2) /* B�����ڹ���*/
        {
            if(SignFlag == 1)                   //��
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaBAppPowFlag = 1;
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaBAppPowFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaBAppPower, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaBAppPower: %d, SignFlag=%d \n", TermSharedData.PhaBAppPower, SignFlag);
        }
        else if(i == 3) /* C�����ڹ���*/
        {
            if(SignFlag == 1)                   //��
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaCAppPowFlag = 1;
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaCAppPowFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaCAppPower, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaCAppPower: %d, SignFlag=%d \n", TermSharedData.PhaCAppPower, SignFlag);
        }
    }
}


/*******************************************************************************
* ��������: SaveI07
* ��������: �洢����
* �������: Pn                  �������
*           DataLen             ���ݳ���
*           DataBuff            ��������
*           DataItemLen         �������
*           OffsetId            �����ֵ�ƫ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void SaveI07(uint8 DataLen, uint8 *DataBuff)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   SignFlag = 0;                       //���ű�־(0:�� / 1:��)
    uint8   X, Y;                               //BCD���ֽ�����BIN���ֽ���
    uint8   i = 0, index = 0;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    X = 3;
    Y = 4;

    ACCOMM_FMT_LOGCC("SaveI07():...... \n");

    for(i = 0; i < 3; i++)
    {
        memcpy(SrcData, DataBuff + index, X);

        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //��
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_SBinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint32*)&TermSharedData.PhaACurr+i, TempBuff, Y);

        if(i == 0)
        {
            gPriMeasurePara.CurrentSignType.PhaAAppCurrentFlag = SignFlag;
        }
        else if(i == 1)
        {
            gPriMeasurePara.CurrentSignType.PhaBAppCurrentFlag = SignFlag;
        }
        else if(i == 2)
        {
            gPriMeasurePara.CurrentSignType.PhaCAppCurrentFlag = SignFlag;
        }

        ACCOMM_FMT_LOGCC("TermSharedData.PhaACurr+%d: %d, SignFlag=%d \n", i, *((uint32*)&TermSharedData.PhaACurr+i), SignFlag);

        index += X;
    }
}

/*******************************************************************************
* ��������: SaveZeroI
* ��������: �洢�������
* �������: Pn                  �������
*           DataLen             ���ݳ���
*           DataBuff            ��������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void SaveZeroI(uint8 DataLen, uint8 *DataBuff)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   X, Y;                               //BCD���ֽ�����BIN���ֽ���
    uint8   SignFlag = 0;

    memcpy(SrcData, &DataBuff[0], 3);

    X = 3;
    Y = 4;
    
    ACCOMM_FMT_LOGCC("SaveZeroI():...... \n");
    
    if(memcmp(SrcData , NO_DATA, X) == 0)
    {
        memset(TempBuff, 0xff, Y);
    }
    else
    {
        if((SrcData[X - 1] & 0x80) != 0)          //��
        {
            SrcData[X - 1] &= 0x7F;
            SignFlag = 1;
        }
        else
        {
            SignFlag = 0;
        }

        //3�ֽ�BCDת��Ϊ4�ֽ�BIN
        BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
    }
    if(SignFlag ==  1)                      //��
    {
        *(uint32*)&TempBuff |= 0x80000000;
    }

    memcpy((uint8*)&TermSharedData.PhaMCurr, TempBuff, Y);
    gPriMeasurePara.CurrentSignType.ZeroCurrentFlag = SignFlag;

    ACCOMM_FMT_LOGCC("TermSharedData.PhaMCurr: %d \n", TermSharedData.PhaMCurr);

}

/*******************************************************************************
* ��������: SavePf
* ��������: �洢��������
* �������: Pn                  �������
*           DataLen             ���ݳ���
*           DataBuff            ��������
*           OffsetId            �����ֵ�ƫ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void SavePf(uint8 DataLen, uint8 *DataBuff)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   SignFlag = 0;                       //���ű�־(0:�� / 1:��)
    uint8   CurIndex = 0;                       //��ǰƫ��
    uint8   X, Y;                               //BCD���ֽ�����BIN���ֽ���
    int i;

    X = 2;
    Y = 2;

    ACCOMM_FMT_LOGCC("SavePf():...... \n");

    for(i = 0; i < 4; i++)
    {
        memcpy(SrcData, &DataBuff[CurIndex], X);

        if(memcmp(SrcData , NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //��
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        CurIndex += X;
        if(i == 0)                              //�ܹ�������
        {
            if(SignFlag == 1)
            {
                gPriMeasurePara.PowerFactorType.AppPfFlag = 1;
            }
            else
            {
                gPriMeasurePara.PowerFactorType.AppPfFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PowerFactor, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PowerFactor: %d, SignFlag=%d \n", TermSharedData.PowerFactor, SignFlag);
        }
        else if(i == 1) /* A�๦������*/
        {
            if(SignFlag == 1)                   //��
            {
                gPriMeasurePara.PowerFactorType.PhaAAppPfFlag = 1;
            }
            else
            {
                gPriMeasurePara.PowerFactorType.PhaAAppPfFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaAPowerFactor, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaAPowerFactor: %d, SignFlag=%d \n", TermSharedData.PhaAPowerFactor, SignFlag);
        }
        else if(i == 2) /* B�๦������*/
        {
            if(SignFlag == 1)                   //��
            {
                gPriMeasurePara.PowerFactorType.PhaBAppPfFlag = 1;
            }
            else
            {
                gPriMeasurePara.PowerFactorType.PhaBAppPfFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaBPowerFactor, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaBPowerFactor: %d, SignFlag=%d \n", TermSharedData.PhaBPowerFactor, SignFlag);
        }
        else if(i == 3) /* C�๦������*/
        {
            if(SignFlag == 1)                   //��
            {
                gPriMeasurePara.PowerFactorType.PhaCAppPfFlag = 1;
            }
            else
            {
                gPriMeasurePara.PowerFactorType.PhaCAppPfFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaCPowerFactor, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaCPowerFactor: %d, SignFlag=%d \n", TermSharedData.PhaCPowerFactor, SignFlag);
        }
    }
}

/*******************************************************************************
* ��������: SaveAngle
* ��������: �洢���
* �������:
*           DataLen             ���ݳ���
*           DataBuff            ��������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void SaveAngle(uint8 DataLen, uint8 *DataBuff, uint32	DI07)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   SignFlag = 0;                       //���ű�־(0:�� / 1:��)
    uint8   X, Y;                               //BCD���ֽ�����BIN���ֽ���
    uint8   i = 0, index = 0;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    X = 2;
    Y = 2;

    ACCOMM_FMT_LOGCC("SaveAngle():...... \n");

    switch (DI07)
    {
        case 0x0207FF00:  //��ѹ�������
        {
            for(i = 0; i < 3; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    if((SrcData[X - 1] & 0x80) != 0)   //��
                    {
                        SrcData[X - 1] &= 0x7F;
                        SignFlag = 1;
                    }
                    else
                    {
                        SignFlag = 0;
                    }
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                index += X;

                if(i == 0)
                {
                    if(SignFlag == 1)                   //��
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaAVolCurrAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaAVolCurrAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaAAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() ��ѹ������� TermSharedData.PhaAAngle: %d \n", TermSharedData.PhaAAngle);		        
                }
                else if(i == 1)
                {
                    if(SignFlag == 1)                   //��
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBVolCurrAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBVolCurrAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaBAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() ��ѹ������� TermSharedData.PhaBAngle: %d \n", TermSharedData.PhaBAngle);		        
                }
                else if(i == 2)
                {
                    if(SignFlag == 1)                   //��
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCVolCurrAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCVolCurrAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaCAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() ��ѹ������� TermSharedData.PhaCAngle: %d \n", TermSharedData.PhaCAngle);		        
                }
            }
        }
            break;

        case 0xEFB10000:  //��ѹ���
        {
            for(i = 0; i < 3; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    if((SrcData[X - 1] & 0x80) != 0)   //��
                    {
                        SrcData[X - 1] &= 0x7F;
                        SignFlag = 1;
                    }
                    else
                    {
                        SignFlag = 0;
                    }
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                index += X;

                if(i == 0)
                {
                    if(SignFlag == 1)                   //��
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaAVolAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaAVolAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaAVoltAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() ��ѹ��� TermSharedData.PhaAVoltAngle: %d \n", TermSharedData.PhaAVoltAngle);		        
                }
                else if(i == 1)
                {
                    if(SignFlag == 1)                   //��
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBVolAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBVolAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaBVoltAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() ��ѹ��� TermSharedData.PhaBVoltAngle: %d \n", TermSharedData.PhaBVoltAngle);		        
                }
                else if(i == 2)
                {
                    if(SignFlag == 1)                   //��
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCVolAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCVolAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaCVoltAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() ��ѹ��� TermSharedData.PhaCVoltAngle: %d \n", TermSharedData.PhaCVoltAngle);		        
                }
            }
        }
            break;

        case 0xEFB20000:  //�������
        {
            for(i = 0; i < 3; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    if((SrcData[X - 1] & 0x80) != 0)   //��
                    {
                        SrcData[X - 1] &= 0x7F;
                        SignFlag = 1;
                    }
                    else
                    {
                        SignFlag = 0;
                    }
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                index += X;

                if(i == 0)
                {
                    if(SignFlag == 1)                   //��
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaACurrAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaACurrAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaACurrAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() ������� TermSharedData.PhaACurrAngle: %d \n", TermSharedData.PhaACurrAngle);		        
                }
                else if(i == 1)
                {
                    if(SignFlag == 1)                   //��
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBCurrAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBCurrAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaBCurrAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() ������� TermSharedData.PhaBCurrAngle: %d \n", TermSharedData.PhaBCurrAngle);		        
                }
                else if(i == 2)
                {
                    if(SignFlag == 1)                   //��
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCCurrAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCCurrAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaCCurrAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() ������� TermSharedData.PhaCCurrAngle: %d \n", TermSharedData.PhaCCurrAngle);		        
                }
            }
        }
            break;

        case 0x028800FF:  //��ѹ��� �� �������
        {
            for(i = 0; i < 6; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    if((SrcData[X - 1] & 0x80) != 0)   //��
                    {
                        SrcData[X - 1] &= 0x7F;
                        SignFlag = 1;
                    }
                    else
                    {
                        SignFlag = 0;
                    }
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                index += X;

                if(i == 0)
                {
                    if(SignFlag == 1)                   //��
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaAVolAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaAVolAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaAVoltAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() case 0x028800FF: ��ѹ��� TermSharedData.PhaAVoltAngle: %d \n", TermSharedData.PhaAVoltAngle);		        
                }
                else if(i == 1)
                {
                    if(SignFlag == 1)                   //��
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBVolAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBVolAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaBVoltAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() case 0x028800FF: ��ѹ��� TermSharedData.PhaBVoltAngle: %d \n", TermSharedData.PhaBVoltAngle);		        
                }
                else if(i == 2)
                {
                    if(SignFlag == 1)                   //��
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCVolAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCVolAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaCVoltAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() case 0x028800FF: ��ѹ��� TermSharedData.PhaCVoltAngle: %d \n", TermSharedData.PhaCVoltAngle);		        
                }
                else if(i == 3)
                {
                    if(SignFlag == 1)                   //��
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaACurrAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaACurrAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaACurrAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() case 0x028800FF: ������� TermSharedData.PhaACurrAngle: %d \n", TermSharedData.PhaACurrAngle);		        
                }
                else if(i == 4)
                {
                    if(SignFlag == 1)                   //��
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBCurrAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBCurrAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaBCurrAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() case 0x028800FF: ������� TermSharedData.PhaBCurrAngle: %d \n", TermSharedData.PhaBCurrAngle);		        
                }
                else if(i == 5)
                {
                    if(SignFlag == 1)                   //��
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCCurrAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCCurrAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaCCurrAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() case 0x028800FF: ������� TermSharedData.PhaCCurrAngle: %d \n", TermSharedData.PhaCCurrAngle);		        
                }
            }
        }
            break;

        default:
            return;
    }

}

/*******************************************************************************
* ��������: SaveHarmonic
* ��������: �洢г��
* �������:
*           DataLen             ���ݳ���
*           DataBuff            ��������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void SaveHarmonic(uint8 DataLen, uint8 *DataBuff, uint32	DI07)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   SignFlag = 0;                       //���ű�־(0:�� / 1:��)
    uint8   X, Y;                               //BCD���ֽ�����BIN���ֽ���
    uint8   i = 0, index = 0;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    X = 2;
    Y = 2;

    ACCOMM_FMT_LOGCC("SaveHarmonic():...... \n");

    switch (DI07)
    {
        //г��������
        case 0x020A01FF:  //A���ѹг�������ʣ��ܼ�2��n�Σ�
        {
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy((uint16*)&TermSharedData.PhaAVoltHarmonic[i], TempBuff, Y);

                ACCOMM_FMT_LOGCC("SaveHarmonic() г����Ϣ TermSharedData.PhaAVoltHarmonic[%d]: %d \n", i, TermSharedData.PhaAVoltHarmonic[i]);

                index += X;
            }
        }
            break;

        case 0x020A02FF:  //B���ѹг�������ʣ��ܼ�2��n�Σ�
        {
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy((uint16*)&TermSharedData.PhaBVoltHarmonic[i], TempBuff, Y);

                ACCOMM_FMT_LOGCC("SaveHarmonic() г����Ϣ TermSharedData.PhaBVoltHarmonic[%d]: %d \n", i, TermSharedData.PhaBVoltHarmonic[i]);

                index += X;
            }
        }
            break;

        case 0x020A03FF:  //C���ѹг�������ʣ��ܼ�2��n�Σ�
        {
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy((uint16*)&TermSharedData.PhaCVoltHarmonic[i], TempBuff, Y);

                ACCOMM_FMT_LOGCC("SaveHarmonic() г����Ϣ TermSharedData.PhaCVoltHarmonic[%d]: %d \n", i, TermSharedData.PhaCVoltHarmonic[i]);

                index += X;
            }
        }
            break;

        case 0x020B01FF:  //A�����г�������ʣ��ܼ�2��n�Σ�
        {
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy((uint16*)&TermSharedData.PhaACurrHarmonic[i], TempBuff, Y);

                ACCOMM_FMT_LOGCC("SaveHarmonic() г����Ϣ TermSharedData.PhaACurrHarmonic[%d]: %d \n", i, TermSharedData.PhaACurrHarmonic[i]);

                index += X;
            }
        }
            break;

        case 0x020B02FF:  //B�����г�������ʣ��ܼ�2��n�Σ�
        {
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy((uint16*)&TermSharedData.PhaBCurrHarmonic[i], TempBuff, Y);

                ACCOMM_FMT_LOGCC("SaveHarmonic() г����Ϣ TermSharedData.PhaBCurrHarmonic[%d]: %d \n", i, TermSharedData.PhaBCurrHarmonic[i]);

                index += X;
            }
        }
            break;

        case 0x020B03FF:  //C�����г�������ʣ��ܼ�2��n�Σ�	    
        {
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy((uint16*)&TermSharedData.PhaCCurrHarmonic[i], TempBuff, Y);

                ACCOMM_FMT_LOGCC("SaveHarmonic() г����Ϣ TermSharedData.PhaCCurrHarmonic[%d]: %d \n", i, TermSharedData.PhaCCurrHarmonic[i]);

                index += X;
            }
        }
            break;


        //г������
        case 0x020C01FF:  //A���ѹг���������ܼ�2��n�Σ�
        {
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy((uint16*)&TermSharedData.PhaAVoltHarmonicValue[i], TempBuff, Y);

                ACCOMM_FMT_LOGCC("SaveHarmonic() г����Ϣ TermSharedData.PhaAVoltHarmonicValue[%d]: %d \n", i, TermSharedData.PhaAVoltHarmonicValue[i]);

                index += X;
            }
        }
            break;

        case 0x020C02FF:  //B���ѹг���������ܼ�2��n�Σ�
        {
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy((uint16*)&TermSharedData.PhaBVoltHarmonicValue[i], TempBuff, Y);

                ACCOMM_FMT_LOGCC("SaveHarmonic() г����Ϣ TermSharedData.PhaBVoltHarmonicValue[%d]: %d \n", i, TermSharedData.PhaBVoltHarmonicValue[i]);

                index += X;
            }
        }
            break;

        case 0x020C03FF:  //C���ѹг���������ܼ�2��n�Σ�
        {
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy((uint16*)&TermSharedData.PhaCVoltHarmonicValue[i], TempBuff, Y);

                ACCOMM_FMT_LOGCC("SaveHarmonic() г����Ϣ TermSharedData.PhaCVoltHarmonicValue[%d]: %d \n", i, TermSharedData.PhaCVoltHarmonicValue[i]);

                index += X;
            }
        }
            break;

        case 0x020D01FF:  //A�����г���������ܼ�2��n�Σ�
        {
            X = 3;
            Y = 4;
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    if((SrcData[X - 1] & 0x80) != 0)   //��
                    {
                        SrcData[X - 1] &= 0x7F;
                        SignFlag = 1;
                    }
                    else
                    {
                        SignFlag = 0;
                    }
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy(&TermSharedData.PhaACurrHarmonicValue[i], TempBuff, Y);
                gPriMeasurePara.HarmonicRateValueType.PhaACurrHarmonicValueFlag[i] = SignFlag;
                TermSharedData.PhaACurrHarmonicValue[i] = 10*TermSharedData.PhaACurrHarmonicValue[i];

                ACCOMM_FMT_LOGCC("SaveHarmonic() г����Ϣ TermSharedData.PhaACurrHarmonicValue[%d]: %d \n", i, TermSharedData.PhaACurrHarmonicValue[i]);

                index += X;
            }
        }
            break;

        case 0x020D02FF:  //B�����г���������ܼ�2��n�Σ�
        {
            X = 3;
            Y = 4;
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    if((SrcData[X - 1] & 0x80) != 0)   //��
                    {
                        SrcData[X - 1] &= 0x7F;
                        SignFlag = 1;
                    }
                    else
                    {
                        SignFlag = 0;
                    }
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy(&TermSharedData.PhaBCurrHarmonicValue[i], TempBuff, Y);
                gPriMeasurePara.HarmonicRateValueType.PhaBCurrHarmonicValueFlag[i] = SignFlag;
                TermSharedData.PhaBCurrHarmonicValue[i] = 10*TermSharedData.PhaBCurrHarmonicValue[i];

                ACCOMM_FMT_LOGCC("SaveHarmonic() г����Ϣ TermSharedData.PhaBCurrHarmonicValue[%d]: %d \n", i, TermSharedData.PhaBCurrHarmonicValue[i]);

                index += X;
            }
        }
            break;

        case 0x020D03FF:  //C�����г���������ܼ�2��n�Σ�		
        {
            X = 3;
            Y = 4;
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    if((SrcData[X - 1] & 0x80) != 0)   //��
                    {
                        SrcData[X - 1] &= 0x7F;
                        SignFlag = 1;
                    }
                    else
                    {
                        SignFlag = 0;
                    }
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy(&TermSharedData.PhaCCurrHarmonicValue[i], TempBuff, Y);
                gPriMeasurePara.HarmonicRateValueType.PhaCCurrHarmonicValueFlag[i] = SignFlag;
                TermSharedData.PhaCCurrHarmonicValue[i] = 10*TermSharedData.PhaCCurrHarmonicValue[i];

                ACCOMM_FMT_LOGCC("SaveHarmonic() г����Ϣ TermSharedData.PhaCCurrHarmonicValue[%d]: %d \n", i, TermSharedData.PhaCCurrHarmonicValue[i]);

                index += X;
            }
        }
            break;
            

        default:
            return;
    }
}

/*******************************************************************************
* ��������: SaveTopo
* ��������: �洢����ʶ����
* �������: Pn                  �������
*           DataLen             ���ݳ���
*           DataBuff            ��������
*           DataItemLen         �������
*           OffsetId            �����ֵ�ƫ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void SaveTopo(uint8 DataLen, uint8 *DataBuff, uint32	DI07)
{
    float32 fSrcValue = 0;
    uint32  nDstValue = 0;
    uint8   SignFlag = 0;                       //���ű�־(0:�� / 1:��)
    uint8   X, Y;                               //BCD���ֽ�����BIN���ֽ���
    uint8   i = 0, index = 0;
    uint8   ConfirmBuf[MAX_QUEUE_BUFF_LEN] = {0};     //ȷ��Ӧ��֡
    int     ConfirmBufLen = 0;
    int     ret = 0;

    ACCOMM_FMT_DEBUG("SaveTopo():...... \n");

    uint8 MinLen = 6+1+4+4+4+1+2;
    if(DataLen < MinLen)
    {
        ACCOMM_FMT_DEBUG("SaveTopo() if(DataLen < MinLen) MinLen=%d \n", MinLen);
        return;
    }

    //ʶ��ɹ�ʱ��(ֱ�Ӳ����ն�ʱ��)
    time_t tStartTime;//����ʱ��
    time(&tStartTime); /*��ȡtime_t���͵ĵ�ǰʱ��*/
    systm_to_ooptm(tStartTime, &TermSharedData.TopoIdenResult.stIdenTime);  //�¼�����ʱ��
    index += 6;//(����6�ֽڱ���)

    //�ź�������λ 1�ֽ�, 0:δ֪, 1:A��, 2:B��, 3:C��, 4:����
    memcpy(&TermSharedData.TopoIdenResult.nPhase, DataBuff + index, 1);
    index += 1;

    //�ź�ǿ��S1 4�ֽ� �����Ը������ϱ�
    //�ź�ǿ��S2 4�ֽ� �����Ը������ϱ�
    //�������� 4�ֽ� �����Ը������ϱ�
    X = 4;
    Y = 4;
    for(i = 0; i < 3; i++)
    {
        memcpy((uint8 *)&fSrcValue, DataBuff + index, X);
        if(fSrcValue < 0)
        {
            SignFlag = 1;
            nDstValue = (uint32)(0 - fSrcValue);
        }
        else
        {
            SignFlag = 0;
            nDstValue = (uint32)(fSrcValue);
        }

        if(i == 0)
        {
            memcpy(&TermSharedData.TopoIdenResult.nSignalS1, &nDstValue, Y);
            gPriMeasurePara.SignalIdenType.SignalS1Flag = SignFlag;
        }
        else if(i == 1)
        {
            memcpy(&TermSharedData.TopoIdenResult.nSignalS2, &nDstValue, Y);
            gPriMeasurePara.SignalIdenType.SignalS2Flag = SignFlag;
        }
        else if(i == 2)
        {
            memcpy(&TermSharedData.TopoIdenResult.nNoise, &nDstValue, Y);
            gPriMeasurePara.SignalIdenType.NoiseFlag = SignFlag;
        }

        index += X;
    }

    //ʶ���źų��� 1�ֽ�
    memcpy(&TermSharedData.TopoIdenResult.nInfoLen, DataBuff + index, 1);
    index += 1;

    //ʶ���ź����� �䳤
    memcpy(TermSharedData.TopoIdenResult.aInfo, DataBuff + index, TermSharedData.TopoIdenResult.nInfoLen);
    index += TermSharedData.TopoIdenResult.nInfoLen;

    ACCOMM_FMT_DEBUG("nFlag=%d, nPhase=%d, nSignalS1=%u, SignalS2Flag=%d, nSignalS2=%u, SignalS2Flag=%d, nNoise=%u, NoiseFlag=%d, nInfoLen=%d \n", 
        TermSharedData.TopoIdenResult.nFlag, TermSharedData.TopoIdenResult.nPhase, TermSharedData.TopoIdenResult.nSignalS1, 
        gPriMeasurePara.SignalIdenType.SignalS1Flag, TermSharedData.TopoIdenResult.nSignalS2, gPriMeasurePara.SignalIdenType.SignalS2Flag, 
        TermSharedData.TopoIdenResult.nNoise, gPriMeasurePara.SignalIdenType.NoiseFlag, TermSharedData.TopoIdenResult.nInfoLen);
    ACCOMM_BUF_TRACE(TermSharedData.TopoIdenResult.aInfo, TermSharedData.TopoIdenResult.nInfoLen, "info:");

    //�����¼����ϱ�
    ACDB_FMT_DEBUG("SaveTopo()���: ��������ʶ���¼�! \n");
    InitEventPara();
    //SaveTopoEventOccur();
    SaveTopoResultRecord();

    //��װȷ��Ӧ��֡
    ConfirmBufLen = make_frame_confirm(DataBuff, DataLen, DI07, ConfirmBuf);
    if(ConfirmBufLen > 0)
    {
        ret = PlcWriteQueue(pConfirmDown, ConfirmBuf, ConfirmBufLen);
        ACCOMM_FMT_DEBUG("����ȷ��Ӧ����, ConfirmBufLen=%d, ret=%d \n", ConfirmBufLen, ret);
    }
}

/*******************************************************************************
* ��������: SavePara
* ��������: �洢����
* �������:
*           DataLen             ���ݳ���
*           DataBuff            ��������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void SavePara(uint8 DataLen, uint8 *DataBuff, uint32	DI07)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   X, Y;                               //BCD���ֽ�����BIN���ֽ���
    uint8   i = 0, index = 0;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    X = 1;
    Y = 1;

    ACCOMM_FMT_TRACE("SavePara():...... \n");

    switch (DI07)
    {
        case 0x04000103:             //�����������
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy(&TermSharedData.MaxDemandPeriodRD, TempBuff, Y);

            ACCOMM_FMT_TRACE("SavePara() ������Ϣ MaxDemandPeriodRD: %u \n", TermSharedData.MaxDemandPeriodRD);
        }
            break;

        case 0x04000104:             //����ʱ��
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy(&TermSharedData.SlipDiffTimeRD, TempBuff, Y);

            ACCOMM_FMT_TRACE("SavePara() ������Ϣ SlipDiffTimeRD: %u \n", TermSharedData.SlipDiffTimeRD);
        }
            break;

        case 0x04000106:			   //������ʱ�����л�ʱ��
        {
            X = 5;
            Y = 5;
            uint8 nYear = 0;
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(1, 1, &TempBuff[0], &SrcData[0], 1);
                BcdX_To_BinY(1, 1, &TempBuff[1], &SrcData[1], 1);
                BcdX_To_BinY(1, 1, &TempBuff[2], &SrcData[2], 1);
                BcdX_To_BinY(1, 1, &TempBuff[3], &SrcData[3], 1);
                BcdX_To_BinY(1, 1, &TempBuff[4], &SrcData[4], 1);
            }
            memcpy(&TermSharedData.AddiZoneChangeTimeRD.minute, &TempBuff[0], 1);
            memcpy(&TermSharedData.AddiZoneChangeTimeRD.hour, &TempBuff[1], 1);
            memcpy(&TermSharedData.AddiZoneChangeTimeRD.day, &TempBuff[2], 1);
            memcpy(&TermSharedData.AddiZoneChangeTimeRD.month, &TempBuff[3], 1);
            memcpy(&nYear, &TempBuff[4], 1);
            TermSharedData.AddiZoneChangeTimeRD.year = 2000 + nYear;

            ACCOMM_FMT_TRACE("SavePara() ������ʱ�����л�ʱ�� minute=%u, hour=%u, day=%u, month=%u, year=%u \n", 
                TermSharedData.AddiZoneChangeTimeRD.minute, TermSharedData.AddiZoneChangeTimeRD.hour, 
                TermSharedData.AddiZoneChangeTimeRD.day, TermSharedData.AddiZoneChangeTimeRD.month, 
                TermSharedData.AddiZoneChangeTimeRD.year);
        }
            break;

        case 0x04000107:			   //��������ʱ���л�ʱ��
        {
            X = 5;
            Y = 5;
            uint8 nYear = 0;
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(1, 1, &TempBuff[0], &SrcData[0], 1);
                BcdX_To_BinY(1, 1, &TempBuff[1], &SrcData[1], 1);
                BcdX_To_BinY(1, 1, &TempBuff[2], &SrcData[2], 1);
                BcdX_To_BinY(1, 1, &TempBuff[3], &SrcData[3], 1);
                BcdX_To_BinY(1, 1, &TempBuff[4], &SrcData[4], 1);
            }
            memcpy(&TermSharedData.AddiPeriChangeTimeRD.minute, &TempBuff[0], 1);
            memcpy(&TermSharedData.AddiPeriChangeTimeRD.hour, &TempBuff[1], 1);
            memcpy(&TermSharedData.AddiPeriChangeTimeRD.day, &TempBuff[2], 1);
            memcpy(&TermSharedData.AddiPeriChangeTimeRD.month, &TempBuff[3], 1);
            memcpy(&nYear, &TempBuff[4], 1);
            TermSharedData.AddiPeriChangeTimeRD.year = 2000 + nYear;

            ACCOMM_FMT_TRACE("SavePara() ��������ʱ���л�ʱ�� minute=%u, hour=%u, day=%u, month=%u, year=%u \n", 
                TermSharedData.AddiPeriChangeTimeRD.minute, TermSharedData.AddiPeriChangeTimeRD.hour, 
                TermSharedData.AddiPeriChangeTimeRD.day, TermSharedData.AddiPeriChangeTimeRD.month, 
                TermSharedData.AddiPeriChangeTimeRD.year);
        }
            break;

        case 0x04000201:             //��ʱ���� ��14
        {
            uint8 nNum = 0;
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy(&nNum, TempBuff, Y);

            TermSharedData.ZonePeriNumRD.yPeriod = nNum;
            TermSharedData.CurrZoneTableRD.nNum = nNum;
            TermSharedData.AddiZoneTableRD.nNum = nNum;
            
            ACCOMM_FMT_TRACE("SavePara() ������Ϣ ��ʱ����(��14): %u \n", nNum);
        }
            break;
        case 0x04000202:             //��ʱ�α��� ��8
        {
            uint8 nNum = 0;
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy(&nNum, TempBuff, Y);

            TermSharedData.ZonePeriNumRD.dPeriodMet = nNum;
            TermSharedData.CurrDayPeriTableRD.nNum = nNum;
            TermSharedData.AddiDayPeriTableRD.nNum = nNum;
            
            ACCOMM_FMT_TRACE("SavePara() ������Ϣ ��ʱ�α���(��8): %u \n", nNum);
        }
            break;
        case 0x04000203:             //��ʱ���� ��14
        {
            int i = 0;
            uint8 nNum = 0;
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy(&nNum, TempBuff, Y);

            TermSharedData.ZonePeriNumRD.dPeriod = nNum;
            for(i=0; i<TermSharedData.CurrDayPeriTableRD.nNum; i++)
            {
                TermSharedData.CurrDayPeriTableRD.dPeriod[i].nNum = nNum;
            }
            for(i=0; i<TermSharedData.AddiDayPeriTableRD.nNum; i++)
            {
                TermSharedData.AddiDayPeriTableRD.dPeriod[i].nNum = nNum;
            }
            
            ACCOMM_FMT_TRACE("SavePara() ������Ϣ ��ʱ����(��14): %u \n", nNum);
        }
            break;
        case 0x04000204:             //������ ��63
        {
            uint8 nNum = 0;
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy(&nNum, TempBuff, Y);

            TermSharedData.ZonePeriNumRD.rate = nNum;
            
            ACCOMM_FMT_TRACE("SavePara() ������Ϣ ������(��63): %u \n", nNum);
        }
            break;
        case 0x04000205:             //���������� ��254
        {
            uint8 nNumH = 0;
            uint8 nNumL = 0;
            X = 2;
            Y = 2;
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(1, 1, &TempBuff[0], &SrcData[0], 1);
                BcdX_To_BinY(1, 1, &TempBuff[1], &SrcData[1], 1);
            }
            memcpy(&nNumL, &TempBuff[0], 1);
            memcpy(&nNumH, &TempBuff[1], 1);

            TermSharedData.ZonePeriNumRD.pubHoliday = nNumL;
            
            ACCOMM_FMT_TRACE("SavePara() ������Ϣ ����������(��254): %u \n", nNumL);
        }
            break;

        case 0x04000601:			   //�й���Ϸ�ʽ������
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                //�����ֲ���Ҫת��, ֱ��ȡֵ
                //BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                memcpy(TempBuff, SrcData, Y);
            }
            memcpy(&TermSharedData.ActPowCombWordRD.nValue, TempBuff, Y);

            ACCOMM_FMT_TRACE("SavePara() ������Ϣ ActPowCombWordRD: %u \n", TermSharedData.ActPowCombWordRD.nValue);
        }
            break;

        case 0x04000602:			   //�޹���Ϸ�ʽ1������
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                //�����ֲ���Ҫת��, ֱ��ȡֵ
                //BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                memcpy(TempBuff, SrcData, Y);
            }
            memcpy(&TermSharedData.ReaPowComb1WordRD.nValue, TempBuff, Y);

            ACCOMM_FMT_TRACE("SavePara() ������Ϣ ReaPowComb1WordRD: %u \n", TermSharedData.ReaPowComb1WordRD.nValue);
        }
            break;

        case 0x04000603:			   //�޹���Ϸ�ʽ2������
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                //�����ֲ���Ҫת��, ֱ��ȡֵ
                //BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                memcpy(TempBuff, SrcData, Y);
            }
            memcpy(&TermSharedData.ReaPowComb2WordRD.nValue, TempBuff, Y);

            ACCOMM_FMT_TRACE("SavePara() ������Ϣ ReaPowComb2WordRD: %u \n", TermSharedData.ReaPowComb2WordRD.nValue);
        }
            break;

        case 0x04010000:             //��ǰ��ʱ����(���14��ʱ��)
        {
            uint8 nZoneCount = DataLen/3;//ÿ��ʱ������ʱ�α�š��ա���3���ֽ�
            TermSharedData.CurrZoneTableRD.nNum = nZoneCount;

            for(i=0; i<nZoneCount; i++)
            {
                memcpy(SrcData, DataBuff + index, 3);//3���ֽ�����Ϊ��š��ա���

                if(memcmp(SrcData, NO_DATA, 3) == 0)
                {
                    memset(TempBuff, 0xff, 3);
                }
                else
                {
                    //���
                    BcdX_To_BinY(X, Y, &TempBuff[0], &SrcData[0], 1);
                    
                    //��
                    BcdX_To_BinY(X, Y, &TempBuff[1], &SrcData[1], 1);

                    //��
                    BcdX_To_BinY(X, Y, &TempBuff[2], &SrcData[2], 1);
                }
                memcpy(&TermSharedData.CurrZoneTableRD.period[i].meterNum, &TempBuff[0], Y);
                memcpy(&TermSharedData.CurrZoneTableRD.period[i].day, &TempBuff[1], Y);
                memcpy(&TermSharedData.CurrZoneTableRD.period[i].mon, &TempBuff[2], Y);

                ACCOMM_FMT_TRACE("SavePara() ������Ϣ CurrZoneTableRD i=%d, nNum=%d, meterNum=%d, day=%d, mon=%d \n", 
                    i, TermSharedData.CurrZoneTableRD.nNum, TermSharedData.CurrZoneTableRD.period[i].meterNum, 
                    TermSharedData.CurrZoneTableRD.period[i].day, TermSharedData.CurrZoneTableRD.period[i].mon);

                index += 3;
            }
            
        }
            break;

        case 0x04020000:             //������ʱ����(���14��ʱ��)
        {
            uint8 nZoneCount = DataLen/3;//ÿ��ʱ������ʱ�α�š��ա���3���ֽ�
            TermSharedData.AddiZoneTableRD.nNum = nZoneCount;

            for(i=0; i<nZoneCount; i++)
            {
                memcpy(SrcData, DataBuff + index, 3);//3���ֽ�����Ϊ��š��ա���

                if(memcmp(SrcData, NO_DATA, 3) == 0)
                {
                    memset(TempBuff, 0xff, 3);
                }
                else
                {
                    //���
                    BcdX_To_BinY(X, Y, &TempBuff[0], &SrcData[0], 1);
                    
                    //��
                    BcdX_To_BinY(X, Y, &TempBuff[1], &SrcData[1], 1);

                    //��
                    BcdX_To_BinY(X, Y, &TempBuff[2], &SrcData[2], 1);
                }
                memcpy(&TermSharedData.AddiZoneTableRD.period[i].meterNum, &TempBuff[0], Y);
                memcpy(&TermSharedData.AddiZoneTableRD.period[i].day, &TempBuff[1], Y);
                memcpy(&TermSharedData.AddiZoneTableRD.period[i].mon, &TempBuff[2], Y);

                ACCOMM_FMT_TRACE("SavePara() ������Ϣ AddiZoneTableRD i=%d, nNum=%d, meterNum=%d, day=%d, mon=%d \n", 
                    i, TermSharedData.AddiZoneTableRD.nNum, TermSharedData.AddiZoneTableRD.period[i].meterNum, 
                    TermSharedData.AddiZoneTableRD.period[i].day, TermSharedData.AddiZoneTableRD.period[i].mon);

                index += 3;
            }
            
        }
            break;

        case 0x04010001:             //��ǰ����ʱ�α�(���8��, ��1��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04010002:             //��ǰ����ʱ�α�(���8��, ��2��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04010003:             //��ǰ����ʱ�α�(���8��, ��3��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04010004:             //��ǰ����ʱ�α�(���8��, ��4��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04010005:             //��ǰ����ʱ�α�(���8��, ��5��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04010006:             //��ǰ����ʱ�α�(���8��, ��6��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04010007:             //��ǰ����ʱ�α�(���8��, ��7��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04010008:             //��ǰ����ʱ�α�(���8��, ��8��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        {
            uint8 nTableIndex = (uint32)(DI07 & 0x000000ff) - 1;
            uint8 nPeriodCount = DataLen/3;//ÿ��ʱ�ξ��з��ʺš��֡�ʱ3���ֽ�
            TermSharedData.CurrDayPeriTableRD.dPeriod[nTableIndex].nNum = nPeriodCount;
            
            for(i = 0; i < nPeriodCount; i++)
            {
                memcpy(SrcData, DataBuff + index, 3);//3���ֽ�����Ϊ��š��ա���

                if(memcmp(SrcData, NO_DATA, 3) == 0)
                {
                    memset(TempBuff, 0xff, 3);
                }
                else
                {
                    //���ʺ�
                    BcdX_To_BinY(X, Y, &TempBuff[0], &SrcData[0], 1);
                    
                    //����
                    BcdX_To_BinY(X, Y, &TempBuff[1], &SrcData[1], 1);

                    //ʱ
                    BcdX_To_BinY(X, Y, &TempBuff[2], &SrcData[2], 1);
                }
                memcpy(&TermSharedData.CurrDayPeriTableRD.dPeriod[nTableIndex].period[i].rateNO, &TempBuff[0], Y);
                memcpy(&TermSharedData.CurrDayPeriTableRD.dPeriod[nTableIndex].period[i].min, &TempBuff[1], Y);
                memcpy(&TermSharedData.CurrDayPeriTableRD.dPeriod[nTableIndex].period[i].hour, &TempBuff[2], Y);

                ACCOMM_FMT_TRACE("SavePara() ������Ϣ CurrDayPeriTableRD nTableIndex=%d, nNum=%d, i=%d, periodNum=%d, rateNO=%d, min=%d, hour=%d \n", 
                    nTableIndex, TermSharedData.CurrDayPeriTableRD.nNum, i, TermSharedData.CurrDayPeriTableRD.dPeriod[nTableIndex].nNum, 
                    TermSharedData.CurrDayPeriTableRD.dPeriod[nTableIndex].period[i].rateNO, TermSharedData.CurrDayPeriTableRD.dPeriod[nTableIndex].period[i].min, 
                    TermSharedData.CurrDayPeriTableRD.dPeriod[nTableIndex].period[i].hour);

                index += 3;
            }

            g_nRefreshCurrDayFlagArr[nTableIndex] = 1;//��ˢ��
        }
            break;

        case 0x04020001:             //��������ʱ�α�(���8��, ��1��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04020002:             //��������ʱ�α�(���8��, ��2��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04020003:             //��������ʱ�α�(���8��, ��3��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04020004:             //��������ʱ�α�(���8��, ��4��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04020005:             //��������ʱ�α�(���8��, ��5��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04020006:             //��������ʱ�α�(���8��, ��6��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04020007:             //��������ʱ�α�(���8��, ��7��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04020008:             //��������ʱ�α�(���8��, ��8��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        {
            uint8 nTableIndex = (uint32)(DI07 & 0x000000ff) - 1;
            uint8 nPeriodCount = DataLen/3;//ÿ��ʱ�ξ��з��ʺš��֡�ʱ3���ֽ�
            TermSharedData.AddiDayPeriTableRD.dPeriod[nTableIndex].nNum = nPeriodCount;
            
            for(i = 0; i < nPeriodCount; i++)
            {
                memcpy(SrcData, DataBuff + index, 3);//3���ֽ�����Ϊ��š��ա���

                if(memcmp(SrcData, NO_DATA, 3) == 0)
                {
                    memset(TempBuff, 0xff, 3);
                }
                else
                {
                    //���ʺ�
                    BcdX_To_BinY(X, Y, &TempBuff[0], &SrcData[0], 1);
                    
                    //����
                    BcdX_To_BinY(X, Y, &TempBuff[1], &SrcData[1], 1);

                    //ʱ
                    BcdX_To_BinY(X, Y, &TempBuff[2], &SrcData[2], 1);
                }
                memcpy(&TermSharedData.AddiDayPeriTableRD.dPeriod[nTableIndex].period[i].rateNO, &TempBuff[0], Y);
                memcpy(&TermSharedData.AddiDayPeriTableRD.dPeriod[nTableIndex].period[i].min, &TempBuff[1], Y);
                memcpy(&TermSharedData.AddiDayPeriTableRD.dPeriod[nTableIndex].period[i].hour, &TempBuff[2], Y);

                ACCOMM_FMT_TRACE("SavePara() ������Ϣ CurrDayPeriTableRD nTableIndex=%d, nNum=%d, i=%d, periodNum=%d, rateNO=%d, min=%d, hour=%d \n", 
                    nTableIndex, TermSharedData.AddiDayPeriTableRD.nNum, i, TermSharedData.AddiDayPeriTableRD.dPeriod[nTableIndex].nNum, 
                    TermSharedData.AddiDayPeriTableRD.dPeriod[nTableIndex].period[i].rateNO, TermSharedData.AddiDayPeriTableRD.dPeriod[nTableIndex].period[i].min, 
                    TermSharedData.AddiDayPeriTableRD.dPeriod[nTableIndex].period[i].hour);

                index += 3;
            }

            g_nRefreshAddiDayFlagArr[nTableIndex] = 1;//��ˢ��
        }
            break;

        case 0x04000503:             //��ǰ����ģ�����ʱ�α���������һ��
        {
            memcpy(&TermSharedData.g_nCurrDayPeriLabel, DataBuff + index, 1);
            TermSharedData.g_nCurrDayPeriLabel &= 0x01;
            ACCOMM_FMT_TRACE("SavePara() ������Ϣ g_nCurrDayPeriLabel: %u(0:��һ��; 1:�ڶ���) \n", TermSharedData.g_nCurrDayPeriLabel);
            g_nRefreshCurrDayPeriLabelFlag = 1;//��ˢ��
        }
            break;

        default:
            return;
    }
}

/*******************************************************************************
* ��������:	AmrRtDataSave
* ��������:	����ʵʱ���ݴ洢
* �������:	PrtlID              ��Լ����
*           DataLen             ���ݳ���
*           DataBuff            ��������
* �������:	��
* �� �� ֵ: ��
*******************************************************************************/
void AmrRtDataSave(uint16 DataLen, uint8 *DataBuff)
{
    uint32  DI07;                                //07���ݱ�ʶ
    uint16  Lenoffset = 0;
    uint8   nLen = 0;

    Lenoffset = 0;
    while(Lenoffset < DataLen)
    {
        DI07 = *(uint32*)(&DataBuff[Lenoffset]);
        nLen = DataBuff[Lenoffset+4];
        switch(DI07)
        {
        case 0x0001ff00:                        //(��ǰ)�����й������ܼ��ƽ��ʾֵ
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x0002ff00:                        //(��ǰ)�����й������ܼ��ƽ��ʾֵ
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x0003ff00:                        //(��ǰ)�����޹������ܼ��ƽ��ʾֵ
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x0004ff00:                        //(��ǰ)�����޹������ܼ��ƽ��ʾֵ
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x0005ff00:                        //(��ǰ)��һ�����޹��ܼ��ƽ�ȵ���ʾֵ
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_1_REA, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x0006ff00:                        //(��ǰ)�ڶ������޹��ܼ��ƽ�ȵ���ʾֵ
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_2_REA, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x0007ff00:                        //(��ǰ)���������޹��ܼ��ƽ�ȵ���ʾֵ
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_3_REA, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x0008ff00:                        //(��ǰ)���������޹��ܼ��ƽ�ȵ���ʾֵ
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_4_REA, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00150000:                        //(A��)�����й��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00160000:                        //(A��)�����й��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00170000:                        //(A��)�����޹��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00180000:                        //(A��)�����޹��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00290000:                        //(B��)�����й��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x002a0000:                        //(B��)�����й��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x002b0000:                        //(B��)�����޹��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x002c0000:                        //(B��)�����޹��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x003d0000:                        //(C��)�����й��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_C, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x003e0000:                        //(C��)�����й��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_C, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x003f0000:                        //(C��)�����޹��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_C, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00400000:                        //(C��)�����޹��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_C, 0);
            Lenoffset += (nLen+5);
            break;


        /* ���� ���� begin */
        case 0x0000ff00:                        //(��ǰ)����й����ܿ�
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_CONBIN_ACT, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x0009ff00:                        //(��ǰ)�������ڵ���
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x000aff00:                        //(��ǰ)�������ڵ���
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00190000:                        //A���һ�����޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x002d0000:                        //B���һ�����޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00410000:                        //C���һ�����޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_C, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x001a0000:                        //A��ڶ������޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x002e0000:                        //B��ڶ������޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00420000:                        //C��ڶ������޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_C, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x001b0000:                        //A����������޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x002f0000:                        //B����������޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00430000:                        //C����������޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_C, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x001c0000:                        //A����������޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00300000:                        //B����������޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00440000:                        //C����������޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_C, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x001d0000:                        //A���������ڵ���
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00310000:                        //B���������ڵ���
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00450000:                        //C���������ڵ���
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_C, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x001e0000:                        //A �෴�����ڵ���
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00320000:                        //B�෴�����ڵ���
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00460000:                        //C�෴�����ڵ���
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_C, 0);
            Lenoffset += (nLen+5);
            break;

        /* ���� ���� end */

        /***�߾��� ������ begin***/
        case 0xf101ff00:                        //(��ǰ)�����й������ܼ��ƽ��ʾֵ
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf102ff00:                        //(��ǰ)�����й������ܼ��ƽ��ʾֵ
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf103ff00:                        //(��ǰ)�����޹������ܼ��ƽ��ʾֵ
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf104ff00:                        //(��ǰ)�����޹������ܼ��ƽ��ʾֵ
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf105ff00:                        //(��ǰ)��һ�����޹��ܼ��ƽ�ȵ���ʾֵ
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf106ff00:                        //(��ǰ)�ڶ������޹��ܼ��ƽ�ȵ���ʾֵ
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf107ff00:                        //(��ǰ)���������޹��ܼ��ƽ�ȵ���ʾֵ
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf108ff00:                        //(��ǰ)���������޹��ܼ��ƽ�ȵ���ʾֵ
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1150000:                        //(A��)�����й��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1160000:                        //(A��)�����й��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1170000:                        //(A��)�����޹��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1180000:                        //(A��)�����޹��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1290000:                        //(B��)�����й��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf12a0000:                        //(B��)�����й��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf12b0000:                        //(B��)�����޹��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf12c0000:                        //(B��)�����޹��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf13d0000:                        //(C��)�����й��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_C_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf13e0000:                        //(C��)�����й��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_C_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf13f0000:                        //(C��)�����޹��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_C_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1400000:                        //(C��)�����޹��ܵ���ʾֵ
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_C_HD, 1);
            Lenoffset += (nLen+5);
            break;


        /* ���� ���� begin */
        case 0xf100ff00:                        //(��ǰ)����й����ܿ�
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_CONBIN_ACT_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf109ff00:                        //(��ǰ)�������ڵ���
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf10aff00:                        //(��ǰ)�������ڵ���
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1190000:                        //A���һ�����޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf12d0000:                        //B���һ�����޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1410000:                        //C���һ�����޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_C_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf11a0000:                        //A��ڶ������޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf12e0000:                        //B��ڶ������޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1420000:                        //C��ڶ������޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_C_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf11b0000:                        //A����������޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf12f0000:                        //B����������޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1430000:                        //C����������޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_C_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf11c0000:                        //A����������޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1300000:                        //B����������޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1440000:                        //C����������޹�����
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_C_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf11d0000:                        //A���������ڵ���
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1310000:                        //B���������ڵ���
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1450000:                        //C���������ڵ���
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_C_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf11e0000:                        //A �෴�����ڵ���
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1320000:                        //B�෴�����ڵ���
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1460000:                        //C�෴�����ڵ���
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_C_HD, 1);
            Lenoffset += (nLen+5);
            break;

        /* ���� ���� end */

        /***�߾��� ������ end***/

        case 0x0101ff00:                //(��ǰ)�����й��ܼ��ƽ���������
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT);
            Lenoffset += (nLen+5);
            break;

        case 0x0102ff00:                //(��ǰ)�����й��ܼ��ƽ���������
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT);
            Lenoffset += (nLen+5);
            break;

        case 0x0103ff00:                //(��ǰ)�����޹��ܼ��ƽ���������
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA);
            Lenoffset += (nLen+5);
            break;

        case 0x0104ff00:                //(��ǰ)�����޹��ܼ��ƽ���������
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA);
            Lenoffset += (nLen+5);
            break;
            
        /* ���� ���� begin */
        case 0x0105FF00:                //(��ǰ)��һ�����������
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_1_REA);
            Lenoffset += (nLen+5);
            break;

        case 0x0106FF00:                //(��ǰ)�ڶ������������
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_2_REA);
            Lenoffset += (nLen+5);
            break;

        case 0x0107FF00:                //(��ǰ)���������������
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_3_REA);
            Lenoffset += (nLen+5);
            break;

        case 0x0108FF00:                //(��ǰ)���������������
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_4_REA);
            Lenoffset += (nLen+5);
            break;

        case 0x0109ff00:                //(��ǰ)���������������
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP);
            Lenoffset += (nLen+5);
            break;

        case 0x010aff00:                //(��ǰ)���������������
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP);
            Lenoffset += (nLen+5);
            break;          

        case 0x01150000:                //(��ǰ)A�� �����й�
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_A);
            Lenoffset += (nLen+5);
            break;

        case 0x01290000:                //(��ǰ)B�� �����й�
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_B);
            Lenoffset += (nLen+5);
            break;

        case 0x013d0000:                //(��ǰ)C�� �����й�
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_C);
            Lenoffset += (nLen+5);
            break;

        case 0x01160000:                //(��ǰ)A�� �����й�
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_A);
            Lenoffset += (nLen+5);
            break;

        case 0x012a0000:                //(��ǰ)B�� �����й�
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_B);
            Lenoffset += (nLen+5);
            break;

        case 0x013e0000:                //(��ǰ)C�� �����й�
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_C);
            Lenoffset += (nLen+5);
            break;

        case 0x01170000:                //(��ǰ)A�� ����޹�1
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_A);
            Lenoffset += (nLen+5);
            break;

        case 0x012b0000:                //(��ǰ)B�� ����޹�1
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_B);
            Lenoffset += (nLen+5);
            break;

        case 0x013f0000:                //(��ǰ)C�� ����޹�1
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_C);
            Lenoffset += (nLen+5);
            break;

        case 0x01180000:                //(��ǰ)A�� ����޹�2
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_A);
            Lenoffset += (nLen+5);
            break;

        case 0x012c0000:                //(��ǰ)B�� ����޹�2
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_B);
            Lenoffset += (nLen+5);
            break;

        case 0x01400000:                //(��ǰ)C�� ����޹�2
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_C);
            Lenoffset += (nLen+5);
            break;

        case 0x01190000:                //(��ǰ)A�� ��һ�����������
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_A);
            Lenoffset += (nLen+5);
            break;

        case 0x012d0000:                //(��ǰ)B�� ��һ�����������
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_B);
            Lenoffset += (nLen+5);
            break;

        case 0x01410000:                //(��ǰ)C�� ��һ�����������
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_C);
            Lenoffset += (nLen+5);
            break;

        case 0x011a0000:                //(��ǰ)A�� �ڶ������޹�����
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_A);
            Lenoffset += (nLen+5);
            break;

        case 0x012e0000:                //(��ǰ)B�� �ڶ������޹�����
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_B);
            Lenoffset += (nLen+5);
            break;

        case 0x01420000:                //(��ǰ)C�� �ڶ������޹�����
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_C);
            Lenoffset += (nLen+5);
            break;

        case 0x011b0000:                //(��ǰ)A�� ���������������
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_A);
            Lenoffset += (nLen+5);
            break;

        case 0x012f0000:                //(��ǰ)B�� ���������������
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_B);
            Lenoffset += (nLen+5);
            break;

        case 0x01430000:                //(��ǰ)C�� ���������������
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_C);
            Lenoffset += (nLen+5);
            break;

        case 0x011c0000:                //(��ǰ)A�� ���������޹�����
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_A);
            Lenoffset += (nLen+5);
            break;

        case 0x01300000:                //(��ǰ)B�� ���������޹�����
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_B);
            Lenoffset += (nLen+5);
            break;

        case 0x01440000:                //(��ǰ)C�� ���������޹�����
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_C);
            Lenoffset += (nLen+5);
            break;

        case 0x011d0000:                //(��ǰ)A�� ���������������
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_A);
            Lenoffset += (nLen+5);
            break;

        case 0x01310000:                //(��ǰ)B�� ���������������
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_B);
            Lenoffset += (nLen+5);
            break;

        case 0x01450000:                //(��ǰ)C�� ���������������
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_C);
            Lenoffset += (nLen+5);
            break;

        case 0x011e0000:                //(��ǰ)A�� �������ڵ���
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_A);
            Lenoffset += (nLen+5);
            break;

        case 0x01320000:                //(��ǰ)B�� �������ڵ���
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_B);
            Lenoffset += (nLen+5);
            break;

        case 0x01460000:                //(��ǰ)C�� �������ڵ���
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_C);
            Lenoffset += (nLen+5);
            break;

        /* ���� ���� end */
            

        case 0x0203ff00:                        //(��ǰ)˲ʱ�й�����
            if(nLen > 12)
            {
                nLen = 12;
            }

            SaveP07(nLen, &DataBuff[Lenoffset+5]);
            Lenoffset += (nLen+5);
            break;

        case 0x0204ff00:               //(��ǰ)˲ʱ�޹�����
            if(nLen > 12)
            {
                nLen = 12;
            }
            SaveQ07(nLen, &DataBuff[Lenoffset+5]);
            Lenoffset += (nLen+5);
            break;

        /* ���ڹ��� ���� begin */
        case 0x0205FF00:               //(��ǰ)˲ʱ���ڹ���
            if(nLen > 12)
            {
                nLen = 12;
            }
            SaveS07(nLen, &DataBuff[Lenoffset+5]);
            Lenoffset += (nLen+5);
            break;

        /* ���ڹ��� ���� end*/

        case 0x0201ff00:               //(��ǰ)�����ѹ
            if(nLen > 6)
            {
                nLen = 6;
            }
            SaveU07(nLen, &DataBuff[Lenoffset+5]);
            Lenoffset += (nLen+5);
            break;

        case 0x0202ff00:                        //(��ǰ)�������
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveI07(nLen, &DataBuff[Lenoffset+5]);
            Lenoffset += (nLen+5);
            break;

        case 0x02800001:                        //(��ǰ)�������
            if(nLen > 3)
            {
                nLen = 3;
            }
            SaveZeroI(nLen, &DataBuff[Lenoffset+5]);
            Lenoffset += (nLen+5);
            break;

        case 0x02800002:                        //(��ǰ)����Ƶ��
            if(nLen > 2)
            {
                nLen = 2;
            }
            Savefreq07(nLen, &DataBuff[Lenoffset+5]);
            Lenoffset += (nLen+5);
            break;

        case 0x0206ff00:                        //(��ǰ)˲ʱ�ܼ����๦������
            if(nLen > 8)
            {
                nLen = 8;
            }
            SavePf(nLen, &DataBuff[Lenoffset+5]);
            Lenoffset += (nLen+5);
            break;

        case 0x04000507:  //״̬��7
            if(nLen > 2)
            {
                nLen = 2;
            }
            memcpy((uint16*)&TermSharedData.MetStusPara.Rsw7, &DataBuff[Lenoffset+5], 2);
            Lenoffset += (nLen+5);
            ACCOMM_FMT_LOGCC("AmrRtDataSave() ״̬�� 0x04000507: \n");
            ACCOMM_BUF_LOGCC((uint8*)&TermSharedData.MetStusPara.Rsw7, 2, "״̬��");
            break;

        case 0x04000506:  //״̬��6
            if(nLen > 2)
            {
                nLen = 2;
            }
            memcpy((uint16*)&TermSharedData.MetStusPara.Rsw6, &DataBuff[Lenoffset+5], 2);
            Lenoffset += (nLen+5);
            ACCOMM_FMT_LOGCC("AmrRtDataSave() ״̬�� 0x04000506: \n");
            ACCOMM_BUF_LOGCC((uint8*)&TermSharedData.MetStusPara.Rsw6, 2, "״̬��");
            break;

        case 0x04000505:  //״̬��5
            if(nLen > 2)
            {
                nLen = 2;
            }
            memcpy((uint16*)&TermSharedData.MetStusPara.Rsw5, &DataBuff[Lenoffset+5], 2);
            Lenoffset += (nLen+5);
            ACCOMM_FMT_LOGCC("AmrRtDataSave() ״̬�� 0x04000505: \n");
            ACCOMM_BUF_LOGCC((uint8*)&TermSharedData.MetStusPara.Rsw5, 2, "״̬��");
            break;

        case 0x04000504:  //״̬��4
            if(nLen > 2)
            {
                nLen = 2;
            }
            memcpy((uint16*)&TermSharedData.MetStusPara.Rsw4, &DataBuff[Lenoffset+5], 2);
            Lenoffset += (nLen+5);
            ACCOMM_FMT_LOGCC("AmrRtDataSave() ״̬�� 0x04000504: \n");
            ACCOMM_BUF_LOGCC((uint8*)&TermSharedData.MetStusPara.Rsw4, 2, "״̬��");
            break;

        case 0x04000503:  //״̬��3
            if(nLen > 2)
            {
                nLen = 2;
            }
            memcpy((uint16*)&TermSharedData.MetStusPara.Rsw3, &DataBuff[Lenoffset+5], 2);
            Lenoffset += (nLen+5);
            ACCOMM_FMT_LOGCC("AmrRtDataSave() ״̬�� 0x04000503: \n");
            ACCOMM_BUF_LOGCC((uint8*)&TermSharedData.MetStusPara.Rsw3, 2, "״̬��");
            break;

        case 0x04000502:  //״̬��2
            if(nLen > 2)
            {
                nLen = 2;
            }
            memcpy((uint16*)&TermSharedData.MetStusPara.Rsw2, &DataBuff[Lenoffset+5], 2);
            Lenoffset += (nLen+5);
            ACCOMM_FMT_LOGCC("AmrRtDataSave() ״̬�� 0x04000502: \n");
            ACCOMM_BUF_LOGCC((uint8*)&TermSharedData.MetStusPara.Rsw2, 2, "״̬��");
            break;

        case 0x04000501:  //״̬��1
            if(nLen > 2)
            {
                nLen = 2;
            }
            memcpy((uint16*)&TermSharedData.MetStusPara.Rsw1, &DataBuff[Lenoffset+5], 2);
            Lenoffset += (nLen+5);
            ACCOMM_FMT_LOGCC("AmrRtDataSave() ״̬�� 0x04000501: \n");
            ACCOMM_BUF_LOGCC((uint8*)&TermSharedData.MetStusPara.Rsw1, 2, "״̬��");
            break;

        /* ��� ���� begin */
        case 0x0207FF00:  //��ѹ�������
        case 0xEFB10000:  //��ѹ���
        case 0xEFB20000:  //�������
            if(nLen > 6)
            {
                nLen = 6;
            }
            SaveAngle(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;

        case 0x028800FF:  //��ѹ��� �� �������
            if(nLen > 12)
            {
                nLen = 12;
            }
            SaveAngle(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;

        /* ��� ���� end */

        /* г�� ���� begin */
        case 0x020A01FF:  //A���ѹг�������ʣ��ܼ�2��n�Σ�
        case 0x020A02FF:  //B���ѹг�������ʣ��ܼ�2��n�Σ�
        case 0x020A03FF:  //C���ѹг�������ʣ��ܼ�2��n�Σ�
        case 0x020B01FF:  //A�����г�������ʣ��ܼ�2��n�Σ�
        case 0x020B02FF:  //B�����г�������ʣ��ܼ�2��n�Σ�
        case 0x020B03FF:  //C�����г�������ʣ��ܼ�2��n�Σ�
            if(nLen > (2*31))
            {
                nLen = 2*31;//����ģ�����͵ı�����31��
            }
            SaveHarmonic(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;

        /* г�� ���� end */

        /* г������ ���� begin */
        case 0x020C01FF:  //A���ѹг���������ܼ�2��n�Σ�
        case 0x020C02FF:  //B���ѹг���������ܼ�2��n�Σ�
        case 0x020C03FF:  //C���ѹг���������ܼ�2��n�Σ�
        if(nLen > (2*31))
            {
                nLen = 2*31;//����ģ�����͵ı�����31��
            }
            SaveHarmonic(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;
        case 0x020D01FF:  //A�����г���������ܼ�2��n�Σ�
        case 0x020D02FF:  //B�����г���������ܼ�2��n�Σ�
        case 0x020D03FF:  //C�����г���������ܼ�2��n�Σ�
            if(nLen > (3*31))
            {
                nLen = 3*31;//����ģ�����͵ı�����31��
            }
            SaveHarmonic(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;

        /* г�� ���� end */

        /* ���ɲ��� ���� begin */
        case 0x04000103:             //�����������
            if(nLen > 1)
            {
                nLen = 1;
            }
            SavePara(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;
        case 0x04000104:             //����ʱ��
            if(nLen > 1)
            {
                nLen = 1;
            }
            SavePara(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;
        case 0x04010000:             //��ǰ��ʱ����(���14��ʱ��)
        case 0x04020000:             //������ʱ����(���14��ʱ��)
            if(nLen > 3*14)
            {
                nLen = 3*14;
            }
            SavePara(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;
        case 0x04010001:             //��ǰ����ʱ�α�(���8��, ��1��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04010002:             //��ǰ����ʱ�α�(���8��, ��2��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04010003:             //��ǰ����ʱ�α�(���8��, ��3��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04010004:             //��ǰ����ʱ�α�(���8��, ��4��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04010005:             //��ǰ����ʱ�α�(���8��, ��5��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04010006:             //��ǰ����ʱ�α�(���8��, ��6��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04010007:             //��ǰ����ʱ�α�(���8��, ��7��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04010008:             //��ǰ����ʱ�α�(���8��, ��8��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04020001:             //��������ʱ�α�(���8��, ��1��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04020002:             //��������ʱ�α�(���8��, ��2��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04020003:             //��������ʱ�α�(���8��, ��3��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04020004:             //��������ʱ�α�(���8��, ��4��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04020005:             //��������ʱ�α�(���8��, ��5��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04020006:             //��������ʱ�α�(���8��, ��6��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04020007:             //��������ʱ�α�(���8��, ��7��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        case 0x04020008:             //��������ʱ�α�(���8��, ��8��ʱ��ʱ�α�����, ÿ�����14ʱ��)
            if(nLen > 3*14)
            {
                nLen = 3*14;
            }
            SavePara(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;

        /* ���ɲ��� ���� end */

        /* ����ʶ�� ���� begin */
        case 0xf2000001:             //(�Զ�����չ)����ʶ����(�㽭)
            SaveTopo(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;
        /* ����ʶ�� ���� end */

        default:
            Lenoffset = DataLen;
            break;
        }
    }

    //��ӡ��� ������ͳ��ģ�鹲���ڴ�ṹ��
}


/*******************************************************************************
* ��������:	AmrRtDataSave
* ��������:	����ʵʱ���ݴ洢
* �������:	PrtlID              ��Լ����
*           DataLen             ���ݳ���
*           DataBuff            ��������
* �������:	��
* �� �� ֵ: ��
*******************************************************************************/
void AmrRtDataSaveSingle(uint16 DataLen, uint8 *DataBuff)
{
    uint32	DI07;								 //07���ݱ�ʶ
    uint16	Lenoffset = 0;
    uint8	nLen = 0;

    Lenoffset = 0;

    if(DataLen < 5)//���ٰ���4�ֽ����ݱ�ʶ+1�ֽ�����
    {
        ACCOMM_FMT_DEBUG("AmrRtDataSaveSingle() DataLen=%d����! ���ٰ���4�ֽ����ݱ�ʶ+1�ֽ�����\n", DataLen);
        return;
    }

    DI07 = *(uint32*)(&DataBuff[Lenoffset]);
    nLen = DataLen - 4;//��ȥ4�ֽ����ݱ�ʶ����, ʣ������Ϊ����������
    switch(DI07)
    {
    /* ���ɲ��� ���� begin */
    case 0x04000103:             //�����������
    case 0x04000104:             //����ʱ��
        if(nLen > 1)
        {
            nLen = 1;
        }
        SavePara(nLen, &DataBuff[Lenoffset+4], DI07);
        Lenoffset += (nLen+4);
        break;
    case 0x04000106:			   //������ʱ�����л�ʱ�� ��ʱ������
    case 0x04000107:			   //��������ʱ���л�ʱ�� ��ʱ������
        if(nLen > 5)
        {
            nLen = 5;
        }
        SavePara(nLen, &DataBuff[Lenoffset+4], DI07);
        Lenoffset += (nLen+4);
        break;
    case 0x04000201:             //��ʱ���� ��14
    case 0x04000202:             //��ʱ�α��� ��8
    case 0x04000203:             //��ʱ���� ��14
    case 0x04000204:             //������ ��63
    //case 0x04000205:             //���������� ��254 2�ֽ�
    case 0x04000601:			   //�й���Ϸ�ʽ������
    case 0x04000602:			   //�޹���Ϸ�ʽ1������
    case 0x04000603:			   //�޹���Ϸ�ʽ2������
        if(nLen > 1)
        {
            nLen = 1;
        }
        SavePara(nLen, &DataBuff[Lenoffset+4], DI07);
        Lenoffset += (nLen+4);
        break;
    case 0x04000205:             //���������� ��254
        if(nLen > 2)
        {
            nLen = 2;
        }
        SavePara(nLen, &DataBuff[Lenoffset+4], DI07);
        Lenoffset += (nLen+4);
        break;
    case 0x04010000:             //��ǰ��ʱ����(���14��ʱ��)
    case 0x04020000:             //������ʱ����(���14��ʱ��)
        if(nLen > 3*14)
        {
            nLen = 3*14;
        }
        SavePara(nLen, &DataBuff[Lenoffset+4], DI07);
        Lenoffset += (nLen+4);
        break;
    case 0x04010001:             //��ǰ����ʱ�α�(���8��, ��1��ʱ��ʱ�α�����, ÿ�����14ʱ��)
    case 0x04010002:             //��ǰ����ʱ�α�(���8��, ��2��ʱ��ʱ�α�����, ÿ�����14ʱ��)
    case 0x04010003:             //��ǰ����ʱ�α�(���8��, ��3��ʱ��ʱ�α�����, ÿ�����14ʱ��)
    case 0x04010004:             //��ǰ����ʱ�α�(���8��, ��4��ʱ��ʱ�α�����, ÿ�����14ʱ��)
    case 0x04010005:             //��ǰ����ʱ�α�(���8��, ��5��ʱ��ʱ�α�����, ÿ�����14ʱ��)
    case 0x04010006:             //��ǰ����ʱ�α�(���8��, ��6��ʱ��ʱ�α�����, ÿ�����14ʱ��)
    case 0x04010007:             //��ǰ����ʱ�α�(���8��, ��7��ʱ��ʱ�α�����, ÿ�����14ʱ��)
    case 0x04010008:             //��ǰ����ʱ�α�(���8��, ��8��ʱ��ʱ�α�����, ÿ�����14ʱ��)
    case 0x04020001:             //��������ʱ�α�(���8��, ��1��ʱ��ʱ�α�����, ÿ�����14ʱ��)
    case 0x04020002:             //��������ʱ�α�(���8��, ��2��ʱ��ʱ�α�����, ÿ�����14ʱ��)
    case 0x04020003:             //��������ʱ�α�(���8��, ��3��ʱ��ʱ�α�����, ÿ�����14ʱ��)
    case 0x04020004:             //��������ʱ�α�(���8��, ��4��ʱ��ʱ�α�����, ÿ�����14ʱ��)
    case 0x04020005:             //��������ʱ�α�(���8��, ��5��ʱ��ʱ�α�����, ÿ�����14ʱ��)
    case 0x04020006:             //��������ʱ�α�(���8��, ��6��ʱ��ʱ�α�����, ÿ�����14ʱ��)
    case 0x04020007:             //��������ʱ�α�(���8��, ��7��ʱ��ʱ�α�����, ÿ�����14ʱ��)
    case 0x04020008:             //��������ʱ�α�(���8��, ��8��ʱ��ʱ�α�����, ÿ�����14ʱ��)
        if(nLen > 3*14)
        {
            nLen = 3*14;
        }
        SavePara(nLen, &DataBuff[Lenoffset+4], DI07);
        Lenoffset += (nLen+4);
        break;
    case 0x04000503:             //��ǰ����ģ�����ʱ�α���������һ��
        if(nLen > 2)
        {
            nLen = 2;
        }
        SavePara(nLen, &DataBuff[Lenoffset+4], DI07);
        Lenoffset += (nLen+4);
        break;

    /* ���ɲ��� ���� end */

    /* ����ʶ�� ���� begin */
    case 0x04401004:             //(�Զ�����չ)����ʶ����(�㽭)
        SaveTopo(nLen, &DataBuff[Lenoffset+4], DI07);
        Lenoffset += (nLen+4);
        break;
    /* ����ʶ�� ���� end */

    default:
        Lenoffset = DataLen;
        break;
    }
}


void Pro_AC_Amr_Response(uint8 *pRecvData)
{
    uint16	i;
    uint8	*pDataBuff = NULL;
    uint16	DataLen = 0;

    pDataBuff 	= 	&pRecvData[MTR_DATA_INDEX+1];
    DataLen	= pRecvData[LEN_INDEX] + (pRecvData[LEN_INDEX+1]<<8);

    //�������33����

    for(i = 0; i < DataLen; i++)
    {
        pRecvData[MTR_DATA_INDEX + i + 1] -= 0x33;
    }
    AmrRtDataSave(DataLen, pDataBuff);
}

void Pro_AC_Amr_Response_Single(uint8 *pRecvData)
{
    uint16	i;
    uint8	*pDataBuff = NULL;
    uint16	DataLen = 0;

    pDataBuff 	= 	&pRecvData[MTR_DATA_INDEX];
    DataLen	= pRecvData[LEN_INDEX];

    //�������33����

    for(i = 0; i < DataLen; i++)
    {
        pRecvData[MTR_DATA_INDEX + i] -= 0x33;
    }
    AmrRtDataSaveSingle(DataLen, pDataBuff);
}


/*******************************************************************************
* ��������: UpdateStatistics
* ��������: ����ͳ������
* �������:
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void UpdateStatistics(uint8 type)
{
    ACCOMM_FMT_LOGCC("UpdateStatistics():...... \n");
    float pi = acos(-1.0);//����

#if TEST_VER_FLAG == 1
    {
        if(type == SAMPLE_TYPE_REALTIME)
        {
            if (gSaveFlag.bFirstSampRealtime == FALSE)
            {
                gSaveFlag.bFirstSampRealtime = TRUE;
                gSaveFlag.bFirstSampSpecial = TRUE;
            }
            
            if(1)//��ѹ���ͳ��
            {
                //��A���ѹΪ��׼
                int32 UZeroX = 0;
                int32 UZeroY = 0;
                int32  PhaAVoltAngle = 0.1 * TermSharedData.PhaAVoltAngle;
                int32  PhaBVoltAngle = 0.1 * TermSharedData.PhaBVoltAngle;
                int32  PhaCVoltAngle = 0.1 * TermSharedData.PhaCVoltAngle;
                if(gPriMeasurePara.AngleDir.BitSect.PhaAVolAngFlag == 1)
                {
                    PhaAVoltAngle = 0 - PhaAVoltAngle;
                }
                
                if(gPriMeasurePara.AngleDir.BitSect.PhaBVolAngFlag == 1)
                {
                    PhaBVoltAngle = 0 - PhaBVoltAngle;
                }
                
                if(gPriMeasurePara.AngleDir.BitSect.PhaCVolAngFlag == 1)
                {
                    PhaCVoltAngle = 0 - PhaCVoltAngle;
                }

                //�����ѹ����
                UZeroX = TermSharedData.PhaAVolt + TermSharedData.PhaBVolt*cos((PhaBVoltAngle-PhaAVoltAngle)*pi/180) + TermSharedData.PhaCVolt*cos((PhaCVoltAngle-PhaAVoltAngle)*pi/180);
                UZeroY = 0 + TermSharedData.PhaBVolt*sin((PhaBVoltAngle-PhaAVoltAngle)*pi/180) + TermSharedData.PhaCVolt*sin((PhaCVoltAngle-PhaAVoltAngle)*pi/180);
                TermSharedData.PhaZeroVolt = sqrt(UZeroX*UZeroX + UZeroY*UZeroY);
                /*
                ACCOMM_FMT_LOGCC("PhaAVolt=%d, PhaBVolt=%d, PhaCVolt=%d, PhaAVoltAngle=%d, PhaBVoltAngle=%d, PhaCVoltAngle=%d, UZeroX=%d, UZeroY=%d \n", 
                        TermSharedData.PhaAVolt, TermSharedData.PhaBVolt, TermSharedData.PhaCVolt, 
                        PhaAVoltAngle, PhaBVoltAngle, PhaCVoltAngle, UZeroX, UZeroY);
                */
                ACCOMM_FMT_LOGCC("�����ѹTermSharedData.PhaZeroVolt: %d \n", TermSharedData.PhaZeroVolt);

                //�����ѹ
                UZeroX = TermSharedData.PhaAVolt + TermSharedData.PhaBVolt*cos((PhaBVoltAngle-PhaAVoltAngle-120)*pi/180) + TermSharedData.PhaCVolt*cos((PhaCVoltAngle-PhaAVoltAngle+120)*pi/180);
                UZeroY = 0 + TermSharedData.PhaBVolt*sin((PhaBVoltAngle-PhaAVoltAngle-120)*pi/180) + TermSharedData.PhaCVolt*sin((PhaCVoltAngle-PhaAVoltAngle+120)*pi/180);
                TermSharedData.PhaPosVolt = sqrt(UZeroX*UZeroX + UZeroY*UZeroY);
                /*
                ACCOMM_FMT_LOGCC("PhaAVolt=%d, PhaBVolt=%d, PhaCVolt=%d, PhaAVoltAngle=%d, PhaBVoltAngle=%d, PhaCVoltAngle=%d, UZeroX=%d, UZeroY=%d \n", 
                        TermSharedData.PhaAVolt, TermSharedData.PhaBVolt, TermSharedData.PhaCVolt, 
                        PhaAVoltAngle, PhaBVoltAngle, PhaCVoltAngle, UZeroX, UZeroY);
                */
                ACCOMM_FMT_LOGCC("�����ѹTermSharedData.PhaPosVolt: %d \n", TermSharedData.PhaPosVolt);

                //�����ѹ
                UZeroX = TermSharedData.PhaAVolt + TermSharedData.PhaBVolt*cos((PhaBVoltAngle-PhaAVoltAngle+120)*pi/180) + TermSharedData.PhaCVolt*cos((PhaCVoltAngle-PhaAVoltAngle-120)*pi/180);
                UZeroY = 0 + TermSharedData.PhaBVolt*sin((PhaBVoltAngle-PhaAVoltAngle+120)*pi/180) + TermSharedData.PhaCVolt*sin((PhaCVoltAngle-PhaAVoltAngle-120)*pi/180);
                TermSharedData.PhaNegVolt = sqrt(UZeroX*UZeroX + UZeroY*UZeroY);
                /*
                ACCOMM_FMT_LOGCC("PhaAVolt=%d, PhaBVolt=%d, PhaCVolt=%d, PhaAVoltAngle=%d, PhaBVoltAngle=%d, PhaCVoltAngle=%d, UZeroX=%d, UZeroY=%d \n", 
                        TermSharedData.PhaAVolt, TermSharedData.PhaBVolt, TermSharedData.PhaCVolt, 
                        PhaAVoltAngle, PhaBVoltAngle, PhaCVoltAngle, UZeroX, UZeroY);
                */
                ACCOMM_FMT_LOGCC("�����ѹTermSharedData.PhaNegVolt: %d \n", TermSharedData.PhaNegVolt);
                
                //��ѹ��ƽ����
                if(TermSharedData.PhaPosVolt != 0)
                {
                    TermSharedData.VoltUnbalanceRate = 100 * 100 * TermSharedData.PhaNegVolt/(TermSharedData.PhaPosVolt + 0.0);
                }
                ACCOMM_FMT_LOGCC("��ѹ��ƽ����TermSharedData.VoltUnbalanceRate: %d \n", TermSharedData.VoltUnbalanceRate);
                
            }
            
            if(1)//�������ͳ��
            {
                //��A�����Ϊ��׼
                int32 IZeroX = 0;
                int32 IZeroY = 0;
                int32  PhaACurrAngle = 0.1 * TermSharedData.PhaACurrAngle;
                int32  PhaBCurrAngle = 0.1 * TermSharedData.PhaBCurrAngle;
                int32  PhaCCurrAngle = 0.1 * TermSharedData.PhaCCurrAngle;
                if(gPriMeasurePara.AngleDir.BitSect.PhaACurrAngFlag == 1)
                {
                    PhaACurrAngle = 0 - PhaACurrAngle;
                }
                
                if(gPriMeasurePara.AngleDir.BitSect.PhaBCurrAngFlag == 1)
                {
                    PhaBCurrAngle = 0 - PhaBCurrAngle;
                }
                
                if(gPriMeasurePara.AngleDir.BitSect.PhaCCurrAngFlag == 1)
                {
                    PhaCCurrAngle = 0 - PhaCCurrAngle;
                }

                //�����������
                /*
                //��������Ѵӽ���ֱ�ӻ�ȡ, ������
                IZeroX = TermSharedData.PhaACurr + TermSharedData.PhaBCurr*cos((PhaBCurrAngle-PhaACurrAngle)*pi/180) + TermSharedData.PhaCCurr*cos((PhaCCurrAngle-PhaACurrAngle)*pi/180);
                IZeroY = 0 + TermSharedData.PhaBCurr*sin((PhaBCurrAngle-PhaACurrAngle)*pi/180) + TermSharedData.PhaCCurr*sin((PhaCCurrAngle-PhaACurrAngle)*pi/180);
                TermSharedData.PhaMCurr = sqrt(IZeroX*IZeroX + IZeroY*IZeroY);
                ACCOMM_FMT_LOGCC("�������TermSharedData.PhaZeroCurr: %d \n", TermSharedData.PhaMCurr);
                */
                
                //�������
                IZeroX = TermSharedData.PhaACurr + TermSharedData.PhaBCurr*cos((PhaBCurrAngle-PhaACurrAngle-120)*pi/180) + TermSharedData.PhaCCurr*cos((PhaCCurrAngle-PhaACurrAngle+120)*pi/180);
                IZeroY = 0 + TermSharedData.PhaBCurr*sin((PhaBCurrAngle-PhaACurrAngle-120)*pi/180) + TermSharedData.PhaCCurr*sin((PhaCCurrAngle-PhaACurrAngle+120)*pi/180);
                TermSharedData.PhaPosCurr = sqrt(IZeroX*IZeroX + IZeroY*IZeroY);
                ACCOMM_FMT_LOGCC("�������TermSharedData.PhaPosCurr: %d \n", TermSharedData.PhaPosCurr);

                //�������
                IZeroX = TermSharedData.PhaACurr + TermSharedData.PhaBCurr*cos((PhaBCurrAngle-PhaACurrAngle+120)*pi/180) + TermSharedData.PhaCCurr*cos((PhaCCurrAngle-PhaACurrAngle-120)*pi/180);
                IZeroY = 0 + TermSharedData.PhaBCurr*sin((PhaBCurrAngle-PhaACurrAngle+120)*pi/180) + TermSharedData.PhaCCurr*sin((PhaCCurrAngle-PhaACurrAngle-120)*pi/180);
                TermSharedData.PhaNegCurr = sqrt(IZeroX*IZeroX + IZeroY*IZeroY);
                ACCOMM_FMT_LOGCC("�������TermSharedData.PhaNegCurr: %d \n", TermSharedData.PhaNegCurr);
                
                //������ƽ����
                if(TermSharedData.PhaPosCurr != 0)
                {
                    TermSharedData.CurrUnbalanceRate = 100 * 100 * TermSharedData.PhaNegCurr/(TermSharedData.PhaPosCurr + 0.0);
                }
                ACCOMM_FMT_LOGCC("������ƽ����TermSharedData.CurrUnbalanceRate: %d \n", TermSharedData.CurrUnbalanceRate);
                
            }
            
        }
        else if(type == SAMPLE_TYPE_DEMAND)
        {
            if (gSaveFlag.bFirstSampDemand == FALSE)
            {
                gSaveFlag.bFirstSampDemand = TRUE;
            }
        }
        else if(type == SAMPLE_TYPE_ENERGY)
        {
            if (gSaveFlag.bFirstSampEnergy == FALSE)
            {
                gSaveFlag.bFirstSampEnergy = TRUE;
            }
        }
        else if(type == SAMPLE_TYPE_HARMONIC)
        {
            if (gSaveFlag.bFirstSampHarmonic == FALSE)
            {
                gSaveFlag.bFirstSampHarmonic = TRUE;
            }
        }
    }
#else
    {
        if(type == SAMPLE_TYPE_REALTIME)
        {
            //�����ѹ����
            uint16 UZeroX = 0;
            uint16 UZeroY = 0;
            int32  PhaBVoltAngle = 0.1 * TermSharedData.PhaBVoltAngle;
            int32  PhaCVoltAngle = 0.1 * TermSharedData.PhaCVoltAngle;
            if(gPriMeasurePara.AngleDir.BitSect.PhaBVolAngFlag == 1)
            {
                PhaBVoltAngle = 0 - PhaBVoltAngle;
            }
            
            if(gPriMeasurePara.AngleDir.BitSect.PhaCVolAngFlag == 1)
            {
                PhaCVoltAngle = 0 - PhaCVoltAngle;
            }
            
            UZeroX = TermSharedData.PhaAVolt + TermSharedData.PhaBVolt*cos(PhaBVoltAngle*pi/180) + TermSharedData.PhaCVolt*cos(PhaCVoltAngle*pi/180);
            UZeroY = 0 + TermSharedData.PhaBVolt*sin(PhaBVoltAngle*pi/180) + TermSharedData.PhaCVolt*sin(PhaCVoltAngle*pi/180);
            TermSharedData.PhaZeroVolt = sqrt(UZeroX*UZeroX + UZeroY*UZeroY);
            ACCOMM_FMT_LOGCC("TermSharedData.PhaZeroVolt: %d \n", TermSharedData.PhaZeroVolt);
            
            //��ѹ��ƽ����
            uint16 MinVolt = TermSharedData.PhaAVolt;
            uint16 MaxVolt = TermSharedData.PhaAVolt;
            MinVolt = (TermSharedData.PhaBVolt < MinVolt) ? TermSharedData.PhaBVolt : MinVolt;
            MinVolt = (TermSharedData.PhaCVolt < MinVolt) ? TermSharedData.PhaCVolt : MinVolt;
            MaxVolt = (TermSharedData.PhaBVolt > MaxVolt) ? TermSharedData.PhaBVolt : MaxVolt;
            MaxVolt = (TermSharedData.PhaCVolt > MaxVolt) ? TermSharedData.PhaCVolt : MaxVolt;
            if(MaxVolt > 0)
            {
                TermSharedData.VoltUnbalanceRate = 100 * MinVolt/(MaxVolt + 0.0);
                ACCOMM_FMT_LOGCC("TermSharedData.VoltUnbalanceRate: %d \n", TermSharedData.VoltUnbalanceRate);
            }
            

            //������ƽ����
            uint16 MinCurr = TermSharedData.PhaACurr;
            uint16 MaxCurr = TermSharedData.PhaACurr;
            MinCurr = (TermSharedData.PhaBCurr < MinCurr) ? TermSharedData.PhaBCurr : MinCurr;
            MinCurr = (TermSharedData.PhaCCurr < MinCurr) ? TermSharedData.PhaCCurr : MinCurr;
            MaxCurr = (TermSharedData.PhaBCurr > MaxCurr) ? TermSharedData.PhaBCurr : MaxCurr;
            MaxCurr = (TermSharedData.PhaCCurr > MaxCurr) ? TermSharedData.PhaCCurr : MaxCurr;
            if(MaxCurr > 0)
            {
                TermSharedData.CurrUnbalanceRate = 100 * MinCurr/(MaxCurr + 0.0);
                ACCOMM_FMT_LOGCC("TermSharedData.CurrUnbalanceRate: %d \n", TermSharedData.CurrUnbalanceRate);
            }
            
        }
        else if(type == SAMPLE_TYPE_DEMAND)
        {
            
        }
        else if(type == SAMPLE_TYPE_ENERGY)
        {
            
        }
        else if(type == SAMPLE_TYPE_HARMONIC)
        {
            
        }
    }
#endif
}

/*********************************************************************
    �������ܵ�Ԫ��
    ����������
    ���������isUpdateAC �Ƿ���Ҫ���� isGetVer�Ƿ���Ҫ�а汾��
    ���������
        ��
    ��������ֵ��
                   > 0: �������ݵĳ���		, < 0: ������Ϣ
    �ڲ��������̣�
********************************************************************/
void ReadACSample(BOOL isUpdateAC, BOOL isGetVer, uint8 inType)
{
    RS232_COMM_PARA commPara;
    int32 recvLen = 0;
    int32 sendLen = 0;
    int32 bRet = 0;
    uint8  recvBuf[2000];
    uint8  sendBuf[1000];
    uint8  recvBufTemp[2000];
    static READ_AC_STATE state;
    uint8 *ptr = NULL;  //Ӧ������֡��ʼָ��(0x68)
    uint8 *pstart = NULL;  //Ӧ������֡��ʼָ��(�׵�ַ,���ܲ���0x68,���ڶ�֡��֡)
    int32  validlen = 0;  //һ֡�������ĳ���
    int32  leftlen = 0;  //����ʣ�೤��
    static AC_BAUDRATE eBaudRate = BAUDRATE115200;  //Ĭ�ϲ�����
    uint8  nRecvNullMaxCnt = 10;  //�����������Ϊ�յĴ���
    static uint8  nRecvNullCnt = 0;  //��������Ϊ�յĴ���
    BOOL   bAcDateAndTimeSet = FALSE;
    BOOL   bAcDateSet = FALSE;
    BOOL   bAcTimeSet = FALSE;
    BOOL   bDataInit = FALSE;
    BOOL   bParamInit = FALSE;
    BOOL   bDemandInit = FALSE;
    BOOL   bRelay485 = FALSE;
    BOOL   bRelayUDP = FALSE;
    BOOL   bDemandPeriodSet = FALSE;//���������������
    BOOL   bSlipTimeSet = FALSE;//����ʱ������
    BOOL   bZoneNumSet = FALSE;//ʱ��������
    BOOL   bPeriodNumSet = FALSE;//��ʱ��������
    BOOL   bZoneTableSet = FALSE;//ʱ��������
    BOOL   bDayPeriodSet = FALSE;//��ʱ�α�����
    BOOL   bParaSet = FALSE;//��������
    BOOL   bParaQuery = FALSE;//������ѯ
    uint8  pipe = 0;  //485����ͨ��
    int    initRet = 0;
    int    i = 0;
    int    CheckRetSum = 0;

    //��ֹ�����ڲɼ�����ʱ��UpdateStatistics����������ˢ�³�ȫ0
    uint8  type = SAMPLE_TYPE_NULL;

    if (isUpdateAC)
    {
        bRet = JcUpdateDeal(fd_ac, FALSE);
        if (bRet == -2)
        {
            state = INIT_AC_STATE;
        }
        else
        {
            return;
        }
    }

    if (isGetVer)
    {
        bRet = JcUpdateDeal(fd_ac, TRUE);
        if (bRet == -2)
        {
            state = INIT_AC_STATE;
        }
        else if(bRet == -3)
        {
            //�Ѿ��ﵽ���Դ�������, �����ٻ��汾
        }
        else
        {
            return;
        }
    }

    memset(recvBuf, 0x00, sizeof(recvBuf));
    memset(sendBuf, 0x00, sizeof(sendBuf));

    switch (state)
    {
    case INIT_AC_STATE:
        memset((void *)&commPara, 0, sizeof(RS232_COMM_PARA));

        //9600������
        if(eBaudRate == BAUDRATE9600)
        {
            commPara.dwBaudRate = 9600;
            commPara.byByteSize = 8;
            commPara.byParity = 'e';
            commPara.byStopBits = 1;
        }
        else if(eBaudRate == BAUDRATE115200)
        {
            //115200������
            commPara.dwBaudRate = 115200;
            commPara.byByteSize = 8;
            commPara.byParity = 'e';
            commPara.byStopBits = 1;
        }
        else
        {
            ACCOMM_FMT_TRACE("ReadACSample() INIT_AC_STATE...���������ʹ���! \n");
            sleep(2);
            break;
        }

        sprintf((char *)commPara.sDeviceName, gUartConf.dev);

        if (fd_ac < 0)
        {
            ACCOMM_FMT_TRACE("if (fd_ac < 0)...111...JCcommPara.sDeviceName=%s, dwBaudRate=%d, byByteSize:%d, byParity:%c, byStopBits:%d \n", commPara.sDeviceName, 
                commPara.dwBaudRate, commPara.byByteSize, commPara.byParity, commPara.byStopBits);
            fd_ac = commdrv_Init_232(&commPara);   /*�򿪴���*/
        }
        if (fd_ac < 0)
        {
            ACCOMM_FMT_TRACE("if (fd_ac < 0)...222...JCcommPara.sDeviceName=%s, dwBaudRate=%d, byByteSize:%d, byParity:%c, byStopBits:%d \n", commPara.sDeviceName, 
                commPara.dwBaudRate, commPara.byByteSize, commPara.byParity, commPara.byStopBits);
            state = INIT_AC_STATE;
            sleep(2);
            break;
        }
        ACCOMM_FMT_TRACE("JCcommPara.sDeviceName = %s, fd_ac = %d\n", commPara.sDeviceName, fd_ac);

        //�������л�֮���ٻ�һ�°汾��Ϣ����ֹǰ���ٻ�ʧ��
        if(gBaudRateChangedFlag)
        {
            gBaudRateChangedFlag = 0;
            gCallVerAfterBaudRateChanged = 1;
            ACCOMM_FMT_TRACE("ReadACSample() INIT_AC_STATE...�������л�֮���ٻ�һ�°汾��Ϣ����ֹǰ���ٻ�ʧ�� \n");
        }
        
        state = READ_AC_SEND_STATE;
        break;

        // ���ͳ������ɵ�֡
    case READ_AC_SEND_STATE:
        //ACCOMM_FMT_TRACE("ReadACSample() READ_AC_SEND_STATE...type=%d\n", type);
        //���建��
        sendLen = 0;
        if ((1 == g_nAcSetDateFlag) && 
            (1 == g_nAcSetTimeFlag) &&
            (!TermSharedData.DateSetFlag) &&
            (!TermSharedData.TimeSetFlag))
        {
            //ͬʱ�Խ���ģ�����ں�ʱ��
            g_nAcSetDateFlag = 0;
            g_nAcSetTimeFlag = 0;
            bAcDateAndTimeSet = TRUE;

            sendLen = make_frame_ac_date_time(sendBuf);

            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "�������ں�ʱ��");
        }
        else if (1 == g_nAcSetDateFlag)//�жϽ���ģ���Ƿ���ҪУʱ
        {
            //����ģ��Уʱ(����)
            g_nAcSetDateFlag = 0;
            bAcDateSet = TRUE;
            if(TRUE == TermSharedData.DateSetFlag)
            {
                sendLen = make_frame_master_date(sendBuf);
                TermSharedData.DateSetFlag = FALSE;
            }
            else
            {
                sendLen = make_frame_ac_date(sendBuf);
            }
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "��������");
        }
        else if (1 == g_nAcSetTimeFlag)//�жϽ���ģ���Ƿ���ҪУʱ
        {
            //����ģ��Уʱ(ʱ��)
            g_nAcSetTimeFlag = 0;
            bAcTimeSet = TRUE;
            if(TRUE == TermSharedData.TimeSetFlag)
            {
                sendLen = make_frame_master_time(sendBuf);
                TermSharedData.TimeSetFlag = FALSE;
            }
            else
            {
                sendLen = make_frame_ac_time(sendBuf);
            }
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "����ʱ��");
        }
        else if(1 == g_nAcDataInitFlag)//�ж��Ƿ���Ҫ������ʼ�������ݳ�ʼ��
        {
            //���ݳ�ʼ��
            g_nAcDataInitFlag = 0;
            bDataInit = TRUE;
            sendLen = make_frame_init_data(sendBuf);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "���ݳ�ʼ��");
        }
        else if(1 == g_nAcParamInitFlag)
        {
            //������ʼ��
            g_nAcParamInitFlag = 0;
            bParamInit = TRUE;
            sendLen = make_frame_init_param(sendBuf);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "������ʼ��");
        }
        else if(1 == g_nAcDemandInitFlag)
        {
            //������ʼ��
            g_nAcDemandInitFlag = 0;
            bDemandInit = TRUE;
            sendLen = make_frame_init_demand(sendBuf);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "������ʼ��");
        }
        else if(TermSharedData.MaxDemandPeriodSetFlag == TRUE)
        {
            //���������������
            TermSharedData.MaxDemandPeriodSetFlag = FALSE;
            bDemandPeriodSet = TRUE;
            sendLen = make_frame_set_demand_period(sendBuf);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "���������������");
            g_nAcReadDemandPeriodFlag = 1;//����֮����Ҫ��ѯһ��
        }
        else if(1 == g_nAcReadDemandPeriodFlag)
        {
            //��ѯ�����������
            g_nAcReadDemandPeriodFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000103);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "��ѯ�����������");
        }
        else if(TermSharedData.SlipDiffTimeSetFlag == TRUE)
        {
            //���û���ʱ��
            TermSharedData.SlipDiffTimeSetFlag = FALSE;
            bSlipTimeSet = TRUE;
            sendLen = make_frame_set_slip_time(sendBuf);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "���û���ʱ��");
            g_nAcReadSlipTimeFlag = 1;//����֮����Ҫ��ѯһ��
        }
        else if(1 == g_nAcReadSlipTimeFlag)
        {
            //��ѯ����ʱ��
            g_nAcReadSlipTimeFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000104);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "��ѯ����ʱ��");
        }
        else if(TermSharedData.AddiZoneChangeTimeSetFlag == TRUE)
        {
            //���ñ�����ʱ�����л�ʱ��
            TermSharedData.AddiZoneChangeTimeSetFlag = FALSE;
            bParaSet = TRUE;
            sendLen = make_frame_zone_table_change_time(sendBuf);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "���ñ�����ʱ�����л�ʱ��");
            g_nAddiZoneChangeTimeFlag = 1;//����֮����Ҫ��ѯһ��
        }
        else if(1 == g_nAddiZoneChangeTimeFlag)
        {
            //��ѯ������ʱ�����л�ʱ��
            g_nAddiZoneChangeTimeFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000106);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "��ѯ������ʱ�����л�ʱ��");
        }
        else if(TermSharedData.AddiPeriChangeTimeSetFlag == TRUE)
        {
            //���ñ�������ʱ�α��л�ʱ��
            TermSharedData.AddiPeriChangeTimeSetFlag = FALSE;
            bParaSet = TRUE;
            sendLen = make_frame_peri_table_change_time(sendBuf);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "���ñ�������ʱ�α��л�ʱ��");
            g_nAddiPeriChangeTimeFlag = 1;//����֮����Ҫ��ѯһ��
        }
        else if(1 == g_nAddiPeriChangeTimeFlag)
        {
            //��ѯ��������ʱ�α��л�ʱ��
            g_nAddiPeriChangeTimeFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000107);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "��ѯ��������ʱ�α��л�ʱ��");
        }
        else if(TermSharedData.ActPowCombWordSetFlag == TRUE)
        {
            //�����й���Ϸ�ʽ������
            TermSharedData.ActPowCombWordSetFlag = FALSE;
            bParaSet = TRUE;
            sendLen = make_frame_set_p_q_word(sendBuf, 0x04000601, TermSharedData.ActPowCombWordSet.nValue);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "�����й���Ϸ�ʽ������=%d", TermSharedData.ActPowCombWordSet.nValue);
            g_nActPowCombWordFlag = 1;//����֮����Ҫ��ѯһ��
        }
        else if(1 == g_nActPowCombWordFlag)
        {
            //��ѯ�й���Ϸ�ʽ������
            g_nActPowCombWordFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000601);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "��ѯ�й���Ϸ�ʽ������");
        }
        else if(TermSharedData.ReaPowComb1WordSetFlag == TRUE)
        {
            //�����޹���Ϸ�ʽ1������
            TermSharedData.ReaPowComb1WordSetFlag = FALSE;
            bParaSet = TRUE;
            sendLen = make_frame_set_p_q_word(sendBuf, 0x04000602, TermSharedData.ReaPowComb1WordSet.nValue);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "�����޹���Ϸ�ʽ1������=%d", TermSharedData.ReaPowComb1WordSet.nValue);
            g_nReaPowComb1WordFlag = 1;//����֮����Ҫ��ѯһ��
        }
        else if(1 == g_nReaPowComb1WordFlag)
        {
            //��ѯ�޹���Ϸ�ʽ1������
            g_nReaPowComb1WordFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000602);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "��ѯ�޹���Ϸ�ʽ1������");
        }
        else if(TermSharedData.ReaPowComb2WordSetFlag == TRUE)
        {
            //�����޹���Ϸ�ʽ2������
            TermSharedData.ReaPowComb2WordSetFlag = FALSE;
            bParaSet = TRUE;
            sendLen = make_frame_set_p_q_word(sendBuf, 0x04000603, TermSharedData.ReaPowComb2WordSet.nValue);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "�����޹���Ϸ�ʽ2������=%d", TermSharedData.ReaPowComb2WordSet.nValue);
            g_nReaPowComb2WordFlag = 1;//����֮����Ҫ��ѯһ��
        }
        else if(1 == g_nReaPowComb2WordFlag)
        {
            //��ѯ�޹���Ϸ�ʽ2������
            g_nReaPowComb2WordFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000603);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "��ѯ�޹���Ϸ�ʽ2������");
        }
        else if(TermSharedData.ZoneTableNumSetFlag == TRUE)
        {
            //����ʱ������
            TermSharedData.ZoneTableNumSetFlag = FALSE;
            bZoneNumSet = TRUE;
            sendLen = make_frame_set_zone_peri_num(sendBuf, 0x04000201, TermSharedData.ZonePeriNumSet.yPeriod);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "����ʱ������=%d", TermSharedData.ZonePeriNumSet.yPeriod);
            g_nAcReadZoneNumFlag = 1;//����֮����Ҫ��ѯһ��
        }
        else if(1 == g_nAcReadZoneNumFlag)
        {
            //��ѯʱ������
            g_nAcReadZoneNumFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000201);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "��ѯʱ������");
        }
        else if(TermSharedData.DayPeriTableNumSetFlag == TRUE)
        {
            //������ʱ�α�����
            TermSharedData.DayPeriTableNumSetFlag = FALSE;
            bPeriodNumSet = TRUE;
            sendLen = make_frame_set_zone_peri_num(sendBuf, 0x04000202, TermSharedData.ZonePeriNumSet.dPeriodMet);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "������ʱ�α�����=%d", TermSharedData.ZonePeriNumSet.dPeriodMet);
            g_nAcReadDayTableNumFlag = 1;//����֮����Ҫ��ѯһ��
        }
        else if(1 == g_nAcReadDayTableNumFlag)
        {
            //��ѯ��ʱ�α�����
            g_nAcReadDayTableNumFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000202);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "��ѯ��ʱ�α�����");
        }
        else if(TermSharedData.DayPeriNumSetFlag == TRUE)
        {
            //����ʱ������
            TermSharedData.DayPeriNumSetFlag = FALSE;
            bPeriodNumSet = TRUE;
            sendLen = make_frame_set_zone_peri_num(sendBuf, 0x04000203, TermSharedData.ZonePeriNumSet.dPeriod);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "������ʱ������=%d", TermSharedData.ZonePeriNumSet.dPeriod);
            g_nAcReadDayPeriNumFlag = 1;//����֮����Ҫ��ѯһ��
        }
        else if(1 == g_nAcReadDayPeriNumFlag)
        {
            //��ѯʱ������
            g_nAcReadDayPeriNumFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000203);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "��ѯ��ʱ������");
        }
        else if(TermSharedData.CostRateNumSetFlag == TRUE)
        {
            //���÷�������
            TermSharedData.CostRateNumSetFlag = FALSE;
            bParaSet = TRUE;
            sendLen = make_frame_set_zone_peri_num(sendBuf, 0x04000204, TermSharedData.ZonePeriNumSet.rate);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "���÷�������=%d", TermSharedData.ZonePeriNumSet.rate);
            g_nAcReadCostRateNumFlag = 1;//����֮����Ҫ��ѯһ��
        }
        else if(1 == g_nAcReadCostRateNumFlag)
        {
            //��ѯ��������
            g_nAcReadCostRateNumFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000204);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "��ѯ��������");
        }
        else if(TermSharedData.HolidayNumSetFlag == TRUE)
        {
            //���ù�����������
            TermSharedData.HolidayNumSetFlag = FALSE;
            bParaSet = TRUE;
            sendLen = make_frame_set_holiday_num(sendBuf, 0x04000205, TermSharedData.ZonePeriNumSet.pubHoliday);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "���ù�����������=%d", TermSharedData.ZonePeriNumSet.pubHoliday);
            g_nAcReadHolidayNumFlag = 1;//����֮����Ҫ��ѯһ��
        }
        else if(1 == g_nAcReadHolidayNumFlag)
        {
            //��ѯ������������
            g_nAcReadHolidayNumFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000205);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "��ѯ������������");
        }
        else if(TermSharedData.CurrZoneTableSetFlag == TRUE)
        {
            //���õ�ǰ��ʱ����
            TermSharedData.CurrZoneTableSetFlag = FALSE;
            bZoneTableSet = TRUE;
            sendLen = make_frame_set_curr_zone(sendBuf);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "���õ�ǰ��ʱ����");
            g_nAcReadCurrZoneFlag = 1;//����֮����Ҫ��ѯһ��
        }
        else if(TermSharedData.AddiZoneTableSetFlag == TRUE)
        {
            //���ñ�����ʱ����
            TermSharedData.AddiZoneTableSetFlag = FALSE;
            bZoneTableSet = TRUE;
            sendLen = make_frame_set_addi_zone(sendBuf);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "���ñ�����ʱ����");
            g_nAcReadAddiZoneFlag = 1;//����֮����Ҫ��ѯһ��
        }
        else if(1 == g_nAcReadCurrZoneFlag)
        {
            //��ѯ��ǰ��ʱ����
            g_nAcReadCurrZoneFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04010000);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "��ѯ��ǰ��ʱ����");
        }
        else if(1 == g_nAcReadAddiZoneFlag)
        {
            //��ѯ������ʱ����
            g_nAcReadAddiZoneFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04020000);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "��ѯ������ʱ����");
        }
        else if(1 == g_nReadCurrDayPeriLabelFlag)
        {
            //��ѯ����ģ�鵱ǰ����ʱ�α�����һ��
            g_nReadCurrDayPeriLabelFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000503);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "��ѯ����ģ�鵱ǰ����ʱ�α�����һ��");
        }
        else if(TermSharedData.CurrDayPeriTableSetFlag == TRUE)
        {
            BOOL bNeedSet = FALSE;
            uint8 nNeedSetIndex = 0;
            for(i=0; i<TermSharedData.CurrDayPeriTableSet.nNum; i++)
            {
                if(i == OOP_MAX_DAYPERIODS)
                    break;
                if(1 == TermSharedData.CurrDayPeriSetFlagArr[i])
                {
                    TermSharedData.CurrDayPeriSetFlagArr[i] = 0;
                    nNeedSetIndex = i;
                    bNeedSet = TRUE;
                    break;
                }
            }
            if(bNeedSet)
            {
                //���ý���ģ��ĵ�һ����ʱ�α�
                bZoneTableSet = TRUE;
                sendLen = make_frame_set_curr_day_period(sendBuf, nNeedSetIndex);
                ACCOMM_BUF_DEBUG(sendBuf, sendLen, "���õ�ǰ����ʱ�α�nNeedSetIndex=%d", nNeedSetIndex);
                g_nAcReadCurrDayPeriFlag = 1;//����֮����Ҫ��ѯһ��
                g_nAcReadCurrDayFlagArr[nNeedSetIndex] = 1;
            }
            else
            {
                //����Ҫ���õ�ǰ����ʱ�α�
                TermSharedData.CurrDayPeriTableSetFlag = FALSE;
                state = READ_AC_SEND_STATE;        // ���ط���
                ACCOMM_FMT_TRACE("ReadACSample() READ_AC_SEND_STATE...CurrZoneTableSetFlag = FALSE\n");
                break;
            }
            
        }
        else if(TermSharedData.AddiDayPeriTableSetFlag == TRUE)
        {
            BOOL bNeedSet = FALSE;
            uint8 nNeedSetIndex = 0;
            for(i=0; i<TermSharedData.AddiDayPeriTableSet.nNum; i++)
            {
                if(i == OOP_MAX_DAYPERIODS)
                    break;
                if(1 == TermSharedData.AddiDayPeriSetFlagArr[i])
                {
                    TermSharedData.AddiDayPeriSetFlagArr[i] = 0;
                    nNeedSetIndex = i;
                    bNeedSet = TRUE;
                    break;
                }
            }
            if(bNeedSet)
            {
                //���ý���ģ��ĵڶ�����ʱ�α�
                bZoneTableSet = TRUE;
                sendLen = make_frame_set_addi_day_period(sendBuf, nNeedSetIndex);
                ACCOMM_BUF_DEBUG(sendBuf, sendLen, "���ñ�������ʱ�α�nNeedSetIndex=%d", nNeedSetIndex);
                g_nAcReadAddiDayPeriFlag = 1;//����֮����Ҫ��ѯһ��
                g_nAcReadAddiDayFlagArr[nNeedSetIndex] = 1;
            }
            else
            {
                //����Ҫ���ñ�������ʱ�α�
                TermSharedData.AddiDayPeriTableSetFlag = FALSE;
                state = READ_AC_SEND_STATE;        // ���ط���
                ACCOMM_FMT_TRACE("ReadACSample() READ_AC_SEND_STATE...AddiZoneTableSetFlag = FALSE\n");
                break;
            }
            
        }
        else if(1 == g_nAcReadCurrDayPeriFlag)
        {
            //��ѯ����ģ��ĵ�һ����ʱ�α�
            BOOL bNeedQuery = FALSE;
            for(i=0; i<TermSharedData.CurrDayPeriTableRD.nNum; i++)
            {
                if(1 == g_nAcReadCurrDayFlagArr[i])
                {
                    g_nAcReadCurrDayFlagArr[i] = 0;
                    sendLen = make_frame_query_para(sendBuf, 0x04010001 + i);
                    bNeedQuery = TRUE;
                    bParaQuery = TRUE;
                    ACCOMM_BUF_DEBUG(sendBuf, sendLen, "��ѯ��ǰ����ʱ�α� i=%d", i);
                    break;
                }
            }
            if(!bNeedQuery)
            {
                g_nAcReadCurrDayPeriFlag = 0;
                state = READ_AC_SEND_STATE;        // ���ط���
                ACCOMM_FMT_TRACE("ReadACSample() READ_AC_SEND_STATE...g_nAcReadCurrDayPeriFlag = FALSE\n");
                break;
            }
            
        }
        else if(1 == g_nAcReadAddiDayPeriFlag)
        {
            //��ѯ����ģ��ĵڶ�����ʱ�α�
            BOOL bNeedQuery = FALSE;
            for(i=0; i<TermSharedData.AddiDayPeriTableRD.nNum; i++)
            {
                if(1 == g_nAcReadAddiDayFlagArr[i])
                {
                    g_nAcReadAddiDayFlagArr[i] = 0;
                    sendLen = make_frame_query_para(sendBuf, 0x04020001 + i);
                    bNeedQuery = TRUE;
                    bParaQuery = TRUE;
                    ACCOMM_BUF_DEBUG(sendBuf, sendLen, "��ѯ��������ʱ�α� i=%d", i);
                    break;
                }
            }
            if(!bNeedQuery)
            {
                g_nAcReadAddiDayPeriFlag = 0;
                state = READ_AC_SEND_STATE;        // ���ط���
                ACCOMM_FMT_TRACE("ReadACSample() READ_AC_SEND_STATE...g_nAcReadAddiDayPeriFlag = FALSE\n");
                break;
            }
            
        }
        else
        {
            //�ж��Ƿ���ȷ��֡��Ҫ�·�
            sendLen = PlcReadQueue(pConfirmDown, sendBuf);
            if(sendLen > 0)
            {
                ACCOMM_BUF_DEBUG(sendBuf, sendLen, "���ȴ���ȷ��Ӧ��֡");
            }
            else
            {
                //�����ж�UDP�Ƿ�������
                sendLen = PlcReadQueue(pUdpRecvDown, sendBuf);
                if(sendLen > 0)
                {
                    bRelayUDP = TRUE;//���ȴ���UDP����
                    ACCOMM_BUF_DEBUG(sendBuf, sendLen, "����UDPת������");
                }
                else
                {
                    sendLen = PlcReadQueue(pDealRecvDown[pipe], sendBuf);
                    if (sendLen > 0)
                    {
                        bRelay485 = TRUE;//���ȴ���485���ڱ���
                        ACCOMM_BUF_DEBUG(sendBuf, sendLen, "���ȴ���485ת������");
                    }
                    else
                    {
                        //��ֹ�����ڲɼ�����ʱ��UpdateStatistics����������ˢ�³�ȫ0
                        type = inType;
                        
                        if(type == SAMPLE_TYPE_ENERGY)
                        {
                            sendLen = MakeFrameReadEnergy(sendBuf);
                            ACCOMM_FMT_LOGCC("ReadACSample() READ_AC_SEND_STATE...�ɼ���������\n");
                        }
                        else if(type == SAMPLE_TYPE_DEMAND)
                        {
                            sendLen = MakeFrameReadDemand(sendBuf);
                            ACCOMM_FMT_LOGCC("ReadACSample() READ_AC_SEND_STATE...�ɼ���������\n");
                        }
                        else if(type == SAMPLE_TYPE_REALTIME)
                        {
                            sendLen = MakeFrameReadRealtime(sendBuf);
                            ACCOMM_FMT_LOGCC("ReadACSample() READ_AC_SEND_STATE...�ɼ�ʵʱ����\n");
                        }
                        else if(type == SAMPLE_TYPE_HARMONIC)
                        {
                            sendLen = MakeFrameReadHarmonic(sendBuf);
                            ACCOMM_FMT_LOGCC("ReadACSample() READ_AC_SEND_STATE...�ɼ�г������\n");
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }
        }
        
        if (sendBuf > 0)
        {
            commdrv_Send_232(fd_ac, sendBuf, sendLen);
        }
        state = READ_AC_RECV_STATE;      // ״̬��break����˳��ִ��

        ACCOMM_BUF_LOGCC(sendBuf, sendLen, " ���ͱ���: ");//Ĭ�ϲ���ӡ

        if(bDataInit == TRUE)//���ݳ�ʼ��
        {
            sleep(1);
        }
        else if(bParamInit == TRUE)//������ʼ��
        {
            sleep(1);
        }
        else if(bDemandInit == TRUE)//������ʼ��
        {
            sleep(1);
        }
        else if(bRelay485 == TRUE)//͸��485����
        {
            usleep(500000);
        }
        else if(bParaQuery == TRUE)//������ѯ
        {
            bParaQuery = FALSE;
            usleep(20000);
        }
        else if(bAcDateSet || bAcTimeSet || bDemandPeriodSet || bSlipTimeSet || 
            bZoneNumSet || bPeriodNumSet || bZoneTableSet || bDayPeriodSet || 
            bParaSet)//��������
        {
            usleep(10000);
        }
        else//ʵʱ���ݲ�ѯ
        {
            usleep(200000);
        }
        
        // ���������ɻ���֡
    case READ_AC_RECV_STATE:
        recvLen = commdrv_Recv_232(fd_ac, recvBuf, sizeof(recvBuf), 200, 5, 0xFF);
        if (recvLen <= 0)
        {
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "��������Ϊ�� recvLen[%d] ���͵ı���:", recvLen);
            nRecvNullCnt ++;
            if(nRecvNullCnt >= nRecvNullMaxCnt)
            {
                ACCOMM_FMT_TRACE("ReadACSample() READ_AC_RECV_STATE...��������Ϊ�մ���nRecvNullCnt=%d, ����������nRecvNullMaxCnt=%d, ��ǰ���������eBaudRate=%d (%d:BAUDRATE9600, %d:BAUDRATE115200) \n", 
                    nRecvNullCnt, nRecvNullMaxCnt, eBaudRate, BAUDRATE9600, BAUDRATE115200);

                nRecvNullCnt = 0;
                if(eBaudRate == BAUDRATE9600)
                {
                    eBaudRate = BAUDRATE115200;
                }
                else if(eBaudRate == BAUDRATE115200)
                {
                    eBaudRate = BAUDRATE9600;
                }

                ACCOMM_FMT_TRACE("ReadACSample() READ_AC_RECV_STATE...�޸Ĳ���������, �л�֮���������eBaudRate=%d (%d:BAUDRATE9600, %d:BAUDRATE115200) \n", 
                    eBaudRate, BAUDRATE9600, BAUDRATE115200);
                
                state = CLOSE_AC_STATE;        // �л�����������
                gBaudRateChangedFlag = 1;      // �������л���־
                break;
            }
        }
        else
        {
            nRecvNullCnt = 0;
        }

        //���ȴ���UDP͸��
        if (bRelayUDP)//�յ�UDP���Ӧ����
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//�����ݴ�
            }
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "UDPת�����ձ���");
            
            PlcWriteQueue(pUdpSendDown, recvBuf, recvLen);
            bRelayUDP = FALSE;
            state = READ_AC_SEND_STATE;        // ���ط���
            break;
        }

        if (bAcDateAndTimeSet == TRUE)//�յ���������Ӧ����
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//�����ݴ�
            }
            
            initRet = check_timing_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "�������ں�ʱ����ձ���[%d](1�ɹ�; -1ʧ��)", initRet);
            bAcDateAndTimeSet = FALSE;
            state = READ_AC_SEND_STATE;        // ���ط���
            break;
        }

        if (bAcDateSet == TRUE)//�յ���������Ӧ����
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//�����ݴ�
            }
            initRet = check_timing_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "�������ڽ��ձ���[%d](1�ɹ�; -1ʧ��)", initRet);
            bAcDateSet = FALSE;
            state = READ_AC_SEND_STATE;        // ���ط���
            break;
        }

        if (bAcTimeSet == TRUE)//�յ�����ʱ��Ӧ����
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//�����ݴ�
            }
            initRet = check_timing_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "����ʱ����ձ���[%d](1�ɹ�; -1ʧ��)", initRet);
            bAcTimeSet = FALSE;
            state = READ_AC_SEND_STATE;        // ���ط���
            break;
        }

        if (bDataInit == TRUE)//�յ����ݳ�ʼ��Ӧ����
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//�����ݴ�
            }
            initRet = check_data_initial_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "���ݳ�ʼ�����ձ���[%d](1�ɹ�; -1ʧ��)", initRet);
            bDataInit = FALSE;
            state = READ_AC_SEND_STATE;        // ���ط���
            break;
        }

        if (bParamInit == TRUE)//�յ�������ʼ��Ӧ����
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//�����ݴ�
            }
            initRet = check_param_initial_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "������ʼ�����ձ���[%d](1�ɹ�; -1ʧ��)", initRet);
            bParamInit = FALSE;
            state = READ_AC_SEND_STATE;        // ���ط���
            break;
        }

        if (bDemandInit == TRUE)//�յ�������ʼ��Ӧ����
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//�����ݴ�
            }
            initRet = check_demand_initial_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "������ʼ�����ձ���[%d](1�ɹ�; -1ʧ��)", initRet);
            bDemandInit = FALSE;
            state = READ_AC_SEND_STATE;        // ���ط���
            break;
        }

        if (bDemandPeriodSet == TRUE)//�յ����������������Ӧ����
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//�����ݴ�
            }
            initRet = check_set_demand_period_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "��������������ڽ��ձ���[%d](1�ɹ�; -1ʧ��)", initRet);
            bDemandPeriodSet = FALSE;
            state = READ_AC_SEND_STATE;        // ���ط���
            break;
        }

        if (bSlipTimeSet == TRUE)//�յ����û���ʱ��Ӧ����
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//�����ݴ�
            }
            initRet = check_set_slip_time_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "���û���ʱ����ձ���[%d](1�ɹ�; -1ʧ��)", initRet);
            bSlipTimeSet = FALSE;
            state = READ_AC_SEND_STATE;        // ���ط���
            break;
        }

        if (bZoneNumSet == TRUE)//�յ�����ʱ������Ӧ����
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//�����ݴ�
            }
            initRet = check_set_zone_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "����ʱ���������ձ���[%d](1�ɹ�; -1ʧ��)", initRet);
            bZoneNumSet = FALSE;
            state = READ_AC_SEND_STATE;        // ���ط���
            break;
        }

        if (bPeriodNumSet == TRUE)//�յ�������ʱ�α�/ʱ������Ӧ����
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//�����ݴ�
            }
            initRet = check_set_day_period_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "������ʱ�α�/ʱ���������ձ���[%d](1�ɹ�; -1ʧ��)", initRet);
            bPeriodNumSet = FALSE;
            state = READ_AC_SEND_STATE;        // ���ط���
            break;
        }

        if (bZoneTableSet == TRUE)//�յ�����ʱ����Ӧ����
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//�����ݴ�
            }
            initRet = check_set_zone_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "����ʱ������ձ���[%d](1�ɹ�; -1ʧ��)", initRet);
            bZoneTableSet = FALSE;
            state = READ_AC_SEND_STATE;        // ���ط���
            break;
        }

        if (bDayPeriodSet == TRUE)//�յ�������ʱ�α�Ӧ����
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//�����ݴ�
            }
            initRet = check_set_day_period_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "������ʱ�α���ձ���[%d](1�ɹ�; -1ʧ��)", initRet);
            bDayPeriodSet = FALSE;
            state = READ_AC_SEND_STATE;        // ���ط���
            break;
        }

        if (bParaSet == TRUE)//�յ����ò���Ӧ����
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//�����ݴ�
            }
            initRet = check_set_para_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "���ò������ձ���[%d](1�ɹ�; -1ʧ��)", initRet);
            bParaSet = FALSE;
            state = READ_AC_SEND_STATE;        // ���ط���
            break;
        }

        if (bRelay485 == TRUE)//�յ�485���Ӧ����
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//�����ݴ�
            }
            ACCOMM_BUF_TRACE(recvBuf, recvLen, "485ת�����ձ���");
            
            PlcWriteQueue(pDealSendDown[pipe], recvBuf, recvLen);
            bRelay485 = FALSE;
            state = READ_AC_SEND_STATE;        // ���ط���
            break;
        }
        
        if (recvLen > 9)//��485���Ӧ����
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//�����ݴ�
            }
            ACCOMM_BUF_LOGCC(recvBuf, recvLen, "Recv: ");

            pstart = (uint8 *)recvBuf;
            leftlen = recvLen;
            while(leftlen >= 0x0d)//ѭ�����1�ֽڳ��Ⱥ�2�ֽڳ���, ��������ʧ���˳�
            {
                while(leftlen >= 0x0d)//���ĳ���ռ2�ֽ�
                {
                    memset(recvBufTemp, 0x00, sizeof(recvBufTemp));
                    ptr = (uint8*)commfun_memchr_gb((char *)pstart, 0x68, leftlen);
                    if(ptr != NULL)
                    {
                        leftlen -= (ptr - pstart);					
                        memmove(recvBufTemp, ptr, leftlen);
                        
                        bRet = STDAC_CheckRspFrame(recvBufTemp, leftlen);
                        if(bRet == 1)
                        {
                            CheckRetSum = 0;
                            validlen = (recvBufTemp[9] + (recvBufTemp[10]<<8) + 0x0d);  //������Ч����
                            ACCOMM_BUF_LOGCC(recvBufTemp, validlen, "L2��Ч��֡����: ");
                            Pro_AC_Amr_Response(recvBufTemp);
                            
                            pstart = ptr + validlen;
                            leftlen -= validlen;
                        }
                        else
                        {
                            ACCOMM_FMT_LOGCC("bRet=%d \n", bRet);
                            CheckRetSum++;
                            break;
                        }
                    }
                    else
                    {
                        ACCOMM_FMT_LOGCC("if(ptr == NULL) \n");
                        CheckRetSum++;
                        break;
                    }
                }

                while(leftlen >= 0x0d)//���ĳ���ռ1�ֽ�
                {
                    memset(recvBufTemp, 0x00, sizeof(recvBufTemp));
                    ptr = (uint8*)commfun_memchr_gb((char *)pstart, 0x68, leftlen);
                    if(ptr != NULL)
                    {
                        leftlen -= (ptr - pstart);					
                        memmove(recvBufTemp, ptr, leftlen);
                        bRet = STDAC_CheckRspFrame_Single(recvBufTemp, leftlen);
                        if(bRet == 1)
                        {
                            CheckRetSum = 0;
                            validlen = (recvBufTemp[9] + 0x0c);  //������Ч����
                            ACCOMM_BUF_LOGCC(recvBufTemp, validlen, "L1��Ч��֡����: ");
                            Pro_AC_Amr_Response_Single(recvBufTemp);
                            
                            pstart = ptr + validlen;
                            leftlen -= validlen;
                        }
                        else
                        {
                            ACCOMM_FMT_LOGCC("bRet=%d \n", bRet);
                            CheckRetSum++;
                            break;
                        }
                    }
                    else
                    {
                        ACCOMM_FMT_LOGCC("if(ptr == NULL) \n");
                        CheckRetSum++;
                        break;
                    }
                }

                if(CheckRetSum >= 2)
                {
                    ACCOMM_FMT_LOGCC("if(CheckRetSum >= 2) \n");
                    break;
                }
            }

            //ͳ��������(��ֱ�Ӳɼ�)
            UpdateStatistics(type);
        }

        state = READ_AC_SEND_STATE;        // ״̬��break����˳��ִ��
        break;

    case CLOSE_AC_STATE:
        ACCOMM_FMT_TRACE("ReadACSample() CLOSE_AC_STATE...\n");
        if (fd_ac >= 0)
        {
            close(fd_ac);
        }

        fd_ac = -1;

        state = INIT_AC_STATE;
        sleep(2);
        break;

    default:

        break;
    }
}

/*********************************************************************
    �������ܵ�Ԫ��HAL�ӿڶ�Ӧ�Ľ̲�ģ���շ���������
    ����������
    ���������
    ���������
        ��
    ��������ֵ��
    �ڲ��������̣�
********************************************************************/
void ReadACSampleHAL(BOOL isUpdateAC, BOOL isGetVer)
{
    int32 recvLen = 0;
    int32 sendLen = 0;
    int32 bRet = 0;
    uint8  recvBuf[2000];
    uint8  sendBuf[600];
    static READ_AC_STATE state = INIT_AC_STATE;
    static UART_DEVICE_T* dev = NULL;
    uint8 *ptr=NULL;  //Ӧ������֡��ʼָ��
    int ret = 0;

    memset(recvBuf, 0x00, sizeof(recvBuf));
    memset(sendBuf, 0x00, sizeof(sendBuf));

    switch (state)
    {
    case INIT_AC_STATE:
        ACPRINT(" ReadACSampleHAL() INIT_AC_STATE...\n");
        if(NULL == dev)
        {
            //dev = (UART_DEVICE_T*)hal_device_get(UART43_DEVICE_ID_STR);
            ACPRINT(" dev:%p \n", dev);
            sleep(1);
        }
        else
        {
            // ���豸
            ACPRINT(" dev:%p \n", dev);
            ret = dev->uart_param_set(dev, 9600, 8, 1, PARITY_EVEN);  //TRUE:1, FALSE:0
            //ret = dev->uart_param_set(dev, 115200, 8, 1, PARITY_NO);
            ACPRINT(" dev->uart_param_set() ret=%d \n", ret);
            if(ret > 0)
            {
                state = READ_AC_SEND_STATE;      // ״̬��break����˳��ִ��
            }
        }

        break;

    case READ_AC_SEND_STATE:  // ���ͳ������ɵ�֡
        ACPRINT(" ReadACSampleHAL() READ_AC_SEND_STATE...\n");
    
        sendLen = MakeFrameReadACModule(sendBuf);
        ret = dev->uart_data_send(dev, sendBuf, sizeof(sendLen));
        ACPRINT(" dev->uart_data_send() ret=%d \n", ret);
        if(ret > 0)
        {
            state = READ_AC_RECV_STATE;      // ״̬��break����˳��ִ��
        }
        printfBuff(sendBuf, sendLen, 0);
        
    case READ_AC_RECV_STATE:  // ���������ɻ���֡
        ACPRINT(" ReadACSampleHAL() READ_AC_RECV_STATE...\n");
        recvLen = dev->uart_data_recv(dev, recvBuf, sizeof(recvBuf));
        ACPRINT(" recvLen = %d\n", recvLen);
        
        if (recvLen > 0)
        {
            printfBuff(recvBuf, recvLen, 1);
            ptr = (uint8*)commfun_memchr_gb((char *)recvBuf, 0x68, recvLen);
            if(ptr != NULL)
            {
                recvLen -= (ptr - recvBuf);
                memmove(recvBuf, ptr, recvLen);
                bRet = STDAC_CheckRspFrame(recvBuf, recvLen);
                if(bRet == 1)
                {
                    Pro_AC_Amr_Response(recvBuf);
                }
            }
        }

        state = READ_AC_SEND_STATE;        // ״̬��break����˳��ִ��
        break;

    case CLOSE_AC_STATE:
        ACPRINT(" ReadACSampleHAL() CLOSE_AC_STATE...\n");
        if(NULL != dev)
        {
            // �ر��豸
            hal_device_release((HW_DEVICE *)dev);
        }

        dev = NULL;
        state = INIT_AC_STATE;
        break;

    default:

        break;
    }
}


/*******************************************************************************
* ��������: ac_calc_deal
* ��������: ���ɴ����߳����غ���
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void ac_calc_deal(uint32* cntchk)
{
    uint16 nEnergySecCounter = 3;         //�����ɼ����ʱ��
    uint16 nDemandSecCounter = 4;         //�����ɼ����ʱ��
    uint16 nHarmonicSecCounter = 4;       //г���ɼ����ʱ��
    uint16 nRealtimeSecCounter = 0;       //ʵʱ���ݲɼ����ʱ��
    uint8  nTaskType = SAMPLE_TYPE_NULL;  //��������

    uint16 nEnergySecCycle = 3;           //�����ɼ�����
    uint16 nDemandSecCycle = 5;           //�����ɼ�����
    uint16 nHarmonicSecCycle = 5;         //г���ɼ�����
    uint16 nRealtimeSecCycle = 1;         //ʵʱ���ݲɼ�����

    BOOL  isUpdateAC = FALSE;
    BOOL  isGetVer = FALSE;

    TimeManageInit();                      // ʱ�����
    SharedMemMark.Bitsect.FreshMark = 1;   // ����ˢ�½��ɹ�������

    while(1)
    {
        *cntchk = 0;
        TimeManageStart();                 // һ��ʱ�����ʼ
        nTaskType = SAMPLE_TYPE_NULL;      //��������

        // ���봦��
        if(gTimeFlag.BitSect.Second)
        {
            nEnergySecCounter ++;
            nDemandSecCounter ++;
            nHarmonicSecCounter ++;
            nRealtimeSecCounter ++;
            
            isUpdateAC = isJCUpdateRunning();//����Ƿ���Ҫ��������
            isGetVer = isJCNeedGetVer();     //�Ƿ���Ҫ��ȡ�汾��Ϣ
        }

        //�����������
        if(nDemandSecCounter >= nDemandSecCycle)
        {
            nDemandSecCounter = 0;
            nTaskType = SAMPLE_TYPE_DEMAND;  //�������� ����
            ACCOMM_FMT_TRACE("TASK:DEMAND, Cycle=%d \n", nDemandSecCycle);
        }
        else if(nHarmonicSecCounter >= nHarmonicSecCycle)
        {
            nHarmonicSecCounter = 0;
            nTaskType = SAMPLE_TYPE_HARMONIC;  //�������� г��
            ACCOMM_FMT_TRACE("TASK:HARMONIC, Cycle=%d \n", nHarmonicSecCycle);
        }
        else if(nEnergySecCounter >= nEnergySecCycle)
        {
            nEnergySecCounter = 0;
            nTaskType = SAMPLE_TYPE_ENERGY;  //�������� ����
            ACCOMM_FMT_TRACE("TASK:ENERGY, Cycle=%d \n", nEnergySecCycle);
        }
        else if(nRealtimeSecCounter >= nRealtimeSecCycle)
        {
            nRealtimeSecCounter = 0;
            nTaskType = SAMPLE_TYPE_REALTIME;  //�������� ʵʱ����
            ACCOMM_FMT_TRACE("TASK:REALTIME, Cycle=%d \n", nRealtimeSecCycle);
        }
        else
        {
            nTaskType = SAMPLE_TYPE_NULL;  //�������� ��
        }

#if AC_DEV_HAL == 1
        ReadACSampleHAL(FALSE, FALSE);
#else
        if (isUpdateAC == TRUE)
        {
            ReadACSample(TRUE, FALSE, nTaskType);
        }
        else if (isGetVer == TRUE)
        {
            ReadACSample(FALSE, TRUE, nTaskType);
        }
        else
        {
            ReadACSample(FALSE, FALSE, nTaskType);  //���ﲻ��Ҫ�жϹ���, ��Ϊǰ�����������ʱ���Ѿ��жϹ�
        }
#endif

        usleep(10000);                  // �ȴ�NEC��������,��Ϊ500ms,���ϲ�����̫��
        TimeManageEnd();                 // һ��ʱ��������
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
}


