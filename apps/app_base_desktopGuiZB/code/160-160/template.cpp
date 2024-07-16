/*
***************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限责任公司
             All rights reserved.
  程序名称: template.h
  功    能：FKGA42显示模块背景模板库
  开 发 人：gaoyf
  开发时间：2007-1

  版本历史:

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
#include "msg_base.h"
#include "out_msg.h"


// 场强信号bmp路径
#define SIGNAL_ICON_COUNT	17
#ifdef AREA_CHONGQING
const char* g_icon_signal[]=
{
	"conn.bmp",
	"signal11.bmp",
	"signal12.bmp",
	"signal13.bmp",
	"signal14.bmp",
	"gprs.bmp",
};
#else
const char* g_icon_signal[]=
{
	"conn.bmp",
	"signal2g1.bmp",
	"signal2g2.bmp",
	"signal2g3.bmp",
	"signal2g4.bmp",
	"signal3g1.bmp",
	"signal3g2.bmp",
	"signal3g3.bmp",
	"signal3g4.bmp",
	"signal4g1.bmp",
	"signal4g2.bmp",
	"signal4g3.bmp",
	"signal4g4.bmp",
	"signal5g1.bmp",
	"signal5g2.bmp",
	"signal5g3.bmp",
	"signal5g4.bmp",
};
#endif
const char* g_icon_netFormat[]=
{
	"2g.bmp",
	"3g.bmp",
	"4g.bmp",
	"5g.bmp",
	"cdma.bmp",
	"gprs.bmp",
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
// 通信类型bmp路径
#if TD_SCDMA == 1
#define PROTO_ICON_COUNT	24
#else
#define PROTO_ICON_COUNT	22
#endif

const char* g_icon_proto[]=
{
    "gprs.bmp",         // 0
    "gprs-offline.bmp", // 1
    "cdma.bmp",         // 2
    "cdma-offline.bmp", // 3
    "sms.bmp",          // 4
    "sms-offline.bmp",  // 5
    "radio.bmp",        // 6
    "radio-offline.bmp",// 7
    "rs232.bmp",        // 8
    "rs232-offline.bmp",// 9
    "csd.bmp",          // 10
    "csd-offline.bmp",  // 11
    "other.bmp",        // 12
    "other-offline.bmp",// 13
    "pstn.bmp",			// 14
    "pstn-offline.bmp",	// 15
    #if SHANGHAI_FUN == 1
    "E.bmp",//16
    "E-offline.bmp",//17
    #else
    "L.bmp",//16
    "L-offline.bmp",//17
    #endif
	#if TD_SCDMA == 1
	"td.bmp",			// 18
	"td-offline.bmp",	// 19
	#endif
    "3g.bmp",//20
    "3gx.bmp",//21
    "4g.bmp",//22
    "4gx.bmp",//23
};
#if ANHUI_FUN == 1
//密钥bmp路径
#define ESAM_ICON_COUNT 1
char* g_icon_esam[]=
{
	"esam.bmp",
};
#endif
// 测量点号bmp路径
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
	BITMAP		mapSignal[SIGNAL_ICON_COUNT];	// 场强信号bitmap数组
	int			bSignal[SIGNAL_ICON_COUNT];
    int			bSignal2[SIGNAL_ICON_COUNT];
    
    BITMAP		mapNet[NET_ICON_COUNT];	// 网络制式bitmap数组
	int			bNet[NET_ICON_COUNT];
	
	BITMAP		mapProto[PROTO_ICON_COUNT];		// 通信类型bitmap数组
	int			bProto[PROTO_ICON_COUNT];

	BITMAP		mapMeasure[MEASURE_ICON_COUNT];	// 测量点号bitmap数组
	int			bMeasure[MEASURE_ICON_COUNT];

	BITMAP		mapAlarm[ALARM_ICON_COUNT];		// 告警信息bitmap数组
	int			bAlarm[ALARM_ICON_COUNT];

	BITMAP		mapArrow[ARROW_ICON_COUNT];		// 箭头信息bitmap数组
	int			bArrow[ARROW_ICON_COUNT];

	#if ANHUI_FUN == 1
	BITMAP		mapEsam[ESAM_ICON_COUNT];	// 正式密钥bitmap数组
	int			bEsam[ESAM_ICON_COUNT];
	#endif

	BITMAP		mapBatFlag[BATFLAG_ICON_COUNT];     //电池电量数组
	int			bBatFlag[BATFLAG_ICON_COUNT];
	
	#if SHANGHAI_FUN == 1
	

	BITMAP		map485cbFlag[RS48CB_ICON_COUNT];    //485抄表状态
	int			b485cbFlag[RS48CB_ICON_COUNT];
	#endif
}TEMPLATE_ICON;
TEMPLATE_ICON g_template_icon;

typedef struct _TEMPLATE_INFO
{
	int			nState;
	int			nProto;
	BOOL		bLogin;		// 是否登陆
	BOOL		bShowLogin;	// 没有登陆时用于闪烁
	int			nSignal;
	int 		nSignal2;
	int			nAlarm;
	BOOL		bShowAlarm;	// 用于闪烁
	BOOL		bCbTask;	// 是否处于抄表状态
	char		czStatusInfo[30];		// 状态条信息
	char		czStatusInfoTmp[30];	// 状态条信息

	char		czAlarmTime[4];
	char		czTaskTime[4];

	BOOL		bUpdate;
	int			nCurState;	// 当前显示的状态，如功控、电控、购电控

	BOOL		bInvalidate;
	char		bTimeCtrl;	// 时段控是否投入
	char		bFactoryCtrl;//厂休控是否投入
	char		bStopCtrl;	// 营业报停控是否投入
	char		bFlowCtrl;	// 下浮控是否投入

	char		bDayCtrl;	// 日电控是否投入
	char		bMonthCtrl;	// 月电控是否投入
	char		bBuyCtrl;	// 购电控是否投入

	BOOL		bConn;		// 是否上线
	BITMAP		mapConn;	// 是否上线

    BOOL		bConn2;		// 是否上线

	BOOL		bProtect;	// 是否保电
	BITMAP		mapProtected;// 是否保电

	BOOL		bBat;		   // 是否显示电池图标
	int	        bBatFlag;	   // 电池状态， 0，掉电; 1，充电; 2.满电

	BOOL		bBlue;		   // 是否显示蓝牙图标
	BITMAP	    mapBlue;	   // 蓝牙图标
    
    BOOL		bEsam;	// 是否秘钥下装
	BITMAP		mapEsam;// 是否秘钥下装

    BOOL		bNet;	// 是否以太网连接
	BITMAP		mapNet; // 是否以太网连接
    BITMAP		wireNet; // 是否无线网连接
	
    #if SHANGHAI_FUN == 1
	BOOL		bPowerOff;	// 是否停电
	BITMAP		mapPowerOff;// 是否停电

	BOOL		bProgram;	// 是否编程
	BITMAP		mapProgram;// 是否编程

	int			nRS485_1;   //4851抄表状态
	int			nRS485_2;   //4852抄表状态
    #endif
}TEMPLATE_INFO;
TEMPLATE_INFO g_template_info;
// 判断是否正在获取状态栏信息
// 因为获取和使用字符串在两个线程中，因此需要采用一点互斥
// 但没有使用信号量等，只使用一个标识
BOOL g_bGetStatus = FALSE;
extern uint32 gethstatus;

#if SHANGHAI_FUN == 1
//显示电池电量
void BatStateDraw(HDC hdc)
{
    int bRet = 0;
    int BatInfo = 0;				//当前的电池状态
    static int lstBatInfo = 0;		//缓存的上次的电池状态
    static int nCurBatTick = 0;		//计数器，电池充电或者欠压的时候，来记录电池显示哪个图标
    int iIndex = 0;

    //bRet = ReadRamXData (Tmn_Bat_Info_ID, (char *)&BatInfo);/*读取终端后备电池自检信息*/
    bRet = -1;
    //终端后备电池自检信息  BIN 1 byte
    // 0x00表示设备不存在，0x01表示正在充电，0x02表示已充满，0x03标志正在放电，负数表示欠压

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

        if(BatInfo == 0)			//不存在
            iIndex = 0;
        else if(BatInfo == 1)		//正在充电
        {
            iIndex = nCurBatTick + 2;
            if(nCurBatTick >= 3)
                nCurBatTick = 0;
            else
                nCurBatTick ++;
        }
        else if(BatInfo == 2)		//充电已满
            iIndex = 5;
        else if(BatInfo == 3)		//正在放电
            iIndex = 4;
        else if(BatInfo == 5)		//欠压
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

//事件告警对应
const char* OBJ[] =
{
	"3100",	//1
	"无效",//"参数丢失",
	"3118",
	"3104",
	"3115",				// 5
	"无效",//"功控跳闸",
	"无效",//"电控跳闸",
	"无效",//"电能表参数变更",
	"无效",//"3119",
	"无效",//"电压回路异常",			// 10
	"无效",//"相序异常",
	"3105",
	"无效",//"电能表故障信息",
	"3106",
	"无效",//"谐波越限告警",			// 15
	"无效",//"直流模拟量越限",
	"无效",//"电流/电压不平衡度越限",
	"无效",//"电容器投切自锁",
	"3202",//"购电参数设置",
	"3109",			// 20
	"310A",
	"3116",
	"无效",//"电控告警",		// 23无
	"无效",//"电压越限",
	"无效",//"电流越限",				// 25
	"无效",//"视在功率越限",			// 26
	"310B",
	"310C",
	"310D",
	"310E",			// 30
	"310F",		    // 31
	"3110",			// 32
	"无效",//"电表状态字变位",		// 33
	"无效",//"CT异常",				// 34
	"3111",			// 35
	"无效",//"电表状态字变位",		// 33
	"无效",//"电表状态字变位",		// 33
	"无效",//"电表状态字变位",		// 33
	"无效",//"电表状态字变位",		// 33
	"无效",//"电表状态字变位",		// 33
	"3114",//"电表状态字变位",		// 33
};

#define BATTERY_LOW				4650		//电池电压4.65V
#define BATTERY_FULL			6000	    //电池电压6V

/**********************************************************************
* @brief     ：备用电源充电控制
* @param[in] val: 0： 不充电， 1： 充电
* @return    ：1 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2020-07-27
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
* @name      : get_battery_status
* @brief     ：检查终端电池状态插拔状态
* @param[in] ：
* @param[out]：
* @return    ：1-插入/0-没有插入 -1错误
* @Create    : 梅安宁
* @Date      ：2022-06-25
* @Update    :
**********************************************************************/
int battery_insert_check()
{
    uint8  type = 1;
    uint32 data1 = 1;
    uint32 data2 = 2;
    uint32 data3 = 3;
    uint32 data4 = 4;
    uint32 data5 = 5;
    int bReadCount = 0;
    
    int fd = open("/dev/ym", O_RDONLY|O_NONBLOCK);
    if (fd < 0)
    {
        return -1;
    }
    
    while(bReadCount++ < 10)
    {
        read(fd, &data1, sizeof(data1));
        usleep(1000);
        
        read(fd, &data2, sizeof(data2));
        usleep(1000);
        
        read(fd, &data3, sizeof(data3));
        usleep(1000);
        
        read(fd, &data4, sizeof(data4));
        usleep(1000);
        
        read(fd, &data5, sizeof(data5));
        //连续5次读到相同的值，防止干扰
        if(data1 == data2 && data1 == data3 && data1 == data4 && data1 == data5)
        {
            type = ((data1 >> 8) & 0x0001);
            break;
        }
    }
    close(fd);

    return 1 == type ? 0 : 1;
}

/**********************************************************************
* @name      : check_power_status
* @brief     ：检查终端电源状态
* @return    ：1 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2020-07-27
* @Update    :
**********************************************************************/
int check_power_status()
{
	int vol = 0;
	static int count = 20;

    if( 0 == battery_insert_check())		//无电池
	{
        g_template_info.bBatFlag = 0;
        return 1;
	}

	if(count == 20)
	{
		set_battery_charge(0);	//断电
	
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

		set_battery_charge(1);	//充电

		if((vol > BATTERY_FULL) || 0 == battery_insert_check())		//无电池

		{
	        g_template_info.bBatFlag = 0;
		}
		else if(vol <BATTERY_LOW )		//电池充电
		{
	        g_template_info.bBatFlag = 1;
		}
		else	//电池正常
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

void Template1Init(DWORD dwPluginId)
{
	memset(g_template_icon.bSignal, 0, sizeof(int)*SIGNAL_ICON_COUNT);
	memset(g_template_icon.bProto, 0, sizeof(int)*PROTO_ICON_COUNT);
	memset(g_template_icon.bMeasure, 0, sizeof(int)*MEASURE_ICON_COUNT);
	memset(g_template_icon.bAlarm, 0, sizeof(int)*ALARM_ICON_COUNT);
	memset(g_template_icon.bArrow, 0, sizeof(int)*ARROW_ICON_COUNT);

	// 下面的图标不再load，而到需要的时候再获取
	/*
	// 获取所有的bmp文件到相应的结构中去
	char czBuf[200];
	int i;
	// 场强信号bitmap数组
	for(i = 0; i < SIGNAL_ICON_COUNT; i ++)
	{
		sprintf(czBuf, "%s%s", ICON_PATH, g_icon_signal[i]);
		LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapSignal[i], czBuf);
	}
	// 通信类型bitmap数组
	for(i = 0; i < PROTO_ICON_COUNT; i ++)
	{
		sprintf(czBuf, "%s%s", ICON_PATH, g_icon_proto[i]);
		LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapProto[i], czBuf);
	}
	// 测量点号bitmap数组
	for(i = 0; i < MEASURE_ICON_COUNT; i ++)
	{
		sprintf(czBuf, "%s%s", ICON_PATH, g_icon_measure[i]);
		LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapMeasure[i], czBuf);
	}
	// 告警信息bitmap数组
	for(i = 0; i < ALARM_ICON_COUNT; i ++)
	{
		sprintf(czBuf, "%s%s", ICON_PATH, g_icon_alarm[i]);
		LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapAlarm[i], czBuf);
	}*/


	// 电池电量
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
	// 释放所有的bmp
	// 通信类型bitmap数组
	for(i = 0; i < SIGNAL_ICON_COUNT; i ++)
	{
		if(g_template_icon.bSignal[i] != 0)
			UnloadBitmap(&g_template_icon.mapSignal[i]);
	}

    // 网络制式bitmap数组
	for(i = 0; i < NET_ICON_COUNT; i ++)
	{
		if(g_template_icon.bNet[i] != 0)
			UnloadBitmap(&g_template_icon.mapNet[i]);
	}
    
	// 通信类型bitmap数组
	for(i = 0; i < PROTO_ICON_COUNT; i ++)
	{
		if(g_template_icon.bProto[i] != 0)
			UnloadBitmap(&g_template_icon.mapProto[i]);
	}
	// 测量点号bitmap数组
	for(i = 0; i < MEASURE_ICON_COUNT; i ++)
	{
		if(g_template_icon.bMeasure[i] != 0)
			UnloadBitmap(&g_template_icon.mapMeasure[i]);
	}
	// 告警信息bitmap数组
	for(i = 0; i < ALARM_ICON_COUNT; i ++)
	{
		if(g_template_icon.bAlarm[i] != 0)
			UnloadBitmap(&g_template_icon.mapAlarm[i]);
	}

	// 箭头信息bitmap数组
	for(i = 0; i < ARROW_ICON_COUNT; i ++)
	{
		if(g_template_icon.bArrow[i] != 0)
			UnloadBitmap(&g_template_icon.mapArrow[i]);
	}

	// 电池电量
	for(i = 0; i < BATFLAG_ICON_COUNT; i ++)
	{
		if(g_template_icon.bBatFlag[i] != 0)
			UnloadBitmap(&g_template_icon.mapBatFlag[i]);
	}

	// 释放天线、保电图标
	UnloadBitmap(&g_template_info.mapConn);
	UnloadBitmap(&g_template_info.mapProtected);
    UnloadBitmap(&g_template_info.mapEsam);
	UnloadBitmap(&g_template_info.mapNet);
	UnloadBitmap(&g_template_info.mapBlue);
#if ANHUI_FUN == 1
	//释放正式密钥图标
	for(i=0;i < ESAM_ICON_COUNT;i++)
	{
		if(g_template_icon.bEsam[i] != 0)
			UnloadBitmap(&g_template_icon.mapEsam[i]);
	}
#endif
}

//运营商代码
const char* g_factory[] = { "移","联", "电","专","未" };
bool conStatusback = false;
void Template1Draw(HDC hdc)
{
    MoveTo(hdc, 0, 15);
    LineTo(hdc, 160, 15);

    MoveTo(hdc, 0, 144);
    LineTo(hdc, 160, 144);

    int nLeft = 0;
    int nTop = 0;
    char czBuf[300];

	GetStatusInfo_Desk();

    // 场强信号
    int nSignal = 0;
    nLeft = 1;
	if(g_template_info.nSignal > 0 && g_template_info.nSignal < 32)
    {
#ifdef AREA_CHONGQING
    if (g_template_info.nSignal%8 == 0 && g_template_info.nSignal/8 >= 1)
        nSignal = g_template_info.nSignal/8;
    else
        nSignal = g_template_info.nSignal/8 + 1;
        if(nSignal > 4) //容错
        {
            nSignal = 4;
        }
        if(nSignal < 1) //容错
        {
            nSignal = 1;
        }
        if(toolInfoFresh.channel1.netFormat > 3) //容错
        {
            toolInfoFresh.channel1.netFormat = 2;
        }
        if(toolInfoFresh.channel1.conStatus == TRUE)
        {
            char czBuf[200];
            if(g_template_icon.bSignal[nSignal] == 0)
            {
                sprintf(czBuf, "%s%s", ICON_PATH, g_icon_signal[nSignal]);
                LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapSignal[nSignal], czBuf);
                g_template_icon.bSignal[nSignal] = 1;
            }
            FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_info.mapConn);//显示信号塔
            SelectFont(hdc,g_guiComm.fontSmall);
            FillBoxWithBitmap(hdc, nLeft+10, nTop, 0, 0, &g_template_icon.mapSignal[nSignal]);
            TextOut(hdc, nLeft+15, nTop, "G");
        }
        else
        {
            if(g_template_icon.bSignal[nSignal] == 0)
            {
                char czBuf[200];
                sprintf(czBuf, "%s%s", ICON_PATH, g_icon_signal[nSignal]);
                LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapSignal[nSignal], czBuf);
                g_template_icon.bSignal[nSignal] = 1;
            }
            SelectFont(hdc,g_guiComm.fontSmall);
            FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_info.mapConn);//显示信号塔
            FillBoxWithBitmap(hdc, nLeft+10, nTop, 0, 0, &g_template_icon.mapSignal[nSignal]);
        }
#else
        nSignal = g_template_info.nSignal/8;

        if(nSignal > 4) //容错
        {
            nSignal = 4;
        }

        if(nSignal < 1) //容错
        {
            nSignal = 1;
        }

        if(toolInfoFresh.channel1.netFormat > 3) //容错
        {
            toolInfoFresh.channel1.netFormat = 2;
        }
        if(toolInfoFresh.channel1.conStatus == TRUE ||g_template_info.bConn == TRUE)
        {
            nSignal +=toolInfoFresh.channel1.netFormat*4;
            if(g_template_icon.bSignal[nSignal] == 0)
            {
                char czBuf[200];
                sprintf(czBuf, "%s%s", ICON_PATH, g_icon_signal[nSignal]);
                LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapSignal[nSignal], czBuf);
                g_template_icon.bSignal[nSignal] = 1;
            }
            FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_icon.mapSignal[nSignal]);
        }
#endif
    }
    else 
    {
    	FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_info.mapConn);	//显示信号塔
    }

	nLeft += 28;
    //以太网连接
    if (conStatusback != toolInfoFresh.channel1.conStatus || toolInfoFresh.channel1.conStatus == false)
    {
        conStatusback = toolInfoFresh.channel1.conStatus;
        uint8 sendbuf[1] = {1};
        gui_send_wirelessmsg(0, WIRELESS_IOP_GET_DIAGINFO, sendbuf, 1);
    }
#ifdef AREA_CHONGQING
    SelectFont(hdc,g_guiComm.fontSmall);
    if(toolInfoFresh.channel1.conStatus)
    {
        char czBuf[200];
        bzero(czBuf,sizeof(czBuf));
        if(DiagInfoDisp.operat > 3 || DiagInfoDisp.operat < 0){
            DiagInfoDisp.operat = 4;
        }
        if(0 < toolInfoFresh.channel1.signalStrengh && toolInfoFresh.channel1.signalStrengh < 33)
        {
            sprintf(czBuf, "%d%s%dG", toolInfoFresh.channel1.signalStrengh, g_factory[DiagInfoDisp.operat] , toolInfoFresh.channel1.netFormat+2);
            TextOut(hdc, nLeft, nTop, czBuf);
        }
    }
    else if (toolInfoFresh.ethConStatus == TRUE)
    {
        FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_info.mapNet);
    }
    else if(g_template_info.bConn == TRUE)
    {
        char czBuf[200];
        bzero(czBuf,sizeof(czBuf));
        if(DiagInfoDisp.operat > 3 || DiagInfoDisp.operat < 0){
            DiagInfoDisp.operat = 4;
        }
        if(0 < toolInfoFresh.channel1.signalStrengh && toolInfoFresh.channel1.signalStrengh < 33)
        {
            sprintf(czBuf, "%d%s%dG", toolInfoFresh.channel1.signalStrengh, g_factory[DiagInfoDisp.operat] , toolInfoFresh.channel1.netFormat+2);
            TextOut(hdc, nLeft, nTop, czBuf);
        }
    }
    nLeft += 18;
    if(toolInfoFresh.warning == 1 || toolInfoFresh.keyStatus == 0)
    {
        if(g_template_info.bShowAlarm == TRUE)
        {
            if(g_template_icon.bAlarm[0] == 0)
            {
                char czBuf[200];
                sprintf(czBuf, "%s%s", ICON_PATH, g_icon_alarm[0]);
                LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapAlarm[0], czBuf);
                g_template_icon.bAlarm[0] = 1;
            }
            FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_icon.mapAlarm[0]);
        }
    }
    ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontBig);

    // 设置1-16的测量点信息
    nLeft += 36;
    if(toolInfoFresh.infoID >= 0 && toolInfoFresh.infoID <= 2040)//会覆盖之前事件显示
    { 
        MoveTo(hdc, nLeft, 0);
        LineTo(hdc, nLeft, 14);
        

        char czBuf[200];
        sprintf(czBuf,"%04d",toolInfoFresh.infoID);
        TextOut(hdc, nLeft+1, -1, czBuf);

        MoveTo(hdc, nLeft+33, 0);
        LineTo(hdc, nLeft+33, 14);
    }

    if(g_template_info.bBat == TRUE)
    {
        if(g_template_icon.bBatFlag[g_template_info.bBatFlag] == 0)
        {
            char czBuf[200];
            sprintf(czBuf, "%s%s", ICON_PATH, g_icon_Batflag[g_template_info.bBatFlag]);
            LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapBatFlag[g_template_info.bBatFlag], czBuf);
            g_template_icon.bBatFlag[g_template_info.bBatFlag] = 1;
        }
        FillBoxWithBitmap(hdc,135,0,16,12,&g_template_icon.mapBatFlag[g_template_info.bBatFlag]);
    }
    else
    {
        // 画时间信息
        memset(czBuf, 0x00, sizeof(czBuf));
        time_t t;
        time(&t);
        tm *p = localtime(&t);
        sprintf(czBuf, "%02d:%02d", p->tm_hour, p->tm_min);
        TextOut(hdc, 119, -1, czBuf);
    }

    // 画状态信息
    SelectFont(hdc,g_guiComm.fontSmall);

    for(int i = 0; i < 10; i ++)
    {
        if(g_bGetStatus == FALSE)
        {
            g_bGetStatus = TRUE;
            strcpy(czBuf, g_template_info.czStatusInfo);
            g_bGetStatus = FALSE;
            break;
        }
        usleep(10000);
    }
    TextOut(hdc, 2, 147, "                     ");//覆盖掉之前显示的文字
//    TextOut(hdc, 2, 147, "终端正在抄表...");//覆盖掉之前显示的文字
    if(strlen(czBuf) != 0)
    {
//        TextOut(hdc, 2, 147, "                     ");//覆盖掉之前显示的文字
        TextOut(hdc, 2, 147, czBuf);
    }
#else
#ifdef AREA_JIANGXI
    if(toolInfoFresh.channel1.conStatus == TRUE)
    {
        char czBuf[200];
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf, "%s%s", ICON_PATH, g_icon_netFormat[toolInfoFresh.channel1.netFormat]);
        LoadBitmapFromFile(HDC_SCREEN, &g_template_info.wireNet, czBuf);
        FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_info.wireNet);
    }
    else if (toolInfoFresh.ethConStatus == TRUE)
    {
        FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_info.mapNet);
    } 
    else if(g_template_info.bConn == TRUE)
    {
        char czBuf[200];
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf, "%s%s", ICON_PATH, g_icon_netFormat[toolInfoFresh.channel1.netFormat]);
        LoadBitmapFromFile(HDC_SCREEN, &g_template_info.wireNet, czBuf);
        FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_info.wireNet);
    }
#elif defined AREA_JIBEI
    if(DiagInfoDisp.operat > 3 || DiagInfoDisp.operat < 0){
        DiagInfoDisp.operat = 4;
    }
    if(toolInfoFresh.channel1.conStatus == TRUE)
    {
        char czBuf[200];
        bzero(czBuf,sizeof(czBuf));
        if(toolInfoFresh.channel1.netFormat == 0){
            if(DiagInfoDisp.operat == 2){
                sprintf(czBuf, "%s%s", ICON_PATH, g_icon_netFormat[4]);
            }else{
                sprintf(czBuf, "%s%s", ICON_PATH, g_icon_netFormat[5]);
            }
        }else{
            sprintf(czBuf, "%s%s", ICON_PATH, g_icon_netFormat[toolInfoFresh.channel1.netFormat]);
        }
        LoadBitmapFromFile(HDC_SCREEN, &g_template_info.wireNet, czBuf);
        FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_info.wireNet);
    }
    else if (toolInfoFresh.ethConStatus == TRUE)
    {
        FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_info.mapNet);
    } 
    else if(g_template_info.bConn == TRUE)
    {
        char czBuf[200];
        bzero(czBuf,sizeof(czBuf));
        if(toolInfoFresh.channel1.netFormat == 0){
            if(DiagInfoDisp.operat == 2){
                sprintf(czBuf, "%s%s", ICON_PATH, g_icon_netFormat[4]);
            }else{
                sprintf(czBuf, "%s%s", ICON_PATH, g_icon_netFormat[5]);
            }
        }else{
            sprintf(czBuf, "%s%s", ICON_PATH, g_icon_netFormat[toolInfoFresh.channel1.netFormat]);
        }
        LoadBitmapFromFile(HDC_SCREEN, &g_template_info.wireNet, czBuf);
        FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_info.wireNet);
    }
#elif defined AREA_SICHUAN
    SelectFont(hdc,g_guiComm.fontSmall);
    if(DiagInfoDisp.operat > 3 || DiagInfoDisp.operat < 0){
        DiagInfoDisp.operat = 4;
    }

    if(toolInfoFresh.channel1.conStatus == TRUE)
    {
        char czBuf[200];
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf, "%s", g_factory[DiagInfoDisp.operat]);
        TextOut(hdc, nLeft, nTop, czBuf);
    }
    else if (toolInfoFresh.ethConStatus == TRUE)
    {
        FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_info.mapNet);
    } 
    else if(g_template_info.bConn == TRUE)
    {
        char czBuf[200];
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf, "%s", g_factory[DiagInfoDisp.operat]);
        TextOut(hdc, nLeft, nTop, czBuf);
    }
#else
    if(toolInfoFresh.ethConStatus == TRUE || g_template_info.bNet == TRUE)
    {
        FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_info.mapNet); 
    }
#endif
    if(toolInfoFresh.warning == 1 || toolInfoFresh.keyStatus == 0)
    {   
        nLeft += 18;
        
        if(g_template_info.bShowAlarm == TRUE)
        {
            if(g_template_icon.bAlarm[0] == 0)
            {
                char czBuf[200];
                sprintf(czBuf, "%s%s", ICON_PATH, g_icon_alarm[0]);
                LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapAlarm[0], czBuf);
                g_template_icon.bAlarm[0] = 1;
            }
            FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_icon.mapAlarm[0]);
        }

        if(g_template_info.bEsam == TRUE)
        {
            FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_info.mapEsam);
        }
            
    }

	// 蓝牙
    nLeft += 18;
    if(toolInfoFresh.blueStatus == 2 || g_template_info.bBlue == TRUE)
    { 
       FillBoxWithBitmap(hdc, nLeft, nTop, 0, 0, &g_template_info.mapBlue);
    }
	
    ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontBig);

    // 设置1-16的测量点信息
    nLeft += 18;
    if(toolInfoFresh.infoID >= 0 && toolInfoFresh.infoID <= 2040)//会覆盖之前事件显示
    { 
        MoveTo(hdc, nLeft, 0);
        LineTo(hdc, nLeft, 14);
        

        char czBuf[200];
        sprintf(czBuf,"%04d",toolInfoFresh.infoID);
        TextOut(hdc, nLeft+1, -1, czBuf);

        MoveTo(hdc, nLeft+33, 0);
        LineTo(hdc, nLeft+33, 14);
    }

	if(g_template_info.bBat == TRUE)
    {
        if(g_template_icon.bBatFlag[g_template_info.bBatFlag] == 0)
        {
            char czBuf[200];
            sprintf(czBuf, "%s%s", ICON_PATH, g_icon_Batflag[g_template_info.bBatFlag]);
            LoadBitmapFromFile(HDC_SCREEN, &g_template_icon.mapBatFlag[g_template_info.bBatFlag], czBuf);
            g_template_icon.bBatFlag[g_template_info.bBatFlag] = 1;
        }
        FillBoxWithBitmap(hdc,135,0,16,12,&g_template_icon.mapBatFlag[g_template_info.bBatFlag]);
    }
	else
	{
		// 画时间信息
	    memset(czBuf, 0x00, sizeof(czBuf));
	    time_t t;
	    time(&t);
	    tm *p = localtime(&t);
	    sprintf(czBuf, "%02d:%02d", p->tm_hour, p->tm_min);
	    TextOut(hdc, 119, -1, czBuf);
	}
    

    // 画状态信息
    SelectFont(hdc,g_guiComm.fontSmall);

    for(int i = 0; i < 10; i ++)
    {
        if(g_bGetStatus == FALSE)
        {
            g_bGetStatus = TRUE;
            strcpy(czBuf, g_template_info.czStatusInfo);
            g_bGetStatus = FALSE;
            break;
        }
        usleep(10000);
    }
	TextOut(hdc, 2, 147, "                     ");//覆盖掉之前显示的文字
//    TextOut(hdc, 2, 147, "终端正在抄表...");//覆盖掉之前显示的文字
    if(strlen(czBuf) != 0)
    {
//        TextOut(hdc, 2, 147, "                     ");//覆盖掉之前显示的文字
        TextOut(hdc, 2, 147, czBuf);
    }
#endif
}

void Template1Timer(int nId)
{
	if(nId == TIMER_MAIN)
	{
		if(toolInfoFresh.warning == 1 && toolInfoFresh.keyStatus == 0)
		{// 如果告警和esam都需要闪烁，则两个交替闪烁
		    g_template_info.bEsam = g_template_info.bShowAlarm;
			g_template_info.bShowAlarm = !g_template_info.bShowAlarm;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}
		else if(toolInfoFresh.warning == 1)
		{// 如果已经告警，闪烁
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
		check_power_status();
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
	}
}
PLUGIN_INFO g_Template1 = {
	0xFFFFFF/*id*/,
	Template1Init/*InitLayer*/,
	Template1Uninit/*UninitLayer*/,
	Template1Draw/*Draw*/,
	Template1Timer/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};

//常显页面
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
	sprintf(czBuf, "能源控制器");
	DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    rc.top += tmpTop;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "(%s)", ptype);
	DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    // 读取当前时间
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


//#ifdef AREA_JIANGXI
//void pluginDrawZCU(HDC hdc)
//{
//	SelectFont(hdc, g_guiComm.fontBig);
//	ITEM_NOT_SELECT;
//
//	char czBuf[64] = { 0 };
//
//	int tmpTop = 22;
//
//	RECT rc;
//	rc.left = 0;
//	rc.right = g_guiComm.nWidth;
//	rc.top = 48;
//	rc.bottom = rc.top + 16;
//	sprintf(czBuf, "国网");
//	DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
//
//
//    rc.top += tmpTop;
//    rc.bottom = rc.top + 16;
//    sprintf(czBuf, "江西省电力有限公司");
//	DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
//
//}
//#else
void pluginDrawZCU(HDC hdc)
{
	SelectFont(hdc, g_guiComm.fontBig);
	ITEM_NOT_SELECT;

	char czBuf[64] = { 0 };

	int tmpTop = 22;

	RECT rc;
	rc.left = 0;
	rc.right = g_guiComm.nWidth;
	rc.top = 48;
	rc.bottom = rc.top + 16;
#ifdef AREA_CHONGQING
	sprintf(czBuf, "低压集抄集中器");
#else
	sprintf(czBuf, "专变采集终端Ⅲ型");
#endif
	DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    // 读取当前时间
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
//#endif

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
	"打开串口",//0
	"检测通信模块",//1
	"检测SIM卡",//2
	"网络注册",//3
	"获取信号",//4
	"读取通信模块型号",//5
	"设置APN",//6
	"检测gprs网络",//7
	"开始拨号",//8
	"PPP验证",//9
	"正在连接服务器",//10
	"设置短信内容格式",//11
	"设置短信存储模式",//12
	"设置来电显示",//13
	"LCP链路协商",//14
	"IPCP协商",//15
	"连接服务器成功",//16

	//#if COMM_RETRY == 1	不要删掉，为了移植的时候能够搜索到该标识
	"发送登录报文",		// 17
	"发送心跳报文",		// 18
	"发送退出报文",		// 19
	"收到登录确认",		// 20
	"收到心跳确认",		// 21
	"收到退出确认",		// 22

	//#if WDASN_RADIO_EN == 1	不要删掉，为了移植的时候能够搜索到该标识
	"扫描当前信道",		//23
	"附着申请",			//24
	"连接申请",			//25
	"连接成功",			//26
	"设置行政区码和地址",	//27
	"启动连接",			//28
	"连接等待",			//29
	"设置登陆次数和间隔",	//30

	//#if ETH_MODEL == 1		不要删掉，为了移植的时候能够搜索到该标识
	"关闭以太模块",		// 31
	"启动以太模块",		// 32
	"初始化以太模块",	// 33
	"设置终端IP地址",	// 34
	"设置终端网关",		// 35

	////#if TD_SCDMA == 1		//TD-SCDMA功能
	"关闭网络信号分贝值的上报",	//36//TD_AT_DSQ_0
	"关闭网络上报",				//TD_AT_CREG_0
	"激活USIM卡",					//TD_AT_CFUN_5
	"激活协议栈",					//TD_AT_CFUN_1
	"自动附着GPRS",				//TD_AT_DCTA
	"搜索网络",						//TD_AT_COPS
	"设置PS业务",					//TD_AT_CGEQREQ
	"关闭MT自动应答",				//TD_AT_ATS0
	"测试命令",					//TD_AT_TEST
	"设置为自动TD模式",			//AT_TD_DDTM_TD45
	"设置为自动GPRS模式",		//AT_TD_DDTM_GPRS46
	"检测模块运行模式",			//47

	//#if WORK_MODE == 1
	"等待客户端连接",			//48//等待客户端连接

	"打开串口",//49
	"检测SIM卡",//50
	"设置主动上报",//51
	"网络注册",//52
	"获取信号",//53
	"读取通信模块型号",//54
	"设置APN",//55
	"连接服务器",//56
	"获取终端注册的IP地址",//57
	"设置服务参数",//58
	"检测gprs网络",//59
	"连接服务器成功",//60
	"连接结束",//61
};
int g_nCchStateCnt = 62;

const char* g_czCchErr[] =
{
	"打开串口失败",//0
	"通信模块异常",//1
	"SIM卡异常",//2
	"注册网络失败",//3
	"信号弱",//4
	"读取通信模块型号失败",//5
	"APN设置失败",//6
	"无gprs网络",//7
	"拨号不成功",//8
	"PPP验证失败",//9
	"连接服务器失败",//10
	"设置短信内容格式失败",//11
	"设置短信存储模式失败",//12
	"设置来电显示失败",//13
	"LCP链路协商失败",//14
	"IPCP协商失败",//15
	//#if WDASN_RADIO_EN == 1	不要删掉，为了移植的时候能够搜索到该标识
	"连接失败",//16
	//#if ETH_MODEL == 1		不要删掉，为了移植的时候能够搜索到该标识
	"初始化以太模块失败",//17
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
	"打开串口失败",//49
	"SIM卡异常",//50
	"通信模块异常",//51
	"注册网络失败",//52
	"信号弱",//53
	"读取通信模块型号失败",//54
	"APN设置失败",//55
	"连接服务器失败",//56
	"获取终端IP地址失败",//57
	"设置服务参数失败",//58
	"无gprs网络",//59
	"连接失败",//60
	"初始化以太模块失败",//61
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
			sprintf(g_template_info.czStatusInfoTmp, "%s%s", pInfo, (czVal==1)?"投入":"解除");
			g_template_info.nCurState = nIndex;
			g_template_info.bUpdate = TRUE;
		}
	}
	else if(g_template_info.bUpdate == FALSE && g_template_info.nCurState == nIndex)
	{
		sprintf(g_template_info.czStatusInfoTmp, "%s%s", pInfo, (czVal==1)?"投入":"解除");
	}
}

void GetNewStatusInfo()
{
	//int nRet= -1;
	return ;
}
// 桌面消息
void GetStatusInfo_Desk()
{
	g_template_info.nState = toolInfoFresh.ethConStatus;//以太网状态

	g_template_info.nSignal = toolInfoFresh.channel1.signalStrengh;
	if(toolInfoFresh.channel1.netFormat == 1)
		g_template_info.nProto = (g_template_info.nState==0)?20:21;
	else if(toolInfoFresh.channel1.netFormat == 2)
		g_template_info.nProto = (g_template_info.nState==0)?22:23;

	g_template_info.nSignal2 = toolInfoFresh.channel2.signalStrengh;

//	if(toolInfoFresh.ethConStatus == 0)
//	{
//		g_template_info.nState = toolInfoFresh.ethConStatus;
//		g_template_info.nProto = (g_template_info.nState==0)?16:17;
//		g_template_info.nSignal = -1;
//	}

	strcpy(g_template_info.czStatusInfoTmp, "");
	if(g_template_info.nState != 0)
	{
		//GetCchInfo(g_template_info.nState);
		if(g_template_info.bLogin != FALSE)
		{
			g_template_info.bLogin = FALSE;
			g_template_info.bShowLogin = TRUE;
		}
	}
	else
	{
		g_template_info.bLogin = TRUE;
	}

	g_template_info.bShowAlarm = toolInfoFresh.warning;
	if(g_template_info.bShowAlarm)
	{
		g_template_info.nAlarm = 0;
	}

//	if(g_template_info.bLogin == TRUE && g_PPPDiagFlag[0] == TRUE)
//		memset(display_statu_buf, 0, sizeof(display_statu_buf));

	if(g_bGetStatus == FALSE)
    {
        g_bGetStatus = TRUE;// 用该标识的话，基本应该不会产生同时操作的问题了
        usleep(20000);// 再等一会，怎么也不会在strcpy中了吧
        if(statusInfoFresh.dispEnd == 0 || time(NULL) < statusInfoFresh.dispEnd)
        {
        	strcpy(g_template_info.czStatusInfo, statusInfoFresh.text);
        }
		else
		{
			memset(g_template_info.czStatusInfo,0,sizeof(g_template_info.czStatusInfo));
		}
        
        g_bGetStatus = FALSE;
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
