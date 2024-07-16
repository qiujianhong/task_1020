/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：    通信模块其他功能：数据中心操作
* @date：     2020-8-28
* @history： 
*********************************************************************
*/
#ifndef	__COMM_DB_H
#define __COMM_DB_H

#ifdef __cplusplus
        extern "C" {
#endif

typedef int (*CheckDBFUN)(int, void *, uint8 *);

typedef struct tagCommDBCheck
{
    CheckDBFUN   pCheckFunc;
    void        *pCfg;
    uint8       *pRestart;
}COMM_DBCHECK_T;

/* 数据中心消息处理线程句柄 */
typedef struct tagCommDBHandle
{
    int                dbclient;
    uint8              unNum;
    uint8              res[3];
    COMM_DBCHECK_T     tCheck[8];

    uint8             *pRunFlag;
}COMM_DB_HANDLE_T;


void comm_dbcheck_add(COMM_DB_HANDLE_T *ptHandle, CheckDBFUN pFunc, void *pArg, uint8 *pRestart);
void *comm_db_entry(void *pArg);


#ifdef __cplusplus
}
#endif

#endif

