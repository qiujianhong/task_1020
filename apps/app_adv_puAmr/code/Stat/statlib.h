/*
*********************************************************************
* @file    AutoTask.c
* @brief�� ͳ��ģ��ʵ���ļ�
* @author������
* @date��   2019.11.11
* @copyright 2019 �Ͼ��������ӹɷ����޹�˾ All rights reserved.
*********************************************************************
*/

#ifndef __STAT_LIB_H__
#define __STAT_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

/*--���������Ͷ���--*/
#define MTR_485       1   /*-485��-*/
#define MTR_SIM       2   /*-ģ���-*/
#define MTR_PULSE     3   /*-����-*/
#define MTR_VIR       4   /*-����-*/
#define MTR_AC        5   /*-����-*/
#define MTR_PLC       6   /*-�ز�-*/
#define MTR_PLCMUI    7   /*-�ز��๦��-*/

#define UPUP_EN          (1)       //��ѹ�����������ڹ���Խ�����ޣ������ޣ�ʱ���Ƿ���ΪԽ���ޣ����ޣ�ʱ��
#define SMALL_EN         (1)       //�ڵ����Ƚ�С��ʱ���Ƿ�Ӧ�ý��õ������������ 1 ����  0 ����
#define TOTAL_EN_CHECK   (0)       //�������ݽ���У�飬�鿴�ǲ��ǵ��ڷַ������

typedef enum tag_STA_CALC_INTERVAL
{
    STA_CALC_DAY = 0,
    STA_CALC_MON = 1,
    STA_CALC_MAX
}STA_CALC_INTERVAL_E;

typedef struct tag_STA_VOLT_ACCTIME_CFG
{
    OOP_WORD3V_T phaseVolt;     //�����ѹ
    uint8 powerType;             //���߷�ʽ,�����Ϊ 1���������߱�Ϊ 2���������߱�Ϊ 3
    OOP_VOLTPARAM_T voltParam;   //��ѹ�ϸ��ʲ���
    uint16 addTime;              //���˴�ͳ��ʱ�ۻ�ʱ��
    STA_CALC_INTERVAL_E interval;//ͳ�Ƽ��
}STA_VOLT_ACCTIME_CFG_T;

typedef struct tag_STA_MTR_INFO
{
    uint8     type;       /**< MTR_AC... */
    uint16    nIndex;     /**< 485����������   */
    OOP_TSA_T tsa;        /**< 485��ĵ�ַ�����ڼ���MAC   */
    OOP_OAD_U pulseOAD;   /**< ������OAD    */    
    uint8     pwrType;    /**< ���߷�ʽ... */
}STA_MTR_INFO_T;

//˽���ļ��洢�ṹ
/*
        ��������������������������������������������������
        ���ϴ�ʵʱ������160�ֽڣ�          ��
        ��������������������������������������������������
        ���ϴη��ӵ�����160�ֽڣ�          ��
        ��������������������������������������������������
        �����ܼӵ�����80�ֽڣ�            ��
        ��������������������������������������������������
        �����ܼӵ�����80�ֽڣ�            ��
        ��������������������������������������������������
        �����ܼӵ�����80�ֽڣ�            ��
        ��������������������������������������������������
*/

//ÿ�����ӵ�����Ԫ��С���ܣ��⣬�壬ƽ���� 5*8 sizeof(uint64)��
#define STA_MIN_ENERGY_UNIT_SIZE 40 

//һ��SEG���������й��������й��������޹��������޹�4����Ԫ
#define STA_MIN_ENERGY_SEG_SIZE  4*STA_MIN_ENERGY_UNIT_SIZE

//���ӵ���������
#define STA_MIN_ENERGY_BASE_SEG  0

//���ӵ������ݶ�
#define STA_MIN_ENERGY_DATA_SEG  STA_MIN_ENERGY_BASE_SEG+STA_MIN_ENERGY_SEG_SIZE 

//���ڸ���Ԫƫ����
#define STA_POS_ACT_ENERGY_OFFSET  0*STA_MIN_ENERGY_UNIT_SIZE  /**< �����й����� */
#define STA_NEG_ACT_ENERGY_OFFSET  1*STA_MIN_ENERGY_UNIT_SIZE  /**< �����й����� */        
#define STA_POS_RCT_ENERGY_OFFSET  2*STA_MIN_ENERGY_UNIT_SIZE  /**< �����޹����� */   
#define STA_NEG_RCT_ENERGY_OFFSET  3*STA_MIN_ENERGY_UNIT_SIZE  /**< �����޹����� */ 

//�¼�У��ӿ�
//ÿ�����ӵ�����Ԫ��С���ܣ��⣬�壬ƽ���� 5*8 sizeof(uint64)+1�ֽ�У�飩
#define STA_MINCS_ENERGY_UNIT_SIZE 41 

//һ��SEG���������й��������й��������޹��������޹�4����Ԫ
#define STA_MINCS_ENERGY_SEG_SIZE  4*STA_MINCS_ENERGY_UNIT_SIZE

//���ӵ���������
#define STA_MINCS_ENERGY_BASE_SEG  0

//���ӵ������ݶ�
#define STA_MINCS_ENERGY_DATA_SEG  STA_MINCS_ENERGY_BASE_SEG+STA_MINCS_ENERGY_SEG_SIZE 

//���ڸ���Ԫƫ����
#define STA_POS_ACTCS_ENERGY_OFFSET  0*STA_MINCS_ENERGY_UNIT_SIZE  /**< �����й����� */
#define STA_NEG_ACTCS_ENERGY_OFFSET  1*STA_MINCS_ENERGY_UNIT_SIZE  /**< �����й����� */        
#define STA_POS_RCTCS_ENERGY_OFFSET  2*STA_MINCS_ENERGY_UNIT_SIZE  /**< �����޹����� */   
#define STA_NEG_RCTCS_ENERGY_OFFSET  3*STA_MINCS_ENERGY_UNIT_SIZE  /**< �����޹����� */ 

//���ܼӵ������ݶ�
#define STA_TGMINCS_ENERGY_DATA_SEG  STA_MINCS_ENERGY_DATA_SEG+STA_MINCS_ENERGY_SEG_SIZE

#define STA_ACTCS_ENERGY_OFFSET  0*STA_MINCS_ENERGY_UNIT_SIZE  /**< �й����� */
#define STA_RCTCS_ENERGY_OFFSET  1*STA_MINCS_ENERGY_UNIT_SIZE  /**< �޹����� */

#define STA_SLIPPERIOD_MAX (255)  //��󻬲����ڣ���Ϊ����������8λ�޷�������

/** @brief ����ʻ��� */
typedef struct tag_STA_SLIPPOWER_FIFO
{
    uint32 nCount;                       //��Ա���� 
    int64 slipAct[STA_SLIPPERIOD_MAX+1]; //�й����ʶ��� 
    int64 slipRct[STA_SLIPPERIOD_MAX+1]; //�޹����ʶ���    
}STA_SLIPPOWER_FIFO_T;

/** @brief ����ɾ��*/
typedef struct tag_STA_TGDEL
{
    pthread_mutex_t flock;                      //�� ���̷߳���
    uint8           delVaild;                   //��Ч��ʶ
}STA_TGDEL_T;

/** @brief ��Ȩ״̬   */
typedef enum tag_STA_PORT_AUTHORIZE
{
    STA_PORT_AUTH_PERMANENT = 0,   //������Ȩ
    STA_PORT_AUTH_PROVISIONAL = 1, //��ʱ��Ȩ
    STA_PORT_AUTH_FORBIDDEN = 2,   //�ر���Ȩ
}STA_PORT_AUTHORIZE_E;

typedef struct tag_STA_PORT_AUTHORIZE_INFO
{
    STA_PORT_AUTHORIZE_E status; //��Ȩ״̬
    BOOLEAN isAuthChg;   //��Ȩ״̬�仯           1���� 0����
    BOOLEAN isAuthedcmd; //��ȫ��֤�������� 1���� 0����
    BOOLEAN isSuccLocal; //���ذ�ȫ��֤ͨ��         1���� 0����
    uint8 authTime;      //��Ȩʱ��
}STA_PORT_AUTHORIZE_INFO;

#define AUTH_STATUS_GET(time) ((time==255)?STA_PORT_AUTH_PERMANENT:(time==0)?STA_PORT_AUTH_FORBIDDEN:STA_PORT_AUTH_PROVISIONAL)

#define abs_int64(vari)  ((vari>=(int64)0) ? (vari):-(vari))  /*-int64�����ݵľ���ֵ-*/

/** @brief �������OIö��   */
typedef enum tag_MT_FROZEN_OI
{
    MT_FROZEN_TRAN = 0x5000,
    MT_FROZEN_SEC = 0x5001,
    MT_FROZEN_MIN = 0x5002,
    MT_FROZEN_HOUR = 0x5003,
    MT_FROZEN_DAY = 0x5004,
    MT_FROZEN_SETTLING_DAY = 0x5005,
    MT_FROZEN_MON = 0x5006,
    MT_FROZEN_YEAR = 0x5007
}MT_FROZEN_OI_E;

/** @brief ����ͳ��OIö��   */
typedef enum tag_MT_REGION_OI
{
    MT_REGION_DAY  = 0x219A,
    MT_REGION_MON  = 0x219B,
}MT_REGION_OI_E;

/** @brief �ۼ�ƽ��ͳ��OIö��   */
typedef enum tag_MT_TOTALAVG_OI
{
    MT_TOTALAVG_DAY  = 0x2198,
    MT_TOTALAVG_MON  = 0x2199,
}MT_TOTALAVG_OI_E;

/** @brief ��ֵͳ��OIö��   */
typedef enum tag_MT_EXTREME_OI
{
    MT_EXTREME_DAY  = 0x2196,
    MT_EXTREME_MON  = 0x2197,
}MT_EXTREME_OI_E;

/** @brief �ϸ���ͳ��OIö��   */
typedef enum tag_MT_QR_OI
{
    MT_QR_DAY  = 0x2192,
    MT_QR_MON  = 0x2193,
}MT_QR_OI_E;

/**@briefԽ��ͳ��OIö��*/
typedef enum tag_MT_POWERCUT_OI
{
    MT_POWERCUT_DAY  = 0x2194,
    MT_POWERCUT_MON  = 0x2195,
}MT_POWERCUT_OI_E;

/** @brief �ϸ�״̬ö��   */
typedef enum tag_STA_QSTATE
{
    MT_QSTATE_QR  = 0, //�ϸ�   
    MT_QSTATE_UP  = 1, //Խ����
    MT_QSTATE_DW  = 2  //Խ����
}STA_QSTATE_E;

/** @brief �ϸ���ͳ�ƽ�� */
typedef struct tag_STA_QR_RES
{
    uint32       mtTime;  //���ʱ�� 
    uint32       qrTime;  //�ϸ�ʱ�� 
    uint32       upTime;  //������ʱ��
    uint32       dwTime;  //������ʱ��
    uint32       upupTime;//��������ʱ��
    uint32       dwdwTime;//��������ʱ��
    uint32       tmpupupTime;//��ʱ��������ʱ��
    uint32       tmpdwdwTime;//��ʱ��������ʱ��  
    uint32       tmpupTime;//��ʱ������ʱ��
    uint32       tmpdwTime;//��ʱ������ʱ��  
    uint16       qrRate;  //�ϸ���
    uint16       upRate;  //��������
    uint16       dwRate;  //��������
    STA_QSTATE_E mtState; //��ǰ״̬
}OOP_QR_RESULT_T;

/** @brief �ϸ���ͳ�Ʋ��� */
typedef struct tag_STA_QR_PARA
{
    uint32          upupLimit;        /* ������Խ��ֵ*/
    uint32          upLimit;          /* ����Խ��ֵ*/   
    uint32          upupcontime;      /* Խ�����޳���ʱ��*/
    uint32          upcontime;        /* Խ���޳���ʱ��*/
    uint32          upRecov;          /* ���޻ָ�ֵ*/
    uint32          dwRecov;          /* ���޻ָ�ֵ*/
    uint32          dwLimit;          /* ����Խ��ֵ*/
    uint32          dwdwLimit;        /* ������Խ��ֵ*/    
    uint32          dwcontime;        /* Խ���޳���ʱ��*/
    uint32          dwdwcontime;      /* Խ�����޳���ʱ��*/
    uint32          spantime;         /* ��������ʱ��*/
}STA_QR_PARA_T;

typedef int (*STA_CALC_ADAPT)(DB_CLIENT clientid, STA_MTR_INFO_T *mtrInfo, void *object);
typedef int (*STA_DATA_ADAPT)(DB_CLIENT clientid, STA_MTR_INFO_T *mtrInfo, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit);

/** @brief �ϸ���ͳ�ƶ��� */
typedef struct tag_STA_QR_OBJ
{
    OOP_OAD_U       dstOAD;          /* ��OAD(�ݴ˾���period�ĵ�λ) */
    OOP_OAD_U       srcOAD;           /* ��OAD(ͳ�ƶ���)*/
    uint32          periCnt;          /* ���ڼ���(������������)*/
    uint32          freqCnt;          /* �������(�����ļ����)*/      
    OOP_TI_T        period;           /* ͳ�������� */
    OOP_TI_T        freq;             /* �������� */
    STA_QR_PARA_T   param;            /* ͳ�Ʋ��� */    
    OOP_DIGIT_T    *srcData;          /* Դ����ָ��*/
    uint32          srcUnits;         /* Դ���ݵ�Ԫ��*/
    STA_CALC_ADAPT  paraAdapt;        /* �������亯��*/
    STA_DATA_ADAPT  dataAdapt;        /* �������亯��*/
    STA_CALC_ADAPT  calcAdapt;        /* �������亯��*/    
    uint32          srcValid;         /* Դ������Ч��*/ 
}STA_QR_OBJ_T;

void sta_qr_entry(DB_CLIENT clientid);
int sta_qr_comm_calc(DB_CLIENT clientid, STA_MTR_INFO_T *mtrInfo, void *pObject);

void sta_volt_qualifyrate_calc(void);
void sta_curr_imbalance_calc(void);
void sta_volt_imbalance_calc(void);
void sta_mpoint_entry(void);
void sta_pulse_energy_calc(void);
void sta_tgsets_data_calc(void);

void sta_tmn_power_acctime(void);
void sta_tmn_reset_acctimes(void);
void sta_tmn_comm_traffic(void);
void sta_tmn_port_authorize_tmcnt(void);
void sta_tmn_port_authorize_status(uint8 tmAuth, STA_PORT_AUTHORIZE_INFO *status);
void sta_tmn_port_authorize_operate(void);
int sta_ssh_service_authorize_operate(STA_PORT_AUTHORIZE_E status);
void sta_past_data_clear(void);
void sta_tg_data_clear(DB_CLIENT fd, OOP_OAD_U tgOAD);

void sta_tg_min_elec_init();
int sta_tg_min_acte_read(OI tgOI, int64 *acteTotal, uint32 *storeNo);

extern int appmain_send_msg(uint32 msgid, uint16 label, uint8 *pMsg, uint16 msglen);
int sta_record_read_new(DB_CLIENT clientid, RECORD_OAD_T *recordOAD, uint32 *storeTm, void *outData, uint32 outLen);


#ifdef __cplusplus
}
#endif

#endif  /* __STAT_LIB_H__ */

