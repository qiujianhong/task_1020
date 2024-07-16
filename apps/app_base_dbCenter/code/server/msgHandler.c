/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： msgHander.c
           消息处理接口实现代码
* @date： 2019-11-9
*********************************************************************
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "baseType.h"
#include "constant.h"
#include "dbCenterConfig.h"
#include "dbDataInterface.h"
#include "globalConfiguration.h"
#include "msgHandler.h"
#include "msgService.h"
#include "olog.h"
#include "xdrCustom.h"

static LOGIN_CB_FP s_loginCnFp = NULL;
extern void *db_ocat;
extern void *db_err;
extern void *timeout_ocat;
extern uint8 g_app_id[25];
/**
*********************************************************************
* @brief：      设置登录回调
* @param[in]：
               fp         登录回调函数

* @return：
*********************************************************************
*/
void set_login_cb_fp(LOGIN_CB_FP fp)
{
    s_loginCnFp = fp;
}

/**
*********************************************************************
* @brief： 登录响应
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 login_ack(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint8 loginAck = 0;
    uint8* start_req_xdr = req_xdr;
    loginAck       = pop_u8(&req_xdr);  //获取逻辑设备号
    sint32 dataLen = req_xdr - start_req_xdr;
    if (dataLen != req_xdr_len)
    {
        olog_error(db_ocat, "The login ack message length is invalid");
        return -2;
    }

    if (s_loginCnFp)
    {
        s_loginCnFp(loginAck);
    }
    return 0;
}

/**
*********************************************************************
* @brief： 心跳处理API
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 heartbeat_req(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 len = 0;
    uint8 *tmp = rsp_xdr;
    push_u32(&tmp, 0x01);
    len = tmp - rsp_xdr;
    return len;
}


/**
*********************************************************************
* @brief： 查询版本号API
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 get_version_req(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 len = 0;
    uint8 *tmp = rsp_xdr;

	push_nbyte(&tmp, (uint8*)APP_VERSION, strlen((const sint8 *)APP_VERSION));
    len = tmp - rsp_xdr;
    
    uint32 a, b, c, d, e, f;
    sscanf(APP_PUBLISH_TIME, "%u/%u/%u %u:%u:%u", &a, &b, &c, &d, &e, &f);
    e = 0;
    f = 0;
    
    rsp_xdr[len++] = (uint8)((a & 0xff00) >> 8);
    rsp_xdr[len++] = (uint8)(a & 0xff);
    rsp_xdr[len++] = (uint8)b;
    rsp_xdr[len++] = (uint8)c;
    rsp_xdr[len++] = (uint8)d;
    rsp_xdr[len++] = (uint8)e;
    rsp_xdr[len++] = (uint8)f;
    return len;
}

/**
*********************************************************************
* @brief： 查询app id
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 get_app_id_req(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    uint8 *tmp = rsp_xdr;
    push_nbyte(&tmp, g_app_id, 24);
    return 25;
}


/**
*********************************************************************
* @brief： 读普通数据API
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 read_normal_data(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 ret                 = 0;
    sint32 len                 = 0;
    uint8 *tmp                 = rsp_xdr;
    uint8 *rsp_num_tmp         = rsp_xdr;
    NORMAL_DATA_READ_REQ_T req = {0};
    NORMAL_DATA_READ_RSP_T rsp = {0};
    uint8* start_req_xdr = req_xdr;
    req.logicalDeviceNum = pop_u8(&req_xdr);   //获取逻辑设备号
    req.infoNum          = pop_u16(&req_xdr);  //获取信息点号
    req.dataNum         = pop_u8(&req_xdr);   //获取数据数量
    
#ifndef MSG_SIMULATION_TEST

    sint32 dataLen = req_xdr - start_req_xdr;
    dataLen += req.dataNum*4;

    if (dataLen != req_xdr_len)
    {
        olog_error(db_ocat, "The read normal data message length is invalid");
        return -2;
    }

	tmp++;          //SEQUENCE循环结束再赋值
//	push_u8(&tmp, req.dataNum);

    while(req.dataNum > 0)
    {
        req.dataTag = pop_u32(&req_xdr);  //获取数据标识
        
        ret = m_read_normal_data(&req, 0, &rsp);
        if (0 == ret)
        {
            if ((tmp - rsp_xdr + rsp.bufLen) > MAX_PAYLOAD_LEN)
            {
                olog_warn(db_ocat, "The read content is execcd the max payload length");
                break;
            } 
            else
            {
                // push SEQUENCE
                rsp.dataNum++;
                push_u32(&tmp, rsp.dataTag);               //数据项标识
                push_u16(&tmp, rsp.dataTypeID);            //数据类标识
                push_nbyte(&tmp, rsp.outBuf, rsp.bufLen);  // push数据值响应数组
            }

        }
        else
        {
            ret = m_read_normal_data(&req, 1, &rsp);
            if (0 == ret)
            {
                if ((tmp - rsp_xdr + rsp.bufLen) > MAX_PAYLOAD_LEN)
                {
                    olog_warn(db_ocat, "The read content is execcd the max payload length");
                    break;
                } 
                else
                {
                    // push SEQUENCE
                    rsp.dataNum++;
                    push_u32(&tmp, rsp.dataTag);               //数据项标识
                    push_u16(&tmp, rsp.dataTypeID);            //数据类标识
                    push_nbyte(&tmp, rsp.outBuf, rsp.bufLen);  // push数据值响应数组
                }
            }
            
        }

        if (rsp.outBufFreeFp)
        {
            rsp.outBufFreeFp(rsp.outBuf);  //使用分配者提供的释放函数释放
            rsp.outBufFreeFp = NULL;
        }
    
        req.dataNum--;
    }

    if (rsp.outBufFreeFp)
    {
        rsp.outBufFreeFp(rsp.outBuf);  //使用分配者提供的释放函数释放
        rsp.outBufFreeFp = NULL;
    }

    //SEQUENCE循环结束赋值
    push_u8(&rsp_num_tmp, rsp.dataNum);
    
    
    len = tmp - rsp_xdr;
    
#else
    req.dataTag          = pop_u32(&req_xdr);  //获取数据标识
    push_u8(&tmp, _OPTIONSL_TRUE);
    push_nbyte(&tmp, (uint8 *)"0123456789", strlen("0123456789"));
    len = tmp - rsp_xdr;
#endif

    return len;
}

/**
*********************************************************************
* @brief： 写普通数据API
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    ret             是否成功
*********************************************************************
*/
sint32 write_normal_data(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 ret                       = 0;
    sint32 len                       = 0;
    uint8 *tmp                       = rsp_xdr;
    uint8  successNum                = 0;
    NORMAL_DATA_WRITE_RSP_T req      = {0};
    uint32 *pdataTag                 = NULL;
    int    i                         = 0;
    sint32 dataLen                   = 0;

    if (req_xdr_len > MAX_PAYLOAD_LEN)
    {
        olog_error(db_ocat, "The write normal data message length is invalid");
        return -2;
    }
    
    uint8* start_req_xdr = req_xdr;
    req.logicalDeviceNum = pop_u8(&req_xdr);                  //获取逻辑设备号
    req.infoNum          = pop_u16(&req_xdr);                 //获取信息点号
    req.dataNum          = pop_u8(&req_xdr);                  //获取信息点号

    if (req.dataNum > 0)
    {
        pdataTag = malloc(sizeof(uint32)*req.dataNum);
        if (pdataTag == NULL)
        {
            olog_error(db_ocat, "The write normal data malloc num %u is fail", req.dataNum);
            return -2;
        }

        memset(pdataTag, 0, sizeof(uint32)*req.dataNum);
    }

#ifndef MSG_SIMULATION_TEST
    for(i=0; i<req.dataNum; i++)
    {
        req.dataTag          = pop_u32(&req_xdr);                 //获取数据标识
        req.dataTypeID       = pop_u16(&req_xdr);                 //获取数据Type标识
        req.outBufLen        = pop_nbyte(&req_xdr, &req.outBuf);  //获取需要写入的buf

        dataLen = req_xdr - start_req_xdr;
        if (dataLen > req_xdr_len )
        {
            olog_error(db_ocat, "The write normal data message length is invalid");
			ret = -2;		//置错误标记，记录错误报文
            break;
        }

        ret = m_write_normal_data(&req);
        if (ret == 0)
        {
            pdataTag[successNum] = req.dataTag;
            successNum ++;
        }
    }

    //变更上报
    if (successNum != 0)
    {
        send_data_update_event(req.logicalDeviceNum, req.infoNum, req.dataTypeID, _OPTIONSL_FALSE, 0, pdataTag, successNum);
    }
    
    if (successNum == req.dataNum)
    {
        push_u8(&tmp, _TRUE);
    }
    else
    {
        push_u8(&tmp, _FALSE);
    }
   
    len = tmp - rsp_xdr;
#else
    olog_debug(db_ocat, "req.logicalDeviceNum  = %d", req.logicalDeviceNum);
    olog_debug(db_ocat, "req.infoNum  = %d", req.infoNum);
    olog_debug(db_ocat, "req.dataTag  = %d", req.dataTag);
    olog_debug(db_ocat, "req.outBufLen  = %d", req.outBufLen);
    hdzlog_debug(req.outBuf, req.outBufLen);
    push_u8(&tmp, _TRUE);
    len = tmp - rsp_xdr;
#endif

	if (pdataTag != NULL)
    {
        free(pdataTag);
    }

    if(ret != -2)		//错误标记判断，报文错误时，返回-2，记录错误报文
	{
		ret = len;
	}
    return ret;
}

/**
*********************************************************************
* @brief： 开始读记录型数据API
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 start_read_record_data(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 ret                       = 0;
    sint32 len                       = 0;
    sint32 i                         = 0;
    RECORD_DATA_START_READ_REQ_T req = {0};
    RECORD_DATA_START_READ_RSP_T rsp = {0};
    uint8 *tmp                       = rsp_xdr;

    uint8* start_req_xdr = req_xdr;
    req.logicalDeviceNum = pop_u8(&req_xdr);                                 //获取逻辑设备号
    req.infoNum          = pop_u16(&req_xdr);                                //获取信息点号
    req.queryTimeType    = pop_u8(&req_xdr);                                 //获取查询类型
    if (req.queryTimeType != 255)
    {
        req.startTime = pop_u32(&req_xdr);  //获取起始时间
        req.endTime   = pop_u32(&req_xdr);  //获取结束时间
    }
    req.sortType         = pop_u8(&req_xdr);                                 //获取排序类型
    req.majorDIOptinal       = pop_u8(&req_xdr);
    if (req.majorDIOptinal > 1)
    {
        olog_error(db_ocat, "the message is invalid");
        return -2;
    }
    if(req.majorDIOptinal == 1)
    {
        req.majorDI          = pop_u32(&req_xdr);                //获取主DI
    }     
    req.minorDINum       = pop_nint(&req_xdr, (uint8 **)(&(req.minorDIs)));  //获取次DI

    sint32 dataLen = req_xdr - start_req_xdr;
    if (dataLen != req_xdr_len)
    {
        olog_error(db_ocat, "The start read record data message length is invalid");
        return -2;
    }

    if (req.minorDINum > 0)
    {
        for (i = 0; i < req.minorDINum; i++)
        {
            req.minorDIs[i] = ntohl(req.minorDIs[i]);  //字节序转换
        }
    }

#ifndef MSG_SIMULATION_TEST
    ret = m_start_read_record_data(&req, &rsp);

    if (ret == 0)
    {
        push_u32(&tmp, rsp.handleNum);  //返回句柄
        push_u32(&tmp, rsp.recordNum);  //返回记录条数
    }
    else
    {
        //失败返回0
        push_u32(&tmp, 0);
        push_u32(&tmp, 0);
    }

    len = tmp - rsp_xdr;
#else
    olog_debug(db_ocat, "req.logicalDeviceNum  = %d", req.logicalDeviceNum);
    olog_debug(db_ocat, "req.infoNum  = %d", req.infoNum);
    olog_debug(db_ocat, "req.queryTimeType  = %d", req.queryTimeType);
    olog_debug(db_ocat, "req.startTime  = %d", req.startTime);
    olog_debug(db_ocat, "req.endTime  = %d", req.endTime);
    olog_debug(db_ocat, "req.sortType  = %d", req.sortType);
    olog_debug(db_ocat, "req.minorDINum  = %d", req.minorDINum);
    olog_debug(db_ocat, "req.majorDI  = %d", req.majorDI);
    hdzlog_debug(req.minorDIs, 100);
    for (i = 0; i < req.minorDINum; i++)
    {
        olog_debug(db_ocat, "req.minorDIs[%d]  = %d", i, req.minorDIs[i]);
    }
    push_u32(&tmp, 555);
    push_u32(&tmp, 666);
    len = tmp - rsp_xdr;
#endif

    return len;
}

/**
*********************************************************************
* @brief： 单步读记录型数据API
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 step_read_record(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 ret                    = 0;
    sint32 len                    = 0;
    uint8 *tmp                    = rsp_xdr;
    READ_RECORD_STEP_REQ_T req    = {0};
    READ_RECORD_STEP_RSP_T rsp    = {0};
    uint8 subBuf[MAX_PAYLOAD_LEN-MAX_STEP_HEADER] = {0};//接口协议中 17 =OPTIONAL+信息点号+SEQUENCE +OPTIONAL+数据时标长度
    uint8 *subp                   = subBuf;
    sint32 sunLen                 = 0;
    sint16 itemSum                = 0;
    uint8* start_req_xdr          = req_xdr;

    req.handerNum = pop_u32(&req_xdr);  //获取句柄号
    uint8 *checkP = NULL;
    req.checkInfoOpt       = pop_u8(&req_xdr);
    if (req.checkInfoOpt > 1)
    {
        olog_error(db_ocat, "Invaild message");
        return 0;
    }

    if(req.checkInfoOpt == 1)
    {
        req.checkInfoLen = pop_sequnce_length(&req_xdr);
        if(req.checkInfoLen > sizeof(req.checkInfo))
        {
            push_u8(&tmp, _OPTIONSL_FALSE);
            olog_debug(db_ocat,"step_read_record parameter err!");
            return -2;
        }
		pop_fixbyte(&req_xdr, &checkP, req.checkInfoLen);  //获取效验信息
#ifdef AREA_FUJIAN
        addr_trans_format(checkP, (uint8)req.checkInfoLen, (char*)req.checkInfo, sizeof(req.checkInfo));
        req.checkInfoLen *=2;
#else
		memcpy(req.checkInfo, checkP, req.checkInfoLen);
#endif
    }

    sint32 dataLen = req_xdr - start_req_xdr;
    if (dataLen != req_xdr_len)
    {
        olog_error(db_ocat, "The step read record message length is invalid");
        return -2;
    }
    // req.dataBufLen = pop_u32(&req_xdr);  //获取buff长度

#ifndef MSG_SIMULATION_TEST
    ret = m_step_read_record(&req, &rsp, subp, &sunLen, &itemSum);
    if (ret == 0)
    {
        push_u8(&tmp, _OPTIONSL_TRUE);

        push_u16(&tmp, rsp.infoNum);      //添加信息点号
        push_sequnce_length(&tmp, itemSum);
        push_fixbyte(&tmp, subBuf, sunLen);
        if(rsp.timeFlag)
        {
            push_u8(&tmp, _OPTIONSL_TRUE);
            push_u32(&tmp, rsp.startTime);    //添加起始时间
            push_u32(&tmp, rsp.endTime);      //添加结束时间
            push_u32(&tmp, rsp.storageTime);  //添加存储时间
        }
        else
        {
            push_u8(&tmp, _OPTIONSL_FALSE);
        }

    }
    else
    {
        push_u8(&tmp, _OPTIONSL_FALSE);
    }
    len = tmp - rsp_xdr;
#else
#endif

    return len;
}

/**
*********************************************************************
* @brief： 停止读记录型数据API
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 stop_read_record_data(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 ret = 0;
    sint32 len = 0;
    uint8 *tmp = rsp_xdr;
    RECORD_DATA_STOP_READ_REQ_T req;
    uint8* start_req_xdr = req_xdr;

    req.handleNum = pop_u32(&req_xdr);  //获取数据句柄
    sint32 dataLen = req_xdr - start_req_xdr;
    if (dataLen != req_xdr_len)
    {
        olog_error(db_ocat, "The stop read record data message length is invalid");
        return -2;
    }

#ifndef MSG_SIMULATION_TEST
    ret = m_stop_read_record_data(&req);
    if (ret == 0)
    {
        push_u8(&tmp, _TRUE);
    }
    else
    {
        push_u8(&tmp, _FALSE);
    }
    len = tmp - rsp_xdr;
#else
    olog_debug(db_ocat, "req.handleNum  = %d", req.handleNum);
    push_u8(&tmp, _TRUE);
    len = tmp - rsp_xdr;
#endif

    return len;
}

typedef struct WRITE_RECORD_DATA_INFO
{
    uint8  *pminorDI;
    uint16 data_len;
    uint8  *p_data_buf;
} WRITE_RECORD_DATA_INFO_T;


/**
*********************************************************************
* @brief： 写记录型数据API
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 write_record_data(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 ret                  = 0;
    sint32 len                  = 0;
    uint8 *tmp                  = rsp_xdr;
    RECORD_DATA_WRITE_REQ_T req = {0};
    RECORD_DATA_WRITE_RSP_T rsp = {0};
    uint8* start_req_xdr        = req_xdr;

    req.logicalDeviceNum = pop_u8(&req_xdr);   //获取逻辑设备号

    req.majorDIOptinal   = pop_u8(&req_xdr);   //获取主DI选项
    if (req.majorDIOptinal > 1)
    {
        olog_error(db_ocat, "the message is invalid");
        return -2;
    }
    if (req.majorDIOptinal == _OPTIONSL_TRUE)
    {
        req.majorDI          = pop_u32(&req_xdr);  //获取主DI
        olog_debug(db_ocat,"write_record_data , major DI[%d]!", req.majorDI);
    }
    
    req.minorDIOptinal   = _OPTIONSL_TRUE;
    if (req.minorDIOptinal == _OPTIONSL_TRUE)
    {
        req.minorDI = pop_u32(&req_xdr);  //获取次DI
    }
    req.dataTypeID  = pop_u16(&req_xdr);  //获取数据Type标识
    req.startTime   = pop_u32(&req_xdr);  //获取起始时间
    req.endTime     = pop_u32(&req_xdr);  //获取结束时间
    req.storageTime = pop_u32(&req_xdr);  //获取存储时间
    req.infoNum     = pop_u16(&req_xdr);  //获取信息点号

    uint8 *checkP = NULL;
    req.checkInfoLen = pop_sequnce_length(&req_xdr);
    if(req.checkInfoLen > sizeof(req.checkInfo))
    {
        push_u8(&tmp, _OPTIONSL_FALSE);
        olog_debug(db_ocat,"write_record_data parameter err!");
        return 0;
    }    
    pop_fixbyte(&req_xdr, &checkP, req.checkInfoLen);  //获取效验信息
    memcpy(req.checkInfo, checkP, req.checkInfoLen);

    req.outBufLen = pop_nbyte(&req_xdr, &req.outBuf);  //获取写入内容
    sint32 dataLen = req_xdr - start_req_xdr;
    olog_debug(db_ocat, "datalen[%d], req_len[%d]", dataLen, req_xdr_len);
    if (dataLen != req_xdr_len || req_xdr_len > MAX_PAYLOAD_LEN)
    {
        olog_error(db_ocat, "The write record data message length is invalid");
        return -2;
    }

    ret = m_write_record_data(&req, &rsp);
    if (ret == 0)
    {
        push_u32(&tmp, rsp.storageIndex);  //成功输出索引编号
        // dzlog_debug("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        send_data_update_event(req.logicalDeviceNum, req.infoNum, req.dataTypeID, req.majorDIOptinal, req.majorDI, &req.minorDI, 1);
        // dzlog_debug("================================");
    }
    else
    {
        // push_u16(&tmp, rsp.storageIndex);//失败输出0
        push_u32(&tmp, 0);  //失败输出0
        olog_warn(db_ocat, "write record fail, major DI[%d]",  req.majorDI);
    }
    len = tmp - rsp_xdr;

    return len;
}

/**
*********************************************************************
* @brief： 写记录型数据API,2021-5-30 新规约接口
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 write_record_data_more(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 ret                  = 0;
    sint32 len                  = 0;
    uint8 *tmp                  = rsp_xdr;
    RECORD_DATA_WRITE_REQ_T req = {0};
    RECORD_DATA_WRITE_RSP_T rsp = {0};
    uint8* start_req_xdr        = req_xdr;
    uint8 tmp_data_num          = 0;
    uint8 *pminorDI             = NULL;
    WRITE_RECORD_DATA_INFO_T *p_data_info = NULL;
    uint8 i                     = 0;
    uint8 secc_num              = 0;
    uint32 *psecc_minorDI       = NULL;

    req.logicalDeviceNum = pop_u8(&req_xdr);   //获取逻辑设备号

    req.startTime   = pop_u32(&req_xdr);  //获取起始时间
    req.endTime     = pop_u32(&req_xdr);  //获取结束时间
    req.storageTime = pop_u32(&req_xdr);  //获取存储时间
    req.infoNum     = pop_u16(&req_xdr);  //获取信息点号
    req.dataTypeID  = pop_u16(&req_xdr);  //获取数据Type标识
    
    req.majorDIOptinal   = pop_u8(&req_xdr);   //获取主DI选项
    if (req.majorDIOptinal > 1)
    {
        olog_error(db_ocat, "the message is invalid");
        return -2;
    }
    if (req.majorDIOptinal == _OPTIONSL_TRUE)
    {
        req.majorDI = pop_u32(&req_xdr);  //获取主DI
    }
    
    req.minorDIOptinal = pop_u8(&req_xdr);   //获取次DI数量
    if (req.minorDIOptinal < 1)
    {
        olog_error(db_ocat, "the minorDI num is err");
        return -2;
    }

    //次数据DI起始地址
    pminorDI = req_xdr;
    
    if(req_xdr-start_req_xdr + req.minorDIOptinal*sizeof(uint32) > req_xdr_len)
    {
        push_u8(&tmp, _OPTIONSL_FALSE);
        olog_debug(db_ocat,"write_record_data parameter err!");
        return -2;
    }  
    req_xdr += req.minorDIOptinal*sizeof(uint32);

    //数据域起始地址
    tmp_data_num = pop_u8(&req_xdr); 
    if (req.minorDIOptinal != tmp_data_num)
    {
        olog_error(db_ocat, "the minorDI num[%u]!=data_num[%u]", req.minorDIOptinal, tmp_data_num);
        return -2;
    }

    p_data_info = (WRITE_RECORD_DATA_INFO_T*)malloc(sizeof(WRITE_RECORD_DATA_INFO_T)*req.minorDIOptinal);
    if (p_data_info == NULL)
    {
        olog_error(db_ocat, "write_record_data malloc err!");
        return -1;
    }

    psecc_minorDI = (uint32*)malloc(sizeof(uint32)*req.minorDIOptinal);
    if (psecc_minorDI == NULL)
    {
        olog_error(db_ocat, "write_record_data malloc err!");
		free(p_data_info);
        return -1;
    }

    for(i=0; i<req.minorDIOptinal; i++)
    {
        p_data_info[i].pminorDI = pminorDI;     //获取次DI
        pminorDI += 4;
        p_data_info[i].data_len = pop_sequnce_length(&req_xdr);
        
        if(req_xdr-start_req_xdr + p_data_info[i].data_len > req_xdr_len)
        {
            push_u8(&tmp, _OPTIONSL_FALSE);
            olog_debug(db_ocat,"write_record_data parameter err!");
            free(p_data_info);
			free(psecc_minorDI);
            return -2;
        }
        
        p_data_info[i].p_data_buf = req_xdr;  //获取数据域
        req_xdr += p_data_info[i].data_len;
    }

    uint8 *checkP = NULL;
    req.checkInfoLen = pop_sequnce_length(&req_xdr);
    if(req.checkInfoLen > sizeof(req.checkInfo)  || req_xdr - start_req_xdr + req.checkInfoLen > req_xdr_len || req_xdr_len > MAX_PAYLOAD_LEN)
    {
        push_u8(&tmp, _OPTIONSL_FALSE);
        olog_debug(db_ocat,"write_record_data parameter err!");
        free(p_data_info);
    	free(psecc_minorDI);
        return -2;
    }    
    pop_fixbyte(&req_xdr, &checkP, req.checkInfoLen);  //获取效验信息
    memcpy(req.checkInfo, checkP, req.checkInfoLen);

    
    for(i=0; i<req.minorDIOptinal; i++)
    {
        req.minorDI = pop_u32(&p_data_info[i].pminorDI);  //获取次DI

        req.outBufLen = p_data_info[i].data_len;
        req.outBuf = p_data_info[i].p_data_buf;  //获取写入内容
        p_data_info[i].p_data_buf += req.outBufLen;
        
        ret = m_write_record_data(&req, &rsp);
        if (ret == 0)
        {
            psecc_minorDI[secc_num] = req.minorDI;
            secc_num ++;
        }
        else
        {
            continue;
        }
    }

    if (secc_num > 0)
    {
        send_data_update_event(req.logicalDeviceNum, req.infoNum, req.dataTypeID, req.majorDIOptinal, req.majorDI, psecc_minorDI, secc_num);
    }
    
    if (secc_num == req.minorDIOptinal)
    {
        push_u32(&tmp, rsp.storageIndex);
    }
    else
    {
        push_u32(&tmp, 0);  //失败输出0
    }
    
    len = tmp - rsp_xdr;
	
    free(p_data_info);
    free(psecc_minorDI);
    return len;
}


/**
*********************************************************************
* @brief： 更新记录型数据API
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 update_record_data(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 ret                   = 0;
    sint32 len                   = 0;
    uint8 *tmp                   = rsp_xdr;
    RECORD_DATA_UPDATE_REQ_T req = {0};
    uint8* start_req_xdr         = req_xdr;

    req.logicalDeviceNum = pop_u8(&req_xdr);   //获取逻辑设备号

    req.majorDIOptinal   = pop_u8(&req_xdr);   //获取次DI选项
    if (req.majorDIOptinal > 1)
    {
        olog_error(db_ocat, "the message is invalid");
        return -2;
    }
    if (req.majorDIOptinal == _OPTIONSL_TRUE)
    {
        req.majorDI          = pop_u32(&req_xdr);  //获取主DI
    }
    req.minorDIOptinal   = _OPTIONSL_TRUE;   //获取次DI选项
    if (req.minorDIOptinal == _OPTIONSL_TRUE)
    {
        req.minorDI = pop_u32(&req_xdr);  //获取次DI
    }
    req.screenType = pop_u8(&req_xdr);        //获取更新类型
    if (req.screenType == SCREEN_TYPE_INDEX)  //以索引方式更新
    {
        req.storageIndex = pop_u32(&req_xdr);  //获取索引
    }
    else if (req.screenType == SCREEN_TYPE_STORAGE_TIME)  //以存储时间戳更新
    {
        req.storageTime = pop_u32(&req_xdr);  //获取存储时间
    }
    else
    {
        // error
        olog_warn(db_ocat, "screenType set fial [%d]", req.screenType);
        return -1;
    }
    req.infoNum = pop_u16(&req_xdr);  //获取信息点号

    uint8 *checkP = NULL;
    req.checkInfoLen = pop_sequnce_length(&req_xdr);
    if(req.checkInfoLen > sizeof(req.checkInfo))
    {
        push_u8(&tmp, _OPTIONSL_FALSE);
        olog_debug(db_ocat,"step_read_record parameter err!");
        return -1;
    }    
    pop_fixbyte(&req_xdr, &checkP, req.checkInfoLen);  //获取效验信息
    memcpy(req.checkInfo, checkP, req.checkInfoLen);

    req.outBufLen = pop_nbyte(&req_xdr, &req.outBuf);
    sint32 dataLen = req_xdr - start_req_xdr;
    if (dataLen != req_xdr_len || req_xdr_len > MAX_PAYLOAD_LEN)
    {
        olog_error(db_ocat, "The update record data message length is invalid");
        return -2;
    }

#ifndef MSG_SIMULATION_TEST
    ret = m_update_record_data(&req);

    if (ret >= 0)
    {
        push_u8(&tmp, _TRUE);
        send_data_update_event(req.logicalDeviceNum, req.infoNum, 5, req.majorDIOptinal, req.majorDI, &req.minorDI, 1);        
    }
    else
    {
        push_u8(&tmp, _FALSE);
    }
    len = tmp - rsp_xdr;
#else
    olog_debug(db_ocat, "req.logicalDeviceNum  = %d", req.logicalDeviceNum);
    olog_debug(db_ocat, "req.majorDI  = %d", req.majorDI);
    olog_debug(db_ocat, "req.minorDIOptinal  = %d", req.minorDIOptinal);
    olog_debug(db_ocat, "req.minorDI  = %d", req.minorDI);
    olog_debug(db_ocat, "req.screenType  = %d", req.screenType);
    olog_debug(db_ocat, "req.storageIndex  = %d", req.storageIndex);
    olog_debug(db_ocat, "req.storageTime  = %d", req.storageTime);
    olog_debug(db_ocat, "req.infoNum  = %d", req.infoNum);
    olog_debug(db_ocat, "req.checkInfo  = %s", req.checkInfo);
    olog_debug(db_ocat, "req.outBufLen  = %d", req.outBufLen);
    hdzlog_debug(req.outBuf, req.outBufLen);
    push_u8(&tmp, _TRUE);
    len = tmp - rsp_xdr;
#endif
    return len;
}

/**
*********************************************************************
* @brief： 获取存储数据条数API
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据条数
*********************************************************************
*/
sint32 get_storage_num(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 ret                   = 0;
    sint32 len                   = 0;
    uint8 *tmp                   = rsp_xdr;
    STORAGE_NUMBER_GET_REQ_T req = {0};
    STORAGE_NUMBER_GET_RSP_T rsp = {0};
    uint8* start_req_xdr         = req_xdr;

    req.logicalDeviceNum = pop_u8(&req_xdr);   //获取逻辑设备号]
    req.infoNum          = pop_u16(&req_xdr);  //获取信息点号

	req.majorDIOptinal   = pop_u8(&req_xdr);   //获取次DI选项
    if (req.majorDIOptinal > 1)
    {
        olog_error(db_ocat, "the message is invalid");
        return -2;
    }
    if (req.majorDIOptinal == _OPTIONSL_TRUE)
    {
        req.majorDI          = pop_u32(&req_xdr);  //获取主DI
    }    
    req.minorDIOptinal = _OPTIONSL_TRUE;   //获取次DI选项
    if (req.minorDIOptinal == _OPTIONSL_TRUE)
    {
        req.minorDI = pop_u32(&req_xdr);  //获取次DI
    }

    req.screenType = pop_u8(&req_xdr);
    if (req.screenType == SCREEN_TYPE_ALL)
    {
        // ALL获取
    }
    else if (req.screenType == SCREEN_TYPE_STORAGE_TIME)  //以存储时间戳进行筛选
    {
        req.storageStartTime = pop_u32(&req_xdr);  //获取存储开始时间
        req.storageEndTime   = pop_u32(&req_xdr);  //获取存储结束时间
    }
    else
    {
        // error
        olog_debug(db_ocat, "get_storage_num screenType set fial [%d]", req.screenType);
        push_u32(&tmp, 0);  //失败返回0
        return (tmp - rsp_xdr);
    }

    sint32 dataLen = req_xdr - start_req_xdr;
    if (dataLen != req_xdr_len)
    {
        olog_error(db_ocat, "The get storage num message length is invalid");
        return -2;
    }

#ifndef MSG_SIMULATION_TEST
    ret = m_get_storage_num(&req, &rsp);
    if (ret == 0)
    {
        push_u32(&tmp, rsp.recordNum);
    }
    else
    {
        olog_debug(db_ocat, "get storage num fail");
        push_u32(&tmp, 0);  //失败返回0
    }
    len = tmp - rsp_xdr;
#else
    olog_debug(db_ocat, "req.majorDI  = %d", req.majorDI);
    olog_debug(db_ocat, "req.minorDIOptinal  = %d", req.minorDIOptinal);
    olog_debug(db_ocat, "req.minorDI  = %d", req.minorDI);
    push_u32(&tmp, 888);
    len = tmp - rsp_xdr;
#endif

    return len;
}

/**
*********************************************************************
* @brief： 获取存储记录深度API
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 get_depth_num(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 ret                  = 0;
    sint32 len                  = 0;
    STORAGE_DEPTH_GET_REQ_T req = {0};
    STORAGE_DEPTH_GET_RSP_T rsp = {0};
    uint8 *tmp                  = rsp_xdr;
    uint8* start_req_xdr        = req_xdr;
    
	pop_u8(&req_xdr);	//获取逻辑设备号

	req.majorDIOptinal   = pop_u8(&req_xdr);   //获取主DI选项
    if (req.majorDIOptinal > 1)
    {
        return -2;
    }
    if (req.majorDIOptinal == _OPTIONSL_TRUE)
    {
        req.majorDI          = pop_u32(&req_xdr);  //获取主DI
    }
    req.minorDIOptinal = _OPTIONSL_TRUE;   //获取次DI选项
    if (req.minorDIOptinal == _OPTIONSL_TRUE)
    {
        req.minorDI = pop_u32(&req_xdr);  //获取次DI
    }

    olog_debug(db_ocat, "%s, majorDI[%d], minorDI[%d]", __FUNCTION__, req.majorDI, req.minorDI);
    sint32 dataLen = req_xdr - start_req_xdr;
    if (dataLen != req_xdr_len)
    {
        olog_error(db_ocat, "The get depth num message length is invalid");
        return -2;
    }

#ifndef MSG_SIMULATION_TEST
    ret = m_get_depth_num(&req, &rsp);
    if (ret == 0)
    {
        push_u32(&tmp, rsp.depthNum);
    }
    else
    {
        push_u32(&tmp, 0);  //失败返回0
        olog_debug(db_ocat, "get depth num fail");
    }
    len = tmp - rsp_xdr;
#else
    olog_debug(db_ocat, "req.majorDI  = %d", req.majorDI);
    olog_debug(db_ocat, "req.minorDIOptinal  = %d", req.minorDIOptinal);
    olog_debug(db_ocat, "req.minorDI  = %d", req.minorDI);
    push_u32(&tmp, 999);
    len = tmp - rsp_xdr;
#endif
    return len;
}


/**
*********************************************************************
* @brief： 数据初始化API 2021-5-30 新规约接口,保留6—不可初始化事件数据
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 data_init_new(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 len      = 0;
    sint32 ret      = 0;
    uint8 *tmp      = rsp_xdr;
    DB_INIT_REQ_T req = {0};
    uint8* start_req_xdr = req_xdr;

    req.logicalDeviceNum = pop_u8(&req_xdr);  //获取逻辑设备号
    req.optinal = pop_u8(&req_xdr);          //获取数据Type标识Optional
    if(req.optinal)
    {
        req.dataTypeID = pop_u16(&req_xdr);          //获取数据Type标识
    }

    sint32 dataLen = req_xdr - start_req_xdr;
    if (dataLen != req_xdr_len)
    {
        olog_error(db_ocat, "The data init message length is invalid");
        return -2;
    }
#ifndef MSG_SIMULATION_TEST
    olog_warn(db_ocat, "data_init start deviceID = %d, req.optinal=%d, req.dataTypeID=%d", req.logicalDeviceNum, req.optinal, req.dataTypeID);
    ret = m_data_init_new(&req);
    if (ret == 0)
    {
        push_u8(&tmp, _TRUE);
        send_db_init_event(0, req.logicalDeviceNum, req.optinal, req.dataTypeID);
    }
    else
    {
        push_u8(&tmp, _FALSE);
    }
    len = tmp - rsp_xdr;
#else
    olog_debug(db_ocat, "deviceID = %d", req.logicalDeviceNum);
    push_u8(&tmp, _TRUE);
    len = tmp - rsp_xdr;
#endif
	olog_warn(db_ocat, "data_init end return = %d", ret);

    return len;
}



/**
*********************************************************************
* @brief： 检查数据是否是普通型数据
* @param[in]：
* @param[out]：
* @return：   
*********************************************************************
*/
sint32 check_data_normal(uint8 logicalDeviceNum, uint16 infoNum, uint32 majorDI)
{
    NORMAL_DATA_READ_REQ_T req = {0};
    NORMAL_DATA_READ_RSP_T rsp = {0};
    req.logicalDeviceNum = logicalDeviceNum;   //获取逻辑设备号
    req.infoNum          = infoNum;            //获取信息点号
    req.dataTag          = majorDI;            //获取数据项标识

    sint32 ret = 0;
    ret = m_read_normal_data(&req, 0, &rsp);
    if(ret < 0)
    {
        ret = m_read_normal_data(&req, 1, &rsp);
    }
    
    if(ret < 0)
    {
        return -1;
    }
    
    return rsp.dataTypeID;
}

/**
*********************************************************************
* @brief： 带参数指定数据初始化API
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 data_init_by_parameter(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 len          = 0;
    sint32 ret          = 0;
    uint8 *tmp          = rsp_xdr;
    DATA_INIT_REQ_T req = {};

    uint8* start_req_xdr = req_xdr;

    req.logicalDeviceNum = pop_u8(&req_xdr);   //获取逻辑设备号
    req.infoNum          = pop_u16(&req_xdr);  //获取信息点号 

	req.dataTypeID       = pop_u16(&req_xdr);          //获取数据Type标识

    req.choice          = pop_u8(&req_xdr);  //选项
	if(req.choice == 1)
    {
        req.majorDIOptinal   = pop_u8(&req_xdr);   //获取主DI选项
        if (req.majorDIOptinal > 1)
        {
            olog_error(db_ocat, "the message is invalid");
            return -2;
        }
        if (req.majorDIOptinal == _OPTIONSL_TRUE)
        {
            req.majorDI          = pop_u32(&req_xdr);  //获取主DI
        }
        req.minorDIOptinal   = _OPTIONSL_TRUE;
        if (req.minorDIOptinal == _OPTIONSL_TRUE)  //
        {
            req.minorDI = pop_u32(&req_xdr);  //获取次DI
        }
    }
    else if(req.choice != 0)
    {
        olog_debug(db_ocat,"data_init_by_parameter param error");        
        return -2;
    }

    sint32 dataLen = req_xdr - start_req_xdr;
    if (dataLen != req_xdr_len)
    {
        olog_error(db_ocat, "The data init by param message length is invalid");
        return -2;
    }
    

#ifndef MSG_SIMULATION_TEST
    ret = m_data_init_by_parameter(&req, false);
    if (ret == 0)
    {
        push_u8(&tmp, _TRUE);
        
        if(req.choice == 1)
        {
            send_data_update_event(req.logicalDeviceNum, req.infoNum, req.dataTypeID, req.majorDIOptinal, req.majorDI, &req.minorDI, 1);
        }
        else
        {
            if( req.infoNum == 0xffff )
            {
                send_db_init_event(0, req.logicalDeviceNum, 1, req.dataTypeID);
            }
			else
			{
                send_data_update_event(req.logicalDeviceNum, req.infoNum, req.dataTypeID, req.majorDIOptinal, req.majorDI, &req.minorDI, 0);
            }
            
        }               
    }
    else
    {
        push_u8(&tmp, _FALSE);
    }
    len = tmp - rsp_xdr;
#else
    olog_debug(db_ocat, "req.logicalDeviceNum  = %d", req.logicalDeviceNum);
    olog_debug(db_ocat, "req.infoNum  = %d", req.infoNum);
    olog_debug(db_ocat, "req.majorDI  = %d", req.majorDI);
    olog_debug(db_ocat, "req.minorDIOptinal  = %d", req.minorDIOptinal);
    olog_debug(db_ocat, "req.minorDI  = %d", req.minorDI);
    push_u8(&tmp, _TRUE);
    len = tmp - rsp_xdr;
#endif

    return len;
}

/**
*********************************************************************
* @brief： 恢复出厂设置API 2021-5-30 新规约接口,保留6—不可初始化事件数据
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 factory_Reset_new(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 len     = 0;
    sint32 ret     = 0;
    uint8 *tmp     = rsp_xdr;
    uint8* start_req_xdr = req_xdr;

    uint8 deviceID = pop_u8(&req_xdr);  //获取逻辑设备号
    sint32 dataLen = req_xdr - start_req_xdr;
    if (dataLen != req_xdr_len)
    {
        olog_error(db_ocat, "The factory reset message length is invalid");
        return -2;
    }
#ifndef MSG_SIMULATION_TEST
    olog_warn(db_ocat, "factory reset start deviceID = %d", deviceID);
    ret = m_factory_Reset_new(deviceID);
    if (ret == 0)
    {
        push_u8(&tmp, _TRUE);
        send_db_init_event(1, deviceID, 0, 0);
    }
    else
    {
        push_u8(&tmp, _FALSE);
    }
    len = tmp - rsp_xdr;
#else
    olog_debug(db_ocat, "factory reset deviceID = %d", deviceID);
    push_u8(&tmp, _TRUE);
    len = tmp - rsp_xdr;
#endif
	olog_warn(db_ocat, "factory reset end return = %d", ret);

    return len;
}


#ifdef AREA_FUJIAN
/**
*********************************************************************
* @brief： 开始读记录型数据API,福建集中器扩展接口
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 start_read_record_data_fj(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 ret                       = 0;
    sint32 len                       = 0;
    sint32 i                         = 0;
    RECORD_DATA_START_READ_REQ_T req = {0};
    RECORD_DATA_START_READ_RSP_T rsp = {0};
    uint8 *tmp                       = rsp_xdr;

    uint8* start_req_xdr = req_xdr;
    req.logicalDeviceNum = pop_u8(&req_xdr);                                 //获取逻辑设备号
    req.infoNum          = pop_u16(&req_xdr);                                //获取信息点号
    req.addrLen 		 = pop_nbyte(&req_xdr, &(req.pAddr));  				 //表地址，长度为0表示所有
    req.queryTimeType		 = pop_u8(&req_xdr);							 //筛选方式,福建扩展接口只支持
	if (req.queryTimeType != 0xff)
    {
        req.startTime 		 = pop_u32(&req_xdr);  							 //获取起始时间
    	req.endTime 		 = pop_u32(&req_xdr); 							  //获取结束时间
    }
    req.sortType         = pop_u8(&req_xdr);                                 //获取排序类型
    req.majorDIOptinal       = pop_u8(&req_xdr);
    if (req.majorDIOptinal > 1)
    {
        olog_error(db_ocat, "the message is invalid");
        return -2;
    }
    if(req.majorDIOptinal == 1)
    {
        req.majorDI          = pop_u32(&req_xdr);                //获取主DI
    }  
    req.minorDINum		 = pop_nint(&req_xdr, (uint8 **)(&(req.minorDIs)));  //获取次DI，0表示所有

    sint32 dataLen = req_xdr - start_req_xdr;
    if (dataLen != req_xdr_len)
    {
        olog_error(db_ocat, "The start read record data message length is invalid");
        return -2;
    }

    if (req.minorDINum > 0)
    {
        for (i = 0; i < req.minorDINum; i++)
        {
            req.minorDIs[i] = ntohl(req.minorDIs[i]);  //字节序转换
        }
    }

    ret = m_start_read_record_data_fj(&req, &rsp);

    if (ret == 0)
    {
        push_u32(&tmp, rsp.handleNum);  //返回句柄
        push_u32(&tmp, rsp.recordNum);  //返回记录条数
    }
    else
    {
        //失败返回0
        push_u32(&tmp, 0);
        push_u32(&tmp, 0);
    }

    len = tmp - rsp_xdr;

    return len;
}

/**
*********************************************************************
* @brief： 带参数指定数据初始化API
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 data_init_by_parameter_fj(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 len          = 0;
    sint32 ret          = 0;
    uint8 *tmp          = rsp_xdr;
    DATA_INIT_REQ_T_FJ req = {};

    uint8* start_req_xdr = req_xdr;

    req.logicalDeviceNum = pop_u8(&req_xdr);   //获取逻辑设备号
    req.infoNum          = pop_u16(&req_xdr);  //获取信息点号 
	req.majorDIOptinal   = pop_u8(&req_xdr);   //获取主DI选项
    if (req.majorDIOptinal > 1)
    {
        olog_error(db_ocat, "the message is invalid");
        return -2;
    }
    if (req.majorDIOptinal == _OPTIONSL_TRUE)
    {
        req.majorDI          = pop_u32(&req_xdr);  //获取主DI
    }
    req.minorDI = pop_u32(&req_xdr);  //获取次DI

	req.queryTimeType = pop_u8(&req_xdr);   //筛选方式
    if (req.queryTimeType != 0xff)
    {
        req.storageStartTime = pop_u32(&req_xdr);  //存储开始时间
		req.storageEndTime = pop_u32(&req_xdr);  //存储结束时间
    }
	
    sint32 dataLen = req_xdr - start_req_xdr;
    if (dataLen != req_xdr_len)
    {
        olog_error(db_ocat, "The data init by param message length is invalid");
        return -2;
    }
    
    ret = m_data_init_by_parameter_fj(&req);
    if (ret == 0)
    {
        push_u8(&tmp, _TRUE);
        
        send_data_update_event(req.logicalDeviceNum, req.infoNum, 5, req.majorDIOptinal, req.majorDI, &req.minorDI, 1);             
    }
    else
    {
        push_u8(&tmp, _FALSE);
    }
    len = tmp - rsp_xdr;

    return len;
}

#endif

/**
*********************************************************************
* @brief： 设置更新通知API
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
sint32 set_subscription(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, void *subscriber)
{
    sint32 len             = 0;
    sint32 ret             = 0;
    sint8 *subscriber_name = (sint8 *)subscriber;  //订阅者名称
    uint8 *tmp             = rsp_xdr;
    uint8* start_req_xdr   = req_xdr;

    uint8 deviceID         = pop_u8(&req_xdr);  //获取订阅的逻辑设备
    uint16 *infoNums       = NULL;
    uint8 infoNumNum       = pop_nshort(&req_xdr, (uint8 **)(&infoNums));  //获取订阅的信息点数组
    uint32 *dataITDs       = NULL;
    uint8 dataIDNum        = pop_nint(&req_xdr, (uint8 **)(&dataITDs));  //获取订阅的数据标识数组

    sint32 dataLen = req_xdr - start_req_xdr;
    if (dataLen != req_xdr_len)
    {
        olog_error(db_ocat, "The set subscription message length is invalid");
        return -2;
    }


#ifndef MSG_SIMULATION_TEST
    ret = msg_subscribe(subscriber_name, deviceID, infoNums, infoNumNum, dataITDs, dataIDNum);
    if (ret == 0)
    {
        push_u8(&tmp, _TRUE);
    }
    else
    {
        push_u8(&tmp, _FALSE);
    }
    len = tmp - rsp_xdr;
#else

    olog_debug(db_ocat, "subscriber_name = %s", subscriber_name);
    olog_debug(db_ocat, "deviceID = %d", deviceID);
    olog_debug(db_ocat, "infoNumNum = %d", infoNumNum);
    for (i = 0; i < infoNumNum; i++)
    {
        infoNums[i] = ntohs(infoNums[i]);
        olog_debug(db_ocat, "infoNums[%d]  = %d", i, infoNums[i]);
    }
    olog_debug(db_ocat, "dataIDNum = %d", dataIDNum);
    for (i = 0; i < dataIDNum; i++)
    {
        dataITDs[i] = ntohl(dataITDs[i]);
        olog_debug(db_ocat, "dataITDs[%d]  = %d", i, dataITDs[i]);
    }
    push_u8(&tmp, _TRUE);
    len = tmp - rsp_xdr;
#endif

    return len;
}

/**
*********************************************************************
* @brief： 开始单步写记录型数据API
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 start_write_record_data(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 ret = 0;
    sint32 len = 0;
    // RECORD_DATA_START_READ_REQ_T req = {0};
    RECORD_DATA_START_WRITE_RSP_T rsp = {0};
    uint8 *tmp                        = rsp_xdr;

    ret = m_start_write_record_data(&rsp);
    if (ret == 0)
    {
        push_u32(&tmp, rsp.handleNum);  //返回句柄
        // push_u32(&tmp, rsp.recordNum);//返回记录条数
    }
    else
    {
        //失败返回0
        push_u32(&tmp, 0);
        // push_u32(&tmp, 0);
    }

    len = tmp - rsp_xdr;

    return len;
}

/**
*********************************************************************
* @brief： 单步写记录型数据API
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 step_write_record_data(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 ret                  = 0;
    sint32 len                  = 0;
    WRITE_RECORD_STEP_REQ_T req = {0};
    sint32 rsp                  = {0};  // Index No
    uint8 *tmp                  = rsp_xdr;
    uint8* start_req_xdr        = req_xdr;

    req.handerNum               = pop_u32(&req_xdr);  //句柄
    req.logi_devNo              = pop_u8(&req_xdr);   //获取逻辑设备号
    req.majorDI                 = pop_u32(&req_xdr);  //获取主DI
    req.minorDIOptinal          = pop_u8(&req_xdr);   //获取次DI选项
    if (req.minorDIOptinal == _OPTIONSL_TRUE)
    {
        req.minorDI = pop_u32(&req_xdr);  //获取次DI
    }
    req.startTime   = pop_u32(&req_xdr);  //获取起始时间
    req.endTime     = pop_u32(&req_xdr);  //获取结束时间
    req.storageTime = pop_u32(&req_xdr);  //获取结束时间

    req.infoNum = pop_u16(&req_xdr);  //获取信息点号

    uint8 *checkP = NULL;
    req.checkInfoLen = pop_sequnce_length(&req_xdr);
    if(req.checkInfoLen > sizeof(req.checkInfo))
    {
        push_u8(&tmp, _OPTIONSL_FALSE);
        olog_debug(db_ocat,"step_write_record_data parameter err!");
        return 0;
    }
    pop_fixbyte(&req_xdr, &checkP, req.checkInfoLen);  //获取效验信息
    memcpy(req.checkInfo, checkP, req.checkInfoLen);
    // req.dataBufLen = pop_u32(&req_xdr);//获取buff长度
    req.outBufLen = pop_nbyte(&req_xdr, &req.outBuf);

    printf("req.handerNum:%u \n", req.handerNum);

    sint32 dataLen = req_xdr - start_req_xdr;
    if (dataLen != req_xdr_len)
    {
        olog_error(db_ocat, "The step write record data message length is invalid");
        return -2;
    }

    ret = m_step_write_record_data(&req, &rsp);

    if (ret == 0)
    {
        push_u32(&tmp, rsp);  //返回Ｉｎｄｅｘ
        send_data_update_event(req.logi_devNo, req.infoNum, 5, req.minorDIOptinal, req.majorDI, &req.minorDI, 1);
    }
    else
    {
        //失败返回0
        push_u32(&tmp, 0);
    }

    len = tmp - rsp_xdr;

    return len;
}
/**
*********************************************************************
* @brief： 结束单步写记录型数据API
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
sint32 fin_step_write_record_data(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len)
{
    sint32 ret = 0;
    sint32 len = 0;
    uint8 *tmp = rsp_xdr;
    uint8* start_req_xdr = req_xdr;
    WRITE_RECORD_STEP_REQ_T req = {0};
    sint8 rsp                   = {0};

    req.handerNum = pop_u32(&req_xdr);  //获取句柄号
    sint32 dataLen = req_xdr - start_req_xdr;
    if (dataLen != req_xdr_len)
    {
        olog_error(db_ocat, "The fin step write record_data message length is invalid");
        return -2;
    }

    ret = m_fin_step_write_record_data(&req, &rsp);
    if (ret == 0)
    {
        push_u8(&tmp, _OPTIONSL_TRUE);
    }
    else
    {
        push_u8(&tmp, _OPTIONSL_FALSE);
    }

    len = tmp - rsp_xdr;

    return len;
}

/**
*********************************************************************
* @brief： ERROR MSG API
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
static sint32 error_msg_handler(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, uint32 tag)
{
    sint32 len = 0;
    uint8 *tmp = rsp_xdr;

    push_u32(&tmp, htonl(tag));
    push_u16(&tmp, -1);  //错误码-1

    len = tmp - rsp_xdr;

    return len;
}

/**
*********************************************************************
* @brief： 选择消息接口处理函数API
* @param[in]：
               tag          消息标签
* @param[out]：
               none
* @return：    XDR_HANDLE    消息处理句柄号
*********************************************************************
*/
XDR_HANDLE select_handle_func(uint32 tag)
{
    XDR_HANDLE handle = NULL;
    uint16 IID        = tag >> 16;     //消息的IID
    uint16 IOP        = tag & 0xffff;  //消息的IOP

#ifdef SQL_DUMP_DEBUG
    olog_error(db_ocat, "select_handle_func IID:%02x **  IOP:%02x \n", IID, IOP);
#endif

    switch (IID)
    {
        case 0x0000:
            switch (IOP)
            {
                case 0x0010:  //心跳
                    handle = heartbeat_req;
                    break;
                case 0x0013:    //版本信息
                    handle = get_version_req;
                    break;
                case 0x0014:    //错误信息,不处理
                    break;
                case 0x0016:    //APP id
                    handle = get_app_id_req;
                    break;
                default:
                    olog_warn(db_ocat, "unidentification IOP[0x%x]", IOP);
                    break;
            }
            break;

        case 0x0001:
            switch (IOP)
            {
                case 0x0010:  // register ack
                    handle = login_ack;
                    break;
                default:
                    olog_warn(db_ocat, "unidentification IOP[0x%x]", IOP);
                    break;
            }
            break;
        case 0x05:
            switch (IOP)
            {
                case 0x0010:
                    handle = read_normal_data;
                    break;
                case 0x0011:
                    handle = write_normal_data;
                    break;
                case 0x0012:
                    handle = start_read_record_data;
                    break;
                case 0x0013:
                    handle = step_read_record;
                    break;
                case 0x0014:
                    handle = stop_read_record_data;
                    break;
                case 0x0015:
                    handle = write_record_data_more;
                    break;
                case 0x0016:
                    handle = update_record_data;
                    break;
                case 0x0020:
                    handle = get_storage_num;
                    break;
                case 0x0021:
                    handle = get_depth_num;
                    break;
                case 0x0030:
                    handle = data_init_new;
                    break;
                case 0x0031:
                    handle = data_init_by_parameter;
                    break;
                case 0x0032:
                    handle = factory_Reset_new;
                    break;
#ifdef AREA_FUJIAN
				case 0xf012:
                    handle = start_read_record_data_fj;
                    break;
				case 0xf031:
                    handle = data_init_by_parameter_fj;
#endif

                // case 0x40: //单独处理
                //     handle = set_subscription;
                //     break;
//                case 0x42:
//                    handle = start_write_record_data;
//                    break;
//                case 0x43:
//                    handle = step_write_record_data;
//                    break;
//                case 0x44:
//                    handle = fin_step_write_record_data;
//                    break;
                default:
                    olog_warn(db_ocat, "unidentification IOP[0x%x]", IOP);
                    break;
            }
            break;

        default:
            olog_warn(db_ocat, "unidentification IID[0x%x]", IID);
            break;
    }
    return handle;
}

#include <time.h>
#define MAXBUFSIZE 1024

typedef  struct iostat       
{
    char avg_cpu_name[MAXBUFSIZE];
    char avg_cpu[MAXBUFSIZE];
    char iostat_name[MAXBUFSIZE];
    char iostat[MAXBUFSIZE];
    char iostat2[MAXBUFSIZE];
} CPU_IOSTAT ;  

typedef struct PACKED         
{
    char name[20]; 
    long total; 
    char name2[20];
    long free;            
}MEM_OCCUPY;

void get_occupy(char * scup, int len)
{  
    FILE *fd;                                                                                 
    fd = fopen ("/proc/loadavg", "r"); //这里只读取loadavg文件的第一行及cpu总信息，如需获取每核cpu的使用情况，请分析stat文件的接下来几行。
    if (!fd)  
    {
        fd = fopen ("/proc/loadavg", "r");
    }

    if (!fd)  
        return ; 
    fgets(scup, len, fd); 
   fclose(fd);
}  
void get_mem_occupy(MEM_OCCUPY * mem){
    FILE * fd;
    char buff[MAXBUFSIZE];
    fd = fopen("/proc/meminfo","r");
    if (!fd)  
    {
        fd = fopen ("/proc/meminfo", "r");
    }

    if (!fd)  
        return ; 
    
    fgets (buff, sizeof(buff), fd); 
    sscanf (buff, "%s %ld", mem->name,&mem->total);  
    fgets (buff, sizeof(buff), fd); 
    sscanf (buff, "%s %ld", mem->name2,&mem->free); 
    fclose(fd);
}

void get_io_occupy(CPU_IOSTAT *pstat){
    char cmd[] ="iostat -c -d /dev/mmcblk0p7 -d /dev/mmcblk0p9 -k 1 2";
    char buffer[MAXBUFSIZE];  
    FILE* pipe = popen(cmd, "r");  
    if (!pipe)  
    {
        pipe = popen(cmd, "r");
    }
    if (!pipe)  return ;    
    fgets(buffer, sizeof(buffer), pipe);
    fgets(buffer, sizeof(buffer), pipe);
    fgets(buffer, sizeof(buffer), pipe);
    fgets(buffer, sizeof(buffer), pipe);

    fgets(buffer, sizeof(buffer), pipe);
    fgets(buffer, sizeof(buffer), pipe);
    fgets(buffer, sizeof(buffer), pipe);
    fgets(buffer, sizeof(buffer), pipe);

    fgets(buffer, sizeof(buffer), pipe);
    fgets(pstat->avg_cpu_name, MAXBUFSIZE, pipe);
    fgets(pstat->avg_cpu, MAXBUFSIZE, pipe);
    
    fgets(buffer, sizeof(buffer), pipe);
    fgets(pstat->iostat_name, MAXBUFSIZE, pipe);
    fgets(pstat->iostat, MAXBUFSIZE, pipe);
    fgets(pstat->iostat2, MAXBUFSIZE, pipe);
    pclose(pipe);
    return ;
}

void get_vmstat_occupy(CPU_IOSTAT *pstat){
    char cmd[] ="vmstat 1 2";
    char buffer[MAXBUFSIZE];  

    FILE* pipe = popen(cmd, "r");  
    if (!pipe)  
    {
        pipe = popen(cmd, "r");
    }
    if (!pipe)  return ;

    fgets(pstat->avg_cpu_name, MAXBUFSIZE, pipe);
    fgets(pstat->avg_cpu, MAXBUFSIZE, pipe);
    fgets(buffer, sizeof(buffer), pipe);
    fgets(pstat->iostat_name, MAXBUFSIZE, pipe);
    pclose(pipe);
    return ;
}

void get_disk_occupy(char ** reused){
    char cmd[50]="df /data/app/";
    
    char buffer[MAXBUFSIZE];
    FILE* pipe = popen(cmd, "r");    
    char fileSys[20];
    char blocks[20];
    char used[20];
    char free[20];
    char percent[10];
    char moment[20];
    
    if (!pipe)  return ;  
    if(fgets(buffer, sizeof(buffer), pipe)!=NULL){
        sscanf(buffer,"%s %s %s %s %s %s",fileSys,blocks,used,free,percent,moment);
    }
    if(fgets(buffer, sizeof(buffer), pipe)!=NULL){
        sscanf(buffer,"%s %s %s %s %s %s",fileSys,blocks,used,free,percent,moment);
    }
    strcpy(*reused,percent);
    pclose(pipe);
    return ;
}

void get_status()
{  
    CPU_IOSTAT ocpu;
    MEM_OCCUPY mem;
    char cpu_user[128];

    //获取cpu使用率
    memset(cpu_user, 0, 128);
    get_occupy(cpu_user, 128); 
    olog_warn(timeout_ocat, "cpu used:%s", cpu_user);

    //获取内存使用率
    memset(&mem, 0, sizeof(MEM_OCCUPY));
    get_mem_occupy(&mem);

    double using = ((double)(mem.total - mem.free)/mem.total)*100;
    olog_warn(timeout_ocat, "mem used:%4.2f\n",using);

    //获取io使用率
    memset(&ocpu, 0, sizeof(CPU_IOSTAT));
    get_io_occupy(&ocpu);
    olog_warn(timeout_ocat, "io used:%s", ocpu.avg_cpu_name);
    olog_warn(timeout_ocat, "io used:%s", ocpu.avg_cpu);
    olog_warn(timeout_ocat, "io used:%s", ocpu.iostat_name);
    olog_warn(timeout_ocat, "io used:%s", ocpu.iostat);
    olog_warn(timeout_ocat, "io used:%s", ocpu.iostat2);

    //获取vmstat使用率
    memset(&ocpu, 0, sizeof(CPU_IOSTAT));
    get_vmstat_occupy(&ocpu);
    olog_warn(timeout_ocat, "vmstat:%s", ocpu.avg_cpu_name);
    olog_warn(timeout_ocat, "vmstat:%s", ocpu.avg_cpu);
    olog_warn(timeout_ocat, "vmstat:%s", ocpu.iostat_name);

    //获取当前磁盘的使用率
    char t[20]="";
    char *used = t;
    get_disk_occupy(&used);

    olog_warn(timeout_ocat, "disk used:%s\n\n",used);

}

/*******************************************************************
* @brief： 消息处理
* @param[in]：
               RAW_OBJ_T *rawObjReq    //请求消息报文
               RAW_OBJ_T *rawObjRsp    //消息处理后的回复消息报文
               MSG_OBJ_T *msgReq       //解析后的请求消息结构体
               MSG_OBJ_T *msgRsp       //消息处理后的回复消息结构体
               MSG_CLIENT_T *client    //通信客户端句柄
* @param[out]：none
* @return： 返回值说明
*********************************************************************/
void msg_handler(RAW_OBJ_T *rawObjReq, RAW_OBJ_T *rawObjRsp, MSG_OBJ_T *msgReq, MSG_OBJ_T *msgRsp, MSG_CLIENT_T *client)
{
    XDR_HANDLE msgHandle = NULL;
    sint32 ackLen        = 0;
    uint32 errAckLen     = 0;
    uint8 ackPayload[MAX_PAYLOAD_LEN];

    UNUSED(rawObjReq);
    rawObjRsp->length = 0;
    //发送给smiOS需要处理的消息
    // if (0 == strncmp(g_configObj.serviceCfg.serverName, msgReq->destination,
    //                  sizeof(g_configObj.serviceCfg.serverName)))
    {
        if (msgReq->tag == SUB_MSG_TAG)  //订阅消息单独处理
        {
            ackLen = set_subscription(msgReq->payload, msgReq->length, ackPayload, sizeof(ackPayload), msgReq->source);
        }
        else
        {
            msgHandle = select_handle_func(msgReq->tag);
            if (NULL == msgHandle)
            {
                olog_error(db_ocat, "Unsupported tag:[%08X]", msgReq->tag);
                if(msgReq->tag != 0x00000014)  //错误信息,不处理
                {
                    errAckLen =
                        error_msg_handler(msgReq->payload, msgReq->length, ackPayload, sizeof(ackPayload), msgReq->tag);
                    if (errAckLen > 0)
                    {
                        *msgRsp             = *msgReq;
                        msgRsp->prm         = (msgReq->prm&0xfe);
                        msgRsp->destination = msgReq->source;
                        msgRsp->source      = g_configObj.serviceCfg.serverName;
                        msgRsp->tag         = 0x00000014;
                        build_msg(msgRsp, ackPayload, errAckLen, rawObjRsp);
                        holog_debug(db_ocat, rawObjRsp->payload, rawObjRsp->length);
                    }
                }
                    
            }
            else
            {
                struct timespec starttime;
                clock_gettime(CLOCK_MONOTONIC_RAW, &starttime);
                memset(ackPayload, 0, sizeof(ackPayload));
                ackLen = msgHandle(msgReq->payload, msgReq->length, ackPayload, sizeof(ackPayload));
                struct timespec endtime;
                clock_gettime(CLOCK_MONOTONIC_RAW, &endtime);

                unsigned long long  startNS = (unsigned long long)starttime.tv_sec * 1000 * 1000 * 1000 + starttime.tv_nsec;
                unsigned long long  endNS = (unsigned long long)endtime.tv_sec * 1000 * 1000 * 1000 + endtime.tv_nsec;
                unsigned long long  sec = (endNS - startNS) / (1000 * 1000 * 1000);
                if (sec > 2) 
				{
                    unsigned long long  nsec = (endNS - startNS) % (1000 * 1000 * 1000);
                    olog_warn(timeout_ocat, "消息发送方[%s] tag[0x%08x] TO sec[%llu], nsec[%llu]!", msgReq->source, msgReq->tag, sec, nsec);
					holog_debug(timeout_ocat, rawObjReq->payload, rawObjReq->length);
//                    get_status();
                }
            }
        }

        if (ackLen > 0)
        {
            *msgRsp             = *msgReq;
            msgRsp->prm         = (msgReq->prm&0xfe);
            msgRsp->destination = msgReq->source;
            msgRsp->source      = g_configObj.serviceCfg.serverName;
            build_msg(msgRsp, ackPayload, ackLen, rawObjRsp);
            holog_debug(db_ocat, rawObjRsp->payload, rawObjRsp->length);
        }
        else if (ackLen == -2) /* The message is invalid */
        {
        	olog_error(db_err, "收到错误或非法消息：");
        	holog_error(db_err, rawObjReq->payload, rawObjReq->length);
            memset(ackPayload, 0, sizeof(ackPayload));
            errAckLen = error_msg_handler(msgReq->payload, msgReq->length, ackPayload, sizeof(ackPayload), msgReq->tag);
            if (errAckLen > 0)
            {
                *msgRsp             = *msgReq;
                msgRsp->prm         = (msgReq->prm&0xfe);
                msgRsp->destination = msgReq->source;
                msgRsp->source      = g_configObj.serviceCfg.serverName;
                msgRsp->tag         = 0x00000014;
                build_msg(msgRsp, ackPayload, errAckLen, rawObjRsp);
            }

        }
    }
}
