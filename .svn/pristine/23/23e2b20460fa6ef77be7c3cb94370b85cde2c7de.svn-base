/*
*********************************************************************
* @file    oopSecurity.c
* @brief： apdu安全传输
* @author：贺宁
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/

#include "baseCommon.h"
#include "OopAll.h"
#include "comm.h"
#include "msg_id.h"
#include "appcomm.h"
#include "apducomm.h"
#include "macro.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopSecurity.h"
#include "sm3sum.h"

#if DESC("安全传输相关接口",1)

//根据OI获取默认的安全模式字
#define GEN_SECMODWD_TMN(get,set,action,proxy)\
		(((uint16)get << 12)|((uint16)set << 8)|((uint16)action << 4)|((uint16)proxy << 0))


/**********************************************************************
* @name      : default_security_mod_get
* @brief     ：根据对象OI获取默认的安全模式-终端
* @param[in] ：OI:    对象OI
* @param[out]：
* @return    ：默认的安全模式
* @Create    : 
* @Date      ：2019-12-31
* @Update    :
**********************************************************************/
SecModWd_t default_security_mod_get(uint16 OI)
{
    SecModWd_t secModWd;
    secModWd.secMod = 0;
    switch(OI&0xF000)
    {
    case 0x0000:
        secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL, 0, 0, 0x04);
        break;
    case 0x1000:
        secModWd.secMod = GEN_SECMODWD_TMN(SecMod_PlainMAC, 0, 0, 0);
    case 0x2000:
        if((OI&0x0FFF) == 0x002C)
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL, 0, 0, 0x04);
        else if((OI&0x0FFF) == 0x002F)
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_PlainMAC, SecMod_CipherMAC, SecMod_NULL, 0);
        else
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_PlainMAC, SecMod_CipherMAC, SecMod_PlainMAC, 0);
        break;
    case 0x3000:
        secModWd.secMod = GEN_SECMODWD_TMN(SecMod_PlainMAC, SecMod_CipherMAC, SecMod_CipherMAC, 0);
        break;
    case 0x4000:
        switch(OI&0x0FFF)
        {
        case 0x0000:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL, SecMod_CipherMAC, SecMod_NULL, 0);
            break;
        case 0x0001:
		case 0x0111:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL, SecMod_CipherMAC, 0, 0);
            break;
        case 0x0002:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL, SecMod_PlainMAC, 0, 0);
            break;
        case 0x0003:
		case 0x0103:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_PlainMAC, SecMod_PlainMAC, 0, 0);
            //secModWd.secMod = GEN_SECMODWD(SecMod_PlainMAC, SecMod_CipherMAC, 0, 0);
			break;
        case 0x000A:
        case 0x000B:
        case 0x0018:
        case 0x0019:
        case 0x001A:
        case 0x001B:
        case 0x001C:
        case 0x001D:
        case 0x001E:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_PlainMAC, SecMod_CipherMAC, 0, 0);
            break;
        default:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_PlainMAC, SecMod_CipherMAC, SecMod_CipherMAC, 0);
            break;
        }
        break;
    case 0x5000:
        secModWd.secMod = GEN_SECMODWD_TMN(SecMod_PlainMAC, 0, SecMod_PlainMAC, 0x00);
        break;
    case 0x6000:
        secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL, SecMod_NULL, SecMod_NULL, 0x00);
        break;
    case 0x7000:
        secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL, SecMod_NULL, SecMod_NULL, 0x00);
        break;
    case 0x8000:
        secModWd.secMod = GEN_SECMODWD_TMN(SecMod_PlainMAC, SecMod_CipherMAC, SecMod_CipherMAC, 0x00);
        break;
    case 0xF000:
        switch(OI&0x0FFF)
        {
        case 0x0000:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL, SecMod_CipherMAC, SecMod_CipherMAC, 0x00);
            break;
        case 0x0001:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL, SecMod_Cipher, SecMod_Cipher, 0x00);
            break;
        case 0x0002:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL, SecMod_NULL, SecMod_NULL, 0x00);
            break;
        case 0x0100:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL, SecMod_PlainMAC, SecMod_CipherMAC, 0x00);
            break;
        case 0x0101:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL, SecMod_CipherMAC, SecMod_Cipher, 0x00);
            break;
        default:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL, SecMod_NULL, SecMod_NULL, 0x00);
            break;
        }
        break;
    }
    return secModWd;
}


/**********************************************************************
* @name      : security_mod_get
* @brief     ：根据对象OI获取默认的安全模式-终端
* @param[in] ：OI:        对象OI
* @param[out]：
* @return    ：SecModWd_t 安全模式
* @Create    : 
* @Date      ：2019-12-31
* @Update    :
**********************************************************************/
SecModWd_t security_mod_get(CHANNEL_CACHE_T *ptChannel, uint16 OI)
{
    uint16 i;
    SecModWd_t tmpSecMod;
    tmpSecMod.secMod = 0x888F;
    NOMAL_OAD_T normal = {0};
    OOP_SECURITYFLAG_E flag = SECURITY_OFF;
    OOP_SCURITYMODE_T  mod  = {0};
    uint32 len = 0;
    int32  ret = -1;

    //该通道私有数据标记的是485-4口，全1地址，访问交采数据，需放开权限
    if (ptChannel->private[4])
    {
        return tmpSecMod;
    }
    /* 并发处理提高效率 档案相关不读数据中心 */
    //读安全模式选择
    normal.oad.value = 0xF1010200;
    ret = db_read_nomal(ptChannel->clientid, &normal, sizeof(OOP_SECURITYFLAG_E), (uint8*)&flag, &len);
    if ((ret != ERR_OK) || (len == 0) || (flag == SECURITY_OFF))
    {
        PRTL_FMT_DEBUG("db_read_nomal. ret(%d), flag(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, flag, normal.logicId, normal.infoNum, normal.oad.value);
        return tmpSecMod;
    }

    //读安全模式参数
    memset(&normal, 0, sizeof(normal));
    normal.oad.value = 0xF1010300;
    ret = db_read_nomal(ptChannel->clientid, &normal, sizeof(OOP_SCURITYMODE_T), (uint8*)&mod, &len);
    if ((ret != ERR_OK) || (len == 0))
    {
        PRTL_FMT_LOGCC("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return(default_security_mod_get(OI));
    }

    for(i = 0; i < mod.nNum; i++)
    {
        if(mod.mode[i].object == OI)
        {
            memcpy_r((uint8*)&tmpSecMod.secMod, &mod.mode[i].access, 2);
            return tmpSecMod;
        }
    }
    
    return(default_security_mod_get(OI));
}

/**********************************************************************
* @name      : protocol_securityrequest_long
* @brief     ：安全传输 解析出APDU内容 并逆向组装成报文
* @param[in] ：FRAME_HEAD_T *ptFrame        解析帧头
               uint16 outsize               返回消息最大长度
               APP_CONTEXT_t *pConnect      应用连接属性
* @param[out]：uint8 *pOutMsg               返回消息
               uint8 *pSecurity             安全传输标记
* @return    ：消息长度
* @Create    : 
* @Date      ：2019-12-5
* @Update    :
**********************************************************************/
uint32 protocol_securityrequest_long(FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint32 outsize, APP_CONTEXT_t *pContext, 
                                uint8 *pSecurity)
{
    uint32      Offset = 0;
    int         rslt = 0;

    uint8 *pData = NULL;
    uint8  DataType = 0; //密文1，明文0
    uint32 DataLen = 0;
    uint32 outDataLen = 0;
    uint16 outLentmp = 0;
    uint8  AuthType = 0; //验证类型，0：SID_MAC,1:RN,2:RN_MAC,3:SID

    SID_t SID;
    const uint8 *pMAC = NULL;
    uint8 MACLen = 0;
    uint8 *pAttachData = NULL;
    uint8 AttchDataLen = 0;

    uint8 TerDataHash[32] = {0}; //sm3哈希值
    SecurityData_t SecData;
    uint16 tmplen = 0;

    memset(&SecData, 0, sizeof(SecData));
    memset(&SID, 0, sizeof(SID));
    
    do
    {
        Offset = 1;
        
        DataType = ptFrame->pApdu[Offset++];
        if (DataType>1)
        {
            rslt = 255;
            break;
        }
        DataLen = ptFrame->pApdu[Offset++];
        
        if (DataLen == 0x81)
            DataLen = ptFrame->pApdu[Offset++];
        else if (DataLen == 0x82)
        {
            DataLen = ptFrame->pApdu[Offset++];
            DataLen <<= 8;
            DataLen += ptFrame->pApdu[Offset++];
        }
        else if (DataLen == 0x83)
        {
            DataLen = ptFrame->pApdu[Offset++];
            DataLen <<= 8;
            DataLen += ptFrame->pApdu[Offset++];
            DataLen <<= 8;
            DataLen += ptFrame->pApdu[Offset++];
        }
        
        pData = ptFrame->pApdu+Offset;
        Offset += DataLen;

        AuthType = ptFrame->pApdu[Offset++];
        if(AuthType > 3)
        {
            rslt = 255;
            break;
        }

        APP698_BUF_DEBUG(ptFrame->pApdu, MIN(ptFrame->apduLen, 2048), "长报文-解密类型[%d]-验证类型[%d]", DataType, AuthType);

        switch (AuthType)
        {
        case 0:
            {
                memcpy((uint8*)&SID, ptFrame->pApdu + Offset, 4);
                Offset += 4;

                AttchDataLen = ptFrame->pApdu[Offset++];
                pAttachData = ptFrame->pApdu+Offset;
                Offset += AttchDataLen;
                if (AttchDataLen != 2)
                {
                    rslt = 255;
                    APP698_FMT_DEBUG("按规范要求，附加数据值的长度需要是2\n");
                    break;
                }

                MACLen = ptFrame->pApdu[Offset++];
                pMAC = ptFrame->pApdu+Offset;
                Offset += MACLen;
                if(MACLen >= 0x80)
                {
                    rslt = 255;
                    break;
                }

                SecData.SecType = 0;
                SecData.sid = SID;
                Init_Fd_Esam();
                SetSecurityData(SecData);//清除随机数

                if (DataType == 0)
                {
                    /*1、明文长度大于 2045，则计算明文的 SM3 哈希值 TerDataHash。*/
                    rslt = calc_sm3((uint8 *)pData, DataLen, TerDataHash);
                    if (rslt != 0)
                    {
                        APP698_FMT_DEBUG("sm3哈希算法错误[%d]\n", rslt);
                        break;
                    }

                    /*2、调用 ESAM 接口，传入TerDataHash，SID_MAC。其中 SID_MAC 取值方式：SID 值标识，取自报文。SID 值附加数据，终端自行计算，
                    计算方式为【TerDataHash 的字节长度+MAC 字节长度】，2 字节十六进制，例如 0024 表示长度为36。MAC，取自报文。验证 Mac 是否正确。*/
                    
                    //计算真实附加数据
                    tmplen = sizeof(TerDataHash) + MACLen;
                    memcpy_r(pAttachData, &tmplen, 2);

                    if (ESAM_OK != TEsamDataVerifi(SID, (uint8*)pAttachData, (uint8*)TerDataHash, sizeof(TerDataHash), (uint8*)pMAC, 
                        (uint8*)TerDataHash, &outLentmp))
                    {
                        rslt = 28;
                        APP698_BUF_DEBUG(&SID, sizeof(SID), "SID:");
                        APP698_BUF_DEBUG(TerDataHash, sizeof(TerDataHash), "sm3哈希值:");
                        APP698_BUF_DEBUG(pAttachData, AttchDataLen, "附加数据:");
                        APP698_BUF_DEBUG(pMAC, MACLen, "MAC:");
                        break;
                    }

                    *pSecurity = SecMod_PlainMAC;
                     outDataLen = DataLen;

                }
                else if (DataType == 1)
                {
                    uint32  offset = 0;
                    uint32  len = 0;
                    uint8   type = 0;
                    uint8  *plaindata = NULL;
                    uint32  plainlen = 0;
                    uint8  *cipherdata = NULL;
                    uint32  cipherlen = 0;
                    uint8  *mac = NULL;
                    uint16  macLen = 0;

                    plaindata = (uint8 *)malloc(DataLen * sizeof(uint8));
                    if (plaindata == NULL)
                    {
                        APP698_FMT_DEBUG("申请空间失败\n");
                        rslt = 255;
                        break;
                    }

                    /*1、终端拆分密文,CipherTextData 切分为密文,CipherTextData1、CipherTextMac1；密文 CipherTextData2，CipherTextMac2；密文 CipherTextData3，
                    CipherTextMac3。访问 ESAM 接口，解密得到明文 PlainTextData1，PlainTextData2，PlainTextData3。其中 SID_MAC 取值方式：SID 值标识，取自主站报文。
                    SID 值附加数据，终端自行计算，计算方式为【例如第 1 段密文，CipherTextData1 的字节长度+CipherTextMac1 字节长度】，2 字节十六进制，例如 0024 
                    表示长度为 36。*/
                    while (offset < DataLen)
                    {
                        memcpy_r(&len, &pData[offset], 2);
                        offset += 2;
                        type = pData[offset++];
                        if (type != 1)
                        {
                            APP698_FMT_DEBUG("按规范要求，应该要求是密文+MAC\n");
                            rslt = 255;
                            break;
                        }

                        cipherdata = &pData[offset];
                        cipherlen = len - 1 - 4; //1字节类型，4字节mac
                        offset += cipherlen;
                        
                        macLen = 4;
                        mac = &pData[offset];
                        offset += macLen;

                        //计算真实附加数据,密文+mac长度
                        tmplen = len - 1;
                        memcpy_r(pAttachData, &tmplen, 2);

                        if (ESAM_OK != TEsamDataVerifi(SID, (uint8*)pAttachData, (uint8*)cipherdata, cipherlen, (uint8*)mac, (uint8*)cipherdata, 
                            &outLentmp))
                        {
                            rslt = 28;
                            break;
                        }
                        
                        memcpy(&plaindata[plainlen], cipherdata, outLentmp);
                        plainlen += outLentmp;
                    }

                    //明文数据统一存入pData
                    memcpy(pData, plaindata, plainlen);
                    outDataLen = plainlen;
                    
                    free(plaindata);
                    plaindata = NULL;

                    if (rslt != 0)
                    {
                        break;
                    }
                    else
                    {
                        *pSecurity = SecMod_CipherMAC;
                    }
                }
            }
            break;
        case 3:
            {
                memcpy((uint8*)&SID, ptFrame->pApdu + Offset, 4);
                Offset += 4;
                AttchDataLen = ptFrame->pApdu[Offset++];
                pAttachData = ptFrame->pApdu+Offset;
                Offset += AttchDataLen;
                if (AttchDataLen != 2)
                {
                    APP698_FMT_DEBUG("按规范要求，附加数据值的长度需要是2\n");
                    break;
                }
                
                SecData.SecType = 3;
                SecData.sid = SID;
                Init_Fd_Esam();
                SetSecurityData(SecData);//清除随机数
                
                if (DataType == 1)
                {
                    uint32  offset = 0;
                    uint32  len = 0;
                    uint8   type = 0;
                    uint8  *plaindata = NULL;
                    uint32  plainlen = 0;
                    uint8  *cipherdata = NULL;
                    uint32  cipherlen = 0;

                    plaindata = (uint8 *)malloc(DataLen * sizeof(uint8));
                    if (plaindata == NULL)
                    {
                        APP698_FMT_DEBUG("申请空间失败\n");
                        rslt = 255;
                        break;
                    }
                    
                    /*1、CipherTextData，切分为密文CipherTextData1，CipherTextData2，CipherTextData3。访问 ESAM 接口，解密得到明文 PlainTextData1，PlainTextData2，
                    PlainTextData3。（解密使用的 SID 取自主站报文，SID 值附加数据，终端自行计算，计算方式为【例如第 1 段密文，CipherTextData1 的字节长度】，
                    2 字节十六进制，例如0024 表示长度为 36。*/
                    while (offset < DataLen)
                    {
                        memcpy_r(&len, &pData[offset], 2);
                        offset += 2;
                        type = pData[offset++];
                        if (type != 2)
                        {
                            APP698_FMT_DEBUG("按规范要求，应该要求是密文\n");
                            rslt = 255;
                            break;
                        }

                        cipherdata = &pData[offset];
                        cipherlen = len - 1; //1字节类型
                        offset += cipherlen;


                        //计算真实附加数据,密文长度
                        tmplen = len - 1;
                        memcpy_r(pAttachData, &tmplen, 2);

                        if (ESAM_OK != TEsamDataVerifi(SID, (uint8*)pAttachData, (uint8*)cipherdata, cipherlen, NULL, (uint8*)cipherdata, 
                            &outLentmp))
                        {
                            rslt = 28;
                            break;
                        }
                        
                        memcpy(&plaindata[plainlen], cipherdata, outLentmp);
                        plainlen += outLentmp;
                    }

                    //明文数据统一存入pData
                    memcpy(pData, plaindata, plainlen);
                    outDataLen = plainlen;
                    
                    free(plaindata);
                    plaindata = NULL;

                    if (rslt != 0)
                    {
                        break;
                    }
                    else
                    {
                        *pSecurity = SecMod_Cipher;
                    }
                }
                else
                {
                    rslt = 255;
                    APP698_FMT_DEBUG("不支持的长报文解密类型\n");
                }
            }
            break;
        default:
            rslt = 255;
            APP698_FMT_DEBUG("不支持的长报文解密类型\n");
            break;
        }
    }while(0);

    APP698_FMT_DEBUG("长报文解密结果 rslt[%d] apduLen[%d]\n", rslt, outDataLen);
    if(rslt == 0)
    {
        memmove(ptFrame->pApdu, pData, outDataLen);
        ptFrame->apduLen = outDataLen;
    }
    else
    {
        //ResultData_t *tmpNode;;
        uint8 MakeBuffer[4];
        MakeBuffer[0] = SERVERID_SECURITYRESPONSE;
        MakeBuffer[1] = 2;//错误信息
        MakeBuffer[2] = rslt;//
        MakeBuffer[3] = 0;//无MAC
        //MakeBuffer[4] = 0;//跟随上报
        //MakeBuffer[5] = 0;//时间标签
        memcpy(&ptFrame->pApdu[0], &MakeBuffer[0], sizeof(MakeBuffer));
        ptFrame->apduLen = sizeof(MakeBuffer);
        return app_frame_to_package(ptFrame, pOutMsg, outsize, TRUE);
    }

    return app_frame_to_adaptpackage(ptFrame, pOutMsg, outsize, TRUE);
}


/**********************************************************************
* @name      : securityrequest_getapdu
* @brief     ：安全传输 解析出APDU内容 并逆向组装成报文
* @param[in] ：FRAME_HEAD_T *ptFrame        解析帧头
               uint16 outsize               返回消息最大长度
               APP_CONTEXT_t *pConnect      应用连接属性
* @param[out]：uint8 *pOutMsg               返回消息
               uint8 *pSecurity             安全传输标记
* @return    ：消息长度
* @Create    : 
* @Date      ：2019-12-5
* @Update    :
**********************************************************************/
uint32 protocol_securityrequest(FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint32 outsize, APP_CONTEXT_t *pContext, 
                                uint8 *pSecurity)
{
    uint16		Offset = 0;
    int 		rslt = 1;

    const uint8 *pData = NULL;
    uint8 DataType;//密文1，明文0
    uint32 DataLen = 0;
    uint16 outDataLen = 0;
    uint8 AuthType;//验证类型，0：SID_MAC,1:RN,2:RN_MAC,3:SID
    uint8 RandLen;
    const uint8 *pRand;//随机数
    SID_t SID;
    const uint8 *pMAC;
    uint8 MACLen;
    const uint8 *pAttachData;
    uint8 AttchDataLen;

    // SERVERID_SECURITYRESPONSE的格式判断，后期补充
    if (SERVERID_SECURITYREQUEST == ptFrame->pApdu[0])
    {
        rslt = serial_chkapdu(SERVERID_SECURITYREQUEST, 0, ptFrame->pApdu + 1, ptFrame->apduLen-1);
    }
    if(rslt < 0)
    {
        //APDU错误
        APP698_FMT_DEBUG("protocol_securityrequest解密 APDU错误 rslt = %d\n", rslt);
        rslt = 255;
    }
    else
    {
        ptFrame->apduLen = rslt + 1;//APDU长度+1字节服务ID
        Offset = 1;
        rslt = 0;
        do
        {
            DataType = ptFrame->pApdu[Offset++];
            if(DataType>1)
            {
                rslt = 255;
                break;
            }
            DataLen = ptFrame->pApdu[Offset++];

            if(DataLen == 0x81)
                DataLen = ptFrame->pApdu[Offset++];
            else if(DataLen == 0x82)
            {
                DataLen = ptFrame->pApdu[Offset++];
                DataLen <<= 8;
                DataLen += ptFrame->pApdu[Offset++];
            }
            else if(DataLen == 0x83)
            {
                DataLen = ptFrame->pApdu[Offset++];
                DataLen <<= 8;
                DataLen += ptFrame->pApdu[Offset++];
                DataLen <<= 8;
                DataLen += ptFrame->pApdu[Offset++];
            }

            //增加长报文加解密功能
            #if LONG_ENCRYPTION_EN == 1
            if (((DataType == 0) && (DataLen > 2045)) ||
                ((DataType == 1) && (DataLen > 2048)))
            {
                return protocol_securityrequest_long(ptFrame, pOutMsg, outsize, pContext, pSecurity);
            }
            #endif
            
            pData = ptFrame->pApdu+Offset;
            Offset += DataLen;
            //数据验证信息

            AuthType = ptFrame->pApdu[Offset++];
            if(AuthType>3)
            {
                rslt = 255;
                break;
            }
            SecurityData_t SecData;
            switch(AuthType)
            {
            case 0:
                memcpy((uint8*)&SID,ptFrame->pApdu+Offset,4);
                Offset += 4;

                AttchDataLen = ptFrame->pApdu[Offset++];
                pAttachData = ptFrame->pApdu+Offset;
                Offset += AttchDataLen;
                if(AttchDataLen>=0x80)
                {
                    rslt = 255;
                    break;
                }
                MACLen = ptFrame->pApdu[Offset++];
                pMAC = ptFrame->pApdu+Offset;
                Offset += MACLen;
                if(MACLen>=0x80)
                {
                    rslt = 255;
                    break;
                }
                SecData.SecType = 0;
                SecData.sid = SID;
                Init_Fd_Esam();
                SetSecurityData(SecData);//清除随机数
                if (ESAM_OK != TEsamDataVerifi(SID, (uint8*)pAttachData, (uint8*)pData, (uint16)DataLen, (uint8*)pMAC, (uint8*)pData, &outDataLen))
                {
                    rslt = 28;
                    break;
                }
                else
                {
                    if(DataType==0)//明文
                        *pSecurity = SecMod_PlainMAC;
                    else
                        *pSecurity = SecMod_CipherMAC;
                }
                break;
            case 1:
                RandLen = ptFrame->pApdu[Offset++];
                pRand = ptFrame->pApdu+Offset;
                Offset += RandLen;
                if(RandLen>=0x80)
                {
                    rslt = 255;
                    break;
                }
                SID.CLA = 0x80;
                SID.INS = 0x0E;
                SID.P1  = 0x40;
                SID.P2  = 0x02;
                Init_Fd_Esam();
                SecData.SecType = 1;
                SecData.sid = SID;
                memcpy(SecData.ClientRand,pRand,sizeof(SecData.ClientRand));
                SecData.ClientRandLen = RandLen;
                if(SetSecurityData(SecData)<0)
                {
                    rslt = 255;
                    break;
                }
                else
                {
                    if(DataType==0)//明文
                        *pSecurity = SecMod_PlainMAC;
                    else
                        rslt = 255;
                }
                outDataLen = DataLen;
                break;
            case 2:
                /*RandLen = ptFrame->pApdu[Offset++];
                pRand = ptFrame->pApdu+Offset;
                Offset += RandLen;
                if(RandLen>=0x80)
                {
                	rslt = 255;
                	break;
                }
                MACLen = ptFrame->pApdu[Offset++];
                pMAC = ptFrame->pApdu+Offset;
                Offset += MACLen;
                if(MACLen>=0x80)
                {
                	rslt = 255;
                	break;
                }
                if(SetClientRand(pRand,RandLen)<0)
                {
                	rslt = 255;
                	break;
                }*/
                rslt = 255;//ESAM暂不支持
                break;
            case 3:
                memcpy((uint8*)&SID,ptFrame->pApdu+Offset,4);
                Offset += 4;
                AttchDataLen = ptFrame->pApdu[Offset++];
                pAttachData = ptFrame->pApdu+Offset;
                Offset += AttchDataLen;
                if(AttchDataLen>=0x80)
                {
                    rslt = 255;
                    break;
                }
                SecData.SecType = 3;
                SecData.sid = SID;
                Init_Fd_Esam();
                SetSecurityData(SecData);//清除随机数
                if(ESAM_OK != TEsamDataVerifi(SID,(uint8*)pAttachData, (uint8*)pData, (uint16)DataLen, NULL, (uint8*)pData, &outDataLen))
                {
                    rslt = 28;
                    break;
                }
                else
                {
                    if(DataType==0)//明文
                        *pSecurity = SecMod_PlainMAC;
                    else
                        *pSecurity = SecMod_Cipher;
                }
                break;
            }
        }
        while(0);
    }
    
    APP698_FMT_LOGCC("protocol_securityrequest 解密结果 rslt = %d\n", rslt);
    if(rslt == 0)
    {
        memmove(ptFrame->pApdu, pData, outDataLen);
        ptFrame->apduLen = outDataLen;
    }
    else
    {
        //ResultData_t *tmpNode;;
        uint8 MakeBuffer[4];
        MakeBuffer[0] = SERVERID_SECURITYRESPONSE;
        MakeBuffer[1] = 2;//错误信息
        MakeBuffer[2] = rslt;//
        MakeBuffer[3] = 0;//无MAC
        //MakeBuffer[4] = 0;//跟随上报
        //MakeBuffer[5] = 0;//时间标签
        memcpy(&ptFrame->pApdu[0], &MakeBuffer[0], sizeof(MakeBuffer));
        ptFrame->apduLen = sizeof(MakeBuffer);
        return app_frame_to_package(ptFrame, pOutMsg, outsize, TRUE);
    }

    return app_frame_to_package(ptFrame, pOutMsg, outsize, TRUE);
}

/**********************************************************
* 函数名称：
* 功能描述：
    生成一个安全APDU

* 输入描述：
* 输出描述:

* 返回值：
**********************************************************/
uint32 GenSecurityApdu_long(uint8 *apduData, uint32 apduDataLen,SecurityMode_t SecMod,uint8 SecLev)
{
    uint8 *secApduData = NULL; //安全传输APDU
    uint32 offSet = 0;
    uint8 DataMAC[4] = {0};
    uint32 OutLen = 0;
    
    if(SecMod == SecMod_NULL)
        SecMod = SecMod_PlainMAC;

    if (apduDataLen > 257*1024)
    {
        APP698_FMT_DEBUG("apdu过长, 暂不支持\n");
        return 0;
    }

    secApduData = (uint8 *)malloc(258*1024*sizeof(uint8));
    if (secApduData == NULL)
    {
        APP698_FMT_DEBUG("申请空间失败\n");
        return 0;
    }

    if(ESAM_FAIL == ResponseFrameDeal_long(SecMod,apduData,apduDataLen,secApduData,&OutLen))
    {
        apduData[offSet++] = SERVERID_SECURITYRESPONSE;
        apduData[offSet++] = 2;//错误信息
        apduData[offSet++] = 255;//其他错误
        apduData[offSet++] = 0;//无MAC
    }
    else
    {
        if (OutLen > 258*1024)
        {
            APP698_FMT_DEBUG("超过最大空间范围\n");
            free(secApduData);
            secApduData = NULL;
            return 0;
        }
        
        apduData[offSet++] = SERVERID_SECURITYRESPONSE;
        if(SecMod == SecMod_PlainMAC || SecMod == SecMod_CipherMAC)
        {
            if(OutLen<sizeof(DataMAC))
            {
                apduData[offSet++] = 2;//错误信息
                apduData[offSet++] = 255;//其他错误
                apduData[offSet++] = 0;//无MAC
                free(secApduData);
                secApduData = NULL;
                return offSet;
            }
            else
            {
                if (SecMod == SecMod_PlainMAC)
                {
                    memcpy(DataMAC, secApduData + OutLen - 4, 4);
                    OutLen -= 4;
                }
            }
        }
        if(SecMod == SecMod_PlainMAC || SecMod == SecMod_NULL)
            apduData[offSet++] = 0;//明文传输
        else
            apduData[offSet++] = 1;//密文传输

        if(OutLen > 0xFFFF)
        {
            apduData[offSet++] = 0x83;
            apduData[offSet++] = (uint8)(OutLen>>16);
            apduData[offSet++] = (uint8)(OutLen>>8);
            apduData[offSet++] = (uint8)OutLen;
        }
        else if(OutLen>=0x80)
        {
            apduData[offSet++] = 0x82;
            apduData[offSet++] = (uint8)(OutLen>>8);
            apduData[offSet++] = (uint8)OutLen;
        }
        else
        {
            apduData[offSet++] = OutLen;
        }
        memcpy(apduData+offSet,secApduData,OutLen);
        offSet += OutLen;
        if(SecMod == SecMod_PlainMAC || SecMod == SecMod_CipherMAC)
        {
            apduData[offSet++] = 0x01;//OPTIONAL 带MAC
            apduData[offSet++] = 0x00;//CHIOCER 0 MAC
            apduData[offSet++] = 0x04;
            memcpy(apduData+offSet,DataMAC,4);
            offSet += 4;
        }
        else
        {
            apduData[offSet++] = 0x00;//OPTIONAL 不带MAC
        }
    }
    
    free(secApduData);
    secApduData = NULL;
    return offSet;
}


/**********************************************************
* 函数名称：
* 功能描述：
    生成一个安全APDU

* 输入描述：
* 输出描述:

* 返回值：
**********************************************************/
uint16 GenSecurityApdu(uint8 *apduData, uint16 apduDataLen,SecurityMode_t SecMod,uint8 SecLev)
{
    uint8 secApduData[APDU_BUFF_SIZE_MAX];//安全传输APDU
    uint16 offSet=0;
    uint8 DataMAC[4];
    uint16 OutLen=0;
    if(SecMod == SecMod_NULL)
        SecMod = SecMod_PlainMAC;
    if(ESAM_FAIL == ResponseFrameDeal(SecMod,apduData,apduDataLen,secApduData,&OutLen))
    {
        apduData[offSet++] = SERVERID_SECURITYRESPONSE;
        apduData[offSet++] = 2;//错误信息
        apduData[offSet++] = 255;//其他错误
        apduData[offSet++] = 0;//无MAC
    }
    else
    {
        apduData[offSet++] = SERVERID_SECURITYRESPONSE;
        if(SecMod == SecMod_PlainMAC || SecMod == SecMod_CipherMAC)
        {
            if(OutLen<sizeof(DataMAC))
            {
                apduData[offSet++] = 2;//错误信息
                apduData[offSet++] = 255;//其他错误
                apduData[offSet++] = 0;//无MAC
                return offSet;
            }
            else
            {
                memcpy(DataMAC,secApduData+OutLen-4,4);
                OutLen -= 4;
            }
        }
        if(SecMod == SecMod_PlainMAC || SecMod == SecMod_NULL)
            apduData[offSet++] = 0;//明文传输
        else
            apduData[offSet++] = 1;//密文传输

        if(OutLen>=0x80)
        {
            apduData[offSet++] = 0x82;
            apduData[offSet++] = (uint8)(OutLen>>8);
            apduData[offSet++] = (uint8)OutLen;
        }
        else
        {
            apduData[offSet++] = OutLen;
        }
        memcpy(apduData+offSet,secApduData,OutLen);
        offSet += OutLen;
        if(SecMod == SecMod_PlainMAC || SecMod == SecMod_CipherMAC)
        {
            apduData[offSet++] = 0x01;//OPTIONAL 带MAC
            apduData[offSet++] = 0x00;//CHIOCER 0 MAC
            apduData[offSet++] = 0x04;
            memcpy(apduData+offSet,DataMAC,4);
            offSet += 4;
        }
        else
        {
            apduData[offSet++] = 0x00;//OPTIONAL 不带MAC
        }
    }
    return offSet;
}

/**********************************************************
* 函数名称：
* 功能描述：
    生成一个上报安全APDU

* 输入描述：
* 输出描述:

* 返回值：
**********************************************************/
uint16 GenReportSecurityApdu(uint8 *apduData, uint16 apduDataLen)
{
    uint8 secApduData[APDU_BUFF_SIZE_MAX];//安全传输APDU
    uint8 RandBuff[12];//随机数
    uint16 offSet=0;
    uint8 DataMAC[4];
    memcpy(secApduData,apduData,apduDataLen);
    if(ESAM_FAIL == GetRandAndDataMac(secApduData,apduDataLen,DataMAC,RandBuff))
    {
        return apduDataLen;//读取ESAM信息失败，则不加密，使用明文上报
    }
    else
    {
        apduData[offSet++] = SERVERID_SECURITYREQUEST;
        apduData[offSet++] = 0;//明文传输
        if(apduDataLen>=0x80)
        {
            apduData[offSet++] = 0x82;
            apduData[offSet++] = (uint8)(apduDataLen>>8);
            apduData[offSet++] = (uint8)apduDataLen;
        }
        else
        {
            apduData[offSet++] = apduDataLen;
        }
        memcpy(apduData+offSet,secApduData,apduDataLen);
        offSet += apduDataLen;
        apduData[offSet++] = 0x02;//RAND+MAC
        apduData[offSet++] = 12;//随机数长度
        memcpy(apduData+offSet,RandBuff,12);
        offSet += 12;
        apduData[offSet++] = 4;//mac长度
        memcpy(apduData+offSet,DataMAC,4);
        offSet += 4;
    }
    return offSet;
}

/**********************************************************************
* @name      : esam_state_guishow
* @brief     ：esam密钥下发状态
* @param[in] ：esamState 内部协议     
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int esam_state_guishow(uint8 esamState)
{
    uint8              buf[52] = {0};
    uint16             offset = 0;
    int                ret = 0;

    if(esamState > 1)
    {
        //不存在的esam状态
        return 0;
    }

    //刷新工具栏
    buf[offset++] = 0x01;
    buf[offset++] = 0x05;
    buf[offset++] = esamState;

    ret = comm_send_msg(MSG_APPAMIN_ESAM_STATE, buf, offset);
    if(0 != ret)
    {
        PRTL_FMT_DEBUG("Send esam_state_guishow failed ret[%d]\n", ret);
    }
    
    return ret;
}

#endif
