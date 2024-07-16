/*
*********************************************************************
* @file    class16.c
* @brief： class16处理
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
// class16
//

#if DESC("极值统计基础单元D2B",1)

/**
*********************************************************************
* @brief：      累加平均统计结果表
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
OOP_DAR_E data_to_buf_extreme_result(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_EXTREMERES_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 5;
            
    //关联对象属性
    dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.obj, sizeof(OOP_OAD_U));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),oad(0x%08x)\n", dar, data.obj.value);
        return DATA_TYPE_UNMATCHED;
    }    

    //最大值
    dar = digit_to_fmt698(dstBuf, dstLen, &dOffset, &data.dbMaxValue, sizeof(OOP_DIGIT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("digit_to_fmt698 failed. dar(%d), choice(%d)\n", dar, data.dbMaxValue.choice);
        return DATA_TYPE_UNMATCHED;
    }

    //发生时间
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.tmMax, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),oad(0x%08x)\n", dar, data.obj.value);
        return DATA_TYPE_UNMATCHED;
    } 
    
    //最小值
    dar = digit_to_fmt698(dstBuf, dstLen, &dOffset, &data.dbMinValue, sizeof(OOP_DIGIT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("digit_to_fmt698 failed. dar(%d), choice(%d)\n", dar, data.dbMinValue.choice);
        return DATA_TYPE_UNMATCHED;
    }       

    //发生时间
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.tmMin, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),oad(0x%08x)\n", dar, data.obj.value);
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
* @brief：      累加平均统计结果表
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
OOP_DAR_E data_to_buf_extreme_results(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_EXTREMERESS_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
	uint16 i=0;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

	//结构类型
    dstBuf[dOffset++] = DT_ARRAY;
	//数量
	dstBuf[dOffset++] = data.nNum;

	for(i=0;i<data.nNum;i++)
	{
	    //结构类型
	    dstBuf[dOffset++] = DT_STRUCTURE;

	    //结构成员数
	    dstBuf[dOffset++] = 5;
	            
	    //关联对象属性
	    dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.extremvalue[i].obj, sizeof(OOP_OAD_U));
	    if(dar != DATA_SUCCESS)
	    {
	        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),oad(0x%08x)\n", dar, data.extremvalue[i].obj.value);
	        return DATA_TYPE_UNMATCHED;
	    }    

	    //最大值
	    dar = digit_to_fmt698(dstBuf, dstLen, &dOffset, &data.extremvalue[i].dbMaxValue, sizeof(OOP_DIGIT_T));
	    if(dar != DATA_SUCCESS)
	    {
	        PRTL_FMT_DEBUG("digit_to_fmt698 failed. dar(%d), choice(%d)\n", dar, data.extremvalue[i].dbMaxValue.choice);
	        return DATA_TYPE_UNMATCHED;
	    }

	    //发生时间
	    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.extremvalue[i].tmMax, sizeof(OOP_DATETIME_S_T));
	    if(dar != DATA_SUCCESS)
	    {
	        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),oad(0x%08x)\n", dar, data.extremvalue[i].obj.value);
	        return DATA_TYPE_UNMATCHED;
	    } 
	    
	    //最小值
	    dar = digit_to_fmt698(dstBuf, dstLen, &dOffset, &data.extremvalue[i].dbMinValue, sizeof(OOP_DIGIT_T));
	    if(dar != DATA_SUCCESS)
	    {
	        PRTL_FMT_DEBUG("digit_to_fmt698 failed. dar(%d), choice(%d)\n", dar, data.extremvalue[i].dbMinValue.choice);
	        return DATA_TYPE_UNMATCHED;
	    }       

	    //发生时间
	    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.extremvalue[i].tmMin, sizeof(OOP_DATETIME_S_T));
	    if(dar != DATA_SUCCESS)
	    {
	        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),oad(0x%08x)\n", dar, data.extremvalue[i].obj.value);
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


#endif

#if DESC("极值统计基础单元B2D",1)

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
OOP_DAR_E data_to_buf_class16(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_EXTREMERES_T: //区间统计结果表
        {
            return data_to_buf_extreme_result(pOadInfo, convert);
        }break;
        
        case E_OOP_COGNATES_T: //累加平均/极值统计关联对象属性表
        {
            return data_to_buf_cognates_attr(pOadInfo, convert);
        }break;

        default:
        {
            PRTL_FMT_DEBUG("data_to_buf_class16 failed. oad(0x%08x)\n", pOadInfo->pTab->oad.value);            
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
OOP_DAR_E buf_to_data_class16(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_COGNATES_T: //累加平均/极值统计关联对象属性表
        {
            return buf_to_data_cognates_attr(pOadInfo, convert);
        }break;

        default:
        {
            PRTL_FMT_DEBUG("buf_to_data_class16 failed. oad(0x%08x)\n", pOadInfo->pTab->oad.value);
        }break;        
    }
    
    return DATA_OBJECT_UNAVAILABLE;

}

#endif

#if DESC("SET服务数据接口",1)

#endif

#if DESC("GET服务数据接口",1)

#endif


#if DESC("class16对外接口函数",1)


const OOP_OAD_U g_tg2122_objs[]={//总加组日极值
    {0x23010300},

};

const OOP_OAD_U g_tg2123_objs[]={//总加组月极值
    {0x23010300},
};

const OOP_OAD_U g_pulse2122_objs[]={//脉冲日极值
    {0x24010500},
};

const OOP_OAD_U g_pulse2123_objs[]={//脉冲月极值
    {0x24010500},
};

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
int extreme_data_clear(DB_CLIENT fd, OOP_OAD_U extremeOAD)
{
    int ret = ERR_OK;
    CLEAR_DATA_T clearData;

    MEMZERO(&clearData, sizeof(clearData));
    
    //清除冻结记录
    clearData.recordOAD.classtag = CLASS_DATA_INIT;
    clearData.recordOAD.road.oadCols.nNum = 1;
    clearData.recordOAD.road.oadCols.oad[0].value = extremeOAD.value;
    clearData.recordOAD.road.oadCols.oad[0].attID = 2;
    
    ret = db_clear_oad_data(fd, &clearData);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_clear_oad_data failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, clearData.recordOAD.logicId, clearData.recordOAD.infoNum, clearData.recordOAD.road.oadCols.oad[0].value);
        return ret;
    }

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
int extreme_cfg_cmp(void *vl, void *vr)
{
    OOP_COGNATEOBJ_T *lvalue = (OOP_COGNATEOBJ_T *)vl;
    OOP_COGNATEOBJ_T *rvalue = (OOP_COGNATEOBJ_T *)vr;

    if(lvalue->obj.value == rvalue->obj.value)
    {
        return 0;
    }

    return -1;
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
int extreme_objattr_batch_add(DB_CLIENT fd, OOP_OAD_U extremeOAD, OOP_COGNATES_T *extremeSets)
{
    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0};
    OOP_COGNATES_T data;
    OOP_COGNATES_T subData;
    uint32 len = 0;

    MEMZERO(&data, sizeof(data)); 
    memcpy_s(&subData, sizeof(OOP_COGNATES_T), extremeSets, sizeof(OOP_COGNATES_T));

    normal.oad.value = extremeOAD.value;

    //写单元数据，先读取全量信息，没写就读会失败，所以不处理返回值
    ret = db_read_nomal(fd, &normal, sizeof(data), (uint8*)&data, &len);
    PRTL_FMT_TRACE("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
        ret, normal.logicId, normal.infoNum, normal.oad.value);

    //去除data数组中与subData的重复元素
    arrdsingle(&subData.objs, (int*)&subData.nNum, &data.objs, (int*)&data.nNum, sizeof(OOP_COGNATEOBJ_T), extreme_cfg_cmp);

    //未超过最大个数才添加，否则会造成数组越界
    if((data.nNum + subData.nNum) <= OOP_COGNATE_OBJS)
    {
        memcpy(&data.objs[data.nNum], &subData.objs, sizeof(OOP_COGNATEOBJ_T)*subData.nNum);
        data.nNum += subData.nNum;
    }
    else
    {
        PRTL_FMT_DEBUG("extreme_objattr_batch_add failed, member is full. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    //写入数据
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(fd, &normal, (uint8*)&data, sizeof(data));
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
* @name：       frozen_tgday_objattr_add
* @brief：      总加组日曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               tgOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int extreme_tgday_objattr_add(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_COGNATES_T extremeSets;
    const OOP_OAD_U *pObjs = g_tg2122_objs;
    OOP_OAD_U extremeOAD = {0x21220300};

    MEMZERO(&extremeSets, sizeof(extremeSets)); 

    extremeSets.nNum = sizeof(g_tg2122_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < extremeSets.nNum; i++)
    {
        extremeSets.objs[i].obj.value =  pObjs[i].value; 
        extremeSets.objs[i].obj.nObjID = tgOAD.nObjID;
        extremeSets.objs[i].period = 1; 
        extremeSets.objs[i].freq.unit = 1;
        extremeSets.objs[i].freq.value = 1;
    }

    //清除极值记录
    ret = extreme_data_clear(fd, extremeOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("extreme_data_clear failed. ret(%d), extremeOAD(0x%08x)\n", ret, extremeOAD.value);
    }

    return extreme_objattr_batch_add(fd, extremeOAD, &extremeSets);
}
/**
*********************************************************************
* @name：       frozen_tgday_objattr_add
* @brief：      总加组日曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               tgOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int extreme_tgmon_objattr_add(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_COGNATES_T extremeSets;
    const OOP_OAD_U *pObjs = g_tg2123_objs;
    OOP_OAD_U extremeOAD = {0x21230300};

    MEMZERO(&extremeSets, sizeof(extremeSets)); 

    extremeSets.nNum = sizeof(g_tg2123_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < extremeSets.nNum; i++)
    {
        extremeSets.objs[i].obj.value =  pObjs[i].value; 
        extremeSets.objs[i].obj.nObjID = tgOAD.nObjID;
        extremeSets.objs[i].period = 1; 
        extremeSets.objs[i].freq.unit = 1;
        extremeSets.objs[i].freq.value = 1;
    }

    //清除极值记录
    ret = extreme_data_clear(fd, extremeOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("extreme_data_clear failed. ret(%d), extremeOAD(0x%08x)\n", ret, extremeOAD.value);
    }

    return extreme_objattr_batch_add(fd, extremeOAD, &extremeSets);
}

/**
*********************************************************************
* @name：       extreme_objattr_add
* @brief：      配置总加组时默认添加的极值参数
* @param[in] ：fd           -数据中心句柄
               tgOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：
*********************************************************************
*/
int extreme_tg_objattr_add(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    
    ret |= extreme_tgday_objattr_add(fd, tgOAD);
    ret |= extreme_tgmon_objattr_add(fd, tgOAD);

    return ret;
}

/**
*********************************************************************
* @name：       extreme_objattr_clear
* @brief：      
* @param[in] ：fd           -数据中心句柄
               frzOAD      -冻结主OAD
               frzSets     -冻结关联OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int extreme_objattr_clear(DB_CLIENT fd, OOP_OAD_U extremeOAD)
{
    int ret = ERR_OK;
    CLEAR_DATA_T clearData;

    MEMZERO(&clearData, sizeof(clearData));
    
    //清除冻结记录
    clearData.recordOAD.classtag = CLASS_DATA_INIT;
    clearData.recordOAD.road.oadCols.nNum = 1;
    clearData.recordOAD.road.oadCols.oad[0].value = extremeOAD.value;
    
    ret = db_clear_oad_data(fd, &clearData);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_clear_oad_data failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, clearData.recordOAD.logicId, clearData.recordOAD.infoNum, clearData.recordOAD.road.oadCols.oad[0].value);
        return ret;
    }

    return ret;
}


/**
*********************************************************************
* @name：       extreme_objattr_batch_del
* @brief：      
* @param[in] ：fd           -数据中心句柄
               frzOAD      -冻结主OAD
               frzSets     -冻结关联OAD
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：
*********************************************************************
*/
int extreme_objattr_batch_del(DB_CLIENT fd, OOP_OAD_U extremeOAD, OOP_COGNATES_T *extremeSets)
{
    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0};
    OOP_COGNATES_T data;
    uint32 len = 0;
    CLEAR_DATA_T clearData;
    uint8 i = 0, j = 0;

    MEMZERO(&clearData, sizeof(clearData));
    MEMZERO(&data, sizeof(data)); 

    normal.oad.value = extremeOAD.value;

    //读取原有配置表
    ret = db_read_nomal(fd, &normal, sizeof(data), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    //找到对应的关联属性,把位于其后的分路向前递补移动
    for(i = 0; i < extremeSets->nNum; i++)
    {
        for(j = 0; j < data.nNum; j++)
        {
            if(data.objs[j].obj.value == extremeSets->objs[i].obj.value)
            {
                memmove(&data.objs[j], &data.objs[j+1], (data.nNum-j-1)*sizeof(OOP_COGNATEOBJ_T));
                data.nNum--;
                break;
            }
        }
    }
    
    //如果数量为0，则删除该OAD
    if(data.nNum == 0)
    {
        clearData.recordOAD.classtag = CLASS_DATA_INIT;
        clearData.recordOAD.road.oadCols.nNum = 1;
        clearData.recordOAD.road.oadCols.oad[0].value = extremeOAD.value;
        
        ret = extreme_objattr_clear(fd, extremeOAD);
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("extreme_objattr_clear failed. ret(%d), extremeOAD(0x%08x)\n", ret, extremeOAD.value);
            return ret;
        }
    }
    else
    {
        //写入数据
        normal.classtag = CLASS_DATA_INIT;
        
        ret = db_write_nomal(fd, &normal, (uint8*)&data, sizeof(data));
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
            return ret;
        }
    }

    return ret;
}

/**
*********************************************************************
* @name：       extreme_tgday_objattr_del
* @brief：      
* @param[in] ：fd           -数据中心句柄
               tgOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    :
* @Date      
*********************************************************************
*/
int extreme_tgday_objattr_del(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_COGNATES_T extremeSets;
    const OOP_OAD_U *pObjs = g_tg2122_objs;
    OOP_OAD_U extremeOAD = {0x21220300};

    MEMZERO(&extremeSets, sizeof(extremeSets)); 

    extremeSets.nNum = sizeof(g_tg2122_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < extremeSets.nNum; i++)
    {
        extremeSets.objs[i].obj.value =  pObjs[i].value; 
        extremeSets.objs[i].obj.nObjID = tgOAD.nObjID;
        extremeSets.objs[i].period = 1; 
        extremeSets.objs[i].freq.unit = 1;
        extremeSets.objs[i].freq.value = 1;
    }


    //清除冻结记录
    ret = extreme_data_clear(fd, extremeOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("extreme_data_clear failed. ret(%d), extremeOAD(0x%08x)\n", ret, extremeOAD.value);
    }

    return extreme_objattr_batch_del(fd, extremeOAD, &extremeSets);
}

/**
*********************************************************************
* @name：       extreme_tgmon_objattr_del
* @brief：      
* @param[in] ：fd           -数据中心句柄
               tgOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    :
* @Date      
*********************************************************************
*/
int extreme_tgmon_objattr_del(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_COGNATES_T extremeSets;
    const OOP_OAD_U *pObjs = g_tg2123_objs;
    OOP_OAD_U extremeOAD = {0x21230300};

    MEMZERO(&extremeSets, sizeof(extremeSets)); 

    extremeSets.nNum = sizeof(g_tg2123_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < extremeSets.nNum; i++)
    {
        extremeSets.objs[i].obj.value =  pObjs[i].value; 
        extremeSets.objs[i].obj.nObjID = tgOAD.nObjID;
        extremeSets.objs[i].period = 1; 
        extremeSets.objs[i].freq.unit = 1;
        extremeSets.objs[i].freq.value = 1;
    }


    //清除冻结记录
    ret = extreme_data_clear(fd, extremeOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("extreme_data_clear failed. ret(%d), extremeOAD(0x%08x)\n", ret, extremeOAD.value);
    }

    return extreme_objattr_batch_del(fd, extremeOAD, &extremeSets);
}

/**
*********************************************************************
* @name：       extreme_tg_objattr_del
* @brief：     
* @param[in] ：fd           -数据中心句柄
               tgOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：
*********************************************************************
*/
int extreme_tg_objattr_del(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    
    ret |= extreme_tgday_objattr_del(fd, tgOAD);
    ret |= extreme_tgmon_objattr_del(fd, tgOAD);

    return ret;
}

/**
*********************************************************************
* @name：       extreme_pulseday_objattr_add
* @brief：      脉冲日曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               pulseOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int extreme_pulseday_objattr_add(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_COGNATES_T extremeSets;
    const OOP_OAD_U *pObjs = g_pulse2122_objs;
    OOP_OAD_U extremeOAD = {0x21220300};

    MEMZERO(&extremeSets, sizeof(extremeSets)); 

    extremeSets.nNum = sizeof(g_pulse2122_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < extremeSets.nNum; i++)
    {
        extremeSets.objs[i].obj.value =  pObjs[i].value; 
        extremeSets.objs[i].obj.nObjID = pulseOAD.nObjID;
        extremeSets.objs[i].period = 1; 
        extremeSets.objs[i].freq.unit = 1;
        extremeSets.objs[i].freq.value = 1;
    }

    //清除极值记录
    ret = extreme_data_clear(fd, extremeOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("extreme_data_clear failed. ret(%d), extremeOAD(0x%08x)\n", ret, extremeOAD.value);
    }


    return extreme_objattr_batch_add(fd, extremeOAD, &extremeSets);
}

/**
*********************************************************************
* @name：       extreme_pulseday_objattr_add
* @brief：      脉冲日曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               pulseOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int extreme_pulsemon_objattr_add(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_COGNATES_T extremeSets;
    const OOP_OAD_U *pObjs = g_pulse2123_objs;
    OOP_OAD_U extremeOAD = {0x21230300};

    MEMZERO(&extremeSets, sizeof(extremeSets)); 

    extremeSets.nNum = sizeof(g_pulse2123_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < extremeSets.nNum; i++)
    {
        extremeSets.objs[i].obj.value =  pObjs[i].value; 
        extremeSets.objs[i].obj.nObjID = pulseOAD.nObjID;
        extremeSets.objs[i].period = 1; 
        extremeSets.objs[i].freq.unit = 1;
        extremeSets.objs[i].freq.value = 1;
    }

    //清除极值记录
    ret = extreme_data_clear(fd, extremeOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("extreme_data_clear failed. ret(%d), extremeOAD(0x%08x)\n", ret, extremeOAD.value);
    }


    return extreme_objattr_batch_add(fd, extremeOAD, &extremeSets);
}

/**
*********************************************************************
* @name：       frozen_pulse_objattr_add
* @brief：      总加组月曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               pulseOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int extreme_pulse_objattr_add(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;

    ret |= extreme_pulseday_objattr_add(fd, pulseOAD);
    ret |= extreme_pulsemon_objattr_add(fd, pulseOAD);

    return ret;
}

/**
*********************************************************************
* @name：       extreme_tgday_objattr_del
* @brief：      
* @param[in] ：fd           -数据中心句柄
               tgOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    :
* @Date      
*********************************************************************
*/
int extreme_pulseday_objattr_del(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_COGNATES_T extremeSets;
    const OOP_OAD_U *pObjs = g_pulse2122_objs;
    OOP_OAD_U extremeOAD = {0x21220300};

    MEMZERO(&extremeSets, sizeof(extremeSets)); 

    extremeSets.nNum = sizeof(g_pulse2122_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < extremeSets.nNum; i++)
    {
        extremeSets.objs[i].obj.value =  pObjs[i].value; 
        extremeSets.objs[i].obj.nObjID = pulseOAD.nObjID;
        extremeSets.objs[i].period = 1; 
        extremeSets.objs[i].freq.unit = 1;
        extremeSets.objs[i].freq.value = 1;
    }


    //清除冻结记录
    ret = extreme_data_clear(fd, extremeOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("extreme_data_clear failed. ret(%d), extremeOAD(0x%08x)\n", ret, extremeOAD.value);
    }

    return extreme_objattr_batch_del(fd, extremeOAD, &extremeSets);
}

/**
*********************************************************************
* @name：       extreme_tgday_objattr_del
* @brief：      
* @param[in] ：fd           -数据中心句柄
               tgOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    :
* @Date      
*********************************************************************
*/
int extreme_pulsemon_objattr_del(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_COGNATES_T extremeSets;
    const OOP_OAD_U *pObjs = g_pulse2123_objs;
    OOP_OAD_U extremeOAD = {0x21220300};

    MEMZERO(&extremeSets, sizeof(extremeSets)); 

    extremeSets.nNum = sizeof(g_pulse2123_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < extremeSets.nNum; i++)
    {
        extremeSets.objs[i].obj.value =  pObjs[i].value; 
        extremeSets.objs[i].obj.nObjID = pulseOAD.nObjID;
        extremeSets.objs[i].period = 1; 
        extremeSets.objs[i].freq.unit = 1;
        extremeSets.objs[i].freq.value = 1;
    }


    //清除冻结记录
    ret = extreme_data_clear(fd, extremeOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("extreme_data_clear failed. ret(%d), extremeOAD(0x%08x)\n", ret, extremeOAD.value);
    }

    return extreme_objattr_batch_del(fd, extremeOAD, &extremeSets);
}

/**
*********************************************************************
* @name：       frozen_pulse_objattr_add
* @brief：      总加组月曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               pulseOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int extreme_pulse_objattr_del(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    ret |= extreme_pulseday_objattr_del(fd, pulseOAD);
    ret |= extreme_pulsemon_objattr_del(fd, pulseOAD);

    return ret;
}


/**
*********************************************************************
* @name：       class16_invoke_set
* @brief：      class16的SET服务
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
OOP_DAR_E class16_invoke_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 3:   //关联对象属性表
        {
            dar = cognates_objattr_set(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
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
* @name：       class16_invoke_get
* @brief：      class16的GET服务
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
OOP_DAR_E class16_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 2:   //记录表
        {
            dar = stat_rectable_get(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;   
        
        case 3:   //关联对象属性表
        {
            dar = cognates_objattr_get(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
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
* @name：       class16_invoke_act
* @brief：      class16的ACTION服务
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
OOP_DAR_E class16_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}


#endif
