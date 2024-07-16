/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：规约解析表
* @date： 2019.12.03
*********************************************************************
*/


#ifndef __OAD_INFO_LIST_H__
#define __OAD_INFO_LIST_H__

#ifdef __cplusplus
extern "C"{
#endif

/* 数据类型编码 */
typedef enum _DATA_E
{	
    //
    //OAD相关数据类型编码
    //

    /*通用数据类型编码*/
    E_ARRAY,
    E_STRUCTURE,  
    E_BOOL,				   
    E_BIT_STRING,			   
    E_DOUBLE_LONG,			   
    E_DOUBLE_LONG_UNSIGNED,   
    E_OCTET_STRING,
    E_OCTET_STR2_T,
    E_OCTET_STR3_T,
    E_OCTET_STR4_T,
    E_OCTET_STR32_T,
    E_OCTET_STRING_2K,	
    E_OOP_OCTETVAR16_T,  	   
    E_VISIBLE_STRING,		   
    E_UTF8_STRING,			   
    E_INTEGER,				   
    E_LONG,				   
    E_UNSIGNED,			   
    E_LONG_UNSIGNED,		   
    E_LONG64,				   
    E_LONG64_UNSIGNED,		   
    E_ENUM,				   
    E_FLOAT32, 			   
    E_FLOAT64,				   
    E_DATETIME,			   
    E_DATE,				   
    E_TIME,				   
    E_DATETIME_S,			   
    E_OI,					   
    E_OAD,					   
    E_ROAD,				   
    E_OMD,					   
    E_TI,					   
    E_TSA,					   
    E_MAC,					   
    E_RN,					   
    E_REGION,				   
    E_SCALER_UNIT,			   
    E_RSD,					   
    E_CSD,					   
    E_MS,					   
    E_SID,                    
    E_SIDMAC,                 
    E_COMDCB,				   
    E_RCSD,	
    E_OOP_BITSTR8_T,  //bit-string(SIZE(8))  
    E_OOP_BITSTR64_T,
    E_OOP_BITSTR128_T,
    E_OOP_VISIBLEVAR_T, 
    E_OOP_VISIBLESTR4_T,                 
    E_OOP_VISIBLESTR6_T,            
    E_OOP_VISIBLESTR8_T, 
    E_OOP_OCTETVAR256_T,            
    E_OOP_OCTETVAR1K_T,    
    E_OOP_OCTETVAR2K_T,  
    E_OOP_VISIBLEVAR32_T, 
    E_OOP_SA_T,                      

    /*电量量类*/
    E_OOP_ENERGY_T,                 //电能量(不带符号)     
    E_OOP_ENERGYS_T,                //电能量(带符号)
    E_OOP_ENERGY_ONE_T,             //电能量(不带符号)     
    E_OOP_ENERGYS_ONE_T,            //电能量(带符号)
    E_OOP_HENERGY_T,                //高精度电能量(不带符号)
    E_OOP_HENERGYS_T,	            //高精度电能量(带符号)
    E_OOP_HENERGY_ONE_T,            //高精度电能量(不带符号)
    E_OOP_HENERGYS_ONE_T,	        //高精度电能量(带符号)

    /*需量类*/
    E_OOP_DEMAND_T,                 //需量数组(不带符号)
    E_OOP_DEMANDS_T,                //需量数组(带符号)

    E_OOP_DEMAND_ONE_T,             //需量数组(不带符号)
    E_OOP_DEMANDS_ONE_T,            //需量数组(带符号)

    /*变量类*/
    E_OOP_DWORD3V_T,                //分相变量类  
    E_OOP_WORD3V_T,
    E_OOP_INT3V_T,
    /*功率类*/
	E_OOP_INT4V_T,
	E_OOP_LONG4V_T,
    E_OOP_HARMONIC_T,               //谐波 
    E_OOP_HARMONIC_U_VALUE_T,               //电压谐波含量
    E_OOP_HARMONIC_I_VALUE_T,               //电流谐波含量
    E_OOP_METWORD_T,                //电能表运行状态字          
    E_OOP_BITSTR16_T,               //bit-string(SIZE(16))                         
    E_OOP_REGION_T,                 //区间统计结果集合
    E_OOP_REGIONS_T,                //区间统计对象集合       
    E_OOP_TOTALAVG_T,               //累计平均值定义
    E_OOP_COGNATES_T,               //累加平均关联对象集合
    E_OOP_EXTREMERES_T,             //                 
    E_OOP_VOLTSTAT_T,               //           
    E_OOP_DWORD2_T,             
    E_OOP_WORD2_T,        
    E_OOP_PTCT_T, 
    E_OOP_PULSECFG_T,              
    E_OOP_PULSESETS_T,              //                                 
    E_OOP_PULSEUNIT_T,              //                                                                                                                    

    /*事件类*/
    E_OOP_OADS_T,                   //一组对象属性描述符           
    E_OOP_EVTCFG_3000_T,            //3000-电能表失压事件配置参数
    E_OOP_MEVENT_T,                 //分相事件记录 
    E_OOP_EVTSOURCE_T,              //事件发生源
    E_OOP_LOSTVSTAT_T,           
    E_OOP_MEVTTIME_T,               //                          
    E_OOP_EVTCFG_WD_T,              //       
    E_OOP_EVTCFG_3003_T,            //        
    E_OOP_EVTCFG_3004_T,            //           
    E_OOP_EVTCFG_ID_T,              //          
    E_OOP_EVTCFG_3006_T,            //                                   
    E_OOP_DEMANDLOG_T,              //                                         
    E_OOP_EVTTIME_T,                //                                 
    E_OOP_EVTCFG_DD_T,              //                                            
    E_OOP_CFG_LU_T,              //                    
    E_OOP_EVTCFG_DELAY_T,           //                                                             
    E_OOP_PROGRAMLOG_T,             //                              
    E_OOP_EVENTRESETLOG_T,          //                             
    E_OOP_PROGPERIODLOG_T,          //                                           
    E_OOP_PROGHOLIDAYLOG_T,         //                
    E_OOP_ABNORMALCARD_T,           //      
    E_OOP_REBATELOG_T,              //                   
    E_OOP_CLOCKFLYLOG_T,          
    E_OOP_EVTCFG_3105_T,            //         
    E_OOP_POWERLOG_T,               //           
    E_OOP_POWERONCUT_T,             //            
    E_OOP_EVTCFG_INT_T,          
    E_OOP_EVTSTAT_T,                //          
    E_OOP_EVTCFG_310B_T,            //    
    E_OOP_EVENT_T,                  //    
    E_OOP_EVTCFG_310X_T,            //  
    E_OOP_EVTCFG_310E_T,            //     
    E_OOP_EVTCFG_310F_T,         
    E_OOP_EVTCFG_3110_T,            //      
    E_OOP_SMLOG_T,                  //       
    E_OOP_UNMETERLOG_T,             //     
    E_OOP_RCTRLLOG_T,               //遥控跳闸记录    
    E_OOP_DCLOG_T,                  //      
    E_OOP_EVTCFG_3116_T,            //                     
    E_OOP_METEROFFLOG_T,            //  
    E_OOP_EVTCFG_311A_T,            //        
    E_OOP_P2PCLOCKLOG_T,            //          
    E_OOP_METERDIRTYLOG_T,          //               
    E_OOP_PCTRLLOG_T,               //功控跳闸记录                              
    E_OOP_ECTRLLOG_T,               //电控跳闸记录              
    E_OOP_EWARNLOG_T,               //电控告警事件   
    E_OOP_REPORTS_T,
    E_OOP_SECURITYLOG_T,            //安全变更记录

    /*参变量类*/       
    E_OOP_SYNCCLOCK_T,                    
    E_OOP_GEOGADD_T,            
    E_OOP_GRPADDR_T,            
    E_OOP_CLOCK_T,              
    E_OOP_LCD_T,                    //          
    E_OOP_PERIOD_T,                 //            
    E_OOP_PUBHOLIDAY_T,             
    E_OOP_TIMEZONE_T,               //   
    E_OOP_DAYPERIOD_T,              //  
    E_OOP_RATETARIFF_T,            // 
    E_OOP_STEPPRICE_T,             // 
    E_OOP_LIMITCHARGE_T,        
    E_OOP_OTHERLIMIT_T,         
    E_OOP_POWERLIMIT_T,         
    E_OOP_OTHERPOWERLIMIT_T,    
    E_OOP_VISIBLESTR16_T,           //            
    E_OOP_DC_UPDATE_T,          
    E_OOP_DCU_CONFIG_T,         
    E_OOP_VOLTPARAM_T,              //                
    E_OOP_COEFFICIENT_T,        
    E_OOP_VISIBLEVAR8S_T,        
    E_OOP_PAYDAYS_T,            
    E_OOP_TI_T,                 
    E_OOP_CASCADE_T,            
    E_OOP_SYNCTIME_T,           
    E_OOP_P2PSYNCTIME_T,            // 
    E_OOP_VERSION_T,                //   
    E_OOP_OIS_T,                
    E_OOP_PROTOCOL_T,
    E_OOP_CHANNEL_T,  
    E_OOP_OI_RIGHTS_T,            
    E_OOP_ASSOCIATION_CONTEXT_T,    //                
    E_OOP_GPRSCONFIG_T,         
    E_OOP_MASTERPARAMS_T,       
    E_OOP_SMS_T,                    //            
    E_OOP_VISIBLESTR20_T,       
    E_OOP_VISIBLESTR15_T,           //                                                 
    E_OOP_ETHCONFIG_T,              // 
    E_OOP_ETHIP_T,              
    E_OOP_ETHMAC_T,                 //                              
    E_OOP_APNGROUP_T,           


    /*冻结类*/
    E_OOP_FROZENSETS_T,

    /*采集类*/
    E_OOP_METER_LIST,                //档案列表
    E_OOP_METER_T,                   //档案单元
    E_BASIC_OBJECT,                  //档案基本信息
    E_EXTENDED_OBJECT,               //档案扩展信息
    E_ANNEX_OBJECT_LIST,             //档案附加信息列表
    E_ANNEX_OBJECT,	                 //档案附加信息单元  
    E_METER_M133_T,                  //删除指定档案                                                   
    E_PWD_COMM,                      //                                              
    E_ASSETNO,                       //
    E_OOP_SEARCHMET_RES_LIST,        //                                     
    E_OOP_SEARCHMET_RES_T,           //
    E_OOP_ACROSSAREA_RES_LIST,
    E_OOP_ACROSSAREA_RES_T,          //     
    E_OOP_SEARCHPARAM_T,             //
    E_OOP_TIMINGSEARCH_T,            //
    E_OOP_TASK_LIST,
    E_OOP_TASK_T,                    //
    E_OOP_PLAN_NORMAL_T,             //
    E_OOP_PLAN_EVENT_T,              //
    E_OOP_PLAN_TRANS_T,              //
    E_OOP_TRANSRES_T,                //
    E_OOP_PLAN_REPORT_T,             //
    E_OOP_ACQ_MONITOR_T,             //
    E_OOP_ACQ_RLUESTATE_T,           //
    E_OOP_ADDEDINFO_LIST,            //
    E_OOP_ADDEDINFO_T,               //

  

    /*集合类*/                                                                  

    /*控制接口类*/
    E_OOP_TELECON_PARAM_T,            //远程控制 - 配置参数
    E_OOP_PRTTIME_T,                  //自动保电时段
    E_OOP_CALLFEE_T,                  //催费告警参数，只读
    E_CHINESEINFO,                    //一般中文信息
    E_OOP_POWCON_TIME_T,              //终端功控时段 
    E_OOP_POWCON_WARNING_T,           //功控告警时间
    E_OOP_TGSTATES_T,                 //总加组状态
    E_OOP_TURNSTATES_T,               //轮次状态
    E_OOP_8103_CONFIG_T,              //时段功控   
    E_OOP_8104_CONFIG_T,              //厂休控   
    E_OOP_8105_CONFIG_T,              //营业报停控  
    E_OOP_8107_CONFIG_T,              //购电控   
    E_OOP_8108_CONFIG_T,              //月电控 
      
    /*文件传输接口类*/      
    E_OOP_FILE_T,                     //文件分帧传输管理- 文件信息
    E_OOP_FILEEXC_RST_E,              //文件分帧传输管理 - 命令结果
    E_OOP_FTPSERVER_T,                //文件扩展传输管理 - 服务器信息
      
    /*ESAM接口类*/      
    E_OOP_LICENSEVER_T,               //证书版本
    E_OOP_SECURITYFLAG_E,             //安全模式开关 
      
    /*输入输出类*/      
    E_OOP_SERIALPORT_T,               //RS232列表参数
    E_OOP_RS485_T,                    //RS485列表参数
    E_OOP_IR_T,                       //红外列表参数
    E_OOP_SWITCHIN_STATE_T,           //信号量列表参数
    E_OOP_SWITCHIN_FLAG_T,            //信号量接入属性
    E_OOP_ANALOG_T,                   //直流模拟量
    E_OOP_ANALOG_CONFIG_T,            //直流模拟量配置
    E_OOP_SWITCHOUT_T,                //继电器
    E_OOP_WARNINGOUT_T,               //告警输出
    E_OOP_WARNINGTIME_T,              //允许告警时段
    E_OOP_MUTIFUNCTERMINAL_T,         //多功能端子
    E_OOP_AC_T,                       //交采接口  
    E_OOP_ROUTEMODULE_T,              //载波/微功率无线接口，本地通信模块单元参数
    E_OOP_CHILDNODE_T,                //载波/微功率无线接口，从节点对象列表
    E_OOP_PULSE_T,                    //脉冲输入端口描述
    E_OOP_RADIO_T,                    //230无线专网模块
    E_OOP_RADIO_CHS_T,                //频道设置
    E_OOP_RADIO_SIGS_T,               //有效信号强度
    E_OOP_USB_T,                      //USB设备



    //
    //OMD相关数据类型编码MD
    //

    /*参变量类OMD列表*/

    /*采集类OMD列表*/

    /*控制接口类OMD列表*/

    /*文件传输接口类OMD列表*/

    /*ESAM接口类OMD列表*/
    E_OOP_CUR_COUNT_T,           //当前计数器
    E_SECURITYMODE,              //增加显式安全模式参数（参数）
    E_OOP_SCURITYMODE_T,         //删除显式安全模式参数（对象标识）
    E_OOP_PORTAUTHORIZE_T,       //端口授权
    E_OOPM_ESAM_7_T,             //密钥更新
    E_OOPM_ESAM_8_T,             //证书更新

    /*输入输出类OMD列表*/
    E_OOPM_SERIALPORT_CONFIG_T,  //RS232配置参数
    E_OOPM_IR_CONFIG_T,          //红外配置参数
    E_OOPM_SWITCHOUT_127_T,      //开关量配置参数
    E_OOPM_MULTIOUT_CONFIG_T,    //多功能端子修改工作模式
    E_OOPM_TRANSCMD_T,           //透明转发（参数）
    E_OOPM_PORTPARAM_T,          //配置端口参数（参数）

    /*总加组类*/
    E_OOP_TGSETS_T,
    E_OOP_TGSETTING_T,
    E_OOP_TGCURSTATE_T,
    E_OOP_TGUNIT_T,
    
    /*其他*/    
    E_OOP_VOLTQR_T,
    E_OOP_WORD4V_T,
    E_FRZ_DATETIME_S,
    E_REAL_DATETIME_S,
    E_OOP_WALLET_T,
    
    E_DAYPERIOD,
    E_TIMEZONE,
    E_OOP_PAYDAY_T,
    E_OOP_ST_T,
    E_OOP_MODULELOG_T,
    E_DATA,
//    E_OOP_BITSTR8_T,
//    E_OOP_BITSTR16_T,
//    E_OOP_BITSTR64_T,
//    E_OOP_BITSTR96_T,
//    E_OOP_BITSTR128_T,
    E_NULL = 0xFFFFFFFF,
}DATA_E;

typedef struct
{
    OOP_OAD_U      oad;        //要转换的OAD    
    OOP_CLASS_E    class;      //属于的类
    uint8          OIA1;       //对象标识中A1的值
    uint8          OIA2;       //对象标识中A2的值
    uint32         rsv;        //备用
    OOP_DATATYPE_E dataType;   //数据类型
    uint32         dataLen;    //oad对应的长度 加数据类型
    DATA_E         eData;      //要转成的结构体类型
}OAD_TAB_T;

typedef struct
{
    const OAD_TAB_T *pTab;   //表信息
    uint32    nIndex;        //属性索引
    uint8     noParse;       //不解析数据，1-是，0-否
    uint8     isRec;         //记录型OAD，1-是，0-否    
    DATA_E    subeData;      //表中只有块OAD时，模糊查表后,子结构的数据类型
    uint8     subParse;      //解析子结构，用于关联OAD， 1-是，0-否
}OAD_INFO_T;

typedef struct
{
    OOP_OMD_U      omd;        //要转换的OAD    
    OOP_CLASS_E    class;      //属于的类
    uint8          OIA1;       //对象标识中A1的值
    uint8          OIA2;       //对象标识中A2的值
    uint32         rsv;        //备用
    OOP_DATATYPE_E dataType;   //数据类型
    uint32         dataLen;    //oad对应的长度 加数据类型
    DATA_E         eData;      //要转成的结构体类型
}OMD_INFO_T;

//
//oad列表声明
//

extern const uint32 oadEnergyInfoListLen;
extern const OAD_TAB_T oadEnergyInfoList[];

extern const uint32 oadDemandInfoListLen;
extern const OAD_TAB_T oadDemandInfoList[];

extern const uint32 oadVariateInfoListLen;
extern const OAD_TAB_T oadVariateInfoList[];

extern const uint32 oadEvnetInfoListLen;
extern const OAD_TAB_T oadEvnetInfoList[];

extern const uint32 oadParaInfoListLen;
extern const OAD_TAB_T oadParaInfoList[];

extern const uint32 oadFreezeInfoListLen;
extern const OAD_TAB_T oadFreezeInfoList[];

extern const uint32 oadCollectInfoListLen;
extern const OAD_TAB_T oadCollectInfoList[];

extern const uint32 oadGatherInfoListLen;
extern const OAD_TAB_T oadGatherInfoList[];

extern const uint32 oadControlInfoListLen;
extern const OAD_TAB_T oadControlInfoList[];

extern const uint32 oadFileInfoListLen;
extern const OAD_TAB_T oadFileInfoList[];

extern const uint32 oadEsamInfoListLen;
extern const OAD_TAB_T oadEsamInfoList[];

extern const uint32 oadIoInfoListLen;
extern const OAD_TAB_T oadIoInfoList[];

//
//omd列表声明
//

extern const OMD_INFO_T omdVariateInfoList[];
extern const uint32 omdVariateInfoListLen;

extern const uint32 omdParaInfoListLen;
extern const OMD_INFO_T omdParaInfoList[];

extern const uint32 omdCollectInfoListLen;
extern const OMD_INFO_T omdCollectInfoList[];

extern const uint32 omdControlInfoListLen;
extern const OMD_INFO_T omdControlInfoList[];

extern const uint32 omdFileInfoListLen;
extern const OMD_INFO_T omdFileInfoList[];

extern const uint32 omdEsamInfoListLen;
extern const OMD_INFO_T omdEsamInfoList[];

extern const uint32 omdIoInfoListLen;
extern const OMD_INFO_T omdIoInfoList[];

extern const uint32 omdTgInfoListLen;
extern const OMD_INFO_T omdTgInfoList[];

extern const uint32 omdEvtInfoListLen;
extern const OMD_INFO_T omdEvtInfoList[];

extern const uint32 omdFrozenInfoListLen;
extern const OMD_INFO_T omdFrozenInfoList[];


//
//OI属性权限列表
//

typedef struct{
	uint8 attr_id;                 //属性或方法ID
	OOP_ATTRIBUTE_ACESS_E right;   //访问权限 
}aarAccessRight_t;

typedef struct{
	uint8 attr_id;                 //属性或方法ID
	OOP_ACTION_ACCESS_E   right;   //访问权限 
}marAccessRight_t;

typedef struct{
	uint16 OI;
	uint8 ClassID;
	uint8 AttrNum;
	uint8 MethodNum;
	const aarAccessRight_t *aarAccessRight;
	const marAccessRight_t *marAccessRight;
}ClassBaseList_t;

typedef struct{
	const ClassBaseList_t *pList;
	uint16 num;
}ObjectLit_t;

//Class1
extern const aarAccessRight_t Class1AAR[];
extern const marAccessRight_t Class1MAR[];

//Class2
extern const aarAccessRight_t Class2AAR[];
extern const marAccessRight_t Class2MAR[];

//Class3
extern const aarAccessRight_t Class3AAR[];
extern const aarAccessRight_t Class3AAR_2001[];
extern const marAccessRight_t Class3MAR[];

//Class4
extern const aarAccessRight_t Class4AAR[];
extern const marAccessRight_t Class4MAR[];

//Class5
extern const aarAccessRight_t Class5AAR[];
extern const marAccessRight_t Class5MAR[];

//Class6
extern const aarAccessRight_t Class6AAR[];
extern const marAccessRight_t Class6MAR[];


//Class7
extern const aarAccessRight_t Class7AAR[];
extern const marAccessRight_t Class7MAR[];

//Class8
extern const aarAccessRight_t Class8AAR[];
extern const aarAccessRight_t Class8AAR_1[];
extern const aarAccessRight_t Class8AAR_202F[];
extern const aarAccessRight_t Class8AAR_4000[];
extern const aarAccessRight_t Class8AAR_4204[];
extern const aarAccessRight_t Class8AAR_4401[];
extern const aarAccessRight_t Class8AAR_F101[];
extern const aarAccessRight_t Class8AAR_8000[];
extern const aarAccessRight_t Class8AAR_8001[];
extern const marAccessRight_t Class8MAR[];
extern const marAccessRight_t Class8MAR_1[];
extern const marAccessRight_t Class8MAR_4000[];
extern const marAccessRight_t Class8MAR_4001[];
extern const marAccessRight_t Class8MAR_4006[];
extern const marAccessRight_t Class8MAR_F101[];
extern const marAccessRight_t Class8MAR_8000[];
extern const marAccessRight_t Class8MAR_8001[];
extern const marAccessRight_t Class8MAR_8002[];

//Class9
extern const aarAccessRight_t Class9AAR[];
extern const marAccessRight_t Class9MAR[];

//Class9
extern const aarAccessRight_t Class17AAR[];
extern const marAccessRight_t Class17MAR[];

//Class10
extern const aarAccessRight_t Class10AAR[];
extern const marAccessRight_t Class10MAR[];
extern const marAccessRight_t Class10MAR_6012[];

//Class11
extern const aarAccessRight_t Class11AAR[];
extern const aarAccessRight_t Class11AAR_6002[];
extern const marAccessRight_t Class11MAR[];
extern const marAccessRight_t Class11MAR_4200[];
extern const marAccessRight_t Class11MAR_6000[];
extern const marAccessRight_t Class11MAR_6002[];
extern const marAccessRight_t Class11MAR_6014[];
extern const marAccessRight_t Class11MAR_6016[];
extern const marAccessRight_t Class11MAR_6018[];
extern const marAccessRight_t Class11MAR_601C[];
extern const marAccessRight_t Class11MAR_601E[];
extern const marAccessRight_t Class11MAR_6051[];
#if 1
extern const marAccessRight_t Class11MAR_7000[];
extern const marAccessRight_t Class11MAR_7010[];
#endif
extern const marAccessRight_t Class11MAR_800X[];

//Class12
extern const aarAccessRight_t Class12AAR[];
extern const marAccessRight_t Class12MAR[];

//Class13
extern const aarAccessRight_t Class13AAR[];
extern const aarAccessRight_t Class13AAR_8106[];
extern const marAccessRight_t Class13MAR[];
extern const marAccessRight_t Class13MAR_8106[];


//Class14
//Class15
//Class16
//Class17
//Class18
extern const aarAccessRight_t Class18AAR_F000[];
extern const aarAccessRight_t Class18AAR_F001[];
extern const aarAccessRight_t Class18AAR_F002[];
extern const marAccessRight_t Class18MAR_F000[];
extern const marAccessRight_t Class18MAR_F001[];
extern const marAccessRight_t Class18MAR_F002[];

//Class19
extern const aarAccessRight_t Class19AAR[];
extern const marAccessRight_t Class19MAR[];

//Class20
extern const aarAccessRight_t Class20AAR[];
extern const marAccessRight_t Class20MAR[];

//Class21
extern const aarAccessRight_t Class21AAR[];
extern const marAccessRight_t Class21MAR[];

//Class22
extern const aarAccessRight_t Class22AAR[];
extern const marAccessRight_t Class22MAR[];
extern const marAccessRight_t Class22MAR_1[];
extern const marAccessRight_t Class22MAR_2[];

//Class23
extern const aarAccessRight_t Class23AAR[];
extern const marAccessRight_t Class23MAR[];


//Class24
extern const aarAccessRight_t Class24AAR[];
extern const marAccessRight_t Class24MAR[];

//Class25
extern const aarAccessRight_t Class25AAR[];
extern const marAccessRight_t Class25MAR[];

//Class26
extern const aarAccessRight_t Class26AAR[];
extern const marAccessRight_t Class26MAR[];

//1个管理逻辑设备+3个逻辑设备
extern const ObjectLit_t ObjectList_All[4]; 

#ifdef __cplusplus
}
#endif

#endif /*__OAD_INFO_LIST_H__*/

