#ifndef __FUJIAN_COMMON_H__
#define __FUJIAN_COMMON_H__
#ifdef __cplusplus
extern "C" {
#endif

extern uint8 IsSetGatherMode[PLC_NUM];
extern uint8 gIsGetCCOInfo[PLC_NUM];
extern uint16 taskmng_grop_meter_init(FUJIAN_grop_meter *pgropMeter);


void fujian_state_init ( uint8 CommPipe );
void fujian_data_init_check(uint8 pipe);
void fujian_time_manage ( uint8 CommPipe );
int fujian_task_record_file_del_check(uint8 pipe);
int fujian_time_calc(OOP_DATETIME_S_T * datetime1, OOP_TI_T * ti, OOP_DATETIME_S_T * datetime2,OOP_DATETIME_S_T * datetimeOut);
void fujian_datetime_s_log_write(uint8 pipe,char* path,OOP_DATETIME_S_T *datetime);
int fujian_time_compare(time_t time1,OOP_DATETIME_S_T *datetime);
void fujian_gather_mode_set(uint8 pipe);
void fujian_local_addr_set(uint8 pipe);
int fujian_rs485_init(uint8 pipe,OOP_COMDCB_T *comdcb);
int fujian_trans_task_buf_send(uint8 pipe,uint16 iop,uint8* sendbuf,uint16 sendlen,uint16 *msgIndex);
uint16 fujian_send_payload_pack_0100(uint8 pipe,uint8* sendbuf,uint16 sendlen,uint8 *payload);
uint16 fujian_send_payload_pack_0101(uint8 pipe,OOP_TSA_T *tsa,uint8* sendbuf,uint16 sendlen,uint8 *payload);
uint16 fujian_recv_payload_unpack_0100(uint8* payload,uint16 payloadLen,uint8 *buf);
uint16 fujian_recv_payload_unpack_0101(uint8* payload,uint16 payloadLen,uint8 *buf);
int fujian_tsa_add(OOP_TSA_T * addr, uint16 * num, OOP_TSA_T * meterList);
uint16 fujian_send_payload_pack_0012(uint8 pipe,uint8* sendbuf,uint16 sendlen,uint8 *payload);
uint16 fujian_recv_payload_unpack_0012(uint8* payload,uint16 payloadLen,uint8 *buf);
uint8 fujian_module_info_get(uint8 pipe);
uint8 fujian_amr_mode_get(uint8 pipe);
uint8 fujian_current_task_end(uint8 pipe);
int fujian_trans_task_buf_ack(uint8 pipe,uint16 iop,uint8* sendbuf,uint16 sendlen,uint16 msgIndex,uint16 msgLabel);
int fujian_rs485_send(uint8 pipe,uint8 overtime,uint8 *sendbuf,uint16 sendlen);

int fujian_rs485_recv(uint8 pipe,uint8 overtime,OOP_COMDCB_T *comdcb, uint8* pBuffer, int nBufLen);
uint8 fujian_meter_search_start(uint8 pipe,uint16 allowTime);
BOOL fujian_metersearch_result_check(uint8 pipe,OOP_SEARCHMET_RES_T searchresult,uint16 *infonum);
int taskmng_metersearch_task_deal_485(uint8 pipe, FUJIAN_TASK_RUN_T * runTask);
BOOL taskmng_smart_proxy_msg_unpack_trans_command_request(uint8 pipe,uint8* buf,uint32 len);
int fujian_relay_trans_deal ( uint8 pipe );
void fujian_task_data_event_report(uint8 pipe,OOP_TRANS_TASK_RESULT_T *transResult,uint8 result);
void fujian_task_log_event_report(uint8 pipe,char *logfile,uint16 taskid,OOP_DATETIME_S_T *storetime);
int fujian_gather_mode_get_from_ccoRouter(uint8 pipe);
int fujian_broadcast_tsa_get(uint8 reqtype,OOP_TSA_T *tsa);
 int fujian_datetime_s_compare(OOP_DATETIME_S_T *datetime1,OOP_DATETIME_S_T *datetime2);
void fujian_col_state_write(uint8 pipe,uint16 MtrIndex);
int fujian_ms_index_get(uint8 pipe,OOP_TSA_T *tsa);
void fujian_col_state_update(uint8 pipe,OOP_TSA_T *tsa,uint8 cmd);
int fujian_task_fail_addr_write(uint8 pipe,FUJIAN_TASK_RUN_T * runTask);
int fujian_task_priority_update_get(DB_CLIENT client,uint16 taskid);
void fujian_task_priority_update_deal(uint8 pipe,uint8* buf,uint16 len);
int fujian_meter_index_get(OOP_TSA_T *tsa,FUJIAN_TASK_RUN_T * runTask);
uint8 fujian_485_share_flag_get(DB_CLIENT clientid);
void fujian_collection_send_recv_update(MESSAGE_INFO_T* MsgInfo );
void fujian_cco_info_get(uint8 pipe);
FUJIAN_SPECIFIC_TASK_T *fujian_specific_task_add(FUJIAN_SPECIFIC_TASK_T *pAddInfo,FUJIAN_SPECIFIC_TASK_T *headInfo);
//void fujian_rs485_release(uint8 pipe);
void fujian_power_on_notify(MESSAGE_INFO_T* MsgInfo );
int fujian_ti_compare(OOP_TI_T* ti1,OOP_TI_T* ti2);
int fujian_task_priority_change_notify(OOP_TRANS_TASK_T *taskInfo);
void fujian_task_priority_change(uint8 pipe,uint8* buf,uint16 len);

#ifdef __cplusplus
}
#endif
#endif

