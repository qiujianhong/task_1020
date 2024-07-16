/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��Ӧ�õ��Ƚ��������ϱ�
* @date��    2019-12-5
* @history�� 
*********************************************************************
*/
#ifndef	__LCMTRANS_H
#define __LCMTRANS_H


#ifdef __cplusplus
        extern "C" {
#endif

#define VF_BEGIN       0x68  //֡��ʼ�ַ�-
#define VF_END         0x16  //֡�����ַ�-

typedef enum
{
    UART_PARITY_NONE     = 0,
    UART_PARITY_ODD,       //��У��
    UART_PARITY_EVEN,      //żУ��
    UART_PARITY_SPACE,    
}UART_PARITY_E;

/*���ڲ���*/
typedef struct tag_LC_UART_PARAM_T
{
    uint32        baudRate;      /** < ������ */ 
    uint8         dataBits;      /** < ����λ */
    uint8         stopBits;      /** < ֹͣλ */
    uint8         parity;        /** < У��λ */    
    uint8         flowCtrl;      /** < ����λ */    
}LC_UART_PARAM_T;

/*���ڲ������*/
typedef struct tag_LC_UART_VDEV_T
{
    char             halname[32];   /** < HAL�豸������   */    
    char            *desc;          /** < ��ʵ�豸������   */    
    LC_UART_PARAM_T  param;         /** < ���ڲ���   */
    uint16           frametimeout;  /** < ֡��ʱ   */
    uint16           bytetimeout;   /** < �ֽڳ�ʱ   */    
    int              fd;            /** < ���ھ��(Ҫ��ʼ��Ϊ-1�����ڴ򿪺�����) */
    uint32           sendCnt;       /** < ���ڷ��ͼ��� */
    uint32           recvCnt;       /** < ���ڽ��ռ��� */ 
    struct timespec  sendTm;        /** < ���ڷ���ʱ�� */
    struct timespec  recvTm;        /** < ���ڽ���ʱ�� */  
}LC_UART_VDEV_T;

typedef struct tagLcmTransThread
{
    uint8        *pRunFlag;
    uint8         status;    //״̬
    DB_CLIENT     clientid;
    time_t        curtime;       //��ǰʱ��    
    time_t        lsttime;       //�ϴ�ʱ��
}LCMTRANS_THREAD_T;

/** @brief ��·Ѳ���ȡ״̬   */
typedef enum tag_LC_MONITOR_COL
{
    LCM_INIT  =  0, //��ʼ̬    
    LCM_ERRO  =  1, //����̬
    LCM_CONF  =  2, //����̬    
    LCM_TRAN  =  3, //͸��̬
}LC_MONITOR_COL_E;

typedef int (*MODGET)(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata);

typedef struct tag_LC_MODPARSE
{
    uint32 di;
    MODGET apduGet;
}LC_MODPARSE_T;

#define LCM_SEND_RETRY (0)

#define LCM_OAD_TAVERSION        0x40400200  //TAģ��汾��Ϣ
#define LCM_OAD_WORKSTATUS       0x40400300  //TAģ�鹤����Ϣ

#define LCM_QUEUE_DEPTH (10)
#define LCM_QUEUE_BUFER (2048)

/* ��������*/
typedef struct tag_LCMTRANS_QUEUE_T
{
    uint8   type;                         /** < ��������       */
    uint16  length;                       /** < ���ݳ���       */
    uint8   data[LCM_QUEUE_BUFER];       /** < ���ݻ����� */
}LCMTRANS_QUEUE_T; 

/* �Ƚ��ȳ����� */
typedef struct tag_LCMTRANS_FIFO_T
{
    pthread_mutex_t  lock;                    /*���ջ�����������*/
    uint32           depth;                   /** < �������*/
    uint16           head;
    uint16           tail;
    LCMTRANS_QUEUE_T queue[LCM_QUEUE_DEPTH]; /** < ��������     */
}LCMTRANS_FIFO_T;

typedef struct tagLcmEsamFrameHead
{
    uint32          msglen;  //����
    uint32          apduLen; //apud����
    uint8          *pApdu;
}LCM_ESAMFRAME_HEAD_T;

int uart_open(LC_UART_VDEV_T *pUart);
int uart_close(LC_UART_VDEV_T *pUart);
int uart_write(LC_UART_VDEV_T *pUart, uint8* buf, int len);
int uart_read(LC_UART_VDEV_T *pUart, uint8* buf, int len);

void *app_lcm_trans_work(void *pArg);
void *app_ljm_trans_work(void *pArg);

#ifdef __cplusplus
}
#endif

#endif


