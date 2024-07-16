/*
*********************************************************************
* @file    class6.c
* @brief： class6处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "area.h"
#include "hal.h"
#include "power.h"
#include "adc.h"

//
// class6
//

#if DESC("基础单元D2B",1)

/*********************************************************************
* @brief：      电压合格率
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
OOP_DAR_E data_to_buf_voltqr_unit(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_VOLTQR_T data;
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
            
    //电压监测时间
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dwTime1, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),dwTime1(%d)\n", dar, data.dwTime1);
        return DATA_TYPE_UNMATCHED;
    }
    
    //电压合格率
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.wRate1, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),wRate1(%d)\n", dar, data.wRate1);
        return DATA_TYPE_UNMATCHED;
    }

    //电压超限率
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.wRate2, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),wRate2(%d)\n", dar, data.wRate2);
        return DATA_TYPE_UNMATCHED;
    }        

    //电压超上限时间
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dwTime3, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),dwTime3(%d)\n", dar, data.dwTime3);
        return DATA_TYPE_UNMATCHED;
    }    

    //电压超下限时间
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dwTime4, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),dwTime4(%d)\n", dar, data.dwTime4);
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

/*********************************************************************
* @brief：      电压合格率
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
OOP_DAR_E data_to_buf_voltqr(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_VOLTSTAT_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    DATA_CONVERT_T unitConvert;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    memcpy(&unitConvert, convert, sizeof(DATA_CONVERT_T));

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 2;
    
    unitConvert.dOffset = &dOffset;

    *unitConvert.sOffset = 0;
    unitConvert.srcBuf = &data.day;
    unitConvert.srcLen = sizeof(OOP_VOLTQR_T);

    dar = data_to_buf_voltqr_unit(pOadInfo, &unitConvert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&srcBuf[sOffset], srcLen-sOffset, "data_to_buf_voltqr_unit day failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }
    
    *unitConvert.sOffset = 0;
    unitConvert.srcBuf = &data.mon;

    dar = data_to_buf_voltqr_unit(pOadInfo, &unitConvert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&srcBuf[sOffset], srcLen-sOffset, "data_to_buf_voltqr_unit mon failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
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


/*********************************************************************
* @brief：      线损计算值
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
OOP_DAR_E data_to_buf_lnlsval(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_LNLS_UNIT_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    DATA_CONVERT_T unitConvert;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    memcpy(&unitConvert, convert, sizeof(DATA_CONVERT_T));

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 6;

        //供电量
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.eneSup, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&data.eneSup, sizeof(uint32), "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, dOffset);
        return DATA_TYPE_UNMATCHED;
    }  

    //售电量
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.eneSal, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&data.eneSal, sizeof(uint32), "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, dOffset);
        return DATA_TYPE_UNMATCHED;
    }  

    //倒送电量
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.eneDeliv, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&data.eneDeliv, sizeof(uint32), "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, dOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //分布式电源上网电量
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.eneDistrpwrnet, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&data.eneDistrpwrnet, sizeof(uint32), "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, dOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //线损率
    dar = basic_data_to_buf(E_LONG, dstBuf, dstLen, &dOffset, &data.nRatio, sizeof(int16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&data.nRatio, sizeof(int16), "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, dOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //异常标志
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.abnorFlag, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&data.abnorFlag, sizeof(uint8), "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, dOffset);
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


#endif

#if DESC("基础单元B2D",1)

/*********************************************************************
* @brief：      线损计算数据
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
OOP_DAR_E buf_to_data_lnlsval(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_LNLS_STATISVAL_T lnlsval;
    uint8 *srcBufTmp = (uint8 *)srcBuf;
    uint8 offlen = 0; //数组数量长度
    uint8 i = 0;

    //数组类型
    if(srcBufTmp[offset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check array type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    //数组成员个数
    lnlsval.nNum = get_len_offset(&srcBufTmp[offset], &offlen);
    offset += offlen;

    if(lnlsval.nNum > OOP_MAX_LNLS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check array num failed. num(%d)", lnlsval.nNum);
        return DATA_TYPE_UNMATCHED;
    }

    for(i = 0; i < lnlsval.nNum; i++)
    {
        if(srcBufTmp[offset++] != DT_STRUCTURE)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. offset(%d)", offset);
            return DATA_TYPE_UNMATCHED;
        }

        //结构成员数
        if(srcBufTmp[offset++] != 6)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct num failed. offset(%d)", offset);
            return DATA_TYPE_UNMATCHED;
        }

        //供电量
        dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBufTmp, srcLen, &offset, &lnlsval.nVal[i].eneSup, sizeof(uint32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        }  

        //售电量
        dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBufTmp, srcLen, &offset, &lnlsval.nVal[i].eneSal, sizeof(uint32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        }  

        //倒送电量
        dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBufTmp, srcLen, &offset, &lnlsval.nVal[i].eneDeliv, sizeof(uint32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        } 

        //分布式电源上网电量
        dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBufTmp, srcLen, &offset, &lnlsval.nVal[i].eneDistrpwrnet, sizeof(uint32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        } 

        //线损率
        dar = basic_buf_to_data(E_LONG, srcBufTmp, srcLen, &offset, &lnlsval.nVal[i].nRatio, sizeof(int16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        } 

        //异常标志
        dar = basic_buf_to_data(E_ENUM, srcBufTmp, srcLen, &offset, &lnlsval.nVal[i].abnorFlag, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        }         
    }
    
    memcpy(dstData, &lnlsval, dstLen);
    *pOffset = offset;
     
    return dar;
}

/*********************************************************************
* @brief：      线损计算周期
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
OOP_DAR_E buf_to_data_lnlsper(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_LNLSPER_T lnlsper;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //线损统计周期
    dar = basic_buf_to_data(E_TI, srcBufTmp, srcLen, &offset, &lnlsper.statime, sizeof(OOP_TI_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }        

    memcpy(dstData, &lnlsper, dstLen);
    *pOffset = offset;
     
    return dar;
}
#endif

#if DESC("时钟电池", 1)
/**********************************************************************
* @name      : get_capacitor_volt
* @brief     ：获取时钟电容电压
* @param[in] ：  
* @param[out]：volt: 电压，单位mV
* @return    ：0 成功， <0 失败
* @Create    : 贺宁
* @Date      ：2020-07-27
* @Update    :
**********************************************************************/
int get_rtc_baterry_volt(int32 *volt)
{
    int32 data;

    ADC_DEVICE_T* dev = (ADC_DEVICE_T*)hal_device_get(HW_DEVICE_ID_ADC);
    if(NULL == dev)
    {
        PRTL_FMT_DEBUG("dev is null\n");
        return -1;
    }

    dev->adc_volt_read(dev, EMADC_RTCVOL_CHANNEL, &data);
    PRTL_FMT_DEBUG("rtc volt value: %d\n", data);

    hal_device_release((HW_DEVICE *)dev);

    *volt = data;

    return 0;
}

/*********************************************************************
* @brief：      时钟电池
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
OOP_DAR_E data_to_buf_amr32_battary_status(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 volt = 0;
    uint32 offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    get_rtc_baterry_volt(&volt);

    //电压单位为V，换算为-2 mv/1000*100
    volt = (volt)/10;

    dstBufTmp[offset++] = DT_LONG_UNSIGNED;
    dstBufTmp[offset++] = (volt>>8)&0xFF;
    dstBufTmp[offset++] = (volt)&0xFF;
    *pOffset = offset;

    return DATA_SUCCESS;
}
#endif

#if DESC("备用电池", 1)

#if (defined PRODUCT_ECU) || (defined PRODUCT_ECU_SC)  
/**********************************************************************
* @name      : get_battery_status
* @brief     ：检查终端电池状态插拔状态
* @param[in] ：
* @param[out]：
* @return    ：插入为低0，拔掉为高1， <0 失败
* @Create    : 梅安宁
* @Date      ：2022-06-25
* @Update    :
**********************************************************************/
int battery_insert_check(uint8* status)
{
    int sta = 0;
    POWER_DEVICE_T* dev = (POWER_DEVICE_T*) hal_device_get(HW_DEVICE_ID_POWER);
    if(NULL == dev)
    {
        EVENT_FMT_DEBUG("dev is null!\n");
        return -1;
    }
    
    if(dev->power_battery_state_get(dev,&sta) < 0)
    {
        EVENT_FMT_DEBUG("set_battery_charge failed!\n");
        return -1;
    }

    hal_device_release((HW_DEVICE *)dev);

    *status = sta&0xff;
    EVENT_FMT_TRACE("battery_insert_check status %d!\n", sta);
    
    return sta;
}
#else
/**********************************************************************
* @name      : get_battery_status
* @brief     ：检查终端电池状态插拔状态
* @param[in] ：status 		0 有电池 ，1 电池被拔出
* @param[out]：
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2022-06-25
* @Update    :
**********************************************************************/
int battery_insert_check(uint8* status)
{
    int32 fd = -1;
    uint32 uData;
    int32 iRet = ERR_OK;
    
    /* open device file */
    fd = open("/dev/ym", O_RDONLY|O_NONBLOCK);

    if(fd < 0)
    {
        EVENT_FMT_DEBUG("open get_battery_status file error!\n");
        return ERR_NORMAL;
    }

    /* 读掉电检测状态 */
    if(read(fd, &uData, sizeof(uData)) 
        != sizeof(uData))
    {
       iRet = ERR_IO;
       close(fd);
       return iRet;  
    }

	*status = (uData>>8)&0x01;
    //1拔出电池 0有电池

	EVENT_FMT_TRACE(" get_battery_status [%x]!\n", uData);
	PRTL_FMT_DEBUG(" get_battery_status [%x]!\n", *status);

    close(fd);
    return iRet;    
}
#endif

/**********************************************************************
* @name      : get_bak_baterry_volt
* @brief     ：获取时钟电容电压
* @param[in] ：  
* @param[out]：volt: 电压，单位mV
* @return    ：0 成功， <0 失败
* @Create    : 贺宁
* @Date      ：2020-07-27
* @Update    :
**********************************************************************/
int get_bak_baterry_volt(int32 *volt)
{
    int32 data        = 0;
    uint8 status      = 0;

    battery_insert_check(&status);        //检查电池拔插状态
    if (status != 1)            //1为拔出状态
    {
        status = 0;
    }

    if (status== 0)         //有电池插入才读电压
    {
        ADC_DEVICE_T* dev = (ADC_DEVICE_T*)hal_device_get(HW_DEVICE_ID_ADC);
        if(NULL == dev)
        {
            PRTL_FMT_DEBUG("dev is null\n");
            return -1;
        }

        dev->adc_volt_read(dev, EMADC_BATVOL_CHANNEL, &data);

        #if defined (AREA_CHONGQING) && defined (PRODUCT_ZCU_1)
        data = data/3*3.2;
        #endif
        
        PRTL_FMT_DEBUG("bak volt value: %d\n", data);

        hal_device_release((HW_DEVICE *)dev);
        
    }

    *volt = data;

    return 0;
}

/*********************************************************************
* @brief：      时钟电池
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
OOP_DAR_E data_to_buf_bak_battary_status(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 volt = 0;
    uint32 offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    get_bak_baterry_volt(&volt);

    //电压单位为V，换算为-2 mv/1000*100
    volt = (volt)/10;

    dstBufTmp[offset++] = DT_LONG_UNSIGNED;
    dstBufTmp[offset++] = (volt>>8)&0xFF;
    dstBufTmp[offset++] = (volt)&0xFF;
    *pOffset = offset;

    return DATA_SUCCESS;
}
#endif

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class6)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class6(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
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
        PRTL_FMT_DEBUG("data_to_buf_class2 error, ERR_PTR\n");
        return ERR_PTR;
    }

    switch (pOadInfoTmp->pTab->eData)
    {
        case E_UNSIGNED:
        {
            uint8 data;
            if (dsdata->srcLen != sizeof(uint8))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy(&data, srcDataTmp, dsdata->srcLen); 
            if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &data, sizeof(uint8)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            nRet = ERR_OK;
         
        }
        break;
            
        case E_LONG_UNSIGNED:
        {
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
        case E_ENUM:
        {
            uint8 data;
            if (dsdata->srcLen != sizeof(uint8))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy(&data, srcDataTmp, dsdata->srcLen); 
            if(basic_data_to_buf(E_ENUM, dstBufTmp, dsdata->dstLen-offset, &offset, &data, sizeof(uint8)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            nRet = ERR_OK;
         
        }
        break;
        case E_DOUBLE_LONG_UNSIGNED:
            {
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
        case E_OOP_VOLTSTAT_T:
            {
                OOP_VOLTSTAT_T VOLTSTAT_T;

                //不解析，直接返回数据
                if(pOadInfo->noParse)
                {
                    memcpy((uint8*)&VOLTSTAT_T, srcDataTmp, dsdata->srcLen); 
                    switch(pOadInfo->nIndex)
                    {
                        case 0: 
                        {
                            memcpy(dstBufTmp, &VOLTSTAT_T, sizeof(VOLTSTAT_T)); 
                            offset += sizeof(VOLTSTAT_T);
                        }break;

                        case 1: 
                        {
                            memcpy(dstBufTmp, &VOLTSTAT_T.day, sizeof(OOP_VOLTQR_T)); 
                            offset += sizeof(OOP_VOLTQR_T);
                        }break;
                        case 2: 
                        {
                            memcpy(dstBufTmp, &VOLTSTAT_T.mon, sizeof(OOP_VOLTQR_T)); 
                            offset += sizeof(OOP_VOLTQR_T);
                        }break;  
                    }
                }
                else
                {
                    switch(pOadInfo->nIndex)
                    {
                        case 0:
                        {
                            nRet = data_to_buf_voltqr(pOadInfo, dsdata);
                            if(nRet != ERR_OK)
                            {
                                PRTL_FMT_LOGCC("data_to_buf_voltqr failed. nRet(%d)\n", nRet);
                                nRet = ERR_NORMAL;
                                return nRet;
                            }
                        }break;

                        case 1:
                        {
                            memcpy((uint8*)&VOLTSTAT_T, srcDataTmp, dsdata->srcLen); 

                            dsdata->srcBuf = &VOLTSTAT_T.day;
                            dsdata->srcLen = sizeof(OOP_VOLTQR_T);
                            nRet = data_to_buf_voltqr_unit(pOadInfo, dsdata);
                            if(nRet != ERR_OK)
                            {
                                PRTL_FMT_LOGCC("data_to_buf_voltqr failed. nRet(%d)\n", nRet);
                                nRet = ERR_NORMAL;
                                return nRet;
                            }
                        }
                             break;
                        case 2:    
                        {
                            memcpy((uint8*)&VOLTSTAT_T, srcDataTmp, dsdata->srcLen); 
                            dsdata->srcBuf = &VOLTSTAT_T.mon;
                            dsdata->srcLen = sizeof(OOP_VOLTQR_T);
                            nRet = data_to_buf_voltqr_unit(pOadInfo, dsdata);
                            if(nRet != ERR_OK)
                            {
                                PRTL_FMT_LOGCC("data_to_buf_voltqr failed. nRet(%d)\n", nRet);
                                nRet = ERR_NORMAL;
                                return nRet;
                            }
                        }break;                            
                    }

                    offset = *dsdata->dOffset;
                }
                
                nRet = ERR_OK;
             
            }
        break;
        case E_OOP_DWORD2_T:
            {
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
                OOP_WORD2_T WORD2_T;
                if (dsdata->srcLen != sizeof(OOP_WORD2_T))
                {
                    nRet = ERR_OPTION;
                    break;
                }
                memcpy((uint8*)&WORD2_T, srcDataTmp, dsdata->srcLen); 
                PRTL_FMT_LOGCC("WORD2_T.nValue1 = %d\n", WORD2_T.nValue1);
                PRTL_FMT_LOGCC("WORD2_T.nValue2 = %d\n", WORD2_T.nValue2);
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

//        case E_OOP_METWORD_T:
//        {
//            OOP_METWORD_T metword = {0};
//
//            //传进来的srcLen可能只是数组成员的长度,会比结构长度小，所以只有等于结构长度才拷贝
//            if (dsdata->srcLen == sizeof(OOP_METWORD_T))
//            {
//                memcpy(&metword, srcDataTmp, dsdata->srcLen); 
//            }    
//
//            dstBufTmp[offset++]= DT_ARRAY;
//            dstBufTmp[offset++]= 0x07;
//
//            for(uint32 i=0;i<OOP_METWORD_NUM;i++)
//            {
//                dstBufTmp[offset++]= DT_BIT_STRING;
//                dstBufTmp[offset++]= 0x10;
//                memcpy(&dstBufTmp[offset],&metword.metWord[i],2);
//		        offset = offset+2;
//            }
//            
//            nRet = ERR_OK;
//        }break;
        case E_OOP_METWORD_T:
        {
            ARRAY_INFO_T arrayInfo;
            OOP_METWORD_T metword = {0};

            MEMZERO(&arrayInfo, sizeof(arrayInfo));

            //传进来的srcLen可能只是数组成员的长度,会比结构长度小，所以只有等于结构长度才拷贝
            if (dsdata->srcLen == sizeof(OOP_METWORD_T))
            {
                memcpy(&metword, srcDataTmp, dsdata->srcLen); 
                pOadInfo->subParse = 0; //传入的不是子结构
            }
            else if(dsdata->srcLen == sizeof(uint16))
            {
                pOadInfo->subParse = 1; //传入的是子结构
                metword.nNum = pOadInfo->nIndex;
            }
            else
            {
                nRet = ERR_NORMAL;
                PRTL_FMT_DEBUG("oad%08X 入参长度%u错误\n", pOadInfo->pTab->oad.value, dsdata->srcLen);
                return nRet;
            }
            
            
            arrayInfo.arrLen = sizeof(OOP_METWORD_T);
            arrayInfo.pArray = metword.metWord;
            arrayInfo.maxNum = OOP_METWORD_NUM;
            arrayInfo.unitNum = metword.nNum;
            arrayInfo.unitSize = sizeof(uint16);
            arrayInfo.func = basic_db_to_fmt698;        
            
            pOadInfo->subeData = E_OOP_BITSTR16_T; //重要，基本数据一定要填子结构的数据类型
            
            return class_array_tofmt698(pOadInfo, &arrayInfo, dsdata);

        }break;

		case E_OOP_LOOPSTAT_T:
		{
		    ARRAY_INFO_T arrayInfo;
		    OOP_LOOPSTAT_T loopstat = {0};

		    MEMZERO(&arrayInfo, sizeof(arrayInfo));

		    //传进来的srcLen可能只是数组成员的长度,会比结构长度小，所以只有等于结构长度才拷贝
		    if (dsdata->srcLen == sizeof(OOP_LOOPSTAT_T))
		    {
		        memcpy(&loopstat, srcDataTmp, dsdata->srcLen); 
		    }
		    
		    arrayInfo.arrLen = sizeof(OOP_LOOPSTAT_T);
		    arrayInfo.pArray = loopstat.loopstat;
		    arrayInfo.maxNum = OOP_LOOPSTAT_NUM;
		    arrayInfo.unitNum = loopstat.nNum;
		    arrayInfo.unitSize = sizeof(uint8);
		    arrayInfo.func = basic_db_to_fmt698;        
		    
		    pOadInfo->subeData = E_ENUM; //重要，基本数据一定要填子结构的数据类型
		    
		    return class_array_tofmt698(pOadInfo, &arrayInfo, dsdata);        
		}break;
		case E_OOP_BITSTR16_T:
		{
		    OOP_BITSTR16_T BITSTR16_T;
		    if (dsdata->srcLen != sizeof(BITSTR16_T))
		    {
		        nRet = ERR_OPTION;
		        break;
		    }
		    memcpy((uint8*)&BITSTR16_T, srcDataTmp, dsdata->srcLen); 
		   
		    dstBufTmp[offset++]= DT_BIT_STRING;
		    dstBufTmp[offset++]= 16;

			membitrev(BITSTR16_T.nValue, 16);
		    memcpy(&dstBufTmp[offset],BITSTR16_T.nValue,2);
		    offset = offset+2;
		    nRet = ERR_OK;
		 
		}
		break;
		case E_OOP_BITSTR32_T:
		{
		    OOP_BITSTR32_T BITSTR32_T;
		    if (dsdata->srcLen != sizeof(BITSTR32_T))
		    {
		        nRet = ERR_OPTION;
		        break;
		    }
		    memcpy((uint8*)&BITSTR32_T, srcDataTmp, dsdata->srcLen); 
		   
		    dstBufTmp[offset++]= DT_BIT_STRING;
		    dstBufTmp[offset++]= 32;

			membitrev(BITSTR32_T.nValue, 32);
		    memcpy(&dstBufTmp[offset],BITSTR32_T.nValue,4);
		    offset = offset+4;
		    nRet = ERR_OK;
		 
		}
		break;

         case E_OOP_LONG3V_T:
        {
            ARRAY_INFO_T arrayInfo;
            OOP_LONG3V_T data = {0};

            MEMZERO(&arrayInfo, sizeof(arrayInfo));

            //传进来的srcLen可能只是数组成员的长度,会比结构长度小，所以只有等于结构长度才拷贝
            if (dsdata->srcLen == sizeof(OOP_LONG3V_T))
            {
                memcpy(&data, srcDataTmp, dsdata->srcLen); 
            }
            
            arrayInfo.arrLen = sizeof(OOP_LONG3V_T);
            arrayInfo.pArray = data.nValue;
            arrayInfo.maxNum = 3;
            arrayInfo.rsv = data.rsv;
            arrayInfo.unitNum = data.nNum;
            arrayInfo.unitSize = sizeof(int16);
            arrayInfo.func = basic_db_to_fmt698;        
            
            pOadInfo->subeData = E_LONG; //重要，基本数据一定要填子结构的数据类型
            
            return class_array_tofmt698(pOadInfo, &arrayInfo, dsdata);

        }break;     

         case E_OOP_WORD3V_T:
        {
            ARRAY_INFO_T arrayInfo;
            OOP_WORD3V_T data = {0};

            MEMZERO(&arrayInfo, sizeof(arrayInfo));

            //传进来的srcLen可能只是数组成员的长度,会比结构长度小，所以只有等于结构长度才拷贝
            if (dsdata->srcLen == sizeof(OOP_WORD3V_T))
            {
                memcpy(&data, srcDataTmp, dsdata->srcLen); 
            }
            
            arrayInfo.arrLen = sizeof(OOP_WORD3V_T);
            arrayInfo.pArray = data.nValue;
            arrayInfo.maxNum = 3;
            arrayInfo.rsv = data.rsv;
            arrayInfo.unitNum = data.nNum;
            arrayInfo.unitSize = sizeof(uint16);
            arrayInfo.func = basic_db_to_fmt698;        
            
            pOadInfo->subeData = E_LONG_UNSIGNED; //重要，基本数据一定要填子结构的数据类型
            
            return class_array_tofmt698(pOadInfo, &arrayInfo, dsdata);

        }break;            
		case E_OOP_LNLSPER_T:
		{
			    OOP_TI_T data;
                if (dsdata->srcLen != sizeof(OOP_TI_T))
                {
                    nRet = ERR_OPTION;
                    break;
                }
                memcpy(&data, srcDataTmp, dsdata->srcLen); 
                if(basic_data_to_buf(E_TI, dstBufTmp, dsdata->dstLen-offset, &offset, &data, sizeof(OOP_TI_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
                nRet = ERR_OK;
		}
		break;
		case E_OOP_LNLS_STATISVAL_T:
		{
			ARRAY_INFO_T arrayInfo;
            OOP_LNLS_STATISVAL_T data = {0};
			MEMZERO(&arrayInfo, sizeof(arrayInfo));

            //传进来的srcLen可能只是数组成员的长度,会比结构长度小，所以只有等于结构长度才拷贝
            if (dsdata->srcLen == sizeof(OOP_LNLS_STATISVAL_T))
            {
                memcpy(&data, srcDataTmp, dsdata->srcLen); 
            }
            
            arrayInfo.arrLen = sizeof(OOP_LNLS_STATISVAL_T);
            arrayInfo.pArray = data.nVal;
            arrayInfo.maxNum = 4;
            arrayInfo.rsv = data.rsv;
            arrayInfo.unitNum = data.nNum;
            arrayInfo.unitSize = sizeof(OOP_LNLS_UNIT_T);
            arrayInfo.func = data_to_buf_lnlsval;        
            
            pOadInfo->subeData = E_OOP_LNLS_UNIT_T; //重要，基本数据一定要填子结构的数据类型
            
            return class_array_tofmt698(pOadInfo, &arrayInfo, dsdata);
		}
		break;
        case E_OOP_BREAK_RUN_STATUS_T:
		{
            OOP_BREAK_RUN_STATUS_T status;
            uint8 i=0;
            if (dsdata->srcLen != sizeof(status))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy((uint8*)&status, srcDataTmp, dsdata->srcLen); 
            PRTL_FMT_LOGCC("status.nNum = %d\n", status.nNum);
         
            dstBufTmp[offset++]= DT_ARRAY;
            dstBufTmp[offset++]= status.nNum;

            for(i=0;i<status.nNum;i++)
            {
                    
                if(basic_data_to_buf(E_OOP_BITSTR8_T, dstBufTmp, dsdata->dstLen-offset, &offset,(uint8*)&status.status[i].status1, sizeof(uint8)) < 0)
                {
                    PRTL_FMT_LOGCC("status.nNum = %d\n", status.nNum);
                    nRet = ERR_NORMAL;
                    return nRet;
                }
                
                if(basic_data_to_buf(E_OOP_BITSTR8_T, dstBufTmp, dsdata->dstLen-offset, &offset,(uint8*)&status.status[i].status2, sizeof(uint8)) < 0)
                {
                    PRTL_FMT_LOGCC("status.nNum = %d\n", status.nNum);
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
            
            nRet = ERR_OK;

		}
		break;
        case E_Water_Temperature_T:
            {
                Water_Temperature_T Watertemp;
                if (dsdata->srcLen != sizeof(Watertemp))
                {
                    nRet = ERR_OPTION;
                    break;
                }
                memcpy((uint8*)&Watertemp, srcDataTmp, dsdata->srcLen); 
                PRTL_FMT_LOGCC("WORD2_T.nValue1 = %d\n", Watertemp.supplytmp);
                PRTL_FMT_LOGCC("WORD2_T.nValue2 = %d\n", Watertemp.flowtmp);
                dstBufTmp[offset++]= DT_STRUCTURE;
                dstBufTmp[offset++]= 0x02;

                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset,(uint8*)&Watertemp.supplytmp, sizeof(int32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }

                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset,(uint8*)&Watertemp.flowtmp, sizeof(int32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }

                nRet = ERR_OK;
             
            }
        break;
             case E_OOP_ST_T:
            {
                OOP_ST_T ST;
                if (dsdata->srcLen != sizeof(ST))
                {
                    nRet = ERR_OPTION;
                    break;
                }
                memcpy((uint8*)&ST, srcDataTmp, dsdata->srcLen); 
                PRTL_FMT_LOGCC("ST.nTap = %d\n", ST.nTap);
                PRTL_FMT_LOGCC("ST.nBattery = %d\n", ST.nBattery);
                dstBufTmp[offset++]= DT_STRUCTURE;
                dstBufTmp[offset++]= 0x02;

                if(basic_data_to_buf(E_ENUM, dstBufTmp, dsdata->dstLen-offset, &offset,(uint8*)&ST.nTap, sizeof(uint8)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }

                if(basic_data_to_buf(E_ENUM, dstBufTmp, dsdata->dstLen-offset, &offset,(uint8*)&ST.nBattery, sizeof(uint8)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }

                nRet = ERR_OK;
             
            }
        break;
        case E_OOP_BITSTR8_T:
        {
            OOP_BITSTR8_T data;
            if (dsdata->srcLen != sizeof(data))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy((uint8*)&data, srcDataTmp, dsdata->srcLen); 
            dstBufTmp[offset++]= DT_BIT_STRING;
            dstBufTmp[offset++]= 8;
            memcpy_bitrev(&dstBufTmp[offset], &data.nValue, 1);
            offset++;
            nRet = ERR_OK;
        }
            break;

    default: PRTL_FMT_DEBUG("data_to_buf_class6 failed.unknow eData(%d)\n", pOadInfoTmp->pTab->eData);
        nRet = ERR_OPTION;
        break;
    }

    *dsdata->dOffset = offset;

    return nRet;
}

OOP_DAR_E buf_to_data_class6(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;    
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    
    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_LNLS_STATISVAL_T://线损计算数据
        {
            dOffset += sizeof(OOP_LNLS_STATISVAL_T);
            dar = buf_to_data_lnlsval(pOadInfo, srcBuf, srcLen, &sOffset, dstBuf, dstLen);
        }break;
        
        case E_OOP_LNLSPER_T://线损统计周期
        {
            dOffset += sizeof(OOP_LNLSPER_T);
            dar = buf_to_data_lnlsper(pOadInfo, srcBuf, srcLen, &sOffset, dstBuf, dstLen);
        }break;
        
        default:                 //其它基本类型数据
            return basic_db_from_fmt698(pOadInfo, convert);
        break;
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}

#if DESC("SET接口相关函数",1)

/**
*********************************************************************
* @name：       lnlsper_period_set
* @brief：      线损统计周期设置
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
OOP_DAR_E normal_dbcenter_write(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, uint16 classtag, DATA_CONVERT_T *data)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};

    normal.logicId = pApduInfo->logicAddr;
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = classtag;

    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)data->dstBuf, *data->dOffset);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }   

    return dar;
}

#endif

#if DESC("class6对外接口函数",1)

/**
*********************************************************************
* @name：       class6_invoke_set
* @brief：      class6的SET服务
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
OOP_DAR_E class6_invoke_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class6(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    dar = normal_dbcenter_write(pOadInfo, pApduInfo, CLASS_DATA_INIT, &convert);
    if(dar != DATA_SUCCESS)
    {
        return dar;
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OAD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       class6_invoke_get
* @brief：      class6的GET服务
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
OOP_DAR_E class6_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_NORMAL;
    uint32 offset = 0;
    uint32 sourceoffset=0;
    uint32 srcLen = 0;
    int nRet = ERR_NORMAL;
    uint8 srcData[BUF_LEN_MAX] = {0};
    uint8 dstbuf[2*BUF_LEN_MAX] = {0};
    NOMAL_OAD_T NormalOad = {0};
    uint32 outLen = 0;
    DATA_CONVERT_T dsdata;
    uint32 desoffset = 0;

    if (pOdaInfo->pTab->oad.value == 0x20110200)
    {
        memcpy(&outData->pResultData[offset], inData, 4);
        offset += sizeof(OOP_OAD_U);
        outData->pResultData[offset++] = 0x01;
        
        ret = data_to_buf_amr32_battary_status(pOdaInfo, outData->pResultData, outDataMax, &offset, srcData, srcLen);
        if (ret != ERR_OK)
        {
            return DATA_READ_WRITE_DENIED;
        }

        memcpy(&outData->pResultData[offset], dsdata.dstBuf, outLen);
        outData->resultDataLen = offset;
        
        return DATA_SUCCESS;
    }

    if (pOdaInfo->pTab->oad.value == 0x20120200)
    {
        memcpy(&outData->pResultData[offset], inData, 4);
        offset += sizeof(OOP_OAD_U);
        outData->pResultData[offset++] = 0x01;
        
        ret = data_to_buf_bak_battary_status(pOdaInfo, outData->pResultData, outDataMax, &offset, srcData, srcLen);
        if (ret != ERR_OK)
        {
            return DATA_READ_WRITE_DENIED;
        }

        memcpy(&outData->pResultData[offset], dsdata.dstBuf, outLen);
        outData->resultDataLen = offset;
        
        return DATA_SUCCESS;
    }

    NormalOad.logicId = pApduInfo->logicAddr;

    if((pOdaInfo->pTab->oad.value==0x22030200)||(pOdaInfo->pTab->oad.value==0x22040200)||(pOdaInfo->pTab->oad.value==0x22000200) || (pOdaInfo->pTab->oad.value==0x20500200)
        || (pOdaInfo->pTab->oad.value==0x21310200)||(pOdaInfo->pTab->oad.value==0x21320200)||(pOdaInfo->pTab->oad.value==0x21330200))
    {
        NormalOad.logicId = 0;
    }

    if ((pOdaInfo->pTab->oad.value == 0x20140200) || 
        (pOdaInfo->pTab->oad.value == 0x200f0200) || 
        (pOdaInfo->pTab->oad.value == 0x200f0a00) || 
        (pOdaInfo->pTab->oad.value == 0x200f0b00) || 
        (pOdaInfo->pTab->oad.value == 0x200f0c00) || 
        (pOdaInfo->pTab->oad.value == 0x20260200) ||
        (pOdaInfo->pTab->oad.value == 0x20270200))
    {
        NormalOad.logicId = 1;
    }

    NormalOad.infoNum = 0;
    NormalOad.oad.value = pOdaInfo->pTab->oad.value;

    PRTL_FMT_LOGCC("class6_invoke_get pOdaInfo->pTab->oad.value = %x\n", pOdaInfo->pTab->oad.value);
    PRTL_FMT_LOGCC("class6_invoke_get NormalOad.logicId = %d\n", NormalOad.logicId);
    ret = db_read_nomal(pApduInfo->hUdp, &NormalOad, BUF_LEN_MAX, srcData, &srcLen);
    if((ret != 0)||(0==srcLen))
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, NormalOad.logicId, NormalOad.infoNum, NormalOad.oad.value);
        memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
        offset += sizeof(OOP_OAD_U);
        outData->pResultData[offset ++] = 0x01;
        outData->pResultData[offset ++] = 0x00;
        outData->resultDataLen = 6;
        return DATA_SUCCESS;
    }
    
    dsdata.srcBuf= srcData;
    dsdata.srcLen= srcLen;
    dsdata.sOffset= &sourceoffset;
    dsdata.dstBuf= dstbuf;
    dsdata.dstLen= 1000;
    dsdata.dOffset= &desoffset;
    
    PRTL_FMT_LOGCC("class5_invoke_get len is %d\n", srcLen);
    PRTL_FMT_LOGCC("class5_invoke_get pOdaInfo->pTab->oad is %x\n", pOdaInfo->pTab->oad.value);
    PRTL_FMT_LOGCC("class5_invoke_get pOdaInfo->pTab->eData is %d\n", pOdaInfo->pTab->eData);

    ret = data_to_buf_class6(pOdaInfo, &dsdata);
    outLen=*dsdata.dOffset;
    if((ret == ERR_OK) && (outLen > 0))
    {   
        if(!pOdaInfo->noParse)
        {
            memcpy(&outData->pResultData[offset], inData, 4);
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset] = 0x01;
            offset ++;
        }

        memcpy(&outData->pResultData[offset], dsdata.dstBuf, outLen);
        outData->resultDataLen = offset + outLen;
        nRet = DATA_SUCCESS;
    }
    else
    {
        nRet = DATA_READ_WRITE_DENIED;
    }

    return nRet;
}

/**
*********************************************************************
* @name：       class6_invoke_act
* @brief：      class6的ACTION服务
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
OOP_DAR_E class6_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
     uint16 outLen = 0;

     return outLen;
}


#endif
