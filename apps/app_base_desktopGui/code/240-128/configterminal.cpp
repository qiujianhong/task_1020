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
#include "task_msg.h"
#include "msg_base.h"
#include "OopAll.h"
#include "desk_meter.h"


extern void CancelMenu();
int64 unlawPower = 0xeeeeeeeeeeeeeeee;
uint8 safe_status = 0;		//安全模式： 0——关闭，1——开启，默认关闭
uint8 mac_status = 1;		//MAC校验： 0——关闭，1——开启，默认开启
uint8 hplc_mac_status = 0;      //载波MAC校验开关 默认关闭
uint8 g_Relay485Flag = 0;   //是否支持转发485串口报文: 1-enable 0-disable
uint8 g_MeterreadFlag = 0;  //是否支持抄表补抄 0-关闭，1-开启
extern OOP_GEOGADD_A_T GpsPosition;
extern uint8 getmodemversionflag;
extern char modemversion[4];
int nCurPoint;
DESK_METER_LIST_T meterList;
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
    read_pdata_xram("/mnt/data/app/puAmr/cfg/gprs_default", (char *)ptCprs, 0, sizeof(OOP_GPRSCONFIG_T));
}

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
    nLeft = 46;

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

    nLeft = 156;
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
    sprintf(temp, "%d.%0d℃", value / 1000, value % 1000);
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

uint8 FTP_Telnet_status = 0;		//ftp telnet使能： 0——关闭，1——开启，默认关闭
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
    //uint8     flag3 = 0;
    int32     state = 0;
    int       datav1 = 0;
    int       datav2 = 0;
    //int       datav3 = 0;
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
        //if(0 == adcdev->adc_volt_read(adcdev, EMADC_CAPVOL_CHANNEL, &datav3))
        //{
            //flag3 = 1;
        //}
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

    /*nTop += 20;
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
    TextOut(hdc, nLeft2, nTop, czbuf);*/
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
    int nLeft = 62;
    
    TextOut(hdc, nLeft, nTop, "模块版本：");

    nLeft = 120;
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
    nLeft = 62;
    
    TextOut(hdc, nLeft, nTop, "内部版本：");

    nLeft = 120;
    if (strlen(ac_minver) != 0)
    {
        TextOut(hdc, nLeft, nTop, ac_minver);
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "-----");
    }

    nTop = 100;
    nLeft = 62;
    
    TextOut(hdc, nLeft, nTop, "版本日期：");

    nLeft = 120;
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
    "串口",
    "自适应"
};

void wirelessStatus_set_init(HDC hdc)
{
    if(0 == access("/data/app/wirelessDCM/data/bForceUart", F_OK))
    {
        g_Wireless_status = 1;
    }
    else if(0 == access("/data/app/wirelessDCM/data/bForceUsb", F_OK))
    {
        g_Wireless_status = 0;
    }
    else
    {
        g_Wireless_status = 2;
    }
    g_WirelessIndex = 0;
    g_Wireless_statusOld = g_Wireless_status;
}

void wirelessStatus_set_draw(HDC hdc)
{
    int nTop = 24;
    int nLeft1 = 85;
    int nLeft2 = 115;
    int nLeft3 = 130;

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    TextOut(hdc, 70, nTop, "无线拨号方式");
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "功能设置");

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
        TextOut(hdc, 46, nTop, "返回");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 46, nTop, "返回");
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
    char cmd[128]={0};
    
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
                    PopList(g_WirelessStatusName, 3, g_WirelessStatusName[g_Wireless_status], 120, 80, FALSE);
                    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                    return TRUE;
                }
                else if(2 == g_WirelessIndex)
                {
                    if(g_Wireless_status != g_Wireless_statusOld)
                    {
                        if(0 == g_Wireless_status)//使用USB
                        {
                            sprintf(cmd,"rm /data/app/wirelessDCM/data/bForceUart");
                            popen(cmd,"r");
                            sprintf(cmd,"touch /data/app/wirelessDCM/data/bForceUsb");
                            popen(cmd,"r");
                        }
                        else if(1 == g_Wireless_status)//使用串口
                        {
                            sprintf(cmd,"rm /data/app/wirelessDCM/data/bForceUsb");
                            popen(cmd,"r");
                            sprintf(cmd,"touch /data/app/wirelessDCM/data/bForceUart");
                            popen(cmd,"r");
                        }
                        else
                        {
                            sprintf(cmd,"rm /data/app/wirelessDCM/data/bForceUsb");
                            popen(cmd,"r");
                            sprintf(cmd,"rm /data/app/wirelessDCM/data/bForceUart");
                            popen(cmd,"r");
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

#if DESC("U盘升级，拷日志",1)

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

    if (0 != access("/udisk/package", F_OK))
    {
        g_UpdateStatus = UPDATE_STATUS_NO_PACKAGE;
        return 1;
    }

    system("cp -r /udisk/package/* /tmp/update/");
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
    int nLeft = 84;
	int nLeft1 = 90;
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
        TextOut(hdc, nLeft, nTop, "拷贝文件中...");
        nTop = 60;
        TextOut(hdc, nLeft1, nTop, "耗时约30s");
        nTop = 80;
        TextOut(hdc, nLeft, nTop, "请不要关闭电源");
        nTop = 100;
        TextOut(hdc, nLeft1, nTop, "不要拔掉U盘");

        return;
    }

//要考虑240*128屏幕下显示偏左的问题
    if (g_UpdateStatus == UPDATE_STATUS_INSTALL)
    {
        nTop = 40;
        TextOut(hdc, nLeft1, nTop, "正在安装...");
        nTop = 60;
        TextOut(hdc, nLeft1, nTop, "耗时约1分钟");
        nTop = 80;
        TextOut(hdc, nLeft, nTop, "请不要关闭电源");
        nTop = 100;
        TextOut(hdc, nLeft1, nTop, "可以拔掉U盘");

        return;
    }

    nTop = 36;
    SelectFont(hdc, pg_guicomm->fontBig);

    if (0 == g_UpdateIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "安装程序");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "安装程序");
    }

    nTop += 30;
    if (1 == g_UpdateIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "执行脚本");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "执行脚本");
    }

    nTop += 30;
    if (2 == g_UpdateIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "返回");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "返回");
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
//    char *productName[] = {(char *)"无效值", (char *)"集中器", (char *)"专变" , (char *)""};
//    char buf[200] = {0};

    if (ValidatePwd() == TRUE)
    {
//        sprintf(buf, "%s%s", message, productName[installType]);
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
	int nleft = 100;
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
//	ntop+=28;
//	sprintf(czbuf,"备份数据库文件");
//	if(index == logexportindex)
//	{
//        ITEM_SELECT;
//        TextOut(hdc, nleft, ntop, czbuf);
//        ITEM_NOT_SELECT;
//	}else
//	{
//        TextOut(hdc, nleft, ntop, czbuf);
//	}

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

    time_t t;
    
    /* 1s定时器检查 */
    if (nId != TIMER_MAIN)
    {
        return;
    }

    g_guiMain.nTrunDispTime = 0; /* 在U盘升级页面，不进入轮显 */
    g_guiMain.nLightonTime = 0;
    g_guiMain.bLighton = TRUE;
//    lighton_lcd(); /* 在U盘升级页面，屏幕常亮 */

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

void *CopyThreads(void *arg)
{
    char cmd[512]		= {0};
	char logPath[512]	= {0};
	char AppPath[512]	= {0};

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
	        sprintf(AppPath,"/udisk/Apps_%s_%02d.%02d.%02d", pstatus, ptime.tm_hour, ptime.tm_min, ptime.tm_sec);
	        
	        sprintf(cmd, "mkdir -p %s", AppPath);
	        system(cmd);
	        sprintf(cmd, "cp -rf /usr/local/extapps/* %s/", AppPath);
	        system(cmd);
	        break;
	    }
	    case 2:         //备份所有文件
	    {
	        sprintf(logPath,"/udisk/log_%s_%02d.%02d.%02d", pstatus, ptime.tm_hour, ptime.tm_min, ptime.tm_sec);
			
	        sprintf(cmd, "mkdir -p %s", logPath);
	        system(cmd);
	        
	        sprintf(cmd, "tar -zcvf %s/log.tar.gz /data/app/", logPath);
            system(cmd);

	        sprintf(cmd, "tar -zcvf %s/messages.tar.gz /var/log/messages*", logPath);
            system(cmd);

			sprintf(cmd, "tar -zcvf %s/tmplog.tar.gz /tmp/data/", logPath);
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
                PopMsg("                 正在备份\n\n               复制时间较长\n\n         请勿操作终端或者拔出U盘");
                return FALSE;
            }
        }
    }
	}
	return FALSE;
}


//日志导出
PLUGIN_INFO g_smiOSLogExport = {
	0x20680002/*id*/, 
	LogExportInit/*InitLayer*/,
	LogExportUnInit/*UninitLayer*/,
	LogExportDraw/*Draw*/,
	LogExportTimer/*Timer*/,
	LogExportKeyMsg/*keymsg*/,
	NULL/*ipc*/
};

//U盘升级
PLUGIN_INFO g_smiOSUUpdate = {
	0x20B60000/*id*/, 
	InitUdiskUpdateDraw/*InitLayer*/,
	NULL/*UninitLayer*/,
	UdiskUpdateDraw/*Draw*/,
	UdiskUpdateTimer/*Timer*/,
	UdiskUpdateKey/*keymsg*/,
	NULL/*ipc*/
};

#endif

#if DESC("日志清除",1)
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

    int nTop = 98;
    int nLeft = 44;
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

    nLeft = 164;
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
    0x20B50000,     /* id */
    InitLogClear,   /* InitLayer */
    NULL,           /* UninitLayer */
    LogClearDraw,   /* Draw */
    NULL,           /* Timer */
    LogClearKey,    /* keymsg */
    NULL            /* ipc */
};

#endif

#if DESC("安全模式",1)

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
        #ifdef AREA_JIANGSU
        g_MeterreadFlag= iniparser_getint(ini,"SAFE_MODE:RS485PATCH",1);
        #else
        g_MeterreadFlag= iniparser_getint(ini,"SAFE_MODE:RS485PATCH",0);
        #endif
        iniparser_freedict(ini);
    }

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
    printf("ret=%d, len=%u!\n", ret, len);
	safe_index = 0;
}

void setSafeONOFFDraw1(HDC hdc)
{
    char      czbuf[32];
	SelectFont(hdc,g_guiComm.fontSmall);
	int nTop = 20;
    int nLeft = 50;
    int nLeft2 = 140;

    //标题显示
    ITEM_NOT_SELECT;
    sprintf(czbuf, "安全模式:");//用采加密开关，配电加密开关
    TextOut(hdc, nLeft, nTop, czbuf);
    
    if(safe_index == 0)
     {
         ITEM_SELECT;
         TextOut(hdc, nLeft2, nTop, g_safeModel_list[safe_status]);
         ITEM_NOT_SELECT;
     }
     else
     {
         TextOut(hdc, nLeft2, nTop, g_safeModel_list[safe_status]);
     }

    nTop += 18;
    sprintf(czbuf, "MAC校验:");
    TextOut(hdc, nLeft, nTop, czbuf);

    if(safe_index == 1)
    {
     	ITEM_SELECT;
     	TextOut(hdc, nLeft2, nTop, g_macModel_list[mac_status]);
     	ITEM_NOT_SELECT;
    }
    else
    {
     	TextOut(hdc, nLeft2, nTop, g_macModel_list[mac_status]);
    }

    nTop += 18;
    sprintf(czbuf, "载波MAC校验:");
    TextOut(hdc, nLeft, nTop, czbuf);

    if(safe_index == 2)
    {
    	 ITEM_SELECT;
    	 TextOut(hdc, nLeft2, nTop, g_macModel_list[hplc_mac_status]);
    	 ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_macModel_list[hplc_mac_status]);
    }

    nTop += 18;
    sprintf(czbuf, "交采485通道:");
    TextOut(hdc, nLeft, nTop, czbuf);

    if(safe_index == 3)
    {
    	 ITEM_SELECT;
    	 TextOut(hdc, nLeft2, nTop, g_ac_485_list[g_Relay485Flag]);
    	 ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_ac_485_list[g_Relay485Flag]);
    }

    nTop += 18;
    sprintf(czbuf, "路由学习:");
    TextOut(hdc, nLeft, nTop, czbuf);

    if(safe_index == 4)
    {
     	ITEM_SELECT;
     	TextOut(hdc, nLeft2, nTop, g_routerstudy_list[router_state]);
     	ITEM_NOT_SELECT;
    }
    else
    {
     	TextOut(hdc, nLeft2, nTop, g_routerstudy_list[router_state]);
    }

    if(safe_index == 5)
    {
        ITEM_SELECT;
        TextOut(hdc, 20, 108, "下一页");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 20, 108, "下一页");
    }
    
    if(safe_index == 6)
    {
        ITEM_SELECT;
        TextOut(hdc, 180, 108, "确定");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 180, 108, "确定");
    }

     PopListDraw(hdc);
}

void setSafeONOFFDraw2(HDC hdc)
{
    char      czbuf[32];
    SelectFont(hdc,g_guiComm.fontSmall);
    int nTop = 20;
    int nLeft = 50;
    int nLeft2 = 140;

    sprintf(czbuf, "抄表补抄:");
    TextOut(hdc, nLeft, nTop, czbuf);
    if(safe_index == 7)
    {
    	ITEM_SELECT;
    	TextOut(hdc, nLeft2, nTop, g_meterread_list[g_MeterreadFlag]);
    	ITEM_NOT_SELECT;
    }
    else
    {
    	TextOut(hdc, nLeft2, nTop, g_meterread_list[g_MeterreadFlag]);
    }

    if(safe_index == 8)
    {
        ITEM_SELECT;
        TextOut(hdc, 20, 108, "上一页");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 20, 108, "上一页");
    }

    if(safe_index == 9)
    {
        ITEM_SELECT;
        TextOut(hdc, 180, 108, "确定");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 180, 108, "确定");
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
         {
             safe_index = 9;
         }

         UpdateWindow(g_guiComm.hMainWnd, TRUE);
         return TRUE;

     case KEY_DOWN:
         safe_index ++;
         if(safe_index > 9)
         {
             safe_index = 0;
         }

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
					PopList(g_safeModel_list, 2, g_safeModel_list[safe_status], 140, 32, FALSE);
					break;
             	case 1:
					PopList(g_macModel_list, 2, g_macModel_list[mac_status], 140, 50, FALSE);
					break;
				case 2:
					PopList(g_macModel_list, 2, g_macModel_list[hplc_mac_status], 140, 68, FALSE);
					break;
                case 3:
					PopList(g_ac_485_list, 2, g_ac_485_list[g_Relay485Flag], 140, 85, FALSE);
					break;
                case 4:
					PopList(g_routerstudy_list, 2, g_routerstudy_list[router_state], 140, 100, FALSE);
					break;
                case 7:
					PopList(g_meterread_list, 2, g_meterread_list[g_MeterreadFlag], 140, 32, FALSE);
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
                        PopMsg("              设置成功!\n            正在重启应用...");
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
    int nLeft = 72;

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
    
    int nTop = 80;
    int nLeft = 52;

    memset(pcz, 0, 40);
    if (get_sys_ver(pcz) == 0)
    {
        TextOut(hdc, nLeft, nTop, pcz);
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "-----");
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

#if DESC("密钥版本显示",1)

TESAMInfo_t tESAMInfoBuff;

void symKey_Init(HDC hdc)
{
    memset(&tESAMInfoBuff, 0, sizeof(TESAMInfo_t));
    read_pdata_xram("/data/app/stAmr/cfg/tESAMInfo", (char*)&tESAMInfoBuff, 0, sizeof(TESAMInfo_t));

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
    int nLeft = 10;
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
    for (int i = 0;i < 16; i++){
        if(i == 0){
            sprintf(pcz, "%02X", tESAMInfoBuff.symKeyVersion[i]);
        }else {
            sprintf(pcz, "%s%02X", pcz, tESAMInfoBuff.symKeyVersion[i]);
        }
    }
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

#if DESC("磁盘状态显示",1)

char  device[50] = {0};
uint8 mmc_result[1024] = {0};
int   dis_index = 0;  //
int   data_flag = 0;  // 0 表示读到数据， 其他表示读取失败

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
    int  has_mmc_device = 0;  //0表示未找到 
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

        //找到 
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

    if(0 == has_mmc_device)
    {
        GUI_FMT_DEBUG("设备未找到\n");
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

    //char mmc_cmd1[512] = "mmc read health /dev/";
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
        GUI_FMT_DEBUG("./mmc 命令执行失败, ret = %d\n", ret);
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
    data_flag = get_mmc_information( mmc_result);
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
    int lefts_pace = 20;
    
    int nLeft = lefts_pace;
    int space2 = 12; // 间距 字段与字段之间的间距
    int space  = 190;   //data 起始位置
    
    TextOut(hdc, nLeft, nTop, "Bad Block Manufacturer:");   
    nLeft = space;

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
    nLeft = lefts_pace;
    TextOut(hdc, nLeft, nTop, "Bad Block Runtime System:");
    nLeft = space;
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
    nLeft = lefts_pace;
    TextOut(hdc, nLeft, nTop, "Bad Block Runtime MLC:");    
    nLeft = space;
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
    nLeft = lefts_pace;
    TextOut(hdc, nLeft, nTop, "Max Erase Count EUDA:");
    nLeft = space;
    if(data_flag != 0 || calData(&mmc_result[128], 4, &data) != 0)
    {
         TextOut(hdc, nLeft, nTop, "---");
    }
    else
    {
         sprintf(czBuf, "%d", data);
         TextOut(hdc, nLeft, nTop, czBuf);
    }

    nTop += space2;
    nLeft = lefts_pace;
    TextOut(hdc, nLeft, nTop, "Min Erase Count EUDA:");
    nLeft = space;
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
    nLeft = lefts_pace;
    TextOut(hdc, nLeft, nTop, "Avg Erase Count EUDA:");
    nLeft = space;
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
    nLeft = lefts_pace;
    TextOut(hdc, nLeft, nTop, "Health Device Level EUDA:");
    
    nLeft = space;
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
        disk_status_draw0(hdc);
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
    0x00B000F1, // 0x00A000F9
    disk_status_init,
    NULL,
    disk_status_draw,
    NULL,
    disk_status_Msg,
    NULL
};
#endif


#if DESC("交采电源设置",1)

//unix udp 绑定的文件名称(全路径)，目标地址
#define ACMETER_MSG_PATH       "/tmp/dev"
#define ACMETER_APP_DEST       "/tmp/dev/acmeter"
#define ACMETER_FILEPATH_LEN   256

static char acconf[15]; // acconf: AC Conf(交采电源配置)
static char tmp_acconf[15];
static int g_fUdpAcMeter = 0;
static int set_switch = 0; 
static uint8 set_index;
static uint8 tmp_sel;
static int flg = 0;


#if 1
pthread_mutex_t g_fUdpAcMeterLock;
#define ACMETER_BUFF_LEN       4096        //交互缓冲区长度
uint8   g_AcPiid = 0;     //上报报文帧序列号
#define AC_ADD_VALUE_CLEAR(count, mcount, value) ((count) + (value) >= (mcount) ? 0 : (count) + (value))
#define OUT_DATA_MAX           2048

/*
*********************************************************************
* @name      : puAmr_acMeter_fd_init
* @brief     ：创建与交采APP交互的句柄
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int puAmr_acMeter_fd_init(void)
{
    if (access(ACMETER_APP_DEST, F_OK) != 0)
    {
        GUI_FMT_DEBUG("%s is nonexistent\n", ACMETER_APP_DEST);
        return ERR_NODEV;
    }

    if (g_fUdpAcMeter <= 0)
    {
        char filename[ACMETER_FILEPATH_LEN] = {0};
        sprintf(filename, "%s/%s_acMeter", ACMETER_MSG_PATH, GUI_APP_NAME);
        g_fUdpAcMeter = task_pfmsg_init(filename);
    }
    if (g_fUdpAcMeter <= 0)
    {
        GUI_FMT_DEBUG("%d is invalid\n", g_fUdpAcMeter);
        return ERR_INVALID;
    }

    return ERR_OK;
}


/*
*********************************************************************
* @name      : puAmr_acMeter_revsnd_data
* @brief     ：与交采APP交互的同步接口
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int puAmr_acMeter_revsnd_unit(uint8 *sndData, uint16 sndLen, uint8 *pOutBuf, uint16 *rcvLen)
{
    int32 i = 0;
    int32 iRet = 0;
    uint8 czBuf[ACMETER_BUFF_LEN] = {0};
    uint8 sndBuf[ACMETER_BUFF_LEN] = {0};
    char  sender[ACMETER_FILEPATH_LEN] = {0};

    if (sndLen > sizeof(sndBuf))
    {
        return ERR_OPTION;
    }

    if (g_fUdpAcMeter <= 0)
    {
        if (0 != puAmr_acMeter_fd_init())
        {
            return ERR_INVALID;
        } 
    }

    /* 尝试加锁 */
    while (0 != pthread_mutex_trylock(&g_fUdpAcMeterLock))
    {
        if(i++ > 500)
        {
            GUI_FMT_DEBUG("puAmr_acMeter_revsnd_data lock faild g_fUdpAcMeterLock[]\n", g_fUdpAcMeterLock);
            return ERR_NORESOURCE;
        }
        usleep(10000);
    }

    /* 接收 5s超时 */
    task_msg_settime(g_fUdpAcMeter, 5, 0);
    iRet = task_pfmsg_recv(&g_fUdpAcMeter, czBuf, ACMETER_BUFF_LEN, sender, ACMETER_FILEPATH_LEN);
    pthread_mutex_unlock(&g_fUdpAcMeterLock);

    if (iRet <= 0)
    {
        GUI_FMT_DEBUG("MSGAC recv faild, ret %d\n", iRet);
        return ERR_NORESOURCE;
    }

    GUI_BUF_DEBUG(czBuf, iRet, "MSGAC RECV:");

    *rcvLen = iRet;
    memcpy(pOutBuf, czBuf, *rcvLen);

    return ERR_OK;
}

/*
*********************************************************************
* @name      : puAmr_acMeter_revsnd
* @brief     ：与交采APP交互的同步接口
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int puAmr_acMeter_revsnd(uint8 *sndData, uint16 sndLen, uint8 *pOutBuf, uint16 *rcvLen)
{
    uint32 i = 0;
    int8   ret = ERR_NORMAL;

    while (i < 1)
    {
        ret = puAmr_acMeter_revsnd_unit(sndData, sndLen, pOutBuf, rcvLen);
        if (ERR_OK == ret)
        {
            break;
        }
        else
        {
            i++;
            usleep(10000);
        }
    }

    return ret;
}

/*
*********************************************************************
* @name      : puAmr_acMeter_get
* @brief     ：读交采的通用接口
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int puAmr_acMeter_get(OOP_OAD_U oad, uint8 *prcvbuf, uint16 *prcvLen)
{
    int            ret = ERR_OK;
    //uint8          rcvbuf[OUT_DATA_MAX] = {0};
    //uint16         rcvLen = 0;
    uint8          sndbuf_tmp[25] = {0x68,0x17,0x00,0x43,0x05,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0xA7,0xE6,0x05,0x01,0x00,0x41,0x04,0x02,0x00,0x00,0xFE,0x66,0x16};

    ret = puAmr_acMeter_revsnd(sndbuf_tmp, 25, prcvbuf, prcvLen);
    GUI_BUF_DEBUG(prcvbuf, *prcvLen, "rcvbuf:");
    
    if (ret != ERR_OK)
    {
        GUI_FMT_DEBUG("puAmr_acMeter_revsnd failed. ret(%d), oad(0x%08x)\n", ret, oad.value);
    }

    //memcpy(pOutBuf, rcvbuf, rcvLen);
    
    return ret;

}

/**********************************************************************
* @brief     ：获取交采额定电压类型
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
void get_vol_type(void)
{
    uint8          rcvbuf[OUT_DATA_MAX] = {0};
    uint16         rcvLen = 0;

    if (flg == 1)
        return;
    
    OOP_OAD_U oad = {0};
    int ret = 0;

    oad.value = 0x41040200;
    ret = puAmr_acMeter_get(oad, rcvbuf, &rcvLen);
    if(ret != 0)
    {
        GUI_FMT_DEBUG("puAmr_acMeter_get 额定电压失败 err ret[%d]\n", ret);
        if(acconf[0] == '\0')
        {
            GUI_FMT_DEBUG("设置-----------");
            strcpy(acconf, "-----------");
        }
            
        return;
    }

    GUI_BUF_DEBUG(rcvbuf, rcvLen, "MSGAC RECV:");

    if(rcvbuf[24] == 0x32)
    {
        GUI_FMT_DEBUG("设置三相四线220V");
        strcpy(acconf, "三相四线220V");
        flg = 1;
    }
    else if(rcvbuf[24] == 0x31)
    {
        GUI_FMT_DEBUG("设置三相三线100V");
        strcpy(acconf, "三相三线100V");
        flg = 1;
    }
    else if(rcvbuf[24] == 0x20)
    {
        GUI_FMT_DEBUG("设置三相四线57.7V");
        strcpy(acconf, "三相四线57.7V");
        flg = 1;
    }
    else
    {
        if(acconf[0] == '\0')
        {
            GUI_FMT_DEBUG("设置-----------");
            strcpy(acconf, "-----------");
        }  
    }

    return;
}
#endif

void ProductVoltageCfgInit(DWORD dwPluginId)
{
    set_index = 0;
    set_switch = 0;
    int ret = 0;
    //get_vol_type();
    if (g_fUdpAcMeter <= 0)
    {
        char filename[ACMETER_FILEPATH_LEN] = {0};
        sprintf(filename, "%s/%s_acMeter", ACMETER_MSG_PATH, GUI_APP_NAME);
        g_fUdpAcMeter = task_pfmsg_init(filename);
    }
    
    if (flg == 0)
    {
        uint8 sndBuf[25] = {0x68,0x17,0x00,0x43,0x55,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0x00,0x23,0x14,0x05,0x01,0x00,0x41,0x04,0x02,0x00,0x00,0xfe,0x66,0x16};
        ret = task_pfmsg_send(g_fUdpAcMeter, sndBuf, 25, (char *)ACMETER_APP_DEST);
        GUI_FMT_DEBUG("发送查询额定电压消息，返回[%d]\n", ret);
    }
}

void ProductVoltageCfgDraw(HDC hdc)
{
        //char czbuf[10];
        //char CheckSumFlag=0;
        int nLeft1, nLeft2, y;
        nLeft1 = 10;
        nLeft2 = 90;
        //nAdd = 13;
        y = 20;
    
        get_vol_type();
        SelectFont(hdc,g_guiComm.fontSmall);
    
        ITEM_NOT_SELECT;
        TextOut(hdc, nLeft1, y, "交采电源");
        
        if (set_index == 0)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2-15, y, (set_switch == 0)?acconf:tmp_acconf);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2-15, y, (set_switch == 0)?acconf:tmp_acconf);
        }

        if (set_index == 1)
        {
            ITEM_SELECT
            TextOut(hdc, nLeft2-4, y + 60, "保存");
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2-4, y + 60, "保存");
        }

        PopListDraw(hdc);
        
}

BOOL ProductVoltageCfgKeyMsg(int nKey)
{
    if(g_guiComm.bPopList == TRUE)
	{
	    int nSel=0; 
        PopListKeyMsg(nKey, &nSel);

        if(nKey == KEY_OK)
        {
            if (nSel == 0)
            {
                tmp_sel = 0;
                set_switch = 1;
                strcpy(tmp_acconf, "三相三线100V");
            }
            else if (nSel == 1)
            {
                tmp_sel = 1;
                set_switch = 1;
                strcpy(tmp_acconf, "三相四线220V");
            }
            else
            {
                tmp_sel = 2;
                set_switch = 1;
                strcpy(tmp_acconf, "三相四线57.7V");
            }
        }
    }
    else
    {    
        switch(nKey)
        {
            case KEY_UP:
    	    {
    	        if (set_index > 0)
    	            set_index--;
    	        else
    	            set_index = 1;
    	        UpdateWindow(g_guiComm.hMainWnd, TRUE);
    	        return TRUE;
    	    }
    	    case KEY_DOWN:
    	    {
    	        if (set_index < 1)
    	            set_index++;
    	        else
    	            set_index = 0;
    	        UpdateWindow(g_guiComm.hMainWnd, TRUE);
    	        return TRUE;
    	    }
            case KEY_CANCEL:
            {
                if (strcmp(acconf, "三相三线100V") == 0)
                {
                    tmp_sel = 0;
                }
                else if (strcmp(acconf, "三相四线220V") == 0)
                {
                    tmp_sel = 1;
                }
                else
                {
                    tmp_sel = 2;
                }
                return FALSE;
            }
            case KEY_OK:
            {
                if (ValidatePwd() == TRUE)
	            {
		            if (set_index == 0)
		            {  
                        const char * ListVoltage[] = {"三相三线100V","三相四线220V","三相四线57.7V"};
                        PopList(ListVoltage, 3, acconf, 75, 20, FALSE);
                        return TRUE;
                    }
                    else
                    {
                        int ret = 0;
                        if (g_fUdpAcMeter <= 0)
                        {
                            char filename[ACMETER_FILEPATH_LEN] = {0};
                            sprintf(filename, "%s/%s_acMeter", ACMETER_MSG_PATH, GUI_APP_NAME);
                            g_fUdpAcMeter = task_pfmsg_init(filename);
                        }
                        
                        if (tmp_sel == 0)
                        {
                            strcpy(acconf, "三相三线100V");
                            uint8 sndBuf[17] = {0x68,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0x68,0x05,0x05,0x0A,0xFF,0x43,0xFF,0xFF,0x20,0x16};
                            ret = task_pfmsg_send(g_fUdpAcMeter, sndBuf, 17, (char *)ACMETER_APP_DEST);
                            GUI_FMT_DEBUG("发送三相三线100V，返回[%d]\n", ret);
                        }
                        else if (tmp_sel == 1)
                        {
                            strcpy(acconf, "三相四线220V");
                            uint8 sndBuf[17] = {0x68,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0x68,0x05,0x05,0x0A,0xFF,0x55,0xFF,0xFF,0x32,0x16};
                            ret = task_pfmsg_send(g_fUdpAcMeter, sndBuf, 17, (char *)ACMETER_APP_DEST);
                            GUI_FMT_DEBUG("发送三相四线220V，返回[%d]\n", ret);
                        }
                        else
                        {
                            strcpy(acconf, "三相四线57.7V");
                            uint8 sndBuf[17] = {0x68,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0x68,0x05,0x05,0x0A,0xFF,0x44,0xFF,0xFF,0x21,0x16};
                            ret = task_pfmsg_send(g_fUdpAcMeter, sndBuf, 17, (char *)ACMETER_APP_DEST);
                            GUI_FMT_DEBUG("发送三相四线57.7V，返回[%d]\n", ret);
                        }

                        if (ret == 0)
                        {
                            MessageBox("保存成功！");
                            return TRUE;
                        }
                        else
    		            {
    		                MessageBox("保存失败！");
    		                return FALSE;
    		            }
                    }
                }
            }
                break;
             default:
        	    return (FALSE);
        		break;
    	}
    }
    return (TRUE);
}

//交采电源设置
PLUGIN_INFO g_layerProductVoltageCfg = {
    0x20B70001/*id*/,
    ProductVoltageCfgInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    ProductVoltageCfgDraw/*Draw*/,
    NULL/*Timer*/,
    ProductVoltageCfgKeyMsg/*keymsg*/,
    NULL/*ipc*/
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
        nLeft = 61;
        sprintf(buf, "正在安装...(%u秒)", g_CoWaitTimer);
        TextOut(hdc, nLeft, nTop, buf);
        nTop = 60;
        nLeft = 77;
        TextOut(hdc, nLeft, nTop, "耗时约1分钟");      
        nTop = 80;
        nLeft = 69;
        TextOut(hdc, nLeft, nTop, "请不要关闭电源");
        nTop = 100;
        nLeft = 77;
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
    nLeft = 60;

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

    nLeft = 152;
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
            
            if (g_CoWaitTimer <= 179)
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
const char* ac_nType[] = {"------", "三相四线220V", "三相三线100V", "三相四线57.7V"};
static int nState;
uint8 ac_set_index;
void ac_FeatureCfgInit(DWORD dwPluginId)
{
    ac_set_index = 0;
	return;
}

void ac_FeatureCfgDraw(HDC hdc)
{
	int nLeft1, nLeft2, y;
	nLeft1 = 20;
	nLeft2 = 90;
	y = 40;

	SelectFont(hdc,g_guiComm.fontSmall);

	TextOut(hdc, nLeft1, y, "交采电源");
    if (ac_set_index == 0)
    {
        ITEM_SELECT
        TextOut(hdc, nLeft2 + 50, y, ac_nType[nState]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2 + 50, y, ac_nType[nState]);
    }

    if (ac_set_index == 1)
    {
        ITEM_SELECT
        TextOut(hdc, nLeft2 + 60, y + 60, "保存");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2 + 60, y + 60, "保存");
    }
    PopListDraw(hdc);
}

BOOL ac_FeatureCfgKeyMsg(int nKey)
{
    int maxindex = 1;
    if (g_guiComm.bPopList == TRUE)
    {
        int nSel;
        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {   
            switch (ac_set_index)
            {
            case 0:
            {
                nState = nSel;
            }
            break;
            default:
            break;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
        }
        return TRUE;
    }
   switch (nKey)
    {
	    case KEY_UP:
	    {
	        if (ac_set_index > 0)
	            ac_set_index--;
	        else
	            ac_set_index = maxindex;
	        UpdateWindow(g_guiComm.hMainWnd, TRUE);
	        return TRUE;
	    }
	    case KEY_DOWN:
	    {
	        if (ac_set_index < maxindex)
	            ac_set_index++;
	        else
	            ac_set_index = 0;
	        UpdateWindow(g_guiComm.hMainWnd, TRUE);
	        return TRUE;
	    }
	    case KEY_OK:
	    {
	    	if (ValidatePwd() == TRUE)
	        {
		        if (ac_set_index == 0)
		        {    
		            PopList(ac_nType, 4, ac_nType[nState], 120, 20, FALSE);
		            return TRUE;
		        }
		        //保存配置
		        if (ac_set_index == 1)
		        {
		            //ret = write_normal_data((uint8*)&MeterFile, sizeof(OOP_METER_T), MeterFileOad, g_guiComm.nCurPoint, 0, 2);
		            if (ac_set_index == 0)
		            {
		                MessageBox("当前档案配置保存成功！");
		                ac_set_index = 0;
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


