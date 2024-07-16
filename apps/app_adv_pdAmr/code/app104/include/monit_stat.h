#ifndef __MONIT_STAT_H__
#define __MONIT_STAT_H__

/** @brief Խ���ж���ֵ */
#define MT_VOLT_UP_LIMIT (2354) //��ѹ����Խ��ֵ 1.07*220������10��
#define MT_VOLT_UP_RECOV (2354) //��ѹ���޻ָ�ֵ 1.07*220�����޻ָ�ֵһ��ҪС�����ޣ�������10��

#define MT_VOLT_DW_LIMIT (2046) //��ѹ����Խ��ֵ 0.93*220������10��
#define MT_VOLT_DW_RECOV (2046) //��ѹ���޻ָ�ֵ 0.93*220�����޻ָ�ֵһ��Ҫ�������ޣ�������10��

#define MT_VOLT_DIST_LIMIT (500) //��ѹ������Խ��ֵ%���ٷ�������100������ֵ����10000��
#define MT_VOLT_DIST_RECOV (470) //��ѹ�����ʻָ�ֵ%���ٷ�������100������ֵ����10000��

#define MT_VOLT_UNB_LIMIT (4) //��ѹ��ƽ���Խ��ֵ%���ٷ�������100������ֵ����10000��
#define MT_VOLT_UNB_RECOV (3.8) //��ѹ��ƽ��Ȼָ�ֵ%���ٷ�������100������ֵ����10000��

#define MT_CURR_UNB_LIMIT (30) //������ƽ���Խ��ֵ%���ٷ�������100������ֵ����10000��
#define MT_CURR_UNB_RECOV (25) //������ƽ��Ȼָ�ֵ%���ٷ�������100������ֵ����10000��

#define MT_PWRF_UP_LIMIT (5100) //����Ƶ������Խ��ֵ������100��
#define MT_PWRF_UP_RECOV (5100) //����Ƶ�����޻ָ�ֵ������100��

#define MT_PWRF_DW_LIMIT (4900) //����Ƶ������Խ��ֵ������100��
#define MT_PWRF_DW_RECOV (4900) //����Ƶ�����޻ָ�ֵ������100��

#define MT_FWR_FAC1_LIMIT (930) //��������1�Σ�����1000��
#define MT_FWR_FAC2_LIMIT (980) //��������2�Σ�����1000��

#define MT_INVALID_LIMIT (0xffffffff) //��Ч����ֵ

#define MT_ATTR_INDEX(nIndex) ((nIndex == 0)?nIndex:(nIndex-1))

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
    MT_REGION_MIN  = 0x2100,
    MT_REGION_HOUR = 0x2101,
    MT_REGION_DAY  = 0x2102,
    MT_REGION_MON  = 0x2103,
    MT_REGION_YEAR = 0x2104
}MT_REGION_OI_E;

/** @brief �ۼ�ƽ��ͳ��OIö��   */
typedef enum tag_MT_TOTALAVG_OI
{
    MT_TOTALAVG_MIN  = 0x2110,
    MT_TOTALAVG_HOUR = 0x2111,
    MT_TOTALAVG_DAY  = 0x2112,
    MT_TOTALAVG_MON  = 0x2113,
    MT_TOTALAVG_YEAR = 0x2114
}MT_TOTALAVG_OI_E;

/** @brief ��ֵͳ��OIö��   */
typedef enum tag_MT_EXTREME_OI
{
    MT_EXTREME_MIN  = 0x2120,
    MT_EXTREME_HOUR = 0x2121,
    MT_EXTREME_DAY  = 0x2122,
    MT_EXTREME_MON  = 0x2123,
    MT_EXTREME_YEAR = 0x2124
}MT_EXTREME_OI_E;

/** @brief �ϸ���ͳ��OIö��   */
typedef enum tag_MT_QR_OI
{
    MT_QR_DAY  = 0x2192,
    MT_QR_MON  = 0x2193,
}MT_QR_OI_E;

/**@briefͣ����OIö��*/
typedef enum tag_MT_POWERCUT_OI
{
    MT_POWERCUT_DAY  = 0x2194,
    MT_POWERCUT_MON  = 0x2195,
}MT_POWERCUT_OI_E;

typedef int (*MT_ADAPT)(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit);

/** @brief ��ֵͳ�ƶ��� */
typedef struct tag_MT_EXTREME_OBJ
{
    OOP_OAD_U       mainOAD;          /* ��OAD(�ݴ˾���period�ĵ�λ) */
    OOP_OAD_U       subOAD;           /* ��OAD(ͳ�ƶ���)*/
    uint32          periCnt;          /* ���ڼ���(������������)*/
    uint32          freqCnt;          /* �������(�����ļ����)*/      
    uint32          period;           /* ͳ�������� */
    OOP_TI_T        freq;             /* �������� */
    MT_ADAPT        mtAdapt;          /* ���ݴ�����*/
    OOP_DIGIT_T    *srcData;          /* Դ���ݴ洢*/
    uint8           nIndex;           /* �����ݵĵ�ǰ����*/
}MT_EXTREME_OBJ_T;

/** @brief �ϸ���ͳ�ƶ��� */
typedef struct tag_MT_QR_OBJ
{
    OOP_OAD_U       mainOAD;          /* ��OAD(�ݴ˾���period�ĵ�λ) */
    OOP_OAD_U       subOAD;           /* ��OAD(ͳ�ƶ���)*/
    uint32          periCnt;          /* ���ڼ���(������������)*/
    uint32          freqCnt;          /* �������(�����ļ����)*/      
    uint32          period;           /* ͳ�������� */
    OOP_TI_T        freq;             /* �������� */
    uint32          upLimit;          /* ����Խ��ֵ*/    
    uint32          upRecov;          /* ���޻ָ�ֵ*/
    uint32          dwLimit;          /* ����Խ��ֵ*/
    uint32          dwRecov;          /* ���޻ָ�ֵ*/
    MT_ADAPT        mtAdapt;          /* ���ݴ�����*/
    OOP_DIGIT_T    *srcData;          /* Դ����ָ��*/
    uint8           nIndex;           /* �����ݵĵ�ǰ����*/
}MT_QR_OBJ_T;

/** @brief �ϸ�״̬ö��   */
typedef enum tag_MT_QSTATE
{
    MT_QSTATE_Q  = 0, //�ϸ�   
    MT_QSTATE_U  = 1, //Խ����
    MT_QSTATE_D  = 2  //Խ����
}MT_QSTATE_E;

/** @brief �ϸ���ͳ�ƽ�� */
typedef struct tag_MT_QR_RES
{
    uint32       mtTime;  //���ʱ�� 
    uint32       qrTime;  //�ϸ�ʱ�� 
    uint32       upTime;  //������ʱ��
    uint32       dwTime;  //������ʱ��  
    uint16       qrRate;  //�ϸ���
    uint16       upRate;  //��������
    uint16       dwRate;  //��������
    float64      mtTotal; //�ۼӺ�
    float64      mtAvg;   //ƽ��ֵ
    MT_QSTATE_E  mtState; //��ǰ״̬
}MT_QR_RES_T;

/** @brief ����ͳ�ƶ��� */
typedef struct tag_MT_REGION_OBJ
{
    OOP_OAD_U       mainOAD;              /* ��OAD(�ݴ˾���period�ĵ�λ) */
    OOP_OAD_U       subOAD;               /* ��OAD(ͳ�ƶ���)*/
    uint32          periCnt;              /* ���ڼ���(������������)*/
    uint32          freqCnt;              /* �������(�����ļ����)*/      
    uint32          period;               /* ͳ�������� */
    OOP_TI_T        freq;                 /* �������� */
    uint32          nNum;                 /* ��ֵ���� */
    OOP_DIGIT_T     parm[OOP_MAX_REGION]; /* ���䶨ֵ */   
    MT_ADAPT        mtAdapt;              /* ���ݴ�����*/
    OOP_DIGIT_T    *srcData;              /* Դ����ָ��*/
    uint8           nIndex;               /* �����ݵĵ�ǰ����*/
}MT_REGION_OBJ_T;

/** @brief ������� */
typedef struct tag_MT_FROZEN_OBJ
{
    OOP_OAD_U       mainOAD;          /* ��OAD(�ݴ˾���period�ĵ�λ) */
    OOP_OAD_U       subOAD;           /* ��OAD(ͳ�ƶ���)*/
    uint32          periCnt;          /* ���ڼ���(������������)*/
    uint32          period;           /* ͳ�������� */
}MT_FROZEN_OBJ_T;

/** @brief ͣ������� */
typedef struct tag_MT_POWERCUT_OBJ
{
    OOP_OAD_U       mainOAD;
    uint32          periCnt;
    uint32          freqCnt;
    uint32          period;
    OOP_TI_T        freq;
}MT_POWERCUT_OBJ_T;

/** @brief ͣ����ͳ��*/
typedef struct tag_MT_POWERCUT_RES
{
    OOP_OAD_U       obj;
    BOOLEAN         pwctFLag;
    uint32          countTime;
    time_t          tmstart;
    time_t          tmstop;    
}MT_POWERCUT_RES_T;


int mt_volt_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit);
int mt_curr_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit);
int mt_zerocurr_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit);
int mt_pwrfreq_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit);
int mt_imbalance_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit);
int mt_distortion_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit);
int mt_pwrfact_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit);

#endif
