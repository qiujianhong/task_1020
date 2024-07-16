/*
*********************************************************************
* Copyright(C) 2019 China Electric Power Research Institute
* All rights reserved. 
* @brief��   DB�����������ݽṹ
* @date��    2019/12/30
* @history�� 
*********************************************************************
*/
#ifndef __DB_STRUCT_H_
#define __DB_STRUCT_H_

#ifdef __cplusplus
    extern "C" {
#endif

#define COMPENSATION_TIME       (8*60*60)    //����8Сʱ
#define DAY_TIME               (24*60*60)    //һ���ʱ��

#define TRANSPARENTTIME         (6*60*60)    //͸��ʱ�䵥����ʱ��

#define DATA_LEN_MAX             8192  //���������ݵ���󳤶�
#define RECORD_LEN_MAX           4096  //����¼����ʱ�ظ�����¼��
#define RECORD_HEAD_LEN_MAX      128   //����¼����ʱ�ظ�����¼ͷ����

/*ʱ��ѡ��*/
typedef enum _CLASS_DATA_E
{
    CLASS_DATA_UNINIT = 1,      //���ɳ�ʼ������
    CLASS_DATA_INIT   = 2,      //�ɳ�ʼ������
    CLASS_DATA_NORMAL = 3,      //��ͨ����
    CLASS_DATA_EVENT  = 4,      //�¼�����
    CLASS_DATA_COLLEC = 5,      //�ɼ�����
}DATA_CLASS_E;

/*��ͨ����OADˢѡ����*/
typedef struct _NOMAL_OAD_T
{
    uint8         logicId; //�߼���ַ
    uint16        infoNum; //��Ϣ��  ����Ϣ���Ϊ0xFFFFʱ����ʾȫ����Ϣ��
    OOP_OAD_U     oad;     //��ͨOAD
    uint16        classtag;//�������ʶ, ������û���õ�  DATA_CLASS_E
}NOMAL_OAD_T;

/* ��OAD֧��������� */
#define NORMAL_OADS_MAX   16
/*��ͨ���ݶ��OADˢѡ����*/
typedef struct _NOMAL_OADS_T
{
    uint8         logicId; //�߼���ַ
    uint16        infoNum; //��Ϣ�� 
    uint8         num;
    OOP_OAD_U     oad[NORMAL_OADS_MAX]; //��ͨOAD����
}NOMAL_OADS_T;

/*��ͨ����*/
typedef struct _NOMAL_OAD_DATA_T
{
    OOP_OAD_U     oad;     //�������ʶOAD
    uint16        classtag;//�������ʶ, DATA_CLASS_E
    uint16        datalen;
    uint8         data[DATA_LEN_MAX];
}NOMAL_OAD_DATE_T;
#define JS_OOP_MAX_OADS 45

/** @brief һ���������������(����)  */
typedef struct Object_Attribute_Descriptor_Group_MAX
{
    uint32      nNum;                   /**< �����������Ը���           */
    uint32      rsv;
    OOP_OAD_U   oad[JS_OOP_MAX_OADS];      /**< �������������б�           */
}OOP_OADS_MAX_T;

/** @brief ��¼�Ͷ�������������(����)   */
typedef struct Record_Object_Attribute_Descriptor_MAX
{
    OOP_OAD_U       oadMain;            /**< ��OOP_OAD_U           */
    OOP_OADS_MAX_T      oadCols;            /**< ������������         */
}OOP_ROAD_MAX_T;


/*��¼����OADˢѡ����*/
typedef struct _RECORD_T
{
    uint8         optional;//��OADѡ�� 1-��Ч,0-��Ч �ӿڿ���ͨ����OAD��0,ǿ����Ч */
    uint8         logicId; //�߼���ַ
    uint16        infoNum; //��Ϣ��  ����Ϣ���Ϊ0xFFFFʱ����ʾȫ����Ϣ��
    OOP_ROAD_MAX_T    road;    //ROAD
    uint16        classtag;//�������ʶ, ������û���õ�  DATA_CLASS_E
}RECORD_OAD_T;

/*ʱ��ѡ��*/
typedef enum _CHOOSE_TYPE_E
{
    COL_TM_START = 0,      //�ɼ���ʼʱ��
    COL_TM_END,            //�ɼ�����ʱ��
    COL_TM_STORE,          //�ɼ��洢ʱ��
    COL_NO_STORE,          //���ݴ洢���
    COL_NO_INFO,           //��Ϣ���
    COL_NOT      = 255,    //��ɸѡ
}CHOOSE_TYPE_E;

/*����ʽ*/
typedef enum _SORT_TYPE_E
{
    UP_SORT = 0,               //����
    DOWN_SORT,                 //����
    NO_SORT = 255,             //Ĭ�ϲ�����
}SORT_TYPE_E;

/*����¼������ˢѡ����*/
typedef struct _READ_RECORD_T
{
    RECORD_OAD_T     recordOAD;   //��¼��OAD
    CHOOSE_TYPE_E    cType;       //ʱ��ѡ��
    uint32           cStart;      //ʱ����ʼ��
    uint32           cEnd;        //ʱ�������
    SORT_TYPE_E      sortType;    //����ʽ
    OOP_OCTETVAR64_T MAC;         //����У��
    OOP_TI_T         ti;          //ʱ����

    uint32           bTable;      //�Ƿ��б��ַ
    OOP_OCTETVAR64_T table;       //���ַ
}READ_RECORD_T;

/* ���ص�698���ݸ�ʽ�Ƿ�������漸��*/
typedef struct _RCSD_CHOOSE_T
{
    uint8           isStartTm;   //698��ʽ���Ƿ���Ҫ��ʼʱ�䣬     1 ����Ҫ 0 ����Ҫ
    uint8           isEndTm;     //698��ʽ���Ƿ���Ҫ����ʱ�䣬     1 ����Ҫ 0 ����Ҫ
    uint8           isStoreTm;   //698��ʽ���Ƿ���Ҫ�洢ʱ�䣬     1 ����Ҫ 0 ����Ҫ
    uint8           isTsa;       //698��ʽ���Ƿ���Ҫ�ɼ�ͨ�ŵ�ַ�� 1 ����Ҫ 0 ����Ҫ
}RCSD_CHOOSE_T;

/*��������¼������*/
typedef struct _STEP_READ_RECORD_T
{
    //uint32      handle;       //���
    OOP_OCTETVAR64_T MAC;   //����У��  nNUMΪ0��ʾû�и���У��
    //uint32          len;      //���ݻ��峤��
}STEP_READ_RECORD_T;

/*��������¼�����ݷ��ص����ݵ�Ԫ*/
typedef struct _RECORD_DATA_T
{
    uint32         optional;
    OOP_ROAD_T     road;        //OAD
    uint32         outDatalen;  //���ݳ���
    uint8          outDataBuf[DATA_LEN_MAX];  //�������� 
}RECORD_DATA_T;

#define RECORD_DATA_MAX_NUM   64

/*��������¼�����ݷ��ص����ݵ�Ԫ*/
typedef struct _RECORD_UNIT_T
{
    uint32         colStartTm;  //�ɼ���ʼʱ��
    uint32         colEndTm;    //�ɼ�����ʱ��
    uint32         colStoreTm;  //�ɼ��洢ʱ��
    uint16         infoNum;     //��Ϣ���
    uint16         recordnum;   //��������
    RECORD_DATA_T  record[RECORD_DATA_MAX_NUM];//����
}RECORD_UNIT_T;

/*д��¼������*/
typedef struct _WRITE_RECORD_T
{
    RECORD_OAD_T     recordOAD; 
    uint32           colStartTm;  //�ɼ���ʼʱ��
    uint32           colEndTm;    //�ɼ�����ʱ��
    uint32           colStoreTm;  //�ɼ��洢ʱ��
    OOP_OCTETVAR64_T MAC;         //����У��
    uint32           inDatalen;   //���ݳ���
    uint8            inDataBuf[DATA_LEN_MAX];  //�������� 
}WRITE_RECORD_T;


typedef struct _WRITE_RECORD_DATA_T
{
    uint32           len;                 //���ݳ���
    uint8            data[DATA_LEN_MAX];  //�������� 
}WRITE_RECORD_DATA_T;

/*д��¼������*/
typedef struct _WRITE_RECORDS_T
{
    RECORD_OAD_T        recordOAD; 
    uint32              colStartTm;  //�ɼ���ʼʱ��
    uint32              colEndTm;    //�ɼ�����ʱ��
    uint32              colStoreTm;  //�ɼ��洢ʱ��
    OOP_OCTETVAR64_T    MAC;         //����У��
    WRITE_RECORD_DATA_T datas[JS_OOP_MAX_OADS]; //���OAD˳�򱣳�һ��
}WRITE_RECORDS_T;


/*��������ʱɸѡ��ʽ*/
typedef struct
{
    uint8            choice;      //���ͱ�ʶ
    union
    {
        uint32       storeTm;     //�ɼ��洢ʱ��
        uint32       storeNo;     //�洢���
    };
}FILTER_E;

/*���¼�¼������*/
typedef struct _UPDATE_RECORD_T
{
    RECORD_OAD_T     recordOAD; 
    FILTER_E         filter;      //ˢѡ��ʽ
    OOP_OCTETVAR64_T MAC;         //����У��
    uint32           inDatalen;   //���ݳ���
    uint8            inDataBuf[DATA_LEN_MAX];  //�������� 
}UPDATE_RECORD_T;

typedef enum _GET_TYPE_E
{
    GET_ALL_RECORD = 0,      //���м�¼
    GET_INTER_RECORD,        //�洢ʱ������
}GET_TYPE_E;

/*��ȡ��¼����*/
typedef struct _GET_RECORD_NUM_T
{
    RECORD_OAD_T    recordOAD; 
    GET_TYPE_E      filter;   //ˢѡ��ʽ
    uint32          gStart;   //��ʼ����
    uint32          gEnd;     //��������
}GET_RECORD_NUM_T;

/*��ȡ��¼����*/
typedef struct _CLEAR_DB_DATA_T
{
    RECORD_OAD_T    recordOAD; 
    uint8           bClassTag;   //�������ʶ TRUE��ʾû���������ʶ ������ǰ��
    uint8           bFilter;
    uint8           filterType;  //ɸѡ��ʽ��0- �洢ʱ�� 255 ��ɸѡ
    uint8           res[1];
    uint32          startFilter;
    uint32          endFilter;
}CLEAR_DATA_T;

#ifdef __cplusplus
}
#endif

#endif

