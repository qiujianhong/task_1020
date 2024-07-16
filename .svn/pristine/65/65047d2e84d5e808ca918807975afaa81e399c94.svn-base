/*
*********************************************************************
* @file    class3.c
* @brief： class3处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "commClass.h"
#include "oopservice.h"
#include "ac_sample.h"


//
// class3
//

/**
*********************************************************************
* @brief：      class3 分项数据
* @param[in] ： 无
* @param[out]   pDesbuffer 原始数据结构体
*               nDesLen    原始数据长度
*               OAD        分项数据标识
* @return：     >0            - 成功
               <0            - 错误码
*********************************************************************
*/
int combine_sub_data_class3_buff(uint8 *pDesbuffer, uint16 *nDesLen, uint32 OAD)
{
    switch(OAD)
    {
        /*class 3*/
        case 0x20000201:               //交采A相电压
        case 0x20000202:               //交采B相电压
        case 0x20000203:               //交采C相电压
        {
            uint8 subLabel = OAD & 0x0000000f;
            uint16 nValue = 0;

            if(subLabel == 1)
				nValue = MidTermSharedData.PhaAVolt;
			else if(subLabel == 2)
				nValue = MidTermSharedData.PhaBVolt;
			else if(subLabel == 3)
				nValue = MidTermSharedData.PhaCVolt;
            
			*nDesLen = sizeof(uint16);
       		memcpy(pDesbuffer, &nValue, sizeof(uint16));
        }
            break;

        case 0x20010201:               //交采A相电流
        case 0x20010202:               //交采B相电流
        case 0x20010203:               //交采C相电流
        {
            uint8 subLabel = OAD & 0x0000000f;
            int32 nValue = 0;
            
            if(subLabel == 1)
			{
				if(gMidPriMeasurePara.CurrentSignType.PhaAAppCurrentFlag == 1)
					nValue = 0 - MidTermSharedData.PhaACurr;
				else
					nValue = MidTermSharedData.PhaACurr;
			}
			else if(subLabel == 2)
			{
				if(gMidPriMeasurePara.CurrentSignType.PhaBAppCurrentFlag == 1)
					nValue = 0 - MidTermSharedData.PhaBCurr;
				else
					nValue = MidTermSharedData.PhaBCurr;
			}
			else if(subLabel == 3)
			{
				if(gMidPriMeasurePara.CurrentSignType.PhaCAppCurrentFlag == 1)
					nValue = 0 - MidTermSharedData.PhaCCurr;
				else
					nValue = MidTermSharedData.PhaCCurr;
			}
            
			*nDesLen = sizeof(int32);
       		memcpy(pDesbuffer, &nValue, sizeof(int32));
        }
            break;

        default:
			return -1;
            break;
    }

    return 1;
}

OOP_DAR_E buf_to_data_class3(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    return dar;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class3)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class3_oop_dword3v(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    OAD_INFO_T *pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    ARRAY_INFO_T arrayInfo;

    MEMZERO(&arrayInfo, sizeof(arrayInfo));

    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        ACUDP_FMT_TRACE("data_to_buf_class2 error, ERR_PTR\n");
        return ERR_PTR;
    }
    OOP_WORD3V_T WORD3V_T;

    //注意：传进来的srcLen可能只是数组成员的长度
    if (dsdata->srcLen == sizeof(OOP_WORD3V_T))
    {
        memcpy(&WORD3V_T, srcDataTmp, dsdata->srcLen);

    }
    
    arrayInfo.arrLen = sizeof(OOP_WORD3V_T);
    arrayInfo.pArray = WORD3V_T.nValue;
    arrayInfo.maxNum = 3;
    arrayInfo.unitNum = WORD3V_T.nNum;
    arrayInfo.unitSize = sizeof(uint16);
    arrayInfo.func = basic_db_to_fmt698;
    
    switch (pOadInfoTmp->pTab->oad.nObjID)
    {
    case 0x2000: 
    case 0x2002:
    case 0x2003:
    {
        pOadInfoTmp->subeData = E_LONG_UNSIGNED; //重要，基本数据一定要填子结构的数据类型

        nRet = class_array_tofmt698(pOadInfoTmp, &arrayInfo, dsdata);
    }
    break;
    case 0x200B:
    case 0x200C:
    {
        pOadInfoTmp->subeData = E_LONG; //重要，基本数据一定要填子结构的数据类型

        nRet = class_array_tofmt698(pOadInfoTmp, &arrayInfo, dsdata);
    }
    break;
    default:
        nRet = ERR_OPTION;
        break;
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class3)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class3_oop_elec(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;
    uint8      offlen = 0;
    uint8      lenBuf[3] = {0};
    uint32     i = 0;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;
    OOP_INT3V_T data;

    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        ACUDP_FMT_TRACE("data_to_buf_class3_oop_elec error, ERR_PTR\n");
        return ERR_PTR;
    }

    MEMZERO(&data, sizeof(data));
    if (dsdata->srcLen != sizeof(OOP_INT3V_T))
    {
        ACUDP_FMT_TRACE("data_to_buf_class3_oop_elec error, ERR_OPTION\n");
        return ERR_OPTION;
    }
    
    memcpy(&data, srcDataTmp, dsdata->srcLen);
    offlen = set_len_offset(3, lenBuf);
    dstBufTmp[offset] = pOadInfoTmp->pTab->dataType;
    offset ++;
    memcpy(&dstBufTmp[offset], lenBuf, offlen); 
    offset += offlen;
    
    switch (pOadInfoTmp->pTab->oad.nObjID)
    {
    case 0x2001:
    {
        for (i = 0; i < 3; i++)
        {
            if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dsdata->dstLen-offset, &offset, &data.nValue[i], sizeof(int32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
        }
        nRet = ERR_OK;
    }
    break;
    default:
        nRet = ERR_OPTION;
        break;
    }

    *dsdata->dOffset = offset;

    return nRet;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class3)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class3_base_data(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;

    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        ACUDP_FMT_TRACE("data_to_buf_class3_base_data() 入参检查 error, ERR_PTR\n");
        return ERR_PTR;
    }

    switch (pOadInfoTmp->pTab->eData)
    {
        case E_LONG_UNSIGNED:
        {
            ACUDP_FMT_TRACE("data_to_buf_class3_base_data() case E_LONG_UNSIGNED: \n");
            uint16 data[2];
            if (dsdata->srcLen != sizeof(uint16))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy(data, srcDataTmp, dsdata->srcLen); 
            if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, data, sizeof(uint16)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            nRet = ERR_OK;
         
        }
            break;
        case E_LONG:
        {
            ACUDP_FMT_TRACE("data_to_buf_class3_base_data() case E_LONG: \n");
            int16 data[2];
            if (dsdata->srcLen != sizeof(uint16))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy(data, srcDataTmp, dsdata->srcLen); 
            if(basic_data_to_buf(E_LONG, dstBufTmp, dsdata->dstLen-offset, &offset, data, sizeof(int16)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            nRet = ERR_OK;
         
        }
            break;
        case E_DOUBLE_LONG_UNSIGNED:
        {
            ACUDP_FMT_TRACE("data_to_buf_class3_base_data() case E_DOUBLE_LONG_UNSIGNED: \n");
            uint32 data[4];
            if (dsdata->srcLen != sizeof(uint32))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy(data, srcDataTmp, dsdata->srcLen); 
            if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, data, sizeof(uint32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            nRet = ERR_OK;
         
        }
            break;
        case E_DOUBLE_LONG:
        {
            ACUDP_FMT_TRACE("data_to_buf_class3_base_data() case E_DOUBLE_LONG: \n");
            int32 data[4];
            if (dsdata->srcLen != sizeof(int32))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy(data, srcDataTmp, dsdata->srcLen); 
            if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dsdata->dstLen-offset, &offset, data, sizeof(int32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            nRet = ERR_OK;
         
        }
            break;
        case E_OOP_DWORD2_T:
        {
            ACUDP_FMT_TRACE("data_to_buf_class3_base_data() case E_OOP_DWORD2_T: \n");
            OOP_DWORD2_T DWORD2_T;
            if (dsdata->srcLen != sizeof(OOP_DWORD2_T))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy((uint8*)&DWORD2_T, srcDataTmp, dsdata->srcLen); 
            dstBufTmp[offset++]= DT_STRUCTURE;
            dstBufTmp[offset++]= 0x02;

            if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset,(uint8*)&DWORD2_T.nValue1, sizeof(uint32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }

            if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset,(uint8*)&DWORD2_T.nValue2, sizeof(uint32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }

            nRet = ERR_OK;
         
        }
            break;
        case E_OOP_WORD2_T:
        {
            ACUDP_FMT_TRACE("data_to_buf_class3_base_data() case E_OOP_WORD2_T: \n");
            OOP_WORD2_T WORD2_T;
            if (dsdata->srcLen != sizeof(OOP_WORD2_T))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy((uint8*)&WORD2_T, srcDataTmp, dsdata->srcLen); 
            ACUDP_FMT_LOGCC("WORD2_T.nValue1 = %d\n", WORD2_T.nValue1);
            ACUDP_FMT_LOGCC("WORD2_T.nValue2 = %d\n", WORD2_T.nValue2);
            dstBufTmp[offset++]= DT_STRUCTURE;
            dstBufTmp[offset++]= 0x02;

            if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset,(uint8*)&WORD2_T.nValue1, sizeof(uint16)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }

            if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset,(uint8*)&WORD2_T.nValue2, sizeof(uint16)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }

            nRet = ERR_OK;
         
        }
            break;
        case E_OOP_METWORD_T:
        {
            ACUDP_FMT_TRACE("data_to_buf_class3_base_data() case E_OOP_METWORD_T: \n");
            ARRAY_INFO_T arrayInfo;
            OOP_METWORD_T metword = {0};

            MEMZERO(&arrayInfo, sizeof(arrayInfo));

            //传进来的srcLen可能只是数组成员的长度,会比结构长度小，所以只有等于结构长度才拷贝
            if (dsdata->srcLen == sizeof(OOP_METWORD_T))
            {
                memcpy(&metword, srcDataTmp, dsdata->srcLen); 
            }
            
            arrayInfo.arrLen = sizeof(OOP_METWORD_T);
            arrayInfo.pArray = metword.metWord;
            arrayInfo.maxNum = OOP_METWORD_NUM;
            arrayInfo.unitNum = metword.nNum;
            arrayInfo.unitSize = sizeof(uint16);
            arrayInfo.func = basic_db_to_fmt698;

            //dsdata->dOffset = &offset;  //错误:修改了dsdata->dOffset的值
            
            pOadInfo->subeData = E_OOP_BITSTR16_T; //重要，基本数据一定要填子结构的数据类型
            
            nRet = class_array_tofmt698(pOadInfo, &arrayInfo, dsdata);
            return nRet;

        }
            break;
        
        default: ACUDP_FMT_TRACE("data_to_buf_class3_base_data failed.unknow eData(%d)\n", pOadInfoTmp->pTab->eData);
            nRet = ERR_OPTION;
            break;
    }

    *dsdata->dOffset = offset;

    return nRet;
}


/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class3)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class3(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    pOadInfoTmp = pOadInfo;


    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        ACUDP_FMT_TRACE("data_to_buf_class3() 入参检查 error, ERR_PTR\n");
        return ERR_PTR;
    }
    
    switch (pOadInfoTmp->pTab->oad.nObjID)
    {
	    case 0x2000: 
	    case 0x2002:
	    case 0x2003:
	    case 0x200B:
	    case 0x200C:
            if(0 == pOadInfoTmp->pTab->oad.nIndex)
            {
                ACUDP_FMT_TRACE("data_to_buf_class3() data_to_buf_class3_oop_dword3v() ... \n");
	            return data_to_buf_class3_oop_dword3v(pOadInfo, dsdata);
            }
            else
            {
                ACUDP_FMT_TRACE("data_to_buf_class3() data_to_buf_class3_base_data() ... \n");
	            return data_to_buf_class3_base_data(pOadInfo, dsdata);
            }
	    	break;
	    case 0x2001:
            if(0 == pOadInfoTmp->pTab->oad.nIndex)
            {
                ACUDP_FMT_TRACE("data_to_buf_class3() data_to_buf_class3_oop_elec() ... \n");
	            return data_to_buf_class3_oop_elec(pOadInfo, dsdata);
            }
            else
            {
                ACUDP_FMT_TRACE("data_to_buf_class3() data_to_buf_class3_base_data() ... \n");
	            return data_to_buf_class3_base_data(pOadInfo, dsdata);
            }
	    	break;
	    default:
	        nRet = ERR_OPTION;
	        break;
    }

    return nRet;
}


