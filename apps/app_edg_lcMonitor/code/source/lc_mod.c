/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��·��س�ʼ��
* @date��    2020-6-30
* @history�� 
*********************************************************************
*/

#include "baseCommon.h"
#include <arpa/inet.h>
#include "framecomm.h"
#ifndef PRODUCT_SCU
#include "hal.h"
#include "uart.h"
#endif
#include "iniparser.h"
#include "task_msg.h"
#include "mapmsg.h"
#include "mapmsg_split.h"
#include "appmsg_split.h"
#include "db_api.h"
#include "uart_api.h"

#include "lc_gui.h"
#include "lc_comm.h"
#include "lc_mqtt.h"
#include "lc_monitor.h"
#include "lc_mod.h"
#include "Encrypt.h"

int g_lcm_slot = 0; //��·Ѳ��̶���3�Ųۣ����԰汾ֱ��д��
LC_MONITOR_COL_E g_lc_colstatus = LCM_SEND;
BOOL g_mod_status_chg = 0;
void *g_pUart = NULL; /*���ھ��*/
OOP_LOOPSTAT_T g_legendStatus = {0};
OOP_LOOPSTAT_T g_lastStatus = {0};//����������ͬ״̬�ż�¼״̬�仯
uint8 g_recvRetry = 0; //�ղ������ĵ����Դ���
LC_WORK_STATUS_T g_workstatus[MOD_SAMPLE_PERIOD];
extern uint32 g_secNum;
OOP_CT_VERTIONS_T g_ctVer = {0}; //����ʾ��
LC_EVT_FIFO_T g_ctEvt = {0}; //����ʾ��
uint8 g_verNeedRetry = FALSE; //�Ƿ���Ҫ���¶��汾��

extern void *memcpy_r(void *dest, const void *src, int count);

LC_MODPARSE_T g_modParse[] = 
{
    {MAPMSG_DT_LCM_WORKSTATUS,  lc_ctworkstate_get},
    {MAPMSG_DT_LCM_VERSIONINFO, lc_versioninfo_get},
};

uint32 g_modParse_Max = sizeof(g_modParse)/sizeof(LC_MODPARSE_T);

uint8 send_cmd_7001[20]={0x68,0x01,0x00,0x00,0x00,0x00,0x00,0x68,0x03,0x08,0x34,0x33,0x33,0x3A,0x33,0x33,0x33,0x33,0x7C,0x16}; //7001
uint8 send_cmd_7002[20]={0x68,0x01,0x00,0x00,0x00,0x00,0x00,0x68,0x03,0x08,0x35,0x33,0x33,0x3A,0x33,0x33,0x33,0x33,0x7D,0x16}; //7002
uint8 send_cmd_7003[20]={0x68,0x01,0x00,0x00,0x00,0x00,0x00,0x68,0x03,0x08,0x36,0x33,0x33,0x3A,0x33,0x33,0x33,0x33,0x7E,0x16}; //7003

char *g_uart_parity_desc[] = {"none", "odd", "even"};

#if DESC("�¼���ز���",1)

void lc_cur_abn_event_fifo(OOP_ABNORMALCTLOG_T *pEvt)
{
    uint32 slipID = g_ctEvt.nCount;

    if(g_ctEvt.nCount < 10)
    {
        g_ctEvt.nCount++;
    }
    else //����������ȣ����һ�����ݳ��ӣ��µ���ӵ�β��
    {
        memmove(&g_ctEvt.fifo[0], &g_ctEvt.fifo[1], 9*sizeof(g_ctEvt.fifo[0]));
        slipID = 9;
    }

    memcpy(&g_ctEvt.fifo[slipID], pEvt, sizeof(g_ctEvt.fifo[slipID]));
}

/**********************************************************************
* @name      : lc_abn_event_make
* @brief     �������¼��ṹ��
* @param[in] ��tmEvt  �¼�����ʱ���¼�����ʱ��Ϊ0���¼��ָ�ʱ������ʱ���ǻָ�ʱ��ʱ��
* @param[in] ��source  �¼�����Դ 1-���� 0-�ָ�
* @param[in] ��flag   �쳣��־

* @param[out]��oopEvent ������¼��ṹ��      
* @return    ��
* @Create    : ����
* @Date      ��2020-06-28
* @Update    :
**********************************************************************/
void lc_cur_abn_event_make(time_t tmEvt, uint8 source,uint8 flag, uint32 evtIndex, OOP_ABNORMALCTLOG_T *oopEvent)
{
    OOP_DATETIME_S_T evtTime = {0};
//    OOP_DATETIME_S_T startTm = {0};
        
    //�¼���ʼʱ��
    DT_Time2DateTimeShort(time(NULL), &evtTime);
//    DT_Time2DateTimeShort(tmEvt, &startTm);

    oopEvent->nIndex = evtIndex+1; //�¼���ţ���������

//    if(source == 1) //�¼�����
    {
        oopEvent->tmStart = evtTime; //�¼���ʼʱ��
        MEMZERO(&oopEvent->tmEnd, sizeof(OOP_DATETIME_S_T));      //�¼�����ʱ��̶�Ϊ0 
    }
//    else
//    {
//        oopEvent->tmStart = startTm; //�¼���ʼʱ��
//        oopEvent->tmEnd = evtTime;
//    }
    
    oopEvent->source.choice = DT_ENUM; //����Դ
    oopEvent->source.nValue = source;
    oopEvent->abnmlflg.nValue = flag; //�쳣��־ 

    return;
}

/**********************************************************************
* @name      : lc_abn_event_check
* @brief     ��������·�쳣�¼����
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-7-10
* @Update    :
**********************************************************************/
int lc_cur_abn_event_read(DB_CLIENT client_id, RECORD_OAD_T recordOAD, OOP_OCTETVAR64_T mac, uint16* nEventNum, uint32* nIndex)
{
    READ_RECORD_T startReadRecord;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T stepRecordOut;
    OOP_EVENT_T *pTemp = NULL;
    uint32 handle = 0;
    uint32 recNum = 0;
    int  ret = 0;
    time_t timep;

    memset(&startReadRecord,0,sizeof(READ_RECORD_T));
    memset(&stepReadRecord,0,sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut,0,sizeof(RECORD_UNIT_T));

    time(&timep); //��ȡ��1970������˶����룬����time_t���͵�timep
    timep += COMPENSATION_TIME;
    startReadRecord.recordOAD = recordOAD;
    startReadRecord.cType = 0x02;
    startReadRecord.cStart = 0;
    startReadRecord.cEnd = timep;
    startReadRecord.sortType = 0x01;
   
    ret = db_read_record_start(client_id, &startReadRecord, &handle, &recNum);
    if (ret != ERR_OK)
    {
        LCMONIT_FMT_DEBUG("db_read_record_start, OAD=0x%08x error ret %d\n", recordOAD.road.oadMain.value, ret);
        return ret;

    }

    *nEventNum = (uint16)recNum;
    
    if(recNum == 0)
    {
        db_read_record_end(client_id, handle);
        
        return ERR_OK;
    }

    stepReadRecord.MAC = mac;
    
    ret = db_read_record_step(client_id, handle, &stepReadRecord, &stepRecordOut);   
    db_read_record_end(client_id, handle);
    
    if (ret != ERR_OK)
    {
        LCMONIT_FMT_DEBUG("db_read_record_step,infoNum=%hu, OAD=0x%08x error ret %d\n", recordOAD.infoNum, recordOAD.road.oadMain.value, ret);
        return ret;

    }
    else
    {
        pTemp = (OOP_EVENT_T *)stepRecordOut.record[0].outDataBuf;
        *nIndex = pTemp->nIndex;
    }

    return ERR_OK;
}


/**********************************************************************
* @name      : lc_abn_event_write
* @brief     ��д�����������쳣�¼�
* @param[in] ��source   �¼�����Դ 1-���� 0-�ָ�
* @param[in] ��type      �쳣����
* @param[in] ��phasemap ��Ӧ����λA,B,C  

* @param[out]��oopEvent ������¼��ṹ��      
* @return    ��
* @Create    : ����
* @Date      ��2020-06-28
* @Update    :
**********************************************************************/
int lc_cur_abn_event_write(DB_CLIENT client_id, uint8 source,uint8 type,uint8 phasemap)//�����������쳣�¼�
{
    int ret = 0;
    uint32 storeNo = 0;
    uint8 abnflg=0; //�쳣��־ 0x27���� 0x87��· 0xC7��·
    WRITE_RECORD_T recInData;
    OOP_ABNORMALCTLOG_T oopEvent;     //�����¼���Ϣ
    OOP_OCTETVAR16_T  tmnaddr = {0};        //�ն�ͨ�ŵ�ַ        
    NOMAL_OAD_T  normal = {0}; 
    uint32 len = 0;
    GET_RECORD_NUM_T getRecordNum;
    uint16 recNum = 0;
    uint32 evtIndex = 0;   

    MEMZERO(&recInData, sizeof(WRITE_RECORD_T));
    MEMZERO(&getRecordNum, sizeof(GET_RECORD_NUM_T));
    MEMZERO(&oopEvent, sizeof(OOP_ABNORMALCTLOG_T));

    time_t timep;
    time(&timep); //��ȡ��ǰʱ��

    //��ȡ�ն˵�ַ
    normal.oad.value = 0x40010200;
    ret = db_read_nomal(client_id, &normal, sizeof(OOP_OCTETVAR16_T), (uint8*)&tmnaddr, &len);
    if(ret != ERR_OK)
    {
        LCMONIT_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return ret;
    }
    
    //recInData.recordOAD.logicId = 2; //�߼��豸��
    recInData.recordOAD.infoNum = 0; //��Ϣ���
    recInData.recordOAD.classtag = CLASS_DATA_EVENT; //�������ʶ���¼�������
    recInData.recordOAD.road.oadCols.nNum = 1;
    recInData.recordOAD.road.oadCols.oad[0].value = 0x31200200; //�¼�OADΪ��DI����DIΪ0
/*
    // У���룬ͨ�ŵ�ַ��ǰ6���ֽ�,
    recInData.MAC.nNum =6;
    memcpy(recInData.MAC.value, tmnaddr.value, 6);
*/
    lc_cur_abn_event_read(client_id, recInData.recordOAD, recInData.MAC, &recNum, &evtIndex);

    recInData.colStartTm = timep;//��ʼʱ��
    recInData.colEndTm = timep; //����ʱ��
    recInData.colStoreTm = evtIndex+1; //�洢ʱ��Ϊ�¼����
    recInData.inDatalen = sizeof(OOP_ABNORMALCTLOG_T);
    
    abnflg = (type<<5) | phasemap;
    
    lc_cur_abn_event_make(0, source, abnflg, evtIndex, &oopEvent);
    LCMONIT_FMT_TRACE("source=%d, abnflg=0x%02x, evtIndex=%d, evtNum=%d\n", source, abnflg, evtIndex, recNum);

    //д����и���ʾ��
    lc_cur_abn_event_fifo(&oopEvent);

    //���ݻ���Ϊ�������¼���¼�ṹ��
    memcpy(recInData.inDataBuf, &oopEvent, sizeof(OOP_ABNORMALCTLOG_T));
    
    //д����������
    ret = db_write_record(client_id, &recInData, &storeNo);
    if (ret != 0)
    {
        LCMONIT_FMT_DEBUG("db_write_record ctevent failed, ret(%d).\n", ret);
        return ret;
    }

    //�洢��ǰ��¼��
    normal.oad.value = 0x31200400;
    normal.classtag = CLASS_DATA_NORMAL;
    recNum++;
    
    ret = db_write_nomal(client_id, &normal, (uint8*)&recNum, sizeof(uint16));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_write_recNum failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return ret;
    }

    return ret;
}

/**********************************************************************
* @name      : lc_abn_event_check
* @brief     ��������·�쳣�¼����
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-7-10
* @Update    :
**********************************************************************/
uint16 lc_cur_abn_event_check(DB_CLIENT clientid, OOP_LOOPSTAT_T *loopStat)
{
    int i = 0;
    uint8 phasemap = 0;
    NOMAL_OAD_T  normal = {0}; 
    uint32 len = 0;
    BOOLEAN valid = FALSE;
    int ret = 0;
    char phasename[3] = {'A','B','C'};

    //��ȡ�¼�ʹ�ܱ��
    normal.oad.value = 0x31200900;
    ret = db_read_nomal(clientid, &normal, sizeof(BOOLEAN), (uint8*)&valid, &len);
    if((ret != ERR_OK) || (len == 0) || (valid == FALSE))
    {
        return ret;
    }

    for(i = 0; i < 3; i++)
    {
        //��ȷ������Ч״̬�����������ж�,2�βɼ���ABC����״̬��ͬ����״̬��Ч
        if(loopStat->loopstat[i] != g_lastStatus.loopstat[i])
        {
            LCMONIT_FMT_DEBUG("check phase %c status invalid, lastest(%d) != current(%d).\n", phasename[i], g_lastStatus.loopstat[i], loopStat->loopstat[i]);
            valid = FALSE;
            g_lastStatus.loopstat[i] = loopStat->loopstat[i];
        }  
    }

    //״̬��Ч�Ż����������ж�
    if(valid == FALSE)
    {
        return 0;
    }

    for(i = 0; i < 3; i++)
    {
        LCMONIT_FMT_DEBUG("phase %c: legendStatus %d, currentStatus %d.\n", phasename[i], g_legendStatus.loopstat[i], loopStat->loopstat[i]);
    }
    
    //��������Ƿ�����·�¼�
    for(i = 0; i < 3; i++)
    {
        if((loopStat->loopstat[i] == LC_OOP_STATUS_SC) && (g_legendStatus.loopstat[i] == LC_OOP_STATUS_NR))
        {
            phasemap |= 1<<i;
        }
    }

    //�ж�·�¼�����
    if(phasemap != 0)
    {
        lc_cur_abn_event_write(clientid, 1, LC_EVT_STATUS_SC, phasemap);
        LCMONIT_FMT_DEBUG("check SC event happen, phasemap(%d).\n", phasemap);
    }

    //�����໹���¼�
    if(phasemap != 7)
    {
        phasemap = 0;
        //��������Ƿ�����·�¼�
        for(i = 0; i < 3; i++)
        {
            if((loopStat->loopstat[i] == LC_OOP_STATUS_OC) && (g_legendStatus.loopstat[i] == LC_OOP_STATUS_NR))
            {
                phasemap |= 1<<i;
            }
        }

        //�п�·�¼�����
        if(phasemap != 0)
        {
            lc_cur_abn_event_write(clientid, 1, LC_EVT_STATUS_OC, phasemap);
            LCMONIT_FMT_DEBUG("check OC event happen, phasemap(%d).\n", phasemap);
        }
    }

    //�����໹���¼�
    if(phasemap != 7)
    {
        phasemap = 0;
        //��������Ƿ������Ӱ뵼���¼�
        for(i = 0; i < 3; i++)
        {
            if((loopStat->loopstat[i] == LC_OOP_STATUS_SE) && (g_legendStatus.loopstat[i] == LC_OOP_STATUS_NR))
            {
                phasemap |= 1<<i;
            }
        }

        //�д��Ӱ뵼���¼�����
        if(phasemap != 0)
        {
            lc_cur_abn_event_write(clientid, 1, LC_EVT_STATUS_SE, phasemap);
            LCMONIT_FMT_DEBUG("check SCR event happen, phasemap(%d).\n", phasemap);
        }   
    }

    //��·�쳣�ָ�
    if(phasemap != 7)
    {
        phasemap = 0;
        //��������Ƿ�ָ���·�쳣�¼�
        for(i = 0; i < 3; i++)
        {
            if((loopStat->loopstat[i] == LC_OOP_STATUS_NR) && (g_legendStatus.loopstat[i] == LC_OOP_STATUS_SC))
            {
                phasemap |= 1<<i;
            }
        }

        //��·�쳣�ָ�
        if(phasemap != 0)
        {
            lc_cur_abn_event_write(clientid, 0, LC_EVT_STATUS_SC, phasemap);
            LCMONIT_FMT_DEBUG("check SC event recover, phasemap(%d).\n", phasemap);
        }  
    } 

    //��·�쳣�ָ�
    if(phasemap != 7)
    {
        phasemap = 0;
        //��������Ƿ������Ӱ뵼���¼�
        for(i = 0; i < 3; i++)
        {
            if((loopStat->loopstat[i] == LC_OOP_STATUS_NR) && (g_legendStatus.loopstat[i] == LC_OOP_STATUS_OC))
            {
                phasemap |= 1<<i;
            }
        }

        //��·�쳣�ָ�
        if(phasemap != 0)
        {
            lc_cur_abn_event_write(clientid, 0, LC_EVT_STATUS_OC, phasemap);
            LCMONIT_FMT_DEBUG("check OC event recover, phasemap(%d).\n", phasemap);
        }   
    }

    //���Ӱ뵼���쳣�ָ�
    if(phasemap != 7)
    {
        phasemap = 0;
        //��������Ƿ������Ӱ뵼���¼�
        for(i = 0; i < 3; i++)
        {
            if((loopStat->loopstat[i] == LC_OOP_STATUS_NR) && (g_legendStatus.loopstat[i] == LC_OOP_STATUS_SE))
            {
                phasemap |= 1<<i;
            }
        }

        //���Ӱ뵼���쳣�ָ�
        if(phasemap != 0)
        {
            lc_cur_abn_event_write(clientid, 0, LC_EVT_STATUS_SE, phasemap);
            LCMONIT_FMT_DEBUG("check SCR event recover, phasemap(%d).\n", phasemap);
        }  
    }
 
    //�ݲ�֧�ִ�һ���쳣״̬�л�������һ���쳣״̬�ĳ������˳����Ƿ���Ҫ�����ϸ��쳣�Ļָ��¼����޶���

    //����������Ч״̬
    memcpy(&g_legendStatus, loopStat, sizeof(OOP_LOOPSTAT_T));

    return 0;
}

#endif

#if DESC("���ڲ���",1)

//��������ʱ��Ĳ�ֵ ΢��
int time_check(struct timespec *time1,struct timespec *time2)
{
    int tmp =0;
    if(time2->tv_sec == time1->tv_sec)
    {
        tmp = (time2->tv_nsec -time1->tv_nsec)/1000;
        return tmp;
    }
    if(time2->tv_sec > time1->tv_sec)
    {
        tmp = (time2->tv_sec-time1->tv_sec)*1000+time2->tv_nsec/1000-time1->tv_nsec/1000;
        return tmp;
    }
    return tmp;
}

#ifdef PRODUCT_SCU

/**********************************************************************
* @name      : uart_lock_set
* @brief     �����ô�����
* @param[in] ��fd      ���ھ��
* @return    ��=0,��ȷ��!=0,����
* @Create    : ����
* @Date      ��2021-02-01
* @Update    :
**********************************************************************/
int uart_lock_set(int fd, int type)
{
    struct flock lock;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_type = type;
    
    if (fcntl(fd, F_SETLK, &lock) == 0)
    {   
        return 1;
    }
    else
    {
        return -1;
    }
}

/**********************************************************************
* @name      : uart_speed_set
* @brief     �����ô��ڲ�����
* @param[in] ��fd      ���ھ��
* @return    ��=0,��ȷ��!=0,����
* @Create    : ����
* @Date      ��2021-02-01
* @Update    :
**********************************************************************/
int uart_speed_set(int fd, int speed)
{
    int i; 
    int status; 
    int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B600, B300};
    int name_arr[] = {115200, 38400,  19200,  9600,  4800,  2400,  1200, 600, 300};

    struct termios Opt;     /*Opt�д洢���������ݽṹ*/
    tcgetattr(fd, &Opt);    /*��ȡtermios�ṹ*/
    
    for ( i = 0;  i < sizeof(speed_arr) / sizeof(int);  i++) 
    { 
        if (speed == name_arr[i])   /*����֧�ֵĲ�����*/
        { 
            cfsetispeed(&Opt, speed_arr[i]); /*�������벨��?*/
            cfsetospeed(&Opt, speed_arr[i]); /*�����������?*/
            status = tcsetattr(fd, TCSANOW, &Opt);  /*���ô����µ�termios�ṹ*/
            if  (status != 0) 
            {                                 
                return -1;     
            }    
            
            tcflush(fd,TCIOFLUSH); /*������ڵ�������������е���?*/
            return 1;                     
        }  
    }
    return -1;
}

/**********************************************************************
* @name      : uart_param_set
* @brief     �����ô��ڲ���
* @param[in] ��fd      ���ھ��
* @return    ��=0,��ȷ��!=0,����
* @Create    : ����
* @Date      ��2021-02-01
* @Update    :
**********************************************************************/
int uart_param_set(const int fd, LC_UART_PARAM_T *param)
{ 
    struct termios options; 
    if  ( tcgetattr( fd,&options)  !=  0) /*��ȡ����termios�ṹ*/
    { 
        return(FALSE);  
    }
    
    /*����Ϊ���ӹ�ģʽ, ����?*/
    options.c_oflag = 0;
    options.c_lflag = 0;
    options.c_iflag = IGNBRK;
    options.c_cflag |= CLOCAL | CREAD;
    
    options.c_cflag &= ~CSIZE;  /*���ԭ�ַ���*/
    switch (param->dataBits) /*��������λ��*/
    {   
        case 7:             
            options.c_cflag |= CS7;  /*����7λ����λ?*/
            break;
        case 8:     
            options.c_cflag |= CS8;  /*����8λ����λ?*/
            break;   
        default:    
            return (FALSE);  
    }
    
    switch (param->parity) 
    {   
        case UART_PARITY_NONE: 
            options.c_cflag &= ~PARENB;   /* ������żУ����Ч */
            break;
              
        case UART_PARITY_ODD:                                                               
            options.c_cflag |= (PARODD | PARENB);   /*����Ϊ��Ч��*/  
            break;
              
        case UART_PARITY_EVEN:  
            options.c_cflag &= ~PARODD;
            options.c_cflag |= PARENB;  /* ʹ��żУ����?*/ 
            break;
            
        case UART_PARITY_SPACE: /*spaceУ��*/
            options.c_cflag &= ~CSTOPB;
            break;
            
        default:   
            return (FALSE);  
    } 
     
    /* ����ֹͣ?*/ 
    switch (param->stopBits)
    {   
        case 1: /*1λֹͣλ*/
            options.c_cflag &= ~CSTOPB;  
            break;  
        case 2: /*2λֹͣλ*/
            options.c_cflag |= CSTOPB;  
           break;
        default:  
             return (FALSE); 
    }
        
    tcflush(fd,TCIFLUSH);                   /* �������?*/
    options.c_cc[VTIME] = 100;      /* ���ó�ʱ10 seconds*/   
    options.c_cc[VMIN] = 0;             /* �������ã�ʹ������Ч */
    
    options.c_iflag &= ~(IXON|IXOFF|IXANY); /*�������?*/
    
    if (param->flowCtrl == 1)
        options.c_cflag |= CRTSCTS; /*��Ӳ������*/
    else
        options.c_cflag &= ~CRTSCTS;/*��Ӳ����?*/

    options.c_iflag &= ~IGNCR;  /*�������յ���CR�ַ�*/
    options.c_iflag &= ~ICRNL;  

    if (tcsetattr(fd,TCSANOW,&options) != 0)  /* �Դ��ڽ��в�����??*/
    {  
        return (FALSE);  
    }
    return (TRUE);
}

/**********************************************************************
* @name      : uart_param_get
* @brief     ����ȡ���ڲ���
* @param[in] ��fd      ���ھ��
* @return    ��=0,��ȷ��!=0,����
* @Create    : ����
* @Date      ��2021-02-01
* @Update    :
**********************************************************************/
int uart_param_get(const int fd, LC_UART_PARAM_T *param)
{ 
    struct termios options; 
    if  ( tcgetattr( fd,&options)  !=  0) /*��ȡ����termios�ṹ*/
    { 
        return(FALSE);  
    }

    return TRUE;
}

/**********************************************************************
* @name      : uart_open
* @brief     ���򿪴���,���ظ���
* @param[in] ��uart      ���ھ��
* @return    ��=0,��ȷ��!=0,����
* @Create    : ����
* @Date      ��2021-02-01
* @Update    :
**********************************************************************/
int uart_open(LC_UART_VDEV_T *pUart)
{
    if(pUart == NULL)
    {
        return ERR_PNULL;
    }

    if(pUart->fd < 0)
    {  
        pUart->fd = open(pUart->desc, O_RDWR|O_NOCTTY|O_NONBLOCK ); /*�򿪴���*/
        if(pUart->fd >= 0)
        {
            if (uart_lock_set(pUart->fd, F_WRLCK) == -1)    /*����?*/
            {
                LCMONIT_FMT_DEBUG("uart_lock_set %s failed.\n", pUart->desc);  
                close(pUart->fd);
                pUart->fd = -1;
                return ERR_NORMAL;
            }
        }
    }
        
    if (pUart->fd < 0)     /*���򿪴���ʧ�ܣ���??1����֪ͨӦ��?*/
    {           
        LCMONIT_FMT_DEBUG("open %s failed, %s \n", pUart->desc, strerror(errno));  
        return -1;  
    }

    if (uart_speed_set(pUart->fd, pUart->param.baudRate) == -1)  /*���ô��ڲ���?*/
    {
        LCMONIT_FMT_DEBUG("uart_speed_set %s failed.\n", pUart->desc);  
        close(pUart->fd);
        pUart->fd = -1;
        return ERR_INVAL;
    }           

    if (uart_param_set(pUart->fd, &pUart->param) == FALSE) /*���ô��ڲ��� ����??У�� ֹͣ?*/ 
    {
        LCMONIT_FMT_DEBUG("uart_param_set %s failed.\n", pUart->desc);  
        close(pUart->fd);
        pUart->fd = -1;
        return ERR_INVAL;
    }

    LCMONIT_FMT_DEBUG("open %s success.\n", pUart->desc); 
    
    return ERR_OK;
}

/**********************************************************************
* @name      : uart_open
* @brief     ���򿪴���
* @param[in] ��uart      ���ھ��
* @return    ��=0,��ȷ��!=0,����
* @Create    : ����
* @Date      ��2021-02-01
* @Update    :
**********************************************************************/
int uart_close(LC_UART_VDEV_T *pUart)
{
    int ret = 0;
    
    if(NULL == pUart)
    {
        return ERR_PNULL;
    }
    
    if(pUart->fd >= 0)
    {
        uart_lock_set(pUart->fd, F_UNLCK);
        ret = close(pUart->fd);
        if(ret == ERR_OK)
        {
            pUart->fd = -1;
            
            LCMONIT_FMT_DEBUG("close %s success.\n", pUart->desc);  
        }
        else
        {
            LCMONIT_FMT_DEBUG("close %s failed, %s \n", pUart->desc, strerror(errno));  
        }
    }

    return ret;
}

/**********************************************************************
* @name      : uart_write
* @brief     ���򴮿�д����
* @param[in] ��uart    ���ھ��
* @param[in] ��buf     ���ݻ�����
* @param[in] ��len     ����������
* @return    ��=0,��ȷ��!=0,����
* @Create    : ����
* @Date      ��2021-02-01
* @Update    :
**********************************************************************/
int uart_write(LC_UART_VDEV_T *pUart, uint8* buf, int len)
{
//    int write_len= 0;
    int send_len = 0;  

    if(pUart== NULL)
    {
        return ERR_PNULL;
    }
    
    if (pUart->fd < 0)  
    {  
        LCMONIT_FMT_DEBUG("fd %d is invalid\n", pUart->fd);
        return ERR_INVALID;  
    }

//    do 
//    {
//        if((write_len = write(pUart->fd, buf, len)) <= 0)
//        {
//            if(write_len < 0 && errno == EINTR)
//            {
//                write_len = 0;
//            }
//            else
//            {
//                return ERR_NORMAL;
//            }
//        }
//
//        send_len += write_len;
//
//    }while(send_len < len);

    send_len = write(pUart->fd, buf, len);    
    if(send_len == len)
    {
        clock_gettime(CLOCK_MONOTONIC, &pUart->sendTm);
        pUart->sendCnt++;
        return ERR_OK;
    }
    else
    {
        return ERR_NORMAL;
    }
      
    return ERR_OK;
}

/**********************************************************************
* @name      : uart_read
* @brief     ���Ӵ��ڶ�����
* @param[in] ��uart    ���ھ��
* @param[out]��buf     ���ݻ�����
* @param[in] ��len     ����������
* @return    �����������ݳ���
* @Create    : ����
* @Date      ��2021-02-01
* @Update    :
**********************************************************************/
int uart_read(LC_UART_VDEV_T *pUart, uint8* buf, int len)
{
    int fd = pUart->fd;
    int recv_len = 0;                                       
    int data_len = 0;                                       
    int count = 0;
    int delay = 0;
    fd_set set;
    struct timeval timeout;
    int n = 0;
    uint32 baud = pUart->param.baudRate;
    int firstCnt = pUart->frametimeout*1000;//΢�����
    int packetCnt = pUart->bytetimeout*1000;//΢�����
    
    if(NULL == pUart)
    {
        return ERR_PNULL;
    }

    if(fd < 0)
    {
        return ERR_INVAL;
    }
        
    /*���ڽ������ݳ�ʱʱ�䰴20���ֽڼ��ʱ�������㣬300bps ����һ���ֽ���Ҫ36667us
      һ���ֽڰ�����ʼλ��ֹͣλ��У��λ��8λ���� ��11λ����ÿ����ʱ11/baud��*/
    if((baud%300 != 0 ) || baud == 0)
    {   
        delay = 11000000/9600;
    }
    else
    {   
        delay = 11000000/baud;
    }

    firstCnt /= delay;
    packetCnt /= delay;

    while(1)
    {
        //�ȴ����ڿɶ�
        FD_ZERO(&set);
        FD_SET(fd, &set);
        timeout.tv_sec = 0;
        timeout.tv_usec = (baud < 19200)?30000:1000;
        
        do 
        {
            n = select(fd+1, &set, NULL, NULL, &timeout); //��ʱ����0
        }while(n < 0);
  
        if(n > 0)  
        {
            recv_len = read(fd, buf+data_len, len-data_len);
        }
        else
        {
            recv_len = 0;
        }

        // �յ��ֽں���ʱһ���ֽڵ�ʱ����ȥ����
        if(recv_len > 0)   
        {
            data_len += recv_len;
            count = 0;
            usleep(delay);
        }
        else
        {
            // ��ȡ��ʱ
            if((data_len == 0) && (count > firstCnt)) 
            {
                clock_gettime(CLOCK_MONOTONIC, &pUart->recvTm);
                pUart->recvCnt++;
                usleep(delay);
                return data_len;
            }
            
            //�����ֽڶ�ȡ��ʱ�򳤶��Ѿ�����������
            if((data_len != 0) && ((count > packetCnt) || (data_len > len))) 
            {
                clock_gettime(CLOCK_MONOTONIC, &pUart->recvTm);
                pUart->recvCnt++;
                usleep(delay);
                return (data_len < 0) ? 0 : data_len;                    
            }
        }
  
        count++;
    }

    return data_len;
}

#else
/**********************************************************************
* @name      : uart_open
* @brief     ���򿪴���,���ظ���
* @param[in] ��uart      ���ھ��
* @return    ��=0,��ȷ��!=0,����
* @Create    : ����
* @Date      ��2021-02-01
* @Update    :
**********************************************************************/
int uart_open(LC_UART_VDEV_T *pUart)
{
    int ret = ERR_OK;
    UART_DEVICE_T* dev = NULL;
    
    if((pUart == NULL) || (pUart->desc == NULL))
    {
        return ERR_PNULL;
    }

    dev = (UART_DEVICE_T*)hal_device_get(pUart->halname);
    if(NULL == dev)
    {
        return ERR_PNULL;    
    }

    ret = dev->uart_get_real_dev_name(dev, (uint8*)pUart->desc);
    if(ret <= 0)
    {
        LCMONIT_FMT_DEBUG("uart_get_real_dev_name failed. dev(%s), ret(%d)\n", pUart->halname, ret);
        return ERR_OPTION;    
    }

    // ���豸
    ret = dev->uart_param_set(dev, pUart->param.baudRate, pUart->param.dataBits, pUart->param.stopBits, pUart->param.parity);
    if(ret != 0)
    {
        LCMONIT_FMT_DEBUG("uart_param_set failed. dev(%s), ret(%d)\n", pUart->desc, ret);
        hal_device_release((HW_DEVICE *)dev);
        return ERR_INVAL;
    }

    pUart->fd = (int)dev;

    LCMONIT_FMT_DEBUG("open %s success: baudrate[%d]-databit[%d]-parity[%s]-stopbit[%d].\n", 
        pUart->halname, pUart->param.baudRate, pUart->param.dataBits, g_uart_parity_desc[pUart->param.parity], pUart->param.stopBits);  
    
    return ERR_OK;
}

/**********************************************************************
* @name      : uart_open
* @brief     ���򿪴���
* @param[in] ��uart      ���ھ��
* @return    ��=0,��ȷ��!=0,����
* @Create    : ����
* @Date      ��2021-02-01
* @Update    :
**********************************************************************/
int uart_close(LC_UART_VDEV_T *pUart)
{
    UART_DEVICE_T* dev = (UART_DEVICE_T* )pUart->fd;

    if(NULL == pUart)
    {
        return ERR_PNULL;
    }
    
    if(dev != NULL)
    {
        hal_device_release((HW_DEVICE *)dev);

        dev = NULL;
    }

    pUart->fd = (int)dev;
    
    return ERR_OK;
}

/**********************************************************************
* @name      : uart_write
* @brief     ���򴮿�д����
* @param[in] ��uart    ���ھ��
* @param[in] ��buf     ���ݻ�����
* @param[in] ��len     ����������
* @return    ��=0,��ȷ��!=0,����
* @Create    : ����
* @Date      ��2021-02-01
* @Update    :
**********************************************************************/
int uart_write(LC_UART_VDEV_T *pUart, uint8* buf, int len)
{
    UART_DEVICE_T* dev = (UART_DEVICE_T* )pUart->fd;
    int ret = 0;  

    if(pUart== NULL)
    {
        return ERR_PNULL;
    }
    
    if(dev == NULL)  
    {  
        LCMONIT_FMT_DEBUG("dev is invalid\n");
        return ERR_INVALID;  
    }

    ret = dev->uart_data_send(dev, buf, len);    
    if(ret == 0)
    {
        clock_gettime(CLOCK_MONOTONIC, &pUart->sendTm);
        pUart->sendCnt++;
        return ERR_OK;
    }
    else
    {        
        LCMONIT_FMT_DEBUG("%s send failed.\n", pUart->halname);
        return ERR_NORMAL;
    }
      
    return ERR_OK;
}

/**********************************************************************
* @name      : uart_read
* @brief     ���Ӵ��ڶ�����
* @param[in] ��uart    ���ھ��
* @param[out]��buf     ���ݻ�����
* @param[in] ��len     ����������
* @return    �����������ݳ���
* @Create    : ����
* @Date      ��2021-02-01
* @Update    :
**********************************************************************/
int uart_read(LC_UART_VDEV_T *pUart, uint8* buf, int len)
{
    UART_DEVICE_T* dev = (UART_DEVICE_T* )pUart->fd;
    int fd = -1;
    int recv_len = 0;                                       
    int data_len = 0;                                       
    int delay = 0;
    fd_set set;
    struct timeval timeout;
    int n = 0;
    uint32 baud = pUart->param.baudRate;
    int frameus = pUart->frametimeout*1000;//΢�����
    int octetus = pUart->bytetimeout*1000;//΢�����
    struct timespec timenow;
    struct timespec lastrecv;

    if(dev == NULL)
    {
        return ERR_INVAL;
    }

    fd = dev->uart_get_handle(dev);
    if(fd < 0)
    {
        return ERR_INVAL;
    }

    //�ȴ����ڿɶ�
    FD_ZERO(&set);
    FD_SET(fd, &set);
    timeout.tv_sec = frameus/1000000;
    timeout.tv_usec = frameus%1000000;
    
    do 
    {
        n = select(fd+1, &set, NULL, NULL, &timeout); //��ʱ����0
    }while(n < 0);

    if(n == 0)
    {
        LCMONIT_FMT_DEBUG("%s frame timeout: %d ms\n", pUart->desc, pUart->frametimeout);
        return 0;
    }

    /*���ڽ������ݳ�ʱʱ�䰴20���ֽڼ��ʱ�������㣬300bps ����һ���ֽ���Ҫ36667us
    һ���ֽڰ�����ʼλ��ֹͣλ��У��λ��8λ���� ��11λ����ÿ����ʱ11/baud��*/
    if((baud%300 != 0 ) || baud == 0)
    {   
        delay = 11000000/9600;
    }
    else
    {   
        delay = 11000000/baud;
    }

    //��ֹ�ֽڳ�ʱ��С
    if(octetus < delay)
    {
        octetus += delay;
    }
    
    while(1)
    {
        recv_len = dev->uart_data_recv(dev, buf+data_len, len-data_len);
        
        // �յ��ֽں���ʱһ���ֽڵ�ʱ����ȥ����
        if(recv_len > 0)   
        {        
            clock_gettime(CLOCK_MONOTONIC, &lastrecv);
            data_len += recv_len;

            //�����Ѿ�����������
            if(data_len > len)
            {
                clock_gettime(CLOCK_MONOTONIC, &pUart->recvTm);
                pUart->recvCnt++;
                break;                    
            }
        }
        else
        { 
            clock_gettime(CLOCK_MONOTONIC, &timenow);
            //�ֽڶ�ȡ��ʱ
            if(time_check(&lastrecv,&timenow) > octetus)
            {
                clock_gettime(CLOCK_MONOTONIC, &pUart->recvTm);
                pUart->recvCnt++;
                break;                    
            }
        }
    }

    data_len = (data_len < 0) ? 0 : data_len;
    return data_len;
}

#endif

/**********************************************************************
* @name      : uart_transmit
* @brief     ���������ݴ���
* @param[in] ��void *pDev                   ���ھ��
               int nBufLen                  �����ճ���
               int rsptime                  ���ݷ���ʱ��(20ms)
* @param[out]��
* @return    �����յ����ݵĳ���
* @Create    : ����
* @Date      ��2020-3-13
* @Update    :
**********************************************************************/
int uart_transmit(LC_UART_VDEV_T *pUart, uint8* pSend, int sendLen, uint8* pRecv, int maxRecv)
{
    int    ret = 0;

    ret = uart_write(pUart, pSend, sendLen);
    if(ret != 0)
    {
        return 0;
    }
    
    return uart_read(pUart, pRecv, maxRecv);
}

/**********************************************************************
* @name      : uart_reopen
* @brief     �����´򿪴���
* @param[in] ��uart      ���ھ��
* @return    ��=0,��ȷ��!=0,����
* @Create    : ����
* @Date      ��2021-02-01
* @Update    :
**********************************************************************/
int uart_reopen(LC_UART_VDEV_T *pUart)
{
    int ret = 0;
    
    ret = uart_close(pUart);
    if(ret == 0)
    {
       sleep_no_interrupt(2);
       ret = uart_open(pUart); 
    }

    return ret;
}

#endif

#if DESC("ģ��Э�����",1)

BOOLEAN lc_zero_check(void *buf, uint16 len)
{
    uint8 *tmp = (uint8*)buf;
    
    while(len--)
    {
        if(*tmp++ != '0')
        {
            return FALSE;
        }
    }

    return TRUE;
}


/**********************************************************************
* @name      : lc_make_frame
* @brief     ��ģ��ӿ�Э����֡
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-7-10
* @Update    :
**********************************************************************/
LC_OOP_STATUS_E lc_status_mod2oop(LC_MOD_STATUS_E mod)
{
    switch(mod)
    {
        case LC_MOD_STATUS_NR: return LC_OOP_STATUS_NR; break;
        case LC_MOD_STATUS_SC: return LC_OOP_STATUS_SC; break;
        case LC_MOD_STATUS_OC: return LC_OOP_STATUS_OC; break;
        case LC_MOD_STATUS_SE: return LC_OOP_STATUS_SE; break;
        default:break; 
    }

    return 0;
}


/**********************************************************************
* @name      : lc_make_frame
* @brief     ��ģ��ӿ�Э����֡
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-7-10
* @Update    :
**********************************************************************/
int lc_analogs_parse(uint8 *srcBuf, uint32 *pOffset, uint8* dstBuf)
{
    uint32 sOffset = *pOffset;
    uint8          i = 0;
    uint32 dstData[3] = {0};
    
    if(srcBuf[sOffset++] != DT_ARRAY) //��array
    {
        LCMONIT_BUF_DEBUG(srcBuf, sOffset, "check array type failed.\n");
        return ERR_TYPE;
    }

    if(srcBuf[sOffset++] != 3) //array��Ա��
    {
        LCMONIT_BUF_DEBUG(srcBuf, sOffset, "check array num failed.\n");
        return ERR_TYPE;
    }

    for(i = 0; i < 3; i++)
    {
        if(srcBuf[sOffset++] != DT_DOUBLE_LONG_UNSIGNED) //��double-long-unsigned
        {
            LCMONIT_BUF_DEBUG(srcBuf, sOffset, "check double-long-unsigned type failed.\n");
            return ERR_TYPE;
        }

        memcpy_r(&dstData[i], &srcBuf[sOffset], sizeof(uint32));
        sOffset += sizeof(uint32);
    }

    memcpy(dstBuf, dstData, sizeof(dstData));
    *pOffset = sOffset;

    return 0;
}

/**********************************************************************
* @name      : lc_make_frame
* @brief     ��ģ��ӿ�Э����֡
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-7-10
* @Update    :
**********************************************************************/
uint16 lc_modmsg_make(uint16 dt, uint8 *sendBuf, uint16 buflen)
{
    uint8          msgbuf[128] = {0};
    MAPMSG_INFO_T *pMsgInfo = (MAPMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    static uint8 index = 0;

    pMsgInfo->cmt.dir = MAPMSG_DIR_TERMINAL;
    pMsgInfo->cmt.prm = MAPMSG_PRM_TERMINAL;
    pMsgInfo->cmt.cmd = MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index++;
    
    Temp[offset++] = MAPMSG_APDU_GetRequest;
    mapmsg_memcpy_r(Temp + offset, &dt, sizeof(uint16));
    offset += sizeof(uint16);

    pMsgInfo->len = offset;

    return mapmsg_package(pMsgInfo, sendBuf, buflen);
}

/**********************************************************************
* @name      : lc_workstatus_store
* @brief     ��������Ϣ�洢
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-7-10
* @Update    :
* @detail    :
* ��·Ѳ��ģ��Ӧ������ݵ�λ�ͱ���                  698��Ӧ�Ļ���͵�λ          ת����698����
* ������·״̬             HH          1               
* ��ϻ����������¶� XXX.X                2   ��      -1     ��     1
* ��Ƶ�����ֵ             XXX.XXX     3   kHz     0     Hz    1
* ��Ƶ����Сֵ             XXX.XXX     3   kHz     0     Hz    1
* ��Ƶ������Чֵ            XXX.XXX     3   A      -3     A     1
* ��һ���·�迹Ƶ�� XXX.X                2   kHz     0     Hz    100
* ��һ���·�迹ģֵ XXXXXX.XX            4   mV     -2     ��     1
* ��һ���·�迹�Ƕ� XXX.X                2   ��      -1     ��     1
* �ڶ����·�迹Ƶ�� XXX.X                2   kHz     0     Hz    100
* �ڶ����·�迹ģֵ XXXXXX.XX            4   mV     -2     ��     1
* �ڶ����·�迹�Ƕ� XXX.X                2   ��      -1     ��     1
* �������·�迹Ƶ�� XXX.X                2   kHz     0     Hz    100
* �������·�迹ģֵ XXXXXX.XX            4   mV     -2     ��     1
* �������·�迹�Ƕ� XXX.X                2   ��      -1     ��     1
**********************************************************************/
int lc_workstatus_store(DB_CLIENT clientid, LC_WORK_STATUS_T *workStatus)
{
    int ret = 0;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_WORKINFO_T workinfo;
    MEMZERO(&workinfo, sizeof(OOP_WORKINFO_T));
    OOP_LOOPSTAT_T loopStat = {0};
    int i = 0;
    char phasename[3] = {'A','B','C'};

    for(i = 0; i < 3; i++)
    {
        workinfo.item[i].curloopstat = lc_status_mod2oop(workStatus->loopStatus[i]);
        workinfo.item[i].temp = workStatus->envTemp[i];
        workinfo.item[i].maxfreq = workStatus->freqMax[i];
        workinfo.item[i].minfreq = workStatus->freqMin[i];
        workinfo.item[i].curvalid = workStatus->curValue[i];

        workinfo.item[i].impval.nNum = 3;    
        workinfo.item[i].impval.impeval[0] = workStatus->impeValue1[i];
        workinfo.item[i].impval.impeval[1] = workStatus->impeValue2[i];
        workinfo.item[i].impval.impeval[2] = workStatus->impeValue3[i];

        workinfo.item[i].impang.nNum = 3;
        workinfo.item[i].impang.impeang[0] = workStatus->impeAnge1[i];  
        workinfo.item[i].impang.impeang[1] = workStatus->impeAnge2[i];        
        workinfo.item[i].impang.impeang[2] = workStatus->impeAnge3[i];   

        workinfo.item[i].impfrq.nNum = 3;
        workinfo.item[i].impfrq.impefreq[0] = workStatus->impeFreq1[i]*100;
        workinfo.item[i].impfrq.impefreq[1] = workStatus->impeFreq2[i]*100;
        workinfo.item[i].impfrq.impefreq[2] = workStatus->impeFreq3[i]*100;

        loopStat.loopstat[i] = workinfo.item[i].curloopstat;    

        LCMONIT_FMT_DEBUG("\r\n store phase %c:\
                           \r\n loopState: %d,\
                           \r\n temperature: %d\
                           \r\n freqMax: %d\
                           \r\n freqMin: %d\
                           \r\n current: %d\
                           \r\n Freq_Am_Ang_1: [%d %d %d]\
                           \r\n Freq_Am_Ang_2: [%d %d %d]\
                           \r\n Freq_Am_Ang_3: [%d %d %d]\n",
                            phasename[i],
                            workStatus->loopStatus[i],workStatus->envTemp[i],
                            workStatus->freqMax[i],workStatus->freqMin[i],workStatus->curValue[i],
                            workStatus->impeFreq1[i], workStatus->impeValue1[i],workStatus->impeAnge1[i],
                            workStatus->impeFreq2[i],workStatus->impeValue2[i],workStatus->impeAnge2[i],
                            workStatus->impeFreq3[i],workStatus->impeValue3[i],workStatus->impeAnge3[i]);
     
    }

    //������Ϣ
    workinfo.nNum = 3;

    nomalOAD.classtag = CLASS_DATA_NORMAL;
    nomalOAD.oad.value = 0x40400300;
    ret = db_write_nomal(clientid, &nomalOAD, (uint8*)&workinfo, sizeof(workinfo));
    if(ret != 0)
    {
        LCMONIT_FMT_DEBUG("lc_workstatus_store failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
        ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return ret;
    }

    //��·״̬
    loopStat.nNum = 3;
    
    nomalOAD.oad.value = 0x20500200;
    ret = db_write_nomal(clientid, &nomalOAD, (uint8*)&loopStat, sizeof(loopStat));
    if(ret != 0)
    {
        LCMONIT_FMT_DEBUG("lc_loopStatus_store failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
        ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return ret;
    }

    //��·�쳣�¼�����
    lc_cur_abn_event_check(clientid, &loopStat);
        
    return ret;
}

int lc_versioninfo_store(DB_CLIENT clientid, OOP_CT_VERTIONS_T *version)
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_CTINFO_T ctinfo = {0};
    int i = 0;
    char *type[4] = {"CORE","CT-A","CT-B", "CT-C"};
    ctinfo.nNum = version->nNum;

    for(i = 0; i < ctinfo.nNum; i++)
    {
        memcpy(&ctinfo.item[i].id, &version->ver[i].szId, sizeof(OOP_OCTETVAR64_T));
        ctinfo.item[i].szSoftVer = version->ver[i].szSoftVer;
        ctinfo.item[i].szSoftDate = version->ver[i].szSoftDate;
        ctinfo.item[i].szHardVer = version->ver[i].szHardVer;
        ctinfo.item[i].szHardDate = version->ver[i].szHardDate;
        memcpy(&ctinfo.item[i].szFactory,  &version->ver[i].szFactory, sizeof(OOP_VISIBLESTR4_T));

        MDLMSG_FMT_DEBUG("\r\n ����        : %s\
                           \r\n �ͺ�        : %s\
                           \r\n ID          : %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\
                           \r\n ���̴���    : %s\
                           \r\n ����汾��  : %s\
                           \r\n �����������: %s\
                           \r\n Ӳ���汾��  : %s\
                           \r\n Ӳ����������: %s\
                           \r\n ������չ��Ϣ: %s\n", 
                           type[i], version->ver[i].szType.value,version->ver[i].szId.value[0],version->ver[i].szId.value[1],
                           version->ver[i].szId.value[2],version->ver[i].szId.value[3],version->ver[i].szId.value[4],
                           version->ver[i].szId.value[5],version->ver[i].szId.value[6],version->ver[i].szId.value[7],
                           version->ver[i].szId.value[8],version->ver[i].szId.value[9],version->ver[i].szId.value[10],
                           version->ver[i].szId.value[11],version->ver[i].szId.value[12],version->ver[i].szId.value[13],
                           version->ver[i].szId.value[14],version->ver[i].szId.value[15],version->ver[i].szId.value[16],
                           version->ver[i].szId.value[17],version->ver[i].szId.value[18],version->ver[i].szId.value[19],
                           version->ver[i].szId.value[20],version->ver[i].szId.value[21],version->ver[i].szId.value[22], version->ver[i].szId.value[23], 
                           version->ver[i].szFactory.value,version->ver[i].szSoftVer.value,version->ver[i].szSoftDate.value,
                           version->ver[i].szHardVer.value,version->ver[i].szHardDate.value,version->ver[i].szExtend.value);

        if(lc_zero_check(ctinfo.item[i].szSoftDate.value, strlen(ctinfo.item[i].szSoftDate.value)) == TRUE)
        {
            MDLMSG_FMT_DEBUG("full zero data, need retry.\n");
            g_verNeedRetry = TRUE;
        }
    }

    //�汾��Ϣ
    nomalOAD.classtag = CLASS_DATA_NORMAL;
    nomalOAD.oad.value = 0x40400500;
    ret = db_write_nomal(clientid, &nomalOAD, (uint8*)&ctinfo, sizeof(OOP_CTINFO_T));
    if(ret != 0)
    {
        LCMONIT_FMT_DEBUG("lc_workstatus_store failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
        ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return ret;
    }
 
    return ret;
}

int lc_versioninfo_get(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata)
{
    int ret = 0;
    uint32 sOffset = 0;
    uint8  i = 0;
    uint8 bytes = 0;      //������������
    uint32 nNum  = 0;
    OOP_CT_VERTIONS_T ctVersion;
    uint32 len = 0;

    MEMZERO(&ctVersion, sizeof(OOP_CT_VERTIONS_T));
    
    LCMONIT_BUF_TRACE(apdu, apduLen, "lc version ciphertext apdu.\n");    
    
    //ֻ����APDU�е�DATA,���ܺ��ٽ���
    String_Decrypt(apdu, apduLen);
    
    LCMONIT_BUF_DEBUG(apdu, apduLen, "lc version plaintext apdu.\n");
    
    if(apdu[sOffset++] != DT_ARRAY) //��array
    {
        LCMONIT_BUF_DEBUG(apdu, sOffset, "check array type failed.\n");
        return ERR_TYPE;
    }

    ctVersion.nNum = apdu[sOffset++];

    if(ctVersion.nNum != 4) //array��Ա��
    {
        LCMONIT_BUF_DEBUG(apdu, sOffset, "check array num failed.\n");
        return ERR_TYPE;
    }

    for(i = 0; i < ctVersion.nNum; i++)
    {
        if(apdu[sOffset++] != DT_STRUCTURE) //��struct
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check struct type failed.\n");
            return ERR_TYPE;
        }
        
        if(apdu[sOffset++] != 8) //��Ա��
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check struct num failed.\n");
            return ERR_TYPE;
        }

        //�ͺ�
        if(apdu[sOffset++] != DT_VISIBLE_STRING) //visible-string
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check visible-string type failed.\n");
            return ERR_TYPE;
        }

        ctVersion.ver[i].szType.nNum = appmsg_get_len_offset(&apdu[sOffset], &bytes); //����visible-string�ĳ���
        sOffset += bytes;

        memcpy(&ctVersion.ver[i].szType.value, &apdu[sOffset], ctVersion.ver[i].szType.nNum);
        sOffset += ctVersion.ver[i].szType.nNum;

        //ID
        if(apdu[sOffset++] != DT_VISIBLE_STRING) //visible-string
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check visible-string type failed.\n");
            return ERR_TYPE;
        }

        len = appmsg_get_len_offset(&apdu[sOffset], &bytes); //����visible-string�ĳ���
        sOffset += bytes;

        //�˴�698������Ҫ����octet-stringҪ��ת��
        ctVersion.ver[i].szId.nNum = strt2octet((char*)&apdu[sOffset], len, ctVersion.ver[i].szId.value, 64);
        if ((ctVersion.ver[i].szId.nNum < 0) || (ctVersion.ver[i].szId.nNum > 64))
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check visible-string type failed. ctVersion.ver[i].szId.nNum[%d]\n", ctVersion.ver[i].szId.nNum);
            return ERR_TYPE;
        }
        sOffset += len;
        
        //���̴���
        if(apdu[sOffset++] != DT_VISIBLE_STRING) //visible-string
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check visible-string type failed.\n");
            return ERR_TYPE;
        }

        nNum = appmsg_get_len_offset(&apdu[sOffset], &bytes); //����visible-string�ĳ���
        sOffset += bytes;

        memcpy(&ctVersion.ver[i].szFactory.value, &apdu[sOffset], nNum);
        sOffset += nNum;

        //����汾��
        if(apdu[sOffset++] != DT_VISIBLE_STRING) //visible-string
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check visible-string type failed.\n");
            return ERR_TYPE;
        }

        nNum = appmsg_get_len_offset(&apdu[sOffset], &bytes); //����visible-string�ĳ���
        sOffset += bytes;

        memcpy(&ctVersion.ver[i].szSoftVer.value, &apdu[sOffset], nNum);
        sOffset += nNum;

        //����汾����
        if(apdu[sOffset++] != DT_VISIBLE_STRING) //visible-string
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check visible-string type failed.\n");
            return ERR_TYPE;
        }

        nNum = appmsg_get_len_offset(&apdu[sOffset], &bytes); //����visible-string�ĳ���
        sOffset += bytes;

        memcpy(&ctVersion.ver[i].szSoftDate.value, &apdu[sOffset], nNum);
        sOffset += nNum;

        //Ӳ���汾��
        if(apdu[sOffset++] != DT_VISIBLE_STRING) //visible-string
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check visible-string type failed.\n");
            return ERR_TYPE;
        }

        nNum = appmsg_get_len_offset(&apdu[sOffset], &bytes); //����visible-string�ĳ���
        sOffset += bytes;

        memcpy(&ctVersion.ver[i].szHardVer.value, &apdu[sOffset], nNum);
        sOffset += nNum;

        //Ӳ���汾����
        if(apdu[sOffset++] != DT_VISIBLE_STRING) //visible-string
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check visible-string type failed.\n");
            return ERR_TYPE;
        }

        nNum = appmsg_get_len_offset(&apdu[sOffset], &bytes); //����visible-string�ĳ���
        sOffset += bytes;

        memcpy(&ctVersion.ver[i].szHardDate.value, &apdu[sOffset], nNum);
        sOffset += nNum;

        //������չ��Ϣ
        if(apdu[sOffset++] != DT_VISIBLE_STRING) //visible-string
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check visible-string type failed.\n");
            return ERR_TYPE;
        }

        nNum = appmsg_get_len_offset(&apdu[sOffset], &bytes); //����visible-string�ĳ���
        sOffset += bytes;

        memcpy(&ctVersion.ver[i].szExtend.value, &apdu[sOffset], nNum);
        sOffset += nNum;
    }
    
    //�汾��Ϣ�洢
    ret = lc_versioninfo_store(clientid, &ctVersion);
    g_ctVer = ctVersion;

    return ret;
}

/**********************************************************************
* @name      : lc_modmsg_parse
* @brief     ��ģ��ӿ�Э�����
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-7-10
* @Update    :
**********************************************************************/
int lc_modmsg_parse(DB_CLIENT clientid, uint8 *recvBuf, uint16 buflen)
{
    int            ret = 0;
    uint16         offset = 0;
    MAPMSG_INFO_T *pMsgInfo = NULL;
    uint8         *temp = NULL;
    uint16         dt = 0;
    uint8          i = 0;
    int            tempret = 0;
    uint32 sOffset = 0;
    
    //�ҵ�֡��ʼλ
    while(offset < buflen)
    {
        if(FRAMESTART == recvBuf[offset])
        {
            tempret = mapmsg_split(recvBuf + offset, buflen - offset, &pMsgInfo);
            if(tempret != 0)
            {
                LCMONIT_BUF_DEBUG(recvBuf + offset, buflen - offset, "mapmsg split failed, tempret(%d)\n", tempret);
            }
            break;
        }
        offset++;
    }

    if(pMsgInfo == NULL)
    {
        LCMONIT_BUF_DEBUG(temp, sOffset, "check null pointer.\n");
        return ERR_PNULL;
    }

    //���ܵ�һ�������Ƕ�֡
    if(MAPMSG_CTR_MSG == pMsgInfo->cmt.cmd && pMsgInfo->len > 4)
    {
        temp = pMsgInfo->apdu;

        if((MAPMSG_APDU_GetResponse == temp[0])&&(1 == temp[3]))  //data [1]
        {
            sOffset += 2;

            memcpy_r(&dt, &temp[1], sizeof(uint16));
            sOffset += sizeof(uint16);

            for(i = 0; i < g_modParse_Max; i++)
            {
                if((g_modParse[i].di == dt)&&(g_modParse[i].apduGet != NULL))
                {
                    ret = g_modParse[i].apduGet(clientid, &temp[sOffset], pMsgInfo->len-sOffset, NULL);
                    break;
                }
            }

            if(i == g_modParse_Max)
            {
                ret = ERR_OPTION;
            }
        }
    }
    
    FREE_S(pMsgInfo);
    
    return ret;
}

int lc_ctworkstate_get(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata)
{
    uint32 sOffset = 0;
    uint8  i = 0;
    uint8 bytes = 0;      //������������
    uint32 nNum  = 0;
    int ret = ERR_OK;
    LC_WORK_STATUS_T workstatus;
    char phasename[3] = {'A','B','C'};

    MEMZERO(&workstatus, sizeof(LC_WORK_STATUS_T));

    LCMONIT_BUF_TRACE(apdu, apduLen, "lc ciphertext apdu.\n");    
    
    //ֻ����APDU�е�DATA,���ܺ��ٽ���
    String_Decrypt(apdu, apduLen);
    
    LCMONIT_BUF_DEBUG(apdu, apduLen, "lc plaintext apdu.\n");
    
    if(apdu[sOffset++] != DT_STRUCTURE) //��struct
    {
        LCMONIT_BUF_DEBUG(apdu, sOffset, "check struct type failed.\n");
        return ERR_TYPE;
    }
    
    if(apdu[sOffset++] != 32) //��Ա��
    {
        LCMONIT_BUF_DEBUG(apdu, sOffset, "check struct num failed.\n");
        return ERR_TYPE;
    }
    
    //������·״̬
    if(apdu[sOffset++] != DT_ARRAY) //��array
    {
        LCMONIT_BUF_DEBUG(apdu, sOffset, "check array type failed.\n");
        return ERR_TYPE;
    }
    
    if(apdu[sOffset++] != 3) //array��Ա��
    {
        LCMONIT_BUF_DEBUG(apdu, sOffset, "check array num failed.\n");
        return ERR_TYPE;
    }
    
    for(i = 0; i < 3; i++)
    {
        if(apdu[sOffset++] != DT_OCTET_STRING) //octet-string
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check octet-string type failed.\n");
            return ERR_TYPE;
        }
    
        nNum = appmsg_get_len_offset(&apdu[sOffset], &bytes); //����octet-string�ĳ���
        sOffset += bytes;
    
        memcpy(&workstatus.loopStatus[i], &apdu[sOffset], nNum);
        sOffset += nNum;
    }
    
    //��ʶ��������
    if(apdu[sOffset++] != DT_ARRAY) //��array
    {
        LCMONIT_BUF_DEBUG(apdu, sOffset, "check array type failed.\n");
        return ERR_TYPE;
    }
    
    if(apdu[sOffset++] != 0) //array��Ա��
    {
        LCMONIT_BUF_DEBUG(apdu, sOffset, "check array num failed.\n");
        return ERR_TYPE;
    }
    
    //�����¶�
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.envTemp);
    
    //Ƶ�����ֵ
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.freqMax);
    
    //Ƶ����Сֵ
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.freqMin);
    
    //��Ƶ������Чֵ
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.curValue);
    
    //��һ���迹Ƶ��
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.impeFreq1);
    
    //��һ���迹ģֵ
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.impeValue1);  
    
    //��һ���迹�Ƕ�
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.impeAnge1);                    
    
    //�ڶ����迹Ƶ��
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.impeFreq2);
    
    //�ڶ����迹ģֵ
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.impeValue2);  
    
    //�ڶ����迹�Ƕ�
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.impeAnge2);       
    
    //�������迹Ƶ��
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.impeFreq3);
    
    //�������迹ģֵ
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.impeValue3);  
    
    //�������迹�Ƕ�
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.impeAnge3);
    if(ret != ERR_OK)
    {
        return ret;
    }

    memcpy(&g_workstatus[g_secNum-1], &workstatus, sizeof(LC_WORK_STATUS_T));
    
    for(i = 0; i < 3; i++)
    {
        LCMONIT_FMT_DEBUG("secNum %d, phase %c, loopState %d\n",g_secNum, phasename[i], workstatus.loopStatus[i]);
    }

    return ret;
}

/**
*********************************************************************
* @brief��     lc_status_cpy
* @param[in]�� NA
* @param[out]��NA
* @return��     void
*********************************************************************
*/
void lc_status_cpy(LC_WORK_STATUS_T *dstData, LC_WORK_STATUS_T *srcData,  uint8 phase)
{
    dstData->loopStatus[phase] = srcData->loopStatus[phase];
    
    dstData->envTemp[phase] = srcData->envTemp[phase];
    dstData->freqMax[phase] = srcData->freqMax[phase];
    dstData->freqMin[phase] = srcData->freqMin[phase];
    dstData->curValue[phase] = srcData->curValue[phase];  

    dstData->impeValue1[phase] = srcData->impeValue1[phase];
    dstData->impeValue2[phase] = srcData->impeValue2[phase];
    dstData->impeValue3[phase] = srcData->impeValue3[phase];

    dstData->impeAnge1[phase] = srcData->impeAnge1[phase];  
    dstData->impeAnge2[phase] = srcData->impeAnge2[phase];        
    dstData->impeAnge3[phase] = srcData->impeAnge3[phase];   

    dstData->impeFreq1[phase] = srcData->impeFreq1[phase];
    dstData->impeFreq2[phase] = srcData->impeFreq2[phase];
    dstData->impeFreq3[phase] = srcData->impeFreq3[phase];    
}

#endif

#if DESC("ģ����Ϣ�շ�",1)

/**
*********************************************************************
* @brief��     lc_version_get
* @param[in]�� NA
* @param[out]��NA
* @return��     void
*********************************************************************
*/
int lc_version_get(void *pDev, DB_CLIENT clientid, int socketfd)
{
    int ret = 0;
    uint8 sendbuf[128] = {0};
    uint16 sendlen = 0;   //���ͱ��ĳ���
    uint8 sRecvbuf[1024] = {0};//���ν���
    uint16 sRecvLen = 0; //���ν��ճ���
    static uint16 retryCnt = 0;

    if(pDev == NULL)
    {
        LCMONIT_FMT_DEBUG("pDev is NULL\n");
        return ERR_PNULL;
    }
    
    sendlen = lc_modmsg_make(MAPMSG_DT_LCM_VERSIONINFO, sendbuf, sizeof(sendbuf));

    LCMONIT_BUF_DEBUG(sendbuf, sendlen, "lc_uart_send: version.\n");

    sRecvLen = uart_transmit(pDev, sendbuf, sendlen, sRecvbuf, sizeof(sRecvbuf));
    if(sRecvLen != 0)
    {
        retryCnt = 0;
        ret = lc_modmsg_parse(clientid, sRecvbuf, sRecvLen);
        if(ret != 0)
        {
            LCMONIT_BUF_DEBUG(sRecvbuf, sRecvLen, "lc_modmsg_parse failed,ret(%d).\n", ret);
            return ERR_NORMAL;
        }
    }
    else
    {
        if(++retryCnt >= LCM_SEND_RETRY)
        {
            retryCnt = 0;
            return ERR_INVALID;
        }
    }
    
    return ret;
}

/**
*********************************************************************
* @brief��     lc_workstate_get
* @param[in]�� NA
* @param[out]��NA
* @return��     void
*********************************************************************
*/
int lc_workstate_get(void *pDev, DB_CLIENT clientid, int socketfd)
{
    int ret = 0;
    uint8 sendbuf[128] = {0};
    uint16 sendlen = 0;   //���ͱ��ĳ���
    uint8 sRecvbuf[1024] = {0};//���ν���
    uint16 sRecvLen = 0; //���ν��ճ���
    static uint16 retryCnt = 0;

    if(pDev == NULL)
    {
        LCMONIT_FMT_DEBUG("pDev is NULL\n");
        return ERR_PNULL;
    }
    
    sendlen = lc_modmsg_make(MAPMSG_DT_LCM_WORKSTATUS, sendbuf, sizeof(sendbuf));

    LCMONIT_BUF_DEBUG(sendbuf, sendlen, "lc_uart_send: workstate.\n");

    sRecvLen = uart_transmit(pDev, sendbuf, sendlen, sRecvbuf, sizeof(sRecvbuf));
    if(sRecvLen != 0)
    {
        retryCnt = 0;
        ret = lc_modmsg_parse(clientid, sRecvbuf, sRecvLen);
        if(ret != 0)
        {
            LCMONIT_BUF_DEBUG(sRecvbuf, sRecvLen, "lc_modmsg_parse failed,ret(%d).\n", ret);
            return ERR_NORMAL;
        }
    }
    else
    {
        if(++retryCnt >= LCM_SEND_RETRY)
        {
            retryCnt = 0;
            return ERR_INVALID;
        }
    }

    return ret;
}

/**
*********************************************************************
* @brief��     lc_monitor_read
* @param[in]�� NA
* @param[out]��NA
* @return��     void
*********************************************************************
*/
void lc_monitor_read(void *pDev, uint16 dt, DB_CLIENT clientid, int socketfd)
{
    int ret = 0;
    uint8 sendbuf[128] = {0};
    uint16 sendlen = 0;   //���ͱ��ĳ���
    uint8 sRecvbuf[1024] = {0};//���ν���
    uint16 sRecvLen = 0; //���ν��ճ���

    if(pDev == NULL)
    {
        LCMONIT_FMT_DEBUG("pDev is NULL\n");
        return;
    }
    
    sendlen = lc_modmsg_make(dt, sendbuf, sizeof(sendbuf));

    LCMONIT_BUF_DEBUG(sendbuf, sendlen, "lc_uart_send.\n");

    sRecvLen = uart_transmit(pDev, sendbuf, sendlen, sRecvbuf, sizeof(sRecvbuf));
    if(sRecvLen != 0)
    {
        ret = lc_modmsg_parse(clientid, sRecvbuf, sRecvLen);
        if(ret != 0)
        {
            LCMONIT_BUF_DEBUG(sRecvbuf, sRecvLen, "lc_modmsg_parse failed,ret(%d).\n", ret);
        }
    }
}

void lc_workstate_analyze(DB_CLIENT clientid, int socketfd)
{
    uint32 i = 0, j = 0;
    LC_STATUS_STAT_T statusStat[LC_MOD_STATUS_ED];
    LC_WORK_STATUS_T workstatus;   
    char phasename[3] = {'A','B','C'};

    MEMZERO(&statusStat, sizeof(statusStat));
    MEMZERO(&workstatus, sizeof(LC_WORK_STATUS_T));

    //ͳ��ÿ��״̬���ֵĴ���
    for(i = 0; i < g_secNum; i++)
    {
        for(j = 0; j <3; j++)
        {
            switch(g_workstatus[i].loopStatus[j])
            {
                case LC_MOD_STATUS_NR:
                {
                    statusStat[LC_MOD_STATUS_NR].count[j]++;
                    lc_status_cpy(&statusStat[LC_MOD_STATUS_NR].status, &g_workstatus[i], j);
                }break;

                case LC_MOD_STATUS_SC:
                {
                    statusStat[LC_MOD_STATUS_SC].count[j]++;
                    lc_status_cpy(&statusStat[LC_MOD_STATUS_SC].status, &g_workstatus[i], j);
                }break;

                case LC_MOD_STATUS_OC:
                {
                    statusStat[LC_MOD_STATUS_OC].count[j]++;
                    lc_status_cpy(&statusStat[LC_MOD_STATUS_OC].status, &g_workstatus[i], j);
                }break;

                case LC_MOD_STATUS_SE:
                {
                    statusStat[LC_MOD_STATUS_SE].count[j]++;
                    lc_status_cpy(&statusStat[LC_MOD_STATUS_SE].status, &g_workstatus[i], j);
                }break;

                default:
                {
                    if(i == g_secNum -1) //���һ����Ч����ζ�ű���������Ч
                    {
                        LCMONIT_FMT_DEBUG("���������ݾ���Ч.\n");
                        return;
                    }
                }
           }

            if((i+1)%MOD_SAMPLE_INTERV == 0)
            {
                LCMONIT_FMT_DEBUG("sec %d, phase %c, status %d, count %d\n", i+1, phasename[j], g_workstatus[i].loopStatus[j], statusStat[g_workstatus[i].loopStatus[j]].count[j]);
            }
        }

    }


    for(j = 0; j < 3; j++)
    {
        for(i = 0; i < LC_MOD_STATUS_ED; i++)
        {
            if(statusStat[i].count[j] > MOD_SAMPLE_LIMITS) //�����״̬������ֵ
            {
                lc_status_cpy(&workstatus, &statusStat[i].status, j);
                break;
            }
        }
        
        if(i == LC_MOD_STATUS_ED) //û��һ������ֵ����ȡ���һ��״̬
        {            
            lc_status_cpy(&workstatus, &g_workstatus[g_secNum -1], j);
        }
    }

    //������Ϣ�洢
    lc_workstatus_store(clientid, &workstatus);

    return;
}

/**
*********************************************************************
* @brief��     lc_monitor_collect
* @param[in]�� NA
* @param[out]��NA
* @return��     void
*********************************************************************
*/
void lc_monitor_collect(void *pPublic, void *pPrivate)
{
    LC_APPDATA_T *pAppData = (LC_APPDATA_T *)pPublic;
    LC_MONITOR_DATA_T *ptMsgData = (LC_MONITOR_DATA_T *)pPrivate;
    void *pDev = ptMsgData->pUart;
    DB_CLIENT clientid = ptMsgData->clientid;
    int socketfd = ptMsgData->tasksocket;
    int ret = 0;
    static uint8 lc_status = LCM_SEND;
    LC_UART_VDEV_T *pUart = (LC_UART_VDEV_T*)pDev;

    switch(lc_status)
    {
        case LCM_OPEN:
        {
            uart_close(pDev);
            if(0 == access(pUart->desc, F_OK))
            {
                LCMONIT_FMT_DEBUG("%s is exist\n", pUart->desc);
                lc_status = LCM_INIT;
            }
            sleep_no_interrupt(2);
        }break;

        case LCM_INIT:
        {
            ret = uart_open(pDev);
            if(ret == ERR_OK)
            {
                lc_status = LCM_SEND;
            }
        }break;

        case LCM_SEND:
        {
            if(g_secNum % MOD_SAMPLE_INTERV == 0)
            {
                if(pAppData->appType == LC_APP_OLD)
                {
                    ret = lc_workstate_get(pDev, clientid, socketfd);
                }
                else
                {
                    ret = lcm_workstate_get(pDev, clientid, socketfd);
                }
                
                if(ret != ERR_OK)
                {
                    if(ret == ERR_INVALID)
                    {
                        lc_status = LCM_OPEN;
                    }
                }
            }
        }break;

        default:break;
    }

    return;
}

///**
//*********************************************************************
//* @brief��     lc_monitor_collect
//* @param[in]�� NA
//* @param[out]��NA
//* @return��     void
//*********************************************************************
//*/
//void lc_monitor_collect(DB_CLIENT clientid, int socketfd)
//{
////    char dev[32] = {0};  /* ģ�����ô��������� */
//    static uint8 retry = 0;
//    uint16 sendlen = 0;   //���ͱ��ĳ���
//    uint16 revlen = 0;    //����ƴ�Ӻ��ܳ���
//    int singleLen = 0; //���ν��ճ���
//    int ret = 0;
//    uint8 sendbuf[128] = {0};
//    uint8 recvbuf[1024] = {0}; //ƴ�Ӻ󻺴�
//    uint8 sRecvbuf[512] = {0};//���ν���
//
//    switch(g_lc_colstatus)
//    {
//        case LCM_INIT: 
//        {
//            LCMONIT_FMT_DEBUG("LCM_INIT.\n");
//            
//            if(g_lcm_slot != 0)
//            {
//                //lc_uart_uninit(&g_pUart);
//                g_lc_colstatus = LCM_SEND;
//            }
//            else
//            {
//                //�յ�ģ�����¼�
//                if(g_mod_status_chg)
//                {
//                    g_mod_status_chg = FALSE;
//                    g_lc_colstatus = LCM_GETM;
//                }
//            }
//        }break;
//
//        case LCM_SEND:
//        {
//            LCMONIT_FMT_DEBUG("LCM_SEND.\n");
////            if(g_lcm_slot == 0)
////            {
////                g_lc_colstatus = LCM_GETM;
////                break;
////            }
//            
////            //������ܵ��ظ�ע��
////            if(g_pUart == NULL)
////            {
////                sprintf(dev, "%s%d_1",MOD_DEV_DESC_PRFIX, g_lcm_slot);            
////                g_pUart = lc_uart_init(dev); /*�򿪴���*/
////                if(g_pUart == NULL)
////                {
////                    LCMONIT_FMT_DEBUG("lc_uart_init failed dev: %s.\n", dev);
////                    g_lc_colstatus = LCM_GETM;
////                    break;
////                }
////            }
//
//            sendlen = lc_modmsg_make(MAPMSG_DT_LCM_WORKSTATUS, sendbuf, sizeof(sendbuf));
//            //ret = lc_uart_send(g_pUart, sendbuf, sendlen);
//            //ret = uart_transmit(6, sendbuf, sendlen, sRecvbuf, sizeof(sRecvbuf), &revlen);
//            LCMONIT_BUF_DEBUG(sendbuf, sendlen, "lc_uart_send: workstatus. ret(%d)\n", ret);
//            if(ret != 0)
//            {
//                if(++retry > LCM_SEND_RETRY)
//                {
//                    g_lc_colstatus = LCM_INIT;
//                    retry = 0;
//                }
//            }
//            else
//            {
//                //g_lc_colstatus = LCM_RECV;
//                ret = lc_modmsg_parse(clientid, sRecvbuf, revlen);
//                if(ret != 0)
//                {
//                    LCMONIT_BUF_DEBUG(recvbuf, revlen, "lc_modmsg_parse failed,ret(%d).\n", ret);
//                }
//            }
//
//        }; //���ﲻbreak�������ֱ���գ��շ�ֻһ��״̬
//
//        case LCM_RECV: 
//        {
//            usleep(100000);
//            
//            LCMONIT_FMT_DEBUG("LCM_RECV.\n");
//            
//            if(g_lcm_slot == 0)
//            {
//                g_lc_colstatus = LCM_GETM;
//                break;
//            }
//            
//            //singleLen = lc_uart_recv(g_pUart, sRecvbuf, sizeof(sRecvbuf));
//            if(singleLen > 0)
//            {
//                g_recvRetry = 0;
//                LCMONIT_BUF_DEBUG(sRecvbuf, singleLen, "lc_uart_recv frame.\n");
//            }
//            else
//            {
//                if(++g_recvRetry >= LCM_RECV_RETRY)
//                {
//                    g_recvRetry = 0;
//                    //lc_uart_uninit(&g_pUart);
//                    lc_mod_restart_send(g_lcm_slot, 10); //2����δ�յ��ظ���ģ��ϵ�10s
//                }
//            }
//            
//            while(singleLen > 0)
//            {
//                //��������������, �޷�����ƴ��
//                if(revlen >= sizeof(recvbuf))
//                {
//                    LCMONIT_FMT_DEBUG("recvlen(%d) >= sizeof(recvbuf)(%d) \n", revlen, sizeof(recvbuf));
//                    return;
//                }
//
//                //�Ե������Ľ���ƴ�Ӵ���
//                if(singleLen > (sizeof(recvbuf) - revlen))
//                {
//                    singleLen = sizeof(recvbuf) - revlen;
//                    LCMONIT_FMT_DEBUG("singleLen(%d) > (sizeof(recvbuf)(%d) - revlen(%d))\n", singleLen, sizeof(recvbuf), revlen);
//                }
//                
//                memcpy(&recvbuf[revlen], sRecvbuf, singleLen);
//                revlen += singleLen;    
//
//                usleep(100000);
//                
//                //singleLen = lc_uart_recv(g_pUart, sRecvbuf, sizeof(sRecvbuf));
//                if(singleLen > 0)
//                {
//                    LCMONIT_BUF_DEBUG(sRecvbuf, singleLen, "lc_uart_recv continue frame.\n");
//                }             
//            }
//
//            if(revlen != 0)
//            {
//                ret = lc_modmsg_parse(clientid, recvbuf, revlen);
//                if(ret != 0)
//                {
//                    LCMONIT_BUF_DEBUG(recvbuf, revlen, "lc_modmsg_parse failed,ret(%d).\n", ret);
//                }
//            }
//            //Ϊ��ܾ��ͬʱ�ͷſ��ܵ��µĽ�ʬ���̣��շ������о���򿪺��ͷţ����ǰ�ģ��
//            //lc_uart_uninit(pUart);
//            //pUart = NULL;
//            //lc_version_get(g_pUart, clientid, socketfd);
//
//            g_lc_colstatus = LCM_SEND;   
//        }break;
//
//        case LCM_GETM:
//        {
//            LCMONIT_FMT_DEBUG("LCM_GETM.\n");
//            
//            if(g_lcm_slot != 0)
//            {
//                //lc_uart_uninit(&g_pUart);
//                g_lc_colstatus = LCM_SEND;
//                break;
//            }
//
//             //lc_uart_uninit(&g_pUart);
//             
//            //����ִ�У�1s��һ�Σ���10��
//            if(retry++ < LCM_SEND_RETRY )
//            {
//                lc_get_map_slot(socketfd, MOD_LC_TYPE_PRFIX);
//                g_lc_colstatus = LCM_SEND;
//                
//            }
//            else
//            {   
//                g_lc_colstatus = LCM_INIT;
//                retry = 0;
//            }
//        }break;
//
//        default: g_lc_colstatus = LCM_INIT; break;
//    }
//}

#endif

#if DESC("�㷨ģ����Ϣ����",1)

LC_MODPARSE_T g_ctalgParse[] = 
{
    {LCM_DI_WORKSTATUS,  lcm_ctworkstate_parse},
    {LCM_DI_CTVERSION,   lcm_ctversion_parse},
    {LCM_DI_COREVERSION, lcm_coreversion_parse},
    {LCM_DI_FILETRANSMIT,lcm_filetrans_parse},
};

uint32 g_ctalgParse_Max = sizeof(g_ctalgParse)/sizeof(LC_MODPARSE_T);

void hex_to_ascii(char *p, uint8 BcdNum)
{
    if((BcdNum & 0xF0) < 0xA0)
        p[0] = (BcdNum >> 4)+0x30;        //����
    else
    {
        p[0] = (BcdNum >> 4)+0x37;        //��ĸABCDEF
    }
    if((BcdNum & 0x0F) < 0x0A)
        p[1] = (BcdNum & 0x0F)+0x30;        //����
    else
    {
        p[1] = (BcdNum & 0x0F)+0x37;        //��ĸABCDEF
    }
}

/**********************************************************************
* @name      : data_byte_add
* @brief     �����ֽڼ�����
* @param[out]��pBuf   �������ݻ���
* @param[in] ��lengh ��������С
* @param[in] ��value ����
* @return    ����
* @Create    : ����
* @Date      ��2021-05-20
* @Update    :
**********************************************************************/
void data_byte_add(uint8 *pBuf, uint8 lengh, uint8 value)
{
    uint8 i = 0;

    for(i = 0; i < lengh; i++)
    {
        pBuf[i] += value;
    } 
}

/**********************************************************************
* @name      : data_byte_sub
* @brief     �����ֽڼ�����
* @param[out]��pBuf   �������ݻ���
* @param[in] ��lengh ��������С
* @param[in] ��value ����
* @return    ����
* @Create    : ����
* @Date      ��2021-05-20
* @Update    :
**********************************************************************/
void data_byte_sub(uint8 *pBuf, uint8 lengh, uint8 value)
{
    uint8 i = 0;

    for(i = 0; i < lengh; i++)
    {
        pBuf[i] -= value;
    }  
}

uint16 lcm_frame_encap(FRAME_INFO_T *pFrame, uint8 *frameBuf, uint16 maxLen)
{
    uint16  crc = 0;
    uint16 offset = 0;

    //֡��ʼ��־
    frameBuf[offset++] = VF_BEGIN;

    //��ַ��
    memcpy(&frameBuf[offset], pFrame->addr, DL645_ADDR_LEN);
    offset += DL645_ADDR_LEN;
    
    frameBuf[offset++] = VF_BEGIN;

    //������
    frameBuf[offset++]  = pFrame->ctrl.value;

    //������
    frameBuf[offset++] = pFrame->lengh;

    //������
    memcpy(&frameBuf[offset], pFrame->pData, pFrame->lengh);
    offset += pFrame->lengh;

    crc = checksum_calc(&frameBuf[0], offset);
    frameBuf[offset++] = crc;

    //֡������
    frameBuf[offset++] = VF_END;

    return offset;
}


/**********************************************************************
* @name      : lc_make_frame
* @brief     ��ģ��ӿ�Э����֡
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-7-10
* @Update    :
**********************************************************************/
uint16 lcm_modmsg_make(uint32 di, uint8 *sendBuf, uint16 buflen)
{
    uint16 offset = 0;
    uint8 tmpbuf[128] = {0};
    FRAME_INFO_T frameInfo = {0};

    //����������
    //DI
    memcpy(&tmpbuf[offset], &di, sizeof(uint32));
    offset += sizeof(uint32);
    //������
    tmpbuf[offset++] = 0x00;
    tmpbuf[offset++] = 0x00;
    tmpbuf[offset++] = 0x00;
    tmpbuf[offset++] = 0x00;
    //DATA-��
    data_byte_add(&tmpbuf[0], offset, 0x33);
    //��ַ��
    frameInfo.addr[0] = 0x01;
    frameInfo.addr[1] = 0x00;
    frameInfo.addr[2] = 0x00;
    frameInfo.addr[3] = 0x00;
    frameInfo.addr[4] = 0x00;
    frameInfo.addr[5] = 0x00;
    //������    
    frameInfo.ctrl.value = 0x03;
    //������   
    frameInfo.lengh = offset;
    //������
    frameInfo.pData = tmpbuf;

    return lcm_frame_encap(&frameInfo, sendBuf, buflen);
}

/**********************************************************************
* @name      : lcm_upmsg_make
* @brief     ��������Ϣ��֡
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2022-4-16
* @Update    :
**********************************************************************/
uint16 lcm_upmsg_make(LC_FILE_TRANS_T *fileInfo, uint8 *sendBuf, uint16 buflen)
{
    uint16 offset = 0;
    uint8 tmpbuf[256] = {0};
    FRAME_INFO_T frameInfo = {0};
    uint16 encryptPos = 0;
    uint32 di = LCM_DI_FILETRANSMIT;
    uint8 wordPtr = 0; //4�ֽڶ���λ�� 
    uint32 varAddr = 0; //������ַ

    //����������
    
    //DI
    memcpy(&tmpbuf[offset], &di, sizeof(uint32));
    offset += sizeof(uint32);
    //������
    tmpbuf[offset++] = 0x00;
    tmpbuf[offset++] = 0x00;
    tmpbuf[offset++] = 0x00;
    tmpbuf[offset++] = 0x00;

    //DATA
    encryptPos = offset;

    //��֡��
    memcpy(&tmpbuf[offset], &fileInfo->nBlockNum, sizeof(uint16));
    offset += sizeof(uint16);

    //��ǰ֡��
    memcpy(&tmpbuf[offset], &fileInfo->nBlockId, sizeof(uint16));
    offset += sizeof(uint16);

    //���ݳ���
    memcpy(&tmpbuf[offset], &fileInfo->nLengh, sizeof(uint16));
    offset += sizeof(uint16);
    
    //��������
    memcpy(&tmpbuf[offset], fileInfo->fileData, fileInfo->nLengh);
    offset += fileInfo->nLengh;

    //������㷨��������㣨������ʼλ��ӦΪ4�ı�����
    varAddr = (uint32)&tmpbuf[encryptPos];
    wordPtr = (varAddr%4 == 0) ? 0: (4-varAddr%4);

    //���ƶ���4�ֽڶ���λ�ã�������֮�����ƶ�����
    memmove(&tmpbuf[encryptPos+wordPtr], &tmpbuf[encryptPos], offset-encryptPos);
    String_Encrypt(&tmpbuf[encryptPos+wordPtr], offset-encryptPos);
    memmove(&tmpbuf[encryptPos], &tmpbuf[encryptPos+wordPtr], offset-encryptPos);

//    //DATAʹ���㷨�����
//    String_Encrypt(&tmpbuf[encryptPos], offset-encryptPos);

    //������+0x33����
    data_byte_add(&tmpbuf[0], offset, 0x33);
    
    //��ַ��
    frameInfo.addr[0] = 0x01;
    frameInfo.addr[1] = 0x00;
    frameInfo.addr[2] = 0x00;
    frameInfo.addr[3] = 0x00;
    frameInfo.addr[4] = 0x00;
    frameInfo.addr[5] = 0x00;
    //������    
    frameInfo.ctrl.value = 0x03;
    //������   
    frameInfo.lengh = offset;
    //������
    frameInfo.pData = tmpbuf;

    return lcm_frame_encap(&frameInfo, sendBuf, buflen);
}

int lcm_ctworkstate_parse(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata)
{
    uint32 sOffset = 0;
    uint8  i = 0;
    int ret = ERR_OK;
    LC_WORK_STATUS_T workstatus;
    char phasename[3] = {'A','B','C'};

    MEMZERO(&workstatus, sizeof(LC_WORK_STATUS_T));

    LCMONIT_BUF_TRACE(apdu, apduLen, "lc ciphertext apdu.\n");    
    
    //ֻ����APDU�е�DATA,���ܺ��ٽ���
    String_Decrypt(apdu, apduLen);
    
    LCMONIT_BUF_DEBUG(apdu, apduLen, "lc plaintext apdu.\n");

    //�˴�����Ҫ���Ļ��㣬��ģ��Э���һ�£��ڴ洢ʱ����
    for(i = 0; i < 3; i++)
    {
        workstatus.loopStatus[i] = apdu[sOffset++];                     //������·״̬
        
        workstatus.envTemp[i] = BCD_change_to_long(&apdu[sOffset], 4);  //�����������¶�
        sOffset += 2;

        workstatus.freqMax[i] = BCD_change_to_long(&apdu[sOffset], 6);  //Ƶ�����ֵ
        sOffset += 3;        

        workstatus.freqMin[i] = BCD_change_to_long(&apdu[sOffset], 6);  //Ƶ����Сֵ
        sOffset += 3;                

        workstatus.curValue[i] = BCD_change_to_long(&apdu[sOffset], 6); //��Ƶ������Чֵ
        sOffset += 3;   

        workstatus.impeFreq1[i] = BCD_change_to_long(&apdu[sOffset], 4); //��һ�����·�迹Ƶ��
        sOffset += 2;          

        workstatus.impeValue1[i] = BCD_change_to_long(&apdu[sOffset], 8); //��һ�����·�迹ģֵ
        sOffset += 4;           

        workstatus.impeAnge1[i] = BCD_change_to_long(&apdu[sOffset], 4);  //��һ�����·�迹�Ƕ�
        sOffset += 2;              

         workstatus.impeFreq2[i] = BCD_change_to_long(&apdu[sOffset], 4); //�ڶ������·�迹Ƶ��
        sOffset += 2;          

        workstatus.impeValue2[i] = BCD_change_to_long(&apdu[sOffset], 8); //�ڶ������·�迹ģֵ
        sOffset += 4;           

        workstatus.impeAnge2[i] = BCD_change_to_long(&apdu[sOffset], 4);  //�ڶ������·�迹�Ƕ�
        sOffset += 2;              

        workstatus.impeFreq3[i] = BCD_change_to_long(&apdu[sOffset], 4); //���������·�迹Ƶ��
        sOffset += 2;          

        workstatus.impeValue3[i] = BCD_change_to_long(&apdu[sOffset], 8); //���������·�迹ģֵ
        sOffset += 4;           

        workstatus.impeAnge3[i] = BCD_change_to_long(&apdu[sOffset], 4);  //���������·�迹�Ƕ�
        sOffset += 2;                      
    }
    
    memcpy(&g_workstatus[g_secNum-1], &workstatus, sizeof(LC_WORK_STATUS_T));
    
    for(i = 0; i < 3; i++)
    {
        LCMONIT_FMT_DEBUG("secNum %d, phase %c, loopState %d\n",g_secNum, phasename[i], workstatus.loopStatus[i]);
    }

    return ret;
}

int lcm_ctversion_parse(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata)
{
    uint32 sOffset = 0;
    uint8  i = 0, j = 0;
    uint8 tmpDate[16] = {0};
    OOP_CT_VERTIONS_T ctVersion;

    memcpy(&ctVersion, stdata, sizeof(OOP_CT_VERTIONS_T));
    
    LCMONIT_BUF_TRACE(apdu, apduLen, "lc version ciphertext apdu.\n");    
    
    //ֻ����APDU�е�DATA,���ܺ��ٽ���
    String_Decrypt(apdu, apduLen);
    
    LCMONIT_BUF_DEBUG(apdu, apduLen, "lc version plaintext apdu.\n");
    
    for(i = 0; i < 3; i++)
    {
        ctVersion.ver[1+i].szType.nNum = 24; //�������ͺ�
        memcpy_r(&ctVersion.ver[1+i].szType.value, &apdu[sOffset], ctVersion.ver[1+i].szType.nNum);
        sOffset += ctVersion.ver[1+i].szType.nNum;

        //������ID, HEX
        ctVersion.ver[1+i].szId.nNum = 24; //������ID
        memcpy_r(&ctVersion.ver[1+i].szId.value, &apdu[sOffset], ctVersion.ver[1+i].szId.nNum);
        sOffset += ctVersion.ver[1+i].szId.nNum;

        //���̴���
        memcpy_r(&ctVersion.ver[1+i].szFactory.value, &apdu[sOffset], 4);
        sOffset += 4;

        //����汾��
        memcpy_r(&ctVersion.ver[1+i].szSoftVer.value, &apdu[sOffset], 4);
        sOffset += 4;

        //����汾����:BCB��
        memcpy_r(tmpDate, &apdu[sOffset], 3);
        for(j = 0; j < 3; j++)
        {
            hex_to_ascii(&ctVersion.ver[1+i].szSoftDate.value[j*2], tmpDate[j]);
        }
        sOffset += 3;      

        //Ӳ���汾��
        memcpy_r(&ctVersion.ver[1+i].szHardVer.value, &apdu[sOffset], 4);
        sOffset += 4;

        //Ӳ���汾����:BCB��
        memcpy_r(tmpDate, &apdu[sOffset], 3);
        for(j = 0; j < 3; j++)
        {
            hex_to_ascii(&ctVersion.ver[1+i].szHardDate.value[j*2], tmpDate[j]);
        }        
        sOffset += 3;          

        //������չ��Ϣ
        memcpy_r(&ctVersion.ver[1+i].szExtend.value, &apdu[sOffset], 8);
        sOffset += 8;              
    }

    memcpy(stdata, &ctVersion, sizeof(OOP_CT_VERTIONS_T));
    
    return ERR_OK;
}

int lcm_coreversion_parse(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata)
{
    uint32 sOffset = 0;
    uint8  i = 0;
    uint8 tmpDate[16] = {0};    
    OOP_CT_VERTIONS_T ctVersion;

    memcpy(&ctVersion, stdata, sizeof(OOP_CT_VERTIONS_T));
    
    LCMONIT_BUF_TRACE(apdu, apduLen, "lc version ciphertext apdu.\n");    
    
    //ֻ����APDU�е�DATA,���ܺ��ٽ���
    String_Decrypt(apdu, apduLen);
    
    LCMONIT_BUF_DEBUG(apdu, apduLen, "lc version plaintext apdu.\n");
    
    ctVersion.ver[0].szType.nNum = 24; //�㷨���ͺ�
    memcpy_r(&ctVersion.ver[0].szType.value, &apdu[sOffset], ctVersion.ver[0].szType.nNum);
    sOffset += ctVersion.ver[0].szType.nNum;

    //�㷨��ID,�˴�698������Ҫ����octet-string
    ctVersion.ver[0].szId.nNum = 24; 
    memcpy_r(&ctVersion.ver[0].szId.value, &apdu[sOffset], ctVersion.ver[0].szId.nNum);
    sOffset += ctVersion.ver[0].szId.nNum;    

    //���̴���
    memcpy_r(&ctVersion.ver[0].szFactory.value, &apdu[sOffset], 4);
    sOffset += 4;

    //����汾��
    memcpy_r(&ctVersion.ver[0].szSoftVer.value, &apdu[sOffset], 4);
    sOffset += 4;

    //����汾����:BCD��
    memcpy_r(tmpDate, &apdu[sOffset], 3);
    for(i = 0; i < 3; i++)
    {
        hex_to_ascii(&ctVersion.ver[0].szSoftDate.value[i*2], tmpDate[i]);
    }    
    sOffset += 3;      

    //Ӳ���汾��
    memcpy_r(&ctVersion.ver[0].szHardVer.value, &apdu[sOffset], 4);
    sOffset += 4;

    //Ӳ���汾����:BCD��
    memcpy_r(tmpDate, &apdu[sOffset], 3);
    for(i = 0; i < 3; i++)
    {
        hex_to_ascii(&ctVersion.ver[0].szHardDate.value[i*2], tmpDate[i]);
    }
    sOffset += 3;          

    //������չ��Ϣ
    memcpy_r(&ctVersion.ver[0].szExtend.value, &apdu[sOffset], 8);
    sOffset += 8;              

    memcpy(stdata, &ctVersion, sizeof(OOP_CT_VERTIONS_T));
    
    return ERR_OK;
}

int lcm_filetrans_parse(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata)
{
    uint32 sOffset = 0;
    uint16 blockID = 0;
    LC_FILE_TRANS_T *pFileInfo = (LC_FILE_TRANS_T *)stdata;

    UPDATE_BUF_TRACE(apdu, apduLen, "lc filetrans ciphertext apdu.\n");    
    
    //ֻ����APDU�е�DATA,���ܺ��ٽ���
    String_Decrypt(apdu, apduLen);
    
    UPDATE_BUF_DEBUG(apdu, apduLen, "lc filetrans plaintext apdu.\n");

    memcpy(&blockID, &apdu[sOffset], sizeof(uint16));
    sOffset += sizeof(uint16);

    if(pFileInfo->nBlockId != blockID)
    {
        return ERR_NORMAL;
    }
    
    return ERR_OK;
}


/**********************************************************************
* @name      : lcm_frame_decap
* @brief     ��������Ϣ������ MAPMSG_INFO_T
* @param[in] ��uint8 *pBuf              ���ܵ��ı���
               uint16 bufLen            ���ĳ���
* @param[out]��MAPMSG_INFO_T **ppMsgOut ������������Ϣ
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��2020-1-3
* @Update    :
**********************************************************************/
int lcm_frame_decap(FRAME_INFO_T *pFrame, uint8 *pBuf, uint16 bufLen)
{
    uint8 readcs = 0;
    uint8 calccs = 0;

    if(bufLen < DL645_FRAME_MIN)
    {    
        LCMONIT_FMT_DEBUG("frame lengh check failed, lengh-%d < minlengh-%d.\n", bufLen, DL645_FRAME_MIN);
        return -1;
    }
        
    //��ַ��
    memcpy(pFrame->addr, &pBuf[DL645_ADDR], DL645_ADDR_LEN);
    
    //������
    pFrame->ctrl.value = pBuf[DL645_CTRL];

    //������
    pFrame->lengh = pBuf[DL645_LEGH];

    //������
    memcpy(pFrame->pData, &pBuf[DL645_DATA], pFrame->lengh);

    //У����
    readcs = pBuf[DL645_DATA+pFrame->lengh];
    calccs = checksum_calc(&pBuf[0], DL645_DATA+pFrame->lengh);
    if(readcs != calccs)
    {
        pFrame->lengh = 0;
        LCMONIT_FMT_DEBUG("checksum_calc failed, readcs-%d != calccs-%d.\n", readcs, calccs);
        return -2;
    }

    //�������ֽڼ�33H����
    data_byte_sub(pFrame->pData, pFrame->lengh, 0x33);

    return 0;
}

/**********************************************************************
* @name      : lcm_modmsg_parse
* @brief     ���㷨��Э�����
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-7-10
* @Update    :
**********************************************************************/
int lcm_modmsg_parse(DB_CLIENT clientid, uint8 *recvBuf, uint16 buflen, void *stdata)
{
    int            ret = 0;
    uint16         offset = 0;
    FRAME_INFO_T   frameInfo = {0};
    uint32         di = 0;
    uint8          i = 0;
    int            tempret = 0;
    uint32 sOffset = 0;
    uint8 temp[DL645_DATA_MAX] = {0}; //���ձ��ĵ�������

    frameInfo.pData = temp;
        
    //�ҵ�֡��ʼλ
    while(offset < buflen)
    {
        if(VF_BEGIN == recvBuf[offset])
        {
            tempret = lcm_frame_decap(&frameInfo, recvBuf + offset, buflen - offset);
            if(tempret != 0)
            {
                LCMONIT_BUF_DEBUG(recvBuf + offset, buflen - offset, "lcm_frame_decap failed, tempret(%d)\n", tempret);
            }
            break;
        }
        offset++;
    }

    if(frameInfo.lengh == 0)
    {
        LCMONIT_BUF_DEBUG(temp, sOffset, "check null pointer.\n");
        return ERR_PNULL;
    }

    //���ܵ�һ�������Ƕ�֡
    memcpy(&di, &temp[sOffset], sizeof(uint32));
    sOffset += sizeof(uint32);

    for(i = 0; i < g_ctalgParse_Max; i++)
    {
        if((g_ctalgParse[i].di == di)&&(g_ctalgParse[i].apduGet != NULL))
        {
            ret = g_ctalgParse[i].apduGet(clientid, &temp[sOffset], frameInfo.lengh-sOffset, stdata);
            break;
        }
    }

    if(i == g_ctalgParse_Max)
    {
        ret = ERR_OPTION;
    }
    
    return ret;
}

#endif

#if DESC("�㷨ģ���շ�",1)

/**
*********************************************************************
* @brief��     lc_version_get
* @param[in]�� NA
* @param[out]��NA
* @return��     void
*********************************************************************
*/
int lcm_version_get(void *pDev, DB_CLIENT clientid, int socketfd)
{
    int ret = 0;
    uint8 sendbuf[128] = {0};
    uint16 sendlen = 0;   //���ͱ��ĳ���
    uint8 sRecvbuf[1024] = {0};//���ν���
    uint16 sRecvLen = 0; //���ν��ճ���
    OOP_CT_VERTIONS_T ctVersion;
    LC_UART_VDEV_T *tmpUart = (LC_UART_VDEV_T *)pDev;
    uint32 useTime = 0;
    static uint16 retryCnt = 0;

    if(pDev == NULL)
    {
        LCMONIT_FMT_DEBUG("pDev is NULL\n");
        return ERR_PNULL;
    }

    MEMZERO(&ctVersion, sizeof(OOP_CT_VERTIONS_T));
    ctVersion.nNum = 4;
        
    sendlen = lcm_modmsg_make(LCM_DI_CTVERSION, sendbuf, sizeof(sendbuf));

    LCMONIT_BUF_DEBUG(sendbuf, sendlen, "lc_uart_send: ctversion.\n");

    sRecvLen = uart_transmit(pDev, sendbuf, sendlen, sRecvbuf, sizeof(sRecvbuf));
    if(sRecvLen != 0)
    {
        retryCnt = 0;
        ret = lcm_modmsg_parse(clientid, sRecvbuf, sRecvLen, &ctVersion);
        if(ret != 0)
        {
            LCMONIT_BUF_DEBUG(sRecvbuf, sRecvLen, "lc_modmsg_parse failed,ret(%d).\n", ret);
            return ERR_NORMAL;
        }
    }
    else
    {
        if(++retryCnt >= LCM_SEND_RETRY)
        {
            retryCnt = 0;
            return ERR_INVALID;
        }
    }

    memset(sendbuf, 0, sizeof(sendbuf));
    sendlen = lcm_modmsg_make(LCM_DI_COREVERSION, sendbuf, sizeof(sendbuf));

    LCMONIT_BUF_DEBUG(sendbuf, sendlen, "lc_uart_send: coreversion.\n");

    sRecvLen = uart_transmit(pDev, sendbuf, sendlen, sRecvbuf, sizeof(sRecvbuf));
    if(sRecvLen != 0)
    {
        retryCnt = 0;
        ret = lcm_modmsg_parse(clientid, sRecvbuf, sRecvLen, &ctVersion);
        if(ret != 0)
        {
            LCMONIT_BUF_DEBUG(sRecvbuf, sRecvLen, "lc_modmsg_parse failed,ret(%d).\n", ret);
            return ERR_NORMAL;
        }
    }
    else
    {
        if(++retryCnt >= LCM_SEND_RETRY)
        {
            retryCnt = 0;
            return ERR_INVALID;
        }
    }

    useTime = (tmpUart->recvTm.tv_sec - tmpUart->sendTm.tv_sec)*1000 + (tmpUart->recvTm.tv_nsec - tmpUart->sendTm.tv_nsec)/1000000;

    LCMONIT_FMT_DEBUG("comm use: %d ms, success rate: recv-%d/send-%d = %.2f%%.\n", 
        useTime, tmpUart->recvCnt, tmpUart->sendCnt, tmpUart->sendCnt==0 ? 0: tmpUart->recvCnt*100.0/tmpUart->sendCnt);

    //�汾��Ϣ�洢
    ret = lc_versioninfo_store(clientid, &ctVersion); 

    return ret;
}

/**
*********************************************************************
* @brief��     lc_workstate_get
* @param[in]�� NA
* @param[out]��NA
* @return��     void
*********************************************************************
*/
int lcm_workstate_get(void *pDev, DB_CLIENT clientid, int socketfd)
{
    int ret = 0;
    uint8 sendbuf[128] = {0};
    uint16 sendlen = 0;   //���ͱ��ĳ���
    uint8 sRecvbuf[1024] = {0};//���ν���
    uint16 sRecvLen = 0; //���ν��ճ���
    static uint16 retryCnt = 0;
    LC_UART_VDEV_T *tmpUart = (LC_UART_VDEV_T *)pDev;
    uint32 useTime = 0;

    if(pDev == NULL)
    {
        LCMONIT_FMT_DEBUG("pDev is NULL\n");
        return ERR_PNULL;
    }
    
    sendlen = lcm_modmsg_make(LCM_DI_WORKSTATUS, sendbuf, sizeof(sendbuf));

    LCMONIT_BUF_DEBUG(sendbuf, sendlen, "lc_uart_send: workstate.\n");

    sRecvLen = uart_transmit(pDev, sendbuf, sendlen, sRecvbuf, sizeof(sRecvbuf));
    if(sRecvLen != 0)
    {
        retryCnt = 0;
        ret = lcm_modmsg_parse(clientid, sRecvbuf, sRecvLen, NULL);
        if(ret != 0)
        {
            LCMONIT_BUF_DEBUG(sRecvbuf, sRecvLen, "lc_modmsg_parse failed,ret(%d).\n", ret);
            return ERR_NORMAL;
        }
    }
    else
    {
        if(++retryCnt >= LCM_SEND_RETRY)
        {
            retryCnt = 0;
            return ERR_INVALID;
        }
    }

    useTime = (tmpUart->recvTm.tv_sec - tmpUart->sendTm.tv_sec)*1000 + (tmpUart->recvTm.tv_nsec - tmpUart->sendTm.tv_nsec)/1000000;

    LCMONIT_FMT_DEBUG("comm use: %d ms, success rate: recv-%d/send-%d = %.2f%%.\n", 
        useTime, tmpUart->recvCnt, tmpUart->sendCnt, tmpUart->sendCnt==0 ? 0: tmpUart->recvCnt*100.0/tmpUart->sendCnt);

    return ret;
}

#endif

#if DESC("��������",PRODUCT_SCU)

uint8  g_fileData[LC_UPPACK_SIZE];
/**********************************************************************
* @name      : lc_upfile_scan
* @brief     �������ļ�ɨ��
* @param[in] ��void *pArg     �߳̾��
               void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : ����
* @Date      ��2021-09-07
* @Update    :
**********************************************************************/
int lc_upfile_scan(void *pPublic, void *pPrivate)
{
    LC_MONITOR_DATA_T *ptData = (LC_MONITOR_DATA_T *)pPrivate;
    LC_FILE_TRANS_T *pFileInfo = ptData->fileInfo;
    int size = 0;

    if(0 == access(LC_UPDATE_FILE, F_OK))
    {
        size = file_size_get(LC_UPDATE_FILE);
        UPDATE_FMT_DEBUG("��⵽�����ļ� %s����С %d B, ��ʼ�Ի�·Ѳ�칦�ܵ�Ԫ����.\n", LC_UPDATE_FILE, size);
        
        memset(pFileInfo, 0, sizeof(LC_FILE_TRANS_T));
        pFileInfo->fileData = g_fileData;
    
        pFileInfo->nEndLengh = size % LC_UPPACK_SIZE;
        pFileInfo->nBlockNum = size/LC_UPPACK_SIZE;
        
        if(pFileInfo->nEndLengh)
        {
            pFileInfo->nBlockNum += 1;
        }
    
        ptData->fd = -1;
        ptData->fd = open(LC_UPDATE_FILE, O_RDONLY);
        if(ptData->fd < 0)
        {
            UPDATE_FMT_DEBUG("lc file open failed, %s.\n", strerror(errno));
            return ERR_NORMAL;
        }

        return ERR_OK;
    }

    return ERR_NORMAL;
}

/**********************************************************************
* @name      : lc_update_trans
* @brief     ������������
* @param[in] ��void *pPublic    ��������
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-09-07
* @Update    :
**********************************************************************/
int lc_update_trans(void *pPublic, void *pPrivate)
{
    LC_MONITOR_DATA_T *ptData = (LC_MONITOR_DATA_T *)pPrivate;
    LC_FILE_TRANS_T *pFileInfo = ptData->fileInfo;
    LC_UART_VDEV_T *pUart = ptData->pUart;
    uint8 sendbuf[256] = {0};
    uint16 sendLen = 0;
    int recvLen = 0;
    uint8 recvBuf[256] = {0};
    int ret = 0;
     
    lseek(ptData->fd, pFileInfo->nBlockId*LC_UPPACK_SIZE, SEEK_SET);

    if(pFileInfo->nBlockId == (pFileInfo->nBlockNum-1))
    {
        pFileInfo->nLengh = pFileInfo->nEndLengh;
    }
    else
    {
        pFileInfo->nLengh = LC_UPPACK_SIZE;
    }

    read(ptData->fd, pFileInfo->fileData, pFileInfo->nLengh);

    sendLen = lcm_upmsg_make(pFileInfo, sendbuf, 256);
    UPDATE_BUF_DEBUG(sendbuf, sendLen, "��֡��[%d] ֡���[%d] ����:\n", pFileInfo->nBlockNum , pFileInfo->nBlockId);

    recvLen = uart_transmit(pUart, sendbuf, sendLen, recvBuf, sizeof(recvBuf));
    if(recvLen != 0)
    {
        ret = lcm_modmsg_parse(0, recvBuf, recvLen, pFileInfo);
        if(ret != 0)
        {
            UPDATE_BUF_DEBUG(recvBuf, recvLen, "lc_update_parse failed,ret(%d).\n", ret);
            return ret;
        }
    }

    UPDATE_FMT_DEBUG("֡���[%d] ���ͳɹ�.\n",pFileInfo->nBlockId);

    pFileInfo->nBlockId++;

    return 0;
}

/**********************************************************************
* @name      : lc_transfer_scan
* @brief     ��͸��ɨ��
* @param[in] ��void *pArg     �߳̾��
               void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : ����
* @Date      ��2021-09-07
* @Update    :
**********************************************************************/
int lc_data_transfer(void *pPublic, void *pPrivate)
{
    LC_MONITOR_DATA_T *ptData = (LC_MONITOR_DATA_T *)pPrivate;
    LC_UART_VDEV_T *pUart = ptData->pUart;
    uint8 sendbuf[288] = {0};
    uint16 sendLen = 0;
    UART_SEND_DATA_T tUratSend;
    uint8   dataBuf[512] = {0}; 
    uint32  dataLen = 0;

    memcpy(sendbuf, ptData->transBuf, ptData->transLen);
    sendLen = ptData->transLen;

    LCMONIT_BUF_DEBUG(sendbuf, sendLen, "͸������:");

    tUratSend.datalen = uart_transmit(pUart, sendbuf, sendLen, tUratSend.data, UART_DATA_MAXLEN);
    if(tUratSend.datalen != 0)
    {
        LCMONIT_BUF_DEBUG(tUratSend.data, tUratSend.datalen, "͸������:");
        
        tUratSend.port = ptData->transport;
        dataLen = uart_axdr_data_send_req_package(&tUratSend, dataBuf, 512);

        lc_send_uartmsg(0, UART_IOP_SEND_DATA, dataBuf, dataLen);
    }

    return 0;
}

#endif

#if DESC("��·Ѳ��״̬��",1)

/**
*********************************************************************
* @brief��     lc_monitor_collect
* @param[in]�� NA
* @param[out]��NA
* @return��     void
*********************************************************************
*/
void lc_monitor_deal(void *pPublic, void *pPrivate)
{
//    LC_APPDATA_T *pAppData = (LC_APPDATA_T *)pPublic;
    LC_MONITOR_DATA_T *ptMsgData = (LC_MONITOR_DATA_T *)pPrivate;
    void *pDev = ptMsgData->pUart;
    DB_CLIENT clientid = ptMsgData->clientid;
    int socketfd = ptMsgData->tasksocket;
    int ret = 0;
    LC_UART_VDEV_T *pUart = (LC_UART_VDEV_T*)pDev;
    static uint8 retry = 0;
#ifdef PRODUCT_SCU    
    LC_FILE_TRANS_T *pFileInfo = ptMsgData->fileInfo;

    //�ļ�������ʼ����
    if(ERR_OK == lc_upfile_scan(pPublic, pPrivate))
    {
        ptMsgData->status = LCM_FUPG;
    }  

    //��⴮���Ƿ��쳣
    if(uart_param_get(pUart->fd, &pUart->param) == FALSE)
    {
        ptMsgData->status = LCM_OPEN;
    } 

    //����Ƿ������ģʽ����
    if((g_secNum % MOD_SAMPLE_INTERV == 0) && (ptMsgData->slavereg != UART_RST_SUCCESS))
    {
        lc_uart_mode_check(pPrivate);
    }      
#endif

#ifdef PRODUCT_ECU    

    //�յ�ģ��γ��¼�
    if((g_lcm_slot == 0) && (ptMsgData->status != LCM_GETM))
    {
        ptMsgData->status = LCM_GETM;
        uart_close(pDev);
    }
#endif
                
    switch(ptMsgData->status)
    {
        case LCM_IDLE:
        {      
            LCMONIT_FMT_TRACE("LCM_IDLE.\n");
            //�յ�ģ������¼�
            if(g_mod_status_chg)
            {
                g_mod_status_chg = FALSE;
                ptMsgData->status = LCM_GETM;
            }

        }break;

        case LCM_OPEN:
        {
            LCMONIT_FMT_TRACE("LCM_OPEN.\n");
            uart_close(pDev);

            if(0 == access(pUart->desc, F_OK))
            {
                LCMONIT_FMT_DEBUG("%s is exist\n", pUart->desc);

                sleep_no_interrupt(2);

                ret = uart_open(pDev);
                if(ret == ERR_OK)
                {
                    ptMsgData->status = LCM_INIT;
                }
            }
        }break;

        case LCM_INIT:
        {
            LCMONIT_FMT_TRACE("LCM_INIT.\n");
#ifdef PRODUCT_ECU   
            ret = lc_version_get(pDev, clientid, socketfd);
#else
            ret = lcm_version_get(pDev, clientid, socketfd);
#endif
            if(ret != ERR_OK)
            {
                if(ret == ERR_INVALID)
                {
                    ptMsgData->status = LCM_OPEN;
                }
                else
                {
                    sleep_no_interrupt(5);
                }
            }
            else
            {
                ptMsgData->status = LCM_SEND;
            }
        }break;

        case LCM_SEND:
        {
            LCMONIT_FMT_TRACE("LCM_SEND.\n");
            if(g_secNum % MOD_SAMPLE_INTERV == 0)
            {
#ifdef PRODUCT_ECU 
                ret = lc_workstate_get(pDev, clientid, socketfd);
#else
                ret = lcm_workstate_get(pDev, clientid, socketfd);
#endif
                if(ret != ERR_OK)
                {
                    if(ret == ERR_INVALID)
                    {
                        ptMsgData->status = LCM_OPEN;
                    }
                }
            }

            if(g_secNum % MOD_SAMPLE_PERIOD == 0)
            {
                //�״ΰ汾δ��ȡ�ɹ������Զ�ȡ�汾
                if(g_verNeedRetry == TRUE)
                {
                    g_verNeedRetry = FALSE;
#ifdef PRODUCT_ECU   
                    ret = lc_version_get(pDev, clientid, socketfd);
#else
                    ret = lcm_version_get(pDev, clientid, socketfd);
#endif
                }  
            }
        }break;
            
        case LCM_GETM:
        {
            LCMONIT_FMT_TRACE("LCM_GETM.\n");
            if(g_lcm_slot != 0)
            {
                sprintf(pUart->halname, "%s%d_1",MOD_DEV_DESC_PRFIX, g_lcm_slot); 
                ret = uart_open(pDev);
                if(ret == ERR_OK)
                {
                    ptMsgData->status = LCM_INIT;
                }
            }
            
            //����ִ�У�1s��һ�Σ���10��,�鲻���ȴ�ģ�����
            if(retry++ < 10 )
            {
                lc_get_map_slot(socketfd, MOD_LC_TYPE_PRFIX);                
            }
            else
            {   
                ptMsgData->status = LCM_IDLE;
                retry = 0;
            }
        }break;            
#ifdef PRODUCT_SCU        
        case LCM_FUPG:
        {
            LCMONIT_FMT_TRACE("LCM_FUPG.\n");
            if(pFileInfo->nBlockId >= pFileInfo->nBlockNum)
            {            
                remove(LC_UPDATE_FILE);
                UPDATE_FMT_DEBUG("��·Ѳ�칦�ܵ�Ԫ�������.\n");
                ptMsgData->status = LCM_INIT;
            }
            else
            {
                lc_update_trans(pPublic, pPrivate);
                ptMsgData->status = LCM_FUPG;
            }
        }break;

        case LCM_TRAN:
        {
            LCMONIT_FMT_TRACE("LCM_TRAN.\n");
            lc_data_transfer(pPublic, pPrivate);
            ptMsgData->status = LCM_SEND;
        }break;
#endif
        default:break;
    }
    
    return;
}

#endif

