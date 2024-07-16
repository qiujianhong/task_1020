/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��·���ҵ��
* @date��    2020-6-30
* @history�� 
* @User deatil:

*********************************************************************
*/
#ifndef __USB_UPG_H
#define __USB_UPG_H
    
#ifdef __cplusplus
        extern "C" {
#endif

#define USB_UPDATE_FILE "/data/app/usbManager/copr_update_file"

#define USB_UPPACK_SIZE  (196) //���������ֻ�ܴ���196�ֽ� 

/** @brief �ļ��ֿ鴫��״̬ */
typedef struct
{
    uint16 nBlockNum;       //�ܿ���
    uint8  packStatus[64];   //������״̬
} USB_FILE_STATUS_T;

/** @brief �ļ��ֿ鴫����Ϣ */
typedef struct
{
    uint8  fileMark;   //�ļ���ʶ    
    uint32 nBlockNum;  //�ܿ���
    uint16 nEndLengh;  //���һ�γ���
    uint32 nBlockId;   //��ǰ�����
    uint16 nLengh;     //��ǰ�鳤��
    uint8 *fileData;   //��ǰ������ָ��
} USB_FILE_TRANS_T;

typedef enum
{
    USB_UPDATE_IDLE = 0,
    USB_UPDATE_RUNING = 1,
    USB_UPDATE_MAINTAIN = 2,
}USB_UPDATE_STATUS_E;

typedef enum
{
    UART_PARITY_NONE     = 0,
    UART_PARITY_ODD,       //��У��
    UART_PARITY_EVEN,      //żУ��
    UART_PARITY_SPACE,    
}USB_UART_PARITY_E;

/*���ڲ���*/
typedef struct tag_USB_UART_PARAM_T
{
    uint32        baudRate;      /** < ������ */ 
    uint8         dataBits;      /** < ����λ */
    uint8         stopBits;      /** < ֹͣλ */
    uint8         parity;        /** < У��λ */    
    uint8         flowCtrl;      /** < ����λ */    
}USB_UART_PARAM_T;

/*���ڲ������*/
typedef struct tag_USB_UART_VDEV_T
{
    char            *desc;          /** < �豸������   */    
    USB_UART_PARAM_T param;         /** < ���ڲ���   */
    uint16           frametimeout;  /** < ֡��ʱ   */
    uint16           bytetimeout;   /** < �ֽڳ�ʱ   */    
    int              fd;            /** < ���ھ��(Ҫ��ʼ��Ϊ-1�����ڴ򿪺�����) */
    uint32           sendCnt;       /** < ���ڷ��ͼ��� */
    uint32           recvCnt;       /** < ���ڽ��ռ��� */ 
    struct timespec  sendTm;        /** < ���ڷ���ʱ�� */
    struct timespec  recvTm;        /** < ���ڽ���ʱ�� */
}USB_UART_VDEV_T;

/* ��·Ѳ��������������           */
typedef struct tagUsbUpdateData
{
    USB_UPDATE_STATUS_E status;     //����״̬
    USB_FILE_TRANS_T   *fileInfo;   //�ļ�������Ϣ
    int                fd;         //�ļ����
    USB_UART_VDEV_T    *pUart;      //���ھ��
}USB_UPDATE_DATA_T;

#ifdef __cplusplus
}
#endif

#endif


