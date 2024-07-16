/*
*********************************************************************
* @file    class7.c
* @brief： class7处理
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
#include "area.h"
#include "msg_id.h"

//
// class7
//

const OOP_OAD_U g_evt_300f_obj[]= {//电能表电压逆相序事件
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    
    {0x00102201},//事件发生时刻正向有功总电能
    {0x00202201},//事件发生时刻反向有功总电能
    {0x00302201},//事件发生时刻组合无功1总电能
    {0x00402201},//事件发生时刻组合无功2总电能
    {0x00112201},//事件发生时刻A相正向有功电能
    {0x00212201},//事件发生时刻A相反向有功电能
    {0x00312201},//事件发生时刻A相组合无功1电能
    {0x00412201},//事件发生时刻A相组合无功2电能
    {0x00122201},//事件发生时刻B相正向有功电能
    {0x00222201},//事件发生时刻B相反向有功电能
    {0x00322201},//事件发生时刻B相组合无功1电能
    {0x00422201},//事件发生时刻B相组合无功2电能
    {0x00132201},//事件发生时刻C相正向有功电能
    {0x00232201},//事件发生时刻C相反向有功电能
    {0x00332201},//事件发生时刻C相组合无功1电能
    {0x00432201},//事件发生时刻C相组合无功2电能
    {0x00108201},//事件结束后正向有功总电能
    {0x00208201},//事件结束后反向有功总电能
    {0x00308201},//事件结束后组合无功1总电能
    {0x00408201},//事件结束后组合无功2总电能
    {0x00118201},//事件结束后A相正向有功电能
    {0x00218201},//事件结束后A相反向有功电能
    {0x00318201},//事件结束后A相组合无功1电能
    {0x00418201},//事件结束后A相组合无功2电能
    {0x00128201},//事件结束后B相正向有功电能
    {0x00228201},//事件结束后B相反向有功电能
    {0x00328201},//事件结束后B相组合无功1电能
    {0x00428201},//事件结束后B相组合无功2电能
    {0x00138201},//事件结束后C相正向有功电能
    {0x00238201},//事件结束后C相反向有功电能
    {0x00338201},//事件结束后C相组合无功1电能
    {0x00438201},//事件结束后C相组合无功2电能
};

const OOP_OAD_U g_evt_3010_obj[]={//电能表电流逆相序事件
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    
    {0x00102201},//事件发生时刻正向有功总电能
    {0x00202201},//事件发生时刻反向有功总电能
    {0x00302201},//事件发生时刻组合无功1总电能
    {0x00402201},//事件发生时刻组合无功2总电能
    {0x00112201},//事件发生时刻A相正向有功电能
    {0x00212201},//事件发生时刻A相反向有功电能
    {0x00312201},//事件发生时刻A相组合无功1电能
    {0x00412201},//事件发生时刻A相组合无功2电能
    {0x00122201},//事件发生时刻B相正向有功电能
    {0x00222201},//事件发生时刻B相反向有功电能
    {0x00322201},//事件发生时刻B相组合无功1电能
    {0x00422201},//事件发生时刻B相组合无功2电能
    {0x00132201},//事件发生时刻C相正向有功电能
    {0x00232201},//事件发生时刻C相反向有功电能
    {0x00332201},//事件发生时刻C相组合无功1电能
    {0x00432201},//事件发生时刻C相组合无功2电能
    {0x00108201},//事件结束后正向有功总电能
    {0x00208201},//事件结束后反向有功总电能
    {0x00308201},//事件结束后组合无功1总电能
    {0x00408201},//事件结束后组合无功2总电能
    {0x00118201},//事件结束后A相正向有功电能
    {0x00218201},//事件结束后A相反向有功电能
    {0x00318201},//事件结束后A相组合无功1电能
    {0x00418201},//事件结束后A相组合无功2电能
    {0x00128201},//事件结束后B相正向有功电能
    {0x00228201},//事件结束后B相反向有功电能
    {0x00328201},//事件结束后B相组合无功1电能
    {0x00428201},//事件结束后B相组合无功2电能
    {0x00138201},//事件结束后C相正向有功电能
    {0x00238201},//事件结束后C相反向有功电能
    {0x00338201},//事件结束后C相组合无功1电能
    {0x00438201},//事件结束后C相组合无功2电能
};

const OOP_OAD_U g_evt_3100_obj[]={//终端初始化事件
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
};

const OOP_OAD_U g_evt_3101_obj[]={//终端版本变更事件
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    
    {0x43002302},//事件发生前软件版本号
    {0x43004302},//事件发生前软件版本号
};
const OOP_OAD_U g_evt_3104_obj[]={//终端状态量变位事件
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    
    {0x201E4200},//事件发生时间
    {0xF2034201},//第1路状态量变位发生后状态
    {0xF2034202},//第2路状态量变位发生后状态
    {0xF2034203},//第3路状态量变位发生后状态
    {0xF2034204},//第4路状态量变位发生后状态
    {0xF2034205},//第5路状态量变位发生后状态
    {0xF2034206},//第6路状态量变位发生后状态
    {0xF2034207},//第7路状态量变位发生后状态
    {0xF2034208},//第8路状态量变位发生后状态
    {0xF2034209},//第9路状态量变位发生后状态
    {0xF203420a},//第10路状态量变位发生后状态
    {0xF203420b},//第11路状态量变位发生后状态
    {0xF203420c},//第12路状态量变位发生后状态
    {0xF203420d},//第13路状态量变位发生后状态
    {0xF203420e},//第14路状态量变位发生后状态
    {0xF203420f},//第15路状态量变位发生后状态
    {0xF2034210},//第16路状态量变位发生后状态
};
const OOP_OAD_U g_evt_3105_obj[]={//电能表时钟超差事件
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    {0x33130206},//电能表时钟
    {0x33130207},//终端当前时钟
};

const OOP_OAD_U g_evt_3106_obj[]={//停上电事件记录单元
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    {0x33090206},//属性标志bit-string(SIZE(8))
};

const OOP_OAD_U g_evt_3109_obj[]={//消息认证错误记录
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    
    {0x44012200},//事件发生前安全认证密码
};

const OOP_OAD_U g_evt_310A_obj[]={//终端故障记录
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
};

const OOP_OAD_U g_evt_310B_obj[]={//电能表示度下降
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    
    {0x00102201},//事件发生前正向有功总电能
    {0x00108201},//事件结束后正向有功总电能
};

const OOP_OAD_U g_evt_310C_obj[]={//电能量超差
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    
    {0x00102201},//事件发生前正向有功总电能
    {0x00108201},//事件结束后正向有功总电能
};

const OOP_OAD_U g_evt_310D_obj[]={//电能表飞走
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    
    {0x00102201},//事件发生前正向有功总电能
    {0x00108201},//事件结束后正向有功总电能
};

const OOP_OAD_U g_evt_310E_obj[]={//电能表停走
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    
    {0x00102201},//事件发生前正向有功总电能
};

const OOP_OAD_U g_evt_310F_obj[]={//485抄表失败
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    
    {0x60412200},//事件发生前最近一次抄表成功时间
    {0x00102201},//事件发生前最近一次抄表成功正向有功总电能示值
    {0x00302201},//事件发生前最近一次抄表成功正向无功总电能示值
};

const OOP_OAD_U g_evt_3110_obj[]={//终端与主站通信流量超门限
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    
    {0x22004202},//事件发生后当月已发生通信流量
    {0x31100601},//月通信流量门限
};

const OOP_OAD_U g_evt_3111_obj[]={//发现未知电表事件记录
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    {0x33030206},//ARRAY一个搜表结果
};

const OOP_OAD_U g_evt_3112_obj[]={//跨台区电表事件记录
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    {0x33040206},//ARRAY一个跨台区结果
};

const OOP_OAD_U g_evt_3113_obj[]={//补抄失败事件记录
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    
    {0x60412200},//事件发生前最近一次抄表成功时间
    {0x00102201},//事件发生前最近一次抄表成功正向有功总电能示值
    {0x00302201},//事件发生前最近一次抄表成功正向无功总电能示值
};

const OOP_OAD_U g_evt_3114_obj[]={//对时事件记录
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    
    {0x40002200},//事件发生前对时时间
    {0x40008200},//事件结束后对时时间
};

const OOP_OAD_U g_evt_3115_obj[]={//遥控跳闸记录
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    {0x330A0206},//控制命令状态
    {0x330A0207},//控后2分钟总加组功率 array long64
    {0x330A0208},//控时2分钟总加组功率 array long64
};
    
const OOP_OAD_U g_evt_3118_obj[]={//终端编程记录
    {0x20220200},//事件记录序号5
    {0x201E0200},//事件发生时间8
    {0x20200200},//事件结束时间8
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    {0x33020206},//编程对象列表array OAD
};

const OOP_OAD_U g_evt_3119_obj[]={//终端电流回路异常事件
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态

    {0x20034200},//事件发生时电压电流相角
    {0x00104201},//事件发生时正向有功总电能
};

const OOP_OAD_U g_evt_311C_obj[]={//电能表数据变更监控记录
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    {0x330F0206},//监控数据对象
    {0x330F0207},//变化前数据
    {0x330F0208},//变化后数据

};

const OOP_OAD_U g_evt_3200_obj[]={//功控跳闸记录
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    {0x33050206},//事件发生后2分钟功率long64
    {0x810E0200},//控制对象
    {0x810F0200},//跳闸轮次
    {0x33050209},//功控定值long64(无OAD)
    {0x3305020A},//跳闸发生前总加有功功率long64
};

const OOP_OAD_U g_evt_3201_obj[]={//电控跳闸记录
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    {0x810E0200},//控制对象
    {0x810F0200},//跳闸轮次
    {0x81100200},//电控定值
    {0x33060209},//跳闸发生时总加电能量long64
};

const OOP_OAD_U g_evt_3202_obj[]={//购电参数设置记录
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    {0x810C2200},//事件发生前购电控配置单元
};

const OOP_OAD_U g_evt_3203_obj[]={//电控告警事件记录
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    {0x810E0200},//控制对象
    {0x81100200},//电控定值
};

const OOP_OAD_U g_evt_3140_obj[]={
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    {0x33160206},//异常标志bit-string（16）
    {0x33160207},//以太网端口开放信息long-unsigned
    {0x33160208},//以太网外联信息octet-string
    {0x33160209},//远程通信模块端口开放信息long-unsigned
    {0x3316020A},//远程通信模块外联信息octet-string
    {0x3316020B},//USB设备接入信息bit-string（8）
    {0x3316020C},//本地维护端口信息bit-string（8）
    {0x3316020D},//关键文件目录变更信息octet-string
    {0x3316020E},//以太网登录控制台信息bit-string（8）
    {0x3316020F},//串口登录控制台信息bit-string（8）
    {0x33160210},//非法操作命令信息bit-string（8）
    {0x33160211},//SSH服务异常类别
    {0x33160212},//流量会话异常信息 OPTIONAL
    {0x33160213},//流量带宽异常信息 OPTIONAL
    {0x33160214},//进程异常信息          OPTIONAL
    {0x33160215},//用户登录异常信息 OPTIONAL
};

const OOP_OAD_U g_evt_3199_obj[]={//ICCID变更
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    {0x33990206},//变更前 ICCID
    {0x33990207},//变更后 ICCID
};
	
const OOP_OAD_U g_evt_3600_obj[]={//系统监测事件
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    {0x33270206},//告警类别
    {0x33270207},//实际值
    {0x33270208},//设置阈值
    {0x33270209},//容器名称
    {0x3327020a},//应用名称
    {0x3327020b},//服务名称
};

const OOP_OAD_U g_evt_3601_obj[]={//容器监测事件
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    {0x33270206},//告警类别
    {0x33270207},//实际值
    {0x33270208},//设置阈值
    {0x33270209},//容器名称
    {0x3327020a},//应用名称
    {0x3327020b},//服务名称
};

const OOP_OAD_U g_evt_3602_obj[]={//应用监测事件
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    {0x33270206},//告警类别
    {0x33270207},//实际值
    {0x33270208},//设置阈值
    {0x33270209},//容器名称
    {0x3327020a},//应用名称
    {0x3327020b},//服务名称
};

const OOP_OAD_U g_evt_3120_obj[]={
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//事件上报状态
    {0x33180206},//异常标志bit-string（8）
};

const OOP_OAD_U g_evt_302A_obj[]={
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//事件上报状态
};


#if DESC("事件基础单元D2B",1)

/**
*********************************************************************
* @brief：      事件关联OAD，子结构体拼装报文（临时函数）
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_oad(OOP_OAD_U oad, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
#ifdef AREA_HUNAN
    return data_to_buf_evt_oad_hunan( oad, dstBuf, dstLen, pOffset, srcData, srcLen);
#endif 
    int32 nRet = ERR_NORMAL;
    uint8 *dstBufTmp = (uint8 *)dstBuf; 
    uint32 offset = *pOffset;
    uint8 i = 0;
    OOP_DAR_E dar = DATA_SUCCESS;

    switch(oad.value)
    {
        case 0x810c2201: 
        {
            OOP_CFGUNIT_810C_T data;
            if(srcLen != sizeof(OOP_CFGUNIT_810C_T))
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. srcLen(%d)!=OOP_CFGUNIT_810C_T(%d)\n", srcLen, sizeof(OOP_CFGUNIT_810C_T));
                return DATA_TYPE_UNMATCHED;
            }
            memcpy(&data, srcData, sizeof(data));

            if(data.object == 0)    //第一次设置时，没有上次购电单
            {
                dstBufTmp[offset++] = DT_NULL;
                *pOffset = offset;
                return ERR_OK;
            }

            //结构类型
//            dstBufTmp[offset++] = DT_STRUCTURE;
//
//            //结构成员数
//            dstBufTmp[offset++] = 8;

            dar = basic_data_to_buf(E_OI, dstBufTmp, dstLen-offset, &offset, &data.object, sizeof(uint16));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),object(%d)\n", dar, data.object);
                return DATA_TYPE_UNMATCHED;
            }  

//            dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &data.number, sizeof(uint32));
//            if(dar != DATA_SUCCESS)
//            {
//                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),delay(%d)\n", dar, data.number);
//                return DATA_TYPE_UNMATCHED;
//            }
//
//            dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &data.flag, sizeof(uint8));
//            if(dar != DATA_SUCCESS)
//            {
//                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),delay(%d)\n", dar, data.flag);
//                return DATA_TYPE_UNMATCHED;
//            }
//
//            dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &data.type, sizeof(uint8));
//            if(dar != DATA_SUCCESS)
//            {
//                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),delay(%d)\n", dar, data.flag);
//                return DATA_TYPE_UNMATCHED;
//            }
//
//            dar = basic_data_to_buf(E_LONG64, dstBufTmp, dstLen-offset, &offset, &data.purElectricity, sizeof(int64));
//            if(dar != DATA_SUCCESS)
//            {
//                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),limit(%ll)\n", dar, data.purElectricity);
//                return DATA_TYPE_UNMATCHED;
//            } 
//
//            dar = basic_data_to_buf(E_LONG64, dstBufTmp, dstLen-offset, &offset, &data.alarmLimit, sizeof(int64));
//            if(dar != DATA_SUCCESS)
//            {
//                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),limit(%ll)\n", dar, data.alarmLimit);
//                return DATA_TYPE_UNMATCHED;
//            }  
//
//            dar = basic_data_to_buf(E_LONG64, dstBufTmp, dstLen-offset, &offset, &data.tripLimit, sizeof(int64));
//            if(dar != DATA_SUCCESS)
//            {
//                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),limit(%ll)\n", dar, data.alarmLimit);
//                return DATA_TYPE_UNMATCHED;
//            } 
//
//            dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &data.mode, sizeof(uint8));
//            if(dar != DATA_SUCCESS)
//            {
//                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),delay(%d)\n", dar, data.mode);
//                return DATA_TYPE_UNMATCHED;
//            }
             
        }break;  
		case 0x810c2200: 
        {
            OOP_CFGUNIT_810C_T data;
            if(srcLen != sizeof(OOP_CFGUNIT_810C_T))
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. srcLen(%d)!=OOP_CFGUNIT_810C_T(%d)\n", srcLen, sizeof(OOP_CFGUNIT_810C_T));
                return DATA_TYPE_UNMATCHED;
            }
            memcpy(&data, srcData, sizeof(data));

            if(data.object == 0)        //第一次设置时，没有上次购电单
            {
                dstBufTmp[offset++] = DT_NULL;
                *pOffset = offset;
                return ERR_OK;
            }

            //结构类型
            dstBufTmp[offset++] = DT_STRUCTURE;

            //结构成员数
            dstBufTmp[offset++] = 8;

            dar = basic_data_to_buf(E_OI, dstBufTmp, dstLen-offset, &offset, &data.object, sizeof(uint16));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),object(%d)\n", dar, data.object);
                return DATA_TYPE_UNMATCHED;
            }  

            dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &data.number, sizeof(uint32));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),delay(%d)\n", dar, data.number);
                return DATA_TYPE_UNMATCHED;
            }

            dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &data.flag, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),delay(%d)\n", dar, data.flag);
                return DATA_TYPE_UNMATCHED;
            }

            dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &data.type, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),delay(%d)\n", dar, data.flag);
                return DATA_TYPE_UNMATCHED;
            }

            dar = basic_data_to_buf(E_LONG64, dstBufTmp, dstLen-offset, &offset, &data.purElectricity, sizeof(int64));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),limit(%ll)\n", dar, data.purElectricity);
                return DATA_TYPE_UNMATCHED;
            } 

            dar = basic_data_to_buf(E_LONG64, dstBufTmp, dstLen-offset, &offset, &data.alarmLimit, sizeof(int64));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),limit(%ll)\n", dar, data.alarmLimit);
                return DATA_TYPE_UNMATCHED;
            }  

            dar = basic_data_to_buf(E_LONG64, dstBufTmp, dstLen-offset, &offset, &data.tripLimit, sizeof(int64));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),limit(%ll)\n", dar, data.alarmLimit);
                return DATA_TYPE_UNMATCHED;
            } 

            dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &data.mode, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),delay(%d)\n", dar, data.mode);
                return DATA_TYPE_UNMATCHED;
            }
             
        }break;  
		case 0xF2050200:  //第8路状态变位发生后状态           
        {
            OOP_SWITCHOUT_EVENT_T switchOut;
            memcpy(&switchOut, srcData, srcLen);

			dstBufTmp[offset++] = DT_ARRAY;
			dstBufTmp[offset++] = (uint8)switchOut.nNum;

			int i;
			for(i=0; i<switchOut.nNum; i++)
			{
				dstBufTmp[offset++] = DT_STRUCTURE;
    
	            //结构成员数
	            dstBufTmp[offset++] = 4; 
				
	            if (switchOut.value[i].name.nNum == 0)
                {
                    sprintf(switchOut.value[i].name.value, "Relay-%d", i + 1);
                    switchOut.value[i].name.nNum = strlen(switchOut.value[i].name.value) + 1;
                }
				
	            if(basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBufTmp, dstLen-offset, &offset, &switchOut.value[i].name, sizeof(OOP_VISIBLEVAR_T)) < 0)
	            {
	                nRet = ERR_NORMAL;
	                return nRet;
	            }
	            
	            if(basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &switchOut.value[i].current, sizeof(uint8)) < 0)
	            {
	                nRet = ERR_NORMAL;
	                return nRet;
	            }

				if(basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &switchOut.value[i].attribute, sizeof(uint8)) < 0)
	            {
	                nRet = ERR_NORMAL;
	                return nRet;
	            }

				if(basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &switchOut.value[i].input, sizeof(uint8)) < 0)
	            {
	                nRet = ERR_NORMAL;
	                return nRet;
	            }
			}
        }break;
        case 0x43002302:   //事件发生前软件版本号
        case 0x43004302:   //事件发生后软件版本号
        {
            OOP_VERSION_T version;
            memcpy(&version.szSoftVer, srcData, srcLen);
            
            dstBufTmp[offset++] = DT_VISIBLE_STRING;
            dstBufTmp[offset++] = 4;
            memcpy(&dstBufTmp[offset], version.szSoftVer.value, 4);    
            offset += 4;  

        }break;
        
        case 0x60412200:  //事件发生前最近一次抄表成功时间
        case 0x201E4200:  //事件发生时间
        case 0x40002200:  //对时前时间
        case 0x40008200:  //对时后时间        
        {
            if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen-offset, &offset, srcData, srcLen) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
        }break;
        case 0x44012200:
        case 0x44012201:
        {   
            //PRTL_BUF_DEBUG(srcData,srcLen,"srcData :\n");
            //visiblestr数据类型
            dstBufTmp[offset++] = DT_VISIBLE_STRING;
            dstBufTmp[offset++] = srcLen;
            memcpy(&dstBufTmp[offset],srcData,srcLen);
            offset +=srcLen;
        }break;
        case 0xF2034201:  //第1路状态变位发生后状态
        case 0xF2034202:  //第2路状态变位发生后状态
        case 0xF2034203:  //第3路状态变位发生后状态
        case 0xF2034204:  //第4路状态变位发生后状态   
        case 0xF2034205:  //第5路状态变位发生后状态
        case 0xF2034206:  //第6路状态变位发生后状态
        case 0xF2034207:  //第7路状态变位发生后状态
        case 0xF2034208:  //第8路状态变位发生后状态
#ifdef PRODUCT_ZCU_1
        case 0xF2034209:  //第9路状态变位发生后状态
        case 0xF203420A:  //第10路状态变位发生后状态
        case 0xF203420B:  //第11路状态变位发生后状态
        case 0xF203420C:  //第12路状态变位发生后状态   
        case 0xF203420D:  //第13路状态变位发生后状态
        case 0xF203420E:  //第14路状态变位发生后状态
        case 0xF203420F:  //第15路状态变位发生后状态
        case 0xF2034210:  //第16路状态变位发生后状态
#endif
        {
            INPUTSTATE swstate;
            memcpy(&swstate, srcData, srcLen);
    
            dstBufTmp[offset++] = DT_STRUCTURE;
    
            //结构成员数
            dstBufTmp[offset++] = 2; 
            
            if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &swstate.st, sizeof(uint8)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            
            if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &swstate.cd, sizeof(uint8)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
  
        }break;
    
        case 0x22004202:   //事件发生后，当月已发生通信流量
        case 0x31100601:   //月通信流量门限
        {
            if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, srcData, sizeof(uint32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            } 

        }break;
    	case 0x20140200:
    	{
			OOP_METWORD_T var;
            memcpy(&var, srcData, srcLen);
    
            dstBufTmp[offset++] = DT_ARRAY;
    
            //数组成员个数
            dstBufTmp[offset++] = var.nNum;
    
            for(i = 0; i < var.nNum; i++)
            {
                if(basic_data_to_buf(E_OOP_BITSTR16_T, dstBufTmp, dstLen-offset, &offset, &var.metWord[i], sizeof(uint16)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
    	}
		break;
        case 0x20034200:   //事件发生时电压电流相角
        case 0x20000200:   //电压
        {
            OOP_WORD3V_T var; 
            memcpy(&var, srcData, srcLen);
    
            dstBufTmp[offset++] = DT_ARRAY;
    
            //数组成员个数
            dstBufTmp[offset++] = var.nNum;
    
            for(i = 0; i < var.nNum; i++)
            {
                if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &var.nValue[i], sizeof(uint16)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }break;  
		case 0x20010200:   //电流
        {
            OOP_INT3V_T var; 
            memcpy(&var, srcData, srcLen);
    
            dstBufTmp[offset++] = DT_ARRAY;
    
            //数组成员个数
            dstBufTmp[offset++] = var.nNum;
    
            for(i = 0; i < var.nNum; i++)
            {
                if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, &var.nValue[i], sizeof(int32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }break; 
		case 0x20040200:   //有功功率
		case 0x20050200:   //无功功率
        {
            OOP_INT4V_T var; 
            memcpy(&var, srcData, srcLen);
    
            dstBufTmp[offset++] = DT_ARRAY;
    
            //数组成员个数
            dstBufTmp[offset++] = var.nNum;
    
            for(i = 0; i < var.nNum; i++)
            {
                if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, &var.nValue[i], sizeof(int32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }break; 
		case 0x200A0200:   //功率因数
        {
            OOP_LONG4V_T var; 
            memcpy(&var, srcData, srcLen);
    
            dstBufTmp[offset++] = DT_ARRAY;
    
            //数组成员个数
            dstBufTmp[offset++] = var.nNum;
    
            for(i = 0; i < var.nNum; i++)
            {
                if(basic_data_to_buf(E_LONG, dstBufTmp, dstLen-offset, &offset, &var.nValue[i], sizeof(uint16)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }break;
		case 0xF2030200:   //功率因数
        {
            OOP_SWITCHIN_STATE_T var; 
            memcpy(&var, srcData, srcLen);
    
            dstBufTmp[offset++] = DT_ARRAY;
    
            //数组成员个数
            dstBufTmp[offset++] = var.nNum;
    
            for(i = 0; i < var.nNum; i++)
            {
				dstBufTmp[offset++] = DT_STRUCTURE;
				
				//结构成员数
				dstBufTmp[offset++] = 2; 
				
				if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &var.state[i].st, sizeof(uint8)) < 0)
				{
					nRet = ERR_NORMAL;
					return nRet;
				}
				
				if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &var.state[i].cd, sizeof(uint8)) < 0)
				{
					nRet = ERR_NORMAL;
					return nRet;
				}

        	}
        }break;
    	case 0x00100201:  //事件发生前的正向有功总电能
        case 0x00102201:  //事件发生前的正向有功总电能
        case 0x00112201:  //事件发生前的A相正向有功电能 
        case 0x00122201:  //事件发生前的B相正向有功电能        
        case 0x00132201:  //事件发生前的C相正向有功电能        
        
        case 0x00104201:  //事件发生时的正向有功总电能

		case 0x00200201:  //事件发生前的反向有功总电能
        case 0x00202201:  //事件发生前的反向有功总电能
        case 0x00212201:  //事件发生前的A相反向有功电能 
        case 0x00222201:  //事件发生前的B相反向有功电能        
        case 0x00232201:  //事件发生前的C相反向有功电能  
        
        case 0x00108201:  //事件结束后的正向有功总电能  
        case 0x00118201:  //事件结束后的A相正向有功电能 
        case 0x00128201:  //事件结束后的B相正向有功电能        
        case 0x00138201:  //事件结束后的C相正向有功电能     

        case 0x00208201:  //事件结束后的反向有功总电能  
        case 0x00218201:  //事件结束后的A相反向有功电能 
        case 0x00228201:  //事件结束后的B相反向有功电能        
        case 0x00238201:  //事件结束后的C相反向有功电能 

		case 0x00500201:  //事件结束后的反向有功总电能  
        case 0x00600201:  //事件结束后的A相反向有功电能 
        case 0x00700201:  //事件结束后的B相反向有功电能        
        case 0x00800201:  //事件结束后的C相反向有功电能 
        
        {
            uint32 energys;
            
            if(srcLen != sizeof(uint32))
            {
                PRTL_FMT_DEBUG("srcLen check failed. srcLen(%d) != %d \n", srcLen, sizeof(uint32));
                return ERR_NORMAL;
            }
            
            memcpy(&energys, srcData, srcLen);  

            if(energys == 0) //TODO 姑且使用0作为不存在的判据
            {
                dstBufTmp[offset++] = DT_NULL;
            }
            else
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &energys, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }break; 
        
        case 0x00302201:  //事件发生前的组合无功1总电能 
        case 0x00312201:  //事件发生前的A相组合无功1电能 
        case 0x00322201:  //事件发生前的B相组合无功1电能        
        case 0x00332201:  //事件发生前的C相组合无功1电能  
        
        case 0x00402201:  //事件发生前的组合无功2总电能  
        case 0x00412201:  //事件发生前的A相组合无功2电能 
        case 0x00422201:  //事件发生前的B相组合无功2电能        
        case 0x00432201:  //事件发生前的C相组合无功2电能  

        case 0x00308201:  //事件结束后的组合无功1总电能  
        case 0x00318201:  //事件结束后的A相组合无功1电能 
        case 0x00328201:  //事件结束后的B相组合无功1电能        
        case 0x00338201:  //事件结束后的C相组合无功1电能     

        case 0x00408201:  //事件结束后的组合无功2总电能  
        case 0x00418201:  //事件结束后的A相组合无功2电能 
        case 0x00428201:  //事件结束后的B相组合无功2电能        
        case 0x00438201:  //事件结束后的C相组合无功2电能         
        {
            int energys;

            if(srcLen != sizeof(int))
            {
                PRTL_FMT_DEBUG("srcLen check failed. srcLen(%d) != %d \n", srcLen, sizeof(int));
                return ERR_NORMAL;
            }
            
            memcpy(&energys, srcData, srcLen);  

            if(energys == 0) //TODO 姑且使用0作为不存在的判据
            {
                dstBufTmp[offset++] = DT_NULL;
            }
            else
            {
                if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, &energys, sizeof(int)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }break; 
        default:
        {
            PRTL_FMT_DEBUG("no search this one evt oad %08x\n", oad.value);
            dstBufTmp[offset++] = DT_NULL;
            nRet = ERR_NORMAL; 
        }break;
            
    }

    *pOffset = offset;
    
    return ERR_OK;
}

/**
*********************************************************************
* @brief：       编程对象列表，子结构体拼装报文
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_proglist(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    OOP_PROGRAMLOG_T progEvt;
    uint8 offlen = 0; //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码
    uint8 i = 0;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    memcpy(&progEvt, srcData, srcLen);

    //数组类型
    dstBufTmp[offset++] = DT_ARRAY;

    //数组成员个数
    offlen = set_len_offset(progEvt.nOADNum, lenBuf);
    memcpy(&dstBufTmp[offset], lenBuf, offlen);
    offset += offlen;

    for(i = 0; i < progEvt.nOADNum; i++)
    {       
        //OAD
        dar = basic_data_to_buf(E_OAD, dstBufTmp, dstLen, &offset, &progEvt.oad[i], sizeof(OOP_OAD_U));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("[%d]basic_data_to_buf failed. dar(%d), offset(%d)\n", i, dar, offset);
            return DATA_TYPE_UNMATCHED;
        }
    }

    *pOffset = offset;

    return dar;    
}

/**
*********************************************************************
* @brief：      事件上报状态，子结构体拼装报文
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_report(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    OOP_REPORTS_T evtreport;
    uint8 offlen = 0; //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码
    uint8 i = 0;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    memcpy(&evtreport, srcData, srcLen);

    //数组类型
    dstBufTmp[offset++] = DT_ARRAY;

    //数组成员个数
    offlen = set_len_offset(evtreport.nNum, lenBuf);
    memcpy(&dstBufTmp[offset], lenBuf, offlen);
    offset += offlen;

    //数组成员
    for(i = 0; i < evtreport.nNum; i++)
    {
        //结构类型
        dstBufTmp[offset++] = DT_STRUCTURE;

        //结构成员个数
        dstBufTmp[offset++] = 2;

        //OAD
        dar = basic_data_to_buf(E_OAD, dstBufTmp, dstLen-offset, &offset, &evtreport.state[i].oad, sizeof(OOP_OAD_U));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d), oad(0x%08x)\n", i, dar, evtreport.state[i].oad.value);
            return dar;
        }      

        //上报状态
        dar = basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtreport.state[i].state, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d), state(0x%08x)\n", i, dar, evtreport.state[i].state);
            return dar;
        }         
    }

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @brief：      事件发生源，子结构体拼装报文
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_source(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    OOP_EVTSOURCE_T evtsrc;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    memcpy(&evtsrc, srcData, srcLen);

    switch(evtsrc.choice)
    {
        case DT_OAD: 
        {
            dar = basic_data_to_buf(E_OAD, dstBufTmp, dstLen-offset, &offset, &evtsrc.oad, sizeof(evtsrc.oad));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), oad(0x%08x)\n", dar, evtsrc.oad.value);
                return dar;
            }
        }break;

        case DT_TSA: 
        {
            dar = basic_data_to_buf(E_TSA, dstBufTmp, dstLen-offset, &offset, &evtsrc.add, sizeof(evtsrc.add));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), add.af(%d)\n", dar, evtsrc.add.af);
                return dar;
            }
        }break;  

        case DT_OI: 
        {
            dar = basic_data_to_buf(E_OI, dstBufTmp, dstLen-offset, &offset, &evtsrc.oi, sizeof(evtsrc.oi));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), oi(%d)\n", dar, evtsrc.oi);
                return dar;
            }
        }break;   

         case DT_UNSIGNED: 
        {
            dar = basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtsrc.nValue, sizeof(evtsrc.nValue));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), nValue(%d)\n", dar, evtsrc.nValue);
                return dar;
            }
        }break; 

        case DT_NULL:
        {
            dar = basic_data_to_buf(E_NULL, dstBufTmp, dstLen-offset, &offset, &evtsrc.nValue, sizeof(evtsrc.nValue));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), nValue(%d)\n", dar, evtsrc.nValue);
                return dar;
            }
        }break;
            
        case DT_ENUM:
        {
            dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &evtsrc.nValue, sizeof(evtsrc.nValue));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), nValue(%d)\n", dar, evtsrc.nValue);
                return dar;
            }
        }break;

        default: 
        {
            PRTL_FMT_DEBUG("data_to_buf_evt_source check type failed. choice(%d)\n", evtsrc.choice);
            return DATA_TYPE_UNMATCHED;
        }
        break;
    }
    
    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @brief：      事件发生源，子结构体拼装报文
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_basic(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    EVT_BASIC_T evtBasic;
    uint8 *dstBufTmp = (uint8 *)dstBuf; 
    
    memcpy(&evtBasic, srcData, srcLen);
    
    /*事件记录序号*/
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtBasic.nIndex, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), nIndex(%d)\n", dar, evtBasic.nIndex);
        return dar;
    }
    
    //事件发生时间
    dar = basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen-offset, &offset, &evtBasic.tmStart, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), cStart(%04d-%02d-%02d %02d:%02d:%02d)\n", 
            dar, evtBasic.tmStart.year, evtBasic.tmStart.month, evtBasic.tmStart.day, 
            evtBasic.tmStart.hour, evtBasic.tmStart.minute, evtBasic.tmStart.second);
        return dar;
    }      

    if(evtBasic.tmEnd.year == 0) //TODO 姑且使用0作为不存在的判据
    {
       dstBufTmp[offset++] = DT_NULL; 
    }
    else
    {
        //事件结束时间
        dar = basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen-offset, &offset, &evtBasic.tmEnd, sizeof(OOP_DATETIME_S_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), tmEnd(%04d-%02d-%02d %02d:%02d:%02d)\n", 
                dar, evtBasic.tmEnd.year, evtBasic.tmEnd.month, evtBasic.tmEnd.day, 
                evtBasic.tmEnd.hour, evtBasic.tmEnd.minute, evtBasic.tmEnd.second);
            return dar;
        }
    }
    
     //事件发生源
    dar = data_to_buf_evt_source(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic.source, sizeof(OOP_EVTSOURCE_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_source failed. dar(%d), choice(%d)\n", dar, evtBasic.source.choice);
        return dar;
    }
        
    //事件上报状态
    dar = data_to_buf_evt_report(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic.status, sizeof(OOP_REPORTS_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_report failed. dar(%d), nNum(%d)\n", dar, evtBasic.status.nNum);
        return dar;
    }

    *pOffset = offset;

    return dar;    
}

/**
*********************************************************************
* @brief：      事件关联OAD，子结构体拼装报文
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_coload(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    OOP_EVTBUF_T coload;
    uint8 i = 0;
    uint32 srcOffset = 0;

    memcpy(&coload, srcData, srcLen);

    if(coload.oadNum > OOP_MAX_EVTOADS)
    {
        PRTL_FMT_DEBUG("fatal error. oda(0x%08x), coload.oadNum(%d) > OOP_MAX_OADS(%d)\n", pOadInfo->pTab->oad.value, coload.oadNum, OOP_MAX_OADS);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    for(i = 0; i < coload.oadNum; i++)
    {
        //从OAD的结构中，拼装698格式报文 实现 normal_oad_data_to_buf 函数
        dar = data_to_buf_evt_oad(coload.cols.oad[i], dstBuf, dstLen, &offset, &coload.buf[srcOffset], coload.idLen[i]);
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("data_to_buf_evt_oad failed. [%d] oda(0x%08x), idLen(%d)\n", i, coload.cols.oad[i].value, coload.idLen[i]);
            return DATA_TYPE_UNMATCHED;          
        }
        
        srcOffset += coload.idLen[i];
    }

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @brief：      一个搜表结果，子结构体拼装报文
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_search_met_res(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    int32 nRet = ERR_NORMAL;
    uint32     offset = *pOffset;
    OOP_SEARCHMET_RES_T searchres;
    #if(!defined AREA_JIANGSU)
    uint8 offlen = 0; //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码
    uint8 i = 0;
    #endif
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    memcpy(&searchres, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;

    //结构成员个数
    dstBufTmp[offset++] = 7;    

    //通信地址
    if(basic_data_to_buf(E_TSA, dstBufTmp, dstLen-offset, &offset, &searchres.commAdd, sizeof(searchres.commAdd)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //所属采集器地址
    if(basic_data_to_buf(E_TSA, dstBufTmp, dstLen-offset, &offset, &searchres.collAdd, sizeof(searchres.collAdd)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //规约类型
    if(basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &searchres.proto, sizeof(searchres.proto)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //相位
    if(basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &searchres.phase, sizeof(searchres.phase)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //信号品质
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &searchres.quality, sizeof(searchres.quality)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //搜到的时间
    if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen-offset, &offset, &searchres.serchTime, sizeof(searchres.serchTime)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //附加信息
    dstBufTmp[offset++] = DT_ARRAY;
    #ifdef AREA_JIANGSU
    dstBufTmp[offset++] = 0x00;
    #endif
    #if(!defined AREA_JIANGSU)
    //附加信息个数
    offlen = set_len_offset(searchres.nAdded, lenBuf);
    memcpy(&dstBufTmp[offset], lenBuf, offlen);
    offset += offlen;

    for(i = 0; i < searchres.nAdded; i++)
    {
        //结构类型
        dstBufTmp[offset++] = DT_STRUCTURE;

        //结构成员个数
        dstBufTmp[offset++] = 2;
        
        //OAD
        if(basic_data_to_buf(E_OAD, dstBufTmp, dstLen-offset, &offset, &searchres.info[i].oad, sizeof(OOP_OAD_U)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }

        //Data
        if(vardata_data_to_buf(searchres.info[i].value, dstBufTmp, dstLen-offset, &offset) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }       
    }
    #endif

    *pOffset = offset;

    return ERR_OK;
}


/**
*********************************************************************
* @brief：      一个搜表结果，子结构体拼装报文
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_acrossarea_met_res(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    int32 nRet = ERR_NORMAL;
    uint32     offset = *pOffset;
    OOP_ACROSSAREA_RES_T accross;
//    uint8 offlen = 0; //数组数量长度
//    uint8 lenBuf[3] = {0}; //数组数量编码
    //uint8 i = 0;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    memcpy(&accross, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;

    //结构成员个数
    dstBufTmp[offset++] = 03;    

    //通信地址
    if(basic_data_to_buf(E_TSA, dstBufTmp, dstLen-offset, &offset, &accross.commAdd, sizeof(accross.commAdd)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //所属采集器地址
    if(basic_data_to_buf(E_TSA, dstBufTmp, dstLen-offset, &offset, &accross.netAdd, sizeof(accross.netAdd)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //搜到的时间
    if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen-offset, &offset, &accross.change, sizeof(accross.change)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    *pOffset = offset;

    return ERR_OK;
}


/**
*********************************************************************
* @brief：       关联对象属性表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_asobjattr(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_OADS_T oadsets;
    uint8 offlen = 0; //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码
    uint8 i = 0;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    memcpy(&oadsets, srcData, srcLen);

    //数组类型
    dstBufTmp[offset++] = DT_ARRAY;

    //数组成员个数
    offlen = set_len_offset(oadsets.nNum, lenBuf);
    memcpy(&dstBufTmp[offset], lenBuf, offlen);
    offset += offlen;

    for(i = 0; i < oadsets.nNum; i++)
    {       
        //OAD
        if(basic_data_to_buf(E_OAD, dstBufTmp, dstLen-offset, &offset, &oadsets.oad[i], sizeof(OOP_OAD_U)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }
    }

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      事件上报状态，子结构体拼装报文
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_stat(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTSTAT_T evtStat;
    uint8 offlen = 0; //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码
    uint8 i = 0;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    /*当前值记录表∷=array 当前值
    
      当前值∷=structure
      {
         事件发生源 instance-specific，
         事件发生时间统计 structure
      {
         事件发生次数 double-long-unsigned，
         事件累计时间 double-long-unsigned
      }
    }*/

    memcpy(&evtStat, srcData, srcLen);

    //数组类型
    dstBufTmp[offset++] = DT_ARRAY;

    //数组成员个数
    offlen = set_len_offset(evtStat.nNum, lenBuf);
    memcpy(&dstBufTmp[offset], lenBuf, offlen);
    offset += offlen;


    for(i = 0; i < evtStat.nNum; i++)
    {
        //结构类型
        dstBufTmp[offset++] = DT_STRUCTURE;
        
        //结构成员数
        dstBufTmp[offset++] = 2;


        //事件发生源
        if(data_to_buf_evt_source(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtStat.stat[i].source, sizeof(OOP_EVTSOURCE_T)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }        

        //结构类型
        dstBufTmp[offset++] = DT_STRUCTURE;
        
        //结构成员数
        dstBufTmp[offset++] = 2;

         
        //事件发生次数
        if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtStat.stat[i].dwCount, sizeof(uint32)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }

        //事件累积时间
        if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtStat.stat[i].dwTimes, sizeof(uint32)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }        
    }

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      事件上报状态，子结构体拼装报文
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_time(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTTIME_T evtTime;
    uint8 offlen = 0; //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码
    uint8 i = 0;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
  
    /*  时间状态记录表∷=array 时间状态
        时间状态∷=structure
        {
            事件发生源 instance-specific，
            最近一次时间 structure
        {
            最近一次发生时间 date_time_s，
            最近一次结束时间 date_time_s
        }
    }*/

    memcpy(&evtTime, srcData, srcLen);

    //数组类型
    dstBufTmp[offset++] = DT_ARRAY;

    //数组成员个数
    offlen = set_len_offset(evtTime.nNum, lenBuf);
    memcpy(&dstBufTmp[offset], lenBuf, offlen);
    offset += offlen;

    for(i = 0; i < evtTime.nNum; i++)
    {
        //结构类型
        dstBufTmp[offset++] = DT_STRUCTURE;
        
        //结构成员数
        dstBufTmp[offset++] = 2;

        //事件发生源
        if(data_to_buf_evt_source(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtTime.stat[i].source, sizeof(OOP_EVTSOURCE_T)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }        

        //结构类型
        dstBufTmp[offset++] = DT_STRUCTURE;
        
        //结构成员数
        dstBufTmp[offset++] = 2;

         
        //最近一次发生时间
        if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen-offset, &offset, &evtTime.stat[i].tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }

        //最近一次结束时间
        if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen-offset, &offset, &evtTime.stat[i].tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }        
    }

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief        配置参数-
*               300C-电能表功率因数超下限事件
*               301D-电能表电压不平衡事件
*               301E-电能表电流不平衡事件
*               302D-电能表电流严重不平衡事件
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfgld(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_CFG_LU_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 2;
           

    //下限阈值
    if(basic_data_to_buf(E_LONG, dstBufTmp, dstLen-offset, &offset, &evtcfg.nShort, sizeof(int16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }      

    //判定延时时间
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.nByte, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }  

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
*@brief         配置参数-
*               300E-电能表辅助电源掉电事件
*               300F-电能表电压逆相序事件
*               3010-电能表电流逆相序事件
*               3011-电能表掉电事件
*               3030-通信模块变更事件
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfgdelay(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_DELAY_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 1;
               
    //判定延时时间
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen, &offset, &evtcfg.nDelay, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }  

    *pOffset = offset;
     
    return ERR_OK;
}


/**
*********************************************************************
* @brief：       3105-电能表时钟超差事件配置参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfg3105(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_3105_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 2;
         
    //异常判别阈值
    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.nValue, sizeof(uint16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //关联采集任务号
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.nTID, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }        

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：       停电数据采集配置参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_acquire(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_DATAACQUIRE_T evtcfg;
    uint8 i = 0;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 4;

    //bit-string(SIZE(8))
    dstBufTmp[offset++] = DT_BIT_STRING;

    //bit-string(SIZE(8))长度为8
    dstBufTmp[offset++] = 8;
  
    //采集标志
    memcpy(&dstBufTmp[offset], &evtcfg.flag, sizeof(uint8));
    offset += sizeof(uint8);
         
    //停电事件抄读时间间隔
    dar = basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen, &offset, &evtcfg.interval, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), interval(%d)\n", dar, evtcfg.interval);
        return dar;
    }      

    //停电事件抄读时间限值
    dar = basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen, &offset, &evtcfg.limit, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), limit(%d)\n", dar, evtcfg.limit);
        return dar;
    } 

    //数组类型
    dstBufTmp[offset++] = DT_ARRAY;

    //数组长度
    dstBufTmp[offset++] = evtcfg.meterNum;

    for(i = 0; i < evtcfg.meterNum; i++)
    {
        //需要读取停电事件的电能表
        dar = basic_data_to_buf(E_TSA, dstBufTmp, dstLen, &offset, &evtcfg.meter[i], sizeof(OOP_TSA_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("[%d]basic_data_to_buf failed. dar(%d)\n", i, dar);
            return dar;
        } 

    }
    
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       停电事件甄别限值参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_identify(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_EVENTIDENTIFY_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 6;

    //停电时间最小有效间隔
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &evtcfg.minInterval, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), limit(%d)\n", dar, evtcfg.minInterval);
        return dar;
    } 

    //停电时间最大有效间隔
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &evtcfg.maxInterval, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), limit(%d)\n", dar, evtcfg.maxInterval);
        return dar;
    }   

    //停电事件起止时间偏差限值
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &evtcfg.limit1, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), limit(%d)\n", dar, evtcfg.limit1);
        return dar;
    } 

    //停电事件时间区段偏差限值
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &evtcfg.limit2, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), limit(%d)\n", dar, evtcfg.limit2);
        return dar;
    } 

    //停电发生电压限值
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &evtcfg.occVolt, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), limit(%d)\n", dar, evtcfg.occVolt);
        return dar;
    } 

    //停电恢复电压限值
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &evtcfg.recVolt, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), limit(%d)\n", dar, evtcfg.recVolt);
        return dar;
    } 
    
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       终端停/上电事件配置参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_poweroncut(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_POWERONCUT_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 2;

    //停电发生电压限值
    dar = data_to_buf_evt_acquire(pOdaInfo, dstBufTmp, dstLen, &offset, &evtcfg.cParam, sizeof(OOP_DATAACQUIRE_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return dar;
    }

    //停电恢复电压限值
    dar = data_to_buf_evt_identify(pOdaInfo, dstBufTmp, dstLen, &offset, &evtcfg.lParam, sizeof(OOP_EVENTIDENTIFY_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return dar;
    }

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @brief：       310B-电能表示度下降事件
*               311C-电能表数据变更监控记录
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfg310b(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_310B_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 1;

    //关联采集任务号
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.nTID, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }        

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：       310C-电能表超差事件
*               310D-电能表飞走事件
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfg310x(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_310X_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 2;
         
    //异常判别阈值
    if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.nValue, sizeof(uint32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //关联采集任务号
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.nTID, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }        

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：       310E-电能表停走事件配置参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfg310e(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_310E_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 2;
         
    //阈值
    if(basic_data_to_buf(E_TI, dstBufTmp, dstLen-offset, &offset, &evtcfg.threshold, sizeof(OOP_TI_T)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //关联采集任务号
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.nTID, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }        

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：       310F-终端抄表失败事件配置参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfg310f(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_310F_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 2;
         
    //重试轮次
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.nTicks, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //关联采集任务号
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.nTID, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }        

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：       310B-电能表示度下降事件
*               311C-电能表数据变更监控记录
* @param[in]: srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfg3110(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_3110_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 1;

    //通信流量门限
    if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.nValue, sizeof(uint32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：       310B-电能表示度下降事件
*               311C-电能表数据变更监控记录
* @param[in]: srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfg3140(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_3140_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 3;

    //月累计流量限值
    if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.flowLimit, sizeof(uint32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //流速限值
    if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.speedLimit, sizeof(uint32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    
    //SSH长时间未关闭限值
    if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.sshLTNoClose, sizeof(uint32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：       系统监测事件
* @param[in]: srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfg3600(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_3600_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 6;

    //内存使用率上限阈值
    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.ramRate, sizeof(uint16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //存储使用率上限阈值
    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.memRate, sizeof(uint16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    
    //CPU使用率上限阈值
    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.cpuRate, sizeof(uint16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

	//主板温度上限阈值 
    if(basic_data_to_buf(E_LONG, dstBufTmp, dstLen-offset, &offset, &evtcfg.tempBroadUp, sizeof(uint16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

	//主板温度下限阈值 
    if(basic_data_to_buf(E_LONG, dstBufTmp, dstLen-offset, &offset, &evtcfg.tempBroadDown, sizeof(uint16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

	//监测周期
    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.period, sizeof(uint16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：       以太网开放端口信息
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_ethport(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_SECURITYLOG_T data;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    uint32 i = 0;
    
    memcpy(&data, srcData, srcLen);

    //数组类型
    dstBufTmp[offset++] = DT_ARRAY;
    
    //数据成员数
    dstBufTmp[offset++] = data.nEthPort;

    for(i = 0; i < data.nEthPort; i++)
    {
        //开放端口信息
        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &data.ethPort[i], sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),ethPort(%d)\n", i, dar, data.ethPort[i]);
            return DATA_TYPE_UNMATCHED;
        }
    }
    
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       以太网外联信息
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_ethconnect(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_SECURITYLOG_T data;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    uint32 i = 0;
    
    memcpy(&data, srcData, srcLen);

    //数组类型
    dstBufTmp[offset++] = DT_ARRAY;
    
    //数据成员数
    dstBufTmp[offset++] = data.nEthConnects;

    for(i = 0; i < data.nEthConnects; i++)
    {
        //结构类型
        dstBufTmp[offset++] = DT_STRUCTURE;

        //结构成员数
        dstBufTmp[offset++] = 2;

        //外联IP
        dar = basic_data_to_buf(E_OOP_OCTETVAR16_T, dstBufTmp, dstLen, &offset, &data.ethConnects[i].ip, sizeof(OOP_OCTETVAR16_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),ip.nNum(%d)\n", i, dar, data.ethConnects[i].ip.nNum);
            return DATA_TYPE_UNMATCHED;
        }

        //外联端口
        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &data.ethConnects[i].port, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),port(%d)\n", i, dar, data.ethConnects[i].port);
            return DATA_TYPE_UNMATCHED;
        }        
    }
    
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       通信模块开放端口信息
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_lteport(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_SECURITYLOG_T data;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    uint32 i = 0;
    
    memcpy(&data, srcData, srcLen);

    //数组类型
    dstBufTmp[offset++] = DT_ARRAY;
    
    //数据成员数
    dstBufTmp[offset++] = data.nLtePort;

    for(i = 0; i < data.nLtePort; i++)
    {
        //开放端口信息
        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &data.ltePort[i], sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),ethPort(%d)\n", i, dar, data.ethPort[i]);
            return DATA_TYPE_UNMATCHED;
        }
    }
    
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       通信模块外联信息
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_lteconnect(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_SECURITYLOG_T data;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    uint32 i = 0;
    
    memcpy(&data, srcData, srcLen);

    //数组类型
    dstBufTmp[offset++] = DT_ARRAY;
    
    //数据成员数
    dstBufTmp[offset++] = data.nLteConnects;

    for(i = 0; i < data.nLteConnects; i++)
    {
        //结构类型
        dstBufTmp[offset++] = DT_STRUCTURE;

        //结构成员数
        dstBufTmp[offset++] = 2;

        //外联IP
        dar = basic_data_to_buf(E_OOP_OCTETVAR16_T, dstBufTmp, dstLen, &offset, &data.lteConnects[i].ip, sizeof(OOP_OCTETVAR16_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),ip.nNum(%d)\n", i, dar, data.lteConnects[i].ip.nNum);
            return DATA_TYPE_UNMATCHED;
        }

        //外联端口
        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &data.lteConnects[i].port, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),port(%d)\n", i, dar, data.lteConnects[i].port);
            return DATA_TYPE_UNMATCHED;
        }        
    }
    
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       关键目录信息
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_chgpath(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_SECURITYLOG_T data;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    uint32 i = 0;
    
    memcpy(&data, srcData, srcLen);

    //数组类型
    dstBufTmp[offset++] = DT_ARRAY;
    
    //数据成员数
    dstBufTmp[offset++] = data.nChgPath;

    for(i = 0; i < data.nChgPath; i++)
    {
        //开放端口信息
        dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBufTmp, dstLen, &offset, &data.chgPath[i], sizeof(OOP_VISIBLEVAR_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),chgPath.nNum(%d)\n", i, dar, data.chgPath[i].nNum);
            return DATA_TYPE_UNMATCHED;
        }
    }
    
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       控后/控时总加组有功功率
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_clrttgpower(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    int64 data[8];
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    uint32 i = 0;
    
    memcpy(&data, srcData, srcLen);

    //数组类型
    dstBufTmp[offset++] = DT_ARRAY;
    
    //数据成员数
    dstBufTmp[offset++] = 8;

    for(i = 0; i < 8; i++)
    {
        dar = basic_data_to_buf(E_LONG64, dstBufTmp, dstLen, &offset, &data[i], sizeof(int64));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d), data(%d)\n", i, dar, data[i]);
            return DATA_TYPE_UNMATCHED;
        }
    }
    
    *pOffset = offset;
     
    return dar;
}


#endif

#if DESC("事件基础单元B2D",1)

/**
*********************************************************************
* @brief：      事件上报状态， 报文中解析出子结构体
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_report(const OAD_INFO_T *pOdaInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_REPORTS_T evtreport;
    uint8 offlen = 0; //数组数量长度
    uint8 i = 0;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //数组类型
    if(srcBufTmp[offset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check array type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    //数组成员个数
    evtreport.nNum = get_len_offset(&srcBufTmp[offset], &offlen);
    offset += offlen;

    if(evtreport.nNum > OOP_MAX_REPORTCHANNEL)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check array num failed. num(%d)", evtreport.nNum);
        return DATA_TYPE_UNMATCHED;
    }

    for(i = 0; i < evtreport.nNum; i++)
    {
        //结构类型
        if(srcBufTmp[offset++] != DT_STRUCTURE)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. offset(%d)", offset);
            return DATA_TYPE_UNMATCHED;
        }

        //结构成员个数
        if(srcBufTmp[offset++] != 2)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct num failed. offset(%d)", offset);
            return DATA_TYPE_UNMATCHED;
        }
        
        //OAD
        dar = basic_buf_to_data(E_OAD, srcBufTmp, srcLen-offset, &offset, &evtreport.state[i].oad, sizeof(OOP_OAD_U));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "[%d]basic_buf_to_data failed. dar(%d), offset(%d)\n", i, dar, offset);
            return DATA_TYPE_UNMATCHED;
        }

        //上报状态
        dar = basic_buf_to_data(E_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtreport.state[i].state, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "[%d]basic_buf_to_data failed. dar(%d), offset(%d)\n", i, dar, offset);
            return DATA_TYPE_UNMATCHED;
        } 
    }

    memcpy(dstData, &evtreport, dstLen);
    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      事件发生源， 报文中解析出子结构体
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_source(const OAD_INFO_T *pOdaInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_EVTSOURCE_T evtsrc;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //类型选择
    evtsrc.choice = srcBufTmp[offset++];

    //
    switch(evtsrc.choice)
    {
        case DT_OAD: 
        {
            dar = basic_buf_to_data(E_OAD, srcBufTmp, srcLen-offset, &offset, &evtsrc.oad, sizeof(evtsrc.oad));
            if(dar != DATA_SUCCESS)
            {
                PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
                return DATA_TYPE_UNMATCHED;
            } 
        }break;

        case DT_TSA: 
        {
            dar = basic_buf_to_data(E_TSA, srcBufTmp, srcLen-offset, &offset, &evtsrc.add, sizeof(evtsrc.add));
            if(dar != DATA_SUCCESS)
            {
                PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
                return DATA_TYPE_UNMATCHED;
            } 
        }break;  

        case DT_OI: 
        {
            dar = basic_buf_to_data(E_OI, srcBufTmp, srcLen-offset, &offset, &evtsrc.oi, sizeof(evtsrc.oi));
            if(dar != DATA_SUCCESS)
            {
                PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
                return DATA_TYPE_UNMATCHED;
            } 
        }break;   

        case DT_UNSIGNED: 
        {
            dar = basic_buf_to_data(E_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtsrc.nValue, sizeof(evtsrc.nValue));
            if(dar != DATA_SUCCESS)
            {
                PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
                return DATA_TYPE_UNMATCHED;
            } 
        }break; 

        case DT_NULL: break;  //为空，没东西需要转换

        default: 
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "buf_to_data_evt_source failed. offset(%d)\n", offset);
            return DATA_TYPE_UNMATCHED;  
        }break;
    }
    
    memcpy(dstData, &evtsrc, dstLen);
    *pOffset = offset;
   
    return dar;
}

/**
*********************************************************************
* @brief：      事件发生源，子结构体拼装报文
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 buf_to_data_evt_basic(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_OK;
    uint32     offset = *pOffset;
    EVT_BASIC_T evtBasic;
    uint8 *srcBufTmp = (uint8 *)srcBuf;
    
    /*事件记录序号*/
    if(basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtBasic.nIndex, sizeof(uint32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    
    //事件发生时间
    if(basic_buf_to_data(E_DATETIME_S, srcBufTmp, srcLen-offset, &offset, &evtBasic.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }      
    
    //事件结束时间
    if(basic_buf_to_data(E_DATETIME_S, srcBufTmp, srcLen-offset, &offset, &evtBasic.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    
     //事件发生源
    if(buf_to_data_evt_source(pOadInfo, srcBufTmp, srcLen-offset, &offset, &evtBasic.source, sizeof(OOP_EVTSOURCE_T)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
        
    //事件上报状态
    if(buf_to_data_evt_report(pOadInfo, srcBufTmp, srcLen-offset, &offset, &evtBasic.status, sizeof(OOP_REPORTS_T)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    memcpy(dstData, &evtBasic, dstLen);

    *pOffset = offset;

    return nRet;
}

/**
*********************************************************************
* @brief：      事件关联OAD，子结构体拼装报文
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 buf_to_data_evt_coload(const OAD_INFO_T *pOdaInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{   
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      一个搜表结果，子结构体拼装报文
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 buf_to_data_search_met_res(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{   
    int32 nRet = ERR_NORMAL;
    uint32     offset = *pOffset;
    OOP_SEARCHMET_RES_T searchres;
    uint8 offlen = 0; //数组数量长度
    uint8 i = 0;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //结构类型
    if(srcBufTmp[offset++] != DT_STRUCTURE)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //结构成员数
    if(srcBufTmp[offset++] != 7)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //通信地址
    if(basic_buf_to_data(E_TSA, srcBufTmp, srcLen-offset, &offset, &searchres.commAdd, sizeof(searchres.commAdd)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //所属采集器地址
    if(basic_buf_to_data(E_TSA, srcBufTmp, srcLen-offset, &offset, &searchres.collAdd, sizeof(searchres.collAdd)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //规约类型
    if(basic_buf_to_data(E_ENUM, srcBufTmp, srcLen-offset, &offset, &searchres.proto, sizeof(searchres.proto)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //相位
    if(basic_buf_to_data(E_ENUM, srcBufTmp, srcLen-offset, &offset, &searchres.phase, sizeof(searchres.phase)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //信号品质
    if(basic_buf_to_data(E_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &searchres.quality, sizeof(searchres.quality)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //搜到的时间
    if(basic_buf_to_data(E_DATETIME_S, srcBufTmp, srcLen-offset, &offset, &searchres.serchTime, sizeof(searchres.serchTime)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //附加信息
    if(srcBufTmp[offset++] != DT_ARRAY)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    
    //附加信息个数
    searchres.nAdded = get_len_offset(&srcBufTmp[offset], &offlen);
    offset += offlen;

    for(i = 0; i < searchres.nAdded; i++)
    {
        //结构类型
        if(srcBufTmp[offset++] != DT_STRUCTURE)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }

        //结构成员数
        if(srcBufTmp[offset++] != 2)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }
        
        //OAD
        if(basic_buf_to_data(E_OAD, srcBufTmp, srcLen-offset, &offset, &searchres.info[i].oad, sizeof(OOP_OAD_U)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }

        //Data
//        if(vardata_buf_to_data(&searchres.info[i].value, srcBufTmp, srcLen-offset, &offset) < 0)
//        {
//            nRet = ERR_NORMAL;
//            return nRet;
//        }       
    }

    memcpy(dstData, &searchres, dstLen);
    *pOffset = offset;

    return ERR_OK;
}

/**
*********************************************************************
* @brief：      关联对象属性表
* * @param[in]: srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_asobjattr(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E  dar = DATA_SUCCESS;   
    uint32  offset = *pOffset;
    OOP_OADS_T oadsets;
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
    oadsets.nNum = get_len_offset(&srcBufTmp[offset], &offlen);
    offset += offlen;

    if(oadsets.nNum > OOP_MAX_OADS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check array num failed. num(%d)", oadsets.nNum);
        return DATA_TYPE_UNMATCHED;
    }

    for(i = 0; i < oadsets.nNum; i++)
    {
        dar = basic_buf_to_data(E_OAD, srcBufTmp, srcLen-offset, &offset, &oadsets.oad[i], sizeof(OOP_OAD_U));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "[%d]basic_buf_to_data failed. dar(%d), offset(%d)\n", i, dar, offset);
            return DATA_TYPE_UNMATCHED;
        }
    }

    memcpy(dstData, &oadsets, dstLen);
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       当前值记录表
* * @param[in]: srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 buf_to_data_evt_stat(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTSTAT_T evtStat;
    uint8 offlen = 0; //数组数量长度
    uint8 i = 0;
    uint8 *srcBufTmp = (uint8 *)srcBuf;
    
    /*当前值记录表∷=array 当前值
    
      当前值∷=structure
      {
         事件发生源 instance-specific，
         事件发生时间统计 structure
      {
         事件发生次数 double-long-unsigned，
         事件累计时间 double-long-unsigned
      }
    }*/

    //数组类型
    if(srcBufTmp[offset++] != DT_ARRAY)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //数组成员个数
    evtStat.nNum = get_len_offset(&srcBufTmp[offset], &offlen);
    offset += offlen;

    for(i = 0; i < evtStat.nNum; i++)
    {
        //结构类型
         if(srcBufTmp[offset++] != DT_STRUCTURE)
         {
             nRet = ERR_NORMAL;
             return nRet;
         }
        
         //结构成员数
         if(srcBufTmp[offset++] != 2)
         {
             nRet = ERR_NORMAL;
             return nRet;
         }

        //事件发生源
        if(buf_to_data_evt_source(pOadInfo, srcBufTmp, srcLen-offset, &offset, &evtStat.stat[i].source, sizeof(OOP_EVTSOURCE_T)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }        

        //结构类型
         if(srcBufTmp[offset++] != DT_STRUCTURE)
         {
             nRet = ERR_NORMAL;
             return nRet;
         }
        
         //结构成员数
         if(srcBufTmp[offset++] != 2)
         {
             nRet = ERR_NORMAL;
             return nRet;
         }
         
        //事件发生次数
        if(basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtStat.stat[i].dwCount, sizeof(uint32)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }

        //事件累积时间
        if(basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtStat.stat[i].dwTimes, sizeof(uint32)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }        
    }

    memcpy(dstData, &evtStat, dstLen);
    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：       时间状态记录表
* * @param[in]: srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 buf_to_data_evt_time(const OAD_INFO_T *pOdaInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTTIME_T evtTime;
    uint8 offlen = 0; //数组数量长度
    uint8 i = 0;
    uint8 *srcBufTmp = (uint8 *)srcBuf;
    
    /*  时间状态记录表∷=array 时间状态
        时间状态∷=structure
        {
            事件发生源 instance-specific，
            最近一次时间 structure
        {
            最近一次发生时间 date_time_s，
            最近一次结束时间 date_time_s
        }
    }*/

    //数组类型
    if(srcBufTmp[offset++] != DT_ARRAY)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //数组成员个数
    evtTime.nNum = get_len_offset(&srcBufTmp[offset], &offlen);
    offset += offlen;

    for(i = 0; i < evtTime.nNum; i++)
    {
        //结构类型
         if(srcBufTmp[offset++] != DT_STRUCTURE)
         {
             nRet = ERR_NORMAL;
             return nRet;
         }
        
         //结构成员数
         if(srcBufTmp[offset++] != 2)
         {
             nRet = ERR_NORMAL;
             return nRet;
         }

        //事件发生源
        if(buf_to_data_evt_source(pOdaInfo, srcBufTmp, srcLen-offset, &offset, &evtTime.stat[i].source, sizeof(OOP_EVTSOURCE_T)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }        

        //结构类型
         if(srcBufTmp[offset++] != DT_STRUCTURE)
         {
             nRet = ERR_NORMAL;
             return nRet;
         }
        
         //结构成员数
         if(srcBufTmp[offset++] != 2)
         {
             nRet = ERR_NORMAL;
             return nRet;
         }
         
        //最近一次发生时间
        if(basic_buf_to_data(E_DATETIME_S, srcBufTmp, srcLen-offset, &offset, &evtTime.stat[i].tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }

        //最近一次结束时间
        if(basic_buf_to_data(E_DATETIME_S, srcBufTmp, srcLen-offset, &offset, &evtTime.stat[i].tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }        
    }

    memcpy(dstData, &evtTime, dstLen);
    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief        配置参数-
*               300C-电能表功率因数超下限事件
*               301D-电能表电压不平衡事件
*               301E-电能表电流不平衡事件
*               302D-电能表电流严重不平衡事件
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_cfgld(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_CFG_LU_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //结构类型
     if(srcBufTmp[offset++] != DT_STRUCTURE)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }
    
     //结构成员数
     if(srcBufTmp[offset++] != 2)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct num failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }      

    //触发下限
    dar = basic_data_to_buf(E_LONG, srcBufTmp, dstLen-offset, &offset, &evtcfg.nShort, sizeof(int16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    } 
    
    //判定延时时间
    dar = basic_data_to_buf(E_UNSIGNED, srcBufTmp, dstLen-offset, &offset, &evtcfg.nByte, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief        配置参数-
*               300E-电能表辅助电源掉电事件
*               300F-电能表电压逆相序事件
*               3010-电能表电流逆相序事件
*               3011-电能表掉电事件
*               3030-通信模块变更事件
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_cfgdelay(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_EVTCFG_DELAY_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //结构类型
     if(srcBufTmp[offset++] != DT_STRUCTURE)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }

     //结构成员数
     if(srcBufTmp[offset++] != 1)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct num failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }
    
    //判定延时时间
    dar = basic_buf_to_data(E_UNSIGNED, srcBufTmp, srcLen, &offset, &evtcfg.nDelay, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }    

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       3105-电能表时钟超差事件配置参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_cfg3105(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_EVTCFG_3105_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //结构类型
     if(srcBufTmp[offset++] != DT_STRUCTURE)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }
    
     //结构成员数
     if(srcBufTmp[offset++] != 2)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct num failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }

    //异常判别阈值
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.nValue, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }  

    //关联采集任务号
    dar = basic_buf_to_data(E_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.nTID, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }        

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       停电数据采集配置参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_acquire(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_DATAACQUIRE_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;
    uint32 i = 0;

    //结构类型
    if(srcBufTmp[offset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if(srcBufTmp[offset++] != 4)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct num failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    //bit-string(SIZE(8))
    if(srcBufTmp[offset++] != DT_BIT_STRING)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check bit-string type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    //bit-string(SIZE(8))长度为8
    if(srcBufTmp[offset++] != 8)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check bit-string size failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }    

    //采集标志 
    memcpy(&evtcfg.flag, &srcBufTmp[offset], sizeof(uint8));
    offset += sizeof(uint8);
         
    //停电事件抄读时间间隔
    dar = basic_buf_to_data(E_UNSIGNED, srcBufTmp, srcLen, &offset, &evtcfg.interval, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }          

    //停电事件抄读时间限值
    dar = basic_buf_to_data(E_UNSIGNED, srcBufTmp, srcLen, &offset, &evtcfg.limit, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //数组类型
    if(srcBufTmp[offset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check array type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    //数组元素个数
    evtcfg.meterNum = srcBufTmp[offset++];

    for(i = 0; i < evtcfg.meterNum; i++)
    {
        //需要读取停电事件的电能表
        dar = basic_buf_to_data(E_TSA, srcBufTmp, srcLen, &offset, &evtcfg.meter[i], sizeof(OOP_TSA_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "[%d]basic_buf_to_data failed. dar(%d), offset(%d)\n", i, dar, offset);
            return DATA_TYPE_UNMATCHED;
        }

    }
    
    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       停电事件甄别限值参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_identify(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_EVENTIDENTIFY_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;
    uint32 i = 0;

    //结构类型
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

    //停电时间最小有效间隔
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBufTmp, srcLen, &offset, &evtcfg.minInterval, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", i, dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //停电时间最大有效间隔
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBufTmp, srcLen, &offset, &evtcfg.maxInterval, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", i, dar, offset);
        return DATA_TYPE_UNMATCHED;
    }    

    //停电事件起止时间偏差限值
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBufTmp, srcLen, &offset, &evtcfg.limit1, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", i, dar, offset);
        return DATA_TYPE_UNMATCHED;
    } 

    //停电事件时间区段偏差限值
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBufTmp, srcLen, &offset, &evtcfg.limit2, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", i, dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //停电发生电压限值
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBufTmp, srcLen, &offset, &evtcfg.occVolt, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", i, dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //停电恢复电压限值
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBufTmp, srcLen, &offset, &evtcfg.recVolt, sizeof(uint16)) ;
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", i, dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       终端停/上电事件配置参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_poweroncut(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_POWERONCUT_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //结构类型
    if(srcBufTmp[offset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //结构成员数
    if(srcBufTmp[offset++] != 2)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct num failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    //停电数据采集配置参数
    dar = buf_to_data_evt_acquire(pOadInfo, srcBufTmp, srcLen, &offset, &evtcfg.cParam, sizeof(OOP_DATAACQUIRE_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "buf_to_data_evt_acquire failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //停电事件甄别限值参数
    dar = buf_to_data_evt_identify(pOadInfo, srcBufTmp, srcLen, &offset, &evtcfg.lParam, sizeof(OOP_EVENTIDENTIFY_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "buf_to_data_evt_identify failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }  
    
    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       310B-电能表示度下降事件
*               311C-电能表数据变更监控记录
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_cfg310b(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E  dar = DATA_SUCCESS;   
    uint32  offset = *pOffset;
    OOP_EVTCFG_310B_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //结构类型
     if(srcBufTmp[offset++] != DT_STRUCTURE)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }
    
     //结构成员数
     if(srcBufTmp[offset++] != 1)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct num failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }

    //关联采集任务号
    dar = basic_buf_to_data(E_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.nTID, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       310C-电能表超差事件
*               310D-电能表飞走事件
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_cfg310x(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E  dar = DATA_SUCCESS; 
    uint32  offset = *pOffset;
    OOP_EVTCFG_310X_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //结构类型
     if(srcBufTmp[offset++] != DT_STRUCTURE)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }
    
     //结构成员数
     if(srcBufTmp[offset++] != 2)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct num failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }

         
    //异常判别阈值
    dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.nValue, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }


    //关联采集任务号
    dar = basic_buf_to_data(E_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.nTID, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }      

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       310E-电能表停走事件配置参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_cfg310e(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E  dar = DATA_SUCCESS; 
    uint32  offset = *pOffset;
    OOP_EVTCFG_310E_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //结构类型
     if(srcBufTmp[offset++] != DT_STRUCTURE)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }
    
     //结构成员数
     if(srcBufTmp[offset++] != 2)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct num failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }
         
    //阈值
    dar = basic_buf_to_data(E_TI, srcBufTmp, srcLen-offset, &offset, &evtcfg.threshold, sizeof(OOP_TI_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }   

    //关联采集任务号
    dar = basic_buf_to_data(E_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.nTID, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }       

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       310F-终端抄表失败事件配置参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_cfg310f(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E  dar = DATA_SUCCESS; 
    uint32  offset = *pOffset;
    OOP_EVTCFG_310F_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //结构类型
     if(srcBufTmp[offset++] != DT_STRUCTURE)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }
    
     //结构成员数
     if(srcBufTmp[offset++] != 2)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct num failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }
         
    //重试轮次
    dar = basic_buf_to_data(E_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.nTicks, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    } 

    //关联采集任务号
    dar = basic_buf_to_data(E_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.nTID, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }       

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       310B-电能表示度下降事件
*               311C-电能表数据变更监控记录
* @param[in]: srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_cfg3110(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E  dar = DATA_SUCCESS; 
    uint32  offset = *pOffset;
    OOP_EVTCFG_3110_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

     //结构类型
     if(srcBufTmp[offset++] != DT_STRUCTURE)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }
    
     //结构成员数
     if(srcBufTmp[offset++] != 1)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct num failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }

    //通信流量门限
    dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.nValue, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       系统监测事件
* @param[in]: srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_cfg3600(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E  dar = DATA_SUCCESS; 
    uint32  offset = *pOffset;
    OOP_EVTCFG_3600_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

     //结构类型
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

    //通信流量门限
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.ramRate, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

	dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.memRate, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

	dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.cpuRate, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

	dar = basic_buf_to_data(E_LONG, srcBufTmp, srcLen-offset, &offset, &evtcfg.tempBroadUp, sizeof(int16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

	dar = basic_buf_to_data(E_LONG, srcBufTmp, srcLen-offset, &offset, &evtcfg.tempBroadDown, sizeof(int16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

	dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.period, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       310B-电能表示度下降事件
*               311C-电能表数据变更监控记录
* @param[in]: srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_cfg3140(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E  dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_EVTCFG_3140_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //结构类型
    if(srcBufTmp[offset++] != DT_STRUCTURE)
    {
     PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. offset(%d)", offset);
     return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if(srcBufTmp[offset++] != 3)
    {
     PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct num failed. offset(%d)", offset);
     return DATA_TYPE_UNMATCHED;
    }

    //月累计流量限值
    dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.flowLimit, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //流速限值
    dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.speedLimit, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //SSH长时间未关闭限值
    dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.sshLTNoClose, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return dar;
}

#endif

#if DESC("事件记录单元D2B",1)

/**
*********************************************************************
* @brief：     数据中心格式转698报文，应用监测事件记录单元
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_systemmonitor(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_modulelog param err\n");
        return DATA_OTHER_REASON;
    }

    if (pOadInfo->pTab->eData != E_OOP_SYSTEMMONITOR_T)
    {
         PRTL_FMT_DEBUG("check data type failed.\n");
         return DATA_TYPE_UNMATCHED;
    }

    OOP_MONITORLOG_T systemmonitor;

    if (srcLen != sizeof(systemmonitor))
    {
         PRTL_FMT_DEBUG("check data len failed.\n");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&systemmonitor, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE; 

    //结构成员个数
    dstBufTmp[offset++] = 0x0B;    

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = systemmonitor.nIndex;
    evtBasic.tmStart = systemmonitor.tmStart;
    evtBasic.tmEnd = systemmonitor.tmEnd;
    evtBasic.source = systemmonitor.source;
    evtBasic.status = systemmonitor.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/
    //告警类别
    dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &systemmonitor.warn, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
	//实际值
	dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &systemmonitor.value, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
	   PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
	   return DATA_TYPE_UNMATCHED;
    }
	//设置阈值
	dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &systemmonitor.rate, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
	   PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
	   return DATA_TYPE_UNMATCHED;
    }
	//容器名称
    dstBufTmp[offset++] = DT_VISIBLE_STRING;
    dstBufTmp[offset++] = systemmonitor.containerName.nNum;
    if(systemmonitor.containerName.nNum > 0)
    {
        memcpy(dstBufTmp + offset, systemmonitor.containerName.value, systemmonitor.containerName.nNum);
        offset += systemmonitor.containerName.nNum;
    }
	//应用名称
	dstBufTmp[offset++] = DT_VISIBLE_STRING;
    dstBufTmp[offset++] = systemmonitor.appName.nNum;
    if(systemmonitor.appName.nNum > 0)
    {
        memcpy(dstBufTmp + offset, systemmonitor.appName.value, systemmonitor.appName.nNum);
        offset += systemmonitor.appName.nNum;
    }
	//服务名称
	dstBufTmp[offset++] = DT_VISIBLE_STRING;
    dstBufTmp[offset++] = systemmonitor.serviceName.nNum;
    if(systemmonitor.serviceName.nNum > 0)
    {
        memcpy(dstBufTmp + offset, systemmonitor.serviceName.value, systemmonitor.serviceName.nNum);
        offset += systemmonitor.serviceName.nNum;
    }

    *pOffset = offset;
    
    return dar;
}

OOP_DAR_E data_to_buf_evt_cfg_delay(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_EVTCFG_DELAY_T data = {0};
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data)); 

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    PRTL_FMT_DEBUG("data_to_buf_evt_cfg_delay begin.\n");
    //结构数据类型
    dstBuf[dOffset++] = DT_STRUCTURE;
    dstBuf[dOffset++] = 0x01;

     if (basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nDelay, sizeof(data.nDelay)) < 0)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, dOffset);
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
    return ERR_OK;
}

OOP_DAR_E data_to_buf_evt_cfg_i0(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_LOAD_I0_EVECFG_T data = {0};
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data)); 

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);
    PRTL_FMT_DEBUG("data_to_buf_evt_cfg_i0 begin.\n");
    //结构数据类型
    dstBuf[dOffset++] = DT_STRUCTURE;
    dstBuf[dOffset++] = 0x03;

    if (basic_data_to_buf(E_DOUBLE_LONG, dstBuf, dstLen, &dOffset, &data.nADown, sizeof(data.nADown)) < 0)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, dOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    if (basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.wValue, sizeof(data.wValue)) < 0)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, dOffset);
        return DATA_TYPE_UNMATCHED;
    }

     if (basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nDelay, sizeof(data.nDelay)) < 0)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, dOffset);
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
    return ERR_OK;
}

OOP_DAR_E data_to_buf_evt_cfg_dd(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_EVTCFG_DD_T data = {0};
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data)); 

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    PRTL_FMT_DEBUG("data_to_buf_evt_cfg_dd begin.\n");
    //结构数据类型
    dstBuf[dOffset++] = DT_STRUCTURE;
    dstBuf[dOffset++] = 0x02;

    if (basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dwValue, sizeof(data.dwValue)) < 0)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, dOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    if (basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nDelay, sizeof(data.nDelay)) < 0)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, dOffset);
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
    return ERR_OK;
}

OOP_DAR_E data_to_buf_evt_cfg_lu(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_CFG_LU_T data = {0};
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data)); 

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    PRTL_FMT_DEBUG("data_to_buf_evt_cfg_lu begin.\n");
    //结构数据类型
    dstBuf[dOffset++] = DT_STRUCTURE;
    dstBuf[dOffset++] = 0x02;

    if (basic_data_to_buf(E_LONG, dstBuf, dstLen, &dOffset, &data.nShort, sizeof(data.nShort)) < 0)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, dOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    if (basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nByte, sizeof(data.nByte)) < 0)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, dOffset);
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
    return ERR_OK;
}

OOP_DAR_E data_to_buf_evt_oads(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_OADS_T data = {0};
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data)); 

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    PRTL_FMT_DEBUG("data_to_buf_evt_oads begin.\n");
    //结构数据类型
    dstBuf[dOffset++] = DT_ARRAY;
    dstBuf[dOffset++] = data.nNum;

    for(uint32 i =0; i < data.nNum; i++)
    {
        if (basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.oad[i], sizeof(data.oad[i])) < 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, dOffset);
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
     return ERR_OK;

}

/**
*********************************************************************
* @brief：      标准事件记录单元
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_std(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL) )
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_std param err\n");
        return DATA_OTHER_REASON;
    }

    if (pOadInfo->pTab->eData != E_OOP_EVENT_T)
    {
         PRTL_FMT_DEBUG("data_to_buf_evt_std check data type failed. expect: E_OOP_EVENT_T<->value: %d\n", pOadInfo->pTab->eData);
         return DATA_TYPE_UNMATCHED;
    }
    
    OOP_EVENT_T oopLog;

    if (srcLen != sizeof(oopLog))
    {
         PRTL_FMT_DEBUG("data_to_buf_evt_std check data len failed.");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&oopLog, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    dstBufTmp[offset++] = 5 + oopLog.data.oadNum;

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = oopLog.nIndex;
    evtBasic.tmStart = oopLog.tmStart;
    evtBasic.tmEnd = oopLog.tmEnd;
    evtBasic.source = oopLog.source;
    evtBasic.status = oopLog.state;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/

    //关联对象属性数据
    dar = data_to_buf_evt_coload(pOadInfo, dstBufTmp, dstLen-offset, &offset, &oopLog.data, sizeof(oopLog.data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_coload failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    *pOffset = offset;

    if(offset > dstLen)
    {
        PRTL_FMT_DEBUG("fatal error. dstLen(%d), offset(%d)\n", dstLen, offset);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;       
    }

    return ERR_OK;
}

/**
*********************************************************************
* @brief：     数据中心格式转698报文，通信模块参数变更事件记录单元
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_modulelog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_modulelog param err\n");
        return DATA_OTHER_REASON;
    }

    if (pOadInfo->pTab->eData != E_OOP_MODULELOG_T)
    {
         PRTL_FMT_DEBUG("check data type failed.\n");
         return DATA_TYPE_UNMATCHED;
    }

    OOP_MODULELOG_T oopModuleLog;

    if (srcLen != sizeof(oopModuleLog))
    {
         PRTL_FMT_DEBUG("check data len failed.\n");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&oopModuleLog, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE; 

    //结构成员个数
    dstBufTmp[offset++] = 8;    

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = oopModuleLog.nIndex;
    evtBasic.tmStart = oopModuleLog.tmStart;
    evtBasic.tmEnd = oopModuleLog.tmEnd;
    evtBasic.source = oopModuleLog.source;
    evtBasic.status = oopModuleLog.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/
    //模块宿主的通信地址
    dar = basic_data_to_buf(E_OOP_OCTETVAR16_T, dstBufTmp, dstLen-offset, &offset, &oopModuleLog.add, sizeof(oopModuleLog.add));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //变更前的模块描述符
    if(dstLen-offset < oopModuleLog.preID.nNum + 2)
    {
        PRTL_FMT_DEBUG("data_to_buf_modulelog buf is small ieft %d, need %d\n", dstLen-offset, oopModuleLog.preID.nNum + 2);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }
    dstBufTmp[offset++] = DT_VISIBLE_STRING;
    dstBufTmp[offset++] = oopModuleLog.preID.nNum;
    if(oopModuleLog.preID.nNum > 0)
    {
        memcpy(dstBufTmp + offset, oopModuleLog.preID.value, oopModuleLog.preID.nNum);
        offset += oopModuleLog.preID.nNum;
    }

    //变更后的模块描述符
    if(dstLen-offset < oopModuleLog.curID.nNum + 2)
    {
        PRTL_FMT_DEBUG("data_to_buf_modulelog buf is small ieft %d, need %d\n", dstLen-offset, oopModuleLog.curID.nNum + 2);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }
    dstBufTmp[offset++] = DT_VISIBLE_STRING;
    dstBufTmp[offset++] = oopModuleLog.curID.nNum;
    if(oopModuleLog.curID.nNum > 0)
    {
        memcpy(dstBufTmp + offset, oopModuleLog.curID.value, oopModuleLog.curID.nNum);
        offset += oopModuleLog.curID.nNum;
    }

    *pOffset = offset;
    
    return dar;
}

/**
*********************************************************************
* @brief：     数据中心格式转698报文，终端停上电事件记录单元 0x31060200
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_powerlog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_powerlog param err\n");
        return ERR_PTR;
    }

    if (pOadInfo->pTab->eData != E_OOP_POWERLOG_T)
    {
        PRTL_FMT_DEBUG("check data type failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    OOP_POWERLOG_T oopLog;

    if (srcLen != sizeof(oopLog))
    {
        PRTL_FMT_DEBUG("check data len failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&oopLog, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE;

    //结构成员个数
    dstBufTmp[offset++] = 6 + oopLog.data.oadNum;

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = oopLog.nIndex;
    evtBasic.tmStart = oopLog.tmStart;
    evtBasic.tmEnd = oopLog.tmEnd;
    evtBasic.source = oopLog.source;
    evtBasic.status = oopLog.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/
    //属性标志 bit-string(SIZE(8))
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBufTmp, dstLen, &offset, &oopLog.flag, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //关联对象属性数据
    dar = data_to_buf_evt_coload(pOadInfo, dstBufTmp, dstLen-offset, &offset, &oopLog.data, sizeof(oopLog.data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @brief：      发现未知电能表事件单元 0x31110200
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_smlog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 offlen = 0; //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码    
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    uint32 i = 0;

    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_smlog param err\n");
        return ERR_PTR;
    }

    if (pOadInfo->pTab->eData != E_OOP_SMLOG_T)
    {
        PRTL_FMT_DEBUG("check data type failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    OOP_SMLOG_T oopLog;

    if (srcLen != sizeof(oopLog))
    {
        PRTL_FMT_DEBUG("check data len failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&oopLog, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE;

    //结构成员个数
    dstBufTmp[offset++] = 6;    

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = oopLog.nIndex;
    evtBasic.tmStart = oopLog.tmStart;
    evtBasic.tmEnd = oopLog.tmEnd;
    evtBasic.source = oopLog.source;
    evtBasic.status = oopLog.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/

    //搜表结果
    dstBufTmp[offset++] = DT_ARRAY;

    //数组成员个数
    offlen = set_len_offset(oopLog.nResNum, lenBuf);
    memcpy(&dstBufTmp[offset], lenBuf, offlen);
    offset += offlen;

    for(i = 0; i < oopLog.nResNum; i++)
    {
        //一个搜表结果
        dar = data_to_buf_search_met_res(pOadInfo, dstBufTmp, dstLen-offset, &offset, &oopLog.Res[i], sizeof(OOP_SEARCHMET_RES_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("[%d]data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", i, dar, offset);
            return DATA_TYPE_UNMATCHED;
        }
    }

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @brief：      电能表时钟超差记录单元 0x31050200
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_clockflylog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_clockflylog param err\n");
        return ERR_PTR;
    }

    if (pOadInfo->pTab->eData != E_OOP_CLOCKFLYLOG_T)
    {
        PRTL_FMT_DEBUG("check data type failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    OOP_CLOCKFLYLOG_T oopLog;

    if (srcLen != sizeof(oopLog))
    {
        PRTL_FMT_DEBUG("check data len failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&oopLog, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE;

    //结构成员个数
    dstBufTmp[offset++] = 7 + oopLog.data.oadNum;
  
    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = oopLog.nIndex;
    evtBasic.tmStart = oopLog.tmStart;
    evtBasic.tmEnd = oopLog.tmEnd;
    evtBasic.source = oopLog.source;
    evtBasic.status = oopLog.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/
    //电能表时钟
    dar = basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen-offset, &offset, &oopLog.clkMeter, sizeof(oopLog.clkMeter));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //终端当前时钟
    dar = basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen-offset, &offset, &oopLog.clkTerm, sizeof(oopLog.clkTerm));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //关联对象属性数据
    dar = data_to_buf_evt_coload(pOadInfo, dstBufTmp, dstLen-offset, &offset, &oopLog.data, sizeof(oopLog.data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @brief：      电能表数据变更监控记录单元 0x311c0200
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_meterdirtylog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 dOffset = *pOffset;
    uint32 sOffset = 0;
    uint32 presOffset = 0;
    uint32 aftsOffset = 0; 
    uint8 *srcBufTmp = (uint8 *)srcData;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    uint32 i = 0;
    IC_OAD_INFO_T icInfo;
    DATA_CONVERT_T convert;

    MEMZERO(&convert, sizeof(DATA_CONVERT_T));

    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_meterdirtylog param err\n");
        return ERR_PTR;
    }

    if (pOadInfo->pTab->eData != E_OOP_METERDIRTYLOG_T)
    {
        PRTL_FMT_DEBUG("check data type failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    OOP_METERDIRTYLOG_T oopLog;

    if (srcLen != sizeof(oopLog))
    {
        PRTL_FMT_DEBUG("check data len failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&oopLog, &srcBufTmp[sOffset], srcLen-sOffset);
    sOffset += sizeof(oopLog);

    //结构数据类型
    dstBufTmp[dOffset++] = DT_STRUCTURE;

    //结构成员个数
    dstBufTmp[dOffset++] = 8;
    
    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = oopLog.nIndex;
    evtBasic.tmStart = oopLog.tmStart;
    evtBasic.tmEnd = oopLog.tmEnd;
    evtBasic.source = oopLog.source;
    evtBasic.status = oopLog.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen, &dOffset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, dOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/
    //监控数据对象
    dar = basic_data_to_buf(E_CSD, dstBufTmp, dstLen, &dOffset, &oopLog.di, sizeof(oopLog.di));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, dOffset);
        return DATA_TYPE_UNMATCHED;
    }

    icInfo.choice = 0;
    
    convert.dstBuf = dstBuf;
    convert.dstLen = dstLen;
    convert.dOffset = &dOffset;

    //根据CSD不同来列
    if(oopLog.di.choice == 0) //单OAD
    {
        icInfo.oad.value = oopLog.di.oad.value;

        if(icInfo.oad.nIndex != 0) //约定关联OAD的分量取子结构存储
        {
            icInfo.subParse = TRUE;
        }
        
        convert.srcBuf = oopLog.pre;
        convert.srcLen = oopLog.preLen;
        convert.sOffset = &presOffset;
        
        //变化前数据
        dar = normal_data_to_fmt698(&icInfo, &convert);
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("normal_data2fmt698 failed. dar(%d), oda(0x%08x)\n", dar, icInfo.oad.value);
            return dar;
        }

        convert.srcBuf = oopLog.aft;
        convert.srcLen = oopLog.aftLen;
        convert.sOffset = &aftsOffset;        
        
        //变化后数据
        dar = normal_data_to_fmt698(&icInfo, &convert);         
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("normal_data2fmt698 failed. dar(%d), oda(0x%08x)\n", dar, icInfo.oad.value);
            return dar;
        }

    }
    else if(oopLog.di.choice == 1) //记录型OAD //TODO pre和aft偏移不对
    {
        for(i = 0; i < oopLog.di.road.oadCols.nNum; i++)
        {
        
            icInfo.oad.value = oopLog.di.road.oadCols.oad[i].value;

            if(icInfo.oad.nIndex != 0) //约定关联OAD的分量取子结构存储
            {
                icInfo.subParse = TRUE;
            }
            
            convert.srcBuf = oopLog.pre;
            convert.srcLen = oopLog.preLen;
            convert.sOffset = &presOffset;

            //变化前数据
            dar = normal_data_to_fmt698(&icInfo, &convert);
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("normal_data2fmt698 failed. oda(0x%08x)\n", icInfo.oad.value);
                return dar;
            }

            convert.srcBuf = oopLog.aft;
            convert.srcLen = oopLog.aftLen;
            convert.sOffset = &aftsOffset; 

            //变化后数据
            dar = normal_data_to_fmt698(&icInfo, &convert);         
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("normal_data2fmt698 failed. oda(0x%08x)\n", icInfo.oad.value);
                return dar;
            }           
        }
    }
    else
    {
        PRTL_FMT_DEBUG("error choice(%d)\n", oopLog.di.choice);
        return DATA_TYPE_UNMATCHED;
    }

    *pOffset = dOffset;

    return ERR_OK;
}

/**
*********************************************************************
* @brief：      编程事件记录单元
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_prog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL) )
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_prog param err\n");
        return DATA_OTHER_REASON;
    }

    if (pOadInfo->pTab->eData != E_OOP_PROGRAMLOG_T)
    {
         PRTL_FMT_DEBUG("data_to_buf_evt_std check data type failed. expect: E_OOP_PROGRAMLOG_T(%d)<->value: %d\n", E_OOP_PROGRAMLOG_T, pOadInfo->pTab->eData);
         return DATA_TYPE_UNMATCHED;
    }
    
    OOP_PROGRAMLOG_T oopLog;

    if (srcLen != sizeof(oopLog))
    {
         PRTL_FMT_DEBUG("data_to_buf_evt_std check data len failed.");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&oopLog, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    dstBufTmp[offset++] = 6 + oopLog.data.oadNum;

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = oopLog.nIndex;
    evtBasic.tmStart = oopLog.tmStart;
    evtBasic.tmEnd = oopLog.tmEnd;
    evtBasic.source = oopLog.source;
    evtBasic.status = oopLog.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/

    //编程对象列表
    dar = data_to_buf_evt_proglist(pOadInfo, dstBufTmp, dstLen, &offset, &oopLog, sizeof(oopLog));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_proglist failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //关联对象属性数据
    dar = data_to_buf_evt_coload(pOadInfo, dstBufTmp, dstLen, &offset, &oopLog.data, sizeof(oopLog.data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_coload failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    *pOffset = offset;

    if(offset > dstLen)
    {
        PRTL_FMT_DEBUG("fatal error. dstLen(%d), offset(%d)\n", dstLen, offset);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;       
    }

    return ERR_OK;
}

/**
*********************************************************************
* @brief：     数据中心格式转698报文，安全事件变更记录单元
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_securitylog_21(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_SECURITYLOG_T data;

    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_securitylog param err\n");
        return DATA_OTHER_REASON;
    }

    if (pOadInfo->pTab->eData != E_OOP_SECURITYLOG_T)
    {
         PRTL_FMT_DEBUG("check data type failed.\n");
         return DATA_TYPE_UNMATCHED;
    }

    if (srcLen != sizeof(data))
    {
         PRTL_FMT_DEBUG("check data len failed.\n");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&data, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE; 

    //结构成员个数
    dstBufTmp[offset++] = 16;

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = data.nIndex;
    evtBasic.tmStart = data.tmStart;
    evtBasic.tmEnd = data.tmEnd;
    evtBasic.source = data.source;
    evtBasic.status = data.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

#if 0
    //事件记录序号 20220200
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &data.nIndex, sizeof(data.nIndex));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //事件发生时间 201E0200
    dar = basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen, &offset, &data.tmStart, sizeof(data.tmStart));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }    

    //事件结束时间 20200200
    dar = basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen, &offset, &data.tmEnd, sizeof(data.tmEnd));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    } 

    //TODO 事件发生源和上报状态暂时不召测
#endif

    //*********************************************以下为不同事件的特殊数据******************************************/
    //异常标志
    dar = basic_data_to_buf(E_OOP_BITSTR16_T, dstBufTmp, dstLen, &offset, &data.flag, sizeof(OOP_BITSTR16_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //以太网远程端口开放信息
    dar = data_to_buf_evt_ethport(pOadInfo, dstBufTmp, dstLen, &offset, &data, sizeof(data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //以太网外联信息
    dar = data_to_buf_evt_ethconnect(pOadInfo, dstBufTmp, dstLen, &offset, &data, sizeof(data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //通信模块端口开放信息
    dar = data_to_buf_evt_lteport(pOadInfo, dstBufTmp, dstLen, &offset, &data, sizeof(data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //通信模块外联信息
    dar = data_to_buf_evt_lteconnect(pOadInfo, dstBufTmp, dstLen, &offset, &data, sizeof(data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //USB设备接入信息
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBufTmp, dstLen, &offset, &data.usbInfo, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //本地维护端口接入信息
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBufTmp, dstLen, &offset, &data.rs232Info, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //关键文件目录变更信息
    dar = data_to_buf_evt_chgpath(pOadInfo, dstBufTmp, dstLen, &offset, &data, sizeof(data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //以太网登录控制台信息
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBufTmp, dstLen, &offset, &data.ethFlag, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //串口登录控制台信息
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBufTmp, dstLen, &offset, &data.rs232Flag, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //非法操作命令信息
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBufTmp, dstLen, &offset, &data.errFlag, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    *pOffset = offset;
    
    return dar;
}

/**********************************************************
函数功能      :commfun_buf_is_null
函数功能      :判断缓存是否空
函数输入值:无
函数返回值:无
*************************************************************/
BOOL commfun_buf_is_null(uint8 *buf, uint32 len)
{
    int i = 0;
    
    if ((buf == NULL) || (len == 0))
    {
        return TRUE;
    }

    for (i = 0; i < len; i++)
    {
        if (buf[i] != 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}

/**
*********************************************************************
* @brief：       通信模块外联信息
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_security_session(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_SECURITY_SESSION_T data;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&data, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;

    //结构成员数
    dstBufTmp[offset++] = 7;

    dar = basic_data_to_buf(E_OOP_OCTETVAR16_T, dstBufTmp, dstLen, &offset, &data.ipRemote, 
        sizeof(data.ipRemote));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &data.portRemote, 
        sizeof(data.portRemote));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_OOP_OCTETVAR16_T, dstBufTmp, dstLen, &offset, &data.ipLocal, 
        sizeof(data.ipLocal));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &data.portLocal, 
        sizeof(data.portLocal));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBufTmp, dstLen, &offset, &data.proto, 
        sizeof(data.proto));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBufTmp, dstLen, &offset, &data.prog, 
        sizeof(data.prog));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen, &offset, &data.progType, 
        sizeof(data.progType));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }     
    
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       通信模块外联信息
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_security_stream(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_SECURITY_STREAM_T data;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&data, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;

    //结构成员数
    dstBufTmp[offset++] = 8;

    dar = basic_data_to_buf(E_OOP_OCTETVAR16_T, dstBufTmp, dstLen, &offset, &data.ipRemote, 
        sizeof(data.ipRemote));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &data.portRemote, 
        sizeof(data.portRemote));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_OOP_OCTETVAR16_T, dstBufTmp, dstLen, &offset, &data.ipLocal, 
        sizeof(data.ipLocal));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &data.portLocal, 
        sizeof(data.portLocal));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBufTmp, dstLen, &offset, &data.proto, 
        sizeof(data.proto));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBufTmp, dstLen, &offset, &data.prog, 
        sizeof(data.prog));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &data.streamLimit, 
        sizeof(data.streamLimit));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &data.streamCur, 
        sizeof(data.streamCur));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }    
    
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       通信模块外联信息
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_security_prog(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_SECURITY_PROG_T data;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&data, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;

    //结构成员数
    dstBufTmp[offset++] = 6;

    dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen, &offset, &data.type, 
        sizeof(data.type));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen, &offset, &data.dtStart, 
        sizeof(data.dtStart));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBufTmp, dstLen, &offset, &data.prog, 
        sizeof(data.prog));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBufTmp, dstLen, &offset, &data.progUser, 
        sizeof(data.progUser));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBufTmp, dstLen, &offset, &data.progGroup, 
        sizeof(data.progGroup));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBufTmp, dstLen, &offset, &data.progCmd, 
        sizeof(data.progCmd));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    } 
    
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       通信模块外联信息
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_security_login(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_SECURITY_LOGIN_T data;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&data, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;

    //结构成员数
    dstBufTmp[offset++] = 3;

    dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen, &offset, &data.type, 
        sizeof(data.type));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBufTmp, dstLen, &offset, &data.user, 
        sizeof(data.user));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBufTmp, dstLen, &offset, &data.source, 
        sizeof(data.source));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：     数据中心格式转698报文，安全事件变更记录单元
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_securitylog_22(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_SECURITYLOG_T data;
    uint8 errFLag[2] = {0}; //TODO bitstring暂时特殊处理下，统一修改影响太多 2022.07.22 by hening

    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_securitylog param err\n");
        return DATA_OTHER_REASON;
    }

    if (pOadInfo->pTab->eData != E_OOP_SECURITYLOG_T)
    {
         PRTL_FMT_DEBUG("check data type failed.\n");
         return DATA_TYPE_UNMATCHED;
    }

    if (srcLen != sizeof(data))
    {
         PRTL_FMT_DEBUG("check data len failed.\n");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&data, srcData, srcLen);
    memcpy_r(errFLag, data.flag.nValue, 2);
    
    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE; 

    //结构成员个数
    dstBufTmp[offset++] = 21;

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = data.nIndex;
    evtBasic.tmStart = data.tmStart;
    evtBasic.tmEnd = data.tmEnd;
    evtBasic.source = data.source;
    evtBasic.status = data.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

#if 0
    //事件记录序号 20220200
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &data.nIndex, sizeof(data.nIndex));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //事件发生时间 201E0200
    dar = basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen, &offset, &data.tmStart, sizeof(data.tmStart));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }    

    //事件结束时间 20200200
    dar = basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen, &offset, &data.tmEnd, sizeof(data.tmEnd));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    } 

    //TODO 事件发生源和上报状态暂时不召测
#endif

    //*********************************************以下为不同事件的特殊数据******************************************/
    //异常标志
    dar = basic_data_to_buf(E_OOP_BITSTR16_T, dstBufTmp, dstLen, &offset, &data.flag, sizeof(OOP_BITSTR16_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //以太网远程端口开放信息
    dar = data_to_buf_evt_ethport(pOadInfo, dstBufTmp, dstLen, &offset, &data, sizeof(data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //以太网外联信息
    dar = data_to_buf_evt_ethconnect(pOadInfo, dstBufTmp, dstLen, &offset, &data, sizeof(data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //通信模块端口开放信息
    dar = data_to_buf_evt_lteport(pOadInfo, dstBufTmp, dstLen, &offset, &data, sizeof(data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //通信模块外联信息
    dar = data_to_buf_evt_lteconnect(pOadInfo, dstBufTmp, dstLen, &offset, &data, sizeof(data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //USB设备接入信息
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBufTmp, dstLen, &offset, &data.usbInfo, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //本地维护端口接入信息
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBufTmp, dstLen, &offset, &data.rs232Info, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //关键文件目录变更信息
    dar = data_to_buf_evt_chgpath(pOadInfo, dstBufTmp, dstLen, &offset, &data, sizeof(data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //以太网登录控制台信息
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBufTmp, dstLen, &offset, &data.ethFlag, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //串口登录控制台信息
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBufTmp, dstLen, &offset, &data.rs232Flag, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //非法操作命令信息
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBufTmp, dstLen, &offset, &data.errFlag, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //SSH服务异常
    dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen, &offset, &data.errSSH, sizeof(data.errSSH));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    if(get_bit_value(errFLag, 16, 12))
    {
        dar = data_to_buf_security_session(pOadInfo, dstBufTmp, dstLen, &offset, &data.errSession, sizeof(data.errSession));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        }
    }
    else
    {
        dstBufTmp[offset++] = 0x00;
    }

    if(get_bit_value(errFLag, 16, 13))
    {
        dar = data_to_buf_security_stream(pOadInfo, dstBufTmp, dstLen, &offset, &data.errStream, sizeof(data.errStream));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        }
    }
    else
    {
        dstBufTmp[offset++] = 0x00;
    }

    if(get_bit_value(errFLag, 16, 14))
    {
        dar = data_to_buf_security_prog(pOadInfo, dstBufTmp, dstLen, &offset, &data.errProg, sizeof(data.errProg));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        }
    }
    else
    {
        dstBufTmp[offset++] = 0x00;
    }

    if(get_bit_value(errFLag, 16, 15))
    {
        dar = data_to_buf_security_login(pOadInfo, dstBufTmp, dstLen, &offset, &data.errLogin, sizeof(data.errLogin));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        }
    }
    else
    {
        dstBufTmp[offset++] = 0x00;
    }

    *pOffset = offset;
    
    return dar;
}

/**
*********************************************************************
* @brief：     数据中心格式转698报文，安全事件变更记录单元
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_securitylog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    #if SEC_MONITOR_NEW_22 == 1
    return data_to_buf_securitylog_22(pOadInfo, dstBuf, dstLen, pOffset, srcData, srcLen);
    #else
    return data_to_buf_securitylog_21(pOadInfo, dstBuf, dstLen, pOffset, srcData, srcLen);
    #endif
}

/**
*********************************************************************
* @brief：     数据中心格式转698报文，遥控事件记录单元
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_rctrllog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_RCTRLLOG_T event;

    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_rctrllog param err\n");
        return DATA_OTHER_REASON;
    }

    if (pOadInfo->pTab->eData != E_OOP_RCTRLLOG_T)
    {
         PRTL_FMT_DEBUG("check data type failed.\n");
         return DATA_TYPE_UNMATCHED;
    }

    if (srcLen != sizeof(event))
    {
         PRTL_FMT_DEBUG("check data len failed.\n");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&event, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE; 

    //结构成员个数
    dstBufTmp[offset++] = 8;

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = event.nIndex;
    evtBasic.tmStart = event.tmStart;
    evtBasic.tmEnd = event.tmEnd;
    evtBasic.source = event.source;
    evtBasic.status = event.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/

	/*遥控命令类型*/
    dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &event.cmd, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), event.cmd(%d)\n", dar, event.cmd);
        return dar;
    }

    //控后2分钟总加组有功功率 0x330A0207
    dar = data_to_buf_evt_clrttgpower(pOadInfo, dstBufTmp, dstLen, &offset, &event.prePower, sizeof(event.prePower));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //控时2分钟总加组有功功率 0x330A0208
    dar = data_to_buf_evt_clrttgpower(pOadInfo, dstBufTmp, dstLen, &offset, &event.llPower, sizeof(event.llPower));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    *pOffset = offset;
    
    return dar;
}

/**
*********************************************************************
* @brief：     数据中心格式转698报文，功控事件记录单元
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_pctrllog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_PCTRLLOG_T event;

    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_pctrllog param err\n");
        return DATA_OTHER_REASON;
    }

    if (pOadInfo->pTab->eData != E_OOP_PCTRLLOG_T)
    {
         PRTL_FMT_DEBUG("check data type failed.\n");
         return DATA_TYPE_UNMATCHED;
    }

    if (srcLen != sizeof(event))
    {
         PRTL_FMT_DEBUG("check data len failed.\n");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&event, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE; 

    //结构成员个数
    dstBufTmp[offset++] = 10;

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = event.nIndex;
    evtBasic.tmStart = event.tmStart;
    evtBasic.tmEnd = event.tmEnd;
    evtBasic.source = event.source;
    evtBasic.status = event.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/

    //控后2分钟功率 0x33050206
    dar = basic_data_to_buf(E_LONG64, dstBufTmp, dstLen, &offset, &event.power2Min, sizeof(int64));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //控制对象 0x810E0200
    dar = basic_data_to_buf(E_OI, dstBufTmp, dstLen, &offset, &event.objCtrl, sizeof(OI));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //跳闸轮次 0x810F0200
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBufTmp, dstLen, &offset, &event.turnState, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //功控定值 0x33050209
    dar = basic_data_to_buf(E_LONG64, dstBufTmp, dstLen, &offset, &event.llSetting, sizeof(int64));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //跳闸发生前总加组有功功率 0x3305020A
    dar = basic_data_to_buf(E_LONG64, dstBufTmp, dstLen, &offset, &event.power, sizeof(int64));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    *pOffset = offset;
    
    return dar;
}

/**
*********************************************************************
* @brief：     数据中心格式转698报文，电控事件记录单元
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_ectrllog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_ECTRLLOG_T event;

    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_ectrllog param err\n");
        return DATA_OTHER_REASON;
    }

    if (pOadInfo->pTab->eData != E_OOP_ECTRLLOG_T)
    {
         PRTL_FMT_DEBUG("check data type failed.\n");
         return DATA_TYPE_UNMATCHED;
    }

    if (srcLen != sizeof(event))
    {
         PRTL_FMT_DEBUG("check data len failed.\n");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&event, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE; 

    //结构成员个数
    dstBufTmp[offset++] = 9;

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = event.nIndex;
    evtBasic.tmStart = event.tmStart;
    evtBasic.tmEnd = event.tmEnd;
    evtBasic.source = event.source;
    evtBasic.status = event.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/

    //控制对象 0x810E0200
    dar = basic_data_to_buf(E_OI, dstBufTmp, dstLen, &offset, &event.objCtrl, sizeof(OI));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //跳闸轮次 0x810F0200
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBufTmp, dstLen, &offset, &event.turnState, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //电控定值 0x81100200
    dar = basic_data_to_buf(E_LONG64, dstBufTmp, dstLen, &offset, &event.llSetting, sizeof(int64));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //跳闸发生时总加组电能量 0x33060209
    dar = basic_data_to_buf(E_LONG64, dstBufTmp, dstLen, &offset, &event.energy, sizeof(int64));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    *pOffset = offset;
    
    return dar;
}

/**
*********************************************************************
* @brief：     数据中心格式转698报文，电控告警记录单元
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_ewarnlog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_EWARNLOG_T event;

    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_ectrllog param err\n");
        return DATA_OTHER_REASON;
    }

    if (pOadInfo->pTab->eData != E_OOP_EWARNLOG_T)
    {
         PRTL_FMT_DEBUG("check data type failed.\n");
         return DATA_TYPE_UNMATCHED;
    }

    if (srcLen != sizeof(event))
    {
         PRTL_FMT_DEBUG("check data len failed.\n");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&event, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE; 

    //结构成员个数
    dstBufTmp[offset++] = 7;

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = event.nIndex;
    evtBasic.tmStart = event.tmStart;
    evtBasic.tmEnd = event.tmEnd;
    evtBasic.source = event.source;
    evtBasic.status = event.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/

    //控制对象 0x810E0200
    dar = basic_data_to_buf(E_OI, dstBufTmp, dstLen, &offset, &event.objCtrl, sizeof(OI));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //电控定值 0x81100200
    dar = basic_data_to_buf(E_LONG64, dstBufTmp, dstLen, &offset, &event.llSetting, sizeof(int64));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    *pOffset = offset;
    
    return dar;
}

/**
*********************************************************************
* @brief：     数据中心格式转698报文，电流互感器异常记录单元 0x31200200
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_ct_abnormal(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_powerlog param err\n");
        return ERR_PTR;
    }

    if (pOadInfo->pTab->eData != E_OOP_ABNORMALCTLOG_T)
    {
        PRTL_FMT_DEBUG("check data type failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    OOP_ABNORMALCTLOG_T oopLog;

    if (srcLen != sizeof(oopLog))
    {
        PRTL_FMT_DEBUG("check data len failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&oopLog, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE;

    //结构成员个数
    dstBufTmp[offset++] = 6;

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = oopLog.nIndex;
    evtBasic.tmStart = oopLog.tmStart;
    evtBasic.tmEnd = oopLog.tmEnd;
    evtBasic.source = oopLog.source;
    evtBasic.status = oopLog.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/

    //异常标志 bit-string(SIZE(8))
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBufTmp, dstLen, &offset, &oopLog.abnmlflg, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @brief：     数据中心格式转698报文，终端对电表校时记录单元 0x311B0200
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_p2pclock(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_p2pclock param err\n");
        return ERR_PTR;
    }

    if (pOadInfo->pTab->eData != E_OOP_P2PCLOCKLOG_T)
    {
        PRTL_FMT_DEBUG("check data type failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    OOP_P2PCLOCKLOG_T p2pclock;
    memset(&p2pclock,0x00,sizeof(OOP_P2PCLOCKLOG_T));

    if (srcLen != sizeof(p2pclock))
    {
        PRTL_FMT_DEBUG("check data len failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&p2pclock, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE;

    //结构成员个数
    dstBufTmp[offset++] = 7;

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = p2pclock.nIndex;
    evtBasic.tmStart = p2pclock.tmStart;
    evtBasic.tmEnd = p2pclock.tmEnd;
    evtBasic.source = p2pclock.source;
    evtBasic.status = p2pclock.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/

    dar = basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen, &offset, &p2pclock.preTime, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_data_to_buf(E_INTEGER, dstBufTmp, dstLen, &offset, &p2pclock.offset, sizeof(char));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @brief：     数据中心格式转698报文，拒绝从节点入网记录单元 0x311F0200
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_refusenode(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    uint32 i;
    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_refusenode param err\n");
        return ERR_PTR;
    }

    if (pOadInfo->pTab->eData != E_OOP_REFUSENODEACCESSLOG_T)
    {
        PRTL_FMT_DEBUG("check data type failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    OOP_REFUSENODEACCESSLOG_T info;
    memset(&info,0x00,sizeof(OOP_REFUSENODEACCESSLOG_T));

    if (srcLen != sizeof(OOP_REFUSENODEACCESSLOG_T))
    {
        PRTL_FMT_DEBUG("check data len failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&info, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE;

    //结构成员个数
    dstBufTmp[offset++] = 6;

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = info.nIndex;
    evtBasic.tmStart = info.tmStart;
    evtBasic.tmEnd = info.tmEnd;
    evtBasic.source = info.source;
    evtBasic.status = info.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/
    dstBufTmp[offset++] = DT_ARRAY;
    dstBufTmp[offset++] = info.info.nNum;
    for(i=0;i<info.info.nNum;i++)
    {
        dstBufTmp[offset++] = DT_OCTET_STRING;
        dstBufTmp[offset++] = 6;
        memcpy(dstBufTmp+offset,info.info.sa[i].nValue,6);
        offset+=6;
    }

    *pOffset = offset;

    return dar;
}
/**
*********************************************************************
* @brief：     数据中心格式转698报文，跨台区电能表事件 0x336010200
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_sysmonitor(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_refusenode param err\n");
        return ERR_PTR;
    }

    if (pOadInfo->pTab->eData != E_OOP_SYSTEMMONITOR_T)
    {
        PRTL_FMT_DEBUG("check data type failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    OOP_MONITORLOG_T info;
    memset(&info,0x00,sizeof(OOP_MONITORLOG_T));

    if (srcLen != sizeof(OOP_MONITORLOG_T))
    {
        PRTL_FMT_DEBUG("check data len failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&info, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE;

    //结构成员个数
    dstBufTmp[offset++] = 11;

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = info.nIndex;
    evtBasic.tmStart = info.tmStart;
    evtBasic.tmEnd = info.tmEnd;
    evtBasic.source = info.source;
    evtBasic.status = info.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/
    
    dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &info.warn , sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &info.value , sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &info.rate , sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBufTmp, dstLen-offset, &offset, &info.containerName , sizeof(info.containerName));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBufTmp, dstLen-offset, &offset, &info.appName , sizeof(info.appName));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBufTmp, dstLen-offset, &offset, &info.serviceName , sizeof(info.serviceName));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    *pOffset = offset;

    return dar;
}


/**
*********************************************************************
* @brief：     数据中心格式转698报文，跨台区电能表事件 0x31120200
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_acrossarea(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    uint32 i;
    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_refusenode param err\n");
        return ERR_PTR;
    }

    if (pOadInfo->pTab->eData != E_OOP_UNMETERLOG_T)
    {
        PRTL_FMT_DEBUG("check data type failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    OOP_UNMETERLOG_T info;
    memset(&info,0x00,sizeof(OOP_UNMETERLOG_T));

    if (srcLen != sizeof(OOP_UNMETERLOG_T))
    {
        PRTL_FMT_DEBUG("check data len failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&info, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE;

    //结构成员个数
    dstBufTmp[offset++] = 6;

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = info.nIndex;
    evtBasic.tmStart = info.tmStart;
    evtBasic.tmEnd = info.tmEnd;
    evtBasic.source = info.source;
    evtBasic.status = info.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/
    dstBufTmp[offset++] = DT_ARRAY;
    dstBufTmp[offset++] = info.nResNum;
    for(i=0;i<info.nResNum;i++)
    {
        dstBufTmp[offset++] = DT_STRUCTURE;
        dstBufTmp[offset++] = 3;
        
        dar = basic_data_to_buf(E_TSA, dstBufTmp, dstLen-offset, &offset, &info.Res[i].commAdd , sizeof(OOP_TSA_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        }
        dar = basic_data_to_buf(E_TSA, dstBufTmp, dstLen-offset, &offset, &info.Res[i].netAdd , sizeof(OOP_TSA_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        }
        dar = basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen-offset, &offset, &info.Res[i].change , sizeof(OOP_DATETIME_S_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        }
    }

    *pOffset = offset;

    return dar;
}


/**
*********************************************************************
* @brief：     数据中心格式转698报文，跨台区电能表事件 0x31120200
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_identify(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    uint32 i;
    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_refusenode param err\n");
        return ERR_PTR;
    }

    if (pOadInfo->pTab->eData != E_OOP_IdentifyResult_T)
    {
        PRTL_FMT_DEBUG("check data type failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    OOP_IdentifyResult_T info;
    memset(&info,0x00,sizeof(OOP_IdentifyResult_T));

    if (srcLen != sizeof(OOP_IdentifyResult_T))
    {
        PRTL_FMT_DEBUG("check data len failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&info, srcData, srcLen);
    EVENT_FMT_DEBUG("Identifynum=%d, signalphase=%d \n", 
            info.IdentifyInformations.Identifynum, info.IdentifyInformations.IdentifyInformation[0].signalphase);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE;

    //结构成员个数
    dstBufTmp[offset++] = 6;

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = info.nIndex;
    evtBasic.tmStart = info.tmStart;
    evtBasic.tmEnd = info.tmEnd;
    evtBasic.source = info.source;
    evtBasic.status = info.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/
    dstBufTmp[offset++] = DT_ARRAY;
    dstBufTmp[offset++] = info.IdentifyInformations.Identifynum;
    EVENT_FMT_DEBUG("Identifynum=%d, signalphase=%d \n", 
                info.IdentifyInformations.Identifynum, info.IdentifyInformations.IdentifyInformation[0].signalphase);

    for(i=0;i<info.IdentifyInformations.Identifynum;i++)
    {
        dstBufTmp[offset++] = DT_STRUCTURE;
        dstBufTmp[offset++] = 7;
        
        dar = basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen-offset, &offset, &info.IdentifyInformations.IdentifyInformation[i].Suctime , sizeof(OOP_DATETIME_S_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        }
//        dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &info.IdentifyInformations.IdentifyInformation[i].signalstatus , sizeof(uint8));
//        if(dar != DATA_SUCCESS)
//        {
//            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
//            return DATA_TYPE_UNMATCHED;
//        }
        dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &info.IdentifyInformations.IdentifyInformation[i].signalphase , sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        }
        dar = basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, &info.IdentifyInformations.IdentifyInformation[i].Signalstrength1 , sizeof(int32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        }
        dar = basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, &info.IdentifyInformations.IdentifyInformation[i].Signalstrength2 , sizeof(int32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        }
//        dar = basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, &info.IdentifyInformations.IdentifyInformation[i].BaseStrength , sizeof(int32));
//        if(dar != DATA_SUCCESS)
//        {
//            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
//            return DATA_TYPE_UNMATCHED;
//        }
        dar = basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, &info.IdentifyInformations.IdentifyInformation[i].BackgroundNoise , sizeof(int32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        }
        dar = basic_data_to_buf(E_OOP_BITSTR16_T, dstBufTmp, dstLen-offset, &offset, &info.IdentifyInformations.IdentifyInformation[i].Signatureinformation , sizeof(OOP_BITSTR16_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        } 
        dar = basic_data_to_buf(E_OOP_OCTETVAR16_T, dstBufTmp, dstLen-offset, &offset, &info.IdentifyInformations.IdentifyInformation[i].Extendedinformation , sizeof(OOP_OCTETVAR16_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;
        }
    }

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @brief：     数据中心格式转698报文，保护功能投退事件 0x35050200
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_pf_input_exit(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
    PRTL_FMT_DEBUG("data_to_buf_p2pclock param err\n");
    return ERR_PTR;
    }
    
    if (pOadInfo->pTab->eData != E_OOP_P2PCLOCKLOG_T)
    {
    PRTL_FMT_DEBUG("check data type failed.\n");
    return DATA_TYPE_UNMATCHED;
    }
    
    OOP_PF_INPUT_EXIT_T p2pprofun;
    memset(&p2pprofun,0x00,sizeof(OOP_PF_INPUT_EXIT_T));
    
    if (srcLen != sizeof(p2pprofun))
    {
    PRTL_FMT_DEBUG("check data len failed.\n");
    return DATA_TYPE_UNMATCHED;
    }
    
    memcpy(&p2pprofun, srcData, srcLen);
    
    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员个数
    dstBufTmp[offset++] = 10;
    
    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = p2pprofun.nIndex;
    evtBasic.tmStart = p2pprofun.tmStart;
    evtBasic.tmEnd = p2pprofun.tmEnd;
    evtBasic.source = p2pprofun.source;
    evtBasic.status = p2pprofun.evtState;
    
    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
    PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
    return DATA_TYPE_UNMATCHED;
    }
    //*********************************************以下为不同事件的特殊数据******************************************/
    dar = basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &p2pprofun.actCase, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    dar = apdu_variant_tofmt698(dstBufTmp, dstLen-offset, &offset, &p2pprofun.constBef,sizeof(OOP_VARIANT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(dstBufTmp, dstLen, "apdu_variant_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    dar = apdu_variant_tofmt698(dstBufTmp, dstLen-offset, &offset, &p2pprofun.constCur,sizeof(OOP_VARIANT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(dstBufTmp, dstLen, "apdu_variant_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &p2pprofun.prodroType, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    dar = data_to_buf_evt_coload(pOadInfo, dstBufTmp, dstLen-offset, &offset, &p2pprofun.data, sizeof(p2pprofun.data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_coload failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @brief：      标准事件记录单元
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evtstat(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;

    uint32     i = 0;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;
    uint8 index = 0;
    EVTSTATE *data = NULL;
         
    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_evtstat error, ERR_PTR\n");
        return ERR_PTR;
    }


    if (pOadInfo->pTab->eData != E_OOP_EVTSTAT_T)
    {
         PRTL_FMT_DEBUG("data_to_buf_evtstat check data type failed. expect: E_OOP_EVTSTAT_T<->value: %d\n", pOadInfo->pTab->eData);
         return DATA_TYPE_UNMATCHED;
    }
    
    OOP_EVTSTAT_T evtstat;

    if (dsdata->srcLen != sizeof(evtstat))
    {
         PRTL_FMT_DEBUG("data_to_buf_evtstat check data len failed.");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&evtstat, srcDataTmp, dsdata->srcLen);

    if(pOadInfo->nIndex == 0)
    {
        dstBufTmp[offset++] = DT_ARRAY;
        dstBufTmp[offset++] = OOP_MAX_EVTSTAT;
        for(i = 0;i<OOP_MAX_EVTSTAT;i++)
        {
            if(0 == get_bit_value((uint8*)&evtstat.index,32,i))
            {
                dstBufTmp[offset++] = DT_NULL;
                continue;
            }
            
            data = &evtstat.stat[i];
            dstBufTmp[offset++] = DT_STRUCTURE;
            dstBufTmp[offset++] = 2;
            //事件发生源 
            switch(data->source.choice)
            {
                case DT_OAD:
                {
                    if(basic_data_to_buf(E_OAD, dstBufTmp, dsdata->dstLen-offset, &offset, &data->source.oad, sizeof(OOP_OAD_U)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }

                }
                    break;
                case DT_TSA:
                {
                    if(basic_data_to_buf(E_TSA, dstBufTmp, dsdata->dstLen-offset, &offset, &data->source.add , sizeof(OOP_TSA_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }

                }
                    break;
                case DT_OI:
                {                
                    if(basic_data_to_buf(E_OI, dstBufTmp, dsdata->dstLen-offset, &offset, &data->source.oi , sizeof(OI)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }

                }
                    break;
                case DT_NULL:
                default:
                {
                    dstBufTmp[offset++] = DT_NULL;
                }
                    break;
            }
            //事件发生时间统计
            dstBufTmp[offset++] = DT_STRUCTURE;
            dstBufTmp[offset++] = 2;
            //事件发生次数
            if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &data->dwCount, sizeof(uint32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }

            //事件累计时间
            if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &data->dwTimes, sizeof(uint32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
        }
    }else
    {
        index = pOadInfo->nIndex - 1;
        if(index >= OOP_MAX_EVTSTAT)
        {
            return ERR_NORMAL;
        }
        if(0 == get_bit_value((uint8*)&evtstat.index,32,index))
        {
            dstBufTmp[offset++] = DT_NULL;
            *dsdata->dOffset = offset;
        
             return ERR_OK;
        }
        data = &evtstat.stat[index];

        dstBufTmp[offset++] = DT_STRUCTURE;
        dstBufTmp[offset++] = 2;
        //事件发生源 
        switch(data->source.choice)
        {
            case DT_OAD:
            {
                if(basic_data_to_buf(E_OAD, dstBufTmp, dsdata->dstLen-offset, &offset, &data->source.oad, sizeof(OOP_OAD_U)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }

            }
                break;
            case DT_TSA:
            {
                if(basic_data_to_buf(E_TSA, dstBufTmp, dsdata->dstLen-offset, &offset, &data->source.add , sizeof(OOP_TSA_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }

            }
                break;
            case DT_OI:
            {                
                if(basic_data_to_buf(E_OI, dstBufTmp, dsdata->dstLen-offset, &offset, &data->source.oi , sizeof(OI)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }

            }
                break;
            case DT_NULL:
            default:
            {
                dstBufTmp[offset++] = DT_NULL;
            }
                break;
        }
        //事件发生时间统计
        dstBufTmp[offset++] = DT_STRUCTURE;
        dstBufTmp[offset++] = 2;
        //事件发生次数
        if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &data->dwCount, sizeof(uint32)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }

        //事件累计时间
        if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &data->dwTimes, sizeof(uint32)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }
    }
    
    *dsdata->dOffset = offset;

    return ERR_OK;
}

/**
*********************************************************************
* @brief：      标准事件记录单元
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evttime(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;

    uint32     i = 0;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;
    uint8 index = 0;
    EVTTIME *data = NULL;
         
    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_evttime error, ERR_PTR\n");
        return ERR_PTR;
    }


    if (pOadInfo->pTab->eData != E_OOP_EVTTIME_T)
    {
         PRTL_FMT_DEBUG("data_to_buf_evttime check data type failed. expect: E_OOP_EVTTIME_T<->value: %d\n", pOadInfo->pTab->eData);
         return DATA_TYPE_UNMATCHED;
    }
    
    OOP_EVTTIME_T evttime;

    if (dsdata->srcLen != sizeof(evttime))
    {
         PRTL_FMT_DEBUG("data_to_buf_evttime check data len failed.");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&evttime, srcDataTmp, dsdata->srcLen);

    if(pOadInfo->nIndex == 0)
    {
        dstBufTmp[offset++] = DT_ARRAY;
        dstBufTmp[offset++] = OOP_MAX_EVTSTAT;
        for(i = 0;i<OOP_MAX_EVTSTAT;i++)
        {
            if(0 == get_bit_value((uint8*)&evttime.index,32,i))
            {
                dstBufTmp[offset++] = DT_NULL;
                continue;
            }
            
            data = &evttime.stat[i];
            dstBufTmp[offset++] = DT_STRUCTURE;
            dstBufTmp[offset++] = 2;
            //事件发生源 
            switch(data->source.choice)
            {
                case DT_OAD:
                {
                    if(basic_data_to_buf(E_OAD, dstBufTmp, dsdata->dstLen-offset, &offset, &data->source.oad, sizeof(OOP_OAD_U)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }
        
                }
                    break;
                case DT_TSA:
                {
                    if(basic_data_to_buf(E_TSA, dstBufTmp, dsdata->dstLen-offset, &offset, &data->source.add , sizeof(OOP_TSA_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }
        
                }
                    break;
                case DT_OI:
                {                
                    if(basic_data_to_buf(E_OI, dstBufTmp, dsdata->dstLen-offset, &offset, &data->source.oi , sizeof(OI)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }
        
                }
                    break;
                case DT_NULL:
                default:
                {
                    dstBufTmp[offset++] = DT_NULL;
                }
                    break;
            }
            //最近一次时间
            dstBufTmp[offset++] = DT_STRUCTURE;
            dstBufTmp[offset++] = 2;
            //最近一次发生时间
            if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, dsdata->dstLen-offset, &offset, &data->tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            
            //最近一次结束时间
            if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, dsdata->dstLen-offset, &offset, &data->tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }

        }

    }else
    {
        index = pOadInfo->nIndex - 1;
        if(index >= OOP_MAX_EVTSTAT)
        {
            return ERR_NORMAL;
        }
        data = &evttime.stat[index];
        dstBufTmp[offset++] = DT_STRUCTURE;
        dstBufTmp[offset++] = 2;
        //事件发生源 
        switch(data->source.choice)
        {
            case DT_OAD:
            {
                if(basic_data_to_buf(E_OAD, dstBufTmp, dsdata->dstLen-offset, &offset, &data->source.oad, sizeof(OOP_OAD_U)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }

            }
                break;
            case DT_TSA:
            {
                if(basic_data_to_buf(E_TSA, dstBufTmp, dsdata->dstLen-offset, &offset, &data->source.add , sizeof(OOP_TSA_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }

            }
                break;
            case DT_OI:
            {                
                if(basic_data_to_buf(E_OI, dstBufTmp, dsdata->dstLen-offset, &offset, &data->source.oi , sizeof(OI)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }

            }
                break;
            case DT_NULL:
            default:
            {
                dstBufTmp[offset++] = DT_NULL;
            }
                break;
        }
        //最近一次时间
        dstBufTmp[offset++] = DT_STRUCTURE;
        dstBufTmp[offset++] = 2;
        //最近一次发生时间
        if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, dsdata->dstLen-offset, &offset, &data->tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }

        //最近一次结束时间
        if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, dsdata->dstLen-offset, &offset, &data->tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }
    }
    
    *dsdata->dOffset = offset;

    return ERR_OK;
}

#endif


#if DESC("数据结构转换D2B&B2D",1)

/**
*********************************************************************
* @brief：      从数据中心的数据转698格式报文(class7)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class7(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
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
        case E_ENUM:                            /**< uint8              */
        {     
            PRTL_FMT_DEBUG("E_ENUM begin.\n");

            dstBuf[dOffset++] = DT_ENUM;
            memcpy_rev(&dstBuf[dOffset++], &srcBuf[sOffset], sizeof(uint8));
            sOffset += 1;

            *convert->dOffset = dOffset;
            *convert->sOffset = sOffset;
            dar = ERR_OK;   
        }
        break;
        case E_OOP_EVTCFG_DELAY_T://
            //sOffset += sizeof(OOP_EVTCFG_DELAY_T);
            return data_to_buf_evt_cfg_delay(pOadInfo, convert);
        break;
        case E_OOP_LOAD_I0_EVECFG_T://电能表零线电流异常事件
            //sOffset += sizeof(OOP_LOAD_I0_EVECFG_T);
            return data_to_buf_evt_cfg_i0(pOadInfo, convert);
        break;
        case E_OOP_EVTCFG_DD_T://超限事件
            //sOffset += sizeof(OOP_EVTCFG_DD_T);
            return data_to_buf_evt_cfg_dd(pOadInfo, convert);
        break;
        case E_OOP_CFG_LU_T://不平衡事件
            //sOffset += sizeof(OOP_CFG_LU_T);
            return data_to_buf_evt_cfg_lu(pOadInfo, convert);
        break; 
        case E_OOP_OADS_T://一组对象属性描述符
            //sOffset += sizeof(OOP_OADS_T);
            return data_to_buf_evt_oads(pOadInfo, convert);
        break;
        case E_OOP_EVENT_T:        //标准事件记录单元
            sOffset += sizeof(OOP_EVENT_T);
            dar = data_to_buf_evt_std(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);
        break;       
        case E_OOP_MODULELOG_T:    //通信模块变更事件记录单元
            sOffset += sizeof(OOP_MODULELOG_T);
            dar = data_to_buf_modulelog(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);
        break; 
        case E_OOP_POWERLOG_T:     //终端停/上电事件记录单元
            sOffset += sizeof(OOP_POWERLOG_T);
            dar = data_to_buf_powerlog(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);
        break; 
        case E_OOP_SMLOG_T:        //发现未知电能表事件单元
            sOffset += sizeof(OOP_SMLOG_T);
            dar = data_to_buf_smlog(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);
        break; 
        case E_OOP_CLOCKFLYLOG_T:  //电能表时钟超差记录单元
            sOffset += sizeof(OOP_CLOCKFLYLOG_T);
            dar = data_to_buf_clockflylog(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);
        break; 
        case E_OOP_METERDIRTYLOG_T://电能表时钟超差记录单元
            sOffset += sizeof(OOP_METERDIRTYLOG_T);
            dar = data_to_buf_meterdirtylog(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);
        break; 
        case E_OOP_PROGRAMLOG_T://终端编程事件记录单元
            sOffset += sizeof(OOP_PROGRAMLOG_T);
            dar = data_to_buf_evt_prog(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);
        break;     
        case E_OOP_SECURITYLOG_T://安全事件记录单元
            sOffset += sizeof(OOP_SECURITYLOG_T);
            dar = data_to_buf_securitylog(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);
        break; 
        case E_OOP_RCTRLLOG_T://遥控跳闸记录单元
            sOffset += sizeof(OOP_RCTRLLOG_T);
            dar = data_to_buf_rctrllog(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);
        break; 

        case E_OOP_PCTRLLOG_T://功控跳闸记录单元
            sOffset += sizeof(OOP_PCTRLLOG_T);
            dar = data_to_buf_pctrllog(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);
        break;

        case E_OOP_ECTRLLOG_T://电控跳闸记录单元
            sOffset += sizeof(OOP_ECTRLLOG_T);
            dar = data_to_buf_ectrllog(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);
        break; 

        case E_OOP_EWARNLOG_T://电控告警记录单元
            sOffset += sizeof(OOP_EWARNLOG_T);
            dar = data_to_buf_ewarnlog(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);
        break; 

        case E_OOP_ABNORMALCTLOG_T://电流互感器异常记录单元
            sOffset += sizeof(OOP_ABNORMALCTLOG_T);
            dar = data_to_buf_ct_abnormal(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);
        break;
        case E_OOP_P2PCLOCKLOG_T:
            sOffset+= sizeof(OOP_P2PCLOCKLOG_T);
            dar = data_to_buf_p2pclock(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);
        break;
        case E_OOP_REFUSENODEACCESSLOG_T:
            sOffset+= sizeof(OOP_REFUSENODEACCESSLOG_T);
            dar = data_to_buf_refusenode(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);

            break;
        case E_OOP_UNMETERLOG_T:
            sOffset+= sizeof(OOP_UNMETERLOG_T);
            dar = data_to_buf_acrossarea(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);

            break; 
        case E_OOP_IdentifyResult_T:
            sOffset+= sizeof(OOP_IdentifyResult_T);
            dar = data_to_buf_identify(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);

            break; 
        case E_OOP_PF_INPUT_EXIT_T: //保护功能投退
            sOffset+= sizeof(OOP_PF_INPUT_EXIT_T);
            dar = data_to_buf_pf_input_exit(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);
        
            break;
            
#ifdef AREA_HUNAN
        case E_OOP_IMPE_OVER_T:
            sOffset+= sizeof(OOP_IMPE_OVER_T);
            dar = data_to_buf_impeover(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);

            break; 
#endif
#ifdef AREA_SHAANXI
		case E_OOP_ICCIDLOG_T:
            sOffset+= sizeof(OOP_ICCID_CHANGE_EVENT_T);
            dar = data_to_buf_ICCID(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);

            break; 
#endif
        case E_OOP_EVTSTAT_T:
        {
            return data_to_buf_evtstat(pOadInfo, convert);
        }
        break;
        case E_OOP_EVTTIME_T:
        {
            return data_to_buf_evttime(pOadInfo, convert);
        }
        break;
		case E_OOP_SYSTEMMONITOR_T:
        {
            sOffset+= sizeof(OOP_MONITORLOG_T);
            dar = data_to_buf_systemmonitor(pOadInfo, dstBuf, dstLen, &dOffset, srcBuf, srcLen);
        }break;

        default:
        {
            PRTL_FMT_DEBUG("default begin.\n");
            dar = basic_db_to_fmt698(pOadInfo,  convert);
        }
            break;
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
    
    return dar;
}

/**
*********************************************************************
* @brief：      从数据中心的数据转698格式报文(class7)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_class7(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
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
        case E_OOP_OADS_T://一组对象属性描述符
            dOffset += sizeof(OOP_OADS_T);
            return buf_to_data_evt_asobjattr(pOadInfo, srcBuf, srcLen, &sOffset, dstBuf, dstLen);
        break; 
        case E_OOP_EVTCFG_3105_T://电能表时钟超差配置参数
            dOffset += sizeof(OOP_EVTCFG_3105_T);
            return buf_to_data_evt_cfg3105(pOadInfo, srcBuf, srcLen, &sOffset, dstBuf, dstLen);
        break; 
        case E_OOP_POWERONCUT_T://终端停/上电事件配置参数
            dOffset += sizeof(OOP_POWERONCUT_T);
            return buf_to_data_evt_poweroncut(pOadInfo, srcBuf, srcLen, &sOffset, dstBuf, dstLen);
        break; 
        case E_OOP_EVTCFG_310B_T://电能表示度下降事件配置参数,电能表数据变更监控记录配置参数
            dOffset += sizeof(OOP_EVTCFG_310B_T);
            return buf_to_data_evt_cfg310b(pOadInfo, srcBuf, srcLen, &sOffset, dstBuf, dstLen);
        break; 
        case E_OOP_EVTCFG_310X_T://电能表超差事件配置参数,电能表飞走事件配置参数
            dOffset += sizeof(OOP_EVTCFG_310X_T);
            return buf_to_data_evt_cfg310x(pOadInfo, srcBuf, srcLen, &sOffset, dstBuf, dstLen);
        break; 
        case E_OOP_EVTCFG_310E_T://电能表停走事件配置参数
            dOffset += sizeof(OOP_EVTCFG_310E_T);
            return buf_to_data_evt_cfg310e(pOadInfo, srcBuf, srcLen, &sOffset, dstBuf, dstLen);
        break; 
        case E_OOP_EVTCFG_310F_T://终端抄表失败事件配置参数
            dOffset += sizeof(OOP_EVTCFG_310F_T);
            return buf_to_data_evt_cfg310f(pOadInfo, srcBuf, srcLen, &sOffset, dstBuf, dstLen);
        break;
        default:                 //其它基本类型数据
            return basic_db_from_fmt698(pOadInfo, convert);
        break;
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}
 
#endif

#if DESC("SET服务的数据存储",1)

/**
*********************************************************************
* @name：       设置关联对象属性
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_asobjattr_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_OADS_T oadsets;
    const uint8 *bufData = inData + 4; //偏掉OAD
    uint16 bufLen = InDataLen - 4;
    uint32 offset = 0;

    //从报文中提取数据
    dar = buf_to_data_evt_asobjattr(pOadInfo, bufData, bufLen, &offset, &oadsets, sizeof(OOP_OADS_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(inData, InDataLen, "buf_to_data_evt_asobjattr failed. dar(%d), oad(0x%08x), offset(%d)\n", dar, pOadInfo->pTab->oad.value, offset);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&oadsets, sizeof(OOP_OADS_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_asobjattr db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       evt_cfgld_set
* @brief       配置参数-
*              300C-电能表功率因数超下限事件
*              301D-电能表电压不平衡事件
*              301E-电能表电流不平衡事件
*              302D-电能表电流严重不平衡事件
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfgld_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_CFG_LU_T evtcfg;

    //从报文中提取数据
    dar = buf_to_data_evt_cfgld(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_CFG_LU_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_cfgld failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_CFG_LU_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfgld db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       evt_cfgdelay_set
* @brief       配置参数-
*               300E-电能表辅助电源掉电事件
*               300F-电能表电压逆相序事件
*               3010-电能表电流逆相序事件
*               3011-电能表掉电事件
*               3030-通信模块变更事件
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfgdelay_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_EVTCFG_DELAY_T evtcfg;

    //从报文中提取数据
    dar = buf_to_data_evt_cfgdelay(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_EVTCFG_DELAY_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_cfgdelay failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_EVTCFG_DELAY_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfgdelay db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}


/**
*********************************************************************
* @name：       电能表时钟超差事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3105_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_EVTCFG_3105_T evtcfg;

    //从报文中提取数据
    dar = buf_to_data_evt_cfg3105(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_EVTCFG_3105_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_asobjattr failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_EVTCFG_3105_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfg3105 db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       电能表时钟超差事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_poweroncut_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_POWERONCUT_T evtcfg;
    uint32 len = 0;

    MEMZERO(&evtcfg, sizeof(OOP_POWERONCUT_T));

    normal.oad.value = pOadInfo->pTab->oad.value;

    if(pOadInfo->nIndex == 0)
    {
        //从报文中提取数据
        dar = buf_to_data_evt_poweroncut(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_POWERONCUT_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("buf_to_data_evt_poweroncut failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
            return dar;
        }
    }
    else
    {
        //设置分量，先读取全量信息，没写就读会失败，所以不处理返回值
        ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_POWERONCUT_T), (uint8*)&evtcfg, &len);
        PRTL_FMT_TRACE("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);

        if(pOadInfo->nIndex == 1)
        {
            //从报文中提取数据
            dar = buf_to_data_evt_acquire(pOadInfo, inData, inLen, offset, &evtcfg.cParam, sizeof(OOP_DATAACQUIRE_T));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("buf_to_data_evt_poweroncut failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
                return dar;
            }
        }
        else if(pOadInfo->nIndex == 2)
        {
            //从报文中提取数据
            dar = buf_to_data_evt_identify(pOadInfo, inData, inLen, offset, &evtcfg.lParam, sizeof(OOP_EVENTIDENTIFY_T));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("buf_to_data_evt_poweroncut failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
                return dar;
            }
        }
    }
    
    normal.classtag = CLASS_DATA_INIT;
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_POWERONCUT_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_poweroncut db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       电能表时钟超差事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg310b_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_EVTCFG_310B_T evtcfg;

    //从报文中提取数据
    dar = buf_to_data_evt_cfg310b(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_EVTCFG_310B_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(inData, inLen,"buf_to_data_evt_cfg310b failed. dar(%d), oad(0x%08x), offset(%d)\n", dar, pOadInfo->pTab->oad.value, *offset);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;

    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_EVTCFG_310B_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfg310b db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       电能表时钟超差事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg310x_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_EVTCFG_310X_T evtcfg;

    //从报文中提取数据
    dar = buf_to_data_evt_cfg310x(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_EVTCFG_310X_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_cfg310x failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;

    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_EVTCFG_310X_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfg310x db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       电能表时钟超差事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg310e_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_EVTCFG_310E_T evtcfg;

    //从报文中提取数据
    dar = buf_to_data_evt_cfg310e(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_EVTCFG_310E_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_cfg310e failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;

    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_EVTCFG_310E_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfg310e db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       电能表时钟超差事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg310f_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_EVTCFG_310F_T evtcfg;

    //从报文中提取数据
    dar = buf_to_data_evt_cfg310f(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_EVTCFG_310F_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_cfg310f failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;

    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_EVTCFG_310F_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfg310f db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       通讯流量超限事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3110_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_EVTCFG_3110_T evtcfg;

    //从报文中提取数据
    dar = buf_to_data_evt_cfg3110(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_EVTCFG_3110_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_cfg3110 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;

    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_EVTCFG_3110_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfg3110 db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}
uint8 appmin_msg_set_syspara(OOP_EVTCFG_3600_T *sys,uint8 *sendbuf)
{
    uint16 index=0;
    memcpy_r(&sendbuf[index],&sys->ramRate,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&sendbuf[index],&sys->memRate,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&sendbuf[index],&sys->cpuRate,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&sendbuf[index],&sys->tempBroadUp,sizeof(int16));
    index+=sizeof(int16);
    memcpy_r(&sendbuf[index],&sys->tempBroadDown,sizeof(int16));
    index+=sizeof(int16);
    memcpy_r(&sendbuf[index],&sys->period,sizeof(uint16));
    index+=sizeof(uint16);

    return index;
}

/**
*********************************************************************
* @name：       系统监测事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3600_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    OOP_EVTCFG_3600_T evtcfg;
    uint8 sendbuf[1024] = {0},recvbuf[100] = {0};
    uint16 sendlen =0,recvlen = 0;
    //从报文中提取数据
    dar = buf_to_data_evt_cfg3600(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_EVTCFG_3600_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_cfg3110 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }
    sendlen = appmin_msg_set_syspara(&evtcfg,sendbuf);
    dar = appmain_send_syncmsg(MSG_APPAMIN_SYS_INPECT,sendbuf,sendlen,recvbuf,sizeof(recvbuf),&recvlen);
    if(dar!=ERR_OK || recvbuf[0] == 0)
    {
        PRTL_FMT_DEBUG("set sys parm error  ret %d\n",dar);
    }
    return dar;
}


/**
*********************************************************************
* @name：       阻抗超限事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3140_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    OOP_EVTCFG_3140_T evtcfg;
    NOMAL_OAD_T normal = {0};

    //从报文中提取数据
    dar = buf_to_data_evt_cfg3140(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_EVTCFG_3140_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_cfg3110 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //发送消息
    ret = appmain_set_flow_param(evtcfg.flowLimit, evtcfg.speedLimit, evtcfg.sshLTNoClose);
    if (ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("appmain_set_flow_param failed. ret(%d), oad(0x%08x)\n", ret, pOadInfo->pTab->oad.value);
        return DATA_OTHER_REASON;
    }

    //写入数据中心
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;

    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_EVTCFG_3140_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfg310e db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }
    
    return dar;
}

/**
*********************************************************************
* @name：       设置最大记录条数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_maxrec_num_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint16 data;
    const uint8 *bufData = inData + 4; //偏掉OAD
    uint16 bufLen = InDataLen - 4;
    uint32 offset = 0;

    //从报文中提取数据
    dar = basic_buf_to_data(E_LONG_UNSIGNED, bufData, bufLen, &offset, &data, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_buf_to_data failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;

    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(uint16));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_maxrec_num db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfgparam_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    const uint8 *bufData = inData + 4; //偏掉OAD
    uint16 bufLen = InDataLen - 4;
    uint32 offset = 0;

    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_CFG_LU_T://电能表功率因数超下限事件
            dar = evt_cfgld_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break;
        case E_OOP_EVTCFG_DELAY_T://通信模块变更事件
            dar = evt_cfgdelay_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break;          
        case E_OOP_EVTCFG_3105_T://电能表时钟超差配置参数
            dar = evt_cfg3105_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break; 
        case E_OOP_POWERONCUT_T: //终端停/上电事件配置参数
            dar = evt_poweroncut_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break; 
        case E_OOP_EVTCFG_310B_T://电能表示度下降事件配置参数,电能表数据变更监控记录配置参数
            dar = evt_cfg310b_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break; 
        case E_OOP_EVTCFG_310X_T://电能量超差事件配置参数,电能表飞走事件配置参数
            dar = evt_cfg310x_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break; 
        case E_OOP_EVTCFG_310E_T://电能表停走事件配置参数
            dar = evt_cfg310e_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break; 
        case E_OOP_EVTCFG_310F_T://终端抄表失败事件配置参数
            dar = evt_cfg310f_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break;
        case E_OOP_EVTCFG_3110_T://通讯流量超限事件配置参数
            dar = evt_cfg3110_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break;  
#ifdef AREA_HUNAN        
        case E_OOP_TemperatureWarningParam_T://温度越限告警事件配置参数
            dar = evt_cfg3361_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break; 
        
        case E_OOP_EVTCFG_31F2_T://阻抗越限事件配置参数
            dar = evt_cfg31f2_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break; 
#endif
        case E_OOP_FLOW_PARAM_T: //通讯流量超限事件配置参数
            dar = evt_cfg3140_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break; 

		case E_OOP_EVTCFG_3600_T: //系统监测事件配置参数
            dar = evt_cfg3600_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break; 

        default:
            dar = DATA_READ_WRITE_DENIED;
        break;
    }

    if(offset > bufLen)
    {
        dar = DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    return dar;
}


/**
*********************************************************************
* @name：       设置关联对象属性
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_report_flag_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data;
    const uint8 *bufData = inData + 4; //偏掉OAD
    uint16 bufLen = InDataLen - 4;
    uint32 offset = 0;

    //从报文中提取数据
    dar = basic_buf_to_data(E_ENUM, bufData, bufLen, &offset, &data, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_asobjattr failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;

    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(uint8));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_report_flag db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}


/**
*********************************************************************
* @name：       设置关联对象属性
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_valid_flag_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data;
    const uint8 *bufData = inData + 4; //偏掉OAD
    uint16 bufLen = InDataLen - 4;
    uint32 offset = 0;

    //从报文中提取数据
    dar = basic_buf_to_data(E_BOOL, bufData, bufLen, &offset, &data, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_buf_to_data failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;

    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(uint8));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_valid_flag db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       设置关联任务序号
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_task_index_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    const uint8 *bufData = inData + 4; //偏掉OAD
    uint16 bufLen = InDataLen - 4;
    uint32 offset = 0;

    dar = evt_cfg310b_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("evt_task_index_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;        
    }

    return dar;
}

#endif

#if DESC("GET服务的数据存储",1)

/**
*********************************************************************
* @brief：      获取读记录型的参数
* @param[in]： pRSD          - 行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 evt_record_param(RSD_T *pRSD, CSD_T *pCSD, OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, READ_RECORD_T *pInReadRecord, RELY_INFO_T *pRelyInfo, uint32 *pReadRecNum)
{
    int32 nRet = ERR_OK;
    READ_RECORD_T  inReadRecord;
    RELY_INFO_T    relyInfo;
    uint32         readRecNum = 0;   
    NOMAL_OAD_T  normal; 
    time_t tmStart = 0;
    time_t tmEnd = 0;
    time_t curTime = time(NULL);
    OOP_OCTETVAR16_T  tmnaddr = {0};        //终端通信地址    
    uint32 len = 0;
    OOP_PRSD_T oopRsd;

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&relyInfo, 0, sizeof(RELY_INFO_T));
    memset(&normal, 0, sizeof(NOMAL_OAD_T));

    oopRsd.choice = pRSD->selId;

    //事件只支持方法9，方法2要转成方法9？
    switch (pRSD->selId)
    {
        case 2: 
        {
            DT_DateTimeHex2Time((DateTimeHex_t *)(pRSD->StartData), &tmStart);

            DT_DateTimeHex2Time((DateTimeHex_t *)(pRSD->EndData), &tmEnd);

            // 注：+8个小时(28800)转成格林时间
            tmStart += COMPENSATION_TIME;
            tmEnd += COMPENSATION_TIME;
            
            //按时间范围筛选，采用升序？
            inReadRecord.sortType = UP_SORT;
            
            //方法OAD目前支持持时标类OAD
            if(pRSD->SelOad.nObjID == 0x201E0200) //事件发生时间
            {
                inReadRecord.cType = COL_TM_START;
            }
            else if(pRSD->SelOad.nObjID == 0x20200200) //事件结束时间
            {
                inReadRecord.cType = COL_TM_END;
            }

            inReadRecord.cStart = tmStart;
            inReadRecord.cEnd = tmEnd;
            memcpy(&inReadRecord.ti, &pRSD->Ti, sizeof(OOP_TI_T));
            readRecNum = 0xffffffff; //传无效的记录数
        }break;

        case 9: //方法9没有时标，但数据中心根据时间筛选，所以时间从0~当前时间
        {
            inReadRecord.cType = COL_TM_STORE;
            inReadRecord.sortType = DOWN_SORT;
            inReadRecord.cStart = 0;
            
            // 注：+8个小时(28800)转成格林时间
            inReadRecord.cEnd = curTime + 1 + COMPENSATION_TIME;  
            readRecNum = pRSD->LastN;
            oopRsd.sel9.nLast = pRSD->LastN;            
        }break;

        default: nRet = ERR_OPTION; break;
    }
   
    //获取RCSD信息
    if(pCSD == NULL)
    {
        PRTL_FMT_TRACE("RCSD = 0, means all record row need to response.\n");  
        relyInfo.oopRcsd.nNum = 0; //RCSD为0，所有列 
    }
    else
    {    
        /*pCSD -- > oopRcsd*/
        if (oop_rcsd_get(pCSD, &relyInfo.oopRcsd) != ERR_OK)
        {
            nRet = ERR_OPTION;
        }
    }

    //终端通信地址
    normal.oad.value = 0x40010200;
    nRet = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_OCTETVAR16_T), (uint8*)&tmnaddr, &len);
    if(nRet != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            nRet, normal.logicId, normal.infoNum, normal.oad.value);
        return nRet;
    }

    // 校验码，通信地址的前6个字节
    inReadRecord.MAC.nNum = 6;
    memcpy(inReadRecord.MAC.value, tmnaddr.value, 6);   
    
    //事件没有主OAD
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = pOadInfo->pTab->oad.value;
    //inReadRecord.recordOAD.logicId = pApduInfo->logicAddr; 

    if (nRet == ERR_OK)
    {
        relyInfo.bpBreakInfo = pApduInfo->bpBreakInfo;
        relyInfo.ms    = pRSD->Ms;
        relyInfo.pOadInfo = pOadInfo;
        relyInfo.oopRsd = &oopRsd;
        *pInReadRecord = inReadRecord;
        *pReadRecNum   = readRecNum;
        *pRelyInfo     = relyInfo;
    } 
        
    return nRet;
}

/**
*********************************************************************
* @brief：      获取事件的所有记录列
* @param[in]： pRSD          - 行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 evt_all_rcsd_get(OAD_INFO_T *pOadInfo, uint16 *pOffset, uint8 *pOutData, uint16 outDataMax)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;
    uint8 oadNum = 0;
    const OOP_OAD_U *pOad = NULL;
    uint32 i = 0;

    switch(pOadInfo->pTab->oad.nObjID)
    {
        case 0x3100:
        {
            oadNum = sizeof(g_evt_3100_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3100_obj;
        }break;
        
        case 0x3101:
        {
            oadNum = sizeof(g_evt_3101_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3101_obj;
        }break;  
        
        case 0x3104:
        {
            oadNum = sizeof(g_evt_3104_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3104_obj;
        }break;             

        case 0x3105:
        {
            oadNum = sizeof(g_evt_3105_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3105_obj;
        }break;  

        case 0x3106:
        {
            oadNum = sizeof(g_evt_3106_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3106_obj;
        }break; 

        case 0x3109:
        {
            oadNum = sizeof(g_evt_3109_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3109_obj;
        }break; 

        case 0x310A:
        {
            oadNum = sizeof(g_evt_310A_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_310A_obj;
        }break; 

        case 0x310B:
        {
            oadNum = sizeof(g_evt_310B_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_310B_obj;
        }break; 

        case 0x310C:
        {
            oadNum = sizeof(g_evt_310C_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_310C_obj;
        }break; 

        case 0x310D:
        {
            oadNum = sizeof(g_evt_310D_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_310D_obj;
        }break; 

        case 0x310E:
        {
            oadNum = sizeof(g_evt_310E_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_310E_obj;
        }break; 

        case 0x310F:
        {
            oadNum = sizeof(g_evt_310F_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_310F_obj;
        }break; 

        case 0x3110:
        {
            oadNum = sizeof(g_evt_3110_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3110_obj;
        }break; 

        case 0x3111:
        {
            oadNum = sizeof(g_evt_3111_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3111_obj;
        }break; 

        case 0x3112:
        {
            oadNum = sizeof(g_evt_3112_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3112_obj;
        }break; 

        case 0x3113:
        {
            oadNum = sizeof(g_evt_3113_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3113_obj;
        }break; 

        case 0x3114:
        {
            oadNum = sizeof(g_evt_3114_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3114_obj;
        }break; 

        case 0x3115:
        {
            oadNum = sizeof(g_evt_3115_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3115_obj;
        }break; 

        case 0x3118:
        {
            oadNum = sizeof(g_evt_3118_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3118_obj;
        }break;
        
        case 0x3119:
        {
            oadNum = sizeof(g_evt_3119_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3119_obj;
        }break;

        case 0x311C:
        {
            oadNum = sizeof(g_evt_311C_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_311C_obj;
        }break;

        case 0x3200:
        {
            oadNum = sizeof(g_evt_3200_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3200_obj;
        }break;

        case 0x3201:
        {
            oadNum = sizeof(g_evt_3201_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3201_obj;
        }break;

        case 0x3202:
        {
            oadNum = sizeof(g_evt_3202_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3202_obj;
        }break;

        case 0x3203:
        {
            oadNum = sizeof(g_evt_3203_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3203_obj;
        }break;

        case 0x300F:
        {
            oadNum = sizeof(g_evt_300f_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_300f_obj;
        }break;
        case 0x3010:
        {
            oadNum = sizeof(g_evt_3010_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3010_obj;
        }break;
        case 0x3140:
        {
            oadNum = sizeof(g_evt_3140_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3140_obj;
        }break;
		case 0x3199:
        {
            oadNum = sizeof(g_evt_3199_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3199_obj;
        }break;

        default:break;
    }

    //SEQUENCE OF CSD  
    pOutData[offset++] = oadNum;

    //关联的OAD和顺序写死的，后面优化
    while(i++ < oadNum)
    {
        pOutData[offset++] = 0; //CSD type 
        memcpy_r(&pOutData[offset], pOad++, sizeof(OOP_OAD_U));
        offset += 4;   
    }
    
    *pOffset = offset;
    
    return nRet;
}

/**
*********************************************************************
* @brief：      获取事件的所有记录列,获取的是解析成报文的
* @param[in]： pRSD          - 行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 evt_default_rcsd_get(OOP_OAD_U *oadEvent, OOP_RCSD_T *pRcsd)
{
    int ret = 0;
    uint8 oadNum = 0;
    const OOP_OAD_U *pOad = NULL;
    uint32 i = 0;

    switch(oadEvent->nObjID)
    {
        case 0x3100:
        {
            oadNum = sizeof(g_evt_3100_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3100_obj;
        }break;
        
        case 0x3101:
        {
            oadNum = sizeof(g_evt_3101_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3101_obj;
        }break;  
        
        case 0x3104:
        {
            oadNum = sizeof(g_evt_3104_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3104_obj;
        }break;             

        case 0x3105:
        {
            oadNum = sizeof(g_evt_3105_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3105_obj;
        }break;  

        case 0x3106:
        {
            oadNum = sizeof(g_evt_3106_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3106_obj;
        }break; 

        case 0x3109:
        {
            oadNum = sizeof(g_evt_3109_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3109_obj;
        }break; 

        case 0x310A:
        {
            oadNum = sizeof(g_evt_310A_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_310A_obj;
        }break; 

        case 0x310B:
        {
            oadNum = sizeof(g_evt_310B_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_310B_obj;
        }break; 

        case 0x310C:
        {
            oadNum = sizeof(g_evt_310C_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_310C_obj;
        }break; 

        case 0x310D:
        {
            oadNum = sizeof(g_evt_310D_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_310D_obj;
        }break; 

        case 0x310E:
        {
            oadNum = sizeof(g_evt_310E_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_310E_obj;
        }break; 

        case 0x310F:
        {
            oadNum = sizeof(g_evt_310F_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_310F_obj;
        }break; 

        case 0x3110:
        {
            oadNum = sizeof(g_evt_3110_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3110_obj;
        }break; 

        case 0x3111:
        {
            oadNum = sizeof(g_evt_3111_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3111_obj;
        }break; 

        case 0x3112:
        {
            oadNum = sizeof(g_evt_3112_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3112_obj;
        }break; 

        case 0x3113:
        {
            oadNum = sizeof(g_evt_3113_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3113_obj;
        }break; 

        case 0x3114:
        {
            oadNum = sizeof(g_evt_3114_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3114_obj;
        }break; 

        case 0x3115:
        {
            oadNum = sizeof(g_evt_3115_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3115_obj;
        }break; 

        case 0x3118:
        {
            oadNum = sizeof(g_evt_3118_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3118_obj;
        }break;
        
        case 0x3119:
        {
            oadNum = sizeof(g_evt_3119_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3119_obj;
        }break;

        case 0x311C:
        {
            oadNum = sizeof(g_evt_311C_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_311C_obj;
        }break;

        case 0x3200:
        {
            oadNum = sizeof(g_evt_3200_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3200_obj;
        }break;

        case 0x3201:
        {
            oadNum = sizeof(g_evt_3201_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3201_obj;
        }break;

        case 0x3202:
        {
            oadNum = sizeof(g_evt_3202_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3202_obj;
        }break;

        case 0x3203:
        {
            oadNum = sizeof(g_evt_3203_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3203_obj;
        }break;

        case 0x300F:
        {
            oadNum = sizeof(g_evt_300f_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_300f_obj;
        }break;
        case 0x3010:
        {
            oadNum = sizeof(g_evt_3010_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3010_obj;
        }break;
        case 0x3140:
        {
            oadNum = sizeof(g_evt_3140_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3140_obj;
        }break;

        case 0x3120: //电流互感器异常事件
        {
            oadNum = sizeof(g_evt_3120_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3120_obj;
        }break;
        
        case 0x302A: //恒定磁场干扰事件
        {
            oadNum = sizeof(g_evt_302A_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_302A_obj;
        }break;

		case 0x3600: //系统检测事件
        {
            oadNum = sizeof(g_evt_3601_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3601_obj;
        }break;

		case 0x3601: //容器检测事件
        {
            oadNum = sizeof(g_evt_3601_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3601_obj;
        }break;

		case 0x3602: //应用检测事件
        {
            oadNum = sizeof(g_evt_3602_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_3602_obj;
        }break;

        default: ret = ERR_NOTEXIST; break;
    }    

    pRcsd->nNum = oadNum;

    for(i = 0; i < pRcsd->nNum; i++)
    {
        pRcsd->cols[i].choice = 0;
        pRcsd->cols[i].oad.value = pOad[i].value;
    }

    return ret;
}

/**
*********************************************************************
* @name：       evt_rectable_recmethod_get
* @brief：      用读记录型数据方式读事件记录表
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_rectable_recmethod_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E  dar = DATA_SUCCESS;
    int nRet = ERR_OK;
    uint16 offset = 4;   //输入报文buf的偏移
    uint16 csdStart = 0;

    CSD_T *pCSD = NULL, *pCSDHead = NULL;
    RSD_T *pRSD = (RSD_T *)malloc(sizeof(RSD_T));

    READ_RECORD_T inReadRecord;
    RELY_INFO_T   relyInfo;
    uint32        readRecNum;    
    RECORD_BUF_T outRecordBuf;
    uint16 outBufLen = 0; //输出缓冲区大小
    OAD_INFO_T oadInfo;

    //参数打印
    PRTL_BUF_TRACE(inData, InDataLen, "evt_rectable_recmethod_get. oad(0x%08x)\n", pOadInfo->pTab->oad.value);

    memcpy(&oadInfo, pOadInfo, sizeof(OAD_INFO_T));

    //报文中提取RSD
    MEMZERO(pRSD, sizeof(RSD_T)); 
    offset += rsd_get(pApduInfo->hUdp, (uint8 *)(inData + offset), InDataLen - 4, pRSD);

    //没有增加,不存在RSD
    if (offset == 4) 
    {
        PRTL_FMT_TRACE("RSD解析错误\n");
        rsd_destroy(pRSD);
        return DATA_OTHER_REASON;
    }

    //报文中提取RCSD
    csdStart = offset;
    pCSDHead = rcsd_get((uint8 *)inData, InDataLen, &offset, FALSE);  
    if (!pCSDHead)
    {
        PRTL_FMT_TRACE("CSD解析错误!\n");
        rsd_destroy(pRSD);
        return DATA_TYPE_UNMATCHED;
    }
    
    /*给读任务记录表接口提供输入参数*/    //头指针没用实际的RCSD   
    pCSD = pCSDHead->next;
    nRet = evt_record_param(pRSD, pCSD, pOadInfo, pApduInfo, &inReadRecord, &relyInfo, &readRecNum);
    if (nRet != ERR_OK)
    {
        rcsd_destroy(pCSDHead);
        rsd_destroy(pRSD);

        PRTL_FMT_DEBUG("frozen_record_param failed. nRet(%d)\n", nRet);
        return DATA_READ_WRITE_DENIED;
    }

    //先封装记录响应头
    read_record_response(&oadInfo, pApduInfo, inData, InDataLen, offset, csdStart, NULL, outData, outDataMax);

    //分配读取记录的空间
    outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
    memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);
    outRecordBuf.tmEndSeg = 0;    
    outBufLen = pApduInfo->connectionInfo.ApplSplitLen - outData->resultDataLen - cal_excludeLen(pApduInfo->connectionInfo);
    
    /*读事件记录表接口，可以读出apdu数据域，可返回断点信息*/
    nRet = record_tmn_event_read(pApduInfo->hUdp, &inReadRecord, &relyInfo, readRecNum, outBufLen, &outRecordBuf);
    if (nRet < 0)
    {
        PRTL_FMT_DEBUG("record_tmn_frozen_read failed. nRet(%d)\n", nRet);
        rcsd_destroy(pCSDHead);
        rsd_destroy(pRSD);
        FREE_S(outRecordBuf.recordBuf);        
        return DATA_READ_WRITE_DENIED;
    }
  
    //封装记录读取结果
    read_record_response(&oadInfo, pApduInfo, inData, InDataLen, offset, csdStart, &outRecordBuf, outData, outDataMax);

    //处理断点信息
    if (outRecordBuf.breakInfo.appSlicingFlag == 1)
    {
        outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型方式读事件记录
        /*记下断点信息*/
        memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
        
    }
    else
    {
        /*清除断点信息*/
        memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
    }
    
    rcsd_destroy(pCSDHead);
    rsd_destroy(pRSD);
    FREE_S(outRecordBuf.recordBuf);
    
    return dar;
}

/**
*********************************************************************
* @name：       evt_rectable_normethod_get
* @brief：      用读普通数据方式读事件记录表
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_rectable_normethod_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E  dar = DATA_OTHER_REASON;
    int32 nRet = ERR_OK;   
    READ_RECORD_T inReadRecord;
    RELY_INFO_T   relyInfo;
    uint32        readRecNum = 0;
    uint16 outOffset = 0;    
    uint16 headLen = 0;
    RECORD_BUF_T outRecordBuf;
    time_t curTime = time(NULL);
    OOP_OCTETVAR16_T  tmnaddr = {0};        //终端通信地址        
    NOMAL_OAD_T  normal; 
    uint32 len = 0;
    OOP_PRSD_T oopRsd;

    MEMZERO(&inReadRecord, sizeof(READ_RECORD_T));
    MEMZERO(&relyInfo, sizeof(RELY_INFO_T));    
    MEMZERO(&normal, sizeof(NOMAL_OAD_T));
    MEMZERO(&oopRsd, sizeof(OOP_PRSD_T));
    MEMZERO(&outRecordBuf, sizeof(RECORD_BUF_T));

    //参数打印
    PRTL_BUF_TRACE(inData, InDataLen, "evt_rectable_normethod_get. oad(0x%08x)\n", pOadInfo->pTab->oad.value + pOadInfo->nIndex);

    outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
    memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);
    outRecordBuf.tmEndSeg = 0;

    /*填写OAD+记录结果1 并记下偏移headLen*/
    memcpy(outData->pResultData, inData, 4);
    outOffset = 4;

    outData->pResultData[outOffset++] = 1; //[1] DATA
    
    outData->resultDataLen += outOffset;
    headLen = outOffset;

    inReadRecord.sortType = NO_SORT;
    inReadRecord.cType = COL_TM_STORE; //最近几条记录，肯定是用存储时间筛选
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = curTime+1;

    //终端通信地址
    normal.oad.value = 0x40010200;
    nRet = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_OCTETVAR16_T), (uint8*)&tmnaddr, &len);
    if(nRet != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            nRet, normal.logicId, normal.infoNum, normal.oad.value);

        FREE_S(outRecordBuf.recordBuf);
        return DATA_READ_WRITE_DENIED;
    }
    
    // 校验码，通信地址的前6个字节
    inReadRecord.MAC.nNum = 6;
    memcpy(inReadRecord.MAC.value, tmnaddr.value, 6);   

    //把OAD的属性索引转换成方法9，读上第n次记录
    if(pOadInfo->nIndex != 0)
    {
        oopRsd.choice = 9;
        oopRsd.sel9.nLast = pOadInfo->nIndex;
        inReadRecord.sortType = DOWN_SORT;
    }
    
    readRecNum = 0xffffffff; 

    relyInfo.method = 1;
    relyInfo.bpBreakInfo = pApduInfo->bpBreakInfo;
    relyInfo.pOadInfo = pOadInfo;
    relyInfo.oopRsd = &oopRsd;
    
    //事件没有次OAD，但是接口填写，应该主DI为0，次DI填OAD
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = pOadInfo->pTab->oad.value; 
    //inReadRecord.recordOAD.logicId = pApduInfo->logicAddr; 
    
    /*读事件记录表接口，可以读出apdu数据域，可返回断点信息*/
    if (record_tmn_event_read(pApduInfo->hUdp, &inReadRecord, &relyInfo, readRecNum, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
    {
        PRTL_FMT_DEBUG("record_event_tmn_read failed.\n");
        dar = DATA_READ_WRITE_DENIED;
    }
    else
    {
        PRTL_BUF_TRACE(outRecordBuf.recordBuf, outRecordBuf.bufLen, "record_event_tmn_read:");
        memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
        outData->resultDataLen += outRecordBuf.bufLen;
    
        if (outRecordBuf.breakInfo.appSlicingFlag == 1)
        {
            outRecordBuf.breakInfo.objPre = 0x01;   //对象属性，普通数据方式读事件记录
            /*记下断点信息*/
            memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
        }
        else
        {
            /*清除断点信息*/
            memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
        }
        
        dar = DATA_SUCCESS;
    }

    FREE_S(outRecordBuf.recordBuf);
    
    return dar;
}

/**
*********************************************************************
* @name：       evt_rectable_get
* @brief：      读事件记录表
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_rectable_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    if(pOadInfo->isRec == TRUE)
    {
        return evt_rectable_recmethod_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }
    else
    {
        return evt_rectable_normethod_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);        
    }

    return dar;
}


/**
*********************************************************************
* @name：       evt_rectable_get
* @brief：      读关联对象属性表
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_objattr_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_OADS_T oadsets;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&oadsets, 0, sizeof(OOP_OADS_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_OADS_T), (uint8*)&oadsets, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        
        //数据中心出错返回NULL
        return oop_null_array_res(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_asobjattr(pOadInfo, outData->pResultData, outDataMax, &offset, &oadsets, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_asobjattr failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;    
}

/**
*********************************************************************
* @name：       evt_rectable_get
* @brief：      当前记录数
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_currec_num_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    uint16 data = 0;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    //normal.logicId = pApduInfo->logicAddr;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(uint16), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        len = sizeof(uint16);
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;
    outData->pResultData[offset++] = DT_LONG_UNSIGNED;
    memcpy_r(&outData->pResultData[offset], &data, len);
    offset += len;

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       evt_rectable_get
* @brief：      最大记录数
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_maxrec_num_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    RECORD_OAD_T tRroad = {0};   
    uint32 data = 0;
    uint32 offset = 0;

    tRroad.logicId = 0;
    tRroad.infoNum = 0;
    tRroad.optional = 0;
    tRroad.road.oadMain.value = 0;
    tRroad.road.oadCols.nNum = 1;
    tRroad.road.oadCols.oad[0].nObjID = pOadInfo->pTab->oad.nObjID;
    tRroad.road.oadCols.oad[0].attID = 2;
    tRroad.road.oadCols.oad[0].attPro = 0;
    tRroad.road.oadCols.oad[0].nIndex = 0;

    ret = db_get_record_depth(pApduInfo->hUdp, &tRroad, &data);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_get_record_depth failed. ret(%d), logicId(0), infoNum(%d), oad(0x%08x)\n",
            ret, tRroad.infoNum, tRroad.road.oadCols.oad[0].value);
        return oop_null_data_res(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;
    outData->pResultData[offset++] = DT_LONG_UNSIGNED;
    memcpy_r(&outData->pResultData[offset], (uint16*)&data, 2);
    offset += 2;

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       evt_cfgld_get
* @brief       配置参数-
*              300C-电能表功率因数超下限事件
*              301D-电能表电压不平衡事件
*              301E-电能表电流不平衡事件
*              302D-电能表电流严重不平衡事件
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfgld_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_CFG_LU_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_CFG_LU_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_CFG_LU_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return oop_null_data_res(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfgld(pOadInfo, outData->pResultData, outDataMax, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_cfgld failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;    
}

/**
*********************************************************************
* @name：       evt_cfgdelay_get
* @brief       配置参数-
*              300E-电能表辅助电源掉电事件
*              300F-电能表电压逆相序事件
*              3010-电能表电流逆相序事件
*              3011-电能表掉电事件
*              3030-通信模块变更事件
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfgdelay_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_EVTCFG_DELAY_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_DELAY_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_EVTCFG_DELAY_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return oop_null_data_res(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfgdelay(pOadInfo, outData->pResultData, outDataMax, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_cfgdelay failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;    
}

/**
*********************************************************************
* @name：       电能表时钟超差事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3105_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_EVTCFG_3105_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_3105_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_EVTCFG_3105_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return oop_null_data_res(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfg3105(pOadInfo, outData->pResultData, outDataMax, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_asobjattr failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;    
}

/**
*********************************************************************
* @name：       电能表时钟超差事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_poweroncut_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_POWERONCUT_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;

    memset(&evtcfg, 0, sizeof(OOP_POWERONCUT_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_POWERONCUT_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return oop_null_data_res(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    //结果类型 [1] DATA
    outData->pResultData[offset++] = 1;

    if(pOadInfo->nIndex == 0)
    {
        dar = data_to_buf_evt_poweroncut(pOadInfo, outData->pResultData, outDataMax, &offset, &evtcfg, len);
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("data_to_buf_evt_poweroncut failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
            return dar;
        }
    }
    else if(pOadInfo->nIndex == 1)
    {
        dar = data_to_buf_evt_acquire(pOadInfo, outData->pResultData, outDataMax, &offset, &evtcfg.cParam, sizeof(OOP_DATAACQUIRE_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("data_to_buf_evt_acquire failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
            return dar;
        }       
    }
    else if(pOadInfo->nIndex == 2)
    {
        dar = data_to_buf_evt_identify(pOadInfo, outData->pResultData, outDataMax, &offset, &evtcfg.lParam, sizeof(OOP_EVENTIDENTIFY_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("data_to_buf_evt_acquire failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
            return dar;
        }       
    }
    
    outData->resultDataLen = offset;

    return dar;    
}

/**
*********************************************************************
* @name：       电能表时钟超差事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg310b_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_EVTCFG_310B_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_310B_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_EVTCFG_310B_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return oop_null_data_res(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfg310b(pOadInfo, outData->pResultData, outDataMax, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_asobjattr failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       电能表时钟超差事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg310x_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_EVTCFG_310X_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_310X_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_EVTCFG_310X_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return oop_null_data_res(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfg310x(pOadInfo, outData->pResultData, outDataMax, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_asobjattr failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       电能表时钟超差事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg310e_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_EVTCFG_310E_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_310E_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_EVTCFG_310E_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return oop_null_data_res(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfg310e(pOadInfo, outData->pResultData, outDataMax, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_asobjattr failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       电能表时钟超差事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg310f_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_EVTCFG_310F_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_310F_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_EVTCFG_310F_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return oop_null_data_res(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfg310f(pOadInfo, outData->pResultData, outDataMax, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_asobjattr failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       通讯流量超限事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3110_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_EVTCFG_3110_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_3110_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_EVTCFG_3110_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return oop_null_data_res(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfg3110(pOadInfo, outData->pResultData, outDataMax, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_asobjattr failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       通讯流量超限事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3140_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_EVTCFG_3140_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_3110_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_EVTCFG_3140_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return oop_null_data_res(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfg3140(pOadInfo, outData->pResultData, outDataMax, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_asobjattr failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       通讯流量超限事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3600_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_EVTCFG_3600_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_3600_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_EVTCFG_3600_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return oop_null_data_res(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfg3600(pOadInfo, outData->pResultData, outDataMax, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_asobjattr failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       evt_rectable_get
* @brief：      读关联对象属性表
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfgparam_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_CFG_LU_T://电能表功率因数超下限事件
            dar = evt_cfgld_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
        case E_OOP_EVTCFG_DELAY_T://通信模块变更事件
            dar = evt_cfgdelay_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break;         
        case E_OOP_EVTCFG_3105_T://电能表时钟超差配置参数
            dar = evt_cfg3105_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
        case E_OOP_POWERONCUT_T: //终端停/上电事件配置参数
            dar = evt_poweroncut_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
        case E_OOP_EVTCFG_310B_T://电能表示度下降事件配置参数,电能表数据变更监控记录配置参数
            dar = evt_cfg310b_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
        case E_OOP_EVTCFG_310X_T://电能表超差事件配置参数,电能表飞走事件配置参数
            dar = evt_cfg310x_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
        case E_OOP_EVTCFG_310E_T://电能表停走事件配置参数
            dar = evt_cfg310e_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
        case E_OOP_EVTCFG_310F_T://终端抄表失败事件配置参数
            dar = evt_cfg310f_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
        case E_OOP_EVTCFG_3110_T://通讯流量超限事件配置参数
            dar = evt_cfg3110_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break;   
#ifdef AREA_HUNAN
        case E_OOP_TemperatureWarningParam_T://温度超限告警事件配置参数
            dar = evt_cfg3361_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
        case E_OOP_EVTCFG_31F2_T:              //阻抗超限告警事件配置参数
            dar = evt_cfg31f2_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
#endif
        case E_OOP_FLOW_PARAM_T:             //通讯流量超限事件配置参数
            dar = evt_cfg3140_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 

		case E_OOP_EVTCFG_3600_T: //系统监测事件配置参数
            dar = evt_cfg3600_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 

        default:
            dar = DATA_READ_WRITE_DENIED;
        break;
    }

    return dar;
}

/**
*********************************************************************
* @name：       evt_report_flag_get
* @brief：      上报标识
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_report_flag_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    uint8 data = 0;
    uint32 len = 0;
    uint32 offset = 0;

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    //读取无数据视为0
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(uint8), &data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        
        if(ret == ERR_NOTEXIST)
        {
            len = sizeof(uint8);
        }
        else
        {
            return DATA_OTHER_REASON;
        }
    }

    outData->pResultData[offset++] = 1;
    outData->pResultData[offset++] = DT_ENUM;
    memcpy_r(&outData->pResultData[offset], &data, len);
    offset += len;

    outData->resultDataLen = offset;

    return dar;
}


/**
*********************************************************************
* @name：       evt_report_flag_get
* @brief：      上报标识
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_valid_flag_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    uint8 data = 0;
    uint32 len = 0;
    uint32 offset = 0;

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    //读取无数据视为0
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(uint8), &data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        
        if(ret == ERR_NOTEXIST)
        {
            len = sizeof(uint8);
        }
        else
        {
            return DATA_OTHER_REASON;
        }
    }

    outData->pResultData[offset++] = 1;
    outData->pResultData[offset++] = DT_BOOL;
    memcpy_r(&outData->pResultData[offset], &data, len);
    offset += len;

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       evt_rectable_get
* @brief：      读关联对象属性表
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_task_index_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    dar = evt_cfg310b_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("evt_task_index_get failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;        
    }

    return dar;
}

#endif

#if DESC("ACT服务数据接口",1)

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
int evt_cfg_add(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, OOP_EVTOADS_T *pData)
{
    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0};
    uint32 len = 0;
    OOP_EVTOADS_T data;    
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
    arrdsingle(&pData->oad, (int*)&pData->nNum, &data.oad, (int*)&data.nNum, sizeof(OOP_OAD_U), NULL);

    //未超过最大个数才添加，否则会造成数组越界
    if((data.nNum + pData->nNum) <= OOP_MAX_EVTOADS)
    {
        memcpy(&data.oad[data.nNum], &pData->oad, sizeof(OOP_OAD_U)*pData->nNum);
        data.nNum += pData->nNum;
    }
    else
    {
        PRTL_FMT_DEBUG("evt_cfg_add failed, member is full. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
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
* @name：       evt_reset
* @brief：      复位
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_reset(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    OOP_OAD_U oad = {0};
    CLEAR_DATA_T clearData;

    MEMZERO(&clearData, sizeof(clearData)); 

    //构造事件记录表的OAD
    oad.nObjID = pOadInfo->pTab->oad.nObjID;
    oad.attID = 2;

    //删除OAD
    clearData.recordOAD.classtag = CLASS_DATA_INIT;
    clearData.recordOAD.road.oadCols.nNum = 1;
    clearData.recordOAD.road.oadCols.oad[0].value = oad.value;
    
    ret = db_clear_oad_data(pApduInfo->hUdp, &clearData);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_clear_oad_data failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, clearData.recordOAD.logicId, clearData.recordOAD.infoNum, clearData.recordOAD.road.oadCols.oad[0].value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       tgsets_cfg_unit_add
* @brief：      添加一个关联对象属性单元
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_objattr_unit_add(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    OOP_EVTOADS_T subData;
    uint32 sOffset = 4;

    MEMZERO(&subData, sizeof(subData)); 
    
    subData.nNum = 1;
    
    //从报文中提取数据
    dar = basic_buf_to_data(E_OAD, inData, InDataLen, &sOffset, &subData.oad[0], sizeof(OOP_OAD_U));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "basic_buf_to_data failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    ret = evt_cfg_add(pOadInfo, pApduInfo, &subData);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_objattr_unit_add failed. ret(%d)\n", ret);
        return DATA_OTHER_REASON;
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
OOP_DAR_E evt_objattr_unit_del(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_EVTOADS_T data;
    OOP_OAD_U unitData;
    CLEAR_DATA_T clearData;
    uint32 sOffset = 4;
    uint32 len = 0;
    uint32 i = 0;
    OOP_OAD_U oad = {0};

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&clearData, sizeof(clearData));     
    MEMZERO(&unitData, sizeof(unitData)); 

    oad.nObjID = pOadInfo->pTab->oad.nObjID;
    oad.attID = 2;

    //从报文中提取数据
    dar = basic_buf_to_data(E_OAD, inData, InDataLen, &sOffset, &unitData, sizeof(unitData));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(inData, InDataLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    normal.oad.value = oad.value;

    //读取原有配置表,读不到认为已经删了,直接返回成功
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_SUCCESS;
    }

    //找到对应的关联属性,把位于其后的分路向前递补移动
    for(i = 0; i < data.nNum; i++)
    {
        if(data.oad[i].value == unitData.value)
        {
            memmove(&data.oad[i], &data.oad[i+1], (data.nNum-i-1)*sizeof(unitData));
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
        
        ret = db_clear_oad_data(pApduInfo->hUdp, &clearData);
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_clear_oad_data failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, clearData.recordOAD.logicId, clearData.recordOAD.infoNum, clearData.recordOAD.road.oadCols.oad[0].value);
            return DATA_OTHER_REASON;
        }
    }
    else
    {
        //写入数据
        normal.classtag = CLASS_DATA_INIT;
        
        ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_EVTOADS_T));
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
            return DATA_OTHER_REASON;
        }
    }
    
    return dar;
}

#endif


#if DESC("class7对外接口函数",1)

/**
*********************************************************************
* @name：       class7_invoke_set
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class7_invoke_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 3:   //关联对象属性表
        {
            dar = evt_asobjattr_set(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;   
        case 5:   //最大记录数
        {
            dar = evt_maxrec_num_set(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;        
        case 6:   //配置参数
        {
            dar = evt_cfgparam_set(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;
        case 8:   //上报标识
        {
            dar = evt_report_flag_set(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;        
        case 9:   //有效标识
        {
            dar = evt_valid_flag_set(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;
        case 11: //关联任务序号
        {
            return evt_task_index_set(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;

        default: 
        {
            dar = DATA_READ_WRITE_DENIED; 
        }break;
    }

    if(dar == DATA_SUCCESS)
    {
        //拷贝OAD
        memcpy_r(outData->pResultData, inData, sizeof(uint32));
        outData->resultDataLen += sizeof(uint32);

        //不包含DAR,统一外面处理
    }

    return dar;
}

/**
*********************************************************************
* @name：       class7_invoke_get
* @brief：      class7的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class7_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    
    switch (pOadInfo->pTab->oad.attID)
    {
        case 2:   //事件记录表
        {
           return evt_rectable_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;
        
        case 3:  //关联对象属性表
        {
            return evt_objattr_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;

        case 4:  //当前记录数
        {
            return evt_currec_num_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;        
         case 5:  //最大记录数
        {
            return evt_maxrec_num_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;

        case 6:  //配置参数
        {
            return evt_cfgparam_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;

        case 7:  //当前值记录表
        {
            return DATA_READ_WRITE_DENIED;//暂不支持
        }break;

        case 8:  //上报标识
        {
            return evt_report_flag_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;

        case 9:  //有效标识
        {
            return evt_valid_flag_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;

         case 10: //时间状态记录表
        {
            return DATA_READ_WRITE_DENIED;//暂不支持
        }break;
        case 11: //关联任务序号
        {
            return evt_task_index_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;       
        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}


/**
*********************************************************************
* @name：       class7_invoke_act
* @brief：      class7的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class7_invoke_act(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 1:   //复位
        {
            dar = evt_reset(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        }break; 
        
        case 4:   //添加一个事件关联对象属性
        {
            dar = evt_objattr_unit_add(pOadInfo,  pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        }break;   
        
        case 5:   //删除一个事件关联对象属性
        {
            dar = evt_objattr_unit_del(pOadInfo,  pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        }break;
        
        default: 
        {
            dar = DATA_READ_WRITE_DENIED; 
        }break;
    }

    return dar;    
}


#endif
