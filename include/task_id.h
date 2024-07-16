/*
*********************************************************************
* @filename: task_id.h
* @brief   : ����IPC UDPͨ�ŷ�ʽ task id ����
* @describe: 32λ ǰ20λ��ģ��id  ��12λ��ģ����task���
             ǰ16λΪ0�����ڲ�ģ��ʹ��
* @history: 
*    auther      date          describe
*    ����      2019-12-23    create
*********************************************************************
*/
#ifndef    __TASK_ID_H
#define __TASK_ID_H


/*-------------------------------------db������ʱ-----------------------------------------*/
/* �ò�698 db���� */
#define DB_YC_START          0x1000
#define DB_YC_AGENT          (DB_YC_START + 1)  //�ò�agent
#define DB_GUI_AGENT         (DB_YC_START + 2)  //��ʾagent

#define YC_DB_CLIENT_START   (DB_YC_START + 0x100)
#define DB_CLIENT_APPMAIN    (YC_DB_CLIENT_START + 1)
#define DB_CLIENT_COMM       (YC_DB_CLIENT_START + 2)
#define DB_CLIENT_PULSE      (YC_DB_CLIENT_START + 3)
#define DB_CLIENT_ACPORT     (YC_DB_CLIENT_START + 4)
#define DB_CLIENT_STATIS     (YC_DB_CLIENT_START + 5)
#define DB_CLIENT_UPDATE     (YC_DB_CLIENT_START + 6)
#define DB_CLIENT_REPORT     (YC_DB_CLIENT_START + 7)
#define DB_CLIENT_EVENT      (YC_DB_CLIENT_START + 8)
#define DB_CLIENT_GUI        (YC_DB_CLIENT_START + 9)
#define DB_CLIENT_GUI_UPDATE (YC_DB_CLIENT_START + 10)
#define DB_CLIENT_CTRL       (YC_DB_CLIENT_START + 11)
#define DB_CLIENT_PUAMR_GUI  (YC_DB_CLIENT_START + 12)
#define DB_CLIENT_OTHERTIME  (YC_DB_CLIENT_START + 13)
#define DB_CLIENT_SMS0       (YC_DB_CLIENT_START + 14)
#define DB_CLIENT_SMS1       (YC_DB_CLIENT_START + 15)
#define DB_CLIENT_OUTMSG     (YC_DB_CLIENT_START + 16)
#define DB_CLIENT_LCMTRANS   (YC_DB_CLIENT_START + 17)
#define DB_CLIENT_COMM_ETH   (YC_DB_CLIENT_START + 18)
#define DB_CLIENT_COMM_PPP   (YC_DB_CLIENT_START + 19)
#define DB_CLIENT_COMM_BLE   (YC_DB_CLIENT_START + 20)
#define DB_CLIENT_COMM_RADIO (YC_DB_CLIENT_START + 21)
#define DB_CLIENT_PROCON     (YC_DB_CLIENT_START + 22)
#define DB_CLIENT_EM         (YC_DB_CLIENT_START + 23)
#define DB_CLIENT_GUI_METER  (YC_DB_CLIENT_START + 24)
#define DB_CLIENT_COMM_2     (YC_DB_CLIENT_START + 25)

#define YC_DB_CLIENT_CHANNEL (DB_YC_START + 0x200)   //�ָ�ͨ����taskid


/* ����104 db���� */
#define DB_PD_START          0x2000
#define DB_PD_AGENT          (DB_PD_START + 1)  //����agent

#define PD_DB_CLIENT_START      (DB_PD_START + 0x100)
#define DB_PD_CLIENT_UPDATE     (PD_DB_CLIENT_START + 1)
#define DB_PD_CLIENT_APP104     (PD_DB_CLIENT_START + 2)
#define DB_PD_CLIENT_COMM       (PD_DB_CLIENT_START + 3)
#define DB_PD_CLIENT_PULSE      (PD_DB_CLIENT_START + 4)
#define DB_PD_CLIENT_104FRAME   (PD_DB_CLIENT_START + 5)
#define DB_PD_CLIENT_GUI        (PD_DB_CLIENT_START + 6)
#define DB_PD_CLIENT_STAT       (PD_DB_CLIENT_START + 7) //���ͳ���߳̾��
#define DB_PD_CLIENT_EVENT      (PD_DB_CLIENT_START + 8) //�����¼�ʹ��

/* ����app db���� */
#define DB_AC_START          0x3000
#define DB_AC_AGENT          (DB_AC_START + 1)  //����app agent

#define AC_DB_CLIENT_START   (DB_AC_AGENT + 0x100)
#define DB_AC_CLIENT_SAVE    (AC_DB_CLIENT_START + 1)
#define DB_AC_CLIENT_UDP     (AC_DB_CLIENT_START + 2)
#define DB_AC_CLIENT_UPDATE  (AC_DB_CLIENT_START + 3) //�������ĸ���
#define DB_AC_CLIENT_RPT    (AC_DB_CLIENT_START + 4)


/* ��·Ѳ�� */
#define DB_LC_START          0x4000
#define DB_LC_AGENT          (DB_LC_START + 1)  //��·Ѳ��app agent
#define DB_LC_CLIENT_MONITOR (DB_LC_START + 2)
#define DB_LC_CLIENT_GUI     (DB_LC_START + 3)

//̨��״̬����
#define DB_PDS_START        0X5000
#define DB_PDS_AGENT        (DB_PDS_START+1)
#define DB_PDS_CLIENT  (DB_PDS_START+2)
#define DB_PDS_CLIENT_DBUPDATE  (DB_PDS_START+3)
#define DB_PDS_CLIENT_COMMON    (DB_PDS_START+4)
#define DB_PDS_CLIENT_LINELOSS    (DB_PDS_START+5)

//�ն���ά��
#define DB_OAM_START         0X6000
#define DB_OAM_AGENT        (DB_OAM_START+1)
#define DB_OAM_CLIENT_COMM  (DB_OAM_START+2)  //ͨ��ģ��

//�ǻ���Դ�ӿ�APP
#define DB_IF_START          0X6100
#define DB_IF_AGENT          (DB_IF_START+0)
#define DB_IF_CLIENT_REPORT  (DB_IF_START+1)  //�ϱ�
#define DB_IF_CLIENT_ETH     (DB_IF_START+2)  //ͨ������
#define DB_IF_CLIENT_SPI     (DB_IF_START+3)  //ͨ������

//�ǻ���Դ�ӿ�JSON
#define DB_JSON_START          0X6200
#define DB_JSON_AGENT          (DB_JSON_START+0)
#define DB_JSON_CLIENT_DBCHG   (DB_JSON_START+1)  //�ϱ�
#define DB_JSON_CLIENT_FRMAE0  (DB_JSON_START+2)  //ͨ������
#define DB_JSON_CLIENT_FRMAE1  (DB_JSON_START+3)  //ͨ������

//ר�为�ɿ���JSON
#define DB_LOADCTRLLER_START          0X6300
#define DB_LOADCTRLLER_AGENT          (DB_LOADCTRLLER_START+0)
#define DB_LOADCTRLLER_CTRL           (DB_LOADCTRLLER_START+1)  //����
#define DB_LOADCTRLLER_STAT           (DB_LOADCTRLLER_START+2)  //ͳ��
#define DB_LOADCTRLLER_EVENT          (DB_LOADCTRLLER_START+3)  //����
#define DB_LOADCTRLLER_GUI            (DB_LOADCTRLLER_START+4)  //��ʾ

/*-------------------------------------db�������-----------------------------------------*/



/*-------------------------------------���̷��俪ʼ-----------------------------------------*/
/* �ò�task���� */
#define YC_PROCESS_START     0x10000 * 0x1000
#define MSG_MONITOR          (YC_PROCESS_START + 1)    //����ͨ�Ź���
#define TIMER_MONITOR        (YC_PROCESS_START + 2)    //��ʱ������
#define UPDATE_DB            (YC_PROCESS_START + 3)    //�������ĸ���
#define APPMAIN_IPC          (YC_PROCESS_START + 4)    //ͨ��ģ�齻��
#define APPMAIN_MSG          (YC_PROCESS_START + 5)    //APPmain����ͨ�ų����
#define COMM_IPC             (YC_PROCESS_START + 6)    //��վͨ�Ŷ���
#define COMM_MSG             (YC_PROCESS_START + 7)    //����ͨ��
#define PULSE_MSG            (YC_PROCESS_START + 8)    //����ͨ��
#define ACPORT_MSG           (YC_PROCESS_START + 9)    //����ͨ��
#define STATIS_MSG           (YC_PROCESS_START + 10)   //����ͨ��
#define PULSE_SYNCMSG        (YC_PROCESS_START + 11)   //����ͨ��
#define COMM_SYNCMSG         (YC_PROCESS_START + 12)   //����ͨ��
#define APPMAIN_SYNCMSG      (YC_PROCESS_START + 13)   //����ͨ��
#define UART_SYNCMSG         (YC_PROCESS_START + 14)   //����ͨ��
#define COMM_TIMER           (YC_PROCESS_START + 15)    //����ͨ��
#define TRAN_MSG             (YC_PROCESS_START + 16)    //����ͨ��


/* ����104 task���� */
#define PD_PROCESS_START     0x10010 * 0x1000
#define PD_MSG_MONITOR       (PD_PROCESS_START + 1)    //����ͨ�Ź���
#define PD_UPDATE_DB         (PD_PROCESS_START + 2)    //�������ĸ���
#define PD_APP104_IPC        (PD_PROCESS_START + 3)    //��վ����ͨ��
#define PD_APP104_MSG        (PD_PROCESS_START + 4)    //APP����ͨ�ų����
#define PD_APP104_SYNC       (PD_PROCESS_START + 5)    //APP����ͬ��ͨ�ų����
#define PD_COMM_IPC          (PD_PROCESS_START + 6)    //������ͨ��
#define PD_COMM_MSG          (PD_PROCESS_START + 7)    //����ͨ��
#define PD_PULSE_MSG         (PD_PROCESS_START + 8)    //����ͨ��
#define PD_UPDATE_FILES      (PD_PROCESS_START + 9)    //�������ĵ�������
#define PD_COMM_SYNCMSG      (PD_PROCESS_START + 10)   //����ͨ��


/* GUI task ���� */
#define GUI_PROCESS_START     0x10020 * 0x1000
#define    GUI_MSG_MONITOR         (GUI_PROCESS_START + 1)
#define GUI_MSG                 (GUI_PROCESS_START + 2)
#define GUI_COMM_MSG         (GUI_PROCESS_START + 3)
#define GUI_SYNCMSG             (GUI_PROCESS_START + 4)

/* GUI task ���� */
#define LC_PROCESS_START     0x10030 * 0x1000
#define LC_MSG_PROC          (LC_PROCESS_START + 1)
#define LC_MSG_TASK          (LC_PROCESS_START + 2)
#define LC_MSG_UPDATE        (LC_PROCESS_START + 3)
#define LC_MSG_WORK          (LC_PROCESS_START + 4)


/* pdsManager���� */
#define PDS_PROCESS_START     0x10040 * 0x1000
#define PDS_MQTT_IPC        (PDS_PROCESS_START+1)
#define PDS_CALC_IPC        (PDS_PROCESS_START+2)

/*-------------------------------------���̷������-----------------------------------------*/

#endif

