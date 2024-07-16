#ifdef AREA_CHONGQING
//重庆集中器的界面绘制

#include <sys/stat.h>
#include <sys/statfs.h>
#include "miniguidef.h"
#include "commdef.h"
#include "main.h"
#include "commfun.h"
#include "iniparser.h"
#include "appmsg_split.h"
#include "out_msg.h"
#include "fileOp.h"
#include "hal.h"
#include "power.h"
#include "adc.h"
#include "gui_prtl.h"
#include "macro.h"
#include "chongqing.h"
#include "oopType.h"
#include "desk_meter.h"

extern void CancelMenu();
static int DaysOfMonth(int year, int month);
static BOOL IsLeap(int year);
extern void appdb_meter_list_record_init(int clientid);
extern int read_db_noamal_data(DB_CLIENT fp, uint32 oad_value, uint8 logic_id, uint16 info_num, uint8* pbuf, uint32 buf_len);
//extern OOP_METER g_gruop_oopMeter;
extern DESK_METER_LIST_T gDeskMeter;
extern void task_status_init(DWORD dwPluginId);
extern int get_task_status(uint16 id);
extern void taskStatusIdReturn(char* pNewStr, void* pPara);
//-查讯当前所属主菜单序号-
static unsigned GetPrimaryMenuIndex(void)
{
    extern int g_narrMenuIndex[20];//引用menulayer.cpp中的全局变量
    return (g_narrMenuIndex[0]+1);
}

//-查讯当前所属次级菜单序号-
static unsigned GetSecondaryMenuIndex(void)
{
    extern int g_narrMenuIndex[20];//引用menulayer.cpp中的全局变量
    return (g_narrMenuIndex[1]+1);
}

SmallDate   g_date=  //-抄表查询日期(全局变量,由页面1-1读写,由页面1-2至1-6读取)-
{
    2000,1,1                 //-初始状态=2000.01.01-
};
static int g_cursor=0;   //-当前光标所处位置(0=年,1=月,2=日)-

static void CQSelectDateInit(DWORD dwPluginID)
{
    time_t        t;
    struct tm    datetime;

    //-函数CQSelectDateInit()只被执行一次-
    //-第二次进入g_CQSelectDate页面时不再通过pfnInitLayer调用,即g_date不再被初始化-
    g_guiMain.pluginCur->pfnInitLayer = NULL;

    //-读当前日期-
    time(&t);
    localtime_r(&t, &datetime);
    g_date.year  = datetime.tm_year+1900;
    g_date.month = datetime.tm_mon+1;
    g_date.day   = datetime.tm_mday;

    //-初始化光标所在位置-
    g_cursor=0;
}

static void CQSelectDateDraw(HDC hdc)
{
    const int nLeft1 = 10;
    const int nLeft2 = 40;
    int x,y;
    char str[100];
    //char buf[100];
    //int i;
    //mtrnum_t meter;

    SelectFont(hdc,g_guiComm.fontSmall);
    y = 20;
    bzero(str,sizeof(str));
    snprintf(str, sizeof(str), "%u-%u %s", //页面标题
        GetPrimaryMenuIndex(),GetSecondaryMenuIndex(),
        g_guiMain.pPrentMenu->pPlugStruct->czCaption);
    TextOut(hdc, nLeft1, y, str);
    y += 32;
    TextOut(hdc, nLeft2, y, "请选择查询日期:");

    y = 70;
    x = 40;
    bzero(str,sizeof(str));
    snprintf(str, sizeof(str), "%4d", g_date.year);
    if (g_cursor==0) {ITEM_SELECT;}
    TextOut(hdc, x, y, str);
    if (g_cursor==0) {ITEM_NOT_SELECT;}
    x += 6*strlen(str);

    TextOut(hdc, x, y, "-");
    x += 6;
    bzero(str,sizeof(str));
    snprintf(str, sizeof(str), "%02d", g_date.month);
    if (g_cursor==1) {ITEM_SELECT;}
    TextOut(hdc, x, y, str);
    if (g_cursor==1) {ITEM_NOT_SELECT;}
    x += 6*strlen(str);

    TextOut(hdc, x, y, "-");
    x += 6;
    bzero(str,sizeof(str));
    snprintf(str, sizeof(str), "%02d", g_date.day);
    if (g_cursor==2) {ITEM_SELECT;}
    TextOut(hdc, x, y, str);
    if (g_cursor==2) {ITEM_NOT_SELECT;}
}

static BOOL CQSelectDateKeyMsg(int nKey)
{
    switch (nKey)
    {
    case KEY_RIGHT:
        g_cursor++;
        if (g_cursor>2)
            g_cursor=2;
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        break;
    case KEY_LEFT:
        g_cursor--;
        if (g_cursor<0)
            g_cursor=0;
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        break;
    case KEY_UP:
        switch (g_cursor)
        {
            case 0:
                g_date.year++;
                if (g_date.year > 2039)
                {    g_date.year = 1940;
                }
                g_date.day = MIN(g_date.day, DaysOfMonth(g_date.year, g_date.month));    //-更改年份或月份时自动检查相应的31,30,29日是否存在,若不存在则作调整-
                break;                                                                    //-例:当前为2000-2-29,更改年份时应自动将day变为28,即变为1999-2-28或2001-2-28-
                                                                                        //-(下同)-
            case 1:
                g_date.month++;
                if (g_date.month > 12)
                {    g_date.month = 1;
                }
                g_date.day = MIN(g_date.day, DaysOfMonth(g_date.year, g_date.month));
                break;

            case 2:
                g_date.day++;
                if (g_date.day > DaysOfMonth(g_date.year, g_date.month))
                {    g_date.day = 1;
                }
                break;

            default:
                break;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        break;
    case KEY_DOWN:
        switch (g_cursor)
        {
            case 0:
                g_date.year--;
                if (g_date.year < 1940)
                    g_date.year = 2039;
                g_date.day = MIN(g_date.day, DaysOfMonth(g_date.year, g_date.month));
                break;

            case 1:
                g_date.month--;
                if (g_date.month < 1)
                    g_date.month = 12;
                g_date.day = MIN(g_date.day, DaysOfMonth(g_date.year, g_date.month));
                break;

            case 2:
                g_date.day--;
                if (g_date.day < 1)
                {    
                    g_date.day = DaysOfMonth(g_date.year, g_date.month);
                }
                break;

            default:
                break;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        break;
    default:
        return (FALSE);
        break;
    }
    return (TRUE);
}

PLUGIN_INFO g_CQSelectDate=
{
    0x00150000,
    CQSelectDateInit,
    NULL,
    CQSelectDateDraw,
    NULL,
    CQSelectDateKeyMsg,
    NULL,
};

extern uint16 tsakID;
extern OOP_ACQ_MONITOR_T g_acq;
void MeterReadStatInit(DWORD dwPluginId)
{
    //appdb_meter_list_record_init(g_hUdbCenter);
}

void MeterReadStatDraw(HDC hdc)
{
    int nLeft1 = 20;
    int nLeft2 = 80;
    int nTop = 20;
    char czBuf[100];
    //int i;

    SelectFont(hdc,g_guiComm.fontSmall);
    bzero(czBuf,sizeof(czBuf));
    snprintf(czBuf, sizeof(czBuf), "%u-%u %s", //页面标题
        GetPrimaryMenuIndex(),GetSecondaryMenuIndex(),
        g_guiMain.pPrentMenu->pPlugStruct->czCaption);
    TextOut(hdc, 10, nTop, czBuf);

    nTop+=20; 
    TextOut(hdc, nLeft1, nTop, "任务ID:");
    sprintf(czBuf, "%hu", tsakID);
    ITEM_SELECT;
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;

    nTop+=20; 
    bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf,"电能表总数 %d",g_acq.sum);
    TextOut(hdc, nLeft1, nTop, czBuf);

     nTop+=20; 
    bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf,"抄表成功数 %d",g_acq.success);
    TextOut(hdc, nLeft1, nTop, czBuf);

    nTop+=20; 
    bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf,"抄表失败数 %d",g_acq.sum - g_acq.success);
    TextOut(hdc, nLeft1, nTop, czBuf);
}

BOOL MeterReadStatMsgKey(int nKey)
{
    switch (nKey)
    {
    case KEY_UP:
    case KEY_LEFT:
        if (tsakID == 1)
        {
            tsakID = 255;
        }
        else
        {
            tsakID--;
        }
        get_task_status(tsakID);
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_DOWN:
    case KEY_RIGHT:
        if (tsakID == 255)
        {
            tsakID = 1;
        }
        else
        {
            tsakID++;
        }
        get_task_status(tsakID);
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
        
    case KEY_OK:
        char bzbuf[50];
        sprintf(bzbuf, "%d", tsakID);
        InputBox(bzbuf, taskStatusIdReturn, NULL, 0x01);
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }

    return FALSE;
}
PLUGIN_INFO g_MeterReadStat= {
    0x0040000c/*id*/, 
    task_status_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    MeterReadStatDraw/*Draw*/,
    NULL/*Timer*/,
    MeterReadStatMsgKey/*keymsg*/,
    NULL/*ipc*/
};

//-查询某年某月所含天数,用于检查该月有无31,30,29日-
static int DaysOfMonth(int year, int month)
{
    switch (month)
    {
    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
        return (31);
        break;
    case 4: case 6: case 9: case 11:
        return (30);
        break;
    case 2:
        return (IsLeap(year)? 29:28);
        break;
    default:  //输入的月份参数超出范围-
        return (0);
    }    
}

//-检测闰年-
static BOOL IsLeap(int year)
{
    return __isleap(year);
}

OOP_ROUTEMODULE_T hplc_datalist;
ROUTEMODULE hplc_data;
uint16 hplcSum;
uint16 netsize = 0;
void CQModuleStatInit(DWORD dwPluginId)
{
    NOMAL_OAD_T normal = {0};
    memset(&hplc_data, 0, sizeof(hplc_data));
    memset(&hplc_datalist, 0, sizeof(hplc_datalist));
    normal.oad.value = 0xF2090200;
    uint32 len = 0;
    int i = 0;
    int ret = ERR_OK;

    for (i = 0; i < OOP_MAX_PLC; i++)
    {
        normal.infoNum = i + 1;
        ret = db_read_nomal(g_hUdbCenter, &normal, sizeof(ROUTEMODULE), (uint8*)&hplc_data, &len);
        if((ret != ERR_OK) || (len == 0))
        {
            GUI_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
            continue;
        }
        else
        {
            hplc_datalist.item[hplc_datalist.nNum] = hplc_data;
            hplc_datalist.nNum ++;
        }
    }
    hplcSum = hplc_info_read_from_meter();
    memset(&normal, 0, sizeof(normal));
    normal.oad.value = 0xF2091400;
    normal.infoNum = 1;
    db_read_nomal(g_hUdbCenter, &normal, sizeof(uint16), (uint8*)&netsize, &len);
}

void CQModuleStateDraw(HDC hdc)
{
    int nLeft0 = 10;
    int nLeft1 = 95;
    int nTop = 20;
    char pbuf[100];
    char zbuf[100];

    SelectFont(hdc,g_guiComm.fontSmall);
    bzero(pbuf,sizeof(pbuf));
    snprintf(pbuf, sizeof(pbuf), "%u-%u %s", //页面标题
        GetPrimaryMenuIndex(),GetSecondaryMenuIndex(),
        g_guiMain.pPrentMenu->pPlugStruct->czCaption);
    TextOut(hdc, nLeft0, nTop, pbuf);
    
    nTop+=20;
    TextOut(hdc, nLeft0, nTop, "本地模块厂家:");
    bzero(pbuf,sizeof(pbuf));
    if(hplc_datalist.nNum == 0){
        TextOut(hdc, nLeft1, nTop, "-----");
    }
    else
    {
        sprintf(pbuf,"%s",hplc_datalist.item[0].verInfo.factory.value);
        TextOut(hdc, nLeft1, nTop, pbuf);
    }

    nTop+=20;
    bzero(pbuf,sizeof(pbuf));
    sprintf(pbuf,"总共节点数: %d", hplcSum);
    TextOut(hdc, nLeft0, nTop, pbuf); 

    nTop+=20;
    bzero(pbuf,sizeof(pbuf));
    bzero(zbuf,sizeof(zbuf));
    if (netsize >= 1)
    {
        sprintf(pbuf,"入网节点数: %d", netsize -1);
        if(hplcSum >= (netsize -1))
            sprintf(zbuf,"故障节点数: %d", hplcSum - netsize + 1);
        else
            sprintf(zbuf,"故障节点数: %d", 0);
    }
    else
    {
        sprintf(pbuf,"入网节点数: %d", 0);
        sprintf(zbuf,"故障节点数: %d", hplcSum);
    }
    TextOut(hdc, nLeft0, nTop, pbuf);

    nTop+=20;
    TextOut(hdc, nLeft0, nTop, zbuf);

}

BOOL CQModuleStateKeyMsg(int nKey)
{
    PLUGIN_INFO* pPlugin;
    switch(nKey)
    {
    case KEY_OK:
        pPlugin = GetPlugIn(0x0041000c);
        if(pPlugin == NULL)
        {
            ASSERT(FALSE);
            return FALSE;
        }
        g_guiMain.pluginCur = pPlugin;
        if(g_guiMain.pluginCur->pfnInitLayer != NULL)
        {
            g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        break;
    default:
        return FALSE;
        break;
    }
    UpdateWindow(g_guiComm.hMainWnd, TRUE);
    return TRUE;
}

PLUGIN_INFO g_CQModuleStat= {
    0x0045000c, 
    CQModuleStatInit,
    NULL,
    CQModuleStateDraw,
    NULL,
    NULL,
    NULL,
};


/************************************************************************/
/* 重庆本地模块组网状态查询*/
/*****************  结束  ***********************************************/
/************************************************************************/    

OOP_DWORD2_T gprsNum;
int gprsNumFlag;
GPRS_DATA_INFO signalInfo;

void GprsNumStatInit(DWORD dwPluginId)
{
    uint16 maxTime = 0;
    uint16 minTime = 0;
    memset(&gprsNum, 0, sizeof(OOP_DWORD2_T));
    memset(&signalInfo, 0xff, sizeof(GPRS_DATA_INFO));
    gprsNumFlag = read_db_noamal_data(g_hUdbCenter, 0x22000200, 0, 0, (uint8*)&gprsNum, sizeof(OOP_DWORD2_T));
    dictionary *ini = NULL;
    ini = iniparser_load("/data/app/desktopGui/data/SignalInfo.ini");
    if(ini!=NULL)
    {
        signalInfo.maxSig = iniparser_getint(ini,"SIGNAL_INFO:MAX_SIG",0xFFFF);
        signalInfo.minSig = iniparser_getint(ini,"SIGNAL_INFO:MIN_SIG",0xFFFF);
        maxTime = iniparser_getint(ini,"SIGNAL_INFO:MAX_TIME",0xFFFF);
        minTime = iniparser_getint(ini,"SIGNAL_INFO:MIN_TIME",0xFFFF);
        iniparser_freedict(ini);
    }
    signalInfo.maxTime.hour = maxTime/60;
    signalInfo.maxTime.minute = maxTime%60;
    signalInfo.minTime.hour = minTime/60;
    signalInfo.minTime.minute = minTime%60;
}

void GprsNumStatDraw(HDC hdc)
{
    int nLeft1 = 20;
    int nLeft2 = 100;
    int nTop = 20;
    char czBuf[100];
    char timeBuf[100];

    SelectFont(hdc,g_guiComm.fontSmall);
    bzero(czBuf,sizeof(czBuf));
    snprintf(czBuf, sizeof(czBuf), "%u-%u %s", //页面标题
        GetPrimaryMenuIndex(),GetSecondaryMenuIndex(),
        g_guiMain.pPrentMenu->pPlugStruct->czCaption);
    TextOut(hdc, 10, nTop, czBuf);
    nTop+=20; 
    TextOut(hdc, nLeft1, nTop, "日通信流量:");

    if (gprsNumFlag < 0)
    {
        TextOut(hdc, nLeft2, nTop, "-- Kb");
    }
    else{
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf, "%1.1f Kb", float(gprsNum.nValue1) / 1024);
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop+=15; 
    TextOut(hdc, nLeft1, nTop, "月通信流量:");
    if (gprsNumFlag < 0)
    {
        TextOut(hdc, nLeft2, nTop, "-- Kb");
    }
    else{
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf, "%1.1f Kb", float(gprsNum.nValue2) / 1024);
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop+=15; 
    TextOut(hdc, nLeft1, nTop, "信号强度MAX:");
    nTop+=15; 
    TextOut(hdc, nLeft1, nTop, "发生时间:");

    bzero(czBuf,sizeof(czBuf));
    bzero(timeBuf,sizeof(timeBuf));
    if((signalInfo.maxSig == 0) || (signalInfo.maxSig == 65535))
    {
        sprintf(czBuf, "-- dBm");
        sprintf(timeBuf, "--:--");
    }
    else
    {
        sprintf(czBuf, "%d dBm", signalInfo.maxSig);
        sprintf(timeBuf, "%02d:%02d", signalInfo.maxTime.hour, signalInfo.maxTime.minute);
    }
    nTop -= 15;
    TextOut(hdc, nLeft2, nTop, czBuf);
    nTop+=15; 
    TextOut(hdc, nLeft2, nTop, timeBuf);

    nTop+=15; 
    TextOut(hdc, nLeft1, nTop, "信号强度MIN:");
     nTop+=15; 
    TextOut(hdc, nLeft1, nTop, "发生时间:");
    bzero(czBuf,sizeof(czBuf));
    bzero(timeBuf,sizeof(timeBuf));
    if((signalInfo.minSig < 0) || (signalInfo.minSig == 65535))
    {
        sprintf(czBuf, "-- dBm");
        sprintf(timeBuf, "--:--");
    }
    else
    {
        sprintf(czBuf, "%d dBm", signalInfo.minSig);
        sprintf(timeBuf, "%02d:%02d", signalInfo.minTime.hour, signalInfo.minTime.minute);
    }
    nTop -= 15;
    TextOut(hdc, nLeft2, nTop, czBuf);
    nTop+=15; 
    TextOut(hdc, nLeft2, nTop, timeBuf);
}

BOOL GprsNumStatKeyMsg(int nKey)
{
PLUGIN_INFO* pPlugin;
switch(nKey)
    {
        case KEY_OK:
            pPlugin = GetPlugIn(0x0043000c);
                if(pPlugin == NULL)
                {
                    ASSERT(FALSE);
                    return FALSE;
                }
               g_guiMain.pluginCur = pPlugin;
            if(g_guiMain.pluginCur->pfnInitLayer != NULL)
                {
                    g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
                }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
        break;
        default:
            return FALSE;
        break;
    }
    UpdateWindow(g_guiComm.hMainWnd, TRUE);
    return TRUE;
}

PLUGIN_INFO g_GprsNumStat= {
    0x0042000c/*id*/, 
    GprsNumStatInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    GprsNumStatDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

/************************************************************************/
/* 重庆集中器中继路由信息查询    (参数查询)                                              */
/*                                                                                                                         */
/************************************************************************/
OOP_ACQ_RLUESTATE_T acqstate;                                        //每个电表的档案信息
uint16 relayLevel[MAX_METER_NUM];
uint16 Down_PRESS;
void CQRouterInfoStatInit(DWORD dwPluginID)
{
    uint32      len = 0;
    NOMAL_OAD_T nomalOad;
    memset(&nomalOad, 0, sizeof(nomalOad));
    memset(relayLevel, 0, sizeof(relayLevel));
    nomalOad.logicId = 0x00;
    nomalOad.oad.value = 0x60320200;
    for (uint16 i = 0; i < gDeskMeter.meterNum; i++){
        memset(&acqstate, 0, sizeof(OOP_ACQ_RLUESTATE_T));
        nomalOad.infoNum = gDeskMeter.meterInfo[i].pn;
        int ret = db_read_nomal(g_hUdbCenter, &nomalOad, sizeof(OOP_ACQ_RLUESTATE_T), (uint8*)&acqstate, &len);
        if ((ret < 0) || (len != sizeof(OOP_ACQ_RLUESTATE_T)))
        {
            continue;
        }
        else{
            relayLevel[acqstate.relayLevel]++;
        }
    }
    Down_PRESS = 0;
}
void CQRouterInfoStatDraw(HDC hdc)
{
    int nLeft1 = 10;
    int nTop = 20;
    char czBuf[200];
    uint8 i;
    SelectFont(hdc,g_guiComm.fontSmall);
    bzero(czBuf,sizeof(czBuf));
    snprintf(czBuf, sizeof(czBuf), "%u-%u %s", //页面标题
        GetPrimaryMenuIndex(),GetSecondaryMenuIndex(),
        g_guiMain.pPrentMenu->pPlugStruct->czCaption);
    TextOut(hdc, nLeft1, nTop, czBuf);
    for(i=0;i<5;i++)
    {
        nTop+=20;
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf,"%d级中继电能表数:    %d", i+Down_PRESS*5, relayLevel[i+Down_PRESS*5]);
        TextOut(hdc, nLeft1, nTop, czBuf);
    }
}

BOOL CQRouterInfoStatKeyMsg(int nKey)
{
PLUGIN_INFO* pPlugin;
switch(nKey)
    {
        case KEY_OK:
            pPlugin = GetPlugIn(0x0043000c);
                if(pPlugin == NULL)
                {
                    ASSERT(FALSE);
                    return FALSE;
                }
               g_guiMain.pluginCur = pPlugin;
            if(g_guiMain.pluginCur->pfnInitLayer != NULL)
                {
                    g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
                }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        case KEY_UP:
        case KEY_LEFT: 
            if (Down_PRESS > 0)
                Down_PRESS--;
            else
                Down_PRESS = 1;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        case KEY_DOWN:
        case KEY_RIGHT: 
            if (Down_PRESS == 0)
                Down_PRESS++;
            else
                Down_PRESS = 0;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        default:
            return FALSE;
            break;
    }
    UpdateWindow(g_guiComm.hMainWnd, TRUE);
    return TRUE;
}
PLUGIN_INFO g_CQRouterInfoStat=
{
    0x0043000c,
    CQRouterInfoStatInit,
    NULL,
    CQRouterInfoStatDraw,
    NULL,
    CQRouterInfoStatKeyMsg,
    NULL
};



#if DESC("测量点有功总抄表查询功能--变量声明及公共函数", 1) 
typedef struct 
{
OOP_ENERGY_T    penergy;                            //正向有功 0x00100200
OOP_ENERGYS_T   penergys;                           //正向无功 0x00300200
OOP_ENERGY_T    nenergy;                            //反向有功 0x00200200
OOP_ENERGYS_T   nenergys;                           //反向无功 0x00400200
OOP_ENERGY_T    quadrant1;                          //第一象限 0x00500200
OOP_ENERGY_T    quadrant2;                          //第二象限 0x00600200
OOP_ENERGY_T    quadrant3;                          //第三象限 0x00700200
OOP_ENERGY_T    quadrant4;                          //第四象限 0x00800200
} DISPLAY_DAY_FROZEN_INFO;

typedef struct
{
    OOP_HENERGY_T        penergy;         //正向有功 0x00100400
    OOP_HENERGYS_T       penergys;        //正向无功 0x00300400
    OOP_HENERGY_T        nenergy;         //反向有功 0x00200400
    OOP_HENERGYS_T       nenergys;        //反向无功 0x00400400
    OOP_HENERGY_T        quadrant1;       //第一象限 0x00500400
    OOP_HENERGY_T        quadrant2;       //第二象限 0x00600400
    OOP_HENERGY_T        quadrant3;       //第三象限 0x00700400
    OOP_HENERGY_T        quadrant4;       //第四象限 0x00800400
}HIGH_PRECISION_DATA_INFO;

extern DISPLAY_DAY_FROZEN_INFO display_day_frozen;
extern void day_frozen_str2time(uint32 * tm);
extern int app_get_meter_list(uint32 infoNum, OOP_METER_T * oopMeter);
extern char     g_day_frozen_store_time[16];

static int      g_page1 = 0; //-当前表号页面(全局变量,取值范围[0~MAX_PAGE],由页面1-2至1-6读写)-
HIGH_PRECISION_DATA_INFO display_day_frozen_high;

int day_frozen_db_read_cq(DB_CLIENT fp, uint8 logic_id, uint16 info_num, uint8 * pbuf, uint32 buf_len, uint8 flg)
{
    int             ret = ERR_NORMAL;
    uint32          len = 0;
    READ_RECORD_T   inReadRecord;
    RECORD_UNIT_T   stepRecordOut;
    OOP_METER_T     oopMeter;
    NOMAL_OAD_T     nomalOAD;
    OOP_OCTETVAR16_T m_tmnAddr;

    //DateTimeHex_t      startData;
    uint32          startTm = 0;
    uint32          endTm = 0;

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    day_frozen_str2time(&startTm);

    // 注：+8个小时(28800)转成格林时间
    startTm += COMPENSATION_TIME;
    endTm = startTm + 24 * 60 * 60 - 1;

    inReadRecord.recordOAD.optional = 1;
    inReadRecord.recordOAD.logicId = logic_id;
    inReadRecord.recordOAD.infoNum = info_num;
    inReadRecord.recordOAD.road.oadMain.value = 0x50040200;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    if (flg == 0)
    {
        inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x00100200;
    }    
    else
    {
        inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x00100400;
    } 
        
    inReadRecord.recordOAD.classtag = 5;
    inReadRecord.cType = COL_TM_STORE;
    inReadRecord.cStart = startTm;
    inReadRecord.cEnd = endTm;
    inReadRecord.sortType = DOWN_SORT;
    inReadRecord.ti.unit = 0;
    inReadRecord.ti.value = 0;

    inReadRecord.MAC.nNum = 6;

    if(info_num == 0)
    {
        memset(&nomalOAD, 0, sizeof(NOMAL_OAD_T));
        nomalOAD.logicId = logic_id;
        nomalOAD.oad.value = 0x40010200;
        ret = db_read_nomal(fp, &nomalOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *) &m_tmnAddr, &len);

        if(0 != ret || len != sizeof(OOP_OCTETVAR16_T))
        {
            GUI_FMT_DEBUG(" db_read_nomal 0x40010200 err = %d!\n", ret);
            return - 1;
        }

        memcpy(inReadRecord.MAC.value, m_tmnAddr.value, 6);
    }
    else
    {
        if(app_get_meter_list(info_num, &oopMeter) != ERR_OK)
        {
            GUI_FMT_DEBUG(" app_get_meter_list %d !\n", info_num);
            return - 1;
        }

        memcpy(inReadRecord.MAC.value, oopMeter.basic.tsa.add, 6);
    }

    ret = db_read_record(fp, &inReadRecord, buf_len, &stepRecordOut);

    if(ret != ERR_OK)
    {
        GUI_FMT_DEBUG(" db_read_record %08x ret=%d !\n", 0x00100200, ret);
        return 0;
    }

    if (flg == 0)
    {
        memcpy(&display_day_frozen.penergy, stepRecordOut.record[0].outDataBuf, sizeof(OOP_ENERGY_T));
        GUI_BUF_DEBUG(stepRecordOut.record[0].outDataBuf, sizeof(OOP_ENERGY_T), "display_day_frozen:\n");
    }    
    else
    {
        memcpy(&display_day_frozen_high.penergy, stepRecordOut.record[0].outDataBuf, sizeof(OOP_HENERGY_T)); 
        GUI_BUF_DEBUG(stepRecordOut.record[0].outDataBuf, sizeof(OOP_ENERGY_T), "display_day_frozen_high:\n");
    }
        

    

    return 0;
}


void QueryEnergyInit(DWORD dwPluginId)
{
    //获取g_date时间传给全局变量g_day_frozen_store_time
    snprintf(g_day_frozen_store_time, sizeof(g_day_frozen_store_time), "%u%02u%02u", g_date.year, g_date.month,
         g_date.day);
    GUI_BUF_DEBUG(g_day_frozen_store_time, 16, "g_day_frozen_store_time:");
    g_page1 = 0;
}

void QueryEnergyDraw(HDC hdc, uint8 enegy_flg)
{
    int             nLeft1 = 20;
    int             nLeft2 = 40;
    int             nTop = 20;
    char            czBuf[100];
    char            str[100];
    int             i;
    mtrnum_t        meter;
    DESK_METER_LIST_T meterList;
    uint8 ret_low = 0;
    uint8 ret_high = 0;

    SelectFont(hdc, g_guiComm.fontSmall);
    bzero(czBuf, sizeof(czBuf));
    GUI_FMT_DEBUG("页面标题：%u-%u %s.\n", GetPrimaryMenuIndex(), GetSecondaryMenuIndex(), 
        g_guiMain.pPrentMenu->pPlugStruct->czCaption);
    //页面标题
    snprintf(czBuf, sizeof(czBuf), "%u-%u %s",
    GetPrimaryMenuIndex(), GetSecondaryMenuIndex(), 
        g_guiMain.pPrentMenu->pPlugStruct->czCaption);
    TextOut(hdc, nLeft1, nTop, czBuf);

    memset(&meterList,0x00,sizeof(DESK_METER_LIST_T));
    meter_info_read_all(&meterList);
    for(i = 0; i < 8; i++)
    {
        meter = meterList.meterInfo[g_page1 * 8 + i].pn;

        if(meterList.meterStatus != DESK_METER_OK || meter < 1)
            break;

        bzero(czBuf, sizeof(czBuf));
        sprintf(czBuf, "%03u", meter);
        GUI_FMT_DEBUG("测量点：%s\n", czBuf);

        memset(&display_day_frozen, 0, sizeof(DISPLAY_DAY_FROZEN_INFO));
        memset(&display_day_frozen_high, 0, sizeof(HIGH_PRECISION_DATA_INFO));

        ret_low = day_frozen_db_read_cq(g_hUdbCenter, 0, meter, (uint8 *) &display_day_frozen, sizeof(DISPLAY_DAY_FROZEN_INFO), 0);
        ret_high = day_frozen_db_read_cq(g_hUdbCenter, 0, meter, (uint8 *) &display_day_frozen_high, sizeof(HIGH_PRECISION_DATA_INFO), 1);
        if(ret_low == 0 || ret_high == 0)
        {
            GUI_FMT_DEBUG("测量点%d的num: %u,示值0020：%12.2f.\n", meter, display_day_frozen.penergy.nNum,
                    (double) display_day_frozen.penergy.nValue[enegy_flg] / 100);
            GUI_FMT_DEBUG("测量点%d的num: %u,示值0040：%12.4f.\n", meter, display_day_frozen_high.penergy.nNum,
                    (double) display_day_frozen_high.penergy.nValue[enegy_flg] / 10000);
            nTop += 12;
            TextOut(hdc, nLeft1, nTop, czBuf);

            bzero(str, sizeof(str));

            if(display_day_frozen.penergy.nNum == 0 && display_day_frozen_high.penergy.nNum == 0)
            {
                sprintf(str, "     ----- kWh");
                GUI_FMT_DEBUG("测量点%d的示值：     -----\n", meter);
            }
            else
            {                
                display_day_frozen_high.penergy.nNum != 0 ? sprintf(str, "%12.4f kWh",
                     (double) display_day_frozen_high.penergy.nValue[enegy_flg] / 10000): sprintf(str, "%12.2f kWh",
                     (double) display_day_frozen.penergy.nValue[enegy_flg] / 100);
            }

            TextOut(hdc, nLeft2, nTop, str);
        }
    }
}

BOOL QueryEnergyKeyMsg(int nKey)
{
    uint16          MAX_PAGE1 = 0;
    DESK_METER_LIST_T meterList;

    memset(&meterList,0x00,sizeof(DESK_METER_LIST_T));
    meter_info_read_all(&meterList);
    if(meterList.meterStatus == DESK_METER_OK && gDeskMeter.meterNum / 8 >= 1)
        MAX_PAGE1 = gDeskMeter.meterNum / 8;

    GUI_FMT_DEBUG("gDeskMeter.meterNum：%u.\n", gDeskMeter.meterNum);
    GUI_FMT_DEBUG("MAX_PAGE1：%u.\n", MAX_PAGE1);

    switch(nKey)
    {
        //-定义上下键为逐页翻页-
        case KEY_UP:
            g_page1--;

            if(g_page1 < 0)
            {
                g_page1 = MAX_PAGE1;
            }

            break;

        case KEY_DOWN:
            g_page1++;

            if(g_page1 > MAX_PAGE1)
            {
                g_page1 = 0;
            }

            break;

        //-定义左右键为快速翻页-
        case KEY_LEFT:
            g_page1 -= 10;

            if(g_page1 < 0)
            {
                g_page1 = MAX_PAGE1;
            }

            break;

        case KEY_RIGHT:
            g_page1 += 10;

            if(g_page1 > MAX_PAGE1)
            {
                g_page1 = 0;
            }

            break;

        default:
            return FALSE;

            break;
    }

    UpdateWindow(g_guiComm.hMainWnd, TRUE);
    return TRUE;
}
#endif

#if DESC("测量点有功总示值",1)
void QueryEnergyTotalDraw(HDC hdc)
{
    QueryEnergyDraw(hdc, 0);
}

PLUGIN_INFO g_QueryEnergyTotal= {
    0x0010000c/*id*/, 
    QueryEnergyInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    QueryEnergyTotalDraw/*Draw*/,
    NULL/*Timer*/,
    QueryEnergyKeyMsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("测量点有功尖示值",1)
void QueryEnergyTipDraw(HDC hdc)
{
    QueryEnergyDraw(hdc, 1);
}
 
PLUGIN_INFO g_QueryEnergyTip= {
    0x0014000c/*id*/, 
    QueryEnergyInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    QueryEnergyTipDraw/*Draw*/,
    NULL/*Timer*/,
    QueryEnergyKeyMsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("测量点有功峰示值",1)
void QueryEnergyPeakDraw(HDC hdc)
{
    QueryEnergyDraw(hdc, 2);
}
 
PLUGIN_INFO g_QueryEnergyPeak= {
    0x0011000c/*id*/, 
    QueryEnergyInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    QueryEnergyPeakDraw/*Draw*/,
    NULL/*Timer*/,
    QueryEnergyKeyMsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("测量点有功平示值",1)
void QueryEnergyFlatDraw(HDC hdc)
{
    QueryEnergyDraw(hdc, 3);
}
 
PLUGIN_INFO g_QueryEnergyFlat= {
    0x0012000c/*id*/, 
    QueryEnergyInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    QueryEnergyFlatDraw/*Draw*/,
    NULL/*Timer*/,
    QueryEnergyKeyMsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("测量点有功谷示值",1)
void QueryEnergyValleyDraw(HDC hdc)
{
    QueryEnergyDraw(hdc, 4);
}
 
PLUGIN_INFO g_QueryEnergyValley= {
    0x0013000c/*id*/, 
    QueryEnergyInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    QueryEnergyValleyDraw/*Draw*/,
    NULL/*Timer*/,
    QueryEnergyKeyMsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

int             gRealIndex; //实时抄表界面的索引
uint8 gTransFlag;       //标记当前是否在透传中 0表示没有 1表示指定测量点抄表 2表示全部测量点抄表
/*********************************************************************
    所属功能单元：调试页面库--实时抄表
    功能描述：    调试页面库初始化函数
    输入参数：    DWORD dwPluginId：页面库ID
    输出参数：
    函数返回值：
    内部处理流程：
********************************************************************/
void realTimeMeterReadingInit(DWORD dwPluginId)
{
    gRealIndex = 0;
    gTransFlag = 0;
}

/*********************************************************************
    所属功能单元：调试页面库--实时抄表
    功能描述：    调试页面库绘制函数
    输入参数：    HDC hdc：绘制上下文句柄
    输出参数：
    函数返回值：
    内部处理流程：
********************************************************************/
void realTimeMeterReadingDraw(HDC hdc)
{

    int             nLeft1 = 10;
    int             nTop = 16;

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;
    TextOut(hdc, nLeft1, nTop, "5-1 实时抄表");
    //nLeft1 += 10;
    nTop += 25;

    if(gRealIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "1、指定测量点抄表");
        ITEM_NOT_SELECT;
    }
    else
        TextOut(hdc, nLeft1, nTop, "1、指定测量点抄表");

    nTop += 25;

    if(gRealIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "2、全部测量点抄表");
        ITEM_NOT_SELECT;
    }
    else
        TextOut(hdc, nLeft1, nTop, "2、全部测量点抄表");

}

/*********************************************************************
    所属功能单元：调试页面库--实时抄表
    功能描述：    调试页面库按键处理消息
    输入参数：    int nKey：按键消息
    输出参数：
    函数返回值：  BOOL：TRUE表示页面库已完成处理，FALSE表示执行缺省处理
    内部处理流程：
********************************************************************/
BOOL realTimeMeterReadingKeyMsg(int nKey)
{
    PLUGIN_INFO *   pPlugin;

    switch(nKey)
    {
        case KEY_DOWN: //上下键用于逐页查看
        case KEY_RIGHT: //左右键用于快速翻页
            gRealIndex++;

            if(gRealIndex > 1)
                gRealIndex = 0;

            break;

        case KEY_UP:
        case KEY_LEFT:
            gRealIndex--;

            if(gRealIndex < 0)
                gRealIndex = 1;

            break;

        case KEY_OK:
            {
                switch(gRealIndex)
                {
                    case 0:
                        pPlugin = GetPlugIn(0x0060001c);

                        if(pPlugin == NULL)
                        {
                            ASSERT(FALSE);
                            return FALSE;
                        }

                        g_guiMain.pluginCur = pPlugin;

                        if(g_guiMain.pluginCur->pfnInitLayer != NULL)
                        {
                            g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
                        }

                        UpdateWindow(g_guiComm.hMainWnd, TRUE);
                        break;

                    case 1:
                        pPlugin = GetPlugIn(0x0060003c);

                        if(pPlugin == NULL)
                        {
                            ASSERT(FALSE);
                            return FALSE;
                        }

                        g_guiMain.pluginCur = pPlugin;

                        if(g_guiMain.pluginCur->pfnInitLayer != NULL)
                        {
                            g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
                        }

                        UpdateWindow(g_guiComm.hMainWnd, TRUE);
                        break;

                    default:
                        break;
                }

                break;
            }

        default:
            break;
    }

    UpdateWindow(g_guiComm.hMainWnd, TRUE);
    return (FALSE);
}

//5-1 实时抄表
PLUGIN_INFO     g_real_time_meter_reading =
{
    0x0060000c /*id*/, 
    realTimeMeterReadingInit /*InitLayer*/, 
    NULL /*UninitLayer*/, 
    realTimeMeterReadingDraw /*Draw*/, 
    NULL /*Timer*/, 
    realTimeMeterReadingKeyMsg /*keymsg*/, 
    NULL                                            /*ipc*/
};


int gRealSingleIndex;   
CQ_METER_TRANS_T gRealSingleTrans;  //透传用的结构体
const char *gTransStatus[4]={"初始","已发送","成功","超时"};   //透传显示在界面上的状态
/*********************************************************************
    所属功能单元：调试页面库--实时抄表
    功能描述：    调试页面库初始化函数
    输入参数：    DWORD dwPluginId：页面库ID
    输出参数：
    函数返回值：
    内部处理流程：
********************************************************************/
void realTimeMeterReadingSingleInit(DWORD dwPluginId)
{
    gRealSingleIndex = 0;
    gTransFlag = 1;
    //判一下状态 以防界面退出后再进就被打断了
    if(gRealSingleTrans.status != CQ_TRANS_SEND)
        memset(&gRealSingleTrans,0x00,sizeof(gRealSingleTrans));
}

/*********************************************************************
    所属功能单元：调试页面库--实时抄表
    功能描述：    调试页面库绘制函数
    输入参数：    HDC hdc：绘制上下文句柄
    输出参数：
    函数返回值：
    内部处理流程：
********************************************************************/
void realTimeMeterReadingSingleDraw(HDC hdc)
{

    int             nLeft1 = 10;
    int             nLeft2 = 90;
    int             nTop = 20;
    char            czbuf[100] = {0};

    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;
    TextOut(hdc, nLeft1, nTop, "5-2 指定测量点抄表");
    nTop += 25;
    TextOut(hdc, nLeft1, nTop, "输入测量点号:");
    sprintf(czbuf, "%d", gRealSingleTrans.pn);

    if(gRealSingleIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czbuf);
        ITEM_NOT_SELECT;
    }
    else
        TextOut(hdc, nLeft2, nTop, czbuf);

    if(gRealSingleIndex == 1)
    {
        ITEM_SELECT;
    }

    TextOut(hdc, nLeft2 + 30, nTop, "启动");
    ITEM_NOT_SELECT;

    nTop += 25;
    TextOut(hdc, nLeft1, nTop, "电能表时间:");

    if(gRealSingleTrans.status != CQ_TRANS_SUCC)
        sprintf(czbuf, "--:--:--");
    else
        sprintf(czbuf, "%02d:%02d:%02d", gRealSingleTrans.meterTime.hour ,gRealSingleTrans.meterTime.minute,gRealSingleTrans.meterTime.second);

    nLeft2 -= 15;
    TextOut(hdc, nLeft2, nTop, czbuf);
    nTop += 25;
    TextOut(hdc, nLeft1, nTop, "有功总示值:");
    //nTop += 20;

    if(gRealSingleTrans.status != CQ_TRANS_SUCC)
        sprintf(czbuf, " - kWh");
    else
    {

        sprintf(czbuf, "%.2f kWh", (double) gRealSingleTrans.meterData/100);
        
    }

    TextOut(hdc, nLeft2, nTop, czbuf);

    nTop += 25;
    TextOut(hdc, nLeft1, nTop, "抄表状态:");
    if(gRealSingleTrans.status < 4)
        TextOut(hdc, nLeft2, nTop, gTransStatus[gRealSingleTrans.status]);

}

/*********************************************************************
    所属功能单元：输入信息点号
    功能描述：    
    输入参数：
    输出参数：
    函数返回值：
    内部处理流程：
********************************************************************/
void pnInsertReturn(char* pNewStr, void* pPara)
{   
    int nPoint = atoi(pNewStr);
    if(nPoint > 5000)
    {
        char bzbuf[100];
        sprintf(bzbuf, "测量点号最大为5000\n确认是否重新输入?");
        if (TRUE == MessageBox(bzbuf, MB_OKCANCEL))
        {
            sprintf(bzbuf, "%d", gRealSingleTrans.pn);
            InputBox(bzbuf, pnInsertReturn, NULL, 0x01);
        }
    }
    else
    {
        gRealSingleTrans.pn = nPoint;
    }
}

#define METER_TRANS_OVERTIME  30   //代理超时时间30秒
/*******************************************************************************
* 函数名称: meter_trans_apdu_pack 
* 函数功能: 根据档案地址组0901代理电表时间和有功示值
* 输入参数: tsa 地址 buflen 最大长度
* 输出参数: buf 组出来的apdu
* 返 回 值: apdu长度
*******************************************************************************/
uint16 meter_trans_apdu_pack(OOP_TSA_T *tsa,uint8 *buf,uint16 buflen)
{
    uint16 index = 0;
    uint16 overtime = METER_TRANS_OVERTIME;
    uint8 apdu[1024]={0};
    OOP_OAD_U oad;
    apdu[index++] = 0x09;
    apdu[index++] = 0x01;
    apdu[index++] = 0x00;
    memcpy_r(&apdu[index],&overtime,sizeof(uint16));
    index+=2;
    apdu[index++] = 1;
    apdu[index++] = 7;
    apdu[index++] = 5;
    memcpy(&apdu[index],tsa->add,6);
    index+=6;
    memcpy_r(&apdu[index],&overtime,sizeof(uint16));
    index+=2;
    apdu[index++] = 2;
    oad.value = 0x40000200;
    memcpy_r(&apdu[index],&oad.value,4);
    index+=4;
    oad.value = 0x00100200;
    memcpy_r(&apdu[index],&oad.value,4);
    index+=4;
    apdu[index++] = 0;  
    if(index > buflen)
        return 0;
    memcpy_s(buf,buflen,apdu,index);
    return index;
}

/**********************************************************************
* @name      : meter_trans_amr_send
* @brief     ：通过taskManager透传电表数据
* @param[in] ：
* @return    ：
* @Date      ：
* @Update    :
**********************************************************************/
int meter_trans_amr_send(uint8* playload, uint16 length,uint16 *msgIndex)
{
    int    ret = 0;
    uint8  sendbuf[101] = {0};
    if(length >100)
    {
        length = 100;
    }

    sendbuf[0] = length;
    memcpy(sendbuf+1, playload, length);

    ret = gui_send_ctsmsg(0, CTS_IOP_READ_ONE_REQ, sendbuf, length+1,msgIndex);
    if (ret != ERR_OK)
    {
        GUI_FMT_DEBUG("gui_send_ctsmsg err!\n");
    }
    
    return ret;
}

/**********************************************************************
* @name      : meter_trans_amr_send
* @brief     ：通过taskManager透传电表数据
* @param[in] ：
* @return    ：
* @Date      ：
* @Update    :
**********************************************************************/
int meter_trans_amr_send_0010(uint8* playload, uint16 length,uint16 *msgIndex)
{
    int    ret = 0;

    ret = gui_send_ctsmsg(0, CTS_IOP_READ_LIST_REQ, playload, length,msgIndex);
    if (ret != ERR_OK)
    {
        GUI_FMT_DEBUG("gui_send_ctsmsg err!\n");
    }
    
    return ret;
}


/*********************************************************************
    所属功能单元：调试页面库--实时抄表
    功能描述：    调试页面库按键处理消息
    输入参数：    int nKey：按键消息
    输出参数：
    函数返回值：  BOOL：TRUE表示页面库已完成处理，FALSE表示执行缺省处理
    内部处理流程：
********************************************************************/
BOOL realTimeMeterReadingSingleKeyMsg(int nKey)
{
    char            bzbuf[50];
    int ret;
    DESK_METER_T meterInfo;
    uint8 payload[1024]={0};
    uint16 len = 0;
    switch(nKey)
    {
        case KEY_LEFT:
            gRealSingleIndex--;

            if(gRealSingleIndex < 0)
                gRealSingleIndex = 1;

            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;

        case KEY_RIGHT:
            gRealSingleIndex++;

            if(gRealSingleIndex > 1)
                gRealSingleIndex = 0;

            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;

        case KEY_OK:
            if(gRealSingleIndex == 0)
            {
                sprintf(bzbuf, "%d", gRealSingleTrans.pn);
                InputBox(bzbuf, pnInsertReturn, NULL, 0x01);
            }
            else if(gRealSingleIndex == 1)
            {   
                //先判断下档案是否有效
                memset(&meterInfo,0x00,sizeof(meterInfo));
                ret = meter_info_read_from_pn(gRealSingleTrans.pn,&meterInfo);
                if(ret < 0)
                {
                    memset(bzbuf,0x00,sizeof(bzbuf));
                    sprintf(bzbuf,"测量点号%d在档案中不存在\n请重新输入\n",gRealSingleTrans.pn);
                    MessageBox(bzbuf);
                        
                }else
                {
                    //启动代理 用0901代理时间和电量
                    len = meter_trans_apdu_pack(&meterInfo.tsa,payload,sizeof(payload));
                    if(len>0)
                    {
                        MessageBox("透传已发送，请稍等\n超时时间30秒!\n");
                        gRealSingleTrans.tsa = meterInfo.tsa;
                        meter_trans_amr_send(payload,len,&gRealSingleTrans.sendIndex);
                        gRealSingleTrans.status = CQ_TRANS_SEND;
                        gRealSingleTrans.sendtime = time(NULL);
                    }
                    
                }

            }

            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
    }

    UpdateWindow(g_guiComm.hMainWnd, TRUE);
    return FALSE;
}

void realTimeMeterReadingSingleTimer(int nId)
{
    time_t timenow;
    if (nId == TIMER_MAIN && gRealSingleTrans.status == CQ_TRANS_SEND)
    {
        timenow = time(NULL);
        if(abs(timenow - gRealSingleTrans.sendtime) > 30)
        {
            GUI_FMT_DEBUG("超时未收到taskManager回码\n");
            gRealSingleTrans.status = CQ_TRANS_FAIL;
        }
    }
}

//5-2 指定测量点抄表
PLUGIN_INFO     g_real_time_meter_reading1 =
{
    0x0060001c /*id*/, 
    realTimeMeterReadingSingleInit /*InitLayer*/, 
    NULL /*UninitLayer*/, 
    realTimeMeterReadingSingleDraw /*Draw*/, 
    realTimeMeterReadingSingleTimer, 
    realTimeMeterReadingSingleKeyMsg /*keymsg*/, 
    NULL                                            /*ipc*/
};

int             gReadTimeReadingIndex;
/*********************************************************************
    所属功能单元：调试页面库--实时抄表
    功能描述：    调试页面库初始化函数
    输入参数：    DWORD dwPluginId：页面库ID
    输出参数：
    函数返回值：
    内部处理流程：
********************************************************************/
void realTimeReadingAllInit(DWORD dwPluginId)
{
    gReadTimeReadingIndex = 0;
}
void realTimeReadingAllDraw(HDC hdc)
{
    int             nLeft1 = 10;
    int             nTop = 20;

    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;
    TextOut(hdc, nLeft1, nTop, "5-3 全部测量点抄表");
    //nLeft1 += 10;
    nTop += 25;
    TextOut(hdc, nLeft1, nTop, "全部抄表将耗时很长!");
    nTop += 25;

    if(gReadTimeReadingIndex == 0)
    {
        ITEM_SELECT; 
        TextOut(hdc, nLeft1, nTop, "启动");
        ITEM_NOT_SELECT;
    }
    else
        TextOut(hdc, nLeft1, nTop, "启动");

    nLeft1 += 50;

    if(gReadTimeReadingIndex == 1)
    {
        ITEM_SELECT; 
        TextOut(hdc, nLeft1, nTop, "取消");
        ITEM_NOT_SELECT;
    }
    else
        TextOut(hdc, nLeft1, nTop, "取消");
}

DESK_METER_LIST_T gMeterlist; //所有档案
int          gPageIndex; //档案翻页用
uint16       gPageMax  ; //最多多少页
CQ_ALL_METER_TRANS_T gAllTrans; //所有电表的透传结构

/*********************************************************************
    所属功能单元：调试页面库--实时抄表
    功能描述：    调试页面库初始化函数
    输入参数：    
    输出参数：
    函数返回值：
    内部处理流程：
********************************************************************/
void readTimeReadingAllDataInit()
{
    uint16 i;
    memset(&gMeterlist, 0X00, sizeof(gMeterlist));
    meter_info_read_all(&gMeterlist);
    gPageIndex = 0;
    gPageMax = gMeterlist.meterNum / 8;
    GUI_FMT_DEBUG("总档案数 %d gPageMax %d\n",gMeterlist.meterNum,gPageMax);
    memset(&gAllTrans,0x00,sizeof(gAllTrans));
    gAllTrans.meternum = gMeterlist.meterNum;
    for(i=0;i<gAllTrans.meternum;i++)
    {
        gAllTrans.transinfo[i].pn = gMeterlist.meterInfo[i].pn;
        gAllTrans.transinfo[i].tsa = gMeterlist.meterInfo[i].tsa;
    }
    gTransFlag = 2;
}


BOOL realTimeReadingAllKeyMsg(int nkey)
{
    PLUGIN_INFO *   pPlugin;

    switch(nkey)
    {
        case KEY_DOWN: //上下键用于逐页查看
        case KEY_RIGHT: //左右键用于快速翻页
            gReadTimeReadingIndex++;

            if(gReadTimeReadingIndex > 1)
                gReadTimeReadingIndex = 0;

            break;

        case KEY_UP:
        case KEY_LEFT:
            gReadTimeReadingIndex--;

            if(gReadTimeReadingIndex < 0)
                gReadTimeReadingIndex = 0;

            break;

        case KEY_OK:
            {
                switch(gReadTimeReadingIndex)
                {
                    case 0:
                        pPlugin = GetPlugIn(0x0060002c);

                        if(pPlugin == NULL)
                        {
                            ASSERT(FALSE);
                            return FALSE;
                        }

                        g_guiMain.pluginCur = pPlugin;

                        if(g_guiMain.pluginCur->pfnInitLayer != NULL)
                        {
                            g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
                        }
                        //在启动的时候初始化 以防从轮显切回去的时候又重新开始透传
                        readTimeReadingAllDataInit();
                        UpdateWindow(g_guiComm.hMainWnd, TRUE);
                        break;

                    case 1:
                        CancelMenu();
                        break;

                    default:
                        break;
                }
            }
    }

    UpdateWindow(g_guiComm.hMainWnd, TRUE);
    return FALSE;
}

PLUGIN_INFO     g_real_time_reading_all =
{
    0x0060003c /*id*/, 
    realTimeReadingAllInit /*InitLayer*/, 
    NULL /*UninitLayer*/, 
    realTimeReadingAllDraw /*Draw*/, 
    NULL /*Timer*/, 
    realTimeReadingAllKeyMsg /*keymsg*/, 
    NULL                                            /*ipc*/
};




void readTimeReadingAllDataDraw(HDC hdc)
{
    int             nLeft1 = 10;
    int             nLeft2 = 45;
    int             nLeft3 = 80;
    int             nTop = 16;
    char            str[100]={0};
    int             i;
    uint16 index;

    SelectFont(hdc, g_guiComm.fontSmall);
    TextOut(hdc, nLeft1, nTop, "5-4 全部测量点抄表"); //显示页面标题
    nTop += 13;
    TextOut(hdc, nLeft1, nTop, "测量点");        //显示页面标题  
    TextOut(hdc, nLeft2 + 5, nTop, "时间");       //显示页面标题  
    TextOut(hdc, nLeft3, nTop, "有功总");        //显示页面标题  
    if(gMeterlist.meterStatus != DESK_METER_OK)
    {
        MessageBox("档案未初始化完成\n请稍后再试!\n");
        CancelMenu();
        return;
    }
    for(i = 0; i < 8; i++)
    {
        index = gPageIndex * 8 + i ;
        if(index >= gAllTrans.meternum)
            break;
        nTop += 12;
        sprintf(str, "%03d", gAllTrans.transinfo[index].pn);
        TextOut(hdc, nLeft1, nTop, str);
        sprintf(str, "--:--");

        if(gAllTrans.transinfo[index].status == CQ_TRANS_SUCC)
        {
            sprintf(str, "%02d:%02d", gAllTrans.transinfo[index].meterTime.hour,gAllTrans.transinfo[index].meterTime.minute);
        }

        TextOut(hdc, nLeft2, nTop, str);
        sprintf(str, "--------");

        if(gAllTrans.transinfo[index].status == CQ_TRANS_SUCC)
        {
            sprintf(str, "%.2f kWh", (double) gAllTrans.transinfo[index].meterData/100);
        }

        TextOut(hdc, nLeft3, nTop, str);
    }
}

void readTimeReadingAllDataTimer(int nId)
{
    time_t timenow;
    uint16 index = gAllTrans.meterindex;
    uint16 len = 0;
    uint8 payload[2048]={0};
    if(index >= gAllTrans.meternum)
        return;
    if (nId == TIMER_QUICK )
    {
        if(gAllTrans.transinfo[index].status == CQ_TRANS_SEND)
        {
            timenow = time(NULL);
            if(abs(timenow - gAllTrans.transinfo[index].sendtime) > 30)
            {
                GUI_FMT_DEBUG("超时未收到taskManager回码\n");
                gAllTrans.transinfo[index].status = CQ_TRANS_FAIL;
                gAllTrans.meterindex++;
                index++;
            }
        }
        if(gAllTrans.transinfo[index].status == CQ_TRANS_SUCC ||  gAllTrans.transinfo[index].status == CQ_TRANS_FAIL)
        {
            gAllTrans.meterindex++;
            index++;
        }
        if(index >= gAllTrans.meternum)
            return;
        if(gAllTrans.transinfo[index].status == CQ_TRANS_INIT)
        {
            //初始态时就发送 
            len = meter_trans_apdu_pack(&gAllTrans.transinfo[index].tsa,payload,sizeof(payload));
            if(len>0)
            {
                GUI_FMT_DEBUG("PN %d 透传发送\n",gAllTrans.transinfo[index].pn);
                meter_trans_amr_send(payload,len,&gAllTrans.transinfo[index].sendIndex);
                gAllTrans.transinfo[index].status = CQ_TRANS_SEND;
                gAllTrans.transinfo[index].sendtime = time(NULL);
            }
        }

    }
}


BOOL readTimeReadingAllDataKeyMsg(int nKey)
{
    switch(nKey)
    {
        //-定义上下键为逐页翻页-
        case KEY_UP:
        case KEY_LEFT:
            gPageIndex--;

            if(gPageIndex < 0)
            {
                gPageIndex = gPageMax;
            }

            break;

        case KEY_DOWN:
        case KEY_RIGHT:
            gPageIndex++;

            if(gPageIndex > gPageMax)
            {
                gPageIndex = 0;
            }

            break;
//
//        //-定义左右键为快速翻页-
//        case KEY_LEFT:
//            gPageIndex -= 10;
//
//            if(gPageIndex < 0)
//            {
//                gPageIndex = gPageMax;
//            }
//
//            break;
//
//        case KEY_RIGHT:
//            gPageIndex += 10;
//
//            if(gPageIndex > gPageMax)
//            {
//                gPageIndex = 0;
//            }
//
//            break;

        default:
            return FALSE;

            break;
    }

    UpdateWindow(g_guiComm.hMainWnd, TRUE);
    return TRUE;
}



PLUGIN_INFO     g_real_time_reading_all_data =
{
    0x0060002c /*id*/, 
    NULL /*InitLayer*/, 
    NULL /*UninitLayer*/, 
    readTimeReadingAllDataDraw /*Draw*/, 
    readTimeReadingAllDataTimer /*Timer*/, 
    readTimeReadingAllDataKeyMsg /*keymsg*/, 
    NULL                                            /*ipc*/
};

/**********************************************************************
* @name      : meter_search_send
* @brief     ：通过taskManager启动搜表
* @param[in] ：
* @return    ：
* @Date      ：
* @Update    :
**********************************************************************/
int meter_search_send()
{
    int    ret = 0;
    uint8  sendbuf[101] = {0};
    uint16 msgIndex;
    uint32 port = 0xF2090201;
    uint16 searchtime = 0; //默认不限时间搜表 等模块自己结束
    uint16 index = 0;
    memcpy_r(&sendbuf[index],&port,4);
    index+=4;
    memcpy_r(&sendbuf[index],&searchtime,2);
    index+=2;

    ret = gui_send_ctsmsg(0, CTS_IOP_SEEK_METER, sendbuf, index,&msgIndex);
    if (ret != ERR_OK)
    {
        GUI_FMT_DEBUG("meter_search_send err!\n");
    }
    
    return ret;
}
/**********************************************************************
* @name      : meter_search_status_get
* @brief     ：从数据中心查询60020a00获取搜表状态
* @param[in] ：
* @return    ：
* @Date      ：
* @Update    :
**********************************************************************/
int meter_search_status_get(uint8 *status)
{
    int ret;
    NOMAL_OAD_T normal;
    memset(&normal,0x00,sizeof(normal));
    normal.oad.value = 0x60020A00;
    uint8 data;
    uint32 len;
    ret = db_read_nomal(g_hUdbCenter,&normal,1,&data,&len);
    if(ret != 0 || len != 1)
    {
        GUI_FMT_DEBUG("获取搜表状态失败\n");
        *status = 0;
    }else
        *status = data;
    return 0;
}

uint8 gMeterSearchStatus = 0;           //搜表状态 0初始 1搜表中 2搜表结束 
const char *gMeterSearchText[]={"表号搜索未启动","表号搜索中..","表号搜索完毕"};
uint32 gMeterSearchTime;            //搜表启动时间
/*********************************************************************
    所属功能单元：调试页面库--搜表
    功能描述：    调试页面库初始化函数
    输入参数：    
    输出参数：
    函数返回值：
    内部处理流程：
********************************************************************/
void meterSearchInit(DWORD dwPluginId)
{
    //上一次搜表结束后十分钟就给状态改成未启动
    if(abs(time(NULL) - gMeterSearchTime) > 600)
    {
       gMeterSearchStatus = 0;
    }
}

void meterSearchDraw(HDC hdc)
{
    
    const int       nLeft1 = 10;
    const int       nTop = 20;
    RECT            rect;
    const char *    str;
    
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;
    
    TextOut(hdc, nLeft1, nTop, "5-5 表号搜索");
    
    /*输出当前搜索状态*/
    rect = ((RECT) { 0, 120, g_guiComm.nWidth, 136 });
    str = gMeterSearchText[gMeterSearchStatus];
    DrawText(hdc, str, strlen(str), &rect, DT_CENTER);
    
    /*按钮*/
    ITEM_SELECT;
    rect = ((RECT) { 0, 72, g_guiComm.nWidth, 88 });
    str = "启动搜索";
    DrawText(hdc, str, strlen(str), &rect, DT_CENTER);
    ITEM_NOT_SELECT;
}

void meterSearchTimer(int nId)
{
    uint8 status;
    //定时10秒查询一次60020A00搜表状态 查看是否搜表结束
    if(gMeterSearchStatus == 1 && nId == TIMER_MAIN )
    {
        if(abs(time(NULL) - gMeterSearchTime)>10)
        {
            meter_search_status_get(&status);
            if(status == 0)
            {
                gMeterSearchStatus = 2;
                GUI_FMT_DEBUG("搜表结束\n");
            }
                
        }
    }
}

BOOL meterSearchKeyMsg(int nKey)
{
    
    if(nKey == KEY_OK)
    {
        if(gMeterSearchStatus ==1)
        {
            MessageBox("当前正在搜表中\n请稍等！\n");
            return FALSE;
        }
        GUI_FMT_DEBUG("启动搜表\n");
        meter_search_send();
        gMeterSearchTime = time(NULL);
        gMeterSearchStatus = 1;
    }
    return FALSE;
}

//5-5表号搜索
PLUGIN_INFO     g_meter_search_start =
{
    0x0061000c /*id*/, 
    meterSearchInit /*InitLayer*/, 
    NULL /*UninitLayer*/, 
    meterSearchDraw /*Draw*/, 
    meterSearchTimer /*Timer*/, 
    meterSearchKeyMsg /*keymsg*/, 
    NULL                                            /*ipc*/
};
/*********************************************************************
    所属功能单元：调试页面库--搜表
    功能描述：    从数据中心读取搜表结果 60020200
    输入参数：    
    输出参数：newList 新增的列表 notFoundList 档案中未搜到的表
    函数返回值：
    内部处理流程：
********************************************************************/
void meter_search_result_get(CQ_METER_SRARCH_RESULT_T *newList,CQ_METER_SRARCH_RESULT_T *notFoundList)
{
    int ret;
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    uint16 i,j;
    DESK_METER_LIST_T meterList;

    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_SEARCHMET_RES_T  SearchResult;
    
    memset(&SearchResult,0x00,sizeof(OOP_SEARCHMET_RES_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    meter_info_read_all(&meterList);
    //给是否搜到标记置0 待会儿读到搜表结果后判断
    for(i = 0;i<meterList.meterNum;i++)
    {
        meterList.meterInfo[i].searchFlag = 0;
    }
    memset(newList,0x00,sizeof(CQ_METER_SRARCH_RESULT_T));
    memset(notFoundList,0x00,sizeof(CQ_METER_SRARCH_RESULT_T));
    
    GUI_FMT_DEBUG("开始读取搜表结果! \n");

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 5000;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60020200;

    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(g_hUdbCenter, &inReadRecord, &handle, &recNum);
    if(ret!=0 || recNum==0)
    {
        GUI_FMT_DEBUG("开始读搜表结果记录失败 ret  %d recNum %d \n",ret,recNum);
    }else
    {
        GUI_FMT_DEBUG("搜表结果记录 recNum %d \n",recNum);
        for(i=0;i<recNum;i++)
        {
            ret = db_read_record_step(g_hUdbCenter, handle, &stepReadRecord, &stepRecordOut);
            if(ret!=0)
            {
                GUI_FMT_DEBUG("单步读搜表结果记录失败 ret  %d i %d \n",ret,i);
                continue;
            }
            if(stepRecordOut.recordnum!=1 || stepRecordOut.record[0].road.oadCols.oad[0].value!=0x60020200
                ||stepRecordOut.record[0].outDatalen!=sizeof(OOP_SEARCHMET_RES_T))
            {
                GUI_FMT_DEBUG("单步读取的搜表结果记录有问题 \n");
                continue;
            }
            memcpy(&SearchResult,stepRecordOut.record[0].outDataBuf,sizeof(OOP_SEARCHMET_RES_T));
            //搜表结果到档案里找下
            for(j=0;j<meterList.meterNum;j++)
            {
                if(meterList.meterInfo[j].searchFlag == 0 && memcmp(meterList.meterInfo[j].tsa.add,SearchResult.commAdd.add,6) == 0)
                {
                    //搜表结果在档案里找到 标记下
                    meterList.meterInfo[j].searchFlag = 1;
                    break;
                }
            }
            //没在档案里找到 加入新增列表
            if(j == meterList.meterNum)
            {
                newList->tsa[newList->num] = SearchResult.commAdd;
                newList->prtl[newList->num] = SearchResult.proto;
                newList->num++;
            }
       }
    }
    db_read_record_end(g_hUdbCenter, handle);
    //给没搜到的表更新下
    for(i=0;i<meterList.meterNum;i++)
    {   //只管载波的
        if(meterList.meterInfo[i].searchFlag ==0 && meterList.meterInfo[i].port.value == 0xF2090201)
        {
            notFoundList->tsa[notFoundList->num] = meterList.meterInfo[i].tsa;
            notFoundList->prtl[notFoundList->num] = meterList.meterInfo[i].prtl;
            notFoundList->num++;
        }
    }
    GUI_FMT_DEBUG("档案总个数 %d 新增电表个数 %d 未找到电表个数 %d \n",meterList.meterNum,newList->num,notFoundList->num);
}

CQ_METER_SRARCH_RESULT_T gMeterSearchNewList;
CQ_METER_SRARCH_RESULT_T gMeterSearchNotFoundList;
int gMeterSearchPageIndex;
uint16 gMeterSearchPageMax;
CQ_ALL_METER_TRANS_T gMeterSearchTrans; //新增电能表透传


/*********************************************************************
    所属功能单元：调试页面库--搜表
    功能描述：    将新增的电表地址加入到透传列表中进行透传
    输入参数：    
    输出参数：
    函数返回值：
    内部处理流程：
********************************************************************/
void meterSearchTransInit(CQ_METER_SRARCH_RESULT_T *newList)
{
    uint16 i;
    memset(&gMeterSearchTrans,0x00,sizeof(gMeterSearchTrans));
    gMeterSearchTrans.meternum = newList->num;
    for(i=0;i<newList->num;i++)
    {
        gMeterSearchTrans.transinfo[i].tsa = newList->tsa[i];
        gMeterSearchTrans.transinfo[i].prtl = newList->prtl[i];
    }

}
/*********************************************************************
    所属功能单元：调试页面库--搜表
    功能描述：    调试页面库初始化函数
    输入参数：    
    输出参数：
    函数返回值：
    内部处理流程：
********************************************************************/
void meterSearchNewInit(DWORD dwPluginId)
{
    meter_search_result_get(&gMeterSearchNewList,&gMeterSearchNotFoundList);
    gMeterSearchPageIndex = 0;
    //1页显示8个
    gMeterSearchPageMax = gMeterSearchNewList.num/8;
    
}

void meterAddrConvert(char* buf,uint8 size,OOP_TSA_T *tsa)
{
    if(buf ==NULL || size < 12)
        return;
    sprintf(buf,"%02x%02x%02x%02x%02x%02x",tsa->add[0],tsa->add[1],tsa->add[2],
            tsa->add[3],tsa->add[4],tsa->add[5]);
}

void meterSearchNewDraw(HDC hdc)
{
    
    int             x, y; // 坐标
    int             first;
    int             last;
    int             i; // 循环变量i (first,last是i的起止值)
    char            szAddr[16]; // 以字符串形式输出的表地址
    
    SelectFont(hdc, g_guiComm.fontSmall);
    
    /*标题*/
    ITEM_NOT_SELECT;
    x = 10;
    y = 16;
    TextOut(hdc, x, y, "5-6 新增电能表地址");
    
    /*左半屏幕显示8条地址*/
    first = gMeterSearchPageIndex * 8;
    last = MIN(first + 8, gMeterSearchNewList.num);
    x = 20;
    
    for(i = first; i < last; i++)
    {
        y   = 32 + 12 * (i - first);
        meterAddrConvert(szAddr,sizeof(szAddr), &gMeterSearchNewList.tsa[i]);
        TextOut(hdc, x, y, szAddr);
    }

    
    /*绘制按钮*/
    ITEM_SELECT;
    x = 52;
    y = 130;
    TextOut(hdc, x, y, "启动抄表");
    ITEM_NOT_SELECT;

}

void meterSearchNewTimer(int nId)
{
    static time_t last ;
    //只有在搜表过程中才会10秒查询一次搜表结果
    if(nId == TIMER_MAIN && gMeterSearchStatus == 1 && abs(time(NULL)-last)>10)
    {
        meter_search_result_get(&gMeterSearchNewList,&gMeterSearchNotFoundList);
        gMeterSearchPageMax = gMeterSearchNewList.num/8;
        last = time(NULL);
    }
}

BOOL meterSearchNewKeyMsg(int nKey)
{
    switch(nKey)
    {
        case KEY_UP:
        case KEY_LEFT:
            gMeterSearchPageIndex--;
        
            if(gMeterSearchPageIndex < 0)
            {
                gMeterSearchPageIndex = gMeterSearchPageMax;
            }
        
            break;
        
        case KEY_DOWN:
        case KEY_RIGHT:
            gMeterSearchPageIndex++;
        
            if(gMeterSearchPageIndex > gMeterSearchPageMax)
            {
                gMeterSearchPageIndex = 0;
            }
        
            break;
        case KEY_OK:
        {
            MessageBox("启动新增电表抄表\n请稍候！\n");
            GUI_FMT_DEBUG("启动新增电表抄表\n");
            meterSearchTransInit(&gMeterSearchNewList);
        }
    }

    return FALSE;
}

//5-6新增电能表地址
PLUGIN_INFO     g_meter_search_new =
{
    0x0062000c /*id*/, 
    meterSearchNewInit /*InitLayer*/, 
    NULL /*UninitLayer*/, 
    meterSearchNewDraw /*Draw*/, 
    meterSearchNewTimer /*Timer*/, 
    meterSearchNewKeyMsg /*keymsg*/, 
    NULL                                            /*ipc*/
};

int gMeterSearchReadPageIndex;
uint16 gMeterSearchReadPageMax;

void meterSearchNewReadInit(DWORD dwPluginId)
{
    gMeterSearchReadPageIndex = 0;
    //1页显示8个
    gMeterSearchReadPageMax = gMeterSearchTrans.meternum/8;
}

void meterSearchNewReadDraw(HDC hdc)
{
    
    int             x, y; // 坐标
    int             first;
    int             last;
    int             i; // 循环变量i (first,last是i的起止值)
    char            szAddr[20]; // 以字符串形式输出的表地址

    char            str[100];
    
    SelectFont(hdc, g_guiComm.fontSmall);
    
    /*标题*/
    ITEM_NOT_SELECT;
    x = 10;
    y = 16;
    TextOut(hdc, x, y, "5-7 新增电表抄表状态");
    y += 15;
    TextOut(hdc, x, y, "电能表地址");
    TextOut(hdc, x + 80, y, "抄表示值");
    first = gMeterSearchReadPageIndex * 8;
    last = MIN(first + 8, gMeterSearchTrans.meternum);
    
    x = 5;
    
    for(i = first; i < last; i++)
    {

        y   = 45 + 12 * (i - first);
        meterAddrConvert(szAddr, sizeof(szAddr),&gMeterSearchTrans.transinfo[i].tsa);
        TextOut(hdc, x, y, szAddr);
    
        sprintf(str, "------");
        if(gMeterSearchTrans.transinfo[i].status == CQ_TRANS_SUCC)
            sprintf(str, "%.2f", (float) gMeterSearchTrans.transinfo[i].meterData / 100);
    
        TextOut(hdc, x + 90, y, str);
    }
     

}
extern uint16 app_crc16_get(const uint8* buf, uint32 len);

/*******************************************************************************
* 函数名称: meter_search_read_trans_pack 
* 函数功能: meter_read_frame_pack 
* 输入参数: tsa 地址 buflen 最大长度
* 输出参数: buf 组出来的报文
* 返 回 值: 报文长度
*******************************************************************************/
uint16 meter_read_frame_pack(OOP_TSA_T *tsa,uint8 prtl,uint8 *buf,uint16 buflen)
{
    uint16 len = 0;
    uint16 i;
    uint8 checkSum = 0;
    uint8 frame[1000]={0};
    uint16 crc;
    //07表
    if(prtl == 2)
    {
        frame[len++] = 0x68;
        memcpy_r(&frame[len],tsa->add,6);
        len+=6;
        frame[len++] = 0x68;
        frame[len++] = 0x11;    //读电量
        frame[len++] = 0x04;
        frame[len++] = 0x00 + 0x33;
        frame[len++] = 0x00 + 0x33;
        frame[len++] = 0x01 + 0x33;
        frame[len++] = 0x00 + 0x33;

        for (i = 0; i < len; i++)
        {
            checkSum += frame[i];
        }
        frame[len++] = checkSum;
        frame[len++] = 0x16;   //结束字符
    }else if(prtl == 3)
    {
        //698表
        frame[len++] = 0x68;   //起始符
        //长度
        frame[len++] = 0x17;
        frame[len++] = 0x00;
        
        frame[len++] = 0x43;   //控制域
        frame[len++] = 0x05;   //SA标志
        //SA地址
        memcpy_r(&frame[len],tsa->add,6);
        len+=6;

        frame[len++] = 0x00;   //CA地址

        crc = app_crc16_get(&frame[1], 11);
        frame[len++] = crc & 0xFF;
        frame[len++] = crc >> 8;

        frame[len++] = 0x05;   //读取请求
        frame[len++] = 0x01;   //读取一个对象属性
        frame[len++] = 0x05;   //PIID
        //OAD
        frame[len++] = 0x00;
        frame[len++] = 0x10;
        frame[len++] = 0x02;
        frame[len++] = 0x00;

        frame[len++] = 0x00;   //timeTag

        crc = app_crc16_get(&frame[1], 21);
        frame[len++] = crc & 0xFF;
        frame[len++] = crc >> 8;

        frame[len++] = 0x16;   //结束字符
    }
    if(len > buflen)
        return 0;
    memcpy(buf,frame,len);
    return len;
}

/*******************************************************************************
* 函数名称: meter_search_read_trans_pack 
* 函数功能: 根据表地址和规约组完整的报文 进行透传 走taskManager的0010
* 输入参数: tsa 地址 buflen 最大长度
* 输出参数: buf 组出来的apdu
* 返 回 值: apdu长度
*******************************************************************************/
uint16 meter_search_read_trans_pack(OOP_TSA_T *tsa,uint8 prtl,uint8 *buf,uint16 buflen)
{
    uint8 payload[1024]={0};
    uint16 index = 0;
    uint32 port = 0xF2090201;
    uint16 overtime = 20;   //20秒超时
    uint16 frameovertime = 20;  //字符超时时间 20毫秒
    uint16 framelen = 0;
    uint8 frame[1024]={0};
    memcpy_r(&payload[index],&port,4);
    index+=4;
    index+=2;   //通讯控制字载波不用
    memcpy_r(&payload[index],&overtime,2);
    index+=2;
    memcpy_r(&payload[index],&frameovertime,2);
    index+=2;
    //根据规约组完整的电表报文
    framelen = meter_read_frame_pack(tsa,prtl,frame,sizeof(frame));
    if(framelen == 0)
        return 0;
    GUI_BUF_DEBUG(frame,framelen,"电表 %02x%02x%02x%02x%02x%02x prtl %d 透传报文为\n",tsa->add[0],tsa->add[1],tsa->add[2],tsa->add[3],
                tsa->add[4],tsa->add[5],prtl);
    payload[index++] = framelen;
    memcpy(&payload[index],frame,framelen);
    index+=framelen;
    if(index > buflen)
        return 0;
    memcpy(buf,payload,index);
    return index;

}

void meterSearchNewReadTimer(int nId)
{
    
    time_t          timenow;
    uint16          index = gMeterSearchTrans.meterindex;
    uint16          len = 0;
    uint8           payload[2048] =
    {
        0
    };
    
    if(index >= gMeterSearchTrans.meternum)
        return;
    
    if(nId == TIMER_QUICK)
    {
        if(gMeterSearchTrans.transinfo[index].status == CQ_TRANS_SEND)
        {
            timenow = time(NULL);
    
            if(abs(timenow - gMeterSearchTrans.transinfo[index].sendtime) > 30)
            {
                GUI_FMT_DEBUG("超时未收到taskManager回码\n");
                gMeterSearchTrans.transinfo[index].status = CQ_TRANS_FAIL;
                gMeterSearchTrans.meterindex++;
                index++;
            }
        }
    
        if(gMeterSearchTrans.transinfo[index].status == CQ_TRANS_SUCC || gMeterSearchTrans.transinfo[index].status == CQ_TRANS_FAIL)
        {
            gMeterSearchTrans.meterindex++;
            index++;
        }
    
        if(index >= gMeterSearchTrans.meternum)
            return;
    
        if(gMeterSearchTrans.transinfo[index].status == CQ_TRANS_INIT)
        {
            //初始态时就发送 
            len = meter_search_read_trans_pack(&gMeterSearchTrans.transinfo[index].tsa,gMeterSearchTrans.transinfo[index].prtl, payload, sizeof(payload));
    
            if(len > 0)
            {
                GUI_FMT_DEBUG(" 透传发送 index %d all %d\n",index,gMeterSearchTrans.meternum);
                meter_trans_amr_send_0010(payload, len, &gMeterSearchTrans.transinfo[index].sendIndex);
                gMeterSearchTrans.transinfo[index].status = CQ_TRANS_SEND;
                gMeterSearchTrans.transinfo[index].sendtime = time(NULL);
            }
        }
    
    }
    

}

//5-7 新增电能表抄表状态
PLUGIN_INFO     g_meter_search_new_read =
{
    0x0065000c /*id*/, 
    meterSearchNewReadInit /*InitLayer*/, 
    NULL /*UninitLayer*/, 
    meterSearchNewReadDraw /*Draw*/, 
    meterSearchNewReadTimer /*Timer*/, 
    NULL /*keymsg*/, 
    NULL                                            /*ipc*/
};

int gMeterNotFoundPageIndex;
uint16 gMeterNotFoundPageMax;

void meterNotFoundAddrInit(DWORD dwPluginId)
{
    meter_search_result_get(&gMeterSearchNewList,&gMeterSearchNotFoundList);
    gMeterNotFoundPageIndex = 0;
    //1页显示8个
    gMeterNotFoundPageMax = gMeterSearchNotFoundList.num/8;
}

void meterNotFoundAddrDraw(HDC hdc)
{
    
    int             x, y; // 坐标
    int             first;
    int             last;
    int             i; // 循环变量i (first,last是i的起止值)
    char            szAddr[16]; // 以字符串形式输出的表地址
    
    SelectFont(hdc, g_guiComm.fontSmall);
    
    /*标题*/
    ITEM_NOT_SELECT;
    x = 10;
    y = 16;
    TextOut(hdc, x, y, "5-8 未搜到电能表地址");
    
    /*左半屏幕显示8条地址*/
    first = gMeterNotFoundPageIndex * 8;
    last = MIN(first + 8, gMeterSearchNotFoundList.num);
    x = 20;
    
    for(i = first; i < last; i++)
    {
        y   = 32 + 12 * (i - first);
        meterAddrConvert(szAddr,sizeof(szAddr), &gMeterSearchNotFoundList.tsa[i]);
        TextOut(hdc, x, y, szAddr);
    }

}

void meterNotFoundAddrTimer(int nId)
{
    static time_t last ;
    //只有在搜表过程中才会10秒查询一次搜表结果
    if(nId == TIMER_MAIN && gMeterSearchStatus == 1 && abs(time(NULL)-last)>10)
    {
        meter_search_result_get(&gMeterSearchNewList,&gMeterSearchNotFoundList);
        gMeterNotFoundPageMax = gMeterSearchNotFoundList.num/16;
        last = time(NULL);
    }
}

BOOL meterNotFoundAddrKeyMsg(int nKey)
{
    switch(nKey)
    {
        case KEY_UP:
        case KEY_LEFT:
            gMeterNotFoundPageIndex--;
        
            if(gMeterNotFoundPageIndex < 0)
            {
                gMeterNotFoundPageIndex = gMeterNotFoundPageMax;
            }
        
            break;
        
        case KEY_DOWN:
        case KEY_RIGHT:
            gMeterNotFoundPageIndex++;
        
            if(gMeterNotFoundPageIndex > gMeterNotFoundPageMax)
            {
                gMeterNotFoundPageIndex = 0;
            }
        
            break;
    }

    return FALSE;
}

//5-8 未搜到电能表地址
PLUGIN_INFO     g_meter_not_found_addr =
{
    0x0063000c /*id*/, 
    meterNotFoundAddrInit /*InitLayer*/, 
    NULL /*UninitLayer*/, 
    meterNotFoundAddrDraw /*Draw*/, 
    meterNotFoundAddrTimer /*Timer*/, 
    meterNotFoundAddrKeyMsg /*keymsg*/, 
    NULL                                            /*ipc*/
};



/*********************************************************************
    所属功能单元：调试页面库--主动上报
    功能描述：    调试页面库绘制函数
    输入参数：    HDC hdc：绘制上下文句柄
    输出参数：
    函数返回值：
    内部处理流程：
********************************************************************/
void autoReportDraw(HDC hdc)
{
    const int       nLeft1 = 10;
    const int       nTop = 16;

    SelectFont(hdc, g_guiComm.fontBig);

    /*标题*/
    ITEM_NOT_SELECT;
    TextOut(hdc, nLeft1, nTop, "5-9 主动上报");

    /*绘制按钮*/
    ITEM_SELECT;
    TextOut(hdc, 52, 72, "呼叫主站");
    ITEM_NOT_SELECT;

}

//5-9 主动上报
PLUGIN_INFO     g_auto_report =
{
    0x0064000c /*id*/, 
    NULL /*InitLayer*/, 
    NULL /*UninitLayer*/, 
    autoReportDraw /*Draw*/, 
    NULL /*Timer*/, 
    NULL /*keymsg*/, 
    NULL                                            /*ipc*/
};


/**********************************************************************
* @name      : meter_trans_amr_recv_check
* @brief     ：收到taskManager回复的代理消息时进行判断 如果可以对应发送的 则解析
* @param[in] ：
* @return    ：0 解析成功 -1 不是该界面需要的
* @Date      ：
* @Update    :
**********************************************************************/
int meter_trans_amr_recv_check(MESSAGE_INFO_T *pMsgInfo)
{
    CQ_METER_TRANS_T *transinfo = NULL;
    if(gTransFlag == 1)
    {
        transinfo = &gRealSingleTrans;
    }else if(gTransFlag == 2)
    {
        transinfo = &gAllTrans.transinfo[gAllTrans.meterindex];
    }else
        return -1;
    GUI_BUF_DEBUG(pMsgInfo->playload,pMsgInfo->msglen,"trans recv index %d sendindex %d \n",pMsgInfo->index,transinfo->sendIndex);
    uint8 apdu[2048]={0};
    uint16 apdulen;
    uint16 index = 0;
    uint8 lenoffset;
    OOP_OAD_U recvoad;
    uint8 i;
    uint8 dataflag = 0;
    //如果当前不是已发送 则不需要
    if(transinfo->status != CQ_TRANS_SEND)
        return -1;
    //再判断下收发消息序号
    if(pMsgInfo->index != transinfo->sendIndex)
        return -1;
    apdulen = get_len_offset(pMsgInfo->playload, &lenoffset);
    memcpy_s(apdu,sizeof(apdu),pMsgInfo->playload+lenoffset,apdulen);
    //开始解析apdu 示例如下
    //89 01 00 01 07 05 00 00 00 00 00 02 02 40 00 02 00 01 1c 07 e7 01 1e 0a 1b 0b 00 10 02 00 01 01 05 06 00 03 53 fb 06 00 00 bf ee 06 00 00 bf 76 06 00 00 e6 f4 06 00 00 bf e4 00 00
    if(apdu[index++]!=0x89 || apdu[index++]!=1)
        return -1;
    index++;
    //个数
    if(apdu[index++]!=1)
        return -1;
    //地址 以防万一再对比下
    if(apdu[index++]!=7 || apdu[index++]!=5)
        return -1;
    if(memcmp(&apdu[index],transinfo->tsa.add,6)!=0)
        return -1;
    index+=6;
    //两个resultnormal
    if(apdu[index++]!=2)
        return -1;
    for(i=0;i<2;i++)
    {
        //oad 
        recvoad.value = 0;
        memcpy_r(&recvoad.value,&apdu[index],4);
        index+=4;
        //结果类型 1成功 0错误
        if(apdu[index++]==1)
        {
            if(recvoad.value == 0x40000200)
            {
                if(apdu[index++] != DT_DATETIME_S)
                    return -1;
                memcpy_r(&transinfo->meterTime.year,&apdu[index],2);
                index+=2;
                transinfo->meterTime.month = apdu[index++];
                transinfo->meterTime.day = apdu[index++];
                transinfo->meterTime.hour = apdu[index++];
                transinfo->meterTime.minute = apdu[index++];
                transinfo->meterTime.second = apdu[index++];
                GUI_FMT_DEBUG("电表%d时间 %d/%d/%d %d:%d:%d\n",transinfo->pn,transinfo->meterTime.year,transinfo->meterTime.month,transinfo->meterTime.day,
                        transinfo->meterTime.hour,transinfo->meterTime.minute,transinfo->meterTime.second);
                dataflag = 1;
            }else if(recvoad.value == 0x00100200)
            {
                if(apdu[index++] != DT_ARRAY || apdu[index++] != 5)
                    return -1;
                //只要总的
                if(apdu[index++] != DT_DOUBLE_LONG_UNSIGNED)
                    return -1;
                dataflag = 1;
                memcpy_r(&transinfo->meterData,&apdu[index],4);
                GUI_FMT_DEBUG("电表%d总电能 %d \n",transinfo->pn,transinfo->meterData);
                index+=4;
                //后面的不用了
                index+=20;
            }
        }else
        {
            //一个字节的错误码
            index++;
        }
    }
    if(dataflag == 1)
    {
        transinfo->status = CQ_TRANS_SUCC;
    }else
        transinfo->status = CQ_TRANS_FAIL;
    
    return 0;
}

/**********************************************************************
* @name      : meter_trans_amr_recv_check
* @brief     ：收到taskManager回复的代理消息时进行判断 如果可以对应发送的 则解析
* @param[in] ：
* @return    ：0 解析成功 -1 不是该界面需要的
* @Date      ：
* @Update    :
**********************************************************************/
int meter_trans_amr_recv_check_0010(MESSAGE_INFO_T *pMsgInfo)
{
    CQ_METER_TRANS_T *transinfo = &gMeterSearchTrans.transinfo[gMeterSearchTrans.meterindex];

    GUI_BUF_DEBUG(pMsgInfo->playload,pMsgInfo->msglen,"trans recv index %d sendindex %d \n",pMsgInfo->index,transinfo->sendIndex);
    uint8 frame[2048]={0};

    uint16 framelen,i;
    uint8 lenoffset;
    uint8 dataflag = 0;
    uint8 addr[6]={0};
    uint8 recvdata[4]={0};
    memcpy_r(addr,transinfo->tsa.add,6);
    //如果当前不是已发送 则不需要
    if(transinfo->status != CQ_TRANS_SEND)
        return -1;
    //再判断下收发消息序号
    if(pMsgInfo->index != transinfo->sendIndex)
        return -1;
    framelen = get_len_offset(pMsgInfo->playload, &lenoffset);
    memcpy_s(frame,sizeof(frame),pMsgInfo->playload+lenoffset,framelen);
    //透传可能回的前面带0xfe 要给过滤掉
    for(i=0;i<framelen;i++)
    {
        if(frame[i] == 0x68)
        {
            memmove(frame,&frame[i],framelen-i);
            break;
        }
    }
    //开始解析透传回码报文
    if(transinfo->prtl == 2)
    {
        //07表解析 68 02 00 00 00 00 99 68 91 08 33 33 34 33 3a b4 54 33 46 16
        if(memcmp(addr,&frame[1],6) == 0 && frame[9] == 8 && frame[10] == 0x33 && frame[11] == 0x33&& frame[12] == 0x34&& frame[13] == 0x33)
        {
           recvdata[0] = frame[14] - 0x33;
           recvdata[1] = frame[15] - 0x33;
           recvdata[2] = frame[16] - 0x33;
           recvdata[3] = frame[17] - 0x33;
           transinfo->meterData = BCD_change_to_long(recvdata,8);
           dataflag = 1;
        }
    }else if(transinfo->prtl == 3)
    {
        //698表解析
        uint8 temp[7] ={0x00,0x10,0x02,0x00,0x01,0x01,0x05};
        if(memcmp(addr,&frame[5],6) == 0 && memcmp(temp,&frame[17],7) == 0)
        {
            memcpy_r(&transinfo->meterData,&frame[25],4);
            dataflag = 1;
        }
    }

    if(dataflag == 1)
    {
        GUI_FMT_DEBUG("电表回复的电量为 %d\n",transinfo->meterData);
        transinfo->status = CQ_TRANS_SUCC;
    }else
    {
        GUI_FMT_DEBUG("电表透传失败\n");
        transinfo->status = CQ_TRANS_FAIL;
    }
    
    return 0;
}





//状态栏 信息点号处理
void layerUninit()
{
    set_gui_infonum(0);
}


#if DESC("参数查询——测量点参数查询",1)

//根据信息点号获取档案信息          
//注意，该函数从原configterminal中复制而来给定信息点号
OOP_METER_T MeterFiles[4];
const OOP_OAD_U MeterFileOad = { 0x60000200 };

//规约类型常量
const char* g_prtlName[] = { "未知","97","07","698","188" };


int get_meterfileInfo(uint16 infonum, OOP_METER_T* MeterFile)
{

    int   ret;
    uint32 len = 0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad = MeterFileOad;
    NormalOad.infoNum = infonum;
    OOP_METER_T Meter;
    memset(&Meter, 0, sizeof(OOP_METER_T));
    ret = db_read_nomal(g_hUdbCenter, &NormalOad, sizeof(OOP_METER_T), (uint8*)&Meter, &len);
    if (ret != 0 || len != sizeof(OOP_METER_T))
        return -1;
    memcpy(MeterFile, &Meter, sizeof(OOP_METER_T));
    return 0;


    //测试环境，用于制造假数据
    /*
    OOP_METER_T Meter;
    memset(&Meter, (uint8)infonum, sizeof(OOP_METER_T));
    memcpy(MeterFile, &Meter, sizeof(OOP_METER_T));
    
    return 0;
    */

}


void meterfileInit(HDC hdc)
{

	//这里设置页面序号，从1开始
    g_guiComm.nIndexPoint = 1;
    memset(MeterFiles, 0, sizeof(MeterFiles)/sizeof(MeterFiles[0])*sizeof(OOP_METER_T));
	//每页获取4个测量点
	for(int i = 1;i <=4;i++)
	{
	    get_meterfileInfo(4 * (g_guiComm.nIndexPoint-1) + i, &MeterFiles[i-1]);
	}


    //下列代码段用来测试使用，制造数据
    /*
    g_guiComm.nIndexPoint = 1;

    for(int i=0;i<4;i++)
    {
        MeterFiles[i].basic.protocol = 0x03;
        memset(&MeterFiles[i].basic.tsa.add,g_guiComm.nIndexPoint,sizeof(MeterFiles[i].basic.tsa.add));
    }
    */
    
}


void meterfile_display_draw_t(HDC hdc)
{
    int nTop = 20;
    int nLeft1 = 10;
    //int nLeft2 = 70;
    ITEM_SELECT;
    TextOut(hdc, nLeft1, nTop, "2-1 测量点参数查询");
    ITEM_NOT_SELECT;
    nTop+=20;

    uint8 Addr[6] = { 0 };
    char czBuf[300] = { 0 };
    memcpy(Addr, MeterFiles[0].basic.tsa.add, 6);
    sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);  
    TextOut(hdc, nLeft1, nTop, czBuf);

}

void meterfile_draw(HDC hdc)
{
    int flag = 0;  //标识档案是否存在
    char czBuf[300] = { 0 };

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    set_gui_infonum(g_guiComm.nIndexPoint);

    //解决测量点号变，对应档案没有更新
    //memset(&MeterFile, 0, sizeof(OOP_METER_T));
    //get_meterfile(g_guiComm.nCurPoint, &MeterFile);
	memset(MeterFiles, 0, sizeof(MeterFiles)/sizeof(MeterFiles[0])*sizeof(OOP_METER_T));
	//每页获取4个测量点
	for(int i = 1;i <=4;i++)
	{
	    get_meterfileInfo(4 * (g_guiComm.nIndexPoint-1) + i, &MeterFiles[i-1]);
	}

	int nTop = 20;
    int nLeft1 = 10;
	TextOut(hdc, nLeft1, nTop, "2-1 测量点参数查询");
    nTop+=20;

    //画测量点号
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    TextOut(hdc, nLeft1, nTop, "测量点号");

    nLeft1+=60;
    TextOut(hdc, nLeft1, nTop, "电表地址");

    nLeft1+=60;
    TextOut(hdc, nLeft1, nTop, "规约");


    //打印测量点号
    nLeft1 = 10;
    nTop = 40;
	for(int i =1;i <= 4;i++)
	{
		nTop += 20;
		sprintf(czBuf, "%04d", 4 * (g_guiComm.nIndexPoint-1) + i);
		TextOut(hdc, nLeft1, nTop, czBuf);
		memset(czBuf, 0x00, sizeof(czBuf));
	}
    

    //画电表地址
    nLeft1=50;
    nTop = 60;
    for(int i = 0 ;i<4;i++)
    {  
        flag = 0;
        if (MeterFiles[i].nIndex != 0)  //由于先给nIndex设0，读出来的就非0，没读出来就还是0
            flag = 1;
        memset(czBuf, 0x00, sizeof(czBuf));
        if (flag == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {
            uint8 Addr[6] = { 0 };
            memcpy(Addr, MeterFiles[i].basic.tsa.add, 6);
            sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
        }
        TextOut(hdc, nLeft1, nTop, czBuf);
        nTop += 20;
        
    }

    //画规约
    nTop = 60;
    nLeft1 = 130;
    for(int i = 0;i < 4;i++)
    {
        
        flag = 0;
        memset(czBuf, 0x00, sizeof(czBuf));
         if (MeterFiles[i].nIndex != 0)
            flag = 1;
         if (flag == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {
            uint8 prtl = MeterFiles[i].basic.protocol;
            if (prtl > 4)
                prtl = 0;
            sprintf(czBuf, "%s", g_prtlName[prtl]);
        }
        TextOut(hdc, nLeft1, nTop, czBuf);
        nTop+=20;
    }
 
}


BOOL meterfile_keymsg(int nKey)
{
    //int MAX=16;
    int MAX = 2048/4; // 总共2048个档案，每页显示4个

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_guiComm.nIndexPoint > 0)
                g_guiComm.nIndexPoint--;
            if (g_guiComm.nIndexPoint == 0)
                g_guiComm.nIndexPoint = MAX;

           
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_guiComm.nIndexPoint < MAX)
            {
                g_guiComm.nIndexPoint++;
            }
            else
            {
                g_guiComm.nIndexPoint = 1;
            }
           
            
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
       
    }
    return FALSE;
}


//电表档案查看
//PLUGIN_INFO g_layer_meterfile = {
//    //0x11030013/*id*/,
//    0x0020000c/*id*/,
//    meterfileInit/*InitLayer*/,
//    layerUninit/*UninitLayer*/,
//    meterfile_draw/*Draw*/,
//    NULL/*Timer*/,
//    meterfile_keymsg/*keymsg*/,
//    NULL/*ipc*/
//};

#endif


int cmd_exec(const char* cmd,  char* result)
{
    char resultBuf[500];

    FILE* fp = NULL;

    if ((fp = popen(cmd, "r")) != NULL)
    {
        while( fgets(resultBuf, sizeof(resultBuf), fp) != NULL) 
        {
            strcat(result, resultBuf);
            if( strlen(result) > sizeof(resultBuf))
            {
                break;
            }
        }
        pclose(fp);
        fp = NULL;
    }
    else
    {
        return -1;
    }
    return 0;
}



#if DESC("参数查询--测量点参数查询(只展示有效测量点)",1)

OOP_METER g_allMeters;
OOP_METER_T allValidMeters[2048];
uint32 allValidMetersNum;
uint32 validMeterspageNum;


void appdb_meter_read_all(int clientid)
{
    int                ret = ERR_NORMAL;
    uint32             i = 0;
    uint32             oopMeterNum = 0;
    OOP_METER_T        oopMeter = {0};
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;

    memset(&g_allMeters, 0, sizeof(OOP_METER));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = NO_VIR_MTR_MAX_NUM + 1;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60000200;

    //不需要附加校验
    stepReadRecord.MAC.nNum = 0;
    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        GUI_FMT_DEBUG("appdb_meter_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                GUI_FMT_DEBUG("appdb_meter_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //检查数据有效性
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x60000200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_METER_T)))
            {
                GUI_FMT_DEBUG("appdb_meter_list_record_init data err !\n");
                continue;
            }

            memcpy(&oopMeter, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);

            g_allMeters.nValue[oopMeterNum] = oopMeter.nIndex;
            //档案序号从1开始
            g_allMeters.oopMeter[oopMeterNum++] = oopMeter;
            
        }
    }
    db_read_record_end(clientid, handle);

    /* 记录档案数量 */
    g_allMeters.nNum = oopMeterNum;

    return;
}


void validMeterFileInit(HDC hdc)
{
    appdb_meter_read_all(g_hUdbCenter);
    allValidMetersNum=0;
    
    memset(allValidMeters,0,sizeof(OOP_METER_T)*2048);
    for(uint32 i = 0;i<g_allMeters.nNum;i++)
    {
        if(g_allMeters.oopMeter[i].nIndex!=0)
        {
            allValidMeters[allValidMetersNum++]=g_allMeters.oopMeter[i];
        }
    }
    
    validMeterspageNum = allValidMetersNum%4==0? allValidMetersNum/4 : allValidMetersNum/4+1;
    g_guiComm.nIndexPoint = 1;
    
}

void validMeterFilesDraw(HDC hdc)
{
    char czBuf[300] = { 0 };

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    
    int nTop = 20;
    int nLeft1 = 10;
	TextOut(hdc, nLeft1, nTop, "2-1 测量点参数查询");
    nTop+=20;

    //画测量点号
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    TextOut(hdc, nLeft1, nTop, "测量点号");

    nLeft1+=60;
    TextOut(hdc, nLeft1, nTop, "电表地址");

    nLeft1+=60;
    TextOut(hdc, nLeft1, nTop, "规约");


    
    if(g_allMeters.nNum>0)
    {
        nLeft1 = 10;
        nTop = 60;
        for(uint32 i=0;i<4;i++)
        {    
            if(4*(g_guiComm.nIndexPoint-1)+i <= allValidMetersNum-1)
            {
                //打印测量点号
                
                sprintf(czBuf, "%04u", allValidMeters[4*(g_guiComm.nIndexPoint-1)+i].nIndex);
		        TextOut(hdc, nLeft1, nTop, czBuf);
		        memset(czBuf, 0x00, sizeof(czBuf));

                //打印电表地址
                nLeft1+=40;
                uint8 Addr[6] = { 0 };
                memcpy(Addr, allValidMeters[4*(g_guiComm.nIndexPoint-1)+i].basic.tsa.add, 6);
                sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
                TextOut(hdc, nLeft1, nTop, czBuf);
                memset(czBuf, 0x00, sizeof(czBuf));

            
                //打印电表规约
                nLeft1+=80;
                uint8 prtl = allValidMeters[4*(g_guiComm.nIndexPoint-1)+i].basic.protocol;
                if (prtl > 4)
                    prtl = 0;
                sprintf(czBuf, "%s", g_prtlName[prtl]);
                TextOut(hdc, nLeft1, nTop, czBuf);
                memset(czBuf, 0x00, sizeof(czBuf));
      
            }
            else
                break;
            nTop+=20;
            nLeft1=10;
        }
        
    }
    
}


BOOL validMeterFilesKeyMSG(int key)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;

    switch (key)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_guiComm.nIndexPoint > 0)
                g_guiComm.nIndexPoint--;
            if (g_guiComm.nIndexPoint == 0)
                g_guiComm.nIndexPoint = validMeterspageNum;

           
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if ((uint32)g_guiComm.nIndexPoint < validMeterspageNum)
            {
                g_guiComm.nIndexPoint++;
            }
            else
            {
                g_guiComm.nIndexPoint = 1;
            }
           
            
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
       
    }
    return FALSE;

}
//电表档案查看
PLUGIN_INFO g_layer_meterfile = {
    //0x11030013/*id*/,
    0x0020000c/*id*/,
    validMeterFileInit/*InitLayer*/,
    layerUninit/*UninitLayer*/,
    validMeterFilesDraw/*Draw*/,
    NULL/*Timer*/,
    validMeterFilesKeyMSG/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("参数查询——通信参数查询", 1)

const OOP_OAD_U Ethip_Oad = { 0x45100400 };
const OOP_OAD_U EthMasterip_Oad = { 0x45100300 };
const OOP_OAD_U WirelessMaster_Oad = { 0x45000300 };



OOP_ETHIP_T Eth_IP;      //以太网ip
OOP_MASTERPARAMS_T EthMaster_IP;      //以太网主站ip
OOP_MASTERPARAMS_T wirelessMaster;  //无线公网主站ip
OOP_GPRSCONFIG_T wirelessConfig;    //无线公网的配置信息，包含APN


int EthIPSet;
int EthMasterIPSet;
int wirelessFlag;
int verInfoFlag;
int g_ethIndex;
uint32 g_gprsIP[4]={0};

OOP_VERSION_S_T g_tguiVer;


void comm_display_init(HDC hdc)
{
    //在数据库读以太网ip
    memset(&Eth_IP, 0x00, sizeof(OOP_ETHIP_T));
    EthIPSet = read_db_noamal_data(g_hUdbCenter, Ethip_Oad.value, 0, 0, (uint8*)&Eth_IP, sizeof(OOP_ETHIP_T));

    //在数据库读以太网主站ip和端口
    memset(&EthMaster_IP,0x00,sizeof(OOP_MASTERPARAMS_T));
    EthMasterIPSet = read_db_noamal_data(g_hUdbCenter, EthMasterip_Oad.value, 0, 0, (uint8*)&EthMaster_IP, sizeof(OOP_MASTERPARAMS_T));

    //在数据库读无线网主站ip和端口
    memset(&wirelessMaster, 0x00, sizeof(OOP_MASTERPARAMS_T));
    read_db_noamal_data(g_hUdbCenter, WirelessMaster_Oad.value, 0, 0, (uint8*)&wirelessMaster, sizeof(OOP_MASTERPARAMS_T));

    //无线公网的终端ip需要通过系统命令获取
    


    //使用系统命令来获取终端ip地址
    
    char cmd[200];
    char result[100];
    memset(cmd,0x00,sizeof(cmd));
    memset(result,0x00,sizeof(result));
    memset(g_gprsIP,0x00,sizeof(g_gprsIP));
    sprintf(cmd, "ifconfig %s | grep 'inet addr' | awk '{ print $2}' | awk -F: '{print $2}' ", "ppp0");
    cmd_exec(cmd,result);
    sscanf(result,"%u.%u.%u.%u",(uint32*)&g_gprsIP[0],(uint32*)&g_gprsIP[1],(uint32*)&g_gprsIP[2],(uint32*)&g_gprsIP[3]);

    //读取无线公网的APN
    wirelessFlag = read_db_noamal_data(g_hUdbCenter, 0x45000200, 0, 0, (uint8*)&wirelessConfig, sizeof(OOP_GPRSCONFIG_T));

    //读取程序软件版本
    memset(&g_tguiVer, 0, sizeof(OOP_VERSION_S_T));
    verInfoFlag = read_db_noamal_data(g_hUdbCenter, 0x4300A300, 0, 0, (uint8*)&g_tguiVer, sizeof(OOP_VERSION_S_T));
    if (verInfoFlag != 0)
    {
        sleep(1);
        verInfoFlag = read_db_noamal_data(g_hUdbCenter, 0x4300A300, 0, 0, (uint8*)&g_tguiVer, sizeof(OOP_VERSION_S_T));
    }

	g_ethIndex = 0;
}



void ethComm_draw(HDC hdc)
{
    int nLeft = 10;
    int nTop = 20;
    char czBuf[300] = {0};
    
    sprintf(czBuf, "2-2通信参数查询");
    TextOut(hdc, nLeft, nTop, czBuf);


    nTop+=20;
    memset(czBuf,0,300);
    sprintf(czBuf, "以太网主站ip以及接入端口");
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop+=20;
    memset(czBuf,0,300);
    sprintf(czBuf, "%d.%d.%d.%d:%d", EthMaster_IP.master[0].ip[0], EthMaster_IP.master[0].ip[1], EthMaster_IP.master[0].ip[2], EthMaster_IP.master[0].ip[3],EthMaster_IP.master[0].port);
    TextOut(hdc, nLeft, nTop, czBuf);

    
//    nLeft+=95;
//    memset(czBuf,0,300);
//    sprintf(czBuf, ":%d", EthMaster_IP.master[0].port);
//    TextOut(hdc, nLeft, nTop, czBuf);


    nLeft = 10;
    nTop = 80;
    memset(czBuf,0,300);
    sprintf(czBuf, "终端以太网ip地址");
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop+=20;
    memset(czBuf,0,300);
    sprintf(czBuf, "%d.%d.%d.%d",Eth_IP.ip[0],Eth_IP.ip[1],Eth_IP.ip[2],Eth_IP.ip[3]);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop+=20;
    memset(czBuf,0x00,300);
    if(verInfoFlag!=0)
    {
        sprintf(czBuf, "程序版本:----");
        TextOut(hdc, nLeft, nTop, czBuf);
    }
    else
    {
        sprintf(czBuf, "程序版本:");
        TextOut(hdc, nLeft, nTop, czBuf);
        memset(czBuf,0X00,300);
        nLeft+=60;
        memcpy(czBuf,g_tguiVer.szSoftVer.value,4);
        TextOut(hdc, nLeft, nTop, czBuf);
    }

//    nLeft+= 30;
//    memset(czBuf,0x00,300);
//    if(wirelessFlag!=0 || wirelessConfig.apn.apn.nNum==0)
//    {
//        sprintf(czBuf, "APN: ----");
//        TextOut(hdc, nLeft, nTop, czBuf);
//    }
//     else
//    {
//        sprintf(czBuf, "APN:");
//        TextOut(hdc, nLeft, nTop, czBuf);
//        memset(czBuf,0x00,300);
//        nLeft += 30;
//        memcpy(czBuf,wirelessConfig.apn.apn.value,wirelessConfig.apn.apn.nNum);
//        TextOut(hdc, nLeft, nTop, czBuf);
//    } 
}


void wirelessComm_draw(HDC hdc)
{
    int nLeft = 10;
    int nTop = 20;
    char czBuf[300] = {0};
    
    SelectFont(hdc,g_guiComm.fontSmall);
    
    sprintf(czBuf, "2-2通信参数查询");
    TextOut(hdc, nLeft, nTop, czBuf);


    nTop+=20;
    memset(czBuf,0,300);
    sprintf(czBuf, "无线主站ip以及接入端口");
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop+=20;
    memset(czBuf,0,300);
    if(wirelessMaster.master[0].ip[0]==0)
    {
        sprintf(czBuf, "-.-.-.-");
        TextOut(hdc, nLeft, nTop, czBuf);
    }
//    else
//    {
//        sprintf(czBuf, "%d.%d.%d.%d", wirelessMaster.master[0].ip[0], wirelessMaster.master[0].ip[1], wirelessMaster.master[0].ip[2], wirelessMaster.master[0].ip[3]);
//        TextOut(hdc, nLeft, nTop, czBuf);   
//    }
    else if(wirelessMaster.master[0].ip[0]==0)
    {
        sprintf(czBuf, "%d.%d.%d.%d:--", wirelessMaster.master[0].ip[0], wirelessMaster.master[0].ip[1], wirelessMaster.master[0].ip[2], wirelessMaster.master[0].ip[3]);
        TextOut(hdc, nLeft, nTop, czBuf);
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d:%d", wirelessMaster.master[0].ip[0], wirelessMaster.master[0].ip[1], wirelessMaster.master[0].ip[2], wirelessMaster.master[0].ip[3],wirelessMaster.master[0].port);
        TextOut(hdc, nLeft, nTop, czBuf);

    }
    
    
//    nLeft+=80;
//    memset(czBuf,0,300);
//    if(wirelessMaster.master[0].ip[0]==0)
//    {
//        sprintf(czBuf, ":-");
//        TextOut(hdc, nLeft, nTop, czBuf);
//    }
//    else
//    {
//        sprintf(czBuf, ":%d", wirelessMaster.master[0].port);
//        TextOut(hdc, nLeft, nTop, czBuf);
//    }
    


    nLeft = 10;
    nTop = 80;
    memset(czBuf,0,300);
    sprintf(czBuf, "终端无线ip地址");
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop+=20;
    memset(czBuf,0,300);
    if(g_gprsIP[0]==0)
    {
        sprintf(czBuf, "-.-.-.-");
        TextOut(hdc, nLeft, nTop, czBuf);
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d",g_gprsIP[0],g_gprsIP[1],g_gprsIP[2],g_gprsIP[3]);
        TextOut(hdc, nLeft, nTop, czBuf);
    }

//    nTop+=20;
//    memset(czBuf,0x00,300);
//    if(verInfoFlag!=0)
//    {
//        sprintf(czBuf, "程序版本:----");
//        TextOut(hdc, nLeft, nTop, czBuf);
//    }
//    else
//    {
//        sprintf(czBuf, "程序版本:");
//        TextOut(hdc, nLeft, nTop, czBuf);
//        memset(czBuf,0X00,300);
//        nLeft+=60;
//        memcpy(czBuf,g_tguiVer.softVer,4);
//        TextOut(hdc, nLeft, nTop, czBuf);
//    }

    nTop+= 20;
    memset(czBuf,0x00,300);
    if(wirelessFlag!=0 || wirelessConfig.apn.apn.nNum==0)
    {
        sprintf(czBuf, "APN: ----");
        TextOut(hdc, nLeft, nTop, czBuf);
    }
     else
    {
        sprintf(czBuf, "APN:");
        TextOut(hdc, nLeft, nTop, czBuf);
        memset(czBuf,0x00,300);
        nLeft += 30;
        memcpy(czBuf,wirelessConfig.apn.apn.value,wirelessConfig.apn.apn.nNum);
        TextOut(hdc, nLeft, nTop, czBuf);
    } 
}

void comm_draw(HDC hdc)
{
    if(g_ethIndex==0)
        wirelessComm_draw(hdc);
    else
        ethComm_draw(hdc);
}

BOOL comm_key(int key)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    
    switch (key)
    {
        case KEY_UP:
        case KEY_DOWN:
        case KEY_RIGHT:
        case KEY_LEFT:
        {
            if(g_ethIndex==1)
                g_ethIndex = 0;
            else if(g_ethIndex ==0)
                g_ethIndex = 1;

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        } 
    }
    return FALSE;

}


//以太网通道查看
PLUGIN_INFO g_ipDisplay = {
    0x0021000c/*id*/,
    comm_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    comm_draw/*Draw*/,
    NULL/*Timer*/,
    comm_key/*keymsg*/,
    NULL/*ipc*/
};


#endif



#if DESC("参数查询————本地模块",1)

//模块数据使用f2090200来查到
//使用下面两个结构来存
OOP_ROUTEMODULE_T datalist;
ROUTEMODULE moduleInfo;
const OOP_OAD_U module_Oad = { 0xF2090200 };
int moduleFlag = 0;
const char *weekText[]={"日","一","二","三","四","五","六"};
int msgRtn;

uint8 moduleInsFlag; 

//extern WIRELESS_INFO_T ModuleInfoDisp;


void localModule_Init(HDC hdc)
{
    //读出本地模块信息 
    memset(&moduleInfo, 0x00, sizeof(ROUTEMODULE));
    moduleFlag = read_db_noamal_data(g_hUdbCenter, module_Oad.value, 0, 1/*这里用1来获取*/, (uint8*)&moduleInfo, sizeof(ROUTEMODULE));

    //发消息给wireless,读4G版本信息
    uint8 m = 1;
    msgRtn=gui_send_wirelessmsg(0, 0x0010,&m,1);

    //msgRtn = read_db_noamal_data(g_hUdbCenter,0xfefe0202,0,0,&moduleInsFlag,sizeof(moduleInsFlag));

}
 
void localModule_Draw(HDC hdc)
{
    int nTop = 20;
    int nLeft1 = 10;
    char czBuf[300] = { 0 };
    TextOut(hdc, nLeft1, nTop, "2-3 本地模块信息");
    

    nLeft1 = 10;
    nTop = 40;
    
    sprintf(czBuf,"本地模块厂家:");
    TextOut(hdc, nLeft1, nTop, czBuf);

    nLeft1+=80;
    memset(czBuf,0x00,300);
    if(moduleFlag!=0)
    {
        sprintf(czBuf,"-------");
        TextOut(hdc, nLeft1, nTop, czBuf);  
    }
    else
    {
        if(moduleInfo.verInfo.factory.nNum>0)
        {        
            memcpy(czBuf,moduleInfo.verInfo.factory.value,moduleInfo.verInfo.factory.nNum);
        }
        else
        {
            sprintf(czBuf,"------");
        }
        TextOut(hdc, nLeft1, nTop, czBuf);
    }


    nTop = 60;
    nLeft1 = 10;
    memset(czBuf,0x00,300);
    sprintf(czBuf,"模块软件版本:");
    TextOut(hdc, nLeft1, nTop, czBuf);

    memset(czBuf,0x00,300);
    nLeft1+=80;
    if(moduleFlag!=0)
    {
        sprintf(czBuf,"-------");
        TextOut(hdc, nLeft1, nTop, czBuf);  
    }
    else
    {
        sprintf(czBuf,"%u",moduleInfo.verInfo.wVer);
        TextOut(hdc, nLeft1, nTop, czBuf);
    }


    nLeft1 = 10;
    nTop = 80;
    memset(czBuf,0x00,300);
    sprintf(czBuf,"软件版本日期:");
    TextOut(hdc, nLeft1, nTop, czBuf);

    nTop=100;
    nLeft1= 10;
    memset(czBuf,0x00,300);
    sprintf(czBuf,"%u年%u月%u日  周%s",moduleInfo.verInfo.dtVer.year,moduleInfo.verInfo.dtVer.month,moduleInfo.verInfo.dtVer.mday,/*moduleInfo.verInfo.dtVer.wday*/ weekText[moduleInfo.verInfo.dtVer.wday]);
    TextOut(hdc, nLeft1, nTop, czBuf);

    nLeft1 = 10;
    nTop = 120;
    memset(czBuf,0x00,300);
    sprintf(czBuf,"GPRS模块版本:%c%c%c%c",ModuleInfoDisp.version.softversion[0],ModuleInfoDisp.version.softversion[1],
                                            ModuleInfoDisp.version.softversion[2],ModuleInfoDisp.version.softversion[3]);
    TextOut(hdc, nLeft1, nTop, czBuf);
}


//本地模块信息查看
PLUGIN_INFO g_moduleInfo = {
    0x0022000c/*id*/,
    localModule_Init/*InitLayer*/,
    NULL/*UninitLayer*/,
    localModule_Draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
    
#endif


#if DESC("参数查询————设备事件查询",1)


struct tm tmPtr;
int d_rtn = 0;
int onOFFflag = 0;
const OOP_OAD_U sysTm_Oad = { 0xfefe0101 };
OOP_POWERLOG_T *g_powerEventlist;
OOP_POWERLOG_T g_powerEvent;
uint32 g_onOFFEvent_num;
int    g_onOFFEvent_index;


void readEventData(DB_CLIENT fp, uint32 oad_value, uint8** p_event_list, uint32 event_len)
{
    int                ret = ERR_NORMAL;
    time_t             timep   = 0;
    uint32             handle  = 0;
    uint8*             ptmp    =  *p_event_list;

    READ_RECORD_T      inReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    STEP_READ_RECORD_T stepReadRecord;

    g_onOFFEvent_num = 0;
    g_onOFFEvent_index = 0;
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    time(&timep); //获取从1970至今过了多少秒，存入time_t类型的timep

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0;
    inReadRecord.cType = COL_NO_STORE;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = timep;
    inReadRecord.sortType = DOWN_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = oad_value;

     ret = db_read_record_start(fp, &inReadRecord, &handle, &g_onOFFEvent_num);
    if (0 != ret || 0 == g_onOFFEvent_num)
    {
        GUI_FMT_DEBUG(" db_read_record_start  ret=%d, num%d!\n", ret, g_onOFFEvent_num);
        return ;
    }

    ptmp = (uint8*)malloc(event_len*g_onOFFEvent_num);
    if (ptmp == NULL)
    {
        GUI_FMT_DEBUG(" malloc  err !\n");
        g_onOFFEvent_num = 0;
        return ;
    }
    memset(ptmp, 0, event_len*g_onOFFEvent_num);
    
    memcpy(&stepReadRecord.MAC, &inReadRecord.MAC, sizeof(OOP_OCTETVAR64_T));

    for(uint32 i=0; i<g_onOFFEvent_num; i++)
    {
        /* 读最近一条记录 */
        ret = db_read_record_step(fp, handle, &stepReadRecord, &stepRecordOut);
        if (0 == ret)
        {
            memcpy(ptmp+(i*event_len), stepRecordOut.record[0].outDataBuf, event_len);
        }
    }
    *p_event_list = ptmp;

    /*结束读记录*/
     db_read_record_end(fp, handle);
}




void deviceEvent_Init(HDC hdc)
{
    //从数据库读出时间
    d_rtn = read_db_noamal_data(g_hUdbCenter, sysTm_Oad.value, 0, 0, (uint8*)&tmPtr, sizeof(struct tm));

    //读出停上电记录
     g_powerEventlist = NULL;
    readEventData(g_hUdbCenter, 0x31060200, (uint8**)&g_powerEventlist, sizeof(OOP_POWERLOG_T));

}


void deviceEvent_Display(HDC hdc)
{
    int nTop = 20;
    int nLeft1 = 10;
    TextOut(hdc, nLeft1, nTop, "2-5 设备事件查询");
    
    nTop+=20;
    char czBuf[300] = { 0 };
    sprintf(czBuf, "最近测量点参数更新时间");
    TextOut(hdc, nLeft1, nTop, czBuf);
    
    //uint16 sysTm[6];
    memset(czBuf,0x00,300);
    nTop+=20;
    if(d_rtn !=0)
    {
        sprintf(czBuf, "_:_:_:_");
        TextOut(hdc, nLeft1, nTop, czBuf);
    }
    else
    {
        sprintf(czBuf, "%d年%d月%d日 %02d:%02d:%02d",1900+tmPtr.tm_year,1+tmPtr.tm_mon,tmPtr.tm_mday,8+tmPtr.tm_hour,tmPtr.tm_min,tmPtr.tm_sec);
        TextOut(hdc, nLeft1, nTop, czBuf);
    }

    nLeft1 = 10;
    nTop +=40;
    memset(czBuf,0x00,300);
    sprintf(czBuf, "最近设备上电时间");
    TextOut(hdc, nLeft1, nTop, czBuf);


    nTop+=20;
    memset(czBuf,0x00,300);
    if(g_onOFFEvent_num > 0)
    {
        //sprintf(czBuf, "%u",g_onOFFEvent_num);
        //TextOut(hdc, nLeft1, nTop, czBuf);

        for(uint32 i =1;i<=g_onOFFEvent_num;i++)
        {
            g_powerEvent = g_powerEventlist[i-1];
            if(g_powerEvent.source.nValue == 1/*上电事件*/)
            {
//                sprintf(czBuf, "%u年%u月%u日 %02u:%02u:%02u",g_powerEvent.tmStart.year,g_powerEvent.tmStart.month,g_powerEvent.tmStart.day,
//                                                               g_powerEvent.tmStart.hour,g_powerEvent.tmStart.minute,g_powerEvent.tmStart.second);
                sprintf(czBuf, "%u年%u月%u日 %02u:%02u:%02u",g_powerEvent.tmEnd.year,g_powerEvent.tmEnd.month,g_powerEvent.tmEnd.day,
                                                               g_powerEvent.tmEnd.hour,g_powerEvent.tmEnd.minute,g_powerEvent.tmEnd.second);


                TextOut(hdc, nLeft1, nTop, czBuf);
                break;
            }
        }
    }
    else
    {
        sprintf(czBuf, "无事件");
        TextOut(hdc, nLeft1, nTop, czBuf);
    }
}

//该函数用来测试使用,
void deviceEvent_Display2(HDC hdc)
{
    int nTop = 20;
    int nLeft1 = 10;

    char czBuf[300] = { 0 };

    sprintf(czBuf, "g_onOFF_NUM = %u",g_onOFFEvent_num);
    memset(czBuf,0x00,300);
    
    if(g_onOFFEvent_num > 0)
    {
        
        sprintf(czBuf, "%u",g_onOFFEvent_num);
        TextOut(hdc, nLeft1, nTop, czBuf);

        for(uint32 i =1;i<=g_onOFFEvent_num;i++)
        {
            nTop+=20;
            g_powerEvent = g_powerEventlist[i-1];
            if(g_powerEvent.source.nValue == 1/*上电事件*/)
            {
                sprintf(czBuf, "%u年%u月%u日 %02u:%02u:%02u",g_powerEvent.tmStart.year,g_powerEvent.tmStart.month,g_powerEvent.tmStart.day,
                                                               g_powerEvent.tmStart.hour,g_powerEvent.tmStart.minute,g_powerEvent.tmStart.second);
                TextOut(hdc, nLeft1, nTop, czBuf);

                nTop+=20;
                memset(czBuf,0,300);
                sprintf(czBuf, "%u年%u月%u日 %02u:%02u:%02u",g_powerEvent.tmEnd.year,g_powerEvent.tmEnd.month,g_powerEvent.tmEnd.day,
                                                               g_powerEvent.tmEnd.hour,g_powerEvent.tmEnd.minute,g_powerEvent.tmEnd.second);
                TextOut(hdc, nLeft1, nTop, czBuf);
                break;
            }
        }
    }
    else
    {
        sprintf(czBuf, "无事件");
        TextOut(hdc, nLeft1, nTop, czBuf);
    }

}


//设备事件查看
PLUGIN_INFO g_deviceEvent = {
    0x0023000c/*id*/,
    deviceEvent_Init/*InitLayer*/,
    NULL/*UninitLayer*/,
    deviceEvent_Display/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

#endif


#if DESC("中继路由信息查询",1)
//这里先做出页面
void relayRouteInit(HDC hdc)
{

}

void relayRouteDraw(HDC hdc)
{
    int nTop = 20;
    int nLeft1 = 10;
    TextOut(hdc, nLeft1, nTop, "2-4 中继路由信息查询");
    
    nTop+=20;
    char czBuf[300] = { 0 };
    sprintf(czBuf, "1级中继电表信息");
    TextOut(hdc, nLeft1, nTop, czBuf);
    
    nTop+=20;
    memset(czBuf,0x00,300);
    TextOut(hdc, nLeft1, nTop, "测量点号");

    nLeft1+=50;
    memset(czBuf,0x00,300);
    TextOut(hdc, nLeft1, nTop, "上级中继测量点号");

}

//中继路由查看
PLUGIN_INFO g_relayRouteDisplay = {
    0x0025000c/*id*/,
    relayRouteInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    relayRouteDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};


#endif


#if DESC("参数查询————终端版本信息",1)

void terminalVersionInfoInit(HDC hdc)
{
    comm_display_init(hdc);

}

void terminalVersionDraw(HDC hdc)
{
    int nTop = 20;
    int nLeft1 = 5;
    int nLeft2 = 108;
    TextOut(hdc, nLeft1, nTop, "2-6 终端版本信息");
    
    nTop+=20;
    char czBuf[300] = { 0 };
    sprintf(czBuf, "终端软件版本");
    TextOut(hdc, nLeft1, nTop, czBuf);

    memset(czBuf,0x00,300);
    memcpy(czBuf,g_tguiVer.szSoftVer.value,4);
    TextOut(hdc, nLeft2, nTop, czBuf);
    
    nTop+=20;
    memset(czBuf,0x00,300);
    TextOut(hdc, nLeft1, nTop, "软件版本发布日期:");

    memset(czBuf,0x00,300);
    //memcpy(czBuf,g_tguiVer.softDate,6);
    sprintf(czBuf,"%c%c/%c%c/%c%c",g_tguiVer.szSoftDate.value[0],g_tguiVer.szSoftDate.value[1],g_tguiVer.szSoftDate.value[2],
                                   g_tguiVer.szSoftDate.value[3],g_tguiVer.szSoftDate.value[4],g_tguiVer.szSoftDate.value[5]);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop+=20;
    memset(czBuf,0x00,300);
    TextOut(hdc, nLeft1, nTop, "终端硬件版本");

    memset(czBuf,0x00,300);
    memcpy(czBuf,g_tguiVer.szHardVer.value,4);
    TextOut(hdc, nLeft2, nTop, czBuf);
    

    nTop+=20;
    memset(czBuf,0x00,300);
    TextOut(hdc, nLeft1, nTop, "硬件版本发布日期:");

    memset(czBuf,0x00,300);
    //memcpy(czBuf,g_tguiVer.hardDate,6);
    sprintf(czBuf,"%c%c/%c%c/%c%c",g_tguiVer.szHardDate.value[0],g_tguiVer.szHardDate.value[1],g_tguiVer.szHardDate.value[2],
                                   g_tguiVer.szHardDate.value[3],g_tguiVer.szHardDate.value[4],g_tguiVer.szHardDate.value[5]);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop+=20;
    memset(czBuf,0x00,300);
    TextOut(hdc, nLeft1, nTop, "厂家扩展信息");

    memset(czBuf,0x00,300);
    memcpy(czBuf,g_tguiVer.szExtend.value,8);
    TextOut(hdc, nLeft2, nTop, czBuf);
    
}

//终端版本信息
PLUGIN_INFO g_trmlVerinfo = {
    0x0024000c/*id*/,
    terminalVersionInfoInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    terminalVersionDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
#endif


#if DESC("参数查询----终端编号查询",1)

int snRtn;
OOP_OCTETVAR16_T trmSN;



void terminalSNQueryInit(HDC hdc)
{
    memset(&trmSN,0x00,sizeof(OOP_OCTETVAR16_T));
    snRtn = read_db_noamal_data(g_hUdbCenter, 0x40030200, 0, 0, (uint8*)&trmSN, sizeof(OOP_OCTETVAR16_T));
}

void terminalSNQueryDraw(HDC hdc)
{
    int nTop = 20;
    int nLeft1 = 10;
    TextOut(hdc, nLeft1, nTop, "2-7 终端编号查询");

    //SelectFont(hdc,g_guiComm.fontBig);
    char czBuf[50]={0};


    nTop=60;
    nLeft1=50;
    TextOut(hdc, nLeft1, nTop, "设备出厂编号");


    
    nTop=100;
    nLeft1 = 20;
    TextOut(hdc, nLeft1, nTop, "设备编号");

    
    memset(czBuf,0x00,sizeof(czBuf));

    nLeft1 +=60;
    if(snRtn!=0)
    {
        TextOut(hdc, nLeft1, nTop, "未设置终端编号");
    }
    else
    {
        for(uint32 i=0;i<trmSN.nNum;i++)
        {
            sprintf(czBuf,"%02x",trmSN.value[i]);
            TextOut(hdc, nLeft1, nTop, czBuf);
            nLeft1+=12;
        }
    }
}

//终端编号查询
PLUGIN_INFO g_trmSNinfo = {
    0x0026000c/*id*/,
    terminalSNQueryInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    terminalSNQueryDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};



#endif




/************************************************************************/
/* 重庆集中器中通信参数设置 (4-1无线公网参数设置)                                             */
/*                                                                      */
/************************************************************************/
uint8 WirelessCommIndex;
OOP_MASTERPARAMS_T GPRSMasterDB;
OOP_GPRSCONFIG_T GprsConfige;
int GprsConfigeIsSet;
extern int write_normal_data(uint8* DataBuf, uint16 DataLen, OOP_OAD_U Oad, uint16 infonum, uint8 logicid, uint8 classtag);
extern BOOL commfun_IP(char* ipin, uint8* ipout);
int apnSetFlag = 0;

void wirelessComm_set_init(HDC hdc)
{
    GUI_FMT_DEBUG("wirelessComm_set_init \n");
    WirelessCommIndex = 0;
    memset(&GPRSMasterDB, 0x00, sizeof(OOP_MASTERPARAMS_T));
    read_db_noamal_data(g_hUdbCenter, 0x45000300, 0, 0, (uint8*)&GPRSMasterDB, sizeof(OOP_MASTERPARAMS_T));
    if (apnSetFlag == 0){
        memset(&GprsConfige, 0x00, sizeof(OOP_GPRSCONFIG_T));
        GprsConfigeIsSet = read_db_noamal_data(g_hUdbCenter, 0x45000200, 0, 0, (uint8*)&GprsConfige, sizeof(OOP_GPRSCONFIG_T));
    }
}

void wirelessCommSetReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[4] = { 0 };

    switch (WirelessCommIndex)
    {
		case 0: // APN
		{
		    GprsConfige.apn.apn.nNum = strlen(pNewStr);
            memset(GprsConfige.apn.apn.value, 0, sizeof(GprsConfige.apn.apn.value));
            memcpy(GprsConfige.apn.apn.value, pNewStr, GprsConfige.apn.apn.nNum);
		}
		break;
	    case 1: //IP地址
	    {
	        if (FALSE == commfun_IP(pNewStr, tmp))
	        {
	            if (TRUE == MessageBox("IP地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
	            {
	                if (GPRSMasterDB.nNum < 1 || GPRSMasterDB.master[0].ip[0] == 0)
	                {
	                    strcpy(czBuf, "");
	                }
	                else
	                {
	                    sprintf(czBuf, "%d.%d.%d.%d", GPRSMasterDB.master[0].ip[0], GPRSMasterDB.master[0].ip[1], GPRSMasterDB.master[0].ip[2], GPRSMasterDB.master[0].ip[3]);
	                }
	                InputBox(czBuf, wirelessCommSetReturn, NULL, 0x02);
	            }
	        }
	        else
	        {
	            memcpy(GPRSMasterDB.master[0].ip, tmp, 4);
	            if (GPRSMasterDB.nNum < 1)
	                GPRSMasterDB.nNum = 1;
	        }
	    }
	    break;
	    case 2:     //端口
	    {
	        int port = atoi(pNewStr);
	        if (port <= 65535)
	        {
	            GPRSMasterDB.master[0].port = port;
	            if (GPRSMasterDB.nNum < 1)
	                GPRSMasterDB.nNum = 1;
	        }
	        else
	        {
	            if (TRUE == MessageBox("端口格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
	            {
	                if (GPRSMasterDB.nNum < 1 || GPRSMasterDB.master[0].port == 65535)
	                {
	                    strcpy(czBuf, "");
	                }
	                else
	                {

	                    sprintf(czBuf, "%d", GPRSMasterDB.master[0].port);
	                }
	                InputBox(czBuf, wirelessCommSetReturn, NULL, 0x01);
	            }
	        }
	    }
	    break;
		case 3: //IP地址
	    {
	        if (FALSE == commfun_IP(pNewStr, tmp))
	        {
	            if (TRUE == MessageBox("IP地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
	            {
	                if (GPRSMasterDB.nNum < 2 || GPRSMasterDB.master[1].ip[0] == 0)
	                {
	                    strcpy(czBuf, "");
	                }
	                else
	                {
	                    sprintf(czBuf, "%d.%d.%d.%d", GPRSMasterDB.master[1].ip[0], GPRSMasterDB.master[1].ip[1], GPRSMasterDB.master[1].ip[2], GPRSMasterDB.master[1].ip[3]);
	                }
	                InputBox(czBuf, wirelessCommSetReturn, NULL, 0x02);
	            }
	        }
	        else
	        {
	            memcpy(GPRSMasterDB.master[1].ip, tmp, 4);
	            if (GPRSMasterDB.nNum < 2)
	                GPRSMasterDB.nNum = 2;
	        }
	    }
	    break;
	    case 4:     //端口
	    {
	        int port = atoi(pNewStr);
	        if (port <= 65535)
	        {
	            GPRSMasterDB.master[1].port = port;
	            if (GPRSMasterDB.nNum < 2)
                {
	                GPRSMasterDB.nNum = 2;
                }
	        }
	        else
	        {
	            if (TRUE == MessageBox("端口格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
	            {
	                if (GPRSMasterDB.nNum < 2 || GPRSMasterDB.master[1].port == 65535)
	                {
	                    strcpy(czBuf, "");
	                }
	                else
	                {
	                    sprintf(czBuf, "%d", GPRSMasterDB.master[1].port);
	                }
	                InputBox(czBuf, wirelessCommSetReturn, NULL, 0x01);
	            }
	        }
	    }
	    break;
    }
}

void wirelessApnSetReturn(char* pNewStr, void* pPara)
{

    GprsConfige.apn.apn.nNum = strlen(pNewStr);
    memset(GprsConfige.apn.apn.value, 0, sizeof(GprsConfige.apn.apn.value));
    memcpy(GprsConfige.apn.apn.value, pNewStr, GprsConfige.apn.apn.nNum);

}

void wirelessComm_set_draw(HDC hdc)
{
    int nLeft0 = 10;
    int nLeft1 = 95;
    int nTop = 20;
    char pbuf[50];
    char pbufIp[50];
    char pbufPort[10];

    SelectFont(hdc,g_guiComm.fontSmall);
    bzero(pbuf,sizeof(pbuf));
    snprintf(pbuf, sizeof(pbuf), "4-1 通信参数设置");
    TextOut(hdc, nLeft0, nTop, pbuf);
   
    nTop+=15;
    TextOut(hdc, nLeft0, nTop, "APN域名");
    bzero(pbuf,sizeof(pbuf));
    sprintf(pbuf, "%s", GprsConfige.apn.apn.nNum == 0 ? "-" : GprsConfige.apn.apn.value);
    nTop+=15;
    if (WirelessCommIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft0, nTop, pbuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft0, nTop, pbuf);
    }

    nTop+=15;
    TextOut(hdc, nLeft0, nTop, "主站IP及接入端口");

    bzero(pbufIp, sizeof(pbufIp));
    bzero(pbufPort, sizeof(pbufPort));

    nTop+=15;
    if (GPRSMasterDB.nNum < 1 || GPRSMasterDB.master[0].ip[0] == 0)
    {
        sprintf(pbufIp, "-.-.-.-");
    }
    else
    {
        sprintf(pbufIp, "%d.%d.%d.%d", GPRSMasterDB.master[0].ip[0], GPRSMasterDB.master[0].ip[1], GPRSMasterDB.master[0].ip[2], GPRSMasterDB.master[0].ip[3]);
    }

    if (WirelessCommIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft0, nTop, pbufIp);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft0, nTop, pbufIp);
    }
    TextOut(hdc, nLeft0 + 100, nTop, ": ");
    if (GPRSMasterDB.nNum < 1 || GPRSMasterDB.master[0].port == 65535)
    {
        sprintf(pbufPort, "-");
    }
    else
    {
        sprintf(pbufPort, "%d", GPRSMasterDB.master[0].port);
    }

    if (WirelessCommIndex == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft0 + 105, nTop, pbufPort);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft0 + 105, nTop, pbufPort);
    }

    nTop+=15;
    TextOut(hdc, nLeft0, nTop, "备用IP及接入端口");


    nTop+=15;
    bzero(pbufIp, sizeof(pbufIp));
    bzero(pbufPort, sizeof(pbufPort));

    if (GPRSMasterDB.nNum < 2 || GPRSMasterDB.master[1].ip[0] == 0)
    {
        sprintf(pbufIp, "-.-.-.-");
    }
    else
    {
        sprintf(pbufIp, "%d.%d.%d.%d", GPRSMasterDB.master[1].ip[0], GPRSMasterDB.master[1].ip[1], GPRSMasterDB.master[1].ip[2], GPRSMasterDB.master[1].ip[3]);
    }

    if (WirelessCommIndex == 3)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft0, nTop, pbufIp);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft0, nTop, pbufIp);
    }
    TextOut(hdc, nLeft0 + 100, nTop, ": ");

    
    if (GPRSMasterDB.nNum < 2 || GPRSMasterDB.master[1].port == 65535)
    {
        sprintf(pbufPort, "-");
    }
    else
    {
        sprintf(pbufPort, "%d", GPRSMasterDB.master[1].port);
    }
    
    if (WirelessCommIndex == 4)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft0 + 105, nTop, pbufPort);
        ITEM_NOT_SELECT;

    }
    else
    {
        TextOut(hdc, nLeft0 + 105, nTop, pbufPort);
    }


    nTop+=20;
    if (WirelessCommIndex == 5)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft0, nTop, "确定");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft0, nTop, "确定");
    }

    if (WirelessCommIndex == 6)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "取消");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "取消");
    }
}

//APN
const char* g_apn_list[] = {"CMIOTDLCJXT.CQ","CQGWDL1.GZM2MAPN","WLWCQDL.VPDN.CQ" };

void g_Apn_Set_init(HDC hdc)
{
    WirelessCommIndex = 0;
}

void g_ApnSetDraw(HDC hdc)
{
    int nLeft0 = 10;
    int nLeft1 = 60;
    int nTop = 20;
    char pbuf[50];

    SelectFont(hdc,g_guiComm.fontSmall);
    TextOut(hdc, nLeft1, nTop, "APN域名");

    nTop+=15;
    bzero(pbuf, sizeof(pbuf));
    sprintf(pbuf, "1.%s" , g_apn_list[0]);
    if (WirelessCommIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft0, nTop, pbuf);
        ITEM_NOT_SELECT;
    }
    else{
        TextOut(hdc, nLeft0, nTop, pbuf);
    }

    nTop+=15;
    bzero(pbuf, sizeof(pbuf));
    sprintf(pbuf, "2.%s", g_apn_list[1]);
    if (WirelessCommIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft0, nTop, pbuf);
        ITEM_NOT_SELECT;
    }
    else{
        TextOut(hdc, nLeft0, nTop, pbuf);
    }

    nTop+=15;
    bzero(pbuf, sizeof(pbuf));
    sprintf(pbuf, "3.%s", g_apn_list[2]);
    if (WirelessCommIndex == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft0, nTop, pbuf);
        ITEM_NOT_SELECT;
    }
    else{
        TextOut(hdc, nLeft0, nTop, pbuf);
    }

    nTop+=15;
    bzero(pbuf, sizeof(pbuf));
    sprintf(pbuf, "4.%s", "自定义APN");
    if (WirelessCommIndex == 3)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft0, nTop, pbuf);
        ITEM_NOT_SELECT;
    }
    else{
        TextOut(hdc, nLeft0, nTop, pbuf);
    }
}

BOOL g_ApnSetKeymsg(int nKey)
{
    int maxindex = 3;
    PLUGIN_INFO* pPlugin;
    switch (nKey)
    {
        case KEY_UP:
        case KEY_LEFT:
        {
            if (WirelessCommIndex > 0)
                WirelessCommIndex--;
            else
                WirelessCommIndex = maxindex;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        case KEY_RIGHT:
        {
            if (WirelessCommIndex < maxindex)
                WirelessCommIndex++;
            else
                WirelessCommIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (WirelessCommIndex)
            {
                case 0:
                {
                    GprsConfige.apn.apn.nNum = strlen(g_apn_list[0]);
                    memset(GprsConfige.apn.apn.value, 0, sizeof(GprsConfige.apn.apn.value));
                    memcpy(GprsConfige.apn.apn.value, g_apn_list[0], GprsConfige.apn.apn.nNum);
                    apnSetFlag = 1;
                    WirelessCommIndex = 0;
                    pPlugin = GetPlugIn(0x0050000c);
                    if(pPlugin == NULL)
                    {
                        ASSERT(FALSE);
                        return FALSE;
                    }
                    g_guiMain.pluginCur = pPlugin;
                    if(g_guiMain.pluginCur->pfnInitLayer != NULL)
                    {
                        g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
                    }
                    UpdateWindow(g_guiComm.hMainWnd, TRUE);
                    break;
                }
                case 1:
                {
                    GprsConfige.apn.apn.nNum = strlen(g_apn_list[1]);
                    memset(GprsConfige.apn.apn.value, 0, sizeof(GprsConfige.apn.apn.value));
                    memcpy(GprsConfige.apn.apn.value, g_apn_list[1], GprsConfige.apn.apn.nNum);
                    apnSetFlag = 1;
                    WirelessCommIndex = 0;
                    pPlugin = GetPlugIn(0x0050000c);
                    if(pPlugin == NULL)
                    {
                        ASSERT(FALSE);
                        return FALSE;
                    }
                    g_guiMain.pluginCur = pPlugin;
                    if(g_guiMain.pluginCur->pfnInitLayer != NULL)
                    {
                        g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
                    }
                    UpdateWindow(g_guiComm.hMainWnd, TRUE);
                    break;
                }
                case 2:
                {
                    GprsConfige.apn.apn.nNum = strlen(g_apn_list[2]);
                    memset(GprsConfige.apn.apn.value, 0, sizeof(GprsConfige.apn.apn.value));
                    memcpy(GprsConfige.apn.apn.value, g_apn_list[2], GprsConfige.apn.apn.nNum);
                    apnSetFlag = 1;
                    WirelessCommIndex = 0;
                    pPlugin = GetPlugIn(0x0050000c);
                    if(pPlugin == NULL)
                    {
                        ASSERT(FALSE);
                        return FALSE;
                    }
                    g_guiMain.pluginCur = pPlugin;
                    if(g_guiMain.pluginCur->pfnInitLayer != NULL)
                    {
                        g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
                    }
                    UpdateWindow(g_guiComm.hMainWnd, TRUE);
                    break;
                }
                case 3:
                {
                    char czBuf[20];
                    sprintf(czBuf, GprsConfige.apn.apn.value);
                    InputBox(czBuf, wirelessApnSetReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                    apnSetFlag = 1;
                    WirelessCommIndex = 0;
                    pPlugin = GetPlugIn(0x0050000c);
                    if(pPlugin == NULL)
                    {
                        ASSERT(FALSE);
                        return FALSE;
                    }
                    g_guiMain.pluginCur = pPlugin;
                    if(g_guiMain.pluginCur->pfnInitLayer != NULL)
                    {
                        g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
                    }
                    UpdateWindow(g_guiComm.hMainWnd, TRUE);
                    break;
                }
            }
        }
    }

    return FALSE;
}
//APN设置
PLUGIN_INFO g_ApnSetDisplay = {
    0x0042000d/*id*/,
    g_Apn_Set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    g_ApnSetDraw/*Draw*/,
    NULL/*Timer*/,
    g_ApnSetKeymsg/*keymsg*/,
    NULL/*ipc*/
};

BOOL wirelessComm_set_keymsg(int nKey)
{
    int ret;
    int maxindex = 7;
    GUI_FMT_DEBUG("wirelessComm_set_keymsg key = %d \n", nKey);
    PLUGIN_INFO* pPlugin;
    switch (nKey)
    {
    case KEY_UP:
    case KEY_LEFT:
    {
        if (WirelessCommIndex > 0)
            WirelessCommIndex--;
        else
            WirelessCommIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        if (WirelessCommIndex < maxindex)
            WirelessCommIndex++;
        else
            WirelessCommIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_CANCEL:
    {
        apnSetFlag = 0;
        return FALSE;
    }
    case KEY_OK:
    {
            switch (WirelessCommIndex)
            {
                case 0: //APN
                 {
                    pPlugin = GetPlugIn(0x0042000d);
                    if(pPlugin == NULL)
                    {
                        ASSERT(FALSE);
                        return FALSE;
                    }
                    g_guiMain.pluginCur = pPlugin;
                    if(g_guiMain.pluginCur->pfnInitLayer != NULL)
                    {
                        g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
                    }
                    UpdateWindow(g_guiComm.hMainWnd, TRUE);
                    break;
                 }
                case 1://IP地址
                {
                    char czBuf[20];
                    if (GPRSMasterDB.nNum < 1 || GPRSMasterDB.master[0].ip[0] == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", GPRSMasterDB.master[0].ip[0], GPRSMasterDB.master[0].ip[1], GPRSMasterDB.master[0].ip[2], GPRSMasterDB.master[0].ip[3]);
                    }
                    InputBox(czBuf, wirelessCommSetReturn, NULL, 0x02);
                    return TRUE;
                }
                case 2://端口
                {
                    char czBuf[20];
                    if (GPRSMasterDB.nNum < 1 || GPRSMasterDB.master[0].port == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d", GPRSMasterDB.master[0].port);
                    }
                    InputBox(czBuf, wirelessCommSetReturn, NULL, 0x01);
                    return TRUE;
                }
                case 3://备用IP地址
                {
                    char czBuf[20];
                    if (GPRSMasterDB.nNum < 2 || GPRSMasterDB.master[1].ip[0] == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", GPRSMasterDB.master[1].ip[0], GPRSMasterDB.master[1].ip[1], GPRSMasterDB.master[1].ip[2], GPRSMasterDB.master[1].ip[3]);
                    }
                    InputBox(czBuf, wirelessCommSetReturn, NULL, 0x02);
                    return TRUE;
                }
                case 4://备用端口
                {
                    char czBuf[20];
                    if (GPRSMasterDB.nNum < 2 || GPRSMasterDB.master[1].port == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d", GPRSMasterDB.master[1].port);
                    }
                    InputBox(czBuf, wirelessCommSetReturn, NULL, 0x01);
                    return TRUE;
                }
                case 5://确认
                {
                    GUI_FMT_DEBUG("确认\n");
                    if (ValidatePwd() == TRUE)
                    {
                        OOP_OAD_U oadMaster = { 0x45000300 };
                        ret = write_normal_data((uint8*)&GPRSMasterDB, sizeof(OOP_MASTERPARAMS_T), oadMaster, 0, 0, 1);
                        if (ret != 0)
                        {
                            MessageBox("主站参数写入失败！");
                        }
                        else
                        {
                            MessageBox("主站参数写入成功！");
                        }
                        OOP_OAD_U oadApn = { 0x45000200 };
                        ret = write_normal_data((uint8*)&GprsConfige, sizeof(OOP_GPRSCONFIG_T), oadApn, 0, 0, 1);
                        if (ret != 0)
                        {
                            MessageBox("APN写入失败！");
                        }
                        else
                        {
                            MessageBox("APN写入成功！");
                        }
                        apnSetFlag = 0;
                        return TRUE;
                    }
                    break;
                }
                case 6://取消
                {
                    apnSetFlag = 0;
                    CancelMenu();
                    UpdateWindow(g_guiComm.hMainWnd, TRUE);
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}
//以太网主站参数设置
PLUGIN_INFO g_wirelessComm_set = {
    0x0050000c/*id*/,
    wirelessComm_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    wirelessComm_set_draw/*Draw*/,
    NULL/*Timer*/,
    wirelessComm_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};



/************************************************************************/
/* 重庆集中器中通信参数设置 (4-2 修改密码)                                              */
/*                                                                      */
/************************************************************************/
typedef struct _CHANGE_PWD
{
    int     nIndex;     // 当前索引
    char    czPwd[PASSWORD_LEN*2+2]; // 密码
}CHANGE_PWD;

CHANGE_PWD g_chg_pwd;

void ChgPwdInit(DWORD dwPluginId)
{
    g_chg_pwd.nIndex = 0;
    memset(g_chg_pwd.czPwd, '0', sizeof(g_chg_pwd.czPwd));
    g_chg_pwd.czPwd[PASSWORD_LEN] = '\0';
    g_chg_pwd.czPwd[PASSWORD_LEN * 2 + 1] = '\0';
}

void ChgPwdDraw(HDC hdc)
{
    int nLeft = 10;
    int nLeft1 = 95;
    int nTop = 20;
    char pbuf[300];
    uint32 i = 0;

    SelectFont(hdc,g_guiComm.fontSmall);
    bzero(pbuf,sizeof(pbuf));
    snprintf(pbuf, sizeof(pbuf), "4-2 修改密码");
    TextOut(hdc, nLeft, nTop, pbuf);
    
    nTop+=20;
    TextOut(hdc, nLeft, nTop, "输入新密码:");

    nTop+=20;
    for (; i < PASSWORD_LEN; i++)
    {
        if (i == (uint32)g_chg_pwd.nIndex)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_chg_pwd.czPwd[i], 1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_chg_pwd.czPwd[i], 1);
        }
        nLeft+=12;
    }

    nTop+=20;
    nLeft = 10;
    TextOut(hdc, nLeft, nTop, "确认新密码:");

    nTop+=20;
    i++;
    for (; i < PASSWORD_LEN * 2 + 1; i++)
    {
        if (i == (uint32)g_chg_pwd.nIndex)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_chg_pwd.czPwd[i], 1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_chg_pwd.czPwd[i], 1);
        }
        nLeft+=12;

    }

    nTop+=20;
    nLeft = 10;
    // 确定
    if (g_chg_pwd.nIndex == PASSWORD_LEN*2+1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "确定");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "确定");
    }

    // 取消
    if (g_chg_pwd.nIndex == PASSWORD_LEN*2+2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "取消");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "取消");
    }
}

// 密码输入页面返回处理
void ChgPwdReturn(char* pNewStr, void* pPara)
{
    if(strlen(pNewStr) != PASSWORD_LEN)
    {
		char msg[64];
		sprintf(msg, "密码长度必须为%d字节\n\n确认是否重新输入?", PASSWORD_LEN);
        MessageBox(msg, MB_OK);
    }
}
BOOL ChgPwdKeyMsg(int nKey)
{
    int nRet = 0;
    switch(nKey)
    {
    case KEY_UP:// 处理在几个详细焦点中移动
        switch(g_chg_pwd.nIndex)
        {
        case PASSWORD_LEN*2 + 1:return FALSE;
		case (PASSWORD_LEN*2)+2:
            return FALSE;
        default:
            g_chg_pwd.czPwd[g_chg_pwd.nIndex] ++;
            if(g_chg_pwd.czPwd[g_chg_pwd.nIndex] > '9')
            {
                g_chg_pwd.czPwd[g_chg_pwd.nIndex] = '0';
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    case KEY_DOWN:// 处理在几个详细焦点中移动
        switch(g_chg_pwd.nIndex)
        {
            case PASSWORD_LEN*2 + 1:return FALSE;
            case (PASSWORD_LEN*2)+2:
                return FALSE;

        default:
            g_chg_pwd.czPwd[g_chg_pwd.nIndex] --;
            if(g_chg_pwd.czPwd[g_chg_pwd.nIndex] < '0')
            {
                g_chg_pwd.czPwd[g_chg_pwd.nIndex] = '9';
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    case KEY_LEFT:// 修改密码
        g_chg_pwd.nIndex --;
        if(g_chg_pwd.nIndex < 0)
        {
            g_chg_pwd.nIndex = PASSWORD_LEN*2+2;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:// 修改密码
        g_chg_pwd.nIndex++;
        if(g_chg_pwd.nIndex > PASSWORD_LEN*2+2)
        {
            g_chg_pwd.nIndex = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
		// 处理详细按键，弹出输入页面
        switch(g_chg_pwd.nIndex)
        {
            case (PASSWORD_LEN*2)+1:
                // 比较输入新密码
                if(memcmp(g_chg_pwd.czPwd, 
                    g_chg_pwd.czPwd+PASSWORD_LEN+1, PASSWORD_LEN) != 0)
                {
                    MessageBox("提示\n两次密码输入不一致！");
                    return TRUE;
                }
                // 保存密码
                if (ValidatePwd() == TRUE)
                {
                    nRet = write_pdata_xram(GUI_PRIVATE_DATA, g_chg_pwd.czPwd, PRIVATE_PASSWORD, PASSWORD_LEN);
                    if (nRet >= 0)
                    {
                        MessageBox("提示\n密码修改成功！");
                    }
                    else
                    {
                        MessageBox("提示\n密码修改失败！");
                    }
                    return TRUE;
                }
                break;
            case (PASSWORD_LEN*2)+2:
                CancelMenu();
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                return TRUE;
        }
    }
    return FALSE;
}

//密码设置
PLUGIN_INFO g_changePwd = {
    0x0051000c/*id*/,
    ChgPwdInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    ChgPwdDraw/*Draw*/,
    NULL/*Timer*/,
    ChgPwdKeyMsg/*keymsg*/,
    NULL/*ipc*/
};


/************************************************************************/
/* 重庆集中器中通信参数设置 (4-3 终端地址码)                                             */
/*                                                                      */
/************************************************************************/
uint8 ChangeAddrIndex;
extern int get_addr(uint8* Addr);
uint8 LocalAddress[6] = { 0 };
typedef struct
{
    int upValue; //上键值按次数
    int dwValue; //下键值按次数
}KEYPRESS_FIR;

KEYPRESS_FIR keyState;
void ChangeAddrInit(DWORD dwPluginId)
{
    memset(&keyState, 0x00, sizeof(KEYPRESS_FIR));
    get_addr(LocalAddress);
}

void ChangeAddrDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;

    char czBuf[300];
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nLeft = 10;
    int nTop = 20;

    nLeft+=30;
    nTop+=40;
    bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "OOP终端地址");
    TextOut(hdc, nLeft, nTop, czBuf);
    
    nTop+=20;
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", LocalAddress[0], LocalAddress[1], LocalAddress[2], LocalAddress[3], LocalAddress[4], LocalAddress[5]);
    ITEM_SELECT;
    TextOut(hdc, nLeft, nTop, czBuf);
    ITEM_NOT_SELECT;

}

void ChangeAddrReturn(char* pNewStr, void* pPara)
{
    int nLen = strlen(pNewStr);
    int ret;

    if (nLen > 12 && TRUE == MessageBox("OOP终端地址长度大于6字节\n\n确认是否重新输入?", MB_OKCANCEL))
    {
        char tmp[20] = { 0 };
        sprintf(tmp, "%02x%02x%02x%02x%02x%02x", LocalAddress[0], LocalAddress[1], LocalAddress[2], LocalAddress[3], LocalAddress[4], LocalAddress[5]);
        InputBox(tmp, ChangeAddrReturn, NULL, 0x01);
    }
    else
    {
        char tmp[20] = { 0 };
        strcpy(tmp, "000000000000");
        strcpy(tmp + 12 - nLen, pNewStr);
        printf("输入的地址为 %s \n", tmp);
        commfun_ASC2BCD((char*)LocalAddress, tmp, 12, 0);
        printf("转换为bcd %02x%02x%02x%02x%02x%02x \n", LocalAddress[0], LocalAddress[1], LocalAddress[2], LocalAddress[3], LocalAddress[4], LocalAddress[5]);
        OOP_OCTETVAR16_T TmpLocalAddr;
        memset(&TmpLocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
        TmpLocalAddr.nNum = 6;
        memcpy(TmpLocalAddr.value, LocalAddress, 6);
        OOP_OAD_U oad = { 0x40010200 };
        ret = write_normal_data((uint8*)&TmpLocalAddr, sizeof(OOP_OCTETVAR16_T), oad, 0, 0, 1);
        if (ret == 0)
        {
            MessageBox("终端地址设置成功");
        }
        else
        {
            MessageBox("终端地址设置失败");
        }
    }
}

BOOL ChangeAddrKeyMsg(int nKey)
{
    switch (nKey)
    {
        case KEY_UP:
        {
            if (keyState.upValue < 2)
                keyState.upValue++;
            else
                keyState.upValue = 0;
            keyState.dwValue = 0;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        {
            if (keyState.dwValue < 2)
                keyState.dwValue++;
            else
                keyState.dwValue = 0;
            if(keyState.dwValue == 2 && keyState.upValue == 2)
            {
                char tmp[20] = {0};
                sprintf(tmp, "%02x%02x%02x%02x%02x%02x", LocalAddress[0], LocalAddress[1], LocalAddress[2], LocalAddress[3], LocalAddress[4], LocalAddress[5]);
                InputBox(tmp, ChangeAddrReturn, NULL, 0x01);
                return TRUE;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
        default: break;
    }
    return FALSE;
}

//终端地址码设置
PLUGIN_INFO g_changeAddr = {
    0x00630000/*id*/,
    ChangeAddrInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    ChangeAddrDraw/*Draw*/,
    NULL/*Timer*/,
    ChangeAddrKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

/************************************************************************/
/* 重庆集中器中通信参数设置 (4-4 虚拟专网用户名及密码)                  */
/*                                                                      */
/************************************************************************/
uint8 VPNIndex;
void ChangeVPNPwdInit(DWORD dwPluginId)
{
    VPNIndex = 0;
    memset(&GprsConfige, 0x00, sizeof(OOP_GPRSCONFIG_T));
    GprsConfigeIsSet = read_db_noamal_data(g_hUdbCenter, 0x45000200, 0, 0, (uint8*)&GprsConfige, sizeof(OOP_GPRSCONFIG_T));
}

void ChangeVPNPwdDraw(HDC hdc)
{
    int nLeft = 10;
    int nLeft1 = 95;
    int nTop = 20;
    char pbufName[50];
    char pbufPassword[50];

    SelectFont(hdc,g_guiComm.fontSmall);
    bzero(pbufName,sizeof(pbufName));
    snprintf(pbufName, sizeof(pbufName), "4-3 虚拟专网用户名");
    TextOut(hdc, nLeft, nTop, pbufName);
    
    nTop+=20;
    TextOut(hdc, nLeft, nTop, "输入用户名");
    bzero(pbufName,sizeof(pbufName));
    bzero(pbufPassword,sizeof(pbufPassword));
    
    nTop+=20;
    if (GprsConfigeIsSet < 0 || GprsConfige.apn.username.nNum == 0)
    {
        sprintf(pbufName, "-");
    }
    else
    {
        sprintf(pbufName, "%s", GprsConfige.apn.username.value);
    }

    if(VPNIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, pbufName);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, pbufName);
    }

    nTop+=20;
    TextOut(hdc, nLeft, nTop, "输入密码");

    nTop+=20;
    if (GprsConfigeIsSet < 0 || GprsConfige.apn.pwd.nNum == 0)
    {
        sprintf(pbufPassword, "-");
    }
    else
    {
        sprintf(pbufPassword, "%s", GprsConfige.apn.pwd.value);
    }

    if(VPNIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, pbufPassword);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, pbufPassword);
    }


    nTop+=20;
    if(VPNIndex == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "确定");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "确定");
    }

    if(VPNIndex == 3)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "取消");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "取消");
    }
}

void GprsConfigeReturn(char* pNewStr, void* pPara)
{
    switch (VPNIndex)
    {
        case 0: //用户名
        {
            GprsConfige.apn.username.nNum = strlen(pNewStr);
            memset(GprsConfige.apn.username.value, 0, sizeof(GprsConfige.apn.username.value));
            memcpy(GprsConfige.apn.username.value, pNewStr, GprsConfige.apn.username.nNum);
            GprsConfigeIsSet = 1;
        }
        break;
        case 1: //密码
        {
            GprsConfige.apn.pwd.nNum = strlen(pNewStr);
            memset(GprsConfige.apn.pwd.value, 0, sizeof(GprsConfige.apn.pwd.value));
            memcpy(GprsConfige.apn.pwd.value, pNewStr, GprsConfige.apn.pwd.nNum);
            GprsConfigeIsSet = 1;
        }
        break;
    }
}

BOOL ChangeVPNPwdKeyMsg(int nKey)
{
    switch (nKey)
    {
        case KEY_UP:
        case KEY_LEFT:
        {
            if (VPNIndex > 0)
            {
                VPNIndex--;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        case KEY_RIGHT:
        {
            if (VPNIndex < 4)
            {
                VPNIndex++;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (VPNIndex)
            {
                case 0: // 用户名
                {
                    char czBuf[20];
                    if (GprsConfige.apn.username.nNum == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%s", GprsConfige.apn.username.value);
                    }
                    InputBox(czBuf, GprsConfigeReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                    return TRUE;

                }
                case 1: // 密码
                {
                    char czBuf[20];
                    if (GprsConfige.apn.pwd.nNum == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%s", GprsConfige.apn.pwd.value);
                    }
                    InputBox(czBuf, GprsConfigeReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                    return TRUE;

                }
                case 2: // 确认 
                {
                    if (ValidatePwd() == TRUE)
                    {
                        OOP_OAD_U oad = { 0x45000200 };
                        int ret = write_normal_data((uint8*)&GprsConfige, sizeof(OOP_GPRSCONFIG_T), oad, 0, 0, 1);
                        if (ret != 0)
                        {
                            MessageBox("用户名和密码写入失败！");
                        }
                        else
                        {
                            MessageBox("用户名和密码写入成功！");
                        }

                    }
                    return TRUE;
                }
                case 3:// 取消
                {
                    CancelMenu();
                    UpdateWindow(g_guiComm.hMainWnd, TRUE);
                    return TRUE;
                }
                default: break;
            }
        }
    	case KEY_CANCEL:
            CancelMenu();
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    return TRUE;
}

//密码设置
PLUGIN_INFO g_changeVPNPwd = {
    0x00740000/*id*/,
    ChangeVPNPwdInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    ChangeVPNPwdDraw/*Draw*/,
    NULL/*Timer*/,
    ChangeVPNPwdKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

/************************************************************************/
/* 重庆集中器中通信参数设置 (4-5 终端当前网络制式)                  */
/*                                                                      */
/************************************************************************/


const char* g_GprsType[4] = 
{
    "auto",
    "2G",
    "3G",
    "4G"
};

typedef struct _GPRS_TYPE_INFO
{
    int     nIndex;
    int     nType;
}GPRS_TYPE;

GPRS_TYPE g_gprs_type;

void WriteGPRSNetMode()
{
    // uint8 paramode =  (uint8)g_gprs_type.nType + 1;
    // WriteXPara(TMN_PARA_EXTEND, SET_GPRSNET_MODE, (char *)&paramode, 0);
	
    return;
}

void ReadGPRSNetMode()
{
/*
    uint8 paramode =  1;

    if(ReadXPara(TMN_PARA_EXTEND, SET_GPRSNET_MODE, (char *)&paramode, 0) < 0 || 
      (paramode > 4 || paramode < 1))
    {
        paramode = 1;
    }

    g_gprs_type.nType = (int)paramode - 1;
*/	
    return;
}

void GprsCfgInit(DWORD dwPluginId)
{
    g_gprs_type.nIndex = 0;	
    ReadGPRSNetMode();

    return;
}

void GprsCfgDraw(HDC hdc)
{
    ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);

    int nIndex = 0;
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    char czBuf[300];
    sprintf(czBuf, "4-4 网络制式设置");

    rc.top = 26;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);

	// 信道类型
    int nTop = rc.top + 26;
    int nLeft1 = 10;
    int nLeft2 = 82;
    TextOut(hdc, nLeft1, nTop, "网络制式");

    if(g_gprs_type.nIndex == nIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_GprsType[g_gprs_type.nType]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_GprsType[g_gprs_type.nType]);
    }
    nIndex++;
    nTop += 72;
    if(g_gprs_type.nIndex == nIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "保存设置");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "保存设置");
    }

    PopListDraw(hdc);
}

BOOL GprsCfgKeyMsg(int nKey)
{
    if(g_guiComm.bPopList == TRUE)
    {
        int nSel;
        PopListKeyMsg(nKey, &nSel);
        if(nKey == KEY_OK)
        {
            switch(g_gprs_type.nIndex)
            {
            case 0:// 网络制式
                g_gprs_type.nType = nSel;
                break;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
        }
        return TRUE;
    }
	
    switch(nKey)
    {
        case KEY_UP:
            g_gprs_type.nIndex --;
            if(g_gprs_type.nIndex < 0)
            {
                g_gprs_type.nIndex = 0;
                return FALSE;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case KEY_DOWN:
            {
                int nMax = 1;
                g_gprs_type.nIndex ++;
                if(g_gprs_type.nIndex > nMax)
                {
                    g_gprs_type.nIndex = nMax;
                    return FALSE;
                }
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case KEY_CANCEL:
	    	return FALSE;
        case KEY_OK:
	        if(ValidatePwd() == TRUE)
	        {
	            switch(g_gprs_type.nIndex)
	            {
                    case 0:
                        PopList(g_GprsType, 4, g_GprsType[g_gprs_type.nType], 82, 42, FALSE);
                        break;
                    case 1:
                        WriteGPRSNetMode();
                        MessageBox("通道信息已经保存！");
                        break;
	            }

                    return TRUE;
	        }
    }
    return TRUE;
}

PLUGIN_INFO g_layerGprsCfg = {
	0x0074000C/*id*/, 
	GprsCfgInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	GprsCfgDraw/*Draw*/,
	NULL/*Timer*/,
	GprsCfgKeyMsg/*keymsg*/,
	NULL/*ipc*/
};

/************************************************************************/
/* 重庆集中器中通信参数设置 (4-6 USB设置)                  */
/*                                                                      */
/************************************************************************/

uint8 usbcom_status = 0;		//USB通信模式： 0——关闭，1——开启，默认关闭
int   usbcom_key_index = 0;
const char *g_usbcom_list[] =
{
    "关闭",
    "启用",
};

void setusbcomInit(DWORD dwPluginId)
{
    // char  usbcommpara;
    g_guiComm.bCanTrunDisp = FALSE;
    /*
    if((ReadXPara(TMN_PARA_EXTEND, Usb_Virtual_Comm_EN, (char *)&usbcommpara, 0x0) < 0) || (usbcommpara != 0x55))
        usbcom_status = 0;
	else
        usbcom_status = 1;
    */
    usbcom_key_index = 0;
}

void setusbcomDraw(HDC hdc)
{
    // int       nTop = 0;
    char      czbuf[32];
    // uint8     i = 0;

    char czBuf[100];
    sprintf(czBuf, "4-5 USB设置");
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 26;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);

    SelectFont(hdc,g_guiComm.fontBig);

    //标题显示
    ITEM_NOT_SELECT;
    sprintf(czbuf, "USB通信:");
    TextOut(hdc, 30, 70, czbuf);

    if(usbcom_key_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, 100, 70, g_usbcom_list[usbcom_status]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 100, 70, g_usbcom_list[usbcom_status]);
    }
    #if HARDWARE_TYPE == 1
    if(usbcom_key_index == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, 120, 90, "确定");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 120, 90, "确定");
    }	
    #else
    if(usbcom_key_index == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, 120, 118, "确定");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 120, 118, "确定");
    }
    #endif
    PopListDraw(hdc);
}

BOOL setusbcomMsg(int nKey)
{
    // uint8     pRD;
    // uint8     pRD1 = 0x55;
    // 弹出List选择处理
    if(g_guiComm.bPopList == TRUE)
    {
        int nSel;
        PopListKeyMsg(nKey, &nSel);
        if(nKey == KEY_OK)
        {
            usbcom_status = nSel;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
        }
        return TRUE;
    }

    switch(nKey)
    {
    case KEY_UP:
        usbcom_key_index--;
        if(usbcom_key_index < 0)
            usbcom_key_index = 1;
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;

    case KEY_DOWN:
        usbcom_key_index ++;
        if(usbcom_key_index > 1)
            usbcom_key_index = 0;
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;

    case KEY_OK:
        if(ValidatePwd() == TRUE)
        {
            // 确认处理
            switch(usbcom_key_index)
            {
            case 0:
                PopList(g_usbcom_list, 2, g_usbcom_list[usbcom_status], 100, 85, FALSE);
                break;
            case 1:
                {
                    /*
                    char  usbcommpara = 0;
                    if(usbcom_status == 1)
						usbcommpara = 0x55;
					WriteXPara(TMN_PARA_EXTEND, Usb_Virtual_Comm_EN, (char *)&usbcommpara, 0x0);
                    */
                    UpdateWindow(g_guiComm.hMainWnd, TRUE);
                    MessageBox("设置成功");
                }
                break;
            default:
                break;
            }
            return TRUE;
        }
    case KEY_CANCEL:
        return FALSE;
    }
    return TRUE;
}

PLUGIN_INFO g_layersetusbcom=
{
    0x0074000D, // 0x0074000D
    setusbcomInit,
    NULL,
    setusbcomDraw,
    NULL,
    setusbcomMsg,
    NULL
};

const char* g_wday[] = {"星期天", "星期一", "星期二", "星期三", "星期四","星期五", "星期六"};

//运行状态 告警
const char* run_status[] = { "正常","故障" };
//投运状态 档案
const char* operation_status[] = { "投运","未投" };
//任务执行状态
const char* execution_status[] = { "抄表","搜表", "完毕" };
//主站通信状态
const char* communication_status[] = { "空闲","通讯", "掉线" };

void Set_CCUTimeDraw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int tmpTop = 22;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 35;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "重庆地区集中器");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    // 读取当前时间
    long lTime = time(NULL);
    struct tm* tmNow = localtime(&lTime);

    rc.top += tmpTop+10;
    rc.bottom = rc.top + 16;
    bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "%04d-%02d-%02d", tmNow->tm_year + 1900, tmNow->tm_mon + 1, tmNow->tm_mday);
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    rc.top += tmpTop;
    rc.bottom = rc.top + 16;
    bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "%s", g_wday[tmNow->tm_wday]);
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc,g_guiComm.fontSmall);
    int nLeft = 20;
    int nTop = 120;

    MoveTo(hdc, nLeft, nTop);
    LineTo(hdc, nLeft, nTop+20);

    MoveTo(hdc, nLeft, nTop);
    LineTo(hdc, nLeft+120, nTop);

    MoveTo(hdc, nLeft, nTop+20);
    LineTo(hdc, nLeft+120, nTop+20);

    MoveTo(hdc, nLeft+30, nTop);
    LineTo(hdc, nLeft+30, nTop+20);

    MoveTo(hdc, nLeft+60, nTop);
    LineTo(hdc, nLeft+60, nTop+20);

    MoveTo(hdc, nLeft+90, nTop);
    LineTo(hdc, nLeft+90, nTop+20);

    MoveTo(hdc, nLeft+120, nTop);
    LineTo(hdc, nLeft+120, nTop+20);

    bzero(czBuf,sizeof(czBuf));
    if(toolInfoFresh.warning == 1)
        sprintf(czBuf, "%s", run_status[1]);
    else
        sprintf(czBuf, "%s", run_status[0]);
    TextOut(hdc, nLeft+2, nTop+5, czBuf);

    bzero(czBuf,sizeof(czBuf));
    if(gDeskMeter.meterNum == 0)
        sprintf(czBuf, "%s", operation_status[1]);
    else
        sprintf(czBuf, "%s", operation_status[0]);
    TextOut(hdc, nLeft+32, nTop+5, czBuf);

    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0};
    uint32 len = 0;
    uint8 CtsReadMeterStatus = 2;
    normal.oad.value  = 0x6012ff00;
    normal.classtag = 3;

    //GUI_FMT_DEBUG("CtsReadMeterStatus %u \n",CtsReadMeterStatus);
    ret = db_read_nomal(g_hUdbCenter, &normal, sizeof(uint8), &CtsReadMeterStatus, &len);
    if(ret != ERR_OK)
        GUI_FMT_DEBUG("ret %u \n",ret);
    bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "%s", execution_status[CtsReadMeterStatus]);
    TextOut(hdc, nLeft+62, nTop+5, czBuf);

    bzero(czBuf,sizeof(czBuf));
    if(toolInfoFresh.channel1.conStatus || toolInfoFresh.ethConStatus == TRUE)
        sprintf(czBuf, "%s", communication_status[1]);
    else
        sprintf(czBuf, "%s", communication_status[2]);
    TextOut(hdc, nLeft+92, nTop+5, czBuf);
}

PLUGIN_INFO g_Set_CCUResetTime = {
    0x00A00102/*id*/,
    NULL/*InitLayer*/,
    NULL/*UninitLayer*/,
    Set_CCUTimeDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

uint8 sshsetindex;
void InitSSHSet(DWORD dwPluginId)
{
    sshsetindex = 0;
}

void SSHSetDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if(pg_guicomm == NULL)
        return;

    SelectFont(hdc,pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 40;
    rc.bottom = rc.top+16;
    char pcz[20];
    strcpy(pcz, "SSH服务设置");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    int nTop = 80;
    int nLeft = 8;
    if(sshsetindex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "开启");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "开启");
    }

    nLeft = 48;
    if(sshsetindex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "永久开启");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "永久开启");
    }

    nLeft = 120;
    if(sshsetindex == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "关闭");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "关闭");
    }
}

//choice 0 开启ssh 1 关闭ssh
void sshset(uint8 choice)
{
    char cmd[100]={0};

    if(choice != 2)
    {
        sprintf(cmd,"systemctl start sshd");
    }else 
    {
        sprintf(cmd,"systemctl stop sshd");        
    }
    GUI_FMT_DEBUG("sshset %s \n",cmd);
    system(cmd);
    
    if(choice != 2)
    {
        sprintf(cmd,"/usr/sbin/sshd -D &");
    }else 
    {
        sprintf(cmd,"ps |grep /usr/sbin/sshd |grep -v grep|awk '{print $1}'|xargs kill all -9| pkill sshd");
    }
    
    GUI_FMT_DEBUG("sshset %s \n",cmd);
    system(cmd);

    if(choice != 1)
    {
        return ;
    }
    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0};
    uint32 i = 0;
    uint32 len = 0;

    OOP_SERVICES_T dataGet = {0};
    memset(&dataGet, 0, sizeof(OOP_SERVICES_T));
    normal.oad.value  = 0x43000b00;
    normal.classtag = CLASS_DATA_UNINIT;

    ret = db_read_nomal(g_hUdbCenter, &normal, sizeof(dataGet), (uint8*)&dataGet, &len);

    for(i = 0; i < dataGet.nNum; i++)
    {
        if(dataGet.item[i].type == 1)
        {
            dataGet.item[i].times = 255;
            break;
        }
    }

    if((i == dataGet.nNum) && (i < OOP_MAX_SERVICES))
    {
        dataGet.item[i].type = 1;
        dataGet.item[i].times = 255;
        dataGet.nNum++;
    }
    
    ret = db_write_nomal(g_hUdbCenter, &normal, (uint8*)&dataGet, sizeof(OOP_SERVICES_T));
    if(ret != ERR_OK)
    {
        GUI_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
    }

}
BOOL SSHSetKey(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if(pg_guicomm == NULL)
        return FALSE;

    switch(nKey)
    {
    case KEY_LEFT:
    case KEY_UP:
        if(sshsetindex>0)
            sshsetindex --;
        else
            sshsetindex = 2;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
    case KEY_DOWN:
        if(sshsetindex<2)
            sshsetindex++;
        else
            sshsetindex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        if(ValidatePwd() == TRUE)
        {
            g_bpop = FALSE;
            switch(sshsetindex)
            {
                case 0:
                    if(FALSE == MessageBox("提示\n是否开启SSH服务\n", MB_OKCANCEL))
                    {
                        return TRUE;
                    }
                    sshset(0);
                    CancelMenu();
                    return TRUE;
                case 1:

                    if(FALSE == MessageBox("提示\n是否永久开启SSH服务\n", MB_OKCANCEL))
                    {
                        return TRUE;
                    }
                    sshset(1);
                    CancelMenu();
                    return TRUE;
                case 2:

                    if(FALSE == MessageBox("提示\n是否关闭SSH服务\n", MB_OKCANCEL))
                    {
                        return TRUE;
                    }
                    sshset(2);
                    CancelMenu();
                    return TRUE;
            }
//        g_bpop = FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

//SSH服务设置
PLUGIN_INFO g_SSHSet_Layer = {
    0x00A000D0/*id*/, 
    InitSSHSet/*InitLayer*/,
    NULL/*UninitLayer*/,
    SSHSetDraw/*Draw*/,
    NULL/*Timer*/,
    SSHSetKey/*keymsg*/,
    NULL/*ipc*/
};
#endif
