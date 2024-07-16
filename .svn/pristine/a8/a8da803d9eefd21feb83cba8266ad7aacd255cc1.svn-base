/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   扩展控制模块 调度线程
* @date：    2021-5-30
* @history： 
*********************************************************************
*/
#include <sys/prctl.h>
#include "baseCommon.h"
#include "secModule.h"
#include "appctrl_data.h"
#include "oopVarUpdate.h"
#include "appcomm.h"
#include "label_id.h"
#include "oopservice.h"
#include "commClass.h"
#include "appctrl_module.h"

#if 0
CTRL_YX_FLAG_T    gCtrlyxFlag;
#endif

CTRL_YX_STATE_LIST_T gCtrlyxState;
CTRL_METER_LIST_T    gCtrlMeterList;


//未来不同产品，端口信息会有变化
const CTRL_PORT_CFG_T gPort[CTRL_PORT_NUM] = 
{
    {0, {0xF2010201}},
    {1, {0xF2010202}},
};

//端口传输状态
CTRL_PORT_STATE_T gState[CTRL_PORT_NUM];

//连接协商信息
CTRL_CNT_T gConnect;

//重要命令队列
CTRL_QUEUE_T  g_send;
CTRL_QUEUE_T  g_recv;


#if DESC("通用接口", 1)
/**********************************************************************
* @name      : app_module_get_msg
* @brief     ：连接协商消息处理
* @param[in] ：pMsgInfo 消息报文
               lenMax   报文最大长度
* @param[out]：buf       输出报文
* @return    ：
**********************************************************************/
uint32 app_module_get_msg(TASK_MSG_t *pMsgInfo, uint8 *buf)
{
    uint32 recvlen  = 0;
    uint16 sOffset = 0;    
    if ((pMsgInfo == NULL) || (buf == NULL))
    {
        return ERR_PTR;
    }

    recvlen = pMsgInfo->msg[sOffset++];
    if (recvlen == 0x81)
    {
        recvlen = pMsgInfo->msg[sOffset++];
    }
    else if (recvlen == 0x82)
    {
        recvlen = pMsgInfo->msg[sOffset++];
        recvlen <<= 8;
        recvlen += pMsgInfo->msg[sOffset++];
    }
    else if (recvlen > 0x82 || recvlen == 0x80)
    {
        return 0;
    }

    memcpy(buf, &pMsgInfo->msg[sOffset], recvlen);
    
    return recvlen;
}
#endif

#if DESC("遥信状态对外接口", 1)
/**********************************************************************
* @name      : app_module_yxstate_init
* @brief     ：遥信数据锁初始化
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
void app_module_yxstate_init(void)
{
    memset(&gCtrlyxState, 0, sizeof(gCtrlyxState));
    pthread_mutex_init(&gCtrlyxState.f_lock, NULL);
    return;
}

/**********************************************************************
* @name      : app_module_yxstate_clean
* @brief     ：遥信数据初始化
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
void app_module_yxstate_clean(void)
{
    uint8 i = 0;

    pthread_mutex_lock(&gCtrlyxState.f_lock);
    for (i = 0; i < CTRL_PORT_NUM; i++)
    {
        memset(&gCtrlyxState.yxStat[i], 0, sizeof(CTRL_YX_STATE_T));
    }
    pthread_mutex_unlock(&gCtrlyxState.f_lock);
    
    return;
}

/**********************************************************************
* @name      : app_module_yxstate_get
* @brief     ：遥信数据初获取(对外接口)
* @param[in] ：tsa 档案
* @param[out]：st  状态
* @return    ：错误码
**********************************************************************/
int app_module_yxstate_get(OOP_TSA_T tsa, OOP_SWITCHIN_STATE_T *st, OOP_SWITCHIN_FLAG_T *fg)
{
    int i = 0;
    int j = 0;
    int ret = ERR_NORMAL;
    int num = 0;

    if ((st == NULL) || (fg == NULL))
    {
        return ERR_PTR;
    }

    pthread_mutex_lock(&gCtrlyxState.f_lock);
    for (i = 0; i < CTRL_PORT_NUM; i++)
    {
        if (gCtrlyxState.yxStat[i].num == 0)
        {
            continue;
        }
        else
        {
            num = gCtrlyxState.yxStat[i].num;
        }
        
        for (j = 0; j < MIN(num, CTRL_DEV_NUM_MAX_PORT); j++)
        {
            if (memcmp(&gCtrlyxState.yxStat[i].tsa[j], &tsa, sizeof(OOP_TSA_T)) == 0)
            {
                *st = gCtrlyxState.yxStat[i].st[j];
                *fg = gCtrlyxState.yxStat[i].fg[j];
                ret = ERR_OK;
                break;
            }
        }

        if (ret == ERR_OK)
        {
            break;
        }
    }
    pthread_mutex_unlock(&gCtrlyxState.f_lock);
    
    return ret;
}

/**********************************************************************
* @name      : app_module_yxSec_get
* @brief     ：遥信数据更新时间获取(对外接口)
* @param[in] ：tsa 档案
* @param[out]：sec  遥信更新时间
* @return    ：错误码
**********************************************************************/
int app_module_yxSec_get(OOP_TSA_T tsa, uint32 *second)
{
    int i = 0;
    int j = 0;
    int ret = ERR_NORMAL;
    int num = 0;

    if (second == NULL)
    {
        return ERR_PTR;
    }

    pthread_mutex_lock(&gCtrlyxState.f_lock);
    for (i = 0; i < CTRL_PORT_NUM; i++)
    {
        if (gCtrlyxState.yxStat[i].num == 0)
        {
            continue;
        }
        else
        {
            num = gCtrlyxState.yxStat[i].num;
        }
        
        for (j = 0; j < MIN(num, CTRL_DEV_NUM_MAX_PORT); j++)
        {
            if (memcmp(&gCtrlyxState.yxStat[i].tsa[j], &tsa, sizeof(OOP_TSA_T)) == 0)
            {
                *second = gCtrlyxState.yxStat[i].sec[j];
                ret = ERR_OK;
                break;
            }
        }

        if (ret == ERR_OK)
        {
            break;
        }
    }
    pthread_mutex_unlock(&gCtrlyxState.f_lock);
    
    return ret;

}

/**********************************************************************
* @name      : app_module_yxstate_set
* @brief     ：遥信数据初获取(对外接口)
* @param[in] ：tsa 档案
* @param[out]：st   状态
* @return    ：错误码
**********************************************************************/
int app_module_yxstate_set(uint8 port, uint16 ctrlPn, OOP_SWITCHIN_STATE_T *st, OOP_SWITCHIN_FLAG_T *fg)
{
    int ret = ERR_NORMAL;

    struct timespec curtime = {0};

    if ((st == NULL) && (fg == NULL))
    {
        return ERR_PTR;
    }

    if (port >= CTRL_PORT_NUM)
    {
        return ERR_FAILPORT;
    }

    clock_gettime(CLOCK_MONOTONIC, &curtime);
    
    pthread_mutex_lock(&gCtrlyxState.f_lock);
    if (st != NULL)
    {
        gCtrlyxState.yxStat[port].st[ctrlPn] = *st;
    }
    
    if (fg != NULL)
    {
        gCtrlyxState.yxStat[port].fg[ctrlPn] = *fg;
    }
    
    gCtrlyxState.yxStat[port].sec[ctrlPn] = curtime.tv_sec;
    ret = ERR_OK;
    pthread_mutex_unlock(&gCtrlyxState.f_lock);
    
    return ret;
}

/**********************************************************************
* @name      : app_module_yxstate_set
* @brief     ：遥信数据初获取(对外接口)
* @param[in] ：num 控制模块档案数量
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_module_yxstate_update_num(uint8 port, uint32 num)
{
    int ret = ERR_NORMAL;

    if (port >= CTRL_PORT_NUM)
    {
        return ERR_FAILPORT;
    }
    
    pthread_mutex_lock(&gCtrlyxState.f_lock);
    gCtrlyxState.yxStat[port].num = num;
    ret = ERR_OK;
    pthread_mutex_unlock(&gCtrlyxState.f_lock);
    
    return ret;
}

/**********************************************************************
* @name      : app_module_yxstate_update_tsa
* @brief     ：遥信数据初获取(对外接口)
* @param[in] ：tsa 档案
* @param[out]：st   状态
* @return    ：错误码
**********************************************************************/
int app_module_yxstate_update_tsa(uint8 port, uint16 ctrlPn, OOP_TSA_T *tsa)
{
    int ret = ERR_NORMAL;

    if (tsa == NULL)
    {
        return ERR_PTR;
    }

    if (port >= CTRL_PORT_NUM)
    {
        return ERR_FAILPORT;
    }
    
    pthread_mutex_lock(&gCtrlyxState.f_lock);
    gCtrlyxState.yxStat[port].tsa[ctrlPn] = *tsa;
    ret = ERR_OK;
    pthread_mutex_unlock(&gCtrlyxState.f_lock);
    
    return ret;
}

#endif

#if DESC("遥信配置对外接口", 0)
/**********************************************************************
* @name      : app_module_yxflag_init
* @brief     ：遥信配置锁初始化
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
void app_module_yxflag_init(void)
{
    memset(&gCtrlyxFlag, 0, sizeof(gCtrlyxFlag));
    pthread_mutex_init(&gCtrlyxFlag.f_lock, NULL);
    return;
}

/**********************************************************************
* @name      : app_module_yxflag_clean
* @brief     ：遥信配置初始化
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
void app_module_yxflag_clean(void)
{
    pthread_mutex_lock(&gCtrlyxFlag.f_lock);
    memset(&gCtrlyxFlag.num, 0, sizeof(uint32));
    memset(&gCtrlyxFlag.tsa[0], 0, CTRL_DEV_NUM_MAX * sizeof(OOP_TSA_T));
    memset(&gCtrlyxFlag.fg[0], 0, CTRL_DEV_NUM_MAX * sizeof(OOP_SWITCHIN_FLAG_T));
    memset(&gCtrlyxFlag.isValid[0], 0, CTRL_DEV_NUM_MAX * sizeof(BOOL));
    pthread_mutex_unlock(&gCtrlyxFlag.f_lock);
    
    return;
}

/**********************************************************************
* @name      : app_module_yxflag_get
* @brief     ：遥信配置获取(对外接口)
* @param[in] ：tsa 档案
* @param[out]：st   状态
* @return    ：错误码
**********************************************************************/
int app_module_yxflag_get(OOP_TSA_T tsa, OOP_SWITCHIN_FLAG_T *fg)
{
    int i = 0;
    int ret = ERR_NORMAL;
    int num = 0;

    if (fg == NULL)
    {
        return ERR_PTR;
    }

    if (gCtrlyxFlag.num == 0)
    {
        num = CTRL_DEV_NUM_MAX;
    }
    else
    {
        num = gCtrlyxFlag.num;
    }
    
    pthread_mutex_lock(&gCtrlyxFlag.f_lock);
    for (i = 0; i < num; i++)
    {
        if (memcmp(&tsa, &gCtrlyxFlag.tsa[i], sizeof(OOP_TSA_T)) != 0)
        {
            *fg = gCtrlyxFlag.fg[i];
            ret = ERR_OK;
        }
    }
    pthread_mutex_unlock(&gCtrlyxFlag.f_lock);
    
    return ret;
}

/**********************************************************************
* @name      : app_module_yxflag_get
* @brief     ：遥信配置获取(对外接口)
* @param[in] ：tsa 档案
* @param[out]：st   状态
* @return    ：错误码
**********************************************************************/
int app_module_yxflag_isVaild(uint16 pn, OOP_TSA_T tsa, BOOL *isVaild)
{
    int ret = ERR_NORMAL;

    if (isVaild == NULL)
    {
        return ERR_PTR;
    }
    
    pthread_mutex_lock(&gCtrlyxFlag.f_lock);
    *isVaild = gCtrlyxFlag.isValid[pn];
    if (memcmp(&tsa, &gCtrlyxFlag.tsa[pn], sizeof(OOP_TSA_T)) != 0)
    {
        SM_FMT_DEBUG("控制模块档案可能有变化 pn[%d]\n", pn);
        *isVaild = FALSE;
    }
    ret = ERR_OK;
    pthread_mutex_unlock(&gCtrlyxFlag.f_lock);
    
    return ret;
}

/**********************************************************************
* @name      : app_module_yxflag_set
* @brief     ：遥信配置设置(对外接口)
* @param[in] ：tsa 档案
* @param[out]：st   状态
* @return    ：错误码
**********************************************************************/
int app_module_yxflag_set(uint16 ctrlPn, OOP_TSA_T tsa, OOP_SWITCHIN_FLAG_T *fg)
{
    int ret = ERR_NORMAL;

    if (fg == NULL)
    {
        return ERR_PTR;
    }
    
    pthread_mutex_lock(&gCtrlyxState.f_lock);
    gCtrlyxFlag.fg[ctrlPn] = *fg;
    gCtrlyxFlag.isValid[ctrlPn] = TRUE;
    ret = ERR_OK;
    pthread_mutex_unlock(&gCtrlyxState.f_lock);
    
    return ret;
}

/**********************************************************************
* @name      : app_module_yxflag_num_set
* @brief     ：档案数量设置(对外接口)
* @param[in] ：num 控制模块档案数量
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_module_yxflag_num_set(uint16 num)
{
    int ret = ERR_NORMAL;
    
    pthread_mutex_lock(&gCtrlyxState.f_lock);
    gCtrlyxState.num = num;
    ret = ERR_OK;
    pthread_mutex_unlock(&gCtrlyxState.f_lock);
    
    return ret;
}
#endif

#if DESC("控制模块档案更新", 1)
/**********************************************************************
* @name      : app_module_yxstate_init
* @brief     ：遥信数据锁初始化
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
void app_module_meter_init(void)
{
    memset(&gCtrlMeterList, 0, sizeof(gCtrlMeterList));
    pthread_mutex_init(&gCtrlMeterList.f_lock, NULL);
    return;
}

/**********************************************************************
* @name      : app_meter_is_module
* @brief     ：档案是否是控制模块
* @param[in] ：
* @param[out]：
* @return    ：BOOL 是扩展模块
**********************************************************************/
BOOL app_meter_is_module(OOP_METER_T *oopMeter)
{
    int    j = 0;
    
    if (oopMeter == NULL)
    {
        return FALSE;
    }

    if (oopMeter->annex.annexObj[0].oad.value != 0x40360200 ||
        oopMeter->annex.annexObj[0].value[0] != 0x04 ||
        oopMeter->annex.annexObj[0].value[1] != 0x02)
    {
        return FALSE;
    }

    //检查端口是否正确
    for (j = 0; j < CTRL_PORT_NUM; j++)
    {
        if (gPort[j].portOAD.value == oopMeter->basic.port.value)
        {
            break;
        }
    }
    if (j == CTRL_PORT_NUM)
    {
        return FALSE;
    }

    return TRUE;
}

/**********************************************************************
* @name      : app_module_meter_update
* @brief     ：档案数据更新
* @param[in] ：
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_module_meter_update(void)
{
    int    i = 0;
    int    j = 0;
    int    ret = ERR_OK;
    
    OOP_METER_T  oopMeter = {0};

    pthread_mutex_lock(&gCtrlMeterList.f_lock);
    for (j = 0; j < CTRL_PORT_NUM; j++)
    {
        gCtrlMeterList.Num[j] = 0;
        memset(&gCtrlMeterList.oopMeter[j][0], 0, CTRL_DEV_NUM_MAX_PORT * sizeof(OOP_METER_T));
    }
    pthread_mutex_unlock(&gCtrlMeterList.f_lock);
    
    for (i = 0; i <= NO_VIR_MTR_MAX_NUM; i++)
    {
        ret = app_get_meter_list(i, &oopMeter);
        if (ret != ERR_OK)
        {
            continue;
        }

        if (oopMeter.annex.annexObj[0].oad.value != 0x40360200 ||
            oopMeter.annex.annexObj[0].value[0] != 0x04 ||
            oopMeter.annex.annexObj[0].value[1] != 0x02)
        {
            continue;
        }

        //检查端口是否正确
        for (j = 0; j < CTRL_PORT_NUM; j++)
        {
            if (gPort[j].portOAD.value == oopMeter.basic.port.value)
            {
                break;
            }
        }
        if (j == CTRL_PORT_NUM)
        {
            continue;
        }

        pthread_mutex_lock(&gCtrlMeterList.f_lock);
        if (gCtrlMeterList.Num[j] >= CTRL_DEV_NUM_MAX_PORT)
        {
            ret = ERR_O_RANGE;
        }
        else
        {
            gCtrlMeterList.oopMeter[j][gCtrlMeterList.Num[j]] = oopMeter;
            gCtrlMeterList.Num[j]++;

            ret = app_module_yxstate_update_tsa(j, gCtrlMeterList.Num[j], &oopMeter.basic.tsa);
            if (ret != ERR_OK)
            {
                SM_FMT_DEBUG("更新遥信列表中模块地址失败 ret[%d]\n", ret);
            }
        }
        pthread_mutex_unlock(&gCtrlMeterList.f_lock);
    }

    //更新遥信列表中模块数量
    for (j = 0; j < CTRL_PORT_NUM; j++)
    {
        ret = app_module_yxstate_update_num(j, gCtrlMeterList.Num[j]);
        if (ret != ERR_OK)
        {
            SM_FMT_DEBUG("更新遥信列表中模块数量失败 ret[%d]\n", ret);
        }
    }

    return ret;
}

/**********************************************************************
* @name      : app_module_meter_get
* @brief     ：档案数据获取
* @param[in] ：信息点
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_module_meter_get(uint16 pn, uint16 port, OOP_METER_T *oopMeter)
{
    int ret = ERR_OK;

    if (port >= CTRL_PORT_NUM)
    {
        return ERR_O_RANGE;
    }

    pthread_mutex_lock(&gCtrlMeterList.f_lock);
    if (pn >= gCtrlMeterList.Num[port])
    {
        ret = ERR_NORESOURCE;
    }
    else
    {
        *oopMeter = gCtrlMeterList.oopMeter[port][pn];
    }
    pthread_mutex_unlock(&gCtrlMeterList.f_lock);
    
    return ret;
}

/**********************************************************************
* @name      : app_module_meter_num_get
* @brief     ：档案数量获取
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
int app_module_meter_num_get_port(uint16 port)
{
    int num = 0;
    
    if (port >= CTRL_PORT_NUM)
    {
        return 0;
    }

    pthread_mutex_lock(&gCtrlMeterList.f_lock);
    num = gCtrlMeterList.Num[port];
    pthread_mutex_unlock(&gCtrlMeterList.f_lock);
    
    return num;
}

/**********************************************************************
* @name      : app_module_meter_num_get_all
* @brief     ：档案数量获取
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
int app_module_meter_num_get_all(void)
{
    int num = 0;
    int i = 0;

    pthread_mutex_lock(&gCtrlMeterList.f_lock);
    for (i = 0; i < CTRL_PORT_NUM; i++)
    {
        num += gCtrlMeterList.Num[i];
    }
    pthread_mutex_unlock(&gCtrlMeterList.f_lock);
    
    return num;
}

#endif

#if DESC("get数据解析", 1)
/**********************************************************************
* @name：      invoke_get_res
* @brief：     解析apdu
* @param[in] ：ptFrame - 698信息
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
int app_module_invoke_get_res(CTRL_DEV_H_T *handle, TASK_MSG_t *pMsgInfo, uint8 *pInMsg, uint32 inLen)
{
    int ret = -1;
    int i = 0;
    OOP_OAD_U serviceOAD = {0};
    uint8     recvbuf[RECV_BUF_LEN];
    uint16    recvlen = 0;
    
    OOP_SWITCHIN_STATE_T st;
    OOP_SWITCHIN_FLAG_T  fg;

    memset(&st, 0, sizeof(st));
    memset(&fg, 0, sizeof(fg));

    memcpy_r(&serviceOAD, &pInMsg[0], sizeof(OOP_OAD_U));

    recvlen = app_module_get_msg(pMsgInfo, recvbuf);
    if ((recvlen == 0) || (recvlen > RECV_BUF_LEN))
    {
        return -1;
    }

    switch (serviceOAD.value)
    {
    case 0xf1000200:
    {
        ret = get_extModule_devid_parse(recvbuf, recvlen, handle->connect->devid, &handle->connect->idlen);
        if (ret != ERR_OK)
        {
            return ret;
        }
        else
        {
            TRANS_BUF_DEBUG(handle->connect->port, handle->connect->devid, handle->connect->idlen, "获取到devid");
            handle->pState->state = SEND_CN_STATE;
            handle->connect->cntState = CNT_GET_IK;
        }
        break;
    }
    case 0xf1000400:
    {
        ret = get_extModule_keyStatus_parse(recvbuf, recvlen, &handle->connect->modKeyStatus);
        if (ret != ERR_OK)
        {
            return ret;
        }
        else
        {
            TRANS_FMT_DEBUG(handle->connect->port, "获取到KeyStatus[%d]\n", handle->connect->modKeyStatus);
            handle->pState->state = SEND_CN_STATE;
            handle->connect->cntState = CNT_UPDATE;
        }
        break;
    }
    case 0xf2030200:
    {
        ret = get_extModule_yx_st_parse(recvbuf, recvlen, &st);
        if (ret != ERR_OK)
        {
            return ret;
        }
        else
        {
            for (i = 0; i < OOP_MAX_SWITCHIN; i++)
            {
                TRANS_FMT_DEBUG(handle->connect->port, "遥信状态[%d] st[%d] cd[%d]\n", i, st.state[i].st, st.state[i].cd);
            }
            ret = app_module_yxstate_set((uint8)((pMsgInfo->label & 0x0f00) >> 8), handle->pState->pn, &st, NULL);
            if (ret != ERR_OK)
            {
                TRANS_FMT_DEBUG(handle->connect->port, "遥信状态更新失败[%d]\n", ret);
            }
            handle->pState->state = SEND_YX_STATE;
        }
        break;
    }
    case 0xf2030400:
    {
        ret = get_extModule_yx_fg_parse(recvbuf, recvlen, &fg);
        if (ret != ERR_OK)
        {
            return ret;
        }
        else
        {
            TRANS_FMT_DEBUG(handle->connect->port, "遥信配置[%d] attrFlag[%d] inputFlag[%d]\n", i, fg.attrFlag, 
                fg.inputFlag);
            ret = app_module_yxstate_set((uint8)((pMsgInfo->label & 0x0f00) >> 8), handle->pState->pn, NULL, &fg);
            if (ret != ERR_OK)
            {
                TRANS_FMT_DEBUG(handle->connect->port, "遥信配置更新失败[%d]\n", ret);
            }
            handle->pState->state = SEND_YX_STATE;
        }
        break;
    }
    default:
        {
            TRANS_FMT_DEBUG(handle->connect->port, "不支持的OAD[0x%08X]\n", serviceOAD.value);
        }
        break;
    }
    
    return 0;
}

/**********************************************************************
* @name：      get_normal_res
* @brief：     解析apdu
* @param[in] ：ptFrame - 698信息
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
int app_module_get_normal_res(CTRL_DEV_H_T *handle, TASK_MSG_t *pMsgInfo, FRAME_HEAD_T *ptFrame)
{
    uint32 dataLen = 0;
    int32  offset = 0;
    OOP_OAD_U oad = {0};

    dataLen = 5; //偏掉OAD+DATA

    memcpy_r(&oad.value, &ptFrame->pApdu[OOP_DATA_OFFSET + offset], sizeof(oad.value));

    //判断是否是DATA
    if (ptFrame->pApdu[OOP_DATA_OFFSET+offset+dataLen-1] == 0)
    {
        ACSAMP_BUF_TRACE(&ptFrame->pApdu[OOP_DATA_OFFSET + offset], dataLen + 1, "find info from apdu oad[0x%08x] err", 
            oad.value);
        //DAR + 错误码
        offset += dataLen + 1;
    }
    else
    {
        if (!apdu_next_position_get(&ptFrame->pApdu[OOP_DATA_OFFSET+offset], &dataLen, ptFrame->apduLen, DT_NULL, FALSE))
        {
            return -1;
        }
        memcpy_r(&oad.value, &ptFrame->pApdu[OOP_DATA_OFFSET+offset], sizeof(oad.value));
        ACSAMP_BUF_TRACE(&ptFrame->pApdu[OOP_DATA_OFFSET + offset], dataLen, "find info from apdu oad[0x%08x]", 
            oad.value);

        app_module_invoke_get_res(handle, pMsgInfo, &ptFrame->pApdu[OOP_DATA_OFFSET + offset], dataLen);

        offset += dataLen; //APDU的内偏移
    }

    return 0;
}

/**********************************************************************
* @name：      app_module_get_normal_list_res
* @brief：     解析apdu
* @param[in] ：ptFrame - 698信息
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
int app_module_get_normal_list_res(CTRL_DEV_H_T *handle, TASK_MSG_t *pMsgInfo, FRAME_HEAD_T *ptFrame)
{
    uint8  sequenceNum = 0;
    uint32 dataLen = 0;
    int32  offset = 0;
    OOP_OAD_U oad = {0};

    sequenceNum = ptFrame->pApdu[3]; //序列个数

    while (sequenceNum--)
    {
        dataLen = 5; //偏掉OAD+DATA

        memcpy_r(&oad.value, &ptFrame->pApdu[OOP_DATA_LIST_OFFSET+offset], sizeof(oad.value));

        //判断是否是DATA
        if (ptFrame->pApdu[OOP_DATA_LIST_OFFSET+offset+dataLen-1] == 0)
        {
            ACSAMP_BUF_TRACE(&ptFrame->pApdu[OOP_DATA_LIST_OFFSET + offset], dataLen + 1, "find info from apdu oad[0x%08x] err", 
                oad.value);
            //DAR + 错误码
            offset += dataLen + 1;
        }
        else
        {
            if (!apdu_next_position_get(&ptFrame->pApdu[OOP_DATA_LIST_OFFSET+offset], &dataLen, ptFrame->apduLen, DT_NULL, FALSE))
            {
                return -1;
            }
            
            ACSAMP_BUF_TRACE(&ptFrame->pApdu[OOP_DATA_LIST_OFFSET + offset], dataLen, "find info from apdu oad[0x%08x]", 
                oad.value);
            
            app_module_invoke_get_res(handle, pMsgInfo, &ptFrame->pApdu[OOP_DATA_LIST_OFFSET + offset], dataLen);
            
            //APDU的内偏移
            offset += dataLen; 
        }
    }

    return 0;
}

/**********************************************************************
* @name：      app_get_response_analyse
* @brief：     解析apdu
* @param[in] ：ptFrame - 698信息
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
int app_module_get_response_analyse(CTRL_DEV_H_T *handle, TASK_MSG_t *pMsgInfo, FRAME_HEAD_T *ptFrame)
{
    uint8 reqtype = ptFrame->pApdu[1];

    switch (reqtype)
    {
    case REQ_GET_NORMAL:
        return app_module_get_normal_res(handle, pMsgInfo, ptFrame);
        break;
    
   case REQ_GET_NORMAL_LIST:
        return app_module_get_normal_list_res(handle, pMsgInfo, ptFrame);
        break;
   
    default:
        return ERR_ITEM;
        break;
    }

    return 0;
}
#endif

#if DESC("act数据解析", 1)
/**********************************************************************
* @name：      invoke_act_res
* @brief：     解析apdu
* @param[in] ：ptFrame - 698信息
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
int app_module_invoke_act_res(CTRL_DEV_H_T *handle, TASK_MSG_t *pMsgInfo, uint8 *pInMsg, uint32 inLen)
{
    int ret = -1;
    OOP_OAD_U serviceOAD = {0};
    uint8     recvbuf[RECV_BUF_LEN];
    uint16    recvlen = 0;

    memcpy_r(&serviceOAD, &pInMsg[0], sizeof(OOP_OAD_U));

    recvlen = app_module_get_msg(pMsgInfo, recvbuf);
    if ((recvlen == 0) || (recvlen > RECV_BUF_LEN))
    {
        return -1;
    }

    switch (serviceOAD.value)
    {
    case 0xf1008000:
    {
        ret = act_extModule_init_parse(recvbuf, recvlen);
        if (ret != ERR_OK)
        {
            return ret;
        }
        else
        {
            handle->pState->state = SEND_CN_STATE;
            handle->connect->cntState = CNT_GET_KEY;
        }
        break;
    }
    case 0xf100b000:
    {
        ret = negotiate_begin_parse(recvbuf, recvlen, handle->connect->cRanCat, &handle->connect->cRanCat_len);
        if (ret != ERR_OK)
        {
            return ret;
        }
        else
        {
            TRANS_BUF_DEBUG(handle->connect->port, handle->connect->cRanCat, handle->connect->cRanCat_len, "获取到cRanCat");
            handle->pState->state = SEND_CN_STATE;
            handle->connect->cntState = CNT_END_NEG;
        }
        break;
    }
    case 0xf100c000:
    {
        ret = negotiate_end_parse(recvbuf, recvlen);
        if (ret != ERR_OK)
        {
            return ret;
        }
        else
        {
            handle->pState->state = SEND_CN_STATE;
            handle->connect->cntState = CNT_SUCCESS;
            SM_BUF_DEBUG(handle->connect->curTsa.add, handle->connect->curTsa.len + 1, "连接协商成功 该模块tsa:");
        }
        break;
    }
    }
    
    return 0;
}

/**********************************************************************
* @name：      act_normal_res
* @brief：     解析apdu
* @param[in] ：ptFrame - 698信息
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
int app_module_act_normal_res(CTRL_DEV_H_T *handle, TASK_MSG_t *pMsgInfo, FRAME_HEAD_T *ptFrame)
{
    uint32 dataLen = 0;
    int32  offset = 0;
    OOP_OAD_U oad = {0};

    dataLen = 5; //偏掉OAD+DAR+DATA

    memcpy_r(&oad.value, &ptFrame->pApdu[OOP_DATA_OFFSET + offset], sizeof(oad.value));

    if (ptFrame->pApdu[OOP_DATA_OFFSET + offset+dataLen -1] != DATA_SUCCESS)
    {
        return -1;
    }

    //判断是否是DATA
    if (ptFrame->pApdu[OOP_DATA_OFFSET+offset+dataLen] == 0)
    {
        ACSAMP_BUF_TRACE(&ptFrame->pApdu[OOP_DATA_OFFSET + offset], dataLen, "find info from apdu oad[0x%08x] err", 
            oad.value);
        //DAR + 错误码
        offset += dataLen;
    }
    else
    {
        if (!apdu_next_position_get(&ptFrame->pApdu[OOP_DATA_OFFSET+offset], &dataLen, ptFrame->apduLen, DT_NULL, FALSE))
        {
            return -2;
        }
        memcpy_r(&oad.value, &ptFrame->pApdu[OOP_DATA_OFFSET+offset], sizeof(oad.value));
        ACSAMP_BUF_TRACE(&ptFrame->pApdu[OOP_DATA_OFFSET + offset], dataLen, "find info from apdu oad[0x%08x]", 
            oad.value);

        app_module_invoke_act_res(handle, pMsgInfo, &ptFrame->pApdu[OOP_DATA_OFFSET + offset], dataLen);

        offset += dataLen; //APDU的内偏移
    }

    return 0;
}

/**********************************************************************
* @name：      app_get_response_analyse
* @brief：     解析apdu
* @param[in] ：ptFrame - 698信息
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
int app_module_act_response_analyse(CTRL_DEV_H_T *handle, TASK_MSG_t *pMsgInfo, FRAME_HEAD_T *ptFrame)
{
    uint8 reqtype = ptFrame->pApdu[1];

    switch (reqtype)
    {
    case REQ_ACT_NORMAL:
        return app_module_act_normal_res(handle, pMsgInfo, ptFrame);
        break;
    
    default:
        return ERR_ITEM;
        break;
    }

    return 0;
}

#endif 

/**********************************************************************
* @name      : app_module_port_index_get
* @brief     ：获取端口索引
* @param[in] ：oad  端口OAD
* @param[out]：port  端口索引
* @return    ：错误码
**********************************************************************/
int app_module_port_index_get(OOP_OAD_U oad, uint8 *port)
{
    uint8 i = 0;

    for (i = 0; i < CTRL_PORT_NUM; i++)
    {
        if (gPort[i].portOAD.value == oad.value)
        {
            break;
        }
    }

    if (i == CTRL_PORT_NUM)
    {
        return ERR_NORMAL;
    }

    *port = i;

    return ERR_OK;
}

/**********************************************************************
* @name      : app_module_get_yx_state
* @brief     ：获取遥信数据
* @param[in] ：tsa 表地址
* @param[out]：st  遥信状态
* @return    ：
**********************************************************************/
int app_module_get_yx_state(OOP_TSA_T tsa, uint32 port, OOP_SWITCHIN_STATE_T *st)
{
    int32     ret = 0;
    uint8     sendbuf[SEND_BUF_LEN];
    uint32    sendlen = 0;
    uint8     recvbuf[RECV_BUF_LEN];
    uint16    recvlen = 0;

    if (st == NULL)
    {
        return ERR_PTR;
    }

    ret = get_extModule_yx_st_packaged(tsa, sendbuf, &sendlen);
    if (ret != ERR_OK)
    {
        SM_FMT_DEBUG("组织报文失败[%d]\n", ret);
        return ret;
    }

    ret = trans_buf_to_taskManager(sendbuf, sendlen, port, 6, 6, recvbuf, RECV_BUF_LEN, &recvlen);
    if (ret != ERR_OK)
    {
        SM_BUF_DEBUG(sendbuf, sendlen, "数据透传失败[%d]:", ret);
        return ret;
    }

    ret = get_extModule_yx_st_parse(recvbuf, recvlen, st);
    if (ret != ERR_OK)
    {
        SM_BUF_DEBUG(recvbuf, recvlen, "报文解析失败[%d]:", ret);
        return ret;
    }
    
    return ERR_OK;
}

/**********************************************************************
* @name      : app_module_get_fg_state
* @brief     ：获取遥信配置
* @param[in] ：tsa 表地址
* @param[out]：fg  遥信配置
* @return    ：
**********************************************************************/
int app_module_get_yx_flag(OOP_TSA_T tsa, uint32 port, OOP_SWITCHIN_FLAG_T *fg)
{
    int32     ret = 0;
    OOP_OAD_U oad = { 0 };
    uint8     sendbuf[SEND_BUF_LEN];
    uint32    sendlen = 0;
    uint8     recvbuf[RECV_BUF_LEN];
    uint16    recvlen = 0;

    if (fg == NULL)
    {
        return ERR_PTR;
    }

    ret = get_extModule_yx_fg_packaged(tsa, sendbuf, &sendlen);
    if (ret != ERR_OK)
    {
        SM_FMT_DEBUG("组织报文失败[%d] oad[0x%08x\n", ret, oad.value);
        return ret;
    }

    ret = trans_buf_to_taskManager(sendbuf, sendlen, port, 6, 6, recvbuf, RECV_BUF_LEN, &recvlen);
    if (ret != ERR_OK)
    {
        SM_BUF_DEBUG(sendbuf, sendlen, "数据透传失败[%d] oad[0x%08x]:", ret, oad.value);
        return ret;
    }

    ret = get_extModule_yx_fg_parse(recvbuf, recvlen, fg);
    if (ret != ERR_OK)
    {
        SM_BUF_DEBUG(recvbuf, recvlen, "报文解析失败[%d] oad[0x%08x]:", ret, oad.value);
        return ret;
    }
    
    return ERR_OK;
}

/**********************************************************************
* @name      : app_module_ctrl_entry
* @brief     ：控制入口函数
* @param[in] ：void *arg
* @param[out]：
* @return    ：
**********************************************************************/
/*void *app_module_ctrl_entry(void *pArg)
{
    uint32 pn = 0;
    uint32 ctrlPn = 0;
    int32  ret = ERR_NORMAL;
    uint32 failCnt = 0;
    
    CTRL_DEV_H_T *handle = (CTRL_DEV_H_T *)pArg;
    OOP_METER_T  oopMeter;
    
    OOP_SWITCHIN_STATE_T st;
    OOP_SWITCHIN_FLAG_T  fg;
        
    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    memset(&st, 0, sizeof(OOP_SWITCHIN_STATE_T));
    memset(&fg, 0, sizeof(OOP_SWITCHIN_FLAG_T));
    
    pthread_mutex_init(&handle->msgQueue.f_lock, NULL);
    ctrl_clean_queue(&handle->msgQueue);
    app_module_yxstate_init();
    app_module_yxflag_init();

    while (1)
    {
        if (pn > NO_VIR_MTR_MAX_NUM)
        {
            app_module_yxstate_num_set(ctrlPn);
            app_module_yxflag_num_set(ctrlPn);
            pn = 0;
            ctrlPn = 0;
        }

        ret = app_get_meter_list(pn, &oopMeter);
        if (ret != ERR_OK)
        {
            pn++;
            continue;
        }

        if (oopMeter.annex.annexObj[0].oad.value != 0x40360200 ||
            oopMeter.annex.annexObj[0].value[0] != 0x04 ||
            oopMeter.annex.annexObj[0].value[1] != 0x02)
        {
            pn++;
            continue;
        }

        pn++;
        ctrlPn++;
    }
    
    return NULL;
}*/

/**********************************************************************
* @name      : app_module_ctrl_msg_proc_cn
* @brief     ：连接协商消息处理
* @param[in] ：pMsgInfo 消息报文
* @param[out]：
* @return    ：
**********************************************************************/
int app_module_ctrl_msg_proc_cn(CTRL_DEV_H_T *handle, TASK_MSG_t *pMsgInfo)
{
    int32     ret = -1;
    uint8     recvbuf[RECV_BUF_LEN];
    uint16    recvlen = 0;
    uint16    sOffset = 0;
    
    FRAME_HEAD_T tFrame = {0};
    uint8        inServerID = 0;

    if (pMsgInfo == NULL)
    {
        return ERR_PTR;
    }

    recvlen = pMsgInfo->msg[sOffset++];
    if (recvlen == 0x81)
    {
        recvlen = pMsgInfo->msg[sOffset++];
    }
    else if (recvlen == 0x82)
    {
        recvlen = pMsgInfo->msg[sOffset++];
        recvlen <<= 8;
        recvlen += pMsgInfo->msg[sOffset++];
    }
    else if (recvlen > 0x82 || recvlen == 0x80)
    {
        return ERR_TYPE;
    }

    memcpy(recvbuf, &pMsgInfo->msg[sOffset], recvlen);
    TRANS_BUF_DEBUG((uint8)((pMsgInfo->label & 0x0f00) >> 8), recvbuf, recvlen, "接受报文数据");

    /* 解析帧头 */
    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    ret = app_get_frame(recvbuf, recvlen, &tFrame, TRUE);
    if (0 != ret)
    {
        TRANS_BUF_DEBUG((uint8)((pMsgInfo->label & 0x0f00) >> 8), recvbuf, recvlen, "输入报文有误 错误码[%d]", ret);
        return ERR_OPTION;
    }
    
    //数据解析
    inServerID = tFrame.pApdu[0];
    switch (inServerID)
    {
    case SERVERID_GETRESPONSE:
        return app_module_get_response_analyse(handle, pMsgInfo, &tFrame);
        break;

    case SERVERID_ACTIONRESPONSE:
        return app_module_act_response_analyse(handle, pMsgInfo, &tFrame);
        break;

    default:
        return ERR_ITEM;
        break;
    }

    return ERR_OK;
}

/**********************************************************************
* @name      : app_module_ctrl_msg_proc_ct
* @brief     ：连接协商消息处理
* @param[in] ：pMsgInfo 消息报文
* @param[out]：
* @return    ：
**********************************************************************/
int app_module_ctrl_msg_proc_ct(CTRL_DEV_H_T *handle, TASK_MSG_t *pMsgInfo)
{
    int32     ret = -1;
    uint8     recvbuf[RECV_BUF_LEN];
    uint16    recvlen = 0;
    uint16    sOffset = 0;
    
    FRAME_HEAD_T tFrame = {0};
    
    CTRL_DEV_MSG_T  *pMsgDev = NULL;

    if (pMsgInfo == NULL)
    {
        return ERR_PTR;
    }

    recvlen = pMsgInfo->msg[sOffset++];
    if (recvlen == 0x81)
    {
        recvlen = pMsgInfo->msg[sOffset++];
    }
    else if (recvlen == 0x82)
    {
        recvlen = pMsgInfo->msg[sOffset++];
        recvlen <<= 8;
        recvlen += pMsgInfo->msg[sOffset++];
    }
    else if (recvlen > 0x82 || recvlen == 0x80)
    {
        return ERR_TYPE;
    }

    memcpy(recvbuf, &pMsgInfo->msg[sOffset], recvlen);
    TRANS_BUF_DEBUG((uint8)((pMsgInfo->label & 0x0f00) >> 8), recvbuf, recvlen, "接受报文数据");

    /* 解析帧头 */
    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    ret = app_get_frame(recvbuf, recvlen, &tFrame, TRUE);
    if (0 != ret)
    {
        TRANS_BUF_DEBUG((uint8)((pMsgInfo->label & 0x0f00) >> 8), recvbuf, recvlen, "输入报文有误 错误码[%d]", ret);
        return ERR_OPTION;
    }

    pMsgDev = (CTRL_DEV_MSG_T*)malloc(sizeof(CTRL_DEV_MSG_T) + tFrame.apduLen);
    if (NULL == pMsgDev)
    {
        TRANS_FMT_DEBUG((uint8)((pMsgInfo->label & 0x0f00) >> 8), "申请内存失败\n");
        return ERR_NORESOURCE;
    }
    memset(pMsgDev, 0, sizeof(CTRL_DEV_MSG_T) + tFrame.apduLen);
    pMsgDev->pNext = NULL;
    pMsgDev->priority = 0;
    pMsgDev->apdulen = tFrame.apduLen;
    memcpy(pMsgDev->apdu, tFrame.pApdu, tFrame.apduLen);
    
    /* 写入消息队列 */
    ctrl_write_queue(handle->pApduRecv, pMsgDev, TRUE);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_module_ctrl_msg_proc_yx
* @brief     ：遥信消息处理
* @param[in] ：pMsgInfo 消息报文
* @param[out]：
* @return    ：
**********************************************************************/
int app_module_ctrl_msg_proc_yx(CTRL_DEV_H_T *handle, TASK_MSG_t *pMsgInfo)
{
    int32     ret = -1;
    uint8     recvbuf[RECV_BUF_LEN];
    uint16    recvlen = 0;
    uint16    sOffset = 0;
    
    FRAME_HEAD_T tFrame = {0};
    uint8        inServerID = 0;

    if (pMsgInfo == NULL)
    {
        return ERR_PTR;
    }

    recvlen = pMsgInfo->msg[sOffset++];
    if (recvlen == 0x81)
    {
        recvlen = pMsgInfo->msg[sOffset++];
    }
    else if (recvlen == 0x82)
    {
        recvlen = pMsgInfo->msg[sOffset++];
        recvlen <<= 8;
        recvlen += pMsgInfo->msg[sOffset++];
    }
    else if (recvlen > 0x82 || recvlen == 0x80)
    {
        return ERR_TYPE;
    }

    memcpy(recvbuf, &pMsgInfo->msg[sOffset], recvlen);
    TRANS_BUF_DEBUG((uint8)((pMsgInfo->label & 0x0f00) >> 8), recvbuf, recvlen, "接受报文数据");

    /* 解析帧头 */
    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    ret = app_get_frame(recvbuf, recvlen, &tFrame, TRUE);
    if (0 != ret)
    {
        TRANS_BUF_DEBUG((uint8)((pMsgInfo->label & 0x0f00) >> 8), recvbuf, recvlen, "输入报文有误 错误码[%d]", ret);
        return ERR_OPTION;
    }
    
    //数据解析
    inServerID = tFrame.pApdu[0];
    switch (inServerID)
    {
    case SERVERID_GETRESPONSE:
        return app_module_get_response_analyse(handle, pMsgInfo, &tFrame);
        break;

    default:
        return ERR_ITEM;
        break;
    }

    return ERR_OK;
}

/**********************************************************************
* @name      : app_module_ctrl_msg_proc
* @brief     ：控制模块消息处理
* @param[in] ：pMsgInfo 消息报文
* @param[out]：
* @return    ：
**********************************************************************/
int app_module_ctrl_msg_proc(CTRL_DEV_H_T *handle, TASK_MSG_t *pMsgInfo)
{
    switch (pMsgInfo->label & 0xf0ff)
    {
    case LABEL_HALT_TRANS_CN:
    {
        return app_module_ctrl_msg_proc_cn(handle, pMsgInfo);
        break;
    }
    case LABEL_HALT_TRANS_CT:
    {
        return app_module_ctrl_msg_proc_ct(handle, pMsgInfo);
        break;
    }
    case LABEL_HALT_TRANS_YX:
    {
        return app_module_ctrl_msg_proc_yx(handle, pMsgInfo);
        break;
    }
    default:
        break;
    }
    return ERR_OK;
}

/**********************************************************************
* @name      : app_module_ctrl_timeout_proc
* @brief     ：超时处理
* @param[in] ：void *arg 句柄
               port      端口
* @param[out]：
* @return    ：
**********************************************************************/
int app_module_ctrl_timeout_proc(void *pArg, uint8 port)
{
    int           ret = ERR_OK;
    CTRL_DEV_H_T *handle = (CTRL_DEV_H_T *)pArg;

    struct timespec curtime = {0};

    clock_gettime(CLOCK_MONOTONIC, &curtime);

    //判断传输状态
    switch (handle->pState[port].state)
    {
    case RECV_CN_STATE:
    {
        //判断端口是否还在接受等待状态，超时时，强制转换发送态
        if (curtime.tv_sec - handle->pState[port].second > CTRL_TIME_OUT)
        {
            handle->pState[port].state = SEND_CN_STATE;
    
            //如果达到最大尝试次数，不再连接
            handle->pState[port].failCnt ++;
            if (handle->pState[port].failCnt > FAIL_NUM_MAX)
            {
                TRANS_FMT_DEBUG(port, "达到最大尝试次数，不再连接 failCnt[%d]\n", handle->pState[port].failCnt);
                handle->pState[port].failCnt = 0;
                handle->pState[port].state = SEND_YX_STATE;
                handle->connect->cntState = CNT_IDLE;
            }
        }
        else
        {
            ret = ERR_NORESOURCE;
        }
        break;
    }
    case RECV_CT_STATE:
    {
        //判断端口是否还在接受等待状态，超时时，强制转换发送态
        if (curtime.tv_sec - handle->pState[port].second > CTRL_TIME_OUT)
        {
            handle->pState[port].state = SEND_CT_STATE;
    
            //如果达到最大尝试次数，不再连接
            handle->pState[port].failCnt ++;
            if (handle->pState[port].failCnt > FAIL_NUM_MAX)
            {
                TRANS_FMT_DEBUG(port, "达到最大尝试次数，不再发送 failCnt[%d]\n", handle->pState[port].failCnt);
                handle->pState[port].failCnt = 0;
                handle->pState[port].state = SEND_YX_STATE;
                handle->connect->cntState = CNT_IDLE;
            }
        }
        else
        {
            ret = ERR_NORESOURCE;
        }
        break;
    }
    case RECV_YX_STATE:
    {
        //判断端口是否还在接受等待状态，超时时，强制转换发送态
        if (curtime.tv_sec - handle->pState[port].second > CTRL_TIME_OUT)
        {
            handle->pState[port].state = SEND_YX_STATE;
    
            //如果达到最大尝试次数后，切换下一个模块
            handle->pState[port].failCnt ++;
            if (handle->pState[port].failCnt > FAIL_NUM_MAX)
            {
                TRANS_FMT_DEBUG(port, "达到最大尝试次数，切换下一个模块 failCnt[%d]\n", handle->pState[port].failCnt);
                handle->pState[port].failCnt = 0;
                handle->pState[port].pn ++;
            }
        }
        else
        {
            ret = ERR_NORESOURCE;
        }
        break;
    }
    default:
        break;
    }

    return ret;
}

/**********************************************************************
* @name      : app_module_ctrl_entry
* @brief     ：控制入口函数
* @param[in] ：void *arg
* @param[out]：
* @return    ：
**********************************************************************/
int app_module_ctrl_connect(void *pArg, uint8 port)
{
    int           ret = ERR_OK;
    CTRL_DEV_H_T *handle = (CTRL_DEV_H_T *)pArg;
    
    uint8  sendbuf[SEND_BUF_LEN];
    uint32 sendlen = 0;
    uint16 label = 0;

    struct timespec curtime = {0};

    if ((handle->connect->cntState == CNT_IDLE) || (handle->connect->port != port))
    {
        return ERR_PTR;
    }

    clock_gettime(CLOCK_MONOTONIC, &curtime);

    //判断传输状态
    switch (handle->pState[port].state)
    {
    case RECV_CN_STATE:
    {
        //判断端口是否还在接受等待状态，超时时，强制转换发送态
        if (curtime.tv_sec - handle->pState[port].second > CTRL_TIME_OUT)
        {
            handle->pState[port].state = SEND_CN_STATE;
        
            //如果达到最大尝试次数后，不再连接
            handle->pState[port].failCnt ++;
            if (handle->pState[port].failCnt > FAIL_NUM_MAX)
            {
                TRANS_FMT_DEBUG(port, "达到最大尝试次数，不再连接 failCnt[%d]\n", handle->pState[port].failCnt);
                handle->pState[port].failCnt = 0;
                handle->pState[port].state = CNT_IDLE;
                return ERR_TIMEOUT;
            }
        }
        else
        {
            return ERR_OK;
        }
        break;
    }
    case RECV_CT_STATE:
    {
        //判断端口是否还在接受等待状态，超时时，强制转换发送态
        if (curtime.tv_sec - handle->pState[port].second > 5)
        {
            handle->pState[port].state = SEND_CT_STATE;
        
            //如果达到最大尝试次数后，不再连接
            handle->pState[port].failCnt ++;
            if (handle->pState[port].failCnt > FAIL_NUM_MAX)
            {
                TRANS_FMT_DEBUG(port, "达到最大尝试次数，不再连接 failCnt[%d]\n", handle->pState[port].failCnt);
                handle->pState[port].failCnt = 0;
                handle->pState[port].state = CNT_IDLE;
                return ERR_TIMEOUT;
            }
        }
        else
        {
            return ERR_OK;
        }
        break;
    }
    default:
        break;
    }
    
    switch (handle->connect->cntState)
    {
    case CNT_GET_ID:
    {
        ret = get_extModule_devid_packaged(handle->connect->curTsa, sendbuf, &sendlen);
        if (ret != ERR_OK)
        {
            TRANS_FMT_DEBUG(port, "组织获取模块ID报文失败 ret[%d]\n", ret);
            break;
        }

        label = LABEL_HALT_TRANS_CN + (port << 8);
        ret = trans_buf_send_to_taskManager(label, sendbuf, sendlen, handle->connect->portOAD.value, 
            handle->connect->baud, 5);
        if (ret != ERR_OK)
        {
            TRANS_BUF_DEBUG(port, sendbuf, sendlen, "数据透传失败[%d]", ret);
        }
        else
        {
            handle->pState[port].state = RECV_CN_STATE;
            handle->pState[port].second = curtime.tv_sec;
            TRANS_BUF_DEBUG(port, sendbuf, sendlen, "发送连接数据", ret);
        }
        break;
    }
        
    case CNT_GET_IK:
    {
        ret = get_esam_factory_ik(handle->connect->devid, handle->connect->idlen, handle->connect->ik, &handle->connect->iklen);
        if (ret != ERR_OK)
        {
            TRANS_FMT_DEBUG(port, "获取esam出厂密钥(IK)失败 ret[%d]\n", ret);
            handle->connect->cntState = CNT_IDLE;
            break;
        }
        else
        {
            TRANS_BUF_DEBUG(port, handle->connect->ik, handle->connect->iklen, "获取esam出厂密钥(IK)成功 ik");
            handle->connect->cntState = CNT_SET_ID_IK;
        }
        break;
    }
        
    case CNT_SET_ID_IK:
    {
        ret = set_terminal_id_and_factory_ik(handle->connect->devid, handle->connect->idlen, handle->connect->ik, 
            handle->connect->iklen);
        if (ret != ERR_OK)
        {
            TRANS_FMT_DEBUG(port, "设置终端的devid和预置出厂IK失败 ret[%d]\n", ret);
            handle->connect->cntState = CNT_IDLE;
            break;
        }
        else
        {
            TRANS_FMT_DEBUG(port, "设置终端的devid和预置出厂IK成功 ret[%d]\n", ret);
            handle->connect->cntState = CNT_ACT_INIT;
        }
        break;
    }

    case CNT_ACT_INIT:
    {
        ret = act_extModule_init_packaged(handle->connect->curTsa, sendbuf, &sendlen);
        if (ret != ERR_OK)
        {
            TRANS_FMT_DEBUG(port, "封装软加密初始化报文给扩展模块失败 ret[%d]\n", ret);
            break;
        }

        label = LABEL_HALT_TRANS_CN + (port << 8);
        ret = trans_buf_send_to_taskManager(label, sendbuf, sendlen, handle->connect->portOAD.value, 
            handle->connect->baud, 5);
        if (ret != ERR_OK)
        {
            TRANS_BUF_DEBUG(port, sendbuf, sendlen, "数据透传失败[%d]", ret);
        }
        else
        {
            handle->pState[port].state = RECV_CN_STATE;
            handle->pState[port].second = curtime.tv_sec;
            TRANS_BUF_DEBUG(port, sendbuf, sendlen, "发送连接数据", ret);
        }
        break;
    }

    case CNT_GET_KEY:
    {
        ret = get_extModule_keyStatus_packaged(handle->connect->curTsa, sendbuf, &sendlen);
        if (ret != ERR_OK)
        {
            TRANS_FMT_DEBUG(port, "封装获取扩展模块密钥状态的报文失败 ret[%d]\n", ret);
            break;
        }

        label = LABEL_HALT_TRANS_CN + (port << 8);
        ret = trans_buf_send_to_taskManager(label, sendbuf, sendlen, handle->connect->portOAD.value, 
            handle->connect->baud, 5);
        if (ret != ERR_OK)
        {
            TRANS_BUF_DEBUG(port, sendbuf, sendlen, "数据透传失败[%d]", ret);
        }
        else
        {
            handle->pState[port].state = RECV_CN_STATE;
            handle->pState[port].second = curtime.tv_sec;
            TRANS_BUF_DEBUG(port, sendbuf, sendlen, "发送连接数据", ret);
        }
        break;
    }
    
    case CNT_UPDATE:
    {
        ret = update_terminal_keyStatus(handle->connect->modKeyStatus, handle->connect->devid, handle->connect->idlen);
        if (ret != ERR_OK)
        {
            TRANS_FMT_DEBUG(port, "终端根据模块的密钥状态调整自身密钥状态失败 ret[%d]\n", ret);
            handle->connect->cntState = CNT_IDLE;
            break;
        }
        else
        {
            TRANS_FMT_DEBUG(port, "终端根据模块的密钥状态调整自身密钥状态成功 ret[%d]\n", ret);
            handle->connect->cntState = CNT_BEGIN_NEG;
        }
        break;
    }

    case CNT_BEGIN_NEG:
    {
        ret = negotiate_begin_packaged(handle->connect->curTsa, sendbuf, &sendlen);
        if (ret != ERR_OK)
        {
            TRANS_FMT_DEBUG(port, "组织开始协商报文失败 ret[%d]\n", ret);
            break;
        }

        label = LABEL_HALT_TRANS_CN + (port << 8);
        ret = trans_buf_send_to_taskManager(label, sendbuf, sendlen, handle->connect->portOAD.value, 
            handle->connect->baud, 5);
        if (ret != ERR_OK)
        {
            TRANS_BUF_DEBUG(port, sendbuf, sendlen, "数据透传失败[%d]", ret);
        }
        else
        {
            handle->pState[port].state = RECV_CN_STATE;
            handle->pState[port].second = curtime.tv_sec;
            TRANS_BUF_DEBUG(port, sendbuf, sendlen, "发送连接数据", ret);
        }
        break;
    }

    case CNT_END_NEG:
    {
        ret = negotiate_end_packaged(handle->connect->curTsa, handle->connect->cRanCat, handle->connect->cRanCat_len, sendbuf, 
            &sendlen);
        if (ret != ERR_OK)
        {
            TRANS_FMT_DEBUG(port, "组织开始协商报文失败 ret[%d]\n", ret);
            break;
        }

        label = LABEL_HALT_TRANS_CN + (port << 8);
        ret = trans_buf_send_to_taskManager(label, sendbuf, sendlen, handle->connect->portOAD.value, 
            handle->connect->baud, 5);
        if (ret != ERR_OK)
        {
            TRANS_BUF_DEBUG(port, sendbuf, sendlen, "数据透传失败[%d]", ret);
        }
        else
        {
            handle->pState[port].state = RECV_CN_STATE;
            handle->pState[port].second = curtime.tv_sec;
            TRANS_BUF_DEBUG(port, sendbuf, sendlen, "发送连接数据", ret);
        }
        break;
    }
    
    case CNT_SUCCESS:
    {
        ret = cipher_encryption_packaged(handle->connect->curTsa, handle->connect->apdu, handle->connect->apdulen, 1, sendbuf, 
            &sendlen);
        if (ret != ERR_OK)
        {
            TRANS_BUF_DEBUG(port, handle->connect->apdu, handle->connect->apdulen, "APDU封装成完整报文失败[%d]:", ret);
        }
        else
        {
            label = LABEL_HALT_TRANS_CT + (port << 8);
            ret = trans_buf_send_to_taskManager(label, sendbuf, sendlen, handle->connect->portOAD.value, 
                handle->connect->baud, 5);
            if (ret != ERR_OK)
            {
                TRANS_BUF_DEBUG(port, sendbuf, sendlen, "数据透传失败[%d]", ret);
            }
            else
            {
                handle->pState[port].state = RECV_CT_STATE;
                handle->pState[port].second = curtime.tv_sec;
                TRANS_BUF_DEBUG(port, sendbuf, sendlen, "发送操作数据", ret);
            }
        }

        //不管重要报文发送成功是否都把连接协商处理结束掉
        handle->connect->cntState = CNT_IDLE;
        break;
    }

    default:
        break;
    }
    return ret;
}

/**********************************************************************
* @name      : app_module_ctrl_apdu
* @brief     ：控制入口函数
* @param[in] ：void *arg
* @param[out]：
* @return    ：
**********************************************************************/
int app_module_ctrl_apdu(void *pArg, uint8 port)
{
    int           ret = ERR_OK;
    CTRL_DEV_H_T *handle = (CTRL_DEV_H_T *)pArg;
    
    uint8  sendbuf[SEND_BUF_LEN];
    uint32 sendlen = 0;
    uint16 label = 0;

    struct timespec curtime = {0};

    if ((handle->pState[port].state != SEND_CT_STATE) || (handle->connect->port != port))
    {
        return ERR_PTR;
    }

    clock_gettime(CLOCK_MONOTONIC, &curtime);

    //判断传输状态
    switch (handle->pState[port].state)
    {
    case SEND_CT_STATE:
    {
        ret = cipher_encryption_packaged(handle->connect->curTsa, handle->connect->apdu, handle->connect->apdulen, 1, sendbuf, 
            &sendlen);
        if (ret != ERR_OK)
        {
            TRANS_BUF_DEBUG(port, handle->connect->apdu, handle->connect->apdulen, "APDU封装成完整报文失败[%d]:", ret);
        }
        else
        {
            label = LABEL_HALT_TRANS_CT + (port << 8);
            ret = trans_buf_send_to_taskManager(label, sendbuf, sendlen, handle->connect->portOAD.value, 
                handle->connect->baud, 5);
            if (ret != ERR_OK)
            {
                TRANS_BUF_DEBUG(port, sendbuf, sendlen, "数据透传失败[%d]", ret);
            }
            else
            {
                handle->pState[port].state = RECV_CT_STATE;
                handle->pState[port].second = curtime.tv_sec;
                TRANS_BUF_DEBUG(port, sendbuf, sendlen, "发送操作数据", ret);
            }
        }
        break;
    }

    default:
        break;
    }
    return ret;
}

/**********************************************************************
* @name      : app_module_ctrl_entry
* @brief     ：控制入口函数
* @param[in] ：void *arg
* @param[out]：
* @return    ：
**********************************************************************/
/*void *app_module_ctrl_entry(void *pArg)
{
    int32  ret = ERR_NORMAL;
    uint8  i = 0;
    uint8  portIndex = 0;
    uint8  port = 0;
    
    uint8  sendbuf[SEND_BUF_LEN];
    uint32 sendlen = 0;
    uint32 yxSec   = 0;

    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    CTRL_DEV_MSG_T  *pMsg = NULL;

    CTRL_DEV_H_T *handle = (CTRL_DEV_H_T *)pArg;
    handle->pState = &gState[0];
    
    OOP_METER_T   oopMeter;
    memset(&oopMeter, 0, sizeof(OOP_METER_T));

    //初始化时置发送态
    for (i = 0; i < CTRL_PORT_NUM; i++)
    {
        
        handle->pState[i].state = SEND_CT_STATE;
        handle->pState[i].pn = 0;

        //将端口上第一个档案的模块先进行连接
        ret = app_module_meter_get(handle->pState[i].pn, i, &oopMeter);
        if (ret != ERR_OK)
        {
            SM_FMT_DEBUG("获取控制模块档案失败[%d] port[%d]\n", ret, i);
            continue;
        }
        handle->pState[i].connect.isReconnect = TRUE;
        handle->pState[i].connect.lastTsa = oopMeter.basic.tsa;
        handle->pState[i].connect.curTsa = oopMeter.basic.tsa;
        handle->pState[i].connect.port = oopMeter.basic.port;
        handle->pState[i].connect.baud = oopMeter.basic.baud;
        handle->pState[i].connect.cntState = CNT_GET_ID;
    }

    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    while (1)
    {
        //协商连接优先处理
        for (i = 0; i < CTRL_PORT_NUM; i++)
        {
            if (handle->pState[i].connect.isReconnect)
            {
                app_module_ctrl_connect(pArg, i);
            }
        }

        clock_gettime(CLOCK_MONOTONIC, &curtime);
        
        //端口循环扫描(不同的产品可能端口数量不一样，但目前主要2个485端口)
        for (i = 0; i < CTRL_PORT_NUM; i++)
        {
            pMsg = ctrl_read_queue(&(handle->msgQueue));
            if (pMsg)
            {
                ret = app_module_port_index_get(pMsg->port, &port);
                if (ret != ERR_OK)
                {
                    free(pMsg);
                    
                    //记录退出时的端口，便于恢复到当前端口
                    portIndex = i;
                    break;
                }
                
                //发送操作数据
                if (memcmp(&handle->pState[port].connect.lastTsa, &pMsg->tsa, sizeof(OOP_TSA_T)) == 0)
                {
                    ret = cipher_encryption_packaged(pMsg->tsa, pMsg->apdu, pMsg->apdulen, 1, sendbuf, &sendlen);
                    if (ret != ERR_OK)
                    {
                        SM_BUF_DEBUG(pMsg->apdu, pMsg->apdulen, "port[%d]APDU封装成完整报文失败[%d]:", port, ret);
                    }
                    else
                    {
                        ret = trans_buf_send_to_taskManager(LABEL_HALT_TRANS_CT, sendbuf, sendlen, pMsg->port.value, pMsg->baud, 5);
                        if (ret != ERR_OK)
                        {
                            SM_BUF_DEBUG(sendbuf, sendlen, "port[%d]数据透传失败[%d]:", port, ret);
                        }
                        else
                        {
                            handle->pState[port].state = RECV_CT_STATE;
                            handle->pState[port].second = curtime.tv_sec;
                            SM_BUF_DEBUG(sendbuf, sendlen, "port[%d]发送操作数据[%d]:", port, ret);
                        }
                    }
                }
                else
                {
                    handle->pState[port].connect.isReconnect = TRUE;
                    handle->pState[port].connect.curTsa = pMsg->tsa;
                    handle->pState[port].connect.port = pMsg->port;
                    handle->pState[port].connect.baud = pMsg->baud;
                    handle->pState[port].connect.inMsg = (uint8 *)malloc(pMsg->apdulen * sizeof(uint8));
                    handle->pState[port].connect.msgLen = pMsg->apdulen;
                    memcpy(handle->pState[port].connect.inMsg, pMsg->apdu, pMsg->apdulen);
                    handle->pState[port].connect.cntState = CNT_GET_ID;

                    //不管连接是否成功，都要把当前要连接的模块设置成上一次连接的模块
                    handle->pState[port].connect.lastTsa = handle->pState[port].connect.curTsa;
                }
                
                free(pMsg);
                
                //记录退出时的端口，便于恢复到当前端口
                portIndex = i;
                break;
            }

            if (portIndex != 0)
            {
                i = portIndex;
                portIndex = 0;
            }
            
            if ((handle->pState[i].pn >= CTRL_DEV_NUM_MAX_PORT) || (handle->pState[i].pn >= app_module_meter_num_get_port(i)))
            {
                handle->pState[i].pn = 0;
            }

            if (handle->pState[i].state != SEND_YX_STATE)
            {
                //判断端口是否还在接受等待状态，超时时，强制转换发送态
                if (curtime.tv_sec - handle->pState[i].second > CTRL_TIME_OUT)
                {
                    handle->pState[i].state = SEND_YX_STATE;

                    //如果达到最大尝试次数后，切换下一个模块
                    handle->pState[i].failCnt ++;
                    if (handle->pState[i].failCnt > FAIL_NUM_MAX)
                    {
                        SM_FMT_DEBUG("达到最大尝试次数，切换下一个模块 port[%d] failCnt[%d]\n", i, handle->pState[i].failCnt);
                        handle->pState[i].failCnt = 0;
                        handle->pState[i].pn ++;
                    }
                }
                continue;
            }
            
            ret = app_module_meter_get(handle->pState[i].pn, i, &oopMeter);
            if (ret != ERR_OK)
            {
                SM_FMT_DEBUG("获取控制模块档案失败[%d] port[%d]\n", ret, i);
                handle->pState[i].pn ++;
                continue;
            }

            ret = app_module_yxSec_get(oopMeter.basic.tsa, &yxSec);
            if (ret != ERR_OK)
            {
                SM_FMT_DEBUG("获取遥信更新时间失败[%d]\n", ret);
            }
            else
            {
                if (curtime.tv_sec == yxSec)
                {
                    //该模块遥信刚刚刷新，无需读遥信
                    handle->pState[i].pn ++;
                    continue;
                }
            }

            ret = get_extModule_yx_st_packaged(oopMeter.basic.tsa, sendbuf, &sendlen);
            if (ret != ERR_OK)
            {
                SM_FMT_DEBUG("组织报文失败[%d]\n", ret);
                handle->pState[i].pn ++;
                continue;
            }

            ret = trans_buf_send_to_taskManager(LABEL_HALT_TRANS_YX, sendbuf, sendlen, oopMeter.basic.port.value, oopMeter.basic.baud, 5);
            if (ret != ERR_OK)
            {
                SM_BUF_DEBUG(sendbuf, sendlen, "port[%d]数据透传失败[%d]:", i, ret);
            }
            else
            {
                handle->pState[i].state = RECV_YX_STATE;
                handle->pState[i].second = curtime.tv_sec;
                SM_BUF_DEBUG(sendbuf, sendlen, "port[%d]发送遥信数据[%d]:", i, ret);
            }
        }

        if (oldtime.tv_sec == curtime.tv_sec)
        {
            usleep(50000);
        }
        else
        {
            oldtime = curtime;
        }
    }

    pthread_exit(NULL);
}*/

/**********************************************************************
* @name      : app_module_ctrl_entry
* @brief     ：控制入口函数
* @param[in] ：void *arg
* @param[out]：
* @return    ：
**********************************************************************/
void *app_module_ctrl_entry(void *pArg)
{
    int32  ret = ERR_NORMAL;
    uint8  i = 0;
    uint8  portIndex = 0;
    uint8  port = 0;
    
    uint8  sendbuf[SEND_BUF_LEN];
    uint32 sendlen = 0;
    uint32 yxSec   = 0;
    uint16 label = 0;

    //线程名
    prctl(PR_SET_NAME, "module_ctrl");
    SM_FMT_DEBUG("启动扩展控制模块处理线程\n");

    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    CTRL_DEV_MSG_T  *pMsg = NULL;

    CTRL_DEV_H_T *handle = (CTRL_DEV_H_T *)pArg;
    
    OOP_METER_T   oopMeter;
    memset(&oopMeter, 0, sizeof(OOP_METER_T));

    //安全算法库回调函数初始化
    init_extModule_callback_fun();

    //初始化时置发送态
    for (i = 0; i < CTRL_PORT_NUM; i++)
    {
        handle->pState[i].state = SEND_YX_STATE;
        handle->pState[i].pn = 0;
    }

    //将第一个端口上第一个档案的模块先进行连接
    for (i = 0; i < CTRL_PORT_NUM; i++)
    {
        ret = app_module_meter_get(handle->pState[i].pn, i, &oopMeter);
        if (ret != ERR_OK)
        {
            SM_FMT_DEBUG("获取控制模块档案失败[%d] port[%d]\n", ret, i);
        }
        else
        {
            SM_FMT_DEBUG("获取控制模块档案成功[%d] port[%d]\n", ret, i);
            break;
        }
    }

    if (i < CTRL_PORT_NUM)
    {
        handle->connect->lastTsa = oopMeter.basic.tsa;
        handle->connect->curTsa = oopMeter.basic.tsa;
        handle->connect->portOAD = oopMeter.basic.port;
        handle->connect->port = i;
        handle->connect->baud = oopMeter.basic.baud;
        handle->connect->cntState = CNT_GET_ID;
        handle->pState[i].state = SEND_CN_STATE;
        TRANS_FMT_DEBUG(i, "获取到档案，将第一个端口上第一个档案的模块先进行连接\n");
    }
    else
    {
        SM_FMT_DEBUG("没有获取到控制模块档案\n");
    }

    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    while (1)
    {
        *handle->pRunFlag = 0;
        
        clock_gettime(CLOCK_MONOTONIC, &curtime);

        for (i = 0; i < CTRL_PORT_NUM; i++)
        {
            //超时检测
            ret = app_module_ctrl_timeout_proc(pArg, i);
            if (ret != ERR_OK)
            {
                continue;
            }
            
            //协商连接优先处理
            if ((handle->connect->cntState != CNT_IDLE) && (i == handle->connect->port))
            {
                ret = app_module_ctrl_connect(pArg, i);
                if (ret == ERR_OK)
                {
                    continue;
                }
            }

            //重要报文优先处理
            if ((handle->pState[i].state == SEND_CT_STATE) && (i == handle->connect->port))
            {
                ret = app_module_ctrl_apdu(pArg, i);
                if (ret == ERR_OK)
                {
                    continue;
                }
            }

            pMsg = ctrl_read_queue(handle->pApduSend);
            if (pMsg)
            {
                ret = app_module_port_index_get(pMsg->portOAD, &port);
                if (ret != ERR_OK)
                {
                    free(pMsg);
                    
                    //记录退出时的端口，便于恢复到当前端口
                    portIndex = i;
                    break;
                }
                
                //发送操作数据
                if (memcmp(&handle->connect->lastTsa, &pMsg->tsa, sizeof(OOP_TSA_T)) == 0)
                {
                    ret = cipher_encryption_packaged(pMsg->tsa, pMsg->apdu, pMsg->apdulen, 1, sendbuf, &sendlen);
                    if (ret != ERR_OK)
                    {
                        TRANS_BUF_DEBUG(port, pMsg->apdu, pMsg->apdulen, "APDU封装成完整报文失败[%d]:", ret);
                    }
                    else
                    {
                        label = LABEL_HALT_TRANS_CT + (port << 8);
                        ret = trans_buf_send_to_taskManager(label, sendbuf, sendlen, pMsg->portOAD.value, pMsg->baud, 5);
                        if (ret != ERR_OK)
                        {
                            TRANS_BUF_DEBUG(port, sendbuf, sendlen, "数据透传失败[%d]", ret);
                        }
                        else
                        {
                            handle->pState[port].state = RECV_CT_STATE;
                            handle->pState[port].second = curtime.tv_sec;
                            TRANS_BUF_DEBUG(port, sendbuf, sendlen, "发送操作数据", ret);
                        }
                    }
                }
                else
                {
                    handle->connect->curTsa = pMsg->tsa;
                    handle->connect->cntState = CNT_GET_ID;

                    //不管连接是否成功，都要把当前要连接的模块设置成上一次连接的模块
                    handle->connect->lastTsa = handle->connect->curTsa;
                }

                //保存重要报文，可能需要重试
                handle->connect->portOAD = pMsg->portOAD;
                handle->connect->baud = pMsg->baud;
                handle->connect->apdu = (uint8 *)malloc(pMsg->apdulen * sizeof(uint8));
                handle->connect->apdulen = pMsg->apdulen;
                memcpy(handle->connect->apdu, pMsg->apdu, pMsg->apdulen);
                handle->connect->port = i;
                
                free(pMsg);
                
                //记录退出时的端口，便于恢复到当前端口
                portIndex = i;
                break;
            }

            if (portIndex != 0)
            {
                i = portIndex;
                portIndex = 0;
            }
            
            if ((handle->pState[i].pn >= CTRL_DEV_NUM_MAX_PORT) || (handle->pState[i].pn >= app_module_meter_num_get_port(i)))
            {
                handle->pState[i].pn = 0;
            }
            
            ret = app_module_meter_get(handle->pState[i].pn, i, &oopMeter);
            if (ret != ERR_OK)
            {
                TRANS_FMT_DEBUG(i, "获取控制模块档案失败[%d] pn[%d]\n", ret, handle->pState[i].pn);
                handle->pState[i].pn ++;
                continue;
            }

            ret = app_module_yxSec_get(oopMeter.basic.tsa, &yxSec);
            if (ret != ERR_OK)
            {
                TRANS_FMT_DEBUG(i, "获取遥信更新时间失败[%d] pn[%d]\n", ret, handle->pState[i].pn);
            }
            else
            {
                if (curtime.tv_sec == yxSec)
                {
                    //该模块遥信刚刚刷新，无需读遥信
                    handle->pState[i].pn ++;
                    continue;
                }
            }

            ret = get_extModule_yx_st_fg_packaged(oopMeter.basic.tsa, sendbuf, &sendlen);
            if (ret != ERR_OK)
            {
                TRANS_FMT_DEBUG(i, "组织报文失败[%d]\n", ret);
                handle->pState[i].pn ++;
                continue;
            }

            label = LABEL_HALT_TRANS_YX + (i << 8);
            ret = trans_buf_send_to_taskManager(label, sendbuf, sendlen, oopMeter.basic.port.value, oopMeter.basic.baud, 5);
            if (ret != ERR_OK)
            {
                TRANS_BUF_DEBUG(i, sendbuf, sendlen, "数据透传失败[%d]", ret);
            }
            else
            {
                handle->pState[i].state = RECV_YX_STATE;
                handle->pState[i].second = curtime.tv_sec;
                TRANS_BUF_DEBUG(i, sendbuf, sendlen, "发送遥信数据", ret);
            }
        }

        if (oldtime.tv_sec == curtime.tv_sec)
        {
            usleep(50000);
        }
        else
        {
            oldtime = curtime;
        }
    }

    pthread_exit(NULL);
}

