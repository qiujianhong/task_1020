/*
*********************************************************************
* @filename: msgSecApp.h
* @brief   : ��ȫAPP��Ϣ����ͨ��
* @describe: 
* @history : 
*********************************************************************
*/

#ifndef _MSG_SECAPP_H_
#define _MSG_SECAPP_H_

#define SMISSAL_FILEPATH_LEN   256
#define SMISSAL_MSG_MAX_LEN    8192

#define SMISSAL_MSG_PATH       "/tmp/dev/"
#define SMISSAL_APP_DEST       "/tmp/dev/smiSsal"

typedef enum 
{
    SEC_DATA_698 = 1,  //698����
    SEC_DATA_SSAL,     //SSAL����
    SEC_DATA_TAG = 4,  //��ȫ����APP��ʶ��Ϣ
    SEC_DATA_SUMMARY,  //��ȡӦ�õ�Ԫ���ժҪ��ǩ����Ϣ
    SEC_DATA_REG,      //��֤Ӧ�����ע����Ϸ���
    SEC_DATA_SIGN,     //��֤Ӧ�����ǩ���Ϸ���
    SEC_DATA_OTHER = 255,
}SEC_DATA_TPYE_E;
//��ȫ�ӹ̱��ķ�װ
typedef struct tagSEC_DATA
{
    uint8  dataType;  
    uint8  cryptType;

    uint32 appType;
    uint8  time[7];

    uint32 devAddrLen;
    uint8  devAddr[16];
    uint8  dstIp[4];
    uint32 policy;

    uint16 lenData;
    uint8  *data;

    uint16 CRC;
}SEC_DATA_T;

typedef struct tag_SOFT_SUMMARY_REQ
{
    uint8               cpuNo;          /**< cpu���      */
    OOP_VISIBLEVAR256_T name;           /**< ������      */
}SOFT_SUMMARY_REQ;

typedef struct tag_OOP_SOFT_SUMMARYS_REQ
{
    uint8            nNum;
    SOFT_SUMMARY_REQ item[16];
}OOP_SOFT_SUMMARYS_REQ;

typedef struct tag_SOFT_SUMMARY_RES
{
    uint8               cpuNo;          /**< cpu���      */
    OOP_VISIBLEVAR256_T name;           /**< ������      */
    OOP_VISIBLEVAR1K_T  data;           /**< ����      */
}SOFT_SUMMARY_RES;

typedef struct tag_OOP_SOFT_SUMMARYS_RES
{
    uint8            nNum;
    SOFT_SUMMARY_RES item[16];
}OOP_SOFT_SUMMARYS_RES;

int sec_smiSsal_fd_init(char *appName);
int sec_info_management(uint8 *send, uint16 sndLen, uint8 *recv, uint16 *rcvLen);
int sec_get_soft_summary(uint8 *addr, uint8 addrLen, OOP_SOFT_SUMMARYS_REQ *req, OOP_SOFT_SUMMARYS_RES *res);
int sec_get_app_tag(uint8 *addr, uint8 addrLen, OOP_OCTETVAR1K_T *tag);
int sec_ssal_en_decryption(SEC_DATA_T *sndData, SEC_DATA_T *rcvData);

#endif
