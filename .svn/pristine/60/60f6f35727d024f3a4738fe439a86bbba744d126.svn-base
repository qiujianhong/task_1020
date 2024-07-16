/*
*********************************************************************
* @file    class20.c
* @brief： class20处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopStorage.h"

//
// class20
//

/**
*********************************************************************
* @brief：      显式安全模式参数
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset    构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class20_access_right(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_OI_RIGHTS_T data = {0};
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码
    uint32 i = 0;
    uint32 j = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //数组类型
    dstBuf[dOffset++] = DT_ARRAY;

    //数组成员个数
    offlen = set_len_offset(data.nNum, lenBuf);
    memcpy(&dstBuf[dOffset], lenBuf, offlen);
    dOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {
        //结构类型
        dstBuf[dOffset++] = DT_STRUCTURE;

        //结构成员数
        dstBuf[dOffset++] = 2;
        
        //对象标识
        dar = basic_data_to_buf(E_OI, dstBuf, dstLen, &dOffset, &data.oiRight[i].oi, sizeof(OI));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),OI(0x%04x)\n", i, dar, data.oiRight[i].oi);
            return DATA_TYPE_UNMATCHED;
        }

        //结构类型
        dstBuf[dOffset++] = DT_STRUCTURE;

        //结构成员数
        dstBuf[dOffset++] = 2;

        //数组类型
        dstBuf[dOffset++] = DT_ARRAY;

        //数组成员个数
        offlen = set_len_offset(data.oiRight[i].accessRight.aars.nNum, lenBuf);
        memcpy(&dstBuf[dOffset], lenBuf, offlen);
        dOffset += offlen;

        for (j = 0; j < data.oiRight[i].accessRight.aars.nNum; j++)
        {
            //结构类型
            dstBuf[dOffset++] = DT_STRUCTURE;

            //结构成员数
            dstBuf[dOffset++] = 2;

            //属性ID
            dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.oiRight[i].accessRight.aars.aar[j].index, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),OI(0x%04x)\n", i, dar, data.oiRight[i].accessRight.aars.aar[j].index);
                return DATA_TYPE_UNMATCHED;
            }

            //属性权限
            dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.oiRight[i].accessRight.aars.aar[j].rw, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),OI(0x%04x)\n", i, dar, data.oiRight[i].accessRight.aars.aar[j].index);
                return DATA_TYPE_UNMATCHED;
            }
        }

        //数组类型
        dstBuf[dOffset++] = DT_ARRAY;

        //数组成员个数
        offlen = set_len_offset(data.oiRight[i].accessRight.mars.nNum, lenBuf);
        memcpy(&dstBuf[dOffset], lenBuf, offlen);
        dOffset += offlen;

        for (j = 0; j < data.oiRight[i].accessRight.mars.nNum; j++)
        {
            //结构类型
            dstBuf[dOffset++] = DT_STRUCTURE;

            //结构成员数
            dstBuf[dOffset++] = 2;

            //属性ID
            dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.oiRight[i].accessRight.mars.mar[j].index, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),OI(0x%04x)\n", i, dar, data.oiRight[i].accessRight.mars.mar[j].index);
                return DATA_TYPE_UNMATCHED;
            }

            //属性权限
            dar = basic_data_to_buf(E_BOOL, dstBuf, dstLen, &dOffset, &data.oiRight[i].accessRight.mars.mar[j].exec, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),OI(0x%04x)\n", i, dar, data.oiRight[i].accessRight.mars.mar[j].exec);
                return DATA_TYPE_UNMATCHED;
            }
        }       
    }       

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
* @brief：      显式安全模式参数
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset    构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class20_association_context(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_ASSOCIATION_CONTEXT_T data = {0};


    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //数组类型
    dstBuf[dOffset++] = DT_STRUCTURE;
    dstBuf[dOffset++] = 7;
    
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.VER , sizeof(data.VER));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nRecvSize , sizeof(data.nRecvSize));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n",  dar);
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nSendSize , sizeof(data.nSendSize));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nMaxPDUSize , sizeof(data.nMaxPDUSize));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed.  dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_data_to_buf(E_OOP_BITSTR64_T, dstBuf, dstLen, &dOffset, data.PCB , sizeof(data.PCB));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed.dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_data_to_buf(E_OOP_BITSTR128_T, dstBuf, dstLen, &dOffset, data.FCB , sizeof(data.FCB));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed.  dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nTimeout , sizeof(data.nTimeout));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed.dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

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
* @brief：      显式安全模式参数(不带array)
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset    构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class20_oi_right(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_OI_RIGHT_T data = {0};
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码
    uint32 j = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 2;
    
    //对象标识
    dar = basic_data_to_buf(E_OI, dstBuf, dstLen, &dOffset, &data.oi, sizeof(OI));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),OI(0x%04x)\n", dar, data.oi);
        return DATA_TYPE_UNMATCHED;
    }

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 2;

    //数组类型
    dstBuf[dOffset++] = DT_ARRAY;

    //数组成员个数
    offlen = set_len_offset(data.accessRight.aars.nNum, lenBuf);
    memcpy(&dstBuf[dOffset], lenBuf, offlen);
    dOffset += offlen;

    for (j = 0; j < data.accessRight.aars.nNum; j++)
    {
        //结构类型
        dstBuf[dOffset++] = DT_STRUCTURE;

        //结构成员数
        dstBuf[dOffset++] = 2;

        //属性ID
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.accessRight.aars.aar[j].index, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),OI(0x%04x)\n", dar, data.accessRight.aars.aar[j].index);
            return DATA_TYPE_UNMATCHED;
        }

        //属性权限
        dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.accessRight.aars.aar[j].rw, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),OI(0x%04x)\n", dar, data.accessRight.aars.aar[j].index);
            return DATA_TYPE_UNMATCHED;
        }
    }

    //数组类型
    dstBuf[dOffset++] = DT_ARRAY;

    //数组成员个数
    offlen = set_len_offset(data.accessRight.mars.nNum, lenBuf);
    memcpy(&dstBuf[dOffset], lenBuf, offlen);
    dOffset += offlen;

    for (j = 0; j < data.accessRight.mars.nNum; j++)
    {
        //结构类型
        dstBuf[dOffset++] = DT_STRUCTURE;

        //结构成员数
        dstBuf[dOffset++] = 2;

        //属性ID
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.accessRight.mars.mar[j].index, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),OI(0x%04x)\n", dar, data.accessRight.mars.mar[j].index);
            return DATA_TYPE_UNMATCHED;
        }

        //属性权限
        dar = basic_data_to_buf(E_BOOL, dstBuf, dstLen, &dOffset, &data.accessRight.mars.mar[j].exec, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),OI(0x%04x)\n", dar, data.accessRight.mars.mar[j].exec);
            return DATA_TYPE_UNMATCHED;
        }
    }             

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
* @brief：     从数据中心的数据转698格式报文(class20)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class20(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x44000200:   //应用连接对象列表
        {
           return data_to_buf_class20_access_right(pOadInfo,  convert);
        }break;
        case 0x44000300:   //应用应用语境信息
        {
           return data_to_buf_class20_association_context(pOadInfo,  convert);
        }break;

        //......
        
        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief：      从698格式报文转数据中心的数据
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_class20(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return 0;
}

/**
*********************************************************************
* @name：      class20_invoke_access_right
* @brief：     class8的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class20_invoke_get_access_right(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;  

    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x44000200:
        {
            uint32 i = 0; 
            uint32 j = 0;
            const ClassBaseList_t *objectList;
	        uint16 totalObjectNum;
			OOP_OI_RIGHTS_T data = {0};
            
            //逻辑地址没用带进来，暂时用0
            objectList = ObjectList_All[0].pList;
			totalObjectNum = ObjectList_All[0].num;

            data.nNum = totalObjectNum;

            if (data.nNum > 2048)
            {
                PRTL_FMT_LOGCC("class20_invoke_access_right err[%d]: data.nNum = %d", DATA_SCOPE_OF_ACCESS_VIOLATED, data.nNum);
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            for (i = 0; i < totalObjectNum; i++)
            {
                OOP_OI_RIGHT_T oiRight = {0};
                OOP_ACCESS_RIGHT accessRight;
                OOP_AAR_ACCESS_RIGHT aarAccessRight = {0};
                OOP_MAR_ACCESS_RIGHT marAccessRight = {0};
                MEMZERO(&accessRight, sizeof(accessRight)); 

                aarAccessRight.nNum = objectList[i].AttrNum;
                if (aarAccessRight.nNum > 128)
                {
                    PRTL_FMT_LOGCC("class20_invoke_access_right err[%d]: aarAccessRight.nNum = %d", DATA_SCOPE_OF_ACCESS_VIOLATED, aarAccessRight.nNum);
                    return DATA_SCOPE_OF_ACCESS_VIOLATED;
                }
                for (j = 0; j < aarAccessRight.nNum; j++)
                {
                    aarAccessRight.aar[j].index = objectList[i].aarAccessRight[j].attr_id;
                    aarAccessRight.aar[j].rw = objectList[i].aarAccessRight[j].right;
                }

                marAccessRight.nNum = objectList[i].MethodNum;
                if (marAccessRight.nNum > 128)
                {
                    PRTL_FMT_LOGCC("class20_invoke_access_right err[%d]: marAccessRight.nNum = %d", DATA_SCOPE_OF_ACCESS_VIOLATED, marAccessRight.nNum);
                    return DATA_SCOPE_OF_ACCESS_VIOLATED;
                }
                for (j = 0; j < marAccessRight.nNum; j++)
                {
                    marAccessRight.mar[j].index = objectList[i].marAccessRight[j].attr_id;
                    marAccessRight.mar[j].exec = objectList[i].marAccessRight[j].right;
                }

                accessRight.aars = aarAccessRight;
                accessRight.mars = marAccessRight;

                oiRight.oi = objectList[i].OI;
                oiRight.accessRight = accessRight;

                data.oiRight[i] = oiRight;
            }

			PRTL_FMT_TRACE("class20_invoke_access_right : data.nNum = %d\n", data.nNum);
            // for (i = 0; i < data.nNum; i++)
            // {
            //     PRTL_FMT_TRACE("class20_invoke_access_right : data.oiRight[i].oi = 0x%04x\n", data.oiRight[i].oi);
            //     PRTL_FMT_TRACE("class20_invoke_access_right : data.oiRight[%d].accessRight.aars.nNum = %d\n", i, data.oiRight[i].accessRight.aars.nNum);
            //     for (j = 0; j < data.oiRight[i].accessRight.aars.nNum; j++)
            //     {
            //         PRTL_FMT_TRACE("class20_invoke_access_right: data.oiRight[%d].accessRight.aars.aar[%d].index = %d\n",
            //             i, j, data.oiRight[i].accessRight.aars.aar[j].index);
            //         PRTL_FMT_TRACE("class20_invoke_access_right: data.oiRight[%d].accessRight.aars.aar[%d].rw = %d\n",
            //             i, j, data.oiRight[i].accessRight.aars.aar[j].rw);
            //     }

            //     PRTL_FMT_TRACE("class20_invoke_access_right : data.oiRight[%d].accessRight.mars.nNum = %d\n", i, data.oiRight[i].accessRight.mars.nNum);
            //     for (j = 0; j < data.oiRight[i].accessRight.mars.nNum; j++)
            //     {
            //         PRTL_FMT_TRACE("class20_invoke_access_right: data.oiRight[%d].accessRight.mars.mar[%d].index = %d\n",
            //             i, j, data.oiRight[i].accessRight.mars.mar[j].index);
            //         PRTL_FMT_TRACE("class20_invoke_access_right: data.oiRight[%d].accessRight.mars.mar[%d].exec = %d\n",
            //             i, j, data.oiRight[i].accessRight.mars.mar[j].exec);
            //     }
            // }

            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
    
	/* 数据和报文转换 */
    dar = data_to_buf_class20(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class20 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**********************************************************************
* @name      : app_get_oi_rights_list
* @brief     ：获取可访问对象
* @param[in] ：i        - 序号
* @param[out]：oiRight  - 可访问对象
* @return    : 错误码
* @Create    : 
* @Update    :
**********************************************************************/
int app_get_oi_rights_list(uint8 logic, uint32 infoNum, OOP_OI_RIGHT_T *pOiRight)
{
    uint32 j = 0;
    uint32 i = infoNum;
    const ClassBaseList_t *objectList;
    OOP_OI_RIGHT_T oiRight;
    OOP_ACCESS_RIGHT accessRight;
    OOP_AAR_ACCESS_RIGHT aarAccessRight = {0};
    OOP_MAR_ACCESS_RIGHT marAccessRight = {0};

    MEMZERO(&oiRight, sizeof(oiRight));

    if (i > 2048)
    {
        PRTL_FMT_LOGCC("app_get_oi_rights_list err[%d]: infoNum = %d", DATA_SCOPE_OF_ACCESS_VIOLATED, i);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    objectList = ObjectList_All[logic].pList;
    MEMZERO(&accessRight, sizeof(accessRight)); 

    aarAccessRight.nNum = objectList[i].AttrNum;
    if (aarAccessRight.nNum > 128)
    {
        PRTL_FMT_LOGCC("app_get_oi_rights_list err[%d]: aarAccessRight.nNum = %d", DATA_SCOPE_OF_ACCESS_VIOLATED, aarAccessRight.nNum);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }
    for (j = 0; j < aarAccessRight.nNum; j++)
    {
        aarAccessRight.aar[j].index = objectList[i].aarAccessRight[j].attr_id;
        aarAccessRight.aar[j].rw = objectList[i].aarAccessRight[j].right;
    }

    marAccessRight.nNum = objectList[i].MethodNum;
    if (marAccessRight.nNum > 128)
    {
        PRTL_FMT_LOGCC("app_get_oi_rights_list err[%d]: marAccessRight.nNum = %d", DATA_SCOPE_OF_ACCESS_VIOLATED, marAccessRight.nNum);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }
    for (j = 0; j < marAccessRight.nNum; j++)
    {
        marAccessRight.mar[j].index = objectList[i].marAccessRight[j].attr_id;
        marAccessRight.mar[j].exec = objectList[i].marAccessRight[j].right;
    }

    accessRight.aars = aarAccessRight;
    accessRight.mars = marAccessRight;

    oiRight.oi = objectList[i].OI;
    oiRight.accessRight = accessRight;

    *pOiRight = oiRight;
    
    return ERR_OK;
}

/**
*********************************************************************
* @name：      class20_invoke_access_rights
* @brief：     class8的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class20_invoke_get_oi_rights(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint16    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x44000200:
        {
            DATA_CONVERT_T convert;
            OOP_OI_RIGHT_T oiRight;                                             //一个可访问对象
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 oiRightNum = 0;
            uint32 oiRightNumOffset = 0;

            MEMZERO(&convert, sizeof(convert));

            PRTL_FMT_LOGCC("获取所有可访问对象\n");

            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x01;                             //结果
            outData->pResultData[offset ++] = DT_ARRAY;                         //类型
            oiRightNumOffset = offset;                                          //档案数量偏移
            outData->pResultData[offset ++] = 0x00;                             //数量，先填0
            bufAllLen = offset;

            for (i = 0; i < ObjectList_All[pApduInfo->logicAddr].num; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oi_rights before bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oi_rights before bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oi_rights before bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oi_rights before bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oi_rights before bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
                if (slicing_is_comtinue_oi_rights(pApduInfo->bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_oi_rights after i = %d\n", i);

                //OI访问列表序号（信息点）从0开始
                ret = app_get_oi_rights_list(pApduInfo->logicAddr, i, &oiRight);
                if (ret < ERR_OK)
                {
                    continue;
                }

                PRTL_FMT_LOGCC("找到可访问对象 序号为%d\n", i);

                //转换成报文数据
                convert.srcBuf = &oiRight;
                convert.srcLen = sizeof(oiRight);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_class20_oi_right(pOadInfo, &convert);  //入参
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outData->resultDataLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个可访问对象报文:", i);
                        oiRightNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("class11_invoke_get_oop_meter isBufFull == TRUE isBufFull = %d\n", isBufFull);
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }

            if (!isBufFull)
            {
                if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = pApduInfo->reqType;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

            PRTL_BUF_LOGCC(outData->pResultData, offset, "整个可访问对象:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", pApduInfo->bpBreakInfo->reqType);

            if (oiRightNum > 0)
            {
                result = DATA_SUCCESS;
                outData->pResultData[oiRightNumOffset] = oiRightNum;   //重新记可访问对象数量
                outData->resultDataLen = offset;
            }
            else
            {
                outData->resultDataLen = 0;
                return DATA_READ_WRITE_DENIED;
            }
        }
        break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }
        break;
    }
    return result;
}

/**
*********************************************************************
* @brief：     应用语境信息
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
* @author    : 贺宁
* @Date      ：2019-12-6                 
*********************************************************************
*/
OOP_DAR_E data_to_buf_class20_app_info(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_ASSOCIATION_CONTEXT_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 7;
            
    //协议版本
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.VER, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),VER(%d)\n", dar, data.VER);
        return DATA_TYPE_UNMATCHED;
    }
    
    //最大接收APDU尺寸单元
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nRecvSize, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),nRecvSize(%d)\n", dar, data.nRecvSize);
        return DATA_TYPE_UNMATCHED;
    }

    //最大发送APDU尺寸单元
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nSendSize, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),nSendSize(%d)\n", dar, data.nSendSize);
        return DATA_TYPE_UNMATCHED;
    }

    //最大可处理APDU尺寸
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nMaxPDUSize, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),nMaxPDUSize(%d)\n", dar, data.nMaxPDUSize);
        return DATA_TYPE_UNMATCHED;
    }

    //协议一致性块
    dar = basic_data_to_buf(E_OOP_BITSTR64_T, dstBuf, dstLen, &dOffset, &data.PCB[0], sizeof(OOP_BITSTR64_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),PCB[0](%d)\n", dar, data.PCB[0]);
        return DATA_TYPE_UNMATCHED;
    }

    //功能一致性块
    dar = basic_data_to_buf(E_OOP_BITSTR128_T, dstBuf, dstLen, &dOffset, &data.FCB[0], sizeof(OOP_BITSTR128_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),FCB[0](%d)\n", dar, data.FCB[0]);
        return DATA_TYPE_UNMATCHED;
    }

    //静态超时时间
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nTimeout, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),nTimeout(%d)\n", dar, data.nTimeout);
        return DATA_TYPE_UNMATCHED;
    }

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
* @name：      class20_invoke_get_app_info
* @brief：     应用语境信息
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class20_invoke_get_app_info(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    OOP_ASSOCIATION_CONTEXT_T data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0; 

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    data.VER = 0x0017;
    data.nSendSize = pApduInfo->connectionInfo.sendFrameLen;
    data.nRecvSize = pApduInfo->connectionInfo.recvFrameLen;
    data.nMaxPDUSize = pApduInfo->connectionInfo.recvApduLen;

    //OAD与返回数据
    memcpy(&outData->pResultData[dOffset], inData, sizeof(OOP_OAD_U));
    dOffset += sizeof(OOP_OAD_U);

    outData->pResultData[dOffset++] = 1;

    //转换成报文数据
    convert.srcBuf = &data;
    convert.srcLen = sizeof(data);
    convert.sOffset = &sOffset;
    
    convert.dstBuf = outData->pResultData;
    convert.dstLen = outDataMax;
    convert.dOffset = &dOffset;
    
    dar = data_to_buf_class20_app_info(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class20_app_info failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = dOffset;

    return dar;
}

/**
*********************************************************************
* @brief：     客户机地址
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
* @Date      ：2019-12-6                 
*********************************************************************
*/
OOP_DAR_E data_to_buf_class20_ca_addr(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);
            
    //当前连接的客户机地址
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),VER(%d)\n", dar, data);
        return DATA_TYPE_UNMATCHED;
    }

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
* @name：      class20_invoke_get_ca_addr
* @brief：     应用语境信息
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class20_invoke_get_ca_addr(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0; 

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    data = 0;

    //OAD与返回数据
    memcpy(&outData->pResultData[dOffset], inData, sizeof(OOP_OAD_U));
    dOffset += sizeof(OOP_OAD_U);

    outData->pResultData[dOffset++] = 1;

    //转换成报文数据
    convert.srcBuf = &data;
    convert.srcLen = sizeof(data);
    convert.sOffset = &sOffset;
    
    convert.dstBuf = outData->pResultData;
    convert.dstLen = outDataMax;
    convert.dOffset = &dOffset;
    
    dar = data_to_buf_class20_ca_addr(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class20_ca_addr failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = dOffset;

    return dar;
}

/**
*********************************************************************
* @brief：     客户机地址
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
* @author    : 贺宁
* @Date      ：2019-12-6                 
*********************************************************************
*/
OOP_DAR_E data_to_buf_class20_securityLevel(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);
            
    //连接认证机制
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),VER(%d)\n", dar, data);
        return DATA_TYPE_UNMATCHED;
    }

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
* @name：      class20_invoke_get_ca_addr
* @brief：     应用语境信息
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class20_invoke_get_securityLevel(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0; 

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    data = 0;

    //OAD与返回数据
    memcpy(&outData->pResultData[dOffset], inData, sizeof(OOP_OAD_U));
    dOffset += sizeof(OOP_OAD_U);

    outData->pResultData[dOffset++] = 1;

    //转换成报文数据
    convert.srcBuf = &data;
    convert.srcLen = sizeof(data);
    convert.sOffset = &sOffset;
    
    convert.dstBuf = outData->pResultData;
    convert.dstLen = outDataMax;
    convert.dOffset = &dOffset;
    
    dar = data_to_buf_class20_securityLevel(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class20_ca_addr failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = dOffset;

    return dar;
}

#if DESC("class20对外接口函数",1)

/**
*********************************************************************
* @name：       class20_invoke_set
* @brief：      class20的SET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class20_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    dar = DATA_READ_WRITE_DENIED;

    return dar;
}

/**
*********************************************************************
* @name：       class20_invoke_get
* @brief：      class20的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class20_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0x4400:
        {
            switch (pOadInfo->pTab->oad.attID)
            {
            case 0x02:
                {
                    return class20_invoke_get_oi_rights(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
                }
                break;
            case 0x03:
                {
                    return class20_invoke_get_app_info(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
                }
                break;   
            case 0x04:
                {
                    return class20_invoke_get_ca_addr(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
                }
                break;   
            case 0x05:
                {
                    return class20_invoke_get_securityLevel(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
                }
                break;        
            default:
                break;
            }
        }
        break;

    default:
        return DATA_OBJECT_UNAVAILABLE;
    }
    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @name：       class20_invoke_act
* @brief：      class20的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class20_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    dar = DATA_READ_WRITE_DENIED;

    return dar;
}


#endif
