/*
*********************************************************************
* @filename: real_gather.c
* @brief   : 实时数据采集
* @describe: 
* @history: 
*    auther      date          describe
*    王津剑      2021-6-28     create
*********************************************************************
*/
#include "baseCommon.h"
#include "cJSON.h"
#include "oopType.h"

#include "min_gather.h"
#include "comm_gather.h"

/**********************************************************************
* @name      : min_gather_real_setfflag
* @brief     ：实时数据设置标记
* @param[in] ：MG_REAL_NODE_T *pDataNode 数据单元
               uint32 type               数据标识(MIN_GATHER_OAD_E)
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-6-29
* @Update    :
**********************************************************************/
void min_gather_real_setfflag(MG_REAL_NODE_T *pDataNode, uint32 type)
{
    switch(type)
    {
        case MIN_GATHER_OAD_20000201:
        {
            pDataNode->dataflag[0] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_20000202:
        {
            pDataNode->dataflag[1] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_20000203:
        {
            pDataNode->dataflag[2] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_20010201:
        {
            pDataNode->dataflag[3] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_20010202:
        {
            pDataNode->dataflag[4] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_20010203:
        {
            pDataNode->dataflag[5] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_20010400:
        {
            pDataNode->dataflag[6] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_20010500:
        {
            pDataNode->dataflag[7] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_20040201:
        {
            pDataNode->dataflag[8] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_20040202:
        {
            pDataNode->dataflag[9] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_20040203:
        {
            pDataNode->dataflag[10] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_20040204:
        {
            pDataNode->dataflag[11] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_43140506:
        {
            pDataNode->dataflag[12] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_43140505:
        {
            pDataNode->dataflag[13] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_43140501:
        {
            pDataNode->dataflag[14] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_43140400:
        {
            pDataNode->dataflag[15] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_43110502:
        {
            pDataNode->dataflag[16] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_43110400:
        {
            pDataNode->dataflag[17] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_00100201:
        {
            pDataNode->dataflag[18] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_00100203:
        {
            pDataNode->dataflag[19] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_00100205:
        {
            pDataNode->dataflag[20] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_00200201:
        {
            pDataNode->dataflag[21] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_00200203:
        {
            pDataNode->dataflag[22] = TRUE;
            break;
        }
        case MIN_GATHER_OAD_00200205:
        {
            pDataNode->dataflag[23] = TRUE;
            break;
        }
        default: return;
    }
}

/**********************************************************************
* @name      : min_gather_real_setfflag
* @brief     ：实时数据设置标记
* @param[in] ：MG_REAL_NODE_T *pDataNode 数据单元
               uint32 type               数据标识(MIN_GATHER_OAD_E)
               uint16 buflen             缓存大小
* @param[out]：char *buf                 数据缓存
* @return    ：长度
* @Create    : 王津剑
* @Date      ：2021-6-29
* @Update    :
**********************************************************************/
int min_gather_real_data_str(MG_REAL_NODE_T *pDataNode, uint32 type, char *buf, uint16 buflen)
{
    int offset = 0;
    offset = sprintf(buf, ",");
    switch(type)
    {
        case MIN_GATHER_OAD_20000201:
        {
            if(TRUE == pDataNode->dataflag[0])
            {
                offset += sprintf(buf + offset, "%d.%d", pDataNode->data_20000201 / 10, pDataNode->data_20000201 % 10);
            }
            break;
        }
        case MIN_GATHER_OAD_20000202:
        {
            if(TRUE == pDataNode->dataflag[1])
            {
                offset += sprintf(buf + offset, "%d.%d", pDataNode->data_20000202 / 10, pDataNode->data_20000202 % 10);
            }
            break;
        }
        case MIN_GATHER_OAD_20000203:
        {
            if(TRUE == pDataNode->dataflag[2])
            {
                offset += sprintf(buf + offset, "%d.%d", pDataNode->data_20000203 / 10, pDataNode->data_20000203 % 10);
            }
            break;
        }
        case MIN_GATHER_OAD_20010201:
        {
            if(TRUE == pDataNode->dataflag[3])
            {
                uint32 datatemp = 0;
                if(pDataNode->data_20000203 >= 0)
                {
                    datatemp = pDataNode->data_20000203;
                    offset += sprintf(buf + offset, "%d.%03d", datatemp / 1000, datatemp % 1000);
                }
                else
                {
                    datatemp = 0 - pDataNode->data_20000203;
                    offset += sprintf(buf + offset, "-%d.%03d", datatemp / 1000, datatemp % 1000);
                }
            }
            break;
        }
        case MIN_GATHER_OAD_20010202:
        {
            if(TRUE == pDataNode->dataflag[4])
            {
                uint32 datatemp = 0;
                if(pDataNode->data_20010202 >= 0)
                {
                    datatemp = pDataNode->data_20010202;
                    offset += sprintf(buf + offset, "%d.%03d", datatemp / 1000, datatemp % 1000);
                }
                else
                {
                    datatemp = 0 - pDataNode->data_20010202;
                    offset += sprintf(buf + offset, "-%d.%03d", datatemp / 1000, datatemp % 1000);
                }
            }
            break;
        }
        case MIN_GATHER_OAD_20010203:
        {
            if(TRUE == pDataNode->dataflag[5])
            {
                uint32 datatemp = 0;
                if(pDataNode->data_20010203 >= 0)
                {
                    datatemp = pDataNode->data_20010203;
                    offset += sprintf(buf + offset, "%d.%03d", datatemp / 1000, datatemp % 1000);
                }
                else
                {
                    datatemp = 0 - pDataNode->data_20010203;
                    offset += sprintf(buf + offset, "-%d.%03d", datatemp / 1000, datatemp % 1000);
                }
            }
            break;
        }
        case MIN_GATHER_OAD_20010400:
        {
            if(TRUE == pDataNode->dataflag[6])
            {
                uint32 datatemp = 0;
                if(pDataNode->data_20010400 >= 0)
                {
                    datatemp = pDataNode->data_20010400;
                    offset += sprintf(buf + offset, "%d.%03d", datatemp / 1000, datatemp % 1000);
                }
                else
                {
                    datatemp = 0 - pDataNode->data_20010400;
                    offset += sprintf(buf + offset, "-%d.%03d", datatemp / 1000, datatemp % 1000);
                }
            }
            break;
        }
        case MIN_GATHER_OAD_20010500:
        {
            if(TRUE == pDataNode->dataflag[7])
            {
                uint32 datatemp = 0;
                if(pDataNode->data_20010500 >= 0)
                {
                    datatemp = pDataNode->data_20010500;
                    offset += sprintf(buf + offset, "%d.%03d", datatemp / 1000, datatemp % 1000);
                }
                else
                {
                    datatemp = 0 - pDataNode->data_20010500;
                    offset += sprintf(buf + offset, "-%d.%03d", datatemp / 1000, datatemp % 1000);
                }
            }
            break;
        }
        case MIN_GATHER_OAD_20040201:
        {
            if(TRUE == pDataNode->dataflag[8])
            {
                uint32 datatemp = 0;
                if(pDataNode->data_20040201 >= 0)
                {
                    datatemp = pDataNode->data_20040201;
                    offset += sprintf(buf + offset, "%d.%d", datatemp / 10, datatemp % 10);
                }
                else
                {
                    datatemp = 0 - pDataNode->data_20040201;
                    offset += sprintf(buf + offset, "-%d.%d", datatemp / 10, datatemp % 10);
                }
            }
            break;
        }
        case MIN_GATHER_OAD_20040202:
        {
            if(TRUE == pDataNode->dataflag[9])
            {
                uint32 datatemp = 0;
                if(pDataNode->data_20040202 >= 0)
                {
                    datatemp = pDataNode->data_20040202;
                    offset += sprintf(buf + offset, "%d.%d", datatemp / 10, datatemp % 10);
                }
                else
                {
                    datatemp = 0 - pDataNode->data_20040202;
                    offset += sprintf(buf + offset, "-%d.%d", datatemp / 10, datatemp % 10);
                }
            }
            break;
        }
        case MIN_GATHER_OAD_20040203:
        {
            if(TRUE == pDataNode->dataflag[10])
            {
                uint32 datatemp = 0;
                if(pDataNode->data_20040203 >= 0)
                {
                    datatemp = pDataNode->data_20040203;
                    offset += sprintf(buf + offset, "%d.%d", datatemp / 10, datatemp % 10);
                }
                else
                {
                    datatemp = 0 - pDataNode->data_20040203;
                    offset += sprintf(buf + offset, "-%d.%d", datatemp / 10, datatemp % 10);
                }
            }
            break;
        }
        case MIN_GATHER_OAD_20040204:
        {
            if(TRUE == pDataNode->dataflag[11])
            {
                uint32 datatemp = 0;
                if(pDataNode->data_20040204 >= 0)
                {
                    datatemp = pDataNode->data_20040204;
                    offset += sprintf(buf + offset, "%d.%d", datatemp / 10, datatemp % 10);
                }
                else
                {
                    datatemp = 0 - pDataNode->data_20040204;
                    offset += sprintf(buf + offset, "-%d.%d", datatemp / 10, datatemp % 10);
                }
            } 
            break;
        }
        case MIN_GATHER_OAD_43140506:
        {
            if(TRUE == pDataNode->dataflag[12])
            {
                uint16 datatemp = 0;
                if(pDataNode->data_43140506 >= 0)
                {
                    datatemp = pDataNode->data_43140506;
                    offset += sprintf(buf + offset, "%d.%d", datatemp / 10, datatemp % 10);
                }
                else
                {
                    datatemp = 0 - pDataNode->data_43140506;
                    offset += sprintf(buf + offset, "-%d.%d", datatemp / 10, datatemp % 10);
                }
            }
            break;
        }
        case MIN_GATHER_OAD_43140505:
        {
            if(TRUE == pDataNode->dataflag[13])
            {
                uint16 datatemp = 0;
                if(pDataNode->data_43140505 >= 0)
                {
                    datatemp = pDataNode->data_43140505;
                    offset += sprintf(buf + offset, "%d.%d", datatemp / 10, datatemp % 10);
                }
                else
                {
                    datatemp = 0 - pDataNode->data_43140505;
                    offset += sprintf(buf + offset, "-%d.%d", datatemp / 10, datatemp % 10);
                }
            }
            break;
        }
        case MIN_GATHER_OAD_43140501:
        {
            if(TRUE == pDataNode->dataflag[14])
            {
                uint16 datatemp = 0;
                if(pDataNode->data_43140501 >= 0)
                {
                    datatemp = pDataNode->data_43140501;
                    offset += sprintf(buf + offset, "%d.%d", datatemp / 10, datatemp % 10);
                }
                else
                {
                    datatemp = 0 - pDataNode->data_43140501;
                    offset += sprintf(buf + offset, "-%d.%d", datatemp / 10, datatemp % 10);
                }
            }
            break;
        }
        case MIN_GATHER_OAD_43140400:
        {
            if(TRUE == pDataNode->dataflag[15])
            {
                offset += sprintf(buf + offset, "%d", pDataNode->data_43140400);
            }
            break;
        }
        case MIN_GATHER_OAD_43110502:
        {
            if(TRUE == pDataNode->dataflag[16])
            {
                uint16 datatemp = 0;
                if(pDataNode->data_43110502 >= 0)
                {
                    datatemp = pDataNode->data_43110502;
                    offset += sprintf(buf + offset, "%d.%d", datatemp / 10, datatemp % 10);
                }
                else
                {
                    datatemp = 0 - pDataNode->data_43110502;
                    offset += sprintf(buf + offset, "-%d.%d", datatemp / 10, datatemp % 10);
                }
            }
            break;
        }
        case MIN_GATHER_OAD_43110400:
        {
            if(TRUE == pDataNode->dataflag[17])
            {
                offset += sprintf(buf + offset, "%d", pDataNode->data_43110400);
            }
            break;
        }
        case MIN_GATHER_OAD_00100201:
        {
            if(TRUE == pDataNode->dataflag[18])
            {
                offset += sprintf(buf + offset, "%d.%02d", pDataNode->data_00100201 / 100, pDataNode->data_00100201 % 100);
            }
            break;
        }
        case MIN_GATHER_OAD_00100203:
        {
            if(TRUE == pDataNode->dataflag[19])
            {
                offset += sprintf(buf + offset, "%d.%02d", pDataNode->data_00100203 / 100, pDataNode->data_00100203 % 100);
            }
            break;
        }
        case MIN_GATHER_OAD_00100205:
        {
            if(TRUE == pDataNode->dataflag[20])
            {
                offset += sprintf(buf + offset, "%d.%02d", pDataNode->data_00100205 / 100, pDataNode->data_00100205 % 100);
            } 
            break;
        }
        case MIN_GATHER_OAD_00200201:
        {
            if(TRUE == pDataNode->dataflag[21])
            {
                offset += sprintf(buf + offset, "%d.%02d", pDataNode->data_00200201 / 100, pDataNode->data_00200201 % 100);
            }
            break;
        }
        case MIN_GATHER_OAD_00200203:
        {
            if(TRUE == pDataNode->dataflag[22])
            {
                offset += sprintf(buf + offset, "%d.%02d", pDataNode->data_00200203 / 100, pDataNode->data_00200203 % 100);
            }
            break;
        }
        case MIN_GATHER_OAD_00200205:
        {
            if(TRUE == pDataNode->dataflag[23])
            {
                offset += sprintf(buf + offset, "%d.%02d", pDataNode->data_00200205 / 100, pDataNode->data_00200205 % 100);
            }
            break;
        }
        default: return 0;
    }
    return offset;
}

/**********************************************************************
* @name      : min_gather_real_clear
* @brief     ：实时数据清空
* @param[in] ：MG_REAL_NODE_T *pDataNode 数据单元
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-6-29
* @Update    :
**********************************************************************/
void min_gather_real_clear(MG_REAL_NODE_T *pDataNode)
{
    memset(pDataNode->dataflag, 0, sizeof(pDataNode->dataflag));
}

/**********************************************************************
* @name      : min_gather_get_node
* @brief     ：分钟内采集数据输入(实时)
* @param[in] ：MG_REAL_NODE_T **ppDataHead  首地址
               uint8 *Addr                  地址
               OOP_DATETIME_S_T *datamin    时间
               uint8 bAll                   全量标记(为TRUE则匹配时间最大的)
* @param[out]：MG_REAL_NODE_T **ppDataHead  首地址
* @return    ：MG_REAL_NODE_T *
* @Create    : 王津剑
* @Date      ：2021-6-29
* @Update    :
**********************************************************************/
MG_REAL_NODE_T *min_gather_get_node(MG_REAL_NODE_T **ppDataHead, uint8 *Addr, OOP_DATETIME_S_T *datamin)
{
    MG_REAL_NODE_T *pProe = *ppDataHead;
    MG_REAL_NODE_T *pTemp = *ppDataHead;
    MG_REAL_NODE_T *pNew = NULL;

    while(pTemp)
    {
        if(0 == memcmp(pTemp->Addr, Addr, 6) || 0 == memcmp(&(pTemp->min), datamin, sizeof(OOP_DATETIME_S_T)))
        {
            return pTemp;
        }
        pTemp = pTemp->pNext;
    }

    /* 创造新标签 */
    pNew = (MG_REAL_NODE_T*)malloc(sizeof(MG_REAL_NODE_T));
    if(NULL == pNew)
    {
        return NULL;
    }
    memset(pNew, 0, sizeof(MG_REAL_NODE_T));
    pNew->min = *datamin;
    pNew->min.second = 0;
    pNew->mintimes = min_gather_time_s(datamin);
    memcpy(pNew->Addr, Addr, 6);

    /* 新标签按照顺序添加到队列中 */
    if(NULL == *ppDataHead || 0 < memcmp(pNew->Addr, pProe->Addr, 6) ||
       (0 == memcmp(pNew->Addr, pProe->Addr, 6) && pNew->mintimes > pProe->mintimes))
    {
        pNew->pNext = *ppDataHead;
        *ppDataHead = pNew;
        return pNew;
    }
    
    pTemp = pProe->pNext;
    while(pTemp)
    {
        if(0 < memcmp(pNew->Addr, pTemp->Addr, 6) ||
           (0 == memcmp(pNew->Addr, pTemp->Addr, 6) && pNew->mintimes > pTemp->mintimes))
        {
            pNew->pNext = pTemp;
            break; 
        }
        pProe = pTemp;
        pTemp = pProe->pNext;
    }
    pProe->pNext = pNew;

    return pNew;
}

/**********************************************************************
* @name      : min_gather_destroy_allnode
* @brief     ：删除实时数据列表
* @param[in] ：MG_REAL_NODE_T **ppDataHead  首地址
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-6-29
* @Update    :
**********************************************************************/
void min_gather_destroy_allnode(MG_REAL_NODE_T **ppDataHead)
{
    MG_REAL_NODE_T *pTemp = *ppDataHead;
    MG_REAL_NODE_T *pNext = *ppDataHead;
    while(pTemp)
    {
        pNext = pTemp->pNext;
        free(pTemp);
        pTemp = pNext;
    }
    *ppDataHead = NULL;
}

/**********************************************************************
* @name      : min_gather_real_copy
* @brief     ：实时数据，全量整理
* @param[in] ：MG_REAL_NODE_T *pSource  原数据
* @param[out]：MG_REAL_NODE_T *pDest    目标数据
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-6-29
* @Update    :
**********************************************************************/
void min_gather_real_copy(MG_REAL_NODE_T *pSource, MG_REAL_NODE_T *pDest)
{
    if(TRUE == pSource->dataflag[0])
    {
        pDest->data_20000201 = pSource->data_20000201;
        pDest->dataflag[0] = TRUE;
    }
    if(TRUE == pSource->dataflag[1])
    {
        pDest->data_20000202 = pSource->data_20000202;
        pDest->dataflag[1] = TRUE;
    }
    if(TRUE == pSource->dataflag[2])
    {
        pDest->data_20000203 = pSource->data_20000203;
        pDest->dataflag[2] = TRUE;
    }
    if(TRUE == pSource->dataflag[3])
    {
        pDest->data_20010201 = pSource->data_20010201;
        pDest->dataflag[3] = TRUE;
    }
    if(TRUE == pSource->dataflag[4])
    {
        pDest->data_20010202 = pSource->data_20010202;
        pDest->dataflag[4] = TRUE;
    }
    if(TRUE == pSource->dataflag[5])
    {
        pDest->data_20010203 = pSource->data_20010203;
        pDest->dataflag[5] = TRUE;
    }
    if(TRUE == pSource->dataflag[6])
    {
        pDest->data_20010400 = pSource->data_20010400;
        pDest->dataflag[6] = TRUE;
    }
    if(TRUE == pSource->dataflag[7])
    {
        pDest->data_20010500 = pSource->data_20010500;
        pDest->dataflag[7] = TRUE;
    }
    if(TRUE == pSource->dataflag[8])
    {
        pDest->data_20040201 = pSource->data_20040201;
        pDest->dataflag[8] = TRUE;
    }
    if(TRUE == pSource->dataflag[9])
    {
        pDest->data_20040202 = pSource->data_20040202;
        pDest->dataflag[9] = TRUE;
    }
    if(TRUE == pSource->dataflag[10])
    {
        pDest->data_20040203 = pSource->data_20040203;
        pDest->dataflag[10] = TRUE;
    }
    if(TRUE == pSource->dataflag[11])
    {
        pDest->data_20040204 = pSource->data_20040204;
        pDest->dataflag[11] = TRUE;
    }
    if(TRUE == pSource->dataflag[12])
    {
        pDest->data_43140506 = pSource->data_43140506;
        pDest->dataflag[12] = TRUE;
    }
    if(TRUE == pSource->dataflag[13])
    {
        pDest->data_43140505 = pSource->data_43140505;
        pDest->dataflag[13] = TRUE;
    }
    if(TRUE == pSource->dataflag[14])
    {
        pDest->data_43140501 = pSource->data_43140501;
        pDest->dataflag[14] = TRUE;
    }
    if(TRUE == pSource->dataflag[15])
    {
        pDest->data_43140400 = pSource->data_43140400;
        pDest->dataflag[15] = TRUE;
    }
    if(TRUE == pSource->dataflag[16])
    {
        pDest->data_43110400 = pSource->data_43110400;
        pDest->dataflag[16] = TRUE;
    }
    if(TRUE == pSource->dataflag[17])
    {
        pDest->data_43110502 = pSource->data_43110502;
        pDest->dataflag[17] = TRUE;
    }
    if(TRUE == pSource->dataflag[18])
    {
        pDest->data_00100201 = pSource->data_00100201;
        pDest->dataflag[18] = TRUE;
    }
    if(TRUE == pSource->dataflag[19])
    {
        pDest->data_00100203 = pSource->data_00100203;
        pDest->dataflag[19] = TRUE;
    }
    if(TRUE == pSource->dataflag[20])
    {
        pDest->data_00100205 = pSource->data_00100205;
        pDest->dataflag[20] = TRUE;
    }
    if(TRUE == pSource->dataflag[21])
    {
        pDest->data_00200201 = pSource->data_00200201;
        pDest->dataflag[21] = TRUE;
    }
    if(TRUE == pSource->dataflag[22])
    {
        pDest->data_00200203 = pSource->data_00200203;
        pDest->dataflag[22] = TRUE;
    }
    if(TRUE == pSource->dataflag[23])
    {
        pDest->data_00200205 = pSource->data_00200205;
        pDest->dataflag[23] = TRUE;
    }
}

/**********************************************************************
* @name      : min_gather_real_all
* @brief     ：实时数据，全量整理
* @param[in] ：MG_REAL_NODE_T **ppAllHead  首地址
               MG_REAL_NODE_T **ppNewHead   地址
* @param[out]：MG_REAL_NODE_T **ppAllHead  首地址
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-6-29
* @Update    :
**********************************************************************/
void min_gather_real_all(MG_REAL_NODE_T **ppAllHead, MG_REAL_NODE_T **ppNewHead)
{
    MG_REAL_NODE_T *pTemp = NULL;
    MG_REAL_NODE_T *pPre = NULL;
    MG_REAL_NODE_T *pNext = NULL;

    while(*ppNewHead)
    {
        pTemp = *ppNewHead;
        *ppNewHead = pTemp->pNext;
        
        pPre = *ppAllHead;
        if(NULL == pPre || 0 < memcmp(pTemp->Addr, pPre->Addr, 6))
        {
            /* 作为新头部 */
            pTemp->pNext = pPre;
            *ppAllHead = pTemp;
        }
        else if(NULL != pPre && 0 == memcmp(pTemp->Addr, pPre->Addr, 6))
        {
            if(pTemp->mintimes > pPre->mintimes)
            {
                /* 取代新头部 */
                pTemp->pNext = pPre->pNext;
                *ppAllHead = pTemp;
                free(pPre);
            }
            else if(pTemp->mintimes == pPre->mintimes)
            {
                /* 数据合并 */
                min_gather_real_copy(pTemp, pPre);
                free(pTemp);
            }
            else
            {
                free(pTemp);
            }
        }
        else if(NULL != pPre)
        {
            pNext = pPre->pNext;
            while(pNext)
            {
                if(0 < memcmp(pTemp->Addr, pNext->Addr, 6))
                {
                    pPre->pNext = pTemp;
                    pTemp->pNext = pNext;
                    pTemp = NULL;
                    break; 
                }
                else if(0 == memcmp(pTemp->Addr, pNext->Addr, 6))
                {
                    if(pTemp->mintimes > pNext->mintimes)
                    {
                        /* 取代 */
                        pPre->pNext = pTemp;
                        pTemp->pNext = pNext->pNext;
                        free(pNext);
                    }
                    else if(pTemp->mintimes == pNext->mintimes)
                    {
                        /* 数据合并 */
                        min_gather_real_copy(pTemp, pNext);
                        free(pTemp);
                    }
                    else
                    {
                        free(pTemp);
                    }
                    pTemp = NULL;
                    break; 
                }
                pPre = pNext;
                pNext = pPre->pNext;
            }
            if(NULL == pNext && NULL != pTemp)
            {
                pPre->pNext = pTemp;
                pTemp->pNext = NULL;
            }
        }
    }
}

/**********************************************************************
* @name      : min_gather_real_store
* @brief     ：实时数据存储
* @param[in] ：uint8 *addr                 终端地址
               char *filepath              存储路径
               MG_REAL_NODE_T *pDataList   数据列表
               OOP_DATETIME_S_T *storemin  存储时间
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-6-29
* @Update    :
**********************************************************************/
int min_gather_real_store(uint8 *addr, char *filepath, MG_REAL_NODE_T *pDataList, OOP_DATETIME_S_T *storemin)
{
    int         ret = MIN_GATHER_SUCCESS;
    int         fd = -1;
    char        filename[256] = {0};
    char        tarname[256] = {0};
    char        strTemp[2048] = {0};
    int         offset = 0;
    uint8       flag = 0;
    uint8       i = 0;
    DIR            *pDdir = NULL;
    struct dirent  *dp = NULL;
    MG_REAL_NODE_T *pTemp = pDataList;
    
    sprintf(filename, "%s/%s%02X%02X%02X%02X%02X%02X_%04d%02d%02d%02d%02d.json", filepath, MIN_GATHER_REAL_NAMR,
                      addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], 
                      storemin->year, storemin->month, storemin->day, storemin->hour, storemin->minute);
    sprintf(tarname,  "%s/%s%02X%02X%02X%02X%02X%02X_%04d%02d%02d%02d%02d.tar.bz2", filepath, MIN_GATHER_REAL_NAMR,
                      addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], 
                      storemin->year, storemin->month, storemin->day, storemin->hour, storemin->minute);

    /* 1. 文件夹检查 */
    if(0 != access(filepath, F_OK))
    {
        if(0 != mkdir(filepath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
        {
            return MIN_GATHER_ERR_FILE;
        }
    }

    /* 2. 删除同名文件 */
    remove(filename);

    /* 3. 打开或新建文件 */
    fd = open(filename, O_CREAT |O_WRONLY | O_APPEND, 0664);
    if(fd < 0)
    {
        return MIN_GATHER_ERR_FILE;
    }

    /* 4. 写文件 */
    write(fd, "[\r\n", 3);
    while(pTemp)
    {
        /* 构造 */
        offset = 0;
        offset = sprintf(strTemp, "{\"%02X%02X%02X%02X%02X%02X\":\"", pTemp->Addr[0], pTemp->Addr[1],
                                  pTemp->Addr[2], pTemp->Addr[3], pTemp->Addr[4], pTemp->Addr[5]);
        for(i = 0; i < 28; i++)
        {
            flag += pTemp->dataflag[i];
        }
        if(0 == flag)
        {
            offset += sprintf(strTemp + offset, ",,,,,,,,,,,,,,,,,,,,,,,,,\"");
        }
        else
        {
            offset += sprintf(strTemp + offset, "%04d-%02d-%02d %02d:%02d:%02d", pTemp->min.year, pTemp->min.month,
                                                pTemp->min.day, pTemp->min.hour, pTemp->min.minute, pTemp->min.second);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_20000201, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_20000202, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_20000203, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_20010201, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_20010202, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_20010203, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_20010400, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_20010500, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_20040201, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_20040202, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_20040203, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_20040204, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_43140506, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_43140505, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_43140501, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_43140400, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_43110502, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_43110400, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_00100201, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_00100203, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_00100205, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_00200201, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_00200203, strTemp + offset, 2048 - offset);
            offset += min_gather_real_data_str(pTemp, MIN_GATHER_OAD_00200205, strTemp + offset, 2048 - offset);
        }
        offset += sprintf(strTemp + offset, "},\r\n");
        write(fd, strTemp, offset);
        memset(strTemp, 0, offset);
        pTemp = pTemp->pNext;
    }
    write(fd, "]", 1);

    /* 5. 关闭文件句柄 */
    close(fd);

    /* 6. 压缩 */
    remove(tarname);
    sprintf(strTemp, "tar -cjf %s %s", tarname, filename);
    system(strTemp);
    usleep(10000);

    /* 7. 删除其他json */
    memset(filename, 0, 256);
    sprintf(filename, "%04d%02d%02d%02d%02d.json", storemin->year, storemin->month, storemin->day, 
                                                   storemin->hour, storemin->minute);
    pDdir = opendir(filepath);
    if(NULL == pDdir)
    {
        return MIN_GATHER_ERR_FILE;
    }
    
    while((dp = readdir(pDdir)) != NULL)
    {
        if(0 == strcmp(dp->d_name, ".") || 0 == strcmp(dp->d_name, ".."))
        {
            continue;
        }
        /* 待压缩文件判断 */
        if(NULL != strstr(dp->d_name, MIN_GATHER_REAL_NAMR) && NULL != strstr(dp->d_name, ".json"))
        {
            sprintf(strTemp, "%s/%s", filepath, dp->d_name);
            if(NULL == strstr(dp->d_name, filename))
            {
                remove(strTemp);
            }
        }
    }
    closedir(pDdir);
    return ret;
}

/**********************************************************************
* @name      : min_gather_insert_data
* @brief     ：分钟采集添加实时数据
* @param[in] ：MG_REAL_NODE_T *pDataNode 数据单元
               uint32 type               数据标识(MIN_GATHER_OAD_E)
               uint32 storeTm            存储时间
               uint8 *data               数据内容
               uint16 datalen            数据长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-6-29
* @Update    :
**********************************************************************/
int min_gather_real_add(MG_REAL_NODE_T *pDataNode, uint32 type, uint32 storeTm, uint8 *data, uint16 datalen)
{
    switch(type)
    {
        case MIN_GATHER_OAD_20000201:
        {
            if(datalen != sizeof(uint16))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_20000201, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_20000202:
        {
            if(datalen != sizeof(uint16))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_20000202, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_20000203:
        {
            if(datalen != sizeof(uint16))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_20000203, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_20010201:
        {
            if(datalen != sizeof(int32))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_20010201, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_20010202:
        {
            if(datalen != sizeof(int32))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_20010202, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_20010203:
        {
            if(datalen != sizeof(int32))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_20010203, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_20010400:
        {
            if(datalen != sizeof(int32))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_20010400, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_20010500:
        {
            if(datalen != sizeof(int32))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_20010500, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_20040201:
        {
            if(datalen != sizeof(int32))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_20040201, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_20040202:
        {
            if(datalen != sizeof(int32))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_20040202, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_20040203:
        {
            if(datalen != sizeof(int32))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_20040203, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_20040204:
        {
            if(datalen != sizeof(int32))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_20040204, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_43140506:
        {
            if(datalen != sizeof(int16))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_43140506, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_43140505:
        {
            if(datalen != sizeof(int16))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_43140505, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_43140501:
        {
            if(datalen != sizeof(int16))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_43140501, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_43140400:
        {
            if(datalen != sizeof(uint8))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_43140400, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_43110502:
        {
            if(datalen != sizeof(int16))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_43110502, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_43110400:
        {
            if(datalen != sizeof(uint8))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_43110400, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_00100201:
        {
            if(datalen != sizeof(uint32))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_00100201, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_00100203:
        {
            if(datalen != sizeof(uint32))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_00100203, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_00100205:
        {
            if(datalen != sizeof(uint32))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_00100205, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_00200201:
        {
            if(datalen != sizeof(uint32))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_00200201, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_00200203:
        {
            if(datalen != sizeof(uint32))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_00200203, data, datalen); 
            break;
        }
        case MIN_GATHER_OAD_00200205:
        {
            if(datalen != sizeof(uint32))
            {
                goto dataerr;
            }
            memcpy(&pDataNode->data_00200205, data, datalen); 
            break;
        }
        default: return MIN_GATHER_ERR_NOTSUPPORT;
    }

    min_gather_real_setfflag(pDataNode, type);
    return MIN_GATHER_SUCCESS;

dataerr:
    return MIN_GATHER_ERR_INPUT;
}

/**********************************************************************
* @name      : min_gather_real_delete
* @brief     ：分钟采集删除指定时间之前的数据
* @param[in] ：uint8 *addr                 终端地址
               char *filepath              存储路径
               uint8 hours                 时间差 小时
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-9-27
* @Update    :
**********************************************************************/
void min_gather_real_delete(uint8 *addr, char *filepath, uint8 hours)
{
    char        filename[256] = {0};
    char        timestr[256] = {0};
    char        strTemp[512] = {0};
    uint8       timelen = 0;
    uint16      offset = 0;
    time_t      cuttimes = 0;
    OOP_DATETIME_S_T timeflag;
    DIR             *pDdir = NULL;
    struct dirent   *dp = NULL;

    if(0 == hours)
    {
        hours = 1;
    }
    cuttimes = time(NULL);
    cuttimes -= 3600 * hours - 60;
    min_gather_time_t(cuttimes, &timeflag);
    
    /* 1. 文件夹检查 */
    if(0 != access(filepath, F_OK))
    {
        return;
    }

    /* 2. 删除变化地址的数据 */
    pDdir = opendir(filepath);
    if(NULL == pDdir)
    {
        return;
    }

    sprintf(filename, "%s%02X%02X%02X%02X%02X%02X_", MIN_GATHER_REAL_NAMR,
                                                     addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    while((dp = readdir(pDdir)) != NULL)
    {
        if(0 == strcmp(dp->d_name, ".") || 0 == strcmp(dp->d_name, ".."))
        {
            continue;
        }
        /* 旧地址判断 */
        if(NULL != strstr(dp->d_name, MIN_GATHER_REAL_NAMR) && NULL == strstr(dp->d_name, filename))
        {
            sprintf(strTemp, "%s/%s", filepath, dp->d_name);
            remove(strTemp);
        }
    }
    closedir(pDdir);

    /* 3. 删除旧数据 */
    offset = strlen(filename);
    sprintf(timestr, "%04d%02d%02d%02d%02d", timeflag.year, timeflag.month, timeflag.day, timeflag.hour, 
                                             timeflag.minute);
    timelen = strlen(timestr);
    
    pDdir = opendir(filepath);
    if(NULL == pDdir)
    {
        return;
    }
    
    while((dp = readdir(pDdir)) != NULL)
    {
        if(0 == strcmp(dp->d_name, ".") || 0 == strcmp(dp->d_name, ".."))
        {
            continue;
        }
        if(NULL != strstr(dp->d_name, MIN_GATHER_REAL_NAMR))
        {
            if(0 > memcmp(dp->d_name + offset, timestr, timelen))
            {
                sprintf(strTemp, "%s/%s", filepath, dp->d_name);
                remove(strTemp);
            }
        }
    }
    closedir(pDdir);
}


