/*
*********************************************************************
* @filename: dbmsg_id.h
* @brief   : 本地IPC UDP通信方式 db msg 定义
* @describe: 32位 使用 后16位是内部使用
* @history: 
*    auther      date          describe
*    王津剑      2019-12-27    create
*********************************************************************
*/
#ifndef    __DBMSG_ID_H
#define __DBMSG_ID_H


/* 数据库操作 */
#define MSG_DB_START                   0x100
#define MSG_DB_READ_NOMAL             (MSG_DB_START + 1)
#define MSG_DB_WRITE_NOMAL            (MSG_DB_START + 2)
#define MSG_DB_READ_NOMALS            (MSG_DB_START + 3)
#define MSG_DB_WRITE_NOMALS           (MSG_DB_START + 4)
#define MSG_DB_READ_RECORD_START      (MSG_DB_START + 5)
#define MSG_DB_READ_RECORD_STEP       (MSG_DB_START + 6)
#define MSG_DB_READ_RECORD_END        (MSG_DB_START + 7)
#define MSG_DB_READ_RECORD            (MSG_DB_START + 8)
#define MSG_DB_WRITE_RECORD           (MSG_DB_START + 9)
#define MSG_DB_UPDATE_RECORD          (MSG_DB_START + 10)
#define MSG_DB_GET_RECORD_NUM         (MSG_DB_START + 11)
#define MSG_DB_GET_RECORD_DEPTH       (MSG_DB_START + 12)
#define MSG_DB_CLEAR_DATA             (MSG_DB_START + 13)
#define MSG_DB_CLEAR_OAD_DATA         (MSG_DB_START + 14)
#define MSG_DB_RECOVER_DEFAULT        (MSG_DB_START + 15)


#endif

