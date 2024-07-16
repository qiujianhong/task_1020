
/*
*********************************************************************
* Copyright(C) 2020 China Electric Power Research Institute
* All rights reserved. 
* @brief：   配网-浙江需求，面向对象数据类型扩展定义
* @date：    2020/08/28
* @history： 
*
*	2020/08/28:
*		1. 创建
*********************************************************************
*/

#ifndef __OOP_PW_ZHEJIANG_H__
#define __OOP_PW_ZHEJIANG_H__

#include "baseCommon.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
 ********************************************************************
 *																	*
 * 25-26-OOP 通信通道类型                                           *
 *																	*
 ********************************************************************
*/
/** @brief 45300200 通信通道类型 */
#define ZJ_CONNECT_PUBLIC    1
#define ZJ_CONNECT_SPECIAL   2
#define ZJ_CONNECT_ETH       3
#define ZJ_CONNECT_RS232     6
#define ZJ_CONNECT_NULL      0xff
typedef struct tag_OOP_ZJ_CONNECTTYPE
{
    uint8           type;           /**< 通信通道类型  FF-无效;01-无线公网;02-无线专网;03-以太网;06-RS232 */
}OOP_ZJ_CONNECT_T;


/** @brie 配变参数结构体 */
typedef struct tag_OOP_PD_TRANSFORMER_PARAM
{
    uint8  isVaild;                     /**< 设备状态 1：有效 0：无效 */
    float  capacity;                    /**< 变压器容量（折算到二次侧） */
    uint32 resCurTransRatio;            /**< 剩余电流互感器变比 */
    uint8  warMaskWord[4];              /**< 告警屏蔽字 从最低位到最高位，按位表示告警信息体序号，0：屏蔽； 1：不屏蔽。*/ 
    float  curNegImbalanceOccurVal;     /**< 电流负序不平衡阈值 缺省值 30%*/
    float  curNegImbalanceRecoverVal;   /**< 电流负序不平衡恢复阈值 缺省值 25%*/
    float  voltQuaUpper;                /**< 合格电压上限（二次值，未设置或 0 时，按照缺省规则判断）*/       
    float  voltQuaLower;                /**< 合格电压下限（二次值，未设置或 0 时，按照缺省规则判断）*/
    float  loadOverOccurRatio;          /**< 判断过负荷、电流越限、超合同容量用电、视在功率越限相对额定值的比例（缺省为 1.2）*/
    float  loadOverRecoverRatio;        /**< 判断过负荷、电流越限、超合同容量用电、视在功率越限恢复相对额定值的比例（缺省为 1.05）*/
    float  capOverOccurVal;             /**< 判断无功过补偿的功率因数（容性）*/
    float  capOverRecoverVal;           /**< 判断无功过补偿恢复的功率因数（容性）*/
    float  indOverOccurVal;             /**< 判断无功欠补偿的功率因数（感性）*/
    float  indOverRecoverVal;           /**< 判断无功欠补偿恢复的功率因数（感性）*/
    float  voltOverOccurRatio;          /**< 判断过压越限相对额定值的比例（缺省 1.15）*/
    float  voltOverRecoverRatio;        /**< 判断过压越限恢复相对额定值的比例（缺省1.05）*/
    float  voltLowerOccurRatio;         /**< 判断欠压越限相对额定值的比例（缺省 0.8）*/
    float  voltLowerRecoverRatio;       /**< 判断欠压越限恢复相对额定值的比例（缺省 0.9）*/
    float  zeroCurOverOccurRatio;       /**< 判断零序电流越限相对额定值的比例（缺省 0.5）*/
    float  zeroCurOverRecoverRatio;     /**< 判断零序电流越限恢复相对额定值的比例（缺省0.5）*/
    float  voltHarmonicOverOccurVal;    /**< 判断电压谐波总畸变率超限的设置值（缺省5%）*/
    float  voltHarmonicOverRecoverVal;  /**< 判断电压谐波总畸变率超限恢复的设置值（缺省4.75%）*/
    float  curHarmonicOverOccurVal;     /**< 判断电流谐波总畸变率超限的设置值（缺省5%）*/
    float  curHarmonicOverRecoverVal;   /**< 判断电流谐波总畸变率超限恢复的设置值（缺省4.75%）*/
    float  voltOddHarmonicUpper;        /**< 奇次电压谐波含有率上限*/
    float  voltEvenHarmonicUpper;       /**< 偶次电压谐波含有率上限*/
    float  curHarmonicUpper[14];        /**< N(2~15)次谐波电流有效值上限*/
    float  resCurOverOccurVal;          /**< 判断剩余电流越限相对额定值的比例（缺省 0.5）*/
    float  resCurOverRecoverVal;        /**< 判断剩余电流越限恢复相对额定值的比例（缺省0.5）*/
    float  loadOverOccurTime;           /**< 判断过负荷、电流过负荷、超合同容量用电、电流不平衡、无功欠补偿、无功过补偿、零序电流越限的时间 缺省 5 分钟*/
    float  loadOverRecoverTime;         /**< 判断过负荷、电流过负荷、超合同容量用电、电流不平衡、无功欠补偿、无功过补偿、零序电流越限恢复的时间 缺省 5 分钟*/
    float  voltHarmonicOverOccurTime;   /**< 判断过压、欠压、电压谐波总畸变率越限的时间 缺省 5 分钟*/
    float  voltHarmonicOverRecoverTime; /**< 判断过压、欠压、电压谐波总畸变率越限恢复的时间 缺省 5 分钟*/
}OOP_PD_TRANSFORMER_PARAM_T;

/** @brief F2F10200 遥测数据信息表 */
typedef struct tag_OOP_ZJ_TeleMetry_PARAM
{
    float  temperature;                  /**< 配电箱内温度(℃); 最高位D15为符号位, 0表示正数, 1表示负数*/
    float  humidity;                     /**< 配电箱内湿度(%)*/
    float  dayResist_A;                  /**< 当日A相阻抗（电阻）数据(Ω)*/
    float  dayResist_B;                  /**< 当日B相阻抗（电阻）数据(Ω)*/
    float  dayResist_C;                  /**< 当日C相阻抗（电阻）数据(Ω)*/

    float  lastDayResist_A;              /**< 上日A相阻抗（电阻）数据(Ω)*/
    float  lastDayResist_B;              /**< 上日B相阻抗（电阻）数据(Ω)*/
    float  lastDayResist_C;              /**< 上日C相阻抗（电阻）数据(Ω)*/

    float  lastDayRate_A;                /**< 上日A相的阻抗数据偏离度; 算法: abs(D2-D1)/D1*/
    float  lastDayRate_B;                /**< 上日B相的阻抗数据偏离度; 算法: abs(D2-D1)/D1*/
    float  lastDayRate_C;                /**< 上日C相的阻抗数据偏离度; 算法: abs(D2-D1)/D1*/

    float  lastDayEResist_A;             /**< 上日A相电抗数据(Ω)*/
    float  lastDayEResist_B;             /**< 上日B相电抗数据(Ω)*/
    float  lastDayEResist_C;             /**< 上日C相电抗数据(Ω)*/

    float  lastDayForwEnergy;            /**< 上日正向有功总电能(kWh)*/
    float  lastDayReveEnergy;            /**< 上日反向有功总电能(kWh)*/

    float  lastDayRealLoss;              /**< 上日实际线损(%)*/
    float  lastDayCalcLoss;              /**< 上日计算线损(%)*/
}OOP_ZJ_TELEMETRY_T;

#ifdef __cplusplus
}
#endif

#endif // __OOP_PW_ZHEJIANG_H__
