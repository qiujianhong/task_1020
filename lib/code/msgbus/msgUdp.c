/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：UDP消息相关访问接口
* @date： 2019.10.23
*********************************************************************
*/
#include "msgUdp.h"

#define RECV_BUF_LENGTH           5120    /*recvfrom接收缓冲区大小*/
#define IS_LITTLE_ENDIAN          1     //组消息的时候index label msgtag是否用小端模式 0为大端 1为小端

/**********************************************************************
* @name      : get_len_offset
* @brief     ：获取串类型数据元素长度
* @param[in] ：const uint8 *indata      数据Buffer
* @param[out]：uint8 *bytes             便宜量
* @return    ：长度
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/
uint16 get_len_offset(uint8 *indata,uint8 *bytes)
{
    uint16 EleNum;
    uint8 nbytes = 1;
    
    EleNum = indata[0];
    if(EleNum==0x81)
    {
        EleNum = indata[1];
        nbytes = 2;
    }
    else if(EleNum==0x82)
    {
        EleNum = indata[1];
        EleNum <<= 8;
        EleNum += indata[2];
        nbytes = 3;
    }
    else if(EleNum==0x80 || EleNum>0x82)
    {
        return 0;
    }
    
    if(bytes != NULL)
        *bytes = nbytes;
        
    return EleNum;
}

/**********************************************************************
* @name      : set_len_offset
* @brief     ：设置元素长度
* @param[in] ：const uint8 *indata      数据Buffer
* @param[out]：const uint8 *indata      数据Buffer
* @return    ：长度
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/
uint8 set_len_offset(uint16 eleLen, uint8 *indata)
{
    if(eleLen < 0x80)
    {
        indata[0] = eleLen;
        return 1;
    }
    else if(eleLen <= 0xffff)
    {
        indata[0] = 0x82;
        indata[1] = (uint8)((eleLen >> 8) & 0xff);
        indata[2] = (uint8)(eleLen & 0xff);
        return 3;
    }
        
    return 0;
}


