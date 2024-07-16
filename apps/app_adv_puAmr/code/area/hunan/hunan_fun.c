
#ifdef AREA_HUNAN

#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopStorage.h"
#include "area.h"
#include "hal.h"
#include "adc.h"
#include "power.h"
#include "../../Common698/AppCommon.h"
#include "../../event/Event.h"

//���汾��Ϣ��ȫ�ֱ�������ʽд����������Ĵ����У�ͳһ�� area.h ������
#ifdef ADV_PU_AMR
FACTORY_VER_TMP_T g_tVer = 
{
    "XXXX",      /**< ���̴���           */
    "CM01",      /**< ����汾��         */
    "220811",    /**< ����汾����       */
    "CC01",      /**< Ӳ���汾��         */
    "220621",    /**< Ӳ���汾����       */
    "CM01-01",   /**< ������չ��Ϣ       */
};
#elif ADV_ST_AMR
    #ifdef PRODUCT_ECU
    //��Դ������
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< ���̴���           */
        "EM01",      /**< ����汾��         */
        "220619",    /**< ����汾����       */
        "ER01",      /**< Ӳ���汾��         */
        "220619",    /**< Ӳ���汾����       */
        "EM01-01",   /**< ������չ��Ϣ       */
    };
    #elif PRODUCT_ZCU_1
    //ר��
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< ���̴���           */
        "1M01",      /**< ����汾��         */
        "210207",    /**< ����汾����       */
        "Z101",      /**< Ӳ���汾��         */
        "200624",    /**< Ӳ���汾����       */
        "1M01-01",   /**< ������չ��Ϣ       */
    };
    #elif PRODUCT_ZCU_2
    //ר��3��
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< ���̴���           */
        "2M01",      /**< ����汾��         */
        "220715",    /**< ����汾����       */
        "Z301",      /**< Ӳ���汾��         */
        "220715",    /**< Ӳ���汾����       */
        "2M01-01",   /**< ������չ��Ϣ       */
    };
    #elif PRODUCT_ZCU_3
    //ר��3��
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< ���̴���           */
        "3M01",      /**< ����汾��         */
        "220716",    /**< ����汾����       */
        "Z301",      /**< Ӳ���汾��         */
        "220715",    /**< Ӳ���汾����       */
        "3M01-01",   /**< ������չ��Ϣ       */
    };
    #endif
#endif


#define USERTYPE_FRAME_BREAKER      212    //��ܶ�·�� �û�����212 �����ܿ���
#define USERTYPE_LOW_BREAKER        213     //��ѹ��·�� ��֧��
#define USERTYPE_METERBOX_BREAKER   220    //�����·��

#if DESC("ͣ�紦��",1)

uint16 g_val_a[OOP_MAX_POWERCUT_MET];     //ͣ��ʱ�����A���ѹ

#endif

extern int IniEventCommInfo(EVENT_COMM_INFO *eventCommInfo);
extern int advance_send_pdsManager(uint16 label, uint16 iop, uint8* playload, uint16 length);

uint16 get_link_normal_data(int clientid,OOP_OAD_U oad,uint8* outbuf)
{
    int ret;
    //���ȡ�ն����� ��ȡ��������

    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 1;
    NormalOad.oad = oad;
    NormalOad.infoNum = 0;
    uint8 buf[1000];

    ret = db_read_nomal(clientid,&NormalOad,sizeof(buf),buf,&len);
    if(ret!=0)
    {
        EVENT_FMT_DEBUG("get_link_normal_data  oad=%08X error ret%d\n",  oad.value, ret);
        return 0;
    }
    memcpy(outbuf,buf,len);
    return len;
    
}
uint16 get_link_record_data(int clientid,uint8 logicid,uint16 infoNum,uint8* Addr,OOP_OAD_U oad,uint8* outbuf)
{
    int ret;
    //���ȡ�ն����� ��ȡ��������
    if(infoNum==0)
    {
        uint32 len=0;
        NOMAL_OAD_T NormalOad;
        NormalOad.logicId = 1;
        NormalOad.oad = oad;
        NormalOad.infoNum = 0;
        uint8 buf[1000];

        ret = db_read_nomal(clientid,&NormalOad,sizeof(buf),buf,&len);
        if(ret!=0)
        {
            EVENT_FMT_DEBUG("get_link_record_data infoNum=%u oad=%08X error ret%d\n", infoNum, oad.value, ret);
            return 0;
        }
        memcpy(outbuf,buf,len);
        return len;
    }
    READ_RECORD_T       startReadRecord;
    RECORD_UNIT_T       stepRecordOut;
    memset(&startReadRecord,0,sizeof(READ_RECORD_T));
    memset(&stepRecordOut,0,sizeof(RECORD_UNIT_T));
    startReadRecord.recordOAD.optional = 0;
    startReadRecord.recordOAD.road.oadCols.nNum = 1;
    startReadRecord.recordOAD.road.oadCols.oad[0] = oad;
    startReadRecord.recordOAD.logicId = logicid;
    startReadRecord.recordOAD.infoNum = infoNum;
    startReadRecord.cType = 0x00;
    startReadRecord.cStart = 0;
    startReadRecord.cEnd = time(NULL) + COMPENSATION_TIME;
    startReadRecord.sortType = 1;
    startReadRecord.MAC.nNum = 6;
    memcpy(startReadRecord.MAC.value, Addr, 6);
    ret = db_read_record(clientid, &startReadRecord, DATA_LEN_MAX, &stepRecordOut);
    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("get_link_record_data infoNum=%u oad=%08X error ret%d\n", infoNum, oad.value, ret);
        return 0;
    } 
    memcpy(outbuf,stepRecordOut.record[0].outDataBuf,stepRecordOut.record[0].outDatalen);
    return stepRecordOut.record[0].outDatalen;
}

/*********************************************************************
�������ܵ�Ԫ��
������������·���ر�λ�¼� 3360
����������������
�����������
��������ֵ����
�ڲ��������̣�
********************************************************************/
int CheckERC_3360(int clientid,uint8* databuf,uint16 datalen)
{

    //uint8 i;
    int ret;
    if(!event_3360.eventCommInfo.checkflag)
    {
        EVENT_FMT_LOGCC("CheckERC_3360 checkflag 0\n");
        return -1 ;
    }
    OOP_EVTSOURCE_T     eventSource; //�¼�����Դ
	OOP_EVTBUF_T        eventData;   //�¼���������
	memset(&eventSource, 0, sizeof(OOP_EVTSOURCE_T));
	memset(&eventData, 0, sizeof(OOP_EVTBUF_T));
    uint8 state; //���طֺ�״̬ 1 �� 2��
    if(datalen!= sizeof(uint8))
    {
        EVENT_FMT_DEBUG("CheckERC_3360 datalen != 1\n");
        return -1 ;
    }
    state = databuf[0];
    if(state !=1 && state !=2)
    {
        EVENT_FMT_DEBUG("CheckERC_3360 state= %d �ֺ�״̬Ӧ��Ϊ1����2 \n",state);
        return -1 ;
    }
    if(event_3360.breakstate==0)
    {
        event_3360.breakstate = state;
        EVENT_FMT_DEBUG("CheckERC_3360 state= %d  \n",state);
        return 0;
    }
    if(state ==event_3360.breakstate)
    {
        EVENT_FMT_DEBUG("CheckERC_3360 δ������λ�¼� ��ǰ״̬ %d  \n",state);
        return 0;
    }
    //������λ ��¼��Ŀǰ״̬ �������¼�
    event_3360.breakstate = state;
    //�¼�����Դ unsigned ��ע��0Ϊ��·�ܿ��أ�1��2��3�������n·��֧��·����
    //ȫ����0
	eventSource.choice = DT_UNSIGNED;
    eventSource.nValue = 0;
    //��Ĭ�Ϲ�����������
	eventData.cols.nNum = 1;
	eventData.oadNum = 1;
	//eventData.cols.oad[0].value = 0x40010200;
	eventData.cols.oad[0].value = 0x28000200;
//	eventData.cols.oad[2].value = 0x20000200;
//	eventData.cols.oad[3].value = 0x20010200;
//	eventData.cols.oad[4].value = 0x20010700;
//	eventData.idLen[0] = sizeof(OOP_OCTETVAR16_T);
	eventData.idLen[0] = sizeof(uint8);
//	eventData.idLen[2] = sizeof(OOP_WORD3V_T);
//	eventData.idLen[3] =sizeof(OOP_INT3V_T);
//	eventData.idLen[4] =sizeof(OOP_INT3V_T);
	
//	OOP_OCTETVAR16_T addr;
//	memset(&addr,0x00,sizeof(OOP_OCTETVAR16_T));
//	addr.nNum = 6;
//	memcpy(addr.value,oopMeter.basic.tsa.add,6);
//	memcpy(eventData.buf,&addr,eventData.idLen[0]);
    eventData.buf[0] = state;
	eventData.len+=eventData.idLen[0];
//	uint16 len;
//    for(i=1;i<5;i++)
//    {
//    	len = get_link_record_data( clientid, logicid,mtr_id, oopMeter.basic.tsa.add,eventData.cols.oad[i],eventData.buf+eventData.len); 
//        if(len == 0)
//        {
//            EVENT_FMT_DEBUG("��ȡ��������0x%08x ����ʧ�� \n",eventData.cols.oad[i]);
//        }
//    	eventData.len+=eventData.idLen[i];
//	}
	ret = WriteEventToDB(clientid, 0, &event_3360.eventCommInfo, eventSource, eventData, 0);
    if (ret < ERR_OK)
	{
		EVENT_FMT_DEBUG("CheckERC_3360 ��¼ʧ�ܷ���%d\n", ret);
		return -1;
	}
	return 0;
}

/*********************************************************************
�������ܵ�Ԫ���¼�����ģ���ṩӦ�ò�������ʹ��
�������������������Ļ�ȡ��·���ر�λ�¼�������д��ȫ�ֱ����С�
���������
���������
  event_3360   ��·���ر�λ�¼�ȫ�ֱ���
��������ֵ��
  0     �ɹ�
  <0    ��������
�ڲ��������̣�
    ���������Ļ�ȡ��·���ر�λ�¼�������д��ȫ�ֱ����С�
********************************************************************/
int Init3360Para()
{

    memset(&event_3360, 0, sizeof(EVENT_3360_INFO));

    event_3360.eventCommInfo.eventOAD.nObjID= 0X3360;
    event_3360.eventCommInfo.eventOAD.value= 0X33600200;


    OOP_RCSD_T eventRCSD={
        10, 0,{
                {0,{{0X20220200},}},//�¼���¼���
                {0,{{0X201E0200},}},//�¼�����ʱ��
                {0,{{0X20200200},}},//�¼�����ʱ��
                {0,{{0X20240200},}},//�¼�����Դ
                {0,{{0X33000200},}},//ͨ���ϱ�״̬

                {0,{{0x40010200},}},//ͨ�ŵ�ַ
                {0,{{0x28000200},}},//��·���طֺ�״̬
                {0,{{0x20000200},}},//��ѹ
                {0,{{0x20010200},}},//����
                {0,{{0x20010700},}},//©����
            }
    };

    event_3360.eventCommInfo.eventRCSD = eventRCSD;

    // ������Ϣ
    IniEventCommInfo(&event_3360.eventCommInfo);
 
    event_3360.initflag = 1;
    return ERR_OK;
}


/**********************************************************************
* @name      : get_temperature_param
* @brief     ����ȡ�¶ȸ澯���ò��� 33610600
* @param[in] ��int clientid    socket���
               uint8 logicId   �߼���ַ
               uint16 infoNum  ��Ϣ���
               uint32 value    oadֵ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��
* @Update    :
**********************************************************************/
int get_temperature_param(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int              ret = 0;
    uint32           readlen = 0;
    NOMAL_OAD_T      tOAD;
    OOP_TemperatureWarningParam_T param;
    if(0x33610600 != value)
    {
        return -1;
    }
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
    memset(&param,0,sizeof(OOP_TemperatureWarningParam_T));
    tOAD.oad.value = value;
    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_TemperatureWarningParam_T), (uint8 *)&param, &readlen);
    if(0 == ret && readlen == sizeof(OOP_TemperatureWarningParam_T))
    {
        memcpy(&event_3361.param,&param,sizeof(OOP_TemperatureWarningParam_T));
    }
    else
    {
        EVENT_FMT_DEBUG("get_temperature_param error \n");
    }
    return 0;
}
/*********************************************************************
�������ܵ�Ԫ���¼�����ģ���ṩӦ�ò�������ʹ��
�������������������Ļ�ȡ�����¶ȸ澯�¼�������д��ȫ�ֱ����С�
���������
���������
  event_3361   �����¶ȸ澯�¼�ȫ�ֱ���
��������ֵ��
  0     �ɹ�
  <0    ��������
�ڲ��������̣�
    ���������Ļ�ȡ�����¶ȸ澯�¼�������д��ȫ�ֱ����С�
********************************************************************/
int Init3361Para()
{
    NOMAL_OAD_T nomalOAD;
    int ret;
    uint32 len;
    memset(&nomalOAD,0x00,sizeof(NOMAL_OAD_T));
    memset(&event_3361, 0, sizeof(EVENT_3361_INFO));

    event_3361.eventCommInfo.eventOAD.value= 0X33610200;

    OOP_RCSD_T eventRCSD={
        7, 0,{
                {0,{{0X20220200},}},//�¼���¼���
                {0,{{0X201E0200},}},//�¼�����ʱ��
                {0,{{0X20200200},}},//�¼�����ʱ��
                {0,{{0X20240200},}},//�¼�����Դ
                {0,{{0X33000200},}},//ͨ���ϱ�״̬

                {0,{{0x26000200},}},//�����¶�
                {0,{{0x20012200},}},//�¼�����ǰ����
            }
    };

    event_3361.eventCommInfo.eventRCSD = eventRCSD;

    // ������Ϣ
    IniEventCommInfo(&event_3361.eventCommInfo);
     //��ȡ �����澯������
    nomalOAD.oad.value = 0x33610600;
    ret = db_read_nomal(gDbCenterClient, &nomalOAD, sizeof(OOP_TemperatureWarningParam_T), (uint8 *)&event_3361.param, &len);
    if (ret != ERR_OK || len == 0)
    {
        EVENT_FMT_DEBUG("nomal_data_read OAD=%8X error ret=%d,len=%u\n", nomalOAD.oad.value, ret, len);
    }

    EVENT_FMT_TRACE("event_3361.param.LowerLimit=%d \n", event_3361.param.LowerLimit);
    EVENT_FMT_TRACE("event_3361.param.UpperLimit =%d \n", event_3361.param.UpperLimit);
    ret = read_pdata_xram("^event_3361",(char*)event_3361.info,0,sizeof(EVENT_3361_METER)*EVENT_3361_RECORD_COUNT);
    if(ret!=0)
    {
        EVENT_FMT_TRACE("read_pdata_xram event_3361 error ret =%d \n", ret);
    }
    event_3361.initflag = 1;
    return ERR_OK;
}

int Init3362Para()
{
    int ret;
    memset(&event_3362, 0, sizeof(EVENT_3362_INFO));

    event_3362.eventCommInfo.eventOAD.value= 0X33620200;


    OOP_RCSD_T eventRCSD={
        7, 0,{
                {0,{{0X20220200},}},//�¼���¼���
                {0,{{0X201E0200},}},//�¼�����ʱ��
                {0,{{0X20200200},}},//�¼�����ʱ��
                {0,{{0X20240200},}},//�¼�����Դ
                {0,{{0X33000200},}},//ͨ���ϱ�״̬

                {0,{{0x2B010200},}},//����澯״̬
                {0,{{0x20012200},}},//����澯״̬
            }
    };

    event_3362.eventCommInfo.eventRCSD = eventRCSD;

    // ������Ϣ
    IniEventCommInfo(&event_3362.eventCommInfo);

    ret = read_pdata_xram("^event",(char*)&event_3362.laststate,SmokeStateAddr,SmokeStateLen);
    {
        EVENT_FMT_TRACE("read_pdata_xram SmokeStateAddrerror ret =%d \n", ret);
    }
    event_3362.initflag = 1;
    return ERR_OK;
}

int Init3363Para()
{
    int ret;
    memset(&event_3363, 0, sizeof(EVENT_3362_INFO));

    event_3363.eventCommInfo.eventOAD.value= 0X33630200;


    OOP_RCSD_T eventRCSD={
        6, 0,{
                {0,{{0X20220200},}},//�¼���¼���
                {0,{{0X201E0200},}},//�¼�����ʱ��
                {0,{{0X20200200},}},//�¼�����ʱ��
                {0,{{0X20240200},}},//�¼�����Դ
                {0,{{0X33000200},}},//ͨ���ϱ�״̬

                {0,{{0x2B020200},}},//��ˮ״̬
            }
    };

    event_3363.eventCommInfo.eventRCSD = eventRCSD;

    // ������Ϣ
    IniEventCommInfo(&event_3363.eventCommInfo);
    ret = read_pdata_xram("^event",(char*)&event_3363.laststate,WaterStateAddr,WaterStateLen);
    {
        EVENT_FMT_TRACE("read_pdata_xram WaterStateAddr error ret =%d \n", ret);
    }
    event_3363.initflag = 1;
    return ERR_OK;
}


int CheckERC_3361(int clientid,uint8 logicid,uint16 mtr_id, OOP_METER_T oopMeter,uint8* databuf,uint16 datalen)
{

    uint8 i;
    int ret;
    EVENT_3361_METER *meterinfo=NULL;
    if(!event_3361.eventCommInfo.checkflag)
    {
        EVENT_FMT_DEBUG("CheckERC_3361 checkflag 0\n");
        return -1 ;
    }
    if(event_3361.param.LowerLimit==0&&event_3361.param.UpperLimit ==0)
    {
        EVENT_FMT_DEBUG("CheckERC_3361 �澯������δ����\n");
        return -1 ;
    }
    int16 temperature ;
    uint8 flag = 0;
    OOP_LONG3V_T tem;
    memset(&tem,0x00,sizeof(OOP_LONG3V_T));
    
    if(datalen!=sizeof(OOP_LONG3V_T))
    {
        EVENT_FMT_DEBUG("CheckERC_3361 datalen %d != %d \n",datalen,sizeof(OOP_LONG3V_T));
        return -1 ;
    }
    memcpy(&tem,databuf,datalen);

    for(i=0;i<EVENT_3361_RECORD_COUNT;i++)
    {
        if(event_3361.info[i].isvalid==1&&mtr_id==event_3361.info[i].pn)
        {
            meterinfo = &event_3361.info[i];
            break;
        }
    }

    for(i=0;i<tem.nNum;i++)
    {
        if(0==get_bit_value((uint8*)&tem.rsv, 32,i))
            continue;
        temperature = tem.nValue[i];
        EVENT_FMT_LOGCC("CheckERC_3361 %d ��ǰ�¶� %d ���� %d ���� %d \n",i,temperature,event_3361.param.LowerLimit,event_3361.param.UpperLimit);
        //�ж��¼�����
        if(NULL==meterinfo)
        {
            if(temperature<event_3361.param.LowerLimit || temperature>event_3361.param.UpperLimit)
            {
                flag = 1;
                EVENT_FMT_DEBUG("CheckERC_3361 �����¶ȸ澯�¼� \n");
                break;
            }
        }else
        {
            //�ж��¼��ָ�
            if(temperature>=event_3361.param.LowerLimit && temperature<=event_3361.param.UpperLimit)
            {
                flag = 1;
                EVENT_FMT_DEBUG("CheckERC_3361 �¶ȸ澯�¼��ָ� \n");
                break;
            }
        }

    }
    if(flag == 0)
    {
        return -1;
    }

    OOP_EVTSOURCE_T     eventSource; //�¼�����Դ
	OOP_EVTBUF_T        eventData;   //�¼���������
	memset(&eventSource, 0, sizeof(OOP_EVTSOURCE_T));
	memset(&eventData, 0, sizeof(OOP_EVTBUF_T));
	eventSource.choice = DT_TSA;
	memcpy(&eventSource.add,&oopMeter.basic.tsa,sizeof(OOP_TSA_T));
	eventData.cols.nNum = 2;
	eventData.oadNum = 2;
	eventData.cols.oad[0].value = 0x26000200;
	eventData.idLen[0] = sizeof(OOP_LONG3V_T);
    memcpy(eventData.buf,&tem,sizeof(OOP_LONG3V_T));
    eventData.len+=eventData.idLen[0] ;
    eventData.cols.oad[1].value = 0x20012200;
	eventData.idLen[1] = sizeof(OOP_INT3V_T);
	uint16 len;
	OOP_OAD_U oad = {0x20010200};
    len = get_link_record_data( clientid, logicid,mtr_id, oopMeter.basic.tsa.add,oad,eventData.buf+eventData.len); 
    if(len==0)
    {
        EVENT_FMT_DEBUG("CheckERC_3361 ��ȡ�¼�����ǰ����ʧ�� \n");
    }
    eventData.len+=eventData.idLen[1] ;
	
    ret = WriteEventToDB(clientid, mtr_id, &event_3361.eventCommInfo, eventSource, eventData, 0);
    if (ret < ERR_OK)
	{
		EVENT_FMT_DEBUG("CheckERC_3361 ��¼ʧ�ܷ���%d\n", ret);
		return -1;
	}
	return 0;
}

int CheckERC_3362(int clientid,uint8* databuf,uint16 datalen)
{
    if(!event_3362.eventCommInfo.checkflag)
    {
        EVENT_FMT_DEBUG("CheckERC_3362 checkflag 0\n");
        return -1 ;
    }
    uint8 state;
    int ret;
    if(datalen!=1)
    {
        EVENT_FMT_DEBUG("CheckERC_3362 errordatalen %d \n",datalen);
        return -1 ;
    }
    state = databuf[0];
    if(state ==event_3362.laststate )
    {
        return -1 ;
    }

    uint8 *paddr = NULL;
    uint8 addrlen;
    OOP_EVTSOURCE_T     eventSource; //�¼�����Դ
	OOP_EVTBUF_T        eventData;   //�¼���������
	memset(&eventSource, 0, sizeof(OOP_EVTSOURCE_T));
	memset(&eventData, 0, sizeof(OOP_EVTBUF_T));
	eventSource.choice = DT_TSA;
	addrlen = app_get_localaddr(&paddr);
	if(addrlen!=0)
	{	
	    eventSource.add.len = addrlen-1;
	    memcpy(eventSource.add.add,paddr,addrlen);
	}

	eventData.cols.nNum = 2;
	eventData.oadNum = 2;
	eventData.cols.oad[0].value = 0x2B010200;
	eventData.idLen[0] = sizeof(uint8);
    memcpy(eventData.buf,&state,sizeof(uint8));
    eventData.len+=eventData.idLen[0] ;
    eventData.cols.oad[1].value = 0x20012200;
	eventData.idLen[1] = sizeof(OOP_INT3V_T);
	uint16 len;
	OOP_OAD_U oad = {0x20010200};
    len = get_link_normal_data( clientid, oad,eventData.buf+eventData.len); 
    if(len==0)
    {
        EVENT_FMT_DEBUG("CheckERC_3362 ��ȡ�¼�����ǰ����ʧ��\n");
    }
    eventData.len+=eventData.idLen[1] ;
    if(state==1)
    {
        ret = WriteEventToDB(clientid, 0, &event_3362.eventCommInfo, eventSource, eventData, 1);
    }else
    {
        ret = WriteEventToDB(clientid, 0, &event_3362.eventCommInfo, eventSource, eventData, 2);
    }
    event_3362.laststate = state;
    write_pdata_xram("^event",(char*)&event_3362.laststate,SmokeStateAddr,SmokeStateLen);
    if (ret < ERR_OK)
	{
		EVENT_FMT_DEBUG("CheckERC_3362 ��¼ʧ�ܷ���%d\n", ret);
		return -1;
	}
	return 0;
}

int CheckERC_3363(int clientid,uint8* databuf,uint16 datalen)
{
    if(!event_3363.eventCommInfo.checkflag)
    {
        EVENT_FMT_DEBUG("CheckERC_3363 checkflag 0\n");
        return -1 ;
    }
    uint8 state;
    int ret;
    if(datalen!=1)
    {
        EVENT_FMT_DEBUG("CheckERC_3363 datalen %d \n",datalen);
        return -1 ;
    }
    state = databuf[0];
    if(state ==event_3363.laststate )
    {
        return -1 ;
    }
    uint8 *paddr = NULL;
    uint8 addrlen;
    OOP_EVTSOURCE_T     eventSource; //�¼�����Դ
	OOP_EVTBUF_T        eventData;   //�¼���������
	memset(&eventSource, 0, sizeof(OOP_EVTSOURCE_T));
	memset(&eventData, 0, sizeof(OOP_EVTBUF_T));
	eventSource.choice = DT_TSA;
	addrlen = app_get_localaddr(&paddr);
	if(addrlen!=0)
	{	
	    eventSource.add.len = addrlen-1;
	    memcpy(eventSource.add.add,paddr,addrlen);
	}

	eventData.cols.nNum = 1;
	eventData.oadNum = 1;
	eventData.cols.oad[0].value = 0x2B020200;
	eventData.idLen[0] = sizeof(uint8);
    memcpy(eventData.buf,&state,sizeof(uint8));
    eventData.len+=eventData.idLen[0] ;

    if(state==1)
    {
        ret = WriteEventToDB(clientid, 0, &event_3363.eventCommInfo, eventSource, eventData, 1);
    }else
    {
        ret = WriteEventToDB(clientid, 0, &event_3363.eventCommInfo, eventSource, eventData, 2);
    }
    event_3363.laststate = state;
    write_pdata_xram("^event",(char*)&event_3363.laststate,WaterStateAddr,WaterStateLen);
    if (ret < ERR_OK)
	{
		EVENT_FMT_DEBUG("CheckERC_3363 ��¼ʧ�ܷ���%d\n", ret);
		return -1;
	}
	return 0;
}


/**********************************************************************
* @name      : get_smoke_state
* @brief     ����ȡ����״̬ 2B010200
* @param[in] ��int clientid    socket���
               uint8 logicId   �߼���ַ
               uint16 infoNum  ��Ϣ���
               uint32 value    oadֵ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��
* @Update    :
**********************************************************************/
int get_smoke_state(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int              ret = 0;
    uint32           readlen = 0;
    NOMAL_OAD_T      tOAD;
    uint8  state = 0;

    if(0x2B010200 != value)
    {
        return -1;
    }
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));

    tOAD.oad.value = value;
 
    ret = db_read_nomal(clientid, &tOAD, sizeof(uint8), &state, &readlen);
    if(0 == ret && readlen == sizeof(uint8))
    {
        CheckERC_3362( clientid,&state, readlen);
    }
    else
    {
        EVENT_FMT_DEBUG("get_smoke_state error \n");
    }
    return 0;
}

/**********************************************************************
* @name      : get_water_state
* @brief     ����ȡˮ��״̬ 2B020200
* @param[in] ��int clientid    socket���
               uint8 logicId   �߼���ַ
               uint16 infoNum  ��Ϣ���
               uint32 value    oadֵ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��
* @Update    :
**********************************************************************/
int get_water_state(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int              ret = 0;
    uint32           readlen = 0;
    NOMAL_OAD_T      tOAD;
    uint8  state = 0;

    if(0x2B020200 != value)
    {
        return -1;
    }
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));

    tOAD.oad.value = value;
 
    ret = db_read_nomal(clientid, &tOAD, sizeof(uint8), &state, &readlen);
    if(0 == ret && readlen == sizeof(uint8))
    {
        CheckERC_3363( clientid,&state, readlen);
    }
    else
    {
        EVENT_FMT_DEBUG("get_water_state error \n");
    }
    return 0;
}

/**********************************************************************
* @name      : get_temperature
* @brief     ����ȡ�ն��¶� 26000200
* @param[in] ��int clientid    socket���
               uint8 logicId   �߼���ַ
               uint16 infoNum  ��Ϣ���
               uint32 value    oadֵ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��
* @Update    :
**********************************************************************/
int get_temperature(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int              ret = 0;
    uint32           readlen = 0;
    NOMAL_OAD_T      tOAD;
    OOP_LONG3V_T data;
    memset(&data,0x00,sizeof(OOP_LONG3V_T));

    if(0x26000200 != value)
    {
        return -1;
    }
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));

    tOAD.oad.value = value;
 
    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_LONG3V_T), (uint8*)&data, &readlen);
    if(0 == ret && readlen == sizeof(OOP_LONG3V_T))
    {
        OOP_METER_T temp;
        memset(&temp,0x00,sizeof(OOP_METER_T));
        uint8 addrlen;
        uint8 *paddr= NULL;
        addrlen = app_get_localaddr(&paddr);
        if(addrlen!=0)
        {
            temp.basic.tsa.len = addrlen-1;
            memcpy(temp.basic.tsa.add,paddr,addrlen);
        }
        //�ն˵��¶ȸ澯�¼�����Դ���ն˵�ַ
        CheckERC_3361( clientid,0,0,temp,(uint8*)&data,readlen);
    }
    else
    {
        EVENT_FMT_DEBUG("get_temperature error \n");
    }
    return 0;
}

/**********************************************************************
* @name      : get_switch_state
* @brief     ����ȡ���ر�λ״̬ 28000200
* @param[in] ��int clientid    socket���
               uint8 logicId   �߼���ַ
               uint16 infoNum  ��Ϣ���
               uint32 value    oadֵ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��
* @Update    :
**********************************************************************/
int get_switch_state(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int              ret = 0;
    uint32           readlen = 0;
    NOMAL_OAD_T      tOAD;
    uint8 data = 0;

    if(0x28000200 != value)
    {
        return -1;
    }
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));

    tOAD.oad.value = value;
 
    ret = db_read_nomal(clientid, &tOAD, sizeof(uint8),&data, &readlen);
    if(0 == ret && readlen == sizeof(uint8))
    {

        //�ն˵��¶ȸ澯�¼�����Դ���ն˵�ַ
        CheckERC_3360( clientid,&data,readlen);
    }
    else
    {
        EVENT_FMT_DEBUG("get_switch_state error \n");
    }
    return 0;
}

#if DESC("��Լ����",1)

/* ������OAD�б� OIA1 = 0x02 */
const OAD_TAB_T hunan_oadInfoList[] = 
{
    /*OAD          class    A1      A2    rsv     dataType                 len*/
    {{0x20A10200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_DWORD3V_T),   E_OOP_DWORD3V_T},          //��·�迹ֵ

    {{0x22120200}, 6,       0x02,   0x02, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},               //��ǰ���״̬
    {{0x28000200}, 6,       0x02,   0x08, 0,      DT_ENUM,                 sizeof(uint8),           E_ENUM},                   //���طֺ�״̬
    {{0x2B010200}, 6,       0x02,   0x0B, 0,      DT_ENUM,                 sizeof(uint8),           E_ENUM},                   //����澯״̬
    {{0x2B020200}, 6,       0x02,   0x0B, 0,      DT_ENUM,                 sizeof(uint8),           E_ENUM},                   //ˮ��״̬

    {{0x2A000200}, 3,       0x02,   0x0A, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},           //��ѹ������
    {{0x2A010200}, 3,       0x02,   0x0A, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},           //��ѹ������¶�
    {{0x2A030200}, 3,       0x02,   0x0A, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},           //��ѹ����ͷ�¶�
    {{0x2A040200}, 3,       0x02,   0x0A, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},           //©�������¶�
    {{0x2A050200}, 3,       0x02,   0x0A, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},           //��֧��ͷ�¶�
    {{0x2A060200}, 3,       0x02,   0x0A, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},           //�����ͷ�¶�

    {{0x31f20200}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_IMPE_OVER_T), E_OOP_IMPE_OVER_T},        //��·�迹Խ���¼� - �¼���¼��
    {{0x31f20300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),      E_OOP_OADS_T},             //��·�迹Խ���¼� - �����������Ա�
    {{0x31f20400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //��·�迹Խ���¼� - ��ǰ��¼��
    {{0x31f20500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //��·�迹Խ���¼� - ����¼��
    {{0x31f20600}, 7,       0x03,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_OOP_EVTCFG_31F2_T},      //��·�迹Խ���¼� - ���ò���
    {{0x31f20700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),   E_OOP_EVTSTAT_T},          //��·�迹Խ���¼� - ��ǰֵ��¼��
    {{0x31f20800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),           E_UNSIGNED},               //��·�迹Խ���¼� - �ϱ���ʶ
    {{0x31f20900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),           E_UNSIGNED},               //��·�迹Խ���¼� - ��Ч��ʶ
    {{0x31f20A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),   E_OOP_EVTTIME_T},          //��·�迹Խ���¼� - ʱ��״̬��¼��
    {{0x33f20200}, 8,		0x03,	0x01, 0,  	  DT_ENUM,				   sizeof(uint8),			E_ENUM},

    {{0x33600200}, 7,       0x03,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_EVENT_T),     E_OOP_EVENT_T},            //��·���ر�λ�¼� - �¼���¼��
    {{0x33600300}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),      E_OOP_OADS_T},             //��·���ر�λ�¼� - �����������Ա�
    {{0x33600400}, 7,       0x03,   0x03, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //��·���ر�λ�¼� - ��ǰ��¼��
    {{0x33600500}, 7,       0x03,   0x03, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //��·���ر�λ�¼� - ����¼��
    {{0x33600700}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),   E_OOP_EVTSTAT_T},          //��·���ر�λ�¼� - ��ǰֵ��¼��
    {{0x33600800}, 7,       0x03,   0x03, 0,      DT_ENUM,                 sizeof(uint8),           E_UNSIGNED},               //��·���ر�λ�¼� - �ϱ���ʶ
    {{0x33600900}, 7,       0x03,   0x03, 0,      DT_BOOL,                 sizeof(uint8),           E_UNSIGNED},               //��·���ر�λ�¼� - ��Ч��ʶ
    {{0x33600A00}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),   E_OOP_EVTTIME_T},          //��·���ر�λ�¼� - ʱ��״̬��¼��  
    
    {{0x33610200}, 7,       0x03,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_EVENT_T),     E_OOP_EVENT_T},            //�����¶ȸ澯�¼� - �¼���¼��
    {{0x33610300}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),      E_OOP_OADS_T},             //�����¶ȸ澯�¼� - �����������Ա�
    {{0x33610400}, 7,       0x03,   0x03, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //�����¶ȸ澯�¼� - ��ǰ��¼��
    {{0x33610500}, 7,       0x03,   0x03, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //�����¶ȸ澯�¼� - ����¼��
    {{0x33610600}, 7,       0x03,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TemperatureWarningParam_T), E_OOP_TemperatureWarningParam_T}, //�����¶ȸ澯�¼� - ���ò���
    {{0x33610700}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),   E_OOP_EVTSTAT_T},          //�����¶ȸ澯�¼� - ��ǰֵ��¼��
    {{0x33610800}, 7,       0x03,   0x03, 0,      DT_ENUM,                 sizeof(uint8),           E_UNSIGNED},               //�����¶ȸ澯�¼� - �ϱ���ʶ
    {{0x33610900}, 7,       0x03,   0x03, 0,      DT_BOOL,                 sizeof(uint8),           E_UNSIGNED},               //�����¶ȸ澯�¼� - ��Ч��ʶ
    {{0x33610A00}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),   E_OOP_EVTTIME_T},          //�����¶ȸ澯�¼� - ʱ��״̬��¼��
    
    {{0x33620200}, 7,       0x03,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_EVENT_T),     E_OOP_EVENT_T},            //����ʪ�ȸ澯�¼� - �¼���¼��
    {{0x33620300}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),      E_OOP_OADS_T},             //����ʪ�ȸ澯�¼� - �����������Ա�
    {{0x33620400}, 7,       0x03,   0x03, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //����ʪ�ȸ澯�¼� - ��ǰ��¼��
    {{0x33620500}, 7,       0x03,   0x03, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //����ʪ�ȸ澯�¼� - ����¼��
    {{0x33620700}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),   E_OOP_EVTSTAT_T},          //����ʪ�ȸ澯�¼� - ��ǰֵ��¼��
    {{0x33620800}, 7,       0x03,   0x03, 0,      DT_ENUM,                 sizeof(uint8),           E_UNSIGNED},               //����ʪ�ȸ澯�¼� - �ϱ���ʶ
    {{0x33620900}, 7,       0x03,   0x03, 0,      DT_BOOL,                 sizeof(uint8),           E_UNSIGNED},               //����ʪ�ȸ澯�¼� - ��Ч��ʶ
    {{0x33620A00}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),   E_OOP_EVTTIME_T},          //����ʪ�ȸ澯�¼� - ʱ��״̬��¼��  
    
    {{0x33630200}, 7,       0x03,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_EVENT_T),     E_OOP_EVENT_T},            //��ˮ�澯�¼� - �¼���¼��
    {{0x33630300}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),      E_OOP_OADS_T},             //��ˮ�澯�¼� - �����������Ա�
    {{0x33630400}, 7,       0x03,   0x03, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //��ˮ�澯�¼� - ��ǰ��¼��
    {{0x33630500}, 7,       0x03,   0x03, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //��ˮ�澯�¼� - ����¼��
    {{0x33630700}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),   E_OOP_EVTSTAT_T},          //��ˮ�澯�¼� - ��ǰֵ��¼��
    {{0x33630800}, 7,       0x03,   0x03, 0,      DT_ENUM,                 sizeof(uint8),           E_UNSIGNED},               //��ˮ�澯�¼� - �ϱ���ʶ
    {{0x33630900}, 7,       0x03,   0x03, 0,      DT_BOOL,                 sizeof(uint8),           E_UNSIGNED},               //��ˮ�澯�¼� - ��Ч��ʶ
    {{0x33630A00}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),   E_OOP_EVTTIME_T},          //��ˮ�澯�¼� - ʱ��״̬��¼��

    {{0x60F10200}, 11,      0x06,   0x00, 0,      DT_ARRAY,                sizeof(OOP_NodeTopology_T), E_OOP_NodeTopology_T},  //�ڵ�������Ϣ
    {{0x60E10200}, 8,       0x06,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_SENSOR_PARAM_T), E_OOP_SENSOR_PARAM_T},  //��ʪ�ȴ�����
    {{0x60E20200}, 8,       0x06,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_SENSOR_PARAM_T), E_OOP_SENSOR_PARAM_T},  //��ʪ�ȴ�����
    {{0x60E30200}, 8,       0x06,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_SENSOR_PARAM_T), E_OOP_SENSOR_PARAM_T},  //��ʪ�ȴ�����
    {{0x60E40200}, 8,       0x06,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_SENSOR_PARAM_T), E_OOP_SENSOR_PARAM_T},  //��ʪ�ȴ�����
};

const uint32 hunan_oadInfoListLen = sizeof(hunan_oadInfoList)/sizeof(hunan_oadInfoList[0]);

/**
*********************************************************************
* @name��       class_hunan_oadinfo_get
* @brief��      ��ѯODA�б��õ�OAD������Ϣ
* @param[in] ��OOP_OAD_U oad        ��ѯOAD
* @param[out]��OAD_INFO_T *info     ODA��Ϣ
* @return    ����ѯ��� TRUE-�ɹ���FALSE-ʧ��
* @author    : �� ��
* @Date      ��2019-12-6
*********************************************************************
*/
int class_hunan_oadinfo_get(OOP_OAD_U oad, OAD_INFO_T *pstOdaInfo)
{
    uint8 OIA  = 0;
    uint8 OIA1 = 0;
    uint8 OIA2 = 0;
    OIA = (uint8)(oad.nObjID>>8);
    OIA1 = (uint8)((OIA>>4)&0x0f);
    OIA2 = (uint8)((OIA)&0x0f);
    OOP_OAD_U blockOAD = {0};
    uint32 size = 0;
    const OAD_TAB_T *pstOdaTab = NULL;

    //��Ϊ0�Ÿ�ֵ������ݹ���ú�ᱻ����
    if(oad.nIndex != 0)  
    {
        pstOdaInfo->nIndex = oad.nIndex;
    }
    

    pstOdaTab = hunan_oadInfoList; size = hunan_oadInfoListLen;

    if(pstOdaTab != NULL)
    {
        while(size--)
        {
            if(pstOdaTab->oad.value == oad.value)
            {
                pstOdaInfo->pTab = pstOdaTab;
                pstOdaInfo->subeData = pstOdaTab->eData;
                return TRUE;
            }
            else if((pstOdaTab->oad.nObjID == oad.nObjID) &&
                    (pstOdaTab->oad.attID == oad.attID) &&
                    (pstOdaTab->oad.nIndex == oad.nIndex)) 
            {
                PRTL_FMT_DEBUG("non-strict search oadInfoList. OAD(0x%08x), ATTPRO(%d), OIA1(%d), OIA2(%d)\n", oad.value, oad.attPro, OIA1, OIA2);
                pstOdaInfo->pTab = pstOdaTab;
                pstOdaInfo->subeData = pstOdaTab->eData;
                return TRUE;                
            }
                    
            pstOdaTab++;
        }
    }

    //���������Ϊ0���鲻�У�����0��һ��
    if(oad.nIndex != 0)  
    {
        blockOAD.value = oad.value;
        blockOAD.nIndex = 0;
        
        PRTL_FMT_DEBUG("block search oadInfoList. OAD(0x%08x), OIA1(%d), OIA2(%d)\n", oad.value, OIA1, OIA2);
        return class_hunan_oadinfo_get(blockOAD, pstOdaInfo);
    }

    PRTL_FMT_DEBUG("search oadInfoList miss. OAD(0x%08x), OIA1(%d), OIA2(%d)\n", oad.value, OIA1, OIA2);

    return FALSE;
}

/* �Զ���OMD�б� OIA1 = 0x0F OIA2 = 0x0F */
const OMD_INFO_T hunan_omdInfoList[] = 
{
    /*OMD          class   A1      A2    rsv     dataType                  len*/
    {{0x60F17F00},   11,     0x06,   0x00,  0,     DT_LONG_UNSIGNED,         sizeof(uint16),           E_LONG_UNSIGNED},  //ʵʱ��������
};
const uint32 hunan_omdInfoListLen = sizeof(hunan_omdInfoList)/sizeof(hunan_omdInfoList[0]);

/**
*********************************************************************
* @name��       class_hunan_omdinfo_get
* @brief��      ��ѯODA�б��õ�OAD������Ϣ
* @param[in] ��OOP_OAD_U oad        ��ѯOAD
* @param[out]��OAD_INFO_T *info     ODA��Ϣ
* @return    ����ѯ��� TRUE-�ɹ���FALSE-ʧ��
* @author    : �� ��
* @Date      ��2019-12-6
*********************************************************************
*/
int class_hunan_omdinfo_get(OOP_OAD_U oad, OAD_INFO_T *pstOdaInfo)
{
    uint8 OIA  = 0;
    uint8 OIA1 = 0;
    uint8 OIA2 = 0;
    OIA = (uint8)(oad.nObjID>>8);
    OIA1 = (uint8)((OIA>>4)&0x0f);
    OIA2 = (uint8)((OIA)&0x0f);
    OOP_OAD_U blockOAD = {0};
    uint32 size = 0;
    const OMD_INFO_T *pstOdaTab = NULL;
     
    pstOdaTab = hunan_omdInfoList; size = hunan_omdInfoListLen;

    if(pstOdaTab != NULL)
    {
        while(size--)
        {
            if(pstOdaTab->omd.value == oad.value)
            {
                pstOdaInfo->pTab = (OAD_TAB_T *)pstOdaTab;
                return TRUE;
            }

            pstOdaTab++;
        }
    }

    //���������Ϊ0���鲻�У�����0��һ��
    if(oad.nIndex != 0)  
    {
        blockOAD.value = oad.value;
        blockOAD.nIndex = 0;
        
        PRTL_FMT_DEBUG("block search oadInfoList. OAD(0x%08x), OIA1(%d), OIA2(%d)\n", oad.value, OIA1, OIA2);
        return class_hunan_omdinfo_get(blockOAD, pstOdaInfo);
    }

    PRTL_FMT_DEBUG("search omdInfoList miss. OAD(0x%08x), OIA1(%d), OIA2(%d)\n", oad.value, OIA1, OIA2);

    return FALSE;
}

/**
*********************************************************************
* @brief��      �¼�����OAD���ӽṹ��ƴװ���ģ���ʱ������
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
int32 data_to_buf_evt_oad_hunan(OOP_OAD_U oad, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint8 *dstBufTmp = (uint8 *)dstBuf; 
    uint32 offset = *pOffset;
    uint8 i = 0;

    switch(oad.value)
    {
        case 0x43002302:   //�¼�����ǰ����汾��
        case 0x43004302:   //�¼�����������汾��
        {
            OOP_VERSION_T version;
            memcpy(&version.szSoftVer, srcData, srcLen);
            
            dstBufTmp[offset++] = DT_VISIBLE_STRING;
            dstBufTmp[offset++] = 4;
            memcpy(&dstBufTmp[offset], version.szSoftVer.value, 4);    
            offset += 4;      

        }break;
        
        case 0x60412200:  //�¼�����ǰ���һ�γ���ɹ�ʱ��
        case 0x201E4200:  //�¼�����ʱ��
        case 0x40002200:  //��ʱǰʱ��
        case 0x40008200:  //��ʱ��ʱ��        
        {
            if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen-offset, &offset, srcData, srcLen) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
        }break;
    
        case 0xF2034201:  //��1·״̬��λ������״̬
        case 0xF2034202:  //��2·״̬��λ������״̬
        case 0xF2034203:  //��3·״̬��λ������״̬
        case 0xF2034204:  //��4·״̬��λ������״̬   
        case 0xF2034205:  //��5·״̬��λ������״̬
        case 0xF2034206:  //��6·״̬��λ������״̬
        case 0xF2034207:  //��7·״̬��λ������״̬
        case 0xF2034208:  //��8·״̬��λ������״̬           
        {
            INPUTSTATE swstate;
            memcpy(&swstate, srcData, srcLen);
    
            dstBufTmp[offset++] = DT_STRUCTURE;
    
            //�ṹ��Ա��
            dstBufTmp[offset++] = 2; 
            
            if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &swstate.st, sizeof(uint8)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            
            if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &swstate.cd, sizeof(uint8)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
  
        }break;
    
        case 0x22004202:   //�¼������󣬵����ѷ���ͨ������
        case 0x31100601:   //��ͨ����������
        {
            if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, srcData, sizeof(uint32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            } 

        }break;
        case 0x20002201:   //�¼�����ǰ��ѹ
        case 0x20002202:   //�¼�����ǰ��ѹ
        case 0x20002203:   //�¼�����ǰ��ѹ
        case 0x20004201:   //�¼��������ѹ
        case 0x20004202:   //�¼��������ѹ
        case 0x20004203:   //�¼��������ѹ
        case 0x20006201:   //�¼�����ǰ��ѹ
        case 0x20006202:   //�¼�����ǰ��ѹ
        case 0x20006203:   //�¼�����ǰ��ѹ
        case 0x20008201:   //�¼��������ѹ
        case 0x20008202:   //�¼��������ѹ
        case 0x20008203:   //�¼��������ѹ
        {
                if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, srcData, sizeof(uint16)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
        }break;  
    
        case 0x20034200:   //�¼�����ʱ��ѹ�������
        case 0x20000200:   //��ѹ
        case 0x20002200:   //�¼�����ǰ��ѹ
        case 0x20004200:   //�¼��������ѹ
        {
            OOP_WORD3V_T var; 
            memcpy(&var, srcData, srcLen);
    
            dstBufTmp[offset++] = DT_ARRAY;
    
            //�����Ա����
            dstBufTmp[offset++] = var.nNum;
    
            for(i = 0; i < var.nNum; i++)
            {
                if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &var.nValue[i], sizeof(uint16)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }break;  
    
        case 0x00102201:  //�¼�����ǰ�������й��ܵ���
        case 0x00112201:  //�¼�����ǰ��A�������й����� 
        case 0x00122201:  //�¼�����ǰ��B�������й�����        
        case 0x00132201:  //�¼�����ǰ��C�������й�����        
        
        case 0x00104201:  //�¼�����ʱ�������й��ܵ���
        
        case 0x00202201:  //�¼�����ǰ�ķ����й��ܵ���
        case 0x00212201:  //�¼�����ǰ��A�෴���й����� 
        case 0x00222201:  //�¼�����ǰ��B�෴���й�����        
        case 0x00232201:  //�¼�����ǰ��C�෴���й�����  
        
        case 0x00108201:  //�¼�������������й��ܵ���  
        case 0x00118201:  //�¼��������A�������й����� 
        case 0x00128201:  //�¼��������B�������й�����        
        case 0x00138201:  //�¼��������C�������й�����     

        case 0x00208201:  //�¼�������ķ����й��ܵ���  
        case 0x00218201:  //�¼��������A�෴���й����� 
        case 0x00228201:  //�¼��������B�෴���й�����        
        case 0x00238201:  //�¼��������C�෴���й�����         
        {
            uint32 energys;
            
            if(srcLen != sizeof(uint32))
            {
                PRTL_FMT_DEBUG("srcLen check failed. srcLen(%d) != %d \n", srcLen, sizeof(uint32));
                return ERR_NORMAL;
            }
            
            memcpy(&energys, srcData, srcLen);  

            if(energys == 0) //TODO ����ʹ��0��Ϊ�����ڵ��о�
            {
                dstBufTmp[offset++] = DT_NULL;
            }
            else
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &energys, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }break; 
        
        case 0x00302201:  //�¼�����ǰ������޹�1�ܵ��� 
        case 0x00312201:  //�¼�����ǰ��A������޹�1���� 
        case 0x00322201:  //�¼�����ǰ��B������޹�1����        
        case 0x00332201:  //�¼�����ǰ��C������޹�1����  
        
        case 0x00402201:  //�¼�����ǰ������޹�2�ܵ���  
        case 0x00412201:  //�¼�����ǰ��A������޹�2���� 
        case 0x00422201:  //�¼�����ǰ��B������޹�2����        
        case 0x00432201:  //�¼�����ǰ��C������޹�2����  

        case 0x00308201:  //�¼������������޹�1�ܵ���  
        case 0x00318201:  //�¼��������A������޹�1���� 
        case 0x00328201:  //�¼��������B������޹�1����        
        case 0x00338201:  //�¼��������C������޹�1����     

        case 0x00408201:  //�¼������������޹�2�ܵ���  
        case 0x00418201:  //�¼��������A������޹�2���� 
        case 0x00428201:  //�¼��������B������޹�2����        
        case 0x00438201:  //�¼��������C������޹�2����         
        {
            int energys;

            if(srcLen != sizeof(int))
            {
                PRTL_FMT_DEBUG("srcLen check failed. srcLen(%d) != %d \n", srcLen, sizeof(int));
                return ERR_NORMAL;
            }
            
            memcpy(&energys, srcData, srcLen);  

            if(energys == 0) //TODO ����ʹ��0��Ϊ�����ڵ��о�
            {
                dstBufTmp[offset++] = DT_NULL;
            }
            else
            {
                if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, &energys, sizeof(int)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }break; 
        case 0x40010200: //ͨ�ŵ�ַ
        {
            OOP_OCTETVAR16_T addr;
            if(srcLen != sizeof(OOP_OCTETVAR16_T))
            {
                PRTL_FMT_DEBUG("srcLen check failed. srcLen(%d) != %d \n", srcLen, sizeof(OOP_OCTETVAR16_T));
                return ERR_NORMAL;
            }
            memcpy(&addr,srcData, srcLen);
            if(addr.nNum == 0)
            {
                dstBufTmp[offset++] = DT_NULL;
            }else
            {
                if(basic_data_to_buf(E_OOP_OCTETVAR16_T, dstBufTmp, dstLen-offset, &offset, &addr, sizeof(OOP_OCTETVAR16_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }
            break;
        case 0x28000200://����״̬
        case 0x2b010200://����澯״̬
        case 0x2b020200://ˮ��״̬
        {
            if(srcLen != sizeof(uint8))
            {
                PRTL_FMT_DEBUG("srcLen check failed. srcLen(%d) != %d \n", srcLen, sizeof(int));
                return ERR_NORMAL;
            }
            uint8 state ;
            memcpy(&state,srcData, srcLen);
            if(state == 0)
            {
                dstBufTmp[offset++] = DT_NULL;
            }else
            {
                if(basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &state, sizeof(uint8)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
 
            }
        }
            break;
        case 0x20012201:   //�¼�����ǰ����
        case 0x20012202:   //�¼�����ǰ����
        case 0x20012203:   //�¼�����ǰ����
        case 0x20014201:   //�¼����������
        case 0x20014202:   //�¼����������
        case 0x20014203:   //�¼����������
        case 0x20016201:   //�¼�����ǰ����
        case 0x20016202:   //�¼�����ǰ����
        case 0x20016203:   //�¼�����ǰ����
        case 0x20018201:   //�¼����������
        case 0x20018202:   //�¼����������
        case 0x20018203:   //�¼����������
        {
                if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, srcData, sizeof(int32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
        }break;  
        case 0x20010200:    //����
        case 0x20012200:    //�¼�����ǰ����     
        case 0x20014200:    //�¼����������        
        {
            OOP_INT3V_T value; 
            if(srcLen != sizeof(OOP_INT3V_T))
            {
                PRTL_FMT_DEBUG("srcLen check failed. srcLen(%d) != %d \n", srcLen, sizeof(int));
                return ERR_NORMAL;
            }

            memcpy(&value, srcData, srcLen);
    
            dstBufTmp[offset++] = DT_ARRAY;
    
            //�����Ա����
            dstBufTmp[offset++] = value.nNum;
    
            for(i = 0; i < value.nNum; i++)
            {
                if(0==get_bit_value((uint8*)&value.rsv,32,i))
                {
                    dstBufTmp[offset++] = DT_NULL;
                    continue;
                }
                            
                if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, &value.nValue[i], sizeof(int32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }break;  

        case 0x20010500:    //ʣ�����
        case 0x20012500:    //�¼�����ǰ��ʣ�����
        {
            int value;
            
            if(srcLen != sizeof(int))
            {
                PRTL_FMT_DEBUG("srcLen check failed. srcLen(%d) != %d \n", srcLen, sizeof(int));
                return ERR_NORMAL;
            }
            
            memcpy(&value, srcData, srcLen);  

            if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen, &offset, &value, sizeof(int)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
        }break; 
            
        case 0x26000200:    //�����¶�
        {
            OOP_LONG3V_T value; 
            if(srcLen != sizeof(OOP_LONG3V_T))
            {
                PRTL_FMT_DEBUG("srcLen check failed. srcLen(%d) != %d \n", srcLen, sizeof(OOP_LONG3V_T));
                return ERR_NORMAL;
            }

            memcpy(&value, srcData, srcLen);
    
            dstBufTmp[offset++] = DT_ARRAY;
    
            //�����Ա����
            dstBufTmp[offset++] = value.nNum;
    
            for(i = 0; i < value.nNum; i++)
            {
                if(0==get_bit_value((uint8*)&value.rsv,32,i))
                {
                    dstBufTmp[offset++] = DT_NULL;
                    continue;
                }
                            
                if(basic_data_to_buf(E_LONG, dstBufTmp, dstLen-offset, &offset, &value.nValue[i], sizeof(int16)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }break;  
        default: nRet = ERR_NORMAL; break;
    }

    *pOffset = offset;
    
    return ERR_OK;
}

/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class3)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/

    /*��μ��*/

    
    
    

        



/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class3)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class3_oop_temp(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_OK;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;
    uint8      offlen = 0;
    uint8      lenBuf[3] = {0};
    uint32     i = 0;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;
    OOP_LONG3V_T data;

    /*��μ��*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class2 error, ERR_PTR\n");
        return ERR_PTR;
    }

    MEMZERO(&data, sizeof(data));
    
    if (dsdata->srcLen != sizeof(OOP_LONG3V_T))
    {
        PRTL_FMT_DEBUG("data_to_buf_class2 error, ERR_OPTION\n");
        return ERR_OPTION;
    }
    
    memcpy(&data, srcDataTmp, dsdata->srcLen);
    offlen = set_len_offset(3, lenBuf);
    dstBufTmp[offset] = pOadInfoTmp->pTab->dataType;
    offset ++;
    
    memcpy(&dstBufTmp[offset], lenBuf, offlen); 
    offset += offlen;

    for (i = 0; i < 3; i++)
    {
        if(0==get_bit_value((uint8*)&data.rsv,32,i))
        {
            dstBufTmp[offset++] = DT_NULL;
            continue;
        }
        
        if(basic_data_to_buf(E_LONG, dstBufTmp, dsdata->dstLen-offset, &offset, &data.nValue[i], sizeof(int32)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }
    }

    *dsdata->dOffset = offset;

    return nRet;
}

/**
*********************************************************************
* @brief��      �¶�Խ�޸澯�¼�
*               
* @param[in]:   srcBuf     Դ������698����
*               srcLen     Դ���ݳ���
*               dstLen     Ŀ�ĳ��ȣ�ͨ���Ǳ����ĳ���
*
* @param[out]�� pOffset     ��������ݺ��Դ������ƫ��
*               dstData    Ŀ����������ͨ���Ǳ����ĵ�ַ
* @return��      =0            - �ɹ�
*               <0            - ������
*********************************************************************
*/
int32 data_to_buf_evt_cfg3361(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_TemperatureWarningParam_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //�ṹ����
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //�ṹ��Ա��
    dstBufTmp[offset++] = 2;

    //�¶�����
    if(basic_data_to_buf(E_LONG, dstBufTmp, dstLen-offset, &offset, &evtcfg.LowerLimit, sizeof(uint32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //�¶�����
    if(basic_data_to_buf(E_LONG, dstBufTmp, dstLen-offset, &offset, &evtcfg.UpperLimit, sizeof(uint32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief��      �迹Խ�޸澯�¼�
*               
* @param[in]:   srcBuf     Դ������698����
*               srcLen     Դ���ݳ���
*               dstLen     Ŀ�ĳ��ȣ�ͨ���Ǳ����ĳ���
*
* @param[out]�� pOffset     ��������ݺ��Դ������ƫ��
*               dstData    Ŀ����������ͨ���Ǳ����ĵ�ַ
* @return��      =0            - �ɹ�
*               <0            - ������
*********************************************************************
*/
int32 data_to_buf_evt_cfg31f2(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_31F2_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //�ṹ����
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //�ṹ��Ա��
    dstBufTmp[offset++] = 1;

    //�迹Խ��ֵ
    if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.impeLimit, sizeof(uint32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief��      ��������������Ա�
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
* @author    : ����
* @Date      ��2019-12-6                 
*********************************************************************
*/
OOP_DAR_E data_to_buf_node_topology(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_NodeTopologyInfo_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //��������

    //�����Ա����

        //�ṹ����
        dstBuf[dOffset++] = DT_STRUCTURE;

        //�ṹ��Ա��
        dstBuf[dOffset++] = 4;
                
        //���ڵ��豸��ַ
    dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data.NodeDevAddr, sizeof(OOP_TSA_T));
        if(dar != DATA_SUCCESS)
        {
        PRTL_BUF_DEBUG(data.NodeDevAddr.add, OOP_TSA_SIZE-1, "basic_data_to_buf failed. dar(%d)\n",dar);
            return DATA_TYPE_UNMATCHED;
        }
        
        //���ڵ���λ��Ϣ
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.NodePhaseInfo, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),oad(0x%08x)\n",dar, data.NodePhaseInfo);
            return DATA_TYPE_UNMATCHED;
        }
        //���ڵ��豸��ַ
    dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data.ParentNodeDevAddr, sizeof(OOP_TSA_T));
        if(dar != DATA_SUCCESS)
        {
        PRTL_BUF_DEBUG(data.ParentNodeDevAddr.add, OOP_TSA_SIZE-1, "basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }   

        //���ڵ���·������ַ
    dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data.ParentNodeLineAddr, sizeof(OOP_TSA_T));
        if(dar != DATA_SUCCESS)
        {
        PRTL_BUF_DEBUG(data.ParentNodeLineAddr.add, OOP_TSA_SIZE-1, "basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
    }

    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

/**
*********************************************************************
* @brief��       ��·�迹ֵ
* @param[in]:   srcBuf     Դ������698����
*               srcLen     Դ���ݳ���
*               dstLen     Ŀ�ĳ��ȣ�ͨ���Ǳ����ĳ���
*
* @param[out] ��pOffset     ��������ݺ��Դ������ƫ��
*               dstData    Ŀ����������ͨ���Ǳ����ĵ�ַ
* @return��      =0            - �ɹ�
*               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_impe(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_DWORD3V_T data;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    uint32 i = 0;
    
    memcpy(&data, srcData, srcLen);

    //��������
    dstBufTmp[offset++] = DT_ARRAY;
    
    //���ݳ�Ա��
    dstBufTmp[offset++] = 3;

    for(i = 0; i < 3; i++)
    {
        dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &data.nValue[i], sizeof(uint32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d), data(%d)\n", i, dar, data.nValue[i]);
            return DATA_TYPE_UNMATCHED;
        }
    }
    
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief��      �¶�Խ�޸澯�¼� 
*             
* @param[in]:   srcBuf     Դ������698����
*               srcLen     Դ���ݳ���
*               dstLen     Ŀ�ĳ��ȣ�ͨ���Ǳ����ĳ���
*
* @param[out]�� pOffset     ��������ݺ��Դ������ƫ��
*               dstData    Ŀ����������ͨ���Ǳ����ĵ�ַ
* @return��      =0            - �ɹ�
*               <0            - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_cfg3361(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E  dar = DATA_SUCCESS; 
    uint32  offset = *pOffset;
    OOP_TemperatureWarningParam_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

     //�ṹ����
     if(srcBufTmp[offset++] != DT_STRUCTURE)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }
    
     //�ṹ��Ա��
     if(srcBufTmp[offset++] != 2)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct num failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }

    //�¶�����
    dar = basic_buf_to_data(E_LONG, srcBufTmp, srcLen-offset, &offset, &evtcfg.LowerLimit, sizeof(int16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //�¶�����
    dar = basic_buf_to_data(E_LONG, srcBufTmp, srcLen-offset, &offset, &evtcfg.UpperLimit, sizeof(int16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }    

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief��      �迹Խ�޸澯�¼� 
*             
* @param[in]:   srcBuf     Դ������698����
*               srcLen     Դ���ݳ���
*               dstLen     Ŀ�ĳ��ȣ�ͨ���Ǳ����ĳ���
*
* @param[out]�� pOffset     ��������ݺ��Դ������ƫ��
*               dstData    Ŀ����������ͨ���Ǳ����ĵ�ַ
* @return��      =0            - �ɹ�
*               <0            - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_cfg31f2(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E  dar = DATA_SUCCESS; 
    uint32  offset = *pOffset;
    OOP_EVTCFG_31F2_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //�ṹ����
    if(srcBufTmp[offset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ա��
    if(srcBufTmp[offset++] != 1)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct num failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    //�迹����ֵ
    dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.impeLimit, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name��       ͨѶ���������¼����ò���
* @brief��      class7��SET����
* @param[in] ��pOdaInfo        OAD��Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
* @return    ���������ʵ�ʳ���
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3361_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_TemperatureWarningParam_T evtcfg;

    //�ӱ�������ȡ����
    dar = buf_to_data_evt_cfg3361(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_TemperatureWarningParam_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_cfg3110 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;

    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_TemperatureWarningParam_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfg3361 db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name��       �迹�����¼����ò���
* @brief��      class7��SET����
* @param[in] ��pOdaInfo        OAD��Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
* @return    ���������ʵ�ʳ���
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg31f2_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_EVTCFG_31F2_T evtcfg;

    //�ӱ�������ȡ����
    dar = buf_to_data_evt_cfg31f2(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_EVTCFG_31F2_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_cfg3110 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;

    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_EVTCFG_31F2_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfg3361 db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name��       �¶ȳ��޸澯�¼����ò���
* @brief��      class7��SET����
* @param[in] ��pOdaInfo        OAD��Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
* @return    ���������ʵ�ʳ���
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3361_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_TemperatureWarningParam_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_3110_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_TemperatureWarningParam_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return oop_null_data_res(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfg3361(pOadInfo, outData->pResultData, outDataMax, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_asobjattr failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       �迹���޸澯�¼����ò���
* @brief��      class7��SET����
* @param[in] ��pOdaInfo        OAD��Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
* @return    ���������ʵ�ʳ���
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg31f2_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_EVTCFG_31F2_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_31F2_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_EVTCFG_31F2_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return oop_null_data_res(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfg31f2(pOadInfo, outData->pResultData, outDataMax, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_asobjattr failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class11_NodeTopology_get
* @brief��      ��ȡ�ڵ�������Ϣ
* @param[in] ��pOdaInfo        OAD��Ϣ
               inData         ���뱨��
               inLen          ���뱨�ĳ���
* @return    ���������ʵ�ʳ���
* @author    : ����
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_nodetopology_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 offset  = 0;
    uint32             i = 0;
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_NodeTopologyInfo_T data;
    uint32    realNum = 0;    //�������ڷ�֡���µ�ÿ֡ʵ�ʳ�Ա��
    uint32    numOffset = 0;  //�����Ա���ڱ����е�λ��
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //�ϵ���Ϣ
    uint32    maxBufLen = 0;
    BOOLEAN   isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
    memset(&data, 0, sizeof(data));
    MEMZERO(&convert, sizeof(convert)); 

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    //��������
    PRTL_FMT_DEBUG("read node topology start\n");
    inReadRecord.recordOAD.logicId = 0;

    if(pOadInfo->nIndex == 0)
    {
        inReadRecord.recordOAD.infoNum = 0xffff;
    }
    else
    {
        inReadRecord.recordOAD.infoNum = pOadInfo->nIndex;
    }
    
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = NO_VIR_MTR_MAX_NUM;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = pOadInfo->pTab->oad.value;
    stepReadRecord.MAC.nNum = 0;
    if(pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        inReadRecord.cStart = pApduInfo->bpBreakInfo->tmStartSeg;    
    }
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //����OAD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;                             //���
    if(pOadInfo->nIndex == 0)
    {
        outData->pResultData[offset ++] = DT_ARRAY;                         //����
        numOffset = offset;                                                 //����ƫ��
        outData->pResultData[offset ++] = 0x00;                             //����������0
    }
    bufAllLen = offset;
    ret = db_read_record_start(pApduInfo->hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("class11_nodetopology_get db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("class11_nodetopology_get db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(pApduInfo->hUdp, handle, &stepReadRecord, &stepRecordOut);
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("class11_nodetopology_get db_read_record_step err = %d!\n", ret);
                continue;
                //�������ĳ�����NULL
            }

            //OAD�뷵������
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != pOadInfo->pTab->oad.value)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_NodeTopologyInfo_T)))
            {
                PRTL_FMT_DEBUG("class11_nodetopology_get data err !\n");
                PRTL_FMT_DEBUG("class11_nodetopology_get stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("class11_nodetopology_get stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("class11_nodetopology_get stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("class11_nodetopology_get sizeof(OOP_METER_T) = %d\n", 
                    sizeof(OOP_METER_T));
                continue;
            }

            memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);

            //ת���ɱ�������
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;

            convert.dstBuf = &outData->pResultData[offset];
            convert.dstLen = outDataMax - offset;
            convert.dOffset = &dOffset;

            breakInfo.tmStartSeg = stepRecordOut.infoNum;
            dar = data_to_buf_node_topology(pOadInfo, &convert);
            if(dar == DATA_SUCCESS)
            {                  
                bufAllLen += dOffset;
                if (bufAllLen <= maxBufLen)
                {
                    memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                    offset += dOffset;
                    outData->resultDataLen = offset;
                    PRTL_FMT_DEBUG(convert.dstBuf, dOffset, "��%d������ڵ�:", i);
                    realNum ++;
                    dOffset = 0;
                    sOffset = 0;
                }
                else
                {
                    dOffset = 0;
                    sOffset = 0;
                    isBufFull = TRUE;
                }
            }
            else
            {
                continue;
            }
            if ((isBufFull) && (offset > 0))
            {
                break;
            }                            
        }
    }
    db_read_record_end(pApduInfo->hUdp, handle);
    PRTL_FMT_DEBUG("read node topology end.\n");
    if (!isBufFull)
    {
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //ĩβ֡
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //��ż�1
            breakInfo.objPre = 0x01;                          //��������
        }
        else
        {
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = pApduInfo->reqType;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //����ĩβ֡
        breakInfo.objPre = 0x01;                             //��������
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //�״����
        }
        else
        {
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //��ż�1
        }
    }
    memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

    if(pOadInfo->nIndex == 0)
    {
        outData->pResultData[numOffset] = realNum;   //���¼�����
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @brief��     �������ĸ�ʽת698���ģ���̨�����ܱ��¼� 0x31120200
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_impeover(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    /*��μ��*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_refusenode param err\n");
        return ERR_PTR;
    }

    if (pOadInfo->pTab->eData != E_OOP_IMPE_OVER_T)
    {
        PRTL_FMT_DEBUG("check data type failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    OOP_IMPE_OVER_T info;
    memset(&info,0x00,sizeof(OOP_IMPE_OVER_T));

    if (srcLen != sizeof(OOP_IMPE_OVER_T))
    {
        PRTL_FMT_DEBUG("check data len failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&info, srcData, srcLen);

    //�ṹ��������
    dstBufTmp[offset++] = DT_STRUCTURE;

    //�ṹ��Ա����
    dstBufTmp[offset++] = 8 + info.data.oadNum;

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = info.nIndex;
    evtBasic.tmStart = info.tmStart;
    evtBasic.tmEnd = info.tmEnd;
    evtBasic.source = info.source;
    evtBasic.status = info.evtState;

    //*********************************************����Ϊ�¼���¼��Ԫ�Ĺ�������******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************����Ϊ��ͬ�¼�����������******************************************/

    //�¼�����ǰ�迹ֵ
    dar = data_to_buf_evt_impe(pOadInfo, dstBufTmp, dstLen-offset, &offset, &info.impeBef, sizeof(OOP_DWORD3V_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //�¼��������迹ֵ
    dar = data_to_buf_evt_impe(pOadInfo, dstBufTmp, dstLen-offset, &offset, &info.impeAft, sizeof(OOP_DWORD3V_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //�¼�����ʱ��λ
    dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &info.curPhase, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //����������������
    dar = data_to_buf_evt_coload(pOadInfo, dstBufTmp, dstLen-offset, &offset, &info.data, sizeof(info.data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_coload failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    *pOffset = offset;

    return dar;
}

int event_tmn_impeover(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_IMPE_OVER_T event;    
    uint32 i = 0 ,j = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO ��ʱ��ô��
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;
    //uint8      offlen = 0;
    //uint8      lenBuf[3] = {0};
    uint32 tmpOffset = 0;

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO ��ʱ��ô��
    memset(pOutBuf, 0, DATA_LEN_MAX);

    //NORMAL������������RECORD����������
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_impeover(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_impeover failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
            return nRet;
        }

        if(relyInfo->method != 1) //��NORAML������ȥ��STRUCTER���ͺͳ���
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            return ERR_NORMAL;
        }
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //�¼���¼���
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen-offset, &offset, &event.nIndex, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
            case 0x201E0200:   //�¼�����ʱ��
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //�¼�����ʱ��
            {
                if(event.tmEnd.year == 0) //����ʹ��0��Ϊ�����ڵ��о�
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }           
            }break;
            case 0x20240200:   //�¼�����Դ
            {
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;            
            case 0x33000200:   //�¼��ϱ�״̬
            {
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;

            case 0x20a12200:   //�¼�����ǰ�迹ֵ
            {
                if(data_to_buf_evt_impe(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.impeBef, sizeof(OOP_DWORD3V_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;

            case 0x20a14200:   //�¼��������迹ֵ
            {
                if(data_to_buf_evt_impe(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.impeAft, sizeof(OOP_DWORD3V_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;    

             case 0x33f20208:   //�¼�����ʱ��λ
            {
                if(basic_data_to_buf(E_ENUM, pOutBuf, tmpOutLen-offset, &offset, &event.curPhase, sizeof(uint8)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;  
                
            default: break;
        }

        tmpOffset = 0;
        for(j = 0; j < event.data.oadNum; j++)
        {
            if(tmpOAD.value == event.data.cols.oad[j].value)
            {
                //nRet = normal_data2fmt698(icInfo, &offset, &event.data.buf[tmpOffset], event.data.idLen[j], pOutBuf, tmpOutLen-offset);
                nRet = data_to_buf_evt_oad(tmpOAD, pOutBuf, tmpOutLen-offset, &offset, &event.data.buf[tmpOffset], event.data.idLen[j]);

                break;
            }

            tmpOffset += event.data.idLen[j];
        }        
   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}

/**********************************************************************
* @name      : start_topology_to_pdsManager
* @brief     ����̨��״̬����APP������������
* @param[in] ��
* @return    ��
* @Date      ��2020-3-18
* @Update    :
**********************************************************************/
int start_topology_to_pdsManager(uint16 time)
{
    uint8  sendbuf[100] = {0};
    uint16 offset = 0;

    sendbuf[offset++] = (uint8)((time >> 8) &0xff);
    sendbuf[offset++] = (uint8)(time & 0xff);

    return advance_send_pdsManager(0, PDS_IOP_TOPOLOGY_START, sendbuf, offset);
}

/**
*********************************************************************
* @name��      class11_invoke_act_topology
* @brief��     ��������
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ��OOP_DAR_E
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_act_topology(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (void *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class11_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class11_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x60F17F00:
        {
            uint16 data = 0;
            memcpy(&data, convert.dstBuf, dOffset);
            PRTL_FMT_LOGCC("class11_invoke_act_searchmeter_start : data = %d\n", data);

            ret = start_topology_to_pdsManager(data);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("start_topology_to_pdsManager err\n");
                return FALSE;
            }
            PRTL_FMT_DEBUG("node topology to taskManager ok\n");
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**********************************************************************
* @name      : get_power_state
* @brief     ����ȡ��Դ״̬
* @param[in] ��  
* @param[out]��state: ��Դ״̬��1�е磬0 ����
* @return    ��0 �ɹ��� <0 ʧ��
* @Create    : ����
* @Date      ��2020-07-27
* @Update    :
**********************************************************************/
int get_tmn_power_state(int *state)
{
    int ret = ERR_OK;
    
    POWER_DEVICE_T* dev = (POWER_DEVICE_T*) hal_device_get(HW_DEVICE_ID_POWER);
    if(NULL == dev)
    {
        PRTL_FMT_DEBUG("dev is null!\n");
        return ERR_PNULL;
    }

    ret |= dev->power_power_state_get(dev,POWER_MAIN,state);
    ret |= dev->power_power_state_get(dev,POWER_MAIN,state);
    ret |= dev->power_power_state_get(dev,POWER_MAIN,state);
    if(ret < 0)
    {
        PRTL_FMT_DEBUG("get power state failed, ret(%d).\n", ret);
        return ret;
    }

    
    PRTL_FMT_TRACE("get power state %d! \n", *state);
    
    hal_device_release((HW_DEVICE *)dev);
    
    return ret;
}

/**
*********************************************************************
* @brief��      ����Уʱģʽ
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class8_sensor_param(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_SENSOR_PARAM_T data;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //�ṹ����
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ա��
    if(srcBuf[sOffset++] != 2)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }  
    
    //�˿�
    dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.port, sizeof(data.port));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //��ַ
    dar = basic_buf_to_data(E_OOP_OCTETVAR16_T, srcBuf, srcLen, &sOffset, &data.addr, sizeof(data.addr));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }


    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}
/**
*********************************************************************
* @name��      class8_invoke_set_sensor_param
* @brief��     ���ô���������
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ��OOP_DAR_E
* @author    : 
* @Date      ��
*********************************************************************
*/
OOP_DAR_E class8_invoke_set_sensor_param(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;
    NOMAL_OAD_T normal = {0};
    int ret;
    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    normal.oad.value = pOadInfo->pTab->oad.value;

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60E10200:   //��ʪ�ȴ�����
        case 0x60E20200:   //��������
        case 0x60E30200:   //ˮ��������
        case 0x60E40200:   //��ѹ��׮ͷ�¶ȴ�����
        {
            OOP_SENSOR_PARAM_T data;
            memcpy(&data, convert.dstBuf, dOffset);
            normal.classtag = CLASS_DATA_NORMAL;
            PRTL_FMT_LOGCC("class8_invoke_set_sensor_param : port 0x%08x addrlen %d  \n", data.port.value,data.addr.nNum);
            PRTL_BUF_LOGCC(data.addr.value, data.addr.nNum, "addr: ");

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(data));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��      class8_invoke_set_oia_60
* @brief��     class8��SET����
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ��OOP_DAR_E
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E class8_invoke_set_oia_60(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
        case 0x60E1:
        case 0x60E2:
        case 0x60E3:
        case 0x60E4:
        {
            switch (pOadInfo->pTab->oad.attID)
            {
            case 0x02:
                {
                    return class8_invoke_set_sensor_param(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
                }
                break;

            default:
                break;
            }
        }
        break;
   
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    return DATA_OBJECT_UNAVAILABLE;
}
/**
*********************************************************************
* @name��      class8_invoke_get_time_precise
* @brief��     class8��GET����
* @param[in] ��pOdaInfo       OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ��OOP_DAR_E
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E class8_invoke_get_sensor_param(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;  

    MEMZERO(&convert, sizeof(convert)); 

    //��������
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60E10200:   //��ʪ�ȴ�����
        case 0x60E20200:   //��������
        case 0x60E30200:   //ˮ��������
        case 0x60E40200:   //��ѹ��׮ͷ�¶ȴ�����
        {
            OOP_SENSOR_PARAM_T data ;
            MEMZERO(&data, sizeof(data));
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            PRTL_FMT_LOGCC("class8_invoke_get_sensor_param  : port 0x%08x addrlen %d  \n", data.port.value,data.addr.nNum);
            PRTL_BUF_LOGCC(data.addr.value, data.addr.nNum, "addr: ");

            //ת���ɱ�������
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
    
    /* ���ݺͱ���ת�� */
    dar = data_to_buf_class8(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class8 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* ��д���Ŀ�ͷ����OAD�����ݽ��0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}
/**
*********************************************************************
* @name��      class8_invoke_get_oia_40
* @brief��     class8��GET����
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ��OOP_DAR_E
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E class8_invoke_get_oia_60(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
        case 0x60E1:
        case 0x60E2:
        case 0x60E3:
        case 0x60E4:
        {
            switch (pOadInfo->pTab->oad.attID)
            {
            case 0x02:
                {
                    return class8_invoke_get_sensor_param(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
                }
                break;    
            default:
                break;
            }
        }
        break;


    default:
        return DATA_OBJECT_UNAVAILABLE;
    }
    return DATA_OBJECT_UNAVAILABLE;
}
/**
*********************************************************************
* @brief��      ����������
* @param[in] �� srcBuf     Դ��������ͨ����һ��������ַ
*               srcLen     Դ���ݳ��ȣ�ͨ���Ǳ�����С
*               dstLen     Ŀ�����ݳ��ȣ�698���ĵ�buffer����

* @param[out] ��pOffset     ��������ݺ��Ŀ��������ƫ��
*               dstData    Ŀ����������698����buffer
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8_sensor_param(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_SENSOR_PARAM_T data ;
    MEMZERO(&data, sizeof(data));
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //�ṹ����
    dstBuf[dOffset++] = DT_STRUCTURE;

    //�ṹ��Ա��
    dstBuf[dOffset++] = 2;
            
    //�˿�
    dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.port, sizeof(data.port));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),\n", i, dar);
        return DATA_TYPE_UNMATCHED;
    }
    
    //��ַ
    dar = basic_data_to_buf(E_OOP_OCTETVAR16_T, dstBuf, dstLen, &dOffset, &data.addr, sizeof(data.addr));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d)\n", i, dar);
        return DATA_TYPE_UNMATCHED;
    }

    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}
#endif
/**
*********************************************************************
* @brief��      �ַ�������
* @param[in] �� src Դ�ַ���
*             delim �ָ���

* @param[out] ��dest     �ָ����ַ���
*             num   �ָ��ĸ���
* @return��   
*********************************************************************
*/
void str_split(char *src,const char *delim,char dest[][100],int *num)
{
    char* p = src;
    char* tmp;
    int i =0;
    tmp = strsep(&p,delim);
    while(tmp!=NULL)
    {
        strcpy(dest[i],tmp);
        tmp = strsep(&p,delim);
        i++;
    }
    *num = i;
}


//���ַ�����ʽ��ipת��Ϊ���� ʧ�ܷ���false
BOOL commfun_IP_unpack(char* ipin, uint8* ipout)
{
    if (ipin == NULL || strlen(ipin) == 0)
        return FALSE;
    char* p1[4];
    char czBuf[20];
    strcpy(czBuf, ipin);

    char* pTmp = p1[0] = czBuf;
    int nIndex = 1;
    while (*pTmp != '\0')
    {
        if (*pTmp == '.')
        {
            *pTmp = '\0';
            pTmp++;
            if (*pTmp == '\0')
            {
                break;
            }
            else
            {
                if (*pTmp > '9' || *pTmp < '0')
                    return FALSE;
                if (nIndex > 4)
                    return FALSE;
                p1[nIndex] = pTmp;
                nIndex++;
            }
        }
        else if (*pTmp > '9' || *pTmp < '0')
        {
            return FALSE;
        }
        pTmp++;
    }
    if (nIndex != 4)
    {
        return FALSE;
    }
    ipout[0] = atoi(p1[0]);
    ipout[1] = atoi(p1[1]);
    ipout[2] = atoi(p1[2]);
    ipout[3] = atoi(p1[3]);
    return TRUE;
}
/**
*********************************************************************
* @brief��      ���Ͻ����յ��Ķ��� 
��Զ��Ӳ����ʼ�����ܣ����ն��յ�ָ������������Զ�Ӳ����ʼ���� 
�����ʽ��cssdl 0  (�м��пո�) 
��TD-LTE / FDD-LTE/GPRSԶ����վ�л����ܣ����ն��յ�ָ����������󣬸���ԭ�ն˵�ͨѶ������������������վ�������վ�л��Ĺ��ܡ�
�����ʽ��cssdl 1 IP ����:�˿ڲ���:APN ���� 
���������cssdl 1 001.000.000.001:4000:cssdl.hn(�м��пո�) 
��CDMAԶ����վ�л����ܣ����ն��յ�ָ����������󣬸���ԭ�ն˵� CDMA ͨѶ������������������վ��
�����ʽ��cssdl 2 IP ����:�˿ڲ���:�������û��������������� 
���������cssdl 2 001.000.000.001:4000:cs@dl.vpdn.hn:hn123456(�м��пո�)

* @param[in] �� buf �յ��Ķ���
            buflen ����

* @param[out] ��
* @return��    =0            - �ɹ�
               <0        - ������
*********************************************************************
*/
int sms_msg_unpack(COMM_SMS_HANDLE_T *ptHandle,uint8 *buf,uint16 buflen)
{
    const char* smshead = "cssdl ";
    uint8 offset = 0;
    char str[50][100];
    int ret;
    int num = 0;
    memset(str,0x00,sizeof(str));
    uint8 ip[4] = {0};
    uint16 port;
    uint32 len;
    OOP_MASTERPARAMS_T masterparam;
    memset(&masterparam,0x00,sizeof(OOP_MASTERPARAMS_T));
    OOP_GPRSCONFIG_T gprsconfig;
    memset(&gprsconfig,0x00,sizeof(OOP_GPRSCONFIG_T));
    
    NOMAL_OAD_T NormalOad ;
    NormalOad.logicId = 0;
    NormalOad.infoNum = 0;
    NormalOad.classtag = CLASS_DATA_UNINIT;
    //���ٵ���cssdl 0  
    if(buflen<strlen(smshead)+1)
        return -1;
    if(strncmp(smshead,(const char*)buf,strlen(smshead))!=0)
    {
        COMM_FMT_DEBUG("hunan cssdl δʶ�� \n");
        return -1;
    }
    offset+=strlen(smshead);
    uint8 cmd = atoi((char*)&buf[offset]);
    switch(cmd)
    {
        //����
        case 0:
        {
            COMM_FMT_DEBUG("hunan tmn reboot ! \n");
            set_reset_flag(ptHandle->dbClient, TMN_REBOOT);
            return 0;
        }
            break;
        case 1:
        {
            offset+=2;
            str_split((char*)&buf[offset], ":",str, &num);
            if(num!=3)
            {
                COMM_FMT_DEBUG("param error !\n");
                return -1;
            }
            //ip
            if(FALSE == commfun_IP_unpack(str[0],ip))
            {
                COMM_FMT_DEBUG("IP error !\n");
                return -1;
            }
            //port
            port = atoi(str[1]);
            masterparam.nNum = 1;
            masterparam.master[0].port = port;
            memcpy(masterparam.master[0].ip,ip,4);
            NormalOad.oad.value = 0x45000300;
            ret = db_write_nomal(ptHandle->dbClient, &NormalOad,(uint8*)&masterparam,sizeof(OOP_MASTERPARAMS_T));
            if(ret!=0)
            {
                COMM_FMT_DEBUG("IP port write error %d \n",ret);
            }
            //APN
            NormalOad.oad.value = 0x45000200;
            ret = db_read_nomal(ptHandle->dbClient, &NormalOad,sizeof(OOP_GPRSCONFIG_T), (uint8*)&gprsconfig,&len);
            if(ret!=0 || len!=sizeof(OOP_GPRSCONFIG_T))
            {
                COMM_FMT_DEBUG("read gprsconfig error %d \n",ret);
            }
            gprsconfig.apn.apn.nNum = strlen(str[2]);
            memcpy(gprsconfig.apn.apn.value,str[2],strlen(str[2]));
            ret = db_write_nomal(ptHandle->dbClient, &NormalOad,(uint8*)&gprsconfig,sizeof(OOP_GPRSCONFIG_T));
            if(ret!=0)
            {
                COMM_FMT_DEBUG("gprsconfigwrite error %d \n",ret);
            }
            COMM_FMT_DEBUG("sms ip %d.%d.%d.%d port %d apn %s  \n",ip[0],ip[1],ip[2],ip[3],port,str[2]);
            return 0;
        }
            break;
        case 2:
        {
            offset+=2;
            str_split((char*)&buf[offset], ":",str, &num);
            if(num!=4)
            {
                COMM_FMT_DEBUG("param error !\n");
                return -1;
            }
            //ip
            if(FALSE == commfun_IP_unpack(str[0],ip))
            {
                COMM_FMT_DEBUG("IP error !\n");
                return -1;
            }
            //port
            port = atoi(str[1]);
            masterparam.nNum = 1;
            masterparam.master[0].port = port;
            memcpy(masterparam.master[0].ip,ip,4);
            NormalOad.oad.value = 0x45000300;
            ret = db_write_nomal(ptHandle->dbClient, &NormalOad,(uint8*)&masterparam,sizeof(OOP_MASTERPARAMS_T));
            if(ret!=0)
            {
                COMM_FMT_DEBUG("IP port write error %d \n",ret);
            }
            //�û�������
            NormalOad.oad.value = 0x45000200;
            ret = db_read_nomal(ptHandle->dbClient, &NormalOad,sizeof(OOP_GPRSCONFIG_T), (uint8*)&gprsconfig,&len);
            if(ret!=0 || len!=sizeof(OOP_GPRSCONFIG_T))
            {
                COMM_FMT_DEBUG("read gprsconfig error %d \n",ret);
            }
            gprsconfig.apn.username.nNum = strlen(str[2]);
            memcpy(gprsconfig.apn.username.value,str[2],strlen(str[2]));
            gprsconfig.apn.pwd.nNum = strlen(str[3]);
            memcpy(gprsconfig.apn.pwd.value,str[3],strlen(str[3]));
            ret = db_write_nomal(ptHandle->dbClient, &NormalOad,(uint8*)&gprsconfig,sizeof(OOP_GPRSCONFIG_T));
            if(ret!=0)
            {
                COMM_FMT_DEBUG("gprsconfigwrite error %d \n",ret);
            }
            COMM_FMT_DEBUG("sms ip %d.%d.%d.%d port %d apnuser %s pwd %s \n",ip[0],ip[1],ip[2],ip[3],port,str[2],str[3]);
            return 0;
        }
            break;
        default:
            break;
    }
    return -1;
}


#endif

