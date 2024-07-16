/*
***************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限责任公司
             All rights reserved.
  程序名称: configcontrol.cpp
  功    能：完成终端数据配置页面库
  开 发 人：gaoyf
  开发时间：2007-1

  版本历史:

    v1.0:
      2006-1-20, gaoyf : New file

  包含页面库：
    终端地址（含行政区码）  0x00630000
    终端当前主备通道信息    0x00660000


***************************************************************************
*/

#include <sys/stat.h>
#include "miniguidef.h"
#include "commdef.h"
#include "main.h"
#include "commfun.h"
#include "dataType.h"
#include "appmsg_struct.h"
#include "appmsg_split.h"
#include "pageInterface.h"
#include "framecomm.h"



uint8 num = 0;
#define MAX_METER_INFONUM   2040   //档案的最大信息点号
#define MAX_TASK_INFONUM 1000       //任务最大信息点号

const OOP_OAD_U MeterFileOad = { 0x60000200 };
const OOP_OAD_U EthIPOad = { 0x45100400 };
const OOP_OAD_U EthConfigOad = { 0x45100200 };


//const OOP_OAD_U CommChannelinfo={0x45200200 }; //新定义了一个oad用来存通信通道信息
const OOP_OAD_U EthMasterOad = { 0x45100300 };
const OOP_OAD_U GPRSConfigOad = { 0x45000200 };
const OOP_OAD_U GPRSMasterOad = { 0x45000300 };
RST_RMT_T g_tShowRmt;
uint8          g_tShowRmtNum = 0;
const char* g_pczRadioDelay[] = { "100", "200", "300", "400", "500", "800", "1000", "2000" };
//电表波特率
const char* g_baud[] = { "300bps","600bps","1200bps","2400bps","4800bps","7200bps","9600bps","19200bps","38400bps","57600bps","115200bps","自适应" };

//规约类型
const char* g_prtl[] = { "未知","DL/T 645-1997","DL/T 645—2007","DL/T 698.45","CJ/T 188—2004" };

//TI格式时间间隔
const char* g_ti[] = { "秒","分","时","日","月","年" };

//方案类型
const char* g_plantype[] = { "普通采集方案","事件采集方案","透明方案","上报方案","脚本方案" };

//端口
const char* g_pipe[] = { "载波-1","载波-2","485-1","485-2","485-3","485-4","485-5","485-6","485-7","485-8","交采" };

//开关
const char* g_on_off[] = { "关闭","开启" };

//ip配置方式
const char* g_ipconfig[] = { "DHCP","静态","PPPoE" };

//通信配置工作模式
const char* g_work_mode[] = { "混合模式","客户机模式","服务器模式" };

//连接方式
const char* g_conn_type[] = { "TCP","UDP" };

//连接应用方式
const char* g_conn_app_type[] = { "主备模式","多连接模式" };

//在线方式
const char* g_line_type[] = { "永久在线","被动激活" };

//客户端通道类型
const char* g_channel_type[] = {"关闭", "无线公网", "无线专网", "以太网", "232串口"};

int RebootTerminal();
uint32 ip[10] = { 0 };
uint16 port[10] = { 0 };

extern void CancelMenu();

void MenuDraw_add_name(HDC hdc, const char* pname);

void MenuDraw(HDC hdc)
{
    MenuDraw_add_name(hdc, ADV_SHOW_NAME);
}


BOOL MenuKeyMsg(int nKey);
void InitMenu(DWORD dwPluginId);

void InitSocket(DWORD dwPluginId);

void SocketDraw(HDC hdc);

void InitRemote(DWORD dwPluginId);

void RemoteDraw(HDC hdc);

PLUGIN_INFO g_menuLayer = {
    0x00A00000/*id*/,
    InitMenu/*InitLayer*/,
    NULL/*UninitLayer*/,
    MenuDraw/*Draw*/,
    NULL/*Timer*/,
    MenuKeyMsg/*keymsg*/,
    NULL/*ipc*/
};
PLUGIN_INFO g_socketstart = {
    0x21120002/*id*/,
    InitSocket/*InitLayer*/,
    NULL/*UninitLayer*/,
    SocketDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
PLUGIN_INFO g_remotestatus = {
    0x21120001/*id*/,
    InitRemote/*InitLayer*/,
    NULL/*UninitLayer*/,
    RemoteDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

#include "db_api.h"
#include "app_gui.h"
#include "oopVarUpdate.h"
#include "oop_pw_area.h"

#ifdef __cplusplus
    extern "C" {
#endif
extern uint8 comm_check_ppp_online(uint8 id, uint32 ip, uint16 port);
extern uint8 comm_check_eth_online(uint8 id, uint32 ip, uint16 port);

#ifdef __cplusplus
}
#endif


/**
*********************************************************************
* @brief： 功能描述 初始化主站连接状态
* @param[in]： 入参说明
* @param[out]： 出参无
* @return： 返回值说明
*********************************************************************
*/
void InitSocket(DWORD dwPluginId)
{

}

/**
*********************************************************************
* @brief： 功能描述 绘制主站连接状态
* @param[in]： 入参说明
* @param[out]： 出参无
* @return： 返回值说明
*********************************************************************
*/
void SocketDraw(HDC hdc)
{
    num = comm_get_listen_master(ip, port, 10);
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;
    char czBuf[64] = { 0 };

    int nLeft2 = 5;
    int nTop = 26;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "主站连接状态");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);
    TextOut(hdc, nLeft2, nTop + 18, "IP");
    TextOut(hdc, nLeft2 + 80, nTop + 20, "端口");
    TextOut(hdc, nLeft2 + 120, nTop + 20, "状态");
    if (num <= 0)
    {
        TextOut(hdc, nLeft2 + 25, nTop + 40, "暂时没有主站连接");
    }

    int i = 0;
    nTop = 46;
    for (i = 0; i < num; i++)
    {

        nLeft2 = 5;
        nTop += 18;
        // 数字转字符串
        char IP[128] = { 0 };
        sprintf(IP, "%d.%d.%d.%d", (ip[i] >> 24) & 0xff, (ip[i] >> 16) & 0xff, (ip[i] >> 8) & 0xff, ip[i] & 0xff);
        TextOut(hdc, nLeft2, nTop, IP);
        nLeft2 += 80;
        char Port[128] = { 0 };
        sprintf(Port, "%d", port[i]);
        TextOut(hdc, nLeft2, nTop, Port);
        nLeft2 += 35;
        if (IP[0] == '\0' || Port[0] == '\0')
        {

            TextOut(hdc, nLeft2, nTop, "未连接");
        }
        else
        {

            TextOut(hdc, nLeft2, nTop, "已连接");
        }
    }
}


/**
*********************************************************************
* @brief： 功能描述 初始化遥信状态
* @param[in]： 入参说明
* @param[out]： 出参无
* @return： 返回值说明
*********************************************************************
*/
void InitRemote(DWORD dwPluginId)
{
    
}

/**
*********************************************************************
* @brief： 功能描述 绘制遥信状态
* @param[in]： 入参说明
* @param[out]： 出参无
* @return： 返回值说明
*********************************************************************
*/
void RemoteDraw(HDC hdc)
{
       g_tShowRmtNum = advPd_get_rmt(&g_tShowRmt);
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;
    char czBuf[64] = { 0 };

    int nLeft2 = 5;
    int nTop = 26;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "遥信状态");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);


    SelectFont(hdc, g_guiComm.fontSmall);

    int i = 0;
    nTop = 44;
    for (i = 0; i < 8; i++)
    {
        char Road[128] = { 0 };
        sprintf(Road, "%d", i + 1);

        if (i % 2 == 0)
        {
            nTop += 18;
            if (((g_tShowRmtNum >> i) & 0x01) == 0x00) {
                strcat(Road, "路:未接入");
                TextOut(hdc, nLeft2, nTop, Road);
            }
            else {

                if (((g_tShowRmt.ST >> i) & 0x01) == 0x00)
                {
                    strcat(Road, "路      :开");

                    TextOut(hdc, nLeft2, nTop, Road);

                }
                else
                {
                    strcat(Road, "路      :合");

                    TextOut(hdc, nLeft2, nTop, Road);
                }
            }

            nLeft2 += 80;
        }
        else
        {

            if (((g_tShowRmtNum >> i) & 0x01) == 0x00) {
                strcat(Road, "路:未接入");

                TextOut(hdc, nLeft2, nTop, Road);
            }
            else {

                if (((g_tShowRmt.ST >> i) & 0x01) == 0x00)
                {
                    strcat(Road, "路      :开");

                    TextOut(hdc, nLeft2, nTop, Road);

                }
                else
                {
                    strcat(Road, "路      :合");

                    TextOut(hdc, nLeft2, nTop, Road);
                }
            }

            nLeft2 = 5;

        }
    }
}




extern APP_MODE_INFO g_APPModeInfoFunc;
extern DB_CLIENT   g_gui_db_client;

const char* g_showTitleVoltag34[] = { "Ua", "Ub", "Uc" };
const char* g_showTitleEelectric[] = { "Ia", "Ib", "Ic" };
const char* g_showTitleJCPower_Y[] = { "正向有功功率", "A相有功功率", "B相有功功率","C相有功功率" };
const char* g_showTitleJCPower_W[] = { "正向无功功率", "A相无功功率", "B相无功功率","C相无功功率" };
const char* g_showTitleJCPowerFactor[] = { "总功率因数 ", "A相功率因数", "B相功率因数", "C相功率因数" };
const char* g_showSwitchState[] = { "分", "合" };
const char* g_showSwitchFlag[] = { "未接入", "接入" };
const char* g_showCtrlState[] = { "合", "开" };
const char* g_showtaskStatus[] = { "未执行", "执行中", "已执行" };
//接口类型
const char* interfaceType[] =
{
    "CDC-ACM",
    "CDC-ECM",
    "HID",
};
//功能配置
const char* pipeFunc[] =
{
    "管理",
    "远程",
    "PLC/RF",
    "RS-485",
    "遥脉",
    "CAN",
    "M-Bus",
    "控制",
    "模拟量",
    "RS232",
};

const char* g_pczPulseAttr[] = { "正向有功", "正向无功", "反向有功", "反向无功" };
const char* g_pczDirect[] = { " -", "正", "反" };
const char* g_pczOperator[] = { " -", "加", "减" };

typedef struct
{
    uint16  port;
    uint8 ip[4];
    uint8  type;
}COMM_CHANNEL_INFO_T;

typedef struct
{
    OOP_ENERGY_T        penergy;         //正向有功 0x00100200
    OOP_ENERGYS_T       penergys;        //正向无功 0x00300200
    OOP_ENERGY_T        nenergy;         //反向有功 0x00200200
    OOP_ENERGYS_T       nenergys;        //反向无功 0x00400200
    OOP_ENERGY_T        quadrant1;       //第一象限 0x00500200
    OOP_ENERGY_T        quadrant2;       //第二象限 0x00600200
    OOP_ENERGY_T        quadrant3;       //第三象限 0x00700200
    OOP_ENERGY_T        quadrant4;       //第四象限 0x00800200
    OOP_WORD3V_T       vol;             //电压       0x20000200
    OOP_INT3V_T         cur;             //电流       0x20010200
    OOP_INT4V_T         ppower;          //有功功率     0x20040200
    OOP_INT4V_T         npower;          //无功功率     0x20050200
    OOP_LONG4V_T        fpower;          //功率因数     0x200A0200
    OOP_DEMAND_T        pdemand;         //正向有功需量 0x10100200
    OOP_DEMAND_T        ndemand;         //反向有功需量 0x10200200
    CHINESEINFO         chinese_info[10];
}DISPLAY_DATA_INFO;

typedef struct
{
    int32            plus_active_power;         //脉冲有功功率 0x24010500
    int32            plus_reactive_power;       //脉冲无功功率 0x24010600
    int64            gruop_active_power;        //总加组有功功率 0x23010300
    int64            gruop_reactive_power;      //总加组无功功率 0x23010400
    OOP_HENERGYS_T   gruop_day_energy;        //总加组日有功电量 0x23010700
    OOP_HENERGYS_T   gruop_month_energy;      //总加组月有功电量 0x23010900
    OOP_TGSETTING_T  ctrl_seting;             //控制设置状态         0x23011000
    OOP_TGCURSTATE_T ctrl_status;             //控制当前状态         0x23011100
    OOP_PULSESETS_T  plus_param;              //脉冲配置       0x24010400
    OOP_TGSETS_T     gruop_param;             //脉冲配置       0x23010200
}CTRL_DATA_INFO;

CTRL_DATA_INFO g_ctrl_data[8];
uint8            eletype;                 //保电
OOP_8107_CONFIG_T tBuyenergyconfig;
int64      g_wallet[8];

DISPLAY_DATA_INFO display_data;

MOD_INFO_ACK_T g_module_info[5];


int write_normal_data(uint8* DataBuf, uint16 DataLen, OOP_OAD_U Oad, uint16 infonum, uint8 logicid, uint8 classtag)
{
    int ret;
    NOMAL_OAD_T NomalOad;
    NomalOad.logicId = logicid;
    NomalOad.oad = Oad;
    NomalOad.classtag = classtag;
    NomalOad.infoNum = infonum;

    ret = db_write_nomal(g_gui_db_client, &NomalOad, DataBuf, DataLen);
    if (ret != 0)
    {
        GUI_FMT_DEBUG("普通数据写入数据库失败oad %08x  \n", Oad.value);
    }
    return ret;
}
int read_db_noamal_data(DB_CLIENT fp, uint32 oad_value, uint8 logic_id, uint16 info_num, uint8* pbuf, uint32 buf_len)
{
    int ret = 0;
    uint32 len = 0;
    NOMAL_OAD_T nomalOAD = { 0 };

    nomalOAD.logicId = logic_id;
    nomalOAD.infoNum = info_num;
    nomalOAD.oad.value = oad_value;
    ret = db_read_nomal(fp, &nomalOAD, buf_len, pbuf, &len);
    if (ret != ERR_OK || len != buf_len)
    {
        GUI_FMT_DEBUG("db_read_nomal  err ret=%d, recNum=%d!\n", ret, len);
        return -1;
    }

    return 0;
}

int read_db_record_data(DB_CLIENT fp, uint32 oad_value, uint8 logic_id, uint16 info_num, uint8* pbuf, uint32 buf_len)
{
    int                ret = ERR_NORMAL;
    time_t             timep = 0;

    READ_RECORD_T      inReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_METER_T        oopMeter;

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    time(&timep); //获取从1970至今过了多少秒，存入time_t类型的timep

    inReadRecord.recordOAD.logicId = logic_id;
    inReadRecord.recordOAD.infoNum = info_num;
    inReadRecord.cType = COL_NO_STORE;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = timep;
    inReadRecord.sortType = DOWN_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = oad_value;

    inReadRecord.MAC.nNum = 0;

    //    if(app_get_meter_list(info_num, &oopMeter) == ERR_OK)
    {
        //        memcpy(inReadRecord.MAC.value, oopMeter.basic.tsa.add, 6);
        ret = db_read_record(fp, &inReadRecord, buf_len, &stepRecordOut);
        if (ret != ERR_OK)
        {
            GUI_FMT_DEBUG(" db_read_record err = %d!\n", ret);
            return -1;
        }
    }

    memcpy(pbuf, stepRecordOut.record[0].outDataBuf, buf_len);

    return 0;
}

//将字符串形式的ip转换为数组 失败返回false
BOOL commfun_IP(char* ipin, uint8* ipout)
{
    if (ipin == NULL || strlen(ipin) == 0)
        return FALSE;
    char* p1[4];
    char czBuf[20];
    strcpy(czBuf, ipin);

    char* pTmp = p1[0] = czBuf;
    int nIndex = 1;
    while (*pTmp != '\0')
    {
        if (*pTmp == '.')
        {
            *pTmp = '\0';
            pTmp++;
            if (*pTmp == '\0')
            {
                break;
            }
            else
            {
                if (*pTmp > '9' || *pTmp < '0')
                    return FALSE;
                if (nIndex > 4)
                    return FALSE;
                p1[nIndex] = pTmp;
                nIndex++;
            }
        }
        else if (*pTmp > '9' || *pTmp < '0')
        {
            return FALSE;
        }
        pTmp++;
    }
    if (nIndex != 4)
    {
        return FALSE;
    }
    ipout[0] = atoi(p1[0]);
    ipout[1] = atoi(p1[1]);
    ipout[2] = atoi(p1[2]);
    ipout[3] = atoi(p1[3]);
    return TRUE;
}

//正向有功电能示数
void forward_active_power_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 15;
    int nLeft2 = 30;
    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "正向有功电能示值");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    // 正向有功
    for (int i = 0; i < 5; i++)
    {
        nTop += tmpTop;
        switch (i) {
        case 0:TextOut(hdc, nLeft, nTop, "总"); break;
        case 1:TextOut(hdc, nLeft, nTop, "尖"); break;
        case 2:TextOut(hdc, nLeft, nTop, "峰"); break;
        case 3:TextOut(hdc, nLeft, nTop, "平"); break;
        case 4:TextOut(hdc, nLeft, nTop, "谷"); break;
        default:ASSERT(FALSE);
        }

        if (display_data.penergy.nNum == 0 || get_bit_value((uint8 *)&display_data.penergy.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kWh");
        }
        else
        {
            sprintf(czBuf, "%12.4f kWh", (double)display_data.penergy.nValue[i] / 100);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}

//正向无功电能示数
void forward_reactive_power_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 15;
    int nLeft2 = 30;
    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "正向无功电能示值");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    // 正向无功
    for (int i = 0; i < 5; i++)
    {
        nTop += tmpTop;
        switch (i) {
        case 0:TextOut(hdc, nLeft, nTop, "总"); break;
        case 1:TextOut(hdc, nLeft, nTop, "尖"); break;
        case 2:TextOut(hdc, nLeft, nTop, "峰"); break;
        case 3:TextOut(hdc, nLeft, nTop, "平"); break;
        case 4:TextOut(hdc, nLeft, nTop, "谷"); break;
        default:ASSERT(FALSE);
        }

        if (display_data.penergys.nNum == 0 || get_bit_value((uint8 *)&display_data.penergys.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kvarh");
        }
        else
        {
            sprintf(czBuf, "%12.4f kvarh", (double)display_data.penergys.nValue[i] / 100);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}


int g_ac_data_display_drawpage_index;
void ac_forward_power_display_init(DWORD dwPluginId)
{
    memset(&display_data.penergy, 0, sizeof(OOP_ENERGY_T));
    memset(&display_data.penergys, 0, sizeof(OOP_ENERGYS_T));
    //    if(taskid == 0)
    {
        read_db_noamal_data(g_gui_db_client, 0x00100200, 1, 0, (uint8*)&display_data.penergy, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00300200, 1, 0, (uint8*)&display_data.penergys, sizeof(OOP_ENERGYS_T));
    }
    //    else
    {
        //        read_db_record_data(g_gui_db_client, 0x00100200, 0, taskid, (uint8*)&display_data.penergy, sizeof(OOP_ENERGY_T));
        //        read_db_record_data(g_gui_db_client, 0x00300200, 0, taskid, (uint8*)&display_data.penergys, sizeof(OOP_ENERGYS_T));
    }
    g_ac_data_display_drawpage_index = 0;
}

//当前正向电能示数
void forward_power_display_draw(HDC hdc)
{

    switch (g_ac_data_display_drawpage_index)
    {
    case 0://第1页
    {
        forward_active_power_draw(hdc);
        break;
    }
    case 1://第2页
    {
        forward_reactive_power_draw(hdc);
        break;
    }
    }

    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

BOOL power_display_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_DOWN:
    {
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 1)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    {
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}


//当前正向电能示数
PLUGIN_INFO g_layer_ac_forward_power_display = {
    0x11010001/*id*/,
    ac_forward_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    forward_power_display_draw/*Draw*/,
    NULL/*Timer*/,
    power_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

void ac_reverse_power_display_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(&display_data.nenergy, 0, sizeof(OOP_ENERGY_T));
    memset(&display_data.nenergys, 0, sizeof(OOP_ENERGYS_T));
    //    if(taskid == 0)
    {
        read_db_noamal_data(g_gui_db_client, 0x00200200, 1, 0, (uint8*)&display_data.nenergy, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00400200, 1, 0, (uint8*)&display_data.nenergys, sizeof(OOP_ENERGYS_T));
    }
    //    else
    {
        //        read_db_record_data(g_gui_db_client, 0x00200200, 0, taskid, (uint8*)&display_data.nenergy, sizeof(OOP_ENERGY_T));
        //        read_db_record_data(g_gui_db_client, 0x00400200, 0, taskid, (uint8*)&display_data.nenergys, sizeof(OOP_ENERGYS_T));
    }
    g_ac_data_display_drawpage_index = 0;
}
//反向有功电能示数
void reverse_active_power_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 15;
    int nLeft2 = 30;
    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "反向有功电能示值");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    // 反向有功
    for (int i = 0; i < 5; i++)
    {
        nTop += tmpTop;
        switch (i) {
        case 0:TextOut(hdc, nLeft, nTop, "总"); break;
        case 1:TextOut(hdc, nLeft, nTop, "尖"); break;
        case 2:TextOut(hdc, nLeft, nTop, "峰"); break;
        case 3:TextOut(hdc, nLeft, nTop, "平"); break;
        case 4:TextOut(hdc, nLeft, nTop, "谷"); break;
        default:ASSERT(FALSE);
        }

        if (display_data.nenergy.nNum == 0 || get_bit_value((uint8 *)&display_data.nenergy.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kWh");
        }
        else
        {
            sprintf(czBuf, "%12.4f kWh", (double)display_data.nenergy.nValue[i] / 100);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}

//反向无功电能示数
void reverse_reactive_power_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 15;
    int nLeft2 = 30;
    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "反向无功电能示值");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    // 反向无功
    for (int i = 0; i < 5; i++)
    {
        nTop += tmpTop;
        switch (i) {
        case 0:TextOut(hdc, nLeft, nTop, "总"); break;
        case 1:TextOut(hdc, nLeft, nTop, "尖"); break;
        case 2:TextOut(hdc, nLeft, nTop, "峰"); break;
        case 3:TextOut(hdc, nLeft, nTop, "平"); break;
        case 4:TextOut(hdc, nLeft, nTop, "谷"); break;
        default:ASSERT(FALSE);
        }

        if (display_data.nenergys.nNum == 0 || get_bit_value((uint8 *)&display_data.nenergys.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kvarh");
        }
        else
        {
            sprintf(czBuf, "%12.4f kvarh", (double)display_data.nenergys.nValue[i] / 100);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}

//当前反向电能示数
void reverse_power_display_draw(HDC hdc)
{
    switch (g_ac_data_display_drawpage_index)
    {
    case 0://第1页
    {
        reverse_active_power_draw(hdc);
        break;
    }
    case 1://第2页
    {
        reverse_reactive_power_draw(hdc);
        break;
    }
    }
    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

//当前反向电能示数
PLUGIN_INFO g_layer_ac_reverse_power_display = {
    0x11010002/*id*/,
    ac_reverse_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    reverse_power_display_draw/*Draw*/,
    NULL/*Timer*/,
    power_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

void ac_quadrant_power_display_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(&display_data.quadrant1, 0, sizeof(OOP_ENERGY_T));
    memset(&display_data.quadrant2, 0, sizeof(OOP_ENERGY_T));
    memset(&display_data.quadrant3, 0, sizeof(OOP_ENERGY_T));
    memset(&display_data.quadrant4, 0, sizeof(OOP_ENERGY_T));
    //   if(taskid == 0)
    {
        read_db_noamal_data(g_gui_db_client, 0x00500200, 1, 0, (uint8*)&display_data.quadrant1, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00600200, 1, 0, (uint8*)&display_data.quadrant2, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00700200, 1, 0, (uint8*)&display_data.quadrant3, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00800200, 1, 0, (uint8*)&display_data.quadrant4, sizeof(OOP_ENERGY_T));
    }
    //    else
    {
        //        read_db_record_data(g_gui_db_client, 0x00500200, 0, taskid, (uint8*)&display_data.quadrant1, sizeof(OOP_ENERGY_T));
        //        read_db_record_data(g_gui_db_client, 0x00600200, 0, taskid, (uint8*)&display_data.quadrant2, sizeof(OOP_ENERGY_T));
        //        read_db_record_data(g_gui_db_client, 0x00700200, 0, taskid, (uint8*)&display_data.quadrant3, sizeof(OOP_ENERGY_T));
        //        read_db_record_data(g_gui_db_client, 0x00800200, 0, taskid, (uint8*)&display_data.quadrant4, sizeof(OOP_ENERGY_T));
    }
}

//反向有功电能示数
void quadrant_power_draw(HDC hdc, int id, OOP_ENERGY_T quadrant)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 15;
    int nLeft2 = 30;
    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "第%d象限无功电能", id);
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    // 反向有功
    for (int i = 0; i < 5; i++)
    {
        nTop += tmpTop;
        switch (i) {
        case 0:TextOut(hdc, nLeft, nTop, "总"); break;
        case 1:TextOut(hdc, nLeft, nTop, "尖"); break;
        case 2:TextOut(hdc, nLeft, nTop, "峰"); break;
        case 3:TextOut(hdc, nLeft, nTop, "平"); break;
        case 4:TextOut(hdc, nLeft, nTop, "谷"); break;
        default:ASSERT(FALSE);
        }

        if (quadrant.nNum == 0 || get_bit_value((uint8 *)&quadrant.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kvarh");
        }
        else
        {
            sprintf(czBuf, "%12.4f kvarh", (double)quadrant.nValue[i] / 100);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}


//四象限无功电能
void quadrant_power_display_draw(HDC hdc)
{
    switch (g_ac_data_display_drawpage_index)
    {
    case 0://第一页
    {
        // 第一象限无功  
        quadrant_power_draw(hdc, 1, display_data.quadrant1);

        break;
    }
    case 1://第4页
    {
        // 第二象限无功
        quadrant_power_draw(hdc, 2, display_data.quadrant2);
        break;
    }
    case 2://第5页
    {
        // 第三象限无功
        quadrant_power_draw(hdc, 3, display_data.quadrant3);
        break;
    }
    case 3://第6页
    {
        // 第四象限无功
        quadrant_power_draw(hdc, 4, display_data.quadrant4);
        break;
    }

    }
    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

BOOL quadrant_power_display_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_DOWN:
    {
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 3)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    {
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 3;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}


//四象限无功电能
PLUGIN_INFO g_layer_ac_quadrant_power_display = {
    0x11010003/*id*/,
    ac_quadrant_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    quadrant_power_display_draw/*Draw*/,
    NULL/*Timer*/,
    quadrant_power_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

void ac_vol_cur_display_init(DWORD dwPluginId)
{
    memset(&display_data.vol, 0, sizeof(OOP_ENERGY_T));
    memset(&display_data.cur, 0, sizeof(OOP_ENERGYS_T));
    //    if(taskid == 0)
    {
        read_db_noamal_data(g_gui_db_client, 0x20000200, 1, 0, (uint8*)&display_data.vol, sizeof(OOP_WORD3V_T));
        read_db_noamal_data(g_gui_db_client, 0x20010200, 1, 0, (uint8*)&display_data.cur, sizeof(OOP_INT3V_T));
    }
    //   else
    {
        //       read_db_record_data(g_gui_db_client, 0x20000200, 0, taskid, (uint8*)&display_data.vol, sizeof(OOP_DWORD3V_T));
        //        read_db_record_data(g_gui_db_client, 0x20010200, 0, taskid, (uint8*)&display_data.cur, sizeof(OOP_INT4V_T));
    }

}

void vol_cur_display_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    float tmp;
    int i = 0;

    char czBuf[64] = { 0 };

    int nLeft2 = 50;
    int nTop = 42;
    int tmpTop = 14;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "电压电流");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontBig);
    TextOut(hdc, 14, nTop, "电压");
    SelectFont(hdc, g_guiComm.fontSmall);

    // 电压
    for (i = 0; i < 3; i++)
    {
        if (display_data.vol.nNum == 0 || get_bit_value((uint8 *)&display_data.vol.rsv,32,i) == 0)
        {
            sprintf(czBuf, "%s --- V", g_showTitleVoltag34[i]);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            tmp = (float)display_data.vol.nValue[i] / 10;
            sprintf(czBuf, "%s %2.1f V", g_showTitleVoltag34[i], tmp);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        nTop += tmpTop;
    }

    SelectFont(hdc, g_guiComm.fontBig);
    TextOut(hdc, 14, nTop, "电流");
    SelectFont(hdc, g_guiComm.fontSmall);
    // 电流
    for (i = 0; i < 3; i++)
    {
        if (display_data.cur.nNum == 0 || get_bit_value((uint8 *)&display_data.cur.rsv,32,i) == 0)
        {
            sprintf(czBuf, "%s --- A", g_showTitleEelectric[i]);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            if (display_data.cur.nValue[i] < 0)
            {
                tmp = ~display_data.cur.nValue[i] + 1;
                tmp = tmp / 1000.0;
                sprintf(czBuf, "%s -%3.3f A", g_showTitleEelectric[i], tmp);
            }
            else
            {
                tmp = display_data.cur.nValue[i];
                tmp = tmp / 1000.0;
                sprintf(czBuf, "%s %3.3f A", g_showTitleEelectric[i], tmp);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        nTop += tmpTop;

    }
}

//电压电流数据
PLUGIN_INFO g_layer_ac_vol_cur_display = {
    0x11010004/*id*/,
    ac_vol_cur_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    vol_cur_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
void ac_power_display_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(&display_data.ppower, 0, sizeof(OOP_INT4V_T));
    memset(&display_data.npower, 0, sizeof(OOP_INT4V_T));
    memset(&display_data.fpower, 0, sizeof(OOP_LONG4V_T));
    //    if(taskid == 0)
    {
        read_db_noamal_data(g_gui_db_client, 0x20040200, 1, 0, (uint8*)&display_data.ppower, sizeof(OOP_INT4V_T));
        read_db_noamal_data(g_gui_db_client, 0x20050200, 1, 0, (uint8*)&display_data.npower, sizeof(OOP_INT4V_T));
        read_db_noamal_data(g_gui_db_client, 0x200a0200, 1, 0, (uint8*)&display_data.fpower, sizeof(OOP_LONG4V_T));
    }
    //    else
    {
        //        read_db_record_data(g_gui_db_client, 0x20040200, 0, taskid, (uint8*)&display_data.ppower, sizeof(OOP_INT4V_T));
        //        read_db_record_data(g_gui_db_client, 0x20050200, 0, taskid, (uint8*)&display_data.npower, sizeof(OOP_INT4V_T));
        //        read_db_record_data(g_gui_db_client, 0x200a0200, 0, taskid, (uint8*)&display_data.fpower, sizeof(OOP_INT4V_T));
    }

}

void power_display_draw_param(HDC hdc, const char* pname, const char** item, OOP_INT4V_T power, const char* punit, int multiple)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };
    float tmp;

    int nLeft = 5;
    int nLeft2 = 80;
    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "%s", pname);
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    for (int i = 0; i < 4; i++)
    {
        nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, item[i]);
        if (power.nNum == 0 || get_bit_value((uint8 *)&power.rsv,32,i) == 0)
        {
            sprintf(czBuf, "-------- %s", punit);
        }
        else
        {
            if (power.nValue[i] < 0)
            {
                tmp = ~power.nValue[i] + 1;
                sprintf(czBuf, "-%6.4f %s", tmp / multiple, punit);
            }
            else
            {
                tmp = power.nValue[i];
                sprintf(czBuf, "%6.4f %s", tmp / multiple, punit);
            }
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}

void power_display_draw_params(HDC hdc, const char* pname, const char** item, OOP_LONG4V_T power, const char* punit, int multiple)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };
    float tmp;

    int nLeft = 5;
    int nLeft2 = 80;
    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "%s", pname);
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    for (int i = 0; i < 4; i++)
    {
        nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, item[i]);
        if (power.nNum == 0 || get_bit_value((uint8 *)&power.rsv,32,i) == 0)
        {
            sprintf(czBuf, "-------- %s", punit);
        }
        else
        {
            if (power.nValue[i] < 0)
            {
                tmp = ~power.nValue[i] + 1;
                sprintf(czBuf, "-%4.3f %s", tmp / multiple, punit);
            }
            else
            {
                tmp = power.nValue[i];
                sprintf(czBuf, "%4.3f %s", tmp / multiple, punit);
            }
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}


void power_display_draw(HDC hdc)
{

    switch (g_ac_data_display_drawpage_index)
    {
    case 0://第1页
    {
        // 有功功率
        power_display_draw_param(hdc, "有功功率", g_showTitleJCPower_Y, display_data.ppower, "kw", 10000);
        break;
    }
    case 1://第2页
    {
        // 无功功率
        power_display_draw_param(hdc, "无功功率", g_showTitleJCPower_W, display_data.npower, "kvar", 10000);
        break;
    }
    case 2://第3页
    {
        // 功率因数
        power_display_draw_params(hdc, "功率因数", g_showTitleJCPowerFactor, display_data.fpower, "", 1000);
        break;
    }
    }

    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

BOOL powers_display_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_DOWN:
    {
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 2)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    {
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 2;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}

//当前功率示数
PLUGIN_INFO g_layer_ac_power_display = {
    0x11010005/*id*/,
    ac_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    power_display_draw/*Draw*/,
    NULL/*Timer*/,
    powers_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

void ac_demand_display_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(&display_data.pdemand, 0, sizeof(OOP_DEMAND_T));
    memset(&display_data.ndemand, 0, sizeof(OOP_DEMAND_T));

    //    if(taskid == 0)
    {
        read_db_noamal_data(g_gui_db_client, 0x10100200, 1, 0, (uint8*)&display_data.pdemand, sizeof(OOP_DEMAND_T));
        read_db_noamal_data(g_gui_db_client, 0x10200200, 1, 0, (uint8*)&display_data.ndemand, sizeof(OOP_DEMAND_T));
    }
    //    else
    {
        //        read_db_record_data(g_gui_db_client, 0x10100200, 0, taskid, (uint8*)&display_data.pdemand, sizeof(OOP_DEMAND_T));
        //        read_db_record_data(g_gui_db_client, 0x10200200, 0, taskid, (uint8*)&display_data.ndemand, sizeof(OOP_DEMAND_T));
    }
}

void demand_display_draw_param(HDC hdc, const char* pname, OOP_DEMAND_T demand)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 8;
    int nLeft1 = 22;
    int nLeft2 = 45;
    int nTop = 26;
    int tmpTop = 16;
    int index = g_ac_data_display_drawpage_index;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "%s", pname);
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    for (int i = (index%2)*3; i < 5; i++)
    {
        nTop += tmpTop;
        switch (i) {
        case 0:TextOut(hdc, nLeft1, nTop, "总:"); break;
        case 1:TextOut(hdc, nLeft1, nTop, "尖:"); break;
        case 2:TextOut(hdc, nLeft1, nTop, "峰:"); break;
        case 3:TextOut(hdc, nLeft1, nTop, "平:"); break;
        case 4:TextOut(hdc, nLeft1, nTop, "谷:"); break;
        default:ASSERT(FALSE);
        }

        if (demand.nNum == 0 || get_bit_value((uint8 *)&demand.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kW");
        }
        else
        {
            sprintf(czBuf, "%10.4f kW", (double)demand.demand[i].nValue / 10000);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "时间:");
        if (demand.nNum == 0)
        {
            sprintf(czBuf, "------------ ");
        }
        else
        {
            sprintf(czBuf, "%hu-%02hu-%02hu %02hu:%02hu:%02hu",
                demand.demand[0].time.year,
                demand.demand[0].time.month,
                demand.demand[0].time.day,
                demand.demand[0].time.hour,
                demand.demand[0].time.minute,
                demand.demand[0].time.second);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}

void demand_display_draw(HDC hdc)
{
    switch (g_ac_data_display_drawpage_index)
    {
    case 0://第1页
    case 1://第2页
    {
        // 正向有功需量
        demand_display_draw_param(hdc, "正向有功最大需量", display_data.pdemand);

        break;
    }
    case 2://第3页
    case 3://第4页
    {
        // 反向有功需量
        demand_display_draw_param(hdc, "反向有功最大需量", display_data.ndemand);
        break;
    }
    }

    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

BOOL demand_display_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_DOWN:
    {
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 3)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    {
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 3;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}

//正反向有功需量
PLUGIN_INFO g_layer_ac_demand_display = {
    0x11010006/*id*/,
    ac_demand_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    demand_display_draw/*Draw*/,
    NULL/*Timer*/,
    demand_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};


MEASURE_MAIN g_measure_main_data;

void data_display_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(&display_data, 0, sizeof(DISPLAY_DATA_INFO));
    if (g_guiComm.nCurPoint == 0)
    {
        read_db_noamal_data(g_gui_db_client, 0x00100200, 1, 0, (uint8*)&display_data.penergy, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00300200, 1, 0, (uint8*)&display_data.penergys, sizeof(OOP_ENERGYS_T));
        read_db_noamal_data(g_gui_db_client, 0x00200200, 1, 0, (uint8*)&display_data.nenergy, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00400200, 1, 0, (uint8*)&display_data.nenergys, sizeof(OOP_ENERGYS_T));
        read_db_noamal_data(g_gui_db_client, 0x00500200, 1, 0, (uint8*)&display_data.quadrant1, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00600200, 1, 0, (uint8*)&display_data.quadrant2, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00700200, 1, 0, (uint8*)&display_data.quadrant3, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00800200, 1, 0, (uint8*)&display_data.quadrant4, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x20000200, 1, 0, (uint8*)&display_data.vol, sizeof(OOP_WORD3V_T));
        read_db_noamal_data(g_gui_db_client, 0x20010200, 1, 0, (uint8*)&display_data.cur, sizeof(OOP_INT3V_T));
        read_db_noamal_data(g_gui_db_client, 0x20040200, 1, 0, (uint8*)&display_data.ppower, sizeof(OOP_INT4V_T));
        read_db_noamal_data(g_gui_db_client, 0x20050200, 1, 0, (uint8*)&display_data.npower, sizeof(OOP_INT4V_T));
        read_db_noamal_data(g_gui_db_client, 0x200a0200, 1, 0, (uint8*)&display_data.fpower, sizeof(OOP_LONG4V_T));
        read_db_noamal_data(g_gui_db_client, 0x10100200, 1, 0, (uint8*)&display_data.pdemand, sizeof(OOP_DEMAND_T));
        read_db_noamal_data(g_gui_db_client, 0x10200200, 1, 0, (uint8*)&display_data.ndemand, sizeof(OOP_DEMAND_T));
    }
    else
    {
        read_db_record_data(g_gui_db_client, 0x00100200, 0, g_guiComm.nCurPoint, (uint8*)&display_data.penergy, sizeof(OOP_ENERGY_T));
        read_db_record_data(g_gui_db_client, 0x00300200, 0, g_guiComm.nCurPoint, (uint8*)&display_data.penergys, sizeof(OOP_ENERGYS_T));
        read_db_record_data(g_gui_db_client, 0x00200200, 0, g_guiComm.nCurPoint, (uint8*)&display_data.nenergy, sizeof(OOP_ENERGY_T));
        read_db_record_data(g_gui_db_client, 0x00400200, 0, g_guiComm.nCurPoint, (uint8*)&display_data.nenergys, sizeof(OOP_ENERGYS_T));
        read_db_record_data(g_gui_db_client, 0x00500200, 0, g_guiComm.nCurPoint, (uint8*)&display_data.quadrant1, sizeof(OOP_ENERGY_T));
        read_db_record_data(g_gui_db_client, 0x00600200, 0, g_guiComm.nCurPoint, (uint8*)&display_data.quadrant2, sizeof(OOP_ENERGY_T));
        read_db_record_data(g_gui_db_client, 0x00700200, 0, g_guiComm.nCurPoint, (uint8*)&display_data.quadrant3, sizeof(OOP_ENERGY_T));
        read_db_record_data(g_gui_db_client, 0x00800200, 0, g_guiComm.nCurPoint, (uint8*)&display_data.quadrant4, sizeof(OOP_ENERGY_T));
        read_db_record_data(g_gui_db_client, 0x20000200, 0, g_guiComm.nCurPoint, (uint8*)&display_data.vol, sizeof(OOP_WORD3V_T));
        read_db_record_data(g_gui_db_client, 0x20010200, 0, g_guiComm.nCurPoint, (uint8*)&display_data.cur, sizeof(OOP_INT3V_T));
        read_db_record_data(g_gui_db_client, 0x20040200, 0, g_guiComm.nCurPoint, (uint8*)&display_data.ppower, sizeof(OOP_INT4V_T));
        read_db_record_data(g_gui_db_client, 0x20050200, 0, g_guiComm.nCurPoint, (uint8*)&display_data.npower, sizeof(OOP_INT4V_T));
        read_db_record_data(g_gui_db_client, 0x200a0200, 0, g_guiComm.nCurPoint, (uint8*)&display_data.fpower, sizeof(OOP_LONG4V_T));
        read_db_record_data(g_gui_db_client, 0x10100200, 0, g_guiComm.nCurPoint, (uint8*)&display_data.pdemand, sizeof(OOP_DEMAND_T));
        read_db_record_data(g_gui_db_client, 0x10200200, 0, g_guiComm.nCurPoint, (uint8*)&display_data.ndemand, sizeof(OOP_DEMAND_T));
    }

}

void data_display_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    float tmp;
    int i = 0;

    char czBuf[64] = { 0 };

    int nLeft = 1;
    int nLeft2 = 50;                    //数值超出屏幕外减掉5
    int nTop = 10;
    int tmpTop = 18;

    switch (g_ac_data_display_drawpage_index)
    {
    case 0://第一页
    {
        // 正向有功
        for (int i = 0; i < 5; i++)
        {
            nTop += tmpTop;
            switch (i) {
            case 0:TextOut(hdc, nLeft, nTop, "正有功总"); break;
            case 1:TextOut(hdc, nLeft, nTop, "正有功尖"); break;
            case 2:TextOut(hdc, nLeft, nTop, "正有功峰"); break;
            case 3:TextOut(hdc, nLeft, nTop, "正有功平"); break;
            case 4:TextOut(hdc, nLeft, nTop, "正有功谷"); break;
            default:ASSERT(FALSE);
            }

            if (display_data.penergy.nNum == 0)
            {
                sprintf(czBuf, "------------ kWh");
            }
            else
            {
                sprintf(czBuf, "%12.4f kWh", (double)display_data.penergy.nValue[i] / 100);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        // 正向无功
        nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "正无功总");

        if (display_data.penergys.nNum == 0)
        {
            sprintf(czBuf, "------------ kvarh");
        }
        else
        {
            sprintf(czBuf, "%12.4f kvarh", (double)display_data.penergys.nValue[0] / 100);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);    //显示右边超出屏幕外

        break;
    }
    case 1://第2页
    {
        // 反向有功
        for (int i = 0; i < 5; i++)
        {
            nTop += tmpTop;
            switch (i) {
            case 0:TextOut(hdc, nLeft, nTop, "反有功总"); break;
            case 1:TextOut(hdc, nLeft, nTop, "反有功尖"); break;
            case 2:TextOut(hdc, nLeft, nTop, "反有功峰"); break;
            case 3:TextOut(hdc, nLeft, nTop, "反有功平"); break;
            case 4:TextOut(hdc, nLeft, nTop, "反有功谷"); break;
            default:ASSERT(FALSE);
            }

            if (display_data.penergy.nNum == 0)
            {
                sprintf(czBuf, "------------ kWh");
            }
            else
            {
                sprintf(czBuf, "%12.4f kWh", (double)display_data.nenergy.nValue[i] / 100);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        // 反向无功
        nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "反无功总");

        if (display_data.penergys.nNum == 0)
        {
            sprintf(czBuf, "------------ kvarh");
        }
        else
        {
            sprintf(czBuf, "%12.4f kvarh", (double)display_data.nenergys.nValue[0] / 100);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        break;
    }
    case 2://第3页
    {
        // 第一象限无功
        nTop += 10;
        for (int i = 0; i < 5; i++)
        {
            nTop += tmpTop;
            switch (i) {
            case 0:TextOut(hdc, nLeft, nTop, "第一象总"); break;
            case 1:TextOut(hdc, nLeft, nTop, "第一象尖"); break;
            case 2:TextOut(hdc, nLeft, nTop, "第一象峰"); break;
            case 3:TextOut(hdc, nLeft, nTop, "第一象平"); break;
            case 4:TextOut(hdc, nLeft, nTop, "第一象谷"); break;
            default:ASSERT(FALSE);
            }

            if (display_data.penergy.nNum == 0)
            {
                sprintf(czBuf, "------------ kvarh");
            }
            else
            {
                sprintf(czBuf, "%12.4f kvarh", (double)display_data.quadrant1.nValue[i] / 100);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        break;
    }
    case 3://第4页
    {
        // 第二象限无功
        nTop += 10;
        for (int i = 0; i < 5; i++)
        {
            nTop += tmpTop;
            switch (i) {
            case 0:TextOut(hdc, nLeft, nTop, "第二象总"); break;
            case 1:TextOut(hdc, nLeft, nTop, "第二象尖"); break;
            case 2:TextOut(hdc, nLeft, nTop, "第二象峰"); break;
            case 3:TextOut(hdc, nLeft, nTop, "第二象平"); break;
            case 4:TextOut(hdc, nLeft, nTop, "第二象谷"); break;
            default:ASSERT(FALSE);
            }

            if (display_data.penergy.nNum == 0)
            {
                sprintf(czBuf, "------------ kvarh");
            }
            else
            {
                sprintf(czBuf, "%12.4f kvarh", (double)display_data.quadrant2.nValue[i] / 100);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        break;
    }
    case 4://第5页
    {
        // 第三象限无功
        nTop += 10;
        for (int i = 0; i < 5; i++)
        {
            nTop += tmpTop;
            switch (i) {
            case 0:TextOut(hdc, nLeft, nTop, "第三象总"); break;
            case 1:TextOut(hdc, nLeft, nTop, "第三象尖"); break;
            case 2:TextOut(hdc, nLeft, nTop, "第三象峰"); break;
            case 3:TextOut(hdc, nLeft, nTop, "第三象平"); break;
            case 4:TextOut(hdc, nLeft, nTop, "第三象谷"); break;
            default:ASSERT(FALSE);
            }

            if (display_data.penergy.nNum == 0)
            {
                sprintf(czBuf, "------------ kvarh");
            }
            else
            {
                sprintf(czBuf, "%12.4f kvarh", (double)display_data.quadrant3.nValue[i] / 100);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        break;
    }
    case 5://第6页
    {
        // 第四象限无功
        nTop += 10;
        for (int i = 0; i < 5; i++)
        {
            nTop += tmpTop;
            switch (i) {
            case 0:TextOut(hdc, nLeft, nTop, "第四象总"); break;
            case 1:TextOut(hdc, nLeft, nTop, "第四象尖"); break;
            case 2:TextOut(hdc, nLeft, nTop, "第四象峰"); break;
            case 3:TextOut(hdc, nLeft, nTop, "第四象平"); break;
            case 4:TextOut(hdc, nLeft, nTop, "第四象谷"); break;
            default:ASSERT(FALSE);
            }

            if (display_data.penergy.nNum == 0)
            {
                sprintf(czBuf, "------------ kvarh");
            }
            else
            {
                sprintf(czBuf, "%12.4f kvarh", (double)display_data.quadrant4.nValue[i] / 100);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        break;
    }
    case 6://第7页
    {
        /*        // 电压
                for(i=0;i<3;i++)
                {
                    nTop += tmpTop;
                    TextOut(hdc, nLeft, nTop, g_showTitleVoltag34[i]);
                    tmp = display_data.vol.nValue[i]/10;
                    sprintf(czBuf, "%2.1f V", tmp);
                    TextOut(hdc, nLeft2, nTop, czBuf);
                }

                // 电流
                for(i=0;i<3;i++)
                {
                    nTop += tmpTop;
                    TextOut(hdc, nLeft, nTop, g_showTitleEelectric[i]);
                    if(display_data.cur.nValue[i] < 0)
                    {
                        tmp = ~display_data.cur.nValue[i] + 1;
                        tmp = tmp/1000.0;
                        sprintf(czBuf, "-%3.3f A", tmp);
                    }
                    else
                    {
                        tmp = display_data.cur.nValue[i];
                        tmp = tmp/1000.0;
                        sprintf(czBuf, "%3.3f A", tmp);
                    }
                    TextOut(hdc, nLeft2, nTop, czBuf);
                }
         */
        vol_cur_display_draw(hdc);
        break;
    }
    case 7://第8页
    {
        nLeft2 = 75;
        // 有功功率
        nTop += 10;
        for (i = 0; i < 4; i++)
        {
            nTop += tmpTop - 4;
            TextOut(hdc, nLeft, nTop, g_showTitleJCPower_Y[i]);
            if (display_data.ppower.nNum == 0)
            {
                sprintf(czBuf, "-------- kw");
            }
            else
            {
                if (display_data.ppower.nValue[i] < 0)
                {
                    tmp = ~display_data.ppower.nValue[i] + 1;
                    sprintf(czBuf, "-%6.4f %s", tmp / 10000, "kw");
                }
                else
                {
                    tmp = display_data.ppower.nValue[i];
                    sprintf(czBuf, "%6.4f %s", tmp / 10000, "kw");
                }
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

 


        break;
    }
    case 8://第9页
    {
        nLeft2 = 75;
        // 无功功率
        nTop += 10;
        for (i = 0; i < 4; i++)
        {
            nTop += tmpTop;
            TextOut(hdc, nLeft, nTop, g_showTitleJCPower_W[i]);
            tmp = display_data.vol.nValue[i] / 10;
            if (display_data.npower.nNum == 0)
            {
                sprintf(czBuf, "-------- kvar");
            }
            else
            {
                if (display_data.npower.nValue[i] < 0)
                {
                    tmp = ~display_data.npower.nValue[i] + 1;
                    sprintf(czBuf, "-%6.4f %s", tmp / 10000, "kvar");
                }
                else
                {
                    tmp = display_data.npower.nValue[i];
                    sprintf(czBuf, "%6.4f %s", tmp / 10000, "kvar");
                }
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        break;
    }
    case 9://第9页
    {
        nLeft2 = 75;
        // 功率因数
        nTop += 10;
        for (i = 0; i < 4; i++)
        {
            nTop += tmpTop;
            TextOut(hdc, nLeft, nTop, g_showTitleJCPowerFactor[i]);
            if (display_data.fpower.nNum == 0)
            {
                sprintf(czBuf, "--");
            }
            else
            {
                if (display_data.fpower.nValue[i] < 0)
                {
                    tmp = ~display_data.fpower.nValue[i] + 1;
                    sprintf(czBuf, "-%4.3f", tmp / 1000.0);
                }
                else
                {
                    tmp = display_data.fpower.nValue[i];
                    sprintf(czBuf, "%4.3f", tmp / 1000.0);
                }
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        break;
    }
    case 10://第10页
    {
        // 正向有功需量
        nLeft2 = 55;
        nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "时间");
        if (display_data.pdemand.nNum == 0)
        {
            sprintf(czBuf, "------------ ");
        }
        else
        {
            sprintf(czBuf, "%hu-%hu-%hu %u:%u:%u",
                display_data.pdemand.demand[0].time.year,
                display_data.pdemand.demand[0].time.month,
                display_data.pdemand.demand[0].time.day,
                display_data.pdemand.demand[0].time.hour,
                display_data.pdemand.demand[0].time.minute,
                display_data.pdemand.demand[0].time.second);
        }
        TextOut(hdc, nLeft2 - 20, nTop, czBuf);

        for (int i = 0; i < 5; i++)
        {
            nTop += tmpTop;
            switch (i) {
            case 0:TextOut(hdc, nLeft, nTop, "正有功总"); break;
            case 1:TextOut(hdc, nLeft, nTop, "正有功尖"); break;
            case 2:TextOut(hdc, nLeft, nTop, "正有功峰"); break;
            case 3:TextOut(hdc, nLeft, nTop, "正有功平"); break;
            case 4:TextOut(hdc, nLeft, nTop, "正有功谷"); break;
            default:ASSERT(FALSE);
            }

            if (display_data.penergy.nNum == 0)
            {
                sprintf(czBuf, "------------ kW");
            }
            else
            {
                sprintf(czBuf, "%12.4f kW", (double)display_data.pdemand.demand[i].nValue / 10000);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        break;
    }
    case 11://第11页
    {
        // 反向有功需量
        nLeft2 = 55;
        nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "时间");
        if (display_data.pdemand.nNum == 0)
        {
            sprintf(czBuf, "------------ ");
        }
        else
        {
            sprintf(czBuf, "%hu-%hu-%hu %u:%u:%u",
                display_data.ndemand.demand[0].time.year,
                display_data.ndemand.demand[0].time.month,
                display_data.ndemand.demand[0].time.day,
                display_data.ndemand.demand[0].time.hour,
                display_data.ndemand.demand[0].time.minute,
                display_data.ndemand.demand[0].time.second);
        }
        TextOut(hdc, nLeft2 - 20, nTop, czBuf);

        for (int i = 0; i < 5; i++)
        {
            nTop += tmpTop;
            switch (i) {
            case 0:TextOut(hdc, nLeft, nTop, "反有功总"); break;
            case 1:TextOut(hdc, nLeft, nTop, "反有功尖"); break;
            case 2:TextOut(hdc, nLeft, nTop, "反有功峰"); break;
            case 3:TextOut(hdc, nLeft, nTop, "反有功平"); break;
            case 4:TextOut(hdc, nLeft, nTop, "反有功谷"); break;
            default:ASSERT(FALSE);
            }

            if (display_data.penergy.nNum == 0)
            {
                sprintf(czBuf, "------------ kW");
            }
            else
            {
                sprintf(czBuf, "%12.4f kW", (double)display_data.ndemand.demand[i].nValue / 10000);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
    }
    break;

    }

    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

BOOL data_display_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_DOWN:
    {
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 10)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    {
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 10;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}

//抄表数据
PLUGIN_INFO g_layer_data_display = {
    0x11020002/*id*/,
    data_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    data_display_draw/*Draw*/,
    NULL/*Timer*/,
    data_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

//抄表数据
PLUGIN_INFO* g_measure_plugin[] =
{
    &g_layer_data_display,
};


void measure_data_display_init(DWORD dwPluginId)
{
    g_guiComm.bCanTrunDisp = FALSE;
    g_measure_main_data.ppDetail = NULL;
    g_measure_main_data.pCurPlugin = NULL;
    // 设置当前选择项
    g_guiComm.nCurPoint = 0;
    if (g_guiComm.bKeyDown == TRUE)
    {
        g_measure_main_data.nIndex = 0;
    }
    else
    {
        g_measure_main_data.nIndex = 1;
    }
}


void measure_data_display_draw(HDC hdc)
{
    // 测量点详细信息页面库绘制
    if (g_measure_main_data.pCurPlugin != NULL)
    {
        ASSERT(g_measure_main_data.pCurPlugin->pfnDraw != NULL);
        g_measure_main_data.pCurPlugin->pfnDraw(hdc);
        return;
    }

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    int nTop = 26;
    int nLeft1 = 30;
    TextOut(hdc, nLeft1, nTop, " 抄表数据查看");

    SelectFont(hdc, g_guiComm.fontSmall);
    // 选择测量点
    nLeft1 = 5;
    int nLeft2 = 85;
    nTop += 26;
    TextOut(hdc, nLeft1, nTop, "请选择测量点");
    char czBuf[20];
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);
    if (g_measure_main_data.nIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    /*
        // 测量点状态
        nTop += 16;
        TextOut(hdc, nLeft1, nTop, "测量点状态");
        TextOut(hdc, nLeft2, nTop, g_pczMeasureFlag[g_measure_main_data.nState]);
    */
    nTop += 25;
    if (g_measure_main_data.nIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "详细信息");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "详细信息");
    }

    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

static BOOL measure_data_display_keymsg(int nKey)
{
    // 处理弹出页面的按键消息处理
    // 当前考虑会有多个连续页面，上下翻页面的情况
    // 但是后来只有一个页面了，但是结构还保存，用以以后的扩展
    int nPoint = g_guiComm.nCurPoint;
    if (g_measure_main_data.pCurPlugin != NULL)
    {
        if (g_measure_main_data.pCurPlugin->pfnKeyMsg != NULL
            && TRUE == g_measure_main_data.pCurPlugin->pfnKeyMsg(nKey))
        {
            return TRUE;
        }
        // 使用缺省操作
        switch (nKey)
        {
        case KEY_CANCEL:
            if (g_measure_main_data.pCurPlugin->pfnUninitLayer != NULL)
            {
                g_measure_main_data.pCurPlugin->pfnUninitLayer();
                g_guiComm.nCurPoint = nPoint;
            }
            g_measure_main_data.pCurPlugin = NULL;
            g_guiComm.nArrow = -1;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        case KEY_UP:// 向上翻页面（在有多个页面的情况下）
            if (g_measure_main_data.nPluginCnt > 1)
            {
                g_guiComm.bKeyDown = FALSE;
                if (g_measure_main_data.pCurPlugin->pfnUninitLayer != NULL)
                {
                    g_measure_main_data.pCurPlugin->pfnUninitLayer();
                    g_guiComm.nCurPoint = nPoint;
                }
                g_measure_main_data.pCurPlugin = NULL;

                g_measure_main_data.nCurPlugin--;
                if (g_measure_main_data.nCurPlugin < 0)
                {
                    g_measure_main_data.nCurPlugin = g_measure_main_data.nPluginCnt - 1;
                }
                g_measure_main_data.pCurPlugin = g_measure_main_data.ppDetail[g_measure_main_data.nCurPlugin];
                ASSERT(g_measure_main_data.pCurPlugin != NULL);

                if (g_measure_main_data.pCurPlugin->pfnInitLayer != NULL)
                {
                    g_measure_main_data.pCurPlugin->pfnInitLayer(g_measure_main_data.pCurPlugin->dwId);
                }

                if (g_measure_main_data.nCurPlugin > 0 && g_measure_main_data.nCurPlugin < g_measure_main_data.nPluginCnt - 1)
                {
                    g_guiComm.nArrow = 0;
                }
                else if (g_measure_main_data.nCurPlugin > 0)
                {
                    g_guiComm.nArrow = 1;
                }
                else if (g_measure_main_data.nCurPlugin < g_measure_main_data.nPluginCnt - 1)
                {
                    g_guiComm.nArrow = 2;
                }
                else
                {
                    g_guiComm.nArrow = -1;
                }
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
            }
            break;
        case KEY_DOWN:// 向下翻页面（在有多个页面的情况下）
            if (g_measure_main_data.nPluginCnt > 1)
            {
                g_guiComm.bKeyDown = TRUE;
                if (g_measure_main_data.pCurPlugin->pfnUninitLayer != NULL)
                {
                    g_measure_main_data.pCurPlugin->pfnUninitLayer();
                    g_guiComm.nCurPoint = nPoint;
                }
                g_measure_main_data.pCurPlugin = NULL;

                g_measure_main_data.nCurPlugin++;
                if (g_measure_main_data.nCurPlugin >= g_measure_main_data.nPluginCnt)
                {
                    g_measure_main_data.nCurPlugin = 0;
                }
                g_measure_main_data.pCurPlugin = g_measure_main_data.ppDetail[g_measure_main_data.nCurPlugin];
                ASSERT(g_measure_main_data.pCurPlugin != NULL);

                if (g_measure_main_data.pCurPlugin->pfnInitLayer != NULL)
                {
                    g_measure_main_data.pCurPlugin->pfnInitLayer(g_measure_main_data.pCurPlugin->dwId);
                }

                if (g_measure_main_data.nCurPlugin > 0 && g_measure_main_data.nCurPlugin < g_measure_main_data.nPluginCnt - 1)
                {
                    g_guiComm.nArrow = 0;
                }
                else if (g_measure_main_data.nCurPlugin > 0)
                {
                    g_guiComm.nArrow = 1;
                }
                else if (g_measure_main_data.nCurPlugin < g_measure_main_data.nPluginCnt - 1)
                {
                    g_guiComm.nArrow = 2;
                }
                else
                {
                    g_guiComm.nArrow = -1;
                }
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
            }
            break;
        }
        return TRUE;
    }

    switch (nKey)
    {
    case KEY_UP:
        g_measure_main_data.nIndex--;
        if (g_measure_main_data.nIndex < 0)
        {
            g_measure_main_data.nIndex = 1;
            return FALSE;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_DOWN:
    {
        g_measure_main_data.nIndex++;
        if (g_measure_main_data.nIndex > 1)
        {
            g_measure_main_data.nIndex = 0;
            return FALSE;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_LEFT:
    {
        if (g_measure_main_data.nIndex == 0)
        {
            g_guiComm.nCurPoint--;
            if (g_guiComm.nCurPoint < 0)
            {
                g_guiComm.nCurPoint = MAX_SEL_POINT;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    }
    break;
    case KEY_RIGHT:
    {
        if (g_measure_main_data.nIndex == 0)
        {
            g_guiComm.nCurPoint++;
            if (g_guiComm.nCurPoint > MAX_SEL_POINT)
            {
                g_guiComm.nCurPoint = 0;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    }
    break;
    case KEY_OK:
    {// 确定时处理
        //int nLeft = 90;
        //int nTop = g_measure_main_data.nIndex*16 + 56;
        switch (g_measure_main_data.nIndex)
        {
        case 0:// 弹出测量点选择List
//              PopMeasure(g_guiComm.nCurPoint-1);
            char bzbuf[50];
            sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
            InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        case 1:
            g_measure_main_data.ppDetail = g_measure_plugin;
            g_measure_main_data.nPluginCnt = sizeof(g_measure_plugin) / sizeof(g_measure_plugin[0]);
            g_measure_main_data.nCurPlugin = 0;
            g_measure_main_data.pCurPlugin = g_measure_main_data.ppDetail[g_measure_main_data.nCurPlugin];
            ASSERT(g_measure_main_data.pCurPlugin != NULL);
            if (g_measure_main_data.nPluginCnt > 1)
            {
                g_guiComm.nArrow = 2;
            }
            g_guiComm.bKeyDown = TRUE;
            if (g_measure_main_data.pCurPlugin->pfnInitLayer != NULL)
            {
                g_measure_main_data.pCurPlugin->pfnInitLayer(g_measure_main_data.pCurPlugin->dwId);
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        }
        return TRUE;
    }
    }
    return FALSE;
}


//抄表数据查看
PLUGIN_INFO g_layer_measure_data_display = {
    0x11020001/*id*/,
    measure_data_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    measure_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    measure_data_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};


void restartAPPInit(DWORD dwPluginId)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    g_ac_data_display_drawpage_index = 0;
    pg_guicomm->bCanTrunDisp = FALSE;
}
void restartAPPDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 40;
    rc.bottom = rc.top + 16;
    const char* pcz = "重启应用";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if (g_ac_data_display_drawpage_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "取消");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "取消");
    }

    nLeft = 110;
    if (g_ac_data_display_drawpage_index == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "确定");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "确定");
    }

}

BOOL restartAPPKeyMsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
    case KEY_LEFT:
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 1;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 1)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch (g_ac_data_display_drawpage_index)
        {
        case 0:
            AppCancelMenu();
            return TRUE;
        case 1:
            //            if(ValidatePwd() == TRUE)
        {
            if (FALSE == MessageBox("提示\n是否重启应用\n请再确认一遍", MB_OKCANCEL))
            {
                return TRUE;
            }
            g_ac_data_display_drawpage_index = 0;
            PopMsg("正在重起应用...");
            //appGUI_send_req_handle(MESSAGE_IID_DESKGUI, DESKGUI_IOP_APP_UNREG, NULL, 0);
            exit(0);
        }
        break;
        }
        return TRUE;
    }
    return FALSE;
}
//重启应用
PLUGIN_INFO g_layer_restartAPP =
{
    0x11040002/*id*/,
    restartAPPInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    restartAPPDraw/*Draw*/,
    NULL/*Timer*/,
    restartAPPKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

void data_initialization_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    g_guiComm.bCanTrunDisp = FALSE;
}
void data_initialization_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 40;
    rc.bottom = rc.top + 16;
    const char* pcz = "数据初始化";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if (g_ac_data_display_drawpage_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "取消");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "取消");
    }

    nLeft = 110;
    if (g_ac_data_display_drawpage_index == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "确定");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "确定");
    }

}

int data_initialization()
{
    PopMsg("正在数据初始化...");

    CLEAR_DATA_T ClearData;
    ClearData.recordOAD.logicId = 7;
    ClearData.bClassTag = FALSE;
    int ret = db_clear_data(g_gui_db_client, &ClearData);
    if (ret == 0)
    {
        GUI_FMT_TRACE("db_clear_data ok. ret(%d)\n", ret);
    }
    else
    {
        GUI_FMT_TRACE("db_clear_data failed. ret(%d)\n", ret);
    }
    return ret;

}

BOOL data_initialization_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_LEFT:
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 1)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch (g_ac_data_display_drawpage_index)
        {
        case 0:
            AppCancelMenu();
            return TRUE;
        case 1:
            if (data_initialization() != 0)
            {
                MessageBox("数据初始化失败!");
            }
            else
            {
                MessageBox("数据初始化成功!");
            }
            return TRUE;

        }
        return TRUE;
    }
    return FALSE;
}

//数据初始化
PLUGIN_INFO g_layer_data_initialization =
{
    0x11040003/*id*/,
    data_initialization_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    data_initialization_draw/*Draw*/,
    NULL/*Timer*/,
    data_initialization_keymsg/*keymsg*/,
    NULL/*ipc*/
};


void param_initialization_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    g_guiComm.bCanTrunDisp = FALSE;
}
void param_initialization_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 40;
    rc.bottom = rc.top + 16;
    const char* pcz = "参数初始化";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if (g_ac_data_display_drawpage_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "取消");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "取消");
    }

    nLeft = 110;
    if (g_ac_data_display_drawpage_index == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "确定");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "确定");
    }

}

BOOL param_initialization_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_LEFT:
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 1)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch (g_ac_data_display_drawpage_index)
        {
        case 0:
            AppCancelMenu();
            return TRUE;
        case 1:
            if (db_recover_default(g_gui_db_client, 0) != 0)
            {
                MessageBox("参数初始化失败!");
            }
            else
            {
                MessageBox("参数初始化成功!");
            }
            return TRUE;

        }
        return TRUE;
    }
    return FALSE;
}

//参数初始化
PLUGIN_INFO g_layer_param_initialization =
{
    0x11040004/*id*/,
    param_initialization_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    param_initialization_draw/*Draw*/,
    NULL/*Timer*/,
    param_initialization_keymsg/*keymsg*/,
    NULL/*ipc*/
};


void app_version_Init(DWORD dwPluginId)
{
}

void app_version_Draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;


    char czBuf[64] = { 0 };

    int nLeft = 5;
    int nLeft2 = 15;
    int nTop = 20;
    int tmpTop = 20;

    // 软件版本号
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "软件版本号");

    nTop += tmpTop;
    sprintf(czBuf, "%u", APP_VERSION);
    TextOut(hdc, nLeft2, nTop, czBuf);


    // 发布时间
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "发布时间");
    nTop += tmpTop;
    snprintf(czBuf, 64, "%s", APP_PUBLISH_TIME);
    TextOut(hdc, nLeft2, nTop, czBuf);

    //    CancelKeyDraw(hdc, 3, 16, 12, 12);

}
//版本信息
PLUGIN_INFO g_layer_app_version_display =
{
    0x11040007/*id*/,
    app_version_Init/*InitLayer*/,
    NULL/*UninitLayer*/,
    app_version_Draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

uint8 EthMasterIndex;
OOP_OAD_U CommChanneload;
COMM_CHANNEL_INFO_T CommChannelinfo;
int ret;

void ethmaster101_display_init(HDC hdc)
{
    
    CommChanneload.value=0x45200200;
    memset(&CommChannelinfo,0x00,sizeof(COMM_CHANNEL_INFO_T));
    ret=read_db_noamal_data(g_gui_db_client, CommChanneload.value, 0, 0, (uint8*)&CommChannelinfo, sizeof(COMM_CHANNEL_INFO_T));
    {
        if(ret==0)
        printf("read_db失败");
    }
}
void ethmaster104_display_init(HDC hdc)
{
    CommChanneload.value=0x45200300;
    memset(&CommChannelinfo,0x00,sizeof(COMM_CHANNEL_INFO_T));
    ret=read_db_noamal_data(g_gui_db_client, CommChanneload.value, 0, 0, (uint8*)&CommChannelinfo, sizeof(COMM_CHANNEL_INFO_T));
}

void ethmaster_display_draw(HDC hdc, const char* pitem)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    uint8 index = 0;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf,"%s",pitem);
    rc.top = 20;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    int nTop = rc.top + 30;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "IP地址");
    if (CommChannelinfo.ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", CommChannelinfo.ip[0], CommChannelinfo.ip[1], CommChannelinfo.ip[2], CommChannelinfo.ip[3]);
    }
    if (EthMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "端口");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (CommChannelinfo.port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", CommChannelinfo.port);
    }
    if (EthMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "通道类型");
    if(CommChannelinfo.type>5)
    {
       sprintf(czBuf, "-"); 
    }
    else
    {
        sprintf(czBuf, "%s", g_channel_type[CommChannelinfo.type]);
        
    }
    if (EthMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}

void ethmaster101_display_draw(HDC hdc)
{
    ethmaster_display_draw(hdc, "101通道信息");
}

void ethmaster104_display_draw(HDC hdc)
{
    ethmaster_display_draw(hdc, "104通道信息");
}

//101通信通道查看
PLUGIN_INFO g_layer_ethmaster101_display = {
    0x21130001/*id*/,
    ethmaster101_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    ethmaster101_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//104通信通道查看
PLUGIN_INFO g_layer_ethmaster104_display = {
    0x21130002/*id*/,
    ethmaster104_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    ethmaster104_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};


// uint8 EthMasterIndex;
// OOP_OAD_U CommChanneload;
// COMM_CHANNEL_INFO_T CommChannelinfo;

void commChannel101_set_init(HDC hdc)
{
    CommChanneload.value=0x45200200;
    memset(&CommChannelinfo,0x00,sizeof(COMM_CHANNEL_INFO_T));
    read_db_noamal_data(g_gui_db_client, CommChanneload.value, 0, 0, (uint8*)&CommChannelinfo, sizeof(COMM_CHANNEL_INFO_T));
}

void commChannel_display_draw(HDC hdc, const char* pitem)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    uint8 index = 0;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf,"%s",pitem);
    rc.top = 20;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    int nTop = rc.top + 30;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "IP地址");
    if (CommChannelinfo.ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", CommChannelinfo.ip[0], CommChannelinfo.ip[1], CommChannelinfo.ip[2], CommChannelinfo.ip[3]);
    }
    if (EthMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "端口");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (CommChannelinfo.port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", CommChannelinfo.port);
    }
    if (EthMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "通道类型");
    if(CommChannelinfo.type>5)
    {
       sprintf(czBuf, "-"); 
    }
    else
    {
        sprintf(czBuf, "%s", g_channel_type[CommChannelinfo.type]);
        
    }
    if (EthMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;

    if (EthMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "取消");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "取消");
    }

    index++;
    nLeft2 = 80;
    if (EthMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "保存");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "保存");
    }
    
    PopListDraw(hdc);
}

void commChannel101_display_draw(HDC hdc)
{
    return commChannel_display_draw(hdc,"101通道信息设置");
}

void EthMasterReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[4] = { 0 };

    switch (EthMasterIndex)
    {
    case 0: //IP地址
    {
        if (FALSE == commfun_IP(pNewStr, tmp))
        {
            if (TRUE == MessageBox("IP地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (CommChannelinfo.ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", CommChannelinfo.ip[0], CommChannelinfo.ip[1], CommChannelinfo.ip[2], CommChannelinfo.ip[3]);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(CommChannelinfo.ip, tmp, 4);
        }
    }
    break;
    case 1:     //端口
    {
        int port = atoi(pNewStr);
        if (port <= 65535)
        {
            CommChannelinfo.port = port;
        }
        else
        {
            if (TRUE == MessageBox("端口格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (CommChannelinfo.port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {

                    sprintf(czBuf, "%d", CommChannelinfo.port);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x01);
            }
        }
    }
    break;
    }
}


BOOL commChannel_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 4;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;
        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (EthMasterIndex)
                {
            case 2:
                {
                CommChannelinfo.type = nSel;
                }
            break;
            default:
                return FALSE;
                }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch (nKey)
    {
    case KEY_UP:
    {
        if (EthMasterIndex > 0)
            EthMasterIndex--;
        else
            EthMasterIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    {
        if (EthMasterIndex < maxindex)
            EthMasterIndex++;
        else
            EthMasterIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (ValidatePwd() == TRUE)
        {
            switch (EthMasterIndex)
            {
            case 0://IP地址
            {
                char czBuf[20];
                if (CommChannelinfo.ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", CommChannelinfo.ip[0], CommChannelinfo.ip[1], CommChannelinfo.ip[2], CommChannelinfo.ip[3]);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x02);
                return TRUE;
            }
            case 1://端口
            {
                char czBuf[20];
                if (CommChannelinfo.port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", CommChannelinfo.port);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x01);
                return TRUE;
            }
            case 2://端口类型
            {
                PopList(g_channel_type, 5, g_channel_type[0], 70, 20, FALSE);
                return TRUE;
            }
            case 3://取消
            {
                if (TRUE == MessageBox("当前配置没有保存\n\n是否确定保存配置？", MB_OKCANCEL))
                {
                    ret = write_normal_data((uint8*)&CommChannelinfo, sizeof(COMM_CHANNEL_INFO_T), CommChanneload, 0, 0, 1);
                    if (ret != 0)
                    {
                        MessageBox("通信通道参数写入失败！");
                    }
                }
                EthMasterIndex = 0;
                return TRUE;
            }
            case 4://保存
            {
                ret = write_normal_data((uint8*)&CommChannelinfo, sizeof(COMM_CHANNEL_INFO_T), CommChanneload, 0, 0, 1);
                if (ret != 0)
                {
                    MessageBox("通信通道参数写入失败！");
                }
                else
                {
                    MessageBox("通信通道参数写入成功！");
                }
                EthMasterIndex = 0;
                return TRUE;
            }
            }
        }
    }
    }

    return FALSE;

}

void commChannel104_set_init(HDC hdc)
{
    CommChanneload.value=0x45200300;
    memset(&CommChannelinfo,0x00,sizeof(COMM_CHANNEL_INFO_T));
    read_db_noamal_data(g_gui_db_client, CommChanneload.value, 0, 0, (uint8*)&CommChannelinfo, sizeof(COMM_CHANNEL_INFO_T));
}

void commChannel104_display_draw(HDC hdc)
{
    return commChannel_display_draw(hdc,"104通道信息设置");
}

//101通信通道设置
PLUGIN_INFO g_layer_ethmaster101_set = {
    0x21130011/*id*/,
    commChannel101_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    commChannel101_display_draw/*Draw*/,
    NULL/*Timer*/,
    commChannel_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

//104通信通道设置
PLUGIN_INFO g_layer_ethmaster104_set = {
    0x21130012/*id*/,
    commChannel104_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    commChannel104_display_draw/*Draw*/,
    NULL/*Timer*/,
    commChannel_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};
    
//死区参数0x49010200
typedef struct tag_OOP_DEAD_ZONE
{
    int32           nI;  //电流死区
    uint32          nAcU;  //交流电压死区
    uint32          nDcU;  //直流电压死区
    int32           nPf;  //功率死区
    int16           nFr;  //频率死区
}OOP_DEAD_ZONE_T;

//低电压报警0x49040200
typedef struct tag_OOP_LOW_U
{
    uint32          nLimit;  //报警门限值
    uint16          nDelay;  //报警延时
}OOP_LOW_U_T;

//过电压报警0x49050200
typedef struct tag_OOP_OVER_U
{
    uint32          nLimit;  //报警门限值
    uint16          nDelay;  //报警延时
}OOP_OVER_U_T;

//合格率参数
typedef struct tag_PASS_RATE_PARAM
{
    OOP_DEAD_ZONE_T dead_zone;  //死区参数0x49010200
    OOP_LOW_U_T     low_val;    //低电压报警0x49040200
    OOP_OVER_U_T    over_val;   //过电压报警0x49050200
}PASS_RATE_PARAM_T;

PASS_RATE_PARAM_T g_psaa_rate_param;
uint8 g_pass_rate_param_set_index;

void pass_rate_param_init(HDC hdc)
{
    g_pass_rate_param_set_index = 0;
    memset(&g_psaa_rate_param, 0xff, sizeof(PASS_RATE_PARAM_T));
    read_db_noamal_data(g_gui_db_client, 0x49010200, 1, 0, (uint8*)&g_psaa_rate_param.dead_zone, sizeof(OOP_DEAD_ZONE_T));
    read_db_noamal_data(g_gui_db_client, 0x49040200, 1, 0, (uint8*)&g_psaa_rate_param.low_val, sizeof(OOP_LOW_U_T));
    read_db_noamal_data(g_gui_db_client, 0x49050200, 1, 0, (uint8*)&g_psaa_rate_param.over_val, sizeof(OOP_OVER_U_T));
}

void pass_rate_param_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "合格率参数显示");
    rc.top = 18;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    int nTop = rc.top + 18;
    int nLeft1 = 5;
    int nLeft2 = 80;
    TextOut(hdc, nLeft1, nTop, "低电压限值");
    if (g_psaa_rate_param.low_val.nLimit == 0xffffffff)
    {
        sprintf(czBuf, "------ V");
    }
    else
    {
        sprintf(czBuf, "%3.2f V", (double)g_psaa_rate_param.low_val.nLimit / 100);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 16;
    TextOut(hdc, nLeft1, nTop, "低电压延时");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (g_psaa_rate_param.low_val.nDelay == 0xffff)
    {
        sprintf(czBuf, "------ 秒");
    }
    else
    {
        sprintf(czBuf, "%hu 秒", g_psaa_rate_param.low_val.nDelay);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 16;
    TextOut(hdc, nLeft1, nTop, "过电压限值");
    if (g_psaa_rate_param.over_val.nLimit == 0xffffffff)
    {
        sprintf(czBuf, "------ V");
    }
    else
    {
        sprintf(czBuf, "%3.2f V", (double)g_psaa_rate_param.over_val.nLimit / 100);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 16;
    TextOut(hdc, nLeft1, nTop, "过电压延时");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (g_psaa_rate_param.over_val.nDelay == 0xffff)
    {
        sprintf(czBuf, "------ 秒");
    }
    else
    {
        sprintf(czBuf, "%hu 秒", g_psaa_rate_param.over_val.nDelay);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 16;
    TextOut(hdc, nLeft1, nTop, "交流电压死区");
    if (g_psaa_rate_param.dead_zone.nAcU == 0xffffffff)
    {
        sprintf(czBuf, "------ V");
    }
    else
    {
        sprintf(czBuf, "%3.2f V", (double)g_psaa_rate_param.dead_zone.nAcU / 100);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 16;
    TextOut(hdc, nLeft1, nTop, "直流电压死区");
    if (g_psaa_rate_param.dead_zone.nDcU == 0xffffffff)
    {
        sprintf(czBuf, "------ V");
    }
    else
    {
        sprintf(czBuf, "%3.2f V", (double)g_psaa_rate_param.dead_zone.nDcU / 100);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

}

//合格率参数查看
PLUGIN_INFO g_layer_pass_rate_param_display = {
    0x21130004/*id*/,
    pass_rate_param_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    pass_rate_param_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};


void pass_rate_param_set_draw(HDC hdc)
{

    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "合格率参数设置");
    rc.top = 18;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    uint8 index = 0;

    int nTop = rc.top + 16;
    int nLeft1 = 5;
    int nLeft2 = 80;
    TextOut(hdc, nLeft1, nTop, "低电压限值");
    if (g_psaa_rate_param.low_val.nLimit == 0xffffffff)
    {
        sprintf(czBuf, "------ V");
    }
    else
    {
        sprintf(czBuf, "%3.2f V", (double)g_psaa_rate_param.low_val.nLimit / 100);
    }

    if (g_pass_rate_param_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    index++;

    nTop += 16;
    TextOut(hdc, nLeft1, nTop, "低电压延时");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (g_psaa_rate_param.low_val.nDelay == 0xffff)
    {
        sprintf(czBuf, "------ 秒");
    }
    else
    {
        sprintf(czBuf, "%hu 秒", g_psaa_rate_param.low_val.nDelay);
    }
    if (g_pass_rate_param_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    index++;

    nTop += 16;
    TextOut(hdc, nLeft1, nTop, "过电压限值");
    if (g_psaa_rate_param.over_val.nLimit == 0xffffffff)
    {
        sprintf(czBuf, "------ V");
    }
    else
    {
        sprintf(czBuf, "%3.2f V", (double)g_psaa_rate_param.over_val.nLimit / 100);
    }
    if (g_pass_rate_param_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    index++;

    nTop += 16;
    TextOut(hdc, nLeft1, nTop, "过电压延时");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (g_psaa_rate_param.over_val.nDelay == 0xffff)
    {
        sprintf(czBuf, "------ 秒");
    }
    else
    {
        sprintf(czBuf, "%hu 秒", g_psaa_rate_param.over_val.nDelay);
    }
    if (g_pass_rate_param_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    index++;

    nTop += 16;
    TextOut(hdc, nLeft1, nTop, "交流电压死区");
    if (g_psaa_rate_param.dead_zone.nAcU == 0xffffffff)
    {
        sprintf(czBuf, "------ V");
    }
    else
    {
        sprintf(czBuf, "%3.2f V", (double)g_psaa_rate_param.dead_zone.nAcU / 100);
    }
    if (g_pass_rate_param_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    index++;

    nTop += 16;
    TextOut(hdc, nLeft1, nTop, "直流电压死区");
    if (g_psaa_rate_param.dead_zone.nDcU == 0xffffffff)
    {
        sprintf(czBuf, "------ V");
    }
    else
    {
        sprintf(czBuf, "%3.2f V", (double)g_psaa_rate_param.dead_zone.nDcU / 100);
    }
    if (g_pass_rate_param_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    index++;

    nTop += 16;
    if (g_pass_rate_param_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1 + 10, nTop, "取消");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1 + 10, nTop, "取消");
    }

    index++;
    nLeft2 = 100;
    if (g_pass_rate_param_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "保存");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "保存");
    }

}

extern void pass_rate_paramReturn(char* pNewStr, void* pPara);
void get_param(char* pNewStr, void* poutPara)
{
    int value = atoi(pNewStr);
    char czBuf[20] = { 0 };
    if (value > 0)
    {
        memcpy(poutPara, &value, sizeof(int));
    }
    else
    {
        if (TRUE == MessageBox("端口格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
        {
            if (*(uint8*)poutPara == 0xff)
            {
                strcpy(czBuf, "--");
            }
            else
            {
                sprintf(czBuf, "%d", (uint32)poutPara);
            }
            InputBox(czBuf, pass_rate_paramReturn, NULL, 0x01);
        }
    }
}

void pass_rate_paramReturn(char* pNewStr, void* pPara)
{
    uint32 tmp = 0;

    switch (g_pass_rate_param_set_index)
    {
    case 0:
    {
        tmp = g_psaa_rate_param.low_val.nLimit;
        get_param(pNewStr, (void*)&tmp);
        g_psaa_rate_param.low_val.nLimit = tmp * 100;
    }
    break;
    case 1:
    {
        tmp = g_psaa_rate_param.low_val.nDelay;
        get_param(pNewStr, (void*)&tmp);
        g_psaa_rate_param.low_val.nDelay = tmp;
    }
    break;
    case 2:
    {
        tmp = g_psaa_rate_param.over_val.nLimit;
        get_param(pNewStr, (void*)&tmp);
        g_psaa_rate_param.over_val.nLimit = tmp * 100;
    }
    break;
    case 3:
    {
        tmp = g_psaa_rate_param.over_val.nDelay;
        get_param(pNewStr, (void*)&tmp);
        g_psaa_rate_param.over_val.nDelay = tmp;
    }
    break;
    case 4:
    {
        tmp = g_psaa_rate_param.dead_zone.nAcU;
        get_param(pNewStr, (void*)&tmp);
        g_psaa_rate_param.dead_zone.nAcU = tmp * 100;
    }
    break;
    case 5:
    {
        tmp = g_psaa_rate_param.dead_zone.nDcU;
        get_param(pNewStr, (void*)&tmp);
        g_psaa_rate_param.dead_zone.nDcU = tmp * 100;
    }
    break;
    }
}
BOOL pass_rate_param_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
    case KEY_UP:
    case KEY_LEFT:
    {
        g_pass_rate_param_set_index--;
        if (g_pass_rate_param_set_index < 0)
        {
            g_pass_rate_param_set_index = 7;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        g_pass_rate_param_set_index++;
        if (g_ac_data_display_drawpage_index > 7)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;
    }
    case KEY_OK:
    {
        if (ValidatePwd() == TRUE)
        {
            switch (g_ac_data_display_drawpage_index)
            {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            {
                char czBuf[20];
                strcpy(czBuf, "");
                InputBox(czBuf, pass_rate_paramReturn, NULL, 0x01);
                return TRUE;
            }
            case 6://取消
            {
                g_ac_data_display_drawpage_index = 0;
                return TRUE;
            }
            case 7://保存
            {
                OOP_OAD_U Oad;
                Oad.value = 0x49010200;
                write_normal_data((uint8*)&g_psaa_rate_param.dead_zone, sizeof(OOP_DEAD_ZONE_T), Oad, 0, 1, 1);
                Oad.value = 0x49040200;
                write_normal_data((uint8*)&g_psaa_rate_param.low_val, sizeof(OOP_LOW_U_T), Oad, 0, 1, 1);
                Oad.value = 0x49050200;
                write_normal_data((uint8*)&g_psaa_rate_param.over_val, sizeof(OOP_OVER_U_T), Oad, 0, 1, 1);

                MessageBox("合格率参数设置成功");
                g_ac_data_display_drawpage_index = 0;
                return TRUE;
            }
            }
        }
    }
    }

    return FALSE;
}

//合格率参数设置
PLUGIN_INFO g_layer_pass_rate_param_set = {
    0x21130014/*id*/,
    pass_rate_param_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    pass_rate_param_set_draw/*Draw*/,
    NULL/*Timer*/,
    pass_rate_param_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

OOP_MASTERPARAMS_T g_showPPPMaster;
OOP_MASTERPARAMS_T g_showETHMaster;
uint8 g_ConnectShowStatus[4] = {0};
uint8 g_channeltype = 0;
/**
*********************************************************************
* @brief： 功能描述 初始化主站连接状态
* @param[in]： 入参说明
* @param[out]： 出参无
* @return： 返回值说明
*********************************************************************
*/
void server_master_init(DWORD dwPluginId)
{
    memset(&g_showPPPMaster, 0, sizeof(OOP_MASTERPARAMS_T));
    memset(&g_showETHMaster, 0, sizeof(OOP_MASTERPARAMS_T));
    read_db_noamal_data(g_gui_db_client, 0x45000300, PD_LOGIC_ID, 0, (uint8*)&g_showPPPMaster, sizeof(OOP_MASTERPARAMS_T));
    read_db_noamal_data(g_gui_db_client, 0x45100300, PD_LOGIC_ID, 0, (uint8*)&g_showETHMaster, sizeof(OOP_MASTERPARAMS_T));

#ifdef AREA_ZHEJIANG
    OOP_ZJ_CONNECT_T   tType;
    tType.type = ZJ_CONNECT_NULL;
    read_db_noamal_data(g_gui_db_client, 0x45300200, PD_LOGIC_ID, 0, (uint8*)&tType, sizeof(OOP_ZJ_CONNECT_T));
    g_channeltype = tType.type;
#endif
}

/**
*********************************************************************
* @brief： 功能描述 绘制主站连接状态
* @param[in]： 入参说明
* @param[out]： 出参无
* @return： 返回值说明
*********************************************************************
*/
void server_master_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;
    char czBuf[100] = {0};
    uint16 offset = 0;
    
    int nLeft = 5;
    int nTop = 30;
    int nTemp = 14;
    uint32 ip;
    
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "连接主站状态");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontSmall);
#ifdef AREA_ZHEJIANG
    switch(g_channeltype)
    {
        case ZJ_CONNECT_PUBLIC:  sprintf(czBuf, "%s开启", "无线公网"); break;
        case ZJ_CONNECT_SPECIAL: sprintf(czBuf, "%s开启", "无线专网"); break;
        case ZJ_CONNECT_ETH:     sprintf(czBuf, "%s开启", "以太网");   break;
        case ZJ_CONNECT_RS232:   sprintf(czBuf, "%s开启", "232串口");  break;
        default:                 sprintf(czBuf, "客户端通道关闭");  break;
    }
    nTop += nTemp;
    TextOut(hdc, nLeft, nTop, czBuf);
    nTop += 6;
#endif

    /* 无线主站链接 */
    offset = sprintf(czBuf, "无线 ");
    ip = htonl(*((uint32 *)g_showPPPMaster.master[0].ip));
    if(0 != ip)
    {
        if(TRUE == comm_check_ppp_online(0, ip, g_showPPPMaster.master[0].port) || 1 == g_ConnectShowStatus[0])
        {
            offset += sprintf(czBuf + offset, "%u.%u.%u.%u:%u", 
                          g_showPPPMaster.master[0].ip[0], g_showPPPMaster.master[0].ip[1], g_showPPPMaster.master[0].ip[2], 
                          g_showPPPMaster.master[0].ip[3], g_showPPPMaster.master[0].port);
        }
        
        g_ConnectShowStatus[0] = 1 - g_ConnectShowStatus[0];
    }
    else
    {
        offset += sprintf(czBuf + offset, "-.-.-.-:-");
    }
    nTop += nTemp;
    TextOut(hdc, nLeft, nTop, czBuf);
    
    offset = sprintf(czBuf, "     ");
    ip = htonl(*((uint32 *)g_showPPPMaster.master[1].ip));
    if(0 != ip)
    {
        if(TRUE == comm_check_ppp_online(0, ip, g_showPPPMaster.master[1].port) || 1 == g_ConnectShowStatus[1])
        {
            offset += sprintf(czBuf + offset, "%u.%u.%u.%u:%u", 
                          g_showPPPMaster.master[1].ip[0], g_showPPPMaster.master[1].ip[1], g_showPPPMaster.master[1].ip[2], 
                          g_showPPPMaster.master[1].ip[3], g_showPPPMaster.master[1].port);
        }
        
        g_ConnectShowStatus[1] = 1 - g_ConnectShowStatus[1];
    }
    else
    {
        offset += sprintf(czBuf + offset, "-.-.-.-:-");
    }
    nTop += nTemp;
    TextOut(hdc, nLeft, nTop, czBuf);

    /* 以太网主站链接 */
    offset = sprintf(czBuf, "以太 ");
    ip = htonl(*((uint32 *)g_showETHMaster.master[0].ip));
    if(0 != ip)
    {
        if(TRUE == comm_check_eth_online(0, ip, g_showETHMaster.master[0].port) || 1 == g_ConnectShowStatus[2])
        {
            offset += sprintf(czBuf + offset, "%u.%u.%u.%u:%u", 
                            g_showETHMaster.master[0].ip[0], g_showETHMaster.master[0].ip[1], g_showETHMaster.master[0].ip[2], 
                            g_showETHMaster.master[0].ip[3], g_showETHMaster.master[0].port);
        }
        g_ConnectShowStatus[2] = 1 - g_ConnectShowStatus[2];
    }
    else
    {
        offset += sprintf(czBuf + offset, "-.-.-.-:-");
    }
    nTop += nTemp + 3;
    TextOut(hdc, nLeft, nTop, czBuf);

    offset = sprintf(czBuf, "     ");
    ip = htonl(*((uint32 *)g_showETHMaster.master[1].ip));
    if(0 != ip)
    {
        if(TRUE == comm_check_eth_online(0, ip, g_showETHMaster.master[1].port) || 1 == g_ConnectShowStatus[3])
        {
            offset += sprintf(czBuf + offset, "%u.%u.%u.%u:%u", 
                            g_showETHMaster.master[1].ip[0], g_showETHMaster.master[1].ip[1], g_showETHMaster.master[1].ip[2], 
                            g_showETHMaster.master[1].ip[3], g_showETHMaster.master[1].port);
        }
        g_ConnectShowStatus[3] = 1 - g_ConnectShowStatus[3];
    }
    else
    {
        offset += sprintf(czBuf + offset, "-.-.-.-:-");
    }
    nTop += nTemp;
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += nTemp + 3;
    TextOut(hdc, nLeft, nTop, "串口 无 RS232 设备");
}

uint32 g_showtimer = 0;
void server_master_timer(int nId)
{
    if(nId == TIMER_MAIN)
    {
        GUI_COMM* pg_guicomm = AppChkGuiComm();
        if (pg_guicomm == NULL)
            return;
        g_showtimer++;
        if(1 < g_showtimer)
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            g_showtimer = 0;
        }
    }
}

//连接主站(S)状态
PLUGIN_INFO g_layer_show_server_master = {
    0x21120003/*id*/,
    server_master_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    server_master_draw/*Draw*/,
    server_master_timer/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

/**
*********************************************************************
* @brief： 功能描述 初始化主站连接状态
* @param[in]： 入参说明
* @param[out]： 出参无
* @return： 返回值说明
*********************************************************************
*/
void server_channel_init(DWORD dwPluginId)
{
    return;
}

/**
*********************************************************************
* @brief： 功能描述 绘制主站连接状态
* @param[in]： 入参说明
* @param[out]： 出参无
* @return： 返回值说明
*********************************************************************
*/
void server_channel_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;
    char czBuf[100] = {0};
    
    int nLeft = 5;
    int nTop = 26;
    int nTemp = 14;
    
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "通信通道类型");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

#ifdef AREA_ZHEJIANG
    OOP_ZJ_CONNECT_T   tType;
    tType.type = ZJ_CONNECT_NULL;
    read_db_noamal_data(g_gui_db_client, 0x45300200, PD_LOGIC_ID, 0, (uint8*)&tType, sizeof(OOP_ZJ_CONNECT_T));

    switch(tType.type)
    {
        case ZJ_CONNECT_PUBLIC:  sprintf(czBuf, "%s开启", "无线公网"); break;
        case ZJ_CONNECT_SPECIAL: sprintf(czBuf, "%s开启", "无线专网"); break;
        case ZJ_CONNECT_ETH:     sprintf(czBuf, "%s开启", "以太网");   break;
        case ZJ_CONNECT_RS232:   sprintf(czBuf, "%s开启", "232串口");  break;
        default:                 sprintf(czBuf, "客户端通道关闭");  break;
    }
#else
    sprintf(czBuf, "无功能");
#endif
    nTop += nTemp;
    TextOut(hdc, nLeft, nTop, czBuf);
}


//通信通道类型查看
PLUGIN_INFO g_layer_show_server_channel = {
    0x21130101/*id*/,
    server_channel_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    server_channel_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

OOP_ETHCONFIG_T g_showETHCfg;
uint8 g_bEthCfg;
uint8 g_bEthMaster;
/**
*********************************************************************
* @brief： 功能描述 初始化主站连接状态
* @param[in]： 入参说明
* @param[out]： 出参无
* @return： 返回值说明
*********************************************************************
*/
void server_ethshow_init(DWORD dwPluginId)
{
    memset(&g_showETHMaster, 0, sizeof(OOP_MASTERPARAMS_T));
    g_bEthMaster = FALSE;
    if(0 == read_db_noamal_data(g_gui_db_client, 0x45100300, PD_LOGIC_ID, 0, (uint8*)&g_showETHMaster, sizeof(OOP_MASTERPARAMS_T)))
    {
        g_bEthMaster = TRUE;
    }

    memset(&g_showETHCfg, 0, sizeof(OOP_ETHCONFIG_T));
    g_bEthCfg = FALSE;
    if(0 == read_db_noamal_data(g_gui_db_client, 0x45100200, PD_LOGIC_ID, 0, (uint8*)&g_showETHCfg, sizeof(OOP_ETHCONFIG_T)))
    {
        g_bEthCfg = TRUE;
    }
}

/**
*********************************************************************
* @brief： 功能描述 绘制主站连接状态
* @param[in]： 入参说明
* @param[out]： 出参无
* @return： 返回值说明
*********************************************************************
*/
void server_ethshow_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontBig);
    ITEM_NOT_SELECT;
    char czBuf[300];
    
    
    int nTop = 26;
    int nLeft1 = 10;
    int nLeft2 = 70;
    int nTemp = 14;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "以太网参数");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, pg_guicomm->fontSmall);

    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "连接方式");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bEthCfg || g_showETHCfg.contype > 1)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", g_conn_type[g_showETHCfg.contype]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "心跳周期");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bEthCfg)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d 秒", g_showETHCfg.heart);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "主站地址");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bEthMaster || 0 == htonl(*((uint32 *)g_showETHMaster.master[0].ip)))
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%u.%u.%u.%u", g_showETHMaster.master[0].ip[0], g_showETHMaster.master[0].ip[1], 
                                      g_showETHMaster.master[0].ip[2], g_showETHMaster.master[0].ip[3]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    
    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "主站端口");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bEthMaster || 0 == g_showETHMaster.master[0].port)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%u", g_showETHMaster.master[0].port);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "备用地址");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bEthMaster || 0 == htonl(*((uint32 *)g_showETHMaster.master[1].ip)))
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%u.%u.%u.%u", g_showETHMaster.master[1].ip[0], g_showETHMaster.master[1].ip[1], 
                                      g_showETHMaster.master[1].ip[2], g_showETHMaster.master[1].ip[3]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    
    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "备用端口");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bEthMaster || 0 == g_showETHMaster.master[1].port)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%u", g_showETHMaster.master[1].port);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
}


//以太网查看
PLUGIN_INFO g_layer_show_eth_channel = {
    0x21130102/*id*/,
    server_ethshow_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    server_ethshow_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};


OOP_GPRSCONFIG_T g_showPPPCfg;
uint8 g_bPPPCfg;
uint8 g_bPPPMaster;

/**
*********************************************************************
* @brief： 功能描述 初始化主站连接状态
* @param[in]： 入参说明
* @param[out]： 出参无
* @return： 返回值说明
*********************************************************************
*/
void server_pppshow_init(DWORD dwPluginId)
{
    memset(&g_showPPPMaster, 0, sizeof(OOP_MASTERPARAMS_T));
    g_bPPPMaster = FALSE;
    if(0 == read_db_noamal_data(g_gui_db_client, 0x45000300, PD_LOGIC_ID, 0, (uint8*)&g_showPPPMaster, sizeof(OOP_MASTERPARAMS_T)))
    {
        g_bPPPMaster = TRUE;
    }

    memset(&g_showPPPCfg, 0, sizeof(OOP_GPRSCONFIG_T));
    g_bPPPCfg = FALSE;
    if(0 == read_db_noamal_data(g_gui_db_client, 0x45000200, PD_LOGIC_ID, 0, (uint8*)&g_showPPPCfg, sizeof(OOP_GPRSCONFIG_T)))
    {
        g_bPPPCfg = TRUE;
    }
}

/**
*********************************************************************
* @brief： 功能描述 绘制主站连接状态
* @param[in]： 入参说明
* @param[out]： 出参无
* @return： 返回值说明
*********************************************************************
*/
void server_pppshow_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontBig);
    ITEM_NOT_SELECT;
    char czBuf[300];
    
    int nTop = 26;
    int nLeft1 = 10;
    int nLeft2 = 70;
    int nTemp = 14;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "无线公/专网参数");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, pg_guicomm->fontSmall);

    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "APN");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bPPPCfg || 0 == g_showPPPCfg.apn.apn.nNum)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", g_showPPPCfg.apn.apn.value);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "连接方式");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bPPPCfg || g_showPPPCfg.contype > 1)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", g_conn_type[g_showPPPCfg.contype]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "心跳周期");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bPPPCfg)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d 秒", g_showPPPCfg.heart);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "主站地址");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bPPPMaster || 0 == htonl(*((uint32 *)g_showPPPMaster.master[0].ip)))
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%u.%u.%u.%u", g_showPPPMaster.master[0].ip[0], g_showPPPMaster.master[0].ip[1], 
                                      g_showPPPMaster.master[0].ip[2], g_showPPPMaster.master[0].ip[3]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    
    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "主站端口");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bPPPMaster || 0 == g_showPPPMaster.master[0].port)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%u", g_showPPPMaster.master[0].port);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "备用地址");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bPPPMaster || 0 == htonl(*((uint32 *)g_showPPPMaster.master[1].ip)))
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%u.%u.%u.%u", g_showPPPMaster.master[1].ip[0], g_showPPPMaster.master[1].ip[1], 
                                      g_showPPPMaster.master[1].ip[2], g_showPPPMaster.master[1].ip[3]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    
    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "备用端口");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bPPPMaster || 0 == g_showPPPMaster.master[1].port)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%u", g_showPPPMaster.master[1].port);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
}

//无线参数查看
PLUGIN_INFO g_layer_show_ppp_channel = {
    0x21130103/*id*/,
    server_pppshow_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    server_pppshow_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

uint8 g_ChannelIndex = 0;
uint8 g_UserIndex = 0;  //选择的index

/**
*********************************************************************
* @brief： 功能描述 初始化主站连接状态
* @param[in]： 入参说明
* @param[out]： 出参无
* @return： 返回值说明
*********************************************************************
*/
void client_setchannel_init(DWORD dwPluginId)
{
    g_UserIndex = 0;
#ifdef AREA_ZHEJIANG
    OOP_ZJ_CONNECT_T   tType;
    tType.type = ZJ_CONNECT_NULL;
    read_db_noamal_data(g_gui_db_client, 0x45300200, PD_LOGIC_ID, 0, (uint8*)&tType, sizeof(OOP_ZJ_CONNECT_T));

    switch(tType.type)
    {
        case ZJ_CONNECT_PUBLIC:  g_ChannelIndex = 1; break;
        case ZJ_CONNECT_SPECIAL: g_ChannelIndex = 2; break;
        case ZJ_CONNECT_ETH:     g_ChannelIndex = 3; break;
        case ZJ_CONNECT_RS232:   g_ChannelIndex = 4; break;
        default:                 g_ChannelIndex = 0; break;
    }
#endif

}

/**
*********************************************************************
* @brief： 功能描述 绘制主站连接状态
* @param[in]： 入参说明
* @param[out]： 出参无
* @return： 返回值说明
*********************************************************************
*/
void client_setchannel_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;
    char czBuf[100] = {0};

    int nLeft1 = 10;
    int nTop = 30;
    
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "通信通道类型设置");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontSmall);

#ifndef AREA_ZHEJIANG
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "无此参数设置");
#else
    int nLeft2 = 70;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "通道类型");

    if(2 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, nLeft2, nTop, g_ChannelIndex <= 4 ? g_channel_type[g_ChannelIndex] : "请选择");
    ITEM_NOT_SELECT;
#endif

    SelectFont(hdc, g_guiComm.fontBig);

    if(0 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, 30, 120, "取消");
    ITEM_NOT_SELECT;

    if(1 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, 92, 120, "确认");
    ITEM_NOT_SELECT;

    PopListDraw(hdc);
}

BOOL client_setchannel_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            g_ChannelIndex = nSel;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch (nKey)
    {
        case KEY_UP:
        case KEY_LEFT:
        {
            if (g_UserIndex > 0)
                g_UserIndex--;
            else
                g_UserIndex = 2;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        case KEY_RIGHT:
        {
            if (g_UserIndex < 2)
                g_UserIndex++;
            else
                g_UserIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (g_UserIndex)
            {
                case 2: //通道类型
                {
                    PopList(g_channel_type, 5, g_channel_type[0], 70, 40, FALSE);
                    return TRUE;
                }
                case 0://取消
                {
                    CancelMenu();
                    return FALSE;
                }
                case 1://确认
                {
#ifdef AREA_ZHEJIANG
                    if (ValidatePwd() == TRUE)
                    {
                        int ret;
                        OOP_ZJ_CONNECT_T   tType;
                        tType.type = ZJ_CONNECT_NULL;
                        switch(g_ChannelIndex)
                        {
                            case 1: tType.type = ZJ_CONNECT_PUBLIC;  break;
                            case 2: tType.type = ZJ_CONNECT_SPECIAL; break;
                            case 3: tType.type = ZJ_CONNECT_ETH;     break;
                            case 4: tType.type = ZJ_CONNECT_RS232;   break;
                            default: tType.type = ZJ_CONNECT_NULL;   break;
                        }
                        OOP_OAD_U uOAD;
                        uOAD.value = 0x45300200;
                        ret = write_normal_data((uint8*)&tType, sizeof(OOP_ZJ_CONNECT_T), uOAD, 0, PD_LOGIC_ID, 1);
                        if (ret != 0)
                        {
                            MessageBox("通信通道配置写入失败！");
                        }
                        else {
                            MessageBox("通信通道配置写入成功！");
                        }
                    }
#endif
                    CancelMenu();
                    return FALSE;
                }
            }
        }
    }
    return FALSE;
}

//通信通道类型设置
PLUGIN_INFO g_layer_set_client_channel = {
    0x21130104/*id*/,
    client_setchannel_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    client_setchannel_draw/*Draw*/,
    NULL/*Timer*/,
    client_setchannel_keymsg/*keymsg*/,
    NULL/*ipc*/
};

/**
*********************************************************************
* @brief： 功能描述 初始化主站连接状态
* @param[in]： 入参说明
* @param[out]： 出参无
* @return： 返回值说明
*********************************************************************
*/
void client_seteth_init(DWORD dwPluginId)
{
    server_ethshow_init(dwPluginId);
    g_UserIndex = 0;
}

/**
*********************************************************************
* @brief： 功能描述 绘制主站连接状态
* @param[in]： 入参说明
* @param[out]： 出参无
* @return： 返回值说明
*********************************************************************
*/
void client_seteth_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;
    char czBuf[100] = {0};

    int nLeft1 = 10;
    int nTop = 30;
    int nTemp = 14;
    int nLeft2 = 70;
    
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "以太网参数设置");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontSmall);

    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "连接方式");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bEthCfg || g_showETHCfg.contype > 1)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", g_conn_type[g_showETHCfg.contype]);
    }
    if(2 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;

    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "心跳周期");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bEthCfg)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d", g_showETHCfg.heart);
    }
    if(3 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;

    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "主站地址");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bEthMaster || 0 == htonl(*((uint32 *)g_showETHMaster.master[0].ip)))
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {

        sprintf(czBuf, "%u.%u.%u.%u", g_showETHMaster.master[0].ip[0], g_showETHMaster.master[0].ip[1], 
                                      g_showETHMaster.master[0].ip[2], g_showETHMaster.master[0].ip[3]);
    }
    if(4 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;
    
    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "主站端口");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bEthMaster || 0 == g_showETHMaster.master[0].port)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%u", g_showETHMaster.master[0].port);
    }
    if(5 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;

    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "备用地址");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bEthMaster || 0 == htonl(*((uint32 *)g_showETHMaster.master[1].ip)))
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {

        sprintf(czBuf, "%u.%u.%u.%u", g_showETHMaster.master[1].ip[0], g_showETHMaster.master[1].ip[1], 
                                      g_showETHMaster.master[1].ip[2], g_showETHMaster.master[1].ip[3]);
    }
    if(6 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;
    
    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "备用端口");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bEthMaster || 0 == g_showETHMaster.master[1].port)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%u", g_showETHMaster.master[1].port);
    }
    if(7 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;

    /* 取消 确认 */
    if(0 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, 30, 130, "取消");
    ITEM_NOT_SELECT;

    if(1 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, 100, 130, "确认");
    ITEM_NOT_SELECT;

    PopListDraw(hdc);
}

void ethConfigReturn(char* pNewStr, void* pPara)
{
    char  czBuf[20] = {0};
    uint8 tmp[4] = {0};

    switch (g_UserIndex)
    {
        case 3:     //心跳周期
        {
            int heart = atoi(pNewStr);
            if (heart <= 65535)
            {
                g_showETHCfg.heart = heart;
                g_bEthCfg = TRUE;
            }
            else
            {
                if (TRUE == MessageBox("心跳周期有误\n\n确认是否重新输入?", MB_OKCANCEL))
                {
                    if(FALSE == g_bEthCfg)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d", g_showETHCfg.heart);
                    }
                    InputBox(czBuf, ethConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 4: //主站ip
        {
            if (FALSE == commfun_IP(pNewStr, tmp))
            {
                if (TRUE == MessageBox("IP地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
                {
                    if (FALSE == g_bEthMaster || g_showETHMaster.master[0].ip[0] == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", g_showETHMaster.master[0].ip[0], g_showETHMaster.master[0].ip[1], 
                                                      g_showETHMaster.master[0].ip[2], g_showETHMaster.master[0].ip[3]);
                    }
                    InputBox(czBuf, ethConfigReturn, NULL, 0x02);
                }
            }
            else
            {
                memcpy(g_showETHMaster.master[0].ip, tmp, 4);
                g_bEthMaster = TRUE;
            }
        }
        break;
        case 5:   //主站端口
        {
            int port = atoi(pNewStr);
            if (port <= 65535)
            {
                g_showETHMaster.master[0].port = port;
                g_bEthMaster = TRUE;
            }
            else
            {
                if (TRUE == MessageBox("端口格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
                {
                    if (FALSE == g_bEthMaster || g_showETHMaster.master[0].port == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d", g_showETHMaster.master[0].port);
                    }
                    InputBox(czBuf, ethConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 6: //备用主站ip
        {
            if (FALSE == commfun_IP(pNewStr, tmp))
            {
                if (TRUE == MessageBox("IP地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
                {
                    if (FALSE == g_bEthMaster || g_showETHMaster.master[1].ip[0] == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", g_showETHMaster.master[1].ip[0], g_showETHMaster.master[1].ip[1], 
                                                      g_showETHMaster.master[1].ip[2], g_showETHMaster.master[1].ip[3]);
                    }
                    InputBox(czBuf, ethConfigReturn, NULL, 0x02);
                }
            }
            else
            {
                memcpy(g_showETHMaster.master[1].ip, tmp, 4);
                g_bEthMaster = TRUE;
            }
        }
        break;
        case 7:   //备用主站端口
        {
            int port = atoi(pNewStr);
            if (port <= 65535)
            {
                g_showETHMaster.master[1].port = port;
                g_bEthMaster = TRUE;
            }
            else
            {
                if (TRUE == MessageBox("端口格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
                {
                    if (FALSE == g_bEthMaster || g_showETHMaster.master[1].port == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d", g_showETHMaster.master[1].port);
                    }
                    InputBox(czBuf, ethConfigReturn, NULL, 0x01);
                }
            }
        }
        break;

    }
}

BOOL client_seteth_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            if(2 == g_UserIndex)
            {
                g_showETHCfg.contype = nSel;
                g_bEthCfg = TRUE;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch (nKey)
    {
        case KEY_UP:
        case KEY_LEFT:
        {
            if (g_UserIndex > 0)
                g_UserIndex--;
            else
                g_UserIndex = 7;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        case KEY_RIGHT:
        {
            if (g_UserIndex < 7)
                g_UserIndex++;
            else
                g_UserIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (g_UserIndex)
            {
                case 0://取消
                {
                    CancelMenu();
                    return FALSE;
                }
                case 1://确认
                {
                    if (ValidatePwd() != TRUE)
                    {
                        int ret;
                        OOP_OAD_U uOAD;
                        uOAD.value = 0x45100200;
                        g_showETHCfg.workmode = MODE_CLIENT;
                        g_showETHCfg.conmode = MODE_ZB;
                        ret = write_normal_data((uint8*)&g_showETHCfg, sizeof(OOP_ETHCONFIG_T), uOAD, 0, PD_LOGIC_ID, 1);
                        
                        uOAD.value = 0x45100300;
                        if(0 != g_showETHMaster.master[1].ip[0])
                        {
                            g_showETHMaster.nNum = 2;
                        }
                        else if(0 == g_showETHMaster.master[0].ip[0] && 0 == g_showETHMaster.master[1].ip[0])
                        {
                            g_showETHMaster.nNum = 0;
                        }
                        else
                        {
                            g_showETHMaster.nNum = 1;
                        }
                        ret |= write_normal_data((uint8*)&g_showETHMaster, sizeof(OOP_MASTERPARAMS_T), uOAD, 0, PD_LOGIC_ID, 1);
                        if (ret != 0)
                        {
                            MessageBox("通信通道配置写入失败！");
                        }
                        else
                        {
                            MessageBox("通信通道配置写入成功！");
                        }
                        CancelMenu();
                    }
                    return FALSE;
                }
                case 2://连接方式
                {
                    PopList(g_conn_type, 2, g_conn_type[0], 70, 40, FALSE);
                    return TRUE;
                }
                case 3://心跳周期
                {
                    char czBuf[20];
                    if (FALSE == g_bEthCfg)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d", g_showETHCfg.heart);
                    }
                    InputBox(czBuf, ethConfigReturn, NULL, 0x01);
                    return TRUE;
                }
                case 4://主站ip
                {
                    char czBuf[20];
                    if (FALSE == g_bEthMaster || g_showETHMaster.master[0].ip[0] == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", g_showETHMaster.master[0].ip[0], g_showETHMaster.master[0].ip[1], 
                                                      g_showETHMaster.master[0].ip[2], g_showETHMaster.master[0].ip[3]);
                    }
                    InputBox(czBuf, ethConfigReturn, NULL, 0x02);
                    return TRUE;
                }
                case 5://主站端口
                {
                    char czBuf[20];
                    if (FALSE == g_bEthMaster || g_showETHMaster.master[0].port == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d", g_showETHMaster.master[0].port);
                    }
                    InputBox(czBuf, ethConfigReturn, NULL, 0x01);
                    return TRUE;
                }
                case 6://主站ip
                {
                    char czBuf[20];
                    if (FALSE == g_bEthMaster || g_showETHMaster.master[1].ip[0] == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", g_showETHMaster.master[1].ip[0], g_showETHMaster.master[1].ip[1], 
                                                      g_showETHMaster.master[1].ip[2], g_showETHMaster.master[1].ip[3]);
                    }
                    InputBox(czBuf, ethConfigReturn, NULL, 0x02);
                    return TRUE;
                }
                case 7://主站端口
                {
                    char czBuf[20];
                    if (FALSE == g_bEthMaster || g_showETHMaster.master[1].port == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d", g_showETHMaster.master[1].port);
                    }
                    InputBox(czBuf, ethConfigReturn, NULL, 0x01);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}


//通信通道类型设置
PLUGIN_INFO g_layer_set_client_eth = {
    0x21130105/*id*/,
    client_seteth_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    client_seteth_draw/*Draw*/,
    NULL/*Timer*/,
    client_seteth_keymsg/*keymsg*/,
    NULL/*ipc*/
};

/**
*********************************************************************
* @brief： 功能描述 初始化主站连接状态
* @param[in]： 入参说明
* @param[out]： 出参无
* @return： 返回值说明
*********************************************************************
*/
void client_setppp_init(DWORD dwPluginId)
{
    server_pppshow_init(dwPluginId);
    g_UserIndex = 0;
}

/**
*********************************************************************
* @brief： 功能描述 绘制主站连接状态
* @param[in]： 入参说明
* @param[out]： 出参无
* @return： 返回值说明
*********************************************************************
*/
void client_setppp_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;
    char czBuf[100] = {0};

    int nLeft1 = 10;
    int nTop = 30;
    int nTemp = 14;
    int nLeft2 = 70;
    
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "无线公/专网参数设置");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontSmall);

    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "APN");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bPPPCfg || 0 == g_showPPPCfg.apn.apn.nNum)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", g_showPPPCfg.apn.apn.value);
    }
    if(2 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;
        
    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "连接方式");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bPPPCfg || g_showPPPCfg.contype > 1)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", g_conn_type[g_showPPPCfg.contype]);
    }
    if(3 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;

    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "心跳周期");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bPPPCfg)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d", g_showPPPCfg.heart);
    }
    if(4 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;

    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "主站地址");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bPPPMaster || 0 == htonl(*((uint32 *)g_showPPPMaster.master[0].ip)))
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {

        sprintf(czBuf, "%u.%u.%u.%u", g_showPPPMaster.master[0].ip[0], g_showPPPMaster.master[0].ip[1], 
                                      g_showPPPMaster.master[0].ip[2], g_showPPPMaster.master[0].ip[3]);
    }
    if(5 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;
    
    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "主站端口");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bPPPMaster || 0 == g_showPPPMaster.master[0].port)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%u", g_showPPPMaster.master[0].port);
    }
    if(6 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;

    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "备用地址");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bPPPMaster || 0 == htonl(*((uint32 *)g_showPPPMaster.master[1].ip)))
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {

        sprintf(czBuf, "%u.%u.%u.%u", g_showPPPMaster.master[1].ip[0], g_showPPPMaster.master[1].ip[1], 
                                      g_showPPPMaster.master[1].ip[2], g_showPPPMaster.master[1].ip[3]);
    }
    if(7 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;
    
    nTop += nTemp;
    TextOut(hdc, nLeft1, nTop, "备用端口");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (FALSE == g_bPPPMaster || 0 == g_showPPPMaster.master[1].port)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%u", g_showPPPMaster.master[1].port);
    }
    if(8 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;


    /* 取消 确认 */
    if(0 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, 132, 100, "取消");
    ITEM_NOT_SELECT;

    if(1 == g_UserIndex)
    {
        ITEM_SELECT;
    }
    TextOut(hdc, 132, 124, "确认");
    ITEM_NOT_SELECT;

    PopListDraw(hdc);
}

void pppConfigReturn(char* pNewStr, void* pPara)
{
    char  czBuf[64] = {0};
    uint8 tmp[4] = {0};

    switch (g_UserIndex)
    {
        case 2: //APN
        {
            g_showPPPCfg.apn.apn.nNum = strlen(pNewStr);
            strcpy(g_showPPPCfg.apn.apn.value, pNewStr);
            g_bPPPCfg = TRUE;
        }
        case 4:     //心跳周期
        {
            int heart = atoi(pNewStr);
            if (heart <= 65535)
            {
                g_showPPPCfg.heart = heart;
                g_bPPPCfg = TRUE;
            }
            else
            {
                if (TRUE == MessageBox("心跳周期有误\n\n确认是否重新输入?", MB_OKCANCEL))
                {
                    if(FALSE == g_bPPPCfg)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d", g_showPPPCfg.heart);
                    }
                    InputBox(czBuf, pppConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 5: //主站ip
        {
            if (FALSE == commfun_IP(pNewStr, tmp))
            {
                if (TRUE == MessageBox("IP地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
                {
                    if (FALSE == g_bPPPMaster || g_showPPPMaster.master[0].ip[0] == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", g_showPPPMaster.master[0].ip[0], g_showPPPMaster.master[0].ip[1], 
                                                      g_showPPPMaster.master[0].ip[2], g_showPPPMaster.master[0].ip[3]);
                    }
                    InputBox(czBuf, pppConfigReturn, NULL, 0x02);
                }
            }
            else
            {
                memcpy(g_showPPPMaster.master[0].ip, tmp, 4);
                g_bPPPMaster = TRUE;
            }
        }
        break;
        case 6:   //主站端口
        {
            int port = atoi(pNewStr);
            if (port <= 65535)
            {
                g_showPPPMaster.master[0].port = port;
                g_bPPPMaster = TRUE;
            }
            else
            {
                if (TRUE == MessageBox("端口格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
                {
                    if (FALSE == g_bPPPMaster || g_showPPPMaster.master[0].port == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d", g_showPPPMaster.master[0].port);
                    }
                    InputBox(czBuf, pppConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 7: //备用主站ip
        {
            if (FALSE == commfun_IP(pNewStr, tmp))
            {
                if (TRUE == MessageBox("IP地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
                {
                    if (FALSE == g_bPPPMaster || g_showPPPMaster.master[1].ip[0] == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", g_showPPPMaster.master[1].ip[0], g_showPPPMaster.master[1].ip[1], 
                                                      g_showPPPMaster.master[1].ip[2], g_showPPPMaster.master[1].ip[3]);
                    }
                    InputBox(czBuf, pppConfigReturn, NULL, 0x02);
                }
            }
            else
            {
                memcpy(g_showPPPMaster.master[1].ip, tmp, 4);
                g_bPPPMaster = TRUE;
            }
        }
        break;
        case 8:   //备用主站端口
        {
            int port = atoi(pNewStr);
            if (port <= 65535)
            {
                g_showPPPMaster.master[1].port = port;
                g_bPPPMaster = TRUE;
            }
            else
            {
                if (TRUE == MessageBox("端口格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
                {
                    if (FALSE == g_bPPPMaster || g_showPPPMaster.master[1].port == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d", g_showPPPMaster.master[1].port);
                    }
                    InputBox(czBuf, pppConfigReturn, NULL, 0x01);
                }
            }
        }
        break;

    }
}

BOOL client_setppp_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            if(3 == g_UserIndex)
            {
                g_showETHCfg.contype = nSel;
                g_bEthCfg = TRUE;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch (nKey)
    {
        case KEY_UP:
        {
            if (g_UserIndex > 0)
                g_UserIndex--;
            else
                g_UserIndex = 8;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        {
            if (g_UserIndex < 8)
                g_UserIndex++;
            else
                g_UserIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_LEFT:
        {
            if (g_UserIndex < 2)
                g_UserIndex = 2;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        {
            if (g_UserIndex >= 2)
                g_UserIndex = 1;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (g_UserIndex)
            {
                case 0://取消
                {
                    CancelMenu();
                    return FALSE;
                }
                case 1://确认
                {
                    if (ValidatePwd() == TRUE)
                    {
                        int ret;
                        OOP_OAD_U uOAD;
                        uOAD.value = 0x45000200;
                        g_showPPPCfg.workmode = MODE_CLIENT;
                        g_showPPPCfg.conmode = MODE_ZB;
                        ret = write_normal_data((uint8*)&g_showPPPCfg, sizeof(OOP_GPRSCONFIG_T), uOAD, 0, PD_LOGIC_ID, 1);
                        
                        uOAD.value = 0x45000300;
                        if(0 != g_showPPPMaster.master[1].ip[0])
                        {
                            g_showPPPMaster.nNum = 2;
                        }
                        else if(0 == g_showPPPMaster.master[0].ip[0] && 0 == g_showPPPMaster.master[1].ip[0])
                        {
                            g_showPPPMaster.nNum = 0;
                        }
                        else
                        {
                            g_showPPPMaster.nNum = 1;
                        }
                        ret |= write_normal_data((uint8*)&g_showPPPMaster, sizeof(OOP_MASTERPARAMS_T), uOAD, 0, PD_LOGIC_ID, 1);
                        if (ret != 0)
                        {
                            MessageBox("通信通道配置写入失败！");
                        }
                        else
                        {
                            MessageBox("通信通道配置写入成功！");
                        }
                        CancelMenu();
                    }
                    return FALSE;
                }
                case 2://APN
                {
                    char czBuf[64];
                    if (FALSE == g_bPPPCfg || 0 == g_showPPPCfg.apn.apn.nNum)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%s", g_showPPPCfg.apn.apn.value);
                    }
                    InputBox(czBuf, pppConfigReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                    return TRUE;
                }
                case 3://连接方式
                {
                    PopList(g_conn_type, 2, g_conn_type[0], 70, 40, FALSE);
                    return TRUE;
                }
                case 4://心跳周期
                {
                    char czBuf[20];
                    if (FALSE == g_bPPPCfg)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d", g_showPPPCfg.heart);
                    }
                    InputBox(czBuf, pppConfigReturn, NULL, 0x01);
                    return TRUE;
                }
                case 5://主站ip
                {
                    char czBuf[20];
                    if (FALSE == g_bPPPMaster || g_showPPPMaster.master[0].ip[0] == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", g_showPPPMaster.master[0].ip[0], g_showPPPMaster.master[0].ip[1], 
                                                      g_showPPPMaster.master[0].ip[2], g_showPPPMaster.master[0].ip[3]);
                    }
                    InputBox(czBuf, pppConfigReturn, NULL, 0x02);
                    return TRUE;
                }
                case 6://主站端口
                {
                    char czBuf[20];
                    if (FALSE == g_bPPPMaster || g_showPPPMaster.master[0].port == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d", g_showPPPMaster.master[0].port);
                    }
                    InputBox(czBuf, pppConfigReturn, NULL, 0x01);
                    return TRUE;
                }
                case 7://主站ip
                {
                    char czBuf[20];
                    if (FALSE == g_bPPPMaster || g_showPPPMaster.master[1].ip[0] == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", g_showPPPMaster.master[1].ip[0], g_showPPPMaster.master[1].ip[1], 
                                                      g_showPPPMaster.master[1].ip[2], g_showPPPMaster.master[1].ip[3]);
                    }
                    InputBox(czBuf, pppConfigReturn, NULL, 0x02);
                    return TRUE;
                }
                case 8://主站端口
                {
                    char czBuf[20];
                    if (FALSE == g_bPPPMaster || g_showPPPMaster.master[1].port == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d", g_showPPPMaster.master[1].port);
                    }
                    InputBox(czBuf, pppConfigReturn, NULL, 0x01);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

//通信通道类型设置
PLUGIN_INFO g_layer_set_client_ppp = {
    0x21130106/*id*/,
    client_setppp_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    client_setppp_draw/*Draw*/,
    NULL/*Timer*/,
    client_setppp_keymsg/*keymsg*/,
    NULL/*ipc*/
};



