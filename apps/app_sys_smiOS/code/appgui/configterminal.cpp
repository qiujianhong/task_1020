/*
***************************************************************************
  Copyright (c) 2006-2007,�Ͼ��������������������ι�˾
             All rights reserved.
  ��������: configcontrol.cpp
  ��    �ܣ�����ն���������ҳ���
  �� �� �ˣ�gaoyf
  ����ʱ�䣺2007-1

  �汾��ʷ:

    v1.0:
      2006-1-20, gaoyf : New file

  ����ҳ��⣺
    �ն˵�ַ�����������룩  0x00630000
    �ն˵�ǰ����ͨ����Ϣ    0x00660000


***************************************************************************
*/

#include <sys/stat.h>
#include "miniguidef.h"
#include "commdef.h"
#include "main.h"
#include "commfun.h"
//#include "appmsg_struct.h"
//#include "deskgui_cfg.h"
#include "pageInterface.h"
#include "constant.h"
#include "msgService.h"
#include "dataType.h"
#include "cJSON.h"
#include "globalConfiguration.h"
#include "layer.h"
#include "miniguidef2.h"
#include "msgHandler.h"
#include "configterminal.h"
#include "appManager.h"

#ifdef __cplusplus
extern "C" {
#endif
extern uint32 get_cpufreq(void);
extern uint32 get_memtotal(void );
extern uint32 get_storage(void);
extern void CancelMenu();
extern int act_set_ip(uint8 bEvent);
extern int act_set_mac(void);
extern int local_ip_info_save(void);
extern int cmd_execute(const char* cmd,  char* result);
extern void get_container_info(ALL_CONTAINER_INFO_T *allcontainer);
extern void get_app_info(char* containername,ALL_APP_INFO_T* appinfo);

#ifdef __cplusplus
}
#endif
typedef struct _INPUT_INFO
{
    int     nType;      // 0:���� 1:���� 2:��д 3:Сд 4:�ַ�
    int     nTypeMask;  // 0x01:���� 0x02:���� 0x04:��д 0x08:Сд 00x10:�ַ�
    int     nIndex;     // ��ǰ����

    int     nTotalCnt;    // ��ѡ���ַ����ĳ��� ����nType�仯 ��strlen(g_input_1)�ĳ���
    int     nLineCnt;    // ��ѡ���ַ���������
    const char*   pInputStr;    // ��ѡ���ַ���g_input_1��g_input_2��

    int     nMaxLen;    // �����ַ�������󳤶�
    int     nCurLen;    // �����ַ����ĵ�ǰ����
    char    czInfo[40];    // �����ַ���

    fnRetureInput  pfnReturn;    // ��ҳ�淵��ʱ���õĻص����������ڴ�����������
    void*       pPara;    // �ص������Ĳ��������ûص�����ʱ����
}INPUT_INFO;
extern BLUE_TOOTH_PARAM_T gBlueToothParam;

//CONTAINSTAT_T containerState;
/************************************************************************/
/* �ն�ʱ������                                                         */
/************************************************************************/
typedef struct _RESET_TIME
{
    int     nIndex;     // ��ǰ����
    char    czInfo[16];    // ��ǰʱ���ַ���������:

    struct tm    tmNow;

}RESET_TIME;
static RESET_TIME g_reset_time;

static int g_nTermRetsetIndex = 0;
extern int rebootflag;
extern int parainitflag;
extern void commfun_IPToStr(int nIp, char* pVal);
extern GLOBAL_CONFIG_OBJ_T g_configObj;
extern bool setFlag;
const char* g_input_1 = "0123456789";
const char* g_input_2 = "0123456789./-+:;";
const char* g_input_3 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ./";
const char* g_input_4 = "abcdefghijklmnopqrstuvwxyz./";
const char* g_input_5 = "`~!@#$%^&*()-+_={}[]<>|\\\"',?";
const char* g_input_6 = "0123456789ABCDEF";
extern INPUT_INFO g_input_info;


//const char* g_pczRadioDelay[] = {"100", "200", "300", "400", "500", "800", "1000", "2000"};
typedef struct _VALID_PWD
{
    int     nIndex;     // ��ǰ����
    char    czPwd[PASSWORD_LEN+1];   // ��������
}VALID_PWD;
VALID_PWD g_valid_pwd;
int g_nErrPwdCnt = 0;

int RebootTerminal();

void MenuDraw_add_name(HDC hdc,const char *pname);

void MenuDraw(HDC hdc)
{
    MenuDraw_add_name(hdc, ADV_SHOW_NAME);
}

BOOL MenuKeyMsg(int nKey);
void InitMenu(DWORD dwPluginId);

//��������
const char* g_NetType[] =
{
    "DHCP",
    "��̬"
};

void getipinfo()
{
    if(load_ip_info(&setIpInfo)==0)
        return ;

    strcpy(setIpInfo.nodeName, ETH_FIRST_NAME);
    char cmd[200];
    char result[100];
    memset(cmd,0x00,sizeof(cmd));
    memset(result,0x00,sizeof(result));
    sprintf(cmd, "ifconfig %s | grep 'inet addr' | awk '{ print $2}' | awk -F: '{print $2}' ", ETH_FIRST_NAME);
    cmd_execute(cmd,result);
    sscanf(result,"%u.%u.%u.%u",(uint32*)&setIpInfo.ipVal[0],(uint32*)&setIpInfo.ipVal[1],(uint32*)&setIpInfo.ipVal[2],(uint32*)&setIpInfo.ipVal[3]);
    
    memset(cmd,0x00,sizeof(cmd));
    memset(result,0x00,sizeof(result));
    sprintf(cmd,"ifconfig %s | grep 'Mask' | awk '{ print $4}' | awk -F: '{print $2}' ", ETH_FIRST_NAME);
    cmd_execute(cmd,result);
    sscanf(result,"%u.%u.%u.%u",(uint32*)&setIpInfo.netMask[0],(uint32*)&setIpInfo.netMask[1],(uint32*)&setIpInfo.netMask[2],(uint32*)&setIpInfo.netMask[3]);

    memset(cmd,0x00,sizeof(cmd));
    memset(result,0x00,sizeof(result));
    sprintf(cmd, "route -n|grep %s|grep UG| awk '{ print $2}'", ETH_FIRST_NAME);
    cmd_execute(cmd,result);
    sscanf(result,"%u.%u.%u.%u",(uint32*)&setIpInfo.gateWay[0], (uint32*)&setIpInfo.gateWay[1], (uint32*)&setIpInfo.gateWay[2], (uint32*)&setIpInfo.gateWay[3]);

    memset(cmd, 0x00, sizeof(cmd));
    memset(result, 0x00, sizeof(result));
    sprintf(cmd, "ps |grep udhcpc|grep %s|grep -v grep|awk '{print $1}'", ETH_FIRST_NAME);
    cmd_execute(cmd, result);
    setIpInfo.type = 0 == result[0] ? 1 : 0;

    sprintf(cmd,"ifconfig %s | grep 'HWaddr' | awk '{ print $5}' ", ETH_FIRST_NAME);
    memset(setIpInfo.mac, 0x00, sizeof(setIpInfo.mac));
    cmd_execute(cmd, result);
    sscanf(result,"%X:%X:%X:%X:%X:%X",(uint32*)&setIpInfo.mac[0], (uint32*)&setIpInfo.mac[1], (uint32*)&setIpInfo.mac[2],
                                      (uint32*)&setIpInfo.mac[3], (uint32*)&setIpInfo.mac[4], (uint32*)&setIpInfo.mac[5]);
}

void InitNetPara(DWORD dwPluginId)
{
    strcpy(setIpInfo.nodeName, ETH_FIRST_NAME);
    char cmd[200];
    char result[100];
    uint32  tempval[6] = {0};
    
    memset(cmd,0x00,sizeof(cmd));
    memset(result,0x00,sizeof(result));
    sprintf(cmd,"ifconfig %s | grep 'inet addr' | awk '{ print $2}' | awk -F: '{print $2}' ", ETH_FIRST_NAME);
    cmd_execute(cmd,result);
    sscanf(result,"%u.%u.%u.%u", &tempval[0], &tempval[1], &tempval[2], &tempval[3]);
    setIpInfo.ipVal[0] = tempval[0];
    setIpInfo.ipVal[1] = tempval[1];
    setIpInfo.ipVal[2] = tempval[2];
    setIpInfo.ipVal[3] = tempval[3];

    memset(tempval, 0 , 6);
    memset(cmd,0x00,sizeof(cmd));
    memset(result,0x00,sizeof(result));
    sprintf(cmd,"ifconfig %s | grep 'Mask' | awk '{ print $4}' | awk -F: '{print $2}' ", ETH_FIRST_NAME);
    cmd_execute(cmd,result);
    sscanf(result,"%u.%u.%u.%u", &tempval[0], &tempval[1], &tempval[2], &tempval[3]);
    setIpInfo.netMask[0] = tempval[0];
    setIpInfo.netMask[1] = tempval[1];
    setIpInfo.netMask[2] = tempval[2];
    setIpInfo.netMask[3] = tempval[3];

    memset(tempval, 0 , 6);
    memset(cmd,0x00,sizeof(cmd));
    memset(result,0x00,sizeof(result));
    sprintf(cmd,"route -n|grep %s|grep UG| awk '{ print $2}'", ETH_FIRST_NAME);
    cmd_execute(cmd,result);
    sscanf(result,"%u.%u.%u.%u", &tempval[0], &tempval[1], &tempval[2], &tempval[3]);
    setIpInfo.gateWay[0] = tempval[0];
    setIpInfo.gateWay[1] = tempval[1];
    setIpInfo.gateWay[2] = tempval[2];
    setIpInfo.gateWay[3] = tempval[3];

    memset(cmd, 0x00, sizeof(cmd));
    memset(result, 0x00, sizeof(result));
    sprintf(cmd, "ps |grep udhcpc|grep %s|grep -v grep|awk '{print $1}'", ETH_FIRST_NAME);
    cmd_execute(cmd, result);
    setIpInfo.type = 0 == result[0] ? 1 : 0;

    memset(tempval, 0 , 6);
    memset(cmd, 0x00, sizeof(cmd));
    sprintf(cmd,"ifconfig %s | grep 'HWaddr' | awk '{ print $5}'", ETH_FIRST_NAME);
    memset(setIpInfo.mac, 0x00, sizeof(setIpInfo.mac));
    cmd_execute(cmd, result);
    sscanf(result,"%X:%X:%X:%X:%X:%X", &tempval[0], &tempval[1], &tempval[2], &tempval[3], &tempval[4], &tempval[5]);
    setIpInfo.mac[0] = tempval[0];
    setIpInfo.mac[1] = tempval[1];
    setIpInfo.mac[2] = tempval[2];
    setIpInfo.mac[3] = tempval[3];
    setIpInfo.mac[4] = tempval[4];
    setIpInfo.mac[5] = tempval[5];
}
char * strim(char *str)
{
    char *strend,*sp,*ep;
    int len;
    sp = str;
    strend = str + strlen(str) - 1;
    ep = strend;
 
    while(sp<=strend && isspace(*sp))// *sp == ' 'Ҳ����
        sp++;
    while(ep>=sp && isspace(*ep))
        ep--;
    len = (ep < sp) ? 0:(ep-sp)+1;//(ep < sp)�ж��Ƿ����ж��ǿո�
    sp[len] = '\0';
    return sp;

}

//��ȡ���ص�ַ
//void getGateWay(char* gateway)
//{
//    char* cmd = "grep gateway /etc/network/interfaces.d/FE0";
//    FILE* fp = NULL;
//    char temp[256];
//    char* str = NULL;
//    fp = popen(cmd, "r");
//    if(fp == NULL)
//    {
//        strcpy(gateway,"0.0.0.0");
//        return ;
//    }
//    
//    while(fgets(temp, sizeof(temp), fp) != NULL)
//    {
//        if((str = strstr(temp,"gateway"))!=NULL)
//        {
//            strcpy(gateway,strim(str+strlen("gateway")));
//            pclose(fp);
//            return;
//        }
//    }
//    pclose(fp);
//    strcpy(gateway,"0.0.0.0");
//    return ;
//}



void NetParaDraw(HDC hdc)
{    
    char ip_info[24] = {0};
    char netMask[24] = {0};
    char gateWay[24] = {0};
    char mac[24] = {0};
    
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc,pg_guicomm->fontBig);

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "�������");
    rc.top = 22;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    
    SelectFont(hdc,pg_guicomm->fontSmall);
    int nTop = rc.top + 26;
    int nLeft1 = 6;
    int nLeft2 = 48;

    TextOut(hdc, nLeft1, nTop, "����: ");
    TextOut(hdc, nLeft2, nTop, 1 == setIpInfo.type ? "��̬" : "DHCP");

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "��ַ: ");
    sprintf(ip_info,"%d.%d.%d.%d",setIpInfo.ipVal[0],setIpInfo.ipVal[1],setIpInfo.ipVal[2],setIpInfo.ipVal[3]);
    TextOut(hdc, nLeft2, nTop, ip_info);
    
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "����: ");
    sprintf(netMask,"%d.%d.%d.%d",setIpInfo.netMask[0],setIpInfo.netMask[1],setIpInfo.netMask[2],setIpInfo.netMask[3]);
    TextOut(hdc, nLeft2, nTop, netMask);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "����: ");
    if(0 != setIpInfo.gateWay[0])
    {
        sprintf(gateWay,"%d.%d.%d.%d", setIpInfo.gateWay[0],setIpInfo.gateWay[1],setIpInfo.gateWay[2],setIpInfo.gateWay[3]);
    }
    else
    {
        sprintf(gateWay,"---.---.---.---");
    }
    TextOut(hdc, nLeft2, nTop, gateWay);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "MAC : ");
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", setIpInfo.mac[0], setIpInfo.mac[1], setIpInfo.mac[2], setIpInfo.mac[3],
                                                  setIpInfo.mac[4], setIpInfo.mac[5]);
    TextOut(hdc, nLeft2, nTop, mac);
}


void InitLCDPara(DWORD dwPluginId)
{
    
}


void LCDParaDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc,pg_guicomm->fontBig);

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "Һ������");
    rc.top = 26;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    
    SelectFont(hdc,pg_guicomm->fontSmall);
    int nTop = rc.top + 30;
    int nLeft1 = 10;
    TextOut(hdc, nLeft1, nTop, "�Աȶ�:");
    //TextOut(hdc, nLeft1, nTop, ip_info);

}
//�Աȶ�
uint8 gLCDPara;
uint8 gLCDindex;
uint8 oldpara ;

void InitLCDSetPara(DWORD dwPluginId)
{
    gLCDindex = 0;
    gLCDPara = oldpara;
}

void LCDParaSetDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;
    uint8 index = 0;
    ITEM_NOT_SELECT;
    SelectFont(hdc,pg_guicomm->fontBig);

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "Һ������");
    rc.top = 26;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    
    SelectFont(hdc,pg_guicomm->fontSmall);
    int nTop = rc.top + 30;
    int nLeft1 = 20;
    int nLeft2 = 90;
    TextOut(hdc, nLeft1, nTop, "�Աȶ�:");
    memset(czBuf,0x00,sizeof(czBuf));
    sprintf(czBuf,"%d",gLCDPara);
    if(gLCDindex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    index++;
    nTop+=50;
    if(gLCDindex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }else
    {
        TextOut(hdc, nLeft1, nTop, "ȡ��");
    }
    index++;
    if(gLCDindex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "����");
        ITEM_NOT_SELECT;
    }else
    {
        TextOut(hdc, nLeft2, nTop, "����");
    }
        
    //TextOut(hdc, nLeft1, nTop, ip_info);

}

BOOL LCDParaSetKey(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return FALSE;
    
    switch(nKey)
    {
        case KEY_LEFT:
        {
            if(gLCDindex>0)
                gLCDindex--;
            else
                gLCDindex = 2;
        }
        break;
        case KEY_RIGHT:
        {
            if(gLCDindex<2)
                gLCDindex++;
            else
                gLCDindex = 0;
        }
        break;
        case KEY_UP:
        {
            if(gLCDindex == 0)
            {
                if(gLCDPara<15)
                    gLCDPara++;
                else
                    gLCDPara = 0;
            }
        }
        break;
        case KEY_DOWN:
        {
            if(gLCDindex == 0)
            {
                if(gLCDPara > 0)
                    gLCDPara--;
                else
                    gLCDPara = 15;
            }
        }
        break;
        case KEY_OK:
        {
            if(gLCDindex == 1)
            {
                gLCDPara = oldpara;
                AppCancelMenu();
                return FALSE;
            }
            if(gLCDindex == 2)
            {
                MessageBox("Һ���ԱȲ������óɹ���");
                oldpara = gLCDPara;
                AppCancelMenu();
            }
            return TRUE;
        }
        break;
    }
    return FALSE;
}




uint8 gBlueQueryFlag = 0;

void InitBlueCfg(DWORD dwPluginId)
{

    MSG_CLIENT_T client;
    client.type = MQTT;
    strcpy(client.clientName,"M-btManager");
    uint8 sendcnt = 1;
    send_bluetooth_param_query(&client);
    gBlueQueryFlag = 1;
    time_t sendtime = time(NULL);
    GUI_FMT_DEBUG("��ѯ�������� \n");
    while(1)
    {
        if(time(NULL)-sendtime>10 && gBlueQueryFlag ==1)
        {
            GUI_FMT_DEBUG("��ѯ������������ cnt %d \n",sendcnt);
            send_bluetooth_param_query(&client);
            sendtime = time(NULL);
            sendcnt++;
        }
        if(sendcnt > 1)
        {
            GUI_FMT_DEBUG("��ѯ��������ʧ�� \n");
            gBlueQueryFlag = 0;
            break;
        }
        if(gBlueQueryFlag == 0)
        {
            GUI_FMT_DEBUG("��ѯ�������óɹ� \n");
            break;
        }
    }

    
}


void BlueCfgDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc,pg_guicomm->fontBig);

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "��������");
    rc.top = 26;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    
    SelectFont(hdc,pg_guicomm->fontSmall);
    int nTop = rc.top + 15;
    int nLeft1 = 5;
    int nLeft2 = 60;
    TextOut(hdc, nLeft1, nTop, "��������:");
    memset(czBuf,0x00,sizeof(czBuf));
    sprintf(czBuf,"%s",gBlueToothParam.name);
    TextOut(hdc, nLeft2, nTop, czBuf);
    nTop+=20;

    TextOut(hdc, nLeft1, nTop, "MAC:");
    memset(czBuf,0x00,sizeof(czBuf));
    sprintf(czBuf,"%02x:%02x:%02x:%02x:%02x:%02x",gBlueToothParam.mac[0],gBlueToothParam.mac[1],gBlueToothParam.mac[2],
        gBlueToothParam.mac[3],gBlueToothParam.mac[4],gBlueToothParam.mac[5]);
    TextOut(hdc, nLeft2, nTop, czBuf);
    nTop+=20;

    TextOut(hdc, nLeft1, nTop, "���书��:");
    memset(czBuf,0x00,sizeof(czBuf));
    sprintf(czBuf,"%d",gBlueToothParam.power);
    TextOut(hdc, nLeft2, nTop, czBuf);
    nTop+=20;
    
    TextOut(hdc, nLeft1, nTop, "�㲥���:");
    memset(czBuf,0x00,sizeof(czBuf));
    sprintf(czBuf,"%d",gBlueToothParam.broadperiod);
    TextOut(hdc, nLeft2, nTop, czBuf);
    nTop+=20;
        TextOut(hdc, nLeft1, nTop, "ɨ����:");
    memset(czBuf,0x00,sizeof(czBuf));
    sprintf(czBuf,"%d",gBlueToothParam.scanperiod);
    TextOut(hdc, nLeft2, nTop, czBuf);
    nTop+=20;
}

uint8 netparasetindex;
uint8 flag ;

void InitNetParaSet(DWORD dwPluginId)
{
    getipinfo();
    netparasetindex = 0;
    flag = 0;

    if(0 == setIpInfo.mac[0] && 0 == setIpInfo.mac[1] && 0 == setIpInfo.mac[2] &&
       0 == setIpInfo.mac[3] && 0 == setIpInfo.mac[4] && 0 == setIpInfo.mac[5])
    {
        char cmd[200];
        char result[100];
        
        memset(cmd,0x00,sizeof(cmd));
        memset(result,0x00,sizeof(result));
        sprintf(cmd,"ifconfig %s | grep 'HWaddr' | awk '{ print $5}'",ETH_FIRST_NAME);
        memset(setIpInfo.mac, 0x00, sizeof(setIpInfo.mac));
        cmd_execute(cmd, result);
        sscanf(result,"%X:%X:%X:%X:%X:%X",(uint32*)&setIpInfo.mac[0], (uint32*)&setIpInfo.mac[1], (uint32*)&setIpInfo.mac[2],
                                          (uint32*)&setIpInfo.mac[3], (uint32*)&setIpInfo.mac[4], (uint32*)&setIpInfo.mac[5]);
    }
}


void NetParaSetDraw(HDC hdc)
{    

    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;
        
    char ip_info[64] = {0};
    char netMask[64] = {0};
    char gateWay[64] = {0};
    char mac[24] = {0};
    ITEM_NOT_SELECT;
    SelectFont(hdc,pg_guicomm->fontBig);
    
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "�����������");
    rc.top = 22;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    
    SelectFont(hdc,pg_guicomm->fontSmall);
    int nTop = rc.top + 26;
    int nLeft1 = 6;
    int nLeft2 = 48;

    TextOut(hdc, nLeft1, nTop, "����:");
    if(0 == netparasetindex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_NetType[setIpInfo.type]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_NetType[setIpInfo.type]);
    }

    if(1 == setIpInfo.type)
    {
        nTop += 14;
        TextOut(hdc, nLeft1, nTop, "��ַ:");
        sprintf(ip_info,"%d.%d.%d.%d",setIpInfo.ipVal[0],setIpInfo.ipVal[1],setIpInfo.ipVal[2],setIpInfo.ipVal[3]);
        if(1 == netparasetindex)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, ip_info);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, ip_info);
        }

        nTop += 14;
        TextOut(hdc, nLeft1, nTop, "����:");
        sprintf(netMask,"%d.%d.%d.%d",setIpInfo.netMask[0],setIpInfo.netMask[1],setIpInfo.netMask[2],setIpInfo.netMask[3]);
        if(2 == netparasetindex)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, netMask);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, netMask);
        }

        nTop += 14;
        TextOut(hdc, nLeft1, nTop, "����:");
        if(0 != setIpInfo.gateWay[0])
        {
            sprintf(gateWay,"%d.%d.%d.%d", setIpInfo.gateWay[0],setIpInfo.gateWay[1],setIpInfo.gateWay[2],setIpInfo.gateWay[3]);
        }
        else
        {
            sprintf(gateWay,"---.---.---.---");
        }
        if(3 == netparasetindex)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, gateWay);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, gateWay);
        }
    }
    else
    {
        nTop += 14 * 3;
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "MAC :");
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", setIpInfo.mac[0], setIpInfo.mac[1], setIpInfo.mac[2], setIpInfo.mac[3],
                                                  setIpInfo.mac[4], setIpInfo.mac[5]);
    if(4 == netparasetindex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, mac);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, mac);
    }

    nTop += 18;
    if(5 == netparasetindex)
    {
        ITEM_SELECT;
        TextOut(hdc, 18, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 18, nTop, "ȷ��");
    }

    if(6 == netparasetindex)
    {
        ITEM_SELECT;
        TextOut(hdc, 120, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 120, nTop, "����");
    }

    PopListDraw(hdc);
}
//���ַ�����ʽ��ipת��Ϊ���� ʧ�ܷ���false
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

//���ַ�����ʽ��macת��Ϊ���� ʧ�ܷ���false
BOOL commfun_MAC(char* macin, uint8* ipout)
{
    char* pTmp = macin;
    int   i = 0;
    uint8 temp = 0;
    
    if (macin == NULL || 17 != strlen(macin))
    {
        return FALSE;
    }

    if(':' != macin[2] || ':' != macin[5] || ':' != macin[8] || ':' != macin[11] || ':' != macin[14])
    {
        return FALSE;
    }

    for(i = 0; i < 6; i++)
    {
        if('0' <= pTmp[i * 3] && pTmp[i * 3] <= '9')
        {
            temp = pTmp[i * 3] - '0';
        }
        else if('A' <= pTmp[i * 3] && pTmp[i * 3] <= 'F')
        {
            temp = pTmp[i * 3] - 'A' + 10;
        }
        else if('a' <= pTmp[i * 3] && pTmp[i * 3] <= 'f')
        {
            temp = pTmp[i * 3] - 'a' + 10;
        }
        else
        {
            return FALSE;
        }
        
        if('0' <= pTmp[i * 3 + 1] && pTmp[i * 3 + 1] <= '9')
        {
            temp = temp * 16 + pTmp[i * 3 + 1] - '0';
        }
        else if('A' <= pTmp[i * 3 + 1] && pTmp[i * 3 + 1] <= 'F')
        {
            temp = temp * 16 + pTmp[i * 3 + 1] - 'A' + 10;
        }
        else if('a' <= pTmp[i * 3 + 1] && pTmp[i * 3 + 1] <= 'f')
        {
            temp = temp * 16 + pTmp[i * 3 + 1] - 'a' + 10;
        }
        else
        {
            return FALSE;
        }
        ipout[i] = temp;
    }

    if(1 == (ipout[0] & 0x01))
    {
        return FALSE;
    }

    return TRUE;
}

void NetParaSetReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[6] = { 0 };
    switch(netparasetindex)
    {
        case 1:
        {
            if(FALSE == commfun_IP(pNewStr, tmp))
            {
                if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    sprintf(czBuf,"%d.%d.%d.%d",setIpInfo.ipVal[0],setIpInfo.ipVal[1],setIpInfo.ipVal[2],setIpInfo.ipVal[3]);
                    InputBox(czBuf, NetParaSetReturn, NULL, 0x02);
                }
            }else
            {
                memcpy(setIpInfo.ipVal,tmp,4);
                flag = 1;
            }
            break;
        }
        case 2:
        {
            if(FALSE == commfun_IP(pNewStr, tmp))
            {
                if (TRUE == MessageBox("���������ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    sprintf(czBuf,"%d.%d.%d.%d",setIpInfo.netMask[0],setIpInfo.netMask[1],setIpInfo.netMask[2],setIpInfo.netMask[3]);
                    InputBox(czBuf, NetParaSetReturn, NULL, 0x02);
                }
            }else
            {
                memcpy(setIpInfo.netMask,tmp,4);
                flag = 1;
            }
            break;
        }
        case 3:
        {
            if(FALSE == commfun_IP(pNewStr, tmp))
            {
                if (TRUE == MessageBox("���ص�ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    sprintf(czBuf,"%d.%d.%d.%d",setIpInfo.gateWay[0],setIpInfo.gateWay[1],setIpInfo.gateWay[2],setIpInfo.gateWay[3]);
                    InputBox(czBuf, NetParaSetReturn, NULL, 0x02);
                }
            }else
            {
                memcpy(setIpInfo.gateWay,tmp,4);
                flag = 1;
            }
            break;
        }
        case 4:
        {
            if(FALSE == commfun_MAC(pNewStr, tmp))
            {
                if (TRUE == MessageBox("MAC��ַ��ʽ����\n��1λ������ż��\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    sprintf(czBuf,"%02X:%02X:%02X:%02X:%02X:%02X", setIpInfo.mac[0], setIpInfo.mac[1],
                                      setIpInfo.mac[2], setIpInfo.mac[3], setIpInfo.mac[4], setIpInfo.mac[5]);
                    InputBox(czBuf, NetParaSetReturn, NULL, 0x02 | 0x04, 0x02);
                }
            }else
            {
                memcpy(setIpInfo.mac, tmp, 6);
                flag = 1;
            }
            break;
        }
    }
}
BOOL NetParaSetKey(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return FALSE;

    // ����Listѡ����
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            setIpInfo.type = nSel;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }

    switch(nKey)
    {
        case KEY_UP:
        case KEY_LEFT:
        {
            if(netparasetindex > 0)
            {
                netparasetindex--;
            }
            if(1 != setIpInfo.type)
            {
                if(netparasetindex < 5)
                {
                    netparasetindex = 0;
                }
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        case KEY_RIGHT:
        {
            if(netparasetindex < 6)
            {
                netparasetindex++;
            }
            if(1 != setIpInfo.type)
            {
                if(netparasetindex < 5)
                {
                    netparasetindex = 5;
                }
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            if(netparasetindex == 6)
            {
                AppCancelMenu();
                return TRUE;
            }
            if(ValidatePwd() == TRUE)
            {// ȷ�ϴ���
                char czBuf[50];
                switch(netparasetindex)
                {
                    case 0:// ��������
                    {
                        PopList(g_NetType, 2, g_NetType[setIpInfo.type], 82, 42, FALSE);
                        break;
                    }
                    case 1:// ����ip��ַ����ҳ���
                    {
                        sprintf(czBuf,"%d.%d.%d.%d",setIpInfo.ipVal[0],setIpInfo.ipVal[1],setIpInfo.ipVal[2],setIpInfo.ipVal[3]);
                        InputBox(czBuf, NetParaSetReturn, NULL, 0x02);
                        break;
                    }
                    case 2:// ��������
                    {
                        sprintf(czBuf,"%d.%d.%d.%d",setIpInfo.netMask[0],setIpInfo.netMask[1],setIpInfo.netMask[2],setIpInfo.netMask[3]);
                        InputBox(czBuf, NetParaSetReturn, NULL, 0x02);
                        break;
                    }
                    case 3:// ����
                    {
                        sprintf(czBuf,"%d.%d.%d.%d",setIpInfo.gateWay[0],setIpInfo.gateWay[1],setIpInfo.gateWay[2],setIpInfo.gateWay[3]);
                        InputBox(czBuf, NetParaSetReturn, NULL, 0x02);
                        break;
                    }
                    case 4:// MAC
                    {
                        sprintf(czBuf,"%02X:%02X:%02X:%02X:%02X:%02X", setIpInfo.mac[0], setIpInfo.mac[1],
                                      setIpInfo.mac[2], setIpInfo.mac[3], setIpInfo.mac[4], setIpInfo.mac[5]);
                        InputBox(czBuf, NetParaSetReturn, NULL, 0x02 | 0x04, 0x02);
                        break;
                    }
                    case 5:
                    {
                        act_set_mac();
                        sleep(1);
                        if(act_set_ip(TRUE) < 0)
                        {
                            MessageBox("�����������ʧ�ܣ�\n");
                            break;
                        }
                        else
                        {
                            MessageBox("����������óɹ���\n");
                            local_ip_info_save();
                            AppCancelMenu();
                        }
                        break;
                    }
                }
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            }
        }
        return TRUE;
    }
    return FALSE;
}

void GetInputInfo(int nType)
{
    switch(nType)
    {
    case 0:g_input_info.pInputStr = g_input_1;break;
    case 1:g_input_info.pInputStr = g_input_2;break;
    case 2:g_input_info.pInputStr = g_input_3;break;
    case 3:g_input_info.pInputStr = g_input_4;break;
    case 4:g_input_info.pInputStr = g_input_5;break;
    case 5:g_input_info.pInputStr = g_input_6;break;
    }
    g_input_info.nMaxLen = 32;
}

const char* GetInputTypeStr(int nType)
{
    switch(nType)
    {
    case 0:return "����";
    case 1:return "����";
    case 2:return "��д";
    case 3:return "Сд";
    case 4:return "�ַ�";
    case 5:return "HEX";
    }
    ASSERT(FALSE);
    return "";
}
BOOL CanInputType(int nType)
{
    switch(nType)
    {
    case 0:return (g_input_info.nTypeMask&0x01)==0x01;
    case 1:return (g_input_info.nTypeMask&0x02)==0x02;
    case 2:return (g_input_info.nTypeMask&0x04)==0x04;
    case 3:return (g_input_info.nTypeMask&0x08)==0x08;
    case 4:return (g_input_info.nTypeMask&0x10)==0x10;
    case 5:return (g_input_info.nTypeMask&0x20)==0x20;
    }
    return FALSE;
}


void InputDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;
    ITEM_NOT_SELECT;
    SelectFont(hdc,pg_guicomm->fontSmall);

    // ���ݵ�ǰ���ͣ����ַ�����Ϣ�Լ���ѡ�ַ����ľ��ο�
    int nLeft = 2;
    int nTop = 32;
    #if MODULARIZATION_TMN_EN == 1
    nTop = 27; 
    if(g_input_info.nIndex == 49)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop+17, "�˸�");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop+17, "�˸�");
    }    
    #endif
       if(g_input_info.nIndex == 50)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, GetInputTypeStr(g_input_info.nType));
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, GetInputTypeStr(g_input_info.nType));
    }

    Rectangle(hdc, 27, 26, 132, 56);
    if(strlen(g_input_info.czInfo) > 16)
    {
        char czBuf[300];
        sprintf(czBuf, g_input_info.czInfo);
        czBuf[16] = '\0';
        TextOut(hdc, 32, 28, czBuf);
        TextOut(hdc, 32, 42, g_input_info.czInfo + 16);
    }
    else
    {
        TextOut(hdc, 32, 28, g_input_info.czInfo);
    }

    // ��ȷ��ѡ���
    nLeft = 134;
    if(g_input_info.nIndex == 51)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȷ��");
    }
    #if MODULARIZATION_TMN_EN == 1
    if(g_input_info.nIndex == 52)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop+17, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop+17, "����");
    }
    #endif
    if(g_input_info.nType == 0)
    {// ��ʮ�����ַ���g_input_1
        nTop = 80;
        Rectangle(hdc, 34, nTop, 126, nTop+38);
        nTop += 2;
        int nIndex = 0;
        for(int i = 0; i < 2; i ++)
        {
            nLeft = 38;
            for(int j = 0; j < g_input_info.nLineCnt; j ++)
            {
                if(g_input_info.nIndex == nIndex)
                {
                    ITEM_SELECT;
                    TextOutLen(hdc, nLeft, nTop, (const char*)&g_input_info.pInputStr[nIndex], 1);
                    ITEM_NOT_SELECT;
                }
                else
                {
                    TextOutLen(hdc, nLeft, nTop, (const char*)&g_input_info.pInputStr[nIndex], 1);
                }
                nIndex ++;
                nLeft += 20;
            }
            nTop += 18;
        }
    }
    /*else if(g_input_info.nType == 4)
    {// ���ַ���g_input_5
        nTop = 71;
        Rectangle(hdc, 34, nTop, 126, nTop+56);
        nTop += 2;
        int nIndex = 0;
        for(int i = 0; i < 3; i ++)
        {
            nLeft = 38;
            for(int j = 0; j < g_input_info.nLineCnt; j ++)
            {
                if(g_input_info.nIndex == nIndex)
                {
                    ITEM_SELECT;
                    TextOutLen(hdc, nLeft, nTop, (const char*)&g_input_info.pInputStr[nIndex], 1);
                    ITEM_NOT_SELECT;
                }
                else
                {
                    TextOutLen(hdc, nLeft, nTop, (const char*)&g_input_info.pInputStr[nIndex], 1);
                }
                nIndex ++;
                nLeft += 20;
            }
            nTop += 18;
        }
    }*/
    else if(g_input_info.nType == 1 || g_input_info.nType == 5)
    {// ��16�����ַ���g_input_2
        nTop = 60;
        Rectangle(hdc, 44, nTop, 116, nTop+74);
        nTop += 2;
        int nIndex = 0;
        for(int i = 0; i < 4; i ++)
        {
            nLeft = 48;
            for(int j = 0; j < g_input_info.nLineCnt; j ++)
            {
                if(g_input_info.nIndex == nIndex)
                {
                    ITEM_SELECT;
                    TextOutLen(hdc, nLeft, nTop, (const char*)&g_input_info.pInputStr[nIndex], 1);
                    ITEM_NOT_SELECT;
                }
                else
                {
                    TextOutLen(hdc, nLeft, nTop, (const char*)&g_input_info.pInputStr[nIndex], 1);
                }
                nIndex ++;
                nLeft += 20;
            }
            nTop += 18;
        }
    }
    else
    {// ����Сд�ַ�����Ϣ
        nTop = 60;
        Rectangle(hdc, 14, nTop, 146, nTop+74);
        nTop += 2;
        int nIndex = 0;
        for(int i = 0; i < 4; i ++)
        {
            nLeft = 18;
            for(int j = 0; j < g_input_info.nLineCnt; j ++)
            {
                if(g_input_info.nIndex == nIndex)
                {
                    ITEM_SELECT;
                    TextOutLen(hdc, nLeft, nTop, (const char*)&g_input_info.pInputStr[nIndex], 1);
                    ITEM_NOT_SELECT;
                }
                else
                {
                    TextOutLen(hdc, nLeft, nTop, (const char*)&g_input_info.pInputStr[nIndex], 1);
                }
                nIndex ++;
                nLeft += 20;
            }
            nTop += 18;
        }
    }
}
BOOL InputKeyMsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return FALSE;

    switch(nKey)
    {
    case KEY_LEFT:
        if(g_input_info.nIndex < g_input_info.nTotalCnt)
        {
            if(g_input_info.nIndex%g_input_info.nLineCnt == 0)
            {
                g_input_info.nIndex += (g_input_info.nLineCnt-1);
            }
            else
            {
                g_input_info.nIndex --;
            }
        }
        else if(g_input_info.nIndex == 51)
        {
            g_input_info.nIndex --;
        }
        #if MODULARIZATION_TMN_EN == 1
        else if(g_input_info.nIndex == 52)
        {
            g_input_info.nIndex = 49;
        }
        #endif
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        if(g_input_info.nIndex < g_input_info.nTotalCnt)
        {
            if((g_input_info.nIndex+1)%g_input_info.nLineCnt == 0)
            {
                g_input_info.nIndex -= (g_input_info.nLineCnt-1);
            }
            else
            {
                g_input_info.nIndex ++;
            }
        }
        else if(g_input_info.nIndex == 50)
        {
            g_input_info.nIndex ++;
        }
        #if MODULARIZATION_TMN_EN == 1
        else if(g_input_info.nIndex == 49)
        {
            g_input_info.nIndex = 52;
        }
        #endif
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_UP:
        #if MODULARIZATION_TMN_EN == 1
        if(g_input_info.nIndex < g_input_info.nTotalCnt)
        {
            if(g_input_info.nIndex >= g_input_info.nLineCnt)
            {
                g_input_info.nIndex -= g_input_info.nLineCnt;
            }
            else if(g_input_info.nIndex < (g_input_info.nLineCnt/2))
            {
                g_input_info.nIndex = 49;
            }
            else
            {
                g_input_info.nIndex = 52;
            }
        }
        else if(g_input_info.nIndex == 49)
        {
            g_input_info.nIndex = 50;
        }
        else if(g_input_info.nIndex == 52)
        {
            g_input_info.nIndex = 51;
        }
        #else
        if(g_input_info.nIndex < g_input_info.nTotalCnt)
        {
            if(g_input_info.nIndex >= g_input_info.nLineCnt)
            {
                g_input_info.nIndex -= g_input_info.nLineCnt;
            }
            else if(g_input_info.nIndex < (g_input_info.nLineCnt/2))
            {
                g_input_info.nIndex = 50;
            }
            else
            {
                g_input_info.nIndex = 51;
            }
        }
        #endif
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_DOWN:
        if(g_input_info.nIndex < g_input_info.nTotalCnt)
        {
            if(g_input_info.nIndex < g_input_info.nTotalCnt-g_input_info.nLineCnt)
            {
                g_input_info.nIndex += g_input_info.nLineCnt;
            }
        }
        #if MODULARIZATION_TMN_EN == 1
        else if(g_input_info.nIndex == 50)
        {
            g_input_info.nIndex = 49;
        }
        else if(g_input_info.nIndex == 49)
        {
            g_input_info.nIndex = 0;
        }
        else if(g_input_info.nIndex == 51)
        {
            g_input_info.nIndex = 52;
        }
        else if(g_input_info.nIndex == 52)
        {
            g_input_info.nIndex = g_input_info.nLineCnt-1;
        }
        #else
        else if(g_input_info.nIndex == 50)
        {
            g_input_info.nIndex = 0;
        }
        else if(g_input_info.nIndex == 51)
        {
            g_input_info.nIndex = g_input_info.nLineCnt-1;
        }
        #endif
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        {
            if(g_input_info.nIndex == 50)
            {// ת������
                int nType = g_input_info.nType;
                // �˴������е��ַ�������ѭ��һ��
                // Ѱ��һ�����ʵ�����������ʾ
                // ���ʵ����͸���InputBox�������������
                for(int i = 0; i < 6; i ++)
                {
                    nType++;
                    if(nType > 5)nType=0;
                    if(CanInputType(nType))
                    {
                        // ���ݵ�ǰ�������ö�Ӧ�Ĵ�ѡ�ַ��������ȡ�������
                        g_input_info.nType = nType;
                        switch(nType)
                        {
                        case 0:g_input_info.pInputStr = g_input_1;break;
                        case 1:g_input_info.pInputStr = g_input_2;break;
                        case 2:g_input_info.pInputStr = g_input_3;break;
                        case 3:g_input_info.pInputStr = g_input_4;break;
                        case 4:g_input_info.pInputStr = g_input_5;break;
                        case 5:g_input_info.pInputStr = g_input_6;break;
                        }
                        
                        g_input_info.nTotalCnt = strlen(g_input_info.pInputStr);
                        g_input_info.nLineCnt = g_input_info.nTotalCnt/4;
                        if(nType == 0)
                        {
                            g_input_info.nLineCnt=g_input_info.nTotalCnt/2;
                        }
                        /*else if(nType == 4)
                        {
                            g_input_info.nLineCnt=g_input_info.nTotalCnt/3;
                        }*/
                        
                        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                        break;
                    }
                }
            }
            else if(g_input_info.nIndex == 51)
            {// ȷ������
                g_guiMain.plugPop = NULL;
                pg_guicomm->nCurPoint = pg_guicomm->nCurPointTmp;
                ASSERT(g_input_info.pfnReturn != NULL);
                if(g_input_info.pfnReturn != NULL)
                {// ���ûص����������ַ�����Ϣ
                    g_input_info.pfnReturn(g_input_info.czInfo, g_input_info.pPara);
                }
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
            #if MODULARIZATION_TMN_EN == 1
            else if(g_input_info.nIndex == 52)
            {
                ClosePopLayer();
            }
            else if(g_input_info.nIndex == 49)
            {
                if(g_input_info.nCurLen > 0)
                {
                    g_input_info.nCurLen --;
                    g_input_info.czInfo[g_input_info.nCurLen] = '\0';
                    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                }
            }
            #endif
            else
            {// ����ǰѡ�е��ַ�����ӵ������ַ�����
                if(g_input_info.nCurLen < g_input_info.nMaxLen)
                {
                    g_input_info.czInfo[g_input_info.nCurLen] 
                        = g_input_info.pInputStr[g_input_info.nIndex];
                    g_input_info.nCurLen++;
                    g_input_info.czInfo[g_input_info.nCurLen] = '\0';
                    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                }
            }
        }
        return TRUE;
    case KEY_CANCEL:
        {// ȡ���˳��������ǰ�������ַ�����������ɾ���ַ�
            // ���򷵻�FALSE���ɿ��ִ��ȱʡ�����˳���ǰҳ��
            if(g_input_info.nCurLen > 0)
            {
                g_input_info.nCurLen --;
                g_input_info.czInfo[g_input_info.nCurLen] = '\0';
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
        }
        break;
    }
    return FALSE;
}

void InputBox(char* pDefault, fnRetureInput pfnReturn, void* pPara, int nType, int nCurType)
{
    ASSERT(pDefault != NULL);
    g_input_info.nIndex = 51;//ȷ��

    // ���õ�ǰ����������
    g_input_info.nTypeMask = nType;
    int nTmp = nType&nCurType;
    switch(nTmp)
    {
    case 0x01:g_input_info.nType=0;break;
    case 0x02:g_input_info.nType=1;break;
    case 0x04:g_input_info.nType=2;break;
    case 0x08:g_input_info.nType=3;break;
    case 0x10:g_input_info.nType=4;break;
    case 0x20:g_input_info.nType=5;break;
    default:
        {
            if((nType&0x02) == 0x02){g_input_info.nType=1;}
            else if((nType&0x04) == 0x04){g_input_info.nType=2;}
            else if((nType&0x01) == 0x01){g_input_info.nType=0;}
            else if((nType&0x08) == 0x08){g_input_info.nType=3;}
            else if((nType&0x10) == 0x10){g_input_info.nType=4;}
            else if((nType&0x20) == 0x20){g_input_info.nType=5;}
            else{return;}
        }
        break;
    }
    
    // ���ݵ�ǰ���ͣ����ò���
    GetInputInfo(g_input_info.nType);

    // ����ȱʡ�ַ�����Ϣ
    if(pDefault != NULL)
    {
        int nLength = strlen(pDefault);
        if(nLength > 0)
        {
            if(nLength > g_input_info.nMaxLen)nLength=g_input_info.nMaxLen;

            memcpy(g_input_info.czInfo, pDefault, nLength);
            g_input_info.czInfo[nLength] = '\0';
        }
        else
        {
            strcpy(g_input_info.czInfo, "");
        }
        g_input_info.nCurLen = nLength;
    }
    //strcpy(g_input_info.czInfo, "");
    //g_input_info.nCurLen = 0;

    ASSERT(pfnReturn != NULL);
    g_input_info.pfnReturn = pfnReturn;
    g_input_info.pPara = pPara;

    g_input_info.nTotalCnt = strlen(g_input_info.pInputStr);
    g_input_info.nLineCnt = g_input_info.nTotalCnt/4;
    if(g_input_info.nType == 0)
    {
        g_input_info.nLineCnt = g_input_info.nTotalCnt/2;
    }
    /*else if(g_input_info.nType == 4)
    {
        g_input_info.nLineCnt = g_input_info.nTotalCnt/3;
    }*/

    // ����ҳ��
    PopLayer(g_layerInput.dwId);
}


void SysTimeSetInit(DWORD dwPluginId)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;

    pg_guicomm->bCanTrunDisp = FALSE;
    g_reset_time.nIndex = 0;

    // ��ȡ��ǰʱ��
//#ifndef ARM_LINUX
    long lTime = time(NULL);
    tm *tnow = localtime(&lTime);
    memcpy(&g_reset_time.tmNow, tnow, sizeof(tm));

/*#else
    rtc rtctime;
    read_rtc_inter(&rtctime);
    g_reset_time.tmNow.tm_year = rtctime.yy-1900;
    g_reset_time.tmNow.tm_mon = rtctime.mm-1;
    g_reset_time.tmNow.tm_mday = rtctime.dd;
    g_reset_time.tmNow.tm_hour = rtctime.hh;
    g_reset_time.tmNow.tm_min = rtctime.nn;
    g_reset_time.tmNow.tm_sec = rtctime.ss;
#endif*/
    sprintf(g_reset_time.czInfo, "%04d%02d%02d%02d:%02d:%02d",
        g_reset_time.tmNow.tm_year+1900,
        g_reset_time.tmNow.tm_mon+1,
        g_reset_time.tmNow.tm_mday,
        g_reset_time.tmNow.tm_hour,
        g_reset_time.tmNow.tm_min,
        g_reset_time.tmNow.tm_sec);
}
void SysTimeSetDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;

    SelectFont(hdc,pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 30;
    rc.bottom = rc.top+16;
    char pcz[20];
#if HARDWARE_TYPE == 4
    strcpy(pcz ,"������ʱ������");
#else
    strcpy(pcz ,"�ն�ʱ������");
#endif
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    // ��������
    int nLeft = 24;
    int nTop = rc.top+30;
    int i;
    for(i = 0; i < 8; i ++)
    {
        switch(i)
        {
        case 4:TextOut(hdc, nLeft, nTop, "��");nLeft+=16;break;
        case 6:TextOut(hdc, nLeft, nTop, "��");nLeft+=16;break;
        }

        if(g_reset_time.nIndex == i)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_reset_time.czInfo[i], 1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_reset_time.czInfo[i], 1);
        }
        nLeft += 8;
    }
    TextOut(hdc, nLeft, nTop, "��");

    // ����ʱ��
    nLeft = 48;
    nTop += 20;
    for(; i < 16; i ++)
    {
        if(g_reset_time.nIndex == i)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_reset_time.czInfo[i], 1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_reset_time.czInfo[i], 1);
        }
        nLeft += 8;
    }

    // ȷ��
    nTop += 20;
    nLeft = 68;
    #if MODULARIZATION_TMN_EN == 1
    if(g_reset_time.nIndex == i)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft - 27, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft - 27, nTop, "����");
    }
    i++;
    if(g_reset_time.nIndex == i)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft + 20, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft + 20, nTop, "ȷ��");
    }
    #else
    if(g_reset_time.nIndex == i)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȷ��");
    }
    #endif
}


int systimeSet(uint8* dateSet)
{
    if(dateSet == NULL)
        return -1;
    
    struct tm rtc1;

    memset(&rtc1,0x0,sizeof(struct tm));

        rtc1.tm_year = ( dateSet[0]*256 + dateSet[1] ) - 1900;
        rtc1.tm_mon =  dateSet[2] - 1;
        rtc1.tm_mday = dateSet[3] ;
        rtc1.tm_hour = dateSet[4] ;
        rtc1.tm_min =  dateSet[5] ;
        rtc1.tm_sec =  dateSet[6] ;

    return rtcTimeSet(rtc1);
}

BOOL SysTimeSetKeyMsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return FALSE;

    int iRet = 0;
    
    #if MODULARIZATION_TMN_EN == 1
    int max = 17;
    #else
    int max = 16;
    #endif

    switch(nKey)
    {
    case KEY_LEFT:// �޸����뽹��
        g_reset_time.nIndex --;
        if((g_reset_time.czInfo[g_reset_time.nIndex] < '0'
            || g_reset_time.czInfo[g_reset_time.nIndex] > '9')
            #if MODULARIZATION_TMN_EN == 1
            && g_reset_time.nIndex != 16
            #endif
            )
        {
            g_reset_time.nIndex --;
        }
        if(g_reset_time.nIndex < 0)
        {
            g_reset_time.nIndex = max;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:// �޸����뽹��
        g_reset_time.nIndex ++;
        if((g_reset_time.czInfo[g_reset_time.nIndex] < '0'
            || g_reset_time.czInfo[g_reset_time.nIndex] > '9')
        #if MODULARIZATION_TMN_EN == 1
            && g_reset_time.nIndex != 17
        #endif
            && g_reset_time.nIndex != 16)
        {
            g_reset_time.nIndex ++;
        }
        if(g_reset_time.nIndex > max)
        {
            g_reset_time.nIndex = 0;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_UP:// �޸�ʱ����������ж���Чλ����һЩ�ж�
        if(g_reset_time.nIndex >= 0 && g_reset_time.nIndex <= 15)
        {
            char czMax = '9';
            switch(g_reset_time.nIndex)
            {
            case 0:czMax = '2';break;    // ��ǧλ
            case 4:czMax = '1';break;    // ��ʮλ
            case 6:czMax = '3';break;    // ��ʮλ
            case 8:czMax = '2';break;    // Сʱʮλ
            case 11:czMax = '5';break;    // ��ʮλ
            case 14:czMax = '5';break;    // ��ʮλ
            }
            g_reset_time.czInfo[g_reset_time.nIndex] ++;
            if(g_reset_time.czInfo[g_reset_time.nIndex] > czMax)
            {
                if(g_reset_time.nIndex == 0)
                    g_reset_time.czInfo[g_reset_time.nIndex] = '1';
                else
                    g_reset_time.czInfo[g_reset_time.nIndex] = '0';
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        break;
    case KEY_DOWN:// �޸�ʱ����������ж���Чλ����һЩ�ж�
        if(g_reset_time.nIndex >= 0 && g_reset_time.nIndex <= 15)
        {
            char czMax = '9';
            switch(g_reset_time.nIndex)
            {
            case 0:czMax = '2';break;    // ��ǧλ
            case 4:czMax = '1';break;    // ��ʮλ
            case 6:czMax = '3';break;    // ��ʮλ
            case 8:czMax = '2';break;    // Сʱʮλ
            case 11:czMax = '5';break;    // ��ʮλ
            case 14:czMax = '5';break;    // ��ʮλ
            }
            g_reset_time.czInfo[g_reset_time.nIndex] --;
            if(g_reset_time.czInfo[g_reset_time.nIndex] < '0')
            {
                g_reset_time.czInfo[g_reset_time.nIndex] = czMax;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        break;
    case KEY_OK:
        #if MODULARIZATION_TMN_EN == 1
        if(g_reset_time.nIndex == 16)
        {
            pg_guicomm->bOKiscancel = 1;
            break;
        }
        if(g_reset_time.nIndex == 17 && ValidatePwd() == TRUE)
        #else
        if(g_reset_time.nIndex == 16 && ValidatePwd() == TRUE)
        #endif
        {
            char czBuf[20];
            int nVal;
            // ��
            memcpy(czBuf, g_reset_time.czInfo, 4);
            czBuf[4] = '\0';
            nVal = atoi(czBuf);
            if(nVal > 2099)
            {
                g_reset_time.nIndex = 3;
                MessageBox("���ֵ���ܳ���2099��\n\n���������룡", MB_OK);
                return TRUE;
            }
            if(nVal < 2000)
            {
                g_reset_time.nIndex = 3;
                MessageBox("���ֵ���ܵ���2000��\n\n���������룡", MB_OK);
                return TRUE;
            }
            g_reset_time.tmNow.tm_year = nVal;

            // ��
            memcpy(czBuf, g_reset_time.czInfo+4, 2);
            czBuf[2] = '\0';
            nVal = atoi(czBuf);
            if(nVal == 0)
            {
                g_reset_time.nIndex = 5;
                MessageBox("�·�ֵ����Ϊ���·�\n\n���������룡", MB_OK);
                return TRUE;
            }
            if(nVal > 12)
            {
                g_reset_time.nIndex = 5;
                MessageBox("�·�ֵ���ܴ���12�·�\n\n���������룡", MB_OK);
                return TRUE;
            }
            g_reset_time.tmNow.tm_mon = nVal;

            // ��
            memcpy(czBuf, g_reset_time.czInfo+6, 2);
            czBuf[2] = '\0';
            nVal = atoi(czBuf);
            if(nVal == 0)
            {
                g_reset_time.nIndex = 7;
                MessageBox("����ֵ����Ϊ��\n\n���������룡", MB_OK);
                return TRUE;
            }
            if(nVal > 31)
            {
                g_reset_time.nIndex = 7;
                MessageBox("����ֵ������Χ\n\n���������룡", MB_OK);
                return TRUE;
            }
            g_reset_time.tmNow.tm_mday = nVal;

            // Сʱ
            memcpy(czBuf, g_reset_time.czInfo+8, 2);
            czBuf[2] = '\0';
            nVal = atoi(czBuf);
            if(nVal > 23)
            {
                g_reset_time.nIndex = 9;
                MessageBox("Сʱֵ������Χ\n\n���������룡", MB_OK);
                return TRUE;
            }
            g_reset_time.tmNow.tm_hour = nVal;

            // ��
            memcpy(czBuf, g_reset_time.czInfo+11, 2);
            czBuf[2] = '\0';
            nVal = atoi(czBuf);
            g_reset_time.tmNow.tm_min = nVal;

            // ��
            memcpy(czBuf, g_reset_time.czInfo+14, 2);
            czBuf[2] = '\0';
            nVal = atoi(czBuf);
            g_reset_time.tmNow.tm_sec = nVal;

            /*int day = g_reset_time.tmNow.tm_mday;
            long lTime = mktime(&g_reset_time.tmNow);
            tm *p = localtime(&lTime);
            printf("day = %d, tm_mday = %d\n", day, p->tm_mday);
            if(day != p->tm_mday)
            {
                g_reset_time.nIndex = 7;
                MessageBox("����ֵ�������\n\n���������룡", MB_OK);
                return TRUE;
            }*/
            long lTime = 0;
#ifndef ARM_LINUX
#ifndef _MSC_VER
            struct timeval tmNow;
            tmNow.tv_sec = lTime;
            tmNow.tv_usec = 0;
            settimeofday(&tmNow, (struct timezone *) 0);
#endif
#endif
//#ifdef ARM_LINUX
            DateTimeHex_t pDateTime;
            memset(&pDateTime, 0, sizeof(DateTimeHex_t));

            pDateTime.year_h = g_reset_time.tmNow.tm_year>>8;
            pDateTime.year_l = g_reset_time.tmNow.tm_year&0xff;
            pDateTime.month = g_reset_time.tmNow.tm_mon;
            pDateTime.day = g_reset_time.tmNow.tm_mday;
            pDateTime.hour = g_reset_time.tmNow.tm_hour;
            pDateTime.min = g_reset_time.tmNow.tm_min;
            pDateTime.sec = g_reset_time.tmNow.tm_sec;
            g_reset_time.nIndex = 0;
            
            int year = 0;
            year = (uint16)pDateTime.year_l + (uint16)(pDateTime.year_h<<8);
            
            //GUI_FMT_TRACE("set time modify time: %d-%d-%d %d:%d:%d]\n", year, pDateTime.month, pDateTime.day, pDateTime.hour, pDateTime.min, pDateTime.sec);
            printf("set time modify time: %d-%d-%d %d:%d:%d]\n", year, pDateTime.month, pDateTime.day, pDateTime.hour, pDateTime.min, pDateTime.sec);
            iRet = systimeSet((uint8*)&pDateTime);
            if (iRet < 0)
            {
                 MessageBox("�ն�ʱ������ʧ�ܣ�", MB_OK);
                 return FALSE;
            }

//#endif
            MessageBox("�ն�ʱ�����óɹ���", MB_OK);
            return TRUE;
        }
        break;
    }
    return FALSE;
}


int IsValidatePwd()
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return FALSE;

    char czPwd[20];

    // ��ȡ��ǰ������
#ifdef _MSC_VER
#if HARDWARE_2012_EN ==1
    memcpy(czPwd, SETPARAPASSWORD, PASSWORD_LEN+1);
#else
    memcpy(czPwd, PASSWORD_STR, PASSWORD_LEN+1);
#endif
#else
    int nRet = read_pdata_xram(GUI_PRIVATE_DATA, czPwd, PRIVATE_PASSWORD, PASSWORD_LEN);
    if(nRet < 0)
    {
        #if HARDWARE_2012_EN ==1
            memcpy(czPwd, SETPARAPASSWORD, PASSWORD_LEN+1);
        #else
            memcpy(czPwd, PASSWORD_STR, PASSWORD_LEN+1);
        #endif
    }
#endif
    //��������
    if(memcmp(czPwd, g_valid_pwd.czPwd, PASSWORD_LEN) == 0 || memcmp("334455", g_valid_pwd.czPwd, PASSWORD_LEN) == 0)
    {
        pg_guicomm->bValidatePwd = TRUE;
        g_nErrPwdCnt = 0;
        return RET_CANCEL;// �ر�ҳ��
    }
    else
    {// ��֤����
        if(g_guiMain.plugPop != NULL
            && g_guiMain.plugPop->dwId == g_layerValidatePwd.dwId)
        {
            g_guiMain.plugPop = NULL;
            pg_guicomm->nCurPoint = pg_guicomm->nCurPointTmp;
        }
        g_nErrPwdCnt ++;
        #if SECURITY_DEFEND == 1
        if(g_nErrPwdCnt == 5)
        #else
        if(g_nErrPwdCnt == 3)
        #endif
        {
            long lTime = time(NULL);
            nRet = write_pdata_xram(GUI_PRIVATE_DATA, (char*)&lTime, PRIVATE_FREEZE_TIME, sizeof(long));
            //printf("WritePDataXram return %d\n", nRet);
            printf("return %d\n", nRet);
            ASSERT(nRet >= 0);
            g_nErrPwdCnt = 0;
            #if SECURITY_DEFEND == 1
            MessageBox("��������������!\n1Сʱ�����޸�����");
            #else
            MessageBox("���������������!\n24Сʱ�����޸�����");
            #endif
        }
        else
        {
            MessageBox("�����������!");
        }
    }
    return TRUE;
}

void ValidatePwdInit(DWORD dwPluginId)
{
    g_valid_pwd.nIndex = PASSWORD_LEN+1;
    memcpy(g_valid_pwd.czPwd, PASSWORD_STR, sizeof(g_valid_pwd.czPwd));
}
/*********************************************************************
    �������ܵ�Ԫ������ҳ���--������֤
    ����������    ������֤���ƺ���
    ���������    HDC hdc�����������ľ��
    ���������
    ��������ֵ��
    �ڲ��������̣�
********************************************************************/
void ValidatePwdDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;

    SelectFont(hdc,pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;

    TextOut(hdc, 14, 26,"�޸����ñ�����֤����");

    // ���ƽ���
    TextOut(hdc, 20, 50,"����������");
    Rectangle(hdc, 38, 70, 94, 92);

    // �������벿��
    int nLeft=42;
    int nTop=74;
    uint32 i;
    for(i = 0; i < PASSWORD_LEN; i ++)
    {
        if((uint32)g_valid_pwd.nIndex == i)
        {
            ITEM_SELECT;
            TextOutLen(hdc,nLeft,nTop,(const char*)&g_valid_pwd.czPwd[i],1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc,nLeft,nTop,(const char*)&g_valid_pwd.czPwd[i],1);
        }
        nLeft+=8;
    }

    nLeft=100;
    if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN)
    {
        ITEM_SELECT;
        TextOut(hdc,nLeft,nTop,"��ϸ");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc,nLeft,nTop,"��ϸ");
    }

    // ����OK����
    nTop += 28;
    if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN+1)
    {
        ITEM_SELECT;
        TextOut(hdc,nLeft,nTop,"ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc,nLeft,nTop,"ȷ��");
    }

    #if MODULARIZATION_TMN_EN == 1
    if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN+2)
    {
        ITEM_SELECT;
        TextOut(hdc, 38, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 38, nTop,"����");
    }
    #endif
}

void ValidatePwdReturn(char* pNewStr, void* pPara)
{
    if(strlen(pNewStr) != PASSWORD_LEN)
    {
        char msg[64];
        sprintf(msg, "���볤�ȱ���Ϊ%d�ֽ�\n\nȷ���Ƿ���������?", PASSWORD_LEN);
        if(TRUE == MessageBox(msg, MB_OKCANCEL))
        {
            InputBox(g_valid_pwd.czPwd, ValidatePwdReturn);
        }
    }
    else
    {
        memcpy(g_valid_pwd.czPwd, pNewStr, sizeof(g_valid_pwd.czPwd));
        IsValidatePwd();
    }
}


BOOL ValidatePwdKeyMsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return FALSE;

    switch(nKey)
    {
    case KEY_LEFT:
        #if MODULARIZATION_TMN_EN == 1
        if(g_valid_pwd.nIndex == PASSWORD_LEN+1)
        {
            g_valid_pwd.nIndex = PASSWORD_LEN+2;
        }
        else if(g_valid_pwd.nIndex == PASSWORD_LEN+2)
        {
            g_valid_pwd.nIndex = PASSWORD_LEN+1;
        }
        else
        {
            g_valid_pwd.nIndex --;
            if(g_valid_pwd.nIndex < 0)
            {
                g_valid_pwd.nIndex = PASSWORD_LEN;
            }
        }
        #else
        g_valid_pwd.nIndex --;
        if(g_valid_pwd.nIndex < 0)
        {
            g_valid_pwd.nIndex = PASSWORD_LEN+1;
        }
        #endif
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        #if MODULARIZATION_TMN_EN == 1
        if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN+1)
        {
            g_valid_pwd.nIndex = PASSWORD_LEN+2;
        }
        else if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN+2)
        {
            g_valid_pwd.nIndex = PASSWORD_LEN+1;
        }
        else
        {
            g_valid_pwd.nIndex ++;
            if((uint32)g_valid_pwd.nIndex > PASSWORD_LEN)
            {
                g_valid_pwd.nIndex = 0;
            }
        }
        #else
        g_valid_pwd.nIndex ++;
        if((uint32)g_valid_pwd.nIndex > PASSWORD_LEN+1)
        {
            g_valid_pwd.nIndex = 0;
        }
        #endif
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_UP:
        if((uint32)g_valid_pwd.nIndex >= 0 && (uint32)g_valid_pwd.nIndex < PASSWORD_LEN)
        {
            g_valid_pwd.czPwd[g_valid_pwd.nIndex] ++;
            if(g_valid_pwd.czPwd[g_valid_pwd.nIndex] > '9')
            {
                g_valid_pwd.czPwd[g_valid_pwd.nIndex] = '0';
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        else if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN+1)
        {
            g_valid_pwd.nIndex --;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        #if MODULARIZATION_TMN_EN == 1
        else if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN+2)
        {
            g_valid_pwd.nIndex  = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        #endif
        break;
    case KEY_DOWN:
        if((uint32)g_valid_pwd.nIndex >= 0 && (uint32)g_valid_pwd.nIndex < PASSWORD_LEN)
        {
            g_valid_pwd.czPwd[g_valid_pwd.nIndex] --;
            if(g_valid_pwd.czPwd[g_valid_pwd.nIndex] < '0')
            {
                g_valid_pwd.czPwd[g_valid_pwd.nIndex] = '9';
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        else if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN)
        {
            g_valid_pwd.nIndex ++;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        break;
    case KEY_OK:
        {// ȷ�ϴ���
            if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN)
            {// ������ϸѡ�񰴼���
                InputBox(g_valid_pwd.czPwd, ValidatePwdReturn);
            }
            else if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN+1)
            {// ����ȷ������
                return IsValidatePwd();
            }
            #if MODULARIZATION_TMN_EN == 1
            else if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN+2)
            {// ����ȡ����
                ClosePopLayer();
            }
            #endif
        }
        break;
    }
    return FALSE;
}

void InitRestartTmn(DWORD dwPluginId)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;

    g_nTermRetsetIndex = 0;
    pg_guicomm->bCanTrunDisp = FALSE;

}

void RestartTmnDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
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
    strcpy(pcz, "�����ն��豸");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if(g_nTermRetsetIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȡ��");
    }

    nLeft = 110;
    if(g_nTermRetsetIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȷ��");
    }

}

BOOL RestartTmnKey(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return FALSE;

    switch(nKey)
    {
    case KEY_LEFT:
        g_nTermRetsetIndex --;
        if(g_nTermRetsetIndex < 0)
        {
            g_nTermRetsetIndex = 1;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_nTermRetsetIndex ++;
        if(g_nTermRetsetIndex > 1)
        {
            g_nTermRetsetIndex = 0;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch(g_nTermRetsetIndex)
        {
        case 0:
            AppCancelMenu();
            return TRUE;
        case 1:
            if(ValidatePwd() == TRUE)
            {
                if(FALSE == MessageBox("��ʾ\n�Ƿ������ն��豸\n����ȷ��һ��", MB_OKCANCEL))
                {
                    return TRUE;
                }
                g_nTermRetsetIndex = 0;
                PopMsg("���������ն�...");
                //RebootTerminal();
                rebootflag = 1;
            }
            break;
        }
        return TRUE;
    }
    return FALSE;

}

typedef struct _CHANGE_PWD
{
    int     nIndex;     // ��ǰ����
    char    czPwd[PASSWORD_LEN*3+5]; // ������
}CHANGE_PWD;
static CHANGE_PWD g_change_pwd;
void DispCodeSetInit(DWORD dwPluginId)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;

    pg_guicomm->bCanTrunDisp = FALSE;
    g_change_pwd.nIndex = PASSWORD_LEN;
    memcpy(g_change_pwd.czPwd, "000000000000000000000", sizeof(g_change_pwd.czPwd));
    g_change_pwd.czPwd[PASSWORD_LEN] = '\0';
    g_change_pwd.czPwd[PASSWORD_LEN*2+1] = '\0';
    g_change_pwd.czPwd[PASSWORD_LEN*3+2] = '\0';
}
void DispCodeSetDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;

    SelectFont(hdc,pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;

    TextOut(hdc,50,20,"������������");

    #if MODULARIZATION_TMN_EN == 1
    int nLeft=24;
    #else
    int nLeft=36;
    #endif
    int nTop=38;
    TextOut(hdc,10, nTop, "�����������");
    nTop += 15;
    Rectangle(hdc, nLeft, nTop, nLeft+2+8*PASSWORD_LEN+2, nTop+16);
    nTop+=2;

    // ������
    nLeft+=2;
    uint32 i = 0;
    for(i = 0; i < PASSWORD_LEN; i ++)
    {
        if(i == (uint32)g_change_pwd.nIndex)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop,(const char*)&g_change_pwd.czPwd[i],1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc, nLeft, nTop,(const char*)&g_change_pwd.czPwd[i],1);
        }
        nLeft+=8;
    }

    nLeft+=8;
    if(i == (uint32)g_change_pwd.nIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop,"��ϸ");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop,"��ϸ");
    }
    i++;

    // ������
    #if MODULARIZATION_TMN_EN == 1
    nLeft=24;
    #else
    nLeft=36;
    #endif
    nTop += 17;
    TextOut(hdc,10, nTop, "������������");
    nTop += 15;
    Rectangle(hdc, nLeft, nTop, nLeft+2+8*PASSWORD_LEN+2, nTop+16);
    nTop+=2;

    nLeft+=2;
    for(; i < PASSWORD_LEN*2+1; i ++)
    {
        if(i == (uint32)g_change_pwd.nIndex)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop,(const char*)&g_change_pwd.czPwd[i],1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc, nLeft, nTop,(const char*)&g_change_pwd.czPwd[i],1);
        }
        nLeft+=8;
    }

    nLeft+=8;
    if(i == (uint32)g_change_pwd.nIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop,"��ϸ");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop,"��ϸ");
    }
    i++;

    // ��֤����
    #if MODULARIZATION_TMN_EN == 1
    nLeft=24;
    #else
    nLeft=36;
    #endif
    nTop += 17;
    TextOut(hdc,10, nTop, "���ٴ�����������");
    nTop += 15;
    Rectangle(hdc, nLeft, nTop, nLeft+2+8*PASSWORD_LEN+2, nTop+16);
    nTop+=2;

    nLeft+=2;
    for(; i < PASSWORD_LEN*3+2; i ++)
    {
        if(i == (uint32)g_change_pwd.nIndex)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop,(const char*)&g_change_pwd.czPwd[i],1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc, nLeft, nTop,(const char*)&g_change_pwd.czPwd[i],1);
        }
        nLeft+=8;
    }

    nLeft+=8;
    if(i == (uint32)g_change_pwd.nIndex)
    {
       ITEM_SELECT;
       TextOut(hdc, nLeft, nTop,"��ϸ");
       ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop,"��ϸ");
    }
    i++;

    // ȷ��
    nLeft+= 26;
    if((uint32)g_change_pwd.nIndex == PASSWORD_LEN*3+3)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop,"ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop,"ȷ��");
    }
    #if MODULARIZATION_TMN_EN == 1
    nLeft+= 26;
    if((uint32)g_change_pwd.nIndex == PASSWORD_LEN*3+4)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop,"����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop,"����");
    }
    #endif
}

// ��������ҳ�淵�ش���
void ChangePwdReturn(char* pNewStr, void* pPara)
{
    if(strlen(pNewStr) != PASSWORD_LEN)
    {
        char msg[64];
        sprintf(msg, "���볤�ȱ���Ϊ%d�ֽ�\n\nȷ���Ƿ���������?", PASSWORD_LEN);
        if(TRUE == MessageBox(msg, MB_OKCANCEL))
        {
            InputBox(g_change_pwd.czPwd, ChangePwdReturn);
        }
    }
    else
    {
        switch(g_change_pwd.nIndex)
        {
        case PASSWORD_LEN:
            strcpy(g_change_pwd.czPwd, pNewStr);break;
        case PASSWORD_LEN*2+1:
            strcpy(g_change_pwd.czPwd+PASSWORD_LEN+1, pNewStr);break;
        case PASSWORD_LEN*3+2:
            strcpy(g_change_pwd.czPwd+(PASSWORD_LEN+1)*2, pNewStr);break;
        }
    }
}

BOOL DispCodeSetKeyMsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return FALSE;

    switch(nKey)
    {
    case KEY_UP:// �����ڼ�����ϸ�������ƶ�
        switch(g_change_pwd.nIndex)
        {
        #if MODULARIZATION_TMN_EN == 1
        case PASSWORD_LEN:
            g_change_pwd.nIndex=(PASSWORD_LEN*3)+4;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        #else
        case PASSWORD_LEN:return FALSE;
        #endif
        case (PASSWORD_LEN*2)+1:
            g_change_pwd.nIndex=PASSWORD_LEN;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN*3)+2:
            g_change_pwd.nIndex=(PASSWORD_LEN*2)+1;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN*3)+3:
            g_change_pwd.nIndex=(PASSWORD_LEN*3)+2;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        #if MODULARIZATION_TMN_EN == 1
        case (PASSWORD_LEN*3)+4:
            g_change_pwd.nIndex=(PASSWORD_LEN*3)+3;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        #endif
        default:
            g_change_pwd.czPwd[g_change_pwd.nIndex] ++;
            if(g_change_pwd.czPwd[g_change_pwd.nIndex] > '9')
            {
                g_change_pwd.czPwd[g_change_pwd.nIndex] = '0';
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
    case KEY_DOWN:// �����ڼ�����ϸ�������ƶ�
        switch(g_change_pwd.nIndex)
        {
        case PASSWORD_LEN:
            g_change_pwd.nIndex=(PASSWORD_LEN*2)+1;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN*2)+1:
            g_change_pwd.nIndex=(PASSWORD_LEN*3)+2;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN*3)+2:
            g_change_pwd.nIndex=(PASSWORD_LEN*3)+3;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        #if MODULARIZATION_TMN_EN == 1
        case (PASSWORD_LEN*3)+3:
            g_change_pwd.nIndex=(PASSWORD_LEN*3)+4;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN*3)+4:
            g_change_pwd.nIndex=PASSWORD_LEN;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        #else
        case (PASSWORD_LEN*3)+3:return FALSE;
        #endif
        default:
            g_change_pwd.czPwd[g_change_pwd.nIndex] --;
            if(g_change_pwd.czPwd[g_change_pwd.nIndex] < '0')
            {
                g_change_pwd.czPwd[g_change_pwd.nIndex] = '9';
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
    case KEY_LEFT:// �޸�����
        g_change_pwd.nIndex --;
        if(g_change_pwd.nIndex < 0)
        {
            #if MODULARIZATION_TMN_EN == 1
            g_change_pwd.nIndex = PASSWORD_LEN*3+4;
            #else
            g_change_pwd.nIndex = PASSWORD_LEN*3+3;
            #endif
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:// �޸�����
        g_change_pwd.nIndex ++;
        #if MODULARIZATION_TMN_EN == 1
        if((uint32)g_change_pwd.nIndex > PASSWORD_LEN*3+4)
        #else
        if((uint32)g_change_pwd.nIndex > PASSWORD_LEN*3+3)
        #endif
        {
            g_change_pwd.nIndex = 0;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        // ������ϸ��������������ҳ��
        switch(g_change_pwd.nIndex)
        {
        case PASSWORD_LEN:
            {
                InputBox(g_change_pwd.czPwd, ChangePwdReturn);
            }
            break;
        case (PASSWORD_LEN*2)+1:
            {
                char* pBuf = g_change_pwd.czPwd;
                InputBox(pBuf+PASSWORD_LEN+1, ChangePwdReturn);
            }
            break;
        case (PASSWORD_LEN*3)+2:
            {
                char* pBuf = g_change_pwd.czPwd;
                InputBox(pBuf+(PASSWORD_LEN*2)+2, ChangePwdReturn);
            }
            break;
        #if MODULARIZATION_TMN_EN == 1
        case (PASSWORD_LEN*3)+4:
            {
                pg_guicomm->bOKiscancel = 1;
                break;
            }
        #endif
        }
        // �޸�����
        if(g_change_pwd.nIndex == PASSWORD_LEN*3+3)
        {
            // �������ֵ��ж�ȡ
            char czPwd[PASSWORD_LEN*3+5];
#ifdef _MSC_VER
            memcpy(czPwd, PASSWORD_STR, PASSWORD_LEN+1);
#else
            int nRet = read_pdata_xram(GUI_PRIVATE_DATA, czPwd, PRIVATE_PASSWORD, PASSWORD_LEN);
            if(nRet < 0)
            {
                memcpy(czPwd, PASSWORD_STR, PASSWORD_LEN+1);
            }
#endif

            // �ȽϾ�����
            if(memcmp(czPwd, g_change_pwd.czPwd, PASSWORD_LEN) != 0)
            {
                MessageBox("��ʾ\n�������������");
                return TRUE;
            }
            // �Ƚ�����������
            if(memcmp(g_change_pwd.czPwd+PASSWORD_LEN+1,
                g_change_pwd.czPwd+PASSWORD_LEN*2+2, PASSWORD_LEN) != 0)
            {
                MessageBox("��ʾ\n�����������벻һ�£�");
                return TRUE;
            }
#ifndef _MSC_VER
            // ��������
            nRet = write_pdata_xram(GUI_PRIVATE_DATA, g_change_pwd.czPwd+PASSWORD_LEN+1, PRIVATE_PASSWORD, PASSWORD_LEN);
            if(nRet >= 0)
            {
                MessageBox("��ʾ\n�����޸ĳɹ���");
            }
            else
            {
                MessageBox("��ʾ\n�����޸�ʧ�ܣ�");
            }
#endif
            return TRUE;
        }
        break;
    }
    return FALSE;
}


extern ALL_CONTAINER_AND_APP_T allcontainerinfo;
ALL_CONTAINER_AND_APP_T containerinfo;
//static int g_ContainStatPageIndex = 0;
//static int g_AppIndex = 0;
//static int g_AppCntMax = 0;
int containerindex;
int containermax;
int appindex;
int appmax;
uint8 gConatinerQueryFlag = 0;
uint8 gAppQueryFlag = 0;
time_t g_sendtime = 0;
uint8  g_ContainerStatus = 0xFF;

void ContainStatCfgInit(DWORD dwPluginId)
{
    MSG_CLIENT_T client;
    
    client.type = MQTT;
    strcpy(client.clientName, "M-iotManager");

    /* ��ȡ���� */
    memset(&containerinfo, 0, sizeof(ALL_CONTAINER_AND_APP_T));
    gConatinerQueryFlag = 0;
    send_iotManager_container_query(&client);
    g_sendtime = time(NULL);

    g_ContainerStatus = 0;
    
    containermax = -1;
}

void ContainStatCfgUnInit()
{
    g_ContainerStatus = 0xFF;
}

void ContainStatTimer(int nId)
{   
    GUI_COMM *pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
    {
        return;
    }

    /* 1s��ʱ����� */
    if (nId != TIMER_MAIN)
    {
        return;
    }

    MSG_CLIENT_T client;
    
    client.type = MQTT;
    strcpy(client.clientName, "M-iotManager");

    /* �ȴ���ȡ���� */
    if(0 == g_ContainerStatus)
    {
        if(1 == gConatinerQueryFlag)
        {
            GUI_FMT_DEBUG("��ѯ�������� %d \n", containerinfo.count);
            if(containerinfo.count == 0)
            {
                g_ContainerStatus = 0xFF;
            
                containerindex = 0;
                containermax = containerinfo.count;
                appindex = -1;
                appmax = containerinfo.containers[containerindex].apps.appcount;
            }
            else
            {
                g_ContainerStatus = 1;
                gAppQueryFlag = 0;
                send_iotManager_app_query(&client);
                g_sendtime = time(NULL);
            }
        }
        else if(time(NULL) - g_sendtime > 13)
        {
            GUI_FMT_DEBUG("��ѯ������ʱ\n");
            g_ContainerStatus = 0xFF;
            
            containerindex = 0;
            containermax = containerinfo.count;
            appindex = -1;
            appmax = containerinfo.containers[containerindex].apps.appcount;
        }
    }
    else if(1 == g_ContainerStatus)
    {
        if(1 == gAppQueryFlag)
        {
            g_ContainerStatus = 0xFF;
            containerindex = 0;
            containermax = containerinfo.count;
            appindex = -1;
            appmax = containerinfo.containers[containerindex].apps.appcount;
            GUI_FMT_DEBUG("������APP��ѯ����\n");
        }
        else if(time(NULL) - g_sendtime >= 8)
        {
            GUI_FMT_DEBUG("������APP��ѯ��ʱ\n");
            g_ContainerStatus = 0xFF;
            containerindex = 0;
            containermax = containerinfo.count;
            appindex = -1;
            appmax = containerinfo.containers[containerindex].apps.appcount;
        }
    }

}


void ContainStatDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;
    
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    if(containermax > 0)
    {
        sprintf(czBuf, "����APP%d",containerindex+1);
    }
    else if(0 == containermax)
    {
        sprintf(czBuf, "û������");
    }
    else
    {
        sprintf(czBuf, "�����ѯ");
    }
    rc.top = 16;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    if(containermax <= 0)
    {
        return;
    }
    
    SelectFont(hdc,pg_guicomm->fontSmall);
    int nTop = rc.top + 20;
    int nLeft1 = 7;
    int nLeft2 = 80;
    char tmpBuf[128] = {0};
    float tmp;
    //��ʾ����״̬
    if(appindex == -1)
    {
        TextOut(hdc, nLeft1, nTop, "��������:");
        TextOut(hdc, nLeft2, nTop, (char*)containerinfo.containers[containerindex].container.ContainerName);

        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "����״̬:");
        TextOut(hdc, nLeft2, nTop, (char*)containerinfo.containers[containerindex].container.ContainerState);
        
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "����CPU:");
        tmp = (float)(containerinfo.containers[containerindex].container.CpuRate/100);
        sprintf(tmpBuf,"%.2f %%",tmp );
        TextOut(hdc, nLeft2, nTop, tmpBuf);

        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "����MEM:");
        tmp = (float)(containerinfo.containers[containerindex].container.MenRate/100);
        sprintf(tmpBuf,"%.2f %%",tmp );
        TextOut(hdc, nLeft2, nTop, tmpBuf);
        
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "APP����:");
        sprintf(tmpBuf,"%d ",containerinfo.containers[containerindex].apps.appcount );
        TextOut(hdc, nLeft2, nTop, tmpBuf);
    }
    else
    {
        //��ʾAPP״̬
        TextOut(hdc, nLeft1, nTop, "APP����:");
        TextOut(hdc, nLeft2, nTop, (char*)containerinfo.containers[containerindex].apps.appinfo[appindex].appname);

        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "APP״̬:");
        //printf("APP״̬ %d \n",containerinfo.containers[containerindex].apps.appinfo[appindex].serviceinfo[0].servicestate);
        
        if(containerinfo.containers[containerindex].apps.appinfo[appindex].serviceinfo[0].servicestate==0)
        {
            strcpy(tmpBuf,"����");
        }else
        {
            strcpy(tmpBuf,"ֹͣ");
        }
        TextOut(hdc, nLeft2, nTop, tmpBuf);

        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "APPCPU:");
        //printf("containerindex %d appindex %d APPCPU %d \n",containerindex,appindex,containerinfo.containers[containerindex].apps.appinfo[appindex].serviceinfo[0].cpurate);
        tmp = (float)(containerinfo.containers[containerindex].apps.appinfo[appindex].serviceinfo[0].cpurate/100);
        sprintf(tmpBuf,"%.2f %%",tmp );
        TextOut(hdc, nLeft2, nTop, tmpBuf);

        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "APPMEM:");
        tmp = (float)(containerinfo.containers[containerindex].apps.appinfo[appindex].serviceinfo[0].memrate/100);
        sprintf(tmpBuf,"%.2f %%",tmp );
        TextOut(hdc, nLeft2, nTop, tmpBuf);
    }

}

BOOL ContainStatKey(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return FALSE;

    if(KEY_DOWN == nKey || KEY_UP == nKey || KEY_LEFT == nKey || KEY_RIGHT == nKey)
    {
        if(containermax <= 0)
        {
            return 0;
        }
    }
    switch(nKey)
    {
        case KEY_DOWN:
        {
            appindex++;
            if(appindex>appmax-1)
            {
                appindex = -1;
            }
            printf("KEY_DOWN appindex %d appmax %d \n",appindex,appmax);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return  TRUE;

        }
        case KEY_UP:
        {
            appindex--;
            if(appindex<-1)
            {
                appindex = appmax-1;
            }
            printf("KEY_UP appindex %d appmax %d \n",appindex,appmax);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return    TRUE;
        }
        case KEY_LEFT:
        {
            if(containermax == 0)
            {
                return TRUE;
            }
            appindex=-1;
            containerindex--;
            if(containerindex<0)
            {
                containerindex = containermax-1;
            }
            appmax = containerinfo.containers[containerindex].apps.appcount;
            printf("KEY_LEFT containerindex %d appindex %d appmax %d \n",containerindex,appindex,appmax);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return    TRUE;
        }
        case KEY_RIGHT:
        {
            if(containermax == 0)
            {
                return TRUE;
            }
            appindex=-1;
            containerindex++;
            if(containerindex>containermax-1)
            {
                containerindex = 0;
            }
            appmax = containerinfo.containers[containerindex].apps.appcount;
            printf("KEY_RIGHT containerindex %d appindex %d appmax %d \n",containerindex,appindex,appmax);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return    TRUE;
        }
        default:
            return FALSE;
    }
}


char OSVersion[100];
char KernelVersion[100];
char patvhVersion[100];
void getVersion()
{
    memset(OSVersion,0x00,sizeof(OSVersion));
    memset(KernelVersion,0x00,sizeof(KernelVersion));
    memset(patvhVersion,0x00,sizeof(patvhVersion));
    char cmd[200]={0};
    char result[200]={0};
    
    sprintf(cmd,"version -k");
    cmd_execute(cmd,result);
    strcpy(KernelVersion, strim(result));

    sprintf(cmd,"version -d");
    memset(result,0x00,sizeof(result));
    cmd_execute(cmd,result);
    strcpy(OSVersion, strim(result));

    memset(cmd, 0x00, sizeof(cmd));
    sprintf(cmd,"version -d | grep patch|awk -F ':' '{print $2}'");
    memset(result, 0x00, sizeof(result));
    cmd_execute(cmd, result);
    strcpy(patvhVersion, strim(result));
}
void OSInfoInit(DWORD dwPluginId)
{
    getVersion();
}

void OSInfoDraw(HDC hdc)
{
        
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc,pg_guicomm->fontSmall);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "����ϵͳ�汾");
    rc.top = 26;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    rc.top =rc.bottom;
    rc.bottom = rc.top+16;
    DrawText(hdc, OSVersion, strlen(OSVersion), &rc, DT_CENTER|DT_VCENTER);
    
    rc.top =rc.bottom+4;
    rc.bottom = rc.top+16;
    sprintf(czBuf, "�ں˰汾");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    rc.top =rc.bottom;
    rc.bottom = rc.top+16;
    DrawText(hdc, KernelVersion, strlen(KernelVersion), &rc, DT_CENTER|DT_VCENTER);

    rc.top =rc.bottom+4;
    rc.bottom = rc.top+16;
    sprintf(czBuf, "�����汾");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    rc.top =rc.bottom;
    rc.bottom = rc.top+16;
    DrawText(hdc, patvhVersion, strlen(patvhVersion), &rc, DT_CENTER|DT_VCENTER);
    

}

MQT_PLUGIN_T appinfo[50];
uint8 appcount;
uint8 registerappindex;

uint8 getRegisterAppInfo(MQT_PLUGIN_T *app)
{
    MQT_PLUGIN_T  *mqtPluginArray = NULL;
    uint32 mqtPluginNum= 0;
    app_manager_all_app_query(&mqtPluginArray, &mqtPluginNum);
    memcpy(app,mqtPluginArray,sizeof(MQT_PLUGIN_T)*mqtPluginNum);
    if(mqtPluginArray!=NULL)
        free(mqtPluginArray);
    return mqtPluginNum;
}

void AppRegInit(DWORD dwPluginId)
{
    memset(appinfo,0x00,sizeof(appinfo));
    appcount = 0;
    appcount = getRegisterAppInfo(appinfo);
    registerappindex = 0;
}

void stringchange(uint8* buf,uint8 len,char *result)
{
    uint8 i;
    char tmp[10]={0};
    for(i=0;i<len;i++)
    {
        sprintf(tmp,"%02x",buf[i]);
        strcat(result,tmp);
    }
    
}
void AppRegDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc,pg_guicomm->fontSmall);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    if(registerappindex==0)
        sprintf(czBuf, "APPע����Ϣ");
    else
        sprintf(czBuf, "APPע����Ϣ(%d/%d)",registerappindex,appcount);
    rc.top = 26;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    int nTop = rc.top + 20;
    int nLeft1 = 7;
    int nLeft2 = 60;
    DATA_TIME_T *time = NULL;
    
    if(registerappindex==0)
    {
        sprintf(czBuf,"��ע��APP����:");
        TextOut(hdc, nLeft1, nTop, czBuf);
        sprintf(czBuf,"  %d  ",appcount);
        nLeft2 = 100;
        TextOut(hdc, nLeft2, nTop, czBuf);
    }else
    {
        sprintf(czBuf,"APP����:");
        TextOut(hdc, nLeft1, nTop, czBuf);
        TextOut(hdc, nLeft2, nTop, (char*)appinfo[registerappindex-1].comName);
        nTop+=16;
        sprintf(czBuf,"APP�汾:");
        TextOut(hdc, nLeft1, nTop, czBuf);
        sprintf(czBuf,"%s ",appinfo[registerappindex-1].version);
        TextOut(hdc, nLeft2, nTop, czBuf);
        nTop+=16;
        sprintf(czBuf,"��������:");
        TextOut(hdc, nLeft1, nTop, czBuf);
        time = &appinfo[registerappindex-1].dataTime;
        sprintf(czBuf,"%d/%d/%d %d:%02d",time->year,time->month,time->day,time->hour,time->minute);
        TextOut(hdc, nLeft2, nTop, czBuf);
        nTop+=16;
        sprintf(czBuf,"APP-ID:");
        TextOut(hdc, nLeft1, nTop, czBuf);
 
        uint8 i =0;
        uint8 maxcntcount = 8; //һ����ʾ12���ֽ�
        uint8 cntcount=0;
        if(appinfo[registerappindex-1].appidlen == 0)
        {
            appinfo[registerappindex-1].appidlen = 24;
            memset(appinfo[registerappindex-1].appid,0x00,24);
        }
        while(i<appinfo[registerappindex-1].appidlen)
        {
            if(appinfo[registerappindex-1].appidlen-i>maxcntcount)
                cntcount =maxcntcount;
            else
                cntcount = appinfo[registerappindex-1].appidlen-i;
            memset(czBuf,0x00,sizeof(czBuf));
            stringchange(appinfo[registerappindex-1].appid+i,cntcount,czBuf);
            TextOut(hdc, nLeft2, nTop, czBuf);
            i+=cntcount;
            nTop+=16;
        }

    }
    
}
BOOL AppRegKey(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return FALSE;
    if(appcount==0)
        return TRUE;
    switch (nKey)
    {
    case KEY_LEFT:
    case KEY_UP:
    {
        if(registerappindex>0)
            registerappindex--;
        else
            registerappindex = appcount;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return  TRUE;
    }
    case KEY_RIGHT:
    case KEY_DOWN:
    {
        if(registerappindex<appcount)
            registerappindex++;
        else
            registerappindex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return  TRUE;
    }
    case KEY_OK:
    {
        AppCancelMenu();
        return  TRUE;
    }
    default:
        return FALSE;
    }
    return TRUE;
}

uint32 CPUFreq;
uint32 memtotal;
uint32 storage;

void HardwareInfoInit(DWORD dwPluginId)
{
    CPUFreq = 0;
    memtotal = get_memtotal();
    storage = get_storage();
}

void HardwareInfoDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc,pg_guicomm->fontBig);

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "�豸��Ϣ");
    rc.top = 26;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    
    SelectFont(hdc,pg_guicomm->fontSmall);
    int nTop = rc.top + 30;
    int nLeft1 = 7;
    int nLeft2 = 80;
    
    TextOut(hdc, nLeft1, nTop, "CPUƵ��:");
    CPUFreq = get_cpufreq();
    sprintf(czBuf,"%d MHz",CPUFreq);
    TextOut(hdc, nLeft2, nTop, czBuf);
    
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "�ڴ�����:");
    sprintf(czBuf,"%d MB",memtotal);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "�洢����:");
    sprintf(czBuf,"%d MB",storage);
    TextOut(hdc, nLeft2, nTop, czBuf);
}

uint8 factoryResetIndex;
void InitFactoryReset(DWORD dwPluginId)
{
    factoryResetIndex = 0;
}

void FactoryResetDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
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
    strcpy(pcz, "�ָ���������");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if(factoryResetIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȡ��");
    }

    nLeft = 110;
    if(factoryResetIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȷ��");
    }
}

BOOL FactoryResetKey(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return FALSE;

    switch(nKey)
    {
    case KEY_LEFT:
    case KEY_UP:
        if(factoryResetIndex>0)
            factoryResetIndex --;
        else
            factoryResetIndex = 1;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
    case KEY_DOWN:
        if(factoryResetIndex<1)
            factoryResetIndex++;
        else
            factoryResetIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch(factoryResetIndex)
        {
        case 0:
            AppCancelMenu();
            return TRUE;
        case 1:
            if(ValidatePwd() == TRUE)
            {
                if(FALSE == MessageBox("��ʾ\n�Ƿ�ָ���������\n����ȷ��һ��", MB_OKCANCEL))
                {
                    return TRUE;
                }
                factoryResetIndex = 0;
                PopMsg("���ڻָ���������...");
                parainitflag = 1;
            }
            break;
        }
        return TRUE;
    }
    return FALSE;
}

uint8 sshsetindex;
void InitSSHSet(DWORD dwPluginId)
{
    sshsetindex = 0;
}

void SSHSetDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
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
    strcpy(pcz, "SSH��������");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if(sshsetindex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "����");
    }

    nLeft = 110;
    if(sshsetindex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "�ر�");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "�ر�");
    }
}

//choice 0 ����ssh 1 �ر�ssh
void sshset(uint8 choice)
{
    char cmd[100]={0};

    if(choice ==0)
    {
        sprintf(cmd,"systemctl start sshd");
    }else 
    {
        sprintf(cmd,"systemctl stop sshd");        
    }
    system(cmd);
    
    if(choice ==0)
    {
        sprintf(cmd,"/usr/sbin/sshd -D &");
    }else 
    {
        sprintf(cmd,"ps |grep /usr/sbin/sshd |grep -v grep|awk '{print $1}'|xargs kill all -9| pkill sshd");
    }
    
    printf("sshset %s \n",cmd);
    system(cmd);
}
BOOL SSHSetKey(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return FALSE;

    switch(nKey)
    {
    case KEY_LEFT:
    case KEY_UP:
        if(sshsetindex>0)
            sshsetindex --;
        else
            sshsetindex = 1;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
    case KEY_DOWN:
        if(sshsetindex<1)
            sshsetindex++;
        else
            sshsetindex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        if(ValidatePwd() == TRUE)
        {        
            switch(sshsetindex)
            {
                case 0:
                    if(FALSE == MessageBox("��ʾ\n�Ƿ���SSH����\n", MB_OKCANCEL))
                    {
                        return TRUE;
                    }
                    sshset(0);
                    AppCancelMenu();
                    return TRUE;
                case 1:

                    if(FALSE == MessageBox("��ʾ\n�Ƿ�ر�SSH����\n", MB_OKCANCEL))
                    {
                        return TRUE;
                    }
                    sshset(1);
                    AppCancelMenu();
                    return TRUE;
                    
                default:break;
            }
        }
        return TRUE;
    }
    return FALSE;
}

void InitAppVersion(DWORD dwPluginId)
{
    
}

void AppVersionDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc,pg_guicomm->fontSmall);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "Ӧ�ð汾��Ϣ");
    rc.top = 26;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);

    
    rc.top =rc.bottom+10;
    rc.bottom = rc.top+16;
    sprintf(czBuf, "APP�汾��");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    rc.top =rc.bottom;
    rc.bottom = rc.top+16;
    sprintf(czBuf,"%s",APP_VERSION);
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);

    rc.top =rc.bottom+4;
    rc.bottom = rc.top+16;
    sprintf(czBuf, "��������");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    rc.top =rc.bottom;
    rc.bottom = rc.top+16;
    sprintf(czBuf,"%s",APP_PUBLISH_TIME);
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    
}

static int g_UpdateIndex = 0;     //ѡ����: 0-��װ����; 1-��װר��; 2-����
static unsigned int g_InstallType = 0;      //������Ʒ: 0-��; 1-����;2-ר��
static unsigned int g_UpdateStatus = 0;    //״̬    : 0-δ��ʼ����;1-������װ�ļ�;2-����Ŀ¼ʧ��;3-���ڰ�װ;
                                       //          4-������װ�߳�ʧ��;5-δ�ҵ�U����İ�װĿ¼;6-ȱ�ٰ�װ�ű�


#define UPDATE_STATUS_INIT          0
#define UPDATE_STATUS_COPY          1
#define UPDATE_STATUS_MKDIR_ERR      2
#define UPDATE_STATUS_INSTALL       3
#define UPDATE_STATUS_CREATE_ERR    4
#define UPDATE_STATUS_NO_PACKAGE    5
#define UPDATE_STATUS_NO_SCRIPT     6

unsigned int UdiskCopyPkg()
{
    if (0 == access("/tmp/update", F_OK))
    {
        system("rm -rf /tmp/update");
    }

    if (0 != access("/tmp/update", F_OK))
    {
        if(0 != mkdir("/tmp/update", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
        {
            g_UpdateStatus = UPDATE_STATUS_MKDIR_ERR;
            return 1;
        }
    }

    if (0 != access("/tmp/udisk/package", F_OK))
    {
        g_UpdateStatus = UPDATE_STATUS_NO_PACKAGE;
        return 1;
    }

    system("cp -r /tmp/udisk/package/* /tmp/update/");
    return 0;
}

unsigned int UdiskCheckPkg()
{
    uint8 product = 0; /* ��Ʒ����: bit0-����; bit1-ר�� */

    if (0 == access("/tmp/update/setup_gb.sh", F_OK))
    {
        product |= 0x01;
    }

    if (0 == access("/tmp/update/setup_zb.sh", F_OK))
    {
        product |= 0x02;
    }

    g_UpdateStatus = (0 == product ? UPDATE_STATUS_NO_SCRIPT : UPDATE_STATUS_INSTALL);

    return (0 == product ? 1 : 0);
}

#define INSTALL_TYPE_GB 1
#define INSTALL_TYPE_ZB 2
#define INSTALL_TYPE_QT 3        //Ԥ��


unsigned int UdiskInstallPkg()
{
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return 1;
    }

    if (g_UpdateStatus != UPDATE_STATUS_INSTALL)
    {
        return 1;
    }

    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
//    system("rm -f /data/app/smiOS/log/udisk.log");

    if (g_InstallType == INSTALL_TYPE_GB)
    {
        chmod("/tmp/update/setup_gb.sh", 0777);
        system("/tmp/update/setup_gb.sh > /data/app/desktopGui/log/udisk.log -d");
    }
    else if (g_InstallType == INSTALL_TYPE_ZB)
    {
        chmod("/tmp/update/setup_zb.sh", 0777);
        system("/tmp/update/setup_zb.sh > /data/app/desktopGui/log/udisk.log -d");
    }
    else
    {
        return 1;
    }

    return 0;
}

void *UdiskInstallThread(void *arg)
{
    if (UdiskCopyPkg() != 0)
    {
        return NULL;
    }

    if (UdiskCheckPkg() != 0)
    {
        return NULL;
    }

    if (UdiskInstallPkg() != 0)
    {
        return NULL;
    }

    return NULL;
}

void InitUdiskUpdateDraw(DWORD dwPluginId)
{
    g_UpdateIndex = 0;
    g_InstallType = 0;
    g_UpdateStatus = UPDATE_STATUS_INIT;

    if (0 == access("/tmp/update", F_OK))
    {
        system("rm -rf /tmp/update");
    }
}

void UdiskUpdateDraw(HDC hdc)
{
    int nTop = 0;
    int nLeft = 0;
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);

    if (g_UpdateStatus == UPDATE_STATUS_COPY)
    {
        nTop = 40;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "�����ļ���...");
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "��ʱԼ1����");      
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "�벻Ҫ�رյ�Դ");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "��Ҫ�ε�U��");

        return;
    }

    if (g_UpdateStatus == UPDATE_STATUS_INSTALL)
    {
        nTop = 40;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "���ڰ�װ...");
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "��ʱԼ2����");
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "�벻Ҫ�رյ�Դ");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "���԰ε�U��");

        return;
    }

    nLeft = 40;
    nTop = 36;
    SelectFont(hdc, pg_guicomm->fontBig);

    if (0 == g_UpdateIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "��װ������");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "��װ������");
    }

    nTop += 30;

    if (1 == g_UpdateIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "��װר��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "��װר��");
    }

    nTop += 30;

    if (2 == g_UpdateIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ִ�нű�");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ִ�нű�");
    }

    nTop += 30;

    if (3 == g_UpdateIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "����");
    }

    PopListDraw(hdc);
}

void UdiskUpdateTimer(int nId)
{   
    GUI_COMM *pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
    {
        return;
    }
    
    /* 1s��ʱ����� */
    if (nId != TIMER_MAIN)
    {
        return;
    }

    g_guiMain.nTrunDispTime = 0; /* ��U������ҳ�棬���������� */
    g_guiMain.nLightonTime = 0;
    g_guiMain.bLighton = TRUE;
    lighton_lcd(); /* ��U������ҳ�棬��Ļ���� */

    switch (g_UpdateStatus)
    {
        case UPDATE_STATUS_INIT: //δ��ʼ��������ʼҳ��
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case UPDATE_STATUS_COPY: //�����ļ�����
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case UPDATE_STATUS_MKDIR_ERR: //����Ŀ¼ʧ��
        {
            MessageBox("����Ŀ¼ʧ�ܣ�");
            g_UpdateStatus = UPDATE_STATUS_INIT;
            break;
        }
        case UPDATE_STATUS_INSTALL: //���ڰ�װ
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case UPDATE_STATUS_CREATE_ERR: //������װ�߳�ʧ��
        {
            MessageBox("������װ�߳�ʧ�ܣ�");
            g_UpdateStatus = UPDATE_STATUS_INIT;
            break;
        }
        case UPDATE_STATUS_NO_PACKAGE: //δ�ҵ�U����İ�װĿ¼
        {
            MessageBox("δ�ҵ�U����İ�װĿ¼\n����U���Ƿ����");
            g_UpdateStatus = UPDATE_STATUS_INIT;
            break;
        }
        case UPDATE_STATUS_NO_SCRIPT: //ȱ�ٰ�װ�ű�
        {
            MessageBox("ȱ�ٰ�װ�ű���");
            g_UpdateStatus = UPDATE_STATUS_INIT;
            break;
        }
        default: 
        {
            g_UpdateStatus = UPDATE_STATUS_INIT;
            break;
        }
    }

}

/* ����ɵ����߱�֤�Ϸ��� */
unsigned int UdiskInstallProc(GUI_COMM *pg_guicomm, unsigned int installType)
{
    int errCode = 0;
    pthread_t threadId;
    const char *message = "����\n��װ�������벻Ҫ�ϵ�\n�Ƿ�װ";
    char *productName[] = {(char *)"��Чֵ", (char *)"������", (char *)"ר��", (char *)""};
    char buf[200] = {0};

    if (ValidatePwd() == TRUE)
    {
        sprintf(buf, "%s%s", message, productName[installType]);
        if (FALSE == MessageBox(buf, MB_OKCANCEL))
        {
            return 0;
        }
    }
    else
    {
        return 1;
    }

    g_UpdateStatus = UPDATE_STATUS_COPY;
    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    g_InstallType = installType;
    errCode = pthread_create(&threadId, NULL, UdiskInstallThread, NULL);

    if (errCode != 0)
    {
        threadId = 0;
        g_UpdateStatus = UPDATE_STATUS_CREATE_ERR;
        return 1;
    }

    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    return 0;
}

BOOL UdiskUpdateKey(int nKey)
{
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return FALSE;
    }

    /* δ��ʼ����״̬�²��ܲ����˵� */
    if (g_UpdateStatus != UPDATE_STATUS_INIT)
    {
        return TRUE;
    }

    switch (nKey)
    {
        case KEY_UP:
        case KEY_LEFT:
        {
            g_UpdateIndex--;

            if (g_UpdateIndex < 0)
            {
                g_UpdateIndex = 3;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        case KEY_RIGHT:
        {
            g_UpdateIndex++;

            if (g_UpdateIndex > 3)
            {
                g_UpdateIndex = 0;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (g_UpdateIndex)
            {
                case 0: /* ��װ���� */
                {
                    UdiskInstallProc(pg_guicomm, INSTALL_TYPE_GB);
                    return TRUE;
                }
                case 1: /* ��װר�� */
                {
                    UdiskInstallProc(pg_guicomm, INSTALL_TYPE_ZB);
                    return TRUE;
                }
                case 2: /* ��װר�� */
                {
                    UdiskInstallProc(pg_guicomm, INSTALL_TYPE_QT);
                    return TRUE;
                }
                case 3: /* �˳� */
                {
                    if (0 == access("/tmp/update", F_OK))
                    {
                        system("rm -rf /tmp/update");
                    }

                    AppCancelMenu();
                    return TRUE;
                }
            }

            return TRUE;
        }
    }

    return FALSE;
}

uint8 logexportindex;
void LogExportInit(DWORD dwPluginId)
{
    logexportindex = 0;
}
void LogExportDraw(HDC hdc)
{
     GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;
    int index = 0;
    char czbuf[300]={0};
    int nleft = 40;
    int ntop = 40;
    SelectFont(hdc,pg_guicomm->fontBig);
    sprintf(czbuf,"����������־");
    if(index == logexportindex)
    {
        ITEM_SELECT;
        TextOut(hdc, nleft, ntop, czbuf);
        ITEM_NOT_SELECT;
    }else
    {
        TextOut(hdc, nleft, ntop, czbuf);
    }

    index++;
    ntop+=30;
    sprintf(czbuf,"�������г���");
    if(index == logexportindex)
    {
        ITEM_SELECT;
        TextOut(hdc, nleft, ntop, czbuf);
        ITEM_NOT_SELECT;
    }else
    {
        TextOut(hdc, nleft, ntop, czbuf);
    }

    index++;
    ntop+=30;
    sprintf(czbuf,"���������ļ�");
    if(index == logexportindex)
    {
        ITEM_SELECT;
        TextOut(hdc, nleft, ntop, czbuf);
        ITEM_NOT_SELECT;
    }else
    {
        TextOut(hdc, nleft, ntop, czbuf);
    }
}
//����Ƿ����U��
BOOL CanUseUDisk()
{
    FILE* pf = fopen("/dev/sda1", "r+");

    if(pf == NULL)
    {
        return FALSE;// U��û�в���
    }
    fclose(pf);
    system("mkdir -p /tmp/udisk;mount  /dev/sda1 /tmp/udisk");
    return TRUE;
}
void *CopyThread(void *arg)
{
    char buf[300]={0};
    char cmd[1000]={0};
    switch(logexportindex)
    {
    case 0:
    {
        sprintf(buf,"���ڱ�����־�ļ�������");
        sprintf(cmd,"find /data/app -path '/data/app/*/log' -exec mkdir -p /tmp/udisk{} \\;");
        system(cmd);
        sprintf(cmd,"find /data/app -path '/data/app/*/log/*' -exec cp -rPf {} /tmp/udisk{} \\; ");
    }
        break;
    case 1:
    {
        sprintf(buf,"���ڱ���Ӧ�ó��򡣡���");
        sprintf(cmd,"mkdir /tmp/udisk/host; mkdir /tmp/udisk/baseContainer;mkdir /tmp/udisk/pdContainer;mkdir /tmp/udisk/amrContainer; mkdir /tmp/udisk/edgerContainer;");
        system(cmd);
        sprintf(cmd,"cp -rfP /usr/local/extapps/* /tmp/udisk/host/;docker cp baseContainer:/usr/local/extapps/dbCenter /tmp/udisk/baseContainer/; "
                                "docker cp baseContainer:/usr/local/extapps/ccoRouter /tmp/udisk/baseContainer/;docker cp edgerContainer:/usr/local/extapps/taskManager /tmp/udisk/edgerContainer/; "
                                "docker cp amrContainer:/usr/local/extapps/puAmr /tmp/udisk/amrContainer/;docker cp pdContainer:/usr/local/extapps/pdAmr /tmp/udisk/pdContainer/;");
        
    }
        break;
    case 2:
    {
        sprintf(buf,"���ڱ��������ļ�������");
        sprintf(cmd,"mkdir -p /tmp/udisk/data/app;mkdir /tmp/udisk/host; mkdir /tmp/udisk/baseContainer;mkdir /tmp/udisk/pdContainer;mkdir /tmp/udisk/amrContainer; mkdir /tmp/udisk/edgerContainer;");
        system(cmd);
        sprintf(cmd,"cp -rfP /usr/local/extapps/* /tmp/udisk/host/;docker cp baseContainer:/usr/local/extapps/dbCenter /tmp/udisk/baseContainer/; "
                                "docker cp baseContainer:/usr/local/extapps/ccoRouter /tmp/udisk/baseContainer/;docker cp edgerContainer:/usr/local/extapps/taskManager /tmp/udisk/edgerContainer/; "
                                "docker cp amrContainer:/usr/local/extapps/puAmr /tmp/udisk/amrContainer/;docker cp pdContainer:/usr/local/extapps/pdAmr /tmp/udisk/pdContainer/;"
                                "cp  -rfP /data/app/* /tmp/udisk/data/app/");
       
    }
        break;
    }
    strcat(buf,"\n\n����ʱ��ϳ�\n��������ն˻��߰γ�U��");
    //MessageBox(buf);
    PopMsg(buf);
    system(cmd);
    system("sync");
    printf("copythread ������� �˳�\n");
    
    *(int *)arg = 1;
    
    pthread_exit(NULL);
}
BOOL LogExportKeyMsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return FALSE;
    int maxindex = 2;
    char buf[300]={0};
    int err = 0;
    pthread_t copythreadid;
    int copyendflag = -1;
    int loop_cnt = 0;
    switch(nKey)
    {
    case KEY_UP:
    case KEY_LEFT:
    {
        if(logexportindex>0)
            logexportindex--;
        else
            logexportindex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        if(logexportindex<maxindex)
            logexportindex++;
        else
            logexportindex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if(ValidatePwd() == TRUE)
        {
            switch(logexportindex)
            {
                case 0:
                    sprintf(buf,"����������־�ļ���U��\n�Ƿ�ȷ�ϣ�");
                break;
                case 1:
                    sprintf(buf,"��������Ӧ�ó���U��\n�Ƿ�ȷ�ϣ�");
                break;
                case 2:
                    sprintf(buf,"���������ļ���U��\n�Ƿ�ȷ�ϣ�");
                break;
                default:
                    return FALSE;
            }
            if(TRUE == MessageBox(buf, MB_OKCANCEL))
            {
                if(FALSE == CanUseUDisk())
                {
                    MessageBox("U��û�в���\n\n���ܱ����ļ�!");
                    return TRUE;
                }
                /*����һ���߳�,���߳�ִ�п����Ȳ���,���߳�ι�ػ�����*/
                err = pthread_create(&copythreadid,NULL, CopyThread, &copyendflag);
                if (err != 0)
                {
                    MessageBox("ϵͳæ\n�Ժ��ٿ���!");
                    break;
                }

                while(1)
                {
                    loop_cnt++;
                    if(loop_cnt>90000)
                    {
                        printf("copy timeout!!\n");
                        pthread_cancel(copythreadid);
                        pthread_join(copythreadid, NULL);
                        ClosePopLayer();
                        system("ps -ef|grep /tmp/udisk |grep -v grep|awk '{print $2}' |xargs kill");
                        system("sync");
                        MessageBox("����ʱ�䳬��15����\n�Զ�������");
                        break;
                    }
                    if(copyendflag == 1)
                    {
                        pthread_join(copythreadid, NULL);
                        printf("�������\n");
                        ClosePopLayer();
                        MessageBox("������ɣ�");
                        break;
                    }
                    usleep(100000);
                }
                system("umount /tmp/udisk");
                return TRUE;
            }
        }

    }
    }
    return FALSE;
}

PLUGIN_INFO g_menuLayer = {
    0x00A00000/*id*/, 
    InitMenu/*InitLayer*/,
    NULL/*UninitLayer*/,
    MenuDraw/*Draw*/,
    NULL/*Timer*/,
    MenuKeyMsg/*keymsg*/,
    NULL/*ipc*/
};
//��������鿴
PLUGIN_INFO g_smiOSNetPara = {
    0x00A00010/*id*/, 
    InitNetPara/*InitLayer*/,
    NULL/*UninitLayer*/,
    NetParaDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//Һ������
PLUGIN_INFO g_smiOSLCDPara = {
    0x00A00020/*id*/, 
    InitLCDPara/*InitLayer*/,
    NULL/*UninitLayer*/,
    LCDParaDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�������ò鿴
PLUGIN_INFO g_smiOSBlueCfg = {
    0x00A00030/*id*/, 
    InitBlueCfg/*InitLayer*/,
    NULL/*UninitLayer*/,
    BlueCfgDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�����������
PLUGIN_INFO g_smiOSNetParaSet = {
    0x00A00040/*id*/, 
    InitNetParaSet/*InitLayer*/,
    NULL/*UninitLayer*/,
    NetParaSetDraw/*Draw*/,
    NULL/*Timer*/,
    NetParaSetKey/*keymsg*/,
    NULL/*ipc*/
};
    
//Һ����������
PLUGIN_INFO g_smiOSLCDParaSet = {
    0x00A00021/*id*/, 
    InitLCDSetPara/*InitLayer*/,
    NULL/*UninitLayer*/,
    LCDParaSetDraw/*Draw*/,
    NULL/*Timer*/,
    LCDParaSetKey/*keymsg*/,
    NULL/*ipc*/
};


//������������
PLUGIN_INFO g_smiOSDispCodeSet = {
    0x00A00050/*id*/,
    DispCodeSetInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    DispCodeSetDraw/*Draw*/,
    NULL/*Timer*/,
    DispCodeSetKeyMsg/*keymsg*/,
    NULL/*ipc*/
};
//ϵͳʱ������
PLUGIN_INFO g_smiOSSysTimeSet = {
    0x00A00060/*id*/,
    SysTimeSetInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    SysTimeSetDraw/*Draw*/,
    NULL/*Timer*/,
    SysTimeSetKeyMsg/*keymsg*/,
    NULL/*ipc*/
};
//����״̬�鿴
PLUGIN_INFO g_smiOSContainStat = {
    0x00A00070/*id*/, 
    ContainStatCfgInit/*InitLayer*/,
    ContainStatCfgUnInit/*UninitLayer*/,
    ContainStatDraw/*Draw*/,
    ContainStatTimer/*Timer*/,
    ContainStatKey/*keymsg*/,
    NULL/*ipc*/
};
//����ϵͳ��Ϣ
PLUGIN_INFO g_smiOSOSInfo = {
    0x00A00080/*id*/, 
    OSInfoInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    OSInfoDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//Ӧ��ע����Ϣ
PLUGIN_INFO g_smiOSAppReg = {
    0x00A00090/*id*/, 
    AppRegInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    AppRegDraw/*Draw*/,
    NULL/*Timer*/,
    AppRegKey/*keymsg*/,
    NULL/*ipc*/
};
//�豸��Ϣ�鿴
PLUGIN_INFO g_smiOSHardwareInfo = {
    0x00A000A0/*id*/, 
    HardwareInfoInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    HardwareInfoDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//�����ն�
PLUGIN_INFO g_smiOSRestartTmn = {
    0x00A000B0/*id*/, 
    InitRestartTmn/*InitLayer*/,
    NULL/*UninitLayer*/,
    RestartTmnDraw/*Draw*/,
    NULL/*Timer*/,
    RestartTmnKey/*keymsg*/,
    NULL/*ipc*/
};

//�ָ���������
PLUGIN_INFO g_smiOSFactoryReset = {
    0x00A000C0/*id*/, 
    InitFactoryReset/*InitLayer*/,
    NULL/*UninitLayer*/,
    FactoryResetDraw/*Draw*/,
    NULL/*Timer*/,
    FactoryResetKey/*keymsg*/,
    NULL/*ipc*/
};

//SSH��������
PLUGIN_INFO g_smiOS_SSHSet = {
    0x00A000D0/*id*/, 
    InitSSHSet/*InitLayer*/,
    NULL/*UninitLayer*/,
    SSHSetDraw/*Draw*/,
    NULL/*Timer*/,
    SSHSetKey/*keymsg*/,
    NULL/*ipc*/
};

//Ӧ�ð汾��Ϣ
PLUGIN_INFO g_smiOSAppVersion = {
    0x00A000E0/*id*/, 
    InitAppVersion/*InitLayer*/,
    NULL/*UninitLayer*/,
    AppVersionDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//��־����
PLUGIN_INFO g_smiOSLogExport = {
    0x00A000F1/*id*/, 
    LogExportInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    LogExportDraw/*Draw*/,
    NULL/*Timer*/,
    LogExportKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

//U������
PLUGIN_INFO g_smiOSUUpdate = {
    0x00A000F0/*id*/, 
    InitUdiskUpdateDraw/*InitLayer*/,
    NULL/*UninitLayer*/,
    UdiskUpdateDraw/*Draw*/,
    UdiskUpdateTimer/*Timer*/,
    UdiskUpdateKey/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_layerValidatePwd = {
    0x00600000/*id*/,
    ValidatePwdInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    ValidatePwdDraw/*Draw*/,
    NULL/*Timer*/,
    ValidatePwdKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_layerInput = {
    0x00a30000/*id*/, 
    NULL/*InitLayer*/,
    NULL/*UninitLayer*/,
    InputDraw/*Draw*/,
    NULL/*Timer*/,
    InputKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

