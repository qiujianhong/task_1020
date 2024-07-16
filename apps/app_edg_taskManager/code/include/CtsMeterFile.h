#ifndef __CTSMETERFILE_H__
#define __CTSMETERFILE_H__

extern void taskmng_amr_info_read(void);
extern int taskmng_addr_isvalid_check(uint8 *MtrAddr,uint8 addrlen);
extern void taskmng_mtr_addr_prtl_get(uint16 Pn,uint8 logicid,uint8 *MtrAddr,uint8 *prtl);
extern void taskmng_mtr_addr_get(uint16 Pn,uint8 logicid,uint8 *MtrAddr);
extern void taskmng_mtr_prtl_get(uint16 Pn,uint8 logicid,uint8 *prtl);
extern uint8 taskmng_mtr_conntype_get(uint16 Pn,uint8 logicid);
extern OOP_TSA_T taskmng_tsa_get(uint16 Pn,uint8 logicid);
extern void taskmng_mtr_basic_info_get(uint16 Pn,uint8 logicid,OOP_METER_BASIC_T *MtrAddr);
extern uint8 taskmng_pipe_get_from_tsa(OOP_TSA_T tsa,uint8 logicid);
extern void taskmng_prtl_get_from_addr(uint8* Addr,uint8 logicid,uint8* prtl);
extern uint16 taskmng_pn_get_from_tsa(OOP_TSA_T tsa,uint8 logicid);
extern void taskmng_prtl_get_from_tsa(OOP_TSA_T tsa,uint8 logicid,uint8* prtl);
extern BOOL taskmng_meter_check_from_pipe(uint8 pipe);
extern uint8 taskmng_baud_get_from_tsa(OOP_TSA_T tsa,uint8 logicid);
extern uint8 taskmng_pipe_get_from_addr(uint8 *Addr,uint8 logicid);
extern uint16 taskmng_pn_get_from_addr(uint8* Addr,uint8 logicid);
extern BOOL taskmng_prtl_check(uint8 prtl);
extern uint8 taskmng_usertype_get_from_tsa(OOP_TSA_T tsa,uint8 logicid);
extern uint8 taskmng_usertype_get_from_no(uint16 Pn,uint8 logicid);
uint16 taskmng_addr_in_amr_info_check(uint8 Pipe, uint8 logicid, uint8 *MtrAddr, uint8 prtl, uint16 *MtrIndex);

uint16 taskmng_meter_file_init(Meter_File_T *pMeterFileAll);
void taskmng_meter_file_ergodic(Meter_File_T *pMeterFileAll);
int taskmng_meter_file_read_from_pn(uint16 infonum,OOP_METER_T *gMeterFile,uint8 logicid);
 void taskmng_meter_search_result_read();
 void taskmng_meter_file_update_check(DB_UPDATE_T *dbupdate);
 void taskmng_meter_file_change_fun(Meter_File_T *pMeterFileAll,uint8 choice,OOP_METER_T gMeterFile,uint8 logicid);
void taskmng_mtr_addr_pn_get_by_userType(uint8 userType,uint8 logicid,uint8 *MtrAddr,uint16 *pn);
BOOLEAN taskmng_check_caq_from_addr(uint8 *Addr,uint8 logicid);
uint16 taskmng_get_tsa_count();
uint16 taskmng_pn_get_from_caj_addr(uint8* Addr);
int16 taskmng_caj_addr_prtl_get(uint16 Pn,uint8 *MtrAddr,uint8 *prtl);
int16 taskmng_caj_addr_get(uint16 Pn,uint8 *MtrAddr);
int16 taskmng_caj_addr_get_from_meteraddr(uint16 *Pn,uint8 *MtrAddr,uint8* cajaddr);
uint8 taskmng_pipe_get_by_pn(uint16 Pn);
 void taskmng_mtr_prtl_get_new(uint16 Pn,uint8 logicid,uint8 *prtl);
void taskmng_mtr_addr_prtl_get_new(uint16 Pn,uint8 logicid,uint8 *MtrAddr,uint8 *prtl);
BOOL taskmng_iot_prtl_check(uint8 pipe);
uint16 taskmng_pn_get_from_tsa_new(OOP_TSA_T *tsa);
void taskmng_mtr_basic_info_get_by_tsa(OOP_TSA_T *tsa,OOP_METER_BASIC_T *MtrAddr);
OOP_METER_ANNEX_T taskmng_mtr_annex_get(uint16 Pn,uint8 logicid);
void taskmng_speed_list_get();
void taskmng_mtr_extend_info_get(uint16 Pn,uint8 logicid,OOP_METER_EXTENDED_T	*extend);
uint16 taskmng_meter_file_init_with_logicid(Meter_File_T *pMeterFileAll,uint8 logicid);


#ifdef AREA_FUJIAN
uint16 taskmng_pn_get_from_tsa_pipe(uint8 pipe,OOP_TSA_T *tsa);

#endif

#endif
