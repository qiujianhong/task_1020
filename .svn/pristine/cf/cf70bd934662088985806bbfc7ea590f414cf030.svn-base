/*
*********************************************************************
* Copyright(C) 2024 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   负荷控制APP数据初始化
* @date：    2024-06-05
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "oopType.h"
#include "db_api.h"
#include "framecomm.h"
#include "task_id.h"

#include "loadCtrller_data.h"
#include "loadCtrller_event.h"

int    g_dbEvent = 0;


/**********************************************************************
* @name      : loadctrl_event_init
* @brief     ：事件处理初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2024-7-1
* @Update    :
**********************************************************************/
int loadctrl_event_init(void *pPublic, void **ppPrivate)
{
    LCTRL_DATA_T    *ptLCData = (LCTRL_DATA_T*)pPublic;

    g_dbEvent = db_client_create(DB_LOADCTRLLER_AGENT, DB_LOADCTRLLER_EVENT);
    if(g_dbEvent <= 0)
    {
        printf("db_client_create failed!\n");
        return -1;
    }

    /* 等待数据中心 */
    while(FALSE == ptLCData->hasDB)
    {
        usleep(100000);
    }

    /* 事件初始化 */

    *ppPrivate = NULL;
    return 0;
}

/**********************************************************************
* @name      : loadctrl_event_proc
* @brief     ：事件处理
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 
* @Date      ：2024-7-1
* @Update    :
**********************************************************************/
int loadctrl_event_proc(void *pArg, void *pPublic, void *pPrivate)
{
    //LCTRL_DATA_T    *ptLCData = (LCTRL_DATA_T*)pPublic;
    
    

    
    
    return 1000000;
}

