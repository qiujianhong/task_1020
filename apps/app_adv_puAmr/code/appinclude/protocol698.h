/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��Ӧ�õ��Ƚ���698��Լ����ͷ�ļ�
* @date��    2019-12-4
* @history�� 
*********************************************************************
*/
#ifndef	__PROTOCOL698_H
#define __PROTOCOL698_H

#ifdef __cplusplus
        extern "C" {
#endif

#define PROTOCOL_POOL_SIZE         10
#define PROTOCOL_QUEUE_SIZE        360

#define PROTOCOL_CHANNEL_MAX_NUM     48

typedef struct tagProtocolThread
{
    uint8         threadid;  //�߳�ID
    uint8         num;       //��ͨ������
    uint16        res;
    uint8        *pRunFlag;
    uint8         channel[PROTOCOL_CHANNEL_MAX_NUM];
}PROTOCOL_THREAD_T;

#define PROTOCOL_THREAD_BIND_CHANNELS(handle, startid, endid)  \
    do{\
        uint8 iid = 0;\
        for(iid = (startid); iid <= (endid); iid++)\
        {\
            if(iid - (startid) < PROTOCOL_CHANNEL_MAX_NUM)\
            {\
                handle.num++;\
                handle.channel[iid - (startid)] = iid;\
            }\
        }\
    }while(0);

void *protocol_698_entry(void *pArg);
int protocol_698_poolinit();
void *protocol_698_pool(void *pArg);
void protocol_analyze_channel(uint8 id);


#ifdef __cplusplus
}
#endif

#endif


