/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：规约解析公共函数
* @date： 2019.12.03
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopStorage.h"
#include "db_comm.h"
#include "area.h"

const IC_COV_T g_ic_cov_op[OOP_IC_MAX] = 
{
     {1 , data_to_buf_class1 ,  buf_to_data_class1},
     {2 , data_to_buf_class2 ,  buf_to_data_class2},
     {3 , data_to_buf_class3 ,  buf_to_data_class3},
     {4 , data_to_buf_class4 ,  buf_to_data_class4},
     {5 , data_to_buf_class5 ,  buf_to_data_class5},
     {6 , data_to_buf_class6 ,  buf_to_data_class6},
     {7 , data_to_buf_class7 ,  buf_to_data_class7},
     {8 , data_to_buf_class8 ,  buf_to_data_class8},
     {9 , data_to_buf_class9 ,  buf_to_data_class9},
     {10, data_to_buf_class10,  buf_to_data_class10},
     {11, data_to_buf_class11,  buf_to_data_class11},
     {12, data_to_buf_class12,  buf_to_data_class12},
     {13, data_to_buf_class13,  buf_to_data_class13},
     {14, data_to_buf_class14,  buf_to_data_class14},
     {15, data_to_buf_class15,  buf_to_data_class15},
     {16, data_to_buf_class16,  buf_to_data_class16},
     {17, data_to_buf_class17,  buf_to_data_class17},
     {18, data_to_buf_class18,  buf_to_data_class18},
     {19, data_to_buf_class19,  buf_to_data_class19},
     {20, data_to_buf_class20,  buf_to_data_class20},
     {21, data_to_buf_class21,  buf_to_data_class21},
     {22, data_to_buf_class22,  buf_to_data_class22},
     {23, data_to_buf_class23,  buf_to_data_class23},
     {24, data_to_buf_class24,  buf_to_data_class24},
     {25, data_to_buf_class25,  buf_to_data_class25},
     {26, data_to_buf_class26,  buf_to_data_class26},
};

#if DESC("规约模块公共函数",1)

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(NULL)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_null(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    if (srcLen != 0)
    {
        PRTL_FMT_DEBUG("data_to_buf_null srclen != 0\n");
        return DATA_TYPE_UNMATCHED;
    }

    //结构类型
    dstBuf[dOffset++] = DT_NULL;   

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

/**
*********************************************************************
* @name：       class_area_oadinfo_get
* @brief：      查询不同地区的ODA列表，得到OAD描述信息
* @param[in] ：OOP_OAD_U oad        查询OAD
* @param[out]：OAD_INFO_T *info     ODA信息
* @return    ：查询结果 TRUE-成功，FALSE-失败
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int class_area_omdinfo_get(OOP_OAD_U oad, OAD_INFO_T *pstOdaInfo)
{
#ifdef AREA_HUNAN
    return class_hunan_omdinfo_get(oad, pstOdaInfo);
#endif
#ifdef AREA_ZHEJIANG
    return class_zhejiang_omdinfo_get(oad, pstOdaInfo);
#endif
    return FALSE;
}

/**
*********************************************************************
* @name：       class_oadinfo_get
* @brief：      查询ODA列表，得到OAD描述信息
* @param[in] ：OOP_OAD_U oad        查询OAD
* @param[out]：OAD_INFO_T *info     ODA信息
* @return    ：查询结果 TRUE-成功，FALSE-失败
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int class_omdinfo_get(OOP_OAD_U oad, OAD_INFO_T *pstOdaInfo)
{
    uint8 OIA  = 0;
    uint8 OIA1 = 0;
    uint8 OIA2 = 0;
    OIA = (uint8)(oad.nObjID>>8);
    OIA1 = (uint8)((OIA>>4)&0x0f);
    OIA2 = (uint8)((OIA)&0x0f);
    OOP_OAD_U blockOAD = {0};
    uint32 size = 0;
    const OMD_INFO_T *pstOdaTab = NULL;
     
    switch(OIA1)
    {
        case 0x02: 
        {
            switch(OIA2)
            {
                case 0x01: pstOdaTab = omdStatInfoList;   size = omdStatInfoListLen; break; 
                case 0x03: pstOdaTab = omdTgInfoList;   size = omdTgInfoListLen; break; 
                case 0x04: pstOdaTab = omdVariateInfoList;   size = omdVariateInfoListLen; break;                  
                default:break; 
            }        
        }break;
            
        case 0x03: pstOdaTab = omdEvtInfoList; size = omdEvtInfoListLen; break;
        case 0x04: pstOdaTab = omdParaInfoList; size = omdParaInfoListLen; break;
        case 0x05: pstOdaTab = omdFrozenInfoList; size = omdFrozenInfoListLen; break;       
        case 0x06: pstOdaTab = omdCollectInfoList; size = omdCollectInfoListLen; break;
        case 0x07: pstOdaTab = omdGatherInfoList; size = omdGatherListLen; break;
        case 0x08: pstOdaTab = omdControlInfoList; size = omdControlInfoListLen; break;  
        case 0x0f: 
        {
            switch(OIA2)
            {
                case 0x00: pstOdaTab = omdFileInfoList; size = omdFileInfoListLen; break;
                case 0x01: pstOdaTab = omdEsamInfoList; size = omdEsamInfoListLen; break;   
                case 0x02: pstOdaTab = omdIoInfoList;   size = omdIoInfoListLen; break;
                case 0x03: pstOdaTab = omdGuiList;   size = omdGuiListLen; break;
                case 0x04: pstOdaTab = omdEsamInfoList; size = omdEsamInfoListLen; break; 
                case 0x0f: pstOdaTab = omdSelfInfoList; size = omdSelfInfoListLen; break;  
                default:break; 
            }
        }break;
        default:break;
    }

    if(pstOdaTab != NULL)
    {
        while(size--)
        {
            if(pstOdaTab->omd.value == oad.value)
            {
                pstOdaInfo->pTab = (OAD_TAB_T *)pstOdaTab;
                return TRUE;
            }

            pstOdaTab++;
        }
    }

    //如果索引不为0，查不中，再用0查一次
    if(oad.nIndex != 0)  
    {
        blockOAD.value = oad.value;
        blockOAD.nIndex = 0;
        
        PRTL_FMT_DEBUG("block search oadInfoList. OAD(0x%08x), OIA1(%d), OIA2(%d)\n", oad.value, OIA1, OIA2);
        return class_omdinfo_get(blockOAD, pstOdaInfo);
    }

    //通用表差不中，则向地域表查询
    if(class_area_omdinfo_get(oad, pstOdaInfo) == TRUE)
    {
       PRTL_FMT_DEBUG("area search oadInfoList. OAD(0x%08x)\n", oad.value);
       return TRUE;
    }

    PRTL_FMT_DEBUG("search omdInfoList miss. OAD(0x%08x), OIA1(%d), OIA2(%d)\n", oad.value, OIA1, OIA2);

    return FALSE;
}

/**
*********************************************************************
* @name：       class_area_oadinfo_get
* @brief：      查询不同地区的ODA列表，得到OAD描述信息
* @param[in] ：OOP_OAD_U oad        查询OAD
* @param[out]：OAD_INFO_T *info     ODA信息
* @return    ：查询结果 TRUE-成功，FALSE-失败
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int class_area_oadinfo_get(OOP_OAD_U oad, OAD_INFO_T *pstOdaInfo)
{
#ifdef AREA_HUNAN
    return class_hunan_oadinfo_get(oad, pstOdaInfo);
#endif
#ifdef AREA_ZHEJIANG
    return class_zhejiang_oadinfo_get(oad, pstOdaInfo);
#endif
    return FALSE;
}

/**
*********************************************************************
* @name：       class_oadinfo_get
* @brief：      查询ODA列表，得到OAD描述信息
* @param[in] ：OOP_OAD_U oad        查询OAD
* @param[out]：OAD_INFO_T *info     ODA信息
* @return    ：查询结果 TRUE-成功，FALSE-失败
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int class_oadinfo_get(OOP_OAD_U oad, OAD_INFO_T *pstOdaInfo)
{
    uint8 OIA  = 0;
    uint8 OIA1 = 0;
    uint8 OIA2 = 0;
    OIA = (uint8)(oad.nObjID>>8);
    OIA1 = (uint8)((OIA>>4)&0x0f);
    OIA2 = (uint8)((OIA)&0x0f);
    OOP_OAD_U blockOAD = {0};
    uint32 size = 0;
    const OAD_TAB_T *pstOdaTab = NULL;

    //不为0才赋值，否则递归调用后会被清零
    if(oad.nIndex != 0)  
    {
        pstOdaInfo->nIndex = oad.nIndex;
    }
    
    switch(OIA1)
    {
        case 0x00: pstOdaTab = oadEnergyInfoList; size = oadEnergyInfoListLen; break;
        case 0x01: pstOdaTab = oadDemandInfoList; size = oadDemandInfoListLen; break; 
        case 0x02: pstOdaTab = oadVariateInfoList; size = oadVariateInfoListLen; break;
        case 0x03: pstOdaTab = oadEvnetInfoList; size = oadEvnetInfoListLen; break;    
        case 0x04: pstOdaTab = oadParaInfoList; size = oadParaInfoListLen; break;
        case 0x05: pstOdaTab = oadFreezeInfoList; size = oadFreezeInfoListLen; break; 
        case 0x06: pstOdaTab = oadCollectInfoList; size = oadCollectInfoListLen; break;
        case 0x07: pstOdaTab = oadGatherInfoList; size = oadGatherInfoListLen; break; 
        case 0x08: pstOdaTab = oadControlInfoList; size = oadControlInfoListLen; break;  
        case 0x0f: 
        {
            switch(OIA2)
            {
                case 0x00: pstOdaTab = oadFileInfoList; size = oadFileInfoListLen; break;
                case 0x01: pstOdaTab = oadEsamInfoList; size = oadEsamInfoListLen; break;   
                case 0x02: pstOdaTab = oadIoInfoList;   size = oadIoInfoListLen; break; 
                case 0x03: pstOdaTab = oadGuiList;   size = oadGuiListLen; break;
                case 0x04: pstOdaTab = oadEsamInfoList; size = oadEsamInfoListLen; break;
                case 0x0f: pstOdaTab = oadSelfInfoList; size = oadSelfInfoListLen; break;   
                default:break; 
            }
        }break;
        default:break;
    }

    if(pstOdaTab != NULL)
    {
        while(size--)
        {
            if(pstOdaTab->oad.value == oad.value)
            {
                pstOdaInfo->pTab = pstOdaTab;
                pstOdaInfo->subeData = pstOdaTab->eData;
                return TRUE;
            }
            else if((pstOdaTab->oad.nObjID == oad.nObjID) &&
                    (pstOdaTab->oad.attID == oad.attID) &&
                    (pstOdaTab->oad.nIndex == oad.nIndex)) 
            {
                PRTL_FMT_DEBUG("non-strict search oadInfoList. OAD(0x%08x), ATTPRO(%d), OIA1(%d), OIA2(%d)\n", oad.value, oad.attPro, OIA1, OIA2);
                pstOdaInfo->pTab = pstOdaTab;
                pstOdaInfo->subeData = pstOdaTab->eData;
                return TRUE;                
            }
                    
            pstOdaTab++;
        }
    }

    //如果索引不为0，查不中，再用0查一次
    if(oad.nIndex != 0)  
    {
        blockOAD.value = oad.value;
        blockOAD.nIndex = 0;
        
        PRTL_FMT_DEBUG("block search oadInfoList. OAD(0x%08x), OIA1(%d), OIA2(%d)\n", oad.value, OIA1, OIA2);
        return class_oadinfo_get(blockOAD, pstOdaInfo);
    }

    //通用表差不中，则向地域表查询
    if(class_area_oadinfo_get(oad, pstOdaInfo) == TRUE)
    {
       PRTL_FMT_DEBUG("area search oadInfoList. OAD(0x%08x)\n", oad.value);
       return TRUE;
    }

    PRTL_FMT_DEBUG("search oadInfoList miss. OAD(0x%08x), OIA1(%d), OIA2(%d)\n", oad.value, OIA1, OIA2);

    return FALSE;
}

/**
*********************************************************************
* @name：       apdu_bpinfo_free
* @brief：      释放断点信息
* @param[in] ：pInfo       要释放的对象指针
* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
void apdu_bpinfo_free(void *pInfo)
{
    //TODO APDU_BP_INFO_T可能有复杂结构
    if(pInfo != NULL)
    {
        free(pInfo);
    }
}

/**********************************************************************
* @name      : set_len_offset
* @brief     ：设置元素长度
* @param[in] ：eleLen      输入数据长度
* @param[out]：indata      数据Buffer
* @return    ：数据Buffer长度
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/
uint8 set_len_offset(uint16 eleLen, uint8 *indata)
{
    if(eleLen < 0x80)
    {
        indata[0] = eleLen;
        return 1;
    }
    else if(eleLen <= 0xff)
    {
        indata[0] = 0x81;
        indata[1] = eleLen;
        return 2;
    }
    else if(eleLen <= 0xffff)
    {
        indata[0] = 0x82;
        indata[1] = (uint8)((eleLen >> 8) & 0xff);
        indata[2] = (uint8)(eleLen & 0xff);
        return 3;
    }
        
    return 0;
}

/**********************************************************************
* @name      : set_len_offset
* @brief     ：设置元素长度
* @param[in] ：eleLen      输入数据长度
* @param[out]：indata      数据Buffer
* @return    ：数据Buffer长度
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/
uint8 set_biglen_offset(uint32 eleLen, uint8 *indata)
{
    if(eleLen < 0x80)
    {
        indata[0] = eleLen;
        return 1;
    }
    else if(eleLen <= 0xffff)
    {
        indata[0] = 0x82;
        indata[1] = (uint8)((eleLen >> 8) & 0xff);
        indata[2] = (uint8)(eleLen & 0xff);
        return 3;
    }
    else if(eleLen <= 0xffffff)
    {
        indata[0] = 0x83;
        //indata[1] = 0;
        indata[1] = (uint8)((eleLen >> 16) & 0xff);
        indata[2] = (uint8)((eleLen >> 8) & 0xff);
        indata[3] = (uint8)(eleLen & 0xff);
        return 4;
    }
    else if(eleLen <= 0xffffffff)
    {
        indata[0] = 0x84;
        indata[1] = (uint8)((eleLen >> 24) & 0xff);
        indata[2] = (uint8)((eleLen >> 16) & 0xff);
        indata[3] = (uint8)((eleLen >> 8) & 0xff);
        indata[4] = (uint8)(eleLen & 0xff);
        return 5;

    }
       
    return 0;
}


/**********************************************************************
* @name      : get_len_offset
* @brief     ：获取串类型数据元素长度
* @param[in] ：*indata      数据Buffer
* @param[out]：*bytes       数据Buffer长度
* @return    ：长度
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/
uint16 get_len_offset(uint8 *indata,uint8 *bytes)
{
    uint16 EleNum;
    uint8 nbytes = 1;
    
    EleNum = indata[0];
    if(EleNum==0x81)
    {
        EleNum = indata[1];
        nbytes = 2;
    }
    else if(EleNum==0x82)
    {
        EleNum = indata[1];
        EleNum <<= 8;
        EleNum += indata[2];
        nbytes = 3;
    }
    else if(EleNum==0x80 || EleNum>0x82)
    {
        return 0;
    }
    
    if(bytes != NULL)
        *bytes = nbytes;
        
    return EleNum;
}

//
// common
//
/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(通用基本数据)
* @param[in]： ppOadInfo     - 数据表指针
               pOffset       - 数据指针偏移
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_common(OAD_INFO_T *pOadInfo, uint32 *oadInfoOffset, void *srcData, uint32 srcLen, void *dstBuf, uint32 *dstLen)
{
    int32 nRet = ERR_NORMAL;
    OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = 0;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)srcData;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (srcData == NULL) || (dstBuf == NULL) || (oadInfoOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class11 error, ERR_PTR\n");
        return ERR_PTR;
    }

    switch (pOadInfoTmp->pTab->eData)
    {
    case E_NULL:                            /**< NULL               */
    {
        offset = 0;
        nRet = ERR_OK;
        break;
    }

    case E_ARRAY:                           /**< vector             */
        break;

    case E_STRUCTURE:                       /**< struct             */
        break;

    case E_BOOL:                            /**< BOOLEAN               */
    {
        if (srcLen != 1)
        {
            nRet = ERR_OPTION;
            break;
        }
        dstBufTmp[offset++] = DT_BOOL;
        memcpy_rev(&dstBufTmp[offset], &srcDataTmp[0], srcLen);
        offset += srcLen;
        nRet = ERR_OK;
        break;
    }
        
    case E_BIT_STRING:                      /**< CBitString         */
        break;

    case E_DOUBLE_LONG:                     /**< int                */
    {
        if (srcLen != 4)
        {
            nRet = ERR_OPTION;
            break;
        }
        dstBufTmp[offset++] = DT_DOUBLE_LONG;
        memcpy_rev(&dstBufTmp[offset], &srcDataTmp[0], srcLen);
        offset += srcLen;
        nRet = ERR_OK;
        break;
    }

    case E_DOUBLE_LONG_UNSIGNED:            /**< uint32             */
    {
        if (srcLen != 4)
        {
            nRet = ERR_OPTION;
            break;
        }
        dstBufTmp[offset++] = DT_DOUBLE_LONG_UNSIGNED;
        memcpy_rev(&dstBufTmp[offset], &srcDataTmp[0], srcLen);
        offset += srcLen;
        nRet = ERR_OK;
        break;
    }

    case E_OCTET_STRING:                    /**< COctetString       */
        break;
    case E_VISIBLE_STRING:                  /**< string             */
        break;
    case E_UTF8_STRING:                     /**< string             */
        break;
    case E_INTEGER:                         /**< char               */
    {
        if (srcLen != 1)
        {
            nRet = ERR_OPTION;
            break;
        }
        dstBufTmp[offset++] = DT_INTEGER;
        memcpy_rev(&dstBufTmp[offset], &srcDataTmp[0], srcLen);
        offset += srcLen;
        nRet = ERR_OK;
        break;
    }

    case E_LONG:                            /**< int16              */
    {
        if (srcLen != 2)
        {
            nRet = ERR_OPTION;
            break;
        }
        dstBufTmp[offset++] = DT_LONG;
        memcpy_rev(&dstBufTmp[offset], &srcDataTmp[0], srcLen);
        offset += srcLen;
        nRet = ERR_OK;
        break;
    }

    case E_UNSIGNED:                        /**< uint8              */
    {
        if (srcLen != 1)
        {
            nRet = ERR_OPTION;
            break;
        }
        dstBufTmp[offset++] = DT_UNSIGNED;
        memcpy_rev(&dstBufTmp[offset], &srcDataTmp[0], srcLen);
        offset += srcLen;
        nRet = ERR_OK;
        break;
    }

    case E_LONG_UNSIGNED:                   /**< uint16             */
    {
        if (srcLen != 2)
        {
            nRet = ERR_OPTION;
            break;
        }
        dstBufTmp[offset++] = DT_LONG_UNSIGNED;
        memcpy_rev(&dstBufTmp[offset], &srcDataTmp[0], srcLen);
        offset += srcLen;
        nRet = ERR_OK;
        break;
    }

    case E_LONG64:                          /**< int64              */
    {
        if (srcLen != 8)
        {
            nRet = ERR_OPTION;
            break;
        }
        dstBufTmp[offset++] = DT_LONG64;
        memcpy_rev(&dstBufTmp[offset], &srcDataTmp[0], srcLen);
        offset += srcLen;
        nRet = ERR_OK;
        break;
    }

    case E_LONG64_UNSIGNED:                 /**< uint64             */
    {
        if (srcLen != 8)
        {
            nRet = ERR_OPTION;
            break;
        }
        dstBufTmp[offset++] = DT_LONG64_UNSIGNED;
        memcpy_rev(&dstBufTmp[offset], &srcDataTmp[0], srcLen);
        offset += srcLen;
        nRet = ERR_OK;
        break;

    }

    case E_ENUM:                            /**< uint8              */
    {
        if (srcLen != 1)
        {
            nRet = ERR_OPTION;
            break;
        }
        dstBufTmp[offset++] = DT_ENUM;
        memcpy_rev(&dstBufTmp[offset], &srcDataTmp[0], srcLen);
        offset += srcLen;
        nRet = ERR_OK;
        break;
    }

    case E_FLOAT32:                         /**< float32            */
    {
        if (srcLen != 4)
        {
            nRet = ERR_OPTION;
            break;
        }
        dstBufTmp[offset++] = DT_FLOAT32;
        memcpy_rev(&dstBufTmp[offset], &srcDataTmp[0], srcLen);
        offset += srcLen;
        nRet = ERR_OK;
        break;
    }

    case E_FLOAT64:                         /**< float64            */
    {
        if (srcLen != 8)
        {
            nRet = ERR_OPTION;
            break;
        }
        dstBufTmp[offset++] = DT_FLOAT64;
        memcpy_rev(&dstBufTmp[offset], &srcDataTmp[0], srcLen);
        offset += srcLen;
        nRet = ERR_OK;
        break;
    }

    case E_DATETIME:                        /**< DateTime           */
        break;
    case E_DATE:                            /**< OOP_DATE_T         */
        break;
    case E_TIME:                            /**< OOP_TIME_T         */
        break;
    case E_DATETIME_S:                      /**< DateTime_S         */
        break;
    case E_OI:                              /**< OI                 */
        break;
    case E_OAD:                             /**< OOP_OAD_U          */
    {
        if (srcLen != sizeof(OOP_OAD_U))
        {
            nRet = ERR_OPTION;
            break;
        }
        dstBufTmp[offset++] = DT_OAD;
        memcpy_rev(&dstBufTmp[offset], &srcDataTmp[0], srcLen);
        offset += srcLen;
        nRet = ERR_OK;
        break;
    }
        
    case E_ROAD:                            /**< OOP_ROAD_T         */
        break;
    case E_OMD:                             /**< OOP_OMD_U          */
        break;
    case E_TI:                              /**< 时间间隔           */
        break;
    case E_TSA:                             /**< 设备地址           */
    {
        OOP_TSA_T oopTsa;
        
        if (srcLen != sizeof(OOP_TSA_T))
        {
            nRet = ERR_OPTION;
            break;
        }
        memcpy(&oopTsa, srcDataTmp, srcLen);

        dstBufTmp[offset++] = DT_TSA;
        dstBufTmp[offset++] = oopTsa.len + 2;   //地址最大长度是16，不需要变长类型
        dstBufTmp[offset++] = oopTsa.af;   //地址最大长度是16，不需要变长类型
        if (oopTsa.flag == 0x01)
        {
            dstBufTmp[offset ++] = oopTsa.vxd; 
            memcpy(&dstBufTmp[offset], &oopTsa.add[0], oopTsa.len); //统一改小端，报文为大端
            offset += oopTsa.len;
        }
        else
        {
            memcpy(&dstBufTmp[offset], &oopTsa.add[0], oopTsa.len + 1); //统一改小端，报文为大端
            offset += oopTsa.len + 1;
        }
        PRTL_BUF_TRACE(&dstBufTmp[0], offset, "从数据中转的地址报文：");
        
        nRet = ERR_OK;
        break;  
    }
        
    case E_MAC:                             /**< OOP_MAC_T签名        */
        break;
    case E_RN:                              /**< 随机数              */
        break;
    case E_REGION:                          /**< 区间类型           */
        break;
    case E_SCALER_UNIT:                     /**< 换算单位           */
        break;
    case E_RSD:                             /**< 行选择符           */
        break;
    case E_CSD:                             /**< 列选择符           */
        break;
    case E_MS:                              /**< 电能表集合            */
        break;
    case E_SID:                             /**< 安全标识           */
        break;
    case E_SIDMAC:                          /**< 安全标识+OOP_MAC_T */
        break;
    case E_COMDCB:                          /**< 串口通讯控制块      */
        break;
    case E_RCSD:                            /**< 记录列选择描述符   */
        break;
    
    default:
        nRet = ERR_OPTION;
        break;
    }

    if (nRet == ERR_OK)
    {
        *oadInfoOffset = 1;
        *dstLen = offset;
    }
    else
    {
        *oadInfoOffset = 0;
        *dstLen = 0;
    }
    
    return nRet;
}

/**
*********************************************************************
* @brief：     从698格式报文中获取数据(通用基本数据)
* @param[in]： ppOadInfo     - 数据表指针
               pOffset       - 数据指针偏移
               srcBuf        - 源数据
               srcLen        - 源数据长度
               dstLen        - 目的数据长度
* @param[out]：dstData       - 目的数据
               offset        - 报文偏移
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 buf_to_data_common(OAD_INFO_T *pOadInfo, uint32 *oadInfoOffset, void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_NORMAL;
    OAD_INFO_T *pOadInfoTmp = NULL;
    uint32 offset     = 0;
    pOadInfoTmp       = pOadInfo;
    uint8 *srcBufTmp  = (uint8 *)srcBuf;
    uint8 *dstDataTmp = (uint8 *)dstData;

    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (srcBuf == NULL) || (dstData == NULL) || (oadInfoOffset == NULL) || (dstLen <= 0) || (pOffset == NULL))
    {
        PRTL_FMT_DEBUG("buf_to_data_common param error, ERR_PTR\n");
        return ERR_PTR;
    }
    switch (pOadInfoTmp->pTab->eData)
    {
    case E_NULL:                            /**< NULL               */
    {
        offset = 0;
        *dstDataTmp = 0;
        nRet = ERR_OK;
        break;
    }

    case E_ARRAY:                           /**< vector             */
        break;

    case E_STRUCTURE:                       /**< struct             */
        break;

    case E_BOOL:                            /**< BOOLEAN               */
    {
        if (srcLen < 2)
        {
            nRet = ERR_OPTION;
            break;
        }
        if (dstLen != sizeof(uint8))
        {
            nRet = ERR_OPTION;
            break;
        }
        if (srcBufTmp[offset++] != DT_BOOL)
        {
            nRet = ERR_TYPE;
            break;
        }
        memcpy_rev(dstDataTmp, &srcBufTmp[offset], dstLen);
        offset += dstLen;

        nRet = ERR_OK;
        break;
    }
        
    case E_BIT_STRING:                      /**< CBitString         */
        break;

    case E_DOUBLE_LONG:                     /**< int                */
    {
        if (srcLen < 5)
        {
            nRet = ERR_OPTION;
            break;
        }
        if (dstLen != sizeof(int32))
        {
            nRet = ERR_OPTION;
            break;
        }
        if (srcBufTmp[offset++] != DT_DOUBLE_LONG)
        {
            nRet = ERR_TYPE;
            break;
        }
        memcpy_rev(dstDataTmp, &srcBufTmp[offset], dstLen);
        offset += dstLen;

        nRet = ERR_OK;
        break;
    }

    case E_DOUBLE_LONG_UNSIGNED:            /**< uint32             */
    {
        if (srcLen < 5)
        {
            nRet = ERR_OPTION;
            break;
        }
        if (dstLen != sizeof(uint32))
        {
            nRet = ERR_OPTION;
            break;
        }
        if (srcBufTmp[offset++] != DT_DOUBLE_LONG_UNSIGNED)
        {
            nRet = ERR_TYPE;
            break;
        }
        memcpy_rev(dstDataTmp, &srcBufTmp[offset], dstLen);
        offset += dstLen;

        nRet = ERR_OK;
        break;
    }

    case E_OCTET_STRING:                    /**< COctetString       */
        break;
    case E_VISIBLE_STRING:                  /**< string             */
        break;
    case E_UTF8_STRING:                     /**< string             */
        break;
    case E_INTEGER:                         /**< char               */
    {
        if (srcLen < 2)
        {
            nRet = ERR_OPTION;
            break;
        }
        if (dstLen != sizeof(char))
        {
            nRet = ERR_OPTION;
            break;
        }
        if (srcBufTmp[offset++] != DT_INTEGER)
        {
            nRet = ERR_TYPE;
            break;
        }
        memcpy_rev(dstDataTmp, &srcBufTmp[offset], dstLen);
        offset += dstLen;
 
        nRet = ERR_OK;
        break;
    }

    case E_LONG:                            /**< int16              */
    {
        if (srcLen < 3)
        {
            nRet = ERR_OPTION;
            break;
        }
        if (dstLen != sizeof(int16))
        {
            nRet = ERR_OPTION;
            break;
        }
        if (srcBufTmp[offset++] != DT_LONG)
        {
            nRet = ERR_TYPE;
            break;
        }
        memcpy_rev(dstDataTmp, &srcBufTmp[offset], dstLen);
        offset += dstLen;

        nRet = ERR_OK;
        break;
    }

    case E_UNSIGNED:                        /**< uint8              */
    {
        if (srcLen < 2)
        {
            nRet = ERR_OPTION;
            break;
        }
        if (dstLen != sizeof(uint8))
        {
            nRet = ERR_OPTION;
            break;
        }
        if (srcBufTmp[offset++] != DT_UNSIGNED)
        {
            nRet = ERR_TYPE;
            break;
        }
        memcpy_rev(dstDataTmp, &srcBufTmp[offset], dstLen);
        offset += dstLen;

        nRet = ERR_OK;
        break;
    }

    case E_LONG_UNSIGNED:                   /**< uint16             */
    {
        if (srcLen < 3)
        {
            nRet = ERR_OPTION;
            break;
        }
        if (dstLen != sizeof(uint16))
        {
            nRet = ERR_OPTION;
            break;
        }
        if (srcBufTmp[offset++] != DT_LONG_UNSIGNED)
        {
            nRet = ERR_TYPE;
            break;
        }
        memcpy_rev(dstDataTmp, &srcBufTmp[offset], dstLen);
        offset += dstLen;

        nRet = ERR_OK;
        break;
    }

    case E_LONG64:                          /**< int64              */
    {
        if (srcLen < 9)
        {
            nRet = ERR_OPTION;
            break;
        }
        if (dstLen != sizeof(int64))
        {
            nRet = ERR_OPTION;
            break;
        }
        if (srcBufTmp[offset++] != DT_LONG64)
        {
            nRet = ERR_TYPE;
            break;
        }
        memcpy_rev(dstDataTmp, &srcBufTmp[offset], dstLen);
        offset += dstLen;

        nRet = ERR_OK;
        break;
    }

    case E_LONG64_UNSIGNED:                 /**< uint64             */
    {
        if (srcLen < 9)
        {
            nRet = ERR_OPTION;
            break;
        }
        if (dstLen != sizeof(uint64))
        {
            nRet = ERR_OPTION;
            break;
        }
        if (srcBufTmp[offset++] != DT_LONG64_UNSIGNED)
        {
            nRet = ERR_TYPE;
            break;
        }
        memcpy_rev(dstDataTmp, &srcBufTmp[offset], dstLen);
        offset += dstLen;

        nRet = ERR_OK;
        break;

    }

    case E_ENUM:                            /**< uint8              */
    {
        if (srcLen < 2)
        {
            nRet = ERR_OPTION;
            break;
        }
        if (dstLen != sizeof(uint8))
        {
            nRet = ERR_OPTION;
            break;
        }
        if (srcBufTmp[offset++] != DT_ENUM)
        {
            nRet = ERR_TYPE;
            break;
        }
        memcpy_rev(dstDataTmp, &srcBufTmp[offset], dstLen);
        offset += dstLen;

        nRet = ERR_OK;
        break;
    }

    case E_FLOAT32:                         /**< float32            */
    {
        if (srcLen < 5)
        {
            nRet = ERR_OPTION;
            break;
        }
        if (dstLen != sizeof(float32))
        {
            nRet = ERR_OPTION;
            break;
        }
        if (srcBufTmp[offset++] != DT_FLOAT32)
        {
            nRet = ERR_TYPE;
            break;
        }
        memcpy_rev(dstDataTmp, &srcBufTmp[offset], dstLen);
        offset += dstLen;

        nRet = ERR_OK;
        break;
    }

    case E_FLOAT64:                         /**< float64            */
    {
        if (srcLen < 9)
        {
            nRet = ERR_OPTION;
            break;
        }
        if (dstLen != sizeof(float64))
        {
            nRet = ERR_OPTION;
            break;
        }
        if (srcBufTmp[offset++] != DT_FLOAT64)
        {
            nRet = ERR_TYPE;
            break;
        }
        memcpy_rev(dstDataTmp, &srcBufTmp[offset], dstLen);
        offset += dstLen;

        nRet = ERR_OK;
        break;
    }

    case E_DATETIME:                        /**< DateTime           */
        break;
    case E_DATE:                            /**< OOP_DATE_T         */
        break;
    case E_TIME:                            /**< OOP_TIME_T         */
        break;
    case E_DATETIME_S:                      /**< DateTime_S         */
        break;
    case E_OI:                              /**< OI                 */
        break;
    case E_OAD:                             /**< OOP_OAD_U          */
    {
        if (srcLen < sizeof(OOP_OAD_U) + 1)
        {
            nRet = ERR_OPTION;
            break;
        }
        if (dstLen != sizeof(OOP_OAD_U))
        {
            nRet = ERR_OPTION;
            break;
        }

        if (srcBufTmp[offset++] != DT_OAD)
        {
            nRet = ERR_TYPE;
            break;
        }
        memcpy_rev(dstDataTmp, &srcBufTmp[offset], dstLen);
        offset += dstLen;

        nRet = ERR_OK;
        break;
    }
        
    case E_ROAD:                            /**< OOP_ROAD_T         */
        break;
    case E_OMD:                             /**< OOP_OMD_U          */
        break;
    case E_TI:                              /**< 时间间隔           */
        break;
    case E_TSA:                             /**< 设备地址           */
    {
        uint8 len = 0;
        OOP_TSA_T data;
        MEMZERO(&data, sizeof(OOP_TSA_T));

        if (srcLen > sizeof(OOP_TSA_T) + 2) // 总长度最大值
        {
            nRet = ERR_OPTION;
            break;
        }
        if (srcBufTmp[offset ++] != DT_TSA) //类型
        {
            nRet = ERR_TYPE;
            break;
        }

        len = srcBufTmp[offset ++]; 
        if (len > dstLen)
        {
            nRet = ERR_OPTION;
            break;
        }        
        if (len != srcBufTmp[offset] + 2)
        {
            nRet = ERR_OPTION;
            break;
        }

        data.af = srcBufTmp[offset ++];   //先获取长度，比如是5
        //带扩展逻辑地址
        if (data.flag == 0x01)
        {
            data.vxd = srcBufTmp[offset ++];
            memcpy_r(data.add, &srcBufTmp[offset], data.len);  //TSA地址统一改小端
            offset += data.len;
        }
        else
        {
            memcpy_r(data.add, &srcBufTmp[offset], data.len + 1); //TSA地址统一改小端
            offset += data.len + 1;
        }

        memcpy(&dstDataTmp[0], &data, sizeof(OOP_TSA_T));
        PRTL_BUF_TRACE(dstDataTmp, len, "从报文中获取逻辑地址[%d]和地址：", data.vxd);

        nRet = ERR_OK;
        break;  
    }
        
    case E_MAC:                             /**< OOP_MAC_T签名        */
        break;
    case E_RN:                              /**< 随机数              */
        break;
    case E_REGION:                          /**< 区间类型           */
        break;
    case E_SCALER_UNIT:                     /**< 换算单位           */
        break;
    case E_RSD:                             /**< 行选择符           */
        break;
    case E_CSD:                             /**< 列选择符           */
        break;
    case E_MS:                              /**< 电能表集合            */
        break;
    case E_SID:                             /**< 安全标识           */
        break;
    case E_SIDMAC:                          /**< 安全标识+OOP_MAC_T */
        break;
    case E_COMDCB:                          /**< 串口通讯控制块      */
        break;
    case E_RCSD:                            /**< 记录列选择描述符   */
        break;
    
    default:
        nRet = ERR_OPTION;
        break;
    }

    if (nRet == ERR_OK)
    {
        *pOffset = offset;
        *oadInfoOffset = 1;
    }
    else
    {
        *pOffset = 0;
        *oadInfoOffset = 0;
    }

    return nRet;
}

/**
*********************************************************************
* @name：       data_type_length_get
* @brief：      数据单元长度
* @param[in] ： dataType       - 数据类型
* @param[out]：
* @return    ： 数据长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
uint8 data_type_length_get(enum tag_OOP_DATATYPE dataType)
{
  switch(dataType)
  {
        case DT_NULL:
            return 0;
        case DT_BOOL:
        case DT_UNSIGNED:
        case DT_INTEGER:
        case DT_ENUM:
            return 1;

        case DT_LONG:
        case DT_LONG_UNSIGNED:
        case DT_OI:
        case DT_SCALER_UNIT:
            return 2;

        case DT_TIME:
            return 3;
        //4
        case DT_DOUBLE_LONG:
        case DT_DOUBLE_LONG_UNSIGNED:

        case DT_FLOAT32:
        case DT_OAD:
        case DT_OMD:

            return 4;
        //5
        case DT_DATE:
        case DT_COMDCB:
            return 5;

        //7
        case DT_DATETIME_S:
            return 7;
        //8
        case DT_LONG64:

        case DT_LONG64_UNSIGNED:
        case DT_FLOAT64:
            return 8;
        //9

        //10
        case DT_DATETIME:
            return 10;
        default:
            return 0xFF;
  }
}

/**
*********************************************************************
* @name：       data_type_edata_to_oop
* @brief：      数据类型转换
* @param[in] ：eData       待转类型
* @parma[ouy]: oopData    目的类型
* @return    ：转是否成功 1-成功，0-不成功
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
BOOLEAN data_type_edata_to_oop(DATA_E eData, OOP_DATATYPE_E *oopData)
{
    switch(eData)
    {
        case E_NULL                : {*oopData = DT_NULL                ; return TRUE;}
        case E_ARRAY               : {*oopData = DT_ARRAY               ; return TRUE;}      
        case E_STRUCTURE           : {*oopData = DT_STRUCTURE           ; return TRUE;}       
        case E_BOOL                : {*oopData = DT_BOOL                ; return TRUE;}     
        case E_BIT_STRING          : {*oopData = DT_BIT_STRING          ; return TRUE;}    
        case E_DOUBLE_LONG         : {*oopData = DT_DOUBLE_LONG         ; return TRUE;}   
        case E_DOUBLE_LONG_UNSIGNED: {*oopData = DT_DOUBLE_LONG_UNSIGNED; return TRUE;}
        case E_OCTET_STRING        : {*oopData = DT_OCTET_STRING        ; return TRUE;}   
        case E_VISIBLE_STRING      : {*oopData = DT_VISIBLE_STRING      ; return TRUE;}  
        case E_UTF8_STRING         : {*oopData = DT_UTF8_STRING         ; return TRUE;}      
        case E_INTEGER             : {*oopData = DT_INTEGER             ; return TRUE;}       
        case E_LONG                : {*oopData = DT_LONG                ; return TRUE;}  
        case E_UNSIGNED            : {*oopData = DT_UNSIGNED            ; return TRUE;}
        case E_LONG_UNSIGNED       : {*oopData = DT_LONG_UNSIGNED       ; return TRUE;}
        case E_LONG64              : {*oopData = DT_LONG64              ; return TRUE;}
        case E_LONG64_UNSIGNED     : {*oopData = DT_LONG64_UNSIGNED     ; return TRUE;}
        case E_ENUM                : {*oopData = DT_ENUM                ; return TRUE;}
        case E_FLOAT32             : {*oopData = DT_FLOAT32             ; return TRUE;}
        case E_FLOAT64             : {*oopData = DT_FLOAT64             ; return TRUE;}
        case E_DATETIME            : {*oopData = DT_DATETIME            ; return TRUE;}
        case E_DATE                : {*oopData = DT_DATE                ; return TRUE;}
        case E_TIME                : {*oopData = DT_TIME                ; return TRUE;}
        case E_DATETIME_S          : {*oopData = DT_DATETIME_S          ; return TRUE;}
        case E_OI                  : {*oopData = DT_OI                  ; return TRUE;}
        case E_OAD                 : {*oopData = DT_OAD                 ; return TRUE;}
        case E_ROAD                : {*oopData = DT_ROAD                ; return TRUE;}
        case E_OMD                 : {*oopData = DT_OMD                 ; return TRUE;}
        case E_TI                  : {*oopData = DT_TI                  ; return TRUE;}
        case E_TSA                 : {*oopData = DT_TSA                 ; return TRUE;}
        case E_MAC                 : {*oopData = DT_MAC                 ; return TRUE;}
        case E_RN                  : {*oopData = DT_RN                  ; return TRUE;}
        case E_REGION              : {*oopData = DT_REGION              ; return TRUE;}
        case E_SCALER_UNIT         : {*oopData = DT_SCALER_UNIT         ; return TRUE;}
        case E_RSD                 : {*oopData = DT_RSD                 ; return TRUE;}
        case E_CSD                 : {*oopData = DT_CSD                 ; return TRUE;}
        case E_MS                  : {*oopData = DT_MS                  ; return TRUE;}
        case E_SID                 : {*oopData = DT_SID                 ; return TRUE;}
        case E_SIDMAC              : {*oopData = DT_SIDMAC              ; return TRUE;}
        case E_COMDCB              : {*oopData = DT_COMDCB              ; return TRUE;}
        case E_RCSD                : {*oopData = DT_RCSD                ; return TRUE;}

        default : return FALSE;
    }

    return FALSE;
}

/**
*********************************************************************
* @name：       data_type_oop_to_edata
* @brief：      数据类型转换
* @param[in] ：eData       待转类型
* @parma[ouy]: oopData    目的类型
* @return    ：转是否成功 1-成功，0-不成功
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
BOOLEAN data_type_oop_to_edata(OOP_DATATYPE_E oopData, DATA_E *eData)
{
    switch(oopData)
    {
        case DT_NULL                : {*eData = E_NULL                ; return TRUE;}
        case DT_ARRAY               : {*eData = E_ARRAY               ; return TRUE;}      
        case DT_STRUCTURE           : {*eData = E_STRUCTURE           ; return TRUE;}       
        case DT_BOOL                : {*eData = E_BOOL                ; return TRUE;}     
        case DT_BIT_STRING          : {*eData = E_BIT_STRING          ; return TRUE;}    
        case DT_DOUBLE_LONG         : {*eData = E_DOUBLE_LONG         ; return TRUE;}   
        case DT_DOUBLE_LONG_UNSIGNED: {*eData = E_DOUBLE_LONG_UNSIGNED; return TRUE;}
        case DT_OCTET_STRING        : {*eData = E_OCTET_STRING        ; return TRUE;}   
        case DT_VISIBLE_STRING      : {*eData = E_VISIBLE_STRING      ; return TRUE;}  
        case DT_UTF8_STRING         : {*eData = E_UTF8_STRING         ; return TRUE;}      
        case DT_INTEGER             : {*eData = E_INTEGER             ; return TRUE;}       
        case DT_LONG                : {*eData = E_LONG                ; return TRUE;}  
        case DT_UNSIGNED            : {*eData = E_UNSIGNED            ; return TRUE;}
        case DT_LONG_UNSIGNED       : {*eData = E_LONG_UNSIGNED       ; return TRUE;}
        case DT_LONG64              : {*eData = E_LONG64              ; return TRUE;}
        case DT_LONG64_UNSIGNED     : {*eData = E_LONG64_UNSIGNED     ; return TRUE;}
        case DT_ENUM                : {*eData = E_ENUM                ; return TRUE;}
        case DT_FLOAT32             : {*eData = E_FLOAT32             ; return TRUE;}
        case DT_FLOAT64             : {*eData = E_FLOAT64             ; return TRUE;}
        case DT_DATETIME            : {*eData = E_DATETIME            ; return TRUE;}
        case DT_DATE                : {*eData = E_DATE                ; return TRUE;}
        case DT_TIME                : {*eData = E_TIME                ; return TRUE;}
        case DT_DATETIME_S          : {*eData = E_DATETIME_S          ; return TRUE;}
        case DT_OI                  : {*eData = E_OI                  ; return TRUE;}
        case DT_OAD                 : {*eData = E_OAD                 ; return TRUE;}
        case DT_ROAD                : {*eData = E_ROAD                ; return TRUE;}
        case DT_OMD                 : {*eData = E_OMD                 ; return TRUE;}
        case DT_TI                  : {*eData = E_TI                  ; return TRUE;}
        case DT_TSA                 : {*eData = E_TSA                 ; return TRUE;}
        case DT_MAC                 : {*eData = E_MAC                 ; return TRUE;}
        case DT_RN                  : {*eData = E_RN                  ; return TRUE;}
        case DT_REGION              : {*eData = E_REGION              ; return TRUE;}
        case DT_SCALER_UNIT         : {*eData = E_SCALER_UNIT         ; return TRUE;}
        case DT_RSD                 : {*eData = E_RSD                 ; return TRUE;}
        case DT_CSD                 : {*eData = E_CSD                 ; return TRUE;}
        case DT_MS                  : {*eData = E_MS                  ; return TRUE;}
        case DT_SID                 : {*eData = E_SID                 ; return TRUE;}
        case DT_SIDMAC              : {*eData = E_SIDMAC              ; return TRUE;}
        case DT_COMDCB              : {*eData = E_COMDCB              ; return TRUE;}
        case DT_RCSD                : {*eData = E_RCSD                ; return TRUE;}

        default : return FALSE;
    }

    return FALSE;
}

/**
*********************************************************************
* @name：       apdu_next_dpos_get
* @brief：      获取报文指定位置处，指定类型数据的长度，也即下一个数据位置
*              这个函数，块类型数据只计算其中一个成员长度
* @param[in] ：inData       输入报文
*              inDataLen   报文长度
*              DataType    需计算长度的数据类型
*              bWithType   1-数据类型由入参携带，0-数据类型从报文获取
* @param[out]：offset       输出的指定类型长度，即从inData的开始的DataType偏移量
* @return    ：查询结果 TRUE-成功，FALSE-失败
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
BOOLEAN apdu_next_dpos_get(const uint8 *inData, uint16 *offset, uint16 inDataLen, OOP_DATATYPE_E DataType, BOOLEAN bWithType)
{
    uint16 dataLen;
    uint8 nchioce;
    uint8 dataNum;
    uint8 i;
    uint16 offSet=*offset;
    OOP_DATATYPE_E dataType;
    
    if(bWithType)
    {   
        dataType = DataType;
    }
    else
    {   
        dataType = (OOP_DATATYPE_E)inData[offSet++];
    }
        
    if(dataType == DT_NULL) 
    {   
        *offset = offSet;
        return TRUE;
    }
    if((dataType == DT_STRUCTURE)
        ||(dataType == DT_ARRAY)
        ||(dataType == DT_OCTET_STRING)
        ||(dataType == DT_VISIBLE_STRING)
        ||(dataType == DT_UTF8_STRING)
        ||(dataType == DT_BIT_STRING)
        ||(dataType == DT_MAC)
        ||(dataType == DT_RN)
        ||(dataType == DT_TSA))
    {
        dataLen = inData[offSet++];
        if(dataLen == 0x81)
            dataLen = inData[offSet++];
        else if(dataLen == 0x82)
        {
            dataLen = inData[offSet++];
            dataLen <<= 8;
            dataLen += inData[offSet++];
        }
        else if(dataLen>0x82||dataLen==0x80)
        {      
            return FALSE;
        }
        if((dataType != DT_ARRAY) && (dataType != DT_OCTET_STRING) && (dataType != DT_VISIBLE_STRING) \
            && (dataType != DT_MAC) &&(dataLen == 0)) 
        {      
            return FALSE;
        }
        
        if(dataType == DT_BIT_STRING)
        {      
            dataLen /= 8;
        }
        
        if((dataType == DT_STRUCTURE)
            ||(dataType == DT_ARRAY))
        {

            if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
            {         
                return FALSE;
            }   
        }
        else
        {
            offSet+=dataLen;
        }
    }
    else if(dataType == DT_RSD)
    {
        uint8 eleNum=0;
        nchioce = inData[offSet++];
        switch(nchioce)
        {
            case 0:return FALSE;//
                break;
            case 1:
                if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_OAD,TRUE))
                    return FALSE;
                if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                break;
            case 2:
                if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_OAD,TRUE))
                    return FALSE;
                if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                break;  
            case 3:
                eleNum = inData[offSet++];
                for(i=0;i<eleNum;i++)
                {
                    if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_OAD,TRUE))
                        return FALSE;
                    if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                        return FALSE;
                    if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                        return FALSE;
                    if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                        return FALSE;
                }
                break;
            case 4:
            case 5:
                if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_DATETIME_S,TRUE))
                    return FALSE;
                if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_MS,TRUE))
                    return FALSE;
                break;
            case 6:
            case 7:
            case 8:
                if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_DATETIME_S,TRUE))
                    return FALSE;
                if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_DATETIME_S,TRUE))
                    return FALSE;
                if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_TI,TRUE))
                    return FALSE;
                if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_MS,TRUE))
                    return FALSE;
                break;
            case 9:
                if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_UNSIGNED,TRUE))
                    return FALSE;
                break;
            case 10:
                if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_UNSIGNED,TRUE))
                    return FALSE;
                if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_MS,TRUE))
                    return FALSE;
                break;
            #ifdef AREA_FUJIAN
            case 50:
                if (FALSE == apdu_next_dpos_get(inData, &offSet, inDataLen, DT_LONG_UNSIGNED, TRUE))
                    return FALSE;
                if (FALSE == apdu_next_dpos_get(inData, &offSet, inDataLen, DT_UNSIGNED, TRUE))
                    return FALSE;
                break;
            case 51:
                if (FALSE == apdu_next_dpos_get(inData, &offSet, inDataLen, DT_LONG_UNSIGNED, TRUE))
                    return FALSE;
                if (FALSE == apdu_next_dpos_get(inData, &offSet, inDataLen, DT_DATETIME_S, TRUE))
                    return FALSE;
                if (FALSE == apdu_next_dpos_get(inData, &offSet, inDataLen, DT_MS, TRUE))
                    return FALSE;
                break;
            case 52:
                if (FALSE == apdu_next_dpos_get(inData, &offSet, inDataLen, DT_LONG_UNSIGNED, TRUE))
                    return FALSE;
                dataNum = inData[offSet++];
                if (dataNum == 0x81)
                {
                    dataNum = inData[offSet++];
                }
                else
                {
                    dataNum = inData[offSet++];
                    dataNum <<= 8;
                    dataNum += inData[offSet++];
                }
                for (i = 0; i < dataNum; i++)
                {
                    if (FALSE == apdu_next_dpos_get(inData, &offSet, inDataLen, DT_LONG_UNSIGNED, TRUE))
                        return FALSE;
                }
                if (FALSE == apdu_next_dpos_get(inData, &offSet, inDataLen, DT_DATETIME_S, TRUE))
                    return FALSE;
                break;
            case 53:
                if (FALSE == apdu_next_dpos_get(inData, &offSet, inDataLen, DT_LONG_UNSIGNED, TRUE))
                    return FALSE;
                if (FALSE == apdu_next_dpos_get(inData, &offSet, inDataLen, DT_DATETIME_S, TRUE))
                    return FALSE;
                if (FALSE == apdu_next_dpos_get(inData, &offSet, inDataLen, DT_DATETIME_S, TRUE))
                    return FALSE;
                if (FALSE == apdu_next_dpos_get(inData, &offSet, inDataLen, DT_MS, TRUE))
                    return FALSE;
                break;
            case 54:
                if (FALSE == apdu_next_dpos_get(inData, &offSet, inDataLen, DT_LONG_UNSIGNED, TRUE))
                    return FALSE;
                if (FALSE == apdu_next_dpos_get(inData, &offSet, inDataLen, DT_LONG_UNSIGNED, TRUE))
                    return FALSE;
                if (FALSE == apdu_next_dpos_get(inData, &offSet, inDataLen, DT_UNSIGNED, TRUE))
                    return FALSE;
                if (FALSE == apdu_next_dpos_get(inData, &offSet, inDataLen, DT_MS, TRUE))
                    return FALSE;
                break;
            #endif    
        }
    }
    else if(dataType == DT_CSD)
    {
        nchioce = inData[offSet++];
        if(nchioce == 0)
        {
            dataLen = data_type_length_get(DT_OAD);
            offSet +=dataLen;
        }
        else if(nchioce == 1)
        {
            dataLen = data_type_length_get(DT_OAD);
            offSet +=dataLen;
            dataNum = inData[offSet++];
            offSet +=dataLen*dataNum;
        }
        else 
            return FALSE;

    }
    else if(dataType == DT_ROAD)
    {
        dataLen = data_type_length_get(DT_OAD);
        offSet +=dataLen;
        dataNum = inData[offSet++];
        offSet +=dataLen*dataNum;
        printf("ROAD LEN = %d\n",dataLen+1+dataLen*dataNum);
    }
    else if(dataType == DT_RCSD)
    {
        dataNum = inData[offSet++];
        for(i=0;i<dataNum;i++)
        {
            if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_CSD,TRUE))
                return FALSE;
        }
    }
    else if(dataType == DT_MS)
    {
        nchioce = inData[offSet++];
        if((nchioce == 0)||(nchioce == 1))
        {
        
        }
        else if(nchioce == 2)//一组用户类型
        {
            dataNum = inData[offSet++];
            dataLen = data_type_length_get(DT_UNSIGNED);
            offSet +=dataLen*dataNum;
        }
        else if(nchioce == 3)
        {
            dataNum = inData[offSet++];
            while(dataNum--)
            {
                dataLen = inData[offSet++];
                if((dataLen>8)||(dataLen==0))
                    return FALSE;
                offSet += dataLen;
            }
        }
        else if(nchioce == 4)
        {
            dataNum = inData[offSet++];
            dataLen = data_type_length_get(DT_LONG_UNSIGNED);
            offSet +=dataLen*dataNum;
        }
        else if(nchioce == 5)//一组用户类型区间
        {
            dataNum = inData[offSet++];
            
            while(dataNum--)
            {
                if(inData[offSet++]>3)
                    return FALSE;
                if(inData[offSet++]!=DT_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_UNSIGNED);
                offSet +=dataLen;
                if(inData[offSet++]!=DT_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_UNSIGNED);
                offSet +=dataLen;
            }
        }
        else if(nchioce == 6)//一组用户地址区间
        {
            dataNum = inData[offSet++];         
            while(dataNum--)
            {
                if(inData[offSet++]>3)
                    return FALSE;
                if(inData[offSet++]!=DT_TSA)
                    return FALSE;
                dataLen = inData[offSet++];
                if((dataLen>8)||(dataLen==0))
                    return FALSE;
                offSet +=dataLen;
                if(inData[offSet++]!=DT_TSA)
                    return FALSE;
                dataLen = inData[offSet++];
                if((dataLen>8)||(dataLen==0))
                    return FALSE;
                offSet +=dataLen;
            }
        }
        else if(nchioce == 7)//一组配置序号区间
        {
            dataNum = inData[offSet++];
            
            while(dataNum--)
            {
                if(inData[offSet++]>3)
                    return FALSE;
                if(inData[offSet++]!=DT_LONG_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_LONG_UNSIGNED);
                offSet +=dataLen;
                if(inData[offSet++]!=DT_LONG_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_LONG_UNSIGNED);
                offSet +=dataLen;
            }
        }
        else
            return FALSE;

    }
    else if(dataType == DT_REGION)
    {
        if(inData[offSet++]>3)
            return FALSE;
        if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
            return FALSE;
        if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
            return FALSE;
    }
    else if(dataType == DT_SID)
    {
        if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_DOUBLE_LONG_UNSIGNED,TRUE))
            return FALSE;
        if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_OCTET_STRING,TRUE))
            return FALSE;   
    }
    else if(dataType == DT_MAC)
    {
        if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_SID,TRUE))
            return FALSE;
        if(FALSE == apdu_next_dpos_get(inData,&offSet,inDataLen,DT_MAC,TRUE))
            return FALSE;
    }
    else if(dataType == DT_TI)
    {
        if(inData[offSet++]>5)
            return FALSE;
        offSet += 2;
    }
    else
    {
        
        dataLen = data_type_length_get(dataType);
        if(0xFF != dataLen) 
            offSet +=dataLen;
        else
            return FALSE;
    }
    if(offSet>inDataLen)
        return FALSE;
    *offset = offSet;
    return TRUE;
}

/**
*********************************************************************
* @name：       apdu_next_position_get
* @brief：      获取报文指定位置处，指定类型数据的长度，也即下一个数据位置
* @param[in] ：inData       输入报文
*              inDataLen   报文长度
*              DataType    需计算长度的数据类型
*              bWithType   1-数据类型由入参携带，0-数据类型从报文获取
* @param[out]：offset       输出的指定类型长度，即从inData的开始的DataType偏移量
* @return    ：查询结果 TRUE-成功，FALSE-失败
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
BOOLEAN apdu_next_position_get(const uint8 *inData, uint32 *offset, uint32 inDataLen, OOP_DATATYPE_E DataType, BOOLEAN bWithType)
{
    uint32 dataLen;
    uint8 nchioce;
    uint8 dataNum;
    uint8 i;
    uint32 offSet=*offset;
    OOP_DATATYPE_E dataType;
    
    if(bWithType)
    {   
        dataType = DataType;
    }
    else
    {   
        dataType = (OOP_DATATYPE_E)inData[offSet++];
    }
        
    if(dataType == DT_NULL) 
    {   
        *offset = offSet;
        return TRUE;
    }
    if((dataType == DT_STRUCTURE)
        ||(dataType == DT_ARRAY)
        ||(dataType == DT_OCTET_STRING)
        ||(dataType == DT_VISIBLE_STRING)
        ||(dataType == DT_UTF8_STRING)
        ||(dataType == DT_BIT_STRING)
        ||(dataType == DT_MAC)
        ||(dataType == DT_RN)
        ||(dataType == DT_TSA))
    {
        dataLen = inData[offSet++];
        if(dataLen == 0x81)
            dataLen = inData[offSet++];
        else if(dataLen == 0x82)
        {
            dataLen = inData[offSet++];
            dataLen <<= 8;
            dataLen += inData[offSet++];
        }
        else if(dataLen == 0x83)
        {
            dataLen = inData[offSet++];
            dataLen <<= 8;
            dataLen += inData[offSet++];
            dataLen <<= 8;
            dataLen += inData[offSet++];
        }
        else if(dataLen>0x83||dataLen==0x80)
        {      
            return FALSE;
        }
        if((dataType != DT_ARRAY) && (dataType != DT_OCTET_STRING) && (dataType != DT_VISIBLE_STRING) \
            && (dataType != DT_MAC) &&(dataLen == 0)) 
        {      
            return FALSE;
        }
        
        if(dataType == DT_BIT_STRING)
        {      
            dataLen /= 8;
        }
        
        if((dataType == DT_STRUCTURE)
            ||(dataType == DT_ARRAY))
        {
            uint16 eleNum = dataLen;
            while(eleNum--)
            {
                if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                {         
                    return FALSE;
                }   
            }
        }
        else
        {
            offSet+=dataLen;
        }
    }
    else if(dataType == DT_RSD)
    {
        uint8 eleNum=0;
        nchioce = inData[offSet++];
        switch(nchioce)
        {
            case 0://
                break;
            case 1:
                if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_OAD,TRUE))
                    return FALSE;
                if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                break;
            case 2:
                if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_OAD,TRUE))
                    return FALSE;
                if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                break;  
            case 3:
                eleNum = inData[offSet++];
                for(i=0;i<eleNum;i++)
                {
                    if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_OAD,TRUE))
                        return FALSE;
                    if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                        return FALSE;
                    if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                        return FALSE;
                    if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                        return FALSE;
                }
                break;
            case 4:
            case 5:
                if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_DATETIME_S,TRUE))
                    return FALSE;
                if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_MS,TRUE))
                    return FALSE;
                break;
            case 6:
            case 7:
            case 8:
                if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_DATETIME_S,TRUE))
                    return FALSE;
                if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_DATETIME_S,TRUE))
                    return FALSE;
                if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_TI,TRUE))
                    return FALSE;
                if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_MS,TRUE))
                    return FALSE;
                break;
            case 9:
                if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_UNSIGNED,TRUE))
                    return FALSE;
                break;
            case 10:
                if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_UNSIGNED,TRUE))
                    return FALSE;
                if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_MS,TRUE))
                    return FALSE;
                break;
            #ifdef AREA_FUJIAN
            case 50:
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_LONG_UNSIGNED, TRUE))
                    return FALSE;
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_UNSIGNED, TRUE))
                    return FALSE;
                break;
            case 51:
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_LONG_UNSIGNED, TRUE))
                    return FALSE;
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_DATETIME_S, TRUE))
                    return FALSE;
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_MS, TRUE))
                    return FALSE;
                break;
            case 52:
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_LONG_UNSIGNED, TRUE))
                    return FALSE;
                dataNum = inData[offSet++];
                if (dataNum == 0x81)
                {
                    dataNum = inData[offSet++];
                }
                else
                {
                    dataNum = inData[offSet++];
                    dataNum <<= 8;
                    dataNum += inData[offSet++];
                }
                for (i = 0; i < dataNum; i++)
                {
                    if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_LONG_UNSIGNED, TRUE))
                        return FALSE;
                }
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_DATETIME_S, TRUE))
                    return FALSE;
                break;
            case 53:
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_LONG_UNSIGNED, TRUE))
                    return FALSE;
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_DATETIME_S, TRUE))
                    return FALSE;
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_DATETIME_S, TRUE))
                    return FALSE;
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_MS, TRUE))
                    return FALSE;
                break;
            case 54:
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_LONG_UNSIGNED, TRUE))
                    return FALSE;
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_LONG_UNSIGNED, TRUE))
                    return FALSE;
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_UNSIGNED, TRUE))
                    return FALSE;
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_MS, TRUE))
                    return FALSE;
                break;
            #endif    
        }
    }
    else if(dataType == DT_CSD)
    {
        nchioce = inData[offSet++];
        if(nchioce == 0)
        {
            dataLen = data_type_length_get(DT_OAD);
            offSet +=dataLen;
        }
        else if(nchioce == 1)
        {
            dataLen = data_type_length_get(DT_OAD);
            offSet +=dataLen;
            dataNum = inData[offSet++];
            offSet +=dataLen*dataNum;
        }
        else 
            return FALSE;

    }
    else if(dataType == DT_ROAD)
    {
        dataLen = data_type_length_get(DT_OAD);
        offSet +=dataLen;
        dataNum = inData[offSet++];
        offSet +=dataLen*dataNum;
        printf("ROAD LEN = %d\n",dataLen+1+dataLen*dataNum);
    }
    else if(dataType == DT_RCSD)
    {
        dataNum = inData[offSet++];
        for(i=0;i<dataNum;i++)
        {
            if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_CSD,TRUE))
                return FALSE;
        }
    }
    else if(dataType == DT_MS)
    {
        nchioce = inData[offSet++];
        if((nchioce == 0)||(nchioce == 1))
        {
        
        }
        else if(nchioce == 2)//一组用户类型
        {
            dataNum = inData[offSet++];
            dataLen = data_type_length_get(DT_UNSIGNED);
            offSet +=dataLen*dataNum;
        }
        else if(nchioce == 3)
        {
            dataNum = inData[offSet++];
            while(dataNum--)
            {
                dataLen = inData[offSet++];
                if((dataLen>8)||(dataLen==0))
                    return FALSE;
                offSet += dataLen;
            }
        }
        else if(nchioce == 4)
        {
            dataNum = inData[offSet++];
            dataLen = data_type_length_get(DT_LONG_UNSIGNED);
            offSet +=dataLen*dataNum;
        }
        else if(nchioce == 5)//一组用户类型区间
        {
            dataNum = inData[offSet++];
            
            while(dataNum--)
            {
                if(inData[offSet++]>3)
                    return FALSE;
                if(inData[offSet++]!=DT_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_UNSIGNED);
                offSet +=dataLen;
                if(inData[offSet++]!=DT_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_UNSIGNED);
                offSet +=dataLen;
            }
        }
        else if(nchioce == 6)//一组用户地址区间
        {
            dataNum = inData[offSet++];         
            while(dataNum--)
            {
                if(inData[offSet++]>3)
                    return FALSE;
                if(inData[offSet++]!=DT_TSA)
                    return FALSE;
                dataLen = inData[offSet++];
                if((dataLen>8)||(dataLen==0))
                    return FALSE;
                offSet +=dataLen;
                if(inData[offSet++]!=DT_TSA)
                    return FALSE;
                dataLen = inData[offSet++];
                if((dataLen>8)||(dataLen==0))
                    return FALSE;
                offSet +=dataLen;
            }
        }
        else if(nchioce == 7)//一组配置序号区间
        {
            dataNum = inData[offSet++];
            
            while(dataNum--)
            {
                if(inData[offSet++]>3)
                    return FALSE;
                if(inData[offSet++]!=DT_LONG_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_LONG_UNSIGNED);
                offSet +=dataLen;
                if(inData[offSet++]!=DT_LONG_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_LONG_UNSIGNED);
                offSet +=dataLen;
            }
        }
        else
            return FALSE;

    }
    else if(dataType == DT_REGION)
    {
        if(inData[offSet++]>3)
            return FALSE;
        if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
            return FALSE;
        if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
            return FALSE;
    }
    else if(dataType == DT_SID)
    {
        if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_DOUBLE_LONG_UNSIGNED,TRUE))
            return FALSE;
        if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_OCTET_STRING,TRUE))
            return FALSE;   
    }
    else if(dataType == DT_SIDMAC)
    {
        if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_SID,TRUE))
            return FALSE;
        if(FALSE == apdu_next_position_get(inData,&offSet,inDataLen,DT_MAC,TRUE))
            return FALSE;
    }
    else if(dataType == DT_TI)
    {
        if(inData[offSet++]>5)
            return FALSE;
        offSet += 2;
    }
    else
    {
        
        dataLen = data_type_length_get(dataType);
        if(0xFF != dataLen) 
            offSet +=dataLen;
        else
            return FALSE;
    }
    if(offSet>inDataLen)
        return FALSE;
    *offset = offSet;
    return TRUE;
}


/**
*********************************************************************
* @name：       data_type_length_check
* @brief：      检查数据类型长度
* @param[in]： dataType   数据类型
* @parma[in]:  dataLen    数据长度
* @return    ：检查是否通过 1-通过，0-不通过
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
BOOLEAN data_type_length_check(DATA_E dataType, uint32 dataLen)
{
    OOP_DATATYPE_E oopType;

    if(data_type_edata_to_oop(dataType, &oopType))
    {
        if(data_type_length_get(oopType) == dataLen)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/**
*********************************************************************
* @name：       basic_type_check
* @brief：      检查ODA是否为普通OAD
* @param[in]： oda   数据类型
* @return    ：检查是否通过 1-通过，0-不通过
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
BOOLEAN normal_oad_check(OOP_OAD_U oda)
{
    //TODO 未完待续
    if((oda.value != 0x30090200) || (oda.value != 0x300A0200))
    {
        return TRUE;
    }

    return FALSE;
}


/**
*********************************************************************
* @name：        normal_data_to_fmt698
* @brief：       非记录型的OAD结构数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out]:  dOffset    构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
                sOffset    取出该数据后的源数据区偏移


* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E normal_data_to_fmt698(const IC_OAD_INFO_T *pIcInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    OAD_INFO_T pstOdaInfo;
    uint32 i = 0;

    /*入参检查*/
    if ((pIcInfo == NULL) || (convert == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL)|| 
        (convert->sOffset == NULL) || (convert->dOffset == NULL))
    {
        PRTL_FMT_DEBUG("normal_data_to_fmt698 check failed. ERR_PTR\n");
        return ERR_PTR;
    }    

    MEMZERO(&pstOdaInfo, sizeof(OAD_INFO_T));   

    if(pIcInfo->choice == 0)
    {
        if(!class_oadinfo_get(pIcInfo->oad, &pstOdaInfo))
        {
            PRTL_FMT_DEBUG("class_oadinfo_get failed.\n");
            return DATA_SCOPE_OF_ACCESS_VIOLATED;
        }
    }
    else
    {
        memcpy(&pstOdaInfo, pIcInfo->pstOdaInfo, sizeof(OAD_INFO_T));
    }

    // 如果源数据长度为0，数据填NULL
    if (convert->srcLen == 0)
    {
        dar = data_to_buf_null(&pstOdaInfo, convert);
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(convert->srcBuf, convert->srcLen, "Data2Buf failed. dar(%d), class(%d), oad(0x%08x)\n", 
                dar, pstOdaInfo.pTab->class, pstOdaInfo.pTab->oad.value);
        }
        PRTL_FMT_DEBUG("srcLen 0, data is null. oad(0x%08x)\n", pstOdaInfo.pTab->oad.value);
        return dar;
    }

    pstOdaInfo.subParse = pIcInfo->subParse;

    for(i = 0; i < OOP_IC_MAX; i++)
    {
        if(pstOdaInfo.pTab->class == g_ic_cov_op[i].ClassID)
        {
            dar = g_ic_cov_op[i].Data2Buf(&pstOdaInfo, convert);
            if(dar != DATA_SUCCESS)
            {
                PRTL_BUF_DEBUG(convert->srcBuf, convert->srcLen, "Data2Buf failed. dar(%d), class(%d), oad(0x%08x)\n", 
                    dar, pstOdaInfo.pTab->class, pstOdaInfo.pTab->oad.value);
            }
            break;
        }
    }

    if(i == OOP_IC_MAX)
    {
        PRTL_FMT_DEBUG("class%d: Data2Buf not support.\n", pstOdaInfo.pTab->class);
        dar =  DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    return dar;
}


/**
*********************************************************************
* @name：        transparent_data_to_fmt698
* @brief：       非记录型的OAD结构数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out]:  dOffset    构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
                sOffset    取出该数据后的源数据区偏移


* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E transparent_data_to_fmt698(const IC_OAD_INFO_T *pIcInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    OAD_INFO_T pstOdaInfo;
    uint32 i = 0;
    uint8 *dstBufTmp=(uint8*)convert->dstBuf;
    uint32 dOffset=*convert->dOffset;
    uint32 dstLen=convert->dstLen;
    OOP_TRANSRES_T TRANSRES_T;
    OOP_DAR_E nRet = DATA_SUCCESS;
    uint8 bytes;
    uint16 datalen;
    
    /*入参检查*/
    if ((pIcInfo == NULL) || (convert == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL)|| 
        (convert->sOffset == NULL) || (convert->dOffset == NULL))
    {
        PRTL_FMT_DEBUG("normal_data_to_fmt698 check failed. ERR_PTR\n");
        return ERR_PTR;
    }    

    MEMZERO(&pstOdaInfo, sizeof(OAD_INFO_T));   

    

    memcpy((uint8*)&TRANSRES_T,convert->srcBuf,convert->srcLen);

    //方案编号
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, (uint32)(dstLen-dOffset), &dOffset, &TRANSRES_T.planID, 1) < 0)
    {
        nRet = DATA_TYPE_UNMATCHED;
        return nRet;
    }
    
    //方案执行时间
    if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, (uint32)(dstLen-dOffset), &dOffset, &TRANSRES_T.eTime, sizeof(OOP_DATETIME_S_T)) < 0)
    {
        nRet = DATA_TYPE_UNMATCHED;
        return nRet;
    }

    //方案执行时间
    if(basic_data_to_buf(E_TSA, dstBufTmp, (uint32)(dstLen-dOffset), &dOffset, &TRANSRES_T.addr, sizeof(OOP_TSA_T)) < 0)
    {
        nRet = DATA_TYPE_UNMATCHED;
        return nRet;
    }

    //结果集
    dstBufTmp[dOffset++]=DT_ARRAY;
    dstBufTmp[dOffset++]=TRANSRES_T.res.nNum;
    for(i=0;i<TRANSRES_T.res.nNum;i++)
    {
          dstBufTmp[dOffset++]=DT_STRUCTURE;
          dstBufTmp[dOffset++]=0x03;
          if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, (uint32)(dstLen-dOffset), &dOffset, &TRANSRES_T.res.result[i].nIndex, 1) < 0)
          {
                nRet = DATA_TYPE_UNMATCHED;
                return nRet;
          }
          if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, (uint32)(dstLen-dOffset), &dOffset, &TRANSRES_T.res.result[i].aTime, sizeof(OOP_DATETIME_S_T)) < 0)
          {
                nRet = DATA_TYPE_UNMATCHED;
                return nRet;
          }
          datalen=set_len_offset(TRANSRES_T.res.result[i].data.nNum, &bytes);
          dOffset=dOffset+bytes;
          memcpy(dstBufTmp+dOffset,TRANSRES_T.res.result[i].data.value,datalen);
          dOffset=dOffset+datalen;      
    }
  

    
    
    
    return dar;
}

/**
*********************************************************************
* @name：       basic_type_check
* @brief：      检查数据类型是否符合能处理的基本型
* @param[in]： dataType   数据类型
* @return    ：检查是否通过 1-通过，0-不通过
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
BOOLEAN basic_type_check(DATA_E dstType)
{
    switch(dstType)
    {
        case E_NULL                : return TRUE;
        case E_ARRAY               : return TRUE;    
        case E_STRUCTURE           : return TRUE;
        case E_BOOL                : return TRUE;  
        case E_BIT_STRING          : return TRUE;
        case E_OOP_BITSTR3_T       : return TRUE;
        case E_OOP_BITSTR8_T       : return TRUE;
        case E_OOP_BITSTR64_T      : return TRUE;
        case E_OOP_BITSTR128_T     : return TRUE;
        case E_DOUBLE_LONG         : return TRUE;    
        case E_DOUBLE_LONG_UNSIGNED: return TRUE;
        case E_OCTET_STRING        : return TRUE;  
        case E_VISIBLE_STRING      : return TRUE;
        case E_UTF8_STRING         : return TRUE;    
        case E_INTEGER             : return TRUE;
        case E_LONG                : return TRUE;  
        case E_UNSIGNED            : return TRUE;
        case E_LONG_UNSIGNED       : return TRUE;    
        case E_LONG64              : return TRUE;
        case E_LONG64_UNSIGNED     : return TRUE;  
        case E_ENUM                : return TRUE;
        case E_FLOAT32             : return TRUE;    
        case E_FLOAT64             : return TRUE;
        case E_DATETIME            : return TRUE;  
        case E_DATE                : return TRUE;
        case E_TIME                : return TRUE;    
        case E_DATETIME_S          : return TRUE;
        case E_OI                  : return TRUE;  
        case E_OAD                 : return TRUE;
        case E_ROAD                : return TRUE;    
        case E_OMD                 : return TRUE;
        case E_TI                  : return TRUE;  
        case E_TSA                 : return TRUE;
        case E_MAC                 : return TRUE;    
        case E_RN                  : return TRUE;
        case E_REGION              : return TRUE;  
        case E_SCALER_UNIT         : return TRUE;
        case E_RSD                 : return TRUE;    
        case E_CSD                 : return TRUE;
        case E_MS                  : return TRUE; 
        case E_MS_F                : return TRUE;
        case E_MS_C                : return TRUE; 
        case E_SID                 : return TRUE;
        case E_SIDMAC              : return TRUE;    
        case E_COMDCB              : return TRUE;
        case E_RCSD                : return TRUE;  
        case E_OOP_VISIBLEVAR_T    : return TRUE; 
        case E_OOP_VISIBLESTR4_T   : return TRUE; 
        case E_OOP_VISIBLESTR6_T   : return TRUE; 
        case E_OOP_VISIBLESTR8_T   : return TRUE; 
        case E_OOP_VISIBLESTR11_T  : return TRUE;         
        case E_OOP_VISIBLESTR256_T  : return TRUE; 
        case E_OOP_VISIBLEVAR32_T  : return TRUE;
        case E_OOP_OCTETVAR16_T    : return TRUE;
        case E_OOP_OCTETVAR64_T    : return TRUE;
        case E_OOP_OCTETVAR256_T   : return TRUE;
        case E_OOP_OCTETVAR1K_T    : return TRUE;
        case E_OOP_OCTETVAR2K_T    : return TRUE;
        case E_OCTET_STRING_2K     : return TRUE;
        case E_OCTET_STR2_T        : return TRUE;
        case E_OCTET_STR3_T        : return TRUE;
        case E_OCTET_STR4_T        : return TRUE;
        case E_OCTET_STR6_T        : return TRUE;
        case E_OCTET_STR8_T        : return TRUE;
        case E_OCTET_STR32_T       : return TRUE;
        case E_OOP_SA_T            : return TRUE;
        case E_OOP_BITSTR16_T      : return TRUE;
        case E_PCSD                : return TRUE;
        case E_CHECKSUM            : return TRUE;

        default: return FALSE;        
    }

    return FALSE;
}

/**
*********************************************************************
* @name：       all_rcsd_get
* @brief：      获取对应OAD的关联OAD
* @param[in]： dataType   数据类型
* @return    ：检查是否通过 1-通过，0-不通过
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 all_rcsd_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *apduInfo, uint16 *pOffset, uint8 *pOutData, uint16 outDataMax)
{
    uint32 offset = *pOffset;

    if (pOadInfo->pTab == NULL)
    {
        PRTL_FMT_DEBUG("all_rcsd_get pOadInfo->pTab[NULL]\n");

        //SEQUENCE OF CSD  
        pOutData[offset++] = 0;

        *pOffset = offset;

        return 0;
    }

    switch(pOadInfo->pTab->class)
    {
        case 7:
        {
            return evt_all_rcsd_get(pOadInfo, pOffset, pOutData, outDataMax);
        }break;
        case 9:
        {
            return frozen_all_rcsd_get(pOadInfo, apduInfo, pOffset, pOutData, outDataMax);
        }break;

        default:break;
    }

    return 0;
}

/**
*********************************************************************
* @name：       class_array_tofmt698
* @brief：      将数组类型数据转换成698格式报文
* @param[in]： pOadInfo   OAD信息
* @param[in]： pArrayInfo   数组信息
* @param[out]：convert   转换信息

* @return    ：= 0成功, != 0, 失败
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class_array_tofmt698(OAD_INFO_T *pOadInfo, ARRAY_INFO_T *pArrayInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    int ret = ERR_OK;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;   
    //uint32 tmpoffset=0;
    DATA_CONVERT_T unitConvert;
    uint32 unitSoffset = 0;
    uint32 i = 0; 
    uint8 offlen = 0;       //数组数量长度
    uint8 lenBuf[3] = {0};  //数组数量编码
    //uint8 validnum=0;

    MEMZERO(&unitConvert, sizeof(unitConvert));

    if(pArrayInfo->unitNum > pArrayInfo->maxNum)
    {
        PRTL_FMT_DEBUG("srcData cross the border. nNum(%d) > maxNum(%d)\n", pArrayInfo->unitNum, pArrayInfo->maxNum);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    if(pOadInfo->noParse)
    {
        if(srcLen-sOffset < pArrayInfo->arrLen)
        {
            PRTL_FMT_DEBUG("srcData cross the border. srcLen(%d)-sOffset(%d) < arrLen(%d)\n", srcLen, sOffset, pArrayInfo->arrLen);
            return DATA_SCOPE_OF_ACCESS_VIOLATED;
        }
        
        if(pOadInfo->nIndex == 0)
        {
            ret = memcpy_s(dstBuf+dOffset, dstLen-dOffset, srcBuf+sOffset, pArrayInfo->arrLen);
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("memcpy_s failed. ret(%d)\n", ret);
                return DATA_OTHER_REASON;
            }
            
            dOffset += pArrayInfo->arrLen;
            sOffset += pArrayInfo->arrLen;
        }
        else
        {
            ret = memcpy_s(dstBuf+dOffset, dstLen-dOffset, (uint8*)pArrayInfo->pArray+(pOadInfo->nIndex-1)*pArrayInfo->unitSize, pArrayInfo->unitSize);
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("memcpy_s failed. ret(%d)\n", ret);
                return DATA_OTHER_REASON;
            }
            
            dOffset += pArrayInfo->unitSize; 
            sOffset += pArrayInfo->unitSize;            
        }
    }
    else
    {
        unitConvert.dstBuf = dstBuf;
        unitConvert.dOffset = &dOffset;
        unitConvert.dstLen = dstLen;
        
        if(pOadInfo->nIndex == 0)
        {
            if(srcLen-sOffset < pArrayInfo->arrLen)
            {
                PRTL_FMT_DEBUG("srcData cross the border. srcLen(%d)-sOffset(%d) < arrLen(%d)\n", srcLen, sOffset, pArrayInfo->arrLen);
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            unitConvert.srcBuf = pArrayInfo->pArray;
            unitConvert.srcLen = pArrayInfo->unitSize*pArrayInfo->unitNum;
            
            //数组类型
            dstBuf[dOffset++] = DT_ARRAY;
            
            //数组成员个数
            //tmpoffset=dOffset;
            offlen = set_len_offset(pArrayInfo->unitNum, lenBuf);
            
            ret = memcpy_s(&dstBuf[dOffset], dstLen-dOffset, lenBuf, offlen);
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("memcpy_s failed. ret(%d)\n", ret);
                return DATA_OTHER_REASON;
            }
            
            dOffset += offlen;

            for(i = 0; i < pArrayInfo->unitNum; i++)
            {
                unitSoffset = i*pArrayInfo->unitSize;
                unitConvert.sOffset = &unitSoffset;
                if((pOadInfo->subeData == E_LONG_UNSIGNED)||(pOadInfo->subeData ==E_LONG)||(pOadInfo->subeData == E_DOUBLE_LONG_UNSIGNED)||(pOadInfo->subeData == E_DOUBLE_LONG)||(pOadInfo->subeData ==E_OOP_LNLS_UNIT_T))
                {
                    if(0==get_bit_value((uint8*)&pArrayInfo->rsv,32,i))
                    {
                        dstBuf[dOffset++] = DT_NULL;
                        continue;
                    }
                    //validnum++;
                }
                dar = pArrayInfo->func(pOadInfo, &unitConvert);
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("[%d] block convert failed. oad(0x%08x), sOffset(%d), dOffset(%d)\n", 
                        i, pOadInfo->pTab->oad.value, unitConvert.sOffset, unitConvert.dOffset);
                    
                    return dar;                
                }
            }
            if((pOadInfo->subeData == E_LONG_UNSIGNED)||(pOadInfo->subeData ==E_LONG))
            {
                //dstBuf[tmpoffset]=validnum;
            }
            sOffset += pArrayInfo->arrLen;
        }
        else //其中一个单元
        {
            if(pOadInfo->subParse) //传入的是子结构
            {
                if(srcLen-sOffset < pArrayInfo->unitSize)
                {
                    PRTL_FMT_DEBUG("srcData cross the border. srcLen(%d)-sOffset(%d) < unitSize(%d)\n", srcLen, sOffset, pArrayInfo->unitSize);
                    return DATA_SCOPE_OF_ACCESS_VIOLATED;
                }

                unitConvert.srcBuf = srcBuf;
                unitConvert.srcLen = srcLen;  
                unitSoffset = sOffset;
            }
            else
            {
                if((pOadInfo->nIndex-1 > pArrayInfo->maxNum) || (pOadInfo->nIndex-1 >= pArrayInfo->unitNum))
                {
                    PRTL_FMT_DEBUG("srcData cross the border. nIndex(%d) > maxNum(%d)|| >= unitNum(%d)\n", pOadInfo->nIndex-1, pArrayInfo->maxNum, pArrayInfo->unitNum);
                    return DATA_SCOPE_OF_ACCESS_VIOLATED;
                }
                
                unitConvert.srcBuf = pArrayInfo->pArray;
                unitConvert.srcLen = pArrayInfo->unitSize*pArrayInfo->unitNum;                
                unitSoffset = (pOadInfo->nIndex-1)*pArrayInfo->unitSize;
            }
            
            unitConvert.sOffset = &unitSoffset;
            
            dar = pArrayInfo->func(pOadInfo, &unitConvert);
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("unit convert failed. oad(0x%08x), sOffset(%d), dOffset(%d)\n", 
                    pOadInfo->pTab->oad.value, unitConvert.sOffset, unitConvert.dOffset);
                
                return dar;                
            }

            sOffset += unitSoffset;
        }
    }   

    if((dOffset > dstLen) ||(sOffset > srcLen))
    {
        PRTL_FMT_DEBUG("cross the border. dOffset(%d) > dstLen(%d) or sOffset(%d) > srcLen(%d)\n", dOffset, dstLen, sOffset, srcLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;       
    
    return dar;
}

OOP_DAR_E class_struct_tofmt698()
{
    OOP_DAR_E dar = DATA_SUCCESS;
    
    return dar;
}

/**
*********************************************************************
* @brief：      转换函数错误码到规约对应的错误码
* @param[in]:  err 错误码
* @return：     dar 698规约对应的错误码
* @author:     贺宁
* @Date：       2020-04-23
*********************************************************************
*/
OOP_DAR_E oop_errcode_to_dar(int err)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    
    switch(err)
    {
        case DBMSG_ERR_OUT_TIME: dar = DATA_REQUEST_TIMEOUT; break;
        default: dar = DATA_OTHER_REASON;
    }

    return dar;
}

#endif

#if DESC("vardata类型解析",1)

/**
*********************************************************************
* @brief：      上报方案的记录型列选择内容编码
* @param[in]： report      原始上报方案参数
* @param[in]： offset      编码时的偏移
* @param[out]：pReportCfg 编码后的上报方案参数
* @return：     void
*********************************************************************
*/
void csd_data_to_buf(OOP_CSD_T csd, uint32 *offset, uint8 *apduValue)
{
    uint8 i = 0;

    /*CSD∷=CHOICE
     {
        对象属性描述符              [0] OAD，
        记录型对象属性描述符 [1] ROAD
     }
     ROAD∷=SEQUENCE
     {
         对象属性描述符            OAD，
         关联对象属性描述符 SEQUENCE OF OAD
     }*/
     
    apduValue[(*offset)++] = csd.choice; //选择项
    
    if(csd.choice == 0) //单OAD类型
    {
        memcpy_r(&apduValue[*offset], &csd.oad, sizeof(uint32)); //OAD
        (*offset) += 4;
    }
    else //记录型ROAD
    {
        memcpy_r(&apduValue[*offset], &csd.road.oadMain, sizeof(uint32)); //OAD
        (*offset) += 4;   
    
        apduValue[(*offset)++] = csd.road.oadCols.nNum; //SEQUENCE个数
        
        for(i = 0; i < csd.road.oadCols.nNum; i++)
        {
            memcpy_r(&apduValue[*offset], &csd.road.oadCols.oad[i], sizeof(uint32)); //OAD
            (*offset) += 4;                       
        }
    }

    return;
}

/**
*********************************************************************
* @brief：      TSA的读取
* @param[in]： report      原始上报方案参数
* @param[in]： offset      编码时的偏移
* @param[out]：pReportCfg 编码后的上报方案参数
* @return：     void
*********************************************************************
*/
void tsa_data_to_buf(OOP_TSA_T tsa, uint32 *offset, uint8 *apduValue)
{
    apduValue[(*offset)++] = tsa.af;  

    memcpy(&apduValue[*offset], tsa.add, tsa.len+1);
    (*offset) += tsa.len+1;

    return;
}

/**
*********************************************************************
* @brief：      可变数据的读取
* @param[in]： report      原始上报方案参数
* @param[in]： offset      编码时的偏移
* @param[out]：pReportCfg 编码后的上报方案参数
* @return：     void
*********************************************************************
*/
void ti_data_to_buf(OOP_TI_T ti, uint32 *offset, uint8 *apduValue)
{
    /*TI∷=SEQUENCE
     {
         单位 ENUMERATED
         { 
             秒 （0），
             分 （1），
             时 （2），
             日 （3），
             月 （4），
             年 （5） 
         }，
         间隔值 long-unsigned
     }*/

    apduValue[(*offset)++] = ti.unit;

    memcpy_r(&apduValue[*offset], (uint8*)&ti.value, sizeof(uint16));
    (*offset) += sizeof(uint16);

    return;
}

/**
*********************************************************************
* @brief：      可变数据的读取
* @param[in]： report      原始上报方案参数
* @param[in]： offset      编码时的偏移
* @param[out]：pReportCfg 编码后的上报方案参数
* @return：     void
*********************************************************************
*/
void datetime_s_data_to_buf(OOP_DATETIME_S_T tmVal, uint32 *offset, uint8 *apduValue)
{
    apduValue[(*offset)++] = (tmVal.year>>8)&0xff;
    apduValue[(*offset)++] = tmVal.year&0xff;
    apduValue[(*offset)++] = tmVal.month;  
    apduValue[(*offset)++] = tmVal.day;           
    apduValue[(*offset)++] = tmVal.hour; 
    apduValue[(*offset)++] = tmVal.minute; 
    apduValue[(*offset)++] = tmVal.second; 

    return;
}

/**
*********************************************************************
* @brief：      可变数据的读取
* @param[in]： report      原始上报方案参数
* @param[in]： offset      编码时的偏移
* @param[out]：pReportCfg 编码后的上报方案参数
* @return：     void
*********************************************************************
*/
void datetime_data_to_buf(OOP_DATETIME_T dtVal, uint32 *offset, uint8 *apduValue)
{
    apduValue[(*offset)++] = (dtVal.year>>8)&0xff;
    apduValue[(*offset)++] = dtVal.year&0xff;
    apduValue[(*offset)++] = dtVal.month;  
    apduValue[(*offset)++] = dtVal.mday; 
    apduValue[(*offset)++] = dtVal.wday;            
    apduValue[(*offset)++] = dtVal.hour; 
    apduValue[(*offset)++] = dtVal.minute; 
    apduValue[(*offset)++] = dtVal.second;   
    apduValue[(*offset)++] = (dtVal.msec>>8)&0xff;
    apduValue[(*offset)++] = dtVal.msec&0xff;

    return;
}

/**
*********************************************************************
* @brief：      可变数据的读取
* @param[in]： report      原始上报方案参数
* @param[in]： offset      编码时的偏移
* @param[out]：pReportCfg 编码后的上报方案参数
* @return：     void
*********************************************************************
*/
void date_data_to_buf(OOP_DATE_T dVal, uint32 *offset, uint8 *apduValue)
{
    apduValue[(*offset)++] = (dVal.year>>8)&0xff;
    apduValue[(*offset)++] = dVal.year&0xff;
    apduValue[(*offset)++] = dVal.month;  
    apduValue[(*offset)++] = dVal.mday; 
    apduValue[(*offset)++] = dVal.wday;            

    return;
}

/**
*********************************************************************
* @brief：      可变数据的读取
* @param[in]： report      原始上报方案参数
* @param[in]： offset      编码时的偏移
* @param[out]：pReportCfg 编码后的上报方案参数
* @return：     void
*********************************************************************
*/
void time_data_to_buf(OOP_TIME_T tVal, uint32 *offset, uint8 *apduValue)
{
    apduValue[(*offset)++] = tVal.hour; 
    apduValue[(*offset)++] = tVal.minute; 
    apduValue[(*offset)++] = tVal.second;             

    return;
}

/**
*********************************************************************
* @brief：      可变数据的读取
* @param[in]： report      原始上报方案参数
* @param[in]： offset      编码时的偏移
* @param[out]：pReportCfg 编码后的上报方案参数
* @return：     void
*********************************************************************
*/
void rpt_plan_ti_read(OOP_TI_T ti, uint32 *offset, uint8 *apduValue)
{
    /*TI∷=SEQUENCE
     {
         单位 ENUMERATED
         { 
             秒 （0），
             分 （1），
             时 （2），
             日 （3），
             月 （4），
             年 （5） 
         }，
         间隔值 long-unsigned
     }*/

    apduValue[(*offset)++] = ti.unit;

    memcpy_r(&apduValue[*offset], (uint8*)&ti.value, sizeof(uint16));
    (*offset) += sizeof(uint16);

    return;
}


/**
*********************************************************************
* @brief：      可变数据的读取
* @param[in]： report      原始上报方案参数
* @param[in]： offset      编码时的偏移
* @param[out]：pReportCfg 编码后的上报方案参数
* @return：     void
*********************************************************************
*/
int32 vardata_data_to_buf(OOP_VARIANT_T variant, uint8 *apduValue, uint32 dstLen, uint32 *offset)
{
    apduValue[(*offset)++] = variant.type; //数值选择

    switch(variant.type)
    {
        case DT_INTEGER:
        case DT_ENUM:
        {
            apduValue[(*offset)++] = (uint8)variant.cVal;
        }break;
        case DT_BOOL: 
        {
            apduValue[(*offset)++] = (uint8)variant.bVal;
        }break;
        case DT_UNSIGNED: 
        {
            apduValue[(*offset)++] = variant.byVal;
        }break;     
        case DT_DATETIME: 
        {
            datetime_data_to_buf(variant.dtVal, offset, apduValue);
        }break;  
        case DT_DATE: 
        {
            date_data_to_buf(variant.dVal, offset, apduValue);         
        }break;   
        case DT_TIME: 
        {          
            time_data_to_buf(variant.tVal, offset, apduValue);         
        }break;   
        case DT_DATETIME_S: 
        {
            datetime_s_data_to_buf(variant.tmVal, offset, apduValue); 
        }break;
        case DT_DOUBLE_LONG: 
        {
            memcpy_r(&apduValue[*offset], &variant.nVal, sizeof(int));
            (*offset) += sizeof(int);
        }break;  
        case DT_LONG_UNSIGNED: 
        {
            memcpy_r(&apduValue[*offset], &variant.wVal, sizeof(uint16));
            (*offset) += sizeof(uint16);
        }break; 
        case DT_LONG: 
        {
            memcpy_r(&apduValue[*offset], &variant.sVal, sizeof(int16));
            (*offset) += sizeof(int16);
        }break;   
        case DT_DOUBLE_LONG_UNSIGNED: 
        {
            memcpy_r(&apduValue[*offset], &variant.dwVal, sizeof(uint32));
            (*offset) += sizeof(uint32);
        }break; 
        case DT_LONG64: 
        {
            memcpy_r(&apduValue[*offset], &variant.llVal, sizeof(int64));
            (*offset) += sizeof(int64);
        }break; 
        case DT_LONG64_UNSIGNED: 
        {
            memcpy_r(&apduValue[*offset], &variant.ullVal, sizeof(uint64));
            (*offset) += sizeof(uint64);
        }break; 
        case DT_FLOAT32: 
        {
            memcpy_r(&apduValue[*offset], &variant.fVal, sizeof(float32));
            (*offset) += sizeof(float32);
        }break;  
        case DT_FLOAT64: 
        {
            memcpy_r(&apduValue[*offset], &variant.dbVal, sizeof(float64));
            (*offset) += sizeof(float64);
        }break;
        case DT_OI: 
        {
            memcpy_r(&apduValue[*offset], &variant.oi, sizeof(OI));
            (*offset) += sizeof(OI);
        }break;
        case DT_TI: 
        {
            ti_data_to_buf(variant.ti, offset, apduValue);
        }break;  
        case DT_TSA: 
        {
            tsa_data_to_buf(variant.tsa, offset, apduValue);
        }break; 
        case DT_OAD: 
        {
            memcpy_r(&apduValue[*offset], &variant.oad, sizeof(OOP_OAD_U));
            (*offset) += sizeof(OOP_OAD_U);
        }break;   
        case DT_OMD: 
        {
            memcpy_r(&apduValue[*offset], &variant.omd, sizeof(OOP_OMD_U));
            (*offset) += sizeof(OOP_OMD_U);
        }break;   
        case DT_CSD: 
        {
            csd_data_to_buf(variant.csd, offset, apduValue);
        }break; 
        case DT_COMDCB: 
        {
            memcpy(&apduValue[*offset], &variant.dcb, sizeof(OOP_COMDCB_T));
            (*offset) += sizeof(OOP_COMDCB_T);
        }break;
        default: return ERR_NORMAL; break;
    }

    return ERR_OK;
}

/**
*********************************************************************
* @brief：      可变数据的读取
* @param[in]： report      原始上报方案参数
* @param[in]： offset      编码时的偏移
* @param[out]：pReportCfg 编码后的上报方案参数
* @return：     void
*********************************************************************
*/
void vardata_buf_to_data(OOP_VARIANT_T *variant, uint8 *apduValue, uint32 srcLen, uint32 *offset)
{

}

#endif


#if DESC("APDU D2B 非DATA内部的RSD&RCSD",1)
/**
*********************************************************************
* @name：        apdu_ti_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_ti_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_TI_T data; 
    
    MEMZERO(&data, sizeof(data));

    data.unit = srcBufTmp[offset++];  
    
    memcpy_r(&data.value, &srcBufTmp[offset], sizeof(uint16));
    offset += sizeof(uint16);

    memcpy(dstData, &data, sizeof(data));

    *pOffset = offset;

    return dar;     

}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_datetime_s_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_DATETIME_S_T data; 
    
    MEMZERO(&data, sizeof(data));
    
    memcpy_r(&data.year, &srcBufTmp[offset], sizeof(uint16));
    offset += sizeof(uint16);

    data.month  = srcBufTmp[offset++];  
    data.day    = srcBufTmp[offset++];           
    data.hour   = srcBufTmp[offset++]; 
    data.minute = srcBufTmp[offset++]; 
    data.second = srcBufTmp[offset++]; 

    memcpy(dstData, &data, sizeof(data));

    *pOffset = offset;

    return dar;     

}

/**
*********************************************************************
* @name：        basic_datetime_s_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 apdu_tsa_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_TSA_T data;
    uint8 checkLen = 0;

    MEMZERO(&data, sizeof(data));

    //TSA长度
    checkLen = srcBufTmp[offset++];

    data.af = srcBufTmp[offset++];

    if(data.len+2 != checkLen)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check tsa size failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    memcpy(data.add, &srcBufTmp[offset], data.len+1); 
    offset += data.len+1;

    memcpy(dstData, &data, sizeof(data));
    
    *pOffset = offset;
    
    return nRet;
}

/**
*********************************************************************
* @name：        basic_datetime_s_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_tsa_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_TSA_T data;
    uint8 checkLen = 0;

    MEMZERO(&data, sizeof(data));
    
    //TSA类型
    if(srcBufTmp[offset++] != DT_TSA)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check tsa type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    //TSA长度
    checkLen = srcBufTmp[offset++];

    data.af = srcBufTmp[offset++];

    if(data.len+2 != checkLen)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check tsa size failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    if (data.flag == 0x01)
    {
        data.vxd = srcBufTmp[offset ++];
        memcpy(data.add, &srcBufTmp[offset], data.len); 
        offset += data.len;
    }
    else
    {
        memcpy(data.add, &srcBufTmp[offset], data.len+1); 
        offset += data.len + 1;
    }

    memcpy(dstData, &data, sizeof(data));
    
    *pOffset = offset;
    
    return nRet;
}

/**
*********************************************************************
* @name：        basic_ms_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_ms_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_MSP_T ms;
    uint32 i = 0;    
    uint8 offlen = 0;      //数组数量长度
    uint32 num = 0;

    /*MS∷=CHOICE
     { 
         无表计              [0]，
         全部用户地址           [1]，
         一组用户类型           [2] SEQUENCE OF unsigned，
         一组用户地址           [3] SEQUENCE OF TSA，
         一组配置序号           [4] SEQUENCE OF long-unsigned，
         一组用户类型区间 [5] SEQUENCE OF Region，
         一组用户地址区间 [6] SEQUENCE OF Region，
         一组配置序号区间 [7] SEQUENCE OF Region
     }*/
     
    MEMZERO(&ms, sizeof(ms));

    ms.choice = srcBufTmp[offset++];

    switch(ms.choice)
    {
        case 0: break;//无表计 
        case 1: break;//所有电能表
        
        case 2:       //一组用户类型
        {
            ms.size = srcBufTmp[offset++];
            ms.mst.size = ms.size;
            for(i = 0; i < ms.msi.size; i++)
            {
                memcpy(&ms.mst.type[i], &srcBufTmp[offset], sizeof(uint8));    
                offset += sizeof(uint8);
            }
        }break;

        case 3:       //一组用户地址
        {
            ms.size = srcBufTmp[offset++];
            ms.msa.size = ms.size;
            for(i = 0; i < ms.msa.size; i++)
            {
                dar = apdu_tsa_fromfmt698(srcBuf, srcLen, &offset, &ms.msa.add[i], sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_tsa_fromfmt698 failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }                     
            }
        }break;   

        case 4:       //一组配置序号
        {
            //数组成员个数
            num = get_len_offset((void*)&srcBufTmp[offset], &offlen);
            offset += offlen;
            ms.size = num;
            ms.msi.size = ms.size;
            for(i = 0; i < ms.msi.size; i++)
            {
               memcpy_r(&ms.msi.idx[i], &srcBufTmp[offset], sizeof(uint16));
               offset += sizeof(uint16);               
            }

        }break;    

        case 5:       //一组用户类型区间
        {
            ms.size = srcBufTmp[offset++];
            ms.msrt.size = ms.size;
            for(i = 0; i < ms.msrt.size; i++)
            {
                ms.msrt.region[i].nType = srcBufTmp[offset++]; //区间类型

                dar = basic_buf_to_data(E_UNSIGNED,srcBuf, srcLen, &offset, &ms.msrt.region[i].start, sizeof(uint8));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }  

                dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &offset, &ms.msrt.region[i].end, sizeof(uint8));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }                
            }
        }break; 

        case 6:       //一组用户地址区间
        {
            ms.size = srcBufTmp[offset++];
            ms.msra.size = ms.size;
            for(i = 0; i < ms.msra.size; i++)
            {
                ms.msra.region[i].nType = srcBufTmp[offset++]; //区间类型
                
                dar = basic_tsa_fromfmt698(srcBuf, srcLen, &offset, &ms.msra.region[i].start, sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_tsa_fromfmt698 failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }  
                
                dar = basic_tsa_fromfmt698(srcBuf, srcLen, &offset, &ms.msra.region[i].end, sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_tsa_fromfmt698 failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }
            }
        }break;   

        case 7:       //一组配置序号区间
        {
            ms.size = srcBufTmp[offset++];
            ms.msri.size = ms.size;
            for(i = 0; i < ms.msra.size; i++)
            {
                ms.msri.region[i].nType = srcBufTmp[offset++]; //区间类型
                
                dar = basic_buf_to_data(E_LONG_UNSIGNED,srcBuf, srcLen, &offset, &ms.msri.region[i].start, sizeof(uint16));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }  
                
                dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &offset, &ms.msri.region[i].end, sizeof(uint16));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }
            }
        }break;          
#if AREA_ZHEJIANG
        case 8:       //一组配置序号
        {
            ms.size = srcBufTmp[offset++];
            ms.msdt.size = ms.size;
            for(i = 0; i < ms.msdt.size; i++)
            {
               memcpy_r(&ms.msdt.type[i], &srcBufTmp[offset], sizeof(uint16));
               offset += sizeof(uint16);               
            }

        }break;    
#endif
        
    }

    memcpy(dstData, &ms, sizeof(ms));

    *pOffset = offset;

    return dar;     

}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/

OOP_DAR_E apdu_variant_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_VARIANT_T variant;  
    DATA_E eData = 0;
    BOOLEAN bValue = 0;
    uint32 typelen = (uint32)((void *)&variant.tmVal - (void *)&variant.type);
    
    MEMZERO(&variant, sizeof(variant));

    variant.type = srcBufTmp[offset++];
    PRTL_FMT_TRACE("type(%d)\n", variant.type);    

    bValue = data_type_oop_to_edata(variant.type, &eData);
    if(bValue)
    {
        offset--; //由于下面函数还要解析一遍类型,union中任何一个变量地址可以代表union的地址
        dar = basic_buf_to_data(eData, srcBuf, srcLen, &offset, &variant.tmVal, sizeof(variant)-typelen);
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. offset(%d)", offset);
            return DATA_TYPE_UNMATCHED;           
        } 
    }
    else
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "undefined variant type. offset(%d)", offset);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }
    
    switch(variant.type)
    {
        case DT_DATETIME_S: 
        {
            PRTL_FMT_TRACE("%04d-%02d-%02d %02d:%02d:%02d\n", 
            variant.tmVal.year, variant.tmVal.month, variant.tmVal.day, 
            variant.tmVal.hour, variant.tmVal.minute, variant.tmVal.second);            
        }break;
        case DT_TI: 
        {
            PRTL_FMT_TRACE("ti.unit(%d), ti.value(%d)\n", variant.ti.unit, variant.ti.value);            
        }break;        
    }
    
    memcpy(dstData, &variant, sizeof(variant));

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel1_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTOR1_T selector;
    uint16 datanum=0;

    /*Selector1∷=SEQUENCE
    {
        对象属性描述符 OAD，
        数值             Data
    }*/
     
    MEMZERO(&selector, sizeof(selector));

    //OAD
    memcpy_r(&selector.oad, &srcBufTmp[offset], sizeof(uint32));
    offset += sizeof(uint32);
    PRTL_FMT_TRACE("oda(0x%08x)\n", selector.oad.value);

    //Data
    if(srcBufTmp[offset]==DT_VISIBLE_STRING)
    {
        offset++;
        datanum = srcBufTmp[offset];
        if(datanum !=0)
        {
            selector.value.type = DT_VISIBLE_STRING;
            selector.value.octVal.nNum = datanum;
            offset =offset+1;
            memcpy(selector.value.octVal.value,(uint8*)&srcBufTmp[offset],selector.value.octVal.nNum);
            offset = offset + selector.value.octVal.nNum;
        }
    }
    else
    {
        dar = apdu_variant_fromfmt698(srcBuf, srcLen, &offset, &selector.value, sizeof(OOP_VARIANT_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_variant_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
            return DATA_TYPE_UNMATCHED;           
        }
    }

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel2_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTOR2_T selector;

    /*Selector2∷=SEQUENCE
    {
        对象属性描述符 OAD，
        起始值            Data，
        结束值            Data，
        数据间隔           Data
    }*/

    MEMZERO(&selector, sizeof(selector));

    //OAD
    memcpy_r(&selector.oad, &srcBufTmp[offset], sizeof(uint32));
    offset += sizeof(uint32);
    PRTL_FMT_TRACE("oda(0x%08x)\n", selector.oad.value);

    //Data
    dar = apdu_variant_fromfmt698(srcBuf, srcLen, &offset, &selector.from, sizeof(OOP_VARIANT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_variant_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }

    //Data
    dar = apdu_variant_fromfmt698(srcBuf, srcLen, &offset, &selector.to, sizeof(OOP_VARIANT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_variant_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }

    //Data
    dar = apdu_variant_fromfmt698(srcBuf, srcLen, &offset, &selector.span, sizeof(OOP_VARIANT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_variant_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}


/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel3_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTOR3_T selector;
    uint32 i = 0;

    /*Selector3∷=SEQUENCE OF Selector2*/

    MEMZERO(&selector, sizeof(selector));

    selector.nNum = srcBufTmp[offset++];
    PRTL_FMT_TRACE("nNum(0x%08x)\n", selector.nNum);

    if (selector.nNum > MAX_SELECTOR2_NUM)
    {
        PRTL_FMT_DEBUG("basic_ccsd_fromfmt698 failed. selector.nNum(%d) > MAX_SELECTOR2_NUM(%d)\n", selector.nNum, MAX_SELECTOR2_NUM);
        return DATA_TYPE_UNMATCHED;
    }

    for(i = 0; i< selector.nNum; i++)
    {
        dar = apdu_rsd_sel2_fromfmt698(srcBuf, srcLen, &offset, &selector.item[i], sizeof(OOP_SELECTOR2_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_ccsd_fromfmt698 failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;           
        }        
    }

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel4_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTORP4_T selector;

    /*Selector4∷=SEQUENCE
     {
        采集启动时间 date_time_s，
        表计集合         MS
     }*/

    MEMZERO(&selector, sizeof(selector));

    dar = apdu_datetime_s_fromfmt698(srcBuf, srcLen, &offset, &selector.time, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_datetime_s_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    dar = apdu_ms_fromfmt698(srcBuf, srcLen, &offset, &selector.ms, sizeof(OOP_MSP_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_datetime_s_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}


/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel5_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTORP5_T selector;

    /*Selector5∷=SEQUENCE
     {
        采集存储时间 date_time_s，
        表计集合         MS
     }*/

    MEMZERO(&selector, sizeof(selector));

    dar = apdu_datetime_s_fromfmt698(srcBuf, srcLen, &offset, &selector.time, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_datetime_s_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    dar = apdu_ms_fromfmt698(srcBuf, srcLen, &offset, &selector.ms, sizeof(OOP_MSP_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_datetime_s_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel6_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTORP6_T selector;

    /*Selector6∷=SEQUENCE
     {
        采集启动时间起始值 date_time_s，
        采集启动时间结束值 date_time_s， 
        时间间隔               TI，
        表计集合               MS
     }*/

    MEMZERO(&selector, sizeof(selector));

    dar = apdu_datetime_s_fromfmt698(srcBuf, srcLen, &offset, &selector.start, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    dar = apdu_datetime_s_fromfmt698(srcBuf, srcLen, &offset, &selector.end, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_ti_fromfmt698(srcBuf, srcLen, &offset, &selector.ti, sizeof(OOP_TI_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_ti_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_ms_fromfmt698(srcBuf, srcLen, &offset, &selector.ms, sizeof(OOP_MSP_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}


/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel7_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTORP7_T selector;

    /*Selector7∷=SEQUENCE
     {
        采集存储时间起始值 date_time_s，
        采集存储时间结束值 date_time_s， 
        时间间隔               TI，
        表计集合               MS
     }*/

    MEMZERO(&selector, sizeof(selector));

    dar = apdu_datetime_s_fromfmt698(srcBuf, srcLen, &offset, &selector.start, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    dar = apdu_datetime_s_fromfmt698(srcBuf, srcLen, &offset, &selector.end, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_ti_fromfmt698(srcBuf, srcLen, &offset, &selector.ti, sizeof(OOP_TI_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_ti_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_ms_fromfmt698(srcBuf, srcLen, &offset, &selector.ms, sizeof(OOP_MSP_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}


/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel8_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTORP8_T selector;

    /*Selector8∷=SEQUENCE
     {
        采集成功时间起始值 date_time_s，
        采集成功时间结束值 date_time_s， 
        时间间隔               TI，
        表计集合               MS
     }*/

    MEMZERO(&selector, sizeof(selector));

    dar = apdu_datetime_s_fromfmt698(srcBuf, srcLen, &offset, &selector.start, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    dar = apdu_datetime_s_fromfmt698(srcBuf, srcLen, &offset, &selector.end, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_ti_fromfmt698(srcBuf, srcLen, &offset, &selector.ti, sizeof(OOP_TI_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_ti_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_ms_fromfmt698(srcBuf, srcLen, &offset, &selector.ms, sizeof(OOP_MSP_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}


/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel9_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTOR9_T selector;

    /*Selector9∷=SEQUENCE
     {
        上第 n 次记录 unsigned  
     }*/
     
    MEMZERO(&selector, sizeof(selector));

    selector.nLast = srcBufTmp[offset++];
    PRTL_FMT_TRACE("nLast(%d)\n", selector.nLast);

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel10_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel10_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTORP10_T selector;

    /*Selector10∷=SEQUENCE
     {
        上第 n 次记录 unsigned,
        表计集合               MS   
     }*/
     
    MEMZERO(&selector, sizeof(selector));

    selector.nTimes = srcBufTmp[offset++];

    dar = apdu_ms_fromfmt698(srcBuf, srcLen, &offset, &selector.ms, sizeof(OOP_MSP_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}


/**
*********************************************************************
* @name：        nontype_rsd_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_PRSD_T rsd; //

    /*RSD∷=CHOICE
    {
        不选择 [0] NULL，
        选择方法 1 [1] Selector1，*
        ......
    }*/

    MEMZERO(&rsd, sizeof(rsd));
    
    rsd.choice = srcBufTmp[offset++];
    PRTL_FMT_TRACE("rsd.choice Selector%d.\n", rsd.choice);

    switch(rsd.choice)
    {
        case 0: //NULL
        {
            PRTL_BUF_TRACE(srcBufTmp, srcLen, "NULL Selector. offset(%d)", offset);
        }break;

        case 1: //Selector1
        {
            dar = apdu_rsd_sel1_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel1, sizeof(OOP_SELECTOR1_T));        
        }break;
        
        case 2: //Selector2
        {
            dar = apdu_rsd_sel2_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel2, sizeof(OOP_SELECTOR2_T));        
        }break;
        
        case 3: //Selector3
        {
            dar = apdu_rsd_sel3_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel3, sizeof(OOP_SELECTOR3_T));        
        }break;
        case 4: //Selector4
        {
            dar = apdu_rsd_sel4_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel4, sizeof(OOP_SELECTORP4_T));     
        }break;
        
        case 5: //Selector5
        {
            dar = apdu_rsd_sel5_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel5, sizeof(OOP_SELECTORP5_T));     
        }break;
        
        case 6: //Selector6
        {
            dar = apdu_rsd_sel6_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel6, sizeof(OOP_SELECTORP6_T));
        }break;
        
        case 7: //Selector7
        {
            dar = apdu_rsd_sel7_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel7, sizeof(OOP_SELECTORP7_T));
        }break;
        
        case 8: //Selector8
        {
            dar = apdu_rsd_sel8_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel8, sizeof(OOP_SELECTORP8_T));
        }break;
      
        case 9: //Selector9
        {
            dar = apdu_rsd_sel9_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel9, sizeof(OOP_SELECTOR9_T));
        }break;
            
        case 10: //Selector10
        {
            dar = apdu_rsd_sel10_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel10, sizeof(OOP_SELECTORP10_T));
        }break;

        default:
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "undefined Selector. offset(%d)", offset);
            return DATA_SCOPE_OF_ACCESS_VIOLATED;
        }break;            
    }

    memcpy(dstData, &rsd, sizeof(rsd));

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        basic_csd_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_csd_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_CSD_T csd; //APDU使用的CCSD，参数使用的CSD
    uint32 i = 0;
     
    MEMZERO(&csd, sizeof(csd));

    /*CSD∷=CHOICE
    {
        对象属性描述符              [0] OAD，
        记录型对象属性描述符 [1] ROAD
    }
    ROAD∷=SEQUENCE
    {
        对象属性描述符            OAD，
        关联对象属性描述符 SEQUENCE OF OAD
    }*/
    
    csd.choice = srcBufTmp[offset++];

    if(csd.choice == 0) //单OAD类型
    {
        memcpy_r(&csd.oad, &srcBufTmp[offset], sizeof(uint32)); //OAD
        offset += sizeof(uint32);
    }
    else if(csd.choice == 1)//记录型ROAD
    {
        memcpy_r(&csd.road.oadMain, &srcBufTmp[offset], sizeof(uint32)); //OAD
        offset += sizeof(uint32);   

        csd.road.oadCols.nNum = srcBufTmp[offset++]; //SEQUENCE个数

        for(i = 0; i < csd.road.oadCols.nNum; i++)
        {
            memcpy_r(&csd.road.oadCols.oad[i], &srcBufTmp[offset], sizeof(uint32)); //OAD
            offset += sizeof(uint32);                       
        }
    }
    else
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "csd choice failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(dstData, &csd, sizeof(csd));

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        basic_ccsd_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_ccsd_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_CCSD_T csd; //APDU使用的CCSD，参数使用的CSD
    uint32 i = 0;
     
    MEMZERO(&csd, sizeof(csd));

    /*CSD∷=CHOICE
    {
        对象属性描述符              [0] OAD，
        记录型对象属性描述符 [1] ROAD
    }
    ROAD∷=SEQUENCE
    {
        对象属性描述符            OAD，
        关联对象属性描述符 SEQUENCE OF OAD
    }*/
    
    csd.choice = srcBufTmp[offset++];

    if(csd.choice == 0) //单OAD类型
    {
        memcpy_r(&csd.oad, &srcBufTmp[offset], sizeof(uint32)); //OAD
        offset += sizeof(uint32);
    }
    else if(csd.choice == 1)//记录型ROAD
    {
        memcpy_r(&csd.road.oadMain, &srcBufTmp[offset], sizeof(uint32)); //OAD
        offset += sizeof(uint32);   

        csd.road.oadCols.nNum = srcBufTmp[offset++]; //SEQUENCE个数

        for(i = 0; i < csd.road.oadCols.nNum; i++)
        {
            memcpy_r(&csd.road.oadCols.oad[i], &srcBufTmp[offset], sizeof(uint32)); //OAD
            offset += sizeof(uint32);                       
        }
    }
    else
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "ccsd choice failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(dstData, &csd, sizeof(csd));

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        basic_rcsd_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rcsd_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_RCSD_T rcsd;
    uint32 i = 0;
     
    MEMZERO(&rcsd, sizeof(rcsd));

    /*RCSD∷=SEQUENCE OF CSD*/
    
    rcsd.nNum = srcBufTmp[offset++];
    PRTL_FMT_TRACE("rcsd.nNum(%d)\n", rcsd.nNum);

    for(i = 0; i < rcsd.nNum; i++)
    {
        dar = apdu_ccsd_fromfmt698(srcBuf, srcLen, &offset, &rcsd.cols[i], sizeof(OOP_CCSD_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_ccsd_fromfmt698 failed. offset(%d)", offset);
            return DATA_TYPE_UNMATCHED;           
        }
        
        PRTL_FMT_TRACE("[%d] choice(%d), oad(0x%08x)\n", i, rcsd.cols[i].choice, rcsd.cols[i].oad.value);        
    }
    
    memcpy(dstData, &rcsd, sizeof(rcsd));

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        apdu_octetstrvar_fromfmt698
* @brief：       可变octet-string数据转换为698报文格式,使用最大容量结构转
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_octetstrvar_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_OCTETVAR2K_T data = {0};
    uint8 offlen = 0;      //数组数量长度

    MEMZERO(&data, sizeof(data));

    //COctetString长度
    data.nNum = get_len_offset(&srcBufTmp[offset], &offlen);
    offset += offlen;

    memcpy(&data.value[0], &srcBufTmp[offset], data.nNum); 
    offset += data.nNum;

    memcpy(dstData, &data, dstLen);

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        apdu_visiblestrvar_fromfmt698
* @brief：       可变visible-string数据转换为698报文格式,使用最大容量结构转
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_visiblestrvar_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_VISIBLEVAR1K_T data = {0};
    uint8 offlen = 0;      //数组数量长度

    MEMZERO(&data, sizeof(data));

    //COctetString长度
    data.nNum = get_len_offset(&srcBufTmp[offset], &offlen);
    offset += offlen;

    memcpy(&data.value[0], &srcBufTmp[offset], data.nNum); 
    offset += data.nNum;

    memcpy(dstData, &data, dstLen);

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        basic_csd_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_region_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_REGIOND_T data;

    MEMZERO(&data, sizeof(data));
   
    data.nType = srcBufTmp[offset++];
    
    dar = digit_from_fmt698(srcBuf, srcLen, &offset, &data.start, sizeof(OOP_DIGIT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("digit_from_fmt698 failed. dar(%d), parm.choice(%d)\n", dar, data.start.choice);
        return DATA_TYPE_UNMATCHED;
    }

    dar = digit_from_fmt698(srcBuf, srcLen, &offset, &data.end, sizeof(OOP_DIGIT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("digit_from_fmt698 failed. dar(%d), parm.choice(%d)\n", dar, data.end.choice);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(dstData, &data, sizeof(data));
    
    *pOffset = offset;
    
    return dar;
}

#endif

#if DESC("APDU B2D 非DATA内部的RSD&RCSD",1)

/**
*********************************************************************
* @name：        basic_ms_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_ti_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_TI_T ti;

    memcpy(&ti, srcDataTmp, sizeof(ti));

    dstBufTmp[offset++] = ti.unit;

    memcpy_r(&dstBufTmp[offset], (uint8*)&ti.value, sizeof(uint16));
    offset += sizeof(uint16);

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        basic_ms_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_datetime_s_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_DATETIME_S_T tmVal;

    memcpy(&tmVal, srcDataTmp, sizeof(tmVal));

    dstBufTmp[offset++] = (tmVal.year>>8)&0xff;
    dstBufTmp[offset++] = tmVal.year&0xff;
    dstBufTmp[offset++] = tmVal.month;  
    dstBufTmp[offset++] = tmVal.day;           
    dstBufTmp[offset++] = tmVal.hour; 
    dstBufTmp[offset++] = tmVal.minute; 
    dstBufTmp[offset++] = tmVal.second; 

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        basic_datetime_s_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 apdu_tsa_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_TSA_T tsa;

    memcpy(&tsa, srcDataTmp, sizeof(tsa));

    //octet-string(SIZE(2...17))
    dstBufTmp[offset++] = tsa.len + 2; 
    
    dstBufTmp[offset++] = tsa.af;  

    memcpy(&dstBufTmp[offset], tsa.add, tsa.len+1); 
    offset += tsa.len+1;

    *pOffset = offset;
    
    return dar;
}

/**
*********************************************************************
* @name：        basic_datetime_s_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_tsa_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_TSA_T data;

    if(srcLen < sizeof(data))
    {
        PRTL_FMT_DEBUG("srclen check failed. expect: srclen(%d) >= size(%d)\n", srcLen, sizeof(data));
        return ERR_OPTION;
    }
    
    memcpy(&data, &srcDataTmp[0], sizeof(data));

    //数据类型
    dstBufTmp[offset++] = DT_TSA;

    //octet-string(SIZE(2...17))
    dstBufTmp[offset++] = data.len + 2; 
    
    dstBufTmp[offset++] = data.af; 

    if (data.flag == 0x01)
    {
        dstBufTmp[offset ++] = data.vxd;
        memcpy(&dstBufTmp[offset], data.add, data.len); 
        offset += data.len;
    }
    else
    {
        memcpy(&dstBufTmp[offset], data.add, data.len + 1); 
        offset += data.len + 1;
    }
    
    *pOffset = offset;

    return nRet;
}

/**
*********************************************************************
* @name：        basic_ms_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_msp_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_MSP_T ms;
    uint32 i = 0;    
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0};

    /*MS∷=CHOICE
     { 
         无表计              [0]，
         全部用户地址           [1]，
         一组用户类型           [2] SEQUENCE OF unsigned，
         一组用户地址           [3] SEQUENCE OF TSA，
         一组配置序号           [4] SEQUENCE OF long-unsigned，
         一组用户类型区间 [5] SEQUENCE OF Region，
         一组用户地址区间 [6] SEQUENCE OF Region，
         一组配置序号区间 [7] SEQUENCE OF Region
     }*/
     
    memcpy(&ms, srcDataTmp, sizeof(ms));

    dstBufTmp[offset++] = ms.choice;

    switch(ms.choice)
    {
        case 0: break; //无表计
        case 1: break; //全部可采集的表计  
        case 2:        //一组用户类型 
        {
            dstBufTmp[offset++] = (uint8)ms.mst.size; //SEQUENCE个数
            
            memcpy(&dstBufTmp[offset], ms.mst.type, ms.mst.size);
            offset += ms.mst.size;
        }break;
        case 3:        //一组用户地址 
        {
            dstBufTmp[offset++] = (uint8)ms.msa.size; //SEQUENCE个数
            
            for(i = 0; i < ms.msa.size; i++)
            {
                dar = apdu_tsa_tofmt698(dstBuf, dstLen, &offset, &ms.msa.add[i], sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("apdu_tsa_tofmt698 failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                } 
            }
        }break;
        case 4:        //一组配置序号
        {
            offlen = set_len_offset(ms.msi.size, lenBuf);
            memcpy(&dstBufTmp[offset], lenBuf, offlen);//SEQUENCE个数
            offset += offlen;

            for(i = 0; i < ms.msi.size; i++)
            {
                memcpy_r(&dstBufTmp[offset], (uint8*)&ms.msi.idx[i], sizeof(uint16));
                offset += sizeof(uint16);
            }
        }break;
        case 5:        //一组用户类型区间 
        {
            dstBufTmp[offset++] = (uint8)ms.msrt.size; //SEQUENCE个数
            
            for(i = 0; i < ms.msrt.size; i++)
            {
                dstBufTmp[offset++] = ms.msrt.region[i].nType; //区间类型
                dstBufTmp[offset++] = DT_UNSIGNED;             //Data choice
                dstBufTmp[offset++] = ms.msrt.region[i].start; //起始值
                dstBufTmp[offset++] = DT_UNSIGNED;             //Data choice
                dstBufTmp[offset++] = ms.msrt.region[i].end;   //结束值             
            }
        }break;   
        case 6:        //一组用户地址区间 
        {
            dstBufTmp[offset++] = (uint8)ms.msra.size; //SEQUENCE个数
            
            for(i = 0; i < ms.msra.size; i++)
            {
                dstBufTmp[offset++] = ms.msra.region[i].nType;   //区间类型
                dar = basic_tsa_tofmt698(dstBufTmp, dstLen, &offset, &ms.msra.region[i].start, sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_tsa_tofmt698 failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }

                dar = basic_tsa_tofmt698(dstBufTmp, dstLen, &offset, &ms.msra.region[i].end, sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_tsa_tofmt698 failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }
            }
        }break;  
        case 7:        //一组配置序号区间 
        {
            dstBufTmp[offset++] = (uint8)ms.msri.size; //SEQUENCE个数
            
            for(i = 0; i < ms.msri.size; i++)
            {
                dstBufTmp[offset++] = ms.msri.region[i].nType;   //区间类型

                dstBufTmp[offset++] = DT_LONG_UNSIGNED;               //Data choice

                memcpy_r(&dstBufTmp[offset], (uint8*)&ms.msri.region[i].start, sizeof(uint16));//起始值
                offset += sizeof(uint16); 

                dstBufTmp[offset++] = DT_LONG_UNSIGNED;               //Data choice

                memcpy_r(&dstBufTmp[offset], (uint8*)&ms.msri.region[i].end, sizeof(uint16));//结束值
                offset += sizeof(uint16); 
            }
        }break; 
#if AREA_ZHEJIANG
        case 8:        //一组配置序号
        {
            dstBufTmp[offset++] = (uint8)ms.msdt.size; //SEQUENCE个数

            for(i = 0; i < ms.msdt.size; i++)
            {
                memcpy_r(&dstBufTmp[offset], (uint8*)&ms.msdt.type[i], sizeof(uint16));
                offset += sizeof(uint16);
            }
        }break;
#endif
        default:break;
    }

    *pOffset = offset;

    return dar;     
}

/**
*********************************************************************
* @name：        basic_ms_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_msc_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_MSC_T ms;
    uint32 i = 0;    
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0};

    /*MS∷=CHOICE
     { 
         无表计              [0]，
         全部用户地址           [1]，
         一组用户类型           [2] SEQUENCE OF unsigned，
         一组用户地址           [3] SEQUENCE OF TSA，
         一组配置序号           [4] SEQUENCE OF long-unsigned，
         一组用户类型区间 [5] SEQUENCE OF Region，
         一组用户地址区间 [6] SEQUENCE OF Region，
         一组配置序号区间 [7] SEQUENCE OF Region
     }*/
     
    memcpy(&ms, srcDataTmp, sizeof(ms));

    dstBufTmp[offset++] = ms.choice;

    switch(ms.choice)
    {
        case 0: break; //无表计
        case 1: break; //全部可采集的表计  
        case 2:        //一组用户类型 
        {
            dstBufTmp[offset++] = (uint8)ms.mst.size; //SEQUENCE个数
            
            memcpy(&dstBufTmp[offset], ms.mst.type, ms.mst.size);
            offset += ms.mst.size;
        }break;
        case 3:        //一组用户地址 
        {
            //dstBufTmp[offset++] = (uint8)ms.msa.size; //SEQUENCE个数
            offlen = set_len_offset(ms.msa.size, lenBuf);
            memcpy(&dstBufTmp[offset], lenBuf, offlen);//SEQUENCE个数
            offset += offlen;
            for(i = 0; i < ms.msa.size; i++)
            {
                dar = apdu_tsa_tofmt698(dstBuf, dstLen, &offset, &ms.msa.add[i], sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("apdu_tsa_tofmt698 failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                } 
            }
        }break;
        case 4:        //一组配置序号
        {
            offlen = set_len_offset(ms.msi.size, lenBuf);
            memcpy(&dstBufTmp[offset], lenBuf, offlen);//SEQUENCE个数
            offset += offlen;

            for(i = 0; i < ms.msi.size; i++)
            {
                memcpy_r(&dstBufTmp[offset], (uint8*)&ms.msi.idx[i], sizeof(uint16));
                offset += sizeof(uint16);
            }
        }break;
        case 5:        //一组用户类型区间 
        {
            dstBufTmp[offset++] = (uint8)ms.msrt.size; //SEQUENCE个数
            
            for(i = 0; i < ms.msrt.size; i++)
            {
                dstBufTmp[offset++] = ms.msrt.region[i].nType; //区间类型
                dstBufTmp[offset++] = DT_UNSIGNED;             //Data choice
                dstBufTmp[offset++] = ms.msrt.region[i].start; //起始值
                dstBufTmp[offset++] = DT_UNSIGNED;             //Data choice
                dstBufTmp[offset++] = ms.msrt.region[i].end;   //结束值             
            }
        }break;   
        case 6:        //一组用户地址区间 
        {
            dstBufTmp[offset++] = (uint8)ms.msra.size; //SEQUENCE个数
            
            for(i = 0; i < ms.msra.size; i++)
            {
                dstBufTmp[offset++] = ms.msra.region[i].nType;   //区间类型
                dar = basic_tsa_tofmt698(dstBufTmp, dstLen, &offset, &ms.msra.region[i].start, sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_tsa_tofmt698 failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }

                dar = basic_tsa_tofmt698(dstBufTmp, dstLen, &offset, &ms.msra.region[i].end, sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_tsa_tofmt698 failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }
            }
        }break;  
        case 7:        //一组配置序号区间 
        {
            dstBufTmp[offset++] = (uint8)ms.msri.size; //SEQUENCE个数
            
            for(i = 0; i < ms.msri.size; i++)
            {
                dstBufTmp[offset++] = ms.msri.region[i].nType;   //区间类型

                dstBufTmp[offset++] = DT_LONG_UNSIGNED;               //Data choice

                memcpy_r(&dstBufTmp[offset], (uint8*)&ms.msri.region[i].start, sizeof(uint16));//起始值
                offset += sizeof(uint16); 

                dstBufTmp[offset++] = DT_LONG_UNSIGNED;               //Data choice

                memcpy_r(&dstBufTmp[offset], (uint8*)&ms.msri.region[i].end, sizeof(uint16));//结束值
                offset += sizeof(uint16); 
            }
        }break; 

        default:break;
    }

    *pOffset = offset;

    return dar;     
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/

OOP_DAR_E apdu_variant_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_VARIANT_T variant;  
    DATA_E eData = 0;
    BOOLEAN bValue = 0;
    uint32 typelen = (uint32)((void *)&variant.tmVal - (void *)&variant.type);
    
    memcpy(&variant, srcDataTmp, sizeof(variant));

    dstBufTmp[offset++] = variant.type;
    PRTL_FMT_TRACE("type(%d)\n", variant.type);    

    bValue = data_type_oop_to_edata(variant.type, &eData);
    if(bValue)
    {
        offset--; //由于下面函数还要解析一遍类型,union中任何一个变量地址可以代表union的地址
        dar = basic_data_to_buf(eData, dstBuf, dstLen, &offset, &variant.tmVal, sizeof(variant)-typelen);
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(dstBufTmp, dstLen, "basic_data_to_buf failed. offset(%d)", offset);
            return DATA_TYPE_UNMATCHED;           
        } 
    }
    else
    {
        PRTL_BUF_DEBUG(dstBufTmp, dstLen, "undefined variant type. offset(%d)", offset);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }
    
    switch(variant.type)
    {
        case DT_DATETIME_S: 
        {
            PRTL_FMT_TRACE("%04d-%02d-%02d %02d:%02d:%02d\n", 
            variant.tmVal.year, variant.tmVal.month, variant.tmVal.day, 
            variant.tmVal.hour, variant.tmVal.minute, variant.tmVal.second);            
        }break;
        case DT_TI: 
        {
            PRTL_FMT_TRACE("ti.unit(%d), ti.value(%d)\n", variant.ti.unit, variant.ti.value);            
        }break;        
    }
    
    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel1_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_SELECTOR1_T selector;

    /*Selector1∷=SEQUENCE
    {
        对象属性描述符 OAD，
        数值             Data
    }*/
     
    memcpy(&selector, srcDataTmp, sizeof(selector));

    //OAD
    memcpy_r(&dstBufTmp[offset], &selector.oad, sizeof(uint32));
    offset += sizeof(uint32);
    PRTL_FMT_TRACE("oda(0x%08x)\n", selector.oad.value);

    //Data
    dar = apdu_variant_tofmt698(dstBuf, dstLen, &offset, &selector.value, sizeof(OOP_VARIANT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(dstBufTmp, dstLen, "apdu_variant_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel2_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_SELECTOR2_T selector;

    /*Selector2∷=SEQUENCE
    {
        对象属性描述符 OAD，
        起始值            Data，
        结束值            Data，
        数据间隔           Data
    }*/

    memcpy(&selector, srcDataTmp, sizeof(selector));

    //OAD
    memcpy_r(&dstBufTmp[offset], &selector.oad, sizeof(uint32));
    offset += sizeof(uint32);
    PRTL_FMT_TRACE("oda(0x%08x)\n", selector.oad.value);

    //Data
    dar = apdu_variant_tofmt698(dstBuf, dstLen, &offset, &selector.from, sizeof(OOP_VARIANT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_variant_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }

    //Data
    dar = apdu_variant_tofmt698(dstBuf, dstLen, &offset, &selector.to, sizeof(OOP_VARIANT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_variant_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }

    //Data
    dar = apdu_variant_tofmt698(dstBuf, dstLen, &offset, &selector.span, sizeof(OOP_VARIANT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_variant_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }

    *pOffset = offset;

    return dar; 
}


/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel3_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_SELECTOR3_T selector;
    uint32 i = 0;

    /*Selector3∷=SEQUENCE OF Selector2*/

    memcpy(&selector, srcDataTmp, sizeof(selector));

    dstBufTmp[offset++] = selector.nNum;
    PRTL_FMT_TRACE("nNum(0x%08x)\n", selector.nNum);

    for(i = 0; i< selector.nNum; i++)
    {
        dar = apdu_rsd_sel2_tofmt698(dstBuf, dstLen, &offset, &selector.item[i], sizeof(OOP_SELECTOR2_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("apdu_rsd_sel2_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
            return DATA_TYPE_UNMATCHED;           
        }        
    }

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel4_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    OOP_SELECTOR4_T selector;

    /*Selector4∷=SEQUENCE
     {
        采集启动时间 date_time_s，
        表计集合         MS
     }*/

    memcpy(&selector, srcDataTmp, sizeof(selector));

    dar = apdu_datetime_s_tofmt698(dstBuf, dstLen, &offset, &selector.time, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_datetime_s_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    dar = apdu_msc_tofmt698(dstBuf, dstLen, &offset, &selector.ms, sizeof(OOP_MSC_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_msc_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_prsd_sel4_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    OOP_SELECTORP4_T selector;

    /*Selector4∷=SEQUENCE
     {
        采集启动时间 date_time_s，
        表计集合         MS
     }*/

    memcpy(&selector, srcDataTmp, sizeof(selector));

    dar = apdu_datetime_s_tofmt698(dstBuf, dstLen, &offset, &selector.time, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_datetime_s_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    dar = apdu_msp_tofmt698(dstBuf, dstLen, &offset, &selector.ms, sizeof(OOP_MSP_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_msp_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel5_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    OOP_SELECTOR5_T selector;

    /*Selector5∷=SEQUENCE
     {
        采集存储时间 date_time_s，
        表计集合         MS
     }*/

    memcpy(&selector, srcDataTmp, sizeof(selector));

    dar = apdu_datetime_s_tofmt698(dstBuf, dstLen, &offset, &selector.time, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_datetime_s_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_msc_tofmt698(dstBuf, dstLen, &offset, &selector.ms, sizeof(OOP_MSC_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_msc_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_prsd_sel5_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    OOP_SELECTORP5_T selector;

    /*Selector5∷=SEQUENCE
     {
        采集存储时间 date_time_s，
        表计集合         MS
     }*/

    memcpy(&selector, srcDataTmp, sizeof(selector));

    dar = apdu_datetime_s_tofmt698(dstBuf, dstLen, &offset, &selector.time, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_datetime_s_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_msp_tofmt698(dstBuf, dstLen, &offset, &selector.ms, sizeof(OOP_MSP_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_msp_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel6_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    OOP_SELECTOR6_T selector;

    /*Selector6∷=SEQUENCE
     {
        采集启动时间起始值 date_time_s，
        采集启动时间结束值 date_time_s， 
        时间间隔               TI，
        表计集合               MS
     }*/

    memcpy(&selector, srcDataTmp, sizeof(selector));

    dar = apdu_datetime_s_tofmt698(dstBuf, dstLen, &offset, &selector.start, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_datetime_s_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_datetime_s_tofmt698(dstBuf, dstLen, &offset, &selector.end, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_datetime_s_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_ti_tofmt698(dstBuf, dstLen, &offset, &selector.ti, sizeof(OOP_TI_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_ti_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_msc_tofmt698(dstBuf, dstLen, &offset, &selector.ms, sizeof(OOP_MSC_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_msc_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }
    
    *pOffset = offset;

    return dar; 
}


/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_prsd_sel6_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    OOP_SELECTORP6_T selector;

    /*Selector6∷=SEQUENCE
     {
        采集启动时间起始值 date_time_s，
        采集启动时间结束值 date_time_s， 
        时间间隔               TI，
        表计集合               MS
     }*/

    memcpy(&selector, srcDataTmp, sizeof(selector));

    dar = apdu_datetime_s_tofmt698(dstBuf, dstLen, &offset, &selector.start, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_datetime_s_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_datetime_s_tofmt698(dstBuf, dstLen, &offset, &selector.end, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_datetime_s_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_ti_tofmt698(dstBuf, dstLen, &offset, &selector.ti, sizeof(OOP_TI_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_ti_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_msp_tofmt698(dstBuf, dstLen, &offset, &selector.ms, sizeof(OOP_MSP_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_msp_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }
    
    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel7_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    OOP_SELECTOR7_T selector;

    /*Selector7∷=SEQUENCE
     {
        采集存储时间起始值 date_time_s，
        采集存储时间结束值 date_time_s， 
        时间间隔               TI，
        表计集合               MS
     }*/

    memcpy(&selector, srcDataTmp, sizeof(selector));

    dar = apdu_datetime_s_tofmt698(dstBuf, dstLen, &offset, &selector.start, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_datetime_s_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    dar = apdu_datetime_s_tofmt698(dstBuf, dstLen, &offset, &selector.end, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_datetime_s_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_ti_tofmt698(dstBuf, dstLen, &offset, &selector.ti, sizeof(OOP_TI_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_ti_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_msc_tofmt698(dstBuf, dstLen, &offset, &selector.ms, sizeof(OOP_MSC_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_msc_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_prsd_sel7_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    OOP_SELECTORP7_T selector;

    /*Selector7∷=SEQUENCE
     {
        采集存储时间起始值 date_time_s，
        采集存储时间结束值 date_time_s， 
        时间间隔               TI，
        表计集合               MS
     }*/

    memcpy(&selector, srcDataTmp, sizeof(selector));

    dar = apdu_datetime_s_tofmt698(dstBuf, dstLen, &offset, &selector.start, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_datetime_s_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    dar = apdu_datetime_s_tofmt698(dstBuf, dstLen, &offset, &selector.end, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_datetime_s_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_ti_tofmt698(dstBuf, dstLen, &offset, &selector.ti, sizeof(OOP_TI_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_ti_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_msp_tofmt698(dstBuf, dstLen, &offset, &selector.ms, sizeof(OOP_MSP_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_msp_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel8_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    OOP_SELECTOR8_T selector;

    /*Selector8∷=SEQUENCE
     {
        采集成功时间起始值 date_time_s，
        采集成功时间结束值 date_time_s， 
        时间间隔               TI，
        表计集合               MS
     }*/

    memcpy(&selector, srcDataTmp, sizeof(selector));

    dar = apdu_datetime_s_tofmt698(dstBuf, dstLen, &offset, &selector.start, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_datetime_s_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    dar = apdu_datetime_s_tofmt698(dstBuf, dstLen, &offset, &selector.end, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_datetime_s_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_ti_tofmt698(dstBuf, dstLen, &offset, &selector.ti, sizeof(OOP_TI_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_ti_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_msc_tofmt698(dstBuf, dstLen, &offset, &selector.ms, sizeof(OOP_MSC_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_msc_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_prsd_sel8_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    OOP_SELECTORP8_T selector;

    /*Selector8∷=SEQUENCE
     {
        采集成功时间起始值 date_time_s，
        采集成功时间结束值 date_time_s， 
        时间间隔               TI，
        表计集合               MS
     }*/

    memcpy(&selector, srcDataTmp, sizeof(selector));

    dar = apdu_datetime_s_tofmt698(dstBuf, dstLen, &offset, &selector.start, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_datetime_s_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    dar = apdu_datetime_s_tofmt698(dstBuf, dstLen, &offset, &selector.end, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_datetime_s_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_ti_tofmt698(dstBuf, dstLen, &offset, &selector.ti, sizeof(OOP_TI_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_ti_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_msp_tofmt698(dstBuf, dstLen, &offset, &selector.ms, sizeof(OOP_MSP_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_msp_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    *pOffset = offset;

    return dar; 
}


/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel9_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_SELECTOR9_T selector;

    /*Selector9∷=SEQUENCE
     {
        上第 n 次记录 unsigned  
     }*/
     
    memcpy(&selector, srcDataTmp, sizeof(selector));

    dstBufTmp[offset++] = selector.nLast;
    PRTL_FMT_TRACE("nLast(%d)\n", selector.nLast);

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel10_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel10_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_SELECTOR10_T selector;

    /*Selector10∷=SEQUENCE
     {
        上第 n 次记录 unsigned,
        表计集合               MS   
     }*/
     
    memcpy(&selector, srcDataTmp, sizeof(selector));

    dstBufTmp[offset++] = selector.nTimes;

    dar = apdu_msc_tofmt698(dstBuf, dstLen, &offset, &selector.ms, sizeof(OOP_MSC_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_msc_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel10_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_prsd_sel10_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_SELECTORP10_T selector;

    /*Selector10∷=SEQUENCE
     {
        上第 n 次记录 unsigned,
        表计集合               MS   
     }*/
     
    memcpy(&selector, srcDataTmp, sizeof(selector));

    dstBufTmp[offset++] = selector.nTimes;

    dar = apdu_msp_tofmt698(dstBuf, dstLen, &offset, &selector.ms, sizeof(OOP_MSP_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_msp_tofmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        nontype_rsd_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_prsd_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_PRSD_T rsd;
     
    memcpy(&rsd, &srcDataTmp[0], srcLen);

    /*RSD∷=CHOICE
    {
        不选择 [0] NULL，
        选择方法 1 [1] Selector1，*
        ......
    }*/
    
    dstBufTmp[offset++] = rsd.choice; //选择项
    PRTL_FMT_TRACE("rsd.choice Selector%d.\n", rsd.choice);

    switch(rsd.choice)
    {
        case 0: //NULL
        {
            PRTL_BUF_TRACE(dstBufTmp, dstLen, "NULL Selector. offset(%d)", offset);
        }break;

        case 1: //Selector1
        {
            dar = apdu_rsd_sel1_tofmt698(dstBuf, dstLen, &offset, &rsd.sel1, sizeof(OOP_SELECTOR1_T));        
        }break;
        
        case 2: //Selector2
        {
            dar = apdu_rsd_sel2_tofmt698(dstBuf, dstLen, &offset, &rsd.sel2, sizeof(OOP_SELECTOR2_T));        
        }break;
        
        case 3: //Selector3
        {
            dar = apdu_rsd_sel3_tofmt698(dstBuf, dstLen, &offset, &rsd.sel3, sizeof(OOP_SELECTOR3_T));        
        }break;
        case 4: //Selector4
        {
            dar = apdu_prsd_sel4_tofmt698(dstBuf, dstLen, &offset, &rsd.sel4, sizeof(OOP_SELECTORP4_T));     
        }break;
        
        case 5: //Selector5
        {
            dar = apdu_prsd_sel5_tofmt698(dstBuf, dstLen, &offset, &rsd.sel5, sizeof(OOP_SELECTORP5_T));     
        }break;
        
        case 6: //Selector6
        {
            dar = apdu_prsd_sel6_tofmt698(dstBuf, dstLen, &offset, &rsd.sel6, sizeof(OOP_SELECTORP6_T));
        }break;
        
        case 7: //Selector7
        {
            dar = apdu_prsd_sel7_tofmt698(dstBuf, dstLen, &offset, &rsd.sel7, sizeof(OOP_SELECTORP7_T));
        }break;
        
        case 8: //Selector8
        {
            dar = apdu_prsd_sel8_tofmt698(dstBuf, dstLen, &offset, &rsd.sel8, sizeof(OOP_SELECTORP8_T));
        }break;
      
        case 9: //Selector9
        {
            dar = apdu_rsd_sel9_tofmt698(dstBuf, dstLen, &offset, &rsd.sel9, sizeof(OOP_SELECTOR9_T));
        }break;
            
        case 10: //Selector10
        {
            dar = apdu_prsd_sel10_tofmt698(dstBuf, dstLen, &offset, &rsd.sel10, sizeof(OOP_SELECTORP10_T));
        }break;

        default:
        {
            PRTL_BUF_DEBUG(dstBufTmp, dstLen, "undefined Selector. offset(%d)", offset);
            return DATA_SCOPE_OF_ACCESS_VIOLATED;
        }break;            
    }

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        nontype_rsd_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_RSD_T rsd;
     
    memcpy(&rsd, &srcDataTmp[0], srcLen);

    /*RSD∷=CHOICE
    {
        不选择 [0] NULL，
        选择方法 1 [1] Selector1，*
        ......
    }*/
    
    dstBufTmp[offset++] = rsd.choice; //选择项
    PRTL_FMT_TRACE("rsd.choice Selector%d.\n", rsd.choice);

    switch(rsd.choice)
    {
        case 0: //NULL
        {
            PRTL_BUF_TRACE(dstBufTmp, dstLen, "NULL Selector. offset(%d)", offset);
        }break;

        case 1: //Selector1
        {
            dar = apdu_rsd_sel1_tofmt698(dstBuf, dstLen, &offset, &rsd.sel1, sizeof(OOP_SELECTOR1_T));        
        }break;
        
        case 2: //Selector2
        {
            dar = apdu_rsd_sel2_tofmt698(dstBuf, dstLen, &offset, &rsd.sel2, sizeof(OOP_SELECTOR2_T));        
        }break;
        
        case 3: //Selector3
        {
            dar = apdu_rsd_sel3_tofmt698(dstBuf, dstLen, &offset, &rsd.sel3, sizeof(OOP_SELECTOR3_T));        
        }break;
        case 4: //Selector4
        {
            dar = apdu_rsd_sel4_tofmt698(dstBuf, dstLen, &offset, &rsd.sel4, sizeof(OOP_SELECTOR4_T));     
        }break;
        
        case 5: //Selector5
        {
            dar = apdu_rsd_sel5_tofmt698(dstBuf, dstLen, &offset, &rsd.sel5, sizeof(OOP_SELECTOR5_T));     
        }break;
        
        case 6: //Selector6
        {
            dar = apdu_rsd_sel6_tofmt698(dstBuf, dstLen, &offset, &rsd.sel6, sizeof(OOP_SELECTOR6_T));
        }break;
        
        case 7: //Selector7
        {
            dar = apdu_rsd_sel7_tofmt698(dstBuf, dstLen, &offset, &rsd.sel7, sizeof(OOP_SELECTOR7_T));
        }break;
        
        case 8: //Selector8
        {
            dar = apdu_rsd_sel8_tofmt698(dstBuf, dstLen, &offset, &rsd.sel8, sizeof(OOP_SELECTOR8_T));
        }break;
      
        case 9: //Selector9
        {
            dar = apdu_rsd_sel9_tofmt698(dstBuf, dstLen, &offset, &rsd.sel9, sizeof(OOP_SELECTOR9_T));
        }break;
            
        case 10: //Selector10
        {
            dar = apdu_rsd_sel10_tofmt698(dstBuf, dstLen, &offset, &rsd.sel10, sizeof(OOP_SELECTOR10_T));
        }break;

        default:
        {
            PRTL_BUF_DEBUG(dstBufTmp, dstLen, "undefined Selector. offset(%d)", offset);
            return DATA_SCOPE_OF_ACCESS_VIOLATED;
        }break;            
    }

    *pOffset = offset;

    return dar;
}


/**
*********************************************************************
* @name：        apdu_csd_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_csd_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_CSD_T csd;
    uint32 i = 0;
     
    memcpy(&csd, &srcDataTmp[0], srcLen);

    /*CSD∷=CHOICE
    {
        对象属性描述符              [0] OAD，
        记录型对象属性描述符 [1] ROAD
    }
    ROAD∷=SEQUENCE
    {
        对象属性描述符            OAD，
        关联对象属性描述符 SEQUENCE OF OAD
    }*/
    
    dstBufTmp[offset++] = csd.choice; //选择项

    if(csd.choice == 0) //单OAD类型
    {
        memcpy_r(&dstBufTmp[offset], &csd.oad, sizeof(uint32)); //OAD
        offset += sizeof(uint32);
    }
    else if(csd.choice == 1)//记录型ROAD
    {
        memcpy_r(&dstBufTmp[offset], &csd.road.oadMain, sizeof(uint32)); //OAD
        offset += sizeof(uint32);   

        dstBufTmp[offset++] = csd.road.oadCols.nNum; //SEQUENCE个数

        for(i = 0; i < csd.road.oadCols.nNum; i++)
        {
            memcpy_r(&dstBufTmp[offset], &csd.road.oadCols.oad[i], sizeof(uint32)); //OAD
            offset += sizeof(uint32);                       
        }
    }
    else
    {
        return DATA_TYPE_UNMATCHED;
    }

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        apdu_csd_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_ccsd_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_CCSD_T csd; //APDU使用的CCSD，参数使用的CSD   
    uint32 i = 0;
     
    memcpy(&csd, &srcDataTmp[0], srcLen);

    /*CSD∷=CHOICE
    {
        对象属性描述符              [0] OAD，
        记录型对象属性描述符 [1] ROAD
    }
    ROAD∷=SEQUENCE
    {
        对象属性描述符            OAD，
        关联对象属性描述符 SEQUENCE OF OAD
    }*/
    
    dstBufTmp[offset++] = csd.choice; //选择项

    if(csd.choice == 0) //单OAD类型
    {
        memcpy_r(&dstBufTmp[offset], &csd.oad, sizeof(uint32)); //OAD
        offset += sizeof(uint32);
    }
    else if(csd.choice == 1)//记录型ROAD
    {
        memcpy_r(&dstBufTmp[offset], &csd.road.oadMain, sizeof(uint32)); //OAD
        offset += sizeof(uint32);   

        dstBufTmp[offset++] = csd.road.oadCols.nNum; //SEQUENCE个数

        for(i = 0; i < csd.road.oadCols.nNum; i++)
        {
            memcpy_r(&dstBufTmp[offset], &csd.road.oadCols.oad[i], sizeof(uint32)); //OAD
            offset += sizeof(uint32);                       
        }
    }
    else
    {
        return DATA_TYPE_UNMATCHED;
    }

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        basic_rcsd_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rcsd_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_RCSD_T rcsd;
    uint32 i = 0;
     
    memcpy(&rcsd, &srcDataTmp[0], srcLen);

    /*RCSD∷=SEQUENCE OF CSD*/   
    
    dstBufTmp[offset++] = rcsd.nNum; //SEQUENCE个数
   
    //遍历RCSD中每个CSD        
    for(i = 0; i < rcsd.nNum; i++)
    {
        dar = apdu_ccsd_tofmt698(dstBuf, dstLen, &offset, &rcsd.cols[i], sizeof(OOP_CCSD_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(dstBuf, dstLen, "apdu_ccsd_tofmt698 failed. offset(%d)", offset);
            return DATA_TYPE_UNMATCHED;           
        }        
    }

    *pOffset = offset;

    return dar;
}

OOP_DAR_E apdu_pcsd_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_PCSD_T rcsd;
    uint32 i = 0;
     
    memcpy(&rcsd, &srcDataTmp[0], srcLen);

    /*RCSD∷=SEQUENCE OF CSD*/   
    
    dstBufTmp[offset++] = rcsd.nNum; //SEQUENCE个数
   
    //遍历RCSD中每个CSD        
    for(i = 0; i < rcsd.nNum; i++)
    {
        dar = apdu_csd_tofmt698(dstBuf, dstLen, &offset, &rcsd.cols[i], sizeof(OOP_CSD_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(dstBuf, dstLen, "apdu_ccsd_tofmt698 failed. offset(%d)", offset);
            return DATA_TYPE_UNMATCHED;           
        }        
    }

    *pOffset = offset;

    return dar;
}

#endif

#if DESC("基础类型解析D2B",1)

/**
*********************************************************************
* @name：        basic_ms_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_ms_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_MSP_T ms;  
     
    memcpy(&ms, srcData, srcLen);

    //MS类型
    dstBufTmp[offset++] = DT_MS;
    
    dar = apdu_msp_tofmt698(dstBuf, dstLen, &offset, srcData, srcLen);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(dstBuf, dstLen, "apdu_csd_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    } 

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        basic_ms_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_msc_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_MSC_T ms;  
     
    memcpy(&ms, srcData, srcLen);

    //MS类型
    dstBufTmp[offset++] = DT_MS;
    
    dar = apdu_msc_tofmt698(dstBuf, dstLen, &offset, srcData, srcLen);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(dstBuf, dstLen, "apdu_csd_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    } 

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        basic_csd_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_csd_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_CSD_T csd;  
     
    memcpy(&csd, srcData, srcLen);

    /*CSD∷=CHOICE
    {
        对象属性描述符              [0] OAD，
        记录型对象属性描述符 [1] ROAD
    }
    ROAD∷=SEQUENCE
    {
        对象属性描述符            OAD，
        关联对象属性描述符 SEQUENCE OF OAD
    }*/

    //CSD类型
    dstBufTmp[offset++] = DT_CSD;
    
    dar = apdu_csd_tofmt698(dstBuf, dstLen, &offset, srcData, srcLen);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(dstBuf, dstLen, "apdu_csd_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    } 

    *pOffset = offset;

    return dar;
}

OOP_DAR_E basic_rcsd_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_RCSD_T csd;  
     
    memcpy(&csd, srcData, srcLen);

    /*CSD∷=CHOICE
    {
        对象属性描述符              [0] OAD，
        记录型对象属性描述符 [1] ROAD
    }
    ROAD∷=SEQUENCE
    {
        对象属性描述符            OAD，
        关联对象属性描述符 SEQUENCE OF OAD
    }*/

    //RCSD类型
    dstBufTmp[offset++] = DT_RCSD;
    
    dar = apdu_rcsd_tofmt698(dstBuf, dstLen, &offset, srcData, srcLen);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(dstBuf, dstLen, "apdu_csd_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    } 

    *pOffset = offset;

    return dar;
}

OOP_DAR_E basic_pcsd_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_PCSD_T csd;  
     
    memcpy(&csd, srcData, srcLen);

    /*CSD∷=CHOICE
    {
        对象属性描述符              [0] OAD，
        记录型对象属性描述符 [1] ROAD
    }
    ROAD∷=SEQUENCE
    {
        对象属性描述符            OAD，
        关联对象属性描述符 SEQUENCE OF OAD
    }*/

    //RCSD类型
    dstBufTmp[offset++] = DT_RCSD;
    
    dar = apdu_pcsd_tofmt698(dstBuf, dstLen, &offset, srcData, srcLen);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(dstBuf, dstLen, "apdu_csd_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    } 

    *pOffset = offset;

    return dar;
}



/**
*********************************************************************
* @name：        basic_csd_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_rsd_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_PRSD_T rsd;  
     
    memcpy(&rsd, srcData, srcLen);

    //RSD类型
    dstBufTmp[offset++] = DT_RSD;
    
    dar = apdu_prsd_tofmt698(dstBuf, dstLen, &offset, srcData, srcLen);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(dstBuf, dstLen, "apdu_rsd_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    } 

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：       basic_octetstr3_tofmt698
* @brief：      基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_octetstr3_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_OCTETSTR3_T octerstr;
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码   
    uint32 i = 0;

    memcpy(&octerstr, &srcDataTmp[0], srcLen);

    //octerstr数据类型
    dstBufTmp[offset++] = DT_OCTET_STRING;

    //octerstr的长度域
    offlen = set_len_offset(srcLen, lenBuf);    
    memcpy(&dstBufTmp[offset], lenBuf, offlen);
    offset += offlen;

    //octerstr数据域
    for(i = 0; i< srcLen; i++)
    {
        dstBufTmp[offset++] = octerstr.nValue[i];
    }
    *pOffset=offset;
    return nRet;
}


int32 basic_octetstr2_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
	int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
	OOP_OCTETSTR2_T octerstr;
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码   
    uint32 i = 0;

	memcpy(&octerstr, &srcDataTmp[0], srcLen);

    //octerstr数据类型
    dstBufTmp[offset++] = DT_OCTET_STRING;

    //octerstr的长度域
    offlen = set_len_offset(srcLen, lenBuf);    
    memcpy(&dstBufTmp[offset], lenBuf, offlen);
    offset += offlen;

    //octerstr数据域
    for(i = 0; i< srcLen; i++)
    {
        dstBufTmp[offset++] = octerstr.nValue[i];
    }
    *pOffset=offset;
    return nRet;

}



/**
*********************************************************************
* @name：       basic_octetstr4_tofmt698
* @brief：      基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 梅安宁
* @Date      ：2022-2-28
*********************************************************************
*/
int32 basic_octetstr4_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_OCTETSTR4_T octerstr;
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码   
    uint32 i = 0;

    memcpy(&octerstr, &srcDataTmp[0], srcLen);

    //octerstr数据类型
    dstBufTmp[offset++] = DT_OCTET_STRING;

    //octerstr的长度域
    offlen = set_len_offset(srcLen, lenBuf);    
    memcpy(&dstBufTmp[offset], lenBuf, offlen);
    offset += offlen;

    //octerstr数据域
    for(i = 0; i< srcLen; i++)
    {
        dstBufTmp[offset++] = octerstr.nValue[i];
    }
    *pOffset=offset;
    return nRet;
}


/**
*********************************************************************
* @name：       basic_octetstr32_tofmt698
* @brief：      基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_octetstr32_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_OCTETSTR32_T octerstr;
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码   
    uint32 i = 0;

    memcpy(&octerstr, &srcDataTmp[0], srcLen);

    //octerstr数据类型
    dstBufTmp[offset++] = DT_OCTET_STRING;

    //octerstr的长度域
    offlen = set_len_offset(srcLen, lenBuf);    
    memcpy(&dstBufTmp[offset], lenBuf, offlen);
    offset += offlen;

    //octerstr数据域
    for(i = 0; i< srcLen; i++)
    {
        dstBufTmp[offset++] = octerstr.nValue[i];
    }
    *pOffset=offset;
    return nRet;
}

/**
*********************************************************************
* @name：        basic_octetstr_data_to_buf
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_octetstrvar_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_OCTETVAR2K_T octetstr;
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码   
    uint32 i = 0;

    memcpy(&octetstr, &srcDataTmp[0], srcLen);

    //octetstring数据类型
    dstBufTmp[offset++] = DT_OCTET_STRING;

    //octetstring的长度域
    offlen = set_len_offset(octetstr.nNum, lenBuf);    
    memcpy(&dstBufTmp[offset], lenBuf, offlen);
    offset += offlen;

    //octetstring数据域
    for(i = 0; i< octetstr.nNum; i++)
    {
        dstBufTmp[offset++] = octetstr.value[i];
    }
	*pOffset=offset;
    return nRet;
}

/**
*********************************************************************
* @name：        basic_octetstr_data_to_buf
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_sa_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_SA_T octetstr;
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码   
    uint32 i = 0;

    memcpy(&octetstr, &srcDataTmp[0], srcLen);

    //octetstring数据类型
    dstBufTmp[offset++] = DT_OCTET_STRING;

    //octetstring的长度域
    offlen = set_len_offset(octetstr.len, lenBuf);    
    memcpy(&dstBufTmp[offset], lenBuf, offlen);
    offset += offlen;

    //octetstring数据域
    for(i = 0; i< octetstr.len; i++)
    {
        dstBufTmp[offset++] = octetstr.add[i];
    }
	*pOffset=offset;
    return nRet;
}

/**
*********************************************************************
* @name：        basic_visiblestr_data_to_buf
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_visiblestrvar_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_VISIBLEVAR1K_T visiblestr;
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码   
    uint32 i = 0;

    memcpy(&visiblestr, &srcDataTmp[0], srcLen);

    //visiblestr数据类型
    dstBufTmp[offset++] = DT_VISIBLE_STRING;

    //visiblestr的长度域
    offlen = set_len_offset(visiblestr.nNum, lenBuf);    
    memcpy(&dstBufTmp[offset], lenBuf, offlen);
    offset += offlen;

    //visiblestr数据域
    for(i = 0; i< visiblestr.nNum; i++)
    {
        dstBufTmp[offset++] = visiblestr.value[i];
    }
    *pOffset=offset;
    return nRet;
}

/**
*********************************************************************
* @name：        basic_visiblestr_data_to_buf
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_visiblestr32_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_VISIBLESTR20_T visiblestr;
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码   
    uint32 i = 0;

    memcpy(&visiblestr, &srcDataTmp[0], srcLen);

    //visiblestr数据类型
    dstBufTmp[offset++] = DT_VISIBLE_STRING;

    //visiblestr的长度域
    offlen = set_len_offset(srcLen, lenBuf);    
    memcpy(&dstBufTmp[offset], lenBuf, offlen);
    offset += offlen;

    //visiblestr数据域
    for(i = 0; i< srcLen; i++)
    {
        dstBufTmp[offset++] = visiblestr.value[i];
    }
    *pOffset=offset;
    return nRet;
}

int32 basic_visiblestr256_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_VISIBLEVAR256_T visiblestr;
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码   
    uint32 i = 0;

    memcpy(&visiblestr, &srcDataTmp[0], srcLen);

    //visiblestr数据类型
    dstBufTmp[offset++] = DT_VISIBLE_STRING;

    //visiblestr的长度域
    offlen = set_len_offset(visiblestr.nNum, lenBuf);
    memcpy(&dstBufTmp[offset], lenBuf, offlen);
    offset += offlen;

    //visiblestr数据域
    for(i = 0; i < visiblestr.nNum; i++)
    {
        dstBufTmp[offset++] = visiblestr.value[i];
    }
    *pOffset=offset;
    return nRet;
}


/**
*********************************************************************
* @name：        basic_visiblestr_data_to_buf(定长)
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_bitstr16_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_BITSTR16_T bitstring;

    memcpy(&bitstring, &srcDataTmp[0], sizeof(OOP_BITSTR16_T));

    //bitstring数据类型
    dstBufTmp[offset++] = DT_BIT_STRING;

    //bitstring的长度域
    dstBufTmp[offset++] = 16;

    //bitstring数据域
    membitrev(bitstring.nValue, 16);
    memcpy(&dstBufTmp[offset], bitstring.nValue, 2);
    //memcpy_r(&dstBufTmp[offset], bitstring.nValue, 2);
    offset += 2;
    
    *pOffset=offset;
    
    return nRet;
}

/**
*********************************************************************
* @name：        basic_datetime_s_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_datetime_s_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_DATETIME_S_T data;

    memcpy(&data, &srcDataTmp[0], srcLen);

    //数据类型
    dstBufTmp[offset++] = DT_DATETIME_S;
    
    memcpy_r(&dstBufTmp[offset], &data.year, sizeof(uint16));
    offset += sizeof(uint16);

    dstBufTmp[offset++] = data.month;  
    dstBufTmp[offset++] = data.day;           
    dstBufTmp[offset++] = data.hour; 
    dstBufTmp[offset++] = data.minute; 
    dstBufTmp[offset++] = data.second; 
    
    *pOffset = offset;

    return nRet;
}

/**
*********************************************************************
* @name：        basic_date_s_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_date_s_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_DATE_T data;

    memcpy(&data, &srcDataTmp[0], srcLen);

    //数据类型
    dstBufTmp[offset++] = DT_DATE;
    
    memcpy_r(&dstBufTmp[offset], &data.year, sizeof(uint16));
    offset += sizeof(uint16);

    dstBufTmp[offset++] = data.month;  
    dstBufTmp[offset++] = data.mday;           
    dstBufTmp[offset++] = data.wday; 
    
    *pOffset = offset;

    return nRet;
}
/**
*********************************************************************
* @name：        basic_datetime_s_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_ti_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;   
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_TI_T data;

    if(srcLen < sizeof(data))
    {
        PRTL_FMT_DEBUG("srclen check failed. expect: srclen(%d) >= size(%d)\n", srcLen, sizeof(data));
        return ERR_OPTION;
    }
    
    memcpy(&data, &srcDataTmp[0], sizeof(data));

    //数据类型
    dstBufTmp[offset++] = DT_TI;

    dstBufTmp[offset++] = data.unit;  

    memcpy_r(&dstBufTmp[offset], &data.value, sizeof(uint16));
    offset += sizeof(uint16);

    *pOffset = offset;

    return nRet;
}

/**
*********************************************************************
* @name：       basic_time_tofmt698
* @brief：      基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_time_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_TIME_T data = {0};

    memcpy(&data, &srcDataTmp[0], srcLen);

    //数据类型
    dstBufTmp[offset++] = DT_TIME;

    dstBufTmp[offset++] = data.hour; 
    dstBufTmp[offset++] = data.minute; 
    dstBufTmp[offset++] = data.second; 
    
    *pOffset = offset;

    return nRet;
}

/**
*********************************************************************
* @name：        basic_comdcb_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_comdcb_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_COMDCB_T data;

    if(srcLen < sizeof(data))
    {
        PRTL_FMT_DEBUG("srclen check failed. expect: srclen(%d) >= size(%d)\n", srcLen, sizeof(data));
        return ERR_OPTION;
    }
    
    memcpy(&data, &srcDataTmp[0], sizeof(data));

    //数据类型
    dstBufTmp[offset++] = DT_COMDCB;

    //波特率
    memcpy_rev(&dstBufTmp[offset], &data.baud, sizeof(data.baud));
    offset += sizeof(data.baud);

    //校验
    memcpy_rev(&dstBufTmp[offset], &data.parity, sizeof(data.parity));
    offset += sizeof(data.parity);

    //数据位
    memcpy_rev(&dstBufTmp[offset], &data.databits, sizeof(data.databits));
    offset += sizeof(data.databits);

    //停止位
    memcpy_rev(&dstBufTmp[offset], &data.stopbits, sizeof(data.stopbits));
    offset += sizeof(data.stopbits);

    //流控
    memcpy_rev(&dstBufTmp[offset], &data.flowctrl, sizeof(data.flowctrl));
    offset += sizeof(data.flowctrl);
    
    *pOffset = offset;

    return nRet;
}

/**
*********************************************************************
* @name：        basic_scaler_unit_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_scaler_unit_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_SCALER_UNIT_T data;

    if(srcLen < sizeof(data))
    {
        PRTL_FMT_DEBUG("srclen check failed. expect: srclen(%d) >= size(%d)\n", srcLen, sizeof(data));
        return ERR_OPTION;
    }
    
    memcpy(&data, &srcDataTmp[0], sizeof(data));

    //数据类型
    dstBufTmp[offset++] = DT_SCALER_UNIT;

    //换算
    dstBufTmp[offset++] = data.scaler; 

    //单位
    dstBufTmp[offset++] = data.unit;  
    
    *pOffset = offset;

    return nRet;
}

/**
*********************************************************************
* @name：        basic_region_tofmt698
* @brief：       698报文格式转换为基本类型数据
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2020-04-26
*********************************************************************
*/
OOP_DAR_E basic_region_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_REGIOND_T data;

    if(srcLen < sizeof(data))
    {
        PRTL_FMT_DEBUG("srclen check failed. expect: srclen(%d) >= size(%d)\n", srcLen, sizeof(data));
        return ERR_OPTION;
    }
    
    memcpy(&data, &srcDataTmp[0], sizeof(data));

    //数据类型
    dstBufTmp[offset++] = DT_REGION;

    dstBufTmp[offset++] = data.nType;
    
    dar = digit_to_fmt698(dstBuf, dstLen, &offset, &data.start, sizeof(OOP_DIGIT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("digit_to_fmt698 failed. dar(%d), parm.choice(%d)\n", dar, data.start.choice);
        return DATA_TYPE_UNMATCHED;
    }

    dar = digit_to_fmt698(dstBuf, dstLen, &offset, &data.end, sizeof(OOP_DIGIT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("digit_to_fmt698 failed. dar(%d), parm.choice(%d)\n", dar, data.end.choice);
        return DATA_TYPE_UNMATCHED;
    }
    
    *pOffset = offset;
    
    return dar;
}

/**
*********************************************************************
* @name：        basic_data_to_buf
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_data_to_fmt698(DATA_E eData, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_OTHER_REASON;
    uint32  sOffset = 0;
    uint32  dOffset = 0;
    uint8 *srcBuf = (uint8 *)convert->srcBuf;    
    uint8 *dstBuf = (uint8 *)convert->dstBuf;
    uint32 srcLen = 0;
    uint32 dstLen = 0;   
    uint32 dataLen = 0;

     /*入参检查*/
    if ((convert == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL)|| 
        (convert->sOffset == NULL) || (convert->dOffset == NULL))
    {
        PRTL_FMT_DEBUG("basic_data_to_fmt698 check failed. ERR_PTR\n");
        return ERR_PTR;
    }

    if(!basic_type_check(eData))
    {
        PRTL_FMT_DEBUG("basic_type_check failed. eData(%u)\n", eData);      
    }
    
    sOffset = *convert->sOffset;
    dOffset = *convert->dOffset;
    srcLen = convert->srcLen;
    dstLen = convert->dstLen;

    switch (eData)
    {
        case E_NULL:                            /**< NULL               */
        {
            dstBuf[dOffset++] = DT_NULL;
            dar = ERR_OK;
            break;
        }

        case E_ARRAY:                           /**< vector             */
            break;

        case E_STRUCTURE:                       /**< struct             */
            break;

        case E_BOOL:                            /**< BOOLEAN               */
        {
            dataLen = sizeof(uint8);
            
            if ((srcLen - sOffset) < sizeof(uint8))       //bool传进来未必是1，当前的bool是定义的4字节
            {
                dar = DATA_OTHER_REASON;
                break;
            }

            dstBuf[dOffset++] = DT_BOOL;
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            dOffset += dataLen;
            sOffset += dataLen;
            
            dar = ERR_OK;
            break;
        }
            
        case E_BIT_STRING:                      /**< CBitString         */
        {
            break;
        }

        case E_OOP_BITSTR3_T:
        {
            dataLen = sizeof(uint8);
            
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            dstBuf[dOffset++] = DT_BIT_STRING;
            dstBuf[dOffset++] = 3;
            
            memcpy(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            //字节反转
            membitrev(&dstBuf[dOffset], 8);
            dOffset += dataLen;
            sOffset += dataLen;

            dar = ERR_OK;
            break;
        }

        case E_OOP_BITSTR8_T:
        {
            dataLen = sizeof(uint8);
            
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            dstBuf[dOffset++] = DT_BIT_STRING;
            dstBuf[dOffset++] = 8;
            
            memcpy(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
			//字节反转
			membitrev(&dstBuf[dOffset], 8);
            dOffset += dataLen;
            sOffset += dataLen;

            dar = ERR_OK;
            break;
        }

        case E_OOP_BITSTR64_T:
        {
            dataLen = 8*sizeof(uint8);
            
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            dstBuf[dOffset++] = DT_BIT_STRING;
            dstBuf[dOffset++] = 64;
            
            memcpy(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
			//字节反转
			membitrev(&dstBuf[dOffset], 64);
            dOffset += dataLen;
            sOffset += dataLen;

            dar = ERR_OK;
            break;
        }

        case E_OOP_BITSTR128_T:
        {
            dataLen = 16*sizeof(uint8);
            
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            dstBuf[dOffset++] = DT_BIT_STRING;
            dstBuf[dOffset++] = 0x81;
            //dstBuf[dOffset++] = 0x00;
            dstBuf[dOffset++] = 0x80;
            
            memcpy(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
			//字节反转
			membitrev(&dstBuf[dOffset], 128);
            dOffset += dataLen;
            sOffset += dataLen;

            dar = ERR_OK;
            break;
        }

        case E_OOP_BITSTR16_T:
        {
            dataLen = sizeof(OOP_BITSTR16_T);     
            dar = basic_bitstr16_tofmt698(dstBuf, dstLen, &dOffset, &srcBuf[sOffset], srcLen-sOffset);
            sOffset += dataLen;
            break;
        }

        case E_DOUBLE_LONG:                     /**< int                */
        {
            dataLen = sizeof(int32);
            
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            dstBuf[dOffset++] = DT_DOUBLE_LONG;
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            dOffset += dataLen;
            sOffset += dataLen;

            dar = ERR_OK;
            break;
        }

        case E_DOUBLE_LONG_UNSIGNED:            /**< uint32             */
        {
            dataLen = sizeof(uint32);
            
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            dstBuf[dOffset++] = DT_DOUBLE_LONG_UNSIGNED;
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            dOffset += dataLen;
            sOffset += dataLen;

            dar = ERR_OK;
            break;
        }

        case E_OCTET_STR6_T:                    /**< octet-string(SIZE(6)) 统一使用最大结构  */
        {
            dataLen = sizeof(OOP_OCTETSTR6_T);     
            dar = basic_octetstr32_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
            break;
        }

        case E_OCTET_STR8_T:                    /**< octet-string(SIZE(8)) 统一使用最大结构  */
        {
            dataLen = sizeof(OOP_OCTETSTR8_T);     
            dar = basic_octetstr32_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
            break;
        }

        case E_OCTET_STR32_T:                    /**< octet-string(SIZE(32))   */
        {
            dataLen = sizeof(OOP_OCTETSTR32_T);     
            dar = basic_octetstr32_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
            break;
        }

        case E_OCTET_STRING:                    /**< COctetString   可变    */
        {
            dataLen = sizeof(OOP_OCTETVAR2K_T);     
            dar = basic_octetstrvar_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
            break;
        }   

        case E_OOP_OCTETVAR64_T:                /**< COctetString       */
        {
            dataLen = sizeof(OOP_OCTETVAR64_T);     
            dar = basic_octetstrvar_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
            break;
        } 
        
        case E_OOP_OCTETVAR256_T:                /**< COctetString       */
        {
            dataLen = sizeof(OOP_OCTETVAR256_T);     
            dar = basic_octetstrvar_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
            break;
        } 

        case E_OOP_OCTETVAR1K_T:                /**< COctetString       */
        {
            dataLen = sizeof(OOP_OCTETVAR1K_T);     
            dar = basic_octetstrvar_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
            break;
        }

        case E_OOP_OCTETVAR2K_T:                /**< COctetString       */
        {
            dataLen = sizeof(OOP_OCTETVAR2K_T);     
            dar = basic_octetstrvar_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
            break;
        }

        case E_OOP_OCTETVAR16_T:
        {
            dataLen = sizeof(OOP_OCTETVAR16_T);     
            dar = basic_octetstrvar_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
            break;
        } 

        case E_OCTET_STR3_T:
        {
            dataLen = sizeof(OOP_OCTETSTR3_T);     
            dar = basic_octetstr3_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
            break;
        } 

		case E_OCTET_STR4_T:
        {
            dataLen = sizeof(OOP_OCTETSTR4_T);     
            dar = basic_octetstr4_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
            break;
        } 

		case E_OCTET_STR2_T:
		{
			dataLen = sizeof(OOP_OCTETSTR2_T);     
            dar = basic_octetstr2_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
            break;
		}

        case E_OOP_SA_T:
        {
            dataLen = sizeof(OOP_SA_T);     
            dar = basic_sa_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
            break;
        }  

        case E_OOP_VISIBLEVAR_T:
        {
            dataLen = sizeof(OOP_VISIBLEVAR_T);     
            dar = basic_visiblestrvar_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);  
            sOffset += dataLen;
            break;
        } 

        case E_OOP_VISIBLESTR4_T:
        {
            dataLen = sizeof(OOP_VISIBLESTR4_T);    
            srcLen = 0x04; 
            dar = basic_visiblestr32_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);  
            sOffset += dataLen;
            break;
        }

        case E_OOP_VISIBLESTR6_T:
        {
            dataLen = sizeof(OOP_VISIBLESTR6_T);   
            srcLen = 0x06;   
            dar = basic_visiblestr32_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);  
            sOffset += dataLen;
            break; 
        }

        case E_OOP_VISIBLESTR8_T:
        {
            dataLen = sizeof(OOP_VISIBLESTR8_T);  
            srcLen = 0x08;    
            dar = basic_visiblestr32_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);  
            sOffset += dataLen;
            break;
        }

        case E_OOP_VISIBLESTR11_T:
        {
            dataLen = sizeof(OOP_VISIBLESTR11_T);  
            srcLen = 11;    
            dar = basic_visiblestr32_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);  
            sOffset += dataLen;
            break;
        }

        case E_OOP_VISIBLEVAR32_T:
        {
            dataLen = sizeof(OOP_VISIBLEVAR32_T);     
            dar = basic_visiblestrvar_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);  
            sOffset += dataLen;
            break;
        }
        case E_OOP_VISIBLESTR256_T:
        {
            dataLen = sizeof(OOP_VISIBLEVAR256_T);     
            dar = basic_visiblestr256_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);  
            sOffset += dataLen;
            break;
        }

        case E_VISIBLE_STRING:                  /**< string             */
            dataLen = sizeof(OOP_VISIBLEVAR1K_T);     
            dar = basic_visiblestrvar_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);  
            sOffset += dataLen;
            break;
        case E_UTF8_STRING:                     /**< string             */
            break;
        case E_INTEGER:                         /**< char               */
        {
            dataLen = sizeof(char);
            
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            dstBuf[dOffset++] = DT_INTEGER;
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            dOffset += dataLen;
            sOffset += dataLen;

            dar = ERR_OK;
            break;
        }

        case E_LONG:                            /**< int16              */
        {
            dataLen = sizeof(int16);
            
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            dstBuf[dOffset++] = DT_LONG;
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            dOffset += dataLen;
            sOffset += dataLen;

            dar = ERR_OK;
            break;
        }

        case E_UNSIGNED:                        /**< uint8              */
        {
            dataLen = sizeof(uint8);
            
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            dstBuf[dOffset++] = DT_UNSIGNED;
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            dOffset += dataLen;
            sOffset += dataLen;

            dar = ERR_OK;
            break;
        }

        case E_LONG_UNSIGNED:                   /**< uint16             */
        {
            dataLen = sizeof(uint16);
            
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            dstBuf[dOffset++] = DT_LONG_UNSIGNED;
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            dOffset += dataLen;
            sOffset += dataLen;

            dar = ERR_OK;
            break;
        }

        case E_LONG64:                          /**< int64              */
        {
            dataLen = sizeof(int64);
            
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }

            int64 tmp = 0;
            memcpy(&tmp, &srcBuf[sOffset], dataLen);
            if (tmp == 0xeeeeeeeeeeeeeeee)
            {
                dstBuf[dOffset++] = DT_NULL;
                sOffset += dataLen;
            }
            else
            {
                dstBuf[dOffset++] = DT_LONG64;
                
                memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
                dOffset += dataLen;
                sOffset += dataLen;
            }

            dar = ERR_OK;
            break;
        }

        case E_LONG64_UNSIGNED:                 /**< uint64             */
        {
            dataLen = sizeof(uint64);
            
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            dstBuf[dOffset++] = DT_LONG64_UNSIGNED;
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            dOffset += dataLen;
            sOffset += dataLen;

            dar = ERR_OK;
            break;

        }

        case E_ENUM:                            /**< uint8              */
        {
            dataLen = sizeof(uint8);
            
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            dstBuf[dOffset++] = DT_ENUM;
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            dOffset += dataLen;
            sOffset += dataLen;

            dar = ERR_OK;
            break;
        }

        case E_FLOAT32:                         /**< float32            */
        {
            dataLen = sizeof(float32);
            
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            dstBuf[dOffset++] = DT_FLOAT32;
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            dOffset += dataLen;
            sOffset += dataLen;

            dar = ERR_OK;
            break;
        }

        case E_FLOAT64:                         /**< float64            */
        {
            dataLen = sizeof(float64);
            
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            dstBuf[dOffset++] = DT_FLOAT64;
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            dOffset += dataLen;
            sOffset += dataLen;

            dar = ERR_OK;
            break;
        }

        case E_DATETIME:                        /**< DateTime           */
            break;
        case E_DATE:                            /**< OOP_DATE_T         */
        {
            dataLen = sizeof(OOP_DATE_T);
            dar = basic_date_s_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;            
        }break;
        case E_TIME:                            /**< OOP_TIME_T         */
        {
            dataLen = sizeof(OOP_TIME_T);
            dar = basic_time_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen; 
            break;
        }

        case E_DATETIME_S:                      /**< DateTime_S         */
        {
            if(check_is_all_ch(srcBuf,0xFF,sizeof(OOP_DATETIME_S_T))||check_is_all_ch(srcBuf,0x00,sizeof(OOP_DATETIME_S_T)))
            {
                    dstBuf[dOffset++] = DT_NULL;
                    dar = ERR_OK;
                    break;
            }
            dataLen = sizeof(OOP_DATETIME_S_T);
            dar = basic_datetime_s_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;            
        }break;
        case E_OI:                              /**< OI                 */
        {
            dataLen = sizeof(OI);
            
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            dstBuf[dOffset++] = DT_OI;
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            dOffset += dataLen;
            sOffset += dataLen;

            dar = ERR_OK;
            
        }break;

        case E_OAD:                             /**< OOP_OAD_U          */
        {
            dataLen = sizeof(OOP_OAD_U);
            
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            dstBuf[dOffset++] = DT_OAD;
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            dOffset += dataLen;
            sOffset += dataLen;

            dar = ERR_OK;
            break;
        }
            
        case E_ROAD:                            /**< OOP_ROAD_T         */
            break;
        case E_OMD:                             /**< OOP_OMD_U          */
            break;
        case E_TI:                              /**< 时间间隔           */
            dataLen = sizeof(OOP_TI_T);
            dar = basic_ti_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen; 
            break;
        case E_TSA:                             /**< 设备地址           */
        {
            dataLen = sizeof(OOP_TSA_T);
            dar = basic_tsa_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;            
        }break;
            
        case E_MAC:                             /**< OOP_MAC_T签名        */
            break;
        case E_RN:                              /**< 随机数              */
            break;
        case E_REGION:                          /**< 区间类型           */
        {
            dataLen = sizeof(OOP_REGIOND_T);
            dar = basic_region_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;            
        }break;

        case E_SCALER_UNIT:                     /**< 换算单位           */
        {
            dataLen = sizeof(OOP_SCALER_UNIT_T);
        
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }

            dar = basic_scaler_unit_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
            break;
        }

        case E_RSD:                             /**< 行选择符           */
            dataLen = sizeof(OOP_PRSD_T);
        
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }

            dar = basic_rsd_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
            break;
        case E_CSD:                             /**< 列选择符           */
        {    dataLen = sizeof(OOP_CSD_T);
        
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            dar = basic_csd_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
        }break;
        case E_MS:                              /**< 电能表集合            */
            {
                dataLen = sizeof(OOP_MSP_T);
        
                if ((srcLen - sOffset) < dataLen)
                {
                    dar = DATA_OTHER_REASON;
                    break;
                }

                dar = basic_ms_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
                sOffset += dataLen;
            }
            break;

        #ifdef AREA_FUJIAN
        case E_MS_C:                              /**< 电能表集合            */
            {
                dataLen = sizeof(OOP_MSC_T);

                if ((srcLen - sOffset) < dataLen)
                {
                    dar = DATA_OTHER_REASON;
                    break;
                }

                dar = basic_msc_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);

                sOffset += dataLen;
            }
            break;
        
        case E_MS_F:                              /**< 电能表集合            */
            {
                dataLen = sizeof(OOP_MSPF_T);
        
                if ((srcLen - sOffset) < dataLen)
                {
                    dar = DATA_OTHER_REASON;
                    break;
                }

                dar = basic_ms_tofmt698_fj(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
                sOffset += dataLen;
            }
            break;
        #endif

        case E_SID:                             /**< 安全标识           */
            break;
        case E_SIDMAC:                          /**< 安全标识+OOP_MAC_T */
            break;
        case E_RCSD:                            /**< 记录列选择描述符   */
            {
                dataLen = sizeof(OOP_RCSD_T);
        
                if ((srcLen - sOffset) < dataLen)
                {
                    dar = DATA_OTHER_REASON;
                    break;
                }
                dar = basic_rcsd_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
                sOffset += dataLen;
            }
            break;
        case E_PCSD:                            /**< 记录列选择描述符   */
            {
                dataLen = sizeof(OOP_PCSD_T);
        
                if ((srcLen - sOffset) < dataLen)
                {
                    dar = DATA_OTHER_REASON;
                    break;
                }
                dar = basic_pcsd_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
                sOffset += dataLen;
            }
            break;
        case E_COMDCB:                          /* 串口通信控制块 */
        {
            dataLen = sizeof(OOP_COMDCB_T);
        
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }

            dar = basic_comdcb_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
            break;
        }
        
        default:
            dar = DATA_OTHER_REASON;
            break;
    }
           
       
    *convert->sOffset = sOffset;
    *convert->dOffset = dOffset;
    
    return dar;
}

/**
*********************************************************************
* @brief：      数据中心格式转698规约格式
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
* @author    : 贺 宁
* @Date      ：2019-12-6               
*********************************************************************
*/
OOP_DAR_E basic_db_to_fmt698(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    DATA_E eData = 0;

     /*入参检查*/
    if ((pOadInfo == NULL) || (convert == NULL) || (convert->srcBuf == NULL) || 
        (convert->dstBuf == NULL)|| (convert->sOffset == NULL) || (convert->dOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_check failed. ERR_PTR\n");
        return ERR_PTR;
    }

    eData = pOadInfo->subeData;

    if(!basic_type_check(eData))
    {
        PRTL_FMT_DEBUG("basic_type_check failed. eData(%u)\n", eData);      
    }
    
    return basic_data_to_fmt698(eData, convert);  
}


/**
*********************************************************************
* @name：        basic_data_to_buf
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_data_to_buf(DATA_E eData, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;

    /*入参检查*/
    if ((srcData == NULL) || (dstBuf == NULL) || (pOffset == NULL) || (!basic_type_check(eData)))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf check failed. ERR_PTR\n");
        return DATA_OTHER_REASON;
    }

    MEMZERO(&convert, sizeof(convert));
   
    convert.srcBuf = srcData;
    convert.srcLen = srcLen;
    convert.sOffset = &sOffset;;

    convert.dstBuf = dstBuf;
    convert.dstLen = dstLen;
    convert.dOffset = pOffset;

    dar = basic_data_to_fmt698(eData, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_from_fmt698 failed. eData(%u)\n", eData);
        return dar;
    }

    if(sOffset > srcLen)
    {
        PRTL_FMT_DEBUG("srcData cross the border. sOffset(%d), srcLen(%d)\n", sOffset, srcLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }
    
    return dar;
}


#endif

#if DESC("基础类型解析B2D",1)
/**
*********************************************************************
* @name：        basic_ms_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_ms_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_MSP_T ms;
     
    MEMZERO(&ms, sizeof(ms));

    //类型
    if(srcBufTmp[offset++] != DT_MS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check csd type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    dar = apdu_ms_fromfmt698(srcBuf, srcLen, &offset, &ms, sizeof(ms));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_csd_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }  
 
    memcpy(dstData, &ms, sizeof(ms));

    *pOffset = offset;

    return dar;
}


/**
*********************************************************************
* @name：        basic_csd_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_csd_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_CSD_T csd;
     
    MEMZERO(&csd, sizeof(csd));

    //类型
    if(srcBufTmp[offset++] != DT_CSD)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check csd type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    dar = apdu_csd_fromfmt698(srcBuf, srcLen, &offset, &csd, sizeof(OOP_CSD_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_csd_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }  
 
    memcpy(dstData, &csd, sizeof(csd));

    *pOffset = offset;

    return dar;
}


/**
*********************************************************************
* @name：        basic_pcsd_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_rcsd_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_PCSD_T rcsd;
    uint32 i = 0;
     
    MEMZERO(&rcsd, sizeof(rcsd));

    /*RCSD∷=SEQUENCE OF CSD*/

    //类型
    if(srcBufTmp[offset++] != DT_RCSD)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check rcsd type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    rcsd.nNum = srcBufTmp[offset++];

    for(i = 0; i < rcsd.nNum; i++)
    {
        dar = apdu_csd_fromfmt698(srcBuf, srcLen, &offset, &rcsd.cols[i], sizeof(OOP_CSD_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_csd_fromfmt698 failed. offset(%d)", offset);
            return DATA_TYPE_UNMATCHED;           
        }

        PRTL_FMT_TRACE("[%d] choice(%d), oad(0x%08x)\n", i, rcsd.cols[i].choice, rcsd.cols[i].oad.value);        
    }
    
    memcpy(dstData, &rcsd, sizeof(rcsd));

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        basic_rsd_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_rsd_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_PRSD_T rsd;
     
    MEMZERO(&rsd, sizeof(rsd));

    //类型
    if(srcBufTmp[offset++] != DT_RSD)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check rsd type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    dar = apdu_rsd_fromfmt698(srcBuf, srcLen, &offset, &rsd, sizeof(OOP_PRSD_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_rsd_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }  
 
    memcpy(dstData, &rsd, sizeof(rsd));

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        apdu_ti_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_ti_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_TI_T data; 
    
    MEMZERO(&data, sizeof(data));

    //DT_TI类型
    if(srcBufTmp[offset++] != DT_TI)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check ti type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    } 

    dar = apdu_ti_fromfmt698(srcBuf, srcLen, &offset, &data, sizeof(OOP_TI_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_ti_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }  

    memcpy(dstData, &data, sizeof(data));

    *pOffset = offset;

    return dar;     

}


/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_datetime_s_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_DATETIME_S_T data; 
    
    MEMZERO(&data, sizeof(data));

    //DATETIME_S类型
    if(srcBufTmp[offset++] != DT_DATETIME_S)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check datetime_s type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }  

    dar = apdu_datetime_s_fromfmt698(srcBuf, srcLen, &offset, &data, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_datetime_s_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    memcpy(dstData, &data, sizeof(data));

    *pOffset = offset;

    return dar;     

}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_time_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_TIME_T data = {0}; 
    
    MEMZERO(&data, sizeof(data));

    //TIME类型
    if(srcBufTmp[offset++] != DT_TIME)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check time type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }  

    data.hour =  srcBufTmp[offset++];
    data.minute =  srcBufTmp[offset++];
    data.second =  srcBufTmp[offset++];

    memcpy(dstData, &data, sizeof(data));

    *pOffset = offset;

    return dar;     

}

/**
*********************************************************************
* @name：        basic_visblestrvar2k_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_visblestr32_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_VISIBLESTR20_T data;
    uint8 offlen = 0;      //数组数量长度
    uint32 nNum  = 0;

    MEMZERO(&data, sizeof(data));

    //最大长度容错
    if (dstLen > sizeof(data))
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check visblestrvar2k length failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //string类型
    if(srcBufTmp[offset++] != DT_VISIBLE_STRING)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check visblestrvar2k type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    //COctetString长度
    nNum = get_len_offset(&srcBufTmp[offset], &offlen);
    offset += offlen;
    
    memcpy(&data.value[0], &srcBufTmp[offset], nNum); 
    offset += nNum;

    memcpy(dstData, &data, dstLen);
    
    *pOffset = offset;
    
    return nRet;
}

/**
*********************************************************************
* @name：       basic_octetstr32_fromfmt698
* @brief：      基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_octetstr32_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_OCTETSTR32_T data;
    uint8 offlen = 0;      //数组数量长度
    uint32 nNum  = 0;

    MEMZERO(&data, sizeof(data));

    //最大长度容错
    if (dstLen > sizeof(data))
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check visblestrvar2k length failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //string类型
    if(srcBufTmp[offset++] != DT_OCTET_STRING)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check visblestrvar2k type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    //COctetString长度
    nNum = get_len_offset(&srcBufTmp[offset], &offlen);
    offset += offlen;
    
    memcpy(&data.nValue[0], &srcBufTmp[offset], nNum); 
    offset += nNum;

    memcpy(dstData, &data, dstLen);
    
    *pOffset = offset;
    
    return nRet;
}


/**
*********************************************************************
* @name：        basic_visblestrvar1k_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_visblestrvar1k_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_VISIBLEVAR1K_T data = {0};
    uint8 offlen = 0;      //数组数量长度
    uint32 nNum  = 0;

    MEMZERO(&data, sizeof(data));

    //最大长度容错
    if (dstLen > sizeof(data))
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check visblestrvar2k length failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //string类型
    if(srcBufTmp[offset++] != DT_VISIBLE_STRING)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check visblestrvar2k type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    //visible-string1K(变长)
    nNum = get_len_offset(&srcBufTmp[offset], &offlen);
    offset += offlen;

    data.nNum = nNum;
    
    memcpy(&data.value[0], &srcBufTmp[offset], nNum); 
    offset += nNum;

    memcpy(dstData, &data, dstLen);
    
    *pOffset = offset;
    
    return nRet;
}

/**
*********************************************************************
* @name：        basic_octetstrvar_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_octetstrvar2k_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_OCTETVAR2K_T data = {0};
    uint8 offlen = 0;      //数组数量长度

    MEMZERO(&data, sizeof(data));

    if (dstLen > sizeof(data))
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check octetstrvar2k len failed. offset(%d)", offset);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }
    
    //COctetString类型
    if(srcBufTmp[offset++] != DT_OCTET_STRING)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check octetstrvar2k type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    //COctetString长度
    data.nNum = get_len_offset(&srcBufTmp[offset], &offlen);
    offset += offlen;
    
    memcpy(&data.value[0], &srcBufTmp[offset], data.nNum); 
    offset += data.nNum;

    memcpy(dstData, &data, dstLen);
    
    *pOffset = offset;
    
    return nRet;
}

/**
*********************************************************************
* @name：       basic_sa_fromfmt698
* @brief：      基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_sa_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SA_T data = {0};
    uint8 offlen = 0;      //数组数量长度

    MEMZERO(&data, sizeof(data));

    if (dstLen > sizeof(data))
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check sa len failed. offset(%d)", offset);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }
    
    //COctetString类型
    if(srcBufTmp[offset++] != DT_OCTET_STRING)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check sa type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    //COctetString长度
    data.len = get_len_offset(&srcBufTmp[offset], &offlen);
    offset += offlen;
    
    memcpy(&data.add[0], &srcBufTmp[offset], data.len); 
    offset += data.len;

    memcpy(dstData, &data, dstLen);
    
    *pOffset = offset;
    
    return nRet;
}

/**
*********************************************************************
* @name：        basic_octetstrvar_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_mac_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_MAC_T data = {0};
    uint8 offlen = 0;      //数组数量长度

    MEMZERO(&data, sizeof(data));
    
    //COctetString类型
    if(srcBufTmp[offset++] != DT_OCTET_STRING)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check mac type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    //COctetString长度
    data.nDNum = get_len_offset(&srcBufTmp[offset], &offlen);
    offset += offlen;
    
    memcpy(&data.data[0], &srcBufTmp[offset], data.nDNum); 
    offset += data.nDNum;

    memcpy(dstData, &data, sizeof(data));
    
    *pOffset = offset;
    
    return nRet;
}


/**
*********************************************************************
* @name：        basic_datetime_s_tofmt698
* @brief：       698报文格式转换为基本类型数据
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_comdcb_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_COMDCB_T data;

    MEMZERO(&data, sizeof(data));
    
    //COMDCB类型
    if(srcBufTmp[offset++] != DT_COMDCB)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check tsa type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    data.baud = srcBufTmp[offset++];
    data.parity = srcBufTmp[offset++];
    data.databits = srcBufTmp[offset++];
    data.stopbits = srcBufTmp[offset++];
    data.flowctrl = srcBufTmp[offset++];

    memcpy(dstData, &data, sizeof(data));
    
    *pOffset = offset;
    
    return nRet;
}

/**
*********************************************************************
* @name：       basic_scaler_unit_fromfmt698
* @brief：      698报文格式转换为基本类型数据
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_scaler_unit_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SCALER_UNIT_T data;

    MEMZERO(&data, sizeof(data));
    
    if (srcBufTmp[offset++] != DT_SCALER_UNIT)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check tsa type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    data.scaler = srcBufTmp[offset++];
    data.unit = srcBufTmp[offset++];

    memcpy(dstData, &data, sizeof(data));
    
    *pOffset = offset;
    
    return nRet;
}

/**
*********************************************************************
* @name：        basic_region_fromfmt698
* @brief：       698报文格式转换为基本类型数据
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2020-04-26
*********************************************************************
*/
OOP_DAR_E basic_region_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_REGIOND_T data;

    MEMZERO(&data, sizeof(data));
    
    //REGION类型
    if(srcBufTmp[offset++] != DT_REGION)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check region type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = apdu_region_fromfmt698(srcBuf, srcLen, &offset, &data, sizeof(OOP_REGIOND_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_rsd_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }  

    memcpy(dstData, &data, sizeof(data));
    
    *pOffset = offset;
    
    return dar;
}

/**
*********************************************************************
* @name：        basic_checksum_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_checksum_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_CHECKSUM_T data = {0};
    uint8 offlen = 0;      //数组数量长度

    MEMZERO(&data, sizeof(data));

    if (dstLen > sizeof(data))
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check checksum len failed. offset(%d)", offset);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }
    
    if(srcBufTmp[offset++] != DT_CHECKSUM)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check checksum type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    data.checktype = srcBufTmp[offset++];           //checksum的校验类型
    if(data.checktype > 3 && data.checktype != 255)
    {
        PRTL_FMT_DEBUG("checksum校验类型错误%d\n", data.checktype);
        return DATA_TYPE_UNMATCHED;
    }
    
    data.checkcode.nNum = get_len_offset(&srcBufTmp[offset], &offlen);
    offset += offlen;
    
    memcpy(&data.checkcode.value[0], &srcBufTmp[offset], data.checkcode.nNum); 
    offset += data.checkcode.nNum;

    memcpy(dstData, &data, dstLen);
    
    *pOffset = offset;
    
    return nRet;
}

/**
*********************************************************************
* @name：        basic_buf_to_data
* @brief：       从698报文格式中获取基本类型数据
* @param[in] ： dstType    目的数据类型
*               srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的数据长度,通常是变量的大小
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
*               dstLen     目的长度，通常是变量的长度
* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_data_from_fmt698(DATA_E eData, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_OTHER_REASON;
    uint32  sOffset = 0;
    uint32  dOffset = 0;
    uint8 *srcBuf = (uint8 *)convert->srcBuf;    
    uint8 *dstBuf = (uint8 *)convert->dstBuf;
    uint32 srcLen = 0;
    uint32 dstLen = 0;   
    uint32 dataLen = 0;

     /*入参检查*/
    if ((convert == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL)|| 
        (convert->sOffset == NULL) || (convert->dOffset == NULL))
    {
        PRTL_FMT_DEBUG("basic_data_from_fmt698 check failed, ERR_PTR\n");
        return ERR_PTR;
    }

    if(!basic_type_check(eData))
    {
        PRTL_FMT_DEBUG("basic_type_check failed. eData(%u)\n", eData);      
    }
    
    sOffset = *convert->sOffset;
    dOffset = *convert->dOffset;
    srcLen = convert->srcLen;
    dstLen = convert->dstLen;

    switch(eData)
    {
        case E_NULL:                            /**< NULL               */
        {
            if (srcBuf[sOffset++] != DT_NULL)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            dar = ERR_OK;
            break;
        }

        case E_ARRAY:                           /**< vector             */
            break;

        case E_STRUCTURE:                       /**< struct             */
            break;

        case E_BOOL:                            /**< BOOLEAN               */
        {
            if (srcLen < 2)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if ((dstLen - dOffset) < sizeof(uint8))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if (srcBuf[sOffset++] != DT_BOOL)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(uint8));
            sOffset += sizeof(uint8);
            dOffset += sizeof(uint8);

            dar = ERR_OK;
            break;
        }
            
        case E_BIT_STRING:                      /**< CBitString         */
            break;

        case E_OOP_BITSTR3_T:                   /**< CBitString         */
        {
            uint8 tmpLen = 0;
            if (srcLen < 2)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if ((dstLen - dOffset) < sizeof(uint8))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if (srcBuf[sOffset++] != DT_BIT_STRING)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }

            tmpLen = srcBuf[sOffset++];
            
            if ((tmpLen != 3) && (tmpLen != 8))
            {
                dar = DATA_DATA_BLOCK_UNAVAILABLE;
                break;
            }

            memcpy(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(uint8));
            //字节反转
            membitrev(&dstBuf[dOffset], 8);
            sOffset += sizeof(uint8);
            dOffset += sizeof(uint8);

            dar = ERR_OK;
            break;
        }

        case E_OOP_BITSTR8_T:                   /**< CBitString         */
        {
            if (srcLen < 2)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if ((dstLen - dOffset) < sizeof(uint8))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if (srcBuf[sOffset++] != DT_BIT_STRING)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            if (srcBuf[sOffset++] != 8)
            {
                dar = DATA_DATA_BLOCK_UNAVAILABLE;
                break;
            }

            memcpy(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(uint8));
			//字节反转
			membitrev(&dstBuf[dOffset], 8);
            sOffset += sizeof(uint8);
            dOffset += sizeof(uint8);

            dar = ERR_OK;
            break;
        }

        case E_OOP_BITSTR64_T:                   /**< CBitString         */
        {
            if (srcLen < 2)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if ((dstLen - dOffset) < 8*sizeof(uint8))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if (srcBuf[sOffset++] != DT_BIT_STRING)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            if (srcBuf[sOffset++] != 64)
            {
                dar = DATA_DATA_BLOCK_UNAVAILABLE;
                break;
            }

            memcpy(&dstBuf[dOffset], &srcBuf[sOffset], 8*sizeof(uint8));
			//字节反转
			membitrev(&dstBuf[dOffset], 64);
            sOffset += sizeof(uint8);
            dOffset += sizeof(uint8);

            dar = ERR_OK;
            break;
        }

        case E_OOP_BITSTR128_T:                   /**< CBitString         */
        {
            if (srcLen < 2)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if ((dstLen - dOffset) < 16*sizeof(uint8))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if (srcBuf[sOffset++] != DT_BIT_STRING)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            if (srcBuf[sOffset++] != 0x82)
            {
                dar = DATA_DATA_BLOCK_UNAVAILABLE;
                break;
            }
            if (srcBuf[sOffset++] != 0x00)
            {
                dar = DATA_DATA_BLOCK_UNAVAILABLE;
                break;
            }
            if (srcBuf[sOffset++] != 0x80)
            {
                dar = DATA_DATA_BLOCK_UNAVAILABLE;
                break;
            }
            memcpy(&dstBuf[dOffset], &srcBuf[sOffset], 16*sizeof(uint8));
			//字节反转
			membitrev(&dstBuf[dOffset], 128);
            sOffset += sizeof(uint8);
            dOffset += sizeof(uint8);

            dar = ERR_OK;
            break;
        }

        case E_DOUBLE_LONG:                     /**< int                */
        {
            if (srcLen < 5)
            {
                dar = DATA_OTHER_REASON;
                break;
            }

            if((dstLen - dOffset) < sizeof(int32))
            {
                dar = DATA_OTHER_REASON;
                break;
            }

            if (srcBuf[sOffset++] != DT_DOUBLE_LONG)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(int32));
            sOffset += sizeof(int32);
            dOffset += sizeof(int32);
            
            dar = ERR_OK;
            break;
        }

        case E_DOUBLE_LONG_UNSIGNED:            /**< uint32             */
        {
            if (srcLen < 5)
            {
                dar = DATA_OTHER_REASON;
                break;
            }

            if((dstLen - dOffset) < sizeof(uint32))
            {
                dar = DATA_OTHER_REASON;
                break;
            }

            if (srcBuf[sOffset++] != DT_DOUBLE_LONG_UNSIGNED)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(uint32));
            sOffset += sizeof(uint32);
            dOffset += sizeof(uint32);

            dar = ERR_OK;
            break;
        }

        case E_OCTET_STRING:                    /**< COctetString       */
        {
    		OOP_OCTETVAR2K_T data = {0};
		    uint8 offlen = 0;      //数组数量长度

		    MEMZERO(&data, sizeof(data));
			//COctetString类型
		    if(srcBuf[sOffset++] != DT_OCTET_STRING)
		    {
		        dar = DATA_OTHER_REASON;
                break;;
		    }

		    //COctetString长度
		    data.nNum = get_len_offset(&srcBuf[sOffset], &offlen);
		    sOffset += offlen;
			memcpy(data.value, &srcBuf[sOffset], data.nNum);
			
		    memcpy(&dstBuf[dOffset], &data, MIN(dstLen, sizeof(data)));
			sOffset += data.nNum;
    		dar = ERR_OK;
    		break;
    	}
        case E_VISIBLE_STRING:                  /**< string 1K          */
        {
            dataLen = sizeof(OOP_VISIBLEVAR1K_T);
            dar = basic_visblestrvar1k_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break; 
        }
            break;
        case E_OOP_VISIBLESTR4_T:               /**< string             */
        {
            dataLen = sizeof(OOP_VISIBLESTR4_T);
            dar = basic_visblestr32_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break; 
        }

        case E_OOP_VISIBLESTR6_T:               /**< string             */
        {
            dataLen = sizeof(OOP_VISIBLESTR6_T);
            dar = basic_visblestr32_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break; 
        }

        case E_OOP_VISIBLESTR8_T:               /**< string             */
        {
            dataLen = sizeof(OOP_VISIBLESTR8_T);
            dar = basic_visblestr32_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break; 
        }

        case E_OOP_VISIBLEVAR32_T:              /* visible-string(SIZE(32)) */
        {
            dataLen = sizeof(OOP_VISIBLEVAR32_T);
            dar = basic_visblestrvar1k_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break; 
        }

		case E_OOP_VISIBLESTR256_T:              /* visible-string(SIZE(32)) */
        {
            dataLen = sizeof(OOP_VISIBLEVAR256_T);
            dar = basic_visblestrvar1k_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break; 
        }

        case E_OOP_VISIBLEVAR_T:                /* octet-string(SIZE(64)) */
        {
            dataLen = sizeof(OOP_VISIBLEVAR_T);
            dar = basic_visblestrvar1k_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break; 
        }

        case E_UTF8_STRING:                     /**< string             */
            break;
        case E_INTEGER:                         /**< char               */
        {
            if (srcLen < 2)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if ((dstLen - dOffset) < sizeof(char))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if (srcBuf[sOffset++] != DT_INTEGER)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(char));
            sOffset += sizeof(char);
            dOffset += sizeof(char);

            dar = ERR_OK;
            break;
        }

        case E_LONG:                            /**< int16              */
        {
            if (srcLen < 3)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if ((dstLen - dOffset) < sizeof(int16))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if (srcBuf[sOffset++] != DT_LONG)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(int16));
            sOffset += sizeof(int16);
            dOffset += sizeof(int16);

            dar = ERR_OK;
            break;
        }

        case E_UNSIGNED:                        /**< uint8              */
        {
            if (srcLen < 2)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if ((dstLen - dOffset) < sizeof(uint8))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if (srcBuf[sOffset++] != DT_UNSIGNED)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(uint8));
            sOffset += sizeof(uint8);
            dOffset += sizeof(uint8);

            dar = ERR_OK;
            break;
        }

        case E_LONG_UNSIGNED:                   /**< uint16             */
        {
            if (srcLen < 3)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if ((dstLen - dOffset) < sizeof(uint16))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if (srcBuf[sOffset++] != DT_LONG_UNSIGNED)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(uint16));
            sOffset += sizeof(uint16);
            dOffset += sizeof(uint16);

            dar = ERR_OK;
            break;
        }

        case E_LONG64:                          /**< int64              */
        {
            if (srcLen < 9)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if ((dstLen - dOffset) < sizeof(int64))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if (srcBuf[sOffset++] != DT_LONG64)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(int64));
            sOffset += sizeof(int64);
            dOffset += sizeof(int64);

            dar = ERR_OK;
            break;
        }

        case E_LONG64_UNSIGNED:                 /**< uint64             */
        {
            if (srcLen < 9)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if ((dstLen - dOffset) < sizeof(uint64))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if (srcBuf[sOffset++] != DT_LONG64_UNSIGNED)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(uint64));
            sOffset += sizeof(uint64);
            dOffset += sizeof(uint64);

            dar = ERR_OK;
            break;

        }

        case E_ENUM:                            /**< uint8              */
        {
            dataLen = sizeof(uint8);
            if (srcLen < 2)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if ((dstLen - dOffset) < sizeof(uint8))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if (srcBuf[sOffset++] != DT_ENUM)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            sOffset += dataLen;
            dOffset += dataLen;

            dar = ERR_OK;
            break;
        }

        case E_FLOAT32:                         /**< float32            */
        {
            dataLen = sizeof(float32);
            if (srcLen < 5)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if ((dstLen - dOffset) < sizeof(float32))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if (srcBuf[sOffset++] != DT_FLOAT32)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            sOffset += dataLen;
            dOffset += dataLen;

            dar = ERR_OK;
            break;
        }

        case E_FLOAT64:                         /**< float64            */
        {
            dataLen = sizeof(float64);
            if (srcLen < 9)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if ((dstLen - dOffset) < sizeof(float64))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if (srcBuf[sOffset++] != DT_FLOAT64)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            sOffset += dataLen;
            dOffset += dataLen;

            dar = ERR_OK;
            break;
        }

        case E_DATETIME:                        /**< DateTime           */
            break;
        case E_DATE:                            /**< OOP_DATE_T         */
            break;
        case E_TIME:                            /**< OOP_TIME_T         */
        {
            dataLen = sizeof(OOP_TIME_T);
            dar = basic_time_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break;
        }
            
        case E_DATETIME_S:                      /**< DateTime_S         */  
            dataLen = sizeof(OOP_DATETIME_S_T);
            dar = basic_datetime_s_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;                
            break;
        case E_OI:                              /**< OI                 */
        {
            dataLen = sizeof(OI);
            
            if (srcLen < sizeof(OI) + 1)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if ((dstLen - dOffset) < sizeof(OI))
            {
                dar = DATA_OTHER_REASON;
                break;
            }

            if (srcBuf[sOffset++] != DT_OI)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            sOffset += dataLen;
            dOffset += dataLen;

            dar = ERR_OK;
            break;
        }
            
        case E_OAD:                             /**< OOP_OAD_U          */
        {
            dataLen = sizeof(OOP_OAD_U);
            
            if (srcLen < sizeof(OOP_OAD_U) + 1)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if ((dstLen - dOffset) < sizeof(OOP_OAD_U))
            {
                dar = DATA_OTHER_REASON;
                break;
            }

            if (srcBuf[sOffset++] != DT_OAD)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            sOffset += dataLen;
            dOffset += dataLen;

            dar = ERR_OK;
            break;
        }
            
        case E_ROAD:                            /**< OOP_ROAD_T         */
        {
            OOP_ROAD_T road;
            uint8 i,num;
            memset(&road,0x00,sizeof(road));
            if (srcBuf[sOffset++] != DT_ROAD)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            memcpy_r(&road.oadMain.value,&srcBuf[sOffset], 4);
            sOffset+=4;
            num = srcBuf[sOffset++];
            road.oadCols.nNum = num;
            for(i=0;i<num;i++)
            {
                memcpy_r(&road.oadCols.oad[i].value,&srcBuf[sOffset], 4);
                sOffset+=4;
            }
            memcpy(&dstBuf[dOffset],&road,sizeof(road));
            dOffset+=sizeof(road);
            dar = ERR_OK;
        }
            break;
        case E_OMD:                             /**< OOP_OMD_U          */
            break;
        case E_TI:                              /**< 时间间隔           */
            dataLen = sizeof(OOP_TI_T);
            dar = basic_ti_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;                               
            break;
        case E_TSA:                             /**< 设备地址           */
        {
            dataLen = sizeof(OOP_TSA_T);
            dar = basic_tsa_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break;  
        }

        case E_OOP_OCTETVAR16_T:                /* octet-string(SIZE(16)) */
        {
            dataLen = sizeof(OOP_OCTETVAR16_T);
            dar = basic_octetstrvar2k_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break; 
        }

        case E_OCTET_STR2_T:                  /* octet-string(SIZE(2)) */
        {
            dataLen = 4;
            dar = basic_octetstr32_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break; 
        }

        case E_OCTET_STR3_T:                  /* octet-string(SIZE(3)) */
        {
            dataLen = 3;
            dar = basic_octetstr32_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break; 
        }

        case E_OCTET_STR4_T:                  /* octet-string(SIZE(4)) */
        {
            dataLen = 4;
            dar = basic_octetstr32_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break; 
        }

        case E_OCTET_STR32_T:                  /* octet-string(SIZE(32)) */
        {
            dataLen = sizeof(OOP_OCTETSTR32_T);
            dar = basic_octetstr32_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break; 
        }

		case E_OOP_OCTETVAR64_T:                /* octet-string(SIZE(64)) */
        {
            dataLen = sizeof(OOP_OCTETVAR64_T);
            dar = basic_octetstrvar2k_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break;
        }

        case E_OOP_OCTETVAR256_T:                /* octet-string(SIZE(256)) */
        {
            dataLen = sizeof(OOP_OCTETVAR256_T);
            dar = basic_octetstrvar2k_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break; 
        }

        case E_OCTET_STRING_2K:                /* octet-string(SIZE(2K)) */
        case E_OOP_OCTETVAR2K_T:
        {
            dataLen = sizeof(OOP_OCTETVAR2K_T);
            dar = basic_octetstrvar2k_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break; 
        }

        case E_OOP_SA_T:
        {
            dataLen = sizeof(OOP_SA_T);
            dar = basic_sa_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break;
        }
            
        case E_MAC:                             /**< OOP_MAC_T签名        */
        {
            dataLen = sizeof(OOP_MAC_T);
            dar = basic_mac_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break; 
        }

        case E_COMDCB:
        {
            dataLen = sizeof(OOP_COMDCB_T);
            dar = basic_comdcb_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break;
        }

        case E_RN:                              /**< 随机数              */
            break;
        case E_REGION:                          /**< 区间类型           */
            dataLen = sizeof(OOP_REGIOND_T);
            dar = basic_region_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break;  

        case E_SCALER_UNIT:                     /**< 换算单位           */
        {
            dataLen = sizeof(OOP_SCALER_UNIT_T);
            dar = basic_scaler_unit_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break;
        }

        case E_RSD:                             /**< 行选择符           */
        {     
            dataLen = sizeof(OOP_PRSD_T);
            dar = basic_rsd_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break;  
        }

        case E_CSD:                             /**< 列选择符           */
        {     
            dataLen = sizeof(OOP_CSD_T);
            dar = basic_csd_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break;  
        }

        case E_MS:                              /**< 电能表集合            */
        {
            dataLen = sizeof(OOP_MSP_T);
            dar = basic_ms_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break;  
        }
        #ifdef AREA_FUJIAN
        case E_MS_F:                              /**< 电能表集合            */
        {
            dataLen = sizeof(OOP_MSPF_T);
            dar = basic_ms_fromfmt698_fj(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break;  
        }
        #endif
        case E_SID:                             /**< 安全标识           */
            break;
        case E_SIDMAC:                          /**< 安全标识+OOP_MAC_T */
            break;
        case E_RCSD:                            /**< 记录列选择描述符   */
        {     
            dataLen = sizeof(OOP_PCSD_T); //参数用PCSD结构体，报文用RCSD结构体，容纳CSD的数量不同
            dar = basic_rcsd_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break;  
        }
        case E_CHECKSUM:                         /**< 24规范校验码类型 */
        {
            dataLen = sizeof(OOP_CHECKSUM_T);
            dar = basic_checksum_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            break;
        }
        default:
            dar = DATA_OTHER_REASON;
            break;
    }

    //报文中的数据类型为空数据
    if (dar != DATA_SUCCESS)
    {
        sOffset = *convert->sOffset;
        dOffset = *convert->dOffset;
    
        if (srcBuf[sOffset++] != DT_NULL)
        {
            dar = DATA_TYPE_UNMATCHED;
        }
        else
        {
            dar = DATA_SUCCESS;
        }
    }

    *convert->sOffset = sOffset;
    *convert->dOffset = dOffset;

    return dar;
}

/**
*********************************************************************
* @brief：      698规约格式转数据中心格式
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
* @author    : 贺 宁
* @Date      ：2019-12-6               
*********************************************************************
*/
OOP_DAR_E basic_db_from_fmt698(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    DATA_E eData = 0;

     /*入参检查*/
    if ((pOadInfo == NULL) || (convert == NULL) || (convert->srcBuf == NULL) || 
        (convert->dstBuf == NULL)|| (convert->sOffset == NULL) || (convert->dOffset == NULL))
    {
        PRTL_FMT_DEBUG("buf_to_data_check failed, ERR_PTR\n");
        return ERR_PTR;
    }

    eData = pOadInfo->pTab->eData;
    if(!basic_type_check(eData))
    {
        PRTL_FMT_DEBUG("basic_type_check failed. eData(%u)\n", eData);      
    }

    return basic_data_from_fmt698(eData, convert);
}

/**
*********************************************************************
* @name：        basic_buf_to_data
* @brief：       从698报文格式中获取基本类型数据(用于目的数据区为变量的场景)
* @param[in] ： dstType    目的数据类型
*               srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的数据长度,通常是变量的大小
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
*               dstLen     目的长度，通常是变量的长度
* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_buf_to_data(DATA_E eData, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 dOffset = 0;

    /*入参检查*/
    if ((srcBuf == NULL) || (dstData == NULL) || (pOffset == NULL) || (!basic_type_check(eData)))
    {
        PRTL_FMT_DEBUG("basic_buf_to_data check failed. ERR_PTR\n");
        return DATA_OTHER_REASON;
    }

    MEMZERO(&convert, sizeof(convert));
   
    convert.srcBuf = (void *)srcBuf;
    convert.srcLen = srcLen;
    convert.sOffset = pOffset;

    convert.dstBuf = dstData;
    convert.dstLen = dstLen;
    convert.dOffset = &dOffset;

    dar = basic_data_from_fmt698(eData, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_from_fmt698 failed. eData(%u)\n", eData);
        return dar;
    }

    if(dOffset > dstLen)
    {
        PRTL_FMT_DEBUG("dstData cross the border. dOffset(%d), dstLen(%d)\n", dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    return dar;
}

#endif

#if DESC("DIGIT类型解析",1)

/**
*********************************************************************
* @name：        digit_from_fmt698
* @brief：       从698规约格式中解析数值类型
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E digit_from_fmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_DIGIT_T digit; 
    
    MEMZERO(&digit, sizeof(digit));

    digit.choice = srcBufTmp[offset++];  
    
    switch(digit.choice)
    {  
        case DT_DOUBLE_LONG: //double long/int32
        {
            memcpy_r(&digit.nVal, &srcBufTmp[offset], sizeof(int32));    
            offset += sizeof(int32);            
        }break;
        case DT_DOUBLE_LONG_UNSIGNED: //double long unsigned/uint32
        {
            memcpy_r(&digit.dwVal, &srcBufTmp[offset], sizeof(uint32));   
            offset += sizeof(uint32);            
        }break;     
        case DT_INTEGER: //integer/char
        {
            memcpy_r(&digit.cVal, &srcBufTmp[offset], sizeof(char)); 
            offset += sizeof(char);
        }break;
        case DT_LONG: //long/int16
        {
            memcpy_r(&digit.lVal, &srcBufTmp[offset], sizeof(int16)); 
            offset += sizeof(int16);
        }break; 
        case DT_UNSIGNED: //unsigned/uint8
        {
            memcpy_r(&digit.bVal, &srcBufTmp[offset], sizeof(uint8)); 
            offset += sizeof(uint8);           
        }break;   
        case DT_LONG_UNSIGNED: //long unsigned/uint16
        {
            memcpy_r(&digit.wVal, &srcBufTmp[offset], sizeof(uint16)); 
            offset += sizeof(uint16);        
        }break;   
        case DT_LONG64: //long64/int64
        {
            memcpy_r(&digit.llVal, &srcBufTmp[offset], sizeof(int64)); 
            offset += sizeof(int64);                      
        }break;   
        case DT_LONG64_UNSIGNED: //long64 unsigned/uint64
        {
            memcpy_r(&digit.ullVal, &srcBufTmp[offset], sizeof(uint64)); 
            offset += sizeof(uint64);                               
        }break;
        default:
        {
            PRTL_FMT_DEBUG("not support this digit. choice(%d)\n", digit.choice);
        }break;
    }

    memcpy(dstData, &digit, sizeof(digit));

    *pOffset = offset;

    return dar;     

}

/**
*********************************************************************
* @name：        digit_to_fmt698
* @brief：       从698规约格式中解析数值类型
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E digit_to_fmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_DIGIT_T digit; 
    
    memcpy(&digit, srcData, sizeof(digit));

    dstBufTmp[offset++] = digit.choice;
    
    switch(digit.choice)
    {  
        case DT_DOUBLE_LONG: //double long/int32
        {
            memcpy_r(&dstBufTmp[offset], &digit.nVal, sizeof(int32));    
            offset += sizeof(int32);            
        }break;
        case DT_DOUBLE_LONG_UNSIGNED: //double long unsigned/uint32
        {
            memcpy_r(&dstBufTmp[offset], &digit.dwVal, sizeof(uint32));   
            offset += sizeof(uint32);            
        }break;     
        case DT_INTEGER: //integer/char
        {
            memcpy_r(&dstBufTmp[offset], &digit.cVal, sizeof(char)); 
            offset += sizeof(char);
        }break;
        case DT_LONG: //long/int16
        {
            memcpy_r(&dstBufTmp[offset], &digit.lVal, sizeof(int16)); 
            offset += sizeof(int16);
        }break; 
        case DT_UNSIGNED: //unsigned/uint8
        {
            memcpy_r(&dstBufTmp[offset], &digit.bVal, sizeof(uint8)); 
            offset += sizeof(uint8);           
        }break;   
        case DT_LONG_UNSIGNED: //long unsigned/uint16
        {
            memcpy_r(&dstBufTmp[offset], &digit.wVal, sizeof(uint16)); 
            offset += sizeof(uint16);        
        }break;   
        case DT_LONG64: //long64/int64
        {
            memcpy_r(&dstBufTmp[offset], &digit.llVal, sizeof(int64)); 
            offset += sizeof(int64);                      
        }break;   
        case DT_LONG64_UNSIGNED: //long64 unsigned/uint64
        {
            memcpy_r(&dstBufTmp[offset], &digit.ullVal, sizeof(uint64)); 
            offset += sizeof(uint64);                               
        }break;
        default:
        {
            dar = DATA_TYPE_UNMATCHED;
            PRTL_FMT_DEBUG("not support this digit. choice(%d)\n", digit.choice);
        }break;
    }

    *pOffset = offset;

    return dar;     
}

#endif

//
// 获取电表集合的几个函数、RSD和RCSD解析、APDU检查、任务记录表断点检查
//
#if DESC("获取电表集合的几个函数、RSD和RCSD解析、APDU检查、任务记录表断点检查",1)
/**
*********************************************************************
* @brief：     比较2个地址是否一致,MtrAddr1可能带通配符
* @param[in]： MtrAddr1     地址1
               MtrAddr2     地址2
* @return：    BOOL
*********************************************************************
*/
BOOL is_mtr_addr_match(uint8* MtrAddr1, uint8* MtrAddr2, uint8 Length)        
{
    int       i = 0;
    char      str1[3] = {0};
    char      str2[3] = {0};
    uint8     tmpbuff[6] = {0};
    OOP_TSA_T oopTsa;
    uint8     addlen = 0;
    OOP_TSA_T tsa1;
    uint8 index = 0;
    MEMZERO(&tsa1,sizeof(OOP_TSA_T));
    MEMZERO(&oopTsa, sizeof(OOP_TSA_T));
    memcpy(&oopTsa, MtrAddr2, sizeof(OOP_TSA_T));
    tsa1.af = MtrAddr1[index++];
    if(tsa1.flag == 1)
    {
        tsa1.vxd = MtrAddr1[index++];
        memcpy(tsa1.add,&MtrAddr1[index],tsa1.len);
        
    }else
    {
        memcpy(tsa1.add,&MtrAddr1[index],tsa1.len+1);
    }

    if(tsa1.oldvxd != oopTsa.oldvxd || tsa1.vxd!=oopTsa.vxd)
        return FALSE;
    
    if(Length > 8)
        return FALSE;

    if((oopTsa.len + 2) != Length)
    {
        return FALSE;
    }

    //逻辑地址扩展区分
    if (oopTsa.flag == 0x01)
    {
        memcpy(tmpbuff, oopTsa.add, oopTsa.len);
        if (memcmp(tsa1.add, tmpbuff, oopTsa.len) == 0 && tsa1.vxd == oopTsa.vxd)
        {
            return TRUE;
        }

        addlen = oopTsa.len;
    }
    else
    {
        memcpy(tmpbuff, oopTsa.add, oopTsa.len + 1);
        if(memcmp(tsa1.add, tmpbuff, oopTsa.len + 1) == 0 && tsa1.oldvxd == oopTsa.oldvxd)
        {
            return TRUE;
        }

        addlen = oopTsa.len + 1;
    }
    
    for (i = 0; i < addlen ; i++)
    {
        if(tsa1.add[i] != oopTsa.add[i ])          //判定有通配符A的情况
        {
            sprintf(str1, "%02x", tsa1.add[i]);
            sprintf(str2, "%02x", oopTsa.add[i]);
            if((str1[0] == 'a') || (str1[0] == 'A'))
            {
                str1[0] = str2[0];
            }

            if((str1[1] == 'a') || (str1[1] == 'A'))
            {
                str1[1] = str2[1];
            }

            if(memcmp(str1, str2, 2) != 0)
                return FALSE;
        }
    }
    return TRUE;
}

BOOL IsMtrAddrInRegion(uint64 StartAddr, uint64 EndAddr, uint8 MatchLen, uint8* pAddr, uint8 AddrLen, uint8 RegionType, BOOL IsF)
{
    uint64 val = 0;
    OOP_TSA_T oopTsa;

    MEMZERO(&oopTsa, sizeof(OOP_TSA_T));
    memcpy(&oopTsa, pAddr, sizeof(OOP_TSA_T));

    if(MatchLen != oopTsa.len + 1)
        return FALSE;

    //逻辑地址扩展区分
    if (oopTsa.flag == 0x01)
    {
        if((((oopTsa.add[0] & 0x0f) == 0x0f) && (!IsF))
                ||(((oopTsa.add[0] & 0x0f) != 0x0f) && IsF))
            return FALSE;
        memcpy_r(&val, &oopTsa.add[0], oopTsa.len);
    }
    else
    {
        if((((oopTsa.add[0] & 0x0f) == 0x0f) && (!IsF))
                ||(((oopTsa.add[0] & 0x0f) != 0x0f) && IsF))
            return FALSE;
        memcpy_r(&val, &oopTsa.add[0], oopTsa.len + 1);
    }

    if(RegionType == 0)
    {
        if((val >= StartAddr) && (val < EndAddr))
            return TRUE;
        else
            return FALSE;
    }
    else if(RegionType == 1)
    {
        if((val > StartAddr) && (val <= EndAddr))
            return TRUE;
        else
            return FALSE;
    }
    else if(RegionType == 2)
    {
        if((val >= StartAddr) && (val <= EndAddr))
            return TRUE;
        else
            return FALSE;
    }
    else
    {
        if((val > StartAddr) && (val < EndAddr))
            return TRUE;
        else
            return FALSE;
    }
}

//合并重组区间
void MergeMtrTypeRegion(uint8* MtrType, uint8* Length, uint8 StartType, uint8 EndType)
{
    uint8  i = 0, j;
    uint8* pBuf = NULL;
    uint8  len = 0;
    if(*Length == 0)
    {
        for(i = StartType; i <= EndType; i++)
        {
            MtrType[(*Length)++] = i;
        }
    }
    else
    {
        j = StartType;
        pBuf = malloc(*Length);
        memcpy(pBuf, MtrType, *Length);
        while((i < *Length) && (j <= EndType))
        {
            if(pBuf[i] < j)
                MtrType[len++] = pBuf[i++];
            else if(pBuf[i] > j)
                MtrType[len++] = j++;
            else
            {
                MtrType[len++] = j;
                i++;j++;
            }
        }
        while(i < *Length)
        {
            MtrType[len++] = pBuf[i++];
        }
        while(j <= EndType)
        {
            MtrType[len++] = j++;
        }
        *Length = len;
        free(pBuf);
        pBuf = NULL;
    }
    return;
}

uint8 GetMtrTypeRegion(const uint8 *inData, uint16* offset, uint8* MtrType)
{
    uint8 i = 0;
    uint8 len = 0;
    uint8 RegionType = 0;
    uint8 StartType, EndType;
    uint8 RegionNum = inData[(*offset)++];
    for(i = 0; i < RegionNum; i++)
    {
        RegionType = inData[(*offset)++];
        if(inData[(*offset)++] != UNSIGNED)
            return 0;
        switch(RegionType)
        {
        case 0:
            StartType = inData[(*offset)++];
            if(inData[(*offset)++] != UNSIGNED)
                return 0;
            EndType= inData[(*offset)++] - 1;
            if(StartType > EndType)     return 0;
            break;
        case 1:
            StartType = inData[(*offset)++] + 1;
            if(inData[(*offset)++] != UNSIGNED)
                return 0;
            EndType = inData[(*offset)++];
            if(StartType > EndType)     return 0;
            break;
        case 2:
            StartType = inData[(*offset)++];
            if(inData[(*offset)++] != UNSIGNED)
                return 0;
            EndType = inData[(*offset)++];
            if(StartType > EndType)     return 0;
            break;
        case 3:
            StartType = inData[(*offset)++] + 1;
            if(inData[(*offset)++] != UNSIGNED)
                return 0;
            EndType = inData[(*offset)++] - 1;
            if(StartType > EndType)     return 0;
            break;
        default:
            return 0;
        }
        MergeMtrTypeRegion(MtrType, &len, StartType, EndType);
    }
    return len;
}

void MergeMetSnRegion(uint16* MetSn, uint16* MetNum, uint16 StartSn, uint16 EndSn)
{
    uint16 tmpNum = *MetNum;

    if(StartSn > EndSn)	return;

    if(tmpNum == 0)
    {
        while(StartSn <= EndSn)
        {
            MetSn[tmpNum++] = StartSn++;
        }
        *MetNum = tmpNum;
    }
    else
    {
        if(StartSn <= MetSn[0])
        {
            if(EndSn >= MetSn[tmpNum-1])
            {
                tmpNum = 0;
                while(StartSn <= EndSn)
                {
                    MetSn[tmpNum++] = StartSn++;
                }
                *MetNum = tmpNum;
            }
            else if(EndSn >= MetSn[0])
            {
                EndSn = MetSn[tmpNum-1];
                tmpNum = 0;
                while(StartSn <= EndSn)
                {
                    MetSn[tmpNum++] = StartSn++;
                }
                *MetNum = tmpNum;
            }
            else
            {
                memmove(MetSn+EndSn-StartSn+1, MetSn, tmpNum*2);
                (*MetNum) += (EndSn-StartSn+1);
                while(StartSn <= EndSn)
                {
                    *(MetSn++) = StartSn++;
                }
            }
        }
        else if(StartSn <= MetSn[tmpNum-1])
        {
            if(EndSn > MetSn[tmpNum-1])
            {
                StartSn = MetSn[tmpNum-1] + 1;
                while(StartSn <= EndSn)
                {
                    MetSn[tmpNum++] = StartSn++;
                }
                *MetNum = tmpNum;
            }
        }
        else
        {
            while(StartSn <= EndSn)
            {
                MetSn[tmpNum++] = StartSn++;
            }
            *MetNum = tmpNum;
        }
    }
    return;
}


BOOL GetMtrAddrRegion(const uint8* inData, uint16* offset, uint8 RegionNum, uint64* StartAddr, uint64* EndAddr, uint8* RegionType, BOOL* IsF, uint8* AddrLen)
{
    uint8  i = 0, lenTSA1 = 0, lenTSA2 = 0;
    uint8  LastByte1 = 0, LastByte2 = 0;
    PRTL_BUF_TRACE(&inData[*offset], 15, "ms[6] buf:");
    for(i = 0; i < RegionNum; i++)
    {
        RegionType[i] = inData[(*offset)++];
        if(RegionType[i] > 3)
            return FALSE;

        if(inData[(*offset)++] != eTSA)
        {
            return FALSE;
        }

        lenTSA1 = inData[(*offset)++];
        if(lenTSA1 > 7)	return FALSE;
        (*offset)++;
        memcpy_r((uint8 *)&StartAddr[i], &inData[*offset], lenTSA1-1);
        *offset += (lenTSA1-1);
        IsF[i] = FALSE;
        LastByte1 = inData[(*offset)-1] & 0x0f;
        if(LastByte1 == 0x0f)
        {
            IsF[i] = TRUE;
        }
        AddrLen[i] = lenTSA1-1;

        if(inData[(*offset)++] != eTSA)
            return FALSE;
        lenTSA2 = inData[(*offset)++];
        if(lenTSA2 > 7)	return FALSE;
        (*offset)++;
        memcpy_r((uint8 *)&EndAddr[i], &inData[*offset], lenTSA2-1);
        *offset += (lenTSA2-1);
        LastByte2 = inData[(*offset)-1] & 0x0f;
        if((lenTSA1 != lenTSA2) || ((LastByte1 == 0x0f) && (LastByte2 != 0x0f))
                ||((LastByte1 != 0x0f) && (LastByte2 == 0x0f)))
            return FALSE;
    }
    return TRUE;
}

/**
*********************************************************************
* @brief：     获取MS
* @param[in]： inData     ms集合链表头
               offset     偏移
* @return：    ms集合链表头
*********************************************************************
*/
MS_T* mtr_list_get(DB_CLIENT hUdp, const uint8 *inData, uint16* offset, BOOL isWithType)
{
    int32   ret = ERR_NORMAL;
    int		i = 0;
    uint8	j = 0;
    uint16	k = 0;
    uint8	Choice = 0;
    MS_T*   pMtrHead = NULL;
    MS_T*   pMtrTail = pMtrHead;
    MS_T*   pNewMtr = pMtrHead;
    OOP_METER_T oopMeter  = {0};

    if(isWithType)
    {
        if(inData[*offset] != 0x5c)
            return NULL;
        (*offset)++;
    }
    Choice = inData[(*offset)++];
    if(0 == Choice)
    {
        return NULL;
    }
    else
    {
        pNewMtr = pMtrHead;
        pMtrTail = pMtrHead;
        switch (Choice)
        {
        case 1:
            //全部用户地址
            for(i = 1; i < NO_VIR_MTR_MAX_NUM + 1; i++)
            {
                ret = app_get_meter_list(i, &oopMeter);
                if (ret == ERR_OK)
                {
                    pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                    memset(pNewMtr, 0, sizeof(MS_T));
                    pNewMtr->oopMeter = oopMeter;
                    pNewMtr->MtrNo    = i;
                    pNewMtr->next = NULL;
                    if(!pMtrHead)
                        pMtrHead = pNewMtr;
                    if(pMtrTail)
                        pMtrTail->next = pNewMtr;
                    pMtrTail = pNewMtr;
                }
                else
                {
                    //PRTL_FMT_DEBUG("mtr_list_get db_read_nomal err i = %d\n", i);
                }
            }
            break;

        case 2://一组用户类型
        {
            uint8*	MtrTypeList = NULL;
            uint8	TypeNum = inData[(*offset)++];
            MtrTypeList = (uint8 *)malloc(TypeNum);
            for(j = 0; j < TypeNum; j++)
            {
                MtrTypeList[j] = inData[(*offset)++];
            }
            for(i = 1; i < NO_VIR_MTR_MAX_NUM + 1; i++)
            {
                ret = app_get_meter_list(i, &oopMeter);
                if (ret == ERR_OK)
                {
                    for(j = 0; j < TypeNum; j++)
                    {
                        if(oopMeter.basic.userType == MtrTypeList[j])
                        {
                            pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                            memset(pNewMtr, 0, sizeof(MS_T));
                            pNewMtr->oopMeter = oopMeter;
                            pNewMtr->MtrNo    = i;
                            pNewMtr->next = NULL;
                            if(!pMtrHead)
                                pMtrHead = pNewMtr;
                            if(pMtrTail)
                                pMtrTail->next = pNewMtr;
                            pMtrTail = pNewMtr;
                            break;
                        }
                    }
                }
                else
                {
                    //PRTL_FMT_DEBUG("mtr_list_get db_read_nomal err i = %d\n", i);
                }
            }
            free(MtrTypeList);
            MtrTypeList = NULL;
        }
        break;
        case 3:
            //一组用户地址
        {
            uint8  mtrAddr1[17] = {0};
            uint8  mtrAddrLen = 0;
            uint8  mtrAddrNum = inData[(*offset)++];
            for(j = 0; j < mtrAddrNum; j++)
            {
                mtrAddrLen = inData[(*offset)++];
                memcpy(mtrAddr1, &inData[*offset], mtrAddrLen);
                for(i = 1; i < NO_VIR_MTR_MAX_NUM + 1; i++)
                {
                    ret = app_get_meter_list(i, &oopMeter);
                    if (ret == ERR_OK)
                    {
                        if (is_mtr_addr_match((uint8*)&mtrAddr1, (uint8*)&oopMeter.basic.tsa, mtrAddrLen))
                        {
                            pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                            memset(pNewMtr, 0, sizeof(MS_T));
                            pNewMtr->oopMeter = oopMeter;
                            pNewMtr->MtrNo    = i;
                            pNewMtr->next = NULL;
                            if(!pMtrHead)
                                pMtrHead = pNewMtr;
                            if(pMtrTail)
                                pMtrTail->next = pNewMtr;
                            pMtrTail = pNewMtr;
                            break;
                        }
                    }
                    else
                    {
                        //PRTL_FMT_DEBUG("mtr_list_get db_read_nomal err i = %d\n", i);
                    }
                }
                (*offset) += mtrAddrLen;
            }
        }
        break;
        case 4:
            //一组用户序号
        {
            uint8	MtrNoCnt = inData[(*offset)++];
            int		No = 1;

            if (MtrNoCnt == 0x81)
            {
                MtrNoCnt = inData[(*offset)++];
            }
            else if (MtrNoCnt == 0x82)
            {
                MtrNoCnt = inData[(*offset)++];
                MtrNoCnt <<= 8;
                MtrNoCnt += inData[(*offset)++];
            }

            for(i = 0; i < MtrNoCnt; i++)
            {
                No  = (uint16)(inData[*offset]<<8) + (uint16)(inData[*offset + 1]);
                ret = app_get_meter_list(No, &oopMeter);
                if (ret == ERR_OK)
                {
                    pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                    memset(pNewMtr, 0, sizeof(MS_T));
                    pNewMtr->oopMeter = oopMeter;
                    pNewMtr->MtrNo    = No;
                    pNewMtr->next = NULL;
                    if(!pMtrHead)
                        pMtrHead = pNewMtr;
                    if(pMtrTail)
                        pMtrTail->next = pNewMtr;
                    pMtrTail = pNewMtr;
                }
                else
                {
                    //PRTL_FMT_DEBUG("mtr_list_get db_read_nomal err i = %d\n", i);
                }
                (*offset) += 2;
            }
        }
        break;
        case 5:
            //一组用户类型区间
        {
            uint8	MtrType[256] = {0};
            uint8	UserType = 0, typeNum = 0;
            typeNum = GetMtrTypeRegion(inData, offset, MtrType);
            for(i = 1; i < NO_VIR_MTR_MAX_NUM + 1; i++)
            {
                ret = app_get_meter_list(i, &oopMeter);
                if (ret == ERR_OK)
                {
                    UserType = oopMeter.basic.userType;
                    for(j = 0; j < typeNum; j++)
                    {
                        if(UserType == MtrType[j])
                        {
                            pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                            memset(pNewMtr, 0, sizeof(MS_T));
                            pNewMtr->oopMeter = oopMeter;
                            pNewMtr->MtrNo    = i;
                            pNewMtr->next = NULL;
                            if(!pMtrHead)
                                pMtrHead = pNewMtr;
                            if(pMtrTail)
                                pMtrTail->next = pNewMtr;
                            pMtrTail = pNewMtr;
                            break;
                        }
                    }
                }
                else
                {
                    //PRTL_FMT_DEBUG("mtr_list_get db_read_nomal err i = %d\n", i);
                }
            }
        }
        break;
        case 6:
            //一组用户地址区间
        {
            uint8  RegionNum = 0;
            uint8  RegionTypeList[256] = {0};
            uint8  AddrLen[256] = {0};
            uint64 *StartAddr = NULL, *EndAddr = NULL;
            BOOL   ContainF[256] = {0};
            RegionNum = inData[(*offset)++];
            StartAddr = malloc(sizeof(uint64)*RegionNum);
            EndAddr = malloc(sizeof(uint64)*RegionNum);
            memset(StartAddr, 0, sizeof(uint64)*RegionNum);
            memset(EndAddr, 0, sizeof(uint64)*RegionNum);
            if(GetMtrAddrRegion(inData, offset, RegionNum, StartAddr, EndAddr, RegionTypeList, ContainF, AddrLen))
            {
                for(i = 1; i < NO_VIR_MTR_MAX_NUM + 1; i++)
                {
                    ret = app_get_meter_list(i, &oopMeter);
                    if (ret == ERR_OK)
                    {
                        for(j = 0; j < RegionNum; j++)
                        {
                            if(IsMtrAddrInRegion(StartAddr[j], EndAddr[j], AddrLen[j], (uint8*)&oopMeter.basic.tsa, oopMeter.basic.tsa.len + 1, RegionTypeList[j], ContainF[j]))
                            {
                                pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                                memset(pNewMtr, 0, sizeof(MS_T));
                                pNewMtr->oopMeter = oopMeter;
                                pNewMtr->MtrNo    = i;
                                pNewMtr->next = NULL;
                                if(!pMtrHead)
                                    pMtrHead = pNewMtr;
                                if(pMtrTail)
                                    pMtrTail->next = pNewMtr;
                                pMtrTail = pNewMtr;
                                break;
                            }
                        }
                    }
                    else
                    {
                        //PRTL_FMT_DEBUG("mtr_list_get db_read_nomal err i = %d\n", i);
                    }
                }
            }
            free(StartAddr);
            free(EndAddr);
        }
        break;
        case 7:
            //一组序号区间
        {
            uint16 StartNo = 1, EndNo = 1;
            uint16 MetNum = 0;
            uint16 *Pn = malloc(2*NO_VIR_MTR_MAX_NUM);
            uint8  RegionNum = inData[(*offset)++];
            uint8  RegionType = 0;
            for(i = 0; i < RegionNum; i++)
            {
                RegionType = inData[(*offset)++];
                if(inData[(*offset)++] != LONG_UNSIGNED)
                {
                    free(Pn);
                    Pn = NULL;
                    return NULL;
                }
                memcpy_r(&StartNo, &inData[*offset], sizeof(uint16));
                (*offset) += 2;
                if(inData[(*offset)++] != LONG_UNSIGNED)
                {
                    free(Pn);
                    Pn = NULL;
                    return NULL;
                }
                memcpy_r(&EndNo, &inData[*offset], sizeof(uint16));
                (*offset) += 2;
                if(RegionType == 0 || RegionType == 3)
                    EndNo--;
                if(RegionType == 1 || RegionType == 3)
                    StartNo++;
                MergeMetSnRegion(Pn, &MetNum, StartNo, EndNo);
            }

            for(k = 0; k < MetNum; k++)
            {
                if(Pn[k] >=10000)
                {
                    
                    ret = GetVirtualPipePn(Pn[k], &oopMeter);
                    PRTL_FMT_DEBUG("感知终端虚拟通道 Pn[k] = %d,ret is %d\n", Pn[k],ret);
                }
                else
                {
                    ret = app_get_meter_list(Pn[k], &oopMeter);
                }
                if (ret == ERR_OK)
                {
                    pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                    memset(pNewMtr, 0, sizeof(MS_T));
                    pNewMtr->oopMeter = oopMeter;
                    pNewMtr->MtrNo    = Pn[k];
                    pNewMtr->next = NULL;
                    if(!pMtrHead)
                        pMtrHead = pNewMtr;
                    if(pMtrTail)
                        pMtrTail->next = pNewMtr;
                    pMtrTail = pNewMtr;
                }
                else
                {
                    //PRTL_FMT_DEBUG("mtr_list_get db_read_nomal err i = %d\n", i);
                }
            }
            free(Pn);
            Pn = NULL;
        }
        break;

        //一组设备类型
        case 8:
        {
            uint16*	MtrTypeList = NULL;
            uint8	TypeNum = inData[(*offset)++];
            uint16  type = 0;
            MtrTypeList = (uint16 *)malloc(2*TypeNum);
            for (j = 0; j < TypeNum; j++)
            {
                MtrTypeList[j] = (uint16)(inData[*offset]<<8) + (uint16)(inData[*offset + 1]);
                (*offset) += 2;
            }
            for(i = 1; i < NO_VIR_MTR_MAX_NUM; i++)
            {
                ret = app_get_meter_list(i, &oopMeter);
                if (ret == ERR_OK)
                {
                    for(j = 0; j < TypeNum; j++)
                    {
                        if (oopMeter.annex.nNum == 0)
                        {
                            break;
                        }
                        for (k = 0; k < oopMeter.annex.nNum; k++)
                        {
                            if (oopMeter.annex.annexObj[k].nLen != sizeof(uint16) || oopMeter.annex.annexObj[k].oad.value != 0x6E010200)
                            {
                                continue;
                            }
                            memcpy(&type, oopMeter.annex.annexObj[k].value, sizeof(uint16));
                            if (type == MtrTypeList[j])
                            {
                                pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                                memset(pNewMtr, 0, sizeof(MS_T));
                                pNewMtr->oopMeter = oopMeter;
                                pNewMtr->MtrNo    = i;
                                pNewMtr->next = NULL;
                                if(!pMtrHead)
                                    pMtrHead = pNewMtr;
                                if(pMtrTail)
                                    pMtrTail->next = pNewMtr;
                                pMtrTail = pNewMtr;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    //PRTL_FMT_DEBUG("mtr_list_get db_read_nomal err i = %d\n", i);
                }
            }
            free(MtrTypeList);
            MtrTypeList = NULL;
        }
        break;

        default:
            break;
        }
    }
    return pMtrHead;
}

/**
*********************************************************************
* @brief：     销毁MS
* @param[in]： pMSHead     ms集合链表头
* @return：    ms集合链表头
*********************************************************************
*/
MS_T* ms_destroy(MS_T* pMSHead)
{
    MS_T *tempMS = NULL;
    MS_T *pMS = pMSHead;
    while(pMS)
    {
        tempMS = pMS->next;
        free(pMS);
        pMS = tempMS;
    }
    pMSHead = NULL;
    return pMSHead;
}

/**
*********************************************************************
* @brief：     获取RSD
* @param[in]： inData      原始报文数据
* @param[in]： InDataLen   报文长度
* @param[out]：pRSD        筛选方法
* @return：    offset      解完后偏移
*********************************************************************
*/
uint16 rsd_get(DB_CLIENT hUdp, const uint8* inData, int InDataLen, RSD_T* pRSD)
{
    uint16 tempLen = InDataLen;
    uint16 offset = 0;
    pRSD->selId = inData[offset++];
    switch(pRSD->selId)
    {
    case 0:
        return offset;
    case 1:
        pRSD->SelOad.nObjID = inData[offset++];
        pRSD->SelOad.nObjID = (pRSD->SelOad.nObjID<<8)+inData[offset++];
        pRSD->SelOad.attID = inData[offset++];
        pRSD->SelOad.nIndex = inData[offset++];
        
        //OAD起始值
        if(inData[offset++] == DT_DATETIME_S) //方法1，只有唯一的数据点
        {
            pRSD->StartData = (uint8 *)malloc(7);
            memcpy(pRSD->StartData, &inData[offset], 7);
            offset += 7;
        }
        else
        {
            offset -= 1;
        }
        
        //OAD结束值
        return offset;
    case 2:
        pRSD->SelOad.nObjID = inData[offset++];
        pRSD->SelOad.nObjID = (pRSD->SelOad.nObjID<<8)+inData[offset++];
        pRSD->SelOad.attID = inData[offset++];
        pRSD->SelOad.nIndex = inData[offset++];
        if(inData[offset++] == DATE_TIME_S)
        {
            pRSD->StartData = (uint8 *)malloc(7);
            memcpy(pRSD->StartData, &inData[offset], 7);
            offset += 7;
            if(inData[offset++] == DATE_TIME_S)
            {
                pRSD->EndData= (uint8 *)malloc(7);
                memcpy(pRSD->EndData, &inData[offset], 7);
                offset += 7;
                if(inData[offset] != eTI && inData[offset] != NULL_DATA)
                {
                    free(pRSD->StartData);
                    pRSD->StartData = NULL;
                    free(pRSD->EndData);
                    pRSD->EndData = NULL;
                    offset -= 17;
                }
                offset++;
                if(inData[offset-1] == eTI)
                {
                    pRSD->Ti.unit = inData[offset++];
                    memcpy_r((uint8 *)&pRSD->Ti.value, &inData[offset], 2);
                    offset += 2;
                }
            }
            else
            {
                free(pRSD->StartData);
                pRSD->StartData = NULL;
                offset -= 9;
            }
        }
        else
        {
            offset -= 1;
        }
        pRSD->Ms = NULL;
        return offset;
    case 3://2的组合模式，需要外面循环调用
        pRSD->SelOad.nObjID = inData[offset++];
        pRSD->SelOad.nObjID = (pRSD->SelOad.nObjID<<8)+inData[offset++];
        pRSD->SelOad.attID = inData[offset++];
        pRSD->SelOad.nIndex = inData[offset++];
        //OAD起始值
        //OAD结束值
        return offset;
    case 4:
    case 5:
        pRSD->StartData= (uint8 *)malloc(7);
        pRSD->EndData = (uint8 *)malloc(7);
        if(tempLen < 7)
        {
            return 0;
        }
        else
        {
            memcpy(pRSD->StartData, &inData[offset], 7);
            memcpy(pRSD->EndData, &inData[offset], 7);
            offset += 7;
        }
        pRSD->Ti.unit = 0xff;
        pRSD->Ti.value = 0;
        pRSD->Ms = mtr_list_get(hUdp, inData, &offset, FALSE);
        if(!pRSD->Ms)
        {
            return 0;
        }
        return offset;
    case 6:
    case 7:
    case 8:
        pRSD->StartData= (uint8 *)malloc(7);
        pRSD->EndData = (uint8 *)malloc(7);
        if(tempLen < 7)
        {
            return 0;
        }
        else
        {
            memcpy(pRSD->StartData, &inData[offset], 7);
            offset += 7;
            tempLen -= 7;
        }

        if(tempLen < 7)
        {
            return 0;
        }
        else
        {
            memcpy(pRSD->EndData, &inData[offset], 7);
            offset += 7;
            tempLen -= 7;
        }
        pRSD->Ti.unit = inData[offset];
        pRSD->Ti.value = inData[offset+1];
        pRSD->Ti.value = ((pRSD->Ti.value)<<8) + inData[offset+2];
        tempLen -= 3;

        if (tempLen <= 0)
        {
            return 0;
        }
        else
        {
            offset += 3;
            pRSD->Ms = mtr_list_get(hUdp, inData, &offset, FALSE);
            if(!pRSD->Ms)
            {
                return 0;
            }
            else
            {
                return offset;
            }
        }
    case 9:
        pRSD->LastN = inData[offset++];
        return offset;
    case 10:
        pRSD->LastN = inData[offset++];
        pRSD->Ms = mtr_list_get(hUdp, inData, &offset, FALSE);
        if(!pRSD->Ms)
        {
            return 0;
        }
        return offset;
    #ifdef AREA_FUJIAN
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
        return rsd_get_fujian(hUdp, inData, InDataLen, pRSD);
        break;
    #endif
    default:
        return 0;
    }
}

/**
*********************************************************************
* @brief：      释放RSD节点
* @param[in]： pRSD 
* @return：     pRSD
*********************************************************************
*/
RSD_T* rsd_destroy(RSD_T* pRSD)
{
    FREE_S(pRSD->StartData);
    FREE_S(pRSD->EndData);
    
    ms_destroy(pRSD->Ms);
    
    FREE_S(pRSD);

    return pRSD;
}

/**
*********************************************************************
* @brief：     获取ROAD下的子OAD
* @param[in]： inData      原始报文数据
* @param[in]： InDataLen   报文长度
* @param[out]：offset      解完后偏移
* @return：    ROAD        pROAD
*********************************************************************
*/
CSD_T* rcsd_destroy(CSD_T* pCSDHead)
{
    CSD_T *TempCSD = NULL;
    CSD_T *pCSD = pCSDHead;

    while (pCSD)
    {
        TempCSD = pCSD->next;

        if (pCSD->CSDType == 1)
        {
            if(pCSD->Road.SubOad != NULL)
            {
                free(pCSD->Road.SubOad);
            }
        }
        free(pCSD);
        pCSD = TempCSD;
    }
    pCSDHead = NULL;
    return pCSDHead;
}

/**
*********************************************************************
* @brief：     获取ROAD下的子OAD
* @param[in]： inData      原始报文数据
* @param[in]： InDataLen   报文长度
* @param[out]：offset      解完后偏移
* @return：    ROAD        pROAD
*********************************************************************
*/
BOOL sub_oad_get(uint8* inData, int inDataLen, uint16* offset, ROAD_T* pROAD)
{
    uint8 i = 0;
    
    pROAD->MainOad.nObjID = (uint16)(inData[(*offset)]<<8) + inData[(*offset)+1];
    (*offset) += 2;
    
    pROAD->MainOad.attID  = inData[(*offset)++];
    pROAD->MainOad.nIndex = inData[(*offset)++];
    inDataLen -= 4;

    pROAD->SubOadNum = inData[(*offset)++];
    inDataLen--;
    pROAD->SubOad = (OOP_OAD_U *)malloc((pROAD->SubOadNum)*sizeof(OOP_OAD_U));
    memset(pROAD->SubOad, 0, (pROAD->SubOadNum)*sizeof(OOP_OAD_U));//清0
    for(i = 0; i < pROAD->SubOadNum; i++)
    {
        pROAD->SubOad[i].nObjID = (uint16)(inData[(*offset)]<<8) + inData[(*offset)+1];
        (*offset) += 2;
        pROAD->SubOad[i].attID  = inData[(*offset)++];
        pROAD->SubOad[i].nIndex = inData[(*offset)++];
        inDataLen -= 4;
    }
    if(inDataLen < 0)
        return FALSE;
    return TRUE;
}

/**
*********************************************************************
* @brief：     获取ROAD下的子OAD
* @param[in]： inData      原始报文数据
* @param[in]： InDataLen   报文长度
* @param[out]：offset      解完后偏移
* @return：    ROAD        pROAD
*********************************************************************
*/
BOOL transparent_plan_sub_oad_get( ROAD_T* pROAD,uint8 planid)
{
    uint8 i = 0;

    pROAD->MainOad.value = 0x601A0200|planid;
    pROAD->SubOadNum = 0x03;
    pROAD->SubOad = (OOP_OAD_U *)malloc((pROAD->SubOadNum)*sizeof(OOP_OAD_U));
    memset(pROAD->SubOad, 0, (pROAD->SubOadNum)*sizeof(OOP_OAD_U));//清0
    for(i = 0; i < pROAD->SubOadNum; i++)
    {
        pROAD->SubOad[i].value = i;

    }
    return TRUE;
}

/**
*********************************************************************
* @brief：     获取RCSD
* @param[in]： inData      原始报文数据
* @param[in]： InDataLen   报文长度
* @param[out]：offset      解完后偏移
* @return：    CSD_T       RCSD
*********************************************************************
*/
CSD_T* rcsd_get(uint8* inData, uint16 inDataLen, uint16* offset, BOOL isWithType)
{
    int CSDNum = 0;
    CSD_T* pCSDHead = (CSD_T *)malloc(sizeof(CSD_T));
    CSD_T* pCSD = NULL;
    CSD_T* CSDTail = NULL;
    int32 i = 0;

    // 先初始化防止乱值导致后面释放出错
    if (pCSDHead != NULL)
    {
        memset(pCSDHead, 0x00, sizeof(CSD_T));
    }

    if (isWithType)
    {
        if (inData[(*offset)++] != 1)
        {
            pCSDHead = rcsd_destroy(pCSDHead);
            return NULL;
        }
        inDataLen--;
    }

    CSDNum = inData[(*offset)++];
    inDataLen--;

    //if (CSDNum <= 0) //允许CSDNum = 0
    //{
   //     pCSDHead = rcsd_destroy(pCSDHead);
   //     return NULL;
   // }

    CSDTail = pCSDHead;

    PRTL_FMT_LOGCC("rcsd_get CSDNum = %d\n", CSDNum);
    while (CSDNum > 0)
    {
        if (isWithType)
        {
            if (inData[(*offset)++] != 0x5b)
            {
                pCSDHead = rcsd_destroy(pCSDHead);
                break;
            }
            inDataLen--;
        }

        pCSD = (CSD_T *)malloc(sizeof(CSD_T));

        if (pCSD != NULL)
        {
            memset(pCSD, 0x00, sizeof(CSD_T));
        }

        pCSD->CSDType = inData[(*offset)++];
        inDataLen--;
        if (pCSD->CSDType == 0)					//OAD类型
        {
            //pCSD->Oad.nObjID = inData[(*offset)++]*256 + inData[(*offset)++];
            //pCSD->Oad.attPro = 
            //pCSD->Oad.attID = inData[(*offset)++];
            //pCSD->Oad.nIndex = inData[(*offset)++];
            memcpy_r(&pCSD->Oad, &inData[*offset], sizeof(uint32));
            (*offset) += 4;
            
            inDataLen -= 4;
            PRTL_FMT_LOGCC("rcsd_get pCSD->CSDType == 0, pCSD->Oad.value = 0x%08X\n", pCSD->Oad.value);
        }
        else if (pCSD->CSDType == 1)			//ROAD类型
        {
            if (!sub_oad_get(inData, inDataLen, offset, &(pCSD->Road)))
            {
                printf("ROAD error!\n");
                pCSDHead = rcsd_destroy(pCSDHead);
                free(pCSD->Road.SubOad);
                free(pCSD);
                pCSD = NULL;
                break;
            }
            PRTL_FMT_LOGCC("rcsd_get pCSD->CSDType == 1, pCSD->Road.MainOad.value = 0x%08x\n", pCSD->Road.MainOad.value);
            for (i = 0; i < pCSD->Road.SubOadNum; i++)
            {
                PRTL_FMT_LOGCC("rcsd_get pCSD->CSDType == 1, pCSD->Road.SubOad[%d].value = 0x%08x\n", i, pCSD->Road.SubOad[i].value);
            }

        }
        else
        {
            pCSDHead = rcsd_destroy(pCSDHead);
            free(pCSD);
            break;
        }
        CSDTail->next = pCSD;                     //CSDTail是个指针，最开始指向头，加入pSCD后，指向pCSD,头指针没有实际内容
        CSDTail = pCSD;
        CSDTail->next = NULL;
        CSDNum--;
    }
    return pCSDHead;
}

/**
*********************************************************************
* @brief：     获取RCSD
* @param[in]： inData      原始报文数据
* @param[in]： InDataLen   报文长度
* @param[out]：offset      解完后偏移
* @return：    CSD_T       RCSD
*********************************************************************
*/
CSD_T* transparent_rcsd_get(uint8 planid)
{
    int CSDNum = 0;
    CSD_T* pCSDHead = (CSD_T *)malloc(sizeof(CSD_T));
    CSD_T* pCSD = NULL;
    CSD_T* CSDTail = NULL;
//    OOP_OAD_U OAD;
//    int32 i = 0;

    // 先初始化防止乱值导致后面释放出错
    if (pCSDHead != NULL)
    {
        memset(pCSDHead, 0x00, sizeof(CSD_T));
    }

    CSDNum = 1;

    CSDTail = pCSDHead;

    while (CSDNum > 0)
    {

        pCSD = (CSD_T *)malloc(sizeof(CSD_T));

        if (pCSD != NULL)
        {
            memset(pCSD, 0x00, sizeof(CSD_T));
        }

        pCSD->CSDType = 0x01;
        if (!transparent_plan_sub_oad_get( &(pCSD->Road),planid))
        {
            printf("ROAD error!\n");
            pCSDHead = rcsd_destroy(pCSDHead);
            free(pCSD->Road.SubOad);
            free(pCSD);
            pCSD = NULL;
            break;
        }
        CSDTail->next = pCSD;                     //CSDTail是个指针，最开始指向头，加入pSCD后，指向pCSD,头指针没有实际内容
        CSDTail = pCSD;
        CSDTail->next = NULL;
        CSDNum--;
    }
    return pCSDHead;
}

/**
*********************************************************************
* @brief：     应用分帧情况下判断是否继续抄读该测量点
* @param[in]： bpInfo      断点信息
* @param[in]： metsn       测量点号
* @param[out]：
* @return：    0           继续抄读
               1           抄读下一个电表信息
*********************************************************************
*/
uint8 slicing_is_comtinue_ms(BP_INFO_T* bpInfo,uint16 metsn, READ_RECORD_T *inReadRecord, uint32 *oopRecNumPn, uint32 *index)
{
    int nRet = 0;

    if(bpInfo == NULL)
        return 0;//继续抄读

    //采集成功时间筛选时，记录下到哪个数据点
    *index = 0;

    if(bpInfo->appSlicingFlag != 0)
    {
        if(bpInfo->metsn != 0)
        {
            if(metsn != bpInfo->metsn)
            {
                nRet = 1;//抄读下一个电表信息
            }
            else
            {
                /*查到断点，将断点信息填入 如果bpInfo->recNumSeg>0时，则是按方法1刷选，起始时间填0，结束时间填断点时间*/
                *oopRecNumPn = bpInfo->recNumSeg;

                if (bpInfo->recNumSeg > 0)
                {
                    inReadRecord->cStart = 0;
                    inReadRecord->cEnd = bpInfo->tmStartSeg;
                }
                else
                {
                    if (inReadRecord->cType == COL_TM_END)
                    {
                        *index = bpInfo->index;
                    }
                    else
                    {
                        inReadRecord->cStart = bpInfo->tmStartSeg;
                    }
                }
                
                bpInfo->metsn = 0;
                nRet = 0;
            }
        }
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     应用分帧情况下判断是否继续抄读该测量点
* @param[in]： bpInfo      断点信息
* @param[in]： metsn       测量点号
* @param[out]：
* @return：    0           继续抄读
               1           抄读下一个电表信息
*********************************************************************
*/
uint8 slicing_is_comtinue_rec_table(BP_INFO_T* bpInfo,uint16 metsn, uint32 *startTime, uint32 *endTime, uint32 *oopRecNumPn)
{
    int nRet = 0;

    if(bpInfo == NULL)
        return 0;//继续抄读

    if(bpInfo->appSlicingFlag != 0)
    {
        if(bpInfo->metsn != 0)
        {
            if(metsn < bpInfo->metsn)
            {
                nRet = 1;//抄读下一个电表信息
            }
            else if(metsn == bpInfo->metsn)
            {
                /*查到断点，将断点信息填入 如果bpInfo->recNumSeg>0时，则是按方法1刷选，起始时间填0，结束时间填断点时间*/
                *oopRecNumPn = bpInfo->recNumSeg;
                if (bpInfo->recNumSeg > 0)
                {
                    *startTime = 0;
                    *endTime   = bpInfo->tmStartSeg;
                }
                else
                {
                    *startTime = bpInfo->tmStartSeg;
                }
                nRet = 0;
            }
            else if (metsn > bpInfo->metsn)
            {
                /* 断点表循环结束，准备进行下一个表的循环，此时的断点时间改成刷选时的起始时间 */
                *oopRecNumPn = 0; 
                nRet = 0;
            }
        }
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     APDU检查
* @param[in]： SevID       服务类型
* @param[in]： nChoice     
               inBuff      输入报文
               inLen       输入长度
* @param[out]：
* @return：    -1:数据帧不完整,-2:服务不支持 ,>0解析的帧长度
*********************************************************************
*/
int serial_chkapdu(uint8 sevID, uint8 nChoice, const uint8 *inBuff, uint32 inLen)
{
    uint8 i, j;
    uint32 offSet = 0;
    switch(sevID)
    {
    case SERVERID_LINKRESPONSE:
    {
        apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE);
        apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE);
        apdu_next_position_get(inBuff, &offSet, inLen, DT_DATETIME, TRUE);
        apdu_next_position_get(inBuff, &offSet, inLen, DT_DATETIME, TRUE);
        if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_DATETIME, TRUE))
            return -1;
    }
    break;
    case SERVERID_CONNECTREQUEST:
    {
        uint8 SecType;
        apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE);
        apdu_next_position_get(inBuff, &offSet, inLen, DT_LONG_UNSIGNED, TRUE);
        offSet += 8;
        offSet += 16;
        apdu_next_position_get(inBuff, &offSet, inLen, DT_LONG_UNSIGNED, TRUE);
        apdu_next_position_get(inBuff, &offSet, inLen, DT_LONG_UNSIGNED, TRUE);
        apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE);
        apdu_next_position_get(inBuff, &offSet, inLen, DT_LONG_UNSIGNED, TRUE);
        if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_DOUBLE_LONG_UNSIGNED, TRUE))
            return -1;
        SecType = inBuff[offSet++];
        switch(SecType)
        {
        case 0:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_NULL, TRUE))
                return -1;
            break;
        case 1:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_VISIBLE_STRING, TRUE))
                return -1;
            break;
        case 2:
        case 3:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OCTET_STRING, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OCTET_STRING, TRUE))
                return -1;
            break;
        default:
            return -2;
        }
    }
    break;
    case SERVERID_RELEASEREQUEST:
    {
        if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
            return -1;
    }
    break;
    case SERVERID_GETREQUEST:
    {
        //uint8 nChoice;
        uint8 seqNum;
        //nChoice = m_BufferRead[m_OffsetRead++];
        switch(nChoice)
        {
        case 1:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OAD, TRUE))
                return -1;
            break;
        case 2:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                return -1;
            seqNum = inBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OAD, TRUE))
                    return -1;
            }
            break;
        case 3:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OAD, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_RSD, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_RCSD, TRUE))
                return -1;
            break;
        case 4:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                return -1;
            seqNum = inBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OAD, TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_RSD, TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_RCSD, TRUE))
                    return -1;
            }
            break;
        case 5:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_LONG_UNSIGNED, TRUE))
                return -1;
            break;
        #if GET_OPTIMIZATION_EN == 1
        case 13:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OAD, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_RSD, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_RCSD, TRUE))
                return -1;
            break;
        #endif
        #if GET_SIMPLIFYRECORD_EN == 1
        case 23:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OAD, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_RSD, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_ROAD, TRUE))
                return -1;
            break;
        #endif
        default:
            return -2;
        }
    }
    break;
    case SERVERID_SETREQUEST:
    {
        //uint8 nChoice;
        uint8 seqNum;
        //nChoice = m_BufferRead[m_OffsetRead++];

        switch(nChoice)
        {
        case 1:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OAD, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_NULL, FALSE))
                return -1;
            break;
        case 2:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                return -1;
            seqNum = inBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OAD, TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_NULL, FALSE))
                    return -1;
            }
            break;
        case 3:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                return -1;
            seqNum = inBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OAD, TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_NULL, FALSE))
                    return -1;
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OAD, TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                    return -1;
            }
            break;
        default:
            return -2;
        }
    }
    break;
    case SERVERID_ACTIONREQUEST:
    {
        //uint8 nChoice;
        uint8 seqNum;
        //nChoice = m_BufferRead[m_OffsetRead++];
        switch(nChoice)
        {
        case 1:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OMD, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_NULL, FALSE))
                return -1;
            break;
        case 2:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                return -1;
            seqNum = inBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OMD, TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_NULL, FALSE))
                    return -1;
            }
            break;
        case 3:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                return -1;
            seqNum = inBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OMD, TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_NULL, FALSE))
                    return -1;
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OAD, TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                    return -1;
            }
            break;
        default:
            return -2;
        }
    }
    break;
    case SERVERID_REPORTRESPONSE:
    {
        //uint8 nChoice;
        uint8 seqNum;
        //nChoice = m_BufferRead[m_OffsetRead++];
        switch(nChoice)
        {
        case 1:
        case 2:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                return -1;
            seqNum = inBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OMD, TRUE))
                    return -1;
            }
            break;
        default:
            return -2;
        }
    }
    break;
    case SERVERID_PROXYREQUEST:
    {
        //uint8 nChoice;
        uint8 seqNum;
        //nChoice = m_BufferRead[m_OffsetRead++];
        switch(nChoice)
        {
        case 1:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_LONG_UNSIGNED, TRUE))
                return -1;
            seqNum = inBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_TSA, TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_LONG_UNSIGNED, TRUE))
                    return -1;
                uint8 oadNum = inBuff[offSet++];
                for(j=0; j<oadNum; j++)
                {
                    if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OAD, TRUE))
                        return -1;
                }
            }
            break;
        case 2:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_LONG_UNSIGNED, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_TSA, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OAD, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_RSD, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_RCSD, TRUE))
                return -1;
            break;
        case 3:
        case 5:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_LONG_UNSIGNED, TRUE))
                return -1;
            seqNum = inBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_TSA, TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_LONG_UNSIGNED, TRUE))
                    return -1;
                uint8 oadNum = inBuff[offSet++];
                for(j=0; j<oadNum; j++)
                {
                    if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OAD, TRUE))
                        return -1;
                    if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_NULL, FALSE))
                        return -1;
                }
            }
            break;
        case 4:
        case 6:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_LONG_UNSIGNED, TRUE))
                return -1;
            seqNum = inBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_TSA, TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_LONG_UNSIGNED, TRUE))
                    return -1;
                uint8 oadNum = inBuff[offSet++];
                for(j=0; j<oadNum; j++)
                {
                    if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OAD, TRUE))
                        return -1;
                    if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_NULL, FALSE))
                        return -1;
                    if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OAD, TRUE))
                        return -1;
                    if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                        return -1;
                }
            }
            break;
        case 7:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_UNSIGNED, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OAD, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_COMDCB, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_LONG_UNSIGNED, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_LONG_UNSIGNED, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OCTET_STRING, TRUE))
                return -1;
            break;
        default:
            return -2;
        }
    }
    break;
    case SERVERID_SECURITYREQUEST:
    {
        uint8 AuthType;
        if(inBuff[offSet++] > 0x01)//应用数据单元CHIOCE 0:明文，1密文
            return -1;
        if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_OCTET_STRING, TRUE))
            return -1;
        AuthType = inBuff[offSet++];
        if(AuthType > 0x03)
            return -1;
        switch(AuthType)
        {
        case 0:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_SIDMAC, TRUE))
                return -1;
            break;
        case 1:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_RN, TRUE))
                return -1;
            break;
        case 2:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_RN, TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_MAC, TRUE))
                return -1;
            break;
        case 3:
            if(FALSE == apdu_next_position_get(inBuff, &offSet, inLen, DT_SID, TRUE))
                return -1;
            break;
        }
    }
    break;
    default:
        return -2;
        break;
    }
    return offSet;
}

#endif

#if DESC("记录型响应",1)

OOP_DAR_E read_record_response(OAD_INFO_T *pOadInfo, APDU_INFO_T *apduInfo, const uint8 *inData, uint16 InDataLen, uint16 offset, uint16 csdStart, RECORD_BUF_T *outRecordBuf, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outOffset = outData->resultDataLen;
    uint16 csdLen = 0;

    //为空封装返回报文头，非空拼装返回的数据
    if(outRecordBuf == NULL)
    { 
        /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
        csdLen = offset-csdStart;
        
        outOffset = 0; 
        memcpy(&outData->pResultData[outOffset], inData, sizeof(uint32));
        outOffset += sizeof(uint32);

        if(csdLen == 1) //RCSD=0
        {
            pOadInfo->noParse = FALSE;
            all_rcsd_get(pOadInfo, apduInfo, &outOffset, outData->pResultData, outDataMax);
        }
        else
        {
            memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
            outOffset += csdLen;
        }
        
        outData->pResultData[outOffset++] = 1;
    }
    else
    {
        memcpy(&outData->pResultData[outOffset], outRecordBuf->recordBuf, outRecordBuf->bufLen);
        outOffset += outRecordBuf->bufLen;
    }

    outData->resultDataLen = outOffset;

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief：       根据参数构造记录型响应报文（目前只用于事件上报）
* @param[in] ： oad       记录表OAD
* @param[in] ： rcsd     列筛选描述符
* @param[out]： convert  数据转换结构
* @return：      =0-成功，!=0-错误
*********************************************************************
*/
OOP_DAR_E record_response_make(OOP_OAD_U oad, OOP_RCSD_T rcsd, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    IC_OAD_INFO_T icInfo;   
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32  dOffset = *convert->dOffset;    

    //OAD
    memcpy_r(&dstBuf[dOffset], &oad, sizeof(uint32));
    dOffset += 4;

    //RCSD
    dar = apdu_rcsd_tofmt698(dstBuf, dstLen, &dOffset, &rcsd, sizeof(OOP_RCSD_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_rcsd_tofmt698 failed. rcsd.nNum(%d)\n", rcsd.nNum);
        return dar;        
    }

    //响应数据
    dstBuf[dOffset++] = 1; //[1] SEQUENCE OF A-RecordRow

    //记录条数
    dstBuf[dOffset++] = 1; //M条记录

    //记录数据
    icInfo.choice = 0;
    icInfo.oad.value = oad.value;

    *convert->dOffset = dOffset;
    
    dar = normal_data_to_fmt698(&icInfo, convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("normal_data_to_fmt698 failed. oad(0x%08x)\n", icInfo.oad.value);
        return dar;        
    }

    //注意：由于是记录型，剥掉前面的结构类型和长度
    memmove(&((char*)convert->dstBuf)[dOffset], &((char*)convert->dstBuf)[dOffset] + 2, dstLen - dOffset - 2);

    *convert->dOffset -= 2;
    
    return dar;
}

/**
*********************************************************************
* @brief：       计算分帧时要减去的长度
* @param[in] ：  connectionInfo 应用连接信息
* @param[out]：  
* @return：      长度
*********************************************************************
*/
uint16 cal_excludeLen(APP_CONTEXT_t connectionInfo)
{    // 分帧时考虑额外数据的长度
    uint16 excludeLen = 0;

    excludeLen += 14;                       // 698帧头

    excludeLen += 8;                        // 最大(应用分帧时)去掉8个字节的长度要组服务和piid之类的数据

    excludeLen += 1;                        // 跟随上报暂时定为00
    excludeLen += 1;                        // 时标
    
    if (connectionInfo.timeTag.tagFlag != 0)  // 如果带时标要加10字节
    {
        excludeLen += 10;
    }
    
    if (connectionInfo.securityServer == TRUE)
    {
        switch (connectionInfo.curSecurityMode) 
        {
            // 明文+ MAC
            case SecMod_PlainMAC:   
                excludeLen += 7;    // OPTIONAL MAC
                break;
                
            // 密文
            case SecMod_Cipher:     
                excludeLen += 16;   // 加密报文最多会增加16字节的长度
                excludeLen += 7;    // OPTIONAL MAC
                break;  
                
            // 密文+ MAC
            case SecMod_CipherMAC:  
                excludeLen += 16;   // 加密报文最多会增加16字节的长度
                excludeLen += 7;    // OPTIONAL MAC
                break;

            default:
                excludeLen += 7;    // OPTIONAL MAC
                break;
        }

        excludeLen += 5;            // 安全response(1)、piip(1)、长度(3)
    }

    excludeLen += 3;                // 698尾校验和0x16

    PRTL_FMT_TRACE("Calculate the length to be subtracted from the frame, excludeLen:%d\n", excludeLen);

    return excludeLen;
}
#endif

BOOL check_is_all_ch(uint8 *buf ,uint8 ch,uint16 len)
{
	uint16 i=0;
	
	for(i=0;i<len;i++)
	{
		if(buf[i] !=ch)
		{
			return FALSE;
		}
	}
	return TRUE;
}

/**
*********************************************************************
* @brief：       召测普通OAD回复NULL类型数据
* @param[in] ：  
* @param[out]：  
* @return：      返回值
* @author:      贺宁
*********************************************************************
*/
OOP_DAR_E oop_null_data_res(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint32 offset = 0;

    //数据中心出错返回NULL
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;
    outData->pResultData[offset++] = DT_NULL;
    outData->resultDataLen += offset;
    
    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief：       召测普通OAD回复空数组
* @param[in] ：  
* @param[out]：  
* @return：      返回值
* @author:      贺宁
*********************************************************************
*/
OOP_DAR_E oop_null_array_res(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint32 offset = 0;

    //数据中心出错返回NULL
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;
    outData->pResultData[offset++] = DT_ARRAY;
    outData->pResultData[offset++] = 0;    
    outData->resultDataLen += offset;
    
    return DATA_SUCCESS;
}


/**********************************************************************
* @name      : app_setdefault_cfg_area
* @brief     ：进程启动 设置地区默认参数
* @param[in] ：int sockdbfd    句柄
* @param[out]：
* @return    ：
**********************************************************************/
void app_setdefault_cfg_area(int sockdbfd)
{
    #ifdef AREA_ANHUI
    return app_setdefault_cfg_anhui(sockdbfd);
    #endif
    
    #ifdef AREA_JIANGSU
    return app_setdefault_cfg_jiangsu(sockdbfd);
    #endif
    
    #ifdef AREA_SHANDONG
    return app_setdefault_cfg_shandong(sockdbfd);
    #endif

    #ifdef AREA_HUBEI
    return app_setdefault_cfg_hubei(sockdbfd);
    #endif

	#ifdef AREA_SHAANXI
    return app_setdefault_cfg_shaanxi(sockdbfd);
    #endif

    #ifdef AREA_FUJIAN
    return app_setdefault_cfg_fujian(sockdbfd);
    #endif

    #ifdef AREA_ZHEJIANG
    return app_setdefault_cfg_zhejiang(sockdbfd);
    #endif

    return;
}
