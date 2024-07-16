/*
*********************************************************************
* @file    class23.c
* @brief： class23处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"

//
// class23
//

typedef uint8 (*CTRL_SHOW_HINT)(uint8, uint8, uint8, uint8*, uint16, uint16*);
extern void appctrl_alarm_data(uint8 ctrltype, uint8 group, uint8 times, uint8 Voindex, CTRL_SHOW_HINT pfunc);
extern void set_total_del_flag(uint8 delVaild);

#if DESC("总加组基础单元D2B",1)

/**
*********************************************************************
* @brief：      总加组配置表
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
OOP_DAR_E data_to_buf_tgsets_cfg(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TGSETS_T data;
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码
    uint8 i = 0;
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
        dstBuf[dOffset++] = 3;
                
        //参与总加的分路通信地址
        dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data.tg[i].add, sizeof(OOP_TSA_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),add_len(%d)\n", i, dar, data.tg[i].add.len);
            return DATA_TYPE_UNMATCHED;
        }
        
        //总加标志
        dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.tg[i].tgFlag, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),tgFlag(%d)\n", i, dar, data.tg[i].tgFlag);
            return DATA_TYPE_UNMATCHED;
        }

        //运算符标志
        dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.tg[i].opFlag, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),opFlag(%d)\n", i, dar, data.tg[i].opFlag);
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
* @brief：      总加组控制设置状态
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
OOP_DAR_E data_to_buf_tgsets_setting(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TGSETTING_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 6;
            
    //时段控定值方案号
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.idxPeriod, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),idxPeriod(%d)\n", dar, data.idxPeriod);
        return DATA_TYPE_UNMATCHED;
    }
    
    //功控时段有效标志位
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBuf, dstLen, &dOffset, &data.pcFlag, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),pcFlag(%d)\n", dar, data.pcFlag);
        return DATA_TYPE_UNMATCHED;
    }

    //功控状态
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBuf, dstLen, &dOffset, &data.pcState, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),pcState(%d)\n", dar, data.pcState);
        return DATA_TYPE_UNMATCHED;
    }

    //电控状态
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBuf, dstLen, &dOffset, &data.ecState, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),ecState(%d)\n", dar, data.ecState);
        return DATA_TYPE_UNMATCHED;
    }

    //功控轮次
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBuf, dstLen, &dOffset, &data.pcTurn, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),pcTurn(%d)\n", dar, data.pcTurn);
        return DATA_TYPE_UNMATCHED;
    }

    //电控轮次
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBuf, dstLen, &dOffset, &data.ecTurn, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),ecTurn(%d)\n", dar, data.ecTurn);
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
* @brief：      总加组当前控制状态
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
OOP_DAR_E data_to_buf_tgsets_curstate(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TGCURSTATE_T data;
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
            
    //当前功控定值
    dar = basic_data_to_buf(E_LONG64, dstBuf, dstLen, &dOffset, &data.pwSetting, sizeof(int64));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),pwSetting(%lld)\n", dar, data.pwSetting);
        return DATA_TYPE_UNMATCHED;
    }

    //当前功率下浮控浮动系数
    dar = basic_data_to_buf(E_INTEGER, dstBuf, dstLen, &dOffset, &data.pwFall, sizeof(char));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),pwFall(%d)\n", dar, data.pwFall);
        return DATA_TYPE_UNMATCHED;
    }
    
    //功控跳闸输出状态
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBuf, dstLen, &dOffset, &data.pcTrip, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),pcTrip(%d)\n", dar, data.pcTrip);
        return DATA_TYPE_UNMATCHED;
    }

    //月电控跳闸输出状态
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBuf, dstLen, &dOffset, &data.mecTrip, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),mecTrip(%d)\n", dar, data.mecTrip);
        return DATA_TYPE_UNMATCHED;
    }

    //购电控跳闸输出状态
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBuf, dstLen, &dOffset, &data.becTrip, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),becTrip(%d)\n", dar, data.becTrip);
        return DATA_TYPE_UNMATCHED;
    }

    //功控越限告警状态
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBuf, dstLen, &dOffset, &data.pcWarn, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),pcWarn(%d)\n", dar, data.pcWarn);
        return DATA_TYPE_UNMATCHED;
    }

    //电控越限告警状态
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBuf, dstLen, &dOffset, &data.ecWarn, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),ecWarn(%d)\n", dar, data.ecWarn);
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
* @brief：      总加组单位及换算
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
OOP_DAR_E data_to_buf_tgsets_unit(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TGUNIT_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 10;
            
    //属性3单位换算
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.unit3, sizeof(OOP_SCALER_UNIT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),unit3(scaler(%d),unit(%d))\n", dar, data.unit3.scaler,data.unit3.unit);
        return DATA_TYPE_UNMATCHED;
    }
    
    //属性4单位换算
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.unit4, sizeof(OOP_SCALER_UNIT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),unit4(scaler(%d),unit(%d))\n", dar, data.unit4.scaler,data.unit4.unit);
        return DATA_TYPE_UNMATCHED;
    }

    //属性5单位换算
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.unit5, sizeof(OOP_SCALER_UNIT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),unit5(scaler(%d),unit(%d))\n", dar, data.unit5.scaler,data.unit5.unit);
        return DATA_TYPE_UNMATCHED;
    }

    //属性6单位换算
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.unit6, sizeof(OOP_SCALER_UNIT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),unit6(scaler(%d),unit(%d))\n", dar, data.unit6.scaler,data.unit6.unit);
        return DATA_TYPE_UNMATCHED;
    }

    //属性7单位换算
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.unit7, sizeof(OOP_SCALER_UNIT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),unit7(scaler(%d),unit(%d))\n", dar, data.unit7.scaler,data.unit7.unit);
        return DATA_TYPE_UNMATCHED;
    }

    //属性8单位换算
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.unit8, sizeof(OOP_SCALER_UNIT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),unit8(scaler(%d),unit(%d))\n", dar, data.unit8.scaler,data.unit8.unit);
        return DATA_TYPE_UNMATCHED;
    }

    //属性9单位换算
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.unit9, sizeof(OOP_SCALER_UNIT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),unit9(scaler(%d),unit(%d))\n", dar, data.unit9.scaler,data.unit9.unit);
        return DATA_TYPE_UNMATCHED;
    }

    //属性10单位换算
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.unit10, sizeof(OOP_SCALER_UNIT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),unit10(scaler(%d),unit(%d))\n", dar, data.unit10.scaler,data.unit10.unit);
        return DATA_TYPE_UNMATCHED;
    }

    //属性11单位换算
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.unit11, sizeof(OOP_SCALER_UNIT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),unit11(scaler(%d),unit(%d))\n", dar, data.unit11.scaler,data.unit11.unit);
        return DATA_TYPE_UNMATCHED;
    }

    //属性12单位换算
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.unit12, sizeof(OOP_SCALER_UNIT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),unit12(scaler(%d),unit(%d))\n", dar, data.unit12.scaler,data.unit12.unit);
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
* @brief：     从数据中心的数据转698格式报文(class1)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class23_henergys(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_HENERGYS_T data = {0};
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    if(pOadInfo->nIndex ==0)
    {
	    dstBuf[dOffset++] = DT_ARRAY;

	    //结构成员数
	    dstBuf[dOffset++] = data.nNum;

	    for (i = 0; i < data.nNum; i++)
	    {
	        dar = basic_data_to_buf(E_LONG64, dstBuf, dstLen, &dOffset, &data.nValue[i], sizeof(int64));
	        if(dar != DATA_SUCCESS)
	        {
	            ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.nValue[i]);
	            return DATA_TYPE_UNMATCHED;
	        }   
	    }  
    }
	else
	{
		dar = basic_data_to_buf(E_LONG64, dstBuf, dstLen, &dOffset, &data.nValue[i], sizeof(int64));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.nValue[i]);
            return DATA_TYPE_UNMATCHED;
        } 
	}
    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}


#endif

#if DESC("总加组基础单元B2D",1)

/**
*********************************************************************
* @brief：      总加组配置单元
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
OOP_DAR_E buf_to_data_tgsets_unit(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TG_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));
    
    //结构类型
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if(srcBuf[sOffset++] != 3)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //冻结周期
    dar = basic_buf_to_data(E_TSA, srcBuf, srcLen, &sOffset, &data.add, sizeof(OOP_TSA_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //关联对象属性
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.tgFlag, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //存储深度
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.opFlag, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
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
* @brief：      总加组配置表
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
OOP_DAR_E buf_to_data_tgsets_cfg(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TGSETS_T data;
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

    if(data.nNum > OOP_MAX_TGSETS)
    {
        PRTL_FMT_DEBUG("总加组成员数超限 %d > %d\n", data.nNum, OOP_MAX_TGSETS);
        return DATA_SCOPE_OF_ACCESS_VIOLATED; 
    }
    
    *unitConvert.sOffset = sOffset;
    for(i = 0; i < data.nNum; i++)
    {
        unitDoffset = 0;
        unitConvert.dstBuf = &data.tg[i];
        unitConvert.dstLen = sizeof(OOP_TG_T);
        unitConvert.dOffset = &unitDoffset;

        dar = buf_to_data_tgsets_unit(pOadInfo, &unitConvert);
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(&srcBuf[sOffset], srcLen-sOffset, "buf_to_data_tgsets_unit failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @brief：     从数据中心的数据转698格式报文(class23)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class23(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_TGSETS_T: //冻结关联对象属性表
        {
            return data_to_buf_tgsets_cfg(pOadInfo, convert);
        }break;

        case E_OOP_HENERGYS_T:
        {
            return data_to_buf_class23_henergys(pOadInfo, convert);
        }break;

        case E_LONG64:
        {
            return basic_db_to_fmt698(pOadInfo, convert);
        }break;
        
        default:
        {
            PRTL_FMT_DEBUG("data_to_buf_class23 failed. oad(0x%08x)\n", pOadInfo->pTab->oad.value);            
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
OOP_DAR_E buf_to_data_class23(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_TGSETS_T: //冻结关联对象属性表
        {
            return buf_to_data_tgsets_cfg(pOadInfo, convert);
        }break;

        default:
        {
            PRTL_FMT_DEBUG("buf_to_data_class23 failed. oad(0x%08x)\n", pOadInfo->pTab->oad.value);
        }break;        
    }
    
    return DATA_OBJECT_UNAVAILABLE;

}

#endif

#if DESC("SET服务数据接口",1)
/**
*********************************************************************
* @name：       tgsets_cfg_set
* @brief：      总加组配置表
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E tgsets_cfg_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_TGSETS_T data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (void *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_tgsets_cfg(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_tgsets_cfg failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_TGSETS_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    //设置总加组单元时，下发冻结配置
    ret = frozen_tg_objattr_add(pApduInfo->hUdp, pOadInfo->pTab->oad);
    if(ret != 0)
    {
        PRTL_FMT_DEBUG("frozen_tg_objattr_add failed. ret(%d), OI(0x%04x)\n", ret, normal.oad.nObjID);
    }

    return dar;
}

/**
*********************************************************************
* @name：       tgsets_slip_cycle_set
* @brief：      总加组滑差时间周期
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E tgsets_slip_cycle_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (void *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = basic_data_from_fmt698(E_UNSIGNED, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "basic_data_from_fmt698 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(uint8));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       tgsets_pwc_runs_set
* @brief：      总加组功/电控轮次
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E tgsets_pwc_turns_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_BITSTR8_T data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (void *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = basic_data_from_fmt698(E_OOP_BITSTR8_T, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "basic_data_from_fmt698 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_BITSTR8_T));
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
* @name：       tgsets_cfg_get
* @brief：      总加组配置表
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E tgsets_cfg_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_TGSETS_T data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 len = 0;    

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_TGSETS_T), (uint8*)&data, &len);
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
    
    dar = data_to_buf_tgsets_cfg(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_tgsets_cfg failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = dOffset;

    return dar;
}

/**
*********************************************************************
* @name：       tgsets_slip_cycle_get
* @brief：      总加组滑差时间周期
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E tgsets_slip_cycle_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 len = 0;    

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(uint8), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
        dOffset += sizeof(OOP_OAD_U);
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
    
    dar = basic_data_to_fmt698(E_UNSIGNED, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_fmt698 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = dOffset;

    return dar;
}

/**
*********************************************************************
* @name：       tgsets_pwc_runs_get
* @brief：      总加组功/电控轮次
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E tgsets_pwc_turns_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_BITSTR8_T data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 len = 0;    

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_BITSTR8_T), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
        dOffset += sizeof(OOP_OAD_U);
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
    
    dar = basic_data_to_fmt698(E_OOP_BITSTR8_T, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_fmt698 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = dOffset;

    return dar;
}

/**
*********************************************************************
* @name：       tgsets_slip_cycle_get
* @brief：      总加组功率
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E tgsets_power_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    int64 data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 len = 0;    

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(int64), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
        dOffset += sizeof(OOP_OAD_U);
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
    
    dar = basic_data_to_fmt698(E_LONG64, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_fmt698 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = dOffset;

    return dar;
}

/**
*********************************************************************
* @name：       tgsets_slip_cycle_get
* @brief：      总加组电量
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E tgsets_energy_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_HENERGYS_T data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 len = 0;    

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
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
    
    dar = data_to_buf_class23_henergys(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_fmt698 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = dOffset;

    return dar;
}

/**
*********************************************************************
* @name：       tgsets_setting_get
* @brief：      总加组控制设置状态
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E tgsets_setting_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_TGSETTING_T data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 len = 0;    

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_TGSETTING_T), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
        dOffset += sizeof(OOP_OAD_U);
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
    
    dar = data_to_buf_tgsets_setting(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_tgsets_setting failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = dOffset;

    return dar;
}

/**
*********************************************************************
* @name：       tgsets_setting_get
* @brief：      总加组当前控制状态
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E tgsets_curstate_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_TGCURSTATE_T data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 len = 0;    

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_TGCURSTATE_T), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
        dOffset += sizeof(OOP_OAD_U);
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
    
    dar = data_to_buf_tgsets_curstate(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_tgsets_curstate failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = dOffset;

    return dar;
}

/**
*********************************************************************
* @name：       tgsets_unit_get
* @brief：      总加组单位及换算
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E tgsets_unit_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_TGUNIT_T data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 len = 0;    

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_TGUNIT_T), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
        dOffset += sizeof(OOP_OAD_U);
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
    
    dar = data_to_buf_tgsets_unit(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_tgsets_unit failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = dOffset;

    return dar;
}

#endif

#if DESC("ACT服务数据接口",1)
/**
*********************************************************************
* @name：       tgsets_cfg_cmp
* @brief：      比较冻结参数是否相同
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
int tgsets_cfg_cmp(void *vl, void *vr)
{
    OOP_TG_T *lvalue = (OOP_TG_T *)vl;
    OOP_TG_T *rvalue = (OOP_TG_T *)vr;

    if(oop_tsa_equal(&lvalue->add, &rvalue->add))
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
int tgsets_cfg_add(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, OOP_TGSETS_T *pData)
{
    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0};
    uint32 len = 0;
    OOP_TGSETS_T data;    
    OOP_OAD_U oad = {0};

    MEMZERO(&data, sizeof(data)); 

    oad.nObjID = pOadInfo->pTab->oad.nObjID;
    oad.attID = 2;

    normal.oad.value = oad.value;

    //写单元数据，先读取全量信息，没写就读会失败，所以不处理返回值
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
    PRTL_FMT_TRACE("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
        ret, normal.logicId, normal.infoNum, normal.oad.value);

    //去除重复元素
    arrdsingle(&pData->tg, (int*)&pData->nNum, &data.tg, (int*)&data.nNum, sizeof(OOP_TG_T), tgsets_cfg_cmp);

    //未超过最大个数才添加，否则会造成数组越界
    if((data.nNum + pData->nNum) <= OOP_MAX_TGSETS)
    {
        memcpy(&data.tg[data.nNum], &pData->tg, sizeof(OOP_TG_T)*pData->nNum);
        data.nNum += pData->nNum;
    }
    else
    {
        PRTL_FMT_DEBUG("tgsets_cfg_add failed, member is full. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
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
* @name：       tgsets_cfg_clear_act
* @brief：      清空总加组配置表
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E tgsets_cfg_clear(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    OOP_OAD_U oad = {0};
    CLEAR_DATA_T clearData;

    MEMZERO(&clearData, sizeof(clearData)); 

    oad.nObjID = pOadInfo->pTab->oad.nObjID;
    oad.attID = 2;

    //删除OAD
    clearData.bClassTag = FALSE;
    clearData.recordOAD.classtag = CLASS_DATA_INIT;
    clearData.recordOAD.optional = 0;
    clearData.recordOAD.road.oadMain.value = 0;
    clearData.recordOAD.road.oadCols.nNum = 1;
    clearData.recordOAD.road.oadCols.oad[0].value = oad.value;
    
    ret = db_clear_oad_data(pApduInfo->hUdp, &clearData);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_clear_oad_data failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, clearData.recordOAD.logicId, clearData.recordOAD.infoNum, clearData.recordOAD.road.oadCols.oad[0].value);
        return DATA_OTHER_REASON;
    }

    //通知统计删除总加组数据
    set_total_del_flag(1<<((oad.nObjID&0xff)-1));

    ret = frozen_tg_objattr_del(pApduInfo->hUdp, pOadInfo->pTab->oad);
    if(ret != 0)
    {
        PRTL_FMT_DEBUG("frozen_tg_objattr_del failed. ret(%d), OI(0x%04x)\n", ret, pOadInfo->pTab->oad.nObjID);
    }
    ret = extreme_tg_objattr_del(pApduInfo->hUdp, pOadInfo->pTab->oad);
    if(ret != 0)
    {
        PRTL_FMT_DEBUG("extreme_tg_objattr_del failed. ret(%d), OI(0x%04x)\n", ret, pOadInfo->pTab->oad.nObjID);
    }

    return dar;
}

/**
*********************************************************************
* @name：       tgsets_cfg_unit_add
* @brief：      添加一个总加组配置单元
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E tgsets_cfg_unit_add(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    OOP_TGSETS_T subData;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;

    MEMZERO(&subData, sizeof(subData)); 
    MEMZERO(&convert, sizeof(convert)); 

    subData.nNum = 1;

    convert.srcBuf = (void *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &subData.tg[0];
    convert.dstLen = sizeof(OOP_TG_T);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_tgsets_unit(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_tgsets_cfg failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    ret = tgsets_cfg_add(pOadInfo, pApduInfo, &subData);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("tgsets_cfg_unit_add failed. ret(%d)\n", ret);
        return DATA_OTHER_REASON;
    }

    ret = frozen_tg_objattr_add(pApduInfo->hUdp, pOadInfo->pTab->oad);
    if(ret != 0)
    {
        PRTL_FMT_DEBUG("frozen_tg_objattr_add failed. ret(%d), OI(0x%04x)\n", ret, pOadInfo->pTab->oad.nObjID);
    }
    
    ret = extreme_tg_objattr_add(pApduInfo->hUdp, pOadInfo->pTab->oad);
    if(ret != 0)
    {
        PRTL_FMT_DEBUG("extreme_tg_objattr_add failed. ret(%d), OI(0x%04x)\n", ret, pOadInfo->pTab->oad.nObjID);
    }

    return dar;
}

/**
*********************************************************************
* @name：       tgsets_cfg_batch_add
* @brief：      添加一个总加组配置单元
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E tgsets_cfg_batch_add(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    OOP_TGSETS_T subData;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;

    MEMZERO(&subData, sizeof(subData)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (void *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &subData;
    convert.dstLen = sizeof(subData);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_tgsets_cfg(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_tgsets_cfg failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    ret = tgsets_cfg_add(pOadInfo, pApduInfo, &subData);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("tgsets_cfg_batch_add failed. ret(%d)\n", ret);
        return DATA_OTHER_REASON;
    }

    ret = frozen_tg_objattr_add(pApduInfo->hUdp, pOadInfo->pTab->oad);
    if(ret != 0)
    {
        PRTL_FMT_DEBUG("frozen_tg_objattr_add failed. ret(%d), OI(0x%04x)\n", ret, pOadInfo->pTab->oad.nObjID);
    }

    ret = extreme_tg_objattr_add(pApduInfo->hUdp, pOadInfo->pTab->oad);
    if(ret != 0)
    {
        PRTL_FMT_DEBUG("extreme_tg_objattr_add failed. ret(%d), OI(0x%04x)\n", ret, pOadInfo->pTab->oad.nObjID);
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
OOP_DAR_E tgsets_cfg_unit_del(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_TGSETS_T data;
    OOP_TSA_T tsa;
    CLEAR_DATA_T clearData;
    uint32 sOffset = 4;
    uint32 len = 0;
    uint32 i = 0;
    OOP_OAD_U oad = {0};

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&clearData, sizeof(clearData));     
    MEMZERO(&tsa, sizeof(tsa)); 

    oad.nObjID = pOadInfo->pTab->oad.nObjID;
    oad.attID = 2;

    //从报文中提取数据
    dar = basic_buf_to_data(E_TSA, inData, InDataLen, &sOffset, &tsa, sizeof(OOP_TSA_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(inData, InDataLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    normal.oad.value = oad.value;

    //读取原有配置表
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    //找到对应的总加分路,把位于其后的分路向前递补移动
    for(i = 0; i < data.nNum; i++)
    {
        if(oop_tsa_equal(&data.tg[i].add, &tsa))
        {
            memmove(&data.tg[i], &data.tg[i+1], (data.nNum-i-1)*sizeof(OOP_TG_T));
            data.nNum--;
            break;
        }
    }

    //如果数量为0，则删除该OAD
    if(data.nNum == 0)
    {
        clearData.recordOAD.classtag = CLASS_DATA_INIT;
        clearData.bClassTag = FALSE;
        clearData.recordOAD.optional = 0;
        clearData.recordOAD.road.oadMain.value = 0;
        clearData.recordOAD.road.oadCols.nNum = 1;
        clearData.recordOAD.road.oadCols.oad[0].value = oad.value;
        
        ret = db_clear_oad_data(pApduInfo->hUdp, &clearData);
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_clear_oad_data failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, clearData.recordOAD.logicId, clearData.recordOAD.infoNum, clearData.recordOAD.road.oadCols.oad[0].value);
            return DATA_OTHER_REASON;
        }
        
        //通知统计删除总加组数据
        set_total_del_flag(1<<((oad.nObjID&0xff)-1));

        ret = frozen_tg_objattr_del(pApduInfo->hUdp, pOadInfo->pTab->oad);
        if(ret != 0)
        {
            PRTL_FMT_DEBUG("frozen_tg_objattr_del failed. ret(%d), OI(0x%04x)\n", ret, pOadInfo->pTab->oad.nObjID);
        }
        ret = extreme_tg_objattr_del(pApduInfo->hUdp, pOadInfo->pTab->oad);
        if(ret != 0)
        {
            PRTL_FMT_DEBUG("extreme_tg_objattr_del failed. ret(%d), OI(0x%04x)\n", ret, pOadInfo->pTab->oad.nObjID);
        }
    }
    else
    {
        //写入数据
        normal.classtag = CLASS_DATA_INIT;
        
        ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_TGSETS_T));
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
            return DATA_OTHER_REASON;
        }

        ret = frozen_tg_objattr_add(pApduInfo->hUdp, pOadInfo->pTab->oad);
        if(ret != 0)
        {
            PRTL_FMT_DEBUG("frozen_tg_objattr_add failed. ret(%d), OI(0x%04x)\n", ret, pOadInfo->pTab->oad.nObjID);
        }

        ret = extreme_tg_objattr_add(pApduInfo->hUdp, pOadInfo->pTab->oad);
        if(ret != 0)
        {
            PRTL_FMT_DEBUG("extreme_tg_objattr_add failed. ret(%d), OI(0x%04x)\n", ret, pOadInfo->pTab->oad.nObjID);
        }
    }
    
    return dar;
}

#endif

#if DESC("class23对外接口函数",1)

/**
*********************************************************************
* @name：       class23_invoke_set
* @brief：      class23的SET服务
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
OOP_DAR_E class23_invoke_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 2:   //总加组配置表
        {
            dar = tgsets_cfg_set(pOadInfo,  pApduInfo, inData, inLen, outData, outDataMax);
        }break;   

        case 13:   //总加组滑差时间周期
        {
            dar = tgsets_slip_cycle_set(pOadInfo,  pApduInfo, inData, inLen, outData, outDataMax);
        }break;           

        case 14:   //总加组功控轮次配置  
        case 15:   //总加组电控轮次配置
        {
#if defined PRODUCT_ZCU_1 || defined PRODUCT_SEU
            appctrl_alarm_data(255, 0, 3, 1, NULL);
#endif
            dar = tgsets_pwc_turns_set(pOadInfo,  pApduInfo, inData, inLen, outData, outDataMax);
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
* @name：       class23_invoke_get
* @brief：      class23的GET服务
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
OOP_DAR_E class23_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 2:   //总加组配置表
        {
            dar = tgsets_cfg_get(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        }break;   
        
        case 3:   //总加组有功功率
        case 4:   //总加组无功功率
        case 5:   //总加组滑差时间内平均有功功率
        case 6:   //总加组滑差时间内平均无功功率
        case 11:  //总加组剩余电量（费）        
        case 12:  //当前功率下浮控控后总加组有功功率冻结值
        {
            //延时等待统计功能结束
            rtc_t time_rt;
            DT_Time2RTC((uint32)time(NULL), &time_rt);
            if ((time_rt.ss == 1) || (time_rt.ss == 0))
            {
                sleep(2);
            }
            dar = tgsets_power_get(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        }break;   

        case 7:   //总加组日有功电量
        case 8:   //总加组月有功电量
        case 9:   //总加组日无功电量
        case 10:  //总加组月无功电量
        {
            //延时等待统计功能结束
            rtc_t time_rt;
            DT_Time2RTC((uint32)time(NULL), &time_rt);
            if ((time_rt.ss == 1) || (time_rt.ss == 0))
            {
                sleep(2);
            }
            dar = tgsets_energy_get(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        }break; 

        case 13:   //总加组滑差时间周期
        {
            dar = tgsets_slip_cycle_get(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        }break;   

        case 14:   //总加组功控轮次配置
        case 15:   //总加组电控轮次配置
        {
            dar = tgsets_pwc_turns_get(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        }break;   

        case 16:   //总加组控制设置状态
        {
            dar = tgsets_setting_get(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        }break;   

        case 17:   //总加组当前控制状态
        {
            dar = tgsets_curstate_get(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        }break;   

        case 18:   //换算及单位
        {
            dar = tgsets_unit_get(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
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
* @name：       class23_invoke_act
* @brief：      class23的ACTION服务
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
OOP_DAR_E class23_invoke_act(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 1:   //清空总加组配置表
        {
            dar = tgsets_cfg_clear(pOadInfo,  pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        }break;   

        case 3:   //添加一个总加组配置单元
        {
            dar = tgsets_cfg_unit_add(pOadInfo,  pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        }break;           

        case 4:   //批量添加总加组配置单元
        {
            dar = tgsets_cfg_batch_add(pOadInfo,  pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        }break;   

        case 5:   //删除一个总加组配置单元
        {
            dar = tgsets_cfg_unit_del(pOadInfo,  pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        }break;   
        
        default: 
        {
            dar = DATA_READ_WRITE_DENIED; 
        }break;
    }

    return dar;
}


#endif
