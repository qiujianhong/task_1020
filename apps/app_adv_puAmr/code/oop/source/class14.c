/*
*********************************************************************
* @file    class14.c
* @brief： class14处理
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
// class14
//

#if DESC("区间统计基础单元D2B",1)

/**
*********************************************************************
* @brief：      区间统计关联对象属性表
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
OOP_DAR_E data_to_buf_regions_attr(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_REGIONS_T data;
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码
    uint8 i = 0;
    uint8 j = 0;
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
        dstBuf[dOffset++] = 4;
                
        //关联对象属性
        dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.objs[i].obj, sizeof(OOP_OAD_U));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oad(0x%08x)\n", i, dar, data.objs[i].obj.value);
            return DATA_TYPE_UNMATCHED;
        }

        //越限判断参数
        dstBuf[dOffset++] = DT_ARRAY;

        //数组成员个数
        dstBuf[dOffset++] = data.objs[i].nNum;

        for(j = 0; j < data.objs[i].nNum; j++)
        {
            if(data.objs[i].parm[j].choice == 0) //union，长度取两者较大
            {
                dar = digit_to_fmt698(dstBuf, dstLen, &dOffset, &data.objs[i].parm[j].value, sizeof(OOP_REGIOND_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("digit_to_fmt698 failed. [%d] dar(%d), parm[%d].choice(%d)\n", i, dar, j, data.objs[i].parm[j].value.choice);
                    return DATA_TYPE_UNMATCHED;
                }
            }
            else
            {
                dar = basic_data_to_buf(E_REGION, dstBuf, dstLen, &dOffset, &data.objs[i].parm[j].value, sizeof(OOP_REGIOND_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d), parm[%d].nType(%d)\n", i, dar, j, data.objs[i].parm[j].type);
                    return DATA_TYPE_UNMATCHED;
                }
            }
        }

        //统计周期
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.objs[i].period, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),period(%d)\n", i, dar, data.objs[i].period);
            return DATA_TYPE_UNMATCHED;
        }  

        //统计间隔
        dar = basic_data_to_buf(E_TI, dstBuf, dstLen, &dOffset, &data.objs[i].freq, sizeof(OOP_TI_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d), ti.unit(%d), ti.value(%d)\n", i, dar, data.objs[i].freq.unit, data.objs[i].freq.value);
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
* @brief：      区间统计统计结果表
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
OOP_DAR_E data_to_buf_region_result(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_REGION_T data;
    uint8 i = 0;
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
            
    //关联对象属性
    dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.obj, sizeof(OOP_OAD_U));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),oad(0x%08x)\n", dar, data.obj.value);
        return DATA_TYPE_UNMATCHED;
    }

    //区间统计值
    dstBuf[dOffset++] = DT_ARRAY;

    //数组成员个数
    dstBuf[dOffset++] = data.nNum;

    for(i = 0; i < data.nNum; i++)
    {
        //结构类型
        dstBuf[dOffset++] = DT_STRUCTURE;

        //结构成员数
        dstBuf[dOffset++] = 2;
        
        //累计时间
        dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.value[i].dwTimes, sizeof(uint32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),dwTimes(%d)\n", i, dar, data.value[i].dwTimes);
            return DATA_TYPE_UNMATCHED;
        }  
        
        //累计次数
        dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.value[i].dwCount, sizeof(uint32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),dwCount(%d)\n", i, dar, data.value[i].dwCount);
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
* @brief：      区间统计统计结果表
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
OOP_DAR_E data_to_buf_region_results(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_REGIONSS_T data;
    uint8 i = 0,j=0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

	//结构类型
    dstBuf[dOffset++] = DT_ARRAY;
	dstBuf[dOffset++] = data.nNum;

	for(j=0;j<data.nNum;j++)
	{
	    //结构类型
	    dstBuf[dOffset++] = DT_STRUCTURE;

	    //结构成员数
	    dstBuf[dOffset++] = 2;
	            
	    //关联对象属性
	    dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.regions[j].obj, sizeof(OOP_OAD_U));
	    if(dar != DATA_SUCCESS)
	    {
	        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),oad(0x%08x)\n", dar, data.regions[j].obj.value);
	        return DATA_TYPE_UNMATCHED;
	    }

	    //区间统计值
	    dstBuf[dOffset++] = DT_ARRAY;

	    //数组成员个数
	    dstBuf[dOffset++] = data.regions[j].nNum;

	    for(i = 0; i < data.regions[j].nNum; i++)
	    {
	        //结构类型
	        dstBuf[dOffset++] = DT_STRUCTURE;

	        //结构成员数
	        dstBuf[dOffset++] = 2;
	        
	        //累计时间
	        dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.regions[j].value[i].dwTimes, sizeof(uint32));
	        if(dar != DATA_SUCCESS)
	        {
	            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),dwTimes(%d)\n", i, dar, data.regions[j].value[i].dwTimes);
	            return DATA_TYPE_UNMATCHED;
	        }  
	        
	        //累计次数
	        dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.regions[j].value[i].dwCount, sizeof(uint32));
	        if(dar != DATA_SUCCESS)
	        {
	            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),dwCount(%d)\n", i, dar, data.regions[j].value[i].dwCount);
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


#endif

#if DESC("区间统计基础单元B2D",1)

/**
*********************************************************************
* @brief：      区间统计关联对象属性单元
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
OOP_DAR_E buf_to_data_regions_unit(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_REGIONOBJ_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint8 i = 0;

    MEMZERO(&data, sizeof(data));
    
    //结构类型
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //结构成员数
    if(srcBuf[sOffset++] != 4)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }  
            
    //关联对象属性
    dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.obj, sizeof(OOP_OAD_U));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //越限判断参数
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //数组成员个数
    data.nNum = srcBuf[sOffset++];
    
    for(i = 0; i < data.nNum; i++)
    {
        if(srcBuf[sOffset++] != DT_REGION) //union，长度取两者较大
        {
            data.parm[i].choice = 0;
            dar = digit_from_fmt698(srcBuf, srcLen, &sOffset, &data.parm[i].value, sizeof(OOP_REGIOND_T));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("[%d] digit_to_fmt698 failed. dar(%d), parm[%d].choice(%d)\n", i, dar, data.parm[i].value.choice);
                return DATA_TYPE_UNMATCHED;
            }
        }
        else
        {
            data.parm[i].choice = 1;       
            dar = apdu_region_fromfmt698(srcBuf, srcLen, &sOffset, &data.parm[i].value, sizeof(OOP_REGIOND_T));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("[%d] apdu_region_fromfmt698 failed. dar(%d), parm[%d].type(%d)\n", i, dar, data.parm[i].type);
                return DATA_TYPE_UNMATCHED;
            }
        }
    }
    
    //统计周期
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.period, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }        
    
    //统计间隔
    dar = basic_buf_to_data(E_TI, srcBuf, srcLen, &sOffset, &data.freq, sizeof(OOP_TI_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n",dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }          

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
* @brief：      冻结关联对象属性表
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
OOP_DAR_E buf_to_data_regions_attr(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_REGIONS_T data;
    uint8 offlen = 0;      //数组数量长度
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 unitDoffset = 0;
    DATA_CONVERT_T unitConvert;

    MEMZERO(&data, sizeof(data));
    memcpy(&unitConvert, convert, sizeof(DATA_CONVERT_T));
    
    //数组类型
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //数组成员个数
    data.nNum = get_len_offset(&srcBuf[sOffset], &offlen);
    sOffset += offlen;

    *unitConvert.sOffset = sOffset;
    for(i = 0; i < data.nNum; i++)
    {
        unitDoffset = 0;
        unitConvert.dstBuf = &data.objs[i];
        unitConvert.dstLen = sizeof(OOP_REGIONOBJ_T);
        unitConvert.dOffset = &unitDoffset;

        dar = buf_to_data_regions_unit(pOadInfo, &unitConvert);
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(&srcBuf[sOffset], srcLen-sOffset, "buf_to_data_regions_unit failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
            return dar;
        }
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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

#endif

#if DESC("数据转换总入口D2B&B2D",1)

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文
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
OOP_DAR_E data_to_buf_class14(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch(pOadInfo->pTab->eData)
    {
    	#ifdef AREA_JIANGSU
		case E_OOP_REGION_T: //区间统计结果表
        {
            if(pOadInfo->Isnormal==1)
            {
                return data_to_buf_region_result(pOadInfo, convert);
            }
            else
            {
                return data_to_buf_region_results(pOadInfo, convert);
            }
        }break;
		#else
        case E_OOP_REGION_T: //区间统计结果表
        {
            return data_to_buf_region_result(pOadInfo, convert);
        }break;
        #endif
        case E_OOP_REGIONS_T: //区间统计关联对象属性表
        {
            return data_to_buf_regions_attr(pOadInfo, convert);
        }break;

        default:
        {
            PRTL_FMT_DEBUG("buf_to_data_class14 failed. oad(0x%08x)\n", pOadInfo->pTab->oad.value);            
        }break;        
    }
    
    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @brief：      从数据中心的数据转698格式报文
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
OOP_DAR_E buf_to_data_class14(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_REGIONS_T: //区间统计关联对象属性表
        {
            return buf_to_data_regions_attr(pOadInfo, convert);
        }break;

        default:
        {
            PRTL_FMT_DEBUG("buf_to_data_class14 failed. oad(0x%08x)\n", pOadInfo->pTab->oad.value);
        }break;        
    }
    
    return DATA_OBJECT_UNAVAILABLE;

}

#endif

#if DESC("SET服务数据接口",1)
/**
*********************************************************************
* @name：       frozen_objattr_set
* @brief：      设置关联对象属性
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E region_objattr_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_REGIONS_T data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_regions_attr(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_region_attr failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_REGIONS_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

#endif

#if DESC("GET服务数据接口",1)

/**
*********************************************************************
* @name：       frozen_objattr_get
* @brief：      获取关联对象属性
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E region_objattr_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_REGIONS_T data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 len = 0;    

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_REGIONS_T), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
        dOffset += sizeof(OOP_OAD_U);
        outData->pResultData[dOffset ++] = 0x01;
        outData->pResultData[dOffset ++] = 0x01;
        outData->pResultData[dOffset ++] = 0x00;
        outData->resultDataLen += dOffset;
        return DATA_SUCCESS;
    }

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
    
    dar = data_to_buf_regions_attr(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "data_to_buf_region_attr failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = dOffset;

    return dar;
}

/**
*********************************************************************
* @brief：      获取区间统计的所有记录列
* @param[in]： pRSD          - 行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：    =0            - 成功
               <0            - 错误码
* @author    : 贺宁
* @Date      ：2020-04-23               
*********************************************************************
*/
int32 region_all_rcsd_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, uint16 *pOffset, uint8 *pOutData, uint16 outDataMax)
{
    int32 ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;
    uint32 i = 0;
    OOP_REGIONS_T data;
    NOMAL_OAD_T normal = {0};
    uint32 len = 0; 
    OOP_RCSD_T  oopRcsd;   
    
    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&oopRcsd, sizeof(oopRcsd)); 

    //读出关联OAD数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.oad.attID = 3;
    
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_REGIONS_T), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("region_all_rcsd_get failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
    }

    oopRcsd.nNum = data.nNum;

    for(i = 0; i< data.nNum; i++)
    {
        oopRcsd.cols[i].oad.value = data.objs[i].obj.value;
    }

    if(pOadInfo->noParse)
    {
        memcpy(&pOutData[offset], &oopRcsd, sizeof(oopRcsd));
        offset += sizeof(oopRcsd);
    }
    else
    {
        dar = apdu_rcsd_tofmt698(pOutData, outDataMax, &offset, &oopRcsd, sizeof(oopRcsd));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("apdu_rcsd_tofmt698 failed. oopRcsd.nNum(%d)\n", oopRcsd.nNum);
            return dar;        
        }
    }
    
    *pOffset = offset;
    
    return ret;
}


/**
*********************************************************************
* @name：       record_table_get
* @brief：      读取记录表
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E region_result_record_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E  dar = DATA_SUCCESS;
    int nRet = ERR_OK;
    uint32 offset = 0;   //输入报文buf的偏移
    uint32 csdStart = 0;
    RECORD_INFO_T recordInfo;
    BUFFER_INFO_T outRecord;
    OOP_OAD_U oopOad;
    OOP_PRSD_T oopRsd;
    OOP_RCSD_T oopRcsd;    
    uint16 innerOffset = 0;
    OAD_INFO_T oadInfo;

    //参数打印
    PRTL_BUF_TRACE(inData, InDataLen, "stat_rectable_get. oad(0x%08x)\n", pOadInfo->pTab->oad.value);

    //初始化
    MEMZERO(&recordInfo, sizeof(recordInfo));
    MEMZERO(&outRecord, sizeof(outRecord));
    MEMZERO(&oopOad, sizeof(oopOad));
    MEMZERO(&oopRsd, sizeof(oopRsd));
    MEMZERO(&oopRcsd, sizeof(oopRcsd));
    memcpy(&oadInfo, pOadInfo, sizeof(OAD_INFO_T));

    //报文中提取OAD
    memcpy_r(&oopOad, &inData[offset], sizeof(uint32));
    offset += sizeof(uint32);

    //报文中提取RSD
    dar = apdu_rsd_fromfmt698(inData, InDataLen, &offset, &oopRsd, sizeof(oopRsd));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("RSD解析错误\n");
        return dar;
    }

    //报文中提取RCSD
    csdStart = offset;
    dar = apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("RCSD解析错误\n");
        return dar;
    }

    if(oopRcsd.nNum == 0)
    {
        oadInfo.noParse = TRUE;
        region_all_rcsd_get(&oadInfo, pApduInfo, &innerOffset, (uint8*)&oopRcsd, sizeof(OOP_RCSD_T));
    }

    //先封装记录响应头
    read_record_response(&oadInfo, pApduInfo, inData, InDataLen, offset, csdStart, NULL, outData, outDataMax);

    //分配读取记录的空间
    recordInfo.oopOad = oopOad;
    recordInfo.oopRsd = oopRsd;   
    recordInfo.oopRcsd = oopRcsd;
    recordInfo.append.pOadInfo = pOadInfo;
    recordInfo.append.bpInfo = pApduInfo->bpBreakInfo;
    
    outRecord.buffer = outData->pResultData;  
    outRecord.offset = &outData->resultDataLen;
    outRecord.bufLen = pApduInfo->connectionInfo.ApplSplitLen - outData->resultDataLen - cal_excludeLen(pApduInfo->connectionInfo);
    
    /*读统计记录表接口，可以读出apdu数据域，可返回断点信息*/
    nRet = record_stat_read(pApduInfo->hUdp, &recordInfo, &outRecord);
    if (nRet != 0)
    {
        PRTL_FMT_LOGCC("record_tmn_frozen_read failed. nRet(%d)\n", nRet);    
        return DATA_READ_WRITE_DENIED;
    }
    
    return dar;
}

/**
*********************************************************************
* @name：       stat_result_normal_get
* @brief：      普通方法读统计结果
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E region_result_normal_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E  dar = DATA_SUCCESS;
    int nRet = ERR_OK;
    uint32 offset = 0;   //输入报文buf的偏移
    RECORD_INFO_T recordInfo;
    BUFFER_INFO_T outRecord;
    OOP_OAD_U oopOad;
    OOP_PRSD_T oopRsd;
    OOP_RCSD_T oopRcsd;    
    uint16 innerOffset = 0;
    OAD_INFO_T oadInfo;

    //参数打印
    PRTL_BUF_TRACE(inData, InDataLen, "region_result_normal_get. oad(0x%08x)\n", pOadInfo->pTab->oad.value);

    //初始化
    MEMZERO(&recordInfo, sizeof(recordInfo));
    MEMZERO(&outRecord, sizeof(outRecord));
    MEMZERO(&oopOad, sizeof(oopOad));
    MEMZERO(&oopRsd, sizeof(oopRsd));
    MEMZERO(&oopRcsd, sizeof(oopRcsd));
    memcpy(&oadInfo, pOadInfo, sizeof(OAD_INFO_T));

    //报文中提取OAD
    memcpy_r(&oopOad, &inData[offset], sizeof(uint32));
    offset += sizeof(uint32);

    //把OAD的属性索引转换成方法9，读上第n次记录,否则相对于使用方法0
    if(pOadInfo->nIndex != 0)
    {
        oopRsd.choice = 9;
        oopRsd.sel9.nLast = pOadInfo->nIndex;
    }
    
    //普通方式读，相当于读所有记录列
    if(oopRcsd.nNum == 0)
    {
        oadInfo.noParse = TRUE;
        region_all_rcsd_get(&oadInfo, pApduInfo, &innerOffset, (uint8*)&oopRcsd, sizeof(OOP_RCSD_T));
    }

    //先封装普通响应头
    memcpy(outData->pResultData, inData, sizeof(uint32));
    outData->resultDataLen += 4;

    outData->pResultData[outData->resultDataLen++] = 1; //[1] DATA

    recordInfo.oopOad = oopOad;
    recordInfo.oopRsd = oopRsd;   
    recordInfo.oopRcsd = oopRcsd;
    recordInfo.append.pOadInfo = pOadInfo;
    recordInfo.append.bpInfo = pApduInfo->bpBreakInfo;
    recordInfo.append.bNormal = TRUE;

    outRecord.buffer = outData->pResultData;  
    outRecord.offset = &outData->resultDataLen;
    outRecord.bufLen = pApduInfo->connectionInfo.ApplSplitLen - outData->resultDataLen - cal_excludeLen(pApduInfo->connectionInfo);
    
    /*读统计记录表接口，可以读出apdu数据域，可返回断点信息*/
    nRet = record_stat_read(pApduInfo->hUdp, &recordInfo, &outRecord);
    if (nRet != 0)
    {
        PRTL_FMT_LOGCC("record_tmn_frozen_read failed. nRet(%d)\n", nRet);    
        return DATA_READ_WRITE_DENIED;
    }
    
    return dar;
}

/**
*********************************************************************
* @name：       region_result_get
* @brief：      读取区间统计结果表
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E region_result_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    if(pOadInfo->isRec == TRUE) 
    {
        return region_result_record_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }
    else
    {
        return region_result_normal_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);        
    }

    return dar;
}

/**
*********************************************************************
* @name：       record_table_get
* @brief：      读取记录表
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E stat_rectable_get_normal(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E  dar = DATA_SUCCESS;
    int nRet = ERR_OK;
    uint32 offset = 0;   //输入报文buf的偏移
    uint32 csdStart = 0;
    RECORD_INFO_T recordInfo;
    BUFFER_INFO_T outRecord;
    OOP_OAD_U oopOad;
    OOP_PRSD_T oopRsd;
    OOP_RCSD_T oopRcsd;    

    //参数打印
    PRTL_BUF_TRACE(inData, InDataLen, "stat_rectable_get. oad(0x%08x)\n", pOadInfo->pTab->oad.value);

    //初始化
    MEMZERO(&recordInfo, sizeof(recordInfo));
    MEMZERO(&outRecord, sizeof(outRecord));
    MEMZERO(&oopOad, sizeof(oopOad));
    MEMZERO(&oopRsd, sizeof(oopRsd));
    MEMZERO(&oopRcsd, sizeof(oopRcsd));

    //报文中提取OAD
    memcpy_r(&oopOad, &inData[offset], sizeof(uint32));
    offset += sizeof(uint32);

    //报文中提取RSD
    dar = apdu_rsd_fromfmt698(inData, InDataLen, &offset, &oopRsd, sizeof(oopRsd));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("RSD解析错误\n");
        return dar;
    }

    //报文中提取RCSD
    csdStart = offset;
    dar = apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("RCSD解析错误\n");
        return dar;
    }

    if(oopRcsd.nNum == 0)
    {
        //TODO,读所有列
    }

    //先封装记录响应头
    read_record_response(pOadInfo, pApduInfo, inData, InDataLen, offset, csdStart, NULL, outData, outDataMax);

    //分配读取记录的空间
    recordInfo.oopOad = oopOad;
    recordInfo.oopRsd = oopRsd;   
    recordInfo.oopRcsd = oopRcsd;
    recordInfo.append.pOadInfo = pOadInfo;
    
    outRecord.buffer = outData->pResultData;  
    outRecord.offset = &outData->resultDataLen;
    outRecord.bufLen = pApduInfo->connectionInfo.ApplSplitLen - outData->resultDataLen - cal_excludeLen(pApduInfo->connectionInfo);
    
    /*读统计记录表接口，可以读出apdu数据域，可返回断点信息*/
    nRet = record_stat_read(pApduInfo->hUdp, &recordInfo, &outRecord);
    if (nRet < 0)
    {
        PRTL_FMT_LOGCC("record_tmn_frozen_read failed. nRet(%d)\n", nRet);    
        return DATA_READ_WRITE_DENIED;
    }
    
    return dar;
}

/**
*********************************************************************
* @name：       record_table_get
* @brief：      读取记录表
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E stat_rectable_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E  dar = DATA_SUCCESS;
    int nRet = ERR_OK;
    uint32 offset = 0;   //输入报文buf的偏移
    uint32 csdStart = 0;
    RECORD_INFO_T recordInfo;
    BUFFER_INFO_T outRecord;
    OOP_OAD_U oopOad;
    OOP_PRSD_T oopRsd;
    OOP_RCSD_T oopRcsd;    

    //参数打印
    PRTL_BUF_TRACE(inData, InDataLen, "stat_rectable_get. oad(0x%08x)\n", pOadInfo->pTab->oad.value);

    //初始化
    MEMZERO(&recordInfo, sizeof(recordInfo));
    MEMZERO(&outRecord, sizeof(outRecord));
    MEMZERO(&oopOad, sizeof(oopOad));
    MEMZERO(&oopRsd, sizeof(oopRsd));
    MEMZERO(&oopRcsd, sizeof(oopRcsd));

    //报文中提取OAD
    memcpy_r(&oopOad, &inData[offset], sizeof(uint32));
    offset += sizeof(uint32);

    //报文中提取RSD
    dar = apdu_rsd_fromfmt698(inData, InDataLen, &offset, &oopRsd, sizeof(oopRsd));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("RSD解析错误\n");
        return dar;
    }

    //报文中提取RCSD
    csdStart = offset;
    dar = apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("RCSD解析错误\n");
        return dar;
    }

    if(oopRcsd.nNum == 0)
    {
        //TODO,读所有列
    }

    //先封装记录响应头
    read_record_response(pOadInfo, pApduInfo, inData, InDataLen, offset, csdStart, NULL, outData, outDataMax);

    //分配读取记录的空间
    recordInfo.oopOad = oopOad;
    recordInfo.oopRsd = oopRsd;   
    recordInfo.oopRcsd = oopRcsd;
    recordInfo.append.pOadInfo = pOadInfo;
    
    outRecord.buffer = outData->pResultData;  
    outRecord.offset = &outData->resultDataLen;
    outRecord.bufLen = pApduInfo->connectionInfo.ApplSplitLen - outData->resultDataLen - cal_excludeLen(pApduInfo->connectionInfo);
    
    /*读统计记录表接口，可以读出apdu数据域，可返回断点信息*/
    nRet = record_stat_read(pApduInfo->hUdp, &recordInfo, &outRecord);
    if (nRet < 0)
    {
        PRTL_FMT_LOGCC("record_tmn_frozen_read failed. nRet(%d)\n", nRet);    
        return DATA_READ_WRITE_DENIED;
    }
    
    return dar;
}

#endif

#if DESC("ACT服务数据接口",1)

/**
*********************************************************************
* @name：       frozen_cfg_cmp
* @brief：      比较冻结参数是否相同
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
int region_cfg_cmp(void *vl, void *vr)
{
    OOP_REGIONOBJ_T *lvalue = (OOP_REGIONOBJ_T *)vl;
    OOP_REGIONOBJ_T *rvalue = (OOP_REGIONOBJ_T *)vr;

    if(lvalue->obj.value == rvalue->obj.value)
    {
        return 0;
    }

    return -1;
}

/**
*********************************************************************
* @name：       frozen_cfg_add
* @brief：      添加冻结参数
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
int region_cfg_add(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, OOP_REGIONS_T *pData)
{
    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0};
    uint32 len = 0;
    OOP_REGIONS_T data;    
    OOP_OAD_U oad = {0};

    MEMZERO(&data, sizeof(data)); 

    oad.nObjID = pOadInfo->pTab->oad.nObjID;
    oad.attID = 3;

    normal.oad.value = oad.value;

    //写单元数据，先读取全量信息，没写就读会失败，所以不处理返回值
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
    PRTL_FMT_TRACE("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
        ret, normal.logicId, normal.infoNum, normal.oad.value);

    //去除重复元素
    arrdsingle(&pData->objs, (int*)&pData->nNum, &data.objs, (int*)&data.nNum, sizeof(OOP_REGIONOBJ_T), region_cfg_cmp);

    //未超过最大个数才添加，否则会造成数组越界
    if((data.nNum + pData->nNum) <= OOP_REGION_OBJS)
    {
        memcpy(&data.objs[data.nNum], &pData->objs, sizeof(OOP_REGIONOBJ_T)*pData->nNum);
        data.nNum += pData->nNum;
    }
    else
    {
        PRTL_FMT_DEBUG("region_cfg_add failed, member is full. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    //写入数据
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(data));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return ret;
}

/**
*********************************************************************
* @name：       tgsets_cfg_unit_add
* @brief：      添加一个关联对象属性单元
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E region_objattr_unit_add(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    OOP_REGIONS_T subData;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;   

    MEMZERO(&subData, sizeof(subData)); 
    MEMZERO(&convert, sizeof(convert)); 

    subData.nNum = 1;
    
    convert.srcBuf = (void *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &subData.objs[0];
    convert.dstLen = sizeof(OOP_REGIONOBJ_T);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_regions_unit(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_regions_unit failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    ret = region_cfg_add(pOadInfo, pApduInfo, &subData);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("region_objattr_unit_add failed. ret(%d)\n", ret);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       tgsets_cfg_unit_del
* @brief：      删除一个总加组配置单元
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E region_objattr_unit_del(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_REGIONS_T data;
    OOP_OAD_U unitData;
    CLEAR_DATA_T clearData;
    uint32 sOffset = 4;
    uint32 len = 0;
    uint32 i = 0;
    OOP_OAD_U oad = {0};

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&clearData, sizeof(clearData));     
    MEMZERO(&unitData, sizeof(unitData)); 

    oad.nObjID = pOadInfo->pTab->oad.nObjID;
    oad.attID = 3;

    //从报文中提取数据
    dar = basic_buf_to_data(E_OAD, inData, InDataLen, &sOffset, &unitData, sizeof(unitData));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(inData, InDataLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    normal.oad.value = oad.value;

    //读取原有配置表,读不到认为已经删了,直接返回成功
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_SUCCESS;
    }

    //找到对应的关联属性,把位于其后的分路向前递补移动
    for(i = 0; i < data.nNum; i++)
    {
        if(data.objs[i].obj.value == unitData.value)
        {
            memmove(&data.objs[i], &data.objs[i+1], (data.nNum-i-1)*sizeof(unitData));
            data.nNum--;
            break;
        }
    }

    //如果数量为0，则删除该OAD
    if(data.nNum == 0)
    {
        clearData.recordOAD.classtag = CLASS_DATA_INIT;
        clearData.recordOAD.road.oadCols.nNum = 1;
        clearData.recordOAD.road.oadCols.oad[0].value = oad.value;
        
        ret = db_clear_oad_data(pApduInfo->hUdp, &clearData);
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_clear_oad_data failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, clearData.recordOAD.logicId, clearData.recordOAD.infoNum, clearData.recordOAD.road.oadCols.oad[0].value);
            return DATA_OTHER_REASON;
        }
    }
    else
    {
        //写入数据
        normal.classtag = CLASS_DATA_INIT;
        
        ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_REGIONS_T));
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
            return DATA_OTHER_REASON;
        }
    }
    
    return dar;
}

#endif


#if DESC("class14对外接口函数",1)

/**
*********************************************************************
* @name：       class14_invoke_set
* @brief：      class14的SET服务
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
OOP_DAR_E class14_invoke_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 3:   //关联对象属性表
        {
            dar = region_objattr_set(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;   
        
        default: 
        {
            dar = DATA_READ_WRITE_DENIED; 
        }break;
    }

    return dar;
}

/**
*********************************************************************
* @name：       class14_invoke_get
* @brief：      class14的GET服务
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
OOP_DAR_E class14_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 2:   //记录表
        {
            dar = region_result_get(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;   
        
        case 3:   //关联对象属性表
        {
            dar = region_objattr_get(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;   
        
        default: 
        {
            dar = DATA_READ_WRITE_DENIED; 
        }break;
    }

    return dar;
}

/**
*********************************************************************
* @name：       class14_invoke_act
* @brief：      class14的ACTION服务
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
OOP_DAR_E class14_invoke_act(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 3:   //添加一个事件关联对象属性
        {
            dar = region_objattr_unit_add(pOadInfo,  pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        }break;   
        
        case 4:   //删除一个事件关联对象属性
        {
            dar = region_objattr_unit_del(pOadInfo,  pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        }break;
        
        default: 
        {
            dar = DATA_READ_WRITE_DENIED; 
        }break;
    }

    return dar;    
}



#endif
