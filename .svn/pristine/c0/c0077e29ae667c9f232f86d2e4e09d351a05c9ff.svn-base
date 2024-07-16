/*
*********************************************************************
* @file    AutoTask.c
* @brief： 统计模块实现文件
* @author：贺宁
* @date：   2019.11.11
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/

#include "framecomm.h"
#include "storage.h"
#include "db_api.h"
#include "statmain.h"
#include "statlib.h"
#include "oopVarUpdate.h"
#include "msg_id.h"
extern uint8  g_udisk_status;
/* 通信句柄，一个线程全局唯一*/
extern DB_CLIENT g_staUdpHandle;
extern uint8   g_MinuteCalcEn;

extern OOP_DATETIME_S_T   realstattime;     /*系统实时时间,全局时间变量*/
extern OOP_DATETIME_S_T   laststattime;     /*系统上一次统计时间*/

/*以下为时间过分,过时,过日,过月标记*/
extern uint8    pastminflag;
extern uint8    pasthourflag;
extern uint8    pastdayflag;
extern uint8    pastmonflag;

//交采实时数据
extern STA_ACDATA_T g_acData;

extern OOP_METER_P_LIST_T oopMeterList;

extern STA_PORT_AUTHORIZE_INFO g_portRS232;
extern STA_PORT_AUTHORIZE_INFO g_portRS485;
extern STA_PORT_AUTHORIZE_INFO g_portUSB;
extern STA_PORT_AUTHORIZE_INFO g_portIR;
extern STA_PORT_AUTHORIZE_INFO g_portSSH;
extern STA_PORT_AUTHORIZE_INFO g_portSFTP;
extern void CheckERC_3140(int fd, uint32 msg_id, uint8 *pmsg, uint16 msglen);
//总加组存储序号，电控用
uint32 g_tgStoreNo = 0;
STA_TGDEL_T g_tgdel = {0};

extern uint8 app_get_localaddr(uint8 **ppAddr);
extern uint8 pulse_get_ok();

STA_SLIPPOWER_FIFO_T g_slippower_fifo[OOP_MAX_TGSETS] = {{0}}; //滑差功率缓存队列
extern OOP_SERVICES_T g_serData;
#if DESC("公共函数实现",1)

/**
*********************************************************************
* @brief：      long unsigned型数据最大值
* @param[in]： bItemNum  数据项数
* @param[in]： awSrc      求值数据项
* @return：     计算结果
*********************************************************************
*/
uint16 sta_lmax_calc(uint8 bItemNum, uint16 *awSrc)
{
    uint8 i;
    uint16  wMax=0;
    
    for(i=0; i<bItemNum; i++)
        wMax = ((awSrc[i]>wMax) ? awSrc[i]:wMax);
    return wMax;
}

/**
*********************************************************************
* @brief：      long unsigned型数据最小值
* @param[in]： bItemNum  数据项数
* @param[in]： awSrc      求值数据项
* @return：     计算结果
*********************************************************************
*/
uint16 sta_lmin_calc(uint8 bItemNum, uint16 *awSrc)
{
    uint8 i=0;
    uint16  wMin=awSrc[i];
    
    for(i=1; i<bItemNum; i++)
        wMin = ((awSrc[i]>wMin) ? wMin:awSrc[i]);
    return wMin;
}

/**
*********************************************************************
* @brief：      double long unsigned型数据最大值
* @param[in]： bItemNum  数据项数
* @param[in]： awSrc      求值数据项
* @return：     计算结果
*********************************************************************
*/
uint32 sta_dlmax_calc(uint8 bItemNum, uint32 *awSrc)
{
    uint8 i;
    uint32  wMax=0;
    
    for(i=0; i<bItemNum; i++)
        wMax = ((awSrc[i]>wMax) ? awSrc[i]:wMax);
    return wMax;
}

/**
*********************************************************************
* @brief：      double long unsigned型数据最小值
* @param[in]： bItemNum  数据项数
* @param[in]： awSrc      求值数据项
* @return：     计算结果
*********************************************************************
*/
uint32 sta_dlmin_calc(uint8 bItemNum, uint32 *awSrc)
{
    uint8 i=0;
    uint32  wMin=awSrc[i];
    
    for(i=1; i<bItemNum; i++)
        wMin = ((awSrc[i]>wMin) ? wMin:awSrc[i]);
    return wMin;
}


/**
*********************************************************************
* @brief：      求带符号的原码的绝对值
* @param[in]： dec  求值数据
* @param[out]：NA
* @return：     计算结果
*********************************************************************
*/
int sta_signed_abs_calc(int dec)
{
    int value;
    value = (dec&0x7fffffff);
    return value;
}

/**
*********************************************************************
* @brief：      获取终端或交采通信地址
* @param[in]:  pstTSA1 参与比较的通信地址1
* @param[in]:  pstTSA2 参与比较的通信地址2
* @return：     FALSE-不相等，TRUE-相等
*********************************************************************
*/
BOOL sta_tsa_equal(OOP_TSA_T *pstTSA1, OOP_TSA_T *pstTSA2)
{
    uint32 i = 0;
    
    if(pstTSA1->af != pstTSA2->af)
    {
        return FALSE;
    }

    for(i = 0; i < pstTSA1->len+1; i++)
    {
        if(pstTSA1->add[i] != pstTSA2->add[i])
        {
            return FALSE;
        }
    }

    return TRUE;
}

/**
*********************************************************************
* @brief：      获取终端或交采通信地址
* @param[out]: pstTSA 通信地址
* @return：     void
*********************************************************************
*/
int sta_addr_read(uint8     logicId, OOP_OAD_U unOAD, void *buffer, uint8 maxLen)
{
    NOMAL_OAD_T nomalOAD = {0};    
    uint32 len = 0;

    nomalOAD.infoNum = 0;
    nomalOAD.logicId = logicId;
    nomalOAD.oad.value = unOAD.value;

    return db_read_nomal(g_staUdpHandle, &nomalOAD, maxLen, (uint8*)buffer, &len);       
}


/**
*********************************************************************
* @brief：      通过通信地址获取测量点信息
* @param[in]:  pstTSA 通信地址
* @param[out]: pstMtrInfo 测量点信息
* @return：     void
*********************************************************************
*/
int sta_mtrtype_read(OOP_TSA_T *pstTSA, STA_MTR_INFO_T* pstMtrInfo)
{
    OOP_OAD_U oad;
    OOP_METER_T meter;
    int i = 0;
    uint8 *pAddr = NULL;
    uint8 len = 0; 
    OOP_OCTETVAR16_T pusleAddr = {0};
    uint8 revValue[16] = {0};

    //遍历8个脉冲计量点，判断总加类型是否为脉冲
    oad.value = 0x24010200;
    for(i = 0; i < 8; i++)
    {
        oad.nObjID = 0x2401 + i;
        
        if(sta_addr_read(0, oad, &pusleAddr, sizeof(pusleAddr)) == 0)
        {
            memcpy(revValue, pusleAddr.value, pusleAddr.nNum);
            if(memcmp(revValue, pstTSA->add, 6) == 0)
            {
                pstMtrInfo->type = MTR_PULSE;
                pstMtrInfo->pulseOAD.value = oad.value;
                return 0;
            }
        }        
    }

    //遍历485计量点，判断总加类型是否为485表
    for(i = 0; i < oopMeterList.oopMeterNum; i++)
    {
        if(app_get_meter_list_by_id(i, &meter) == 0)
        {
            if(sta_tsa_equal(&meter.basic.tsa, pstTSA))
            {
                if(meter.basic.port.nObjID == 0xf201)
                {
                    pstMtrInfo->type = MTR_485;
                    pstMtrInfo->nIndex = meter.nIndex;
                    memcpy(&pstMtrInfo->tsa, pstTSA, sizeof(OOP_TSA_T));
					return 0;
                }
            }
        }        
    }

    //终端地址与之相等，说明该测量点为交采
    len = app_get_localaddr(&pAddr);
    if(len > 0)
    {
        memcpy_rev(revValue, pAddr, len);
        
        if(memcmp(revValue, pstTSA->add, 6) == 0)
        {
            pstMtrInfo->type = MTR_AC;
            return 0;
        }
    }

    return -1;
}

/**
*********************************************************************
* @brief：      获取终端或交采通信地址
* @param[out]: pstTSA 通信地址
* @return：     void
*********************************************************************
*/
void sta_tsa_print(OOP_TSA_T *pstTSA, int (*func)(const char *,...))
{
    int i = 0;

    //+1为地址长度，+2包含头
    for(i = 0; i < pstTSA->len+2; i++)
    {
        func("%02x ",*((uint8*)pstTSA+i));
    }

    func("\n");

    return;    
}

/*******************************************************************************
* 函数名称: sta_checksum_calc
* 函数功能: 计算累加和
* 输入参数: DataLen  数据长度
*           SrcData  数据缓冲区
* 输出参数: 无
* 返 回 值: 累加和
*******************************************************************************/
uint8 sta_checksum_calc(uint8 *SrcData, int DataLen)
{
    int     i  = 0;
    uint8   CS = 0;

    for (i = 0; i < DataLen; i++)
    {
        CS += SrcData[i];
    }

    CS += 0x33;
    
    return CS;
}

/**
*********************************************************************
* @brief：      删除记录型OAD数据
* @param[in]： clientid 数据中心句柄
* @param[in]:  pNormal  普通OAD信息
* @Create    : 贺宁
* @Date      ：2020-04-15
* @return：     void 
*********************************************************************
*/
int sta_delete_record(DB_CLIENT clientid, RECORD_OAD_T *pRecord)
{
    int ret = 0;
    CLEAR_DATA_T delData;

    delData.bClassTag = FALSE;
    memcpy(&delData.recordOAD, pRecord, sizeof(RECORD_OAD_T));
    
    ret = db_clear_oad_data(clientid, &delData);
    if (ret != ERR_OK)
    {
        return DATA_READ_WRITE_DENIED;
    }

    return ret;
}

/**
*********************************************************************
* @brief：      过日脉冲数据清零
* @param[in]： tgParm  总加组参数
* @param[out]: actEnergy 有功电能
* @param[out]: reactEnergy 无功电能
* @Create    : 贺宁
* @Date      ：2020-04-15
* @return：     void 
*********************************************************************
*/
void sta_pulse_pastday_data_clear(void)
{
    int ret = ERR_OK;
    uint32 i = 0;
    NOMAL_OAD_T nomalOAD = {0};       
    OOP_ENERGY_T energy;

    MEMZERO(&energy, sizeof(energy));
    energy.nNum = 5;

    for(i = 0; i < 8; i++)
    {
        nomalOAD.oad.nObjID = 0x2401 + i;
    
        nomalOAD.oad.attID = 7;//当日正向有功电量
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day positive active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
        
            return;
        } 
    
        nomalOAD.oad.attID = 9;//当日反向有功电量
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
        
            return;
        } 
    
        nomalOAD.oad.attID = 11;//当日正向无功电量
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day positive reactive power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
        
            return;
        } 
    
        nomalOAD.oad.attID = 13;//当日反向无功电量
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day negative reactive power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
        
            return;
        } 
    
    }
    
    return;
}

/**
*********************************************************************
* @brief：      过月脉冲数据清零
* @param[in]： tgParm  总加组参数
* @param[out]: actEnergy 有功电能
* @param[out]: reactEnergy 无功电能
* @Create    : 贺宁
* @Date      ：2020-04-15
* @return：     void 
*********************************************************************
*/
void sta_pulse_pastmon_data_clear(void)
{
    int ret = ERR_OK;
    uint32 i = 0;
    NOMAL_OAD_T nomalOAD = {0};       
    OOP_ENERGY_T energy;

    MEMZERO(&energy, sizeof(energy));
    energy.nNum = 5;

    for(i = 0; i < 8; i++)
    {
        nomalOAD.oad.nObjID = 0x2401 + i;
    
        nomalOAD.oad.attID = 8;//当月正向有功电量
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("month positive active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
        
            return;
        } 
    
        nomalOAD.oad.attID = 10;//当月反向有功电量
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("month negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
        
            return;
        } 
    
        nomalOAD.oad.attID = 12;//当月正向无功电量
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("month positive reactive power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
        
            return;
        } 
    
        nomalOAD.oad.attID = 14;//当月反向无功电量
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("month negative reactive power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
        
            return;
        }     
    }
    
    return;
}

/**
*********************************************************************
* @brief：      过日总加组数据清零
* @param[in]： tgParm  总加组参数
* @param[out]: actEnergy 有功电能
* @param[out]: reactEnergy 无功电能
* @Create    : 贺宁
* @Date      ：2020-04-15
* @return：     void 
*********************************************************************
*/
void sta_tgsets_pastday_data_clear(void)
{
    int ret = ERR_OK;
    uint32 i = 0;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_HENERGYS_T henergys;

    MEMZERO(&henergys, sizeof(henergys));
    henergys.nNum = 5;

    for(i = 0; i < 8; i++)
    {
        nomalOAD.oad.nObjID = 0x2301 + i;
    
        nomalOAD.oad.attID = 7;//日有功电量
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&henergys, sizeof(OOP_HENERGYS_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day positive active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
        
            return;
        } 
    
        nomalOAD.oad.attID = 8;//日无功电量
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&henergys, sizeof(OOP_HENERGYS_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
        
            return;
        } 
    }
    
    return;
}

/**
*********************************************************************
* @brief：      过月总加组数据清零
* @param[in]： tgParm  总加组参数
* @param[out]: actEnergy 有功电能
* @param[out]: reactEnergy 无功电能
* @Create    : 贺宁
* @Date      ：2020-04-15
* @return：     void 
*********************************************************************
*/
void sta_tgsets_pastmon_data_clear(void)
{
    int ret = ERR_OK;
    uint32 i = 0;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_HENERGYS_T henergys;

    MEMZERO(&henergys, sizeof(henergys));
    henergys.nNum = 5;

    for(i = 0; i < 8; i++)
    {
        nomalOAD.oad.nObjID = 0x2301 + i;
    
        nomalOAD.oad.attID = 9;//月有功电量
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&henergys, sizeof(OOP_HENERGYS_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day positive active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
        
            return;
        } 
    
        nomalOAD.oad.attID = 10;//月无功电量
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&henergys, sizeof(OOP_HENERGYS_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
        
            return;
        } 
    }
    
    return;
}

/**********************************************************************
* @name      : is_total_del_finish
* @brief     ：上次是否处理结束
* @param[in] ：NULL
* @return    ：TRUE-结束 FALSE-没有
**********************************************************************/
BOOL is_total_del_finish(void)
{
    BOOL flag = FALSE;
    
    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&g_tgdel.flock);

    //读清模式
    if (g_tgdel.delVaild != 0)
    {
        flag = FALSE;
    }
    else
    {
        flag = TRUE;
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&g_tgdel.flock);
    
    return flag;
}

/**********************************************************************
* @name      : set_total_del_flag
* @brief     ：设置删除标记
* @param[in] ：flag ---- 数据删除标识
* @return    ：NULL
**********************************************************************/
void set_total_del_flag(uint8 delVaild)
{
//    int i = 0;
//    //等待上次删除标记处理结束
//    for (i = 0; i < 30; i++)
//    {
//        if (is_total_del_finish() == FALSE)
//        {
//            usleep(100000);
//        }
//        else
//        {
//            break;
//        }
//    }
//    if (i == 30)
//    {
//        PRTL_FMT_DEBUG("上次总加数据删除标记处理超时\n");
//        return;
//    }
    
    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&g_tgdel.flock);

    g_tgdel.delVaild |= delVaild;
    
    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&g_tgdel.flock);

    return;
}

/**********************************************************************
* @name      : get_total_del_flag
* @brief     ：获取删除数据标识
* @param[in] ：NULL
* @return    ：flag ---- 数据删除标识
**********************************************************************/
uint8 get_total_del_flag(void)
{
    uint8 delVaild = 0;
    
    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&g_tgdel.flock);

    //读清模式
    delVaild = g_tgdel.delVaild;
    if (delVaild != 0)
    {
        g_tgdel.delVaild = 0;
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&g_tgdel.flock);
    
    return delVaild;
}

/**
*********************************************************************
* @brief：      过日过月的数据清零
* @param[in]： tgParm  总加组参数
* @param[out]: actEnergy 有功电能
* @param[out]: reactEnergy 无功电能
* @Create    : 贺宁
* @Date      ：2020-04-15
* @return：     void 
*********************************************************************
*/
void sta_past_data_clear(void)
{
    int i = 0;
    OOP_OAD_U tgOAD = {0};
    uint8 tgcflg=0;

    if(pastdayflag)
    {
        STAT_FMT_DEBUG("past day clear pulse and tgsets data.\n");
        sta_pulse_pastday_data_clear();
        sta_tgsets_pastday_data_clear();
    }

    if(pastmonflag)
    {
        STAT_FMT_DEBUG("past mon clear pulse and tgsets data.\n");
        sta_pulse_pastmon_data_clear();
        sta_tgsets_pastmon_data_clear();
    }

    tgcflg = get_total_del_flag();

    //总加组配置变更清理老数据
    for(i = 0; i < 8; i++)
    {
        if(tgcflg&(1UL<<i))
        {
            tgOAD.nObjID = 0x2301+i;
            sta_tg_data_clear(g_staUdpHandle, tgOAD);
        }
    }
}

#endif

#if DESC("终端端口授权",1)

/**
*********************************************************************
* @brief：      SSH服务授权操作
* @param[in]： STA_PORT_AUTHORIZE_E status 服务授权状态
* @return：     void
*********************************************************************
*/


/**
*********************************************************************
* @brief：      SSH服务授权操作
* @param[in]： STA_PORT_AUTHORIZE_E status 服务授权状态
* @return：     void
*********************************************************************
*/
int sta_ssh_service_authorize_operate(STA_PORT_AUTHORIZE_E status)
{
    int ret = ERR_OK;
    uint8 msgdata[1400] = {0};
    uint16 msglen=0;

    if(status == STA_PORT_AUTH_FORBIDDEN)
    {
        msgdata[msglen++] = 1; //关闭SSH授权
        AUTH_FMT_DEBUG("SSH service close.\n");
    }
    else
    {
        msgdata[msglen++] = 0; //开启SSH授权
        AUTH_FMT_DEBUG("SSH service open.\n");
    }
    
    ret = appmain_send_msg(MSG_APPAMIN_SSH_PORT_AUTHORIZE, 0, msgdata, msglen);
    if(ret != ERR_OK)
    {
        AUTH_FMT_DEBUG("appmain_send_msg failed. ret(%d)\n",ret);
        return ret;
    }

    return ret;
}

/**
*********************************************************************
* @brief：      终端端口授权状态
* @param[in]： NA
* @return：     void
*********************************************************************
*/
void sta_tmn_port_authorize_status(uint8 tmAuth, STA_PORT_AUTHORIZE_INFO *authInfo)
{
    authInfo->authTime = tmAuth;
    authInfo->status = AUTH_STATUS_GET(authInfo->authTime);
}

/**
*********************************************************************
* @brief：      终端端口授权
* @param[in]： NA
* @return：     void
*********************************************************************
*/
void sta_tmn_port_authorize_tmcnt(void)
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomal = {0};
    OOP_PORTS_T data;
    OOP_SERVICES_T serData;

    MEMZERO(&data, sizeof(data));
    MEMZERO(&serData, sizeof(serData));
    memcpy(&serData, &g_serData, sizeof(serData));
    
    nomal.classtag = CLASS_DATA_UNINIT;

    //授权时间为分钟数，每过分减去1,只有临时授权才刷新分钟数，永久授权和关闭授权均不需要
    if((g_portSSH.authTime > 0) && (g_portSSH.status == STA_PORT_AUTH_PROVISIONAL))
    {
        g_portSSH.authTime--;
    
        serData.item[0].times = g_portSSH.authTime;

        nomal.oad.value = 0x43000B00;
        //重新写入
        ret = db_write_nomal(g_staUdpHandle, &nomal, (uint8*)&serData, sizeof(serData));
        if(ret != ERR_OK)
        {
            AUTH_FMT_DEBUG("tmn_port_authorize write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(serData));
            return;
        }
    } 
    
    if((g_portRS232.authTime > 0) && (g_portRS232.status == STA_PORT_AUTH_PROVISIONAL))
    {
        g_portRS232.authTime--;
    
        data.item[0] = g_portRS232.authTime;

        nomal.oad.value = 0xF2000500;
        //重新写入
        ret = db_write_nomal(g_staUdpHandle, &nomal, (uint8*)&data, sizeof(data));
        if(ret != ERR_OK)
        {
            AUTH_FMT_DEBUG("tmn_port_authorize write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(data));
            return;
        }
    }

    if((g_portRS485.authTime > 0) && (g_portRS485.status == STA_PORT_AUTH_PROVISIONAL))
    {
        g_portRS485.authTime--;
    
        data.item[0] = g_portRS485.authTime;

        nomal.oad.value = 0xF2010500;
        //重新写入
        ret = db_write_nomal(g_staUdpHandle, &nomal, (uint8*)&data, sizeof(data));
        if(ret != ERR_OK)
        {
            AUTH_FMT_DEBUG("tmn_port_authorize write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(data));
            return;
        }        
    }
        
    if((g_portIR.authTime > 0) && (g_portIR.status == STA_PORT_AUTH_PROVISIONAL))
    {
        g_portIR.authTime--;
    
        data.item[0] = g_portIR.authTime;

        nomal.oad.value = 0xF2020500;
        //重新写入
        ret = db_write_nomal(g_staUdpHandle, &nomal, (uint8*)&data, sizeof(data));
        if(ret != ERR_OK)
        {
            AUTH_FMT_DEBUG("tmn_port_authorize write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(data));
            return;
        }       
    } 

    if((g_portUSB.authTime > 0) && (g_portUSB.status == STA_PORT_AUTH_PROVISIONAL))
    {
        g_portUSB.authTime--;
    
        data.item[0] = g_portUSB.authTime;


        nomal.oad.value = 0xF20E0500;
        //重新写入
        ret = db_write_nomal(g_staUdpHandle, &nomal, (uint8*)&data, sizeof(data));
        if(ret != ERR_OK)
        {
            AUTH_FMT_DEBUG("tmn_port_authorize write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(data));
            return;
        }        
    }  

    return;
}

/**
*********************************************************************
* @brief：      终端端口授权
* @param[in]： NA
* @return：     void
*********************************************************************
*/
void sta_tmn_port_authorize_operate(void)
{
    int ret = ERR_OK;
    uint8 msgdata[1400] = {0};
    uint16 msglen=0;

    if(g_portRS232.isAuthChg == TRUE)
    {
        g_portRS232.isAuthChg = FALSE;
            
        if(g_portRS232.status == STA_PORT_AUTH_FORBIDDEN)
        {
            //TODO 关闭授权
        }
        else
        {
            //TODO 开启授权
        }
    }

    if(g_portRS485.isAuthChg == TRUE)
    {
        g_portRS485.isAuthChg = FALSE;
    
        if(g_portRS485.status == STA_PORT_AUTH_FORBIDDEN)
        {
            //TODO 关闭授权
        }
        else
        {
            //TODO 开启授权
        }        
    }

    if(g_portIR.isAuthChg == TRUE)
    {
        g_portIR.isAuthChg = FALSE;
            
        if(g_portIR.status == STA_PORT_AUTH_FORBIDDEN)
        {
            //TODO 关闭授权
        }
        else
        {
            //TODO 开启授权
        }               
    }

    if(g_portUSB.isAuthChg == TRUE)
    {
        g_portUSB.isAuthChg = FALSE;
    
        //关闭授权时U盘是插入状态，则立即发送消息
        if((g_portUSB.status == STA_PORT_AUTH_FORBIDDEN)&&(g_udisk_status == 1))
        {
            msglen = 0;
            msgdata[msglen++] = 2; 
            EVENT_FMT_DEBUG("USB disk exist.\n");
            CheckERC_3140(g_staUdpHandle, MSG_APPAMIN_UDISK_AUTHORIZE, msgdata, msglen);
        }
    }

    if(g_portSSH.isAuthChg == TRUE)
    {                
        if(g_portSSH.status == STA_PORT_AUTH_FORBIDDEN)
        {
            msglen = 0;
            msgdata[msglen++] = 1; //关闭SSH授权
            
            AUTH_BUF_DEBUG(msgdata, msglen, "SSH service close.\n");

            ret = appmain_send_msg(MSG_APPAMIN_SERVICE_STOP, 0, msgdata, msglen);
            if(ret != ERR_OK)
            {
                AUTH_FMT_DEBUG("auth msg send failed. ret(%d)\n",ret);
                return;
            }
        }
//        else
//        {
//            msgdata[msglen++] = 0; //开启SSH授权
//            AUTH_FMT_DEBUG("SSH service open.\n");           
//        }

        g_portSSH.isAuthChg = FALSE;
    }

    if(g_portSFTP.isAuthChg == TRUE)
    {                
        if(g_portSFTP.status == STA_PORT_AUTH_FORBIDDEN)
        {
            msglen = 0;
            msgdata[msglen++] = 2; //关闭SSH授权
            
            AUTH_BUF_DEBUG(msgdata, msglen, "SFTP service close.\n");

            ret = appmain_send_msg(MSG_APPAMIN_SERVICE_STOP, 0, msgdata, msglen);
            if(ret != ERR_OK)
            {
                AUTH_FMT_DEBUG("auth msg send failed. ret(%d)\n",ret);
                return;
            }
        }
//        else
//        {
//            msgdata[msglen++] = 0; //开启SSH授权
//            AUTH_FMT_DEBUG("SSH service open.\n");           
//        }

        g_portSFTP.isAuthChg = FALSE;
    }
}

#endif

#if DESC("电压合格率计算",1)

/**
*********************************************************************
* @brief：      计算两次统计的时间差,结果为计算电压合格率使用，日月统计同时计算
* @param[out]：alterTime 两次统计的时间增加量, 2个元素，0-日，1-月
* @return：     void
*********************************************************************
*/
void sta_volt_monitor_time(int *alterTime)
{
    time_t tmpTime = 0;
    time_t tmpLstTime = 0;
    int  addTime = 0;     // 变化时间 
    int  detaTime[2] = {0}; // 2个元素，0-日，1-月,因过日月时计算不同，不能合一
    static BOOLEAN firstRun = 1;

    //这里曾经是ramdisk，可以用全局或static变量等价替换
    DT_DateTimeShort2Time(&realstattime, &tmpTime);
    DT_DateTimeShort2Time(&laststattime, &tmpLstTime);

    //计算两次统计间隔的分钟数,上次时间忽略掉非整分秒数
    addTime = (int)(tmpTime -(tmpLstTime/60)*60)/60;

    // 复位肯定是在1分钟内完成，有终端复位的情况不能丢失合格时间
    if(firstRun)
    {
        if (addTime > 1)
        {
            return;
        }

        firstRun = 0;
    }

    STAT_FMT_DEBUG("volt monitor curTime(%d) - lstTime(%d) = (%d)s ~ (%d)min.\n", tmpTime, tmpLstTime, tmpTime-tmpLstTime, addTime);

    /* 往回对时第一次不计算时间 */
    if (addTime < 0)
    {
        return;
    }

    /*上面处理过程日和月时间一致*/
    detaTime[STA_CALC_DAY] = addTime;
    detaTime[STA_CALC_MON] = addTime;

    /* 非正常 非0时0分的过日，对时过日*/
    if ((pastdayflag) && ((realstattime.hour != 0x00) || (realstattime.minute != 0x00)))
    {
        STAT_FMT_DEBUG("past day by datetime update.\n");
        // 经过了这一天,到此刻的越限时间全算
        detaTime[STA_CALC_DAY] = realstattime.hour * 60 + realstattime.minute;
    }

    /* 非正常 非1日0时0分的过月，对时过月*/
    if ((pastmonflag) && ((realstattime.day != 0x01) || (realstattime.hour != 0x00) || (realstattime.minute != 0x00)))
    {
        STAT_FMT_DEBUG("past month by datetime update.\n");
        // 经过了这一月,到此刻的越限时间全算
        detaTime[STA_CALC_MON] = (realstattime.day - 1) * 1440 + realstattime.hour * 60 + realstattime.minute;
    }

    memcpy(alterTime, detaTime, sizeof(detaTime));
}

/**
*********************************************************************
* @brief：      读取电压越限参数
* @param[out]：olimTime 越限时间数组
* @param[out]：qTime 合格时间数组
* @return：     void
*********************************************************************
*/
int sta_volt_param_read(OOP_VOLTPARAM_T *param)
{
    int ret = 0;
    NOMAL_OAD_T nomalOAD = {0};
    uint32 len = 0;
    uint32 inMaxLen = sizeof(OOP_VOLTPARAM_T);
    OOP_OAD_U voltpOAD = {0x40300200};

    nomalOAD.logicId = 0;
    memcpy(&nomalOAD.oad, &voltpOAD, sizeof(OOP_OAD_U));

    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, inMaxLen, (uint8*)param, &len);
    if(ret != 0 )
    {
        STAT_FMT_DEBUG("volt limit paramter read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, inMaxLen);
        return ret;
    }

    return ret;
}

/**
*********************************************************************
* @brief：      计算电压越上限时间累积值
* @param[in]： voltAccCfg 累加配置参数
* @param[out]：accTime  越限累积时间数组
* @param[out]：isQuality 是否合格
* @return：     void
*********************************************************************
*/
void sta_volt_uptime_calc(STA_VOLT_ACCTIME_CFG_T         voltAccCfg, uint32 i, uint16 *accTime, uint8 *isQuality)
{
    uint16 volt[3] = {0};
    uint16 acctime[12] = {0};
    OOP_WORD3V_T phaseVolt;   //分相电压
    OOP_VOLTPARAM_T voltParam; //电压合格率参数
    uint16 addTime;            //到此次统计时累积时间  
    uint8 upflag = 1;
    uint8 upupflag = 1;

    phaseVolt = voltAccCfg.phaseVolt;
    voltParam = voltAccCfg.voltParam;
    addTime = voltAccCfg.addTime;
    
    memcpy(volt, phaseVolt.nValue, phaseVolt.nNum*sizeof(uint16));
    memcpy(acctime, accTime, sizeof(acctime));

    //TODO 判读越限参数的有效性，据此设置upflag，upupflag,或在voltAccCfg中带下

    //计算电压越上限累积时间   
    if(upflag && !upupflag) 
    {
        if(volt[i] >= voltParam.upVolt)
        {
            acctime[i] += addTime;
            *isQuality = 0;
        }

    }
    else if(upflag && upupflag)                 
    {
        if(volt[i] >= voltParam.upVolt && volt[i] <voltParam.uupVolt) 
        {
            acctime[i] += addTime;
            *isQuality = 0;
        }
    }

    memcpy(accTime, acctime, sizeof(acctime));    
}

/**
*********************************************************************
* @brief：      计算电压越上上限时间累积值
* @param[in]： voltAccCfg 累加配置参数
* @param[out]：accTime  越限累积时间数组
* @param[out]：isQuality 是否合格
* @return：     void
*********************************************************************
*/
void sta_volt_uuptime_calc(STA_VOLT_ACCTIME_CFG_T         voltAccCfg, uint32 i, uint16 *accTime, uint8 *isQuality)
{
    uint16 volt[3] = {0};
    uint16 acctime[12] = {0};
    OOP_WORD3V_T phaseVolt;   //分相电压
    OOP_VOLTPARAM_T voltParam; //电压合格率参数
    uint16 addTime;            //到此次统计时累积时间  
    uint8 upupflag = 1;
    static  uint16 upupTime[3] = {0};  //越上上限累计时间
    uint8   upupLmtTime = 0;        //上上限持续时间参数
    uint16  upupRate = 0;           //上上限返回系数

    phaseVolt = voltAccCfg.phaseVolt;
    voltParam = voltAccCfg.voltParam;
    addTime = voltAccCfg.addTime;
    
    memcpy(volt, phaseVolt.nValue, phaseVolt.nNum*sizeof(uint16));
    memcpy(acctime, accTime, sizeof(acctime));

    //TODO 判读越限参数的有效性，据此设置upflag，upupflag,或在voltAccCfg中带下

    /* 计算电压越上上限累积时间 TODO (1-upupRate)计算不对，都是整数
       upupRate要设默认值，默认值可配么，不可配考虑用宏吧。 
    */
    if(upupflag)
    {
        if(upupTime[i] > 0)      //出现了越上上限
        {
            if(volt[i] > (voltParam.uupVolt * (1-upupRate))) //没有小于恢复值此时合格时间也不计
            {
                *isQuality = 0;
            }
        }
    
        if(volt[i] >= voltParam.uupVolt)
        {
            if(voltAccCfg.interval == STA_CALC_DAY)//只写一次日的,防止月统计时再写一次
            {
                upupTime[i] += addTime;
            }
    
            if(upupTime[i] >= upupLmtTime)
            {
            #if UPUP_EN == 1
                acctime[i] += addTime;
            #endif
                acctime[i+6] += addTime;
            }
            *isQuality = 0;
        }
        else
        {
            upupTime[i] = 0;
        }
    }

    memcpy(accTime, acctime, sizeof(acctime));    
}

/**
*********************************************************************
* @brief：      计算电压越下限时间累积值
* @param[in]： voltAccCfg 累加配置参数
* @param[out]：accTime  越限累积时间数组
* @param[out]：isQuality 是否合格
* @return：     void
*********************************************************************
*/
void sta_volt_lowtime_calc(STA_VOLT_ACCTIME_CFG_T         voltAccCfg, uint32 i, uint16 *accTime, uint8 *isQuality)
{
    uint16 volt[3] = {0};
    uint16 acctime[12] = {0};
    OOP_WORD3V_T phaseVolt;   //分相电压
    OOP_VOLTPARAM_T voltParam; //电压合格率参数
    uint16 addTime;            //到此次统计时累积时间  
    uint8 dwflag = 1;
    uint8 dwdwflag = 1;

    phaseVolt = voltAccCfg.phaseVolt;
    voltParam = voltAccCfg.voltParam;
    addTime = voltAccCfg.addTime;
    
    memcpy(volt, phaseVolt.nValue, phaseVolt.nNum*sizeof(uint16));
    memcpy(acctime, accTime, sizeof(acctime));

    //TODO 判读越限参数的有效性，据此设置dwflag，dwdwflag,或在voltAccCfg中带下

    //计算电压越下限累积时间
    if(dwflag && !dwdwflag) //不统计下下限,只统计下限
    {
        if(volt[i] <= voltParam.ldVolt)
        {
            acctime[i+3] += addTime;
            *isQuality = 0;
        }
    }
    else if(dwflag && dwdwflag)
    {
        if(volt[i] <= voltParam.ldVolt && volt[i] > voltParam.lldVolt) //小于下限大于下下限
        {
            acctime[i+3] += addTime;
            *isQuality = 0;
        }
    }

    memcpy(accTime, acctime, sizeof(acctime));    
}

/**
*********************************************************************
* @brief：      计算电压越下下限时间累积值
* @param[in]： voltAccCfg 累加配置参数
* @param[out]：accTime  越限累积时间数组
* @param[out]：isQuality 是否合格
* @return：     void
*********************************************************************
*/
void sta_volt_llowtime_calc(STA_VOLT_ACCTIME_CFG_T         voltAccCfg, uint32 i, uint16 *accTime, uint8 *isQuality)
{
    uint16 volt[3] = {0};
    uint16 acctime[12] = {0};
    OOP_WORD3V_T phaseVolt;   //分相电压
    OOP_VOLTPARAM_T voltParam; //电压合格率参数
    uint16 addTime;            //到此次统计时累积时间  
    uint8 dwdwflag = 1;
    static  uint16 dwdwTime[3] = {0};  //越下下限累计时间
    uint8   dwdwLmtTime = 0;        //下下限持续时间参数
    uint16  dwdwRate = 0;           //下下限返回系数

    phaseVolt = voltAccCfg.phaseVolt;
    voltParam = voltAccCfg.voltParam;
    addTime = voltAccCfg.addTime;
    
    memcpy(volt, phaseVolt.nValue, phaseVolt.nNum*sizeof(uint16));
    memcpy(acctime, accTime, sizeof(acctime));

    //TODO 判读越限参数的有效性，据此设置upflag，upupflag,或在voltAccCfg中带下

    /* 计算电压越下下限累积时间 TODO (1-upupRate)计算不对，都是整数
       upupRate要设默认值，默认值可配么，不可配考虑用宏吧。 */
    if(dwdwflag)
    {
        if(dwdwTime[i] > 0)      //出现了越下下限
        {
            if(volt[i] < (voltParam.lldVolt * (1+dwdwRate))) //没有大于下下限恢复值此时合格时间也不计
            {
                *isQuality = 0;
            }
        }
    
        if(volt[i] <= voltParam.lldVolt)
        {
            if(voltAccCfg.interval == STA_CALC_DAY)//只写一次日的,防止月统计时再写一次
            {
                dwdwTime[i] += addTime;
            }
    
            if(dwdwTime[i] >= dwdwLmtTime)
            {
            #if UPUP_EN == 1
                acctime[i+3] += addTime;
            #endif
                acctime[i+9] += addTime;
            }
            *isQuality = 0;
        }
        else
        {
            dwdwTime[i] = 0;
        }
    }

    memcpy(accTime, acctime, sizeof(acctime));    
}

/**
*********************************************************************
* @brief：      判断越限并计算越限时间累积值
* @param[in]： voltAccCfg 累加配置参数
* @param[out]：accTime  越限累积时间数组
* @param[out]：isQuality 是否合格
* @return：     void
*********************************************************************
*/
void sta_volt_overlim_judge(STA_VOLT_ACCTIME_CFG_T         voltAccCfg, uint32 i, uint16 *accTime, uint8 *isQuality)
{
  //TODO 判读越限参数的有效性，在voltAccCfg中带下

   //先做上上限判断
   sta_volt_uuptime_calc(voltAccCfg, i, accTime, isQuality);

   //上上限未越限才做上限判断
   if(*isQuality)
   {      
        sta_volt_uptime_calc(voltAccCfg, i, accTime, isQuality);

        //上限未越限才做下限判断
        if(*isQuality)
        {
            sta_volt_lowtime_calc(voltAccCfg, i, accTime, isQuality);
        
            //下限未越限才做下下限判断
            if(*isQuality)
            {
                sta_volt_llowtime_calc(voltAccCfg, i, accTime, isQuality);   
            }
        }
   }

   return;
}

/**
*********************************************************************
* @brief：      电压越限和合格时间累加
* @param[in]： voltAccCfg 累加配置参数
* @param[out]：accTime  越限累积时间数组
* @param[out]：quaTime 合格累积时间数组
* @return：     void
*********************************************************************
*/
void sta_volt_quaolimtime_acc(STA_VOLT_ACCTIME_CFG_T voltAccCfg, uint16 *accTime, uint16 *quaTime)
{
    uint16 volt[3] = {0};
    uint16 acctime[12] = {0};//上限(A,B,C),下限(A,B,C),上上限(A,B,C),下下限(A,B,C)
    uint32 i;
    uint8 isQuality;
    OOP_WORD3V_T phaseVolt;   //分相电压
    uint16 addTime;            //到此次统计时累积时间 
    uint8 powerType = 0;

    phaseVolt = voltAccCfg.phaseVolt;
    addTime = voltAccCfg.addTime;
    powerType = voltAccCfg.powerType;
    
    memcpy(volt, phaseVolt.nValue, phaseVolt.nNum*sizeof(uint16));
    memcpy(acctime, accTime, sizeof(acctime));

    STAT_FMT_DEBUG("calcType(%d) addTime(%d), voltParam value: up(%d), upup(%d), dw(%d), dwdw(%d)\n", 
        voltAccCfg.interval, voltAccCfg.addTime, voltAccCfg.voltParam.upVolt, voltAccCfg.voltParam.uupVolt, voltAccCfg.voltParam.ldVolt, voltAccCfg.voltParam.lldVolt);

    for(i = 0; i < 3; i++)
    {
        STAT_FMT_DEBUG("volt[%d] over limit time last: qualify(%d), up(%d), dw(%d), upup(%d), dwdw(%d)\n", 
            i, quaTime[i], acctime[i], acctime[i+3], acctime[i+6], acctime[i+9]);

        STAT_FMT_DEBUG("volt[%d] rt value(%d)\n", i, volt[i]); 
    }
    
    if(volt[0] == 0 && volt[1] == 0 && volt[2] == 0)//TODO 停电不判
    {
        return;
    }

    for(i = 0; i < 12; i++)
    {
        if(acctime[i] == NONEWORD)
        {
            acctime[i] = 0;
        }
    }

    for(i = 0; i < 3; i++)
    {
        //三相三线，B相不统计
        if((i == 1) && (powerType == PWRTYPE_3PHASE3)) continue;
        
        if(volt[i] == NONEWORD) continue;

        //默认电压合格
        isQuality = 1;

        sta_volt_overlim_judge(voltAccCfg, i, acctime, &isQuality);

        //经过判断后没有任何越限，则这段时间电压合格
        if(isQuality == 1)
        {
            quaTime[i]  += addTime;
        }

        // 日数据容错（日数据有错更明显）
        if (voltAccCfg.interval == STA_CALC_DAY)
        {
            if (acctime[i+0] > 1440)
                acctime[i+0] = 1440;
            if (acctime[i+3] > 1440)
                acctime[i+3] = 1440;
            if (acctime[i+6] > 1440)
                acctime[i+6] = 1440;
            if (acctime[i+9] > 1440)
                acctime[i+9] = 1440;
        }
    }

    //三相三线，B相直接合格
    if (powerType == PWRTYPE_3PHASE3)
    {
        quaTime[1] += addTime;
    }

    memcpy(accTime, acctime, sizeof(acctime));

    return;
}

/**
*********************************************************************
* @brief：      日电压合格率计算容错
* @param[in]： voltAccCfg 累加配置参数
* @param[in]： accTime  越限累积时间数组
* @param[out]：quaTime   合格累加时间数组
* @return：     void
*********************************************************************
*/
void sta_volt_fault_tolerant(const uint16 *accTime, uint16 *quaTime)
{
    uint16 acctime[12] = {0};
    uint16 extTime[3] = {0};
    uint8 i = 0;

    memcpy(acctime, accTime, sizeof(acctime));
    memcpy(extTime, quaTime, sizeof(extTime));

    for(i=0;i<3;i++)// 2013 COMMON BUG 
    {
        /*1440分钟是24小时*/
        if(extTime[i] > 1440)  
        {
            extTime[i] = 1440;  
        }
    }

    if(pastdayflag)
    {
        #if UPUP_EN == 1
            if((acctime[0]+acctime[3])<1440)
                extTime[0] = 1440-(acctime[0]+acctime[3]);
            else
                extTime[0] = 0;
        #else
            if((acctime[0]+acctime[3]+acctime[6]+acctime[9])<1440)
                extTime[0] = 1440-(acctime[0]+acctime[3]+acctime[6]+acctime[9]);
            else
                extTime[0] = 0;
        #endif

        #if UPUP_EN == 1
            if((acctime[1]+acctime[4])<1440)
                extTime[1] = 1440-(acctime[1]+acctime[4]);
            else
                extTime[1] = 0;
        #else
            if((acctime[1]+acctime[4]+acctime[7]+acctime[10])<1440)
                extTime[1] = 1440-(acctime[1]+acctime[4]+acctime[7]+acctime[10]);
            else
                extTime[1] = 0;
        #endif

        #if UPUP_EN == 1
            if((acctime[2]+acctime[5])<1440)
                extTime[2] = 1440-(acctime[2]+acctime[5]);
            else
                extTime[2] = 0;
        #else
            if((acctime[2]+acctime[5]+acctime[8]+acctime[11])<1440)
                extTime[2] = 1440-(acctime[2]+acctime[5]+acctime[8]+acctime[11]);
            else
                extTime[2] = 0;
        #endif                
    }    
    
    memcpy(quaTime, extTime, sizeof(extTime));
}

/**
*********************************************************************
* @brief：      统计电压越限累计时间,结果为计算电压合格率使用，日月统计同时计算
* @param[in]： cVolt 当前电压
* @param[out]：qolimTime 合格及越限累积时间数组
* @return：     void
*********************************************************************
*/
int sta_volt_qualifytime_calc(uint16 qolimTimeDay[5][3], uint16 qolimTimeMon[5][3])
{
    int ret = ERR_OK;;
    NOMAL_OAD_T nomalOAD = {0};
    uint32 len = 0;
    STA_VOLT_ACCTIME_CFG_T  voltAccCfg;
    static uint16 olimTime[STA_CALC_MAX][12] = {{0}}; /*用于存储上一次电压越限时间*/
    static uint16 qrateTime[STA_CALC_MAX][3] = {{0}}; /*用于存储上一次电压合格时间*/   
    uint16  acctime[STA_CALC_MAX][12] = {{0}};//三相，每相有越上限，越上上限，越下限，越下下限
    uint16  extTime[STA_CALC_MAX][3] = {{0}};       //电压合格时间,三相   
    int alterTime[STA_CALC_MAX] = {0};    //2个元素，0-日，1-月
    
    //读取电压越限参数
    ret = sta_volt_param_read(&voltAccCfg.voltParam);
    if(ret != 0)
    {
        return ret;
    }

    /* 读取已经记录的越限累积时间（即上次计算的）*/ //TODO 需要存到文件，防止掉电
    memcpy(acctime, olimTime, sizeof(acctime));
    memcpy(extTime, qrateTime, sizeof(extTime));

    /* 时间差计算，电压监测时间 2个元素，0-日，1-月*/
    sta_volt_monitor_time(alterTime);

    //读取交采实时电压
    voltAccCfg.phaseVolt.nNum = 3;
    voltAccCfg.phaseVolt.nValue[0] = g_acData.PhaAVolt;
    voltAccCfg.phaseVolt.nValue[1] = g_acData.PhaBVolt;
    voltAccCfg.phaseVolt.nValue[2] = g_acData.PhaCVolt;

    //交采参数，逻辑设备号为1
    nomalOAD.logicId = 1;
    nomalOAD.oad.value = 0x40100200;

    //交采接线方式
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(uint16), &voltAccCfg.powerType, &len);
    if((ret != 0) || (voltAccCfg.powerType == PWRTYPE_UNKNOWN))
    {
        STAT_FMT_DEBUG("power type read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d), powerType(%d), use default(PWRTYPE_3PHASE4).\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint16), voltAccCfg.powerType);
        voltAccCfg.powerType = PWRTYPE_3PHASE4;
        ret = ERR_OK;
    }

    voltAccCfg.interval = STA_CALC_DAY;
    voltAccCfg.addTime = alterTime[STA_CALC_DAY];

    //计算越限与合格累积时间
    sta_volt_quaolimtime_acc(voltAccCfg, acctime[STA_CALC_DAY], extTime[STA_CALC_DAY]);

    voltAccCfg.interval = STA_CALC_MON;
    voltAccCfg.addTime = alterTime[STA_CALC_MON];

    //计算月越限与合格月累积时间
    sta_volt_quaolimtime_acc(voltAccCfg, acctime[STA_CALC_MON], extTime[STA_CALC_MON]);

    /*写越限累积时间*/
    memcpy(olimTime, acctime, sizeof(olimTime));

    //日数据做容错处理
    sta_volt_fault_tolerant(acctime[STA_CALC_DAY], extTime[STA_CALC_DAY]);

    /*写合格累积时间*/
    memcpy(qrateTime, extTime,sizeof(qrateTime));

    memcpy(qolimTimeDay[0],qrateTime[STA_CALC_DAY], sizeof(qolimTimeDay[0])); //合格
    memcpy(qolimTimeDay[1],olimTime[STA_CALC_DAY],   sizeof(qolimTimeDay[1]));//越上限   
    memcpy(qolimTimeDay[2],olimTime[STA_CALC_DAY]+3, sizeof(qolimTimeDay[2]));//越下限 
    memcpy(qolimTimeDay[3],olimTime[STA_CALC_DAY]+6, sizeof(qolimTimeDay[3]));//越上上限   
    memcpy(qolimTimeDay[4],olimTime[STA_CALC_DAY]+9, sizeof(qolimTimeDay[4]));//越下下限
    
    memcpy(qolimTimeMon[0],qrateTime[STA_CALC_MON], sizeof(qolimTimeMon[0])); //合格
    memcpy(qolimTimeMon[1],olimTime[STA_CALC_MON],   sizeof(qolimTimeMon[1]));//越上限   
    memcpy(qolimTimeMon[2],olimTime[STA_CALC_MON]+3, sizeof(qolimTimeMon[2]));//越下限 
    memcpy(qolimTimeMon[3],olimTime[STA_CALC_MON]+6, sizeof(qolimTimeMon[3]));//越上上限   
    memcpy(qolimTimeMon[4],olimTime[STA_CALC_MON]+9, sizeof(qolimTimeMon[4]));//越下下限  
    
    //过日或过月清当日数据,前面的数据已经写到对应OAD的，后面用从数据中心取
    if(pastdayflag)
    {
        STAT_FMT_DEBUG("past day clear qualify time.\n");
        memset(olimTime[STA_CALC_DAY], 0x00, sizeof(olimTime[STA_CALC_DAY]));
        memset(qrateTime[STA_CALC_DAY], 0x00, sizeof(qrateTime[STA_CALC_DAY]));
    }
    
    if(pastmonflag)
    {
        STAT_FMT_DEBUG("past month clear qualify time.\n");
        memset(olimTime[STA_CALC_MON], 0x00, sizeof(olimTime[STA_CALC_MON]));
        memset(qrateTime[STA_CALC_MON], 0x00, sizeof(qrateTime[STA_CALC_MON]));
    }   

    return ret;
}

/**
*********************************************************************
* @brief：      检查电压合格率是日还是月
* @param[in]： cVolt 当前电压
* @param[in]： qRate 合格率
* @return：     void
*********************************************************************
*/
void sta_volt_qualifyobjs_check(OOP_OAD_U qRate, STA_CALC_INTERVAL_E *interval)
{
    *interval = STA_CALC_MON;

    if(qRate.nIndex == 1)
    {
        *interval  = STA_CALC_DAY;
    }

    return;
}

/**
*********************************************************************
* @brief：      电压合格率计算,电压在正常范围内的时间占监测总时间的比率
* @param[in]： cVolt 当前电压
* @param[in]： qRate 合格率
* @return：     void
*********************************************************************
*/
void sta_volt_qualifyrate_unit_calc(uint16 *extTime, OOP_VOLTQR_T *voltObj)
{   
    uint32 tmTime=0;
        
    #if UPUP_EN == 1
    tmTime = (extTime[0] + extTime[1] + extTime[2]);
    #else
    tmTime = (extTime[0] + extTime[1] + extTime[2]+extTime[3]+extTime[4]);
    #endif

    //终端供电时间为0就不处理了
    if(tmTime != 0)
    {
        //电压合格率，%(*100) 换算-2(*100)，四舍五入,
        voltObj->dwTime1 = tmTime;           //电压监测时间
        #if UPUP_EN == 1
        voltObj->dwTime3 = extTime[1];       //电压超上限时间
        voltObj->dwTime4 = extTime[2];       //电压超下限时间
        #else 
        voltObj->dwTime3 = extTime[1]+extTime[3];
        voltObj->dwTime4 = extTime[2]+extTime[4];
        #endif

        voltObj->wRate1  = (uint16)(extTime[0]*10000/tmTime + 0.5);   //电压合格率
        //voltObj->wRate2 =  (uint16)((voltObj->dwTime3 + voltObj->dwTime4)*10000/tmTime);; //电压越限率   
        voltObj->wRate2 = 10000-voltObj->wRate1;
    }

    return;
}

/**
*********************************************************************
* @brief：      电压合格率计算,电压在正常范围内的时间占监测总时间的比率
* @param[in]： cVolt 当前电压
* @param[in]： qRate 合格率
* @return：     void
*********************************************************************
*/
void sta_volt_qualifyrate_calc(void)
{   
    int16 i;
    int ret = 0;
    uint16 extTime[5] = {0,0,0,0,0};    //电压合格时间
    uint16 phaseTimeDay[5][3] = {{0}};  //三相的日合格时间
    uint16 phaseTimeMon[5][3] = {{0}};  //三相的月合格时间    
    OOP_VOLTSTAT_T  voltObj;//电压合格率对象
    NOMAL_OAD_T nomalOAD = {0};
    
    ret = sta_volt_qualifytime_calc(phaseTimeDay, phaseTimeMon);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_volt_qualifytime_calc failed.\n");
        return;
    }
    
    nomalOAD.oad.value = 0x21310200; //A相电压合格率属性

    for(i = 0; i < 3; i++)
    {
        extTime[0] = phaseTimeDay[0][i]; //合格
        extTime[1] = phaseTimeDay[1][i]; //越上限
        extTime[2] = phaseTimeDay[2][i]; //越下限
        extTime[3] = phaseTimeDay[3][i]; //越上上限
        extTime[4] = phaseTimeDay[4][i]; //越下下限

        STAT_FMT_DEBUG("volt[%d] over limit time day: qualify(%d), up(%d), dw(%d), upup(%d), dwdw(%d)\n", 
            i, extTime[0], extTime[1], extTime[2], extTime[3], extTime[4]);

        sta_volt_qualifyrate_unit_calc(extTime, &voltObj.day);

        extTime[0] = phaseTimeMon[0][i];
        extTime[1] = phaseTimeMon[1][i];
        extTime[2] = phaseTimeMon[2][i];
        extTime[3] = phaseTimeMon[3][i];
        extTime[4] = phaseTimeMon[4][i];

        STAT_FMT_DEBUG("volt[%d] over limit time mon: qualify(%d), up(%d), dw(%d), upup(%d), dwdw(%d)\n", 
            i, extTime[0], extTime[1], extTime[2], extTime[3], extTime[4]);

        sta_volt_qualifyrate_unit_calc(extTime, &voltObj.mon);

        nomalOAD.logicId = 0;
        nomalOAD.oad.nObjID = 0x2131 + i; //分别遍历 A,B,C相的电压合格率
        nomalOAD.classtag = CLASS_DATA_NORMAL;
        
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&voltObj, sizeof(voltObj));
        if(ret != ERR_OK )
        {
            STAT_FMT_DEBUG("volt_qualifyrate_calc write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(voltObj));
            return;
        }       
    }

    return;
}

#endif

#if DESC("不平衡率计算",1)

/**
*********************************************************************
* @brief：      不平衡率计算
* @param[in]： data      数据
* @param[in]： isSigned 是否有符号数
* @param[out]：rate      不平衡率
* @return：     void
*********************************************************************
*/
int sta_imbalance_rate_calc(void *data, BOOLEAN isSigned, uint16 *rate)
{
    int ret = ERR_OK;
    uint32 max = 0;
    uint32 min = 0;
    uint32 tmp[3] = {0};
    uint8   powerType=0; 
    uint32 len = 0;    
    NOMAL_OAD_T nomalOAD = {0};    

    //交采参数，逻辑设备号为1
    nomalOAD.logicId = 1;
    nomalOAD.oad.value = 0x40100200;

    //没设置默认为3相4线
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(uint16), &powerType, &len);
    if((ret != 0) || (powerType == PWRTYPE_UNKNOWN))
    {
        STAT_FMT_DEBUG("power type read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d), powerType(%d), use default(PWRTYPE_3PHASE4).\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint16), powerType);
        powerType = PWRTYPE_3PHASE4;
        ret = ERR_OK;
    }

    if(isSigned)
    {
        if(powerType  == PWRTYPE_3PHASE4)//三相四线
        {
            tmp[0] = sta_signed_abs_calc(((int32*)data)[0]);
            tmp[1] = sta_signed_abs_calc(((int32*)data)[1]);
            tmp[2] = sta_signed_abs_calc(((int32*)data)[2]);    
        }   
        else if(powerType  == PWRTYPE_3PHASE3) //三相三线
        {
            //三相三线B相默认成A相保证最大值最小值的计算
            tmp[0] = sta_signed_abs_calc(((int32*)data)[0]);
            tmp[1] = sta_signed_abs_calc(((int32*)data)[0]);
            tmp[2] = sta_signed_abs_calc(((int32*)data)[2]);    
        }
    }
    else
    {
        if(powerType  == PWRTYPE_3PHASE4) //三相四线
        {
            tmp[0] = ((uint16*)data)[0];
            tmp[1] = ((uint16*)data)[1];
            tmp[2] = ((uint16*)data)[2];
        }
        else if(powerType  == PWRTYPE_3PHASE3) //三相三线
        {
            //三相三线B相默认成A相保证最大值最小值的计算
            tmp[0] = ((uint16*)data)[0];
            tmp[1] = ((uint16*)data)[0];    
            tmp[2] = ((uint16*)data)[2];             
        }        
    }
        
    max =  sta_dlmax_calc(3,tmp);
    min =  sta_dlmin_calc(3,tmp);
   
    if(max != 0)
    {
        *rate = (uint16)((float32)(max-min)*100/max+0.5);
    }

    return ret;
}

/**
*********************************************************************
* @brief：      电流不平衡率计算
* @param[in]： stTSA     电能表地址
* @param[out]：pstMeter 电能表档案
* @return：     void
*********************************************************************
*/
void sta_curr_imbalance_calc(void)
{
    int ret = ERR_OK;
    uint16  ultemp_cur = 0; //电流不平衡度
    int32   curr[3];          //三相电流    
    uint32 len = 0; 
    OOP_INT3V_T rtData = {0};    
    NOMAL_OAD_T nomalOAD = {0};    
     
    //读取实时电流
    nomalOAD.logicId = 1;
    nomalOAD.oad.value = 0x20010200;

    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_INT3V_T), (uint8*)&rtData, &len);
    if((ret != 0) || (rtData.nNum > 4))
    {
        STAT_FMT_DEBUG("real-time current read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d), nNum(%d).\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_INT3V_T), rtData.nNum);
    
        return;
    }
    
    memcpy(curr, rtData.nValue, sizeof(curr));

    //不平衡率计算
    ret = sta_imbalance_rate_calc(curr, 1, &ultemp_cur); 
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_imbalance_rate_calc failed.\n");
        return;
    }

    nomalOAD.logicId = 1;
    nomalOAD.oad.value = 0x20270200;
    nomalOAD.classtag = CLASS_DATA_NORMAL;
    
    //电流不平衡率
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&ultemp_cur, sizeof(uint16)); 
    if(ret < 0)
    {
        STAT_FMT_DEBUG("current imbalance rate write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint16));
    
        return;
    }

    return;
}

/**
*********************************************************************
* @brief：      电压不平衡率计算
* @param[in]： stTSA     电能表地址
* @param[out]：pstMeter 电能表档案
* @return：     void
*********************************************************************
*/
void sta_volt_imbalance_calc(void)
{
    int ret = ERR_OK;
    uint16  ultemp_vlt = 0; //电压不平衡度
    uint16  volt[3];        //三相电压      
    uint32 len = 0;    
    OOP_WORD3V_T rtData = {0};       
    NOMAL_OAD_T nomalOAD = {0};    
    
    //读取实时电压
    nomalOAD.logicId = 1;
    nomalOAD.oad.value = 0x20000200;
    
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_WORD3V_T), (uint8*)&rtData, &len);
    if((ret != 0) || (rtData.nNum > 3))
    {
        STAT_FMT_DEBUG("real-time voltage read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d), nNum(%d).\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_WORD3V_T), rtData.nNum);
    
        return;
    }

    //拷贝一个数组
    arrcpy(volt, sizeof(uint16), rtData.nValue, sizeof(uint32), 3);
      
    //不平衡率计算
    ret = sta_imbalance_rate_calc(volt, 0, &ultemp_vlt); 
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_imbalance_rate_calc failed.\n");
        return;
    }

    nomalOAD.logicId = 1;
    nomalOAD.oad.value = 0x20260200;
    nomalOAD.classtag = CLASS_DATA_NORMAL;
    
    //电压不平衡率
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&ultemp_vlt, sizeof(uint16));  
    if(ret < 0)
    {
        STAT_FMT_DEBUG("current imbalance rate write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint16));
    
        return;
    } 
    
    return;
}

#endif

#if DESC("分钟电量计算",1)

int sta_basic_energy_deal(const STA_MTR_INFO_T * mtrInfo, uint8 rtValid, const int64 *rtData, int64 *lastData)
{
    int i = 0;
    
    if(rtValid == FALSE)  // 
    {
        STAT_FMT_DEBUG("****无数据情况\n");
        
        if(mtrInfo->type == MTR_PULSE)
        {
            if(rtData[0] < 5000)                    //实时数小于0.5度的话,从0开始  
            {
               for(i = 0;i < 5;i++)
               {
                  lastData[i] = 0;                  //电量底数初始化为0
               }
            }
            else                                    //实时数大于等于0.5度的话,记录数据   
            {
               for(i = 0;i < 5;i++)
               {
                   lastData[i] = rtData[i];
               }
            }  
        }
        else                                        //500 485表0.05度计算
        {
            for(i = 0;i < 5;i++)
            {
               lastData[i] = rtData[i];
            }
        }
    }
    else  //读到数据
    {
        for(i = 0;i < 5;i++)
        {
            if(lastData[i] == NONELLWORD)
            {
                STAT_FMT_DEBUG("****数据[%d]无效情况\n", i);
                
                if(mtrInfo->type == MTR_PULSE)
                {
                    if(rtData[0] < 5000)
                    {
                        lastData[i] = 0;
                    }
                    else
                    {
                        lastData[i] = rtData[i];
                    }
                }
                else
                {
                    lastData[i] = rtData[i];
                }
            }
            
        }
    }    

    return 0;
}

/**
*********************************************************************
* @brief：      写1分钟电量基数
* @param[in]： cEnergy  电量OAD
* @param[out]: actEnergy 有功电能
* @param[out]: reactEnergy 无功电能
* @return：     void 
*********************************************************************
*/
int sta_basic_1minen_write(const STA_MTR_INFO_T * mtrInfo, uint32 dataOffsetcs, void *curData)
{
    int     ret = 0;
    char    pfile[20] = {0};  //TODO 私有文件路径 根据测量点号得到的
    char    pfilebak[25] = {0};  //备份文件
    uint8  lastnew[41] = {0};

    //私有文件，485表需要64个，脉冲组8个，交采1个。
    switch(mtrInfo->type)
    {
        case MTR_AC:    sprintf(pfile,"statacsample");break;
        case MTR_485:   sprintf(pfile,"statmeter%d",mtrInfo->nIndex);break;
        case MTR_PULSE: sprintf(pfile,"statpluse%d",mtrInfo->pulseOAD.nObjID&0xff);break;
        default: return -1;
    }
    
    //备份文件名
    sprintf(pfilebak, "bak%s", pfile);

    memcpy(&lastnew, curData, 40);
    lastnew[40] = sta_checksum_calc(lastnew, 40);

    /*更新电量底数*/
    ret = write_pdata_xram(pfile,(char *)lastnew,dataOffsetcs+STA_MINCS_ENERGY_BASE_SEG,STA_MINCS_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("更新电量底数失败ret=%d\n", ret);
    }
    
    /*更新电量底数备份*/
    ret = write_pdata_xram(pfilebak,(char *)lastnew,dataOffsetcs+STA_MINCS_ENERGY_BASE_SEG,STA_MINCS_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("更新电量备份底数失败ret=%d\n", ret);
    }

    return ret;
}

/**
*********************************************************************
* @brief：      读测量点1分钟电量
* @param[in]： cEnergy  电量OAD
* @param[out]: actEnergy 有功电能
* @param[out]: reactEnergy 无功电能
* @return：     void 
*********************************************************************
*/
int sta_basic_1minen_read(const STA_MTR_INFO_T * mtrInfo, uint32 dataOffsetcs, void *basic)
{
    int     ret;
    char    pfile[20] = {0};  //TODO 私有文件路径 根据测量点号得到的
    char    pfilebak[25] = {0};  //备份文件
    uint16  seg_addr = dataOffsetcs + STA_MINCS_ENERGY_BASE_SEG;
    uint8  mtrType = mtrInfo->type;
    uint8  lastnew[41] = {0};
    uint8  cs = 0;

    //私有文件，485表需要64个，脉冲组8个，交采1个。
    switch(mtrType)
    {
        case MTR_AC:    sprintf(pfile,"statacsample");break;
        case MTR_485:   sprintf(pfile,"statmeter%d",mtrInfo->nIndex);break;
        case MTR_PULSE: sprintf(pfile,"statpluse%d",mtrInfo->pulseOAD.nObjID&0xff);break;
        default: return -1;
    }

    //备份文件名
    sprintf(pfilebak, "bak%s", pfile);
    
    /*读取私有外存电能读数(运算基数)*/
    ret = read_pdata_xram(pfile,(char *)lastnew, seg_addr, STA_MINCS_ENERGY_UNIT_SIZE);
    FREEZE_BUF_TRACE(lastnew, 41, "%s读取底数:",pfile);
    
    //计算校验和
    cs = sta_checksum_calc(lastnew, 40);
    //读失败或校验失败则读备份文件
    if((ret != 0) || (cs != lastnew[40]))
    {
        memset(lastnew, 0, sizeof(lastnew));
        ret = read_pdata_xram(pfilebak,(char *)lastnew, seg_addr, STA_MINCS_ENERGY_UNIT_SIZE);
        
        STAT_BUF_DEBUG(lastnew, 41, "%s读取底数备份:",pfilebak);
        cs = sta_checksum_calc(lastnew, 40);
        if((ret != 0) || (cs != lastnew[40])) //读备份失败或校验失败则读备份文件
        {
            return ERR_INVALID;
        }
    }

    //读取正确的处理
    memcpy(basic, &lastnew, STA_MIN_ENERGY_UNIT_SIZE);

    return ERR_OK;
}

/**
*********************************************************************
* @brief：      写1分钟电量基数
* @param[in]： cEnergy  电量OAD
* @param[out]: actEnergy 有功电能
* @param[out]: reactEnergy 无功电能
* @return：     void 
*********************************************************************
*/
int sta_basic_1minen_clear(const STA_MTR_INFO_T * mtrInfo)
{
    int     ret = 0;
    char    pfile[20];  //TODO 私有文件路径 根据测量点号得到的
    char datapath[256] = {0};
    char bakdatapath[256] = {0};

    //删除所有测量点
    if((mtrInfo->type == MTR_485)&&(mtrInfo->nIndex == 0xffff))
    {
        sprintf(datapath,"/data/app/%s/data/statmeter*",ADV_APP_NAME);

        ret = wildcard_remove(datapath);
        if(ret != 0)
        {
            PRTL_FMT_DEBUG("remove \"%s\". ret(%d), %s\n", datapath, ret, strerror(errno));
        }
        else
        {
            PRTL_FMT_DEBUG("remove \"%s\" success.\n", datapath);
        }

        sprintf(bakdatapath,"/data/app/%s/data/bakstatmeter*",ADV_APP_NAME);

        ret = wildcard_remove(bakdatapath);
        if(ret != 0)
        {
            PRTL_FMT_DEBUG("remove \"%s\". ret(%d), %s\n", bakdatapath, ret, strerror(errno));
        }
        else
        {
            PRTL_FMT_DEBUG("remove \"%s\" success.\n", bakdatapath);
        }
    }
    else
    {
        //私有文件，485表需要64个，脉冲组8个，交采1个。
        switch(mtrInfo->type)
        {
            case MTR_AC:    sprintf(pfile,"statacsample");break;
            case MTR_485:   sprintf(pfile,"statmeter%d",mtrInfo->nIndex);break;
            case MTR_PULSE: sprintf(pfile,"statpluse%d",mtrInfo->pulseOAD.nObjID&0xff);break;
            default: return -1;
        }
        
        sprintf(datapath,"/data/app/%s/data/%s",ADV_APP_NAME, pfile);

        ret = remove(datapath);
        if(ret != 0)
        {
            PRTL_FMT_DEBUG("remove \"%s\". ret(%d), %s\n", datapath, ret, strerror(errno));
        }
        else
        {
            PRTL_FMT_DEBUG("remove \"%s\" success.\n", datapath);
        }

        sprintf(bakdatapath,"/data/app/%s/data/bak%s",ADV_APP_NAME, pfile);

        ret = remove(bakdatapath);
        if(ret != 0)
        {
            PRTL_FMT_DEBUG("remove \"%s\". ret(%d), %s\n", bakdatapath, ret, strerror(errno));
        }
        else
        {
            PRTL_FMT_DEBUG("remove \"%s\" success.\n", bakdatapath);
        }
    }

    //立即同步到磁盘
    sync();
    
    return ret;
}

/**
*********************************************************************
* @brief：      写测量点1分钟电量
* @param[in]： cEnergy  电量OAD
* @param[out]: actEnergy 有功电能
* @param[out]: reactEnergy 无功电能
* @return：     void 
*********************************************************************
*/
int sta_delta_1minen_write(const STA_MTR_INFO_T * mtrInfo, uint32 dataOffsetcs, void *delta)
{
    int     ret = 0;
    char    pfile[20] = {0};  //TODO 私有文件路径 根据测量点号得到的
    uint8   deltacs[41] = {0};  //带CS校验的差值数据

    //私有文件，485表需要64个，脉冲组8个，交采1个。
    switch(mtrInfo->type)
    {
        case MTR_AC:    sprintf(pfile,"statacsample");break;
        case MTR_485:   sprintf(pfile,"statmeter%d",mtrInfo->nIndex);break;
        case MTR_PULSE: sprintf(pfile,"statpluse%d",mtrInfo->pulseOAD.nObjID&0xff);break;
        default: return -1;
    }

    memcpy(deltacs, delta, STA_MIN_ENERGY_UNIT_SIZE);
    
    deltacs[40] = sta_checksum_calc(delta, 40);
    
    /*更新分钟电量*/
    ret = write_pdata_xram(pfile,(char *)deltacs,dataOffsetcs+STA_MINCS_ENERGY_DATA_SEG,STA_MINCS_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("更新分钟电量失败ret=%d\n", ret);
    }
    
    return ret;
}

/**
*********************************************************************
* @brief：      读测量点1分钟电量
* @param[in]： cEnergy  电量OAD
* @param[out]: actEnergy 有功电能
* @param[out]: reactEnergy 无功电能
* @return：     void 
*********************************************************************
*/
int sta_delta_1minen_read(const STA_MTR_INFO_T * mtrInfo, uint32 dataOffsetcs, void *delta)
{
    int     ret = 0;
    char    pfile[20] = {0};  //TODO 私有文件路径 根据测量点号得到的
    uint8   deltacs[41] = {0};  //带CS校验的差值数据
    uint16  segOffset = dataOffsetcs + STA_MINCS_ENERGY_DATA_SEG;
    uint8  cs = 0;

    //私有文件，485表需要64个，脉冲组8个，交采1个。
    switch(mtrInfo->type)
    {
        case MTR_AC:    sprintf(pfile,"statacsample");break;
        case MTR_485:   sprintf(pfile,"statmeter%d",mtrInfo->nIndex);break;
        case MTR_PULSE: sprintf(pfile,"statpluse%d",mtrInfo->pulseOAD.nObjID&0xff);break;
        default: return -1;
    }

    /*读取私有外存电能读数*/
    ret = read_pdata_xram(pfile,(char *)deltacs, segOffset, STA_MINCS_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_TRACE("read_pdata_xram failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return ret;        
    }

    cs = sta_checksum_calc(deltacs, 40);
    if(deltacs[40] != cs)
    {
        STAT_FMT_DEBUG("校验失败\n");
        return ERR_CHK;
    }

    memcpy(delta, deltacs, STA_MIN_ENERGY_UNIT_SIZE);
    
    return ret;
}

/**
*********************************************************************
* @brief：      读取上次电量底数,即上次的实时电量
* @param[in]： cEnergy  电量OAD
* @param[out]: actEnergy 有功电能
* @param[out]: reactEnergy 无功电能
* @return：     void 
*********************************************************************
*/
int sta_lastbasic_data_read(const STA_MTR_INFO_T * mtrInfo, uint32 dataOffset, const int64 *curData, int64 *lstData)
{
    int     ret = 0;
    
    ret = sta_basic_1minen_read(mtrInfo, dataOffset, (uint8*)lstData);
    if(ret != ERR_OK)
    {
        sta_basic_energy_deal(mtrInfo, FALSE, curData, lstData);
    }
    else
    {
        sta_basic_energy_deal(mtrInfo, TRUE, curData, lstData);
    }

    return ERR_OK;
}

/**
*********************************************************************
* @brief：      读取实时电量
* @param[in]： cEnergy  电量OAD
* @param[out]: actEnergy 有功电能
* @param[out]: reactEnergy 无功电能
* @return：     void 
*********************************************************************
*/
int sta_realtime_data_read(const STA_MTR_INFO_T * mtrInfo,  OOP_OAD_U srcOAD, int64 *curData)
{
    NOMAL_OAD_T nomalOAD = {0};       
    READ_RECORD_T readRecord;
    RECORD_UNIT_T recordUnit;
    OOP_ENERGY_T energy;
    int ret = 0;
    uint32 len = 0;
    uint32 i = 0;
    
    MEMZERO(&readRecord, sizeof(readRecord));
    MEMZERO(&recordUnit, sizeof(recordUnit));
    MEMZERO(&energy, sizeof(energy));

    if(mtrInfo->type == MTR_AC)
    {
        nomalOAD.logicId = 1;
        nomalOAD.oad.value = srcOAD.value;
        
        ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("db_read_nomal failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
            return ret;
        }
    }
    else if(mtrInfo->type == MTR_PULSE)
    {
        nomalOAD.oad.value = srcOAD.value;
        ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
        if(ret != ERR_OK)
        {
            STAT_FMT_TRACE("db_read_nomal failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
            return ret;
        }
    }
    else if(mtrInfo->type == MTR_485) //采集的电量是按主OAD=0的记录型存储的
    {
        readRecord.recordOAD.road.oadCols.nNum = 1;
        readRecord.recordOAD.road.oadCols.oad[0].value = srcOAD.value; 
        readRecord.recordOAD.infoNum = mtrInfo->nIndex; //与电表TSA关联的配置序号
        readRecord.cType = COL_TM_END;
        readRecord.cEnd = time(NULL) + COMPENSATION_TIME+1; // 注：+8个小时(28800)转成格林时间，结束是开区间
        readRecord.cStart = (readRecord.cEnd/60)*60-60;  //筛选当前和上1分钟内的数据

        //需要跟抄表模块对齐MAC校验算法
        readRecord.MAC.nNum = 6;
        memcpy(readRecord.MAC.value, mtrInfo->tsa.add, 6);  
        
        ret = db_read_record(g_staUdpHandle, &readRecord, sizeof(energy), &recordUnit);
        if(ret != ERR_OK)
        {
            STAT_BUF_TRACE(readRecord.MAC.value, 6, "collect energy read failed. ret(%d) logicId(%d), infoNum(%d), option(%d), oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
               ret, readRecord.recordOAD.logicId, readRecord.recordOAD.infoNum, readRecord.recordOAD.optional, readRecord.recordOAD.road.oadMain.value, readRecord.recordOAD.road.oadCols.oad[0].value, sizeof(energy));
            return ret;
        }

        memcpy(&energy, recordUnit.record[0].outDataBuf, sizeof(energy));
    }
    else
    {
        ;//NOTHING TO DO
    }

    if(energy.nNum > OOP_FULL_RATES)
    {
        STAT_FMT_DEBUG("energy.nNum(%d) > OOP_FULL_RATES(%d).\n", energy.nNum, OOP_FULL_RATES);
        return ret;
    }
    
    arrcpy(curData, sizeof(int64), energy.nValue, sizeof(uint32), energy.nNum);

    //电表判断电量有效性和全FF
    if(mtrInfo->type == MTR_485)
    {
        for(i = 0; i < 5; i++)
        {
            if(0 == get_bit_value((uint8*)&energy.rsv,32,i) 
                || energy.nValue[i] == 0xFFFFFFFF)
            {
                curData[i] = 0;
            }
        }
    }
    
    //交采和485电量换算-2， 脉冲电量换算-4，统一转换为总加组电量换算-4
    if(mtrInfo->type != MTR_PULSE)
    {
        for(i = 0; i < 5; i++)
        {
            curData[i] = curData[i]*100;
        }
    }

    return ret;      
}

/**
*********************************************************************
* @brief：      测量点电量计算
* @param[in]： tgParm  总加组参数
* @param[out]: actEnergy 有功电能
* @param[out]: reactEnergy 无功电能
* @return：     void 
*********************************************************************
*/
void sta_mpoint_energy_calc(const STA_MTR_INFO_T * mtrInfo, OOP_OAD_U srcOAD, uint32 dataOffsetcs)
{
    int     i;
    int64  curData[5] = {0,0,0,0,0};     /*当前电表读数（8字节）*/
    int64  lstData[5] = {0,0,0,0,0};   /*前一分钟电表读数*/
    int64  detaData[5] = {0,0,0,0,0};       /*分钟电量*/
    int ret = 0;
    char *typechr[4] = {"正有", "反有", "正无", "反无"};    
    char *typepchr[4] = {"正有", "正无","反有", "反无"};
    char *mtrichr[7] = {"485", "模拟表", "脉冲", "虚拟", "交采", "载波", "载波多功能"};
    uint8 backFlag = FALSE; //电量倒走标记

    //每次计算前先清零分钟电量
    ret = sta_delta_1minen_write(mtrInfo, dataOffsetcs, detaData); 
    if(ret != 0)
    {
        STAT_FMT_DEBUG("清零分钟电量失败ret=%d\n", ret);
    }

    //读取当前电能示值
    ret = sta_realtime_data_read(mtrInfo, srcOAD, curData);
    if(ret != 0) 
    {
        STAT_FMT_TRACE("sta_rt_data_read failed.\n");
        return;
    }

    //读取上次电能示值(运算基数)
    ret = sta_lastbasic_data_read(mtrInfo, dataOffsetcs, curData, lstData);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_lst_basic_data_read failed.\n");
        return;
    }
    
    //计算分钟电量
    for(i = 0; i < 5; i++)
    {
        if(curData[i] < lstData[i]) /*当前读数比底数小的情况*/
        {
            backFlag = TRUE;
            break;
        }
    }

    if(backFlag)
    {    
        STAT_FMT_DEBUG("电量倒走\n");
        memset(&detaData, 0, sizeof(detaData));
    }
    else
    {
        //计算分钟电量
        for(i = 0; i < 5; i++)
        {
            detaData[i] = curData[i] - lstData[i];
        }
    }

    if(detaData[0] != (detaData[1] + detaData[2] + detaData[3] + detaData[4]))
    {
        STAT_FMT_DEBUG("总电量异常 总 %lld 费率电量和 %lld\n", 
            detaData[0], detaData[1] + detaData[2] + detaData[3] + detaData[4]);
    }

    if(mtrInfo->type != MTR_PULSE)
    {
        STAT_FMT_DEBUG("上次%s示值 Pn %d %s 总 %lld 费率1 %lld 费率2 %lld 费率3 %lld 费率4 %lld\n", 
            typechr[(srcOAD.nObjID>>4)-1], mtrInfo->nIndex, mtrichr[mtrInfo->type-1], lstData[0],lstData[1],lstData[2],lstData[3],lstData[4]);
        STAT_FMT_DEBUG("实时%s示值 Pn %d %s 总 %lld 费率1 %lld 费率2 %lld 费率3 %lld 费率4 %lld\n", 
            typechr[(srcOAD.nObjID>>4)-1], mtrInfo->nIndex, mtrichr[mtrInfo->type-1], curData[0],curData[1],curData[2],curData[3],curData[4]);
        STAT_FMT_DEBUG("本次%s电量 Pn %d %s 总 %lld 费率1 %lld 费率2 %lld 费率3 %lld 费率4 %lld\n", 
            typechr[(srcOAD.nObjID>>4)-1], mtrInfo->nIndex, mtrichr[mtrInfo->type-1], detaData[0],detaData[1],detaData[2],detaData[3],detaData[4]);
    }
    else
    {
        STAT_FMT_DEBUG("上次%s示值 Pn %d %s 总 %lld 费率1 %lld 费率2 %lld 费率3 %lld 费率4 %lld\n", 
            typepchr[srcOAD.attID-15], mtrInfo->pulseOAD.nObjID&0xff, mtrichr[mtrInfo->type-1], lstData[0],lstData[1],lstData[2],lstData[3],lstData[4]);
        STAT_FMT_DEBUG("实时%s示值 Pn %d %s 总 %lld 费率1 %lld 费率2 %lld 费率3 %lld 费率4 %lld\n", 
            typepchr[srcOAD.attID-15], mtrInfo->pulseOAD.nObjID&0xff, mtrichr[mtrInfo->type-1], curData[0],curData[1],curData[2],curData[3],curData[4]);
        STAT_FMT_DEBUG("本次%s电量 Pn %d %s 总 %lld 费率1 %lld 费率2 %lld 费率3 %lld 费率4 %lld\n", 
            typepchr[srcOAD.attID-15], mtrInfo->pulseOAD.nObjID&0xff, mtrichr[mtrInfo->type-1], detaData[0],detaData[1],detaData[2],detaData[3],detaData[4]);
    }
    
    //保存分钟电量底数
    ret = sta_basic_1minen_write(mtrInfo, dataOffsetcs, curData);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("保存分钟电量底数失败ret=%d\n", ret);
    }

    /*保存分钟电量*/ 
    ret = sta_delta_1minen_write(mtrInfo, dataOffsetcs, detaData); 
    if(ret != 0)
    {
        STAT_FMT_DEBUG("保存分钟电量失败ret=%d\n", ret);
    }
}


/**
*********************************************************************
* @brief：      测量点计算入口，计算交采，脉冲，485表的分钟电量
* @param[in]： tgParm  总加组参数
* @param[out]: actEnergy 有功电能
* @param[out]: reactEnergy 无功电能
* @Create    : 贺宁
* @Date      ：2020-04-15
* @return：     void 
*********************************************************************
*/
void sta_mpoint_entry(void)
{
    OOP_OAD_U oad = {0};
    STA_MTR_INFO_T mtrInfo;
    OOP_METER_T meter;
    uint32 i = 0;
    uint8 cnt = 0;
    
    MEMZERO(&mtrInfo, sizeof(mtrInfo));
    MEMZERO(&meter, sizeof(meter));

    //计算交采分钟电量
    mtrInfo.type = MTR_AC;
    
    //计算正向有功
    oad.value = 0x00100200;
    sta_mpoint_energy_calc(&mtrInfo, oad, STA_POS_ACTCS_ENERGY_OFFSET);

    //计算反向有功
    oad.value = 0x00200200;
    sta_mpoint_energy_calc(&mtrInfo, oad, STA_NEG_ACTCS_ENERGY_OFFSET);

    //计算正向无功
    oad.value = 0x00300200;
    sta_mpoint_energy_calc(&mtrInfo, oad, STA_POS_RCTCS_ENERGY_OFFSET);

    //计算反向无功
    oad.value = 0x00400200;
    sta_mpoint_energy_calc(&mtrInfo, oad, STA_NEG_RCTCS_ENERGY_OFFSET);

    //等待脉冲数据准备OK
    while(!pulse_get_ok() && (cnt++ < 20))
    {
        usleep(100000);
    }
    usleep(100000);

    //计算脉冲分钟电量
    mtrInfo.type = MTR_PULSE;

    oad.value = 0x24010200;
    for(i = 0; i < 8; i++)
    {
        oad.nObjID = 0x2401 + i;
        
        mtrInfo.pulseOAD.value = oad.value;

        //计算正向有功
        oad.attID = 15;
        sta_mpoint_energy_calc(&mtrInfo, oad, STA_POS_ACTCS_ENERGY_OFFSET);

        //计算反向有功
        oad.attID = 17;
        sta_mpoint_energy_calc(&mtrInfo, oad, STA_NEG_ACTCS_ENERGY_OFFSET);

        //计算正向无功
        oad.attID = 16;
        sta_mpoint_energy_calc(&mtrInfo, oad, STA_POS_RCTCS_ENERGY_OFFSET);

        //计算反向无功
        oad.attID = 18;
        sta_mpoint_energy_calc(&mtrInfo, oad, STA_NEG_RCTCS_ENERGY_OFFSET);
    }

    //计算485表分钟电量
    for(i = 0; i < oopMeterList.oopMeterNum; i++)
    {
        if(app_get_meter_list_by_id(i, &meter) == 0)
        {
            if(meter.basic.port.nObjID == 0xf201)
            {
                mtrInfo.type = MTR_485;
            }
            else
            {
                continue;
            }
            
            mtrInfo.nIndex = meter.nIndex;
            memcpy(&mtrInfo.tsa, &meter.basic.tsa, sizeof(OOP_TSA_T));

            //计算正向有功
            oad.value = 0x00100200;
            sta_mpoint_energy_calc(&mtrInfo, oad, STA_POS_ACTCS_ENERGY_OFFSET);

            //计算反向有功
            oad.value = 0x00200200;
            sta_mpoint_energy_calc(&mtrInfo, oad, STA_NEG_ACTCS_ENERGY_OFFSET);

            //计算正向无功
            oad.value = 0x00300200;
            sta_mpoint_energy_calc(&mtrInfo, oad, STA_POS_RCTCS_ENERGY_OFFSET);   

            //计算反向无功
            oad.value = 0x00400200;
            sta_mpoint_energy_calc(&mtrInfo, oad, STA_NEG_RCTCS_ENERGY_OFFSET);
        }
    }

    return;
}

#endif

#if DESC("脉冲相关计算",1)
/**
*********************************************************************
* @brief：      脉冲正向有功电能量Positive active electric energy计算（日月同时统计）
* @param[in]： tgODA 总加组对象
* @param[in]： minCalc 总加组对象
* @Create    : 贺宁
* @Date      ：2020-04-15
* @return：     void
*********************************************************************
*/
void sta_pulse_pae_calc(OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    STA_MTR_INFO_T mtrInfo;
    uint32 i = 0;
    int64 minEnergy[5] = {0}; /* （尖、峰、平、谷）分钟电量变量(有功) */
    NOMAL_OAD_T nomalOAD = {0};       
    OOP_ENERGY_T energy;
    uint32 len = 0;

    nomalOAD.oad.value = pulseOAD.value;

    mtrInfo.type = MTR_PULSE;
    mtrInfo.pulseOAD.value = pulseOAD.value;

    //正向有功
    MEMZERO(minEnergy, sizeof(minEnergy));
    sta_delta_1minen_read(&mtrInfo, STA_POS_ACTCS_ENERGY_OFFSET, minEnergy);
    
    nomalOAD.oad.attID = 7;//当日正向有功电量
    
    MEMZERO(&energy, sizeof(energy));
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
    
    energy.nNum = 5; //五费率，总尖峰平谷
    
    for(i = 0; i < energy.nNum; i++)
    {
        energy.nValue[i] += minEnergy[i];
    }
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("day positive active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    } 

    if(pastdayflag)
    {
        STAT_FMT_DEBUG("脉冲%d日正有电量 总 %d 费率1 %d 费率2 %d 费率3 %d 费率4 %d\n",
            pulseOAD.nObjID&0xff, energy.nValue[0], energy.nValue[1], energy.nValue[2], energy.nValue[3], energy.nValue[4]);

    }

    nomalOAD.oad.attID = 8;//当月正向有功电量
    
    MEMZERO(&energy, sizeof(energy));
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
    
    energy.nNum = 5; //五费率，总尖峰平谷
    
    for(i = 0; i < energy.nNum; i++)
    {
        energy.nValue[i] += minEnergy[i];
    }
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("month positive active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    } 

    if(pastmonflag)
    {
        STAT_FMT_DEBUG("脉冲%d月正有电量 总 %d 费率1 %d 费率2 %d 费率3 %d 费率4 %d\n",
            pulseOAD.nObjID&0xff, energy.nValue[0], energy.nValue[1], energy.nValue[2], energy.nValue[3], energy.nValue[4]);

    }

    return;
}

/**
*********************************************************************
* @brief：      脉冲正向无功电能量Positive reactive electric energy计算（日月同时统计）
* @param[in]： tgODA 总加组对象
* @param[in]： minCalc 总加组对象
* @Create    : 贺宁
* @Date      ：2020-04-15
* @return：     void
*********************************************************************
*/
void sta_pulse_pre_calc(OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    STA_MTR_INFO_T mtrInfo;
    uint32 i = 0;
    int64 minEnergy[5] = {0}; /* （尖、峰、平、谷）分钟电量变量(有功) */
    NOMAL_OAD_T nomalOAD = {0};       
    OOP_ENERGY_T energy;
    uint32 len = 0;

    nomalOAD.oad.value = pulseOAD.value;

    mtrInfo.type = MTR_PULSE;
    mtrInfo.pulseOAD.value = pulseOAD.value;

    //正向无功   
    MEMZERO(minEnergy, sizeof(minEnergy));
    sta_delta_1minen_read(&mtrInfo, STA_POS_RCTCS_ENERGY_OFFSET, minEnergy);
    
    nomalOAD.oad.attID = 11;//当日正向无功电量
    
    MEMZERO(&energy, sizeof(energy));
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
    
    energy.nNum = 5; //五费率，总尖峰平谷
    
    for(i = 0; i < energy.nNum; i++)
    {
        energy.nValue[i] += minEnergy[i];
    }
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("day positive reactive power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    } 

    if(pastdayflag)
    {
        STAT_FMT_DEBUG("脉冲%d日正无电量 总 %lld 费率1 %lld 费率2 %lld 费率3 %lld 费率4 %lld\n",
            pulseOAD.nObjID&0xff, energy.nValue[0], energy.nValue[1], energy.nValue[2], energy.nValue[3], energy.nValue[4]);

    }

    nomalOAD.oad.attID = 12;//当月正向无功电量
    
    MEMZERO(&energy, sizeof(energy));
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
    
    energy.nNum = 5; //五费率，总尖峰平谷
    
    for(i = 0; i < energy.nNum; i++)
    {
        energy.nValue[i] += minEnergy[i];
    }
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("month positive reactive power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    } 

    if(pastmonflag)
    {
        STAT_FMT_DEBUG("脉冲%d月正无电量 总 %lld 费率1 %lld 费率2 %lld 费率3 %lld 费率4 %lld\n",
            pulseOAD.nObjID&0xff, energy.nValue[0], energy.nValue[1], energy.nValue[2], energy.nValue[3], energy.nValue[4]);

    }

    return;
}

/**
*********************************************************************
* @brief：      脉冲反向有功电能量negative active electric energy计算（日月同时统计）
* @param[in]： tgODA 总加组对象
* @param[in]： minCalc 总加组对象
* @Create    : 贺宁
* @Date      ：2020-04-15
* @return：     void
*********************************************************************
*/
void sta_pulse_nae_calc(OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    STA_MTR_INFO_T mtrInfo;
    uint32 i = 0;
    int64 minEnergy[5] = {0}; /* （尖、峰、平、谷）分钟电量变量(有功) */
    NOMAL_OAD_T nomalOAD = {0};       
    OOP_ENERGY_T energy;
    uint32 len = 0;

    nomalOAD.oad.value = pulseOAD.value;

    mtrInfo.type = MTR_PULSE;
    mtrInfo.pulseOAD.value = pulseOAD.value;

    //反向有功
    MEMZERO(minEnergy, sizeof(minEnergy));
    sta_delta_1minen_read(&mtrInfo, STA_NEG_ACTCS_ENERGY_OFFSET, minEnergy);
    
    nomalOAD.oad.attID = 9;//当日反向有功电量
    MEMZERO(&energy, sizeof(energy));
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
    
    energy.nNum = 5; //五费率，总尖峰平谷
    
    for(i = 0; i < energy.nNum; i++)
    {
        energy.nValue[i] += minEnergy[i];
    }
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("day negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    } 

    if(pastdayflag)
    {
        STAT_FMT_DEBUG("脉冲%d日反有电量 总 %lld 费率1 %lld 费率2 %lld 费率3 %lld 费率4 %lld\n",
            pulseOAD.nObjID&0xff, energy.nValue[0], energy.nValue[1], energy.nValue[2], energy.nValue[3], energy.nValue[4]);

    }

    nomalOAD.oad.attID = 10;//当月反向有功电量
    MEMZERO(&energy, sizeof(energy));
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
    
    energy.nNum = 5; //五费率，总尖峰平谷
    
    for(i = 0; i < energy.nNum; i++)
    {
        energy.nValue[i] += minEnergy[i];
    }
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("month negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    } 

    if(pastmonflag)
    {
        STAT_FMT_DEBUG("脉冲%d月反有电量 总 %lld 费率1 %lld 费率2 %lld 费率3 %lld 费率4 %lld\n",
            pulseOAD.nObjID&0xff, energy.nValue[0], energy.nValue[1], energy.nValue[2], energy.nValue[3], energy.nValue[4]);

    }

    return;
}

/**
*********************************************************************
* @brief：      脉冲反向无功电能量negative reactive electric energy计算（日月同时统计）
* @param[in]： tgODA 总加组对象
* @param[in]： minCalc 总加组对象
* @Create    : 贺宁
* @Date      ：2020-04-15
* @return：     void
*********************************************************************
*/
void sta_pulse_nre_calc(OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    STA_MTR_INFO_T mtrInfo;
    uint32 i = 0;
    int64 minEnergy[5] = {0}; /* （尖、峰、平、谷）分钟电量变量(有功) */
    NOMAL_OAD_T nomalOAD = {0};       
    OOP_ENERGY_T energy;
    uint32 len = 0;

    nomalOAD.oad.value = pulseOAD.value;

    mtrInfo.type = MTR_PULSE;
    mtrInfo.pulseOAD.value = pulseOAD.value;

    //反向无功
    MEMZERO(minEnergy, sizeof(minEnergy));
    sta_delta_1minen_read(&mtrInfo, STA_NEG_RCTCS_ENERGY_OFFSET, minEnergy);
    
    nomalOAD.oad.attID = 13;//当日反向无功电量
    
    MEMZERO(&energy, sizeof(energy));
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
    
    energy.nNum = 5; //五费率，总尖峰平谷
    
    for(i = 0; i < energy.nNum; i++)
    {
        energy.nValue[i] += minEnergy[i];
    }
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("day negative reactive power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    } 

    if(pastdayflag)
    {
        STAT_FMT_DEBUG("脉冲%d日反无电量 总 %lld 费率1 %lld 费率2 %lld 费率3 %lld 费率4 %lld\n",
            pulseOAD.nObjID&0xff, energy.nValue[0], energy.nValue[1], energy.nValue[2], energy.nValue[3], energy.nValue[4]);

    }

    nomalOAD.oad.attID = 14;//当月反向无功电量
    
    MEMZERO(&energy, sizeof(energy));
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
    
    energy.nNum = 5; //五费率，总尖峰平谷
    
    for(i = 0; i < energy.nNum; i++)
    {
        energy.nValue[i] += minEnergy[i];
    }
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("month negative reactive power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    }        

    if(pastmonflag)
    {
        STAT_FMT_DEBUG("脉冲%d月反无电量 总 %lld 费率1 %lld 费率2 %lld 费率3 %lld 费率4 %lld\n",
            pulseOAD.nObjID&0xff, energy.nValue[0], energy.nValue[1], energy.nValue[2], energy.nValue[3], energy.nValue[4]);

    }

    return;
}


/**
*********************************************************************
* @brief：      脉冲电能量计算（日月同时统计）
* @param[in]： tgODA 总加组对象
* @param[in]： minCalc 总加组对象
* @Create    : 贺宁
* @Date      ：2020-04-15
* @return：     void
*********************************************************************
*/
void sta_pulse_energy_calc(void)
{
    OOP_OAD_U oad = {0};
    uint32 i = 0;
    
    oad.value = 0x24010200;
    for(i = 0; i < 8; i++)
    {
        oad.nObjID = 0x2401 + i;

        sta_pulse_pae_calc(oad);
        sta_pulse_pre_calc(oad);
        sta_pulse_nae_calc(oad);
        sta_pulse_nre_calc(oad);
    }

    return;
}

#endif


#if DESC("总加组公用",1)

/**
*********************************************************************
* @brief：      成员总加计算
* @param[in]： ptct      倍率信息
* @param[in]： opFlag   运算标记
* @param[in]： tgMember 总加成员分量集合
* @param[out]：tgResult 总加结果分量集合
* @param[in]： nNum      总加成员分量数
* @Create    : 贺宁
* @Date      ：2020-04-17
* @return：     void
*********************************************************************
*/
void sta_virtual_tgcalc(const OOP_PTCT_T *ptct, const uint8 opFlag, const int64 *tgMember, int64 *tgResult, uint8 nNum)
{
    uint8 i = 0;

    if(opFlag == 1)
    {
        for(i = 0; i < nNum; i++)
        {
            tgResult[i] -= (tgMember[i]*(ptct->ct)*(ptct->pt));
        }
    }
    else if(opFlag == 0)
    {
        for(i = 0; i < nNum; i++)
        {
            tgResult[i] += (tgMember[i]*(ptct->ct)*(ptct->pt));
        }        
    }
}

/**
*********************************************************************
* @brief：      读取总加组属性表
* @param[in]：  mtrid   计量点号
* @param[out]：pstPara 总加组参数
* @return：     void
*********************************************************************
*/
int sta_virtual_param_read(OOP_OAD_U tgOAD, OOP_TGSETS_T *pstPara)
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};
    uint32 len = 0;
    OOP_OAD_U inOAD = {0x23010200}; //总加组属性表

    inOAD.nObjID = tgOAD.nObjID;
    nomalOAD.logicId = 0;
    memcpy(&nomalOAD.oad, &inOAD, sizeof(OOP_OAD_U));

    /* 读取总加组属性表 */
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_TGSETS_T), (uint8*)pstPara, &len);
    if(ret != 0)
    {
        STAT_FMT_TRACE("tg param read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_TGSETS_T));
    
        return ret;
    }
    
    return ret;
}

/**
*********************************************************************
* @brief：      读取互感器系数
* @param[in]：  pstMtrInfo  计量点信息
* @param[out]：pstPTCT      互感器系数
* @return：     void
*********************************************************************
*/
int sta_virtual_ptct_read(const STA_MTR_INFO_T *pstMtrInfo, OOP_PTCT_T *pstPTCT)
{
    uint32 ct = 0;
    uint32 pt = 0;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_METER_T meter;   //电表档案
    int ret = 0;
    uint32 len = 0;

    //交采的互感器系数0x401c0200-电流互感器变比 0x401d0200-电压互感器变比
    if(pstMtrInfo->type == MTR_AC)
    {
        nomalOAD.logicId = 0; //TODO
        nomalOAD.oad.value = 0x401c0200;

        ret =  db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(uint32), (uint8*)&ct, &len);

        nomalOAD.oad.value = 0x401d0200;
        
        ret |=  db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(uint32), (uint8*)&pt, &len);

        if(ret != 0)
        {
            return ret;
        }
        else
        {
            pstPTCT->ct = ct;
            pstPTCT->pt = pt;
        }
    }

    //脉冲计量点的互感器参数
    if(pstMtrInfo->type == MTR_PULSE)
    {
        nomalOAD.logicId = 0;
        nomalOAD.oad.value = pstMtrInfo->pulseOAD.value;
        nomalOAD.oad.attID = 3;
        
        ret =  db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_PTCT_T), (uint8*)pstPTCT, &len);
        if(ret != 0)
        {
            return ret;
        }     
    }

    //485计量点的互感器参数
    if(pstMtrInfo->type == MTR_485)
    {
        ret =  app_get_meter_list(pstMtrInfo->nIndex, &meter);
        if(ret != 0)
        {
            return ret;
        }
        else
        {
            pstPTCT->ct = meter.extend.CT;
            pstPTCT->pt = meter.extend.PT;
        }
    }

    return ret;    
}

/**
*********************************************************************
* @brief：      从数据中心读取总加电能量
* @param[in]： tgOAD 总加组OAD
* @param[out]： actEnergy 有功电能数组
* @param[out]： rctEnergy 无功电能数组
* @return：     void
*********************************************************************
*/
int sta_virtual_energy_read(OOP_OAD_U tgOAD, int64 actEnergy[2][5], int64 rctEnergy[2][5])
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_HENERGYS_T henergys; 
    uint32 len = 0;

    MEMZERO(&henergys, sizeof(henergys));   

    nomalOAD.oad.nObjID = tgOAD.nObjID;
    nomalOAD.oad.attID = 7; 
    
    ret |= db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(henergys), (uint8*)&henergys, &len);

    ret |= memcpy_s(actEnergy[0], sizeof(actEnergy[0]), henergys.nValue, henergys.nNum*sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("memcpy_s failed. ret(%d)\n", ret);
        return ret;
    }

    nomalOAD.oad.nObjID = tgOAD.nObjID;
    nomalOAD.oad.attID = 8; 

    ret |= db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(henergys), (uint8*)&henergys, &len);

    ret |= memcpy_s(rctEnergy[0], sizeof(rctEnergy[0]), henergys.nValue, henergys.nNum*sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("memcpy_s failed. ret(%d)\n", ret);
        return ret;
    }


    nomalOAD.oad.nObjID = tgOAD.nObjID;
    nomalOAD.oad.attID = 9; 

    ret |= db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(henergys), (uint8*)&henergys, &len);

    ret |= memcpy_s(actEnergy[1], sizeof(actEnergy[1]), henergys.nValue, henergys.nNum*sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("memcpy_s failed. ret(%d)\n", ret);
        return ret;
    }

    nomalOAD.oad.nObjID = tgOAD.nObjID;
    nomalOAD.oad.attID = 10; 

    ret |= db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(henergys), (uint8*)&henergys, &len);

    ret |= memcpy_s(rctEnergy[1], sizeof(rctEnergy[1]), henergys.nValue, henergys.nNum*sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("memcpy_s failed. ret(%d)\n", ret);
        return ret;
    }

    return ret;
}

/**
*********************************************************************
* @brief：      存储总加电能量
* @param[in]： cEnergy 当前电能(不使用，总加需按配置读取所有类型电量，无法统一描述)
* @param[in]： tgODA 总加组电能
* @return：     void
*********************************************************************
*/
void sta_virtual_energy_write(OOP_OAD_U tgOAD, int64 actEnergy[2][5], int64 rctEnergy[2][5])
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_HENERGYS_T henergys;

    MEMZERO(&henergys, sizeof(henergys));
    
    //存储总加组数据0x23010700（日有功），0x23010800（日无功），0x23010900（月有功），0x23010a00（月无功）
    nomalOAD.classtag = CLASS_DATA_NORMAL;
    nomalOAD.oad.nObjID = tgOAD.nObjID;
    nomalOAD.oad.attID = 7; 

    henergys.nNum = 5; //五费率，总尖峰平谷
    ret = memcpy_s(henergys.nValue, sizeof(henergys.nValue), actEnergy[0], sizeof(actEnergy[0]));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("memcpy_s failed. ret(%d)\n", ret);
        return;
    }
            
    db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&henergys, sizeof(henergys));

    nomalOAD.oad.nObjID = tgOAD.nObjID;
    nomalOAD.oad.attID = 8; 

    henergys.nNum = 5; //五费率，总尖峰平谷
    ret = memcpy_s(henergys.nValue, sizeof(henergys.nValue), rctEnergy[0], sizeof(rctEnergy[0]));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("memcpy_s failed. ret(%d)\n", ret);
        return;
    }

    db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&henergys, sizeof(henergys));

    //TODO 过月，把数据存在上月，清当月数据

    nomalOAD.oad.nObjID = tgOAD.nObjID;
    nomalOAD.oad.attID = 9; 

    henergys.nNum = 5; //五费率，总尖峰平谷
    ret = memcpy_s(henergys.nValue, sizeof(henergys.nValue), actEnergy[1], sizeof(actEnergy[1]));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("memcpy_s failed. ret(%d)\n", ret);
        return;
    }

    db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&henergys, sizeof(henergys));

    nomalOAD.oad.nObjID = tgOAD.nObjID;
    nomalOAD.oad.attID = 10; 

    henergys.nNum = 5; //五费率，总尖峰平谷
    ret = memcpy_s(henergys.nValue, sizeof(henergys.nValue), rctEnergy[1], sizeof(rctEnergy[1]));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("memcpy_s failed. ret(%d)\n", ret);
        return;
    }

    db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&henergys, sizeof(henergys));
}

/**
*********************************************************************
* @brief：      获取总加组成员分钟电量
* @param[in]： tgParm  总加组参数
* @param[out]: actEnergy 有功电能
* @param[out]: reactEnergy 无功电能
* @Create    : 贺宁
* @Date      ：2020-04-15
* @return：     void
*********************************************************************
*/
void sta_virtual_member_min_energy_get(const STA_MTR_INFO_T *mtrInfo, const OOP_TG_T *tgParm, int64 *actEnergy, int64 *reactEnergy)
{
    if( tgParm->tgFlag == 1 ) //1-反向
    {
        //计算反向有功和无功
        sta_delta_1minen_read(mtrInfo, STA_NEG_ACTCS_ENERGY_OFFSET, actEnergy);

        sta_delta_1minen_read(mtrInfo, STA_NEG_RCTCS_ENERGY_OFFSET, reactEnergy);
    }
    else  /* 取正向（功率、电量）数据 0-正向*/ 
    {
        //计算正向有功和无功
        sta_delta_1minen_read(mtrInfo, STA_POS_ACTCS_ENERGY_OFFSET, actEnergy);

        sta_delta_1minen_read(mtrInfo, STA_POS_RCTCS_ENERGY_OFFSET, reactEnergy);            
    }
}

pthread_mutex_t g_fTG_flock[OOP_MAX_TG];

/**
*********************************************************************
* @brief：      存储总加分钟电能量，电控用
* @param[in]： cEnergy 当前电能(不使用，总加需按配置读取所有类型电量，无法统一描述)
* @param[in]： tgODA 总加组电能
* @return：     void
*********************************************************************
*/
void sta_tg_min_elec_init()
{
    uint8 i  = 0;

    for(i = 0; i < OOP_MAX_TG; i++)
    {
        pthread_mutex_init(&(g_fTG_flock[i]), NULL);
    }
}


/**
*********************************************************************
* @brief：      存储总加分钟电能量，电控用
* @param[in]： cEnergy 当前电能(不使用，总加需按配置读取所有类型电量，无法统一描述)
* @param[in]： tgODA 总加组电能
* @return：     void
*********************************************************************
*/
void sta_tg_min_elec_write(OOP_OAD_U tgOAD, int64 actEnergy[5])
{
    uint16  segOffset = 0;
    char    pfile[20] = {0};  //私有文件路径 根据测量点号得到的
    int ret = ERR_OK;
    uint8   tgactcs[41] = {0};  //带CS校验的差值数据

    sprintf(pfile,"stattgsets%d",tgOAD.nObjID&0xff);

    memcpy(tgactcs, actEnergy, STA_MIN_ENERGY_UNIT_SIZE);

    tgactcs[40] = sta_checksum_calc((uint8*)actEnergy, 40);

    //有功电量存储
    segOffset = STA_TGMINCS_ENERGY_DATA_SEG + STA_ACTCS_ENERGY_OFFSET;
    ret = write_pdata_xram(pfile, (char *)tgactcs, segOffset, STA_MINCS_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_tg_min_acte_write failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return;
    }

    g_tgStoreNo++;
}

/**
*********************************************************************
* @brief：       读取总加分钟电能量，控制用
* @param[in]：   tgODA     总加组OAD
* @param[out]： acteTotal 总加组有功总电能
* @param[out]： storeNo   存储序号
* @Create    :  贺宁
* @Date      ： 2020-04-20
* @return：      void
*********************************************************************
*/
int sta_tg_min_elec_read(OI tgOI, int64 *acteTotal, uint8 num)
{
    uint16  segOffset = 0;
    char    pfile[20] = {0};  //私有文件路径 根据测量点号得到的
    int ret = ERR_OK;
    uint8   tgactcs[41] = {0};  //带CS校验的差值数据
    uint8  cs = 0;

    sprintf(pfile,"stattgsets%d",tgOI&0xff);

    //有功电量读取
    segOffset = STA_TGMINCS_ENERGY_DATA_SEG + STA_ACTCS_ENERGY_OFFSET;
    ret = read_pdata_xram(pfile, (char *)tgactcs, segOffset, STA_MINCS_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_tg_min_acte_read failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return ret;        
    }

    cs = sta_checksum_calc(tgactcs, 40);
    if(tgactcs[40] != cs)
    {
        return ERR_CHK;
    }
    
    memcpy(acteTotal, tgactcs, num * sizeof(int64));

    return 0;
}


/**
*********************************************************************
* @brief：      存储总加分钟电能量，电控用
* @param[in]： cEnergy 当前电能(不使用，总加需按配置读取所有类型电量，无法统一描述)
* @param[in]： tgODA 总加组电能
* @return：     void
*********************************************************************
*/
void sta_tg_min_acte_write(OOP_OAD_U tgOAD, int64 actEnergy[5])
{
    uint8 index = 0;
    int64 totalEnergy[5] = {0};
    int   ret = 0;

    index = (tgOAD.nObjID & 0xff);
    if(0 == index || index > OOP_MAX_TG)
    {
        return;
    }

    PTHREAD_LOCK_SAFE(&(g_fTG_flock[index - 1]));
    ret = sta_tg_min_elec_read(tgOAD.nObjID, totalEnergy, 5);
    if(0 != ret)
    {
        memset(totalEnergy, 0, sizeof(totalEnergy));
    }
    totalEnergy[0] += actEnergy[0];
    totalEnergy[1] += actEnergy[1];
    totalEnergy[2] += actEnergy[2];
    totalEnergy[3] += actEnergy[3];
    totalEnergy[4] += actEnergy[4];

    sta_tg_min_elec_write(tgOAD, totalEnergy);

    PTHREAD_UNLOCK_SAFE(&(g_fTG_flock[index - 1]));
}

/**
*********************************************************************
* @brief：       读取总加分钟电能量，控制用
* @param[in]：   tgODA     总加组OAD
* @param[out]： acteTotal 总加组有功总电能
* @param[out]： storeNo   存储序号
* @Create    :  贺宁
* @Date      ： 2020-04-20
* @return：      void
*********************************************************************
*/
int sta_tg_min_acte_read(OI tgOI, int64 *acteTotal, uint32 *storeNo)
{
    uint8 index = 0;
    int64 totalEnergy[5] = {0};
    int   ret = 0;
    OOP_OAD_U tgOAD;

    index = (tgOI & 0xff);
    if(0 == index || index > OOP_MAX_TG)
    {
        return -1;
    }

    PTHREAD_LOCK_SAFE(&(g_fTG_flock[index - 1]));
    ret = sta_tg_min_elec_read(tgOI, totalEnergy, 5);
    if(0 == ret)
    {
        *acteTotal = totalEnergy[0];
    }
    else
    {
        *acteTotal = 0;
    }

    *storeNo = g_tgStoreNo;
    /* 清空, 防止复位后读到之前的电量 */
    if(0 != *acteTotal)
    {
        memset(totalEnergy, 0, sizeof(totalEnergy));
        tgOAD.value = 0x23010200;
        tgOAD.nObjID = tgOI;
        sta_tg_min_elec_write(tgOAD, totalEnergy);
    }
    PTHREAD_UNLOCK_SAFE(&(g_fTG_flock[index - 1]));

    return ret;
}

#endif

#if DESC("总加组电量计算",0)
void sta_virtual_energy_period_calc(const STA_MTR_INFO_T * mtrInfo,  uint32 dataOffset, OOP_OAD_U srcOAD, int64 *minEnergy)
{
    int     i;
    int64  rtData[5] = {0,0,0,0,0};     /*当前电表读数（8字节）*/
    int64  lastData[5] = {0,0,0,0,0};   /*前一分钟电表读数*/
    int64  rtEn[5] = {0,0,0,0,0};       /*分钟电量*/
    char    pfile[20];  //私有文件路径 根据测量点号得到的

    //不需要计算分钟电量，直接取上次计算的结果
    if(g_MinuteCalcEn == FALSE)
    {
        sta_lst_min_data_read(mtrInfo, dataOffset, minEnergy);    
        STAT_FMT_TRACE("minute energy no need to calculate.\n");    
        return;
    }

    /*-读取当前时刻采集区数据-*/
    if(sta_rt_data_read(mtrInfo, srcOAD, rtData) < 0) 
    {
        STAT_FMT_DEBUG("sta_virtual_rtdata_read failed.\n");
        return;
    }

    /*读取上次电能读数(运算基数)*/
    if(sta_lst_basic_data_read(mtrInfo, dataOffset, rtData, lastData)<0)
    {
        STAT_FMT_DEBUG("sta_virtual_lstdata_read failed.\n");
        return;
    }
    
    //计算分钟电量
    for(i = 0;i < 5;i++)
    {
        //当中间没有抄通电表的时候，rtData[i] == NONELLWORD
        if(rtData[i] == NONELLWORD)
        {
            rtData[i] = lastData[i];
        }
        
        if(rtData[i] < lastData[i]) /*当前读数比底数小的情况*/
            lastData[i] = rtData[i];

        if(rtData[i] >= lastData[i])
        {
            rtEn[i] = rtData[i] - lastData[i];  /*计算当前分钟电量*/
        }
        else                    
        {
            rtEn[i] = 9999999900LL - lastData[i] + 1 + rtData[i];   //电表溢出回零
        }
    }

#if TOTAL_EN_CHECK == 1
    //校验一下，防止总电量不等于分费率电量
    if(rtEn[0] != (rtEn[1] + rtEn[2] + rtEn[3] + rtEn[4]))
    {
        char tmpBuf[300];
        memset(tmpBuf, 0x0, sizeof(tmpBuf));
        sprintf(tmpBuf, "CalEnMin电量不等，已校正 rtEn = %lld %lld %lld %lld %lld", 
            rtEn[0], rtEn[1], rtEn[2], rtEn[3], rtEn[4]);
        writelog("/mnt/log/stat.log",tmpBuf);

        rtEn[0] = rtEn[1] + rtEn[2] + rtEn[3] + rtEn[4];
    }
#endif

    memcpy((char *)minEnergy, (char *)rtEn, sizeof(lastData));
    memcpy((char *)lastData, (char *)rtData, sizeof(lastData));

    //私有文件，485表需要64个，脉冲组8个，交采1个。
    switch(mtrInfo->type)
    {
        case MTR_AC:    sprintf(pfile,"statacsample");break;
        case MTR_485:   sprintf(pfile,"statmeter%d",mtrInfo->nIndex);break;
        case MTR_PULSE: sprintf(pfile,"statpluse%d",mtrInfo->pulseOAD.nObjID&0xf);break;
        default: return;
    }

    /*更新电量底数*/
    write_pdata_xram(pfile,(char *)lastData,dataOffset+STA_MIN_ENERGY_BASE_SEG,STA_MIN_ENERGY_UNIT_SIZE);
    /*保存分钟电量*/   
    write_pdata_xram(pfile,(char *)rtEn,dataOffset+STA_MIN_ENERGY_DATA_SEG,STA_MIN_ENERGY_UNIT_SIZE); 
}

/**
*********************************************************************
* @brief：      总加组成员的电量计算，函数每分钟运行一次，计算结果为分钟电量
* @param[in]： tgParm  总加组参数
* @param[out]: actEnergy 有功电能
* @param[out]: reactEnergy 无功电能
* @return：     void
*********************************************************************
*/
void sta_virtual_energy_member_calc(const STA_MTR_INFO_T *mtrInfo, const OOP_TG_T *tgParm, int64 *actEnergy, int64 *reactEnergy)
{
    OOP_OAD_U oad;

    //交采和485计量点分钟电量计算
    if((mtrInfo->type == MTR_AC)||(mtrInfo->type == MTR_485))
    {
        if( tgParm->tgFlag == 1 ) //1-反向
        {
            //计算反向有功和无功
            oad.value = 0x00200200;
            sta_virtual_energy_period_calc(mtrInfo, STA_NEG_ACT_ENERGY_OFFSET, oad, actEnergy);

            oad.value = 0x00400200;
            sta_virtual_energy_period_calc(mtrInfo, STA_NEG_RCT_ENERGY_OFFSET, oad, reactEnergy);
        }
        else  /* 取正向（功率、电量）数据 0-正向*/ 
        {
            //计算正向有功和无功
            oad.value = 0x00100200;
            sta_virtual_energy_period_calc(mtrInfo, STA_POS_ACT_ENERGY_OFFSET, oad, actEnergy);

            oad.value = 0x00300200;
            sta_virtual_energy_period_calc(mtrInfo, STA_POS_RCT_ENERGY_OFFSET, oad, reactEnergy);
        }
    }

    //脉冲计量点分钟电量计算
    if(mtrInfo->type == MTR_PULSE)
    {
        oad.value = mtrInfo->pulseOAD.value;
        if( tgParm->tgFlag == 1 ) //1-反向
        {
            //计算反向有功和无功
            oad.attID = 17; //反向有功电能视值
            sta_virtual_energy_period_calc(mtrInfo, STA_NEG_ACT_ENERGY_OFFSET, oad, actEnergy);

            oad.attID = 18; //反向无功电能视值
            sta_virtual_energy_period_calc(mtrInfo, STA_NEG_RCT_ENERGY_OFFSET, oad, reactEnergy);
        }
        else  /* 取正向（功率、电量）数据 0-正向*/ 
        {
            //计算正向有功和无功
            oad.attID = 15; //正向有功电能视值
            sta_virtual_energy_period_calc(mtrInfo, STA_POS_ACT_ENERGY_OFFSET, oad, actEnergy);

            oad.attID = 16; //正向无功电能视值
            sta_virtual_energy_period_calc(mtrInfo, STA_POS_RCT_ENERGY_OFFSET, oad, reactEnergy);
        }        
    } 
}

/**
*********************************************************************
* @brief：      从私有文件读取总加电能量
* @param[in]： cEnergy 当前电能(不使用，总加需按配置读取所有类型电量，无法统一描述)
* @param[in]： tgODA 总加组电能
* @return：     void
*********************************************************************
*/
int sta_virtual_energy_get(OOP_OAD_U tgOAD, int64 actEnergy[2][5], int64 rctEnergy[2][5])
{
    uint16  segOffset = 0;
    char    pfile[20];  //私有文件路径 根据测量点号得到的
    int ret = ERR_OK;

    sprintf(pfile,"stattgsets%d",tgOAD.nObjID&0xf);

    //日有功电量读取
    segOffset = STA_DAY_ENERGY_DATA_SEG + STA_ACT_ENERGY_OFFSET;
    ret = read_pdata_xram(pfile,(char *)actEnergy[0], segOffset, STA_MIN_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("read_pdata_xram failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return ret;        
    }

    //日无功电量读取
    segOffset = STA_DAY_ENERGY_DATA_SEG + STA_RCT_ENERGY_OFFSET;
    ret = read_pdata_xram(pfile,(char *)rctEnergy[0], segOffset, STA_MIN_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("read_pdata_xram failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return ret;        
    }

    //月有功电量读取
    segOffset = STA_MON_ENERGY_DATA_SEG + STA_ACT_ENERGY_OFFSET;
    ret = read_pdata_xram(pfile,(char *)actEnergy[1], segOffset, STA_MIN_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("read_pdata_xram failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return ret;        
    }

    //月无功电量读取
    segOffset = STA_MON_ENERGY_DATA_SEG + STA_RCT_ENERGY_OFFSET;
    ret = read_pdata_xram(pfile,(char *)rctEnergy[1], segOffset, STA_MIN_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("read_pdata_xram failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return ret;        
    }  

    return ret;
}

/**
*********************************************************************
* @brief：      存储总加电能量
* @param[in]： cEnergy 当前电能(不使用，总加需按配置读取所有类型电量，无法统一描述)
* @param[in]： tgODA 总加组电能
* @return：     void
*********************************************************************
*/
void sta_virtual_energy_backup(OOP_OAD_U tgOAD, int64 actEnergy[2][5], int64 rctEnergy[2][5])
{
    uint16  segOffset = 0;
    char    pfile[20];  //私有文件路径 根据测量点号得到的
    int ret = ERR_OK;

    sprintf(pfile,"stattgsets%d",tgOAD.nObjID&0xf);

    //过日，则清空日数据
    if(pastdayflag)
    {
        memset_s(actEnergy[0], sizeof(actEnergy[0]), 0, sizeof(actEnergy[0]));
        memset_s(rctEnergy[0], sizeof(rctEnergy[0]), 0, sizeof(rctEnergy[0]));        
    }

    //过月，则清空月数据
    if(pastmonflag)
    {
        memset_s(actEnergy[1], sizeof(actEnergy[1]), 0, sizeof(actEnergy[1]));
        memset_s(rctEnergy[1], sizeof(rctEnergy[1]), 0, sizeof(rctEnergy[1]));        
    }    

    //日有功电量存储
    segOffset = STA_DAY_ENERGY_DATA_SEG + STA_ACT_ENERGY_OFFSET;
    ret = write_pdata_xram(pfile,(char *)actEnergy[0], segOffset, STA_MIN_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("read_pdata_xram failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return;        
    }

    //日无功电量存储
    segOffset = STA_DAY_ENERGY_DATA_SEG + STA_RCT_ENERGY_OFFSET;
    ret = write_pdata_xram(pfile,(char *)rctEnergy[0], segOffset, STA_MIN_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("read_pdata_xram failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return;        
    }

    //月有功电量存储
    segOffset = STA_MON_ENERGY_DATA_SEG + STA_ACT_ENERGY_OFFSET;
    ret = write_pdata_xram(pfile,(char *)actEnergy[1], segOffset, STA_MIN_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("read_pdata_xram failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return;        
    }

    //月无功电量存储
    segOffset = STA_MON_ENERGY_DATA_SEG + STA_RCT_ENERGY_OFFSET;
    ret = write_pdata_xram(pfile,(char *)rctEnergy[1], segOffset, STA_MIN_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("read_pdata_xram failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return;        
    }       
}

/**
*********************************************************************
* @brief：      总加电能量计算（日月同时统计）
* @param[in]： tgODA 总加组对象
* @param[in]： minCalc 总加组对象
* @return：     void
*********************************************************************
*/
void sta_virtual_energy_calc(OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    int64 tgActAccEnergy[2][5] = {{0}};    /* 日（尖、峰、平、谷）电量总加变量(有功) */
    int64 tgRctAccEnergy[2][5] = {{0}};    /* 日（尖、峰、平、谷）电量总加变量(无功) */

    int64 actEnergy[5] = {0}; /* （尖、峰、平、谷）分钟电量变量(有功) */
    int64 rctEnergy[5] = {0}; /* （尖、峰、平、谷）分钟电量变量(无功) */

    OOP_TGSETS_T tgParam;    //总加组参数
    uint16 i;
    STA_MTR_INFO_T mtrInfo; //计量点信息
    OOP_PTCT_T ptct;        //互感器系数

    /*- 读参与总加的计量点配置信息 -*/
    ret = sta_virtual_param_read(tgOAD, &tgParam);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_virtual_param_read failed. ret(%d), tgOAD(0x%08x)\n", ret, tgOAD.value);
        return;
    }

    //读上次计算的总加组数据（总，尖、峰、平、谷）
    ret = sta_virtual_energy_get(tgOAD, tgActAccEnergy, tgRctAccEnergy);    
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_virtual_energy_read failed. ret(%d), tgOAD(0x%08x)\n", ret, tgOAD.value);
    }
    
    //遍历参与总加的电表(包括485，交采，脉冲)
    for(i = 0; i < tgParam.nNum; i++)
    {
        //获取计量点信息
        ret = sta_mtrtype_read(&tgParam.tg[i].add, &mtrInfo);
        if(ret != 0)
        {
            STAT_FMT_DEBUG("sta_mtrtype_read failed. ret(%d),tgid(%d),tgOAD(0x%8x)\n",ret, i, tgOAD.value);
            STAT_BUF_DEBUG(&tgParam.tg[i].add, sizeof(OOP_TSA_T), "tsa:");
            continue;
        }

        //获取互感器参数
        ret = sta_virtual_ptct_read(&mtrInfo, &ptct);
        if(ret != 0)
        {
            STAT_FMT_DEBUG("sta_virtual_ptct_read failed. ret(%d), mtrType(%d)\n",ret, mtrInfo.type);
            continue;
        }
        
        //获取有功无功电能数据
        //sta_virtual_energy_member_calc(&mtrInfo, &tgParam.tg[i], actEnergy, rctEnergy);
        sta_virtual_member_min_energy_get(&mtrInfo, &tgParam.tg[i], actEnergy, rctEnergy);
        
        //有功电能总加,这里要区分日数据和月数据
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, actEnergy, tgActAccEnergy[0], 5);
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, actEnergy, tgActAccEnergy[1], 5);
        
        //无功电能总加,这里要区分日数据和月数据        
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, rctEnergy, tgRctAccEnergy[0], 5);
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, rctEnergy, tgRctAccEnergy[1], 5);        
    }

    //存储总加结果
    sta_virtual_energy_write(tgOAD, tgActAccEnergy, tgRctAccEnergy); 

    //存储一份到私有文件
    sta_virtual_energy_backup(tgOAD, tgActAccEnergy, tgRctAccEnergy);
}
#endif

#if DESC("总加组功率计算",1)
/**
*********************************************************************
* @brief：      读取实时功率
* @param[in]： cEnergy  电量OAD
* @param[out]: actEnergy 有功电能
* @param[out]: reactEnergy 无功电能
* @Create    : 贺宁
* @Date      ：2020-04-17
* @return：     void 
*********************************************************************
*/
int sta_rt_power_read(const STA_MTR_INFO_T * mtrInfo, OOP_OAD_U srcOAD, int64 *rtPower)
{
    NOMAL_OAD_T nomalOAD = {0};       
    READ_RECORD_T readRecord;
    RECORD_UNIT_T recordUnit;
    OOP_INT4V_T  powerPhase = {0};
    int power = 0;
    int ret = 0;
    uint32 len = 0;

    MEMZERO(&readRecord, sizeof(readRecord));
    MEMZERO(&recordUnit, sizeof(recordUnit));
    
    if(mtrInfo->type == MTR_AC)
    {
        nomalOAD.logicId = 1;
        nomalOAD.oad.value = srcOAD.value;
        
        ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_INT4V_T), (uint8*)&powerPhase, &len);
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("db_read_nomal failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(int));
            return ret;
        }

        power = powerPhase.nValue[0];
    }
    else if(mtrInfo->type == MTR_PULSE)
    {
        nomalOAD.oad.value = srcOAD.value;
        ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(int), (uint8*)&power, &len);
        if(ret != ERR_OK)
        {
            STAT_FMT_TRACE("db_read_nomal failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(int));
            return ret;
        }
    }
    else if(mtrInfo->type == MTR_485) //采集的功率是按主OAD=0的记录型存储的
    {
        readRecord.recordOAD.road.oadCols.nNum = 1;
        readRecord.recordOAD.road.oadCols.oad[0].value = srcOAD.value; 
        readRecord.recordOAD.infoNum = mtrInfo->nIndex; //与电表TSA关联的配置序号
        readRecord.cType = COL_TM_END;
        readRecord.cEnd = time(NULL)+1 + COMPENSATION_TIME; // 注：+8个小时(28800)转成格林时间
        readRecord.cStart = readRecord.cEnd-86400;
        OOP_INT4V_T *ptPowerDate = NULL;

        //需要跟抄表模块对齐MAC校验算法
        readRecord.MAC.nNum = 6;
        memcpy(readRecord.MAC.value, mtrInfo->tsa.add, 6);  
        
        ret = db_read_record(g_staUdpHandle, &readRecord, sizeof(int), &recordUnit);
        if(ret != ERR_OK)
        {
            STAT_BUF_TRACE(readRecord.MAC.value, 6, "collect energy read failed. ret(%d) logicId(%d), infoNum(%d), option(%d), oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
               ret, readRecord.recordOAD.logicId, readRecord.recordOAD.infoNum, readRecord.recordOAD.optional, readRecord.recordOAD.road.oadMain.value, readRecord.recordOAD.road.oadCols.oad[0].value, sizeof(int));
            return ret;
        }
        ptPowerDate = (OOP_INT4V_T *)recordUnit.record[0].outDataBuf;
        power = ptPowerDate->nValue[0];        
    }
    else
    {
        ;//NOTHING TO DO
    }

    *rtPower = (int64)power;
    
    return ret;      
}

/**
*********************************************************************
* @brief：      写总加功率
* @param[in]： tgOAD 总加组OAD
* @param[in]： actData 有功数据
* @param[in]： rctData 无功数据
* @return：     void
*********************************************************************
*/
void sta_tg_power_write(OOP_OAD_U tgOAD, int64 actData, int64 rctData)
{
    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0};
    
    //存储总加组数据0x23010300（有功功率），0x23010400（无功功率）
    normal.classtag = CLASS_DATA_NORMAL;
    normal.oad.nObjID = tgOAD.nObjID;

    normal.oad.attID = 3;    
    ret = db_write_nomal(g_staUdpHandle, &normal, (uint8*)&actData, sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("tg power write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n", 
            ret, normal.logicId, normal.infoNum, normal.oad.value, sizeof(int64));
    }

    normal.oad.attID = 4; 
    ret = db_write_nomal(g_staUdpHandle, &normal, (uint8*)&rctData, sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("tg power write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n", 
            ret, normal.logicId, normal.infoNum, normal.oad.value, sizeof(int64));
    }

    return;
}

/**
*********************************************************************
* @brief：      总加组成员的功率读取
* @param[in]： tgParm  总加组参数
* @param[out]: actEnergy 有功电能
* @param[out]: reactEnergy 无功电能
* @return：     void
*********************************************************************
*/
void sta_tg_member_power_read(const STA_MTR_INFO_T *mtrInfo, const OOP_TG_T *tgParm, int64 *actData, int64 *rctData)
{
    OOP_OAD_U oad;

    //交采和485计量点分钟电量计算
    if((mtrInfo->type == MTR_AC)||(mtrInfo->type == MTR_485))
    {
        if( tgParm->tgFlag == 1 ) //1-反向
        {
            //有功功率
            oad.value = 0x20040200;
            sta_rt_power_read(mtrInfo, oad, actData);

            //反向功率一定是负，如果是正值，表示反向功率为0
            *actData = (*actData < 0)?(*actData):0;
            *actData = abs_int64(*actData);

            //无功功率
            oad.value = 0x20050200;
            sta_rt_power_read(mtrInfo, oad, rctData);
            
            //反向功率一定是负，如果是正值，表示反向功率为0
            *rctData = (*rctData < 0)?(*rctData):0;         
            *rctData = abs_int64(*rctData);
        }
        else  /* 取正向（功率、电量）数据 0-正向*/ 
        {
            //有功功率
            oad.value = 0x20040200;
            sta_rt_power_read(mtrInfo, oad, actData);

            //正向功率一定是正，如果是负值，表示正向功率为0
            *actData = (*actData > 0)?(*actData):0;

            //无功功率
            oad.value = 0x20050200;
            sta_rt_power_read(mtrInfo, oad, rctData);
            
            //正向功率一定是正，如果是负值，表示正向功率为0
            *rctData = (*rctData > 0)?(*rctData):0;
        }
    }

    //脉冲计量点分钟电量计算
    if(mtrInfo->type == MTR_PULSE)
    {
        oad.value = mtrInfo->pulseOAD.value;
        if( tgParm->tgFlag == 1 ) //1-反向
        {
            //有功功率
            oad.attID = 5;
            sta_rt_power_read(mtrInfo, oad, actData);

            //反向功率一定是负，如果是正值，表示反向功率为0
            *actData = (*actData < 0)?(*actData):0;
            *actData = abs_int64(*actData);

            //无功功率
            oad.attID = 6; 
            sta_rt_power_read(mtrInfo, oad, rctData);
            
            //反向功率一定是负，如果是正值，表示反向功率为0
            *rctData = (*rctData < 0)?(*rctData):0;       
            *rctData = abs_int64(*rctData);
        }
        else  /* 取正向（功率、电量）数据 0-正向*/ 
        {
            //有功功率
            oad.attID = 5;
            sta_rt_power_read(mtrInfo, oad, actData);

            //正向功率一定是正，如果是负值，表示正向功率为0
            *actData = (*actData > 0)?(*actData):0;

            //无功功率
            oad.attID = 6; 
            sta_rt_power_read(mtrInfo, oad, rctData);
            
            //正向功率一定是正，如果是负值，表示正向功率为0
            *rctData = (*rctData > 0)?(*rctData):0;
        }        
    } 
}

#endif

#if DESC("总加组相关计算",1)

/**
*********************************************************************
* @brief：      总加组滑差时间内平均功率计算,即功率的滑动平均值,滑差周期即滑动窗口大小
* @param[in]： tgODA 总加组对象
* @param[in]： tgActPower 总加组有功功率
* @param[in]： tgRctPower 总加组无功功率
* @Create    : 贺宁
* @Date      ：2020-04-17
* @return：     void
*********************************************************************
*/
void sta_tg_slip_power_calc(OOP_OAD_U tgOAD, int64 tgActPower, int64 tgRctPower)
{
    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0}; 
    uint8 slipPeriod = 0;           //滑差周期
    uint32 len = 0;
    int64 slipActPowerAve[8] = {0}; //滑差平均有功功率
    int64 slipRctPowerAve[8] = {0}; //滑差平均无功功率
    uint32 tgID = tgOAD.nObjID - 0x2301; //总加组ID
    uint32 slipID = g_slippower_fifo[tgID].nCount; //滑差数据索引    
    int slipCount = 0;     //控制滑差计算数据点数
    int i = 0;

    //读取滑差周期
    normal.oad.value = tgOAD.value;
    normal.oad.attID = 13;

    ret = db_read_nomal(g_staUdpHandle, &normal, sizeof(uint8), (uint8*)&slipPeriod, &len);
    if((ret != ERR_OK) || (slipPeriod == 0))
    {
        STAT_FMT_TRACE("slip period read failed, slip period: %d, default slip period = 1 min. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            slipPeriod, ret, normal.logicId, normal.infoNum, normal.oad.value, sizeof(uint8));

        slipPeriod = 1;
    }

    //缓存是滑动周期最大值，而不是根据滑差周期设置，是为了滑动周期变大变小的情况下都有数据可用
    if(g_slippower_fifo[tgID].nCount <= STA_SLIPPERIOD_MAX)
    {
        g_slippower_fifo[tgID].nCount++;
    }
    else //超过队列深度，则第一个数据出队，新的添加到尾部
    {
        memmove(&g_slippower_fifo[tgID].slipAct[0], &g_slippower_fifo[tgID].slipAct[1], STA_SLIPPERIOD_MAX*sizeof(int64));
        memmove(&g_slippower_fifo[tgID].slipRct[0], &g_slippower_fifo[tgID].slipRct[1], STA_SLIPPERIOD_MAX*sizeof(int64));
        slipID = STA_SLIPPERIOD_MAX;
    }

    //功率数据存储到缓存队列，数据初始化时缓存队列要清空
    g_slippower_fifo[tgID].slipAct[slipID] = tgActPower;
    g_slippower_fifo[tgID].slipRct[slipID] = tgRctPower;

    //计算滑动窗口内的累加值，数据个数未及滑差周期，则按现有个数计算，否则按滑差周期计算
    //由于参与MAX运算都是无符号数，所以MAX结果是无符号数，比较时，有符号的i会被提升成无符号数，所以要强转下
    slipCount = MAX(0, (int)(slipID-slipPeriod+1));
    if(slipID-slipCount+1 > slipPeriod)
    {
        STAT_FMT_DEBUG("总加组%d滑差点数异常 滑差周期 %d 滑差点数 %d\n", tgOAD.nObjID&0xff, slipPeriod, slipID-slipCount+1);
    }
    else
    {
        for(i = slipID; i >= slipCount; i--)
        {
            slipActPowerAve[tgID] += g_slippower_fifo[tgID].slipAct[i];
            slipRctPowerAve[tgID] += g_slippower_fifo[tgID].slipRct[i];
        }
    }

    slipActPowerAve[tgID] /= MIN(slipID+1, slipPeriod);
    slipRctPowerAve[tgID] /= MIN(slipID+1, slipPeriod);

    STAT_FMT_DEBUG("总加组%d滑差功率 有功 %lld 无功 %lld 滑差周期 %d 滑差点数 %d\n", tgOAD.nObjID&0xff, slipActPowerAve[tgID], slipRctPowerAve[tgID], slipPeriod, slipID-slipCount+1);
    
    normal.classtag = CLASS_DATA_NORMAL;

    normal.oad.attID = 5; //滑差有功   
    ret = db_write_nomal(g_staUdpHandle, &normal, (uint8*)&slipActPowerAve[tgID], sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("tg power write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n", 
            ret, normal.logicId, normal.infoNum, normal.oad.value, sizeof(int64));
    }

    normal.oad.attID = 6; //滑差无功
    ret = db_write_nomal(g_staUdpHandle, &normal, (uint8*)&slipRctPowerAve[tgID], sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("tg power write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n", 
            ret, normal.logicId, normal.infoNum, normal.oad.value, sizeof(int64));
    }

    return;
}

/**
*********************************************************************
* @brief：      总加组数据清零
* @param[in]： tgODA 总加组对象
* @param[in]： minCalc 总加组对象
* @Create    : 贺宁
* @Date      ：2020-04-17
* @return：     void
*********************************************************************
*/
void sta_tg_data_clear(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = 0;
    NOMAL_OAD_T nomalOAD = {0};
    RECORD_OAD_T recordOAD = {0};
    OOP_HENERGYS_T henergys;
    int oad_tmp[6] = {3,4,7,8,9,10};//用来存放要删的总加组曲线attID
    nomalOAD.oad.value = tgOAD.value;
    uint32 tgID = tgOAD.nObjID - 0x2301; //总加组ID
    int64 zeropower = 0;   //功率

    MEMZERO(&henergys, sizeof(henergys));
    henergys.nNum = 5;

    nomalOAD.oad.attID = 7;//日有功电量
    ret = db_write_nomal(fd, &nomalOAD, (uint8*)&henergys, sizeof(OOP_HENERGYS_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("day positive active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
    
        return;
    } 
    
    nomalOAD.oad.attID = 8;//日无功电量
    ret = db_write_nomal(fd, &nomalOAD, (uint8*)&henergys, sizeof(OOP_HENERGYS_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("day negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
    
        return;
    }

    nomalOAD.oad.attID = 9;//月有功电量
    ret = db_write_nomal(fd, &nomalOAD, (uint8*)&henergys, sizeof(OOP_HENERGYS_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("day positive active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
    
        return;
    } 
    
    nomalOAD.oad.attID = 10;//月无功电量
    ret = db_write_nomal(fd, &nomalOAD, (uint8*)&henergys, sizeof(OOP_HENERGYS_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("day negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
    
        return;
    } 

    recordOAD.classtag = CLASS_DATA_NORMAL;
    recordOAD.optional = 0;
    recordOAD.road.oadCols.nNum = 1;
    recordOAD.road.oadCols.oad[0].value = tgOAD.value;
    //删除曲线数据
    recordOAD.road.oadMain.value = 0x50020200;
    //分6次删除
    for(int tmp_index = 0; tmp_index < 6; tmp_index++ )
    {
        recordOAD.road.oadCols.oad[0].attID = oad_tmp[tmp_index];  
        ret = sta_delete_record(fd, &recordOAD);
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
        
            return;
        } 
    }

    //删除日冻结数据
    recordOAD.road.oadMain.value = 0x50040200;
    for(int tmp_index = 0; tmp_index < 6; tmp_index++ )
    {
        recordOAD.road.oadCols.oad[0].attID = oad_tmp[tmp_index];  
        ret = sta_delete_record(fd, &recordOAD);
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
        
            return;
        } 
    }

    //删除月冻结数据
    recordOAD.road.oadMain.value = 0x50060200;
    for(int tmp_index = 0; tmp_index < 6; tmp_index++ )
    {
        recordOAD.road.oadCols.oad[0].attID = oad_tmp[tmp_index];  
        ret = sta_delete_record(fd, &recordOAD);
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
        
            return;
        } 
    }

    //清空滑差功率队列
    MEMZERO(&g_slippower_fifo[tgID], sizeof(STA_SLIPPOWER_FIFO_T));

    //清零功率
    nomalOAD.classtag = CLASS_DATA_NORMAL;

    nomalOAD.oad.attID = 3; //有功   
    ret = db_write_nomal(fd, &nomalOAD, (uint8*)&zeropower, sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("tg power write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n", 
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(int64));
    }

    nomalOAD.oad.attID = 4; //无功
    ret = db_write_nomal(fd, &nomalOAD, (uint8*)&zeropower, sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("tg power write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n", 
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(int64));
    }

    nomalOAD.oad.attID = 5; //滑差有功   
    ret = db_write_nomal(fd, &nomalOAD, (uint8*)&zeropower, sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("tg power write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n", 
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(int64));
    }

    nomalOAD.oad.attID = 6; //滑差无功
    ret = db_write_nomal(fd, &nomalOAD, (uint8*)&zeropower, sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("tg power write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n", 
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(int64));
    }

    STAT_FMT_DEBUG("总加组%d已删除数据\n", tgOAD.nObjID&0xff);

    return;
}

/**
*********************************************************************
* @brief：      总加组计算（日月同时计算）
* @param[in]： tgODA 总加组对象
* @param[in]： minCalc 总加组对象
* @Create    : 贺宁
* @Date      ：2020-04-17
* @return：     void
*********************************************************************
*/
void sta_tg_data_calc(OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    int64 tgActAccPower = 0;   // 有功功率总加
    int64 tgRctAccPower = 0;   // 无功功率总加
    int64 actPower = 0;   // 总加成员有功功率
    int64 rctPower = 0;   // 总加成员无功功率  
    
    int64 tgActAccEnergy[STA_CALC_MAX][5] = {{0}};    /* 日（尖、峰、平、谷）电量总加变量(有功) */
    int64 tgRctAccEnergy[STA_CALC_MAX][5] = {{0}};    /* 日（尖、峰、平、谷）电量总加变量(无功) */

    int64 actEnergy[5] = {0}; /* （尖、峰、平、谷）分钟电量变量(有功) */
    int64 rctEnergy[5] = {0}; /* （尖、峰、平、谷）分钟电量变量(无功) */
    int64 tgActEnergy[5] = {0}; /* （尖、峰、平、谷）总加分钟电量变量(有功) */
    OOP_TGSETS_T tgParam;    //总加组参数
    uint16 i;
    STA_MTR_INFO_T mtrInfo = {0}; //计量点信息
    OOP_PTCT_T ptct;        //互感器系数
    char *mtrichr[7] = {"485", "模拟表", "脉冲", "虚拟", "交采", "载波", "载波多功能"};

    /*- 读参与总加的计量点配置信息 -*/
    ret = sta_virtual_param_read(tgOAD, &tgParam);
    if((ret != 0) || (tgParam.nNum == 0))
    {
        return;
    }

    //项数越界了
    if(tgParam.nNum > OOP_MAX_TGSETS)
    {
        STAT_FMT_DEBUG("tgOAD(0x%08x), tgNum(%d) > max(%d)\n", tgOAD.value, tgParam.nNum, OOP_MAX_TGSETS);
        return;
    }

    STAT_FMT_DEBUG("总加组%d计算：成员数 %d.\n", tgOAD.nObjID&0xff, tgParam.nNum);

    //读上次计算的总加电量数据（总，尖、峰、平、谷）
    ret = sta_virtual_energy_read(tgOAD, tgActAccEnergy, tgRctAccEnergy);    
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_virtual_energy_read failed. ret(%d), tgOAD(0x%08x)\n", ret, tgOAD.value);
    }
    
    //遍历参与总加的电表(包括485，交采，脉冲)
    for(i = 0; i < tgParam.nNum; i++)
    {
        //获取计量点信息
        ret = sta_mtrtype_read(&tgParam.tg[i].add, &mtrInfo);
        if(ret != 0)
        {
            STAT_FMT_DEBUG("sta_mtrtype_read failed. ret(%d),tgid(%d),tgOAD(0x%8x)\n",ret, i, tgOAD.value);
            STAT_BUF_DEBUG(&tgParam.tg[i].add, sizeof(OOP_TSA_T), "tsa:");
            continue;
        }

        //获取互感器参数
        ret = sta_virtual_ptct_read(&mtrInfo, &ptct);
        if(ret != 0)
        {
            STAT_FMT_DEBUG("sta_virtual_ptct_read failed. ret(%d), mtrType(%d)\n",ret, mtrInfo.type);
            continue;
        }

        if(mtrInfo.type == MTR_PULSE)
        {
            STAT_FMT_DEBUG("[%d]Pn %d %s %s %s PT %d CT %d\n", i, mtrInfo.pulseOAD.nObjID&0xff, mtrichr[mtrInfo.type-1], 
                (tgParam.tg[i].tgFlag==0)?"正向":"反向", (tgParam.tg[i].opFlag==0)?"加":"减", ptct.pt, ptct.ct);
        }
        else if(mtrInfo.type == MTR_485)
        {
            STAT_FMT_DEBUG("[%d]Pn %d %s %s %s PT %d CT %d\n", i, mtrInfo.nIndex, mtrichr[mtrInfo.type-1], 
                (tgParam.tg[i].tgFlag==0)?"正向":"反向", (tgParam.tg[i].opFlag==0)?"加":"减", ptct.pt, ptct.ct);
        }

        //每一次循环把临时变量清零
        rctPower = 0;
        actPower = 0;

        memset(actEnergy, 0, sizeof(actEnergy));
        memset(rctEnergy, 0, sizeof(rctEnergy));
        
        //获取实时功率数据
        sta_tg_member_power_read(&mtrInfo, &tgParam.tg[i], &actPower, &rctPower);
        //获取分钟电量数据
        sta_virtual_member_min_energy_get(&mtrInfo, &tgParam.tg[i], actEnergy, rctEnergy);

        STAT_FMT_DEBUG("[%d]成员功率 有功 %lld 无功 %lld\n", i, actPower, rctPower);
        
        STAT_FMT_DEBUG("[%d]成员电量 有功 总 %lld 费率1 %lld 费率2 %lld 费率3 %lld 费率4 %lld 无功 总 %lld 费率1 %lld 费率2 %lld 费率3 %lld 费率4 %lld\n", 
            i,actEnergy[0],actEnergy[1],actEnergy[2],actEnergy[3],actEnergy[4],
            rctEnergy[0],rctEnergy[1],rctEnergy[2],rctEnergy[3],rctEnergy[4]);
        
        //有功功率总加
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, &actPower, &tgActAccPower, 1);
        //无功功率总加        
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, &rctPower, &tgRctAccPower, 1);

        //有功电能总加,这里要区分日数据和月数据
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, actEnergy, tgActAccEnergy[STA_CALC_DAY], 5);
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, actEnergy, tgActAccEnergy[STA_CALC_MON], 5);
        
        //无功电能总加,这里要区分日数据和月数据        
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, rctEnergy, tgRctAccEnergy[STA_CALC_DAY], 5);
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, rctEnergy, tgRctAccEnergy[STA_CALC_MON], 5);            

        //有功分钟电量，控制用
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, actEnergy, tgActEnergy, 5);

    }

    STAT_FMT_DEBUG("总加组%d功率 有功 %lld 无功 %lld\n", tgOAD.nObjID&0xff, tgActAccPower, tgRctAccPower);

    STAT_FMT_DEBUG("总加组%d日电量 有功 总 %lld 费率1 %lld 费率2 %lld 费率3 %lld 费率4 %lld 无功 总 %lld 费率1 %lld 费率2 %lld 费率3 %lld 费率4 %lld\n", 
        tgOAD.nObjID&0xff, 
        tgActAccEnergy[STA_CALC_DAY][0],tgActAccEnergy[STA_CALC_DAY][1],tgActAccEnergy[STA_CALC_DAY][2],tgActAccEnergy[STA_CALC_DAY][3],tgActAccEnergy[STA_CALC_DAY][4],
        tgRctAccEnergy[STA_CALC_DAY][0],tgRctAccEnergy[STA_CALC_DAY][1],tgRctAccEnergy[STA_CALC_DAY][2],tgRctAccEnergy[STA_CALC_DAY][3],tgRctAccEnergy[STA_CALC_DAY][4]);

    STAT_FMT_DEBUG("总加组%d月电量 有功 总 %lld 费率1 %lld 费率2 %lld 费率3 %lld 费率4 %lld 无功 总 %lld 费率1 %lld 费率2 %lld 费率3 %lld 费率4 %lld\n", 
        tgOAD.nObjID&0xff, 
        tgActAccEnergy[STA_CALC_MON][0],tgActAccEnergy[STA_CALC_MON][1],tgActAccEnergy[STA_CALC_MON][2],tgActAccEnergy[STA_CALC_MON][3],tgActAccEnergy[STA_CALC_MON][4],
        tgRctAccEnergy[STA_CALC_MON][0],tgRctAccEnergy[STA_CALC_MON][1],tgRctAccEnergy[STA_CALC_MON][2],tgRctAccEnergy[STA_CALC_MON][3],tgRctAccEnergy[STA_CALC_MON][4]);

    STAT_FMT_DEBUG("总加组%d分钟电量 有功 总 %lld 费率1 %lld 费率2 %lld 费率3 %lld 费率4 %lld\n", 
        tgOAD.nObjID&0xff, tgActEnergy[0],tgActEnergy[1],tgActEnergy[2],tgActEnergy[3],tgActEnergy[4]);

    //存储总加分钟电量，控制用
    sta_tg_min_acte_write(tgOAD, tgActEnergy);

    //计算滑差平均功率
    sta_tg_slip_power_calc(tgOAD, tgActAccPower, tgRctAccPower);
    
    //存储总加功率
    sta_tg_power_write(tgOAD, tgActAccPower, tgRctAccPower); 
    
    //存储总加电量
    sta_virtual_energy_write(tgOAD, tgActAccEnergy, tgRctAccEnergy);     
}

/**
*********************************************************************
* @brief：      总加组计算
* @param[in]： tgODA 总加组对象
* @param[in]： minCalc 总加组对象
* @Create    : 贺宁
* @Date      ：2020-04-17
* @return：     void
*********************************************************************
*/
void sta_tgsets_data_calc(void)
{
    OOP_OAD_U oad = {0};
    uint32 i = 0;
//    uint8 cnt = 0;

//    //等待脉冲数据准备OK
//    while(!pulse_get_ok() && (cnt++ < 20))
//    {
//        usleep(100000);
//    }
//    
//    usleep(100000);
    
    //八个总加组的总加功率和电量计算
    oad.value = 0x23010200;
    for(i = 0; i < 8; i++)
    {        
        oad.nObjID = 0x2301 + i;

        sta_tg_data_calc(oad);
    }

    return;
}

#endif

#if DESC("新算法公共函数",1)
/**
*********************************************************************
* @brief：      统计周期处理函数提供统一的输入
* @param[in]： oad       待分析的OAD
* @param[out]：interval 分析出的间隔
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     void
*********************************************************************
*/
void sta_period_analyse(OOP_OAD_U oad, OOP_TI_SPAN_E *interval)
{
//    if(oad.nObjID == MT_FROZEN_SEC)
//    {
//        *interval = TI_SEC; 
//    }
//
//    if((oad.nObjID == MT_FROZEN_MIN)||(oad.nObjID == MT_REGION_MIN)
//        ||(oad.nObjID == MT_TOTALAVG_MIN)||(oad.nObjID == MT_EXTREME_MIN))
//    {
//        *interval = TI_MIN;    
//    }
//
//    if((oad.nObjID == MT_FROZEN_HOUR)||(oad.nObjID == MT_REGION_HOUR)
//        ||(oad.nObjID == MT_TOTALAVG_HOUR)||(oad.nObjID == MT_EXTREME_HOUR))
//    {
//        *interval = TI_HOUR;    
//    } 

    if((oad.nObjID == MT_FROZEN_DAY)||(oad.nObjID == MT_REGION_DAY)
        ||(oad.nObjID == MT_TOTALAVG_DAY)||(oad.nObjID == MT_EXTREME_DAY)
        ||(oad.nObjID == MT_FROZEN_SETTLING_DAY) || (oad.nObjID == MT_QR_DAY)
        ||(oad.nObjID == MT_POWERCUT_DAY))
    {
        *interval =TI_DAY;
    }

    if((oad.nObjID == MT_FROZEN_MON)||(oad.nObjID == MT_REGION_MON)
        ||(oad.nObjID == MT_TOTALAVG_MON)||(oad.nObjID == MT_EXTREME_MON)
        ||(oad.nObjID == MT_QR_MON)||(oad.nObjID == MT_POWERCUT_MON))
    {
        *interval = TI_MON;
    }

//    if((oad.nObjID == MT_FROZEN_YEAR)||(oad.nObjID == MT_REGION_YEAR)
//        ||(oad.nObjID == MT_TOTALAVG_YEAR)||(oad.nObjID == MT_EXTREME_YEAR))
//    {
//        *interval = TI_YEAR;    
//    }      
}

/**
*********************************************************************
* @brief：      周期计数更新
* @param[in]： interval 周期单位
* @param[in]： period   周期
* @param[out]：count     周期计数数
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     1-新周期 0-非新周期
*********************************************************************
*/
BOOLEAN sta_pericnt_update(OOP_TI_SPAN_E span, uint32 period, uint32 *periCnt, uint32 *freqCnt)
{
    BOOLEAN isUpdate = FALSE;

    switch(span)
    {
        case TI_SEC: break;
        case TI_MIN: if(pastminflag)  {(*periCnt)++;}break;
        case TI_HOUR:if(pasthourflag) {(*periCnt)++;}break;
        case TI_DAY: if(pastdayflag)  {(*periCnt)++;}break;
        case TI_MON: if(pastmonflag)  {(*periCnt)++;}break;
        default:break;
    }
    
    //达到周期则清零计数
    if(*periCnt == period)
    {
        STAT_FMT_DEBUG("span(%d), periCnt(%d) == period(%d)\n", span, *periCnt, period);
        
        *periCnt = 0;
        
        if(freqCnt != NULL) 
        {
            *freqCnt = 0;
        }

        isUpdate = TRUE;
    }

    return isUpdate;
}

/**
*********************************************************************
* @brief      ：读记录型数据
* @param[in]  ：clientid   数据中心句柄
* @param[in] ：recordOAD 记录型OAD信息
* @param[out] ：storeTm    存储时间
* @param[out] ：outData    输出的数据
* @param[out] ：outLen     输出缓冲区长度

* @Create     : 贺宁
* @Date       ：2020-11-26
* @return     ：void
*********************************************************************
*/
int sta_record_read(DB_CLIENT clientid, RECORD_OAD_T *recordOAD, uint32 *storeTm, void *outData, uint32 outLen)
{
    int ret = ERR_OK;
    READ_RECORD_T readRecord = {{0}};
    RECORD_UNIT_T recordBuf = {0};

    memcpy(&readRecord.recordOAD, recordOAD, sizeof(RECORD_OAD_T));

    readRecord.cType = COL_TM_STORE;
    readRecord.sortType = DOWN_SORT;
    readRecord.cEnd = time(NULL) + COMPENSATION_TIME+1; // 注：+8个小时(28800)转成格林时间，结束是开区间
    readRecord.cStart = (readRecord.cEnd/60)*60-60;  //筛选当前和上1分钟内的数据
    
    ret = db_read_record(clientid, &readRecord, outLen, &recordBuf);
    if(ret == ERR_OK)
    {
        if(outLen != recordBuf.record[0].outDatalen)
        {
            STAT_FMT_DEBUG("len(%d) != outDatelen(%d), oadMain(0x%08x), oadCols(0x%08x)\n", 
                outLen, recordBuf.record[0].outDatalen, readRecord.recordOAD.road.oadMain.value, readRecord.recordOAD.road.oadCols.oad[0].value);
        }

        *storeTm = recordBuf.colStoreTm;
            
        memcpy(outData, recordBuf.record[0].outDataBuf, outLen);
    }

    return ret;
}


/**
*********************************************************************
* @brief      ：读记录型数据
* @param[in]  ：clientid   数据中心句柄
* @param[in] ：recordOAD 记录型OAD信息
* @param[out] ：storeTm    存储时间
* @param[out] ：outData    输出的数据
* @param[out] ：outLen     输出缓冲区长度

* @Create     : 贺宁
* @Date       ：2020-11-26
* @return     ：void
*********************************************************************
*/
int sta_record_read_new(DB_CLIENT clientid, RECORD_OAD_T *recordOAD, uint32 *storeTm, void *outData, uint32 outLen)
{
    int ret = ERR_OK;
    READ_RECORD_T readRecord = {{0}};
    RECORD_UNIT_T recordBuf = {0};

    memcpy(&readRecord.recordOAD, recordOAD, sizeof(RECORD_OAD_T));

    readRecord.cType = COL_TM_STORE;
    readRecord.sortType = DOWN_SORT;
    readRecord.cStart = time(NULL) + COMPENSATION_TIME -3600;
    readRecord.cEnd = time(NULL) + COMPENSATION_TIME + 1; // 注：+8个小时(28800)转成格林时间
    
    ret = db_read_record(clientid, &readRecord, outLen, &recordBuf);
    if(ret == ERR_OK)
    {
        if(outLen != recordBuf.record[0].outDatalen)
        {
            STAT_FMT_DEBUG("len(%d) != outDatelen(%d), oadMain(0x%08x), oadCols(0x%08x)\n", 
                outLen, recordBuf.record[0].outDatalen, readRecord.recordOAD.road.oadMain.value, readRecord.recordOAD.road.oadCols.oad[0].value);
        }

        *storeTm = recordBuf.colStoreTm;
            
        memcpy(outData, recordBuf.record[0].outDataBuf, outLen);
    }

    return ret;
}

/**
*********************************************************************
* @brief：      写记录型数据
* @param[in] ：clientid  数据中心句柄
* @param[in] ：recordOAD 记录型OAD信息
* @param[in] ：storeTm   存储时间
* @param[in] ：inData     写入的数据
* @param[in] ：inLen      数据长度

* @Create    : 贺宁
* @Date      ：2020-11-26
* @return    ：void
*********************************************************************
*/
int sta_record_write(DB_CLIENT clientid, RECORD_OAD_T *recordOAD, uint32 storeTm, void *inData, uint32 inLen)
{
    WRITE_RECORD_T writeRecord;
    uint32 storeNo;

    MEMZERO(&writeRecord, sizeof(writeRecord));
    memcpy(&writeRecord.recordOAD, recordOAD, sizeof(RECORD_OAD_T));

    writeRecord.inDatalen = inLen;
    writeRecord.colStoreTm = storeTm + COMPENSATION_TIME; // 注：+8个小时(28800)转成格林时间
    writeRecord.colStartTm = writeRecord.colEndTm = writeRecord.colStoreTm;
    
    //传出时，按结构体强转
    memcpy(writeRecord.inDataBuf, (uint8*)inData, inLen);
    
    return db_write_record(clientid, &writeRecord, &storeNo);   
}

/**
*********************************************************************
* @brief：      更新记录型数据
* @param[in] ：clientid  数据中心句柄
* @param[in] ：recordOAD 记录型OAD信息
* @param[in] ：storeTm   存储时间
* @param[in] ：inData     写入的数据
* @param[in] ：inLen      数据长度

* @Create    : 贺宁
* @Date      ：2020-11-26
* @return    ：void
*********************************************************************
*/
int sta_record_update(DB_CLIENT clientid, RECORD_OAD_T *recordOAD, uint32 storeTm, void *inData, uint32 inLen)
{
    UPDATE_RECORD_T updateRecord;

    MEMZERO(&updateRecord, sizeof(updateRecord));
    memcpy(&updateRecord.recordOAD, recordOAD, sizeof(RECORD_OAD_T));

    updateRecord.filter.choice = 1;
    updateRecord.inDatalen = inLen;
    updateRecord.filter.storeTm = storeTm;
    
    //传出时，按结构体强转
    memcpy(updateRecord.inDataBuf, (uint8*)inData, inLen);
    
    return db_update_record(clientid, &updateRecord);   
}

#endif

#if DESC("新算法数适配函数",1)

/**
*********************************************************************
* @brief：      电压数据适配
* @param[in]： clientid  数据中心句柄
* @param[in]： oad        数据ID
* @param[out]：units      子项数
* @param[out]：digit      适配后的数据

* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     void
*********************************************************************
*/
int sta_volt_adapt(DB_CLIENT clientid, STA_MTR_INFO_T *mtrInfo, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit)
{
    int ret = ERR_OK;
    RECORD_OAD_T recordOAD = {0};
    OOP_WORD3V_T rtData = {0};     //当前实时数据
    uint32 storeTm = 0;
    int i = 0;

    recordOAD.infoNum = mtrInfo->nIndex;
    recordOAD.road.oadCols.nNum = 1;
    recordOAD.road.oadCols.oad[0].value = oad.value;

    /* 获取统计数据源 */
    ret = sta_record_read(clientid, &recordOAD, &storeTm, (uint8*)&rtData, sizeof(OOP_WORD3V_T));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("获取统计数据源 failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x).\n",
            ret, recordOAD.logicId, recordOAD.infoNum, recordOAD.road.oadCols.oad[0].value);
    
        return ret;
    }

    *units = rtData.nNum;

    for(i = 0; i < rtData.nNum; i++)
    {
        digit[i].choice = DT_LONG_UNSIGNED;
        digit[i].wVal = rtData.nValue[i];
    }

    return ret;
}

#endif

#if DESC("电表电压合格率统计",1)

//合格率统计：按照面向对象合格率统计思路定义统计参数,定义2192,2193为日月合格率统计
STA_QR_OBJ_T g_qr_objs[] = 
{
    /*dstOAD     srcOAD         periCnt  freqCnt  period   freq     upLimit         upRecov  dwLimit   dwRecov  mtAdapt  srcData  nIndex*/ 
    {{0x21920200},  {0x20000200},    0,   0,    {TI_DAY,1},  {TI_MIN,1}, {0}, NULL, 0, NULL, sta_volt_adapt, sta_qr_comm_calc},  //电压
};
const uint32 g_qr_objs_max = sizeof(g_qr_objs)/sizeof(g_qr_objs[0]);

/**
*********************************************************************
* @brief：      合格率计算
* @param[in]： rtData   数据
* @param[in]： mtrInfo  计量点信息
* @param[in]： object   统计对象
* @param[out]: result   计算结果
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     void
*********************************************************************
*/
void sta_qr_calc(OOP_DIGIT_T rtData, STA_MTR_INFO_T *mtrInfo, STA_QR_OBJ_T *object, OOP_QR_RESULT_T *result)
{    
    STAT_FMT_TRACE("qrstat: rtData(%llu), upLimit(%d), upRecov(%d), dwLimit(%d), dwRecov(%d)\n", 
        rtData.ullVal, object->param.upLimit, object->param.upRecov, object->param.dwLimit, object->param.dwRecov);

    //时间单位都是分钟
    result->mtTime += object->param.spantime;

    //抄不到的数据认为是合格的
    if(object->srcValid == FALSE)
    {
        result->mtState = MT_QSTATE_QR;
        result->qrTime += object->param.spantime;
    }
    else
    {
        //数据全部取最大类型
        if((rtData.ullVal > object->param.dwLimit) && (rtData.ullVal < object->param.upLimit))
        {
            result->mtState = MT_QSTATE_QR;
            result->qrTime += object->param.spantime;
        }
        
        //越上上限
        if(rtData.ullVal >= object->param.upupLimit)
        {
            result->mtState = MT_QSTATE_UP;
            result->upupTime += object->param.spantime;
        }
        else 
        {
            //没越上上限，判断是否越上限
            if(rtData.ullVal >= object->param.upLimit)
            {
                result->mtState = MT_QSTATE_UP;
                result->upTime += object->param.spantime;
            }
        }

        //下下限
        if(rtData.ullVal <= object->param.dwdwLimit)
        {
            result->mtState = MT_QSTATE_DW;
            result->dwdwTime += object->param.spantime;
        }
        else
        {
            //没越下下限判断是否越下限
            if(rtData.ullVal <= object->param.dwLimit)
            {
                result->mtState = MT_QSTATE_DW;
                result->dwTime += object->param.spantime;
            }
        }
    }
    
    //过日时一些容错处理
    if((pastdayflag)&&(object->period.unit == TI_DAY))
    {
        //反复对时不让过日时，有可能时间会大于1440分钟
        if(result->qrTime > 1440)
        {
            result->qrTime = 1440;
        }

        if((result->upTime + result->upupTime + result->dwTime + result->dwdwTime) < 1440)
        {
            result->qrTime = 1440-(result->upTime + result->upupTime + result->dwTime + result->dwdwTime);
        }

        result->mtTime = result->upTime + result->upupTime + result->dwTime + result->dwdwTime + result->qrTime;
    }
    
    //合格率和越限率四舍五入，精确到小数点后2位，首次读到的默认状态为合格
    result->upRate = (uint16)((float32)(result->upTime + result->upupTime)*10000/result->mtTime + 0.5);   
    result->dwRate = (uint16)((float32)(result->dwTime + result->dwdwTime)*10000/result->mtTime + 0.5);  
    result->qrRate = (uint16)((float32)result->qrTime*10000/result->mtTime + 0.5);    
    
    return;
}

/**
*********************************************************************
* @brief：      合格率统计记录
* @param[in]： clientid   数据中心句柄
* @param[in]： object      统计对象
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     void
*********************************************************************
*/
int sta_qr_comm_calc(DB_CLIENT clientid, STA_MTR_INFO_T *mtrInfo, void *pObject)
{
    int ret = ERR_OK;
    OOP_QR_RESULT_T result[3] = {0};
    STA_QR_OBJ_T *object = (STA_QR_OBJ_T *)pObject;    
    OOP_DIGIT_T *rtData = object->srcData;
    uint32 maxlen = sizeof(result);
    uint32 len = 0;
    NOMAL_OAD_T normalOAD = {0};
    int i = 0;
    RECORD_OAD_T recordOAD = {0};
    time_t curTime = 0;
    OOP_DATETIME_S_T tmpTime = {0};
    OOP_VOLTSTAT_T  voltObj = {0};//电压合格率对象

    memcpy(&tmpTime, &realstattime, sizeof(tmpTime));

    //停电不判断
    if((object->srcValid == TRUE)&&(rtData[0].ullVal == 0)&&(rtData[1].ullVal == 0)&&(rtData[2].ullVal == 0))
    {
        return 0;
    }

    normalOAD.logicId = 0;
    normalOAD.infoNum = mtrInfo->nIndex;
    normalOAD.oad.value = object->dstOAD.value;

    //读取历史数据
    ret = db_read_nomal(clientid, &normalOAD, maxlen, (uint8*)result, &len);
    if((ret != ERR_OK) || (len != maxlen))
    {
        STAT_FMT_DEBUG("合格率统计: history read failed. ret(%d) oadMain(0x%08x), infoNum(%d), maxLen(%d)\n", 
            ret, normalOAD.oad.value, normalOAD.infoNum, maxlen);

        //return ret;
    }

    //计算合格率结果
    for(i = 0; i < 3; i++)
    {
        //三相三相制B相不统计
        if((mtrInfo->pwrType == PWRTYPE_3PHASE3) && (i == 1))
        {
            //时间单位都是分钟
            result[i].mtTime += object->param.spantime;
            result[i].qrRate = 10000;
            continue;
        }
        
        sta_qr_calc(object->srcData[i], mtrInfo, object, &result[i]);

        STAT_FMT_TRACE("\r\n 合格率统计: freqCnt(%d): oadMain(0x%08x), oadCols(0x%08x), infoNum(%d)\
                        \r\n mtTime(%d), qrTime(%d) upTime(%d), dwTime(%d)\
                        \r\n upupTime(%d) dwdwTime(%d)\
                        \r\n qrRate(%d), upRate(%d) dwRate(%d)\
                        \r\n mtState(%d)\n", 
                        object->freqCnt, normalOAD.oad.value, object->srcOAD.value, normalOAD.infoNum, 
                        result[i].mtTime,result[i].qrTime,result[i].upTime,result[i].dwTime,
                        result[i].upupTime,result[i].dwdwTime,
                        result[i].qrRate,result[i].upRate,result[i].dwRate,
                        result[i].mtState);
    }
    
    //累积完成后回写
    ret = db_write_nomal(clientid, &normalOAD, (uint8*)result, maxlen);
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("合格率统计: write failed. ret(%d) oadMain(0x%08x), infoNum(%d), maxLen(%d)\n", 
           ret, normalOAD.oad.value, normalOAD.infoNum, maxlen);
    }

    /* 达到统计周期，冻结上周期数据，并清零实时值 */    
    if((pastdayflag) && (object->period.unit == TI_DAY))
    {
        recordOAD.road.oadMain.value = 0x50040200;
        tmpTime.hour = 0;
        tmpTime.minute = 0;
        tmpTime.second = 0;

        DT_DateTimeShort2Time(&tmpTime, &curTime);
        recordOAD.infoNum = mtrInfo->nIndex;
        recordOAD.road.oadCols.nNum = 1;
        recordOAD.road.oadCols.oad[0].value = 0x21310200;

        voltObj.day.wRate1 = result[0].qrRate;
        voltObj.day.wRate2 = result[0].upRate+result[0].dwRate;
        voltObj.day.dwTime1 = result[0].mtTime;
        voltObj.day.dwTime3 = result[0].upTime + result[0].upupTime;
        voltObj.day.dwTime4 = result[0].dwTime + result[0].dwdwTime;
        ret = sta_record_write(clientid, &recordOAD, curTime, &voltObj, sizeof(voltObj));
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("冻结: write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxLen(%d)\n", 
                ret, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, sizeof(voltObj));
        }
        
        recordOAD.road.oadCols.oad[0].value = 0x21320200;
        
        voltObj.day.wRate1 = result[1].qrRate;
        voltObj.day.wRate2 = result[1].upRate+result[1].dwRate;
        voltObj.day.dwTime1 = result[1].mtTime;
        voltObj.day.dwTime3 = result[1].upTime + result[1].upupTime;
        voltObj.day.dwTime4 = result[1].dwTime + result[1].dwdwTime;
        ret = sta_record_write(clientid, &recordOAD, curTime, &voltObj, sizeof(voltObj));
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("冻结: write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxLen(%d)\n", 
                ret, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, sizeof(voltObj));
        }

        recordOAD.road.oadCols.oad[0].value = 0x21330200;
        
        voltObj.day.wRate1 = result[2].qrRate;
        voltObj.day.wRate2 = result[2].upRate+result[2].dwRate;
        voltObj.day.dwTime1 = result[2].mtTime;
        voltObj.day.dwTime3 = result[2].upTime + result[2].upupTime;
        voltObj.day.dwTime4 = result[2].dwTime + result[2].dwdwTime;
        ret = sta_record_write(clientid, &recordOAD, curTime, &voltObj, sizeof(voltObj));
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("冻结: write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxLen(%d)\n", 
                ret, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, sizeof(voltObj));
        }

        //清零记录
        memset(&result, 0, sizeof(result));
        ret = db_write_nomal(clientid, &normalOAD, (uint8*)result, maxlen);

        STAT_FMT_DEBUG("合格率统计: new cycle begin. ret(%d) oadMain(0x%08x), maxLen(%d)\n", 
           ret, normalOAD.oad.value, maxlen);
    }    
    
    return ret;     
}

/**
*********************************************************************
* @name      : sta_qr_dispatch
* @brief     ：分派合格率统计任务
* @param[in] ：clientid   数据中心句柄
* @param[in] ：object      统计对象
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return    ：void
*********************************************************************
*/
void sta_qr_dispatch(DB_CLIENT clientid, STA_MTR_INFO_T *mtrInfo, STA_QR_OBJ_T *object)
{
    int ret = ERR_OK;
    OOP_DIGIT_T srcData[128] = {0};
    
    object->srcData = srcData;
    object->srcValid = FALSE;

    //数据格式统一接口
    if(object->dataAdapt != NULL)
    {
        ret = object->dataAdapt(clientid, mtrInfo, object->srcOAD, &object->srcUnits, srcData);
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("dataAdapt failed. ret(%d), oad(0x%08x)\n", ret, object->srcOAD.value);
        }
        else
        {
            object->srcValid = TRUE;
        }
    }
    else
    {
        STAT_FMT_DEBUG("oad(0x%08x) qr calc not support.\n", object->srcOAD.value);
        return;
    }

    //统计计算接口
    if(object->calcAdapt != NULL)
    {
        ret = object->calcAdapt(clientid, mtrInfo, object);
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("calcAdapt failed. ret(%d), oad(0x%08x)\n", ret, object->srcOAD.value);
            return;
        }
    }
    else
    {
        STAT_FMT_DEBUG("oad(0x%08x) qr calc not support.\n", object->srcOAD.value);
        return;
    }

    return;  
}

/**
*********************************************************************
* @name      : sta_qr_entry
* @brief     ：合格率统计入口
* @param[in] ：clientid 数据中心句柄
* @Create    : 贺宁
* @Date      ：2022-11-03
* @return    ：void
*********************************************************************
*/
void sta_qr_entry(DB_CLIENT clientid)
{
    uint32 i = 0, j = 0;
    STA_QR_OBJ_T *tmpNode = NULL;
    OOP_TI_SPAN_E time_span;
    STA_MTR_INFO_T mtrInfo; //计量点信息
    OOP_METER_T meter;
    OOP_VOLTPARAM_T voltParam;   //电压合格率参数    
    int alterTime[STA_CALC_MAX] = {0};    //2个元素，0-日，1-月
    int ret = 0;

    STAT_FMT_TRACE("****** 合格率统计开始 ****** \n");

    for(i = 0; i < g_qr_objs_max; i++)
    {
        tmpNode = &g_qr_objs[i];

        //统计周期分析
        sta_period_analyse(tmpNode->dstOAD, &time_span);

        //读取电压合格率参数
        ret = sta_volt_param_read(&voltParam);
        if(ret != 0)
        {
            return;
        }
        
        //分析走过的时间
        sta_volt_monitor_time(alterTime);

        tmpNode->param.upupLimit = voltParam.uupVolt;
        tmpNode->param.upLimit = voltParam.upVolt;
        tmpNode->param.dwdwLimit = voltParam.lldVolt;
        tmpNode->param.dwLimit = voltParam.ldVolt;
        tmpNode->param.spantime = alterTime[0];

        //统计计量点信息-交采和485才统计合格率
        for(j = 0; j < oopMeterList.oopMeterNum; j++)
        {
            if(app_get_meter_list_by_id(j, &meter) == 0)
            {
                if(meter.basic.port.nObjID == 0xf201 && 
                    (meter.basic.protocol == PROTO_97 || meter.basic.protocol == PROTO_07))
                {
                        mtrInfo.type = MTR_485;
                }
                else
                {
                    continue;
                }
                
                mtrInfo.nIndex = meter.nIndex;
                mtrInfo.pwrType = meter.basic.pwrType;
                memcpy(&mtrInfo.tsa, &meter.basic.tsa, sizeof(OOP_TSA_T));

                //合格率统计分派函数
                sta_qr_dispatch(clientid, &mtrInfo, tmpNode);
            }
        }
         
        tmpNode->freqCnt++;

        //刷新周期数
        sta_pericnt_update(time_span, tmpNode->period.value, &tmpNode->periCnt, &tmpNode->freqCnt);           
    }

    STAT_FMT_TRACE("****** 合格率统计结束 ****** \n\n");
    
    return;

}

#endif

#if DESC("终端统计",1)

/**
*********************************************************************
* @brief：      终端累计供电时间(日月同时计算)
* @param[in]： mtrid    计量点号
* @param[in]： srcOAD  源数据OAD
* @param[in]： dstOAD  目的数据OAD
* @return：     void
*********************************************************************
*/
void sta_tmn_power_acctime(void)
{
    uint32 wAccMin[STA_CALC_MAX] = {0}; /*日-0,月-1,累计供电时间（分钟）*/
    NOMAL_OAD_T nomalOAD = {0}; 
    uint32 len = 0;
    uint32 ret = 0;
    static int resetflag = 1;
    time_t curTime = 0, lstTime = 0;
    uint32 restartMin = 0;
    OOP_DATETIME_S_T  restartTime = {0};
    int retr = 0;

    nomalOAD.oad.value = 0x22030200;
    nomalOAD.classtag = CLASS_DATA_NORMAL;
    
    /* 获取上次终端供电时间 */
    ret =  db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(wAccMin), (uint8*)wAccMin, &len);
    
    //过日清日数据
    if((pastdayflag) || (ret != 0))
    {
        STAT_FMT_DEBUG("past day or ret = %d, clear day power acctime %d->0.\n", ret, wAccMin[STA_CALC_DAY]);
        wAccMin[STA_CALC_DAY] = 0;
        retr = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)wAccMin, sizeof(wAccMin));
        if(retr != 0)
        {
            STAT_FMT_DEBUG("clear day power acctime failed. ret(%d).\n", retr);
        }
    }

    //过月清月数据
    if((pastmonflag) || (ret != 0))
    {
        STAT_FMT_DEBUG("past mon or ret = %d, clear mon power acctime %d->0.\n", ret, wAccMin[STA_CALC_MON]);
        wAccMin[STA_CALC_MON] = 0;
        retr = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)wAccMin, sizeof(wAccMin));
        if(retr != 0)
        {
            STAT_FMT_DEBUG("clear mon power acctime failed. ret(%d).\n", retr);
        }        
    }

    //过分累加
    if(!pastminflag)
    {
        return;
    }

    //重启后第一次统计需要算入重启时间
    if(resetflag)
    {
        resetflag = 0;
        DT_DateTimeShort2Time(&realstattime, &curTime);
        ret = read_pdata_xram("statpdata",(char *)&restartTime,sizeof(restartTime),sizeof(restartTime));
        DT_DateTimeShort2Time(&restartTime, &lstTime);

        STAT_FMT_DEBUG("ret(%d), offset(%d), restart time(%4d-%02d-%02d %02d:%02d:%02d), sys time(%4d-%02d-%02d %02d:%02d:%02d)\n", 
            ret, sizeof(restartTime), restartTime.year, restartTime.month, restartTime.day, 
            restartTime.hour, restartTime.minute, restartTime.second,
            realstattime.year, realstattime.month, realstattime.day, 
            realstattime.hour, realstattime.minute, realstattime.second);

        restartMin = (curTime-(lstTime/60)*60)/60;

        //防错处理，重启时间不能大于2分钟，就是真的重启消耗了2分钟，那也是不符合测试要求的
        if(restartMin > 2)
        {
            restartMin = 2;
            STAT_FMT_DEBUG("abnormal restart time, correct to 2 min.\n");
        }
        else if(0 == restartMin)
        {
            /* 防止终端启动太快 0分钟计算成1min */
            restartMin = 1;
            STAT_FMT_DEBUG("abnormal restart time, correct to 1 min.\n");
        }

        wAccMin[STA_CALC_DAY] += restartMin;
        wAccMin[STA_CALC_MON] += restartMin;

        STAT_FMT_DEBUG("restart used (curTime:%d-(lstTime:%d/60)*60)/60 = %d min\n", curTime, lstTime, restartMin);
    }
    else
    {
        wAccMin[STA_CALC_DAY]++;
        wAccMin[STA_CALC_MON]++;  
    }
        
    if(wAccMin[STA_CALC_DAY] > 1440)  /*日供电上限1440分钟*/
    {
        STAT_FMT_DEBUG("check day power acctime %d.\n", wAccMin[STA_CALC_DAY]);
        wAccMin[STA_CALC_DAY] = 1440; /*时间限定在上限以下*/
    }

    if(wAccMin[STA_CALC_MON] > 44640) /*月供电上限44640分钟*/
    {
        STAT_FMT_DEBUG("check mon power acctime %d.\n", wAccMin[STA_CALC_MON]);
        wAccMin[STA_CALC_MON] = 44640; /*时间限定在上限以下*/
    }

    //存储数据
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)wAccMin, sizeof(wAccMin));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("tmn power time write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    }
    
    STAT_FMT_TRACE("tmnPowerSupply[day]=%d, tmnPowerSupply[mon]=%d\n", wAccMin[0],wAccMin[1]);

    return;
}


/**
*********************************************************************
* @brief：      终端累计复位次数(日月同时计算)
* @param[in]： mtrid    计量点号
* @param[in]： srcOAD  源数据OAD
* @param[in]： dstOAD  目的数据OAD
* @return：     void
*********************************************************************
*/
void sta_tmn_reset_acctimes(void)
{
    uint16  resetAcc[STA_CALC_MAX] = {0};
    static int resetflag = 1;
    NOMAL_OAD_T nomalOAD = {0}; 
    uint32 len = 0;
    int ret = 0;
    int retr = 0;

    nomalOAD.oad.value = 0x22040200;
    nomalOAD.classtag = CLASS_DATA_NORMAL;

    /* 获取上次复位次数 */
    ret =  db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(resetAcc), (uint8*)resetAcc, &len); 
    if(ret != 0)
    {
        STAT_FMT_DEBUG("read power acctime failed, regarded as reset, ret(%d).\n", ret);
        resetflag = 1; //获取不到等同于一次重启
    }
        
    if((pastdayflag) || (ret != 0))
    {
        STAT_FMT_DEBUG("past day or ret = %d, clear day reset acctime %d->0.\n",ret,resetAcc[STA_CALC_DAY]);
        resetAcc[STA_CALC_DAY]  = 0;
        retr = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)resetAcc, sizeof(resetAcc));
        if(retr != 0)
        {
            STAT_FMT_DEBUG("clear day power acctime failed. ret(%d).\n", retr);
        }        
    }
    
    if((pastmonflag) || (ret != 0))
    {
        STAT_FMT_DEBUG("past mon or ret = %d, clear mon reset acctime.%d->0\n",ret, resetAcc[STA_CALC_MON]);
        resetAcc[STA_CALC_MON]  = 0; 
        retr = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)resetAcc, sizeof(resetAcc));
        if(retr != 0)
        {
            STAT_FMT_DEBUG("clear mon power acctime failed. ret(%d).\n", retr);
        }        
    }
    
    //未重启则返回
    if(!resetflag) 
    {
        return;
    }
    
    resetAcc[STA_CALC_DAY]++; /*日复位次数累加1*/
    resetAcc[STA_CALC_MON]++; /*月复位次数累加1*/ 
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)resetAcc, sizeof(resetAcc));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("tmn reset acc time write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    } 
    
    resetflag = 0;

    STAT_FMT_TRACE("tmnResetTimes[day]=%d, tmnResetTimes[mon]=%d\n", resetAcc[0],resetAcc[1]);  

    return;   
}

extern void comm_flow_get(uint8 bPPP, uint8 index, uint32 *pSendflow, uint32 *pRecvflow);

/**
*********************************************************************
* @brief：      终端通信流量(日月同时计算)
* @param[in]： mtrid    计量点号
* @param[in]： srcOAD  源数据OAD
* @param[in]： dstOAD  目的数据OAD
* @return：     void
*********************************************************************
*/
void sta_tmn_comm_traffic(void)
{
    uint32  traffic[STA_CALC_MAX] = {0};
    uint32 sendFlow = 0;
    uint32 recvFlow = 0; 
    NOMAL_OAD_T nomalOAD = {0}; 
    uint32 len = 0;
    int ret = 0;
    int retr = 0;

    nomalOAD.oad.value = 0x22000200;
    nomalOAD.classtag = CLASS_DATA_NORMAL;

    /* 获取上次通信流量 */
    ret =  db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(traffic), (uint8*)traffic, &len);   
    
    if((pastdayflag) || (ret != 0))
    {
        STAT_FMT_DEBUG("past day or ret = %d, clear day comm traffic %d->0.\n",ret,traffic[STA_CALC_DAY]);
        traffic[STA_CALC_DAY]  = 0;
        retr = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)traffic, sizeof(traffic));
        if(retr != 0)
        {
            STAT_FMT_DEBUG("clear day comm traffic failed. ret(%d).\n", retr);
        }           
    }
    
    if((pastmonflag) || (ret != 0))
    {
        STAT_FMT_DEBUG("past mon or ret = %d, clear mon comm traffic %d->0.\n",ret,traffic[STA_CALC_MON]);
        traffic[STA_CALC_MON]  = 0; 
        retr = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)traffic, sizeof(traffic));
        if(retr != 0)
        {
            STAT_FMT_DEBUG("clear mon comm traffic failed. ret(%d).\n", retr);
        }          
    }

    //读取无线通信流量
    comm_flow_get(TRUE, 0, &sendFlow, &recvFlow);
    
    traffic[STA_CALC_DAY] += sendFlow + recvFlow; /*日通信流量累加*/
    traffic[STA_CALC_MON] += sendFlow + recvFlow; /*月通信流量累加*/ 
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)traffic, sizeof(traffic));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_tmn_comm_traffic write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    } 

    STAT_FMT_TRACE("tmnTraffic[day]=%d, tmnTraffic[mon]=%d\n", traffic[0],traffic[1]);  

    return;
}

#endif


