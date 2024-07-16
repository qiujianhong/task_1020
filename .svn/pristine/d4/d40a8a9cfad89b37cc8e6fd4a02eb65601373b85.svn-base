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
#include "OopAll.h"
#include "desk_meter.h"
extern void CancelMenu();
int64 unlawPower = 0xeeeeeeeeeeeeeeee;
uint8 safe_status = 0;      //安全模式： 0——关闭，1——开启，默认关闭
uint8 mac_status = 1;       //MAC校验： 0——关闭，1——开启，默认开启
uint8 hplc_mac_status = 0;      //载波MAC校验开关 默认关闭
uint8 g_Relay485Flag = 0;   //是否支持转发485串口报文: 1-enable 0-disable
uint8 g_MeterreadFlag = 0;  //是否支持抄表补抄 0-关闭，1-开启
int nCurPoint;
int safe_index = 0;
uint8 router_state =1; //路由学习状态： 0——关闭，1——开启，默认开启
const char *g_safeModel_list[] = {
    "关闭",
    "启用",
};
const char *g_macModel_list[] = {
    "关闭",
    "启用",
};

const char *g_ac_485_list[] = {
    "关闭",
    "启用",
};
const char *g_routerstudy_list[] = {
    "关闭",
    "启用",
};
const char *g_meterread_list[] = {
    "关闭",
    "启用",
};

int set_ini_key_value(const char * ininame, const char * key, uint8 notfound)
{
    char str_value[8] = {0};
    dictionary *ini = NULL;
    ini = iniparser_load(ininame);
    printf("打开配置文件%s \n", ininame);
    if(ini!=NULL)
    {
        sprintf(str_value, "%hu", notfound);
        dictionary_set(ini, key, str_value);
        iniparser_freedict(ini);
    }

    return 0;
}

/**********************************************************************
* @name      : get_line_by_key
* @brief     ：找到关键字所在行的位置和长度
* @param[in] ：FILE *pf            文件句柄
               const char *key    关键字

* @param[out]：long *poffset 关键字所在行起始位置
* @return    ：>0长度, <0错误
* @Create    :
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
int get_line_by_key(FILE *pf/*in*/, const char *key/*in*/, long *poffset/*out*/)
{
    char line[1024] = {0};
    long offset = 0;
    int line_len = 0;

    if(pf == NULL){
        return -1;
    }

    while(fgets(line,1024,pf) != NULL)
    {
        if(strstr(line, key) != NULL)
        {
            offset = ftell(pf); // 记录文件指针位置, 因为后面还要读, 文件指针会移走
            line_len = strlen(line);
            *poffset = offset;
            printf("查找文件配置长度%d %s  \n", line_len, line);
            return line_len;
        }
        
    }
    return -1;
}

int write_cfg_by_key(const char *filename/*in*/, const char *key/*in*/, uint8 value/*in*/)
{
    
    FILE *fp;
    char buf[128]; // 假设每行不超过1024字节, 根据情况调节大小
    long offset;
    
    if (!(fp = fopen(filename, "r+"))) 
    { // 尝试以读写方式打开文件.
        printf("打开文件配置文件%s 失败 \n", filename );
        return -1;
    }

    fseek(fp,0,SEEK_END);
    long lSize = ftell(fp);
    // 用完后需要将内存free掉
    char *str =(char*)malloc(lSize+1);
    rewind(fp); 
    fread(str,sizeof(char),lSize,fp);
    str[lSize] = '\0';

    fseek(fp,0,SEEK_SET);
    int len = get_line_by_key(fp , key, &offset);
    if (len < 0) 
    {
        free(str);
        fclose(fp);
        printf("查找文件配置文件%s 失败 \n", key );
        return -1;
    }

    offset -= len;
    fseek(fp, offset, SEEK_SET);
    sprintf(buf, "%s=%hu", key, value);
    fwrite(buf, strlen(buf), 1, fp);
    fwrite(str+offset+len, strlen(str+offset+len), 1, fp);

    free(str);
    fclose(fp);
    return 0;
}

int write_safe_cfg(const char *filename/*in*/)
{
    
    FILE *fp;
    char buf[128]; // 假设每行不超过1024字节, 根据情况调节大小
 
    if (!(fp = fopen(filename, "w"))) 
    { // 尝试以写方式打开文件.
        printf("打开文件配置文件%s 失败 \n", filename );
        return -1;
    }

    sprintf(buf, "[SAFE_MODE]\n");
    fwrite(buf, strlen(buf), 1, fp);

    
//    sprintf(buf, "SAFE_BUTTON=%hu\n", safe_status);
//    fwrite(buf, strlen(buf), 1, fp);

    sprintf(buf, "MAC_MODE=%hu\n", mac_status);
    fwrite(buf, strlen(buf), 1, fp);
    sprintf(buf, "HPLC_MAC_MODE=%hu\n", hplc_mac_status);
    fwrite(buf, strlen(buf), 1, fp);

    sprintf(buf, "AC485FLAG=%hu\n", g_Relay485Flag);
    fwrite(buf, strlen(buf), 1, fp);
    sprintf(buf, "ROUTER_STATE_STUDY=%hu\n", router_state);
    fwrite(buf, strlen(buf), 1, fp);
    sprintf(buf, "RS485PATCH=%hu\n", g_MeterreadFlag);
    fwrite(buf, strlen(buf), 1, fp);

    fclose(fp);
    return 0;
}

OOP_SECURITYFLAG_E g_safe_mode;

int write_safemode()
{
    int ret;
    NOMAL_OAD_T NomalOad = { 0 };
    NomalOad.logicId = 0;
    NomalOad.oad.value = 0xF1010200;
#if defined AREA_HUNAN || defined AREA_HEBEI || defined AREA_JIANGXI
	NomalOad.classtag = 1;
#else
	NomalOad.classtag = 2;
#endif

    NomalOad.infoNum = 0;

    if (safe_status == 0) 
    {
        g_safe_mode = SECURITY_OFF;
    }
    else 
    {
        g_safe_mode = SECURITY_ON;
    }
    
    ret = db_write_nomal(g_hUdbCenter, &NomalOad, (uint8*)&g_safe_mode, sizeof(OOP_SECURITYFLAG_E));
    if (ret != 0)
    {
        GUI_FMT_DEBUG("设置安全模式设备 0xF1010200 ret=%d \n", ret);
    }
    return ret;
}

/* 从低压集抄获取默认参数 */
void gui_get_default_gprs(OOP_GPRSCONFIG_T *ptCprs)
{
    read_pdata_xram("/data/app/puAmr/cfg/gprs_default", (char *)ptCprs, 0, sizeof(OOP_GPRSCONFIG_T));
}

void setSafeONOFFInit(DWORD dwPluginId)
{
    g_guiComm.bCanTrunDisp = FALSE;

    dictionary *ini = NULL;
    ini = iniparser_load("/data/app/desktopGui/safemode.ini");
    GUI_FMT_DEBUG("读取配置文件/data/app/desktopGui/safemode.ini \n");
    if(ini!=NULL)
    {
//        safe_status= iniparser_getint(ini,"SAFE_MODE:SAFE_BUTTON",0);
        mac_status= iniparser_getint(ini,"SAFE_MODE:MAC_MODE",1);
        hplc_mac_status= iniparser_getint(ini,"SAFE_MODE:HPLC_MAC_MODE",0);
        g_Relay485Flag= iniparser_getint(ini,"SAFE_MODE:AC485FLAG",0);
        router_state= iniparser_getint(ini,"SAFE_MODE:ROUTER_STATE_STUDY",1);
        g_MeterreadFlag= iniparser_getint(ini,"SAFE_MODE:RS485PATCH",0);
        iniparser_freedict(ini);
    }
/*    
    ini = iniparser_load("/data/app/taskManager/taskManager.ini");
    printf("读取配置文件/data/app/taskManager/taskManager.ini \n");
    if(ini!=NULL)
    {
        mac_status= iniparser_getint(ini,"SAFE_MODE:SAFE_BUTTON",1);
        iniparser_freedict(ini);
    }

    ini = iniparser_load("/usr/local/extapps/acMeter/bin/acMeter.ini");
    printf("读取配置文件/usr/local/extapps/acMeter/bin/acMeter.ini \n");
    if(ini!=NULL)
    {
        g_Relay485Flag= iniparser_getint(ini,"RELAY_CONF:AC485FLAG",0);
        iniparser_freedict(ini);
    }   
*/
    int ret = 0;
    uint32 len = 0;
    NOMAL_OAD_T nomalOAD = { 0 };
    nomalOAD.logicId = 0;
    nomalOAD.infoNum = 0;
    nomalOAD.oad.value = 0xF1010200;
    g_safe_mode = SECURITY_OFF;
    ret = db_read_nomal(g_hUdbCenter, &nomalOAD, sizeof(OOP_SECURITYFLAG_E), (uint8*)&g_safe_mode, &len);
    if (ret != ERR_OK || len != sizeof(OOP_SECURITYFLAG_E))
    {
        GUI_FMT_DEBUG("db_read_nomal 0xF1010200 err ret=%d, len=%u!\n", ret, len);
    }

    if (g_safe_mode == SECURITY_OFF) 
    {
        safe_status = 0;
    }
    else 
    {
        safe_status = 1;
    }
    GUI_FMT_DEBUG("ret=%d, len=%u!\n", ret, len);
    safe_index = 0;
}

void setSafeONOFFDraw1(HDC hdc)
{
    //int       nTop = 0;
    char      czbuf[32];
    RECT      rc;
    //uint8     i = 0;

    SelectFont(hdc,g_guiComm.fontSmall);

    //标题显示
    ITEM_NOT_SELECT;
    rc.left  = 15;
    rc.right = g_guiComm.nWidth;
    rc.top   = 20;
    rc.bottom = rc.top+16;
    sprintf(czbuf, "安全模式:");//用采加密开关，配电加密开关
    DrawText(hdc, czbuf, strlen(czbuf), &rc, DT_VCENTER);

    rc.left  = 100;
    rc.right = g_guiComm.nWidth;
    //rc.top   += 20;
    rc.bottom = rc.top+16;

    if(safe_index == 0)
     {
         ITEM_SELECT;
         TextOut(hdc, rc.left, rc.top, g_safeModel_list[safe_status]);
         ITEM_NOT_SELECT;
     }
     else
     {
         TextOut(hdc, rc.left, rc.top, g_safeModel_list[safe_status]);
     }

    ITEM_NOT_SELECT;
    rc.left  = 15;
    rc.right = g_guiComm.nWidth;
    rc.top   += 20;
    rc.bottom = rc.top+16;
    sprintf(czbuf, "MAC校验:");
    DrawText(hdc, czbuf, strlen(czbuf), &rc, DT_VCENTER);

    rc.left  = 100;
    rc.right = g_guiComm.nWidth;
    //rc.top   += 20;
    rc.bottom = rc.top+16;

    if(safe_index == 1)
    {
     ITEM_SELECT;
     TextOut(hdc, rc.left, rc.top, g_macModel_list[mac_status]);
     ITEM_NOT_SELECT;
    }
    else
    {
     TextOut(hdc, rc.left, rc.top, g_macModel_list[mac_status]);
    }
    
    ITEM_NOT_SELECT;
    rc.left  = 15;
    rc.right = g_guiComm.nWidth;
    rc.top   += 20;
    rc.bottom = rc.top+16;
    sprintf(czbuf, "载波MAC校验:");
    DrawText(hdc, czbuf, strlen(czbuf), &rc, DT_VCENTER);

    rc.left  = 100;
    rc.right = g_guiComm.nWidth;
    //rc.top   += 20;
    rc.bottom = rc.top+16;

    if(safe_index == 2)
    {
         ITEM_SELECT;
         TextOut(hdc, rc.left, rc.top, g_macModel_list[hplc_mac_status]);
         ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, rc.left, rc.top, g_macModel_list[hplc_mac_status]);
    }


    ITEM_NOT_SELECT;
    rc.left  = 15;
    rc.right = g_guiComm.nWidth;
    rc.top   += 20;
    rc.bottom = rc.top+16;
    sprintf(czbuf, "交采485通道:");
    DrawText(hdc, czbuf, strlen(czbuf), &rc, DT_VCENTER);

    rc.left  = 100;
    rc.right = g_guiComm.nWidth;
    //rc.top   += 20;
    rc.bottom = rc.top+16;

    if(safe_index == 3)
    {
         ITEM_SELECT;
         TextOut(hdc, rc.left, rc.top, g_ac_485_list[g_Relay485Flag]);
         ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, rc.left, rc.top, g_ac_485_list[g_Relay485Flag]);
    }
    ITEM_NOT_SELECT;
    rc.left  = 15;
    rc.right = g_guiComm.nWidth;
    rc.top   += 20;
    rc.bottom = rc.top+16;
    sprintf(czbuf, "路由学习:");
    DrawText(hdc, czbuf, strlen(czbuf), &rc, DT_VCENTER);

    rc.left  = 100;
    rc.right = g_guiComm.nWidth;
    //rc.top   += 20;
    rc.bottom = rc.top+16;

    if(safe_index == 4)
    {
     ITEM_SELECT;
     TextOut(hdc, rc.left, rc.top, g_routerstudy_list[router_state]);
     ITEM_NOT_SELECT;
    }
    else
    {
     TextOut(hdc, rc.left, rc.top, g_routerstudy_list[router_state]);
    }

    if(safe_index == 5)
    {
        ITEM_SELECT;
        TextOut(hdc, 20, 118, "下一页");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 20, 118, "下一页");
    }
    
     if(safe_index == 6)
     {
         ITEM_SELECT;
         TextOut(hdc, 100, 118, "确定");
         ITEM_NOT_SELECT;
     }
     else
     {
         TextOut(hdc, 100, 118, "确定");
     }

     PopListDraw(hdc);
}

 void setSafeONOFFDraw2(HDC hdc)
 {
     char      czbuf[32];
     RECT      rc;
 
     SelectFont(hdc,g_guiComm.fontSmall);
 
     //标题显示
     ITEM_NOT_SELECT;
     rc.left  = 15;
     rc.right = g_guiComm.nWidth;
     rc.top   = 20;
     rc.bottom = rc.top+16;
     sprintf(czbuf, "抄表补抄:");
     DrawText(hdc, czbuf, strlen(czbuf), &rc, DT_VCENTER);
 
     rc.left  = 100;
     rc.right = g_guiComm.nWidth;
     rc.bottom = rc.top+16;
 
     if(safe_index == 7)
      {
          ITEM_SELECT;
          TextOut(hdc, rc.left, rc.top, g_meterread_list[g_MeterreadFlag]);
          ITEM_NOT_SELECT;
      }
      else
      {
          TextOut(hdc, rc.left, rc.top, g_meterread_list[g_MeterreadFlag]);
      }
 
 
     if(safe_index == 8)
     {
         ITEM_SELECT;
         TextOut(hdc, 20, 118, "上一页");
         ITEM_NOT_SELECT;
     }
     else
     {
         TextOut(hdc, 20, 118, "上一页");
     }
     
    if(safe_index == 9)
    {
      ITEM_SELECT;
      TextOut(hdc, 100, 118, "确定");
      ITEM_NOT_SELECT;
    }
    else
    {
      TextOut(hdc, 100, 118, "确定");
    }

    PopListDraw(hdc);
 }

 void setSafeONOFFDraw(HDC hdc)
 {
    if(safe_index < 7)
    {
        return setSafeONOFFDraw1(hdc);
    }
    else
    {
        return setSafeONOFFDraw2(hdc);
    }
 }

 BOOL setSafeONOFFMsg(int nKey)
 {
     //uint8     pRD;
     //uint8     pRD1 = 0x55;

     int ret = 0;
     // 弹出List选择处理
     if(g_guiComm.bPopList == TRUE)
     {
         int nSel;
         PopListKeyMsg(nKey, &nSel);
         if(nKey == KEY_OK)
         {
             if(safe_index == 0)
                safe_status = nSel;
             else if(safe_index == 1)
                mac_status = nSel;
             else if(safe_index == 2)
                hplc_mac_status = nSel;
             else if(safe_index == 3)
                g_Relay485Flag = nSel;
             else if(safe_index == 4)
                router_state = nSel;
             else if(safe_index == 7)
                g_MeterreadFlag = nSel;
             UpdateWindow(g_guiComm.hMainWnd, TRUE);
         }
         return TRUE;
     }

     switch(nKey)
     {
     case KEY_UP:
         safe_index--;
         if(safe_index < 0)
             safe_index = 9;
         UpdateWindow(g_guiComm.hMainWnd, TRUE);
         return TRUE;

     case KEY_DOWN:
         safe_index ++;
         if(safe_index > 9)
             safe_index = 0;
         UpdateWindow(g_guiComm.hMainWnd, TRUE);
         return TRUE;

     case KEY_OK:
         if(ValidatePwd() == TRUE)
         {// 确认处理

            if (safe_index == 5)
            {
                safe_index = 7;
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                return TRUE;
            }
            else if(safe_index == 8)
            {
                safe_index = 0;
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                return TRUE;
            }
            
            switch(safe_index)
            {
                case 0:
                    PopList(g_safeModel_list, 2, g_safeModel_list[safe_status], 120, 30, FALSE);
                    break;
                case 1:
                    PopList(g_macModel_list, 2, g_macModel_list[mac_status], 120, 50, FALSE);
                    break;
                case 2:
                    PopList(g_macModel_list, 2, g_macModel_list[hplc_mac_status], 120, 70, FALSE);
                    break;
                case 3:
                    PopList(g_ac_485_list, 2, g_ac_485_list[g_Relay485Flag], 120, 90, FALSE);
                    break;
                case 4:
                    PopList(g_routerstudy_list, 2, g_routerstudy_list[router_state], 120, 110, FALSE);
                    break;
                case 7:
					PopList(g_meterread_list, 2, g_meterread_list[g_MeterreadFlag], 120, 30, FALSE);
					break;
             	case 6:
                case 9:
                    write_safe_cfg("/data/app/desktopGui/safemode.ini");
                    ret = write_safemode();
//                    write_cfg_by_key("/data/app/desktopGui/safemode.ini", "SAFE_BUTTON", safe_status);
//                    write_cfg_by_key("/data/app/taskManager/taskManager.ini", "SAFE_BUTTON", mac_status);
//                    write_cfg_by_key("/usr/local/extapps/acMeter/bin/acMeter.ini", "AC485FLAG", g_Relay485Flag);

                    UpdateWindow(g_guiComm.hMainWnd, TRUE);
                    g_bpop = TRUE;
                    if(ret == 0)
                    {
                        PopMsg("       设置成功!\n     正在重启应用...");
                        RebootTerminal();
                    }
                    else
                    {
                        MessageBox("提示\n\n设置安全模式失败", MB_OK);
                        g_bpop = FALSE;

//                        PopMsg("       设置安全模式失败!");
                    }
                    
                    break;
                default:
                    break;
             }
             return TRUE;
         }
     }

     return FALSE;
}


PLUGIN_INFO g_layersetSafeONOFF=
{
    0x000c0003, // 0x000c0003
    setSafeONOFFInit,
    NULL,
    setSafeONOFFDraw,
    NULL,
    setSafeONOFFMsg,
    NULL
};
#ifndef AREA_CHONGQING
uint8 sshsetindex;
uint8 g_sshopen;
void InitSSHSet(DWORD dwPluginId)
{
    char resultBuf[160] = {0};
    FILE* fp = NULL;
    g_sshopen = FALSE;

    if ((fp = popen("ps |grep \"/usr/sbin/sshd\"|grep -v grep|awk '{print $1}'", "r")) != NULL)
    {
        usleep(100000);
        fgets(resultBuf, sizeof(resultBuf), fp);
        if(0 != resultBuf[0])
        {
            g_sshopen = TRUE;
        }
        pclose(fp);
        fp = NULL;
    }
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
    int nLeft = 26;
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

    nLeft = 110;
    if(sshsetindex == 1)
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

/**
*********************************************************************
* @brief：      SSH服务授权操作
* @param[in]： STA_PORT_AUTHORIZE_E status 服务授权状态
* @return：     void
*********************************************************************
*/
int sta_ssh_service_authorize_operate()
{
    int    ret = 0;
    uint8  sendbuf[100] = {0};
    uint16 offset = 0;

    sendbuf[offset++] = 0x01;
    sendbuf[offset++] = 0x01;
    sendbuf[offset++] = 0xff;
    sendbuf[offset++] = 0xff;
    sendbuf[offset++] = 0xff;
    sendbuf[offset++] = 0xff;
    ret = gui_send_smios(0, SYSTEM_IOP_SERVICE_START, sendbuf, offset);
    if ((ret != ERR_OK) )
    {
       GUI_FMT_DEBUG("Reboot_set_to_smiOS err! ret = %d\n", ret);
    }
    else
    {
        GUI_FMT_DEBUG("Reboot_set_to_smiOS success! ret = %d\n", ret);
    }

    return ret;
}

//choice 0 开启ssh 1 关闭ssh
void sshset(uint8 choice)
{
    char cmd[100]={0};
    if(choice ==0 && FALSE == g_sshopen)
    {
        sprintf(cmd,"/usr/sbin/sshd -D &");
        sta_ssh_service_authorize_operate();
    }
    else if(choice ==1 && TRUE == g_sshopen)
    {
        sprintf(cmd,"killall sshd");
    }
    if(0 != cmd[0])
    {
        printf("sshset %s \n",cmd);
        system(cmd);
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

                    if(FALSE == MessageBox("提示\n是否关闭SSH服务\n", MB_OKCANCEL))
                    {
                        return TRUE;
                    }
                    sshset(1);
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
extern BOOL CanUseUDisks();
extern void ModuleDeleteDir(const char *dir);

uint8 g_tcpdump_set_index;  /* 0-抓FE0; 1-抓lo; 2-抓docker0; 3-全抓; 4-关闭; 5-导出U盘; 6-返回并删除抓包目录 */
uint8 g_tcpdump_status;     /* 0-未开启; 1-已开启; 2-已关闭; 3-正在拷贝文件; 4-拷贝文件成功; 5-创建目录失败 */

#define TCPDUMP_FILE_DIR "/tmp/tcpdump"
#define TCPDUMP_SINGLE_FILE_SIZE 100     /* 单个文件大小，单位MB */


pthread_t    g_TCPDumpCaptureThreadId;  /* tcpdump抓包线程ID */
pthread_t    g_TCPDumpCopyThreadId;     /* tcpdump拷贝线程ID */

void *TCPDumpCaptureThread(void *arg)
{
    char *interface = (char *)arg;
    char cmd[128];

    sprintf(cmd, "tcpdump -i %s -C %u -w %s/tcpdump.cap", interface, TCPDUMP_SINGLE_FILE_SIZE, TCPDUMP_FILE_DIR);
    system(cmd);
    /* 线程退出之前自己刷线程id */
    g_TCPDumpCaptureThreadId = 0;
    return NULL;
}

void InitTCPDumpSet(DWORD dwPluginId)
{
    g_tcpdump_set_index = 0;
    g_tcpdump_status = 0;
    g_TCPDumpCaptureThreadId = 0;
    g_TCPDumpCopyThreadId = 0;
}

void TCPDumpSetDraw(HDC hdc)
{
    char buf[256]={0};
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    SelectFont(hdc,pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 40;
    rc.bottom = rc.top+16;
    char pcz[20];
    strcpy(pcz, "TCPDUMP服务设置");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);

    char *fileStatus[] = {(char*)"未开启", (char*)"已开启", (char*)"已关闭",
                          (char*)"正在拷贝文件",       (char*)"拷贝文件成功", (char*)"创建目录失败"};

    int nTop = 75;
    int nLeft = 16;

    TextOut(hdc, nLeft, nTop, "状态");
    TextOut(hdc, nLeft + 35, nTop, fileStatus[g_tcpdump_status]);

    if (g_tcpdump_status == 3)
    {
        nTop = 100;
        nLeft = 10;
        TextOut(hdc, nLeft, nTop, "拷贝文件中...");
        nTop = 115;
        nLeft = 10;
        sprintf(buf, "抓包文件共 %ld MB", GetFilesSizeRecu(TCPDUMP_FILE_DIR) / (1024 * 1024));
        TextOut(hdc, nLeft, nTop, buf);
        nTop = 130;
        nLeft = 10;
        TextOut(hdc, nLeft, nTop, "请勿操作终端或者拔出U盘");

        return;
    }

    nTop = 100;
    nLeft = 10;
    if(g_tcpdump_set_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "抓FE0");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "抓FE0");
    }

    nLeft = 55;
    if(g_tcpdump_set_index == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "抓lo");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "抓lo");
    }

    nLeft = 100;
    if(g_tcpdump_set_index == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "抓docker0");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "抓docker0");
    }

    nTop = 115;
    nLeft = 10;
    if(g_tcpdump_set_index == 3)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "全抓");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "全抓");
    }

    nLeft = 55;
    if(g_tcpdump_set_index == 4)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "关闭");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "关闭");
    }

    nLeft = 100;
    if(g_tcpdump_set_index == 5)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "导出U盘");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "导出U盘");
    }

    nTop = 130;
    nLeft = 10;
    if(g_tcpdump_set_index == 6)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "返回并删除");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "返回并删除");
    }
}

void TCPDumpStop()
{
    char cmd[100]={0};

    sprintf(cmd,"ps aux | grep tcpdump | awk '{print $2}' | xargs kill -INT");
    system(cmd);

    if (0 != g_TCPDumpCaptureThreadId)
    {
        pthread_cancel(g_TCPDumpCaptureThreadId);
        g_TCPDumpCaptureThreadId = 0;
    }

    g_tcpdump_status = 2;
    printf("g_TCPDumpCaptureThreadId %lu \n", g_TCPDumpCaptureThreadId);
    return;
}

/* 获取指定路径可用磁盘空间 */
unsigned int GetAvailableDiskMB(const char *dir)
{
    struct statfs diskInfo;
    unsigned int availableDiskMB = 0;

    statfs(dir, &diskInfo);  
    unsigned long long blockSize = diskInfo.f_bsize;    //每个block里包含的字节数  
    unsigned long long availableDisk = diskInfo.f_bavail * blockSize;   //可用空间大小Byte  
    availableDiskMB = availableDisk >> 20;

    return availableDiskMB;
}

#define AVAILABLE_DISK_SIZE_MB_MIN 100
unsigned int TCPDumpDiskVolumeEnough()
{
    unsigned int availableDiskMB = GetAvailableDiskMB(TCPDUMP_FILE_DIR);

    if (availableDiskMB < AVAILABLE_DISK_SIZE_MB_MIN)
    {
        return 1;
    }

    return 0;  
}

void TCPDumpDeleteOldestFile()
{
    char cmd[256];

    sprintf(cmd, "cd %s && ls -ltar | grep cap | head -1 | awk '{print $9}' | xargs rm -rf", TCPDUMP_FILE_DIR);
    system(cmd);
}

void *TCPDumpCopyThread(void *arg)
{
    char cmd[128]={0};
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return NULL;
    }

    sprintf(cmd, "cp -rfP %s /tmp/udisk/", TCPDUMP_FILE_DIR);
    system(cmd);
    system("sync");

    printf("copythread 操作完成 退出\n");
    g_tcpdump_status = 4;
    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    ModuleDeleteDir(TCPDUMP_FILE_DIR);
    system("umount /tmp/udisk");
    g_TCPDumpCopyThreadId = 0;
    return NULL;
}

void TCPDumpUpdateTimer(int nId)
{   
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    /* 1s定时器检查 */
    if (nId != TIMER_MAIN)
    {
        return;
    }

    g_guiMain.nTrunDispTime = 0; /* 在TCPDump抓包页面，不进入轮显 */
    g_guiMain.nLightonTime = 0;
    g_guiMain.bLighton = TRUE;
    lighton_lcd(); /* 在TCPDump抓包页面，屏幕常亮 */

    if (g_tcpdump_status == 1 && TCPDumpDiskVolumeEnough() != 0)
    {
        //TCPDumpStop();
        TCPDumpDeleteOldestFile();
        //MessageBox("磁盘空间不足!");
    }

    return;
}

unsigned int TCPDumpCaptureProc(uint8 index)
{
    char *interface[4] = {(char*)"FE0", (char*)"lo", (char*)"docker0", (char*)"any"};

    ModuleDeleteDir(TCPDUMP_FILE_DIR);

    if (0 != access(TCPDUMP_FILE_DIR, F_OK))
    {
        if(0 != mkdir(TCPDUMP_FILE_DIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
        {
            g_tcpdump_status = 5;
            return 1;
        }
    }

    if (index < 4)
    {
        if (pthread_create(&g_TCPDumpCaptureThreadId, NULL, TCPDumpCaptureThread, interface[index]) != 0)
        {
            g_TCPDumpCaptureThreadId = 0;
            MessageBox("系统忙\n稍后再抓包!");
            return 1;
        }

        g_tcpdump_status = 1;
        return 0;
    }

    return 1;
}

BOOL TCPDumpSetKey(int nKey)
{
    uint8 oldStatus = 0;
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return FALSE;
    }

    /* 拷贝文件时不能操作菜单 */
    if (g_tcpdump_status == 3)
    {
        return TRUE;
    }

    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_tcpdump_set_index > 0)
            {
                g_tcpdump_set_index--;
            }
            else
            {
                g_tcpdump_set_index = 6;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_tcpdump_set_index < 6)
            {
                g_tcpdump_set_index++;
            }
            else
            {
                g_tcpdump_set_index = 0;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
            if (ValidatePwd() == TRUE)
            {
                g_bpop = FALSE;
                switch(g_tcpdump_set_index)
                {
                
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    {
                        if (g_TCPDumpCaptureThreadId != 0)
                        {
                            MessageBox("提示\ntcpdump正在抓包!\n请先停止抓包\n");
                            return TRUE;
                        }

                        if (FALSE == MessageBox("提示\n是否开启tcpdump抓包服务\n", MB_OKCANCEL))
                        {
                            return TRUE;
                        }

                        TCPDumpCaptureProc(g_tcpdump_set_index);
                        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                        return TRUE;
                    }
                    case 4:
                    {
                        if (FALSE == MessageBox("提示\n是否关闭tcpdump抓包服务\n", MB_OKCANCEL))
                        {
                            return TRUE;
                        }

                        TCPDumpStop();
                        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                        return TRUE;
                    }
                    case 5:
                    {
                        if (access(TCPDUMP_FILE_DIR, F_OK) != 0)
                        {
                            MessageBox("提示\n未检测到抓包文件!\n请先抓包\n");
                            return TRUE;
                        }

                        if (g_TCPDumpCaptureThreadId != 0)
                        {
                            MessageBox("提示\ntcpdump正在抓包!\n请先停止抓包\n");
                            return TRUE;
                        }

                        if (FALSE == CanUseUDisks())
                        {
                            MessageBox("U盘没有插入\n\n不能拷贝文件!");
                            return TRUE;
                        }

                        oldStatus = g_tcpdump_status;
                        g_tcpdump_status = 3;
                        UpdateWindow(pg_guicomm->hMainWnd, TRUE);

                        if (pthread_create(&g_TCPDumpCopyThreadId, NULL, TCPDumpCopyThread, NULL) != 0)
                        {
                            g_TCPDumpCopyThreadId = 0;
                            g_tcpdump_status = oldStatus;
                            MessageBox("系统忙\n稍后再拷贝!");
                            return TRUE;
                        }

                        return TRUE;
                    }
                    case 6:
                    {
                        if (FALSE == MessageBox("提示\n是否返回并删除\n终端内的抓包文件\n", MB_OKCANCEL))
                        {
                            return TRUE;
                        }

                        ModuleDeleteDir(TCPDUMP_FILE_DIR);
                        CancelMenu();
                        return TRUE;
                    }
                }
            }

            return TRUE;
    }

    return FALSE;
}

//TCPDump服务设置
PLUGIN_INFO g_TCPDump_Set_Layer = {
    0x00A000D1,         /*id*/
    InitTCPDumpSet,     /*InitLayer*/
    NULL,               /*UninitLayer*/
    TCPDumpSetDraw,     /*Draw*/
    TCPDumpUpdateTimer, /*Timer*/
    TCPDumpSetKey,      /*keymsg*/
    NULL                /*ipc*/
};

uint8 passwdsetindex;
char g_curtmnPasswd[64] = "Zgdky@guest123";

void passwdchg(char* pNewStr, void* pPara)
{
    memset(g_curtmnPasswd, '\0', strlen(g_curtmnPasswd));
    memcpy(g_curtmnPasswd, pNewStr, strlen(pNewStr));
}

void passwdset(void)
{
    char cmd[100]={0};

    sprintf(cmd,"echo sysadm:%s|chpasswd",g_curtmnPasswd);
    system(cmd);
}

void InitPasswdSet(DWORD dwPluginId)
{
    passwdsetindex = 2;
    g_bpop = TRUE;
}

void UninitPasswdSet()
{
    g_bpop = FALSE;
}


void PasswdSetDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if(pg_guicomm == NULL)
        return;

    SelectFont(hdc,pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 36;
    rc.bottom = rc.top+16;
    const char* pcz = "sysadm密码";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);

    rc.top = 66;
    rc.bottom = rc.top+16;

    if(passwdsetindex == 0)
    {
        ITEM_SELECT;
        DrawText(hdc, g_curtmnPasswd, strlen(g_curtmnPasswd), &rc, DT_CENTER|DT_VCENTER);
        ITEM_NOT_SELECT;
    }
    else
    {
        DrawText(hdc, g_curtmnPasswd, strlen(g_curtmnPasswd), &rc, DT_CENTER|DT_VCENTER);
    }

     SelectFont(hdc,pg_guicomm->fontBig);
     int nTop = 98;
     int nLeft = 26;
    if(passwdsetindex == 1)
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
    if(passwdsetindex == 2)
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

BOOL PasswdSetKey(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if(pg_guicomm == NULL)
        return FALSE;

    switch(nKey)
    {
    case KEY_LEFT:
    case KEY_UP:
        if(passwdsetindex>0)
            passwdsetindex --;
        else
            passwdsetindex = 2;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
    case KEY_DOWN:
        if(passwdsetindex<2)
            passwdsetindex++;
        else
            passwdsetindex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch(passwdsetindex)
        {
            case 0:
                InputBox(g_curtmnPasswd, passwdchg, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return FALSE;

                
            case 1:
                CancelMenu();
                break;

            case 2:
                if(ValidatePwd() == TRUE)
                {
                    passwdset();
                    MessageBox("密码修改成功！");
                    CancelMenu();
                    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                }

                break;
        }
        return TRUE;
    }
    
    return FALSE;
}

//密码修改设置
PLUGIN_INFO g_passwd_Layer = {
    0x00A000F2/*id*/, 
    InitPasswdSet/*InitLayer*/,
    UninitPasswdSet/*UninitLayer*/,
    PasswdSetDraw/*Draw*/,
    NULL/*Timer*/,
    PasswdSetKey/*keymsg*/,
    NULL/*ipc*/
};

uint8 logClearindex;

void LogClear(void)
{
    char cmd[100] = {0};

    //sprintf(cmd, "find /data/app/ -name log -type d |xargs rm -rfv");
    sprintf(cmd,"find /data/app -path '/data/app/*/log/*' |xargs sudo rm -rf");
    system(cmd);
    sprintf(cmd,"find /tmp/data/app -path '/tmp/data/app/*/log/*' |xargs sudo rm -rf");
    system(cmd);
    sprintf(cmd,"find /log/app -path '/log/app/*/*' |xargs sudo rm -rf");
    system(cmd);
    sprintf(cmd,"find /run/log -path '/run/log/*/*' |xargs sudo rm -rf");
    system(cmd);
    //适配新老系统，有无sudo命令问题
    sprintf(cmd,"find /data/app -path '/data/app/*/log/*' |xargs rm -rf");
    system(cmd);
	sprintf(cmd,"find /tmp/data/app -path '/tmp/data/app/*/log/*' |xargs rm -rf");
    system(cmd);
    sprintf(cmd,"find /log/app -path '/log/app/*/*' |xargs rm -rf");
    system(cmd);
    sprintf(cmd,"find /run/log -path '/run/log/*/*' |xargs rm -rf");
    system(cmd);
}

void InitLogClear(DWORD dwPluginId)
{
    logClearindex = 1;
}

void LogClearDraw(HDC hdc)
{
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    SelectFont(hdc, pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 36;
    rc.bottom = rc.top + 16;
    const char *pcz = "日志清除";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, pg_guicomm->fontSmall);

    rc.top = 66;
    rc.bottom = rc.top + 16;

    SelectFont(hdc, pg_guicomm->fontBig);
    int nTop = 98;
    int nLeft = 26;
    if (logClearindex == 0)
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
    if (logClearindex == 1)
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

BOOL LogClearKey(int nKey)
{
    GUI_COMM *pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
    {
        return FALSE;
    }

    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
            if (logClearindex > 0)
            {
                logClearindex--;
            }
            else
            {
                logClearindex = 1;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        case KEY_RIGHT:
        case KEY_DOWN:
            if (logClearindex < 1)
            {
                logClearindex++;
            }
            else
            {
                logClearindex = 0;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        case KEY_OK:
            switch (logClearindex)
            {
                case 0:
                    CancelMenu();
                    return TRUE;

                case 1:
                    LogClear();
                    MessageBox("日志清除成功！");
                    CancelMenu();
                    return TRUE;
            }
            return TRUE;
    }
    
    return FALSE;
}

//日志清除
PLUGIN_INFO g_LogClear_Layer = {
    0x00A000F3,     /* id */
    InitLogClear,   /* InitLayer */
    NULL,           /* UninitLayer */
    LogClearDraw,   /* Draw */
    NULL,           /* Timer */
    LogClearKey,    /* keymsg */
    NULL            /* ipc */
};

static int g_SlotIndex = 0;     //选择结果: 0-升级槽位1; 1-升级槽位2; 2-升级槽位3; 3-升级槽位4; 4-升级槽位5; 5-返回
static unsigned int g_ModuleUpdateStatus = 0;    //状态    : 0-未开始升级;1-拷贝安装文件;2-创建目录失败;3-发送升级消息;
                                       //          4-申请内存失败;5-未找到U盘里的安装目录;6-缺少升级文件;7-等待模组管理器的响应消息;8-收到模组管理器的响应消息
unsigned int g_ModuleWaitTimer = 0;


#define MODULE_UPDATE_STATUS_INIT       0
#define MODULE_UPDATE_STATUS_COPY       1
#define MODULE_UPDATE_STATUS_MKDIR_ERR  2
#define MODULE_UPDATE_STATUS_SEND_MSG   3
#define MODULE_UPDATE_STATUS_MALLOC_ERR 4
#define MODULE_UPDATE_STATUS_NO_DIR     5
#define MODULE_UPDATE_STATUS_NO_BIN     6
#define MODULE_UPDATE_STATUS_WAIT_ACK   7
#define MODULE_UPDATE_STATUS_ACK        8


unsigned int ModuleCopyPkg(unsigned int slotId, char *srcFolder, char *dstFolder)
{
    char cmd[256];

    if (0 == access(dstFolder, F_OK))
    {
        sprintf(cmd, "rm -rf %s", dstFolder);
        system(cmd);
    }

    if (0 != access(dstFolder, F_OK))
    {
        if(0 != mkdir(dstFolder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
        {
            g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_MKDIR_ERR;
            return 1;
        }
    }

    if (0 != access(srcFolder, F_OK))
    {
        g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_NO_DIR;
        return 1;
    }

    sprintf(cmd, "cp -r %s/* %s/", srcFolder, dstFolder);
    system(cmd);
    return 0;
}

unsigned int ModuleCheckPkg(unsigned int slotId, char *dstFolder, char *pkgPath)
{
    DIR           *dir;
    struct dirent *ent;

    if ((dir = opendir(dstFolder)) == NULL)
    {
        g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_NO_BIN;
        return 1;
    }
    
    while ((ent = readdir(dir)) != NULL)
    {
        if ((strcmp(ent->d_name, ".") == 0) || (strcmp(ent->d_name, "..") == 0))
        {
            continue;
        }
        else if (ent->d_type == 8)    /* 8:文件 */
        {
            sprintf(pkgPath, "%s/%s", dstFolder, ent->d_name);
            g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_SEND_MSG;
            closedir(dir);
            return 0;
        }
    }

    g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_NO_BIN;
    closedir(dir);
    return 1;
}

unsigned int ModuleSendMsg(GUI_COMM *pg_guicomm, unsigned int slotId, char *pkgPath)
{
    unsigned int updateSlotId = slotId + 1; /* 槽位号从1开始编号 */
    unsigned int len = 0;
    uint8 pkgPathLen = strlen(pkgPath);
    uint32 msgLen = sizeof(unsigned int) + 1 + pkgPathLen; /* 1:pkgPathLen字段长度 */
    unsigned int offset = 0;
    int ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    char topic[256] = {0};
    const char *desktopGui = "M-desktopGui";
    const char *mapManager = "M-mapManager";

    if (g_ModuleUpdateStatus != MODULE_UPDATE_STATUS_SEND_MSG)
    {
        return 1;
    }

    UpdateWindow(pg_guicomm->hMainWnd, TRUE);

    len = sizeof(MESSAGE_INFO_T) + msgLen;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(len);

    if (NULL == pMsgInfo)
    {
        printf("[0007]%s malloc failed %d\n", __FUNCTION__, len);
        g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_MALLOC_ERR;
        return 1;
    }

    memset(pMsgInfo, 0, len);
    
    pMsgInfo->rpm = 1;
    pMsgInfo->index = gui_get_next_index();
    pMsgInfo->priority = 0;
    pMsgInfo->label = 0;
    pMsgInfo->IID = MESSAGE_IID_MODULE;
    pMsgInfo->IOP = MODULE_IOP_FILE_TRANSFER;
    
    pMsgInfo->sourlen = strlen((char *)desktopGui);
    memcpy(pMsgInfo->souraddr, desktopGui, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(mapManager);
    memcpy(pMsgInfo->destaddr, mapManager, pMsgInfo->destlen);

    pMsgInfo->msglen = msgLen;
    appmsg_memcpy_r(pMsgInfo->playload, (char *)&updateSlotId, sizeof(uint32));
    offset += sizeof(uint32);
    pMsgInfo->playload[offset] = pkgPathLen;
    offset += 1;
    memcpy(pMsgInfo->playload + offset, pkgPath, pkgPathLen);

    snprintf(topic, 256,"%s/%s", desktopGui, pMsgInfo->destaddr);
    ret = gui_send_msg(topic, pMsgInfo, 0);

    free(pMsgInfo);
    g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_WAIT_ACK;
    g_ModuleWaitTimer = 0;
    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    return ret;
}

void ModuleAckProc()
{
    g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_ACK;
}

void InitModuleUpdateDraw(DWORD dwPluginId)
{
    g_SlotIndex = 0;
    g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_INIT;
    g_ModuleWaitTimer = 0;
}

void ModuleUpdateDraw(HDC hdc)
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

    if (g_ModuleUpdateStatus == MODULE_UPDATE_STATUS_COPY)
    {
        nTop = 40;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "拷贝文件中...");
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "耗时约10秒钟");      
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "请不要关闭电源");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "不要拔掉U盘");

        return;
    }

    if (g_ModuleUpdateStatus == MODULE_UPDATE_STATUS_SEND_MSG)
    {
        nTop = 40;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "正在发送升级消息...");
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "耗时约10秒钟");
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "请不要关闭电源");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "可以拔掉U盘");

        return;
    }

    if (g_ModuleUpdateStatus == MODULE_UPDATE_STATUS_WAIT_ACK)
    {
        nTop = 40;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "等待响应消息中...");
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "耗时约10秒钟");      
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "请不要关闭电源");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "可以拔掉U盘");

        return;
    }

    nLeft = 40;
    nTop = 20;
    SelectFont(hdc, pg_guicomm->fontBig);

    if (0 == g_SlotIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "升级槽位1");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "升级槽位1");
    }

    nTop += 20;

    if (1 == g_SlotIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "升级槽位2");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "升级槽位2");
    }

    nTop += 20;

    if (2 == g_SlotIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "升级槽位3");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "升级槽位3");
    }

    nTop += 20;

    if (3 == g_SlotIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "升级槽位4");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "升级槽位4");
    }

    nTop += 20;

    if (4 == g_SlotIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "升级槽位5");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "升级槽位5");
    }

    nTop += 20;

    if (5 == g_SlotIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "返回");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "返回");
    }

    PopListDraw(hdc);
}

void ModuleUpdateTimer(int nId)
{   
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    /* 1s定时器检查 */
    if (nId != TIMER_MAIN)
    {
        return;
    }

    g_guiMain.nTrunDispTime = 0; /* 在模组升级页面，不进入轮显 */
    g_guiMain.nLightonTime = 0;
    g_guiMain.bLighton = TRUE;
    lighton_lcd(); /* 在模组升级页面，屏幕常亮 */

    switch (g_ModuleUpdateStatus)
    {
        case MODULE_UPDATE_STATUS_INIT: //未开始升级，初始页面
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case MODULE_UPDATE_STATUS_COPY: //升级文件拷贝
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case MODULE_UPDATE_STATUS_MKDIR_ERR: //创建目录失败
        {
            MessageBox("创建目录失败！");
            g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_INIT;
            break;
        }
        case MODULE_UPDATE_STATUS_SEND_MSG: //发送升级消息
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case MODULE_UPDATE_STATUS_MALLOC_ERR: //申请内存失败
        {
            MessageBox("申请内存失败！");
            g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_INIT;
            break;
        }
        case MODULE_UPDATE_STATUS_NO_DIR: //未找到U盘里的安装目录
        {
            MessageBox("未找到U盘里的安装目录\n请检查U盘是否插入");
            g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_INIT;
            break;
        }
        case MODULE_UPDATE_STATUS_NO_BIN: //缺少升级文件
        {
            MessageBox("缺少安装脚本！");
            g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_INIT;
            break;
        }
        case MODULE_UPDATE_STATUS_WAIT_ACK: //等待模组管理器的响应消息
        {
            if (g_ModuleWaitTimer <= 10)
            {
                g_ModuleWaitTimer++;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            }
            else
            {
                MessageBox("等待响应消息超时！\n请在桌面模组菜单\n查询升级结果");
                g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_INIT;
                g_ModuleWaitTimer = 0;
            }
            break;
        }
        case MODULE_UPDATE_STATUS_ACK: //收到模组管理器的响应消息
        {
            MessageBox("收到模组管理器响应消息\n请在桌面模组菜单\n查询升级结果");
            g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_INIT;
            break;
        }
        default: 
        {
            g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_INIT;
            break;
        }
    }

}

/* 入参由调用者保证合法性 */
unsigned int ModuleInstallProc(GUI_COMM *pg_guicomm, unsigned int slotId)
{
    const char *message = "警告\n升级过程中请不要断电\n是否升级";
    char *slotNameCh[] = {(char *)"槽位1", (char *)"槽位2", (char *)"槽位3", (char *)"槽位4", (char *)"槽位5"};
    char *slotNameEn[] = {(char *)"slot1", (char *)"slot2", (char *)"slot3", (char *)"slot4", (char *)"slot5"};
    char buf[200] = {0};
    char srcFolder[200] = {0};
    char dstFolder[200] = {0};
    char pkgPath[PATH_MAX_LEN + 1] = {0};

    if (ValidatePwd() == TRUE)
    {
        sprintf(buf, "%s%s", message, slotNameCh[slotId]);
        if (FALSE == MessageBox(buf, MB_OKCANCEL))
        {
            return 0;
        }
    }
    else
    {
        return 1;
    }

    g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_COPY;
    UpdateWindow(pg_guicomm->hMainWnd, TRUE);

    sprintf(srcFolder, "%s%s", "/tmp/udisk/", slotNameEn[slotId]);
    sprintf(dstFolder, "%s%s", "/tmp/", slotNameEn[slotId]);

    if (ModuleCopyPkg(slotId, srcFolder, dstFolder) != 0)
    {
        return 1;
    }

    if (ModuleCheckPkg(slotId, dstFolder, pkgPath) != 0)
    {
        return 1;
    }

    if (ModuleSendMsg(pg_guicomm, slotId, pkgPath) != 0)
    {
        return 1;
    }

    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    return 0;
}

void ModuleDeleteDir(const char *dir)
{
    char cmd[256];

    if (0 == access(dir, F_OK))
    {
        sprintf(cmd, "rm -rf %s", dir);
        system(cmd);
    }
}

BOOL ModuleUpdateKey(int nKey)
{
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return FALSE;
    }

    /* 未开始升级状态下才能操作菜单 */
    if (g_ModuleUpdateStatus != MODULE_UPDATE_STATUS_INIT)
    {
        return TRUE;
    }

    switch (nKey)
    {
        case KEY_UP:
        case KEY_LEFT:
        {
            g_SlotIndex--;

            if (g_SlotIndex < 0)
            {
                g_SlotIndex = 5;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        case KEY_RIGHT:
        {
            g_SlotIndex++;

            if (g_SlotIndex > 5)
            {
                g_SlotIndex = 0;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (g_SlotIndex)
            {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                {
                    ModuleInstallProc(pg_guicomm, g_SlotIndex);
                    return TRUE;
                }

                case 5: /* 退出 */
                {
                    ModuleDeleteDir("/tmp/slot1");
                    ModuleDeleteDir("/tmp/slot2");
                    ModuleDeleteDir("/tmp/slot3");
                    ModuleDeleteDir("/tmp/slot4");
                    ModuleDeleteDir("/tmp/slot5");
                    CancelMenu();
                    return TRUE;
                }
            }

            return TRUE;
        }
    }

    return FALSE;
}


//模组升级
PLUGIN_INFO g_ModuleUpdate = {
    0x00A000F4,             /* id */
    InitModuleUpdateDraw,   /* InitLayer */
    NULL,                   /* UninitLayer */
    ModuleUpdateDraw,       /* Draw */
    ModuleUpdateTimer,      /* Timer */
    ModuleUpdateKey,        /* keymsg */
    NULL                    /* ipc */
};

#define MAX_RESULT_LEN 4096
#define MAX_COMMAND_LEN 256
#define MAX_PAGE_NUM 20
#define ONE_PAGE_LINE_NUM 9
#define ONE_LINE_BYTE_NUM 25


char g_ResultBuf[MAX_RESULT_LEN + 1] = {0};
char g_EchoPrintBuf[MAX_PAGE_NUM][ONE_PAGE_LINE_NUM][ONE_LINE_BYTE_NUM + 1] = {0};

/**
*********************************************************************
* @brief： 命令执行函数
* @param[in]：
               cmd          待执行的命令
* @param[out]：
               result       命令执行结果
* @return： 返回值说明 success  0
                     fail    1
*********************************************************************
*/
uint32 CmdExecute(const char *cmd, char *result)
{
    //有效性检查
    if (NULL == cmd || NULL == result)
    {
        //dzlog_warn("%s, invalid param", __FUNCTION__);
        return -1;
    }
    char tmpBuf[512 + 1];
    char tmpCmd[MAX_COMMAND_LEN + 5] = {0};
    FILE* fp = NULL;
    sprintf(tmpCmd, "%s 2>&1", cmd);
    //命令执行，打开管道
    if ((fp = popen(tmpCmd, "r")) != NULL)
    {
        //获取命令执行日志
        while (fgets(tmpBuf, 512, fp) != NULL)
        {
            if (strlen(result) >= MAX_RESULT_LEN)
            {
                break;
            }

            if (strlen(result) + strlen(tmpBuf) >= MAX_RESULT_LEN)
            {
                tmpBuf[MAX_RESULT_LEN - strlen(result)] = '\0';
                strcat(result, tmpBuf);
                break;
            }

            strcat(result, tmpBuf);
        }
        //关闭管道
        pclose(fp);
        fp = NULL;
    }
    else
    {
        //dzlog_error("%s, popen %s error", __FUNCTION__, tmpCmd);
        sprintf(result, "popen failed. Cmd:%s", tmpCmd);
        return 1;
    }

    return 0;
}

uint32 g_EchoPrintPageIndex = 0;
uint32 g_EchoPrintPageNum = 0;

uint32 EchoPrintTransBuf(const char *inBuf)
{
    uint32 inBufLen = strlen(inBuf);
    uint32 offset = 0;
    uint32 i = 0;
    uint32 j = 0;
    uint32 k = 0;
    uint32 lineFullFlag = 0; /* 1:屏幕一行写满换行，下一个字符是换行符则不再换行 */

    for (i = 0; i < MAX_PAGE_NUM; i++)
    {
        for (j = 0; j < ONE_PAGE_LINE_NUM; j++)
        {
            for (k = 0; k < ONE_LINE_BYTE_NUM; k++)
            {
                if (offset < inBufLen && offset < MAX_RESULT_LEN)
                {
                    if (*(inBuf + offset) == '\n')
                    {
                        offset++;

                        if (lineFullFlag != 1)
                        {
                            break;
                        }
                    }

                    lineFullFlag = 0;
                    g_EchoPrintBuf[i][j][k] = *(inBuf + offset);
                    offset++;
                }
                else
                {
                    return 0;
                }
            }

            if (k == ONE_LINE_BYTE_NUM)
            {
                lineFullFlag = 1;
            }
        }

        if (!(offset < inBufLen && offset < MAX_RESULT_LEN))
        {
            /* inBuf已经转换完成，不用新建一页 */
            return 0;
        }

        if (g_EchoPrintPageNum < MAX_PAGE_NUM)
        {
            g_EchoPrintPageNum++;
        }
    }

    return 0;
}

void EchoPrintInit(DWORD dwPluginId)
{
    g_EchoPrintPageIndex = 0;
    g_EchoPrintPageNum = 1;
    memset(g_EchoPrintBuf, 0, sizeof(g_EchoPrintBuf));
    EchoPrintTransBuf(g_ResultBuf);
}

void EchoPrintDraw(HDC hdc)
{
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    SelectFont(hdc, pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 20;
    rc.bottom = rc.top + 13;

    char buf[100];
    sprintf(buf, "第%u页/共%u页 方向键翻页", g_EchoPrintPageIndex + 1, g_EchoPrintPageNum);
    DrawText(hdc, buf, strlen(buf), &rc, DT_CENTER | DT_VCENTER);

    int nLeft = 5;
    int nTop = 35;
    uint32 i = 0;

    for (i = 0; i < 9; i++)
    {
        nTop = 35 + 12 * i;
        TextOut(hdc, nLeft, nTop, g_EchoPrintBuf[g_EchoPrintPageIndex][i]);
    }
}

BOOL EchoPrintKey(int nKey)
{
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return FALSE;
    }

    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_EchoPrintPageIndex > 0)
            {
                g_EchoPrintPageIndex--;
            }
            else
            {
                g_EchoPrintPageIndex = g_EchoPrintPageNum - 1;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_EchoPrintPageIndex < g_EchoPrintPageNum - 1)
            {
                g_EchoPrintPageIndex++;
            }
            else
            {
                g_EchoPrintPageIndex = 0;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
    }

    return FALSE;
}

/* 回显打印 */
PLUGIN_INFO g_EchoPrint = {
    0x10A000F5,         /* id */
    EchoPrintInit,      /* InitLayer */
    NULL,               /* UninitLayer */
    EchoPrintDraw,      /* Draw */
    NULL,               /* Timer */
    EchoPrintKey,       /* keymsg */
    NULL                /* ipc */
};

uint8 g_SystemCmdIndex = 0;
uint8 g_SystemCmdStatus = 0; /* 0-未执行; 1-正在执行 */
char g_SystemCmdStr[MAX_COMMAND_LEN] = "route -n";

void SystemCmdChg(char *pNewStr, void *pPara)
{
    memset(g_SystemCmdStr, '\0', strlen(g_SystemCmdStr));
    memcpy(g_SystemCmdStr, pNewStr, strlen(pNewStr));
}

void *SystemCmdProcThread(void *arg)
{
    GUI_COMM *pg_guicomm = &g_guiComm;
    
    if (pg_guicomm == NULL)
    {
        return NULL;
    }

    g_SystemCmdStatus = 1;
    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    //system(g_SystemCmdStr);
    memset(g_ResultBuf, 0 , sizeof(g_ResultBuf));
    CmdExecute(g_SystemCmdStr, g_ResultBuf);
    g_SystemCmdStatus = 0;
    MessageBox("System命令已执行！");
    PopLayer(g_EchoPrint.dwId);
    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    return NULL;
}

void SystemCmdInit(DWORD dwPluginId)
{
    g_SystemCmdIndex = 0;
    g_SystemCmdStatus = 0;
    g_bpop = TRUE;
}

void SystemCmdUninit()
{
    g_bpop = FALSE;
}

void SystemCmdDraw(HDC hdc)
{
    GUI_COMM *pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
    {
        return;
    }

    SelectFont(hdc, pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 20;
    rc.bottom = rc.top + 16;
    const char *pcz = "System命令";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);

    rc.top = 40;
    rc.bottom = rc.top + 56;

    if (g_SystemCmdIndex == 0)
    {
        ITEM_SELECT;
        DrawText(hdc, g_SystemCmdStr, strlen(g_SystemCmdStr), &rc, DT_CENTER | DT_VCENTER);
        ITEM_NOT_SELECT;
    }
    else
    {
        DrawText(hdc, g_SystemCmdStr, strlen(g_SystemCmdStr), &rc, DT_CENTER | DT_VCENTER);
    }

    SelectFont(hdc, pg_guicomm->fontBig);
    int nTop = 100;
    int nLeft = 5;

    if (g_SystemCmdStatus == 1)
    {
        nLeft = 10;
        TextOut(hdc, nLeft, nTop, "执行System命令中...");
        nTop = 120;
        TextOut(hdc, nLeft, nTop, "请勿操作终端");
        return;
    }

    if (g_SystemCmdIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "返回");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "返回");
    }

    nLeft = 50;
    if (g_SystemCmdIndex == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "执行");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "执行");
    }

    nLeft = 95;
    if (g_SystemCmdIndex == 3)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "查看回显");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "查看回显");
    }
}

void SystemCmdUpdateTimer(int nId)
{
    /* 1s定时器检查 */
    if (nId != TIMER_MAIN)
    {
        return;
    }

    g_guiMain.nTrunDispTime = 0; /* 在System命令页面，不进入轮显 */
    g_guiMain.nLightonTime = 0;
    g_guiMain.bLighton = TRUE;
    lighton_lcd(); /* 在System命令页面，屏幕常亮 */
}

BOOL SystemCmdKey(int nKey)
{
    pthread_t threadId;
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return FALSE;
    }

    if (g_SystemCmdStatus == 1)
    {
        /* System命令正在执行，不能操作菜单 */
        return TRUE;
    }

    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_SystemCmdIndex > 0)
            {
                g_SystemCmdIndex--;
            }
            else
            {
                g_SystemCmdIndex = 3;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_SystemCmdIndex < 3)
            {
                g_SystemCmdIndex++;
            }
            else
            {
                g_SystemCmdIndex = 0;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (g_SystemCmdIndex)
            {
                case 0:
                {
                    InputBox(g_SystemCmdStr, SystemCmdChg, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x08);
                    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                    return FALSE; /* InputBox()与return FALSE搭配使用 */
                }

                case 1:
                {
                    CancelMenu();
                    break;
                }

                case 2:
                {
                    if (ValidatePwd() == TRUE)
                    {
                        if (FALSE == MessageBox("是否执行System命令", MB_OKCANCEL))
                        {
                            break;
                        }

                        if (pthread_create(&threadId, NULL, SystemCmdProcThread, NULL) != 0)
                        {
                            MessageBox("系统忙\n稍后再执行!");
                            break;
                        }

                        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                    }

                    break;
                }

                case 3:
                {
                    PopLayer(g_EchoPrint.dwId);
                    break;
                }
            }

            return TRUE;
        }
    }

    return FALSE;
}

/* System命令 */
PLUGIN_INFO g_SystemCmd = {
    0x00A000F5,         /* id */
    SystemCmdInit,      /* InitLayer */
    SystemCmdUninit,    /* UninitLayer */
    SystemCmdDraw,      /* Draw */
    SystemCmdUpdateTimer, /* Timer */
    SystemCmdKey,       /* keymsg */
    NULL                /* ipc */
};

unsigned int g_ACModuleIndex = 0;     //选择结果: 0-升级; 1-返回
unsigned int g_ACModuleUpdateStatus = 0;    //状态    : 0-未开始升级;1-拷贝安装文件;2-acMeter目录不存在;
                                       //          3-未找到U盘里的安装目录;4-缺少升级文件;5-正在安装
unsigned int g_ACModuleWaitTimer = 0;


#define AC_MODULE_UPDATE_STATUS_INIT       0
#define AC_MODULE_UPDATE_STATUS_COPY       1
#define AC_MODULE_UPDATE_STATUS_NO_AC_DIR  2
#define AC_MODULE_UPDATE_STATUS_NO_UP_DIR  3
#define AC_MODULE_UPDATE_STATUS_NO_BIN     4
#define AC_MODULE_UPDATE_STATUS_INSTALL    5

#define AC_MODULE_UPDATE_SRC_FOLDER "/tmp/udisk/acupdate"
#define AC_MODULE_UPDATE_DST_FOLDER "/data/app/acMeter"
#define AC_MODULE_UPDATE_DST_FILE "/data/app/acMeter/jc_update_file"
#define AC_MODULE_UPDATE_TMP_FILE "/data/app/acMeter/jc_update_file_back"
unsigned int ACModuleCopyPkg(char *pkgPath)
{
    char cmd[256];

    if (0 != access(AC_MODULE_UPDATE_DST_FOLDER, F_OK))
    {
        g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_NO_AC_DIR;
        return 1;
    }

    ModuleDeleteDir(AC_MODULE_UPDATE_DST_FILE);
    ModuleDeleteDir(AC_MODULE_UPDATE_TMP_FILE);

    sprintf(cmd, "cp -r %s %s", pkgPath, AC_MODULE_UPDATE_DST_FILE);
    system(cmd);
    system("sync");
    g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_INSTALL;
    g_ACModuleWaitTimer = 0;
    return 0;
}

unsigned int ACModuleGetPkgPath(char *pkgPath)
{
    DIR           *dir;
    struct dirent *ent;

    if ((dir = opendir(AC_MODULE_UPDATE_SRC_FOLDER)) == NULL)
    {
        g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_NO_UP_DIR;
        return 1;
    }
    
    while ((ent = readdir(dir)) != NULL)
    {
        if ((strcmp(ent->d_name, ".") == 0) || (strcmp(ent->d_name, "..") == 0))
        {
            continue;
        }
        else if (ent->d_type == 8)    /* 8:文件 */
        {
            sprintf(pkgPath, "%s/%s", AC_MODULE_UPDATE_SRC_FOLDER, ent->d_name);
            closedir(dir);
            return 0;
        }
    }

    g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_NO_BIN;
    closedir(dir);
    return 1;
}

void InitACModuleUpdateDraw(DWORD dwPluginId)
{
    g_ACModuleIndex = 1;
    g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_INIT;
    g_ACModuleWaitTimer = 0;
}

void ACModuleUpdateDraw(HDC hdc)
{
    int nTop = 0;
    int nLeft = 0;
    char buf[200] = {0};
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);

    if (g_ACModuleUpdateStatus == AC_MODULE_UPDATE_STATUS_COPY)
    {
        nTop = 40;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "拷贝文件中...");
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "耗时约1秒钟");      
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "请不要关闭电源");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "不要拔掉U盘");

        return;
    }

    if (g_ACModuleUpdateStatus == AC_MODULE_UPDATE_STATUS_INSTALL)
    {
        nTop = 40;
        nLeft = 16;
        sprintf(buf, "正在安装...(%u秒)", g_ACModuleWaitTimer);
        TextOut(hdc, nLeft, nTop, buf);
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "耗时约7分钟");      
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "请不要关闭电源");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "可以拔掉U盘");

        return;
    }

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 36;
    rc.bottom = rc.top + 16;
    const char *pcz = "交采模块升级";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    nTop = 98;
    nLeft = 26;

    if (g_ACModuleIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "返回");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "返回");
    }

    nLeft = 110;
    if (g_ACModuleIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "确定");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "确定");
    }

    return;
}

void ACModuleUpdateTimer(int nId)
{   
    char buf[200] = {0};
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    /* 1s定时器检查 */
    if (nId != TIMER_MAIN)
    {
        return;
    }

    g_guiMain.nTrunDispTime = 0; /* 在交采模块升级页面，不进入轮显 */
    g_guiMain.nLightonTime = 0;
    g_guiMain.bLighton = TRUE;
    lighton_lcd(); /* 在交采模块升级页面，屏幕常亮 */

    switch (g_ACModuleUpdateStatus)
    {
        case AC_MODULE_UPDATE_STATUS_INIT: //未开始升级，初始页面
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case AC_MODULE_UPDATE_STATUS_COPY: //升级文件拷贝
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case AC_MODULE_UPDATE_STATUS_NO_AC_DIR: //acMeter目录不存在
        {
            MessageBox("acMeter目录不存在！");
            g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_INIT;
            break;
        }
        case AC_MODULE_UPDATE_STATUS_NO_UP_DIR: //未找到U盘里的安装目录
        {
            sprintf(buf, "未找到U盘里的升级目录\n%s\n请检查U盘是否插入", AC_MODULE_UPDATE_SRC_FOLDER);
            MessageBox(buf);
            g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_INIT;
            break;
        }
        case AC_MODULE_UPDATE_STATUS_NO_BIN: //缺少升级文件
        {
            MessageBox("缺少升级文件！");
            g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_INIT;
            break;
        }
        case AC_MODULE_UPDATE_STATUS_INSTALL: //正在安装
        {
            if ((0 != access(AC_MODULE_UPDATE_DST_FILE, F_OK)) && (0 != access(AC_MODULE_UPDATE_TMP_FILE, F_OK)))
            {
                MessageBox("安装完成！\n请在ycjcup.log中\n查看升级结果");
                g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_INIT;
                break;
            }

            if (g_ACModuleWaitTimer <= 1000)
            {
                g_ACModuleWaitTimer++;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            }
            else
            {
                MessageBox("安装超时！\n请在ycjcup.log中\n查看升级结果");
                g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_INIT;
                g_ACModuleWaitTimer = 0;
            }
            break;
        }
        default: 
        {
            g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_INIT;
            break;
        }
    }

}

/* 入参由调用者保证合法性 */
unsigned int ACModuleUpdateProc(GUI_COMM *pg_guicomm)
{
    char pkgPath[PATH_MAX_LEN + 1] = {0};

    if (ValidatePwd() == TRUE)
    {
        if (FALSE == MessageBox("警告\n升级过程中请不要断电\n是否升级", MB_OKCANCEL))
        {
            return 0;
        }
    }
    else
    {
        return 1;
    }

    g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_COPY;
    UpdateWindow(pg_guicomm->hMainWnd, TRUE);

    if (ACModuleGetPkgPath(pkgPath) != 0)
    {
        return 1;
    }

    if (ACModuleCopyPkg(pkgPath) != 0)
    {
        return 1;
    }

    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    return 0;
}

BOOL ACModuleUpdateKey(int nKey)
{
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return FALSE;
    }

    /* 未开始升级状态下才能操作菜单 */
    if (g_ACModuleUpdateStatus != AC_MODULE_UPDATE_STATUS_INIT)
    {
        return TRUE;
    }

    switch (nKey)
    {
        case KEY_UP:
        case KEY_LEFT:
        {
            if (g_ACModuleIndex > 0)
            {
                g_ACModuleIndex--;
            }
            else
            {
                g_ACModuleIndex = 1;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        case KEY_RIGHT:
        {
            if (g_ACModuleIndex < 1)
            {
                g_ACModuleIndex++;
            }
            else
            {
                g_ACModuleIndex = 0;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (g_ACModuleIndex)
            {
                case 0: /* 返回 */
                {
                    CancelMenu();
                    return TRUE;
                }
                case 1: 
                {
                    ACModuleUpdateProc(pg_guicomm);
                    return TRUE;
                }
            }

            return TRUE;
        }
    }

    return FALSE;
}


//交采模块升级
PLUGIN_INFO g_ACModuleUpdate = {
    0x00A000F6,             /* id */
    InitACModuleUpdateDraw, /* InitLayer */
    NULL,                   /* UninitLayer */
    ACModuleUpdateDraw,     /* Draw */
    ACModuleUpdateTimer,    /* Timer */
    ACModuleUpdateKey,      /* keymsg */
    NULL                    /* ipc */
};


static int g_UpdateIndex = 0;     //选择结果: 0-安装公变; 1-安装专变; 2-返回
static unsigned int g_InstallType = 0;      //升级产品: 0-无; 1-公变;2-专变
static unsigned int g_UpdateStatus = 0;    //状态    : 0-未开始升级;1-拷贝安装文件;2-创建目录失败;3-正在安装;
                                       //          4-创建安装线程失败;5-未找到U盘里的安装目录;6-缺少安装脚本


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
#if defined PRODUCT_CCU
    if (0 != access("/tmp/udisk/package", F_OK))
    {
        g_UpdateStatus = UPDATE_STATUS_NO_PACKAGE;
        return 1;
    }

    system("cp -r /tmp/udisk/package/* /tmp/update/");
#else
    if (0 != access("/udisk/package", F_OK))
    {
        g_UpdateStatus = UPDATE_STATUS_NO_PACKAGE;
        return 1;
    }

    system("cp -r /udisk/package/* /tmp/update/");
#endif
    return 0;
}

unsigned int UdiskCheckPkg()
{
    uint8 product = 0; /* 产品类型: bit0-公变; bit1-专变 */

    if (0 == access("/tmp/update/setup.sh", F_OK))
    {
        product |= 0x01;
        sleep(2);  //延时等待一会儿
    }

    g_UpdateStatus = (0 == product ? UPDATE_STATUS_NO_SCRIPT : UPDATE_STATUS_INSTALL);

    return (0 == product ? 1 : 0);
}

#define INSTALL_TYPE_GB 1
#define INSTALL_TYPE_ZB 2
#define INSTALL_TYPE_QT 3    //预留


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

    if (g_InstallType == INSTALL_TYPE_GB)
    {
        system("chmod 755 -R /tmp/update");
        system("/tmp/update/setup.sh > /data/app/desktopGui/log/udisk.log -d");
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
        TextOut(hdc, nLeft, nTop, "拷贝文件中...");
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "耗时约30s");
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "请不要关闭电源");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "不要拔掉U盘");

        return;
    }

    if (g_UpdateStatus == UPDATE_STATUS_INSTALL)
    {
        nTop = 40;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "正在安装...");
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "耗时约1分钟");
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "请不要关闭电源");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "可以拔掉U盘");

        return;
    }

    nLeft = 40;
    nTop = 36;
    SelectFont(hdc, pg_guicomm->fontBig);

    if (0 == g_UpdateIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "安装程序");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "安装程序");
    }

    nTop += 30;
    if (1 == g_UpdateIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "执行脚本");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "执行脚本");
    }

    nTop += 30;
    if (2 == g_UpdateIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "返回");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "返回");
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
    
    /* 1s定时器检查 */
    if (nId != TIMER_MAIN)
    {
        return;
    }

    g_guiMain.nTrunDispTime = 0; /* 在U盘升级页面，不进入轮显 */
    g_guiMain.nLightonTime = 0;
    g_guiMain.bLighton = TRUE;
    lighton_lcd(); /* 在U盘升级页面，屏幕常亮 */

    switch (g_UpdateStatus)
    {
        case UPDATE_STATUS_INIT: //未开始升级，初始页面
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case UPDATE_STATUS_COPY: //升级文件拷贝
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case UPDATE_STATUS_MKDIR_ERR: //创建目录失败
        {
            MessageBox("创建目录失败！");
            g_UpdateStatus = UPDATE_STATUS_INIT;
            break;
        }
        case UPDATE_STATUS_INSTALL: //正在安装
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case UPDATE_STATUS_CREATE_ERR: //创建安装线程失败
        {
            MessageBox("创建安装线程失败！");
            g_UpdateStatus = UPDATE_STATUS_INIT;
            break;
        }
        case UPDATE_STATUS_NO_PACKAGE: //未找到U盘里的安装目录
        {
            MessageBox("未找到U盘里的安装目录\n请检查U盘是否插入");
            g_UpdateStatus = UPDATE_STATUS_INIT;
            break;
        }
        case UPDATE_STATUS_NO_SCRIPT: //缺少安装脚本
        {
            MessageBox("缺少安装脚本！");
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

/* 入参由调用者保证合法性 */
unsigned int UdiskInstallProc(GUI_COMM *pg_guicomm, unsigned int installType)
{
    int errCode = 0;
    pthread_t threadId;
    const char *message = "警告\n安装过程中请不要断电\n是否安装";
    //char *productName[] = {(char *)"无效值", (char *)"集中器", (char *)"专变" , (char *)""};
   // char buf[200] = {0};

    if (ValidatePwd() == TRUE)
    {
        //sprintf(buf, "%s%s", message, productName[installType]);
        if (FALSE == MessageBox(message, MB_OKCANCEL))
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

    /* 未开始升级状态下才能操作菜单 */
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
                g_UpdateIndex = 2;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        case KEY_RIGHT:
        {
            g_UpdateIndex++;
            if (g_UpdateIndex > 2)
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
                case 0: /* 安装程序 */
                {
                    UdiskInstallProc(pg_guicomm, INSTALL_TYPE_GB);
                    return TRUE;
                }
                case 1: /* 安装其他 */
                {
                    UdiskInstallProc(pg_guicomm, INSTALL_TYPE_QT);
                    return TRUE;
                }
                case 2: /* 退出 */
                {
                    CancelMenu();
                    return TRUE;
                }
            }

            return TRUE;
        }
    }

    return FALSE;
}
uint8 logexportindex;
int logflag;
time_t logstarttime;
pthread_t copythreadid;
void LogExportInit(DWORD dwPluginId)
{
    logexportindex = 0;
    logflag = -1;
    g_bpop = TRUE;
}
void LogExportUnInit()
{
    logexportindex = 0;
    g_bpop = FALSE;
}

void LogExportDraw(HDC hdc)
{
     GUI_COMM* pg_guicomm = &g_guiComm;
    if(pg_guicomm == NULL)
        return;
    int index = 0;
    char czbuf[300]={0};
    int nleft = 40;
    int ntop = 36;
    SelectFont(hdc,pg_guicomm->fontBig);
    sprintf(czbuf,"备份所有日志");
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
    sprintf(czbuf,"备份所有程序");
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
    sprintf(czbuf,"备份所有文件");
    if(index == logexportindex)
    {
        ITEM_SELECT;
        TextOut(hdc, nleft, ntop, czbuf);
        ITEM_NOT_SELECT;
    }else
    {
        TextOut(hdc, nleft, ntop, czbuf);
    }

//    index++;
//  ntop+=28;
//  sprintf(czbuf,"备份数据库文件");
//  if(index == logexportindex)
//  {
//        ITEM_SELECT;
//        TextOut(hdc, nleft, ntop, czbuf);
//        ITEM_NOT_SELECT;
//  }else
//  {
//        TextOut(hdc, nleft, ntop, czbuf);
//  }
    switch(logflag)
    {
        case 2:
        {
            pthread_cancel(copythreadid);
            pthread_join(copythreadid, NULL);

            system("ps -ef|grep /tmp/udisk |grep -v grep|awk '{print $2}' |xargs kill");
            system("sync");
            logflag = -1;

            MessageBox("备份时间超过15分钟\n自动结束！");

        }
        break;
        case 1:
        {
            pthread_join(copythreadid, NULL);
            GUI_FMT_TRACE("备份完成\n");

            logflag = -1;

            
MessageBox("备份完成！");
        }
        break;
    }
}

void LogExportTimer(int nId)
{   
    GUI_COMM *pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
    {
        return;
    }

    /* 1s定时器检查 */
    if (nId != TIMER_MAIN)
    {
        return;
    }

    g_guiMain.nTrunDispTime = 0; /* 在U盘升级页面，不进入轮显 */
    g_guiMain.nLightonTime = 0;
    g_guiMain.bLighton = TRUE;
//    lighton_lcd(); /* 在U盘升级页面，屏幕常亮 */

    time_t t;

    if(logflag == 0)
    {
        time(&t);
        if(t - logstarttime>90000)
        {
            GUI_FMT_DEBUG("copy timeout!!\n");
            logflag = 2;
            ClosePopLayer();
        }
    }
    else if(logflag == 1)
    {
        ClosePopLayer();
    }
}

//检测是否插入U盘
BOOL CanUseUDisks()
{
    char cmd[128] = {0};
    char pstatus[128] = {0};

    sprintf(cmd, "ls /sys/class/scsi_device");
    FILE* pf = popen(cmd, "r");  
      
    if(pf == NULL)
    {
        return FALSE;// U盘没有插入
    }
    
    if(fgets(pstatus, 128, pf) == NULL)
    {
        pclose(pf);
        return FALSE;// U盘没有插入
    }
    pclose(pf);

    if(pstatus[strlen(pstatus)-1] == '\n')
        pstatus[strlen(pstatus)-1] = '\0';

    memset(cmd,0 , 128);
    sprintf(cmd, "ls /sys/class/scsi_device/%s/device/block", pstatus);
    pf = popen(cmd, "r");  
      
    if(pf == NULL)
    {
        return FALSE;// 
    }

    memset(pstatus,0 , 128);
    if(fgets(pstatus, 128, pf) == NULL)
    {
        pclose(pf);
        return FALSE;// 
    }
    pclose(pf);

    if(pstatus[strlen(pstatus)-1] == '\n')
        pstatus[strlen(pstatus)-1] = '\0';

    memset(cmd,0 , 128);
    sprintf(cmd, "cat /proc/mounts | grep %s", pstatus);
    pf = popen(cmd, "r");  
      
    if(pf == NULL)
    {
        return FALSE;// 
    }

    memset(pstatus,0 , 128);
    if(fgets(pstatus, 128, pf) == NULL)
    {
        pclose(pf);
        return FALSE;// U盘没有挂载
    }
    pclose(pf);
        
    return TRUE;
}

#if defined PRODUCT_CCU
void *CopyThreads(void *arg)
{
    char buf[300]={0};
    char cmd[1000]={0};

    //拷贝文件夹名称以ip+时间
    char pstatus[128] = "127.0.0.1";
    time_t t;
    time(&t);
    logstarttime = t;
    tm *p = localtime(&t);
    tm ptime;
    memcpy(&ptime,p,sizeof(tm));
    sprintf(cmd, "ifconfig FE0 |awk  'NR==2 {print $2}' |awk -F  \":\"  '{print $2}'");
    FILE* pf = popen(cmd, "r");  
    if(pf != NULL)
    {
        fgets(pstatus, 128, pf);
    }
    pclose(pf);
    if(pstatus[strlen(pstatus)-1] == '\n')
        pstatus[strlen(pstatus)-1] = '\0';

    sprintf(buf,"          正在备份\n\n        复制时间较长\n\n  请勿操作终端或者拔出U盘");
//    g_bpop = TRUE;
//    PopMsg(buf);

    system("cp -rf /tmp/data/app /data/app");
    system("sync");


    switch(logexportindex)
    {
    case 0:         //备份日志文件
    {
        sprintf(cmd,"mkdir -p /tmp/udisk/log_%s_%02d.%02d", pstatus, ptime.tm_hour, ptime.tm_min);
        system(cmd);
        system("tar -zcvf /tmp/datalog.tar.gz /data/app/");
        system("tar -zcvf /tmp/tmplog.tar.gz /tmp/data/");
        system("tar -zcvf /tmp/runlog.tar.gz /run/log/");
        system("tar -zcvf /tmp/logapplog.tar.gz /log/app/");
        system("tar -zcvf /tmp/messageslog.tar.gz /var/log/messages* ");
        system("tar -zcvf /tmp/varlog.tar.gz /var/log/*.log ");
        sprintf(cmd,"mv /tmp/*log.tar.gz /tmp/udisk/log_%s_%02d.%02d", pstatus, ptime.tm_hour, ptime.tm_min);
    }
        break;
    case 1:         //备份应用程序
    {
        sprintf(cmd,"mkdir /tmp/udisk/host; mkdir /tmp/udisk/baseContainer;mkdir /tmp/udisk/pdContainer;mkdir /tmp/udisk/amrContainer; mkdir /tmp/udisk/edgerContainer;");
        system(cmd);
        sprintf(cmd,"cp -rfP /usr/local/extapps/* /tmp/udisk/host/;docker cp baseContainer:/usr/local/extapps/dbCenter /tmp/udisk/baseContainer/; "
                                "docker cp baseContainer:/usr/local/extapps/ccoRouter /tmp/udisk/baseContainer/;docker cp edgerContainer:/usr/local/extapps/taskManager /tmp/udisk/edgerContainer/; "
                                "docker cp amrContainer:/usr/local/extapps/puAmr /tmp/udisk/amrContainer/;docker cp pdContainer:/usr/local/extapps/pdAmr /tmp/udisk/pdContainer/;");
        
    }
        break;
    case 2:         //备份所有文件
    {
        sprintf(cmd,"mkdir -p /tmp/udisk/data/app; mkdir -p /tmp/udisk/var/log; mkdir /tmp/udisk/host; mkdir /tmp/udisk/baseContainer;mkdir /tmp/udisk/pdContainer;mkdir /tmp/udisk/amrContainer; mkdir /tmp/udisk/edgerContainer;");
        system(cmd);
        sprintf(cmd,"cp -rfP /usr/local/extapps/* /tmp/udisk/host/;docker cp baseContainer:/usr/local/extapps/dbCenter /tmp/udisk/baseContainer/; "
                                "docker cp baseContainer:/usr/local/extapps/ccoRouter /tmp/udisk/baseContainer/;docker cp edgerContainer:/usr/local/extapps/taskManager /tmp/udisk/edgerContainer/; "
                                "docker cp amrContainer:/usr/local/extapps/puAmr /tmp/udisk/amrContainer/;docker cp pdContainer:/usr/local/extapps/pdAmr /tmp/udisk/pdContainer/;"
                                "cp  -rfP /data/app/* /tmp/udisk/data/app/; cp -rfP /var/log/* /tmp/udisk/var/log;");
       
    }
        break;
    case 3:         //备份数据库文件
    {
        sprintf(cmd,"mkdir -p /tmp/udisk/dbCenterData_%s_%02d.%02d", pstatus, ptime.tm_hour, ptime.tm_min);
        system(cmd);
        system("tar -zcvf /tmp/dbCenterData.tar.gz /data/app/dbCenter/data");
        system("tar -zcvf /tmp/tmpdb.tar.gz /tmp/data/TMemory*");
        sprintf(cmd,"mv /tmp/dbCenterData.tar.gz /tmp/udisk/dbCenterData_%s_%02d.%02d", pstatus, ptime.tm_hour, ptime.tm_min);
        system(cmd);
        sprintf(cmd,"mv /tmp/tmpdb.tar.gz /tmp/udisk/dbCenterData_%s_%02d.%02d", pstatus, ptime.tm_hour, ptime.tm_min);
    }
        break;
    }
    
    system(cmd);
    system("sync");

//    g_bpop = FALSE;
    GUI_FMT_TRACE("copythread 操作完成 退出\n");
    
    *(int *)arg = 1;
    
    pthread_exit(NULL);
}
#else
void *CopyThreads(void *arg)
{
    char buf[512]       = {0};
    char cmd[512]       = {0};
    char logPath[512]   = {0};
    char AppPath[512]   = {0};

    //拷贝文件夹名称以ip+时间
    char pstatus[128] = "127.0.0.1";
    time_t t;
    time(&t);
    logstarttime = t;
    tm *p = localtime(&t);
    tm ptime;
    memcpy(&ptime,p,sizeof(tm));
    sprintf(cmd, "ifconfig FE0 | grep 'inet addr' | awk '{ print $2}' | awk -F: '{print $2}'");
    FILE* pf = popen(cmd, "r");  
    if(pf != NULL)
    {
        fgets(pstatus, 128, pf);
    }
    pclose(pf);
    if(pstatus[strlen(pstatus)-1] == '\n')
        pstatus[strlen(pstatus)-1] = '\0';

    sprintf(buf,"          正在备份\n\n        复制时间较长\n\n  请勿操作终端或者拔出U盘");
//    g_bpop = TRUE;
//    PopMsg(buf);

//    system("cp -rf /tmp/data/* /mnt/data/app/dbCenter/data/");
//    system("sync");

    

    switch(logexportindex)
    {
        case 0:         //备份日志文件
        {
            sprintf(logPath,"/tmp/udisk/log_%s_%02d.%02d.%02d", pstatus, ptime.tm_hour, ptime.tm_min, ptime.tm_sec);
            
            sprintf(cmd, "mkdir -p %s", logPath);
            system(cmd);
            
            sprintf(cmd, "tar -zcvf %s/log.tar.gz /data/app/", logPath);
            system(cmd);

            sprintf(cmd, "tar -zcvf %s/tmplog.tar.gz /tmp/data/", logPath);
            system(cmd);

            sprintf(cmd, "tar -zcvf %s/runlog.tar.gz /run/log/", logPath);
            system(cmd);

            sprintf(cmd, "tar -zcvf %s/logapplog.tar.gz /log/app/", logPath);
            system(cmd);

            sprintf(cmd, "tar -zcvf %s/varlog.tar.gz /var/log/*.log ", logPath);
            system(cmd);

            sprintf(cmd, "tar -zcvf %s/messages.tar.gz /var/log/messages*", logPath);
            system(cmd);
            break;
        }
        case 1:         //备份应用程序
        {
            sprintf(AppPath,"/tmp/udisk/Apps_%s_%02d.%02d.%02d", pstatus, ptime.tm_hour, ptime.tm_min, ptime.tm_sec);
            
            sprintf(cmd, "mkdir -p %s", AppPath);
            system(cmd);
            sprintf(cmd, "cp -rf /usr/local/extapps/* %s/", AppPath);
            system(cmd);
            break;
        }
        case 2:         //备份所有文件
        {
            sprintf(logPath,"/tmp/udisk/log_%s_%02d.%02d.%02d", pstatus, ptime.tm_hour, ptime.tm_min, ptime.tm_sec);
            
            sprintf(cmd, "mkdir -p %s", logPath);
            system(cmd);
            
            sprintf(cmd, "tar -zcvf %s/log.tar.gz /data/app/", logPath);
            system(cmd);

            sprintf(cmd, "tar -zcvf %s/tmplog.tar.gz /tmp/data/", logPath);
            system(cmd);

            sprintf(cmd, "tar -zcvf %s/messages.tar.gz /var/log/messages*", logPath);
            system(cmd);

            sprintf(cmd, "tar -zcvf %s/varlog.tar.gz /var/log/*.log ", logPath);
            system(cmd);
//
//            sprintf(cmd, "tar -zcvf %s/data.tar.gz /data/app/", logPath);
//            system(cmd);
            
            sprintf(AppPath,"/udisk/Apps_%s_%02d.%02d.%02d", pstatus, ptime.tm_hour, ptime.tm_min, ptime.tm_sec);
            
            sprintf(cmd, "mkdir -p %s", AppPath);
            system(cmd);
            sprintf(cmd, "cp -rf /usr/local/extapps/* %s/", AppPath);
            system(cmd);
            break;
        }
    }
    
//    system(cmd);
    system("sync");

//    g_bpop = FALSE;
    GUI_FMT_TRACE("copythread 操作完成 退出\n");
    
    *(int *)arg = 1;
    
    pthread_exit(NULL);
}

#endif

BOOL LogExportKeyMsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if(pg_guicomm == NULL)
        return FALSE;
    int maxindex = 2;
    char buf[300]={0};
    int err = 0;
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
//            g_bpop = FALSE;
            switch(logexportindex)
            {
                case 0:
                    sprintf(buf,"即将备份日志文件到U盘\n是否确认？");
                break;
                case 1:
                    sprintf(buf,"即将备份应用程序到U盘\n是否确认？");
                break;
                case 2:
                    sprintf(buf,"即将备份文件到U盘\n是否确认？");
                break;
                case 3:
                    sprintf(buf,"即将备份数据库文件到U盘\n是否确认？");
                break;
                default:
                    return FALSE;
            }
            if(TRUE == MessageBox(buf, MB_OKCANCEL))
            {
                if(FALSE == CanUseUDisks())
                {
                    MessageBox("U盘没有插入\n\n不能备份文件!");
                    return TRUE;
                }
                /*创建一个线程,子线程执行拷贝等操作,主线程喂守护进程*/
                logflag = 0;
                err = pthread_create(&copythreadid,NULL, CopyThreads, &logflag);
                if (err != 0)
                {
                    logflag = -1;
                    MessageBox("系统忙\n稍后再拷贝!");
                    break;
                }
                PopMsg("          正在备份\n\n        复制时间较长\n\n  请勿操作终端或者拔出U盘");
                return FALSE;
            }
        }
    }
    }
    return FALSE;
}


//日志导出
PLUGIN_INFO g_smiOSLogExport = {
    0x00A000F1/*id*/, 
    LogExportInit/*InitLayer*/,
    LogExportUnInit/*UninitLayer*/,
    LogExportDraw/*Draw*/,
    LogExportTimer/*Timer*/,
    LogExportKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

//U盘升级
PLUGIN_INFO g_smiOSUUpdate = {
    0x00A000F0/*id*/, 
    InitUdiskUpdateDraw/*InitLayer*/,
    NULL/*UninitLayer*/,
    UdiskUpdateDraw/*Draw*/,
    UdiskUpdateTimer/*Timer*/,
    UdiskUpdateKey/*keymsg*/,
    NULL/*ipc*/
};
    
uint8 bluetooth_status;
int bluetooth_index;
const char *g_bluetooth_list[] = {
    "关闭",
    "开启",
};
void bluetooth_Init(DWORD dwPluginId)
{
//    get_bluetooth_state(&bluetooth_status);
    bluetooth_index = 0;
    g_bpop = TRUE;
}
void bluetooth_Uninit()
{
    g_bpop = FALSE;
}

void bluetooth_Draw(HDC hdc)
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
    strcpy(pcz, "蓝牙功能设置");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if(bluetooth_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "开启");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "开启");
    }

    nLeft = 110;
    if(bluetooth_index == 1)
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

//choice 1 开启ssh 0 关闭ssh
void bluetooth_set(uint8 choice)
{
    char cmd[100]={0};
    int ret = 0;
    ret = set_bluetooth_state(choice);
    
    if(ret == 0)
    {
        sprintf(cmd,"%s蓝牙功能成功!", g_bluetooth_list[choice]);
        if(choice == 1)
        {
            bluetooth_status = BLUETOOH_ON;
        }
        else
        {
            bluetooth_status = BLUETOOH_OFF;
        }
        write_pdata_xram(GUI_PARA, (char *)&bluetooth_status, Bluetooth_Status, Bluetooth_Status_Len);
    }
    else 
    {
        sprintf(cmd,"%s蓝牙功能失败!", g_bluetooth_list[choice]);
    }

    MessageBox(cmd);
}
BOOL bluetooth_KeyMsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if(pg_guicomm == NULL)
        return FALSE;

    switch(nKey)
    {
    case KEY_LEFT:
    case KEY_UP:
        if(bluetooth_index>0)
            bluetooth_index --;
        else
            bluetooth_index = 1;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
    case KEY_DOWN:
        if(bluetooth_index<1)
            bluetooth_index++;
        else
            bluetooth_index = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        if(ValidatePwd() == TRUE)
        {
            switch(bluetooth_index)
            {
                case 0:
                    if(FALSE == MessageBox("提示\n是否开启蓝牙功能\n", MB_OKCANCEL))
                    {
                        return TRUE;
                    }
                    bluetooth_set(1);
                    CancelMenu();
                    return TRUE;
                case 1:

                    if(FALSE == MessageBox("提示\n是否关闭蓝牙功能\n", MB_OKCANCEL))
                    {
                        return TRUE;
                    }
                    bluetooth_set(0);
                    CancelMenu();
                    return TRUE;
            }
//        g_bpop = FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

PLUGIN_INFO g_layer_bluetooth=
{
    0x00A000F7, // 0x00A000F7
    bluetooth_Init,
    bluetooth_Uninit,
    bluetooth_Draw,
    NULL,
    bluetooth_KeyMsg,
    NULL
};

void CPU_temperature_Init(DWORD dwPluginId)
{

}

int get_cpu_cur_temper(char * temp, int buf_len)
{
    if(temp == NULL)
        return -1;
    int cpu0 = -100;
    int cpu1 = -100;
    char buf[32] = {0};
    int value = 0;
    FILE* pipe0 = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    FILE* pipe1 = fopen("/sys/class/thermal/thermal_zone1/temp", "r");
    if(pipe0)
    {
        if(NULL != fgets(buf, 32, pipe0))
        {
            cpu0 = atoi(buf);
        }
        fclose(pipe0);
    }
    if(pipe1)
    {
        memset(buf, 0, 32);
        if(NULL != fgets(buf, 32, pipe1))
        {
            cpu1 = atoi(buf);
        }
        fclose(pipe1);
    }
    if(cpu0 <= -100 && cpu1 <= -100)
    {
        GUI_FMT_DEBUG("查询cpu当前温度失败!\n");
        return -1;
    }
    value = cpu0 > cpu1 ? cpu0 : cpu1;
#ifdef BOARD_3358
    sprintf(temp, "%d.%0d℃", value / 1000, value % 1000);
#endif
#ifdef BOARD_801
    sprintf(temp, "%d℃", value);
#endif
    return 0;
}

void CPU_temperature_Draw(HDC hdc)
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
    char pcz[40];
    strcpy(pcz, "CPU当前温度");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);
    
    int nTop = 80;
    int nLeft = 26;
    
    TextOut(hdc, nLeft, nTop, "当前温度：");

    nLeft = 90;
    if(get_cpu_cur_temper(pcz, 40) == 0)
    {
        TextOut(hdc, nLeft, nTop, pcz);
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "-----℃");
    }
    
}


PLUGIN_INFO g_layer_CPU_temperature=
{
    0x00A000F8, // 0x00A000F8
    CPU_temperature_Init,
    NULL,
    CPU_temperature_Draw,
    NULL,
    NULL,
    NULL
};

//FTP TELNET 功能开启关闭设置

uint8 FTP_Telnet_status = 0;        //ftp telnet使能： 0——关闭，1——开启，默认关闭
const char *g_FTP_TelnetModel_list[] = {
    "关闭",
    "启用",
};
uint8 key_index = 0;

void setFTP_TelnetONOFFInit(DWORD dwPluginId)
{
    g_guiComm.bCanTrunDisp = FALSE;

    //if ((ReadXPara(TMN_PARA_EXTEND,FTP_TELNET_EN, (char *)&FTP_Telnet_status, 0) < 0) || (FTP_Telnet_status != 1))
    {
        FTP_Telnet_status = 0;
    }

    key_index = 0;
}

void setFTP_TelnetONOFFDraw(HDC hdc)
{
    //int       nTop = 0;
    char      czbuf[32];
    RECT      rc;
    //uint8     i = 0;

    SelectFont(hdc,g_guiComm.fontBig);

    //标题显示
    ITEM_NOT_SELECT;
    rc.left  = 0;
    rc.right = g_guiComm.nWidth;
    rc.top   = 40;
    rc.bottom = rc.top+16;
    sprintf(czbuf, "ftp/Telnet使能设置:");
    DrawText(hdc, czbuf, strlen(czbuf), &rc, DT_VCENTER);

    rc.left  = 30;
    rc.right = g_guiComm.nWidth;
    rc.top   = 70;
    rc.bottom = rc.top+16;

     if(key_index == 0)
     {
         ITEM_SELECT;
         TextOut(hdc, rc.left, rc.top, g_FTP_TelnetModel_list[FTP_Telnet_status]);
         ITEM_NOT_SELECT;
     }
     else
     {
         TextOut(hdc, rc.left, rc.top, g_FTP_TelnetModel_list[FTP_Telnet_status]);
     }

     if(key_index == 1)
     {
         ITEM_SELECT;
         TextOut(hdc, 120, 110, "确定");
         ITEM_NOT_SELECT;
     }
     else
     {
         TextOut(hdc, 120, 110, "确定");
     }

     PopListDraw(hdc);
}

 BOOL setFTP_TelnetONOFFMsg(int nKey)
 {
     //uint8     pRD;
     //uint8     pRD1 = 0x55;
     // 弹出List选择处理
     if(g_guiComm.bPopList == TRUE)
     {
         int nSel;
         PopListKeyMsg(nKey, &nSel);
         if(nKey == KEY_OK)
         {
             FTP_Telnet_status = nSel;
             UpdateWindow(g_guiComm.hMainWnd, TRUE);
         }
         return TRUE;
     }

     switch(nKey)
     {
     case KEY_UP:
         key_index--;
         if(key_index < 0)
             key_index = 1;
         UpdateWindow(g_guiComm.hMainWnd, TRUE);
         return TRUE;

     case KEY_DOWN:
         key_index ++;
         if(key_index > 1)
             key_index = 0;
         UpdateWindow(g_guiComm.hMainWnd, TRUE);
         return TRUE;

     case KEY_OK:
         if(ValidatePwd() == TRUE)
         {// 确认处理
             switch(key_index)
             {
                case 0:
                    PopList(g_FTP_TelnetModel_list, 2, g_FTP_TelnetModel_list[FTP_Telnet_status], 30, 80, FALSE);
                    break;
                case 1:
                    //WriteXPara(TMN_PARA_EXTEND,FTP_TELNET_EN, (char *)&FTP_Telnet_status, 0);
                    UpdateWindow(g_guiComm.hMainWnd, TRUE);
                    MessageBox("设置成功");
                    if(FTP_Telnet_status ==1 )//开启
                    {
                        system("inetd &");
                    }
                    else if(FTP_Telnet_status==0)//关闭
                    {
                        system("killall inetd");
                    }
                    break;
                default:
                    break;
             }
             return TRUE;
         }
     }

    return TRUE;
}

PLUGIN_INFO g_layersetFTP_TelnetONOFF=
{
    0x000c0004, // 0x000c0004
    setFTP_TelnetONOFFInit,
    NULL,
    setFTP_TelnetONOFFDraw,
    NULL,
    setFTP_TelnetONOFFMsg,
    NULL
};


#if DESC("电源供电状态",1)

/**********************************************************************
* @name      : gui_check_cell_insert
* @brief     ：从遥信中获取电池是否插入
* @param[in] ：
* @param[out]：
* @return    ：TRUE-插入/FALSE-没有插入
* @Create    : 王津剑
* @Date      ：2022-4-13
* @Update    :
**********************************************************************/
uint8 gui_check_cell_insert()
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
        return 0;
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

    return 1 == type ? FALSE : TRUE;
}

void power_status_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if(pg_guicomm == NULL)
        return;
    //int       nTop = 0;
    char      czbuf[32];
    RECT      rc;
    uint8     flag = 0;
    uint8     flag1 = 0;
    uint8     flag2 = 0;
    uint8     flag3 = 0;
    int32     state = 0;
    int       datav1 = 0;
    int       datav2 = 0;
    int       datav3 = 0;
    int nTop = 16;
    int nLeft = 10;
    int nLeft2 = 80;

    POWER_DEVICE_T* dev = (POWER_DEVICE_T*) hal_device_get(HW_DEVICE_ID_POWER);
    if(NULL != dev)
    {
        dev->power_power_state_get(dev, POWER_MAIN, &state);
        flag = 1;
        hal_device_release((HW_DEVICE *)dev);
    }
    
    SelectFont(hdc,g_guiComm.fontBig);

    //标题显示
    ITEM_NOT_SELECT;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;
    rc.top = 24;
    rc.bottom = rc.top+16;
    char pcz[40] = {0};
    strcpy(pcz, "电源供电状态");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);
    nTop = rc.top + 24;
    TextOut(hdc, nLeft, nTop, "供电方式：");
    if(0 == flag)
    {
        sprintf(czbuf, "----");
    }
    else
    {
        if(state == 1)
        {
            sprintf(czbuf, "正常供电");
        }
        else
        {
            sprintf(czbuf, "电池供电");
        }
    }
    TextOut(hdc, nLeft2, nTop, czbuf);

    ADC_DEVICE_T* adcdev = (ADC_DEVICE_T*)hal_device_get(HW_DEVICE_ID_ADC);
    if(NULL != adcdev)
    {
        if(0 == adcdev->adc_volt_read(adcdev, EMADC_RTCVOL_CHANNEL, &datav1))
        {
            flag1 = 1;
        }
        if(0 == adcdev->adc_volt_read(adcdev, EMADC_BATVOL_CHANNEL, &datav2))
        {
            flag2 = 1;
        }
        if(0 == adcdev->adc_volt_read(adcdev, EMADC_CAPVOL_CHANNEL, &datav3))
        {
            flag3 = 1;
        }
        hal_device_release((HW_DEVICE *)adcdev);
    }

    nTop += 20;
    TextOut(hdc, nLeft, nTop, "时钟电池：");
    memset(czbuf, 0, 32);
    if(0 == flag1)
    {
        sprintf(czbuf, "----");
    }
    else
    {
        sprintf(czbuf, "%d.%03d V", datav1 / 1000, datav1 % 1000);
    }
    TextOut(hdc, nLeft2, nTop, czbuf);

    nTop += 20;
    TextOut(hdc, nLeft, nTop, "备用电池：");
    memset(czbuf, 0, 32);
    if(0 == flag2 || FALSE == gui_check_cell_insert())
    {
        sprintf(czbuf, "----");
    }
    else
    {
        sprintf(czbuf, "%d.%03d V", datav2 / 1000, datav2 % 1000);
    }
    TextOut(hdc, nLeft2, nTop, czbuf);

    nTop += 20;
    TextOut(hdc, nLeft, nTop, "超级电容：");
    memset(czbuf, 0, 32);
    if(0 == flag3)
    {
        sprintf(czbuf, "----");
    }
    else
    {
        sprintf(czbuf, "%d.%03d V", datav3 / 1000, datav3 % 1000);
    }
    TextOut(hdc, nLeft2, nTop, czbuf);
}

PLUGIN_INFO g_layer_power_status=
{
    0x00A000F9, // 0x00A000F9
    NULL,
    NULL,
    power_status_draw,
    NULL,
    NULL,
    NULL
};
#endif

#if DESC("磁盘状态显示",1)

char device[50] = {0};
uint8 mmc_result[1024] = {0};
int   dis_index = 0;  //
int   data_flag = 0;  // 0 表示读到数据， 其他表示读取失败

//将数组逆序  buf:首地址  len:数组长度
int calData(uint8 *buf, uint16 len, uint32 *data)
{
    uint16  i = 0;

    if(buf == NULL || data == NULL)
    {
        return -1;
    }
    
    if(len <= 0 )
    {
        return -2;
    }
    
    //计算值
    *data = buf[0];
    
    for(i=1; i<len;i++)
    {
        *data += buf[i]<<(i*8);
    }
    
    return 0;
} 

int StringHextoHex(char *str, unsigned char *out, int *outlen)
{
    //str中的数据以' '相隔
    if(str == NULL || out == NULL || outlen == NULL)
    {
        return -1;
    }
    
    int index = 0;
    int outIndex = 0;
    uint8 data = 0;
    int flag = 1;                //认为是高4位
    while(str[index] != '\0')
    {
      //判是否空格
      if(str[index] == ' ')
      {
        flag = 1;
        out[outIndex++] = data;
        data = 0;
        index++;
        continue;
      }
      else if(str[index] >= '0' && str[index] <= '9')
      {
        data += (str[index] - '0') << (4 * flag);
      }
      else if(str[index] >= 'A' && str[index] <= 'Z')
      {
        data += (str[index] - 'A' + 10) << (4 * flag);
      }
      else if(str[index] >= 'a' && str[index] <= 'z')
      {
        data += (str[index] - 'a' + 10) << (4 * flag);
      }
      else if(str[index] == '\n')
      {
        //最后会有一个换行
        index++;
        continue;
      }
      else
      {
        GUI_FMT_DEBUG("str[%d] = %d\n", index, str[index]);
        return -2;
      }
      index++;
      flag = 0;
    }

    //补最后一个数据
//    if(str[index]=='\0' && index >= 2 && str[index - 1] != ' ')
//    {
//       out[outIndex] = data;
//    }

    *outlen = outIndex + 1;
    return 0;
}

uint32 MmcCmdExecute(const char *cmd, char *result)
{
    //有效性检查
    if (NULL == cmd || NULL == result)
    {
        //dzlog_warn("%s, invalid param", __FUNCTION__);
        return -1;
    }
    char tmpBuf[512 + 1];
    char tmpCmd[MAX_COMMAND_LEN + 5] = {0};
    FILE* fp = NULL;
    sprintf(tmpCmd, "%s 2>&1", cmd);
    //命令执行，打开管道
    if ((fp = popen(tmpCmd, "r")) != NULL)
    {
        //获取命令执行日志
        while (fgets(tmpBuf, 512, fp) != NULL)
        {
            if (strlen(result) >= MAX_RESULT_LEN)
            {
                
                break;
            }

            if (strlen(result) + strlen(tmpBuf) >= MAX_RESULT_LEN)
            {
                tmpBuf[MAX_RESULT_LEN - strlen(result)] = '\0';
                strcat(result, tmpBuf);
                
                break;
            }

            strcat(result, tmpBuf);
        }
        //关闭管道
        pclose(fp);
        fp = NULL;
    }
    else
    {
        //dzlog_error("%s, popen %s error", __FUNCTION__, tmpCmd);
        sprintf(result, "popen failed. Cmd:%s", tmpCmd);
        return 1;
    }

    return 0;
}

int get_root_mmc_device(char * device)
{
    char buf[1024] = {0};
    int  offset = 0; //设备号偏移度，相对pfile
    int has_mmc_device = 0;

    if(device == NULL)
        return -1;
    
    FILE* pipe = fopen("/proc/diskstats", "r");
    if (!pipe)
    {
        pipe = fopen("/proc/diskstats", "r");
    }
    if (!pipe)
    {
        GUI_FMT_DEBUG("查询mmc设备失败 错误码[-2]\n");
        return -2;
    }
    
    // fseek(pipe, -1024L, SEEK_END);
    while (fgets(buf, 1024, pipe) != NULL)
    
    {
        GUI_FMT_DEBUG("%s", buf);
        char *pfile = NULL;
        char *pfend = NULL;
        if ((pfile = strstr(buf, "mmc")) != NULL)
        {
            //pfile 向前移动到次设备号
            pfile -= 2;
            
            if(pfile[0] == '0')
            {
                pfile += 2;
                //确定以mmc开头的首个字符串
                pfend = pfile;
                offset = 0;
                while((pfend[offset] != ' '))
                {
                    offset++;
                    //offset 不可能大于50
                    if(offset > 50)
                    {
                        GUI_FMT_DEBUG("设备号长度大于50\n");
                        return -3;
                    }
                }
                has_mmc_device = 1;
                memcpy(device, pfile, offset);
             
                break;
            }
            else
            {
                continue;
            }
        }
        
        memset(buf, 0x0, sizeof(buf));
    }
    if(0 == has_mmc_device )
    {
        GUI_FMT_DEBUG("mmc设备未找到\n");
        return -4;
    }
    return 0;
}


int get_mmc_information(uint8 *result)
{
    if(result == NULL)
    {
        GUI_FMT_DEBUG("get_mmc_information() 入参为空指针");
        return -1;
    }
    int ret = 0;
    char *pdata = NULL;//指向要的数据，目前是指response 0 : 

    //指令需要改
    char mmc_cmd[512] = "cd /usr/bin/ && ./mmc read health /dev/";
    
    uint8 data[2014] = {0};
    int   datalen    = 0;
    char  tmp[MAX_RESULT_LEN + 1]  = {0};
    
    ret = get_root_mmc_device(device);
    if(ret != 0)
    {
        GUI_FMT_DEBUG("查询mmc设备失败, ret = %d\n", ret);
        return -2;
    }
    strcat(mmc_cmd, device);
    
    GUI_FMT_DEBUG("%d, %s \n", ret, mmc_cmd);

    ret = MmcCmdExecute(mmc_cmd, tmp);
    if(0 != ret)
    {
        GUI_FMT_DEBUG("./mmc 命令执行失败， ret = %d\n", ret);
        return -3;
    }

    pdata = strstr(tmp, "response 0 : \n");
    
    if(pdata != NULL)
    {
        pdata += strlen("response 0 : \n");
    }
    else
    {
        GUI_FMT_DEBUG("命令执行结果中无response 0, 认为./mmc 命令执行失败\n");
        return -3;
    }
    
    GUI_BUF_TRACE(pdata, strlen(pdata), "打印结果%s , %d \n", pdata, strlen(pdata));
    ret = StringHextoHex(pdata, data, &datalen);

    GUI_BUF_DEBUG(data, datalen, "转化成16进制 \n", data[24]);

    memcpy(result, data, datalen);
    
    return 0;
}
void disk_status_init(HDC hdc)
{
     dis_index = 0;
    data_flag =  get_mmc_information( mmc_result);
    GUI_FMT_DEBUG("device = %s \n", device);
}

void disk_status_draw0(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if(pg_guicomm == NULL)
       return;
    
    uint32 data = 0;
    char      czBuf[32];
    SelectFont(hdc,pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 20;
    rc.bottom = rc.top+16;
    char pcz[40];
    strcpy(pcz, "磁盘当前状态");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);

    int nTop = 40;
    int nLeft = 8;

    
    int space  = 10; // 间距 字段与值之间的间距
    int space2 = 16; // 间距 字段与字段之间的间距
    TextOut(hdc, nLeft, nTop, "Bad Block Manufacturer:");
    
    nTop += space;
    nLeft = 60;

    if(data_flag != 0 || calData(&mmc_result[24], 4, &data) != 0)
    {
         TextOut(hdc, nLeft, nTop, "---");
    }
    else
    {
         sprintf(czBuf, "%d", data);
         TextOut(hdc, nLeft, nTop, czBuf);
    }

    nTop += space2;
    nLeft = 8;
    TextOut(hdc, nLeft, nTop, "Bad Block Runtime System:");
    
    nTop += space;
    nLeft = 60;
    if(data_flag != 0 || calData(&mmc_result[28], 4, &data) != 0)
    {
         TextOut(hdc, nLeft, nTop, "---");
    }
    else
    {
         sprintf(czBuf, "%d", data);
         TextOut(hdc, nLeft, nTop, czBuf);
    }

    nTop += space2;
    nLeft = 8;
    TextOut(hdc, nLeft, nTop, "Bad Block Runtime MLC:");
    nTop += space;
    nLeft = 60;
    if(data_flag != 0 || calData(&mmc_result[36], 4, &data) != 0)
    {
         TextOut(hdc, nLeft, nTop, "---");
    }
    else
    {
         sprintf(czBuf, "%d", data);
         TextOut(hdc, nLeft, nTop, czBuf);
    }

    nTop += space2;
    nLeft = 8;
    TextOut(hdc, nLeft, nTop, "Max Erase Count EUDA:");
    nTop += space;
    nLeft = 60;
    if(data_flag != 0 || calData(&mmc_result[128], 4, &data) != 0)
    {
         TextOut(hdc, nLeft, nTop, "---");
    }
    else
    {
         sprintf(czBuf, "%d", data);
         TextOut(hdc, nLeft, nTop, czBuf);
    }

}

void disk_status_draw1(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if(pg_guicomm == NULL)
       return;
    
    uint32 data = 0;
    char      czBuf[32];
    SelectFont(hdc,pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 20;
    rc.bottom = rc.top+16;
    char pcz[40];
    strcpy(pcz, "磁盘当前状态");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);

    int nTop = 40;
    int nLeft = 8;

    
    int space  = 10; // 间距 字段与值之间的间距
    int space2 = 16; // 间距 字段与字段之间的间距

    TextOut(hdc, nLeft, nTop, "Min Erase Count EUDA:");
    nTop += space;
    nLeft = 60;
    if(data_flag != 0 || calData(&mmc_result[132], 4, &data) != 0)
    {
         TextOut(hdc, nLeft, nTop, "---");
    }
    else
    {
         sprintf(czBuf, "%d", data);
         TextOut(hdc, nLeft, nTop, czBuf);
    }

    nTop += space2;
    nLeft = 8;
    TextOut(hdc, nLeft, nTop, "Avg Erase Count EUDA:");
    nTop += space;
    nLeft = 60;
    if(data_flag != 0 || calData(&mmc_result[136], 4, &data) != 0)
    {
         TextOut(hdc, nLeft, nTop, "---");
    }
    else
    {
         sprintf(czBuf, "%d", data);
         TextOut(hdc, nLeft, nTop, czBuf);
    }

    nTop += space2;
    nLeft = 8;
    TextOut(hdc, nLeft, nTop, "Health Device Level EUDA:");
    nTop += space;
    nLeft = 60;
    if(data_flag != 0 || calData(&mmc_result[176], 4, &data) != 0)
    {
         TextOut(hdc, nLeft, nTop, "---");
    }
    else
    {
         sprintf(czBuf, "%d%%", data);
         TextOut(hdc, nLeft, nTop, czBuf);
    }


}

void disk_status_draw(HDC hdc)
{
    if(dis_index == 0)
    {
        disk_status_draw0(hdc);
    }
    else
    {
        disk_status_draw1(hdc);
    }
}

BOOL disk_status_Msg(int nKey)
{
     switch(nKey)
     {
     case KEY_UP:
         dis_index--;
         if(dis_index < 0)
             dis_index = 1;
         UpdateWindow(g_guiComm.hMainWnd, TRUE);
         return TRUE;

     case KEY_DOWN:
         dis_index ++;
         if(dis_index > 1)
             dis_index = 0;
         UpdateWindow(g_guiComm.hMainWnd, TRUE);
         return TRUE;

     default:
        return FALSE;
     }

     return FALSE;
}

PLUGIN_INFO g_disk_status=
{
    0x00B000F1, // 0x00B000F1
    disk_status_init,
    NULL,
    disk_status_draw,
    NULL,
    disk_status_Msg,
    NULL
};

#endif


#if DESC("交采版本显示",1)
void ac_ver_Init(DWORD dwPluginId)
{

}
extern char ac_minver[100];
extern char ac_data_ver[100];
extern char ac_ver[100];
extern char ac_meterIC_ver[100];

void ac_ver_Draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if(pg_guicomm == NULL)
        return;

    SelectFont(hdc,pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 30;
    rc.bottom = rc.top+16;
    char pcz[40];
    strcpy(pcz, "交采模块当前版本");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);
    
    int nTop = 60;
    int nLeft = 26;
    
    TextOut(hdc, nLeft, nTop, "模块版本：");

    nLeft = 90;
    memset(pcz, 0, 40);
    if (strlen(ac_ver) != 0)
    {
        TextOut(hdc, nLeft, nTop, ac_ver);
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "-----");
    }

    nTop = 80;
    nLeft = 26;
    
    TextOut(hdc, nLeft, nTop, "内部版本：");

    nLeft = 90;
    if (strlen(ac_minver) != 0)
    {
        TextOut(hdc, nLeft, nTop, ac_minver);
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "-----");
    }

    nTop = 100;
    nLeft = 26;
    
    TextOut(hdc, nLeft, nTop, "版本日期：");

    nLeft = 90;
    if (strlen(ac_data_ver) != 0)
    {
        TextOut(hdc, nLeft, nTop, ac_data_ver);
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "-----");
    }

#ifdef AREA_SHANDONG
    nTop = 120;
    nLeft = 26;
    
    TextOut(hdc, nLeft, nTop, "计量版本：");

    nLeft = 90;
    if (strlen(ac_meterIC_ver) != 0)
    {
        TextOut(hdc, nLeft, nTop, ac_meterIC_ver);
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "-----");
    }
#endif

}


PLUGIN_INFO g_layer_ac_ver=
{
    0x00A000FA, // 0x00A000FA
    ac_ver_Init,
    NULL,
    ac_ver_Draw,
    NULL,
    NULL,
    NULL
};

#endif

#if DESC("无线拨号方式设置", 1)
uint8 g_Wireless_status = 0;
uint8 g_Wireless_statusOld = 0;
uint8 g_WirelessIndex = 0;

//运营商
const char* g_WirelessStatusName[] =
{
    "USB",
    "串口"
};

void wirelessStatus_set_init(HDC hdc)
{
    if(0 == access("/data/app/wirelessDCM/data/bForceUart", F_OK))
    {
        g_Wireless_status = 1;
    }
    else
    {
        g_Wireless_status = 0;
    }
    g_WirelessIndex = 0;
    g_Wireless_statusOld = g_Wireless_status;
}

void wirelessStatus_set_draw(HDC hdc)
{
    int nTop = 24;
    int nLeft1 = 16;
    int nLeft2 = 60;
    int nLeft3 = 88;

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    TextOut(hdc, 24, nTop, "无线拨号方式");
    nTop += 20;
    TextOut(hdc, 48, nTop, "功能设置");

    SelectFont(hdc, pg_guicomm->fontSmall);
    nTop += 32;
    TextOut(hdc, nLeft1, nTop, "设置: ");
    if(0 == g_WirelessIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_WirelessStatusName[g_Wireless_status]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_WirelessStatusName[g_Wireless_status]);
    }

    SelectFont(hdc, pg_guicomm->fontBig);
    nTop += 32;
    if(1 == g_WirelessIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, 30, nTop, "返回");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 30, nTop, "返回");
    }

    if(2 == g_WirelessIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, "确认");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, "确认");
    }
    
    PopListDraw(hdc);
}

BOOL wirelessStatus_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
        
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;
        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            g_Wireless_status = nSel;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        return TRUE;
    }
    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_WirelessIndex > 0)
            {
                g_WirelessIndex--;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_WirelessIndex < 2)
            {
                g_WirelessIndex++;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            if(1 == g_WirelessIndex)
            {
                CancelMenu();
                return TRUE;
            }
            else if(TRUE == ValidatePwd())
            {
                if(0 == g_WirelessIndex)
                {
                    PopList(g_WirelessStatusName, 2, g_WirelessStatusName[g_Wireless_status], 80, 80, FALSE);
                    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                    return TRUE;
                }
                else if(2 == g_WirelessIndex)
                {
                    if(g_Wireless_status != g_Wireless_statusOld)
                    {
                        if(0 == g_Wireless_status)
                        {
                            system("rm /data/app/wirelessDCM/data/bForceUart");
                        }
                        else
                        {
                            system("touch /data/app/wirelessDCM/data/bForceUart");
                        }
                        MessageBox("设置成功\n拔插模块或重启终端生效！");
                    }
                    CancelMenu();
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

PLUGIN_INFO g_layer_wirelessStatus_set = {
    0x00A000FC/*id*/,
    wirelessStatus_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    wirelessStatus_set_draw/*Draw*/,
    NULL/*Timer*/,
    wirelessStatus_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

uint8 proxyip[4]={0};
uint16 proxyport = 0;
uint8 proxyindex;
const char* proxy_filepath = "/etc/comm_proxy/sysConfig.cfg";
const char* ipkey = "StationPrepositionIp";
const char* portkey = "StationPrepositionPort";

char * strim(char *str)
{
    char *strend,*sp,*ep;
    int len;
    sp = str;
    strend = str + strlen(str) - 1;
    ep = strend;
 
    while(sp<=strend && isspace(*sp))// *sp == ' '也可以
        sp++;
    while(ep>=sp && isspace(*ep))
        ep--;
    len = (ep < sp) ? 0:(ep-sp)+1;//(ep < sp)判断是否整行都是空格
    sp[len] = '\0';
    return sp;

}

//将字符串形式的ip转换为数组 失败返回false
BOOL commfun_IP_unpack(char* ipin, uint8* ipout)
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
    for (int i =0;i<4;i++){
        if(atoi(p1[i]) > 255)
            return FALSE;
        ipout[i] = atoi(p1[i]);
    }
    return TRUE;
}

//从配置文件中获取ip和端口
int get_proxy_ip_port()
{
    FILE *fp;
    char buf[512] = {0};
    uint8 count = 0;
    char* tmp = NULL;
    fp = fopen(proxy_filepath,"r+");
    if(NULL == fp)
    {
        GUI_FMT_DEBUG("打开通信代理配置文件失败 %s ！\n",proxy_filepath);
        return -1;
    }
    while(fgets(buf,sizeof(buf),fp)!=NULL)
    {
        tmp = strstr(buf,ipkey);
        if(tmp!=NULL)
        {
            tmp+=strlen(ipkey);
            tmp++;//偏移一个等号
            //必须要去掉最后的换行符
            if(TRUE == commfun_IP_unpack(strim(tmp),proxyip))
                GUI_FMT_DEBUG("获取ip成功 %d.%d.%d.%d ！\n",proxyip[0],proxyip[1],proxyip[2],proxyip[3]);
            count++;
            continue;
        }
        tmp = strstr(buf,portkey);
        if(tmp!=NULL)
        {
            tmp+=strlen(portkey);
            tmp++;
            proxyport = atoi(tmp);
            GUI_FMT_DEBUG("获取port成功 %d！\n",proxyport);
            count++;
        }
        if(count==2)
            break;
    }
    fclose(fp);
    return 0;
}

void set_proxy_ip_port()
{
    FILE *fp;
    char buf[512] = {0};
    char str[100][512] = {0};
    uint16 i=0;
    uint16 linenum;

    fp = fopen(proxy_filepath,"r+");
    if(NULL == fp)
    {
        GUI_FMT_DEBUG("打开通信代理配置文件失败 %s ！\n",proxy_filepath);
        return ;
    }
    while(fgets(buf,sizeof(buf),fp)!=NULL)
    {
        if(strstr(buf,ipkey)!=NULL)
        {
            sprintf(str[i],"%s=%d.%d.%d.%d\n",ipkey,proxyip[0],proxyip[1],proxyip[2],proxyip[3]);
        }else if(strstr(buf,portkey)!=NULL)
        {
            sprintf(str[i],"%s=%d\n",portkey,proxyport);
        }else
        {
            strcpy(str[i],buf);
        }
        i++;
        GUI_FMT_DEBUG("i %d %s ！\n",i,buf);
        if(i>=100)
            break;
    }
    linenum = i;
    fseek(fp,0,SEEK_SET);
    for(i=0;i<linenum;i++)
    {
        fputs(str[i],fp);
    }
    fclose(fp);
}

void comm_proxy_Init(DWORD dwPluginId)
{
#ifndef AREA_HUNAN
    return;
#endif
    get_proxy_ip_port();
    g_bpop = TRUE;
}

void comm_proxy_Draw(HDC hdc)
{

#ifndef AREA_HUNAN
    MessageBox("非湖南版本\n不支持修改通信代理配置！");
    CancelMenu();
    return;
#endif
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
    uint8 index = 0;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    char czBuf[300];
    int nTop = 40;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "主站IP");
    sprintf(czBuf,"%d.%d.%d.%d",proxyip[0],proxyip[1],proxyip[2],proxyip[3]);
    if(proxyindex == index)
    {
        ITEM_SELECT
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT
    }else
        TextOut(hdc, nLeft2, nTop, czBuf);

    index++;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "主站端口");
    sprintf(czBuf,"%d",proxyport);
    
    if(proxyindex == index)
    {
        ITEM_SELECT
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT
    }else
        TextOut(hdc, nLeft2, nTop, czBuf);

    index++;
    nTop+=40;
    if(proxyindex == index)
    {
        ITEM_SELECT
        TextOut(hdc, nLeft1, nTop, "取消");
        ITEM_NOT_SELECT
    }else
        TextOut(hdc, nLeft1, nTop,  "取消");

    index++;
    nLeft2 = 80;
    if(proxyindex == index)
    {
        ITEM_SELECT
        TextOut(hdc, nLeft2, nTop, "保存");
        ITEM_NOT_SELECT
    }else
        TextOut(hdc, nLeft2, nTop,  "保存");

}
void proxyConfigReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[4] = { 0 };
    switch(proxyindex)
    {
    case 0:
    {
        if(FALSE == commfun_IP_unpack(pNewStr,tmp))
        {
            if (TRUE == MessageBox("IP地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                sprintf(czBuf,"%d.%d.%d.%d",proxyip[0],proxyip[1],proxyip[2],proxyip[3]);
                InputBox(czBuf, proxyConfigReturn, NULL, 0x02);
                
            }
        }else
        {
            memcpy(proxyip,tmp,4);
        }
    }
        break;
    case 1:
    {
        int port = atoi(pNewStr);
        if(port<=65535)
        {
            proxyport = port;
        }else
        {
            if (TRUE == MessageBox("端口格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                sprintf(czBuf, "%d", proxyport);
                InputBox(czBuf, proxyConfigReturn, NULL, 0x01);
            }
        }
    }
        break;
    }

}
 BOOL comm_proxy_Msg(int nKey)
 {
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int maxindex = 3;
    char czBuf[20];
    switch(nKey)
    {
    case KEY_UP:
    case KEY_LEFT:
    {
        if(proxyindex>0)
            proxyindex--;
         else
            proxyindex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        if (proxyindex < maxindex)
            proxyindex++;
        else
            proxyindex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if(ValidatePwd() == TRUE)
        {
            switch(proxyindex)
            {
            case 0://IP
            {
                sprintf(czBuf,"%d.%d.%d.%d",proxyip[0],proxyip[1],proxyip[2],proxyip[3]);
                InputBox(czBuf, proxyConfigReturn, NULL, 0x02 );
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
            case 1://port
            {
                sprintf(czBuf, "%d", proxyport);
                InputBox(czBuf, proxyConfigReturn, NULL, 0x01);
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
            case 2://取消
            {
                CancelMenu();
                proxyindex = 0;
                return TRUE;
            }
            case 3: //保存
            {
                set_proxy_ip_port();
                proxyindex =0;
                PopMsg("       设置成功!\n     正在重启...");
                GUI_FMT_DEBUG("修改完成 重启终端\n");
                RebootTerminal();
                return TRUE;
            }
            }
        }
    }

    }

        
    return FALSE;
 }
PLUGIN_INFO g_layer_comm_proxy=
{
    0x00A000FB, // 0x00A000FB
    comm_proxy_Init,
    NULL,
    comm_proxy_Draw,
    NULL,
    comm_proxy_Msg,
    NULL
};

#define MAX_METER_INFONUM   3600   //档案的最大信息点号
#define MAX_TASK_INFONUM 1000       //任务最大信息点号
#define MAX_PLAN_INFONUM 1000       //方案最大信息点号
#define MAX_SHAANXI_SELECT 9        //陕西默认9种上行通讯参数

const OOP_OAD_U MeterFileOad = { 0x60000200 };
const OOP_OAD_U NormalPlanOad = { 0x60140200 };
const OOP_OAD_U TaskOad = { 0x60120200 };
const OOP_OAD_U EthIPOad = { 0x45100400 };
const OOP_OAD_U EthConfigOad = { 0x45100200 };
const OOP_OAD_U EthMasterOad = { 0x45100300 };
//const OOP_OAD_U GPRSConfigOad = { 0x45000200 };
//const OOP_OAD_U GPRSMasterOad = { 0x45000300 };
const OOP_OAD_U SecuConfigOad = { 0xF1010200 };
const OOP_OAD_U RS485Oad = { 0xF2010202 };
const char* g_pczRadioDelay[] = { "100", "200", "300", "400", "500", "800", "1000", "2000" };
//电表波特率
const char* g_baud[] = { "300bps","600bps","1200bps","2400bps","4800bps","7200bps","9600bps","19200bps","38400bps","57600bps","115200bps","自适应" };

//规约类型
const char* g_prtl[] = { "未知","DL/T 645-1997","DL/T 645—2007","DL/T 698.45","CJ/T 188—2004" };

//TI格式时间间隔
const char* g_ti[] = { "秒","分","时","日","月","年" };

//方案类型
const char* g_plantype[] = { "普通采集方案","事件采集方案","透明方案","上报方案","脚本方案" };

//普通采集类型
const char* g_normalPlanType[] = {"采集当前数据", "采集上第N次", "按冻结时标采集", "按时标间隔采集", "补抄"};

//MS集合
const char* g_ms[] = {"无表计", "全部用户地址", "一组用户类型", "一组用户地址", "一组配置序号", "一组用户类型区间", "一组用户地址区间", "一组配置序号区间"};

//存储时标
const char* g_tmFlag[] = {"未定义", "任务开始时间", "相对当日0:0", "相对上日23:59", "相对上日0:0", "相对当月1日0:0", "数据冻结时标", "相对上月末23:59"};

//区间类型
const char* g_regionType[] = {"前闭后开", "前开后闭", "前闭后闭", "前开后开"};

//事件采集类型
const char* g_eventPlanType[] = {"周期采集", "根据通知采集所有", "根据通知采集指定", "根据通知存储"};

//上报标识
const char* g_eventReportFlag[] = {"不上报", "立即上报"};

//上报方案类型
const char* g_reportPlanType[] = {"对象属性", "记录型对象属性"};

//上报方案RSD
const char* g_reportPlanRSD[] = {"不选择", "选择方法1",  "选择方法2",  "选择方法3",  "选择方法4",  "选择方法5",  "选择方法6",
                                 "选择方法7",  "选择方法8",  "选择方法9",  "选择方法10"};

//端口
const char* g_pipe[] = { "载波-1","载波-2","485-1","485-2","485-3","485-4","485-5","485-6","485-7","485-8","交采" };

//开关
const char* g_on_off[] = { "关闭","开启" };

//ip配置方式
const char* g_ipconfig[] = { "DHCP","静态","PPPoE" };

//通信配置工作模式
const char* g_work_mode[] = { "混合模式","客户机模式","服务器模式" };

//运营商代码
const char* g_operator_seclct[] = { "中国移动","中国电信","中国联通","未知" };

//网络类型
const char* g_net_mode[] = { "2G","3G","4G","5G","未知"};

//鉴权方式
const char* g_auth_mode[] = { "null","chap","pap","pap&chap"};

//连接方式
const char* g_conn_type[] = { "TCP","UDP" };

//连接应用方式
const char* g_conn_app_type[] = { "主备模式","多连接模式" };

//在线方式
const char* g_line_type[] = { "永久在线","被动激活" };

//载波频点选择设置或者查询
const char* g_Hplc_type[] = { "设置载波频点","查询载波频点" };

//终端地址选择显示或者设置
const char* g_Addr_type[] = { "显示模式","设置模式" };

const char*  g_sensor_name_value[]={"温湿度传感器","水浸传感器","烟雾传感器","桩头温度传感器"};

//端口设置类型
const char* g_Port_type[] = { "自适应","远程通信","抄表" };

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
    OOP_WORD3V_T        vol;             //电压       0x20000200
    OOP_WORD3V_T        xj;              //相角
    OOP_INT3V_T         cur;             //电流       0x20010200
    OOP_INT4V_T         ppower;          //有功功率     0x20040200
    OOP_INT4V_T         npower;          //无功功率     0x20050200
    OOP_LONG4V_T        fpower;          //功率因数     0x200A0200
    OOP_DEMAND_T        pdemand;         //正向有功需量 0x10100200
    OOP_DEMAND_T        ndemand;         //反向有功需量 0x10200200
    OOP_HARMONIC_T      harmonicV[3];    //三相电压谐波含有率0x200D0200  0 x200D0400
    OOP_HARMONIC_T      harmonicC[3];    //三相电流谐波含有率0x200E0200  0 x200E0400
    OOP_HARMONIC_U_VALUE_T      harmonicV_value[3];    //三相电压谐波含量0x20330200  0 x20330400
    OOP_HARMONIC_I_VALUE_T      harmonicC_value[3];    //三相电流谐波含量0x20340200  0 x20340400
    OOP_CHINESEINFOS_T         chinese_info;
    OOP_SWITCHOUT_T     switchout;
    OOP_SECURITYFLAG_E  safemode;
    OOP_DATETIME_S_T    memtTime;
}DISPLAY_DATA_INFO;

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
}DISPLAY_DAY_FROZEN_INFO;

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

const char* g_showTitleVoltag34[] = { "Ua", "Ub", "Uc" };
const char* g_showTitleEelectric[] = { "Ia", "Ib", "Ic" };
const char* g_showTitleXj[] = { "A电压相角", "B电压相角", "C电压相角" };
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
    int32            plus_active_power;         //脉冲有功功率 0x24010500
    int32            plus_reactive_power;       //脉冲无功功率 0x24010600
    int64            gruop_active_power;        //总加组有功功率 0x23010300
    int64            gruop_reactive_power;      //总加组无功功率 0x23010400
    OOP_HENERGYS_T   gruop_day_energy;        //总加组日有功电量 0x23010700
    OOP_HENERGYS_T   gruop_day_ractenergy;    //总加组日无功电量 0x23010800
    OOP_HENERGYS_T   gruop_month_energy;      //总加组月有功电量 0x23010900
    OOP_HENERGYS_T   gruop_month_ractenergy;  //总加组月无功电量 0x23010a00
    OOP_BITSTR8_T    ctrlPwTurn;              //0x230x0e00 总加组功控轮次配置  控制读取
    OOP_BITSTR8_T    ctrlEcTurn;              //0x230x0f00 总加组电控轮次配置  控制读取
    OOP_TGSETTING_T  ctrl_seting;             //控制设置状态         0x23011000
    OOP_TGCURSTATE_T ctrl_status;             //控制当前状态         0x23011100
    OOP_PULSESETS_T  plus_param;              //脉冲配置       0x24010400
    OOP_PULSESETS_B_T plus_b_param;           //脉冲配置       0x24011C00
    OOP_OCTETVAR16_T pulse_addr;              //脉冲地址       0x24010200
    OOP_TGSETS_T     gruop_param;             //脉冲配置       0x23010200
}CTRL_DATA_INFO;

CTRL_DATA_INFO g_ctrl_data[8];
CTRL_DATA_INFO g_group_data;
uint8            eletype;                 //保电
OOP_8107_CONFIG_T tBuyenergyconfig;
uint8            g_bGuiBuyLastData = 0;
uint8            g_bGuiBuyLastType = 0;
int64            g_GuiBuyLast = 0;
uint8            g_bGuiBuyEndData = 0;
int64            g_GuiBuyEnd = 0;
int64      g_wallet[8];

DISPLAY_DATA_INFO display_data;
DISPLAY_DATA_INFO display_datas;
DISPLAY_DAY_FROZEN_INFO display_day_frozen;

HIGH_PRECISION_DATA_INFO g_high_precision_data;
OOP_OAD_U GPRSConfigOad;
OOP_OAD_U GPRSMasterOad;
MOD_INFO_ACK_T g_module_info[5];


#if DESC("终端地址",1)

int get_addr(uint8* Addr)
{
    int   ret;
    uint32 len = 0;
    NOMAL_OAD_T NormalOad;
    
    if(Addr == NULL)
    {
        return -1;
    }
    
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x40010200;
    NormalOad.infoNum = 0;
    OOP_OCTETVAR16_T LocalAddr;
    memset(&LocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
    ret = db_read_nomal(g_hUdbCenter, &NormalOad, sizeof(OOP_OCTETVAR16_T), (uint8*)&LocalAddr, &len);
    if (ret != 0 || len != sizeof(OOP_OCTETVAR16_T))
        return -1;
    memcpy(Addr, LocalAddr.value, LocalAddr.nNum);
    printf("终端地址 0x%02x%02x%02x%02x%02x%02x \n", Addr[5], Addr[4], Addr[3], Addr[2], Addr[1], Addr[0]);
    return LocalAddr.nNum;
}
#endif


int get_clientno(uint8* Addr)
{

    int   ret;
    uint32 len = 0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x40030200;
    NormalOad.infoNum = 0;
    OOP_OCTETVAR16_T LocalAddr;
    memset(&LocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
    ret = db_read_nomal(g_hUdbCenter, &NormalOad, sizeof(OOP_OCTETVAR16_T), (uint8*)&LocalAddr, &len);
    if (ret != 0 || len != sizeof(OOP_OCTETVAR16_T))
    {
        return -1;
    }
    memcpy(Addr, LocalAddr.value, 6);
    printf("客户编号 0x%02x%02x%02x%02x%02x%02x \n", Addr[5], Addr[4], Addr[3], Addr[2], Addr[1], Addr[0]);
    return LocalAddr.nNum;
}

/**
*********************************************************************
* @brief：      福建特殊需求
* @param[in]： OOP_GPRSCONFIG_T *pGprsConfig   配置
* @param[out]：OOP_GPRSCONFIG_T *pGprsConfig   新配置
* @return：     TRUE-用户名密码调整/FALSE-未调整
               <0            - 错误码
*********************************************************************
*/
uint8 grps_fujian_apn(OOP_GPRSCONFIG_T *pGprsConfig)
{
    if(0 == strcmp("fjep.fj", pGprsConfig->apn.apn.value) || 0 == strcmp("FJEP.FJ", pGprsConfig->apn.apn.value))
    {
        int   addrlen = 0;
        uint8 pAddr[16] = {0};
        int   i = 0;
        OOP_VISIBLEVAR_T username;
        OOP_VISIBLEVAR_T pwd;

        /* 密码取终端地址 */
        memset(&pwd, 0, sizeof(OOP_VISIBLEVAR_T));
        addrlen = get_clientno(pAddr);
        if(addrlen <= 0)
        {
            memset(pwd.value, '0', 12);
            pwd.nNum = 12;
        }
        else
        {
            for(i = 0; i < addrlen; i++)
            {
                pwd.value[2 * (addrlen - i) - 2] = (pAddr[i] & 0x0f) + '0';
                pwd.value[2 * (addrlen - i) - 1] = ((pAddr[i] >> 4) & 0x0f) + '0';
                pwd.nNum += 2;
            }
        }
        
        /* 用户名 */
        memset(&username, 0, sizeof(OOP_VISIBLEVAR_T));
        MEMCPY_R(username.value, pwd.value, pwd.nNum);
        username.nNum = pwd.nNum;

        if(0 != memcmp(&username, &(pGprsConfig->apn.username), sizeof(OOP_VISIBLEVAR_T)) || 
           0 != memcmp(&pwd, &(pGprsConfig->apn.pwd), sizeof(OOP_VISIBLEVAR_T)))
        {
            memcpy(&(pGprsConfig->apn.username), &username, sizeof(OOP_VISIBLEVAR_T));
            memcpy(&(pGprsConfig->apn.pwd), &pwd, sizeof(OOP_VISIBLEVAR_T));
            return TRUE;
        }
        
    }
    return FALSE;
}

//状态栏 信息点号处理
void Uninit()
{
    set_gui_infonum(0);
}
    
#if DESC("数据中心读写",1)

int write_normal_data(uint8* DataBuf, uint16 DataLen, OOP_OAD_U Oad, uint16 infonum, uint8 logicid, uint8 classtag)
{
    int ret;
    NOMAL_OAD_T NomalOad;
    NomalOad.logicId = logicid;
    NomalOad.oad = Oad;
    NomalOad.classtag = classtag;
    NomalOad.infoNum = infonum;

    ret = db_write_nomal(g_hUdbCenter, &NomalOad, DataBuf, DataLen);
    if (ret != 0)
    {
        GUI_FMT_DEBUG("普通数据写入数据库失败oad %08x  ret = %d\n", Oad.value, ret);
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
        GUI_FMT_DEBUG("db_read_nomal  err ret=%d, recNum=%d, buf_len=%u , oad_value = %08x!\n", ret, len, buf_len, oad_value);
        return -1;
    }

    return 0;
}

/**********************************************************************
* @name      : app_get_meter_list
* @brief     ：获取档案数据
* @param[in] ：i        - 档案序号
* @param[out]：oopMeter - 档案
* @return    : 错误码
* @Create    : 
* @Date      ：2019-12-16
* @Update    :
**********************************************************************/
int app_get_meter_list(uint32 infoNum, OOP_METER_T *oopMeter)
{
    return read_db_noamal_data(g_hUdbCenter, 0x60000200, 0, (uint16)infoNum, (uint8*)oopMeter, sizeof(OOP_METER_T));

}

OOP_METER g_gruop_oopMeter;

void appdb_meter_list_record_init(int clientid)
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

    memset(&g_gruop_oopMeter, 0, sizeof(OOP_METER));
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

            g_gruop_oopMeter.nValue[oopMeterNum] = oopMeter.nIndex;
            //档案序号从1开始
            g_gruop_oopMeter.oopMeter[oopMeterNum++] = oopMeter;
            
        }
    }
    db_read_record_end(clientid, handle);

    /* 记录档案数量 */
    g_gruop_oopMeter.nNum = oopMeterNum;

    return;
}

void groupPointReturn(char* pNewStr, void* pPara)
{
    uint32 nPoint = atoi(pNewStr);
    bool flag = false;
    if(nPoint >= VIR_BEGIN)
    {
        char bzbuf[100];
        sprintf(bzbuf, "测量点最大为%d\n确认是否重新输入?", (VIR_BEGIN-1));
        if(TRUE == MessageBox(bzbuf, MB_OKCANCEL))
        {
            sprintf(bzbuf, "%d", g_gruop_oopMeter.nValue[g_guiComm.nCurPoint]);
            InputBox(bzbuf, groupPointReturn, NULL, 0x01);
        }
    }else{
        for (uint32 i = 0; i < g_gruop_oopMeter.nNum; i++){
            if(g_gruop_oopMeter.nValue[i] == nPoint){
                g_guiComm.nCurPoint = i;
                flag = true;
                g_gruop_oopMeter.nValue[g_guiComm.nCurPoint] = nPoint;
            }
        }
        if (flag == false){
            char bzbuf[100];
            sprintf(bzbuf, "%d不在档案中\n确认是否重新输入?", nPoint);
            if(TRUE == MessageBox(bzbuf, MB_OKCANCEL))
            {
                sprintf(bzbuf, "%d", g_gruop_oopMeter.nValue[g_guiComm.nCurPoint]);
                InputBox(bzbuf, groupPointReturn, NULL, 0x01);
            }
       }
    }
}

int read_db_record_data(DB_CLIENT fp, uint32 oad_value, uint8 logic_id, uint16 info_num, uint8* pbuf, uint32 buf_len)
{
    int                ret = ERR_NORMAL;
    uint32             len     = 0;
    time_t             timep = 0;

    READ_RECORD_T      inReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_METER_T        oopMeter;
    NOMAL_OAD_T        nomalOAD;
    OOP_OCTETVAR16_T   m_tmnAddr;

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    time(&timep); //获取从1970至今过了多少秒，存入time_t类型的timep

    inReadRecord.recordOAD.logicId = logic_id;
    inReadRecord.recordOAD.infoNum = info_num;
    inReadRecord.cType = COL_TM_END;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = timep + COMPENSATION_TIME;
    inReadRecord.sortType = DOWN_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = oad_value;

    inReadRecord.MAC.nNum = 6;

    if(info_num == 0)
    {
        memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
        nomalOAD.logicId = logic_id;
        nomalOAD.oad.value = 0x40010200;
        ret = db_read_nomal(fp, &nomalOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&m_tmnAddr, &len);
        if(0 != ret || len != sizeof(OOP_OCTETVAR16_T))
        {
            GUI_FMT_DEBUG(" db_read_nomal 0x40010200 err = %d!\n", ret);
            return -1;
        }
         memcpy(inReadRecord.MAC.value, m_tmnAddr.value, 6);
    }
    else
    {
        if (app_get_meter_list(info_num, &oopMeter) != ERR_OK)
        {
            GUI_FMT_DEBUG(" app_get_meter_list %d !\n", info_num);
            return -1;
        }
        memcpy(inReadRecord.MAC.value, oopMeter.basic.tsa.add, 6);
    }

    ret = db_read_record(fp, &inReadRecord, buf_len, &stepRecordOut);
    if (ret != ERR_OK)
    {
        GUI_FMT_DEBUG(" db_read_record %08x ret=%d !\n", oad_value, ret);
        return -1;
    }
    memcpy(pbuf, stepRecordOut.record[0].outDataBuf, buf_len);

    return 0;
}

int read_db_record_data_main(DB_CLIENT fp, uint32 oad_main, uint32 oad_value, uint8 logic_id, uint16 info_num, uint8* pbuf, uint32 buf_len)
{
    int                ret = ERR_NORMAL;
    uint32             len     = 0;
    time_t             timep = 0;

    READ_RECORD_T      inReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_METER_T        oopMeter;
    NOMAL_OAD_T        nomalOAD;
    OOP_OCTETVAR16_T   m_tmnAddr;

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    time(&timep); //获取从1970至今过了多少秒，存入time_t类型的timep

    inReadRecord.recordOAD.logicId = logic_id;
    inReadRecord.recordOAD.infoNum = info_num;
   inReadRecord.cType = COL_TM_END;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = timep + COMPENSATION_TIME;
    inReadRecord.sortType = DOWN_SORT;
    inReadRecord.recordOAD.optional = 1;
    inReadRecord.recordOAD.road.oadMain.value = oad_main;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = oad_value;

    inReadRecord.MAC.nNum = 6;

    if(info_num == 0)
    {
        memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
        nomalOAD.logicId = logic_id;
        nomalOAD.oad.value = 0x40010200;
        ret = db_read_nomal(fp, &nomalOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&m_tmnAddr, &len);
        if(0 != ret || len != sizeof(OOP_OCTETVAR16_T))
        {
            GUI_FMT_DEBUG(" db_read_nomal 0x40010200 err = %d!\n", ret);
            return -1;
        }
         memcpy(inReadRecord.MAC.value, m_tmnAddr.value, 6);
    }
    else
    {
        if (app_get_meter_list(info_num, &oopMeter) != ERR_OK)
        {
            GUI_FMT_DEBUG(" app_get_meter_list %d !\n", info_num);
            return -1;
        }
        memcpy(inReadRecord.MAC.value, oopMeter.basic.tsa.add, 6);
    }

    ret = db_read_record(fp, &inReadRecord, buf_len, &stepRecordOut);
    if (ret != ERR_OK)
    {
        GUI_FMT_DEBUG(" db_read_record %08x ret=%d !\n", oad_value, ret);
        return -1;
    }
    memcpy(pbuf, stepRecordOut.record[0].outDataBuf, buf_len);

    return 0;
}

int read_db_record_6e20(DB_CLIENT fp, uint16 id, uint8* pbuf, uint32 buf_len)
{
    int                ret = ERR_NORMAL;
    time_t             timep = 0;

    READ_RECORD_T      inReadRecord;
    RECORD_UNIT_T      stepRecordOut;


    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    time(&timep); //获取从1970至今过了多少秒，存入time_t类型的timep

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = id;
    inReadRecord.cType = COL_TM_START;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = timep;
    inReadRecord.sortType = DOWN_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6e200200;

    ret = db_read_record(fp, &inReadRecord, buf_len, &stepRecordOut);
    if (ret != ERR_OK)
    {
        GUI_FMT_DEBUG(" db_read_record 0x6e200200 ret=%d !\n", ret);
        return -1;
    }
    memcpy(pbuf, stepRecordOut.record[0].outDataBuf, buf_len);

    return 0;
}


#endif
    
//#if DESC("交采数据自动更新",1)
////extern OOP_SWITCHIN_STATE_T    switchState;    //遥信状态
////extern OOP_SWITCHIN_FLAG_T     switchFlag;     //接入标志
//
////交采数据自动更新
//typedef struct 
//{
//    uint32  oadValue;
//    uint32  dateLen;
//    uint8   *pBuf;
//    uint8   *pvaild;
//}AcDataInfo;
//
////交采数据自动更新列表
//AcDataInfo g_ac_data_list[] = 
//{/*     数据标识,   数据长度,     数据内容 */
//    {0x00100200,    sizeof(OOP_ENERGY_T),                  (uint8 *)&display_data.penergy},
//    {0x00300200,    sizeof(OOP_ENERGYS_T),                 (uint8 *)&display_data.penergys},
//    {0x00200200,    sizeof(OOP_ENERGY_T),                  (uint8 *)&display_data.nenergy},
//    {0x00400200,    sizeof(OOP_ENERGYS_T),                 (uint8 *)&display_data.nenergys},
//    {0x00500200,    sizeof(OOP_ENERGY_T),                  (uint8 *)&display_data.quadrant1},
//    {0x00600200,    sizeof(OOP_ENERGY_T),                  (uint8 *)&display_data.quadrant2},
//    {0x00700200,    sizeof(OOP_ENERGY_T),                  (uint8 *)&display_data.quadrant3},
//    {0x00800200,    sizeof(OOP_ENERGY_T),                  (uint8 *)&display_data.quadrant4},
//    {0x20000200,    sizeof(OOP_WORD3V_T),                  (uint8 *)&display_data.vol},
//    {0x20010200,    sizeof(OOP_INT3V_T),                   (uint8 *)&display_data.cur},
//    {0x20040200,    sizeof(OOP_INT4V_T),                   (uint8 *)&display_data.ppower},
//    {0x20050200,    sizeof(OOP_INT4V_T),                   (uint8 *)&display_data.npower},
//    {0x200A0200,    sizeof(OOP_LONG4V_T),                  (uint8 *)&display_data.fpower},
//    {0x10100200,    sizeof(OOP_DEMAND_T),                  (uint8 *)&display_data.pdemand},
//    {0x10200200,    sizeof(OOP_DEMAND_T),                  (uint8 *)&display_data.ndemand},
////    {0xf2030200,    sizeof(OOP_SWITCHIN_STATE_T),          (uint8 *)&switchState},
////    {0xf2030400,    sizeof(OOP_SWITCHIN_FLAG_T),           (uint8 *)&switchFlag},
//
//    {0x00100400,    sizeof(OOP_HENERGY_T),                  (uint8 *)&g_high_precision_data.penergy},
//    {0x00300400,    sizeof(OOP_HENERGYS_T),                 (uint8 *)&g_high_precision_data.penergys},
//    {0x00200400,    sizeof(OOP_HENERGY_T),                  (uint8 *)&g_high_precision_data.nenergy},
//    {0x00400400,    sizeof(OOP_HENERGYS_T),                 (uint8 *)&g_high_precision_data.nenergys},
//    {0x00500400,    sizeof(OOP_HENERGY_T),                  (uint8 *)&g_high_precision_data.quadrant1},
//    {0x00600400,    sizeof(OOP_HENERGY_T),                  (uint8 *)&g_high_precision_data.quadrant2},
//    {0x00700400,    sizeof(OOP_HENERGY_T),                  (uint8 *)&g_high_precision_data.quadrant3},
//    {0x00800400,    sizeof(OOP_HENERGY_T),                  (uint8 *)&g_high_precision_data.quadrant4},
//};
//
//uint16 g_ac_data_num = sizeof(g_ac_data_list)/sizeof(AcDataInfo);
//
//
///**********************************************************************
//* @name      : update_ac_data
//* @brief     : 更新交采数据
//* @param[in] ：int clientid    socket句柄
//               uint8 logicId   逻辑地址
//               uint16 infoNum  信息点号
//               uint32 value    oad值
//* @param[out]：
//* @return    ：
//* @Create    : 梅安宁
//* @Date      ：2020-07-31
//* @Update    :
//**********************************************************************/
//int update_ac_data(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
//{
//    int                i = 0;
//    int                ret = 0;
//    uint32             readlen = 0;
//    uint8              pDataBuf[1024] = {0};
//    NOMAL_OAD_T        nomalOAD;
//
//    memset(&nomalOAD, 0, sizeof(NOMAL_OAD_T));
//
//    nomalOAD.logicId = logicId;
//    nomalOAD.infoNum = infoNum;
//    nomalOAD.oad.value = value;
//
//    for(i=0; i<g_ac_data_num; i++)
//    {
//        if(g_ac_data_list[i].oadValue == value)
//        {
//            ret = db_read_nomal(clientid, &nomalOAD, 1024, pDataBuf, &readlen);
//            if (ret != ERR_OK || readlen == 0 || (g_ac_data_list[i].dateLen != readlen))
//            {
//                GUI_FMT_DEBUG("Updata logicId=%u, infoNum=%u, OAD=%08X  error ret=%d, len=%u, dateLen=%u\n", logicId, infoNum, value, ret, readlen, g_ac_data_list[i].dateLen);
//                return ret;
//            }
//            
//            memcpy(g_ac_data_list[i].pBuf, pDataBuf, readlen);
//            GUI_FMT_TRACE("Updata Nomal logicId=%u, infoNum=%u, OAD=%08X  OK !\n", logicId, infoNum, value);
//            GUI_BUF_TRACE(pDataBuf, readlen, "buf:");
//            return 0;
//        }
//        
//    }
//    
//    return -1;
//}
//#endif
//    
#if DESC("轮次状态",1)
//void g_turnstart_init(DWORD dwPluginId)
//{
//    memset(&display_data.switchout, 0, sizeof(OOP_SWITCHOUT_T));
//
//    read_db_noamal_data(g_hUdbCenter, 0xF2050200, 0, 0, (uint8*)&display_data.switchout, sizeof(OOP_SWITCHOUT_T));
//}
//void g_turnstart_draw(HDC hdc)
//{
//    ITEM_NOT_SELECT;
//    SelectFont(hdc, g_guiComm.fontBig);
//
//    uint32 i = 0;
//    char czBuf[100] = { 0 };
//    int nLeft = 5;
//    int nTop = 22;
//    uint8 turnMax = 0;
//    char* turnName[] = { (char*)"一轮", (char*)"二轮", (char*)"三轮", (char*)"四轮",
//                        (char*)"五轮", (char*)"六轮", (char*)"七轮", (char*)"八轮" };
//    RECT rc;
//
//    rc.left = 0;
//    rc.right = g_guiComm.nWidth;
//    sprintf(czBuf, "%s", "轮次状态");
//    rc.top = 22;
//    rc.bottom = rc.top + 16;
//    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
//
//    nTop += 24;
//    appctrl_get_devturn(&turnMax);
//    SelectFont(hdc, g_guiComm.fontSmall);
//    for (i = 0; i < turnMax; i++)
//    {
//        if (i >= display_data.switchout.nNum)
//        {
//            sprintf(czBuf, "%s:   --    --", turnName[i]);
//        }
//        else
//        {
//            if (0 == display_data.switchout.value[i].input)
//            {
//                sprintf(czBuf, "%s:   %s  %s", turnName[i],
//                    0 == display_data.switchout.value[i].current ? "合闸" : "跳闸",
//                    0 == display_data.switchout.value[i].attribute ? "脉冲式" : "保持式");
//            }
//            else
//            {
//                sprintf(czBuf, "%s:   --    --", turnName[i]);
//            }
//        }
//        TextOut(hdc, nLeft, nTop, czBuf);
//        nTop += 20;
//    }
//}
//
////轮次状态
//PLUGIN_INFO g_turnstart = {
//    0x11010012/*id*/,
//    g_turnstart_init/*InitLayer*/,
//    NULL/*UninitLayer*/,
//    g_turnstart_draw/*Draw*/,
//    NULL/*Timer*/,
//    NULL/*keymsg*/,
//    NULL/*ipc*/
//};
#endif
    
//#if DESC("安全模式",1)
//uint8 SafeConfigIndex;
////初始化安全模式
//void g_safemode_init(DWORD dwPluginId)
//{
//    memset(&display_data.safemode, 0, sizeof(OOP_SECURITYFLAG_E));
//
//    int ret = read_db_noamal_data(g_hUdbCenter, 0xF1010200, 0, 0, (uint8*)&display_data.safemode, sizeof(OOP_SECURITYFLAG_E));
//    printf("ret=%d, mod %u", ret, display_data.safemode);
//
//    SafeConfigIndex = 0;
//}
////绘制安全模式界面
//void g_safemode_draw(HDC hdc)
//{
//    GUI_COMM* pg_guicomm = &g_guiComm;
//    if (pg_guicomm == NULL)
//        return;
//    int index = 0;
//    ITEM_NOT_SELECT;
//    char czBuf[300];
//    SelectFont(hdc, pg_guicomm->fontSmall);
//    int nTop = 20;
//    int nLeft1 = 10;
//    int nLeft2 = 70;
//    TextOut(hdc, nLeft1, nTop, "安全模式");
//
//    if (display_data.safemode == SECURITY_OFF)
//    {
//        sprintf(czBuf, "%s", g_on_off[0]);
//    }
//    else {
//        sprintf(czBuf, "%s", g_on_off[1]);
//    }
//    if (SafeConfigIndex == index)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft2, nTop, czBuf);
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, czBuf);
//    }
//    index++;
//
//    nLeft2 += 60;
//    if (SafeConfigIndex == index)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft2, nTop, "取消");
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, "取消");
//    }
//
//    index++;
//    nTop += 20;
//    if (SafeConfigIndex == index)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft2, nTop, "保存");
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, "保存");
//    }
//
//    PopListDraw(hdc);
//
//}
//BOOL g_safemode_keymsg(int nKey)
//{
//    GUI_COMM* pg_guicomm = &g_guiComm;
//    if (pg_guicomm == NULL)
//        return FALSE;
//    int ret;
//    int maxindex = 2;
//    if (pg_guicomm->bPopList == TRUE)
//    {
//        int nSel = 0;
//
//        PopListKeyMsg(nKey, &nSel);
//        if (nKey == KEY_OK)
//        {
//            switch (SafeConfigIndex)
//            {
//            case 0://安全模式
//            {
//                if (nSel == 0) {
//                    display_data.safemode = SECURITY_OFF;
//                }
//                else {
//                    display_data.safemode = SECURITY_ON;
//                }
//
//                SafeConfigIndex = 1;
//            }
//            break;
//            default:
//                return FALSE;
//            }
//            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
//        }
//        return TRUE;
//    }
//    switch (nKey)
//    {
//    case KEY_UP:
//    case KEY_LEFT:
//    {
//        if (SafeConfigIndex > 0)
//            SafeConfigIndex--;
//        else
//            SafeConfigIndex = maxindex;
//        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
//        return TRUE;
//    }
//    case KEY_DOWN:
//    case KEY_RIGHT:
//    {
//        if (SafeConfigIndex < maxindex)
//            SafeConfigIndex++;
//        else
//            SafeConfigIndex = 0;
//        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
//        return TRUE;
//    }
//    case KEY_OK:
//    {
//        if (ValidatePwd() == TRUE)
//        {
//            switch (SafeConfigIndex)
//            {
//            case 0: //安全模式模式
//            {
//                PopList(g_on_off, 2, g_on_off[0], 70, 20, FALSE);
//                return TRUE;
//            }
//            case 1://取消
//            {
//                if (TRUE == MessageBox("当前配置没有保存\n\n是否确定保存配置？", MB_OKCANCEL))
//                {
//                    ret = write_normal_data((uint8*)&display_data.safemode, sizeof(OOP_SECURITYFLAG_E), SecuConfigOad, 0, 0, 2);
//                    if (ret != 0)
//                    {
//                        MessageBox("安全模式写入失败！");
//                    }
//
//                }
//                SafeConfigIndex = 0;
//                return TRUE;
//            }
//            case 2://保存
//            {
//                ret = write_normal_data((uint8*)&display_data.safemode, sizeof(OOP_SECURITYFLAG_E), SecuConfigOad, 0, 0, 2);
//                if (ret != 0)
//                {
//                    MessageBox("安全模式写入失败！");
//                }
//                else {
//                    MessageBox("安全模式写入成功！");
//                }
//                SafeConfigIndex = 0;
//                return TRUE;
//            }
//            }
//        }
//    }
//    }
//    return FALSE;
//}
//
////安全模式
//PLUGIN_INFO g_safemode = {
//    0x000c0003/*id*/,
//    g_safemode_init/*InitLayer*/,
//    NULL/*UninitLayer*/,
//    g_safemode_draw/*Draw*/,
//    NULL/*Timer*/,
//    g_safemode_keymsg/*keymsg*/,
//    NULL/*ipc*/
//};
//#endif
    
#if DESC("高精度当前正向有功电能示数",1)
void high_forward_active_power_draw(HDC hdc, HIGH_PRECISION_DATA_INFO high_precision_data)
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

        if (high_precision_data.penergy.nNum == 0 ||get_bit_value((uint8 *)&high_precision_data.penergy.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kWh");
        }
        else
        {
            sprintf(czBuf, "%12.4f kWh", (double)high_precision_data.penergy.nValue[i] / 10000);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
}

//正向无功电能示数
void high_forward_reactive_power_draw(HDC hdc, HIGH_PRECISION_DATA_INFO high_precision_data)
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

        if (high_precision_data.penergys.nNum == 0||get_bit_value((uint8 *)&high_precision_data.penergys.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kvarh");
        }
        else
        {
            sprintf(czBuf, "%12.4f kvarh", (double)high_precision_data.penergys.nValue[i] / 10000);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}


int g_ac_data_display_drawpage_index;
void ac_high_forward_power_display_init(DWORD dwPluginId)
{
    memset(&g_high_precision_data.penergy, 0, sizeof(OOP_HENERGY_T));
    memset(&g_high_precision_data.penergys, 0, sizeof(OOP_HENERGYS_T));
    read_db_noamal_data(g_hUdbCenter, 0x00100400, 1, 0, (uint8*)&g_high_precision_data.penergy, sizeof(OOP_HENERGY_T));
    read_db_noamal_data(g_hUdbCenter, 0x00300400, 1, 0, (uint8*)&g_high_precision_data.penergys, sizeof(OOP_HENERGYS_T));

    g_ac_data_display_drawpage_index = 0;
}

//当前正向电能示数
void high_forward_power_display_draw(HDC hdc)
{

    switch (g_ac_data_display_drawpage_index)
    {
    case 0://第1页
    {
        high_forward_active_power_draw(hdc, g_high_precision_data);
        break;
    }
    case 1://第2页
    {
        high_forward_reactive_power_draw(hdc, g_high_precision_data);
        break;
    }
    }

    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

BOOL high_power_display_keymsg(int nKey)
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
PLUGIN_INFO g_layer_ac_high_forward_power_display = {
    0x11010007/*id*/,
    ac_high_forward_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    high_forward_power_display_draw/*Draw*/,
    NULL/*Timer*/,
    high_power_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("当前正向有功电能量示值",1)

//当前正向电能示数
void high_active_power_display_draw(HDC hdc)
{
    high_forward_active_power_draw(hdc, g_high_precision_data);
}


//当前正向有功电能量示值
PLUGIN_INFO g_layer_ac_high_active_power_display = {
    0x11010010/*id*/,
    ac_high_forward_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    high_active_power_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("当前正向无功电能量示值",1)

//当前正向无功电能量示值
void high_reactive_power_display_draw(HDC hdc)
{
    high_forward_reactive_power_draw(hdc, g_high_precision_data);
}


//当前正向无功电能量示值
PLUGIN_INFO g_layer_ac_high_reactive_power_display = {
    0x1101000c/*id*/,
    ac_high_forward_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    high_reactive_power_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("高精度当前反向电能示数",1)
void ac_high_reverse_power_display_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(&g_high_precision_data.nenergy, 0, sizeof(OOP_HENERGY_T));
    memset(&g_high_precision_data.nenergys, 0, sizeof(OOP_HENERGYS_T));
    read_db_noamal_data(g_hUdbCenter, 0x00200400, 1, 0, (uint8*)&g_high_precision_data.nenergy, sizeof(OOP_HENERGY_T));
    read_db_noamal_data(g_hUdbCenter, 0x00400400, 1, 0, (uint8*)&g_high_precision_data.nenergys, sizeof(OOP_HENERGYS_T));

    g_ac_data_display_drawpage_index = 0;
}
//反向有功电能示数
void high_reverse_active_power_draw(HDC hdc, HIGH_PRECISION_DATA_INFO high_precision_data)
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

        if (high_precision_data.nenergy.nNum == 0||get_bit_value((uint8 *)&high_precision_data.nenergy.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kWh");
        }
        else
        {
            sprintf(czBuf, "%12.4f kWh", (double)high_precision_data.nenergy.nValue[i] / 10000);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}

//反向无功电能示数
void high_reverse_reactive_power_draw(HDC hdc, HIGH_PRECISION_DATA_INFO high_precision_data)
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

        if (high_precision_data.nenergys.nNum == 0||get_bit_value((uint8 *)&high_precision_data.nenergys.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kvarh");
        }
        else
        {
            sprintf(czBuf, "%12.4f kvarh", (double)high_precision_data.nenergys.nValue[i] / 10000);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}

//当前反向电能示数
void high_reverse_power_display_draw(HDC hdc)
{
    switch (g_ac_data_display_drawpage_index)
    {
    case 0://第1页
    {
        high_reverse_active_power_draw(hdc, g_high_precision_data);
        break;
    }
    case 1://第2页
    {
        high_reverse_reactive_power_draw(hdc, g_high_precision_data);
        break;
    }
    }
    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

//当前反向电能示数
PLUGIN_INFO g_layer_ac_high_reverse_power_display = {
    0x11010008/*id*/,
    ac_high_reverse_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    high_reverse_power_display_draw/*Draw*/,
    NULL/*Timer*/,
    high_power_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("当前反向有功电能量示值",1)

//当前反向电能示数
void active_high_reverse_power_display_draw(HDC hdc)
{
     high_reverse_active_power_draw(hdc, g_high_precision_data);
}

//当前反向电能示数
PLUGIN_INFO g_layer_high_reverse_power_display = {
    0x1101000d/*id*/,
    ac_high_reverse_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    active_high_reverse_power_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("当前反向无功电能量示值",1)

//当前反向电能示数
void reactive_high_reverse_power_display_draw(HDC hdc)
{
    high_reverse_reactive_power_draw(hdc, g_high_precision_data);
}
//当前反向电能示数
PLUGIN_INFO g_layer_high_reactive_power_display = {
    0x1101000e/*id*/,
    ac_high_reverse_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    reactive_high_reverse_power_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("高精度四象限无功电能",1)
void ac_high_quadrant_power_display_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(&g_high_precision_data.quadrant1, 0, sizeof(OOP_HENERGY_T));
    memset(&g_high_precision_data.quadrant2, 0, sizeof(OOP_HENERGY_T));
    memset(&g_high_precision_data.quadrant3, 0, sizeof(OOP_HENERGY_T));
    memset(&g_high_precision_data.quadrant4, 0, sizeof(OOP_HENERGY_T));

    read_db_noamal_data(g_hUdbCenter, 0x00500400, 1, 0, (uint8*)&g_high_precision_data.quadrant1, sizeof(OOP_HENERGY_T));
    read_db_noamal_data(g_hUdbCenter, 0x00600400, 1, 0, (uint8*)&g_high_precision_data.quadrant2, sizeof(OOP_HENERGY_T));
    read_db_noamal_data(g_hUdbCenter, 0x00700400, 1, 0, (uint8*)&g_high_precision_data.quadrant3, sizeof(OOP_HENERGY_T));
    read_db_noamal_data(g_hUdbCenter, 0x00800400, 1, 0, (uint8*)&g_high_precision_data.quadrant4, sizeof(OOP_HENERGY_T));
    
}

//反向有功电能示数
void high_quadrant_power_draw(HDC hdc, int id, OOP_HENERGY_T quadrant)
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

    // 象限无功电能
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

        if (quadrant.nNum == 0||get_bit_value((uint8 *)&quadrant.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kvarh");
        }
        else
        {
            sprintf(czBuf, "%12.4f kvarh", (double)quadrant.nValue[i] / 10000);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}


//四象限无功电能
void high_quadrant_power_display_draw(HDC hdc)
{
    switch (g_ac_data_display_drawpage_index)
    {
    case 0://第一页
    {
        // 第一象限无功  
        high_quadrant_power_draw(hdc, 1, g_high_precision_data.quadrant1);

        break;
    }
    case 1://第4页
    {
        // 第二象限无功
        high_quadrant_power_draw(hdc, 2, g_high_precision_data.quadrant2);
        break;
    }
    case 2://第5页
    {
        // 第三象限无功
        high_quadrant_power_draw(hdc, 3, g_high_precision_data.quadrant3);
        break;
    }
    case 3://第6页
    {
        // 第四象限无功
        high_quadrant_power_draw(hdc, 4, g_high_precision_data.quadrant4);
        break;
    }

    }
    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

BOOL high_quadrant_power_display_keymsg(int nKey)
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
PLUGIN_INFO g_layer_ac_high_quadrant_power_display = {
    0x11010009/*id*/,
    ac_high_quadrant_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    high_quadrant_power_display_draw/*Draw*/,
    NULL/*Timer*/,
    high_quadrant_power_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif
    
#if DESC("正向有功电能示数",1)
void forward_active_power_draw(HDC hdc, OOP_ENERGY_T penergy, char *title)
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
    DrawText(hdc, title, strlen(title), &rc, DT_CENTER | DT_VCENTER);
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

        if (penergy.nNum == 0 || get_bit_value((uint8 *)&penergy.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kWh");
        }
        else
        {
            sprintf(czBuf, "%12.2f kWh", (double)penergy.nValue[i] / 100);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
}

//正向无功电能示数
void forward_reactive_power_draw(HDC hdc, OOP_ENERGYS_T penergys, char *title)
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
    DrawText(hdc, title, strlen(title), &rc, DT_CENTER | DT_VCENTER);
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

        if (penergys.nNum == 0 || get_bit_value((uint8 *)&penergys.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kvarh");
        }
        else
        {
            sprintf(czBuf, "%12.2f kvarh", (double)penergys.nValue[i] / 100);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}

void ac_forward_power_display_init(DWORD dwPluginId)
{
    memset(&display_data.penergy, 0, sizeof(OOP_ENERGY_T));
    memset(&display_data.penergys, 0, sizeof(OOP_ENERGYS_T));
    //    if(taskid == 0)
    {
        read_db_noamal_data(g_hUdbCenter, 0x00100200, 1, 0, (uint8*)&display_data.penergy, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_hUdbCenter, 0x00300200, 1, 0, (uint8*)&display_data.penergys, sizeof(OOP_ENERGYS_T));
    }
    //    else
    {
        //        read_db_record_data(g_hUdbCenter, 0x00100200, 0, taskid, (uint8*)&display_data.penergy, sizeof(OOP_ENERGY_T));
        //        read_db_record_data(g_hUdbCenter, 0x00300200, 0, taskid, (uint8*)&display_data.penergys, sizeof(OOP_ENERGYS_T));
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
        read_db_noamal_data(g_hUdbCenter, 0x00100200, 1, 0, (uint8*)&display_data.penergy, sizeof(OOP_ENERGY_T));
        char title[64] = "正向有功电能示值";
        forward_active_power_draw(hdc, display_data.penergy, title);
        break;
    }
    case 1://第2页
    {
    
        read_db_noamal_data(g_hUdbCenter, 0x00300200, 1, 0, (uint8*)&display_data.penergys, sizeof(OOP_ENERGYS_T));
        char title[64] = "正向无功电能示值";
        forward_reactive_power_draw(hdc, display_data.penergys, title);
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
#endif
    
#if DESC("当前反向电能示数",1)
void ac_reverse_power_display_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(&display_data.nenergy, 0, sizeof(OOP_ENERGY_T));
    memset(&display_data.nenergys, 0, sizeof(OOP_ENERGYS_T));
    //    if(taskid == 0)
    {
        read_db_noamal_data(g_hUdbCenter, 0x00200200, 1, 0, (uint8*)&display_data.nenergy, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_hUdbCenter, 0x00400200, 1, 0, (uint8*)&display_data.nenergys, sizeof(OOP_ENERGYS_T));
    }
    //    else
    {
        //        read_db_record_data(g_hUdbCenter, 0x00200200, 0, taskid, (uint8*)&display_data.nenergy, sizeof(OOP_ENERGY_T));
        //        read_db_record_data(g_hUdbCenter, 0x00400200, 0, taskid, (uint8*)&display_data.nenergys, sizeof(OOP_ENERGYS_T));
    }
    g_ac_data_display_drawpage_index = 0;
}
//反向有功电能示数
void reverse_active_power_draw(HDC hdc, OOP_ENERGY_T nenergy, char *title)
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
    DrawText(hdc, title, strlen(title), &rc, DT_CENTER | DT_VCENTER);
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

        if (nenergy.nNum == 0  || get_bit_value((uint8 *)&nenergy.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kWh");
        }
        else
        {
            sprintf(czBuf, "%12.2f kWh", (double)nenergy.nValue[i] / 100);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}

//反向无功电能示数
void reverse_reactive_power_draw(HDC hdc, OOP_ENERGYS_T nenergys, char *title)
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
    DrawText(hdc, title, strlen(title), &rc, DT_CENTER | DT_VCENTER);
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

        if (nenergys.nNum == 0  || get_bit_value((uint8 *)&nenergys.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kvarh");
        }
        else
        {
            sprintf(czBuf, "%12.2f kvarh", (double)nenergys.nValue[i] / 100);
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
        char title[64] = "反向有功电能示值";
        reverse_active_power_draw(hdc, display_data.nenergy, title);
        break;
    }
    case 1://第2页
    {
        char title[64] = "反向无功电能示值";
        reverse_reactive_power_draw(hdc, display_data.nenergys, title);
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
#endif
    
#if DESC("四象限无功电能",1)
void ac_quadrant_power_display_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(&display_data.quadrant1, 0, sizeof(OOP_ENERGY_T));
    memset(&display_data.quadrant2, 0, sizeof(OOP_ENERGY_T));
    memset(&display_data.quadrant3, 0, sizeof(OOP_ENERGY_T));
    memset(&display_data.quadrant4, 0, sizeof(OOP_ENERGY_T));
    //   if(taskid == 0)
    {
        read_db_noamal_data(g_hUdbCenter, 0x00500200, 1, 0, (uint8*)&display_data.quadrant1, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_hUdbCenter, 0x00600200, 1, 0, (uint8*)&display_data.quadrant2, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_hUdbCenter, 0x00700200, 1, 0, (uint8*)&display_data.quadrant3, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_hUdbCenter, 0x00800200, 1, 0, (uint8*)&display_data.quadrant4, sizeof(OOP_ENERGY_T));
    }
    //    else
    {
        //        read_db_record_data(g_hUdbCenter, 0x00500200, 0, taskid, (uint8*)&display_data.quadrant1, sizeof(OOP_ENERGY_T));
        //        read_db_record_data(g_hUdbCenter, 0x00600200, 0, taskid, (uint8*)&display_data.quadrant2, sizeof(OOP_ENERGY_T));
        //        read_db_record_data(g_hUdbCenter, 0x00700200, 0, taskid, (uint8*)&display_data.quadrant3, sizeof(OOP_ENERGY_T));
        //        read_db_record_data(g_hUdbCenter, 0x00800200, 0, taskid, (uint8*)&display_data.quadrant4, sizeof(OOP_ENERGY_T));
    }
}

//反向有功电能示数
void quadrant_power_draw(HDC hdc, OOP_ENERGY_T quadrant, char *title)
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
    DrawText(hdc, title, strlen(title), &rc, DT_CENTER | DT_VCENTER);
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
            sprintf(czBuf, "%12.2f kvarh", (double)quadrant.nValue[i] / 100);
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
        char title[64] = "第1象限无功电能";
        quadrant_power_draw(hdc, display_data.quadrant1, title);

        break;
    }
    case 1://第4页
    {
        // 第二象限无功
        char title[64] = "第2象限无功电能";
        quadrant_power_draw(hdc, display_data.quadrant2, title);
        break;
    }
    case 2://第5页
    {
        // 第三象限无功
        char title[64] = "第3象限无功电能";
        quadrant_power_draw(hdc, display_data.quadrant3, title);
        break;
    }
    case 3://第6页
    {
        // 第四象限无功
        char title[64] = "第4象限无功电能";
        quadrant_power_draw(hdc, display_data.quadrant4, title);
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
#endif
    
#if DESC("电压电流数据",1)
void ac_vol_cur_display_init(DWORD dwPluginId)
{
    memset(&display_data.vol, 0, sizeof(OOP_ENERGY_T));
    memset(&display_data.cur, 0, sizeof(OOP_ENERGYS_T));
    //    if(taskid == 0)
    {
        read_db_noamal_data(g_hUdbCenter, 0x20000200, 1, 0, (uint8*)&display_data.vol, sizeof(OOP_WORD3V_T));
        read_db_noamal_data(g_hUdbCenter, 0x20010200, 1, 0, (uint8*)&display_data.cur, sizeof(OOP_INT3V_T));
    }
    //   else
    {
        //       read_db_record_data(g_hUdbCenter, 0x20000200, 0, taskid, (uint8*)&display_data.vol, sizeof(OOP_DWORD3V_T));
        //        read_db_record_data(g_hUdbCenter, 0x20010200, 0, taskid, (uint8*)&display_data.cur, sizeof(OOP_INT3V_T));
    }

}

void vol_cur_display_draw(HDC hdc, DISPLAY_DATA_INFO bdisplay_data)
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
        if (bdisplay_data.vol.nNum == 0 || get_bit_value((uint8 *)&bdisplay_data.vol.rsv,32,i) == 0)
        {
            sprintf(czBuf, "%s --- V", g_showTitleVoltag34[i]);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            tmp = (float)bdisplay_data.vol.nValue[i] / 10;
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
        if (bdisplay_data.cur.nNum == 0 || get_bit_value((uint8 *)&bdisplay_data.cur.rsv,32,i) == 0)
        {
            sprintf(czBuf, "%s --- A", g_showTitleEelectric[i]);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            if (bdisplay_data.cur.nValue[i] < 0)
            {
                tmp = ~bdisplay_data.cur.nValue[i] + 1;
                tmp = tmp / 1000.0;
                sprintf(czBuf, "%s -%3.3f A", g_showTitleEelectric[i], tmp);
            }
            else
            {
                tmp = bdisplay_data.cur.nValue[i];
                tmp = tmp / 1000.0;
                sprintf(czBuf, "%s %3.3f A", g_showTitleEelectric[i], tmp);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        nTop += tmpTop;

    }
}

void vol_cur_display_draws(HDC hdc)
{
    read_db_noamal_data(g_hUdbCenter, 0x20000200, 1, 0, (uint8*)&display_data.vol, sizeof(OOP_WORD3V_T));
    read_db_noamal_data(g_hUdbCenter, 0x20010200, 1, 0, (uint8*)&display_data.cur, sizeof(OOP_INT3V_T));
    vol_cur_display_draw(hdc, display_data);
}


//电压电流数据
PLUGIN_INFO g_layer_ac_vol_cur_display = {
    0x11010004/*id*/,
    ac_vol_cur_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    vol_cur_display_draws/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
#endif
    
#if DESC("当前功率示数",1)
void ac_power_display_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(&display_data.ppower, 0, sizeof(OOP_INT4V_T));
    memset(&display_data.npower, 0, sizeof(OOP_INT4V_T));
    memset(&display_data.fpower, 0, sizeof(OOP_LONG4V_T));
    //    if(taskid == 0)
    {
        read_db_noamal_data(g_hUdbCenter, 0x20040200, 1, 0, (uint8*)&display_data.ppower, sizeof(OOP_INT4V_T));
        read_db_noamal_data(g_hUdbCenter, 0x20050200, 1, 0, (uint8*)&display_data.npower, sizeof(OOP_INT4V_T));
        read_db_noamal_data(g_hUdbCenter, 0x200a0200, 1, 0, (uint8*)&display_data.fpower, sizeof(OOP_LONG4V_T));
    }
    //    else
    {
        //        read_db_record_data(g_hUdbCenter, 0x20040200, 0, taskid, (uint8*)&display_data.ppower, sizeof(OOP_INT4V_T));
        //        read_db_record_data(g_hUdbCenter, 0x20050200, 0, taskid, (uint8*)&display_data.npower, sizeof(OOP_INT4V_T));
        //        read_db_record_data(g_hUdbCenter, 0x200a0200, 0, taskid, (uint8*)&display_data.fpower, sizeof(OOP_INT4V_T));
    }

//    printf("display_data.fpower[0] %d, [1]%d\n ", display_data.fpower.nValue[0], display_data.fpower.nValue[0]);

}

void valueDisplayProcess_strUnit(int64 energy, char* str, size_t n,uint32 weight,const char* unit);

void power_display_draw_param(HDC hdc, const char* pname, const char** item, OOP_INT4V_T power, const char* punit, int multiple)
{
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };
    //float tmp;

    int nLeft = 5;
    int nLeft2 = 40;
    int nTop = 16;
    int tmpTop = 15;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 16;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "%s", pname);
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    for (int i = 0; i < 4; i++)
    {
        nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, item[i]);
        nTop+=13;
        if (power.nNum == 0 || get_bit_value((uint8 *)&power.rsv,32,i) == 0)
        {
            sprintf(czBuf, "-------- %s", punit);
        }
        else
        {
            if (power.nValue[i] < 0)
            {
                //tmp = ~power.nValue[i] + 1;
                //sprintf(czBuf, "-%6.4f %s", tmp / multiple, punit);
                valueDisplayProcess_strUnit(power.nValue[i],czBuf,64,4,punit);
            }
            else
            {
                //tmp = power.nValue[i];
                //sprintf(czBuf, "%6.4f %s", tmp / multiple, punit);
                valueDisplayProcess_strUnit(power.nValue[i],czBuf,64,4,punit);
            }
        }
        //TextOut(hdc, nLeft2, nTop, czBuf);
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
        power_display_draw_param(hdc, "有功功率", g_showTitleJCPower_Y, display_data.ppower, "W", 10000);
        break;
    }
    case 1://第2页
    {
        // 无功功率
        power_display_draw_param(hdc, "无功功率", g_showTitleJCPower_W, display_data.npower, "var", 10000);
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
#endif

#if DESC("功率因数",1)
void power_display_init(DWORD dwPluginId)
{
    memset(&display_data.fpower, 0, sizeof(OOP_LONG4V_T));
    read_db_noamal_data(g_hUdbCenter, 0x200a0200, 1, 0, (uint8*)&display_data.fpower, sizeof(OOP_LONG4V_T));

}

void powe_draw(HDC hdc)
{
    power_display_draw_params(hdc, "功率因数", g_showTitleJCPowerFactor, display_data.fpower, "", 1000);
}

//当前功率因数
PLUGIN_INFO g_layer_power_display = {
    0x1101000f/*id*/,
    power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    powe_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("正反向有功需量",1)
void ac_demand_display_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(&display_data.pdemand, 0, sizeof(OOP_DEMAND_T));
    memset(&display_data.ndemand, 0, sizeof(OOP_DEMAND_T));

    //    if(taskid == 0)
    {
        read_db_noamal_data(g_hUdbCenter, 0x10100200, 1, 0, (uint8*)&display_data.pdemand, sizeof(OOP_DEMAND_T));
        read_db_noamal_data(g_hUdbCenter, 0x10200200, 1, 0, (uint8*)&display_data.ndemand, sizeof(OOP_DEMAND_T));
    }
    //    else
    {
        //        read_db_record_data(g_hUdbCenter, 0x10100200, 0, taskid, (uint8*)&display_data.pdemand, sizeof(OOP_DEMAND_T));
        //        read_db_record_data(g_hUdbCenter, 0x10200200, 0, taskid, (uint8*)&display_data.ndemand, sizeof(OOP_DEMAND_T));
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
        if (demand.nNum == 0 || get_bit_value((uint8 *)&demand.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ ");
        }
        else
        {
            sprintf(czBuf, "%hu-%02hu-%02hu %02hu:%02hu:%02hu",
                demand.demand[i].time.year,
                demand.demand[i].time.month,
                demand.demand[i].time.day,
                demand.demand[i].time.hour,
                demand.demand[i].time.minute,
                demand.demand[i].time.second);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        if (nTop >= 122)
        {
            break;
        }
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
#endif

#if DESC("轮显总剩余电量",1)

int64 remainPower[8];
uint8 buytype[8];
void ac_remainPower_display_init(DWORD dwPluginId)
{
    uint32 oad_value = 0;
    int ret = 0, index = 0;
    OOP_8107_CONFIG_T Buyenergyconfig = {0};

    memset(remainPower, 0, sizeof(remainPower));
    memset(buytype, 0, sizeof(buytype));
    
    for(int i = 0; i < 8; i++)
    {
        oad_value = 0x23010B00 + i * 0x00010000;
        ret = read_db_noamal_data(g_hUdbCenter, oad_value, 0, 0, (uint8*)&remainPower[i], sizeof(int64));
        if(ret != 0)
        {
            remainPower[i] = unlawPower;
        }
    }

    //获取总加组剩余电量的单位
    read_db_noamal_data(g_hUdbCenter, 0x81070200, 0, 0, (uint8*)&Buyenergyconfig, sizeof(OOP_8107_CONFIG_T));
    for(uint32 i = 0; i < Buyenergyconfig.nNum; i++)
    {
        index = Buyenergyconfig.item[i].object - 0x2301;
        buytype[index] = Buyenergyconfig.item[i].type;
    }
}

void  remainPower_display_draw1(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    int i = 0;
    char czBuf[64] = { 0 };

    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "总剩余电量");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontSmall);

    //显示前4个总加组的剩余电量
    for (i = 0; i < 4; i++)
    {
        nTop += tmpTop;
        if (unlawPower == remainPower[i])
        {
            sprintf(czBuf, "总加组%d -------- %s", i+1, (buytype[i] == 1) ? "元" : "kWh");
        }
        else
        {
            sprintf(czBuf, "总加组%d %8.2f %s", i+1, (double)remainPower[i] / 10000, (buytype[i] == 1) ? "元" : "kWh");
        }
        TextOut(hdc, 15, nTop, czBuf);
    }
}

void  remainPower_display_draw2(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    int i = 0;
    char czBuf[64] = { 0 };

    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "总剩余电量");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontSmall);

    //第二屏显示后4个总加组的剩余电量
    for (i = 0; i < 4; i++)
    {
        nTop += tmpTop ;
        if (unlawPower == remainPower[4 + i])
        {
            sprintf(czBuf, "总加组%d -------- %s", i+5,    (buytype[i+4] == 1) ? "元" : "kWh");
        }
        else
        {
            sprintf(czBuf, "总加组%d %8.2f %s", i+5, (double)remainPower[4 + i] / 10000, (buytype[i+4] == 1) ? "元" : "kWh");
        }
        TextOut(hdc, 15, nTop, czBuf);
    }
}


//总剩余电量第一屏
PLUGIN_INFO g_layer_ac_remainPower_display1 = {
    0x11010077/*id*/,
    ac_remainPower_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    remainPower_display_draw1/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//总剩余电量第二屏
PLUGIN_INFO g_layer_ac_remainPower_display2 = {
    0x11010078/*id*/,
    ac_remainPower_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    remainPower_display_draw2/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

#endif


#if DESC("电压电流谐波含有率",1)
int8 g_harmonic_index;
void harmonic_display_init(DWORD dwPluginId)
{
    memset(display_data.harmonicV, 0, sizeof(OOP_HARMONIC_T)*3);
    memset(display_data.harmonicC, 0, sizeof(OOP_HARMONIC_T)*3);
    g_harmonic_index = 0;

    read_db_noamal_data(g_hUdbCenter, 0x200D0200, 1, 0, (uint8*)&display_data.harmonicV[0], sizeof(OOP_HARMONIC_T));
    read_db_noamal_data(g_hUdbCenter, 0x200D0300, 1, 0, (uint8*)&display_data.harmonicV[1], sizeof(OOP_HARMONIC_T));
    read_db_noamal_data(g_hUdbCenter, 0x200D0400, 1, 0, (uint8*)&display_data.harmonicV[2], sizeof(OOP_HARMONIC_T));
    read_db_noamal_data(g_hUdbCenter, 0x200E0200, 1, 0, (uint8*)&display_data.harmonicC[0], sizeof(OOP_HARMONIC_T));
    read_db_noamal_data(g_hUdbCenter, 0x200E0300, 1, 0, (uint8*)&display_data.harmonicC[1], sizeof(OOP_HARMONIC_T));
    read_db_noamal_data(g_hUdbCenter, 0x200E0400, 1, 0, (uint8*)&display_data.harmonicC[2], sizeof(OOP_HARMONIC_T));

}

void harmonic_display_draw(HDC hdc)
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
    if(g_harmonic_index == 0)
    {
        sprintf(czBuf, "总谐波含有率");
    }
    else
    {
        sprintf(czBuf, "%u次谐波含有率", g_harmonic_index + 1);
    }
    
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontBig);
    TextOut(hdc, 14, nTop, "电压");
    SelectFont(hdc, g_guiComm.fontSmall);

    // 电压
    for (i = 0; i < 3; i++)
    {
        if (display_data.harmonicV[i].nNum == 0 || get_bit_value((uint8 *)&display_data.harmonicV[i].rsv,32,g_harmonic_index) == 0 )
        {
            sprintf(czBuf, "%s --- %%", g_showTitleVoltag34[i]);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            tmp = (float)display_data.harmonicV[i].nValue[g_harmonic_index] / 100;
            sprintf(czBuf, "%s %2.2f %%", g_showTitleVoltag34[i], tmp);
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
        if (display_data.harmonicC[i].nNum == 0 || get_bit_value((uint8 *)&display_data.harmonicC[i].rsv,32,g_harmonic_index) == 0 )
        {
            sprintf(czBuf, "%s --- %%", g_showTitleEelectric[i]);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            
            tmp = display_data.harmonicC[i].nValue[g_harmonic_index];
            tmp = tmp / 100;
            sprintf(czBuf, "%s %2.2f %%", g_showTitleEelectric[i], tmp);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        nTop += tmpTop;

    }
}

BOOL harmonic_display_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        g_harmonic_index++;
        if (g_harmonic_index >= OOP_MAX_HARMONIC)
        {
            g_harmonic_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    case KEY_LEFT:
    {
        g_harmonic_index--;
        if (g_harmonic_index < 0)
        {
            g_harmonic_index = OOP_MAX_HARMONIC - 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}

//电压电流谐波含有率
PLUGIN_INFO g_layer_harmonic_display = {
    0x1101000a/*id*/,
    harmonic_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    harmonic_display_draw/*Draw*/,
    NULL/*Timer*/,
    harmonic_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("电压电流谐波含量",1)
int8 g_harmonic_value_index;
void harmonic_value_display_init(DWORD dwPluginId)
{
    memset(display_data.harmonicV_value, 0, sizeof(OOP_HARMONIC_U_VALUE_T)*3);
    memset(display_data.harmonicC_value, 0, sizeof(OOP_HARMONIC_I_VALUE_T)*3);
    g_harmonic_value_index = 1;

    read_db_noamal_data(g_hUdbCenter, 0x20330200, 1, 0, (uint8*)&display_data.harmonicV_value[0], sizeof(OOP_HARMONIC_U_VALUE_T));
    read_db_noamal_data(g_hUdbCenter, 0x20330300, 1, 0, (uint8*)&display_data.harmonicV_value[1], sizeof(OOP_HARMONIC_U_VALUE_T));
    read_db_noamal_data(g_hUdbCenter, 0x20330400, 1, 0, (uint8*)&display_data.harmonicV_value[2], sizeof(OOP_HARMONIC_U_VALUE_T));
    read_db_noamal_data(g_hUdbCenter, 0x20340200, 1, 0, (uint8*)&display_data.harmonicC_value[0], sizeof(OOP_HARMONIC_I_VALUE_T));
    read_db_noamal_data(g_hUdbCenter, 0x20340300, 1, 0, (uint8*)&display_data.harmonicC_value[1], sizeof(OOP_HARMONIC_I_VALUE_T));
    read_db_noamal_data(g_hUdbCenter, 0x20340400, 1, 0, (uint8*)&display_data.harmonicC_value[2], sizeof(OOP_HARMONIC_I_VALUE_T));

}

void harmonic_value_display_draw(HDC hdc)
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
    sprintf(czBuf, "%u次谐波电压电流", g_harmonic_value_index + 1);
    
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontBig);
    TextOut(hdc, 14, nTop, "电压");
    SelectFont(hdc, g_guiComm.fontSmall);

    // 电压
    for (i = 0; i < 3; i++)
    {
        if (display_data.harmonicV_value[i].nNum == 0 || get_bit_value((uint8 *)&display_data.harmonicV_value[i].rsv,32,g_harmonic_index) == 0 )
        {
            sprintf(czBuf, "%s --- V", g_showTitleVoltag34[i]);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            tmp = (float)display_data.harmonicV_value[i].nValue[g_harmonic_value_index] / 10;
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
        if (display_data.harmonicC_value[i].nNum == 0 || get_bit_value((uint8 *)&display_data.harmonicC_value[i].rsv,32,g_harmonic_index) == 0 )
        {
            sprintf(czBuf, "%s --- A", g_showTitleEelectric[i]);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            if (display_data.harmonicC_value[i].nValue[g_harmonic_value_index] < 0)
            {
                tmp = ~display_data.harmonicC_value[i].nValue[g_harmonic_value_index] + 1;
                tmp = tmp / 1000.0;
                sprintf(czBuf, "%s -%3.3f A", g_showTitleEelectric[i], tmp);
            }
            else
            {
                tmp = display_data.harmonicC_value[i].nValue[g_harmonic_value_index];
                tmp = tmp / 1000.0;
                sprintf(czBuf, "%s %3.3f A", g_showTitleEelectric[i], tmp);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        nTop += tmpTop;

    }
}

BOOL harmonic_value_display_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        g_harmonic_value_index++;
        if (g_harmonic_value_index > OOP_MAX_HARMONIC - 1)
        {
            g_harmonic_value_index = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    case KEY_LEFT:
    {
        g_harmonic_value_index--;
        if (g_harmonic_value_index < 1)
        {
            g_harmonic_value_index = OOP_MAX_HARMONIC - 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}

//电压电流谐波含有率
PLUGIN_INFO g_layer_harmonic_value_display = {
    0x1101000b/*id*/,
    harmonic_value_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    harmonic_value_display_draw/*Draw*/,
    NULL/*Timer*/,
    harmonic_value_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("日冻结数据", 1)
MEASURE_MAIN g_measure_main_data;
OOP_METER_T  MeterFile;
const char*  g_measure_point_state[] = {"无效", "有效"};
const char*  g_measure_point_type[] = {"-", "485表", "载波表", "交采表"};
char g_day_frozen_store_time[16] = "";
int g_day_frozen_display_drawpage_index;

//将端口OAD转换为测量点类型
int get_infonum_type(OOP_OAD_U port)
{
    switch (port.nObjID)
    {
    case 0xF209:
    {
        if (port.attID == 2)
            return 2;
    }
        break;
    case 0xF201:
    {
        if (port.attID == 2)
            return 1;
    }
        break;
    case 0xF208:
    {
        if (port.attID == 2)
            return 3;
    }
        break;
    default:
        break;
    }
    return 0;

}

void day_frozen_time2str(char *str, int strlen)
{
    time_t timep = 0;
    time(&timep);       //获取从1970至今过了多少秒，存入time_t类型的timep
    timep -= 24 * 3600; //当前时间的前一天
    strftime(str, strlen, "%Y%m%d", localtime((time_t*)&timep));
}

void day_frozen_time_flush(char* pNewStr, void* pPara)
{
    GUI_FMT_DEBUG("day_frozen_time_flush %s\n", pNewStr);
    int len;
    len = strlen(pNewStr);
    if (len < 0 || len > 8)
    {
        char bzbuf[100];
        char buf[32];
        time_t timep = 0;
        
        time(&timep);       //获取从1970至今过了多少秒，存入time_t类型的timep
        timep -= 24 * 3600; //当前时间的前一天
        strftime(buf, sizeof(buf), "%Y%m%d(%Y年%m月%d日)", localtime((time_t*)&timep));
        sprintf(bzbuf, "输入示例为:\n %s\n确认是否重新输入?", buf);
        if(TRUE == MessageBox(bzbuf, MB_OKCANCEL))
        {
            InputBox(g_day_frozen_store_time, day_frozen_time_flush, NULL, 0x01);
        }
    }
    else
    {
        memcpy(g_day_frozen_store_time, pNewStr, sizeof(g_day_frozen_store_time));
    }
}

void day_frozen_str2time(uint32            *tm)
{
    uint16          year;
    uint8           month;
    uint8           day;
    DateTimeHex_t   startData;

    year    = (g_day_frozen_store_time[0] - '0') * 1000 + (g_day_frozen_store_time[1] - '0') * 100 +
                (g_day_frozen_store_time[2] - '0') * 10 + (g_day_frozen_store_time[3]- '0');
    month   = (g_day_frozen_store_time[4] - '0') * 10 + (g_day_frozen_store_time[5] - '0');
    day     = (g_day_frozen_store_time[6] - '0') * 10 + (g_day_frozen_store_time[7] - '0');

    // 日冻结存储时间为某天的23点59分0秒
    startData.year_h    = year >> 8;
    startData.year_l    = year & 0xFF;
    startData.month     = month;
    startData.day       = day;
    startData.hour      = 0x0;
    startData.min       = 0x0;
    startData.sec       = 0x0;

    if (!DT_DateTimeHex2Time((DateTimeHex_t*)&startData, (time_t*)tm))
    {
        GUI_FMT_DEBUG("day_frozen_str2time: store time error!\n");
    }
    
}

int day_frozen_db_read(DB_CLIENT fp, uint8 logic_id, uint16 info_num, uint8* pbuf, uint32 buf_len)
{
    int                ret = ERR_NORMAL;
    uint32             len     = 0;
    READ_RECORD_T      inReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_METER_T        oopMeter;
    NOMAL_OAD_T        nomalOAD;
    OOP_OCTETVAR16_T   m_tmnAddr;
    //DateTimeHex_t      startData;
    uint32             startTm = 0;
    uint32             endTm = 0;

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    day_frozen_str2time(&startTm);
    // 注：+8个小时(28800)转成格林时间
    startTm += COMPENSATION_TIME;
    endTm = startTm+24*60*60-1;

    inReadRecord.recordOAD.optional = 1;
    inReadRecord.recordOAD.logicId = logic_id;
    inReadRecord.recordOAD.infoNum = info_num;
    inReadRecord.recordOAD.road.oadMain.value = 0x50040200;
    inReadRecord.recordOAD.road.oadCols.nNum = 8;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x00100200;
    inReadRecord.recordOAD.road.oadCols.oad[1].value = 0x00300200;
    inReadRecord.recordOAD.road.oadCols.oad[2].value = 0x00200200;
    inReadRecord.recordOAD.road.oadCols.oad[3].value = 0x00400200;
    inReadRecord.recordOAD.road.oadCols.oad[4].value = 0x00500200;
    inReadRecord.recordOAD.road.oadCols.oad[5].value = 0x00600200;
    inReadRecord.recordOAD.road.oadCols.oad[6].value = 0x00700200;
    inReadRecord.recordOAD.road.oadCols.oad[7].value = 0x00800200;
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
        memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
        nomalOAD.logicId = logic_id;
        nomalOAD.oad.value = 0x40010200;
        ret = db_read_nomal(fp, &nomalOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&m_tmnAddr, &len);
        if(0 != ret || len != sizeof(OOP_OCTETVAR16_T))
        {
            GUI_FMT_DEBUG(" db_read_nomal 0x40010200 err = %d!\n", ret);
            return -1;
        }
         memcpy(inReadRecord.MAC.value, m_tmnAddr.value, 6);
    }
    else
    {
        if (app_get_meter_list(info_num, &oopMeter) != ERR_OK)
        {
            GUI_FMT_DEBUG(" app_get_meter_list %d !\n", info_num);
            return -1;
        }
        memcpy(inReadRecord.MAC.value, oopMeter.basic.tsa.add, 6);
    }

    ret = db_read_record(fp, &inReadRecord, buf_len, &stepRecordOut);
    if (ret != ERR_OK)
    {
        GUI_FMT_DEBUG(" db_read_record %08x ret=%d !\n", 0x00100200, ret);
        return -1;
    }
    memcpy(&display_day_frozen.penergy, stepRecordOut.record[0].outDataBuf, sizeof(OOP_ENERGY_T)); 
    memcpy(&display_day_frozen.penergys, stepRecordOut.record[1].outDataBuf, sizeof(OOP_ENERGYS_T));
    memcpy(&display_day_frozen.nenergy, stepRecordOut.record[2].outDataBuf, sizeof(OOP_ENERGY_T));

    memcpy(&display_day_frozen.nenergys, stepRecordOut.record[3].outDataBuf, sizeof(OOP_ENERGYS_T));
    memcpy(&display_day_frozen.quadrant1, stepRecordOut.record[4].outDataBuf, sizeof(OOP_ENERGY_T)); 
    memcpy(&display_day_frozen.quadrant2, stepRecordOut.record[5].outDataBuf, sizeof(OOP_ENERGY_T));
    memcpy(&display_day_frozen.quadrant3, stepRecordOut.record[6].outDataBuf, sizeof(OOP_ENERGY_T)); 
    memcpy(&display_day_frozen.quadrant4, stepRecordOut.record[7].outDataBuf, sizeof(OOP_ENERGY_T));
    return 0;
}

void day_frozen_display_init(DWORD dwPluginId)
{
    g_day_frozen_display_drawpage_index = 0;
    if (g_guiComm.nCurPoint == 0)
    {
        memset(&display_data, 0, sizeof(DISPLAY_DATA_INFO));
        read_db_noamal_data(g_hUdbCenter, 0x00100200, 1, 0, (uint8*)&display_data.penergy, sizeof(OOP_ENERGY_T));
    }
    else
    {
        memset(&display_day_frozen, 0, sizeof(DISPLAY_DAY_FROZEN_INFO));
        day_frozen_db_read(g_hUdbCenter, 0, g_guiComm.nCurPoint, (uint8*)&display_day_frozen, sizeof(DISPLAY_DAY_FROZEN_INFO));
    }
}

void day_frozen_display_draw(HDC hdc)
{
    switch (g_day_frozen_display_drawpage_index)
    {
        case 0://第1页
        {
            // 正向有功
            char title[64] = "日冻结正向有功电能";
            forward_active_power_draw(hdc, display_day_frozen.penergy, title);
            break;
        }
        case 1://第2页
        {
            // 正向无功
            char title[64] = "日冻结正向无功电能";
            forward_reactive_power_draw(hdc, display_day_frozen.penergys, title);
            break;
        }
        case 2:
        {
            // 反向有功
            char title[64] = "日冻结反向有功电能";
            reverse_active_power_draw(hdc, display_day_frozen.nenergy, title);
            break;
        }
        case 3:
        {
            // 反向无功
            char title[64] = "日冻结反向无功电能";
            reverse_reactive_power_draw(hdc, display_day_frozen.nenergys, title);
            break;
        }
        case 4:
        {
            // 第一象限无功
            char title[64] = "日冻结第1象限无功";
            quadrant_power_draw(hdc, display_day_frozen.quadrant1, title);
            break;
        }
        case 5:
        {
            // 第二象限无功
            char title[64] = "日冻结第2象限无功";
            quadrant_power_draw(hdc, display_day_frozen.quadrant2, title);
            break;
        }
        case 6:
        {
            // 第三象限无功
            char title[64] = "日冻结第3象限无功";
            quadrant_power_draw(hdc, display_day_frozen.quadrant3, title);
            break;
        }
        case 7:
        {
            // 第四象限无功
            char title[64] = "日冻结第4象限无功";
            quadrant_power_draw(hdc, display_day_frozen.quadrant4, title);
            break;
        }
        default:
            break;
    }
}

BOOL day_frozen_display_keymsg(int nKey)
{
    int nPoint = g_guiComm.nCurPoint;
    switch (nKey)
    {
        case KEY_DOWN:
        {
            
            g_day_frozen_display_drawpage_index++;
            if (g_day_frozen_display_drawpage_index > 7)
            {
                g_day_frozen_display_drawpage_index = 0;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return  TRUE;

        }
        case KEY_UP:
        {
            g_day_frozen_display_drawpage_index--;
            if (g_day_frozen_display_drawpage_index < 0)
            {
                g_day_frozen_display_drawpage_index = 7;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        {
            g_guiComm.nCurPoint++;
            if (g_guiComm.nCurPoint > MAX_SEL_POINT)
            {
                g_guiComm.nCurPoint = 0;
            }
            set_gui_infonum((uint16)g_guiComm.nCurPoint);
            day_frozen_display_init(0);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return  TRUE;

        }
        break;
        
        case KEY_LEFT:
        {
            g_guiComm.nCurPoint--;
            if (g_guiComm.nCurPoint < 0)
            {
                g_guiComm.nCurPoint = MAX_SEL_POINT;
            }
            set_gui_infonum((uint16)g_guiComm.nCurPoint);
            day_frozen_display_init(0);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
        break;
        case KEY_CANCEL:
            g_guiComm.nCurPoint = nPoint;
        break;
        default:
            break;
    }

    return FALSE;
}

//日冻结数据
PLUGIN_INFO g_layer_day_frozen_display = {
    0x00/*id*/,
    day_frozen_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    day_frozen_display_draw/*Draw*/,
    NULL/*Timer*/,
    day_frozen_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

//日冻结数据
PLUGIN_INFO* g_day_frozen_plugin[] =
{
    &g_layer_day_frozen_display,
};

void measure_day_frozen_display_init(DWORD dwPluginId)
{
    memset(&g_measure_main_data, 0, sizeof(g_measure_main_data));
    memset(&MeterFile, 0, sizeof(OOP_METER_T));
        
    // 设置当前选择项
    g_guiComm.nCurPoint = 1;
    g_measure_main_data.nIndex = 0;
    g_measure_main_data.pCurPlugin = NULL;

    // 日冻结时间显示为当前时间的前一天
    day_frozen_time2str(g_day_frozen_store_time, sizeof(g_day_frozen_store_time));
}

void measure_day_frozen_display_draw(HDC hdc)
{
    // 测量点详细信息页面库绘制
    if (g_measure_main_data.pCurPlugin != NULL)
    {
        ASSERT(g_measure_main_data.pCurPlugin->pfnDraw != NULL);
        g_measure_main_data.pCurPlugin->pfnDraw(hdc);
        return;
    }

    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    int nTop = 20;
    int nLeft1 = 50;
    TextOut(hdc, nLeft1, nTop, "冻结数据");

    // 选择测量点
    nLeft1 = 5;
    int nLeft2 = 85;
    nTop += 18;
    TextOut(hdc, nLeft1, nTop, "输入冻结日期");
    if (g_measure_main_data.nIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_day_frozen_store_time);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_day_frozen_store_time);
    }

    nTop += 18;
    TextOut(hdc, nLeft1, nTop, "请选择测量点");
    char czBuf[20];
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);
    set_gui_infonum((uint16)g_guiComm.nCurPoint);
    if (g_measure_main_data.nIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    //GUI_FMT_DEBUG("measure_data_display_draw %d\n", g_guiComm.nCurPoint);
    if (app_get_meter_list(g_guiComm.nCurPoint, &MeterFile) == ERR_OK)
    {
        g_measure_main_data.nState = 1;
        g_measure_main_data.nType = get_infonum_type(MeterFile.basic.port);
        sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", MeterFile.basic.tsa.add[0], MeterFile.basic.tsa.add[1],
                MeterFile.basic.tsa.add[2], MeterFile.basic.tsa.add[3], MeterFile.basic.tsa.add[4], MeterFile.basic.tsa.add[5]);
    }
    else
    {
        g_measure_main_data.nState = 0;
        g_measure_main_data.nType = 0;
    }
    
    // 测量点状态
    nTop += 18;
    TextOut(hdc, nLeft1, nTop, "测量点状态");
    TextOut(hdc, nLeft2, nTop, g_measure_point_state[g_measure_main_data.nState]);
    
    // 测量点类型
    nTop += 18;
    TextOut(hdc, nLeft1, nTop, "测量点类型");
    TextOut(hdc, nLeft2, nTop, g_measure_point_type[g_measure_main_data.nType]);
    
    if (g_measure_main_data.nState)
    {
        // 表地址
        nTop += 18;
        TextOut(hdc, nLeft1, nTop, "表地址");
        TextOut(hdc, nLeft2, nTop, czBuf);

        // 详细信息
        nTop += 18;
        if (g_measure_main_data.nIndex == 2)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, "详细信息");
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, "详细信息");
        }
    }
}

static BOOL measure_day_frozen_display_keymsg(int nKey)
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
            if (app_get_meter_list(g_guiComm.nCurPoint, &MeterFile) != ERR_OK) //如果获取档案失败
                g_measure_main_data.nIndex = 0;
            g_guiComm.nArrow = -1;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        case KEY_UP:// 向上翻页面（在有多个页面的情况下）
            if (g_measure_main_data.nPluginCnt > 1)
            {
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
        if (g_measure_main_data.nState)
        {
            g_measure_main_data.nIndex--;
            if (g_measure_main_data.nIndex < 0)
            {
                g_measure_main_data.nIndex = 2;
            }
        }
        else
        {
             if (g_measure_main_data.nIndex == 1)
                g_measure_main_data.nIndex = 0;
             else
                g_measure_main_data.nIndex = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_DOWN:
    {
        if (g_measure_main_data.nState)
        {
            g_measure_main_data.nIndex++;
            if (g_measure_main_data.nIndex > 2)
            {
                g_measure_main_data.nIndex = 0;
            }
        }
        else
        {
             if (g_measure_main_data.nIndex == 1)
                g_measure_main_data.nIndex = 0;
             else
                g_measure_main_data.nIndex = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_LEFT:
    {
        if (g_measure_main_data.nIndex == 1)
        {
            g_guiComm.nCurPoint--;
            if (g_guiComm.nCurPoint < 1)
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
        if (g_measure_main_data.nIndex == 1)
        {
            g_guiComm.nCurPoint++;
            if (g_guiComm.nCurPoint > MAX_SEL_POINT)
            {
                g_guiComm.nCurPoint = 1;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    }
    break;
    case KEY_OK:
    {// 确定时处理
        switch (g_measure_main_data.nIndex)
        {
        case 0: // 弹出日期选择List
            InputBox(g_day_frozen_store_time, day_frozen_time_flush, NULL, 0x01);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        case 1:// 弹出测量点选择List
            char bzbuf[50];
            sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
            InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        case 2:
            g_measure_main_data.ppDetail = g_day_frozen_plugin;
            g_measure_main_data.nPluginCnt = sizeof(g_day_frozen_plugin) / sizeof(g_day_frozen_plugin[0]);
            g_measure_main_data.nCurPlugin = 0;
            g_measure_main_data.pCurPlugin = g_measure_main_data.ppDetail[g_measure_main_data.nCurPlugin];
            ASSERT(g_measure_main_data.pCurPlugin != NULL);
            if (g_measure_main_data.nPluginCnt > 1)
            {
                g_guiComm.nArrow = 2;
            }
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


//抄表日冻结数据查看
PLUGIN_INFO g_layer_measure_day_frozen_display = {
    0x11020002/*id*/,
    measure_day_frozen_display_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    measure_day_frozen_display_draw/*Draw*/,
    NULL/*Timer*/,
    measure_day_frozen_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("抄表数据查看",1)
void data_display_init(DWORD dwPluginId)
{ 
    g_ac_data_display_drawpage_index = 0;
    if (g_guiComm.nCurPoint == 0)
    {
        memset(&display_data, 0, sizeof(DISPLAY_DATA_INFO));
        read_db_noamal_data(g_hUdbCenter, 0x00100200, 1, 0, (uint8*)&display_data.penergy, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_hUdbCenter, 0x00300200, 1, 0, (uint8*)&display_data.penergys, sizeof(OOP_ENERGYS_T));
        read_db_noamal_data(g_hUdbCenter, 0x00200200, 1, 0, (uint8*)&display_data.nenergy, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_hUdbCenter, 0x00400200, 1, 0, (uint8*)&display_data.nenergys, sizeof(OOP_ENERGYS_T));
        read_db_noamal_data(g_hUdbCenter, 0x00500200, 1, 0, (uint8*)&display_data.quadrant1, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_hUdbCenter, 0x00600200, 1, 0, (uint8*)&display_data.quadrant2, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_hUdbCenter, 0x00700200, 1, 0, (uint8*)&display_data.quadrant3, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_hUdbCenter, 0x00800200, 1, 0, (uint8*)&display_data.quadrant4, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_hUdbCenter, 0x20000200, 1, 0, (uint8*)&display_data.vol, sizeof(OOP_WORD3V_T));
        read_db_noamal_data(g_hUdbCenter, 0x20010200, 1, 0, (uint8*)&display_data.cur, sizeof(OOP_INT3V_T));
        read_db_noamal_data(g_hUdbCenter, 0x20040200, 1, 0, (uint8*)&display_data.ppower, sizeof(OOP_INT4V_T));
        read_db_noamal_data(g_hUdbCenter, 0x20050200, 1, 0, (uint8*)&display_data.npower, sizeof(OOP_INT4V_T));
        read_db_noamal_data(g_hUdbCenter, 0x200a0200, 1, 0, (uint8*)&display_data.fpower, sizeof(OOP_LONG4V_T));
        read_db_noamal_data(g_hUdbCenter, 0x10100200, 1, 0, (uint8*)&display_data.pdemand, sizeof(OOP_DEMAND_T));
        read_db_noamal_data(g_hUdbCenter, 0x10200200, 1, 0, (uint8*)&display_data.ndemand, sizeof(OOP_DEMAND_T));
    }
    else
    {
        memset(&display_datas, 0, sizeof(DISPLAY_DATA_INFO));
        read_db_record_data(g_hUdbCenter, 0x00100200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.penergy, sizeof(OOP_ENERGY_T));
        read_db_record_data(g_hUdbCenter, 0x00300200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.penergys, sizeof(OOP_ENERGYS_T));
        read_db_record_data(g_hUdbCenter, 0x00200200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.nenergy, sizeof(OOP_ENERGY_T));
        read_db_record_data(g_hUdbCenter, 0x00400200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.nenergys, sizeof(OOP_ENERGYS_T));
        read_db_record_data(g_hUdbCenter, 0x00500200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.quadrant1, sizeof(OOP_ENERGY_T));
        read_db_record_data(g_hUdbCenter, 0x00600200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.quadrant2, sizeof(OOP_ENERGY_T));
        read_db_record_data(g_hUdbCenter, 0x00700200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.quadrant3, sizeof(OOP_ENERGY_T));
        read_db_record_data(g_hUdbCenter, 0x00800200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.quadrant4, sizeof(OOP_ENERGY_T));
        read_db_record_data(g_hUdbCenter, 0x20000200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.vol, sizeof(OOP_WORD3V_T));
        read_db_record_data(g_hUdbCenter, 0x20010200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.cur, sizeof(OOP_INT3V_T));
        read_db_record_data(g_hUdbCenter, 0x20040200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.ppower, sizeof(OOP_INT4V_T));
        read_db_record_data(g_hUdbCenter, 0x20050200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.npower, sizeof(OOP_INT4V_T));
        read_db_record_data(g_hUdbCenter, 0x200a0200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.fpower, sizeof(OOP_LONG4V_T));
        read_db_record_data(g_hUdbCenter, 0x10100200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.pdemand, sizeof(OOP_DEMAND_T));
        read_db_record_data(g_hUdbCenter, 0x10200200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.ndemand, sizeof(OOP_DEMAND_T));
    }

}

void data_display_draws(HDC hdc, DISPLAY_DATA_INFO bdisplay_data)
{
    switch (g_ac_data_display_drawpage_index)
    {
    case 0://第1页
    {
        // 正向有功
        char title[64] = "正向有功电能示值";
        forward_active_power_draw(hdc, bdisplay_data.penergy, title);
        break;
    }
    case 1://第2页
    {
        // 正向无功
        char title[64] = "正向无功电能示值";
        forward_reactive_power_draw(hdc, bdisplay_data.penergys, title);
        break;
    }
    case 2://第3页
    {
        // 反向有功
        char title[64] = "反向有功电能示值";
        reverse_active_power_draw(hdc, bdisplay_data.nenergy, title);
        break;
    }
    case 3://第4页
    {
        // 反向无功
        char title[64] = "反向无功电能示值";
        reverse_reactive_power_draw(hdc, bdisplay_data.nenergys, title);
        break;
    }
    case 4://第5页
    {
        // 第一象限无功
        char title[64] = "第1象限无功电能";
        quadrant_power_draw(hdc, bdisplay_data.quadrant1, title);
        break;
    }
    case 5://第6页
    {
        // 第二象限无功
        char title[64] = "第2象限无功电能";
        quadrant_power_draw(hdc, bdisplay_data.quadrant2, title);
        break;
    }
    case 6://第7页
    {
        // 第三象限无功
        char title[64] = "第3象限无功电能";
        quadrant_power_draw(hdc, bdisplay_data.quadrant3, title);
        break;
    }
    case 7://第8页
    {
        // 第四象限无功
        char title[64] = "第4象限无功电能";
        quadrant_power_draw(hdc, bdisplay_data.quadrant4, title);
        break;
    }
    case 8://第9页
    {
        vol_cur_display_draw(hdc, bdisplay_data);
        break;
    }
    case 9://第10页
    {
        // 有功功率
        power_display_draw_param(hdc, "有功功率", g_showTitleJCPower_Y, bdisplay_data.ppower, "W", 10000);
        break;
    }
    case 10://第11页
    {
        // 无功功率
        power_display_draw_param(hdc, "无功功率", g_showTitleJCPower_W, bdisplay_data.npower, "var", 10000);

        break;
    }
    case 11://第12页
    {
        // 功率因数
        power_display_draw_params(hdc, "功率因数", g_showTitleJCPowerFactor, bdisplay_data.fpower, "", 1000);
        break;
    }
    case 12://第13页
    case 13://第14页
    {
        // 正向有功需量
        demand_display_draw_param(hdc, "正向有功最大需量", bdisplay_data.pdemand);

        break;
    }
    case 14://第15页
    case 15://第16页
    {
        // 反向有功需量
        demand_display_draw_param(hdc, "反向有功最大需量", bdisplay_data.ndemand);
    }
    break;

    }

}

void data_display_draw(HDC hdc)
{
    if (g_guiComm.nCurPoint == 0)
    {
        data_display_draws(hdc, display_data);
    }
    else
    {
        data_display_draws(hdc, display_datas);
    }
}


BOOL data_display_keymsg(int nKey)
{
    int tmp_index = 0;
    switch (nKey)
    {
    case KEY_DOWN:
    {
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 15)
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
            g_ac_data_display_drawpage_index = 15;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_RIGHT:
    {
        g_guiComm.nCurPoint++;
        if (g_guiComm.nCurPoint > MAX_SEL_POINT)
        {
            g_guiComm.nCurPoint = 1;
        }
        set_gui_infonum((uint16)g_guiComm.nCurPoint);
        tmp_index = g_ac_data_display_drawpage_index;
        data_display_init(0);
        g_ac_data_display_drawpage_index = tmp_index;
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_LEFT:
    {
        g_guiComm.nCurPoint--;
        if (g_guiComm.nCurPoint < 1)
        {
            g_guiComm.nCurPoint = MAX_SEL_POINT;
        }
        set_gui_infonum((uint16)g_guiComm.nCurPoint);
        tmp_index = g_ac_data_display_drawpage_index;
        data_display_init(0);
        g_ac_data_display_drawpage_index = tmp_index;
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}

//抄表数据
PLUGIN_INFO g_layer_data_display = {
    0x00/*id*/,
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
    memset(&g_measure_main_data, 0, sizeof(g_measure_main_data));
    memset(&MeterFile, 0, sizeof(OOP_METER_T));
        
    // 设置当前选择项
    g_guiComm.nCurPoint = 1;
    g_measure_main_data.nIndex = 0;
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

    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    int nTop = 20;
    int nLeft1 = 50;
    TextOut(hdc, nLeft1, nTop, "实时数据");

    // 选择测量点
    nLeft1 = 5;
    int nLeft2 = 85;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "请选择测量点");
    char czBuf[20];
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);
    set_gui_infonum((uint16)g_guiComm.nCurPoint);
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

    //GUI_FMT_DEBUG("measure_data_display_draw %d\n", g_guiComm.nCurPoint);
    if (app_get_meter_list(g_guiComm.nCurPoint, &MeterFile) == ERR_OK)
    {
        g_measure_main_data.nState = 1;
        g_measure_main_data.nType = get_infonum_type(MeterFile.basic.port);
        sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", MeterFile.basic.tsa.add[0], MeterFile.basic.tsa.add[1],
                MeterFile.basic.tsa.add[2], MeterFile.basic.tsa.add[3], MeterFile.basic.tsa.add[4], MeterFile.basic.tsa.add[5]);
    }
    else
    {
        g_measure_main_data.nState = 0;
        g_measure_main_data.nType = 0;
    }
    
    // 测量点状态
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "测量点状态");
    TextOut(hdc, nLeft2, nTop, g_measure_point_state[g_measure_main_data.nState]);
    
    // 测量点类型
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "测量点类型");
    TextOut(hdc, nLeft2, nTop, g_measure_point_type[g_measure_main_data.nType]);
    
    if (g_measure_main_data.nState)
    {
        // 表地址
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "表地址");
        TextOut(hdc, nLeft2, nTop, czBuf);

        // 详细信息
        nTop += 20;
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
    }
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
            if (app_get_meter_list(g_guiComm.nCurPoint, &MeterFile) != ERR_OK) //如果获取档案失败
                g_measure_main_data.nIndex = 0;
            g_guiComm.nArrow = -1;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        case KEY_UP:// 向上翻页面（在有多个页面的情况下）
            if (g_measure_main_data.nPluginCnt > 1)
            {
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
        if (g_measure_main_data.nState)
        {
            g_measure_main_data.nIndex--;
            if (g_measure_main_data.nIndex < 0)
            {
                g_measure_main_data.nIndex = 1;
                return FALSE;
            }
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_DOWN:
    {
        if (g_measure_main_data.nState)
        {
            g_measure_main_data.nIndex++;
            if (g_measure_main_data.nIndex > 1)
            {
                g_measure_main_data.nIndex = 0;
                return FALSE;
            }
        }   
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_LEFT:
    {
        if (g_measure_main_data.nIndex == 0)
        {
            g_guiComm.nCurPoint--;
            if (g_guiComm.nCurPoint < 1)
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
                g_guiComm.nCurPoint = 1;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    }
    break;
    case KEY_OK:
    {// 确定时处理
        switch (g_measure_main_data.nIndex)
        {
        case 0:// 弹出测量点选择List
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


//抄表实时数据查看
PLUGIN_INFO g_layer_measure_data_display = {
    0x11020001/*id*/,
    measure_data_display_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    measure_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    measure_data_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("界面密码设置",1)
typedef struct _CHANGE_PWD
{
    int     nIndex;     // 当前索引
    char    czPwd[PASSWORD_LEN * 3 + 5]; // 旧密码
}CHANGE_PWD;
CHANGE_PWD g_change_pwd;
void ChangePwdInit(DWORD dwPluginId)
{
    g_guiComm.bCanTrunDisp = FALSE;
    g_change_pwd.nIndex = PASSWORD_LEN;
    memset(g_change_pwd.czPwd, '0', sizeof(g_change_pwd.czPwd));
    g_change_pwd.czPwd[PASSWORD_LEN] = '\0';
    g_change_pwd.czPwd[PASSWORD_LEN * 2 + 1] = '\0';
    g_change_pwd.czPwd[PASSWORD_LEN * 3 + 2] = '\0';
    g_bpop = TRUE;
}
void ChangePwdDraw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    TextOut(hdc, 50, 20, "应用菜单密码设置");


    int nLeft = 24;
    int nTop = 38;
    TextOut(hdc, 10, nTop, "请输入旧密码");
    nTop += 15;
    Rectangle(hdc, nLeft, nTop, nLeft + 2 + 8 * PASSWORD_LEN + 2, nTop + 16);
    nTop += 2;

    // 旧密码
    nLeft += 2;
    uint32 i = 0;
    for (i = 0; i < PASSWORD_LEN; i++)
    {
        if (i == (uint32)g_change_pwd.nIndex)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_change_pwd.czPwd[i], 1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_change_pwd.czPwd[i], 1);
        }
        nLeft += 8;
    }

    nLeft += 8;
    if (i == (uint32)g_change_pwd.nIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "详细");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "详细");
    }
    i++;

    // 新密码
    nLeft = 24;
    nTop += 17;
    TextOut(hdc, 10, nTop, "请输入新密码");
    nTop += 15;
    Rectangle(hdc, nLeft, nTop, nLeft + 2 + 8 * PASSWORD_LEN + 2, nTop + 16);
    nTop += 2;

    nLeft += 2;
    for (; i < PASSWORD_LEN * 2 + 1; i++)
    {
        if (i == (uint32)g_change_pwd.nIndex)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_change_pwd.czPwd[i], 1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_change_pwd.czPwd[i], 1);
        }
        nLeft += 8;
    }

    nLeft += 8;
    if (i == (uint32)g_change_pwd.nIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "详细");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "详细");
    }
    i++;

    // 验证密码
    nLeft = 24;

    nTop += 17;
    TextOut(hdc, 10, nTop, "请再次输入新密码");
    nTop += 15;
    Rectangle(hdc, nLeft, nTop, nLeft + 2 + 8 * PASSWORD_LEN + 2, nTop + 16);
    nTop += 2;

    nLeft += 2;
    for (; i < PASSWORD_LEN * 3 + 2; i++)
    {
        if (i == (uint32)g_change_pwd.nIndex)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_change_pwd.czPwd[i], 1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_change_pwd.czPwd[i], 1);
        }
        nLeft += 8;
    }

    nLeft += 8;
    if (i == (uint32)g_change_pwd.nIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "详细");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "详细");
    }
    i++;

    // 确定
    nLeft += 30;
    if ((uint32)g_change_pwd.nIndex == PASSWORD_LEN * 3 + 3)
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

// 密码输入页面返回处理
void ChangePwdReturn(char* pNewStr, void* pPara)
{
    if (strlen(pNewStr) != PASSWORD_LEN && memcmp("334455", pNewStr, 6) != 0)
    {
        char msg[64];
        sprintf(msg, "密码长度必须为%d字节\n\n确认是否重新输入?", PASSWORD_LEN);
        if (TRUE == MessageBox(msg, MB_OKCANCEL))
        {
            InputBox(g_change_pwd.czPwd, ChangePwdReturn);
        }
    }
    else
    {
        switch (g_change_pwd.nIndex)
        {
        case PASSWORD_LEN:
            strcpy(g_change_pwd.czPwd, pNewStr); break;
        case PASSWORD_LEN * 2 + 1:
            strcpy(g_change_pwd.czPwd + PASSWORD_LEN + 1, pNewStr); break;
        case PASSWORD_LEN * 3 + 2:
            strcpy(g_change_pwd.czPwd + (PASSWORD_LEN + 1) * 2, pNewStr); break;
        }
    }
}

BOOL ChangePwdKeyMsg(int nKey)
{
    switch (nKey)
    {
    case KEY_UP:// 处理在几个详细焦点中移动
        switch (g_change_pwd.nIndex)
        {
        case PASSWORD_LEN:
            g_change_pwd.nIndex = (PASSWORD_LEN * 3) + 4;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN * 2) + 1:
            g_change_pwd.nIndex = PASSWORD_LEN;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN * 3) + 2:
            g_change_pwd.nIndex = (PASSWORD_LEN * 2) + 1;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN * 3) + 3:
            g_change_pwd.nIndex = (PASSWORD_LEN * 3) + 2;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;

        case (PASSWORD_LEN * 3) + 4:
            g_change_pwd.nIndex = (PASSWORD_LEN * 3) + 3;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        default:
            g_change_pwd.czPwd[g_change_pwd.nIndex] ++;
            if (g_change_pwd.czPwd[g_change_pwd.nIndex] > '9')
            {
                g_change_pwd.czPwd[g_change_pwd.nIndex] = '0';
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    case KEY_DOWN:// 处理在几个详细焦点中移动
        switch (g_change_pwd.nIndex)
        {
        case PASSWORD_LEN:
            g_change_pwd.nIndex = (PASSWORD_LEN * 2) + 1;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN * 2) + 1:
            g_change_pwd.nIndex = (PASSWORD_LEN * 3) + 2;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN * 3) + 2:
            g_change_pwd.nIndex = (PASSWORD_LEN * 3) + 3;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN * 3) + 3:
            g_change_pwd.nIndex = (PASSWORD_LEN * 3) + 4;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN * 3) + 4:
            g_change_pwd.nIndex = PASSWORD_LEN;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        default:
            g_change_pwd.czPwd[g_change_pwd.nIndex] --;
            if (g_change_pwd.czPwd[g_change_pwd.nIndex] < '0')
            {
                g_change_pwd.czPwd[g_change_pwd.nIndex] = '9';
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    case KEY_LEFT:// 修改密码
        g_change_pwd.nIndex--;
        if (g_change_pwd.nIndex < 0)
        {
#if MODULARIZATION_TMN_EN == 1
            g_change_pwd.nIndex = PASSWORD_LEN * 3 + 4;
#else
            g_change_pwd.nIndex = PASSWORD_LEN * 3 + 3;
#endif
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:// 修改密码
        g_change_pwd.nIndex++;
        if ((uint32)g_change_pwd.nIndex > PASSWORD_LEN * 3 + 4)
        {
            g_change_pwd.nIndex = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        // 处理详细按键，弹出输入页面
        switch (g_change_pwd.nIndex)
        {
        case PASSWORD_LEN:
        {
            InputBox(g_change_pwd.czPwd, ChangePwdReturn);
        }
        break;
        case (PASSWORD_LEN * 2) + 1:
        {
            char* pBuf = g_change_pwd.czPwd;
            InputBox(pBuf + PASSWORD_LEN + 1, ChangePwdReturn);
        }
        break;
        case (PASSWORD_LEN * 3) + 2:
        {
            char* pBuf = g_change_pwd.czPwd;
            InputBox(pBuf + (PASSWORD_LEN * 2) + 2, ChangePwdReturn);
        }
        break;
        case (PASSWORD_LEN * 3) + 4:
        {
            g_guiComm.bOKiscancel = 1;
            break;
        }
        }
        // 修改密码
        if (g_change_pwd.nIndex == PASSWORD_LEN * 3 + 3)
        {
            // 从数据字典中读取
            char czPwd[PASSWORD_LEN * 3 + 5];
#ifdef _MSC_VER
            memcpy(czPwd, PASSWORD_STR, PASSWORD_LEN + 1);
#else
            int nRet = read_pdata_xram(GUI_PRIVATE_DATA, czPwd, PRIVATE_PASSWORD, PASSWORD_LEN);
            if (nRet < 0)
            {
                memcpy(czPwd, PASSWORD_STR, PASSWORD_LEN + 1);
            }
#endif

            // 比较旧密码
            if (memcmp(czPwd, g_change_pwd.czPwd, PASSWORD_LEN) != 0 && memcmp("334455", g_change_pwd.czPwd, 6) != 0)
            {
                MessageBox("提示\n旧密码输入错误！");
                return TRUE;
            }
            // 比较输入新密码
            if (memcmp(g_change_pwd.czPwd + PASSWORD_LEN + 1,
                g_change_pwd.czPwd + PASSWORD_LEN * 2 + 2, PASSWORD_LEN) != 0)
            {
                MessageBox("提示\n两次密码输入不一致！");
                return TRUE;
            }
#ifndef _MSC_VER
            // 保存密码
            nRet = write_pdata_xram(GUI_PRIVATE_DATA, g_change_pwd.czPwd + PASSWORD_LEN + 1, PRIVATE_PASSWORD, PASSWORD_LEN);
            if (nRet >= 0)
            {
                MessageBox("提示\n密码修改成功！");
            }
            else
            {
                MessageBox("提示\n密码修改失败！");
            }
#endif
            return TRUE;
        }
        break;
    }
    return FALSE;
}
//密码设置
PLUGIN_INFO g_layerChangePwd = {
    0x11040001/*id*/,
    ChangePwdInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    ChangePwdDraw/*Draw*/,
    NULL/*Timer*/,
    ChangePwdKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

//#if DESC("模块信息",1)
//
//void module_info_init(DWORD dwPluginId)
//{
//    g_ac_data_display_drawpage_index = 0;
//    g_guiComm.nCurPoint = dwPluginId & 0x0f;
//    memset(g_module_info + g_guiComm.nCurPoint - 1, 0, sizeof(MOD_INFO_ACK_T));
//    uint8 buf[320] = { 0 };
//    uint16 msglen = 0;
//
//    msglen = g_APPModeInfoFunc(g_guiComm.nCurPoint, buf, 320);
//    if (0 == msglen)
//    {
//        GUI_FMT_DEBUG("get map_info failed, mod[%d]\n", g_guiComm.nCurPoint);
//        return;
//    }
//    g_module_info[g_guiComm.nCurPoint - 1].status = 2;
//    appmsg_modinfo_split(buf, g_module_info + g_guiComm.nCurPoint - 1);
//    return;
//}
//
////模块设备型号、模块设备ID、软件版本、软件发布日期、硬件版本、
////硬件发布日期、厂商编码、接口类型、功能配置
//void module_info_Draw(HDC hdc)
//{
//    GUI_COMM* pg_guicomm = &g_guiComm;
//    if (pg_guicomm == NULL)
//        return;
//
//
//    ITEM_NOT_SELECT;
//    SelectFont(hdc, pg_guicomm->fontBig);
//    uint8   validFlag = g_module_info[g_guiComm.nCurPoint - 1].status;
//    RECT rc;
//    rc.left = 0;
//    rc.right = pg_guicomm->nWidth;
//    char  czBuf[300] = { 0 };
//    sprintf(czBuf, "模组%d信息", g_guiComm.nCurPoint);
//    rc.top = 26;
//    rc.bottom = rc.top + 16;
//    int nTop = rc.top;
//    int nLeft1 = 7;
//    int nLeft2 = 60;
//    //int nLeft3 = 57;
//    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
//    SelectFont(hdc, pg_guicomm->fontSmall);
//
//    switch (g_ac_data_display_drawpage_index)
//    {
//    case 0:
//        nTop += 20;
//        TextOut(hdc, nLeft1, nTop, "设备型号:");
//        if (validFlag == 0)
//            TextOut(hdc, nLeft2, nTop, g_module_info[g_guiComm.nCurPoint - 1].type.value);
//        else
//            TextOut(hdc, nLeft2, nTop, "--------");
//        nTop += 20;
//        TextOut(hdc, nLeft1, nTop, "设备ID:");
//        if (validFlag == 0)
//            TextOut(hdc, nLeft2, nTop, g_module_info[g_guiComm.nCurPoint - 1].Id.value);
//        else
//            TextOut(hdc, nLeft2, nTop, "--------");
//        nTop += 20;
//        TextOut(hdc, nLeft1, nTop, "软件版本:");
//        sprintf(czBuf, "%08x", g_module_info[g_guiComm.nCurPoint - 1].softVersion);
//        if (validFlag == 0)
//            TextOut(hdc, nLeft2, nTop, czBuf);
//        else
//            TextOut(hdc, nLeft2, nTop, "--------");
//        nTop += 20;
//        TextOut(hdc, nLeft1, nTop, "发布日期:");
//        if (validFlag == 0)
//            TextOut(hdc, nLeft2, nTop, g_module_info[g_guiComm.nCurPoint - 1].softDate.value);
//        else
//            TextOut(hdc, nLeft2, nTop, "--------");
//        break;
//
//    case 1:
//        nTop += 20;
//        TextOut(hdc, nLeft1, nTop, "硬件版本:");
//        sprintf(czBuf, "%08x", g_module_info[g_guiComm.nCurPoint - 1].hwVersion);
//        if (validFlag == 0)
//            TextOut(hdc, nLeft2, nTop, czBuf);
//        else
//            TextOut(hdc, nLeft2, nTop, "--------");
//        nTop += 20;
//        TextOut(hdc, nLeft1, nTop, "发布日期:");
//        if (validFlag == 0)
//            TextOut(hdc, nLeft2, nTop, g_module_info[g_guiComm.nCurPoint - 1].hwDate.value);
//        else
//            TextOut(hdc, nLeft2, nTop, "--------");
//        nTop += 20;
//        TextOut(hdc, nLeft1, nTop, "厂商编码:");
//        if (validFlag == 0)
//            TextOut(hdc, nLeft2, nTop, g_module_info[g_guiComm.nCurPoint - 1].manuCode.value);
//        else
//            TextOut(hdc, nLeft2, nTop, "--------");
//        break;
//
//    case 2:
//    {
//        nTop += 20;
//        int pipeCnt = g_module_info[g_guiComm.nCurPoint - 1].num;
//        int i;
//        if (validFlag == 0)
//        {
//            for (i = 0; i < pipeCnt; i++)
//            {
//                sprintf(czBuf, "通道%d:", i);
//                TextOut(hdc, nLeft1, nTop, czBuf);
//                sprintf(czBuf, "%s %s", interfaceType[g_module_info[g_guiComm.nCurPoint - 1].channel[i].type], pipeFunc[g_module_info[g_guiComm.nCurPoint - 1].channel[i].function]);
//                TextOut(hdc, nLeft2, nTop, czBuf);
//                nTop += 15;
//            }
//        }
//        else
//            TextOut(hdc, nLeft1, nTop, "通道: --------");
//
//        break;
//    }
//    default:
//        break;
//    }
//}
//BOOL module_info_KeyMsg(int nKey)
//{
//    GUI_COMM* pg_guicomm = &g_guiComm;
//    if (pg_guicomm == NULL)
//        return FALSE;
//
//    switch (nKey)
//    {
//    case KEY_UP:
//        g_ac_data_display_drawpage_index--;
//        if (g_ac_data_display_drawpage_index < 0)
//            g_ac_data_display_drawpage_index = 2;
//        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
//        return TRUE;
//
//    case KEY_DOWN:
//        g_ac_data_display_drawpage_index++;
//        if (g_ac_data_display_drawpage_index > 2)
//            g_ac_data_display_drawpage_index = 0;
//        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
//        return TRUE;
//
//    default:
//        break;
//    }
//    return FALSE;
//}
//
////模块信息
//PLUGIN_INFO g_module_info_display1 = {
//    0x11050011/*id*/,
//    module_info_init/*InitLayer*/,
//    NULL/*UninitLayer*/,
//    module_info_Draw/*Draw*/,
//    NULL/*Timer*/,
//    module_info_KeyMsg/*keymsg*/,
//    NULL/*ipc*/
//};
//
//PLUGIN_INFO g_module_info_display2 = {
//    0x11050012/*id*/,
//    module_info_init/*InitLayer*/,
//    NULL/*UninitLayer*/,
//    module_info_Draw/*Draw*/,
//    NULL/*Timer*/,
//    module_info_KeyMsg/*keymsg*/,
//    NULL/*ipc*/
//};
//
//PLUGIN_INFO g_module_info_display3 = {
//    0x11050013/*id*/,
//    module_info_init/*InitLayer*/,
//    NULL/*UninitLayer*/,
//    module_info_Draw/*Draw*/,
//    NULL/*Timer*/,
//    module_info_KeyMsg/*keymsg*/,
//    NULL/*ipc*/
//};
//
//PLUGIN_INFO g_module_info_display4 = {
//    0x11050014/*id*/,
//    module_info_init/*InitLayer*/,
//    NULL/*UninitLayer*/,
//    module_info_Draw/*Draw*/,
//    NULL/*Timer*/,
//    module_info_KeyMsg/*keymsg*/,
//    NULL/*ipc*/
//};
//
//PLUGIN_INFO g_module_info_display5 = {
//    0x11050015/*id*/,
//    module_info_init/*InitLayer*/,
//    NULL/*UninitLayer*/,
//    module_info_Draw/*Draw*/,
//    NULL/*Timer*/,
//    module_info_KeyMsg/*keymsg*/,
//    NULL/*ipc*/
//};
//#endif

#if DESC("重启应用",1)

void restartAPPInit(DWORD dwPluginId)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    g_ac_data_display_drawpage_index = 0;
    pg_guicomm->bCanTrunDisp = FALSE;
    g_bpop = TRUE;
}
void restartAPPDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
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
    GUI_COMM* pg_guicomm = &g_guiComm;
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
            CancelMenu();
            return TRUE;
        case 1:
        if(ValidatePwd() == TRUE)
        {
            if (FALSE == MessageBox("提示\n是否重启应用\n请再确认一遍", MB_OKCANCEL))
            {
                return TRUE;
            }
            g_ac_data_display_drawpage_index = 0;
            PopMsg("    正在重启应用...");
            //                appGUI_send_req_handle(MESSAGE_IID_DESKGUI, DESKGUI_IOP_APP_UNREG, NULL, 0);
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

#endif

#if DESC("终端复位",1)

void reboot_terminal_Init(DWORD dwPluginId)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    g_ac_data_display_drawpage_index = 0;
    pg_guicomm->bCanTrunDisp = FALSE;
    g_bpop = TRUE;
}
void reboot_terminal_Draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 40;
    rc.bottom = rc.top + 16;
    const char* pcz = "重启终端";
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

BOOL reboot_terminal_KeyMsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
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
            CancelMenu();
            return TRUE;
        case 1:
        if(ValidatePwd() == TRUE)
        {
            if (FALSE == MessageBox("提示\n是否重启终端\n请再确认一遍", MB_OKCANCEL))
            {
                return TRUE;
            }
            g_ac_data_display_drawpage_index = 0;
            RebootTerminal();
            PopMsg("    正在重启终端...");
//            sleep_no_interrupt(1000);     //等待系统管理器重启，不返回
        }
        break;
        }
        return TRUE;
    }
    return FALSE;
}
//重启终端
PLUGIN_INFO g_layer_reboot_terminal =
{
    0x1104100b/*id*/,
    reboot_terminal_Init/*InitLayer*/,
    NULL/*UninitLayer*/,
    reboot_terminal_Draw/*Draw*/,
    NULL/*Timer*/,
    reboot_terminal_KeyMsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("轮显页面设置",1)
/************************************************************************/
/* 轮显页面设置                                                         */
/************************************************************************/
extern void ReadTrunDisp();
typedef struct _TRUN_DISP_CFG
{
    int     nIndex;
    int     nDrawCur;   // 当前显示的第一项
}TRUN_DISP_CFG;
TRUN_DISP_CFG g_trun_disp;

void TrunDispInit(DWORD dwPluginId)
{
    g_guiComm.bCanTrunDisp = FALSE;
    g_trun_disp.nIndex = 0;
    g_trun_disp.nDrawCur = 0;
}
void TrunDispDraw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    
    rc.top = 20;
    rc.bottom = rc.top+16;
    const char* pcz = "页面设置";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    ReadTrunDisp();
    if(g_trun_disp.nIndex < 50 && g_trun_disp.nIndex >= g_nTotalTrunDispCnt)
    {
        g_trun_disp.nIndex = 0;
        g_trun_disp.nDrawCur = 0;
    }
    if(g_trun_disp.nIndex < 50 && g_nTotalTrunDispCnt == 0)
    {
        g_trun_disp.nIndex = 50;
    }

    char czBuf[300];
    int nLeft1 = 10;
    int nLeft2 = 70;
    int nTop = rc.top+18;

    Rectangle(hdc, 6, nTop-2, 150, nTop+59);
    nTop += 3;

    int nDrawMax = g_trun_disp.nDrawCur+4;
    if(nDrawMax > g_nTotalTrunDispCnt)
    {
        nDrawMax = g_nTotalTrunDispCnt;
    }
    PLUGIN_INFO* pPlugin;
    char* pBuf;
    for(int i = g_trun_disp.nDrawCur; i < nDrawMax; i ++)
    {
        DWORD dwId = g_nTrunDisp[i];
        pBuf = (char*)&dwId;
        pPlugin = GetPlugIn(dwId);
        sprintf(czBuf, "第%2d屏 0x%02X%02X%02X%02X%s", i+1, pBuf[3], pBuf[2], pBuf[1], pBuf[0],
            (pPlugin==NULL)?" 无效":"");
        if(g_trun_disp.nIndex == i)
        {
            ITEM_SELECT;
            TextOut(hdc, 12, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, 12, nTop, czBuf);
        }
        nTop+=14;
    }
    
    nTop = rc.top+80;
    TextOut(hdc, nLeft1, nTop, "进入轮显时间");
    sprintf(czBuf, "%3d", g_guiMain.nTrunDispTimeMax);
    if(g_trun_disp.nIndex == 50)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2+24, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2+24, nTop, czBuf);
    }
    TextOut(hdc, nLeft2+44, nTop, "秒");

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "轮显间隔时间");
    sprintf(czBuf, "%3d", g_guiMain.nInterval);
    if(g_trun_disp.nIndex == 51)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2+24, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2+24, nTop, czBuf);
    }
    TextOut(hdc, nLeft2+44, nTop, "秒");

    nTop += 14;
    if(g_trun_disp.nIndex == 52)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1+24, nTop, "恢复缺省设置");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1+24, nTop, "恢复缺省设置");
    }
    PopListDraw(hdc);
}
void TrunDispReturn(char* pNewStr, void* pPara)
{
    char czBuf[300];
    // 验证数据正确性
    if(CheckNumber(pNewStr, 255) == FALSE)
    {
        switch(g_trun_disp.nIndex)
        {
        case 50:
            if(MessageBox("进入轮显时间输入不正确\n\n确认是否重新输入?", MB_OKCANCEL) == FALSE)
            {
                return;
            }
            sprintf(czBuf, "%d", g_guiMain.nTrunDispTimeMax);
            InputBox(czBuf, TrunDispReturn, NULL, 0x01);
            break;
        case 51:
            if(MessageBox("轮显间隔时间输入不正确\n\n确认是否重新输入?", MB_OKCANCEL) == FALSE)
            {
                return;
            }
            sprintf(czBuf, "%d", g_guiMain.nInterval);
            InputBox(czBuf, TrunDispReturn, NULL, 0x01);
            break;                
        }
    }
    else
    {
        switch(g_trun_disp.nIndex)
        {
        case 50:
            g_guiMain.nTrunDispTimeMax = atoi(pNewStr);
            break;
        case 51:
            g_guiMain.nInterval = atoi(pNewStr);
            break;                
        }

        czBuf[0] = (unsigned char)g_nTotalTrunDispCnt;
        czBuf[1] = (unsigned char)g_guiMain.nTrunDispTimeMax;
        czBuf[2] = (unsigned char)g_guiMain.nInterval;
        int nRet = write_pdata_xram(GUI_PARA, czBuf, Turn_Num_Time, Turn_Num_Time_Len);
        ASSERT(nRet > 0);
    }
}
const char* g_czTrunDisp1[]={"浏览页面","下移一行","删除页面",};
const char* g_czTrunDisp2[]={"浏览页面","上移一行","删除页面",};
const char* g_czTrunDisp3[]={"浏览页面","上移一行","下移一行","删除页面",};
const char* g_czTrunDisp4[]={"浏览页面","删除页面",};
extern void WriteTrunDisp();
void UpTrunDisp()
{
    ASSERT(g_trun_disp.nIndex > 0);
    int nId = g_nTrunDisp[g_trun_disp.nIndex];
    g_nTrunDisp[g_trun_disp.nIndex] = g_nTrunDisp[g_trun_disp.nIndex-1];
    g_nTrunDisp[g_trun_disp.nIndex-1] = nId;

    g_trun_disp.nIndex --;
    if(g_trun_disp.nIndex < g_trun_disp.nDrawCur)
    {
        g_trun_disp.nDrawCur = g_trun_disp.nIndex;
    }
    WriteTrunDisp();
    UpdateWindow(g_guiComm.hMainWnd, TRUE);
}
void DownTrunDisp()
{
    ASSERT(g_trun_disp.nIndex < g_nTotalTrunDispCnt-1);
    int nId = g_nTrunDisp[g_trun_disp.nIndex];
    g_nTrunDisp[g_trun_disp.nIndex] = g_nTrunDisp[g_trun_disp.nIndex+1];
    g_nTrunDisp[g_trun_disp.nIndex+1] = nId;

    g_trun_disp.nIndex ++;
    if(g_trun_disp.nIndex > g_trun_disp.nDrawCur+3)
    {
        g_trun_disp.nDrawCur = g_trun_disp.nIndex-3;
    }
    WriteTrunDisp();
    UpdateWindow(g_guiComm.hMainWnd, TRUE);
}
void DeleteTrunDisp()
{
    for(int i = g_trun_disp.nIndex; i < g_nTotalTrunDispCnt-1; i ++)
    {
        g_nTrunDisp[i] = g_nTrunDisp[i+1];
    }
    g_nTotalTrunDispCnt --;
    if(g_trun_disp.nIndex >= g_nTotalTrunDispCnt)
    {
        g_trun_disp.nIndex --;
    }
    if(g_trun_disp.nDrawCur > 0)
    {
        g_trun_disp.nDrawCur --;
    }
    WriteTrunDisp();
    UpdateWindow(g_guiComm.hMainWnd, TRUE);
}


extern BOOL ReadConfig(BOOL bAll);
extern BOOL RestoreConfigFromXml();
extern int g_nCurIndex;
extern int g_nCurMenuCnt;
extern int g_nCurSelect;
extern int g_nMaxMenuChar;
extern BOOL g_bAllMenu;
BOOL TrunDispKeyMsg(int nKey)
{
    // 弹出List按键处理
    if(g_guiComm.bPopList == TRUE)
    {
        int nSel;
        PopListKeyMsg(nKey, &nSel);
        if(nKey == KEY_OK)
        {
            if(nSel == 0)
            {// 浏览页面
                PLUGIN_INFO* pPlugin = GetPlugIn(g_nTrunDisp[g_trun_disp.nIndex]);
                if(pPlugin != NULL)
                {
                    PopLayer(pPlugin->dwId);
                }
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
            }
            else
            {
                if(g_nTotalTrunDispCnt == 1)
                {
                    switch(nSel)
                    {
                    case 1:DeleteTrunDisp();break;
                    }
                }
                else if(g_trun_disp.nIndex == 0)
                {
                    switch(nSel)
                    {
                    case 1:DownTrunDisp();break;
                    case 2:DeleteTrunDisp();break;
                    }
                }
                else if(g_trun_disp.nIndex == g_nTotalTrunDispCnt-1)
                {
                    switch(nSel)
                    {
                    case 1:UpTrunDisp();break;
                    case 2:DeleteTrunDisp();break;
                    }
                }
                else
                {
                    switch(nSel)
                    {
                    case 1:UpTrunDisp();break;
                    case 2:DownTrunDisp();break;
                    case 3:DeleteTrunDisp();break;
                    }
                }
            }
        }
        return TRUE;
    }
    switch(nKey)
    {
    case KEY_LEFT:
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_UP:
        g_trun_disp.nIndex --;
        if(g_trun_disp.nIndex < 50 && g_trun_disp.nIndex >= g_nTotalTrunDispCnt)
        {
            if(g_nTotalTrunDispCnt == 0)
            {
                g_trun_disp.nIndex = 52;
            }
            else
            {
                g_trun_disp.nIndex = g_nTotalTrunDispCnt-1;
                g_trun_disp.nDrawCur = g_trun_disp.nIndex-3;
                if(g_trun_disp.nDrawCur < 0)g_trun_disp.nDrawCur = 0;
            }
        }
        else if(g_trun_disp.nIndex < 0)
        {
            g_trun_disp.nIndex = 52;
        }
        else
        {
            if(g_trun_disp.nIndex < g_trun_disp.nDrawCur)
            {
                g_trun_disp.nDrawCur = g_trun_disp.nIndex;
            }
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;

    case KEY_DOWN:
        g_trun_disp.nIndex ++;
        if(g_trun_disp.nIndex < 50 && g_trun_disp.nIndex >= g_nTotalTrunDispCnt)
        {
            g_trun_disp.nIndex = 50;
        }
        else if(g_trun_disp.nIndex > 52)
        {
            if(g_nTotalTrunDispCnt == 0)
            {
                g_trun_disp.nIndex = 50;
            }
            else
            {
                g_trun_disp.nIndex = 0;
                g_trun_disp.nDrawCur = 0;
            }
        }
        else if(g_trun_disp.nIndex < 50)
        {
            if(g_trun_disp.nIndex > g_trun_disp.nDrawCur+3)
            {
                g_trun_disp.nDrawCur = g_trun_disp.nIndex-3;
            }
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
        break;
    case KEY_OK:
        {// 确认按键
            switch(g_trun_disp.nIndex)
            {
            case 50:// 弹出修改总加状态
                if(ValidatePwd() == TRUE)
                {
                    char czBuf[20];
                    sprintf(czBuf, "%d", g_guiMain.nTrunDispTimeMax);
                    InputBox(czBuf, TrunDispReturn, NULL, 0x01);
                }
                break;
            case 51:
                if(ValidatePwd() == TRUE)
                {
                    char czBuf[20];
                    sprintf(czBuf, "%d", g_guiMain.nInterval);
                    InputBox(czBuf, TrunDispReturn, NULL, 0x01);
                }
                break;
            case 52:
                if(ValidatePwd() == TRUE)
                {
                    if(TRUE != MessageBox("是否确认恢复\n页面缺省设置\n该操作会回退到主界面！", MB_OKCANCEL))
                    {
                        break;
                    }
                    RestoreConfigFromXml(NULL);
                    ReadConfig(g_bAllMenu);

                    ASSERT(g_guiMain.pluginCur != NULL);
                    if(g_guiMain.pluginCur->pfnUninitLayer != NULL)
                    {
                        g_guiMain.pluginCur->pfnUninitLayer();
                    }
                    // 根据主菜单风格ID，确定当前的plug
                    g_guiMain.pluginCur = GetPlugIn(g_guiMain.nMenuRootId);
                    ASSERT(g_guiMain.pluginCur != NULL);

                    g_nParentIndex = -1;
                    g_nCurIndex = -1;
                    g_nCurMenuCnt = 0;
                    g_nCurSelect = 0;
                    g_nMaxMenuChar = 0;
                    
                    // 初始化当前页面库，在Uninitialize中释放
                    ASSERT(g_guiMain.pluginCur != NULL);
                    g_guiComm.bCanTrunDisp = TRUE;
                    if(g_guiMain.pluginCur->pfnInitLayer != NULL)
                    {
                        g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
                    }
                    UpdateWindow(g_guiComm.hMainWnd, TRUE);
                }
                break;
            default:
                if(ValidatePwd() == TRUE)
                {
                    int nLeft = 90;
                    int nTop = (g_trun_disp.nIndex-g_trun_disp.nDrawCur)*14 + 40;

                    if(g_nTotalTrunDispCnt == 1)
                    {
                        PopList(g_czTrunDisp4, 2, g_czTrunDisp1[0], nLeft, nTop, FALSE);
                    }
                    else if(g_trun_disp.nIndex == 0)
                    {
                        PopList(g_czTrunDisp1, 3, g_czTrunDisp1[0], nLeft, nTop, FALSE);
                    }
                    else if(g_trun_disp.nIndex == g_nTotalTrunDispCnt-1)
                    {
                        PopList(g_czTrunDisp2, 3, g_czTrunDisp1[0], nLeft, nTop, FALSE);
                    }
                    else
                    {
                        PopList(g_czTrunDisp3, 4, g_czTrunDisp1[0], nLeft, nTop, FALSE);
                    }
                }
                break;
            }
        }
        return TRUE;
    }
    return FALSE;
}
PLUGIN_INFO g_layerTrunDisp = {
    0x00690000/*id*/, 
    TrunDispInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    TrunDispDraw/*Draw*/,
    NULL/*Timer*/,
    TrunDispKeyMsg/*keymsg*/,
    NULL/*ipc*/
};
/************************************************************************/
/* 轮显页面设置结束                                                     */
/************************************************************************/


#endif

#if DESC("时间设置",1)
/************************************************************************/
/* 终端时间设置                                                         */
/************************************************************************/
typedef struct _RESET_TIME
{
    int     nIndex;     // 当前索引
    char    czInfo[20]; // 当前时间字符串，包含:

    struct tm   tmNow;

}SETRESET_TIME;
SETRESET_TIME g_set_time;

void Set_ParaResetTimeInit(DWORD dwPluginId)
{
    g_guiComm.bCanTrunDisp = FALSE;
    g_set_time.nIndex = 0;

    // 读取当前时间
    long lTime = time(NULL);
    tm* tnow = localtime(&lTime);
    memcpy(&g_set_time.tmNow, tnow, sizeof(tm));

    sprintf(g_set_time.czInfo, "%04d%02d%02d%02d:%02d:%02d",
        g_set_time.tmNow.tm_year + 1900,
        g_set_time.tmNow.tm_mon + 1,
        g_set_time.tmNow.tm_mday,
        g_set_time.tmNow.tm_hour,
        g_set_time.tmNow.tm_min,
        g_set_time.tmNow.tm_sec);
}
void Set_ParaResetTimeDraw(HDC hdc)
{
#ifdef AREA_SHANXI
        MessageBox("山西终端时钟\n不支持设置!");
        CancelMenu();
        return;
#endif

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 30;
    rc.bottom = rc.top + 16;
    char pcz[20];
    strcpy(pcz, "终端时间设置");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    // 绘制日期
    int nLeft = 24;
    int nTop = rc.top + 30;
    int i;
    for (i = 0; i < 8; i++)
    {
        switch (i)
        {
        case 4:TextOut(hdc, nLeft, nTop, "年"); nLeft += 16; break;
        case 6:TextOut(hdc, nLeft, nTop, "月"); nLeft += 16; break;
        }

        if (g_set_time.nIndex == i)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_set_time.czInfo[i], 1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_set_time.czInfo[i], 1);
        }
        nLeft += 8;
    }
    TextOut(hdc, nLeft, nTop, "日");

    // 绘制时间
    nLeft = 48;
    nTop += 20;
    for (; i < 16; i++)
    {
        if (g_set_time.nIndex == i)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_set_time.czInfo[i], 1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_set_time.czInfo[i], 1);
        }
        nLeft += 8;
    }

    // 确定
    nTop += 20;
    nLeft = 68;
    if (g_set_time.nIndex == i)
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
BOOL Set_ParaResetTimeKeyMsg(int nKey)
{
    int iRet = 0;

    switch (nKey)
    {
    case KEY_LEFT:// 修改输入焦点
        g_set_time.nIndex--;
        if (g_set_time.czInfo[g_set_time.nIndex] < '0'
            || g_set_time.czInfo[g_set_time.nIndex] > '9')
        {
            g_set_time.nIndex--;
        }
        if (g_set_time.nIndex < 0)
        {
            g_set_time.nIndex = 16;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:// 修改输入焦点
        g_set_time.nIndex++;
        if ((g_set_time.czInfo[g_set_time.nIndex] < '0'
            || g_set_time.czInfo[g_set_time.nIndex] > '9')
            && g_set_time.nIndex != 16)
        {
            g_set_time.nIndex++;
        }
        if (g_set_time.nIndex > 16)
        {
            g_set_time.nIndex = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_UP:// 修改时间参数，其中对有效位做了一些判断
        if (g_set_time.nIndex >= 0 && g_set_time.nIndex <= 15)
        {
            char czMax = '9';
            switch (g_set_time.nIndex)
            {
            case 0:czMax = '2'; break;   // 年千位
            case 4:czMax = '1'; break;   // 月十位
            case 6:czMax = '3'; break;   // 日十位
            case 8:czMax = '2'; break;   // 小时十位
            case 11:czMax = '5'; break;  // 分十位
            case 14:czMax = '5'; break;  // 秒十位
            }
            g_set_time.czInfo[g_set_time.nIndex] ++;
            if (g_set_time.czInfo[g_set_time.nIndex] > czMax)
            {
                if (g_set_time.nIndex == 0)
                    g_set_time.czInfo[g_set_time.nIndex] = '1';
                else
                    g_set_time.czInfo[g_set_time.nIndex] = '0';
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
        break;
    case KEY_DOWN:// 修改时间参数，其中对有效位做了一些判断
        if (g_set_time.nIndex >= 0 && g_set_time.nIndex <= 15)
        {
            char czMax = '9';
            switch (g_set_time.nIndex)
            {
            case 0:czMax = '2'; break;   // 年千位
            case 4:czMax = '1'; break;   // 月十位
            case 6:czMax = '3'; break;   // 日十位
            case 8:czMax = '2'; break;   // 小时十位
            case 11:czMax = '5'; break;  // 分十位
            case 14:czMax = '5'; break;  // 秒十位
            }
            g_set_time.czInfo[g_set_time.nIndex] --;
            if (g_set_time.czInfo[g_set_time.nIndex] < '0')
            {
                g_set_time.czInfo[g_set_time.nIndex] = czMax;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
        break;
    case KEY_OK:
        printf("ENTER KEY_OK\n");
        if (g_set_time.nIndex == 16 && ValidatePwd() == TRUE)
        {
            char czBuf[20];
            int nVal;
            // 年
            memcpy(czBuf, g_set_time.czInfo, 4);
            czBuf[4] = '\0';
            nVal = atoi(czBuf);
            if (nVal > 2099)
            {
                g_set_time.nIndex = 3;
                MessageBox("年份值不能超出2099年\n\n请重新输入！", MB_OK);
                return TRUE;
            }
            if (nVal < 2000)
            {
                g_set_time.nIndex = 3;
                MessageBox("年份值不能低于2000年\n\n请重新输入！", MB_OK);
                return TRUE;
            }
            g_set_time.tmNow.tm_year = nVal;

            // 月
            memcpy(czBuf, g_set_time.czInfo + 4, 2);
            czBuf[2] = '\0';
            nVal = atoi(czBuf);
            if (nVal == 0)
            {
                g_set_time.nIndex = 5;
                MessageBox("月份值不能为零月份\n\n请重新输入！", MB_OK);
                return TRUE;
            }
            if (nVal > 12)
            {
                g_set_time.nIndex = 5;
                MessageBox("月份值不能大于12月份\n\n请重新输入！", MB_OK);
                return TRUE;
            }
            g_set_time.tmNow.tm_mon = nVal;

            // 日
            memcpy(czBuf, g_set_time.czInfo + 6, 2);
            czBuf[2] = '\0';
            nVal = atoi(czBuf);
            if (nVal == 0)
            {
                g_set_time.nIndex = 7;
                MessageBox("日期值不能为零\n\n请重新输入！", MB_OK);
                return TRUE;
            }
            if (nVal > 31)
            {
                g_set_time.nIndex = 7;
                MessageBox("日期值超出范围\n\n请重新输入！", MB_OK);
                return TRUE;
            }
            g_set_time.tmNow.tm_mday = nVal;

            // 小时
            memcpy(czBuf, g_set_time.czInfo + 8, 2);
            czBuf[2] = '\0';
            nVal = atoi(czBuf);
            if (nVal > 23)
            {
                g_set_time.nIndex = 9;
                MessageBox("小时值超出范围\n\n请重新输入！", MB_OK);
                return TRUE;
            }
            g_set_time.tmNow.tm_hour = nVal;

            // 分
            memcpy(czBuf, g_set_time.czInfo + 11, 2);
            czBuf[2] = '\0';
            nVal = atoi(czBuf);
            g_set_time.tmNow.tm_min = nVal;

            // 秒
            memcpy(czBuf, g_set_time.czInfo + 14, 2);
            czBuf[2] = '\0';
            nVal = atoi(czBuf);
            g_set_time.tmNow.tm_sec = nVal;


            DateTimeHex_t pDateTime;
            memset(&pDateTime, 0, sizeof(DateTimeHex_t));

            pDateTime.year_h = g_set_time.tmNow.tm_year >> 8;
            pDateTime.year_l = g_set_time.tmNow.tm_year & 0xff;
            pDateTime.month = g_set_time.tmNow.tm_mon;
            pDateTime.day = g_set_time.tmNow.tm_mday;
            pDateTime.hour = g_set_time.tmNow.tm_hour;
            pDateTime.min = g_set_time.tmNow.tm_min;
            pDateTime.sec = g_set_time.tmNow.tm_sec;
            g_set_time.nIndex = 0;

            int year = 0;
            year = (uint16)pDateTime.year_l + (uint16)(pDateTime.year_h << 8);

            GUI_FMT_TRACE("set time modify time: %d-%d-%d %d:%d:%d]\n", year, pDateTime.month, pDateTime.day, pDateTime.hour, pDateTime.min, pDateTime.sec);
            iRet = systime_set_to_smiOS(pDateTime);
            if (iRet < 0)
            {
                MessageBox("终端时间设置失败！", MB_OK);
                return FALSE;
            }
            GUI_FMT_TRACE("终端时间设置成功！\n");
            MessageBox("终端时间设置成功！", MB_OK);
            return TRUE;
        }
        break;
    case KEY_CANCEL:
        CancelMenu();
        return TRUE;
        break;
    }
    return FALSE;
}
PLUGIN_INFO g_Set_ParaResetTime = {
    0x00620001/*id*/,
    Set_ParaResetTimeInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    Set_ParaResetTimeDraw/*Draw*/,
    NULL/*Timer*/,
    Set_ParaResetTimeKeyMsg/*keymsg*/,
    NULL/*ipc*/
};
/************************************************************************/
/* 终端时间设置结束                                                     */
/************************************************************************/

#endif

#if DESC("数据初始化",1)


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
    ClearData.recordOAD.logicId = 0;
    ClearData.bClassTag = FALSE;
    int ret = 0;
    ret = db_clear_data(g_hUdbCenter, &ClearData);
    if (ret == ERR_OK)
    {
        GUI_FMT_TRACE("db_clear_data ok. ret(%d)\n", ret);
    }
    else
    {
        GUI_FMT_TRACE("db_clear_data failed. ret(%d)\n", ret);
    }
//  ret |= data_init_sent_to_acmeter();
//  if (ret != ERR_OK)
    {
//      GUI_FMT_TRACE("param_init_sent_to_acmeter err. ret(%d)\n", ret);
    }

    PopMsg(NULL);
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
            CancelMenu();
            return TRUE;
        case 1:
            if(ValidatePwd() == TRUE)
            {
                if (data_initialization() != 0)
                {
                    MessageBox("数据初始化失败!");
                }
                else
                {
                    MessageBox("数据初始化成功!");
                }
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

#endif

#if DESC("参数初始化",1)


void param_initialization_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    g_guiComm.bCanTrunDisp = FALSE;
    g_bpop = TRUE;
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
            CancelMenu();
            break;
        case 1:
            if(ValidatePwd() == TRUE)
            {
                if (db_recover_default(g_hUdbCenter, 0) == 0)
                {
    #ifdef AREA_FUJIAN
                /*向载波模块发送初始化命令，不等待接受*/
                    uint8 head = 17,tail = 20,len=0;
                    uint8 LocalAddr[6] = { 0 };
                    uint8 apduhead[17] = {0x09,0x32,0x1e,0xf2,0x09,0x02,0x01,0x00,0x06,0x02,0x08,0x01,0x00,0x00,0x32,0x00,0x0a};
                    get_addr(LocalAddr);
                    uint8 apdu[tail+head+2+6]={0};
                    uint8 index = 0;
                    memcpy(apdu,apduhead,head);
                    index +=head;
                    apdu[index++] = 0x07;
                    apdu[index++] = 0x05;
                    memcpy(&apdu[index],LocalAddr,6);
                    index+=6;
                    uint8 apdutail[tail] = {0x01,0x00,0x00,0x0f,0x68,0x0f,0x00,0x43,0x00,0x00,0x00,0x80,0x25,0x33,0x01,0x02,0x00,0x1e,0x16,0x00};
                    
                    memcpy(&apdu[index],apdutail,tail);
                    index+=tail;
                    len = index;
                    gui_mannual_amr_send(apdu, len);
    
    #endif
                    PopMsg("    参数初始化成功！\n    正在重启应用...");
                    //调用设置默认参数的接口 地区相关
    //                app_setdefault_cfg_area(g_hUdbCenter);
    //                RebootTerminal();
                    
    
                }
                else
                {
                    MessageBox("参数初始化失败!");
                    CancelMenu();
                }
            }
            break;
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
#endif

#if DESC("交采参数初始化",1)

void ac_param_initialization_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    g_guiComm.bCanTrunDisp = FALSE;
}
void ac_param_initialization_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 40;
    rc.bottom = rc.top + 16;
    const char* pcz = "交采参数初始化";
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

BOOL ac_param_initialization_keymsg(int nKey)
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
            CancelMenu();
            return TRUE;
        case 1:
            if(ValidatePwd() == TRUE)
            {
                if (param_init_sent_to_acmeter() == 0)
                {
                    MessageBox("交采参数初始化成功!");
                    CancelMenu();
                }
                else
                {
                    MessageBox("交采参数初始化失败!");
                }
            }
            return TRUE;

        }
        return TRUE;
    }
    return FALSE;
}

//交采参数初始化
PLUGIN_INFO g_layer_ac_param_initialization =
{
    0x11040008/*id*/,
    ac_param_initialization_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    ac_param_initialization_draw/*Draw*/,
    NULL/*Timer*/,
    ac_param_initialization_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif


#if DESC("交采数据初始化",1)

void data_param_initialization_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 40;
    rc.bottom = rc.top + 16;
    const char* pcz = "交采数据初始化";
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

BOOL data_param_initialization_keymsg(int nKey)
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
            CancelMenu();
            return TRUE;
        case 1:
            if(ValidatePwd() == TRUE)
            {
                if (data_init_sent_to_acmeter() == 0)
                {
                    MessageBox("交采数据初始化成功!");
                    CancelMenu();
                }
                else
                {
                    MessageBox("交采数据初始化失败!");
                }
            }
            return TRUE;

        }
        return TRUE;
    }
    return FALSE;
}

//交采数据初始化
PLUGIN_INFO g_layer_data_param_initialization =
{
    0x11040010/*id*/,
    param_initialization_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    data_param_initialization_draw/*Draw*/,
    NULL/*Timer*/,
    data_param_initialization_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("任务状态查看",1)
uint16 tsakID;
OOP_ACQ_MONITOR_T g_acq;

#ifdef AREA_FUJIAN
OOP_TRANS_TASK_RLUESTATE_T g_acq_fj;
int drawIndex = 0;
OOP_GATHER_E        gather;

#endif

int get_task_status(uint16 id)
{
//    OOP_TASK_T oopTask;
    memset(&g_acq, 0, sizeof(OOP_ACQ_MONITOR_T));
#ifdef AREA_FUJIAN
    memset(&g_acq_fj, 0, sizeof(OOP_TRANS_TASK_RLUESTATE_T));
    if(gather == OOP_GATHER_FJ)     //福建增补
    {
        return read_db_record_6e20(g_hUdbCenter, id, (uint8*)&g_acq_fj, sizeof(OOP_TRANS_TASK_RLUESTATE_T));
    }
#endif
    
//    if (app_get_task_list(id, &oopTask) != 0)
//        return 0;
    return read_db_noamal_data(g_hUdbCenter, 0x60340200, 0, id, (uint8*)&g_acq, sizeof(OOP_ACQ_MONITOR_T));
}

void taskStatusIdReturn(char* pNewStr, void* pPara)
{
    int value = atoi(pNewStr);
    char tmp[20] = { 0 };
#ifdef AREA_FUJIAN
    if (value > 65535 && TRUE == MessageBox("任务ID大于65535\n\n确认是否重新输入?", MB_OKCANCEL))
    {           
        sprintf(tmp, "%d", tsakID);
        InputBox(tmp, taskStatusIdReturn, NULL, 0x01);
    }
#else
    if (value > 255 && TRUE == MessageBox("任务ID大于255\n\n确认是否重新输入?", MB_OKCANCEL))
    {           
        sprintf(tmp, "%d", tsakID);
        InputBox(tmp, taskStatusIdReturn, NULL, 0x01);
    }
#endif
    else
    {
        if (value != 0){
            tsakID = value;
//          memset(&g_acq, 0, sizeof(OOP_ACQ_MONITOR_T));
            get_task_status(tsakID);
        }
    }
}

void task_status_init(DWORD dwPluginId)
{
#ifdef AREA_FUJIAN
    drawIndex = 0;
    int ret = 0;
    ret = read_db_noamal_data(g_hUdbCenter, 0x6e000200, 0, 0, (uint8*)&gather, sizeof(OOP_GATHER_E));
    if(0 != ret || gather > OOP_GATHER_FJ)
    {
        gather = OOP_GATHER_FJ;     //默认福建模式
    }
#endif
    tsakID = 1;
//    memset(&g_acq, 0, sizeof(OOP_ACQ_MONITOR_T));
    get_task_status(tsakID);
    printf("taskid %d state %d \n", tsakID, g_acq.taskState);
    g_guiComm.bCanTrunDisp = FALSE;


}
#ifdef AREA_FUJIAN
void task_status_draw_fj(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;
    
    int nTop = 36;
    int nLeft1 = 5;
    int nLeft2 = 70;
    char czBuf[64] = { 0 };
//    TextOut(hdc, nLeft1, nTop, "任务ID:");
//    sprintf(czBuf, "%hu", tsakID);
//    ITEM_SELECT;
//    TextOut(hdc, nLeft2, nTop, czBuf);
//    ITEM_NOT_SELECT;

    if(drawIndex == 0)
    {
        TextOut(hdc, nLeft1, nTop, "任务ID:");
        sprintf(czBuf, "%hu", tsakID);
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
        
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "存储时间:");
        sprintf(czBuf, "%02d-%02d %02d:%02d:%02d", g_acq_fj.storeTime.month, g_acq_fj.storeTime.day, g_acq_fj.storeTime.hour, g_acq_fj.storeTime.minute, g_acq_fj.storeTime.second);
        TextOut(hdc, nLeft2 - 5, nTop, czBuf); 

        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "存储次数:");
        sprintf(czBuf, "%hu", g_acq_fj.storenum);
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "开始时间:");
        sprintf(czBuf, "%02d-%02d %02d:%02d:%02d", g_acq_fj.firststartTime.month, g_acq_fj.firststartTime.day, g_acq_fj.firststartTime.hour, g_acq_fj.firststartTime.minute, g_acq_fj.firststartTime.second);
        TextOut(hdc, nLeft2 - 5, nTop, czBuf);

        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "最后时间:");
        sprintf(czBuf, "%02d-%02d %02d:%02d:%02d", g_acq_fj.laststartTime.month, g_acq_fj.laststartTime.day, g_acq_fj.laststartTime.hour, g_acq_fj.laststartTime.minute, g_acq_fj.laststartTime.second);
        TextOut(hdc, nLeft2 - 5, nTop, czBuf);
    }
    else if(drawIndex == 1)
    {
        TextOut(hdc, nLeft1, nTop, "配置总数:");
        sprintf(czBuf, "%hu", g_acq_fj.paramSum);
        TextOut(hdc, nLeft2, nTop, czBuf);
        
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "采集成功数:");
        sprintf(czBuf, "%hu", g_acq_fj.success);
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "加工成功数:");
        sprintf(czBuf, "%hu", g_acq_fj.machingSum);
        TextOut(hdc, nLeft2, nTop, czBuf);
    
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "发送报文数:");
        sprintf(czBuf, "%hu", g_acq_fj.sendNum);
        TextOut(hdc, nLeft2, nTop, czBuf);
    
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "接收报文数:");
        sprintf(czBuf, "%hu", g_acq_fj.receNum);
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
        

}
#endif

void task_status_draw(HDC hdc)
{
#ifdef AREA_FUJIAN
    if(gather == OOP_GATHER_FJ)     //福建增补
    {
        return task_status_draw_fj(hdc);
    }
#endif

    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;
    
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 80;
    char czBuf[64] = { 0 };
    TextOut(hdc, nLeft1, nTop, "任务ID:");
    sprintf(czBuf, "%hu", tsakID);
    ITEM_SELECT;
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "任务状态:");
    TextOut(hdc, nLeft2, nTop, g_showtaskStatus[g_acq.taskState]);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "采集总数:");
    sprintf(czBuf, "%hu", g_acq.sum);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "采集成功数:");
    sprintf(czBuf, "%hu", g_acq.success);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "发送报文数:");
    sprintf(czBuf, "%hu", g_acq.send);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "接收报文数:");
    sprintf(czBuf, "%hu", g_acq.receive);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "开始时间:");
    sprintf(czBuf, "%02d-%02d %02d:%02d:%02d", g_acq.startTime.month, g_acq.startTime.day,
        g_acq.startTime.hour, g_acq.startTime.minute, g_acq.startTime.second);
    TextOut(hdc, nLeft2 - 10, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "结束时间:");
    sprintf(czBuf, "%02d-%02d %02d:%02d:%02d", g_acq.endTime.month, g_acq.endTime.day,
        g_acq.endTime.hour, g_acq.endTime.minute, g_acq.endTime.second);
    TextOut(hdc, nLeft2 - 10, nTop, czBuf);

}

BOOL task_status_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_DOWN:
    {
#ifdef AREA_FUJIAN
        drawIndex += 1;
        if (drawIndex > 1)
        {
            drawIndex = 0;
        }

#endif
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    {
#ifdef AREA_FUJIAN
        drawIndex -= 1;
        if (drawIndex < 0)
        {
            drawIndex = 1;
        }
#endif
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_LEFT:

#ifdef AREA_FUJIAN
        if (tsakID == 1 )
        {
            tsakID = 65535;
        }
#else
        if (tsakID == 1)
        {
            tsakID = 255;
        }
#endif
        else
        {
            tsakID--;
        }
//        memset(&g_acq, 0, sizeof(OOP_ACQ_MONITOR_T));
        get_task_status(tsakID);
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
#ifdef AREA_FUJIAN
        if (tsakID == 65535)
        {
            tsakID = 1;
        }
#else
        if (tsakID == 255)
        {
            tsakID = 1;
        }
#endif
        else
        {
            tsakID++;
        }
//        memset(&g_acq, 0, sizeof(OOP_ACQ_MONITOR_T));
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

//任务状态查看
PLUGIN_INFO g_layer_task_status =
{
    0x11040005/*id*/,
    task_status_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    task_status_draw/*Draw*/,
    NULL/*Timer*/,
    task_status_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("任务失败表计集合",1)
//uint16 tsakID;
int g_failIndex = 0;
int g_maxIndex = 0;
OOP_TASK_FAIL_TSA_LIST_T g_tsaFail;

int get_task_fail(uint16 id)
{
    memset(&g_tsaFail, 0, sizeof(OOP_TASK_FAIL_TSA_LIST_T));
    return read_db_noamal_data(g_hUdbCenter, 0xff300a00, 0, id, (uint8*)&g_tsaFail, sizeof(OOP_TASK_FAIL_TSA_LIST_T));
}

void task_fail_id_return(char* pNewStr, void* pPara)
{
    int value = atoi(pNewStr);
    char tmp[20] = { 0 };
#ifdef AREA_FUJIAN
    if (value > 65535 && TRUE == MessageBox("任务ID大于65535\n\n确认是否重新输入?", MB_OKCANCEL))
    {           
        sprintf(tmp, "%d", tsakID);
        InputBox(tmp, task_fail_id_return, NULL, 0x01);
    }
#else
    if (value > 255 && TRUE == MessageBox("任务ID大于255\n\n确认是否重新输入?", MB_OKCANCEL))
    {           
        sprintf(tmp, "%d", tsakID);
        InputBox(tmp, task_fail_id_return, NULL, 0x01);
    }
#endif
    else
    {
        if (value != 0){
            tsakID = value;
            get_task_fail(tsakID);
        }
    }
}

void task_fail_init(DWORD dwPluginId)
{
    tsakID = 1;
    get_task_fail(tsakID);
    g_guiComm.bCanTrunDisp = FALSE;
    g_failIndex = 0;
    
    g_maxIndex = g_tsaFail.failNum/6;

    if(g_tsaFail.failNum % 6 != 0)
    {
        g_maxIndex += 1;
    }
    g_bpop = TRUE;
}

void task_fail_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;
    
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 80;
    char czBuf[64] = { 0 };
    TextOut(hdc, nLeft1, nTop, "任务ID:");
    sprintf(czBuf, "%hu", tsakID);
    ITEM_SELECT;
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "失败总数量:");
    sprintf(czBuf, "%d", g_tsaFail.failNum);
    TextOut(hdc, nLeft2, nTop, czBuf);

    int i = 0;
    for(i=g_failIndex*6; i<(g_tsaFail.failNum<(g_failIndex+1)*6?g_tsaFail.failNum:(g_failIndex+1)*6); i++)
    {
        nTop += 15;
        sprintf(czBuf, "表地址%d:", i+1);
        TextOut(hdc, nLeft1, nTop, czBuf);
        sprintf(czBuf, "%02X%02X%02X%02X%02X%02X", g_tsaFail.failAddr[i][0], g_tsaFail.failAddr[i][1], g_tsaFail.failAddr[i][2] ,
            g_tsaFail.failAddr[i][3], g_tsaFail.failAddr[i][4], g_tsaFail.failAddr[i][5]);
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}

BOOL task_fail_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_DOWN:
    {
        g_failIndex += 1;
        if (g_failIndex >= g_maxIndex)
        {
            g_failIndex = 0;
        }

        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    {
        g_failIndex -= 1;
        if (g_failIndex < 0)
        {
            g_failIndex = g_maxIndex-1>0?g_maxIndex-1:0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_LEFT:

#ifdef AREA_FUJIAN
        if (tsakID == 1)
        {
            tsakID = 65535;
        }
#else
        if (tsakID == 1)
        {
            tsakID = 255;
        }
#endif
        else
        {
            tsakID--;
        }
        get_task_fail(tsakID);
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
#ifdef AREA_FUJIAN
        if (tsakID == 65535)
        {
            tsakID = 1;
        }
#else
        if (tsakID == 255)
        {
            tsakID = 1;
        }
#endif
        else
        {
            tsakID++;
        }
        get_task_fail(tsakID);
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
        
    case KEY_OK:
        char bzbuf[50];
        sprintf(bzbuf, "%d", tsakID);
        InputBox(bzbuf, task_fail_id_return, NULL, 0x01);
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }

    return FALSE;
}

//任务失败表计集合
PLUGIN_INFO g_layer_task_fail =
{
    0x110400ff/*id*/,
    task_fail_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    task_fail_draw/*Draw*/,
    NULL/*Timer*/,
    task_fail_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("手动抄表测试",1)
//CRC计算
const uint16 fcstab[256] = {
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};
#define PPPINITFCS16 0xffff /* Initial FCS value */
#define PPPGOODFCS16 0xf0b8 /* Good final FCS value */

uint16 app_crc16_getex(uint16 CRC16_data,const uint8* buf, uint32 len)
{
    const unsigned char *cp=buf;
    while (len--)
        CRC16_data = (CRC16_data >> 8) ^ fcstab[(CRC16_data ^ *cp++) & 0xff];
    return (CRC16_data);
}
uint16 app_crc16_get(const uint8* buf, uint32 len)
{
    uint16 trialfcs;
    /* add on output */
    trialfcs = app_crc16_getex( PPPINITFCS16, buf, len );
    trialfcs ^= 0xffff; /* complement */
    return trialfcs;
}

struct manual_amr_result_struc
{
    uint8   protocol;   //规约类型
    uint8   piid;       //piid
    char    state[16];  //返回结果状态
    char    time[16];   //返回结果时间
    char    data[16];   //返回结果数据(正向有功)
};
int8    g_manual_amr_index;
uint16  g_manual_amr_timeout;
uint8   g_manual_amr_piid = 1;
struct manual_amr_result_struc g_manual_amr_rst;

void manualAmrTimeOutReturn(char* pNewStr, void* pPara)
{
    int value = atoi(pNewStr);
    char tmp[20] = { 0 };
    if (value > 65535 && TRUE == MessageBox("超时时间大于65535\n\n确认是否重新输入?", MB_OKCANCEL))
    {           
        sprintf(tmp, "%d", g_manual_amr_timeout);
        InputBox(tmp, manualAmrTimeOutReturn, NULL, 0x01);
    }
    else
    {
        g_manual_amr_timeout = value; 
    }
}

void manualAmrMakeFrame(uint8 *apdu, uint8 *len, OOP_METER_T oopMeter)
{
    uint8   index = 0;
    uint16  crc;
    
    apdu[index++] = 0x09;   //代理
    apdu[index++] = 0x07;   //透明转发
    apdu[index++] = g_manual_amr_piid++;   //PIID
    //端口OAD
    apdu[index++] = (oopMeter.basic.port.value >> 24) & 0xFF;
    apdu[index++] = (oopMeter.basic.port.value >> 16) & 0xFF;
    apdu[index++] = (oopMeter.basic.port.value >> 8) & 0xFF;
    apdu[index++] = oopMeter.basic.port.value & 0xFF;

    apdu[index++] = oopMeter.basic.baud;    //波特率
    apdu[index++] = 0x02;   //偶校验
    apdu[index++] = 0x08;   //数据位
    apdu[index++] = 0x01;   //停止位
    apdu[index++] = 0x00;   //流控
    //报文超时时间
    apdu[index++] = g_manual_amr_timeout >> 8;
    apdu[index++] = g_manual_amr_timeout & 0xFF;
    //字节超时时间(10ms)
    apdu[index++] = 0x00;
    apdu[index++] = 0x0A;
    if (oopMeter.basic.protocol == 3)   //698协议
    {
        apdu[index++] =  0x19; //透传报文长度 
        apdu[index++] = 0x68;   //起始符
        //长度
        apdu[index++] = 0x17;
        apdu[index++] = 0x00;
        
        apdu[index++] = 0x43;   //控制域
        apdu[index++] = 0x05;   //SA标志
        //SA地址
        apdu[index++] = oopMeter.basic.tsa.add[5];
        apdu[index++] = oopMeter.basic.tsa.add[4];
        apdu[index++] = oopMeter.basic.tsa.add[3];
        apdu[index++] = oopMeter.basic.tsa.add[2];
        apdu[index++] = oopMeter.basic.tsa.add[1];
        apdu[index++] = oopMeter.basic.tsa.add[0];

        apdu[index++] = 0x00;   //CA地址

        crc = app_crc16_get(&apdu[index - 11], 11);
        apdu[index++] = crc & 0xFF;
        apdu[index++] = crc >> 8;

        apdu[index++] = 0x05;   //读取请求
        apdu[index++] = 0x01;   //读取一个对象属性
        apdu[index++] = 0x05;   //PIID
        //OAD
        apdu[index++] = 0x00;
        apdu[index++] = 0x10;
        apdu[index++] = 0x02;
        apdu[index++] = 0x00;

        apdu[index++] = 0x00;   //timeTag

        crc = app_crc16_get(&apdu[index - 21], 21);
        apdu[index++] = crc & 0xFF;
        apdu[index++] = crc >> 8;

        apdu[index++] = 0x16;   //结束字符

    }   
    else
    {
        uint8   checkSum = 0; //校验和
        int     i;
        
        apdu[index++] = 0x13; //透传报文长度
        apdu[index++] = 0xFE;
        apdu[index++] = 0xFE;
        apdu[index++] = 0xFE;
        apdu[index++] = 0x68; //起始符
        //SA地址
        apdu[index++] = oopMeter.basic.tsa.add[5];
        apdu[index++] = oopMeter.basic.tsa.add[4];
        apdu[index++] = oopMeter.basic.tsa.add[3];
        apdu[index++] = oopMeter.basic.tsa.add[2];
        apdu[index++] = oopMeter.basic.tsa.add[1];
        apdu[index++] = oopMeter.basic.tsa.add[0];
        apdu[index++] = 0x68; //起始符
        apdu[index++] = 0x11; //控制域
        apdu[index++] = 0x04; //长度
        apdu[index++] = 0x00 + 0x33;
        apdu[index++] = 0x00 + 0x33;
        apdu[index++] = 0x01 + 0x33;
        apdu[index++] = 0x00 + 0x33;

        for (i = 0; i < index; i++)
        {
            checkSum += apdu[index - 14 + i];
        }
        apdu[index++] = checkSum;
        apdu[index++] = 0x16;   //结束字符
    }
    apdu[index++] = 0x00;   //timeTag

    *len = index;
}

void manual_amr_init(DWORD dwPluginId)
{
    g_manual_amr_index = 0;
    g_guiComm.nCurPoint = 1;
    g_manual_amr_timeout = 50;
    memset(&g_manual_amr_rst, 0, sizeof(g_manual_amr_rst));
    memset(&MeterFile,0,sizeof(OOP_METER_T));
}

void manual_amr_draw(HDC hdc)
{
    //int flag = 0;  //标识档案是否存在
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 85;
    int nLeft3 = 50;
    char czBuf[300] = { 0 };

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    //背光灯常量，等待回收
    //g_guiMain.nLightonTime = 0;
    //g_guiMain.bLighton = TRUE;

    set_gui_infonum((uint16)g_guiComm.nCurPoint);

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);

    TextOut(hdc, nLeft1, nTop, "测量点号");
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);
    if (g_manual_amr_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "抄表地址");
    if (MeterFile.nIndex != g_guiComm.nCurPoint)
    {
        app_get_meter_list(g_guiComm.nCurPoint, &MeterFile);
    }
    
    if (MeterFile.basic.tsa.len != 0 && MeterFile.nIndex==g_guiComm.nCurPoint)
    {
        sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", MeterFile.basic.tsa.add[0], MeterFile.basic.tsa.add[1],
                MeterFile.basic.tsa.add[2], MeterFile.basic.tsa.add[3], MeterFile.basic.tsa.add[4], MeterFile.basic.tsa.add[5]);
    }
    else
    {
        sprintf(czBuf, "--");
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "超时时间(s)");
    sprintf(czBuf, "%d", g_manual_amr_timeout);
    if (g_manual_amr_index == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
  
    
    nTop += 20;
    if (g_manual_amr_index == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, "确定测试");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, "确定测试");
    }

    nTop += 12;
    TextOut(hdc, 0, nTop, "---------------------------------------------");
    
    nTop += 12;
    TextOut(hdc, nLeft1, nTop, "状态");  

    if (g_manual_amr_index == 3)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2 - 10, nTop, g_manual_amr_rst.state);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2 - 10, nTop, g_manual_amr_rst.state);
    }
    
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "返回时间");
    TextOut(hdc, nLeft2 - 10, nTop, g_manual_amr_rst.time);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "总正向有功");
    TextOut(hdc, nLeft2 - 10, nTop, g_manual_amr_rst.data);
}

BOOL manual_amr_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    { 
        case KEY_UP:
        {
            g_manual_amr_index--;
            if (g_manual_amr_index < 0)
                g_manual_amr_index = 2;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);            
            return TRUE;
        }
        case KEY_DOWN:
        {
            g_manual_amr_index++;
            if (g_manual_amr_index > 2)
                g_manual_amr_index = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_LEFT:
        {
            if (g_manual_amr_index == 0)
            {
                g_guiComm.nCurPoint--;
                if (g_guiComm.nCurPoint < 0)
                {
                    g_guiComm.nCurPoint = 3500/*MAX_SEL_POINT*/;
                }
                memset(&g_manual_amr_rst,0,sizeof(manual_amr_result_struc));
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                return TRUE;
            }
        }
        break;
        case KEY_RIGHT:
        {
            if (g_manual_amr_index == 0)
            {
                g_guiComm.nCurPoint++;
                if (g_guiComm.nCurPoint > 3500/*MAX_SEL_POINT*/)
                {
                    g_guiComm.nCurPoint = 0;
                }
                memset(&g_manual_amr_rst,0,sizeof(manual_amr_result_struc));
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                return TRUE;
            }
        }
        break;
        case KEY_OK:
        {
            if (g_manual_amr_index == 0)
            {
                char bzbuf[50];
                sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
                InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            }

            if (g_manual_amr_index == 1)
            {
                char bzbuf[50];
                sprintf(bzbuf, "%d", g_manual_amr_timeout);
                InputBox(bzbuf, manualAmrTimeOutReturn, NULL, 0x01);
            }
            
            if (g_manual_amr_index == 2)
            {
////                OOP_METER_T        oopMeter;
//                uint8   apdu[100] = {0};
//                uint8   len = 0;
//                
////                memset(&oopMeter, 0, sizeof(OOP_METER_T));
//                if (MeterFile.nIndex != g_guiComm.nCurPoint)
//                {
//                    app_get_meter_list(g_guiComm.nCurPoint, &MeterFile);
//                }
//
//
//                if (MeterFile.basic.tsa.len == 0||MeterFile.nIndex!=g_guiComm.nCurPoint)
//                {
//                    GUI_FMT_DEBUG(" app_get_meter_list %d !\n", g_guiComm.nCurPoint);
//                    MessageBox("请输入正确的测量点号!", MB_OK);
//                    return FALSE;
//                }
//
//                g_manual_amr_index++;
//                memset(&g_manual_amr_rst, 0, sizeof(g_manual_amr_rst));
//                sprintf(g_manual_amr_rst.state, "测试中...");
//                g_manual_amr_rst.piid = g_manual_amr_piid;
//                g_manual_amr_rst.protocol = MeterFile.basic.protocol;
//                manualAmrMakeFrame(apdu, &len, MeterFile);
//                gui_mannual_amr_send(apdu, len);
                OOP_METER_T        oopMeter;
                uint8   apdu[100] = {0};
                uint8   len = 0;
                
                memset(&oopMeter, 0, sizeof(OOP_METER_T));
                if (app_get_meter_list(g_guiComm.nCurPoint, &oopMeter) != ERR_OK)
                {
                    GUI_FMT_DEBUG(" app_get_meter_list %d !\n", g_guiComm.nCurPoint);
                    MessageBox("请输入正确的测量点号!", MB_OK);
                    return FALSE;
                }

                g_manual_amr_index++;
                memset(&g_manual_amr_rst, 0, sizeof(g_manual_amr_rst));
                sprintf(g_manual_amr_rst.state, "测试中...");
                g_manual_amr_rst.piid = g_manual_amr_piid;
                g_manual_amr_rst.protocol = oopMeter.basic.protocol;
                manualAmrMakeFrame(apdu, &len, oopMeter);
                gui_mannual_amr_send(apdu, len);
            }

            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    }
    return FALSE;
}


//手动抄表测试
PLUGIN_INFO g_layer_manual_amr =
{
    0x11040006/*id*/,
    manual_amr_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    manual_amr_draw/*Draw*/,
    NULL/*Timer*/,
    manual_amr_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("载波组网信息查看",1)
//struct line_net_result_struc
//{
//    uint8   protype;        //查询状态（是否超时）0,初始，1测试中2.测试完成3.测试超时
//    uint8   routestate;    //返回路由学习状态 0.未完成 1.已完成 2.初始状态
//    uint16  linefilenum;    //当前档案个数
//    uint16  netnum;         //返回组网成功个数
//};
//struct line_net_result_struc g_line_net_rst;
//
//int8    g_line_net_index;
//uint16  g_line_net_timeout;
//uint16  len;
//uint8   g_line_net_piid = 1;
//
//
//void line_net_apdu(uint8 *apdu,uint16 *len,uint8 protype)
//{
//    uint8   index=0;
//    memset(apdu,0,100);
//    apdu[index++]=0x09;
//    apdu[index++]=0x07;
//    apdu[index++]=g_line_net_piid++;
//    apdu[index++]=0xf2;
//    apdu[index++]=0x09;
//    apdu[index++]=0x02;    
//    apdu[index++]=0x01;
//    //波特率
//    apdu[index++]=0x03;
//    apdu[index++]=0x02;
//    apdu[index++]=0x08;
//    apdu[index++]=0x01;
//    apdu[index++]=0x00;
//    //
//    apdu[index++] = g_line_net_timeout >> 8;
//    apdu[index++] = g_line_net_timeout & 0xFF;
//    //字节超时时间(10ms)
//    apdu[index++] = 0x00;
//    apdu[index++] = 0x0A;
//    if(protype==0)
//    {
//    //透传报文组帧1736.2 组网个数
//       apdu[index++] = 0x12;
//       apdu[index++] = 0x68;
//       apdu[index++] = 0x12;
//       apdu[index++] = 0x00;
//       apdu[index++] = 0x43;
//       apdu[index++] = 0x00;
//       apdu[index++] = 0x00;
//       apdu[index++] = 0x00;
//       apdu[index++] = 0x80;
//       apdu[index++] = 0x25;
//       apdu[index++] = 0x33;
//       apdu[index++] = 0x10;
//       apdu[index++] = 0x10;
//       apdu[index++] = 0x02;
//       apdu[index++] = 0x01;
//       apdu[index++] = 0x00;
//       apdu[index++] = 0x0a;
//       apdu[index++] = 0x48;
//       apdu[index++] = 0x16;
//
//    }
//    else
//    {
//       apdu[index++] = 0x0f;
//       apdu[index++] = 0x68;
//       apdu[index++] = 0x0f;
//       apdu[index++] = 0x00;
//       apdu[index++] = 0x43;
//       apdu[index++] = 0x00;
//       apdu[index++] = 0x00;
//       apdu[index++] = 0x00;
//       apdu[index++] = 0x00;
//       apdu[index++] = 0x00;
//       apdu[index++] = 0x0e;
//       apdu[index++] = 0x10;
//       apdu[index++] = 0x08;
//       apdu[index++] = 0x00;
//       apdu[index++] = 0x69;
//       apdu[index++] = 0x16;
//
//    }
//    apdu[index++] = 0x00;   //timeTag
//    *len=index;
//}
//
//void line_net_init(DWORD dwPluginId)
//{
//
//    g_line_net_index = 0;
//    g_line_net_timeout = 50;
//    memset(&g_line_net_rst, 0, sizeof(line_net_result_struc));
//    uint8   apdu[100] = {0};
//    g_line_net_rst.linefilenum=oopMeterList.oopMeterNum;
//    g_line_net_rst.routestate=2;
////    line_net_apdu(apdu,&len,0);
////    gui_mannual_amr_send(apdu, len);
//    g_line_net_rst.protype=1;
//
////    line_net_apdu(apdu,&len,1);  //查询路由学习
////    gui_mannual_amr_send(apdu, len);
//
//}
//
//void line_net_draw(HDC hdc)
//{
//    
//    GUI_COMM* pg_guicomm = &g_guiComm;
//    if (pg_guicomm == NULL)
//    return;
//
//    ITEM_NOT_SELECT;
//    SelectFont(hdc, pg_guicomm->fontBig);
//    RECT rc;
//    rc.left = 0;
//    rc.right = pg_guicomm->nWidth;
//
//    char czBuf[300];
//    sprintf(czBuf,"%s","载波组网信息");
//    rc.top = 20;
//    rc.bottom = rc.top + 16;
//    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
//    SelectFont(hdc, pg_guicomm->fontSmall);
//
//    int nTop = rc.top + 30;
//    int nLeft1 = 10;
//    int nLeft2 = 100;
//    TextOut(hdc, nLeft1, nTop, "当前档案个数");
//    if (g_line_net_rst.linefilenum == 0)
//    {
//        sprintf(czBuf, "0");
//    }
//    else
//    {
//        sprintf(czBuf, "%d",g_line_net_rst.linefilenum);
//    }
//
//    TextOut(hdc, nLeft2, nTop, czBuf);
//
//    nTop += 20;
//    TextOut(hdc, nLeft1, nTop, "组网成功个数");
//    memset(czBuf, 0x00, sizeof(czBuf));
//    if (g_line_net_rst.netnum == 0)
//    {
//        sprintf(czBuf, "0");
//    }
//    else    
//    {
//        sprintf(czBuf, "%d", g_line_net_rst.netnum);
//    }
//    TextOut(hdc, nLeft2, nTop, czBuf);
//
//    nTop += 20;
//    TextOut(hdc, nLeft1, nTop, "路由学习状态");
//    if(g_line_net_rst.routestate==0)
//    {
//        sprintf(czBuf, "未完成"); 
//    }
//    else if(g_line_net_rst.routestate==1)
//    {
//        sprintf(czBuf,"已完成");
//    }
//    else if(g_line_net_rst.routestate==2)
//    {
//        sprintf(czBuf,"初始");
//    }
//    TextOut(hdc, nLeft2, nTop, czBuf);
//    
//    nTop += 20;
//    TextOut(hdc, 0, nTop, "---------------------------------------------");
//
//    nTop +=12;
//    TextOut(hdc, nLeft1, nTop, "状态:");
//
//    if(g_line_net_rst.protype==0)
//    {
//        memset(czBuf,0x00,sizeof(czBuf));
//        sprintf(czBuf, "初始");
//    }
//    if(g_line_net_rst.protype==1)
//    {
//        memset(czBuf,0x00,sizeof(czBuf));
//        sprintf(czBuf, "测试中");   
//    }
//    if(g_line_net_rst.protype==2)
//    {
//        memset(czBuf,0x00,sizeof(czBuf));
//        sprintf(czBuf, "测试完成");   
//    }
//    if(g_line_net_rst.protype==3)
//    {
//        memset(czBuf,0x00,sizeof(czBuf));
//        sprintf(czBuf, "测试超时"); 
//    }
//    if(g_line_net_index==0)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft1+30, nTop, czBuf);
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft1+30, nTop, czBuf); 
//    }
//    memset(czBuf,0x00,sizeof(czBuf));
//    sprintf(czBuf, "重试");
//    if(g_line_net_index==1)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft1+90, nTop, czBuf);
//        ITEM_NOT_SELECT;
//
//    }
//    else
//    {
//        TextOut(hdc, nLeft1+90, nTop, czBuf);
//    }
//}
//
//BOOL line_net_keymsg(int nKey)
//{
//    GUI_COMM* pg_guicomm = &g_guiComm;
//    if (pg_guicomm == NULL)
//        return FALSE;
//    switch(nKey)
//    {
//        case KEY_UP:
//          {
//            if(g_line_net_index==0)
//                g_line_net_index=1;
//            else
//                g_line_net_index=0;
//            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
//            return TRUE;
//          }  
//        case KEY_DOWN:
//          {
//            if(g_line_net_index==0)
//                g_line_net_index=1;
//            else
//                g_line_net_index=0;
//            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
//            return TRUE;
//          }
//        case KEY_OK:
//            {
////            uint8 apdu[100]={0}; 
//    
//            memset(&g_line_net_rst,0,sizeof(line_net_result_struc)); //重试需要初始化
//            g_line_net_rst.linefilenum=oopMeterList.oopMeterNum;           
//            g_line_net_rst.routestate=2;            //设定路由状态为初始
////            line_net_apdu(apdu,&len,0);             //组查组网个数帧
//////            gui_mannual_amr_send(apdu, len);        //发送
//            g_line_net_rst.protype=1;               //舍定状态为查询中
////            line_net_apdu(apdu,&len,1);             //查询路由学习
////            gui_mannual_amr_send(apdu, len);
//            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
//            sleep(10);
//            if(g_line_net_rst.protype!=2)
//               g_line_net_rst.protype=3;
//            return TRUE;
//            }
//    }
//    return FALSE;
//}
//
//
////载波组网信息查看
//PLUGIN_INFO g_layer_line_net =
//{
//    0x1104000a/*id*/,
//    line_net_init/*InitLayer*/,
//    NULL/*UninitLayer*/,
//    line_net_draw/*Draw*/,
//    NULL/*Timer*/,
//    line_net_keymsg/*keymsg*/,
//    NULL/*ipc*/
//};

#endif

#if DESC("软件版本号",1)

extern APP_VERSION_INFO_T g_AppVersion;
int g_VersionIndex;
void app_version_Init(DWORD dwPluginId)
{
    g_VersionIndex = 0;
    gui_seek_app();
}

void app_version_Draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;

    char czBuf[64] = {0};
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 25;
    rc.bottom = rc.top + 16;
    
    int nLeft = 5;
    int nLeft2 = 15;
    int nTop = 20;
    int tmpTop = 20;

    if(g_AppVersion.loginAppCnt == 0)
    {
        SelectFont(hdc, pg_guicomm->fontBig);
        
        sprintf(czBuf, "%s", ADV_APP_NAME);
        DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

//      SelectFont(hdc, pg_guicomm->fontSmall);
        // 软件版本号
        nTop = 35+tmpTop;
        TextOut(hdc, nLeft, nTop, "软件版本号");

        nTop += tmpTop;
        sprintf(czBuf, "%s", APP_VERSION);
        TextOut(hdc, nLeft2, nTop, czBuf);


        // 发布时间
        nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "发布时间");
        nTop += tmpTop;
        snprintf(czBuf, 64, "%s", APP_PUBLISH_TIME);
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        SelectFont(hdc, pg_guicomm->fontBig);
        
        sprintf(czBuf, "%s", g_AppVersion.pluginInfo[g_VersionIndex].comName);
        DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

//      SelectFont(hdc, pg_guicomm->fontSmall);
        // 软件版本号
        nTop = 35+tmpTop;
        TextOut(hdc, nLeft, nTop, "软件版本号");

        nTop += tmpTop;
        sprintf(czBuf, "%s", g_AppVersion.pluginInfo[g_VersionIndex].version);
        TextOut(hdc, nLeft2, nTop, czBuf);


        // 发布时间
        nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "发布时间");
        nTop += tmpTop;
        snprintf(czBuf, 64, "%04u-%02u-%02u %02u:%02u:%02u", g_AppVersion.pluginInfo[g_VersionIndex].dataTime.year_h*256+g_AppVersion.pluginInfo[g_VersionIndex].dataTime.year_l
            , g_AppVersion.pluginInfo[g_VersionIndex].dataTime.month, g_AppVersion.pluginInfo[g_VersionIndex].dataTime.day
            , g_AppVersion.pluginInfo[g_VersionIndex].dataTime.hour, g_AppVersion.pluginInfo[g_VersionIndex].dataTime.min
            , g_AppVersion.pluginInfo[g_VersionIndex].dataTime.sec);
        TextOut(hdc, 7, nTop, czBuf);
    }   
     
}

BOOL app_version_KeyMsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
    case KEY_LEFT:
    case KEY_UP:
        g_VersionIndex--;
        if (g_VersionIndex < 0)
        {
            g_VersionIndex = g_AppVersion.loginAppCnt-1;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
    case KEY_DOWN:
        g_VersionIndex++;
        if (g_VersionIndex >= g_AppVersion.loginAppCnt)
        {
            g_VersionIndex = 0;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    return FALSE;
}

//版本信息
PLUGIN_INFO g_layer_app_version_display =
{
    0x11040007/*id*/,
    app_version_Init/*InitLayer*/,
    NULL/*UninitLayer*/,
    app_version_Draw/*Draw*/,
    NULL/*Timer*/,
    app_version_KeyMsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("终端版本信息",1)

typedef struct{
    uint8   factory[4];  /**< 厂商代码           */
    uint8   softVer[4];  /**< 软件版本号         */
    uint8   softDate[6]; /**< 软件版本日期       */
    uint8   hardVer[4];  /**< 硬件版本号         */
    uint8   hardDate[6]; /**< 硬件版本日期       */
    uint8   extend[8];   /**< 厂家扩展信息       */
}__attribute__ ((packed)) Factory_Version_t;

Factory_Version_t g_tGuiVer;

void tmn_version_Init(DWORD dwPluginId)
{
    int ret = 0;
	memset(&g_tGuiVer, 0, sizeof(Factory_Version_t));
    ret = read_db_noamal_data(g_hUdbCenter, 0x43000300, 0, 0, (uint8*)&g_tGuiVer, sizeof(Factory_Version_t));
    if(0 != ret)
    {
        /* 保存到私有文件 */
        read_pdata_xram("/data/app/stAmr/cfg/version", (char*)&g_tGuiVer, 0, sizeof(Factory_Version_t));
    }
}

void get_area_name(char *areaName)
{
    if (areaName == NULL)
    {
        printf("get_area_name input err!\n");
        return;
    }

#ifdef AREA_FUJIAN
        strcpy(areaName, "福建");
        return;
#endif

#ifdef AREA_ANHUI
    strcpy(areaName, "安徽");
    return;
#endif

#ifdef AREA_HUNAN
    strcpy(areaName, "湖南");
    return;
#endif

#ifdef AREA_ZHEJIANG
    strcpy(areaName, "浙江");
    return;
#endif

#ifdef AREA_JIANGXI
    strcpy(areaName, "江西");
    return;
#endif

#ifdef AREA_SHANDONG
    strcpy(areaName, "山东");
    return;
#endif

#ifdef AREA_GANSU
    strcpy(areaName, "甘肃");
    return;
#endif

#ifdef AREA_HUBEI
    strcpy(areaName, "湖北");
    return;
#endif

#ifdef AREA_SHAANXI
    strcpy(areaName, "陕西");
    return;
#endif

#ifdef AREA_SHANXI
    strcpy(areaName, "山西");
    return;
#endif

#ifdef AREA_HEBEI
    strcpy(areaName, "河北");
    return;
#endif

#ifdef AREA_HENAN
    strcpy(areaName, "河南");
    return;
#endif

#ifdef AREA_TIANJIN
    strcpy(areaName, "天津");
    return;
#endif

#ifdef AREA_CHONGQING
    strcpy(areaName, "重庆");
    return;
#endif

#ifdef AREA_JIBEI
    strcpy(areaName, "冀北");
    return;
#endif

#ifdef AREA_SICHUAN
    strcpy(areaName, "四川");
    return;
#endif

#ifdef AREA_JIANGSU
    strcpy(areaName, "江苏");
    return;
#endif

    strcpy(areaName, "国网");
    return;
}

void tmn_version_Draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;


    char czBuf[64] = { 0 };
    char strtemp[12] = { 0 };
    char strtemp1[12] = { 0 };
    char areaName[64] = { 0 };

    int nLeft = 5;
    int nLeft2 = 15;
    int nTop = 10;
    int tmpTop = 15;

    // 地区及厂商ID
    get_area_name(areaName);
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "地区及厂商ID");
    nTop += tmpTop;
    memset(strtemp, 0, 12);
    memset(strtemp1, 0, 12);
    memcpy(strtemp, g_tGuiVer.factory , 4);
    sprintf(czBuf, "%s %s", areaName, strtemp);
    TextOut(hdc, nLeft2, nTop, czBuf);

    // 软件版本号及发布时间
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "软件版本号及发布时间");
    nTop += tmpTop;
    memset(strtemp, 0, 12);
    memset(strtemp1, 0, 12);
    memcpy(strtemp, g_tGuiVer.softVer, 4);
    memcpy(strtemp1, g_tGuiVer.softDate, 6);
    sprintf(czBuf, "%s %s", strtemp, strtemp1);
    TextOut(hdc, nLeft2, nTop, czBuf);


    // 硬件版本号及发布时间
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "硬件版本号及发布时间");
    nTop += tmpTop;
    memset(strtemp, 0, 12);
    memset(strtemp1, 0, 12);
    memcpy(strtemp, g_tGuiVer.hardVer, 4);
    memcpy(strtemp1, g_tGuiVer.hardDate, 6);
    snprintf(czBuf, 64, "%s %s", strtemp, strtemp1);
    TextOut(hdc, nLeft2, nTop, czBuf);

    // 厂家扩展信息
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "厂家扩展信息");
    nTop += tmpTop;
    memset(strtemp, 0, 12);
    memset(strtemp1, 0, 12);
    memcpy(strtemp, g_tGuiVer.extend, 8);
    sprintf(czBuf, "%s", strtemp);
    TextOut(hdc, nLeft2, nTop, czBuf);
}

//终端版本信息
PLUGIN_INFO g_layer_tmn_version_display =
{
    0x11040009/*id*/,
    tmn_version_Init/*InitLayer*/,
    NULL/*UninitLayer*/,
    tmn_version_Draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("通信地址查看",1)

uint8 LocalAddr[16] = { 0 };

void address_display_init(HDC hdc)
{
    get_addr(LocalAddr);
}

void address_display_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "终端通信地址");
    rc.top = 26;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, pg_guicomm->fontSmall);

    memset(czBuf, 0x00, sizeof(czBuf));
#ifdef AREA_ZHEJIANG
    sprintf(czBuf, "%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
#else
    sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
#endif
    rc.top += 30;
    rc.bottom = rc.top + 10;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

}

//通信地址查看
PLUGIN_INFO g_layer_address_display = {
    0x11030001/*id*/,
    address_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    address_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
#endif


#if DESC("以太网通信参数查看",1)

OOP_ETHIP_T EthIP;      //终端ip
int EthIPisSet;
OOP_MASTERPARAMS_T EthMaster;
OOP_ETHCONFIG_T EthConfig;
int EthConfigisSet;
int gEthIndex;

int cmd_execute(const char* cmd,  char* result)
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
    } else {
        return -1;
    }
    return 0;
}

void ip_display_init(HDC hdc)
{
    memset(&EthIP, 0x00, sizeof(OOP_ETHIP_T));
    EthIPisSet = read_db_noamal_data(g_hUdbCenter, EthIPOad.value, 0, 0, (uint8*)&EthIP, sizeof(OOP_ETHIP_T));
    if (EthIPisSet < 0)
    {
        char cmd[200];
        char result[100];
        memset(cmd,0x00,sizeof(cmd));
        memset(result,0x00,sizeof(result));
        strcpy(cmd,"ifconfig FE0 | grep 'inet addr' | awk '{ print $2}' | awk -F: '{print $2}' ");
        cmd_execute(cmd,result);
        sscanf(result,"%u.%u.%u.%u",(uint32*)&EthIP.ip[0], (uint32*)&EthIP.ip[1], (uint32*)&EthIP.ip[2],(uint32*) &EthIP.ip[3]);
        
        memset(cmd,0x00,sizeof(cmd));
        memset(result,0x00,sizeof(result));
        strcpy(cmd,"ifconfig FE0 | grep 'Mask' | awk '{ print $4}' | awk -F: '{print $2}' ");
        cmd_execute(cmd,result);
        sscanf(result,"%u.%u.%u.%u",(uint32*)&EthIP.mask[0], (uint32*)&EthIP.mask[1], (uint32*)&EthIP.mask[2], (uint32*)&EthIP.mask[3]);

        memset(cmd,0x00,sizeof(cmd));
        memset(result,0x00,sizeof(result));
        strcpy(cmd,"ifconfig FE0 | grep 'Mask' | awk '{ print $3}' | awk -F: '{print $2}' ");
        cmd_execute(cmd,result);
        sscanf(result,"%u.%u.%u.%u",(uint32*)&EthIP.ipGateway[0], (uint32*)&EthIP.ipGateway[1], (uint32*)&EthIP.ipGateway[2], (uint32*)&EthIP.ipGateway[3]);
        EthIPisSet = 0;
    }

    

}


void ethmaster_display_init(HDC hdc)
{
    memset(&EthMaster, 0x00, sizeof(OOP_MASTERPARAMS_T));
    read_db_noamal_data(g_hUdbCenter, EthMasterOad.value, 0, 0, (uint8*)&EthMaster, sizeof(OOP_MASTERPARAMS_T));
}


void eth_display_init(HDC hdc)
{
    gEthIndex = 0;
    memset(&EthConfig, 0x00, sizeof(OOP_ETHCONFIG_T));
    EthConfigisSet = read_db_noamal_data(g_hUdbCenter, EthConfigOad.value, 0, 0, (uint8*)&EthConfig, sizeof(OOP_ETHCONFIG_T));
    ethmaster_display_init(hdc);
    ip_display_init(hdc);
}

void ip_display_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    char czBuf[300];

    int nTop = 30;
    int nLeft1 = 10;
    int nLeft2 = 70;

    TextOut(hdc, nLeft1, nTop, "IP地址");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthIPisSet < 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {

        sprintf(czBuf, "%d.%d.%d.%d", EthIP.ip[0], EthIP.ip[1], EthIP.ip[2], EthIP.ip[3]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 16;
    TextOut(hdc, nLeft1, nTop, "子网掩码");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthIPisSet < 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", EthIP.mask[0], EthIP.mask[1], EthIP.mask[2], EthIP.mask[3]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 16;
    TextOut(hdc, nLeft1, nTop, "网关地址");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthIPisSet < 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", EthIP.ipGateway[0], EthIP.ipGateway[1], EthIP.ipGateway[2], EthIP.ipGateway[3]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 16;
    TextOut(hdc, nLeft1, nTop, "主站地址");
    if (EthMaster.nNum < 1 || EthMaster.master[0].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[0].ip[0], EthMaster.master[0].ip[1], EthMaster.master[0].ip[2], EthMaster.master[0].ip[3]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 16;
    TextOut(hdc, nLeft1, nTop, "主站端口");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthMaster.nNum < 1 || EthMaster.master[0].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", EthMaster.master[0].port);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 16;
    nLeft2 += 20;
    ITEM_SELECT
    TextOut(hdc, nLeft2, nTop, "上一页");
    ITEM_NOT_SELECT;
    
}

void ethmaster_display_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    char czBuf[300];
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 25;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "工作模式");
    if (EthConfigisSet < 0 || EthConfig.workmode>2)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", g_work_mode[EthConfig.workmode]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 16;
    TextOut(hdc, nLeft1, nTop, "连接方式");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0 || EthConfig.contype>1)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%s", g_conn_type[EthConfig.contype]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 16;
    TextOut(hdc, nLeft1, nTop, "连接模式");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0 || EthConfig.conmode>1)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", g_conn_app_type[EthConfig.conmode]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 16;
    TextOut(hdc, nLeft1, nTop, "超时时间");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d 秒", EthConfig.dail.mask.dailtime);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 16;
    TextOut(hdc, nLeft1, nTop, "重发次数");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", EthConfig.dail.mask.dailcnt);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 16;
    TextOut(hdc, nLeft1, nTop, "心跳周期");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d 秒", EthConfig.heart);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 16;
    nLeft2 += 20;
    ITEM_SELECT
    TextOut(hdc, nLeft2, nTop, "下一页");
    ITEM_NOT_SELECT;

}


void eth_display_draw(HDC hdc)
{
    if (gEthIndex == 0)//第一页
    {
        return ethmaster_display_draw(hdc);
    }
    else    //第二页
    {
        return ip_display_draw(hdc);
    }
}

BOOL eth_display_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
    {
        return FALSE;
    }

    switch (nKey)
    {
    case  KEY_OK:
    case  KEY_UP:
    case  KEY_DOWN:
        if (gEthIndex == 0)
            gEthIndex = 1;
        else
            gEthIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    return FALSE;
}

//以太网通信参数查看
PLUGIN_INFO g_layer_eth_display = {
    0x11030002/*id*/,
    eth_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    eth_display_draw/*Draw*/,
    NULL/*Timer*/,
    eth_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("数据采集模式", 1)
uint8 g_GatherType = OOP_GATHER_FJ;
uint8 g_GatherTypeOld = OOP_GATHER_FJ;
uint8 g_GatherIndex = 0;

//运营商
const char* g_GatherName[] =
{
    "国网协议",
    "福建增补"
};

void gatherFJ_set_init(HDC hdc)
{
    int ret = 0;
    ret = read_db_noamal_data(g_hUdbCenter, 0x6E000200, 0, 0, &g_GatherType, sizeof(uint8));
    if(0 != ret || g_GatherType > OOP_GATHER_FJ)
    {
        g_GatherType = OOP_GATHER_FJ;
        g_GatherTypeOld = 0xff;                        //默认状态下，设置福建模式时写入数据中心
    }
    else
    {
        g_GatherTypeOld = g_GatherType;             //非默认状态下，采集模式没变化，不写数据中心
    }
    g_GatherIndex = 0;
}

void gatherFJ_set_draw(HDC hdc)
{
    int nTop = 24;
    int nLeft1 = 16;
    int nLeft2 = 60;
    int nLeft3 = 88;

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    TextOut(hdc, 32, nTop, "外部设备数据");
    nTop += 20;
    TextOut(hdc, 48, nTop, "采集模式");

    SelectFont(hdc, pg_guicomm->fontSmall);
    nTop += 32;
    TextOut(hdc, nLeft1, nTop, "模式: ");
    if(0 == g_GatherIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_GatherName[g_GatherType]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_GatherName[g_GatherType]);
    }

    SelectFont(hdc, pg_guicomm->fontBig);
    nTop += 32;
    if(1 == g_GatherIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, 30, nTop, "返回");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 30, nTop, "返回");
    }

    if(2 == g_GatherIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, "确认");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, "确认");
    }
    
    PopListDraw(hdc);
}

BOOL gatherFJ_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
        
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;
        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            g_GatherType = nSel;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        return TRUE;
    }
    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_GatherIndex > 0)
            {
                g_GatherIndex--;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_GatherIndex < 2)
            {
                g_GatherIndex++;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            if(1 == g_GatherIndex)
            {
                CancelMenu();
                return TRUE;
            }
            else if(TRUE == ValidatePwd())
            {
                if(0 == g_GatherIndex)
                {
                    PopList(g_GatherName, 2, g_GatherName[g_GatherType], 80, 80, FALSE);
                    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                    return TRUE;
                }
                else if(2 == g_GatherIndex)
                {
                    if(g_GatherType != g_GatherTypeOld)
                    {
                        if (TRUE == MessageBox("需要重启终端生效\n\n是否确定生效配置？", MB_OKCANCEL))
                        {
                            int ret = 0;
                            OOP_OAD_U gatherOad;
                            gatherOad.value = 0x6E000200;
                            ret = write_normal_data(&g_GatherType, 1, gatherOad, 0, 0, CLASS_DATA_UNINIT);
                            if (ret != 0)
                            {
                                MessageBox("数据采集模式参数写入失败！");
                            }
                            else
                            {
                                RebootTerminal();
                                MessageBox("设置成功\n\n终端重启！");
                            }
                        }
                    }
                    else
                    {   
                        MessageBox("采集模式没有变化\n\n退出设置页面！");
                    
}
                    CancelMenu();
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

PLUGIN_INFO g_layer_GatherFJ_set = {
    0x11030084/*id*/,
    gatherFJ_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gatherFJ_set_draw/*Draw*/,
    NULL/*Timer*/,
    gatherFJ_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("RS485端口数据共享", 1)
uint8 g_Rs485_status = 0;
uint8 g_Rs485_statusOld = 0;
uint8 g_Rs485Index = 0;

//运营商
const char* g_Rs485StatusName[] =
{
    "停用",
    "启用"
};

void rs485Status_set_init(HDC hdc)
{
    g_Rs485_status = 0;
    read_db_noamal_data(g_hUdbCenter, 0x6E030300, 0, 0, &g_Rs485_status, sizeof(uint8));
    if(g_Rs485_status > 1)
    {
        g_Rs485_status = 0;
    }
    g_Rs485Index = 0;
    g_Rs485_statusOld = g_Rs485_status;
}

void rs485Status_set_draw(HDC hdc)
{
    int nTop = 24;
    int nLeft1 = 16;
    int nLeft2 = 60;
    int nLeft3 = 88;

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    TextOut(hdc, 4, nTop, "RS485-2端口数据共享");
    nTop += 20;
    TextOut(hdc, 48, nTop, "功能设置");

    SelectFont(hdc, pg_guicomm->fontSmall);
    nTop += 32;
    TextOut(hdc, nLeft1, nTop, "设置: ");
    if(0 == g_Rs485Index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_Rs485StatusName[g_Rs485_status]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_Rs485StatusName[g_Rs485_status]);
    }

    SelectFont(hdc, pg_guicomm->fontBig);
    nTop += 32;
    if(1 == g_Rs485Index)
    {
        ITEM_SELECT;
        TextOut(hdc, 30, nTop, "返回");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 30, nTop, "返回");
    }

    if(2 == g_Rs485Index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, "确认");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, "确认");
    }
    
    PopListDraw(hdc);
}

BOOL rs485Status_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
        
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;
        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            g_Rs485_status = nSel;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        return TRUE;
    }
    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_Rs485Index > 0)
            {
                g_Rs485Index--;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_Rs485Index < 2)
            {
                g_Rs485Index++;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            if(1 == g_Rs485Index)
            {
                CancelMenu();
                return TRUE;
            }
            else if(TRUE == ValidatePwd())
            {
                if(0 == g_Rs485Index)
                {
                    PopList(g_Rs485StatusName, 2, g_Rs485StatusName[g_Rs485_status], 80, 80, FALSE);
                    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                    return TRUE;
                }
                else if(2 == g_Rs485Index)
                {
                    if(g_Rs485_status != g_Rs485_statusOld)
                    {
                        int ret = 0;
                        OOP_OAD_U statusOad;
                        statusOad.value = 0x6E030300;
                        ret = write_normal_data(&g_Rs485_status, 1, statusOad, 0, 0, CLASS_DATA_INIT);
                        if (ret != 0)
                        {
                            MessageBox("485数据共享写入失败！");
                        }
                        else
                        {
                            MessageBox("设置成功！");
                        }
                    }
                    CancelMenu();
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

PLUGIN_INFO g_layer_rs485Status_set = {
    0x11030085/*id*/,
    rs485Status_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    rs485Status_set_draw/*Draw*/,
    NULL/*Timer*/,
    rs485Status_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif


#if DESC("以太网通信参数设置",1)

//uint8 EthConfigIndex;
//void eth_set_init(HDC hdc)
//{
//    memset(&EthConfig, 0x00, sizeof(OOP_ETHCONFIG_T));
//    EthConfigisSet = read_db_noamal_data(g_hUdbCenter, EthConfigOad.value, 0, 0, (uint8*)&EthConfig, sizeof(OOP_ETHCONFIG_T));
//  ethmaster_display_init(hdc);
//  ip_display_init(hdc);
//    EthConfigIndex = 0;
//}
//
//void ip_set_draw(HDC hdc)
//{
//    GUI_COMM* pg_guicomm = &g_guiComm;
//    if (pg_guicomm == NULL)
//        return;
//
//    uint8 index = 7;
//    ITEM_NOT_SELECT;
//    char czBuf[300];
//    SelectFont(hdc, pg_guicomm->fontSmall);
//
//    int nTop = 30;
//    int nLeft1 = 10;
//    int nLeft2 = 70;
//
//    TextOut(hdc, nLeft1, nTop, "IP地址");
//    memset(czBuf, 0x00, sizeof(czBuf));
//    if (EthIPisSet < 0 || EthIP.ip[0] == 0)
//    {
//        sprintf(czBuf, "-.-.-.-");
//    }
//    else
//    {
//
//        sprintf(czBuf, "%d.%d.%d.%d", EthIP.ip[0], EthIP.ip[1], EthIP.ip[2], EthIP.ip[3]);
//    }
//    if (EthConfigIndex == index)
//    {
//        ITEM_SELECT
//            TextOut(hdc, nLeft2, nTop, czBuf);
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, czBuf);
//    }
//
//    index++;
//    nTop += 16;
//    TextOut(hdc, nLeft1, nTop, "子网掩码");
//    memset(czBuf, 0x00, sizeof(czBuf));
//    if (EthIPisSet < 0 || EthIP.mask[0] == 0)
//    {
//        sprintf(czBuf, "-.-.-.-");
//    }
//    else
//    {
//
//        sprintf(czBuf, "%d.%d.%d.%d", EthIP.mask[0], EthIP.mask[1], EthIP.mask[2], EthIP.mask[3]);
//    }
//    if (EthConfigIndex == index)
//    {
//        ITEM_SELECT
//            TextOut(hdc, nLeft2, nTop, czBuf);
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, czBuf);
//    }
//
//    index++;
//    nTop += 16;
//    TextOut(hdc, nLeft1, nTop, "网关地址");
//    memset(czBuf, 0x00, sizeof(czBuf));
//    if (EthIPisSet < 0 || EthIP.ipGateway[0] == 0)
//    {
//        sprintf(czBuf, "-.-.-.-");
//    }
//    else
//    {
//
//        sprintf(czBuf, "%d.%d.%d.%d", EthIP.ipGateway[0], EthIP.ipGateway[1], EthIP.ipGateway[2], EthIP.ipGateway[3]);
//    }
//    if (EthConfigIndex == index)
//    {
//        ITEM_SELECT
//            TextOut(hdc, nLeft2, nTop, czBuf);
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, czBuf);
//    }
//
//    index++;
//    nTop += 16;
//    TextOut(hdc, nLeft1, nTop, "主站地址");
//    if (EthMaster.nNum < 1 || EthMaster.master[0].ip[0] == 0)
//    {
//        sprintf(czBuf, "-.-.-.-");
//    }
//    else
//    {
//        sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[0].ip[0], EthMaster.master[0].ip[1], EthMaster.master[0].ip[2], EthMaster.master[0].ip[3]);
//    }
//    if (EthConfigIndex == index)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft2, nTop, czBuf);
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, czBuf);
//    }
//
//    index++;
//    nTop += 16;
//    TextOut(hdc, nLeft1, nTop, "主站端口");
//    memset(czBuf, 0x00, sizeof(czBuf));
//    if (EthMaster.nNum < 1 || EthMaster.master[0].port == 65535)
//    {
//        sprintf(czBuf, "-");
//    }
//    else
//    {
//
//        sprintf(czBuf, "%d", EthMaster.master[0].port);
//    }
//    if (EthConfigIndex == index)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft2, nTop, czBuf);
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, czBuf);
//    }
//
//  index++;
//  nTop += 16;
//    nLeft2 += 20;
//  if (EthConfigIndex == index)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft2, nTop, "上一页");
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, "上一页");
//    }
//
//    index++;
//    nTop += 16;
//
//    if (EthConfigIndex == index)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft1, nTop, "取消");
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft1, nTop, "取消");
//    }
//
//    index++;
//    nLeft2 = 80;
//    if (EthConfigIndex == index)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft2, nTop, "保存");
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, "保存");
//    }
//
//    PopListDraw(hdc);
//}
//
//void ethmaster_set_draw(HDC hdc)
//{
//    GUI_COMM* pg_guicomm = &g_guiComm;
//    if (pg_guicomm == NULL)
//        return;
//    int index = 0;
//    ITEM_NOT_SELECT;
//    char czBuf[300];
//    SelectFont(hdc, pg_guicomm->fontSmall);
//    int nTop = 20;
//    int nLeft1 = 10;
//    int nLeft2 = 70;
//    TextOut(hdc, nLeft1, nTop, "工作模式");
//    if (EthConfigisSet < 0 || EthConfig.workmode>2)
//    {
//        sprintf(czBuf, "-");
//    }
//    else
//    {
//        sprintf(czBuf, "%s", g_work_mode[EthConfig.workmode]);
//    }
//    if (EthConfigIndex == index)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft2, nTop, czBuf);
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, czBuf);
//    }
//
//    index++;
//    nTop += 16;
//    TextOut(hdc, nLeft1, nTop, "连接方式");
//    memset(czBuf, 0x00, sizeof(czBuf));
//    if (EthConfigisSet < 0 || EthConfig.contype>1)
//    {
//        sprintf(czBuf, "-");
//    }
//    else
//    {
//
//        sprintf(czBuf, "%s", g_conn_type[EthConfig.contype]);
//    }
//    if (EthConfigIndex == index)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft2, nTop, czBuf);
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, czBuf);
//    }
//
//
//    index++;
//    nTop += 16;
//    TextOut(hdc, nLeft1, nTop, "连接模式");
//    memset(czBuf, 0x00, sizeof(czBuf));
//    if (EthConfigisSet < 0 || EthConfig.conmode>1)
//    {
//        sprintf(czBuf, "-");
//    }
//    else
//    {
//        sprintf(czBuf, "%s", g_conn_app_type[EthConfig.conmode]);
//    }
//    if (EthConfigIndex == index)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft2, nTop, czBuf);
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, czBuf);
//    }
//
//    index++;
//    nTop += 16;
//    TextOut(hdc, nLeft1, nTop, "超时时间");
//    memset(czBuf, 0x00, sizeof(czBuf));
//    if (EthConfigisSet < 0)
//    {
//        sprintf(czBuf, "-");
//    }
//    else
//    {
//
//        sprintf(czBuf, "%d 秒", EthConfig.dail.mask.dailtime);
//    }
//    if (EthConfigIndex == index)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft2, nTop, czBuf);
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, czBuf);
//    }
//
//    index++;
//    nTop += 16;
//    TextOut(hdc, nLeft1, nTop, "重发次数");
//    memset(czBuf, 0x00, sizeof(czBuf));
//    if (EthConfigisSet < 0)
//    {
//        sprintf(czBuf, "-");
//    }
//    else
//    {
//
//        sprintf(czBuf, "%d", EthConfig.dail.mask.dailcnt);
//    }
//    if (EthConfigIndex == index)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft2, nTop, czBuf);
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, czBuf);
//    }
//
//    index++;
//    nTop += 16;
//    TextOut(hdc, nLeft1, nTop, "心跳周期");
//    memset(czBuf, 0x00, sizeof(czBuf));
//    if (EthConfigisSet < 0)
//    {
//        sprintf(czBuf, "-");
//    }
//    else
//    {
//
//        sprintf(czBuf, "%d 秒", EthConfig.heart);
//    }
//    if (EthConfigIndex == index)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft2, nTop, czBuf);
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, czBuf);
//    }
//
//  index++;
//  nTop += 16;
//    nLeft2 += 20;
//  if (EthConfigIndex == index)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft2, nTop, "下一页");
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, "下一页");
//    }
//
//    PopListDraw(hdc);
//
//}
//
//
//void eth_set_draw(HDC hdc)
//{
//   if (EthConfigIndex < 7)//第一页
//    {
//        return ethmaster_set_draw(hdc);
//    }
//    else    //第二页
//    {
//        return ip_set_draw(hdc);
//    }
//}
//
////将字符串形式的ip转换为数组 失败返回false
//BOOL commfun_IP(char* ipin, uint8* ipout)
//{
//    if (ipin == NULL || strlen(ipin) == 0)
//        return FALSE;
//    char* p1[4];
//    char czBuf[20];
//    strcpy(czBuf, ipin);
//
//    char* pTmp = p1[0] = czBuf;
//    int nIndex = 1;
//    while (*pTmp != '\0')
//    {
//        if (*pTmp == '.')
//        {
//            *pTmp = '\0';
//            pTmp++;
//            if (*pTmp == '\0')
//            {
//                break;
//            }
//            else
//            {
//                if (*pTmp > '9' || *pTmp < '0')
//                    return FALSE;
//                if (nIndex > 4)
//                    return FALSE;
//                p1[nIndex] = pTmp;
//                nIndex++;
//            }
//        }
//        else if (*pTmp > '9' || *pTmp < '0')
//        {
//            return FALSE;
//        }
//        pTmp++;
//    }
//    if (nIndex != 4)
//    {
//        return FALSE;
//    }
//    ipout[0] = atoi(p1[0]);
//    ipout[1] = atoi(p1[1]);
//    ipout[2] = atoi(p1[2]);
//    ipout[3] = atoi(p1[3]);
//    return TRUE;
//}
//
//void IpConfigReturn(char* pNewStr, void* pPara)
//{
//    char czBuf[20] = { 0 };
//    uint8 tmp[4] = { 0 };
//
//    switch (EthConfigIndex)
//    {
//      case 8: //ip地址
//      {
//          if (FALSE == commfun_IP(pNewStr, tmp))
//          {
//              if (TRUE == MessageBox("IP地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
//              {
//                  if (EthIPisSet < 0 || EthIP.ip[0] == 0)
//                  {
//                      strcpy(czBuf, "");
//                  }
//                  else
//                  {
//                      sprintf(czBuf, "%d.%d.%d.%d", EthIP.ip[0], EthIP.ip[1], EthIP.ip[2], EthIP.ip[3]);
//                  }
//                  InputBox(czBuf, IpConfigReturn, NULL, 0x02);
//              }
//          }
//          else
//          {
//              memcpy(EthIP.ip, tmp, 4);
//              EthIPisSet = 1;
//          }
//      }
//      break;
//      case 9://子网掩码
//      {
//          if (FALSE == commfun_IP(pNewStr, tmp))
//          {
//              if (TRUE == MessageBox("子网掩码格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
//              {
//                  if (EthIPisSet < 0 || EthIP.mask[0] == 0)
//                  {
//                      strcpy(czBuf, "");
//                  }
//                  else
//                  {
//                      sprintf(czBuf, "%d.%d.%d.%d", EthIP.mask[0], EthIP.mask[1], EthIP.mask[2], EthIP.mask[3]);
//                  }
//                  InputBox(czBuf, IpConfigReturn, NULL, 0x02);
//              }
//          }
//          else
//          {
//              memcpy(EthIP.mask, tmp, 4);
//              EthIPisSet = 1;
//          }
//      }
//      break;
//      case 10://网关地址
//      {
//          if (FALSE == commfun_IP(pNewStr, tmp))
//          {
//              if (TRUE == MessageBox("网关地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
//              {
//                  if (EthIPisSet < 0 || EthIP.ipGateway[0] == 0)
//                  {
//                      strcpy(czBuf, "");
//                  }
//                  else
//                  {
//                      sprintf(czBuf, "%d.%d.%d.%d", EthIP.ipGateway[0], EthIP.ipGateway[1], EthIP.ipGateway[2], EthIP.ipGateway[3]);
//                  }
//                  InputBox(czBuf, IpConfigReturn, NULL, 0x02);
//              }
//          }
//          else
//          {
//              memcpy(EthIP.ipGateway, tmp, 4);
//              EthIPisSet = 1;
//          }
//      }
//      break;
//    }
//
//}
//
//void EthMasterReturn(char* pNewStr, void* pPara)
//{
//    char czBuf[20] = { 0 };
//    uint8 tmp[4] = { 0 };
//
//    switch (EthConfigIndex)
//    {
//      case 11: //IP地址
//      {
//          if (FALSE == commfun_IP(pNewStr, tmp))
//          {
//              if (TRUE == MessageBox("IP地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
//              {
//                  if (EthMaster.nNum < 1 || EthMaster.master[0].ip[0] == 0)
//                  {
//                      strcpy(czBuf, "");
//                  }
//                  else
//                  {
//                      sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[0].ip[0], EthMaster.master[0].ip[1], EthMaster.master[0].ip[2], EthMaster.master[0].ip[3]);
//                  }
//                  InputBox(czBuf, EthMasterReturn, NULL, 0x02);
//              }
//          }
//          else
//          {
//              memcpy(EthMaster.master[0].ip, tmp, 4);
//              if (EthMaster.nNum < 1)
//                  EthMaster.nNum = 1;
//          }
//      }
//      break;
//      case 12:     //端口
//      {
//          int port = atoi(pNewStr);
//          if (port <= 65535)
//          {
//              EthMaster.master[0].port = port;
//              if (EthMaster.nNum < 1)
//                  EthMaster.nNum = 1;
//          }
//          else
//          {
//              if (TRUE == MessageBox("端口格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
//              {
//                  if (EthMaster.nNum < 1 || EthMaster.master[0].port == 65535)
//                  {
//                      strcpy(czBuf, "");
//                  }
//                  else
//                  {
//
//                      sprintf(czBuf, "%d", EthMaster.master[0].port);
//                  }
//                  InputBox(czBuf, EthMasterReturn, NULL, 0x01);
//              }
//          }
//      }
//      break;
//    }
//}
//
//
//void EthConfigReturn(char* pNewStr, void* pPara)
//{
//    char czBuf[20] = { 0 };
//    uint8 tmp[4] = { 0 };
//
//    switch (EthConfigIndex)
//    {
//    case 3: //代理服务器地址
//    {
//        if (FALSE == commfun_IP(pNewStr, tmp))
//        {
//            if (TRUE == MessageBox("IP地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
//            {
//                if (EthConfigisSet < 0 || EthConfig.proxyip[0] == 0)
//                {
//                    strcpy(czBuf, "");
//                }
//                else
//                {
//                    sprintf(czBuf, "%d.%d.%d.%d", EthConfig.proxyip[0], EthConfig.proxyip[1], EthConfig.proxyip[2], EthConfig.proxyip[3]);
//                }
//                InputBox(czBuf, EthConfigReturn, NULL, 0x02);
//            }
//        }
//        else
//        {
//            memcpy(EthConfig.proxyip, tmp, 4);
//            EthConfigisSet = 1;
//        }
//    }
//    break;
//    case 4:     //代理端口
//    {
//        int port = atoi(pNewStr);
//        if (port <= 65535)
//        {
//            EthConfig.proxyport = port;
//            EthConfigisSet = 1;
//        }
//        else
//        {
//            if (TRUE == MessageBox("端口格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
//            {
//                if (EthConfigisSet < 0 || EthConfig.proxyport == 65535)
//                {
//                    strcpy(czBuf, "");
//                }
//                else
//                {
//                    sprintf(czBuf, "%d", EthConfig.proxyport);
//                }
//                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
//            }
//        }
//    }
//    break;
//    case 5:     //超时时间
//    {
//        int time = atoi(pNewStr);
//        if (time <= 0x3f)      //超时时间占6位 最大0x3f
//        {
//            EthConfig.dail.mask.dailtime = time;
//            EthConfigisSet = 1;
//        }
//        else
//        {
//            if (TRUE == MessageBox("超时时间最大63 \n\n确认是否重新输入?", MB_OKCANCEL))
//            {
//                if (EthConfigisSet < 0)
//                {
//                    strcpy(czBuf, "");
//                }
//                else
//                {
//                    sprintf(czBuf, "%d", EthConfig.dail.mask.dailtime);
//                }
//                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
//            }
//        }
//    }
//    break;
//    case 6:     //重发次数
//    {
//        int dailcnt = atoi(pNewStr);
//        if (dailcnt <= 3)      //重发次数占2位 最大0x3
//        {
//            EthConfig.dail.mask.dailcnt = dailcnt;
//            EthConfigisSet = 1;
//        }
//        else
//        {
//            if (TRUE == MessageBox("重发次数最大3 \n\n确认是否重新输入?", MB_OKCANCEL))
//            {
//                if (EthConfigisSet < 0)
//                {
//                    strcpy(czBuf, "");
//                }
//                else
//                {
//                    sprintf(czBuf, "%d", EthConfig.dail.mask.dailcnt);
//                }
//                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
//            }
//        }
//    }
//    break;
//    case 7:     //心跳周期
//    {
//        int heart = atoi(pNewStr);
//        if (heart <= 65535)      //
//        {
//            EthConfig.heart = heart;
//            EthConfigisSet = 1;
//        }
//        else
//        {
//            if (TRUE == MessageBox("心跳周期有误\n\n确认是否重新输入?", MB_OKCANCEL))
//            {
//                if (EthConfigisSet < 0)
//                {
//                    strcpy(czBuf, "");
//                }
//                else
//                {
//                    sprintf(czBuf, "%d", EthConfig.heart);
//                }
//                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
//            }
//        }
//    }
//    break;
//    }
//}
//
//void save_eth_config(void)
//{
//  int ret = 0;
//  int rtn = 0;
//  
//  ret = write_normal_data((uint8*)&EthMaster, sizeof(OOP_MASTERPARAMS_T), EthMasterOad, 0, 0, 1);
//    if (ret != 0)
//    {
//        MessageBox("以太网主站参数写入失败！");
//      rtn = 1;
//    }
//  ret = write_normal_data((uint8*)&EthConfig, sizeof(OOP_ETHCONFIG_T), EthConfigOad, 0, 0, 1);
//    if (ret != 0)
//  {
//        MessageBox("以太网通信配置参数写入失败！");
//      rtn = 1;
//    }
//  ret = write_normal_data((uint8*)&EthIP, sizeof(OOP_ETHIP_T), EthIPOad, 0, 0, 1);
//    if (ret != 0)
//    {
//        MessageBox("IP地址配置写入失败！");
//      rtn = 1;
//    }
//    else
//    {
//        eth_work_loginout_requet(0);
//        sleep(5);
//      ret = ipconfig_send_to_smiOS((char*)"eth0", EthIP.ipConfig, EthIP.ip, EthIP.mask, EthIP.ipGateway);
//        if (ret != 0)
//        {
//            MessageBox("设置终端IP地址失败！");
//          rtn = 1;
//        }
//        uint8 flag = 0;
//        OOP_OAD_U oadnum;
//        oadnum.value = 0x451004FF;
//        write_normal_data(&flag, sizeof(uint8), oadnum, 0, 0, 1);
//    }
//  
//    if (rtn == 0)
//    {
//        MessageBox("以太网参数设置成功！");
//    }
//  return ;
//}
//
//BOOL eth_set_keymsg(int nKey)
//{
//    GUI_COMM* pg_guicomm = &g_guiComm;
//    if (pg_guicomm == NULL)
//        return FALSE;
//    int maxindex = 14;
//    if (pg_guicomm->bPopList == TRUE)
//    {
//        int nSel;
//
//        PopListKeyMsg(nKey, &nSel);
//        if (nKey == KEY_OK)
//        {
//            switch (EthConfigIndex)
//            {
//            case 0://工作模式
//            {
//                EthConfig.workmode = nSel;
//                EthConfigisSet = 1;
//            }
//            break;
//            case 1://连接方式
//            {
//                EthConfig.contype = nSel;
//                EthConfigisSet = 1;
//            }
//            break;
//            case 2://连接应用方式
//            {
//                EthConfig.conmode = nSel;
//                EthConfigisSet = 1;
//            }
//            break;
//            default:
//                return FALSE;
//            }
//            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
//        }
//        return TRUE;
//    }
//    switch (nKey)
//    {
//    case KEY_UP:
//    case KEY_LEFT:
//    {
//        if (EthConfigIndex > 0)
//            EthConfigIndex--;
//        else
//            EthConfigIndex = maxindex;
//        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
//        return TRUE;
//    }
//    case KEY_DOWN:
//    case KEY_RIGHT:
//    {
//        if (EthConfigIndex < maxindex)
//            EthConfigIndex++;
//        else
//            EthConfigIndex = 0;
//        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
//        return TRUE;
//    }
//    case KEY_OK:
//    {
//        if (ValidatePwd() == TRUE)
//        {
//            switch (EthConfigIndex)
//            {
//            case 0: //工作模式
//            {
//                PopList(g_work_mode, 3, g_work_mode[0], 70, 20, FALSE);
//                return TRUE;
//            }
//            case 1://连接方式
//            {
//                PopList(g_conn_type, 2, g_conn_type[0], 70, 40, FALSE);
//                return TRUE;
//            }
//            case 2://连接应用方式
//            {
//                PopList(g_conn_app_type, 2, g_conn_app_type[0], 70, 60, FALSE);
//                return TRUE;
//            }
//            case 3://超时时间
//            {
//                char czBuf[20];
//                if (EthConfigisSet < 0)
//                {
//                    strcpy(czBuf, "");
//                }
//                else
//                {
//                    sprintf(czBuf, "%d", EthConfig.dail.mask.dailtime);
//                }
//                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
//                return TRUE;
//            }
//            case 4://重发次数
//            {
//                char czBuf[20];
//                if (EthConfigisSet < 0)
//                {
//                    strcpy(czBuf, "");
//                }
//                else
//                {
//                    sprintf(czBuf, "%d", EthConfig.dail.mask.dailcnt);
//                }
//                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
//                return TRUE;
//            }
//            case 5://心跳周期
//            {
//                char czBuf[20];
//                if (EthConfigisSet < 0)
//                {
//                    strcpy(czBuf, "");
//                }
//                else
//                {
//                    sprintf(czBuf, "%d", EthConfig.heart);
//                }
//                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
//                return TRUE;
//            }
//          case 6://下一页
//            {
//              EthConfigIndex = 7;
//              UpdateWindow(pg_guicomm->hMainWnd, TRUE);
//                return TRUE;
//            }
//          case 12://上一页
//            {
//              EthConfigIndex = 0;
//              UpdateWindow(pg_guicomm->hMainWnd, TRUE);
//                return TRUE;
//            }
//            case 7://IP地址
//            {
//                char czBuf[20];
//                if (EthIPisSet < 0 || EthIP.ip[0] == 0)
//                {
//                    strcpy(czBuf, "");
//                }
//                else
//                {
//                    sprintf(czBuf, "%d.%d.%d.%d", EthIP.ip[0], EthIP.ip[1], EthIP.ip[2], EthIP.ip[3]);
//                }
//                InputBox(czBuf, IpConfigReturn, NULL, 0x02);
//                return TRUE;
//            }
//            case 8://子网掩码
//            {
//                char czBuf[20];
//                if (EthIPisSet < 0 || EthIP.mask[0] == 0)
//                {
//                    strcpy(czBuf, "");
//                }
//                else
//                {
//                    sprintf(czBuf, "%d.%d.%d.%d", EthIP.mask[0], EthIP.mask[1], EthIP.mask[2], EthIP.mask[3]);
//                }
//                InputBox(czBuf, IpConfigReturn, NULL, 0x02);
//                return TRUE;
//            }
//            case 9://网关地址
//            {
//                char czBuf[20];
//                if (EthIPisSet < 0 || EthIP.ipGateway[0] == 0)
//                {
//                    strcpy(czBuf, "");
//                }
//                else
//                {
//                    sprintf(czBuf, "%d.%d.%d.%d", EthIP.ipGateway[0], EthIP.ipGateway[1], EthIP.ipGateway[2], EthIP.ipGateway[3]);
//                }
//                InputBox(czBuf, IpConfigReturn, NULL, 0x02);
//                return TRUE;
//            }
//          case 10://IP地址
//            {
//                char czBuf[20];
//                if (EthMaster.nNum < 1 || EthMaster.master[0].ip[0] == 0)
//                {
//                    strcpy(czBuf, "");
//                }
//                else
//                {
//                    sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[0].ip[0], EthMaster.master[0].ip[1], EthMaster.master[0].ip[2], EthMaster.master[0].ip[3]);
//                }
//                InputBox(czBuf, EthMasterReturn, NULL, 0x02);
//                return TRUE;
//            }
//            case 11://端口
//            {
//                char czBuf[20];
//                if (EthMaster.nNum < 1 || EthMaster.master[0].port == 65535)
//                {
//                    strcpy(czBuf, "");
//                }
//                else
//                {
//                    sprintf(czBuf, "%d", EthMaster.master[0].port);
//                }
//                InputBox(czBuf, EthMasterReturn, NULL, 0x01);
//                return TRUE;
//            }
//            case 13://取消
//            {
//              CancelMenu();
//                if (TRUE == MessageBox("当前配置没有保存\n\n是否确定保存配置？", MB_OKCANCEL))
//                {
//                    save_eth_config();
//                }
//                EthConfigIndex = 0;
//                return TRUE;
//            }
//            case 14://保存
//            {
//              save_eth_config();
//                EthConfigIndex = 0;
//              CancelMenu();
//                return TRUE;
//            }
//            }
//        }
//    }
//    }
//    return FALSE;
//}
//
////以太网通信配置设置
//PLUGIN_INFO g_layer_eth_set = {
//    0x11030012/*id*/,
//    eth_set_init/*InitLayer*/,
//    NULL/*UninitLayer*/,
//    eth_set_draw/*Draw*/,
//    NULL/*Timer*/,
//    eth_set_keymsg/*keymsg*/,
//    NULL/*ipc*/
//};

int guicmd_execute(const char* cmd,  char* result)
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

uint8 EthIPIndex;
void ip_set_init(HDC hdc)
{
    memset(&EthIP, 0x00, sizeof(OOP_ETHIP_T));
    EthIPisSet = read_db_noamal_data(g_hUdbCenter, EthIPOad.value, 0, 0, (uint8*)&EthIP, sizeof(OOP_ETHIP_T));
    EthIPIndex = 0;
}

void ip_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    uint8 index = 0;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "IP地址配置");
    rc.top = 20;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    int nTop = 40;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "配置方式");
    if (EthIPisSet < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        if (EthIP.ipConfig < 3)
            sprintf(czBuf, "%s", g_ipconfig[EthIP.ipConfig]);
    }
    if (EthIPIndex == index)
    {
        ITEM_SELECT
            TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "IP地址");
    memset(czBuf, 0x00, sizeof(czBuf));
    if(0 == EthIPisSet && 0 == EthIP.ipConfig)
    {
        char cmd[100] = {0};
        char result[100] = {0};
        uint32 ipaddr[4] = {0};
        
        strcpy(cmd,"ifconfig FE0 | grep 'inet addr' | awk '{ print $2}' | awk -F: '{print $2}' ");
        cmd_execute(cmd, result);
        sscanf(result, "%u.%u.%u.%u", &ipaddr[0], &ipaddr[1], &ipaddr[2], &ipaddr[3]);
        if(0 == ipaddr[0])
        {
            sprintf(czBuf, "-.-.-.-");
        }
        else
        {
            sprintf(czBuf, "%d.%d.%d.%d", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
        }
    }
    else if (EthIPisSet < 0 || EthIP.ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", EthIP.ip[0], EthIP.ip[1], EthIP.ip[2], EthIP.ip[3]);
    }
    if (EthIPIndex == index)
    {
        ITEM_SELECT
            TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "子网掩码");
    memset(czBuf, 0x00, sizeof(czBuf));
    if(0 == EthIPisSet && 0 == EthIP.ipConfig)
    {
        char cmd[100] = {0};
        char result[100] = {0};
        uint32 ipaddr[4] = {0};
        
        strcpy(cmd,"ifconfig FE0 | grep 'Mask' | awk '{ print $4}' | awk -F: '{print $2}' ");
        cmd_execute(cmd, result);
        sscanf(result, "%u.%u.%u.%u", &ipaddr[0], &ipaddr[1], &ipaddr[2], &ipaddr[3]);
        if(0 == ipaddr[0])
        {
            sprintf(czBuf, "-.-.-.-");
        }
        else
        {
            sprintf(czBuf, "%d.%d.%d.%d", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
        }
    }
    else if (EthIPisSet < 0 || EthIP.mask[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", EthIP.mask[0], EthIP.mask[1], EthIP.mask[2], EthIP.mask[3]);
    }
    if (EthIPIndex == index)
    {
        ITEM_SELECT
            TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "网关地址");
    memset(czBuf, 0x00, sizeof(czBuf));
    if(0 == EthIPisSet && 0 == EthIP.ipConfig)
    {
        char cmd[100] = {0};
        char result[100] = {0};
        uint32 ipaddr[4] = {0};
        
        strcpy(cmd,"route -n|grep FE0|grep UG| awk '{ print $2}'");
        cmd_execute(cmd, result);
        sscanf(result, "%u.%u.%u.%u", &ipaddr[0], &ipaddr[1], &ipaddr[2], &ipaddr[3]);
        if(0 == ipaddr[0])
        {
            sprintf(czBuf, "-.-.-.-");
        }
        else
        {
            sprintf(czBuf, "%d.%d.%d.%d", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
        }
    }
    else if (EthIPisSet < 0 || EthIP.ipGateway[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", EthIP.ipGateway[0], EthIP.ipGateway[1], EthIP.ipGateway[2], EthIP.ipGateway[3]);
    }
    if (EthIPIndex == index)
    {
        ITEM_SELECT
            TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "用户名");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthIPisSet < 0 || EthIP.pppoeUser.nNum == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%s", EthIP.pppoeUser.value);
    }
    if (EthIPIndex == index)
    {
        ITEM_SELECT
            TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "密码");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthIPisSet < 0 || EthIP.pppoePwd.nNum == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%s", EthIP.pppoePwd.value);
    }
    if (EthIPIndex == index)
    {
        ITEM_SELECT
            TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;

    if (EthIPIndex == index)
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
    if (EthIPIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "保存配置");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "保存配置");
    }

    PopListDraw(hdc);
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
    for(int index = 0; index < 4; index++)
    {
        if(atoi(p1[index])>255)
        {
            return FALSE;
        }
    }    
    for (int i =0;i<4;i++){
        if(atoi(p1[i]) > 255)
            return FALSE;
        ipout[i] = atoi(p1[i]);
    }
    return TRUE;
}

void IpConfigReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[4] = { 0 };

    switch (EthIPIndex)
    {
    case 1: //ip地址
    {
        if (FALSE == commfun_IP(pNewStr, tmp))
        {
            if (TRUE == MessageBox("IP地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (EthIPisSet < 0 || EthIP.ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthIP.ip[0], EthIP.ip[1], EthIP.ip[2], EthIP.ip[3]);
                }
                InputBox(czBuf, IpConfigReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(EthIP.ip, tmp, 4);
            EthIPisSet = 1;
        }
    }
    break;
    case 2://子网掩码
    {
        if (FALSE == commfun_IP(pNewStr, tmp))
        {
            if (TRUE == MessageBox("子网掩码格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (EthIPisSet < 0 || EthIP.mask[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthIP.mask[0], EthIP.mask[1], EthIP.mask[2], EthIP.mask[3]);
                }
                InputBox(czBuf, IpConfigReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(EthIP.mask, tmp, 4);
            EthIPisSet = 1;
        }
    }
    break;
    case 3://网关地址
    {
        if (FALSE == commfun_IP(pNewStr, tmp))
        {
            if (TRUE == MessageBox("网关地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (EthIPisSet < 0 || EthIP.ipGateway[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthIP.ipGateway[0], EthIP.ipGateway[1], EthIP.ipGateway[2], EthIP.ipGateway[3]);
                }
                InputBox(czBuf, IpConfigReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(EthIP.ipGateway, tmp, 4);
            EthIPisSet = 1;
        }
    }
    break;
    case 4:
    {
        EthIP.pppoeUser.nNum = strlen(pNewStr);
        strcpy(EthIP.pppoeUser.value, pNewStr);
        EthIPisSet = 1;
    }
    break;
    case 5:
    {
        EthIP.pppoePwd.nNum = strlen(pNewStr);
        strcpy(EthIP.pppoePwd.value, pNewStr);
        EthIPisSet = 1;
    }
    break;
    }

}

BOOL ip_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 7;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (EthIPIndex)
            {
            case 0://IP配置方式
            {
                EthIP.ipConfig = nSel;
                EthIPisSet = 1;
                if(0 == EthIP.ipConfig)
                {
                    memset(EthIP.ip, 0, 4);
                    memset(EthIP.mask, 0, 4);
                    memset(EthIP.ipGateway, 0, 4);
                }
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
    case KEY_LEFT:
    {
        if (EthIPIndex > 0)
            EthIPIndex--;
        else
            EthIPIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        if (EthIPIndex < maxindex)
            EthIPIndex++;
        else
            EthIPIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (EthIPIndex == 6)
        {
            CancelMenu();
            return TRUE;
        }
        if (ValidatePwd() == TRUE)
        {
            switch (EthIPIndex)
            {
            case 0: //ip配置方式
            {
                PopList(g_ipconfig, 3, g_ipconfig[0], 70, 40, FALSE);
                return TRUE;
            }
            case 1://IP地址
            {
                char czBuf[20];
                if (EthIPisSet < 0 || EthIP.ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthIP.ip[0], EthIP.ip[1], EthIP.ip[2], EthIP.ip[3]);
                }
                InputBox(czBuf, IpConfigReturn, NULL, 0x02);
                return TRUE;
            }
            case 2://子网掩码
            {
                char czBuf[20];
                if (EthIPisSet < 0 || EthIP.mask[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthIP.mask[0], EthIP.mask[1], EthIP.mask[2], EthIP.mask[3]);
                }
                InputBox(czBuf, IpConfigReturn, NULL, 0x02);
                return TRUE;
            }
            case 3://网关地址
            {
                char czBuf[20];
                if (EthIPisSet < 0 || EthIP.ipGateway[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthIP.ipGateway[0], EthIP.ipGateway[1], EthIP.ipGateway[2], EthIP.ipGateway[3]);
                }
                InputBox(czBuf, IpConfigReturn, NULL, 0x02);
                return TRUE;
            }
            case 4://用户名
            {
                char czBuf[20];
                if (EthIPisSet < 0 || EthIP.pppoeUser.nNum == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%s", EthIP.pppoeUser.value);
                }
                InputBox(czBuf, IpConfigReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                return TRUE;
            }
            case 5://密码
            {
                char czBuf[20];
                if (EthIPisSet < 0 || EthIP.pppoePwd.nNum == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%s", EthIP.pppoePwd.value);
                }
                InputBox(czBuf, IpConfigReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                return TRUE;
            }
            case 6://取消
            {
                CancelMenu();
                if (TRUE == MessageBox("当前配置没有保存\n\n是否确定保存配置？", MB_OKCANCEL))
                {
                    ret = write_normal_data((uint8*)&EthIP, sizeof(OOP_ETHIP_T), EthIPOad, 0, 0, 1);
                    if (ret != 0)
                    {
                        MessageBox("IP地址配置写入失败！");
                    }
                    else
                    {
                        if(0 != ipconfig_send_to_smiOS((char*)"FE0", EthIP.ipConfig, EthIP.ip, EthIP.mask, EthIP.ipGateway))
                        {
                            MessageBox("设置IP地址失败！");
                        }
                        uint8 flag = 0;
                        OOP_OAD_U oadnum;
                        oadnum.value = 0x451004FF;
                        write_normal_data(&flag, sizeof(uint8), oadnum, 0, 0, 1);
                    }
                }
                EthIPIndex = 0;
                return TRUE;
            }
            case 7://保存
            {
                ret = write_normal_data((uint8*)&EthIP, sizeof(OOP_ETHIP_T), EthIPOad, 0, 0, 1);
                if (ret != 0)
                {
                    MessageBox("IP地址配置写入失败！");
                }
                else
                {
                    MessageBox("IP地址配置写入成功！");
                    if(0 != ipconfig_send_to_smiOS((char*)"FE0", EthIP.ipConfig, EthIP.ip, EthIP.mask, EthIP.ipGateway))
                    {
                        MessageBox("设置IP地址失败！");
                    }
                    uint8 flag = 0;
                    OOP_OAD_U oadnum;
                    oadnum.value = 0x451004FF;
                    write_normal_data(&flag, sizeof(uint8), oadnum, 0, 0, 1);
                }
                CancelMenu();
                EthIPIndex = 0;
                return TRUE;
            }
            }
        }
    }
    }

    return FALSE;
}
//以太网通道查看
PLUGIN_INFO g_layer_ip_set = {
    0x11030012/*id*/,
    ip_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    ip_set_draw/*Draw*/,
    NULL/*Timer*/,
    ip_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

uint8 EthConfigIndex;
void eth_set_init(HDC hdc)
{
    memset(&EthConfig, 0x00, sizeof(OOP_ETHCONFIG_T));
    EthConfigisSet = read_db_noamal_data(g_hUdbCenter, EthConfigOad.value, 0, 0, (uint8*)&EthConfig, sizeof(OOP_ETHCONFIG_T));
    EthConfigIndex = 0;
}
void eth_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
    int index = 0;
    ITEM_NOT_SELECT;
    char czBuf[300];
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "工作模式");
    if (EthConfigisSet < 0 || EthConfig.workmode>2)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", g_work_mode[EthConfig.workmode]);
    }
    if (EthConfigIndex == index)
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
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "连接方式");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0 || EthConfig.contype>1)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%s", g_conn_type[EthConfig.contype]);
    }
    if (EthConfigIndex == index)
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
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "连接模式");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0 || EthConfig.conmode>1)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", g_conn_app_type[EthConfig.conmode]);
    }
    if (EthConfigIndex == index)
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
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "代理IP");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0 || EthConfig.proxyip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {

        sprintf(czBuf, "%d.%d.%d.%d", EthConfig.proxyip[0], EthConfig.proxyip[1], EthConfig.proxyip[2], EthConfig.proxyip[3]);
    }
    if (EthConfigIndex == index)
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
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "代理端口");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0 || EthConfig.proxyport == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", EthConfig.proxyport);
    }
    if (EthConfigIndex == index)
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
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "超时时间");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d 秒", EthConfig.dail.mask.dailtime);
    }
    if (EthConfigIndex == index)
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
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "重发次数");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", EthConfig.dail.mask.dailcnt);
    }
    if (EthConfigIndex == index)
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
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "心跳周期");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d 秒", EthConfig.heart);
    }
    if (EthConfigIndex == index)
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
    nTop -= 30;
    nLeft2 = 130;
    if (EthConfigIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "取消");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "取消");
    }

    index++;
    nTop += 25;

    if (EthConfigIndex == index)
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

void EthConfigReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[4] = { 0 };

    switch (EthConfigIndex)
    {
    case 3: //代理服务器地址
    {
        if (FALSE == commfun_IP(pNewStr, tmp))
        {
            if (TRUE == MessageBox("IP地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (EthConfigisSet < 0 || EthConfig.proxyip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthConfig.proxyip[0], EthConfig.proxyip[1], EthConfig.proxyip[2], EthConfig.proxyip[3]);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(EthConfig.proxyip, tmp, 4);
            EthConfigisSet = 1;
        }
    }
    break;
    case 4:     //代理端口
    {
        int port = atoi(pNewStr);
        if (port <= 65535)
        {
            EthConfig.proxyport = port;
            EthConfigisSet = 1;
        }
        else
        {
            if (TRUE == MessageBox("端口格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (EthConfigisSet < 0 || EthConfig.proxyport == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthConfig.proxyport);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
            }
        }
    }
    break;
    case 5:     //超时时间
    {
        int time = atoi(pNewStr);
        if (time <= 0x3f)      //超时时间占6位 最大0x3f
        {
            EthConfig.dail.mask.dailtime = time;
            EthConfigisSet = 1;
        }
        else
        {
            if (TRUE == MessageBox("超时时间最大63 \n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (EthConfigisSet < 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthConfig.dail.mask.dailtime);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
            }
        }
    }
    break;
    case 6:     //重发次数
    {
        int dailcnt = atoi(pNewStr);
        if (dailcnt <= 3)      //重发次数占2位 最大0x3
        {
            EthConfig.dail.mask.dailcnt = dailcnt;
            EthConfigisSet = 1;
        }
        else
        {
            if (TRUE == MessageBox("重发次数最大3 \n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (EthConfigisSet < 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthConfig.dail.mask.dailcnt);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
            }
        }
    }
    break;
    case 7:     //心跳周期
    {
        int heart = atoi(pNewStr);
        if (heart <= 65535)      //
        {
            EthConfig.heart = heart;
            EthConfigisSet = 1;
        }
        else
        {
            if (TRUE == MessageBox("心跳周期有误\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (EthConfigisSet < 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthConfig.heart);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
            }
        }
    }
    break;
    }
}

BOOL eth_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 9;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (EthConfigIndex)
            {
            case 0://工作模式
            {
                EthConfig.workmode = nSel;
                EthConfigisSet = 1;
            }
            break;
            case 1://连接方式
            {
                EthConfig.contype = nSel;
                EthConfigisSet = 1;
            }
            break;
            case 2://连接应用方式
            {
                EthConfig.conmode = nSel;
                EthConfigisSet = 1;
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
    case KEY_LEFT:
    {
        if (EthConfigIndex > 0)
            EthConfigIndex--;
        else
            EthConfigIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        if (EthConfigIndex < maxindex)
            EthConfigIndex++;
        else
            EthConfigIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (ValidatePwd() == TRUE)
        {
            switch (EthConfigIndex)
            {
            case 0: //工作模式
            {
                PopList(g_work_mode, 3, g_work_mode[0], 70, 20, FALSE);
                return TRUE;
            }
            case 1://连接方式
            {
                PopList(g_conn_type, 2, g_conn_type[0], 70, 40, FALSE);
                return TRUE;
            }
            case 2://连接应用方式
            {
                PopList(g_conn_app_type, 2, g_conn_app_type[0], 70, 60, FALSE);
                return TRUE;
            }
            case 3://代理服务器地址
            {
                char czBuf[20];
                if (EthConfigisSet < 0 || EthConfig.proxyip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {

                    sprintf(czBuf, "%d.%d.%d.%d", EthConfig.proxyip[0], EthConfig.proxyip[1], EthConfig.proxyip[2], EthConfig.proxyip[3]);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x02);
                return TRUE;
            }
            case 4://代理端口
            {
                char czBuf[20];
                if (EthConfigisSet < 0 || EthConfig.proxyport == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthConfig.proxyport);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
                return TRUE;
            }
            case 5://超时时间
            {
                char czBuf[20];
                if (EthConfigisSet < 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthConfig.dail.mask.dailtime);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
                return TRUE;
            }
            case 6://重发次数
            {
                char czBuf[20];
                if (EthConfigisSet < 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthConfig.dail.mask.dailcnt);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
                return TRUE;
            }
            case 7://心跳周期
            {
                char czBuf[20];
                if (EthConfigisSet < 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthConfig.heart);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
                return TRUE;
            }
            case 8://取消
            {
                if (TRUE == MessageBox("当前配置没有保存\n\n是否确定保存配置？", MB_OKCANCEL))
                {
                    ret = write_normal_data((uint8*)&EthConfig, sizeof(OOP_ETHCONFIG_T), EthConfigOad, 0, 0, 1);
                    if (ret != 0)
                    {
                        MessageBox("以太网通信配置写入失败！");
                    }

                }
                EthConfigIndex = 0;
                return TRUE;
            }
            case 9://保存
            {
                ret = write_normal_data((uint8*)&EthConfig, sizeof(OOP_ETHCONFIG_T), EthConfigOad, 0, 0, 1);
                if (ret != 0)
                {
                    MessageBox("以太网通信配置写入失败！");
                }
                else {
                    MessageBox("以太网通信配置写入成功！");
                }
                EthConfigIndex = 0;
                return TRUE;
            }
            }
        }
    }
    }
    return FALSE;
}

//以太网通信配置设置
PLUGIN_INFO g_layer_eth_set = {
    0x11030016/*id*/,
    eth_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    eth_set_draw/*Draw*/,
    NULL/*Timer*/,
    eth_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

uint8 EthMasterIndex;
void ethmaster_set_init(HDC hdc)
{
    read_db_noamal_data(g_hUdbCenter, EthMasterOad.value, 0, 0, (uint8*)&EthMaster, sizeof(OOP_MASTERPARAMS_T));
    EthMasterIndex = 0;
}

void ethmaster_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    uint8 index = 0;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "以太网主站参数设置");
    rc.top = 20;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    int nTop = rc.top + 30;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "IP地址");
    if (EthMaster.nNum < 1 || EthMaster.master[0].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[0].ip[0], EthMaster.master[0].ip[1], EthMaster.master[0].ip[2], EthMaster.master[0].ip[3]);
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
    if (EthMaster.nNum < 1 || EthMaster.master[0].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", EthMaster.master[0].port);
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
    TextOut(hdc, nLeft1, nTop, "备用地址");
    if (EthMaster.nNum < 2 || EthMaster.master[1].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[1].ip[0], EthMaster.master[1].ip[1], EthMaster.master[1].ip[2], EthMaster.master[1].ip[3]);
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
    TextOut(hdc, nLeft1, nTop, "备用端口");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthMaster.nNum < 2 || EthMaster.master[1].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", EthMaster.master[1].port);
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
                if (EthMaster.nNum < 1 || EthMaster.master[0].ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[0].ip[0], EthMaster.master[0].ip[1], EthMaster.master[0].ip[2], EthMaster.master[0].ip[3]);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(EthMaster.master[0].ip, tmp, 4);
            if (EthMaster.nNum < 1)
                EthMaster.nNum = 1;
        }
    }
    break;
    case 1:     //端口
    {
        int port = atoi(pNewStr);
        if (port <= 65535)
        {
            EthMaster.master[0].port = port;
            if (EthMaster.nNum < 1)
                EthMaster.nNum = 1;
        }
        else
        {
            if (TRUE == MessageBox("端口格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (EthMaster.nNum < 1 || EthMaster.master[0].port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {

                    sprintf(czBuf, "%d", EthMaster.master[0].port);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x01);
            }
        }
    }
    break;
    case 2: //备用IP地址
    {
        if (FALSE == commfun_IP(pNewStr, tmp))
        {
            if (TRUE == MessageBox("IP地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (EthMaster.nNum < 2 || EthMaster.master[1].ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[1].ip[0], EthMaster.master[1].ip[1], EthMaster.master[1].ip[2], EthMaster.master[1].ip[3]);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(EthMaster.master[1].ip, tmp, 4);
            if (EthMaster.nNum < 2)
                EthMaster.nNum = 2;
        }
    }
    break;
    case 3:     //端口
    {
        int port = atoi(pNewStr);
        if (port <= 65535)
        {
            EthMaster.master[1].port = port;
            if (EthMaster.nNum < 2)
                EthMaster.nNum = 2;
        }
        else
        {
            if (TRUE == MessageBox("端口格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (EthMaster.nNum < 1 || EthMaster.master[1].port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {

                    sprintf(czBuf, "%d", EthMaster.master[1].port);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x01);
            }
        }
    }
    break;
    }
}
BOOL ethmaster_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 5;

    switch (nKey)
    {
    case KEY_UP:
    case KEY_LEFT:
    {
        if (EthMasterIndex > 0)
            EthMasterIndex--;
        else
            EthMasterIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
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
                if (EthMaster.nNum < 1 || EthMaster.master[0].ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[0].ip[0], EthMaster.master[0].ip[1], EthMaster.master[0].ip[2], EthMaster.master[0].ip[3]);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x02);
                return TRUE;
            }
            case 1://端口
            {
                char czBuf[20];
                if (EthMaster.nNum < 1 || EthMaster.master[0].port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthMaster.master[0].port);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x01);
                return TRUE;
            }
            case 2://备用IP地址
            {
                char czBuf[20];
                if (EthMaster.nNum < 2 || EthMaster.master[1].ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[1].ip[0], EthMaster.master[1].ip[1], EthMaster.master[1].ip[2], EthMaster.master[1].ip[3]);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x02);
                return TRUE;
            }
            case 3://备用端口
            {
                char czBuf[20];
                if (EthMaster.nNum < 2 || EthMaster.master[1].port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthMaster.master[1].port);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x01);
                return TRUE;
            }
            case 4://取消
            {
                if (TRUE == MessageBox("当前配置没有保存\n\n是否确定保存配置？", MB_OKCANCEL))
                {
                    ret = write_normal_data((uint8*)&EthMaster, sizeof(OOP_MASTERPARAMS_T), EthMasterOad, 0, 0, 1);
                    if (ret != 0)
                    {
                        MessageBox("以太网主站参数写入失败！");
                    }
                }
                EthMasterIndex = 0;
                return TRUE;
            }
            case 5://保存
            {
                ret = write_normal_data((uint8*)&EthMaster, sizeof(OOP_MASTERPARAMS_T), EthMasterOad, 0, 0, 1);
                if (ret != 0)
                {
                    MessageBox("以太网主站参数写入失败！");
                }
                else
                {
                    MessageBox("以太网主站参数写入成功！");
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
//以太网主站参数设置
PLUGIN_INFO g_layer_ethmaster_set = {
    0x11030017/*id*/,
    ethmaster_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    ethmaster_set_draw/*Draw*/,
    NULL/*Timer*/,
    ethmaster_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};


#endif

#if DESC("GPRS参数查看",1)

OOP_MASTERPARAMS_T GPRSMaster;
OOP_GPRSCONFIG_T GprsConfig;

void gprsmaster_display_init(HDC hdc)
{
    GPRSMasterOad.value = 0x45000300;
    memset(&GPRSMaster, 0x00, sizeof(OOP_MASTERPARAMS_T));
    read_db_noamal_data(g_hUdbCenter, GPRSMasterOad.value, 0, 0, (uint8*)&GPRSMaster, sizeof(OOP_MASTERPARAMS_T));
}
uint8 GprsPageIndex;    //通信配置需要两页显示
int GprsConfigIsSet;

void gprs_display_init(HDC hdc)
{
    GPRSConfigOad.value = 0x45000200;
    GprsPageIndex = 0;
    memset(&GprsConfig, 0x00, sizeof(OOP_GPRSCONFIG_T));
    GprsConfigIsSet = read_db_noamal_data(g_hUdbCenter, GPRSConfigOad.value, 0, 0, (uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T));
    if(0 != GprsConfigIsSet)
    {
        gui_get_default_gprs(&GprsConfig);
        GprsConfigIsSet = 0;
    }
#ifdef AREA_FUJIAN
    if(TRUE == grps_fujian_apn(&GprsConfig))
    {
        /* 显示直接修改 */
        write_normal_data((uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T), GPRSConfigOad, 0, 0, 1);
    }
#endif
}

void gprsmaster_display_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "无线公网主站参数");
    rc.top = 20;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    int nTop = rc.top + 30;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "IP地址");
    if (GPRSMaster.nNum < 1 || GPRSMaster.master[0].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[0].ip[0], GPRSMaster.master[0].ip[1], GPRSMaster.master[0].ip[2], GPRSMaster.master[0].ip[3]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "端口");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (GPRSMaster.nNum < 1 || GPRSMaster.master[0].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", GPRSMaster.master[0].port);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "备用地址");
    if (GPRSMaster.nNum < 2 || GPRSMaster.master[1].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[1].ip[0], GPRSMaster.master[1].ip[1], GPRSMaster.master[1].ip[2], GPRSMaster.master[1].ip[3]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "备用端口");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (GPRSMaster.nNum < 2 || GPRSMaster.master[1].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", GPRSMaster.master[1].port);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

}

void gprs_display_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    char czBuf[300];
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    if (GprsPageIndex == 0)//第一页
    {
        TextOut(hdc, nLeft1, nTop, "工作模式");
        if (GprsConfigIsSet < 0 || GprsConfig.workmode>2)
        {
            sprintf(czBuf, "-");
        }
        else
        {
            sprintf(czBuf, "%s", g_work_mode[GprsConfig.workmode]);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "在线方式");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.linetype>1)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", g_line_type[GprsConfig.linetype]);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "连接方式");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.contype>1)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", g_conn_type[GprsConfig.contype]);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "连接模式");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.conmode>1)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", g_conn_app_type[GprsConfig.conmode]);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "APN");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.apn.apn.nNum == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", GprsConfig.apn.apn.value);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "用户名");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.apn.username.nNum == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", GprsConfig.apn.username.value);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "密码");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.apn.pwd.nNum == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", GprsConfig.apn.pwd.value);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        nLeft2 += 20;
        ITEM_SELECT
            TextOut(hdc, nLeft2, nTop, "下一页");
        ITEM_NOT_SELECT;
    }
    else    //第二页
    {
        TextOut(hdc, nLeft1, nTop, "代理地址");
        if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyip[0] == 0)
        {
            sprintf(czBuf, "-.-.-.-");
        }
        else
        {
            sprintf(czBuf, "%d.%d.%d.%d", GprsConfig.apn.proxyip[0], GprsConfig.apn.proxyip[1], GprsConfig.apn.proxyip[2], GprsConfig.apn.proxyip[3]);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "代理端口");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyport == 65535)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%d", GprsConfig.apn.proxyport);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "超时时间");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.dail.mask.dailtime == 0x3f)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%d 秒", GprsConfig.dail.mask.dailtime);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "重发次数");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%d", GprsConfig.dail.mask.dailcnt);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "心跳周期");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%d 秒", GprsConfig.heart);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        nLeft2 += 20;
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "上一页");
        ITEM_NOT_SELECT;
    }
}

BOOL gprs_display_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    if (nKey == KEY_OK)
    {
        if (GprsPageIndex == 0)
            GprsPageIndex = 1;
        else
            GprsPageIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    return FALSE;
}

//无线公网通信配置查看
PLUGIN_INFO g_layer_gprs_display = {
    0x11030008/*id*/,
    gprs_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprs_display_draw/*Draw*/,
    NULL/*Timer*/,
    gprs_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};


//无线公网1主站参数查看
PLUGIN_INFO g_layer_gprsmaster_display = {
    0x11030009/*id*/,
    gprsmaster_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprsmaster_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
void gprs2master_display_init(HDC hdc)
{
    GPRSMasterOad.value = 0x45010300;
    memset(&GPRSMaster, 0x00, sizeof(OOP_MASTERPARAMS_T));
    read_db_noamal_data(g_hUdbCenter, GPRSMasterOad.value, 0, 0, (uint8*)&GPRSMaster, sizeof(OOP_MASTERPARAMS_T));
}
//无线公网2主站参数查看
PLUGIN_INFO g_layer_gprs2master_display = {
    0x11030121/*id*/,
    gprs2master_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprsmaster_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("GPRS参数设置",1)


uint8 GprsMasterIndex;
void gprsmaster_set_init(HDC hdc)
{
    GPRSMasterOad.value = 0x45000300;
    memset(&GPRSMaster, 0x00, sizeof(OOP_MASTERPARAMS_T));
    read_db_noamal_data(g_hUdbCenter, GPRSMasterOad.value, 0, 0, (uint8*)&GPRSMaster, sizeof(OOP_MASTERPARAMS_T));
    GprsMasterIndex = 0;
}

void gprsmaster_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    uint8 index = 0;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "无线公网主站设置");
    rc.top = 20;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    int nTop = rc.top + 30;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "IP地址");
    if (GPRSMaster.nNum < 1 || GPRSMaster.master[0].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[0].ip[0], GPRSMaster.master[0].ip[1], GPRSMaster.master[0].ip[2], GPRSMaster.master[0].ip[3]);
    }
    if (GprsMasterIndex == index)
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
    if (GPRSMaster.nNum < 1 || GPRSMaster.master[0].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", GPRSMaster.master[0].port);
    }
    if (GprsMasterIndex == index)
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
    TextOut(hdc, nLeft1, nTop, "备用地址");
    if (GPRSMaster.nNum < 2 || GPRSMaster.master[1].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[1].ip[0], GPRSMaster.master[1].ip[1], GPRSMaster.master[1].ip[2], GPRSMaster.master[1].ip[3]);
    }
    if (GprsMasterIndex == index)
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
    TextOut(hdc, nLeft1, nTop, "备用端口");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (GPRSMaster.nNum < 2 || GPRSMaster.master[1].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", GPRSMaster.master[1].port);
    }
    if (GprsMasterIndex == index)
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

    if (GprsMasterIndex == index)
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
    if (GprsMasterIndex == index)
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
void GprsMasterReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[4] = { 0 };

    switch (GprsMasterIndex)
    {
    case 0: //IP地址
    {
        if (FALSE == commfun_IP(pNewStr, tmp))
        {
            if (TRUE == MessageBox("IP地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (GPRSMaster.nNum < 1 || GPRSMaster.master[0].ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[0].ip[0], GPRSMaster.master[0].ip[1], GPRSMaster.master[0].ip[2], GPRSMaster.master[0].ip[3]);
                }
                InputBox(czBuf, GprsMasterReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(GPRSMaster.master[0].ip, tmp, 4);
            if (GPRSMaster.nNum < 1)
                GPRSMaster.nNum = 1;
        }
    }
    break;
    case 1:     //端口
    {
        int port = atoi(pNewStr);
        if (port <= 65535)
        {
            GPRSMaster.master[0].port = port;
            if (GPRSMaster.nNum < 1)
                GPRSMaster.nNum = 1;
        }
        else
        {
            if (TRUE == MessageBox("端口格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (GPRSMaster.nNum < 1 || GPRSMaster.master[0].port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {

                    sprintf(czBuf, "%d", GPRSMaster.master[0].port);
                }
                InputBox(czBuf, GprsMasterReturn, NULL, 0x01);
            }
        }
    }
    break;
    case 2: //备用IP地址
    {
        if (FALSE == commfun_IP(pNewStr, tmp))
        {
            if (TRUE == MessageBox("IP地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (GPRSMaster.nNum < 2 || GPRSMaster.master[1].ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[1].ip[0], GPRSMaster.master[1].ip[1], GPRSMaster.master[1].ip[2], GPRSMaster.master[1].ip[3]);
                }
                InputBox(czBuf, GprsMasterReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(GPRSMaster.master[1].ip, tmp, 4);
            if (GPRSMaster.nNum < 2)
                GPRSMaster.nNum = 2;
        }
    }
    break;
    case 3:     //端口
    {
        int port = atoi(pNewStr);
        if (port <= 65535)
        {
            GPRSMaster.master[1].port = port;
            if (GPRSMaster.nNum < 2)
                GPRSMaster.nNum = 2;
        }
        else
        {
            if (TRUE == MessageBox("端口格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (GPRSMaster.nNum < 1 || GPRSMaster.master[1].port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {

                    sprintf(czBuf, "%d", GPRSMaster.master[1].port);
                }
                InputBox(czBuf, GprsMasterReturn, NULL, 0x01);
            }
        }
    }
    break;
    }
}
BOOL gprsmaster_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 5;

    switch (nKey)
    {
    case KEY_UP:
    case KEY_LEFT:
    {
        if (GprsMasterIndex > 0)
            GprsMasterIndex--;
        else
            GprsMasterIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        if (GprsMasterIndex < maxindex)
            GprsMasterIndex++;
        else
            GprsMasterIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (ValidatePwd() == TRUE)
        {
            switch (GprsMasterIndex)
            {
            case 0://IP地址
            {
                char czBuf[20];
                if (GPRSMaster.nNum < 1 || GPRSMaster.master[0].ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[0].ip[0], GPRSMaster.master[0].ip[1], GPRSMaster.master[0].ip[2], GPRSMaster.master[0].ip[3]);
                }
                InputBox(czBuf, GprsMasterReturn, NULL, 0x02);
                return TRUE;
            }
            case 1://端口
            {
                char czBuf[20];
                if (GPRSMaster.nNum < 1 || GPRSMaster.master[0].port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", GPRSMaster.master[0].port);
                }
                InputBox(czBuf, GprsMasterReturn, NULL, 0x01);
                return TRUE;
            }
            case 2://备用IP地址
            {
                char czBuf[20];
                if (GPRSMaster.nNum < 2 || GPRSMaster.master[1].ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[1].ip[0], GPRSMaster.master[1].ip[1], GPRSMaster.master[1].ip[2], GPRSMaster.master[1].ip[3]);
                }
                InputBox(czBuf, GprsMasterReturn, NULL, 0x02);
                return TRUE;
            }
            case 3://备用端口
            {
                char czBuf[20];
                if (GPRSMaster.nNum < 2 || GPRSMaster.master[1].port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", GPRSMaster.master[1].port);
                }
                InputBox(czBuf, GprsMasterReturn, NULL, 0x01);
                return TRUE;
            }
            case 4://取消
            {
                if (TRUE == MessageBox("当前配置没有保存\n\n是否确定保存配置？", MB_OKCANCEL))
                {
                    ret = write_normal_data((uint8*)&GPRSMaster, sizeof(OOP_MASTERPARAMS_T), GPRSMasterOad, 0, 0, 1);
                    if (ret != 0)
                    {
                        MessageBox("无线公网主站参数写入失败！");
                    }
                }
                GprsMasterIndex = 0;
                return TRUE;
            }
            case 5://保存
            {
                ret = write_normal_data((uint8*)&GPRSMaster, sizeof(OOP_MASTERPARAMS_T), GPRSMasterOad, 0, 0, 1);
                if (ret != 0)
                {
                    MessageBox("无线公网主站参数写入失败！");
                }
                else
                {
                    MessageBox("无线公网主站参数写入成功！");
                }
                GprsMasterIndex = 0;
                return TRUE;
            }
            }
        }
    }
    }

    return FALSE;
}
//以太网主站参数设置
PLUGIN_INFO g_layer_gprsmaster_set = {
    0x11030019/*id*/,
    gprsmaster_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprsmaster_set_draw/*Draw*/,
    NULL/*Timer*/,
    gprsmaster_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};
void gprs2master_set_init(HDC hdc)
{
    GPRSMasterOad.value = 0x45010300;
    memset(&GPRSMaster, 0x00, sizeof(OOP_MASTERPARAMS_T));
    read_db_noamal_data(g_hUdbCenter, GPRSMasterOad.value, 0, 0, (uint8*)&GPRSMaster, sizeof(OOP_MASTERPARAMS_T));
    GprsMasterIndex = 0;
}
PLUGIN_INFO g_layer_gprs2master_set = {
    0x1103002b/*id*/,
    gprs2master_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprsmaster_set_draw/*Draw*/,
    NULL/*Timer*/,
    gprsmaster_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};


uint8 GprsExpandPageIndex;    //多网络信息需要多页显示上下翻页
uint8 GprsnumnetPageIndex;   //左右翻页
OOP_APNGROUP_T GprsExpand;   //多网络配置结构体
OOP_NETCONFIG_T Gprsnumnet;  //单网络配置结构体
int GprsConfigHandle;        //读db数据句柄，0成功，其他失败
OOP_OAD_U GPRSExpandOad;     //存OAD

void gprs_numnet_init(HDC hdc)
{
    GPRSExpandOad.value = 0x45000e00;
    GprsExpandPageIndex = 0;
    GprsnumnetPageIndex = 0;
    memset(&GprsExpand, 0x00, sizeof(OOP_APNGROUP_T));
    GprsConfigHandle = read_db_noamal_data(g_hUdbCenter, GPRSExpandOad.value, 0, 0, (uint8*)&GprsExpand, sizeof(OOP_APNGROUP_T));
}

void gprs_numnet_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "第%d组网络参数",GprsnumnetPageIndex+1);
    rc.top = 20;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = rc.top+20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    memset(&Gprsnumnet, 0, sizeof(OOP_NETCONFIG_T));
    Gprsnumnet=GprsExpand.net[GprsnumnetPageIndex];
    if(GprsExpand.nNum!=0)
    {
        if (GprsExpandPageIndex == 0)  //第一页
        {   
            TextOut(hdc, nLeft1, nTop, "运营商");
            if (GprsConfigHandle < 0 || Gprsnumnet.factory==255)
            {
                sprintf(czBuf, "-");
            }
            else 
            {
                sprintf(czBuf, "%s", g_operator_seclct[Gprsnumnet.factory]);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);

            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "网络类型");
            memset(czBuf, 0x00, sizeof(czBuf));
            if (GprsConfigHandle < 0 || Gprsnumnet.network==255)
            {
                sprintf(czBuf, "-");
            }
            else 
            {

                sprintf(czBuf, "%s", g_net_mode[Gprsnumnet.network]);
            }

            TextOut(hdc, nLeft2, nTop, czBuf);

            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "APN");
            memset(czBuf, 0x00, sizeof(czBuf));
            if (GprsConfigHandle < 0 || Gprsnumnet.apn.apn.nNum == 0)
            {
                sprintf(czBuf, "-");
            }
            else
            {

                sprintf(czBuf, "%s", Gprsnumnet.apn.apn.value);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);

            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "用户名");
            memset(czBuf, 0x00, sizeof(czBuf));
            if (GprsConfigHandle < 0 || Gprsnumnet.apn.username.nNum == 0)
            {
                sprintf(czBuf, "-");
            }
            else
            {

                sprintf(czBuf, "%s", Gprsnumnet.apn.username.value);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);

            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "密码");
            memset(czBuf, 0x00, sizeof(czBuf));
            if (GprsConfigHandle < 0 || Gprsnumnet.apn.pwd.nNum == 0)
            {
                sprintf(czBuf, "-");
            }
            else
            {

                sprintf(czBuf, "%s", Gprsnumnet.apn.pwd.value);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
            
            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "代理地址");
            if (GprsConfigHandle < 0 || Gprsnumnet.apn.proxyip[0] == 0)
            {
                sprintf(czBuf, "-.-.-.-");
            }
            else
            {
                sprintf(czBuf, "%d.%d.%d.%d", Gprsnumnet.apn.proxyip[0], Gprsnumnet.apn.proxyip[1],Gprsnumnet.apn.proxyip[2], Gprsnumnet.apn.proxyip[3]);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);   
            
            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "代理端口");
            memset(czBuf, 0x00, sizeof(czBuf));
            if (GprsConfigHandle < 0 || Gprsnumnet.apn.proxyport == 65535)
            {
                sprintf(czBuf, "-");
            }
            else
            {
            
                sprintf(czBuf, "%d", Gprsnumnet.apn.proxyport);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
       }
        else                                            //第二页
       {
          
            TextOut(hdc, nLeft1, nTop, "鉴权方式");
            memset(czBuf, 0x00, sizeof(czBuf));
            if (GprsConfigHandle < 0 || Gprsnumnet.auth>4)
            {
                sprintf(czBuf, "-");
            }
            else
            {
            
                sprintf(czBuf, "%s ", g_auth_mode[Gprsnumnet.auth]);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
            
            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "IP地址");
            if (GprsConfigHandle < 0 || Gprsnumnet.master.master[0].ip[0] == 0)
            {
                sprintf(czBuf, "-.-.-.-");
            }
            else
            {
                sprintf(czBuf, "%d.%d.%d.%d", Gprsnumnet.master.master[0].ip[0], Gprsnumnet.master.master[0].ip[1], Gprsnumnet.master.master[0].ip[2], Gprsnumnet.master.master[0].ip[3]);
            }
            
            TextOut(hdc, nLeft2, nTop, czBuf);
            
            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "端口");
            memset(czBuf, 0x00, sizeof(czBuf));
            if (GprsConfigHandle < 0 || Gprsnumnet.master.master[0].port==0)
            {
                sprintf(czBuf, "-");
            }
            else
            {
            
                sprintf(czBuf, "%d ", Gprsnumnet.master.master[0].port);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
            
            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "备用地址");
            if (GprsConfigHandle < 0 || Gprsnumnet.master.master[1].ip[0] == 0)
            {
                sprintf(czBuf, "-.-.-.-");
            }
            else
            {
                sprintf(czBuf, "%d.%d.%d.%d", Gprsnumnet.master.master[1].ip[0], Gprsnumnet.master.master[1].ip[1], Gprsnumnet.master.master[1].ip[2], Gprsnumnet.master.master[1].ip[3]);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);

            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "备用端口");
            memset(czBuf, 0x00, sizeof(czBuf));
            if (GprsConfigHandle < 0|| Gprsnumnet.master.master[1].port == 65535)
            {
                sprintf(czBuf, "-");
            }
            else
            {

                sprintf(czBuf, "%d", Gprsnumnet.master.master[1].port);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
            
      }
   }
   else
   {
            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "未检查到多网络参数");
   }
}

BOOL gprs_numnet_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    switch (nKey)
        {
    case KEY_LEFT:
        if (GprsExpand.nNum!=0)
        {   
            if (GprsnumnetPageIndex == 0)
                {      
                GprsnumnetPageIndex =GprsExpand.nNum-1;
                GprsExpandPageIndex=0;
                }
            else 
                {
                GprsnumnetPageIndex--;
                GprsExpandPageIndex=0;
                }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        else
        {
            GprsnumnetPageIndex=0;
            GprsExpandPageIndex=0;
        }
        return TRUE;
        
    case  KEY_RIGHT:
        if (GprsExpand.nNum!=0)
        {
            if(GprsnumnetPageIndex == GprsExpand.nNum-1)
                {      
                GprsnumnetPageIndex = 0;
                GprsExpandPageIndex=0;
                }
            else 
                {
                GprsnumnetPageIndex++;
                GprsExpandPageIndex=0;
                }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        else
        {
            GprsnumnetPageIndex=0;
            GprsExpandPageIndex=0;
        }
        return TRUE;
        
    case  KEY_UP:
        if(GprsExpandPageIndex == 0)
            GprsExpandPageIndex = 1;
        else 
            GprsExpandPageIndex=0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case  KEY_DOWN:
        if(GprsExpandPageIndex == 0)
            GprsExpandPageIndex = 1;
        else 
            GprsExpandPageIndex=0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    
        }
        return FALSE;

}


//无线公网1多网络配置
PLUGIN_INFO g_layer_gprs_numnet = {
    0x1103000A/*id*/,
    gprs_numnet_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprs_numnet_draw/*Draw*/,
    NULL/*Timer*/,
    gprs_numnet_keymsg/*keymsg*/,
    NULL/*ipc*/
};

void gprs2_numnet_init(HDC hdc)
{
    GPRSExpandOad.value = 0x45010e00;
    GprsExpandPageIndex = 0;
    GprsnumnetPageIndex = 0;
    memset(&GprsExpand, 0x00, sizeof(OOP_APNGROUP_T));
    GprsConfigHandle = read_db_noamal_data(g_hUdbCenter, GPRSExpandOad.value, 0, 0, (uint8*)&GprsExpand, sizeof(OOP_APNGROUP_T));
}


//无线公网2多网络配置
PLUGIN_INFO g_layer_gprs2_numnet = {
    0x11030122/*id*/,
    gprs2_numnet_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprs_numnet_draw/*Draw*/,
    NULL/*Timer*/,
    gprs_numnet_keymsg/*keymsg*/,
    NULL/*ipc*/
};


void gprs2_display_init(HDC hdc)
{
    GPRSConfigOad.value = 0x45010200;
    GprsPageIndex = 0;
    memset(&GprsConfig, 0x00, sizeof(OOP_GPRSCONFIG_T));
    GprsConfigIsSet = read_db_noamal_data(g_hUdbCenter, GPRSConfigOad.value, 0, 0, (uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T));
}

//无线公网通信配置查看
PLUGIN_INFO g_layer_gprs2_display = {
    0x11030120/*id*/,
    gprs2_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprs_display_draw/*Draw*/,
    NULL/*Timer*/,
    gprs_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

uint8 GprsConfigIndex;
void gprs_set_init(HDC hdc)
{
    GPRSConfigOad.value = 0x45000200;
    GprsPageIndex = 0;
    GprsConfigIndex = 0;
    memset(&GprsConfig, 0x00, sizeof(OOP_GPRSCONFIG_T));
    GprsConfigIsSet = read_db_noamal_data(g_hUdbCenter, GPRSConfigOad.value, 0, 0, (uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T));
    if(0 != GprsConfigIsSet)
    {
        gui_get_default_gprs(&GprsConfig);
        GprsConfigIsSet = 0;
    }
#ifdef AREA_FUJIAN
    if(TRUE == grps_fujian_apn(&GprsConfig))
    {
        /* 显示直接修改 */
        write_normal_data((uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T), GPRSConfigOad, 0, 0, 1);
    }
#endif
}

void gprs_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    uint8 index = 0;
    ITEM_NOT_SELECT;
    char czBuf[300];
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    if (GprsPageIndex == 0)//第一页
    {

        TextOut(hdc, nLeft1, nTop, "工作模式");
        if (GprsConfigIsSet < 0 || GprsConfig.workmode>2)
        {
            sprintf(czBuf, "-");
        }
        else
        {
            sprintf(czBuf, "%s", g_work_mode[GprsConfig.workmode]);
        }
        if (GprsConfigIndex == index)
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
        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "在线方式");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.linetype>1)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", g_line_type[GprsConfig.linetype]);
        }
        if (GprsConfigIndex == index)
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

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "连接方式");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.contype>1)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", g_conn_type[GprsConfig.contype]);
        }
        if (GprsConfigIndex == index)
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

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "连接模式");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.conmode>1)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", g_conn_app_type[GprsConfig.conmode]);
        }
        if (GprsConfigIndex == index)
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

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "APN");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.apn.apn.nNum == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", GprsConfig.apn.apn.value);
        }
        if (GprsConfigIndex == index)
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

#ifdef AREA_FUJIAN
        grps_fujian_apn(&GprsConfig);
#endif

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "用户名");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.apn.username.nNum == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", GprsConfig.apn.username.value);
        }
        if (GprsConfigIndex == index)
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

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "密码");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.apn.pwd.nNum == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", GprsConfig.apn.pwd.value);
        }
        if (GprsConfigIndex == index)
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

        nTop += 15;
        nLeft2 += 20;
        if (GprsConfigIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, "下一页");
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, "下一页");
        }
        PopListDraw(hdc);
    }
    else    //第二页
    {
        TextOut(hdc, nLeft1, nTop, "代理地址");
        if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyip[0] == 0)
        {
            sprintf(czBuf, "-.-.-.-");
        }
        else
        {
            sprintf(czBuf, "%d.%d.%d.%d", GprsConfig.apn.proxyip[0], GprsConfig.apn.proxyip[1], GprsConfig.apn.proxyip[2], GprsConfig.apn.proxyip[3]);
        }
        if (GprsConfigIndex == index)
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

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "代理端口");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyport == 65535)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%d", GprsConfig.apn.proxyport);
        }
        if (GprsConfigIndex == index)
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

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "超时时间");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.dail.mask.dailtime == 0x3f)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%d 秒", GprsConfig.dail.mask.dailtime);
        }
        if (GprsConfigIndex == index)
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

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "重发次数");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%d", GprsConfig.dail.mask.dailcnt);
        }
        if (GprsConfigIndex == index)
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

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "心跳周期");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%d 秒", GprsConfig.heart);
        }
        if (GprsConfigIndex == index)
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

        nTop += 15;
        nLeft2 += 20;
        if (GprsConfigIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, "上一页");
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, "上一页");
        }

        index++;
        nTop += 25;

        if (GprsConfigIndex == index)
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
        if (GprsConfigIndex == index)
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
}

void GprsConfigReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[4] = { 0 };
    if (GprsPageIndex == 0)
    {
        switch (GprsConfigIndex)
        {
        case 4: //APN
        {
            GprsConfig.apn.apn.nNum = strlen(pNewStr);
            memset(GprsConfig.apn.apn.value, 0, sizeof(GprsConfig.apn.apn.value));
            memcpy(GprsConfig.apn.apn.value, pNewStr, GprsConfig.apn.apn.nNum);
            GprsConfigIsSet = 1;
        }
        break;
        case 5: //用户名
        {
            GprsConfig.apn.username.nNum = strlen(pNewStr);
            memset(GprsConfig.apn.username.value, 0, sizeof(GprsConfig.apn.username.value));
            memcpy(GprsConfig.apn.username.value, pNewStr, GprsConfig.apn.username.nNum);
            GprsConfigIsSet = 1;
        }
        break;
        case 6: //密码
        {
            GprsConfig.apn.pwd.nNum = strlen(pNewStr);
            memset(GprsConfig.apn.pwd.value, 0, sizeof(GprsConfig.apn.pwd.value));
            memcpy(GprsConfig.apn.pwd.value, pNewStr, GprsConfig.apn.pwd.nNum);
            GprsConfigIsSet = 1;
        }
        break;
        }
    }
    else
    {
        switch (GprsConfigIndex)
        {
        case 0: //代理服务器地址
        {
            if (FALSE == commfun_IP(pNewStr, tmp))
            {
                if (TRUE == MessageBox("IP地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyip[0] == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", GprsConfig.apn.proxyip[0], GprsConfig.apn.proxyip[1], GprsConfig.apn.proxyip[2], GprsConfig.apn.proxyip[3]);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x02);
                }
            }
            else
            {
                memcpy(GprsConfig.apn.proxyip, tmp, 4);
                GprsConfigIsSet = 1;
            }
        }
        break;
        case 1: //端口
        {
            int port = atoi(pNewStr);
            if (port <= 65535)
            {
                GprsConfig.apn.proxyport = port;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("端口格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyport == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.apn.proxyport);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 2: //超时时间
        {
            int time = atoi(pNewStr);
            if (time < 0x3f)
            {
                GprsConfig.dail.mask.dailtime = time;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("超时时间有误\n\n确认是否重新输入?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0 || GprsConfig.dail.mask.dailtime == 0x3f)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.dail.mask.dailtime);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 3: //重发次数
        {
            int cnt = atoi(pNewStr);
            if (cnt < 4)
            {
                GprsConfig.dail.mask.dailcnt = cnt;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("重发次数有误\n\n确认是否重新输入?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.dail.mask.dailcnt);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 4: //心跳周期
        {
            int heart = atoi(pNewStr);
            if (heart < 65535)
            {
                GprsConfig.heart = heart;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("心跳周期有误\n\n确认是否重新输入?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.heart);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;

        }
    }
}

BOOL gprs_set_keymsg_page1(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int maxindex = 7;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (GprsConfigIndex)
            {
            case 0://工作模式
            {
                GprsConfig.workmode = nSel;
                GprsConfigIsSet = 1;
            }
            break;
            case 1://在线方式
            {
                GprsConfig.linetype = nSel;
                GprsConfigIsSet = 1;
            }
            break;
            case 2://连接方式
            {
                GprsConfig.contype = nSel;
                GprsConfigIsSet = 1;
            }
            break;
            case 3://连接模式
            {
                GprsConfig.conmode = nSel;
                GprsConfigIsSet = 1;
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
    case KEY_LEFT:
    {
        if (GprsConfigIndex > 0)
            GprsConfigIndex--;
        else
            GprsConfigIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        if (GprsConfigIndex < maxindex)
            GprsConfigIndex++;
        else
            GprsConfigIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (GprsConfigIndex == 7)
        {
            GprsPageIndex = 1;
            GprsConfigIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        if (ValidatePwd() == TRUE)
        {
            switch (GprsConfigIndex)
            {
            case 0: //工作模式
            {
                PopList(g_work_mode, 3, g_work_mode[0], 70, 20, FALSE);
                return TRUE;
            }
            case 1: //工作模式
            {
                PopList(g_line_type, 2, g_line_type[0], 70, 35, FALSE);
                return TRUE;
            }
            case 2://连接方式
            {
                PopList(g_conn_type, 2, g_conn_type[0], 70, 50, FALSE);
                return TRUE;
            }
            case 3://连接应用方式
            {
                PopList(g_conn_app_type, 2, g_conn_app_type[0], 70, 65, FALSE);
                return TRUE;
            }
            case 4://APN
            {
                char czBuf[20];
                if (GprsConfigIsSet < 0 || GprsConfig.apn.apn.nNum == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {

                    sprintf(czBuf, "%s", GprsConfig.apn.apn.value);
                }
                InputBox(czBuf, GprsConfigReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                return TRUE;
            }
            case 5://用户名
            {
                char czBuf[20];
                if (GprsConfigIsSet < 0 || GprsConfig.apn.username.nNum == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {

                    sprintf(czBuf, "%s", GprsConfig.apn.username.value);
                }
                InputBox(czBuf, GprsConfigReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                return TRUE;
            }
            case 6://密码
            {
                char czBuf[20];
                if (GprsConfigIsSet < 0 || GprsConfig.apn.pwd.nNum == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%s", GprsConfig.apn.pwd.value);
                }
                InputBox(czBuf, GprsConfigReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                return TRUE;
            }

            }
        }
    }
    }
    return FALSE;
}
BOOL gprs_set_keymsg_page2(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 7;
    switch (nKey)
    {
    case KEY_UP:
    case KEY_LEFT:
    {
        if (GprsConfigIndex > 0)
            GprsConfigIndex--;
        else
            GprsConfigIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        if (GprsConfigIndex < maxindex)
            GprsConfigIndex++;
        else
            GprsConfigIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (GprsConfigIndex == 5)
        {
            GprsPageIndex = 0;
            GprsConfigIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        if (ValidatePwd() == TRUE)
        {
            switch (GprsConfigIndex)
            {
            case 0://代理地址
            {
                char czBuf[20];
                if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", GprsConfig.apn.proxyip[0], GprsConfig.apn.proxyip[1], GprsConfig.apn.proxyip[2], GprsConfig.apn.proxyip[3]);
                }
                InputBox(czBuf, GprsConfigReturn, NULL, 0x02);
                return TRUE;
            }
            case 1://代理端口
            {
                char czBuf[20];
                if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyport == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", GprsConfig.apn.proxyport);
                }
                InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                return TRUE;
            }
            case 2://超时时间
            {
                char czBuf[20];
                if (GprsConfigIsSet < 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", GprsConfig.dail.mask.dailtime);
                }
                InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                return TRUE;
            }
            case 3://重试次数
            {
                char czBuf[20];
                if (GprsConfigIsSet < 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", GprsConfig.dail.mask.dailcnt);
                }
                InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                return TRUE;
            }
            case 4://心跳
            {
                char czBuf[20];
                if (GprsConfigIsSet < 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", GprsConfig.heart);
                }
                InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                return TRUE;
            }

            case 6://取消
            {
                if (TRUE == MessageBox("当前配置没有保存\n\n是否确定保存配置？", MB_OKCANCEL))
                {
#ifdef AREA_FUJIAN
                    grps_fujian_apn(&GprsConfig);
#endif
                    ret = write_normal_data((uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T), GPRSConfigOad, 0, 0, 1);
                    if (ret != 0)
                    {
                        MessageBox("无线公网通信配置写入失败！");
                    }
                }
                GprsPageIndex = 0;
                GprsConfigIndex = 0;
                return TRUE;
            }
            case 7://保存
            {
#ifdef AREA_FUJIAN
                grps_fujian_apn(&GprsConfig);
#endif
                ret = write_normal_data((uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T), GPRSConfigOad, 0, 0, 1);
                if (ret != 0)
                {
                    MessageBox("无线公网通信配置写入失败！");
                }
                else
                {
                    MessageBox("无线公网主站参数写入成功！");
                }
                GprsPageIndex = 0;
                GprsConfigIndex = 0;
                return TRUE;
            }
            }
        }
    }

    }
    return FALSE;
}

BOOL gprs_set_keymsg(int nKey)
{
    if (GprsPageIndex == 0)
        return gprs_set_keymsg_page1(nKey);
    else
        return gprs_set_keymsg_page2(nKey);
}
//无线公网通信配置设置
PLUGIN_INFO g_layer_gprs_set = {
    0x11030018/*id*/,
    gprs_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprs_set_draw/*Draw*/,
    NULL/*Timer*/,
    gprs_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};
void gprs2_set_init(HDC hdc)
{
    GPRSConfigOad.value = 0x45010200;
    GprsPageIndex = 0;
    GprsConfigIndex = 0;
    memset(&GprsConfig, 0x00, sizeof(OOP_GPRSCONFIG_T));
    GprsConfigIsSet = read_db_noamal_data(g_hUdbCenter, GPRSConfigOad.value, 0, 0, (uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T));
}
//无线公网2通信配置设置
PLUGIN_INFO g_layer_gprs2_set = {
    0x1103002a/*id*/,
    gprs2_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprs_set_draw/*Draw*/,
    NULL/*Timer*/,
    gprs_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("电表档案||电能表参数查看",1)

//根据信息点号获取档案信息
int get_meterfile(uint16 infonum, OOP_METER_T* MeterFile)
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
}

//将端口OAD转换为名称
void get_portname(OOP_OAD_U port, char* name)
{
    switch (port.nObjID)
    {
    case 0xF209:
    {
        if (port.attID == 2)
        {
            sprintf(name, "载波-%d", port.nIndex);
            return;
        }
    }
    break;
    case 0xF201:
    {
        if (port.attID == 2)
        {
            sprintf(name, "485-%d", port.nIndex);
            return;
        }
    }
    break;
    case 0xF208:
    {
        sprintf(name, "交采端口");
        return;
    }
    case 0xF20A:
    {
        if (port.attID == 2)
        {
            sprintf(name, "脉冲-%d", port.nIndex);
            return;
        }
    }
    default:
        break;
    }
    sprintf(name, "-");

}

void meterfile_display_init(HDC hdc)
{
    g_guiComm.nCurPoint = 1;
    memset(&MeterFile, 0, sizeof(OOP_METER_T));
    get_meterfile(g_guiComm.nCurPoint, &MeterFile);
}
void meterfile_display_draw(HDC hdc)
{
    int flag = 0;  //标识档案是否存在
    char czBuf[300] = { 0 };

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    set_gui_infonum(g_guiComm.nCurPoint);

    //解决测量点号变，对应档案没有更新
    memset(&MeterFile, 0, sizeof(OOP_METER_T));
    get_meterfile(g_guiComm.nCurPoint, &MeterFile);

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "局编号：");
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);

    ITEM_SELECT;
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;
    
    if (MeterFile.nIndex != 0)
        flag = 1;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "表地址：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 Addr[6] = { 0 };
        memcpy(Addr, MeterFile.basic.tsa.add, 6);
        sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "波特率：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 index = MeterFile.basic.baud;
        if (index > 11)
            index = 11;
        sprintf(czBuf, "%s", g_baud[index]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "协议：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 prtl = MeterFile.basic.protocol;
        if (prtl > 4)
            prtl = 0;
        sprintf(czBuf, "%s", g_prtl[prtl]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "通道：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        get_portname(MeterFile.basic.port, czBuf);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
}
BOOL meterfile_display_keymsg(int nKey)
{
    int MAX=16;

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_guiComm.nCurPoint > 0)
                g_guiComm.nCurPoint--;
            if (g_guiComm.nCurPoint == 0)
                g_guiComm.nCurPoint = MAX;

            memset(&MeterFile, 0, sizeof(OOP_METER_T));
            get_meterfile(g_guiComm.nCurPoint, &MeterFile);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_guiComm.nCurPoint < MAX)
            {
                g_guiComm.nCurPoint++;
            }
            else
            {
                g_guiComm.nCurPoint = 1;
            }
            memset(&MeterFile, 0, sizeof(OOP_METER_T));
            get_meterfile(g_guiComm.nCurPoint, &MeterFile);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            char bzbuf[50];
            sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
            InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);

            return TRUE;
        }
    }
    return FALSE;
}
//电表档案查看
PLUGIN_INFO g_layer_meterfile_display = {
    0x11030013/*id*/,
    meterfile_display_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    meterfile_display_draw/*Draw*/,
    NULL/*Timer*/,
    meterfile_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif


#if DESC("普通采集方案查看",1)
MEASURE_MAIN    g_normal_plan_csd_data;
OOP_PLAN_NORMAL_T   g_guiNormalPlan;

//根据普通方案信息点号获取普通采集方案
int normal_plan_read(uint16 id,OOP_PLAN_NORMAL_T *PlanInfo)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60140200;
    NormalOad.infoNum = id;

    ret = db_read_nomal(g_hUdbCenter, &NormalOad, sizeof(OOP_PLAN_NORMAL_T), (uint8*)PlanInfo, &len);

    if(ret !=0 ||len!=sizeof(OOP_PLAN_NORMAL_T))
        return -1 ;
    return 0;
}

void csd2str(char *str, OOP_RECORD_COL_T recdCol)
{
    uint32 i, j;
    uint32 roadColNum;

    for (i = 0; i < recdCol.num; i++) //csd数组
    {
        if (recdCol.csd[i].choice == 0) //oad
        {
            sprintf(str + strlen(str), "*00%08X", recdCol.csd[i].oad.value);
        }
        else //road
        {
            roadColNum = recdCol.csd[i].road.oadCols.nNum;
            sprintf(str + strlen(str), "*%08X:", recdCol.csd[i].oad.value);
            sprintf(str + strlen(str), "%02X", roadColNum);
            for (j = 0; j < roadColNum; j++)
            {
                if (strlen(str)>800)
                    break;
                sprintf(str + strlen(str), "%08X,", recdCol.csd[i].road.oadCols.oad[j].value);
            }
            if (roadColNum != 0)
            {
                str[strlen(str) - 1] = '\0'; //去掉最后一个逗号
            }
        }
    }
}

void normal_plan_csd_display_init(HDC hdc)
{
}

void normal_plan_csd_display_draw(HDC hdc)
{
    int index = 0;
    int len;
    int nTop = 20;
    int nLeft = 3;
    char czBuf[1024] = { 0 };
    char tBuf[27] = {0};    //一行显示26个字符

    csd2str(czBuf, g_guiNormalPlan.recdCol);
    len = strlen(czBuf);
        
    //GUI_FMT_DEBUG("czBuf: %s\n", czBuf);
    while (index < len - 26)
    {
        memcpy(tBuf, &czBuf[index], 26);
        tBuf[26] = '\0';
        index += 26;
        TextOut(hdc, nLeft, nTop, tBuf);
        nLeft = 3;
        nTop += 10;
    }
    if (index < len)
    {
        memcpy(tBuf, &czBuf[index], len - index);
        tBuf[len - index] = '\0';
        TextOut(hdc, nLeft, nTop, tBuf);
    }
}

BOOL normal_plan_csd_display_keymsg(int nKey)
{
    return FALSE;
}

//csd设置详情查看按钮
PLUGIN_INFO g_layer_normal_plan_csd_display = {
    0x00/*id*/,
    normal_plan_csd_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    normal_plan_csd_display_draw/*Draw*/,
    NULL/*Timer*/,
    normal_plan_csd_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

//csd设置详情查看按钮
PLUGIN_INFO* g_normal_plan_csd_display_plugin[] =
{
    &g_layer_normal_plan_csd_display,
};

void normal_plan_display_init(HDC hdc)
{
    g_guiComm.nCurPoint = 1;
    memset(&g_guiNormalPlan, 0, sizeof(OOP_PLAN_NORMAL_T));
    normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);

    g_normal_plan_csd_data.ppDetail = NULL;
    g_normal_plan_csd_data.pCurPlugin = NULL;
    g_normal_plan_csd_data.nIndex = 0;
}

void normal_plan_display_draw_1st(HDC hdc)
{
    int flag = 0;  //标识档案是否存在
    char czBuf[300] = { 0 };
    uint8 index = 0;

    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;

    //解决测量点号变，对应方案没有更新
    memset(&g_guiNormalPlan, 0, sizeof(OOP_PLAN_NORMAL_T));
    normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);

    TextOut(hdc, nLeft1, nTop, "方案编号：");
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);

    if (g_normal_plan_csd_data.nIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    
    if (g_guiNormalPlan.planID != 0)
        flag = 1;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "存储深度：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d", g_guiNormalPlan.memDepth);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "采集类型：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        index = g_guiNormalPlan.recdRow.choice;
        if (index > 4)
            index = 0;
        sprintf(czBuf, "%s", g_normalPlanType[index]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    if (flag == 0 || (index != 0 && index != 2))
    {
        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "值与单位：");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (flag == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {  
            if (index == 1) //采集上第N次
            {
                sprintf(czBuf, "%d次", g_guiNormalPlan.recdRow.num);
            }
            else if (index == 3) //按时标间隔采集
            {
                sprintf(czBuf, "%d%s", g_guiNormalPlan.recdRow.ti.value, g_ti[g_guiNormalPlan.recdRow.ti.unit]);
            }
            else //补抄
            {
                sprintf(czBuf, "%d%s", g_guiNormalPlan.recdRow.rd.ti.value, g_ti[g_guiNormalPlan.recdRow.rd.ti.unit]);
            }
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "表计类型：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        index = g_guiNormalPlan.ms.choice;
        if (index > 7)
            index = 0;
        sprintf(czBuf, "%s", g_ms[index]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    if (flag == 0 || index == 2 || index == 4)
    {
        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "表计配置：");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (flag == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {
            int i;
            if (index == 2) //一组用户类型
            {
                for (i = 0; i <  g_guiNormalPlan.ms.mst.size; i++)
                    sprintf(czBuf + strlen(czBuf), "%d ", g_guiNormalPlan.ms.mst.type[i]);
            }
            else if (index == 4) //一组配置序号
            {
                for (i = 0; i <  g_guiNormalPlan.ms.msi.size; i++)
                    sprintf(czBuf + strlen(czBuf), "%d ", g_guiNormalPlan.ms.msi.idx[i]);
            }     
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "存储时标：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        index = g_guiNormalPlan.tmFlag;
        if (index > 7)
            index = 0;
        sprintf(czBuf, "%s", g_tmFlag[index]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "列选择符：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "查看详情");
    }
    if (g_normal_plan_csd_data.nIndex == 1)
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

void normal_plan_display_draw(HDC hdc)
{
    // 绘制查看CSD页面
    if (g_normal_plan_csd_data.pCurPlugin != NULL)
    {
        ASSERT(g_normal_plan_csd_data.pCurPlugin->pfnDraw != NULL);
        g_normal_plan_csd_data.pCurPlugin->pfnDraw(hdc);
        return;
    }

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);

    set_gui_infonum((uint16)g_guiComm.nCurPoint);

    normal_plan_display_draw_1st(hdc);
    
}

BOOL normal_plan_display_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;

    if (g_normal_plan_csd_data.pCurPlugin != NULL)
    {
        if (g_normal_plan_csd_data.pCurPlugin->pfnKeyMsg != NULL
            && TRUE == g_normal_plan_csd_data.pCurPlugin->pfnKeyMsg(nKey))
        {
            return TRUE;
        }

        switch (nKey)
        {
            case KEY_CANCEL:
                if (g_normal_plan_csd_data.pCurPlugin->pfnUninitLayer != NULL)
                {
                    g_normal_plan_csd_data.pCurPlugin->pfnUninitLayer();
                }
                g_normal_plan_csd_data.pCurPlugin = NULL;
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                break;
        }
        return TRUE;
    }

    switch (nKey)
    {  
        case KEY_UP:
        {
            g_normal_plan_csd_data.nIndex--;
            if (g_normal_plan_csd_data.nIndex < 0)
                g_normal_plan_csd_data.nIndex = 1;

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        {
            g_normal_plan_csd_data.nIndex++;
            if (g_normal_plan_csd_data.nIndex > 1)
                g_normal_plan_csd_data.nIndex = 0;

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_LEFT:
        {
            if (g_guiComm.nCurPoint > 0)
                g_guiComm.nCurPoint--;
            if (g_guiComm.nCurPoint == 0)
                g_guiComm.nCurPoint = MAX_PLAN_INFONUM;

            memset(&g_guiNormalPlan, 0, sizeof(OOP_PLAN_NORMAL_T));
            normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        {
            if (g_guiComm.nCurPoint < MAX_PLAN_INFONUM)
            {
                g_guiComm.nCurPoint++;
            }
            else
            {
                g_guiComm.nCurPoint = 1;
            }
            memset(&g_guiNormalPlan, 0, sizeof(OOP_PLAN_NORMAL_T));
            normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            if (g_normal_plan_csd_data.nIndex == 0)
            {
                char bzbuf[50];
                sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
                InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            }
            else
            {
                g_normal_plan_csd_data.ppDetail = g_normal_plan_csd_display_plugin;
                g_normal_plan_csd_data.nPluginCnt = sizeof(g_normal_plan_csd_display_plugin) / sizeof(g_normal_plan_csd_display_plugin[0]);
                g_normal_plan_csd_data.nCurPlugin = 0;
                g_normal_plan_csd_data.pCurPlugin = g_normal_plan_csd_data.ppDetail[g_normal_plan_csd_data.nCurPlugin];
                ASSERT(g_normal_plan_csd_data.pCurPlugin != NULL);

                if (g_normal_plan_csd_data.pCurPlugin->pfnInitLayer != NULL)
                {
                    g_normal_plan_csd_data.pCurPlugin->pfnInitLayer(g_normal_plan_csd_data.pCurPlugin->dwId);
                }
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    }
    return FALSE;
}

//普通方案查看
PLUGIN_INFO g_layer_normal_plan_display = {
    0x11030070/*id*/,
    normal_plan_display_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    normal_plan_display_draw/*Draw*/,
    NULL/*Timer*/,
    normal_plan_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("事件采集方案查看",1)
MEASURE_MAIN        g_normal_plan_road_data;
OOP_PLAN_EVENT_T    g_guiEventPlan;

//根据信息点号获取事件采集方案
int event_plan_read(uint16 id, OOP_PLAN_EVENT_T *PlanInfo)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60160200;
    NormalOad.infoNum = id;

    ret = db_read_nomal(g_hUdbCenter, &NormalOad, sizeof(OOP_PLAN_EVENT_T), (uint8*)PlanInfo, &len);

    if(ret !=0 ||len!=sizeof(OOP_PLAN_EVENT_T))
        return -1 ;
    return 0;
}

void road2str(char *str, OOP_EVENTSELECT_T recdCol)
{
    uint32 i, j;
    uint32 roadColNum;

    for (i = 0; i < recdCol.nNum; i++) //road数组
    {
        roadColNum = recdCol.cols[i].oadCols.nNum;
        sprintf(str + strlen(str), "*%08X:", recdCol.cols[i].oadMain.value);
        sprintf(str + strlen(str), "%02X", roadColNum);
        for (j = 0; j < roadColNum; j++)
        {
            if (strlen(str)>800)
                break;
            sprintf(str + strlen(str), "%08X,", recdCol.cols[i].oadCols.oad[j].value);
        }
        if (roadColNum != 0)
        {
            str[strlen(str) - 1] = '\0'; //去掉最后一个逗号
        }
    }
}

void event_plan_road_display_init(HDC hdc)
{
}

void event_plan_road_display_draw(HDC hdc)
{
    int index = 0;
    int len;
    int nTop = 20;
    int nLeft = 3;
    char czBuf[1024] = { 0 };
    char tBuf[27] = {0};    //一行显示26个字符

    road2str(czBuf, g_guiEventPlan.events);
    len = strlen(czBuf);
        
    //GUI_FMT_DEBUG("czBuf: %s\n", czBuf);
    while (index < len - 26)
    {
        memcpy(tBuf, &czBuf[index], 26);
        tBuf[26] = '\0';
        index += 26;
        TextOut(hdc, nLeft, nTop, tBuf);
        nLeft = 3;
        nTop += 10;
    }
    if (index < len)
    {
        memcpy(tBuf, &czBuf[index], len - index);
        tBuf[len - index] = '\0';
        TextOut(hdc, nLeft, nTop, tBuf);
    }
}


BOOL event_plan_road_display_keymsg(int nKey)
{
    return FALSE;
}

//csd设置详情查看按钮
PLUGIN_INFO g_layer_event_plan_road_display = {
    0x00/*id*/,
    event_plan_road_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    event_plan_road_display_draw/*Draw*/, 
    NULL/*Timer*/,
    event_plan_road_display_keymsg/*keymsg*/,  
    NULL/*ipc*/
};

//csd设置详情查看按钮
PLUGIN_INFO* g_event_plan_road_display_plugin[] =
{
    &g_layer_event_plan_road_display,
};

void event_plan_display_init(HDC hdc)
{
    g_guiComm.nCurPoint = 1;
    memset(&g_guiEventPlan, 0, sizeof(OOP_PLAN_EVENT_T));
    event_plan_read(g_guiComm.nCurPoint, &g_guiEventPlan);

    g_normal_plan_road_data.ppDetail = NULL;
    g_normal_plan_road_data.pCurPlugin = NULL;
    g_normal_plan_road_data.nIndex = 0;
}

void event_plan_display_draw_1st(HDC hdc)
{
    int flag = 0;  //标识档案是否存在
    char czBuf[300] = { 0 };
    uint8 index = 0;

    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;

    //解决测量点号变，对应方案没有更新
    memset(&g_guiEventPlan, 0, sizeof(OOP_PLAN_EVENT_T));
    event_plan_read(g_guiComm.nCurPoint, &g_guiEventPlan);

    TextOut(hdc, nLeft1, nTop, "方案编号：");
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);

    if (g_normal_plan_road_data.nIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    
    if (g_guiEventPlan.planID != 0)
        flag = 1;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "存储深度：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d", g_guiEventPlan.nDepth);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "采集类型：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        index = g_guiEventPlan.nType;
        if (index > 3)
            index = 0;
        sprintf(czBuf, "%s", g_eventPlanType[index]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "表计类型：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        index = g_guiEventPlan.ms.choice;
        if (index > 7)
            index = 0;
        sprintf(czBuf, "%s", g_ms[index]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    if (flag == 0 || index == 2 || index == 4)
    {
        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "表计配置：");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (flag == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {
            int i;
            if (index == 2) //一组用户类型
            {
                for (i = 0; i <  g_guiEventPlan.ms.mst.size; i++)
                    sprintf(czBuf + strlen(czBuf), "%d ", g_guiEventPlan.ms.mst.type[i]);
            }
            else if (index == 4) //一组配置序号
            {
                for (i = 0; i <  g_guiEventPlan.ms.msi.size; i++)
                    sprintf(czBuf + strlen(czBuf), "%d ", g_guiEventPlan.ms.msi.idx[i]);
            }     
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "上报标识");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        index = g_guiEventPlan.bReport;
        if (index > 1)
            index = 0;
        sprintf(czBuf, "%s", g_eventReportFlag[index]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "采集内容");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "查看详情");
    }
    if (g_normal_plan_road_data.nIndex == 1)
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

void event_plan_display_draw(HDC hdc)
{
    // 绘制查看ROAD页面
    if (g_normal_plan_road_data.pCurPlugin != NULL)
    {
        ASSERT(g_normal_plan_road_data.pCurPlugin->pfnDraw != NULL);
        g_normal_plan_road_data.pCurPlugin->pfnDraw(hdc);
        return;
    }

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);

    set_gui_infonum((uint16)g_guiComm.nCurPoint);

    event_plan_display_draw_1st(hdc);
    
}

BOOL event_plan_display_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;

    if (g_normal_plan_road_data.pCurPlugin != NULL)
    {
        if (g_normal_plan_road_data.pCurPlugin->pfnKeyMsg != NULL
            && TRUE == g_normal_plan_road_data.pCurPlugin->pfnKeyMsg(nKey))
        {
            return TRUE;
        }

        switch (nKey)
        {
            case KEY_CANCEL:
                if (g_normal_plan_road_data.pCurPlugin->pfnUninitLayer != NULL)
                {
                    g_normal_plan_road_data.pCurPlugin->pfnUninitLayer();
                }
                g_normal_plan_road_data.pCurPlugin = NULL;
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                break;
        }
        return TRUE;
    }

    switch (nKey)
    {  
        case KEY_UP:
        {
            g_normal_plan_road_data.nIndex--;
            if (g_normal_plan_road_data.nIndex < 0)
                g_normal_plan_road_data.nIndex = 1;

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        {
            g_normal_plan_road_data.nIndex++;
            if (g_normal_plan_road_data.nIndex > 1)
                g_normal_plan_road_data.nIndex = 0;

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_LEFT:
        {
            if (g_guiComm.nCurPoint > 0)
                g_guiComm.nCurPoint--;
            if (g_guiComm.nCurPoint == 0)
                g_guiComm.nCurPoint = MAX_PLAN_INFONUM;

            memset(&g_guiEventPlan, 0, sizeof(OOP_PLAN_EVENT_T));
            event_plan_read(g_guiComm.nCurPoint, &g_guiEventPlan);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        {
            if (g_guiComm.nCurPoint < MAX_PLAN_INFONUM)
            {
                g_guiComm.nCurPoint++;
            }
            else
            {
                g_guiComm.nCurPoint = 1;
            }
            memset(&g_guiEventPlan, 0, sizeof(OOP_PLAN_EVENT_T));
            event_plan_read(g_guiComm.nCurPoint, &g_guiEventPlan);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            if (g_normal_plan_road_data.nIndex == 0)
            {
                char bzbuf[50];
                sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
                InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            }
            else
            {
                g_normal_plan_road_data.ppDetail = g_event_plan_road_display_plugin;
                g_normal_plan_road_data.nPluginCnt = sizeof(g_event_plan_road_display_plugin) / sizeof(g_event_plan_road_display_plugin[0]);
                g_normal_plan_road_data.nCurPlugin = 0;
                g_normal_plan_road_data.pCurPlugin = g_normal_plan_road_data.ppDetail[g_normal_plan_road_data.nCurPlugin];
                ASSERT(g_normal_plan_road_data.pCurPlugin != NULL);

                if (g_normal_plan_road_data.pCurPlugin->pfnInitLayer != NULL)
                {
                    g_normal_plan_road_data.pCurPlugin->pfnInitLayer(g_normal_plan_road_data.pCurPlugin->dwId);
                }
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    }
    return FALSE;
}


//事件方案查看
PLUGIN_INFO g_layer_event_plan_display = {
    0x11030071/*id*/,
    event_plan_display_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    event_plan_display_draw/*Draw*/,
    NULL/*Timer*/,
    event_plan_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("上报方案查看",1)
OOP_PLAN_REPORT_T   g_guiReportPlan;

//根据信息点号获取事件采集方案
int report_plan_read(uint16 id, OOP_PLAN_REPORT_T *PlanInfo)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x601C0200;
    NormalOad.infoNum = id;

    ret = db_read_nomal(g_hUdbCenter, &NormalOad, sizeof(OOP_PLAN_REPORT_T), (uint8*)PlanInfo, &len);

    if(ret !=0 ||len!=sizeof(OOP_PLAN_REPORT_T))
        return -1 ;
    return 0;
}

void report_plan_display_init(HDC hdc)
{
    g_guiComm.nCurPoint = 1;
    memset(&g_guiReportPlan, 0, sizeof(OOP_PLAN_REPORT_T));
    report_plan_read(g_guiComm.nCurPoint, &g_guiReportPlan);

    g_normal_plan_csd_data.ppDetail = NULL;
    g_normal_plan_csd_data.pCurPlugin = NULL;
    g_normal_plan_csd_data.nIndex = 0;
}

void report_plan_display_draw_1st(HDC hdc)
{
    int flag = 0;  //标识档案是否存在
    char czBuf[300] = { 0 };
    uint8 index = 0;

    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;

    //解决测量点号变，对应方案没有更新
    memset(&g_guiReportPlan, 0, sizeof(OOP_PLAN_REPORT_T));
    report_plan_read(g_guiComm.nCurPoint, &g_guiReportPlan);

    TextOut(hdc, nLeft1, nTop, "方案编号：");
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);

    if (g_normal_plan_csd_data.nIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    
    if (g_guiReportPlan.planID != 0)
        flag = 1;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "上报通道：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {  
        sprintf(czBuf, "%08X", g_guiReportPlan.channel.oad[0].value);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "超时时间：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d %s", g_guiReportPlan.timeout.value, g_ti[g_guiReportPlan.timeout.unit]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "最大重试：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d", g_guiReportPlan.retry);
    }
    TextOut(hdc, nLeft2, nTop, czBuf); 


    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "上报类型：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        index = g_guiReportPlan.data.choice;
        if (index > 1)
            index = 0;
        sprintf(czBuf, "%s", g_reportPlanType[index]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "RSD：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        index = g_guiReportPlan.data.record.tRSD.choice;
        if (index > 10)
            index = 0;
        sprintf(czBuf, "%s", g_reportPlanRSD[index]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "RCSD：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "查看详情");
    }
    if (g_normal_plan_csd_data.nIndex == 1)
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

void report_plan_display_draw(HDC hdc)
{
    // 绘制查看CSD页面
    if (g_normal_plan_csd_data.pCurPlugin != NULL)
    {
        ASSERT(g_normal_plan_csd_data.pCurPlugin->pfnDraw != NULL);
        g_normal_plan_csd_data.pCurPlugin->pfnDraw(hdc);
        return;
    }

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);

    set_gui_infonum((uint16)g_guiComm.nCurPoint);

    report_plan_display_draw_1st(hdc);
    
}

BOOL report_plan_display_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;

    if (g_normal_plan_csd_data.pCurPlugin != NULL)
    {
        if (g_normal_plan_csd_data.pCurPlugin->pfnKeyMsg != NULL
            && TRUE == g_normal_plan_csd_data.pCurPlugin->pfnKeyMsg(nKey))
        {
            return TRUE;
        }

        switch (nKey)
        {
            case KEY_CANCEL:
                if (g_normal_plan_csd_data.pCurPlugin->pfnUninitLayer != NULL)
                {
                    g_normal_plan_csd_data.pCurPlugin->pfnUninitLayer();
                }
                g_normal_plan_csd_data.pCurPlugin = NULL;
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                break;
        }
        return TRUE;
    }

    switch (nKey)
    {  
        case KEY_UP:
        {
            g_normal_plan_csd_data.nIndex--;
            if (g_normal_plan_csd_data.nIndex < 0)
                g_normal_plan_csd_data.nIndex = 1;

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        {
            g_normal_plan_csd_data.nIndex++;
            if (g_normal_plan_csd_data.nIndex > 1)
                g_normal_plan_csd_data.nIndex = 0;

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_LEFT:
        {
            if (g_guiComm.nCurPoint > 0)
                g_guiComm.nCurPoint--;
            if (g_guiComm.nCurPoint == 0)
                g_guiComm.nCurPoint = MAX_PLAN_INFONUM;

            memset(&g_guiReportPlan, 0, sizeof(OOP_PLAN_REPORT_T));
            report_plan_read(g_guiComm.nCurPoint, &g_guiReportPlan);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        {
            if (g_guiComm.nCurPoint < MAX_PLAN_INFONUM)
            {
                g_guiComm.nCurPoint++;
            }
            else
            {
                g_guiComm.nCurPoint = 1;
            }
            memset(&g_guiReportPlan, 0, sizeof(OOP_PLAN_REPORT_T));
            report_plan_read(g_guiComm.nCurPoint, &g_guiReportPlan);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            if (g_normal_plan_csd_data.nIndex == 0)
            {
                char bzbuf[50];
                sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
                InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            }
            else
            {
                g_normal_plan_csd_data.ppDetail = g_normal_plan_csd_display_plugin;
                g_normal_plan_csd_data.nPluginCnt = sizeof(g_normal_plan_csd_display_plugin) / sizeof(g_normal_plan_csd_display_plugin[0]);
                g_normal_plan_csd_data.nCurPlugin = 0;
                g_normal_plan_csd_data.pCurPlugin = g_normal_plan_csd_data.ppDetail[g_normal_plan_csd_data.nCurPlugin];
                ASSERT(g_normal_plan_csd_data.pCurPlugin != NULL);

                if (g_normal_plan_csd_data.pCurPlugin->pfnInitLayer != NULL)
                {
                    g_normal_plan_csd_data.pCurPlugin->pfnInitLayer(g_normal_plan_csd_data.pCurPlugin->dwId);
                }
            }
            if (g_guiReportPlan.data.choice == 1) //记录型
            {
                g_guiNormalPlan.recdCol.num = g_guiReportPlan.data.record.tRCSD.nNum;
                memcpy(g_guiNormalPlan.recdCol.csd, g_guiReportPlan.data.record.tRCSD.cols, sizeof(OOP_CSD_T) * OOP_MAX_CSDS);
            }            
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    }
    return FALSE;
}

//上报方案查看
PLUGIN_INFO g_layer_report_plan_display = {
    0x11030072/*id*/,
    report_plan_display_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    report_plan_display_draw/*Draw*/,
    NULL/*Timer*/,
    report_plan_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("采集任务查看",1)
//根据任务id获取任务信息
int get_taskinfo(uint16 Taskid, OOP_TASK_T* TaskInfo)
{
    int   ret;
    uint32 len = 0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60120200;
    NormalOad.infoNum = Taskid;
    OOP_TASK_T task;
    memset(&task, 0, sizeof(OOP_TASK_T));
    ret = db_read_nomal(g_hUdbCenter, &NormalOad, sizeof(OOP_TASK_T), (uint8*)&task, &len);
    if (ret != 0 || len != sizeof(OOP_TASK_T))
        return -1;
    memcpy(TaskInfo, &task, sizeof(OOP_TASK_T));
    return 0;
}

uint16 TaskIndex;
OOP_TASK_T TaskInfo;
void task_display_init(HDC hdc)
{
    g_guiComm.nCurPoint = 1;
    memset(&TaskInfo, 0x00, sizeof(OOP_TASK_T));
    get_taskinfo(g_guiComm.nCurPoint, &TaskInfo);
}

//将OOP_DATETIME_S_T格式的时间转换为字符串
void ooptime_to_str(OOP_DATETIME_S_T time, char* buf)
{
    sprintf(buf, "%d/%d/%d %d:%d:%d", time.year, time.month, time.day, time.hour, time.minute, time.second);
}

void task_display_draw(HDC hdc)
{
    int flag = 0;  //标识任务是否存在
    char czBuf[300] = { 0 };

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "任务ID：");
    
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);
    ITEM_SELECT;
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;
    
    if (TaskInfo.id != 0)
        flag = 1;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "执行间隔：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d %s", TaskInfo.acqFreq.value, g_ti[TaskInfo.acqFreq.unit]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "方案类型：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        if (TaskInfo.planType > 0 && TaskInfo.planType < 6)
            sprintf(czBuf, "%s", g_plantype[TaskInfo.planType - 1]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "方案编号：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d", TaskInfo.planID);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "开始时间：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        ooptime_to_str(TaskInfo.startTime, czBuf);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "结束时间：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        ooptime_to_str(TaskInfo.endTime, czBuf);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "任务状态：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", TaskInfo.state == 1 ? "正常" : "停用");
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
}

BOOL task_display_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
        case KEY_LEFT:
        {
            if (g_guiComm.nCurPoint > 0)
                g_guiComm.nCurPoint--;
            if (g_guiComm.nCurPoint == 0)
                g_guiComm.nCurPoint = MAX_TASK_INFONUM;
            memset(&TaskInfo, 0x00, sizeof(OOP_TASK_T));
            get_taskinfo(g_guiComm.nCurPoint, &TaskInfo);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        {
            if (g_guiComm.nCurPoint < MAX_TASK_INFONUM)
            {
                g_guiComm.nCurPoint++;
            }
            else
            {
                g_guiComm.nCurPoint = 1;
            }
            memset(&TaskInfo, 0x00, sizeof(OOP_TASK_T));
            get_taskinfo(g_guiComm.nCurPoint, &TaskInfo);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
    case KEY_OK:
        {
            char bzbuf[50];
            sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
            InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);

            return TRUE;
        }
    }
    return FALSE;
}
//采集任务查看
PLUGIN_INFO g_layer_task_display = {
    0x11030004/*id*/,
    task_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    task_display_draw/*Draw*/,
    NULL/*Timer*/,
    task_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("HPLC参数查看", 1)

int HPLC_set_index;
uint8 HPLCChannel;      //HPLC频点F2090900
int ChannelIsSet;     //是否设置
BOOL recvbit;
BOOL Hchannel_bit;
/*属性11（台区识别启停标记）∷=unsigned
启动标志：
bit0：台区识别使能标志（0：禁止；1：允许）
bit1：台区改切使能标志（0：禁止；1：允许）
bit2：离网感知使能标志（0：禁止；1：允许）
bit3~bit7：备用。
*/
uint8 HPLCAcrossAreaFlag;  //F2090B00 台区识别
int AcrossAreaFlagIsSet;

//属性22（拒绝节点上报使能标志）∷= enum{禁止（0），使能（1）}
uint8 HPLCNodeRejectFlag;       //F2091600
int NodeRejectFlagIsSet;

//搜表状态 属性8 60020a00
uint8 SearchMeterStatus;
int SearchMeterFlagIsSet;


void HPLC_display_init(HDC hdc)
{
    AcrossAreaFlagIsSet = read_db_noamal_data(g_hUdbCenter, 0xF2090B00, 0, 0, (uint8*)&HPLCAcrossAreaFlag, 1);
    NodeRejectFlagIsSet = read_db_noamal_data(g_hUdbCenter, 0xF2091600, 0, 0, (uint8*)&HPLCNodeRejectFlag, 1);
    SearchMeterFlagIsSet = read_db_noamal_data(g_hUdbCenter, 0x60020a00, 0, 0, (uint8*)&SearchMeterStatus, 1);
    printf("AcrossAreaFlagIsSet %d NodeRejectFlagIsSet %d", AcrossAreaFlagIsSet, NodeRejectFlagIsSet);
}
void HPLC_display_draw(HDC hdc)
{
    char czBuf[300] = { 0 };

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 120;

    TextOut(hdc, nLeft1, nTop, "台区识别开关：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (AcrossAreaFlagIsSet < 0)
    {
        sprintf(czBuf, "关闭");
    }
    else
    {
        uint8 tmp = HPLCAcrossAreaFlag & 0x01;
        sprintf(czBuf, "%s", tmp == 0 ? "关闭" : "开启");
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "离网感知开关：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (AcrossAreaFlagIsSet < 0)
    {
        sprintf(czBuf, "关闭");
    }
    else
    {
        uint8 tmp = (HPLCAcrossAreaFlag >> 2) & 0x01;
        sprintf(czBuf, "%s", tmp == 0 ? "关闭" : "开启");
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "拒绝节点上报开关：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (NodeRejectFlagIsSet < 0)
    {
        sprintf(czBuf, "关闭");
    }
    else
    {
        sprintf(czBuf, "%s", HPLCNodeRejectFlag == 0 ? "关闭" : "开启");
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "搜表状态：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (SearchMeterFlagIsSet < 0)
    {
        sprintf(czBuf, "空闲");
    }
    else
    {
        sprintf(czBuf, "%s", SearchMeterStatus == 0 ? "空闲" : "搜表中");
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
}
//HPLC参数查看
PLUGIN_INFO g_layer_HPLC_display = {
    0x11030005/*id*/,
    HPLC_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    HPLC_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("终端地址设置", 1)
// 终端地址
TERMINAL_ADDR g_terminal_addr;
int conFigParaSetIndex = 0;

void address_set_init(HDC hdc)
{
    get_addr(LocalAddr);
}

void address_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
#ifdef AREA_SHANXI
    MessageBox("山西终端通信地址\n不支持设置!");
    CancelMenu();
    return;
#endif
#ifdef AREA_HENAN
    MessageBox("河南终端通信地址\n不支持设置!");
    CancelMenu();
    return;
#endif
#ifdef AREA_JIANGXI
    MessageBox("江西终端通信地址\n不支持设置!");
    CancelMenu();
    return;
#endif
#ifdef AREA_SICHUAN
    MessageBox("四川终端通信地址\n不支持设置!");
    CancelMenu();
    return;
#endif
#ifdef AREA_HUNAN
    MessageBox("湖南终端通信地址\n不支持设置!");
    CancelMenu();
    return;
#endif

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "终端通信地址");
    rc.top = 26;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, pg_guicomm->fontSmall);

    memset(czBuf, 0x00, sizeof(czBuf));
#ifdef AREA_ZHEJIANG
    sprintf(czBuf, "%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
#else
    sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
#endif
    rc.top += 30;
    rc.bottom = rc.top + 15;
    ITEM_SELECT;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    ITEM_NOT_SELECT;
}

void AddrReturn(char* pNewStr, void* pPara)
{
    int nLen = strlen(pNewStr);
    int ret;

#ifdef AREA_ZHEJIANG
    if (nLen > 8 && TRUE == MessageBox("OOP终端地址长度大于4字节\n\n确认是否重新输入?", MB_OKCANCEL))
    {
        char tmp[20] = { 0 };
        sprintf(tmp, "%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
        InputBox(tmp, AddrReturn, NULL, 0x01);
    }
    else
    {
        char tmp[20] = { 0 };
        strcpy(tmp, "00000000");
        strcpy(tmp + 8 - nLen, pNewStr);
        printf("输入的地址为 %s \n", tmp);
        commfun_ASC2BCD((char*)LocalAddr, tmp, 8, 0);
        printf("转换为bcd %02x%02x%02x%02x \n", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
        OOP_OCTETVAR16_T TmpLocalAddr;
        memset(&TmpLocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
        TmpLocalAddr.nNum = 4;
        memcpy(TmpLocalAddr.value, LocalAddr, 4);
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
#else
    if (nLen > 12 && TRUE == MessageBox("OOP终端地址长度大于6字节\n\n确认是否重新输入?", MB_OKCANCEL))
    {
        char tmp[20] = { 0 };
        sprintf(tmp, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
        InputBox(tmp, AddrReturn, NULL, 0x01);
    }
    else
    {
        char tmp[20] = { 0 };
        strcpy(tmp, "000000000000");
        strcpy(tmp + 12 - nLen, pNewStr);
        printf("输入的地址为 %s \n", tmp);
        commfun_ASC2BCD((char*)LocalAddr, tmp, 12, 0);
        printf("转换为bcd %02x%02x%02x%02x%02x%02x \n", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
        OOP_OCTETVAR16_T TmpLocalAddr;
        memset(&TmpLocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
        TmpLocalAddr.nNum = 6;
        memcpy(TmpLocalAddr.value, LocalAddr, 6);
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
#endif
}

void AddrSetReturn(char* pNewStr, void* pPara)
{
    int nLen = strlen(pNewStr);
    int ret;
    switch(conFigParaSetIndex)
    {
        case 1:
            {
            if (nLen > 4 && TRUE == MessageBox("行政区码长度大于2字节\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                char tmp[20] = { 0 };
                sprintf(tmp, "%02x%02x", g_terminal_addr.czCode[0], g_terminal_addr.czCode[1]);
                InputBox(tmp, AddrSetReturn, NULL, 0x01);
            }
            else
            {
                char tmp[20] = { 0 };
                strcpy(tmp, "0000");
                strcpy(tmp + 4 - nLen, pNewStr);
                printf("输入的地址为 %s \n", tmp);
                commfun_ASC2BCD((char*)g_terminal_addr.czCode, tmp, 4, 0);
                printf("转换为bcd %02x%02x", g_terminal_addr.czCode[0], g_terminal_addr.czCode[1]);
            }
            break;
            }
        case 2:
            {
            if (nLen > 6 && TRUE == MessageBox("终端长度大于3字节\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                char tmp[20] = { 0 };
                sprintf(tmp, "%02x%02x%02x", g_terminal_addr.czAddr[0], g_terminal_addr.czAddr[1], g_terminal_addr.czAddr[2]);
                InputBox(tmp, AddrSetReturn, NULL, 0x01);
            }
            else
            {
                char tmp[20] = { 0 };
                strcpy(tmp, "000000");
                strcpy(tmp + 6 - nLen, pNewStr);
                printf("输入的地址为 %s \n", tmp);
                commfun_ASC2BCD((char*)g_terminal_addr.czAddr, tmp, 6, 0);
                printf("转换为bcd %02x%02x%02x", g_terminal_addr.czAddr[0], g_terminal_addr.czAddr[1], g_terminal_addr.czAddr[2]);
            }
            break;
            }
        case 3:
            {
           if (nLen > 12 && TRUE == MessageBox("OOP终端地址长度大于6字节\n\n确认是否重新输入?", MB_OKCANCEL))
           {
               char tmp[20] = { 0 };
               sprintf(tmp, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
               InputBox(tmp, AddrSetReturn, NULL, 0x01);
           }
           else
           {
               char tmp[20] = { 0 };
               strcpy(tmp, "000000000000");
               strcpy(tmp + 12 - nLen, pNewStr);
               printf("输入的地址为 %s \n", tmp);
               commfun_ASC2BCD((char*)LocalAddr, tmp, 12, 0);
               printf("转换为bcd %02x%02x%02x%02x%02x%02x \n", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
               OOP_OCTETVAR16_T TmpLocalAddr;
               memset(&TmpLocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
               TmpLocalAddr.nNum = 6;
               memcpy(TmpLocalAddr.value, LocalAddr, 6);
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
    }
}

BOOL address_set_keymsg(int nKey)
{

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
        case KEY_OK:
        {
            if (ValidatePwd() == TRUE)
            {
                char tmp[20] = {0};
#ifdef AREA_ZHEJIANG
                sprintf(tmp, "%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
#else
                sprintf(tmp, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
#endif
                InputBox(tmp, AddrReturn, NULL, 0x01);
                return TRUE;
            }
            break;
        }
        default: break;
    }
    return FALSE;
}

//通信地址设置
PLUGIN_INFO g_layer_address_set = {
    0x11030011/*id*/,
    address_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    address_set_draw/*Draw*/,
    NULL/*Timer*/,
    address_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("终端地址设置，隐藏菜单", 1)

void address_set_draw1(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;

    char czBuf[300];
#ifdef AREA_CHONGQING
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nLeft = 10;
    int nTop = 20;
    sprintf(czBuf, "2-8 终端地址码");
    TextOut(hdc, nLeft, nTop, czBuf);
    
    nLeft+=30;
    nTop+=40;
    bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "OOP终端地址");
    TextOut(hdc, nLeft, nTop, czBuf);
    
    nTop+=20;
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
    TextOut(hdc, nLeft, nTop, czBuf);
#else
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;
    sprintf(czBuf, "终端通信地址");
    rc.top = 26;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, pg_guicomm->fontSmall);

    memset(czBuf, 0x00, sizeof(czBuf));
#ifdef AREA_ZHEJIANG
    sprintf(czBuf, "%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
#else
    sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
#endif
    rc.top += 30;
    rc.bottom = rc.top + 15;
    ITEM_SELECT;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    ITEM_NOT_SELECT;
#endif
}

#ifdef AREA_CHONGQING
//通信地址设置
PLUGIN_INFO g_layer_address_set1 = {
    0x13031011/*id*/,
    address_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    address_set_draw1/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
#else
//通信地址设置
PLUGIN_INFO g_layer_address_set1 = {
    0x13031011/*id*/,
    address_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    address_set_draw1/*Draw*/,
    NULL/*Timer*/,
    address_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif
#endif

#if DESC("电表档案设置", 1)


uint8 meterfile_set_index;
void meterfile_set_init(HDC hdc)
{
    g_guiComm.nCurPoint = 1;
    meterfile_set_index = 0;
    nCurPoint = 0;
}

void meterfile_set_draw(HDC hdc)
{
    int flag = 0;  //标识档案是否存在
    char czBuf[300] = { 0 };
    int index = 0;
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    if(nCurPoint != g_guiComm.nCurPoint)
    {
        memset(&MeterFile, 0, sizeof(OOP_METER_T));
        get_meterfile(g_guiComm.nCurPoint, &MeterFile);
        set_gui_infonum(g_guiComm.nCurPoint);
        nCurPoint = g_guiComm.nCurPoint;
    }

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "测量点号：");
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);
    if (meterfile_set_index == index)
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
    if (MeterFile.nIndex != 0)
        flag = 1;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "通信地址：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 Addr[6] = { 0 };
        memcpy(Addr, MeterFile.basic.tsa.add, 6);
        sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
    }
    if (meterfile_set_index == index)
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
    TextOut(hdc, nLeft1, nTop, "波特率：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 index = MeterFile.basic.baud;
        if (index > 11)
            index = 11;
        sprintf(czBuf, "%s", g_baud[index]);
    }
    if (meterfile_set_index == index)
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
    TextOut(hdc, nLeft1, nTop, "规约类型：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 prtl = MeterFile.basic.protocol;
        if (prtl > 4)
            prtl = 0;
        sprintf(czBuf, "%s", g_prtl[prtl]);
    }
    if (meterfile_set_index == index)
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
    TextOut(hdc, nLeft1, nTop, "端口：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        get_portname(MeterFile.basic.port, czBuf);
    }
    if (meterfile_set_index == index)
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

    if (meterfile_set_index == index)
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
    nLeft2 = 50;
    if (meterfile_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "删除");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "删除");
    }

    index++;
    int nLeft3 = 90;
    if (meterfile_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, "保存设置");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, "保存设置");
    }

    PopListDraw(hdc);
}

//设置电表档案地址
void MeterAddrReturn(char* pNewStr, void* pPara)
{
    int nLen = strlen(pNewStr);
    uint8 Addr[20] = { 0 };
    char tmp[20] = { 0 };
    if (nLen > 12 && TRUE == MessageBox("设置电表地址长度大于6字节\n\n确认是否重新输入?", MB_OKCANCEL))
    {

        memcpy(Addr, MeterFile.basic.tsa.add, 6);
        sprintf(tmp, "%02x%02x%02x%02x%02x%02x", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
        InputBox(tmp, MeterAddrReturn, NULL, 0x01);
    }
    else
    {

        strcpy(tmp, "000000000000");
        strcpy(tmp + 12 - nLen, pNewStr);
        printf("输入的地址为 %s \n", tmp);
        commfun_ASC2BCD((char*)Addr, tmp, 12, 0);
        printf("转换为bcd %02x%02x%02x%02x%02x%02x \n", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
        MeterFile.basic.tsa.len = 5;
        memcpy(MeterFile.basic.tsa.add, Addr, 6);
//      memcpy_rev(MeterFile.basic.tsa.add, Addr, 6);
        MeterFile.nIndex = g_guiComm.nCurPoint;
    }

}

OOP_OAD_U get_set_pipe(int index)
{
    OOP_OAD_U oad;
    memset(&oad, 0x00, sizeof(OOP_OAD_U));
    switch (index)
    {
    case 0:
        oad.value = 0xF2090201;
        break;
    case 1:
        oad.value = 0xF2090202;
        break;
    case 2:
        oad.value = 0xF2010201;
        break;
    case 3:
        oad.value = 0xF2010202;
        break;
    case 4:
        oad.value = 0xF2010203;
        break;
    case 5:
        oad.value = 0xF2010204;
        break;
    case 6:
        oad.value = 0xF2010205;
        break;
    case 7:
        oad.value = 0xF2010206;
        break;
    case 8:
        oad.value = 0xF2010207;
        break;
    case 9:
        oad.value = 0xF2010208;
        break;
    case 10:
        oad.value = 0xF2080208;
        break;
    default:
        break;
    }
    return oad;
}

//删除电表档案
void delete_meterfile(uint16 infonum)
{
    RECORD_OAD_T RecordDel;
    RecordDel.logicId = 0;
    RecordDel.infoNum = infonum;
    RecordDel.road.oadMain.value = 0;
    RecordDel.road.oadCols.nNum = 1;
    RecordDel.road.oadCols.oad[0] = MeterFileOad;
    CLEAR_DATA_T clear;
    clear.recordOAD = RecordDel;
    clear.bClassTag = FALSE;
    int ret;
    ret = db_clear_oad_data(g_hUdbCenter, &clear);
    if (ret == 0)
    {
        GUI_FMT_DEBUG("档案删除成功 infonum %d \n", infonum);
    }
    else
    {
        GUI_FMT_DEBUG("档案删除失败 infonum %d \n", infonum);
    }

}

BOOL meterfile_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 7;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (meterfile_set_index)
            {
            case 2://波特率
            {
                MeterFile.basic.baud = nSel;
            }
            break;
            case 3://规约类型
            {
                MeterFile.basic.protocol = nSel;
            }
            break;
            case 4://端口
            {
                MeterFile.basic.port = get_set_pipe(nSel);
            }
            break;
            default:
                return FALSE;
            }
            MeterFile.nIndex = g_guiComm.nCurPoint;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }


    switch (nKey)
    {
        case KEY_LEFT:
        {
            if (g_guiComm.nCurPoint > 0)
                g_guiComm.nCurPoint--;
            if (g_guiComm.nCurPoint == 0)
                g_guiComm.nCurPoint = MAX_METER_INFONUM;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        {
            if (g_guiComm.nCurPoint < MAX_METER_INFONUM)
            {
                g_guiComm.nCurPoint++;
            }
            else
            {
                g_guiComm.nCurPoint = 1;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_UP:
        {
            if (meterfile_set_index > 0)
                meterfile_set_index--;
            else
                meterfile_set_index = maxindex;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        {
            if (meterfile_set_index < maxindex)
                meterfile_set_index++;
            else
                meterfile_set_index = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            if (ValidatePwd() == TRUE)
            {
                if (meterfile_set_index == 0)
                {
                    char bzbuf[50];
                    sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
                    InputBox(bzbuf, CurPointReturn, NULL, 0x01);
        //          UpdateWindow(g_guiComm.hMainWnd, TRUE);

                    return TRUE;
                }
                
                //设置电表地址
                if (meterfile_set_index == 1)
                {
                    uint8 Addr[6] = { 0 };
                    char tmp[20] = { 0 };
                    memcpy(Addr, MeterFile.basic.tsa.add, 6);
                    sprintf(tmp, "%02x%02x%02x%02x%02x%02x", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
                    if (strcmp(tmp, "000000000000") == 0)
                        strcpy(tmp, "");
                    InputBox(tmp, MeterAddrReturn, NULL, 0x01);
                    return TRUE;
                }
                //设置波特率
                if (meterfile_set_index == 2)
                {
                    printf("设置波特率\n");
                    PopList(g_baud, 12, g_baud[0], 70, 40, FALSE);
                    return TRUE;
                }
                //规约类型
                if (meterfile_set_index == 3)
                {
                    printf("设置规约类型\n");
                    PopList(g_prtl, 5, g_prtl[0], 70, 40, FALSE);
                    return TRUE;
                }
                //端口
                if (meterfile_set_index == 4)
                {
                    printf("设置端口\n");
                    PopList(g_pipe, 11, g_pipe[0], 70, 40, FALSE);
                    return TRUE;
                }
                //取消
                if (meterfile_set_index == 5)
                {
                    if (TRUE == MessageBox("当前配置没有保存\n\n是否确定保存配置？", MB_OKCANCEL))
                    {
                        ret = write_normal_data((uint8*)&MeterFile, sizeof(OOP_METER_T), MeterFileOad, g_guiComm.nCurPoint, 0, 2);
                        if (ret == 0)
                        {
                            MessageBox("当前档案配置保存成功！");
                        }
                    }
                    meterfile_set_index = 0;
                    return FALSE;
                }
                //删除
                if (meterfile_set_index == 6)
                {
                    if (TRUE == MessageBox("确认删除当前电表档案？", MB_OKCANCEL))
                    {
                        delete_meterfile(g_guiComm.nCurPoint);
                        meterfile_set_init(0x11030003);
                        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                        return TRUE;
                    }
                    meterfile_set_index = 0;
                    return FALSE;
                }
                //保存配置
                if (meterfile_set_index == 7)
                {
                    ret = write_normal_data((uint8*)&MeterFile, sizeof(OOP_METER_T), MeterFileOad, g_guiComm.nCurPoint, 0, 2);
                    if (ret == 0)
                    {
                        MessageBox("当前档案配置保存成功！");
                        meterfile_set_index = 0;
                        return TRUE;
                    }
                    else
                    {
                        MessageBox("当前档案配置保存失败！");
                        return FALSE;
                    }

                }
            }
        }
    }
    return FALSE;
}

//电表档案设置
PLUGIN_INFO g_layer_meterfile_set = {
    0x11030003/*id*/,
    meterfile_set_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    meterfile_set_draw/*Draw*/,
    NULL/*Timer*/,
    meterfile_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif


#if DESC("普通采集方案MS设置",1)
uint8       g_normalPlanMsType;
OOP_MSP_T   g_normalPlanMsSet;
int8        g_normalPlanMsSetIndex = 0;
#define     NORMAL_PLAN_MS_REGION_START 0
#define     NORMAL_PLAN_MS_REGION_END               1

void normal_plan_ms_sprintf(char *str, uint8 type, uint8 state)
{
    switch (type)
    {
        case 2: //一组用户类型
            sprintf(str, "%d", g_normalPlanMsSet.mst.type[0]);
            break;
        case 3: //一组用户地址
            sprintf(str, "%02x%02x%02x%02x%02x%02x", g_normalPlanMsSet.msa.add[0].add[0], g_normalPlanMsSet.msa.add[0].add[1],
                        g_normalPlanMsSet.msa.add[0].add[2], g_normalPlanMsSet.msa.add[0].add[3],
                        g_normalPlanMsSet.msa.add[0].add[4], g_normalPlanMsSet.msa.add[0].add[5]);
            break;
        case 4: //一组配置序号
            sprintf(str, "%d", g_normalPlanMsSet.msi.idx[0]);
            break;
        case 5:
            if (state == NORMAL_PLAN_MS_REGION_START)
            {
                sprintf(str, "%d", g_normalPlanMsSet.msrt.region[0].start);
            }
            else if (state == NORMAL_PLAN_MS_REGION_END)
            {
                sprintf(str, "%d", g_normalPlanMsSet.msrt.region[0].end);
            }
            break;
        case 6:
            if (state == NORMAL_PLAN_MS_REGION_START)
            {
                sprintf(str, "%02x%02x%02x%02x%02x%02x", g_normalPlanMsSet.msra.region[0].start.add[0], g_normalPlanMsSet.msra.region[0].start.add[1],
                        g_normalPlanMsSet.msra.region[0].start.add[2], g_normalPlanMsSet.msra.region[0].start.add[3],
                        g_normalPlanMsSet.msra.region[0].start.add[4], g_normalPlanMsSet.msra.region[0].start.add[5]);
            }
            else if (state == NORMAL_PLAN_MS_REGION_END)
            {
                sprintf(str, "%02x%02x%02x%02x%02x%02x", g_normalPlanMsSet.msra.region[0].end.add[0], g_normalPlanMsSet.msra.region[0].end.add[1],
                        g_normalPlanMsSet.msra.region[0].end.add[2], g_normalPlanMsSet.msra.region[0].end.add[3],
                        g_normalPlanMsSet.msra.region[0].end.add[4], g_normalPlanMsSet.msra.region[0].end.add[5]);
            }
            break;
        case 7:
            if (state == NORMAL_PLAN_MS_REGION_START)
            {
                sprintf(str, "%d", g_normalPlanMsSet.msri.region[0].start);
            }
            else if (state == NORMAL_PLAN_MS_REGION_END)
            {
                sprintf(str, "%d", g_normalPlanMsSet.msri.region[0].end);
            }
            break;
    }
}

void normalPlanMsReturn(char* pNewStr, void* pPara)
{
    int value;
    char tmp[20] = { 0 };

    switch (g_normalPlanMsType)
    {
        case 2: case 5: //用户类型
        {
            value = atoi(pNewStr);
            if (value > 255 && MessageBox("用户类型大于255\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (g_normalPlanMsSetIndex == 0 || g_normalPlanMsSetIndex == 1)
                    normal_plan_ms_sprintf(tmp, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_START);
                else if (g_normalPlanMsSetIndex == 2)
                    normal_plan_ms_sprintf(tmp, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_END);
                InputBox(tmp, normalPlanMsReturn, NULL, 0x01);
            }
            else
            {
                if (g_normalPlanMsSetIndex == 0)
                    g_normalPlanMsSet.mst.type[0] = value;
                else if (g_normalPlanMsSetIndex == 1)
                    g_normalPlanMsSet.msrt.region[0].start = value;
                else if (g_normalPlanMsSetIndex == 2)
                    g_normalPlanMsSet.msrt.region[0].end = value;
            }
        }
        break;
        case 4: case 7: //配置序号
        {
            value = atoi(pNewStr);
            if (value > 65535 && MessageBox("配置序号大于65535\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (g_normalPlanMsSetIndex == 0 || g_normalPlanMsSetIndex == 1)
                    normal_plan_ms_sprintf(tmp, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_START);
                else if (g_normalPlanMsSetIndex == 2)
                    normal_plan_ms_sprintf(tmp, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_END);
                InputBox(tmp, normalPlanMsReturn, NULL, 0x01);
            }
            else
            {
                if (g_normalPlanMsSetIndex == 0)
                    g_normalPlanMsSet.msi.idx[0] = value;
                else if (g_normalPlanMsSetIndex == 1)
                    g_normalPlanMsSet.msri.region[0].start = value;
                else if (g_normalPlanMsSetIndex == 2)
                    g_normalPlanMsSet.msri.region[0].end = value;
            }
        }
        break;
        case 3: case 6: //用户地址
        {
            value = strlen(pNewStr);
            if (value != 12 && MessageBox("地址长度为12\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (g_normalPlanMsSetIndex == 0 || g_normalPlanMsSetIndex == 1)
                    normal_plan_ms_sprintf(tmp, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_START);
                else if (g_normalPlanMsSetIndex == 2)
                    normal_plan_ms_sprintf(tmp, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_END);
                InputBox(tmp, normalPlanMsReturn, NULL, 0x01);
            }
            else
            {
                int i;
                if (g_normalPlanMsSetIndex == 0)
                {
                    for (i = 0; i < 6; i++)
                        g_normalPlanMsSet.msa.add[0].add[i] = (pNewStr[2 * i] - '0') * 16 + (pNewStr[2 * i + 1] - '0');
                }
                else if (g_normalPlanMsSetIndex == 1)
                {
                    for (i = 0; i < 6; i++)
                        g_normalPlanMsSet.msra.region[0].start.add[i] = (pNewStr[2 * i] - '0') * 16 + (pNewStr[2 * i + 1] - '0');
                }
                else if (g_normalPlanMsSetIndex == 2)
                {
                    for (i = 0; i < 6; i++)
                        g_normalPlanMsSet.msra.region[0].end.add[i] = (pNewStr[2 * i] - '0') * 16 + (pNewStr[2 * i + 1] - '0');
                }
            }
        }
        break;
    }
}


void normal_plan_ms_set_init(HDC hdc)
{
    memset(&g_guiNormalPlan.ms, 0, sizeof(OOP_MSP_T));
    memset(&g_normalPlanMsSet, 0, sizeof(OOP_MSP_T));
    g_normalPlanMsSetIndex = 0;
}

void normal_plan_ms_set_group_draw(HDC hdc)
{
    int nTop    = 60;
    int nLeft1  = 20;
    int nLeft2  = 85;
    char bzbuf[50] = {0};
    int8 index = 0;

    switch (g_normalPlanMsType)
    {
        case 2: //用户类型
            TextOut(hdc, nLeft1, nTop, "用户类型：");
            break;
        case 3: //用户地址
            TextOut(hdc, nLeft1, nTop, "用户地址：");
            break;
        case 4: //配置序号
            TextOut(hdc, nLeft1, nTop, "配置序号：");
            break;
    }
    normal_plan_ms_sprintf(bzbuf, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_START);
    if (g_normalPlanMsSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, bzbuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, bzbuf);
    }

    index++;    
    nTop += 40;
    if (g_normalPlanMsSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, 40, nTop, "添加");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 40, nTop, "添加");
    }

    index++;
    if (g_normalPlanMsSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, 85, nTop, "删除");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 85, nTop, "删除");
    } 
}

void normal_plan_ms_set_region_draw(HDC hdc)
{
    int nTop    = 40;
    int nLeft1  = 15;
    int nLeft2  = 85;
    char bzbuf[50] = {0};
    int8 index = 0;

    TextOut(hdc, nLeft1, nTop, "区间类型：");
    int i = 0;
    if (g_normalPlanMsType == 5)
        i = g_normalPlanMsSet.msrt.region[0].nType;
    else if (g_normalPlanMsType == 6)
        i = g_normalPlanMsSet.msra.region[0].nType;
    else if (g_normalPlanMsType == 7)
        i = g_normalPlanMsSet.msri.region[0].nType;
    if (g_normalPlanMsSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_regionType[i]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_regionType[i]);
    }
    
    nTop += 25;
    TextOut(hdc, nLeft1, nTop, "区间起始值：");
    normal_plan_ms_sprintf(bzbuf, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_START);
    index++;
    if (g_normalPlanMsSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, bzbuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, bzbuf);
    }
    
    nTop += 25;
    TextOut(hdc, nLeft1, nTop, "区间结束值：");
    normal_plan_ms_sprintf(bzbuf, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_END);
    index++;
    if (g_normalPlanMsSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, bzbuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, bzbuf);
    }

    index++;    
    nTop += 25;
    if (g_normalPlanMsSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, 35, nTop, "添加");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 35, nTop, "添加");
    }

    index++;
    if (g_normalPlanMsSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, 90, nTop, "删除");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 90, nTop, "删除");
    } 
}


void normal_plan_ms_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);

    switch (g_normalPlanMsType)
    {
        case 2: case 3: case 4: //组
            normal_plan_ms_set_group_draw(hdc);
            break;
        case 5: case 6: case 7: //区间
            normal_plan_ms_set_region_draw(hdc);
            break;
    }
    PopListDraw(hdc);
}

void normal_plan_ms_set_group_key_ok()
{
    char bzbuf[50];

    switch (g_normalPlanMsSetIndex)
    {
        case 0:
            normal_plan_ms_sprintf(bzbuf, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_START);
            InputBox(bzbuf, normalPlanMsReturn, NULL, 0x01); 
            break;
        case 1: //添加
            if (TRUE == MessageBox("确定要添加？", MB_OKCANCEL))
            {
                int index;
                g_guiNormalPlan.ms.choice = g_normalPlanMsType;
                switch (g_normalPlanMsType)
                {
                    case 2:
                        index = g_guiNormalPlan.ms.mst.size;
                        g_guiNormalPlan.ms.mst.type[index] = g_normalPlanMsSet.mst.type[0];
                        g_guiNormalPlan.ms.mst.size++;
                        break;
                        
                    case 3:
                        index = g_guiNormalPlan.ms.msa.size;
                        g_guiNormalPlan.ms.msa.add[index].len = 5;
                        memcpy(&g_guiNormalPlan.ms.msa.add[index].add, g_normalPlanMsSet.msa.add[0].add, 6);
                        g_guiNormalPlan.ms.msa.size++;
                        break;

                    case 4:
                        index = g_guiNormalPlan.ms.msi.size;
                        g_guiNormalPlan.ms.msi.idx[index] = g_normalPlanMsSet.msi.idx[0];
                        g_guiNormalPlan.ms.msi.size++;
                        break;
                }
            }
            break;
        case 2: //删除
            if (TRUE == MessageBox("确定要删除？", MB_OKCANCEL))
            {
                
                memset(&g_guiNormalPlan.ms, 0, sizeof(OOP_MSP_T));
            }
            break;
    }
     

}

void normal_plan_ms_set_region_key_ok()
{
    char bzbuf[50];

    switch (g_normalPlanMsSetIndex)
    {
        case 0: //区间类型
            PopList(g_regionType, 4, g_regionType[0], 70, 40, FALSE);
            break;
        case 1:
            normal_plan_ms_sprintf(bzbuf, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_START);
            InputBox(bzbuf, normalPlanMsReturn, NULL, 0x01); 
            break;
        case 2:
            normal_plan_ms_sprintf(bzbuf, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_END);
            InputBox(bzbuf, normalPlanMsReturn, NULL, 0x01); 
        case 3: //添加
            if (TRUE == MessageBox("确定要添加？", MB_OKCANCEL))
            {
                int index;
                g_guiNormalPlan.ms.choice = g_normalPlanMsType;
                switch (g_normalPlanMsType)
                {
                    case 5:
                        index = g_guiNormalPlan.ms.msrt.size;
                        g_guiNormalPlan.ms.msrt.region[index].nType = g_normalPlanMsSet.msrt.region[0].nType;
                        g_guiNormalPlan.ms.msrt.region[index].start = g_normalPlanMsSet.msrt.region[0].start;
                        g_guiNormalPlan.ms.msrt.region[index].end = g_normalPlanMsSet.msrt.region[0].end;
                        g_guiNormalPlan.ms.msrt.size++;
                        break;
                        
                    case 6:
                        index = g_guiNormalPlan.ms.msra.size;
                        g_guiNormalPlan.ms.msra.region[index].nType = g_normalPlanMsSet.msra.region[0].nType;
                        g_guiNormalPlan.ms.msra.region[index].start.len = 5;
                        memcpy(&g_guiNormalPlan.ms.msra.region[index].start.add, g_normalPlanMsSet.msra.region[0].start.add, 6);
                        g_guiNormalPlan.ms.msra.region[index].end.len = 5;
                        memcpy(&g_guiNormalPlan.ms.msra.region[index].end.add, g_normalPlanMsSet.msra.region[0].end.add, 6);
                        g_guiNormalPlan.ms.msra.size++;
                        break;

                    case 7:
                        index = g_guiNormalPlan.ms.msri.size;
                        g_guiNormalPlan.ms.msri.region[index].nType = g_normalPlanMsSet.msri.region[0].nType;
                        g_guiNormalPlan.ms.msri.region[index].start = g_normalPlanMsSet.msri.region[0].start;
                        g_guiNormalPlan.ms.msri.region[index].end = g_normalPlanMsSet.msri.region[0].end;
                        g_guiNormalPlan.ms.msri.size++;
                        break;
                }
            }
            break;
        case 4: //删除
            if (TRUE == MessageBox("确定要删除？", MB_OKCANCEL))
            {
                
                memset(&g_guiNormalPlan.ms, 0, sizeof(OOP_MSP_T));
            }
            break;
    }
}

BOOL normal_plan_ms_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int maxindex = 0;

    switch (g_normalPlanMsType)
    {
        case 2: case 3: case 4:
            maxindex = 2;
            break;
        case 5: case 6: case 7:
            maxindex = 4;
            break;
    }

    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (g_normalPlanMsSetIndex)
            {
                case 0: //区间类型
                {
                    if (g_normalPlanMsType == 5)
                        g_normalPlanMsSet.msrt.region[0].nType = nSel;
                    else if (g_normalPlanMsType == 6)
                        g_normalPlanMsSet.msra.region[0].nType = nSel;
                    else if (g_normalPlanMsType == 7)
                        g_normalPlanMsSet.msri.region[0].nType = nSel;
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
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_normalPlanMsSetIndex > 0)
                g_normalPlanMsSetIndex--;
            else
                g_normalPlanMsSetIndex = maxindex;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_normalPlanMsSetIndex < maxindex)
                g_normalPlanMsSetIndex++;
            else
                g_normalPlanMsSetIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (g_normalPlanMsType)
            {
                case 2: case 3: case 4:
                    normal_plan_ms_set_group_key_ok();
                    break;
                case 5: case 6: case 7:
                    normal_plan_ms_set_region_key_ok();
                    break;
            }
            return TRUE;
        }
    }
    return FALSE;
}

//MS数据设置
PLUGIN_INFO g_layer_normal_plan_ms_set = {
    0x11/*id*/,
    normal_plan_ms_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    normal_plan_ms_set_draw/*Draw*/,
    NULL/*Timer*/,
    normal_plan_ms_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

//MS数据设置
PLUGIN_INFO* g_normal_plan_ms_set_plugin[] =
{
    &g_layer_normal_plan_ms_set,
};

#endif

#if DESC("普通采集方案CSD设置",1)
OOP_CSD_T       g_normalPlanCSD;
uint8           g_csd_oads_set_num;

//OAD的十六进制字符串转化为整数
int OAD2i(char *str)
{
    int radix = 16;
    int value = 0;

    while(*str)
    {
        if(*str >= '0' && *str <= '9')
            value = value * radix + *str - '0';    //0到9的字符串
        else if(*str >= 'A' && *str <= 'F')
            value = value * radix + *str - 'A' + 10;   //大写十六进制的ABCDEF的字符串的情况
        else if(*str >= 'a' && *str <= 'f')
            value = value * radix + *str - 'a' + 10;  // 小写十六进制的abcdef的字符串的情况

        str++;
    }
    return value;
}

void normalPlanCsdReturn(char* pNewStr, void* pPara)
{
    int nLen = strlen(pNewStr);
    char tmp[20] = { 0 };
    if (nLen != 8 && TRUE == MessageBox("OAD的长度为8\n\n确认是否重新输入?", MB_OKCANCEL))
    {
        switch (g_normal_plan_csd_data.nIndex)
        {
            case 0:
                sprintf(tmp, "%08X", g_normalPlanCSD.oad.value);
                break;
            case 2:
                sprintf(tmp, "%08X", g_normalPlanCSD.road.oadMain.value);
                break;
            case 3:
                sprintf(tmp, "%08X", g_normalPlanCSD.road.oadCols.oad[0].value);
                break;
        }               
        
        InputBox(tmp, normalPlanCsdReturn, NULL, 0x01);
    }
    else
    {
        int value = OAD2i(pNewStr);
        //GUI_FMT_DEBUG("normalPlanCsdReturn %d\n", value);
        switch (g_normal_plan_csd_data.nIndex)
        {
            case 0:
                g_normalPlanCSD.oad.value = value;
                break;
            case 2:
                g_normalPlanCSD.road.oadMain.value = value;
                break;
            case 3:
                g_normalPlanCSD.road.oadCols.oad[0].value = value;
                break;
        }  
    }
}

void normal_plan_csd_set_init(HDC hdc)
{
    memset(&g_guiNormalPlan.recdCol, 0, sizeof(OOP_RECORD_COL_T));
    memset(&g_normalPlanCSD, 0, sizeof(OOP_CSD_T));
    g_csd_oads_set_num = 0;

    g_normal_plan_csd_data.ppDetail = NULL;
    g_normal_plan_csd_data.pCurPlugin = NULL;
    g_normal_plan_csd_data.nIndex = 0;
}

void normal_plan_csd_set_draw(HDC hdc)
{
    // 绘制查看CSD页面
    if (g_normal_plan_csd_data.pCurPlugin != NULL)
    {
        ASSERT(g_normal_plan_csd_data.pCurPlugin->pfnDraw != NULL);
        g_normal_plan_csd_data.pCurPlugin->pfnDraw(hdc);
        return;
    }

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop    = 20;
    int nLeft1  = 10;
    int nLeft2  = 25;
    int nLeft3  = 65;
    int index = 0;
    char bzbuf[50] = {0};

    TextOut(hdc, nLeft1, nTop, "普通型：");
    nTop += 20;
    TextOut(hdc, nLeft2, nTop, "OAD");
    sprintf(bzbuf, "%08X", g_normalPlanCSD.oad.value);
    if (g_normal_plan_csd_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, bzbuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, bzbuf);
    }

    index++;
    if (g_normal_plan_csd_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3 + 60, nTop, "添加");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3 + 60, nTop, "添加");
    }

    index++;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "记录型：");
    nTop += 20;
    TextOut(hdc, nLeft2, nTop, "主OAD");
    sprintf(bzbuf, "%08X", g_normalPlanCSD.road.oadMain.value);
    if (g_normal_plan_csd_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, bzbuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, bzbuf);
    }

    index++;
    nTop += 20;
    TextOut(hdc, nLeft2, nTop, "子OAD");
    sprintf(bzbuf, "%08X", g_normalPlanCSD.road.oadCols.oad[0].value);
    if (g_normal_plan_csd_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, bzbuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, bzbuf);
    }

    index++;
    if (g_normal_plan_csd_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3 + 60, nTop - 10, "添加");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3 + 60, nTop - 10, "添加");
    }

    index++;
    nTop += 20;
    nLeft1 = 40;
    nLeft2 = 100;
    if (g_normal_plan_csd_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "查看");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "查看");
    }

    index++;
    if (g_normal_plan_csd_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "删除");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "删除");
    }
}

BOOL normal_plan_csd_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int maxindex = 6;

    if (g_normal_plan_csd_data.pCurPlugin != NULL)
    {
        if (g_normal_plan_csd_data.pCurPlugin->pfnKeyMsg != NULL
            && TRUE == g_normal_plan_csd_data.pCurPlugin->pfnKeyMsg(nKey))
        {
            return TRUE;
        }

        switch (nKey)
        {
            case KEY_CANCEL:
                if (g_normal_plan_csd_data.pCurPlugin->pfnUninitLayer != NULL)
                {
                    g_normal_plan_csd_data.pCurPlugin->pfnUninitLayer();
                }
                g_normal_plan_csd_data.pCurPlugin = NULL;
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                break;
        }
        return TRUE;
    }
    
    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_normal_plan_csd_data.nIndex > 0)
                g_normal_plan_csd_data.nIndex--;
            else
                g_normal_plan_csd_data.nIndex = maxindex;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_normal_plan_csd_data.nIndex < maxindex)
                g_normal_plan_csd_data.nIndex++;
            else
                g_normal_plan_csd_data.nIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (g_normal_plan_csd_data.nIndex)
            {
                char bzbuf[50];
                case 0: //OAD
                {
                    sprintf(bzbuf, "%08X", g_normalPlanCSD.oad.value);
                    InputBox(bzbuf, normalPlanCsdReturn, NULL, 0x01);  
                }
                break;
                case 1: //添加OAD
                {
                    sprintf(bzbuf, "添加第%d个csd，类型为OAD", g_guiNormalPlan.recdCol.num + 1);
                    if (TRUE == MessageBox(bzbuf, MB_OKCANCEL))
                    {
                        uint8 num = g_guiNormalPlan.recdCol.num;
                        g_guiNormalPlan.recdCol.csd[num].choice = 0;
                        g_guiNormalPlan.recdCol.csd[num].oad.value = g_normalPlanCSD.oad.value;
                        g_guiNormalPlan.recdCol.num++;
                    }
                }
                break;
                case 2: //主OAD
                {
                    sprintf(bzbuf, "%08X", g_normalPlanCSD.road.oadMain.value);
                    InputBox(bzbuf, normalPlanCsdReturn, NULL, 0x01);
                }
                break;

                case 3: //子OAD
                {
                    sprintf(bzbuf, "%08X", g_normalPlanCSD.road.oadCols.oad[0].value);
                    InputBox(bzbuf, normalPlanCsdReturn, NULL, 0x01);
                }
                break;
                case 4: //添加主OAD和子OAD
                {
                    sprintf(bzbuf, "主OAD:%08X \n\n子OAD:%08X", 
                        g_normalPlanCSD.road.oadMain.value,
                        g_normalPlanCSD.road.oadCols.oad[0].value);
                    uint8 num = g_guiNormalPlan.recdCol.num; 

                    if (num > OOP_MAX_CSDS)
                    {
                        MessageBox("CSD个数已到上限", MB_OK);
                    }
                    else if (g_csd_oads_set_num > OOP_MAX_OADS)
                    {
                        MessageBox("关联OAD个数已到上限", MB_OK);
                    }
                    else if (TRUE == MessageBox(bzbuf, MB_OKCANCEL))
                    {

                        if (num == 0)
                        {
                            g_guiNormalPlan.recdCol.csd[num].choice = 1;
                            g_guiNormalPlan.recdCol.csd[num].road.oadMain = g_normalPlanCSD.road.oadMain;
                            g_guiNormalPlan.recdCol.csd[num].road.oadCols.oad[g_csd_oads_set_num].value = g_normalPlanCSD.road.oadCols.oad[0].value;
                            g_csd_oads_set_num++;
                            g_guiNormalPlan.recdCol.csd[num].road.oadCols.nNum = g_csd_oads_set_num;
                            num++;
                        }
                        else
                        {
                            if (g_normalPlanCSD.road.oadMain.value !=  g_guiNormalPlan.recdCol.csd[num - 1].road.oadMain.value)
                            {
                                num++;
                                g_csd_oads_set_num = 0;
                            }
                            g_guiNormalPlan.recdCol.csd[num - 1].choice = 1;
                            g_guiNormalPlan.recdCol.csd[num - 1].road.oadMain = g_normalPlanCSD.road.oadMain;
                            g_guiNormalPlan.recdCol.csd[num - 1].road.oadCols.oad[g_csd_oads_set_num].value = g_normalPlanCSD.road.oadCols.oad[0].value;
                            g_csd_oads_set_num++;
                            g_guiNormalPlan.recdCol.csd[num - 1].road.oadCols.nNum = g_csd_oads_set_num;
                        }
                        g_guiNormalPlan.recdCol.num = num;
                    }
                }
                break;
                case 5: //查看
                {
                    g_normal_plan_csd_data.ppDetail = g_normal_plan_csd_display_plugin;
                    g_normal_plan_csd_data.nPluginCnt = sizeof(g_normal_plan_csd_display_plugin) / sizeof(g_normal_plan_csd_display_plugin[0]);
                    g_normal_plan_csd_data.nCurPlugin = 0;
                    g_normal_plan_csd_data.pCurPlugin = g_normal_plan_csd_data.ppDetail[g_normal_plan_csd_data.nCurPlugin];
                    ASSERT(g_normal_plan_csd_data.pCurPlugin != NULL);

                    if (g_normal_plan_csd_data.pCurPlugin->pfnInitLayer != NULL)
                    {
                        g_normal_plan_csd_data.pCurPlugin->pfnInitLayer(g_normal_plan_csd_data.pCurPlugin->dwId);
                    }
                    UpdateWindow(g_guiComm.hMainWnd, TRUE);
                }
                break;
                case 6: //重新输入
                {
                    if (TRUE == MessageBox("确定要删除？", MB_OKCANCEL))
                    {
                        memset(&g_guiNormalPlan.recdCol, 0, sizeof(OOP_RECORD_COL_T));
                    }
                }
                break;
                return TRUE;
            }   
        }
    }
    return FALSE;
}


//csd数据
PLUGIN_INFO g_layer_normal_plan_csd_set = {
    0x00/*id*/,
    normal_plan_csd_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    normal_plan_csd_set_draw/*Draw*/,
    NULL/*Timer*/,
    normal_plan_csd_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

//csd数据
PLUGIN_INFO* g_normal_plan_csd_set_plugin[] =
{
    &g_layer_normal_plan_csd_set,
};
#endif

#if DESC("普通采集方案设置",1)
MEASURE_MAIN g_normal_plan_main_data;

//删除普通采集方案
void delete_normal_plan(uint16 infonum)
{
    RECORD_OAD_T RecordDel;
    RecordDel.logicId = 0;
    RecordDel.infoNum = infonum;
    RecordDel.road.oadMain.value = 0;
    RecordDel.road.oadCols.nNum = 1;
    RecordDel.road.oadCols.oad[0] = NormalPlanOad;
    CLEAR_DATA_T clear;
    clear.recordOAD = RecordDel;
    clear.bClassTag = FALSE;
    int ret;
    ret = db_clear_oad_data(g_hUdbCenter, &clear);
    if (ret == 0)
    {
        GUI_FMT_DEBUG("普通采集方案删除成功 infonum %d \n", infonum);
    }
    else
    {
        GUI_FMT_DEBUG("普通采集方案删除失败 infonum %d \n", infonum);
    }

}
//设置普通采集方案编号
void normal_PlanIdReturn(char* pNewStr, void* pPara)
{
    int nLen = atoi(pNewStr);
    char tmp[20] = { 0 };
    if (nLen > 255 && TRUE == MessageBox("设置值大于255\n\n确认是否重新输入?", MB_OKCANCEL))
    {
        sprintf(tmp, "%d", g_guiComm.nCurPoint);
        InputBox(tmp, normal_PlanIdReturn, NULL, 0x01);
    }
    else
    {
        g_guiComm.nCurPoint = nLen;
    }
}


//设置存储深度
void MemDepthReturn(char* pNewStr, void* pPara)
{
    int nLen = atoi(pNewStr);
    char tmp[20] = { 0 };
    if (nLen > 65535 && TRUE == MessageBox("设置存储深度大于65535\n\n确认是否重新输入?", MB_OKCANCEL))
    {
        sprintf(tmp, "%d", g_guiNormalPlan.memDepth);
        InputBox(tmp, MemDepthReturn, NULL, 0x01);
    }
    else
    {
        g_guiNormalPlan.memDepth = nLen;
    }
}

//设置值
void normalPlanValueReturn(char* pNewStr, void* pPara)
{
    int nLen = atoi(pNewStr);
    char tmp[20] = { 0 };
    if (nLen > 65535 && TRUE == MessageBox("设置值大于65535\n\n确认是否重新输入?", MB_OKCANCEL))
    {
        if (g_guiNormalPlan.recdRow.choice == 1)
        {
            sprintf(tmp, "%d", g_guiNormalPlan.recdRow.num);
        }
        else if (g_guiNormalPlan.recdRow.choice == 3)
        {
            sprintf(tmp, "%d", g_guiNormalPlan.recdRow.ti.value);
        }
        else if (g_guiNormalPlan.recdRow.choice == 4)
        {
            sprintf(tmp, "%d", g_guiNormalPlan.recdRow.rd.ti.value);
        }
        InputBox(tmp, normalPlanValueReturn, NULL, 0x01);
    }
    else
    {
        if (g_guiNormalPlan.recdRow.choice == 1)
        {
            g_guiNormalPlan.recdRow.num = nLen;
        }
        else if (g_guiNormalPlan.recdRow.choice == 3)
        {
            g_guiNormalPlan.recdRow.ti.value = nLen;
        }
        else if (g_guiNormalPlan.recdRow.choice == 4)
        {
            g_guiNormalPlan.recdRow.rd.ti.value = nLen;
        }
    }
}

void normal_plan_set_init(HDC hdc)
{
    g_guiComm.nCurPoint = 1;

    nCurPoint = 0;
    g_normal_plan_main_data.ppDetail = NULL;
    g_normal_plan_main_data.pCurPlugin = NULL;
    g_normal_plan_main_data.nIndex = 0;
}

void normal_plan_set_draw(HDC hdc)
{
    // 绘制设置详情
    if (g_normal_plan_main_data.pCurPlugin != NULL)
    {
        ASSERT(g_normal_plan_main_data.pCurPlugin->pfnDraw != NULL);
        g_normal_plan_main_data.pCurPlugin->pfnDraw(hdc);
        return;
    }


    if(nCurPoint != g_guiComm.nCurPoint)
    {
        memset(&g_guiNormalPlan, 0, sizeof(OOP_PLAN_NORMAL_T));
        normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);
        g_normalPlanMsType = g_guiNormalPlan.ms.choice;
        set_gui_infonum((uint16)g_guiComm.nCurPoint);
        nCurPoint = g_guiComm.nCurPoint;
    }
    int flag = 0;  //标识档案是否存在
    char czBuf[300] = { 0 };
    int index = 0;
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 5;
    int nLeft2 = 65;
    TextOut(hdc, nLeft1, nTop, "方案编号：");
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);
    if (g_normal_plan_main_data.nIndex == index)
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
    if (g_guiNormalPlan.planID != 0)
        flag = 1;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "存储深度：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d", g_guiNormalPlan.memDepth);
    }
    if (g_normal_plan_main_data.nIndex == index)
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
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "采集类型：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 choice;
        choice = g_guiNormalPlan.recdRow.choice;
        if (choice > 4)
            choice = 0;
        sprintf(czBuf, "%s", g_normalPlanType[choice]);
    }
    if (g_normal_plan_main_data.nIndex == index)
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
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "值与单位：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 choice;
        choice = g_guiNormalPlan.recdRow.choice;
        if (choice > 4 || choice == 0 || choice == 2)
            sprintf(czBuf, "%s", "-");
        else
        {
            if (choice == 1) //采集上第N次
            {
                sprintf(czBuf, "%d", g_guiNormalPlan.recdRow.num);
            }
            else if (choice == 3) //按时标间隔采集
            {
                sprintf(czBuf, "%d", g_guiNormalPlan.recdRow.ti.value);
            }
            else //补抄
            {
                sprintf(czBuf, "%d", g_guiNormalPlan.recdRow.rd.ti.value);
            }
        }
    }
    if (g_normal_plan_main_data.nIndex == index)
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
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 choice;
        choice = g_guiNormalPlan.recdRow.choice;
        if (choice > 4 || choice == 0 || choice == 2)
            sprintf(czBuf, "%s", "-");
        else
        {
            if (choice == 1) //采集上第N次
            {
                sprintf(czBuf, "次");
            }
            else if (choice == 3) //按时标间隔采集
            {
                sprintf(czBuf, "%s", g_ti[g_guiNormalPlan.recdRow.ti.unit]);
            }
            else //补抄
            {
                sprintf(czBuf, "%s", g_ti[g_guiNormalPlan.recdRow.rd.ti.unit]);
            }
        }
    }
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2 + 30, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2 + 30, nTop, czBuf);
    }
    

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "表计集合：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 choice;
        choice = g_guiNormalPlan.ms.choice;
        if (choice > 7)
            choice = 0;
        sprintf(czBuf, "%s", g_ms[choice]);
    }
    if (g_normal_plan_main_data.nIndex == index)
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
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "存储时标：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 choice;
        choice = g_guiNormalPlan.tmFlag;
        if (choice > 7)
            choice = 0;
        sprintf(czBuf, "%s", g_tmFlag[choice]);
    }
    if (g_normal_plan_main_data.nIndex == index)
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
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "列选择符：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "设置详情");
    }
    if (g_normal_plan_main_data.nIndex == index)
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
    if (g_normal_plan_main_data.nIndex == index)
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
    nLeft2 = 50;
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "删除");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "删除");
    }

    index++;
    int nLeft3 = 90;
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, "保存设置");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, "保存设置");
    }

    PopListDraw(hdc);
}

BOOL normal_plan_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 10;

    if (g_normal_plan_main_data.pCurPlugin != NULL)
    {
        if (g_normal_plan_main_data.pCurPlugin->pfnKeyMsg != NULL
            && TRUE == g_normal_plan_main_data.pCurPlugin->pfnKeyMsg(nKey))
        {
            return TRUE;
        }

        switch (nKey)
        {
            case KEY_CANCEL:
                if (g_normal_plan_main_data.pCurPlugin->pfnUninitLayer != NULL)
                {
                    g_normal_plan_main_data.pCurPlugin->pfnUninitLayer();
                }
                g_normal_plan_main_data.pCurPlugin = NULL;
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                break;
        }
        return TRUE;
    }
    
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (g_normal_plan_main_data.nIndex)
            {
            case 2: //采集类型
            {
                g_guiNormalPlan.recdRow.choice = nSel;
            }
            break;
            case 4: //单位
            {
                g_guiNormalPlan.recdRow.ti.unit = nSel;
            }
            break;
            case 5: //表计类型
            {
                g_guiNormalPlan.ms.choice = nSel;
                if (nSel != 0 && nSel != 1) //不是"无表计"且不是"全部用户地址"
                {
                    g_normalPlanMsType = nSel;
                    g_normal_plan_main_data.ppDetail = g_normal_plan_ms_set_plugin;
                    g_normal_plan_main_data.nPluginCnt = sizeof(g_normal_plan_ms_set_plugin) / sizeof(g_normal_plan_ms_set_plugin[0]);
                    g_normal_plan_main_data.nCurPlugin = 0;
                    g_normal_plan_main_data.pCurPlugin = g_normal_plan_main_data.ppDetail[g_normal_plan_main_data.nCurPlugin];
                    ASSERT(g_normal_plan_main_data.pCurPlugin != NULL);

                    if (g_normal_plan_main_data.pCurPlugin->pfnInitLayer != NULL)
                    {
                        g_normal_plan_main_data.pCurPlugin->pfnInitLayer(g_normal_plan_main_data.pCurPlugin->dwId);
                    }
                }

            }
            break;
            case 6: //存储时标
            {
                g_guiNormalPlan.tmFlag = nSel;
            }
            break;
            default:
                return FALSE;
            }
            g_guiNormalPlan.planID = g_guiComm.nCurPoint;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }

    switch (nKey)
    {
    case KEY_LEFT:
    {
        if (g_guiComm.nCurPoint > 0)
            g_guiComm.nCurPoint--;
        if (g_guiComm.nCurPoint == 0)
            //g_guiComm.nCurPoint = MAX_PLAN_INFONUM;
            g_guiComm.nCurPoint = 255;
        memset(&g_guiNormalPlan, 0, sizeof(OOP_PLAN_NORMAL_T));
        normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);
        set_gui_infonum(g_guiComm.nCurPoint);
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_RIGHT:
    {
        //if (g_guiComm.nCurPoint < MAX_PLAN_INFONUM)
        if (g_guiComm.nCurPoint < 255)
        {
            g_guiComm.nCurPoint++;
        }
        else
        {
            g_guiComm.nCurPoint = 1;
        }
        memset(&g_guiNormalPlan, 0, sizeof(OOP_PLAN_NORMAL_T));
        normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);
        set_gui_infonum(g_guiComm.nCurPoint);
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_UP:
    {
        if (g_normal_plan_main_data.nIndex > 0)
            g_normal_plan_main_data.nIndex--;
        else
            g_normal_plan_main_data.nIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    {
        if (g_normal_plan_main_data.nIndex < maxindex)
            g_normal_plan_main_data.nIndex++;
        else
            g_normal_plan_main_data.nIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (g_normal_plan_main_data.nIndex == 0)
        {
            char bzbuf[50];
            sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
            //InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            InputBox(bzbuf, normal_PlanIdReturn, NULL, 0x01);
            return TRUE;
        }
        
        //设置存储深度
        if (g_normal_plan_main_data.nIndex == 1)
        {
            char bzbuf[50];
            sprintf(bzbuf, "%d", g_guiNormalPlan.memDepth);
            InputBox(bzbuf, MemDepthReturn, NULL, 0x01);
            return TRUE;
        }
        //设置采集类型
        if (g_normal_plan_main_data.nIndex == 2)
        {
            PopList(g_normalPlanType, 5, g_normalPlanType[0], 70, 40, FALSE);
            return TRUE;
        }
        //设置值
        if (g_normal_plan_main_data.nIndex == 3)
        {
            char bzbuf[50];
            if (g_guiNormalPlan.recdRow.choice == 1)
            {
                sprintf(bzbuf, "%d", g_guiNormalPlan.recdRow.num);
                InputBox(bzbuf, normalPlanValueReturn, NULL, 0x01);
            }
            else if (g_guiNormalPlan.recdRow.choice == 3)
            {
                sprintf(bzbuf, "%d", g_guiNormalPlan.recdRow.ti.value);
                InputBox(bzbuf, normalPlanValueReturn, NULL, 0x01);
            }
            else if (g_guiNormalPlan.recdRow.choice == 4)
            {
                sprintf(bzbuf, "%d", g_guiNormalPlan.recdRow.rd.ti.value);
                InputBox(bzbuf, normalPlanValueReturn, NULL, 0x01);
            }
            return TRUE;
        }
        //设置单位
        if (g_normal_plan_main_data.nIndex == 4 && (g_guiNormalPlan.recdRow.choice == 3 || g_guiNormalPlan.recdRow.choice == 4))
        {
            PopList(g_ti, 6, g_ti[0], 70, 40, FALSE);
            return TRUE;
        }
        //设置表计集合
        if (g_normal_plan_main_data.nIndex == 5)
        {
            PopList(g_ms, 8, g_ms[0], 50, 40, FALSE);
            return TRUE;
        }
        //设置存储时标
        if (g_normal_plan_main_data.nIndex == 6)
        {
            PopList(g_tmFlag, 8, g_tmFlag[0], 50, 40, FALSE);
            return TRUE;
        }

        //设置csd
        if (g_normal_plan_main_data.nIndex == 7)
        {
            g_normal_plan_main_data.ppDetail = g_normal_plan_csd_set_plugin;
            g_normal_plan_main_data.nPluginCnt = sizeof(g_normal_plan_csd_set_plugin) / sizeof(g_normal_plan_csd_set_plugin[0]);
            g_normal_plan_main_data.nCurPlugin = 0;
            g_normal_plan_main_data.pCurPlugin = g_normal_plan_main_data.ppDetail[g_normal_plan_main_data.nCurPlugin];
            ASSERT(g_normal_plan_main_data.pCurPlugin != NULL);

            if (g_normal_plan_main_data.pCurPlugin->pfnInitLayer != NULL)
            {
                g_normal_plan_main_data.pCurPlugin->pfnInitLayer(g_normal_plan_main_data.pCurPlugin->dwId);
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }

        //取消
        if (g_normal_plan_main_data.nIndex == 8)
        {
            if (TRUE == MessageBox("当前配置没有保存\n\n是否确定保存配置？", MB_OKCANCEL))
            {
                ret = write_normal_data((uint8*)&g_guiNormalPlan, sizeof(OOP_PLAN_NORMAL_T), NormalPlanOad, g_guiComm.nCurPoint, 0, 2);
                if (ret == 0)
                {
                    MessageBox("当前档案配置保存成功！");
                }
            }
            g_normal_plan_main_data.nIndex = 0;
            return FALSE;
        }
        //删除
        if (g_normal_plan_main_data.nIndex == 9)
        {
            if (TRUE == MessageBox("确认删除当前采集方案？", MB_OKCANCEL))
            {
                delete_normal_plan(g_guiComm.nCurPoint);
                g_guiComm.nCurPoint = 1;
                normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
            g_normal_plan_main_data.nIndex = 0;
            return FALSE;
        }
        //保存配置
        if (g_normal_plan_main_data.nIndex == 10)
        {
            GUI_FMT_DEBUG("write_normal_data %d\n", g_guiNormalPlan.ms.choice);
            ret = write_normal_data((uint8*)&g_guiNormalPlan, sizeof(OOP_PLAN_NORMAL_T), NormalPlanOad, g_guiComm.nCurPoint, 0, 2);
            if (ret == 0)
            {
                MessageBox("当前采集方案配置保存成功！");
                g_normal_plan_main_data.nIndex = 0;
                return TRUE;
            }
            else
            {
                MessageBox("当前采集方案配置保存失败！");
                return FALSE;
            }
        }
    }
    }

    return FALSE;
}

//普通方案设置
PLUGIN_INFO g_layer_normal_plan_set = {
    0x11030080/*id*/,
    normal_plan_set_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    normal_plan_set_draw/*Draw*/,
    NULL/*Timer*/,
    normal_plan_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("采集任务设置", 1)
int     g_tastSetIndex;

void delete_task(uint16 infonum)
{
    RECORD_OAD_T RecordDel;
    RecordDel.logicId = 0;
    RecordDel.infoNum = infonum;
    RecordDel.road.oadMain.value = 0;
    RecordDel.road.oadCols.nNum = 1;
    RecordDel.road.oadCols.oad[0] = TaskOad;
    CLEAR_DATA_T clear;
    clear.recordOAD = RecordDel;
    clear.bClassTag = FALSE;
    int ret;
    ret = db_clear_oad_data(g_hUdbCenter, &clear);
    if (ret == 0)
    {
        GUI_FMT_DEBUG("采集任务删除成功 infonum %d \n", infonum);
    }
    else
    {
        GUI_FMT_DEBUG("采集任务删除失败 infonum %d \n", infonum);
    }

}

void taskSetDefaultPara(void)
{
    TaskInfo.id = g_guiComm.nCurPoint;

    TaskInfo.startTime.year     = 2017;
    TaskInfo.startTime.month    = 1;
    TaskInfo.startTime.day      = 1;
    TaskInfo.startTime.hour     = 0;
    TaskInfo.startTime.minute   = 0;
    TaskInfo.startTime.second   = 0;
    TaskInfo.startTime.week     = 0;

    TaskInfo.endTime.year       = 2099;
    TaskInfo.endTime.month      = 1;
    TaskInfo.endTime.day        = 1;
    TaskInfo.endTime.hour       = 0;
    TaskInfo.endTime.minute     = 0;
    TaskInfo.endTime.second     = 0;
    TaskInfo.endTime.week       = 0;

    TaskInfo.startScriptID      = 0;
    TaskInfo.endScriptID        = 0;

    TaskInfo.workPeriod.nNum    = 1;
    TaskInfo.workPeriod.type    = 0;

    TaskInfo.workPeriod.period[0].startHour = 0;
    TaskInfo.workPeriod.period[0].startMin  = 0;
    TaskInfo.workPeriod.period[0].endHour   = 23;
    TaskInfo.workPeriod.period[0].endMin    = 59;
}

void taskSetTimeReturn(char* pNewStr, void* pPara)
{
    int nLen = atoi(pNewStr);
    char tmp[20] = { 0 };
    if (nLen > 65535 && TRUE == MessageBox("设置时间大于65535\n\n确认是否重新输入?", MB_OKCANCEL))
    {
        if (g_tastSetIndex == 1)
            sprintf(tmp, "%d", TaskInfo.acqFreq.value);
        else if (g_tastSetIndex == 5)
            sprintf(tmp, "%d", TaskInfo.acqDelay.value);
        InputBox(tmp, taskSetTimeReturn, NULL, 0x01);
    }
    else
    {
        if (g_tastSetIndex == 1)
            TaskInfo.acqFreq.value = nLen;
        else if (g_tastSetIndex == 5)
            TaskInfo.acqDelay.value = nLen;
    }
}

void taskSetPlanIdReturn(char* pNewStr, void* pPara)
{
    int nLen = atoi(pNewStr);
    char tmp[20] = { 0 };
    if (nLen > 255 && TRUE == MessageBox("设置值大于255\n\n确认是否重新输入?", MB_OKCANCEL))
    {
        if(g_tastSetIndex == 0)
            sprintf(tmp, "%d", g_guiComm.nCurPoint);
        else if (g_tastSetIndex == 4)
            sprintf(tmp, "%d", TaskInfo.planID);
        else if (g_tastSetIndex == 7)
            sprintf(tmp, "%d", TaskInfo.priority);
        InputBox(tmp, taskSetPlanIdReturn, NULL, 0x01);
    }
    else
    {
        if(g_tastSetIndex == 0)
            g_guiComm.nCurPoint = nLen;
        else if (g_tastSetIndex == 4)
            TaskInfo.planID = nLen;
        else if (g_tastSetIndex == 7)
            TaskInfo.priority = nLen;
    }
}


void task_set_init(HDC hdc)
{
    g_guiComm.nCurPoint = 1;
    nCurPoint = 0;
    g_tastSetIndex = 0;
}

void task_set_draw(HDC hdc)
{
    char czBuf[300] = { 0 };

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
    if(nCurPoint != g_guiComm.nCurPoint)
    {
        memset(&TaskInfo, 0x00, sizeof(OOP_TASK_T));
        get_taskinfo(g_guiComm.nCurPoint, &TaskInfo);
        set_gui_infonum((uint16)g_guiComm.nCurPoint);
        nCurPoint = g_guiComm.nCurPoint;
    }

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int index = 0;
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "任务ID：");
    
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);
    if (g_tastSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "执行间隔：");
    memset(czBuf, 0x00, sizeof(czBuf));
    index++;
    sprintf(czBuf, "%d", TaskInfo.acqFreq.value);
    if (g_tastSetIndex == index)
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
    sprintf(czBuf, "%s", g_ti[TaskInfo.acqFreq.unit]);
    if (g_tastSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2 + 30, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2 + 30, nTop, czBuf);
    }

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "方案类型：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (TaskInfo.planType == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        if (TaskInfo.planType > 0 && TaskInfo.planType < 6)
            sprintf(czBuf, "%s", g_plantype[TaskInfo.planType - 1]);
    }
    index++;
    if (g_tastSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "方案编号：");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d", TaskInfo.planID);
    index++;
    if (g_tastSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "延时：");
    memset(czBuf, 0x00, sizeof(czBuf));
    index++;
    sprintf(czBuf, "%d", TaskInfo.acqDelay.value);
    if (g_tastSetIndex == index)
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
    sprintf(czBuf, "%s", g_ti[TaskInfo.acqDelay.unit]);
    if (g_tastSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2 + 30, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2 + 30, nTop, czBuf);
    }

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "优先级：");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d", TaskInfo.priority);
    index++;
    if (g_tastSetIndex == index)
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
    if (g_tastSetIndex == index)
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
    nLeft2 = 50;
    if (g_tastSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "删除");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "删除");
    }

    index++;
    int nLeft3 = 90;
    if (g_tastSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, "保存设置");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, "保存设置");
    }

    PopListDraw(hdc);
}

BOOL task_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 10;
    
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (g_tastSetIndex)
            {
            case 2: //执行间隔单位
            {
                TaskInfo.acqFreq.unit = nSel;
            }
            break;
            case 3: //方案类型
            {         
                TaskInfo.planType = nSel + 1;
            }
            break;
            case 6: //延时单位
            {
                TaskInfo.acqDelay.unit = nSel;
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
    case KEY_LEFT:
    {
        if (g_guiComm.nCurPoint > 0)
            g_guiComm.nCurPoint--;
        if (g_guiComm.nCurPoint == 0)
            //g_guiComm.nCurPoint = MAX_PLAN_INFONUM;
            g_guiComm.nCurPoint =255;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_RIGHT:
    {
        //if (g_guiComm.nCurPoint < MAX_PLAN_INFONUM)
        if (g_guiComm.nCurPoint < 255)
        {
            g_guiComm.nCurPoint++;
        }
        else
        {
            g_guiComm.nCurPoint = 1;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_UP:
    {
        if (g_tastSetIndex > 0)
            g_tastSetIndex--;
        else
            g_tastSetIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    {
        if (g_tastSetIndex < maxindex)
            g_tastSetIndex++;
        else
            g_tastSetIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (g_tastSetIndex == 0)
        {
            char bzbuf[50];
            sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
            //InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            InputBox(bzbuf, taskSetPlanIdReturn, NULL, 0x01);
            return TRUE;
        }
        
        //设置执行或延时间隔
        if (g_tastSetIndex == 1 || g_tastSetIndex == 5)
        {
            char bzbuf[50];
            if (g_tastSetIndex == 1)
                sprintf(bzbuf, "%d", TaskInfo.acqFreq.value);
            else
                sprintf(bzbuf, "%d", TaskInfo.acqDelay.value);
            InputBox(bzbuf, taskSetTimeReturn, NULL, 0x01);
            return TRUE;
        }
        
        //设置执行或延时单位
        if (g_tastSetIndex == 2 || g_tastSetIndex == 6)
        {
            PopList(g_ti, 6, g_ti[0], 70, 40, FALSE);
            return TRUE;
        }
        
        //设置方案编号
        if (g_tastSetIndex == 3)
        {
            PopList(g_plantype, 5, g_plantype[0], 70, 40, FALSE);
            return TRUE;
        }

        //设置方案编号
        if (g_tastSetIndex == 4)
        {
            char bzbuf[50];
            sprintf(bzbuf, "%d", TaskInfo.planID);
            InputBox(bzbuf, taskSetPlanIdReturn, NULL, 0x01);
            return TRUE;
        }
        
        //设置优先级
        if (g_tastSetIndex == 7)
        {
            char bzbuf[50];
            sprintf(bzbuf, "%d", TaskInfo.priority);
            InputBox(bzbuf, taskSetPlanIdReturn, NULL, 0x01);
            return TRUE;
        }
        
        //取消
        if (g_tastSetIndex == 8)
        {
            if (TRUE == MessageBox("当前配置没有保存\n\n是否确定保存配置？", MB_OKCANCEL))
            {
                taskSetDefaultPara();
                ret = write_normal_data((uint8*)&TaskInfo, sizeof(OOP_TASK_T), TaskOad, g_guiComm.nCurPoint, 0, 2);
                if (ret == 0)
                {
                    MessageBox("当前任务配置保存成功！");
                }
            }
            memset(&TaskInfo, 0x00, sizeof(OOP_TASK_T));
            get_taskinfo(g_guiComm.nCurPoint, &TaskInfo);
            g_tastSetIndex = 0;
            return FALSE;
        }
        //删除
        if (g_tastSetIndex == 9)
        {
            if (TRUE == MessageBox("确认删除当前采集任务？", MB_OKCANCEL))
            {
                delete_task(g_guiComm.nCurPoint);
                memset(&TaskInfo, 0x00, sizeof(OOP_TASK_T));
                get_taskinfo(g_guiComm.nCurPoint, &TaskInfo);
                g_tastSetIndex = 0;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
            g_tastSetIndex = 0;
            return FALSE;
        }
        //保存配置
        if (g_tastSetIndex == 10)
        {
            taskSetDefaultPara();
            ret = write_normal_data((uint8*)&TaskInfo, sizeof(OOP_TASK_T), TaskOad, g_guiComm.nCurPoint, 0, 2);
            if (ret == 0)
            {
                MessageBox("当前采集任务保存成功！");
                g_tastSetIndex = 0;
                return TRUE;
            }
            else
            {
                MessageBox("当前采集任务保存失败！");
                return FALSE;
            }
        }
    }
    }
    return FALSE;
}


//采集任务设置
PLUGIN_INFO g_layer_task_set = {
    0x11030090/*id*/,
    task_set_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    task_set_draw/*Draw*/,
    NULL/*Timer*/,
    task_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif


#if DESC("HPLC参数设置", 1)

void HPLC_set_init(HDC hdc)
{
    AcrossAreaFlagIsSet = read_db_noamal_data(g_hUdbCenter, 0xF2090B00, 0, 0, &HPLCAcrossAreaFlag, 1);
    NodeRejectFlagIsSet = read_db_noamal_data(g_hUdbCenter, 0xF2091600, 0, 0, &HPLCNodeRejectFlag, 1);
    printf("AcrossAreaFlagIsSet %d NodeRejectFlagIsSet %d", AcrossAreaFlagIsSet, NodeRejectFlagIsSet);
    HPLC_set_index = 0;
}

void HPLC_set_draw(HDC hdc)
{
    char czBuf[300] = { 0 };

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
    int index = 0;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 120;
    TextOut(hdc, nLeft1, nTop, "台区识别开关：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (AcrossAreaFlagIsSet < 0)
    {
        sprintf(czBuf, "关闭");
    }
    else
    {
        uint8 tmp = HPLCAcrossAreaFlag & 0x01;
        sprintf(czBuf, "%s", tmp == 0 ? "关闭" : "开启");
    }
    if (HPLC_set_index == index)
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
    TextOut(hdc, nLeft1, nTop, "离网感知开关：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (AcrossAreaFlagIsSet < 0)
    {
        sprintf(czBuf, "关闭");
    }
    else
    {
        uint8 tmp = ((HPLCAcrossAreaFlag>>2) & 0x01);
        sprintf(czBuf, "%s", tmp == 0 ? "关闭" : "开启");
    }
    if (HPLC_set_index == index)
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
    TextOut(hdc, nLeft1, nTop, "拒绝节点上报开关：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (NodeRejectFlagIsSet < 0)
    {
        sprintf(czBuf, "关闭");
    }
    else
    {
        sprintf(czBuf, "%s", HPLCNodeRejectFlag == 0 ? "关闭" : "开启");
    }
    if (HPLC_set_index == index)
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

    if (HPLC_set_index == index)
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
    if (HPLC_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "保存配置");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "保存配置");
    }

    PopListDraw(hdc);
}

BOOL HPLC_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    uint8 areaident = HPLCAcrossAreaFlag & 0x01; //台区识别标志
    uint8 awaynet =  (HPLCAcrossAreaFlag>>2) & 0x01;   //离网标志
    int maxindex = 4;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (HPLC_set_index)
            {
            case 0://台区识别开关
            {
                AcrossAreaFlagIsSet = 1;
                areaident = nSel;
            }
            break;
            case 1://离网感知开关
            {
                AcrossAreaFlagIsSet = 1;
                awaynet = nSel;
                //HPLCAcrossAreaFlag = HPLCAcrossAreaFlag | (nSel << 2);
            }
            break;
            case 2://拒绝节点上报开关
            {
                NodeRejectFlagIsSet = 1;
                HPLCNodeRejectFlag = nSel;
            }
            break;
            default:
                return FALSE;
            }
            HPLCAcrossAreaFlag = (awaynet<<2)|areaident;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }

    switch (nKey)
    {
    case KEY_LEFT:
    case KEY_UP:
    {
        if (HPLC_set_index > 0)
            HPLC_set_index--;
        else
            HPLC_set_index = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_RIGHT:
    case KEY_DOWN:
    {
        if (HPLC_set_index < maxindex)
            HPLC_set_index++;
        else
            HPLC_set_index = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        //台区识别开关
        if (HPLC_set_index == 0)
        {
            PopList(g_on_off, 2, g_on_off[0], 120, 20, FALSE);
            return TRUE;
        }
        //离网感知开关
        if (HPLC_set_index == 1)
        {
            PopList(g_on_off, 2, g_on_off[0], 120, 40, FALSE);
            return TRUE;
        }
        //拒绝节点上报开关
        if (HPLC_set_index == 2)
        {
            PopList(g_on_off, 2, g_on_off[0], 120, 60, FALSE);
            return TRUE;
        }
        //取消
        if (HPLC_set_index == 3)
        {
            if (TRUE == MessageBox("当前配置没有保存\n\n是否确定保存配置？", MB_OKCANCEL))
            {
                OOP_OAD_U oad;
                oad.value = 0xF2090B00;
                ret = write_normal_data(&HPLCAcrossAreaFlag, 1, oad, 0, 0, 2);
                if (ret != 0)
                {
                    MessageBox("HPLC台区识别开关保存失败");
                }
                oad.value = 0xF2091600;
                ret = write_normal_data(&HPLCNodeRejectFlag, 1, oad, 0, 0, 2);
                if (ret != 0)
                {
                    MessageBox("HPLC拒绝节点上报开关保存失败");
                }
                MessageBox("配置保存成功！");
            }
            HPLC_set_index = 0;
            return FALSE;
        }
        //保存
        if (HPLC_set_index == 4)
        {
            OOP_OAD_U oad;
            oad.value = 0xF2090B00;
            ret = write_normal_data(&HPLCAcrossAreaFlag, 1, oad, 0, 0, 2);
            if (ret != 0)
            {
                MessageBox("HPLC台区识别开关保存失败");
            }
            oad.value = 0xF2091600;
            ret = write_normal_data(&HPLCNodeRejectFlag, 1, oad, 0, 0, 2);
            if (ret != 0)
            {
                MessageBox("HPLC拒绝节点上报开关保存失败");
            }
            MessageBox("配置保存成功！");
            HPLC_set_index = 0;
            return FALSE;
        }
    }
    }
    return FALSE;
}
//HPLC参数设置
PLUGIN_INFO g_layer_HPLC_set = {
    0x11030015/*id*/,
    HPLC_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    HPLC_set_draw/*Draw*/,
    NULL/*Timer*/,
    HPLC_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};
    

#endif

#if DESC("载波频点设置和查询", 1)
uint8 page_select;

void HCHanenl_set_init(HDC hdc)
{
    HPLC_set_index = 0;
    HPLCChannel = 0;
    recvbit=2;
    page_select=0;
}

void HCHanenl_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
    int index = 0;
    char czBuf[300] = {0};
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 80;
    TextOut(hdc, nLeft1, nTop, "功能选择：");
    if(page_select == 0)
    {
       sprintf(czBuf, "%s", "设置载波频点"); 
    }
    else
    {   
    sprintf(czBuf, "%s", "查询载波频点"); 
    }
    if (HPLC_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    if(page_select == 0)
    {
    nTop+=20;
    index++;
    TextOut(hdc, nLeft1, nTop, "载波频点：");
    if (HPLCChannel < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d", HPLCChannel);
    }

    if (HPLC_set_index == index)
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
    nTop += 40;

    if (HPLC_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "返回");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "返回");
    }

    index++;
    if (HPLC_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "设置");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "设置");
    }
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "***********************************");
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "下发结果");
    if(recvbit==3)
    TextOut(hdc, nLeft2, nTop, "下发中");
    else if(recvbit == 2)
    TextOut(hdc, nLeft2, nTop, "等待下发");
    else if(recvbit == 1)
    TextOut(hdc, nLeft2, nTop, "下发成功");
    else if(recvbit == 0)
    TextOut(hdc, nLeft2, nTop, "下发失败");
    }
    else
    {       
    nTop+=20;
    index++;
    if (HPLC_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "返回");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "返回");
    }
    index++;
    if (HPLC_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "查询");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "查询");
    }
    nTop+=20;
    TextOut(hdc, nLeft1, nTop, "***********************************");
    nTop += 40;
    TextOut(hdc, nLeft1, nTop, "查询结果");
    if(recvbit==5)
    sprintf(czBuf, "%s", "等待召测");
    else if(recvbit == 4)
    sprintf(czBuf, "%s", "召测中");
    else if(recvbit<4)
    sprintf(czBuf, "%s%d", "频段",recvbit);
    index++;
    if (HPLC_set_index == index)
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
    PopListDraw(hdc);
}

void HPLCChennelReturn(char* pNewStr, void* pPara)
{
    int inputdata = atoi(pNewStr);
    char tmp[20] = { 0 };
    printf("输入的HPLC频点为 %s %d\n", pNewStr, inputdata);
    if (inputdata > 3 && TRUE == MessageBox("设置频点大于3\n\n确认是否重新输入?", MB_OKCANCEL))
    {
        strcpy(tmp, "");
        InputBox(tmp, HPLCChennelReturn, NULL, 0x01);
    }
    else
    {
        if (inputdata <= 3)
        {
            HPLCChannel = inputdata;
            ChannelIsSet = 0;
        }
    }
}
void hplcAmrMakeFrame(uint8 *apdu, uint8 *len,uint8 flag)
{
    uint8   index=0;
    uint8   tmpoffset;
    uint8 cs=0,i;
    memset(apdu,0,100);
    apdu[index++]=0x09;
    apdu[index++]=0x07;
    apdu[index++]=0x01;
    apdu[index++]=0xf2;
    apdu[index++]=0x09;
    apdu[index++]=0x02;    
    apdu[index++]=0x01;
    //波特率
    apdu[index++]=0x03;
    apdu[index++]=0x02;
    apdu[index++]=0x08;
    apdu[index++]=0x01;
    apdu[index++]=0x00;
    //
    apdu[index++] = 0x00;
    apdu[index++] = 0x32;
    //字节超时时间(10ms)
    apdu[index++] = 0x00;
    apdu[index++] = 0x0A;
    if(flag == 1)
    {
        apdu[index++] = 0x0F;
        apdu[index++] = 0x68;
        apdu[index++] = 0x0F;
        apdu[index++] = 0x00;
        apdu[index++] = 0x43;
        apdu[index++] = 0x00;
        apdu[index++] = 0x00;
        apdu[index++] = 0x00;
        apdu[index++] = 0x00;
        apdu[index++] = 0x00;
        apdu[index++] = 0x0E;
        apdu[index++] = 0x03;
        apdu[index++] = 0x80;
        apdu[index++] = 0x01;
        apdu[index++] = 0xD5;
        apdu[index++] = 0x16;
    }
    else
   {
    apdu[index++] = 0x10;
    apdu[index++] = 0x68;
    apdu[index++] = 0x10;
    apdu[index++] = 0x00;
    tmpoffset=index;
    apdu[index++] = 0x43;
    apdu[index++] = 0x00;
    apdu[index++] = 0x00;
    apdu[index++] = 0x00;
    apdu[index++] = 0x00;
    apdu[index++] = 0x00;
    apdu[index++] = 0x0d;
    apdu[index++] = 0x05;
    apdu[index++] = 0x80;
    apdu[index++] = 0x01;
    apdu[index++] = HPLCChannel;
    for(i=tmpoffset;i<index;i++)
    {
    cs+=apdu[i];
    }
    apdu[index++] = cs;
    apdu[index++] = 0x16;
   }
    apdu[index++] = 0x00;
    *len = index;
}

BOOL HCHanenl_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
    {
        return FALSE;
    }
    uint8 maxindex = 3;
//    uint8 buf[100]={0};
//    uint8 buflen=0;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (HPLC_set_index)
            {
            case 0://功能选择
            {
                AcrossAreaFlagIsSet = 1;
                page_select = nSel;
            }
            break;
            default:
            return FALSE;            
            }
            
            if(page_select == 0)
            {
            recvbit = 2;
            }
            else
            {
            recvbit = 5;
            }
            HPLC_set_index = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        return TRUE;
    }
    switch (nKey)
    {
    case KEY_LEFT:
    case KEY_UP:
    {
        if (HPLC_set_index > 0)
            HPLC_set_index--;
        else
            HPLC_set_index = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_RIGHT:
    case KEY_DOWN:
    {
        if (HPLC_set_index < maxindex)
            HPLC_set_index++;
        else
            HPLC_set_index = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        //频点设置
        if (HPLC_set_index == 0)
        {
            PopList(g_Hplc_type, 2, g_on_off[0], 80, 20, FALSE);
            return TRUE;
        }

        if(page_select==0)
        {
        if (HPLC_set_index == 1)
        {
            char tmp[20] = { 0 };
            strcpy(tmp, "");
            InputBox(tmp, HPLCChennelReturn, NULL, 0x01);
            return TRUE;
        }
        //取消
        if (HPLC_set_index == 2)
        {
            CancelMenu();
            HPLC_set_index = 0;
            return FALSE;
        }
        //保存
        if (HPLC_set_index == 3)
        {
//            hplcAmrMakeFrame(buf,&buflen,0);
//            gui_mannual_amr_send(buf,buflen);
            recvbit=3;
            HPLC_set_index = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return FALSE;
        }
       }
        else
        {
        if (HPLC_set_index == 2)
        {
//            hplcAmrMakeFrame(buf,&buflen,1);
//            gui_mannual_amr_send(buf,buflen);
            recvbit=4;
            HPLC_set_index = 3;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return FALSE;
        }
        if (HPLC_set_index == 1)
        {
            CancelMenu();
            HPLC_set_index = 0;
            return FALSE;
        }
        }
    }
    }
    return FALSE;
}

PLUGIN_INFO g_layer_Hchanenl_set = {
    0x11030083/*id*/,
    HCHanenl_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    HCHanenl_set_draw/*Draw*/,
    NULL/*Timer*/,
    HCHanenl_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("Ki Kv Kp",1)

OOP_METER_T g_MeterT[16];            //60010200 电能表档案
  
uint32 g_3kCurPage = 0;        

uint8 g_3kCurNum = 8;   //控制每页显示多少条数据

DESK_METER_LIST_T meterList;

//3k值为60010203.电压电流互感器变比
void kivp_display_init(DWORD dwPluginId)
{
    //g_meterCurPage = (dwPluginId) & 0x0F;
    //appdb_meter_list_record_init(g_hUdbCenter);
    /*set_gui_infonum(g_3kCurPage);

    memset(&g_MeterT, 0, sizeof(OOP_METER_T)*16);

   for(int i = 1;i<=g_3kCurNum; i++)
   {
    read_db_noamal_data(g_hUdbCenter, 0x60000200, 0, g_3kCurPage*g_3kCurNum + i, (uint8*)&g_MeterT[g_3kCurPage*g_3kCurNum+i-1], sizeof(OOP_METER_T));
   }*/
   memset(&meterList,0x00,sizeof(DESK_METER_LIST_T));
   meter_info_read_all(&meterList);
}

void kivp_plate_draw(HDC hdc)
{
    char czBuf[100];

    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;
    sprintf(czBuf, "测量点");
   	TextOut(hdc, 2, 20, czBuf);

    bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "Ki");
    TextOut(hdc, 52, 20, czBuf);

    bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "Kv");
    TextOut(hdc, 85, 20, czBuf);
    
    bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "Kp");
    TextOut(hdc, 124, 20, czBuf);

}

void kivp_Total_draw(HDC hdc)
{
    
    //kivp_display_init(0x20130000);
    
    char czBuf[100];

    kivp_plate_draw(hdc);
    if (meterList.meterNum == 0)
        TextOut(hdc, 60, 50, "无电表记录");
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    //判断是否有数据,判断依据:g_MeterT[i].nIndex == 0
    if(g_3kCurNum*g_3kCurPage < meterList.meterNum){

        for(int i =0;i<g_3kCurNum;i++)
        {
            if (meterList.meterInfo[g_3kCurPage*g_3kCurNum+i].pn != 0){
                bzero(czBuf,sizeof(czBuf));
                sprintf(czBuf,"%d.",meterList.meterInfo[g_3kCurPage*g_3kCurNum+i].pn);
                TextOut(hdc, 1, 33+13*i, czBuf);
                bzero(czBuf,sizeof(czBuf));
                sprintf(czBuf, "%5hu", meterList.meterInfo[g_3kCurPage*g_3kCurNum+i].CT);
                TextOut(hdc, 30, 33+13*i, czBuf);
                bzero(czBuf,sizeof(czBuf));
                sprintf(czBuf, "%5hu", meterList.meterInfo[g_3kCurPage*g_3kCurNum+i].PT);
                TextOut(hdc, 65, 33+13*i, czBuf);
                bzero(czBuf,sizeof(czBuf));
                sprintf(czBuf, "%6u", (meterList.meterInfo[g_3kCurPage*g_3kCurNum+i].PT)*(meterList.meterInfo[g_3kCurPage*g_3kCurNum+i].CT));
                TextOut(hdc, 100, 33+13*i, czBuf);
                
            }
        }
    }
}


BOOL kivp_Key_Msg(int nkey)
{
    uint32 curPageMax;
    if (meterList.meterNum%8 == 0)
        curPageMax = meterList.meterNum/8 - 1;
    else
        curPageMax = meterList.meterNum/8;
    switch (nkey)
    {
        case KEY_DOWN:
            g_3kCurPage>=curPageMax? g_3kCurPage=0: ++g_3kCurPage;
            break;

        case KEY_UP:
            g_3kCurPage<=0?g_3kCurPage=curPageMax : --g_3kCurPage;
            break;
            
        case KEY_CANCEL:
            g_3kCurPage = 0;
            CancelMenu();
            break;
    }
    return TRUE;

}


//3k值
PLUGIN_INFO g_layer_kivp_display = {
    0x20130000/*id*/,
    kivp_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    kivp_Total_draw/*Draw*/,
    NULL/*Timer*/,
    kivp_Key_Msg/*keymsg*/,
    NULL/*ipc*/
};

#endif


#if DESC("脉冲参数",1)

OOP_PTCT_T  sdata[8];

void kivp_puls_display_init(DWORD dwPluginId)
{

    memset(sdata, 0, sizeof(OOP_PTCT_T)*8);
    read_db_noamal_data(g_hUdbCenter, 0x24010300, 0, 0, (uint8*)&sdata[0], sizeof(OOP_PTCT_T));

    read_db_noamal_data(g_hUdbCenter, 0x24020300, 0, 0, (uint8*)&sdata[1], sizeof(OOP_PTCT_T));

    read_db_noamal_data(g_hUdbCenter, 0x24030300, 0, 0, (uint8*)&sdata[2], sizeof(OOP_PTCT_T));

    read_db_noamal_data(g_hUdbCenter, 0x24040300, 0, 0, (uint8*)&sdata[3], sizeof(OOP_PTCT_T));

    read_db_noamal_data(g_hUdbCenter, 0x24050300, 0, 0, (uint8*)&sdata[4], sizeof(OOP_PTCT_T));

    read_db_noamal_data(g_hUdbCenter, 0x24060300, 0, 0, (uint8*)&sdata[5], sizeof(OOP_PTCT_T));

    read_db_noamal_data(g_hUdbCenter, 0x24070300, 0, 0, (uint8*)&sdata[6], sizeof(OOP_PTCT_T));

    read_db_noamal_data(g_hUdbCenter, 0x24080300, 0, 0, (uint8*)&sdata[7], sizeof(OOP_PTCT_T));

}

void kivp_puls_plate_draw(HDC hdc)
{
    char czBuf[100];

    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;
    bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "脉冲");
   	TextOut(hdc, 2, 20, czBuf);

    bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "Ki");
    TextOut(hdc, 52, 20, czBuf);

    bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "Kv");
    TextOut(hdc, 85, 20, czBuf);
    
    bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "Kp");
    TextOut(hdc, 124, 20, czBuf);


}

void kivp_puls_Total_draw(HDC hdc)
{

    char czBuf[100];

    kivp_puls_plate_draw(hdc);
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    for(int i =0;i<4;i++)
    {
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf,"第%d路",i+1+4*g_3kCurPage);
        TextOut(hdc,1, 40+25*i, czBuf);
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf, "%5hu", sdata[g_3kCurPage*4+i].pt);
        TextOut(hdc, 34, 40+25*i, czBuf);
        sprintf(czBuf, "%5hu", sdata[g_3kCurPage*4+i].ct);
        TextOut(hdc, 67, 40+25*i, czBuf);
        sprintf(czBuf, "%6qu", (uint64)(sdata[g_3kCurPage*4+i].pt)*(sdata[g_3kCurPage*4+i].ct));
        TextOut(hdc, 100, 40+25*i, czBuf);

    }
}

BOOL kivp_puls_Key_Msg(int nkey)
{
	uint32 curPageMax = 1;
	switch (nkey)
	{
		case KEY_DOWN:
			g_3kCurPage>=curPageMax? g_3kCurPage=0: ++g_3kCurPage;
			break;

		case KEY_UP:
			g_3kCurPage<=0?g_3kCurPage=curPageMax : --g_3kCurPage;
			break;
			
		case KEY_CANCEL:
			g_3kCurPage = 0;
			CancelMenu();
			break;
	}
	return TRUE;

}


PLUGIN_INFO g_layer_kivp_puls_display = {
    0x20130001/*id*/,
    kivp_puls_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    kivp_puls_Total_draw/*Draw*/,
    NULL/*Timer*/,
    kivp_puls_Key_Msg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("协处理器版本显示",1)
void co_ver_Init(DWORD dwPluginId)
{

}

int get_co_ver(char * temp)
{
    char buf[1024] = {0};

    if(temp == NULL)
        return -1;
    
    FILE* pipe = fopen("/data/app/rspSample/data/version", "r");
    if (!pipe)
    {
        pipe = fopen("/data/app/rspSample/data/version", "r");
    }
    if (!pipe)
    {
        GUI_FMT_DEBUG("查询协处理器版本失败 错误码[-2]\n");
        return -2;
    }

    if (fgets(buf, 1024, pipe) != NULL)
    {
        strcpy(temp, buf);
    }
    else
    {
        GUI_FMT_DEBUG("查询协处理器版本失败 错误码[-3]\n");
        return -3;
    }
    
    fclose(pipe);
    return 0;
}

void co_ver_Draw(HDC hdc)
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
    char pcz[40];
    strcpy(pcz, "协处理器当前版本");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);
    
    int nTop = 80;
    int nLeft = 20;

    memset(pcz, 0, 40);
    if (get_co_ver(pcz) == 0)
    {
        TextOut(hdc, nLeft, nTop, pcz);
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "-----");
    }

    return;
}


PLUGIN_INFO g_layer_co_ver=
{
    0x00A000FD, // 0x00A000FD
    co_ver_Init,
    NULL,
    co_ver_Draw,
    NULL,
    NULL,
    NULL
};

#endif

#if DESC("系统版本显示",1)
void sys_ver_Init(DWORD dwPluginId)
{

}

int get_sys_ver(char * temp)
{
    char buf[1024] = {0};

    if(temp == NULL)
        return -1;
    
    FILE* pipe = fopen("/proc/version", "r");  
    if (!pipe)  
    {
        pipe = fopen("/proc/version", "r");
    }
    if (!pipe)
    {
        GUI_FMT_DEBUG("查询系统版本失败, /proc/version！\n");
        return -1;
    }

    fseek(pipe, -1024L, SEEK_END);
    while (fgets(buf, 1024, pipe) != NULL)
    {
        GUI_FMT_TRACE("%s", buf);
        char *pfile = NULL;
        if ((pfile = strstr(buf,"+++OS_RK3358")) != NULL)
        {
            if (strlen(pfile) < 256)
            {
                strcpy(temp, pfile);
            }
            else
            {
                GUI_FMT_DEBUG("查询系统版本失败, 长度溢出len[%d]\n", strlen(pfile));
            }
        }
        memset(buf, 0x0, sizeof(buf));
    }

    if (temp[0] == '\0')
    {
        fclose(pipe);
        return -1; 
    }

    if(temp[strlen(temp)-1] == '\n')
        temp[strlen(temp)-1] = '\0';
    
    fclose(pipe);
    return 0;
}

int get_sys_ver_cmd(char * temp)
{
    char cmd[200]={0};
    char CMDResult[200]={0};
    
    sprintf(cmd,"version -d");
    memset(CMDResult,0x00,sizeof(CMDResult));
    cmd_execute(cmd,CMDResult);
    if(CMDResult[0] == '\0')
    {
        return -1;
    }

    strcpy(temp, CMDResult);
    if(temp[strlen(temp)-1] == '\n')
        temp[strlen(temp)-1] = '\0';

    return 0;
}

void sys_ver_Draw(HDC hdc)
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
    char pcz[40];
    strcpy(pcz, "系统当前版本");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);
    
    rc.top =rc.bottom+20;
    rc.bottom = rc.top+16;
    memset(pcz, 0, 40);
    if (get_sys_ver(pcz) == 0)
    {
        DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);
    }
#if defined (AREA_CHONGQING) && defined (PRODUCT_CCU)
    else if(get_sys_ver_cmd(pcz) == 0)
    {

       DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);
    }
#endif
    else
    {
        DrawText(hdc, "-----", strlen("-----"), &rc, DT_CENTER|DT_VCENTER);
    }

    return;
}


PLUGIN_INFO g_layer_sys_ver=
{
    0x00A000FF, // 0x00A000FF
    sys_ver_Init,
    NULL,
    sys_ver_Draw,
    NULL,
    NULL,
    NULL
};

#endif

//以下代码为协处理器升级功能
#if DESC("协处理器升级", 1)
unsigned int g_CoUpdateIndex = 0;         //选择结果: 0-升级; 1-返回
unsigned int g_CoUpdateStatus = 0;  //状态    : 0-未开始升级;1-拷贝安装文件;2-目录不存在;
                                    //          3-未找到U盘里的安装目录;4-缺少升级文件;5-正在安装
unsigned int g_CoWaitTimer = 0;


#define CO_UPDATE_STATUS_INIT       0
#define CO_UPDATE_STATUS_COPY       1
#define CO_UPDATE_STATUS_NO_AC_DIR  2
#define CO_UPDATE_STATUS_NO_UP_DIR  3
#define CO_UPDATE_STATUS_NO_BIN     4
#define CO_UPDATE_STATUS_INSTALL    5

#define CO_UPDATE_SRC_FOLDER "/tmp/udisk/coupdate"
#define CO_UPDATE_DST_FOLDER "/data/app/rspSample"
#define CO_UPDATE_DST_FILE   "/data/app/rspSample/update_file.bin"
#define CO_UPDATE_START_FILE   "/data/app/rspSample/data/startupdate"
#define CO_UPDATE_END_FILE   "/data/app/rspSample/data/endupdate"
unsigned int COModuleCopyPkg(char *pkgPath)
{
    char cmd[256];

    if (0 != access(CO_UPDATE_DST_FOLDER, F_OK))
    {
        g_CoUpdateStatus = CO_UPDATE_STATUS_NO_AC_DIR;
        return 1;
    }

    ModuleDeleteDir(CO_UPDATE_DST_FILE);
    ModuleDeleteDir(CO_UPDATE_END_FILE);

    sprintf(cmd, "cp -r %s %s", pkgPath, CO_UPDATE_DST_FILE);
    system(cmd);
    system("sync");
    g_CoUpdateStatus = CO_UPDATE_STATUS_INSTALL;
    g_CoWaitTimer = 0;
    return 0;
}

unsigned int COModuleGetPkgPath(char *pkgPath)
{
    DIR           *dir;
    struct dirent *ent;

    if ((dir = opendir(CO_UPDATE_SRC_FOLDER)) == NULL)
    {
        g_CoUpdateStatus = CO_UPDATE_STATUS_NO_UP_DIR;
        return 1;
    }
    
    while ((ent = readdir(dir)) != NULL)
    {
        if ((strcmp(ent->d_name, ".") == 0) || (strcmp(ent->d_name, "..") == 0))
        {
            continue;
        }
        else if (ent->d_type == 8)    /* 8:文件 */
        {
            sprintf(pkgPath, "%s/%s", CO_UPDATE_SRC_FOLDER, ent->d_name);
            closedir(dir);
            return 0;
        }
    }

    g_CoUpdateStatus = CO_UPDATE_STATUS_NO_BIN;
    closedir(dir);
    return 1;
}

void InitCoModuleUpdateDraw(DWORD dwPluginId)
{
    g_CoUpdateIndex = 1;
    g_CoUpdateStatus = CO_UPDATE_STATUS_INIT;
    g_CoWaitTimer = 0;
}

void COModuleUpdateDraw(HDC hdc)
{
    int nTop = 0;
    int nLeft = 0;
    char buf[200] = {0};
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);

    if (g_CoUpdateStatus == CO_UPDATE_STATUS_COPY)
    {
        nTop = 40;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "拷贝文件中...");
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "耗时约1秒钟");      
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "请不要关闭电源");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "不要拔掉U盘");

        return;
    }

    if (g_CoUpdateStatus == CO_UPDATE_STATUS_INSTALL)
    {
        nTop = 40;
        nLeft = 16;
        sprintf(buf, "正在安装...(%u秒)", g_CoWaitTimer);
        TextOut(hdc, nLeft, nTop, buf);
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "耗时约1分钟");      
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "请不要关闭电源");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "可以拔掉U盘");

        return;
    }

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 36;
    rc.bottom = rc.top + 16;
    const char *pcz = "协处理器升级";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    nTop = 98;
    nLeft = 26;

    if (g_CoUpdateIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "返回");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "返回");
    }

    nLeft = 110;
    if (g_CoUpdateIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "确定");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "确定");
    }

    return;
}

void COModuleUpdateTimer(int nId)
{   
    char buf[200] = {0};
    char ver[128] = {0};
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    /* 1s定时器检查 */
    if (nId != TIMER_MAIN)
    {
        return;
    }

    g_guiMain.nTrunDispTime = 0; /* 在协处理器升级页面，不进入轮显 */
    g_guiMain.nLightonTime = 0;
    g_guiMain.bLighton = TRUE;
    lighton_lcd(); /* 在协处理器页面，屏幕常亮 */

    switch (g_CoUpdateStatus)
    {
        case CO_UPDATE_STATUS_INIT: //未开始升级，初始页面
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case CO_UPDATE_STATUS_COPY: //升级文件拷贝
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case CO_UPDATE_STATUS_NO_AC_DIR: //目录不存在
        {
            MessageBox("rspSample目录不存在！");
            g_CoUpdateStatus = CO_UPDATE_STATUS_INIT;
            break;
        }
        case CO_UPDATE_STATUS_NO_UP_DIR: //未找到U盘里的安装目录
        {
            sprintf(buf, "未找到U盘里的升级目录\n%s\n请检查U盘是否插入", CO_UPDATE_SRC_FOLDER);
            MessageBox(buf);
            g_CoUpdateStatus = CO_UPDATE_STATUS_INIT;
            break;
        }
        case CO_UPDATE_STATUS_NO_BIN: //缺少升级文件
        {
            MessageBox("缺少升级文件！");
            g_CoUpdateStatus = CO_UPDATE_STATUS_INIT;
            break;
        }
        case CO_UPDATE_STATUS_INSTALL: //正在安装
        {
            if ((0 == access(CO_UPDATE_END_FILE, F_OK)) && (0 != access(CO_UPDATE_START_FILE, F_OK)))
            {
                sleep(1);             //延时等待一会儿
                if (get_co_ver(ver) == 0)
                {
                    sprintf(buf, "安装完成！\n查看升级结果\n%s", ver);
                    MessageBox(buf);
                }
                else
                {
                    MessageBox("安装完成！\n请在隐藏菜单中\n查看升级结果");
                }
                g_CoUpdateStatus = CO_UPDATE_STATUS_INIT;
                break;
            }

            if (g_CoWaitTimer <= 1000)
            {
                g_CoWaitTimer++;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            }
            else
            {
                MessageBox("安装超时！\n请在update.log中\n查看升级结果");
                g_CoUpdateStatus = CO_UPDATE_STATUS_INIT;
                g_CoWaitTimer = 0;
            }
            break;
        }
        default: 
        {
            g_CoUpdateStatus = CO_UPDATE_STATUS_INIT;
            break;
        }
    }

}

/* 入参由调用者保证合法性 */
unsigned int COModuleUpdateProc(GUI_COMM *pg_guicomm)
{
    char pkgPath[PATH_MAX_LEN + 1] = {0};
    char ver[128] = {0};
    char msg[128] = {0};

    if (ValidatePwd() == TRUE)
    {
        if (get_co_ver(ver) == 0)
        {
            sprintf(msg, "警告\n升级过程中请不要断电\n当前%s\n是否升级", ver);
            if (FALSE == MessageBox(msg, MB_OKCANCEL))
            {
                return 0;
            }
        }
        else
        {
            if (FALSE == MessageBox("警告\n升级过程中请不要断电\n是否升级", MB_OKCANCEL))
            {
                return 0;
            }
        }
    }
    else
    {
        return 1;
    }

    g_CoUpdateStatus = CO_UPDATE_STATUS_COPY;
    UpdateWindow(pg_guicomm->hMainWnd, TRUE);

    if (COModuleGetPkgPath(pkgPath) != 0)
    {
        return 1;
    }

    if (COModuleCopyPkg(pkgPath) != 0)
    {
        return 1;
    }

    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    return 0;
}

BOOL COModuleUpdateKey(int nKey)
{
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return FALSE;
    }

    /* 未开始升级状态下才能操作菜单 */
    if (g_CoUpdateStatus != CO_UPDATE_STATUS_INIT)
    {
        return TRUE;
    }

    switch (nKey)
    {
        case KEY_UP:
        case KEY_LEFT:
        {
            if (g_CoUpdateIndex > 0)
            {
                g_CoUpdateIndex--;
            }
            else
            {
                g_CoUpdateIndex = 1;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        case KEY_RIGHT:
        {
            if (g_CoUpdateIndex < 1)
            {
                g_CoUpdateIndex++;
            }
            else
            {
                g_CoUpdateIndex = 0;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (g_CoUpdateIndex)
            {
                case 0: /* 返回 */
                {
                    CancelMenu();
                    return TRUE;
                }
                case 1: 
                {
                    COModuleUpdateProc(pg_guicomm);
                    return TRUE;
                }
            }

            return TRUE;
        }
    }

    return FALSE;
}

//协处理器升级
PLUGIN_INFO g_CoUpdate = {
    0x00A000FE,             /* id */
    InitCoModuleUpdateDraw, /* InitLayer */
    NULL,                   /* UninitLayer */
    COModuleUpdateDraw,     /* Draw */
    COModuleUpdateTimer,    /* Timer */
    COModuleUpdateKey,      /* keymsg */
    NULL                    /* ipc */
};
#endif


#if DESC("配置参数",1)
/************************************************************************/
/* 配置参数                                                             */
/************************************************************************/
//char g_czMac[20];
//char g_czIp1[20];
//char g_czIp2[20];
//int g_nIpCount = 0;
// 0 显示页 1设置页
int cfp_page_select = 0;

NOMAL_OAD_T  NormalOad;
OOP_OAD_U EthIPOad2 = { 0x45100400};
uint32 g_GWIP[4] = {0};

void ConfigParaInit(DWORD dwPluginId)
{
    get_addr(LocalAddr);
    memset(&EthIP, 0x00, sizeof(OOP_ETHIP_T));
    EthIPisSet = read_db_noamal_data(g_hUdbCenter, EthIPOad2.value, 0, 0, (uint8*)&EthIP, sizeof(OOP_ETHIP_T));
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x40010200;
    NormalOad.infoNum = 0;
    cfp_page_select=0;
    conFigParaSetIndex = 0;
    EthIPisSet = 1;

    read_pdata_xram("/data/app/desktopGui/data/radio", (char*)&g_terminal_addr.czCode, 0, 2);
    read_pdata_xram("/data/app/desktopGui/data/radio", (char*)&g_terminal_addr.czAddr, 2, 3);

    char cmd[200];
    char result[100];
    memset(cmd,0x00,sizeof(cmd));
    memset(result,0x00,sizeof(result));
    memset(g_GWIP,0x00,sizeof(g_GWIP));
    sprintf(cmd, "ifconfig %s | grep 'inet addr' | awk '{ print $2}' | awk -F: '{print $2}' ", "ppp0");
    cmd_execute(cmd,result);
    sscanf(result,"%u.%u.%u.%u",(uint32*)&g_GWIP[0],(uint32*)&g_GWIP[1],(uint32*)&g_GWIP[2],(uint32*)&g_GWIP[3]);
}
void ConfigParaDraw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
     ITEM_NOT_SELECT;
     RECT rc;
     rc.left = 0;
     rc.right = g_guiComm.nWidth;
     rc.top = 20;
     rc.bottom = rc.top + 16;

    char czBuf[64] = { 0 };

    sprintf(czBuf, "%s", (cfp_page_select == 0)?"显示模式":"设置模式");
    if( conFigParaSetIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, 60, 20, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 60, 20, czBuf);
    }
    
    SelectFont(hdc,g_guiComm.fontSmall);
    
    // 行政区码
    int nLeft = 80;
    int nTop = 44;
    TextOut(hdc, 1, nTop, "行政区码");
    sprintf(czBuf, "%02x%02x", g_terminal_addr.czCode[0], g_terminal_addr.czCode[1]);
    if(cfp_page_select == 1 && conFigParaSetIndex == 1)
        {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, czBuf);
    }
        
    // 终端地址
    nTop += 18;
    TextOut(hdc, 1, nTop, "终端地址");
    sprintf(czBuf, "%02x%02x%02x", g_terminal_addr.czAddr[0], g_terminal_addr.czAddr[1], g_terminal_addr.czAddr[2]);
    if(cfp_page_select == 1 && conFigParaSetIndex == 2)
        {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, czBuf);
    }
    
    // OOP终端地址
    nTop += 18;
    sprintf(czBuf, "OOP终端地址");
    TextOut(hdc, 1, nTop, czBuf);
    memset(czBuf, 0, 64);
    if(cfp_page_select == 1 && conFigParaSetIndex == 3)
    {
        ITEM_SELECT;
        sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], 
                                                 LocalAddr[4], LocalAddr[5]);
        TextOut(hdc, nLeft, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], 
                                                 LocalAddr[4], LocalAddr[5]);
        TextOut(hdc, nLeft, nTop, czBuf);
    }
    
    //公网IP
    nTop += 36;
    TextOut(hdc, 0, nTop, "公网IP");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (g_GWIP[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", g_GWIP[0], g_GWIP[1], g_GWIP[2], g_GWIP[3]);
    }
    TextOut(hdc, nLeft-24, nTop, czBuf);
}
void ConfigParaReturn(char* pNewStr, void* pPara)
{
    OOP_ETHIP_T *pethIP;
    int *pEthIPisSet = &EthIPisSet;
    pethIP = &EthIP;
    uint8 iptmp[4] = { 0 };
    char czBuf[20] = { 0 };
   // uint8 tmp[4] = { 0 };

    switch(conFigParaSetIndex)
    {
        case 2:
        {
           //终端地址

        }
    break;
        case 1:
        {
            // 行政区码长度
            int nLen = strlen(pNewStr);
            if(nLen > 4 && TRUE == MessageBox("行政区码长度大于4字节\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                
            }
            else
            {
                
            }
        }
    break;
    case 3:
        {// OOP地址
            int nLen = strlen(pNewStr);
            #ifdef AREA_ZHEJIANG
            int ret = 0;
            if (nLen > 8 && TRUE == MessageBox("OOP终端地址长度大于4字节\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                char tmp[20] = { 0 };
                sprintf(tmp, "%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
                InputBox(tmp, ConfigParaReturn, NULL, 0x01);
             }
            else
            {
                char tmp[20] = { 0 };
                strcpy(tmp, "00000000");
                strcpy(tmp + 8 - nLen, pNewStr);
                printf("输入的地址为 %s \n", tmp);
                commfun_ASC2BCD((char*)LocalAddr, tmp, 8, 0);
                printf("转换为bcd %02x%02x%02x%02x \n", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
                OOP_OCTETVAR16_T TmpLocalAddr;
                memset(&TmpLocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
                TmpLocalAddr.nNum = 4;
                memcpy(TmpLocalAddr.value, LocalAddr, 4);
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
            #else
            if (nLen > 12 && TRUE == MessageBox("OOP终端地址长度大于6字节\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                char tmp[20] = { 0 };
                sprintf(tmp, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
                InputBox(tmp, ConfigParaReturn, NULL, 0x01);
            }
            else
            {
                char tmp[20] = { 0 };
                strcpy(tmp, "000000000000");
                strcpy(tmp + 12 - nLen, pNewStr);
                printf("输入的地址为 %s \n", tmp);
                commfun_ASC2BCD((char*)LocalAddr, tmp, 12, 0);
                printf("转换为bcd %02x%02x%02x%02x%02x%02x \n", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
                OOP_OCTETVAR16_T TmpLocalAddr;
                memset(&TmpLocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
                TmpLocalAddr.nNum = 6;
                memcpy(TmpLocalAddr.value, LocalAddr, 6);
                OOP_OAD_U oad = { 0x40010200 };
                int ret = write_normal_data((uint8*)&TmpLocalAddr, sizeof(OOP_OCTETVAR16_T), oad, 0, 0, 1);
                if (ret == 0)
                {
                    MessageBox("终端地址设置成功");
                }
                else
                {
                    MessageBox("终端地址设置失败");
                }
            }
            #endif
        }
    break;
    case 4 :
    {
        if (FALSE == commfun_IP(pNewStr, iptmp))
        {
            if (TRUE == MessageBox("IP地址格式错误\n\n确认是否重新输入?", MB_OKCANCEL))
            {
                if (pethIP->ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", pethIP->ip[0], pethIP->ip[1], pethIP->ip[2], pethIP->ip[3]);
                }
                InputBox(czBuf, ConfigParaReturn, NULL, 0x02);
              
            }
        }
       else
        {
            memcpy(pethIP->ip, iptmp, 4);
            *pEthIPisSet = 1;
            //  OOP_OAD_U EthIPOad = { 0x45100400 };
            // OOP_OAD_U EthIPOad2 = { 0x45110400}; 
            int ret = write_normal_data((uint8*)&EthIP, sizeof(OOP_ETHIP_T), EthIPOad2, 0, 0, 1);
            if (ret != 0)
            {
                MessageBox("IP地址配置写入失败！");
            }
            else
            {
                MessageBox("IP地址配置写入成功！");

                if(0 != ipconfig_send_to_smiOS((char*)"FE0", EthIP.ipConfig, EthIP.ip, EthIP.mask, EthIP.ipGateway))
                {
                    MessageBox("设置IP地址失败！");
                }
                uint8 flag = 0;
                OOP_OAD_U oadnum = { 0x451004FF };

                write_normal_data(&flag, sizeof(uint8), oadnum, 0, 0, 1);
            }
            CancelMenu();
            conFigParaSetIndex = 0;
        }
        break;
    }
}
}
/*
BOOL ConfigParaSetKeymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
    {
        return FALSE;
    }
//    uint8 buf[100]={0};
//    uint8 buflen=0;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
       
        return TRUE;
    }
    switch (nKey)
    {
    case KEY_LEFT:
    case KEY_UP:
   
    case KEY_RIGHT:
    case KEY_OK:
    case KEY_DOWN:
    {
        page_select = 1;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    
    }
    return FALSE;
    
}
*/
BOOL ConfigParaSetKeymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
    {
        return FALSE;
    }
    
    switch(nKey)
    {
    case KEY_UP:
        conFigParaSetIndex --;
        if(conFigParaSetIndex < 0)
        {
            if(cfp_page_select == 0)
            {
                conFigParaSetIndex = 0;
            }
            else
            {
                conFigParaSetIndex = 3;
            }
        }
       UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_DOWN:
        if(cfp_page_select == 0)
        {
            conFigParaSetIndex = 0;
        }
        else
        {
            conFigParaSetIndex ++;
            if(conFigParaSetIndex > 3)
            {
                conFigParaSetIndex = 0;
            }
        }
       UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
#ifdef AREA_SHANXI
            MessageBox("山西终端通信地址\n不支持设置!");
            return TRUE;
#endif
#ifdef AREA_HENAN
            MessageBox("河南终端通信地址\n不支持设置!");
            return TRUE;
#endif
#ifdef AREA_JIANGXI
            MessageBox("江西终端通信地址\n不支持设置!");
            return TRUE;
#endif
#ifdef AREA_SICHUAN
            MessageBox("四川终端通信地址\n不支持设置!");
            return TRUE;
#endif
#ifdef AREA_HUNAN
            MessageBox("湖南终端通信地址\n不支持设置!");
            return TRUE;
#endif
        if(ValidatePwd() == TRUE)
        {
            // 确认处理
            switch(conFigParaSetIndex)
            {
            case 0:
                if(cfp_page_select == 0)
                {
#ifndef AREA_HUNAN
                    cfp_page_select = 1;
#else
                    cfp_page_select = 0;
#endif
                }
                else
                {
                    cfp_page_select = 0;
                }
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                break;
                // 弹出终端地址输入页面库
            case 1:
                {
                char tmp[20] = {0};
                sprintf(tmp, "%02x%02x", g_terminal_addr.czCode[0], g_terminal_addr.czCode[1]);
                InputBox(tmp, AddrSetReturn, NULL, 0x01);
                 break;
                }
                // 弹出行政区码输入页面库
            case 2:
                {
                char tmp[20] = {0};
                sprintf(tmp, "%02x%02x%02x", g_terminal_addr.czAddr[0], g_terminal_addr.czAddr[1], g_terminal_addr.czAddr[2]);
                    
                InputBox(tmp, AddrSetReturn, NULL, 0x01);
                break;
                }
            case 3:
                {
                 char tmp[20] = {0};
                #ifdef AREA_ZHEJIANG
                sprintf(tmp, "%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
                #else
                sprintf(tmp, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
                #endif
                InputBox(tmp, AddrSetReturn, NULL, 0x01);
                break;
                }
            }
        }
        return TRUE;
    case KEY_CANCEL:
#ifdef AREA_SHANXI
        return FALSE;
#endif
#ifdef AREA_JIANGXI
        return FALSE;
#endif
#ifdef AREA_HENAN
        return FALSE;
#endif
#ifndef AREA_HUNAN
        if(TRUE == MessageBox("当前配置没有保存\n\n是否确定保存配置？", MB_OKCANCEL))
        {
            addr_send_to_radio((uint8*)g_terminal_addr.czCode, (uint8*)g_terminal_addr.czAddr, LocalAddr);
            write_pdata_xram("/data/app/desktopGui/data/radio", (char*)&g_terminal_addr.czCode, 0, 2);
            write_pdata_xram("/data/app/desktopGui/data/radio", (char*)&g_terminal_addr.czAddr, 2, 3);
            MessageBox("终端地址设置成功");
        }
#endif
        return FALSE;
    default: break;
    }
    return FALSE;
}

PLUGIN_INFO g_layer_config_para = {
    0x20720000/*id*/,
    ConfigParaInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    ConfigParaDraw/*Draw*/,
    NULL/*Timer*/,
    ConfigParaSetKeymsg/*keymsg*/,
    NULL/*ipc*/
};


/************************************************************************/
/* 配置参数结束                                                         */
/************************************************************************/
#endif

#if DESC("控制参数",1)
uint8 g_GuiCtrlGroup = 0;
uint8 g_GuiPeriodPlan = 0;
OOP_8103_CONFIG_T   g_GuiPeriodParam;           //0x81030200 时段控
OOP_8104_CONFIG_T   g_GuiRestParam;             //0x81040200 厂休控
OOP_8105_CONFIG_T   g_GuiStopParam;             //0x81050200 报停控
OOP_POWCON_LOW_T    g_GuiDownParam[OOP_MAX_TG]; //0x81067fff 下浮控
OOP_8108_CONFIG_T   g_GuiMonthParam;              //0x81080200 月电控 

OOP_POWCON_TIME_T ptPowconTime;
PCSEG_INFO_T g_PCSegData[16]; //-8个功控时段的分布 保险一点翻倍-
uint8        g_bPCSegNo = 0; //-功控时段数量-
uint8 ctrl_PageIndex;    //时段控多页显示上下翻页
uint8 appctrl_cal_period()//-计算功控时段-
{
    memset(&ptPowconTime ,0x0,sizeof(OOP_POWCON_TIME_T));
    int ret = read_db_noamal_data(g_hUdbCenter, 0x81010200, 0, 0, (uint8*)&ptPowconTime, sizeof(OOP_POWCON_TIME_T));
    if(0 != ret)
    {
        GUI_FMT_DEBUG("Get PowconTime time 0x%x failed, ret %d!\n", 0x81010200, ret);
    }
    uint8 aBuf[12] = {0};
    uint8 ctrlState[48] = {0};
    uint8 i,j;        //-循环变量-
    uint8 k = 0;
    char temp[400] = {0};
    uint16 offset = 0;

    memcpy(aBuf, ptPowconTime.period, sizeof(ptPowconTime.period));
    for(i = 0; i < 12; i++)
    {
        for(j = 0; j < 4; j++)
        {
            ctrlState[k] = ((aBuf[i] >> (2 * j)) & 0x03);
            if(3 == ctrlState[k])
            {
                ctrlState[k] = 0;
            }
            k++;
        }
    }
    memset(g_PCSegData ,0x0,sizeof(g_PCSegData));
    g_bPCSegNo = 0;
    
    g_PCSegData[0].home = 0;
    g_PCSegData[0].end = 30;
    g_PCSegData[0].index = ctrlState[0];

    for(k = 0; k < 48; k++)
    {
        if(g_PCSegData[g_bPCSegNo].index != ctrlState[k])
        {
            g_PCSegData[g_bPCSegNo].end = 30 * k;
            g_bPCSegNo++;
            g_PCSegData[g_bPCSegNo].home = 30 * k;
            g_PCSegData[g_bPCSegNo].index = ctrlState[k];
        }
    }
    g_PCSegData[g_bPCSegNo].end = 30 * k;
    g_bPCSegNo++;

    for(i = 0; i < g_bPCSegNo; i++)
    {
        offset += sprintf(temp + offset, " (%d %d~%d, %d)", i + 1, g_PCSegData[i].home, g_PCSegData[i].end, 
                          g_PCSegData[i].index);
    }
    return g_bPCSegNo;
}

void gruop_data_display_uninit()
{
   set_gui_infonum(0);
   return ;
}

void ctrl_period_param_display_init(DWORD dwPluginId)
{
    g_GuiCtrlGroup = (dwPluginId) & 0x0F;
    set_gui_infonum(g_GuiCtrlGroup + 1);
    g_GuiPeriodPlan = 0;

    memset(&g_GuiPeriodParam, 0, sizeof(OOP_8103_CONFIG_T));
    read_db_noamal_data(g_hUdbCenter, 0x81030200, 0, 0, (uint8*)&g_GuiPeriodParam, sizeof(OOP_8103_CONFIG_T));
    appctrl_cal_period();
    ctrl_PageIndex = 0;
}

void ctrl_rest_param_display_init(DWORD dwPluginId)
{
    g_GuiCtrlGroup = (dwPluginId) & 0x0F;
    set_gui_infonum(g_GuiCtrlGroup + 1);

    memset(&g_GuiRestParam, 0, sizeof(OOP_8104_CONFIG_T));
    read_db_noamal_data(g_hUdbCenter, 0x81040200, 0, 0, (uint8*)&g_GuiRestParam, sizeof(OOP_8104_CONFIG_T));
}

void ctrl_stop_param_display_init(DWORD dwPluginId)
{
    g_GuiCtrlGroup = (dwPluginId) & 0x0F;
    set_gui_infonum(g_GuiCtrlGroup + 1);

    memset(&g_GuiStopParam, 0, sizeof(OOP_8105_CONFIG_T));
    read_db_noamal_data(g_hUdbCenter, 0x81050200, 0, 0, (uint8*)&g_GuiStopParam, sizeof(OOP_8105_CONFIG_T));
}

void ctrl_down_param_display_init(DWORD dwPluginId)
{
    g_GuiCtrlGroup = (dwPluginId) & 0x0F;
    set_gui_infonum(g_GuiCtrlGroup + 1);

    memset(&g_GuiDownParam[g_GuiCtrlGroup], 0, sizeof(OOP_POWCON_LOW_T));
    read_db_noamal_data(g_hUdbCenter, 0x81067fff, 0, g_GuiCtrlGroup + 1, (uint8*)&g_GuiDownParam[g_GuiCtrlGroup], 
sizeof(OOP_POWCON_LOW_T));
}

void ctrl_month_param_display_init(DWORD dwPluginId)
{
    g_GuiCtrlGroup = (dwPluginId) & 0x0F;
    set_gui_infonum(g_GuiCtrlGroup + 1);

    memset(&g_GuiMonthParam, 0, sizeof(OOP_8108_CONFIG_T));
    read_db_noamal_data(g_hUdbCenter, 0x81080200, 0, 0, (uint8*)&g_GuiMonthParam, sizeof(OOP_8108_CONFIG_T));
}

void ctrl_period_param_display_draw(HDC hdc)
{
    char czBuf[100];
    char czBuf1[100];
    uint16 offset = 0;
    int nTop = 32;
    int nLeft = 1;
    int tmpTop = 16;
    uint32 i = 0;
    OOP_CFGUNIT_8109_T* ptCfg = NULL;
    char* PlanName[] = { (char*)"一", (char*)"二", (char*)"三" };
    int64 tempnum = 0;
    int64 tempnum1 = 0;

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "时段控参数 总加组%d", g_GuiCtrlGroup + 1);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    /* 检索是否存在配置 */
    for (i = 0; i < g_GuiPeriodParam.nNum; i++)
    {
        if (0x2301 + g_GuiCtrlGroup == g_GuiPeriodParam.item[i].object)
        {
            ptCfg = &(g_GuiPeriodParam.item[i]);
            break;
        }
    }

    if (NULL == ptCfg)
    {
        nTop += 16;
        TextOut(hdc, nLeft + 20, nTop + 10, "未配置方案");
        return;
    }

    SelectFont(hdc, g_guiComm.fontSmall);
    nTop += 16;
    sprintf(czBuf, "浮动系数   %d", ptCfg->floatCoe);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    if (0 == ((ptCfg->plan >> g_GuiPeriodPlan) & 0x01))
    {
        sprintf(czBuf, "第%s方案无效", PlanName[g_GuiPeriodPlan]);
        TextOut(hdc, nLeft, nTop, czBuf);
        return;
    }
    sprintf(czBuf, "第%s方案", PlanName[g_GuiPeriodPlan]);
    TextOut(hdc, nLeft, nTop, czBuf);

    /* 下面绘制时段定值 */
    offset = 0;
    char PowconTime[40];
    if (ctrl_PageIndex == 0)
    {
        for (i = 0; i < 4; i++)
        {
            if (g_PCSegData[i].index == 0)
                sprintf(PowconTime, "[%02d:%02d~%02d:%02d]", g_PCSegData[i].home/60, g_PCSegData[i].home%60, g_PCSegData[i].end/60, g_PCSegData[i].end%60);
            else
                sprintf(PowconTime, "[%02d:%02d~%02d:%02d]", g_PCSegData[i].home/60, g_PCSegData[i].home%60, g_PCSegData[i].end/60, g_PCSegData[i].end%60);
            if (((ptCfg->value[g_GuiPeriodPlan].valid >> i) & 0x01)&&(g_PCSegData[i].index != 0))
            {
                tempnum = ptCfg->value[g_GuiPeriodPlan].defValue[i] / (10000L);
                tempnum1 = ptCfg->value[g_GuiPeriodPlan].defValue[i] - tempnum * (10000L);
                if (ptCfg->value[g_GuiPeriodPlan].defValue[i] > 90000000
                    || ptCfg->value[g_GuiPeriodPlan].defValue[i] < -90000000)
                {
                    if (ptCfg->value[g_GuiPeriodPlan].defValue[i] < 0)
                    {
                        tempnum1 = 0 - tempnum1;
                    }
                    //offset += sprintf(czBuf + offset, "%s:%8lld.%1lld", PowconTime, tempnum, tempnum1 / 1000);
//                    double tempDefVal= tempnum+ tempnum1 *0.0001;
//                    offset += sprintf(czBuf + offset, "%s: %-9.4f", PowconTime, tempDefVal);  
                    valueDisplayProcess_strUnit(ptCfg->value[g_GuiPeriodPlan].defValue[i], czBuf1, 100, 4, "W");
                    offset += sprintf(czBuf + offset, "%s:", PowconTime);
                    offset += sprintf(czBuf + offset, "%s", czBuf1);  
                }
                else
                {
                    if (ptCfg->value[g_GuiPeriodPlan].defValue[i] < 0)
                    {
                        tempnum1 = 0 - tempnum1;
                    }
                    //offset += sprintf(czBuf + offset, "%s:%5lld.%04lld", PowconTime, tempnum, tempnum1);
//                    double tempDefVal= tempnum+ tempnum1 *0.0001;
//                    offset += sprintf(czBuf + offset, "%s: %-9.4f", PowconTime, tempDefVal);  

                    valueDisplayProcess_strUnit(ptCfg->value[g_GuiPeriodPlan].defValue[i], czBuf1, 100, 4, "W");
                    offset += sprintf(czBuf + offset, "%s:", PowconTime);
                    offset += sprintf(czBuf + offset, "%s", czBuf1);                    
                }
            }
            else
            {
                offset += sprintf(czBuf + offset, "%s:      ----", PowconTime);
            }
            nTop += tmpTop;
            TextOut(hdc, 3, nTop, czBuf);
            offset = 0;
        }
        sprintf(czBuf, "下一页");
    }
    else{
        for (i = 4; i < 8; i++)
        {
            if (g_PCSegData[i].index == 0)
                sprintf(PowconTime, "[%02d:%02d~%02d:%02d]", g_PCSegData[i].home/60, g_PCSegData[i].home%60, g_PCSegData[i].end/60, g_PCSegData[i].end%60);
            else
                sprintf(PowconTime, "[%02d:%02d~%02d:%02d]", g_PCSegData[i].home/60, g_PCSegData[i].home%60, g_PCSegData[i].end/60, g_PCSegData[i].end%60);
            if (((ptCfg->value[g_GuiPeriodPlan].valid >> i) & 0x01)&&(g_PCSegData[i].index != 0))
            {
                tempnum = ptCfg->value[g_GuiPeriodPlan].defValue[i] / (10000L);
                tempnum1 = ptCfg->value[g_GuiPeriodPlan].defValue[i] - tempnum * (10000L);
                if (ptCfg->value[g_GuiPeriodPlan].defValue[i] > 90000000
                    || ptCfg->value[g_GuiPeriodPlan].defValue[i] < -90000000)
                {
                    if (ptCfg->value[g_GuiPeriodPlan].defValue[i] < 0)
                    {
                        tempnum1 = 0 - tempnum1;
                    }
                    //offset += sprintf(czBuf + offset, "%s:%8lld.%1lld", PowconTime, tempnum, tempnum1 / 1000);
//                    double tempDefVal= tempnum+ tempnum1 *0.0001;
//                    offset += sprintf(czBuf + offset, "%s: %-9.4f", PowconTime, tempDefVal); 
                    valueDisplayProcess_strUnit(ptCfg->value[g_GuiPeriodPlan].defValue[i], czBuf1, 100, 4, "W");
                    offset += sprintf(czBuf + offset, "%s:", PowconTime);
                    offset += sprintf(czBuf + offset, "%s", czBuf1);  
                }
                else
                {
                    if (ptCfg->value[g_GuiPeriodPlan].defValue[i] < 0)
                    {
                        tempnum1 = 0 - tempnum1;
                    }
                    //offset += sprintf(czBuf + offset, "%s:%5lld.%04lld", PowconTime, tempnum, tempnum1);
//                    double tempDefVal= tempnum+ tempnum1 *0.0001;
//                    offset += sprintf(czBuf + offset, "%s: %-9.4f", PowconTime, tempDefVal);     
                    valueDisplayProcess_strUnit(ptCfg->value[g_GuiPeriodPlan].defValue[i], czBuf1, 100, 4, "W");
                    offset += sprintf(czBuf + offset, "%s:", PowconTime);
                    offset += sprintf(czBuf + offset, "%s", czBuf1);  
                }
            }
            else
            {
                offset += sprintf(czBuf + offset, "%s:      ----", PowconTime);
            }
            nTop += tmpTop;
            TextOut(hdc, 3, nTop, czBuf);
            offset = 0;
        }
        sprintf(czBuf, "上一页");
    }
    ITEM_SELECT;
    TextOut(hdc, 110, 64, czBuf);
    ITEM_NOT_SELECT;
}

void ctrl_rest_param_display_draw(HDC hdc)
{
    char czBuf[100];
    int nTop = 32;
    int nLeft = 5;
    int tmpTop = 16;
    uint32 i = 0;
    OOP_CFGUNIT_810A_T* ptCfg = NULL;
    int64  tempnum = 0;
    char* PlanName[] = {(char*)"一", (char*)"二", (char*)"三", (char*)"四", (char*)"五", (char*)"六", (char*)"日" };

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "厂休控参数 总加组%d", g_GuiCtrlGroup + 1);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    /* 检索是否存在配置 */
    for (i = 0; i < g_GuiRestParam.nNum; i++)
    {
        if (0x2301 + g_GuiCtrlGroup == g_GuiRestParam.item[i].object)
        {
            ptCfg = &(g_GuiRestParam.item[i]);
            break;
        }
    }

    if (NULL == ptCfg)
    {
        nTop += 16;
        TextOut(hdc, nLeft + 20, nTop + 10, "未配置方案");
        return;
    }

    SelectFont(hdc, g_guiComm.fontSmall);
    nTop += 16;
    tempnum = ptCfg->defValue - ((ptCfg->defValue / 10L) * 10L);
    if (ptCfg->defValue < 0)
    {
        tempnum = 0 - tempnum;
    }

    sprintf(czBuf, "定值  %lld.%lld W", ptCfg->defValue / 10, tempnum);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "起始限电时间 %02d:%02d:%02d", ptCfg->startTime.hour, ptCfg->startTime.minute, 
                                                     ptCfg->startTime.second);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "限电延续时间 %d min", ptCfg->duration);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "每周限电日");
    nTop += tmpTop;
    for (i = 1; i < 8; i++)
    {
        if ((ptCfg->powLimit.nValue >> i) & 0x01)
        {
            TextOut(hdc, 30 + 16 * (i - 1), nTop, PlanName[i - 1]);
        }
    }
}

void ctrl_stop_param_display_draw(HDC hdc)
{
    char czBuf[100];
    int nTop = 32;
    int nLeft = 5;
    int tmpTop = 16;
    uint32 i = 0;
    OOP_CFGUNIT_810B_T* ptCfg = NULL;
    int64  tempnum = 0;

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "报停控参数 总加组%d", g_GuiCtrlGroup + 1);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    /* 检索是否存在配置 */
    for (i = 0; i < g_GuiStopParam.nNum; i++)
    {
        if (0x2301 + g_GuiCtrlGroup == g_GuiStopParam.item[i].object)
        {
            ptCfg = &(g_GuiStopParam.item[i]);
            break;
        }
    }

    if (NULL == ptCfg)
    {
        nTop += 16;
        TextOut(hdc, nLeft + 20, nTop + 10, "未配置方案");
        return;
    }

    SelectFont(hdc, g_guiComm.fontSmall);
    nTop += 16;
    tempnum = ptCfg->defValue - ((ptCfg->defValue / 10L) * 10L);
    if (ptCfg->defValue < 0)
    {
        tempnum = 0 - tempnum;
    }

    sprintf(czBuf, "定值  %lld.%lld W", ptCfg->defValue / 10, tempnum);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "起始时间");
    nTop += tmpTop;
    sprintf(czBuf, "%04d年%02d月%02d日", ptCfg->startTime.year, ptCfg->startTime.month, ptCfg->startTime.day);
    TextOut(hdc, nLeft, nTop, czBuf);
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "结束时间");
    nTop += tmpTop;
    sprintf(czBuf, "%04d年%02d月%02d日", ptCfg->endTime.year, ptCfg->endTime.month, ptCfg->endTime.day);
    TextOut(hdc, nLeft, nTop, czBuf);
}

void ctrl_down_param_display_draw(HDC hdc)
{
    char czBuf[100];
    uint8 offset = 0;
    int nTop = 32;
    int nLeft = 5;
    int tmpTop = 14;
    uint8 i = 0;
    uint8 turnMax = 0;
    LCONSCHEME* ptCfg = NULL;
    char* turnName[] = {(char*)"一轮", (char*)"二轮", (char*)"三轮", (char*)"四轮",
                        (char*)"五轮", (char*)"六轮", (char*)"七轮", (char*)"八轮" };

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "下浮控参数 总加组%d", g_GuiCtrlGroup + 1);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    /* 检索是否存在配置 */
    if (0x2301 + g_GuiCtrlGroup != g_GuiDownParam[g_GuiCtrlGroup].object)
    {
        nTop += 16;
        TextOut(hdc, nLeft + 20, nTop + 10, "未配置方案");
        return;
    }

    SelectFont(hdc, g_guiComm.fontSmall);
    ptCfg = &(g_GuiDownParam[g_GuiCtrlGroup].scheme);

    nTop += tmpTop;
    sprintf(czBuf, "滑差时间   %d min", ptCfg->slipTime);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "浮动系数   %d%%", ptCfg->floatCoefficient);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "冻结延时   %d min", ptCfg->delayTime);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "控制时间   %d min", ptCfg->conTime * ((uint16)30));
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "告警时间(单位min):");
    TextOut(hdc, nLeft, nTop, czBuf);
    turnMax = 4;

    for (i = 0; i < turnMax; i++)
    {
        offset += sprintf(czBuf + offset, "  %s %3d", turnName[i], ptCfg->waringTime[i]);
        if (0 == i % 2)
        {
            offset += sprintf(czBuf + offset, "   ");
        }
        else
        {
            nTop += tmpTop;
            TextOut(hdc, nLeft, nTop, czBuf);
            offset = 0;
        }
    }
}

void ctrl_month_param_display_draw(HDC hdc)
{
    char czBuf[100];
    int nTop = 32;
    int nLeft = 5;
    int tmpTop = 16;
    uint32 i = 0;
    OOP_CFGUNIT_810D_T* ptCfg = NULL;
    int64  tempnum = 0;

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "月电控参数 总加组%d", g_GuiCtrlGroup + 1);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    /* 检索是否存在配置 */
    for (i = 0; i < g_GuiMonthParam.nNum; i++)
    {
        if (0x2301 + g_GuiCtrlGroup == g_GuiMonthParam.item[i].object)
        {
            ptCfg = &(g_GuiMonthParam.item[i]);
            break;
        }
    }

    if (NULL == ptCfg)
    {
        nTop += 16;
        TextOut(hdc, nLeft + 20, nTop + 10, "未配置方案");
        return;
    }

    SelectFont(hdc, g_guiComm.fontSmall);
    nTop += 16;
    tempnum = ptCfg->defValue - ((ptCfg->defValue / 10000L) * 10000L);
    if (ptCfg->defValue < 0)
    {
        tempnum = 0 - tempnum;
    }

    sprintf(czBuf, "定值     %lld.%04lld kWh", ptCfg->defValue / 10000L, tempnum);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "门限系数  %d%%", ptCfg->limitCoe);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "浮动系数  %d%%", ptCfg->floatCoe);
    TextOut(hdc, nLeft, nTop, czBuf);
}

BOOL ctrl_period_param_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_RIGHT:
    {
        if (g_GuiPeriodPlan >= 2)
        {
            g_GuiPeriodPlan = 0;
        }
        else
        {
            g_GuiPeriodPlan++;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_LEFT:
    {
        if (0 == g_GuiPeriodPlan)
        {
            g_GuiPeriodPlan = 2;
        }
        else
        {
            g_GuiPeriodPlan--;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
        {
            if(ctrl_PageIndex == 0)
            {
                ctrl_PageIndex = 1;

            }else{
                 ctrl_PageIndex = 0;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
        }
    default:
        return FALSE;
    }

}


//时段控参数查看
PLUGIN_INFO g_layer_ctrl_period_display1 = {
    0x11031010/*id*/,
    ctrl_period_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_period_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_period_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};
//时段控参数查看
PLUGIN_INFO g_layer_ctrl_period_display2 = {
    0x11031011/*id*/,
    ctrl_period_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_period_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_period_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};
//时段控参数查看
PLUGIN_INFO g_layer_ctrl_period_display3 = {
    0x11031012/*id*/,
    ctrl_period_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_period_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_period_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};
//时段控参数查看
PLUGIN_INFO g_layer_ctrl_period_display4 = {
    0x11031013/*id*/,
    ctrl_period_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_period_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_period_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};
//时段控参数查看
PLUGIN_INFO g_layer_ctrl_period_display5 = {
    0x11031014/*id*/,
    ctrl_period_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_period_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_period_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};
//时段控参数查看
PLUGIN_INFO g_layer_ctrl_period_display6 = {
    0x11031015/*id*/,
    ctrl_period_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_period_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_period_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};
//时段控参数查看
PLUGIN_INFO g_layer_ctrl_period_display7 = {
    0x11031016/*id*/,
    ctrl_period_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_period_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_period_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};
//时段控参数查看
PLUGIN_INFO g_layer_ctrl_period_display8 = {
    0x11031017/*id*/,
    ctrl_period_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_period_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_period_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};

//厂休控参数查看
PLUGIN_INFO g_layer_ctrl_rest_display1 = {
    0x11031020/*id*/,
    ctrl_rest_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_rest_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//厂休控参数查看
PLUGIN_INFO g_layer_ctrl_rest_display2 = {
    0x11031021/*id*/,
    ctrl_rest_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_rest_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//厂休控参数查看
PLUGIN_INFO g_layer_ctrl_rest_display3 = {
    0x11031022/*id*/,
    ctrl_rest_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_rest_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//厂休控参数查看
PLUGIN_INFO g_layer_ctrl_rest_display4 = {
    0x11031023/*id*/,
    ctrl_rest_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_rest_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//厂休控参数查看
PLUGIN_INFO g_layer_ctrl_rest_display5 = {
    0x11031024/*id*/,
    ctrl_rest_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_rest_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//厂休控参数查看
PLUGIN_INFO g_layer_ctrl_rest_display6 = {
    0x11031025/*id*/,
    ctrl_rest_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_rest_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//厂休控参数查看
PLUGIN_INFO g_layer_ctrl_rest_display7 = {
    0x11031026/*id*/,
    ctrl_rest_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_rest_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//厂休控参数查看
PLUGIN_INFO g_layer_ctrl_rest_display8 = {
    0x11031027/*id*/,
    ctrl_rest_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_rest_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//报停控参数查看
PLUGIN_INFO g_layer_ctrl_stop_display1 = {
    0x11031030/*id*/,
    ctrl_stop_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_stop_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//报停控参数查看
PLUGIN_INFO g_layer_ctrl_stop_display2 = {
    0x11031031/*id*/,
    ctrl_stop_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_stop_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//报停控参数查看
PLUGIN_INFO g_layer_ctrl_stop_display3 = {
    0x11031032/*id*/,
    ctrl_stop_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_stop_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//报停控参数查看
PLUGIN_INFO g_layer_ctrl_stop_display4 = {
    0x11031033/*id*/,
    ctrl_stop_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_stop_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//报停控参数查看
PLUGIN_INFO g_layer_ctrl_stop_display5 = {
    0x11031034/*id*/,
    ctrl_stop_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_stop_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//报停控参数查看
PLUGIN_INFO g_layer_ctrl_stop_display6 = {
    0x11031035/*id*/,
    ctrl_stop_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_stop_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//报停控参数查看
PLUGIN_INFO g_layer_ctrl_stop_display7 = {
    0x11031036/*id*/,
    ctrl_stop_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_stop_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//报停控参数查看
PLUGIN_INFO g_layer_ctrl_stop_display8 = {
    0x11031037/*id*/,
    ctrl_stop_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_stop_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//下浮控参数查看
PLUGIN_INFO g_layer_ctrl_down_display1 = {
    0x11031040/*id*/,
    ctrl_down_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_down_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//下浮控参数查看
PLUGIN_INFO g_layer_ctrl_down_display2 = {
    0x11031041/*id*/,
    ctrl_down_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_down_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//下浮控参数查看
PLUGIN_INFO g_layer_ctrl_down_display3 = {
    0x11031042/*id*/,
    ctrl_down_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_down_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//下浮控参数查看
PLUGIN_INFO g_layer_ctrl_down_display4 = {
    0x11031043/*id*/,
    ctrl_down_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_down_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//下浮控参数查看
PLUGIN_INFO g_layer_ctrl_down_display5 = {
    0x11031044/*id*/,
    ctrl_down_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_down_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//下浮控参数查看
PLUGIN_INFO g_layer_ctrl_down_display6 = {
    0x11031045/*id*/,
    ctrl_down_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_down_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//下浮控参数查看
PLUGIN_INFO g_layer_ctrl_down_display7 = {
    0x11031046/*id*/,
    ctrl_down_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_down_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//下浮控参数查看
PLUGIN_INFO g_layer_ctrl_down_display8 = {
    0x11031047/*id*/,
    ctrl_down_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_down_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//月电控参数查看
PLUGIN_INFO g_layer_ctrl_month_display1 = {
    0x11031050/*id*/,
    ctrl_month_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_month_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//月电控参数查看
PLUGIN_INFO g_layer_ctrl_month_display2 = {
    0x11031051/*id*/,
    ctrl_month_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_month_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//月电控参数查看
PLUGIN_INFO g_layer_ctrl_month_display3 = {
    0x11031052/*id*/,
    ctrl_month_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_month_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//月电控参数查看
PLUGIN_INFO g_layer_ctrl_month_display4 = {
    0x11031053/*id*/,
    ctrl_month_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_month_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//月电控参数查看
PLUGIN_INFO g_layer_ctrl_month_display5 = {
    0x11031054/*id*/,
    ctrl_month_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_month_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//月电控参数查看
PLUGIN_INFO g_layer_ctrl_month_display6 = {
    0x11031055/*id*/,
    ctrl_month_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_month_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//月电控参数查看
PLUGIN_INFO g_layer_ctrl_month_display7 = {
    0x11031056/*id*/,
    ctrl_month_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_month_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//月电控参数查看
PLUGIN_INFO g_layer_ctrl_month_display8 = {
    0x11031057/*id*/,
    ctrl_month_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_month_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

#endif






#if DESC("控制状态",1)

const char *gCtrStatusStr[] = {"----", "解除", "投入"};
const char *gCtrlProtectStatusStr[] = {"----", "解除", "保电", "自动保电"};

uint16 g_CtrlInfoNum = 0;

int8  g_CtrlPeriodEffect = 0;
int8  g_CtrlRestEffect = 0;
int8  g_CtrlStopEffect = 0;
int8  g_CtrlDownEffect = 0;

int8  g_CtrlBuyEffect = 0;
int8  g_CtrlMonthEffect = 0;

int8  g_CtrlProtectStatus = 0;



void ControlStatusInit(DWORD dwPluginID)
{
    g_guiComm.nCurPoint = (dwPluginID>>8)&0xff;
    set_gui_infonum((uint16)g_guiComm.nCurPoint);
    
    g_CtrlInfoNum = g_guiComm.nCurPoint;
    g_CtrlPeriodEffect = -1;
    g_CtrlRestEffect = -1;
    g_CtrlStopEffect = -1;
    g_CtrlDownEffect = -1;
    read_db_noamal_data(g_hUdbCenter, 0x810306ff, 0, g_CtrlInfoNum, (uint8*)&g_CtrlPeriodEffect, sizeof(uint8));
    read_db_noamal_data(g_hUdbCenter, 0x810406ff, 0, g_CtrlInfoNum, (uint8*)&g_CtrlRestEffect, sizeof(uint8));
    read_db_noamal_data(g_hUdbCenter, 0x810506ff, 0, g_CtrlInfoNum, (uint8*)&g_CtrlStopEffect, sizeof(uint8));
    read_db_noamal_data(g_hUdbCenter, 0x810606ff, 0, g_CtrlInfoNum, (uint8*)&g_CtrlDownEffect, sizeof(uint8));
    g_CtrlBuyEffect = -1;
    g_CtrlMonthEffect = -1;
    read_db_noamal_data(g_hUdbCenter, 0x810706ff, 0, g_CtrlInfoNum, (uint8*)&g_CtrlBuyEffect, sizeof(uint8));
    read_db_noamal_data(g_hUdbCenter, 0x810806ff, 0, g_CtrlInfoNum, (uint8*)&g_CtrlMonthEffect, sizeof(uint8));

    g_CtrlProtectStatus = -1;
    read_db_noamal_data(g_hUdbCenter, 0x80010200, 0, 0, (uint8*)&g_CtrlProtectStatus, sizeof(uint8));
}

/*********************************************************************
所属功能单元：动态刷新页面绘制函数
功能描述：绘制终端各个控制状态的投入情况
输入参数：无
输出参数：无
函数返回值：无
内部处理流程： 
根据由初始画面函数获得控制组号guiComm.nCurPoint,绘制4个
控制组的情况
********************************************************************/
void ControlStatusDraw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontBig);


    int nLeft1 = 30;
    int nLeft2 = 95;
    int nTop = 20;


    TextOut(hdc, nLeft1, nTop, "下浮控");
    TextOut(hdc, nLeft1, nTop+17*1, "报停控");
    TextOut(hdc, nLeft1, nTop+17*2, "厂休控");
    TextOut(hdc, nLeft1, nTop+17*3, "时段控");
    TextOut(hdc, nLeft1, nTop+17*4, "月电控");
    TextOut(hdc, nLeft1, nTop+17*5, "购电控");
    TextOut(hdc, nLeft1, nTop+17*6, "保  电");

    TextOut(hdc, nLeft2, nTop, gCtrStatusStr[g_CtrlDownEffect+1]); // 下
    TextOut(hdc, nLeft2, nTop+17*1, gCtrStatusStr[g_CtrlStopEffect+1]);  // 报
    TextOut(hdc, nLeft2, nTop+17*2, gCtrStatusStr[g_CtrlRestEffect+1]);  // 厂
    TextOut(hdc, nLeft2, nTop+17*3, gCtrStatusStr[g_CtrlPeriodEffect+1]); // 时    
    TextOut(hdc, nLeft2, nTop+17*4, gCtrStatusStr[g_CtrlMonthEffect+1]);    // 月
    TextOut(hdc, nLeft2, nTop+17*5, gCtrStatusStr[g_CtrlBuyEffect+1]);// 购
    TextOut(hdc, nLeft2, nTop+17*6, gCtrlProtectStatusStr[g_CtrlProtectStatus+1]);  // 保电
}

void ControlStatusCleanup()
{
    UninitDefault();
}

PLUGIN_INFO g_ControlStatusDraw1={
    0x00010101,
    ControlStatusInit,
    ControlStatusCleanup,
    ControlStatusDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_ControlStatusDraw2={
    0x00010201,
    ControlStatusInit,
    ControlStatusCleanup,
    ControlStatusDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_ControlStatusDraw3={
    0x00010301,
    ControlStatusInit,
    ControlStatusCleanup,
    ControlStatusDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_ControlStatusDraw4={
    0x00010401,
    ControlStatusInit,
    ControlStatusCleanup,
    ControlStatusDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_ControlStatusDraw5={
    0x00010501,
    ControlStatusInit,
    ControlStatusCleanup,
    ControlStatusDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_ControlStatusDraw6={
    0x00010601,
    ControlStatusInit,
    ControlStatusCleanup,
    ControlStatusDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_ControlStatusDraw7={
    0x00010701,
    ControlStatusInit,
    ControlStatusCleanup,
    ControlStatusDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_ControlStatusDraw8={
    0x00010801,
    ControlStatusInit,
    ControlStatusCleanup,
    ControlStatusDraw,
    NULL,
    NULL,
    NULL
};

#endif 

#if DESC("购电信息",1)
/**********************************************************************************
【子函数声明】
子函数: MakeFormatedStringOfEnergy()
功能描述：将电能量数据转换为以kWh或MWh为单位的字符串
输入参数：sint64 energy 电能量
          size_t n 输出字符串缓冲区的最大长度(缺省情况下假定缓冲区至少有30字节空间可用)
输出参数：char *str 输出字符串缓冲区,
函数返回值：无
函数用法举例：
    char data[100];
    char str[50];
    if (ReadMtrXData(GROUP_OFFSET+g_guiComm.nCurPoint, RT_DATA, VIR_BUY_EN_CTRL_INFO, data) > 0)
    {
        MakeFormatedStringOfEnergy(chartoint64(data), str, sizeof(str));
        TextOut(hdc, x, y, str);
    }
    另外可参见BuyDataDraw()中对MakeFormatedStringOfEnergy()的调用
**********************************************************************************/

static void MakeFormatedStringOfEnergy(int64 energy, char* str, size_t n = 30);

int advance_send_guimsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    return 0;
}

void advance_gui_infonum(uint16 infonum)
{
    uint8 buf[16] = {0};
    uint16 msglen = 0;

    buf[msglen++] = 1;
    buf[msglen++] = 4;
    appmsg_memcpy_r(buf + msglen, &infonum, sizeof(uint16));
    msglen += sizeof(uint16);
    advance_send_guimsg(0, DESKGUI_IOP_FRESH_TOOL, buf, msglen);
}

/*******************************************************************
所属功能单元：动态刷新页面绘制函数
功能描述：绘制购电信息
输入参数：无
输出参数：无
函数返回值：无
内部处理流程：
根据由初始画面函数获得控制组号g_guiComm.nCurPoint,绘制4个
控制组的情况
********************************************************************/
void BuyDataDraw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    char czBuf[100];
    int nLeft1 = 4;
    int nLeft2 = 56;
    int nTop = 38;
    int nTempTop = 13;
    uint16 oiobject = 0x2301 + g_guiComm.nCurPoint;

    BOOL NoData = FALSE;
    if (tBuyenergyconfig.nNum == 0)
    {
        NoData = TRUE;
    }

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "购电参数 总加组%d", g_guiComm.nCurPoint + 1);
    rc.top = 22;
    rc.bottom = rc.top + 14;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    int index = -1;
    for (uint32 i = 0; i < tBuyenergyconfig.nNum; i++)
    {
        if (tBuyenergyconfig.item[i].object == oiobject)
        {
            index = i;
            break;
        }
    }

    int isfee = 0;
    if (NoData == FALSE || index >= 0)
    {
        if (tBuyenergyconfig.item[index].type == 0)
            isfee = 0;
        else
            isfee = 1;
    }
    else
        isfee = 0;

    TextOut(hdc, nLeft1, nTop, "购电单号");

    if (NoData == TRUE || index == -1)
    {
        TextOut(hdc, nLeft2, nTop, "--------");
    }
    else
    {
        sprintf(czBuf, "%08d", tBuyenergyconfig.item[index].number);
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += nTempTop;
    TextOut(hdc, nLeft1, nTop, "购电方式");

    if (NoData == TRUE || index == -1)
    {
        TextOut(hdc, nLeft2, nTop, "----");
    }
    else
    {
        if (tBuyenergyconfig.item[index].flag == 0)
        {
            TextOut(hdc, nLeft2, nTop, "追加");
        }
        else if (tBuyenergyconfig.item[index].flag == 1)
        {
            TextOut(hdc, nLeft2, nTop, "刷新");
        }
    }

    nTop += nTempTop;
    //TextOut(hdc, nLeft1, nTop, "购前电量");
    if(g_bGuiBuyLastType == 0)
    {
        TextOut(hdc, nLeft1, nTop, "购前电量");
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "购前电费");
    }
    if (NoData == TRUE || index == -1 || FALSE == g_bGuiBuyLastData)
    {
        sprintf(czBuf, "------------ %s", (g_bGuiBuyLastType == 1) ? "元" : "kWh");
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        if (g_bGuiBuyLastType == 1)
        {
            sprintf(czBuf, "%12.4f 元", g_GuiBuyLast * 0.0001);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            MakeFormatedStringOfEnergy(g_GuiBuyLast, czBuf);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
    }

    nTop += nTempTop;
    //TextOut(hdc, nLeft1, nTop, "购电量");
    isfee==0? TextOut(hdc, nLeft1, nTop, "购电量"):TextOut(hdc, nLeft1, nTop, "购电费");
    if (NoData == TRUE || index == -1)
    {
        sprintf(czBuf, "------------ %s", (isfee == 1) ? "元" : "kWh");
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        if (isfee == 1)
        {
            sprintf(czBuf, "%12.4f 元", tBuyenergyconfig.item[index].purElectricity * 0.0001);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            MakeFormatedStringOfEnergy(tBuyenergyconfig.item[index].purElectricity, czBuf);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
    }

    nTop += nTempTop;
    //TextOut(hdc, nLeft1, nTop, "购后电量");
    isfee==0? TextOut(hdc, nLeft1, nTop, "购后电量"):TextOut(hdc, nLeft1, nTop, "购后电费");
    if (NoData == TRUE || index == -1 || FALSE == g_bGuiBuyEndData)
    {
        sprintf(czBuf, "------------ %s", (isfee == 1) ? "元" : "kWh");
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        if (isfee == 1)
        {
            sprintf(czBuf, "%12.4f 元", g_GuiBuyEnd * 0.0001);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            MakeFormatedStringOfEnergy(g_GuiBuyEnd, czBuf);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
    }

    nTop += nTempTop;
    TextOut(hdc, nLeft1, nTop, "报警门限");

    if (NoData == TRUE || index == -1)
    {
        sprintf(czBuf, "------------ %s", (isfee == 1) ? "元" : "kWh");
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        if (isfee == 1)
        {
            sprintf(czBuf, "%12.4f 元", tBuyenergyconfig.item[index].alarmLimit * 0.0001);
        }
        else
        {
            MakeFormatedStringOfEnergy(tBuyenergyconfig.item[index].alarmLimit, czBuf);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += nTempTop;
    TextOut(hdc, nLeft1, nTop, "跳闸门限");

    if (NoData == TRUE || index == -1)
    {
        sprintf(czBuf, "------------ %s", (isfee == 1) ? "元" : "kWh");
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        if (isfee == 1)
        {
            sprintf(czBuf, "%12.4f 元", tBuyenergyconfig.item[index].tripLimit * 0.0001);
        }
        else
        {
            MakeFormatedStringOfEnergy(tBuyenergyconfig.item[index].tripLimit, czBuf);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }


    nTop += nTempTop;
    if (isfee == 1)
        TextOut(hdc, nLeft1, nTop, "剩余金额");
    else
        TextOut(hdc, nLeft1, nTop, "剩余电量");
    if (NoData == TRUE || index == -1 || g_wallet[g_guiComm.nCurPoint] == 0xffffffff)
    {
        sprintf(czBuf, "------------ %s", (isfee == 1) ? "元" : "kWh");
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        if (isfee == 1)
        {
            sprintf(czBuf, "%12.4f 元", (double)g_wallet[g_guiComm.nCurPoint] * 0.0001);
        }
        else
        {
            MakeFormatedStringOfEnergy((double)g_wallet[g_guiComm.nCurPoint], czBuf);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
}

void BuyDataInit(DWORD dwPluginID)
{
    g_guiComm.nCurPoint = dwPluginID & 0x0f;
    advance_gui_infonum(g_guiComm.nCurPoint + 1);

    memset(&tBuyenergyconfig, 0, sizeof(tBuyenergyconfig));
    read_db_noamal_data(g_hUdbCenter, 0x81070200, 0, 0, (uint8*)&tBuyenergyconfig, sizeof(OOP_8107_CONFIG_T));
    uint32 oadvalue;

    memset(g_wallet + g_guiComm.nCurPoint, 0xff, sizeof(int64));
    oadvalue = 0x23010b00 + (0x00010000) * g_guiComm.nCurPoint;
    read_db_noamal_data(g_hUdbCenter, oadvalue, 0, 0, (uint8*)&g_wallet[g_guiComm.nCurPoint], sizeof(int64));

    g_bGuiBuyLastData = FALSE;
    if (0 == read_db_noamal_data(g_hUdbCenter, 0x810703ff, 0, g_guiComm.nCurPoint + 1, (uint8*)&g_GuiBuyLast, sizeof(int64)))
    {
        g_bGuiBuyLastData = TRUE;
    }
    g_bGuiBuyEndData = FALSE;
    if (0 == read_db_noamal_data(g_hUdbCenter, 0x810704ff, 0, g_guiComm.nCurPoint + 1, (uint8*)&g_GuiBuyEnd, sizeof(int64)))
    {
        g_bGuiBuyEndData = TRUE;
    }
    g_bGuiBuyLastType = 0;
    if (0 == read_db_noamal_data(g_hUdbCenter, 0x810705ff, 0, g_guiComm.nCurPoint + 1, &g_bGuiBuyLastType, sizeof(uint8)))
    {
        g_bGuiBuyLastData = TRUE;
    }
}
void BuyDataCleanup(void)
{
    advance_gui_infonum(0);
    UninitDefault();
}

PLUGIN_INFO g_BuyDatadisplay1 = {
    0x11030040,
    BuyDataInit,
    BuyDataCleanup,
    BuyDataDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_BuyDatadisplay2 = {
    0x11030041,
    BuyDataInit,
    BuyDataCleanup,
    BuyDataDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_BuyDatadisplay3 = {
    0x11030042,
    BuyDataInit,
    BuyDataCleanup,
    BuyDataDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_BuyDatadisplay4 = {
    0x11030043,
    BuyDataInit,
    BuyDataCleanup,
    BuyDataDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_BuyDatadisplay5 = {
    0x11030044,
    BuyDataInit,
    BuyDataCleanup,
    BuyDataDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_BuyDatadisplay6 = {
    0x11030045,
    BuyDataInit,
    BuyDataCleanup,
    BuyDataDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_BuyDatadisplay7 = {
    0x11030046,
    BuyDataInit,
    BuyDataCleanup,
    BuyDataDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_BuyDatadisplay8 = {
    0x11030047,
    BuyDataInit,
    BuyDataCleanup,
    BuyDataDraw,
    NULL,
    NULL,
    NULL
};

void MakeFormatedStringOfEnergy(int64 energy, char* str, size_t n)
{
    double val_kWh;
    double val_MWh;

    if (str == NULL || n <= 0)
    {
        return;
    }

    val_kWh = energy * 0.0001;//数据字典中约定内部按缩放10000倍存储电能量
    if (fabs(val_kWh) < 10000000.0)
    {
        snprintf(str, n - 1, "%12.4f kWh", val_kWh);
    }
    else
    {
        val_MWh = val_kWh / 1000.0;
        snprintf(str, n - 1, "%12.2f MWh", val_MWh);
    }
    str[n - 1] = '\0';
}

#endif



#if DESC("中文信息",1)
uint32 g_display_info_index;

typedef struct tag_chinaInfoExtend
{
    CHINESEINFO chinaInfo;
    uint64 addTimeValue;
}chinaInfoExtend;

void exchangeInfo(chinaInfoExtend info[],uint32 a,uint32 b)
{
    chinaInfoExtend tmpInfo;
    tmpInfo = info[a];
    info[a] = info[b];
    info[b] = tmpInfo;
}

void chineseInfoSort(chinaInfoExtend info[],uint32 infoSize)
{
    for(uint32 i=0;i<infoSize;i++)
    {
        uint32 max=i;
        for(uint32 j=i;j<infoSize;j++)
        {
            if(info[j].addTimeValue > info[max].addTimeValue)
                max=j;            
        }
        exchangeInfo(info,max,i);
    }
}

void ZhMsgInfoInit (DWORD dwPluginId)
{
    g_guiComm.nCurPoint = ((dwPluginId&0xff)/5)+1;
    g_guiComm.nIndexPoint = (dwPluginId&0xff)%5;                      // 用于翻屏
    memset(&display_datas, 0, sizeof(display_datas));
    uint32 oadvalue;
    oadvalue = 0x80030200 + (0x00010000) * (g_guiComm.nCurPoint-1);
    read_db_noamal_data(g_hUdbCenter, oadvalue, 0, 0, (uint8*)&display_datas.chinese_info, sizeof(OOP_CHINESEINFOS_T));
}
void ZhMsgInfoCleanup (void)
{
    UninitDefault();
}

/*********************************************************************
所属功能单元：动态刷新页面绘制函数
功能描述：获得中文信息
输入参数：无
输出参数：无
函数返回值：无
内部处理流程： 
********************************************************************/ 
void ZhMsgInfoDraw (HDC hdc) 
{
    char czBuf[300];
    char temp[30];
    SelectFont(hdc,g_guiComm.fontBig);
    //uint8 flg = 0;
    //uint8 index = 0;
    g_display_info_index =g_guiComm.nIndexPoint;
//    if (display_datas.chinese_info.nNum > 0)
//    {
//        for (uint8 n = 0; n < display_datas.chinese_info.nNum; n++)
//        {
//            if((g_guiComm.nIndexPoint+1) == display_datas.chinese_info.item[n].index)
//            {
//                flg = 1;
//                index = n;
//                GUI_FMT_DEBUG("g_guiComm.nIndexPoint:%d,flg:%d,index:%d\n", g_guiComm.nIndexPoint, flg, index);
//                break;
//            }
//        }
//    }

    //新需求：现在按照中文信息的时间顺序进行展示，而不是按照添加顺序
    chinaInfoExtend chinainfoExt[OOP_MAX_CHINESEINFO];
    //CHINESEINFO chinInfo[OOP_MAX_CHINESEINFO];
    for(uint32 i =0;i<display_datas.chinese_info.nNum;i++)
    {
        //chinInfo[i]=display_data.chinese_info.item[i];
        chinainfoExt[i].chinaInfo = display_datas.chinese_info.item[i];
        chinainfoExt[i].addTimeValue = display_datas.chinese_info.item[i].time.year  * 10000000000
                                     + display_datas.chinese_info.item[i].time.month * 100000000
                                     + display_datas.chinese_info.item[i].time.day   * 1000000
                                     + display_datas.chinese_info.item[i].time.hour  * 10000
                                     + display_datas.chinese_info.item[i].time.minute* 100
                                     + display_datas.chinese_info.item[i].time.second* 1;
    }

    //然后对chinInfo数组的index进行排序
    chineseInfoSort(chinainfoExt,display_datas.chinese_info.nNum);
    //CHINESEINFO *pInfo = &(chinInfo[g_display_info_index]);
    CHINESEINFO *pInfo = &(chinainfoExt[g_display_info_index].chinaInfo);

    //if(flg == 0)
    if (display_datas.chinese_info.nNum == 0)
    {   
        SelectFont(hdc,g_guiComm.fontSmall);
        sprintf(temp,"--年-月-日-:-:-");
        TextOut(hdc, 40, 128, temp);

        sprintf(temp, "%s%d", (g_guiComm.nCurPoint==1)?"一般信息":"重要信息", g_guiComm.nIndexPoint+1);
        TextOut(hdc, 56, 16, temp);
        SelectFont(hdc,g_guiComm.fontBig);
        return;
    }
    else
    {   
        SelectFont(hdc,g_guiComm.fontSmall);
//        sprintf(temp,"%04d年%02d月%02d日%02d:%02d:%02d"
//            ,display_datas.chinese_info.item[index].time.year
//            ,display_datas.chinese_info.item[index].time.month
//            ,display_datas.chinese_info.item[index].time.day
//            ,display_datas.chinese_info.item[index].time.hour
//            ,display_datas.chinese_info.item[index].time.minute
//            ,display_datas.chinese_info.item[index].time.second);
          sprintf(temp,"%04d年%02d月%02d日%02d:%02d:%02d"
            ,pInfo->time.year
            ,pInfo->time.month
            ,pInfo->time.day
            ,pInfo->time.hour
            ,pInfo->time.minute
            ,pInfo->time.second);
        TextOut(hdc, 10, 130, temp);
        
        sprintf(temp, "%s%d", (g_guiComm.nCurPoint==1)?"一般信息":"重要信息", g_guiComm.nIndexPoint+1);
        TextOut(hdc, 56, 16, temp);
        SelectFont(hdc,g_guiComm.fontBig);
    }
    

    RECT rc;
    rc.left = 0;
    rc.top = 34;
    rc.right = 159;
    rc.bottom = 130;

    //不多于45个汉字(90字符)时，更改格式
    czBuf[256] = '\0';
    
    int nLen = pInfo->info.nNum;
    memcpy(czBuf, pInfo->info.value, nLen);
    czBuf[nLen] = '\0';
    if(nLen <= 90)
    {   // 设置行间距
        SelectFont(hdc,g_guiComm.fontBig);
    }
    else
    {   
        // 设置行间距
        SelectFont(hdc,g_guiComm.fontSmall);
    }
    DrawText(hdc, czBuf, -1, &rc, DT_LEFT);
}

extern void KeepNextLayer();
BOOL ZhMsgInfoKeyMsg(int nKey)
{
    switch(nKey)
    {
    case KEY_LEFT:
    case KEY_RIGHT:
        KeepNextLayer();
        return TRUE;
    case KEY_UP:
        g_guiComm.nIndexPoint --;
        if(g_guiComm.nIndexPoint < 0)
        {
            g_guiComm.nIndexPoint = (int)display_datas.chinese_info.nNum-1; /*4*/
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_DOWN:
        g_guiComm.nIndexPoint ++;
        if(g_guiComm.nIndexPoint > (int)display_datas.chinese_info.nNum-1)  /*4*/
        {
            g_guiComm.nIndexPoint = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    return FALSE;
}

PLUGIN_INFO g_ZhMsgInfoDraw1 =
{
    0x20400000, 
    ZhMsgInfoInit, 
    ZhMsgInfoCleanup, 
    ZhMsgInfoDraw, 
    NULL, 
    ZhMsgInfoKeyMsg, 
    NULL 
};
PLUGIN_INFO g_ZhMsgInfoDraw2 =
{
    0x20400001, 
    ZhMsgInfoInit, 
    ZhMsgInfoCleanup, 
    ZhMsgInfoDraw, 
    NULL, 
    ZhMsgInfoKeyMsg, 
    NULL 
};
PLUGIN_INFO g_ZhMsgInfoDraw3 =
{
    0x20400002, 
    ZhMsgInfoInit, 
    ZhMsgInfoCleanup, 
    ZhMsgInfoDraw, 
    NULL, 
    ZhMsgInfoKeyMsg, 
    NULL 
};
PLUGIN_INFO g_ZhMsgInfoDraw4 =
{
    0x20400003, 
    ZhMsgInfoInit, 
    ZhMsgInfoCleanup, 
    ZhMsgInfoDraw, 
    NULL, 
    ZhMsgInfoKeyMsg, 
    NULL 
};
PLUGIN_INFO g_ZhMsgInfoDraw5 =
{
    0x20400004, 
    ZhMsgInfoInit, 
    ZhMsgInfoCleanup, 
    ZhMsgInfoDraw, 
    NULL, 
    ZhMsgInfoKeyMsg, 
    NULL 
};
PLUGIN_INFO g_ZhMsgInfoDraw6 =
{
    0x20400005, 
    ZhMsgInfoInit, 
    ZhMsgInfoCleanup, 
    ZhMsgInfoDraw, 
    NULL, 
    ZhMsgInfoKeyMsg, 
    NULL 
};
PLUGIN_INFO g_ZhMsgInfoDraw7 =
{
    0x20400006, 
    ZhMsgInfoInit, 
    ZhMsgInfoCleanup, 
    ZhMsgInfoDraw, 
    NULL, 
    ZhMsgInfoKeyMsg, 
    NULL 
};
PLUGIN_INFO g_ZhMsgInfoDraw8 =
{
    0x20400007, 
    ZhMsgInfoInit, 
    ZhMsgInfoCleanup, 
    ZhMsgInfoDraw, 
    NULL, 
    ZhMsgInfoKeyMsg, 
    NULL 
};
PLUGIN_INFO g_ZhMsgInfoDraw9 =
{
    0x20400008, 
    ZhMsgInfoInit, 
    ZhMsgInfoCleanup, 
    ZhMsgInfoDraw, 
    NULL, 
    ZhMsgInfoKeyMsg, 
    NULL 
};
PLUGIN_INFO g_ZhMsgInfoDraw10 =
{
    0x20400009, 
    ZhMsgInfoInit, 
    ZhMsgInfoCleanup, 
    ZhMsgInfoDraw, 
    NULL, 
    ZhMsgInfoKeyMsg, 
    NULL 
};
#endif


#if DESC("开关状态",1)
OOP_SWITCHIN_STATE_T    switchState;    //遥信状态
OOP_SWITCHIN_FLAG_T     switchFlag;     //接入标志

void switch_status_display_init(DWORD dwPluginId)
{
    memset(&switchState, 0, sizeof(OOP_SWITCHIN_STATE_T));
    memset(&switchFlag, 0, sizeof(OOP_SWITCHIN_FLAG_T));
    read_db_noamal_data(g_hUdbCenter, 0xf2030200, 0, 0, (uint8*)&switchState, sizeof(OOP_SWITCHIN_STATE_T));
    read_db_noamal_data(g_hUdbCenter, 0xf2030400, 0, 0, (uint8*)&switchFlag, sizeof(OOP_SWITCHIN_FLAG_T));
}

#define SWITCH_SHOW(S, F, i) 0 == ((F.inputFlag.bitValue >> i) & 0x01) ? "--" : (0 == S.state[i].st ? "分" : "合")


//遥信状态
void switch_status_display_draw(HDC hdc)
{
    char *switchname[] = {(char*)"一", (char*)"二", (char*)"三", (char*)"四", (char*)"五", 
                          (char*)"六", (char*)"七", (char*)"八"};
    char czBuf[64] = {0};
    
    read_db_noamal_data(g_hUdbCenter, 0xf2030200, 0, 0, (uint8*)&switchState, sizeof(OOP_SWITCHIN_STATE_T));
    read_db_noamal_data(g_hUdbCenter, 0xf2030400, 0, 0, (uint8*)&switchFlag, sizeof(OOP_SWITCHIN_FLAG_T));

    int nLeft = 5;
    int nLeft1 = 90;
    int nTop = 35;
    int tmpTop = 22;
//    uint8 status = 0;
    
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "开关状态");
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontSmall);
    // 遥信状态
    for (uint32 i = 0; i < 4; i++)
    {
        if(0 == i % 2)
        {
            nTop += tmpTop;
        }
        //sprintf(czBuf, "第%s路  %s", switchname[i], g_showSwitchState[switchState.state[i].st]);
        sprintf(czBuf, "第%s路  %s", switchname[i], SWITCH_SHOW(switchState, switchFlag, i));
        TextOut(hdc, (0 == i % 2) ? nLeft : nLeft1, nTop, czBuf);
    }

    OOP_STATUS_SIGNAL_T  g_StatusSignal;//信号节点
    memset(&g_StatusSignal, 0, sizeof(OOP_STATUS_SIGNAL_T));
    read_db_noamal_data(g_hUdbCenter, 0xf2030600, 0, 0, (uint8*)&g_StatusSignal, sizeof(OOP_STATUS_SIGNAL_T));

    //门节点
    nTop += tmpTop;
    memset(czBuf, 0, 64);
    if(0 == g_StatusSignal.nNum)
    {
        sprintf(czBuf, "门节点  --");
    }
    else
    {
        sprintf(czBuf, "门节点  %s",  g_showSwitchState[g_StatusSignal.ST.nValue & 0x01]);
    }
    TextOut(hdc, nLeft, nTop, czBuf);

    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

//遥信状态
PLUGIN_INFO g_layer_switch_status_display = {
    0x000b0000/*id*/,
    switch_status_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    switch_status_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
#endif

//该函数处理功率电量等值显示,防止大数值跃屏
void valueDisplayProcess_strUnit(int64 energy, char* str, size_t n,uint32 weight,const char* unit)
{
    double val_kWh = energy;
    //const char *units[8]={"w","wh","var"};

    if (str == NULL || n <= 0)
    {
        return;
    }

    for(uint32 i =1;i<=weight;i++)
    {
        val_kWh = val_kWh *0.1;
    }

    //val_kWh = energy * 0.0001;//数据字典中约定内部按缩放10000倍存储电能量

    //单位自适应来解决数值过大不能显示全部的问题
    //求位数
    int64 val_autoAdap=(int64)val_kWh;
    uint8 digitsCnt=0;
    while(val_autoAdap)
    {
        val_autoAdap = val_autoAdap/10;
        digitsCnt++;
    }
    if(digitsCnt<=6)
    {
        //val_kWh = energy * 0.0001;//数据字典中约定内部按缩放10000倍存储电能量
        snprintf(str, n - 1, "%6.4fk%s", val_kWh,unit);
        str[n - 1] = '\0';
    }
    else if(digitsCnt>6 && digitsCnt<=9)
    {
        val_kWh = val_kWh*0.001;//数据字典中约定内部按缩放10000倍存储电能量
        snprintf(str, n - 1, "%6.4fM%s", val_kWh,unit);
        str[n - 1] = '\0';
    }
    else
    {
        val_kWh = val_kWh*0.001*0.001;//数据字典中约定内部按缩放10000倍存储电能量
        snprintf(str, n - 1, "%6.4fG%s", val_kWh,unit);
        str[n - 1] = '\0';
    }
}


#if DESC("当前功率", 1)

void CurPowerSep1(HDC hdc);
void CurPowerSep2(HDC hdc);


void CurPowerDrawTotal(HDC hdc)
{   
    char czBuf[70] = { 0 };
    ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);
    
    TextOut(hdc,15,40,"当前总有功功率");
    TextOut(hdc,15,70,"当前总无功功率");

    //先求出8路的总有功和总无功功率

    sprintf(czBuf, "%6.4f %s", (double)g_group_data.gruop_active_power / 10000, "kW");
    TextOut(hdc,15,56,czBuf);


    // 总无功功率
    sprintf(czBuf, "%6.4f %s", (double)g_group_data.gruop_reactive_power / 10000, "kvar");
    TextOut(hdc, 15, 86, czBuf);
    
}



void CurPowerInit(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(g_ctrl_data, 0, sizeof(g_ctrl_data));
    memset(&g_group_data, 0, sizeof(g_group_data));
    //读取脉冲有功和无功功率
    //注意,你这里读取的是第一路脉冲的总有功和总无功功率
    //总有功和总无功,可以先把8路全部读出来,然后求和
    read_db_noamal_data(g_hUdbCenter, 0x23010300, 0, 0, (uint8*)&g_group_data.gruop_active_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23010400, 0, 0, (uint8*)&g_group_data.gruop_reactive_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x24010500, 0, 0, (uint8*)&g_ctrl_data[0].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24010600, 0, 0, (uint8*)&g_ctrl_data[0].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24020500, 0, 0, (uint8*)&g_ctrl_data[1].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24020600, 0, 0, (uint8*)&g_ctrl_data[1].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24030500, 0, 0, (uint8*)&g_ctrl_data[2].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24030600, 0, 0, (uint8*)&g_ctrl_data[2].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24040500, 0, 0, (uint8*)&g_ctrl_data[3].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24040600, 0, 0, (uint8*)&g_ctrl_data[3].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24050500, 0, 0, (uint8*)&g_ctrl_data[4].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24050600, 0, 0, (uint8*)&g_ctrl_data[4].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24060500, 0, 0, (uint8*)&g_ctrl_data[5].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24060600, 0, 0, (uint8*)&g_ctrl_data[5].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24070500, 0, 0, (uint8*)&g_ctrl_data[6].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24070600, 0, 0, (uint8*)&g_ctrl_data[6].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24080500, 0, 0, (uint8*)&g_ctrl_data[7].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24080600, 0, 0, (uint8*)&g_ctrl_data[7].plus_reactive_power, sizeof(int32));  
}


void CurPowerDraw(HDC hdc)
{

    switch(g_ac_data_display_drawpage_index)
        {
            case 0:   //总有功和共无功功率
                CurPowerDrawTotal(hdc);
                break;
            case 1:   //前四路功率
                CurPowerSep1(hdc);
                break;
            case 2:
                CurPowerSep2(hdc);
                break;
        } 
}



//前四路功率显示页面
void CurPowerSep1(HDC hdc)
{
    char czBuf[70] = { 0 };
    ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);
    
    TextOut(hdc,15,20,"第1路功率");
    TextOut(hdc,15,50,"第2路功率");
    TextOut(hdc,15,80,"第3路功率");
    TextOut(hdc,15,110,"第4路功率");

    for(int i = 0;i<4;i++)
    {
        sprintf(czBuf, "%6.4f %s", (double)g_ctrl_data[i].plus_active_power / 10000, "kW");
        TextOut(hdc, 90, 20+i*30, czBuf);
        bzero(czBuf,sizeof(czBuf)); 
    }
    
}

//后四路功率显示页面
void CurPowerSep2(HDC hdc)
{
    char czBuf[70] = { 0 };
    ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);
    
    TextOut(hdc,15,20,"第5路功率");
    TextOut(hdc,15,50,"第6路功率");
    TextOut(hdc,15,80,"第7路功率");
    TextOut(hdc,15,110,"第8路功率");

    for(int i = 0;i<4;i++)
    {
        sprintf(czBuf, "%6.4f %s", (double)g_ctrl_data[i+4].plus_active_power / 10000, "kW");
        TextOut(hdc, 90, 20+i*30, czBuf);
        bzero(czBuf,sizeof(czBuf)); 
    }
    
}


//按键消息
BOOL CurPowerKeyMsg(int nKey)
{
    //3个页面循环显示
    switch (nKey)
        {
            case KEY_DOWN:
                g_ac_data_display_drawpage_index>=2? g_ac_data_display_drawpage_index=0: ++g_ac_data_display_drawpage_index;
                break;

            case KEY_UP:
                g_ac_data_display_drawpage_index<=0?g_ac_data_display_drawpage_index=2 : --g_ac_data_display_drawpage_index;
                break;

            case KEY_CANCEL:
                CancelMenu();
                break;
        }
    return TRUE;
}


//当前总有功/无功功率页面
PLUGIN_INFO g_layer_cur_powerTotal = {
    0x20080001/*id*/,
    CurPowerInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    CurPowerDraw/*Draw*/,
    NULL/*Timer*/,
    CurPowerKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

void G_CurPowerSep1(HDC hdc);
void G_CurPowerSep2(HDC hdc);


void G_CurPowerDrawTotal(HDC hdc)
{	
	char czBuf[70] = { 0 };
	ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);
	
	TextOut(hdc,10,40,"当前总有功功率");
	TextOut(hdc,10,70,"当前总无功功率");

	//先求出8路的总有功和总无功功率
	int64 totalActivePow=0;
	int64 totalReactivePow=0;
	
	for(int i=0;i<8;i++)
	{
		totalActivePow+=g_ctrl_data[i].gruop_active_power;
		totalReactivePow+=g_ctrl_data[i].gruop_reactive_power;
	}
	
	sprintf(czBuf, "%6.4f %s", (double)totalActivePow / 10000, "kW");
	TextOut(hdc,10,56,czBuf);


    // 总无功功率
	sprintf(czBuf, "%6.4f %s", (double)totalReactivePow / 10000, "kvar");
    TextOut(hdc, 10, 86, czBuf);
	
}



void G_CurPowerInit(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(g_ctrl_data, 0, sizeof(g_ctrl_data));

	//读取脉冲有功和无功功率
	//注意,你这里读取的是第一路脉冲的总有功和总无功功率
	//总有功和总无功,可以先把8路全部读出来,然后求和
    read_db_noamal_data(g_hUdbCenter, 0x23010300, 0, 0, (uint8*)&g_ctrl_data[0].gruop_active_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23010400, 0, 0, (uint8*)&g_ctrl_data[0].gruop_reactive_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23020300, 0, 0, (uint8*)&g_ctrl_data[1].gruop_active_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23020400, 0, 0, (uint8*)&g_ctrl_data[1].gruop_reactive_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23030300, 0, 0, (uint8*)&g_ctrl_data[2].gruop_active_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23030400, 0, 0, (uint8*)&g_ctrl_data[2].gruop_reactive_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23040300, 0, 0, (uint8*)&g_ctrl_data[3].gruop_active_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23040400, 0, 0, (uint8*)&g_ctrl_data[3].gruop_reactive_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23050300, 0, 0, (uint8*)&g_ctrl_data[4].gruop_active_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23050400, 0, 0, (uint8*)&g_ctrl_data[4].gruop_reactive_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23060300, 0, 0, (uint8*)&g_ctrl_data[5].gruop_active_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23060400, 0, 0, (uint8*)&g_ctrl_data[5].gruop_reactive_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23070300, 0, 0, (uint8*)&g_ctrl_data[6].gruop_active_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23070400, 0, 0, (uint8*)&g_ctrl_data[6].gruop_reactive_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23080300, 0, 0, (uint8*)&g_ctrl_data[7].gruop_active_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23080400, 0, 0, (uint8*)&g_ctrl_data[7].gruop_reactive_power, sizeof(int64));  
}


void G_CurPowerDraw(HDC hdc)
{

	switch(g_ac_data_display_drawpage_index)
		{
			case 0:   //总有功和共无功功率
				G_CurPowerDrawTotal(hdc);
				break;
			case 1:   //前四路功率
				G_CurPowerSep1(hdc);
				break;
			case 2:
				G_CurPowerSep2(hdc);
				break;
		} 
}


//前四路功率显示页面
void G_CurPowerSep1(HDC hdc)
{
	char czBuf[70] = { 0 };
	ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);
	
	TextOut(hdc,10,20,"总加组1有功功率");
	TextOut(hdc,10,50,"总加组2有功功率");
	TextOut(hdc,10,80,"总加组3有功功率");
	TextOut(hdc,10,110,"总加组4有功功率");

	for(int i = 0;i<4;i++)
	{
		sprintf(czBuf, "%6.4f %s", (double)g_ctrl_data[i].gruop_active_power / 10000, "kW");
		TextOut(hdc, 10, 35+i*30, czBuf);
		bzero(czBuf,sizeof(czBuf));	
	}
	
}

//后四路功率显示页面
void G_CurPowerSep2(HDC hdc)
{
	char czBuf[70] = { 0 };
	ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);
	
	TextOut(hdc,10,20,"总加组5有功功率");
	TextOut(hdc,10,50,"总加组6有功功率");
	TextOut(hdc,10,80,"总加组7有功功率");
	TextOut(hdc,10,110,"总加组8有功功率");

	for(int i = 0;i<4;i++)
	{
		sprintf(czBuf, "%6.4f %s", (double)g_ctrl_data[i+4].gruop_active_power / 10000, "kW");
		TextOut(hdc, 10, 35+i*30, czBuf);
		bzero(czBuf,sizeof(czBuf));	
	}
	
}


//当前总有功/无功功率页面
PLUGIN_INFO g_layer_gruop_cur_powerTotal = {
    0x20000000/*id*/,
    G_CurPowerInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    G_CurPowerDraw/*Draw*/,
    NULL/*Timer*/,
    CurPowerKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

#endif


#if DESC("有功功率", 1)

void ActivePowerDraw(HDC hdc)
{
    power_display_draw_param(hdc, "有功功率", g_showTitleJCPower_Y, display_data.ppower, "W", 10000);
}

//当前总有功功率页面
PLUGIN_INFO g_layer_active_powerTotal = {
    0x20080002/*id*/,
    ac_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    ActivePowerDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("无功功率", 1)

void ReactivePowerDraw(HDC hdc)
{
    power_display_draw_param(hdc, "无功功率", g_showTitleJCPower_W, display_data.npower, "var", 10000);
}

//当前总无功功率页面
PLUGIN_INFO g_layer_reactive_powerTotal = {
    0x20080003/*id*/,
    ac_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    ReactivePowerDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("当前电量", 1)

int curEleQ_DM = 0;

//8路总加组
typedef struct{
	//第i路总加组的日/月有功电量
	OOP_HENERGYS_T zjfpg[2];
	
	//第i路总加组的日无功电量
	OOP_HENERGYS_T day_reactiveEleQ;
	
	//第i路总加组的月无功电量
	OOP_HENERGYS_T month_reactiveEleQ;
}Group;

Group groupData[8];


/**
    该页面画出共有的部分
    @param numOfLu: 第几路,1-8路
    @dayOrMonth:日还是月    ,0是日,1是月
    
**/
void CurElecQPlateDraw(HDC hdc,int numOfLu,int dayOrMonth)
{   
    char czBuf[70];
    sprintf(czBuf, "总加组 %d",numOfLu+1);

    ITEM_NOT_SELECT;

    SelectFont(hdc,g_guiComm.fontSmall);

    TextOut(hdc,58,20,czBuf);

    int nLeft1 = 40;
    int nTop = 50;

    switch (dayOrMonth)
    {
        case 0:
            TextOut(hdc,nLeft1,35,"当日有功电量  ");
            break;
        case 1:
            TextOut(hdc,nLeft1,35,"当日无功电量");
            break;
        case 2:
            TextOut(hdc,nLeft1,35,"当月有功电量");
            break;
        case 3:
            TextOut(hdc,nLeft1,35,"当月无功电量");
            break;
        default:
            break;
    }

    TextOut(hdc, 0, nTop, "总 ");
        for(int i = 0; i < 4; i ++)
        {
            nTop += 18;
            switch(i) 
            {
            case 0:TextOut(hdc, 0, nTop, "尖 ");break;
            case 1:TextOut(hdc, 0, nTop, "峰 ");break;
            case 2:TextOut(hdc, 0, nTop, "平 ");break;
            case 3:TextOut(hdc, 0, nTop, "谷 ");break;
            default:ASSERT(FALSE);
            }
        }

    
}


void MakeFormatedEnergy(int64 energy, char* str, int dayOrMonth, size_t n = 30)
{
    double val_kWh;

    if (str == NULL || n <= 0)
    {
        return;
    }

    val_kWh = energy * 0.0001;//数据字典中约定内部按缩放10000倍存储电能量
    if(dayOrMonth == 0 || dayOrMonth == 2)
        snprintf(str, n - 1, "%6.4f kWh", val_kWh);
    else
        snprintf(str, n - 1, "%6.4f kvarh", val_kWh);
    
    str[n - 1] = '\0';
}


void CurElecQDrawTotal(HDC hdc,int numOfLu,int dayOrMonth)
{
    char czBuf[100];
    int nLeft2 = 22;
    int nTop = 50;

    //先画plate
    CurElecQPlateDraw(hdc,numOfLu,dayOrMonth);

    ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);

    if (dayOrMonth == 0 || dayOrMonth == 2)
    {
        uint8 index = 0;
        if (dayOrMonth == 2)
            index =1;
        //groupData[i]表示第i+1路,zjfpg[0-4]表示日的总尖峰平谷,zjfpg[5-9]表示月的总尖峰平谷
        MakeFormatedEnergy(groupData[numOfLu].zjfpg[index].nValue[0], czBuf, dayOrMonth);
        TextOut(hdc,nLeft2,nTop,czBuf);
        nTop+=18;
        
        MakeFormatedEnergy(groupData[numOfLu].zjfpg[index].nValue[1], czBuf, dayOrMonth);
        TextOut(hdc,nLeft2,nTop,czBuf);
        nTop+=18;
        
        MakeFormatedEnergy(groupData[numOfLu].zjfpg[index].nValue[2], czBuf, dayOrMonth);
        TextOut(hdc,nLeft2,nTop,czBuf);
        nTop+=18;
        
        MakeFormatedEnergy(groupData[numOfLu].zjfpg[index].nValue[3], czBuf, dayOrMonth);
        TextOut(hdc,nLeft2,nTop,czBuf);
        nTop+=18;
        
        MakeFormatedEnergy(groupData[numOfLu].zjfpg[index].nValue[4], czBuf, dayOrMonth);
        TextOut(hdc,nLeft2,nTop,czBuf);
    }
    else
    {
        bzero(czBuf,sizeof(czBuf));
        dayOrMonth == 1?  MakeFormatedEnergy(groupData[numOfLu].day_reactiveEleQ.nValue[0], czBuf, dayOrMonth)
                        :   MakeFormatedEnergy(groupData[numOfLu].month_reactiveEleQ.nValue[0], czBuf, dayOrMonth);
        TextOut(hdc,nLeft2,nTop,czBuf);
        nTop+=18;

        bzero(czBuf,sizeof(czBuf));
        dayOrMonth == 1?  MakeFormatedEnergy(groupData[numOfLu].day_reactiveEleQ.nValue[1], czBuf, dayOrMonth)
                        :   MakeFormatedEnergy(groupData[numOfLu].month_reactiveEleQ.nValue[1], czBuf, dayOrMonth);
        TextOut(hdc,nLeft2,nTop,czBuf);
        nTop+=18;

        bzero(czBuf,sizeof(czBuf));
        dayOrMonth == 1?  MakeFormatedEnergy(groupData[numOfLu].day_reactiveEleQ.nValue[2], czBuf, dayOrMonth)
                        :   MakeFormatedEnergy(groupData[numOfLu].month_reactiveEleQ.nValue[2], czBuf, dayOrMonth);
        TextOut(hdc,nLeft2,nTop,czBuf);
        nTop+=18;

        bzero(czBuf,sizeof(czBuf));
        dayOrMonth == 1?  MakeFormatedEnergy(groupData[numOfLu].day_reactiveEleQ.nValue[3], czBuf, dayOrMonth)
                        :   MakeFormatedEnergy(groupData[numOfLu].month_reactiveEleQ.nValue[3], czBuf, dayOrMonth);
        TextOut(hdc,nLeft2,nTop,czBuf);
        nTop+=18;

        bzero(czBuf,sizeof(czBuf));
        dayOrMonth == 1?  MakeFormatedEnergy(groupData[numOfLu].day_reactiveEleQ.nValue[4], czBuf, dayOrMonth)
                        :   MakeFormatedEnergy(groupData[numOfLu].month_reactiveEleQ.nValue[4], czBuf, dayOrMonth);
        TextOut(hdc,nLeft2,nTop,czBuf);
    }
}


void CurEleQInit(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    curEleQ_DM = 0;
    memset(g_ctrl_data, 0, sizeof(g_ctrl_data));

    //读取8个计量点的当日、月的有功、无功电量
    //int32            plus_active_power;         //脉冲有功功率 0x24010500
    //int32            plus_reactive_power;       //脉冲无功功率 0x24010600
    //int64            gruop_active_power;        //总加组有功功率 0x23010300
    //int64            gruop_reactive_power;      //总加组无功功率 0x23010400
    //OOP_HENERGYS_T   gruop_day_energy;        //总加组日有功电量 0x23010700
    //OOP_HENERGYS_T   gruop_day_ractenergy;    //总加组日无功电量 0x23010800
    //OOP_HENERGYS_T   gruop_month_energy;      //总加组月有功电量 0x23010900
    //OOP_HENERGYS_T   gruop_month_ractenergy;  //总加组月无功电量 0x23010a00
    //OOP_BITSTR8_T    ctrlPwTurn;              //0x230x0e00 总加组功控轮次配置  控制读取
    //OOP_BITSTR8_T    ctrlEcTurn;              //0x230x0f00 总加组电控轮次配置  控制读取
    //OOP_TGSETTING_T  ctrl_seting;             //控制设置状态         0x23011000
    //OOP_TGCURSTATE_T ctrl_status;             //控制当前状态         0x23011100
    //OOP_PULSESETS_T  plus_param;              //脉冲配置       0x24010400
    //OOP_PULSESETS_B_T plus_b_param;           //脉冲配置       0x24011C00
    //OOP_OCTETVAR16_T pulse_addr;              //脉冲地址       0x24010200
    //OOP_TGSETS_T     gruop_param;             //脉冲配置       0x23010200

    uint32 group1[2] ={0x23010700, 0x23010900};
    //第一路数据
    for(int j =0;j<2;j++)
    {
        read_db_noamal_data(g_hUdbCenter,group1[j],0,0,(uint8*)&(groupData[0].zjfpg[j]),sizeof(OOP_HENERGYS_T));
    }
    read_db_noamal_data(g_hUdbCenter, 0x23010800, 0, 0, (uint8*)&(groupData[0].day_reactiveEleQ) , sizeof(OOP_HENERGYS_T));
    read_db_noamal_data(g_hUdbCenter, 0x23010a00, 0, 0, (uint8*)&(groupData[0].month_reactiveEleQ), sizeof(OOP_HENERGYS_T));



    uint32 group2[2] ={0x23020700, 0x23020900};
    //第二路数据
    for(int j =0;j<2;j++)
    {
        read_db_noamal_data(g_hUdbCenter,group2[j],0,0,(uint8*)&(groupData[1].zjfpg[j]),sizeof(OOP_HENERGYS_T));
    }
    read_db_noamal_data(g_hUdbCenter, 0x23020800, 0, 0, (uint8*)&(groupData[1].day_reactiveEleQ) , sizeof(OOP_HENERGYS_T));
    read_db_noamal_data(g_hUdbCenter, 0x23020a00, 0, 0, (uint8*)&(groupData[1].month_reactiveEleQ), sizeof(OOP_HENERGYS_T));

    
    uint32 group3[2] ={0x23030700, 0x23030900};
    //第三路数据
    for(int j =0;j<2;j++)
    {
        read_db_noamal_data(g_hUdbCenter,group3[j],0,0,(uint8*)&(groupData[2].zjfpg[j]),sizeof(OOP_HENERGYS_T));
    }
    read_db_noamal_data(g_hUdbCenter, 0x23030800, 0, 0, (uint8*)&(groupData[2].day_reactiveEleQ) , sizeof(OOP_HENERGYS_T));
    read_db_noamal_data(g_hUdbCenter, 0x23030a00, 0, 0, (uint8*)&(groupData[2].month_reactiveEleQ), sizeof(OOP_HENERGYS_T));
    

    uint32 group4[2] ={0x23040700, 0x23040900};
    //第四路数据
    for(int j =0;j<2;j++)
    {
        read_db_noamal_data(g_hUdbCenter,group4[j],0,0,(uint8*)&(groupData[3].zjfpg[j]),sizeof(OOP_HENERGYS_T));
    }
    read_db_noamal_data(g_hUdbCenter, 0x23040800, 0, 0, (uint8*)&(groupData[3].day_reactiveEleQ) , sizeof(OOP_HENERGYS_T));
    read_db_noamal_data(g_hUdbCenter, 0x23040a00, 0, 0, (uint8*)&(groupData[3].month_reactiveEleQ), sizeof(OOP_HENERGYS_T));



    uint32 group5[2] ={0x23050700, 0x23050900};
    //第五路数据
    for(int j =0;j<2;j++)
    {
        read_db_noamal_data(g_hUdbCenter,group5[j],0,0,(uint8*)&(groupData[4].zjfpg[j]),sizeof(OOP_HENERGYS_T));
    }
    read_db_noamal_data(g_hUdbCenter, 0x23050800, 0, 0, (uint8*)&(groupData[4].day_reactiveEleQ) , sizeof(OOP_HENERGYS_T));
    read_db_noamal_data(g_hUdbCenter, 0x23050a00, 0, 0, (uint8*)&(groupData[4].month_reactiveEleQ), sizeof(OOP_HENERGYS_T));


    uint32 group6[2] ={0x23060700, 0x23060900};
    //第六路数据
    for(int j =0;j<2;j++)
    {
        read_db_noamal_data(g_hUdbCenter,group6[j],0,0,(uint8*)&(groupData[5].zjfpg[j]),sizeof(OOP_HENERGYS_T));
    }
    read_db_noamal_data(g_hUdbCenter, 0x23060800, 0, 0, (uint8*)&(groupData[5].day_reactiveEleQ) , sizeof(OOP_HENERGYS_T));
    read_db_noamal_data(g_hUdbCenter, 0x23060a00, 0, 0, (uint8*)&(groupData[5].month_reactiveEleQ), sizeof(OOP_HENERGYS_T));


    uint32 group7[2] ={0x23070700, 0x23070900};
    //第七路数据
    for(int j =0;j<2;j++)
    {
        read_db_noamal_data(g_hUdbCenter,group7[j],0,0,(uint8*)&(groupData[6].zjfpg[j]),sizeof(OOP_HENERGYS_T));
    }
    read_db_noamal_data(g_hUdbCenter, 0x23070800, 0, 0, (uint8*)&(groupData[6].day_reactiveEleQ) , sizeof(OOP_HENERGYS_T));
    read_db_noamal_data(g_hUdbCenter, 0x23070a00, 0, 0, (uint8*)&(groupData[6].month_reactiveEleQ), sizeof(OOP_HENERGYS_T));


    uint32 group8[2] ={0x23080700, 0x23080900};
    //第八路数据
    for(int j =0;j<2;j++)
    {
        read_db_noamal_data(g_hUdbCenter,group8[j],0,0,(uint8*)&(groupData[7].zjfpg[j]),sizeof(OOP_HENERGYS_T));
    }
    read_db_noamal_data(g_hUdbCenter, 0x23080800, 0, 0, (uint8*)&(groupData[7].day_reactiveEleQ) , sizeof(OOP_HENERGYS_T));
    read_db_noamal_data(g_hUdbCenter, 0x23080a00, 0, 0, (uint8*)&(groupData[7].month_reactiveEleQ), sizeof(OOP_HENERGYS_T));
        
}


void CurElecQDraw(HDC hdc)
{
    
    CurElecQDrawTotal(hdc,g_ac_data_display_drawpage_index,curEleQ_DM);
            
}




//按键消息
BOOL CurEleQKeyMsg(int nKey)
{
    
    switch (nKey)
        {
            case KEY_DOWN:
                g_ac_data_display_drawpage_index>=7? g_ac_data_display_drawpage_index=0: ++g_ac_data_display_drawpage_index;
                break;

            case KEY_UP:
                g_ac_data_display_drawpage_index<=0?g_ac_data_display_drawpage_index=7 : --g_ac_data_display_drawpage_index;
                break;

            case KEY_CANCEL:
                CancelMenu();
                break;
            
            //左右键切换日和月的数据显示
            case KEY_RIGHT:
                curEleQ_DM >=3? curEleQ_DM =0: ++curEleQ_DM;
                break;
            
            case KEY_LEFT:
                curEleQ_DM <=0? curEleQ_DM =3: --curEleQ_DM;
                break;
                
        }
    return TRUE;
}


//当前总有功/无功功率页面
PLUGIN_INFO g_layer_cur_EleQ = {
    0x00021100/*id*/,
    CurEleQInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    CurElecQDraw/*Draw*/,
    NULL/*Timer*/,
    CurEleQKeyMsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("负荷曲线",1)

int64 curPows[96];

int db_readRecord(DB_CLIENT fd, READ_RECORD_T *ptRecord, uint32 recordLen)
{
    int        ret = 0;   //DBMSG_SUCCESS=0;
    READ_RECORD_T      inReadRecordTmp;
    STEP_READ_RECORD_T stepReadRecord;
    uint32 handle;
    uint32 recNum;
    int32  nRet = 0;
    RECORD_UNIT_T       recordOut;
    memset(&recordOut, 0, sizeof(RECORD_UNIT_T));
    if(fd <= 0 || NULL == ptRecord)
    {
        return 0x2000; //DBMSG_ERR_INPUT = 0x2000;
    }

    if (recordLen > DATA_LEN_MAX)
    {
        return ERR_O_RANGE;
    }
    /* 只读最近一条 */
    inReadRecordTmp = *ptRecord;
    inReadRecordTmp.sortType = DOWN_SORT;

    ret = db_read_record_start(fd, &inReadRecordTmp, &handle, &recNum);
    if (0 != ret)
    {
        return (0x10000000 + ret);
    }
    if(0 == recNum)
    {
        return ERR_NORESOURCE;
    }
    
    memcpy(&stepReadRecord.MAC, &inReadRecordTmp.MAC, sizeof(OOP_OCTETVAR64_T));

    uint32 cnt = 0, index = 0;
    uint32 lastRecordTime;
    //先读一条
    ret = db_read_record_step(fd, handle, &stepReadRecord, &recordOut);
    if (0 != ret)
    {
        return ret;
    }
    
    index = (recordOut.colEndTm - inReadRecordTmp.cStart) / 900;
    memcpy(&curPows[index], recordOut.record[0].outDataBuf, recordLen);
	lastRecordTime = recordOut.colEndTm;
    cnt ++;

    while((cnt < 96)&&(cnt < recNum))
    {
        ret = db_read_record_step(fd, handle, &stepReadRecord, &recordOut);
        if (0 != ret)
        {
            return ret;
        }
        //判断该记录的时间距离上一条是否大于等于15分钟,如果是,那么记录该数据;否则就跳过
        if((lastRecordTime - recordOut.colEndTm) > 14 * 60)
        {
            //计算读取出来的数据是属于哪个点的
            index = (recordOut.colEndTm - inReadRecordTmp.cStart) / 900;
            memcpy(&curPows[index], recordOut.record[0].outDataBuf, recordLen);
            lastRecordTime = recordOut.colEndTm;
            cnt ++;
        }
    }

    /*结束读记录*/
    ret = db_read_record_end(fd, handle);
    if (0 != ret)
    {
        return (0x20000000 + ret);
    }

    return nRet;

}

void power_curve_TimeSetToday(time_t* tStart,time_t* tEnd)
{
	time_t timp;
	struct tm *tmStart;
	struct tm *tmEnd;

	time(&timp);

	//将当前时间转换一下
	tmEnd = localtime(&timp);
	
	tmStart =(tm*)malloc(sizeof(tm));
	memcpy(tmStart,tmEnd,sizeof(tm));

	//将tmEnd的hour和min和sec修改
	tmStart->tm_hour = 0;
	tmStart->tm_min = 0;
	tmStart->tm_sec = 0;
	*tStart =  mktime(tmStart);
    
    tmEnd->tm_hour = 23;
    tmEnd->tm_min = 59;
	tmEnd->tm_sec = 59;
	*tEnd = mktime(tmEnd);

	free(tmStart);

}

void power_curve_TimeSet(time_t* tStart,time_t* tEnd)
{
    //我们测试一下时间的函数
    time_t timp;
    struct tm *tmStart;
    struct tm *tmEnd;

    //获取当前时间的大值
    time(&timp);

    //获取昨天同一时刻的大值
    timp = timp - 60*60*24;

    //将昨天同一时刻的大值转换成结构 
    tmStart = localtime(&timp);
    
    tmEnd =(tm*)malloc(sizeof(tm));
    memcpy(tmEnd,tmStart,sizeof(tm));

    //将昨天同一时刻的结构的hour和min和sec修改为0
    tmStart->tm_hour = 0;
    tmStart->tm_min = 0;
    tmStart->tm_sec = 0;

    //得到tmEnd
    //memcpy(tmEnd,tmStart,sizeof(tm));

    //将tmEnd的hour和min和sec修改
    tmEnd->tm_hour = 23;
    tmEnd->tm_min = 59;
    tmEnd->tm_sec = 59;

    *tStart =  mktime(tmStart);
    *tEnd = mktime(tmEnd);
    
    //bzero(czBuf,sizeof(czBuf));
    //sprintf(czBuf, "%d,%d,%d,%d,%d",tmStart->tm_mon+1,tmStart->tm_mday,tmStart->tm_hour,tmStart->tm_min,tmStart->tm_sec);
    //TextOut(hdc, 50, 50, czBuf);  

    //bzero(czBuf,sizeof(czBuf));
    //sprintf(czBuf, "%d,%d,%d,%d,%d",tmEnd->tm_mon+1,tmEnd->tm_mday,tmEnd->tm_hour,tmEnd->tm_min,tmEnd->tm_sec);
    //TextOut(hdc, 50, 80, czBuf);

    free(tmEnd);

}


int readCurveData(DB_CLIENT fp, uint32 oad_value, uint8 logic_id, uint16 info_num,uint32 buf_len)
{
    //
    int                ret = ERR_NORMAL;
    uint32             len     = 0;
    time_t             timep = 0;

    READ_RECORD_T      inReadRecord;
    OOP_METER_T        oopMeter;
    NOMAL_OAD_T        nomalOAD;
    OOP_OCTETVAR16_T   m_tmnAddr;

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&oopMeter, 0, sizeof(OOP_METER_T));

    time(&timep); //获取从1970至今过了多少秒，存入time_t类型的timep
    timep +=28800;
    //曲线的开始时间和结束时间
    //开始时间为当日0点0分0秒 结束时间 当日23点59分59秒
    time_t start,end;
    power_curve_TimeSetToday(&start,&end);

    inReadRecord.recordOAD.logicId = logic_id;
    inReadRecord.recordOAD.infoNum = info_num;
    inReadRecord.cType = COL_TM_START;
    inReadRecord.cStart = start + 28800;
    inReadRecord.cEnd = end + 28800;
    inReadRecord.sortType = DOWN_SORT;
    inReadRecord.recordOAD.optional = 1;
    inReadRecord.recordOAD.road.oadMain.value = 0x50020200;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = oad_value;

    inReadRecord.MAC.nNum = 6;

    if(info_num == 0)
    {
        memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
        nomalOAD.logicId = logic_id;
        nomalOAD.oad.value = 0x40010200;
        ret = db_read_nomal(fp, &nomalOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&m_tmnAddr, &len);
        if(0 != ret || len != sizeof(OOP_OCTETVAR16_T))
        {
            GUI_FMT_DEBUG(" db_read_nomal 0x40010200 err = %d!\n", ret);
            return -1;
        }
         memcpy(inReadRecord.MAC.value, m_tmnAddr.value, 6);
    }
    else
    {
        if (app_get_meter_list(info_num, &oopMeter) != ERR_OK)
        {
            GUI_FMT_DEBUG(" app_get_meter_list %d !\n", info_num);
            return -1;
        }
        memcpy(inReadRecord.MAC.value, oopMeter.basic.tsa.add, 6);
    }
    ret = db_readRecord(fp, &inReadRecord, buf_len);
    if (ret != ERR_OK)
    {
        GUI_FMT_DEBUG(" db_read_record %08x ret=%d !\n", oad_value, ret);
        return -1;
    }
    return 0;

}

uint8 groupCurveIndex = 0;
uint8 lastIndex = 0;
void power_curve_display_init(DWORD dwPluginId)
{
    uint32 oadvalue;
    memset(curPows, 0 , sizeof(int64)*96);
    oadvalue = 0x23010300 + (0x00010000) *groupCurveIndex;
    readCurveData(g_hUdbCenter,oadvalue,0,0,sizeof(int64));
    lastIndex = groupCurveIndex;
}

void power_curve_display_plate_draw(HDC hdc)
{
    ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top+16;
//    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);

    // 画坐标
    int nLeft = 10;
    int nTop = 48;
    int nBottom = 130;
    int nRight = 150;

    MoveTo(hdc, nLeft-1, nTop-1);
    LineTo(hdc, nLeft-3, nTop+8);
    MoveTo(hdc, nLeft-1, nTop-1);
    LineTo(hdc, nLeft+1, nTop+8);

    MoveTo(hdc, nRight+1, nBottom+1);
    LineTo(hdc, nRight-8, nBottom-1);
    LineTo(hdc, nRight+1, nBottom+1);
    LineTo(hdc, nRight-8, nBottom+3);

    MoveTo(hdc, nLeft-1, nTop-1);
    LineTo(hdc, nLeft-1, nBottom+1);
    LineTo(hdc, nRight+1, nBottom+1);

    double dSetp = (double)(nRight-nLeft-10)/96;   // 24个点
    // 画x轴时间
    int nTmpTop = nBottom+2;
    // 0点
    int nTmpLeft = nLeft;
    TextOut(hdc, nTmpLeft-3, nTmpTop, "0");

    // 5点
    nTmpLeft = (int)(dSetp*20)+nLeft;
    TextOut(hdc, nTmpLeft-3, nTmpTop, "5");
    MoveTo(hdc, nTmpLeft, nTmpTop);
    LineTo(hdc, nTmpLeft, nTmpTop-3);

    // 10点
    nTmpLeft = (int)(dSetp*40)+nLeft;
    TextOut(hdc, nTmpLeft-6, nTmpTop, "10");
    MoveTo(hdc, nTmpLeft, nTmpTop);
    LineTo(hdc, nTmpLeft, nTmpTop-3);

    // 15点
    nTmpLeft = (int)(dSetp*60)+nLeft;
    TextOut(hdc, nTmpLeft-6, nTmpTop, "15");
    MoveTo(hdc, nTmpLeft, nTmpTop);
    LineTo(hdc, nTmpLeft, nTmpTop-3);

    // 20点
    nTmpLeft = (int)(dSetp*80)+nLeft;
    TextOut(hdc, nTmpLeft-6, nTmpTop, "20");
    MoveTo(hdc, nTmpLeft, nTmpTop);
    LineTo(hdc, nTmpLeft, nTmpTop-3);

    TextOut(hdc,144, nTmpTop, "h");
}

void power_curve_Totaldraw(HDC hdc)
{
    power_curve_display_plate_draw(hdc);
    //总加组号变化时，刷新数据
    if(lastIndex != groupCurveIndex)
    {
        uint32 oadvalue;
        memset(curPows, 0 , sizeof(int64)*96);
        oadvalue = 0x23010300 + (0x00010000) *groupCurveIndex;
        readCurveData(g_hUdbCenter,oadvalue,0,0,sizeof(int64));
        lastIndex = groupCurveIndex;
    }
    char czBuf[100] = {0};
    SelectFont(hdc, g_guiComm.fontSmall);
    TextOut(hdc, 45, 20, "总加组");
    sprintf(czBuf, "%d", groupCurveIndex + 1);
    ITEM_SELECT;
    TextOut(hdc, 90, 20, czBuf);
    ITEM_NOT_SELECT;

    //首先遍历96个数据,求出其中的最大值
    int32 powMax = 0;
    for(uint32 i =0;i<sizeof(curPows)/sizeof(curPows[0]);i++)
    {
        if(curPows[i]> powMax)
            powMax = curPows[i];
    }
    
    //纵坐标最大值不应该被默认
    MEMZERO(czBuf, sizeof(czBuf));
    if(powMax < 10000)
    {
        sprintf(czBuf, "%.1f W", (double)powMax/10); //换算成W 
    }
    else
    {
        sprintf(czBuf, "%.2f kW", (double)powMax / 10000); //换算成KW 
    }

    TextOut(hdc, 4, 34, czBuf);
    GUI_FMT_DEBUG("powMax = %u!\n", powMax);
    
    // 绘制96个点,并将其连在一起
    int nLeft = 10;
    int nTop = 60;
    int nBottom = 130;
    int nRight = 140;
    
    double dSetp = (double)(nRight-nLeft)/96;   // 96个点
    int nHeight = nBottom-nTop;
    BOOL bMoveTo = FALSE;
    int nVal;
    for(int i = 0; i < 96; i ++)
    {
        double rate = 0;
        rate = (double)curPows[i]/powMax;
        if (rate >= 1)
            rate = 1;
        int nTmpTop = nBottom - (int)(rate*nHeight);
        nVal = nLeft+(int)(i*dSetp);
        if(bMoveTo == FALSE)
        {
            MoveTo(hdc, nVal, nTmpTop);
            // 考虑到有可能只有一个孤独的点,所以使用该函数设置点的值
            SetPixel(hdc, nVal, nTmpTop, PIXEL_black);
            bMoveTo = TRUE;
        }
        else
        {
            SetPixel(hdc, nVal, nTmpTop, PIXEL_black);
            LineTo(hdc, nVal, nTmpTop);
        }
    }
}

void pointReturn(char* pNewStr, void* pPara)
{
    uint32 nPoint = atoi(pNewStr);
    if(nPoint > CtrlGrp_ROUTE)
    {
        char bzbuf[100];
        sprintf(bzbuf, "总加组最大为%d\n确认是否重新输入?", CtrlGrp_ROUTE);
        if(TRUE == MessageBox(bzbuf, MB_OKCANCEL))
        {
            sprintf(bzbuf, "%d", groupCurveIndex + 1);
            InputBox(bzbuf, pointReturn, NULL, 0x01);
        }
    }
    else{
        groupCurveIndex = nPoint - 1;
    }
}

BOOL powerCurve_Key_Msg(int nkey)
{
    switch (nkey)
    {
        case KEY_CANCEL:
            groupCurveIndex = 0;
            CancelMenu();
            break;
        case KEY_UP:
        case KEY_LEFT:
            if (groupCurveIndex == 0)
                groupCurveIndex = 7;
            else
                groupCurveIndex--;
            power_curve_display_init(0x20020100);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        case KEY_DOWN:
        case KEY_RIGHT:
            if (groupCurveIndex == 7)
                groupCurveIndex = 0;
            else
                groupCurveIndex++;
            power_curve_display_init(0x20020100);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        case KEY_OK:// 确定时处理
            char bzbuf[50];
            sprintf(bzbuf, "%d", groupCurveIndex + 1);
            InputBox(bzbuf, pointReturn, NULL, 0x01);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
    }
    return TRUE;

}


//负荷曲线
PLUGIN_INFO g_layer_powerCurve_display = {
    0x00520106/*id*/,
    power_curve_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    power_curve_Totaldraw/*Draw*/,
    NULL/*Timer*/,
    powerCurve_Key_Msg/*keymsg*/,
    NULL/*ipc*/
};


#endif

//该函数处理功率电量等值显示,防止大数值跃屏
void valueDisplayProcess(int64 energy, char* str, size_t n,uint32 weight,uint8 unit)
{
    double val_kWh = energy;
    const char *units[8]={"W","Wh","var"};

    if (str == NULL || n <= 0)
    {
        return;
    }

    for(uint32 i =1;i<=weight;i++)
    {
        val_kWh = val_kWh *0.1;
    }

    //val_kWh = energy * 0.0001;//数据字典中约定内部按缩放10000倍存储电能量

    //单位自适应来解决数值过大不能显示全部的问题
    //求位数
    int64 val_autoAdap=(int64)val_kWh;
    uint8 digitsCnt=0;
    while(val_autoAdap)
    {
        val_autoAdap = val_autoAdap/10;
        digitsCnt++;
    }
    if(digitsCnt<=6)
    {
        //val_kWh = energy * 0.0001;//数据字典中约定内部按缩放10000倍存储电能量
        snprintf(str, n - 1, "%6.4fk%s", val_kWh,units[unit]);
        str[n - 1] = '\0';
    }
    else if(digitsCnt>6 && digitsCnt<=9)
    {
        val_kWh = val_kWh*0.001;//数据字典中约定内部按缩放10000倍存储电能量
        snprintf(str, n - 1, "%6.4fM%s", val_kWh,units[unit]);
        str[n - 1] = '\0';
    }
    else
    {
        val_kWh = val_kWh*0.001*0.001;//数据字典中约定内部按缩放10000倍存储电能量
        snprintf(str, n - 1, "%6.4fG%s", val_kWh,units[unit]);
        str[n - 1] = '\0';
    }
}


#if DESC("功控记录",1)
uint32 g_event_num;
int    g_event_index;

//功控记录
OOP_PCTRLLOG_T *g_pctrllogEventlist;
OOP_PCTRLLOG_T g_pctrllogEvent;
BOOL event_display_keymsg(int nKey)
{
    //g_event_num = 5;
    switch (nKey)
    {
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        g_event_index++;
        if ((uint32)g_event_index >= g_event_num)
        {
            g_event_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    case KEY_LEFT:
    {
        g_event_index--;
        if (g_event_index < 0)
        {
            g_event_index = (g_event_num==0)?0: g_event_num-1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}

void get_event_data(DB_CLIENT fp, uint32 oad_value, uint8** p_event_list, uint32 event_len)
{
    int                ret = ERR_NORMAL;
//    uint32             len     = 0;
    time_t             timep   = 0;
    uint32             handle  = 0;
    uint8*             ptmp    =  *p_event_list;

    READ_RECORD_T      inReadRecord;
    RECORD_UNIT_T      stepRecordOut;
//    OOP_METER_T        oopMeter;
//    NOMAL_OAD_T        nomalOAD;
//    OOP_OCTETVAR16_T   m_tmnAddr;
    STEP_READ_RECORD_T stepReadRecord;

    g_event_num = 0;
    g_event_index = 0;
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
//    memset(&oopMeter, 0, sizeof(OOP_METER_T));
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

//    inReadRecord.MAC.nNum = 6;
//
//    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
//    nomalOAD.logicId = 0;
//    nomalOAD.oad.value = 0x40010200;
//    ret = db_read_nomal(fp, &nomalOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&m_tmnAddr, &len);
//    if(0 != ret || len != sizeof(OOP_OCTETVAR16_T))
//    {
//        GUI_FMT_DEBUG(" db_read_nomal 0x40010200 err = %d!\n", ret);
//        return ;
//    }
//     memcpy(inReadRecord.MAC.value, m_tmnAddr.value, 6);

     ret = db_read_record_start(fp, &inReadRecord, &handle, &g_event_num);
    if (0 != ret || 0 == g_event_num)
    {
        GUI_FMT_DEBUG(" db_read_record_start  ret=%d, num%d!\n", ret, g_event_num);
        return ;
    }

    ptmp = (uint8*)malloc(event_len*g_event_num);
    if (ptmp == NULL)
    {
        GUI_FMT_DEBUG(" malloc  err !\n");
        g_event_num = 0;
        return ;
    }
    memset(ptmp, 0, event_len*g_event_num);
    
    memcpy(&stepReadRecord.MAC, &inReadRecord.MAC, sizeof(OOP_OCTETVAR64_T));

    for(uint32 i=0; i<g_event_num; i++)
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

void power_ctr_evt_init_GK(DWORD dwPluginId)
{
    g_pctrllogEventlist = NULL;
    get_event_data(g_hUdbCenter, 0x32000200, (uint8**)&g_pctrllogEventlist, sizeof(OOP_PCTRLLOG_T));
}
void power_ctr_evt_Uninit()
{
    if (g_pctrllogEventlist != NULL)
    {
        free(g_pctrllogEventlist);
        g_pctrllogEventlist = NULL;
    }
}

void power_ctr_evt_draw_GK(HDC hdc)
{

    char czBuf[64] = { 0 };
    int nLeft = 10;
    int nLeft2 = 50;
    int nTop = 20;
    int tmpTop = 18;
    int i = 0;
    //float tmp;

    memset(&g_pctrllogEvent, 0, sizeof(OOP_PCTRLLOG_T));
    if (g_pctrllogEventlist != NULL)
    {
        g_pctrllogEvent = g_pctrllogEventlist[g_event_index];      
    }

    switch (g_pctrllogEvent.source.oi)
    {
    case 0x8103:
        TextOut(hdc, 40, 20, "时段控记录");
        break;
    case 0x8104:
        TextOut(hdc, 40, 20, "厂休控记录");
        break;
    case 0x8105:
        TextOut(hdc, 40, 20, "营业报停控记录");
        break;
    case 0x8106:
        TextOut(hdc, 40, 20, "下浮控记录");
        break;
    default:
        TextOut(hdc, 50, 20, "功控记录");
        break;
    }

    nTop += tmpTop;
    if (g_pctrllogEvent.turnState.nValue != 0)
    {
        for (i = 0; i < 8; i++)
        {
            if ((g_pctrllogEvent.turnState.nValue >> i & 0x01) != 0)
            {
                sprintf(czBuf,"轮次 %d ", i + 1);
                break;
            }
        }
    }
    else
    {
        sprintf(czBuf,"轮次 - ");
    }
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    if (g_pctrllogEvent.objCtrl == 0)
    {
        sprintf(czBuf, "控制对象: 总加组 -");
    }
    else
    {
        sprintf(czBuf, "控制对象: 总加组%d", g_pctrllogEvent.objCtrl&0x0F);
    }
    TextOut(hdc, nLeft, nTop, czBuf);

    nLeft2 += 25;

    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "跳闸前功率");
    if ((g_pctrllogEvent.nIndex == 0) || (g_pctrllogEvent.power == unlawPower))
    {
        sprintf(czBuf, "-------- kW");
    }
    else
    {
        //tmp = g_pctrllogEvent.power;
        valueDisplayProcess(g_pctrllogEvent.power,czBuf,30,4,0);
        //sprintf(czBuf, "%6.4fkW", tmp / 10000);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "跳闸后功率");
    if ((g_pctrllogEvent.nIndex == 0)||(g_pctrllogEvent.power2Min == unlawPower))
    {
        sprintf(czBuf, "-------- kW");
    }
    else
    {
        //tmp = g_pctrllogEvent.power2Min;
        valueDisplayProcess(g_pctrllogEvent.power2Min,czBuf,30,4,0);
        //sprintf(czBuf, "%6.4fkW", tmp / 10000);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "当前定值");
    if (g_pctrllogEvent.nIndex == 0)
    {
        sprintf(czBuf, "-------- kW");
    }
    else
    {
        //tmp = g_pctrllogEvent.llSetting;
        valueDisplayProcess(g_pctrllogEvent.llSetting,czBuf,30,4,0);
        //sprintf(czBuf, "%6.4fkW", tmp / 10000);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);


    nTop += tmpTop;

    if (g_pctrllogEvent.tmStart.year == 0)
    {
        sprintf(czBuf, "--年-月-日-:-");
    }
    else
    {
        sprintf(czBuf, "%04hu年%02hu月%02hu日%02hu:%02hu",
            g_pctrllogEvent.tmStart.year,
            g_pctrllogEvent.tmStart.month,
            g_pctrllogEvent.tmStart.day,
            g_pctrllogEvent.tmStart.hour,
            g_pctrllogEvent.tmStart.minute);
    }
    TextOut(hdc, 30, nTop, czBuf);

}

//功控记录
PLUGIN_INFO g_layer_power_ctr_evt_display_GK = {
    0x11010022/*id*/,
    power_ctr_evt_init_GK/*InitLayer*/,
    power_ctr_evt_Uninit/*UninitLayer*/,
    power_ctr_evt_draw_GK/*Draw*/,
    NULL/*Timer*/,
    event_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif


#if DESC("失电记录",1)
OOP_POWERLOG_T *g_powerlogEventlist;
OOP_POWERLOG_T g_powerlogEvent[20];

void pwct_ctr_evt_init(DWORD dwPluginId)
{
    uint32 i = 0;
    
    g_event_index = 0;
    g_event_num = 0;
    g_powerlogEventlist = NULL;
    get_event_data(g_hUdbCenter, 0x31060200, (uint8**)&g_powerlogEventlist, sizeof(OOP_POWERLOG_T));

    //获取停电记录单元
    
    memset(g_powerlogEvent, 0, sizeof(OOP_POWERLOG_T)*15);
//    if ((g_powerlogEventlist != NULL))
//    {
//        for(uint32 j = 0;j < 15; j++)
//        {
//            //(g_powerlogEvent.source.nValue = 1 表示该事件是一个上电事件,其中记录了停电和回复时间
//            //if((g_powerlogEventlist[j].source.nValue == 1)&&(g_powerlogEventlist[j].tmStart.year !=0))
//            if(g_powerlogEventlist[j].tmStart.year !=0)
//            {   //遍历事件list,从记录单元中获取停电的记录(事件发生源enum=停电)
//                g_powerlogEvent[i++] = g_powerlogEventlist[j];
//            }
//        }
//        g_event_num = i;
//    }

    if ((g_powerlogEventlist != NULL))
    {
        for(uint32 j = 0;j < g_event_num; j++)
        {
//            if(j<=g_event_num)
//            {   //遍历事件list,从记录单元中获取停电的记录(事件发生源enum=停电)
//                g_powerlogEvent[i++] = g_powerlogEventlist[j];
//            }
            g_powerlogEvent[i++] = g_powerlogEventlist[j];
        }
    }
    
}
void pwct_ctr_evt_Uninit()
{
    if (g_powerlogEventlist != NULL)
    {
        free(g_powerlogEventlist);
        g_powerlogEventlist = NULL;
    }
}

void pwct_ctr_rcrd_Platedraw(HDC hdc)
{
    ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);

    TextOut(hdc, 50, 30, "失电记录");

    TextOut(hdc, 10, 70, "起始时间");
    TextOut(hdc, 10, 110, "恢复时间");
    
}


void pwct_ctr_rcrd_TotalDraw(HDC hdc)
{
    //pwct_ctr_evt_init(0x20070000);
    char czBuf[40] = { 0 };
    //首先画一遍模板

    if ((g_powerlogEventlist != NULL) && (g_event_num != 0))
    {
        pwct_ctr_rcrd_Platedraw(hdc);

//        sprintf(czBuf, "第%d条记录 ",g_event_index + 1);
//        TextOut(hdc, 10, 50, czBuf);
        sprintf(czBuf, "%d ",g_event_index + 1);
        TextOut(hdc, 110, 30, czBuf);
        //sprintf(czBuf, "类型: %s",g_powerlogEvent[g_event_index].source.nValue==0?"停电":"上电");
        sprintf(czBuf, "类型: %s",g_powerlogEventlist[g_event_index].source.nValue==0?"停电":"上电");
        TextOut(hdc, 10, 50, czBuf);

        
        bzero(czBuf,sizeof(czBuf));
//        if(g_powerlogEvent[g_event_index].tmStart.year!=0)
//        sprintf(czBuf, "%04hu年%02hu月%02hu日%02hu:%02hu:%02hu",
//                    g_powerlogEvent[g_event_index].tmStart.year,
//                    g_powerlogEvent[g_event_index].tmStart.month,
//                    g_powerlogEvent[g_event_index].tmStart.day,
//                    g_powerlogEvent[g_event_index].tmStart.hour,
//                    g_powerlogEvent[g_event_index].tmStart.minute,
//                    g_powerlogEvent[g_event_index].tmStart.second);

        if(g_powerlogEventlist[g_event_index].tmStart.year!=0)
        sprintf(czBuf, "%04hu年%02hu月%02hu日%02hu:%02hu:%02hu",
                    g_powerlogEventlist[g_event_index].tmStart.year,
                    g_powerlogEventlist[g_event_index].tmStart.month,
                    g_powerlogEventlist[g_event_index].tmStart.day,
                    g_powerlogEventlist[g_event_index].tmStart.hour,
                    g_powerlogEventlist[g_event_index].tmStart.minute,
                    g_powerlogEventlist[g_event_index].tmStart.second);
        
        TextOut(hdc, 10, 90, czBuf);
        bzero(czBuf,sizeof(czBuf));
//        if(g_powerlogEvent[g_event_index].tmEnd.year!=0)
//        sprintf(czBuf, "%04hu年%02hu月%02hu日%02hu:%02hu:%02hu",
//                    g_powerlogEvent[g_event_index].tmEnd.year,
//                    g_powerlogEvent[g_event_index].tmEnd.month,
//                    g_powerlogEvent[g_event_index].tmEnd.day,
//                    g_powerlogEvent[g_event_index].tmEnd.hour,
//                    g_powerlogEvent[g_event_index].tmEnd.minute,
//                    g_powerlogEvent[g_event_index].tmEnd.second);

        if(g_powerlogEventlist[g_event_index].tmEnd.year!=0)
        sprintf(czBuf, "%04hu年%02hu月%02hu日%02hu:%02hu:%02hu",
                    g_powerlogEventlist[g_event_index].tmEnd.year,
                    g_powerlogEventlist[g_event_index].tmEnd.month,
                    g_powerlogEventlist[g_event_index].tmEnd.day,
                    g_powerlogEventlist[g_event_index].tmEnd.hour,
                    g_powerlogEventlist[g_event_index].tmEnd.minute,
                    g_powerlogEventlist[g_event_index].tmEnd.second);
        TextOut(hdc, 10, 130, czBuf);
    }
    else
    {
        //没有停电记录
        TextOut(hdc, 40, 80, "没有停电记录");
    }
}

void pwct_ctr_rcrd_Draw(HDC hdc)
{
    pwct_ctr_rcrd_TotalDraw(hdc);

}


//失电记录
PLUGIN_INFO g_layer_pwct_ctr_evt_display = {
    0x00070000/*id*/,
    pwct_ctr_evt_init/*InitLayer*/,
    pwct_ctr_evt_Uninit/*UninitLayer*/,
    pwct_ctr_rcrd_Draw/*Draw*/,
    NULL/*Timer*/,
    event_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif


#if DESC("遥控记录",1)
OOP_RCTRLLOG_T *g_rctrllogEventlist;
OOP_RCTRLLOG_T g_rctrllogEvent;

//遥控记录单元中的prePower[8]和llPower[8]存放着8轮次的跳闸前和跳闸后功率
//oad中的index存放着轮次值

void rct_ctr_evt_init(DWORD dwPluginId)
{
    g_event_index = 0;
    g_rctrllogEventlist = NULL;
    get_event_data(g_hUdbCenter, 0x31150200, (uint8**)&g_rctrllogEventlist, sizeof(OOP_RCTRLLOG_T));
}

void rct_ctr_evt_Uninit()
{
    if (g_rctrllogEventlist != NULL)
    {
        free(g_rctrllogEventlist);
        g_rctrllogEventlist = NULL;
    }
}


void rct_ctr_rcrd_Platedraw(HDC hdc)
{
    ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);

    TextOut(hdc, 50, 30, "遥控记录");

    TextOut(hdc, 10, 90, "跳闸前功率");
    TextOut(hdc, 10, 110, "跳闸后功率");
    
}

void rct_ctr_rcrd_TotalDraw(HDC hdc)
{
    char czBuf[40] = { 0 };

    //首先画一遍模板
    rct_ctr_rcrd_Platedraw(hdc);

    //先获取每个每个轮次的事件结构
    memset(&g_rctrllogEvent, 0, sizeof(OOP_RCTRLLOG_T));
    if (g_rctrllogEventlist != NULL)
    {
        g_rctrllogEvent = g_rctrllogEventlist[g_event_index];
    }

    //获取当前轮次
    uint8 turn  = g_rctrllogEvent.source.oad.nIndex;
    //确保读到有效数据
    if(turn != 0)
    {
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf,"继电器 %d ", turn);
        TextOut(hdc, 10, 50, czBuf);

        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf,"%04hu年%02hu月%02hu日%02hu:%02hu",g_rctrllogEvent.tmStart.year,
                                g_rctrllogEvent.tmStart.month,
                                g_rctrllogEvent.tmStart.day,
                                g_rctrllogEvent.tmStart.hour,
                                g_rctrllogEvent.tmStart.minute);
        TextOut(hdc, 10, 70, czBuf);

        bzero(czBuf,sizeof(czBuf));
        if (g_rctrllogEvent.llPower[0] == unlawPower)
            sprintf(czBuf,"---- kW");
        else
            //sprintf(czBuf,"%6.4fkW",(float)g_rctrllogEvent.llPower[0] / 10000);
            valueDisplayProcess(g_rctrllogEvent.llPower[0],czBuf,30,4,0);
        TextOut(hdc, 73,90, czBuf);

        bzero(czBuf,sizeof(czBuf));

        if (g_rctrllogEvent.prePower[0] == unlawPower)
            sprintf(czBuf,"---- kW");
        else
            //sprintf(czBuf,"%6.4fkW",(float)g_rctrllogEvent.prePower[0] / 10000);
            valueDisplayProcess(g_rctrllogEvent.prePower[0],czBuf,30,4,0);
        TextOut(hdc, 73,110, czBuf);

    }
    else
    {
        //没有读到数据
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf,"继电器 - ");
        TextOut(hdc, 10, 50, czBuf);

        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf,"--年-月-日-:- ");
        TextOut(hdc, 10, 70, czBuf);
        
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf,"---- kW");
        TextOut(hdc, 75, 90, czBuf);

        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf,"---- kW");
        TextOut(hdc, 75, 110, czBuf);
        
    }   
}

void rct_ctr_rcrd_Draw(HDC hdc)
{
    rct_ctr_rcrd_TotalDraw(hdc);

}

//遥控记录
PLUGIN_INFO g_layer_rct_ctr_evt_display = {
    0x00060000/*id*/,
    rct_ctr_evt_init/*InitLayer*/,
    rct_ctr_evt_Uninit/*UninitLayer*/,
    rct_ctr_rcrd_Draw/*Draw*/, 
    NULL/*Timer*/,
    event_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif


#if DESC("电控记录",1)

OOP_ECTRLLOG_T *g_ectrllogEventlist;
OOP_ECTRLLOG_T g_ectrllogEvent;
uint8 bugEneryType;

void elect_ctr_evt_init(DWORD dwPluginId)
{
    g_event_index = 0;
    g_ectrllogEventlist = NULL;
    get_event_data(g_hUdbCenter, 0x32010200, (uint8**)&g_ectrllogEventlist, sizeof(OOP_ECTRLLOG_T));
    bugEneryType = -1;
}
void elect_ctr_evt_Uninit()
{
    if (g_ectrllogEventlist != NULL)
    {
        free(g_ectrllogEventlist);
        g_ectrllogEventlist = NULL;
    }
}

void elect_ctr_rcrd_Platedraw(HDC hdc)
{
    ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);
    if (g_ectrllogEvent.source.oi == 0x8107)
    {
        TextOut(hdc, 50, 30, "购电控记录");
//        if(bugEneryType==0)
//            TextOut(hdc, 10, 110, "跳闸时剩余电量");
//        else if(bugEneryType ==1)
//            TextOut(hdc, 10, 110, "跳闸时剩余电费");
        //注意,698规约中不能区分电量控还是电费控,购电控的单位统一写为kwh/元
        TextOut(hdc, 10, 110, "跳闸时剩余电量/费");
    }
    else if(g_ectrllogEvent.source.oi == 0x8108)
    {
        TextOut(hdc, 50, 30, "月电控记录");
        TextOut(hdc, 10, 110, "跳闸时当月总电量");
    }
    else
    {
        TextOut(hdc, 50, 30, "电控记录");
        //TextOut(hdc, 60, 40, "跳闸前功率");
        TextOut(hdc, 10, 110, "跳闸时电能量");
        //TextOut(hdc, 60, 100, "当时定值");
    }
    
}


void elect_ctr_rcrd_TotalDraw(HDC hdc)
{
    char czBuf[30] = { 0 };
    //获取第i个轮次的电控跳闸记录单元
    memset(&g_ectrllogEvent, 0, sizeof(OOP_ECTRLLOG_T));
    if (g_ectrllogEventlist != NULL)
    {
        g_ectrllogEvent = g_ectrllogEventlist[g_event_index];
    }

    //如果是购电控,那么还需要读一下是购电方式是电量还是电费
    if(g_ectrllogEvent.source.oi==0x8107)
    {
        uint16 grp = g_ectrllogEvent.objCtrl;

        //读一下购电控记录
        memset(&tBuyenergyconfig, 0, sizeof(tBuyenergyconfig));
        read_db_noamal_data(g_hUdbCenter, 0x81070200, 0, 0, (uint8*)&tBuyenergyconfig, sizeof(OOP_8107_CONFIG_T));
        
        //找到跳闸时对应的总加组
        //int index = -1;
        for (uint32 i = 0; i < tBuyenergyconfig.nNum; i++)
        {
            if (tBuyenergyconfig.item[i].object == grp)
            {
                //index = i;
                bugEneryType = tBuyenergyconfig.item[i].type;
                break;
            }
        }
        
    }

    //首先画一遍模板
    elect_ctr_rcrd_Platedraw(hdc);

    //画轮次
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;
    int j = 0;
    if(g_ectrllogEvent.turnState.nValue == 0)
    {
        sprintf(czBuf, "%s", "轮次 -");
        TextOut(hdc, 10, 50, czBuf);
    }
        
    else
    {
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf, "%s ", "轮次");
        TextOut(hdc, 10, 50, czBuf);
        for (int i = 0; i < 8; i++)
        {
            if ((g_ectrllogEvent.turnState.nValue >> i & 0x01) != 0)
            {
                sprintf(czBuf, "%d ", i+1);
                TextOut(hdc, 38 + 10*j++, 50, czBuf);
            }
        }
    }

    if (g_ectrllogEvent.objCtrl == 0)
    {
        sprintf(czBuf, "控制对象: 总加组 -");
    }
    else
    {
        sprintf(czBuf, "控制对象: 总加组%d", g_ectrllogEvent.objCtrl&0x0F);
    }
    TextOut(hdc, 10, 70, czBuf);

    //画时间
    SelectFont(hdc, g_guiComm.fontSmall);
    bzero(czBuf,sizeof(czBuf));
    if(g_ectrllogEvent.tmStart.year == 0)
        sprintf(czBuf,  "--年-月-日-:-");
    else
    {
        sprintf(czBuf, "%04d年%02d月%02d日%02d:%02d ",        (uint16)g_ectrllogEvent.tmStart.year,
                                                    (uint8)g_ectrllogEvent.tmStart.month,
                                                    (uint8)g_ectrllogEvent.tmStart.day,
                                                    (uint8)g_ectrllogEvent.tmStart.hour,
                                                    (uint8)g_ectrllogEvent.tmStart.minute);
    }
    TextOut(hdc, 10, 90, czBuf);


    //画跳闸时剩余电量:energy?
    SelectFont(hdc, g_guiComm.fontSmall);
    bzero(czBuf,sizeof(czBuf));
    if (g_ectrllogEvent.nIndex == 0)
    {
        if (g_ectrllogEvent.source.oi == 0x8107) //购电控需要判断是电量还是电费
            //bugEneryType==0? sprintf(czBuf, "--- kWh"):sprintf(czBuf, "--- 元");
            //购电控的剩余电量单位统一为kwh/元
            sprintf(czBuf, "--- kwh/元");
        else
            sprintf(czBuf, "--- kWh");
    }
    else
    {
        float64 tmp = g_ectrllogEvent.energy;
        if (g_ectrllogEvent.source.oi == 0x8107)//购电控需要判断是电量还是电费
            //bugEneryType==0? sprintf(czBuf, "%6.4f kWh", tmp / 10000):sprintf(czBuf, "%6.4f 元", tmp / 10000);
            //购电控的单位为kwh/元
            sprintf(czBuf, "%6.4f kWh/元", tmp / 10000);
        else
            sprintf(czBuf, "%6.4f kWh", tmp / 10000);
        
    }
    TextOut(hdc, 10, 130, czBuf);
}



void elect_ctr_rcrd_Draw(HDC hdc)
{
    elect_ctr_rcrd_TotalDraw(hdc);

}


//电控记录
PLUGIN_INFO g_layer_elect_ctr_evt_display = {
    0x00050000/*id*/,
    elect_ctr_evt_init/*InitLayer*/,
    elect_ctr_evt_Uninit/*UninitLayer*/,
    elect_ctr_rcrd_Draw/*Draw*/,
    NULL/*Timer*/,
    event_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("恒定磁场干扰事件",1)

OOP_EVENT_T g_magneticEvent;
OOP_EVENT_T *g_magneticEventlist;
void magneticEvent_display_init(DWORD dwPluginId)
{
    g_magneticEventlist = NULL;
    get_event_data(g_hUdbCenter, 0x302a0200, (uint8**)&g_magneticEventlist, sizeof(OOP_EVENT_T));
}
void magneticEvent_display_Uninit()
{
    if (g_magneticEventlist != NULL)
    {
        free(g_magneticEventlist);
        g_magneticEventlist = NULL;
    }
    
}

//恒定磁场干扰事件
void magneticEvent_display_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);

    char czBuf[64] = { 0 };

    int nLeft = 15;
    int nLeft2 = 85;
    int nTop = 22;
    int tmpTop = 25;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = nTop;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "磁场干扰事件");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    memset(&g_magneticEvent, 0, sizeof(OOP_EVENT_T));
    if (g_magneticEventlist != NULL)
    {
        g_magneticEvent = g_magneticEventlist[g_event_index];
    }

    //磁场干扰事件
    nTop += tmpTop + 10;
    TextOut(hdc, nLeft, nTop, "记录序号");
    sprintf(czBuf, "%u", g_magneticEvent.nIndex);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "发生时间");
    if (g_magneticEvent.tmStart.year == 0 && g_magneticEvent.tmStart.month == 0 && g_magneticEvent.nIndex == 0)
    {
        sprintf(czBuf, "--年--月--日--:--");
    }
    else
    {
        sprintf(czBuf, "%04hu-%02hu-%02hu %02hu:%02hu:%02hu",
            g_magneticEvent.tmStart.year,
            g_magneticEvent.tmStart.month,
            g_magneticEvent.tmStart.day,
            g_magneticEvent.tmStart.hour,
            g_magneticEvent.tmStart.minute,
            g_magneticEvent.tmStart.second);
    }

    nTop += tmpTop;
    TextOut(hdc, 30, nTop, czBuf);

    
}

//恒定磁场干扰事件
PLUGIN_INFO g_layer_magneticEvent_display = {
    0x00080000/*id*/,
    magneticEvent_display_init/*InitLayer*/,
    magneticEvent_display_Uninit/*UninitLayer*/,
    magneticEvent_display_draw/*Draw*/,
    NULL/*Timer*/,
    event_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif


#if DESC("电压相角",1)
void ac_vol_xj_display_init(DWORD dwPluginId)
{
    memset(&display_data.xj, 0, sizeof(OOP_ENERGY_T));
    //    if(tasfd == 0)
    {
        read_db_noamal_data(g_hUdbCenter, 0x20020200, 1, 0, (uint8*)&display_data.xj, sizeof(OOP_WORD3V_T));
    }
    //   else
    {
        //       read_db_record_data(g_hUdbCenter, 0x20000200, 0, taskid, (uint8*)&display_data.vol, sizeof(OOP_DWORD3V_T));
        //        read_db_record_data(g_hUdbCenter, 0x20010200, 0, taskid, (uint8*)&display_data.cur, sizeof(OOP_INT3V_T));
    }

}

void vol_xj_display_draw(HDC hdc, DISPLAY_DATA_INFO bdisplay_data)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    float tmp;
    int i = 0;

    char czBuf[64] = { 0 };

    int nLeft2 = 30;
    int nTop = 42;
    int tmpTop = 14;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "电压相角");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontSmall);

    // 电压相角
    for (i = 0; i < 3; i++)
    {
        if (bdisplay_data.xj.nNum == 0 || get_bit_value((uint8 *)&bdisplay_data.xj.rsv,32,i) == 0)
        {
            sprintf(czBuf, "%s --- V", g_showTitleXj[i]);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            tmp = (float)bdisplay_data.xj.nValue[i] / 10;
            sprintf(czBuf, "%s %2.1f V", g_showTitleXj[i], tmp);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        nTop += tmpTop;
    }

   
}

void vol_xj_display_draws(HDC hdc)
{
    read_db_noamal_data(g_hUdbCenter, 0x20020200, 1, 0, (uint8*)&display_data.xj, sizeof(OOP_WORD3V_T));
    vol_xj_display_draw(hdc, display_data);
}


//电压相角
PLUGIN_INFO g_layer_ac_vol_xj_display = {
    0x00990002/*id*/,
    ac_vol_xj_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    vol_xj_display_draws/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("客户编号", 1)

void clientno_set_init(HDC hdc)
{
    get_clientno(LocalAddr);
}

void clientno_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "终端客户编号");
    rc.top = 26;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, pg_guicomm->fontSmall);
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], 
                                               LocalAddr[5]);
    rc.top += 30;
    rc.bottom = rc.top + 15;
    ITEM_SELECT;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    ITEM_NOT_SELECT;
}

void clientnoReturn(char* pNewStr, void* pPara)
{
    int nLen = strlen(pNewStr);
    int ret;
    
    if (nLen > 12 && TRUE == MessageBox("OOP客户编号长度大于6字节\n\n确认是否重新输入?", MB_OKCANCEL
))
    {
        char tmp[20] = { 0 };
        sprintf(tmp, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], 
                                                 LocalAddr[5]);
        InputBox(tmp, AddrReturn, NULL, 0x01);
    }
    else
    {
        char tmp[20] = { 0 };
        strcpy(tmp, "000000000000");
        strcpy(tmp + 12 - nLen, pNewStr);
        printf("输入的地址为 %s \n", tmp);
        commfun_ASC2BCD((char*)LocalAddr, tmp, 12, 0);
        printf("转换为bcd %02x%02x%02x%02x%02x%02x \n", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], 
                                                        LocalAddr[4], LocalAddr[5]);
        OOP_OCTETVAR16_T TmpLocalAddr;
        memset(&TmpLocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
        TmpLocalAddr.nNum = 6;
        memcpy(TmpLocalAddr.value, LocalAddr, 6);
        OOP_OAD_U oad = { 0x40030200 };
        ret = write_normal_data((uint8*)&TmpLocalAddr, sizeof(OOP_OCTETVAR16_T), oad, 0, 0, 1);
        if (ret == 0)
        {
            MessageBox("客户编号设置成功");
        }
        else
        {
            MessageBox("客户编号设置失败");
        }
    }
}

BOOL clientno_set_keymsg(int nKey)
{

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
        case KEY_OK:
        {
            if (ValidatePwd() == TRUE)
            {
                char tmp[20] = {0};
                sprintf(tmp, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], 
                                                         LocalAddr[4], LocalAddr[5]);
                InputBox(tmp, clientnoReturn, NULL, 0x01);
                return TRUE;
            }
            break;
        }
        default: break;
    }
    return FALSE;
}

//通信地址设置
PLUGIN_INFO g_layer_clientno_set = {
    0x11030021/*id*/,
    clientno_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    clientno_set_draw/*Draw*/,
    NULL/*Timer*/,
    clientno_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif



#if DESC("ESAM下发状态",1)


void aesam_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;


    char czBuf[64] = { 0 };

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 36;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "ESAM下发状态");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);


    if (toolInfoFresh.keyStatus == 1)
    {
        sprintf(czBuf, "已下发");
    }
    else
    {
        sprintf(czBuf, "未下发");
    }

    rc.top = 56;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

   
}

//ESAM下发状态
PLUGIN_INFO g_layer_aesam = {
    0x1104000a/*id*/,
    NULL/*InitLayer*/,
    NULL/*UninitLayer*/,
    aesam_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("485-2口设置",1)

uint8 port_set_index;
SERIALPORT data485Set;
int exist;
void port_set_485_init(HDC hdc)
{
    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0};
    uint32 len = 0;
    exist = 0;
    memset(&data485Set, 0, sizeof(data485Set));
    normal.oad.value = 0xF2010202;
    ret = db_read_nomal(g_hUdbCenter, &normal, sizeof(SERIALPORT), (uint8*)&data485Set, &len);
    if((ret != ERR_OK) || (len == 0))
    {
        GUI_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
        ret, normal.logicId, normal.infoNum, normal.oad.value);
    }else{
        exist = 1;
    }
    port_set_index = 0;
}

void port_set_485_draw(HDC hdc)
{
    char czBuf[300] = { 0 };
    int index = 0;
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;

    TextOut(hdc, nLeft1+20, nTop, "485-2口设置");
    SelectFont(hdc, pg_guicomm->fontSmall);

    nTop += 30;
    TextOut(hdc, nLeft1, nTop, "端口描述：485-2");

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "波特率：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (exist == 0)
    {
        sprintf(czBuf, "9600bps");
        data485Set.param.baud = 6;
    }
    else
    {
        uint8 index = data485Set.param.baud;
        if (index > 11)
            index = 11;
        sprintf(czBuf, "%s", g_baud[index]);
    }
    if (port_set_index == index)
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
    TextOut(hdc, nLeft1, nTop, "端口功能：");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (exist == 0)
    {
        sprintf(czBuf, "自适应");
    }
    else
    {
        uint8 index = data485Set.func;
        if ((index<0)||(index > 2))
            sprintf(czBuf, "自适应");
        else
            sprintf(czBuf, "%s", g_Port_type[index]);
    }
    if (port_set_index == index)
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
    if (port_set_index == index)
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
    int nLeft3 = 90;
    if (port_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, "保存设置");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, "保存设置");
    }

    PopListDraw(hdc);
}

BOOL port_set_485_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 3;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;
        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (port_set_index)
            {
            case 0://波特率
            {
                data485Set.param.baud = nSel;
            }
            break;
            case 1://端口功能
            {
                data485Set.func = nSel;
            }
            break;
            default:
                return FALSE;
            }
            exist = 1;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }


    switch (nKey)
    {
        case KEY_UP:
        {
            if (port_set_index > 0)
                port_set_index--;
            else
                port_set_index = maxindex;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        {
            if (port_set_index < maxindex)
                port_set_index++;
            else
                port_set_index = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            if (ValidatePwd() == TRUE)
            {
                //设置波特率
                if (port_set_index == 0)
                {
                    printf("设置波特率\n");
                    PopList(g_baud, 12, g_baud[0], 70, 60, FALSE);
                    return TRUE;
                }
                //端口功能
                if (port_set_index == 1)
                {
                    printf("设置端口功能\n");
                    PopList(g_Port_type, 3, g_Port_type[0], 70, 80, FALSE);
                    return TRUE;
                }
                //取消
                if (port_set_index == 2)
                {
                    if (TRUE == MessageBox("当前配置没有保存\n\n是否确定保存配置？", MB_OKCANCEL))
                    {
                        ret = write_normal_data((uint8*)&data485Set, sizeof(SERIALPORT), RS485Oad, 0, 0, 2);
                        if (ret == 0)
                        {
                            MessageBox("当前配置保存成功！");
                        }
                    }
                    port_set_index = 0;
                    return FALSE;
                }
                //保存配置
                if (port_set_index == 3)
                {
                    ret = write_normal_data((uint8*)&data485Set, sizeof(SERIALPORT), RS485Oad, 0, 0, 2);
                    if (ret == 0)
                    {
                        MessageBox("当前配置保存成功！");
                        port_set_index = 0;
                        return TRUE;
                    }
                    else
                    {
                        MessageBox("当前配置保存失败！");
                        return FALSE;
                    }

                }
            }
        }
    }
    return FALSE;
}

//RS485-II端口设置
PLUGIN_INFO g_port_set_485 = {
    0x00A00100/*id*/,
    port_set_485_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    port_set_485_draw/*Draw*/,
    NULL/*Timer*/,
    port_set_485_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#ifdef AREA_FUJIAN

uint8 gPortFun; //端口功能
const char *gFunName[]={"485","CAN"};
uint8 gPortFunIndex;

void fun_4852_init(HDC hdc)
{
    gPortFun = 0xff;
    int ret = gui_485_workmode_get();
    if(ret < 0)
    {
        GUI_FMT_DEBUG("获取4852口功能失败 ret %d\n",ret);
        gPortFun = 0;
    }
    gPortFunIndex = 0;
}

void fun_4852_draw(HDC hdc)
{

    char czBuf[300] = { 0 };
    int index = 0;
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 80;

    TextOut(hdc, nLeft1+10, nTop, "485-2口功能切换");
    SelectFont(hdc, pg_guicomm->fontSmall);

    nTop += 30;
    TextOut(hdc, nLeft1, nTop, "当前功能：");
    if(gPortFun > 1  )
        strcpy(czBuf,"--");
    else
        sprintf(czBuf,"%s",gFunName[gPortFun]);
    if(gPortFunIndex == index )
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }else
        TextOut(hdc, nLeft2, nTop, czBuf);
    
    index++;
    nTop += 50;
    nLeft1+=10;
    if (gPortFunIndex == index)
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
    int nLeft3 = 90;
    if (gPortFunIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, "保存");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, "保存");
    }

    PopListDraw(hdc);
}

BOOL fun_4852_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 2;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;
        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            
            gPortFun = nSel;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }


    switch (nKey)
    {
        case KEY_UP:
        {
            if (gPortFunIndex > 0)
                gPortFunIndex--;
            else
                gPortFunIndex = maxindex;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        {
            if (gPortFunIndex < maxindex)
                gPortFunIndex++;
            else
                gPortFunIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            if (ValidatePwd() == TRUE)
            {
                //设置
                if (gPortFunIndex == 0)
                {
                    PopList(gFunName, 2, gFunName[0], 70, 50, FALSE);
                    return TRUE;
                }
                //取消
                if (gPortFunIndex == 1)
                {
                    if (TRUE == MessageBox("当前配置没有保存\n\n是否确定保存配置？", MB_OKCANCEL))
                    {
                        ret = gui_485_workmode_set(gPortFun);
                        if (ret == 0)
                        {
                            MessageBox("当前配置保存成功！");
                        }
                    }
                    gPortFunIndex = 0;
                    return FALSE;
                }
                //保存配置
                if (gPortFunIndex == 2)
                {
                    ret = gui_485_workmode_set(gPortFun);
                    if (ret == 0)
                    {
                        MessageBox("当前配置保存成功！");
                        gPortFunIndex = 0;
                        return TRUE;
                    }
                    else
                    {
                        MessageBox("当前配置保存失败！");
                        return FALSE;
                    }

                }
            }
        }
    }
    return FALSE;

}
//福建485和can切换改为界面上软件控制
PLUGIN_INFO g_4852_fun = {
    0x11050001/*id*/,
    fun_4852_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    fun_4852_draw/*Draw*/,
    NULL/*Timer*/,
    fun_4852_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("密钥版本显示",1)

TESAMInfo_t tESAMInfoBuff;
void symKey_Init(HDC hdc)
{
    memset(&tESAMInfoBuff, 0, sizeof(TESAMInfo_t));
#if defined (PRODUCT_CCU)
    read_pdata_xram("/data/app/puAmr/cfg/tESAMInfo", (char*)&tESAMInfoBuff, 0, sizeof(TESAMInfo_t));
#endif

#if defined (PRODUCT_ZCU_3)
    read_pdata_xram("/data/app/stAmr/cfg/tESAMInfo", (char*)&tESAMInfoBuff, 0, sizeof(TESAMInfo_t));
#endif
}

void symKey_Draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if(pg_guicomm == NULL)
       return;

    SelectFont(hdc,pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;
    int esamFlag = 0;
    int nTop = 20;
    int nLeft = 5;
    char pcz[150];
    for (int i = 0;i < 16; i++){
        if(tESAMInfoBuff.symKeyVersion[i] != 0){
            esamFlag = 1;
            break;
        }
    }
    bzero(pcz,sizeof(pcz));
    if (esamFlag == 0)
        TextOut(hdc, nLeft, nTop, "密钥状态  测试");
    else
        TextOut(hdc, nLeft, nTop, "密钥状态  正式");
    nTop += 18;
    TextOut(hdc, nLeft, nTop, "ESAM序列号");

    nTop += 18;
    bzero(pcz,sizeof(pcz));
    sprintf(pcz, "%02X%02X%02X%02X%02X%02X%02X%02X", tESAMInfoBuff.esamSerial[0], tESAMInfoBuff.esamSerial[1], tESAMInfoBuff.esamSerial[2],
    tESAMInfoBuff.esamSerial[3], tESAMInfoBuff.esamSerial[4], tESAMInfoBuff.esamSerial[5], tESAMInfoBuff.esamSerial[6], tESAMInfoBuff.esamSerial[7]);
    TextOut(hdc, nLeft, nTop, pcz);

    nTop += 18;
    bzero(pcz,sizeof(pcz));
    sprintf(pcz, "ESAM版本号 %02X%02X%02X%02X", tESAMInfoBuff.esamVersion[0], tESAMInfoBuff.esamVersion[1], tESAMInfoBuff.esamVersion[2], tESAMInfoBuff.esamVersion[3]);
    TextOut(hdc, nLeft, nTop, pcz);

    nTop += 18;
    TextOut(hdc, nLeft, nTop, "对称密钥版本");

    nTop += 18;
    bzero(pcz,sizeof(pcz));
    sprintf(pcz, "%02X%02X%02X%02X%02X%02X%02X%02X", tESAMInfoBuff.symKeyVersion[0], tESAMInfoBuff.symKeyVersion[1], tESAMInfoBuff.symKeyVersion[2],
    tESAMInfoBuff.symKeyVersion[3], tESAMInfoBuff.symKeyVersion[4], tESAMInfoBuff.symKeyVersion[5], tESAMInfoBuff.symKeyVersion[6], tESAMInfoBuff.symKeyVersion[7]);
    TextOut(hdc, nLeft, nTop, pcz);

    nTop += 18;
    bzero(pcz,sizeof(pcz));
    sprintf(pcz, "%02X%02X%02X%02X%02X%02X%02X%02X", tESAMInfoBuff.symKeyVersion[8], tESAMInfoBuff.symKeyVersion[9], tESAMInfoBuff.symKeyVersion[10],
    tESAMInfoBuff.symKeyVersion[11], tESAMInfoBuff.symKeyVersion[12], tESAMInfoBuff.symKeyVersion[13], tESAMInfoBuff.symKeyVersion[14], tESAMInfoBuff.symKeyVersion[15]);
    TextOut(hdc, nLeft, nTop, pcz);
    return;
}


PLUGIN_INFO g_layer_symKey_ver=
{
    0x000c0001, // 0x000c0001
    symKey_Init,
    NULL,
    symKey_Draw,
    NULL,
    NULL,
    NULL
};

#endif

static unsigned int g_HplcUpdateStatus = 0;
unsigned int g_HplcWaitTimer = 0;
static int g_HplcIndex = 0;

#define HPLC_UPDATE_STATUS_INIT       0
#define HPLC_UPDATE_STATUS_COPY       1
#define HPLC_UPDATE_STATUS_MKDIR_ERR  2
#define HPLC_UPDATE_STATUS_SEND_MSG   3
#define HPLC_UPDATE_STATUS_NO_DIR     4
#define HPLC_UPDATE_STATUS_MALLOC_ERR 5
#define HPLC_UPDATE_STATUS_NO_BIN     6
#define HPLC_UPDATE_STATUS_WAIT_ACK   7
#define HPLC_UPDATE_STATUS_ACK        8

#define HPLC_UPDATE_SRC_FOLDER "/tmp/udisk/hplcupdate"

unsigned int HplcSendMsg(GUI_COMM *pg_guicomm, char *destPath, char *srcePath, int type)
{
    uint32 oadValue = 0xF2090200; 
    uint32 len = 0;
    uint8 buf[500] = {0};
    uint16 offset = 0;
    uint8 lenoffset = 0;
    uint8 lenBuf[3] = {0}; //数组数量编码
    int ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    char topic[64] = {0};
    const char *desktopGui = "M-desktopGui";
    const char *taskManager = "M-taskManager";
    if(type == 0)
        buf[offset++] = 81;
    else
        buf[offset++] = 82;
    memcpy_r(&buf[offset], &oadValue,4);
    offset += sizeof(uint32);

    if(type == 1)
    {
        uint32 subOadValue = 0xF2100200; 
        buf[offset++] = 1;
        memcpy_r(&buf[offset], &subOadValue,4);
        offset += sizeof(uint32);
    }
    lenoffset = set_len_offset(strlen(srcePath), lenBuf);
    memcpy(&buf[offset], lenBuf, lenoffset);
    offset += lenoffset;
    memcpy(&buf[offset], srcePath, strlen(srcePath));
    offset += strlen(srcePath);
    
    memset(lenBuf, 0, sizeof(lenBuf));
    lenoffset = set_len_offset(strlen(destPath), lenBuf);
    memcpy(&buf[offset], lenBuf, lenoffset);
    offset += lenoffset;
    memcpy(&buf[offset], destPath, strlen(destPath));
    offset += strlen(destPath);

    struct stat zbuf;
    int fd = open(srcePath, O_RDONLY);
    fstat(fd, &zbuf);
    memcpy_r(&buf[offset],&zbuf.st_size,4);
    offset+=4;

    buf[offset++] = 3;
    buf[offset++] = 5;

    memset(lenBuf, 0, sizeof(lenBuf));
    lenoffset = set_len_offset(1, lenBuf);
    memcpy(&buf[offset], lenBuf, lenoffset);
    offset += lenoffset;
    memcpy(&buf[offset], "0", 1);
    offset++;

    buf[offset++] = 0;

    memcpy_r(0,&buf[offset],2);
    offset += 2;

    buf[offset++] = 0;
    memset(lenBuf, 0, sizeof(lenBuf));
    lenoffset = set_len_offset(1, lenBuf);
    memcpy(&buf[offset], lenBuf, lenoffset);
    offset += lenoffset;
    memcpy(&buf[offset], "0", 1);
    offset++;

    buf[offset++] = 0;

    buf[offset++] = 0;

    buf[offset++] = 0;

    buf[offset++] = 0;

    if (g_HplcUpdateStatus != HPLC_UPDATE_STATUS_SEND_MSG)
    {
        return 1;
    }

    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    len = sizeof(MESSAGE_INFO_T) +  offset;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(len);
    if (NULL == pMsgInfo)
    {
        g_HplcUpdateStatus = HPLC_UPDATE_STATUS_MALLOC_ERR;
        return 1;
    }

    memset(pMsgInfo, 0, len);

    pMsgInfo->rpm = 1;
    pMsgInfo->index = gui_get_next_index();
    pMsgInfo->priority = 0;
    pMsgInfo->label = 0;
    pMsgInfo->IID = 0x1003;
    pMsgInfo->IOP = 0x0020;
    pMsgInfo->msglen = offset;

    pMsgInfo->sourlen = strlen((char *)desktopGui);
    memcpy(pMsgInfo->souraddr, desktopGui, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(taskManager);
    memcpy(pMsgInfo->destaddr, taskManager, pMsgInfo->destlen);

    memcpy(&pMsgInfo->playload, buf, offset);
    sprintf(topic, "%s/%s", desktopGui, pMsgInfo->destaddr);
    ret = gui_send_msg(topic, pMsgInfo, 0);

    free(pMsgInfo);
    g_HplcUpdateStatus = HPLC_UPDATE_STATUS_WAIT_ACK;
    g_ModuleWaitTimer = 0;

    UpdateWindow(pg_guicomm->hMainWnd, TRUE);

    return ret;
}

unsigned int HplcGetPkgPath()
{
    DIR           *dir;
    if ((dir = opendir(HPLC_UPDATE_SRC_FOLDER)) == NULL)
    {
        g_HplcUpdateStatus = HPLC_UPDATE_STATUS_NO_DIR;
        return 1;
    }
    
    closedir(dir);
    return 0;
}

void InitHplcUpdateDraw(DWORD dwPluginId)
{
    g_HplcIndex = 0;
    g_HplcUpdateStatus = HPLC_UPDATE_STATUS_INIT;
    g_HplcWaitTimer = 0;
}

void HplcUpdateDraw(HDC hdc)
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

    if (g_HplcUpdateStatus == HPLC_UPDATE_STATUS_COPY)
    {
        nTop = 40;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "拷贝文件中...");
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "耗时约10秒钟");      
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "请不要关闭电源");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "不要拔掉U盘");

        return;
    }

    if (g_HplcUpdateStatus == HPLC_UPDATE_STATUS_SEND_MSG)
    {
        nTop = 40;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "正在发送升级消息...");
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "耗时约10秒钟");
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "请不要关闭电源");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "可以拔掉U盘");
        return;
    }

    if (g_HplcUpdateStatus == HPLC_UPDATE_STATUS_WAIT_ACK)
    {
        nTop = 40;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "等待响应消息中...");
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "耗时约10秒钟");      
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "请不要关闭电源");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "可以拔掉U盘");

        return;
    }

    nLeft = 40;
    nTop = 40;
    SelectFont(hdc, pg_guicomm->fontBig);
    if (0 == g_HplcIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "主节点升级");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "主节点升级");
    }

    nTop += 40;
    if (1 == g_HplcIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "子节点升级");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "子节点升级");
    }

    nTop += 40;
    if (2 == g_HplcIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "返回");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "返回");
    }

    PopListDraw(hdc);
}

void HplcUpdateTimer(int nId)
{   
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    /* 1s定时器检查 */
    if (nId != TIMER_MAIN)
    {
        return;
    }
    g_guiMain.nTrunDispTime = 0; /* 在模组升级页面，不进入轮显 */
    g_guiMain.nLightonTime = 0;
    g_guiMain.bLighton = TRUE;
    lighton_lcd(); /* 在模组升级页面，屏幕常亮 */

    switch (g_HplcUpdateStatus)
    {   
        case HPLC_UPDATE_STATUS_INIT: //未开始升级，初始页面
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case HPLC_UPDATE_STATUS_COPY: //升级文件拷贝
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case HPLC_UPDATE_STATUS_MKDIR_ERR: //创建目录失败
        {
            MessageBox("创建目录失败！");
            g_HplcUpdateStatus = HPLC_UPDATE_STATUS_INIT;
            break;
        }
        case HPLC_UPDATE_STATUS_SEND_MSG: //发送升级消息
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case HPLC_UPDATE_STATUS_NO_DIR: //未找到U盘里的安装目录
        {
            MessageBox("未找到U盘里的安装目录\n请检查U盘是否插入");
            g_HplcUpdateStatus = HPLC_UPDATE_STATUS_INIT;
            break;
        }
        case HPLC_UPDATE_STATUS_MALLOC_ERR: //申请内存失败
        {
            MessageBox("申请内存失败！");
            g_HplcUpdateStatus = HPLC_UPDATE_STATUS_INIT;
            break;
        }
        case HPLC_UPDATE_STATUS_NO_BIN: //缺少升级文件
        {
            MessageBox("缺少安装脚本！");
            g_HplcUpdateStatus = HPLC_UPDATE_STATUS_INIT;
            break;
        }
        case HPLC_UPDATE_STATUS_WAIT_ACK: //等待响应消息
        {
            if (g_HplcWaitTimer <= 10)
            {
                g_HplcWaitTimer++;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            }
            else
            {
                MessageBox("等待响应消息超时！\n请在通信模块信息\n查询升级结果");
                g_HplcUpdateStatus = HPLC_UPDATE_STATUS_INIT;
                g_HplcWaitTimer = 0;
            }
            break;
        }
        case HPLC_UPDATE_STATUS_ACK: //收到响应消息
        {
            MessageBox("收到响应消息\n请在通信模块信息\n查询升级结果");
            g_HplcUpdateStatus = HPLC_UPDATE_STATUS_INIT;
            break;
        }
        default: 
        {
            g_HplcUpdateStatus = HPLC_UPDATE_STATUS_INIT;
            break;
        }
    }
}

unsigned int HplcCopyPkg()
{
    if (0 == access("/data/app/ccoRouter/hplc", F_OK))
    {
        system("rm -rf /data/app/ccoRouter/hplc/*");
    }

    if (0 != access("/data/app/ccoRouter/hplc", F_OK))
    {
        if(0 != mkdir("/data/app/ccoRouter/hplc", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
        {
            g_HplcUpdateStatus = HPLC_UPDATE_STATUS_MKDIR_ERR;
            return 1;
        }
    }

    if (0 != access("/tmp/udisk/hplcupdate/hplc_update_file", F_OK))
    {
        g_HplcUpdateStatus = HPLC_UPDATE_STATUS_NO_BIN;
        return 1;
    }

    g_HplcUpdateStatus = HPLC_UPDATE_STATUS_COPY;
    system("cp -r /tmp/udisk/hplcupdate/hplc_update_file /data/app/ccoRouter/hplc");
    return 0;
}

/* 入参由调用者保证合法性 */
unsigned int HplcInstallProc(GUI_COMM *pg_guicomm, int type)
{
    char srcePath[PATH_MAX_LEN + 1] = {0};
    char destPath[PATH_MAX_LEN + 1] = {0};
    const char *message = "警告\n安装过程中请不要断电\n是否安装";

    char buf[200] = {0};
    if (ValidatePwd() == TRUE)
    {
        sprintf(buf, "%s", message);
        if (FALSE == MessageBox(buf, MB_OKCANCEL))
        {
            return 0;
        }
    }
    else
    {
        return 1;
    }

    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    
    if (HplcGetPkgPath() != 0)
    {
        return 1;
    }

    if (HplcCopyPkg() != 0)
    {
        return 1;
    }
    sprintf(destPath, "%s", "/tmp/udisk/hplcupdate/hplc_update_file");
    sprintf(srcePath, "%s", "/data/app/ccoRouter/hplc/hplc_update_file");
    g_HplcUpdateStatus = HPLC_UPDATE_STATUS_SEND_MSG;
    if (HplcSendMsg(pg_guicomm, destPath, srcePath, type) != 0)
    {
        return 1;
    }
    g_HplcUpdateStatus = HPLC_UPDATE_STATUS_ACK;
    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    return 0;
}

BOOL HplcUpdateKey(int nKey)
{
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return FALSE;
    }

    /* 未开始升级状态下才能操作菜单 */
    if (g_HplcUpdateStatus != HPLC_UPDATE_STATUS_INIT)
    {
        return TRUE;
    }

    switch (nKey)
    {
        case KEY_UP:
        case KEY_LEFT:
        {
            if (g_HplcIndex > 0)
                g_HplcIndex--;
            else
                g_HplcIndex = 2;

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        case KEY_RIGHT:
        {
            if (g_HplcIndex == 2)
                g_HplcIndex = 0;
            else
                g_HplcIndex++;

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (g_HplcIndex)
            {
                case 0:
                {
                    HplcInstallProc(pg_guicomm, 0);
                    return TRUE;
                }
                case 1:
                {
                    HplcInstallProc(pg_guicomm, 1);
                    return TRUE;
                }
                case 2: /* 退出 */
                {
                    ModuleDeleteDir("/data/app/ccoRouter/hplc");
                    CancelMenu();
                    return TRUE;
                }
            }

            return TRUE;
        }
    }

    return FALSE;
}

//HPLC升级
PLUGIN_INFO g_HplcUpdate = {
    0x000c0006,             /* id */
    InitHplcUpdateDraw,   /* InitLayer */
    NULL,                   /* UninitLayer */
    HplcUpdateDraw,       /* Draw */
    HplcUpdateTimer,      /* Timer */
    HplcUpdateKey,        /* keymsg */
    NULL                    /* ipc */
};

#ifdef AREA_FUJIAN
unsigned int wirelessDCMSendMsg(uint16 iop)
{
    GUI_FMT_DEBUG("即将搜索运营商网络信号 发送消息请求 iop = 0x%x Start, iop\n");
    unsigned int len = 0;
    int ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    char topic[256] = {0};
    const char *desktopGui = "M-desktopGui";
    const char *wirelessDCM = "M-wirelessDCM";
    len = sizeof(MESSAGE_INFO_T);
    pMsgInfo = (MESSAGE_INFO_T *)malloc(len);

    if (NULL == pMsgInfo)
    {
        GUI_FMT_DEBUG("申请内存空间失败\n");
        printf("[0007]%s malloc failed %d\n", __FUNCTION__, len);
        return 1;
    }

    memset(pMsgInfo, 0, len);
    
    pMsgInfo->rpm = 1;
    pMsgInfo->index = gui_get_next_index();
    pMsgInfo->priority = 0;
    pMsgInfo->label = 0;
    pMsgInfo->IID = MESSAGE_IID_WIRELESS;
    pMsgInfo->IOP = iop;
    
    pMsgInfo->sourlen = strlen((char *)desktopGui);
    memcpy(pMsgInfo->souraddr, desktopGui, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(wirelessDCM);
    memcpy(pMsgInfo->destaddr, wirelessDCM, pMsgInfo->destlen);

    pMsgInfo->msglen = 0;
    snprintf(topic, 256,"%s/%s", desktopGui, pMsgInfo->destaddr);
    ret = gui_send_msg(topic, pMsgInfo, 0);
    GUI_FMT_DEBUG("即将搜索运营商网络信号 发送消息请求 End\n");

    free(pMsgInfo);
    return ret;
}

#if DESC("运营商网络信号查看",1)
extern char g_DispScanData[MAX_RESULT_LEN + 1];
uint32 g_QueryProviderSignalPageIndex = 0;
uint32 g_QueryProviderSignalPageNum = 0;

void QueryProviderSignalInit(DWORD dwPluginId)
{
    wirelessDCMSendMsg(WIRELESS_IOP_GTOPTSCAN_LAST_AT);
    g_EchoPrintPageIndex = 0;
    g_EchoPrintPageNum = 1;
    memset(g_EchoPrintBuf, 0, sizeof(g_EchoPrintBuf));
    EchoPrintTransBuf(g_DispScanData);
}

void QueryProviderSignalDraw(HDC hdc)
{
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    SelectFont(hdc, pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 20;
    rc.bottom = rc.top + 13;
    char buf[100];
    int nLeft = 5;
    int nTop = 35;
    if (g_DispScanData == NULL || strlen(g_DispScanData) == 0)
    {
        sprintf(buf, "没有搜索到运营商网络信号");
        TextOut(hdc, nLeft, nTop, buf);
        return;
    }
    sprintf(buf, "第%u页/共%u页 方向键翻页", g_EchoPrintPageIndex + 1, g_EchoPrintPageNum);
    DrawText(hdc, buf, strlen(buf), &rc, DT_CENTER | DT_VCENTER);

    uint32 i = 0;

    for (i = 0; i < 9; i++)
    {
        nTop = 35 + 12 * i;
        TextOut(hdc, nLeft, nTop, g_EchoPrintBuf[g_EchoPrintPageIndex][i]);
    }
}

BOOL QueryProviderSignalKey(int nKey)
{
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return FALSE;
    }

    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_EchoPrintPageIndex > 0)
            {
                g_EchoPrintPageIndex--;
            }
            else
            {
                g_EchoPrintPageIndex = g_EchoPrintPageNum - 1;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_EchoPrintPageIndex < g_EchoPrintPageNum - 1)
            {
                g_EchoPrintPageIndex++;
            }
            else
            {
                g_EchoPrintPageIndex = 0;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
    }

    return FALSE;
}

//运营商网络信号查看
PLUGIN_INFO g_QueryProviderSignal = {
	0x11220006/*id*/, 
	QueryProviderSignalInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	QueryProviderSignalDraw/*Draw*/,
	NULL/*Timer*/,
	QueryProviderSignalKey/*keymsg*/,
	NULL/*ipc*/
};
#endif

#if DESC("搜索运营商网络信号",1)
void g_SearchProviderSignalInit(DWORD dwPluginId)
{
}

void g_SearchProviderSignalDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
	if(pg_guicomm == NULL)
		return;
	char czbuf[300]={0};
	int nleft = 40;
	int ntop = 30;
	SelectFont(hdc,pg_guicomm->fontBig);
	sprintf(czbuf,"开始搜索");
	ITEM_SELECT;
    TextOut(hdc, nleft, ntop, czbuf);
    ITEM_NOT_SELECT;
    
}

extern void setIsNeedScan(bool condition);
BOOL g_SearchProviderSignalKeyMsg(int nKey)
{
	GUI_COMM* pg_guicomm = &g_guiComm;
	if(pg_guicomm == NULL)
		return FALSE;
	char buf[300]={0};

	switch(nKey)
	{
        case KEY_OK:
        {
            if(ValidatePwd() == TRUE)
            {
                sprintf(buf,"即将搜索运营商网络信号\n是否确认？");
                if(TRUE == MessageBox(buf, MB_OKCANCEL))
                {
                    //ming 提供set方法
                    GUI_FMT_DEBUG("即将搜索运营商网络信号 \n");
                    wirelessDCMSendMsg(WIRELESS_IOP_GTOPTSCAN_AT);
                    return FALSE;
                }
            }
        }
	}
	return FALSE;
}

//搜索运营商网络信号
PLUGIN_INFO g_SearchProviderSignal = {
	0x16110000/*id*/, 
	g_SearchProviderSignalInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	g_SearchProviderSignalDraw/*Draw*/,
	NULL/*Timer*/,
	g_SearchProviderSignalKeyMsg/*keymsg*/,
	NULL/*ipc*/
};
#endif
#endif

/************************************************************************/
/* 重庆抄表失败清单*/
/************************************************************************/
OOP_TASK_FAIL_NUM_LIST_T failNumList;
uint16 failIndex;
void MeterReadFailStatInit(DWORD dwPluginId)
{
    failIndex = 0;
    memset(&failNumList, 0, sizeof(failNumList));
    NOMAL_OAD_T normal = {0};
    normal.oad.value = 0xFF300B00;
    uint32 len = 0;
    normal.classtag = 5;
    normal.infoNum = 0;
    db_read_nomal(g_hUdbCenter, &normal, sizeof(OOP_TASK_FAIL_NUM_LIST_T), (uint8*)&failNumList, &len);
}

void MeterReadFailStatDraw(HDC hdc)
{
    int nTop = 20;
    char czBuf[100];
    SelectFont(hdc,g_guiComm.fontSmall);
    bzero(czBuf,sizeof(czBuf));
#ifdef AREA_CHONGQING
    snprintf(czBuf, sizeof(czBuf), "%u-%u %s", 3,4,
        g_guiMain.pPrentMenu->pPlugStruct->czCaption);
#else
     nTop = 10;
#endif
    TextOut(hdc, 10, nTop, czBuf);
    nTop+=20;
    TextOut(hdc, 8, nTop, "测量点号  抄表地址");

    for(uint16 i = 0;i+failIndex*6 < failNumList.FailNum && i < 6; i++){
        nTop+=14;
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf,"%03d", failNumList.Pn[i+failIndex*6]);
        TextOut(hdc, 15, nTop, czBuf);
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf,"%02X%02X%02X%02X%02X%02X", failNumList.FailAddr[i+failIndex*6][5],failNumList.FailAddr[i+failIndex*6][4],
        failNumList.FailAddr[i+failIndex*6][3], failNumList.FailAddr[i+failIndex*6][2], failNumList.FailAddr[i+failIndex*6][1],
        failNumList.FailAddr[i+failIndex*6][0]);
        TextOut(hdc, 55, nTop, czBuf);
    }
}

BOOL  MeterReadFailStatMsg(int nKey)
{
    PLUGIN_INFO* pPlugin;
    uint16 Maxindex_num;
    if (failNumList.FailNum%6 == 0 && failNumList.FailNum/6 >= 1)
        Maxindex_num = failNumList.FailNum/6 - 1;
    else
        Maxindex_num = failNumList.FailNum/6;
    switch(nKey)
    {
        case KEY_UP:
        case KEY_LEFT: 
            if(failIndex == 0)
               failIndex = Maxindex_num;
            else
               failIndex--;
           UpdateWindow(g_guiComm.hMainWnd, TRUE);
           return TRUE;
        case KEY_DOWN:
        case KEY_RIGHT: 
            if(failIndex == Maxindex_num)
               failIndex = 0;
            else
               failIndex++;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;

        case KEY_OK:
              pPlugin = GetPlugIn(0x0042000c);
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
    return FALSE;
}
    
PLUGIN_INFO g_MeterReadFailStat= {
    0x0041000c/*id*/, 
    MeterReadFailStatInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    MeterReadFailStatDraw/*Draw*/,
    NULL,
    MeterReadFailStatMsg/*keymsg*/,
    NULL/*ipc*/
};