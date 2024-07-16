/*******************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限责任公司
             All rights reserved.
    程序名称: sys_cfg.h
    功    能：系统配置文件定义
    开 发 人：尤斌
    开发时间：2007-10-24
    版本历史: 2007-10-24 ,v1.0 : New
*******************************************************************************/
#ifndef _SYS_CFG_H
#define _SYS_CFG_H

//begin of GB101\101 define 

/*国网融合终端增加GB101_104协议  ，为方便代码移植，
   所有新增或修改代码均可收索GB101_104_EN查询，朴军海 2019年6月26日*/
   
#define GB101_104_EN            0  /*上行规约同时支持DLT/T634.5 104、 DL/T634.5 101规约功能使能*/

#define PROTOCAL_101               1 //-101规约类型-
#define PROTOCAL_104               2 //-104规约类型-
#define PROTOCAL_376               3 //-GB376.1规约类型-
#define PROTOCAL_698_45            4 //-GB698.45.1规约类型-

#define NOMMAL_PROT_TYPE        PROTOCAL_698_45 //-未设置参数时，默认的协议类型-

#define SOE_MAX_NUM             40  //soe支持最大数量
//----------------------功能选择开关，以下4个功能开关需要根据用的要求，是否放开--------------
#define YC_CHANGE_SEND_EN           1        //-遥测数据变化上送功能-
#define EN_CYC_SEND_EN              0        //-突发送电量上送功能-
#define YC_CYC_SEND_EN              0        //-循环上送数据功能-
#define BACK_SCAN_EN                0        //-背景扫描功能-
//-------------------end of 功能选择开关------------------------------------------------------
//end of GB101\101 define 

#define CHONG_QING_FUN              0           //重庆功能
#define JIANG_SU_FUN                0           //江苏功能
#define FUJIAN_FUN                  0           //福建功能
#define HEBEI_FUN                   0           //河北功能
#define SHANDONG_FUN                0           //山东功能
#define HENAN_FUN                   0           //河南功能
#define HUABEI_FUN                  0           //华北功能
#define HUBEI_FUN                   0           //湖北功能
#define BEIJING_FUN                 0           //北京功能
#define SHANGHAI_FUN                0           //上海功能
#define GANSU_FUN                   0           //甘肃功能
#define SICHUAN_FUN                 0           //四川功能
#define LIAONING_FUN                0           //辽宁功能
#define NINGXIA_FUN                 0           //宁夏功能
#define JIANGXI_FUN                 0           //江西功能
#define XINJIANG_FUN                0           //新疆功能
#define ZHEJIANG_FUN                0           //浙江功能
#define HEILONGJIANG_FUN            0           //黑龙江功能
#define HUNAN_FUN                   0           //湖南功能
#define SHANXI_FUN                  0           //山西功能
#define SHANXI1_FUN                 0           //陕西功能
#define JILIN_FUN                   0           //吉林功能
#define MENGDONG_FUN                0           //蒙东功能
#define TIANJIN_FUN                 0           //天津功能
#define ANHUI_FUN                   0           //安徽功能

#define STD_698PRO_BETA	            1           //国网698规约
#define STD_698PRO_PATCH			1			//国网698规约勘误
#define DELGW2013                   0           //屏蔽无用代码
#define SECURITY_DEFEND				0			//国网安全防护功能


#define HARDWARE_TYPE               4       //1:一型大终端;2:二型江苏终端;3:三型208终端;4:集中器终端;9:II型集中器

#define MODULARIZATION_TMN_EN       1           // 模块化终端功能

#if MODULARIZATION_TMN_EN == 1
#define MDL_RECOGNITION_EN          1           // 模组识别功能
#endif

#if HARDWARE_TYPE == 1
#define HARDWARE_IV_EN              1       //江苏降成本I型终端
#else
#define HARDWARE_IV_EN              0       //江苏降成本I型终端
#endif
#define TRAFFIC8_EN                 0           //8费率使能


#define GPRS_USB_VIRTUAL_NET_EN     0           //采用USB虚拟网口的GPRS通信方式
#define LINK_BREAKPOINT_EN          1           //应用分帧使能

#define MET07_RT_DUMP_CURVE_MIN_EN  1           //07电表实时转存曲线使能
#define PROTOCOL_CHANGE             0           //376.1协议和698协议切换
#define CH_AC485_CACHE_EN           0
#define DELIVER_DETECT_EN  			1  			//送检功能使能，对某些控制去掉了现场应用的控制
												//针对北京电科院的台体做了相关处理
#define DELIVER_LOG_EN				0			//送检时候，日志功能

#define APN_ADAPT_CHANGE_EN         0           //APN自适应功能
#define JIAOCAIYOUHUA 				1           //新增交采优化项目宏定义，便于代码合入

#define CBTASK_NEW					1
#define CBTASK_OOP					1
#define ROUTER_UPDATE_FILE			"/mnt/router_update_file"
#define ROUTER_UPDATE_FILE_BACK	    "/mnt/router_update_file_back"

#if MODULARIZATION_TMN_EN == 1
#define MDL_UPDATE_FILE             "/data/app/mapManager/mdl_update_file"
#define MDL_UPDATE_FILE_BACK        "/data/app/mapManager/mdl_update_file_back"

#define JC_UPDATE_FILE              "/data/app/acMeter/jc_update_file"
#define JC_UPDATE_FILE_BACK         "/data/app/acMeter/jc_update_file_back"

#define JC_UPDATE_FILE_IC           "/data/app/acMeter/jc_update_file_ic"
#define JC_UPDATE_FILE_TOPO         "/data/app/acMeter/jc_update_file_topu"

#define MDL_CTL_FILE_NAME           "XLDZ_MODULE_YK"
#define MDL_YXYM_FILE_NAME          "XLDZ_MODULE_YXYM"
#define MDL_RS485_FILE_NAME         "XLDZ_MODULE_RS485"
#define MDL_PLC_FILE_NAME            "XLDZ_MODULE_PLC"
#define JC_FILE_NAME                "XLDZ_MODULE_JC"
#define MDL_CTL_UPDATE_FILE         "/mnt/ctl_update_file"
#define MDL_CTL_UPDATE_FILE_BACK    "/mnt/ctl_update_file_back"
#define MDL_YXYM_UPDATE_FILE        "/mnt/yxym_update_file"
#define MDL_YXYM_UPDATE_FILE_BACK   "/mnt/yxym_update_file_back"
#define MDL_RS485_UPDATE_FILE       "/mnt/rs485_update_file"
#define MDL_RS485_UPDATE_FILE_BACK  "/mnt/rs485_update_file_back"

#define MDL_PLC_UPDATE_FILE           "/mnt/plc_update_file"
#define MDL_PLC_UPDATE_FILE_BACK    "/mnt/plc_update_file_back"
#define MDL_TMP_UPDATE_FIEL            "/mnt/tmp_update_file"
#define MDL_TMP_UPDATE_FILE_BACK    "/mnt/tmp_update_file_back"

#define MDL_JC_UPDATE_FILE          "/mnt/jc_update_file"
#define MDL_JC_UPDATE_FILE_BACK     "/mnt/jc_update_file_back"

#endif

#define GET_DATA1_RELAY_EN            1            //召测一类数据F129以后的数据，是否采用穿透的方式（目前只做F129，否则表台测试可能有问题）
#define REMOTE_VPN_EN               1           //终端远程VPN功能
#define REMOTE_VPN_FILE             "^RemoteVpnInfo"        //交互文件
#define REMOTE_VPN_SCRIPT           "/etc/ppp/peers/vpn"    //VPN拨号脚本的文件

/*******************************************************************************
* 常用字符与ASCII代码对照表
* 0(0x30)~9(0x39)    !(0x21)    #(0x23)        %(0x25)    .(0x2E)        :(0x3A)    ?(0x3F)
* A(0x41)~G(0x47)    H(0x48)~N(0x4E)        O(0x4F)~T(0x54)        U(0x55)~Z(0x5A)
* a(0x61)~g(0x67)    h(0x68)~n(0x6E)        o(0x6F)~t(0x74)        u(0x75)~z(0x7A)
*******************************************************************************/
/*******************************************************************************
* 以下定义需要根据地区和硬件选择相关的宏定义
*******************************************************************************/
#define MINICURVE                   1           //打开mini  curve 功能

#define EXTEND_PARA                 1           //独立的扩展参数功能
#define EXTEND_DATA                 1           //独立的扩展数据功能
#define EXTEND_RAMDATA              1           //独立的扩展RamDisk数据功能
#define HARDWARE_2012_EN            1           //2012年新规范终端功能
#define EVENT_2012_EN               1           //2012年新规范事件

#define EVENT_2014_EN               1           //2014年新规范事件
#define EVENT_698_EN                0           //698表事件上报功能
#define EVENT_2014_ENABLE           1           //事件分级功能是否起效（如果该宏关闭，则事件分级的启动代码无效，事件分级的处理代码依然有效；由于该功能目前还没有正式规范，所以默认不起用）
#define EVENT_2014_EVENTPARA_EN     0           //事件分级判断时候，抄读到电表状态字后，是否需要根据F105、F106的参数情况来决定是否抄表内时间（对于国网送检台体，必须打开；其他地区不建议打开）

#define SAL_PRTL                    0           //SSAL安全协议

#define BROADBAND_698_EXT_FUNC      0           //宽带扩展功能及698表事件上报

//协议栈类型，0-通讯模块的协议栈；1-终端协议栈
#define PRTLSTACK_TYPE_ID           (HARDWARE_TYPE == 1 || HARDWARE_TYPE == 2)

#define COMMPARA_BAK_EN             0           //通讯参数备份功能（参数能够通过F3恢复出厂通讯参数）

#define TMNHALT_METHOD              1           //终端停上电新依据

#define GPRS_ETH_ALL_OLINE          1           //GPRS和以太网同时在线

#define	LONG_NEW_APN                0           //增加新字典用于存储长度大于16的APN，安徽、甘肃、山西、浙江、江苏打开时需要额外修改

//start Common Problem
#if HARDWARE_TYPE == 4
#define PLC_COMM_TYPE		        0           //载波：路由板还是通道板开关选择,1为弥亚威64M通道板，0为路由板
#endif
//end Common Problem
#define PULSECONSTS_6400			1			//脉冲常数3200/6400可配	(特别要求时才开)

#define AUTO_AC_TYPE				1			//参考福建程序,实现三相三线100V/三线四线220V自适应的功能，
#if AUTO_AC_TYPE==1 && (HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4)
#undef CONFIG_AC_33
#undef Volt57p7Enable
#endif

#define ESAM_ENCRYPT_EN             (HARDWARE_TYPE == 1 || HARDWARE_TYPE == 2 || HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4)    //ESAM加密使能开关
#define ESAM_ENCRYPT_EN_2012        (ESAM_ENCRYPT_EN == 1)    //2012版ESAM加密使能开关
#define BOOTSCREEN                  1               //显示启动时候显示静态画面
#define SpeBOOTSCREEN               BOOTSCREEN      //启动时候做的特殊显示画面
#define GUI_SYSVERSION_EN            1                //0表示不显示系统版本，轮显画面以及调试信息第六页中的备IP的显示照常显示
                                                    //1表示去除轮显画面以及调试信息第六页中的备IP的显示，由显示系统版本代替

#define DEL_DAYDATA_EN              1               //小于5M 的时候删除一些数据，先删除最早日期的数据
#if DEL_DAYDATA_EN ==1
#define DEL_DATA_CAP                (5*1024*1024)   //小于5M 的时候删除一些数据，先删除最早日期的数据
#define DEL_DAY_CAP                 1               //容量小于DEL_DATA_CAP的时候一次删除的天数
#endif

#define TH208_RTU                    (HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4)            //TH208终端的宏
#define ZJ_PRTL                        0            //浙江规约
#define GB_PRTL                        1            //04国标规约

#define MANUFACTURER                "xxxx"       //注: 376终端生产工序改进增补规约要求支持F161下发厂商代号，因此未来肯能会不允许再使用宏MANUFACTURER定义厂商代号

//终端硬件版本号ASCII    4
#if HARDWARE_TYPE == 1            //一型终端
#define HW_VIRSION            "XA13"
#elif HARDWARE_TYPE == 2        //二型终端
#define HW_VIRSION            "XB21"
#elif HARDWARE_TYPE == 3        //三型终端
#define HW_VIRSION            "T208"
#elif HARDWARE_TYPE == 4        //集中器终端
#define HW_VIRSION            "JZQ1"
#elif HARDWARE_TYPE == 9        //集中器终端
#define HW_VIRSION            "JZQ2"
#else
#define HW_VIRSION            "T208"
#endif

#define GetMewMeter_EN              1           //搜表功能

#define TD_SCDMA                    1            //TD-SCDMA功能
#if TD_SCDMA == 1
#define TDSCDMA_ID                    9            /*与主站通讯通道类型 TD_SCDMA*/
#define TDSCDMA_GSM_ID                10            /*与主站通讯通道类型 TD_SCDMA_GSM*/
#endif
#define FASTER_RADIO_EN             1           //高速直通电台功能
#if FASTER_RADIO_EN == 1
#define FASTER_RADIO_ID             12
#endif

#define PRTL_STACK_EXCHANGE_EN          1            //GPRS内置协议栈和外置协议栈切换功能，通过显示和主台规约帧可切换协议栈(需重启终端)

#define GPRS_RETRY_EN                0            //GRPS模块在接收数据报文中，如果SIM卡出现问题接收到URC_CLOSE的主动上报或者接收到主站返回的乱码数据，接收函数返回-1
                                                //通常程序会重新打开模块串口，不会重启模块
                                                //四川三能集中器，在切换到备用端口后，通信出现异常，一直尝试重新打开串口，未能重启模块
                                                //该宏使能后，如果三次尝试重新打开串口，通信仍不成功，就重新切换IP和端口

#if CBTASK_NEW == 1
#define MET07_CBDAY_TO_DAYFIX        1            //07表支持从表中直接获取上一日冻结数据作为日定点数据
#define MET07_JIESR_TO_MONFIX        0            //07表支持从表中直接获取上一结算日数据作为月定点数据
#define MET97_SHANGRI_TO_MONFIX        0            //97表支持从表中直接获取数据作为月定点数据
#define MET07_CBDAY_TO_MONFIX        1            //07表支持从1号前一天数据转存为上月定点数据
#define MET07_RTDATA_TO_MONFIX        0            //07表支持实时数据转存为当月数据
                                                //可以和上面月冻结的宏同时开启,正常用实时数据存,冻结数据抄到后再补冻
#endif

#define DAY_FREEZE_MINITE           0           //日、月冻结实时转存到上一日/月的时间:0时0分钟


#define FAC_MODE                    1            //出厂模式
#define DISTRITE_SCALE              50          //大小台区规模分界线
#define STYLE_NUM_ENABLE            1            //是否支持大类号、小类号
#define MEM_CATA_NEW                0//(STYLE_NUM_ENABLE == 1 && HARDWARE_TYPE == 4)    //新的存储策略
//#if MEM_CATA_NEW == 1
#define	WIDE_COUNT			        16		    //大类号数量
#define NARROW_COUNT		        16		    //小类号数量
//#endif

#define PARACLEAR_WITH_AC			1			//参数初始化时交采的参数也初始化
#define DefualtSetPLCInter		    0	        /*软件根据现场的反馈分为测试版本和出厂版本，
											        此处为载波抄表间隔根据此宏定义进行区别*/
#define COMM_I_AUTO_SWITCH_EN		0			//485-1口维护和抄表自动切换功能,默认关闭

#define AUTOTASK_NEW				1

#define DCD_FUN						TH208_RTU	//设置DCD 模式
#define COMM_DEBUG_PRO				1			//通讯调试信息显示
#define WORK_MODE					1			//服务器功能模式

#define NAND_SPECIAL				1
#define WR_READ_EN					1

//主站设置内置交采测量点时候，同步将交采表645的地址更新
#define ACADDR_FROM_PRTL            (HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4|| HARDWARE_TYPE == 9)

#define USB_CLEAN_ACDATA			1			/*通过插入u盘检测其中的clearjcdata.tar.gz文件来清内置交采数据*/


#define PLC_AMR_EN		    		(HARDWARE_TYPE == 4)   		//低压集抄功能使能
#define PHOTO_PATH					"/mnt/data/photo"
#define ETH_MODEL					1//(HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4)		//以太网模块使能
#define ETH_IP_THREAD				ETH_MODEL					//设置以太网信道IP的线程
#define CLEAR_MTR_HISDATA			1			//重新配置测量点的时候，是否需要清除该测量点的历史数据，默认是关闭的
#define proAdvanceEN				1			//车间生产改进增加的规约(支持了广播(行政区码0x9999 终端地址0)设置和招测)
#define proAdvanceEN2				proAdvanceEN//车间生产改进(第二次增补:修正对行政区码0x9999支持,增补新终端通讯参数初始化功能)
#define BAT_RMT_EN					TH208_RTU ||(HARDWARE_TYPE == 2)//通过遥信位来判断终端电池是否存在
#if BAT_RMT_EN == 1
    #if (HARDWARE_TYPE == 2)
	#define	 BAT_HARDWARE_POS		0x00000010	//电池标识位
	#else
	#define	 BAT_HARDWARE_POS		0x00000100	//电池标识位
	#endif
	#define  BAT_POS			    0x08		//Tmn_Kg_Status_ID中，电池放在D3位
#endif

#if HARDWARE_IV_EN ==1
#define	 BAT_HARDWARE_POS		0x80000000	//电池标识位
#define  BAT_POS			    0x08					//Tmn_Kg_Status_ID中，电池放在D3位
#endif

#define PROTECT_HW				    (HARDWARE_TYPE == 2)		//硬件解除保电功能
#if PROTECT_HW == 1
#define	PROTECT_KEY				    0x01	//保电解除
#define	CHAOBIAO_KEY			    0x02	//抄表
#endif

#define DOUBLE_CHANNEL_EN           0           // 双通道使能

#define GUI_CLEAR_MTRDATA			1			//显示界面设置测量点时候，需要通过IPC清除测量点数据
#define GUI_CLEAR_MTR				0xA0		//在界面手动设置485测量点的时候，清除485数据
#define GUI_CLEAR_DATA				0xA1		//数据初始化
#define GUI_CLEAR_AC_DATA			0x91		//清交采数据

#define COMM_PROTECT				1			//无通讯保护功能
#define PASSWORD_16					1			//16字节密码功能

#define COMM_RETRY					1			//
#if COMM_PROTECT == 1
    //每天零点将接收有效帧计数器清0,每天6:00,判断在6小时内没有收到有效帧,重新启动终端（reboot）
    #define COMM_6_HOUR_IDLESEC        21600
    //每天8点将接收有效帧计数器清0,每天18:00,判断在10小时内没有收到有效帧,重新启动终端（reboot）
    #define COMM_10_HOUR_IDLESEC    36000

    #define PRT_PRINT(x...) ({ if (getenv("DEBUG_PROTECT") != NULL)  printf(x);})
    #define COMM_PROTECT_LOG        "/mnt/log/protect_comm.log"
#endif

#define DATA1F25_PULSE_VLUE         1           //1类数据F25项当测量点为脉冲时是否回值

/********************************************************************************
脉冲模块配置文件:
9260终端中，32路开关量完全作为遥信、脉冲，门节点、保密键、校表键、尾盖等开关量有8路单独的开关量
DEFAULT_JLM_POS、JLM_HARDWARE_POS之间的关系:
JLM_HARDWARE_POS根据硬件情况进行变化，表示门节点在8路上的哪一路
如果硬件配置在第一路则为0x01，第二路为0x02，以此类推，一般来讲，在硬件的第6路，即0x20

DEFAULT_JLM_POS是门接点在遥信中的位置
(因为04规约中实际上没有门接点的概念，但为了和我们终端硬件相匹配，就增加了门接点的概念，实际上对04规约来讲，就是新增了一路遥信
在浙江规约中，就没有该问题，如果在4路遥信基础上配置了1路门接点，那么终端中共有4路遥信+1路门接点)
在04规约中，该属性与遥信路数配置是累加的，如果已经配置了4路遥信，在加上门接点，那么就有5路遥信
如果配置DEFAULT_JLM_POS = 3，那么就是将门接点的变位信息显示为第3路遥信的变位信息
原来3、4路遥信的变位信息则显示为4、5
短接门接点，使用规约召测应该是第3路遥信发生了变位;短接第3路遥信，规约召测第4路遥信发生了变位;1、2路遥信的状态还是和原来一致
一般来将，门接点的位置在第8路，也就是短接门接点后，第8路遥信发生了变位;这样做就不会影响前面的遥信。
如果终端硬件没有门接点，将门接点的位置设为大于8小于17的值即可，规约和显示就都不处理

根据硬件情况正确配置了JLM_HARDWARE_POS值后，DEFAULT_JLM_POS对应位的遥信状态才能被采集到
*******************************************************************************/
#if HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4
    #define  JLM_HARDWARE_POS        0x00000010    //计量门的硬件位置
    #define  FHJ_HARDWARE_POS        0x00000000    //防护键的硬件位置
    #define  DEFAULT_JLM_POS        8            //默认计量门在遥信中的第几路，根据不同地区用户的要求决定 从1开始
#elif HARDWARE_TYPE == 2
    #define  JLM_HARDWARE_POS        0x00        //物理上计量门的位置 0x10 表示是扩展板的第5路
    #define  FHJ_HARDWARE_POS        0x00000040    //防护键的硬件位置
    #define  DEFAULT_JLM_POS        9            //默认计量门在遥信中的第几路，根据不同地区用户的要求决定 从1开始
    #if PROTECT_HW == 1
        #define  PRJ_HARDWARE_POS    0x00000080        //接解除电键的硬件位置
        //#define  CB_HARDWARE_POS    0x00001000        //抄表键的硬件位置
    #endif
#else
    #define  JLM_HARDWARE_POS        0x00100000    //物理上计量门的位置 0x10 表示是扩展板的第5路
    #define  FHJ_HARDWARE_POS        0x00000000    //防护键的硬件位置
    #define  DEFAULT_JLM_POS        8            //默认计量门在遥信中的第几路，根据不同地区用户的要求决定 从1开始
#endif

/*******************************************************************************
    遥信脉冲配置，如果某位是遥信或者脉冲，将相应位置1。
    如果基本板1、2路为脉冲，则PULSE_BASE为0x03；如果扩展板7、8路为脉冲，则PULSE_EXTEND为0xC0；遥信配置类似
    注意的是不要将某一位同时置为1(既是遥信也是脉冲)，这样程序会出错。
    PULSE_BASE        基本板脉冲配置情况
    PULSE_EXTEND    扩展板脉冲配置情况
    SIG_BASE        基本板遥信配置情况
    SIG_EXTEND        扩展板遥信配置情况
    PULSE_MAX_NUM   系统最大脉冲点(物理脉冲端子,注意不是脉冲测量点)数量,端子编号1~8
    YAOXIN_MAX_NUM  系统最大遥信点(物理脉冲端子)数量,端子编号1~8
*******************************************************************************/
#if HARDWARE_TYPE == 1            //一型终端
    #if HARDWARE_IV_EN == 1
    #define PULSE_BASE             0xFF
    #define PULSE_EXTEND        0x00
    #define SIG_BASE             0xFF
    #define SIG_EXTEND            0xFF
    #define PULSE_MAX_NUM         8
    #define YAOXIN_MAX_NUM         16
    #else
    #define PULSE_BASE         0xFF
    #define PULSE_EXTEND    0x00
    #define SIG_BASE         0x00
    #define SIG_EXTEND        0x7F
    #define PULSE_MAX_NUM     8
    #define YAOXIN_MAX_NUM     8
    #endif
#elif HARDWARE_TYPE == 2        //二型终端 8~16位 前2mc后4yx
    #define PULSE_BASE             0x00
    #define PULSE_EXTEND        0x03
    #define SIG_BASE             0x0F
    #define SIG_EXTEND            0x00
    #define PULSE_MAX_NUM         2
    #define YAOXIN_MAX_NUM         4
#elif HARDWARE_TYPE == 3        //三型终端
    #define PULSE_BASE             0x0C        //默认基本板3、4路为脉冲
    #define PULSE_EXTEND        0x00        //
    #define SIG_BASE             0x03        //默认基本板1、2路为遥信
    #define SIG_EXTEND            0x00        //
    #define PULSE_MAX_NUM         2
    #define YAOXIN_MAX_NUM         2
#elif HARDWARE_TYPE == 4        //集中器终端
    #define PULSE_BASE             0x00        //
    #define PULSE_EXTEND        0x00        //
    #define SIG_BASE             0x0F        //默认基本板1、2、3、4路为遥信
    #define SIG_EXTEND            0x00        //
    #define PULSE_MAX_NUM         0
    #define YAOXIN_MAX_NUM         4
#elif HARDWARE_TYPE == 9        //II型集中器
    #define PULSE_BASE             0x00        //
    #define PULSE_EXTEND        0x00        //
    #define SIG_BASE             0x01        //默认基本板1路为遥信
    #define SIG_EXTEND            0x00        //
    #define PULSE_MAX_NUM         0
    #define YAOXIN_MAX_NUM         1
#else
    #define PULSE_BASE             0x3C        //默认基本板3、4路为脉冲
    #define PULSE_EXTEND        0x00        //默认扩展板3、4路为脉冲
    #define SIG_BASE             0xC3        //默认基本板1、2路为遥信
    #define SIG_EXTEND            0x00        //默认扩展板1、2路为遥信
    #define PULSE_MAX_NUM         4
    #define YAOXIN_MAX_NUM         4
#endif

#if HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4        //261
    #define AC261_CALC_EN           1           //261内置交采使能,该标志打开，同时须打开AC_CALC_EN
    #define AC_CALC_EN              1           //内置交采使能
    #define HR_CALC_EN              1           //内置谐波使能
#elif HARDWARE_TYPE == 9
    #define AC261_CALC_EN           1           //261内置交采使能
    #define AC_CALC_EN              1           //内置交采使能
    #define HR_CALC_EN              0           //内置谐波使能
#elif HARDWARE_TYPE == 1
    #define AC261_CALC_EN           0           //261内置交采使能
    #define AC_CALC_EN              1           //内置交采使能
    #define HR_CALC_EN              1           //内置谐波使能
#elif HARDWARE_TYPE == 2
    #define AC261_CALC_EN           0           //261内置交采使能
    #define AC_CALC_EN              1           //内置交采使能
    #define HR_CALC_EN              0           //内置谐波使能
#else                                             //a11
    #define AC261_CALC_EN           0           //261内置交采使能
    #define AC_CALC_EN              0           //内置交采使能
    #define HR_CALC_EN              0           //内置谐波使能
#endif

#define RELAYAC_EN                    AC261_CALC_EN    //是否允许对内置交采表下发中继命令,III型与集中器

#if 1
    #define RRLAYAC_COMM            3            //穿透内置交采的时候，主台需要下发的端口号
    #define RELAYAC_PORT_ID            4009        //定义与ACSampling进程间通信的端口号
    #define CMDID_WRITE                0x04        //写数据
#endif

//电容器无功投切控制使能，仅针对交采，故前提AC_CALC_EN须打开
//无功投切功能和控制功能冲突，所以控制功能必须关闭
#define CAPACITOR_GRP_NUM           4           //电容器组数
#define CAPACITOR_NUM       16             //参数设置查询终端支持设置的电容器组数
#if AC_CALC_EN == 1 && CONTROL_EN == 0
    #define CAPACITOR_CTRL_EN       1             //当前任何时候关闭无功投切
#else
    #define CAPACITOR_CTRL_EN       0             //当前任何时候关闭无功投切
    #define CAPACITOR_CTRLPARASET_EN       1             //不带电容投切时运行终端设置其电容参数
#endif

#if HARDWARE_TYPE == 1        //一型终端
    #if HARDWARE_IV_EN == 1
    #define TURN_NUM                    8            //轮次
    #else
    #define TURN_NUM                    4            //轮次
    #endif
#elif HARDWARE_TYPE == 2    //二型终端
    #define TURN_NUM                    4            //轮次
#else
    #define TURN_NUM                    2            //轮次
#endif

#if AC261_CALC_EN == 0 && AC_CALC_EN == 1
#define OS_JCTASK_EN             1
#else
#define OS_JCTASK_EN             0
#endif

#define TTYS0 0
#define TTYS1 1
#define TTYS2 2
#define TTYS3 3
#define TTYS4 4
#define TTYS5 5
#define TTYS6 6

#if (HARDWARE_TYPE == 1)
    #define PIPE0 TTYS2
    #define PIPE1 TTYS3
    #define PIPE2 254
    #define PIPE3 TTYS6
    #define PIPE4 254
    #define PIPE5 254
#elif (HARDWARE_TYPE == 2)
    #define PIPE0 TTYS2
    #define PIPE1 TTYS3
    #define PIPE2 254
    #define PIPE3 254
    #define PIPE4 254
    #define PIPE5 254
#elif HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4
    #define PIPE0 TTYS2
    #define PIPE1 TTYS3//新国网将维护485设置成可以抄表可以校表口，可以相互切换
    //#define PIPE1 254
    #define PIPE2 254
    #define PIPE3 254
    #define PIPE4 254
    #define PIPE5 254
#elif HARDWARE_TYPE == 9
    #define PIPE0 TTYS2
    #define PIPE1 TTYS3//
    #define PIPE2 254
    #define PIPE3 254
    #define PIPE4 254
    #define PIPE5 254
    #define PIPE6 TTYS6
#endif

/*******************************************************************************
*系统支持的485抄表通道数量
*如果为1，表示系统支持一个485抄表口，
*如果为2，表示系统支持两个485抄表口
*******************************************************************************/
#define CB_PIPE_NUM                    6            //平台最多3个抄表通道不要修改，哪个通道没有，关闭下面的宏定义
#define PIPE0_ONOFF                 1           //通道0  ，都有
#define PIPE1_ONOFF                 1            //通道1,  现在也全部都打开
#if HARDWARE_IV_EN == 1
#define PIPE2_ONOFF                 1            //通道2   can0
#else
#define PIPE2_ONOFF                 0            //通道2   电流环
#endif

//#if OS_JCTASK_EN == 1                           //通道3   接交采表
#define PIPE3_ONOFF                 1
//#else
//    #define PIPE3_ONOFF             0
//#endif
#define PIPE4_ONOFF                 1  //通道4接脉冲表
#if HARDWARE_IV_EN == 1
#define PIPE5_ONOFF                 1  //通道2   can1
#else
#define PIPE5_ONOFF                 0
#endif

#if HARDWARE_TYPE == 1
#define WH_485_MAX_NUM              1           // 专变I型只有一路内部交采通讯口
#else
#define WH_485_MAX_NUM              2           // 维护485的最大数量
#endif

#define PORT_PRTL_AC                1           //主站下发交采测量点时候使用的端口号
#define PORT_PRTL_485_1             2           //主站下发第1路485时候使用的端口号
#define PORT_PRTL_485_2             3           //主站下发第2路485时候使用的端口号
#define PORT_PRTL_PULSE             11          //主站下发脉冲表使用的端口号
#define PORT_PRTL_BROAD_PLC         28          //抄读宽带载波时使用的端口号
#define PORT_PRTL_PLC               31          //主站下发载波测量点时候使用的端口号

#define PIPE_METER_485_1            1           //抄表模块抄读第1路485时候使用的通道号
#define PIPE_METER_485_2            2           //抄表模块抄读第2路485时候使用的通道号
#if OS_JCTASK_EN == 0
#define PIPE_METER_AC               3           //抄表模块抄读内置交采时候使用的通道号
#else
#define PIPE_METER_AC               4           //抄表模块抄读内置交采时候使用的通道号
#endif
#define PIPE_METER_PULSE            5           //抄表模块抄读脉冲表使用的通道号
#define PIPE_METER_AC_INNER         3           //内置交采端口号(专变I型降成本、III型、集中器)
#define PIPE_METER_BROAD_PLC        28          //抄读宽带载波时使用的端口号
#define PIPE_METER_PLC              31          //抄表模块抄读载波测量点时候使用的通道号

#define PIPE1_485_DEFAULT_CB           1         //485 I  口默认为抄表口
#define PIPE2_485_DEFAULT_CB           0         //485 II 口默认为维护口

/********************************************************************************/


//以下配置内容不用修改
#if HARDWARE_TYPE == 9
#define NO_VIR_MTR_MAX_NUM            192                            //不包含总加组的最大测量点个数(含载波测量点)
#define MTR485_MAX_NUM                192                            //485测量点的个数(始终为64)
#elif HARDWARE_TYPE == 4
#define MTR485_MAX_NUM                64                            //485测量点的个数(始终为64)
#define NO_VIR_MTR_MAX_NUM            3500                        //不包含总加组的最大测量点个数
#else
#define MTR485_MAX_NUM                64                            //485测量点的个数(始终为64)
#define NO_VIR_MTR_MAX_NUM            64                        //不包含总加组的最大测量点个数

#endif


//#define MTR_MAX_NUM                    (NO_VIR_MTR_MAX_NUM    + 16)    //包含总加组的最大测量点个数

//系统485计量点和内置交采计量点数量
#define MTRPLS_MAX_NUM              8                                        //脉冲测量点个数
#define AC_MTR_NO                      (NO_VIR_MTR_MAX_NUM + 1)                // 交采测量点号
#define PULSE_MTR_NO_BEGIN              (AC_MTR_NO+1)                              // 脉冲测量点起始号

#define VIR_BEGIN                   (PULSE_MTR_NO_BEGIN + 8)    //总加组起始编号, 只有04规约需要从65开始，普通测量点1~64
#define    REPEATERS_MAX_NUM              64
#define    JOIN_MAX_NUM                3                            //级联最大数量，集抄部分增加
#define    MTRVIP_MAX_NUM                20                            //重点户个数
#define DataDir_ROUTE               16                            //系统字典定义支持的最大总加组数
#define CtrlGrp_ROUTE                  8                              //总加组数


#define MTR_MAX_NUM                    (VIR_BEGIN + CtrlGrp_ROUTE) //所有测量点的最大数量

#define MEMFILE_MTR_NUM                (MTRPLS_MAX_NUM + CtrlGrp_ROUTE + 1) // 日、月、曲线冻结的文件中头中测量点的数量
                                                                         // 包括交采、脉冲、总加组


#define DEFAULT_FEE_NUM             4                            //默认费率数


#if HARDWARE_TYPE == 1 || HARDWARE_TYPE == 2 || HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4 || HARDWARE_TYPE == 9
    #define LMT_CDMA_ONLINE         0            //限制CDMA的永久在线功能
    #define TIME_CTRL_ON_LINE       1            //时段在线功能
    #define NET_FLUX_CTRL           1            //网络日流量控制(对日流量698规约只统计不控制)
    #define NET_FLUX_CTRL_MON          1            //网络月流量控制
    #define APP_CTRL_ON_LINE        0            //应用层控制上线
    #define GPRS_FLUX_CTRL_OFF      0           //GPRS流量控制关闭-送检测试时台体会把流量门限设置成1会掉线
#else
    #define LMT_CDMA_ONLINE         0            //限制CDMA的永久在线功能
    #define TIME_CTRL_ON_LINE       0            //时段在线功能
    #define NET_FLUX_CTRL           0            //网络日流量控制(对日流量698规约只统计不控制)
    #define NET_FLUX_CTRL_MON          0            //网络月流量控制
    #define APP_CTRL_ON_LINE        0            //应用层控制上线
    #define GPRS_FLUX_CTRL_OFF      0           //GPRS流量控制关闭-送检测试时台体会把流量门限设置成1会掉线
#endif

#if NET_FLUX_CTRL_MON == 1
//终端初始化后，会将流量默认为0xffffffff,这样主站召测时候可以回否认
//默认一个50M的流量，在通讯中使用，可以防止流量超出
//500M的流量一般是足够了，不会出现不够用的情况 500 * 1024 * 1024 = 52,428,8000
#define MAX_FLUX_MON                524288000L
#define MAX_FLUX_PLUS_MON            10485760L               //10M，超流量保护功能（超过流量门限10M后下线，不再允许拨号）
#define TMN_COMM_LIMIT_EXT          1
#define TMN_COMM_LIMIT_FILE         "^OverFluxLmtTest"
#endif
/*******************************************************************************/

#if (DELIVER_DETECT_EN == 1)
    #if JIANG_SU_FUN == 1
    #define POWER_CUT_SAFE             1            //上电保电功能选择，在送检版本中建议定义为0，朴军海
    #define DEFAULT_PW_PARA            99900000000LL
    #else
    #define POWER_CUT_SAFE             1            //上电保电功能选择，在送检版本中建议定义为0，朴军海
    #define DEFAULT_PW_PARA            100L
    #endif
#else
    #define POWER_CUT_SAFE             1            //上电保电功能选择，在送检版本中建议定义为0，朴军海
    #define DEFAULT_PW_PARA            99900000000LL
#endif

#define CB_INTERVAL                 2            //默认485抄表周期-先定成2分钟，面向对象测试软件抄表周期未确定暂定2分钟

#define ANOLOGY_EN                    0            //直流模拟量使能

//抄表模块宏定义
#define OS_RELAYTASK_EN              1
#define OS_METLGR_EN                 1
#define OS_METLGR2FRAME_EN           1
#define OS_METDLT645_EN              1
#define OS_METABB_EN                 1
#define OS_METEMAIL_EN                1
#define OS_METWS20_EN                1
#define OS_METHND_EN                 1
#define OS_METHT_EN                  1
#define OS_METEDMI_EN                1
#define OS_METJSBLACK_EN             0              //数据字典谐波部分对应不起来
#define OS_METDSSD25_EN              1
#define OS_JSSIMULATE_EN             1
#define OS_CQINTELL_EN               0              //数据字典缺项，只针对重庆
#define OS_ZJSIM_EN                  0              //数据字典缺项，只针对浙江模拟表
#define OS_METHR_EN                  1
#define OS_METZMC_EN                 1
#define OS_METZZ_EN              ZHENGZHOU_FUN      //郑州表,与ZHENGZHOU_FUN宏一起打开
#define OS_METFJ_EN                  1
#define OS_METCBH_EN                 1
#define OS_METSTD2007_EN            1           //2007表规约使能开关
#if HARDWARE_TYPE == 1
#define OS_JC2007TASK_EN            1           // 2007规约交采使能
#else
#define OS_JC2007TASK_EN            0           // 2007规约交采使能
#endif
#define OS_METDLMS_EN               1           //DLMS电能表
#define OS_SMARTBREAKER_EN          0          //江苏I型集中器支持智能断路器通信协议功能

/*******************************************************************************
* 显示模块设置
* 修改显示模块值需要对应修改build.make文件
*******************************************************************************/
#define GUI_FUN_EN                    (HARDWARE_TYPE != 9)           //GUI功能使能
#define GUI_LOGON_LAYER              0x000f0000
#define MAX_TRUNDISP_CNT              180            //轮显最大个数
#define NEW_GW_GUI_APPEND           1           //新国网新增功能

#if NEW_GW_GUI_APPEND == 1
#define GUI_RELAY_PN_FILE           "^GuiRelayPnInfo"  /*保存点抄的测量点号
                                                        *第1个字节为点抄启动标志0x55有效,0xBB为点抄取消态
                                                        *其余为0xAA
                                                        *第2、3个字节为要点抄的数目
                                                        *第4、5个字节起为要点抄测量号,第6为点抄成功标志
                                                        *后面重复第4、5、6个字节格式
                                                       */
#endif


//设置os操作系统路径
#if HARDWARE_TYPE == 1            //一型终端
    #define DRIVER_BAOMI            0             //是否读取保密驱动 0:没有保密键 1：有保密键(231类型) 2：有保密键(2型终端类型)
    #define DRIVER_CAOBIAO          1             //是否读取抄表驱动 0:没有抄表按键 1:有抄表按键(2型，01g有抄表按键)
    #define MID_LCD_STYLE              1           //0:160x160 1:240x128
    #define GUI_DISP_TYPE             2           //0:2型风格(无状态栏) 1:231风格(有状态栏)  2:01g风格(240x128)
    #define GUI_TYPE_DIR            "240-128"
#elif HARDWARE_TYPE == 2        //二型终端
    #define DRIVER_BAOMI            1             //是否读取保密驱动 0:没有保密键 1：有保密键(231类型) 2：有保密键(2型终端类型)
    #define DRIVER_CAOBIAO          1             //是否读取抄表驱动 0:没有抄表按键 1:有抄表按键(2型，01g有抄表按键)
    #define MID_LCD_STYLE              0             //0:160x160 1:240x128
    #define GUI_DISP_TYPE             1           //0:2型风格(无状态栏) 1:231风格(有状态栏)  2:01g风格(240x128)
    #define GUI_TYPE_DIR            "style231"
#elif HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4 || HARDWARE_TYPE == 9        //三型终端
    #if HARDWARE_TYPE == 3
    #define DRIVER_BAOMI            1             //是否读取保密驱动 0:没有保密键 1：有保密键(231类型) 2：有保密键(2型终端类型)
    #else    //集中器没有保密键
    #define DRIVER_BAOMI            0             //是否读取保密驱动 0:没有保密键 1：有保密键(231类型) 2：有保密键(2型终端类型)
    #endif
    #define DRIVER_CAOBIAO          0             //是否读取抄表驱动 0:没有抄表按键 1:有抄表按键(2型，01g有抄表按键)
    #define MID_LCD_STYLE              0             //0:160x160 1:240x128
    #define GUI_DISP_TYPE             1           //0:2型风格(无状态栏) 1:231风格(有状态栏)  2:01g风格(240x128)
    #define GUI_TYPE_DIR            "style231"
#else
    #define DRIVER_BAOMI            1             //是否读取保密驱动 0:没有保密键 1：有保密键(231类型) 2：有保密键(2型终端类型)
    #define DRIVER_CAOBIAO          0             //是否读取抄表驱动 0:没有抄表按键 1:有抄表按键(2型，01g有抄表按键)
    #define MID_LCD_STYLE              0             //0:160x160 1:240x128

    #define GUI_DISP_TYPE             1           //0:2型风格(无状态栏) 1:231风格(有状态栏)  2:01g风格(240x128)
    #define GUI_TYPE_DIR            "style231"
#endif

#if SAL_PRTL == 1
#define SecMod_SAL                  0x10        //SAL 密文
#endif
// 双通道时改变串口号，宏放到地区的最下面
#if DOUBLE_CHANNEL_EN == 1
#define COMMU_TTYS_EXT                  5            //扩展的通讯通道使用串口

#undef INFRARED_TTYS
#define INFRARED_TTYS                      99            //主通讯通道使用串口
#endif

#endif
