/*
*********************************************************************
* @filename: appctrl_module.h
* @brief   : ��ȫ���㷨ģ��
* @describe: 
* @history : 
*********************************************************************
*/

#ifndef __APP_CTRL_MODULE_H
#define __APP_CTRL_MODULE_H

#include "task_msg.h"
#include "appctrl_data.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CTRL_DEV_NUM_MAX      128
#define CTRL_DEV_NUM_MAX_PORT (CTRL_DEV_NUM_MAX/CTRL_PORT_NUM)

//���ܻ�ӿ���ģ��Ķ˿�������δ����ͬ��Ʒ���б仯
#define CTRL_PORT_NUM    2

//����ʧ��������
#define FAIL_NUM_MAX     3 

//���䳬ʱʱ��
#define CTRL_TIME_OUT    5 

//ң��״̬
typedef struct tagCTRL_YX_STATE
{
    uint32               num;
    uint32               sec[CTRL_DEV_NUM_MAX_PORT]; //�ϴθ���ʱ��
    OOP_TSA_T            tsa[CTRL_DEV_NUM_MAX_PORT];
    OOP_SWITCHIN_STATE_T  st[CTRL_DEV_NUM_MAX_PORT];
    OOP_SWITCHIN_FLAG_T   fg[CTRL_DEV_NUM_MAX_PORT];
}CTRL_YX_STATE_T;

typedef struct tagCTRL_YX_STATE_LIST
{
    pthread_mutex_t      f_lock;
    CTRL_YX_STATE_T      yxStat[CTRL_PORT_NUM];
}CTRL_YX_STATE_LIST_T;

#if 0
//ң������
typedef struct tagCTRL_YX_FLAG
{
    pthread_mutex_t      f_lock;
    uint32               num;
    BOOL                 isValid[CTRL_DEV_NUM_MAX];
    OOP_TSA_T            tsa[CTRL_DEV_NUM_MAX];
    OOP_SWITCHIN_FLAG_T  fg[CTRL_DEV_NUM_MAX];
}CTRL_YX_FLAG_T;
#endif

typedef enum
{
    IDLE_CTRL_STATE = 0,
    SEND_YX_STATE,
    RECV_YX_STATE,
    SEND_CT_STATE,
    RECV_CT_STATE,
    SEND_CN_STATE,
    RECV_CN_STATE,
}CTRL_STATE_E;

typedef enum
{
    CNT_IDLE = 0,
    CNT_GET_ID = 1,
    CNT_GET_IK,
    CNT_SET_ID_IK,
    CNT_ACT_INIT,
    CNT_GET_KEY,
    CNT_UPDATE,
    CNT_BEGIN_NEG,
    CNT_END_NEG,
    CNT_SUCCESS,
}CTRL_CNT_E;

/* ���Ӵ��� */
typedef struct tagCTRL_CNT
{
    OOP_TSA_T    lastTsa;     //��һ�����ӵ�ģ���ַ������л���ģ�飬��Ҫ��������
    OOP_TSA_T    curTsa;      //Ҫ�������ӵ�ģ��
    CTRL_CNT_E   cntState;    //Э��״̬
    uint32       idlen;       //Э���豸ID����
    uint8        devid[64];   //Э���豸ID
    uint32       iklen;       //Э���豸IK����
    uint8        ik[64];      //Э���豸IK
    uint8        modKeyStatus;//ģ����Կ״̬
    uint32       cRanCat_len; //������չģ�������ص� 48 �ֽ��������ݳ���
    uint8        cRanCat[64]; //������չģ�������ص� 48 �ֽ���������
    OOP_OAD_U    portOAD;     //��Ҫ�������Ķ˿ں�
    uint8        port;        //��Ҫ�������Ķ˿�����
    uint8        baud;        //��Ҫ�������Ĳ�����
    uint32       apdulen;     //��Ҫ�������ĳ���
    uint8       *apdu;        //��Ҫ��������
}CTRL_CNT_T;

/* �˿�״̬ */
typedef struct tagCTRL_PORT_STATE
{
    uint32       second;      //ϵͳ��������
    uint32       pn;          //����ģ������
    CTRL_STATE_E state;       //�˿�״̬
    uint8        failCnt;     //ʧ�ܴ���
}CTRL_PORT_STATE_T;

/* �豸�߳����� */
typedef struct tagCTRLDevHandle
{
    uint8             *pRunFlag;
    CTRL_QUEUE_T      *pApduSend;            //����
    CTRL_QUEUE_T      *pApduRecv;            //����
    CTRL_PORT_STATE_T *pState;
    CTRL_CNT_T        *connect;              //�������Ӵ���
}CTRL_DEV_H_T;

/* �˿����� */
typedef struct tagCTRL_PORT_CFG
{
    uint16       port;     //���
    OOP_OAD_U    portOAD;  //�˿ں�
}CTRL_PORT_CFG_T;

/* �������ݻ���ṹ*/
typedef struct tagCTRL_METER_LIST
{
    pthread_mutex_t f_lock;                                        //�� ���̷߳���
    uint16          Num[CTRL_PORT_NUM];                            //����
    OOP_METER_T     oopMeter[CTRL_PORT_NUM][CTRL_DEV_NUM_MAX_PORT];//ÿ���˿ڵĵ���
}CTRL_METER_LIST_T;

//�˿ڴ���״̬
extern CTRL_PORT_STATE_T gState[CTRL_PORT_NUM];

//����Э����Ϣ
extern CTRL_CNT_T gConnect;

//��Ҫ�������
extern CTRL_QUEUE_T  g_send;
extern CTRL_QUEUE_T  g_recv;

extern void app_module_meter_init(void);
extern BOOL app_meter_is_module(OOP_METER_T *oopMeter);
extern int app_module_meter_update(void);
extern int app_module_ctrl_msg_proc(CTRL_DEV_H_T *handle, TASK_MSG_t *pMsgInfo);
extern int app_module_yxstate_get(OOP_TSA_T tsa, OOP_SWITCHIN_STATE_T *st, OOP_SWITCHIN_FLAG_T *fg);

#ifdef __cplusplus
}
#endif

#endif

