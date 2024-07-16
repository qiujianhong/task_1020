/*
*********************************************************************
* @file    class9.c
* @brief： class9处理
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
#include "ac_main.h"
#include "oopStorage.h"

//
// class9
//

/**
*********************************************************************
* @brief：      获取冻结的所有记录列
* @param[in]： pRSD          - 行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 frozen_all_rcsd_get(OAD_INFO_T *pOadInfo, DB_CLIENT fd, uint16 *pOffset, uint8 *pOutData, uint16 outDataMax)
{
    int32 ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;
    uint32 i = 0;
    OOP_FROZENSETS_T data;
    NOMAL_OAD_T normal = {0};
    uint32 len = 0; 
    OOP_RCSD_T  oopRcsd;   
    
    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&oopRcsd, sizeof(oopRcsd)); 

    //读出关联OAD数据
    normal.logicId = 1;
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.oad.attID = 3;
    
    ret = db_read_nomal(fd, &normal, sizeof(OOP_FROZENSETS_T), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        ACUDP_FMT_DEBUG("frozen_all_rcsd_get failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
    }

    oopRcsd.nNum = 2 + data.nNum;
    oopRcsd.cols[0].oad.value = 0x20230200;
    oopRcsd.cols[1].oad.value = 0x20210200;

    for(i = 0; i< data.nNum; i++)
    {
        oopRcsd.cols[i+2].oad.value = data.objs[i].oad.value;
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
            ACUDP_FMT_DEBUG("apdu_rcsd_tofmt698 failed. oopRcsd.nNum(%d)\n", oopRcsd.nNum);
            return dar;        
        }
    }
    
    *pOffset = offset;
    
    return ret;
}

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
int frozen_cfg_cmp(void *vl, void *vr)
{
    OOP_FROZENOBJ_T *lvalue = (OOP_FROZENOBJ_T *)vl;
    OOP_FROZENOBJ_T *rvalue = (OOP_FROZENOBJ_T *)vr;

    if(lvalue->oad.value == rvalue->oad.value)
    {
        return 0;
    }

    return -1;
}


/**
*********************************************************************
* @name：       frozen_cfg_unit_del
* @brief：      删除一个冻结对象属性
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E frozen_cfg_unit_del(const OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_FROZENSETS_T data;
    OOP_OAD_U delID;
    CLEAR_DATA_T clearData;
    uint32 sOffset = 4;
    uint32 len = 0;
    uint32 i = 0;
    OOP_OAD_U oad = {0};

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&clearData, sizeof(clearData));     
    MEMZERO(&delID, sizeof(delID)); 

    oad.nObjID = pOadInfo->pTab->oad.nObjID;
    oad.attID = 3;

    //从报文中提取数据
    dar = basic_buf_to_data(E_OAD, inData, InDataLen, &sOffset, &delID, sizeof(delID));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(inData, InDataLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    normal.oad.value = oad.value;
    normal.logicId = 1;//交采数据

    //读取原有配置表,读不到认为已经删了,直接返回成功
    ret = db_read_nomal(gDbUdpClient, &normal, sizeof(data), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        ACUDP_FMT_TRACE("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_SUCCESS;
    }

    //找到对应的关联属性,把位于其后的分路向前递补移动
    for(i = 0; i < data.nNum; i++)
    {
        if(data.objs[i].oad.value == delID.value)
        {
            memmove(&data.objs[i], &data.objs[i+1], (data.nNum-i-1)*sizeof(OOP_FROZENOBJ_T));
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
        clearData.recordOAD.logicId = 1;//交采数据
        
        ret = db_clear_oad_data(gDbUdpClient, &clearData);
        if(ret != ERR_OK)
        {
            ACUDP_FMT_TRACE("db_clear_oad_data failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, clearData.recordOAD.logicId, clearData.recordOAD.infoNum, clearData.recordOAD.road.oadCols.oad[0].value);
            return DATA_OTHER_REASON;
        }
    }
    else
    {
        //写入数据
        normal.classtag = CLASS_DATA_INIT;
        normal.logicId = 1;//交采数据
        
        ret = db_write_nomal(gDbUdpClient, &normal, (uint8*)&data, sizeof(data));
        if(ret != ERR_OK)
        {
            ACUDP_FMT_TRACE("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
            return DATA_OTHER_REASON;
        }
    }
    
    return dar;
}

/**
*********************************************************************
* @name：       frozen_objattr_batch_add
* @brief：      批量添加冻结对象属性
* @param[in] ：fd           -数据中心句柄
               frzOAD      -冻结主OAD
               frzSets     -冻结关联OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_objattr_batch_add(DB_CLIENT fd, OOP_OAD_U frzOAD, OOP_FROZENSETS_T *frzSets)
{
    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0};
    OOP_FROZENSETS_T data;
    OOP_FROZENSETS_T subData;
    uint32 len = 0;

    MEMZERO(&data, sizeof(data)); 
    memcpy_s(&subData, sizeof(OOP_FROZENSETS_T), frzSets, sizeof(OOP_FROZENSETS_T));

    normal.oad.value = frzOAD.value;
    normal.logicId = 1;

    //写单元数据，先读取全量信息，没写就读会失败，所以不处理返回值
    ret = db_read_nomal(fd, &normal, sizeof(data), (uint8*)&data, &len);
    ACUDP_FMT_TRACE("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
        ret, normal.logicId, normal.infoNum, normal.oad.value);

    //去除data数组中与subData的重复元素
    arrdsingle(&subData.objs, (int*)&subData.nNum, &data.objs, (int*)&data.nNum, sizeof(OOP_FROZENOBJ_T), frozen_cfg_cmp);

    //未超过最大个数才添加，否则会造成数组越界
    if((data.nNum + subData.nNum) <= OOP_MAX_FROZENOBJ)
    {
        memcpy(&data.objs[data.nNum], &subData.objs, sizeof(OOP_FROZENOBJ_T)*subData.nNum);
        data.nNum += subData.nNum;
    }
    else
    {
        ACUDP_FMT_TRACE("frozen_cfg_batch_add failed, member is full. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    //写入数据
    normal.classtag = CLASS_DATA_INIT;
    normal.logicId = 1;
    
    ret = db_write_nomal(fd, &normal, (uint8*)&data, sizeof(data));
    if(ret != ERR_OK)
    {
        ACUDP_FMT_TRACE("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return ret;
}


/**
*********************************************************************
* @brief：      冻结关联对象属性单元
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
OOP_DAR_E buf_to_data_frozen_unit(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_FROZENOBJ_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));
    
    //结构类型
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //结构成员数
    if(srcBuf[sOffset++] != 3)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }  
    
    //冻结周期
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.nPeriod, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //关联对象属性
    dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.oad, sizeof(OOP_OAD_U));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //存储深度
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.nDepth, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }        

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
OOP_DAR_E buf_to_data_frozen_attr(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_FROZENSETS_T data;
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
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
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
        unitConvert.dstLen = sizeof(OOP_FROZENOBJ_T);
        unitConvert.dOffset = &unitDoffset;

        dar = buf_to_data_frozen_unit(pOadInfo, &unitConvert);
        if(dar != DATA_SUCCESS)
        {
            ACUDP_BUF_TRACE(&srcBuf[sOffset], srcLen-sOffset, "buf_to_data_frozen_unit failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
            return dar;
        }
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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


/**
*********************************************************************
* @name：       frozen_cfg_batch_add
* @brief：      批量添加冻结对象属性
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E frozen_cfg_batch_add(const OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    OOP_FROZENSETS_T subData;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    OOP_OAD_U oad = {0};

    MEMZERO(&subData, sizeof(subData)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (void *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &subData;
    convert.dstLen = sizeof(subData);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_frozen_attr(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_frozen_attr failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    oad.nObjID = pOadInfo->pTab->oad.nObjID;
    oad.attID = 3;

    ret = frozen_objattr_batch_add(gDbUdpClient, oad, &subData);
    if(ret != ERR_OK)
    {
        ACUDP_FMT_TRACE("frozen_cfg_batch_add failed. ret(%d)\n", ret);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       frozen_rectable_rec_get
* @brief：      读记录方式获取冻结记录表
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E frozen_rectable_rec_get(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    ACUDP_BUF_TRACE(inData, InDataLen, "frozen_rectable_rec_get. oad(0x%08x)\n", pOadInfo->pTab->oad.value);

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
        ACUDP_FMT_TRACE("RSD解析错误\n");
        return dar;
    }

    //报文中提取RCSD
    csdStart = offset;
    dar = apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("RCSD解析错误\n");
        return dar;
    }

    if(oopRcsd.nNum == 0)
    {
        //TODO,读所有列
        oadInfo.noParse = TRUE;
        all_rcsd_get(&oadInfo, gDbUdpClient, &innerOffset, (uint8*)&oopRcsd, sizeof(OOP_RCSD_T));
    }

    //先封装记录响应头
    read_record_response(&oadInfo, gDbUdpClient, inData, InDataLen, offset, csdStart, NULL, outData, outDataMax);

    //分配读取记录的空间
    recordInfo.oopOad = oopOad;
    recordInfo.oopRsd = oopRsd;   
    recordInfo.oopRcsd = oopRcsd;
    recordInfo.append.pOadInfo = pOadInfo;
  
    outRecord.buffer = outData->pResultData;  
    outRecord.offset = &outData->resultDataLen;
    outRecord.bufLen = outDataMax;
    
    /*读统计记录表接口，可以读出apdu数据域，可返回断点信息*/
    nRet = record_frozen_read(gDbUdpClient, &recordInfo, &outRecord);
    if (nRet != 0)
    {
        ACUDP_FMT_DEBUG("record_tmn_frozen_read failed. nRet(%d)\n", nRet);    
        return nRet;
    }
    
    return dar;
}


/**
*********************************************************************
* @name：       class9_invoke_get
* @brief：      class9的GET服务
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
OOP_DAR_E class9_invoke_get(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 2:   //冻结记录表
        {
            dar = frozen_rectable_rec_get(pOadInfo, inData, InDataLen, outData, outDataMax);
        }break;   
                
        default: 
        {
            dar = DATA_READ_WRITE_DENIED; 
        }break;
    }

    return dar;
}


