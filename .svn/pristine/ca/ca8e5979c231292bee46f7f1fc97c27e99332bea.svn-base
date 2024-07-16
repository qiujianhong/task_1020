/*
*********************************************************************
* @file    class25.c
* @brief： class25处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "appmsg_struct.h"
#include "appmsg_split.h"

extern void comm_set_rediag(uint8 id);

//
// class25
//
/**
*********************************************************************
* @brief：      公网通信工作模式
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_work_mode(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    int32 nRet = ERR_NORMAL;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    //OAD:45000201
    if(basic_data_to_buf(DT_ENUM, &dstBufTmp[offset], dstLen-offset, &offset, srcData, 1) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    } 
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      公网通信在线方式
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_line_type(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    int32 nRet = ERR_NORMAL;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    //OAD:45000202
    if(basic_data_to_buf(DT_ENUM, &dstBufTmp[offset], dstLen-offset, &offset, srcData, 1) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    } 
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      公网通信连接方式
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_connect_type(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    int32 nRet = ERR_NORMAL;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    //OAD:45000203
    if(basic_data_to_buf(DT_ENUM, &dstBufTmp[offset], dstLen-offset, &offset, srcData, 1) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    } 
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      公网通信连接应用方式
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_connect_mode(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    int32 nRet = ERR_NORMAL;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    //OAD:45000204
    if(basic_data_to_buf(DT_ENUM, &dstBufTmp[offset], dstLen-offset, &offset, srcData, 1) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    } 
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      公网通信连接应用方式
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_listen_port_list(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    int32 nRet = ERR_NORMAL;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    //OAD:45000205
    if(basic_data_to_buf(DT_ENUM, &dstBufTmp[offset], dstLen-offset, &offset, srcData, 1) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    } 
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      获取公网通信的通信配置
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_gprs_config(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    int32 nRet = ERR_NORMAL;
    uint32     offset = *pOffset;
    uint32     i=0;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_GPRSCONFIG_T GPRSCONFIG_T;
    
    memcpy((uint8*)&GPRSCONFIG_T, srcData, srcLen);
    dstBufTmp[offset++] = DT_STRUCTURE;
    dstBufTmp[offset++] = 0x0C;
    //工作模式
    if(basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &GPRSCONFIG_T.workmode, 1) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    } 
    //在线方式
    if(basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &GPRSCONFIG_T.linetype, 1) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    } 
    //连接方式
    if(basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &GPRSCONFIG_T.contype, 1) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    } 
    //连接应用方式
    if(basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &GPRSCONFIG_T.conmode, 1) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    } 
    //侦听端口列表
    dstBufTmp[offset++] = DT_ARRAY;
    dstBufTmp[offset++] = GPRSCONFIG_T.listenNum;
    for(i=0;i<GPRSCONFIG_T.listenNum;i++)
    {
        if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &GPRSCONFIG_T.listen[i], 2) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        } 
    }

    //APN
    if(basic_data_to_buf(E_VISIBLE_STRING, dstBufTmp, dstLen-offset, &offset, &GPRSCONFIG_T.apn.apn, sizeof(OOP_VISIBLEVAR_T)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    //用户名
    if(basic_data_to_buf(E_VISIBLE_STRING, dstBufTmp, dstLen-offset, &offset, &GPRSCONFIG_T.apn.username, sizeof(OOP_VISIBLEVAR_T)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //密码
    if(basic_data_to_buf(E_VISIBLE_STRING, dstBufTmp, dstLen-offset, &offset, &GPRSCONFIG_T.apn.pwd, sizeof(OOP_VISIBLEVAR_T)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //代理服务器
    dstBufTmp[offset++] = DT_OCTET_STRING;
    dstBufTmp[offset++] = 0x04;
    memcpy(dstBufTmp+offset,GPRSCONFIG_T.apn.proxyip,4);
    offset=offset+4;

    //代理端口
    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &GPRSCONFIG_T.apn.proxyport, 2) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //超时时间及重发次数
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &GPRSCONFIG_T.dail.body, 1) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    //心跳周期
    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &GPRSCONFIG_T.heart, 2) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    *pOffset=offset;
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      获取公网通信的主站通信参数表
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_master_commu_param(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen, uint32 nindex)
{   
    int32 nRet = ERR_NORMAL;
    uint32     offset = *pOffset;
    uint32     i=0;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_MASTERPARAMS_T MASTERPARAMS_T;
    uint32 nNum = nindex;
    memcpy((uint8*)&MASTERPARAMS_T, srcData, srcLen);
    if(!nNum)
    {
        nNum = MASTERPARAMS_T.nNum;
        i = 0;
        dstBufTmp[offset++] = DT_ARRAY;
        dstBufTmp[offset++] = nNum;
    }
    else
        i = nNum-1;
    for(;i<nNum;i++)
    {
        dstBufTmp[offset++] = DT_STRUCTURE;
        dstBufTmp[offset++] = 2;
        //IP地址
        dstBufTmp[offset++] = DT_OCTET_STRING;
        dstBufTmp[offset++] = 0x04;
        memcpy(dstBufTmp+offset,MASTERPARAMS_T.master[i].ip,4);
        offset=offset+4;
        //端口
        if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &MASTERPARAMS_T.master[i].port, 2) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }
    }
    *pOffset=offset;
    return ERR_OK;
}


/**
*********************************************************************
* @brief：      获取公网通信的短信通信参数
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_sms_commu_param(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    
    uint32     offset = *pOffset;
    uint32     i = 0;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_SMS_T SMS_T;
    uint32 len = 0;

    if(srcLen < sizeof(OOP_SMS_T))
    {
        return ERR_NORMAL;
    }
    memset((uint8*)&SMS_T,0x00,sizeof(OOP_SMS_T));
    memcpy((uint8*)&SMS_T, srcData, srcLen);
    dstBufTmp[offset++] = DT_STRUCTURE;
    dstBufTmp[offset++] = 3;
    dstBufTmp[offset++] = DT_VISIBLE_STRING;

    //短信中心号码
    len = strlen((char*)SMS_T.centernum);
    dstBufTmp[offset++] = len;
    strcpy((char*)&dstBufTmp[offset], SMS_T.centernum);
    offset = offset + len;

    //主站号码
    dstBufTmp[offset++] = DT_ARRAY;
    dstBufTmp[offset++] = SMS_T.hostnum.nNum;
    for(i = 0; i < SMS_T.hostnum.nNum; i++)
    {
        len = strlen((char*)SMS_T.hostnum.item[i].value);
        dstBufTmp[offset++] = DT_VISIBLE_STRING;
        dstBufTmp[offset++] = len;
        strcpy((char*)&dstBufTmp[offset], SMS_T.hostnum.item[i].value);
        offset = offset + len;
    }

    //短信通知目的号码
    dstBufTmp[offset++] = DT_ARRAY;
    dstBufTmp[offset++] = SMS_T.localnum.nNum;
    for(i = 0; i < SMS_T.localnum.nNum; i++)
    {
        len = strlen((char*)SMS_T.localnum.item[i].value);
        dstBufTmp[offset++] = DT_VISIBLE_STRING;
        dstBufTmp[offset++] = len;
        strcpy((char*)&dstBufTmp[offset], SMS_T.localnum.item[i].value);
        offset = offset + len;
    }
    
    *pOffset = offset;
    return ERR_OK;
}


/**
*********************************************************************
* @brief：      获取版本信息
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_version_info(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    
    uint32     offset = *pOffset;
    
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_VERSION_T VERSION_T;
    
    memcpy((uint8*)&VERSION_T, srcData, srcLen);
    dstBufTmp[offset++] = DT_STRUCTURE;
    dstBufTmp[offset++] = 6;
    //厂商代码
    dstBufTmp[offset++] = DT_VISIBLE_STRING;
    dstBufTmp[offset++] =4;
    memcpy(&dstBufTmp[offset],VERSION_T.szFactory.value,4);
    offset=offset+4;
    //软件版本号
    dstBufTmp[offset++] = DT_VISIBLE_STRING;
    dstBufTmp[offset++] =4;
    memcpy(&dstBufTmp[offset],VERSION_T.szSoftVer.value,4);
    offset=offset+4;
    //软件版本日期
    dstBufTmp[offset++] = DT_VISIBLE_STRING;
    dstBufTmp[offset++] =6;
    memcpy(&dstBufTmp[offset],VERSION_T.szSoftDate.value,6);
    offset=offset+6;
    //硬件版本号
    dstBufTmp[offset++] = DT_VISIBLE_STRING;
    dstBufTmp[offset++] =4;
    memcpy(&dstBufTmp[offset],VERSION_T.szHardVer.value,4);
    offset=offset+4;
    //硬件版本日期
    dstBufTmp[offset++] = DT_VISIBLE_STRING;
    dstBufTmp[offset++] =6;
    memcpy(&dstBufTmp[offset],VERSION_T.szHardDate.value,6);
    offset=offset+6;
    //厂商扩展信息
    dstBufTmp[offset++] = DT_VISIBLE_STRING;
    dstBufTmp[offset++] =8;
    memcpy(&dstBufTmp[offset],VERSION_T.szExtend.value,8);
    offset=offset+8;
    *pOffset=offset;
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      获取支持规约列表
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_protocol_list(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    uint32     offset = *pOffset;
    uint32     i=0;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_PROTOCOL_T PROTOCOL_T;
    
    memcpy((uint8*)&PROTOCOL_T, srcData, srcLen);
    dstBufTmp[offset++] = DT_ARRAY;
    dstBufTmp[offset++] = PROTOCOL_T.nNum;
    for(i = 0; i < PROTOCOL_T.nNum; i++)
    {
        dstBufTmp[offset++] = DT_VISIBLE_STRING;
        dstBufTmp[offset++] = PROTOCOL_T.value[i].nNum;
        memcpy(dstBufTmp + offset, PROTOCOL_T.value[i].value, PROTOCOL_T.value[i].nNum);
        offset += PROTOCOL_T.value[i].nNum;
    }
    *pOffset=offset;
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      获取SIM卡ICCID
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_sim_iccid(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
   
    uint32     offset = *pOffset;
    
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_VISIBLESTR20_T VISIBLESTR20_T;
    
    memcpy((uint8*)&VISIBLESTR20_T, srcData, srcLen);
    dstBufTmp[offset++] = DT_VISIBLE_STRING;
    dstBufTmp[offset++] = 20;
    memcpy(&dstBufTmp[offset],VISIBLESTR20_T.value,20);
    offset=offset+20;
    *pOffset=offset;
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      获取IMSI
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_imsi(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    
    uint32     offset = *pOffset;
    
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_VISIBLESTR15_T VISIBLESTR15_T;
    
    memcpy((uint8*)&VISIBLESTR15_T, srcData, srcLen);
    dstBufTmp[offset++] = DT_VISIBLE_STRING;
    dstBufTmp[offset++] = 15;
    memcpy(&dstBufTmp[offset],VISIBLESTR15_T.value, 15);
    offset=offset+15;
    *pOffset=offset;
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      获取信号强度
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_signal_strength(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    int32 nRet = ERR_NORMAL;
    uint32     offset = *pOffset;
    
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    if(basic_data_to_buf(E_LONG, &dstBufTmp[offset], dstLen-offset, &offset, srcData, 2) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    *pOffset=offset;
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      获取SIM卡号码
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_sim_number(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    
    uint32     offset = *pOffset;
    
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_VISIBLESTR16_T VISIBLESTR16_T;

    memcpy((uint8*)&VISIBLESTR16_T, srcData, srcLen);
    dstBufTmp[offset++] = DT_VISIBLE_STRING;
    dstBufTmp[offset++] = 0x0F;
    memcpy(&dstBufTmp[offset],VISIBLESTR16_T.value,15);
    offset=offset+15;
    *pOffset=offset;
    return ERR_OK;
}
int32 data_to_buf_terminal_nettype(void *dstBuf, uint32 dstLen, uint32 *pOffset, uint8 operate, uint8 nettype)
{   
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    dstBufTmp[offset++] = DT_STRUCTURE;
    dstBufTmp[offset++] = 2;
    dstBufTmp[offset++] = DT_UNSIGNED;
    dstBufTmp[offset++] = operate;
    dstBufTmp[offset++] = DT_UNSIGNED;
    dstBufTmp[offset++] = nettype;

    *pOffset=offset;
    return ERR_OK;
}
/**
*********************************************************************
* @brief：      获取终端IP
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_terminal_ip(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    uint32     offset = *pOffset;
    
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_VISIBLESTR16_T VISIBLESTR16_T;

    memcpy((uint8*)&VISIBLESTR16_T, srcData, srcLen);
    dstBufTmp[offset++] = DT_OCTET_STRING;
    dstBufTmp[offset++] = 4;
    memcpy(&dstBufTmp[offset],VISIBLESTR16_T.value, 4);
    offset += 4;
    *pOffset=offset;
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      获取SIM卡号码
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_dev_type(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_VISIBLEVAR_T tString;

    memcpy(&tString, srcData, srcLen);
    dstBufTmp[offset++] = DT_VISIBLE_STRING;
    dstBufTmp[offset++] = tString.nNum;
    memcpy(dstBufTmp + offset, tString.value, tString.nNum);
    offset += tString.nNum;
    *pOffset = offset;
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      获取运营商及网络制式
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_nettype(void *dstBuf, uint32 dstLen, uint32 *pOffset, uint8 operat, uint8 nettype)
{   
    uint32     offset = *pOffset;
    
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    dstBufTmp[offset++] = DT_STRUCTURE;
    dstBufTmp[offset++] = 2;
    dstBufTmp[offset++] = DT_UNSIGNED;
    if(1 == operat)
    {
        dstBufTmp[offset++] = 2;
    }
    else if(2 == operat)
    {
        dstBufTmp[offset++] = 1;
    }
    else
    {
        dstBufTmp[offset++] = operat;
    }

    dstBufTmp[offset++] = DT_UNSIGNED;
    dstBufTmp[offset++] = nettype;

    *pOffset=offset;
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      多网络配置
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_apngroup(void *dstBuf, uint32 dstLen, uint32 *pOffset, OOP_APNGROUP_T *pApnGroup)
{   
    uint32     offset = *pOffset;
    uint8  *dstBufTmp = (uint8 *)dstBuf;
    uint8          i = 0;
    uint8          j = 0;

    dstBufTmp[offset++] = DT_ARRAY;
    dstBufTmp[offset++] = pApnGroup->nNum;

    for(i = 0; i < pApnGroup->nNum; i++)
    {
        dstBufTmp[offset++] = DT_STRUCTURE;
        dstBufTmp[offset++] = 9;

        //运营商代码
        dstBufTmp[offset++] = DT_ENUM;
        dstBufTmp[offset++] = pApnGroup->net[i].factory;
        //网络类型代码
        dstBufTmp[offset++] = DT_ENUM;
        dstBufTmp[offset++] = pApnGroup->net[i].network;
        //APN
        dstBufTmp[offset++] = DT_VISIBLE_STRING;
        dstBufTmp[offset++] = pApnGroup->net[i].apn.apn.nNum;
        memcpy(dstBufTmp + offset, pApnGroup->net[i].apn.apn.value, pApnGroup->net[i].apn.apn.nNum);
        offset += pApnGroup->net[i].apn.apn.nNum;
        //用户名
        dstBufTmp[offset++] = DT_VISIBLE_STRING;
        dstBufTmp[offset++] = pApnGroup->net[i].apn.username.nNum;
        memcpy(dstBufTmp + offset, pApnGroup->net[i].apn.username.value, pApnGroup->net[i].apn.username.nNum);
        offset += pApnGroup->net[i].apn.username.nNum;
        //密码
        dstBufTmp[offset++] = DT_VISIBLE_STRING;
        dstBufTmp[offset++] = pApnGroup->net[i].apn.pwd.nNum;
        memcpy(dstBufTmp + offset, pApnGroup->net[i].apn.pwd.value, pApnGroup->net[i].apn.pwd.nNum);
        offset += pApnGroup->net[i].apn.pwd.nNum;
        //代理服务器地址
        dstBufTmp[offset++] = DT_OCTET_STRING;
        dstBufTmp[offset++] = 4;
        memcpy(dstBufTmp + offset, pApnGroup->net[i].apn.proxyip, 4);
        offset += 4;
        //代理端口
        dstBufTmp[offset++] = DT_LONG_UNSIGNED;
        memcpy_r(dstBufTmp + offset, &(pApnGroup->net[i].apn.proxyport), sizeof(uint16));
        offset += sizeof(uint16);
        //鉴权方式
        dstBufTmp[offset++] = DT_ENUM;
        dstBufTmp[offset++] = pApnGroup->net[i].auth;

        //主站通信参数
        dstBufTmp[offset++] = DT_ARRAY;
        dstBufTmp[offset++] = pApnGroup->net[i].master.nNum;
        for(j = 0; j < pApnGroup->net[i].master.nNum; j++)
        {
            dstBufTmp[offset++] = DT_STRUCTURE;
            dstBufTmp[offset++] = 2;
            //IP地址
            dstBufTmp[offset++] = DT_OCTET_STRING;
            dstBufTmp[offset++] = 4;
            memcpy(dstBufTmp + offset, pApnGroup->net[i].master.master[j].ip, 4);
            offset += 4;
            //端口
            dstBufTmp[offset++] = DT_LONG_UNSIGNED;
            memcpy_r(dstBufTmp + offset, &(pApnGroup->net[i].master.master[j].port), sizeof(uint16));
            offset += sizeof(uint16);
        }
    }

    *pOffset = offset;
    return ERR_OK;
}
/**
*********************************************************************
* @brief：      获取运营商及网络制式
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_net_info(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    OOP_NETINFO_T NETINFO;
    uint32     offset = *pOffset;
    
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    memcpy(&NETINFO, srcData, srcLen);
    
    dstBufTmp[offset++] = DT_STRUCTURE;

    dstBufTmp[offset++] = 0x02;

    if(basic_data_to_buf(E_UNSIGNED, &dstBufTmp[offset], dstLen-offset, &offset, (uint8*)&NETINFO.nNetOpt, 2) < 0)
    {
        
        return DATA_TYPE_UNMATCHED;
    }
    if(basic_data_to_buf(E_UNSIGNED, &dstBufTmp[offset], dstLen-offset, &offset, (uint8*)&NETINFO.nNetType, 2) < 0)
    {
        
        return DATA_TYPE_UNMATCHED;
    }
    *pOffset = offset;
    
    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief：     设置公网通信的通信配置
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_gprs_config(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo,void *srcData, uint32 srcLen, uint32 *pOffset, void *dstBuf, uint32 *dstLen)
{
    OOP_GPRSCONFIG_T GPRSCONFIG_T;
    uint32 i=0;
    uint32 offset=*pOffset;
    int result;
    uint8 *srcDataTmp = (uint8 *)srcData;
    uint8 num;

    memset((uint8*)&GPRSCONFIG_T,0x00,sizeof(OOP_GPRSCONFIG_T));

    if(DT_NULL == srcDataTmp[offset])
    {
        *pOffset = offset + 1;
        return DATA_SUCCESS;
    }

    if(srcDataTmp[offset++]!=DT_STRUCTURE)
    {
        return DATA_TYPE_UNMATCHED;
    }
    offset++;
    //工作模式
    result=basic_buf_to_data(E_ENUM,srcDataTmp,srcLen,&offset,&GPRSCONFIG_T.workmode,1);
    if(GPRSCONFIG_T.workmode>2)
    {
       return DATA_READ_WRITE_DENIED;
    }
    //在线方式
    result=basic_buf_to_data(E_ENUM,srcDataTmp,srcLen,&offset,&GPRSCONFIG_T.linetype,1);
    //连接方式
    result=basic_buf_to_data(E_ENUM,srcDataTmp,srcLen,&offset,&GPRSCONFIG_T.contype,1);
    //连接应用方式
    result=basic_buf_to_data(E_ENUM,srcDataTmp,srcLen,&offset,&GPRSCONFIG_T.conmode,1);
    PRTL_FMT_LOGCC("buf_to_data_gprs_config11, offset is %d\n",offset);
    //侦听端口列表
    if(DT_NULL != srcDataTmp[offset])
    {
        if(srcDataTmp[offset++]!=DT_ARRAY)
        {
            return DATA_TYPE_UNMATCHED;
        }
        GPRSCONFIG_T.listenNum=srcDataTmp[offset++];
        for(i=0;i<GPRSCONFIG_T.listenNum;i++)
        {
            result=basic_buf_to_data(E_LONG_UNSIGNED,srcDataTmp,srcLen,&offset,&GPRSCONFIG_T.listen[i],2);
        }
    }
    else
    {
        offset++;
        GPRSCONFIG_T.listenNum = 1;
    }
    
    //APN
    PRTL_FMT_LOGCC("buf_to_data_gprs_config22 offset is %d\n",offset);
    if(srcDataTmp[offset]==DT_NULL)
    {
        offset++;
        GPRSCONFIG_T.apn.apn.nNum=0x00;
    }
    else if(srcDataTmp[offset]==DT_VISIBLE_STRING)
    {
        offset++;
        if(srcDataTmp[offset]==0x00)
        {
            offset++;
            GPRSCONFIG_T.apn.apn.nNum=0x00;
        }
        else
        {
            GPRSCONFIG_T.apn.apn.nNum=srcDataTmp[offset++];
            memcpy(GPRSCONFIG_T.apn.apn.value,srcDataTmp+offset,GPRSCONFIG_T.apn.apn.nNum);
            offset=offset+GPRSCONFIG_T.apn.apn.nNum;
        }
    }
    else
    {
        return DATA_TYPE_UNMATCHED;
    }
    
    //用户名
    PRTL_FMT_LOGCC("buf_to_data_gprs_config33 offset is %d\n",offset);
    if(srcDataTmp[offset]==DT_NULL)
    {
        offset++;
        GPRSCONFIG_T.apn.username.nNum=0x00;
    }
    else if(srcDataTmp[offset]==DT_VISIBLE_STRING)
    {
        offset++;
        if(srcDataTmp[offset]==0x00)
        {
            offset++;
            GPRSCONFIG_T.apn.username.nNum=0x00;
        }
        else
        {
            GPRSCONFIG_T.apn.username.nNum=srcDataTmp[offset++];
            memcpy(GPRSCONFIG_T.apn.username.value,srcDataTmp+offset,GPRSCONFIG_T.apn.username.nNum);
            offset=offset+GPRSCONFIG_T.apn.username.nNum;
        }
    }
    else
    {
        return DATA_TYPE_UNMATCHED;
    }
    
    //密码
    PRTL_FMT_LOGCC("buf_to_data_gprs_config44 offset is %d\n",offset);
    if(srcDataTmp[offset]==DT_NULL)
    {
        offset++;
        GPRSCONFIG_T.apn.pwd.nNum=0x00;
    }
    else if(srcDataTmp[offset]==DT_VISIBLE_STRING)
    {
        offset++;
        if(srcDataTmp[offset]==0x00)
        {
            offset++;
            GPRSCONFIG_T.apn.pwd.nNum=0x00;
        }
        else
        {
            GPRSCONFIG_T.apn.pwd.nNum=srcDataTmp[offset++];
            memcpy(GPRSCONFIG_T.apn.pwd.value,srcDataTmp+offset,GPRSCONFIG_T.apn.pwd.nNum);
            offset=offset+GPRSCONFIG_T.apn.pwd.nNum;
        }
    }
    else
    {
        return DATA_TYPE_UNMATCHED;
    }
    
    //代理服务器地址
    PRTL_FMT_LOGCC("buf_to_data_gprs_config55 offset is %d\n",offset);
    if(srcDataTmp[offset]==DT_NULL)
    {
        offset++;
        memset(GPRSCONFIG_T.apn.proxyip,0x00,4);
    }
    else if(srcDataTmp[offset]==DT_OCTET_STRING)
    {
        offset++;
        num=srcDataTmp[offset++];

        if(num>4)
        {
            return DATA_TYPE_UNMATCHED;
        }
        memcpy(GPRSCONFIG_T.apn.proxyip,srcDataTmp+offset,num);
        offset=offset+num;
    }
    else
    {
        return DATA_TYPE_UNMATCHED;
    }

    //代理端口
    if(DT_NULL == srcDataTmp[offset])
    {
        offset++;
        GPRSCONFIG_T.apn.proxyport = 0;
    }
    else
    {
        PRTL_FMT_LOGCC("buf_to_data_gprs_config55 offset is %d\n",offset);
        result=basic_buf_to_data(E_LONG_UNSIGNED,srcDataTmp,srcLen,&offset,&GPRSCONFIG_T.apn.proxyport,2);
        if(result !=DATA_SUCCESS)
        {
            return DATA_TYPE_UNMATCHED;
        }
    }
    //超时时间及重发次数
    result=basic_buf_to_data(E_UNSIGNED,srcDataTmp,srcLen,&offset,&GPRSCONFIG_T.dail.body,1);
    if(result !=DATA_SUCCESS)
    {
        return DATA_TYPE_UNMATCHED;
    }
    //心跳周期（秒）
    result=basic_buf_to_data(E_LONG_UNSIGNED,srcDataTmp,srcLen,&offset,&GPRSCONFIG_T.heart,2);
    if(result !=DATA_SUCCESS)
    {
        return DATA_TYPE_UNMATCHED;
    }
    memcpy((uint8*)dstBuf,(uint8*)&GPRSCONFIG_T,sizeof(OOP_GPRSCONFIG_T));
    *dstLen=sizeof(OOP_GPRSCONFIG_T);
    return result;
}

/**
*********************************************************************
* @brief：     设置公网通信的通信配置
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_gprs_heart_config(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo,void *srcData, uint32 srcLen, uint32 *pOffset, void *dstBuf, uint32 *dstLen)
{
    OOP_GPRSCONFIG_T GPRSCONFIG_T;
    uint32 offset=*pOffset;
    uint32 len=0;
    int result;
    int ret;
    uint8 *srcDataTmp = (uint8 *)srcData;

    NOMAL_OAD_T NormalOad ;
    NormalOad.logicId = 0;
    NormalOad.infoNum = 0;
    NormalOad.oad.value = pOadInfo->pTab->oad.value&0xFFFFFF00;

    memset((uint8*)&GPRSCONFIG_T,0x00,sizeof(OOP_GPRSCONFIG_T));
    ret = db_read_nomal(pApduInfo->hUdp, &NormalOad, sizeof(OOP_GPRSCONFIG_T), (uint8*)&GPRSCONFIG_T, &len);
    if((ret !=0)||(len !=sizeof(OOP_GPRSCONFIG_T)))
    {
        
    }
    //心跳周期（秒）
    result=basic_buf_to_data(E_LONG_UNSIGNED,srcDataTmp,srcLen,&offset,&GPRSCONFIG_T.heart,2);

    memcpy((uint8*)dstBuf,(uint8*)&GPRSCONFIG_T,sizeof(OOP_GPRSCONFIG_T));
    *dstLen=sizeof(OOP_GPRSCONFIG_T);
    return result;
}

/**
*********************************************************************
* @brief：      设置主站通信参数
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 buf_to_data_master_commu_param(void *srcData, uint32 srcLen, uint32 *pOffset, void *dstBuf, uint32 *dstLen)
{
    OOP_MASTERPARAMS_T MASTERPARAMS_T;
    uint32 i=0;
    uint32 offset=*pOffset;
    int result;
    uint8 *srcDataTmp = (uint8 *)srcData;
    uint16 dataLen;

    memset((uint8*)&MASTERPARAMS_T,0x00,sizeof(OOP_MASTERPARAMS_T));
    if(DT_NULL == srcDataTmp[offset])
    {
        *pOffset = offset + 1;
        return DATA_SUCCESS;
    }

    if(srcDataTmp[offset++] !=DT_ARRAY)
    {
        return ERR_OPTION;
    }
    MASTERPARAMS_T.nNum=srcDataTmp[offset++];
    for(i=0;i<MASTERPARAMS_T.nNum;i++)
    {
        if(srcDataTmp[offset++] !=DT_STRUCTURE)
        {
            return ERR_OPTION;
        }
        offset++;
        if(srcDataTmp[offset++] !=DT_OCTET_STRING)
        {
            return ERR_OPTION;
        }
        dataLen = srcDataTmp[offset++];
        if(dataLen ==0x81)
        {
            dataLen = srcDataTmp[offset++];
        }
        else if(dataLen == 0x82)
        {
            dataLen = srcDataTmp[offset++];
            dataLen <<= 8;
            dataLen += srcDataTmp[offset++];
        }
        else if(dataLen>0x82||dataLen==0x80)
        {
            return ERR_OPTION;
        }
        //ip
        memcpy(MASTERPARAMS_T.master[i].ip,srcDataTmp+offset,dataLen);
        //port
        offset=offset+dataLen;
        result=basic_buf_to_data(E_LONG_UNSIGNED,srcDataTmp,srcLen,&offset,&MASTERPARAMS_T.master[i].port,2);   
        
    }
    memcpy((uint8*)dstBuf,(uint8*)&MASTERPARAMS_T,sizeof(OOP_MASTERPARAMS_T));
    *dstLen=sizeof(OOP_MASTERPARAMS_T);
    *pOffset=offset;
//  frame_printf(dstBuf, *dstLen);
    return result;
}

/**
*********************************************************************
* @brief：      设置短信通信参数
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 buf_to_data_sms_commu_param(void *srcData, uint32 srcLen, uint32 *pOffset, void *dstBuf, uint32 *dstLen)
{
    OOP_SMS_T SMS_T;
    uint32 i = 0;
    uint32 offset = *pOffset;
    uint8 *srcDataTmp = (uint8 *)srcData;
    uint32 len = 0;
    

    memset((uint8*)&SMS_T, 0x00, sizeof(OOP_SMS_T));
    if(DT_NULL == srcDataTmp[offset])
    {
        *pOffset = offset + 1;
        return DATA_SUCCESS;
    }

    if(srcDataTmp[offset++] != DT_STRUCTURE)
    {
        return ERR_OPTION;
    }

    //结构体数量
    len = srcDataTmp[offset++];
    if (len != 3)
    {
        return ERR_FORMAT;
    }
    
    if(srcDataTmp[offset++] != DT_VISIBLE_STRING)
    {
        return ERR_OPTION;
    }

    //短信中心号码
    len = srcDataTmp[offset++];
    memcpy(SMS_T.centernum, srcDataTmp+offset, len);

    //主站号码
    offset = offset + len;
    if(srcDataTmp[offset++] != DT_ARRAY)
    {
        return ERR_OPTION;
    }
    SMS_T.hostnum.nNum = srcDataTmp[offset++];
    for(i=0;i<SMS_T.hostnum.nNum;i++)
    {
        if(srcDataTmp[offset++] != DT_VISIBLE_STRING)
        {
            return ERR_OPTION;
        }
        len = srcDataTmp[offset++];
        memcpy(SMS_T.hostnum.item[i].value, srcDataTmp + offset, len);
        offset = offset + len;
    }

    //短信通知目的号码
    if(srcDataTmp[offset++] != DT_ARRAY)
    {
        return ERR_OPTION;
    }
    SMS_T.localnum.nNum = srcDataTmp[offset++];
    for(i=0;i<SMS_T.localnum.nNum;i++)
    {
        if(srcDataTmp[offset++] != DT_VISIBLE_STRING)
        {
            return ERR_OPTION;
        }
        len = srcDataTmp[offset++];
        memcpy(SMS_T.localnum.item[i].value, srcDataTmp + offset, len);
        offset = offset + len;
    }

    memcpy((uint8*)dstBuf,(uint8*)&SMS_T,sizeof(OOP_SMS_T));
    *dstLen=sizeof(OOP_SMS_T);
    return ERR_OK;

}

/**
*********************************************************************
* @brief：      设置版本信息
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 buf_to_data_version_info(void *srcData, uint32 srcLen, uint32 *pOffset, void *dstBuf, uint32 *dstLen)
{
    OOP_VERSION_T VERSION_T;
    
    uint32 offset=*pOffset;
    uint8 *srcDataTmp = (uint8 *)srcData;
    
    
    if(srcDataTmp[offset++] !=DT_STRUCTURE)
    {
        return ERR_OPTION;
    }
    PRTL_FMT_LOGCC("srcDataTmp[offset] is %d\n", srcDataTmp[offset]);
    offset++;
    if(srcDataTmp[offset++] !=DT_VISIBLE_STRING)
    {
        return ERR_OPTION;
    }
    offset++;
    memcpy(VERSION_T.szFactory.value,srcDataTmp+offset,4);
    offset=offset+4;
    if(srcDataTmp[offset++] !=DT_VISIBLE_STRING)
    {
        return ERR_OPTION;
    }
    offset++;
    memcpy(VERSION_T.szSoftVer.value,srcDataTmp+offset,4);
    offset=offset+4;
    if(srcDataTmp[offset++] !=DT_VISIBLE_STRING)
    {
        return ERR_OPTION;
    }
    offset++;
    memcpy(VERSION_T.szSoftDate.value,srcDataTmp+offset,6);
    offset=offset+6;
    if(srcDataTmp[offset++] !=DT_VISIBLE_STRING)
    {
        return ERR_OPTION;
    }
    offset++;
    memcpy(VERSION_T.szHardVer.value,srcDataTmp+offset,4);
    offset=offset+4;
    if(srcDataTmp[offset++] !=DT_VISIBLE_STRING)
    {
        return ERR_OPTION;
    }
    offset++;
    memcpy(VERSION_T.szHardDate.value,srcDataTmp+offset,6);
    offset=offset+6;
    if(srcDataTmp[offset++] !=DT_VISIBLE_STRING)
    {
        return ERR_OPTION;
    }
    offset++;
    memcpy(VERSION_T.szExtend.value,srcDataTmp+offset,8);
    offset=offset+8;
    *pOffset = offset;
    memcpy((uint8*)dstBuf,(uint8*)&VERSION_T,sizeof(OOP_VERSION_T));
    *dstLen=sizeof(OOP_VERSION_T);
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      设置运营商及网络制式
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_net_info(void *srcData, uint32 srcLen, uint32 *pOffset, void *dstBuf, uint32 *dstLen)
{
    OOP_NETINFO_T NETINFO;
    OOP_DAR_E result; 
    uint32 offset=*pOffset;
    uint8 *srcDataTmp = (uint8 *)srcData;
    
    
    if(srcDataTmp[offset++] !=DT_STRUCTURE)
    {
        return DATA_TYPE_UNMATCHED;
    }
    PRTL_FMT_LOGCC("srcDataTmp[offset] is %d\n", srcDataTmp[offset]);
    offset++;
    //运营商
    result=basic_buf_to_data(E_LONG_UNSIGNED,srcDataTmp,srcLen,&offset,&NETINFO.nNetOpt,1);
    if(result !=DATA_SUCCESS)
    {
        return DATA_TYPE_UNMATCHED;
    }
    //网络制式
    result=basic_buf_to_data(E_LONG_UNSIGNED,srcDataTmp,srcLen,&offset,&NETINFO.nNetType,1);
    if(result !=DATA_SUCCESS)
    {
        return DATA_TYPE_UNMATCHED;
    }
    *pOffset = offset;
    memcpy((uint8*)dstBuf,(uint8*)&NETINFO,sizeof(NETINFO));
    *dstLen=sizeof(NETINFO);
    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief：      设置版本信息
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_apn_group(void *srcData, uint32 srcLen, uint32 *pOffset, void *dstBuf, uint32 *dstLen)
{
    OOP_APNGROUP_T APNGROUP;
    OOP_DAR_E result; 
    uint32 offset=*pOffset;
    uint8 i=0,j=0;
    uint8 *srcDataTmp = (uint8 *)srcData;
    
    
    if(srcDataTmp[offset++] !=DT_ARRAY)
    {
        return DATA_TYPE_UNMATCHED;
    }
    PRTL_FMT_LOGCC("srcDataTmp[offset] is %d\n", srcDataTmp[offset]);

    APNGROUP.nNum = srcDataTmp[offset++];

    for(i=0;i<APNGROUP.nNum;i++)
    {
        if(srcDataTmp[offset++] !=DT_STRUCTURE)
        {
            return DATA_TYPE_UNMATCHED;
        }
        offset++;
        //运营商代码
        result=basic_buf_to_data(E_ENUM,srcDataTmp,srcLen,&offset,&APNGROUP.net[i].factory,1);
        if(result !=DATA_SUCCESS)
        {
            return DATA_TYPE_UNMATCHED;
        }
        //网络类型代码
        result=basic_buf_to_data(E_ENUM,srcDataTmp,srcLen,&offset,&APNGROUP.net[i].network,1);
        if(result !=DATA_SUCCESS)
        {
            return DATA_TYPE_UNMATCHED;
        }
        //APN
        if(srcDataTmp[offset]==DT_NULL)
        {
            offset++;
        }
        else if(srcDataTmp[offset]==DT_VISIBLE_STRING)
        {
            offset++;
            APNGROUP.net[i].apn.apn.nNum = srcDataTmp[offset++];
            memcpy((uint8*)&srcDataTmp[offset],APNGROUP.net[i].apn.apn.value,APNGROUP.net[i].apn.apn.nNum);
            offset = offset + APNGROUP.net[i].apn.apn.nNum;
        }
        else
        {
            return DATA_TYPE_UNMATCHED;
        }
        //用户名
        if(srcDataTmp[offset]==DT_NULL)
        {
            offset++;
        }
        else if(srcDataTmp[offset]==DT_VISIBLE_STRING)
        {
            offset++;
            APNGROUP.net[i].apn.username.nNum = srcDataTmp[offset++];
            memcpy((uint8*)&srcDataTmp[offset],APNGROUP.net[i].apn.username.value,APNGROUP.net[i].apn.username.nNum);
            offset = offset + APNGROUP.net[i].apn.username.nNum;
        }
        else
        {
            return DATA_TYPE_UNMATCHED;
        }
        //密码
        if(srcDataTmp[offset]==DT_NULL)
        {
            offset++;
        }
        else if(srcDataTmp[offset]==DT_VISIBLE_STRING)
        {
            offset++;
            APNGROUP.net[i].apn.pwd.nNum = srcDataTmp[offset++];
            memcpy((uint8*)&srcDataTmp[offset],APNGROUP.net[i].apn.pwd.value,APNGROUP.net[i].apn.pwd.nNum);
            offset = offset + APNGROUP.net[i].apn.pwd.nNum;
        }
        else
        {
            return DATA_TYPE_UNMATCHED;
        }
        //代理服务器地址
        if(srcDataTmp[offset]==DT_NULL)
        {
            offset++;
        }
        else if(srcDataTmp[offset]==DT_OCTET_STRING)
        {
            offset++;
            memcpy((uint8*)&srcDataTmp[offset],APNGROUP.net[i].apn.proxyip,4);
            offset = offset + 4;
        }
        else
        {
            return DATA_TYPE_UNMATCHED;
        }
        //代理端口
        result=basic_buf_to_data(E_LONG_UNSIGNED,srcDataTmp,srcLen,&offset,&APNGROUP.net[i].apn.proxyport,2);
        if(result !=DATA_SUCCESS)
        {
            return DATA_TYPE_UNMATCHED;
        }
        //鉴权方式
        result=basic_buf_to_data(E_ENUM,srcDataTmp,srcLen,&offset,&APNGROUP.net[i].auth,1);
        if(result !=DATA_SUCCESS)
        {
            return DATA_TYPE_UNMATCHED;
        }
        //主站通信参数
        if(srcDataTmp[offset++] !=DT_ARRAY)
        {
            return DATA_TYPE_UNMATCHED;
        }
        APNGROUP.net[i].master.nNum = srcDataTmp[offset++];

        for(j=0;j<APNGROUP.net[i].master.nNum;j++)
        {
            if(srcDataTmp[offset++] !=DT_STRUCTURE)
            {
                return DATA_TYPE_UNMATCHED;
            }
            offset++;
            //IP地址
            if(srcDataTmp[offset]==DT_NULL)
            {
                offset++;
            }
            else if(srcDataTmp[offset]==DT_OCTET_STRING)
            {
                offset++;
                memcpy((uint8*)&srcDataTmp[offset],APNGROUP.net[i].master.master[j].ip,4);
                offset = offset + 4;
            }
            else
            {
                return DATA_TYPE_UNMATCHED;
            }
            //端口
            result=basic_buf_to_data(E_LONG_UNSIGNED,srcDataTmp,srcLen,&offset,&APNGROUP.net[i].master.master[j].port,2);
            if(result !=DATA_SUCCESS)
            {
                return DATA_TYPE_UNMATCHED;
            }
        }
    }
    *pOffset = offset;
    memcpy((uint8*)dstBuf,(uint8*)&APNGROUP,sizeof(APNGROUP));
    *dstLen=sizeof(APNGROUP);
    return DATA_SUCCESS;
}


/**
*********************************************************************
* @brief：      设置版本信息
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_protocol_list(void *srcData, uint32 srcLen, uint32 *pOffset, void *dstBuf, uint32 *dstLen)
{
    OOP_PROTOCOL_T PROTOCOL_T;
    
    uint32 offset=*pOffset;
    uint8 *srcDataTmp = (uint8 *)srcData;
    uint8 num=0,i;
    uint8 strlen=0;
    
    
    if(srcDataTmp[offset++] !=DT_ARRAY)
    {
        return DATA_TYPE_UNMATCHED;
    }
    PRTL_FMT_LOGCC("srcDataTmp[offset] is %d\n", srcDataTmp[offset]);
    num=srcDataTmp[offset++];

    for(i=0;i<num;i++)
    {
        if(srcDataTmp[offset++] !=DT_VISIBLE_STRING)
        {
            return DATA_TYPE_UNMATCHED;
        }
        strlen=srcDataTmp[offset++];
        memcpy((uint8*)&PROTOCOL_T,srcDataTmp+offset,strlen);
        offset=offset+strlen;
    }

    memcpy((uint8*)dstBuf,(uint8*)&PROTOCOL_T,sizeof(OOP_PROTOCOL_T));
    *dstLen=sizeof(OOP_PROTOCOL_T);
    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief：      设置多网络配置
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：ptApnGroup    - 目的数据
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_apngroup(void *srcData, uint32 srcLen, uint32 *pOffset, OOP_APNGROUP_T *ptApnGroup)
{
    uint32 offset=*pOffset;
    uint8 *srcDataTmp = (uint8 *)srcData;
    uint8          num = 0;
    uint8          i = 0;
    uint8          j = 0;
    
    if(DT_NULL == srcDataTmp[offset])
    {
        memset(ptApnGroup, 0, sizeof(OOP_APNGROUP_T));
        *pOffset = offset + 1;

        return DATA_SUCCESS;
    }

    if(DT_ARRAY != srcDataTmp[offset++])
    {
        return DATA_TYPE_UNMATCHED;
    }
    ptApnGroup->nNum = srcDataTmp[offset++];
    for(i = 0; i < ptApnGroup->nNum; i++)
    {
        if(DT_STRUCTURE != srcDataTmp[offset++])
        {
            return DATA_TYPE_UNMATCHED;
        }
        if(9 != srcDataTmp[offset++])
        {
            return DATA_TYPE_UNMATCHED;
        }

        //运营商代码
        if(DT_ENUM != srcDataTmp[offset++])
        {
            return DATA_TYPE_UNMATCHED;
        }
        ptApnGroup->net[i].factory = srcDataTmp[offset++]; 
 
        //网络类型代码
        if(DT_ENUM != srcDataTmp[offset++])
        {
            return DATA_TYPE_UNMATCHED;
        }
        ptApnGroup->net[i].network = srcDataTmp[offset++]; 

        //APN
        if(DT_NULL == srcDataTmp[offset])
        {
            ptApnGroup->net[i].apn.apn.nNum = 0;
            offset++;
        }
        else
        {
            if(DT_VISIBLE_STRING != srcDataTmp[offset++])
            {
                return DATA_TYPE_UNMATCHED;
            }
            ptApnGroup->net[i].apn.apn.nNum = srcDataTmp[offset++];
            if(ptApnGroup->net[i].apn.apn.nNum > 64)
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            if(ptApnGroup->net[i].apn.apn.nNum > 0)
            {
                memcpy(ptApnGroup->net[i].apn.apn.value, srcDataTmp + offset, ptApnGroup->net[i].apn.apn.nNum);
            }
            offset += ptApnGroup->net[i].apn.apn.nNum;
        }

        //用户名
        if(DT_NULL == srcDataTmp[offset])
        {
            ptApnGroup->net[i].apn.username.nNum = 0;
            offset++;
        }
        else
        {
            if(DT_VISIBLE_STRING != srcDataTmp[offset++])
            {
                return DATA_TYPE_UNMATCHED;
            }
            ptApnGroup->net[i].apn.username.nNum = srcDataTmp[offset++];
            if(ptApnGroup->net[i].apn.username.nNum > 64)
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            if(ptApnGroup->net[i].apn.username.nNum > 0)
            {
                memcpy(ptApnGroup->net[i].apn.username.value, srcDataTmp + offset, ptApnGroup->net[i].apn.username.nNum);
            }
            offset += ptApnGroup->net[i].apn.username.nNum;
        }

        //密码
        if(DT_NULL == srcDataTmp[offset])
        {
            ptApnGroup->net[i].apn.pwd.nNum = 0;
            offset++;
        }
        else
        {
            if(DT_VISIBLE_STRING != srcDataTmp[offset++])
            {
                return DATA_TYPE_UNMATCHED;
            }
            ptApnGroup->net[i].apn.pwd.nNum = srcDataTmp[offset++];
            if(ptApnGroup->net[i].apn.pwd.nNum > 64)
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            if(ptApnGroup->net[i].apn.pwd.nNum > 0)
            {
                memcpy(ptApnGroup->net[i].apn.pwd.value, srcDataTmp + offset, ptApnGroup->net[i].apn.pwd.nNum);
            }
            offset += ptApnGroup->net[i].apn.pwd.nNum;
        }

        //代理服务器地址
        if(DT_NULL == srcDataTmp[offset])
        {
            memset(ptApnGroup->net[i].apn.proxyip, 0, 4);
            offset++;
        }
        else
        {
            if(DT_OCTET_STRING != srcDataTmp[offset++])
            {
                return DATA_TYPE_UNMATCHED;
            }
            num = srcDataTmp[offset++];
            if(num > 4)
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            if(num > 0)
            {
                memcpy(ptApnGroup->net[i].apn.proxyip, srcDataTmp + offset, num);
            }
            offset += num;
        }

        //代理端口
        if(DT_LONG_UNSIGNED != srcDataTmp[offset++])
        {
            return DATA_TYPE_UNMATCHED;
        }
        memcpy_r(&(ptApnGroup->net[i].apn.proxyport), srcDataTmp + offset, 2);
        offset += 2;

        //鉴权方式
        if(DT_ENUM != srcDataTmp[offset++])
        {
            return DATA_TYPE_UNMATCHED;
        }
        ptApnGroup->net[i].auth = srcDataTmp[offset++];

        //主站通信参数
        if(DT_ARRAY != srcDataTmp[offset++])
        {
            return DATA_TYPE_UNMATCHED;
        }
        ptApnGroup->net[i].master.nNum = srcDataTmp[offset++];
        if(ptApnGroup->net[i].master.nNum > 4)
        {
            return DATA_SCOPE_OF_ACCESS_VIOLATED;
        }
        for(j = 0; j < ptApnGroup->net[i].master.nNum; j++)
        {
            if(DT_STRUCTURE != srcDataTmp[offset++])
            {
                return DATA_TYPE_UNMATCHED;
            }
            if(2 != srcDataTmp[offset++])
            {
                return DATA_TYPE_UNMATCHED;
            }

            //IP地址
            if(DT_OCTET_STRING != srcDataTmp[offset++])
            {
                return DATA_TYPE_UNMATCHED;
            }
            if(4 != srcDataTmp[offset++])
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            memcpy(ptApnGroup->net[i].master.master[j].ip, srcDataTmp + offset, 4);
            offset += 4;
    
            //端口
            if(DT_LONG_UNSIGNED != srcDataTmp[offset++])
            {
                return DATA_TYPE_UNMATCHED;
            }
            memcpy_r(&(ptApnGroup->net[i].master.master[j].port), srcDataTmp + offset, 2);
            offset += 2;
        }
    }

    *pOffset = offset;

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class25)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class25(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{

    int32 nRet = DATA_SUCCESS;
	
#if (defined AREA_SHAANXI)
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;

    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        ACUDP_FMT_TRACE("data_to_buf_class25() 入参检查 error, ERR_PTR\n");
        return ERR_PTR;
    }

    switch (pOadInfoTmp->pTab->eData)
    {
        case E_LONG:
        {
        	ACUDP_FMT_TRACE("data_to_buf_class25() case E_LONG: \n");
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
            ACUDP_FMT_TRACE("data_to_buf_class25() case E_ENUM: \n");
			
            if (dsdata->srcLen != sizeof(uint8))
            {
                nRet = ERR_OPTION;
                break;
            }
            dstBufTmp[offset++]= DT_ENUM;
            dstBufTmp[offset++]= srcDataTmp[0];
            nRet = ERR_OK;
        }
        	break;
        case E_OCTET_STR4_T:
        {
            ACUDP_FMT_TRACE("data_to_buf_class25() case E_OCTET_STR4_T: \n");
            OOP_OCTETSTR4_T data;
            if (dsdata->srcLen != sizeof(OOP_OCTETSTR4_T))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy(&data, srcDataTmp, dsdata->srcLen); 
            if(basic_data_to_buf(E_OCTET_STR4_T, dstBufTmp, dsdata->dstLen-offset, &offset, &data, sizeof(int32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            nRet = ERR_OK;
         
        }
        	break;
        case E_4GMONITOR_T:
        {
            ACUDP_FMT_TRACE("data_to_buf_class25() case E_4GMONITOR_T: \n");
            OOP_4GMONITOR_INFO_T monitorInfo;
            if (dsdata->srcLen != sizeof(OOP_4GMONITOR_INFO_T))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy((uint8*)&monitorInfo, srcDataTmp, dsdata->srcLen); 
            dstBufTmp[offset++]= DT_STRUCTURE;
            dstBufTmp[offset++]= 0x04;

            if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dsdata->dstLen-offset, &offset,(uint8*)&monitorInfo.rsrp, sizeof(int)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }

			if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dsdata->dstLen-offset, &offset,(uint8*)&monitorInfo.rsrq, sizeof(int)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }

			if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dsdata->dstLen-offset, &offset,(uint8*)&monitorInfo.simr, sizeof(int)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }

            if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset,(uint8*)&monitorInfo.arfcn, sizeof(uint32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }

            nRet = ERR_OK;
         
        }
        	break;
        default: ACUDP_FMT_TRACE("data_to_buf_class25 failed.unknow eData(%d)\n", pOadInfoTmp->pTab->eData);
	        nRet = ERR_OPTION;
	        break;
    }

    *dsdata->dOffset = offset;
#endif 

    return nRet;
}

/**
*********************************************************************
* @brief：      从数据中心的数据转698格式报文(class24)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_class25(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return 0;
}


#if DESC("class25对外接口函数",1)

/**
*********************************************************************
* @name：       class25_invoke_set
* @brief：      class25的SET服务
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
OOP_DAR_E class25_invoke_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret;
    uint32 len;
    uint32 offset=4;
    uint32 srcLen;
    OOP_DAR_E result;
    NOMAL_OAD_T NormalOad ;
    OOP_OAD_U oad;
    NormalOad.logicId = 0;
    NormalOad.infoNum = 0;
    NormalOad.oad.value = pOadInfo->pTab->oad.value;

    oad.value=pOadInfo->pTab->oad.value;
    PRTL_FMT_LOGCC("class25_invoke_set,pOadInfo->pTab->oad.value is %x\n", pOadInfo->pTab->oad.value);
    switch(oad.attID)
    {
        case 0x02:
        { 
            srcLen=sizeof(OOP_GPRSCONFIG_T);
            uint8 buffer[srcLen];

            if(pOadInfo->nIndex==0x00)
            {
                result=buf_to_data_gprs_config(pOadInfo,pApduInfo,(uint8 *)inData, inLen, &offset, buffer, &len);
                
                if(result !=ERR_OK)
                {
                    return result;
                }
            }
            else if(pOadInfo->nIndex==0x0C)
            {
                PRTL_FMT_LOGCC("oad.nIndex is %x\n", oad.nIndex);
                result=buf_to_data_gprs_heart_config(pOadInfo, pApduInfo,(uint8 *)inData, inLen, &offset, buffer, &len);
                
                if(result !=ERR_OK)
                {
                    return result;
                }
            }
            NormalOad.classtag = CLASS_DATA_UNINIT;
            ret=db_write_nomal(pApduInfo->hUdp, &NormalOad,buffer,len);
            
            if(ret !=0)
            {
                return DATA_READ_WRITE_DENIED;
            }
        }
        break;
        case 0x03:
        { 
            srcLen=sizeof(OOP_MASTERPARAMS_T);
            uint8 buffer[srcLen];

            result=buf_to_data_master_commu_param((uint8 *)inData, inLen, &offset, buffer, &len);
            if(result !=ERR_OK)
            {
                return result;
            }

            NormalOad.classtag = CLASS_DATA_UNINIT;
            ret=db_write_nomal(pApduInfo->hUdp, &NormalOad,buffer,len);
            printf("ret is %d\n",ret);
            if(ret<0)
            {
                return DATA_READ_WRITE_DENIED;
            }
            
        }
        break;
        case 0x04:
        { 
            srcLen=sizeof(OOP_SMS_T);
            uint8 buffer[srcLen];
            printf("class26_invoke_set\n");
            result=buf_to_data_sms_commu_param((uint8 *)inData, inLen, &offset, buffer, &len);
            printf("result is %d\n",result);
            if(result !=ERR_OK)
            {
                return result;
            }

            NormalOad.classtag = CLASS_DATA_UNINIT;
            ret=db_write_nomal(pApduInfo->hUdp, &NormalOad,buffer,len);
            if(ret<0)
            {
                return DATA_READ_WRITE_DENIED;
            }   
        }
        break;
        case 0x05:
        { 
            srcLen=sizeof(OOP_VERSION_T);
            uint8 buffer[srcLen];
            result=buf_to_data_version_info((uint8 *)inData, inLen, &offset, buffer, &len);
            PRTL_FMT_LOGCC("result is %d\n", result);
            if(result !=ERR_OK)
            {
                return result;
            }

            NormalOad.classtag = CLASS_DATA_UNINIT;
            ret=db_write_nomal(pApduInfo->hUdp, &NormalOad,buffer,len);
            if(ret !=0)
            {
                return DATA_READ_WRITE_DENIED;
            }       
        }
        break;
        case 0x06:
//      { 
//
//          srcLen=sizeof(OOP_PROTOCOL_T);
//          uint8 buffer[srcLen];
//          result=buf_to_data_protocol_list((uint8 *)inData, inLen, &offset, buffer, &len);
//          PRTL_FMT_LOGCC("result is %d\n", result);
//          if(result !=ERR_OK)
//          {
//              return result;
//          }
//
//          NormalOad.classtag = CLASS_DATA_UNINIT;
//          ret=db_write_nomal(pApduInfo->hUdp, &NormalOad,buffer,len);
//          if(ret !=0)
//          {
//              return DATA_READ_WRITE_DENIED;
//          }       
//      }
//      break;
        case 0x07:
        case 0x08:
        case 0x09:
        case 0x0B:
        case 0x0D:
        { 

            PRTL_FMT_DEBUG("pOadInfo->pTab->oad.value is %x\n", pOadInfo->pTab->oad.value);
            return DATA_READ_WRITE_DENIED;
                
        }
        break;
        case 0x0A:
        {
            PRTL_FMT_LOGCC("pOadInfo->pTab->oad.value is %x\n", pOadInfo->pTab->oad.value);
            OOP_VISIBLESTR16_T VISIBLESTR16_T;
            uint8 strlen=0;
            
            if(inData[offset++] !=DT_VISIBLE_STRING)
            {
                return DATA_TYPE_UNMATCHED;
            }
            strlen=inData[offset++];
            memcpy(VISIBLESTR16_T.value,inData+offset,strlen);
            offset=offset+strlen;
            NormalOad.classtag = CLASS_DATA_UNINIT;
            ret=db_write_nomal(pApduInfo->hUdp, &NormalOad,(uint8*)&VISIBLESTR16_T,sizeof(VISIBLESTR16_T));
            if(ret !=0)
            {
                return DATA_READ_WRITE_DENIED;
            }
            return DATA_SUCCESS;      
        }
        break;
        case 0x0E:
        {
            PRTL_FMT_LOGCC("pOadInfo->pTab->oad.value is %x\n", pOadInfo->pTab->oad.value);
            OOP_APNGROUP_T tApnGroup;
            
            memset(&tApnGroup, 0, sizeof(OOP_APNGROUP_T));
            ret = buf_to_data_apngroup((uint8 *)inData, inLen, &offset, &tApnGroup);
            if(0 != ret)
            {
                PRTL_FMT_DEBUG("class25_invoke_set get apn group failed, oad[0x%08x]\n", pOadInfo->pTab->oad.value);
                return ret;
            }

            NormalOad.classtag = CLASS_DATA_UNINIT;
            ret=db_write_nomal(pApduInfo->hUdp, &NormalOad, (uint8*)&tApnGroup, sizeof(OOP_APNGROUP_T));
            if(ret !=0)
            {
                PRTL_FMT_DEBUG("class25_invoke_set write apn group failed, oad[0x%08x]\n", pOadInfo->pTab->oad.value);
                return DATA_READ_WRITE_DENIED;
            }
            uint8 i, j;
            char bufstr[2048] = {0};
            uint16 offset = 0;
            OOP_NETCONFIG_T *ptNet = NULL;
            OOP_MASTERPARAM_T *ptMaster = NULL;

            for(i = 0; i < tApnGroup.nNum; i++)
            {
                ptNet = &(tApnGroup.net[i]);

                offset += sprintf(bufstr + offset, "Array %d: factory %d, nettype %d, apn %s, usr %s, pwd %s, "
                                                   "%u.%u.%u.%u:%u, auth %d\n", 
                                                   i + 1, ptNet->factory, ptNet->network, ptNet->apn.apn.value, 
                                                   ptNet->apn.username.value, ptNet->apn.pwd.value, 
                                                   ptNet->apn.proxyip[0], ptNet->apn.proxyip[1], ptNet->apn.proxyip[2],
                                                   ptNet->apn.proxyip[3], ptNet->apn.proxyport, ptNet->auth);
                for(j = 0; j < ptNet->master.nNum; j++)
                {
                    ptMaster = &(ptNet->master.master[j]);
                    offset += sprintf(bufstr + offset, "  master %d, %u.%u.%u.%u:%u\n", j + 1, ptMaster->ip[0], 
                                      ptMaster->ip[1], ptMaster->ip[2], ptMaster->ip[3], ptMaster->port);
                }
            }
            PRTL_FMT_DEBUG("Add Apn Group:\n%s\n", bufstr);
            return DATA_SUCCESS;      
        }
        break;
        default:

        break;
            
    }
        return result;

}

/**
*********************************************************************
* @name：       class25_invoke_get
* @brief：      class25的GET服务
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
OOP_DAR_E class25_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret;
    uint32 len=0;
    uint32 offset=0;
    uint32 srcLen;
    int result = ERR_OK;
    NOMAL_OAD_T NormalOad ;
    NormalOad.logicId = 0;
    NormalOad.infoNum = 0;
    NormalOad.oad.value = pOadInfo->pTab->oad.value;

    switch(pOadInfo->pTab->oad.value)
    {
        case 0x45000200:
        case 0x45010200:
        { 
//          OOP_GPRSCONFIG_T GPRSCONFIG_T;
            srcLen=sizeof(OOP_GPRSCONFIG_T);
            uint8 buffer[srcLen];
            ret=db_read_nomal(pApduInfo->hUdp, &NormalOad,srcLen, buffer, &len);
            if(ret!=0)
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->pResultData[5]=0x00;
                outData->resultDataLen=6;
                return ERR_OK;
            }
            printf("class25_invoke_get len is %d\n",len);
            result=data_to_buf_gprs_config(outData->pResultData+5, outDataMax, &offset, buffer,len);
            if(result==ERR_OK)
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->resultDataLen=offset+5;
            }
            else
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->pResultData[5]=0x00;
                outData->resultDataLen=6;
            }
            break;
        }
        case 0x45000300:
        case 0x45010300:
        { 
//          OOP_MASTERPARAMS_T MASTERPARAMS_T;
            srcLen=sizeof(OOP_MASTERPARAMS_T);
            uint8 buffer[srcLen];
            uint32 realoadvalue,nindex;                  //实际传进来的oad
            realoadvalue = pOadInfo->pTab->oad.value | pOadInfo->nIndex;
            PRTL_FMT_DEBUG("class25_invoke_get seach oad , oad[0x%08x]\n", realoadvalue);
            nindex = pOadInfo->nIndex;
            ret=db_read_nomal(pApduInfo->hUdp, &NormalOad,srcLen, buffer, &len);
            if(ret !=0)
            {
                memcpy_r(outData->pResultData,(uint8*)&realoadvalue,4);
                outData->pResultData[4]=0x01;
                outData->pResultData[5]=0x00;
                outData->resultDataLen=6;
                return ERR_OK;
            }
            printf("class26_invoke_get len is %d\n",len);
            result=data_to_buf_master_commu_param(outData->pResultData+5, outDataMax, &offset, buffer,len,nindex);
            if(result==ERR_OK)
            {
                memcpy_r(outData->pResultData,(uint8*)&realoadvalue,4);
                outData->pResultData[4]=0x01;
                outData->resultDataLen=offset+5;
            }
            else
            {
                memcpy_r(outData->pResultData,(uint8*)&realoadvalue,4);
                outData->pResultData[4]=0x01;
                outData->pResultData[5]=0x00;
                outData->resultDataLen=6;
            }
            break;      
        }
        case 0x45000400:
        case 0x45010400:
        { 
//          OOP_SMS_T SMS_T;
            srcLen=sizeof(OOP_SMS_T);
            uint8 buffer[srcLen];
            ret=db_read_nomal(pApduInfo->hUdp, &NormalOad,srcLen, buffer, &len);
//          printf("CCCCC!!!\n");
//          printf("len is %d\n",len);
//          frame_printf(buffer,len);
            if(ret !=0)
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->pResultData[5]=0x00;
                outData->resultDataLen=6;
                return ERR_OK;
            }
            result=data_to_buf_sms_commu_param(outData->pResultData+5, outDataMax, &offset, buffer,len);
            if(result==ERR_OK)
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->resultDataLen=offset+5;
            }
            else
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->pResultData[5]=0x00;
                outData->resultDataLen=6;
            }
            break;
        }
        case 0x45000500:
        case 0x45010500:
        { 
            uint8  buf[320] = {0};
            uint16 msglen = 0;
            WIRELESS_INFO_T tModeInfo;
            OOP_VERSION_T data;
            uint8 bGetDBCenter = FALSE;
            msglen = appmain_get_ppp_mode(0x45000500 == pOadInfo->pTab->oad.value ? 1 : 2, buf, 320);
            if(0 == msglen)
            {
                PRTL_FMT_DEBUG("class25_invoke_get ppp_mode failed, oad[0x%08x]\n", pOadInfo->pTab->oad.value);
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[5] = 0;
                outData->pResultData[4] = 0x01;
                outData->resultDataLen = 1 + 5;
                return DATA_SUCCESS;
            }
            memset(&tModeInfo, 0, sizeof(WIRELESS_INFO_T));
            appmsg_wireinfo_split(buf, &tModeInfo);
            if(0 == tModeInfo.instatus)
            {
                ret = db_read_nomal(pApduInfo->hUdp, &NormalOad, sizeof(OOP_VERSION_T), (uint8*)&data, &len);
                if(0 != ret)
                {
                    PRTL_FMT_DEBUG("Ppp_mode offline, oad[0x%08x] get dbCenter failed %d\n", pOadInfo->pTab->oad.value, ret);
                    memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                    outData->pResultData[5] = 0;
                    outData->pResultData[4] = 0x01;
                    outData->resultDataLen = 1 + 5;
                    return DATA_SUCCESS;
                }
                bGetDBCenter = TRUE;
            }
            
            if(FALSE == bGetDBCenter)
            {
                MEMZERO(&data, sizeof(data));
                memcpy(&data.szFactory.value[0], tModeInfo.version.manuNum, 4);
                memcpy(&data.szSoftVer.value[0], tModeInfo.version.softversion, 4);
                memcpy(&data.szSoftDate.value[0], tModeInfo.version.softdata, 6);
                memcpy(&data.szHardVer.value[0], tModeInfo.version.hwversion, 4);
                memcpy(&data.szHardDate.value[0], tModeInfo.version.hwdata, 6);
                memcpy(&data.szExtend.value[0], tModeInfo.version.manuExten, 8);
                if(0 != data.szFactory.value[0])
                {
                    db_write_nomal(pApduInfo->hUdp, &NormalOad, (uint8*)&data, sizeof(OOP_VERSION_T));
                }
            }
            result = data_to_buf_version_info(outData->pResultData+5, outDataMax, &offset, &data, sizeof(OOP_VERSION_T));
            if(result == ERR_OK)
            {
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[4] = 0x01;
                outData->resultDataLen = offset + 5;
            }
            else
            {
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[4] = 0x01;
                outData->pResultData[5] = 0x00;
                outData->resultDataLen = 6;
            }
            break;
        }
        case 0x45000600:
        case 0x45010600:
        { 
//          OOP_PROTOCOL_T PROTOCOL_T;
            srcLen=sizeof(OOP_PROTOCOL_T);
            uint8 buffer[srcLen];
            ret=db_read_nomal(pApduInfo->hUdp, &NormalOad,srcLen, buffer, &len);
            if(ret !=0)
            {
                /* 没有设置默认写698 */
                OOP_PROTOCOL_T *ptTemp = (OOP_PROTOCOL_T *)buffer;
                memset(ptTemp, 0, sizeof(OOP_PROTOCOL_T));
                ptTemp->nNum = 1;
                ptTemp->value[0].nNum = 6;
                memcpy(ptTemp->value[0].value, "698.45", 6);
                len = srcLen;
            }
            result=data_to_buf_protocol_list(outData->pResultData+5, outDataMax, &offset, buffer,len);
            if(result==ERR_OK)
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->resultDataLen=offset+5;
            }
            else
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->pResultData[5]=0x00;
                outData->resultDataLen=6;
            }
            break;
        }
        case 0x45000700:
        case 0x45010700:
        { 
            uint8  buf[480] = {0};
            uint16 msglen = 0;
            WIRE_DIAG_INFO_T tDiagInfo;

            msglen = appmain_get_ppp_diag(0x45000700 == pOadInfo->pTab->oad.value ? 1 : 2, buf, 480);
            if(0 == msglen)
            {
                PRTL_FMT_DEBUG("class25_invoke_get ppp_diag failed, oad[0x%08x]\n", pOadInfo->pTab->oad.value);
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[5] = 0;
                outData->pResultData[4] = 0x01;
                outData->resultDataLen = 1 + 5;
                return DATA_SUCCESS;
            }
            memset(&tDiagInfo, 0, sizeof(WIRE_DIAG_INFO_T));
            appmsg_wirediag_split(buf, &tDiagInfo);
            if(1 != tDiagInfo.simstatus)
            {
                PRTL_FMT_DEBUG("class25_invoke_get ppp_mode offline, oad[0x%08x]\n", pOadInfo->pTab->oad.value);
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[5] = 0;
                outData->pResultData[4] = 0x01;
                outData->resultDataLen = 1 + 5;
                return DATA_SUCCESS;
            }

            result=data_to_buf_sim_iccid(outData->pResultData+5, outDataMax, &offset, tDiagInfo.CCID.value, 20);
            if(result==ERR_OK)
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->resultDataLen=offset+5;
            }
            else
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->pResultData[5]=0x00;
                outData->resultDataLen=6;
            }
            break;
        }
        case 0x45000800:
        case 0x45010800:
        { 
            uint8  buf[480] = {0};
            uint16 msglen = 0;
            WIRE_DIAG_INFO_T tDiagInfo;

            msglen = appmain_get_ppp_diag(0x45000800 == pOadInfo->pTab->oad.value ? 1 : 2, buf, 480);
            if(0 == msglen)
            {
                PRTL_FMT_DEBUG("class25_invoke_get ppp_diag failed, oad[0x%08x]\n", pOadInfo->pTab->oad.value);
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[5] = 0;
                outData->pResultData[4] = 0x01;
                outData->resultDataLen = 1 + 5;
                return DATA_SUCCESS;
            }
            memset(&tDiagInfo, 0, sizeof(WIRE_DIAG_INFO_T));
            appmsg_wirediag_split(buf, &tDiagInfo);
            if(1 != tDiagInfo.simstatus)
            {
                PRTL_FMT_DEBUG("class25_invoke_get ppp_mode offline, oad[0x%08x]\n", pOadInfo->pTab->oad.value);
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[5] = 0;
                outData->pResultData[4] = 0x01;
                outData->resultDataLen = 1 + 5;
                return DATA_SUCCESS;
            }

            result=data_to_buf_imsi(outData->pResultData+5, outDataMax, &offset, tDiagInfo.IMSI.value, 15);
            if(result==ERR_OK)
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->resultDataLen=offset+5;
            }
            else
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x00;
                outData->pResultData[5]=0x00;
                outData->resultDataLen=6;
            }
            break;
        }
        case 0x45000900:
        case 0x45010900:
        { 
            uint8  buf[480] = {0};
            uint16 msglen = 0;
            WIRE_DIAG_INFO_T tDiagInfo;
            uint16 csq = 0;

            msglen = appmain_get_ppp_diag(0x45000900 == pOadInfo->pTab->oad.value ? 1 : 2, buf, 480);
            if(0 == msglen)
            {
                PRTL_FMT_DEBUG("class25_invoke_get ppp_diag failed, oad[0x%08x]\n", pOadInfo->pTab->oad.value);
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[5] = 0;
                outData->pResultData[4] = 0x01;
                outData->resultDataLen = 1 + 5;
                return DATA_SUCCESS;
            }
            memset(&tDiagInfo, 0, sizeof(WIRE_DIAG_INFO_T));
            appmsg_wirediag_split(buf, &tDiagInfo);
            if(1 != tDiagInfo.simstatus)
            {
                PRTL_FMT_DEBUG("class25_invoke_get ppp_mode offline, oad[0x%08x]\n", pOadInfo->pTab->oad.value);
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[5] = 0;
                outData->pResultData[4] = 0x01;
                outData->resultDataLen = 1 + 5;
                return DATA_SUCCESS;
            }

            csq = tDiagInfo.csqvalue;
            len = sizeof(uint16);
            result=data_to_buf_signal_strength(outData->pResultData+5, outDataMax, &offset, (void*)&csq, len);
            if(result==ERR_OK)
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->resultDataLen=offset+5;
            }
            else
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->pResultData[5]=0x00;
                outData->resultDataLen=6;
            }
            break;
        }
        case 0x45000A00:
        case 0x45010A00:
        { 
//          OOP_VISIBLESTR16_T VISIBLESTR16_T; 
            srcLen=sizeof(OOP_VISIBLESTR16_T);
            uint8 buffer[srcLen];
            ret=db_read_nomal(pApduInfo->hUdp, &NormalOad,srcLen, buffer, &len);
            if(ret !=0)
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->pResultData[5]=00;
                outData->resultDataLen=6;
                return DATA_SUCCESS;
            }
            result=data_to_buf_sim_number(outData->pResultData+5, outDataMax, &offset, buffer,len);
            if(result==ERR_OK)
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->resultDataLen=offset+5;
            }
            else
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->pResultData[5]=0x00;
                outData->resultDataLen=6;
            }
            break;
        }
        case 0x45000B00:
        case 0x45010B00:
        { 
            uint8  buf[480] = {0};
            uint16 msglen = 0;
            WIRE_DIAG_INFO_T tDiagInfo;

            msglen = appmain_get_ppp_diag(0x45000B00 == pOadInfo->pTab->oad.value ? 1 : 2, buf, 480);
            if(0 == msglen)
            {
                PRTL_FMT_DEBUG("class25_invoke_get ppp_diag failed, oad[0x%08x]\n", pOadInfo->pTab->oad.value);
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[5] = 0;
                outData->pResultData[4] = 0x01;
                outData->resultDataLen = 1 + 5;
                return DATA_SUCCESS;
            }
            memset(&tDiagInfo, 0, sizeof(WIRE_DIAG_INFO_T));
            appmsg_wirediag_split(buf, &tDiagInfo);
            if(1 != tDiagInfo.simstatus)
            {
                PRTL_FMT_DEBUG("class25_invoke_get ppp_mode offline, oad[0x%08x]\n", pOadInfo->pTab->oad.value);
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[5] = 0;
                outData->pResultData[4] = 0x01;
                outData->resultDataLen = 1 + 5;
                return DATA_SUCCESS;
            }

            result=data_to_buf_terminal_ip(outData->pResultData+5, outDataMax, &offset, tDiagInfo.IP, 4);
            if(result==ERR_OK)
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->resultDataLen=offset+5;
            }
            else
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->pResultData[5]=0x00;
                outData->resultDataLen=6;
            }
            break;
        }
        case 0x45000C00:
        case 0x45010C00:
        { 
//          OOP_VISIBLESTR16_T VISIBLESTR16_T; 
            srcLen=sizeof(OOP_VISIBLEVAR_T);
            uint8 buffer[srcLen];
            ret=db_read_nomal(pApduInfo->hUdp, &NormalOad, srcLen, buffer, &len);
            if(ret !=0)
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->pResultData[5]=00;
                outData->resultDataLen=6;
                return DATA_SUCCESS;
            }
            result=data_to_buf_dev_type(outData->pResultData+5, outDataMax, &offset, buffer,len);
            if(result==ERR_OK)
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->resultDataLen=offset+5;
            }
            else
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->pResultData[5]=0x00;
                outData->resultDataLen=6;
            }
            
        }
        break;
        case 0x45000D00:
        case 0x45010D00:
        { 
            uint8  buf[480] = {0};
            uint16 msglen = 0;
            WIRE_DIAG_INFO_T tDiagInfo;

            msglen = appmain_get_ppp_diag(0x45000D00 == pOadInfo->pTab->oad.value ? 1 : 2, buf, 480);
            if(0 == msglen)
            {
                PRTL_FMT_DEBUG("class25_invoke_get ppp_diag failed, oad[0x%08x]\n", pOadInfo->pTab->oad.value);
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[5] = 0;
                outData->pResultData[4] = 0x01;
                outData->resultDataLen = 1 + 5;
                return DATA_SUCCESS;
            }
            memset(&tDiagInfo, 0, sizeof(WIRE_DIAG_INFO_T));
            appmsg_wirediag_split(buf, &tDiagInfo);
            if(1 != tDiagInfo.simstatus)
            {
                PRTL_FMT_DEBUG("class25_invoke_get ppp_mode offline, oad[0x%08x]\n", pOadInfo->pTab->oad.value);
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[5] = 0;
                outData->pResultData[4] = 0x01;
                outData->resultDataLen = 1 + 5;
                return DATA_SUCCESS;
            }

            result=data_to_buf_nettype(outData->pResultData+5, outDataMax, &offset, tDiagInfo.operat, tDiagInfo.networkType);
            if(result==ERR_OK)
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->resultDataLen=offset+5;
            }
            else
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->pResultData[5]=0x00;
                outData->resultDataLen=6;
            }
            break;
        }
        break;
        case 0x45000E00:
        case 0x45010E00:
        { 
            OOP_APNGROUP_T tApnGroup;

            ret = db_read_nomal(pApduInfo->hUdp, &NormalOad, sizeof(OOP_APNGROUP_T), (uint8*)&tApnGroup, &len);
            if(ret != 0 || len != sizeof(OOP_APNGROUP_T))
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->pResultData[5]=00;
                outData->resultDataLen=6;
                return DATA_SUCCESS;
            }

            result = data_to_buf_apngroup(outData->pResultData + 5, outDataMax, &offset, &tApnGroup);
            if(ERR_OK == result)
            {
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[4] = 0x01;
                outData->resultDataLen = offset + 5;
            }
            else
            {
                PRTL_FMT_DEBUG("class25_invoke_get apn group failed, oad[0x%08x]\n", pOadInfo->pTab->oad.value);
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[4] = 0x01;
                outData->pResultData[5] = 0x00;
                outData->resultDataLen = 6;
            }
            break;
        }
        default: break;
            
    }
    return result;
}


/**
*********************************************************************
* @name：       class25_invoke_act
* @brief：      class25的ACTION服务
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
OOP_DAR_E class25_invoke_act(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint32 offset  = 0;

    switch (pOadInfo->pTab->oad.value)
    {
        case 0x45000100:
        case 0x45010100:
        {
            comm_set_rediag(0x45000100 == pOadInfo->pTab->oad.value ? 1 : 2);
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
            break;
        }  
        default:  return DATA_OBJECT_UNAVAILABLE;
    }

    return DATA_SUCCESS;
}


#endif
