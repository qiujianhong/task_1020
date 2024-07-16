#include "security.h"
#include "../include/protocol104.h"
#include "framecomm.h"
#include "TESAM.h"

static uint8   gSecurityEnable = 0;
//加解密接口缓存
static uint8   DAEasmUsing = 0;                                     //配网加密芯片使用权
static uint8   DAEsamSndbuf[DA_ESAM_MAX_LEN];
static uint8   DAEsamRcvbuf[DA_ESAM_MAX_LEN];
static uint8   gDAEsamDecBuf[DA_ESAM_MAX_LEN];                      //解密缓冲区
static uint8   gDAEsamSTemBuf0[DA_ESAM_MAX_LEN];                    //加密发送临时缓冲区
static uint8   gDAEsamSTemBuf1[DA_ESAM_MAX_LEN];                    //加密发送临时缓冲区

//连接参数缓存
static uint8   gHostRandNum[SECURITY_MAX_PIPE][8];                  //主站下发的随机数
static uint8   gNetRandNum[SECURITY_MAX_PIPE][8];                   //网关下发的随机数
static uint8   gMtuRandNum[SECURITY_MAX_PIPE][8];                   //终端产生的随机数
static uint8   gToolsID[SECURITY_MAX_PIPE][8];                      //管理工具ID
static uint16  gdCertBufIndx[SECURITY_MAX_PIPE][129] = {{0}};       //保存证书每帧的起始位置
static uint8   gDAEsamCertBuf[SECURITY_MAX_PIPE][DA_ESAM_MAX_LEN];  //终端加密证书缓冲区
static uint8   gCertCode[SECURITY_MAX_PIPE] = {0xff};               //证书标识，用于比较
static uint8   bfgCertCodeDown[SECURITY_MAX_PIPE] = {0};            //证书下载开始标志
static uint8   bfgMtuCertSendEn[SECURITY_MAX_PIPE][5] = {{0}};      //主站召测终端证书，终端返回证书逻辑控制标志
                                                                    //第一个字节存放返回主站的应用类型，不为0表示有证书需要上送
                                                                    //后面两字节是证书的长度
static uint8   gCertSendDelay[SECURITY_MAX_PIPE] = {5};             //上送证书等待延时
static uint8   bfgDAEsamRunEn[SECURITY_MAX_PIPE] = {0};             //配网加密认证通过标志，1主站认证,2网关认证,3证书管理工具

//接收/上报缓存
#if SECURITY_FRAME_FETCH == 1
static uint8   gRecvTempBuf[SECURITY_MAX_PIPE][SECURITY_FRAME_MAX_LEN+50] = {{0}};
static int     gRecvTempLen[SECURITY_MAX_PIPE] = {0};
#endif
//static uint8   gSendTempBuf1[DA_ESAM_MAX_LEN];//上报数据临时缓冲，用于被动发送数据临时缓冲
//static uint8   gSendTempBuf3[DA_ESAM_MAX_LEN];//上报数据临时缓冲，用于被动发送数据临时缓冲
//static uint8   gLogBuf[2048] = {0};

/*
********************************************************************
所属功能单元：配网加密模块
功能描述    ：获取加密芯片使用权限
输入参数    ：无
输出参数    ：无

函数返回值  ：1表示获取到，
              0表示未获取到， 有其他线程在使用

作者        ：朴军海
创建日期    ：2020年8月8日
*******************************************************************/
static int GetDAEsamUse(void)
{
    uint8 i = 0;

    while(i++<3)
    {
        if(DAEasmUsing)
        {
            usleep(100);
        }
        else
        {
            DAEasmUsing = 1;
            return 1;
        }
    }

    return 0;
}

/*
********************************************************************
所属功能单元：配网加密模块
功能描述    ：释放加密芯片使用权限
输入参数    ：无
输出参数    ：无

函数返回值  ：无

作者        ：朴军海
创建日期    ：2020年8月8日
*******************************************************************/
static void ReleaseADEsam(void)
{
    usleep(10000);//因为两次读数据需要延时10ms，这里统一处理了
    DAEasmUsing = 0;

    return ;
}

/*
********************************************************************
所属功能单元：配网加密模块
功能描述    ：检查104/101 业务是否需要需要验签
输入参数    ：输入数据缓冲区pinbuf 内为解密后的完整报文
输出参数    ：*ptype，验签类型

函数返回值  ：1表示需要验签，0表示不需验签

作者        ：朴军海
创建日期    ：2020年8月
*******************************************************************/
static int SignNeedCheck(uint8 apptype,uint8 *ptype)
{
    uint16 rtn = 0;

    switch(apptype)
    {
        case APP_TYPE_FILE_ABOUT:   //01 文件、参数相关
        case APP_TYPE_PARA_ACK:     //0x03 参数确认等
        case APP_TYPE_CTRL_SELECT:  //0x05 遥控选择、遥控撤销
        case APP_TYPE_CTRL_RUN:     //0x07遥控执行
        case APP_TYPE_UPDATA:       //0x08升级相关
        {
            rtn = 1;
            *ptype  = DA_ESAM_INFO_SIGN_CHECK;
            break;
        }
        default:
        {
            rtn = 0;
            break;
        }
    }

    return rtn;
}

/*
********************************************************************
所属功能单元：配网加密模块
功能描述    ：检查104/101 业务是否需要需要加密
输入参数    ：*pInPdu 数据源缓冲区
                  apptype ，应用类型
输出参数    ：无

函数返回值  ：1表示需要加密，0表示不需加密

作者        ：朴军海
创建日期    ：2019年8月8日
*******************************************************************/
uint8 CheckBufEnTry(uint8 *pInPdu,uint8 apptype)
{
    uint8 rtn = 0;
    S104FRAME_HEAD *p104;  //104规约帧格式

    //if(apptype == PRTL_GB104)
    {
        p104 = (S104FRAME_HEAD  *)pInPdu;
        if(p104->sCrtlType12.ibit.fir == 0)//I帧
        {
            //需要继续分析里面的报文，pjhtest
            rtn = 1;
        }
    }
#if 0
    else
    {
        if(pInPdu[0] != 0x10)
        {
            //需要继续分析里面的报文，pjhtest
            rtn = 1;
        }
    }
#endif
    return rtn;
}

/**********************************************************************
* @name      : security_get_serialnum
* @brief     ：获取8字节加密芯片序列号
* @param[in] ：
* @param[out]：pOutBuf 序号缓存
* @return    ：1表示获取成功，0表示处理失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
static uint16 DAMakeFrameHead(uint8 *pOutBuf, uint16 bLen, DA_SFP_S_NUM_U frameType)
{
    uint16 index = 0;

    frameType.ubit.keyport = 0; //来源于终端
    pOutBuf[index++] = 0xEB;
    pOutBuf[index++] = bLen>>8;//长度高字节在前，低字节在后
    pOutBuf[index++] = bLen&0xff;
    pOutBuf[index++] = 0xEB;
    pOutBuf[index++] = frameType.commtype[0];
    pOutBuf[index++] = frameType.commtype[1];

    return index;
}

/*
********************************************************************
所属功能单元：GB101规约处理模块
功能描述        ：计算校验和
输入参数        : p 数据；len 数据长度
输出参数        ：
函数返回值      ：校验和
作     者       : 朴军海
创建日期        ：2014年5月3日
修改者          ：丁俊杰
修改说明        ：
修改日期        ：
*******************************************************************/
static uint8 chksum(uint8 *p,uint16 len)
{
    uint16 i ;
    uint8 bchksum = 0;

    for(i=0;i<len;i++)
        bchksum += *(p+i);
    return bchksum;
}

/**********************************************************************
* @name      : DAAddFrameTail
* @brief     ：组帧，增加校验和及结束符
* @param[in] ：pOutBuf帧缓存   bLen报文类型+数据域长度
* @param[out]：pOutBuf输出报文缓存
* @return    ：返回输出安全协议报文长度
* @Create    : 陈德昶
* @Date      ：2020-03-06
* @Update    :

**********************************************************************/
static uint16 DAAddFrameTail(uint8 *pOutBuf, uint16 bLen)
{
    uint16 index = bLen + 4; //加上4字节头

    pOutBuf[index++] = chksum((uint8 *)&pOutBuf[4],bLen);
    pOutBuf[index++] = 0xD7;
    //PRTL_BUF_DEBUG(pOutBuf, index, "SECURITY发送报文:\n");

    return index;
}

/**********************************************************************
* @name      : DACheckFrame
* @brief     ：检查帧格式
* @param[in] ：pInBuf需要检查的帧缓存，inLen缓存长度
* @param[out]：
* @return    ：>0帧长度，0帧不完整需要继续缓存，-1未检查到有效帧
* @Create    : 陈德昶
* @Date      ：2020-03-10
* @Update    :

**********************************************************************/
static int DACheckFrame(uint8* pInBuf, int inLen)
{
    DASAFE_FRAME_HEAD *p = (DASAFE_FRAME_HEAD *)pInBuf;

    //检查头
    if (p->headH != 0xEB || p->headL != 0xEB)
    {
        return -1;
    }

    //计算数据域长度
    int len = p->lenH << 8;
    len += p->lenL;
    //4头+1校验和+1结束符
    if (len + 6 > SECURITY_FRAME_MAX_LEN)
    {
        PRTL_FMT_DEBUG("SECURITY帧过长,长:%u,最长:%u\n", len, SECURITY_FRAME_MAX_LEN);

        return -1;
    }

    //帧不完整，4头+数据域长度+1检验和+1结束符
    if (len + 6 > inLen)
        return 0;

    //检查校验和及结束符
    uint8 cs = chksum(&pInBuf[4], len);

    if (cs != pInBuf[4+len] || pInBuf[5+len] != 0xD7)
    {
        PRTL_FMT_DEBUG("SECURITY校验和或结束符错误,校验和%u、%u,结束符:%u\n", cs, pInBuf[4+len], pInBuf[5+len]);

        return -1;
    }

    return len + 6;
}

#if SECURITY_FRAME_FETCH == 1
static int DAGetFrame(int pipe, uint8* pInBuf, int inLen, uint8* pOutBuf)
{
    int ret = 0;
    int offset = 0;

    //缓存未存满
    if (inLen > 0 && gRecvTempLen[pipe] + inLen < sizeof(gRecvTempBuf[pipe]))
    {
        memcpy(gRecvTempBuf[pipe], pInBuf, inLen);
        gRecvTempLen[pipe] += inLen;
        inLen = 0;
    }
    else
        PRTL_FMT_DEBUG("SECURITY接收帧缓存已满\n");

    while (offset + sizeof(DASAFE_FRAME_HEAD) + 2 < gRecvTempLen[pipe])
    {
        int len = DACheckFrame(&gRecvTempBuf[pipe][offset], gRecvTempLen[pipe]-offset);

        if (len < 0)
        {
            offset++;
            continue;
        }

        if (len == 0)
            break;

        PRTL_FMT_DEBUG("SECURITY收到帧,长度%u\n", len);
        //保存正确帧
        memcpy(pOutBuf, &gRecvTempBuf[pipe][offset], len);
        ret = len;
        offset += ret;
        break;
    }

    //移除已解析的数据
    MEMMOVE_S(gRecvTempBuf[pipe], 1024, &gRecvTempBuf[pipe][offset], 1024-offset, gRecvTempLen[pipe] - offset);
    gRecvTempLen[pipe] -= offset;

    return ret;
}
#endif

/*
********************************************************************
所属功能单元：配网加密模块
功能描述    ：加密/解密出现错误的组帧
输入参数    ：*dType，错误类型
              commtype报文类型低字节
输出参数    ：*pOutBuf,组帧存放缓冲区

函数返回值  ：组帧数据长度

作者        ：朴军海
创建日期    ：2019年8月15日(纪念抗战结束)
*******************************************************************/
static int MakeErrFram(uint8 *pOutBuf,uint16 dType,DA_SFP_S_NUM_U frameType)
{
    uint16 index = 6;

    frameType.ubit.encry = 0;
    pOutBuf[index++] = 0x1f;    //应用类型管理过程错误0x1f
    pOutBuf[index++] = 0;       //安全扩展区长度高字节
    pOutBuf[index++] = 0x02;    //安全扩展区长度低字节
    pOutBuf[index++] = dType>>8;
    pOutBuf[index++] = dType&0xff;

    DAMakeFrameHead(pOutBuf, index-4, frameType);
    return DAAddFrameTail(pOutBuf, index-4);
}

/**********************************************************************
* @name      : security_get_serialnum
* @brief     ：获取8字节加密芯片序列号
* @param[in] ：
* @param[out]：pOutBuf 序号缓存
* @return    ：1表示获取成功，0表示处理失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_get_serialnum(uint8* pOutBuf)
{
    int ret = 0;

    if(GetDAEsamUse())//先申请加密芯片使用权
    {
        int len =  DAEsamCommand(DA_ESAM_SERI_READ,DAEsamRcvbuf,DAEsamSndbuf,NULL,NULL);
        PRTL_FMT_DEBUG("security_get_serialnum() len=%d \n", len);

        if(len>0)
        {
            memcpy(pOutBuf, &DAEsamRcvbuf[4], 8);

            ret = 1;
        }

        ReleaseADEsam();
    }

    PRTL_FMT_DEBUG("security_get_serialnum() ret=%d \n", ret);
    return ret;
}

/**********************************************************************
* @name      : security_get_randnum
* @brief     ：获取8字节随机数
* @param[in] ：pInBuf 网络随机数
* @param[out]：pOutBuf 获取的随机数
* @return    ：1表示获取成功，0表示处理失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_get_randnum(uint8* pOutBuf)
{
    int ret = 0;

    if(GetDAEsamUse())//先申请加密芯片使用权
    {
        gDAEsamSTemBuf0[0] = 0;//字节长度8
        gDAEsamSTemBuf0[1] = 0;//长度高字节
        gDAEsamSTemBuf0[2] = 8;//长度低字节
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)gNetRandNum,8);
        //对主站随机数进行签名
        int len =  DAEsamCommand(DA_ESAM_RAND_READ,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        if(len > 0)
        {
            memcpy((char *)pOutBuf,(char *)&DAEsamRcvbuf[4],8);//随机数8字节
            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_gen_randnumsign
* @brief     ：生成随机数签名
* @param[in] ：pInBuf  主机随机数
* @param[out]：pOutBuf 签名结果
* @return    ：1表示获取成功，0表示处理失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_get_randnumsign(uint8* pInBuf, uint8* pOutBuf)
{
    int ret = 0;

    //签名
    if(GetDAEsamUse())          //先申请加密芯片使用权
    {
        gDAEsamSTemBuf0[0] = 0; //字节长度8
        gDAEsamSTemBuf0[1] = 0; //长度高字节
        gDAEsamSTemBuf0[2] = 8; //长度低字节
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)pInBuf,8);
        int len =  DAEsamCommand(DA_ESAM_RAND_SIGN,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        if(len>0)
        {
            memcpy((char *)gMtuRandNum,(char *)&DAEsamRcvbuf[4],8);//随机数8字节
            memcpy((char *)pOutBuf,(char *)&DAEsamRcvbuf[4],72);

            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_chk_randnumsign
* @brief     ：检查随机数签名
* @param[in] ：pInBuf  签名缓存
* @param[out]：pOutBuf 检查结果
* @return    ：1表示获取成功，0表示处理失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_chk_randnumsign(uint8* pInBuf, uint8* pOutBuf)
{
    int ret = 0;

    if(GetDAEsamUse())                  //先申请加密芯片使用权
    {
        gDAEsamSTemBuf0[0] = pInBuf[64];//取主站密钥标识
        gDAEsamSTemBuf0[1] = 0;
        gDAEsamSTemBuf0[2] = 64;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)&pInBuf[0],64);

        int len =  DAEsamCommand(DA_ESAM_RAND_SIGN_CHECK,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        if(len > 0)
        {
            memcpy((char *)&pOutBuf[0],(char *)DAEsamRcvbuf,2);

            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_chk_randnumsign
* @brief     ：检查随机数签名
* @param[in] ：pInBuf  签名缓存
* @param[out]：pOutBuf 检查结果
* @return    ：1表示获取成功，0表示处理失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_chk_toolrandnumsign(uint8* pInBuf, uint8* pOutBuf)
{
    int ret = 0;

    if(GetDAEsamUse())                  //先申请加密芯片使用权
    {
        gDAEsamSTemBuf0[0] = pInBuf[64];//取主站密钥标识
        gDAEsamSTemBuf0[1] = 0;
        gDAEsamSTemBuf0[2] = 64;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)&pInBuf[0],64);

        int len =  DAEsamCommand(DA_TOOL_RAND_SIGN_CHECK,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        if(len > 0)
        {
            memcpy((char *)&pOutBuf[0],(char *)DAEsamRcvbuf,2);

            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_get_keyversion
* @brief     ：获取秘钥版本
* @param[in] ：
* @param[out]：pOutBuf 8字节秘钥版本
* @return    ：1表示获取成功，0表示处理失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_get_keyversion(uint8* pOutBuf)
{
    int ret = 0;

    if(GetDAEsamUse())//先申请加密芯片使用权
    {
        int len =  DAEsamCommand(DA_ESAM_KEY_VER,DAEsamRcvbuf,DAEsamSndbuf,NULL,NULL);

        if(len > 0)
        {
            memcpy((char *)&pOutBuf[0],(char *)&DAEsamRcvbuf[4],1);
            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_update_key
* @brief     ：更新秘钥
* @param[in] ：pInBuf  秘钥缓存
* @param[out]：pOutBuf 更新结果
* @return    ：1表示获取成功，0表示处理失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_update_key(uint8* pInBuf, uint8* pOutBuf)
{
    int ret = 0;

    if(GetDAEsamUse())//先申请加密芯片使用权
    {
        gDAEsamSTemBuf0[0] = pInBuf[245];//取主站密钥标识
        gDAEsamSTemBuf0[1] = 0;
        gDAEsamSTemBuf0[2] = 245;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)pInBuf,245);
        int len =  DAEsamCommand(DA_ESAM_KYE_UPDATA,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        if(len > 0)
        {
            memcpy((char *)&pOutBuf[0],(char *)DAEsamRcvbuf,2);
            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_restore_key
* @brief     ：恢复秘钥
* @param[in] ：pInBuf  秘钥缓存
* @param[out]：pOutBuf 恢复结果
* @return    ：1表示获取成功，0表示处理失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_restore_key(uint8* pInBuf, uint8* pOutBuf)
{
    int ret = 0;

    if(GetDAEsamUse())//先申请加密芯片使用权
    {
        gDAEsamSTemBuf0[0] = pInBuf[245];//取主站密钥标识
        gDAEsamSTemBuf0[1] = 0;
        gDAEsamSTemBuf0[2] = 245;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)pInBuf,245);
        int len =  DAEsamCommand(DA_ESAM_KYE_REBACK,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        if(len>0)
        {
            memcpy((char *)&pOutBuf[0],(char *)DAEsamRcvbuf,2);
            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_restore_key
* @brief     ：恢复秘钥
* @param[in] ：pInBuf  秘钥缓存
* @param[out]：pOutBuf 恢复结果
* @return    ：1表示获取成功，0表示处理失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_restore_toolkey(uint8* pInBuf, uint16 inLen, uint8* pOutBuf)
{
    int ret = 0;

    if(GetDAEsamUse())//先申请加密芯片使用权
    {
        gDAEsamSTemBuf0[0] = 0;//取主站密钥标识
        gDAEsamSTemBuf0[1] = inLen >> 8;
        gDAEsamSTemBuf0[2] = inLen;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)pInBuf,inLen);
        int len =  DAEsamCommand(DA_TOOL_KEY_REBACK, DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        if(len>0)
        {
            memcpy((char *)&pOutBuf[0],(char *)DAEsamRcvbuf,2);
            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_get_publickey
* @brief     ：获取公钥
* @param[in] ：
* @param[out]：pOutBuf 加密好的公钥缓存
* @return    ：>0表示公钥长度，0表示处理失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_get_publickey(int pipe, uint8* pOutBuf)
{
    int ret = 0;

    //读取公钥
    ret = GetDAEsamUse();
    PRTL_FMT_DEBUG("security_get_publickey() 申请加密芯片使用权 ret=%d\n", ret);
    if(ret)//先申请加密芯片使用权
    {
        int len =  DAEsamCommand(DA_ESAM_READ_PUBLIC_KEY,DAEsamRcvbuf,DAEsamSndbuf,NULL,NULL);

        if(len > 0)
        {
            //加密
            int index = 1;
            int encrylen = len - 4 + 16 + 3;//-4 (9000,len), +16 (ID + R1), +3 (1应用类型, 2长度)
            gDAEsamSTemBuf0[index++] = encrylen>>8;
            gDAEsamSTemBuf0[index++] = encrylen&0xff;
            memcpy((char *)&gDAEsamSTemBuf0[index],(char *)gToolsID[pipe],8);
            index += 8;
            memcpy((char *)&gDAEsamSTemBuf0[index],(char *)gMtuRandNum[pipe],8);
            index += 8;
            gDAEsamSTemBuf0[index++] = TOOL_TYPE_MTU_KEY_RES_ACK;
            gDAEsamSTemBuf0[index++] = 0;
            gDAEsamSTemBuf0[index++] = 0x40;
            memcpy((char *)&gDAEsamSTemBuf0[index],(char *)&DAEsamRcvbuf[4],len-4);

            memset(DAEsamRcvbuf, 0, sizeof(DAEsamRcvbuf));
            usleep(10000);

            len = DAEsamCommand(DA_TOOL_ENCRYPTE,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,gMtuRandNum[pipe]);

            if(len>0)
            {
                memcpy((char *)&pOutBuf[0],(char *)&DAEsamRcvbuf[4],len-4);
                ret = len - 4;
            }
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_get_mtutestcert
* @brief     ：获取终端测试证书
* @param[in] ：
* @param[out]：pOutBuf 证书缓存
* @return    ：>0证书长度，0表示处理失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_get_mtutestcert(uint8* pOutBuf)
{
    int ret = 0;

    //读取终端初始证书长度
    if(GetDAEsamUse())//先申请加密芯片使用权
    {
        int len = DAEsamCommand(DA_ESAM_READ_MTU_CERTLEN,DAEsamRcvbuf,DAEsamSndbuf,NULL,NULL);

        if(len > 0)
        {//读取终端初始证书
            usleep(10000);
            gDAEsamSTemBuf0[1] = DAEsamRcvbuf[4];//证书长度高字节
            gDAEsamSTemBuf0[2] = DAEsamRcvbuf[5];//证书长度低字节
            memset(DAEsamRcvbuf, 0, sizeof(DAEsamRcvbuf));
            len = DAEsamCommand(DA_ESAM_READ_MTU_CERT,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

            if(len > 0)
            {//证书回写
                int index = 2;
                gDAEsamSTemBuf0[1] = DAEsamRcvbuf[index++];//证书长度高字节
                gDAEsamSTemBuf0[2] = DAEsamRcvbuf[index++];//证书长度低字节
                int cerlen = (gDAEsamSTemBuf0[1]<<8) + gDAEsamSTemBuf0[2];
                memcpy((char *)&pOutBuf[0],(char *)&DAEsamRcvbuf[index],cerlen);
                ret = cerlen;
            }
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_get_mtucert
* @brief     ：获取终端证书
* @param[in] ：cerID 证书标识  cerbuf 证书缓存  cerlen证书长度
* @param[out]：pOutBuf 证书缓存
* @return    ：>0证书长度，-1表示处理失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_get_mtucert(uint8* pOutBuf)
{
    int ret = 0;

    if(GetDAEsamUse())//先申请加密芯片使用权
    {
        int len =  DAEsamCommand(DA_ESAM_MTU_CERT_GET,DAEsamRcvbuf,DAEsamSndbuf,NULL,NULL);

        if(len > 0)
        {
            int cerlen = (DAEsamRcvbuf[2]<<8) + DAEsamRcvbuf[3];
            memcpy((char *)pOutBuf,(char *)&DAEsamRcvbuf[4],cerlen);
            ret = cerlen;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_update_hostcert
* @brief     ：更新主机证书
* @param[in] ：cerID 证书标识  cerbuf 证书缓存  cerlen证书长度
* @param[out]：pOutBuf 更新结果
* @return    ：1表示获取成功，0表示处理失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_update_hostcert(uint8 cerID, uint8* cerbuf, uint16 cerlen, uint8* pOutBuf)
{
    int ret = 0;

    //证书更新
    if(GetDAEsamUse())//先申请加密芯片使用权
    {
        gDAEsamSTemBuf0[0] = cerID;
        gDAEsamSTemBuf0[1] = cerlen>>8;
        gDAEsamSTemBuf0[2] = cerlen&0xff;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)cerbuf,cerlen);
        int len = DAEsamCommand(DA_ESAM_HOST_CERT_UPDATA,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        if(len>0)//更新成功
        {
            memcpy((char *)&pOutBuf[0],(char *)DAEsamRcvbuf,2);
            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_update_mtucert
* @brief     ：更新终端证书
* @param[in] ：cerID 证书标识  cerbuf 证书缓存  cerlen证书长度
* @param[out]：pOutBuf 更新结果
* @return    ：1表示获取成功，0表示处理失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_update_mtucert(uint8 cerID, uint8* cerbuf, uint16 cerlen, uint8* pOutBuf)
{
    int ret = 0;

    //证书更新
    if(GetDAEsamUse())//先申请加密芯片使用权
    {
        gDAEsamSTemBuf0[0] = cerID;
        gDAEsamSTemBuf0[1] = cerlen>>8;
        gDAEsamSTemBuf0[2] = cerlen&0xff;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)cerbuf,cerlen);
        int len = DAEsamCommand(DA_ESAM_MTU_CERT_UPDATA,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        if(len>0)//更新成功
        {
            memcpy((char *)&pOutBuf[0],(char *)DAEsamRcvbuf,2);
            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_decryte_tool
* @brief     ：解密认证报文
* @param[in] ：pInBuf 原始报文缓存   inLen 原始报文长度
* @param[out]：pOutBuf 解密结果缓存
* @return    ：>0解密结果长度，-1表示处理失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_decryte_tool(int pipe, uint8* pInBuf, int inLen, uint8* pOutBuf)
{
    int ret = 0;

    //解密
    if(GetDAEsamUse())//先申请加密芯片使用权
    {
        int    deciplen = inLen;//解密长度
        uint16 datalen  = deciplen + 16;//toolid 8字节+ R1 8字节

        gDAEsamSTemBuf0[1] = datalen>>8;
        gDAEsamSTemBuf0[2] = datalen&0xff;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)gToolsID[pipe],8);
        memcpy((char *)&gDAEsamSTemBuf0[11],(char *)gMtuRandNum[pipe],8);
        memcpy((char *)&gDAEsamSTemBuf0[19],(char *)&pInBuf[0],deciplen);
        int len =  DAEsamCommand(DA_TOOL_DECIPHER,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,gMtuRandNum[pipe]);

        if(len > 0)
        {
            //解密成功，下面是加密芯片返回数据格式
            //9000(2字节)+Le(2字节)+ReqData
            int cerlen = len - 4;//含义见上面描述
            int index  = 4;

            memcpy((char *)&pOutBuf[0],(char *)&DAEsamRcvbuf[index],cerlen);
            ret = cerlen;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_get_authsign
* @brief     ：认证信息签名
* @param[in] ：pInBuf 原始报文缓存   inLen 原始报文长度
* @param[out]：pOutBuf 签名结果缓存
* @return    ：>0签名结果长度，-1表示处理失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_get_infosign(uint8* pInBuf, int inLen, uint8* pOutBuf)
{
    int ret = 0;
    int len = 0;

    if(GetDAEsamUse())//先申请加密芯片使用权
    {
        gDAEsamSTemBuf0[1] = inLen >> 8;
        gDAEsamSTemBuf0[2] = inLen & 0xff;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)pInBuf,inLen);

        len = DAEsamCommand(DA_ESAM_INFO_SIGN,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        //发送给主站
        if(len > 0)
        {
            memcpy((char *)&pOutBuf[0],(char *)&DAEsamRcvbuf[4],len - 4);

            ret = len - 4;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_chk_infosign
* @brief     ：验证信息签名，签名不包含应用数据域长度和安全扩展区长度
* @param[in] ：pInBuf 原始报文缓存   inLen 原始报文长度
* @param[out]：pOutBuf 签名结果缓存
* @return    ：1 验证成功 0验证失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_chk_infosign(uint8* pInBuf, int inLen, uint8* pOutBuf, BOOL appFrame)
{
    //获取验签数据及数据长度
    int ret = 0;
    int len = 0;
    int extlen = 0;

    if(GetDAEsamUse())//先申请加密芯片使用权
    {
        int index1 = 0, index2 = 0;

        if (appFrame)
        {
            len = pInBuf[index2++];//数据域长度
            //去掉应用数据区长度,保存104原始报文
            memcpy((char *)&gDAEsamDecBuf[index1+3],(char *)&pInBuf[index2],len);
            index2 += len;
            index1 += len;

            //是否有安全扩展区
            if (inLen > pInBuf[0] + 4) //数据域长度+1数据域长度位+2安全扩展区长度+1 asKID
            {
                //获取扩展区长度
                extlen = pInBuf[index2++] << 8;
                extlen += pInBuf[index2++];
                //长度有效
                if (index2 + extlen > inLen)
                {
                    PRTL_FMT_DEBUG("SECURITY验证104签名失败,扩展数据区长度%u错误\n", extlen);

                    return ret;
                }
            }
            else {
                PRTL_FMT_DEBUG("SECURITY验证104签名失败,无扩展数据区\n");

                return ret;
            }
        }
        else {
            extlen = inLen;
        }

        //是否有安全扩展区
        if (extlen > 1) //数据域长度+1数据域长度位+2安全扩展区长度+1 asKID
        {
            extlen--;//减去asKID
            //保存扩展区数据
            memcpy((char *)&gDAEsamDecBuf[index1+3],(char *)&pInBuf[index2],extlen);
            index1 += extlen;
            index2 += extlen;
            gDAEsamDecBuf[0] = pInBuf[index2]; //asKID
            gDAEsamDecBuf[1] = index1 >> 8;
            gDAEsamDecBuf[2] = index1;

            len = DAEsamCommand(DA_ESAM_INFO_SIGN_CHECK,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamDecBuf,NULL);

            if(len > 0)
            {
                if (pOutBuf != NULL)
                    memcpy(pOutBuf, &DAEsamRcvbuf[4], 2);

                ret = 1;
            }
            else {
                PRTL_FMT_DEBUG("SECURITY验证104签名失败\n");
            }
        }
        else {
            PRTL_FMT_DEBUG("SECURITY验证104签名失败,扩展数据区长度%u错误\n", extlen);
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_chk_cert
* @brief     ：验证证书
* @param[in] ：certId 证书标识 pInBuf 原始报文缓存   inLen 原始报文长度
* @param[out]：pOutBuf 验证结果缓存
* @return    ：1 验证成功 0验证失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_chk_cert(uint8* pInBuf, int inLen, uint8* pOutBuf)
{
    //获取验签数据及数据长度
    int ret = 0;
    int len = 0;

    if(GetDAEsamUse())//先申请加密芯片使用权
    {
        gDAEsamDecBuf[1] = inLen>>8;
        gDAEsamDecBuf[2] = inLen&0xff;

        memcpy((char *)&gDAEsamDecBuf[3],(char *)pInBuf,inLen);
        len = DAEsamCommand(DA_TOO_CERT_CHECK,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamDecBuf,NULL);

        if(len > 0)
        {
            if (pOutBuf != NULL)
                memcpy(pOutBuf, &DAEsamRcvbuf[4], 2);

            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_encryte
* @brief     ：加密处理，根据应用类型添加
* @param[in] ：len ，数据源长度  pinbuf,数据源存放缓冲区  bType 数据源应用类型
* @param[out]：*poutbuf，加密数据存放缓冲区
* @return    ：组帧返回的数据长度，-1表示处理失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_encryte(int pipe, uint16 len,uint8 *pinbuf,uint8 *poutbuf,uint8 bType)
{
    int    len1     = 0;
    uint16 index     = 0;
    uint16 encrtlen = 0;
    int    rtnlen     = -1;
    BOOL   addRandNum = FALSE;

    switch(bType)
    {
        case APP_TYPE_CTRL_SELECT_ACK://需要添加随机数
        {
            if (security_get_randnum(gMtuRandNum[pipe]))
            {
                addRandNum = TRUE;
            }
            else
            {
                return -1;
            }
            break;
        }
        case APP_TYPE_NOMMAL:
        default:
        {
            break;
        }
    }

    if(GetDAEsamUse())//先申请加密芯片使用权
    {
        //明文数据
        index = 3;
        gDAEsamSTemBuf1[index++] = bType;
        gDAEsamSTemBuf1[index++] = len;
        memcpy((char *)&gDAEsamSTemBuf1[index],(char *)pinbuf,len);
        index += len;
        gDAEsamSTemBuf1[index++] = 0;//扩展区数据长度高字节
        if (addRandNum) {
            gDAEsamSTemBuf1[index++] = 8;//扩展区数据长度低字节
            memcpy((char *)&gDAEsamSTemBuf1[index],(char *)gMtuRandNum[pipe],8);
            index += 8;
        }
        else
            gDAEsamSTemBuf0[index++] = 0;        //扩展区数据长度低字节

        encrtlen = index - 3;

        //对数据进行加密处理
        gDAEsamSTemBuf1[0] = 0;
        gDAEsamSTemBuf1[1] = encrtlen>>8;
        gDAEsamSTemBuf1[2] = encrtlen&0xff;

        memcpy((char *)&gDAEsamSTemBuf1[3],(char *)gDAEsamSTemBuf0,encrtlen);
        len1 =  DAEsamCommand(DA_ESAM_ENCRYPTE,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf1,gHostRandNum[pipe]);

        if(len1 > 0)
        {
            rtnlen = len1 -4;
            memcpy((char *)poutbuf,(char *)&DAEsamRcvbuf[4],rtnlen);
        }
        else
        {
            rtnlen = -1;
        }

        ReleaseADEsam();
    }
    else
        return -1;

    return rtnlen;
}

/**********************************************************************
* @name      : security_decryte
* @brief     ：加密处理，根据应用类型添加
* @param[in] ：len ，数据源长度  pinbuf,数据源存放缓冲区
* @param[out]：*poutbuf，解密之后的帧缓存
* @return    ：组帧返回的数据长度，-1表示处理失败
* @Create    : 陈德昶
* @Date      ：2020-03-03
* @Update    :

**********************************************************************/
int security_decryte(int pipe, uint8* pInBuf,uint16 bLen,uint8 *pOutBuf)
{
    int   ret = 0;
    int   len = 0;

    //解密
    if(GetDAEsamUse())//先申请加密芯片使用权
    {
        gDAEsamSTemBuf0[0] = 0;
        gDAEsamSTemBuf0[1] = bLen>>8;
        gDAEsamSTemBuf0[2] = bLen&0xff;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)pInBuf,bLen);
        len =    DAEsamCommand(DA_ESAM_DECIPHER,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,gHostRandNum[pipe]);

        if(len > 4)//解密成功
        {
            len = (DAEsamRcvbuf[2]<<8) + DAEsamRcvbuf[3];
            memcpy((char *)&pOutBuf[0],(char *)&DAEsamRcvbuf[4],len);//将接收缓冲区还原为明文

            ret = len;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_recv_certdata
* @brief     ：接收证书数据，完成组帧操作
* @param[in] ：pip通道号 pAppPdu数据缓存 appLen数据长度
* @param[out]：pCertId证书标识
* @return    ：-1表示处理失败 0证书未接收完成               >0证书长度
* @Create    : 陈德昶
* @Date      ：2020-03-09
* @Update    :

**********************************************************************/
int security_recv_certdata(int pipe, uint8 appType, uint8* pAppPdu, int appLen, uint8* pCertId)
{
    int   index = 0;
    int   cerlen = 0;
    uint8 bCertCode = 0;    //证书标识
    uint8 bTotalNo;         //总帧数
    uint8 bCurrenNo;        //当前帧号

    if (appType != TOOL_TYPE_CERT_FILE_RES)
        bCertCode = pAppPdu[index++];

    bTotalNo    = pAppPdu[index++];
    bCurrenNo   = pAppPdu[index++];

    PRTL_FMT_DEBUG("security_recv_certdata() pipe:%u, 证书标识:%u, 总帧数:%u, 当前帧号:%u\n", pipe, bCertCode, bTotalNo, bCurrenNo);

    if((bfgCertCodeDown[pipe] == 0)&&(bCurrenNo == 1))//首帧下发
    {
        gCertCode[pipe]   = bCertCode;  //证书标识
        bfgCertCodeDown[pipe] = 1;
    }

    if(gCertCode[pipe] == bCertCode)
    {
        cerlen  = appLen - 3;//证书标识1字节，总帧数1字节，当前帧序号1字节

        if (appType == TOOL_TYPE_CHECK_RES_START)
            cerlen -= 8;//证书管理工具ID8字节

        int dCertBufIndx = gdCertBufIndx[pipe][bCurrenNo-1];
        if((dCertBufIndx + cerlen ) < DA_ESAM_MAX_LEN)
        {
            //保存证书更新密文包
            memcpy((char *)&gDAEsamCertBuf[pipe][dCertBufIndx],(char *)&pAppPdu[index],cerlen);

            if (appType == TOOL_TYPE_CHECK_RES_START)
                memcpy((char *)gToolsID[pipe],(char *)&pAppPdu[index + cerlen],8);//保存管理证书ID

            if(bCurrenNo<129)
            {
                gdCertBufIndx[pipe][bCurrenNo] = dCertBufIndx + cerlen;//保存下一帧的起始存放位置

                if(bTotalNo != bCurrenNo)//当前帧并非最后一帧
                {
                    return 0;
                }

                *pCertId = bCertCode;

                //最后一帧,证书接收完整,解密&验签
                //获取证书更新密文包长度
                return gdCertBufIndx[pipe][bCurrenNo];
            }
            else
                PRTL_FMT_DEBUG("SECURITY证书序号太大,pipe:%u,序号:%u\n", pipe, bCurrenNo);
        }
        else
            PRTL_FMT_DEBUG("SECURITY证书太大,pipe:%u,序号:%u,已存长度:%u,本帧长度:%u\n",
                           pipe, bCurrenNo, dCertBufIndx, cerlen);
    }
    else
        PRTL_FMT_DEBUG("SECURITY证书号异常,pipe:%u,序号:%u,前帧:%u,本帧:%u\n",
                       pipe, bCurrenNo, gCertCode[pipe], bCertCode);

    return -1;
}

/**********************************************************************
* @name      : security_handle_certdata
* @brief     ：下装/更新证书报文处理
* @param[in] ：pip通道号 pAppPdu数据缓存 appLen数据长度
* @param[out]：pOutBuf返回报文缓存
* @return    ：返回报文长度
* @Create    : 陈德昶
* @Date      ：2020-03-09
* @Update    :

**********************************************************************/
int security_handle_certdata(int pipe, DASAFE_FRAME_HEAD* head, uint8* pAppPdu, int appLen, uint8* pOutBuf)
{
    int   len = 0;
    int   index = 0;
    int   cerlen = 0;
    uint8 bCenrCode = 0;    //证书标识

    cerlen = security_recv_certdata(pipe, head->apptype, pAppPdu, appLen, &bCenrCode);

    if (cerlen == 0)
        return 0;

    if (cerlen > 0)
    {
        if (head->apptype == APP_TYPE_CERT_CHANGE_RES)
        {
            //证书更新密文包解密
            cerlen = security_decryte(pipe, gDAEsamCertBuf[pipe], cerlen, gDAEsamCertBuf[pipe]);

            if (cerlen > 0)
            {
                usleep(10000);
                if (security_chk_infosign(gDAEsamCertBuf[pipe], cerlen, NULL, FALSE) == 0)
                {
                    cerlen = 0;
                }
            }
            else
                PRTL_FMT_DEBUG("SECURITY证书解密失败,pipe:%u,应用类型:%02X,证书号:%u,证书长度:%u\n",
                               pipe, head->apptype, bCenrCode, cerlen);
        }

        if (cerlen > 72)
        {
            index = 6;

            if (head->apptype == APP_TYPE_CERT_CHANGE_RES)
                pOutBuf[index++] = APP_TYPE_CERT_CHANGE_ACK; //0x71终端返回证书更新结果
            else if (head->apptype == APP_TYPE_CERT_DOWN_RES)
                pOutBuf[index++] = APP_TYPE_CERT_DOWN_ACK;
            pOutBuf[index++] = 0; //两字节安全扩展信息长度
            pOutBuf[index++] = 2;

            if (head->apptype == APP_TYPE_CERT_CHANGE_RES)
                len = security_update_hostcert(bCenrCode, gDAEsamCertBuf[pipe]+1, cerlen-72, &pOutBuf[index]);
            else if (head->apptype == APP_TYPE_CERT_DOWN_RES)
                len = security_update_mtucert(bCenrCode, gDAEsamCertBuf[pipe]+1, cerlen-72, &pOutBuf[index]);

            if (len > 0)
            {
                index += 2;
                bfgCertCodeDown[pipe] = 0;
                gCertCode[pipe] = 0xff;
                memset((char *)gdCertBufIndx[pipe], 0, 129 * 2);

                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }
            else
                PRTL_FMT_DEBUG("SECURITY证书更新失败,pipe:%u,应用类型:%02X,证书号:%u,证书长度:%u\n",
                               pipe, head->apptype, bCenrCode, cerlen);
        }
    }

    bfgCertCodeDown[pipe] = 0;
    gCertCode[pipe] = 0xff;
    memset((char *)gdCertBufIndx[pipe], 0, 129 * 2);

    return MakeErrFram(pOutBuf,DASAFE_ERR_CERT_DOWN,head->commtype);
}

/**********************************************************************
* @name      : security_handle_toolauth
* @brief     ：证书管理工具认证请求处理
* @param[in] ：pip通道号 pAppPdu数据缓存 appLen数据长度
* @param[out]：pOutBuf返回报文缓存
* @return    ：返回报文长度
* @Create    : 陈德昶
* @Date      ：2020-03-09
* @Update    :

**********************************************************************/
int security_handle_toolauth(int pipe, DASAFE_FRAME_HEAD* head, uint8* pAppPdu, int appLen, uint8* pOutBuf)
{
    //流程:首先接收，收到完整报文后，验证，验证成功后，读取随机数，发给工具
    int index = 0;
    int cerlen = 0;
    uint8 bCenrCode = 0;    //证书标识

    cerlen = security_recv_certdata(pipe, head->apptype, pAppPdu, appLen, &bCenrCode);

    PRTL_FMT_DEBUG("security_handle_toolauth() cerlen=%d \n", cerlen);
    if (cerlen > 0)
    {
        if (security_chk_cert(gDAEsamCertBuf[pipe], cerlen, NULL))
        {
            if (security_get_randnum(gMtuRandNum[pipe]))
            {
                index = 6;
                pOutBuf[index++] = TOOL_TYPE_CHECK_RES_START_ACK;
                pOutBuf[index++] = 0; //两字节安全扩展信息长度
                pOutBuf[index++] = 8;
                memcpy((char *)&pOutBuf[index],(char *)gMtuRandNum[pipe],8);
                index += 8;

                bfgCertCodeDown[pipe] = 0;
                gCertCode[pipe] = 0xff;
                memset((char *)gdCertBufIndx[pipe], 0, 129 * 2);

                DAMakeFrameHead(pOutBuf, index-4, head->commtype);

                return DAAddFrameTail(pOutBuf, index-4);
            }
            else
                PRTL_FMT_DEBUG("SECURITY获取随机数失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);
        }
        else
            PRTL_FMT_DEBUG("SECURITY证书验证失败,pipe:%u,应用类型:%02X,证书号:%u,证书长度:%u\n",
                           pipe, head->apptype, bCenrCode, cerlen);
    }
    else 
    {
        //分帧响应
        PRTL_FMT_DEBUG("security_handle_toolauth() 分帧响应 \n");
        index = 6;
        pOutBuf[index++] = TOOL_TYPE_KEY_CERT_SEND_ACK;
        pOutBuf[index++] = 0; //两字节安全扩展信息长度
        pOutBuf[index++] = 2;
        pOutBuf[index++] = 0x90;
        pOutBuf[index++] = 0;
        DAMakeFrameHead(pOutBuf, index-4, head->commtype);

        return DAAddFrameTail(pOutBuf, index-4);
    }

    bfgCertCodeDown[pipe] = 0;
    gCertCode[pipe] = 0xff;
    memset((char *)gdCertBufIndx[pipe], 0, 129 * 2);

    return MakeErrFram(pOutBuf,DASAFE_ERR_CERT_DOWN,head->commtype);
}

/**********************************************************************
* @name      : security_handle_appframe
* @brief     ：G101/104应用类型帧处理
* @param[in] ：pip通道号 pAppPdu数据缓存 appLen数据长度 bType G101/104帧类型
* @param[out]：pOutBuf返回报文缓存
* @return    ：返回报文长度
* @Create    : 陈德昶
* @Date      ：2020-03-09
* @Update    :

**********************************************************************/
int security_handle_appframe(int pipe, DASAFE_FRAME_HEAD* head, uint8* pAppPdu, int appLen, uint8* pOutBuf)
{
    int   len;
    uint8 appType = 0;
    uint8 signtype;
    uint8 app104buf[255] = {0};

    if(bfgDAEsamRunEn[pipe] == 0)//业务流程非法
    {
        PRTL_FMT_DEBUG("SECURITY业务流程非法(还没有认证),pipe:%u,应用类型:%02X\n", pipe, head->apptype);

        return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
    }

    if (pAppPdu[0] < 4)
    {
        PRTL_FMT_DEBUG("SECURITY应用数据区长度%u错误,pipe:%u,应用类型:%02X\n", pAppPdu[0], pipe, head->apptype);

        return MakeErrFram(pOutBuf,BUSS_ERR_MESS_UNPACK,head->commtype);
    }

    if(SignNeedCheck(head->apptype, &signtype))//判断是否需要验签
    {    //开始验签，签名不包含应用类型及长度
        len = security_chk_infosign(pAppPdu, appLen, NULL, TRUE);

        if(len <= 0)
        {//验签失败
            PRTL_FMT_DEBUG("SECURITY签名验证失败,pipe:%u,应用类型:%02X,长度:%u\n", pipe, head->apptype, pAppPdu[0]);
            return MakeErrFram(pOutBuf,BUSS_ERR_MESS_CHACK,head->commtype);
        }
        //验签成功执行后续逻辑
    }

    //调用104解帧
    len = frame_data_unpack(pAppPdu[0], &pAppPdu[1], app104buf, &appType);
    if (len <= 0) {
        int i = 0;
        PRTL_FMT_DEBUG("SECURITY 104解析未回码,pipe:%u,应用类型:%02X,报文:\n", pipe, head->apptype);
        for (; i < pAppPdu[0]; i++)
        {
            printf("%02X ", pAppPdu[i+1]);
        }
        printf("\n");

        return 0;
    }

    return security_packappframe(pipe, appType, app104buf, len, pOutBuf);
}

/**********************************************************************
* @name      : security_handle_gwframe
* @brief     ：与网关通信应用类型帧处理
* @param[in] ：pip通道号 pAppPdu数据缓存 appLen数据长度
* @param[out]：pOutBuf返回报文缓存
* @return    ：返回报文长度
* @Create    : 陈德昶
* @Date      ：2020-03-09
* @Update    :

**********************************************************************/
int security_handle_gwframe(int pipe, DASAFE_FRAME_HEAD* head, uint8 * pAppPdu, int appLen, uint8* pOutBuf)
{
    int index = 0;

    switch(head->apptype)
    {
        case APP_TYPE_NET_RAND://0x20 网关发送随机数，等待终端签名
        {
            int index1 = 6;
            index += 2;//扩展安全区长度2字节
            //保存网关随机数
            memcpy((char *)gNetRandNum[pipe],(char *)&pAppPdu[index],8);//随机数8字节
            pOutBuf[index1++] = APP_TYPE_MTU_AUTH;//应用类型0x21 终端向网关认证申请
            pOutBuf[index1++] = 0; //两字节安全扩展信息长度，高字节长度为0
            pOutBuf[index1++] = 73;//8字节终端随机数+ 64字节主站随机数签名+ 1字节终端密钥标识
            //保存主站随机数
            memcpy((char *)gHostRandNum[pipe],(char *)&pAppPdu[index],8);//随机数8字节

            if (security_get_randnumsign(&pAppPdu[index], &pOutBuf[index1])) {
                index1 += 72;
                pOutBuf[index1++] = 0x06;//密钥标识固定为6
                DAMakeFrameHead(pOutBuf, index1-4, head->commtype);

                return DAAddFrameTail(pOutBuf, index1-4);
            }

            PRTL_FMT_DEBUG("SECURITY获取随机数签名失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case APP_TYPE_NET_AUTH://0x22网关返回终端的认证结果，并向终端申请认证
        {
            int index1 = 6;
            index += 2;//扩展安全区长度2字节
            pOutBuf[index1++] = APP_TYPE_AUTH_ACK;///0x23认证结果返回
            pOutBuf[index1++] = 0; //两字节安全扩展信息长度
            pOutBuf[index1++] = 2;

            if (security_chk_randnumsign(&pAppPdu[index], &pOutBuf[index1]))
            {
                bfgDAEsamRunEn[pipe] = 2;
                index1 += 2;
                DAMakeFrameHead(pOutBuf, index1-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index1-4);
            }

            PRTL_FMT_DEBUG("SECURITY获取随机数签名失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_RAND_CHECK,head->commtype);
        }
        case APP_TYPE_NET_SERI_RES: //0x24网关申请终端及芯片序列号
        {
            uint8 serials[10] = {0};

            if (security_get_serialnum(serials))
            {
                index  = 6;
                pOutBuf[index++] = APP_TYPE_NET_SERI_ACK;  //0x25终端返回终端序列号及加密芯片序列号
                pOutBuf[index++] = 0; //两字节安全扩展信息长度
                pOutBuf[index++] = 32;
                //终端序列号(ID)=== 3字节终端类型 + 6字节厂商代码+ 3字节生产批号+ 8字节生产日期+ 4字节流水号
                pOutBuf[index++] = 'R';//终端型号R13
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '3';
                pOutBuf[index++] = 'X';//厂商代码XXXXXX
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = '0';//生产批号001
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '2';//生产日期20200626
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '9';
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '6';
                pOutBuf[index++] = '2';
                pOutBuf[index++] = '6';
                pOutBuf[index++] = '0';//流水号，取加密芯片序列号
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '0';
                pOutBuf[index++] = serials[7];
                memcpy((char *)&pOutBuf[index],(char *)&serials,8);
                index += 8;
                DAMakeFrameHead(pOutBuf, index - 4, head->commtype);

                return DAAddFrameTail(pOutBuf, index - 4);
            }

            PRTL_FMT_DEBUG("SECURITY获取序列号失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        default:
            return 0;
    }
}

/**********************************************************************
* @name      : security_handle_gwframe
* @brief     ：与主站通信应用类型帧处理
* @param[in] ：pip通道号 pAppPdu数据缓存 appLen数据长度
* @param[out]：pOutBuf返回报文缓存
* @return    ：返回报文长度
* @Create    : 陈德昶
* @Date      ：2020-03-09
* @Update    :

**********************************************************************/
int security_handle_hostframe(int pipe, DASAFE_FRAME_HEAD* head, uint8 * pAppPdu, int appLen, uint8* pOutBuf)
{
    int index = 0;

    switch(head->apptype)
    {
        //主站与终端双向认证应用类型
        case APP_TYPE_HOST_RAND:    //0x50主站发送随机数
        {
            //保存主站随机数
            memcpy((char *)gHostRandNum[pipe],(char *)&pAppPdu[index],8);//随机数8字节
            //将签名结果+ 密钥标识1字节 (0x06) 发送给主站
            index  = 6;
            pOutBuf[index++] = APP_TYPE_CLIENT_AUTH;//应用类型0x51 终端向主站认证申请
            pOutBuf[index++] = 0; //两字节安全扩展信息长度
            pOutBuf[index++] = 73;

            if (security_get_randnumsign(gHostRandNum[pipe], &pOutBuf[index]))
            {
                index += 72;
                pOutBuf[index++] = 0x06;//密钥标识固定为6
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY获取随机数签名失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case APP_TYPE_HOST_AUTH:    //0x52主站返回终端的认证结果，并向终端申请认证
        {
            uint8 result[2] = {0};

            if (security_chk_randnumsign(&pAppPdu[index], result))
            {
                bfgDAEsamRunEn[pipe] = 1;
                index  = 6;
                pOutBuf[index++] = APP_TYPE_HOST_ACK;///0x53认证结果返回
                pOutBuf[index++] = 0; //两字节安全扩展信息长度
                pOutBuf[index++] = 2;
                memcpy((char *)&pOutBuf[index],(char *)result,2);
                index += 2;

                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY获取随机数签名失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }

        case APP_TYPE_HOST_SERI_RES://0x54主站申请终端加密芯片序列号
        {
            index  = 6;
            pOutBuf[index++] = APP_TYPE_HOST_SERI_ACK; //0x55终端返回终端序列号及加密芯片序列号
            pOutBuf[index++] = 0; //两字节安全扩展信息长度
            pOutBuf[index++] = 8;

            if (security_get_serialnum(&pOutBuf[index])) {
                index += 8;
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY获取序列号失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case APP_TYPE_HOST_CODE_RES://0x56主站申请终端特征码
        {
            uint8 serials[10] = {0};

            if (security_get_serialnum(serials))
            {
                index  = 6;
                pOutBuf[index++] = APP_TYPE_HOST_CODE_ACK; //0x55终端返回终端序列号及加密芯片序列号
                pOutBuf[index++] = 0; //两字节安全扩展信息长度
                pOutBuf[index++] = 32;
                //终端序列号(ID)=== 3字节终端类型 + 6字节厂商代码+ 3字节生产批号+ 8字节生产日期+ 4字节流水号
                pOutBuf[index++] = 'R';//终端型号R13
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '3';
                pOutBuf[index++] = 'X';//厂商代码XXXXXX
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = '0';//生产批号001
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '2';//生产日期20200626
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '9';
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '6';
                pOutBuf[index++] = '2';
                pOutBuf[index++] = '6';
                pOutBuf[index++] = '0';//流水号，取加密芯片序列号
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '0';
                pOutBuf[index++] = serials[7];
                memcpy((char *)&pOutBuf[index],(char *)&serials,8);
                index += 8;
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY获取序列号失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }

        case APP_TYPE_MANA_CODE_RES://0x58主站通过管理通道申请终端特征码
        {
            uint8 serials[10] = {0};

            if (security_get_serialnum(serials))
            {
                index  = 6;
                pOutBuf[index++] = APP_TYPE_MANA_CODE_ACK; //0x55终端返回终端序列号及加密芯片序列号
                pOutBuf[index++] = 0; //两字节安全扩展信息长度
                pOutBuf[index++] = 60;
                //终端序列号(ID)=== 3字节终端类型 + 6字节厂商代码+ 3字节生产批号+ 8字节生产日期+ 4字节流水号
                pOutBuf[index++] = 'R';//终端型号R13
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '3';
                pOutBuf[index++] = 'X';//厂商代码XXXXXX
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = '0';//生产批号001
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '2';//生产日期20200626
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '9';
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '6';
                pOutBuf[index++] = '2';
                pOutBuf[index++] = '6';
                pOutBuf[index++] = '0';//流水号，取加密芯片序列号
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '0';
                pOutBuf[index++] = serials[7];
                memset((char *)&pOutBuf[index],0x30,28);//华为ESN
                index += 28;
                memcpy((char *)&pOutBuf[index],(char *)serials,8);
                index += 8;

                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY获取序列号失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        //主站与终端密钥管理应用类型
        case APP_TYPE_KEY_VER_RES:    //0x60主站向终端申请密钥版本
        {
            index  = 6;
            pOutBuf[index++] = APP_TYPE_KEY_VER_ACK; //0x61终端返回密钥版本
            pOutBuf[index++] = 0; //两字节安全扩展信息长度
            pOutBuf[index++] = 9;

            if (security_get_keyversion(&pOutBuf[index++]))
            {
                if (security_get_randnum(&pOutBuf[index]))
                {
                    memcpy(gMtuRandNum[pipe], &pOutBuf[index], 8);
                    index += 8;
                    DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                    return DAAddFrameTail(pOutBuf, index-4);
                }
            }

            PRTL_FMT_DEBUG("SECURITY获取秘钥版本失败,pipe:%u\n", pipe);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case APP_TYPE_KEY_CHANGE_RES://0x62主站发起密钥更新
        {
            if(bfgDAEsamRunEn[pipe] == 0)//业务流程非法
            {
                PRTL_FMT_DEBUG("SECURITY业务流程非法(还没有认证),pipe:%u,应用类型:%02X\n", pipe, head->apptype);

                return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
            }

            uint8 result[2] = {0};

            if (security_update_key(&pAppPdu[index], result))
            {
                index  = 6;
                pOutBuf[index++] = APP_TYPE_KEY_CHANGE_ACK; //0x63//终端返回密钥更新结果
                pOutBuf[index++] = 0; //两字节安全扩展信息长度
                pOutBuf[index++] = 2;
                memcpy((char *)&pOutBuf[index],(char *)result,2);
                index += 2;
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY更新秘钥失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,DASAFE_ERR_KEY_CHANGE,head->commtype);
        }
        case APP_TYPE_KEY_REBACK_RES://0x64主站发起密钥恢复
        {
            if(bfgDAEsamRunEn[pipe] == 0)//业务流程非法
            {
                PRTL_FMT_DEBUG("SECURITY业务流程非法(还没有认证),pipe:%u,应用类型:%02X\n", pipe, head->apptype);

                return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
            }

            uint8 result[2] = {0};

            if (security_restore_key(&pAppPdu[index], result))
            {
                index  = 6;
                pOutBuf[index++] = APP_TYPE_KEY_REBACK_ACK; //0x63//终端返回密钥更新结果
                pOutBuf[index++] = 0; //两字节安全扩展信息长度
                pOutBuf[index++] = 2;
                memcpy((char *)&pOutBuf[index],(char *)result,2);
                index += 2;
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY恢复秘钥失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,DASAFE_ERR_KEY_REBACK,head->commtype);
        }
        //主站与终端证书管理应用类型
        case APP_TYPE_CERT_CHANGE_RES://0x70主站发起证书更新
        case APP_TYPE_CERT_DOWN_RES:  //0x72主站发起证书下载
        {
            if(bfgDAEsamRunEn[pipe] == 0)//业务流程非法
            {
                PRTL_FMT_DEBUG("SECURITY业务流程非法(还没有认证),pipe:%u,应用类型:%02X\n", pipe, head->apptype);

                return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
            }

            return security_handle_certdata(pipe, head, &pAppPdu[index], appLen, pOutBuf);
        }
        case APP_TYPE_CERT_GET_RES:   //0x74主站获取终端证书
        {
            if(bfgDAEsamRunEn[pipe] == 0)//业务流程非法
            {
                PRTL_FMT_DEBUG("SECURITY业务流程非法(还没有认证),pipe:%u,应用类型:%02X\n", pipe, head->apptype);

                return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
            }

            int cerlen = security_get_mtucert(gDAEsamCertBuf[pipe]);

            if (cerlen > 0)
            {
                bfgMtuCertSendEn[pipe][0] = APP_TYPE_CERT_GET_ACK;//等待主逻辑上报，分帧不用等应答
                bfgMtuCertSendEn[pipe][1] = cerlen>>8;
                bfgMtuCertSendEn[pipe][2] = cerlen&0xff;
                bfgMtuCertSendEn[pipe][3] = head->commtype.commtype[0];
                bfgMtuCertSendEn[pipe][4] = head->commtype.commtype[1];
                return 0;
            }

            PRTL_FMT_DEBUG("SECURITY获取终端证书失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case APP_TYPE_CERT_GET_OVER:
        {
            PRTL_FMT_DEBUG("SECURITY收到证书提取结果%02X%02X,pipe:%u\n", pAppPdu[0], pAppPdu[1], pipe);
            return 0;
        }
        default:
            return 0;
    }
}

/**********************************************************************
* @name      : security_handle_gwframe
* @brief     ：与证书管理工具通信应用类型帧处理
* @param[in] ：pip通道号 pAppPdu数据缓存 appLen数据长度
* @param[out]：pOutBuf返回报文缓存
* @return    ：返回报文长度
* @Create    : 陈德昶
* @Date      ：2020-03-09
* @Update    :

**********************************************************************/
int security_handle_toolframe(int pipe, DASAFE_FRAME_HEAD* head, uint8* pAppPdu, int appLen, uint8* pOutBuf)
{
    int index = 0;
    int len = 0;
    uint8 bCenrCode = 0;
    int ret = 0;

    switch(head->apptype)
    {
        //证书管理工具应用类型 定义
        case TOOL_TYPE_CHECK_RES_START://0x30证书管理工具发起认证请求
        {
            ret = security_handle_toolauth(pipe, head, &pAppPdu[index], appLen, pOutBuf);
            PRTL_FMT_DEBUG("security_handle_toolframe() 0x30证书管理工具发起认证请求 ret=%d \n", ret);
            return ret;
        }
        case TOOL_TYPE_CHECK_RES://0x32证书管理工具对终端认证请求的响应
        {
            uint8 result[2] = {0};

            if (security_chk_toolrandnumsign(&pAppPdu[index], result))
            {
                bfgDAEsamRunEn[pipe] = 3;
                index  = 6;
                pOutBuf[index++] = TOOL_TYPE_CHECK_RES_ACK;//认证结果返回
                pOutBuf[index++] = 0; //两字节安全扩展信息长度
                pOutBuf[index++] = 2;
                memcpy((char *)&pOutBuf[index],(char *)result,2);
                index += 2;

                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY验证随机数签名失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_RAND_CHECK,head->commtype);
        }
        case TOOL_TYPE_KEY_VER_RES://0x34获取终端密钥版本
        {
            index  = 6;
            pOutBuf[index++] = TOOL_TYPE_KEY_VER_RES_ACK;//终端返回密钥版本
            pOutBuf[index++] = 0; //两字节安全扩展信息长度
            pOutBuf[index++] = 9;

            if (security_get_keyversion(&pOutBuf[index++]))
            {
                if (security_get_randnum(&pOutBuf[index]))
                {
                    memcpy(gMtuRandNum[pipe], &pOutBuf[index], 8);
                    index += 8;
                    DAMakeFrameHead(pOutBuf, index-4, head->commtype);

                    return DAAddFrameTail(pOutBuf, index-4);
                }

                PRTL_FMT_DEBUG("SECURITY获取随机数失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);
            }
            else
                PRTL_FMT_DEBUG("SECURITY获取秘钥版本失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case TOOL_TYPE_MTU_ID_RES://0x36获取终端序列号
        {
            uint8 serials[8];

            if (security_get_serialnum(serials))
            {
                index  = 6;
                pOutBuf[index++] = TOOL_TYPE_MTU_ID_RES_ACK; //终端返回终端序列号及加密芯片序列号
                pOutBuf[index++] = 0; //两字节安全扩展信息长度
                pOutBuf[index++] = 24;
                //终端序列号(ID)=== 3字节终端类型 + 6字节厂商代码+ 3字节生产批号+ 8字节生产日期+ 4字节流水号
                pOutBuf[index++] = 'R';//终端型号R13 3字节
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '3';
                pOutBuf[index++] = 'X';//厂商代码XXXXXX 6字节
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = '0';//生产批号001 3字节
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '2';//生产日期20200626 8字节
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '9';
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '6';
                pOutBuf[index++] = '2';
                pOutBuf[index++] = '6';
                pOutBuf[index++] = '0';//流水号，取加密芯片序列号 4字节
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '0';
                pOutBuf[index++] = serials[7];
                //memcpy(&pOutBuf[index], serials, 8);
                //index += 8;
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY获取序列号失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case TOOL_TYPE_MTU_SERI_RES://0x38获取终端内置安全芯片序列号
        {
            uint8 serials[8];

            index  = 6;
            pOutBuf[index++] = TOOL_TYPE_MTU_SERI_RES_ACK; //终端返回芯片序列号及加密芯片序列号
            pOutBuf[index++] = 0; //两字节安全扩展信息长度
            pOutBuf[index++] = 8;

            if (security_get_serialnum(serials))
            {
                memcpy(pOutBuf+index, serials, 8);
                index += 8;
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY获取序列号失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case TOOL_TYPE_MTU_KEY_RES://0x3A获取终端安全芯片公钥
        {
            if(bfgDAEsamRunEn[pipe] == 0)//业务流程非法
            {
                PRTL_FMT_DEBUG("SECURITY业务流程非法(还没有认证),pipe:%u,应用类型:%02X\n", pipe, head->apptype);

                return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
            }

            index  = 6;
            len = security_get_publickey(pipe, &pOutBuf[index]);
            PRTL_FMT_DEBUG("SECURITY获取公钥, pipe:%u, 返回长度len:%u\n", pipe, len);

            if (len > 0) {
                index += len;
                head->commtype.ubit.encry = 1;
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);

                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY获取公钥失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case TOOL_TYPE_CERT_FILE_RES://0x3C证书管理工具将生成的证书请求文件发给终端
        {
            if(bfgDAEsamRunEn[pipe] == 0)//业务流程非法
            {
                PRTL_FMT_DEBUG("SECURITY业务流程非法(还没有认证),pipe:%u,应用类型:%02X\n", pipe, head->apptype);

                return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
            }

            int cerlen = security_recv_certdata(pipe, head->apptype, &pAppPdu[index], appLen, &bCenrCode);
            PRTL_FMT_DEBUG("case TOOL_TYPE_CERT_FILE_RES://0x3C cerlen=%d \n", cerlen);
            
            if (cerlen > 0) {
                //如果是最后一帧，签名
                index  = 6;
                pOutBuf[index++] = TOOL_TYPE_CERT_FILE_RES_ACK;

                len = security_get_infosign(gDAEsamCertBuf[pipe], cerlen, &pOutBuf[index+2]);

                if (len > 0)
                {
                    pOutBuf[index++] = len>>8; //两字节安全扩展信息长度
                    pOutBuf[index++] = len&0xff;
                    index += len;
                    memset((char *)gdCertBufIndx[pipe], 0, 129 * 2);

                    head->commtype.ubit.encry = 0; //非加密
                    DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                    return DAAddFrameTail(pOutBuf, index-4);
                }

                PRTL_FMT_DEBUG("SECURITY获取证书签名失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);
            }
            else 
            {
                //分帧响应
                PRTL_FMT_DEBUG("case TOOL_TYPE_CERT_FILE_RES://0x3C 分帧响应 \n");
                index = 6;
                pOutBuf[index++] = TOOL_TYPE_KEY_CERT_SEND_ACK;
                pOutBuf[index++] = 0; //两字节安全扩展信息长度
                pOutBuf[index++] = 2;
                pOutBuf[index++] = 0x90;
                pOutBuf[index++] = 0;
                head->commtype.ubit.encry = 0; //非加密
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);

                return DAAddFrameTail(pOutBuf, index-4);
            }

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case TOOL_TYPE_CERT_FILE_DOWN://0x3E证书管理工具将配电应用CA证书、主站证书、网关证书、终端证书导入终端
        {
            int cerlen = security_recv_certdata(pipe, head->apptype, &pAppPdu[index], appLen, &bCenrCode);

            if (cerlen == 0)
                break;

            if (cerlen > 0)
            {
                int i = 0;
                int j = 0;

                printf("收到证书,type:%02X,id:%u,内容:\n", head->apptype, bCenrCode);
                while (i < cerlen)
                {
                    for (j = 0; j < 33 && i < cerlen; j++)
                    {
                        printf("%02X", gDAEsamCertBuf[pipe][i++]);
                    }
                    printf("\n");
                }

                index  = 6;
                pOutBuf[index++] = TOOL_TYPE_CERT_FILE_DOWN_ACK; //终端返回证书下装结果
                pOutBuf[index++] = 0; //两字节安全扩展信息长度
                pOutBuf[index++] = 2;

                if(bCenrCode == 0x06)//终端证书
                    len = security_update_mtucert(gCertCode[pipe], gDAEsamCertBuf[pipe], cerlen, &pOutBuf[index]);
                else
                    len = security_update_hostcert(gCertCode[pipe], gDAEsamCertBuf[pipe], cerlen, &pOutBuf[index]);

                if (len > 0)
                {
                    index += 2;

                    bfgCertCodeDown[pipe] = 0;
                    gCertCode[pipe] = 0xff;
                    memset((char *)gdCertBufIndx[pipe], 0, 129 * 2);

                    DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                    return DAAddFrameTail(pOutBuf, index-4);
                }

                PRTL_FMT_DEBUG("SECURITY更新证书失败,pipe:%u,应用类型:%02X,证书号:%u\n", pipe, head->apptype, bCenrCode);
            }

            bfgCertCodeDown[pipe] = 0;
            gCertCode[pipe] = 0xff;
            memset((char *)gdCertBufIndx[pipe], 0, 129 * 2);

            return MakeErrFram(pOutBuf,DASAFE_ERR_CERT_DOWN,head->commtype);
        }
        case TOOL_TYPE_CERT_REBACK_RES://0x40终端初始证书回写
        {
            if(bfgDAEsamRunEn[pipe] == 0)//业务流程非法
            {
                PRTL_FMT_DEBUG("SECURITY业务流程非法(还没有认证),pipe:%u,应用类型:%02X\n", pipe, head->apptype);

                return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
            }

            int len = security_get_mtutestcert(gDAEsamCertBuf[pipe]);

            if (len > 0)
            {
                index  = 6;
                pOutBuf[index++] = TOOL_TYPE_CERT_REBACK_RES_ACK; //终端返回证书下装结果
                pOutBuf[index++] = 0; //两字节安全扩展信息长度
                pOutBuf[index++] = 2;

                if (security_update_mtucert(0, gDAEsamCertBuf[pipe], len, &pOutBuf[index]))
                {
                    index += 2;
                    DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                    return DAAddFrameTail(pOutBuf, index-4);
                }

                PRTL_FMT_DEBUG("SECURITY更新终端证书失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);
            }
            else
                PRTL_FMT_DEBUG("SECURITY获取测试证书失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,DASAFE_ERR_CERT_DOWN,head->commtype);
        }
        case TOOL_TYPE_KEY_CERT_RES://0x42证书管理工具导出终端公钥证书
        {
            if(bfgDAEsamRunEn[pipe] == 0)//业务流程非法
            {
                PRTL_FMT_DEBUG("SECURITY业务流程非法(还没有认证),pipe:%u,应用类型:%02X\n", pipe, head->apptype);

                return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
            }

            int cerlen = security_get_mtucert(gDAEsamCertBuf[pipe]);

            if (cerlen > 0)
            {
                bfgMtuCertSendEn[pipe][0] = TOOL_TYPE_KEY_CERT_SEND;//等待主逻辑上报，分帧需等应答
                bfgMtuCertSendEn[pipe][1] = cerlen>>8;
                bfgMtuCertSendEn[pipe][2] = cerlen&0xff;
                bfgMtuCertSendEn[pipe][3] = head->commtype.commtype[0];
                bfgMtuCertSendEn[pipe][4] = head->commtype.commtype[1];

                return 0;
            }

            PRTL_FMT_DEBUG("SECURITY获取终端证书失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,DASAFE_ERR_CERT_DOWN,head->commtype);
        }
        case TOOL_TYPE_KEY_CERT_RES_ACK://0x44证书管理工具向终端返回证书导出结果
        {
            gCertSendDelay[pipe] = 0;
            return 0;
        }
        case TOOL_TYPE_KEY_CERT_SEND_ACK://0x45分帧数据传输结果返回
        {
            //影响下一帧的传送
            return 0;
        }
        case TOOL_TYPE_MTU_KEY_REBACK://0x46证书管理工具恢复终端密钥
        {
            if(bfgDAEsamRunEn[pipe] == 0)//业务流程非法
            {
                PRTL_FMT_DEBUG("SECURITY业务流程非法(还没有认证),pipe:%u,应用类型:%02X\n", pipe, head->apptype);

                return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
            }

            uint8 result[2] = {0};

            if (security_restore_toolkey(&pAppPdu[index], appLen, result))
            {
                index = 6;
                pOutBuf[index++] = TOOL_TYPE_MTU_KEY_REBACK_ACK; //终端返回密钥恢复结果
                pOutBuf[index++] = 0; //两字节安全扩展信息长度
                pOutBuf[index++] = 0x02;
                memcpy((char *)&pOutBuf[index],(char *)result,2);
                index += 2;
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);

                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY恢复秘钥失败,pipe:%u,应用类型:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,DASAFE_ERR_KEY_REBACK,head->commtype);
        }
        //case TOOL_TYPE_APP_DATA_RES:   //0x48证书管理工具向终端发送配电数据,    工具暂不支持
        //TOOL_TYPE_APP_DATA_RES_ACK:  //0x49终端向证书管理工具发送配电数据，管理工具暂不支持
        default:
            break;
    }

    return 0;
}

/**********************************************************************
* @name      : security_dealframe
* @brief     ：安全协议解析处理
* @param[in] ：pip通道号 pInPdu安全协议报文输入缓存 appLen输入报文长度
* @param[out]：pOutBuf输出报文缓存
* @return    ：返回输出报文长度
* @Create    : 陈德昶
* @Date      ：2020-03-06
* @Update    :

**********************************************************************/
int security_dealframe(int pipe, uint8 *pInPdu, int nInPduLen, uint8 *pOutBuf)
{
    int     OutbufLen = 0;//返回的发送数据长度
    int     applen = 0;
    int     index = 0;
    uint8*  pAppPdu;
    DASAFE_FRAME_HEAD *p = (DASAFE_FRAME_HEAD *)pInPdu;

    applen = (p->lenH<<8) + p->lenL - 2; //解密之前的长度，去掉2字节报文类型

    if(p->commtype.ubit.encry == 1)
    {
        if (bfgDAEsamRunEn[pipe] == 0)
        {
            PRTL_FMT_DEBUG("SECURITY解密失败,认证之前无法解密\n");
            return 0;
        }

        //解密数据域
        if (bfgDAEsamRunEn[pipe] < 3) //主站网关解密
        {
            PRTL_FMT_DEBUG("bfgDAEsamRunEn[%u]=%d, 进入主站网关解密\n", pipe, bfgDAEsamRunEn[pipe]);
            applen = security_decryte(pipe, &pInPdu[6], applen, &pInPdu[6]);
        }
        else //证书管理工具解密
        {
            PRTL_FMT_DEBUG("bfgDAEsamRunEn[%u]=%d, 进入证书管理工具解密\n", pipe, bfgDAEsamRunEn[pipe]);
            applen = security_decryte_tool(pipe, &pInPdu[6], applen, &pInPdu[6]);
        }

        if (applen <= 0) {
            PRTL_FMT_DEBUG("SECURITY解密失败, pipe:%u, applen=%d\n", pipe, applen);

            return MakeErrFram(pOutBuf, BUSS_ERR_MESS_DECI, p->commtype);
        }

        PRTL_BUF_DEBUG(&pInPdu[6], applen, "SECURITY解密后明文,pipe:%u", pipe);
    }

    //获取数据域
    pAppPdu = &pInPdu[6];

    p->apptype = pAppPdu[index++];
    applen--; //去掉应用类型1字节得到数据域长度

    PRTL_FMT_DEBUG("security_dealframe() 应用类型p->apptype=%02x, commtype[0]=%02x, commtype[1]=%02x, 是否加密:%u \n", 
        p->apptype, p->commtype.commtype[0], p->commtype.commtype[1], p->commtype.ubit.encry);

    switch(p->apptype)
    {
        case APP_TYPE_NOMMAL:               //0x00 通用
        case APP_TYPE_FILE_ABOUT:           //0x01文件、参数相关
        case APP_TYPE_PARA_ACK:             //0x03参数确认等
        case APP_TYPE_CTRL_SELECT:          //0x05 遥控选择、遥控撤销
        case APP_TYPE_CTRL_RUN:             //0x07遥控执行
        {
            int len1 = security_handle_appframe(pipe, p, &pAppPdu[index], applen, pOutBuf);

            if (len1 < 0)
                return -1;

            OutbufLen = len1;

            break;
        }

        case APP_TYPE_NET_RAND:             //0x20 网关发送随机数，等待终端签名
        case APP_TYPE_NET_AUTH:             //0x22网关返回终端的认证结果，并向终端申请认证
        case APP_TYPE_NET_SERI_RES:         //0x24网关申请终端及芯片序列号
        {
            //获取扩展区长度
            applen = pAppPdu[index++] << 8;
            applen += pAppPdu[index++];
            if (applen + sizeof(DASAFE_FRAME_HEAD) + 4 > nInPduLen)
            {
                PRTL_FMT_DEBUG("SECURITY扩展数据长度0x%02X错误,pipe:%u\n", applen, pipe);
                return 0;
            }

            OutbufLen = security_handle_gwframe(pipe, p, &pAppPdu[index], applen, pOutBuf);
            break;
        }

        //证书管理工具应用类型 定义
        case TOOL_TYPE_CHECK_RES_START:     //0x30证书管理工具发起认证请求
        case TOOL_TYPE_CHECK_RES:           //0x32证书管理工具对终端认证请求的响应
        case TOOL_TYPE_KEY_VER_RES:         //0x34获取终端密钥版本
        case TOOL_TYPE_MTU_ID_RES:          //0x36获取终端序列号
        case TOOL_TYPE_MTU_SERI_RES:        //0x38获取终端内置安全芯片序列号
        case TOOL_TYPE_MTU_KEY_RES:         //0x3A获取终端安全芯片公钥
        case TOOL_TYPE_CERT_FILE_RES:       //0x3C证书管理工具将生成的证书请求文件发给终端
        case TOOL_TYPE_CERT_FILE_DOWN:      //0x3E证书管理工具将配电应用CA证书、主站证书、网关证书、终端证书导入终端
        case TOOL_TYPE_CERT_REBACK_RES:     //0x40终端初始证书回写
        case TOOL_TYPE_KEY_CERT_RES:        //0x42证书管理工具导出终端公钥证书
        case TOOL_TYPE_KEY_CERT_RES_ACK:    //0x44证书管理工具向终端返回证书导出结果
        case TOOL_TYPE_KEY_CERT_SEND_ACK:   //0x45分帧数据传输结果返回
        case TOOL_TYPE_MTU_KEY_REBACK:      //0x46证书管理工具恢复终端密钥
        {
            //获取扩展区长度
            applen = pAppPdu[index++] << 8;
            applen += pAppPdu[index++];
            if (applen + sizeof(DASAFE_FRAME_HEAD) + 4 > nInPduLen)
            {
                PRTL_FMT_DEBUG("SECURITY扩展数据长度0x%02X错误,pipe:%u\n", applen, pipe);
                return 0;
            }

            OutbufLen = security_handle_toolframe(pipe, p, &pAppPdu[index], applen, pOutBuf);
            break;
        }

        //主站与终端双向认证应用类型
        case APP_TYPE_HOST_RAND:            //0x50主站发送随机数
        case APP_TYPE_HOST_AUTH:            //0x52主站返回终端的认证结果，并向终端申请认证
        case APP_TYPE_HOST_SERI_RES:        //0x54主站申请终端加密芯片序列号
        case APP_TYPE_HOST_CODE_RES:        //0x56主站申请终端特征码
        case APP_TYPE_MANA_CODE_RES:        //0x58主站通过管理通道申请终端特征码
        //主站与终端密钥管理应用类型
        case APP_TYPE_KEY_VER_RES:          //0x60主站向终端申请密钥版本
        case APP_TYPE_KEY_CHANGE_RES:       //0x62主站发起密钥更新
        case APP_TYPE_KEY_REBACK_RES:       //0x64主站发起密钥恢复
        //主站与终端证书管理应用类型
        case APP_TYPE_CERT_CHANGE_RES:      //0x70主站发起证书更新
        case APP_TYPE_CERT_DOWN_RES:        //0x72主站发起证书下载
        case APP_TYPE_CERT_GET_RES:         //0x74主站获取终端证书
        case APP_TYPE_CERT_GET_OVER:        //0x76主站返回证书提取结果
        {
            applen = pAppPdu[index++] << 8;
            applen += pAppPdu[index++];
            if (applen + sizeof(DASAFE_FRAME_HEAD) + 4 > nInPduLen)
            {
                PRTL_FMT_DEBUG("SECURITY扩展数据长度0x%02X错误,pipe:%u\n", applen, pipe);
                return 0;
            }

            OutbufLen = security_handle_hostframe(pipe, p, &pAppPdu[index], applen, pOutBuf);
            break;
        }

        default:
        {
            PRTL_FMT_DEBUG("SECURITY不认识的应用类型%02X,pipe:%u\n", p->apptype, pipe);
            break;
        }
    }

    return OutbufLen;
}

/**********************************************************************
* @name      : security_unframe
* @brief     ：安全协议解帧处理
* @param[in] ：pip通道号 pInPdu安全协议报文输入缓存 appLen输入报文长度
* @param[out]：pOutBuf输出报文缓存
* @return    ：返回输出报文长度
* @Create    : 陈德昶
* @Date      ：2020-03-06
* @Update    :

**********************************************************************/
int security_unpackframe(int pipe, uint8 *pInPdu, int nInPduLen, uint8 *pOutBuf)
{
    PRTL_BUF_DEBUG(pInPdu, nInPduLen, "SECURITY收到报文,pip:%u,",pipe);

    if (SECURITY_MAX_PIPE <= pipe) {
        PRTL_FMT_DEBUG("SECURITY通道号过大,pipe:%u\n", pipe);

        return 0;
    }

#if SECURITY_FRAME_FETCH == 1
    int     applen = 0;
    uint8   dapdu[4096] = {0};

    //获取有效帧数据
    if ((applen = DAGetFrame(pipe, pInPdu, nInPduLen, dapdu)) <= 0)
        return 0;

    return security_dealframe(pipe, dapdu, applen, pOutBuf);
#else
    uint8 appType = 0;

    if (DACheckFrame(pInPdu, nInPduLen) <= 0) {
        if (gSecurityEnable) {
            PRTL_FMT_DEBUG("SECURITY帧格式错误,pipe:%u\n", pipe);

            return 0;
        }

        return frame_data_unpack(nInPduLen, pInPdu, pOutBuf, &appType);
    }

    return security_dealframe(pipe, pInPdu, nInPduLen, pOutBuf);
#endif
}

/**********************************************************************
* @name      : security_packappframe
* @brief     ：安全协议组帧处理，完成加密组帧操作
* @param[in] ：pip通道号 pInPdu安全协议报文输入(G104报文)缓存 nInPduLen输入报文长度
* @param[out]：pOutBuf输出报文缓存
* @return    ：返回输出安全协议报文长度
* @Create    : 陈德昶
* @Date      ：2020-03-06
* @Update    :

**********************************************************************/
int security_packappframe(int pipe, uint8 appType, uint8* pInPdu, uint8 nInPduLen, uint8* pOutBuf)
{
    if (!gSecurityEnable)
    {
        memcpy(pOutBuf, pInPdu, nInPduLen);

        return nInPduLen;
    }

    int index = 0;
    int len   = 0;
    DA_SFP_S_NUM_U frameType = {.commtype={0}};

    if(CheckBufEnTry(pInPdu, 0))//判断是否需要加密
    {
        frameType.ubit.encry = 1;
        index = DAMakeFrameHead(pOutBuf, nInPduLen, frameType);
        //加密、签名处理，并填写安全扩展区
        len = security_encryte(pipe, nInPduLen, pInPdu, &pOutBuf[index], appType);

        if(len > 0)
        {
            index += len;
        }
        else {
            PRTL_FMT_DEBUG("SECURITY加密失败,pipe:%u,应用类型:%02X,数据长:%u\n", pipe, appType, nInPduLen);
            return -1;
        }
    }
    else
    {
        index = DAMakeFrameHead(pOutBuf, nInPduLen, frameType);
        pOutBuf[index++] = appType;
        pOutBuf[index++] = nInPduLen;
        memcpy((char *)&pOutBuf[index],(char *)pInPdu,nInPduLen);
        index += nInPduLen;
        pOutBuf[index++] = 0; //两字节安全扩展信息长度
        pOutBuf[index++] = 0;
    }

    return DAAddFrameTail(pOutBuf, index-4);
}

/**********************************************************************
* @name      : security_report
* @brief     ：分帧上报终端证书
* @param[in] ：
* @param[out]：pPipe证书上报的通道号,pOutBuf发送数据缓存
* @return    ：-1未发送证书 0正在发送 >0发送帧长度
* @Create    : 陈德昶
* @Date      ：2020-03-09
* @Update    :

**********************************************************************/
int security_report(uint8* pPipe, uint8* pOutBuf)
{
    static uint8  bStep[SECURITY_MAX_PIPE] = {0};
    static uint16 gindex[SECURITY_MAX_PIPE] = {0};
    uint16 cerlen = 0;
    uint16 index = 0;
    uint16 len = 0;
    uint8  totalno =  0, curno = 0, cerid = 6; //终端证书标识固定为6
    int    i = 0, ret = -1;
    DA_SFP_S_NUM_U frameType;
    memset(&frameType, 0, sizeof(DA_SFP_S_NUM_U));

    for (i = 0; i < SECURITY_MAX_PIPE; i++)
    {
        if (!bfgMtuCertSendEn[i][0])
            continue;

        ret = 0;
        cerlen = (bfgMtuCertSendEn[i][1]<<8) + bfgMtuCertSendEn[i][2];
        totalno = (cerlen + CERT_SEND_MAX_LEN-1) / CERT_SEND_MAX_LEN;
        memcpy(frameType.commtype, &bfgMtuCertSendEn[i][3], 2);

        switch(bStep[i])
        {
            case 0://发送数据
            {
                if(gindex[i] <  cerlen)
                {
                    if((cerlen - gindex[i])> CERT_SEND_MAX_LEN)
                    {
                        len = CERT_SEND_MAX_LEN;
                    }
                    else
                    {
                        len = cerlen - gindex[i];
                    }

                    curno = gindex[i] / CERT_SEND_MAX_LEN + 1;
                    bStep[i] = 1;
                    gCertSendDelay[i] = 5;//延时5秒

                    index = 6;
                    pOutBuf[index++] = bfgMtuCertSendEn[i][0];
                    pOutBuf[index++] = (len+3) >> 8; //两字节安全扩展信息长度
                    pOutBuf[index++] = (len+3);
                    pOutBuf[index++] = cerid;
                    pOutBuf[index++] = totalno;
                    pOutBuf[index++] = curno;

                    memcpy(&pOutBuf[index], &gDAEsamCertBuf[i][gindex[i]], len);
                    index += len;
                    gindex[i] += len;
                    *pPipe = i;

                    DAMakeFrameHead(pOutBuf, index-4, frameType);
                    return DAAddFrameTail(pOutBuf, index-4);
                }

                gindex[i] = 0;
                bStep[i] = 0;
                bfgMtuCertSendEn[i][0] = 0;
                break;
            }
            case 1://等待
            {
                if(bfgMtuCertSendEn[i][0] == APP_TYPE_CERT_GET_ACK)//不需要延时，短时间sleep
                {
                    usleep(10000);//延时10毫秒
                    bStep[i] = 0;
                }
                else
                {
                    if(gCertSendDelay[i])
                    {
                        usleep(10000);//延时1秒
                        gCertSendDelay[i]--;
                        if(gCertSendDelay[i] == 0)
                        {
                            bStep[i] = 0;
                        }
                    }
                    else
                    {
                        bStep[i] = 0;
                    }
                }
                break;
            }
            default:
            {
                PRTL_FMT_DEBUG("SECURITY上报步骤异常,pipe:%u,应用类型:%02X,数据长:%u,步骤:%u\n", i, bfgMtuCertSendEn[i][0], cerlen, bStep[i]);
                gindex[i] = 0;
                bStep[i] = 0;
                bfgMtuCertSendEn[i][0] = 0;
                break;
            }
        }
    }

    return ret;
}

/**********************************************************************
* @name      : security_enable
* @brief     ：使能安全协议解析及上报
* @param[in] ：enable 0禁止  其它 使能
* @param[out]：
* @return    ：
* @Create    : 陈德昶
* @Date      ：2020-04-02
* @Update    :

**********************************************************************/
void security_enable(uint8 enable)
{
    gSecurityEnable = (enable != 0);

    PRTL_FMT_DEBUG("设置安装协议使能标志%u\n", enable);
}


