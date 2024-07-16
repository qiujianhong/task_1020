/*
***************************************************************************
  Copyright (c) 2006-2007,�Ͼ��������������������ι�˾
             All rights reserved.
  ��������: template.h
  ��    �ܣ�FKGA42��ʾģ�鱳��ģ���
  �� �� �ˣ�gaoyf
  ����ʱ�䣺2007-1

  �汾��ʷ:

    v1.0:
      2006-1-8, gaoyf : New file

***************************************************************************
*/

#include "main.h"
#include "commdef.h"
#include "gui_prtl.h"
#include "hal.h"
#include "adc.h"
#include "power.h"

extern int g_narrMenuIndex[20];
extern int g_nMenuNodes;
extern BOOL IsMenuLayer(DWORD dwID);
extern BOOL g_bOftenDisp;
extern int g_nCurSelect;
// ��ǿ�ź�bmp·��
#define SIGNAL_ICON_COUNT	5
const char* g_icon_signal[]=
{
	"conn.bmp",
	"signal11.bmp",
	"signal12.bmp",
	"signal13.bmp",
	"signal14.bmp",
};

#define NET_ICON_COUNT	4
const char* g_icon_net[]=
{
	"2g.bmp",
	"3g.bmp",
	"4g.bmp",
	"5g.bmp",
};

#define ARROW_ICON_COUNT	3
const char* g_icon_arrow[]=
{
	"arrowupdown.bmp",
	"arrowup.bmp",
	"arrowdown.bmp",
};
// ͨ������bmp·��
#define PROTO_ICON_COUNT	4


const char* g_icon_proto[]=
{
    "gprs.bmp",         // 0
    "radio.bmp",        // 62
    "other-offline.bmp",// 13
    "L-online.bmp",//16
};
#if ANHUI_FUN == 1
//��Կbmp·��
#define ESAM_ICON_COUNT 1
char* g_icon_esam[]=
{
	"esam.bmp",
};
#endif
// �������bmp·��
#define MEASURE_ICON_COUNT	17
const char* g_icon_measure[]=
{
	"0.bmp",
	"mtr1.bmp",
	"mtr2.bmp",
	"mtr3.bmp",
	"mtr4.bmp",
	"mtr5.bmp",
	"mtr6.bmp",
	"mtr7.bmp",
	"mtr8.bmp",
	"mtr9.bmp",
	"mtr10.bmp",
	"mtr11.bmp",
	"mtr12.bmp",
	"mtr13.bmp",
	"mtr14.bmp",
	"mtr15.bmp",
	"mtr16.bmp",
};

#define ALARM_ICON_COUNT	1
const char* g_icon_alarm[]=
{
	"alarm.bmp",
};

#define BATFLAG_ICON_COUNT 3
const char *g_icon_Batflag[]=
{
    "lowpow_1.bmp",
    "power2.bmp",
    "fullpow.bmp",
};

#if SHANGHAI_FUN == 1
#define RS48CB_ICON_COUNT 4
const char* g_icon_485_cb[]=
{
    "485-1-T.bmp",
    "485-1-R.bmp",
    "485-2-T.bmp",
    "485-2-R.bmp",
};


#endif

typedef struct _TEMPLATE_ICON
{
	BITMAP		mapSignal[SIGNAL_ICON_COUNT];	// ��ǿ�ź�bitmap����
	int			bSignal[SIGNAL_ICON_COUNT];
    int			bSignal2[SIGNAL_ICON_COUNT];
    
    BITMAP		mapNet[NET_ICON_COUNT];	// ������ʽbitmap����
	int			bNet[NET_ICON_COUNT];
	
	BITMAP		mapProto[PROTO_ICON_COUNT];		// ͨ������bitmap����
	int			bProto[PROTO_ICON_COUNT];

	BITMAP		mapMeasure[MEASURE_ICON_COUNT];	// �������bitmap����
	int			bMeasure[MEASURE_ICON_COUNT];

	BITMAP		mapAlarm[ALARM_ICON_COUNT];		// �澯��Ϣbitmap����
	int			bAlarm[ALARM_ICON_COUNT];

	BITMAP		mapArrow[ARROW_ICON_COUNT];		// ��ͷ��Ϣbitmap����
	int			bArrow[ARROW_ICON_COUNT];

	#if ANHUI_FUN == 1
	BITMAP		mapEsam[ESAM_ICON_COUNT];	// ��ʽ��Կbitmap����
	int			bEsam[ESAM_ICON_COUNT];
	#endif

	BITMAP		mapBatFlag[BATFLAG_ICON_COUNT];     //��ص�������
	int			bBatFlag[BATFLAG_ICON_COUNT];
	
	#if SHANGHAI_FUN == 1
	

	BITMAP		map485cbFlag[RS48CB_ICON_COUNT];    //485����״̬
	int			b485cbFlag[RS48CB_ICON_COUNT];
	#endif
}TEMPLATE_ICON;
TEMPLATE_ICON g_template_icon;

typedef struct _TEMPLATE_INFO
{
	int			nState;
	int			nProto;
	BOOL		bLogin;		// �Ƿ��½
	BOOL		bShowLogin;	// û�е�½ʱ������˸
	int			nSignal;
	int 		nSignal2;
	int			nAlarm;
	BOOL		bShowAlarm;	// ������˸
	BOOL		bCbTask;	// �Ƿ��ڳ���״̬
	char		czStatusInfo[30];		// ״̬����Ϣ
	char		czStatusInfoTmp[30];	// ״̬����Ϣ

	char		czAlarmTime[4];
	char		czTaskTime[4];

	BOOL		bUpdate;
	int			nCurState;	// ��ǰ��ʾ��״̬���繦�ء���ء������

	BOOL		bInvalidate;
	char		bTimeCtrl;	// ʱ�ο��Ƿ�Ͷ��
	char		bFactoryCtrl;//���ݿ��Ƿ�Ͷ��
	char		bStopCtrl;	// Ӫҵ��ͣ���Ƿ�Ͷ��
	char		bFlowCtrl;	// �¸����Ƿ�Ͷ��

	char		bDayCtrl;	// �յ���Ƿ�Ͷ��
	char		bMonthCtrl;	// �µ���Ƿ�Ͷ��
	char		bBuyCtrl;	// ������Ƿ�Ͷ��

	BOOL		bConn;		// �Ƿ�����
	BITMAP		mapConn;	// �Ƿ�����

    BOOL		bConn2;		// �Ƿ�����

	BOOL		bProtect;	// �Ƿ񱣵�
	BITMAP		mapProtected;// �Ƿ񱣵�

	BOOL		bBat;		   // �Ƿ���ʾ���ͼ��
	int	        bBatFlag;	   // ���״̬�� 0������; 1�����; 2.����

	BOOL		bBlue;		   // �Ƿ���ʾ����ͼ��
	BITMAP	    mapBlue;	   // ����ͼ��
    
    BOOL		bEsam;	// �Ƿ���Կ��װ
	BITMAP		mapEsam;// �Ƿ���Կ��װ

    BOOL		bNet;	// �Ƿ���̫������
	BITMAP		mapNet; // �Ƿ���̫������
	
    #if SHANGHAI_FUN == 1
	BOOL		bPowerOff;	// �Ƿ�ͣ��
	BITMAP		mapPowerOff;// �Ƿ�ͣ��

	BOOL		bProgram;	// �Ƿ���
	BITMAP		mapProgram;// �Ƿ���

	int			nRS485_1;   //4851����״̬
	int			nRS485_2;   //4852����״̬
    #endif
}TEMPLATE_INFO;
TEMPLATE_INFO g_template_info;
// �ж��Ƿ����ڻ�ȡ״̬����Ϣ
// ��Ϊ��ȡ��ʹ���ַ����������߳��У������Ҫ����һ�㻥��
// ��û��ʹ���ź����ȣ�ֻʹ��һ����ʶ
BOOL g_bGetStatus = FALSE;
extern uint32 gethstatus;

#if SHANGHAI_FUN == 1
//��ʾ��ص���
void BatStateDraw(HDC hdc)
{
    int bRet = 0;
    int BatInfo = 0;				//��ǰ�ĵ��״̬
    static int lstBatInfo = 0;		//������ϴεĵ��״̬
    static int nCurBatTick = 0;		//����������س�����Ƿѹ��ʱ������¼�����ʾ�ĸ�ͼ��
    int iIndex = 0;

    //bRet = ReadRamXData (Tmn_Bat_Info_ID, (char *)&BatInfo);/*��ȡ�ն˺󱸵���Լ���Ϣ*/
    bRet = -1;
    //�ն˺󱸵���Լ���Ϣ  BIN 1 byte
    // 0x00��ʾ�豸�����ڣ�0x01��ʾ���ڳ�磬0x02��ʾ�ѳ�����0x03��־���ڷŵ磬������ʾǷѹ

    if(bRet < 0 || (unsigned char)BatInfo == 0xFF)
    {
        //TextOut(hdc,60,100+nOff,"--");
    }
    else
    {
        if(BatInfo & 0x80)
        {
            BatInfo = 5;
        }
        if(lstBatInfo != BatInfo)
        {
            lstBatInfo = BatInfo;
            nCurBatTick = 0;
        }

        if(BatInfo == 0)			//������
            iIndex = 0;
        else if(BatInfo == 1)		//���ڳ��
        {
            iIndex = nCurBatTick + 2;
            if(nCurBatTick >= 3)
                nCurBatTick = 0;
            else
                nCurBatTick ++;
        }
        else if(BatInfo == 2)		//�������
            iIndex = 5;
        else if(BatInfo == 3)		//���ڷŵ�
            iIndex = 4;
        else if(BatInfo == 5)		//Ƿѹ
        {
            if(nCurBatTick == 0)
            {
                iIndex = 2;
                nCurBatTick = 1;
            }
            else
            {
                iIndex = -1;
                nCurBatTick = 0;
            }
        }
        else
            iIndex = -1;

        if(iIndex != -1)
        {
            if(g_template_icon.bBatFlag[iIndex] == 0)
            {
                char czBuf[200];
                sprintf(czBuf, "%s%s", ICON_PATH, g_icon_Batflag[iIndex]);
                LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapBatFlag[iIndex], czBuf);
                g_template_icon.bBatFlag[iIndex] = 1;
            }
            FillBoxWithBitmap(hdc,130,146,16,12,&g_template_icon.mapBatFlag[iIndex]);
        }

    }
}

void GetPowerOffInfo(int nIndex)
{
    int8 bfgPower = 0;

    g_template_info.bPowerOff = FALSE;

    //ReadRamXData(Tmn_Power_Info_ID, (char *)&bfgPower);
    bfgPower = -1;

    if(bfgPower < 0)
    {
        g_template_info.bPowerOff = TRUE;
    }
    else
    {
        g_template_info.bPowerOff = FALSE;
    }
}

void GetProgramInfo(int nIndex)
{

}

void GetRS485CBInfo(int nIndex)
{
    char cb_485_1;
    char cb_485_2;

    g_template_info.nRS485_1 = -1;
    g_template_info.nRS485_2 = -1;

    //ReadRamXData(RS485I_Comm_Flag, &cb_485_1);
    cb_485_1 = 0x55;
    if(cb_485_1 == 0x55)
    {
        g_template_info.nRS485_1 = 0;
    }
    else if(cb_485_1 == 0xAA)
    {
        g_template_info.nRS485_1 = 1;
    }

    //ReadRamXData(RS485II_Comm_Flag, &cb_485_2);
    cb_485_2 = 0x55;
    if(cb_485_2 == 0x55)
    {
        g_template_info.nRS485_2 = 2;
    }
    else if(cb_485_2 == 0xAA)
    {
        g_template_info.nRS485_2 = 3;
    }

}
#endif

//�¼��澯��Ӧ
const char* OBJ[] =
{
	"3100",	//1
	"��Ч",//"������ʧ",
	"3118",
	"3104",
	"3115",				// 5
	"��Ч",//"������բ",
	"��Ч",//"�����բ",
	"��Ч",//"���ܱ�������",
	"��Ч",//"3119",
	"��Ч",//"��ѹ��·�쳣",			// 10
	"��Ч",//"�����쳣",
	"3105",
	"��Ч",//"���ܱ������Ϣ",
	"3106",
	"��Ч",//"г��Խ�޸澯",			// 15
	"��Ч",//"ֱ��ģ����Խ��",
	"��Ч",//"����/��ѹ��ƽ���Խ��",
	"��Ч",//"������Ͷ������",
	"3202",//"�����������",
	"3109",			// 20
	"310A",
	"3116",
	"��Ч",//"��ظ澯",		// 23��
	"��Ч",//"��ѹԽ��",
	"��Ч",//"����Խ��",				// 25
	"��Ч",//"���ڹ���Խ��",			// 26
	"310B",
	"310C",
	"310D",
	"310E",			// 30
	"310F",		    // 31
	"3110",			// 32
	"��Ч",//"���״̬�ֱ�λ",		// 33
	"��Ч",//"CT�쳣",				// 34
	"3111",			// 35
	"��Ч",//"���״̬�ֱ�λ",		// 33
	"��Ч",//"���״̬�ֱ�λ",		// 33
	"��Ч",//"���״̬�ֱ�λ",		// 33
	"��Ч",//"���״̬�ֱ�λ",		// 33
	"��Ч",//"���״̬�ֱ�λ",		// 33
	"3114",//"���״̬�ֱ�λ",		// 33
};

#define BATTERY_LOW				4650		//��ص�ѹ4.65V
#define BATTERY_FULL			6000	    //��ص�ѹ5.7V


/**********************************************************************
* @brief     �����õ�Դ������
* @param[in] val: 0�� ����磬 1�� ���
* @return    ��1 �ɹ��� <0 ʧ��
* @Create    : ÷����
* @Date      ��2020-07-27
* @Update    :
**********************************************************************/
int set_battery_charge(uint8 val)
{
#ifdef AREA_HUNAN	

	POWER_DEVICE_T* dev = (POWER_DEVICE_T*) hal_device_get(HW_DEVICE_ID_POWER);
	if(NULL == dev)
	{
        GUI_FMT_LOGCC("dev is null!\n");
        return -1;
	}

	if(dev->power_battery_charge_set(dev,val) < 0)
	{
		GUI_FMT_LOGCC("set_battery_charge failed!\n");
		return -1;
	}

	hal_device_release((HW_DEVICE *)dev);
#endif

	return 1;
}

/**********************************************************************
* @name      : check_power_status
* @brief     ������ն˵�Դ״̬
* @return    ��1 �ɹ��� <0 ʧ��
* @Create    : ÷����
* @Date      ��2020-07-27
* @Update    :
**********************************************************************/
int check_power_status()
{
	int vol = 0;

	static int count = 20;

	if(count == 20)
	{
		set_battery_charge(0);	//�ϵ�
	
		ADC_DEVICE_T* dev = (ADC_DEVICE_T*) hal_device_get(HW_DEVICE_ID_ADC);
		if(NULL == dev)
		{
	        GUI_FMT_LOGCC("dev is null!\n");
	        return -1;
		}
		
		if(dev->adc_volt_read(dev,EMADC_BATVOL_CHANNEL,&vol) < 0)
		{
			GUI_FMT_LOGCC("get power state failed!\n");
			return -1;
		}

	    GUI_FMT_LOGCC("get power state%d! \n", vol);
		hal_device_release((HW_DEVICE *)dev);

		set_battery_charge(1);	//���

		if(vol > BATTERY_FULL)		//�޵��
		{
	        g_template_info.bBatFlag = 0;
		}
		else if(vol <BATTERY_LOW )		//��س��
		{
	        g_template_info.bBatFlag = 1;
		}
		else	//�������
		{
	        g_template_info.bBatFlag = 2;
		}
		
	}
	
	count --;
	if(count <= 0)
	{
		count = 20;
	}
	
	return 1;
}



// ������Ϣ
void GetStatusInfo(HDC hdc)
{
	int nLeft = 0;
    int nTop = 0;
	// ��ǿ�ź�
    int nSignal = 0;
	
	if(toolInfoFresh.channel1.signalStrengh > 0  && toolInfoFresh.channel1.signalStrengh < 32)
	{
		nLeft = 81;
		FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_info.mapConn);	//��ʾ�ź���
		nSignal = toolInfoFresh.channel1.signalStrengh/6;
        if(nSignal > 4) 
    	{
			nSignal=4;
    	}

		nLeft += 11;
        if(nSignal > 0)
        {
            if(g_template_icon.bSignal[nSignal] == 0)
            {
                char czBuf[200];
                sprintf(czBuf, "%s%s", ICON_PATH, g_icon_signal[nSignal]);
                LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapSignal[nSignal], czBuf);
                g_template_icon.bSignal[nSignal] = 1;
            }
            FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_icon.mapSignal[nSignal]);
        }

        nLeft += 15;

        if(toolInfoFresh.channel1.netFormat >= 0 && (toolInfoFresh.channel1.conStatus == TRUE ||g_template_info.bConn == TRUE))
    	{
    	    if(toolInfoFresh.channel1.netFormat < NET_ICON_COUNT)
            {
    			if(g_template_icon.bNet[toolInfoFresh.channel1.netFormat] == 0)
        		{
        			char czBuf[200];
        			sprintf(czBuf, "%s%s", ICON_PATH, g_icon_net[toolInfoFresh.channel1.netFormat]);
        			LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapNet[toolInfoFresh.channel1.netFormat], czBuf);
        			g_template_icon.bNet[toolInfoFresh.channel1.netFormat] = 1;
        		}
        		FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_icon.mapNet[toolInfoFresh.channel1.netFormat]);
    		}
        }
        
	}

	if(toolInfoFresh.ethConStatus == 1)
	{
		nLeft = 124;
        char czBuf[200];
        sprintf(czBuf, "%s%s", ICON_PATH, g_icon_proto[3]);
        LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapProto[3], czBuf);
		FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_icon.mapProto[3]);
	}


	if(toolInfoFresh.radioStatus == 1)
	{
		nLeft = 141;
        char czBuf[200];
        sprintf(czBuf, "%s%s", ICON_PATH, g_icon_proto[1]);
        LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapProto[1], czBuf);
		FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_icon.mapProto[1]);
	}

}


void Template1Init(DWORD dwPluginId)
{
	memset(g_template_icon.bSignal, 0, sizeof(int)*SIGNAL_ICON_COUNT);
	memset(g_template_icon.bProto, 0, sizeof(int)*PROTO_ICON_COUNT);
	memset(g_template_icon.bMeasure, 0, sizeof(int)*MEASURE_ICON_COUNT);
	memset(g_template_icon.bAlarm, 0, sizeof(int)*ALARM_ICON_COUNT);
	memset(g_template_icon.bArrow, 0, sizeof(int)*ARROW_ICON_COUNT);

	// �����ͼ�겻��load��������Ҫ��ʱ���ٻ�ȡ
	/*
	// ��ȡ���е�bmp�ļ�����Ӧ�Ľṹ��ȥ
	char czBuf[200];
	int i;
	// ��ǿ�ź�bitmap����
	for(i = 0; i < SIGNAL_ICON_COUNT; i ++)
	{
		sprintf(czBuf, "%s%s", ICON_PATH, g_icon_signal[i]);
		LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapSignal[i], czBuf);
	}
	// ͨ������bitmap����
	for(i = 0; i < PROTO_ICON_COUNT; i ++)
	{
		sprintf(czBuf, "%s%s", ICON_PATH, g_icon_proto[i]);
		LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapProto[i], czBuf);
	}
	// �������bitmap����
	for(i = 0; i < MEASURE_ICON_COUNT; i ++)
	{
		sprintf(czBuf, "%s%s", ICON_PATH, g_icon_measure[i]);
		LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapMeasure[i], czBuf);
	}
	// �澯��Ϣbitmap����
	for(i = 0; i < ALARM_ICON_COUNT; i ++)
	{
		sprintf(czBuf, "%s%s", ICON_PATH, g_icon_alarm[i]);
		LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapAlarm[i], czBuf);
	}*/


	// ��ص���
	g_template_info.bBatFlag = BATFLAG_ICON_COUNT -1;

	g_template_info.nProto = -1;
	g_template_info.nState = 1;
	g_template_info.nSignal = 0;
	g_template_info.nAlarm = -1;

	g_template_info.bLogin = FALSE;
	g_template_info.bShowLogin = TRUE;

	g_template_info.bConn = FALSE;
	g_template_info.bProtect = FALSE;

	g_template_info.bBlue = FALSE;

	char czBuf[200];
	sprintf(czBuf, "%s%s", ICON_PATH, "conn.bmp");
	LoadBitmapFromFile(HDC_SCREEN, &g_template_info.mapConn, czBuf);

	sprintf(czBuf, "%s%s", ICON_PATH, "blue.bmp");
	LoadBitmapFromFile(HDC_SCREEN, &g_template_info.mapBlue, czBuf);

	sprintf(czBuf, "%s%s", ICON_PATH, "protect.bmp");
	LoadBitmapFromFile(HDC_SCREEN, &g_template_info.mapProtected, czBuf);

    sprintf(czBuf, "%s%s", ICON_PATH, "esam.bmp");
    LoadBitmapFromFile(HDC_SCREEN, &g_template_info.mapEsam, czBuf);

    sprintf(czBuf, "%s%s", ICON_PATH, "L-online.bmp");
    LoadBitmapFromFile(HDC_SCREEN, &g_template_info.mapNet, czBuf);
    
#if SHANGHAI_FUN == 1
	g_template_info.bPowerOff = FALSE;
	g_template_info.bProgram  = FALSE;
	sprintf(czBuf, "%s%s", ICON_PATH, "T.bmp");
	LoadBitmapFromFile(HDC_SCREEN, &g_template_info.mapPowerOff, czBuf);

	sprintf(czBuf, "%s%s", ICON_PATH, "P.bmp");
	LoadBitmapFromFile(HDC_SCREEN, &g_template_info.mapProgram, czBuf);
#endif
	g_template_info.bCbTask = FALSE;

	g_template_info.nCurState = -1;
	g_template_info.bInvalidate = FALSE;
}
void Template1Uninit()
{
	int i;
	// �ͷ����е�bmp
	// ͨ������bitmap����
	for(i = 0; i < SIGNAL_ICON_COUNT; i ++)
	{
		if(g_template_icon.bSignal[i] != 0)
			UnloadBitmap(&g_template_icon.mapSignal[i]);
	}

    // ������ʽbitmap����
	for(i = 0; i < NET_ICON_COUNT; i ++)
	{
		if(g_template_icon.bNet[i] != 0)
			UnloadBitmap(&g_template_icon.mapNet[i]);
	}
    
	// ͨ������bitmap����
	for(i = 0; i < PROTO_ICON_COUNT; i ++)
	{
		if(g_template_icon.bProto[i] != 0)
			UnloadBitmap(&g_template_icon.mapProto[i]);
	}
	// �������bitmap����
	for(i = 0; i < MEASURE_ICON_COUNT; i ++)
	{
		if(g_template_icon.bMeasure[i] != 0)
			UnloadBitmap(&g_template_icon.mapMeasure[i]);
	}
	// �澯��Ϣbitmap����
	for(i = 0; i < ALARM_ICON_COUNT; i ++)
	{
		if(g_template_icon.bAlarm[i] != 0)
			UnloadBitmap(&g_template_icon.mapAlarm[i]);
	}

	// ��ͷ��Ϣbitmap����
	for(i = 0; i < ARROW_ICON_COUNT; i ++)
	{
		if(g_template_icon.bArrow[i] != 0)
			UnloadBitmap(&g_template_icon.mapArrow[i]);
	}

	// ��ص���
	for(i = 0; i < BATFLAG_ICON_COUNT; i ++)
	{
		if(g_template_icon.bBatFlag[i] != 0)
			UnloadBitmap(&g_template_icon.mapBatFlag[i]);
	}

	// �ͷ����ߡ�����ͼ��
	UnloadBitmap(&g_template_info.mapConn);
	UnloadBitmap(&g_template_info.mapProtected);
    UnloadBitmap(&g_template_info.mapEsam);
	UnloadBitmap(&g_template_info.mapNet);
	UnloadBitmap(&g_template_info.mapBlue);
#if ANHUI_FUN == 1
	//�ͷ���ʽ��Կͼ��
	for(i=0;i < ESAM_ICON_COUNT;i++)
	{
		if(g_template_icon.bEsam[i] != 0)
			UnloadBitmap(&g_template_icon.mapEsam[i]);
	}
#endif
}

void Template1Draw(HDC hdc)
{
	char czBuf[100];
	

	//MoveTo(hdc, 0, 14);
   	//LineTo(hdc, 240, 14);

	SelectFont(hdc,g_guiComm.fontBig);

	GetStatusInfo(hdc);

	time_t t;
	time(&t);
	tm *p = localtime(&t);
	sprintf(czBuf, "%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec);
	TextOut(hdc, 175, -2, czBuf);

	if((g_guiMain.bTrunDisp == TRUE && g_guiMain.nTrunDispCnt > 0)
		|| g_guiMain.plugMsg != NULL
		|| g_guiMain.plugPop != NULL)
	{
		sprintf(czBuf, "%04d-%02d-%02d", p->tm_year+1900, p->tm_mon+1, p->tm_mday);
		TextOut(hdc, 4, -2, czBuf);
	}
	else
	{
        g_nMenuNodes = MAX(1, g_nMenuNodes);
        if (g_nMenuNodes == 1){
            g_narrMenuIndex[0] = g_nCurSelect;
        }
		char* pBuf = czBuf;
		for(int i = 0; i < g_nMenuNodes; i ++)
		{
			if(i == 0)
			{
				sprintf(pBuf, "%d", g_narrMenuIndex[i]+1);
			}
			else
			{
				sprintf(pBuf, "-%d", g_narrMenuIndex[i]+1);
			}
			pBuf += strlen(pBuf);
		}
		if(g_nMenuNodes < 2)
		{
			sprintf(pBuf, "-0");
			pBuf += strlen(pBuf);
		}
		*pBuf = ' ';
		pBuf ++;
		*pBuf = '\0';
		
		if(IsMenuLayer(g_guiMain.pluginCur->dwId))
		{
			if( g_bOftenDisp == FALSE)
			{
				TextOut(hdc, 2, -2, czBuf);
				pBuf = czBuf;
				if(g_nParentIndex == -1)
				{
					sprintf(pBuf, "���˵�");
				}
				else
				{
					MENU_INFO* pMenu = g_menuMain[g_nParentIndex];
					sprintf(pBuf, pMenu->czCaption);
				}
				
				RECT rc;
				rc.left = 0;
				rc.right = g_guiComm.nWidth;
				rc.top = 24;
				rc.bottom = rc.top+16;
				DrawText(hdc, pBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
			}
		}
		else
		{
			int nLen = strlen(czBuf);
			TextOut(hdc, 2, -2, czBuf);
			pBuf = czBuf;
			MENU_INFO* pMenu = g_guiMain.pPrentMenu;
			sprintf(pBuf, pMenu->pPlugStruct[g_guiMain.nPageLayerIndex].czCaption);
			SelectFont(hdc,g_guiComm.fontSmall);
			TextOut(hdc, 2+nLen*8, 1, pBuf);
		}
	}

    //(g_template_info.bBat == TRUE)
    //
        if(g_template_icon.bBatFlag[g_template_info.bBatFlag] == 0)
        {
            char czBuf[200];
            sprintf(czBuf, "%s%s", ICON_PATH, g_icon_Batflag[g_template_info.bBatFlag]);
            LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapBatFlag[g_template_info.bBatFlag], czBuf);
            g_template_icon.bBatFlag[g_template_info.bBatFlag] = 1;
        }
        FillBoxWithBitmap(hdc,158,0,0,0,&g_template_icon.mapBatFlag[g_template_info.bBatFlag]);
    //
	SelectFont(hdc,g_guiComm.fontSmall);
}

/**********************************************************************
* @name      : get_battery_status
* @brief     ������ն˵��״̬���״̬
* @param[in] ��
* @param[out]��
* @return    ������Ϊ��0���ε�Ϊ��1�� <0 ʧ��
* @Create    : ÷����
* @Date      ��2022-06-25
* @Update    :
**********************************************************************/
int evt_battery_insert_check(void)
{
    int32 fd = -1;
    uint32 uData;
    int32 iRet = ERR_OK;

    fd = open("/dev/ym", O_RDONLY|O_NONBLOCK);

    if(fd < 0)
    {
        GUI_FMT_DEBUG("open get_battery_status file error!\n");
        return ERR_NORMAL;
    }
    
    /* ��ذβ���״̬ */
    if(read(fd, &uData, sizeof(uData)) 
        != sizeof(uData))
    {
       iRet = ERR_IO;
       close(fd);
       return iRet;  
    }

    iRet = (uData>>8)&0x01;
    //1�γ���� 0�е��

    //GUI_FMT_DEBUG(" get_battery_status [%x]!\n", iRet);

  //  printf("power.data = %d\n", data);
    close(fd);
    return iRet;    
}

void Template1Timer(int nId)
{
    int             status = 0;
	if(nId == TIMER_MAIN)
	{
		if(toolInfoFresh.warning == 1 && toolInfoFresh.keyStatus == 0)
		{// ����澯��esam����Ҫ��˸��������������˸
		    g_template_info.bEsam = g_template_info.bShowAlarm;
			g_template_info.bShowAlarm = !g_template_info.bShowAlarm;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}
		else if(toolInfoFresh.warning == 1)
		{// ����Ѿ��澯����˸
			g_template_info.bShowAlarm = !g_template_info.bShowAlarm;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}
		else if(toolInfoFresh.keyStatus == 0)
		{
			g_template_info.bEsam = !g_template_info.bEsam;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}

        if((g_template_info.bLogin == FALSE)&&(gethstatus == TRUE))
		{
			g_template_info.bShowLogin = !g_template_info.bShowLogin;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}
		if(gethstatus == FALSE)
		{
			g_template_info.bShowLogin =FALSE;
			g_template_info.bNet =FALSE;
		}
		
		if(toolInfoFresh.channel1.conStatus == FALSE && toolInfoFresh.channel2.conStatus == FALSE)
		{
			g_template_info.bConn = !g_template_info.bConn;
            g_template_info.bConn2 = g_template_info.bConn;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}
        else if(toolInfoFresh.channel1.conStatus == FALSE)
		{
			g_template_info.bConn = !g_template_info.bConn;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}
        else if(toolInfoFresh.channel2.conStatus == FALSE)
		{
			g_template_info.bConn2 = !g_template_info.bConn2;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}

        if((toolInfoFresh.ethConStatus == FALSE)&&(gethstatus == TRUE))
		{
			g_template_info.bNet = !g_template_info.bNet;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}

		if(toolInfoFresh.blueStatus == 1)
	    { 
	       	g_template_info.bBlue = !g_template_info.bBlue;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
	    }

		g_template_info.bBat = !g_template_info.bBat;
        status = evt_battery_insert_check();

        if(status != 1)            //1Ϊ�γ�״̬
        {
            status = 0;
        }
        else
        {
            g_template_info.bBatFlag = 0;
        }

        if(status == 0) 
		    check_power_status();
        
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
	}
}
PLUGIN_INFO g_Template1 = {
	0x20FFFFFF/*id*/,
	Template1Init/*InitLayer*/,
	Template1Uninit/*UninitLayer*/,
	Template1Draw/*Draw*/,
	Template1Timer/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};

//����ҳ��
void pluginInit(DWORD dwPluginId)
{
}

void pluginDraw(HDC hdc, const char* ptype)
{
	SelectFont(hdc, g_guiComm.fontBig);
	ITEM_NOT_SELECT;

	char czBuf[64] = { 0 };

	int tmpTop = 22;

	RECT rc;
	rc.left = 0;
	rc.right = g_guiComm.nWidth;
	rc.top = 38;
	rc.bottom = rc.top + 16;
	sprintf(czBuf, "��Դ������");
	DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    rc.top += tmpTop;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "(%s)", ptype);
	DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    // ��ȡ��ǰʱ��
	long lTime = time(NULL);
	struct tm* tmNow = localtime(&lTime);

    rc.top += tmpTop;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "%04d-%02d-%02d", tmNow->tm_year + 1900, tmNow->tm_mon + 1, tmNow->tm_mday);
	DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    rc.top += tmpTop;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "%02d:%02d:%02d", tmNow->tm_hour, tmNow->tm_min, tmNow->tm_sec);
	DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

}

void pluginDrawZCU(HDC hdc)
{
	SelectFont(hdc, g_guiComm.fontBig);
	ITEM_NOT_SELECT;

	char czBuf[64] = { 0 };

	int tmpTop = 22;

	RECT rc;
	rc.left = 0;
	rc.right = g_guiComm.nWidth;
	rc.top = 40;
	rc.bottom = rc.top + 16;
    
    #ifdef AREA_JIANGSU
    sprintf(czBuf, "����ר��ɼ��ն�I��");
    #else
	sprintf(czBuf, "����ר��ɼ��ն�I��");
    #endif
	DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    // ��ȡ��ǰʱ��
	long lTime = time(NULL);
	struct tm* tmNow = localtime(&lTime);

    rc.top += tmpTop;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "%04d-%02d-%02d", tmNow->tm_year + 1900, tmNow->tm_mon + 1, tmNow->tm_mday);
	DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    rc.top += tmpTop;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "%02d:%02d:%02d", tmNow->tm_hour, tmNow->tm_min, tmNow->tm_sec);
	DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

}


void pluginDraw_puamr(HDC hdc)
{
	pluginDrawZCU(hdc);
}

void pluginDraw_stamr(HDC hdc)
{
	pluginDrawZCU(hdc);
}


PLUGIN_INFO g_puamr = {
	0xFFF/*id*/,
	pluginInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	pluginDraw_puamr/*Draw*/,
	NULL/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};

PLUGIN_INFO g_stamr = {
	0xFFFF/*id*/,
	pluginInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	pluginDraw_stamr/*Draw*/,
	NULL/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};


const char* g_czCchState[] =
{
	"�򿪴���",//0
	"���ͨ��ģ��",//1
	"���SIM��",//2
	"����ע��",//3
	"��ȡ�ź�",//4
	"��ȡͨ��ģ���ͺ�",//5
	"����APN",//6
	"���gprs����",//7
	"��ʼ����",//8
	"PPP��֤",//9
	"�������ӷ�����",//10
	"���ö������ݸ�ʽ",//11
	"���ö��Ŵ洢ģʽ",//12
	"����������ʾ",//13
	"LCP��·Э��",//14
	"IPCPЭ��",//15
	"���ӷ������ɹ�",//16

	//#if COMM_RETRY == 1	��Ҫɾ����Ϊ����ֲ��ʱ���ܹ��������ñ�ʶ
	"���͵�¼����",		// 17
	"������������",		// 18
	"�����˳�����",		// 19
	"�յ���¼ȷ��",		// 20
	"�յ�����ȷ��",		// 21
	"�յ��˳�ȷ��",		// 22

	//#if WDASN_RADIO_EN == 1	��Ҫɾ����Ϊ����ֲ��ʱ���ܹ��������ñ�ʶ
	"ɨ�赱ǰ�ŵ�",		//23
	"��������",			//24
	"��������",			//25
	"���ӳɹ�",			//26
	"������������͵�ַ",	//27
	"��������",			//28
	"���ӵȴ�",			//29
	"���õ�½�����ͼ��",	//30

	//#if ETH_MODEL == 1		��Ҫɾ����Ϊ����ֲ��ʱ���ܹ��������ñ�ʶ
	"�ر���̫ģ��",		// 31
	"������̫ģ��",		// 32
	"��ʼ����̫ģ��",	// 33
	"�����ն�IP��ַ",	// 34
	"�����ն�����",		// 35

	////#if TD_SCDMA == 1		//TD-SCDMA����
	"�ر������źŷֱ�ֵ���ϱ�",	//36//TD_AT_DSQ_0
	"�ر������ϱ�",				//TD_AT_CREG_0
	"����USIM��",					//TD_AT_CFUN_5
	"����Э��ջ",					//TD_AT_CFUN_1
	"�Զ�����GPRS",				//TD_AT_DCTA
	"��������",						//TD_AT_COPS
	"����PSҵ��",					//TD_AT_CGEQREQ
	"�ر�MT�Զ�Ӧ��",				//TD_AT_ATS0
	"��������",					//TD_AT_TEST
	"����Ϊ�Զ�TDģʽ",			//AT_TD_DDTM_TD45
	"����Ϊ�Զ�GPRSģʽ",		//AT_TD_DDTM_GPRS46
	"���ģ������ģʽ",			//47

	//#if WORK_MODE == 1
	"�ȴ��ͻ�������",			//48//�ȴ��ͻ�������

	"�򿪴���",//49
	"���SIM��",//50
	"���������ϱ�",//51
	"����ע��",//52
	"��ȡ�ź�",//53
	"��ȡͨ��ģ���ͺ�",//54
	"����APN",//55
	"���ӷ�����",//56
	"��ȡ�ն�ע���IP��ַ",//57
	"���÷������",//58
	"���gprs����",//59
	"���ӷ������ɹ�",//60
	"���ӽ���",//61
};
int g_nCchStateCnt = 62;

const char* g_czCchErr[] =
{
	"�򿪴���ʧ��",//0
	"ͨ��ģ���쳣",//1
	"SIM���쳣",//2
	"ע������ʧ��",//3
	"�ź���",//4
	"��ȡͨ��ģ���ͺ�ʧ��",//5
	"APN����ʧ��",//6
	"��gprs����",//7
	"���Ų��ɹ�",//8
	"PPP��֤ʧ��",//9
	"���ӷ�����ʧ��",//10
	"���ö������ݸ�ʽʧ��",//11
	"���ö��Ŵ洢ģʽʧ��",//12
	"����������ʾʧ��",//13
	"LCP��·Э��ʧ��",//14
	"IPCPЭ��ʧ��",//15
	//#if WDASN_RADIO_EN == 1	��Ҫɾ����Ϊ����ֲ��ʱ���ܹ��������ñ�ʶ
	"����ʧ��",//16
	//#if ETH_MODEL == 1		��Ҫɾ����Ϊ����ֲ��ʱ���ܹ��������ñ�ʶ
	"��ʼ����̫ģ��ʧ��",//17
	0,//18
	0,//19
	0,//20
	0,//21
	0,//22
	0,//23
	0,//24
	0,//25
	0,//26
	0,//27
	0,//28
	0,//29
	0,//30
	0,//31
	0,//32
	0,//33
	0,//34
	0,//35
	0,//36
	0,//37
	0,//38
	0,//39
	0,//40
	0,//41
	0,//42
	0,//43
	0,//44
	0,//45
	0,//46
	0,//47
	0,//48
	"�򿪴���ʧ��",//49
	"SIM���쳣",//50
	"ͨ��ģ���쳣",//51
	"ע������ʧ��",//52
	"�ź���",//53
	"��ȡͨ��ģ���ͺ�ʧ��",//54
	"APN����ʧ��",//55
	"���ӷ�����ʧ��",//56
	"��ȡ�ն�IP��ַʧ��",//57
	"���÷������ʧ��",//58
	"��gprs����",//59
	"����ʧ��",//60
	"��ʼ����̫ģ��ʧ��",//61
};

int g_nCchErrCnt = 62;



void SetCtrlsPara(char* pCtrl, char czVal, int nIndex, const char* pInfo)
{
	//printf("%s  %d  %d %d  %d\n", pInfo, *pCtrl, czVal, g_template_info.bUpdate, g_template_info.nCurState);
	if(czVal != *pCtrl)
	{
		*pCtrl = czVal;
		if(g_template_info.bUpdate == FALSE)
		{
			sprintf(g_template_info.czStatusInfoTmp, "%s%s", pInfo, (czVal==1)?"Ͷ��":"���");
			g_template_info.nCurState = nIndex;
			g_template_info.bUpdate = TRUE;
		}
	}
	else if(g_template_info.bUpdate == FALSE && g_template_info.nCurState == nIndex)
	{
		sprintf(g_template_info.czStatusInfoTmp, "%s%s", pInfo, (czVal==1)?"Ͷ��":"���");
	}
}


BITMAP mapLogo;
void WelcomeInit(DWORD dwPluginId)
{
	char czBuf[300];
	sprintf(czBuf, "%slogo.bmp", ICON_PATH);
	LoadBitmapFromFile(HDC_SCREEN, &mapLogo, czBuf);
}
void WelcomeUninit()
{
	UnloadBitmap(&mapLogo);
}
void WelcomeDraw(HDC hdc)
{
	FillBoxWithBitmap(hdc, 0, 17, 160, 130, &mapLogo);
}
PLUGIN_INFO g_WelcomeLayer = {
	0xFFFFFE/*id*/,
	WelcomeInit/*InitLayer*/,
	WelcomeUninit/*UninitLayer*/,
	WelcomeDraw/*Draw*/,
	NULL/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};
