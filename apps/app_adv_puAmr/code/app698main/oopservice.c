/*
*********************************************************************
* @file    oopservice.c
* @brief： apdu应用服务处理
* @author：贺宁
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/

#include "baseCommon.h"
#include "md5sum.h"

#include "OopAll.h"
#include "EventIO.h"
#include "appcomm.h"
#include "apducomm.h"
#include "Event.h"
#include "AutoTask.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "macro.h"
#include "oopStorage.h"

extern ProxyLockQueue ProxyQueue;
extern ProxyLockQueue ProxyrespondeQueue;

#if DESC("规约解析全局变量定义",1)

/** @brief 接口类处理函数 */
const PRTL_IC_OPERATE_T g_prtl_ic_op[OOP_IC_MAX]=
{
    {1 , class1_invoke_get,  class1_invoke_set, class1_invoke_act},
    {2 , class2_invoke_get,  class2_invoke_set, class2_invoke_act},
    {3 , class3_invoke_get,  class3_invoke_set, class3_invoke_act},
    {4 , class4_invoke_get,  class4_invoke_set, class4_invoke_act},
    {5 , class5_invoke_get,  class5_invoke_set, class5_invoke_act},
    {6 , class6_invoke_get,  class6_invoke_set, class6_invoke_act},
    {7 , class7_invoke_get,  class7_invoke_set, class7_invoke_act},
    {8 , class8_invoke_get,  class8_invoke_set, class8_invoke_act},
    {9 , class9_invoke_get,  class9_invoke_set, class9_invoke_act},
    {10, class10_invoke_get,class10_invoke_set,class10_invoke_act},
    {11, class11_invoke_get,class11_invoke_set,class11_invoke_act},
    {12, class12_invoke_get,class12_invoke_set,class12_invoke_act},
    {13, class13_invoke_get,class13_invoke_set,class13_invoke_act},
    {14, class14_invoke_get,class14_invoke_set,class14_invoke_act},
    {15, class15_invoke_get,class15_invoke_set,class15_invoke_act},
    {16, class16_invoke_get,class16_invoke_set,class16_invoke_act},
    {17, class17_invoke_get,class17_invoke_set,class17_invoke_act},
    {18, class18_invoke_get,class18_invoke_set,class18_invoke_act},
    {19, class19_invoke_get,class19_invoke_set,class19_invoke_act},
    {20, class20_invoke_get,class20_invoke_set,class20_invoke_act},
    {21, class21_invoke_get,class21_invoke_set,class21_invoke_act},
    {22, class22_invoke_get,class22_invoke_set,class22_invoke_act},
    {23, class23_invoke_get,class23_invoke_set,class23_invoke_act},
    {24, class24_invoke_get,class24_invoke_set,class24_invoke_act},
    {25, class25_invoke_get,class25_invoke_set,class25_invoke_act},
    {26, class26_invoke_get,class26_invoke_set,class26_invoke_act},
};

#endif

#if DESC("文件中公共函数",1)

/**
*********************************************************************
* @name：       app_apdubreak_init
* @brief：      断点信息初始化
* @param[in] ：
* @param[out]：断点句柄
* @return    ：返回消息长度
* @author    : 
* @Date      ：2019-12-17
*********************************************************************
*/
void *app_apdubreak_init()
{
    BP_INFO_T *pApduBreak = NULL;
    
    pApduBreak = (BP_INFO_T *) malloc(sizeof(BP_INFO_T));
    
    if(NULL != pApduBreak)
    {
        memset(pApduBreak, 0, sizeof(BP_INFO_T));     
    }
    
    return pApduBreak;
}

/**
*********************************************************************
* @name：       app_apdubreak_init
* @brief：      断点句柄释放
* @param[in] ：void *pBreak   断点句柄
* @param[out]：
* @return    ：返回消息长度
* @author    : 
* @Date      ：2019-12-17
*********************************************************************
*/
void app_apdubreak_free(void **pBreak)
{
    FREE_S(*pBreak);           
}

/**
*********************************************************************
* @name：       app_apdubreak_init
* @brief：      断点句柄释放
* @param[in] ：void *pBreak   断点句柄
* @param[out]：
* @return    ：返回消息长度
* @author    : 
* @Date      ：2019-12-17
*********************************************************************
*/
void app_apdubreak_zero(void *pBreak)
{
    if(pBreak)
    {
        memset(pBreak, 0, sizeof(BP_INFO_T));
    }
}

/**
*********************************************************************
* @name：       service_msg_construct
* @brief：      SET服务入口,这里处理是从oad开始
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               uint16 msglen                消息长度
               headLen                      APDU头长度
* @param[out]：uint8 *pOutMsg                返回消息
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 service_msg_construct(CHANNEL_CACHE_T *ptChannel, const uint8 *pInMsg, uint16 headLen, uint8 *pOutMsg, uint16 msglen)
{
    uint16 i = headLen;
    BP_INFO_T breakInfo = {0};
    uint16    msglenTmp = 0;
    uint32    offset    = 0;

    memcpy(&breakInfo, ptChannel->pApduBreak, sizeof(BP_INFO_T));
    msglenTmp = msglen;

    pOutMsg[0] = pInMsg[0]|0x80;
    msglen++;  
    
    while(--i)
    {
        pOutMsg[i] = pInMsg[i];
        msglen++;
    }

    /*应用分帧报文处理*/
    if ((breakInfo.appSlicingFlag == 0x01) && (pInMsg[0] == 0x05))
    {
        /*get next*/
        pOutMsg[1] = 0x05;

        /*预留出分帧信息位置*/
        msglen += 5;   

        if (pInMsg[1] == 0x02)
        {
            memmove(&pOutMsg[OOP_DATA_OFFSET_NEXT], &pOutMsg[OOP_DATA_LIST_OFFSET], msglenTmp);
            msglen -= 1;
        }
        else
        {
            memmove(&pOutMsg[OOP_DATA_OFFSET_NEXT], &pOutMsg[OOP_DATA_OFFSET], msglenTmp);
        }
        
        offset += OOP_DATA_OFFSET;

        /*末帧标识*/
        pOutMsg[offset++] = breakInfo.isEnd;

        /*分帧序号*/
        memcpy_r(&pOutMsg[offset], &breakInfo.seq, sizeof(uint16));
        offset += sizeof(uint16);

        /*NORMARL或RECORD类型对象*/
        if((breakInfo.reqType == REQ_GET_NORMAL) || (breakInfo.reqType == REQ_GET_NORMAL_LIST) || (breakInfo.objPre == 0x01))
        {
            pOutMsg[offset++] = 1;
        }
        else if((breakInfo.reqType == REQ_GET_RECORD) || (breakInfo.reqType == REQ_GET_RECORD_LIST) || (breakInfo.objPre == 0x02))
        {
            pOutMsg[offset++] = 2;
        }
         
        if ((breakInfo.reqType == REQ_GET_NORMAL_LIST) && (breakInfo.oadNum > 0))
        {
            pOutMsg[offset++] = breakInfo.oadNum;
        }
        else
        {
            /*sequence of 暂时先填1*/
            pOutMsg[offset++] = 0x01;
        }
    }

    /*如果是末帧，清空断点信息*/
    if ((breakInfo.appSlicingFlag == 0x01) && (breakInfo.isEnd == 0x01))
    {
        memset(ptChannel->pApduBreak, 0, sizeof(BP_INFO_T));
    }
    
    //TODO 跟随上报信息域
    pOutMsg[msglen ++] = 0;

    //时间标签域
    pOutMsg[msglen++] = 0;
    
#if 0    
    pOutMsg[msglen++] = ptChannel->tContext.timeTag.tagFlag;
    
    if(ptChannel->tContext.timeTag.tagFlag > 0)
    {
        memcpy(&pOutMsg[msglen], ptChannel->tContext.timeTag.tagBuff, 10);
        msglen += 10;
    }
#endif

    return msglen;
}

#endif

#if DESC("读取服务处理函数",1)
/**
*********************************************************************
* @name：       set_err_segement_framing
* @brief：      SET服务的错误帧
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               uint16 msglen                消息长度
               headLen                      APDU头长度
* @param[out]：uint8 *pOutMsg                返回消息
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 get_err_seg_framing(OAD_INFO_T *pOadInfo, APDU_INFO_T *apduInfo, uint8* pInMsg, uint16 inLen, OOP_DAR_E dar, uint8 *pOutMsg, uint16 msgmaxlen)
{
    uint16 offset = 4;
    uint32 innerOffset = 4, csdStart = 0;

    //OAD
    memcpy(pOutMsg, pInMsg, sizeof(uint32));
    
    if(pOadInfo->isRec == TRUE)
    {
        //记录型要组RCSD
        if(apdu_next_position_get(pInMsg, &innerOffset, inLen, DT_RSD, TRUE))
        {
            csdStart = innerOffset;
            if(apdu_next_position_get(pInMsg, &innerOffset, inLen, DT_RCSD, TRUE))
            {
                memcpy(pOutMsg+offset, pInMsg+csdStart, innerOffset-csdStart);
                offset += (innerOffset-csdStart);
            }

            //RCSD = 0
            if(innerOffset == (csdStart + 1))
            {
                offset--;
                all_rcsd_get(pOadInfo, apduInfo, &offset, pOutMsg, msgmaxlen);
            }
        }
    }
    
    pOutMsg[offset++] = 0; //[0] DAR

    //DAR
    pOutMsg[offset++] = dar;

    return offset;
}

/**
*********************************************************************
* @name：       invoke_get_req_other
* @brief：      SET服务入口,这里处理是从oad开始
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 invoke_get_req_other(CHANNEL_CACHE_T *ptChannel, const uint8 *pInMsg, uint16 inLen, uint8 *pOutMsg, uint16 msgmaxlen)
{
    uint16 msglen = 0;
    OOP_DAR_E oopDar = DATA_OTHER_REASON;
    OOP_OAD_U serviceOAD = {0}; 
    OAD_INFO_T pstOdaInfo = {0};    
    APDU_INFO_T apduInfo = {0};
    RESULT_DATA_T outData;
    #if (!defined AREA_SHANDONG)
    SecModWd_t secModWd = {0};
    #endif
    int i = 0;

    memcpy_r(&serviceOAD, &pInMsg[0], sizeof(OOP_OAD_U));
    PRTL_FMT_TRACE("invoke_get_req:serviceOAD(%08x)\n", serviceOAD.value);
    PRTL_BUF_DEBUG(pInMsg, inLen, "输入的APDU:");

    apduInfo.hUdp = ptChannel->clientid;
    apduInfo.logicAddr = ptChannel->private[0];
    apduInfo.reqType = ptChannel->private[1]; //到这里不会是next类型
    apduInfo.NoEsamMac = ptChannel->private[4]; //将通道里标记交采模组特殊标记转入apduInfo
    apduInfo.bpBreakInfo = (BP_INFO_T *)ptChannel->pApduBreak;
    apduInfo.bpBreakInfo->reqType = ptChannel->private[1]; //到这里不会是next类型
    apduInfo.connectionInfo = ptChannel->tContext;
    #ifdef AREA_SHANDONG
    apduInfo.runTi.value = ptChannel->private[6];
    apduInfo.runTi.unit = ptChannel->private[7];
    apduInfo.isreport = ptChannel->private[8];
    #endif 
    outData.pResultData = pOutMsg;
    outData.resultDataLen = 0;

    pstOdaInfo.noParse = ptChannel->private[3];

    if((apduInfo.reqType == REQ_GET_RECORD) || (apduInfo.reqType == REQ_GET_RECORD_LIST))
    {
        pstOdaInfo.isRec = TRUE;
    }
    
    if (class_oadinfo_get(serviceOAD, &pstOdaInfo))
    {
        //校验时戳
        if((ptChannel->tContext.timeTag.tagFlag > 0) && (ptChannel->tContext.timeTag.isTimeOut == TRUE))
        {
            return get_err_seg_framing(&pstOdaInfo, &apduInfo, (uint8 *)pInMsg, inLen, DATA_TIMEFLAG_INVALID, pOutMsg, msgmaxlen);        
        }
        /* 适配打高分批量读数据 不校验安全模式 */
        
            //安全模式，判断访问权限
            #if (!defined AREA_SHANDONG)
            secModWd = security_mod_get(ptChannel, serviceOAD.nObjID);
            PRTL_FMT_TRACE("请求的安全模式=%02X,默认安全模式=%02X\n", ptChannel->tContext.curSecurityMode, secModWd.get);
            if(ptChannel->tContext.curSecurityMode > secModWd.get)
            {
                PRTL_FMT_DEBUG("Object-%04X 安全模式级别不够!\n",serviceOAD.nObjID);
                oopDar = DATA_READ_WRITE_DENIED;
                return get_err_seg_framing(&pstOdaInfo, &apduInfo, (uint8 *)pInMsg, inLen, oopDar, pOutMsg, msgmaxlen);
            }
            #endif
        while (i < OOP_IC_MAX)
        {
            if (g_prtl_ic_op[i].ClassID == pstOdaInfo.pTab->class)
            {
                PRTL_FMT_LOGCC("invoke_get_req:pstOdaInfo->class = %d\n", pstOdaInfo.pTab->class);

                //TODO 调用前还要判断权限,还有判时戳,OAD就不在报文中传了
                oopDar = g_prtl_ic_op[i].InvokGet(&pstOdaInfo, &apduInfo, &pInMsg[0], inLen, &outData, msgmaxlen);
                if (oopDar == DATA_SUCCESS)
                {
                    msglen = outData.resultDataLen;
                    ptChannel->pApduBreak = apduInfo.bpBreakInfo;
                    ptChannel->private[9] = apduInfo.bpBreakInfo->appSlicingFlag; //回传本次是否需要应用分帧
                    PRTL_BUF_DEBUG(outData.pResultData, msglen, "返回的APDU:");

                    /*回填断点信息中的OAD*/
                    apduInfo.bpBreakInfo->serviceOAD = serviceOAD;
                }
                else
                {
                    PRTL_BUF_DEBUG(pInMsg, inLen, "InvokGet: fault response. dar(%d), class(%d)\n", oopDar, pstOdaInfo.pTab->class);
                }
                break;
            }
            
            i++;
        }

        //注意构造错误消息帧
        if(i == OOP_IC_MAX)
        {
            oopDar = DATA_READ_WRITE_DENIED;
            PRTL_BUF_DEBUG(pInMsg, inLen, "InvokGet: not find parse func. dar(%d), class(%d)\n", oopDar, pstOdaInfo.pTab->class);
        }
    }
    else
    {
        oopDar = DATA_OBJECT_UNDEFINED;
        PRTL_BUF_DEBUG(pInMsg, inLen, "InvokGet:not find oda table. dar(%d), oda(0x%08x)\n", oopDar, serviceOAD.value);
    }

    //apdu数据内容空，且为上报通道，直接返回0
    if ((ptChannel->private[5] == 1) && ((apduInfo.isDateNull == 1) || (oopDar != DATA_SUCCESS)))
    {
        PRTL_FMT_DEBUG("InvokGet:data null. dar(%d), oad(0x%08x)\n", oopDar, serviceOAD.value);
        return 0;
    }

    //返回数据的，不组织报文//TODO,有空优化成返回错误码
    if(pstOdaInfo.noParse) 
    {
        return msglen;
    }

    //错误信息帧处理,如果内部已经组织错误帧，返回的oopDar应该为0
    if(oopDar != DATA_SUCCESS)
    {
        return get_err_seg_framing(&pstOdaInfo, &apduInfo, (uint8 *)pInMsg, inLen, oopDar, pOutMsg, msgmaxlen);
    }

    return msglen;
}

/**
*********************************************************************
* @name：       invoke_get_req_att1
* @brief：      SET服务入口,这里处理是从oad开始
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 invoke_get_req_att1(CHANNEL_CACHE_T *ptChannel, const uint8 *pInMsg, uint16 inLen, uint8 *pOutMsg, uint16 msgmaxlen)
{
    uint16 msglen = 0;
    uint16 offset = 0;
    OOP_DAR_E oopDar = DATA_SUCCESS;
    OOP_OAD_U serviceOAD = {0}; 
    OAD_INFO_T pstOdaInfo = {0};    
    APDU_INFO_T apduInfo = {0};
    #if (!defined AREA_SHANDONG)
    SecModWd_t secModWd = {0};
    #endif

    memcpy_r(&serviceOAD, &pInMsg[0], sizeof(OOP_OAD_U));
    PRTL_FMT_TRACE("invoke_get_req:serviceOAD(%08x)\n", serviceOAD.value);
    PRTL_BUF_DEBUG(pInMsg, inLen, "输入的APDU:");

    apduInfo.hUdp = ptChannel->clientid;
    apduInfo.logicAddr = ptChannel->private[0];
    apduInfo.reqType = ptChannel->private[1]; //到这里不会是next类型
    apduInfo.NoEsamMac = ptChannel->private[4]; //将通道里标记交采模组特殊标记转入apduInfo
    apduInfo.bpBreakInfo = (BP_INFO_T *)ptChannel->pApduBreak;
    apduInfo.bpBreakInfo->reqType = ptChannel->private[1]; //到这里不会是next类型
    apduInfo.connectionInfo = ptChannel->tContext;

    pstOdaInfo.noParse = ptChannel->private[3];

    if((apduInfo.reqType == REQ_GET_RECORD) || (apduInfo.reqType == REQ_GET_RECORD_LIST))
    {
        pstOdaInfo.isRec = TRUE;
    }
    
    if (serviceOAD.attID == 1)
    {
        //校验时戳
        if((ptChannel->tContext.timeTag.tagFlag > 0) && (ptChannel->tContext.timeTag.isTimeOut == TRUE))
        {
            return get_err_seg_framing(&pstOdaInfo, &apduInfo, (uint8 *)pInMsg, inLen, DATA_TIMEFLAG_INVALID, pOutMsg, msgmaxlen);        
        }
        
        //安全模式，判断访问权限
        #if (!defined AREA_SHANDONG)
        secModWd = security_mod_get(ptChannel, serviceOAD.nObjID);
        PRTL_FMT_TRACE("请求的安全模式=%02X,默认安全模式=%02X\n", ptChannel->tContext.curSecurityMode, secModWd.get);
        if(ptChannel->tContext.curSecurityMode > secModWd.get)
        {
            PRTL_FMT_DEBUG("Object-%04X 安全模式级别不够!\n",serviceOAD.nObjID);
            oopDar = DATA_READ_WRITE_DENIED;
            return get_err_seg_framing(&pstOdaInfo, &apduInfo, (uint8 *)pInMsg, inLen, oopDar, pOutMsg, msgmaxlen);
        }
        #endif
        /* 组织属性1报文*/
        memcpy(pOutMsg, pInMsg, sizeof(OOP_OAD_U));
        offset += sizeof(OOP_OAD_U);
        pOutMsg[offset ++] = 0x01;
        pOutMsg[offset ++] = 0x09;
        pOutMsg[offset ++] = 0x02;
        pOutMsg[offset ++] = (uint8)(serviceOAD.nObjID >> 8);
        pOutMsg[offset ++] = (uint8)(serviceOAD.nObjID);
        msglen += offset;
    }
    else
    {
        oopDar = DATA_OBJECT_UNDEFINED;
        PRTL_BUF_DEBUG(pInMsg, inLen, "InvokGet:not find oda table. dar(%d), oda(0x%08x)\n", oopDar, serviceOAD.value);
    }

    //错误信息帧处理,如果内部已经组织错误帧，返回的oopDar应该为0
    if(oopDar != DATA_SUCCESS)
    {
        return get_err_seg_framing(&pstOdaInfo, &apduInfo, (uint8 *)pInMsg, inLen, oopDar, pOutMsg, msgmaxlen);
    }

    return msglen;
}

/**
*********************************************************************
* @name：       invoke_get_req
* @brief：      SET服务入口,这里处理是从oad开始
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 invoke_get_req(CHANNEL_CACHE_T *ptChannel, const uint8 *pInMsg, uint16 inLen, uint8 *pOutMsg, uint16 msgmaxlen)
{
    OOP_OAD_U serviceOAD = {0}; 

    memcpy_r(&serviceOAD, &pInMsg[0], sizeof(OOP_OAD_U));

    //针对属性1做处理
    if (serviceOAD.attID == 1)
    {
        return invoke_get_req_att1(ptChannel, pInMsg, inLen, pOutMsg, msgmaxlen);
    }
    else
    {
        return invoke_get_req_other(ptChannel, pInMsg, inLen, pOutMsg, msgmaxlen);
    }
}

/**
*********************************************************************
* @name：       get_normal_req
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 get_normal_req(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    /*GetRequestNormal∷=SEQUENCE
    {
         服务序号-优先级           PIID，
         一个对象属性描述符 OAD，
    }*/

    uint16 msglen = 0;
    BP_INFO_T *bpInfo = ptChannel->pApduBreak;
    uint16 offset=0;
    uint16 tmpoffset=0;
    uint16 i=0;
    OOP_OAD_U OAD = {0};
    Proxyresquest proxyplc;
    uint8 buf[200]={0};
    uint8 cs=0;
    BP_INFO_T breakInfo = {0};
//    uint8 EvtBuffer[5];
//    OOP_DAR_E dar = DATA_SUCCESS;

    //传入的报文是PIID之后的部分，传出的报文是PIID-ACD以后的部分
    memcpy_r(&OAD, ptFrame->pApdu+3, sizeof(OOP_OAD_U));
    if((OAD.value==0xF2090900) || (OAD.value==0xF2096200))
    {
        memset((uint8*)&proxyplc,0x00,sizeof(Proxyresquest));
        memset(buf,0x00,sizeof(buf));
        proxyplc.PIID = ptFrame->pApdu[2];
        proxyplc.ServerID = SERVERID_GETREQUEST;
        proxyplc.nChioce = 0x08;//du
        if(ptChannel->tContext.securityServer==TRUE)
        proxyplc.SecMod = ptChannel->tContext.curSecurityMode;
        else
        proxyplc.SecMod = SecMod_NULL;
        proxyplc.FrameHeadInfo.oad=OAD;
        memcpy(buf+offset,ptFrame->pApdu,3);
        offset = offset+3;
        buf[offset++]= 0xF2;
        buf[offset++]= 0x09;
        buf[offset++]= 0x02;
        buf[offset++]= 0x01;
        buf[offset++]= 0x06;
        buf[offset++]= 0x02;
        buf[offset++]= 0x08;
        buf[offset++]= 0x01;
        buf[offset++]= 0x00;
        buf[offset++]= 0x00;
        buf[offset++]= 0x64;
        buf[offset++]= 0x00;
        buf[offset++]= 0x64;
        buf[offset++]= 0x0F;
        buf[offset++]= 0x68;
        buf[offset++]= 0x0F;
        buf[offset++]= 0x00;
        tmpoffset = offset;
        buf[offset++]= 0x43;
        buf[offset++]= 0x00;
        buf[offset++]= 0x00;
        buf[offset++]= 0x00;
        buf[offset++]= 0x00;
        buf[offset++]= 0x00;
        buf[offset++]= 0x0E;
        buf[offset++]= 0x03;
        if(OAD.value==0xF2090900)
        {
        buf[offset++]= 0x80;
        buf[offset++]= 0x01;
        }
        else if(OAD.value==0xF2096200)
        {
        buf[offset++]= 0x01;
        buf[offset++]= 0x19;
        }
        for(i=tmpoffset;i<offset;i++)
        {
            cs+=buf[i];
        }
        buf[offset++]= cs;
        buf[offset++]= 0x16;
     
        
        proxyplc.RequestApduLen=offset;
        memcpy(proxyplc.pRequestApdu,buf,proxyplc.RequestApduLen);
        PRTL_BUF_LOGCC(proxyplc.pRequestApdu,proxyplc.RequestApduLen,"转换后的透传设置频点报文");
        proxyplc.FrameHeadInfo.pipe = ptChannel->id;
        proxyplc.FrameHeadInfo.ctrl.ctrl = ptFrame->dir|ptFrame->prm|ptFrame->separate|ptFrame->sc|ptFrame->funCode;
        PRTL_FMT_LOGCC("proxyplc.FrameHeadInfo.ctrl.ctrl is %d\n",proxyplc.FrameHeadInfo.ctrl.ctrl);
        proxyplc.FrameHeadInfo.tsa.af=ptFrame->sAddr.len;
        memcpy(proxyplc.FrameHeadInfo.tsa.add,ptFrame->sAddr.addr,proxyplc.FrameHeadInfo.tsa.af);
        PRTL_FMT_LOGCC("proxyplc.FrameHeadInfo.tsa.af is %d\n",proxyplc.FrameHeadInfo.tsa.af);
        ProxyWriteQueue(&ProxyQueue,(uint8*)&proxyplc,sizeof(Proxyresquest));
        return 0;
    }

    memcpy(&breakInfo, bpInfo, sizeof(BP_INFO_T));
    memset(bpInfo, 0, sizeof(BP_INFO_T));
    ptChannel->private[9] = 0;

    //传入的报文是PIID之后的部分，传出的报文是PIID-ACD以后的部分
    msglen = invoke_get_req(ptChannel, &ptFrame->pApdu[OOP_DATA_OFFSET], ptFrame->apduLen-OOP_DATA_OFFSET, &pOutMsg[OOP_DATA_OFFSET], msgmaxlen-OOP_DATA_OFFSET);

    if(msglen != 0)
    {        
        //加上PIID-ACD之前的部分，和两个OPTIONAL 时戳与跟随上报信息
        msglen = service_msg_construct(ptChannel, ptFrame->pApdu, OOP_DATA_OFFSET, pOutMsg, msglen);
    }

    /*带出应用分帧标识给调度层*/
    *pBreakFlag = ptChannel->private[9];

    if(*pBreakFlag != TRUE) //没有应用分帧，则仍保留上次断点
    {
        memcpy(bpInfo, &breakInfo, sizeof(BP_INFO_T));
    }

    return msglen;
}


/**
*********************************************************************
* @name：       get_normal_list_req
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 get_normal_list_req(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    /*SetRequestNormalList∷=SEQUENCE
    {
         服务序号-优先级           PIID，
         若干个对象属性            SEQUENCE OF
         {
            一个对象属性描述符 OAD，
         }
    }*/

    uint16 msglen = 0;
    uint8 sequenceNum;
    uint16  dataLen = 0;
    uint16  offset = 0;
    uint8 *outTmp = &pOutMsg[OOP_DATA_LIST_OFFSET];   //留出空间写OAD前面部分（ServiceID,responseID,PIID-ID, SEQUENCE OF）
    uint16 msgmaxlenTmp = msgmaxlen-OOP_DATA_LIST_OFFSET; //留出空间写OAD前面部分
    uint16 tmpMsgLen = 0;
    uint32 maxBufLen = ptChannel->tContext.ApplSplitLen - cal_excludeLen(ptChannel->tContext);
    uint32 oadNum = 0;

    BOOLEAN   isBufFull = FALSE;
	BP_INFO_T breakInfo = {0};
	BP_INFO_T *bpInfo = ptChannel->pApduBreak;
    memcpy(&breakInfo, bpInfo, sizeof(BP_INFO_T));
    memset(bpInfo, 0, sizeof(BP_INFO_T));
    ptChannel->private[9] = 0;

    sequenceNum = ptFrame->pApdu[3]; //序列个数
    OOP_OAD_U oad={0};
    uint8 allnum = sequenceNum;
    PRTL_FMT_DEBUG("allnum %d \n",allnum);
    while(sequenceNum--)
    {
        dataLen = 4; //偏掉OAD

        //适配跑分做特殊处理 先从缓存中看下有没有数据 如果有就直接用 没有则继续从规约获取
        //新软件不再用逻辑地址1来读了 直接判个数
        //if(1 == ptFrame->sAddr.logicAddr)
        if(allnum > 40)
        {
            memcpy_r(&oad.value,&ptFrame->pApdu[OOP_DATA_LIST_OFFSET + offset],4);
            tmpMsgLen = apdu_cache_get(oad,outTmp);
            if(tmpMsgLen == 0)
            {
                PRTL_FMT_DEBUG("apdu_cache_get 0\n");
                tmpMsgLen = invoke_get_req(ptChannel, &ptFrame->pApdu[OOP_DATA_LIST_OFFSET + offset], dataLen, outTmp, msgmaxlenTmp);
                //再讲从规约读到的数据写入缓存
                apdu_cache_add(oad,outTmp,tmpMsgLen);
            }
        }else
        {
            //传入的报文是PIID之后的部分，传出的报文是PIID-ACD以后的部分
            tmpMsgLen = invoke_get_req(ptChannel, &ptFrame->pApdu[OOP_DATA_LIST_OFFSET + offset], dataLen, outTmp, msgmaxlenTmp);
        }

        outTmp += tmpMsgLen;
        msgmaxlenTmp -= tmpMsgLen;
        oadNum ++;

        if ((msglen + tmpMsgLen > maxBufLen) && (ptChannel->private[9] != 1))
        {
            isBufFull = TRUE;
            bpInfo->oadNo = sequenceNum;
            bpInfo->oadNum = oadNum - 1;
            COMM_FMT_DEBUG("读多个OAD产生断点，断点序号[%d] OAD个数[%d]\n", bpInfo->oadNo, bpInfo->oadNum);
            break;
        }
        else if ((msglen + tmpMsgLen > maxBufLen) && (ptChannel->private[9] == 1))
        {
            PRTL_FMT_DEBUG("异常报文，双重应用分帧\n");
        }

        offset += dataLen; //APDU的内偏移

        msglen += tmpMsgLen;
    }

    if (isBufFull)
    {
        bpInfo->seq = BRK_SEQ_START;                       //首次序号
        bpInfo->reqType = REQ_GET_NORMAL_LIST;
        bpInfo->appSlicingFlag = 1;
        bpInfo->isEnd = 0x00;                              //不是末尾帧

        ptChannel->private[9] = 1;
    }

    if(msglen != 0)
    {
        //加上PIID-ACD之前的部分，和两个OPTIONAL 时戳与跟随上报信息
        msglen = service_msg_construct(ptChannel, ptFrame->pApdu, OOP_DATA_LIST_OFFSET, pOutMsg, msglen);
    }

	/*带出应用分帧标识给调度层*/
    *pBreakFlag = ptChannel->private[9];

    if(*pBreakFlag != TRUE) //没有应用分帧，则仍保留上次断点
    {
        memcpy(bpInfo, &breakInfo, sizeof(BP_INFO_T));
    }

    return msglen;
}

/**
*********************************************************************
* @name：       get_normal_list_req
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 get_normal_list_next(CHANNEL_CACHE_T* ptChannel, FRAME_HEAD_T* ptFrame, uint8* pOutMsg, uint16 msgmaxlen,
    uint8* pBreakFlag)
{
    /*SetRequestNormalList∷=SEQUENCE
    {
        服务序号-优先级           PIID，
        若干个对象属性            SEQUENCE OF
        {
            一个对象属性描述符 OAD，
        }
    }*/

    uint16 msglen = 0;
    uint8 sequenceNum;
    uint16  dataLen = 0;
    uint16  offset = 0;
    uint8* outTmp = &pOutMsg[OOP_DATA_LIST_OFFSET];   //留出空间写OAD前面部分（ServiceID,responseID,PIID-ID, SEQUENCE OF）
    uint16 msgmaxlenTmp = msgmaxlen - OOP_DATA_LIST_OFFSET; //留出空间写OAD前面部分
    uint16 tmpMsgLen = 0;
    uint32 maxBufLen = ptChannel->tContext.ApplSplitLen - cal_excludeLen(ptChannel->tContext);
    uint32 oadNum = 0;

    BOOLEAN   isBufFull = FALSE;
    BP_INFO_T* bpInfo = ptChannel->pApduBreak;
    ptChannel->private[9] = 0;

    sequenceNum = ptFrame->pApdu[3]; //序列个数

    if (bpInfo->appSlicingFlag != 0x01)
    {
        PRTL_FMT_DEBUG("处理出错，分帧标志[%d] 是否末尾[%d] 分帧序号[%d]\n", bpInfo->appSlicingFlag, bpInfo->isEnd, bpInfo->seq);
        return 0;
    }

    while (sequenceNum--)
    {
        dataLen = 4; //偏掉OAD

        //直接到断点处
        if (sequenceNum > bpInfo->oadNo)
        {
            offset += dataLen; //APDU的内偏移
            continue;
        }

        //传入的报文是PIID之后的部分，传出的报文是PIID-ACD以后的部分
        tmpMsgLen = invoke_get_req(ptChannel, &ptFrame->pApdu[OOP_DATA_LIST_OFFSET + offset], dataLen, outTmp, msgmaxlenTmp);
        outTmp += tmpMsgLen;
        msgmaxlenTmp -= tmpMsgLen;
        oadNum ++;

        //暂时不考虑单个OAD也有应用分帧的情况
        if (msglen + tmpMsgLen > maxBufLen)
        {
            isBufFull = TRUE;
            bpInfo->oadNo = sequenceNum;
            bpInfo->oadNum = oadNum - 1;
            COMM_FMT_DEBUG("读多个OAD产生断点，断点序号[%d] OAD个数[%d]\n", bpInfo->oadNo, bpInfo->oadNum);
            break;
        }

        offset += dataLen; //APDU的内偏移

        msglen += tmpMsgLen;
    }

    if (!isBufFull)
    {
        bpInfo->appSlicingFlag = 0x01;
        bpInfo->isEnd = 0x01;                          //末尾帧
        bpInfo->seq++;                                 //序号加1
        bpInfo->reqType = REQ_GET_NORMAL_LIST;         //普通数据

        //所有OAD召测完成
        bpInfo->oadNum = oadNum;
    }
    else
    {
        bpInfo->seq++;                                 //序号加1
        bpInfo->reqType = REQ_GET_NORMAL_LIST;         //普通数据
        bpInfo->appSlicingFlag = 1;
        bpInfo->isEnd = 0x00;                          //不是末尾帧

        ptChannel->private[9] = 1;
    }

    if (msglen != 0)
    {
        //加上PIID-ACD之前的部分，和两个OPTIONAL 时戳与跟随上报信息
        msglen = service_msg_construct(ptChannel, ptFrame->pApdu, OOP_DATA_LIST_OFFSET, pOutMsg, msglen);
    }

    /*带出应用分帧标识给调度层*/
    *pBreakFlag = ptChannel->private[9];

    return msglen;
}

/**
*********************************************************************
* @name：       get_record_req
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 get_record_req(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    BP_INFO_T breakInfo = {0};

    /*GetRequestRecord∷=SEQUENCE
    {
         服务序号-优先级           PIID，
         读取一个记录型对象属性 GetRecord，
         
    }*/

    uint16 msglen = 0;
    BP_INFO_T *bpInfo = ptChannel->pApduBreak;
    memcpy(&breakInfo, bpInfo, sizeof(BP_INFO_T));
    memset(bpInfo, 0, sizeof(BP_INFO_T));
    ptChannel->private[9] = 0;

    //传入的报文是PIID之后的部分，传出的报文是PIID-ACD以后的部分
    msglen = invoke_get_req(ptChannel, &ptFrame->pApdu[OOP_DATA_OFFSET], ptFrame->apduLen-OOP_DATA_OFFSET, &pOutMsg[OOP_DATA_OFFSET], msgmaxlen-OOP_DATA_OFFSET);

    if(msglen != 0)
    {
        PRTL_FMT_TRACE("get_record_req:msglen(%d)\n",msglen);
        
        //加上PIID-ACD之前的部分，和两个OPTIONAL 时戳与跟随上报信息
        msglen = service_msg_construct(ptChannel, ptFrame->pApdu, OOP_DATA_OFFSET, pOutMsg, msglen);
    }

    /*带出应用分帧标识给调度层*/
    *pBreakFlag = ptChannel->private[9];

    if(*pBreakFlag != TRUE) //没有应用分帧，则仍保留上次断点
    {
        memcpy(bpInfo, &breakInfo, sizeof(BP_INFO_T));
    }

    return msglen;
}

/**
*********************************************************************
* @name：       get_record_list_req
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 get_record_list_req(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    /*GetRequestRecordList∷=SEQUENCE
    {
         服务序号-优先级              PIID，
         若干个记录型对象属性            SEQUENCE OF GetRecord，
    }*/

    uint16 msglen = 0;
    uint8 sequenceNum;
    uint32  dataLen = 0;
    uint16  offset = 0;
    uint8 *outTmp = &pOutMsg[OOP_DATA_LIST_OFFSET];   //留出空间写OAD前面部分（ServiceID,responseID,PIID-ID, SEQUENCE OF）
    uint16 msgmaxlenTmp = msgmaxlen-OOP_DATA_LIST_OFFSET; //留出空间写OAD前面部分
    uint16 tmpMsgLen = 0;

    sequenceNum = ptFrame->pApdu[3]; //序列个数
        
    while(sequenceNum--)
    {
        dataLen = 0;
        
        if (!apdu_next_position_get(&ptFrame->pApdu[OOP_DATA_LIST_OFFSET+offset], &dataLen, ptFrame->apduLen, DT_OAD, TRUE))
        {
            return msglen;
        }
        
        if (!apdu_next_position_get(&ptFrame->pApdu[OOP_DATA_LIST_OFFSET+offset], &dataLen, ptFrame->apduLen, DT_RSD, TRUE))
        {
            return msglen;
        }

        if (!apdu_next_position_get(&ptFrame->pApdu[OOP_DATA_LIST_OFFSET+offset], &dataLen, ptFrame->apduLen, DT_RCSD, TRUE))
        {
            return msglen;
        }
        
        //传入的报文是PIID之后的部分，传出的报文是PIID-ACD以后的部分
        tmpMsgLen = invoke_get_req(ptChannel, &ptFrame->pApdu[OOP_DATA_LIST_OFFSET + offset], dataLen, outTmp, msgmaxlenTmp);
        outTmp += tmpMsgLen;
        msgmaxlenTmp -= tmpMsgLen;

        offset += dataLen; //APDU的内偏移

        msglen += tmpMsgLen;
    }

    if(msglen != 0)
    {
        //加上PIID-ACD之前的部分，和两个OPTIONAL 时戳与跟随上报信息
        msglen = service_msg_construct(ptChannel, ptFrame->pApdu, OOP_DATA_LIST_OFFSET, pOutMsg, msglen);
    }

    return msglen;
}


/**
*********************************************************************
* @name：       get_next_req
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 get_next_req(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    /*GetRequestNormal∷=SEQUENCE
    {
         服务序号-优先级           PIID，
         一个对象属性描述符 OAD，
    }*/

    uint16 msglen = 0;
    FRAME_HEAD_T tSrcFrame = {0};   //用于获取原始报文
    int errcode = 0;
    uint16 seq = 0;
    BP_INFO_T *bpInfo = ptChannel->pApduBreak;

    /*获取应用分帧序号，对比确认*/
    seq = (uint16)((ptFrame->pApdu[OOP_DATA_OFFSET]<<8) + ptFrame->pApdu[OOP_DATA_OFFSET + 1]);
    if (bpInfo->seq != seq)
    {
        PRTL_FMT_DEBUG("获取应用分帧序号 不一致 need(%d) != real(%d)\n", bpInfo->seq, seq);
        return 0;
    }

    /* 解析帧头 */
    memset(&tSrcFrame, 0, sizeof(FRAME_HEAD_T));
    errcode = app_get_frame(ptChannel->pBreakBakMsg, ptChannel->BreakMsgLen, &tSrcFrame, TRUE);
    if (errcode != APP698_SECCESS)
    {
        PRTL_FMT_DEBUG("原始报文解析错误, errcode(%d)\n", errcode);
        return 0;
    }

    //断点中保存的是上次的请求类型，分帧读下一帧仍然要保留上次的请求类型
    ptChannel->private[1] = bpInfo->reqType;

    if ((bpInfo->appSlicingFlag == 1) && (bpInfo->reqType == REQ_GET_NORMAL_LIST) && (bpInfo->oadNum > 0))
    {
        return get_normal_list_next(ptChannel, &tSrcFrame, pOutMsg, msgmaxlen, pBreakFlag);
    }

    //传入的报文是PIID之后的部分，传出的报文是PIID-ACD以后的部分
    msglen = invoke_get_req(ptChannel, &tSrcFrame.pApdu[OOP_DATA_OFFSET], tSrcFrame.apduLen-OOP_DATA_OFFSET, &pOutMsg[OOP_DATA_OFFSET], msgmaxlen-OOP_DATA_OFFSET);

    if(msglen != 0)
    {
        PRTL_FMT_TRACE("get_normal_res:msglen(%d)\n",msglen);
        
        //加上PIID-ACD之前的部分，和两个OPTIONAL 时戳与跟随上报信息
        msglen = service_msg_construct(ptChannel, ptFrame->pApdu, OOP_DATA_OFFSET, pOutMsg, msglen);
    }

    /*带出应用分帧标识给调度层*/
    *pBreakFlag = bpInfo->appSlicingFlag;

    return msglen;
}


/**
*********************************************************************
* @name：       get_md5_req
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 get_md5_req(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    uint16 msglen = 0;
    uint8  apdudata[8192] = {0};
    uint32 oad = 0;

    MEMCPY_R(&oad, ptFrame->pApdu + OOP_DATA_OFFSET, 4);
    //传入的报文是PIID之后的部分，传出的报文是PIID-ACD以后的部分
    msglen = invoke_get_req(ptChannel, &ptFrame->pApdu[OOP_DATA_OFFSET], ptFrame->apduLen-OOP_DATA_OFFSET, apdudata, 8192);
    PRTL_BUF_DEBUG(apdudata, msglen, "OAD %08X result:", oad);

    if(msglen <= 5 && msglen > 0)
    {
        memcpy(pOutMsg + OOP_DATA_OFFSET, ptFrame->pApdu + OOP_DATA_OFFSET, 4);
        pOutMsg[OOP_DATA_OFFSET + 4] = 0;
        pOutMsg[OOP_DATA_OFFSET + 5] = DATA_OTHER_REASON;
        msglen = 6;
    }
    
    if(msglen != 0)
    {
        memcpy(pOutMsg + OOP_DATA_OFFSET, apdudata, 4);
        pOutMsg[OOP_DATA_OFFSET + 4] = apdudata[4];
        if(0 == apdudata[4])
        {
            pOutMsg[OOP_DATA_OFFSET + 5] = apdudata[5];
        }
        else
        {
            pOutMsg[OOP_DATA_OFFSET + 5] = 16;
            GetDataMD5(apdudata + 5, msglen - 5, pOutMsg + OOP_DATA_OFFSET + 6);
            PRTL_BUF_DEBUG(pOutMsg + OOP_DATA_OFFSET + 6, 16, "OAD %08X MD5:", oad);
            msglen = 6 + 16;
        }
        //加上PIID-ACD之前的部分，和两个OPTIONAL 时戳与跟随上报信息
        msglen = service_msg_construct(ptChannel, ptFrame->pApdu, OOP_DATA_OFFSET, pOutMsg, msglen);
    }

    return msglen;
}

/**
*********************************************************************
* @name：       analyse_puAmr_acMeter
* @brief：      交采服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回698消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
*********************************************************************
*/
uint16 analyse_puAmr_acMeter(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    uint16 msglen = 0;
    int32 ret = 0;
    FRAME_HEAD_T tFrame;
    uint8 *pAddr = NULL;
    uint8  len = 0;
    uint8  uNewApdu[LOCAL_RECV_FRAM_MAXLEN] = {0};

    ret = puAmr_acMeter_revsnd(ptFrame->pApdu, ptFrame->apduLen, uNewApdu, &msglen);
    if ((msglen==0) || (ret != ERR_OK))
    {
        return 0;
    }

    /* 是否安全加密 */
    if (ptChannel->tContext.securityServer == TRUE)
    {
        msglen = GenSecurityApdu(uNewApdu, msglen, ptChannel->tContext.curSecurityMode, ptChannel->tContext.securityMode);
    }

    //上面pOutMsg返回的是完整的APDU，下面拼封装链路层头
    memcpy(&tFrame, ptFrame, sizeof(FRAME_HEAD_T));
    tFrame.dir = 1;
    tFrame.apduLen = msglen;
    tFrame.pApdu = uNewApdu;
    if(FALSE == ptFrame->NoEsamMac)
    {   /* 特殊类型地址保持原地址 */
        len = app_get_localaddr(&pAddr);
        if(0 != len)
        {
            tFrame.sAddr.len = len;
            memcpy(tFrame.sAddr.addr, pAddr, len);
            tFrame.sAddr.type = 0;
        }
    }
    tFrame.headlen = APUD_COUNT_FRAME_LEN(tFrame.sAddr.len, tFrame.sAddr.logicAddr);
    tFrame.msglen = tFrame.headlen + tFrame.apduLen + APP_698_CS_REGIN_SIZE;

    msglen = app_frame_to_package(&tFrame, pOutMsg, msgmaxlen, TRUE);

    return msglen;
}                                                       
                             
/**
*********************************************************************
* @name：       get_request_analyse
* @brief：      GET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回698消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 get_request_analyse(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    uint16 msglen = 0;
    uint8 reqtype = ptFrame->pApdu[1];
    FRAME_HEAD_T tFrame;
    uint8 *pAddr = NULL;
    uint8  len = 0;
    uint8  uNewApdu[LOCAL_RECV_FRAM_MAXLEN] = {0};

    //主站对终端的设置
    if((ptFrame->dir != 0) || (ptFrame->prm != 1) || (ptFrame->funCode == 0))
    {
        return msglen;
    }

    //TODO 时标处理 TimeTag OPTIONAL
    switch(reqtype)
    {
        case REQ_GET_NORMAL:             //读取一个对象属性
            msglen = get_normal_req(ptChannel, ptFrame, uNewApdu, LOCAL_RECV_FRAM_MAXLEN, pBreakFlag);
        break;
        case REQ_GET_NORMAL_LIST:        //读取若干对象属性
            msglen = get_normal_list_req(ptChannel, ptFrame, uNewApdu, LOCAL_RECV_FRAM_MAXLEN, pBreakFlag);
        break;
        case REQ_GET_RECORD:             //读取一个记录型对象属性
            msglen = get_record_req(ptChannel, ptFrame, uNewApdu, LOCAL_RECV_FRAM_MAXLEN, pBreakFlag);
        break; 
        case REQ_GET_RECORD_LIST:       //读取若干个记录型对象属性
            msglen = get_record_list_req(ptChannel, ptFrame, uNewApdu, LOCAL_RECV_FRAM_MAXLEN, pBreakFlag);
        break;    
        case REQ_GET_NEXT:              //读取分帧传输的下一帧
            msglen = get_next_req(ptChannel, ptFrame, uNewApdu, LOCAL_RECV_FRAM_MAXLEN, pBreakFlag);
        break;    
        case REQ_GET_MD5:               //读取一个对象属性的MD5值
            msglen = get_md5_req(ptChannel, ptFrame, uNewApdu, LOCAL_RECV_FRAM_MAXLEN, pBreakFlag);
        break;         
        default:
            return 0;
    }
    if(msglen==0)
    {
        return 0;
    }
    /* 是否安全加密 */
    if(ptChannel->tContext.securityServer == TRUE)
    {
        msglen = GenSecurityApdu(uNewApdu, msglen, ptChannel->tContext.curSecurityMode, ptChannel->tContext.securityMode);
    }

    //上面pOutMsg返回的是完整的APDU，下面拼封装链路层头
    memcpy(&tFrame, ptFrame, sizeof(FRAME_HEAD_T));
    tFrame.dir = 1;
    tFrame.apduLen = msglen;
    tFrame.pApdu = uNewApdu;
    if(FALSE == ptFrame->NoEsamMac)
    {   /* 特殊类型地址保持原地址 */
        len = app_get_localaddr(&pAddr);
        if(0 != len)
        {
            tFrame.sAddr.len = len;
            memcpy(tFrame.sAddr.addr, pAddr, len);
            tFrame.sAddr.type = 0;
        }
    }
    tFrame.headlen = APUD_COUNT_FRAME_LEN(tFrame.sAddr.len, tFrame.sAddr.logicAddr);
    tFrame.msglen = tFrame.headlen + tFrame.apduLen + APP_698_CS_REGIN_SIZE;

    msglen = app_frame_to_package(&tFrame, pOutMsg, msgmaxlen, TRUE);

    return msglen;
}

#endif

#if DESC("设置服务处理函数",1)

/**
*********************************************************************
* @name：       set_err_segement_framing
* @brief：      SET服务的错误帧
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               uint16 msglen                消息长度
               headLen                      APDU头长度
* @param[out]：uint8 *pOutMsg                返回消息
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 set_err_seg_framing(OOP_OAD_U oad, OOP_DAR_E dar, uint8 *pOutMsg, uint16 msgmaxlen)
{
    uint16 offset = 4;

    //OAD
    memcpy_r(pOutMsg, &oad, sizeof(uint32));

    //DAR
    pOutMsg[offset++] = dar;

    return offset;
}

/**
*********************************************************************
* @name：       invoke_set_req
* @brief：      SET服务入口,这里处理是从oad开始
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 invoke_set_req(CHANNEL_CACHE_T *ptChannel, const uint8 *pInMsg, uint16 inLen, uint8 *pOutMsg, uint16 msgmaxlen)
{
    OOP_DAR_E oopDar = DATA_OTHER_REASON;
    OOP_OAD_U serviceOAD = {0}; 
    OAD_INFO_T pstOdaInfo = {0};    
    APDU_INFO_T apduInfo = {0};
    RESULT_DATA_T outData;
    int i = 0;
    #if (!defined AREA_SHANDONG)
    SecModWd_t secModWd = {0};
    #endif
         
    memcpy_r(&serviceOAD, &pInMsg[0], sizeof(OOP_OAD_U));
    PRTL_FMT_TRACE("set_normal_req: serviceOAD(0x%08x)\n", serviceOAD.value);
    PRTL_BUF_DEBUG(pInMsg, inLen, "输入的APDU:");

    //校验时戳
    if((ptChannel->tContext.timeTag.tagFlag > 0) && (ptChannel->tContext.timeTag.isTimeOut == TRUE))
    {
        return set_err_seg_framing(serviceOAD, DATA_TIMEFLAG_INVALID, pOutMsg, msgmaxlen);        
    }

    //安全模式，判断访问权限
    #if (!defined AREA_SHANDONG)
    secModWd = security_mod_get(ptChannel, serviceOAD.nObjID);
    PRTL_FMT_TRACE("请求的安全模式=%02X,默认安全模式=%02X\n", ptChannel->tContext.curSecurityMode, secModWd.set);
    if (ptChannel->tContext.curSecurityMode > secModWd.set)
    {
        PRTL_FMT_DEBUG("Object-%04X 安全模式级别不够!\n",serviceOAD.nObjID);
        oopDar = DATA_READ_WRITE_DENIED;
        return set_err_seg_framing(serviceOAD, oopDar, pOutMsg, msgmaxlen);
    }
    #endif
    apduInfo.hUdp = ptChannel->clientid;
    apduInfo.logicAddr = ptChannel->private[0];
    apduInfo.reqType = ptChannel->private[1];
    apduInfo.NoEsamMac = ptChannel->private[4]; //将通道里标记交采模组特殊标记转入apduInfo
    apduInfo.bpBreakInfo = (BP_INFO_T *)ptChannel->pApduBreak;
    apduInfo.connectionInfo = ptChannel->tContext;
    outData.pResultData = pOutMsg;
    outData.resultDataLen = 0;
    
    if(class_oadinfo_get(serviceOAD, &pstOdaInfo))
    {
        while(i < OOP_IC_MAX)
        {
            if(g_prtl_ic_op[i].ClassID == pstOdaInfo.pTab->class)
            {
                //TODO 调用前还要判断权限,还有判时戳
                oopDar = g_prtl_ic_op[i].InvokSet(&pstOdaInfo, &apduInfo, &pInMsg[0], inLen, &outData, msgmaxlen);
                if (oopDar != DATA_SUCCESS)
                {
                    PRTL_BUF_DEBUG(pInMsg, inLen, "InvokSet: fault response. dar(%d), class(%d)\n", oopDar, pstOdaInfo.pTab->class);
                }
                else
                {
                    PRTL_BUF_DEBUG(outData.pResultData, outData.resultDataLen, "返回的APDU:");
                }

                break;
            }
            
            i++;
        }

        if(i == OOP_IC_MAX)
        {
            oopDar = DATA_READ_WRITE_DENIED;
            PRTL_BUF_DEBUG(pInMsg, inLen, "InvokSet: not find parse func. dar(%d), class(%d)\n", oopDar, pstOdaInfo.pTab->class);
        }
    } 
    else
    {
        oopDar = DATA_OBJECT_UNDEFINED;
        PRTL_BUF_DEBUG(pInMsg, inLen, "InvokSet:not find oda table. dar(%d), oda(0x%08x)\n", oopDar, serviceOAD.value);
    }

    //记录每个OAD的编程信息
    if (oopDar == DATA_SUCCESS)
    {
        ptChannel->private[2] = 0;
        CheckERC_3118(ptChannel->clientid, PS_ON, serviceOAD);
    }

    return set_err_seg_framing(serviceOAD, oopDar, pOutMsg, msgmaxlen);
}

/**
*********************************************************************
* @name：       set_normal_req
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 set_normal_req(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    /*SetRequestNormal∷=SEQUENCE
    {
         服务序号-优先级           PIID，
         一个对象属性描述符 OAD，
         数据                 Data
    }*/
    
    uint16 msglen = 0;
    uint16 offset=0;
    uint16 tmpoffset=0;
    uint16 i=0;
    OOP_OAD_U OAD = {0};
    Proxyresquest proxyplc;
    uint8 buf[200]={0};
    uint8 cs=0;
//    uint8 EvtBuffer[5];
//    OOP_DAR_E dar = DATA_SUCCESS;

    //传入的报文是PIID之后的部分，传出的报文是PIID-ACD以后的部分
    memcpy_r(&OAD, ptFrame->pApdu+3, sizeof(OOP_OAD_U));
    if((OAD.value==0xF2090900) || (OAD.value==0xF2096200))
    {
        memset((uint8*)&proxyplc,0x00,sizeof(Proxyresquest));
        memset(buf,0x00,sizeof(buf));
        proxyplc.PIID = ptFrame->pApdu[2];
        proxyplc.ServerID = SERVERID_SETREQUEST;
        proxyplc.nChioce = 0x08;//du
        if(ptChannel->tContext.securityServer==TRUE)
        proxyplc.SecMod = ptChannel->tContext.curSecurityMode;
        else
        proxyplc.SecMod = SecMod_NULL;
        proxyplc.FrameHeadInfo.oad=OAD;
        memcpy(buf+offset,ptFrame->pApdu,3);
        offset = offset+3;
        buf[offset++]= 0xF2;
        buf[offset++]= 0x09;
        buf[offset++]= 0x02;
        buf[offset++]= 0x01;
        buf[offset++]= 0x06;
        buf[offset++]= 0x02;
        buf[offset++]= 0x08;
        buf[offset++]= 0x01;
        buf[offset++]= 0x00;
        buf[offset++]= 0x00;
        buf[offset++]= 0x64;
        buf[offset++]= 0x00;
        buf[offset++]= 0x64;
        buf[offset++]= 0x10;
        buf[offset++]= 0x68;
        buf[offset++]= 0x10;
        buf[offset++]= 0x00;
        tmpoffset = offset;
        buf[offset++]= 0x43;
        buf[offset++]= 0x00;
        buf[offset++]= 0x00;
        buf[offset++]= 0x00;
        buf[offset++]= 0x00;
        buf[offset++]= 0x00;
        buf[offset++]= 0x0D;
        buf[offset++]= 0x05;
        if(OAD.value==0xF2090900)
        {
        buf[offset++]= 0x80;
        buf[offset++]= 0x01;
        }
        else if(OAD.value==0xF2096200)
        {
        buf[offset++]= 0x01;
        buf[offset++]= 0x19;
        }
        buf[offset++]= ptFrame->pApdu[8];
        for(i=tmpoffset;i<offset;i++)
        {
            cs+=buf[i];
        }
        buf[offset++]= cs;
        buf[offset++]= 0x16;
     
        
        proxyplc.RequestApduLen=offset;
        memcpy(proxyplc.pRequestApdu,buf,proxyplc.RequestApduLen);
        PRTL_BUF_LOGCC(proxyplc.pRequestApdu,proxyplc.RequestApduLen,"转换后的透传设置频点报文");
        PRTL_FMT_LOGCC("ptChannel->tContext.securityServer is %d,ptChannel->tContext.curSecurityMode is %d\n",ptChannel->tContext.securityServer,ptChannel->tContext.curSecurityMode);
        if(ptChannel->tContext.securityServer==TRUE)
        {
            proxyplc.SecMod = ptChannel->tContext.curSecurityMode;
        }
        else
        {
            proxyplc.SecMod = SecMod_NULL;
        }
        proxyplc.FrameHeadInfo.pipe = ptChannel->id;
        proxyplc.FrameHeadInfo.ctrl.ctrl = ptFrame->dir|ptFrame->prm|ptFrame->separate|ptFrame->sc|ptFrame->funCode;
        PRTL_FMT_LOGCC("proxyplc.FrameHeadInfo.ctrl.ctrl is %d,proxyplc.SecMod is %d\n",proxyplc.FrameHeadInfo.ctrl.ctrl,proxyplc.SecMod);
        proxyplc.FrameHeadInfo.tsa.af=ptFrame->sAddr.len;
        memcpy(proxyplc.FrameHeadInfo.tsa.add,ptFrame->sAddr.addr,proxyplc.FrameHeadInfo.tsa.af);
        PRTL_FMT_LOGCC("proxyplc.FrameHeadInfo.tsa.af is %d\n",proxyplc.FrameHeadInfo.tsa.af);
        ProxyWriteQueue(&ProxyQueue,(uint8*)&proxyplc,sizeof(Proxyresquest));
        return 0;

    }
    msglen = invoke_set_req(ptChannel, &ptFrame->pApdu[OOP_DATA_OFFSET], ptFrame->apduLen-OOP_DATA_OFFSET, &pOutMsg[OOP_DATA_OFFSET], msgmaxlen-OOP_DATA_OFFSET);

    if(msglen != 0)
    {
        //dar = pOutMsg[4]; //OAD+DAR
        //PRTL_FMT_TRACE("set_normal_res:msglen(%d),dar(%d)\n",msglen,dar);

#if 0        
        //设置成功时先写一下终端编程事件记录
        if(dar == DATA_SUCCESS)
        {
            EvtBuffer[0] = DT_OAD;
            memcpy(&EvtBuffer[1], pOutMsg, 4);
            EventList.pCheckERC03(EvtBuffer, 5);
        }
#endif
        //加上PIID-ACD之前的部分，和两个OPTIONAL 时戳与跟随上报信息
        msglen = service_msg_construct(ptChannel, ptFrame->pApdu, OOP_DATA_OFFSET, pOutMsg, msglen);
    }

    return msglen;
}


/**
*********************************************************************
* @name：       set_normal_list_req
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 set_normal_list_req(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    /*SetRequestNormalList∷=SEQUENCE
    {
         服务序号-优先级           PIID，
         若干个对象属性            SEQUENCE OF
         {
            一个对象属性描述符 OAD，
            数据                 Data
         }
    }*/

    uint16 msglen = 0;
    uint8 EvtBuffer[100]; //限定最多设置20个OAD 20*5
    uint16 evtOffset = 0;      //事件buffer中的偏移
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 sequenceNum;
    uint32  dataLen = 0;
    uint16  offset = 0;
    uint8 *outTmp = &pOutMsg[OOP_DATA_LIST_OFFSET];   //留出空间写OAD前面部分（ServiceID,responseID,PIID-ID, SEQUENCE OF）
    uint16 msgmaxlenTmp = msgmaxlen-OOP_DATA_LIST_OFFSET; //留出空间写OAD前面部分
    uint16 tmpMsgLen = 0;

    sequenceNum = ptFrame->pApdu[3]; //序列个数
    
    while(sequenceNum--)
    {
        dataLen = 4; //偏掉OAD

        //偏掉数据长度，到达下一个OAD
        if (!apdu_next_position_get(&ptFrame->pApdu[OOP_DATA_LIST_OFFSET+offset], &dataLen, ptFrame->apduLen, DT_NULL, FALSE))
        {
            return msglen;
        }
        
        //传入的报文是PIID之后的部分，传出的报文是PIID-ACD以后的部分
        tmpMsgLen = invoke_set_req(ptChannel, &ptFrame->pApdu[OOP_DATA_LIST_OFFSET + offset], dataLen, outTmp, msgmaxlenTmp);
        if(tmpMsgLen != 0)
        {
            dar = outTmp[4]; //OAD+DAR
            PRTL_BUF_TRACE(outTmp, tmpMsgLen, "set_normal_res sub:dar(%d)\n",dar);
            //设置成功时先写一下终端编程事件记录
            if((dar == DATA_SUCCESS) && (evtOffset < 100))
            {
                EvtBuffer[evtOffset++] = DT_OAD;
                memcpy(&EvtBuffer[evtOffset], outTmp, 4);
                evtOffset +=4;
            }
        }

        outTmp += tmpMsgLen;
        msgmaxlenTmp -= tmpMsgLen;

        offset += dataLen; //APDU的内偏移
        msglen += tmpMsgLen;
    }

    if(msglen != 0)
    {
        PRTL_FMT_TRACE("set_normal_res:msglen(%d),dar(%d)\n",msglen,dar);
#if 0  
        EventList.pCheckERC03(EvtBuffer, evtOffset);
#endif
        //加上PIID-ACD之前的部分，和两个OPTIONAL 时戳与跟随上报信息
        msglen = service_msg_construct(ptChannel, ptFrame->pApdu, OOP_DATA_LIST_OFFSET, pOutMsg, msglen);
    }

    return msglen;
}

/**
*********************************************************************
* @name：       stg_normal_list_req
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 stg_normal_list_req(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    /*SetThenGetRequestNormalList∷=SEQUENCE
    {
         服务序号-优先级                  PIID，
         若干个设置后读取对象属性              SEQUENCE OF
         {
            一个设置的对象属性 OAD，
            数据                 Data
            一个读取的对象属性 OAD，
            延时读取时间             unsigned
         }
    }*/

    uint16 msglen = 0;
    uint8 EvtBuffer[100]; //限定最多设置20个OAD 20*5
    uint16 evtOffset = 0;      //事件buffer中的偏移
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 sequenceNum;
    uint32  dataLen = 0;
    uint16  offset = 0;
    uint8 *outTmp = &pOutMsg[OOP_DATA_LIST_OFFSET];   //留出空间写OAD前面部分（ServiceID,responseID,PIID-ID, SEQUENCE OF）
    uint16 msgmaxlenTmp = msgmaxlen-OOP_DATA_LIST_OFFSET; //留出空间写OAD前面部分
    uint16 tmpMsgLen = 0;
    uint8 delays = 0; //写后再读的延时秒数 

    sequenceNum = ptFrame->pApdu[3]; //序列个数
    
    while(sequenceNum--)
    {
        dataLen = 4; //OAD的长度

        //OAD后的数据长度
        if (!apdu_next_position_get(&ptFrame->pApdu[OOP_DATA_LIST_OFFSET+offset], &dataLen, ptFrame->apduLen, DT_NULL, FALSE))
        {
            return msglen;
        }
        
        //传入的报文是PIID之后的部分，传出的报文是PIID-ACD以后的部分
        tmpMsgLen = invoke_set_req(ptChannel, &ptFrame->pApdu[OOP_DATA_LIST_OFFSET + offset], dataLen, outTmp, msgmaxlenTmp);
        if(tmpMsgLen != 0)
        {
            dar = outTmp[4]; //OAD+DAR
            PRTL_BUF_TRACE(outTmp, tmpMsgLen, "stg_normal_res sub:dar(%d)\n",dar);
            //设置成功时先写一下终端编程事件记录
            if((dar == DATA_SUCCESS) && (evtOffset < 100))
            {
                EvtBuffer[evtOffset++] = DT_OAD;
                memcpy(&EvtBuffer[evtOffset], outTmp, 4);
                evtOffset +=4;
            }
        }

        outTmp += tmpMsgLen;
        msgmaxlenTmp -= tmpMsgLen;

        offset += dataLen; //APDU的内偏移
        msglen += tmpMsgLen;

        //设置后读取
        dataLen = 5; //OAD长度（4） + 延时读取时间长度（1） unsigned 

        delays = ptFrame->pApdu[OOP_DATA_LIST_OFFSET + offset+4];
        PRTL_FMT_TRACE("delay time: %d s \n",delays);
        
        if(delays != 0)
        {
            sleep(delays);  //延时处理
        }
        else
        {
            //为0则由服务器决定延时
        }

        //先将通道里的私有数据1 - pApdu[1] 设置成1,( 05 01)的读法
        ptChannel->private[1] = 1;
        tmpMsgLen = invoke_get_req(ptChannel, &ptFrame->pApdu[OOP_DATA_LIST_OFFSET + offset], dataLen, outTmp, msgmaxlenTmp);
        outTmp += tmpMsgLen;
        msgmaxlenTmp -= tmpMsgLen;

        offset += dataLen;  //APDU的内偏移

        msglen += tmpMsgLen;
    }

    if(msglen != 0)
    {
        PRTL_FMT_TRACE("set_normal_res:msglen(%d),dar(%d)\n",msglen,dar);

#if 0  
        EventList.pCheckERC03(EvtBuffer, evtOffset);
#endif


        //加上PIID-ACD之前的部分，和两个OPTIONAL 时戳与跟随上报信息
        msglen = service_msg_construct(ptChannel, ptFrame->pApdu, OOP_DATA_LIST_OFFSET, pOutMsg, msglen);
    }

    return msglen;
}

/**
*********************************************************************
* @name：       set_request_analyse
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回698消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 set_request_analyse(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    uint16 msglen = 0;
    uint8 reqtype = ptFrame->pApdu[1];
    FRAME_HEAD_T tFrame;
    uint8 *pAddr = NULL;
    uint8  len = 0;
    uint8  uNewApdu[LOCAL_RECV_FRAM_MAXLEN] = {0};
    OOP_OAD_U oad;

    //主站对终端的设置
    if((ptFrame->dir != 0) || (ptFrame->prm != 1))
    {
        return msglen;
    }

    //编程事件是否记录标志
    MEMZERO(&oad, sizeof(oad));
    ptChannel->private[2] = -1;

    //TODO 时标处理 TimeTag OPTIONAL

    switch(reqtype)
    {
        case REQ_SET_NORMAL:             //设置一个对象属性
            msglen = set_normal_req(ptChannel, ptFrame, uNewApdu, LOCAL_RECV_FRAM_MAXLEN, pBreakFlag);
        break;
        case REQ_SET_NORMAL_LIST:        //设置若干对象属性
            msglen = set_normal_list_req(ptChannel, ptFrame, uNewApdu, LOCAL_RECV_FRAM_MAXLEN, pBreakFlag);
        break;
        case REQ_STG_NORMAL_LIST:        //设置后读取若干对象属性
            msglen = stg_normal_list_req(ptChannel, ptFrame, uNewApdu, LOCAL_RECV_FRAM_MAXLEN, pBreakFlag);
        break; 
        default:
            return 0;
    }
    if(msglen==0)
    {
        return 0;
    }
    //编程事件记录 PS_OK记录 PS_ERR不记录
    if (ptChannel->private[2] == 0)
    {
        CheckERC_3118(ptChannel->clientid, PS_OK, oad);
    }
    else
    {
        CheckERC_3118(ptChannel->clientid, PS_ERR, oad);
    }
    ptChannel->private[2] = 0;

    /* 是否安全加密 */
    if(ptChannel->tContext.securityServer == TRUE)
    {
        msglen = GenSecurityApdu(uNewApdu, msglen, ptChannel->tContext.curSecurityMode, ptChannel->tContext.securityMode);
    }

    //上面pOutMsg返回的是完整的APDU，下面拼封装链路层头
    memcpy(&tFrame, ptFrame, sizeof(FRAME_HEAD_T));
    tFrame.dir = 1;
    tFrame.apduLen = msglen;
    tFrame.pApdu = uNewApdu;
    if(FALSE == ptFrame->NoEsamMac)
    {   /* 特殊类型地址保持原地址 */
        len = app_get_localaddr(&pAddr);
        if(0 != len)
        {
            tFrame.sAddr.len = len;
            memcpy(tFrame.sAddr.addr, pAddr, len);
            tFrame.sAddr.type = 0;
        }
    }
    tFrame.headlen = APUD_COUNT_FRAME_LEN(tFrame.sAddr.len, tFrame.sAddr.logicAddr);
    tFrame.msglen = tFrame.headlen + tFrame.apduLen + APP_698_CS_REGIN_SIZE;

    msglen = app_frame_to_package(&tFrame, pOutMsg, msgmaxlen, TRUE);

    return msglen;
}

#endif

#if DESC("操作服务处理函数",1)

/**
*********************************************************************
* @name：       act_err_seg_framing
* @brief：      ACT服务的错误帧
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               uint16 msglen                消息长度
               headLen                      APDU头长度
* @param[out]：uint8 *pOutMsg                返回消息
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 act_err_seg_framing(OOP_OAD_U oad, OOP_DAR_E dar, uint8 *pOutMsg, uint16 msgmaxlen)
{
    uint16 offset = 4;

    //OAD
    memcpy_r(pOutMsg, &oad, sizeof(uint32));

    //DAR
    pOutMsg[offset++] = dar;

    //DATA OPTIONAL
    pOutMsg[offset++] = 0;

    return offset;
}

/**
*********************************************************************
* @name：       invoke_act_req
* @brief：      SET服务入口,这里处理是从oad开始
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag          
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 invoke_act_req(CHANNEL_CACHE_T *ptChannel, const uint8 *pInMsg, uint32 inLen, uint8 *pOutMsg, uint16 msgmaxlen)
{
    OOP_DAR_E oopDar = DATA_OTHER_REASON;
    OOP_OAD_U serviceOAD = {0}; 
    OAD_INFO_T pstOdaInfo = {0};    
    APDU_INFO_T apduInfo = {0};
    RESULT_DATA_T outData;
    #if (!defined AREA_SHANDONG)
    SecModWd_t secModWd = {0};
    #endif
    int i = 0;

    memcpy_r(&serviceOAD, &pInMsg[0], sizeof(OOP_OAD_U));
    PRTL_FMT_TRACE("invoke_act_req:serviceOAD(%08x)\n", serviceOAD.value);
    if( serviceOAD.value == 0xF0010800)
    {
        PRTL_FMT_DEBUG( "输入的APDU: F0 01 08 00 len %d \n",inLen);
    }else
    {
        PRTL_BUF_DEBUG(pInMsg, inLen, "输入的APDU:");
    }
    

    //校验时戳
    if((ptChannel->tContext.timeTag.tagFlag > 0) && (ptChannel->tContext.timeTag.isTimeOut == TRUE))
    {
        return act_err_seg_framing(serviceOAD, DATA_TIMEFLAG_INVALID, pOutMsg, msgmaxlen);        
    }

    //安全模式，判断访问权限
    #if (!defined AREA_SHANDONG)
    secModWd = security_mod_get(ptChannel, serviceOAD.nObjID);
    PRTL_FMT_TRACE("请求的安全模式=%02X,默认安全模式=%02X\n", ptChannel->tContext.curSecurityMode, secModWd.action);
    if(ptChannel->tContext.curSecurityMode > secModWd.action)
    {
        PRTL_FMT_DEBUG("Object-%04X 安全模式级别不够!\n",serviceOAD.nObjID);
        oopDar = DATA_READ_WRITE_DENIED;
        return act_err_seg_framing(serviceOAD, oopDar, pOutMsg, msgmaxlen);
    }
    #endif
    apduInfo.hUdp = ptChannel->clientid;
    apduInfo.logicAddr = ptChannel->private[0];
    apduInfo.reqType = ptChannel->private[1];
    apduInfo.NoEsamMac = ptChannel->private[4]; //将通道里标记交采模组特殊标记转入apduInfo
    apduInfo.bpBreakInfo = (BP_INFO_T *)ptChannel->pApduBreak;
    apduInfo.connectionInfo = ptChannel->tContext;
    outData.pResultData = pOutMsg;
    outData.resultDataLen = 0;

    PRTL_FMT_LOGCC("serviceOAD.value is %x\n",serviceOAD.value);
    if (class_omdinfo_get(serviceOAD, &pstOdaInfo))
    {
        while (i < OOP_IC_MAX)
        {
            if(g_prtl_ic_op[i].ClassID == pstOdaInfo.pTab->class)
            {
                //TODO 调用前还要判断权限,还有判时戳,OAD就不在报文中传了
                oopDar = g_prtl_ic_op[i].InvokAct(&pstOdaInfo, &apduInfo, &pInMsg[0], inLen, &outData, msgmaxlen);
                if (oopDar != DATA_SUCCESS)
                {
                    PRTL_BUF_DEBUG(pInMsg, inLen, "InvokAct: fault response. dar(%d), class(%d)\n", oopDar, pstOdaInfo.pTab->class);
                }
                else
                {
                    PRTL_BUF_DEBUG(outData.pResultData, outData.resultDataLen, "返回的APDU:");
                }

                break;
            }

            i++;
        }

        if (i == OOP_IC_MAX)
        {
            oopDar = DATA_READ_WRITE_DENIED;
            PRTL_BUF_DEBUG(pInMsg, inLen, "InvokAct: not find parse func. dar(%d), class(%d)\n", oopDar, pstOdaInfo.pTab->class);
        }
    }  
    else
    {
        oopDar = DATA_OBJECT_UNDEFINED;
        PRTL_BUF_DEBUG(pInMsg, inLen, "InvokAct:not find oda table. dar(%d), oda(0x%08x)\n", oopDar, serviceOAD.value);
    } 

    /* 和文件传输 buf_to_data_class18_F000_get_update 的约定 */
    if(0 == outData.resultDataLen && 0xff == pOutMsg[0] && 0xff == pOutMsg[1] 
       && 0xff == pOutMsg[2] && 0xff == pOutMsg[3])
    {
        return 0;
    }
    if(0 == outData.resultDataLen)
    {
        return act_err_seg_framing(serviceOAD, oopDar, pOutMsg, msgmaxlen);
    }
    return outData.resultDataLen;
}

/**
*********************************************************************
* @name：       act_normal_req
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 act_normal_req(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    /*SetRequestNormal∷=SEQUENCE
    {
         服务序号-优先级           PIID，
         一个对象属性描述符 OMD，
         数据                 Data
    }*/

    uint16 msglen = 0;
    OOP_OAD_U OAD = {0};
    Proxyresquest proxyplc;
    uint8 buf[200];
    //uint16 buflen=0;
    uint16 offset=0;
    uint16 framelen=0;
    uint16 tmpoffset=7;
    uint16 tmpoffset1=0;
    //uint8 tsalen=0;
    uint8 bytes=0;
    OOP_TSA_T tsa;
    //OOP_OAD_U port;
    OOP_METER_BASIC_T  basic;
//    uint8 EvtBuffer[5];
//    OOP_DAR_E dar = DATA_SUCCESS;

    //传入的报文是PIID之后的部分，传出的报文是PIID-ACD以后的部分
     
    memcpy_r(&OAD, ptFrame->pApdu+3, sizeof(OOP_OAD_U));
    if(OAD.value==0xF2097F00)
    {
        memset((uint8*)&proxyplc,0x00,sizeof(Proxyresquest));
        proxyplc.PIID = ptFrame->pApdu[2];
        proxyplc.ServerID = SERVERID_ACTIONREQUEST;
        proxyplc.nChioce = 0x08;//du
        if(ptChannel->tContext.securityServer==TRUE)
        proxyplc.SecMod = ptChannel->tContext.curSecurityMode;
        else
        proxyplc.SecMod = SecMod_NULL;
        proxyplc.FrameHeadInfo.oad=OAD;
        memcpy(buf+offset,ptFrame->pApdu,3);
        offset = offset+3;

        if(ptFrame->pApdu[tmpoffset++] !=DT_STRUCTURE)
        {


        }
        tmpoffset++;
        if(ptFrame->pApdu[tmpoffset++] !=DT_TSA)
        {

        }
        tmpoffset++;
        tsa.af =ptFrame->pApdu[tmpoffset++];
        memcpy(tsa.add,&ptFrame->pApdu[tmpoffset],tsa.len+1);
        tmpoffset = tmpoffset+tsa.len+1;

        if(TRUE==app_get_meter_basic_info_by_tsa(tsa,&basic))
        {
            #if AREA_ZHEJIANG
            memcpy_r((uint8*)&buf[offset],(uint8*)&basic.port,sizeof(OOP_OAD_U));
            offset =offset + sizeof(OOP_OAD_U);
            buf[offset++]= basic.baud;
            buf[offset++]= 0x02;
            buf[offset++]= 0x08;
            buf[offset++]= 0x01;
            buf[offset++]= 0x00;
            #else
            buf[offset++]= 0xF2;
            buf[offset++]= 0x09;
            buf[offset++]= 0x02;
            buf[offset++]= 0x01;
            buf[offset++]= 0x06;
            buf[offset++]= 0x02;
            buf[offset++]= 0x08;
            buf[offset++]= 0x01;
            buf[offset++]= 0x00;
            #endif
            
        }
        else
        {
            buf[offset++]= 0xF2;
            buf[offset++]= 0x09;
            buf[offset++]= 0x02;
            buf[offset++]= 0x01;
            buf[offset++]= 0x06;
            buf[offset++]= 0x02;
            buf[offset++]= 0x08;
            buf[offset++]= 0x01;
            buf[offset++]= 0x00;
        }
        if(ptFrame->pApdu[tmpoffset++] !=DT_LONG_UNSIGNED)
        {
        
        }
        memcpy(buf+offset,ptFrame->pApdu+tmpoffset,2);
        offset =offset+2;
        memcpy(buf+offset,ptFrame->pApdu+tmpoffset,2);
        offset =offset+2;
        tmpoffset =tmpoffset +2;
   
        if(ptFrame->pApdu[tmpoffset++] !=DT_OCTET_STRING)
        {

        }
        tmpoffset1=tmpoffset;
        framelen = get_len_offset(ptFrame->pApdu+tmpoffset, &bytes);
        framelen = framelen + bytes;
        memcpy(buf+offset,ptFrame->pApdu+tmpoffset1,framelen);
        offset=offset+framelen;
        PRTL_BUF_LOGCC(ptFrame->pApdu,ptFrame->apduLen,"载波透明转发");
        proxyplc.RequestApduLen=offset;
        memcpy(proxyplc.pRequestApdu,buf,proxyplc.RequestApduLen);
        PRTL_BUF_LOGCC(proxyplc.pRequestApdu,proxyplc.RequestApduLen,"转换后的载波透传报文");
        proxyplc.FrameHeadInfo.pipe = ptChannel->id;
        proxyplc.FrameHeadInfo.ctrl.ctrl = ptFrame->dir|ptFrame->prm|ptFrame->separate|ptFrame->sc|ptFrame->funCode;
        PRTL_FMT_LOGCC("proxyplc.FrameHeadInfo.ctrl.ctrl is %d\n",proxyplc.FrameHeadInfo.ctrl.ctrl);
        proxyplc.FrameHeadInfo.tsa.len=ptFrame->sAddr.len;
        proxyplc.FrameHeadInfo.tsa.vxd = ptFrame->sAddr.logicAddr;
        proxyplc.FrameHeadInfo.tsa.type = ptFrame->sAddr.type;
        memcpy(proxyplc.FrameHeadInfo.tsa.add,ptFrame->sAddr.addr,proxyplc.FrameHeadInfo.tsa.len);
        PRTL_FMT_LOGCC("proxyplc.FrameHeadInfo.tsa.af is %d\n",proxyplc.FrameHeadInfo.tsa.af);
        ProxyWriteQueue(&ProxyQueue,(uint8*)&proxyplc,sizeof(Proxyresquest));
        return 0;

    }

    if(ptChannel->pApduBreak)
    {
        ((BP_INFO_T *)ptChannel->pApduBreak)->piid = ptFrame->pApdu[2];
        ((BP_INFO_T *)ptChannel->pApduBreak)->channelid = ptChannel->id;
        ((BP_INFO_T *)ptChannel->pApduBreak)->frameaddr = (uint8*)ptFrame;
    }
    msglen = invoke_act_req(ptChannel, &ptFrame->pApdu[OOP_DATA_OFFSET], ptFrame->apduLen-OOP_DATA_OFFSET, &pOutMsg[OOP_DATA_OFFSET], msgmaxlen-OOP_DATA_OFFSET);

    if(msglen != 0)
    {
        //dar = pOutMsg[4]; //OAD+DAR
//        PRTL_FMT_TRACE("set_normal_res:msglen(%d),dar(%d)\n",msglen,dar);
#if 0        
        //设置成功时先写一下终端编程事件记录
        if(dar == DATA_SUCCESS)
        {
            EvtBuffer[0] = DT_OAD;
            memcpy(&EvtBuffer[1], pOutMsg, 4);
            EventList.pCheckERC03(EvtBuffer, 5);
        }
#endif
        //加上PIID-ACD之前的部分，和两个OPTIONAL 时戳与跟随上报信息
        msglen = service_msg_construct(ptChannel, ptFrame->pApdu, OOP_DATA_OFFSET, pOutMsg, msglen);
    }

    return msglen;
}


/**
*********************************************************************
* @name：       act_normal_list_req
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 act_normal_list_req(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    /*SetRequestNormalList∷=SEQUENCE
    {
         服务序号-优先级           PIID，
         若干个对象属性            SEQUENCE OF
         {
            一个对象属性描述符 OAD，
            数据                 Data
         }
    }*/

    uint16 msglen = 0;
    uint8 EvtBuffer[100]; //限定最多设置20个OAD 20*5
    uint16 evtOffset = 0;      //事件buffer中的偏移
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 sequenceNum;
    uint32  dataLen = 0;
    uint16  offset = 0;
    uint8 *outTmp = &pOutMsg[OOP_DATA_LIST_OFFSET];   //留出空间写OAD前面部分（ServiceID,responseID,PIID-ID, SEQUENCE OF）
    uint16 msgmaxlenTmp = msgmaxlen-OOP_DATA_LIST_OFFSET; //留出空间写OAD前面部分
    uint16 tmpMsgLen = 0;

    sequenceNum = ptFrame->pApdu[3]; //序列个数
    
    while(sequenceNum--)
    {
        dataLen = 4; //偏掉OAD

        //偏掉数据长度，到达下一个OAD
        if (!apdu_next_position_get(&ptFrame->pApdu[OOP_DATA_LIST_OFFSET+offset], &dataLen, ptFrame->apduLen, DT_NULL, FALSE))
        {
            return msglen;
        }
        
        //传入的报文是PIID之后的部分，传出的报文是PIID-ACD以后的部分
        tmpMsgLen = invoke_act_req(ptChannel, &ptFrame->pApdu[OOP_DATA_LIST_OFFSET + offset], dataLen, outTmp, msgmaxlenTmp);
        if(tmpMsgLen != 0)
        {
            dar = outTmp[4]; //OAD+DAR
            PRTL_BUF_TRACE(outTmp, tmpMsgLen, "act_normal_res sub:dar(%d)\n",dar);
            //设置成功时先写一下终端编程事件记录
            if((dar == DATA_SUCCESS) && (evtOffset < 100))
            {
                EvtBuffer[evtOffset++] = DT_OAD;
                memcpy(&EvtBuffer[evtOffset], outTmp, 4);
                evtOffset +=4;
            }
        }
        
        outTmp += tmpMsgLen;
        msgmaxlenTmp -= tmpMsgLen;

        offset += dataLen; //APDU的内偏移
        msglen += tmpMsgLen;
    }

    if(msglen != 0)
    {
#if 0
        EventList.pCheckERC03(EvtBuffer, evtOffset);
#endif
        //加上PIID-ACD之前的部分，和两个OPTIONAL 时戳与跟随上报信息
        msglen = service_msg_construct(ptChannel, ptFrame->pApdu, OOP_DATA_LIST_OFFSET, pOutMsg, msglen);
    }

    return msglen;
}

/**
*********************************************************************
* @name：       atg_normal_list_req
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 atg_normal_list_req(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    /*SetThenGetRequestNormalList∷=SEQUENCE
    {
         服务序号-优先级                  PIID，
         若干个设置后读取对象属性              SEQUENCE OF
         {
            一个设置的对象属性 OAD，
            数据                 Data
            一个读取的对象属性 OAD，
            延时读取时间             unsigned
         }
    }*/

    uint16 msglen = 0;
    uint8 EvtBuffer[100]; //限定最多设置20个OAD 20*5
    uint16 evtOffset = 0;      //事件buffer中的偏移
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 sequenceNum;
    uint32  dataLen = 0;
    uint16  offset = 0;
    uint8 *outTmp = &pOutMsg[OOP_DATA_LIST_OFFSET];   //留出空间写OAD前面部分（ServiceID,responseID,PIID-ID, SEQUENCE OF）
    uint16 msgmaxlenTmp = msgmaxlen-OOP_DATA_LIST_OFFSET; //留出空间写OAD前面部分
    uint16 tmpMsgLen = 0;

    sequenceNum = ptFrame->pApdu[3]; //序列个数
    
    while(sequenceNum--)
    {
        dataLen = 4; //OAD的长度

        //OAD后的数据长度
        if (!apdu_next_position_get(&ptFrame->pApdu[OOP_DATA_LIST_OFFSET+offset], &dataLen, ptFrame->apduLen, DT_NULL, FALSE))
        {
            return msglen;
        }
        
        //传入的报文是PIID之后的部分，传出的报文是PIID-ACD以后的部分
        tmpMsgLen = invoke_act_req(ptChannel, &ptFrame->pApdu[OOP_DATA_LIST_OFFSET + offset], dataLen, outTmp, msgmaxlenTmp);
        if(tmpMsgLen != 0)
        {
            dar = outTmp[4]; //OAD+DAR
            PRTL_BUF_TRACE(outTmp, tmpMsgLen, "atg_normal_res sub:dar(%d)\n",dar);
            //设置成功时先写一下终端编程事件记录
            if((dar == DATA_SUCCESS) && (evtOffset < 100))
            {
                EvtBuffer[evtOffset++] = DT_OAD;
                memcpy(&EvtBuffer[evtOffset], outTmp, 4);
                evtOffset +=4;
            }
        }

        outTmp += tmpMsgLen;
        msgmaxlenTmp -= tmpMsgLen;

        offset += dataLen; //APDU的内偏移
        msglen += tmpMsgLen;

        //设置后读取
        dataLen = 5; //OAD长度（4） + 延时读取时间长度（1） unsigned 
        
        //先将通道里的私有数据1 - pApdu[1] 设置成1,( 05 01)的读法
        ptChannel->private[1] = 1;
        tmpMsgLen = invoke_get_req(ptChannel, &ptFrame->pApdu[OOP_DATA_LIST_OFFSET + offset], dataLen, outTmp, msgmaxlenTmp);
        outTmp += tmpMsgLen;
        msgmaxlenTmp -= msglen;

        offset += dataLen;  //APDU的内偏移

        msglen += tmpMsgLen;
    }

    if(msglen != 0)
    {
        PRTL_FMT_TRACE("set_normal_res:msglen(%d),dar(%d)\n",msglen,dar);
#if 0
        EventList.pCheckERC03(EvtBuffer, evtOffset);
#endif
        //加上PIID-ACD之前的部分，和两个OPTIONAL 时戳与跟随上报信息
        msglen = service_msg_construct(ptChannel, ptFrame->pApdu, OOP_DATA_LIST_OFFSET, pOutMsg, msglen);
    }

    return msglen;
}

                             
/**
*********************************************************************
* @name：       action_request_analyse
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回698消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 action_request_analyse(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    uint16 msglen = 0;
    uint8 reqtype = ptFrame->pApdu[1];
    FRAME_HEAD_T tFrame;
    uint8 *pAddr = NULL;
    uint8  len = 0;
    uint8  uNewApdu[LOCAL_RECV_FRAM_MAXLEN] = {0};

    //主站对终端的设置
    if((ptFrame->dir != 0) || (ptFrame->prm != 1))
    {
        return msglen;
    }

    //TODO 时标处理 TimeTag OPTIONAL

    switch(reqtype)
    {
        case REQ_ACT_NORMAL:             //操作一个对象属性
            msglen = act_normal_req(ptChannel, ptFrame, uNewApdu, LOCAL_RECV_FRAM_MAXLEN, pBreakFlag);
        break;
        case REQ_ACT_NORMAL_LIST:        //操作若干对象属性
            msglen = act_normal_list_req(ptChannel, ptFrame, uNewApdu, LOCAL_RECV_FRAM_MAXLEN, pBreakFlag);
        break;
        case REQ_ATG_NORMAL_LIST:        //操作后读取若干对象属性
            msglen = atg_normal_list_req(ptChannel, ptFrame, uNewApdu, LOCAL_RECV_FRAM_MAXLEN, pBreakFlag);
        break; 
        default:
            return 0;
    }
    if(msglen == 0)
    {
        return 0;
    }
    /* 是否安全加密 */
    if(ptChannel->tContext.securityServer == TRUE)
    {
        msglen = GenSecurityApdu(uNewApdu, msglen, ptChannel->tContext.curSecurityMode, ptChannel->tContext.securityMode);
    }

    //上面pOutMsg返回的是完整的APDU，下面拼封装链路层头
    memcpy(&tFrame, ptFrame, sizeof(FRAME_HEAD_T));
    tFrame.dir = 1;
    tFrame.apduLen = msglen;
    tFrame.pApdu = uNewApdu;
    if(FALSE == ptFrame->NoEsamMac)
    {   /* 特殊类型地址保持原地址 */
        len = app_get_localaddr(&pAddr);
        if(0 != len)
        {
            tFrame.sAddr.len = len;
            memcpy(tFrame.sAddr.addr, pAddr, len);
            tFrame.sAddr.type = 0;
        }
    }
    tFrame.headlen = APUD_COUNT_FRAME_LEN(tFrame.sAddr.len, tFrame.sAddr.logicAddr);
    tFrame.msglen = tFrame.headlen + tFrame.apduLen + APP_698_CS_REGIN_SIZE;
    tFrame.bExtmsg = FALSE;
    msglen = app_frame_to_package(&tFrame, pOutMsg, msgmaxlen, TRUE);

    return msglen;
}

#endif

#if DESC("操作服务处理函数",1)

/**
*********************************************************************
* @name：       rpt_normal_list_res
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 rpt_normal_list_res(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    uint16 msglen = 0;
    uint32 offset = 0;
    uint8 piid = 0;
    uint8 sequenceNum = 0;
    OOP_OAD_U oad = {0};
    uint8 i = 0;

    piid = ptFrame->pApdu[2];
    sequenceNum = ptFrame->pApdu[3];
    PRTL_FMT_TRACE("rpt_normal_list_res. PIID(%d), sequenceNum(%d)\n", piid, sequenceNum);

    for(i = 0; i < sequenceNum; i++)
    {
        memcpy_r(&oad, &ptFrame->pApdu[OOP_DATA_LIST_OFFSET+offset], sizeof(uint32));
        offset += sizeof(uint32);

        //check
        PRTL_FMT_TRACE("check normal response: [%d]oad(0x%08x)\n", i, oad.value);
    }

    rpt_response_check(RPT_TYPE_TASK, piid);
    rpt_response_check(RPT_TYPE_EVENT, piid);
    rpt_response_check(RPT_TYPE_RETRY, piid);
    
    return msglen;
}

/**
*********************************************************************
* @name：       rpt_record_list_res
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 rpt_record_list_res(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    uint16 msglen = 0;
    uint32 offset = 0;
    uint8 piid = 0;
    uint8 sequenceNum = 0;
    OOP_OAD_U *pOads = NULL;
    uint8 i = 0;

    piid = ptFrame->pApdu[2];
    sequenceNum = ptFrame->pApdu[3];
    PRTL_FMT_TRACE("rpt_record_list_res. PIID(%d), sequenceNum(%d)\n", piid, sequenceNum);

    pOads = (OOP_OAD_U*)MALLOC_S(sequenceNum*sizeof(OOP_OAD_U));

    for(i = 0; i < sequenceNum; i++)
    {
        memcpy_r(&pOads[i], &ptFrame->pApdu[OOP_DATA_LIST_OFFSET+offset], sizeof(uint32));
        offset += sizeof(uint32);

        //check
        PRTL_FMT_TRACE("check record response: [%d]oad(0x%08x)\n", i, pOads[i].value);
    }
    
    rpt_response_check(RPT_TYPE_TASK, piid);
    rpt_response_check(RPT_TYPE_EVENT, piid);
    rpt_response_check(RPT_TYPE_MTREVT, piid);
    rpt_response_check(RPT_TYPE_RETRY, piid);

    FREE_S(pOads);
    return msglen;
}


/**
*********************************************************************
* @name：       rpt_trans_data_res
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 rpt_trans_data_res(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    uint16 msglen = 0;
    uint8 piid = 0;

    piid = ptFrame->pApdu[2];

    PRTL_FMT_TRACE("rpt_trans_data_res. PIID(%d)\n", piid);

    rpt_response_check(RPT_TYPE_MTREVT, piid);
    rpt_response_check(RPT_TYPE_RETRY, piid);
    
    return msglen;    
}
                             
/**
*********************************************************************
* @name：       rpt_trans_data_res
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回APDU消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 rpt_minute_data_res(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    uint16 msglen = 0;
    uint8 piid = 0;

    piid = ptFrame->pApdu[2];

    PRTL_FMT_TRACE("rpt_minute_data_res. PIID(%d)\n", piid);

    rpt_response_check(RPT_TYPE_MTREVT, piid);
    
    return msglen;    
}

/**
*********************************************************************
* @name：       report_response_analyse
* @brief：      SET服务入口
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint16 msgmaxlen             返回消息最大长度
* @param[out]：uint8 *pOutMsg               返回698消息
               uint8 *pBreakFlag            是否返回应用分帧
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 report_response_analyse(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag)
{
    uint16 msglen = 0;
    uint8 restype = ptFrame->pApdu[1];
    uint8  uNewApdu[LOCAL_RECV_FRAM_MAXLEN] = {0};

    if(ptFrame->dir != 0 || ptFrame->prm != 0)
    {
        return msglen;
    }

    switch(restype)
    {
        case RES_RPT_NORMAL_LIST:        //上报若干个对象属性响应
            msglen = rpt_normal_list_res(ptChannel, ptFrame, uNewApdu, LOCAL_RECV_FRAM_MAXLEN, pBreakFlag);
        break;
        case RES_RPT_RECORD_LIST:        //上报若干个记录型对象属性的响应
            msglen = rpt_record_list_res(ptChannel, ptFrame, uNewApdu, LOCAL_RECV_FRAM_MAXLEN, pBreakFlag);
        break;
        case RES_RPT_TRANS_DATA:         //上报透明数据的响应
            msglen = rpt_trans_data_res(ptChannel, ptFrame, uNewApdu, LOCAL_RECV_FRAM_MAXLEN, pBreakFlag);
        break; 
        case RES_RPT_MINUTE_DATA:         //上报透明数据的响应
            msglen = rpt_trans_data_res(ptChannel, ptFrame, uNewApdu, LOCAL_RECV_FRAM_MAXLEN, pBreakFlag);
        break; 
        default:
            return 0;   
    }
    
    return msglen;    
}

#endif
#if DESC("apdu缓存",1)
ALL_APDU_CACHE_T g_apduCache;

/**********************************************************************
* @name      : apdu_cache_init
* @brief     ：初始化函数
* @param[in] 
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：
* @Update    :
**********************************************************************/
void apdu_cache_init()
{
    memset(&g_apduCache,0,sizeof(g_apduCache));
    pthread_rwlock_init(&g_apduCache.lock,NULL);
}

/**********************************************************************
* @name      : apdu_cache_get
* @brief     ：根据oad获取对应的apdu 如果当前能找到apdu 但是时标相差过大 也不用
* @param[in]    oad
* @param[out]：outbuf 找到的apdu
* @return    ：apdu长度 0表示没找到
* @Create    : 
* @Date      ：
* @Update    :
**********************************************************************/
uint16 apdu_cache_get(OOP_OAD_U oad,uint8* outbuf)
{
    uint16 apdulen = 0;
    uint16 i;
    time_t timenow = time(NULL);
    pthread_rwlock_rdlock(&g_apduCache.lock);
    for(i=0;i<g_apduCache.cacheNum;i++)
    {
        if(oad.value == g_apduCache.cacheList[i].oad.value)
        {
            //30秒内的都算有效
            PRTL_FMT_DEBUG("findoad %08x now %d apdutime %d apdulen %d \n", oad.value,timenow,g_apduCache.cacheList[i].apdutime,g_apduCache.cacheList[i].apdulen);
            if(abs(timenow - g_apduCache.cacheList[i].apdutime ) < 30)
            {
                memcpy(outbuf,g_apduCache.cacheList[i].apdu,g_apduCache.cacheList[i].apdulen);
                apdulen = g_apduCache.cacheList[i].apdulen;
            }
            break;
        }
    }
    pthread_rwlock_unlock(&g_apduCache.lock);
    return apdulen;
}

/**********************************************************************
* @name      : apdu_cache_add
* @brief     ：根据oad将apdu加进去 
* @param[in]    oad
                inbuf 要添加的apdu
                inlen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：
* @Update    :
**********************************************************************/
int apdu_cache_add(OOP_OAD_U oad,uint8* inbuf,uint16 inlen)
{
    uint16 i;
    time_t timenow = time(NULL);
    int index = -1;
    pthread_rwlock_wrlock(&g_apduCache.lock);
    for(i=0;i<g_apduCache.cacheNum;i++)
    {
        if(oad.value == g_apduCache.cacheList[i].oad.value)
        {
            index = i;
            //找到原先写的apdu 可能是时差超过了 更新下
            memcpy(g_apduCache.cacheList[i].apdu,inbuf,inlen);
            g_apduCache.cacheList[i].apdulen = inlen;
            g_apduCache.cacheList[i].apdutime = timenow;
            break;
        }
    }
    //需要在最后添加新的
    if(index == -1)
    {
        if(g_apduCache.cacheNum >= MAX_APDU_CACHE_NUM)
        {
            PRTL_FMT_DEBUG("g_apduCache.cacheNum %d\n", g_apduCache.cacheNum);
            pthread_rwlock_unlock(&g_apduCache.lock);
            return -1;
        }
        index = g_apduCache.cacheNum;
        g_apduCache.cacheList[index].oad.value = oad.value;
        memcpy(g_apduCache.cacheList[index].apdu,inbuf,inlen);
        g_apduCache.cacheList[index].apdulen = inlen;
        g_apduCache.cacheList[index].apdutime = timenow;
        g_apduCache.cacheNum++;
        PRTL_FMT_DEBUG("add oad %08x num %d \n",oad.value, g_apduCache.cacheNum);
    }
    pthread_rwlock_unlock(&g_apduCache.lock);
    return 0;
}


#endif

