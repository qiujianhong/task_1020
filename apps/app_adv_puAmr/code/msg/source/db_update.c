/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   高级APP对外通信模块 数据中心更新
* @date：    2019-12-31
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "db_api.h"
#include "task_id.h"
#include "task_msg.h"
#include "msg_id.h"

#include "msg_base.h"
#include "db_update.h"
#include "EventIO.h"
#include "app_gui.h"
#include "appcomm.h"


#define UPDATE_ALL     0xffff
#define UPOAD_NULL     0
#define UPMASK_ALL     0xffffffff
#define UPINIT_ALL     0xff
#define UPCLASS_ALL    0xffff


extern void app_get_initlock();
extern int appdb_get_localaddr(int sockdbfd, uint8 logicId, uint16 infoNum, uint32 value);
extern int appdb_get_groupaddr(int sockdbfd, uint8 logicId, uint16 infoNum, uint32 value);
extern int appdb_get_password(int sockdbfd, uint8 logicId, uint16 infoNum, uint32 value);
extern int appdb_get_context(int sockdbfd, uint8 logicId, uint16 infoNum, uint32 value);
extern int appdb_get_version(int sockdbfd, uint8 logicId, uint16 infoNum, uint32 value);

extern void pulse_oad_notify_init();
extern int pulse_oad_notify(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int appdb_get_meter_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void meter_list_lock_init(void);

extern int appdb_get_seek_meter_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int appdb_get_encryption_res_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void seek_meter_list_lock_init(void);
extern void seek_encryption_list_lock_init(void);

extern int appdb_get_seek_across_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void seek_across_list_lock_init(void);

extern int appdb_get_nodeversion_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void nodeversion_list_lock_init(void);

extern int appdb_get_childnode_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void childnode_list_lock_init(void);

extern int appdb_get_task_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void task_list_lock_init(void);

extern int appdb_get_plan_report_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void plan_report_list_lock_init(void);

extern int appdb_get_plan_normal_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void plan_normal_list_lock_init(void);

extern int appdb_get_plan_event_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void plan_event_list_lock_init(void);


extern int comm_change_update(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void comm_change_init();

extern int stat_change_update(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void stat_change_init();

extern int rpt_mtr_evt_update(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void rpt_mtr_evt_init();
extern int rpt_trans_result_update(int clientid, uint8 logicId, uint16 infoNum, uint32 oadMain, uint8 oadColsNum, uint32* oadCols);

extern int update_ac_data(int clientid, uint8 logicId, uint16 infoNum, uint32 value);

extern int GetEventNomalParam(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void CheckERC_3104(DB_CLIENT fd, OOP_SWITCHIN_STATE_T switchTemp);
extern int evt_init_event_deal(int clientid, uint8         initType, uint8 logicId, uint16 classTag);
extern int UpdateLatestMeterReadDataList(int clientid, uint8 logicId, uint16 infoNum, uint32 oadMain, uint8 oadColsNum, uint32* oadCols);
extern void InitLatestMeterReadDataLock(void);

extern void appctrl_oad_notify_init();
extern int appctrl_oad_notify(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int creat_chip_module_change_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int creat_refuseslave_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int creat_identify_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value);

#ifdef AREA_ZHEJIANG
extern int appdb_get_cbir_result_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void cbir_list_lock_init(void);
extern void app_datainit_cbir_result(uint8 logicid,uint16 classtag);
#endif

extern int creat_route_module_change_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int creat_smlog_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int creat_unmeterlog_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int appdb_get_plan_trans_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void plan_trans_list_lock_init(void);
extern int app_timingmode_read(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int app_syncclock_read(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void sync_clock_param_lock_init(void);
extern void ethip_info_init(void);
extern int ethip_info_read(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int creat_meter_adjust_tmie_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value);

extern int appmain_buzzer_warn_data(int clientid, uint8 logicId, uint16 infoNum, uint32 value);

extern void sync_search_param_lock_init(void);
extern int app_search_param_read(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void sync_cycle_param_lock_init(void);
extern int app_time_sync_cycle_read(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int creat_communicate_module_change_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value);

#ifdef AREA_HUNAN
extern int get_smoke_state(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int get_water_state(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int get_temperature(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int get_switch_state(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int get_temperature_param(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
#endif

#ifdef AREA_FUJIAN
extern void task_list_lock_init_fj(void);
extern int appdb_get_task_list_fj(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void plan_report_list_lock_init_fj(void);
extern int appdb_get_plan_report_list_fj(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int app_server_info_read(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int app_ftp_info_read(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void app_ftp_info_lock_init(void);
extern int rpt_fj_evt_plan_update(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
#endif

extern int appdb_get_clientNO(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int update_perceptionterminal_info(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
OOP_PERCEPTION_TERMINAL_LIST_T perceptionterminal;

int DB_Update_0x40010200(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int ret = 0;
    ret = appdb_get_localaddr(clientid, logicId, infoNum, value);
    ret |= comm_change_update(clientid, logicId, infoNum, value);
    return ret;
}

int DB_Update_0xf2030400(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int ret = 0;
    ret = GetEventNomalParam(clientid, logicId, infoNum, value);
    ret |= pulse_oad_notify(clientid, logicId, infoNum, value);
//#if defined PRODUCT_CCU || defined PRODUCT_ECU
    ret |= update_ac_data(clientid, logicId, infoNum, value);
//#endif
    return ret;
}

int DB_Update_0xf2030200(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int ret = 0;
    ret = GetEventNomalParam(clientid, logicId, infoNum, value);
//#if defined PRODUCT_CCU || defined PRODUCT_ECU
    ret |= update_ac_data(clientid, logicId, infoNum, value);
//#endif
    return ret;
}


int DB_Update_0x40160200(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int ret = 0;
    ret = appctrl_oad_notify(clientid, logicId, infoNum, value);
    ret |= pulse_oad_notify(clientid, logicId, infoNum, value);
    return ret;
}

int DB_Update_0x20000200(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int ret = 0;
    ret = GetEventNomalParam(clientid, logicId, infoNum, value);
    ret |= stat_change_update(clientid, logicId, infoNum, value);
//#if defined PRODUCT_CCU || defined PRODUCT_ECU
    ret |= update_ac_data(clientid, logicId, infoNum, value);
//#endif
    return ret;
}

int DB_Update_0xF2060400(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int ret = 0;
    ret = appctrl_oad_notify(clientid, logicId, infoNum, value);
    ret |= appmain_buzzer_warn_data(clientid, logicId, infoNum, value);
    return ret;
}

/* 
 * 数据中心更新
 * 逻辑设备号和信息点号为0xffff, 标识通配;
 * 筛选数据是OAD, 记录型数据更新匹配的主DI, 普通数据更新匹配的是第一个次DI
 * 掩码, 删选数据和更新数据掩码部分相同, 则匹配
 * 需要关注的更新在下面数组中添加
*/
DB_UPDATE_T g_dbUpdateInfo[] = 
{/* 逻辑设备号,  信息点号,   筛选数据,   掩码      , 普通数据更新函数,    记录型数据更新, 初始化函数 */
    {UPDATE_ALL, UPDATE_ALL, 0x0,        UPMASK_ALL, NULL, UpdateLatestMeterReadDataList, InitLatestMeterReadDataLock},
       
    {UPDATE_ALL, UPDATE_ALL, 0x60000200, UPMASK_ALL, appdb_get_meter_list,NULL,           meter_list_lock_init},
    {UPDATE_ALL, UPDATE_ALL, 0x60020200, UPMASK_ALL, appdb_get_seek_meter_list,NULL,      seek_meter_list_lock_init},
    {UPDATE_ALL, UPDATE_ALL, 0x60020500, UPMASK_ALL, appdb_get_seek_across_list,NULL,     seek_across_list_lock_init},
    {UPDATE_ALL, UPDATE_ALL, 0x60020800, UPMASK_ALL, app_search_param_read, NULL,         sync_search_param_lock_init},
    {UPDATE_ALL, UPDATE_ALL, 0x60120200, UPMASK_ALL, appdb_get_task_list, NULL,           task_list_lock_init}, 
    {UPDATE_ALL, UPDATE_ALL, 0x60140200, UPMASK_ALL, appdb_get_plan_normal_list, NULL,    plan_normal_list_lock_init},
    {UPDATE_ALL, UPDATE_ALL, 0x60160200, UPMASK_ALL, appdb_get_plan_event_list,  NULL,    plan_event_list_lock_init},
    {UPDATE_ALL, UPDATE_ALL, 0x60180200, UPMASK_ALL, appdb_get_plan_trans_list,  NULL,    plan_trans_list_lock_init},
    {UPDATE_ALL, UPDATE_ALL, 0x601C0200, UPMASK_ALL, appdb_get_plan_report_list, NULL,    plan_report_list_lock_init},
    {UPDATE_ALL, UPDATE_ALL, 0x60200200, UPMASK_ALL, appdb_get_encryption_res_list,NULL,  seek_encryption_list_lock_init},
    {UPDATE_ALL, UPDATE_ALL, 0x40010200, UPMASK_ALL, DB_Update_0x40010200, NULL,          app_get_initlock},
    {UPDATE_ALL, UPDATE_ALL, 0x40010200, UPMASK_ALL, appdb_get_clientNO,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x40000300, UPMASK_ALL, app_timingmode_read, NULL,           sync_clock_param_lock_init},
    {UPDATE_ALL, UPDATE_ALL, 0x40000400, UPMASK_ALL, app_syncclock_read,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x40000600, UPMASK_ALL, app_time_sync_cycle_read, NULL,      sync_cycle_param_lock_init},
    {UPDATE_ALL, UPDATE_ALL, 0x40050200, UPMASK_ALL, appdb_get_groupaddr, NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x44010200, UPMASK_ALL, appdb_get_password,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x44000300, UPMASK_ALL, appdb_get_context,   NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x43000300, UPMASK_ALL, appdb_get_version,   NULL,           NULL},
    //事件模块
    //0x3100,0x3101,0x3104,0x3105,0x3106,0x310a,0x310b,0x310c,0x310d,0x310e,0x310f,0x3110,0x3111,0x3114,0x3118,0x3119,0x311c,0x3115,0x3140,0x3109
    //0600,0800,0900
    {UPDATE_ALL, UPDATE_ALL, 0x31000800, 0xf000f0ff, GetEventNomalParam,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0xf2030200, UPMASK_ALL, DB_Update_0xf2030200,NULL,         NULL},
    {UPDATE_ALL, UPDATE_ALL, 0xf2030400, UPMASK_ALL, DB_Update_0xf2030400,NULL,         NULL},
	{UPDATE_ALL, UPDATE_ALL, 0x22000200, UPMASK_ALL, GetEventNomalParam,  NULL,           NULL},
	{UPDATE_ALL, UPDATE_ALL, 0x60020A00, UPMASK_ALL, GetEventNomalParam,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x43000800, UPMASK_ALL, GetEventNomalParam,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x43000a00, UPMASK_ALL, GetEventNomalParam,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x300f0600, UPMASK_ALL, GetEventNomalParam,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x300f0800, UPMASK_ALL, GetEventNomalParam,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x300f0900, UPMASK_ALL, GetEventNomalParam,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x20140200, UPMASK_ALL, GetEventNomalParam,  NULL,           NULL},
    //0x32000900,0x32010800,0x32010900,0x32020800,0x32020900,0x32030800,0x32030900,
    {UPDATE_ALL, UPDATE_ALL, 0x32000800, 0xff00f0ff, GetEventNomalParam,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x810c0200, UPMASK_ALL, GetEventNomalParam,  NULL,           NULL},

        
    {UPDATE_ALL, UPDATE_ALL, 0x24010300, 0xfff0ffff, pulse_oad_notify,    NULL,           pulse_oad_notify_init},
    {UPDATE_ALL, UPDATE_ALL, 0x24010400, 0xfff0ffff, pulse_oad_notify,    NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x24011C00, 0xfff0ffff, pulse_oad_notify,    NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0xf2030300, UPMASK_ALL, pulse_oad_notify,    NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x49080200, UPMASK_ALL, pulse_oad_notify,    NULL,           NULL},

    {UPDATE_ALL, UPDATE_ALL, 0x40160200, UPMASK_ALL, DB_Update_0x40160200,NULL,           NULL},
        
    {0,          0         , 0x45000200, 0xff00ffff, comm_change_update,  NULL,           comm_change_init},
    {0,          0         , 0x45000300, 0xff00ffff, comm_change_update,  NULL,           NULL},
    {0,          0         , 0x45000400, 0xfff0ffff, comm_change_update,  NULL,           NULL},
    {0,          0         , 0x45000E00, 0xfff0ffff, comm_change_update,  NULL,           NULL},
    {0,          0         , 0x451004FE, 0xfff0ffff, comm_change_update,  NULL,           NULL},

    {UPDATE_ALL, UPDATE_ALL, 0x50000300, UPMASK_ALL, stat_change_update,  NULL,           stat_change_init},
    {UPDATE_ALL, UPDATE_ALL, 0x50010300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x50020300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x50030300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x50040300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x50050300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x50060300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x50070300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21000300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21010300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21020300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21030300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21040300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21100300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21110300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21120300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21130300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21140300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21200300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21210300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21220300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21230300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21240300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x43000B00, UPMASK_ALL, stat_change_update,  NULL,           NULL},    
    {UPDATE_ALL, UPDATE_ALL, 0xF2000500, UPMASK_ALL, stat_change_update,  NULL,           NULL},    
    {UPDATE_ALL, UPDATE_ALL, 0xF2010500, UPMASK_ALL, stat_change_update,  NULL,           NULL},    
    {UPDATE_ALL, UPDATE_ALL, 0xF2020500, UPMASK_ALL, stat_change_update,  NULL,           NULL},    
    {UPDATE_ALL, UPDATE_ALL, 0xF20E0500, UPMASK_ALL, stat_change_update,  NULL,           NULL},    
    {1,          0,          0x20000200, UPMASK_ALL, DB_Update_0x20000200,NULL,           NULL},

//#if defined PRODUCT_CCU || defined PRODUCT_ECU
    {1,          0,          0x00100200, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x00300200, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x00200200, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x00400200, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x00500200, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x00600200, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x00700200, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x00800200, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x20010200, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x20010400, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x20040200, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x20050200, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x200A0200, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x10100200, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x10200200, UPMASK_ALL, update_ac_data,NULL,           NULL},

    {1,          0,          0x00100400, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x00300400, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x00200400, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x00400400, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x00500400, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x00600400, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x00700400, UPMASK_ALL, update_ac_data,NULL,           NULL},
    {1,          0,          0x00800400, UPMASK_ALL, update_ac_data,NULL,           NULL},
//#endif
    {UPDATE_ALL, UPDATE_ALL, 0xFF100A00, UPMASK_ALL, rpt_mtr_evt_update,  NULL,           rpt_mtr_evt_init},
    {UPDATE_ALL, UPDATE_ALL, 0xFF100B00, UPMASK_ALL, rpt_mtr_evt_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0xFF100C00, UPMASK_ALL, rpt_mtr_evt_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0xFF100D00, UPMASK_ALL, rpt_mtr_evt_update,  NULL,           NULL},
    //{UPDATE_ALL, UPDATE_ALL, 0x60020500, UPMASK_ALL, creat_unmeterlog_event,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0xF2090200, UPMASK_ALL, creat_route_module_change_event,NULL,NULL},
    {UPDATE_ALL, UPDATE_ALL, 0xF2090500, UPMASK_ALL, creat_chip_module_change_event, NULL,childnode_list_lock_init},
    {UPDATE_ALL, UPDATE_ALL, 0xF2091500, UPMASK_ALL, creat_refuseslave_event, NULL,       NULL},
    //{UPDATE_ALL, UPDATE_ALL, 0x45000C00, UPMASK_ALL, creat_communicate_module_change_event,NULL,NULL},
    #if AREA_ZHEJIANG
    {UPDATE_ALL, UPDATE_ALL, 0x4E070200, UPMASK_ALL, creat_identify_event, NULL,       NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x4E080200, UPMASK_ALL, appdb_get_cbir_result_list, NULL, cbir_list_lock_init},
    #endif
    //{UPDATE_ALL, UPDATE_ALL, 0x60020200, UPMASK_ALL, creat_smlog_event,   NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0xFFA00100, UPMASK_ALL, creat_meter_adjust_tmie_event, NULL, NULL},
    {UPDATE_ALL, UPDATE_ALL, 0xFF110E00, UPMASK_ALL, update_perceptionterminal_info, NULL, NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x601A0201, 0xffffff00, NULL,  rpt_trans_result_update,      NULL},
    

    //控制模块
    //0x80000200 0x80010200 0x81000200 0x81010200 0x81020200 0x81030200 0x81040200 0x81050200 0x81070200 0x81080200
    {UPDATE_ALL, UPDATE_ALL, 0x80000200, 0xf0f0ffff, appctrl_oad_notify,  NULL,           appctrl_oad_notify_init},
    {UPDATE_ALL, UPDATE_ALL, 0x80007fff, UPMASK_ALL, appctrl_oad_notify,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x800081ff, UPMASK_ALL, appctrl_oad_notify,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x800082ff, UPMASK_ALL, appctrl_oad_notify,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x80010300, UPMASK_ALL, appctrl_oad_notify,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x80010400, UPMASK_ALL, appctrl_oad_notify,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x80010500, UPMASK_ALL, appctrl_oad_notify,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x80020300, UPMASK_ALL, appctrl_oad_notify,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x80027fff, UPMASK_ALL, appctrl_oad_notify,  NULL,           NULL},
    //0x810306ff 0x810406ff 0x810506ff 0x810606ff 0x810706ff 0x810806ff
    {UPDATE_ALL, UPDATE_ALL, 0x810306ff, 0xfff0ffff, appctrl_oad_notify,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x81037fff, UPMASK_ALL, appctrl_oad_notify,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x81067fff, UPMASK_ALL, appctrl_oad_notify,  NULL,           NULL},

    //0x2301 0x2302 0x2303 0x2304 0x2305 0x2306 0x2307 0x2308  
    {UPDATE_ALL, UPDATE_ALL, 0x23010200, 0xfff0ffff, appctrl_oad_notify,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x23010300, 0xfff0ffff, appctrl_oad_notify,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x23010500, 0xfff0ffff, appctrl_oad_notify,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x23010900, 0xfff0ffff, appctrl_oad_notify,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x23010e00, 0xfff0ffff, appctrl_oad_notify,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x23010f00, 0xfff0ffff, appctrl_oad_notify,  NULL,           NULL},

    {UPDATE_ALL, UPDATE_ALL, 0x40180200, UPMASK_ALL, appctrl_oad_notify,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0xf2060400, UPMASK_ALL, DB_Update_0xF2060400,NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0xf2050200, UPMASK_ALL, appctrl_oad_notify,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0xF2090C00, UPMASK_ALL, appdb_get_nodeversion_list,NULL,     nodeversion_list_lock_init},
#ifdef AREA_HUNAN
    {UPDATE_ALL, UPDATE_ALL, 0x28000200, UPMASK_ALL, get_switch_state, NULL,       NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x26000200, UPMASK_ALL, get_temperature, NULL,       NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x2B010200, UPMASK_ALL, get_smoke_state, NULL,       NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x2B020200, UPMASK_ALL, get_water_state, NULL,       NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x33610600, UPMASK_ALL, get_temperature_param, NULL,       NULL},
#endif
#ifdef AREA_FUJIAN
    {UPDATE_ALL, UPDATE_ALL, 0x6E110200, UPMASK_ALL, appdb_get_task_list_fj, NULL,        task_list_lock_init_fj}, 
    {UPDATE_ALL, UPDATE_ALL, 0x6E170200, UPMASK_ALL, appdb_get_plan_report_list_fj, NULL, plan_report_list_lock_init_fj},
    {UPDATE_ALL, UPDATE_ALL, 0xF0020400, UPMASK_ALL, app_server_info_read, NULL,          app_ftp_info_lock_init},
    {UPDATE_ALL, UPDATE_ALL, 0xF0020700, UPMASK_ALL, app_ftp_info_read,    NULL,          NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x6E330200, UPMASK_ALL, rpt_fj_evt_plan_update,NULL,         NULL},
#endif
};
    
uint16 g_dbUpdateNum = sizeof(g_dbUpdateInfo)/sizeof(DB_UPDATE_T);

/*
 * 数据中心更新定制 针对没有主DI、只有一个次数据DI的类型
 * 数据类型标识 DATA_CLASS_E
 * 数据定制     TRUE-普通类型; FALSE-记录类型
 * 筛选数据
*/
DB_CUSTOM_T g_DBCustom[] = 
{/* 数据类型,  数据定制,   筛选数据 */
    {CLASS_DATA_COLLEC, TRUE, 0xFF100A00},
    {CLASS_DATA_COLLEC, TRUE, 0xFF100B00},
    {CLASS_DATA_COLLEC, TRUE, 0xFF100C00}, 
    {CLASS_DATA_COLLEC, TRUE, 0xFF100D00},
};
    
uint16 g_dbcustomNum = sizeof(g_DBCustom)/sizeof(DB_CUSTOM_T);

extern void pulse_normal_init();
extern void pulse_set_normalflag(uint8 logicid, uint16 classTag, uint8 flag);
extern void pulse_arg_init();
extern void pulse_set_argflag(uint8 logicid, uint16 classTag, uint8 flag);
extern void ctrl_data_init();
extern void ctrl_set_dataflag(uint8 logicid, uint16 classTag, uint8 flag);
extern void ctrl_arg_init();
extern void ctrl_set_argflag(uint8 logicid, uint16 classTag, uint8 flag);

/**********************************************************************
* @name      : DB_INIT_INFO
* @brief     ：初始化事件
* @param[in] ：int clientid    db客户端
               uint8 initType  初始化类型
               uint8 logicId   逻辑设备号
               uint16 classTag 数据类型标记
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-03-6
* @Update    :
**********************************************************************/
extern void app_datainit_seek_meter(uint8 logicid,uint16 classtag);
extern void app_datainit_encryption(uint8 logicid,uint16 classtag);
int DB_INIT_INFO(int clientid, uint8 initType, uint8 logicId, uint16 classTag)
{
    pulse_set_normalflag(logicId, classTag, TRUE);
    ctrl_set_dataflag(logicId, classTag, TRUE);
    app_datainit_seek_meter(logicId, classTag);
    app_datainit_encryption(logicId, classTag);
    #ifdef AREA_ZHEJIANG
    app_datainit_cbir_result(logicId, classTag);
    #endif
    return evt_init_event_deal(clientid, initType, logicId, classTag);
}

//#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)|| (defined AREA_JIANGSU)
//void taskmng_perception_tmn_info_load(DB_CLIENT clientid)
//{
//    int   ret;
//    uint32 Len;
//    uint16 i;
//
//    NOMAL_OAD_T NormalOad;
//    NormalOad.logicId = 0;
//    NormalOad.oad.value = 0xFF110E00;
//    NormalOad.infoNum = 1;
//    OOP_PERCEPTION_TERMINAL_INFO_T terminalinfo;
//    uint8 num=0;
//    uint8 PerceptionTerminalInfoNum=0;
//
//    memset((uint8*)&perceptionterminal,0x00,sizeof(OOP_PERCEPTION_TERMINAL_LIST_T));
//
//    for(i=0;i<96;i++)
//    {
//        NormalOad.infoNum = i+1;
//        ret = db_read_nomal(clientid,&NormalOad,sizeof(terminalinfo),(uint8*)&terminalinfo,&Len);
//        if(ret!=0 || Len != sizeof(OOP_PERCEPTION_TERMINAL_INFO_T))
//        {
//            PRTL_FMT_DEBUG("ret %d, Len %d\n",ret,Len);
//            continue;
//        }
//        memcpy((uint8*)&perceptionterminal.terminalinfo[i],(uint8*)&terminalinfo,sizeof(terminalinfo));
//        PerceptionTerminalInfoNum++;
//    }
//    PRTL_FMT_DEBUG("PerceptionTerminalInfoNum %d\n",PerceptionTerminalInfoNum);
//    if(PerceptionTerminalInfoNum<1)
//    {
//        return;
//    }
//    perceptionterminal.num = PerceptionTerminalInfoNum;
//    num = perceptionterminal.terminalinfo[PerceptionTerminalInfoNum-1].meterlist.num;
//    if(num>=1)
//    {
//        perceptionterminal.totalpn = perceptionterminal.terminalinfo[PerceptionTerminalInfoNum-1].meterlist.meterinfo[num-1].pn;
//        perceptionterminal.totalpn++;
//    }
//     PRTL_FMT_DEBUG("perceptionterminal.num %d, perceptionterminal.totalpn %d\n",
//                            perceptionterminal.num, perceptionterminal.totalpn);
//    return;
//
//}
//#endif

/**********************************************************************
* @name      : DB_INIT_INIT
* @brief     ：初始化事件初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-05-11
* @Update    :
**********************************************************************/
void DB_INIT_INIT()
{
    pulse_normal_init();
    ctrl_data_init();
}

/**********************************************************************
* @name      : DB_Default_INFO
* @brief     ：初始化事件 恢复出厂设置
* @param[in] ：int clientid    db客户端
               uint8 initType  初始化类型
               uint8 logicId   逻辑设备号
               uint16 classTag 数据类型标记
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-03-6
* @Update    :
**********************************************************************/
int DB_Default_INFO(int clientid, uint8 initType, uint8 logicId, uint16 classTag)
{
    pulse_set_argflag(logicId, CLASS_DATA_INIT, TRUE);
    pulse_set_normalflag(logicId, CLASS_DATA_NORMAL, TRUE);

    ctrl_set_argflag(logicId, CLASS_DATA_INIT, TRUE);
    ctrl_set_dataflag(logicId, CLASS_DATA_NORMAL, TRUE);
    return evt_init_event_deal(clientid, initType, logicId, classTag);
}

/**********************************************************************
* @name      : DB_Default_INIT
* @brief     ：恢复出厂设置初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-05-11
* @Update    :
**********************************************************************/
void DB_Default_INIT()
{
    pulse_arg_init();
    ctrl_arg_init();
}

/* 
 * 数据中心初始化事件
 * 逻辑设备号为0xff, 标识通配
 * 数据类标识为0xffff, 标识通配
 * 需要关注的更新在下面数组中添加
*/
DBINIT_EVENT_T g_dbInitInfo[] = 
{/*  初始化类型, 逻辑设备号, 数据类标识, 回调函数,           初始化函数 */
    {0,          UPINIT_ALL, UPDATE_ALL, DB_INIT_INFO,       DB_INIT_INIT},
    {1,          UPINIT_ALL, UPDATE_ALL, DB_Default_INFO,    DB_Default_INIT},
};

uint16 g_dbInitNum = sizeof(g_dbInitInfo)/sizeof(DBINIT_EVENT_T);

DBUPDATE_LIST_T g_dbUpdateList;

/**********************************************************************
* @name      : db_updata_init
* @brief     ：数据更新初始化 主要是锁
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
void db_updata_init()
{
    uint16 i;

    pthread_mutex_init(&g_dbUpdateList.flock, NULL);
    g_dbUpdateList.head = NULL;
    g_dbUpdateList.tail = NULL;

    for(i = 0; i < g_dbUpdateNum; i++)
    {
        if(g_dbUpdateInfo[i].initc)
        {
            g_dbUpdateInfo[i].initc();
        }
    }

    for(i = 0; i < g_dbInitNum; i++)
    {
        if(g_dbInitInfo[i].initc)
        {
            g_dbInitInfo[i].initc();
        }
    }
}

/**********************************************************************
* @name      : db_list_add
* @brief     ：增加数据中心更新列表
* @param[in] ：uint8 type    类型 0,1
               void *pMsg    消息 DB_UPDATE_MSG_T 或 DBINIT_EVENT_MSG_T
* @param[out]：
* @return    ：0-成功/错误
* @Create    : 王津剑
* @Date      ：2020-9-10
* @Update    :
**********************************************************************/
int db_list_add(uint8 type, void *pMsg)
{
    DBUPDATE_NODE_T *pNode;

    if(type > 1 || NULL == pMsg)
    {
        return -1;
    }

    pNode = (DBUPDATE_NODE_T *)malloc(sizeof(DBUPDATE_NODE_T));
    if(NULL == pNode)
    {
        return -2;
    }
    memset(pNode, 0, sizeof(DBUPDATE_NODE_T));

    pNode->type = type;
    if(0 == type)
    {
        memcpy(&(pNode->msg), pMsg, sizeof(DB_UPDATE_MSG_T));
    }
    else if(1 == type)
    {
        memcpy(&(pNode->msg), pMsg, sizeof(DBINIT_EVENT_MSG_T));
    }
    
    /* 添加到队列中 */
    pthread_mutex_lock(&g_dbUpdateList.flock);
    if(NULL == g_dbUpdateList.head)
    {
        g_dbUpdateList.head = pNode;
    }
    if(NULL != g_dbUpdateList.tail)
    {
        g_dbUpdateList.tail->pNext = pNode;
    }
    g_dbUpdateList.tail = pNode;

    pthread_mutex_unlock(&g_dbUpdateList.flock);
    return 0;
}

/**********************************************************************
* @name      : db_list_get
* @brief     ：获取数据中心更新列表
* @param[in] ：
* @param[out]：DBUPDATE_NODE_T *pNode   消息节点
* @return    ：0-成功/错误
* @Create    : 王津剑
* @Date      ：2020-9-10
* @Update    :
**********************************************************************/
int db_list_get(DBUPDATE_NODE_T *pNode)
{
    if(NULL == pNode)
    {
        return -1;
    }

    pthread_mutex_lock(&g_dbUpdateList.flock);
    if(NULL == g_dbUpdateList.head)
    {
        pthread_mutex_unlock(&g_dbUpdateList.flock);
        return -2;
    }

    memcpy(pNode, g_dbUpdateList.head, sizeof(DBUPDATE_NODE_T));

    free(g_dbUpdateList.head);
    g_dbUpdateList.head = pNode->pNext;
    if(NULL == g_dbUpdateList.head)
    {
        g_dbUpdateList.tail = NULL;
    }
    pthread_mutex_unlock(&g_dbUpdateList.flock);
    return 0;
}


/**********************************************************************
* @name      : db_update_entry
* @brief     ：启动更新数据消息任务
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-31
* @Update    :
**********************************************************************/
void *db_update_entry(void *pArg)
{
    int              ret = 0;
    int              recvret = 0;
    uint16           i;
    DB_CLIENT        clientid = 0;
    DBUPDATE_NODE_T     tMsgNode;
    DB_UPDATE_MSG_T    *pUpdateMsg = NULL;
    DBINIT_EVENT_MSG_T *pInitMsg = NULL;
    uint16           index = 0;

    clientid = db_client_create(DB_YC_AGENT, DB_CLIENT_UPDATE);
    if(clientid <= 0)
    {
        printf("Main update db db_client_create error!\n");
        usleep(100);
        exit(0); //进程退出
    }
//    #if (defined AREA_JIANGXI) || (defined AREA_ANHUI)|| (defined AREA_JIANGSU)
//    taskmng_perception_tmn_info_load(clientid);
//    #endif
    memset(&tMsgNode, 0, sizeof(DBUPDATE_NODE_T));
    /* 循环收消息 */
    for(;;)
    {
        recvret = db_list_get(&tMsgNode);
        if(0 != recvret)
        {
            usleep(10000);
            continue;
        }

        if(0 == tMsgNode.type)    /* 数据更新 */
        {
            pUpdateMsg = &(tMsgNode.msg.tUpdate);
            if(TRUE == pUpdateMsg->bRecord)
            {
                /* 记录型数据更新 */
                index = pUpdateMsg->index[0];
                if(index < g_dbUpdateNum && NULL != g_dbUpdateInfo[index].rfunc)
                {
                    ret = g_dbUpdateInfo[index].rfunc(clientid, pUpdateMsg->logicId, pUpdateMsg->infoNum, 
                                                      pUpdateMsg->dataflag, pUpdateMsg->slavenum, pUpdateMsg->slavedata);
                    YCDB_FMT_TRACE("Update record 0x%08x, di 0x%08x, ret %d, logic %d, infonum %d\n", 
                                   pUpdateMsg->dataflag, g_dbUpdateInfo[index].dataflag, ret, pUpdateMsg->logicId, pUpdateMsg->infoNum);
                }
            }
            else
            {
                /* 普通型数据更新 */
                for(i = 0; i < pUpdateMsg->slavenum; i++)
                {
                    index = pUpdateMsg->index[i];
                    if(index < g_dbUpdateNum && NULL != g_dbUpdateInfo[index].func)
                    {
                        ret = g_dbUpdateInfo[index].func(clientid, pUpdateMsg->logicId, pUpdateMsg->infoNum, 
                                                         pUpdateMsg->slavedata[i]);
                        YCDB_FMT_TRACE("Update normal 0x%08x, di 0x%08x, ret %d, logic %d, infonum %d\n", 
                                       pUpdateMsg->slavedata[i], g_dbUpdateInfo[index].dataflag, ret, pUpdateMsg->logicId, pUpdateMsg->infoNum);
                    }
                }
            }
        }
        else if(1 == tMsgNode.type)  /* 初始化 */
        {
            pInitMsg = &(tMsgNode.msg.tEvent);
            for(i = 0; i < g_dbInitNum; i++)
            {
                if(pInitMsg->initType == g_dbInitInfo[i].initType
                   && (pInitMsg->logicId == g_dbInitInfo[i].logicId || UPINIT_ALL == g_dbInitInfo[i].logicId)
                   && (pInitMsg->classTag == g_dbInitInfo[i].classTag || UPCLASS_ALL == g_dbInitInfo[i].classTag))
                {
                    if(g_dbInitInfo[i].func)
                    {
                        ret = g_dbInitInfo[i].func(clientid, pInitMsg->initType, pInitMsg->logicId, 
                                                   pInitMsg->classTag);
                        YCDB_FMT_TRACE("INIT db ligic %d, ret %d, type %d, class %d\n", 
                                        pInitMsg->logicId, ret, pInitMsg->initType, pInitMsg->classTag);
                    }
                    break;
                }
            }

        }

        memset(&tMsgNode, 0, sizeof(DBUPDATE_NODE_T));
    }
    
    db_client_close(clientid);
    return 0;
}

char     g_updatastr[1024] = {0};
/**********************************************************************
* @name      : db_update_msg_proc
* @brief     ：db操作消息处理
* @param[in] ：MESSAGE_INFO_T *pMsg 消息结构体
* @param[out]：
* @return    ：0-成功/错误码(返回DB_AGENT_UNKONW_MSG, 由其他消息函数处理)
* @Create    : 王津剑
* @Date      ：2019-12-30
* @Update    :
**********************************************************************/
int db_update_msg_proc(MESSAGE_INFO_T *pMsg)
{
    int                ret = 0;
    DB_UPDATE_MSG_T    tUpdateMsg;
    DBINIT_EVENT_MSG_T tInitMsg = {0};
    uint8             *pTemp = NULL;
    uint16             offset = 0;
    uint16             i, j;
    uint8              bIndexOK = FALSE;

    if(NULL == pMsg)
    {
        return -1;
    }

    /* 1 先判断消息IID是否本函数处理 */
    if(MESSAGE_IID_DB != pMsg->IID)
    {
        return DB_AGENT_UNKONW_MSG;
    }

    if(DB_IOP_UPDATE_EVENT == pMsg->IOP)
    {
        pTemp = pMsg->playload;
        memset(&tUpdateMsg, 0, sizeof(DB_UPDATE_MSG_T));
        
        tUpdateMsg.logicId = *pTemp;
        pTemp++;

        MEMCPY_R(&tUpdateMsg.infoNum, pTemp, 2);
        pTemp += 2;

        MEMCPY_R(&tUpdateMsg.classtag, pTemp, 2);
        pTemp += 2;

        tUpdateMsg.bOADData = *pTemp;
        pTemp++;
        if(0 == tUpdateMsg.bOADData)
        {
            YCDB_FMT_TRACE("DB Update no data, logicId %d, infoNum %d, classtag %d\n", tUpdateMsg.logicId,
                           tUpdateMsg.infoNum, tUpdateMsg.classtag);
            return 0;
        }
        
        tUpdateMsg.bOptional = *pTemp;
        pTemp++;

        /* 逻辑设备7的属于配网，直接过滤 */
        if (tUpdateMsg.logicId == 7)
        {
            YCDB_FMT_TRACE("DB Update error msg logicId %d\n", tUpdateMsg.logicId);
            return -1;
        }

        if(1 == tUpdateMsg.bOptional)
        {
            tUpdateMsg.dataflag = *(uint32 *)pTemp;
            pTemp += 4;
            tUpdateMsg.dataflag = ntohl(tUpdateMsg.dataflag);
        }
        else if(tUpdateMsg.bOptional > 1)
        {
            YCDB_FMT_TRACE("DB Update error msg %d %d, optional %d\n", tUpdateMsg.logicId, tUpdateMsg.infoNum, tUpdateMsg.bOptional);
            return -1;
        }
        
        tUpdateMsg.slavenum = MIN(*pTemp, DB_UPDATE_MAX_SLAVE);
        pTemp++;
        if(0 == tUpdateMsg.slavenum)
        {
            YCDB_FMT_TRACE("DB Update error msg %d %d master 0x%x, slavenum is 0\n", 
                           tUpdateMsg.logicId, tUpdateMsg.infoNum, tUpdateMsg.dataflag);
            return -1;
        }
        
        offset = sprintf(g_updatastr, "DB Update %d %d class %d, master 0x%x, slave:", tUpdateMsg.logicId, tUpdateMsg.infoNum, 
                         tUpdateMsg.classtag, tUpdateMsg.dataflag);
        for(i = 0; i < tUpdateMsg.slavenum; i++)
        {
            tUpdateMsg.slavedata[i] = *(uint32*)pTemp;
            pTemp += 4;
            tUpdateMsg.slavedata[i] = ntohl(tUpdateMsg.slavedata[i]);
            tUpdateMsg.index[i] = DB_DEFAULT_INDEX;

            offset += sprintf(g_updatastr + offset, " 0x%x", tUpdateMsg.slavedata[i]);
        }
        
        /* 判断是否记录数据 */
        if(1 == tUpdateMsg.bOptional || CLASS_DATA_EVENT == tUpdateMsg.classtag || CLASS_DATA_COLLEC == tUpdateMsg.classtag)
        {
            tUpdateMsg.bRecord = TRUE;
        }
        else
        {
            tUpdateMsg.bRecord = FALSE;
        }
        /* 定制化改变 */
        if(0 == tUpdateMsg.bOptional && 1 == tUpdateMsg.slavenum)
        {
            for(i = 0; i < g_dbcustomNum; i++)
            {
                if(g_DBCustom[i].classTag == tUpdateMsg.classtag && g_DBCustom[i].dataflag == tUpdateMsg.slavedata[0])
                {
                    tUpdateMsg.bRecord = ((TRUE == g_DBCustom[i].bNormal) ? FALSE : TRUE);
                    break;
                }
            }
        }
        
        YCDB_FMT_TRACE("%s %s\n", TRUE == tUpdateMsg.bRecord ? "Record" : "Normal", g_updatastr);
        memset(g_updatastr, 0, offset);
        if(TRUE == tUpdateMsg.bRecord)
        {
            /* 记录型获取index */
            for(i = 0; i < g_dbUpdateNum; i++)
            {
                if(NULL != g_dbUpdateInfo[i].rfunc &&
                   (tUpdateMsg.dataflag & g_dbUpdateInfo[i].maskInfo) == (g_dbUpdateInfo[i].dataflag & g_dbUpdateInfo[i].maskInfo)&&
                   (UPDATE_ALL == g_dbUpdateInfo[i].logicId || tUpdateMsg.logicId == g_dbUpdateInfo[i].logicId) &&
                   (UPDATE_ALL == g_dbUpdateInfo[i].infoNum || tUpdateMsg.infoNum == g_dbUpdateInfo[i].infoNum))
                {
                    tUpdateMsg.index[0] = i;
                    bIndexOK = TRUE;
                    break;
                }
            }
        }
        else
        {
            for(j = 0; j < tUpdateMsg.slavenum; j++)
            {
                for(i = 0; i < g_dbUpdateNum; i++)
                {
                    if(NULL != g_dbUpdateInfo[i].func &&
                       (tUpdateMsg.slavedata[j] & g_dbUpdateInfo[i].maskInfo) == (g_dbUpdateInfo[i].dataflag & g_dbUpdateInfo[i].maskInfo) && 
                       (UPDATE_ALL == g_dbUpdateInfo[i].logicId || tUpdateMsg.logicId == g_dbUpdateInfo[i].logicId) &&
                       (UPDATE_ALL == g_dbUpdateInfo[i].infoNum || tUpdateMsg.infoNum == g_dbUpdateInfo[i].infoNum))
                    {
                        tUpdateMsg.index[j] = i;
                        
                        bIndexOK = TRUE;
                        break;
                    }
                }
            }
        }
        if(TRUE == bIndexOK)
        {
            /* 自己发给自己 */
            ret = db_list_add(0, &tUpdateMsg);
            if(0 != ret)
            {
                YCMSG_FMT_DEBUG("Update add failed, db 0x%x, logic %d, infonum %d\n", 
                                tUpdateMsg.dataflag, tUpdateMsg.logicId, tUpdateMsg.infoNum);
            }
            else
            {
                YCDB_FMT_TRACE("Update add, db 0x%x, logic %d, infonum %d\n", 
                                tUpdateMsg.dataflag, tUpdateMsg.logicId, tUpdateMsg.infoNum);
            }
        }
    }
    else if(DB_IOP_INIT_EVENT == pMsg->IOP)
    {
        pTemp = pMsg->playload;
        tInitMsg.initType = pTemp[offset++];
        if(0 == tInitMsg.initType)
        {
            tInitMsg.logicId = pTemp[offset++];
            if(1 == pTemp[offset++])
            {
                tInitMsg.classTag = *(uint16 *)(pTemp + offset);
                tInitMsg.classTag = ntohs(tInitMsg.classTag);
            }
        }
        else if(1 == tInitMsg.initType)
        {
            tInitMsg.logicId = pTemp[offset++];
        }
        else
        {
            YCDB_FMT_TRACE("Unknow dbinit msg type %d\n", tInitMsg.initType);
            return ret;
        }
        YCDB_FMT_TRACE("DB Init msg type %d, logic %d, class %d\n", tInitMsg.initType, tInitMsg.logicId, 
                        tInitMsg.classTag);

        /* 自己发给自己 */
        ret = db_list_add(1, &tInitMsg);
        if(0 != ret)
        {
            YCMSG_FMT_DEBUG("DB init add failed, type %d, logic %d, infonum %d\n",
                            tInitMsg.initType, tInitMsg.logicId, tInitMsg.initType);
        }
    }
    else
    {
        return DB_AGENT_UNKONW_MSG;
    }
    return ret;
}

