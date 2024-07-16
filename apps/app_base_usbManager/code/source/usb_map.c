/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控mqtt
* @date：    2020-6-30
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "framecomm.h"
#include "task_base.h"

#include "cJSON.h"
#include "usb_init.h"
#include "usb_map.h"


int usb_hotplug_init()  
{  
    const int buffersize = 1024;  
    int ret;  

    struct sockaddr_nl snl;  
    bzero(&snl, sizeof(struct sockaddr_nl));  
    snl.nl_family = AF_NETLINK;  
    snl.nl_pid = getpid();  
    snl.nl_groups = 1;  

    int s = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);  
    if (s == -1)   
    {  
        perror("socket");  
        return -1;  
    }  
    setsockopt(s, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize));  

    ret = bind(s, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl));  
    if (ret < 0)   
    {  
        perror("bind");  
        close(s);  
        return -1;  
    }  

    return s;  
}  

int usb_map_init(void *pPublic, void **ppPrivate)
{
    int ret = 0;
    USB_MAP_T *ptData = NULL;
    
    ptData = (USB_MAP_T *)malloc(sizeof(USB_MAP_T));
    if(NULL == ptData)
    {
        USB_FMT_DEBUG("usb_update_init malloc failed\n");
        return -1;
    }
    
    memset(ptData, 0, sizeof(USB_MAP_T));

    ptData->socket = usb_hotplug_init();

    *ppPrivate = ptData;

    return ret;
}

int usb_map_add(USB_DEVICES_T *pUsbDev)
{
    int ret = ERR_OK;
    FILE* fp = NULL;
    FILE* fp2 = NULL;
//    FILE* fp3 = NULL;
    char cmd[512] = {0};
    char buf[512] = {0};
    char *tmp;
    char tmpStr[512] = {0};
    char tmpStr1[256] = {0};
    char *str;
    //char filepath[20] = {0};
    char ttyName[256] = {0};
    char mapName[256] = {0};
    char linkName[256] = {0};
    uint8 acmid = 0;

    sprintf(cmd, "ls %s:1.* -d",pUsbDev->path);

    fp = popen(cmd, "r"); 
    if (NULL == fp)   
    {   
        USB_FMT_DEBUG("%s err\n", cmd);   
        ret = -1;
        return ret;   
    }
    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        tmp = buf;
        USB_FMT_DEBUG("result = %s\n", buf);
    
        sprintf(&tmpStr[0], "%s:1.", pUsbDev->path);
        
        if ((str = strstr(tmp, tmpStr)) == NULL)
        {
            break;
        }
        else
        {
            USB_FMT_DEBUG("str = %s no = %d\n", str, str[strlen(tmpStr)] - 0x30);
        }
    
        if((str[strlen(tmpStr)] - 0x30)%2 == 0)
        {
            acmid = (str[strlen(tmpStr)] - 0x30)/2 ;
        }
        else
        {
            continue;
        }

        //去掉末尾的换行符
        if(strncpy(tmpStr1,str,strlen(buf)-1) == NULL)
        {
            break;
        }
        USB_FMT_DEBUG("tmpStr1 = %s\n",tmpStr1);
        
//        strncpy(filepath,buf,3);
//        USB_FMT_DEBUG("filepath = %s,buf = %s\n",filepath,buf);

        memset(cmd, 0, sizeof(cmd));
        
        sprintf(cmd,"ls %s/tty/ | egrep ttyACM*\\|usb*",tmpStr1);
        USB_FMT_DEBUG("%s\n",cmd);
        memset(buf,0x0,sizeof(buf));
        fp2 = popen(cmd,"r");
        if(fp2 == NULL)
        {
            USB_FMT_DEBUG("%s err\n", cmd);   
            continue;
        }

        if(fgets(buf,sizeof(buf),fp2) == NULL)
        {
            USB_FMT_DEBUG("未找到 ttyACM*或者usb*\n");
            pclose(fp2);   
            continue;
        }
        
        if(buf[0] == 0x00)
        {
            pclose(fp2);   
            continue;
        }
        memset(tmpStr1,0x0,sizeof(tmpStr1));
        if(strstr(buf,"ttyACM") != NULL)
        {
            strncpy(tmpStr1,buf,strlen(buf)-1);
        }
        else if(strstr(buf,"usb") != NULL)
        {
            strncpy(tmpStr1,buf,strlen(buf)-1);
        }
        else
        {
            pclose(fp2);   
            continue;
        }
        
        sprintf(ttyName,"/dev/%s", tmpStr1);
        
        USB_FMT_DEBUG("ttyName = %s\n",ttyName);
        USB_FMT_DEBUG("acmid = %d\n", acmid);

        if(acmid < pUsbDev->linkerNum)
        {
            sprintf(mapName, "%s", pUsbDev->linker[acmid]);
        }
        else
        {
            pclose(fp2);
            continue;
        }
        
        // 已经建立映射直接退出
        if ((access(mapName, F_OK) >= 0) && (readlink(mapName, linkName, 255) == 0) && (strcmp(linkName, ttyName) == 0))
        {
            USB_FMT_DEBUG("存在软链接的设备名 name = %s\n", mapName);
            pUsbDev->status = USB_DEV_ADD_OK;
        }
        else
        {
            ret = remove(mapName);
            if(ret != 0)
            {
                USB_FMT_DEBUG("remove \"%s\" failed(%d), %s\n", mapName, ret, strerror(errno));
            }

            ret = symlink(ttyName, mapName);
            if(ret != 0)
            {
                USB_FMT_DEBUG("symlink \"%s\"->\"%s\" failed(%d), %s\n", mapName, ttyName, ret, strerror(errno));
            }              
        
            USB_FMT_DEBUG("创建软链接的设备名 name = %s\n", mapName);
            pUsbDev->status = USB_DEV_ADD_OK;
        } 
        
        pclose(fp2);
    }

    pclose(fp);

    return ret;
}

int usb_map_remove(USB_DEVICES_T *pUsbDev)
{
    int ret = ERR_OK;
    char ttyacm[512] = {0};
    int acmid = 0;

    for(acmid = 0; acmid < pUsbDev->linkerNum; acmid++)
    {
        sprintf(ttyacm, "%s",pUsbDev->linker[acmid]);
        ret = remove(ttyacm);
        if(ret != 0)
        {
            USB_FMT_DEBUG("remove \"%s\" failed(%d), %s\n", ttyacm, ret, strerror(errno));
        }
        else
        {
            USB_FMT_DEBUG("remove \"%s\" success.\n", ttyacm);
            pUsbDev->status = USB_DEV_RMV_OK;
        }        
    }

    return ret;
}

/**********************************************************************
* @name      : uart_work_schedule
* @brief     ：串口服务业务线程
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int usb_map_proc(void *pArg, void *pPublic, void *pPrivate)
{
    int i = 0, j = 0;
    char buf[UEVENT_BUFFER_SIZE * 2] = {0};  
    USB_APPDATA_T *pAppData = (USB_APPDATA_T *)pPublic;
//    USB_MAP_T *pMap = (USB_MAP_T *)pPrivate;
    
    //recv(pMap->socket, &buf, sizeof(buf), 0);  
    //USB_FMT_DEBUG("%s\n", buf);
    static uint8   s_baseOK = 0;

    if(0 == s_baseOK)
    {
        s_baseOK = 1;
        BASE_TASK_SET_OK(pArg); //基础APP完成准备工作
    }

    for(i = 0; i < pAppData->devNum; i++)
    {
        if((path_exist(pAppData->devices[i].path) == TRUE)
            &&((pAppData->devices[i].status == USB_DEV_REMOVE)||(pAppData->devices[i].status == USB_DEV_RMV_OK)))
        {
            USB_FMT_DEBUG("%s add.\n", pAppData->devices[i].name);
            pAppData->devices[i].status = USB_DEV_ADD;
        }
        
        if((path_exist(pAppData->devices[i].path) == FALSE)
            &&((pAppData->devices[i].status == USB_DEV_ADD)||(pAppData->devices[i].status == USB_DEV_ADD_OK)))
        {
            USB_FMT_DEBUG("%s remove.\n", pAppData->devices[i].name);
            pAppData->devices[i].status = USB_DEV_REMOVE;
        }
    
        /* USB设备的插拔会出现字符信息，通过比较不同的信息确定特定设备的插拔*/
        if((memcmp(buf,pAppData->devices[i].add,strlen(pAppData->devices[i].add))==0)
            &&(pAppData->devices[i].status == USB_DEV_REMOVE))
        {
            USB_FMT_DEBUG("%s add.\n", pAppData->devices[i].name);
            pAppData->devices[i].status = USB_DEV_ADD;
        }
        
        if((memcmp(buf,pAppData->devices[i].remove,strlen(pAppData->devices[i].remove))==0)
            &&(pAppData->devices[i].status == USB_DEV_ADD))
        {
            USB_FMT_DEBUG("%s remove.\n", pAppData->devices[i].name);
            pAppData->devices[i].status = USB_DEV_REMOVE;
        }

        //路径存在，但软链接不存在
        if(path_exist(pAppData->devices[i].path) == TRUE)
        {
            for(j = 0; j < pAppData->devices[i].linkerNum; j++)
            {
                if (access(pAppData->devices[i].linker[j], F_OK) < 0) //软链接不存在
                {
                    pAppData->devices[i].status = USB_DEV_ADD;
                    break;
                }
            }
        }

        if(pAppData->devices[i].status == USB_DEV_ADD)
        {
            usb_map_add(&pAppData->devices[i]);
        }
        else if(pAppData->devices[i].status == USB_DEV_REMOVE)
        {
            usb_map_remove(&pAppData->devices[i]);
        }
    }

    return 100000;
}

