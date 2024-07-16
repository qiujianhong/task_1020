/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程主动上报
* @date：    2019-12-5
* @history： 
*********************************************************************
*/
#ifndef	__APPREPORT_H
#define __APPREPORT_H

#ifdef __cplusplus
        extern "C" {
#endif

BOOL app_report_getdb(uint8 *buff    , int sockdbfd);
int appdb_get_voltage(int sockdbfd, uint8 logicId, uint16 infoNum, uint32 value);
int appdb_get_current(int sockdbfd, uint8 logicId, uint16 infoNum, uint32 value);
int appdb_get_factor(int sockdbfd, uint8 logicId, uint16 infoNum, uint32 value);
int appdb_get_activepower(int sockdbfd, uint8 logicId, uint16 infoNum, uint32 value);
int appdb_get_reactivepower(int sockdbfd, uint8 logicId, uint16 infoNum, uint32 value);
int appdb_get_activePositiveEnergy(int sockdbfd, uint8 logicId, uint16 infoNum, uint32 value);
int appdb_get_ReactivePositiveEnergy(int sockdbfd, uint8 logicId, uint16 infoNum, uint32 value);
int appdb_get_version(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
int appdb_get_localaddr(int sockdbfd, uint8 logicId, uint16 infoNum, uint32 value);
int appdb_get_voltageAngle(int sockdbfd, uint8 logicId, uint16 infoNum, uint32 value);
int appdb_get_currentAngle(int sockdbfd, uint8 logicId, uint16 infoNum, uint32 value);


#ifdef __cplusplus
}
#endif

#endif



