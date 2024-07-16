/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief����Լ������
* @date�� 2019.12.03
*********************************************************************
*/


#ifndef __OAD_INFO_LIST_H__
#define __OAD_INFO_LIST_H__

#ifdef __cplusplus
extern "C"{
#endif

/* �������ͱ��� */
typedef enum _DATA_E
{	
    //
    //OAD����������ͱ���
    //

    /*ͨ���������ͱ���*/
    E_ARRAY,
    E_STRUCTURE,  
    E_BOOL,				   
    E_BIT_STRING,			   
    E_DOUBLE_LONG,			   
    E_DOUBLE_LONG_UNSIGNED,   
    E_OCTET_STRING,
    E_OCTET_STR2_T,  //SIZE(2)
    E_OCTET_STR3_T,  //SIZE(3)
    E_OCTET_STR4_T,  //SIZE(4)
    E_OCTET_STR6_T,  //SIZE(6)
    E_OCTET_STR8_T,  //SIZE(8)
    E_OCTET_STR32_T, //SIZE(32)
    E_OCTET_STRING_2K,	
    E_OOP_OCTETVAR16_T,  	   
    E_VISIBLE_STRING,		//�䳤������   
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
    E_MS_F,
    E_MS_C,					   
    E_SID,                    
    E_SIDMAC,                 
    E_COMDCB,				   
    E_RCSD,
    E_OOP_BITSTR3_T,  //bit-string(SIZE(8))  
    E_OOP_BITSTR8_T,  //bit-string(SIZE(8))  
    E_OOP_BITSTR64_T,
    E_OOP_BITSTR128_T,
    E_OOP_VISIBLEVAR_T, 
    E_OOP_VISIBLESTR4_T,                 
    E_OOP_VISIBLESTR6_T,            
    E_OOP_VISIBLESTR8_T, 
    E_OOP_VISIBLESTR11_T, 
    E_OOP_VISIBLESTR256_T,  //�䳤��������������Ҫ��VAR
    E_OOP_OCTETVAR256_T,            
    E_OOP_OCTETVAR1K_T,    
    E_OOP_OCTETVAR2K_T,  
    E_OOP_VISIBLEVAR32_T, 
    E_OOP_OCTETVAR64_T,
    E_OOP_SA_T,                      
    E_OOP_NETST_T,
    E_OOP_PRIVATE_NET_T,
    E_WIRELESS_INFO_T,

    /*��������*/
    E_OOP_ENERGY_T,                 //������(��������)     
    E_OOP_ENERGYS_T,                //������(������)
    E_OOP_HENERGY_T,                //�߾��ȵ�����(��������)
    E_OOP_HENERGYS_T,	            //�߾��ȵ�����(������)

    /*������*/
    E_OOP_DEMAND_T,                 //��������(��������)
    E_OOP_DEMANDS_T,                //��������(������)

    /*������*/
    E_OOP_DWORD3V_T,                //��������� 
    E_OOP_WORD3V_T,
    E_OOP_LONG3V_T,
    E_OOP_INT3V_T,
    /*������*/
	E_OOP_INT4V_T,
	E_OOP_LONG4V_T,
    E_OOP_HARMONIC_T,               //г��  
    E_OOP_HARMONIC_U_VALUE_T,       //��ѹг������
    E_OOP_HARMONIC_I_VALUE_T,       //����г������
    E_OOP_METWORD_T,                //���ܱ�����״̬��          
    E_OOP_BITSTR16_T,               //bit-string(SIZE(16))
    E_OOP_BITSTR32_T,               //bit-string(SIZE(32))
    E_OOP_LOOPSTAT_T,
    E_OOP_REGION_T,                 //����ͳ�ƽ������
    E_OOP_REGIONS_T,                //����ͳ�ƶ��󼯺�       
    E_OOP_TOTALAVG_T,               //�ۼ�ƽ��ֵ����
    E_OOP_COGNATES_T,               //�ۼ�ƽ���������󼯺�
    E_OOP_EXTREMERES_T,             //                 
    E_OOP_VOLTSTAT_T,               //           
    E_OOP_DWORD2_T,             
    E_OOP_WORD2_T,        
    E_OOP_PTCT_T, 
    E_OOP_PULSECFG_T,              
    E_OOP_PULSESETS_T,              //                                 
    E_OOP_PULSEUNIT_T,              //                                                                                                                    

    /*�¼���*/
    E_OOP_OADS_T,                   //һ���������������           
    E_OOP_EVTCFG_3000_T,            //3000-���ܱ�ʧѹ�¼����ò���
    E_OOP_MEVENT_T,                 //�����¼���¼ 
    E_OOP_EVTSOURCE_T,              //�¼�����Դ
    E_OOP_LOSTVSTAT_T,           
    E_OOP_MEVTSTAT_T,
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
    E_OOP_EVTCFG_3600_T,            // 
    E_OOP_SMLOG_T,                  //       
    E_OOP_UNMETERLOG_T,             //     
    E_OOP_RCTRLLOG_T,               //ң����բ��¼    
    E_OOP_DCLOG_T,                  //      
    E_OOP_EVTCFG_3116_T,            //                     
    E_OOP_METEROFFLOG_T,            //  
    E_OOP_EVTCFG_311A_T,            //        
    E_OOP_P2PCLOCKLOG_T,            //          
    E_OOP_METERDIRTYLOG_T,          //  
    E_OOP_METOFFLINELOG_T,
    E_OOP_REFUSENODEACCESSLOG_T,
    E_OOP_ABNORMALCTLOG_T,
    E_OOP_PCTRLLOG_T,               //������բ��¼                              
    E_OOP_ECTRLLOG_T,               //�����բ��¼              
    E_OOP_EWARNLOG_T,               //��ظ澯�¼�   
    E_OOP_REPORTS_T,
    E_OOP_SECURITYLOG_T,            //��ȫ�����¼
    E_OOP_IdentifyResult_T,         //����ʶ���¼
    E_OOP_PF_INPUT_EXIT_T,
    E_OOP_SYSTEMMONITOR_T,

    /*�α�����*/       
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
    E_OOP_TASPCUSEMOD_T,
    E_OOP_WORKINFO_T,
    E_OOP_TAUNIT_T,
    E_OOP_CTINFO_T,
    E_OOP_CURLOOPCKENABLE_T,
    E_OOP_COEFFICIENT_T,        
    E_OOP_VISIBLEVAR8S_T,        
    E_OOP_PAYDAYS_T,            
    E_OOP_TI_T,   
    E_OOP_NETTOPOLOGYINFO_T,
    E_OOP_NETSINFO_T,
    E_OOP_NODEPHASEINFO_T,
    E_OOP_NETBASEINFO_T,
    E_OOP_NETNODEINFO_T,
    E_OOP_CASCADE_T,            
    E_OOP_SYNCTIME_T,           
    E_OOP_P2PSYNCTIME_T,            // 
    E_OOP_VERSION_T,                //   
    E_OOP_OIS_T,                
    E_OOP_PROTOCOL_T,
    E_OOP_CHANNEL_T,  
    E_OOP_SERVICES_T,
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
    E_OOP_DEVICERUNSTAT_T,
    E_PILE_WORK_STATUS_T,
    E_CHARGE_SERVICE_T,
    E_OOP_REFUSESLAVE_T,
    E_OOP_HARMONIC_DOUBLE_VALUE,
    E_OOP_HARMONIC_DOUBLE_CUR_VALUE,
    E_OOP_SOFTREC_LIST,
    E_OOP_LOAD_I0_EVECFG_T,
    /*������*/
    E_OOP_FROZENSETS_T,

    /*�ɼ���*/
    E_OOP_METER_LIST,                //�����б�
    E_OOP_METER_T,                   //������Ԫ
    E_BASIC_OBJECT,                  //����������Ϣ
    E_EXTENDED_OBJECT,               //������չ��Ϣ
    E_ANNEX_OBJECT_LIST,             //����������Ϣ�б�
    E_ANNEX_OBJECT,	                 //����������Ϣ��Ԫ  
    E_METER_M133_T,                  //ɾ��ָ������                                                   
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

  

    /*������*/                                                                  
    E_OOP_CONTAINER_LIST,           //  ��������
    E_OOP_APP_INFO_LIST,            //Ӧ�ü���
    /*���ƽӿ���*/
    E_OOP_TELECON_PARAM_T,            //Զ�̿��� - ���ò���
    E_OOP_PRTTIME_T,                  //�Զ�����ʱ��
    E_OOP_CALLFEE_T,                  //�߷Ѹ澯������ֻ��
    E_CHINESEINFO,                    //һ��������Ϣ
    E_OOP_POWCON_TIME_T,              //�ն˹���ʱ�� 
    E_OOP_POWCON_WARNING_T,           //���ظ澯ʱ��
    E_OOP_TGSTATES_T,                 //�ܼ���״̬
    E_OOP_TURNSTATES_T,               //�ִ�״̬
    E_OOP_8103_CONFIG_T,              //ʱ�ι���   
    E_OOP_8104_CONFIG_T,              //���ݿ�   
    E_OOP_8105_CONFIG_T,              //Ӫҵ��ͣ��  
    E_OOP_8107_CONFIG_T,              //�����   
    E_OOP_8108_CONFIG_T,              //�µ�� 
    E_OOP_GDK_PARAM_T,                //��������õ�Ԫ
      
    /*�ļ�����ӿ���*/      
    E_OOP_FILE_T,                     //�ļ���֡�������- �ļ���Ϣ
    E_OOP_FILEEXC_RST_E,              //�ļ���֡������� - ������
    E_OOP_FTPSERVER_T,                //�ļ���չ������� - ��������Ϣ
      
    /*ESAM�ӿ���*/      
    E_OOP_LICENSEVER_T,               //֤��汾
    E_OOP_SECURITYFLAG_E,             //��ȫģʽ���� 
      
    /*���������*/      
    E_OOP_SERIALPORT_T,               //RS232�б����
    E_OOP_RS485_T,                    //RS485�б����
    E_OOP_IR_T,                       //�����б����
    E_OOP_SWITCHIN_STATE_T,           //�ź����б����
    E_OOP_SWITCHIN_FLAG_T,            //�ź�����������
    E_OOP_ANALOG_T,                   //ֱ��ģ����
    E_OOP_ANALOG_CONFIG_T,            //ֱ��ģ��������
    E_OOP_SWITCHOUT_T,                //�̵���
    E_OOP_WARNINGOUT_T,               //�澯���
    E_OOP_WARNINGTIME_T,              //����澯ʱ��
    E_OOP_MUTIFUNCTERMINAL_T,         //�๦�ܶ���
    E_OOP_AC_T,                       //���ɽӿ�  
    E_OOP_ROUTEMODULE_T,              //�ز�/΢�������߽ӿڣ�����ͨ��ģ�鵥Ԫ����
    E_OOP_CHILDNODE_T,                //�ز�/΢�������߽ӿڣ��ӽڵ�����б�
    E_OOP_PULSE_T,                    //��������˿�����
    E_OOP_RADIO_T,                    //230����ר��ģ��
    E_OOP_RADIO_CHS_T,                //Ƶ������
    E_OOP_RADIO_SIGS_T,               //��Ч�ź�ǿ��
    E_OOP_USB_T,                      //USB�豸


    E_OOP_OBJ_SHOW_T,
    E_OOP_OBJ_SHOW_PARAM_T,
    //
    //OMD����������ͱ���MD
    //

    /*�α�����OMD�б�*/

    /*�ɼ���OMD�б�*/

    /*���ƽӿ���OMD�б�*/

    /*�ļ�����ӿ���OMD�б�*/

    /*ESAM�ӿ���OMD�б�*/
    E_OOP_CUR_COUNT_T,           //��ǰ������
    E_SECURITYMODE,              //������ʽ��ȫģʽ������������
    E_OOP_SCURITYMODE_T,         //ɾ����ʽ��ȫģʽ�����������ʶ��
    E_OOP_PORTAUTHORIZE_T,       //�˿���Ȩ
    E_OOPM_ESAM_7_T,             //��Կ����
    E_OOPM_ESAM_8_T,             //֤�����
    E_OOP_LOG_INFO_T,
    /*���������OMD�б�*/
    E_OOPM_ADDPORT_T,
    E_OOPM_SERIALPORT_CONFIG_T,  //RS232���ò���
    E_OOPM_IR_CONFIG_T,          //�������ò���
    E_OOPM_SWITCHOUT_127_T,      //���������ò���
    E_OOPM_MULTIOUT_CONFIG_T,    //�๦�ܶ����޸Ĺ���ģʽ
    E_OOPM_TRANSCMD_T,           //͸��ת����������
    E_OOPM_PORTPARAM_T,          //���ö˿ڲ�����������
    E_OOP_TERMINAL_LIMIT_T,
    E_OOP_BROADCASTCLOCK_T,

    /*�ܼ�����*/
    E_OOP_TGSETS_T,
    E_OOP_TGSETTING_T,
    E_OOP_TGCURSTATE_T,
    E_OOP_TGUNIT_T,
    
    /*����*/    
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
    E_PCSD,
//    E_OOP_BITSTR8_T,
//    E_OOP_BITSTR16_T,
//    E_OOP_BITSTR64_T,
//    E_OOP_BITSTR96_T,
//    E_OOP_BITSTR128_T,
    E_OOP_LNLS_STATISVAL_T,
    E_OOP_BREAK_RUN_STATUS_T,
    E_OOP_LNLSPER_T,
	E_OOP_LNLS_UNIT_T,
	E_OOP_Power_Characteristic_Record,
	E_OOP_Energy_Characteristic_Record,
	E_OOP_LINE_LOSS_T,
	E_OOP_SWITCH_IMPEDANCE_T,
#ifdef AREA_HUNAN
    E_OOP_TemperatureWarningParam_T,
    //E_OOP_NodeTopology_T,
    E_OOP_IMPE_OVER_T,
    E_OOP_EVTCFG_31F2_T,
    E_OOP_SENSOR_PARAM_T,
#endif
    E_OOP_NodeTopology_T,

#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)
    //E_OOP_LINE_LOSS_T,
    //E_OOP_SWITCH_IMPEDANCE_T,
#endif
    E_OOP_DEVFREZDATAS_T,
	#ifdef AREA_SHANDONG
    E_OOP_HUMITURE_2E20_T,
    E_OOP_LOCK_STATE_2E21_T,
    E_OOP_HARMONIC_VOL_T,
    E_OOP_HARMONIC_CUR_T,
#endif
    //E_OOP_BREAK_RUN_STATUS_T,
    //E_OOP_Power_Characteristic_Record,
    //E_OOP_Energy_Characteristic_Record,

#ifdef AREA_SHAANXI
	E_OOP_ICCIDLOG_T,				//ICCID����¼�
	E_4GMONITOR_T,					//4G��������Ϣ
#endif

#ifdef AREA_FUJIAN
        E_BATTERY_STATUS_T,             //ͣ�繤�����״̬
        E_SINGAL_INFO_T,                //�ֳ��ź���Ϣ
#endif

    E_Water_Temperature_T,
    E_OOP_FLOW_PARAM_T,
    E_CHECKSUM,                        //24�淶У������������
    E_NULL = 0xFFFFFFFF,
}DATA_E;

typedef struct
{
    OOP_OAD_U      oad;        //Ҫת����OAD    
    OOP_CLASS_E    class;      //���ڵ���
    uint8          OIA1;       //�����ʶ��A1��ֵ
    uint8          OIA2;       //�����ʶ��A2��ֵ
    uint32         rsv;        //����
    OOP_DATATYPE_E dataType;   //��������
    uint32         dataLen;    //oad��Ӧ�ĳ��� ����������
    DATA_E         eData;      //Ҫת�ɵĽṹ������
}OAD_TAB_T;

typedef struct
{
    const OAD_TAB_T *pTab;   //����Ϣ
    uint32    nIndex;        //��������
    uint8     noParse;       //���������ݣ�1-�ǣ�0-��
    uint8     isRec;         //��¼��OAD��1-�ǣ�0-��    
    DATA_E    subeData;      //����ֻ�п�OADʱ��ģ������,�ӽṹ����������
    uint8     subParse;      //�����ӽṹ�����ڹ���OAD�� 1-�ǣ�0-��
}OAD_INFO_T;

typedef struct
{
    OOP_OMD_U      omd;        //Ҫת����OAD    
    OOP_CLASS_E    class;      //���ڵ���
    uint8          OIA1;       //�����ʶ��A1��ֵ
    uint8          OIA2;       //�����ʶ��A2��ֵ
    uint32         rsv;        //����
    OOP_DATATYPE_E dataType;   //��������
    uint32         dataLen;    //oad��Ӧ�ĳ��� ����������
    DATA_E         eData;      //Ҫת�ɵĽṹ������
}OMD_INFO_T;

//
//oad�б�����
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

extern const uint32 oadGuiListLen;
extern const OAD_TAB_T oadGuiList[];

extern const uint32 oadSelfInfoListLen;
extern const OAD_TAB_T oadSelfInfoList[];

//
//omd�б�����
//

extern const OMD_INFO_T omdVariateInfoList[];
extern const uint32 omdVariateInfoListLen;

extern const uint32 omdParaInfoListLen;
extern const OMD_INFO_T omdParaInfoList[];

extern const uint32 omdCollectInfoListLen;
extern const OMD_INFO_T omdCollectInfoList[];

extern const uint32 omdGatherListLen;
extern const OMD_INFO_T omdGatherInfoList[];

extern const uint32 omdControlInfoListLen;
extern const OMD_INFO_T omdControlInfoList[];

extern const uint32 omdFileInfoListLen;
extern const OMD_INFO_T omdFileInfoList[];

extern const uint32 omdEsamInfoListLen;
extern const OMD_INFO_T omdEsamInfoList[];

extern const uint32 omdIoInfoListLen;
extern const OMD_INFO_T omdIoInfoList[];

extern const uint32 omdGuiListLen;
extern const OMD_INFO_T omdGuiList[];

extern const uint32 omdSelfInfoListLen;
extern const OMD_INFO_T omdSelfInfoList[];

extern const uint32 omdTgInfoListLen;
extern const OMD_INFO_T omdTgInfoList[];

extern const uint32 omdEvtInfoListLen;
extern const OMD_INFO_T omdEvtInfoList[];

extern const uint32 omdFrozenInfoListLen;
extern const OMD_INFO_T omdFrozenInfoList[];

extern const uint32 omdStatInfoListLen;
extern const OMD_INFO_T omdStatInfoList[];


//
//OI����Ȩ���б�
//

typedef struct{
	uint8 attr_id;                 //���Ի򷽷�ID
	OOP_ATTRIBUTE_ACESS_E right;   //����Ȩ�� 
}aarAccessRight_t;

typedef struct{
	uint8 attr_id;                 //���Ի򷽷�ID
	OOP_ACTION_ACCESS_E   right;   //����Ȩ�� 
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

//1�������߼��豸+3���߼��豸
extern const ObjectLit_t ObjectList_All[4]; 

#ifdef __cplusplus
}
#endif

#endif /*__OAD_INFO_LIST_H__*/

