/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� ��������
* @date�� 2019-10-25
*********************************************************************
*/

#ifndef __AC_UPDATE_H__
#define __AC_UPDATE_H__

#include "ac_redef.h"

//��������ģ�������������߼���ַ
#define TOPO_LOGIC_ADDR       197

#define UPDATE_PATH_WITH_VER  "/data/app/acMeter/upFile/"
#define UPDATE_FILE_NAME      "jc_update_file"

extern uint8 gVerBuf[8];              // ���ڽ��ɰ汾��Ϣ
extern uint8 gVerBufLast[8];          // �����ϰ潻�ɰ汾��Ϣ 
extern uint8 gDateBuf[8];             // ���ڽ��ɰ汾������Ϣ
extern uint8 gDateBufLast[8];         // �����ϰ潻�ɰ汾������Ϣ
extern uint8 gVerSBuf[8];             // ���ڽ���С�汾��Ϣ 
extern uint8 gVerSBufLast[8];         // �����ϰ潻��С�汾��Ϣ 
extern uint8 gJCUpdateFileMark;       // �����ļ���ʶȫ�ֱ��. 0x01:����ģ��; 0x02:ң��ģ��;0x03:485ģ��;0x04:����ģ��

extern int ac_update_file_close(int* Soft_fd);
extern int IsCloseInnerDelayState(int Condition, int* Cnt, int CntMax);
extern int JcUpdateDeal(int32 fd_spi, BOOL isGetVer);
extern BOOL isJCUpdateRunning(void);
extern BOOL isJCNeedGetVer(void);
extern void ClearCommRecvBuff(int32 fd);

#endif