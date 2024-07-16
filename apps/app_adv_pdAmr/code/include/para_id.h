/*
*******************************************************************************
 Copyright (c) 2006-2007,南京新联电子仪器有限公司项目部
             All rights reserved.
 程序名称：para_id.h
 功    能: 参数项定义
*******************************************************************************
*/

#ifndef _PARA_ID_H
#define _PARA_ID_H

#define TMN_RUN_PARA_NUM	166		//--终端运行参数总数目--
#define TMN_FUNC_PARA_NUM	282		//--终端功能参数总数目--
#define JLD_PARA_NUM		 88     //--计量点参数项数目--
#define TMN_EXT_PARA_NUM  53        //终端扩展参数--
#define MTR_EXT_PARA_NUM   3        //测量点扩展参数--
#define TMN_EXT_PARA_AREA_NUM   1  //各个地区的终端增补参数--
#define MTR_EXT_PARA_AREA_NUM   1  //各个地区的测量点增补参数

//-平台定义的终端运行参数-
enum tmn_run_para
{
    A1A2_Addr_ID=0,          // 2字节,BCD，终端行政区划码，表示本地(市)行政区划代码
    Tmr_Addr_ID,             // 2字节，终端地址，表示该终端的地址编号。FFFF为广播地址，0000为无效地址
    Tmn_Addr_OOP_ID,         // 17字节，面向对象协议终端的地址，首字节为长度，2-16字节为BCD码
    Cch_Type_ID,             // 2字节，终端与主站通信所采用的主通道类型。二进制数据体，前1字节表示通信通道，
                                // =1：短信；=2：无线公网;   3：DTMF；4：以太网；=5：红外；=6：RS232；=7：CSD；=8：Radio; 
                                // =9：TD-SCDMA; =10：TD-SCDMA-GSM; =11:WDASN_Radio; =12:高速电台
                                // 第二字节表示公网信道的工作模式:1表示永久在线;   2 表示被动激活模式
    Cch_Type_ID2,            // 2字节，备用一通信类型，格式同上

    Cch_Type_ID3,            // 2字节，备用二通信类型，格式同上
    Cch_Tel_Code_ID,         // 16字节,visible-string，主用主站电话号码与终端通信的主站电话号码对普通电话信道，前8字节有效，代表16位电话号码，后8字节无效。
                                // 每位数据范围0----B，0—9代表号码0----9，A为','，代表拨号停顿；B为'#'，F为无效填充
                                // 对GSM信道，前8字节，代表主站手机电话号码，后8字节代表短消息服务中心电话号码。
                                // (均没有“+”)按字面顺序存放,如13812345678,第1字节放13
    Cch_Tel_Code_ID2,        // 16字节,visible-string，备用一主站电话号码，格式同上
    Sms_Notify_Addr_ID,      // 16字节,visible-string，短信通知目的号码
    Cch_Ip_Addr_ID,          // 6字节，主用主站ip地址，前4个字节每个字节表示ip地址的一段，后两个字节表示端口号

    Cch_Ip_Addr_ID2,         // 6字节，格式同上，备用一主站ip地址
    Cch_Ip_Addr_ID3,         // 6字节，格式同上，备用二主站ip地址
    Tmn_Ip_Addr_698_ID,      // 4字节,终端IP地址（主站下发）
    Mask_Addr_698_ID,        // 4字节,子网掩码地址（主站下发）
    Default_Gw_Addr_ID,      // 6字节,默认网关地址,前4个字节每个字节表示ip地址的一段，后两个字节表示端口号

    Proxy_Typr_ID,           // 1字节,代理服务器类型0~3依次表示:不使用代理,http connect代理,socks4代理,socks5代理
    Proxy_Addr_ID,           // 6字节,代理服务器地址,前4个字节每个字节表示ip地址的一段，后两个字节表示端口号
    Proxy_Link_Type_ID,      // 1字节,代理服务器连接方式:0~1依次表示:无须验证,需要用户名/密码
    Name_Lenth_ID,           // 1字节,用户名长度0~20,0表示无用户名
    Name_ID,                 // 20字节,用户名ASCII码

    Password_Length_ID,      // 1字节,密码长度,0表示无密码
    Password_ID,             // 20字节,bin密码ASCII码
    DT_Port_ID,              // 2字节,侦听端口号
    Tmn_Work_Mode_ID,        // 1字节,终端工作模式:0-混合模式;1-客户机模式;2-服务器模式
    APN_Addr_Len_ID,         // 1字节，APN地址长度

    APN_Addr_ID,             // 16字节，APN地址，ASCII码，用于GPRS登陆。默认为'CMNET'
    Com_Para_ID,             // 3*4字节，端口参数，用于radio、rs232等通信方式下的端口参数配置
                                // 共配置3个端口，每个端口配置为4个字节, 包括端口波特率、校验位、数据位、停止位
                                // 第1字节表示端口波特率，HEX码，值为端口通信波特率/300；
                                // 第2字节表示校验方式，0无校验、1偶校验、2奇校验；
                                // 第3字节表示端口数据位；
                                // 第4字节表示停止位，0：1位、1：1.5位、2：2位
    Tmr_Group_Addr_ID,       // 136字节，8个终端组(片)地址，终端的组地址编码信息
                                // 每个组地址17字节,地址最大16字节BCD，第1字节为地址长度
                                // 共支持8个组地址
    NET_PROTOCOL_ID,         // 1字节，TCP/UDP选择，为1表示TCP，为0表示UDP
    NET_TYPE_ID,             // 1字节，CDMA/GPRS选择，为1表示CDMA，为0表示GPRS

    SLEEP_TIME_ID,           // 2字节，CDMA/GPRS休眠时间, 单位: 分钟
    Heart_Beat_Time_ID,      // 2字节，心跳周期，单位为秒
    Radio_Delay_ID,          // 1字节，终端数传机延时时间，20ms的整数倍
    Timeout_Retry_ID,        // 2字节，终端等待从动站响应的超时时间和重发次数
                                // #1:确认超时由字节1的D0-D7和字节2的D0-D3组成0-4095秒范围的超时时间。
                                // #2:重发次数由字节2的D4-D5组成，最多3次,为0表示不允许重发。字节2的D6-D7备用
    Max_Delta_Time_ID,       // 1字节，允许发送传输延时时间，单位为分

    Svc_Cnfrm_ID,            // 2字节，需要主站确认的通信服务标志,按位表示通信服务，1表示确认，0表示不确认
                                // #1:D0:1类数据自动上报,D1:2类数据自动上报,D2:3类数据自动上报,D3~D7备用
                                // #2:备用
    TMN_Serial_Num_ID,       // 21字节，终端编号(客户编号)，终端出厂前的物理编号(SN)
                                // 第一字节为长度，最长20字节，一般为数字,包含出厂时间编次等信息
    TMN_Capital_SN_ID,       // 33字节，终端资产管理编号，第一字节为长度，最长32字节
    SOFTWARE_VERSION_ID,     // 4字节，BCD，当前正在运行的终端软件版本号
    SOFTWARE_VERSION_INN_ID, // 4字节，BCD，终端软件内部版本号

    SPECIALNET_NAME_ID,      // 32字节ASCII码，虚拟专网用户名，用于GPRS登陆，默认为'CMNET'
    SPECIALNET_PWD_ID,       // 32字节ASCII码，虚拟专网密码
    Sys_Mark_ID,             // 32字节ASCII码 地区缩写,系统识别码(备忘:376.1规约‘终端配置容量信息码’用的是Sys_Mark_ID的前11字节)
    Idle_Time_ID,            // 1字节，单位为分，激活方式连续无通信自动断线时间
    Online_Time_ID,          // 3字节，终端时段在线参数

    Login_Inter,             // 2字节，单位为秒，GPRS/CDMA登陆时间间隔
    Log_Max_Unit,            // 1字节，GPRS/CDMA登陆的次数
    Tmn_Traffic_Lmt,         // 月通信流量保护门限,BIN,4字节,若通信流量设置为0，表示终端不进行流量控制
    Relay_Addr_ID,           // 33字节，中继转发设置，第1字节D7为1表示允许终端转发,为0为不转发；D6-D0位为被转发终端数N
                                // 当终端允许转发时，以下字节为N个被转发的终端地址，每个地址为2字节。
                                // 转发终端数N最大值为16。被转发终端地址按地址域定义
    TMN_RADIO_RS_EN,         // 1字节，RS编码标志使用，0 标识无RS编码  1 表示RS11/15  2 表示RS9/15

    TRANS_REASON_LEN,        // 1字节，传送原因地址长度
    Tmr_Addr_ID_LEN,         // 1字节，链路地址长度
    PUBLIC_ADDR_LEN,         // 1字节，公共地址长度
    INFO_ADDR_LEN,           // 1字节，信息体地址长度
    PROTOCAL_TYPE_ID,        // 1字节，归约类型,1=101，4=104，3=376.1

    CAP_SWITCH_READ_ID1,     // 无功投切装置对应遥信抄表的参数
    CAP_SWITCH_READ_ID2,     // 无功投切装置对应遥信抄表的参数
    CAP_SWITCH_READ_ID3,     // 无功投切装置对应遥信抄表的参数
    CAP_SWITCH_READ_ID4,     // 无功投切装置对应遥信抄表的参数
    CAP_SWITCH_READ_ID5,     // 无功投切装置对应遥信抄表的参数

    CAP_SWITCH_READ_ID6,     // 无功投切装置对应遥信抄表的参数
    FAC_CODE_ID,             // 4字节，厂商代号
    HARDWARE_VERSION_ID,     // 4字节，硬件版本号
    CasSub_Tmn_List,         // 16字节，广东配变要求，级联从终端地址列表
    CasSub_Hub_List,         // 4字节,字节数由20减成4个广东配变要求，级联从集中器地址列表

    Cas_Tmn_Cfg,             // 1字节，广东配变级联主从配置
    EDMI_USER,               // 32字节ASCII码，EDMI电表用户名
    EDMI_PASW,               // 32字节ASCII码，EDMI电表密码
    PLC_MODULE_COMMON_ID,    // 2字节BIN码，东软III代模式使能(1:使能 / 0:无效 无线频道号#1:频道号(1~10) #2:预留通道板波特率，供弥亚威通道板使用，本字典在原有的EDMI_BPS上修改
    TMN_AREAINFO_ID,         // 1字节，江苏程序中作为地区标识使用 0=江苏系统  1=南京系统  2=宜兴系统。

    CTJC_A_Sec_Open_ID,      // 2字节，A相二次开路检测
    CTJC_A_Sec_Open_Max_ID,  // 2字节，A相二次开路检测上限
    CTJC_A_Sec_Open_Min_ID,  // 2字节，A相二次开路检测下限
    CTJC_A_Fir_Close_Max_ID, // 2字节，A相一次短路检测上限
    CTJC_A_Fir_Close_Min_ID, // 2字节，A相一次短路检测下限

    CTJC_A_Normal_Max_ID,    // 2字节，A相检测正常上限
    CTJC_A_Normal_Min_ID,    // 2字节，A相检测正常下限
    CTJC_A_Sec_Close_ID,     // 2字节，A相二次短路检测
    CTJC_B_Sec_Open_ID,      // 2字节，B相二次开路检测
    CTJC_B_Sec_Open_Max_ID,  // 2字节，B相二次开路检测上限

    CTJC_B_Sec_Open_Min_ID,  // 2字节，B相二次开路检测下限
    CTJC_B_Fir_Close_Max_ID, // 2字节，B相一次短路检测上限
    CTJC_B_Fir_Close_Min_ID, // 2字节，B相一次短路检测下限
    CTJC_B_Normal_Max_ID,    // 2字节，B相检测正常上限
    CTJC_B_Normal_Min_ID,    // 2字节，B相检测正常下限

    CTJC_B_Sec_Close_ID,     // 2字节，B相二次短路检测
    CTJC_C_Sec_Open_ID,      // 2字节，C相二次开路检测
    CTJC_C_Sec_Open_Max_ID,  // 2字节，C相二次开路检测上限
    CTJC_C_Sec_Open_Min_ID,  // 2字节，C相二次开路检测下限
    CTJC_C_Fir_Close_Max_ID, // 2字节，C相一次短路检测上限

    CTJC_C_Fir_Close_Min_ID, // 2字节，C相一次短路检测下限
    CTJC_C_Normal_Max_ID,    // 2字节，C相检测正常上限
    CTJC_C_Normal_Min_ID,    // 2字节，C相检测正常下限
    CTJC_C_Sec_Close_ID,     // 2字节，C相二次短路检测
    PULSE_CONST_ID,          // 2字节，BIN，电能表脉冲常数

    PLC_READ_RELAY,          // 2个字节BIN  抄载波电表测量点的时候，是否使用中继的方式抄读 0xAA——中继方式 其他为不中继 默认为不中继
    Pulse_RmtSig_Config,     // 该参数已不使用
    JOIN485_ComPara_ID,      // 4字节，级联485端口参数，包括端口波特率、校验位、数据位、停止位
                                // 第1字节表示端口波特率，HEX码，值为端口通信波特率/300；
                                // 第2字节表示校验方式，0无校验、1偶校验、2奇校验；
                                // 第3字节表示端口数据位；
                                // 第4字节表示停止位，0：1位、1：1.5位、2：2位
    RS232_ComPara_ID,        // 4字节，本地232端口参数
    WIRELESS_ComPara_ID,     // 4字节，小无线端口参数

    AC_Config_577_ID,        // 1字节,手动选择三相四线终端额定电压是220V还是57.7V：在已确定交采硬件为三相四线的前提下，配置为0x55时表示57.7V，否则按默认220V处理；当确定交采硬件为三相三线时,一律按110V处理，忽略AC_Config_577_ID
    WL_CHANNLE_GROUP_ID,     // 1字节，无线信道号：0～63
    WL_NETWORK_TRANSMITTING_POWER,// 1字节，编码表示主无线模块发射功率：00  最高发射功率  01 次高发射功率  02  次低发射功率  03 最低发射功率  04～07自定义0～63
    SIM_NUM_ID,              // 20字节，BIN，SIM卡号
    Longitude_Value,         // 5字节，BCD，经度度值，按规约格式直接存储，附录A28

    Latitude_Value,          // 5字节，BCD，纬度度值，按规约格式直接存储,附录A.28
    Height_Value,            // 4字节，BIN，高度，按规约格式直接存储,附录A.28
    AUTOGETMTR_TIME,         // 2字节，自动搜表的起始时间
    IP_FIRSE_ID,             // 5字节，第一网络节点的IP地址、有效性（前4字节为IP、第5字节为有效性；一般指eth0，供以太网口使用）
    IP_SECOND_ID,            // 5字节，第二网络节点的IP地址、有效性（前4字节为IP、第5字节为有效性；一般指eth1，供宽带载波使用）

    ETH_GPRS_SWITCH_AUTO,    // 1字节,启用以太网自动切换,0xaa-不启用 0x55-启用,默认开启
    Gprs_Connect_Type_ID ,   // 1字节，无线公网接口类连接应用方式  enum{主备模式（0），多连接模式（1）
    Check_Time_Mode_ID ,     // 1字节，校时模式
    AT4G_GUI_MODE ,          // 1字节，1:显示为4G，其他:显示为2G
    TMN_RESERVED_U08_1_ID ,  // 1字节，保留ID，防止日后扩充

    TMN_RESERVED_U08_2_ID ,  // 1字节，保留ID，防止日后扩充
    TMN_RESERVED_U08_3_ID ,  // 1字节，保留ID，防止日后扩充
    TMN_RESERVED_U08_4_ID ,  // 1字节，复位标志，3远方复位，4停电复位，8本地复位
    TMN_RESERVED_U08_5_ID ,  // 1字节，保留ID，防止日后扩充
    TMN_RESERVED_U08_6_ID ,  // 1字节，保留ID，防止日后扩充

    TMN_RESERVED_U08_7_ID ,  // 1字节，保留ID，防止日后扩充
    TMN_RESERVED_U08_8_ID ,  // 1字节，保留ID，防止日后扩充
    TMN_RESERVED_U16_1_ID ,  // 2字节，遥测循环上报间隔，单位秒
    TMN_RESERVED_U16_2_ID ,  // 2字节，电量循环上送间隔，单位秒
    TMN_RESERVED_U16_3_ID ,  // 2字节，背景扫描上送间隔，单位秒

    TMN_RESERVED_U16_4_ID ,  // 2字节，保留ID，防止日后扩充
    TMN_RESERVED_U16_5_ID ,  // 2字节，保留ID，防止日后扩充
    TMN_RESERVED_U16_6_ID ,  // 2字节，保留ID，防止日后扩充
    TMN_RESERVED_U16_7_ID ,  // 2字节，保留ID，防止日后扩充
    TMN_RESERVED_U16_8_ID ,  // 2字节，保留ID，防止日后扩充

    TMN_RESERVED_U32_1_ID ,  // 4字节，保留ID，防止日后扩充
    TMN_RESERVED_U32_2_ID ,  // 4字节，保留ID，防止日后扩充
    TMN_RESERVED_U32_3_ID ,  // 4字节，保留ID，防止日后扩充
    TMN_RESERVED_U32_4_ID ,  // 4字节，保留ID，防止日后扩充
    TMN_RESERVED_U64_1_ID ,  // 8字节，保留ID，防止日后扩充

    TMN_RESERVED_U64_2_ID ,  // 8字节，保留ID，防止日后扩充
    TMN_RESERVED_U64_3_ID ,  // 8字节，保留ID，防止日后扩充
    TMN_RESERVED_U64_4_ID ,  // 8字节，保留ID，防止日后扩充
    Device_Describe_Len,     // 1字节,设备描述符长度
    Device_Describe,         // 32字节,设备描述符

    FACTORY_EXP_INFO_LEN,    // 1字节,厂商扩展信息长度
    FACTORY_EXP_INFO,        // 8字节,厂商扩展信息
    PRODUCT_DATE,            // 7字节,生产日期
    JLM_LOCATION,            // 计量门在遥信配置中的位置,该参数已不使用
    State_Access_ID,         // 1字节，状态量接入标志位，D0~D7依次对应1~8路状态量输入，1：接入，0：未接入

    State_Property_ID,       // 1字节，状态量属性标志位，D0~D7依次对应1~8路状态量输入，置1：a型触点。0：b型触点
    State_Alert_ID,          // 1字节，状态量告警标志位，D0~D7按位表示，
                                // 置1：对应的状态量发生变位产生的告警事件为重要事件，即应按有关重要告警事件进行处理
                                // 置0：对应的状态量发生变位产生的告警事件为一般告警事件
    Anology_Access_ID,       // 1字节，直流模拟量接入标志位，D0~D7对位表示第1~第8路的模拟量输入端口
                                // 置1：接入，置0：未接入，缺省为0
    Anology_Ratio_ID,        // 2*8字节，直流模拟量变比，其数据格式参见04规约数据格式02
    Anology_Hign_Limit_ID,   // 8*8字节，直流模拟量上限，格式同上(备注:适应A2数据格式的定义,最小为0.0001,edit time 10.04.16)

    Anology_Low_Limit_ID,    // 8*8字节，直流模拟量下限，格式同上(备注:适应A2数据格式的定义,最小为0.0001,edit time 10.04.16)
    Anology_Ratio_Vals_ID,   // 8*8字节，直流模拟量量程起始值,最小0.0001KW(备注:适应A2数据格式的定义,最小为0.0001,edit time 10.04.16)
    Anology_Ratio_Kb_ID,     // 8*8字节，直流模拟量量程终止值,最小0.0001KW(备注:适应A2数据格式的定义,最小为0.0001,edit time 10.04.16)
    Pulse_Num_ID,            // 1字节，脉冲量数量
    Pulse_State_ID,          // 8字节，脉冲量状态标记，脉冲量状态标记，每位从低到高表示每个脉冲量的状态 0：无效；1：有效

    Pulse_Cfg_Info_ID,       // 6*8字节,从OI-0x2401为1开始的编号,脉冲属性,电表常数,每六个字节代表一路脉冲的属性配置
                                // 最高字节表示该路脉冲对应的测量点号,次高字节表示脉冲属性
                                // D0~D2表示脉冲属性，D3~D7备用，000正向有功、001反向有功、010正向无功、
                                // 011反向无功、100正向感性无功、101正向容性无功、110反向容性无功、111反向感性无功
                                // 低4个字节表示电表常数
    BFG_EVENT_SEND,          // 1字节,允许终端主动上报标志,1允许,0不允许
    BFG_FOLLOW_SEND,         // 1字节,允许终端跟随上报标志,1允许,0不允许
    Summ_ConfigAdd_ID,       // 1字节,总加组配置组数
    AC_COMM_BAUT_ID,         // 2字节BIN,交采校表端口速率

    SECURITY_MOD_ON_ID,      // 1字节,安全模式选择，0不起用安全模式参数，1起用安全模式参数
    SECURITY_MOD_NUM_ID,     // 2字节,安全模式参数个数
    SECURITY_MOD_PARA_ID,    // 4X500字节,安全模式参数
    ALARM_TMSEG_ID,          // 8*6字节,告警运行时段
    Relay_Switch_Attr_ID ,   // 2字节，继电器单元开关属性

    TMN_RUN_PAPA_RESERVE_ID, // 1024字节,备用
};

//-平台定义的终端功能参数-
enum tmn_func_para
{
    Tariff_SEG_ID=0,         // 48字节,电量费率时段,电量尖峰平谷时段，
                                // 每4bit表示半小时时间间隔，24个字节按位依次表示48个半小时区间
                                // 00为平、01为峰时段、10为谷时段、11为尖时段
    Protect_Exclude_Para_ID, // 1字节,保电剔除参数。D0=0：非保电；D0=1：保电;  D4=1 剔除投入 D4=0 剔除解除
    Protect_Exclude_Time_ID, // 2字节,终端无通信允许时间（自动保电),终端连续未收到符合本区域地址的有效主站报文的时间
                                // 当超过规定的无通信时间，则自动保电；该值为0则不自动保电。值为FFH即为解除
    Protect_PowerOnDelayTime,// 2字节,上电自动保电时长（分钟），0表示上电不自动保电
    Protect_PowerOnTimeSeg,  // 17字节,第一字节时段个数，2*8,(起始时间(时)+结束时间(时))*8

    PC_Alarm_Tm_Para_ID,     // 8字节,功控告警时间,功率控制告警时间表示超过功率设定值跳闸的过负荷持续时间。单位：分钟
                                // N表示总共有N轮跳闸(淅江的hhmmss格式统一转换为分钟数，一个字节，没有轮次概念)
    Keep_Base_Constant,      // 8字节,功控保底定值,单位:0.0001kw(原先为4个字节,单位0.0001)
    PC_SEG_TIME_ID,          // 16字节,功控时段,终端功控时段,同04格式,使用了12字节，后4字节预留
    PC_Fl_Rate_Para_ID,      // 1字节,功率定值浮动系数,有符号数,单位%
    Mon_En_Ctrl_Fl_Rate_ID,  // 1字节,月电控定值浮动系数,同功率定值浮动系数

    Energy_Tariff_Num_ID,    // 1字节,终端电能量费率数
    Energy_Super_Lack_ID,    // 4字节bin,电能量超差阈值, 倍率 单位： 0.01
    Energy_Super_Lack_Task_ID,// 1字节bin,电能量超差阈值关联采集任务号
    Energy_Flying_ID,        // 4字节bin,电能表飞走阈值, 倍率 单位： 0.01
    Energy_Flying_Task_ID,   // 1字节bin,电能表飞走关联采集任务号

    Energy_Stoping_ID,       // 2字节bin,电能表停走阈值,单位：分钟,如＝2 ,代表2分钟
    Energy_Stoping_Task_ID,  // 1字节bin,电能表停走关联采集任务号
    Energy_Tune_Time_ID,     // 2字节bin,电能表校时阈值,单位：1秒
    Energy_Tune_Time_Task_ID,// 1字节bin,电能表校时关联采集任务号
    CURR_REV_CURR_ID,        // 2字节,电流反极性之电流阀值,扩大100倍

    PCtrl_SetFree_Info_ID,   // 1*16字节,功控投入标志D0时段控，D1厂休控 D2 营业报停控，D3 下浮控其中置  1投入、0解除
    PC_Turn_Para_ID,         // 8*16字节,功控轮次有效标志,功率控制轮次有效标志
                                // 共8字节64bit，按位表示64轮，值为1有效（投入）、0无效（不投入）
    TC_SEG_Flag_ID,          // 3*16字节,时段控时段投入标志,时段控投入标志：1投入、0解除(浙江中每位对应8个时段的标志)
    TC_SEG_TAC_NO_ID,        // 1*16字节,时段控投入方案号
    TC_SEG_TAC_VALID_ID,     // 1*16字节,时段控方案有效性标识,bit0~bit2分别标识第一~第三套，1:有效，0:无效

    PC_SEG_Value_ID1,        // 8*8*16字节,功控时段定值1,单位kw,1套8个定值，支持总加组1-16(备注:原先定值为4个字节,目前支持最小0.0001kw修改时间10.4.15)
    PC_SEG_Value_ID2,        // 格式同上
    PC_SEG_Value_ID3,        // 格式同上
    Rest_PC_Para_ID,         // 12*16组字节,厂休控参数,包括厂休控功率定值、厂休控起始时间、厂休控持续时间、厂休日等
                                // 功率定值8字节(1次值)；厂休控起始时间2个字节(bcd码,分时)
                                // 厂休控持续时间2个字节，bin，单位：分钟；
                                // 厂休日1个字节，D1~D7表示星期一~日，D0=0(备注:原先定值为4个字节,目前支持最小0.0001kw修改时间10.4.15)
    Ectrl_SetFree_Info_ID,   // 1*16字节,电控投入标志,D0:日电控，D1:月电控,D2:购电控  置1投入、0解除

    EC_Turn_Para_ID,         // 8*16字节,电控轮次有效标志,电控轮次有效标志
                                // 共8个字节64bit表示64轮电控跳闸有效标志，1有效、0无效
    Day_En_Ctrl_Para_ID,     // 8*16字节,日电控电量定值,电量定值8字节表示,支持16个总加组,日用电量的限额，为一次值，单位kwh
    Mon_En_Ctrl_Para_ID,     // 8*16字节,月电控电量定值,格式同上
    Mon_En_Alarm_Para_ID,    // 1*16字节,月电控电量告警门限系数,格式BCD
    Buy_En_Ctrl_Para1_ID,    // 4*16字节,购电单号,购电的时间信息，一般是分时日月,BCD码

    APPEND_REF_TAG_ID,       // 1*16字节,追加/刷新标志,置“55H”：追加；置“AAH” 刷新；其它数值无效
    Buy_En_Ctrl_Para2_ID,    // 8*16字节,购电量,当前用户所购买的电量或电费，参见上面电量定值格式
                                // (有符号数，不是规约格式，可直接用于有符号数计算),支持16个总加组
    Buy_En_Ctrl_Para3_ID,    // 8*16字节,剩余电量,格式同上
    Buy_En_Ctrl_Para4_ID,    // 8*16字节,告警电量,用户的剩余购电量（电费）一旦小于某一电量（电费）定值时，终端开始报警
                                // 该电量（电费）的值称为告警电量（电费），格式同上
    Buy_En_Ctrl_Para5_ID,    // 8*16字节,跳闸电量,当用户购电量（电费）用完，终端跳完第一轮后所用的电量（电费）
                                // 超过某一电量（电费）值后，终端会跳下一轮，该电量（电费）值称为步长电量（电费），格式同上

    Buy_En_Ctrl_Para6_ID,    // 1*16字节,购电类型 1:电费，0:电量，其他:保留
    Buy_En_Ctrl_Para7_ID,    // 1*16字节,购电控模式 1:远程模式，0:本地模式，其他:保留
    Power_Ctrl_Valid_ID,     // 1*16字节,功控有效标识，bit0:时段控，bit1:厂休控，bit2:报停控，1:有效，0:无效
    Elec_Ctrl_Valid_ID,      // 1*16字节,电控有效标识，bit0:购电控，bit1:月电控，1:有效，0:无效
    Energy_Tariff_ID,        // 4*48字节,终端电能量费率,04规约数据格式03,4字节*14费率

    Press_Money_Ctrl_ID,     // 1*16组字节,催费告警投入标志,1投入、0解除
    Press_Money_Ctrl_Para_ID,// 3字节,催费告警参数,催费告警允许∕禁止标志位，D0~D23按位顺序对位表示0~23点
                                // 每位对应表示1个小时，置“1”：告警，置“0”：不告警
    StopCrtl_Start_End_ID,   // 6*16字节,营业报停控报停起始、结束时间,支持16个总加组,
                                // 日月年,起始时间在前，结束时间在后
    StopCrtl_Power_Value_ID, // 8*16字节,营业报停控功率定值,8字节,支持16个总加组0.0001kw(备注:原先定值为4个字节,修改时间10.4.15)
    Power_Calcatute_Time_ID, // 1*16字节,功率计算滑差时间,支持16个总加组,功率控制的功率计算滑差时间（范围：1~60），单位：分

    Buy_En_Ctrl_Para_Flag_ID,// 1字节,电量电费控标志,1投入、0解除
    Tmn_Reset_Flg_ID,        // 1字节,终端复位标志
    Rmt_Ctrl_Info_ID,        // 遥控控制情况 8×4bytes
    PowerDown_Ctrl_Para_ID,  // 当前功率下浮控 8×4bytes
    Buy_En_Ctrl_ID,          // 16*16组，购电量前电量8bytes购电后电量8bytes

    PowerDown_Turn_Time_ID,  // 当前功率下浮控第1,2,3,4轮告警时间4bytes;控制时间1byte
    PowerDown_Ctrl_Value_ID, // 功率下浮控定值,8字节,单位: 0.0001kw(备注:原先定值为4个字节,修改时间10.4.15)
    Tmn_Protect_Time_ID,     // 终端保电投入持续时间 2byte (单位: min)
    TMN_JC_MTR_NUM_ID,       // 2字节，终端配置的交采电表总数  
    TMN_RS485_MTR_NUM_ID,    // 2字节，终端配置的485电表总数  

    TMN_PLC_MTR_NUM_ID,      // 2字节，终端配置的载波电表总数  
    TMN_JC_JLD_NO,           // 2字节，终端交采测量点号，终端只允许有1个测量点
    Tmn_Flux_Over_Flag_ID,   // 1字节,终端月流量超标标志:1超标,0未超标
    Start_MainTain_time,     // 42字节 ,1:搜表时段启动标识(1启动)，2:搜表时段个数，8*5:前3字节时间，后2字节搜表时长
    End_MainTain_time,       // 2字节 ,t2搜表结束时间 B1: 小时 B0: 分钟

    PLC_SEARCH_UPDATE_FLAG,  // 1字节 ,自动更新采集档案BOOL
    PowOff_CB_Flag,          // 1字节,停电数据采集标志
    PowOff_CB_Interval,      // 1字节,停电事件抄读时间间隔
    PowOff_CB_TimeLimited,   // 1字节 ,停电事件抄读时间限值
    PowOff_CB_Mtr_Num,       // 2字节 ,需要读取停电事件电能表个数

    PowOff_CB_Mtr_Tsa,       // 17*16字节 ,需要读取停电事件电能表
    PowOff_CB_Mtr_1,         // 2字节 ,需要读取停电事件测量点号1
    PowOff_CB_Mtr_2,         // 2字节 ,需要读取停电事件测量点号2
    PowOff_CB_Mtr_3,         // 2字节 ,需要读取停电事件测量点号3
    PowOff_CB_Mtr_4,         // 2字节 ,需要读取停电事件测量点号4

    PowOff_CB_Mtr_5,         // 2字节 ,需要读取停电事件测量点号5
    PowOff_CB_Mtr_6,         // 2字节 ,需要读取停电事件测量点号6
    PowOff_Min_Time,         // 2字节 ,停电时间最小有效间隔
    PowOff_Max_Time,         // 2字节 ,停电时间最大有效间隔
    PowOff_Start_Time_Dev,   // 2字节 ,停电事件起止时间偏差限值

    PowOff_Sec_Time_Dev,     // 2字节 ,停电事件时间区段偏差限值
    PowOff_Set_Vol,          // 2字节 ,终端停电发生电压限值
    PowOff_Reset_Vol,        // 2字节 ,终端停电恢复电压限值
    PowOff_AutoSend_Flag,    // 1字节 ,停电事件主动上报标志
    Last_Battery_Flag,       // 1字节 ,上一次电池状态

    Broadcast_Check_Time_Flag_ID,// 1字节 BOOL, 终端广播校时启动开关标识 1:启动 其它:不启动
    Broadcast_Start_Time_ID, // 3字节 time, 终端广播校时启动时间
    SB_Check_Time_Flag_ID,   // 1字节 BOOL, 终端单地址广播校时启动开关标识 1:启动 其它:不启动
    SB_Start_Time_ID,        // 3字节 time, 终端单地址广播校时启动时间
    SB_Time_Err_ID,          // 2字节 BIN, 终端单地址广播校时时钟误差阈值

    PowerOn_Show_Time,       // 1字节 BIN, 单位为秒,上电全显时间
    Led_Open_Time,           // 2字节 BIN, 单位为秒,背光点亮时间
    Watch_Gui_Led_Open_Time, // 2字节 BIN, 单位为秒,显示查看背光点亮时间
    PowerOff_Led_Hold_Time,  // 2字节 BIN, 单位为秒,无电按键屏幕驻留最大时间
    Show_En_Decimal_Digits,  // 1字节 BIN, 显示电能小数位数

    Show_Demand_Decimal_Digits,// 1字节 BIN, 显示功率（最大需量）小数位数
    Show_Triff_PlanNo,       // 1字节 BIN, 液晶①②字样意义
    Moni_Mtr_Data_Task_ID,   // 1字节 BIN, 电能表数据变更监控记录任务号
    Energy_Down_Task_ID,     // 1字节 BIN, 电能表示数下降关联采集任务号
    Last_Heart_Time_Cnt_ID,  // 1字节 BIN, 最近心跳时间总个数

    Max_Exclude_Cnt_ID,      // 1字节 BIN, 最大值剔除个数
    Min_Exclude_Cnt_ID,      // 1字节 BIN, 最小值剔除个数
    Delay_Comm_Lmt_Val_ID,   // 1字节 BIN, 通讯延时阈值（单位：秒）
    Least_Valid_Num_ID,      // 1字节 BIN, 最少有效个数
    Report_Com_Oad_List_ID,  // 40字节, 上报通道OAD列表

    Join_En_ID,              // 1字节 BIN, 级联标志
    Join_Comm_Oad_ID,        // 4字节 BIN, 级联通信端口号OAD
    Join_Recv_Over_Time_ID,  // 2字节 BIN, 总等待超时（10ms）
    Join_Byte_Over_Time_ID,  // 2字节 BIN, 字节超时（10ms）
    Join_Retry_Times_ID,     // 1字节 BIN, 重发次数 

    Join_RECYC_Inter_ID,     // 1字节 BIN, 巡测周期（min）
    Join_Comm_Num_ID,        // 1字节 BIN, 级联（被）端口数
    Join_Tmn_Num_ID,         // 1字节 BIN, 级联终端个数和地址关联，不超过8个
    Join_Tmn_Addr_01_ID,     // 17字节 BIN, 第一字节长度，级联（被）终端地址01
    Join_Tmn_Addr_02_ID,     // 17字节 BIN, 第一字节长度，级联（被）终端地址02

    Join_Tmn_Addr_03_ID,     // 17字节 BIN, 第一字节长度，级联（被）终端地址03
    Join_Tmn_Addr_04_ID,     // 17字节 BIN, 第一字节长度，级联（被）终端地址04
    Join_Tmn_Addr_05_ID,     // 17字节 BIN, 第一字节长度，级联（被）终端地址05
    Join_Tmn_Addr_06_ID,     // 17字节 BIN, 第一字节长度，级联（被）终端地址06
    Join_Tmn_Addr_07_ID,     // 17字节 BIN, 第一字节长度，级联（被）终端地址07

    Join_Tmn_Addr_08_ID,     // 17字节 BIN, 第一字节长度，级联（被）终端地址08
    CBFali_Turn_Count,       // 1字节 BIN, 终端抄表失败事件 重试轮次
    CBFali_Task_ID,          // 1字节 BIN, 终端抄表失败事件 关联采集任务号
    Summ_Add_Mtsumnumber_ID, // 1*16字节,总加组1-16的测量点数量
    Summ_Add_Mtnumber_VIR1_ID,// 2*64字节,总加组1计量点号及总加标志,每个测量点的该属性为1个字节
                                // 其中 D0~D5编码表示64个测量点号，数值范围0~63
                                // D6表示测量点的正向还是反向有功/无功功率、有功/无功电能量参与总加的标志,置“0”正向；置“1”反向
                                // D7表示参与总加运算的运算符标志 置“0” 加运算;置“1”减运算

    Summ_Add_Mtnumber_VIR2_ID,// 2*64字节,总加组2计量点号及总加标志,每个测量点的该属性为1个字节
                                // 其中 D0~D5编码表示64个测量点号，数值范围0~63
                                // D6表示测量点的正向还是反向有功/无功功率、有功/无功电能量参与总加的标志,置“0”正向；置“1”反向
                                // D7表示参与总加运算的运算符标志 置“0” 加运算;置“1”减运算
    Summ_Add_Mtnumber_VIR3_ID,// 2*64字节,总加组3计量点号及总加标志,每个测量点的该属性为1个字节
                                // 其中 D0~D5编码表示64个测量点号，数值范围0~63
                                // D6表示测量点的正向还是反向有功/无功功率、有功/无功电能量参与总加的标志,置“0”正向；置“1”反向
                                // D7表示参与总加运算的运算符标志 置“0” 加运算;置“1”减运算
    Summ_Add_Mtnumber_VIR4_ID,// 2*64字节,总加组4计量点号及总加标志,每个测量点的该属性为1个字节
                                // 其中 D0~D5编码表示64个测量点号，数值范围0~63
                                // D6表示测量点的正向还是反向有功/无功功率、有功/无功电能量参与总加的标志,置“0”正向；置“1”反向
                                // D7表示参与总加运算的运算符标志 置“0” 加运算;置“1”减运算
    Summ_Add_Mtnumber_VIR5_ID,// 2*64字节,总加组5计量点号及总加标志,每个测量点的该属性为1个字节
                                // 其中 D0~D5编码表示64个测量点号，数值范围0~63
                                // D6表示测量点的正向还是反向有功/无功功率、有功/无功电能量参与总加的标志,置“0”正向；置“1”反向
                                // D7表示参与总加运算的运算符标志 置“0” 加运算;置“1”减运算
    Summ_Add_Mtnumber_VIR6_ID,// 2*64字节,总加组6计量点号及总加标志,每个测量点的该属性为1个字节
                                // 其中 D0~D5编码表示64个测量点号，数值范围0~63
                                // D6表示测量点的正向还是反向有功/无功功率、有功/无功电能量参与总加的标志,置“0”正向；置“1”反向
                                // D7表示参与总加运算的运算符标志 置“0” 加运算;置“1”减运算

    Summ_Add_Mtnumber_VIR7_ID,// 2*64字节,总加组7计量点号及总加标志,每个测量点的该属性为1个字节
                                // 其中 D0~D5编码表示64个测量点号，数值范围0~63
                                // D6表示测量点的正向还是反向有功/无功功率、有功/无功电能量参与总加的标志,置“0”正向；置“1”反向
                                // D7表示参与总加运算的运算符标志 置“0” 加运算;置“1”减运算
    Summ_Add_Mtnumber_VIR8_ID,// 2*64字节,总加组8计量点号及总加标志,每个测量点的该属性为1个字节
                                // 其中 D0~D5编码表示64个测量点号，数值范围0~63
                                // D6表示测量点的正向还是反向有功/无功功率、有功/无功电能量参与总加的标志,置“0”正向；置“1”反向
                                // D7表示参与总加运算的运算符标志 置“0” 加运算;置“1”减运算
    Summ_Add_Mtnumber_VIR9_ID,// 2*64字节,总加组9计量点号及总加标志,每个测量点的该属性为1个字节
                                // 其中 D0~D5编码表示64个测量点号，数值范围0~63
                                // D6表示测量点的正向还是反向有功/无功功率、有功/无功电能量参与总加的标志,置“0”正向；置“1”反向
                                // D7表示参与总加运算的运算符标志 置“0” 加运算;置“1”减运算
    Summ_Add_Mtnumber_VIR10_ID,// 2*64字节,总加组10计量点号及总加标志,每个测量点的该属性为1个字节
                                // 其中 D0~D5编码表示64个测量点号，数值范围0~63
                                // D6表示测量点的正向还是反向有功/无功功率、有功/无功电能量参与总加的标志,置“0”正向；置“1”反向
                                // D7表示参与总加运算的运算符标志 置“0” 加运算;置“1”减运算
    Summ_Add_Mtnumber_VIR11_ID,// 2*64字节,总加组11计量点号及总加标志,每个测量点的该属性为1个字节
                                // 其中 D0~D5编码表示64个测量点号，数值范围0~63
                                // D6表示测量点的正向还是反向有功/无功功率、有功/无功电能量参与总加的标志,置“0”正向；置“1”反向
                                // D7表示参与总加运算的运算符标志 置“0” 加运算;置“1”减运算

    Summ_Add_Mtnumber_VIR12_ID,// 2*64字节,总加组12计量点号及总加标志,每个测量点的该属性为1个字节
                                // 其中 D0~D5编码表示64个测量点号，数值范围0~63
                                // D6表示测量点的正向还是反向有功/无功功率、有功/无功电能量参与总加的标志,置“0”正向；置“1”反向
                                // D7表示参与总加运算的运算符标志 置“0” 加运算;置“1”减运算
    Summ_Add_Mtnumber_VIR13_ID,// 2*64字节,总加组13计量点号及总加标志,每个测量点的该属性为1个字节
                                // 其中 D0~D5编码表示64个测量点号，数值范围0~63
                                // D6表示测量点的正向还是反向有功/无功功率、有功/无功电能量参与总加的标志,置“0”正向；置“1”反向
                                // D7表示参与总加运算的运算符标志 置“0” 加运算;置“1”减运算
    Summ_Add_Mtnumber_VIR14_ID,// 2*64字节,总加组14计量点号及总加标志,每个测量点的该属性为1个字节
                                // 其中 D0~D5编码表示64个测量点号，数值范围0~63
                                // D6表示测量点的正向还是反向有功/无功功率、有功/无功电能量参与总加的标志,置“0”正向；置“1”反向
                                // D7表示参与总加运算的运算符标志 置“0” 加运算;置“1”减运算
    Summ_Add_Mtnumber_VIR15_ID,// 2*64字节,总加组15计量点号及总加标志,每个测量点的该属性为1个字节
                                // 其中 D0~D5编码表示64个测量点号，数值范围0~63
                                // D6表示测量点的正向还是反向有功/无功功率、有功/无功电能量参与总加的标志,置“0”正向；置“1”反向
                                // D7表示参与总加运算的运算符标志 置“0” 加运算;置“1”减运算
    Summ_Add_Mtnumber_VIR16_ID,// 2*64字节,总加组16计量点号及总加标志,每个测量点的该属性为1个字节
                                // 其中 D0~D5编码表示64个测量点号，数值范围0~63
                                // D6表示测量点的正向还是反向有功/无功功率、有功/无功电能量参与总加的标志,置“0”正向；置“1”反向
                                // D7表示参与总加运算的运算符标志 置“0” 加运算;置“1”减运算

    Summ_Add_MtTSA_VIR1_ID,  // 17*16字节,总加组1计量点存储的TSA地址
    Summ_Add_MtTSA_VIR2_ID,  // 17*16字节,总加组2计量点存储的TSA地址
    Summ_Add_MtTSA_VIR3_ID,  // 17*16字节,总加组3计量点存储的TSA地址
    Summ_Add_MtTSA_VIR4_ID,  // 17*16字节,总加组4计量点存储的TSA地址
    Summ_Add_MtTSA_VIR5_ID,  // 17*16字节,总加组5计量点存储的TSA地址

    Summ_Add_MtTSA_VIR6_ID,  // 17*16字节,总加组6计量点存储的TSA地址
    Summ_Add_MtTSA_VIR7_ID,  // 17*16字节,总加组7计量点存储的TSA地址
    Summ_Add_MtTSA_VIR8_ID,  // 17*16字节,总加组8计量点存储的TSA地址
    Summ_Add_MtTSA_VIR9_ID,  // 17*16字节,总加组9计量点存储的TSA地址
    Summ_Add_MtTSA_VIR10_ID, // 17*16字节,总加组10计量点存储的TSA地址

    Summ_Add_MtTSA_VIR11_ID, // 17*16字节,总加组11计量点存储的TSA地址
    Summ_Add_MtTSA_VIR12_ID, // 17*16字节,总加组12计量点存储的TSA地址
    Summ_Add_MtTSA_VIR13_ID, // 17*16字节,总加组13计量点存储的TSA地址
    Summ_Add_MtTSA_VIR14_ID, // 17*16字节,总加组14计量点存储的TSA地址
    Summ_Add_MtTSA_VIR15_ID, // 17*16字节,总加组15计量点存储的TSA地址

    Summ_Add_MtTSA_VIR16_ID, // 17*16字节,总加组16计量点存储的TSA地址
    STORE_RATE_ID,           // 1字节,存储周期,单位分钟
    VOLTAGE_ZERO_DRIFT_ID,   // 2字节,电压零漂门槛值，1位小数
    CURR_ZERO_DRIFT_ID,      // 2字节,电流零漂门槛值，2位小数
    VOLT_CHANGE_DRIFT_ID,    // 2字节,电压变化门槛值，1位小数

    CURR_CHANGE_DRIFT_ID,    // 2字节,电流变化门槛值，2位小数
    SOE_RELAY_TIME_ID,       // 2字节,SOE防抖时间，单位10毫秒
    Curr_Protect_ID,         // 2字节,电流保护定值，2位小数
    Protect_Relay_Time_ID,   // 2字节,保护时限定值，单位10毫秒
    GB104_RUN_PARA01,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限

    GB104_RUN_PARA02,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限
    GB104_RUN_PARA03,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限
    GB104_RUN_PARA04,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限
    GB104_RUN_PARA05,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限
    GB104_RUN_PARA06,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限

    GB104_RUN_PARA07,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限
    GB104_RUN_PARA08,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限
    GB104_RUN_PARA09,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限
    GB104_RUN_PARA0A,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限
    GB104_RUN_PARA0B,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限

    GB104_RUN_PARA0C,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限
    GB104_RUN_PARA0D,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限
    GB104_RUN_PARA0E,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限
    GB104_RUN_PARA0F,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限
    GB104_RUN_PARA10,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限

    GB104_RUN_PARA11,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限
    GB104_RUN_PARA12,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限
    GB104_RUN_PARA13,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限
    GB104_RUN_PARA14,        // 9字节 ,1字节表示参数状态，0x55表示激活，0xaa表示未激活，后面8字节，分别表示门限、平滑时间、下限、上限
    GB104_YC_BFG01,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活

    GB104_YC_BFG02,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活
    GB104_YC_BFG03,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活
    GB104_YC_BFG04,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活
    GB104_YC_BFG05,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活
    GB104_YC_BFG06,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活

    GB104_YC_BFG07,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活
    GB104_YC_BFG08,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活
    GB104_YC_BFG09,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活
    GB104_YC_BFG10,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活
    GB104_YC_BFG11,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活

    GB104_YC_BFG12,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活
    GB104_YC_BFG13,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活
    GB104_YC_BFG14,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活
    GB104_YC_BFG15,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活
    GB104_YC_BFG16,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活

    GB104_YC_BFG17,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活
    GB104_YC_BFG18,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活
    GB104_YC_BFG19,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活
    GB104_YC_BFG20,          // 1字节,表示参数状态，0x55表示激活，0xaa表示未激活
    DA_PARA_CURRENT_PORT,    // 2字节，当前定值区号

    DA_PARA_ASPECT,          // 3字节，参数特征标识，每个区号一个字节
    CURRENT_BLIND_SPOT,      // 30字节，电流死区,分三组，每组10个字节,第一个字节存储tag标识，下同
    AC_VOLTAGE_BLIND_SPOT,   // 30字节，交流电压死区,分三组，每组10个字节
    DC_VOLTAGE_BLIND_SPOT,   // 30字节，直流电压死区,分三组，每组10个字节
    POWER_BLIND_SPOT,        // 30字节，功率死区,分三组，每组10个字节

    FREQUENCY_BLIND_SPOT,    // 30字节，频率死区,分三组，每组10个字节
    COS_BLIND_SPOT,          // 30字节，功率因数死区 ,分三组，每组10个字节
    PT_FIRST_RATING,         // 30字节，PT一次额定 ,分三组，每组10个字节
    PT_SECONT_RATING,        // 30字节，PT二次额定 ,分三组，每组10个字节
    VOLTAGE_LOW_ALARM_VALUE, // 30字节，低电压报警门限值  ,分三组，每组10个字节

    VOLTAGE_LOW_ALARM_CYCLE, // 30字节，低电压报警周期  ,分三组，每组10个字节，单位秒
    VOLTAGE_OVER_ALARM_VALUE,// 30字节，过电压报警门限值,分三组，每组10个字节
    VOLTAGE_OVER_ALARM_CYCLE,// 30字节，过电压报警周期,分三组，每组10个字节，单位秒
    CURRENT_OVER_ALARM_VALUE,// 30字节，重载报警门限值,分三组，每组10个字节
    CURRENT_OVER_ALARM_CYCLE,// 30字节，重载报警周期,分三组，每组10个字节，单位秒

    CURRENT_HIGH_ALARM_VALUE,// 30字节，过载报警门限值,分三组，每组10个字节
    CURRENT_HIGE_ALARM_CYCLE,// 30字节，过载报警周期,分三组，每组10个字节，单位秒
    SWITCH_DEBOUNCE_TIME,    // 30字节，开入量采集防抖时间 ,分三组，每组10个字节，单位毫秒
    CTRL_ON_DEBOUNCE_TIME,   // 30字节，分闸输出脉冲保持时间,分三组，每组10个字节，单位毫秒
    CTRL_OFF_DEBOUNCE_TIME,  // 30字节，合闸输出脉冲保持时间,分三组，每组10个字节，单位毫秒

    BATTERY_ACTIVE_CYCLE,    // 30字节，蓄电池自动活化周期,分三组，每组10个字节，单位分钟
    BATTERY_ACTIVE_TIME,     // 30字节，蓄电池自动活化时刻,分三组，每组10个字节，秒分时日月年
    L01_CT_FIRST_RATING,     // 30字节，L01相CT一次额定,分三组，每组10个字节
    L01_CT_SECOND_RATING,    // 30字节，L01相CT二次额定,分三组，每组10个字节
    L01_ZEROCT_FIRST_RATING, // 30字节，L01零序CT一次额定,分三组，每组10个字节

    L01_ZEROCT_SECOND_RATING,// 30字节，L01零序CT二次额定,分三组，每组10个字节
    L02_CT_FIRST_RATING,     // 30字节，L02相CT一次额定,分三组，每组10个字节
    L02_CT_SECOND_RATING,    // 30字节，L02相CT二次额定,分三组，每组10个字节
    L02_ZEROCT_FIRST_RATING, // 30字节，L02零序CT一次额定,分三组，每组10个字节
    L02_ZEROCT_SECOND_RATING,// 30字节，L02零序CT二次额定,分三组，每组10个字节

    FAULT_LIGHT_EN,          // 6字节，故障指示灯自动复归投入,分三组，每组2个字节
    FAULT_LIGHT_TIME,        // 30字节，故障指示灯自动复归时间,分三组，每组10个字节，单位秒
    FAULT_SOE_HODE_TIME,     // 30字节，故障遥信保持时间 ,分三组，每组10个字节，单位秒
    FIRST_FTU_EN,            // 6字节，首端FTU投入 ,分三组，每组2个字节
    X_ALARM_TIME_VALUE,      // 30字节，X时间定值 ,分三组，每组10个字节,单位毫秒

    Y_ALARM_TIME_VALUE,      // 30字节，Y时间定值 ,分三组，每组10个字节,单位毫秒
    C_ALARM_TIME_VALUE,      // 30字节，C时间定值 ,分三组，每组10个字节,单位毫秒
    S_ALARM_TIME_VALUE,      // 30字节，S时间定值 ,分三组，每组10个字节,单位毫秒
    GROUND_OPEN_TIME_VALUE,  // 30字节，单相接地跳闸时间 ,分三组，每组10个字节,单位毫秒
    RECLOSE_SWITCH_TIME_VALUE,// 30字节，选线跳闸重合时间定值 ,分三组，每组10个字节,单位毫秒

    AUTO_SHORT_CIRCUIT_EN,   // 6字节，自适应相间短路故障处理投入,分三组，每组2个字节
    AUTO_GROUND_CHECK_EN,    // 6字节，自适应单相接地故障处理投入,分三组，每组2个字节
    FIRST_RECLOSE_EN,        // 6字节，一次重合闸投退,分三组，每组2个字节
    FIRST_RECLOSE_TIME_VALUE,// 30字节，一次重合时间,分三组，每组10个字节,单位毫秒
    HIGH_CURREN_CLOSED_EN,   // 6字节，大电流闭锁重投退,分三组，每组2个字节

    HIGH_CURREN_CLOSED_TIME, // 30字节，大电流闭重定值,分三组，每组10个字节
    L1_CURRENT_HIGH_POWER_EN,// 6字节，L01过流停电跳闸投退,分三组，每组2个字节
    L1_I_CURRENT_HIGH_ALARM_EN,// 6字节，L01过流I段告警投退,分三组，每组2个字节
    L1_I_CURRENT_HIGH_SWITCH_EN,// 6字节01过流I段出口投退,分三组，每组2个字节
    L1_I_CURRENT_HIGH_VALUE, // 30字节L01过流I段定值,分三组，每组10个字节,A

    L1_I_CURRENT_HIGH_TIME,  // 30字节L01过流I段时间,分三组，每组10个字节,单位毫秒
    L1_II_CURRENT_HIGH_ALARM_EN,// 6字节，L01过流II段告警投退,分三组，每组2个字节
    L1_II_CURRENT_HIGH_SWITCH_EN,// 3字节,L01过流II段出口投退,分三组，每组2个字节
    L1_II_CURRENT_HIGH_VALUE,// 30字节L01过流II段定值,分三组，每组10个字节
    L1_II_CURRENT_HIGH_TIME, // 30字节L01过流II段时间,分三组，每组10个字节,单位毫秒

    L1_ZERO_CURRENT_HIGH_ALARM_EN,// 6字节，L01零序过流告警投退,分三组，每组2个字节
    L1_ZERO_CURRENT_HIGH_SWITCH_EN,// 6字节,L01零序过流出口投退,分三组，每组2个字节
    L1_ZERO_CURRENT_HIGH_VALUE,// 30字节L01零序过流定值,分三组，每组10个字节,A
    L1_ZERO_CURRENT_HIGH_TIME,// 30字节L01零序过流时间,分三组，每组10个字节,单位毫秒
    L1_LOW_CURRENT_HIGH_ALARM_EN,// 6字节，L01小电流接地告警投退,分三组，每组2个字节

    L1_LOW_CURRENT_HIGH_SWITCH_EN,// 6字节,L01小电流接地出口投退,分三组，每组2个字节
    L2_CURRENT_HIGH_POWER_EN,// 6字节，L02过流停电跳闸投退 ,分三组，每组2个字节
    L2_I_CURRENT_HIGH_ALARM_EN,// 6字节，L02过流I段告警投退,分三组，每组2个字节
    L2_I_CURRENT_HIGH_SWITCH_EN,// 6字节,L02过流I段出口投退,分三组，每组2个字节
    L2_I_CURRENT_HIGH_VALUE, // 30字节L02过流I段定值,分三组，每组10个字节

    L2_I_CURRENT_HIGH_TIME,  // 30字节L02过流I段时间,分三组，每组10个字节,单位毫秒
    L2_II_CURRENT_HIGH_ALARM_EN,// 6字节，L02过流II段告警投退,分三组，每组2个字节
    L2_II_CURRENT_HIGH_SWITCH_EN,// 6字节,L02过流II段出口投退,分三组，每组2个字节
    L2_II_CURRENT_HIGH_VALUE,// 30字节L02过流II段定值,分三组，每组10个字节
    L2_II_CURRENT_HIGH_TIME, // 30字节L02过流II段时间,分三组，每组10个字节,单位毫秒

    L2_ZERO_CURRENT_HIGH_ALARM_EN,// 6字节，L02零序过流告警投退,分三组，每组2个字节
    L2_ZERO_CURRENT_HIGH_SWITCH_EN,// 6字节,L02零序过流出口投退,分三组，每组2个字节
    L2_ZERO_CURRENT_HIGH_VALUE,// 30字节L02零序过流定值,分三组，每组10个字节
    L2_ZERO_CURRENT_HIGH_TIME,// 30字节L02零序过流时间,分三组，每组10个字节,单位毫秒
    L2_LOW_CURRENT_HIGH_ALARM_EN,// 6字节，L02小电流接地告警投退 ,分三组，每组2个字节

    L2_LOW_CURRENT_HIGH_SWITCH_EN,// 6字节,L02小电流接地出口投退 ,分三组，每组2个字节
    TMN_FUN_PAPA_RESERVE_ID, // 1024字节,备用
};

//-平台定义的计量点参数项-
enum mtr_para
{
    Mt_Para_State_ID=0,      // 1字节,计量点状态：1有效、0无效
    Meter_Address,           // 17字节,16进制可包含'F',电表通信地址,第一个字节表示地址长度 05 12 34 56 78 09 12
    Mt_Para_Speed_ID,        // 1字节,计量点波特率,X*300
    Mt_Para_Prtcl_ID,        // 1字节,计量点通信规约类型
    Meter_Port_ID,           // 4字节,通信端口号OAD

    Mt_Para_Pass_LENGTH,     // 1字节,通信密码长度
    Mt_Para_Pass_ID,         // 6字节,通信密码,终端与电能表通信的密码
    Mt_Para_Tariffer_ID,     // 1字节,费率数,通信接入的测量点的电能费率个数，数值范围1~14
    Mt_Energy_IntValue_ID,   // 1字节,有功电能示值整数位个数
    Mt_Energy_Decimal_ID,    // 1字节,有功电能示值小数位个数

    Meter_UserType_ID,       // 1字节,用户类型
    Mtr_Cnnt_Style_ID,       // 1字节,计量点接线方式,1单相、3三相三线、4三相四线(新规约0:未知,1:单相,2:三相三线,3:三相四线)
    Acq_Address_ID,          // 17字节,采集器地址(698)
    Asset_No_Length,         // 1字节, 资产号长度
    Asset_No_ID,             // 16字节, 资产号

    MT_CT_PT_ID,             // 4字节, 电表ctpt,高2字节为PT,低2字节CT
    Annex_Num_ID,            // 1字节, 电表附加信息域OAD个数
    Mt_Para_Charct_ID,       // 1字节,计量点性质:01：485表、02：模拟量、03脉冲量、04计算值、05：交采、06：载波、07、载波多功能
    Mt_Turn_Num_ID,          // 2字节,电能表/交流采样装置序号,1~64
    Mt_Belong_Point_ID,      // 1字节,现在用来存储下发的485表规约类型，用来解决多对一时，一对多如何还原的问题

    Mt_Para_Com_ID,          // 1字节,计量点通信端口号,376.1规约宣贯定义内置交采为端口1,RS485COM1为端口2,RS485COM2为端口3,RS485COM3为端口4,...,级联通信为端口30,载波为端口31
    Mt_Para_Addr_ID,         // 6字节,BCD,计量点通信地址,数值范围0~999999999999
    MT_SWITCH_CTRL_ID,       // 2字节,端口0,特殊测量点，开关操作允许标志，0x55ac表示可以操作
    MT_SWITCH_CTRL_ID1,      // 2字节,端口1,特殊测量点，开关操作允许标志，0x55ac表示可以操作
    MT_SWITCHPUB_CTRL_ID,    // 2字节,端口0,特殊测量点，共补遥信读取标志，0x55ac表示可以操作

    MT_SWITCHPUB_CTRL_ID1,   // 2字节,端口1,特殊测量点，共补遥信读取标志，0x55ac表示可以操作
    AMR_COLLECTOR_COMM_ADDR_ID,// 6字节,所属采集器通信地址
    AMR_USER_METER_CLASS_ID, // 1字节,用户及电表分类号
                                // D4~7:用户分类号(1-6)对应如下:
                                // 1：大型专变用户
                                // 2：中小型专变用户
                                // 3：低压三相一般工商业用户
                                // 4：低压单相一般工商业用户
                                // 5：居民用户
                                // 6：公变考核计量点
                                // D0~3:电表分类号(0-15)
                                // 0：对应所属用户分类所要求的全部数据项
                                // 1~15：表示15类数据项配置选择（在所属用户分类所要求的数据项范围内）
    AMR_IMPORTANCE_FLAG_ID,  // 重点表标志（0：普通/1：重点）
    Rating_Volt_ID,          // 2字节,额定电压,扩大10倍

    Rating_Curr_ID,          // 2字节,额定电流,扩大100倍
    Cur_Lmt_Para_ID,         // 3*4字节,电流越上限参数,扩大1000倍,分别表示ABC三相,带符号的bin存储
    Cur_Lmt_Para_KeepTime,   // 1字节,电流越限(上限)参数持续时间
    Cur_Lmt_Para_RetRate,    // 2字节,电流越限(上限)参数返回系数,%扩大10倍,最高位代表符号位bin格式
    Gate_Cur_Para_KeepTime,  // 1字节,电流门限(上上限)持续时间

    Gate_Cur_Para_RetRate,   // 2字节,电流门限(上上限)返回系数,%扩大10倍,最高位代表符号位bin格式
    Zero_Cur_Lmt_KeepTime,   // 1字节,零序电流上限持续时间
    Zero_Cur_Lmt_RetRate,    // 2字节,零序电流上限返回系数,%扩大10倍,最高位代表符号位bin格式
    Vol_Abnromal1_KeepTime_ID,// 1字节，电压异常上限(上上限)的持续时间
    Vol_Abnromal1_RetRate_ID,// 2字节，电压异常上限(上上限)的返回系数%扩大10倍,最高位代表符号位bin格式

    Vol_Abnromal2_KeepTime_ID,// 1字节，电压异常下限(下下限)的持续时间
    Vol_Abnromal2_RetRate_ID,// 2字节，电压异常下限(下下限)的返回系数%扩大10倍,最高位代表符号位bin格式
    Pow_Gate_Para_KeepTime,  // 1字节,视在功率上上限持续时间
    Pow_Gate_Para_RetRate,   // 2字节,视在功率上上限返回系数,%扩大10倍,最高位代表符号位bin格式
    Pow_Lmt_Para_KeepTime,   // 1字节,视在功率上限持续时间

    Pow_Lmt_Para_RetRate,    // 2字节,视在功率上限返回系数,%扩大10倍,最高位代表符号位bin格式
    Vol_Lost_balance_KeepTime,// 1字节,电压不平衡限值持续时间
    Vol_Lost_balance_RetRate,// 2字节,电压不平衡限值返回系数,%扩大10倍,最高位代表符号位bin格式
    Cur_Lost_balance_KeepTime,// 1字节,电流不平衡限值持续时间
    Cur_Lost_balance_RetRate,// 2字节,电流不平衡限值返回系数,%扩大10倍,最高位代表符号位bin格式

    Gate_Cur_Para,           // 3*4字节,电流门限参数(上上限),扩大1000倍,分别表示ABC三相,带符号的bin存储
    PHASE_CUR_OVER_LMT,      // 2字节,过流门限,扩大100倍
    PHASE_CUR_UP_LMT,        // 2字节,相电流上限（额定电流门限）,扩大100倍
    Zero_Cur_Lmt_ID,         // 4字节,零序电流上限,扩大1000倍
    Vol_Lmt_Above_ID,        // 2字节,合格电压上限,扩大10倍

    Vol_Lmt_Below_ID,        // 2字节,合格电压下限,扩大10倍
    Vol_Lost_Para_ID,        // 2字节,电压断相门限,扩大10倍
    Vol_Abnromal1_Para_ID,   // 2字节,电压异常上限(上上限)值,扩大10倍
    Vol_Abnromal2_Para_ID,   // 2字节,电压异常下限(下下限)值,扩大10倍
    Pow_Gate_Para_ID,        // 4字节,视在功率上上限,扩大10000倍

    Pow_Lmt_Para_ID,         // 4字节,视在功率上限,扩大10000倍
    Vol_Lost_balance_ID,     // 2字节,电压不平衡限值,%扩大10倍,最高位代表符号位bin格式
    Vol_Lost_Time_ID,        // 2字节,连续失压时间限值,连续失压时间限值，单位：min
    Cur_Lost_balance_ID,     // 2字节,电流不平衡阀值,%扩大10倍,最高位代表符号位bin格式
    Ovld_Para,               // 2字节,电流过负荷的相对额定值比例,扩大100倍

    Cap_Ctrl_Method_ID,      // 1字节,电容器投切控制方式,控制方式：1：当地控制；2：远方遥控；3：闭锁：4：解锁；其它：备用。
    Cap_CtrlType_ID,         // 1字节,电容器控制类型，将Cap_CtrlType_Para_ID拆解成而得
    Cap_LockType_ID,         // 1字节,电容器锁控制类型，将Cap_CtrlType_Para_ID拆解成而得
    Cap_CtrlType_Para_ID,    // 1字节,电容器投切控制方式
    Capa_Capacity_ID,        // 8*16字节,电容装见容量,04数据格式02最小功率为0.0001KW

    Compen_Tag_ID,           // 1*16字节,测量点16个电容器组的共补/分补标志
    Compen_Phase_ID,         // 1*16字节,测量点16个分补相位
    Dest_PF_ID,              // 2字节,目标功率因数,04数据格式02
    Put_RP_Lmt_ID,           // 3字节,投入无功功率门限,04数据格式23
    Cut_RP_Lmt_ID,           // 3字节,切除无功功率门限,同上

    Cap_Delay_Time_ID,       // 1字节,延时时间,单位:秒
    Cap_Act_Int_ID,          // 1字节,动作时间间隔,单位:分
    Cap_Volt_Over_ID,        // 2字节,过电压,04数据格式07
    Cap_Volt_Over_Diff_ID,   // 2字节,过电压回差值,同上
    Cap_Volt_Lack_ID,        // 2字节,欠电压,同上

    Cap_Volt_Lack_Diff_ID,   // 2字节,欠电压回差值,同上
    Comm_Byte,               // 通信数据位  1byte 5位、6位、7位、8位
    Summ_Add_Sequence_ID,    // 1字节,总加组序号
    Vir_Power_Lmt_ID,        // 4字节,总加组功率定值0.01KW
    Vir_En_Lmt_ID,           // 8字节,总加组电量定值0.0001kwh

    Met_Office_Num_ID,       // 电表局编号12byte
    Cur_Low_Para_ID,         // 4字节,电流轻载限值,扩大1000倍
    METER_PAPA_RESERVE_ID,   // 512字节,预留备用
};

//-平台定义的终端扩展参数-
enum tmn_ext_para
{
    TMN_EXT_485_II_TYPE_ID=10000,// 1字节，485II口的功能：0——被抄485口，1——抄表485口，默认被抄口
    TMN_AC_ADDR_OFFSET,      // BIN，终端交采地址偏移
    EventReadCycle_ID,       // BIN，512字节, 事件等级1~256的采集周期参数F107，每个等级占2字节：周期数值BIN 1  周期单位BIN 1
    TMN_EXT_485_I_TYPE_ID,   // 1字节，485I口的功能：0——被抄485口，1——抄表485口，默认被抄口
    MTR_NUM_OF_485_I_ID,     // 2字节,485-1口上485电表的数量

    MTR_NUM_OF_485_II_ID,    // 2字节,485-2口上485电表的数量
    METER_485_LOG_EN,        // 1字节,485抄表日志使能,方便现场调试,0x55打开,其他关闭
    AMR07_DAY_DATA_SAVE_TYPE,// 1字节，07规约电表日数据存储类型(初始化保留,0xAA:不判断数据时标存储,且日冻结数据采集失败时，通过实时转日冻结,其他无效)
    DATA_POINT_COPY_FROM_REALDATA,// 1字节，07规约测量点的日数据是否采用实时数据转存, 0x55:是，其它: 否
    DOUBLE_CHANNEL_RADIO_TYPE_ID,// BIN，1字节双通道第一信道中电台类型 2:wdasn 1:普通电台 0:默认高速电台(软猫)

    PRTLSTACK_EXCHANGE,      // 1字节,内外置协议栈切换 0:外置协议栈 ； 1:内置协议栈
    TMN_EXT_Cch_Ip_Addr_ID,  // 6字节，主用主站ip地址，前4个字节每个字节表示ip地址的一段，后两个字节表示端口号
    TMN_EXT_Cch_Ip_Addr_ID2, // 6字节，格式同上，备用一主站ip地址
    TMN_EXT_Cch_Ip_Addr_ID3, // 6字节，格式同上，备用二主站ip地址
    TMN_EXT_APN_Addr_ID,     // 16字节，APN地址，ASCII码，用于GPRS登陆。默认为'CMNET'

    TMN_EXT_SPECIALNET_NAME_ID,// 32字节ASCII码，虚拟专网用户名，用于GPRS登陆，默认为'CMNET'
    TMN_EXT_SPECIALNET_PWD_ID,// 32字节ASCII码，虚拟专网密码
    DOUBLE_CHANNEL_EXT_EN,   // BIN，1字节 扩展信道有效标识   0x55-有效 0xAA-无效
    SET_START_DELAY,         // 1字节,通信模块开机时序 0:表示开启3S ； 1:表示关闭1S
    TMN_PAPA_EXTEND_RESERVE_1,// 1字节,上报方案变更标识，1表示变更，0表示没有变更

    RS232_COMDCB,            // 5字节232端口参数,包括端口波特率、校验位、数据位、停止位
                                // 第1字节表示端口波特率
                                // enum:0-255, 300/600/1200/2400/4800/7200/9600/19200/38400/57600/115200/自适应(255)；
                                // 第2字节表示校验方式，enum: 0无校验、1奇校验、2偶校验；
                                // 第3字节表示端口数据位；enum: 5（5），6（6），7（7），8（8）
                                // 第4字节表示停止位，1（1），2（2）
                                // 第5字节表示流控，无(0)，硬件(1)，软件(2)
    RS232_PORT_FUNC,         // 1字节232端口功能: enum:上行通信(0),抄表(1),级联(2),停用(3),默认上行通信
    RS485_1_COMDCB,          // 5字节485-1端口参数,包括端口波特率、校验位、数据位、停止位
                                // 第1字节表示端口波特率
                                // enum:0-255, 300/600/1200/2400/4800/7200/9600/19200/38400/57600/115200/自适应(255)；
                                // 第2字节表示校验方式，enum: 0无校验、1奇校验、2偶校验；
                                // 第3字节表示端口数据位；enum: 5（5），6（6），7（7），8（8）
                                // 第4字节表示停止位，1（1），2（2）
                                // 第5字节表示流控，无(0)，硬件(1)，软件(2)
    RS485_1_PORT_FUNC,       // 1字节485-1端口功能: enum:上行通信(0),抄表(1),级联(2),停用(3),默认上行通信
    RS485_2_COMDCB,          // 5字节485-2端口参数,包括端口波特率、校验位、数据位、停止位
                                // 第1字节表示端口波特率
                                // enum:0-255, 300/600/1200/2400/4800/7200/9600/19200/38400/57600/115200/自适应(255)；
                                // 第2字节表示校验方式，enum: 0无校验、1奇校验、2偶校验；
                                // 第3字节表示端口数据位；enum: 5（5），6（6），7（7），8（8）
                                // 第4字节表示停止位，1（1），2（2）
                                // 第5字节表示流控，无(0)，硬件(1)，软件(2)

    RS485_2_PORT_FUNC,       // 1字节485-2端口功能: enum:上行通信(0),抄表(1),级联(2),停用(3),默认上行通信
    RS485_3_COMDCB,          // 5字节485-3端口参数,包括端口波特率、校验位、数据位、停止位
                                // 第1字节表示端口波特率
                                // enum:0-255, 300/600/1200/2400/4800/7200/9600/19200/38400/57600/115200/自适应(255)；
                                // 第2字节表示校验方式，enum: 0无校验、1奇校验、2偶校验；
                                // 第3字节表示端口数据位；enum: 5（5），6（6），7（7），8（8）
                                // 第4字节表示停止位，1（1），2（2）
                                // 第5字节表示流控，无(0)，硬件(1)，软件(2)
    RS485_3_PORT_FUNC,       // 1字节485-3端口功能: enum:上行通信(0),抄表(1),级联(2),停用(3),默认上行通信
    INFRARED_COMDCB,         // 5字节红外端口参数,包括端口波特率、校验位、数据位、停止位
                                // 第1字节表示端口波特率
                                // enum:0-255, 300/600/1200/2400/4800/7200/9600/19200/38400/57600/115200/自适应(255)；
                                // 第2字节表示校验方式，enum: 0无校验、1奇校验、2偶校验；
                                // 第3字节表示端口数据位；enum: 5（5），6（6），7（7），8（8）
                                // 第4字节表示停止位，1（1），2（2）
                                // 第5字节表示流控，无(0)，硬件(1)，软件(2)
    PLC_OR_WIRELESS_COMDCB,  // 5字节载波、微功率无线端口参数,包括端口波特率、校验位、数据位、停止位
                                // 第1字节表示端口波特率
                                // enum:0-255, 300/600/1200/2400/4800/7200/9600/19200/38400/57600/115200/自适应(255)；
                                // 第2字节表示校验方式，enum: 0无校验、1奇校验、2偶校验；
                                // 第3字节表示端口数据位；enum: 5（5），6（6），7（7），8（8）
                                // 第4字节表示停止位，1（1），2（2）
                                // 第5字节表示流控，无(0)，硬件(1)，软件(2)

    BLUETOOTH_COMDCB,        // 5字节蓝牙端口参数,包括端口波特率、校验位、数据位、停止位
                                // 第1字节表示端口波特率
                                // enum:0-255, 300/600/1200/2400/4800/7200/9600/19200/38400/57600/115200/自适应(255)；
                                // 第2字节表示校验方式，enum: 0无校验、1奇校验、2偶校验；
                                // 第3字节表示端口数据位；enum: 5（5），6（6），7（7），8（8）
                                // 第4字节表示停止位，1（1），2（2）
                                // 第5字节表示流控，无(0)，硬件(1)，软件(2)
    TMN_EXT_485_III_TYPE_ID, // 1字节，485III口的功能：0——被抄485口，1——抄表485口，默认被抄口
    MTR_NUM_OF_485_III_ID,   // 2字节,485-3口上485电表的数量
    Eth_Cch_Ip_Addr_ID,      // 6字节，以太网主用主站ip地址，前4个字节每个字节表示ip地址的一段，后两个字节表示端口号
    Eth_Cch_Ip_Addr_Back_ID1,// 6字节，以太网格式同上，备用一主站ip地址

    Eth_Cch_Ip_Addr_Back_ID2,// 6字节，以太网格式同上，备用二主站ip地址
    Eth_Work_Mode_ID,        // 1字节，以太网终端工作模式:0-混合模式;1-客户机模式;2-服务器模式
    Eth_NET_PROTOCOL_ID,     // 1字节，TCP/UDP选择，为1表示TCP，为0表示UDP
    Eth_Connect_Type_ID,     // 1字节，以太网主备模式（0），多连接模式（1）
    Eth_DT_Port_ID,          // 2字节，以太网侦听端口号

    Eth_Proxy_Addr_ID,       // 6字节，以太网代理服务器地址,前4个字节每个字节表示ip地址的一段，后两个字节表示端口号
    Eth_Timeout_Retry_ID,    // 2字节，以太网终端等待从动站响应的超时时间和重发次数
                                // #1:确认超时由字节1的D0-D7和字节2的D0-D3组成0-4095秒范围的超时时间。
                                // #2:重发次数由字节2的D4-D5组成，最多3次,为0表示不允许重发。字节2的D6-D7备用
    Eth_Heart_Beat_Time_ID,  // 2字节，以太网心跳周期，单位为秒
    Eth_Ip_Get_Mode_ID,      // 1字节，以太网IP地址获取模式 DHCP（0），静态（1），PPPoE（2）
    Eth_PPPoE_Name_ID,       // 32字节，以太网PPPoE用户名

    Eth_PPPoE_Pwd_ID,        // 32字节，以太网PPPoE密码
    MANUAL_SET_NET_WORKMODE_ID,// 1字节，手动设置模块网络制式
    Usb_Virtual_Comm_EN,     // 1字节, USB虚拟串口通信使能,0x55打开,其他关闭
    MAP_PubNet_ID1,          // 2字节，公网远程通信多接入点备用通道
                                // #1:1字节  运营商
                                // #2:1字节  网络类型
                                // #3:1字节  APN的长度,最大32字节
                                // #4:32字节 APN
                                // #5:1字节  用户名长度,最大20字节
                                // #6:20字节 用户名
                                // #7:1字节  密码长度,最大20字节
                                // #8:20字节 密码
                                // #9:4字节 代理服务器地址
                                // #10:2字节 代理端口
                                // #11:1字节 主站通信参数数组数目最大支持2个
                                // #12:4字节 主站IP地址
                                // #13:2字节 主站端口
                                // #14:4字节 主站IP地址
                                // #15:2字节 主站端口
    MAP_PubNet_ID2,          // 2字节，公网远程通信多接入点备用通道
                                // #1:1字节  运营商
                                // #2:1字节  网络类型
                                // #3:1字节  APN的长度,最大32字节
                                // #4:32字节 APN
                                // #5:1字节  用户名长度,最大20字节
                                // #6:20字节 用户名
                                // #7:1字节  密码长度,最大20字节
                                // #8:20字节 密码
                                // #9:4字节 代理服务器地址
                                // #10:2字节 代理端口
                                // #11:1字节 主站通信参数数组数目最大支持2个
                                // #12:4字节 主站IP地址
                                // #13:2字节 主站端口
                                // #14:4字节 主站IP地址
                                // #15:2字节 主站端口

    MAP_PubNet_ID3,          // 2字节，公网远程通信多接入点备用通道
                                // #1:1字节  运营商
                                // #2:1字节  网络类型
                                // #3:1字节  APN的长度,最大32字节
                                // #4:32字节 APN
                                // #5:1字节  用户名长度,最大20字节
                                // #6:20字节 用户名
                                // #7:1字节  密码长度,最大20字节
                                // #8:20字节 密码
                                // #9:4字节 代理服务器地址
                                // #10:2字节 代理端口
                                // #11:1字节 主站通信参数数组数目最大支持2个
                                // #12:4字节 主站IP地址
                                // #13:2字节 主站端口
                                // #14:4字节 主站IP地址
                                // #15:2字节 主站端口
    FTP_TELNET_EN,           // 1字节, FTP/TELNET使能,1打开,0关闭
    Mac_Select_ID,           // 1字节，Mac校验，1代表开启，0代表关闭
};

//-平台定义的测量点扩展参数-
enum mtr_ext_para
{
    MTR_EXTEND_PARA_TEST0=11000,// 1字节，测量点扩展参数开始
    Freeze_Density_2014_ID,  // 248*2字节，表示2类数据中冻结类型为“曲线”的F1~F248曲线冻结密度参数，数值范围为0～255。
                                // 当冻结密度为127时，表示该曲线冻结密度取默认值（交采测量点电压曲线1min，其他15min）。
                                // 当冻结密度为其他值时，按Q/GDW 1376.1-2013 的表C.1 数据冻结密度定义执行。
    EventGradeConf_Mtr_ID,   // BIN，1字节，电能表数据分级归类参数F105
};

//-平台定义的各个地区终端扩展参数-
enum tmn_ext_para_area
{
    TMN_AREA_EXTEND_PARA_TEST0=12000,// 1字节，各地区终端扩展参数开始
};

//-平台定义的各个地区测量点扩展参数-
enum mtr_ext_para_area
{
    MTR_AREA_EXTEND_PARA_TEST0=13000,// 1字节，各地区测量点扩展参数开始
};

extern const uint16 TmnRunParaLen[TMN_RUN_PARA_NUM];
extern const uint16 TmnFuncParaLen[TMN_FUNC_PARA_NUM];
extern const uint16 MtrParaLen[JLD_PARA_NUM];
extern const uint16 TmnExtParaLen[TMN_EXT_PARA_NUM];           //平台定义的终端扩展参数
extern const uint16 MtrExtParaLen[MTR_EXT_PARA_NUM];           //平台定义的测量点扩展参数
extern const uint16 TmnExtParaAreaLen[TMN_EXT_PARA_AREA_NUM];  //平台定义的各个地区终端扩展参数
extern const uint16 MtrExtParaAreaLen[MTR_EXT_PARA_AREA_NUM];  //平台定义的各个地区测量点扩展参数
extern const BOOL TmnExtParaNoClear[TMN_EXT_PARA_NUM];
extern const BOOL MtrExtParaNoClear[MTR_EXT_PARA_NUM];
extern const BOOL TmnExtParaAreaNoClear[TMN_EXT_PARA_AREA_NUM];
extern const BOOL MtrExtParaAreaNoClear[MTR_EXT_PARA_AREA_NUM];
#endif /* #ifndef PARA_ID_H */

