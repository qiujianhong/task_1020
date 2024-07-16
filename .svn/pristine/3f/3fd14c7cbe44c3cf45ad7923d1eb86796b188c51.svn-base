/**
*********************************************************************
* @copyright Copyright(C) 2019 China Electric Power Research Institute
* All rights reserved.
* @file    amr_msg_interface.h
* @brief   抄表组件消息
* @note    
* @warning 注意事项
* @date    2019-06-01
*********************************************************************
*/

#ifndef __AMR_MESSAGE_INTERFACE_H__
#define __AMR_MESSAGE_INTERFACE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define SMI_AMR_NAME                "smiAmr"

/** @brief     抄表组件消息接口ID     */
#define IID_METERING                5

/**
******************************************************************************
@brief        载波抄读信息
@verbatim
            PLC_READING_INFO ∷= SEQUENCE 
            {
                中继路由级数        : unsigned,
                信道标识            : unsigned,
                载波抄读相位        : unsigned,
                载波信号品质        : unsigned,
                载波标志            : unsigned,
                保留                : octet-string(SIZE(3))
            }
@endverbatim
******************************************************************************
*/

/**
******************************************************************************
@brief        获取抄读内容
@verbatim
            IOP_AMR_GET_DATA ∷= SEQUENCE 
            {
                端口号                : double-long-unsigned,
                测量点序号            : long-unsigned,
                通信控制字            : unsigned,
                表计地址            : octet-string(SIZE(6))
            }

            ACK ∷= octet-string
@endverbatim
******************************************************************************
*/
#define IOP_AMR_GET_DATA                11

/**
******************************************************************************
@brief        上报抄读内容
@verbatim
            IOP_AMR_RPT_DATA ∷= SEQUENCE 
            {
                端口号          : double-long-unsigned,
                测量点序号        : long-unsigned,
                协议类型        : unsigned,
                数据帧            : octet-string,
                抄读信息        : PLC_READING_INFO OPTIONAL
            }
@endverbatim
******************************************************************************
*/
#define IOP_AMR_RPT_DATA                12

/**
******************************************************************************
@brief        上报事件数据
@verbatim
            IOP_AMR_EVT_DATA ∷= SEQUENCE 
            {
                端口号          : double-long-unsigned,
                测量点序号        : long-unsigned,
                协议类型        : unsigned,
                数据帧            : octet-string,
                抄读信息        : PLC_READING_INFO OPTIONAL
            }
@endverbatim
******************************************************************************
*/
#define IOP_AMR_EVT_DATA                13

/**
******************************************************************************
@brief        启动搜表
@verbatim
            IOP_AMR_RGST_START ∷= SEQUENCE 
            {       
                端口号                    : double-long-unsigned,
                开始时间                : double-long,
                持续时间                : double-long,
                从节点重发次数            : unsigned,
                随机等待时间片个数        : unsigned
            }
@endverbatim
******************************************************************************
*/
#define IOP_AMR_RGST_START                14    

/**
******************************************************************************
@brief        表计参数
@verbatim
            表计参数 ::= SEQUENCE 
            {
                表计地址                : octet-string(SIZE(6)),
                采集器地址              : octet-string(SIZE(6)),
                表计协议号              : unsiged,
                通信参数                : unsiged
            }
@endverbatim
******************************************************************************
*/

/**
******************************************************************************
@brief        搜表数据上报
@verbatim
            IOP_AMR_RGST_REPORT ∷= SEQUENCE 
            {       
                端口号                    : double-long-unsigned,        
                载波传输附加信息        : PLC_READING_INFO OPTIONAL
                表计档案                : SEQUENCE OF 表计参数
            }
@endverbatim
******************************************************************************
*/
#define IOP_AMR_RGST_REPORT             15    

/**
******************************************************************************
@brief        查询HPLC信息
@verbatim
            IOP_AMR_HPLC_QUERY ∷= SEQUENCE 
            {       
                端口号                : double-long-unsigned,
                信息类型            : unsigned
            }
@endverbatim
******************************************************************************
*/
#define IOP_AMR_HPLC_QUERY                16

/**
******************************************************************************
@brief        上报HPLC信息
@verbatim
            IOP_AMR_HPLC_REPORT ∷= SEQUENCE 
            {       
                端口号                : double-long-unsigned,
                起始序号            : long-unsigned OPTIONAL,  
                信息类型            : CHOICE
                {
                    网络拓扑信息                    [1] SEQUENCE OF AMR_HPLC_TOPOLOGYINFO_T,
                    查询宽带载波芯片信息            [2] SEQUENCE OF AMR_HPLC_CHIPINFO_T,
                    查询多网络信息                    [3] AMR_HPLC_NETSINFO_T,
                    查询节点相线信息                [4] SEQUENCE OF AMR_HPLC_PHASEINFO_T,
                    查询网络规模                    [5] long-unsigned,
                    查询本地主节点模块ID信息        [6]    AMR_HPLC_MASTERID_T,
                    查询从节点ID信息                 [7] SEQUENCE OF AMR_HPLC_SLAVEID_T,
                    查询网络基本信息                [8] AMR_HPLC_BASEINFO_T,
                    查询网络节点信息                [9] SEQUENCE OF AMR_HPLC_NETNODEINFO_T
                }
            }
 @endverbatim
 ******************************************************************************
 */
#define IOP_AMR_HPLC_REPORT             17

/**
******************************************************************************
@brief        获取模块抄表状态
@verbatim
            IOP_AMR_METERING_STAT ∷= double-long-unsigned(端口号)
            
            ACK    ∷= SEQUENCE
            {       
                最近一次接收时间    : double-long-unsigned,
                模块抄表状态        : double-long
                模块搜表状态        : double-long
            }
@endverbatim
******************************************************************************
*/
#define IOP_AMR_METERING_STAT            18

/**
******************************************************************************
@brief        暂停搜表
@verbatim
            IOP_AMR_RGST_STOP ∷= NULL
@endverbatim
******************************************************************************
*/
#define IOP_AMR_RGST_STOP                19

/**
******************************************************************************
@brief        暂停抄表
@verbatim
            IOP_AMR_PAUSE ∷= double-long-unsigned(端口号)
@endverbatim
******************************************************************************
*/
#define IOP_AMR_PAUSE                    20

/**
******************************************************************************
@brief        恢复抄表
@verbatim
            IOP_AMR_RESUME ∷= double-long-unsigned(端口号)
@endverbatim
******************************************************************************
*/
#define IOP_AMR_RESUME                     21

/**
******************************************************************************
@brief        重启抄表
@verbatim
            IOP_AMR_RESTART ∷= double-long-unsigned(端口号)
@endverbatim
******************************************************************************
*/
#define IOP_AMR_RESTART                    22

/**
******************************************************************************
@brief        档案下发
@verbatim
            IOP_AMR_ADD_METER ∷= SEQUENCE OF
            {    
                端口号          : double-long-unsigned
                电能表档案         : AMR_METER_T
            }

            AMR_METER_T ∷= SEQUENCE
            {
                测量点序号        : long-unsigned,
                测量点地址        : octet-string(SIZE(6))，
                测量点MAC        : octet-string(SIZE(6))，
                协议类型        : unsigned,
                通信参数        : unsigned
            }
@endverbatim
******************************************************************************
*/
#define IOP_AMR_ADD_METER                23

/**
******************************************************************************
@brief        清除档案
@verbatim
            IOP_AMR_CLEAR_METER ∷= NULL
@endverbatim
******************************************************************************
*/
#define IOP_AMR_CLEAR_METER                24

/**
******************************************************************************
@brief        删除档案
@verbatim
            IOP_AMR_DELETE_METER ∷ = SEQUENCE OF long-unsigned
@endverbatim
******************************************************************************
*/
#define IOP_AMR_DELETE_METER            25

/**
 ******************************************************************************
 @brief        数据透传
 @verbatim
             IOP_AMR_TRANSMISSION ∷= SEQUENCE
            {
               端口号            : double-long-unsigned,
               通讯字            : unsigned,
               帧超时时间        : long-unsigned,
               字符超时时间        : long-unsigned,
               发送数据内容        : octet-string,
            }
            
             ACK ∷= octet-string 
 @endverbatim            
 ******************************************************************************
 */
#define IOP_AMR_TRANSMISSION            26

/**
******************************************************************************
@brief        路由数据初始化
@verbatim
            IOP_AMR_ROUTER_DATA_INIT    ∷= double-long-unsigned(端口号)
@endverbatim            
******************************************************************************
*/
#define IOP_AMR_ROUTER_DATA_INIT        27

/**
******************************************************************************
@brief        路由参数初始化
@verbatim
            IOP_AMR_ROUTER_PARAM_INIT    ∷= double-long-unsigned(端口号)
@endverbatim            
******************************************************************************
*/
#define IOP_AMR_ROUTER_PARAM_INIT        28

/**
******************************************************************************
@brief        复位路由
@verbatim
            IOP_AMR_ROUTER_RESET    ∷= SEQUENCE
            {
                端口号            : double-long-unsigned
                延时时间        : unsigned
            }
@endverbatim            
******************************************************************************
*/
#define IOP_AMR_ROUTER_RESET            29

/**
******************************************************************************
@brief        启动组网
@verbatim
            IOP_AMR_ROUTER_RESET    ∷= double-long-unsigned(端口号)
@endverbatim            
******************************************************************************
*/
#define IOP_AMR_ROUTER_STARTNET            30

/**
******************************************************************************
@brief        网络维护
@verbatim
            IOP_AMR_ROUTER_MAINTNET       ∷= double-long-unsigned(端口号)
@endverbatim            
******************************************************************************
*/
#define IOP_AMR_ROUTER_MAINTNET            31

/**
******************************************************************************
@brief        从节点主动上报开关设置
@verbatim
            IOP_AMR_ROUTER_REPORT_SWITCH    ∷= SEQUENCE
            {
                端口号                : double-long-unsigned
                开关参数            : BOOLEAN
            }            
@endverbatim            
******************************************************************************
*/
#define IOP_AMR_ROUTER_REPORT_SWITCH    32

/**
******************************************************************************
@brief        从节点监控延时获取
@verbatim
            IOP_AMR_FWDDELAY_GET ∷= SEQUENCE
            {
                端口号        : long-unsigned
                报文内容    : octet-string
            }
            ACK ∷= long-unsigned 
@endverbatim
******************************************************************************
*/
#define IOP_AMR_FWDDELAY_GET            33 

/**
******************************************************************************
@brief        从节点监控
@verbatim
             IOP_AMR_ROUTER_MONITOR ∷= SEQUENCE
            {
                端口号            : double-long-unsigned,
                规约类型        : unsigned,
                载波从节点地址    : octet-string(SIZE(6)),
                通信控制字        : unsigned, 
                帧超时时间        : long-unsigned, 
                字节超时时间    : long-unsigned, 
                发送报文        : octet-string
            }
            
            ACK ∷= SEQUENCE
             {
                报文传输时间    : long-usigned, 
                接收报文        : octet-string
            }
@endverbatim
******************************************************************************
*/
#define IOP_AMR_ROUTER_MONITOR            34

/**
 ******************************************************************************
 @brief    消息-获取广播延时
 @verbatim
         IOP_AMR_BDC_DELAY_GET ∷= SEQUENCE
        {
            端口号    : double-long-unsigned,
            规约    : unsigned,
            报文    : octet-string
        }

        ACK ∷= long-unsigned 
 @endverbatim
 ******************************************************************************
 */
#define IOP_AMR_BDC_DELAY_GET            35

/**
 ******************************************************************************
 @brief    消息-广播
 @verbatim
         IOP_AMR_BDC_START ∷= SEQUENCE
        {
            端口号    : double-long-unsigned
            规约    : unsigned 
            报文    : octet-string
        }
 @endverbatim
 ******************************************************************************
 */
#define IOP_AMR_BDC_START                36

/**
 ******************************************************************************
 @brief    消息-路由模块信息
 @verbatim
         IOP_AMR_MODULE_INFO ∷= double-long-unsigned(端口号)

         ACK ∷= SEQUENCE
        {
            模块型号                         : unsigned 
            本地通信模式字                   : octet-string(SIZE(6)) 
            从节点监控最大超时               : long-unsigned 
            广播最大超时时间                 : long-unsigned 
            最大支持报文长度                 : long-unsigned 
            升级最大单个包长度               : long-unsigned 
            升级操作等待时间                 : unsigned 
            广播成功延时时间                 : long-unsigned 
            模块编号                         : octet-string(SIZE(6))
            支持的最大从节点数量             : long-unsigned 
            当前从节点数量                   : long-unsigned 
            模块版本                         : visible-string
        }
 @endverbatim
 ******************************************************************************
 */
#define IOP_AMR_MODULE_INFO               37

/**
 ******************************************************************************
 @brief    消息-HPLC信息配置
 @verbatim
         IOP_AMR_HPLC_CONFIG ∷= SEQUENCE
         {
            端口号            : double-long-unsigned(端口号),
            频点            : unsigned,
            台区识别开关    : unsigned, 
            启停标志        : unsigned
         }        
 @endverbatim
 ******************************************************************************
 */
#define IOP_AMR_HPLC_CONFIG                38

/**
 ******************************************************************************
 @brief    消息-HPLC频点设置
 @verbatim
         IOP_AMR_HPLCFREQ_SET ∷= SEQUENCE
         {
            端口号    : double-long-unsigned(端口号),
            频点    : unsigned 
         }

        ACK ∷= BOOLEAN
 @endverbatim
 ******************************************************************************
 */
#define IOP_AMR_HPLCFREQ_SET                39

/**
 ******************************************************************************
 @brief    消息-HPLC频点查询
 @verbatim
         IOP_AMR_HPLCFREQ_GET ∷= double-long-unsigned(端口号)

        ACK ∷    = unsigned 
 @endverbatim
 ******************************************************************************
 */
#define IOP_AMR_HPLCFREQ_GET                40

/**
 ******************************************************************************
 @brief    消息-HPLC台区识别
 @verbatim
         IOP_AMR_HPLC_AREAID_SET ∷= SEQUENCE
         {
            端口号            : double-long-unsigned(端口号),
            台区识别开关    : unsigned 
         }
 @endverbatim
 ******************************************************************************
 */
#define IOP_AMR_HPLC_AREAID_SET             41

/**
 ******************************************************************************
 @brief    消息-HPLC离网感知
 @verbatim
         IOP_AMR_HPLC_OFFLINE_SET ∷= SEQUENCE
         {
            端口号            : double-long-unsigned(端口号),
            离网感知开关    : unsigned 
         }

        ACK ∷= BOOLEAN
 @endverbatim
 ******************************************************************************
 */
#define IOP_AMR_HPLC_OFFLINE_SET             42

/**
 ******************************************************************************
 @brief    消息-判断是否可以自动抄表
 @verbatim
         IOP_AMR_METERING_ENABLE ∷= double-long-unsigned(端口号)

         ACK ∷= unsigned (自动抄表状态)
 @endverbatim
 ******************************************************************************
 */
#define IOP_AMR_METERING_ENABLE             43

/**
 ******************************************************************************
 @brief    消息-路由初始化
 @verbatim
         IOP_AMR_ROUTER_INIT ∷= double-long-unsigned(端口号)
 @endverbatim
 ******************************************************************************
 */
#define IOP_AMR_ROUTER_INIT                 44

/**
 ******************************************************************************
 @brief    消息-请求依通信延时修正通信数据
 @verbatim
        IOP_AMR_COMM_DELAY       ∷= SEQUENCE
        {
            端口号                    : double-long-unsigned
            规约类型                : unsigned 
            载波从节点地址            : octet-string(SIZE(6))
            通信控制字                 : unsigned 
            帧超时时间                 : long-unsigned 
            字节超时时间            : long-unsigned 
            发送报文                : octet-string
        }
        
        ACK ∷= SEQUENCE
        {
            报文传输时间            : long-usigned 
            接收报文                : octet-string
        }
 @endverbatim
 ******************************************************************************
 */
#define IOP_AMR_COMM_DELAY                 45

/**
 ******************************************************************************
 @brief    消息-路由运行信息
 @verbatim
         IOP_AMR_RUN_INFO ∷= double-long-unsigned(端口号)
        
         ACK ∷= SEQUENCE
        {
            本地通信模式字            : octet-string(SIZE(6)),
            从节点监控最大超时        : long-unsigned,
            广播最大超时时间        : long-unsigned, 
            最大支持报文长度        : long-unsigned, 
            升级最大单个包长度        : long-unsigned, 
            升级操作等待时间        : unsigned, 
            广播成功延时时间        : long-unsigned, 
            模块编号                : octet-string(SIZE(6)),
            支持的最大从节点数量    : long-unsigned, 
            当前从节点数量            : long-unsigned, 
            模块版本                : visible-string
        }
 @endverbatim
 ******************************************************************************
 */
#define IOP_AMR_RUN_INFO                 46
/**
 ******************************************************************************
 @brief    消息-模块地址设置
 @verbatim
         IOP_AMR_MODULE_ADDR ∷= octet-string(SIZE(6))
 @endverbatim
 ******************************************************************************
 */
#define IOP_AMR_MODULE_ADDR               47

#ifdef __cplusplus
}
#endif

#endif // __AMR_MESSAGE_INTERFACE_H__
